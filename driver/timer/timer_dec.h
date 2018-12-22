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

#define TIMER_UNIT_NUM   (6)
#define UNIT_PER_TIMER		(3)

#define TIMER0_TICK_PER_MSEC        3250
#define TIMER0_TICK_PER_SEC         3250000

#define TIME_BASE                   TIME_TypeDef

//CP only can use AIRQ irq.
#define SYSTIME_IRQ                 IRQ_MMP_AP1_TIMER1

#define SYSTIME                     TIME1

#define TIMER_RES_CNT               2

#define TIMER0_MAX_CNT              0xFFFFFFFFUL

typedef void (*irq_func)(void*);

typedef struct IrqArgTag{
	int timer_index;
	int timer_unit;
	int mode;
	irq_func func;//timeout function.
	void* func_arg_p;
}IrqArg;

typedef enum{
	TIMER_GROUP_NORMAL = 0,
	TIMER_GROUP_SYSTEM,
	TIMER_GROUP_WAKEUP,
	TIMER_GROUP_WAKEUP_CP,
	TIMER_GROUP_AP
}TIMER_GROUP;

typedef enum {
        TIMER_FREE = 0,
        TIMER_BUSY
}TIMER_STAT;

typedef struct TimerAttrTag{
	uint32_t idx;
	uint32_t unit;
	TIMER_GROUP group;
	int stat;
	int irq_connected;
}TimerAttr;

typedef enum{
	TIMER_UNIT0 = 0,
	TIMER_UNIT1,
	TIMER_UNIT2
}TIMER_UNIT;

typedef enum{
	TIMER_CLK_13M = 0,//configureable clock
	TIMER_CLK_32K,
	TIMER_CLK_6P5M,
	TIMER_CLK_3P25M,
	TIMER_CLK_1M
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

//rtc functions.
void rtc_clk_init();
void rtc_alarm(uint32_t secs);
void rtc_disable(void);
uint32_t pmic_rtc_read(void);
void rtc_test(int argc, uint8_t** argv);

void generic_timer_config(void);
uint64_t generic_timer_read(void);
#ifdef CONFIG_GENERIC_TIME
void generic_timer_test(void);
void generic_count_c2();
void generic_count_loop(void);
void generic_count_test(int argc, uint8_t** argv);
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

int start_timer(int ms, irq_func irq_handler, uint32_t flag);
void stop_timer(int index);
void free_timer(int index);

//run_mode 0--free run 1--one shot.
int start_timer_wakeup(int ms, irq_func irq_handler);
void timer_test_start(uint32_t index, uint32_t unit, uint32_t run_mode, uint32_t interval);
void timer_test_stop(uint32_t index, uint32_t unit);
void time_start_all(uint32_t run_mode, uint32_t interval);
void timer_stop_all();
void timer_test(int argc, uint8_t **argv);

#endif

