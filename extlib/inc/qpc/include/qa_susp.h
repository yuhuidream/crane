#ifndef H_QA_SUSP_H
#define H_QA_SUSP_H

#ifdef QACTIVE_SUSPEND
/** \brief Main structure for AO-suspend-wakeup mechanism, may be integrated
* to QActive structure in the future
* 
*/
typedef struct QSuspActiveTag {

    uint8_t  prio;                             /* priority of active object */
#ifdef CONFIG_SMP
    uint8_t  core;                    /* The core that current AO sleeps on */
#endif

    uint8_t  status;       /* 0: wake up, 1: suspended, 2: wait for wake-up */

#ifdef COMPOSITE_LEVEL_SUSPEND
    int32_t  suspCompNum;
#endif

    int32_t  result;                      /* return value from the other AO */
                    /* structure that contains the context of suspending AO */
    QRegCntxt context;

#ifdef CONFIG_SMP
    QSpinlock lock;
#endif

    /** link to the next suspend active object in the list
    */
    struct QSuspActiveTag *next;
} QSuspActive;

#ifdef COMPOSITE_LEVEL_SUSPEND
#define QA_SUSP_COMP_INC(p)    (suspAOList[p].suspCompNum ++)
#define QA_SUSP_COMP_DEC(p)    (suspAOList[p].suspCompNum --)
#define QA_HAS_SUSP_COMP(p)    (suspAOList[p].suspCompNum)
#define QA_HAS_NO_SUSP_COMP(p) (suspAOList[p].suspCompNum == 0)
#endif

/** Status of Active object
*/
enum QActiveStatus{
   QA_ACTIVE = 0,
   QA_SUSPENDED,
   QA_WAIT_TO_WAKE,
   QA_AUTO_SUSPENDED
};

extern QSuspActive suspAOList[QF_MAX_ACTIVE+1];

/** \brief Initialize the AO-suspend-wakeup(a.k.a. RTC break down) mechanism
* 
* \sa QA_KICK_SEND_RESULT(), QA_SUSPEND_WAIT4RESUME()
*/
void QA_suspInit(void);

/** \brief Check if AO(whose priority is p) is suspended or not
* 
* \a p, AO priority
* \sa QA_SUSPEND_WAIT4RESUME()
*/
int QA_isSuspended(uint8_t p);

/** \brief After slave AO finish work for its master, it will wake up
* master AO and give master the result  
* 
* \a p, priority of AO which will be waked up; result, response from slave AO
* \sa 
*/
void QA_KICK_SEND_RESULT(uint8_t p, int result);

/** \brief Suspend current AO(which is calling this function, master) and wait
* for other AO(assistant or slave) to wake it up after finish master AO's work
*
* \sa QA_KICK_SEND_RESULT()
*/
int QA_SUSPEND_WAIT4RESUME(void);

/** \brief Vanilla will bring up suspending AO when it's waked up by a 
* slave AO
* 
* \a c, core id
* \sa QA_SUSPEND_WAIT4RESUME(), QA_KICK_SEND_RESULT()
*/
void QF_WakeUpSuspAO(uint8_t c);

/** \brief AO set up a alarm(arm a ms time event) and go to sleep
*
* \a ms, time when the sleeping AO will be waked up
* \sa QTimeEvt_yell()
*/
void QA_mSleep(int ms);

/** \brief When the time event which is armed by a sleeping AO expires, it
* will yell the AO and wake it ip
*
* \a te, pointer of time event
* \sa QA_mSleep(), QTimeEvt_isAlarm()
*/
void QTimeEvt_yell(QTimeEvt *te);

/** \brief Check if this time evt is armed by a sleeping AO
*
* \a te, pointer of time event
* \sa QA_mSleep(), QTimeEvt_yell()
*/
int QTimeEvt_isAlarm(QTimeEvt *te);

/** \brief Macro used to save the ARM registers before an AO is suspended
* 
* \a p, AO priority
* \sa QA_contextRestore()
*/
#define  QA_contextSave(p)       \
   do{      \
      __asm__ __volatile__(      \
         "mov  r2, %0\n"      \
         "mrs  r0, cpsr\n"    \
         "mov  r1, pc\n"         \
         "stmed r2!, {r0-r1, r3-r14}\n"      \
         :     \
         :"r"(&(suspAOList[p].context.lr))   \
         :"r0", "r1", "r2"    \
      );    \
   }while(0)

/** \brief Macro used to restore the ARM registers when an AO is waked up
* 
* \note This macro have several versions according to diffrent implements
* of AO-suspend-wakeup mechanism
*
* \a c, core id(may be not used); p, AO priority
* \sa QA_contextSave()
*/
#define   QA_contextRestore(c, p)   \
   do{      \
      (void)c;    \
      __asm__ __volatile__(         \
         "mov   r0, %0\n"        \
         "ldmda r0, {r1-r2, r3-r12, sp, lr}\n"     \
         "msr   cpsr_c, #(0x12 | 0xC0)\n"       \
         "msr   spsr_cxsf,  r1\n"         \
         "mov   lr, r2\n"        \
         "movs  pc, lr\n"        \
         :        \
         :"r"(&(suspAOList[p].context.lr))      \
         :        \
      );       \
   }while(0)

#ifdef QACTIVE_WAKE_DISORDER
#include "bbu_malloc.h"
#define QF_MALLOC(size)  bbu_malloc(size)
#define QF_MFREE(pt)     bbu_mfree(pt)

/** \brief Macro used to save ARM registers of access point of QF schedular
* 
* \a c, core id(not used in single core mode)
* \sa QF_SCHED_POINT_RESTORE()
*/
#ifdef CONFIG_SMP
#define QF_SCHED_POINT_SAVE(c)   \
   do{      \
      __asm__ __volatile__(      \
         "mov   r2, %0\n"     \
         "mrs   r0, cpsr\n"      \
         "mov   r1, pc\n"     \
         "stmed r2!, {r0-r1, r3-r14}\n"  \
         :     \
         :"r"(&(QCORE_ACS_PNT_CNTXT(c).lr))  \
         :"r0", "r1", "r2"    \
      );    \
   }while(0)
#else
#define QF_SCHED_POINT_SAVE(c)   \
   do{      \
      (void)c;    \
      __asm__ __volatile__(      \
         "mov   r2, %0\n"     \
         "mrs   r0, cpsr\n"      \
         "mov   r1, pc\n"     \
         "stmed r2!, {r0-r1, r3-r14}\n"  \
         :     \
         :"r"(&(QCORE_ACS_PNT_CNTXT().lr))  \
         :"r0", "r1", "r2"    \
      );    \
   }while(0)
#endif

/** \brief Macro used to restore ARM registers of access point of QF schedular
* 
* \a c, core id(not used in single core mode)
* \sa QF_SCHED_POINT_SAVE()
*/
#ifdef CONFIG_SMP
#define QF_SCHED_POINT_RESTORE(c)   \
   do{   \
      (void)c;  \
      __asm__ __volatile__(      \
         "mov   r0, %0\n"        \
         "ldmda r0, {r1-r2, r3-r12, sp, lr}\n"     \
         "msr   cpsr_c, #(0x12 | 0xC0)\n"       \
         "msr   spsr_cxsf, r1\n"       \
         "mov   lr,  r2\n"       \
         "movs  pc,  lr\n"    \
         :     \
         :"r"(&QCORE_ACS_PNT_CNTXT(c).lr)    \
         :     \
      );       \
      while(1);            /* should never reach here */ \
   }while(0)

#else
#define QF_SCHED_POINT_RESTORE(c)   \
   do{   \
      (void)c;   \
      __asm__ __volatile__(      \
         "mov   r0, %0\n"        \
         "ldmda r0, {r1-r2, r3-r12, sp, lr}\n"     \
         "msr   cpsr_c, #(0x12 | 0xC0)\n"       \
         "msr   spsr_cxsf, r1\n"       \
         "mov   lr,  r2\n"       \
         "movs  pc,  lr\n"    \
         :     \
         :"r"(&QCORE_ACS_PNT_CNTXT().lr)    \
         :     \
      );       \
      while(1);            /* should never reach here */ \
   }while(0)
#endif

#endif                              /* end of 'ifdef QACTIVE_WAKE_DISORDER' */

#else                                    /* else of 'ifdef QACTIVE_SUSPEND' */

#define QF_MALLOC(size)                ((void)size)
#define QF_MFREE(pt)                   ((void)pt)
#define QA_suspInit()
#define QA_isSuspended(p)                ((void)p, 0)
#define QA_KICK_SEND_RESULT(p, result)   do{(void)c;(void)result;}while(0)
#define QA_SUSPEND_WAIT4RESUME()         (0)
#define QF_WakeUpSuspAO(c)               do{(void)c;}while(0)
#define QF_SCHED_POINT_SAVE(c)           do{(void)c;}while(0)
#define QF_SCHED_POINT_RESTORE(c)        do{(void)c;}while(0)
#define QA_mSleep(ms)                    do{(void)ms;}while(0)
#define QTimeEvt_isAlarm(te)             ((void)te, 0)
#define QTimeEvt_yell(te)                do{(void)te;}while(0)
#endif                                   /* else of 'ifdef QACTIVE_SUSPEND' */
#endif                                      /* end of '#ifndef H_QA_SUSP_H' */
