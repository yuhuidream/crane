/*****************************************************************************
* Product: "Fly'n'Shoot" game example
* Last Updated for Version: 4.0.04
* Date of the Last Update:  Aug 01, 2009
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
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
#include "bbu.h"
#include "bsp.h"
#include "pmic.h"
#ifdef NANO_SDH
#include "mmc_ops.h"
#endif
/* Local-scope objects -----------------------------------------------------*/

//extern int bbu_qspi_test2(void);

static int cr5_main(int argc, char *argv)
{
    //Some initial work of Cortex-R5
    #ifndef CONFIG_NATO
    //pp_init();  //CA7 does not touch PP in NANO BBU to save binary size
    #endif
    SystemInit();

    //CP_RELEASE_FLAG = CR5_RELEASED;

    bbu_printf(BBU_PROMPT);
    cli_handler();

}

static int ca7_main(int argc, char *argv)
{
    uint32_t output_info;

    SystemInit();

#ifdef CONFIG_MMT
    bbu_output_init1();
#endif

    if(platform_is_realsi())
        ddr_swp_init();

    //Some initial work of Cortex-A7
#ifndef CONFIG_NATO
    #ifdef CONFIG_MMT
    bbu_putchar(0x86);
    #endif
    //ustica_vbuck235_on(VBUCK3, 1250,1250);
    #ifdef NANO_SDH
    if(mmc_card_init())
        bbu_printf("Error: mmc init faild!\r\n");
    #endif
#endif

#ifdef CONFIG_MMT
    bbu_output_init2();
#endif

    output_info = BU_REG_READ(0xd4282ce8);
    output_info = (output_info & 0xf0) >> 4;
    if(output_info)
    bbu_printf("\n\rUSBSerial\n\r");
    else
    bbu_printf("\n\rUARTSerial\n\r");

    //bbu_mtsp_test(0x100000, 0x40000, 0xf, 0);
    //transfer_ctrl(0x1000000);

    cli_handler();

}

/*..........................................................................*/
int main(int argc, char *argv[]) {
#ifdef CONFIG_CA7
    ca7_main(argc, argv);
#endif

#ifdef CONFIG_CR5
    cr5_main(argc, argv);
#endif
    return 0;
}
