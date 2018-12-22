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

#define BAUDRATE_9600     		9600
#define BAUDRATE_14400     		14400
#define BAUDRATE_38400     		38400
#define BAUDRATE_57600     		57600
#define	BAUDRATE_115200			115200
#define	BAUDRATE_921600			921600
#define BAUDRATE_1842000	    1842000
#define BAUDRATE_3686400        3686400

extern uint32_t uart_baudrate;


void setup_ffuart(int baudrate);
void setup_ffuart_interrupt(void);
void lpm_ffuart(void);

#endif /* __UART_DEC_H */

