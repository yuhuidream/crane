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
***   This file contains the USB Host API specific functions to close pipe(s).
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
*  Function Name  : _usb_host_close_pipe
*  Returned Value : None
*  Comments       :
*        _usb_host_close_pipe routine removes the pipe from the open pipe list
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_close_pipe
(
    /* [IN] the USB Host state structure */
    _usb_host_handle     handle,

    /* [IN] the pipe (handle) to close */
    _usb_pipe_handle     pipe_handle
)
{
    /* Body */
    //   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
    PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_close_pipe\r\n");
#endif

    //  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

    if (pipe_descr_ptr->PIPE_ID > USB_MAX_PIPES)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_close_pipe invalid pipe\r\n");
#endif
        return USBERR_INVALID_PIPE_HANDLE;
    } /* Endif */

    USB_lock();

#ifndef __VUSB32__
#ifdef __USB_OS_MQX__
    /* Call the low-level routine to free the controller specific resources */
    ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)
     usb_host_ptr->CALLBACK_STRUCT_PTR)->\
    HOST_FREE_CONTROLLER_RESOURCE(handle, pipe_descr_ptr);
#else
    /* Call the low-level routine to free the controller specific resources */
    _usb_ehci_free_resources(handle, pipe_descr_ptr);
#endif
#endif

    /* de-initialise the pipe descriptor */
    memset((pointer)pipe_descr_ptr, 0, sizeof(PIPE_DESCRIPTOR_STRUCT));

    USB_unlock();

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_close_pipe SUCCESSFUL\r\n");
#endif

    return USB_OK;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_close_all_pipes
*  Returned Value : None
*  Comments       :
*  _usb_host_close_all_pipes routine removes the pipe from the open pipe list
*
*END*-----------------------------------------------------------------*/
void  _usb_host_close_all_pipes
(
    /* [IN] the USB Host state structure */
    _usb_host_handle  handle
)
{
    /* Body */
    int_16 i;
    USB_HOST_STATE_STRUCT_PTR usb_host_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_close_all_pipes\r\n");
#endif

    USB_lock();

#ifdef __VUSB32__
    for (i = 0; i < usb_host_ptr->MAX_PIPES; i++)
    {
        if (!(usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR[i].OPEN))
        {
            break;
        }
        else
        {
#else
    for (i = 0; i < USB_MAX_PIPES; i++)
    {
        if (!(usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR[i].OPEN))
        {
            break;
        }
        else
        {
#ifdef __USB_OS_MQX__
            /* Call the low-level routine to free the controller specific
            ** resources for this pipe
            */
            ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)\
             usb_host_ptr->CALLBACK_STRUCT_PTR)->\
            HOST_FREE_CONTROLLER_RESOURCE(handle,
                                          &usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR[i]);
#else
            /* Allocate bandwidth if periodic pipe */
            _usb_ehci_free_resources(handle,
                                     &usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR[i]);
#endif
#endif
            /* de-initialise the pipe descriptor */
            memset((pointer)(&usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR[i]), 0,
                   sizeof(PIPE_DESCRIPTOR_STRUCT));
        } /* Endif */
    } /* Endfor */

#ifdef __VUSB32__
    /* Initialize the current pipe id to -1 indicating that there are no pipes
    ** queued
    */
    usb_host_ptr->CURRENT_PIPE_ID = -1;
#endif

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_close_all_pipes SUCCESSFUL\r\n");
#endif

} /* Endbody */

/* EOF */




