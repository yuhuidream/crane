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
***   This file contains the USB Host API specific main functions.
***
**************************************************************************
**END*********************************************************/
#include "hostapi.h"
#include "usbprv_host.h"
#include "usb_types.h"

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif
#include "usb_hsic.h"



//extern void * USB_memalloc(uint_32 n);
/* this should be global for access by the OTG source code */
USB_HOST_STATE_STRUCT_PTR usb_host_state_struct_ptr;

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_init
*  Returned Value : error or USB_OK
*  Comments       :
*        Initializes the USB hardware and installs the USB
*  interrupt handler
*END*-----------------------------------------------------------------*/

uint_32 _usb_host_init
(
    /* [IN] device number */
    uint_8                  device_number,

    /* [IN] maximum number of Pipes */
    uint_32                 frame_list_size,

    /* [OUT] the USB host handle */
    _usb_host_handle _PTR_  handle
)
{
    /* Body */
    uint_8 error;
#ifdef __USB_OS_MQX__
    USB_CALLBACK_FUNCTIONS_STRUCT_PTR call_back_table_ptr;
#endif

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_init\r\n");
#endif

    /* Initialize the USB interface. */
    usb_host_state_struct_ptr = (USB_HOST_STATE_STRUCT_PTR)
                                USB_memalloc(sizeof(USB_HOST_STATE_STRUCT));

    if (usb_host_state_struct_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_init host state malloc failure\r\n");
#endif
        return USBERR_ALLOC;
    } /* Endif */

    memset((void *)usb_host_state_struct_ptr, 0, sizeof(USB_HOST_STATE_STRUCT));

    /* No devices attached yet */
    /*
    ** We should maintain device lists per controller handle
    */
    usb_host_state_struct_ptr->DEVICE_LIST_PTR = NULL;

    usb_host_state_struct_ptr->DEV_NUM = device_number;
    usb_host_state_struct_ptr->FRAME_LIST_SIZE = frame_list_size;

    /* Allocate the USB Host Pipe Descriptors */
    usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR =
        (PIPE_DESCRIPTOR_STRUCT_PTR)USB_memalloc (((sizeof(PIPE_DESCRIPTOR_STRUCT)) *  USB_MAX_PIPES));

    //   if (usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR == NULL) {
    if ( !usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR )
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_init pipe allocation malloc failure\n");
#endif
        return USBERR_ALLOC;
    } /* Endif */


    memset((void *)(usb_host_state_struct_ptr->PIPE_DESCRIPTOR_BASE_PTR), 0,
           ((sizeof(PIPE_DESCRIPTOR_STRUCT)) *  USB_MAX_PIPES));

    USB_lock();

#ifdef __VUSB32__
    /* Initialize the pipe queue heads and tails to -1 - indicating that
    ** the queue is empty
    */
    usb_host_state_struct_ptr->CURRENT_ISO_HEAD = -1;
    usb_host_state_struct_ptr->CURRENT_ISO_TAIL = -1;
    usb_host_state_struct_ptr->CURRENT_INTR_HEAD = -1;
    usb_host_state_struct_ptr->CURRENT_INTR_TAIL = -1;
    usb_host_state_struct_ptr->CURRENT_CTRL_HEAD = -1;
    usb_host_state_struct_ptr->CURRENT_CTRL_TAIL = -1;
    usb_host_state_struct_ptr->CURRENT_BULK_HEAD = -1;
    usb_host_state_struct_ptr->CURRENT_BULK_TAIL = -1;
#endif

#ifdef __USB_OS_MQX__
    call_back_table_ptr = (USB_CALLBACK_FUNCTIONS_STRUCT_PTR)
                          _mqx_get_io_component_handle(IO_USB_COMPONENT);


    usb_host_state_struct_ptr->CALLBACK_STRUCT_PTR =
        (call_back_table_ptr + device_number);

    if (!usb_host_state_struct_ptr->CALLBACK_STRUCT_PTR)
    {
        return USBERR_DRIVER_NOT_INSTALLED;
    } /* Endif */

    error =
        ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_state_struct_ptr->\
         CALLBACK_STRUCT_PTR)->HOST_INIT(usb_host_state_struct_ptr);
#else
#ifdef __VUSB32__
    error = _usb_hci_vusb11_init((pointer)usb_host_state_struct_ptr);
#else
    if ( usb_host_state_struct_ptr->DEV_NUM == 0)
        //error = _usb_hci_vusb20_init((pointer)usb_host_state_struct_ptr);
        ;
    else if ( usb_host_state_struct_ptr->DEV_NUM == 1)
    {
        //error = _usb_hsichci_vusb20_init((pointer)usb_host_state_struct_ptr);
        ;

    } else if (usb_host_state_struct_ptr->DEV_NUM == 2)
    {
        error = _usb_icusb_hci_vusb20_init((pointer)usb_host_state_struct_ptr);


    }
#endif
#endif

    USB_unlock();

    if (error)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_init returning error status\r\n");
#endif
        return error;
    } /* Endif */

    *handle = (pointer)usb_host_state_struct_ptr;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_init SUCCESSFULL\r\n");
#endif
    return USB_OK;

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_host_register_service
* Returned Value : USB_OK or error code
* Comments       :
*     Registers a callback routine for a specified event.
*
*END*--------------------------------------------------------------------*/
uint_32 _usb_host_register_service
(
    /* [IN] Handle to the USB device */
    _usb_host_handle         handle,

    /* [IN] type of event or endpoint number to service */
    uint_8                   type,

    /* [IN] Pointer to the service's callback function */
    void(_CODE_PTR_ service)(pointer, uint_32)
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
    USB_SERVICE_STRUCT_PTR              service_ptr;
    USB_SERVICE_STRUCT_PTR _PTR_        search_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_register_service\r\n");
#endif

    /* Needs mutual exclusion */
    USB_lock();

    /* Search for an existing entry for type */
    for (search_ptr = &usb_host_ptr->SERVICE_HEAD_PTR;
            *search_ptr;
            search_ptr = &(*search_ptr)->NEXT)
    {
        if ((*search_ptr)->TYPE == type)
        {
            /* Found an existing entry */
            USB_unlock();
#ifdef _HOST_DEBUG_
            bbu_printf("_usb_host_register_service USBERR_OPEN_SERVICE");
#endif
            return USBERR_OPEN_SERVICE;
        } /* Endif */
    } /* Endfor */

    /* No existing entry found - create a new one */
    service_ptr = (USB_SERVICE_STRUCT_PTR)USB_memalloc\
                  (sizeof(USB_SERVICE_STRUCT));

    if (!service_ptr)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_register_service memalloc failed");
#endif

        return USBERR_ALLOC_SERVICE;
    } /* Endif */

    service_ptr->TYPE = type;
    service_ptr->SERVICE = service;
    service_ptr->NEXT = NULL;
    *search_ptr = service_ptr;

    USB_unlock();

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_register_service SUCCESSFUL\r\n");
#endif

    return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_host_call_service
* Returned Value : USB_OK or error code
* Comments       :
*     Calls the appropriate service for the specified type, if one is
*     registered.
*
*END*--------------------------------------------------------------------*/
uint_32 _usb_host_call_service
(
    /* [IN] Handle to the USB device */
    _usb_host_handle handle,

    /* [IN] Type of service or endpoint */
    uint_8           type,

    /* [IN] Number of bytes in transmission */
    uint_32          length
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
    USB_SERVICE_STRUCT_PTR              service_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_call_service\r\n");
#endif

    /* Needs mutual exclusion */
    USB_lock();

    /* Search for an existing entry for type */
    for (service_ptr = usb_host_ptr->SERVICE_HEAD_PTR;
            service_ptr;
            service_ptr = service_ptr->NEXT)
    {
        if (service_ptr->TYPE == type)
        {
            service_ptr->SERVICE(handle, length);
            USB_unlock();
#ifdef _HOST_DEBUG_
            bbu_printf("_usb_host_call_service, service exists\r\n");
#endif
            return USB_OK;
        } /* Endif */

    } /* Endfor */

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_call_service, service closed\r\n");
#endif
    return USBERR_CLOSED_SERVICE;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_host_check_service
* Returned Value : USB_OK or error code
* Comments       :
*     Check a service is registered or not.
*
*END*--------------------------------------------------------------------*/
uint_32 _usb_host_check_service
(
    /* [IN] Handle to the USB device */
    _usb_host_handle handle,

    /* [IN] Type of service or endpoint */
    uint_8           type,

    /* [IN] Number of bytes in transmission */
    uint_32          length
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
    USB_SERVICE_STRUCT_PTR              service_ptr;
    length = length;
    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    /* Needs mutual exclusion */
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_check_service\r\n");
#endif

    USB_lock();

    /* Search for an existing entry for type */
    for (service_ptr = usb_host_ptr->SERVICE_HEAD_PTR;
            service_ptr;
            service_ptr = service_ptr->NEXT)
    {
        if (service_ptr->TYPE == type)
        {
#ifdef _HOST_DEBUG_
            bbu_printf("_usb_host_check_service service found\r\n");
#endif

            USB_unlock();
            return USB_OK;
        } /* Endif */
    } /* Endfor */

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_check_service service closed\r\n");
#endif

    return USBERR_CLOSED_SERVICE;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_open_pipe
*  Returned Value : USB_OK or error
*  Comments       :
*  _usb_host_open_pipe routine initializes a new pipe descriptor and
*  returns USB_OK or error and the pipe descriptor for the USB bus address,
*  endpoint number, direction of transfer, maximum packet size, interval
*  and the type of pipe
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_open_pipe
(
    /* [IN] the USB Host state structure */
    _usb_host_handle              handle,

    /* [IN] Pipe initialization parameters */
    PIPE_INIT_PARAM_STRUCT_PTR    pipe_init_params_ptr,

    /* [OUT] Pipe handle */
    _usb_pipe_handle _PTR_        pipe_handle
)
{
    /* Body */
    PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
    PIPE_INIT_PARAM_STRUCT_PTR pipe_descr_as_params;
    USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
    uint_32 i;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_open_pipe\r\n");
#endif
    USB_lock();

    for (i = 0; i < USB_MAX_PIPES; i++)
    {
        /* Check if a pipe is not open */
        if (!(usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR[i].OPEN))
        {
            break;
        } /* Endif */
    } /* Endfor */

    USB_unlock();

    if (i != USB_MAX_PIPES)
    {
        pipe_descr_ptr =
            &usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR[i];
        *pipe_handle = (pointer)pipe_descr_ptr;
        pipe_descr_as_params =
            (PIPE_INIT_PARAM_STRUCT_PTR)pipe_descr_ptr;

        /* copy identical fields params --> descriptor */
        *pipe_descr_as_params = *pipe_init_params_ptr;
        /* additional fields following identical header */
        pipe_descr_ptr->CURRENT_INTERVAL = pipe_descr_ptr->INTERVAL;
        pipe_descr_ptr->NEXTDATA01 = 0;       /* Start DTC at Data0 */
        pipe_descr_ptr->PIPE_ID = i;
        pipe_descr_ptr->OPEN = TRUE;
        pipe_descr_ptr->CURRENT_NAK_COUNT = pipe_descr_ptr->NAK_COUNT;
#ifdef __VUSB32__
        pipe_descr_ptr->DONT_ZERO_TERMINATE = TRUE;
#endif

#ifdef __VUSBHS__
#ifdef __USB_OS_MQX__
        if (pipe_descr_ptr->PIPETYPE <= USB_INTERRUPT_PIPE)
        {
            /* Call the low-level routine to send a setup packet */
            if (((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)\
                    usb_host_ptr->CALLBACK_STRUCT_PTR)->\
                    HOST_ALLOC_BANDWIDTH(handle, pipe_descr_ptr))
            {
                pipe_descr_ptr->OPEN = FALSE;
                USB_memfree((pointer)pipe_descr_ptr->tr_list_ptr);
#ifdef _HOST_DEBUG_
                bbu_printf("_usb_host_open_pipe bandwidth alloc failed\r\n");
#endif
                return USBERR_BANDWIDTH_ALLOC_FAILED;
            } /* Endif */
        } /* Endif */
#else
        if (pipe_descr_ptr->PIPETYPE <= USB_INTERRUPT_PIPE)
        {
            /* Allocate bandwidth if periodic pipe */
            if (_usb_ehci_allocate_bandwidth(handle, pipe_descr_ptr))
            {
                pipe_descr_ptr->OPEN = FALSE;
                USB_memfree((pointer)pipe_descr_ptr->tr_list_ptr);
#ifdef _HOST_DEBUG_
                bbu_printf("_usb_host_open_pipe bandwidth alloc failed\r\n");
#endif

                return USBERR_BANDWIDTH_ALLOC_FAILED;
            } /* Endif */
        } /* Endif */
#endif
#endif
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_open_pipe SUCCESSFUL\r\n");
#endif
        return USB_OK;
    } /* Endif */

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_open_pipe failed\r\n");
#endif
    return USBERR_OPEN_PIPE_FAILED;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_new_tr_element
*  Returned Value : pointer to new host tr struct, NULL if error
*  Comments       :
*         This function will allocate and link a new TR element in the
*  list of TRs for the specified pipe. It is assumed the caller has issued
*  USB_lock() before entry.
*
*END*-----------------------------------------------------------------*/
static PIPE_TR_STRUCT_PTR _usb_host_new_tr_element
(
    /* [IN] the pipe handle */
    _usb_pipe_handle  pipe_handle
)
{
    /* Body */

    PIPE_DESCRIPTOR_STRUCT_PTR    pipe_descr_ptr;
    PIPE_TR_STRUCT_PTR            pipe_tr_ptr, temp_pipe_tr_ptr;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_new_tr_element\r\n");
#endif

    pipe_tr_ptr = (PIPE_TR_STRUCT_PTR)USB_memalloc(sizeof(PIPE_TR_STRUCT));
    if (pipe_tr_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_new_tr_element malloc failed\r\n");
#endif
        return NULL;
    }
    USB_dev_memzero((pointer)pipe_tr_ptr, sizeof(PIPE_TR_STRUCT));

    /* Link pipe's TR structs in circular list of 1 or more items,
    ** where the pipe descriptor's tr list pointer holds the
    ** address of the next struct for scheduling (i.e. the
    ** "beginning" of the list).
    */
    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;
    temp_pipe_tr_ptr = pipe_descr_ptr->tr_list_ptr;

    if (temp_pipe_tr_ptr == NULL)
    {
        /* No existing items, make a circle of one */
        pipe_descr_ptr->tr_list_ptr = pipe_tr_ptr->next = pipe_tr_ptr;
    }
    else
    {
        /* Add new item to the "end" of the existing list */
        while (temp_pipe_tr_ptr->next != pipe_descr_ptr->tr_list_ptr)
            temp_pipe_tr_ptr = temp_pipe_tr_ptr->next;

        temp_pipe_tr_ptr->next = pipe_tr_ptr;
        pipe_tr_ptr->next = pipe_descr_ptr->tr_list_ptr;
    } /* EndIf */

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_new_tr_element SUCCESSFUL\r\n");
#endif
    return pipe_tr_ptr;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_tr_element
*  Returned Value : pointer to host's pipe TR struct, NULL if error
*  Comments       :
*     If the tr_index is zero or exists on the list, returns NULL
*     Otherwise initializes to zero a TR element with all linkages,
*       and sets its tr_index to the parameter value.  If no unused
*       tr_struct exists on the list (i.e. none has tr_index = 0),
*       a new struct is allocated and linked into the circular list.
*
*END*-----------------------------------------------------------------*/
PIPE_TR_STRUCT_PTR _usb_host_get_tr_element
(
    /* [IN] the pipe handle */
    _usb_pipe_handle  pipe_handle,

    /* [IN] transfer parameters */
    uint_32           tr_index
)
{
    /* Body */
    PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
    PIPE_TR_STRUCT_PTR         pipe_tr_list, temp_pipe_tr_list;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_tr_element\r\n");
#endif

    if (tr_index == 0)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_get_tr_element error null tr_index\r\n");
#endif
        return NULL;   /* zero is unused-TR flag */
    }

    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;
    pipe_tr_list = pipe_descr_ptr->tr_list_ptr;

    USB_lock();
    if (pipe_tr_list == NULL)     /* No list exists, start one */
    {
        pipe_tr_list = _usb_host_new_tr_element(pipe_handle);
        if (pipe_tr_list == NULL)
        {
            USB_unlock();
#ifdef _HOST_DEBUG_
            bbu_printf("_usb_host_get_tr_element error null pipe tr_list\r\n");
#endif
            return NULL;
        } /* Endif */
    }
    else      /* scan for unused TR's, and for index already in list */
    {
        temp_pipe_tr_list = pipe_tr_list;
        for (; ;)
        {
            /* TR index is either unused, or */
            if (temp_pipe_tr_list->TR_INDEX == 0)
            {
                pipe_tr_list = temp_pipe_tr_list;
                break;
            } /* Endif */
            /* TR index is a duplicate (= error), or */
            if (temp_pipe_tr_list->TR_INDEX == tr_index)
            {
                USB_unlock();
#ifdef _HOST_DEBUG_
                bbu_printf("_usb_host_get_tr_element error duplicate tr_index\r\n");
#endif
                return NULL;
            } /* Endif */
            /* TR is last in list, so insert a new one, or */
            if (temp_pipe_tr_list->next == pipe_tr_list)
            {
                pipe_tr_list = _usb_host_new_tr_element(pipe_handle);
                break;
            } /* Endif */

            /* Step over item to next on the list */
            temp_pipe_tr_list = temp_pipe_tr_list->next;

        } /* EndFor */
    } /* EndIf */
    pipe_tr_list->TR_INDEX = tr_index;  /* mark this TR busy */
    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_tr_element SUCCESSFUL\r\n");
#endif
    return pipe_tr_list;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_set_up_TR
*  Returned Value : None
*  Comments       :
*     Sets up a TR from init parameters.
*
*END*-----------------------------------------------------------------*/
uint_32  _usb_host_set_up_TR
(
    /* [IN] the pipe handle */
    _usb_pipe_handle           pipe_handle,

    /* [IN] transfer parameters */
    TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr,

    /* [OUT] the TR address on the pipe's TR list */
    PIPE_TR_STRUCT_PTR _PTR_   pipe_tr_ptr_ptr
)
{
    /* Body */
    PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
    PIPE_TR_STRUCT_PTR         pipe_tr_ptr;
    TR_STRUCT_PTR              tr_ptr;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_set_up_TR\r\n");
#endif

    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;
    /* Check if the pipe id is valid */
    if (pipe_descr_ptr->PIPE_ID > USB_MAX_PIPES)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_set_up_TR Invalid pipe handle\r\n");
#endif
        return USBERR_INVALID_PIPE_HANDLE;
    } /* Endif */

    pipe_tr_ptr = _usb_host_get_tr_element(pipe_handle,  tr_params_ptr->TR_INDEX);

    if (pipe_tr_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_set_up_TR NULL pipe handle\r\n");
#endif

        return USBERR_INVALID_PIPE_HANDLE;
    } /* Endif */

    tr_ptr = (TR_STRUCT_PTR)pipe_tr_ptr;

    /* Copy user-provided TR to system-linked TR */
    *tr_ptr = *((TR_STRUCT_PTR)tr_params_ptr);

    /* If TR's setup packet pointer is non-null, copy the packet */
    if (tr_params_ptr->DEV_REQ_PTR != NULL)
    {
        pipe_tr_ptr->setup_packet =
            *((USB_SETUP_PTR)tr_params_ptr->DEV_REQ_PTR);
    } /* Endif */

    /* return successful transfer initiation status */
    pipe_tr_ptr->status = USB_STATUS_TRANSFER_QUEUED;
    *pipe_tr_ptr_ptr = pipe_tr_ptr;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_set_up_TR SUCCESSFUL\r\n");
#endif

    return USB_STATUS_TRANSFER_QUEUED;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_setup
*  Returned Value : None
*  Comments       :
*  Sends a Setup packet. Internally, following the SendSetup call
*  this takes care of the whole transaction (including recieving or sending
*  any data to or from the device.
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_send_setup
(
    /* [IN] the USB Host state structure   */
    _usb_host_handle           handle,

    /* [IN] the pipe handle */
    _usb_pipe_handle           pipe_handle,

    /* [IN] transfer parameters */
    TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr
)
{
    /* Body */
    PIPE_TR_STRUCT_PTR         pipe_tr_ptr;
    PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
    //   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
    uint_32                    return_code;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_send_setup\r\n");
#endif

#ifdef __VUSB32__

    //  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

    /* Check if the pipe id is valid */
    if (pipe_descr_ptr->PIPE_ID > USB_MAX_PIPES)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_send_setup invalid pipe handle\r\n");
#endif
        return USBERR_INVALID_PIPE_HANDLE;
    } /* Endif */

    USB_lock();

    /* Check if a previously queued transfer is still in progress for this
    ** pipe
    */
    if (pipe_descr_ptr->STATUS != USB_STATUS_IDLE)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_send_setup transfer in progress\r\n");
#endif

        return USB_STATUS_TRANSFER_IN_PROGRESS;
    }
    else
    {
        USB_unlock();
        /* Initialize the pipe descriptor for the current transfer request */
        pipe_descr_ptr->SOFAR = 0;
        pipe_descr_ptr->NEXTDATA01 = 0;       /* reset DATA0/1 */

        pipe_descr_ptr->RX_PTR = tr_params_ptr->RX_BUFFER;

        pipe_descr_ptr->TX1_PTR = (uchar_ptr)&pipe_descr_ptr->SETUP;
        pipe_descr_ptr->TX2_PTR = tr_params_ptr->TX_BUFFER;

        pipe_descr_ptr->TODO1 = tr_params_ptr->RX_LENGTH;
        pipe_descr_ptr->TODO2 = tr_params_ptr->TX_LENGTH;

        pipe_descr_ptr->SEND_PHASE = FALSE;
        pipe_descr_ptr->FIRST_PHASE = TRUE;
        pipe_descr_ptr->TR = *tr_params_ptr;
        pipe_descr_ptr->SETUP = *(USB_SETUP_PTR)tr_params_ptr->DEV_REQ_PTR;

        /***************************************************************
        For data caching it is important that we update the memory
        with the intended contents.
        ***************************************************************/
#ifdef _DATA_CACHE_
        USB_dcache_flush_mlines((void *)&pipe_descr_ptr->SETUP, 1);
        USB_dcache_invalidate_mlines((void *)pipe_descr_ptr->RX_PTR, 1);
#endif


        /* Check if this setup packet will have a send data phase */
        if (!((((INTERNAL_USB_SETUP_PTR)pipe_descr_ptr->TX1_PTR)->BREQUESTTYPE) &
                USB_SETUP_DATA_XFER_DIRECTION))
        {
            pipe_descr_ptr->SEND_PHASE = TRUE;
        } /* Endif */

        USB_lock();

        /* return successful transfer initiation status */
        pipe_descr_ptr->STATUS = USB_STATUS_TRANSFER_QUEUED;

#ifdef __USB_OS_MQX__

        /* Call the low-level routine to send a setup packet */
        ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
        HOST_SEND_SETUP(handle, pipe_descr_ptr);
#else
        _usb_hci_vusb11_send_setup(handle, pipe_descr_ptr);
#endif

        /* Indicate that a transfer is pending on this pipe */
        pipe_descr_ptr->PACKETPENDING = TRUE;

        /*full speed driver assumes that transfer is queued. It should be changed
        in a future release */
        return_code = USB_STATUS_TRANSFER_QUEUED;

        USB_unlock();
    } /* Endif */

#else

    USB_lock();

    /* Initialize a TR and link it into the pipe's TR list */
    return_code = _usb_host_set_up_TR(pipe_handle, tr_params_ptr, &pipe_tr_ptr);

    if (return_code != USB_STATUS_TRANSFER_QUEUED)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_send_setup error status\r\n");
#endif

        return return_code;
    } /* Endif */

    // usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

    pipe_descr_ptr->NEXTDATA01 = 0;       /* reset DATA0/1 */

    /* We have obtained the current TR on the Pipe's TR list
    ** from _usb_host_set_up_TR
    */
    /* true if this setup packet will have a send data phase */
    pipe_tr_ptr->SEND_PHASE = !(pipe_tr_ptr->setup_packet.BMREQUESTTYPE &
                                USB_SETUP_DATA_XFER_DIRECTION);

    /***************************************************************
    For data caching it is important that we update the memory
    with the intended contents.
    ***************************************************************/
#ifdef _DATA_CACHE_
    USB_dcache_flush_mlines(
        (uint_32)&pipe_tr_ptr->setup_packet,
        1);

    if (pipe_descr_ptr->DIRECTION == USB_SEND)
    {
        if (pipe_tr_ptr->TX_LENGTH > 0)
        {
            USB_dcache_flush_mlines(
                (uint_32)pipe_tr_ptr->TX_BUFFER,
                pipe_tr_ptr->TX_LENGTH
            );
        }
    }
    else
    {
        if (pipe_tr_ptr->RX_LENGTH > 0)
        {
            USB_dcache_invalidate_mlines(
                (uint_32)pipe_tr_ptr->RX_BUFFER,
                pipe_tr_ptr->RX_LENGTH
            );
        }
    }

#endif


#ifdef __USB_OS_MQX__
    /* Call the low-level routine to send a setup packet */
    return_code = ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
                  HOST_SEND_SETUP(handle, pipe_descr_ptr, pipe_tr_ptr);
#else
    return_code = _usb_hci_vusb20_send_setup(handle, pipe_descr_ptr, pipe_tr_ptr);
#endif

    USB_unlock();

#endif

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_send_setup SUCCESSFUL\r\n");
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
*  Function Name  : _usb_host_get_transfer_status
*  Returned Value : Status
*  Comments       :
* _usb_host_get_transfer_status is a routine that returns status of a transfer.
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_get_transfer_status
(
    /* [IN] the pipe handle */
    _usb_pipe_handle     pipe_handle,

    /* [IN] the transfer number */
    uint_32              tr_number
)
{
    /* Body */
    PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
    PIPE_TR_STRUCT_PTR         pipe_tr_list_ptr;
    uint_32                    status = USBERR_INVALID_PIPE_HANDLE;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_transfer_status");
#endif

#ifdef __VUSB32__
    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_transfer_status SUCCESSFUL");
#endif
    return pipe_descr_ptr->STATUS;
#else

    USB_lock();


    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;
    if (pipe_descr_ptr != NULL)
    {
        pipe_tr_list_ptr = pipe_descr_ptr->tr_list_ptr;
        if (pipe_tr_list_ptr != NULL)
        {
            do
            {
                if (pipe_tr_list_ptr->TR_INDEX == tr_number)
                {
                    status = pipe_tr_list_ptr->status;
                    break;
                } /* Endif */
                pipe_tr_list_ptr = pipe_tr_list_ptr->next;
            }
            while (pipe_tr_list_ptr != pipe_descr_ptr->tr_list_ptr);
        } /* Endif */
        if (pipe_tr_list_ptr->TR_INDEX != tr_number)
        {
            /* Transaction done */
            status = USB_STATUS_IDLE;
        } /* Endif */
    } /* Endif */

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_get_transfer_status SUCCESSFUL");
#endif

    return status;
#endif
} /* Endbody */

/* EOF */




