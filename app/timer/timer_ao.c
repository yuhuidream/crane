#include "timer_dec.h"
#include "bbu_help.h"

Q_DEFINE_THIS_FILE

typedef struct TimerAoTag{
    QActive super;                              
}Timer;    

static QState Timer_initial(Timer *me, QEvent const *e);
static QState Timer_process(Timer *me, QEvent const *e);

static Timer l_timer;
QActive * const AO_Timer = (QActive *)&l_timer;

/* Timer Construction */
void Timer_ctor(void){
    Timer *me = &l_timer;
    QActive_ctor(&me->super, (QStateHandler)&Timer_initial);
}

QState Timer_initial(Timer *me, QEvent const *e){
    (void)e;

    QS_OBJ_DICTIONARY(&l_timer);

    QS_FUN_DICTIONARY(&Timer_initial);
    QS_FUN_DICTIONARY(&Timer_process);

    QS_SIG_DICTIONARY(TIMER_FROM_COM_SIG, &l_timer);
    QS_SIG_DICTIONARY(TIMER_SIG, &l_timer);
    /* Connect interrupt to this AO, when AO transfers, the interrupt be
    * rerouted
    */
    //QA_IRQ_CONNECT(IRQ_MMP_RTC_ALARM,  AO_Timer);
    //QA_IRQ_CONNECT(IRQ_MMP_WDT,        AO_Timer);

#ifdef CONFIG_MMT
    //enable watchdog in ppset.c ->pmic_dvc_init first
  	start_timer(5000, pmic_watchdog_timer_reset, 0);
#endif
    return Q_TRAN(&Timer_process);
}

QState Timer_process(Timer *me, QEvent const *e){
    switch(e->sig){
        case TIMER_FROM_COM_SIG:{    
            ComCmdEvt *evt = (ComCmdEvt *)e;
            timer_test(evt->argc, evt->argv);

            return Q_HANDLED();
        }
        case TIMER_RTC_SIG:{
            ComCmdEvt *evt = (ComCmdEvt *)e;
            rtc_test(evt->argc, evt->argv);

            return Q_HANDLED();
        }
        case TIMER_HANDLE_SIG:{
            TimerIrqEvt *evt = (TimerIrqEvt*)e;
            timer_irq_func func = evt->func_p;
            void* func_arg = evt->arg;
            if(func)
                func(func_arg);

            return Q_HANDLED();
        }
        case TIMER_SIG:{
            return Q_HANDLED();
        }
    }   
    return Q_SUPER(&QHsm_top);
}



