#ifndef _H_ARCH_SPINLOCK_H_
#define _H_ARCH_SPINLOCK_H_

typedef struct {
	volatile unsigned int lock;
} spinlock_t;

#define __SPIN_LOCK_UNLOCKED  	{ 0 }

typedef struct {
	volatile unsigned int lock;
} rwlock_t;

#define __RW_LOCK_UNLOCKED		  { 0 }

#define smp_mb()     __asm__ __volatile__ ("dsb" : : : "memory")

static inline void dsb_sev(void)
{
	__asm__ __volatile__ (
		"dsb\n"
		"sev\n"
	);
}

/*
 * ARMv6 Spin-locking.
 *
 * We exclusively read the old value.  If it is zero, we may have
 * won the lock, so we try exclusively storing it.  A memory barrier
 * is required after we get a lock, and before we release it, because
 * V6 CPUs are assumed to have weakly ordered memory.
 *
 * Unlocked value: 0
 * Locked value: 1
 */

#define arch_spin_is_locked(x)		((x)->lock != 0)
#define spin_unlock_wait(lock) \
	do { while (arch_spin_is_locked(lock)) cpu_relax(); } while (0)

#define spin_lock_flags(lock, flags) spin_lock(lock)

static inline void spin_lock(spinlock_t *lock)
{
	unsigned long tmp;

	__asm__ __volatile__(
		"1:	ldrex	%0, [%1]\n"
		"	teq	%0, #0\n"
		"wfene\n"
		"	strexeq	%0, %2, [%1]\n"
		"	teqeq	%0, #0\n"
		"	bne	1b"
			: "=&r" (tmp)
			: "r" (&lock->lock), "r" (1)
			: "cc");

	smp_mb();
}

static inline int spin_trylock(spinlock_t *lock)
{
	unsigned long tmp;

	__asm__ __volatile__(
		"	ldrex	%0, [%1]\n"
		"	teq	%0, #0\n"
		"	strexeq	%0, %2, [%1]"
			: "=&r" (tmp)
			: "r" (&lock->lock), "r" (1)
			: "cc");

	if (tmp == 0) {
		smp_mb();
		return 1;
	} else {
		return 0;
	}
}

static inline void spin_unlock(spinlock_t *lock)
{
	smp_mb();

	__asm__ __volatile__(
		"	str	%1, [%0]\n"
			:
			: "r" (&lock->lock), "r" (0)
			: "cc");

	dsb_sev();
}

/*
 * RWLOCKS
 *
 *
 * Write locks are easy - we just set bit 31.  When unlocking, we can
 * just write zero since the lock is exclusively held.
 */

static inline void write_lock(rwlock_t *rw)
{
	unsigned long tmp;

	__asm__ __volatile__(
		"1:	ldrex	%0, [%1]\n"
		"	teq	%0, #0\n"
		"wfene\n"
		"	strexeq	%0, %2, [%1]\n"
		"	teq	%0, #0\n"
		"	bne	1b"
			: "=&r" (tmp)
			: "r" (&rw->lock), "r" (0x80000000)
			: "cc");

	smp_mb();
}

static inline int write_trylock(rwlock_t *rw)
{
	unsigned long tmp;

	__asm__ __volatile__(
		"1:	ldrex	%0, [%1]\n"
		"	teq	%0, #0\n"
		"	strexeq	%0, %2, [%1]"
			: "=&r" (tmp)
			: "r" (&rw->lock), "r" (0x80000000)
			: "cc");

	if (tmp == 0) {
		smp_mb();
		return 1;
	} else {
		return 0;
	}
}

static inline void write_unlock(rwlock_t *rw)
{
	smp_mb();

	__asm__ __volatile__(
		"str	%1, [%0]\n"
		:
		: "r" (&rw->lock), "r" (0)
		: "cc");

	dsb_sev();
}

/* write_can_lock - would write_trylock() succeed? */
#define write_can_lock(x)		((x)->lock == 0)

/*
 * Read locks are a bit more hairy:
 *  - Exclusively load the lock value.
 *  - Increment it.
 *  - Store new lock value if positive, and we still own this location.
 *    If the value is negative, we've already failed.
 *  - If we failed to store the value, we want a negative result.
 *  - If we failed, try again.
 * Unlocking is similarly hairy.  We may have multiple read locks
 * currently active.  However, we know we won't have any write
 * locks.
 */
static inline void read_lock(rwlock_t *rw)
{
	unsigned long tmp, tmp2;

	__asm__ __volatile__(
		"1:		\n"
		"	ldrex	%0, [%2]\n"
		"	adds	%0, %0, #1\n"
		"	strexpl	%1, %0, [%2]\n"
		"wfemi\n"
		"	rsbpls	%0, %1, #0\n"
		"	bmi	1b"
			: "=&r" (tmp), "=&r" (tmp2)
			: "r" (&rw->lock)
			: "cc");

	smp_mb();
}

static inline void read_unlock(rwlock_t *rw)
{
	unsigned long tmp, tmp2;

	smp_mb();

	__asm__ __volatile__(
		"1:		\n"
		"	ldrex	%0, [%2]\n"
		"	sub	%0, %0, #1\n"
		"	strex	%1, %0, [%2]\n"
		"	teq	%1, #0\n"
		"	bne	1b"
			: "=&r" (tmp), "=&r" (tmp2)
			: "r" (&rw->lock)
			: "cc");

	if (tmp == 0)
		dsb_sev();
}

static inline int read_trylock(rwlock_t *rw)
{
	unsigned long tmp, tmp2 = 1;

	__asm__ __volatile__(
		"1:		\n"
		"	ldrex	%0, [%2]\n"
		"	adds	%0, %0, #1\n"
		"	strexpl	%1, %0, [%2]\n"
			: "=&r" (tmp), "+r" (tmp2)
			: "r" (&rw->lock)
			: "cc");

	smp_mb();
	return tmp2 == 0;
}

/* read_can_lock - would read_trylock() succeed? */
#define read_can_lock(x)		((x)->lock < 0x80000000)

#define read_lock_flags(lock, flags)  read_lock(lock)
#define write_lock_flags(lock, flags) write_lock(lock)

extern void restore_irq_fiq(uint32_t);
extern  uint32_t disable_irq_fiq(void);
extern void enable_irq_fiq(void);

static inline void restore_enable_int(unsigned int flags)
{
   __asm__ __volatile__(
      "msr CPSR_c, %0\n"
      :: "r" (flags)
   );
}

static inline unsigned int save_disable_int(void)
{
   unsigned int flags;
   __asm__ __volatile__(
      "MRS     %0,CPSR \n"
      "MSR     cpsr_c,#(0xC0 | 0x1F)"
         : "=r" (flags)
   );
   return flags;
}

#define spin_lock_irqsave(lock, flags)				\
do {								\
	flags = save_disable_int();\
	spin_lock(lock);	\
} while (0)

#define spin_unlock_irqrestore(lock, flags)				\
do {								\
	spin_unlock(lock);	\
	restore_enable_int(flags);\
} while (0)

#endif