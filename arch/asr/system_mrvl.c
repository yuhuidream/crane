/**************************************************************************//**
 * @file     system_mrvl.c
 * @brief    CMSIS-like System Source File for Nevo SOC
 * @note
 * Copyright (C) 2011 Marvell. All rights reserved.
 *
 *******************************************************************************/
#include "predefines.h"
#include "bbuinfo.h"
#include "PMUA.h"
#include "PMUM.h"
#include "APBCLK.h"
#include "uart_dec.h"
#include "interrupt.h"
#include "mfp-emei.h"
#include "gpio_dec.h"
#include "timer_dec.h"
#include "system_mrvl.h"
#include "CIU.h"
#include "PMR.h"
#include "cache_ops.h"
#include "set_mmu.h"
#include "pmic.h"
#include "i2c_dec.h"
#ifdef CONFIG_SMP
#include "scu.h"
#endif


extern int GetCPUID(void);
extern void sys_timer_init(void);

static void enable_clocks(void)
{
    PMUM->ACGR = 0xFFFFFFFF;

    APBClock->GPIO_CLK_RST = 0x7;
    APBClock->GPIO_CLK_RST = 0x3;
    APBClock->AIB_CLK_RST = 0x7;
    APBClock->AIB_CLK_RST = 0x3;
}

#ifdef CONFIG_ETM_PTM
/*
 Enable ETM trace, There is overhead, so just for debug!
 ETM will record the instruction flow AS cpu goes all the way.
 There is also JTAG low level script to read register method.
 and the chip watcher method for further debug.
*/
static void enable_coresight_access(void)
{ //1
    BU_REG_WRITE(0xd411cfb0, 0xC5ACCE55);
    BU_REG_WRITE(0xd411dfb0, 0xC5ACCE55);
    BU_REG_WRITE(0xd4109fb0, 0xC5ACCE55);
    BU_REG_WRITE(0xd4108fb0, 0xC5ACCE55);
    BU_REG_WRITE(0xd4105fb0, 0xC5ACCE55);
}

static void enable_etm_core0(void)
{//2
    BU_REG_WRITE(0xd411c000, 0x400);
    BU_REG_WRITE(0xd411c008, 0x406f);
    BU_REG_WRITE(0xd411c020, 0x6f);
    BU_REG_WRITE(0xd411c024, 0x1000000);
    BU_REG_WRITE(0xd411c200, 0x1);
    BU_REG_WRITE(0xd411c000, 0x0);
}

static void enable_etm_core1(void)
{ //3
    BU_REG_WRITE(0xd411d000, 0x400);
    BU_REG_WRITE(0xd411d008, 0x406f);
    BU_REG_WRITE(0xd411d020, 0x6f);
    BU_REG_WRITE(0xd411d024, 0x1000000);
    BU_REG_WRITE(0xd411d200, 0x2);
    BU_REG_WRITE(0xd411d000, 0x0);
}

static void enable_etm_core2(void)
{//2
    BU_REG_WRITE(0xd411c000, 0x400);
    BU_REG_WRITE(0xd411c008, 0x406f);
    BU_REG_WRITE(0xd411c020, 0x6f);
    BU_REG_WRITE(0xd411c024, 0x1000000);
    BU_REG_WRITE(0xd411c200, 0x4);
    BU_REG_WRITE(0xd411c000, 0x0);
}

static void enable_etm_core3(void)
{ //3
    BU_REG_WRITE(0xd411d000, 0x400);
    BU_REG_WRITE(0xd411d008, 0x406f);
    BU_REG_WRITE(0xd411d020, 0x6f);
    BU_REG_WRITE(0xd411d024, 0x1000000);
    BU_REG_WRITE(0xd411d200, 0x8);
    BU_REG_WRITE(0xd411d000, 0x0);
}

static void enable_coresight_others() { //other units
    BU_REG_WRITE(0xd4108304, 0x1000);
    BU_REG_WRITE(0xd4105304, 0x1);
    BU_REG_WRITE(0xd4109004, 0x0);
    BU_REG_WRITE(0xd4109000, 0x3);
    BU_REG_WRITE(0xd4105020, 0x1);
}
#endif //CONFIG_ETM_PTM

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the SOC system. Initialize the System.
 */

#define DTE_VALID_SIG   0xAABBCCDD
#define DTE_PASS_SIG    0x0
#define DTE_VALID_REG   (*(volatile uint32_t *)0xd1001900)
#define DTE_STATUS_REG  (*(volatile uint32_t *)0xd1001904)

//Those two only written here, read by all
uint32_t soc_type =0, soc_step=0;
uint32_t dte_done = 0, dte_pass = 0;

void SystemInit (void)
{
    //Mask Logs
    bbu_print_level = INFO_LEVEL;

//    PMUM->DBG = 0; //allow CR5 to access AP side AXI and SOC DDR.

    //should put at the very start
    icu_init();
#ifdef CONFIG_CP_ALONE
    setup_ffuart(uart_baudrate);
    setup_ffuart_interrupt();
#endif

    bbu_pid(&soc_type, &soc_step);
    irq_glb_enable();
    sys_timer_init(); //Initialize Seagull timers

    if(platform_is_realsi())
        pmic_init();

#ifdef CONFIG_CACHE
    #ifdef CONFIG_L2_CACHE
    nezha2_l2_cache_init();
    bbu_printf("L2 cache enabled\n\r");
    #endif
#endif
#if !defined(CONFIG_STRESS)
    if(platform_is_realsi())
        bbu_bid();
#endif
    ipc_init();
    ripc_init();
    //Unmask Logs
    bbu_print_level = INFO_LEVEL;
}
