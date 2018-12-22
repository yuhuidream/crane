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
***   This file contains the USB Host API specific function to shutdown
***   the host
***
**************************************************************************
**END*********************************************************/
#include "hostapi.h"
#include "usbprv_host.h"

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_shutdown
*  Returned Value : USB_OK or error code
*  Comments       :
*        Shutdown an initialized USB Host
*
*END*-----------------------------------------------------------------*/
extern    USB_HOST_STATE_STRUCT_PTR   usb_host_state_struct_ptr;
void    usb_host_close(void)
{

    _usb_host_handle         handle = (USB_HOST_STATE_STRUCT_PTR)usb_host_state_struct_ptr;
    _usb_host_shutdown(handle);

}



void _usb_host_shutdown
(
    /* [IN] the USB_host handle */
    _usb_host_handle         handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR   usb_host_ptr;
    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_shutdown\r\n");
#endif


#ifdef __USB_OS_MQX__
    /* De-initialize and disconnect the host hardware from the bus */
    ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
    HOST_SHUTDOWN(handle);
#else
#ifdef __VUSB32__
    _usb_hci_vusb11_shutdown(handle);
#else
    _usb_hci_vusb20_shutdown(handle);
#endif
#endif
    /* Free all Pipe Descriptors */
    USB_memfree((pointer)usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR);

    /* Free the USB state structure */
    USB_memfree(handle);
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_shutdown SUCCESSFUL\r\n");
#endif


} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_bus_control
*  Returned Value : None
*  Comments       :
* _usb_host_bus_control controls the bus operations such as asserting and
* deasserting the bus reset, asserting and deasserting resume,
* suspending and resuming the SOF generation
*
*END*-----------------------------------------------------------------*/

void _usb_host_bus_control
(
    /* [IN] the USB Host state structure */
    _usb_host_handle           handle,

    /* The operation to be performed on the bus */
    uint_8                     bcontrol
)
{
    /* Body */
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_bus_control\r\n");
#endif

#ifdef __USB_OS_MQX__
    USB_HOST_STATE_STRUCT_PTR         usb_host_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
    HOST_BUS_CONTROL(handle, bcontrol);
#else
#ifdef __VUSB32__
    _usb_hci_vusb11_bus_control(handle, bcontrol);
#else
    _usb_hci_vusb20_bus_control(handle, bcontrol);
#endif
#endif
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_bus_control SUCCESSFUL\r\n");
#endif

} /* Endbody */

/* EOF */




