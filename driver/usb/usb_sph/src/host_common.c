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
***   This file contains common functions for USB class drivers
***
*** Implementation Notes:
***   An interface is supported by two data structures on the
***      owning device's list of allocated memory:-
***   (1) a pipe bundle struct with pointers to open pipes for
***      all endpoints on the interface
***   (2) a class-interface struct with queues for TR logging,
***      pipe handles, and other class-specific status etc.
***   Each <class_name>.h must define its interface struct, which must
***      begin with the fields in GENERAL_CLASS (see common.h),
***      if common routines are going to be used.
***      It is strongly recommended that all class init functions
***      call usb_host_class_intf_init (pointer to pipe bundle,
***      pointer to class-intf, (pointer)&class_anchor ).  This
***      will verify correct pointers, zero the interface struct,
***      and set up the header pointers + key correctly.
***   All class usage of the pointers etc. in the struct
***      must be protected against detach (which occurs without
***      advance notice):-
***         USB_lock();
***         if (usb_host_class_intf_validate
***               (class-call-struct pointer) )
***           {
***
***           Issue commands / Use pipes etc.
***
***           }
***         USB_unlock();
***
**************************************************************************
**END*********************************************************/
#include "usb_types.h"
#include "hostapi.h"

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#else
#include "usbprv_host.h"
#endif

extern CLASS_MAP class_interface_map[];

/* Unique non-zero key to assure valid interface use. */
static uint_32 Class_Interface_Key = 0;

/* Prototypes, copy to stack frames for each use instance */

static TR_INIT_PARAM_STRUCT tr_prototype =
{
    0,    /* transaction index */
    NULL, /* TX buffer */
    NULL, /* Rx buffer */
    0,    /* TX/data phase length */
    0,    /* RX/data phase length */
    NULL, /* callback pointer */
    NULL, /* callback param */
    NULL  /* setup packet ptr */
};

/* Unpublished functions, not intended for application use */
void  usb_hostdev_attach_detach(DEV_INSTANCE_PTR, uint_32);
void  usb_hostdev_delete_interface(DEV_INSTANCE_PTR, INTERFACE_DESCRIPTOR_PTR);
static USB_STATUS usb_hostdev_get_class(_usb_device_instance_handle,
                                        uint_16, pointer _PTR_ );
static boolean usb_hostdev_keep_class(INTERFACE_DESCRIPTOR_PTR);
static uint_32 usb_hostdev_mask_class_etc(uint_8_ptr , uint_8_ptr);
static USB_STATUS  usb_hostdev_open_pipe(_usb_device_instance_handle,
        uint_32, uint_8, _usb_pipe_handle _PTR_);
static boolean  usb_host_driver_info_match(DEV_INSTANCE_PTR,
        INTERFACE_DESCRIPTOR_PTR, USB_HOST_DRIVER_INFO_PTR);
static void  usb_host_driver_info_scan(DEV_INSTANCE_PTR,
                                       INTERFACE_DESCRIPTOR_PTR, uint_32);
static boolean usb_host_driver_info_nonzero(USB_HOST_DRIVER_INFO_PTR);
static CLASS_MAP_PTR  usb_host_map_class_intf(PIPE_BUNDLE_STRUCT_PTR);
extern void  usb_dev_list_close_pipe_bundle(PIPE_BUNDLE_STRUCT_PTR);
extern USB_STATUS  usb_dev_list_free_memory(_usb_device_instance_handle);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_attach_detach
* Returned Value : none
* Comments       :
*     Scan device config for interfaces that may need callbacks
*     for matching classes etc. from each interface. This function is
*     called when there configuration change or attach or detach event
*
*END*--------------------------------------------------------------------*/
void  usb_hostdev_attach_detach
(
    /* [IN] USB device */
    DEV_INSTANCE_PTR           dev_ptr,

    /* [IN] event causing callback */
    uint_32                    event_code
)
{
    /* Body */
    DESCRIPTOR_UNION           desc;
    uint_32                    cfg_end;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_attach_detach\r\n");
#endif

    USB_lock();

    desc.pntr = &dev_ptr->memlist->payload;
    cfg_end = desc.word + utoh16(desc.cfig->wTotalLength);

    /* We will always start with config desc so update the search pointer */
    desc.word += desc.cfig->bLength;

    /* Scan through all the interface descriptors for this configuration */
    do
    {
        if (desc.intf->bDescriptorType == DESCTYPE_INTERFACE)
            usb_host_driver_info_scan(dev_ptr, desc.intf, event_code);
        desc.word += desc.intf->bLength; /* next descriptor */
    }
    while (desc.word < cfg_end);

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_attach_detach SUCCESSFUL\r\n");
#endif
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_busy_check
* Returned Value : USB_OK or
*                    USBERR_DEVICE_NOT_FOUND
*                    USBERR_DEVICE_BUSY
* Comments       :
*     Function to verify pointer is address of an instance on device list.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
USB_STATUS  usb_hostdev_busy_check
(
    _usb_device_instance_handle  device_handle
)
{
    /* Body */
    DEV_INSTANCE_PTR           dev_ptr = (DEV_INSTANCE_PTR)device_handle;
    USB_HOST_STATE_STRUCT_PTR  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)dev_ptr->host;
    DEV_INSTANCE_PTR           test_ptr = (DEV_INSTANCE_PTR)usb_host_ptr->DEVICE_LIST_PTR;
#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_busy_check\r\n");
#endif

    while ((test_ptr != dev_ptr) && (test_ptr != NULL))
        test_ptr = test_ptr->next;

    if (test_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_hostdev_busy_check device not found\r\n");
#endif
        return USBERR_DEVICE_NOT_FOUND;
    }

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_busy_check SUCCESSFUL\r\n");
#endif

    return (test_ptr->memlist->next != NULL) ?
           USBERR_DEVICE_BUSY : USB_OK;
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_validate
* Returned Value : USB_OK or USBERR_DEVICE_NOT_FOUND
* Comments       :
*     Function to verify pointer is address of an instance on device list.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
USB_STATUS  usb_hostdev_validate
(
    pointer  device_handle
)
{
    /* Body */
    DEV_INSTANCE_PTR           dev_ptr = (DEV_INSTANCE_PTR)device_handle;
    USB_HOST_STATE_STRUCT_PTR  usb_host_ptr = dev_ptr->host;
    DEV_INSTANCE_PTR           test_ptr = (DEV_INSTANCE_PTR)usb_host_ptr->DEVICE_LIST_PTR;
#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_validate\r\n");
#endif

    while ((test_ptr != dev_ptr) && (test_ptr != NULL))
        test_ptr = test_ptr->next;

#ifdef _HOST_DEBUG_
    if (test_ptr == NULL)
    {
        bbu_printf("usb_hostdev_validate null device handle\r\n");
    }
    else
    {
        bbu_printf("usb_hostdev_validate SUCCESSFUL\r\n");
    }
#endif

    return (test_ptr == NULL) ? USBERR_DEVICE_NOT_FOUND : USB_OK;
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hostdev_get_buffer
*  Returned Value : pointer to the buffer, or NULL + error
*  Comments       :
*        Applications should use this function to get I/O buffers and
*        other work areas that stay allocated until the device is
*        detached.  At that time, these are all freed by the system.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_hostdev_get_buffer
(
    /* [IN] the device instance */
    _usb_device_instance_handle   dev_handle,

    /* [IN] buffer size to get */
    uint_32                       buffer_size,

    /* [OUT] pointer to the buffer, or NULL */
    uchar_ptr               _PTR_ buffer_ptr
)
{
    /* Body */
    DEV_MEMORY_PTR    buffer;
    DEV_INSTANCE_PTR  dev_ptr;
    USB_STATUS        error;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_get_buffer\r\n");
#endif

    USB_lock();

    error = usb_hostdev_validate(dev_handle);
    if (error != USB_OK)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_hostdev_get_buffer invalid device handle\r\n");
#endif
        return error;
    } /* Endif */

    dev_ptr = (DEV_INSTANCE_PTR)dev_handle;

    error =  usb_dev_list_get_memory(dev_ptr, buffer_size,
                                     USB_MEMTYPE_BUFFER, (pointer)&buffer);

    if (error != USB_OK)
        *buffer_ptr = NULL;
    else
        *buffer_ptr = (uchar_ptr)&buffer->payload;

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_get_buffer SUCCESSFUL\r\n");
#endif
    return error;

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_hostdev_get_class
*  Returned Value : pointer to class/interface struct, or NULL + error
*  Comments       :
*        This wrapper is for _usb_hostdev_select_interface use only.
*
*END*-----------------------------------------------------------------*/
static USB_STATUS usb_hostdev_get_class
(
    /* [IN] the device instance */
    _usb_device_instance_handle   dev_handle,

    /* [IN] buffer size to get */
    uint_16                       struct_size,

    /* [OUT] pointer to the buffer, or NULL */
    pointer                 _PTR_ struct_ptr
)
{
    /* Body */
    DEV_MEMORY_PTR    memory_block;
    DEV_INSTANCE_PTR  dev_ptr;
    USB_STATUS        error;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_get_class\r\n");
#endif

    USB_lock();
    error = usb_hostdev_validate(dev_handle);
    if (error != USB_OK)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("usb_hostdev_get_class invalid device handle\r\n");
#endif
        return error;
    } /* Endif */

    dev_ptr = (DEV_INSTANCE_PTR)dev_handle;

    error =  usb_dev_list_get_memory
             (dev_ptr,
              struct_size,
              USB_MEMTYPE_CLASS,
              (pointer)&memory_block);

    if (error != USB_OK)
        *struct_ptr = NULL;
    else
        *struct_ptr = (pointer)&memory_block->payload;

    USB_unlock();

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_get_class SUCCESSFUL\r\n");
#endif
    return error;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_hostdev_keep_class
*  Returned Value : TRUE if class/interface exists/should be kept
*                   FALSE if class/interface should be built/deleted
*  Comments       :
*        Audio interfaces associated with a control interface can be
*        changed without tearing down the whole collection. Don't
*        make a new (delete an old) class-interface struct when
*        these interfaces are selected(deleted).
*  It is presumed that this function is called with interrupts disabled
*
*END*-----------------------------------------------------------------*/
static USB_STATUS usb_hostdev_keep_class
(
    /* [IN] interface descriptor */
    INTERFACE_DESCRIPTOR_PTR   intf
)
{
    /* Body */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_keep_class\r\n");
#endif

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_keep_class SUCCESSFUL\r\n");
#endif

    return (intf->bInterfaceClass == USB_CLASS_AUDIO) &&
           ((intf->bInterfaceSubClass == USB_SUBCLASS_AUD_STREAMING) ||
            (intf->bInterfaceSubClass == USB_SUBCLASS_AUD_MIDI_STRM));

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hostdev_get_descriptor
*  Returned Value : handle of the descriptor, or NULL + error
*  Comments       :
*     During a successful get interface call, a pointer to the
*     interface is saved for use by subsequent get endpoint calls.
*     Since non-control endpoints are "owned" by interfaces, calls
*     for endpoints without first knowing the interface are invalid.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_hostdev_get_descriptor
(
    /* [IN] the device instance */
    _usb_device_instance_handle   dev_handle,

    /* [IN] the type of descriptor to get */
    descriptor_type               desc_type,

    /* [IN] the descriptor index, or 0 */
    uint_8                        desc_index,

    /* [IN] the interface alternate, or 0 */
    uint_8                        intf_alt,

    /* [OUT] handle of the descriptor, or NULL */
    pointer                 _PTR_ descriptor
)
{
    /* Body */

    DEV_INSTANCE_PTR  dev_ptr;
    DESCRIPTOR_UNION  ptr1, ptr2;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_get_descriptor\r\n");
#endif

    USB_lock();

    *descriptor = NULL; /* Assume the worst */

    /* Check that device handle points to a device */
    if (usb_hostdev_validate(dev_handle) != USB_OK)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_hostdev_get_descriptor invalid device handle\r\n");
#endif
        return USBERR_DEVICE_NOT_FOUND;
    } /* Endif */

    dev_ptr = (DEV_INSTANCE_PTR)dev_handle;

    if (dev_ptr->state < DEVSTATE_CFG_READ)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_hostdev_get_descriptor device busy\r\n");
#endif
        return USBERR_DEVICE_BUSY;
    } /* Endif */

    /* Device descriptor is in the instance struct */
    if (desc_type == DESCTYPE_DEVICE)
    {
        *descriptor = &dev_ptr->dev_descriptor;
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_hostdev_get_descriptor SUCCESSFUL\r\n");
#endif
        return USB_OK;
    } /* EndIf */

    /*--------------------------------------------------------**
    ** If descriptor type is Configuration, the pointer is    **
    **    found in ptr1.  Other types of descriptor need      **
    **    to be found by stepping through the config one      **
    **    descriptor at a time.                               **
    ** To prevent parsing past the config buffer, ptr2 is     **
    **    set to the starting address plus its total size.    **
    **--------------------------------------------------------*/


    ptr1.pntr = dev_ptr->memlist;
    ptr1.word += MEM_HEADER_LEN; /* offset past the header */

    if (desc_type != DESCTYPE_CONFIG)
    {
        ptr2.word = ptr1.word + utoh16(ptr1.cfig->wTotalLength);

#define  HOSTDEV_GET_NEXT_DESCRIPTOR\
    ptr1.word += ptr1.cfig->bLength;    \
    if (ptr1.word >= ptr2.word) {       \
        USB_unlock();                    \
        return USBERR_NO_DESCRIPTOR;     \
    } /* Endif */

        switch (desc_type)
        {
        case DESCTYPE_INTERFACE:
            if (ptr1.cfig->bNumInterfaces < desc_index)
            {
                USB_unlock();
#ifdef _HOST_DEBUG_
                bbu_printf("_usb_hostdev_get_descriptor no INTF descriptor\r\n");
#endif
                return USBERR_NO_DESCRIPTOR;
            } /* Endif */

            do
            {
                HOSTDEV_GET_NEXT_DESCRIPTOR
            }
            while (
                (ptr1.intf->bDescriptorType != DESCTYPE_INTERFACE) ||
                (ptr1.intf->bInterfaceNumber != desc_index) ||
                (ptr1.intf->bAlternateSetting != intf_alt));

            dev_ptr->intf_recently = ptr1.intf; /* use for endpoints */
            break;
        case DESCTYPE_ENDPOINT:
            /* Endpoints must follow a known valid interface --
            ** the pointer cannot be NULL, and must point to
            ** the type & length for interface descriptors */
            ptr1.intf = dev_ptr->intf_recently;

            if (ptr1.intf == NULL)
            {
                USB_unlock();
#ifdef _HOST_DEBUG_
                bbu_printf("_usb_hostdev_get_descriptor no EP descriptor\r\n");
#endif
                return USBERR_NO_DESCRIPTOR;
            } /* Endif */

            if ((ptr1.intf->bDescriptorType != DESCTYPE_INTERFACE) ||
                    (ptr1.intf->bLength != 9) )
            {
                dev_ptr->intf_recently = NULL;
                USB_unlock();
#ifdef _HOST_DEBUG_
                bbu_printf("_usb_hostdev_get_descriptor no descriptor\r\n");
#endif
                return USBERR_NO_DESCRIPTOR;
            } /* EndIf */

            /* Endpoint 0 is NOT included in the count, and
            ** there is no descriptor for it after interface
            */
            if ((desc_index == 0) ||
                    (desc_index > ptr1.intf->bNumEndpoints))
            {
                USB_unlock();
#ifdef _HOST_DEBUG_
                bbu_printf("_usb_hostdev_get_descriptor no EP0 descriptor\r\n");
#endif
                return USBERR_NO_DESCRIPTOR;
            } /* Endif */

            do
            {
                HOSTDEV_GET_NEXT_DESCRIPTOR
                switch (ptr1.ndpt->bDescriptorType)
                {
                case DESCTYPE_ENDPOINT:
                    desc_index--;
                    break ;
                case DESCTYPE_INTERFACE:
#ifdef _HOST_DEBUG_
                    bbu_printf("_usb_hostdev_get_descriptor no INTF descriptor\r\n");
#endif
                    return USBERR_NO_DESCRIPTOR;
                default:
                    break ;
                } /* EndCase */
            }
            while (desc_index > 0);
            break;
        case DESCTYPE_QUALIFIER:
        case DESCTYPE_OTHER_SPEED:
        case DESCTYPE_INTF_POWER:
            do
            {
                HOSTDEV_GET_NEXT_DESCRIPTOR
            }
            while (ptr1.cfig->bDescriptorType != desc_type);
            break;
        case DESCTYPE_STRING:   /* Not in config, use Ch. 9 call */
        default:
            USB_unlock();
#ifdef _HOST_DEBUG_
            bbu_printf("_usb_hostdev_get_descriptor no STRING descriptor\r\n");
#endif
            return USBERR_NO_DESCRIPTOR;
        } /* EndSwitch */
    } /* EndIf */
#undef  HOSTDEV_GET_NEXT_DESCRIPTOR

    *descriptor = ptr1.pntr;
    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_get_descriptor SUCCESSFUL\r\n");
#endif
    return USB_OK;

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_hostdev_find_pipe_handle
* Returned Value : pipe handle, or NULL if not found
* Comments       :
*     Function to find a pipe with specified type and direction
*
*END*--------------------------------------------------------------------*/
_usb_pipe_handle  _usb_hostdev_find_pipe_handle
(
    /* [IN] the device instance */
    _usb_device_instance_handle      dev_handle,

    /* interface handle */
    _usb_interface_descriptor_handle intf_handle,

    /* Pipe type = one of:
    ** USB_ISOCHRONOUS_PIPE
    ** USB_INTERRUPT_PIPE
    ** USB_CONTROL_PIPE
    ** USB_BULK_PIPE
    */
    uint_8                           pipe_type,

    /* Pipe direction (ignored for control pipe) = one of:
    ** USB_RECV
    ** USB_SEND
    */
    uint_8                           pipe_direction
)
{
    /* Body */
    DESCRIPTOR_UNION           desc;
    DEV_INSTANCE_PTR           dev_ptr;
    DEV_MEMORY_PTR             dev_mem;
    PIPE_BUNDLE_STRUCT_PTR     pbs_ptr;
    _usb_pipe_handle           pipe_ptr;
    uint_8                     index;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_find_pipe_handle\r\n");
#endif

    USB_lock();
    /* Verify that the intf_handle points to an interface descriptor */
    desc.pntr = intf_handle;
    if ((desc.intf->bDescriptorType != DESCTYPE_INTERFACE)
            || (desc.intf->bLength != 9))
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_hostdev_find_pipe_handle invalid interface\r\n");
#endif
        return NULL;
    } /* Endif */

    /* Scan device instance for the interface */
    dev_ptr = (DEV_INSTANCE_PTR)dev_handle;

    /* The device may have several interfaces selected */
    for (index = 0; index < dev_ptr->num_of_interfaces; index++)
    {
        if (desc.intf == dev_ptr->intf_descriptor[index])
        {
            /* Bundle structs are linked into the memory list */
            dev_mem = dev_ptr->memlist;
            do
            {
                if (dev_mem->blktype == USB_MEMTYPE_PIPES)
                {
                    /* A bundle is owned by a device & interface */
                    pbs_ptr = (pointer)&dev_mem->payload;
                    if ((pbs_ptr->dev_handle == (pointer)dev_ptr)
                            && (pbs_ptr->intf_handle == desc.pntr))
                    {
                        pipe_ptr = usb_hostdev_get_pipe_handle(pbs_ptr,
                                                               pipe_type, pipe_direction);
                        USB_unlock();
#ifdef _HOST_DEBUG_
                        bbu_printf("_usb_hostdev_find_pipe_handle SUCCESSFUL\r\n");
#endif
                        return pipe_ptr;
                    } /* Endif */
                }
                else
                {
                    dev_mem = dev_mem->next;
                } /* EndIf */
            }
            while (dev_mem != NULL);
        } /* EndIf */
    } /* EndFor */

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_find_pipe_handle error\r\n");
#endif
    return NULL;

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_get_pipe_handle
* Returned Value : pipe handle, or NULL if none in bundle
* Comments       :
*     Function to map a particular pipe type and direction in
*     the given bundle (which contains device/interface).
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
_usb_pipe_handle  usb_hostdev_get_pipe_handle
(
    /* pointer to pipe bundle struct */
    PIPE_BUNDLE_STRUCT_PTR     pbs_ptr,

    /* Pipe type = one of:
    ** USB_ISOCHRONOUS_PIPE
    ** USB_INTERRUPT_PIPE
    ** USB_CONTROL_PIPE
    ** USB_BULK_PIPE
    */
    uint_8                     pipe_type,

    /* Pipe direction (ignored for control pipe) = one of:
    ** USB_RECV
    ** USB_SEND
    */
    uint_8                     pipe_direction
)
{
    /* Body */

    DESCRIPTOR_UNION           desc;
    PIPE_DESCRIPTOR_STRUCT_PTR pipe_ptr;
    uint_8                     m, n;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_get_pipe_handle\r\n");
#endif

    if (USB_OK != usb_hostdev_validate(pbs_ptr->dev_handle))
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_hostdev_get_pipe_handle invalid device handle\r\n");
#endif
        return NULL;

    }

    switch (pipe_type)
    {
    case USB_CONTROL_PIPE:
#ifdef _HOST_DEBUG_
        bbu_printf("usb_hostdev_get_pipe_handle SUCCESSFUL\r\n");
#endif
        return ((DEV_INSTANCE_PTR)pbs_ptr->dev_handle)->control_pipe;
    case USB_BULK_PIPE:
    case USB_INTERRUPT_PIPE:
    case USB_ISOCHRONOUS_PIPE:
        break;
    default:
#ifdef _HOST_DEBUG_
        bbu_printf("usb_hostdev_get_pipe_handle FAILED\r\n");
#endif
        return NULL;
    } /* EndSwitch */

    switch (pipe_direction)
    {
    case USB_RECV:
    case USB_SEND:
        break;
    default:
#ifdef _HOST_DEBUG_
        bbu_printf("usb_hostdev_get_pipe_handle invalid direction\r\n");
#endif
        return NULL;
    } /* EndSwitch */

    desc.intf = (INTERFACE_DESCRIPTOR_PTR)pbs_ptr->intf_handle;

    if ((desc.intf->bDescriptorType != DESCTYPE_INTERFACE) ||
            (desc.intf->bLength != 9))
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_hostdev_get_pipe_handle invalid interface\r\n");
#endif
        return NULL;
    } /* Endif */

    m = desc.intf->bNumEndpoints;

    for (n = 0 ;  n < m ;  n++)
    {
        pipe_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)
                   pbs_ptr->pipe_handle[n];
        if ((pipe_ptr->PIPETYPE == pipe_type) &&
                (pipe_ptr->DIRECTION == pipe_direction))
        {
#ifdef _HOST_DEBUG_
            bbu_printf("usb_hostdev_get_pipe_handle SUCCESSFUL\r\n");
#endif
            return (pointer)pipe_ptr;
        } /* Endif */
    } /* EndFor */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_get_pipe_handle FAILED\r\n");
#endif
    return NULL;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_class_intf_init
* Returned Value : USB_OK or error
* Comments       :
*     Initialize header of class interface info struct.
*     Incoming params are checked to catch errors early
*     before attempts to use the struct.
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_host_class_intf_init
(
    /* [IN] device/descriptor/pipe handles */
    PIPE_BUNDLE_STRUCT_PTR  pbs_ptr,

    /* [IN] class-interface handle */
    pointer                 class_intf_handle,

    /* [IN] address of class's instance list anchor */
    pointer                 anchor_ptr
)
{
    /* Body */

    DEV_INSTANCE_PTR              dev_ptr;
    GENERAL_CLASS_PTR             class_ptr = NULL;
    INTERFACE_DESCRIPTOR_PTR      intf_desc;
    DEV_MEMORY_PTR                mem_ptr;
    USB_STATUS                    error;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_class_intf_init\r\n");
#endif

    USB_lock();

    error = usb_hostdev_validate (pbs_ptr->dev_handle);

    if (error != USB_OK)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("usb_host_class_intf_init invalid device\r\n");
#endif
        return error;
    } /* EndIf */

    dev_ptr = (DEV_INSTANCE_PTR)pbs_ptr->dev_handle;
    intf_desc = (INTERFACE_DESCRIPTOR_PTR)pbs_ptr->intf_handle;

    if ((intf_desc->bDescriptorType != DESCTYPE_INTERFACE) ||
            (intf_desc->bLength != sizeof(INTERFACE_DESCRIPTOR)))
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("usb_host_class_intf_init interface not found\r\n");
#endif
        return USBERR_NO_INTERFACE;
    } /* EndIf */

    for (mem_ptr = dev_ptr->memlist; ; mem_ptr = mem_ptr->next)
    {

        if (mem_ptr == NULL)
        {
            USB_unlock();
#ifdef _HOST_DEBUG_
            bbu_printf("usb_host_class_intf_init no interface memory\r\n");
#endif
            return USBERR_NO_INTERFACE;
        } /* EndIf */

        if ((mem_ptr->blktype == USB_MEMTYPE_CLASS) &&
                ((pointer)&mem_ptr->payload == class_intf_handle))
        {
            break;
        } /* Endif */

    } /* EndFor */

    if (anchor_ptr == NULL)
    {
        USB_unlock ();
#ifdef _HOST_DEBUG_
        bbu_printf("usb_host_class_intf_init invalid parameter\r\n");
#endif
        return USBERR_INVALID_ANCHOR;
    } /* Endif */

    memset((pointer)class_intf_handle, 0, mem_ptr->blksize);

    /* Find last item already on list, add new item at end */
    class_ptr = (GENERAL_CLASS_PTR)anchor_ptr;

    while (class_ptr->next != NULL)
        class_ptr = class_ptr->next;

    class_ptr->next = (GENERAL_CLASS_PTR)class_intf_handle;

    /* Initialize header fields of new item */
    class_ptr = (GENERAL_CLASS_PTR)class_intf_handle;
    class_ptr->next = NULL;
    class_ptr->anchor = (GENERAL_CLASS_PTR)anchor_ptr;
    class_ptr->dev_handle = pbs_ptr->dev_handle;
    class_ptr->intf_handle = pbs_ptr->intf_handle;
    class_ptr->host_handle = dev_ptr->host;

    do
    {
        Class_Interface_Key++;
    }
    while (Class_Interface_Key == 0);

    class_ptr->key_code = Class_Interface_Key;

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_class_intf_init SUCCESSFUL\r\n");
#endif
    return USB_OK;
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_class_intf_validate
* Returned Value : uint_32, depends on coded_key in the parameter
*                    coded_key = 0, retrieve intf's key value
*                    coded key > 0, TRUE if intf OK to use
* Comments       :
*     This routine should be used by all classes using pointers
*     or other values kept in the struct:
*     USB_lock();
*     if (usb_host_class_intf_validate(class_intf_ptr) )
*        {
*           Issue commands / Use pipes etc.
*        }
*     USB_unlock();
*     It is presumed that this function is called with interrupts disabled
*
*END*--------------------------------------------------------------------*/
uint_32  usb_host_class_intf_validate
(
    /* [IN] class-interface handle */
    pointer  class_intf_handle
)
{
    /* Body */
    CLASS_CALL_STRUCT_PTR   cc_ptr;
    GENERAL_CLASS_PTR       gen_ptr;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_class_intf_validate\r\n");
#endif
    cc_ptr = (CLASS_CALL_STRUCT_PTR)class_intf_handle;
    gen_ptr = (GENERAL_CLASS_PTR)cc_ptr->class_intf_handle;

    /* Match key to caller's value */
    if (cc_ptr->code_key != 0 )
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_host_class_intf_validate SUCCESSFUL\r\n");
#endif
        return (cc_ptr->code_key == gen_ptr->key_code);
    }
    else
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_host_class_intf_validate SUCCESSFUL\r\n");
#endif
        return (cc_ptr->code_key = gen_ptr->key_code);
    } /* EndIf */

} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_driver_info_match
* Returned Value : TRUE for driver match, else FALSE
* Comments       :
*     Match device driver info with class etc. in device/interface
*     following the ordering in "USB Common Class Specification"
*     Rev. 1.0, August 18, 1998, p. 7
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static boolean  usb_host_driver_info_match
(
    /* [IN] USB device */
    DEV_INSTANCE_PTR           dev_ptr,

    /* [IN] Configuration number */
    INTERFACE_DESCRIPTOR_PTR   intf_ptr,

    /* [IN] TRUE=attach, FALSE=detach */
    USB_HOST_DRIVER_INFO_PTR   info_ptr
)
{
    /* Body */
    uint_16  info_Vendor, info_Product, dev_Vendor, dev_Product;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_driver_info_match\r\n");
#endif

    info_Vendor = utoh16(info_ptr->idVendor);
    info_Product = utoh16(info_ptr->idProduct);

    /* If vendor and product are listed in table (non-zero) */
    if ((info_Vendor | info_Product) != 0)
    {
        dev_Vendor = utoh16(dev_ptr->dev_descriptor.idVendor);
        dev_Product = utoh16(dev_ptr->dev_descriptor.idProduct);

        if ((info_Vendor == dev_Vendor) &&
                (info_Product == dev_Product))
        {
#ifdef _HOST_DEBUG_
            bbu_printf("usb_host_driver_info_match PID, VID match\r\n");
#endif
            return TRUE;
        } /* Endif */

        if (dev_ptr->dev_descriptor.bDeviceClass == 0xFF)
        {

            if ((info_Vendor == dev_Vendor) &&
                    (info_ptr->bDeviceSubClass ==
                     dev_ptr->dev_descriptor.bDeviceSubClass))
            {
#ifdef _HOST_DEBUG_
                bbu_printf("usb_host_driver_info_match VID, subclass match\r\n");
#endif
                return TRUE;
            } /* Endif */

        }
        else
        {
            if ((info_ptr->bDeviceClass ==
                    dev_ptr->dev_descriptor.bDeviceClass) &&
                    (info_ptr->bDeviceSubClass ==
                     dev_ptr->dev_descriptor.bDeviceSubClass))
            {
#ifdef _HOST_DEBUG_
                bbu_printf("usb_host_driver_info_match class, subclass match\r\n");
#endif
                return TRUE;
            }
        } /* Endif */

        /* No Device match, try Interface */
        if (intf_ptr->bInterfaceClass == 0xFF)
        {

            if ((info_Vendor == dev_Vendor) &&
                    (info_ptr->bDeviceSubClass ==
                     intf_ptr->bInterfaceSubClass))
            {
#ifdef _HOST_DEBUG_
                bbu_printf("usb_host_driver_info_match interface vid,class,subclass match\r\n");
#endif
                return TRUE;
            } /* Endif */

        } /* Endif */
    } /* Endif */

    /* No Vendor/Device matches, try Interface class */
    if (intf_ptr->bInterfaceClass != 0xFF)
    {
        if ((info_ptr->bDeviceClass == intf_ptr->bInterfaceClass) &&
                (info_ptr->bDeviceSubClass == intf_ptr->bInterfaceSubClass))
        {
#ifdef _HOST_DEBUG_
            bbu_printf("usb_host_driver_info_match interface class,subclass match\r\n");
#endif
            return TRUE;
        } /* Endif */
    } /* Endif */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_driver_info_match interface No Match\r\n");
#endif

    return FALSE;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_driver_info_scan
* Returned Value : none
* Comments       :
*     Call attach/detach callbacks for interfaces matching user-list items.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static void  usb_host_driver_info_scan
(
    /* [IN] USB device */
    DEV_INSTANCE_PTR           dev_ptr,

    /* [IN] Configuration number */
    INTERFACE_DESCRIPTOR_PTR   intf_ptr,

    /* [IN] event causing this call */
    uint_32                    event_code
)
{
    /* Body */
    boolean                          match, matched = FALSE;
    USB_HOST_DRIVER_INFO_PTR         info_ptr;
    USB_HOST_STATE_STRUCT_PTR        host_struct_ptr;
    _usb_device_instance_handle      dev_handle;
    _usb_interface_descriptor_handle intf_handle;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_driver_info_scan\r\n");
#endif

    if (dev_ptr->host == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_host_driver_info_scan NULL host pointer\r\n");
#endif
        return;

    }

    /* Get the host handle on which this device is on */
    host_struct_ptr = (USB_HOST_STATE_STRUCT_PTR)dev_ptr->host;

    /* Get the driver info for attach callback when a match occurs */
    info_ptr = (USB_HOST_DRIVER_INFO_PTR)\
               host_struct_ptr->DEVICE_INFO_TABLE;
    dev_handle = (_usb_device_instance_handle)dev_ptr;
    intf_handle = (_usb_interface_descriptor_handle)intf_ptr;

    if (info_ptr)
    {

        while (usb_host_driver_info_nonzero(info_ptr) == TRUE)
        {
            match = usb_host_driver_info_match(dev_ptr, intf_ptr, info_ptr);

            if (match == TRUE)
            {
                matched = TRUE;
                /* The match has occurred so notify the application of such
                ** interface
                */
                info_ptr->attach_call(dev_handle, intf_handle, event_code);
            } /* Endif */

            info_ptr++;
        } /* EndWhile */

        if (!matched)
        {
            /* None of the registered driver info matched so check if a
            ** default driver is installed
            */
            if (info_ptr->attach_call)
            {
                /* The match has not occurred so notify the application of such
                ** interface via the default callback function
                */
                info_ptr->attach_call(dev_handle, intf_handle, event_code);
            } /* Endif */
        } /* Endif */
    } /* Endif */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_driver_info_scan SUCCESSFUL\r\n");
#endif

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_driver_info_nonzero
* Returned Value : none
* Comments       :
*     Function to detect the all-zero end-of-list item.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static boolean  usb_host_driver_info_nonzero
(
    /* [IN] USB device */
    USB_HOST_DRIVER_INFO_PTR           info_ptr
)
{
    /* Body */
#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_driver_info_nonzero\r\n");
#endif

    if (info_ptr->bDeviceClass != 0)
        return TRUE;

    if (info_ptr->bDeviceProtocol != 0)
        return TRUE;

    if (info_ptr->bDeviceSubClass != 0)
        return TRUE;

    if (info_ptr->idVendor[0] != 0)
        return TRUE;

    if (info_ptr->idVendor[1] != 0)
        return TRUE;

    if (info_ptr->idProduct[0] != 0)
        return TRUE;

    if (info_ptr->idProduct[1] != 0)
        return TRUE;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_driver_info_nonzero FAILURE\r\n");
#endif

    return FALSE;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_map_class_intf
* Returned Value : pointer to matching class map item, or NULL
* Comments       :
*           Map interface class etc. to a class driver table item.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static CLASS_MAP_PTR  usb_host_map_class_intf
(
    /* interface's device/descriptor/pipe bundle */
    PIPE_BUNDLE_STRUCT_PTR  pbs_ptr
)
{
    /* Body */
    CLASS_MAP_PTR              map_ptr;
    INTERFACE_DESCRIPTOR_PTR   intf_desc;
    uint_32                    intf_class, map_class;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_map_class_intf\r\n");
#endif

    intf_desc = (INTERFACE_DESCRIPTOR_PTR)pbs_ptr->intf_handle;

    for (map_ptr = &class_interface_map[0]; ; map_ptr++)
    {
        if (map_ptr->class_init == NULL)
        {
            map_ptr = NULL;
            break;
        } /* EndIf */
        intf_class = usb_hostdev_mask_class_etc(&intf_desc->bInterfaceClass,
                                                &map_ptr->class_mask);

        map_class = usb_hostdev_mask_class_etc(&map_ptr->class,
                                               &map_ptr->class_mask);

        if (intf_class != 0)
            if (intf_class == map_class)
                break;

    } /* EndFor */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_host_map_class_intf SUCCESSFUL\r\n");
#endif

    return map_ptr;

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_host_driver_info_register
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to register user's desired-interfaces array of structs.
*
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_host_driver_info_register
(
    /* usb host */
    _usb_host_handle     host_handle,

    /* Device info table */
    pointer              info_table_ptr
)
{
    /* Body */

    USB_HOST_STATE_STRUCT_PTR host_struct_ptr;
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_driver_info_register\r\n");
#endif

    USB_lock();
    if (host_handle == NULL || info_table_ptr == NULL)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_host_driver_info_register, NULL pointers\r\n");
#endif
        return USBERR_UNKNOWN_ERROR;
    } /* Endif */

    host_struct_ptr = (USB_HOST_STATE_STRUCT_PTR)host_handle;
    host_struct_ptr->DEVICE_INFO_TABLE = info_table_ptr;

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_host_driver_info_register SUCCESSFUL\r\n");
#endif
    return USB_OK;
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_hostdev_select_config
* Returned Value : USB_OK, or error status
* Comments       :
*     Function to tear down an old configuration, and set up
*     a new configuration with the same or different index.
*
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_hostdev_select_config
(
    /* [IN] USB device */
    _usb_device_instance_handle   dev_handle,

    /* [IN] Configuration number */
    uint_8                        config_no
)
{
    /* Body */
    DEV_INSTANCE_PTR        dev;
    USB_STATUS              error;

    USB_lock();

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_select_config");
#endif

    error = usb_hostdev_validate(dev_handle);
    if (error != USB_OK)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_hostdev_select_config invalid device");
#endif
        return error;
    } /* Endif */

    dev = (DEV_INSTANCE_PTR)dev_handle;

    if (config_no > dev->dev_descriptor.bNumConfigurations)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("_usb_hostdev_select_config invalid config no.");
#endif
        return USBERR_INVALID_CFIG_NUM;
    } /* Endif */

    if (dev->memlist != NULL)
        usb_dev_list_free_memory(dev_handle);

    dev->cfg_value = config_no;   /* requested config. number */
    dev->new_config = 1;          /* new config flag */
    dev->state = DEVSTATE_SET_CFG;
    _usb_host_ch9_set_configuration((pointer)dev, dev->cfg_value);

    USB_unlock();

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_select_config SUCCESSFUL");
#endif
    return USB_OK;

} /* EndBody */



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_delete_interface
* Returned Value : none
* Comments       :
*     Function to tear down an existing interface.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
void  usb_hostdev_delete_interface
(
    /* [IN] USB device */
    DEV_INSTANCE_PTR         dev,

    /* [IN] Interface */
    INTERFACE_DESCRIPTOR_PTR intf
)
{
    /* Body */
    DESCRIPTOR_UNION         desc;
    DEV_MEMORY_PTR           mem1, mem2;
    GENERAL_CLASS_PTR        class_ptr1, class_ptr2;
    PIPE_BUNDLE_STRUCT_PTR   pbs_ptr;
    uint_8                   intf_no;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_delete_interface\r\n");
#endif

    for (intf_no = 0; intf_no < dev->num_of_interfaces; intf_no++)
    {
        desc.intf = dev->intf_descriptor[intf_no];

        if (desc.intf == NULL)
            continue;

        /* Equal interface numbers (alternates don't count here) */
        if (intf->bInterfaceNumber != desc.intf->bInterfaceNumber)
            continue;

        /* Find pipe bundle for this interface on memory list */
        for (mem1 = dev->memlist; mem1 != NULL; mem1 = mem1->next)
        {
            if (mem1->blktype != USB_MEMTYPE_PIPES)
                continue;
            pbs_ptr = (pointer)&mem1->payload;

            if (intf != pbs_ptr->intf_handle)
                continue;

            /* Unlink pipe bundle from memory list */
            mem2 = dev->memlist;
            if (mem1 == mem2)
            {
                dev->memlist = mem1->next;
            }
            else
            {
                while (mem2->next != mem1)
                    mem2 = mem2->next;
                mem2->next = mem1->next;
            } /* EndIf */

            /* Close pipes, free memory list item */
            usb_dev_list_close_pipe_bundle(pbs_ptr);
            USB_memfree(mem1);
            dev->intf_descriptor[intf_no] = NULL;
            intf_no = dev->num_of_interfaces;
            break;
        } /* EndFor */

        /* If associated audio interface, don't delete class struct */
        if (usb_hostdev_keep_class(desc.intf))
        {
#ifdef _HOST_DEBUG_
            bbu_printf("usb_hostdev_delete_interface audio interface\r\n");
#endif
            return;
        }

        /* Find class-interface on memory list */
        for (mem1 = dev->memlist; mem1 != NULL; mem1 = mem1->next)
        {
            if (mem1->blktype != USB_MEMTYPE_CLASS)
                continue;
            class_ptr1 = (pointer)&mem1->payload;
            if (intf != class_ptr1->intf_handle)
                continue;

            /* Invalidate class_intf */
            class_ptr1->key_code = 0;
            /* Unlink class-intf from memory list */
            mem2 = dev->memlist;
            if (mem1 == mem2)
            {
                dev->memlist = mem1->next;
            }
            else
            {

                while (mem2->next != mem1)
                    mem2 = mem2->next;

                mem2->next = mem1->next;
            } /* EndIf */

            /* Unlink class-intf from class's list */
            class_ptr2 = class_ptr1->anchor; /* A(anchor) */
            if (class_ptr2->next == class_ptr1)
            {
                class_ptr2->next = class_ptr1->next;
            }
            else
            {
                while (class_ptr2->next != class_ptr1)
                    class_ptr2 = class_ptr2->next;
                class_ptr2->next = class_ptr1->next;
            } /* EndIf */

            /* Free memory list item */
            USB_memfree(mem1);
            intf_no = dev->num_of_interfaces;
            break;
        } /* EndFor */

    } /* EndFor */
#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_delete_interface SUCCESSFUL\r\n");
#endif

} /* EndBody */



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_mask_class_etc
* Returned Value : masked class/sub-class/protocol
* Comments       :
*     It is assumed that class, sub-class, and protocol are
*     successive uint-8 values (as in a USB descriptor),
*     and likewise the masks to be ANDed with them.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static uint_32  usb_hostdev_mask_class_etc
(
    /* [IN] pointer to class etc. */
    uint_8_ptr      csp_ptr,

    /* [IN] pointer to masks */
    uint_8_ptr      msk_ptr
)
{
    /* Body */

    uint_32  result;
#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_mask_class_etc\r\n");
#endif

    /* MSB of result = class */
    result = (*csp_ptr) & (*msk_ptr);

    /* 2nd byte of result = sub-class */
    csp_ptr++;
    msk_ptr++;
    result = (result << 8) | ((*csp_ptr) & (*msk_ptr));

    /* LSB of result = sub-class */
    csp_ptr++;
    msk_ptr++;
    result = (result << 8) | ((*csp_ptr) & (*msk_ptr));

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_mask_class_etc SUCCESSFUL\r\n");
#endif

    return result;

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_open_pipe
* Returned Value : pipe handle + USB_OK, or error status
* Comments       :
*     Function to open a pipe for an endpoint.  Note that endpoints
*     following an interface are numbered 1, 2, 3, ... bNumEndpoints,
*     where the number of endpoints is in the interface descriptor.
*  It is presumed that this function is called with USB interrupts disabled
*
*END*--------------------------------------------------------------------*/
static USB_STATUS  usb_hostdev_open_pipe
(
    /* [IN] USB device */
    _usb_device_instance_handle    dev_handle,

    /* [IN] Only FLAG=1 is presently defined (= dont't send empty packet) */
    uint_32                        flags,

    /* [IN] Endpoint number 0, 1, ... N */
    uint_8                         ep_num,

    /* [OUT] Handle of opened pipe */
    _usb_pipe_handle _PTR_         pipe_handle_ptr
)
{
    /* Body */

    DESCRIPTOR_UNION         desc;
    DEV_INSTANCE_PTR         dev;
    PIPE_INIT_PARAM_STRUCT   pipe_init_params;
    USB_STATUS               error;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_open_pipe\r\n");
#endif

    error = _usb_hostdev_get_descriptor(dev_handle,
                                        DESCTYPE_ENDPOINT,
                                        ep_num,
                                        0,
                                        (pointer)&desc);

    if (error != USB_OK)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_hostdev_open_pipe failed\r\n");
#endif
        return error;
    }

    dev = (DEV_INSTANCE_PTR)dev_handle;

    pipe_init_params.DEV_INSTANCE = dev_handle;
    pipe_init_params.INTERVAL = desc.ndpt->iInterval;
    pipe_init_params.MAX_PACKET_SIZE =
        utoh16(desc.ndpt->wMaxPacketSize) & PACKET_SIZE_MASK;
    pipe_init_params.NAK_COUNT = DEFAULT_MAX_NAK_COUNT;
    pipe_init_params.FIRST_FRAME =
        pipe_init_params.FIRST_UFRAME = 0;
    pipe_init_params.FLAGS = flags;
    pipe_init_params.DEVICE_ADDRESS = ((DEV_INSTANCE_PTR)dev_handle)->address;
    pipe_init_params.ENDPOINT_NUMBER =
        desc.ndpt->bEndpointAddress & ENDPOINT_MASK;
    pipe_init_params.DIRECTION =
        (desc.ndpt->bEndpointAddress & IN_ENDPOINT) ?
        USB_RECV : USB_SEND;

    switch (desc.ndpt->bmAttributes & EP_TYPE_MASK)
    {
    case ISOCH_ENDPOINT:
        pipe_init_params.PIPETYPE = USB_ISOCHRONOUS_PIPE;
        break;
    case BULK_ENDPOINT:
        pipe_init_params.PIPETYPE = USB_BULK_PIPE;
        break;
    case IRRPT_ENDPOINT:
        pipe_init_params.PIPETYPE = USB_INTERRUPT_PIPE;
        break;
    default:
        pipe_init_params.PIPETYPE = 0;
        break;
    } /* EndSwitch */

    pipe_init_params.SPEED = dev->speed;
    pipe_init_params.TRS_PER_UFRAME = 1;
    if ((pipe_init_params.PIPETYPE == USB_ISOCHRONOUS_PIPE) ||
            (pipe_init_params.PIPETYPE == USB_INTERRUPT_PIPE))
    {
        switch (utoh16(desc.ndpt->wMaxPacketSize) & ADDITIONAL_MASK)
        {
        case TWO_ADDITIONAL:
            pipe_init_params.TRS_PER_UFRAME++;
        case ONE_ADDITIONAL:
            pipe_init_params.TRS_PER_UFRAME++;
            break;
        default:
            break;
        } /* EndSwitch */
    } /* Endif */

    error = _usb_host_open_pipe(dev->host,
                                &pipe_init_params,
                                pipe_handle_ptr);

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_open_pipe SUCCESSFUL\r\n");
#endif

    return error;

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_hostdev_select_interface
* Returned Value : interface struct + USB_OK, or error status
* Comments       :
*     Function to tear down old interface, and set up
*     a new one with the same or different index/alternate.
*     For each interface, only one of its alternates
*     can be selected per USB spec.  Also per spec, an endpoint
*     can be in only one interface (though it may exist in
*     several alternates of that interface).
*
*END*--------------------------------------------------------------------*/
USB_STATUS  _usb_hostdev_select_interface
(
    /* [IN] USB device */
    _usb_device_instance_handle      dev_handle,

    /* [IN] Interface to be selected */
    _usb_interface_descriptor_handle intf_handle,

    /* [OUT] Initialized class-specific interface struct */
    pointer                          class_intf_ptr
)
{
    /* Body */
    CLASS_CALL_STRUCT_PTR         class_ptr = NULL;
    DESCRIPTOR_UNION              desc;
    DEV_INSTANCE_PTR              dev;
    USB_HOST_DRIVER_INFO_PTR      info_ptr;
    USB_HOST_STATE_STRUCT_PTR     host_struct_ptr;
    DEV_MEMORY_PTR                mem1, mem2;
    INTERFACE_DESCRIPTOR_PTR      intf;
    CLASS_MAP_PTR                 class_map;
    _usb_pipe_handle              pipe_handle;
    PIPE_BUNDLE_STRUCT_PTR        pbs_ptr;
    USB_STATUS                    error;
    uint_16                       block_size;
    uint_8                        ep_num, intf_no;

#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_select_interface\r\n");
#endif

    USB_lock();
    error = usb_hostdev_validate (dev_handle);

    if (error != USB_OK)
        goto BadExit;

    dev = (DEV_INSTANCE_PTR)dev_handle;
    intf = (INTERFACE_DESCRIPTOR_PTR)intf_handle;

    /* Get the host handle on which this device is on */
    host_struct_ptr = (USB_HOST_STATE_STRUCT_PTR)dev->host;

    /* Get the driver info for attach callback when a match occurs */
    info_ptr = (USB_HOST_DRIVER_INFO_PTR)\
               host_struct_ptr->DEVICE_INFO_TABLE;

    if ((intf->bDescriptorType != DESCTYPE_INTERFACE) ||
            (intf->bLength != sizeof(INTERFACE_DESCRIPTOR)))
    {
        error = USBERR_NO_INTERFACE;
        goto BadExit;
    } /* EndIf */

    /* If an interface was previously selected, tear it down */
    usb_hostdev_delete_interface(dev, intf);

    /* Check that there is an available interface pointer */
    for (intf_no = 0; ; intf_no++)
    {
        if (intf_no >= dev->num_of_interfaces)
        {
            error = USBERR_NO_INTERFACE;
            goto BadExit;
        } /* EndIf */

        if (dev->intf_descriptor[intf_no] == NULL)
            break;
    } /* EndFor */

    /* Create pipe bundle for memory list */
    mem1 = dev->memlist;

    if (mem1 == NULL)   /* Must be at least config on list */
    {
        error = USBERR_NO_INTERFACE;
        goto BadExit;
    } /* EndIf */

    while (mem1->next != NULL) /* Scan to last existing item */
        mem1 = mem1->next;

    block_size = sizeof(PIPE_BUNDLE_STRUCT) +
                 sizeof(_usb_pipe_handle) * (intf->bNumEndpoints - 1);

    error = usb_dev_list_get_memory(dev,
                                    block_size,
                                    USB_MEMTYPE_PIPES,
                                    (pointer)&mem2);

    if (error != USB_OK)
    {
        error = USBERR_GET_MEMORY_FAILED;
        goto BadExit;
    } /* EndIf */

    /* Locate interface/alternate in devices configuration */
    error = _usb_hostdev_get_descriptor(dev_handle,
                                        DESCTYPE_INTERFACE,
                                        intf->bInterfaceNumber,
                                        intf->bAlternateSetting,
                                        (pointer)&desc);

    if (error != USB_OK)
        goto BadExit;

    /* Locate each endpoint descriptor, open a pipe for it */
    pbs_ptr = (pointer)&mem2->payload;
    for (ep_num = 1; ep_num <= intf->bNumEndpoints; ep_num++)
    {
        error = usb_hostdev_open_pipe (dev_handle,
                                       0, ep_num, &pipe_handle);

        /* If pipe not OK, unlink bundle from list and delete it */
        if (error != USB_OK)
        {
            mem1->next = NULL;  /* snip off list */
            USB_memfree (mem2);
            goto BadExit;
        } /* EndIf */
        pbs_ptr->pipe_handle[ep_num - 1] = pipe_handle;
    } /* EndFor */

    pbs_ptr->dev_handle = dev_handle;
    pbs_ptr->intf_handle = intf_handle;
    dev->intf_descriptor[intf_no] = intf;

    if (usb_host_driver_info_nonzero(info_ptr))
    {

        /* Map interface to class driver, get & initialize driver struct */
        error = USBERR_NO_DEVICE_CLASS;  /* Assume the worst */
        class_ptr = (CLASS_CALL_STRUCT_PTR)class_intf_ptr;

        if (class_ptr == NULL)
            goto BadExit;

        class_map = usb_host_map_class_intf(pbs_ptr);

        if (class_map == NULL)
            goto BadExit;

        /* Don't get new struct for associated interfaces */
        if (!usb_hostdev_keep_class(intf))
        {
            error = usb_hostdev_get_class (dev_handle,
                                           class_map->intf_struct_size,
                                           (pointer)&class_ptr->class_intf_handle);

            if (error != USB_OK)
                goto BadExit;
        } /* EndIf */

        class_map->class_init(pbs_ptr, class_ptr);

        if (class_ptr->code_key == 0)
            goto BadExit;

    } /* Endif */

    /* Set interface on the device */
    dev->state = DEVSTATE_SET_INTF;
    _usb_host_ch9_set_interface((pointer)dev, intf->bAlternateSetting,
                                intf->bInterfaceNumber);

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_select_interface SUCESSFUL\r\n");
#endif
    return USB_OK;

BadExit:
    if (!error) error = USBERR_NO_DEVICE_CLASS;
    class_ptr->class_intf_handle = NULL;
    class_ptr->code_key = 0;
    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("_usb_hostdev_select_interface bad exit\r\n");
#endif
    return error;

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hostdev_tr_init
* Returned Value : Initialize TR with globally unique index
* Comments       :
*     To ensure that a new TR_STRUCT is used by the system for
*     each new transaction, it is essential to have a distinct
*     TR_INDEX value.  Completed transactions have TR_INDEX
*     cleared to zero, it is invalid for a new transaction.
*
*END*--------------------------------------------------------------------*/
void usb_hostdev_tr_init
(
    /* [OUT] initialized TR_INIT_PARAM_STRUCT */
    TR_INIT_PARAM_STRUCT_PTR   tr_ptr,

    /* [IN] callback to call after completion */
    tr_callback                callback,

    /* [IN] the param to pass back to the callback function */
    pointer                    callback_param
)
{
    /* Body */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_tr_init\r\n");
#endif
    USB_lock();

    do
    {
        tr_prototype.TR_INDEX++;
    }
    while (tr_prototype.TR_INDEX == 0);

    *tr_ptr = tr_prototype;
    tr_ptr->CALLBACK = callback;
    tr_ptr->CALLBACK_PARAM = callback_param;

    USB_unlock();
#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_tr_init SUCCESSFUL\r\n");
#endif
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_hostdev_get_speed
* Returned Value : Device speed
* Comments       :
*     Return USB_SPEED_LOW, USB_SPEED_HIGH or USB_SPEED_FULL
*
*END*--------------------------------------------------------------------*/

uint_32 _usb_hostdev_get_speed
(
    /* [IN] USB device */
    _usb_device_instance_handle      dev_handle
)
{
    /* Body */
#ifdef _HOST_DEBUG_
    bbu_printf("usb_hostdev_tr_init SUCCESSFUL\r\n");
#endif
    return ((DEV_INSTANCE_PTR)dev_handle)->speed;
} /* Endbody */


/* EOF */
