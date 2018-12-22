/*****************************************************************************
* Product: BBU
*
* Purpose: PMU AO
*  This AO should handle the following tests/task in PMU,
*
*  +DVFM:setv,pp_change, PLL_change
*  +LPM: entry and exit(wakeup) test; per system wide, each unit should keep its
*  own lowest power state while the test here will also call the lpm_pre and lpm_post functions.
*  This has many core/device/system low power modes (C/CGM/D/S) entry and exit sequences.
*  esp. The D0CGM mode of AP subsys for CP to access DDR. clock gating, GPU/video/modem on/off
*
*  +Profiler and Policy:this monitor the PP duty cycle and generate the adjust event for DVFM and LPM.
*  over-heating need to be considered, PMCR provides the status bit.
*  +Wakeup managment: wake up sources enable/disable, interrupt and send event to LPM exit sequences.
*
*  +Reset managment and test: different reset sources and effects
*
*  +Static process compensation: per each chip
*
*  +Stress test: PP_change time, LPM entry/exit, reset
*  +Memory: R/W test during different PP(may not included here); retention test during LPM process
*  +Limit test: Vmin(both active and D2), Fmax
*
*  Besides the above tests, we need to enable the system sleep/wake with some policies, such as,
*  uart wake up hold a 5s lock,etc.  This can be disabled at the initial bring up stage of a new chip.
*  And this may be distributed in the idle process and wake up sources driver code.
*
* Copyright (C) 2010-2012 Marvell MTBL-Hefei. All rights reserved.
*
* Author: Lianghu Xu  email: xlianghu@marvell.com
*
*****************************************************************************/

#include "qp_port.h"
#include "bsp.h"
#include "bbu.h"
#include "ppset.h"
#include "pmu_dec.h"
#include "utils.h"

#include <string.h>


Q_DEFINE_THIS_FILE

//@@@@@@@@@@@@@@@FIXME: design a generic/unified pmu framework for BBU@@@@@@@@@@@@@@@@
/*..........................................................................*/
typedef struct pmutag {                      /* the pmu active object */
    QActive super;                           /* derive from QActive */
    //other data                             /* just start with simple test, will be evolved gradually*/
} pmu;

ComCmdEvt pmuCmdEvt[1];          /* Contains the lpm cmd event info to use in QF_IDLE process*/
uint32_t  lpm_lock[1];           /* QF_IDLE don't access lpm when it's locked to set parameters*/
                                             /* hierarchical state machine ... */
static QState pmu_initial       (pmu *me, QEvent const *e);
static QState pmu_processing    (pmu *me, QEvent const *e);


static pmu l_pmu;                            /* the sole instance of the pmu active object */

/* Public-scope objects----------------------------------------------------*/
QActive * const AO_pmu = (QActive *)&l_pmu; /*opaque pointer to PMU AO*/

/*..........................................................................*/
void pmu_ctor(void) {
    pmu *me = &l_pmu;
    QActive_ctor(&me->super, (QStateHandler)&pmu_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState pmu_initial(pmu *me, QEvent const *e) {
    (void)e;               /* avoid compiler warning about unused parameter */
    if(platform_is_realsi()){
        pp_init();

/*Note: This should run after pp_init which enabled PMIC DVC function!*/
        //lpm_init();
        fab_gating_init();
    }
    if(!platform_is_haps())
        lpm_init();
    lpm_lock[0] = 1;

    return Q_TRAN(&pmu_processing);
}

/*..........................................................................*/
QState pmu_processing(pmu *me, QEvent const *e) {
    //uint32_t pp, lpm;
    ComCmdEvt *cmdEvt = (ComCmdEvt *)e;
    switch (e->sig) {
      case Q_INIT_SIG: {
            //bbu_printf("\r\n"BBU_PROMPT);
            return Q_HANDLED();
        }
      case PMU_PPSET_SIG:{
         if(bbu_ppset_test(cmdEvt->argc, cmdEvt->argv) == 1)
            screen_cmd_ack(1, cmdEvt->scr_cmd_index);
         bbu_printf(BBU_PROMPT);
         return Q_HANDLED();
      }
      case PMU_OPSET_SIG:{
         //if(bbu_opset_test(cmdEvt->argc, cmdEvt->argv) == 1)
            screen_cmd_ack(1, cmdEvt->scr_cmd_index);
         bbu_printf(BBU_PROMPT);
         return Q_HANDLED();
      }
      case PMU_IPWK_SIG:{
         //if(ipwk_test() == 1)
            screen_cmd_ack(1, cmdEvt->scr_cmd_index);
         bbu_printf(BBU_PROMPT);
         return Q_HANDLED();
      }      
      case PMU_LPMSET_SIG:{
#ifndef CONFIG_MP3_POWER_TEST
         bbu_printf("\n\r");
#endif
         switch(cmdEvt->argc){
            case 1:{
               bbu_printf("lpmset--> No lpm mode specified.\n\r");
               break;
            }

            default:{
                   if(check_help(cmdEvt->argv[1])){
                      bbu_lpmset_help();
                   }
                   else{
                      int offset;
                      char *phChar;
                      lpm_lock[0] = 1; //lock the lock
                      memcpy(&(pmuCmdEvt[0]), cmdEvt, sizeof(ComCmdEvt));
                      phChar = &(pmuCmdEvt[0].cmdline[0]);
                      while(*phChar == 0)
                          phChar ++ ;
                      offset = (int)phChar - (int)pmuCmdEvt[0].argv[0];
                      for(uint32_t i = 0; i < pmuCmdEvt[0].argc; i++){
                         pmuCmdEvt[0].argv[i] += offset;
                      }
                      lpm_lock[0] = 0; //release the lock
                      //lpmset(cmdEvt->argc, cmdEvt->argv); //move to call in QF_IDLE
                      //The judgement should be done after wake up in QF_IDLE
                   }
               break;
            }
         }
#ifndef CONFIG_MP3_POWER_TEST
         bbu_printf(BBU_PROMPT);
#endif
         return Q_HANDLED();
      }
      case PMU_MSA_SIG:{
          msa_tests(cmdEvt->argc, cmdEvt->argv);
          screen_cmd_ack(1, cmdEvt->scr_cmd_index);
          return Q_HANDLED();
      }
    }
    return Q_SUPER(&QHsm_top);
}




