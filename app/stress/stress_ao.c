/*****************************************************************************
* Product: BBU
* 
* Purpose: STRESS AO
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
#ifdef CONFIG_GCU
#include "gcu.h"
#endif
extern int super_pi_main (int argc, char *argv[], void* ao);
extern int mlucas_screen_idx;

Q_DEFINE_THIS_FILE

/* stress Active Object ....................................................*/
typedef struct STRESSTag {
   QActive super;             /* extend the QActive class */

}STRESS;

/* hierarchical state machine ............................................. */
static QState stress_initial(STRESS *me, QEvent const *e);
static QState stress_processing(STRESS *me, QEvent const *e);


static STRESS l_stress;    /* the sole instance of the STRESS active object */

/* global objects.......................................................... */
QActive * const AO_STRESS = (QActive *)&l_stress; /*opaque pointer to stress*/

/*..........................................................................*/
void STRESS_ctor(void) {
   STRESS *me = &l_stress;
   QActive_ctor(&me->super, (QStateHandler)&stress_initial);

}

/*..........................................................................*/
static QState stress_initial(STRESS *me, QEvent const *e){
   (void)e;      /* avoid the "unreferenced parameter" warning */
   return Q_TRAN(&stress_processing);
}

/*..........................................................................*/   
static QState stress_processing(STRESS *me, QEvent const *e){
   (void)me;
   ComCmdEvt *cmdEvt = (ComCmdEvt *)e;

   switch(e->sig){
      case BBU_STRESS_SIG:{
   #ifdef CONFIG_H264
         int result = h264_test(1, 0, 0);
         stress_cmd_ack(result, "h264");
   #endif
         if(continue2stress())
            QACTIVE_POST(AO_STRESS, (QEvt *)e, me);
         else{
            if(isAutoRunStress()){
               QEvt *restartEvt = Q_NEW(QEvt, BBU_STRESS_SIG);
               QACTIVE_POST(AO_SCHED, (QEvt *)restartEvt, me);
            }else{
               bbu_printf("Stress Test is terminated in core 1\n\r");
               //QEvt *abortEvt = Q_NEW(QEvt, STRESS_ABORT_SIG);
               //QACTIVE_POST(AO_SCHED, (QEvt *)abortEvt, me);
            }
         }
         return Q_HANDLED();
      }
      case BBU_LUCDWT_SIG:{
         if((cmdEvt->argc > 2) && !strcmp((char *)cmdEvt->argv[cmdEvt->argc-2], "cpu")){
         #ifdef CONFIG_CPU_AO
            uint8_t cpu_id = conv_dec((char *)cmdEvt->argv[cmdEvt->argc-1]);
            if(cpu_id < CORE_NUM){
               cmdEvt->argc -= 2;
               QACTIVE_POST((QActive *)AO_CPU[cpu_id], (QEvt *)cmdEvt, me);
            }else{
               bbu_printf("lucdwt-->  CPU ID is out of range\n\r");
            }
         #else
            bbu_printf("This feature is not supported as AO per CPU mechanism is disabled\n\r");
         #endif
         }else{
            lucas_main(cmdEvt->argc, (char **)cmdEvt->argv);
            screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         }
         return Q_HANDLED();
      }
      case STRESS_CPBP_SIG:{
         if((cmdEvt->argc > 2) && !strcmp((char *)cmdEvt->argv[cmdEvt->argc-2], "cpu")){
         #ifdef CONFIG_CPU_AO
            uint8_t cpu_id = conv_dec((char *)cmdEvt->argv[cmdEvt->argc-1]);
            if(cpu_id < CORE_NUM){
               cmdEvt->argc -= 2;
               QACTIVE_POST((QActive *)AO_CPU[cpu_id], (QEvt *)cmdEvt, me);
            }else{
               bbu_printf("super_pi-->  CPU ID is out of range\n\r");
            }
         #else
            bbu_printf("This feature is not supported as AO per CPU mechanism is disabled\n\r");
         #endif
         }else{
            calc_print_biggest_prime();
         }
         bbu_printf(BBU_PROMPT);
         return Q_HANDLED();
      }
      case SUPER_PI_SIG:{
         if((cmdEvt->argc > 2) && !strcmp((char *)cmdEvt->argv[cmdEvt->argc-2], "cpu")){
         #ifdef CONFIG_CPU_AO
            uint8_t cpu_id = conv_dec((char *)cmdEvt->argv[cmdEvt->argc-1]);
            if(cpu_id < CORE_NUM){
               cmdEvt->argc -= 2;
               QACTIVE_POST((QActive *)AO_CPU[cpu_id], (QEvt *)cmdEvt, me);
            }else{
               bbu_printf("super_pi-->  CPU ID is out of range\n\r");
            }
         #else
            bbu_printf("This feature is not supported as AO per CPU mechanism is disabled\n\r");
         #endif
         }else{
            super_pi_main(cmdEvt->argc, (char **)cmdEvt->argv, me);
            stress_cmd_ack(SCREEN_TEST_PASS, "super_pi");
         }
         bbu_printf("\r"BBU_PROMPT);
         return Q_HANDLED();
      }
#ifdef CONFIG_MLUCAS
      case BBU_MLUCAS_SIG:{
         if((cmdEvt->argc > 2) && !strcmp((char *)cmdEvt->argv[cmdEvt->argc-2], "cpu")){
         #ifdef CONFIG_CPU_AO
            uint8_t cpu_id = conv_dec((char *)cmdEvt->argv[cmdEvt->argc-1]);
            if(cpu_id < CORE_NUM){
               cmdEvt->argc -= 2;
               QACTIVE_POST((QActive *)AO_CPU[cpu_id], (QEvt *)cmdEvt, me);
            }else{
               bbu_printf("mlucas-->  CPU ID is out of range\n\r");
            }
         #else
            bbu_printf("This feature is not supported as AO per CPU mechanism is disabled\n\r");
         #endif
         }else{
            mlucas_init_mfile();
            mlucas_screen_idx = cmdEvt->scr_cmd_index;
            bbu_printf("%s is running Mlucas Test\n\r", MY_NAME);
            MLUCAS_MAIN(cmdEvt->argc, (char **)cmdEvt->argv);
            free_mlucas_buffers();
            screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
            mlucas_screen_idx = -1;
         }
         bbu_printf(BBU_PROMPT);
         return Q_HANDLED();
      }
#endif
   }
   return Q_SUPER(&QHsm_top);
}
