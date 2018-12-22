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
***   chip.
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
*  Function Name  : _usb_hci_vusb20_bus_control
*  Returned Value : None
*  Comments       :
*        Bus control
*END*-----------------------------------------------------------------*/

void _usb_hci_vusb20_bus_control
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle,

    /* The operation to be performed on the bus */
    uint_8                  bControl
)
{
    /* Body */
    //USB_HOST_STATE_STRUCT_PTR usb_host_ptr;

    // usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    switch (bControl)
    {
    case USB_ASSERT_BUS_RESET:
        break;
    case USB_ASSERT_RESUME:
        break;
    case USB_SUSPEND_SOF:
        _usb_ehci_bus_suspend(handle);
        break;
    case USB_DEASSERT_BUS_RESET:
        _usb_ehci_bus_reset(handle);
        break;
    case USB_RESUME_SOF:
        _usb_ehci_bus_resume(handle);
        break;
    case USB_DEASSERT_RESUME:
    default:
        break;
    } /* EndSwitch */
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_bus_suspend
*  Returned Value : None
*  Comments       :
*        Suspend the bus
*END*-----------------------------------------------------------------*/

void _usb_ehci_bus_suspend
(
    /* [IN] the USB Host state structure */
    _usb_host_handle              handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    VUSB20_REG_STRUCT_PTR                        dev_ptr;
    VUSB20_REG_STRUCT_PTR                        cap_dev_ptr;
    uint_8                                       i, total_port_numbers;
    uint_32                                      port_control;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    cap_dev_ptr = (VUSB20_REG_STRUCT_PTR)
                  _bsp_get_usb_capability_register_base(usb_host_ptr->DEV_NUM);

    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    total_port_numbers =
        (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS &
         EHCI_HCS_PARAMS_N_PORTS);

    USB_lock();


    /* Suspend all ports */
    for (i = 0; i < total_port_numbers; i++)
    {
        port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i];

        if (port_control & EHCI_PORTSCX_PORT_ENABLE)
        {

            port_control = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0];
            port_control &= ~EHCI_PORTSCX_W1C_BITS;

            dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i] =
                (port_control | EHCI_PORTSCX_PORT_SUSPEND);
        } /* Endif */

    } /* Endfor */

    /* Stop the controller
    SGarg: This should not be done. If the controller is stopped, we will
    get no attach or detach interrupts and this will stop all operatings
    including the OTG state machine which is still running assuming that
    Host is alive.

    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD &=
       ~EHCI_CMD_RUN_STOP;
    */

    USB_unlock();

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_bus_resume
*  Returned Value : None
*  Comments       :
*        Resume the bus
*END*-----------------------------------------------------------------*/

void _usb_ehci_bus_resume
(
    /* [IN] the USB Host state structure */
    _usb_host_handle              handle

)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    VUSB20_REG_STRUCT_PTR                        dev_ptr;
    VUSB20_REG_STRUCT_PTR                        cap_dev_ptr;
    uint_8                                       i, total_port_numbers;
    uint_32                                      port_control;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    cap_dev_ptr = (VUSB20_REG_STRUCT_PTR)
                  _bsp_get_usb_capability_register_base(usb_host_ptr->DEV_NUM);
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    total_port_numbers =
        cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS &
        EHCI_HCS_PARAMS_N_PORTS;

    USB_lock();


    /* Resume all ports */
    for (i = 0; i < total_port_numbers; i++)
    {
        port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i];

        if (port_control & EHCI_PORTSCX_PORT_ENABLE)
        {
            port_control &= ~EHCI_PORTSCX_W1C_BITS;
            dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i] =
                (port_control | EHCI_PORTSCX_PORT_FORCE_RESUME);
        } /* Endif */

    } /* Endfor */

    /*
     S Garg: This should not be done. See comments in suspend.
       Restart the controller
       dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD |=
       EHCI_CMD_RUN_STOP;
      */

    USB_unlock();

} /* EndBody */

/* EOF */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_bus_reset
*  Returned Value : None
*  Comments       :
*        Resume the bus
*END*-----------------------------------------------------------------*/

void _usb_ehci_bus_reset
(
    /* [IN] the USB Host state structure */
    _usb_host_handle              handle


)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    VUSB20_REG_STRUCT_PTR                        dev_ptr;
    VUSB20_REG_STRUCT_PTR                        cap_dev_ptr;
    uint_8                                       i, total_port_numbers;
    uint_32                                      port_control;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    cap_dev_ptr = (VUSB20_REG_STRUCT_PTR)
                  _bsp_get_usb_capability_register_base(usb_host_ptr->DEV_NUM);
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    total_port_numbers =
        cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS &
        EHCI_HCS_PARAMS_N_PORTS;

    USB_lock();


    /* Resume all ports */
    for (i = 0; i < total_port_numbers; i++)
    {
        port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i];

        if (port_control & EHCI_PORTSCX_PORT_ENABLE)
        {
            port_control &= ~EHCI_PORTSCX_W1C_BITS;
            dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i] =
                (port_control | EHCI_PORTSCX_PORT_RESET);
        } /* Endif */

    } /* Endfor */

    /*
     S Garg: This should not be done. See comments in suspend.
       Restart the controller
       dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD |=
       EHCI_CMD_RUN_STOP;
      */

    USB_unlock();

} /* EndBody */

/* EOF */











