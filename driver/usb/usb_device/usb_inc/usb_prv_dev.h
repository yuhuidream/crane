#ifndef __usbprv_dev_h__
#define __usbprv_dev_h__ 1
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

*** Description:      
***  This file contains the private defines, externs and
***  data structure definitions required by the VUSB_HS Device
***  driver.
***                                                               
**************************************************************************
**END*********************************************************/
#include "predefines.h"
#include "vusbhs.h" 
#include"USB.h"
#include <string.h>
#define  MAX_EP_TR_DESCRS                    (32)
#define  MAX_XDS_FOR_TR_CALLS                (32)
#define  MAX_USB_DEVICES                     (1)
#define  USB_MAX_ENDPOINTS                   (16)

/*Assumption here is that all control endpoints are sequential 0,1,..
if they are not you need to modify the tr_complete routine to handle that */
#define  USB_MAX_CONTROL_ENDPOINTS           (1)

#define  USB_MAX_CTRL_PAYLOAD                (64)
 
/* Macro for aligning the EP queue head to 32 byte boundary */
#define USB_MEM32_ALIGN(n)                   ((n) + (-(n) & 31))

/* Macro for aligning the EP queue head to 1024 byte boundary */
#define USB_MEM1024_ALIGN(n)                 ((n) + (-(n) & 1023))

/* Macro for aligning the EP queue head to 1024 byte boundary */
#define USB_MEM2048_ALIGN(n)                 ((n) + (-(n) & 2047))

#define USB_XD_QADD(head,tail,XD)      \
   if ((head) == NULL) {         \
      (head) = (XD);            \
   } else {                      \
      (tail)->SCRATCH_PTR->PRIVATE = (XD);   \
   } /* Endif */                 \
   (tail) = (XD);               \
   (XD)->SCRATCH_PTR->PRIVATE = NULL
   
#define USB_XD_QGET(head,tail,XD)      \
   (XD) = (head);               \
   if (head) {                   \
      (head) = (XD_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#define EHCI_DTD_QADD(head,tail,dTD)      \
   if ((head) == NULL) {         \
      (head) = (dTD);            \
   } else {                      \
      (tail)->SCRATCH_PTR->PRIVATE = (void *) (dTD);   \
   } /* Endif */                 \
   (tail) = (dTD);               \
   (dTD)->SCRATCH_PTR->PRIVATE = NULL
   
#define EHCI_DTD_QGET(head,tail,dTD)      \
   (dTD) = (head);               \
   if (head) {                   \
      (head) = (head)->SCRATCH_PTR->PRIVATE;  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

/*
#define EHCI_DTD_QGET(head,tail,dTD)      \
   (dTD) = (head);               \
   if (head) {                   \
      (head) = (VUSB20_EP_TR_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      }               \
   }
*/
/***************************************
**
** Data structures
**
*/

/* Callback function storage structure */
typedef struct service_struct {
   uint_8   TYPE;
   void     (_CODE_PTR_ SERVICE)(pointer, boolean, uint_8, uint_8_ptr, uint_32, uint_8);
   struct service_struct _PTR_   NEXT;
} SERVICE_STRUCT, _PTR_ SERVICE_STRUCT_PTR;

typedef struct xd_struct {
   uchar          EP_NUM;           /* Endpoint number */
   uchar          BDIRECTION;       /* Direction : Send/Receive */
   uint_8         EP_TYPE;          /* Type of the endpoint: Ctrl, Isoch, Bulk, 
                                    ** Int 
                                    */
   uchar          BSTATUS;          /* Current transfer status */
   uchar_ptr      WSTARTADDRESS;    /* Address of first byte */
   uint_32        WTOTALLENGTH;     /* Number of bytes to send/recv */
   uint_32        WSOFAR;           /* Number of bytes recv'd so far */
   uint_16        WMAXPACKETSIZE;   /* Max Packet size */
   boolean        DONT_ZERO_TERMINATE;
   uint_8         MAX_PKTS_PER_UFRAME;
   SCRATCH_STRUCT_PTR SCRATCH_PTR;
} XD_STRUCT, _PTR_ XD_STRUCT_PTR;

/* The USB Device State Structure */
typedef struct {
   boolean                          BUS_RESETTING;       /* Device is 
                                                         ** being reset 
                                                         */
   boolean                          TRANSFER_PENDING;    /* Transfer pending ? */

   VUSB20_REG_STRUCT_PTR          	DEV_PTR; /* Device Controller 
                                                         ** Register base 
                                                         ** address 

                                                         */


   
  MAVL_USB_TypeDef_PTR            NEVO_DEV_PTR;     /* Device Controller             //by tongchao
                                                         ** Register base 
                                                         ** address 

                                                         */ 

														 
   pointer                          CALLBACK_STRUCT_PTR;
   
   SERVICE_STRUCT_PTR               SERVICE_HEAD_PTR;       /* Head struct 
                                                         ** address of 
                                                         ** registered services 
                                                         */
   XD_STRUCT_PTR                    TEMP_XD_PTR;         /* Temp xd for ep init */
   XD_STRUCT_PTR                    XD_BASE;
   XD_STRUCT_PTR                    XD_HEAD;             /* Head Transaction 
                                                         ** descriptors 
                                                         */
   XD_STRUCT_PTR                    XD_TAIL;             /* Tail Transaction 
                                                         ** descriptors 
                                                         */
   XD_STRUCT_PTR                    PENDING_XD_PTR;      /* pending transfer */
   uint_32                          XD_ENTRIES;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR  EP_QUEUE_HEAD_PTR;   /* Endpoint Queue 
                                                         ** head 
                                                         */
   uchar_ptr                        DRIVER_MEMORY;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR  EP_QUEUE_HEAD_BASE;
   VUSB20_EP_TR_STRUCT_PTR          DTD_BASE_PTR;        /* Device transfer 
                                                         ** descriptor pool 
                                                         ** address 
                                                         */
   VUSB20_EP_TR_STRUCT_PTR          DTD_ALIGNED_BASE_PTR;/* Aligned transfer 
                                                         ** descriptor pool 
                                                         ** address 
                                                         */
   VUSB20_EP_TR_STRUCT_PTR          DTD_HEAD;
   VUSB20_EP_TR_STRUCT_PTR          DTD_TAIL;
   VUSB20_EP_TR_STRUCT_PTR          EP_DTD_HEADS[USB_MAX_ENDPOINTS * 2];
   VUSB20_EP_TR_STRUCT_PTR          EP_DTD_TAILS[USB_MAX_ENDPOINTS * 2];
   SCRATCH_STRUCT_PTR               XD_SCRATCH_STRUCT_BASE;
   
   
   SCRATCH_STRUCT_PTR               SCRATCH_STRUCT_BASE;
   
   /* These fields are kept only for USB_shutdown() */
   void(_CODE_PTR_                  OLDISR_PTR)(pointer);
   pointer                          OLDISR_DATA;
   uint_16                          USB_STATE;
   uint_16                          USB_DEVICE_STATE;
   uint_16                          USB_SOF_COUNT;
   uint_16                          DTD_ENTRIES;
   uint_16                          ERRORS;
   uint_16                          USB_DEV_STATE_B4_SUSPEND;
   uint_8                           DEV_NUM;             /* USB device number 
                                                         ** on the board 
                                                         */
   uint_8                           DEV_VEC;             /* Interrupt vector 
                                                         ** number for USB 
                                                         */
   uint_8                           SPEED;               /* Low Speed, 
                                                         ** High Speed, 
                                                         ** Full Speed 
                                                         */
   uint_8                           MAX_ENDPOINTS;       /* Max endpoints
                                                         ** supported by this
                                                         ** device
                                                         */
                                                         
   uint_8                           USB_CURR_CONFIG;                                                         
   uint_8                           DEVICE_ADDRESS;
} USB_DEV_STATE_STRUCT, _PTR_ USB_DEV_STATE_STRUCT_PTR;


/***************************************
**
** Prototypes
**
*/
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __USB_OS_MQX__
extern void usb_dci_vusb20_isr(pointer);
#else
extern DEVICE_INTERRUPT_ROUTINE_KEYWORD void usb_dci_vusb20_isr(void);
#endif


extern uint_8 bsp_get_usb_vector(uint_8 device_number);
//extern pointer bsp_get_usb_base(uint_8 device_number);
extern pointer bsp_get_nevo_usb_base(uint_8 device_number);
//extern pointer bsp_get_usb_capability_register_base(uint_8 device_number);
extern void usb_dci_vusb20_process_tr_complete(usb_device_handle handle);
extern void usb_dci_vusb20_process_reset(usb_device_handle handle);
extern void usb_dci_vusb20_process_tr_complete(usb_device_handle handle);
extern void usb_dci_vusb20_process_suspend(usb_device_handle handle);
extern void usb_dci_vusb20_process_SOF(usb_device_handle handle);
extern void usb_dci_vusb20_process_port_change(usb_device_handle handle);
extern void usb_dci_vusb20_process_error(usb_device_handle handle);

// fl: can not find declear extern void _usb_dci_vusb20_hnp_shutdown();


#ifdef __cplusplus
}
#endif

#endif
