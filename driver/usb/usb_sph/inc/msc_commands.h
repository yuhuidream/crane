#ifndef __msc_commands_h__
#define __msc_commands_h__
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
***   This file contains mass storage class application types and definitions.
***
**************************************************************************
**END*********************************************************/

#include "host_dev_list.h"
#include "host_common.h"

/* Application-specific definitions */
#define  MAX_PENDING_TRANSACTIONS      16
#define  MAX_FRAME_SIZE                1024
#define  HOST_CONTROLLER_NUMBER        0


#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)

#define  USB_OTG_PORT                      0
#define USB_HSIC1_PORT                   1
#define USB_ICUSB_PORT                   2

/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/


//typedef struct device_struct
//{
//   uint_32                          dev_state;  /* Attach/detach state */

//  PIPE_BUNDLE_STRUCT_PTR           pbs;        /* Device & pipes */
//  _usb_device_instance_handle      dev_handle;
//  _usb_interface_descriptor_handle intf_handle;
//  CLASS_CALL_STRUCT                class_intf; /* Class-specific info */

//} DEVICE_STRUCT,  _PTR_ DEVICE_STRUCT_PTR;
//#if 0
typedef struct device_struct
{
    uint_32                          dev_state;  /* Attach/detach state */
    PIPE_BUNDLE_STRUCT_PTR           pbs;        /* Device & pipes */
    _usb_device_instance_handle      dev_handle;
    _usb_interface_descriptor_handle intf_handle;
    CLASS_CALL_STRUCT                class_intf; /* Class-specific info */

} DEVICE_STRUCT,  _PTR_ DEVICE_STRUCT_PTR;
//#endif
/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void    usb_host_mass_device_event
(
    _usb_device_instance_handle,
    _usb_interface_descriptor_handle,
    uint_32
);

void    usb_host_mass_ctrl_callback
(
    _usb_pipe_handle,
    pointer,
    uchar_ptr,
    uint_32,
    uint_32
);


void callback_bulk_pipe (USB_STATUS status,
                         pointer p1,
                         pointer  p2,
                         uint_32 buffer_length
                        );

void usb_host_mass_test_storage(      uint_8 host_device_num
                               );

extern  uint_8 hsic_phy_end_bit;



#ifdef __cplusplus
}
#endif

typedef pointer _usb_host_handle;                  /* host state struct   */

#endif

/* EOF */





