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
***   This file contains  the USB class driver mapping table
***
**************************************************************************
**END*********************************************************/


#include "hostapi.h"
#include "host_ch9.h"
#include "host_common.h"

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif

#define USBCLASS_INC_MASS

#ifdef USBCLASS_INC_MASS
#include "usb_mass_bo.h"
#endif

#undef  USBCLASS_INC_PRINTER
#ifdef USBCLASS_INC_PRINTER
#include "usb_printer.h"
#endif

#undef  USBCLASS_INC_HID
#ifdef USBCLASS_INC_HID
#include "usb_hid.h"
#endif

#undef  USBCLASS_INC_CDC
#ifdef USBCLASS_INC_CDC
#include "usb_cdc.h"
#endif

#undef  USBCLASS_INC_AUDIO
#ifdef USBCLASS_INC_AUDIO
#include "usb_audio.h"
#endif

CLASS_MAP class_interface_map[] =
{
#ifdef USBCLASS_INC_MASS
    {
        usb_class_mass_init,
        sizeof(USB_MASS_CLASS_INTF_STRUCT),
        USB_CLASS_MASS_STORAGE,
        USB_SUBCLASS_MASS_UFI,
        USB_PROTOCOL_MASS_BULK,
        0xFF, 0x00, 0x00
    },
#endif
#ifdef USBCLASS_INC_PRINTER
    {
        usb_printer_init,
        sizeof(PRINTER_INTERFACE_STRUCT),
        USB_CLASS_PRINTER,
        USB_SUBCLASS_PRINTER,
        USB_PROTOCOL_PRT_BIDIR,
        0xFF, 0xFF, 0xFF
    },
#endif
#ifdef USBCLASS_INC_HID
    {
        usb_class_hid_init,
        sizeof(USB_HID_CLASS_INTF_STRUCT),
        USB_CLASS_HID,
        0,
        0,
        0xFF, 0x00, 0x00
    },
#endif
#ifdef USBCLASS_INC_CDC
    {
        usb_ecm_init,
        sizeof(CDC_INTERFACE_STRUCT),
        USB_CLASS_COMMUNICATION,
        USB_SUBCLASS_COM_ETHERNET,
        0,
        0xFF, 0xFF, 0xFF
    },
    {
        usb_acm_init,
        sizeof(CDC_INTERFACE_STRUCT),
        USB_CLASS_COMMUNICATION,
        USB_SUBCLASS_COM_ABSTRACT,
        USB_PROTOCOL_COM_V25,
        0xFF, 0xFF, 0xFF
    },
    {
        usb_acm_init,
        sizeof(CDC_INTERFACE_STRUCT),
        USB_CLASS_COMMUNICATION,
        USB_SUBCLASS_COM_ABSTRACT,
        0xFF,                      /* Vendor Specific Protocol */
        0xFF, 0xFF, 0xFF
    },
    {
        usb_data_init,
        sizeof(CDC_INTERFACE_STRUCT),
        USB_CLASS_DATA,
        0,
        0,
        0xFF, 0x00, 0x00
    },
#endif
#ifdef USBCLASS_INC_AUDIO
    {
        usb_audio_init,
        sizeof(AUDIO_INTERFACE_STRUCT),
        USB_CLASS_AUDIO,
        USB_SUBCLASS_AUD_CONTROL,
        0,
        0xFF, 0x00, 0x00
    },
    {
        usb_audio_init,
        sizeof(AUDIO_INTERFACE_STRUCT),
        USB_CLASS_AUDIO,
        USB_SUBCLASS_AUD_STREAMING,
        0,
        0xFF, 0x00, 0x00
    },
#endif
    {
        NULL,
        0,
        0, 0, 0,
        0, 0, 0
    }
};

/* EOF */
