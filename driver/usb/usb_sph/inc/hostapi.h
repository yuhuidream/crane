#ifndef __hostapi_h__
#define __hostapi_h__ 1

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
***   This file contains the USB Host API speicifc data structures and function
***   definitions.
***
**************************************************************************
**END*********************************************************/
#include "usb_types.h"
#include "usb_dec.h"
#include "usbprv.h"
#include "host_common.h"
#include "host_ch9.h"
#include "host_dev_list.h"


#define  USB_MAX_PIPES                       (24)
#define  USB_MAX_INTERFACE                   (10)

#define  USB_INTERFACE_DESCRIPTOR_TYPE       (0x04)

/* Available service types */
#define  USB_SERVICE_HOST_RESUME             (0x01)
#define  USB_SERVICE_SYSTEM_ERROR            (0x02)
#define  USB_SERVICE_SPEED                   (0x03)
#define  USB_SERVICE_ATTACH                  (0x04)
#define  USB_SERVICE_DETACH                  (0x05)
#define  USB_SERVICE_STALL_PACKET            (0x06)
#define  USB_SERVICE_ENUMERATION_DONE        (0x07)
#define  USB_SERVICE_1MS_TIMER               (0x08)


//#define _HOST_DEBUG_
//#define __USB_OTG__



/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

extern uint_32 _usb_host_init(uint_8, uint_32, _usb_host_handle _PTR_);
extern uint_32 _usb_host_open_pipe(_usb_host_handle, PIPE_INIT_PARAM_STRUCT_PTR,  _usb_pipe_handle _PTR_);
extern uint_32 _usb_host_close_pipe(_usb_host_handle, _usb_pipe_handle);
extern void _usb_host_close_all_pipes(_usb_host_handle);
extern uint_32 _usb_host_register_service(_usb_host_handle, uint_8,
        void(_CODE_PTR_)(pointer, uint_32));
extern uint_32 _usb_host_unregister_service(_usb_host_handle, uint_8);
extern uint_32 _usb_host_call_service(_usb_host_handle, uint_8, uint_32);
extern uint_32 _usb_host_check_service(_usb_host_handle, uint_8, uint_32);
extern uint_32 _usb_host_get_transfer_status(_usb_pipe_handle,  uint_32);
extern uint_32 _usb_host_cancel_transfer(_usb_host_handle, _usb_pipe_handle,  uint_32);
extern uint_32 _usb_host_get_frame_number(_usb_host_handle);
extern uint_32 _usb_host_get_micro_frame_number(_usb_host_handle);
extern uint_32 _usb_host_recv_data(_usb_host_handle, _usb_pipe_handle, TR_INIT_PARAM_STRUCT_PTR);
extern uint_32 _usb_host_send_data(_usb_host_handle, _usb_pipe_handle, TR_INIT_PARAM_STRUCT_PTR);
extern uint_32 _usb_host_send_setup(_usb_host_handle, _usb_pipe_handle, TR_INIT_PARAM_STRUCT_PTR);
extern void _usb_host_bus_control(_usb_host_handle, uint_8);
extern void _usb_host_shutdown(_usb_host_handle);
extern void _usb_host_process_default_pipe(_usb_host_handle, uint_32, uint_32);
extern void usb_dev_list_attach_device(_usb_host_handle, uint_8, uint_8, uint_8);
extern void usb_dev_list_detach_device(_usb_host_handle, uint_8, uint_8);
extern void _usb_host_cntrl_transaction_done(_usb_host_handle, uint_32, uint_32);
extern void _usb_ch9_set_address(_usb_host_handle, _usb_pipe_handle, USB_SETUP_PTR, uchar_ptr);
extern void _usb_ch9_get_description(_usb_host_handle, _usb_pipe_handle, USB_SETUP_PTR, uchar_ptr);
extern void _usb_ch9_set_config(_usb_host_handle, _usb_pipe_handle, USB_SETUP_PTR, uchar_ptr);
extern void _usb_ch9_set_feature(_usb_host_handle, _usb_pipe_handle, USB_SETUP_PTR, uchar_ptr);
extern void _usb_ch9_clear_feature(_usb_host_handle, _usb_pipe_handle, USB_SETUP_PTR, uchar_ptr);
extern void _usb_ch9_vendor(_usb_host_handle, _usb_pipe_handle, USB_SETUP_PTR, uchar_ptr);
extern void _usb_ch9_class(_usb_host_handle, _usb_pipe_handle, USB_SETUP_PTR, uchar_ptr);
extern boolean _usb_host_class_get_device_info_and_init_class(_usb_host_handle,
   DEVICE_CLASS_INFO_STRUCT_PTR);
extern uint_8 _usb_host_class_install(_usb_host_handle, uint_8, uint_8, uint_8,
                                      char_ptr, pointer (_CODE_PTR_  init)(pointer, uint_32),
                                      void (_CODE_PTR_  deinit)(pointer), void (_CODE_PTR_  callback_init)(pointer, char_ptr),
                                      void (_CODE_PTR_  callback_remove)(pointer));
extern void  _usb_host_class_uninitialize_all_services(_usb_host_handle);

#ifdef __cplusplus
}
#endif
extern void _usb_ehci_bus_suspend
(
    /* [IN] the USB Host state structure */
    _usb_host_handle              handle
);
extern void _usb_ehci_bus_resume
(
    /* [IN] the USB Host state structure */
    _usb_host_handle              handle
);
extern void _usb_ehci_bus_reset
(
    /* [IN] the USB Host state structure */
    _usb_host_handle              handle
);


#endif

/* EOF */
