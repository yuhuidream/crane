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
***   This file contains the USB Host API specific functions to receive data.
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
*  Function Name  : _usb_host_recv_data
*  Returned Value : error or status of the transfer
*  Comments       :
* The Receive Data routine is non-blocking routine that causes a buffer
* to be made available for data recieved from the
* USB host. It takes the buffer and passes it down to lower
level hardware driver.
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_recv_data
(
    /* [IN] the USB Host state structure */
    _usb_host_handle           handle,

    /* [IN] the pipe handle */
    _usb_pipe_handle           pipe_handle,

    /* [IN] transfer parameters */
    TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr
)
{
    /* Body */
    PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
    USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
    USB_STATUS status;
    PIPE_TR_STRUCT_PTR   pipe_tr_ptr;
    uint_32  return_code;


#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_recv_data");
#endif

    /*****************************************************************
    CODE FOR ARC FULL SPEED CORE
    *****************************************************************/

#ifdef __VUSB32__
    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

    /* Check if the pipe id is valid */
    if (pipe_descr_ptr->PIPE_ID > USB_MAX_PIPES)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_recv_data inalid pipe handle");
#endif

        return USBERR_INVALID_PIPE_HANDLE;
    } /* Endif */

    USB_lock();

    /* Check if a previously queued packet is still pending */
    if (pipe_descr_ptr->STATUS != USB_STATUS_IDLE)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_recv_data transfer in progress");
#endif
        return USB_STATUS_TRANSFER_IN_PROGRESS;
    }
    else
    {
        USB_unlock();
        /* Initialize the pipe desciptor with the new receive transfer request
        ** for this pipe
        */
        pipe_descr_ptr->TR = *tr_params_ptr;
        pipe_descr_ptr->SOFAR = 0;
        pipe_descr_ptr->RX_PTR = tr_params_ptr->RX_BUFFER;

        if ((pipe_descr_ptr->PIPETYPE == USB_ISOCHRONOUS_PIPE) &&
                (pipe_descr_ptr->TODO1 > pipe_descr_ptr->MAX_PACKET_SIZE))
        {
            /* For isochronous pipe, we are accepting a maximum of MAX_PACKET_SIZE
            ** transfer
            */
            pipe_descr_ptr->TODO1 = pipe_descr_ptr->MAX_PACKET_SIZE;
        }
        else
        {
            pipe_descr_ptr->TODO1 = tr_params_ptr->RX_LENGTH;
        } /* Endif */

#ifdef _DATA_CACHE_
        /*Must Flush and Invalidate the buffer before sending
        /receiving the data in it */
        USB_dcache_invalidate_mlines((uint_32)pipe_descr_ptr->RX_PTR,
                                     tr_params_ptr->RX_LENGTH);

#endif


        USB_lock();

        /* successful transfer initiation status. */
        pipe_descr_ptr->STATUS = USB_STATUS_TRANSFER_QUEUED;

#ifdef __USB_OS_MQX__

        /* Call the low-level routine to receive data */
        ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
        HOST_RECV(handle, pipe_descr_ptr);
#else
        _usb_hci_vusb11_recv_data(handle, pipe_descr_ptr);
#endif

        pipe_descr_ptr->PACKETPENDING = TRUE;

        /* full speed driver assumes that transfer queue is succesfull. This should change
        in future releases */

        status = USB_STATUS_TRANSFER_QUEUED;

        USB_unlock();
    } /* Endif */


    /*****************************************************************
    CODE FOR ARC HS SPEED CORE
    *****************************************************************/

#else


    USB_lock();

    return_code = _usb_host_set_up_TR(pipe_handle, tr_params_ptr, &pipe_tr_ptr);

    if (return_code != USB_STATUS_TRANSFER_QUEUED)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_recv_data transfer queue failed");
#endif
        return return_code;
    } /* Endif */

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

#ifdef _DATA_CACHE_
    /*Must Flush and Invalidate the buffer before sending
    /receiving the data in it */
    USB_dcache_invalidate_mlines((uint_32)pipe_tr_ptr->RX_BUFFER,
                                 pipe_tr_ptr->RX_LENGTH);

#endif

    /* We have obtained the current TR on the Pipe's TR list
    ** from _usb_host_set_up_TR
    */
#ifdef __USB_OS_MQX__

    /* Call the low-level routine to receive data */
    status = ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
             HOST_RECV(handle, pipe_descr_ptr, pipe_tr_ptr);
#else
    status = _usb_hci_vusb20_recv_data(handle, pipe_descr_ptr, pipe_tr_ptr);
#endif

    USB_unlock();

#endif
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_recv_data SUCCESSFUL");
#endif

    if (status == USB_OK)
    {
        return USB_STATUS_TRANSFER_QUEUED;
    }
    else
    {
        return status;
    }


} /* Endbody */

/* EOF */




