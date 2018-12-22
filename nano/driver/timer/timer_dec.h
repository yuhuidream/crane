#ifndef __TIMER_DEC_H__
#define __TIMER_DEC_H__
#include "qp_port.h"
#include "qassert.h"
#include <stdlib.h>
#include <string.h>

#include "predefines.h"
#include "TIME_REG.h"
#include "bbu.h"
#include "clock.h"
#include "interrupt.h"

#define TIMER0_TICK_PER_MSEC        3250
#define TIMER0_TICK_PER_SEC         3250000

#define TIME_BASE                   TIME_TypeDef

#ifdef CONFIG_CA7
#define SYSTIME                     TIME1
#define SYSTIME_IRQ                 IRQ_MMP_AP1_TIMER1
#else
#define SYSTIME                     TIME2
#define SYSTIME_IRQ                 IRQ_MMP_AP2_TIMER1
#endif

#define TIMER0_MAX_CNT              0xFFFFFFFFUL

#define TIMER_RES_CNT				1

typedef enum{
	TIMER_UNIT0 = 0,
	TIMER_UNIT1,
	TIMER_UNIT2
}TIMER_UNIT;

typedef enum{
	TIMER_CLK_13M = 0,//configureable clock
	TIMER_CLK_32K
}TIMER_CLK;

typedef enum{
	TIMER_MODE_PERIODIC = 0,
	TIMER_MODE_FREE_RUN
}TIMER_COUNT_MODE;

typedef enum{
	TIMER_PERIODIC = 0,
	TIMER_ONE_SHOT
}TIMER_RUN_MODE;

typedef enum{
	TIMER_RUNNING = 0,
	TIMER_STOP
}TIMER_RUN_STATUS;


// Delay functions.
void udelay(int us);
void mdelay(int ms);
void delay(int s);

//system timer functions.
uint32_t timer_get_count(int id);
uint32_t TIMER_GET_INTERVAL(uint32_t end, uint32_t begin);

#ifdef CONFIG_GENERIC_TIME
void generic_timer_config(void);
unsigned long long generic_timer_read(void);
void generic_timer_test(void);
#endif

//watch dog timer function.
void wdt_test(unsigned int seconds, unsigned int reset_int);
void do_wdt_reset(void);
void pmic_watchdog_enable(void);
void pmic_watchdog_timer_reset(void);
#ifdef CONFIG_MMT
void wdt_reset(void);
void wdt_start(int sec);
#endif

int count_2_msec(uint32_t count);
int interval_2_msec(uint32_t begin, uint32_t end);
int count_2_sec(uint32_t count);
int interval_2_sec(uint32_t begin, uint32_t end);

#ifdef CONFIG_CA7
int bbu_delay_test(int argc, uint8_t **argv);
#endif

#endif

