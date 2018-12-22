
#ifndef ATE_TEST_H
#define ATE_TEST_H

/* For ATE test, we use GPIO to show test status */
#define ATE_TEST_BEGIN (1<<0)
#define ATE_TEST_PASS (1<<1)
#define ATE_TEST_FAIL (1<<2)
#define ATE_TEST_END (1<<3)

#define ATE_TRUE    TEST_TRUE
#define ATE_FALSE   TEST_FALSE

/* 
 * ATE test IO port select: 
 *  need to modify driver/common/mfp-emei.h when modifying these IOs
 */
/* Output */
#define ATE_LIVE_SATUS_PORT        GPIO13
#define ATE_CURRENT_STATUS_PORT    GPIO7
#define ATE_CURRENT_RESULT_PORT    GPIO5
#define ATE_GLOBAL_STATUS_PORT     GPIO9    //GPIO_INVALID
#define ATE_GLOBAL_RESULT_PORT     GPIO11    //GPIO_INVALID

/* Input */
#define ATE_SEL_PORT1    GPIO31
#define ATE_SEL_PORT2    GPIO33
#define ATE_SEL_PORT3    GPIO34
#define ATE_SEL_PORT4    GPIO35
#define ATE_SEL_PORT5    GPIO36
#define ATE_START_PORT   GPIO8

enum
{
	ATE_SEL1 = 0,
	ATE_SEL2, 
	ATE_SEL3, 
	ATE_SEL4, 
	ATE_SEL5, 
	ATE_START,
	ATE_CASE_SEL_MAX, 
};

#define ATE_MAX_CASE_INDEX ((1 << (ATE_CASE_SEL_MAX-1)) - 1) 
#define ATE_MAX_CMD_NUM    (ATE_MAX_CASE_INDEX - 1)


//#define ATE_DEBUG_ON

#if defined(ATE_DEBUG_ON)
#define DEBUG_ATE_PRINT(fmt, args...) bbu_printf(fmt, ##args)
#else
#define DEBUG_ATE_PRINT(fmt, args...)
#endif

void ate_test_ack(uint8_t result, uint8_t cmd_index);
void ate_timeout_handle(void);
uint8_t collect_ate_result(const ATEAckEvt *e);
uint8_t ate_start_test(uint8_t group_id);
void ate_clear_status_result(uint8_t cmd_index);
void ate_cmd_init(void);
uint8_t is_cmd_in_ate_table(ComCmdEvt *cmde);
void ate_test_request(uint8_t group_id);
void ate_test_cmd_display(void);
uint32_t ate_load_cmd_from_local(void);
void ate_free_group_list(void);
#ifdef CONFIG_ATE_PE_FS
uint32_t ate_add_cmd_to_group(uint32_t seq, uint32_t index, char *cmd, uint32_t timeout);
#endif
void ate_init(void);
void ate_start_show_live_status();

#endif
