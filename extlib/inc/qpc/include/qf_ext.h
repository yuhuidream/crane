#ifndef qf_ext_h
#define qf_ext_h

#include "qf_conf.h"

#ifndef Q_CPU_NUM
#define Q_CPU_NUM 4
#endif

#ifndef QMC_CORE_NUM
#define QMC_CORE_NUM        ((uint8_t)Q_CPU_NUM)
#endif

#define QMC_PRIMARY_CORE    0

#define INVALID_CORE      ((uint8_t)(QMC_CORE_NUM + 1))
#define INVALID_AO_PRIO   ((uint8_t)(QF_MAX_ACTIVE + 1))
#define INVALID_ACTIVE    ((QActive *)0)
#define NO_CORE_2_STEAL   (INVALID_CORE)

#ifdef CONFIG_SUSP_DBG
#define QF_DBG_ON 1
#else
#define QF_DBG_ON 0
#endif

#define WATCHDOG_INIT_COUNT   (QMC_CORE_NUM*10)
#define WATCHDOG_INCRE_STEP   (QMC_CORE_NUM)
#define WATCHDOG_DECRE_STEP   (1)

#ifdef  CONFIG_CPU_AO
#define QF_CPU_AO_PRIO(c_)    (QF_MAX_ACTIVE-c_)
#endif

#define QF_DBG_MSG(str, ...) \
       ((void)((QF_DBG_ON)?(bbu_printf (str, ##__VA_ARGS__)),1:0))

/** \brief structure to store the register context
*/
typedef struct QRegCntxtTag{
   uint32_t cpsr;
   uint32_t pc;
   uint32_t r3_r12[10];
   uint32_t sp;
   uint32_t lr;
}QRegCntxt;

#include "qspinlock.h"

#include "qp_port.h"

#include "qpset.h"

#include "qa_susp.h"            /* added for RTC break down mechanism */

#include "qcompo.h"          /* added for the composite-level suspend */

#include "qmc_set.h"

#include "qa_stk.h"

typedef enum{
   OFF_LINE = 0,
   ON_LINE,
   OFF_LINE_PREPARE
}QCoreStatus;

typedef struct QCoreCntxtTag{
                            /* Active object set of AOs runing on this core */
   QMCSet       actSet;

                  /* Backup of Active object set of AOs runing on this core */
   QMCSet       actSetBk;

   QActive*     curAct;                     /* Pointer to current runing AO */
   QSpinlock    curActLock;

   QCoreStatus  status;                /* core status: on-line, off-line... */
   QSpinlock    statusLock;

#ifdef QACTIVE_SUSPEND
   QSuspActive* actSuspHead;               /* Head of the suspended AO List */
   QSpinlock    actSuspHeadLock;
 #ifdef QACTIVE_WAKE_DISORDER
                                /* Access point context of the QF schedular */
   QRegCntxt  acsPnt;
 #endif

 #ifdef COMPOSITE_LEVEL_SUSPEND
   QCompo*     curComp;
   QSpinlock   curCompLock;
 #endif
#endif

#ifdef QF_CONTEXT_CTRL
   QEvt          * curEvt;
   QStateHandler  curAoState;
#endif

#ifdef QF_STACK_PER_AO
   uint32_t sp;
#endif

              /* SW watch dog used for stealing works from busy cores */
   int          wDogCnt;
   QSpinlock    wDogLock;

   /* This lock is used to avoid below situations
    * 1. Core a is stealed by Core b and Core c simultaneously
    * 2. During Core b steals AO c for Core a, Core a schedule AO c 
    *    and run it
    */
   QSpinlock    schedLock;

}QCoreCntxt;

extern QMCSet QA_suspSet_;   /* Set indicates which AOs are suspended */

#ifdef CONFIG_SMP
extern QCoreCntxt QF_core_[QMC_CORE_NUM];
extern QSpinlock  QA_suspSetLock_;
extern QSpinlock  QCore_activeSetLock_;
extern QSpinlock  QCoreHotPlugLock_;

#define QCORE_ACTIVE_SET(c)         (QF_core_[c].actSet)
#define QCORE_ACTIVE_BK_SET(c)      (QF_core_[c].actSetBk)
#define QCORE_CUR_ACT(c)            (QF_core_[c].curAct)
#define QCORE_CURRENT_COMP(c)       (QF_core_[c].curComp)
#define QCORE_STATUS(c)             (QF_core_[c].status)
#define QCORE_SUSP_ACT_HEAD(c)      (QF_core_[c].actSuspHead)
#define QCORE_ACS_PNT_CNTXT(c)      (QF_core_[c].acsPnt)
#define QCORE_WATCH_DOG_CNT(c)      (QF_core_[c].wDogCnt)
#define QCORE_STACK_POINTER(c)      (QF_core_[c].sp)
#define QCORE_CUR_ACT_STATE(c)      (QF_core_[c].curAoState)
#define QCORE_CUR_EVT(c)            (QF_core_[c].curEvt)

#define QA_IRQ_CONNECT(irq_, act_)   QA_irqConnect(irq_, act_)

#define QCORE_HOT_PLUG_LOCK()           \
        Q_SPIN_LOCK(&QCoreHotPlugLock_)
#define QCORE_HOT_PLUG_UNLOCK()         \
        Q_SPIN_UNLOCK(&QCoreHotPlugLock_)

#define QCORE_ACTIVE_SET_LOCK()         \
        Q_SPIN_LOCK(&QCore_activeSetLock_)
#define QCORE_ACTIVE_SET_UNLOCK()       \
        Q_SPIN_UNLOCK(&QCore_activeSetLock_)

#define QCORE_CUR_ACT_LOCK(c)    \
        Q_SPIN_LOCK(&QF_core_[c].curActLock)
#define QCORE_CUR_ACT_UNLOCK(c)  \
        Q_SPIN_UNLOCK(&QF_core_[c].curActLock)

#define QCORE_CURRENT_COMP_LOCK(c)      \
        Q_SPIN_LOCK(&QF_core_[c].curCompLock)
#define QCORE_CURRENT_COMP_UNLOCK(c)    \
        Q_SPIN_UNLOCK(&QF_core_[c].curCompLock)

#define QCORE_STATUS_LOCK(c)            \
        Q_SPIN_LOCK(&QF_core_[c].statusLock)
#define QCORE_STATUS_UNLOCK(c)          \
        Q_SPIN_UNLOCK(&QF_core_[c].statusLock)

#define QCORE_SUSP_ACT_HEAD_LOCK(c)     \
        Q_SPIN_LOCK(&QF_core_[c].actSuspHeadLock)
#define QCORE_SUSP_ACT_HEAD_UNLOCK(c)   \
        Q_SPIN_UNLOCK(&QF_core_[c].actSuspHeadLock)

#define QCORE_WATCH_DOG_LOCK(c)         \
        Q_SPIN_LOCK(&QF_core_[c].wDogLock)
#define QCORE_WATCH_DOG_UNLOCK(c)       \
        Q_SPIN_UNLOCK(&QF_core_[c].wDogLock)

#define QCORE_SCHED_TRYLOCK(c)          \
        Q_SPIN_TRYLOCK(&QF_core_[c].schedLock)
#define QCORE_SCHED_LOCK(c)             \
        Q_SPIN_LOCK(&QF_core_[c].schedLock)
#define QCORE_SCHED_UNLOCK(c)           \
        Q_SPIN_UNLOCK(&QF_core_[c].schedLock)

#else
extern  QCoreCntxt QF_core_;
#define QCORE_ACTIVE_SET()         (QF_core_.actSet)
#define QCORE_ACTIVE_BK_SET()      (QF_core_.actSetBk)
#define QCORE_CUR_ACT()            (QF_core_.curAct)
#define QCORE_CURRENT_COMP()       (QF_core_.curComp)
#define QCORE_SUSP_ACT_HEAD()      (QF_core_.actSuspHead)
#define QCORE_ACS_PNT_CNTXT()      (QF_core_.acsPnt)
#define QCORE_STACK_POINTER()      (QF_core_.sp)
#define QCORE_CUR_ACT_STATE()      (QF_core_.curAoState)
#define QCORE_CUR_EVT()            (QF_core_.curEvt)
#define QCORE_SCHED_TRYLOCK(c)
#define QCORE_SCHED_LOCK(c)
#define QCORE_SCHED_UNLOCK(c) 
#define QA_IRQ_CONNECT(irq_, act_)

#endif

/** \brief Framework initialization for multi-core SMP extensions
*/
void QF_extInit(void);

/** \brief Used to get a specific core's ready set of AO event quene 
* 
* \a c, core id; readysSet
*/
void QC_getReadySet(QMCSet *readySet, uint8_t c);

#ifdef CONFIG_QH
void QActive_unmask(uint8_t p, uint8_t c);
#define QACTIVE_UNMASK(p_, c_)   QActive_unmask(p_, c_)
#else
#define QACTIVE_UNMASK(p_, c_)
#endif


/** \brief LRAB(Long RTC Auto Break) mechanism
* This is a extended mechanism for BBU. In BBU, we sometimes have some very
* long tests which will last for a long time and even not terminate, such as
* mlucas. Then, this mechanism can be used to break such long RTC(not a real 
* RTC) so that we can enter other commands or do other tests during the break.
*/
#ifdef LRAB
extern void (*ipc_2_break_lrtc)(uint8_t c);

/** \brief Interval to break long RTC in an AO with LRAB enabled
*/
#define QACTIVE_LRAB_INTERVAL(me_)   (me_)->rInterval

/** \brief Construct an AO with LRAB mechanism
*/
void QActive_ctorX(QActive * const me, QStateHandler initial);

/** \brief Used to break current long RTC, this function should be called
* in interrupt handler(QF_tick() of timer or IPC(SGI) handler).
* It should have no harm when it's called not in a long RTC
*/
void QActive_lrtcBreak(void);

/** \brief Enable long RTC auto break mechanism at AO \a me
*/
void QA_enableLRAB(QActive *me);

/** \brief Disable long RTC auto break mechanism at AO \a me
*/
void QA_disableLRAB(QActive *me);

/** \brief Check if a time event used for LRAB
*/
int QTimeEvt_4LRAB(QTimeEvt *te);

/** \brief Used to break other core's long RTC in multi-core mode
*/
#ifdef CONFIG_SMP
void QC_sendIpc2breakLrtc(uint8_t c);
#endif

/** \brief flag to indicate that a time event expires to break long RTC
*/
#define LRAB_TRIGGERED (0xdeadbeef)

#endif

/** \brief Below are some macros used for ctrl+c/ctrl+d feartures
* ctrl+c is to break RTC of current core which uart interrupt is routed to
* ctrl+d is used to break RTCs of all the cores. 
* Current implement is based on QHsm, NOT QMsm, maybe need to re-implemented
* for QMsm. 
*/
#ifdef QF_CONTEXT_CTRL
#ifdef CONFIG_SMP
/** \brief Check if AO state is saved before QF dispatch event to current AO
*/
#define QF_CONTEXT_SAVED(c_)        QCORE_CUR_EVT(c_)

/** \brief Clear the record saved by last QF_SAVE_CONTEXT
*/
#define QF_CLEAR_CONTEXT(c_)        \
    do{     \
        QCORE_CUR_EVT(c_) = (QEvt *)0;   \
    } while(0)

/** \brief Save some AO state machine information before dispatch event to it
*/
#define QF_SAVE_CONTEXT(c_, a_, e_)  \
     do {   \
        QCORE_CUR_ACT_STATE(c_) = a_->super.state.fun; \
        QCORE_CUR_EVT(c_) = (QEvt *)e_;   \
    } while(0)


/** \brief Break current RTC by restore AO state to be stable and jump to
* QP access point(a.k.a. the point before run to dispatch endless loop)
*/
#define QF_RESTORE_CONTEXT(c_)          \
    do {   \
        if(QF_CONTEXT_SAVED(c_)){   \
            QCORE_CUR_ACT(c_)->super.state.fun = QCORE_CUR_ACT_STATE(c_);  \
            QCORE_CUR_ACT(c_)->super.temp.fun = QCORE_CUR_ACT_STATE(c_);  \
            QCORE_CUR_EVT(c_)->sig = (QSignal)MAX_SIG;   \
            QACTIVE_UNMASK(QCORE_CUR_ACT(c_)->prio, c_); \
            QF_SCHED_POINT_RESTORE(c_);   \
        }   \
    }while(0)

#else
#define QF_CONTEXT_SAVED(c_)        QCORE_CUR_EVT()

#define QF_CLEAR_CONTEXT(c_)        \
    do{     \
        (void)c_;          \
        QCORE_CUR_EVT() = (QEvt *)0;   \
    } while(0)

#define QF_SAVE_CONTEXT(c_, a_, e_)  \
     do {   \
        (void)c_;          \
        QCORE_CUR_ACT_STATE() = a_->super.state.fun; \
        QCORE_CUR_EVT() = (QEvt *)e_;   \
    } while(0)


#define QF_RESTORE_CONTEXT(c_)          \
    do {   \
        (void)c_;          \
        if(QF_CONTEXT_SAVED(c_)){   \
            QCORE_CUR_ACT()->super.state.fun = QCORE_CUR_ACT_STATE();  \
            QCORE_CUR_ACT()->super.temp.fun = QCORE_CUR_ACT_STATE();  \
            QCORE_CUR_EVT()->sig = (QSignal)MAX_SIG;   \
            QACTIVE_UNMASK(QCORE_CUR_ACT()->prio, 0); \
            QF_SCHED_POINT_RESTORE(0);   \
        }   \
    }while(0)
#endif

#else
/** When QF_CONTEXT_CTRL is not defined, all related macros are empty
*/
#define QF_CONTEXT_SAVED(c_) 
#define QF_CLEAR_CONTEXT(c_)
#define QF_SAVE_CONTEXT(c_, a_, e_)
#define QF_RESTORE_CONTEXT(c_)
#endif

/** \brief Pointer to the CPU local AO pointer array, QC_localAoInit
* is used to initialize it.
*/
extern QActive **QC_LOCAL_AO;
void QC_localAoInit(QActive **cpuAo);

#ifdef CONFIG_SMP
/** \brief Register some ICU APIs to virtual QP ICU, QP ICU is used
* to keep an AO and the interrupt which are connected to it on the
* core. When an AO transfer from coreA to coreB, all its connected
* interrupts are rerouted to coreB.  
*/
void QF_icuInit(void (*irq_target_set)(int irq, int target),
               int  (*irq_target_get)(int irq),
               void (*irg_target_clear)(int irq),
               int  int_total_nums);

/** \biref Used to get current core id
*/
uint8_t (*QC_getSmpCoreId)(void);
#else
#define QF_icuInit(set_, get_, clear_, num_)
#define QC_getSmpCoreId()   (0)
#endif

/** \brief Framework initialization for multi-core SMP extensions
*  
*/
void QF_extInit(void);

/** \brief Used to get a specific core's ready set of AO event quene 
* 
* \a c, core id; readysSet
*/
void QC_getReadySet(QMCSet *readySet, uint8_t c);


/****************************************************************************
  Below functions are defined in qa_core.c and qc_act.c
  They're used to manage and check the relationship between Core and AO
  Most of them are necessary for QF multi-core extension, when CONFIG_SMP
  is not defined, they're dummy. Some of them are also useful in single
  core mode for other QF extension features(Need to clear???)
*****************************************************************************/
#if !defined(QF_ORIGINAL) || defined(CONFIG_SMP)
/** \brief Get the AO pointer which is running on core(c)
* 
* \a c, core id(ignored in single-core mode)
* \sa QC_setCurrentAO(), QC_clearCurrentAO()
*/
QActive* QC_getCurrentAO(uint8_t c);

/** \brief When Core(c) is scheduled to handle AO(act), record its pointer
* 
* \a c, core id(ignored in single-core mode); act, AO pointer
* \sa QC_getCurrentAO() QC_clearCurrentAO()
*/
void QC_setCurrentAO(uint8_t c, QActive *act);

/** \brief After core(c) finishes handling AO(act), clear the record marked
* by QC_setCurrentAO()
* 
* \a c, core id(ignored in single-core mode)
* \sa QC_getCurrentAO() QC_setCurrentAO()
*/
void QC_clearCurrentAO(uint8_t c);

/** \brief Clear the running-AO records of all cores
* 
* \note This function is used in initial stage(in QF_extInit())
*/
void QC_clearAllCurrentAO(void);

/** \brief Set AO(act) to be a slave of Core(c), then it will be added to
* the QA_coreSet_ of Core(c) and removed from others'. If there are any 
* interrupts belong to this AO, they're also rerouted to the new master core
* 
* \a act(pointer of AO); c, core id
* \sa QA_getCore(), QA_clearCore(), QA_isBelong2Core()
*/
void QA_setCore(QActive *act, uint8_t c);

/** \brief Free AO(act) from Core(c)
* 
* \a act(pointer of AO); c, core id
* \sa QA_setCore(), QA_getCore(), QA_isBelong2Core()
*/
void QA_clearCore(QActive *act, uint8_t c);

/** \brief Get the master Core id of AO(act)
* 
* \a act(pointer of AO)
* \sa QA_setCore(), QA_clearCore(), QA_isBelong2Core()
*/
uint8_t QA_getCore(QActive *act);

/** \brief Free AO(act) totally and it doesn't belong to any core
* 
* \note This function is usually called when stop an AO
* \a act(pointer of AO)
* \sa QA_setCore(), QA_clearCore(), QA_getCore()
*/
void QA_clearCoreAll(QActive *act);
#else
#define QC_getCurrentAO(c)          ((void)c, 0)
#define QC_setCurrentAO(c, act)     do{(void)c; (void)act;}while(0)
#define QC_clearCurrentAO(c)        do{(void)c;}while(0)
#define QC_clearAllCurrentAO()

#define QA_setCore(act, c)          do{(void)c; (void)act;}while(0)
#define QA_getCore(act)             ((void)act, 0)
#define QA_clearCore(act, c)        do{(void)c; (void)act;}while(0)
#define QA_clearCoreAll(act)        do{(void)act;}while(0)
#endif






/****************************************************************************
  Below functions are defined in qc_htplg.c
  They're used to support Core hot plug and unplug mechanism and only
  meaningfull in SMP mode.
  We use the macro 'QF_CPU_HTPLG' to enable this mechanism, if this macro
  and CONFIG_SMP are not defined, all these functions are dummy
*****************************************************************************/
#if defined(QF_CPU_HTPLG) && defined(CONFIG_SMP)

/** \brief migrate all AOs of a shutting down Core(c) to other alive cores
* Usually, the AOs are transferred to next online core(c+1%CORE_NUM).
* And the current ownership are backupped for restore when Core(c) restart 
* 
* \a c, the core which will be shut down
* \sa QA_restoreBackAll()
*/
void QA_migrateAwayAll(uint8_t c);

/** \brief restore th AOs to a restarted Core(c), there AOs are belong to 
* core(c) before it is shut down
* 
* \a c, the core which is restarted
*
* \note, this function may be useless, we may needn't to restore and just let
* work stealing mechanism to maintain the AO ownership, then we needn't to
* backup AO ownership of a dying core in QA_migrateAwayAll() either
*
* \sa QA_migrateAwayAll
*/
void QA_restoreBackAll(uint8_t c);

/** \brief This function is called by a killer core to shut down Core(c)
*
* \a c, victim core which will be shut down
*
* \sa QC_setSelfDown(), QC_rebootSecondary(c)
*/
int QC_setCoreDown(uint8_t c);

/** \brief This function is called by victim core, after this, this core will
* go offline and to lower power mode
* 
* \sa QC_setCoreDown(), QC_rebootSecondary()
*/
int QC_setSelfDown(void);

/** \brief This function is called by a saviour to reboot Core(c)
* \a c, the core which will be restarted
*
* \sa QC_setCoreDown(), QC_setSelfDown()
*/
void QC_rebootSecondary(uint8_t c);
#else
/** \brief dummy version of above functions
*/
#define QA_migrateAwayAll(c)         do{(void)c;}while(0)
#define QA_restoreBackAll(c)         do{(void)c;}while(0)
#define QC_setCoreDown(c)            ((void)c, 0)
#define QC_setSelfDown(c)
#define QC_rebootSecondary(c)        do{(void)c;}while(0)
#endif




/****************************************************************************
  Below functions are defined in qc_wksl.c
  They are all used for work stealing(WKSL) mechanism.
  WKSL is neccessary part in QF multi-core extension, it's used to balance
  the loading for all online cores.
  If the CONFIG_SMP macro isn't defined, all these functions are dummy
*****************************************************************************/
#ifdef CONFIG_SMP

/** \brief This functions is called by an idle core, it will snoop its 
* neighbour cores and steal an AO from an busy core
*
* \a c, id of the core which is idle 
* \sa QC_stealFromBusyCore()
*/
void QC_WorkStealWhenIdle(uint8_t c);

/** \brief This functions is called by a core(needn't to be idle) to steal
* AO from its neighbour cores which is much busier than it
* 
* \note This mechanism need a software watchdog to measure the cores' loading
* \a c, thief core id
* \sa QC_WorkStealWhenIdle(), QMC_WatchDogInit()
*/
void QC_stealFromBusyCore(uint8_t c);

/** \brief Initialize watchdog mechanism for QC_stealFromBusyCore()
* must be called in QF_extInit()
*
* \sa QC_stealFromBusyCore(), QC_WatchDogReset()
*/
void QMC_WatchDogInit(void);

/** \brief Reset the watchdog counter of Core(c)
*
*\ c, core id
* \sa QMC_WatchDogInit()
*/
void QC_WatchDogReset(uint8_t c);

/** \brief Check if worksteal scheduler is turned on(return 1) or not(return 0)
* 
* \sa QC_turnOnWorksteal(), QC_turnOffWorksteal()
*/
int QC_isWorkStealOn(void);

/** \brief Turn on worksteal scheduler
* 
* \sa QC_turnOffWorksteal(), QC_isWorkStealOn()
*/
void QC_turnOnWorksteal(void);

/** \brief Turn off worksteal scheduler
* 
* \sa QC_isWorkStealOn(), QC_turnOnWorksteal()
*/
void QC_turnOffWorksteal(void);
#else
#define QC_WorkStealWhenIdle(c)    do{(void)c;}while(0)
#define QC_stealFromBusyCore(c)    do{(void)c;}while(0)
#define QC_WatchDogReset(c)        do{(void)c;}while(0)
#define QMC_WatchDogInit()
#define QC_isWorkStealOn()         (0)
#define QC_turnOnWorksteal()
#define QC_turnOffWorksteal()
#endif





/****************************************************************************
  Below functions are defined in qc_onln.c
  They're only meaningfull in multi-core mode
  If the CONFIG_SMP macro isn't defined, all these functions are dummy
*****************************************************************************/

#ifdef CONFIG_SMP
/** \brief Initialize core status and number of online cores
* d
* \sa QC_isOnline(), QC_setOffline(), QC_getOnlineNum()
*/
void QC_onlnInit(void);

/** \brief Set status of Core(c) to be online
* 
* \a c, core id
* \sa QC_isOnline(), QC_setOffline(), QC_getOnlineNum()
*/
void QC_setOnline(uint8_t c);

/** \brief Check if Core(c) is online(return 1), otherwise return 0
* 
* \a c, core id
* \sa QC_setOnline(), QC_setOffline(), QC_isOffline()
*/
int QC_isOnline(uint8_t c);

/** \brief Set status of Core(c) to be offline
* 
* \a c, core id
* \sa QC_setOnline(), QC_isOffline()
*/
void QC_setOffline(uint8_t c);

/** \brief Check if Core(c) is offline(return 1), otherwise return 0
* 
* \a c, core id
* \sa QC_setOffline(), QC_isOnline()
*/
int QC_isOffline(uint8_t c);

/** \brief Set status of Core(c) to be preparing to offline
* 
* \a c, core id
* \sa QC_isPreOffline()
*/
void QC_setPreOffline(uint8_t c);

/** \brief Check if Core(c) is preparing to offline(return 1),
* otherwise return 0
* 
* \a c, core id
* \sa QC_setPreOffline()
*/
int QC_isPreOffline(uint8_t c);

/** \brief Get the number of current online cores
* 
* \sa QC_setOnline(), QC_setOffline()
*/
int QC_getOnlineNum(void);
#else
#define QC_setOnline(c)          do{(void)c;}while(0)
#define QC_setOffline(c)         do{(void)c;}while(0)
#define QC_setPrepareOffline(c)  do{(void)c;}while(0)
#define QC_isOnline(c)           ((void)c, 1)
#define QC_isOffline(c)          ((void)c, 0)
#define QC_isPrepareOffline(c)   ((void)c, 0)
#define QC_getOnlineNum()        (1)
#define QC_onlnInit()
#endif






/****************************************************************************
  Below functions are defined in qc_irq.c
  They're only meaningfull in multi-core mode
  If the CONFIG_SMP macro isn't defined, all these functions are dummy
*****************************************************************************/
#if defined(CONFIG_SMP) && defined(QACTIVE_IRQ_TIED)
/** \brief Set interrupt(irq) to be a slave of AO(act), irq will be routed
* to the core that act belongs to
* 
* \a irq, interrupt id; act, AO pointer
* \sa QA_irqRelatedTo(), QC_irqRelatedTo()
*/
void QA_irqConnect(int irq, QActive *act);

/** \brief Initialize the relationship between interrupts and AOs, this 
* function must be called in QF_extInit()
* 
* \sa QA_irqConnect()
*/
void QA_relatedIrqInit(void);

/** \brief Get the pointer of master AO of the interrupt(irq)
* 
* \a irq, interrupt id
* \sa QA_irqConnect()
*/
QActive *QA_irqRelatedTo(int irq);

/** \brief Get the core id that interrupt(irq) is related to (its master
* AO's master)
* 
* \a irq, interrupt id
* \sa QA_irqRelatedTo()
*/
uint8_t QC_irqRelatedTo(int irq);

/** \brief Reroute all the slave interrupts of AO(act), this function is
* used when AO(act) is transferred to a new core
*
* \a act, AO pointer
* \sa QC_irqRelatedTo(), QA_irqRelatedTo()
*/
void QA_irqReroute(QActive *act);
#else
#define QA_irqConnect(irq, act)     do{(void)irq;(void)act;}while(0)
#define QA_relatedIrqInit()
#define QA_irqRelatedTo(irq)        ((void)irq, (QActive *)0)
#define QC_irqRelatedTo(irq)        ((void)irq, 0)
#define QA_irqReroute(act)          do{(void)act;}while(0)
#endif





/****************************************************************************
  Below functions are defined in qa_tied.c
  They're only meaningfull in multi-core mode and controlled by the 
  macro QACTIVE_CPU_TIED
  If the CONFIG_SMP macro isn't defined, all these functions are dummy
*****************************************************************************/
#if defined(QACTIVE_CPU_TIED) && defined(CONFIG_SMP)
/** \brief The set that indicates which active objects are tied
* 
*/
extern QMCSet QA_tiedSet_;
extern QSpinlock QA_tiedSetLock_;

/** \brief Free an AO(act) and it can be stolen by other core
* 
* \a act, AO pointer
* \sa QA_setTiedCore()
*/
void QA_clearTied(QActive *act);

/** \brief Tie the AO(act) to Core(c) and it won't be stolen by other cores
* 
* \a act, AO pointer; c, core id
* \sa QA_clearTied()
*/
void QA_setTiedCore(QActive *act, uint8_t c);

/**
  * Check if active object 'act' is tied to core 'c'
  */
/** \brief Check if AO(act) is tied to Core(c) (return 1), otherwise retun 0
* 
* \a act, AO pointer; c, core id
* \sa QA_setTiedCore(), QA_clearTied()
*/
int QA_isTied2Core(QActive *act, uint8_t c);
#else
#define QA_clearTied(act)              do{(void)act;}while(0)
#define QA_setTiedCore(act, c)         do{(void)act;(void)c;}while(0)
#define QA_isTied2Core(act, c)         ((void)act, (void)c, 1)
#endif




/****************************************************************************
  Below macros are used to measure CPU utilization rate of all the AOs
  They are controlled by macro QACTIVE_CPU_CYCLE
*****************************************************************************/
#ifdef QACTIVE_CPU_CYCLE
/** \brief Pointers of the functions which are used to trace the CPU 
* utilization rate of each AO. QA_cpuUrtInit is used to initialize them.
*/
void (*QA_CYCLE_STAT_BEGIN)(uint8_t prio, uint8_t core);
void (*QA_CYCLE_STAT_END)(uint8_t prio, uint8_t core);
void QA_cpuUrtInit(void (*cpu_urt_begin)(uint8_t prio, uint8_t core),
                   void (*cpu_urt_end)(uint8_t prio, uint8_t core));
#else
#define QA_CYCLE_STAT_BEGIN(prio, core)
#define QA_CYCLE_STAT_END(prio, core)
#endif

/****************************************************************************
        Below functions and macros are for QK multi-core extension 
*****************************************************************************/
#ifdef CONFIG_QK
#define NO_CORE_2_DONATE  (INVALID_CORE)

/**
 * Trace the AO preempt status which a new preempt generates
 */
void QK_preemptGenerate(uint8_t c, uint8_t p);

/**
 * Trace the AO preempt status which an old preempt degenerates
 */
void QK_preemptDegenerate(uint8_t c);

/**
 * Update the current preempt AO
 */
void QK_preemptUpdate(uint8_t c, uint8_t p);

/**
 * check if an AO is running or preempted
 */
int QK_isAoRunning(uint8_t c, uint8_t p);

/**
 * check if an AO is worthy to donate to a core
 */
uint8_t QC_isWorthy2Donate(uint8_t p);

/**
 * donate an AO to an idle core
 */
int QC_doWorkDonating(uint8_t cle, uint8_t p);
#endif

#endif
