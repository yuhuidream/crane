#ifdef CONFIG_ATE

#include "bbu.h"
#include <string.h>
#include <stdlib.h>
#include "tri_led.h"
#include "timer_dec.h"
#include "gpio_dec.h"
#include "bsp.h"
#include "ate.h"
#include "ate_test.h"
#include "GPIO.h"


uint8_t ate_case_sel[] = 
{
	[ATE_SEL1]   = ATE_SEL_PORT1,
	[ATE_SEL2]   = ATE_SEL_PORT2,
	[ATE_SEL3]   = ATE_SEL_PORT3,
	[ATE_SEL4]   = ATE_SEL_PORT4,
	[ATE_SEL5]   = ATE_SEL_PORT5,
	[ATE_START]  = ATE_START_PORT,
};

/* Save the start signal input, used to filter glitch */
static uint8_t last_start_level;

void ate_test_isr(void *data)
{
	(void)data;
	uint8_t i;
	uint8_t sel;

	udelay(5);  /* wait a moment for gpio to be stable */	
	
	if (last_start_level == gpio_get_pin_level(ate_case_sel[ATE_START])) {
		return;
	} else {
		last_start_level = gpio_get_pin_level(ate_case_sel[ATE_START]);
	}
	
	/* decode case select */
	sel = 0;
	for (i = 0; i < ATE_START; i++) {
		sel = sel << 1;
		sel |= gpio_get_pin_level(ate_case_sel[i]);
	}
		
	DEBUG_ATE_PRINT("[ATE] Start interrupt! Sel: 0x%x\r\n", sel);
#if 0
	if (last_start_level == 0) {
		DEBUG_ATE_PRINT("ATE request clear status/result!\r\n");
		if (sel != 0) {
			ate_clear_status_result(sel);			
		}
	} else if (last_start_level == 1) {
		DEBUG_ATE_PRINT("ATE request to run test %d!\r\n", sel);
		if (sel != 0) {
		 	/* start request to run select case */
			ate_test_request(sel);
		}
	}
#else	
	if (last_start_level == 1) {
		DEBUG_ATE_PRINT("ATE request to run test %d!\r\n", sel);
		if (sel != 0) {
		 	/* start request to run select case */
			ate_test_request(sel);
		}
	}
	
#endif
	return;
}



/**
 * For ATE test, we use GPIO to show test beginning and the result
*/

void ate_case_select_gpio_init(void)
{
	uint8_t i;
	
	for (i = 0; i <= ATE_START; i++) {
		gpio_set_input(ate_case_sel[i]);
	}
 	mdelay(1);

   last_start_level = gpio_get_pin_level(ate_case_sel[ATE_START]);
   DEBUG_ATE_PRINT("ATE START INPUT Level: %d\r\n", last_start_level);
   
   gpio_isr_connect(ate_case_sel[ATE_START], ate_test_isr);
	gpio_fall_enable(ate_case_sel[ATE_START]);
	gpio_rise_enable(ate_case_sel[ATE_START]);
	
	return;
}


void ate_save_enter_lpm(void)
{
	last_start_level = gpio_get_pin_level(ate_case_sel[ATE_START]);
	gpio_fall_disable(ate_case_sel[ATE_START]);
	gpio_rise_disable(ate_case_sel[ATE_START]);	
	
	DEBUG_ATE_PRINT("Disable START interrupt before enter lpm!\r\n");

	return;
}


void ate_resume_exit_lpm(void)
{
 	ate_case_select_gpio_init();
	
	DEBUG_ATE_PRINT("Enable START interrupt after exit lpm!\r\n");
	return;
}



#endif

