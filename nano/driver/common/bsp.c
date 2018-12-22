/*****************************************************************************
* Product: "Fly 'n' Shoot" game example with cooperative "Vanilla" kernel
* Last Updated for Version: 4.3.00
* Date of the Last Update:  Nov 01, 2011
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2011 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/

/*This folder contain the driver for QP and board init functions necessary for system to start run, such pin-mux,etc*/

#include "predefines.h"
#include "bsp.h"
#include "bbu.h"
#include "TIME_REG.h"
#include "UART.h"
#include "APBCLK.h"
#include "interrupt.h"
#include "system_mrvl.h"
#include "timer_dec.h"
#include "gic_dec.h"
#include "PMR.h"
#include "PMUA.h"
#include "utils.h"
#include "ICU.h"
#include "CIU.h"

/*..........................................................................*/
void SysTick_Handler(void *tick) {
    (void)tick; /* avoid warning */
}

/*..........................................................................*/
#if 0
void BSP_init(int argc, char *argv[]) {

/* initialize the whole system,such as clock, pin-mux initial, interrupts, necessary peripherials,etc. */
    SystemInit();
    (void)argc;           /* avoid compiler warning about unused parameters */
    (void)argv;
}
#endif

void Q_onAssert(void){
}

/*..........................................................................*/
/* error routine that is called if the CMSIS library encounters an error    */
void assert_failed(char const *file, int line) {
    while(1);
}

int platform_type(void)
{
    return (CIU->CHIP_ID >> 24) & 0xF;
}

int platform_is_zebu(void)
{
    return (platform_type() == ZEBU_Z1);
}

int platform_is_z1(void)
{
    return (platform_type() == ZEBU_Z1);
}

int platform_is_haps(void)
{
    return (platform_type() == HAPS);
}

int platform_is_emulator(void)
{
    return (platform_type() == ZEBU_Z1);
}

int platform_is_realsi(void)
{
    return (platform_type() == REAL_SI);
}

/*--------------------------------------------------------------------------*/
