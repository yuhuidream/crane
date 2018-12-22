#ifndef h_qmc_set_h
#define h_qmc_set_h

/** \brief macro performs two QPSet8(\a s1, \a s2) add computing,
* computing result is assigned to \a s
*/
#define QPSet8_and(s0_, s1_, s2_)      \
        ( (s0_)->bits = ((s1_)->bits) & ((s2_)->bits) )

/** \brief macro performs two QPSet8(\a s1, \a s2) orr computing,
* computing result is assigned to \a s
*/
#define QPSet8_orr(s0_, s1_, s2_)      \
        ( (s0_)->bits = ((s1_)->bits) | ((s2_)->bits) )

/** \brief Inline function gets negative value of \a s1, result is 
* assigned to \a s
*/
#define QPSet8_not(s0_, s1_)          \
        ( (s0_)->bits = ~((s1_)->bits) )

/** \brief Inline function clears \a s to zero
*/
#define QPSet8_zero(s0_)              \
        ( (s0_)->bits = (uint8_t)0 )

/** \brief Inline function copies \a s1 to \a s
*/
#define QPSet8_cpy(s0_, s1_)          \
        ( (s0_)->bits = ((s1_)->bits) )

/** \brief Inline function performs two QPset64(\a s1, \a s2) add computing,
* computing result is assigned to \a s
*/
static inline void QPSet64_and(QPSet64 *s, QPSet64 *s1, QPSet64 *s2)
{
   for(int i = 0; i < 8; i++){
      if((s->bits[i] = (s1->bits[i] & s2->bits[i])))
         s->bytes |= Q_ROM_BYTE(QF_pwr2Lkup[i + (uint8_t)1]);
      else
         s->bytes &= Q_ROM_BYTE(QF_invPwr2Lkup[i + (uint8_t)1]);
   }
}

/** \brief Inline function performs two QPset64(\a s1, \a s2) orr computing,
* computing result is assigned to \a s
*/
static inline void QPSet64_orr(QPSet64 *s, QPSet64 *s1, QPSet64 *s2)
{
   for(int i = 0; i < 8; i++){
      if((s->bits[i] = (s1->bits[i] | s2->bits[i])))
         s->bytes |= Q_ROM_BYTE(QF_pwr2Lkup[i + (uint8_t)1]);
      else
         s->bytes &= Q_ROM_BYTE(QF_invPwr2Lkup[i + (uint8_t)1]);
   }
}

/** \brief Inline function gets negative value of \a s1, result is 
* assigned to \a s
*/
static inline void QPSet64_not(QPSet64 *s, QPSet64 *s1)
{
   for(int i = 0; i < 8; i++){
      if((s->bits[i] = ~(s1->bits[i])))
         s->bytes |= Q_ROM_BYTE(QF_pwr2Lkup[i + (uint8_t)1]);
      else
         s->bytes &= Q_ROM_BYTE(QF_invPwr2Lkup[i + (uint8_t)1]);
   }
}

/** \brief Inline function clears \a s to zero
*/
static inline void QPSet64_zero(QPSet64 *s)
{
   for(int i = 0; i < 8; i++){
      s->bits[i] = 0;
   }
   s->bytes = 0;
}

/** \brief Inline function copies \a s1 to \a s
*/
static inline void QPSet64_cpy(QPSet64 *s, QPSet64 *s1)
{
   for(int i = 0; i < 8; i++){
      s->bits[i] = s1->bits[i];
   }
   s->bytes = s1->bytes;
}

/** \brief Below macros assign the right definition, macros and APIs for 
* QMCset, that depends on how many active objects are supported in current
* system
*/
#if (QF_MAX_ACTIVE <= 8)
#define QMCSet QPSet8
                             /* native QF core AO set operations */
#define QACTIVE_CORE_SIGNAL_(act_, core_) \
    QPSet8_insert(&QCORE_ACTIVE_SET(core_) (act_)->prio)

#define QACTIVE_CORE_ONEMPTY_(act_, core_) \
    QPSet8_remove(&QCORE_ACTIVE_SET(core_), (act_)->prio)

#define QMCSet_hasElement     QPSet8_hasElement
#define QMCSet_notEmpty       QPSet8_notEmpty
#define QMCSet_findMax        QPSet8_findMax
#define QMCSet_insert         QPSet8_insert
#define QMCSet_remove         QPSet8_remove
#define QMCSet_and            QPSet8_and
#define QMCSet_orr            QPSet8_orr
#define QMCSet_not            QPSet8_not
#define QMCSet_zero           QPSet8_zero
#define QMCSet_cpy            QPSet8_cpy
#else
#define QMCSet QPSet64
                             /* native QF core AO set operations */
#define QACTIVE_CORE_SIGNAL_(act_, core_) \
    QPSet64_insert(&QCORE_ACTIVE_SET(core_), (act_)->prio)

#define QACTIVE_CORE_ONEMPTY_(act_, core_) \
    QPSet64_remove(&QCORE_ACTIVE_SET(core_), (act_)->prio)

#define QMCSet_hasElement     QPSet64_hasElement
#define QMCSet_notEmpty       QPSet64_notEmpty
#define QMCSet_findMax        QPSet64_findMax
#define QMCSet_insert         QPSet64_insert
#define QMCSet_remove         QPSet64_remove
#define QMCSet_and            QPSet64_and
#define QMCSet_orr            QPSet64_orr
#define QMCSet_not            QPSet64_not
#define QMCSet_zero           QPSet64_zero
#define QMCSet_cpy            QPSet64_cpy
#endif

#endif