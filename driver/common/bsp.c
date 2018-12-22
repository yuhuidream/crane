/*****************************************************************************
* Product: "Fly 'n' Shoot" game example with cooperative "Vanilla" kernel
* Last Updated for Version: 4.3.00
* Date of the Last Update:  Nov 01, 2011
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2011 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/

/*This folder contain the driver for QP and board init functions necessary for system to start run, such pin-mux,etc*/

#include "predefines.h"
#include "qp_port.h"
#include "bsp.h"
#include "bbu.h"
#include "mfp.h"
#include "TIME_REG.h"
#include "UART.h"
#include "APBCLK.h"
#include "interrupt.h"
#include "system_mrvl.h"
#include "timer_dec.h"
#include "PMR.h"
#include "PMUA.h"
#include "qf.h"
#include "utils.h"
#include "ICU.h"
#include "CIU.h"

Q_DEFINE_THIS_FILE

#if defined(CONFIG_APB_TIME_TICK)
#if 0
#define SYSTICK_COUNT           16250*2   //5ms@6.5MHz clock
#endif
#if 0
// for mmp3fpga
#define SYSTICK_COUNT           16250*4   //5ms@13MHz clock
#endif
#if 1
#define SYSTICK_COUNT           16250     //5ms@3.25MHz clock for CPU local timers and generic timers
#endif
#endif
extern uint32_t ao_stack_start;
extern uint32_t ao_stack_end;
extern void ao_cycle_record_begin(uint8_t prio, uint8_t core);
extern void ao_cycle_record_end(uint8_t prio, uint8_t core);

static void sys_timer_handle()      //TIME1 timer0
{
#if defined(CONFIG_LOCAL_TIME_TICK)

    if(BU_REG_READ(MP_TMR_INT_STATUS))
    {
        BU_REG_WRITE(MP_TMR_INT_STATUS,1);
    }

#elif defined(CONFIG_APB_TIME_TICK)

    SYSTIME->ICRn[0] |= ICRN_TCLR0_MASK;      //clear timer0 irq
    SYSTIME->IERn[0] &= ~IERN_IE0_MASK;       //disable timer0 irq

#ifndef QF_ONDEMAND_TICK
    SYSTIME->Tn_Mm[0][0] = timer_get_count(0) + SYSTICK_COUNT;  //reset 5ms match value at 3.25MHz

    SYSTIME->IERn[0] |= IERN_IE0_MASK;            //enable timer0 irq
#endif

#else //default use generic timer
    //TBD
#endif
}


#ifdef Q_SPY

    QSTimeCtr QS_tickTime_;
    //QSTimeCtr QS_tickPeriod_;
    static uint8_t l_SysTick_Handler;
   // #define UART_BAUD_RATE      115200
    #define UART_TXFIFO_DEPTH     16

#endif

/*..........................................................................*/
void SysTick_Handler(void *tick) {
    static QEvent const tickEvt = QEVT_INIT(TIME_TICK_SIG, 0, 0);
    (void)tick; /* avoid warning */
#ifdef Q_SPY
    QS_tickTime_ += SYSTICK_COUNT;       /* account for the clock count of one tick */
#endif

#if 0
    //clear the interrupt:the OSSR bits are cleared by writing a one to the proper bit position.
    NEVO_TMR->OSSR |= BIT_0; //clear the interrupt
    NEVO_TMR->OIER &= (~BIT_0); // disable match interrupt.
    NEVO_TMR->OSMR0 = NEVO_TMR->OSCR0 + SYSTICK_COUNT;  //reset 5ms match value at 3.25MHz
    NEVO_TMR->OIER  |= BIT_0; // enable interrupt:Assert OSSR[x] if a match occurs between OSMRx and OSCRx.
#endif
    sys_timer_handle();
#ifdef Q_SPY
    QF_TICK_X(0, &l_SysTick_Handler);                             /* process all armed time events */
    QF_PUBLISH(&tickEvt,&l_SysTick_Handler);      /* publish the tick event to all subscribers */
#else
    QF_TICK_X(0, 0);
    QF_PUBLISH(&tickEvt, 0);      /* publish the tick event to all subscribers */
#endif
}

/*..........................................................................*/
             /* Core local timer is used to driver QK scheduler in SMP mode */
#if defined(CONFIG_QK) && defined(CONFIG_SMP)
void local_timer_handler(void)
{
    if(BU_REG_READ(MP_TMR_INT_STATUS))
    {
        BU_REG_WRITE(MP_TMR_INT_STATUS,1);
    }
    /* this fucntion almost does nothing but to drive QK scheduler */
}

void local_timer_init(void)
{
   /* Make sure our local interrupt controller has this enabled */
    GIC_ISR_ENABLE(GIC_INT_TMR, 15, 0); /* PPIs don't care the target */
    GIC_ISR_CONNECT(GIC_INT_TMR,local_timer_handler, (void *)0);

  /*use preload value later, it will auto-reload when it reaches zero*/
    BU_REG_WRITE(MP_TMR_LOAD,SYSTICK_COUNT);
    BU_REG_WRITE(MP_TMR_CNTR,SYSTICK_COUNT);
    BU_REG_WRITE(MP_TMR_CTRL, MP_TMR_CTRL_ENA|MP_TMR_CTRL_IT_ENA|MP_TMR_CTRL_AUTO_RELD);
}

void local_timer_disable(void)
{
   uint32_t reg;
   GIC_ISR_DISABLE(GIC_INT_TMR);
   GIC_ISR_DISCONNECT(GIC_INT_TMR);
   reg = BU_REG_READ(MP_TMR_CTRL);
   reg &= ~(MP_TMR_CTRL_ENA|MP_TMR_CTRL_IT_ENA|MP_TMR_CTRL_AUTO_RELD);
   BU_REG_WRITE(MP_TMR_CTRL,  reg);
}

#endif

/*..........................................................................*/
void BSP_init(int argc, char *argv[]) {
    if (QS_INIT((void *)0) == 0) {    /* initialize the QS software tracing */
        Q_ERROR();
    }


#ifdef CONFIG_SMP
    QC_getSmpCoreId = smp_processor_id;
    QF_icuInit(irq_target_set, irq_target_get, irq_target_clear, GIC_INT_NUMS);
#endif

#ifdef QF_STACK_PER_AO
    QF_stackInit(ao_stack_start, ao_stack_end);
#endif

#ifdef QACTIVE_CPU_CYCLE
    QA_cpuUrtInit(ao_cycle_record_begin, ao_cycle_record_end);
#endif

#ifdef CONFIG_SMP
    QC_htplgInit(core_sleep_down, wake_up_core);
#endif

/* initialize the whole system,such as clock, pin-mux initial, interrupts, necessary peripherials,etc. */
    SystemInit();

#if 0
    /*
    *only invoke once to init cpu_possible_mask
    */
    possible_cpumask_init();

    /* set core 0 online */
    set_cpu_online(0, 1);
#endif

    QS_RESET(); //should be called after the QS output link (UART now) is ready.

    QS_FUN_DICTIONARY(&l_SysTick_Handler);
    (void)argc;           /* avoid compiler warning about unused parameters */
    (void)argv;
}

#ifdef QF_ONDEMAND_TICK
int qfTickEnabled = 0;
QSpinlock qfTickEnabledLock = Q_SPIN_LOCK_INITIALIZER;
/*..........................................................................*/
int QF_sysTickEnabled(void){
    int ret;
    Q_SPIN_LOCK(&qfTickEnabledLock);
    ret = qfTickEnabled;
    Q_SPIN_UNLOCK(&qfTickEnabledLock);
    return ret;
}

/*..........................................................................*/
uint32_t QF_getAlarmLeft(void)
{
   uint32_t match, tick, count;
   match = SYSTIME->Tn_Mm[0][0];
   count = timer_get_count(0);
   count = (match > count)?(match-count):(0xFFFFFFFF-count+match);
   tick = (count%SYSTICK_COUNT >= (SYSTICK_COUNT/2))?1 : 0;
   tick += (count/SYSTICK_COUNT);
   return tick;
}

/*..........................................................................*/
void QF_setAlarmInterval(uint32_t tick)
{
   SYSTIME->Tn_Mm[0][0] = timer_get_count(0) + (SYSTICK_COUNT*tick);
   SYSTIME->IERn[0] |= IERN_IE0_MASK;
}

/*..........................................................................*/
void QF_sysTickEnable(void){
    if(!QF_sysTickEnabled()){
#if defined(CONFIG_LOCAL_TIME_TICK)
        /* Make sure our local interrupt controller has this enabled */
        GIC_ISR_ENABLE(GIC_INT_TMR, 15, 0);
        GIC_ISR_CONNECT(GIC_INT_TMR,SysTick_Handler, (void *)0);

        /*use preload value later, it will auto-reload when it reaches zero*/
        BU_REG_WRITE(MP_TMR_LOAD,SYSTICK_COUNT);
        BU_REG_WRITE(MP_TMR_CNTR,SYSTICK_COUNT);
        BU_REG_WRITE(MP_TMR_CTRL, MP_TMR_CTRL_ENA|MP_TMR_CTRL_IT_ENA|MP_TMR_CTRL_AUTO_RELD);

#elif defined(CONFIG_APB_TIME_TICK)
        ISR_Connect(SYSTIME_IRQ, SysTick_Handler, NULL);
        INT_Enable(SYSTIME_IRQ, IRQ_ROUTE, 15);

#ifndef QF_ONDEMAND_TICK
        SYSTIME->Tn_Mm[0][0] = timer_get_count(0) + SYSTICK_COUNT;  //first set 5ms match value at 3.25MHz
        SYSTIME->IERn[0] |= IERN_IE0_MASK;            //enable timer0 irq
#endif
#else //default use generic timer
    //TBD
#endif
        Q_SPIN_LOCK(&qfTickEnabledLock);
        qfTickEnabled = 1;
        Q_SPIN_UNLOCK(&qfTickEnabledLock);
    }
}

/*..........................................................................*/
void QF_sysTickDisable(void){
    if(QF_sysTickEnabled()){
#if defined(CONFIG_LOCAL_TIME_TICK)
        GIC_ISR_DISABLE(GIC_INT_TMR);
        GIC_ISR_DISCONNECT(GIC_INT_TMR);
#elif defined(CONFIG_APB_TIME_TICK)
        INT_Disable(SYSTIME_IRQ);
        SYSTIME->IERn[0] &= ~IERN_IE0_MASK; //disable timer0 irq
#else //default use generic timer
        //TBD
#endif
        Q_SPIN_LOCK(&qfTickEnabledLock);
        qfTickEnabled = 0;
        Q_SPIN_UNLOCK(&qfTickEnabledLock);
    }
}
#endif

/*..........................................................................*/
void QF_onStartup(void) {
    bbu_printf("\n\r"BBU_PROMPT);

    QTimer_register(QF_sysTickEnable, 
                    QF_sysTickDisable, 
                    QF_getAlarmLeft, 
                    QF_setAlarmInterval,
                    120000,   //10 minutes
                    0); //RATE 0

#if defined(CONFIG_AUTO)
	 /* let screen test run automatically after system power on */
    POST_CMD2CLI("screen");
#endif
    //POST_CMD2CLI("commt 0 2 0");
}

/*..........................................................................*/
void QF_onCleanup(void) {
}

#ifdef CONFIG_QK
/*..........................................................................*/
void QK_init(void) {
}
#endif

#ifdef CONFIG_VIDEOPLAY_POWER_TEST
static char *lpmset31[3]={"lpmset","3","1"};
static char *lpmset51[3]={"lpmset","5","1"};
#endif

/*........We go into Idle from QF_run last step with QF_INT_DISABLED........*/

extern ComCmdEvt pmuCmdEvt[1];          /* Contains the lpm cmd event info to use in QF_IDLE process*/
extern uint32_t  lpm_lock[1];           /* QF_IDLE don't access lpm when it's locked to set parameters*/

#ifdef CONFIG_CR5
#ifdef CONFIG_UART_AP
int uart_polling = 0;
#else
int uart_polling = 1;
#endif
#endif

#ifdef CONFIG_QK
void QK_onIdle(void) {
#else
void QF_onIdle(void) {        /* entered with interrupts LOCKED, see NOTE01 */
#endif

   //bbu_putchar('a'); for debug UART in poll mode
    /* toggle the User LED on and then off, see NOTE02 */
   // GPIOC->DATA_Bits[USER_LED] = USER_LED;         /* turn the User LED on  */
   // GPIOC->DATA_Bits[USER_LED] = 0;                /* turn the User LED off */
#ifdef CONFIG_CR5
#ifndef CONFIG_CP_ALONE
    if(uart_polling)
        ffuart_fifo_irq_handler();
#endif
#endif

#ifdef Q_SPY

#ifndef CONFIG_QK
    QF_INT_ENABLE();
#endif
    QF_INT_DISABLE();
    Q_SPIN_LOCK(&spy_buf_lock);
    uint16_t fifo = UART_TXFIFO_DEPTH;       /* max bytes we can accept */
    uint8_t const *block;
    block = QS_getBlock(&fifo);    /* try to get next block to transmit */
    while(fifo-- != 0){                    /* any bytes in the block? */
        while(!(QSPY_UART->LSR & LSR_TEMT));
        QSPY_UART->THR = *block++;         /* put into the FIFO */
    }
    Q_SPIN_UNLOCK(&spy_buf_lock);
    QF_INT_ENABLE();



#elif defined NDEBUG /* only if not debugging (idle mode hinders debugging) */
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular MCU.
    */
 //   __WFI();                                          /* Wait-For-Interrupt */
    /*Note: wake up source will start the CPU again*/
#ifndef CONFIG_QK
    QF_INT_ENABLE();
#endif
#else
#ifndef CONFIG_QK
    QF_INT_ENABLE();
#endif
#endif

  /*Here we insert the lpm test to let smp core enter into different low power modes.
  The per core mode type is and enter barrier are global variables controlled by PMU_AO.
  enter barrier is blocked when setting the lpm modes then it's enabled just before PMU_AO return.
 */
   if (0==lpm_lock[0]) {

      lpm_lock[0] = 1;

      lpmset(pmuCmdEvt[0].argc, pmuCmdEvt[0].argv);

#ifndef CONFIG_MP3_POWER_TEST
      screen_cmd_ack(1, pmuCmdEvt[0].scr_cmd_index);
#endif
      /*Note:lpm PASS can't be used by the screen test, if screen_AO can continue to run after UDR,
        it should set PASS*/
   }
}

/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
	 //bbu_printf("Assert@file %s,line %d", file, line);
	bbu_printf("\r\nAssert Error: ");
    bbu_printf("\r\nFile: %s   Line: %d\r\n", file, line);
    bbu_printf("\r\nNot handled! Please Restart...... \r\n");
    bbu_printf("\r\n"BBU_PROMPT);

    mdelay(10); /*enough time to print it out*/

    QF_INT_DISABLE();      /* make sure that all interrupts are disabled */
#ifdef CONFIG_CR5
    cp_assert_cp_handler();
#endif
    for (;;) {       /* NOTE: replace the loop with reset for final version */
    }
}

/*..........................................................................*/
/* error routine that is called if the CMSIS library encounters an error    */
void assert_failed(char const *file, int line) {
    Q_onAssert(file, line);
}

/*--------------------------------------------------------------------------*/
#ifdef Q_SPY
/*..........................................................................*/
uint8_t QS_onStartup(void const *arg) {
    (void)arg;//avoid warning.
    uint8_t qsBuf[256*1024];                  /* buffer for Quantum Spy */
    QS_initBuf(qsBuf, sizeof(qsBuf));
#ifdef Q_SPY_EXT
    memset(&QSAoSet, 0, sizeof(QSAoSet));// set ao set list to 0 at first.
    memset(&QSSmSet, 0, sizeof(QSSmSet));// set sm set list to 0 at first.
#endif

    QS_tickTime_ = SYSTICK_COUNT;        /* to start the timestamp at zero, ref QS_onGetTime() */
   /* setup the QS filters... */
    QS_FILTER_ON(QS_ALL_RECORDS);
//    QS_FILTER_ON(QS_USER);
/*    QS_FILTER_ON(QS_ALL_RECORDS);*/
/*    QS_FILTER_OFF(QS_QF_ACTIVE_ADD);*/
/*    QS_FILTER_OFF(QS_QF_ACTIVE_REMOVE);*/
/*    QS_FILTER_OFF(QS_QF_ACTIVE_SUBSCRIBE);*/
/*//    QS_FILTER_OFF(QS_QF_ACTIVE_UNSUBSCRIBE);*/
/*    QS_FILTER_OFF(QS_QF_ACTIVE_POST_FIFO);*/
/*//    QS_FILTER_OFF(QS_QF_ACTIVE_POST_LIFO);*/
/*    QS_FILTER_OFF(QS_QF_ACTIVE_GET);*/
/*    QS_FILTER_OFF(QS_QF_ACTIVE_GET_LAST);*/
/*    QS_FILTER_OFF(QS_QF_EQUEUE_INIT);*/
/*    QS_FILTER_OFF(QS_QF_EQUEUE_POST_FIFO);*/
/*    QS_FILTER_OFF(QS_QF_EQUEUE_POST_LIFO);*/
/*    QS_FILTER_OFF(QS_QF_EQUEUE_GET);*/
/*    QS_FILTER_OFF(QS_QF_EQUEUE_GET_LAST);*/
/*    QS_FILTER_OFF(QS_QF_MPOOL_INIT);*/
/*    QS_FILTER_OFF(QS_QF_MPOOL_GET);*/
/*    QS_FILTER_OFF(QS_QF_MPOOL_PUT);*/
/*    QS_FILTER_OFF(QS_QF_PUBLISH);*/
/*    QS_FILTER_OFF(QS_QF_NEW);*/
/*    QS_FILTER_OFF(QS_QF_GC_ATTEMPT);*/
/*    QS_FILTER_OFF(QS_QF_GC);*/
/*//    QS_FILTER_OFF(QS_QF_TICK);*/
/*    QS_FILTER_OFF(QS_QF_TIMEEVT_ARM);*/
/*    QS_FILTER_OFF(QS_QF_TIMEEVT_AUTO_DISARM);*/
/*    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM_ATTEMPT);*/
/*    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM);*/
/*    QS_FILTER_OFF(QS_QF_TIMEEVT_REARM);*/
/*    QS_FILTER_OFF(QS_QF_TIMEEVT_POST);*/
/*    QS_FILTER_OFF(QS_QF_CRIT_ENTRY);*/
/*    QS_FILTER_OFF(QS_QF_CRIT_EXIT);*/
/*    QS_FILTER_OFF(QS_QF_ISR_ENTRY);*/
/*    QS_FILTER_OFF(QS_QF_ISR_EXIT);*/

    //QS_FILTER_AO_OBJ(AO_Table);  //need to add

    return (uint8_t)1;                                    /* return success */
}
/*..........................................................................*/
void QS_onCleanup(void) {
}
/*..........................................................................*/
QSTimeCtr QS_onGetTime(void) {            /* invoked with interrupts locked */
#if defined(CONFIG_LOCAL_TIME_TICK)
    if(BU_REG_READ(MP_TMR_INT_STATUS)){
        return QS_tickTime_ + (QSTimeCtr)(SYSTICK_COUNT - BU_REG_READ(MP_TMR_CNTR));
    }
    else {
        return QS_tickTime_ - (QSTimeCtr)(BU_REG_READ(MP_TMR_CNTR));
    }

#elif defined(CONFIG_APB_TIME_TICK)
    if (SYSTIME->SRn[0] & SRN_M0_MASK) {    /* the match occured, but the SysTick_ISR did not run yet */
        return QS_tickTime_ + (QSTimeCtr)(timer_get_count(0) - SYSTIME->Tn_Mm[0][0]);
    }
    else {     /* the match has not occurred yet */
        return QS_tickTime_ - (QSTimeCtr)(SYSTIME->Tn_Mm[0][0] - timer_get_count(0));
    }
#else //default use generic timer
    //TBD
#endif

}
/*..........................................................................*/
void QS_onFlush(void){
    uint16_t fifo = UART_TXFIFO_DEPTH;                     /* Tx FIFO depth */
    uint8_t const *block;
    QF_INT_DISABLE();
    while((block = QS_getBlock(&fifo)) != (uint8_t *)0){
        QF_INT_ENABLE();
                                           /* busy-wait until TX FIFO empty */
        while(!(QSPY_UART->LSR & LSR_TEMT));

        while(fifo-- != 0){                    /* any bytes in the block? */
            QSPY_UART->THR = *block++;                    /* put into the TX FIFO */
        }
        fifo = UART_TXFIFO_DEPTH;              /* re-load the Tx FIFO depth */
        QF_INT_DISABLE();
    }
    QF_INT_ENABLE();
}

#endif                                                             /* Q_SPY */

int platform_type(void)
{
    return (CIU->CHIP_ID >> 24) & 0xF;
}

int platform_is_zebu(void)
{
    return (platform_type() == ZEBU_Z1);
}

int platform_is_z1(void)
{
    return (platform_type() == ZEBU_Z1);
}

int platform_is_haps(void)
{
    return (platform_type() == HAPS);
}

int platform_is_emulator(void)
{
    return (platform_type() == ZEBU_Z1);
}

int platform_is_realsi(void)
{
    return (platform_type() == REAL_SI);
}

/*--------------------------------------------------------------------------*/
/*****************************************************************************
* NOTE01:
* The QF_onIdle() callback is called with interrupts locked, because the
* determination of the idle condition might change by any interrupt posting
* an event. QF::onIdle() must internally unlock interrupts, ideally
* atomically with putting the CPU to the power-saving mode.
*
* NOTE02:
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/
