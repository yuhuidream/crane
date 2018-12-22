// ============================================================================
//
//     Copyright (c) Marvell Corporation 2013  -  All rights reserved
//
//  This computer program contains confidential and proprietary information,
//  and  may NOT  be reproduced or transmitted, in whole or in part,  in any
//  form,  or by any means electronic, mechanical, photo-optical, or  other-
//  wise, and  may NOT  be translated  into  another  language  without  the
//  express written permission from Marvell Corporation.
//
// ============================================================================
// =      C O M P A N Y      P R O P R I E T A R Y      M A T E R I A L       =
// ============================================================================

/*
-------------------------------------------------------------------------------
*
*
*  description:   USB serial  driver header file
*
*  author:chaotong@marvell.com
*
*
*
*
*-----------------------------------------------------------------------------*/
#ifndef _USB_serial_H_
#define _USB_serial_H_
#include "usb_types.h"
//#include "define.h"

/*
 * Descriptor types ... USB 2.0 spec table 9.5
 */
/* This must be counted manually and updated with the descriptor */
/* 1*Config(9) + 1*ACStandInterface(9)+ 1*ACClassInterface(1e) + 1*ASStandInterface0(9)+1*ASGenrInterface0(7)
 +1*Type1(b)+1*ASStandEndpoint0(9)+1*ASClassEndpoint0(7)+1*ASStandInterface1(9)+1*ASGenrInterface1(7)
 +1*Type1(b)+1*ASStandEndpoint1(9)+1*ASClassEndpoint1(7)=0x86          by tongchao*/

typedef struct
{
    uint_32 bitrate;
    uint_8 format;
    uint_8 paritytype;
    uint_8 datatype;
} LINE_CODING;

typedef struct
{
    uint_16 index;
    LINE_CODING current_line;

} Reaserch_Line_Codeing;
#define SEND_ENCAPSULATED_COMMAND   0x00
#define GET_ENCAPSULATED_RESPONSE   0x01
#define SET_COMM_FEATURE            0x02
#define GET_COMM_FEATURE            0x03
#define CLEAR_COMM_FEATURE          0x04
#define SET_LINE_CODING             0x20
#define GET_LINE_CODING             0x21
#define SET_CONTROL_LINE_STATE      0x22
#define SEND_BREAK                  0x23

#define VIRTUAL_COM_PORT_DATA_SIZE  512//64

#define status() 1

//#ifdef LINUX_CDC
//#define DRIVER_VENDOR_ID  0x0AEB//0x12d1//0x0403////0x19d2//0x05A3    // Marvell0x1286//
//#define DRIVER_PRODUCT_ID 0x0012//0x1da1//0x6001////0x0366//0x8380    0x4E31//
//#else
#define DRIVER_VENDOR_ID    0x1286          //0x1288  
#define DRIVER_PRODUCT_ID    0x4E33                   //0x4E31  0x4E33
//#endif

#define DRIVER_RELEASE_ID   0x0100  //device driver release number: 0.1.0.0


#define ISR_EP_SIZE        16//0x0f
#define COM_PORT_DATA_SIZE              64

#define USB_CDC_SUBCLASS_ACM         2
#define USB_CDC_ACM_PROTO_AT_V25TER 1

#define MODEM_COMM_NOTIFY_SERIAL_STATE_LENGTH  10

#define ACM_CTRL_DCD        0x01
#define ACM_CTRL_DSR        0x02
#define ACM_CTRL_BRK        0x04
#define ACM_CTRL_RI            0x08
#define ACM_CTRL_FRAMING    0x10
#define ACM_CTRL_PARITY     0x20
#define ACM_CTRL_OVERRUN    0x40
//#ifdef LINUX_CDC
#define acmInterfaceNum     0  //4
#define acmInterfaceNum2   2
//#else
//#define acmInterfaceNum                 2
//#define acmInterfaceNum2         4
//#endif

//extern USB_DEV_STATE_STRUCT_PTR getUSBhandle(void);

#endif
