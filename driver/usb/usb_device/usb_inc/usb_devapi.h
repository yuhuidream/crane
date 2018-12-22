#ifndef __devapi_h__
#define __devapi_h__ 1
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
***  This file contains the declarations specific to the USB Device API
***
**************************************************************************
**END*********************************************************/
//#include "usbtypes.h"
#include "predefines.h"
#include "USB.h"
//#include "usbprv.h"
#include "usb_types.h"
//#include "usb_prv.h"
//#define _DEVICE_DEBUG_ 1


#define  USB_SEND                         (1)
#define  USB_RECV                         (0)

#ifndef __USB_OS_MQX__
#define USB_lock()                         _disable_interrupts()
#define USB_unlock()                        _enable_interrupts()
#endif


/* Endpoint types */
#define  USB_CONTROL_ENDPOINT             (0)
#define  USB_ISOCHRONOUS_ENDPOINT         (1)
#define  USB_BULK_ENDPOINT                (2)
#define  USB_INTERRUPT_ENDPOINT           (3)

/* Informational Request/Set Types */
#define  USB_STATUS_DEVICE_STATE          (0x01)
#define  USB_STATUS_INTERFACE             (0x02)
#define  USB_STATUS_ADDRESS               (0x03)
#define  USB_STATUS_CURRENT_CONFIG        (0x04)
#define  USB_STATUS_SOF_COUNT             (0x05)
#define  USB_STATUS_DEVICE                (0x06)
#define  USB_STATUS_TEST_MODE             (0x07)
#define  USB_FORCE_FULL_SPEED             (0x08)
#define  USB_PHY_LOW_POWER_SUSPEND        (0x09)
#define  USB_STATUS_ENDPOINT              (0x10)
#define  USB_STATUS_ENDPOINT_NUMBER_MASK  (0x0F)

#define  USB_TEST_MODE_TEST_PACKET        (0x0400)

/* Available service types */
/* Services 0 through 15 are reserved for endpoints */
#define  USB_SERVICE_EP0                  (0x00)
#define  USB_SERVICE_EP1                  (0x01)
#define  USB_SERVICE_EP2                  (0x02)
#define  USB_SERVICE_EP3                  (0x03)
#define  USB_SERVICE_BUS_RESET            (0x10)
#define  USB_SERVICE_SUSPEND              (0x11)
#define  USB_SERVICE_SOF                  (0x12)
#define  USB_SERVICE_RESUME               (0x13)
#define  USB_SERVICE_SLEEP                (0x14)
#define  USB_SERVICE_SPEED_DETECTION      (0x15)
#define  USB_SERVICE_ERROR                (0x16)
#define  USB_SERVICE_STALL                (0x17)
#define  USB_SERVICE_RESET_ENPOINT   (0x18)

#define  EHCI_PORTSCX_PAR_XCVR_SELECT              (0xC0000000)
#define  EHCI_PORTSCX_PTW_SELECT_MASK               (0x10000000)
#define  EHCI_PORTSCX_PTW_8BIT_SELECT               (0x00000000)
#define  EHCI_PORTSCX_UTMI_XCVR_SELECT              (0x00000000)

//typedef pointer usb_device_handle;

#ifdef __cplusplus
extern "C" {
#endif

extern uint_8 usb_device_init(uint_8, usb_device_handle _PTR_, uint_8);
extern uint_8 usb_device_recv_data(usb_device_handle, uint_8, uchar_ptr,
   uint_32);
extern uint_8 usb_device_send_data(usb_device_handle, uint_8, uchar_ptr,
   uint_32);
extern uint_8 usb_device_get_transfer_status(usb_device_handle, uint_8,
   uint_8);
extern void usb_device_get_transfer_details(usb_device_handle, uint_8,
   uint_8, uint_32_ptr _PTR_);
extern uint_8 usb_device_cancel_transfer(usb_device_handle, uint_8, uint_8);
extern uint_8 usb_device_get_status(usb_device_handle, uint_8, uint_16_ptr);
extern uint_8 usb_device_set_status(usb_device_handle, uint_8, uint_16);
extern uint_8 usb_device_register_service(usb_device_handle, uint_8,
   void(_CODE_PTR_ service)(pointer, boolean, uint_8, uint_8_ptr, uint_32, uint_8));
extern uint_8 usb_device_unregister_service(usb_device_handle, uint_8);
extern uint_8 usb_device_call_service(usb_device_handle, uint_8, boolean,
   boolean, uint_8_ptr, uint_32, uint_8);
extern void usb_device_shutdown(usb_device_handle);
extern void usb_device_set_address(usb_device_handle, uint_8);
extern void usb_device_read_setup_data(usb_device_handle, uint_8, uchar_ptr);
extern void usb_device_assert_resume(usb_device_handle);
extern uint_8 usb_device_init_endpoint(usb_device_handle, uint_8, uint_16,
   uint_8, uint_8, uint_8);
extern void usb_device_stall_endpoint(usb_device_handle,
   uint_8, uint_8);
extern void usb_device_unstall_endpoint(usb_device_handle,
   uint_8, uint_8);
extern uint_8 usb_device_deinit_endpoint(usb_device_handle, uint_8, uint_8);

#ifdef __cplusplus
}
#endif

#define __VUSBHS__ 1 

#endif
/* EOF */

