#ifndef _H_Q_SPINLOCK_H_
#define _H_Q_SPINLOCK_H_

/* macros used to lock and unlock a common QSpinlock */
#define Q_SPIN_LOCK              Q_spinLock
#define Q_SPIN_UNLOCK            Q_spinUnlock
#define Q_SPIN_TRYLOCK           Q_spinTryLock

/* macros used to lock and unlock refCtrLock_ of event */
#define QEVENT_SPIN_LOCK(e_)     Q_spinLock(&(((QEvt *)e_)->refCtrLock_))
#define QEVENT_SPIN_UNLOCK(e_)   Q_spinUnlock(&(((QEvt *)e_)->refCtrLock_))

/* macros used to lock and unlock a QEQueue */
#define QEQUEUE_SPIN_LOCK(eq_)   Q_spinLock(&((eq_)->lock))
#define QEQUEUE_SPIN_UNLOCK(eq_) Q_spinUnlock(&((eq_)->lock))

/* macros used to lock and unlock a QMPool */
#define QMPOOL_SPIN_LOCK(mp_)    Q_spinLock(&((mp_)->lock))
#define QMPOOL_SPIN_UNLOCK(mp_)  Q_spinUnlock(&((mp_)->lock))

/* macros used to lock and unlock a QSubscrList */
#define QSUBLST_SPIN_LOCK(sig_)   \
                     Q_spinLock(&((QF_PTR_AT_(QF_subscrList_, sig_)).lock))
#define QSUBLST_SPIN_UNLOCK(sig_) \
                     Q_spinUnlock(&((QF_PTR_AT_(QF_subscrList_, sig_)).lock))

/* macros used to lock and unlock a QCompoSubscrList */
#define QCOMPO_SUBLST_LOCK(sig_)   \
    Q_spinLock(&((QF_PTR_AT_(QCompo_subscrList_, (sig_-QF_maxSignal_))).lock))
#define QCOMPO_SUBLST_UNLOCK(sig_)   \
    Q_spinUnlock(&((QF_PTR_AT_(QCompo_subscrList_, (sig_-QF_maxSignal_))).lock))

/* macros used to lock and unlock a QTimeEvt */
#define QTE_SPIN_LOCK(te_)    Q_spinLock(&(((QTimeEvt *)te_)->lock))
#define QTE_SPIN_UNLOCK(te_)  Q_spinUnlock(&(((QTimeEvt *)te_)->lock))

#define QTE_HEAD_SPIN_LOCK(rate_)   Q_spinLock(&(QF_timeEvtHead_[rate_].lock))
#define QTE_HEAD_SPIN_UNLOCK(rate_) Q_spinUnlock(&(QF_timeEvtHead_[rate_].lock))

#ifdef CONFIG_SMP
/* architecture spinlock head files */
#include "spinlock.h"

/************************ Spin Lock defines and macros **********************/
typedef struct QSpinlockTag{
	spinlock_t lock;
#ifdef CONFIG_DEBUG_SPINLOCK
	unsigned int magic, owner_cpu;
	void *owner_ao;
#endif
}QSpinlock;

#define Q_SPINLOCK_MAGIC		          (0xdead4ead)
#define Q_SPINLOCK_OWNER_AO_INIT	    ((void *)-1L)
#define Q_SPINLOCK_OWNER_CPU_INIT     (0xFF)

#ifdef CONFIG_DEBUG_SPINLOCK
#define Q_SPIN_DEBUG_INIT		                \
	.magic     = Q_SPINLOCK_MAGIC,		          \
	.owner_cpu = Q_SPINLOCK_OWNER_CPU_INIT,			\
	.owner_ao  = Q_SPINLOCK_OWNER_AO_INIT
#else
#define Q_SPIN_DEBUG_INIT
#endif

#define Q_SPIN_LOCK_INITIALIZER         \
	{					                            \
		.lock = __SPIN_LOCK_UNLOCKED,	\
		Q_SPIN_DEBUG_INIT		                \
	}

#define Q_SPIN_LOCK_UNLOCKED	 \
	(QSpinlock) Q_SPIN_LOCK_INITIALIZER

/************************* RW Lock defines and macros ***********************/
typedef struct QRwlockTag{
	rwlock_t lock;
#ifdef CONFIG_DEBUG_SPINLOCK
	unsigned int magic, owner_cpu;
	void *owner_ao;
#endif
}QRwlock;

#define Q_RWLOCK_MAGIC		0xdeaf1eed

#ifdef CONFIG_DEBUG_SPINLOCK
#define Q_RW_LOCK_UNLOCKED					              \
	(QRwlock)	{	                                    \
		    .lock      = __RW_LOCK_UNLOCKED,	    \
				.magic     = Q_RWLOCK_MAGIC,			        \
				.owner_cpu = Q_SPINLOCK_OWNER_CPU_INIT,		\
				.owner_ao  = Q_SPINLOCK_OWNER_AO_INIT	 }
#else
#define Q_RW_LOCK_UNLOCKED   \
	(QRwlock)	{	.lock = __RW_LOCK_UNLOCKED }
#endif


/* macro used to define and initialize a spinlock */
#define Q_SPINLOCK_DEFINE(_lock) \
	QSpinlock _lock = Q_SPIN_LOCK_UNLOCKED

/* macro used to define and initialize a rwlock */
#define Q_RWLOCK_DEFINE(_lock) \
	QRwlock _lock = Q_RW_LOCK_UNLOCKED

/* macro used to initialize a spinlock */
#define Q_spinLockInit(_lock)			\
do{									\
	_lock = Q_SPIN_LOCK_UNLOCKED;	\
}while(0)

/* macro used to initialize a rwlock */
#define Q_rwLockInit(_lock)			\
do{									\
	_lock = Q_RW_LOCK_UNLOCKED;	\
}while(0)


/*.......................... spin lock function ............................*/
static inline void Q_spinLock(QSpinlock *lock)
{
	/* need to add a function to disable preempt for QK */
	spin_lock(&lock->lock);
}

/*......................... spin unlock function ...........................*/
static inline void Q_spinUnlock(QSpinlock *lock)
{
	spin_unlock(&lock->lock);
	/* need to add a function to enable preempt for QK */
}

/*......................... spin trylock function ..........................*/
static inline int Q_spinTryLock(QSpinlock *lock)
{
	/* need to add a function to disable preempt for QK */
	return spin_trylock(&lock->lock);
}

/****************************************************************************/
/*.......................... read lock function ............................*/
static inline void Q_readLock(QRwlock   *lock)
{
	/* need to add a function to disable preempt for QK */
	read_lock(&lock->lock);
}

/*......................... read unlock function ...........................*/
static inline void Q_readUnlock(QRwlock   *lock)
{
	read_unlock(&lock->lock);
	/* need to add a function to enable preempt for QK */
}

/*......................... read trylock function ..........................*/
static inline int Q_readTryLock(QRwlock   *lock)
{
	/* need to add a function to disable preempt for QK */
	return read_trylock(&lock->lock);
}

/****************************************************************************/
/*.......................... write lock function ...........................*/
static inline void Q_writeLock(QRwlock   *lock)
{
	/* need to add a function to disable preempt for QK */
	write_lock(&lock->lock);
}

/*......................... write unlock function ..........................*/
static inline void Q_writeUnlock(QRwlock   *lock)
{
	write_unlock(&lock->lock);
	/* need to add a function to enable preempt for QK */
}

/*......................... write trylock function .........................*/
static inline int Q_writeTryLock(QRwlock   *lock)
{
	/* need to add a function to disable preempt for QK */
	return write_trylock(&lock->lock);
}

#else    /* If CONFIG_SMP is not defined, all the spinlock macros are empty */
#define QSpinlock   int
#define QRwlock     int
#define Q_SPIN_LOCK_UNLOCKED   (0)
#define Q_RW_LOCK_UNLOCKED     (0)
#define Q_SPIN_LOCK_INITIALIZER (0)
#define Q_SPINLOCK_DEFINE(_lock)
#define Q_RWLOCK_DEFINE(_lock)
#define Q_spinLockInit(_lock)
#define Q_rwLockInit(_lock)

/*.......................... spin lock function ............................*/
#define Q_spinLock(_lock)
/*......................... spin unlock function ...........................*/
#define Q_spinUnlock(_lock)
/*......................... spin trylock function ..........................*/
#define Q_spinTryLock(_lock)
/*.......................... read lock function ............................*/
#define Q_readLock(_lock)
/*......................... read unlock function ...........................*/
#define Q_readUnlock(_lock)
/*......................... read trylock function ..........................*/
#define Q_readTryLock(_lock)
/*.......................... write lock function ...........................*/
#define Q_writeLock(_lock)
/*......................... write unlock function ..........................*/
#define Q_writeUnlock(_lock)
/*......................... write trylock function .........................*/
#define Q_writeTryLock(_lock)
#endif


#endif
