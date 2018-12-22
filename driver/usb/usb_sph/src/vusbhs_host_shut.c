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
***   This file contains the low-level Host API functions specific to the VUSB
***   chip for shutting down the host controller .
***
**************************************************************************
**END*********************************************************/
#include "hostapi.h"
#include "usbprv_host.h"

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif


#ifndef __USB_OS_MQX__
extern boolean IN_ISR;
#endif

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hci_vusb20_shutdown
*  Returned Value : None
*  Comments       :
*     Shutdown and de-initialize the VUSB1.1 hardware
*
*END*-----------------------------------------------------------------*/

void _usb_hci_vusb20_shutdown
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR         usb_host_ptr;
    VUSB20_REG_STRUCT_PTR             dev_ptr;
    ACTIVE_QH_MGMT_STRUCT_PTR         active_list_member_ptr, temp_ptr = NULL;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    dev_ptr      = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    /* Disable interrupts */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR &=
        ~VUSB20_HOST_INTR_EN_BITS;

    /* Stop the controller */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD &=
        ~EHCI_CMD_RUN_STOP;

    /* Reset the controller to get default values */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD = EHCI_CMD_CTRL_RESET;

    /**********************************************************
    ensure that periodic list in uninitilized.
    **********************************************************/
    usb_host_ptr->ITD_LIST_INITIALIZED = FALSE;
    usb_host_ptr->SITD_LIST_INITIALIZED = FALSE;
    usb_host_ptr->PERIODIC_LIST_INITIALIZED = FALSE;
    usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR = NULL;

    /**********************************************************
    Free all memory occupied by active transfers
    **********************************************************/
    active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
    while (active_list_member_ptr)
    {
        temp_ptr =  active_list_member_ptr;
        active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                                 active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
        USB_memfree(temp_ptr);

    }

    active_list_member_ptr = usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;
    while (active_list_member_ptr)
    {
        temp_ptr =  active_list_member_ptr;
        active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                                 active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
        USB_memfree(temp_ptr);

    }

    /* Free all controller specific memory */
    USB_memfree((pointer)usb_host_ptr->CONTROLLER_MEMORY);

#if 0
    /* Free all controller specific memory */
    USB_memfree((pointer)usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR);
    USB_memfree((pointer)usb_host_ptr->ASYNC_LIST_BASE_ADDR);
    USB_memfree((pointer)usb_host_ptr->ITD_BASE_PTR);
    USB_memfree((pointer)usb_host_ptr->ITD_SCRATCH_STRUCT_BASE);
    USB_memfree((pointer)usb_host_ptr->SITD_BASE_PTR);
    USB_memfree((pointer)usb_host_ptr->QTD_BASE_PTR);
    USB_memfree((pointer)usb_host_ptr->QTD_SCRATCH_STRUCT_BASE);
    USB_memfree((pointer)usb_host_ptr->SITD_SCRATCH_STRUCT_BASE);
    USB_memfree((pointer)usb_host_ptr->PERIODIC_LIST_BASE_ADDR);
    USB_memfree((pointer)usb_host_ptr->QH_SCRATCH_STRUCT_BASE);
#endif

} /* EndBody */

/* EOF */









