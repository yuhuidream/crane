/**************************************************************************//**
 * @file     system_mrvl.c
 * @brief    CMSIS-like System Source File for Nevo SOC
 * @note
 * Copyright (C) 2011 Marvell. All rights reserved.
 *
 *******************************************************************************/
#include "predefines.h"
#include "PMUA.h"
#include "PMUM.h"
#include "APBCLK.h"
#include "uart_dec.h"
#include "interrupt.h"
#include "gpio_dec.h"
#include "timer_dec.h"
#include "system_mrvl.h"
#include "CIU.h"
#include "PMR.h"
#include "gic_dec.h"
#include "cache_ops.h"
#include "set_mmu.h"
#include "procida.h"
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
//uint32_t dte_done = 0, dte_pass = 0;

void SystemInit (void)
{
#ifdef CONFIG_CA7
///////////////////////////////////////////////////////////////////////////////
//                  Initialization Work of Cortex-A7                         //
///////////////////////////////////////////////////////////////////////////////
    enable_clocks();

    //CP UART
    *(volatile uint32_t *)(0xd401e1A8) = 0x0000D0C1;
    *(volatile uint32_t *)(0xd401e1AC) = 0x0000D0C1;

    //AP UART
    *(volatile uint32_t *)(0xd401e150) = 0x0000D0C1;
    *(volatile uint32_t *)(0xd401e154) = 0x0000D0C1;

//    *(volatile uint32_t *)(0xd401e2F8) = 0x0000D0C6;
//    *(volatile uint32_t *)(0xd401e2FC) = 0x0000D0C6;

    setup_ffuart(uart_baudrate); /*UART poll mode can be enabled early, enable interrupt later*/
    setup_ffuart_interrupt();

#ifdef CONFIG_CACHE
    enable_caches();
#endif

    //There is rtc interrupt enable inside, should be after GIC init
    sys_timer_init();

    if(is_i_cache_enabled() && is_d_cache_enabled())
       bbu_printf("I&D cache of CA7 enabled\n\r");
    else
       bbu_printf("Caches are disabled\n\r");

   if(!platform_is_realsi())
        init_lpddr2();

    //bbu_mtsp_test(0x0100000,0x10000,0xFFFFFFFF,0);

#ifndef CONFIG_NANO_BL
    //Need to release CR5 here and wait CR5 finish some initial work
    if(platform_is_emulator())
        //With special ROM, cr5 will jump to 0x0@DDR
        CIU->SW_BRANCH_ADDR = 0x4;
    else
        release_seagull();
#endif
#else
///////////////////////////////////////////////////////////////////////////////
//                 Initialization Work of Cortex-R5                          //
///////////////////////////////////////////////////////////////////////////////
    /* setup uart to print the below logs, move the interrupt enable part 
       to cli_initial, we can't enable uart interrupt here because the
       event pools and AO_Cli hasn't been initialized so far */
#ifdef CONFIG_CP_ALONE
    enable_clocks();

    //CP UART
    *(volatile uint32_t *)(0xd401e1A8) = 0x0000D0C1;
    *(volatile uint32_t *)(0xd401e1AC) = 0x0000D0C1;

    //AP UART
    *(volatile uint32_t *)(0xd401e150) = 0x0000D0C1;
    *(volatile uint32_t *)(0xd401e154) = 0x0000D0C1;

    setup_ffuart(uart_baudrate); /*UART poll mode can be enabled early, enable interrupt later*/
    setup_ffuart_interrupt();

    //if(!platform_is_realsi())
    //    init_lpddr2();

#endif

#if 0
#ifdef CONFIG_MMT
extern uint8_t readUsbInitFlag(void);
    uint8_t  flag=0;
    usb_serial_init(1);

    while(!flag)
    {
        flag=readUsbInitFlag();
        udelay(1);
    }
#endif
#endif
    sys_timer_init();

#ifndef CONFIG_NATO
//    infEmeiReadPartInfo();
//    bbu_bid();
    bbu_pid(&soc_type, &soc_step);
#endif

    //init all I2C unit after init timer, need delay when init i2c
    if(platform_is_realsi())
        i2c_init_all();

#ifdef CONFIG_CACHE
    bbu_printf("L1 caches of CR5 enabled\n\r");
    nezha2_l2_cache_init();
#endif

    /* check if the board have pmic on it, this function must be called
    * before any pmic operations
     */
    if(platform_is_realsi())
        pmic_init();

    bbu_printf("\r\n**ASR NezhaC BBU revision SLT_ATE_R0**\r\n"
               "Compiled on %s %s with BBU_CONF=%s\r\n", \
               __DATE__, __TIME__, CONF_STRING);
    bbu_printf("Hello world, this is Cortex R5\n\r");
#endif
}
