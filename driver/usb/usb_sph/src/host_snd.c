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
***   This file contains the USB Host API specific functions to send data.
***
**************************************************************************
**END*********************************************************/
#include "hostapi.h"
#include "usbprv_host.h"

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif

#ifndef __VUSB32__
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_frame_number
*  Returned Value : frame number
*  Comments       :
* This function can be used to inquire the current frame number.
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_get_frame_number
(
    /* [IN] the USB Host state structure */
    _usb_host_handle  handle
)
{
    /* Body */
    //   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
    //  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_frame_number");
#endif

#ifdef __USB_OS_MQX__
    /* Call the low-level send routine */
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_frame_number OS mode SUCCESSFUL");
#endif

    return ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
           HOST_EHCI_GET_FRAME_NUM(handle);
#else
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_frame_number non os SUCCESSFUL");
#endif
    return (_usb_ehci_get_frame_number(handle));
#endif
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_micro_frame_number
*  Returned Value : frame number
*  Comments       :
* This function can be used to inquire the current micro frame number.
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_get_micro_frame_number
(
    /* [IN] the USB Host state structure */
    _usb_host_handle  handle
)
{
    /* Body */
    //   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
    //  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_micro_frame_number");
#endif

#ifdef __USB_OS_MQX__

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_micro_frame_number OS mode SUCCESSFUL");
#endif

    /* Call the low-level send routine */
    return ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
           HOST_EHCI_GET_MICRO_FRAME_NUM(handle);
#else
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_micro_frame_number non OS mode SUCCESSFUL");
#endif
    return (_usb_ehci_get_micro_frame_number(handle));
#endif
} /* EndBody */
#endif

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_data
*  Returned Value : error or status of the transfer
*  Comments       :
* The Send Data routine is non-blocking routine that causes a block of data
* to be made available for transmission to the USB host.
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_send_data
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
    //   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
    uint_32  return_code;

    PIPE_TR_STRUCT_PTR         pipe_tr_ptr;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_send_data\r\n");
#endif

#ifdef __VUSB32__
    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

    /* Check if the pipe id is valid */
    if (pipe_descr_ptr->PIPE_ID > USB_MAX_PIPES)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_send_data inalid pipe handle\r\n");
#endif
        return USBERR_INVALID_PIPE_HANDLE;
    } /* Endif */

    USB_lock();

    /* Check if a previously queued transfer is still in progress */
    if (pipe_descr_ptr->STATUS != USB_STATUS_IDLE)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_send_data transfer in progress\r\n");
#endif
        return USB_STATUS_TRANSFER_IN_PROGRESS;
    }
    else
    {
        USB_unlock();
        /* Initialize the pipe descriptor with the new send request */
        pipe_descr_ptr->TR = *tr_params_ptr;
        pipe_descr_ptr->SOFAR = 0;
        pipe_descr_ptr->TX1_PTR = tr_params_ptr->TX_BUFFER;

        if ((pipe_descr_ptr->PIPETYPE == USB_ISOCHRONOUS_PIPE) &&
                (pipe_descr_ptr->TODO1 > pipe_descr_ptr->MAX_PACKET_SIZE))
        {
            /* For isochronous pipes, we transfer a mzximum of MAX_PACKET_SIZE
            ** packet
            */
            pipe_descr_ptr->TODO1 = pipe_descr_ptr->MAX_PACKET_SIZE;
        }
        else
        {
            /* For all others max of 64KB */
            pipe_descr_ptr->TODO1 = tr_params_ptr->TX_LENGTH;
        } /* Endif */

#ifdef _DATA_CACHE_
        /*Must Flush and Invalidate the buffer before sending
        /receiving the data in it */
        USB_dcache_flush_mlines(
            (uint_32)pipe_descr_ptr->TX1_PTR,
            tr_params_ptr->TX_LENGTH
        );
#endif


        USB_lock();

        /* return successful transfer initiation status */
        pipe_descr_ptr->STATUS = USB_STATUS_TRANSFER_QUEUED;

#ifdef __USB_OS_MQX__

        /* Call the low-level send routine */
        ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
        HOST_SEND(handle, pipe_descr_ptr);
#else
        _usb_hci_vusb11_send_data(handle, pipe_descr_ptr);
#endif

        /* Indicate that a packet is pending */
        pipe_descr_ptr->PACKETPENDING = TRUE;

        /*full speed driver assumes that transfer is queued. It should be
        changed in future changes */

        return_code =  USB_STATUS_TRANSFER_QUEUED;

        USB_unlock();
    } /* Endif */

#else


    USB_lock();
    return_code = _usb_host_set_up_TR(pipe_handle, tr_params_ptr, &pipe_tr_ptr);
    if (return_code != USB_STATUS_TRANSFER_QUEUED)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_send_data failed to queue transfer\r\n");
#endif

        return return_code;
    } /* Endif */

    //   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

#ifdef _DATA_CACHE_
    /*Must Flush and Invalidate the buffer before sending
    /receiving the data in it */
    USB_dcache_flush_mlines(
        (uint_32)pipe_tr_ptr->TX_BUFFER,
        pipe_tr_ptr->TX_LENGTH
    );
#endif


    /* We have obtained the current TR on the Pipe's TR list
    ** from _usb_host_set_up_TR
    */
#ifdef __USB_OS_MQX__
    /* Call the low-level send routine */
    return_code = ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
                  HOST_SEND(handle, pipe_descr_ptr, pipe_tr_ptr);
#else
    return_code = _usb_hci_vusb20_send_data(handle, pipe_descr_ptr, pipe_tr_ptr);
#endif

    USB_unlock();

#endif

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_send_data SUCCESSFUL\r\n");
#endif

    if (return_code == USB_OK)
    {
        return USB_STATUS_TRANSFER_QUEUED;
    }
    else
    {
        return  return_code;
    }

} /* Endbody */





/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_cancel_transfer
*  Returned Value : error or status of the transfer before cancellation
*  Comments       :
* _usb_host_cancel_transfer is a non-blocking routine that causes a transfer to
* be terminated.
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_cancel_transfer
(
    /* [IN] the USB Host state structure */
    _usb_host_handle     handle,

    /* [IN] the pipe handle */
    _usb_pipe_handle     pipe_handle,

    /* [IN] the transfer numnber for this pipe */
    uint_32              tr_number
)
{
    /* Body */
    uint_32  bRet = 0;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_cancel_transfer");
#endif
    handle = handle;
    pipe_handle = pipe_handle;
    tr_number = tr_number;

#ifdef __VUSBHS__

    PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
    PIPE_TR_STRUCT_PTR         pipe_tr_ptr;
    USB_HOST_STATE_STRUCT_PTR usb_host_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

    USB_lock();

    /* Check if valid pipe id */
    if (pipe_descr_ptr->PIPE_ID > USB_MAX_PIPES)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_cancel_transfer, invalid pipe");
#endif
        return USBERR_INVALID_PIPE_HANDLE;
    } /* Endif */

    pipe_tr_ptr = pipe_descr_ptr->tr_list_ptr;
    while (pipe_tr_ptr->TR_INDEX != tr_number)
    {
        /* If unused TR found, or end of list, exit with bad status */
        if ((pipe_tr_ptr->TR_INDEX == 0) ||
                (pipe_tr_ptr->next == pipe_descr_ptr->tr_list_ptr))
        {
            USB_unlock();
#ifdef _HOST_DEBUG_
            bbu_printf("_usb_host_cancel_transfer, invalid pipe");
#endif
            return USBERR_INVALID_PIPE_HANDLE;
        } /* EndIf */
        pipe_tr_ptr = pipe_tr_ptr->next;
    } /* EndWhile */

    /* Get the current status */
    bRet = pipe_tr_ptr->status;

#ifdef __USB_OS_MQX__
    /* Cancel the transfer at low-level */
    ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
    HOST_CANCEL(handle, pipe_descr_ptr, pipe_tr_ptr);
#else
    _usb_hci_vusb20_cancel_transfer(handle, pipe_descr_ptr, pipe_tr_ptr);
#endif

    /* Indicate that the pipe is idle */
    pipe_tr_ptr->status = USB_STATUS_IDLE;

    if (pipe_tr_ptr->CALLBACK != NULL)
    {
        pipe_tr_ptr->CALLBACK((pointer)pipe_descr_ptr,
                              pipe_tr_ptr->CALLBACK_PARAM,
                              NULL,
                              0,
                              USBERR_SHUTDOWN);
    } /* Endif */

    USB_unlock();
#elif defined __VUSB32__
    PIPE_DESCRIPTOR_STRUCT_PTR             pipe_descr_ptr;
    volatile USB_HOST_STATE_STRUCT _PTR_   usb_host_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

    /* Get the current status */
    bRet = pipe_descr_ptr->STATUS;

#ifdef __USB_OS_MQX__
    /* Cancel the transfer at low-level */
    ((volatile USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
    HOST_CANCEL(handle, pipe_descr_ptr);
#else
    _usb_hci_vusb11_cancel_transfer(handle, pipe_descr_ptr);
#endif

    /* Indicate that a transfer is not pending */
    pipe_descr_ptr->PACKETPENDING = FALSE;

    /* Indicate that the pipe is idle */
    pipe_descr_ptr->STATUS = USB_STATUS_IDLE;

    /* Update the head of the queue for a particular type */
    _usb_host_update_current_head(handle, pipe_descr_ptr->PIPETYPE);
#endif

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_cancel_transfer,SUCCESSFUL");
#endif

    /* Return the status prior to the transfer cancellation */
    return bRet;

} /* Endbody */

/* EOF */




