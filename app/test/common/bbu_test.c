#include "bbu.h"
#include <string.h>
#include <stdlib.h>
#include "timer_dec.h"
#include "gpio_dec.h"
#ifdef CONFIG_SCREEN_CHIP 
#include "screen_test.h"
#endif
#ifdef CONFIG_ATE
#include "ate_test.h"
#endif
#include "bbu_test.h"


void bbu_test_start(void)
{
	return;
}


/*
 * BBU Test result return function
 * This function will be renamed to bbu_test_done in the future
*/
void screen_cmd_ack(uint8_t result, uint8_t cmd_index)
{
	if (result == BBU_TEST_FAIL || result == BBU_TEST_TIMEOUT) {
#ifdef Q_SPY
		bbu_printf("\r\n%d\r\nFAIL\r\n");
#else
		bbu_printf("\r\nFAIL\r\n");
#endif
	} else if (result == BBU_TEST_PASS || result == BBU_TEST_CHECK) {
#ifdef Q_SPY
		bbu_printf("\r\n%d\r\nPASS\r\n");
#else
		bbu_printf("\r\nPASS\r\n");
#endif
	} else {
#ifdef Q_SPY
		bbu_printf("\r\n%d\r\nFAIL! Unknown ACK!\r\n");
#else
		bbu_printf("\r\nFAIL! Unknown ACK!\r\n");
#endif
	}
	bbu_printf(BBU_PROMPT);


#ifdef CONFIG_SCREEN_CHIP  
	if (cmd_index & TEST_ACK_FROM_SCREEN) {
		screen_test_cmd_ack(result, (cmd_index & ~TEST_ACK_FROM_SCREEN));
	} else {
#ifdef CONFIG_ATE
		ate_test_ack(result, cmd_index);
#endif
	}

#endif

	return;
}

