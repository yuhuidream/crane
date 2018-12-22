/*
 *  Screen Test 
 *
 *  This file contain program to post auto screen test commands to AOs,and handle their 
 *  response, so that at the end of auto screen test output screen test reports.
 *
 *  For command you want join in auto screen test, just add your command in the 
 *  screen_table below(parameter should be contained), then send response
 *  event ScreenCmdAck(contain signal COM_RX_SCREEN_ACK_SIG) to AO_Cli.
 *  
 */
#include "bbu.h"
#include "bsp.h"
#include <string.h>
#include <stdlib.h>
#include "tri_led.h"
#include "timer_dec.h"
#include "gpio_dec.h"
#include "screen_comp.h"
#include "bbu_test.h"
#include "screen_test.h"
#include "qassert.h"
#include "list.h"

Q_DEFINE_THIS_MODULE("screen_test.c")

typedef struct
{
	char cmd[MAX_LINE_LEN];
	
	/*
	 * Command does not receive response before timeout_deadline arrived
	 * will be assumed failed
	 */
	uint32_t timeout_deadline;
	
	/*
	 * select which group this command will locate in.
	 * if equal to zero, default group_id will be command index
	 */
	uint32_t group_id;
} screen_test_entry;

/* struct slt_entry used to manage all commands in one group */
struct slt_entry
{
	struct list_head entry_list;
	
	/*
	 * cmd_index is an ID used to discrimiate which command 
	 * result ACK belong to
	 */
	uint32_t cmd_index;
	
	/* command name */
	char cmd[MAX_LINE_LEN];
	
	/*
	 * Command does not receive response before timeout_deadline arrived
	 * will be assumed failed
	 */
	uint32_t timeout_deadline;
	
	/* Used to save time consumed by each command */
	uint32_t time_used_ms;
	
	/*
	 * Commands in same group_id will be post at the same time,
	 * And group of samll group_id will be post firstly
	 */
	uint32_t group_id;
	
	/*
	 * Used to save command execution result
	 *
	 * 0x00 -- initial value to show command not execute yet,
	           and once executed not count repeatedly
	 * 0x01 -- success 
	 * 0x02 -- failure 											
	 * 0x03 -- completion, need people to check result 
	 * 0x04 -- command execution time out 
	 * 0x05 -- not supported command
	 */
	uint8_t result;
};

/* struct slt_sub_group used to manage all groups */
struct slt_sub_group
{
	struct list_head sub_gp_list;
	uint8_t group_id;
	
	/*
	 * group_max_timeout is the maximum timeout value of all commands of
	 * this group
	 */
	uint32_t group_max_timeout;
	
	/* flag used to show if this group finish all commands  */
	uint32_t group_stat;
#define SLT_GR_INIT 0
#define SLT_GR_DONE 1
	
	/* total command number of this group */
	uint32_t cmd_num;
	
	/* head point of commands list */
	struct list_head cmd_head;
};

/* Function declaration, just can be called in this file */
static uint32_t slt_load_cmd_from_table(void *table_addr);
static void bbu_screen_test_help(void);
static inline void slt_display_cmd_info(void);
static void slt_test_report(void);
static uint8_t screen_test_run(void);

/*
 * local static variable
 */
static uint32_t screen_cmd_number;
/* control wether screen test stop when meeting error */
static uint8_t toend;
/* list head of all group */
static struct list_head sub_group_head;

#define SLT_CMD_CELL(_name, _timeout, _group_id)        \
{                                                       \
	.cmd = _name,                                   \
	.timeout_deadline = _timeout,                   \
	.group_id = _group_id,                          \
}
#define SLT_CMD_END SLT_CMD_CELL("\0", 0, 0)

/* local slt test table */
static screen_test_entry local_screen_table[] = 
{
    /*
     * put integral commands here to join in auto screen test. 
     * For example "ledt g", quotes are needed! 
    */
#if defined(CONFIG_SCREEN_BOARD) || defined(CONFIG_SCREEN_CHIP) 
/******************************************************************************
 *                           Chip Test Start                                  *
 ******************************************************************************/
	SLT_CMD_CELL("pid", 10, 0),
	SLT_CMD_CELL("commt 1 1 0", 10, 0),
	SLT_CMD_CELL("lpmset 3 1", 1, 0),
	SLT_CMD_CELL("lpmset 3 1", 1, 0),
	SLT_CMD_CELL("lpmset 3 1", 1, 0),
	SLT_CMD_CELL("lpmset 9 7 5 1 1 3", 5, 0),  /* 1s is not enough on some socket board at Korea AE, use 3s instead */

//	SLT_CMD_CELL("usbclose", 10, 0),
//	SLT_CMD_CELL("cput", 10, 0),  // Tests cpu write a large memory with different data 
	SLT_CMD_CELL("ilcd", 10, 0),

//#if defined(CONFIG_AUTO)
	SLT_CMD_CELL("setv 1 1375 11", 10, 0),
//#endif

//	SLT_CMD_CELL("ppset 200 1", 10, 0),  //hard to pass
	SLT_CMD_CELL("ppset 6", 10, 0),
	SLT_CMD_CELL("mtsp 0x00000000 0x80000 0xffffffff", 10, 0),
	SLT_CMD_CELL("ppset 45", 10, 0),
	SLT_CMD_CELL("mcps", 10, 0),

	SLT_CMD_CELL("pmic", 10, 0),
	SLT_CMD_CELL("ikey", 5, 0),
	SLT_CMD_CELL("mtsp 0x30000000 0x400000 0xffffffff", 10, 0),
	SLT_CMD_CELL("dma mdma 0", 10, 0),

#ifdef CONFIG_VPU    
	SLT_CMD_CELL("vpu dec 0 5", 10, 0),  // decode AVC
	SLT_CMD_CELL("vpu enc 0 5", 10, 0),  // encode AVC
#endif

#ifdef CONFIG_H264
	SLT_CMD_CELL("h264 3", 10, 0),  //used to test NEON
#endif

#ifdef CONFIG_VFP
	SLT_CMD_CELL("vfpt float32_add", 10, 0),
#endif

	SLT_CMD_CELL("set_ao stress 1", 10, 0),
	SLT_CMD_CELL("mlucas -iters 2", 60, 0),
	SLT_CMD_CELL("set_ao stress 2", 10, 0),
	SLT_CMD_CELL("mlucas -iters 2", 60, 0),
	SLT_CMD_CELL("set_ao stress 3", 10, 0),
	SLT_CMD_CELL("mlucas -iters 2", 60, 0),

#ifdef CONFIG_AUDIO
	SLT_CMD_CELL("audio i2s play 1", 10, 0),
	SLT_CMD_CELL("audio i2s record 0x05000000 1", 10, 0),
	SLT_CMD_CELL("audio pcm play 1", 10, 0),
	SLT_CMD_CELL("audio pcm record 0x05000000 1", 10, 0),
#endif 
	SLT_CMD_CELL("geu  geudiag 3 1", 10, 0),
	SLT_CMD_CELL("fuseread", 10, 0),

#ifdef CONFIG_GCU
	SLT_CMD_CELL("gcu3d mem 0", 10, 0),  // 3D texture7 test
	SLT_CMD_CELL("gcu2d mem 0", 10, 0),  // 2D bitcopy test
	SLT_CMD_CELL("gcu3drot", 60, 0),  // remove it because it's not test in PE and can't pass at Korea AE.
#endif

#ifdef CONFIG_ISP
#ifdef CONFIG_FVTS
	SLT_CMD_CELL("sfvts", 1, 0),
#else
	SLT_CMD_CELL("icam 1 1 1", 10, 0),
	SLT_CMD_CELL("delay 2", 5, 0),
#endif
#endif
	SLT_CMD_CELL("icam 0 1 1", 10, 0),

	SLT_CMD_CELL("isd", 2, 0),
	SLT_CMD_CELL("sd_test -b", 10, 0),  /* basic sd read/write test */
	SLT_CMD_CELL("usbm", 10, 0),
	//SLT_CMD_CELL("setv 1", 10, 0),
	SLT_CMD_CELL("ii2c", 10, 0),
	SLT_CMD_CELL("i2c_scan 1", 10, 0),
	SLT_CMD_CELL("i2c_scan 2", 10, 0),
	SLT_CMD_CELL("i2c_scan 3", 10, 0),
	//SLT_CMD_CELL("stress pe 1", 150, 0),

/******************************************************************************
 *                           Chip Test End                                    *
 ******************************************************************************/

#if defined(CONFIG_SCREEN_BOARD)
/******************************************************************************
 *                           Board Test Start                                 *
 ******************************************************************************/

	SLT_CMD_CELL("baro", 10, 0),  /* barometer test */

/******************************************************************************
 *                           Board Test End                                   *
 ******************************************************************************/
#endif

#endif  /* End of defined(CONFIG_SCREEN_BOARD) || defined(CONFIG_SCREEN_CHIP) */
	SLT_CMD_END,
};

/*
 * Following two tables are used for DDR tuning stress test
 */
static const screen_test_entry ddr533_table[] = 
{
	SLT_CMD_CELL("ppset 49", 2, 0),
	SLT_CMD_CELL("stress ddr 10", 2000, 0),
	
	SLT_CMD_END,
};

static const screen_test_entry ddr400_table[] = 
{
	SLT_CMD_CELL("ppset 50", 2, 0),
	SLT_CMD_CELL("stress ddr 10", 2000, 0),
	
	SLT_CMD_END,
};

static const screen_test_entry ddr312_table[] = 
{
	SLT_CMD_CELL("ppset 48", 2, 0),
	SLT_CMD_CELL("stress ddr 10", 2000, 0),

	SLT_CMD_END,
};

static const screen_test_entry ddr156_table[] = 
{
	SLT_CMD_CELL("ppset 47", 2, 0),
	SLT_CMD_CELL("stress ddr 10", 2000, 0),

	SLT_CMD_END,
};

/**
 * screen_handler
 *
 * Screen Test function, three actions:
 * 1) Output help information of this command
 * 2) Output a list consisting of commands joining in auto screen test
 * 3) Run auto screen test.
 */
uint8_t screen_handler(int argc, uint8_t **argv)
{
	uint8_t screen_run_flag = 0;

	switch (argc) {
	case 1:
		/* Without parameter, start to execute auto screen test */
		if (screen_test_run()) {
			toend = 0;
			screen_run_flag = 1;
		}
		break;
	case 2:
		if (!strcmp((const char*)argv[1], "/h") || 
		    !strcmp((const char*)argv[1], "--help")) {
			bbu_screen_test_help();
		} else if (!strcmp((const char*)argv[1], "toend")) {
			if (screen_test_run()) {
				/* 
				 * Continue test when error happen 
				 */
				toend = 1;
				screen_run_flag = 1;
			}	
		} else if (!strcmp((const char*)argv[1], "ddr533")) {
			slt_load_cmd_from_table((void *)ddr533_table);
			if (screen_test_run()) {
				toend = 0;
				screen_run_flag = 1;
			}
		} else if (!strcmp((const char*)argv[1], "ddr400")) {
			slt_load_cmd_from_table((void *)ddr400_table);
			if (screen_test_run()) {
				toend = 0;
				screen_run_flag = 1;
			}
		} else if (!strcmp((const char*)argv[1], "ddr312")) {
			slt_load_cmd_from_table((void *)ddr312_table);
			if (screen_test_run()) {
				toend = 0;
				screen_run_flag = 1;
			}
		} else if (!strcmp((const char*)argv[1], "ddr156")) {
			slt_load_cmd_from_table((void *)ddr156_table);
			if (screen_test_run()) {
				toend = 0;
				screen_run_flag = 1;
			}
		} else if (!strcmp((const char*)argv[1], "-d")) {
			slt_display_cmd_info();
		} else {
			bbu_printf("screen-->  The argument not correct\n\r");
		}
		break;
	default:
		bbu_printf("screen-->  Too many arguments\n\r");
		break;
	}	
	return screen_run_flag;
}

void screen_init(void)
{
	INIT_LIST_HEAD(&sub_group_head);
}

void slt_update_cmd_number(void)
{
	struct slt_sub_group *sub_gp;
	
	screen_cmd_number = 0;
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		screen_cmd_number += sub_gp->cmd_num;
	}
}

static uint32_t slt_load_cmd_from_table(void *table_addr)
{
	screen_test_entry *entry = table_addr;
	uint32_t i;
	uint32_t gp_id;
	
	slt_free_group_list();
	for (i = 0; strlen(entry->cmd) != 0; i++) {
		if (!entry->group_id)
			gp_id = i+1;
		else
			gp_id = entry->group_id;
		
		slt_add_cmd_to_group(gp_id, i+1, entry->cmd,
		                     entry->timeout_deadline);
		entry++;
	}
	
	slt_update_cmd_number();
	return 0;
}

uint32_t slt_load_cmd_from_local(void)
{
	return slt_load_cmd_from_table((void *)local_screen_table);
}

void slt_free_group_list(void)
{
	struct slt_sub_group *sub_gp;
	struct slt_entry *cmd_entry;
	
	if (list_empty(&sub_group_head))
		return;
	
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
			bbu_mfree(cmd_entry);
		}
		bbu_mfree(sub_gp);
	}
	INIT_LIST_HEAD(&sub_group_head);
	return;
}

uint32_t slt_add_cmd_to_group(uint32_t seq, uint32_t index, char *cmd, uint32_t timeout)
{
	struct slt_sub_group *sub_gp;
	struct slt_entry *cmd_entry;
	
	/* NULL command will not add to group */
	if (!cmd || cmd[0] == '\0')
		return 1;
	
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		if (sub_gp->group_id == seq) {
			cmd_entry = bbu_malloc(sizeof(struct slt_entry));
			strcpy(cmd_entry->cmd, cmd);
			cmd_entry->time_used_ms = 0xffffffff;
			cmd_entry->result = BBU_TEST_INIT;
			cmd_entry->cmd_index = index;
			cmd_entry->timeout_deadline = timeout;
			list_add_tail(&cmd_entry->entry_list, &sub_gp->cmd_head);
			sub_gp->cmd_num++;
			if (cmd_entry->timeout_deadline > sub_gp->group_max_timeout)
				sub_gp->group_max_timeout = cmd_entry->timeout_deadline;
			
			break;
		}
	}
	
	if (&sub_gp->sub_gp_list == &sub_group_head) {
		struct slt_sub_group *sub_gp_new, *next;
		sub_gp_new = bbu_malloc(sizeof(struct slt_sub_group));
		sub_gp_new->group_id = seq;
		sub_gp_new->cmd_num = 1;
		sub_gp_new->group_stat = SLT_GR_INIT;
		sub_gp_new->group_max_timeout = timeout;
		INIT_LIST_HEAD(&sub_gp_new->cmd_head);
		
		list_for_each_entry_safe(sub_gp, next, &sub_group_head,
		                         sub_gp_list) {
			if (sub_gp_new->group_id < sub_gp->group_id) {
				break;
			}
		}
		list_add(&sub_gp_new->sub_gp_list, sub_gp->sub_gp_list.prev);
		
		cmd_entry = bbu_malloc(sizeof(struct slt_entry));
		strcpy(cmd_entry->cmd, cmd);
		cmd_entry->time_used_ms = 0xffffffff;
		cmd_entry->result = BBU_TEST_INIT;
		cmd_entry->cmd_index = index;
		cmd_entry->timeout_deadline = timeout;
		list_add_tail(&cmd_entry->entry_list, &sub_gp_new->cmd_head);
	}
	return 0;
}

static void slt_post_all_cmd_in_group(struct slt_sub_group *sub_gp)
{
	struct slt_entry *cmd_entry;
	
	sub_gp->group_stat = SLT_GR_INIT;
	list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
		ScreenCmdEvt *sce = Q_NEW(ScreenCmdEvt,COM_RX_CMD_READY_SIG);
		
		sce->len = (uint8_t)strlen(cmd_entry->cmd);
		sce->cmd_index = (cmd_entry->cmd_index) | TEST_ACK_FROM_SCREEN;
		sce->source = 1;
		strcpy((char *)sce->cmd, cmd_entry->cmd);
		
		bbu_printf("\r\n\r\nScreen test \"%s\" has been sent out......\r\n",
		           cmd_entry->cmd);
		bbu_printf("\r\nWaiting ACK......\r\n\r\n");		
		cmd_entry->time_used_ms = bbu_get_msec();
		QACTIVE_POST(AO_Cli, (QEvent *)sce, 0);
	}
	
	/* First screen command is sleep test, just need 1s timeout */  
	QTimeEvt_postIn(&(l_screen_comp.ScreenTestTimeEvt), AO_TEST,
	                BSP_TICKS_PER_SEC * sub_gp->group_max_timeout);
}

static inline uint8_t slt_check_if_group_finished(struct slt_sub_group *sub_gp)
{
	struct slt_entry *cmd_entry;
	
	list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
		if (cmd_entry->result == BBU_TEST_INIT)
			return 0;
	}
	return 1;
}

static struct slt_sub_group *slt_update_cmd_result(uint32_t cmd_index, uint8_t result)
{
	struct slt_sub_group *sub_gp;
	struct slt_entry *cmd_entry;
	
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
			if (cmd_entry->cmd_index == cmd_index) {
				/*
				 * if command already executed, just ignore
				 * new result
				 */
				if (cmd_entry->result != BBU_TEST_INIT)
					return NULL;
				cmd_entry->result = result;
				cmd_entry->time_used_ms = bbu_get_msec() - 
					                  cmd_entry->time_used_ms;
				return sub_gp;
			}
		}
	}
	return NULL;
}

static inline void slt_reset_status(void)
{
	struct slt_sub_group *sub_gp;
	struct slt_entry *cmd_entry;
	
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
			cmd_entry->time_used_ms = 0xffffffff;
			cmd_entry->result = BBU_TEST_INIT;
		}
		sub_gp->group_stat = SLT_GR_INIT;
	}	
	return;
}

/**
 * screen_test_run
 *
 * After AO_Cli received screen test command,this funciton scans screen_table, 
 * and send event containing command to AO_Cli, then  AO_Cli analyze this event
 * just like receive commands from UART, and dispatch it to related AO to
 * respond requests. 
*/
static uint8_t screen_test_run(void)
{
	struct slt_sub_group *sub_gp;
	
	/*
	 * every time screen test start, need to clear previous test 
	 * status before starting a new one
	 */
	/* reset all group commands status */
	slt_reset_status();
	/* reset LED display */
	led_red_off();
	led_green_off();
	/* reset LCD display */
	//LCD_Color(RGB_BLUE, lcd_buffer);
	//LCD_Welcome();
	
	if (!screen_cmd_number) {
		bbu_printf("\r\n\tAuto Screen Test Table is Empty!\r\n");
		return 0;
	}
	
	mdelay(20);
	bbu_printf("\r\n-------------------------------------------------------------\r\n");
	bbu_printf("-------------------------------------------------------------\r\n");
	bbu_printf("---- The bbu screen test start,Please wait for a moment! ----\r\n");
	bbu_printf("---------------- Total Test Commands: %d ------------------\r\n",
	           screen_cmd_number);
	bbu_printf("-------------------------------------------------------------\r\n");
	bbu_printf("-------------------------------------------------------------\r\n");
	
	sub_gp = list_first_entry(&sub_group_head, struct slt_sub_group, sub_gp_list);
	slt_post_all_cmd_in_group(sub_gp);
	return 1;
}

/**
 * collect_screen_result
 *
 * Handle ACK from AOs, then record the events in order to 
 * output screen test reports after the last command executed.
 */
uint8_t collect_screen_result(const ScreenCmdAck *e)
{
	struct slt_sub_group *sub_gp, *next;
	uint8_t cmd_index;

	cmd_index = (e->cmd_index) & (~TEST_ACK_FROM_SCREEN);
	if (!cmd_index)
		return 0;
	
	sub_gp = slt_update_cmd_result(cmd_index, e->result);
	/*
	 * If this group already finished, just update the result and exit
	 * Note: lack of this may lead to repeated test
	 */
	if (!sub_gp || sub_gp->group_stat == SLT_GR_DONE)
		return 0;
	
	/* Here control whether to terminate test when meeting failed command */
	if (!toend) {
		if (e->result == BBU_TEST_FAIL || 
		    e->result == BBU_TEST_TIMEOUT) {
			slt_test_report();		
			/* disarm screen test time event when over */
			QTimeEvt_disarm(&(l_screen_comp.ScreenTestTimeEvt));  
			return 1;
		}
	}
	
	if (!slt_check_if_group_finished(sub_gp)) {
		QTimeEvt_rearm(&(l_screen_comp.ScreenTestTimeEvt),
	                       BSP_TICKS_PER_SEC * sub_gp->group_max_timeout);
		return 0;
	}
	QTimeEvt_disarm(&(l_screen_comp.ScreenTestTimeEvt));
	sub_gp->group_stat = SLT_GR_DONE;
	
	if (list_is_last(&sub_gp->sub_gp_list, &sub_group_head)) {
		slt_test_report();
		/* disarm screen test time event when over */
		QTimeEvt_disarm(&(l_screen_comp.ScreenTestTimeEvt));
		return 1;
	}
	
	next = list_entry(sub_gp->sub_gp_list.next, typeof(*sub_gp),
	                  sub_gp_list);
	mdelay(5);  /* delay 5ms between test group */
	slt_post_all_cmd_in_group(next);

	return 0;
}

/**
 * screen_timeout_handle
 *
 * Description:
 * If not receiving ACK before timeout happen, post a timeout event to myself,
 * an exception is command "lpmset 3 1", which do not have ack itself, will
 * make a fake pass event to myself 
 */
void screen_timeout_handle(void)
{
	struct slt_sub_group *sub_gp;
	struct slt_entry *cmd_entry;	
	
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		if (sub_gp->group_stat == SLT_GR_DONE)
			continue;
		list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
			if (cmd_entry->result == BBU_TEST_INIT) {
				if (strcmp(cmd_entry->cmd, "lpmset 3 1") == 0) {
					/*
					 * For "lpmset 3 1", after 1s timeout, 
					 * let screen test go on with wakeup test.
					 */
					screen_cmd_ack(BBU_TEST_PASS, 
					    (cmd_entry->cmd_index |
					     TEST_ACK_FROM_SCREEN));
				} else {
					screen_cmd_ack(BBU_TEST_TIMEOUT,
					    (cmd_entry->cmd_index|
					     TEST_ACK_FROM_SCREEN));				
				}
			}
		}
		break;
	}
}

static void slt_test_report(void)
{
	struct slt_sub_group *sub_gp;
	struct slt_entry *cmd_entry;
	uint32_t total_failed = 0;
	uint32_t total_passed = 0;
	uint32_t total_under_checked = 0;
	uint32_t total_timeout = 0;
	uint32_t total_unknown = 0;
	uint32_t total_undo = 0;
	uint32_t index = 0;
	
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
			if(cmd_entry->result == BBU_TEST_PASS)
				total_passed++;
			else if(cmd_entry->result == BBU_TEST_FAIL)		
				total_failed++;
			else if(cmd_entry->result == BBU_TEST_CHECK)
				total_under_checked++;
			else if(cmd_entry->result == BBU_TEST_TIMEOUT)
				total_timeout++;
			else if(cmd_entry->result == BBU_TEST_INIT)
				total_undo++;
			else
				total_unknown++;
		}	
	}

	bbu_printf("\r\n\r\n----------------------------------------------\r\n");
	bbu_printf("----------------------------------------------\r\n");
	bbu_printf("\tScreen Test Commands Finished!\r\n");
	bbu_printf("\tTest Report is Below:\r\n");
	bbu_printf("----------------------------------------------\r\n");
	bbu_printf("----------------------------------------------\r\n");
	
	bbu_printf("\r\n\tTotal command number: %d\r\n",screen_cmd_number);
	bbu_printf("\r\n\t1) Success Command number: %d\r\n",total_passed);
	bbu_printf("\r\n\t2) Failed Command number: %d \r\n",total_failed);
	bbu_printf("\r\n\t3) Command completed but need check: %d\r\n",total_under_checked);
	bbu_printf("\r\n\t4) Timeout command number: %d\r\n",total_timeout);
	if (total_undo)
		bbu_printf("\r\n\t5) Not started command number: %d\r\n",total_undo);

	bbu_printf("\r\n----------------------------------------------\r\n");

	if (total_under_checked != 0) {
		bbu_printf("\r\n\tFollowing commands completed but need check:\r\n");
		
		index = 0;
		list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
			list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
				if(BBU_TEST_CHECK == cmd_entry->result) {
					index++;						
					bbu_printf("\t%d): %s\r\n", index, cmd_entry->cmd);
				}
			}
		}
		bbu_printf("----------------------------------------------\r\n");
	}		
	
	if (total_failed) {
		bbu_printf("\r\n\tFollowing commands Failed:\r\n");			
		
		index = 0;
		list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
			list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
				if(BBU_TEST_FAIL == cmd_entry->result) {
					index++;						
					bbu_printf("\t%d): %s\r\n", index, cmd_entry->cmd);
				}
			}
		}
		bbu_printf("----------------------------------------------\r\n");
	}

	if (total_timeout) {
		bbu_printf("\r\n\tFollowing commands Failed:\r\n");			
		
		index = 0;
		list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
			list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
				if(BBU_TEST_TIMEOUT == cmd_entry->result) {
					index++;						
					bbu_printf("\t%d): %s\r\n", index, cmd_entry->cmd);
				}
			}
		}
		bbu_printf("----------------------------------------------\r\n");
	}
		
	if (total_failed == 0 && total_timeout == 0) {
		bbu_printf("\r\nPASS\r\n");
		/*  except commands needed human to check result  */
		printpass();
#ifdef CONFIG_MMT
		bbu_putchar(0x82);
#endif
		/*  LED green on to show screen test passed  */
		led_green_on();
		/*  LCD to show test pass  */
		//lcd_disp_pass();
		//POST_CMD2CLI("stress");
	} else if (total_failed || total_timeout) {
		bbu_printf("\r\nFAIL\r\n");
#ifdef CONFIG_MMT
		bbu_putchar(0x83);
#endif
		printfail();
		/* LED red on to show screen test failed */
		led_red_on();
		/* LCD to show test fail */
		//lcd_disp_fail();
	}
	bbu_printf("\r\n-------------------------------------------------------------\r\n");
	bbu_printf("-------------------------------------------------------------\r\n");
	bbu_printf("---------------- The bbu screen test end! -----------------\r\n");
	bbu_printf("-------------------------------------------------------------\r\n");
	bbu_printf("-------------------------------------------------------------\r\n");
	
	bbu_printf(BBU_PROMPT);
}

static inline void bbu_screen_test_help(void)
{
    bbu_printf("\r\n screen - BBU screen test command\r\n");
    bbu_printf("\r\n Usage :screen [<option>]\r\n");
    bbu_printf("\r\n Description:\r\n");
    bbu_printf(" This command perform a auto test with no manual intervention");
    bbu_printf( "based an auto test command table!\r\n");
    
    bbu_printf("\r\n Parameter:\r\n");
    bbu_printf(" <option>: If omitted, begin to run auto screen test.\r\n");
    bbu_printf("   -d     -- Inquire what commands join in auto screen test.\r\n");
    bbu_printf("   toend  -- Run all test even error occur\r\n");

    bbu_printf(BBU_PROMPT);
}

/**
 * Output a list of commands joining in auto screen test based screen table.
 */
static inline void slt_display_cmd_info(void)
{
	struct slt_sub_group *sub_gp;
	struct slt_entry *cmd_entry;

	bbu_printf("\r\n%10s%10s%40s%20s%20s%20s\r\n", "Group", "index", "BBU Test", 
	           "Result", "Ind Time(ms)", "Timeout(seconds)");
	
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
			uint8_t result[20];
			
			if (cmd_entry->result == BBU_TEST_PASS) {
				strcpy((char *)result, "Pass");
			} else if (cmd_entry->result == BBU_TEST_FAIL) {
				strcpy((char *)result, "Fail");
			} else if (cmd_entry->result == BBU_TEST_TIMEOUT) {
				strcpy((char *)result, "Timeout");
			} else if (cmd_entry->result == BBU_TEST_CHECK) {
				strcpy((char *)result, "Check");
			} else {
				strcpy((char *)result, "NA");
			}

			if (cmd_entry->time_used_ms == 0xffffffff) {
				bbu_printf("%10d%10d%40s%20s%20s%20d\r\n", 
				           sub_gp->group_id, cmd_entry->cmd_index, cmd_entry->cmd,
				           result, "NA", cmd_entry->timeout_deadline);
			} else {
				bbu_printf("%10d%10d%40s%20s%20d%20d\r\n",
				           sub_gp->group_id, cmd_entry->cmd_index, cmd_entry->cmd,
				           result, cmd_entry->time_used_ms,
				           cmd_entry->timeout_deadline);
			}
		}
	}	
	bbu_printf(BBU_PROMPT);
}

/*
 * Screen Test result return function
 */
inline void screen_test_cmd_ack(uint8_t result, uint8_t cmd_index)
{
	/*
	 * if command not come from screen test, do not need to return result 
	 */
	if(cmd_index) {
		ScreenCmdAck *sca = Q_NEW(ScreenCmdAck, SCREEN_TEST_ACK_SIG);
		/*
		 * just send command index back so that AO_Cli can 
		 * distinguish each command, must do it
		 */
		sca->cmd_index = cmd_index;  
		
		/*
		 * Depend on your command execute return value
		 * 0x01 -- success
		 * 0x02 -- failure
		 * 0x03 -- complete but need people to check result
		 */
		sca->result = result;   
								
		/* post this event to AO_Cli ,showing completion */
		QACTIVE_POST(AO_TEST, (QEvent *)sca, 0);
	}
}

