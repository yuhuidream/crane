/*****************************************************************************
* Product: BBU
* 
* Purpose: This file contains the defines and declarations for the 
*          UART setup code.
* 
* Copyright (C) 2010-2011 Marvell MTBL-Hefei. All rights reserved.
*
* Author: Lianghu Xu  email: xlianghu@marvell.com
* 
*****************************************************************************/
#ifndef __UART_DEC_H
#define __UART_DEC_H

//=============================================================================
//                                DEFINITIONS
//=============================================================================

#define UARTCLK_L 	14857000
#define UARTCLK_H   59429000   

#define NAK_TIMEOUT_VAL    	(int)((10.0 / (4131.0e-9 * 65536.0)) + 0.5)

extern uint32_t uart_baudrate;

#define BAUDRATE_9600     		9600
#define BAUDRATE_14400     		14400
#define BAUDRATE_38400     		38400
#define BAUDRATE_57600     		57600
#define	BAUDRATE_115200			115200
#define	BAUDRATE_921600			921600
#define BAUDRATE_1842000	    1842000
#define BAUDRATE_3686400        3686400


void setup_ffuart(int baudrate);
void setup_ffuart_interrupt(void);
void lpm_ffuart(void);

#ifdef CONFIG_CPUART
#define DBG_UART_IRQ   IRQ_MMP_NEWUART
#else
#define DBG_UART_IRQ   IRQ_MMP_UART1
#endif

#ifdef Q_SPY
#ifdef Q_SPY_EXT
void setup_qspy_ffuart(int baudrate);
void setup_qspy_ffuart_interrupt(void);
#endif
#endif
#endif /* __UART_DEC_H */

