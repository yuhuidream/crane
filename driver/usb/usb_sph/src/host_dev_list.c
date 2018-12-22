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
***   This file contains the device-tree root for USB class drivers
***
***
**************************************************************************
**END*********************************************************/

#include "usb_types.h"
#ifdef __USB_OS_MQX__
#include "types.h"
#include "mqx_arc.h"
#else
#ifdef __USB_OTG__
#include "otgapi.h"
#include "usbprv_otg.h"
#endif
#include "usbprv_host.h"
#endif





static const PIPE_INIT_PARAM_STRUCT pipe_init_params_prototype =
{
    NULL,                   /* device instance pointer */
    0,                      /* polling interval */
    64,                     /* max. packet size */
    DEFAULT_MAX_NAK_COUNT,  /* max. NAKs */
    0, 0,                   /* 1st frame/uframe */
    0,                      /* zero-length last packet */
    0,                      /* device address on a particular USB */
    0,                      /* endpoint # */
    0,                      /* direction */
    0,                      /* pipe type */
    0,                      /* speed */
    1                       /* tr's per uframe */
};

/* Unpublished functions, not intended for application use */
extern void  usb_hostdev_attach_detach
(DEV_INSTANCE_PTR,
 uint_32);

extern void  usb_hostdev_delete_interface
(DEV_INSTANCE_PTR,
 INTERFACE_DESCRIPTOR_PTR);

USB_STATUS  usb_dev_list_free_memory
(_usb_device_instance_handle);

static void  usb_dev_list_close_pipe
(_usb_host_handle,
 PIPE_DESCRIPTOR_STRUCT_PTR
);

void  usb_dev_list_close_pipe_bundle
(PIPE_BUNDLE_STRUCT_PTR);


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_dev_list_free_memory
* Returned Value : USB_OK if valid device, else error code
* Comments       :
*     Close pipes & free memory on device's linked list, whose
*        anchor is device.memlist-->
*
*END*--------------------------------------------------------------------*/
USB_STATUS usb_dev_list_free_memory
(
    /* [IN] Handle for the USB device */
    _usb_device_instance_handle   dev_handle
)
{
    /* Body */
    DEV_INSTANCE_PTR  dev;
    DEV_MEMORY_PTR    mem_ptr, list_ptr;
    USB_STATUS        error;
    uint_8            intf_no;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_free_memory\r\n");
#endif

    error = usb_hostdev_validate(dev_handle);
    if (error != USB_OK)
#ifdef _HOST_DEBUG_
        bbu_printf("usb_dev_list_free_memory invalid device handle\r\n");
#endif
    return error;

    dev = (DEV_INSTANCE_PTR)dev_handle;

    /* Deleting interfaces closes their pipes */
    for (intf_no = 0; intf_no < dev->num_of_interfaces; intf_no++)
        usb_hostdev_delete_interface(dev, dev->intf_descriptor[intf_no]);

    /* Free memory blocks on this device's list */
    USB_lock();
    mem_ptr = dev->memlist;
    list_ptr = mem_ptr->next;

    do
    {
        USB_memfree(mem_ptr);
        mem_ptr = list_ptr;
        if (mem_ptr == NULL)
            break;
        list_ptr = mem_ptr->next;
    }
    while (mem_ptr != NULL);

    dev->memlist = NULL;
    USB_unlock();

#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_free_memory SUCCESSFUL\r\n");
#endif
    return USB_OK;

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_dev_list_get_memory
* Returned Value : USB_OK if memory allocated, else error code
* Comments       :
*     Memory is added at the end of a linked list, whose
*        anchor is device.memlist-->
*
*END*--------------------------------------------------------------------*/
USB_STATUS usb_dev_list_get_memory
(
    /* [IN] Pointer to the USB device */
    DEV_INSTANCE_PTR        dev_ptr,

    /* [IN] Size of memory payload required */
    uint_16                 mem_size,

    /* [IN] Type of memory payload required */
    memory_type             mem_type,

    /* [OUT] Pointer to memory block's header */
    pointer         _PTR_   header_ptr
)
{
    /* Body */

    DEV_MEMORY_PTR  mem_ptr, list_ptr;
    USB_STATUS      error;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_get_memory\r\n");
#endif

    USB_lock();
    error = usb_hostdev_validate((pointer)dev_ptr);
    USB_unlock();

    if (error != USB_OK)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_dev_list_get_memory invalid device handle\r\n");
#endif
        return error;
    }
    if ((mem_type <= USB_MEMTYPE_MIN_VALUE) ||
            (mem_type >= USB_MEMTYPE_MAX_VALUE))
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_dev_list_get_memory invalid memory type\r\n");
#endif
        return USBERR_INVALID_MEM_TYPE;
    } /* Endif */

    /* get memory for header + payload, rounded up to 4-byte */
    mem_ptr = (DEV_MEMORY_PTR)\
              USB_memalloc((mem_size + MEM_HEADER_LEN + 3) & 0xFFFC);

    if (mem_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_dev_list_get_memory malloc failed\r\n");
#endif

        return USBERR_GET_MEMORY_FAILED;
    }

    if (dev_ptr->memlist == NULL)
    {
        dev_ptr->memlist = mem_ptr;
    }
    else
    {
        list_ptr = dev_ptr->memlist;

        while (list_ptr->next != NULL)
            list_ptr = list_ptr->next;

        list_ptr->next = mem_ptr;
    } /* EndIf */

    mem_ptr->next = NULL;
    mem_ptr->blktype = mem_type;
    mem_ptr->blksize = mem_size;
    *header_ptr = (pointer)mem_ptr;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_get_memory SUCCESSFUL\r\n");
#endif
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_dev_list_attach_device
* Returned Value :
* Comments       :
*     This function will be called when attach interrupt happens, to
*       add onto the device list and do common initialization.
*
*END*--------------------------------------------------------------------*/
void usb_dev_list_attach_device
(
    _usb_host_handle  handle,
    uint_8            speed,
    uint_8            hub_no,
    uint_8            port_no
)
{
    /* Body */
    DEV_INSTANCE_PTR           dev_instance_ptr;
    PIPE_INIT_PARAM_STRUCT     ctrl_pipe_init_params;
    USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
    DEV_INSTANCE_PTR           device_root = NULL;
#ifdef __USB_OTG__
    uint_32                    state;
#endif
#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_attach_device attach device\r\n");
#endif


    //  bbu_printf("usb_dev_list_attach_device attach device\r\n");

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    /* Allocate new device instance */
    dev_instance_ptr = (DEV_INSTANCE_PTR)\
                       USB_memalloc(sizeof(DEV_INSTANCE));

    if (dev_instance_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_dev_list_attach_device failed to malloc device handle\r\n");
#endif
        return;
    } /* EndIf */

    memset((void *)dev_instance_ptr, 0, sizeof(DEV_INSTANCE));

    dev_instance_ptr->host = handle;
    dev_instance_ptr->speed = speed;
    dev_instance_ptr->hub_no = hub_no;
    dev_instance_ptr->port_no = port_no;
    dev_instance_ptr->cfg_value = 0; /* We don't know yet what the
                                    ** device's default configuration is
                                    */

    USB_lock();
    /* Insert instance in pushdown list of devices */
    dev_instance_ptr->next = usb_host_ptr->DEVICE_LIST_PTR;     /* existing list */
    usb_host_ptr->DEVICE_LIST_PTR = (pointer)dev_instance_ptr;  /* root = new device */
    device_root = dev_instance_ptr;

    /* Find unused address from 1 - 127 for this host */
    device_root->address = 1;
    do
    {
        dev_instance_ptr = device_root->next; /* 2nd device, if any */
        while (dev_instance_ptr != NULL)      /* step through list  */
        {
            if (device_root->address == dev_instance_ptr->address)
            {
                device_root->address++;
                break;
            } /* EndIf */
            dev_instance_ptr = dev_instance_ptr->next;
        } /* EndWhile */
    }
    while ((dev_instance_ptr != NULL) && (device_root->address <= 127));

    dev_instance_ptr = device_root;

    /* If all 127 addresses used up, delete instance & bail out */
    if (dev_instance_ptr->address > 127)
    {
        usb_host_ptr->DEVICE_LIST_PTR = (pointer)\
                                        dev_instance_ptr->next; /* pop off list */
        USB_unlock();
        USB_memfree((pointer)dev_instance_ptr);
#ifdef _HOST_DEBUG_
        bbu_printf("usb_dev_list_attach_device out of addresses\r\n");
#endif
        return;
    } /* EndIf */

    USB_unlock();

    /*-----------------------------------------------------------**
    ** Open control pipe, get first 8 bytes of device descriptor **
    ** The host_ch9 routine internally sets the callback to      **
    **    usb_host_cntrl_transaction_done (in usb_host_ch9.c)    **
    **    where the action resumes on completion of the get.     **
    **-----------------------------------------------------------*/

    ctrl_pipe_init_params = pipe_init_params_prototype;
    ctrl_pipe_init_params.DEV_INSTANCE = dev_instance_ptr;
    ctrl_pipe_init_params.PIPETYPE = USB_CONTROL_PIPE;
    ctrl_pipe_init_params.SPEED = dev_instance_ptr->speed;

#ifdef __USB_OTG__
    _usb_otg_get_status((pointer)usb_otg_state_struct_ptr,
                        USB_OTG_STATE, &state);

    if (state < B_IDLE)
    {
        _usb_otg_set_status((pointer)usb_otg_state_struct_ptr,
                            USB_OTG_A_BUS_REQ, TRUE);
        _usb_otg_set_status((pointer)usb_otg_state_struct_ptr,
                            USB_OTG_B_CONN, TRUE);
    }
    else
    {
        _usb_otg_set_status((pointer)usb_otg_state_struct_ptr,
                            USB_OTG_A_CONN, TRUE);
    } /* Endif */
#endif

    if (USB_OK != _usb_host_open_pipe(dev_instance_ptr->host,
                                      &ctrl_pipe_init_params,
                                      &dev_instance_ptr->control_pipe))
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_dev_list_attach_device open pipe failed\r\n");
#endif
        return;
    } /* Endif */

    /* Set state to enter after transaction completion */
    dev_instance_ptr->state = DEVSTATE_DEVDESC8;

    _usb_host_ch9_get_descriptor((pointer)dev_instance_ptr,
                                 DESCTYPE_DEVICE << 8, 0, 8,
                                 (uchar_ptr)&dev_instance_ptr->dev_descriptor);
#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_attach_device SUCCESSFUL\r\n");
#endif

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_dev_list_close_pipe
* Returned Value :
* Comments       :
*     Cancel pending TR's and close a pipe.
*
*END*--------------------------------------------------------------------*/
void  usb_dev_list_close_pipe
(
    _usb_host_handle              handle,
    PIPE_DESCRIPTOR_STRUCT_PTR    pipe_ptr
)
{
    /* Body */
    //   USB_HOST_STATE_STRUCT_PTR        usb_host_ptr;
    PIPE_TR_STRUCT_PTR               tr_ptr;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_close_pipe\r\n");
#endif

    if (pipe_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_dev_list_close_pipe NULL pipe\r\n");
#endif
        return;
    }

    // usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    USB_lock();

#ifdef __VUSB32__
    _usb_host_cancel_transfer(handle, pipe_ptr, 0);
#endif

    /* Cancel all active TR's in the pipe */
    tr_ptr = pipe_ptr->tr_list_ptr;
    if (tr_ptr)
    {
        while (tr_ptr->TR_INDEX != 0)
        {
            tr_ptr->CALLBACK = NULL;   /* no surprises */

#ifndef __VUSB32__
#ifdef __USB_OS_MQX__
            /* Cancel the transfer at low-level */
            ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->\
            HOST_CANCEL(handle, pipe_ptr, tr_ptr);
#else
            _usb_hci_vusb20_cancel_transfer(handle, pipe_ptr, tr_ptr);
#endif
#endif
            /* Indicate that the TR is idle */
            tr_ptr->TR_INDEX = 0;
            tr_ptr->status = USB_STATUS_IDLE;
            tr_ptr = tr_ptr->next;
        } /* EndWhile */
    } /* Endif */

    /* zero the pipe descriptor, except keep TR list */
    tr_ptr = pipe_ptr->tr_list_ptr;
    _usb_host_close_pipe(handle, (pointer)pipe_ptr);
    pipe_ptr->tr_list_ptr = tr_ptr;

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_close_pipe SUCCESSFUL\r\n");
#endif

} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_dev_list_close_pipe_bundle
* Returned Value :
* Comments       :
*    Close all pipes in an interface's bundle.
*
*END*--------------------------------------------------------------------*/
void  usb_dev_list_close_pipe_bundle
(
    PIPE_BUNDLE_STRUCT_PTR    pbs_ptr
)
{
    /* Body */

    _usb_host_handle  handle;
    uint_8            index, index_max;
#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_close_pipe_bundle\r\n");
#endif

    handle = ((DEV_INSTANCE_PTR)pbs_ptr->dev_handle)->host;
    index_max = ((INTERFACE_DESCRIPTOR_PTR)
                 pbs_ptr->intf_handle)->bNumEndpoints;

    for (index = 0; index < index_max; index++)
        usb_dev_list_close_pipe(handle, pbs_ptr->pipe_handle[index]);

#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_close_pipe_bundle SUCCESSFUL\r\n");
#endif

} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_dev_list_detach_device
* Returned Value :
* Comments       :
*     This function will be called when detach interrupt happens.
*
*END*--------------------------------------------------------------------*/
void  usb_dev_list_detach_device
(
    _usb_host_handle  handle,
    uint_8            hub_no,
    uint_8            port_no
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR  usb_host_ptr =
        (USB_HOST_STATE_STRUCT_PTR)handle;
    DEV_INSTANCE_PTR  dev_instance_ptr;
    // clean warning DEV_INSTANCE_PTR  _PTR_ device_root = &(DEV_INSTANCE_PTR)usb_host_ptr->DEVICE_LIST_PTR;
    DEV_INSTANCE_PTR  _PTR_ device_root;
#ifdef __USB_OTG__
    uint_32                    state;
#endif
    device_root = (DEV_INSTANCE_PTR  _PTR_)(&usb_host_ptr->DEVICE_LIST_PTR);

#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_detach_device\r\n");
#endif

    /* search device list for the one being detached */
    USB_lock();
    for (dev_instance_ptr = *device_root;
            dev_instance_ptr != NULL;
            dev_instance_ptr = dev_instance_ptr->next)
    {
        if (dev_instance_ptr->port_no != port_no)
            continue;
        if (dev_instance_ptr->hub_no != hub_no)
            continue;
        if (dev_instance_ptr->host != handle)
            continue;
        break;
    } /* EndFor */

    USB_unlock();

    if (dev_instance_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_dev_list_detach_device NULL device pointer\r\n");
#endif
        return;  /* No match, abandon ship! */
    } /* Endif */

    /* SGarg 09/23/2003 . Sometimes  a detach could come even before a
    attach is finished. This means that we should make sure that device
    memory is not NULL before we try to clean it up. For example this happens
    with OPT tester test 4.10.*/

    if (dev_instance_ptr->memlist != NULL)
    {
        /* Notify the application of unavailable interfaces */
        usb_hostdev_attach_detach(dev_instance_ptr, USB_DETACH_EVENT);

    }

    /* Unlink device from the instance list */
    dev_instance_ptr->control_callback = NULL; /* no surprises */

    /* Close control pipe */
    usb_dev_list_close_pipe(handle, dev_instance_ptr->control_pipe);

    /* SGarg 09/24/2003 . Sometimes  a detach could come even before a
    attach is finished. This means that we should make sure that device
    memory is not NULL before we try to clean it up. For example this happens
    with OPT tester test 4.10.*/
    if (dev_instance_ptr->memlist != NULL)
    {
        usb_dev_list_free_memory((pointer)dev_instance_ptr);
    }

    USB_lock();
#ifdef MSC_CLASS
    memfree_pipe4msc(dev_instance_ptr );
#endif
    /* Find the address of the "pointer to the detached device" */
    while (*device_root != dev_instance_ptr)
    {
        device_root = &(*device_root)->next;
    } /* Endwhile */

    /* Remove the device */
    *device_root = dev_instance_ptr->next;

    USB_memfree(dev_instance_ptr);

#ifdef __USB_OTG__
    _usb_otg_get_status((pointer)usb_otg_state_struct_ptr,
                        USB_OTG_STATE, &state);

    if (state < B_IDLE)
    {
        _usb_otg_get_status((pointer)usb_otg_state_struct_ptr,
                            USB_OTG_B_CONN, &state);
        if (state)
        {
            _usb_otg_set_status((pointer)usb_otg_state_struct_ptr,
                                USB_OTG_B_CONN, FALSE);
        } /* Endif */
    }
    else
    {
        _usb_otg_get_status((pointer)usb_otg_state_struct_ptr,
                            USB_OTG_A_CONN, &state);
        if (state)
        {
            _usb_otg_set_status((pointer)usb_otg_state_struct_ptr,
                                USB_OTG_A_CONN, FALSE);
        } /* Endif */
    } /* Endif */
#endif

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("usb_dev_list_detach_device SUCCESSFUL\r\n");
#endif

} /* EndBody */

/* EOF */

