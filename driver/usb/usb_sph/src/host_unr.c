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
***   This file contains the USB Host API specific functions to
***   unregister a service.
***
**************************************************************************
**END*********************************************************/
#include "hostapi.h"
#include "usbprv_host.h"

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif

/*FUNCTION*------------------------------------------------------------ ----
*
* Function Name  : _usb_host_unregister_service
* Returned Value : USB_OK or error code
* Comments       :
*     Unregisters a callback routine for a specified event or endpoint.
*
*END*--------------------------------------------------------------------*/
uint_32 _usb_host_unregister_service
(
    /* [IN] Handle to the USB device */
    _usb_host_handle           handle,

    /* [IN] type of event or endpoint number to service */
    uint_8                     type
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
    USB_SERVICE_STRUCT_PTR              service_ptr;
    USB_SERVICE_STRUCT_PTR _PTR_        search_ptr;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_unregister_service\r\n");
#endif

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    /* Needs mutual exclusion */
    USB_lock();

    /* Search for an existing entry for type */
    for (search_ptr = &usb_host_ptr->SERVICE_HEAD_PTR;
            *search_ptr;
            search_ptr = &(*search_ptr)->NEXT)
    {
        if ((*search_ptr)->TYPE == type)
        {
            /* Found an existing entry - delete it */
            break;
        } /* Endif */
    } /* Endfor */

    /* No existing entry found */
    if (!*search_ptr)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_unregister_service no service found\r\n");
#endif
        return USBERR_CLOSED_SERVICE;
    } /* Endif */

    service_ptr = *search_ptr;
    *search_ptr = service_ptr->NEXT;

    USB_unlock();

    USB_memfree((pointer)service_ptr);

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_unregister_service SUCCESSFUL\r\n");
#endif
    return USB_OK;

} /* EndBody */

/* EOF */




