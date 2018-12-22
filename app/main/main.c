/*****************************************************************************
* Product: "Fly'n'Shoot" game example
* Last Updated for Version: 4.0.04
* Date of the Last Update:  Aug 01, 2009
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
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
#include "qp_port.h"
#include "bsp.h"
#include "bbu.h"
#include "bbu_malloc.h"
#include "mrvl_amp.h"

/* Local-scope objects -----------------------------------------------------*/
#if !defined(CONFIG_DBG) 
static QEvent const *l_consoleQSto[252]; /* Event queue storage for Console */
#endif
static QEvent const *l_cliQSto[32];          /* Event queue storage for Cli */
static QEvent const *l_dmaQSto[16];          /* Event queue storage for DMA */
static QEvent const *l_timerQSto[16];      /* Event queue storage for Timer */
static QEvent const *l_commonQSto[16]; /* Event queue storage for COMMON AO */
static QEvent const *l_i2cQSto[16];          /* Event queue storage for I2C */
static QEvent const *l_pmuQSto[8];           /* Event queue storage for PMU */
static QEvent const *l_usbQSto[8];           /* Event queue storage for USB */
static QEvent const *l_stressQSto[16]; /* Event queue storage for STRESS AO */
static QEvent const *l_schedQSto[16];   /* Event queue storage for SCHED AO */
static QEvent const *l_ipcQSto[16];       /* Event queue storage for IPC AO */
/*The common event pools with different size*/
static KeypEvt     l_uPoolSto[NHISTORY + 32];           /* micro event pool */
static ComCliEvt   l_smlPoolSto[NHISTORY + 64 + 128+48];/* small event pool */
static ComCmdEvt   l_medPoolSto[NHISTORY + 32];        /* medium event pool */

static QSubscrList    l_subscrSto[MAX_PUB_SIG];
static QCompoSubscrList l_compoSubscrSto[MAX_COMPO_PUB_SIG - MAX_PUB_SIG];
/*..........................................................................*/
int main(int argc, char *argv[]) {

                          /* explicitly invoke the active objects' ctors... */
    I2C_ctor(); //I2C should be at first
#if !defined(CONFIG_DBG)                              
    Console_ctor();
#endif:
    Timer_ctor();

#ifndef CONFIG_MMT
    USB_ctor();
#endif
    
    STRESS_ctor();
    SCHED_ctor();

    Cli_ctor();
    COMMON_ctor();
    IPC_ctor();

    BSP_init(argc, argv);           /* initialize the Board Support Package */
    pmu_ctor(); //after the XTC settings set, should set those XTC in OBM?

    //BBU_mallocInit();
    //mem_hotplug_init(); /*this func invokes bbu_malloc, so should be after BBU_mallocInit*/

    QF_init();     /* initialize the framework and the underlying RT kernel */

                                           /* initialize the event pools... */
    QF_poolInit(l_uPoolSto, sizeof(l_uPoolSto), sizeof(l_uPoolSto[0]));
    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));

    QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));   /* init publish-subscribe */
    QCompo_psInit(l_compoSubscrSto, Q_DIM(l_compoSubscrSto));

                /* send object dictionaries for event pools... */
    QS_OBJ_DICTIONARY(l_uPoolSto);
    QS_OBJ_DICTIONARY(l_smlPoolSto);
    QS_OBJ_DICTIONARY(l_medPoolSto);
#ifdef CONFIG_FAT32
    QS_OBJ_DICTIONARY(l_bigPoolSto);
#endif

               /* send signal dictionaries for globally published events... */
    QS_SIG_DICTIONARY(TIME_TICK_SIG, 0);

#if !defined(CONFIG_DBG)
    QACTIVE_START(AO_Console,                          
                  4,                                            /* priority */
                  l_consoleQSto, Q_DIM(l_consoleQSto),         /* evt queue */ 
                  (void *)0, 4,                     /* has per-thread stack */
                  (QEvent *)0,                   /* no initialization event */
                  0);                                       /* initial core */ 
#endif
     QACTIVE_START(AO_SCHED,                                                              
                  5,                                            /* priority */
                  l_schedQSto, Q_DIM(l_schedQSto),             /* evt queue */ 
                  (void *)0, 8,                     /* has per-thread stack */ 
                  (QEvent *)0,                   /* no initialization event */
                  0);
    QACTIVE_START(AO_STRESS,                                                              
                  6,                                            /* priority */
                  l_stressQSto, Q_DIM(l_stressQSto),           /* evt queue */ 
                  (void *)0, 30,                    /* has per-thread stack */ 
                  (QEvent *)0,                   /* no initialization event */
                  1);                                       /* initial core */         
                                  /* initial core */               
    QACTIVE_START(AO_Timer,                                                              
                  10,                                           /* priority */ 
                  l_timerQSto, Q_DIM(l_timerQSto),             /* evt queue */ 
                  (void *)0, 4,                     /* has per-thread stack */ 
                  (QEvent *)0,                   /* no initialization event */
                  0);                                       /* initial core */
    QACTIVE_START(AO_I2C,                                              
                  11,                                          /* priority */
                  l_i2cQSto, Q_DIM(l_i2cQSto),                 /* evt queue */
                  (void *)0, 4,                     /* has per-thread stack */
                  (QEvent *)0,                   /* no initialization event */
                  0);                                       /* initial core */   

     QACTIVE_START(AO_COMMON,                                                              
                  18,                                           /* priority */
                  l_commonQSto, Q_DIM(l_commonQSto),           /* evt queue */ 
                  (void *)0, 8,                     /* has per-thread stack */ 
                  (QEvent *)0,                   /* no initialization event */
                  0);                                       /* initial core */

    QACTIVE_START(AO_pmu,                                                              
                  19,                                           /* priority */ 
                  l_pmuQSto, Q_DIM(l_pmuQSto),                 /* evt queue */ 
                  (void *)0, 4,                     /* has per-thread stack */ 
                  (QEvent *)0,                   /* no initialization event */
                  0);                                       /* initial core */
#ifndef CONFIG_MMT
    QACTIVE_START(AO_USB,                                                              
                  20,                                           /* priority */ 
                  l_usbQSto, Q_DIM(l_usbQSto),                 /* evt queue */ 
                  (void *)0, 4,                     /* has per-thread stack */ 
                  (QEvent *)0,                   /* no initialization event */
                 0);                                       /* initial core */
#endif
   QACTIVE_START(AO_IPC,                                                              
                  22,                                           /* priority */ 
                  l_ipcQSto, Q_DIM(l_ipcQSto),                 /* evt queue */ 
                  (void *)0, 4,                     /* has per-thread stack */ 
                  (QEvent *)0,                   /* no initialization event */
                  0);

    QACTIVE_START(AO_Cli,                                                              
                  30,                                           /* priority */
                  l_cliQSto, Q_DIM(l_cliQSto),                 /* evt queue */ 
                  (void *)0, 4,                     /* has per-thread stack */ 
                  (QEvent *)0,                   /* no initialization event */
                  0);                                       /* initial core */

#ifdef Q_SPY_EXT
    bbu_printf("QS_TIMESTAMP\r\n");
    bbu_printf(BBU_PROMPT" %d ms", QS_onGetTime()/3250);
    bbu_printf("\r\n"BBU_PROMPT);
#endif
#ifdef CONFIG_MMT
    bbu_putchar(0x86);
#endif

#ifdef CONFIG_MMT
    bbu_output_init2 ();
#endif
//    usb_reset_hsic();                          /*for fix bootrom init hsic bug */
//    fix_power_leakage();                        /*for 28nm usb AXI clock enable concern*/

    QF_run();                                     /* run the QF application */

    return 0;                                   /*Note:QF_run never returns*/
}
