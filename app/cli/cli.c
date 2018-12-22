/*****************************************************************************
* Product: BBU
*
* Purpose: Cli AO
*
* Copyright (C) 2010-2011 Marvell MTBL-Hefei. All rights reserved.
*
* Author: Lianghu Xu  email: xlianghu@marvell.com
*
*****************************************************************************/

#include <string.h>  /*newlibc should support it, otherwise need to code some basic operations*/
#include "qp_port.h"
#include "bsp.h"
#include "bbu.h"
#include "ascii.h"
#include "utils.h"
#include "uart_dec.h"
#include "gic_dec.h"
#include "cli_comp.h"
#ifdef Q_SPY_EXT
#include "cli_qspy_comp.h"
#endif
#ifdef CONFIG_ATE
#include "ate.h"
#endif

Q_DEFINE_THIS_FILE

typedef struct CliTag{
	QActive  super;                                  /* derive from QActive */
#ifdef Q_SPY_EXT
	CliQspyComp cliqspycomp;
#else
	CliComp  clicomp;
#endif
}Cli;

CmdLineEntry gCmdTable[MAX_CMD_NUM];

static QState Cli_initial(Cli *me, QEvent const *e);
static QState Cli_processing(Cli *me, QEvent const *e);

static Cli l_cli;                         /* the sole instance of the Cli active object */

/* Public-scope objects----------------------------------------------------*/
QActive * const AO_Cli = (QActive *)&l_cli; /*opaque pointer to Console AO*/

void Cli_ctor(void){
	Cli *me = &l_cli;
	QActive_ctor(&me->super, (QStateHandler)&Cli_initial);
#ifdef Q_SPY_EXT
	CliQspyComp_ctor(&me->cliqspycomp);
#else
	CliComp_ctor(&me->clicomp);
#endif
}

QState Cli_initial(Cli *me, QEvent const *e){
	(void)e;
#ifdef Q_SPY_EXT
	CliQspyComp_init(&me->cliqspycomp);
#else
	CliComp_init(&me->clicomp);
#endif

 	QS_OBJ_DICTIONARY(&l_cli);

//    QS_FUN_DICTIONARY(&QHsm_top);
    QS_FUN_DICTIONARY(&Cli_initial);
    QS_FUN_DICTIONARY(&Cli_processing);

    QS_SIG_DICTIONARY(COM_RX_CMD_READY_SIG,      me);
    QS_SIG_DICTIONARY(COM_RX_NEXT_BYTE_SIG,      me);
    QS_SIG_DICTIONARY(COM_RX_REQUEST_SIG,        me);
//    QS_SIG_DICTIONARY(Q_ENTRY_SIG,               me);
//    QS_SIG_DICTIONARY(Q_INIT_SIG,                me);
	CmdLineEntry lCmdTable[] =
    {
        //{"foo", AO_Demo, SYSTEM_CMD_DEMO_SIG, "The demo test command."},
        /*pmu commands*/
        {"ddrsw", AO_COMMON, BBU_DDR_SWEEP_SIG, "DDR sweep cmd"},
        {"ppset", AO_pmu, PMU_PPSET_SIG, "PP change command of the subsystem"},
        {"msa", AO_pmu, PMU_MSA_SIG, "MSA related tests"},
        {"dpeek",AO_COMMON,BBU_DPEEK_SIG,"dpeek command via DSP"},
        {"dpoke",AO_COMMON,BBU_DPOKE_SIG,"dpoke command via DSP"},
        {"peek",AO_COMMON,BBU_PEEK_SIG,"peek command"},
        {"poke",AO_COMMON,BBU_POKE_SIG,"poke command"},
        {"dfmem", AO_COMMON,BBU_DFMEM_SIG,"2MSA: Fill memory area with specified data"},
        {"dvmem", AO_COMMON,BBU_DVMEM_SIG,"2MSA: verify the memory area correctness, pair with dfmem"},
        {"dmemt", AO_COMMON,BBU_DMEMT_SIG,"2MSA: write and readback and compare data into memory area"},
        {"drc", AO_COMMON,BBU_DRC_SIG,"2MSA: test DSP's latency in MSA CPU cycles for reading one DDR word"},
        {"dwc", AO_COMMON,BBU_DWC_SIG,"2MSA: test DSP's latency in MSA CPU cycles for writting one DDR word"},
        {"reset", AO_COMMON, BBU_RESET, "software reset"},
        {"xtc",AO_COMMON,BBU_XTC_SIG,"set rtc/wtc command"},
        {"lpmset", AO_pmu, PMU_LPMSET_SIG, "LPM mode command of the subsystem"},
        {"opset", AO_pmu, PMU_OPSET_SIG, "OP change command of individual freq."},
        {"ipwk", AO_pmu, PMU_IPWK_SIG, "Inter-processor wakeup, AP wakeup CP or CP wakeup DSP."},

        {"timer", AO_Timer, TIMER_FROM_COM_SIG, "Timer test Command"},
        {"rtc", AO_Timer, TIMER_RTC_SIG, "RTC test Command"},
#ifdef CONFIG_GENERIC_TIME
		{"genct", AO_Timer, TIMER_GEN_TEST_SIG, "Generic count test Command"},
#endif


         /* i2c commands */
        {"i2c_scan",AO_I2C,I2C_SCAN_SIG,"I2C scan command"},
        {"ai2c",AO_I2C,I2C_ADDR_SIG,"I2C enter slave address command"},
        {"ri2c",AO_I2C,I2C_RD_TEST_SIG,"I2C read test command"},
        {"wi2c",AO_I2C,I2C_WR_TEST_SIG,"I2C write test command"},
        {"ii2c",AO_I2C,I2C_INIT_SIG,"I2C init command"},
        {"i2cm",AO_I2C,I2C_MODE_SIG,"I2C set mode command"},

        {"wdt",AO_COMMON,BBU_WDT_SIG,"watchdog test command"},
        {"pwm",AO_COMMON,BBU_PWM_SIG,"PWM test command"},
        {"dump",AO_COMMON,BBU_DUMP_SIG,"memory dump command"},
        {"fmem",AO_COMMON,BBU_FMEM_SIG,"Fill memory with data command"},
        {"mem_copy",AO_COMMON,BBU_MEMCOPY_SIG,"Copy memory from source to destination command"},
        {"memt",AO_COMMON,BBU_MEMT_SIG,"Standard BBU memory test command"},
        {"mcps", AO_COMMON, BBU_MCPS_SIG, "measure core frequency command"},
        {"vtpa", AO_COMMON, BBU_VTPA_SIG, "Virtual address to physical address command"},
        {"test", AO_COMMON, BBU_TEST_SIG, "common ao test command"},
		{"lcd", AO_COMMON, LCD_SIG, "lcd test command"},
        {"offline", AO_COMMON, CAM_OFFLINE_SIG, "camera offline test command"},
        {"camera_read_id", AO_COMMON, CAM_READ_ID_SIG, "camera read i2c id test command"},
        {"camera_init", AO_COMMON, CAM_ICAM_SIG, "camera init test command"},
        {"camera_online", AO_COMMON, CAM_ONLINE_SIG, "camera online test command"},
    
        {"camera_reg", AO_COMMON, CAM_REG_SIG, "camera reg test command"},

        {"camera_online_irq", AO_COMMON, CAM_ONLINE_IRQ_SIG, "camera online iqr test command"},

        {"dtc", AO_COMMON, BBU_DTC_SIG, "DTC test command"},
        {"sbit", AO_COMMON, BBU_SBIT_SIG, "Set bit test command"},
        {"cbit", AO_COMMON, BBU_CBIT_SIG, "Clear bit test command"},
        {"rbit", AO_COMMON, BBU_RBIT_SIG, "Read bit test command"},
        {"mgwp", AO_COMMON, BBU_MGWP_SIG, "MCU generate word pattern test command"},
        {"mfile", AO_COMMON, BBU_MFILE_SIG, "Memory file test command"},
        {"pmic", AO_COMMON, BBU_PMIC_SIG, "Get PMIC revision command"},
        {"sldo", AO_COMMON, BBU_SLDO_SIG, "Set VLDO voltage command"},
        {"gpio", AO_COMMON, BBU_GPIO_SIG, "Set GPIO command"},
#ifdef CONFIG_TMARK
        {"tmark", AO_COMMON, BBU_TMARK_SIG, "TMark baremetal test command"},
#endif
        {"pmtst", AO_COMMON,  BBU_DEVPM_SIG, "pmtst test command"},
        {"mfptst", AO_COMMON, BBU_MFPTEST_SIG, "mfp test command"},
        {"delay", AO_COMMON, BBU_DELAY_SIG, "delay second command"},
        {"mdelay", AO_COMMON, BBU_MDELAY_SIG, "delay msecond command"},
        {"print",AO_COMMON, BBU_PRINT_SIG, "Set print level command"},
        {"uart", AO_COMMON, BBU_UART_SIG, "Uart owner switch command"},

        {"cput", AO_COMMON, BBU_CPUT_SIG, "CPU test command"},
        {"cachebench", AO_COMMON,BBU_CACHEBENCH_SIG, "Cachebench test command"},
        {"scrt", AO_COMMON, BBU_SCRT_SIG, "Screening Test command"},
        {"clrs", AO_COMMON, BBU_CLRS_SIG, "Clear uart screen command"},
        {"ddrp", AO_COMMON, BBU_DDRP_SIG, "Set ddr priority command"},
        {"ddrw", AO_COMMON, BBU_DDRW_SIG, "Set ddr weight command"},
        {"ddrt", AO_COMMON, BBU_DDRT_SIG, "DDR write-read-check test command"},
        {"durm", AO_COMMON, BBU_DURM_SIG, "DDR utilization rate measurement command"},
        {"curm", AO_COMMON, BBU_CURM_SIG, "CPU utilization rate measurement command"},
        {"cmark",AO_COMMON, BBU_CMARK_SIG, "CoreMark test command"},
        {"linpack", AO_COMMON, BBU_LINPACK_SIG, "Linpack n=100 benchmark"},
        {"whetdc", AO_COMMON, BBU_WHET_SIG, "Whetstone benchmark"},
        {"dhry",   AO_COMMON, BBU_DHRY_SIG, "Dhrystone benchmark"},
#ifdef CONFIG_FFTS
        {"ffts",   AO_COMMON, BBU_FFTS_SIG, "The Fastest Fourier Transform in the South"},
#endif
        {"pdown", AO_COMMON, BBU_PDOWN, "software power down"},
        {"vibra_st", AO_COMMON, BBU_VIBRA_ST,"vibrator start command"},
        {"vibra_sp", AO_COMMON, BBU_VIBRA_SP,"vibrator stop command"},
        {"commt", AO_COMMON, BBU_COMMT_SIG,"Seagull&MSA reset and boot and digRF3 external loopback test command"},
        {"setv", AO_COMMON, BBU_SETV_SIG, "Set VBUCK voltage command"},
        {"mtsp", AO_COMMON, BBU_MTSP_SIG, "Memory Test Subroutine Package command"},
        {"smtd", AO_COMMON, BBU_SMTD_SIG, "Set Memory Test Defaults command"},
        {"pid",  AO_COMMON, BBU_PID_SIG,  "Get Processor ID command"},
        {"ipc",  AO_COMMON, BBU_IPC_SIG,  "IPC test command"},
        {"ripc",  AO_COMMON, BBU_RIPC_SIG,  "RIPC test command"},
        {"scmd",  AO_COMMON, BBU_SCMD_SIG,  "Send command to the other core"},
        {"cmda",  AO_COMMON,BBU_CMDA_SIG,"Send command to AP"},
        {"cmdd",  AO_COMMON,BBU_CMDD_SIG,"Send command to DSP"},

        {"stress",AO_SCHED, STRESS_STRT_SIG, "BBU Stress test command"},
        {"lucdwt", AO_STRESS, BBU_LUCDWT_SIG, "Mersenne number test of DR. Richard Crandall"},
        {"cpbp",   AO_STRESS,  STRESS_CPBP_SIG, "Calc and print the biggest prime"},
        {"super_pi",  AO_STRESS, SUPER_PI_SIG,   "Super Pi benchmark"},
        {"neonspeed", AO_COMMON, BBU_NEON_SPEED_SIG, "NEON speed benchmark"},
        {"pwddr", AO_COMMON, BBU_DDR_POWER_SIG, "DDR Power test command"},
#ifndef CONFIG_MMT

        //{"usbm",AO_USB,USB_IUSB_STORAGE,"USB storage test command"},
        //{"usbcharger",AO_USB,USB_CHARGER,"USB charger test"},
        {"usbclose",AO_USB,USB_CLOSE,"USB close command"},
        //{"usbspeaker",AO_USB,USB_SPEAKER,"USB speaker test"},
        //{"usbm_mmc",AO_USB,USB_MMC_STORAGE,"MMC based U-disk test"},
        //{"usbserial",AO_USB,USB_SERIAL,"USB serial test"},
        //{"usbecm",AO_USB,USB_ECM,"USB ecm test"},
        //{"usbdr",AO_USB,USB_DR,"USB dump register test"},
        //{"usbdqh",AO_USB,USB_DQH,"USB dump DQH test"},
        //{"usbcdc2",AO_USB,USB_CDC2,"USB cdc2 test"},
        //{"usbrndis",AO_USB,USB_RNDIS,"USB rndis test"},
        {"usbhost",AO_USB,USB_HOST_TEST,"USB host test"},
#endif
        {"malloc", AO_Cli, MAX_SIG, "BBU memory allocation test command"},

        {"dte", AO_COMMON, DTE_TEST_RES_SIG, "get DTE test result command"},

#ifdef QF_STACK_PER_AO
        {"stack",     AO_COMMON,  BBU_STACK_SIG, "AO stack test command"},
#endif

		{"help",   AO_Cli, MAX_SIG, "BBU help message print command"},
        {0,0,0,0}  //the terminate of the command table
    };

    memcpy(gCmdTable, lCmdTable,sizeof(lCmdTable));
	memset(test_result, 0, sizeof(test_result));

	return Q_TRAN(&Cli_processing);
}

static QState Cli_processing(Cli *me, QEvent const *e){
	switch(e->sig){
		case COM_RX_REQUEST_SIG:
		case COM_RX_NEXT_BYTE_SIG:
		case COM_RX_CMD_READY_SIG:{
#ifdef Q_SPY_EXT
			CliQspyComp_dispatch(&me->cliqspycomp, e);
#else
			CliComp_dispatch(&me->clicomp, e);
#endif
        	return Q_HANDLED();
		}
	}
	return Q_SUPER(&QHsm_top);
}
