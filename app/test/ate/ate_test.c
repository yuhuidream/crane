#include "bbu.h"
#include <string.h>
#include <stdlib.h>
#include "tri_led.h"
#include "timer_dec.h"
#include "gpio_dec.h"
#include "ate_comp.h"
#include "bsp.h"
#include "ate_test.h"
#include "ate.h"
#include "bbu_test.h"
#include "qassert.h"
#include "list.h"

Q_DEFINE_THIS_MODULE("ate_test.c")

typedef struct
{
	char cmd[MAX_LINE_LEN];

	/* GPIOs to output test result and status */
	int32_t stat_gpio;
	int32_t res_gpio;

	/*
	 * Command does not receive response before timeout_deadline arrived
	 * will be assumed failed
	 */
	uint32_t timeout_deadline;  /* in seconds */

	/*
	 * select which group this command will locate in.
	 * if equal to zero, default group_id will be command index
	 */
	uint32_t group_id;
} ate_cmd_entry;

typedef struct
{
	uint8_t stat;
	uint8_t res;
} ate_record_entry;

/* struct ate_entry used to manage all commands in one group */
struct ate_entry
{
	struct list_head entry_list;
	
	/*
	 * cmd_index is an ID used to discrimiate which command 
	 * result ACK belong to
	 */
	uint32_t cmd_index;
	
	/* command name */
	char cmd[MAX_LINE_LEN];
	
	/* GPIOs to output test result and status */
	int32_t stat_gpio;
	int32_t res_gpio;
	
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

/* struct ate_sub_group used to manage all groups */
struct ate_sub_group
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
#define ATE_GR_INIT 0
#define ATE_GR_DONE 1
	
	/* total command number of this group */
	uint32_t cmd_num;

	/* Execute result of the whole group */
	uint8_t result;
	
	/* head point of commands list */
	struct list_head cmd_head;
};

#ifdef CONFIG_ATE_GLOBAL_STAT_RES	
static void ate_update_global_test_status(void);
#endif

int ate_timer;

/* list head of all group */
static struct list_head sub_group_head;
static struct ate_sub_group *cur_gp;

#define ATE_CMD_CELL(_name, _timeout, _group_id, _stat_gpio, _res_gpio)      \
{                                                                            \
	.cmd = _name,                                                        \
	.timeout_deadline = _timeout,                                        \
	.group_id = _group_id,                                               \
	.stat_gpio = _stat_gpio,                                             \
	.res_gpio = _res_gpio,                                               \
}
#define ATE_CMD_CELL_NGPIO(name, timeout, group_id)	                     \
	ATE_CMD_CELL(name, timeout, group_id, GPIO_INVALID, GPIO_INVALID)
#define ATE_CMD_END ATE_CMD_CELL_NGPIO("\0", 0, 0)

static const ate_cmd_entry local_ate_table[] = 
{
	/* 
	 * Put ATE test case below 
	*/
//	ATE_CMD_CELL_NGPIO("dte", 1, 0),
	ATE_CMD_CELL_NGPIO("pid", 10, 0),
	ATE_CMD_CELL_NGPIO("ppset 0", 10, 0),
	ATE_CMD_CELL_NGPIO("commt 0xf 2 3 25", 10, 0),
	ATE_CMD_CELL_NGPIO("ppset 6", 10, 0),
	ATE_CMD_CELL_NGPIO("commt 1 1 1", 10, 0),
	ATE_CMD_CELL_NGPIO("ppset 1", 10, 0),
	ATE_CMD_CELL_NGPIO("commt 1 1 2", 10, 0),
	ATE_CMD_CELL_NGPIO("fuseread", 10, 0),
	ATE_CMD_CELL_NGPIO("mtsp 0x30000000 0x40000 0xffffffff", 10, 0),
	ATE_CMD_CELL_NGPIO("ppset 8", 10, 0),
	ATE_CMD_CELL_NGPIO("ppset 23", 10, 0),
	ATE_CMD_CELL_NGPIO("mtsp 0 0x40000 0xfff1fffa", 10, 0),
	ATE_CMD_CELL_NGPIO("mtsp 0x30000000 0x40000 0xfff1fffa", 10, 0),
	ATE_CMD_CELL_NGPIO("dma mdma 0", 10, 0),
#ifdef CONFIG_VPU    
	ATE_CMD_CELL_NGPIO("vpu dec 0 1", 10, 0),  // decode AVC
	ATE_CMD_CELL_NGPIO("vpu enc 0 1", 10, 0),  // encode MPEG4
#endif
#ifdef CONFIG_GCU
	ATE_CMD_CELL_NGPIO("gcu3d mem 0", 10, 0),  // 3D texture7 test
	ATE_CMD_CELL_NGPIO("gcu2d mem 0", 10, 0),  // 2D bitcopy test
#endif
#ifdef CONFIG_H264
	ATE_CMD_CELL_NGPIO("h264 3", 10, 0),  //used to test NEON
#endif
#ifdef CONFIG_VFP
	ATE_CMD_CELL_NGPIO("vfpt float32_add", 10, 0),
#endif
	ATE_CMD_CELL_NGPIO("mlucas -iters 2", 10, 0),
	ATE_CMD_CELL_NGPIO("geu geudiag 3 1", 10, 0),
	ATE_CMD_CELL_NGPIO("lpmset 3 1", 1, 0),
	ATE_CMD_CELL_NGPIO("lpmset 9 7 5 1 1 6", 5, 0),  /* 1s is not enough on some socket board at Korea AE, use 3s instead */
	ATE_CMD_CELL_NGPIO("mcps", 10, 0),	
	ATE_CMD_CELL_NGPIO("icam 0 0 2", 5, 0),

	/* Test case End */
	ATE_CMD_END,
};

void ate_init(void)
{
	INIT_LIST_HEAD(&sub_group_head);
}

static void ate_reset_group_cmd_status(struct ate_sub_group *sub_gp)
{
	struct ate_entry *cmd_entry;

	list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
		cmd_entry->result = ATE_TEST_BEGIN;
		cmd_entry->time_used_ms = 0xffffffff;
#ifdef CONFIG_ATE_EACH_STAT_RES
		/* At start of test, status high, result low */
		gpio_output_high(cmd_entry->stat_gpio);
		gpio_output_low(cmd_entry->res_gpio);
#endif
	}

#ifdef CONFIG_ATE_CURRENT_STAT_RES
	/* Current status/result ouput */
	gpio_output_high(ATE_CURRENT_STATUS_PORT);
	gpio_output_low(ATE_CURRENT_RESULT_PORT);
#endif
	sub_gp->result = ATE_TEST_BEGIN;

	return;
}

static void ate_reset_all_cmd_status(void)
{

	struct ate_sub_group *sub_gp;
	
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		ate_reset_group_cmd_status(sub_gp);
	}
			
#ifdef CONFIG_ATE_GLOBAL_STAT_RES
	gpio_output_high(ATE_GLOBAL_STATUS_PORT);
	gpio_output_low(ATE_GLOBAL_RESULT_PORT);	
#endif

	return;
}

void ate_cmd_init(void)
{
#ifdef CONFIG_ATE_EACH_STAT_RES
	struct ate_sub_group *sub_gp;
	struct ate_entry *cmd_entry;

	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
			gpio_set_output(cmd_entry->stat_gpio);
			gpio_set_output(cmd_entry->res_gpio);

		}
	}
#endif
#ifdef CONFIG_ATE_CURRENT_STAT_RES
	gpio_set_output(ATE_CURRENT_STATUS_PORT);
	gpio_set_output(ATE_CURRENT_RESULT_PORT);
#endif
	/*
	 * For debug use, generate ATE start signal from keypad
	*/
//	gpio_set_output(GPIO1);
//	gpio_output_high(GPIO1);

#ifdef CONFIG_ATE_GLOBAL_STAT_RES
	gpio_set_output(ATE_GLOBAL_STATUS_PORT);
	gpio_set_output(ATE_GLOBAL_RESULT_PORT);	
#endif
	ate_reset_all_cmd_status();	
	ate_case_select_gpio_init();

	return;
}
void ate_free_group_list(void)
{
	struct ate_sub_group *sub_gp;
	struct ate_entry *cmd_entry;
	
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

uint32_t ate_add_cmd_to_group(uint32_t seq, uint32_t index, char *cmd,
                              uint32_t timeout)
{
	struct ate_sub_group *sub_gp;
	struct ate_entry *cmd_entry;
	
	/* NULL command will not add to group */
	if (!cmd || cmd[0] == '\0')
		return 1;
	
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		if (sub_gp->group_id == seq) {
			cmd_entry = bbu_malloc(sizeof(struct ate_entry));
			strcpy(cmd_entry->cmd, cmd);
			cmd_entry->time_used_ms = 0xffffffff;
			cmd_entry->result = ATE_TEST_BEGIN;
			cmd_entry->cmd_index = index;

			/*
			 * Todo:
			 * Does not support result/status gpio for each command
			 */
			cmd_entry->stat_gpio = GPIO_INVALID;
			cmd_entry->res_gpio = GPIO_INVALID;

			cmd_entry->timeout_deadline = timeout;
			list_add_tail(&cmd_entry->entry_list, &sub_gp->cmd_head);
			sub_gp->cmd_num++;
			if (cmd_entry->timeout_deadline > sub_gp->group_max_timeout)
				sub_gp->group_max_timeout = cmd_entry->timeout_deadline;
			
			break;
		}
	}
	
	if (&sub_gp->sub_gp_list == &sub_group_head) {
		struct ate_sub_group *sub_gp_new, *next;
		sub_gp_new = bbu_malloc(sizeof(struct ate_sub_group));
		sub_gp_new->group_id = seq;
		sub_gp_new->cmd_num = 1;
		sub_gp_new->group_stat = ATE_GR_INIT;
		sub_gp_new->group_max_timeout = timeout;
		INIT_LIST_HEAD(&sub_gp_new->cmd_head);
		
		list_for_each_entry_safe(sub_gp, next, &sub_group_head,
		                         sub_gp_list) {
			if (sub_gp_new->group_id < sub_gp->group_id) {
				break;
			}
		}
		list_add(&sub_gp_new->sub_gp_list, sub_gp->sub_gp_list.prev);
		
		cmd_entry = bbu_malloc(sizeof(struct ate_entry));
		strcpy(cmd_entry->cmd, cmd);
		cmd_entry->time_used_ms = 0xffffffff;
		cmd_entry->result = ATE_TEST_BEGIN;
		cmd_entry->cmd_index = index;

		/*
		 * Todo:
		 * Does not support result/status gpio for each command
		 */
		cmd_entry->stat_gpio = GPIO_INVALID;
		cmd_entry->res_gpio = GPIO_INVALID;

		cmd_entry->timeout_deadline = timeout;
		list_add_tail(&cmd_entry->entry_list, &sub_gp_new->cmd_head);
	}
	return 0;
}

static uint32_t ate_load_cmd_from_table(void *table_addr)
{
	ate_cmd_entry *entry = table_addr;
	uint32_t i;
	uint32_t gp_id;
	
	ate_free_group_list();
	for (i = 0; strlen(entry->cmd) != 0; i++) {
		if (!entry->group_id)
			gp_id = i+1;
		else
			gp_id = entry->group_id;
		
		ate_add_cmd_to_group(gp_id, i+1, entry->cmd,
		                     entry->timeout_deadline);
		entry++;
	}
	
	ate_cmd_init();
	return 0;
}

uint32_t ate_load_cmd_from_local(void)
{
	return ate_load_cmd_from_table((void *)local_ate_table);
}

static void ate_post_all_cmd_in_group(struct ate_sub_group *sub_gp)
{
	struct ate_entry *cmd_entry;
	
	/* Save current group */
	cur_gp = sub_gp;
	sub_gp->group_stat = ATE_GR_INIT;

	list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
		ScreenCmdEvt *sce = Q_NEW(ScreenCmdEvt,COM_RX_CMD_READY_SIG);
		
		sce->len = (uint8_t)strlen(cmd_entry->cmd);
		sce->cmd_index = cmd_entry->cmd_index;
		sce->source = 1;
		strcpy((char *)sce->cmd, cmd_entry->cmd);
		
		bbu_printf("\r\n>>>>>>>>> Start ATE test \"%s\"\r\n",
		           cmd_entry->cmd);
		cmd_entry->time_used_ms = bbu_get_msec();
		QACTIVE_POST(AO_Cli, (QEvent *)sce, AO_TEST);
	}
	
	/* First screen command is sleep test, just need 1s timeout */  
	QTimeEvt_postIn(&(l_ate_comp.TestTimeEvt), AO_TEST,
	                BSP_TICKS_PER_SEC * sub_gp->group_max_timeout);
}

static inline uint8_t ate_check_if_group_finished(struct ate_sub_group *sub_gp)
{
	struct ate_entry *cmd_entry;
	
	list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
		if (cmd_entry->result == ATE_TEST_BEGIN)
			return 0;
	}
	return 1;
}

static struct ate_sub_group *ate_update_cmd_result(uint32_t cmd_index, uint8_t result)
{
	struct ate_sub_group *sub_gp= cur_gp;
	struct ate_entry *cmd_entry;
	
	list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
		if (cmd_entry->cmd_index == cmd_index) {
			/*
			 * if command already executed, just ignore
			 * new result
			 */
			if (cmd_entry->result != ATE_TEST_BEGIN)
				return NULL;
			cmd_entry->result = result;
			if (result == ATE_TEST_PASS) {
				cmd_entry->time_used_ms = 
					bbu_get_msec() - cmd_entry->time_used_ms;
			} else {
				cmd_entry->time_used_ms = 0xffffffff;
			}
#ifdef CONFIG_ATE_EACH_STAT_RES
			switch(result) {
			case ATE_TEST_PASS:
				/* At end of test, If successful, Status low, result high */
				gpio_output_low(cmd_entry->stat_gpio);
				gpio_output_high(cmd_entry->res_gpio);
				break;

			case ATE_TEST_FAIL:
				/* At end of test, If failed, Status low, result low */
				gpio_output_low(cmd_entry->stat_gpio);
				gpio_output_low(cmd_entry->res_gpio);
				break;
			default:
				bbu_printf("Error: not expected result %d\r\n", result);
				break;
			}
#endif	
			if (result == ATE_TEST_PASS &&
			    sub_gp->result != ATE_TEST_FAIL) {
				sub_gp->result = ATE_TEST_PASS;
			} else {
				sub_gp->result = ATE_TEST_FAIL;
			}

			return sub_gp;
		}
	}
	return NULL;
}

static void ate_invert_live_status_pin(void *data)
{
	(void)data;
	uint8_t level = gpio_get_pin_level(ATE_LIVE_SATUS_PORT);
	
	if (level)
		gpio_output_low(ATE_LIVE_SATUS_PORT);
	else
		gpio_output_high(ATE_LIVE_SATUS_PORT);
//	bbu_printf("live pin level: %d\r\n", 
//	           gpio_get_pin_level(ATE_LIVE_SATUS_PORT));
	return;
}

void ate_start_show_live_status()
{
	/*
	 * Output a square wave to show software alive per PE's request
	*/
	gpio_set_output(ATE_LIVE_SATUS_PORT);
	ate_timer = start_timer(5, ate_invert_live_status_pin, 0);
	if(ate_timer < 0) {
	    bbu_printf("ERROR: No free timer for ate...\r\n");
	}
	
	return;
}

#ifdef CONFIG_ATE_GLOBAL_STAT_RES	
static void ate_update_global_test_status(void)
{
	struct ate_sub_group *sub_gp;
	struct ate_entry *cmd_entry;
	uint32_t fail_num = 0;

	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
			if (cmd_entry->result == ATE_TEST_BEGIN)
				return;
			if (cmd_entry->result == ATE_TEST_FAIL)
				fail_num++;
		}
	}

	if (fail_num) {
		gpio_output_low(ATE_GLOBAL_STATUS_PORT);
		gpio_output_low(ATE_GLOBAL_RESULT_PORT);			
	} else {
		gpio_output_low(ATE_GLOBAL_STATUS_PORT);
		gpio_output_high(ATE_GLOBAL_RESULT_PORT);		
	}

	return;
}
#else
#define ate_update_global_test_status() do{ }while(0)
#endif

static void update_ate_group_gpio_status(uint32_t result)
{
#ifdef CONFIG_ATE_CURRENT_STAT_RES
	switch(result) {
	case ATE_TEST_PASS:
		/* Current status/result ouput */
		gpio_output_low(ATE_CURRENT_STATUS_PORT);
		gpio_output_high(ATE_CURRENT_RESULT_PORT);
		break;
	
	case ATE_TEST_FAIL:
		/* Current status/result ouput */
		gpio_output_low(ATE_CURRENT_STATUS_PORT);
		gpio_output_low(ATE_CURRENT_RESULT_PORT);
		break;
	default:
		bbu_printf("Error: not expected result %d\r\n", result);
		break;
	}
#endif
	if (l_ate_comp.all_in_one) {
		mdelay(10);	
	}

	return;
}

uint8_t ate_start_test(uint8_t group_id)
{
	struct ate_sub_group *sub_gp;

	if (group_id == ATE_MAX_CASE_INDEX) {
		ate_reset_all_cmd_status();
		group_id = 1;
		l_ate_comp.all_in_one = 1;
	}

	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		if(sub_gp->group_id == group_id) {
			ate_reset_group_cmd_status(sub_gp);
			ate_post_all_cmd_in_group(sub_gp);
			return ATE_TRUE;
		}
	}

	return ATE_FALSE;
}

/**
 * collect_screen_result
 *
 * Handle ACK from AOs, then record the events in order to 
 * output screen test reports after the last command executed.
 */
uint8_t collect_ate_result(const ATEAckEvt *e)
{
	struct ate_sub_group *sub_gp, *next;
	uint8_t cmd_index;

	cmd_index = e->cmd_index;
	if (!cmd_index)
		return 0;
	
	sub_gp = ate_update_cmd_result(cmd_index, e->result);
	/*
	 * If this group already finished, just update the result and exit
	 * Note: lack of this may lead to repeated test
	 */
	if (!sub_gp || sub_gp->group_stat == ATE_GR_DONE)
		return 0;
	
	if (!ate_check_if_group_finished(sub_gp)) {
		QTimeEvt_rearm(&(l_ate_comp.TestTimeEvt),
	                       BSP_TICKS_PER_SEC * sub_gp->group_max_timeout);
		return 0;
	}
	/* disarm ate test time event when over */
	QTimeEvt_disarm(&(l_ate_comp.TestTimeEvt));
	sub_gp->group_stat = ATE_GR_DONE;
	update_ate_group_gpio_status(sub_gp->result);
	
	if (l_ate_comp.all_in_one == 1) {
		if (list_is_last(&sub_gp->sub_gp_list, &sub_group_head)) {
			//ate_test_report();
			ate_update_global_test_status();
			l_ate_comp.all_in_one = 0;
			bbu_printf("************************[ATE] all in one test end!************************\r\n");
		} else {
			next = list_entry(sub_gp->sub_gp_list.next,
			                  typeof(*sub_gp), sub_gp_list);
			mdelay(5);  /* delay 5ms between test group */
			ate_post_all_cmd_in_group(next);
			return ATE_FALSE;		
		}
	} else {
		l_ate_comp.all_in_one = 0;
	}

	return ATE_TRUE;
}

/**
 * ate_timeout_handle
 *
 * Description:
 * If not receiving ACK before timeout happen, post a timeout event to myself,
 * an exception is command "lpmset 3 1", which do not have ack itself, will
 * make a fake pass event to myself 
 */
void ate_timeout_handle(void)
{
	struct ate_sub_group *sub_gp = cur_gp;
	struct ate_entry *cmd_entry;	

	list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
		if (cmd_entry->result == ATE_TEST_BEGIN) {
			if (strcmp(cmd_entry->cmd, "lpmset 3 1") == 0) {
				/*
				 * For "lpmset 3 1", after 1s timeout, 
				 * let screen test go on with wakeup test.
				 */
				screen_cmd_ack(BBU_TEST_PASS,
				               cmd_entry->cmd_index);
			} else {
				screen_cmd_ack(BBU_TEST_TIMEOUT,
				               cmd_entry->cmd_index);				
			}
		}
	}
}

void ate_test_ack(uint8_t result, uint8_t cmd_index)
{
	ATEAckEvt *evt;

	DEBUG_ATE_PRINT("cmd index %d from case owner!\r\n", cmd_index);
	
	if (cmd_index == 0)
		return;

	if (result == BBU_TEST_PASS || result == BBU_TEST_CHECK) {
		result = ATE_TEST_PASS;
	} else {
		result = ATE_TEST_FAIL;
	}
	
	evt = Q_NEW(ATEAckEvt, ATE_TEST_RESULI_SIG);
	evt->cmd_index = cmd_index;	
	evt->result = result;		
	
	QACTIVE_POST(AO_TEST, (QEvent *)evt, 0);
	return;
}

void ate_test_request(uint8_t group_id)
{
	ATEReqEvt *evt = Q_NEW(ATEReqEvt, ATE_TEST_REQ_SIG);
	
	evt->group_id = group_id;
	QACTIVE_POST(AO_TEST, (QEvent *)evt, 0);
	
	return;
}

uint8_t is_cmd_in_ate_table(ComCmdEvt *cmde)
{
	struct ate_sub_group *sub_gp;
	struct ate_entry *cmd_entry;
	uint8_t cmd[MAX_LINE_LEN];

	/* argv[i] is the pointer to args in cmdline, e.g. argv[0]
	 * point to the cmd name */
	uint8_t *argv[CMDLINE_MAX_ARGS];
	uint8_t *pcChar;
	uint32_t argc;
	uint16_t i, bFindArg;
	
	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {
			strcpy((char *)cmd, cmd_entry->cmd);
			pcChar = cmd;
			argc = 0;
			bFindArg = 1;
		
			while (*pcChar) {
				if (*pcChar == ' ' || *pcChar == '\t') {
					/* 
					 * if it's space, then replace it with zero and 
					 * set the flag to search for the next arg 
					 */
					*pcChar = '\0';
					bFindArg = 1;
				} else { 
					/* 
					 * it is not space, so it must be a char in an argument 
					 */
					if(bFindArg) {
						if(argc < CMDLINE_MAX_ARGS) {
							argv[argc] = pcChar;
							argc++;
							bFindArg = 0;
						} else {
							bbu_printf("Too many arguments entered..., argc: %d\r\n", 
							           argc);
						}
					}
				}
				pcChar++;
			}

			if (cmde->argc == argc) {
				for (i = 0; i < argc; i++) {
					if (strcmp((const char*)cmde->argv[i], (const char*)argv[i])) {
						break;
					}
				}
				if (i == argc) {
					DEBUG_ATE_PRINT("Command in ate table, group_id=%d\r\n", sub_gp->group_id);
					return (sub_gp->group_id);
				}
			}
		}
	}
	DEBUG_ATE_PRINT("Command not in ate table!\r\n");
	return 0;
}

/*
 * Output a list of commands joining in auto screen test based screen table.
*/
void ate_test_cmd_display(void)
{
	struct ate_sub_group *sub_gp;
	struct ate_entry *cmd_entry;

	bbu_printf("\r\n ATE Test commands are below:\r\n");
	bbu_printf("\r\n%10s%10s%40s%20s%20s%20s\r\n", "Group ID","Index",
	            "ATE Test", "Result", "Ind Time(ms)", "Timeout(seconds)");

	list_for_each_entry(sub_gp, &sub_group_head, sub_gp_list) {
		list_for_each_entry(cmd_entry, &sub_gp->cmd_head, entry_list) {	
			uint8_t result[5];
		
			if (cmd_entry->result == ATE_TEST_PASS) {
				strcpy((char *)result, "Pass");
			} else if(cmd_entry->result == ATE_TEST_FAIL)  {
				strcpy((char *)result, "Fail");
			} else {
				strcpy((char *)result, "NA");
			}
		
			if (cmd_entry->time_used_ms == 0xffffffff) {
				bbu_printf("%10d%10d%40s%20s%20s%20d\r\n",
				    sub_gp->group_id, cmd_entry->cmd_index,
				    cmd_entry->cmd, result, "NA",
				    cmd_entry->timeout_deadline);
			} else {
				bbu_printf("%10d%10d%40s%20s%20d%20d\r\n",
				    sub_gp->group_id, cmd_entry->cmd_index,
				    cmd_entry->cmd, result, cmd_entry->time_used_ms,
				    cmd_entry->timeout_deadline);
			}
		}
	}
	bbu_printf(BBU_PROMPT);
}

