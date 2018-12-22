/*****************************************************************************
* Product: BBU
*
* Purpose: COMMON AO
*
* Copyright (C) 2010-2011 Marvell MTBL-Hefei. All rights reserved.
*
* Author: Yu Zhang  email: zhangy@marvell.com
*
*****************************************************************************/
#include "predefines.h"
#include "qp_port.h"
#include "bbu.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include "cache-test.h"
#include "cpu_pat.h"
#include "pmic.h"
#include "core_tests.h"
#include "common_tests.h"
#include "mem_tests.h"
#include "clock.h"
#include "timer_dec.h"
#include "md5.h"
#include "pmu_dec.h"
#include "perf_counter.h"
#include "stress.h"
#include "cp15.h"
#include "ipc_app.h"
#include "lcd_test.h"
#include "offline_test.h"
#include "camera_test.h"

Q_DEFINE_THIS_FILE

extern void mmp_arch_reset(char mode);
extern uint32_t dte_done;
extern uint32_t dte_pass;

/* common Active Object ....................................................*/
typedef struct COMMONTag {
   QActive super;             /* extend the QActive class */
}COMMON;

/* hierarchical state machine ............................................. */
static QState common_initial(COMMON *me, QEvent const *e);
static QState common_processing(COMMON *me, QEvent const *e);


static COMMON l_common;    /* the sole instance of the COMMON active object */

/* global objects.......................................................... */
QActive * const AO_COMMON = (QActive *)&l_common; /*opaque pointer to common*/

/*..........................................................................*/
void COMMON_ctor(void) {
   COMMON *me = &l_common;
   QActive_ctor(&me->super, (QStateHandler)&common_initial);

}

/*..........................................................................*/
static QState common_initial(COMMON *me, QEvent const *e){
   (void)e;      /* avoid the "unreferenced parameter" warning */
    QS_OBJ_DICTIONARY(&l_common);

    QS_FUN_DICTIONARY(&common_initial);
    QS_FUN_DICTIONARY(&common_processing);

    QS_SIG_DICTIONARY(BBU_TMARK_SIG,      me);
    QS_SIG_DICTIONARY(BBU_MTSP_SIG,       me);

#ifndef CONFIG_CR5
#ifdef SV_MEM_TEST
	sv_mem_test_datapattern_init();
#endif

#ifdef SIMPLE_DATA_PATTEN
	//simple_data_test(0);
#endif
#endif

    return Q_TRAN(&common_processing);
}

extern int GetCPUID(void);
/*..........................................................................*/
static QState common_processing(COMMON *me, QEvent const *e){
   (void)me;
   ComCmdEvt *cmdEvt = (ComCmdEvt *)e;
   int result;
   switch(e->sig){
      case BBU_PWM_SIG:{
          int pwm;
          if((cmdEvt->argc) >= 2)
            pwm = conv_dec(cmdEvt->argv[1]);
          else
            pwm = 0;
          pwm_test(pwm);
          screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
          return Q_HANDLED(); 
      }
      case BBU_WDT_SIG:{
         if((cmdEvt->argc) < 3)
         {
            bbu_printf("wdt <second> <mode>\n\r");
            bbu_printf(" <mode>: 1 - reset, 0 - interrupt\n\r");
         }else{
            int sec = conv_dec(cmdEvt->argv[1]);
            int mode = conv_dec(cmdEvt->argv[2]);
            bbu_printf("wdt test! second: %d, mode: %s\n\r", sec, (mode == 1)?"reset":"interrupt");
            wdt_test(sec, mode);
            screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         }
         return Q_HANDLED();
      }
      case BBU_RIPC_SIG:{
         result = bbu_ripc_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_DDR_SWEEP_SIG:
         result = bbu_ddr_sweep_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      case BBU_SMTD_SIG:{
         bbu_smtd(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_MTSP_SIG:{
         result = bbu_mtsp(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         stress_cmd_ack(result, "mtsp");
         if(cmdEvt->argc == 5 && 0== strcmp((char*)cmdEvt->argv[4],"stress"))
         {
            QACTIVE_POST((QActive *)AO_COMMON, e, AO_COMMON);
            const char *pp_set_cmd = "ppset 100 1";
            ScreenCmdEvt *sce = Q_NEW(ScreenCmdEvt, COM_RX_CMD_READY_SIG);
            sce->len = sizeof(pp_set_cmd);
            strcpy((char*)sce->cmd, pp_set_cmd);
            QACTIVE_POST(AO_Cli, (QEvent *)sce, 0);
         }
         return Q_HANDLED();
      }
      case BBU_PID_SIG:{
         uint32_t cpu_type = 0, cpu_step = 0;
         result = bbu_pid(&cpu_type, &cpu_step);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_UART_SIG:{
          result = bbu_uart_switch(cmdEvt->argc, cmdEvt->argv);
          screen_cmd_ack(result, cmdEvt->scr_cmd_index);
          return Q_HANDLED();
      }
      case BBU_PEEK_SIG:{
         bbu_peek(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_POKE_SIG:{
         bbu_poke(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_DPEEK_SIG:{
         bbu_dpeek(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_DPOKE_SIG:{
         bbu_dpoke(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_DFMEM_SIG: {
        bbu_dfmem(cmdEvt->argc, cmdEvt->argv);
        screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
        return Q_HANDLED();
      }
    case BBU_DVMEM_SIG: {
        bbu_dvmem(cmdEvt->argc, cmdEvt->argv);
        screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
        return Q_HANDLED();
      }
    case BBU_DMEMT_SIG: {
        bbu_dmemt(cmdEvt->argc, cmdEvt->argv);
        screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
        return Q_HANDLED();
      }
    case BBU_DRC_SIG: {
        bbu_drc(cmdEvt->argc, cmdEvt->argv);
        screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
        return Q_HANDLED();
      }
    case BBU_DWC_SIG: {
        bbu_dwc(cmdEvt->argc, cmdEvt->argv);
        screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
        return Q_HANDLED();
      }
      case BBU_SETV_SIG:
         result = bbu_setv_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      case BBU_RESET:{
         if((cmdEvt->argc) == 2){
            if(strcmp((const char*)cmdEvt->argv[1], "s") == 0)
               mmp_arch_reset(1);
            if(strcmp((const char*)cmdEvt->argv[1], "w") == 0)
               mmp_arch_reset(2);
            if(strcmp((const char*)cmdEvt->argv[1], "p") == 0)
               mmp_arch_reset(4);
         }
         else
         if((cmdEvt->argc) == 1)
            mmp_arch_reset(2);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_XTC_SIG:{
         bbu_xtc_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_DEVPM_SIG:{
         //pm_test(); //removed
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_MFPTEST_SIG:{
         mfp_tests();
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_DUMP_SIG:{
         bbu_dump(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_FMEM_SIG:{
         bbu_fmem(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_MEMCOPY_SIG:{
         bbu_memcopy(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_MEMT_SIG:{
         result = bbu_memt(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_EICA_SIG:{
         bbu_eica_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_DICA_SIG:{
         bbu_dica_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_CTST_SIG:{
         bbu_cache_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_L2CT_SIG:{
         bbu_sl2c_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_SCRT_SIG:{
         bbu_screening_test(0, 3);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
#ifdef CONFIG_CR5
      case BBU_DTC_SIG:{
         result = dtc_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
#endif
      case BBU_TEST_SIG:{
        // bbu_printf("put the function call here to be called by the test command! \r\n ");
         //mlucas_init_mfile();
         //MLUCAS_MAIN(cmdEvt->argc, (char **)cmdEvt->argv);
         #ifdef CONFIG_CR5
         mpu_config_dump();
         #endif
         temp_memory_copy_test();
         //print_current_time();
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
	  case LCD_SIG:{
		  int test_case = 1;
		  int ret = -1;
	      bbu_printf("crane lcd test start!\r\n ");
          if((cmdEvt->argc) >= 2){
			  test_case = atoi(cmdEvt->argv[1]);
			  bbu_printf("test case is %d!\r\n ", test_case);
		  }
		  ret = lcd_test_entry(test_case);
		  if(0 == ret)
	      	screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
		  else
			  screen_cmd_ack(SCREEN_TEST_FIAL, cmdEvt->scr_cmd_index);
	      return Q_HANDLED();
	  }
      case CAM_OFFLINE_SIG: {
	      bbu_printf("camera offline test start!\r\n ");
	      offline_regs_walk_through();
	      screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
	      return Q_HANDLED();
      }
      case CAM_READ_ID_SIG: {
            bbu_printf("camera read i2c id test start!\r\n ");
            camear_sensor_read_id();
            screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
            return Q_HANDLED();

      }
      case CAM_ICAM_SIG: {
          bbu_printf("camera init test start!\r\n ");
          camear_sensor_init();
          screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
          return Q_HANDLED();
      }

      case CAM_REG_SIG: {
          bbu_printf("camera reg test start!\r\n ");
          camera_reg_read_write();
          screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
          return Q_HANDLED();
      }

      case CAM_ONLINE_SIG: {
          bbu_printf("camera raw8 spi 2lan normal  test start!\r\n ");
          camera_online(RAW8_SPI_2LAN_NORMAL);
          screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
          return Q_HANDLED();
      }
      case CAM_ONLINE_IRQ_SIG: {
              bbu_printf("camera online irq start!\r\n ");
              camera_online(RAW8_SPI_2LAN_NORMAL);
              screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
              return Q_HANDLED();
       }


      case BBU_SBIT_SIG:{
         result = bbu_sbit_test(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         bbu_printf("BBU> ");
         return Q_HANDLED();
      }
      case BBU_CBIT_SIG:{
         result = bbu_cbit_test(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         bbu_printf("BBU> ");
         return Q_HANDLED();
      }
      case BBU_RBIT_SIG:{
         result = bbu_rbit_test(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         bbu_printf("BBU> ");
         return Q_HANDLED();
      }
      case BBU_MGWP_SIG:{
         result = bbu_mcuGenerateWordPattern(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_SCMD_SIG:{
         bbu_sctoc_help(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(1, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_CMDA_SIG:{
         bbu_sctoc_help(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(1, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_CMDD_SIG:{
         bbu_scmd2msa(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(1, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_IPC_SIG:{
         result = bbu_ipc_test(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(1, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_MFILE_SIG:{
         result = bbu_mfile_test(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         bbu_printf("BBU> ");
         return Q_HANDLED();
      }
#ifdef CONFIG_FFTS
      case BBU_FFTS_SIG:{
         result = ffts_main(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         bbu_printf(BBU_PROMPT);
         return Q_HANDLED();
      }
#endif
      case BBU_MCPS_SIG:{
         bbu_mcps();
         screen_cmd_ack(1, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_SLDO_SIG:
         bbu_sldo_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
       case BBU_PMIC_SIG:
         result = bbu_pmic_func(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      case BBU_TMARK_SIG:{
         TMark_main(cmdEvt->argc, (char **)cmdEvt->argv);
         if(0== strcmp((char*)cmdEvt->argv[1],"stress"))
         {
             QACTIVE_POST((QActive *)AO_COMMON, e, AO_COMMON);
            POST_CMD2CLI("h264");
            POST_CMD2CLI("ppset 100 1");
            #if (0)
             const char *pp_set_cmd = "ppset 100 1";
             ScreenCmdEvt *sce = Q_NEW(ScreenCmdEvt, COM_RX_CMD_READY_SIG);
             sce->len = sizeof(pp_set_cmd);
             strcpy((char*)sce->cmd, pp_set_cmd);
             QACTIVE_POST(AO_Cli, (QEvent *)sce, 0);
            #endif
         }
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_CMARK_SIG:{
         bbu_printf("\n\rCoreMark baremetal test begins, please wait...\n\r");
         core_mark_main(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_CPUT_SIG:{
         cpu_test(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_CACHEBENCH_SIG:{
         bbu_printf("\n\r");
         cachebench_main(cmdEvt->argc, (char **)cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_GPIO_SIG:{
         bbu_gpio(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_CLRS_SIG:{
         bbu_clear_screen();
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_CURM_SIG:{
         bbu_curm(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_PRINT_SIG:{
         bbu_print_switch(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_COMMT_SIG:{
	if((cmdEvt->argc>1)&&(check_help(cmdEvt->argv[1]))){
            bbu_commt_help();
            screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
        }
        else
        {
            //set the voltage before call comm_d2 to test CP&MSA boot Vmin.
            uint32_t _msa_mode = XpStrToValue((char *)cmdEvt->argv[1]);
            uint32_t _test_index = XpStrToValue((char *)cmdEvt->argv[2]);

            if (((_msa_mode&0xF) != 0) && ((_msa_mode&0xF) != 1)&&((_msa_mode&0xF) !=2)&&((_msa_mode&0xF) !=3)&&((_msa_mode&0xF) != 0xF))
            {
                _msa_mode = 1; //put to D2 is default
                bbu_printf("not supported msa mode, use default mode as entering D2 with modems off\r\n");
            }

            #if 0
            adfc(0);
            pm8xx_vbuck_cfg2(VBUCK_ON, VBUCK1, 1150, DVC00);
            pm8xx_vbuck_cfg2(VBUCK_ON, VBUCK1, 1150, DVC01);
            #endif //set use commands in formal release for flexible

            uint32_t count, loops, msa_pp_index;
            msa_pp_index = 2; //pp1: 312MHz; pp2: 416MHz; pp3: 499MHz MHz; pp4: 624MHz; pp5: 832MHz
            loops = 1;
            count = 0;
            if(cmdEvt->argc > 3) loops = XpStrToValue((char *)cmdEvt->argv[3]);
            if(cmdEvt->argc > 4) msa_pp_index = XpStrToValue((char *)cmdEvt->argv[4]);

            for (uint32_t i = 0; i<loops; i++) { //at least test two times to make sure it pass stable
               comm_test(_msa_mode, _test_index, msa_pp_index);
               if (msa_boot_test == 1)
               {
                  switch (_test_index)
                  {
                  case 4: //GSM equalizer test
                      if(gsm_eq_test == 1)count++;
                     break;
                  default: //no additional test
                      count++;
                     break;
                  }
               }
            }

            if(count == loops){
               result = SCREEN_TEST_PASS;
            }
            else
            {
               result = SCREEN_TEST_FIAL;
            }
            bbu_printf("The commt test passed %d times, failed %d times.\r\n", count, (loops-count));

            screen_cmd_ack(result, cmdEvt->scr_cmd_index);
            //stress_cmd_ack(result, "commt");
         }
         return Q_HANDLED();
       }
      case BBU_PDOWN:{
         bbu_printf("power down....");
         pm8xx_base_write(0x0d, 0x20);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_DELAY_SIG:{
         if((cmdEvt->argc) == 2){
            delay(conv_dec((char *)cmdEvt->argv[1]));
#ifdef CONFIG_CP_TIME_TICK
	    bbu_printf("Use CP timer(D0210000) as delay tick\r\n");
#else
	    bbu_printf("Use AP timeras delay tick\r\n");
#endif
            screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         }
         else
		    bbu_printf("delay need a interger parameter.\n\r");
         return Q_HANDLED();
      }
      case BBU_MDELAY_SIG:{
         if((cmdEvt->argc) == 2){
            mdelay(conv_dec((char *)cmdEvt->argv[1]));
            screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         }
         else
            bbu_printf("mdelay need a interger parameter.\n\r");
         return Q_HANDLED();
      }
      case BBU_LINPACK_SIG:{
         linpack_main();
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_WHET_SIG:{
         whetstone_main(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
      case BBU_DHRY_SIG:{
         int loops;
         if(cmdEvt->argc == 2 && !check_help((uint8_t *)cmdEvt->argv[1])){
            loops = conv_dec((char *)cmdEvt->argv[1]);
            drys_test(loops);
         }else{
            bbu_dhry_help();
         }
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
#ifdef QF_STACK_PER_AO
      case BBU_STACK_SIG:{
         bbu_stack(cmdEvt->argc, cmdEvt->argv);
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
#endif
#ifdef CONFIG_SUSP_DBG
      case AO_WAKE_SIG:{
         int p = conv_dec((char *)cmdEvt->argv[1]);
         if( p <= QF_MAX_ACTIVE)
            QA_KICK_SEND_RESULT(p, p);
         else{
#ifdef COMPOSITE_LEVEL_SUSPEND
            uint32_t compo = conv_hex((char *)cmdEvt->argv[2]);
            QCOMPO_KICK_SEND_RESULT((QCompo *)compo, p);
#endif
         }
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         return Q_HANDLED();
      }
#endif
      case DTE_TEST_RES_SIG: {
         uint8_t result;
         if (dte_done) {
            if (dte_pass) {
               result = BBU_TEST_PASS;
               bbu_printf("DTE test pass!\r\n");
               }
            else {
               result = BBU_TEST_FAIL;
               bbu_printf("DTE test fail!\r\n");
               }
         } else {
            result = BBU_TEST_FAIL;
            bbu_printf("Error: There is no feedback about DTE test from OBM!\r\n");
         }
         screen_cmd_ack(result, cmdEvt->scr_cmd_index);
      }
   }

   return Q_SUPER(&QHsm_top);
}
