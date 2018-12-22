#ifndef __host_common_h__
#define __host_common_h__
/**HEADER*********************************************************
-----------------------------------------------------------------------------
 CONFIDENTIAL AND PROPRIETARY INFORMATION OF TRANSDIMENSION, INC. AND/OR ITS
 WHOLLY OWNED SUBSIDIARY SOFTCONNEX TECHNOLOGIES, INC. THIS NOTICE IS NOT TO
 BE DELETED, MODIFIED, MOVED OR CHANGED IN ANY WAY.

 (TransDimension, Inc. and SoftConnex Technologies, Inc. are referred to
 collectively below as TransDimension, Inc.)
 Copyright (c) 1999 - 2004 by TransDimension, Inc.

 This software is protected by copyright laws and international copyright
 treaties, as well as other intellectual property laws and treaties.  This
 software is a CONFIDENTIAL, unpublished work of authorship, and with portions
 constituting TRADE SECRETS of TransDimension, Inc..  Any unauthorized use,
 disclosure, and/or reproduction of this software, or any part of this software;
 or distribution of this software in any form or by any means; or storage of
 this software in any database or retrieval system, without the express written
 consent of, and license from, TransDimension, Inc. is strictly prohibited.
 This software is protected under the copyright and/or trade secret laws in
 other countries in addition to USA.  All Rights Reserved.  Failure to abide by
 the use, disclosure and/or reproduction restrictions may result in civil and
 or criminal penalties, and will be prosecuted to the maximum extent of the law.
 -------------------------------------------------------------------------------

*** Comments:
***   This file contains definitions for USB common utilities.
***
**************************************************************************
**END*********************************************************/


/*------------------------------**
** Class / SubClass / Protocol  **
**------------------------------*/
#include "usb_main.h"
#define  USB_CLASS_AUDIO            1
#define  USB_SUBCLASS_UNDEFINED     0
#define  USB_SUBCLASS_AUD_CONTROL   1
#define  USB_SUBCLASS_AUD_STREAMING 2
#define  USB_SUBCLASS_AUD_MIDI_STRM 3
#define  USB_PROTOCOL_UNDEFINED     0

#define  USB_CLASS_COMMUNICATION    2
#define  USB_SUBCLASS_COM_DIRECT    1
#define  USB_SUBCLASS_COM_ABSTRACT  2
#define  USB_SUBCLASS_COM_TELEHONE  3
#define  USB_SUBCLASS_COM_MULTICHAN 4
#define  USB_SUBCLASS_COM_CAPI      5
#define  USB_SUBCLASS_COM_ETHERNET  6
#define  USB_SUBCLASS_COM_ATM_NET   7
#define  USB_PROTOCOL_COM_V25       1

#define  USB_CLASS_PRINTER          7
#define  USB_SUBCLASS_PRINTER       1
#define  USB_PROTOCOL_PRT_UNIDIR    1
#define  USB_PROTOCOL_PRT_BIDIR     2
#define  USB_PROTOCOL_PRT_1284      3

#define  USB_CLASS_MASS_STORAGE     8
#define  USB_SUBCLASS_MASS_RBC      1
#define  USB_SUBCLASS_MASS_ATAPI    2
#define  USB_SUBCLASS_MASS_QIC157   3
#define  USB_SUBCLASS_MASS_UFI      4
#define  USB_SUBCLASS_MASS_SFF8070I 5
#define  USB_SUBCLASS_MASS_SCSI     6
#define  USB_PROTOCOL_MASS_IRRPT    0
#define  USB_PROTOCOL_MASS_NOIRRPT  1
#define  USB_PROTOCOL_MASS_BULK     0x50


#define  USB_CLASS_HID              3
#define  USB_SUBCLASS_HID_NONE      0
#define  USB_SUBCLASS_HID_BOOT      1
#define  USB_PROTOCOL_HID_KEYBOARD  1
#define  USB_PROTOCOL_HID_MOUSE     2


#define  USB_CLASS_DATA             0x0A
/* No data subclasses, set to 0 */
#define  USB_PROTOCOL_DATA_I430     0x30
#define  USB_PROTOCOL_DATA_HDLC     0x31
#define  USB_PROTOCOL_DATA_TRANS    0x32
#define  USB_PROTOCOL_DATA_Q921M    0x50
#define  USB_PROTOCOL_DATA_Q921     0x51
#define  USB_PROTOCOL_DATA_Q921TM   0x52
#define  USB_PROTOCOL_DATA_V42BIS   0x90
#define  USB_PROTOCOL_DATA_EUROISDN 0x91
#define  USB_PROTOCOL_DATA_V120     0x92
#define  USB_PROTOCOL_DATA_CAPI20   0x93
#define  USB_PROTOCOL_DATA_HOST     0xFE
#define  USB_PROTOCOL_DATA_CDC      0xFE
#define  USB_PROTOCOL_DATA_VENDOR   0xFF

#if 0
/*--------------------------------------------------------------**
** Debug macros, assume _DBUG_ is defined during development    **
**    (perhaps in the make file), and undefined for production. **
**--------------------------------------------------------------*/
#ifndef _DBUG_
#define DEBUG_FLUSH
#define DEBUG_PRINT(X)
#define DEBUG_PRINT2(X,Y)
#else
#define DEBUG_FLUSH
#define DEBUG_PRINT(X)     BBU_putstr(X);
#define DEBUG_PRINT2(X,Y)
#endif


/*--------------------------------------------------------------**
** ASSERT macros, assume _ASSERT_ is defined during development **
**    (perhaps in the make file), and undefined for production. **
** This macro will check for pointer values and other validations
** wherever appropriate.
**--------------------------------------------------------------*/
#ifndef _ASSERT_
#define ASSERT(X,Y)
#else
#define ASSERT(X,Y)
#endif


#endif

/* Setup */
typedef struct
{
    uchar    BMREQUESTTYPE;
    uchar    BREQUEST;
    uint_8   WVALUE[2];
    uint_8   WINDEX[2];
    uint_8   WLENGTH[2];
} USB_SETUP, _PTR_ USB_SETUP_PTR;

/*-------------------------------------------------------------**
** Transaction callback parameters:                            **
**    pointer     pipe handle                                  **
**    pointer     parameter specified by higher level          **
**    uchar_ptr   pointer to buffer containing data (TX or RX) **
**    uint_32     length of data transferred                   **
**    USB_STATUS  status, preferably USB_OK or USB_DONE etc.   **
**-------------------------------------------------------------*/

typedef  void  (_CODE_PTR_ tr_callback)(pointer, pointer, uchar_ptr, uint_32, uint_32);


typedef pointer _usb_host_handle;                  /* host state struct   */
typedef pointer _usb_device_instance_handle;       /* item on host's list */
typedef pointer _usb_interface_handle;             /* class-related data  */
typedef pointer _usb_interface_descriptor_handle;  /* Chapter 9 descriptor*/
typedef pointer _usb_pipe_handle;                  /* pipe or pipe-bundle */
typedef pointer _usb_class_intf_handle;            /* interface info.     */
typedef pointer _usb_class_handle;
typedef uint_32 USB_STATUS;
typedef uint_32 descriptor_type;

/*---------------------------------------------------------**
** Attach/Detach callback parameters:                      **
**    _usb_device_instance_handle      new/removed device  **
**    _usb_interface_descriptor_handle matching class etc. **
**   uint_32                           code (attach etc.). **
**---------------------------------------------------------*/

typedef  void  (_CODE_PTR_ event_callback)
(_usb_device_instance_handle,
 _usb_interface_descriptor_handle,
 uint_32);

/* TR_STRUCT, TR_INIT_PARAM_STRUCT and PIPE_TR_STRUCT have
**  the following identical fields at the beginning  */
#define TR_INFO_FIELDS                                                 \
    \
    uint_32     TR_INDEX;           /* Transfer number on this pipe   */\
    uchar_ptr   TX_BUFFER;          /* address of buffer containing   */\
    /* the data to be transmitted     */\
    /* (including OUT data phase of   */\
    /* control transfers)             */\
    uchar_ptr   RX_BUFFER;          /* address of buffer to receive   */\
    /* data                           */\
    uint_32     TX_LENGTH;          /* length of data to transmit.    */\
    /* For control transfers, the     */\
    /* length of data for the         */\
    /* data phase                     */\
    uint_32     RX_LENGTH;          /* length of data to be received  */\
    /* For control transfers, this    */\
    /* indicates the length of data   */\
    /* for the data phase             */\
    tr_callback CALLBACK;           /* The callback function to be    */\
    /* invoked when a transfer is     */\
    /* completed or an error is to be */\
    /* reported                       */\
    pointer     CALLBACK_PARAM;     /* The second parameter to be     */\
    /* passed into the callback       */\
    /* function when it is invoked */



typedef struct
{
    TR_INFO_FIELDS
} TR_STRUCT, _PTR_ TR_STRUCT_PTR;

typedef struct
{
    TR_INFO_FIELDS                  /* 28 byte in size */
    uchar_ptr   DEV_REQ_PTR;        /* address of buffer containing the
                                   ** setup packet to send. Relevant
                                   ** for control pipes only */
} TR_INIT_PARAM_STRUCT, _PTR_ TR_INIT_PARAM_STRUCT_PTR;


/* PIPE_INIT_PARAM_STRUCT and PIPE_DESCRIPTOR_STRUCT have
**  the following identical fields at the beginning  */
#define PIPE_INFO_FIELDS                                             \
    \
    pointer     DEV_INSTANCE;       /* Instance on list of all devices that owns pipe.     */ \
    uint_32     INTERVAL;           /* Interval for polling pipe for data transfer.          */ \
    uint_32     MAX_PACKET_SIZE;    /* Pipe's maximum packet size for sending or receiving.   */ \
    uint_32     NAK_COUNT;          /* The maximum number of NAK   */ \
    /* responses tolerated.        */ \
    /* MUST be zero for interrupt  */ \
    /* For USB 1.1, after count    */ \
    /* NAKs/frame,transaction is   */ \
    /* deferred to next frame      */ \
    /* For USB2.0, the host        */ \
    /* controller will not execute */ \
    /* transaction following this  */ \
    /* many NAKs.                  */ \
    uint_32     FIRST_FRAME;        /* The frame # to start xfer   */ \
    uint_32     FIRST_UFRAME;       /* Microframe # to start xfer  */ \
    uint_32     FLAGS;              /* After all data transferred, */ \
    /* should we terminate the     */ \
    /* transfer with a zero length */ \
    /*  packet if the last packet  */ \
    /*  size == MAX_PACKET_SIZE?   */ \
    uint_8      DEVICE_ADDRESS;     /* The device's address 1-127  */ \
    uint_8      ENDPOINT_NUMBER;    /* The endpoint number         */ \
    uint_8      DIRECTION;          /* Input or Output             */ \
    uint_8      PIPETYPE;           /* Control, interrupt, bulk or  isochronous                 */ \
    uint_8      SPEED;              /* High/full/low               */ \
    uint_8      TRS_PER_UFRAME;     /* Number of transaction per   */ \
    /* frame, only high-speed      */ \
    /* high-bandwidth pipes.       */

typedef struct
{
    PIPE_INFO_FIELDS
} PIPE_INIT_PARAM_STRUCT, _PTR_ PIPE_INIT_PARAM_STRUCT_PTR;

/* Class Callback function storage structure */
typedef struct device_class_info_struct
{
    uint_8               CLASS_TYPE;
    uint_8               SUB_CLASS;
    uint_8               PROTOCOL;
} DEVICE_CLASS_INFO_STRUCT, _PTR_ DEVICE_CLASS_INFO_STRUCT_PTR;

#ifdef _DBUG_
extern uint_32 trace_index;
extern uint_32 trace_max;
extern uint_32 trace_array[];
#define DBUG_TRACE(X)                                  \
    trace_array[trace_max & trace_index] = (uint_32)(X);\
    trace_array[trace_max & (trace_index+2)] = 0xABCDEF;\
    trace_index++;                                      \
     
#else
#define  DBUG_TRACE(X)
#endif


/******************************************************************************
**
** Macros to swap between USB byte arrays and HOST types.
** Stack and applications should use the following API:
**
** READ functions
**    Parameters:
**       "p"            Pointer to data stored in USB order byte array
**
**    Macros:
**       utoh32(p)      Read 32 bit number from "p", return HOST uint_32
**       utoh16(p)      Read 16 bit number from "p", return HOST uint_16
**       utoh8(p)       Read  8 bit number from "p", return HOST uint_8
**
** WRITE functions
**    Parameters:
**       "p"            Array of bytes in which to store data in USB byte order
**       "x"            A HOST data type to be converted into a byte array
**
**    Macros:
**       htou32(p,x)    Store "x" (uint_32) into USB order byte array at "p"
**       htou16(p,x)    Store "x" (uint_16) into USB order byte array at "p"
**       htou8(p,x)     Store "x" (uint_8) into USB order byte array at "p"
*/


#define HOST_WRITE_OCT32(p,x,a,b,c,d) ((p)[a] = ((uint_32)(x)) >> 24 & 0xFF, \
                                       (p)[b] = ((uint_32)(x)) >> 16 & 0xFF, \
                                       (p)[c] = ((uint_32)(x)) >>  8 & 0xFF, \
                                       (p)[d] = ((uint_32)(x))       & 0xFF)

#define HOST_WRITE_OCT16(p,x,a,b)     ((p)[a] = ((uint_16)(x)) >>  8 & 0xFF, \
                                       (p)[b] = ((uint_16)(x))       & 0xFF)

#define HOST_WRITE_OCT8(p,x)          ((p)[0] = (( uint_8)(x))       & 0xFF)

#define HOST_READ_OCT32(p,a,b,c,d)  ((((uint_32)(p)[a] & 0xFF) << 24) | \
                                     (((uint_32)(p)[b] & 0xFF) << 16) | \
                                     (((uint_32)(p)[c] & 0xFF) <<  8) | \
                                     (((uint_32)(p)[d] & 0xFF)      ))

#define HOST_READ_OCT16(p,a,b)      ((((uint_16)(p)[a] & 0xFF) <<  8) | \
                                     (((uint_16)(p)[b] & 0xFF)      ))

#define HOST_READ_OCT8(p)            (((( uchar)(p)[0] & 0xFF)      ))


#define HOST_WRITE_BEOCT_32(p,x)    HOST_WRITE_OCT32(p,x,0,1,2,3)
#define HOST_WRITE_LEOCT_32(p,x)    HOST_WRITE_OCT32(p,x,3,2,1,0)
#define HOST_WRITE_BEOCT_16(p,x)    HOST_WRITE_OCT16(p,x,0,1)
#define HOST_WRITE_LEOCT_16(p,x)    HOST_WRITE_OCT16(p,x,1,0)
#define HOST_WRITE_BEOCT_8(p,x)     HOST_WRITE_OCT8(p,x)
#define HOST_WRITE_LEOCT_8(p,x)     HOST_WRITE_OCT8(p,x)

#define HOST_READ_BEOCT_32(p)       HOST_READ_OCT32(p,0,1,2,3)
#define HOST_READ_LEOCT_32(p)       HOST_READ_OCT32(p,3,2,1,0)
#define HOST_READ_BEOCT_16(p)       HOST_READ_OCT16(p,0,1)
#define HOST_READ_LEOCT_16(p)       HOST_READ_OCT16(p,1,0)
#define HOST_READ_BEOCT_8(p)        HOST_READ_OCT8(p)
#define HOST_READ_LEOCT_8(p)        HOST_READ_OCT8(p)

#define utoh32(p)    HOST_READ_OCT32(p,3,2,1,0)
#define utoh16(p)    HOST_READ_OCT16(p,1,0)
#define utoh8(p)     HOST_READ_OCT8(p)

#define htou32(p,x)  HOST_WRITE_OCT32(p,x,3,2,1,0)
#define htou16(p,x)  HOST_WRITE_OCT16(p,x,1,0)
#define htou8(p,x)   HOST_WRITE_OCT8(p,x)



/* Information for one class or device driver */
typedef struct driver_info
{
    uint_8         idVendor[2];      /* Vendor ID per USB-IF */
    uint_8         idProduct[2];     /* Product ID per manufacturer */
    uint_8         bDeviceClass;     /* Class code, if 0 see interface */
    uint_8         bDeviceSubClass;  /* Sub-Class code, 0 if class = 0 */
    uint_8         bDeviceProtocol;  /* Protocol, if 0 see interface */
    uint_8         reserved;         /* Alignment padding */
    event_callback attach_call;
} USB_HOST_DRIVER_INFO, _PTR_ USB_HOST_DRIVER_INFO_PTR;

/* Pipe bundle = device handle + interface handle +
**                1..N pipe handles
** NOTE: The pipe handles are for non-control pipes only, i.e.
**          pipes belonging strictly to this interface.
**       The control pipe belongs to the device, even if it
**          is being used by the device's interfaces.  Hence
**          a pointer to the device instance is provided.
**          Closing pipes for the interface does NOT close
**          the control pipe which may still be required to
**          set new configurations/interfaces etc. */
typedef struct pipe_bundle_struct
{
    _usb_device_instance_handle      dev_handle;
    _usb_interface_descriptor_handle intf_handle;
    _usb_pipe_handle                 pipe_handle[4];
} PIPE_BUNDLE_STRUCT, _PTR_ PIPE_BUNDLE_STRUCT_PTR;

/* The following struct is for storing a class's validity-check
** code with the pointer to the data.  The address of one such
** struct is passed as a pointer to select-interface calls,
** where values for that interface get initialized.  Then the
** struct should be passed to class calls using the interface. */
#if 0
typedef struct class_call_struct
{
    _usb_class_intf_handle  class_intf_handle;
    uint_32                 code_key;
}  CLASS_CALL_STRUCT, _PTR_ CLASS_CALL_STRUCT_PTR;
#endif
/* The following define the internal mapping from interface
** class / sub-class / protocol to class drivers
*/
typedef  void  (_CODE_PTR_ INTF_INIT_CALL)
(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
typedef  struct  class_map
{
    INTF_INIT_CALL class_init;       /* class driver initialization */
    uint_16        intf_struct_size; /* size of class's data struc  */
    uint_8         class;            /* interface descriptor class, */
    uint_8         sub_class;        /*   sub-class, and            */
    uint_8         protocol;         /*   protocol.                 */
    uint_8         class_mask;       /* masks ANDed with class etc. */
    uint_8         sub_mask;         /*   mask = 0x00 matches any   */
    uint_8         prot_mask;        /*   mask = 0xFF must be exact */
} CLASS_MAP, _PTR_ CLASS_MAP_PTR;

/* The generalized heading can alias to any class-intf
** which defines its first four fields as follows: */
typedef struct general_class
{
    struct general_class _PTR_       next;    /* next list item */
    struct general_class _PTR_       anchor;  /* list start */
    _usb_device_instance_handle      dev_handle;
    _usb_interface_descriptor_handle intf_handle;
    _usb_host_handle                 host_handle;
    uint_32                          key_code;
} GENERAL_CLASS, _PTR_ GENERAL_CLASS_PTR;

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS usb_host_class_intf_init(PIPE_BUNDLE_STRUCT_PTR,
                                    pointer, pointer);

uint_32  usb_host_class_intf_validate(pointer);

USB_STATUS _usb_host_driver_info_register(_usb_host_handle,
        pointer);

USB_STATUS usb_hostdev_busy_check(_usb_device_instance_handle);

_usb_pipe_handle _usb_hostdev_find_pipe_handle(_usb_device_instance_handle,
        _usb_interface_descriptor_handle, uint_8, uint_8);

USB_STATUS _usb_hostdev_get_buffer(_usb_device_instance_handle,
                                   uint_32, uchar_ptr _PTR_ );

USB_STATUS _usb_hostdev_get_descriptor(_usb_device_instance_handle,
                                       descriptor_type, uint_8, uint_8, pointer _PTR_);

_usb_pipe_handle usb_hostdev_get_pipe_handle(PIPE_BUNDLE_STRUCT_PTR,
        uint_8, uint_8);

USB_STATUS _usb_hostdev_select_config(_usb_device_instance_handle,
                                      uint_8);

USB_STATUS _usb_hostdev_select_interface(_usb_device_instance_handle,
        _usb_interface_descriptor_handle, pointer);

void usb_hostdev_tr_init(TR_INIT_PARAM_STRUCT_PTR, tr_callback, pointer);

USB_STATUS usb_hostdev_validate(_usb_device_instance_handle);

extern uint_32 _usb_hostdev_get_speed(_usb_device_instance_handle);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
