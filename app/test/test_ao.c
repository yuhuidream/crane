/*
 *  test_ao.c
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */


#include "qp_port.h"
#include "bsp.h"
#include "utils.h"
#include "bbu.h"
#include "timer_dec.h"
#ifdef CONFIG_ATE
#include "ate_comp.h"
#include "ate_test.h"
#endif
#ifdef CONFIG_SCREEN_CHIP 
#include "screen_comp.h"
#include "screen_test.h"
#endif
#ifdef CONFIG_ATE_PE_FS
#include "bbu_mfile.h"
#include "bbu_fs.h"
#include "utils.h"
#endif
#ifdef CONFIG_SDH_API_TEST
#include "sd_ops.h"
#include "mmc_ops.h"
#endif


#define CMD_SLT_TEST (1 << 0)
#define CMD_ATE_TEST    (1 << 1)

//#define CONFIG_ATE_PE_FS  /* Move to bbu_conf file */

Q_DEFINE_THIS_FILE

typedef struct TESTTag {
	QActive super;
	QTimeEvt TestTimeEvt;
#ifdef CONFIG_ATE
	ATE_COMP *ate_comp;
#endif
#ifdef CONFIG_SCREEN_CHIP 
	SCREEN_COMP *screen_comp;
#endif
	uint8_t cur_test_src;
	uint8_t screen_cmd_index;
} TEST;

typedef enum script_file
{
	SCRIPT_TYPE_NONE,
	SCRIPT_TYPE_DDR,
	SCRIPT_TYPE_MMC,
	SCRIPT_TYPE_END,
} script_type;

typedef enum parse_status
{
	SCRIPT_LINE_IDLE,
	SCRIPT_LINE_VALID,
	SCRIPT_LINE_INVALID,
} script_line_status;

#ifdef CONFIG_ATE_PE_FS
typedef uint32_t (*load_cmd_fn)(uint32_t, uint32_t, char *, uint32_t);
static uint32_t load_cmd_table_from_emmc_sd(void);
static uint32_t ate_load_cmd_from_ddr_script(void);
static uint32_t slt_load_cmd_from_ddr_script(void);
#endif

/* hierarchical state machine ............................................. */
static QState test_initial(TEST *me, QEvent const *e);
static QState test_processing(TEST *me, QEvent const *e);
#ifdef CONFIG_ATE_PE_FS
static QState test_load_cmd_from_script(TEST *me, QEvent const *e);
#endif

static TEST l_test;
/* global objects.......................................................... */
QActive * const AO_TEST = (QActive *)&l_test;   /*opaque pointer to SDH */

void TEST_ctor(void)
{
	TEST *me = &l_test;;

	QActive_ctor(&me->super, (QStateHandler)&test_initial);
	QTimeEvt_ctor(&me->TestTimeEvt, AO_TEST_TIMEOUT_SIG);
#ifdef CONFIG_ATE
	me->ate_comp = (ATE_COMP *)ATE_COMP_ctor();
	me->ate_comp->father = AO_TEST;
#endif	

#ifdef CONFIG_SCREEN_CHIP 
	me->screen_comp = (SCREEN_COMP *)SCREEN_COMP_ctor();
	me->screen_comp->father = AO_TEST;
#endif

	return;
}

static QState test_initial(TEST *me, QEvent const *e)
{
	(void)e;
#ifdef CONFIG_ATE_PE_FS
	uint32_t mflag;

	mflag = init_ate_slt_scripts();
#endif
#ifdef CONFIG_ATE
	QHsm_init((QHsm *)(me->ate_comp), (QEvent *)0);
	ate_init();

#ifdef CONFIG_ATE_PE_FS
	if (mflag & BIT_1) {
		ate_load_cmd_from_ddr_script();
	} else {
		ate_load_cmd_from_local();
	}
#else
	ate_load_cmd_from_local();
#endif
	ate_start_show_live_status();
#endif
#ifdef CONFIG_SCREEN_CHIP 
	QHsm_init((QHsm *)(me->screen_comp), (QEvent *)0);
	screen_init();

#ifdef CONFIG_ATE_PE_FS
	if (mflag & BIT_0) {
		slt_load_cmd_from_ddr_script();
	} else {
		slt_load_cmd_from_local();
	}
#else
	slt_load_cmd_from_local();
#endif
#endif
	return Q_TRAN(&test_processing);
}

#ifdef CONFIG_ATE_PE_FS
static QState test_load_cmd_from_script(TEST *me, QEvent const *e)
{
	switch(e->sig) {
		case Q_ENTRY_SIG: {
			SDHInitReqEvt  *evt;
						
			bbu_printf("Start to init eMMC before loading script file......\r\n");
			
			evt = Q_NEW(SDHInitReqEvt, MMC_INIT_REQ_SIG);
			evt->AO_caller = AO_TEST;
			evt->auto_break = 0;
			QACTIVE_POST(AO_SDH,(QEvent *)evt, 0);
			
			/* 
			 * Disarm screen test time event first to ensure next arm command correct 
			*/
			QTimeEvt_disarm(&(l_test.TestTimeEvt));
			QTimeEvt_postIn(&(l_test.TestTimeEvt), AO_TEST, BSP_TICKS_PER_SEC * 5);
				
			return Q_HANDLED();			
		}
		
		case AO_TEST_TIMEOUT_SIG: {
			QTimeEvt_disarm(&(l_test.TestTimeEvt));
		}
		
		case MMC_INIT_FAIL_SIG: {
			bbu_printf("Warning: eMMC init failed, use default command sequence for ATE/Screen!\r\n");
			return Q_TRAN(&test_processing);
		}
		
		case MMC_INIT_SUC_SIG: {
			uint32_t err;

			mdelay(10);
			bbu_printf("Ready to mount fat32 filesystem from eMMC......\r\n");
			err = init_fs_block(AO_TEST, MOUNT_EMMC);
			if (err) {
				if (load_cmd_table_from_emmc_sd()) {
					screen_cmd_ack(SCREEN_TEST_FIAL, l_test.screen_cmd_index);
				} else {
					screen_cmd_ack(SCREEN_TEST_PASS, l_test.screen_cmd_index);
				}
			} else {
				bbu_printf("Warning: There is no valid fat32 filesystem in eMMC!\r\n");
				bbu_printf("Can use \"usbm_mmc\" to make eMMC-based U-Disk, "
				           "then copy script files into it \r\n");
				
				screen_cmd_ack(SCREEN_TEST_FIAL, l_test.screen_cmd_index);
			}
			
			return Q_TRAN(&test_processing);
		}	
	
	}

	return Q_SUPER(&QHsm_top);
}
#endif

static QState test_processing(TEST *me, QEvent const *e)
{
	switch(e->sig) {
		case Q_ENTRY_SIG: {
			return Q_HANDLED();
		}
	
#ifdef CONFIG_ATE
		case ATE_TEST_REQ_SIG:
	    	case ATE_TEST_RESULI_SIG:
	    	case ATE_TEST_TIMEOUT_SIG: 
	    	case ATE_TEST_CTRL_SIG: {
			QHsm_dispatch((QHsm *)(me->ate_comp), e);
			return Q_HANDLED();	
		}
#ifdef CONFIG_ATE_PE_FS
		case LOAD_ATE_CMD_SIG: {
			ComCmdEvt *cmde = (ComCmdEvt *)e;
			
			l_test.screen_cmd_index = cmde->scr_cmd_index;
			
			if(cmde->argc == 1 ||
			   (cmde->argc == 2 && !strcmp((const char*)cmde->argv[1], "local"))) {
				ate_load_cmd_from_local();
				bbu_printf("Success to restore default ATE command sequence!\r\n");
			} else if(cmde->argc == 1 ||
			          !strcmp((const char*)cmde->argv[1], "script")) {
				l_test.cur_test_src = CMD_ATE_TEST;
				return Q_TRAN(&test_load_cmd_from_script);
			}
			
			return Q_HANDLED();
		}
#endif
#endif

#ifdef CONFIG_SCREEN_CHIP
		case SCREEN_TEST_START_SIG:
		case SCREEN_TEST_ACK_SIG: 
		case SCREEN_TIMEOUT_SIG: {
			QHsm_dispatch((QHsm *)(me->screen_comp), e);
			return Q_HANDLED();	
		}
#ifdef CONFIG_ATE_PE_FS
		case LOAD_SLT_CMD_SIG: {
			ComCmdEvt *cmde = (ComCmdEvt *)e;
			l_test.screen_cmd_index = cmde->scr_cmd_index;
			
			if(cmde->argc == 1 ||
			   (cmde->argc == 2 && !strcmp((const char*)cmde->argv[1], "local"))) {
				slt_load_cmd_from_local();
				bbu_printf("Success to restore default SLT command sequence!\r\n");
			} else if(cmde->argc == 1 ||
			          !strcmp((const char*)cmde->argv[1], "script")) {
				l_test.cur_test_src = CMD_SLT_TEST;
				return Q_TRAN(&test_load_cmd_from_script);	
			}
			
			return Q_HANDLED();
		}
#endif
#endif

#ifdef CONFIG_SDH_API_TEST
		case AUTO_EMMC_SD_TEST: {
			uint32_t i, err;
			uint8_t *p, *q;
			
			err = sd_init_rtc_auto_break(AO_TEST);
			if (!err) {			
				bbu_printf("wirte sd:\r\n");
				sd_write_rtc_auto_break(AO_TEST, 0, 0x25400000, 10);
				bbu_printf("read sd\r\n");
				sd_read_rtc_auto_break(AO_TEST, 0, 0x1f100000, 10);
			
				p = 0x25400000;
				q = 0x1f100000;
			
				for (i = 0; i < 10*512; i++) {
					if (*p != *q)
						break;
				}
				if (i < 10*512)
					bbu_printf("sd read/write test failed!\r\n");
				else 
					bbu_printf("sd auto break mechanism success!\r\n");
			} else {
				bbu_printf("SD init failed!\r\n");
			}
			
			err = mmc_init_rtc_auto_break(AO_TEST);
			if (!err) {
				bbu_printf("wirte emmc:\r\n");
				mmc_write_rtc_auto_break(AO_TEST, 0, 0x25600000, 10);
				bbu_printf("read emmc\r\n");
				mmc_read_rtc_auto_break(AO_TEST, 0, 0x1f300000, 10);
			
				p = 0x25600000;
				q = 0x1f300000;
			
				for (i = 0; i < 10*512; i++) {
					if (*p != *q)
						break;
				}
				if (i < 10*512)
					bbu_printf("emmc read/write test failed!\r\n");
				else 
					bbu_printf("eMMC auto break mechanism success!\r\n");
			} else {
				bbu_printf("eMMC init failed!\r\n");
			}
			
			return Q_HANDLED();
		}
#endif
	}
	
	return Q_SUPER(&QHsm_top);
}

#ifdef CONFIG_ATE_PE_FS
static script_line_status parse_script_line_and_load(char* cmd, uint32_t line,
			load_cmd_fn load_command)
{
	uint32_t timeout = 0;
	uint32_t seq = 0;
	char *ptr_timeout = NULL;
	char *ptr_seq = NULL;

	ptr_timeout = strstr((char *)cmd, ",");
	if (ptr_timeout) {
		*ptr_timeout = '\0';					
	
		ptr_seq = strstr((char *)(ptr_timeout+1), ",");
		if (ptr_seq)
			*ptr_seq = '\0';					
	} else {
		ptr_seq = NULL;
	}
	
	if (cmd[0] == '\0')  /* skip blank line */
		return SCRIPT_LINE_IDLE;
	
	if (ptr_timeout) {
		char *ptr = ptr_timeout;
		uint32_t para_error = 0;
		uint8_t is_data = 0;
		
		do {
			ptr++;
			if (*ptr == ' ' || *ptr == '\t' ||
			    *ptr == ',') {
				if (is_data) {
					*ptr = '\0';
					break;
				} else {
					continue;
				}
			} else if (*ptr <= '9' && *ptr >= '0') {
				is_data = 1;
			} else if (*ptr == '\0') {
				break;
			} else {
				para_error = 1;
				break;
			}				
		} while(1);
		
		if (para_error)
			return SCRIPT_LINE_INVALID;
		
		if (is_data) {
			timeout = conv_dec(ptr_timeout + 1);
		}
		
	} else {
		/* default is 10 seconds */
		timeout = 10;
	}

	if (ptr_seq) {
		char *ptr = ptr_seq;
		uint32_t para_error = 0;
		uint8_t is_data = 0;
		
		do {
			ptr++;
			if (*ptr == ' ' || *ptr == '\t' ||
			    *ptr == ',') {
				if (is_data) {
					*ptr = '\0';
					break;
				} else {
					continue;
				}
			} else if (*ptr <= '9' && *ptr >= '0') {
				is_data = 1;
			} else if (*ptr == '\0') {
				break;
			} else {
				para_error = 1;
				break;
			}				
		} while(1);
		
		if (para_error)
			return SCRIPT_LINE_INVALID;
		
		if (is_data) {
			seq = conv_dec(ptr_seq + 1);
		}
		
	} else {
		seq = line;  /* if there is not seq parameter, default is index */
	}
	
	if (load_command(seq, line, (char *)cmd, timeout))
		return SCRIPT_LINE_INVALID;

	return SCRIPT_LINE_VALID;
}

static uint32_t parse_script_and_load_from_emmc(char *file_path, load_cmd_fn load_command)
{
	script_line_status status = SCRIPT_LINE_VALID;
	char* cmd;
	uint32_t err = 0;
	uint8_t i, j, k;
	uint8_t check_script_start = 0;

	FF_FILE * pFileRead;
	FF_T_INT32 size;
	FF_T_UINT32 line_size;
	
	pFileRead = (FF_FILE*) bbu_fopen((char *)file_path,
			(FF_T_INT8)FF_MODE_READ, AO_TEST);
	if (!pFileRead) {
		bbu_printf("Warning: File %s not found!\r\n", file_path);
		err = 1;
		return err;
	} 
	
	line_size = MAX_LINE_LEN;
	cmd = bbu_malloc(line_size);
	
	for (i = 0, j = 1; ; i++) {
		memset(cmd, 0, line_size);
		size = bbu_freadline(&cmd, &line_size, pFileRead, AO_TEST);
		if (!size) {
			err = 1;
			bbu_printf("Error: Script file is empty!\r\n");
			break;
		}
		/* Remove the LF and CR in each line first */
		for (k = 0; k < size; k++) {
			uint8_t end = 0;
	
			if (cmd[k] == '\r' || cmd[k] == '\n' ||
			    cmd[k] == '#') {
				cmd[k] = '\0';
				end = 1;
			}
	
			if (end)
				cmd[k] = '\0';
		}
		cmd[size+1] = '\0';

		if (strstr((char *)cmd, "COMMAND START")) {
			check_script_start = 1;
			continue;
		}
			
		if (strstr((char *)cmd, "COMMAND END"))
			break;

		if (!check_script_start)
			continue;

		status = parse_script_line_and_load(cmd, j, load_command);
		if (status == SCRIPT_LINE_INVALID)
			break;
		else if (status == SCRIPT_LINE_VALID)
			j++;
	}
	bbu_mfree(cmd);
	bbu_fclose(pFileRead, AO_TEST);
	
	if (status == SCRIPT_LINE_INVALID) {
		err = 1;
	}
	return err;
}

static uint32_t parse_script_and_load_from_ddr(char *file_path, load_cmd_fn load_command)
{
	script_line_status status = SCRIPT_LINE_VALID;
	char* cmd;
	uint32_t err = 0;
	uint8_t i, j, k;
	uint8_t check_script_start = 0;

	MFILE * pFileRead = NULL;
	int size, line_size;

	pFileRead = mfopen((const char *)file_path, "r");
	if (!pFileRead) {
		bbu_printf("Warning: File %s not found!\r\n", file_path);
		err = 1;
		return err;
	} 
	
	line_size = MAX_LINE_LEN;
	cmd = bbu_malloc(line_size);
	
	for (i = 0, j = 1; ; i++) {
		memset(cmd, 0, line_size);
		size = mfgetline(cmd, &line_size, pFileRead);
		if (size < 0) {
			err = 1;
			bbu_printf("Error: Script file line read failed!\r\n");
			break;
		}
		/* Remove the LF and CR in each line first */
		for (k = 0; k < size; k++) {
			uint8_t end = 0;
	
			if (cmd[k] == '\r' || cmd[k] == '\n' ||
			    cmd[k] == '#') {
				cmd[k] = '\0';
				end = 1;
			}
	
			if (end)
				cmd[k] = '\0';
		}
		cmd[size+1] = '\0';

		if (strstr((char *)cmd, "COMMAND START")) {
			check_script_start = 1;
			continue;
		}
			
		if (strstr((char *)cmd, "COMMAND END"))
			break;

		if (!check_script_start)
			continue;

		status = parse_script_line_and_load(cmd, j, load_command);
		if (status == SCRIPT_LINE_INVALID)
			break;
		else if (status == SCRIPT_LINE_VALID)
			j++;
	}
	bbu_mfree(cmd);
	mfclose(pFileRead);
	
	if (status == SCRIPT_LINE_INVALID) {
		err = 1;
	}
	return err;
}

static uint32_t load_cmd_table_from_emmc_sd(void)
{
	uint32_t err = 0;
	
#ifdef CONFIG_ATE	
	/* 
	 * First load ATE command table 
	 */
	if (l_test.cur_test_src & CMD_ATE_TEST) {
		ate_free_group_list();
		l_test.cur_test_src &= ~CMD_ATE_TEST;
		bbu_printf("\r\nBegin to load ATE command table from \"ate_cmd.txt\"...\r\n");
		err = parse_script_and_load_from_emmc((char *)"EMMC:/ate_cmd.txt", ate_add_cmd_to_group);
		if (err) {
			ate_load_cmd_from_local();
			bbu_printf("Warning: Load failed! Use default "
				   "ATE command sequence!\r\n");
		} else {
			bbu_printf("Success: Load finished!\r\n");
		}
		
		ate_cmd_init();
		return err;
	}
#endif	

#ifdef CONFIG_SCREEN_CHIP
	/* 
	 * Then load Screen command table 
	 */
	if (l_test.cur_test_src & CMD_SLT_TEST) {
		slt_free_group_list();
		l_test.cur_test_src &= ~CMD_SLT_TEST;
		bbu_printf("Begin to load SLT command table from \"slt_cmd.txt\"...\r\n");
		err = parse_script_and_load_from_emmc((char *)"EMMC:/slt_cmd.txt", slt_add_cmd_to_group);
		if (err) {
			slt_load_cmd_from_local();
			bbu_printf("Warning: Load failed! Use default "
				   "SLT command sequence!\r\n");
		} else {
			bbu_printf("Success: Load finished!\r\n");
		}
		slt_update_cmd_number();
		return err;
	}
#endif	
	return 1;
}

#ifdef CONFIG_ATE
static uint32_t ate_load_cmd_from_ddr_script(void)
{
	uint32_t err = 0;

	ate_free_group_list();
	l_test.cur_test_src &= ~CMD_ATE_TEST;
	//bbu_printf("\r\nMFILE: load ATE command table from \"ate_cmd.txt\"...\r\n");
	err = parse_script_and_load_from_ddr((char *)"ate_cmd.txt", ate_add_cmd_to_group);
	if (err) {
		ate_load_cmd_from_local();
		bbu_printf("Warning: Load failed! Use default "
			   "ATE command sequence!\r\n");
	} else {
		//bbu_printf("Success: Load finished!\r\n");
	}
	
	ate_cmd_init();
	return err;
}
#endif

#ifdef CONFIG_SCREEN_CHIP
static uint32_t slt_load_cmd_from_ddr_script(void)
{
	uint32_t err = 0;

	slt_free_group_list();
	l_test.cur_test_src &= ~CMD_SLT_TEST;
	//bbu_printf("MFILE: load SLT command table from \"slt_cmd.txt\"...\r\n");
	err = parse_script_and_load_from_ddr((char *)"slt_cmd.txt", slt_add_cmd_to_group);
	if (err) {
		slt_load_cmd_from_local();
		bbu_printf("Warning: Load failed! Use default "
			   "SLT command sequence!\r\n");
	} else {
		//bbu_printf("Success: Load finished!\r\n");
	}
	slt_update_cmd_number();
	return err;
}
#endif

#endif  /* CONFIG_ATE_PE_FS */
