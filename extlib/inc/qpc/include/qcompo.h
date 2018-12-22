#ifndef h_qcompo_h
#define h_qcompo_h

/** Both the component level suspend-wakeup and transfer need stack-per-
* component support. We have two options here, 1. build-in defer queue, 
* 2, outside defer queue(we can use it through QCompo_start() function).
* Currently, we enable the build-in defer queue by define 
* QCOMPO_INTERNAL_QUEUE, its size is defined by macro DEFER_QUEUE_LEN.
*/
#define QCOMPO_INTERNAL_QUEUE
#define DEFER_QUEUE_LEN   10

#define INVALID_COMPONENT ((QCompo *)0)

#define QA_EXIT_STATE    1
#define QA_CONT_STATE    0

/** Component types when enabling component transfer between AOs
*/
typedef enum{
    UNTRANSFERABLE = 0,
    TRANSFERABLE,
    TRANSFERRED
}QCompoType;

/** Structure of extended component, currently, it support component level
* suspend-wakeup/transfer/event-publish-subscribe mechanism
*/
typedef struct QCompoTag {
    QHsm            super;
    QF_EQUEUE_TYPE  defQueue;

    uint32_t        stk;
    uint32_t        stkSize;

    uint8_t         status;

#ifdef COMPOSITE_LEVEL_SUSPEND 
                          /* This point to a buffer used to save registers */
    QRegCntxt       *context;
#endif

    QActive         *owner;

                       /* This point to a buffer used to save stack context */
    uint32_t        *stack; 

#ifdef QCOMPO_INTERNAL_QUEUE
    QEvt    const *deferQSto[DEFER_QUEUE_LEN];
#endif

    int             result;

    QCompoType      type;
} QCompo;

typedef struct QCompoSubscrEntryTag{
   QCompo *compo;
   struct QCompoSubscrEntryTag *next;
}QCompoSubscrEntry;

typedef struct QCompoSubscrListTag{
   QCompoSubscrEntry *head;

#ifdef CONFIG_SMP
   QSpinlock lock;
#endif
}QCompoSubscrList;

/** \brief This macro is used to construct a component
*/
#define QCompo_ctor(me_, initial_)  QHsm_ctor(&(me_)->super, initial_)

/** \brief This macro is used to defer the events which is dispatched
* to a sleeping component by its container AO.
*/
#define QCompo_defer(me_, e_)        \
        QEQueue_postFIFO( &( (me_)->defQueue ), e_)

/** \brief This macro is used to recall all the deferred events before an
* container AO begins to wake up its sleeping child component which these
* events belong to.
*/
#define QCompo_recall(act_, me_)     \
        QActive_recallAll((act_), &((me_)->defQueue) )


#ifdef COMPOSITE_LEVEL_SUSPEND
/** \brief This macro is used to save the AO stack position before AO 
* begins to dispatch an event to a child component
*/
#define QF_SAVE_AO_SP(me_)   \
   do{      \
      __asm__ __volatile__(   \
        "mov  r1, %0\n"       \
        "str  sp, [r1, #0]\n"    \
        :         \
        :"r" (&((me_)->stkBk))    \
        :"r1"     \
      );          \
   }while(0)

/** \brief This macro is used to switch to component stack when AO begins
* to dispatch an event to a child component
*/
#define QF_STK_AO_2_COMPO(compo_) \
   do{      \
      __asm__ __volatile__(      \
         "mov  r1,  %0\n"     \
         "ldr  sp, [r1, #0]\n"      \
         :     \
         :"r" (&compo_->stk)      \
         :"r1"       \
      );    \
   }while(0)

/** \brief This macro is used to switch to AO stack when AO has finished
* dispatching an event to a child component
*/
#define QF_STK_COMPO_2_AO(ao_) \
   do{      \
      __asm__ __volatile__(      \
         "mov  r1,  %0\n"     \
         "ldr  sp, [r1, #0]\n"      \
         :     \
         :"r" (&(ao_->stkBk))      \
         :"r1"       \
      );    \
   }while(0)

/** \brief This macro is used to save the general registers before a component
* go to suspend
*/
#define  QCompo_contextSave(me_)       \
   do{   \
      __asm__ __volatile__(   \
         "mov   r2, %0\n"  \
         "mrs   r0, cpsr\n"   \
         "mov   r1, pc\n"  \
         "stmed r2!, {r0-r1, r3-r14}\n"   \
         :  \
         :"r"(&((me_)->context->lr))   \
         :"r0", "r1", "r2" \
      ); \
   }while(0)

/** \brief This macro is used to restore the general registers before an
* container AO begins to wake up its sleeping child component
*/
#define   QCompo_contextRestore(comp)   \
   do{   \
      __asm__ __volatile__(   \
         "mov  r0, %0\n"   \
         "mov  r2, %1\n"   \
         "mov  r1, %2\n"   \
         "ldr  r0, [r0, #0]\n"  /* dest, pointer to backup buffer */  \
         "ldr  r2, [r2, #0]\n"  /* dest + len */   \
         "ldr  r1, [r1, #0]\n"  /* source */   \
         "sub  r2, r2, r0\n"     /* len */  \
         "mov  r4, %3\n"            \
            /* only r0-r3 and r12 used in this function */  \
         ".extern memcpy_use_neon\n"  \
         "bl memcpy_use_neon\n"   \
         "mov   r0,  r4\n"   \
         "ldmda r0, {r1-r2, r3-r12, sp, lr}\n"  \
         "msr   cpsr_c, #(0x12 | 0xC0)\n"   \
         "msr   spsr_cxsf, r1\n"   \
         "mov   lr, r2\n"   \
         "movs  pc, lr\n"   \
         :   \
         :"r"(&((comp)->owner->stkBk)),           \
          "r"(&((comp)->owner->stk)),         \
          "r"(&((comp)->stack)),   \
          "r"(&((comp)->context->lr))      \
         :"r0", "r1", "r2", "r3", "r4", "r12"  \
      );  \
      while(1);  /* should never reach here */  \
   }while(0)

/** \brief define an event which is used to wake up a suspended component
*/
typedef struct CompoWakeEvtTag {
    QEvt  super;
    QCompo *compo;
} CompoWakeEvt;

/** \brief This macro is used at the end of processing state of a container AO
* before return Q_SUPER(). It will check if unhandled event is component-
* subscribed event and dispatch it to all the components which subscribe it.
*/
#define QCOMPO_SCHEDULAR_EXIT(e_)  \
   do{                           \
      if(QCompo_isSubscrEvt(e_)){  \
         QCompo_subscrEvtDispatch(e_);           \
         return Q_HANDLED();   \
      }                             \
   }while(0)

extern void *memcpy_use_neon(void *dest, const void *src, int n);

/** \brief This function is used to start an extended component
*/
void QCompo_start(QCompo *const me, QCompoType type, 
                  QEvt const *qSto[], uint32_t qLen,
                  void *stkSto, uint32_t stkSize,
                  QActive *owner
                  );

/** \brief This function is used to stop an extended component
*/
void QCompo_stop(QCompo *const me);

/** \brief check if the component is suspended
*/
int QCompo_isSuspend(QCompo *me);

/** \brief wake up a component if the event is a CompoWakeEvt
*/
void QCOMPO_SCHEDULAR_ENTRY(QEvt const * const e);

/** \brief kick a suspended component and send a response of its request to it
*/
void QCOMPO_KICK_SEND_RESULT(QCompo *compo, int result);

/** \brief set the current running component of a core
*/
void QC_setCurrentCompo(uint8_t c, QCompo *me);

/** \brief clear the current running component of a core
*/
void QC_clearCurrentCompo(uint8_t c);

/** \brief get the current running component of a core
*/
QCompo *QC_getCurrentCompo(uint8_t c);

/** \brief check if a core is handling a component
*/
int QC_isHandlingCompo(uint8_t c);

/** \brief an component go to sleep and let the core schedule others
*/
void QCompo_schedOther(QCompo *comp, uint8_t c);

/** \brief suspend current component and wait for others to resume it
*/
int QCOMPO_SUSPEND_WAIT4RESUME(void);

/** \brief component wake up after it owner AO receive a wakeup event
*/
void QCompo_wakeup(QCompo *comp, QEvt const * const e);

/** \brief dispatch event to a component 
*/
void QCompo_dispatch(QCompo * const me, QEvt const * const e);

#endif    /* end of 'ifdef COMPOSITE_LEVEL_SUSPEND' */

extern QCompoSubscrList *QCompo_subscrList_;
extern enum_t QCompo_maxSignal_;

/** \brief This function is used to initialize the component level 
* publish-subscribe mechanism. 
*/
void QCompo_psInit(QCompoSubscrList * const subscrSto, 
                   uint32_t const subscrStoLen);

/** \brief This function is used to publish a component-subscribed event
*/
#ifndef Q_SPY
void QCompo_publish(QEvt const * const e);
#else
void QCompo_publish(QEvt const * const e, void const * const sender);
#endif

/** \brief This function is used by a component to subscribe an event
*/
void QCompo_subscribe(QCompo const * const me, enum_t const sig);

/** \brief This function is used by a component to unsubscribe an event
*/
void QCompo_unsubscribe(QCompo const * const me, enum_t const sig);

/** \brief This function is used by a component to unsubscribe all its 
* subscribed events
*/
void QCompo_unsubscribeAll(QCompo const * const me);

/** \brief This function is used check if a event is component-subscribed 
* event
*/
int QCompo_isSubscrEvt(QEvt const * const e);

/** \brief This function is used by a container AO to dispatch a component-
* subscribed event
*/
void QCompo_subscrEvtDispatch(QEvt const * const e);

/** \brief this function and its according macro QACTIVE_SCHEDULE_ENTRY are
* used at the entry of a container AO.
*/
int QActive_scheduleEntry(QActive * const me, QEvt const * const e);
#define QACTIVE_SCHEDULE_ENTRY(me_, e_)   \
   do{                       \
      if(QActive_scheduleEntry((QActive *)me_, e_) == QA_EXIT_STATE)   \
         return Q_HANDLED();     \
   }while(0)


/** \brief this function is used by a container AO to snoop its event queue,
* return 1 if this AO has more than one event which belongs to different
* components.
*/
int QActive_snoopQueue(QActive * const me, QEvt const * const e);

/** \brief this function is used by a container AO to throw its current event
* to an container AO of an idle CPU. return 1 if successfully to throw, else
* return 0.
*/
int QActive_pushCompo2Idle(QActive * const me, QEvt const * const e);

/** \brief when a helper AO finish dispatching an event to a compoent which
* doesn't belong to it, it will send a return event to the component owner 
* to let it get the component back.
*/
void QActive_returnCompo(QCompo * const me, uint8_t c);

#endif                        /* end of 'ifndef h_qhsm_comp' */
