/*****************************************************************************
* Product: BBU
* 
* Purpose: Stress Test Schedular AO
* 
* Copyright (C) 2010-2011 Marvell MTBL-Hefei. All rights reserved.
*
* Author: Yu Zhang  email: zhangy@marvell.com
* 
*****************************************************************************/
#include "predefines.h"
#include "qp_port.h"
#include "bbu.h"
#include "prime_test.h"
#include "stress.h"
#include "clock.h"
#include "utils.h"
#include "timer_dec.h"
#include "bsp.h"
#include "ipc_app.h"
#include <stdlib.h>
#include <string.h>

Q_DEFINE_THIS_FILE

#define STRESS_TEST_TIME   (3*60)                             /* 3 minutes */
#define UPPER_STEP_TIME    (5*60)

/* sched Active Object ....................................................*/
typedef struct SCHEDTag {
   QActive super;                              /* extend the QActive class */

   uint8_t cmdIdx;
   uint8_t cmdNum;
   uint8_t periodicNum;
   uint8_t autoRun;

   uint8_t  strsCont;
   uint8_t  testType;
   uint8_t  duration;
   uint8_t  cowkTest;
   uint8_t  cowkTestRslt;
   uint32_t lastTime;
   uint32_t testTime;
   uint32_t scr_index;

   QTimeEvt testOver;
   QTimeEvt displayDelay; 
   QTimeEvt contEvt;
   QTimeEvt checkEvt;
   char failInfo[32];
   StressCmdEntry *stressCmd;
}SCHED;

/* hierarchical state machine ............................................. */
static QState sched_initial   (SCHED *me, QEvent const *e);
static QState sched_idle      (SCHED *me, QEvent const *e);
static QState sched_processing(SCHED *me, QEvent const *e);
static QState sched_pass      (SCHED *me, QEvent const *e);
static QState sched_fail      (SCHED *me, QEvent const *e);
static QState sched_master_pass      (SCHED *me, QEvent const *e);
static QState sched_master_fail      (SCHED *me, QEvent const *e);

/* common static functions................................................. */
static void stress_begin(int test_type);
static char* get_test_name(int test_type);
static void display_stress_pass(int test_type);
static void display_stress_fail(int test_type);
static void stop_ongoing_tests(SCHED *me);
static void display_stress_step_fail(int test_type);
static void display_stress_step_pass(int test_type);
static void reset_stress_test(SCHED *me);
static void init_cmd_array(SCHED *me, int test_type);
static void display_stress_list(StressType type);
static StressType get_test_type(char *cmd);

extern int vbuck1_self_incre_decre(void);
extern int vbuck1_self_increase(void);
extern int set_lowest_vol_for_vbuck1_self_change(uint32_t low);
extern void vpu_freq_rtc_self_change(void);


static SCHED l_sched;       /* the sole instance of the SCHED active object */

/* global objects.......................................................... */
QActive * const AO_SCHED = (QActive *)&l_sched;    /*opaque pointer to sched*/

/*..........................................................................*/
void SCHED_ctor(void) {
   SCHED *me = &l_sched;
   QActive_ctor(&me->super, (QStateHandler)&sched_initial);
   /* If no fail evt received before this timeEvt 
      triggerred, the test passes */
   QTimeEvt_ctor(&me->testOver,     STRESS_PASS_SIG);
   QTimeEvt_ctor(&me->displayDelay, STRESS_OVER_SIG);
   QTimeEvt_ctor(&me->contEvt,      STRESS_CONT_SIG);
   QTimeEvt_ctor(&me->checkEvt,     STRESS_CHECK_SIG);

}

/*..........................................................................*/
static QState sched_initial(SCHED *me, QEvent const *e){
   (void)e;                  /* avoid the "unreferenced parameter" warning */
   me->strsCont = 0;
   me->lastTime = STRESS_TEST_TIME;
   me->testTime = STRESS_TEST_TIME;
   me->testType = DDR_STRESS;
   me->cowkTest = TESTALONE;
   me->cowkTestRslt = CO_TEST_GOON;
   memset(me->failInfo, 0, sizeof(me->failInfo)/sizeof(char));
   return Q_TRAN(&sched_idle);
}

/*..........................................................................*/   
static QState sched_idle(SCHED *me, QEvent const *e){
   (void)me;
   ComCmdEvt *cmdEvt = (ComCmdEvt *)e;
   switch(e->sig){
      case Q_ENTRY_SIG:{
         return Q_HANDLED();
      }
      case STRESS_STRT_SIG:{
         switch(cmdEvt->argc){
            case 1:{
               me->strsCont = 1;
               me->scr_index = cmdEvt->scr_cmd_index;
               init_cmd_array(me, me->testType);
               bbu_printf("me->testType %d\n\r", me->testType);
               if(me->testType == CW_STRESS)
               {
                  me->cowkTest = TESTSLAVE;
               }
               if(me->cmdNum == 0){
                  bbu_printf("The test command array is empty\n\r");
                  return Q_HANDLED();
               }
               if(me->testType != CW_STRESS)
                  stress_begin(me->testType);
               static QEvt const contEvt = QEVT_INIT(STRESS_CONT_SIG, 0, 0);
               QACTIVE_POST(AO_SCHED, (QEvt *)&contEvt, me);
               bbu_printf(DBG_MSG "POST testOver TimeEvt\n\r");
               if(me->lastTime > UPPER_STEP_TIME){
                  QTimeEvt_postIn(&me->testOver, (QActive *)me,
                                UPPER_STEP_TIME*BSP_TICKS_PER_SEC);
                  me->lastTime -= UPPER_STEP_TIME;
               }else{
                  QTimeEvt_postIn(&me->testOver, (QActive *)me,
                                me->lastTime*BSP_TICKS_PER_SEC);
                  me->lastTime = 0;
               }
               //print_current_time();
               return Q_TRAN(&sched_processing);
            }
            case 2:{
               if(check_help(cmdEvt->argv[1])){
                  bbu_stress_help();                           /* stress /h */
                  return Q_HANDLED();
               }else if(!strcmp((char*)cmdEvt->argv[1],"init")){
                  reset_stress_test(me);                     /* stress init */
                  bbu_printf("Stress test is initialized\n\r");
                  return Q_HANDLED();
               }else if(!strcmp((char*)cmdEvt->argv[1],"abort")){
                  QEvt *abtEvt = Q_NEW(QEvt, STRESS_PASS_SIG);
                  QACTIVE_POST(AO_SCHED, (QEvt *)abtEvt, me);
                  bbu_printf("Stress test will be terminated immediately\n\r");
                  return Q_HANDLED();
               }else if(!strcmp((char*)cmdEvt->argv[1],"auto")){
                  me->autoRun = 1;                          /* stress auto */
                  cmdEvt->argc = 1;
                  vbuck1_self_incre_decre();
                  QACTIVE_POST(AO_SCHED, (QEvt *)e, me);
                  return Q_HANDLED();
               }else{
                  StressType type = get_test_type((char*)cmdEvt->argv[1]);
                  if(type == INVALID_STRESS){
                     uint32_t time = conv_dec((char *)cmdEvt->argv[1])*60;
                     if(time){
                        me->lastTime = time;
                        me->testTime = time;
                     }
                     else{
                        bbu_printf("Unsupported test type\n\r");
                        bbu_stress_help(); 
                        return Q_HANDLED();
                     }
                  }else{
                     me->testType = (uint8_t)type;
                  }
               }
               cmdEvt->argc = 1;    /* repost current event and goto case 1 */
               QACTIVE_POST(AO_SCHED, (QEvt *)e, me);
               break;
            }
            case 3:{
               if(!strcmp((char*)cmdEvt->argv[1],"auto")){
                  vbuck1_self_incre_decre();           /* stress auto <min> */
                  me->autoRun = 1; /* auto run stress for default type(ddr) */
                  uint32_t time = conv_dec((char *)cmdEvt->argv[2])*60;
                  if(time)
                     me->lastTime = time;
                  else{
                     bbu_printf("Unsupported test type\n\r");
                     bbu_stress_help(); 
                     return Q_HANDLED();
                  }
               }else{
                  StressType type = get_test_type((char*)cmdEvt->argv[1]);
                  if(type == INVALID_STRESS){
                     bbu_printf("Unsupported test type\n\r");
                     bbu_stress_help(); 
                     return Q_HANDLED();
                  }else{
                     me->testType = (uint8_t)type;
                     if(!strcmp((char*)cmdEvt->argv[2],"auto")){
                        me->autoRun = 1;                    /* stress ddr auto */
                        vbuck1_self_incre_decre();
                     }else if(!strcmp((char*)cmdEvt->argv[2],"list")){
                         display_stress_list(type);
                         bbu_printf(BBU_PROMPT);
                         return Q_HANDLED();
                     }else{
                        uint32_t time = conv_dec((char *)cmdEvt->argv[2])*60;
                        if(time){
                           me->lastTime = time;
                           me->testTime = time;
                        }
                        else{
                           bbu_printf("Unsupported option %s\n\r", (char*)cmdEvt->argv[2]);
                           bbu_stress_help(); 
                           return Q_HANDLED();
                        }
                     }
                  }
               }
               cmdEvt->argc = 1;    /* repost current event and goto case 1 */
               QACTIVE_POST(AO_SCHED, (QEvt *)e, me);
               break;
            }
            case 4:{
               if(!strcmp((char*)cmdEvt->argv[2],"auto")){
                  me->autoRun = 1;
                  uint32_t time = conv_dec((char *)cmdEvt->argv[3])*60;
                  if(time)
                     me->lastTime  = time;
                  else{
                     bbu_printf("Unsupported test type\n\r");
                     bbu_stress_help(); 
                     return Q_HANDLED();
                     }
                  StressType type = get_test_type((char*)cmdEvt->argv[1]);
                  if(type == INVALID_STRESS){
                     bbu_printf("Unsupported test type\n\r");
                     bbu_stress_help(); 
                     return Q_HANDLED();
                  }else{
                     me->testType = (uint8_t)type;
                  }
               }else{
                  bbu_printf("Unsupported option\n\r");
                  bbu_stress_help(); 
                  return Q_HANDLED();
               }
               cmdEvt->argc = 1;    /* repost current event and goto case 1 */
               QACTIVE_POST(AO_SCHED, (QEvt *)e, me);
               break;
            }
            default:
               bbu_stress_help(); 
               break;
         }
         return Q_HANDLED();
      }
      case STRESS_AUTO_SIG:{ /* This signal will be received from AO_STRESS
                              to restart the stress test in Auto-run mode */
         vbuck1_self_incre_decre();
         ComCmdEvt *strtEvt = Q_NEW(ComCmdEvt, STRESS_STRT_SIG);
         strtEvt->argc = 1;
         QACTIVE_POST(AO_SCHED, (QEvt *)strtEvt, me);
         return Q_HANDLED();
      }
      case CO_STRESS_FAIL_SIG:{
         if(me->cowkTest == TESTMASTER)
         {
            bbu_printf("Stress test on slave core fails\n\r");
            me->cowkTestRslt = CO_TEST_FAIL;
         }
         return Q_HANDLED();
      }
      case CO_STRESS_PASS_SIG:{
         if(me->cowkTest == TESTMASTER)
         {
            bbu_printf("Stress test on slave core passes\n\r");
            me->cowkTestRslt = CO_TEST_PASS;
         }
         return Q_HANDLED();
      }
   }
   return Q_SUPER(&QHsm_top);
}

/*..........................................................................*/   
static QState sched_processing(SCHED *me, QEvent const *e){
   (void)me;

   switch(e->sig){
      case STRESS_CONT_SIG:{
         /* Find a armed test and post it */
         QTimeEvt_disarm(&me->contEvt);
         for(;me->cmdIdx < me->cmdNum; me->cmdIdx++){
            if(me->stressCmd[me->cmdIdx].type){
               if(TRIGGER == me->stressCmd[me->cmdIdx].type){
                  QEvt *stressEvt = Q_NEW(QEvt, BBU_STRESS_SIG);
                  QACTIVE_POST(AO_STRESS, stressEvt, me);
                  mdelay(100);
               }else if(COSTRESS == me->stressCmd[me->cmdIdx].type) {
                  int cmd_len;
                  char cmdstr[256] = {0};
                  cmd_len = bbu_sprintf(cmdstr, "stress cw %d", me->testTime/60);
                  cmdstr[cmd_len] = '\0';
                  ComCliEvt *sce = QIPC_NEW(ComCliEvt, COM_RX_CMD_READY_SIG);
                  sce->len = sizeof(cmdstr);
                  sce->source = 0;  /* differ with command join in auto screen test */
                  strcpy((char*)sce->CliBuffer, cmdstr);
                  QIPC_sendFIFO(IPC2AP, (QEvent *)sce);
                  me->cowkTest = TESTMASTER;
               }else{
                  POST_CMD2CLI(me->stressCmd[me->cmdIdx].Cmd);
                  bbu_printf("\n\rSTRESS> %s\n\r", 
                  me->stressCmd[me->cmdIdx].Cmd);
               }
               
               if(me->stressCmd[me->cmdIdx].type != PERIODIC)
                  me->stressCmd[me->cmdIdx].type = DISARMED;
               me->cmdIdx ++ ;
               break;
            }
         }
         if(me->cmdIdx == me->cmdNum){
            if(me->periodicNum != 0){ /* still have periodic cmd */
               me->cmdIdx = 0;
               /* Here may be changed when we have periodic cmd */
               //QACTIVE_POST((QActive *)me, (QEvt *)e, me);
            }else{
               if(me->duration)
                  QTimeEvt_postEvery(&me->checkEvt, (QActive *)me,
                                me->duration*BSP_TICKS_PER_SEC);
            }
         }else{
            if(me->strsCont && (me->stressCmd[me->cmdIdx-1].wait)){
               //QACTIVE_POST((QActive *)me, (QEvt *)e, me);
               QTimeEvt_postIn(&me->contEvt, (QActive *)me,
                                me->stressCmd[me->cmdIdx-1].wait*BSP_TICKS_PER_SEC);
            }
         }
         return Q_HANDLED();
      }
      case STRESS_PASS_SIG:{
         if(me->lastTime == 0){
            /* Will let the stress test schedular in AO_STRESS stop posting */
            me->strsCont = 0;
                                        /* Inform AO_STRESS that tests pass */
            QACTIVE_POST(AO_STRESS, (QEvt *)e, me);
                                            /* terminate another core's RTC */
                       /* send command to cli to stop some continuous tests */
            stop_ongoing_tests(me);
            if(me->cowkTest == TESTMASTER){
               return Q_TRAN(&sched_master_pass);
            }else{
               QTimeEvt_postIn(&me->displayDelay,(QActive *)me,3*BSP_TICKS_PER_SEC);
               return Q_TRAN(&sched_pass);
            }
         }else{
            if(me->lastTime > UPPER_STEP_TIME){
               QTimeEvt_postIn(&me->testOver, (QActive *)me,
                             UPPER_STEP_TIME*BSP_TICKS_PER_SEC);
               me->lastTime -= UPPER_STEP_TIME;
            }else{
               QTimeEvt_postIn(&me->testOver, (QActive *)me,
                             me->lastTime*BSP_TICKS_PER_SEC);
               me->lastTime = 0;
            }
            return Q_HANDLED();
         }
      }
      case STRESS_FAIL_SIG:{
            /* Will let the stress test schedular in AO_STRESS stop posting */
         me->strsCont = 0;
                                        /* Inform AO_STRESS that tests pass */
         QACTIVE_POST(AO_STRESS, (QEvt *)e, me);
                                            /* terminate another core's RTC */
         stop_ongoing_tests(me);
         if(me->cowkTest == TESTMASTER){
            return Q_TRAN(&sched_master_fail);
         }else{
            QTimeEvt_postIn(&me->displayDelay, (QActive *)me,  3*200);
            return Q_TRAN(&sched_fail);
         }
      }
      case STRESS_STRT_SIG:{
         ComCmdEvt *cmdEvt = (ComCmdEvt *)e;
         if(cmdEvt->argc == 2){
            if(!strcmp((char*)cmdEvt->argv[1],"init")){
               reset_stress_test(me);                     /* stress init */
               bbu_printf("Stress test is initialized\n\r");
               return Q_HANDLED();
            }else if(!strcmp((char*)cmdEvt->argv[1],"abort")){
               QEvt *abtEvt = Q_NEW(QEvt, STRESS_PASS_SIG);
               QACTIVE_POST(AO_SCHED, (QEvt *)abtEvt, me);
               bbu_printf("Stress test will be terminated immediately\n\r");
               return Q_HANDLED();
            }
         }
         bbu_printf("Error: Stress is still ongoing!");
         return Q_HANDLED();
      }
      case STRESS_ACK_SIG:{
         StressAckEvt *ackEvt = (StressAckEvt *)e;
         for(int i = 0; i <me->cmdNum; i++){
            if(!strncmp(me->stressCmd[i].Cmd, ackEvt->cmd, strlen(ackEvt->cmd))){
               if(me->stressCmd[i].duration){
                  me->stressCmd[i].alive = 1;
               }
               /* If the Ack is from a periodic test, trigger the next scheduling */
               if(me->stressCmd[i].type == PERIODIC){
                  POST_CMD2CLI(me->stressCmd[i].Cmd);
                  bbu_printf("\n\rSTRESS> %s\n\r",me->stressCmd[i].Cmd);
               }
               bbu_printf(DBG_MSG"Receive Ack From '%s'\n\r", ackEvt->cmd);
            }
         }
         return Q_HANDLED();
      }
      case STRESS_CHECK_SIG:{
         for(int i = 0; i <me->cmdNum; i++){
            if(me->stressCmd[i].duration){
               if(me->stressCmd[i].alive){
                  bbu_printf(DBG_MSG"Test '%s' is alive\n\r", 
                              me->stressCmd[i].Cmd);
                  me->stressCmd[i].alive = 0;
               }else{
                  static char fail[32];
                  bbu_sprintf(fail, "Miss Ack: '%s'", me->stressCmd[i].Cmd);
                  stress_test_fail(fail);
                  break;
               }
            }
         }
         return Q_HANDLED();
      }
   }
   return Q_SUPER(&sched_idle);
}

/*..........................................................................*/   
static QState sched_fail(SCHED *me, QEvent const *e){
   switch(e->sig){
      case STRESS_OVER_SIG:{
          if(me->autoRun){   /* In auto run mode */
            display_stress_step_fail(me->testType);
            QEvt *stressEvt = Q_NEW(QEvt, STRESS_AUTO_SIG);
            QACTIVE_POST(AO_SCHED, stressEvt, me);
          }else{
            if(me->testType == CW_STRESS){
               QEvt *cwFailEvt = QIPC_NEW(QEvt, CO_STRESS_FAIL_SIG);
               QIPC_sendFIFO(IPC2AP, (QEvt *)cwFailEvt);
            }else{
               display_stress_fail(me->testType);
               screen_cmd_ack(SCREEN_TEST_FIAL, me->scr_index);
            }
          }
          reset_stress_test(me);
          QTimeEvt_disarm(&me->displayDelay);
          QTimeEvt_disarm(&me->testOver);
          QTimeEvt_disarm(&me->checkEvt);
          return Q_TRAN(&sched_idle);
      }
   }
   return Q_SUPER(&sched_idle);
}

/*..........................................................................*/   
static QState sched_pass(SCHED *me, QEvent const *e){
   switch(e->sig){
      case STRESS_OVER_SIG:{
         if(me->autoRun){
            display_stress_step_pass(me->testType);
            QEvt *stressEvt = Q_NEW(QEvt, STRESS_AUTO_SIG);
            QACTIVE_POST(AO_SCHED, stressEvt, me);
         }else{
            if(me->testType == CW_STRESS){
               QEvt *cwPassEvt = QIPC_NEW(QEvt, CO_STRESS_PASS_SIG);
               QIPC_sendFIFO(IPC2AP, (QEvt *)cwPassEvt);
            }else{
               display_stress_pass(me->testType);
               screen_cmd_ack(SCREEN_TEST_PASS, me->scr_index);
            }
         }
         reset_stress_test(me);
         QTimeEvt_disarm(&me->displayDelay);
         QTimeEvt_disarm(&me->testOver);
         QTimeEvt_disarm(&me->checkEvt);
         return Q_TRAN(&sched_idle);
      }
   }
   return Q_SUPER(&sched_idle);
}

/*..........................................................................*/  
static QState sched_master_pass      (SCHED *me, QEvent const *e)
{
   QEvt *rltEvt;
   switch(e->sig){
      case Q_ENTRY_SIG:{
         if(me->cowkTestRslt == CO_TEST_PASS){
            rltEvt = (QEvt *)Q_NEW(QEvt, CO_STRESS_PASS_SIG);
            QACTIVE_POST(AO_SCHED, rltEvt, 0);
         }
         if(me->cowkTestRslt == CO_TEST_FAIL){
            rltEvt = (QEvt *)Q_NEW(QEvt, CO_STRESS_FAIL_SIG);
            QACTIVE_POST(AO_SCHED, rltEvt, 0);
         }
         if(me->cowkTestRslt == CO_TEST_GOON){
            /* wait 10 seconds for slave core, if can't receive test result from
             slave core, the test is taken as FAIL
            */
            QTimeEvt_postIn(&me->displayDelay,(QActive *)me,10*BSP_TICKS_PER_SEC);
         }
         return Q_HANDLED();
      }
      case CO_STRESS_PASS_SIG:{
         QTimeEvt_postIn(&me->displayDelay,(QActive *)me,3*BSP_TICKS_PER_SEC);
         return Q_TRAN(&sched_pass);
      }
      case CO_STRESS_FAIL_SIG:{
         QTimeEvt_postIn(&me->displayDelay,(QActive *)me,3*BSP_TICKS_PER_SEC);
         return Q_TRAN(&sched_fail);
      }
      case STRESS_OVER_SIG:{
         bbu_printf("Can't receive test result from slave core\n\r");
         QTimeEvt_disarm(&me->displayDelay);
         QTimeEvt_postIn(&me->displayDelay,(QActive *)me,3*BSP_TICKS_PER_SEC);
         return Q_TRAN(&sched_fail);
      }
   }
   return Q_SUPER(&sched_idle);
}

/*..........................................................................*/
static QState sched_master_fail      (SCHED *me, QEvent const *e)
{
   QEvt *rltEvt;
   switch(e->sig){
      case Q_ENTRY_SIG:{
         if(me->cowkTestRslt == CO_TEST_PASS){
            rltEvt = (QEvt *)Q_NEW(QEvt, CO_STRESS_PASS_SIG);
            QACTIVE_POST(AO_SCHED, rltEvt, 0);
         }
         if(me->cowkTestRslt == CO_TEST_FAIL){
            rltEvt = (QEvt *)Q_NEW(QEvt, CO_STRESS_FAIL_SIG);
            QACTIVE_POST(AO_SCHED, rltEvt, 0);
         }
         if(me->cowkTestRslt == CO_TEST_GOON){
            /* wait 10 seconds for slave core, if can't receive test result from
             slave core, the test is taken as FAIL
            */
            QTimeEvt_postIn(&me->displayDelay,(QActive *)me,10*BSP_TICKS_PER_SEC);
         }
         return Q_HANDLED();
      }
      case CO_STRESS_PASS_SIG:{
         QTimeEvt_postIn(&me->displayDelay,(QActive *)me,3*BSP_TICKS_PER_SEC);
         return Q_TRAN(&sched_fail);
      }
      case CO_STRESS_FAIL_SIG:{
         QTimeEvt_postIn(&me->displayDelay,(QActive *)me,3*BSP_TICKS_PER_SEC);
         return Q_TRAN(&sched_fail);
      }
      case STRESS_OVER_SIG:{
         bbu_printf("Can't receive test result from slave core\n\r");
         QTimeEvt_disarm(&me->displayDelay);
         QTimeEvt_postIn(&me->displayDelay,(QActive *)me,3*BSP_TICKS_PER_SEC);
         return Q_TRAN(&sched_fail);
      }
   }
   return Q_SUPER(&sched_idle);
}

/*..........................................................................*/  
static void stress_begin(int test_type){
   bbu_printf("\r\n"
              "=======================================================\r\n");
   bbu_printf("=======================================================\r\n");
   bbu_printf("            The BBU %s Stress Test Begin \r\n",
              get_test_name(test_type));
   bbu_printf("            Will last for %d minutes\n\r", 
             (l_sched.lastTime/60));
   bbu_printf("=======================================================\r\n");
   bbu_printf("=======================================================\r\n");
}

/*..........................................................................*/
static void display_stress_pass(int test_type){
   bbu_printf("\r\n"
              "=======================================================\r\n");
   bbu_printf("=======================================================\r\n");
   bbu_printf("         The BBU %s Stress Test Done\r\n", 
              get_test_name(test_type));
   bbu_printf("=======================================================\r\n");
   bbu_printf("=======================================================\r\n");
   printpass();
   bbu_printf("\n\r\n\r%s STRESS PASS\n\r\n\r", get_test_name(test_type));
   bbu_printf("=======================================================\r\n");
   bbu_printf(BBU_PROMPT);
}

/*..........................................................................*/
static void display_stress_step_pass(int test_type){
   bbu_printf("-------------------------------\n\r");
   bbu_printf("   %s STRESS STEP PASS\n\r", get_test_name(test_type));
   bbu_printf("-------------------------------\n\r");
}

/*..........................................................................*/
static void display_stress_step_fail(int test_type){
   bbu_printf("-------------------------------\n\r");
   bbu_printf("-------STRESS STEP FAIL--------\n\r");
   bbu_printf("-------------------------------\n\r");
   bbu_printf("\n\r\n\r%s STRESS FAIL: %s\n\r\n\r", get_test_name(test_type),
      ((SCHED *)AO_SCHED)->failInfo);
}

/*..........................................................................*/  
static void display_stress_fail(int test_type){
   bbu_printf("\r\n"
              "=======================================================\r\n");
   bbu_printf("=======================================================\r\n");
   bbu_printf("============== The BBU Stress Test Done ===============\r\n");
   bbu_printf("=======================================================\r\n");
   bbu_printf("=======================================================\r\n");
   printfail();
   bbu_printf("\n\r\n\r%s STRESS FAIL: %s\n\r\n\r", get_test_name(test_type), 
             ((SCHED *)AO_SCHED)->failInfo);
   bbu_printf("=======================================================\r\n");
   bbu_printf(BBU_PROMPT);
}

/*..........................................................................*/  
static void stop_ongoing_tests(SCHED *me){
   int exit = 0;
   for(int i = 0; i < me->cmdNum; i++){
      if(me->stressCmd[i].StopCmd != NULL){
         if(!strcmp(me->stressCmd[i].StopCmd, "exit")){
            exit = 1;
          }else{
            POST_CMD2CLI(me->stressCmd[i].StopCmd);
           }
      }
   }
   /* This means some cases need to be terminated rudely, such as
    * mlucas
    */
   if(exit){
#ifdef CONFIG_SMP
       mdelay(500);
       sgi_broadcast_all_but_self(GIC_INT_SW_13);
       mdelay(100);
#endif
   }
}

/*..........................................................................*/
static void reset_stress_test(SCHED *me){
   me->cmdIdx = 0;

   if(!me->autoRun){ /* In autoRun, don't need to reset the last_time */
      me->lastTime = STRESS_TEST_TIME;
      me->testTime = STRESS_TEST_TIME;
      me->testType = DDR_STRESS;
      me->cowkTest = TESTALONE;
      me->cowkTestRslt = CO_TEST_GOON;
   }

   for(int i = 0; i < me->cmdNum; i++){
      if(!strcmp(me->stressCmd[i].Cmd, "trigger stress_ao"))
         me->stressCmd[i].type = TRIGGER;
      if(me->stressCmd[i].type == DISARMED)
         me->stressCmd[i].type = ONE_SHOT;
   }
   memset(me->failInfo, 0, sizeof(me->failInfo)/sizeof(char));
}

/*..........................................................................*/
int continue2stress(void){
   int ret;
   ret = l_sched.strsCont;
   return ret;
}

/*..........................................................................*/
int isAutoRunStress(void){
   int ret;
   ret = l_sched.autoRun;
   return ret;
}
/*..........................................................................*/
void stress_test_fail(char *info){
   stress_cmd_ack(SCREEN_TEST_FIAL, info);
}

void stress_test_ack(char *info){
   stress_cmd_ack(SCREEN_TEST_PASS, info);
}

void stress_cmd_ack(int result, char *info){
                                                    /* If during stress test */
    SCHED *me = &(l_sched);
    if(me->strsCont != 0){
      if(result == SCREEN_TEST_FIAL){
         strcpy( ((SCHED *)AO_SCHED)->failInfo, info);
         QEvt *failEvt = Q_NEW(QEvt, STRESS_FAIL_SIG);
         QACTIVE_POST(AO_SCHED, (QEvent *)failEvt, 0);
      }else{
         for(int i = 0; i <me->cmdNum; i++){
            if(!strncmp(me->stressCmd[i].Cmd, info,strlen(info)))
            {
               if((me->stressCmd[i].duration) || 
                  (me->stressCmd[i].type == PERIODIC))
               {
                  StressAckEvt *ackEvt = Q_NEW(StressAckEvt, STRESS_ACK_SIG);
                  strcpy(ackEvt->cmd, info);
                  QACTIVE_POST(AO_SCHED, (QEvent *)ackEvt, 0);
               }
            }
         }
      }
   }
}

void stress_trigger_next(void){
    if(l_sched.strsCont != 0){
        QEvt *triggerEvt = Q_NEW(QEvt, STRESS_CONT_SIG);
        QACTIVE_POST(AO_SCHED, (QEvent *)triggerEvt, 0);
    }
}

   static StressCmdEntry cmdArrayDdr[] = 
   {
     /* test start cmd    test stop cmd      test type  wait  duration alive*/
      {"commt 0 2 0",     NULL,                 ONE_SHOT,  1,      0,      0},
      {"mtsp 0x2200000 80000 0xFFFFFFFF", NULL, PERIODIC,  1,      0,      0},
  #ifdef CONFIG_H264
      {"h264 1",          NULL,                 PERIODIC,  2,      40,      0},
  #endif
      {"scmd stress cw",  "scmd stress abort",  COSTRESS,  2,      0,      0},

   };
/*-----------------------------------------------*/
   static StressCmdEntry cmdArrayPwr[] = 
   {
     /* test start cmd    test stop cmd      test type  wait  duration alive*/
      {"ttdm 1",          "ttdm 0",          ONE_SHOT,  1,       0,     0},
      {"commt 0 2 0",     NULL,              ONE_SHOT,  1,      0,      0},
      {"ilcd",            NULL,              ONE_SHOT,  1,      0,      0},
      {"icam 1 1 2",      NULL,              ONE_SHOT,  2,      0,      0},
  #ifdef CONFIG_MLUCAS
      {"mlucas",          "exit",            ONE_SHOT,  3,      45,     0},
  #endif
  #ifdef CONFIG_GCU
      /* stress taiji 1080p */
      {"gcu3dstress 100 100",     "gcu3dstop 100",     ONE_SHOT,  3,      60,     0},
  #endif
  #ifdef CONFIG_VPU /* 1080p decode */
      {"vpu stress 2",    "vpu stress 0",     ONE_SHOT,  3,       0,     0},
  #endif
   };
/*-----------------------------------------------*/
   static StressCmdEntry cmdArrayPe[] = 
   {
     /* test start cmd    test stop cmd      test type  wait  duration alive*/
      {"commt 0 2 0",     NULL,              ONE_SHOT,  1,      0,       0},
  #ifdef CONFIG_H264
      {"h264 stress 1",   "h264 stress 0",   ONE_SHOT,  1,      40,       0},
  #endif
  #ifdef CONFIG_GCU  /* Taiji */
      /* stress taiji 1080p */
      {"gcu3dstress 100 100",     "gcu3dstop 100",     ONE_SHOT,  3,      60,     0},
  #endif
  #ifdef CONFIG_VPU
      {"vpu stress 1",  "vpu stress 0",      ONE_SHOT,   3,     0,      0},
  #endif
   };
/*-----------------------------------------------*/
   static StressCmdEntry cmdArrayPe2[] = 
   {
      /* test start cmd    test stop cmd      test type  wait  duration alive*/
    //  {"commt 0 2 0",      NULL,              ONE_SHOT,   1,     0,      0},
      {"mlucas",           NULL,              ONE_SHOT,   2,     0,      0},
  #ifdef CONFIG_H264
      {"h264 stress 1",    "h264 stress 0",   ONE_SHOT,   1,     40,      0},
  #endif
  #ifdef CONFIG_GCU
      /* stress taiji 1080p */
      {"gcu3dstress 100 100",     "gcu3dstop 100",     ONE_SHOT,  3,      60,     0},
  #endif
  #ifdef CONFIG_VPU  /* 1080p decode */
      {"vpu stress 2",     "vpu stress 0",        ONE_SHOT,   3,     0,      0},
  #endif
  #ifdef CONFIG_AUDIO
      {"audio mf",             NULL,                ONE_SHOT,   1,     0,      0},
      {"audio i2s play 3 0 1", "audio i2s disable", ONE_SHOT,   1,     0,      0},
  #endif
   };

/*-----------------------------------------------*/
   static StressCmdEntry cmdArrayMiniddr[] = 
   {
     /* test start cmd    test stop cmd      test type  wait  duration alive*/
      {"commt 0 2 0",     NULL,              ONE_SHOT,  1,      0,      0},
      {"ilcd",            NULL,              ONE_SHOT,  1,      0,      0},
      {"icam 1 1 2",      NULL,              ONE_SHOT,  2,      0,      0},
      {"dma stress 0 32 0x100000",    "dma stop",       ONE_SHOT,  1,      0,      0},
  #ifdef CONFIG_H264
      {"h264 stress 1",   "h264 stress 0",   ONE_SHOT,  2,      60,      0},
  #endif
   };
/*-----------------------------------------------*/
#ifdef CONFIG_GCU
static StressCmdEntry cmdArrayGcu[] = 
   {
      /* test start cmd    test stop cmd      test type  wait  duration alive*/
      /* stress taiji 1080p */
      {"gcu3dstress 100 100",     "gcu3dstop 100",     ONE_SHOT,  3,      60,     0},
   };
#endif

/*-----------------------------------------------*/
#ifdef CONFIG_VPU
static StressCmdEntry cmdArrayVpu[] = 
   {
      /* test start cmd    test stop cmd      test type  wait  duration alive*/
      {"vpu stress 2",     "vpu stress 0",    ONE_SHOT,   3,     0,      0},
   };
#endif

static StressCmdEntry cmdArraySoc[] = 
   {
     /* test start cmd    test stop cmd      test type  wait  duration alive*/
      {"commt 0 2 0",     NULL,              ONE_SHOT,  1,      0,      0},
      {"ilcd",            NULL,              ONE_SHOT,  1,      0,      0},
      {"icam 1 1 2",      NULL,              ONE_SHOT,  2,      0,      0},
  #ifdef CONFIG_MLUCAS
      {"mlucas",          "exit",            ONE_SHOT,  3,      45,     0},
  #endif
  #ifdef CONFIG_GCU
      /* stress taiji 1080p */
      {"gcu3dstress 100 100",     "gcu3dstop 100",     ONE_SHOT,  3,      60,     0},
  #endif
  #ifdef CONFIG_VPU /* 1080p decode */
      {"vpu stress 2",    "vpu stress 0",     ONE_SHOT,  3,       0,     0},
  #endif
   };

/*-----------------------------------------------*/
   static StressCmdEntry cmdArrayCpu[] = 
   {
      
  /* test start cmd           test stop cmd            test type  wait  duration alive*/
#ifdef CONFIG_MLUCAS
   {"mlucas cpu 0",              "exit",                 ONE_SHOT,  1,      0,      0},
#endif
   {"super_pi stress 1 cpu 2",   "super_pi stress 0 cpu 2",    ONE_SHOT,  1,      0,      0},
   {"cpbp cpu 1",                "exit",                 ONE_SHOT,  1,      0,      0},
#ifdef CONFIG_H264
   {"h264 dbg 0",                NULL,                   ONE_SHOT,  1,      0,      0},
   {"h264 3",                    NULL,                   PERIODIC,  1,      0,      0},
#endif  
   };

/*-----------------------------------------------*/
   static StressCmdEntry cmdArrayCw[] = 
   {
   /* test start cmd           test stop cmd            test type  wait  duration alive*/
      {"mtsp 0x2000000 80000 0xFFFFFFFF", NULL,               PERIODIC,  1,      0,      0},
   };

static StressTestEntry stressTests[] = {
   {"ddr",     "DDR",        cmdArrayDdr,     sizeof(cmdArrayDdr)/sizeof(StressCmdEntry),       DDR_STRESS},
   {"cpu",     "CPU",        cmdArrayCpu,     sizeof(cmdArrayCpu)/sizeof(StressCmdEntry),       CPU_STRESS},
   {"power",   "POWER",      cmdArrayPwr,     sizeof(cmdArrayPwr)/sizeof(StressCmdEntry),       PWR_STRESS},
   {"pe",      "PE",         cmdArrayPe,      sizeof(cmdArrayPe)/sizeof(StressCmdEntry),        PE_STRESS},
   {"pe2",     "PE2",        cmdArrayPe2,     sizeof(cmdArrayPe2)/sizeof(StressCmdEntry),       PE2_STRESS},
#ifdef CONFIG_GCU
   {"gcu",     "GCU",        cmdArrayGcu,     sizeof(cmdArrayGcu)/sizeof(StressCmdEntry),       GCU_STRESS},
#endif
#ifdef CONFIG_VPU
   {"vpu",     "VPU",        cmdArrayVpu,     sizeof(cmdArrayVpu)/sizeof(StressCmdEntry),       VPU_STRESS},
#endif
   {"miniddr", "MINI DDR",   cmdArrayMiniddr, sizeof(cmdArrayMiniddr)/sizeof(StressCmdEntry),   MINI_DDR_STRESS},
   {"soc",     "SOC",        cmdArraySoc,     sizeof(cmdArraySoc)/sizeof(StressCmdEntry),       SOC_STRESS},
   {"cw",      "CW",         cmdArrayCw,      sizeof(cmdArrayCw)/sizeof(StressCmdEntry),        CW_STRESS},
};

/*..........................................................................*/
static char* get_test_name(int test_type){
   char *name = "UNKNOWN";
   memset(name, 0, sizeof(name));
   for(size_t i = 0; i < sizeof(stressTests)/sizeof(StressTestEntry); i++){
      if(stressTests[i].type == test_type){
         name = (char *)stressTests[i].name;
         break;
      }
   }
   return name;
}

/*..........................................................................*/
static StressType get_test_type(char *cmd)
{
   StressType type = INVALID_STRESS;
   for(size_t i = 0; i < sizeof(stressTests)/sizeof(StressTestEntry); i++){
      if(!strcmp(stressTests[i].cmd, cmd)){
         type = stressTests[i].type;
         break;
      }
   }
   return type;
}

/*..........................................................................*/
static void display_stress_list(StressType type)
{
    StressCmdEntry *cmdArray;
    int cmdNum;
    for(size_t i = 0; i < sizeof(stressTests)/sizeof(StressTestEntry); i++){
      if(type == stressTests[i].type){
         cmdArray = stressTests[i].cmdArray;
         cmdNum = stressTests[i].cmdNum;
         bbu_printf("\n\r%s Stress Test List:\n\r", stressTests[i].name);
         break;
      }
   }
   for(int i = 0; i < cmdNum; i++){
       bbu_printf("%s\n\r", cmdArray[i].Cmd);
   }
}

/*..........................................................................*/
static void init_cmd_array(SCHED *me, int test_type)
{
   me->cmdIdx = 0;
   for(size_t i = 0; i < sizeof(stressTests)/sizeof(StressTestEntry); i++){
      if(stressTests[i].type == test_type){
         me->stressCmd =  stressTests[i].cmdArray;
         me->cmdNum = stressTests[i].cmdNum;
         break;
      }
   }
   uint8_t duration = 0;
   me->periodicNum = 0;
   for(int i = 0; i < me->cmdNum; i++){
      if(me->stressCmd[i].type == PERIODIC)
         me->periodicNum++;
      if(me->stressCmd[i].duration > duration)
         duration = me->stressCmd[i].duration;
      me->stressCmd[i].alive = 0;
   }
   me->duration = duration;
}

/*..........................................................................*/
void print_all_stress_tests(void)
{
   for(size_t i = 0; i < sizeof(stressTests)/sizeof(StressTestEntry); i++){
      bbu_printf("%s ", stressTests[i].cmd);
   }
}
