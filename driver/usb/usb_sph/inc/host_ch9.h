#ifndef __host_ch9_h__
#define __host_ch9_h__
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
***   This file contains definitions for standard device requests,
***   i.e. USB specification Chapter 9 requests.
***
**************************************************************************
**END*********************************************************/

/*----------------------------------------------------------------**
** Chapter 9.4 Standard Device Requests -- all devices            **
** See Table 9-3 p. 250 of USB 2.0 spec for combinations          **
**   of request type bitfields with requests, WVALUE, WINDEX etc. **
**----------------------------------------------------------------*/
#define  REQ_TYPE_DEVICE    0x00
#define  REQ_TYPE_INTERFACE 0x01
#define  REQ_TYPE_ENDPOINT  0x02

/* Combine one of the 3 above with one of the following 2 */
#define  REQ_TYPE_IN        0x80
#define  REQ_TYPE_OUT       0x00

/* Also for class requests set the following bit */
#define  REQ_TYPE_CLASS     0x20

/* Standard USB requests, see Chapter 9 */
#define  REQ_GET_STATUS          0
#define  REQ_CLEAR_FEATURE       1
#define  REQ_SET_FEATURE         3
#define  REQ_SET_ADDRESS         5
#define  REQ_GET_DESCRIPTOR      6
#define  REQ_SET_DESCRIPTOR      7
#define  REQ_GET_CONFIGURATION   8
#define  REQ_SET_CONFIGURATION   9
#define  REQ_GET_INTERFACE       10
#define  REQ_SET_INTERFACE       11
#define  REQ_SYNCH_FRAME         12

#define  DESCTYPE_DEVICE         0x1
#define  DESCTYPE_CONFIG         0x2
#define  DESCTYPE_STRING         0x3
#define  DESCTYPE_INTERFACE      0x4
#define  DESCTYPE_ENDPOINT       0x5
#define  DESCTYPE_QUALIFIER      0x6
#define  DESCTYPE_OTHER_SPEED    0x7
#define  DESCTYPE_INTF_POWER     0x8
#define  DESCTYPE_OTG            0x9

/*******************************************************************
**
** Values specific to CLEAR FEATURE commands (must go to common.h later)
*/

#define  ENDPOINT_HALT        0
#define  DEVICE_REMOTE_WAKEUP 1
#define  TEST_MODE            2

/* States of device instances on the device list */

/* initial device state */
#define  DEVSTATE_INITIAL        0x00

/* device descriptor [0..7]*/
#define  DEVSTATE_DEVDESC8       0x01

/* address set */
#define  DEVSTATE_ADDR_SET       0x02

/* full device descriptor */
#define  DEVSTATE_DEV_DESC       0x03

/* config descriptor [0..7] */
#define  DEVSTATE_GET_CFG9       0x04

/* config set */
#define  DEVSTATE_SET_CFG        0x05

/* full config desc. read in */
#define  DEVSTATE_CFG_READ       0x06

/* application callbacks */
#define  DEVSTATE_APP_CALL       0x07

/* Select interface done */
#define  DEVSTATE_SET_INTF       0x08

#define  DEVSTATE_ENUM_OK        0x09

#define  DEVSTATE_CHK_OTG        0x0A

/* Event codes for attach/detach etc. callback */
#define  USB_ATTACH_EVENT        1   /* device attach */
#define  USB_DETACH_EVENT        2   /* device detach */
#define  USB_CONFIG_EVENT        3   /* device reconfigured */
#define  USB_INTF_EVENT          4   /* device interface selected */

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS _usb_host_ch9_clear_feature(_usb_device_instance_handle,
                                       uint_8, uint_8, uint_16);
USB_STATUS _usb_host_ch9_get_configuration(_usb_device_instance_handle,
        uchar_ptr);
USB_STATUS _usb_host_ch9_get_descriptor(_usb_device_instance_handle,
                                        uint_16, uint_16, uint_16, uchar_ptr);
USB_STATUS _usb_host_ch9_get_interface(_usb_device_instance_handle,
                                       uint_8, uchar_ptr);
USB_STATUS _usb_host_ch9_get_status(_usb_device_instance_handle,
                                    uint_8, uint_8, uchar_ptr);
USB_STATUS _usb_host_ch9_set_address(_usb_device_instance_handle);
USB_STATUS _usb_host_ch9_set_configuration(_usb_device_instance_handle,
        uint_16);
USB_STATUS _usb_host_ch9_set_descriptor(_usb_device_instance_handle,
                                        uint_16, uint_16, uint_16, uchar_ptr);
USB_STATUS _usb_host_ch9_set_feature(_usb_device_instance_handle,
                                     uint_8, uint_8, uint_16);
USB_STATUS _usb_host_ch9_set_interface(_usb_device_instance_handle,
                                       uint_8, uint_8);
USB_STATUS _usb_host_ch9_synch_frame(_usb_device_instance_handle,
                                     uint_8, uchar_ptr);
USB_STATUS  _usb_host_register_ch9_callback(_usb_device_instance_handle,
        tr_callback, pointer);
USB_STATUS _usb_hostdev_cntrl_request(_usb_device_instance_handle,
                                      USB_SETUP_PTR, uchar_ptr, tr_callback, pointer);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
