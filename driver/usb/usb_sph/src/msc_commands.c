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
***   This file contains device driver for mass storage class. This code tests
***   the UFI set of commands.
***
**************************************************************************
**END*********************************************************/

/**************************************************************************
Include the OS and BSP dependent files that define IO functions and
basic types. You may like to change these files for your board and RTOS
**************************************************************************/
/**************************************************************************
Include the USB stack header files.
**************************************************************************/
//#include "types.h"
#include "usb_types.h"
#ifdef __USB_OTG__
#include "otgapi.h"
#include "usb_devapi.h"
#else
#include "hostapi.h"
#endif

//#include "BU_interrupt.h"

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#include "bsp.h"
#include "fio.h"
#else
#ifndef __NO_SETJMP
#include "stdio.h"
#endif
#include "stdlib.h"
#endif
#include "usbprv_host.h"
//#include "BU_Interrupt.h"
uint_8 VENDOR[2];
/**************************************************************************
Local header files for this application
**************************************************************************/
#include "host_common.h"
#ifdef __USB_OTG__
#include "otgmain.h"
#endif
//#include "usb_main.h"
//#include "usbprv_dev.h"
//#include "define.h"
#include "msc_commands.h"
/**************************************************************************
Class driver files for this application
**************************************************************************/

#include "usb_mass_bo.h"
#include "usb_mass_ufi.h"
//#include "EFSL/Ls.h"
//#include "EFSL/efs.h"
#include "interrupt.h"

#include "USB_HSIC_PHY.h"
#include "USB.h"
#include "usb_hsic.h"
#include "ic_usb.h"
#include "usb_test_app.h"

#include "clock.h"

//extern void * USB_memalloc(uint_32 n);
extern volatile boolean Host_inited;
//#define   USB_PRIORITY    15
extern volatile boolean test_flag;

/**************************************************************************
A driver info table defines the devices that are supported and handled
by file system application. This table defines the PID, VID, class and
subclass of the USB device that this application listens to. If a device
that matches this table entry, USB stack will generate a attach callback.
As noted, this table defines a UFI class device and a USB
SCSI class device (e.g. high-speed hard disk) as supported devices.
see the declaration of structure USB_HOST_DRIVER_INFO for details
or consult the software architecture guide.
**************************************************************************/

#ifndef  __USB_OTG__
static  USB_HOST_DRIVER_INFO DriverInfoTable[] =
{

    /* Floppy drive */
    {
        {0x00, 0x00},                 /* Vendor ID per USB-IF             */
        {0x00, 0x00},                 /* Product ID per manufacturer      */
        USB_CLASS_MASS_STORAGE,       /* Class code                       */
        USB_SUBCLASS_MASS_UFI,        /* Sub-Class code                   */
        USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_mass_device_event    /* Application call back function   */
    },

    /* USB 2.0 hard drive */
    {

        {0x49, 0x0D},                 /* Vendor ID per USB-IF             */
        {0x00, 0x30},                 /* Product ID per manufacturer      */
        USB_CLASS_MASS_STORAGE,       /* Class code                       */
        USB_SUBCLASS_MASS_SCSI,       /* Sub-Class code                   */
        USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_mass_device_event    /* Application call back function   */
    },

    {
        {0x00, 0x00},                 /* All-zero entry terminates        */
        {0x00, 0x00},                 /*    driver info list.             */
        0,
        0,
        0,
        0,
        NULL
    }

};
#else

extern USB_HOST_DRIVER_INFO DriverInfoTable[];


#endif
/**************************************************************************
   Global variables
**************************************************************************/
volatile DEVICE_STRUCT       mass_device;// = { 0 };   /* mass storage device struct */
volatile boolean bCallBack = FALSE;
volatile USB_STATUS bStatus       = USB_OK;
volatile uint_32  dBuffer_length = 0;
boolean  Attach_Notification = FALSE;
_usb_host_handle        host_handle;         /* global handle for calling host   */


/* the following is the mass storage class driver object structure. This is
used to send commands down to  the class driver. See the Class API document
for details */

COMMAND_OBJECT_PTR pCmd, pCmd1;

/*some handles for communicating with USB stack */
_usb_host_handle     host_handle; /* host controller status etc. */
extern boolean _usb_ehci_process_port_change
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle
);
/**************************************************************************
The following is the way to define a multi tasking system in MQX RTOS.
Remove this code and use your own RTOS way of defining tasks (or threads).
**************************************************************************/

#ifdef __USB_OS_MQX__
#define MAIN_TASK          (10)
//extern void Main_Task(uint_32 param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK,      Main_Task,      3000L,  7L, "Main",      MQX_AUTO_START_TASK},
    { 0L,             0L,             0L,   0L, 0L,          0L }
};

#endif




/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*----------------------------------------------------------------------*/
int alloc_size;
void (*usb_otg_host_ISR_Handle)(void);

void (*usb_sph_host_ISR_Handle)(void);

void (*usb_ic_usb_host_ISR_Handle)(void);
#if 0
void usb_otg_host_isr(void *usb)
{
    usb = usb;
    if ( usb_otg_host_ISR_Handle )
        (usb_otg_host_ISR_Handle)();
}
void usb_otg_host_isr_enable(void)
{
    usb_otg_host_ISR_Handle = (void (*)())_usb_hci_vusb20_isr;

    ISR_Connect(IRQ_MMP_USB, usb_otg_host_isr, NULL);
    INT_Enable(IRQ_MMP_USB,  0,  0xf);//enable USB interrupt
}
void usb_otg_host_isr_disable(void)
{
    ISR_Disconnect(IRQ_MMP_USB);
    INT_Disable(IRQ_MMP_USB);//enable USB interrupt
}


void usb_sph_host_isr(void *usb)
{
    usb = usb;
    if ( usb_sph_host_ISR_Handle )
        (usb_sph_host_ISR_Handle)();
}


void usb_sph_host_isr_enable(void)
{
    usb_sph_host_ISR_Handle = (void (*)())_usb_hsic_hci_vusb20_isr;

    ISR_Connect(IRQ_MMP_SPH, usb_sph_host_isr, NULL);
    INT_Enable(IRQ_MMP_SPH,  0,  0xf);//enable USB interrupt
}

void usb_sph_host_isr_disable(void)
{

    ISR_Disconnect(IRQ_MMP_SPH);
    INT_Disable(IRQ_MMP_SPH);//disable USB interrupt



}
#endif


void usb_ic_usb_host_isr(void *usb)
{
    usb = usb;
    if ( usb_ic_usb_host_ISR_Handle )
        (usb_ic_usb_host_ISR_Handle)();
}
void usb_ic_usb_host_isr_enable(void)
{
    usb_ic_usb_host_ISR_Handle = (void (*)())_usb_icusb_hci_vusb20_isr;
    ISR_Connect(IRQ_MMP_USB, usb_ic_usb_host_isr, NULL);
    INT_Enable(IRQ_MMP_USB, IRQ_ROUTE, 15);
}

void usb_ic_usb_host_isr_disable(void)
{
    INT_Disable(IRQ_MMP_USB);
}

void    usb_host_isr_disable(void)
{
    if ( usb_service_type == OTG_HOST)
    {
        //usb_otg_host_isr_disable();
        ;

    }
    else if (usb_service_type == HSIC_HOST)
    {
        //usb_sph_host_isr_disable();
        ;

    }
    else if (usb_service_type == IC_USB)
    {
        usb_ic_usb_host_isr_disable();
    }


}
extern void  _turn_power_33(void);
extern void    usb_host_close(void);

void hsic_host_test_driver(void)
{
    DEV_INSTANCE_PTR  dev;
    static uint8_t i = 0;
    if (i == 1)
        return;
    dev = (DEV_INSTANCE_PTR)mass_device.dev_handle;


    //USB_HSIC1_PHY->HSIC_CTL0|=HSIC_CTL0_S2H_DRV_SE0_4RESUME;

    //  usb_controller_read(((USB_HOST_STATE_STRUCT_PTR)dev->host)->DEV_NUM);

    _usb_host_bus_control(dev->host, USB_SUSPEND_SOF);
    // usb_controller_read(((USB_HOST_STATE_STRUCT_PTR)dev->host)->DEV_NUM);

    delay(25);
    // host_last_step();


    _usb_host_bus_control(dev->host, USB_RESUME_SOF);
    //   usb_controller_read(((USB_HOST_STATE_STRUCT_PTR)dev->host)->DEV_NUM);

    i++;


    delay(2);

    // usb_controller_read(((USB_HOST_STATE_STRUCT_PTR)dev->host)->DEV_NUM);

    mass_device.dev_state = USB_DEVICE_INTERFACED;



}
#ifdef __USB_OS_MQX__
//void Main_Task ( uint_32 param )
#else
void usb_host_main( uint_8 host_device_num)
#endif

{
    /* Body */

    USB_STATUS           status = USB_OK;
    memset((void *) &mass_device, 0, sizeof(mass_device));
    _disable_interrupts();


    alloc_size = 0;
    //usb_host_ISR_Handle= (void (*)())_usb_hci_vusb20_isr;
#ifdef __USB_OS_MQX__
    _int_install_unexpected_isr ( );
    status = _usb_driver_install(0, &_bsp_usb_callback_table);
    if (status)
    {
        printf("ERROR: %ls", status);
        exit(1);
    } /* End if */
#endif

    status = _usb_host_init
             (host_device_num,    /* Use value in header file */
              MAX_FRAME_SIZE,            /* Frame size per USB spec  */
              &host_handle);             /* Returned pointer */

    if (status != USB_OK)
    {
        bbu_printf(("usb_host_init failed \n" ));
    } /* End if */

    /*
    ** Since we are going to act as the host driver, register
    ** the driver information for wanted class/subclass/protocols
    */
    status = _usb_host_driver_info_register(host_handle, DriverInfoTable);
    if (status != USB_OK)
    {
        bbu_printf(("_usb_host_driver_info_register failed \n" ));
    }
#if 0
    if ( host_device_num == USB_OTG_PORT )
    {
        usb_otg_host_ISR_Handle = (void (*)())_usb_hci_vusb20_isr;
        usb_otg_host_isr_enable();
    }
        if ( host_device_num == USB_HSIC1_PORT )
    {
        usb_sph_host_ISR_Handle = (void (*)())_usb_hsic_hci_vusb20_isr;
        usb_sph_host_isr_enable();
        bbu_printf("usb hsic host intrrupt enable \r\n");
        //USB_HSIC1_PHY->USB_CTL0|=(BIT_5|BIT_6);
    }
#endif
    mass_device.dev_state = 0;
    mass_device.pbs = NULL;         /* Device & pipes */
    mass_device.dev_handle = NULL;
    mass_device.intf_handle = NULL;
    mass_device.class_intf.class_intf_handle = NULL; /* Class-specific info */
    mass_device.class_intf.code_key = 0; /* Class-specific info */

    bCallBack = FALSE;
    bStatus       = USB_OK;
    dBuffer_length = 0;
    Attach_Notification = FALSE;


    if ( host_device_num == USB_ICUSB_PORT)
    {

        usb_ic_usb_host_isr_enable();
        bbu_printf("usb ic_usb host intrrupt enable \r\n");
        //USB_HSIC1_PHY->USB_CTL0|=(BIT_5|BIT_6);
    }


    /*----------------------------------------------------**
    ** Infinite loop, waiting for events requiring action **
    **----------------------------------------------------*/
    for ( ; ; )
    {
    #if 0
        if (hsic_phy_end_bit == 1)
        {
            hsic_phy_end_bit = 2;
            bbu_printf("host_last_step \r\n");
            mdelay(15);
            host_last_step();
        }
        //usb_sph_phy_read();
    #endif

        switch ( mass_device.dev_state )
        {
        case USB_DEVICE_IDLE:
            break ;
        case USB_DEVICE_ATTACHED:
            bbu_printf(("Mass Storage Attached\n" ));
            mass_device.dev_state = USB_DEVICE_SET_INTERFACE_STARTED;
            status = _usb_hostdev_select_interface(mass_device.dev_handle,
                                                   mass_device.intf_handle, (pointer)&mass_device.class_intf);
            break ;
        case USB_DEVICE_SET_INTERFACE_STARTED:
            break;
        case USB_DEVICE_INTERFACED:
            bbu_printf(("Mass Storage Interfaced, Enter MSD test\r\n" ));
            usb_host_mass_test_storage(host_device_num);
            mass_device.dev_state = USB_DEVICE_OTHER;
            break;

        case USB_DEVICE_DETACHED:
            bbu_printf(("Mass Storage Detached\n" ));
            mass_device.dev_state = USB_DEVICE_IDLE;
            usb_host_close();
            return ;
        //break;
        case USB_DEVICE_OTHER:
            if (usb_service_type == HSIC_HOST)
                hsic_host_test_driver();
            //bbu_printf(("other\n" ));

            break ;

        default:
            //            bbu_printf( ("Unknown Mass Storage Device State = %d\n",
            //               mass_device.dev_state );

            break ;

        } /* End switch */
    } /* End for */

} /* End body */

#ifdef __USB_OTG__
extern _usb_otg_handle      app_otg_handle;
extern void app_process_HNP
(
    _usb_host_handle  handle
);
extern void _usb_host_bus_control
(
    /* [IN] the USB Host state structure */
    _usb_host_handle           handle,

    /* The operation to be performed on the bus */
    uint_8                     bcontrol
);
static uint_8 hnp_num = 0;
void init_host_class(void)
{
    memset((void *) &mass_device, 0, sizeof(mass_device));

    mass_device.dev_state = 0;
    //   uint_32                          dev_state;  /* Attach/detach state */
    //   PIPE_BUNDLE_STRUCT_PTR
    //device.pbs = NULL;            /* Device & pipes */
    //   _usb_device_instance_handle
    mass_device.dev_handle = NULL;
    //   _usb_interface_descriptor_handle
    mass_device.intf_handle = NULL;
    //   CLASS_CALL_STRUCT
    mass_device.class_intf.class_intf_handle = NULL; /* Class-specific info */
    mass_device.class_intf.code_key = 0; /* Class-specific info */

    bCallBack = FALSE;
    bStatus       = USB_OK;
    dBuffer_length = 0;
    Attach_Notification = FALSE;






}


void host_main
(
    _usb_host_handle  handle
)
{
    uint_32                       state = 0;
    handle = handle;
    USB_STATUS                    status;
    //char                          c;
    //uint_32 last_state            = 0xFFFF;
    //TR_INIT_PARAM_STRUCT          tr_to_bulk_out = {0};
    //TR_INIT_PARAM_STRUCT          tr_to_bulk_in  = {0};
    DEV_INSTANCE_PTR  dev;
    //USB_memzero( device,sizeof(DEVICE_STRUCT));


    mdelay(30);

    bbu_printf("+++++++++++++++++++wait the usb to connect+++++++++++++++++\r\n");

    _usb_otg_get_status(app_otg_handle, USB_OTG_STATE, &state);

    /*************************************************************************
    Once the OTG application enters the host state, it loops through a busy
    while loop checking for the state change. If state does not remain
    as Host, it returns the control back to OTG main loop (see file demo.c)
    *************************************************************************/

    while ((state == A_HOST) | (state == B_HOST))
    {
        /*************************************************************************
        Keep taking action based on the event that ocsured      .
        *************************************************************************/
        //FIXME:
        //device.STATE = USB_DEVICE_ATTACHED;

        //  INT_Enable( INT_USB, IRQ_ROUTE, USB_PRIORITY );
        //_usb_otg_vusbhs_isr();

        switch ( mass_device.dev_state )
        {
        case USB_DEVICE_IDLE:
            break ;
        case USB_DEVICE_ATTACHED:
            bbu_printf( "Mass Storage Attached\r\n" );
            //          fflush(stdout);
            mass_device.dev_state = USB_DEVICE_SET_INTERFACE_STARTED;
            status = _usb_hostdev_select_interface(mass_device.dev_handle,
                                                   mass_device.intf_handle, (pointer)&mass_device.class_intf);
            break ;
        case USB_DEVICE_SET_INTERFACE_STARTED:
            break;
        case USB_DEVICE_INTERFACED:
            bbu_printf( "Mass Storage Interfaced, Enter MSD test\r\n" );
            usb_host_mass_test_storage(  0);
            mass_device.dev_state = USB_DEVICE_OTHER;
            break ;
            //      return;

        case USB_DEVICE_DETACHED:
            bbu_printf( "Mass Storage Detached\r\n" );
            //             fflush(stdout);
            //   mass_device.dev_state = USB_DEVICE_IDLE;
            // usb_host_close();
            init_host_class();
            return;
        case USB_DEVICE_OTHER:
#if 0
            dev = (DEV_INSTANCE_PTR)mass_device.dev_handle;
            if (state == A_HOST)
            {

                if (hnp_num < 4)
                    app_process_HNP(dev->host);
                hnp_num++;

            }
            else if (state == B_HOST)
            {
                if (hnp_num < 4)
                    _usb_host_bus_control(dev->host, USB_SUSPEND_SOF);
                hnp_num++;



            }
            if (hnp_num >= 4)
                return;
#endif
            break ;
        default:
            //            DEBUGMSG(TTC_DBG_LEVEL, ( "Unknown Mass Storage Device State = %d\n",
            //               mass_device.dev_state );
            //               fflush(stdout);
            break ;
        } /* Endswitch */



        /*************************************************************************
        Keep checking the state of the OTG state machine
        *************************************************************************/

        _usb_otg_get_status(app_otg_handle, USB_OTG_STATE, &state);
        // bbu_printf("host_main\r\n");
        mdelay(100);

    } /* Endwhile */
} /* Endbody */
#endif

void usbPipeFree(  _usb_pipe_handle pipe_handle)
{
    PIPE_TR_STRUCT_PTR            pipe_tr_ptr;
    PIPE_DESCRIPTOR_STRUCT_PTR    pipe_descr_ptr;
    PIPE_TR_STRUCT_PTR               tr_ptr;

    pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;
    tr_ptr = pipe_descr_ptr->tr_list_ptr;
    pipe_tr_ptr = tr_ptr;
    while (pipe_tr_ptr->next != tr_ptr)
        pipe_tr_ptr = pipe_tr_ptr->next;
    pipe_tr_ptr->next = NULL;

    while (tr_ptr)
    {

        // bbu_printf("tr_ptr is 0x%x\r\n",tr_ptr);
        pipe_tr_ptr = tr_ptr;
        tr_ptr = tr_ptr->next;
        USB_memfree(pipe_tr_ptr);


    }



}
void memfree_pipe4msc(DEV_INSTANCE_PTR  dev_instance_ptr )
{

    USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr;
    CLASS_CALL_STRUCT                   ccs;
    // DEV_INSTANCE_PTR           dev_ptr;

    _usb_pipe_handle                           pipe_handle;
    //dev_ptr= (DEV_INSTANCE_PTR)dev_instance_ptr;
    ccs = (CLASS_CALL_STRUCT)mass_device.class_intf;
    intf_ptr = (USB_MASS_CLASS_INTF_STRUCT_PTR)(ccs.class_intf_handle);
    pipe_handle = dev_instance_ptr->control_pipe;
    usbPipeFree( pipe_handle);
    pipe_handle = intf_ptr->BULK_IN_PIPE;
    usbPipeFree( pipe_handle);
    pipe_handle = intf_ptr->BULK_OUT_PIPE;
    usbPipeFree( pipe_handle);


}



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_device_event
* Returned Value : None
* Comments       :
*     called when mass storage device has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_mass_device_event
(
    /* [IN] pointer to device instance */
    _usb_device_instance_handle      dev_handle,

    /* [IN] pointer to interface descriptor */
    _usb_interface_descriptor_handle intf_handle,

    /* [IN] code number for event causing callback */
    uint_32           event_code
)
{
    /* Body */
    /* clean warning INTERFACE_DESCRIPTOR_PTR   intf_ptr =
       (INTERFACE_DESCRIPTOR_PTR)intf_handle; */

    switch (event_code)
    {
    case USB_CONFIG_EVENT:
    /* Drop through into attach, same processing */
    case USB_ATTACH_EVENT:
        if (mass_device.dev_state == USB_DEVICE_IDLE)
        {
            mass_device.dev_handle = dev_handle;
            mass_device.intf_handle = intf_handle;
            mass_device.dev_state = USB_DEVICE_ATTACHED;
            bbu_printf("USB_DEVICE_ATTACHED \r\n");
        }
        else
        {
            bbu_printf(("Mass Storage Already Attached\n"));
        } /* EndIf */
        break;
    case USB_INTF_EVENT:
        mass_device.dev_state = USB_DEVICE_INTERFACED;
        break;
    case USB_DETACH_EVENT:
        if ((mass_device.dev_state == USB_DEVICE_INTERFACED)
                || (mass_device.dev_state == USB_DEVICE_OTHER))
        {
            mass_device.dev_handle = NULL;
            mass_device.intf_handle = NULL;
            mass_device.dev_state = USB_DEVICE_DETACHED;
        }
        else
        {
            bbu_printf(("Mass Storage Not Attached\n"));
        } /* EndIf */
        break;
    default:
        mass_device.dev_state = USB_DEVICE_IDLE;
        break;
    } /* EndSwitch */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hub_device_event
* Returned Value : None
* Comments       :
*     called when mass storage device has been attached, detached, etc.
*END*--------------------------------------------------------------------*/



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_ctrl_calback
* Returned Value : None
* Comments       :
*     called on completion of a control-pipe transaction.
*
*END*--------------------------------------------------------------------*/

void usb_host_mass_ctrl_callback
(
    /* [IN] pointer to pipe */
    _usb_pipe_handle  pipe_handle,

    /* [IN] user-defined parameter */
    pointer           user_parm,

    /* [IN] buffer address */
    uchar_ptr         buffer,

    /* [IN] length of data transferred */
    uint_32           buflen,

    /* [IN] status, hopefully USB_OK or USB_DONE */
    uint_32           status
)
{
    /* Body */
    bCallBack = TRUE;
    bStatus = status;
    pipe_handle = pipe_handle;
    user_parm = user_parm;
    buffer = buffer;
    buflen = buflen;
    status = status;
} /* End body */


void callback_bulk_pipe
(
    /* [IN] Status of this command */
    USB_STATUS status,

    /* [IN] pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
    pointer p1,

    /* [IN] pointer to the command object*/
    pointer  p2,

    /* [IN] Length of data transmitted */
    uint_32 buffer_length
)
{
    /* Body */
    //bbu_printf("Callback is length %d\n",buffer_length);
    dBuffer_length = buffer_length;
    bCallBack = TRUE;
    bStatus = status;
    p1 = p1;
    p2 = p2;
} /* End body */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name : usb_host_mass_fs_test
 * Return Value
 * Comments :
 *    Using open source FS
 * end*--------------------------------------------------------------------*/
/*static int usb_host_mass_fs_test
    (
        void
    )
{
    uint8_t filenameString[15]  = {0};
    EmbeddedFileSystem          efs;
    EmbeddedFile                file;
    DirList                     list;
    uint8_t textFileBuffer[50] = "Marvell test usb mass storage \r\n" ;
    if(efs_init(&efs,0)!=0)
    {*/
/* efs initialisation fails*/
/*      bbu_printf(TTC_DBG_LEVEL,("EFS_INIT Failed \r"));
        return -1;
    }
    if (ls_openDir(&list, &(efs.myFs), "/") != 0)
    {*/
/*Could not open the selected directory*/
/*      bbu_printf(TTC_DBG_LEVEL, ("ls_openDir failed \r"));
        return(-2);
    }
    while(ls_getNext(&list) == 0)
    {

          filenameString[0]  = list.currentEntry.FileName[0];
          filenameString[1]  = list.currentEntry.FileName[1];
          filenameString[2]  = list.currentEntry.FileName[2];
          filenameString[3]  = list.currentEntry.FileName[3];
          filenameString[4]  = list.currentEntry.FileName[4];
          filenameString[5]  = list.currentEntry.FileName[5];
          filenameString[6]  = list.currentEntry.FileName[6];
          filenameString[7]  = list.currentEntry.FileName[7];
          filenameString[8]  = '.' ;
          filenameString[9]  = list.currentEntry.FileName[8];
          filenameString[10] = list.currentEntry.FileName[9];
          filenameString[11] = list.currentEntry.FileName[10];
          bbu_printf(TTC_DBG_LEVEL, ("\r\n ls_getNext \r\n"));
          bbu_printf(TTC_DBG_LEVEL, (filenameString));
    }*/
/*  if (file_fopen(&file,&efs.myFs,"HOSTDEMO.TXT",'w')==0)
    { */
/* Write buffer to file */
//      file_write(&file, 50, textFileBuffer);
/*close file and filesystem*/
/*      file_fclose(&file);
        fs_umount(&efs.myFs);
     }
    return 0;
} */
#if 0
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_test_storage
* Returned Value : None
* Comments       :
*     Calls the UFI command set for testing
*END*--------------------------------------------------------------------*/
void msc_free_buffer(void)
{


    USB_memfree(pCmd->CBW_PTR);
    USB_memfree(pCmd->CSW_PTR);
    USB_memfree( pCmd);



}
void usb_host_mass_test_storage
(
    void
)
{
    /* Body */
    USB_STATUS                                 status = USB_OK;
    uint_8                                     bLun = 0;
    uchar_ptr                                  buff_out, buff_in, p;
    INQUIRY_DATA_FORMAT                        inquiry;
    CAPACITY_LIST                              capacity_list;
    // clean warning MODE_SELECT_PARAMETER_LIST                 md_list;
    MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO read_capacity;
    REQ_SENSE_DATA_FORMAT                      req_sense;
    // clean warning FORMAT_UNIT_PARAMETER_BLOCK                formatunit = { 0};

    int i;
    BU_U32 cap, sec;


    pCmd = (COMMAND_OBJECT_PTR) USB_memalloc(sizeof(COMMAND_OBJECT_STRUCT));
    USB_dev_memzero(pCmd, sizeof(COMMAND_OBJECT_STRUCT));

    pCmd->LUN      = bLun;
    pCmd->CALL_PTR = (pointer)&mass_device.class_intf;
    pCmd->CALLBACK = callback_bulk_pipe;

    pCmd->CBW_PTR = (CBW_STRUCT_PTR) USB_memalloc(sizeof(CBW_STRUCT));
    USB_dev_memzero(pCmd->CBW_PTR, sizeof(CBW_STRUCT));

    pCmd->CSW_PTR = (CSW_STRUCT_PTR) USB_memalloc(sizeof(CSW_STRUCT));
    USB_dev_memzero(pCmd->CSW_PTR, sizeof(CSW_STRUCT));


    buff_in = (uchar_ptr)USB_memalloc(0x400C);

    bbu_printf("\n ++++++++++++++++++++++++START OF A NEW SESSION++++++++++++++++++++++\r\n");

    /* Test the GET MAX LUN command */
    bbu_printf("Testing: GETMAXLUN Command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;

    status = usb_class_mass_getmaxlun_bulkonly(
                 (pointer)&mass_device.class_intf, &bLun,
                 usb_host_mass_ctrl_callback);

    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */



    /* Test the TEST UNIT READY command */
    bbu_printf("Testing: TEST UNIT READY Command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status =  usb_mass_ufi_test_unit_ready(pCmd);

    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */



    /* Test the REQUEST SENSE command */
    /*   bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
       bCallBack = FALSE;
       status = usb_mass_ufi_request_sense(pCmd, &req_sense,
          sizeof(REQ_SENSE_DATA_FORMAT));
       while (!bCallBack){;}
       if (!bStatus) {
          bbu_printf("...OK\r\n");
       }
    */

    /* Test the INQUIRY command */
    bbu_printf("Testing: UFI_INQUIRY command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_inquiry(pCmd, (uchar_ptr) &inquiry,
                                  sizeof(INQUIRY_DATA_FORMAT));
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */


    /* Test the REQUEST SENSE command */
    /*   bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
       bCallBack = FALSE;
       status = usb_mass_ufi_request_sense(pCmd, &req_sense,
          sizeof(REQ_SENSE_DATA_FORMAT));
       while (!bCallBack){;}
       if (!bStatus) {
          bbu_printf("...OK\r\n");
       }
    */


    /* Test the READ FORMAT CAPACITY command */
    bbu_printf("Testing: READ FORMAT CAPACITY command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_format_capacity(pCmd, (uchar_ptr)&capacity_list,
                                          sizeof(CAPACITY_LIST));
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    }
    //  p = (uchar_ptr)&capacity_list,
    //  DEBUGMSG(TTC_DBG_LEVEL,("   format capacity = "));
    //  for ( i=0; i<sizeof(CAPACITY_LIST); i++)
    //      DEBUGMSG(TTC_DBG_LEVEL,(" %02x ", *p++));



    /* Test the REQUEST SENSE command */
    /*   bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
       bCallBack = FALSE;
       status = usb_mass_ufi_request_sense(pCmd, &req_sense,
          sizeof(REQ_SENSE_DATA_FORMAT));
       while (!bCallBack){;}
       if (!bStatus) {
          bbu_printf("...OK\r\n");
       }
     */

    /* Test the READ CAPACITY command */
    bbu_printf("Testing: READ CAPACITY command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_read_capacity(pCmd, (uchar_ptr)&read_capacity,
                                        sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    }

    p = (uchar *)&read_capacity;
    for ( cap = 0, i = 0; i < 4; i++)
        cap = (cap << 8) | (BU_U32)(*p++);
    bbu_printf("Capacity = %d MBytes    \r\n ", cap >> 11);
    //BBU_puthexd();
    bbu_printf("\r\n");

    for ( sec = 0, i = 0; i < 4; i++)
        sec = (sec << 8) | (BU_U32)(*p++);
    bbu_printf("Sector Size =%d\r\n  ", sec );
    //BBU_puthexd();
    bbu_printf("\r\n");




    /* Test the MODE SELECT command */
    //   bbu_printf("Testing: MODE SELECT command\r\n");
    //   fflush(stdout);
    //   bCallBack = FALSE;

    //md_list. Fill parameters here
    //   status = usb_mass_ufi_mode_select(pCmd, &md_list);

    /* Wait till command comes back */
    //   while (!bCallBack){;}
    //   if (!bStatus) {
    //      DEBUGMSG(TTC_DBG_LEVEL,("...OK\n"));
    //} /* Endif */



    /* Test the REQUEST SENSE command */
    /*   bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
       bCallBack = FALSE;
       status = usb_mass_ufi_request_sense(pCmd, &req_sense,
          sizeof(REQ_SENSE_DATA_FORMAT));
       while (!bCallBack){;}
       if (!bStatus) {
          bbu_printf("...OK\r\n");
       }
    */


    /* Test the READ 10 command */
    bbu_printf("Testing: READ 10 command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_read_10(pCmd, 0, buff_in, 512, 1);

    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */

    //  p = (uchar *)buff_in;
    //  DEBUGMSG(TTC_DBG_LEVEL,("  Sector 0 = "));
    //  for ( i=0; i< 512; i++)
    //      DEBUGMSG(TTC_DBG_LEVEL,(" %02x ", *p++));



    /* Test the MODE SENSE command */
    bbu_printf("Testing: MODE SENSE  command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_mode_sense(pCmd,
                                     0, //PC
                                     0x1c, //page code
                                     buff_in,
                                     0xC0);
    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */



    /* Test the PREVENT ALLOW command */
    bbu_printf("Testing: PREVENT ALLOW   command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;

    status = usb_mass_ufi_prevent_allow_medium_removal(
                 pCmd,
                 1 // prevent
             );

    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */



    /* Test the REQUEST SENSE command */
    /*   bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
       bCallBack = FALSE;

       status = usb_mass_ufi_request_sense(pCmd, &req_sense,
          sizeof(REQ_SENSE_DATA_FORMAT));

       // Wait till command comes back
       while (!bCallBack){;}
       if (!bStatus) {
          bbu_printf("...OK\r\n");
       }
    */

    /* Test the VERIFY command */
    bbu_printf("Testing: VERIFY   command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;

    status = usb_mass_ufi_verify(
                 pCmd,
                 0x400, // block address
                 1 //length to be verified
             );

    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */

    /* Test the WRITE 10 command */
    ///   bbu_printf("Testing: WRITE 10 command\r\n");
    //   fflush(stdout);
    ///  bCallBack = FALSE;

    ///  buff_out = (uchar_ptr) USB_memalloc(0x0F);
    ///  status = usb_mass_ufi_write_10(pCmd, 8, buff_out, 512, 1);

    /* Wait till command comes back */
    ///  while (!bCallBack){;}
    ///  if (!bStatus) {
    ///     bbu_printf("...OK\r\n");
    ///  } /* Endif */


    /* Test the REQUEST SENSE command */
    bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_request_sense(pCmd, &req_sense,
                                        sizeof(REQ_SENSE_DATA_FORMAT));
    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */



    /* Test the usb_mass_ufi_start_stop command */
    bbu_printf("Testing: START STOP command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_start_stop(pCmd, 0, 1);
    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */


    bbu_printf("+++++++++++++++++++Test done!+++++++++++++++++++++++++++\r\n");
    bbu_printf("                 The Vendor is : ");
    if ((VENDOR[1] == 0x9) && (VENDOR[0] == 0x51))
        bbu_printf(" KINSTON\r\n");
    else
        bbu_printf(" OTHERS\r\n");
    bbu_printf("                 The Capacity is : ", cap);

    bbu_printf(" MB.\r\n");
    bbu_printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
    bbu_printf(" \r\n\r\n");
    //   fflush(stdout);
    USB_memfree(buff_in);
    msc_free_buffer();

} /* Endbody */
#endif
/* EOF */
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_test_storage
* Returned Value : None
* Comments       :
*     Calls the UFI command set for testing
*END*--------------------------------------------------------------------*/
void msc_free_buffer(void)
{


    USB_memfree(pCmd->CBW_PTR);
    USB_memfree(pCmd->CSW_PTR);
    USB_memfree( pCmd);



}


void simple_test_hsic_speed(void)
{
    //  uint_32 i=0,j=0,sector=0x80,length=0x10000;
    uint_32 i = 0, j = 0, sector = 0x80 * 4, length = 0x10000 * 4;

    uint_32 time_start = 0, time_end = 0;
    uchar_ptr     buff_in = (uchar_ptr)USB_memalloc(0x50000);
    uchar_ptr     buff_out = (uchar_ptr)USB_memalloc(0x50000);
    uint_32        result = 0, status = 0;


    memset(buff_out, 'a', 0x50000);
    memset(buff_in, 0, 0x50000);
    for (j = 0; j <= 21; j++)
    {
        if (j % 2 == 0)
        {
            memset(buff_out, 'a', 0x50000);
            bbu_printf("write a for test \r\n");


        }
        else
        {
            memset(buff_out, '5', 0x50000);
            bbu_printf("write 5 for test \r\n");



        }
        time_start = bbu_get_msec();
        for (i = 0; i < 1024 / 4; i++)
        {
            bCallBack = FALSE;
            status = usb_mass_ufi_write_10(pCmd, 0, buff_out, length, sector);

            while (!bCallBack)
            {
                ;
            }

            if (!bStatus)
            {
                //  bbu_printf("...OK\r\n");
            } /* Endif */
        }
        time_end = bbu_get_msec();
        result = (64 * 1000) / (time_end - time_start);
        bbu_printf("time  is %d ms\r\n", (time_end - time_start));

        bbu_printf("HSIC write speed is %d MB/S\r\n", result);


        time_start = bbu_get_msec();
        for (i = 0; i < 1024 / 4; i++)
        {
            bCallBack = FALSE;
            status = usb_mass_ufi_read_10(pCmd, 0, buff_in, length, sector);

            while (!bCallBack)
            {
                ;
            }

            if (!bStatus)
            {
                // bbu_printf("...OK\r\n");
            } /* Endif */

        }
        time_end = bbu_get_msec();
        bbu_printf("time  is %d ms\r\n", (time_end - time_start));

        result = (64 * 1000) / (time_end - time_start);
        bbu_printf("HSIC read speed is %d MB/S\r\n", result);
    }
    for (i = 0x40000; i > 0x3ff9c; i--)
    {

        bbu_printf("buffer in is %d\r\n", buff_in[i]);

    }




    USB_memfree(buff_out);
    USB_memfree(buff_in);






}








uint_8                                     bLun = 0x0 ;
uint_8                                     btestlun[20] = {0};
void usb_host_mass_test_storage
(
    uint_8 host_device_num
)
{
    /* Body */
    USB_STATUS                                 status = USB_OK;
    //uint_8                                     bLun ;
    uchar_ptr                                  buff_out, buff_in, p;
    INQUIRY_DATA_FORMAT                        inquiry;
    CAPACITY_LIST                              capacity_list;
    // clean warning MODE_SELECT_PARAMETER_LIST                 md_list;
    REQ_SENSE_DATA_FORMAT                      req_sense;
    // clean warning FORMAT_UNIT_PARAMETER_BLOCK                formatunit = { 0};
    //    MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO read_capacity;
    MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO_PTR  pread_capacity;

    int i;
    BU_U32 cap, sec;
    pread_capacity = (MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO_PTR)USB_memalloc(sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));
    //   bbu_printf("pread_capacity is 0x%x\r\n",pread_capacity);

    pCmd = (COMMAND_OBJECT_PTR) USB_memalloc(sizeof(COMMAND_OBJECT_STRUCT));
    USB_dev_memzero(pCmd, sizeof(COMMAND_OBJECT_STRUCT));
    // bbu_printf("pCmd is 0x%x\r\n",pCmd);

    pCmd->LUN      = bLun;
    pCmd->CALL_PTR = (pointer)&mass_device.class_intf;
    pCmd->CALLBACK = callback_bulk_pipe;

    pCmd->CBW_PTR = (CBW_STRUCT_PTR) USB_memalloc(sizeof(CBW_STRUCT));
    USB_dev_memzero(pCmd->CBW_PTR, sizeof(CBW_STRUCT));
    //  bbu_printf(" pCmd->CBW_PTR  is 0x%x\r\n", pCmd->CBW_PTR );

    pCmd->CSW_PTR = (CSW_STRUCT_PTR) USB_memalloc(sizeof(CSW_STRUCT));
    USB_dev_memzero(pCmd->CSW_PTR, sizeof(CSW_STRUCT));
    //   bbu_printf(" pCmd->CSW_PTR  is 0x%x\r\n", pCmd->CSW_PTR );


    buff_in = (uchar_ptr)USB_memalloc(0x400C);
    //    bbu_printf(" buff_in  is 0x%x\r\n",buff_in);

    bbu_printf("\n ++++++++++++++++++++++++START OF A NEW SESSION++++++++++++++++++++++\r\n");

    /* Test the GET MAX LUN command */
    bbu_printf("Testing: GETMAXLUN Command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;

    //usb_host_isr_disable() ;
    status = usb_class_mass_getmaxlun_bulkonly(
                 (pointer)&mass_device.class_intf, (uchar_ptr)& bLun,
                 usb_host_mass_ctrl_callback);
    //mdelay(10);
    // usb_host_isr_enable();


    //Wait till command comes back
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    }

    //   for(int i=0;i<9;i++)
    bbu_printf("0x%x \r\n", bLun);
    // if(bLun!=0)
    //    return;
    /* Test the TEST UNIT READY command */
    bbu_printf("Testing: TEST UNIT READY Command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status =  usb_mass_ufi_test_unit_ready(pCmd);

    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */



    /* Test the REQUEST SENSE command */
    /*   bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
       bCallBack = FALSE;
       status = usb_mass_ufi_request_sense(pCmd, &req_sense,
          sizeof(REQ_SENSE_DATA_FORMAT));
       while (!bCallBack){;}
       if (!bStatus) {
          bbu_printf("...OK\r\n");
       }
    */

    /* Test the INQUIRY command */
    bbu_printf("Testing: UFI_INQUIRY command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_inquiry(pCmd, (uchar_ptr) &inquiry,
                                  sizeof(INQUIRY_DATA_FORMAT));
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */


    /* Test the REQUEST SENSE command */
    /*   bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
       bCallBack = FALSE;
       status = usb_mass_ufi_request_sense(pCmd, &req_sense,
          sizeof(REQ_SENSE_DATA_FORMAT));
       while (!bCallBack){;}
       if (!bStatus) {
          bbu_printf("...OK\r\n");
       }
    */


    /* Test the READ FORMAT CAPACITY command */
    bbu_printf("Testing: READ FORMAT CAPACITY command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_format_capacity(pCmd, (uchar_ptr)&capacity_list,
                                          sizeof(CAPACITY_LIST));
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    }
    //  p = (uchar_ptr)&capacity_list,
    //  DEBUGMSG(TTC_DBG_LEVEL,("   format capacity = "));
    //  for ( i=0; i<sizeof(CAPACITY_LIST); i++)
    //      DEBUGMSG(TTC_DBG_LEVEL,(" %02x ", *p++));



    /* Test the REQUEST SENSE command */
    /*   bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
       bCallBack = FALSE;
       status = usb_mass_ufi_request_sense(pCmd, &req_sense,
          sizeof(REQ_SENSE_DATA_FORMAT));
       while (!bCallBack){;}
       if (!bStatus) {
          bbu_printf("...OK\r\n");
       }
     */

    /* Test the READ CAPACITY command */
    bbu_printf("Testing: READ CAPACITY command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_read_capacity(pCmd, (uchar_ptr)pread_capacity,
                                        sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    }

    p = (uchar *)pread_capacity;
    for ( cap = 0, i = 0; i < 4; i++)
        cap = (cap << 8) | (BU_U32)(*p++);
    bbu_printf("Capacity = %d Bytes    \r\n " , cap);
    //BBU_puthexd();
    bbu_printf("\r\n");

    for ( sec = 0, i = 0; i < 4; i++)
        sec = (sec << 8) | (BU_U32)(*p++);
    bbu_printf("Sector Size =%d\r\n  ", sec );
    //BBU_puthexd();
    bbu_printf("\r\n");
    cap = cap * sec >> 20;



    /* Test the MODE SELECT command */
    //   bbu_printf("Testing: MODE SELECT command\r\n");
    //   fflush(stdout);
    //   bCallBack = FALSE;

    //md_list. Fill parameters here
    //   status = usb_mass_ufi_mode_select(pCmd, &md_list);

    /* Wait till command comes back */
    //   while (!bCallBack){;}
    //   if (!bStatus) {
    //      DEBUGMSG(TTC_DBG_LEVEL,("...OK\n"));
    //} /* Endif */



    /* Test the REQUEST SENSE command */
    /*   bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
       bCallBack = FALSE;
       status = usb_mass_ufi_request_sense(pCmd, &req_sense,
          sizeof(REQ_SENSE_DATA_FORMAT));
       while (!bCallBack){;}
       if (!bStatus) {
          bbu_printf("...OK\r\n");
       }
    */


    /* Test the READ 10 command */
    bbu_printf("Testing: READ 10 command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_read_10(pCmd, 0, buff_in, 512, 1);

    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */

    //  p = (uchar *)buff_in;
    //  DEBUGMSG(TTC_DBG_LEVEL,("  Sector 0 = "));
    //  for ( i=0; i< 512; i++)
    //      DEBUGMSG(TTC_DBG_LEVEL,(" %02x ", *p++));



    /* Test the MODE SENSE command */
    bbu_printf("Testing: MODE SENSE  command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_mode_sense(pCmd,
                                     0, //PC
                                     0x1c, //page code
                                     buff_in,
                                     0xC0);
    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */



    /* Test the PREVENT ALLOW command */
    bbu_printf("Testing: PREVENT ALLOW   command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;

    status = usb_mass_ufi_prevent_allow_medium_removal(
                 pCmd,
                 1 // prevent
             );

    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */



    /* Test the REQUEST SENSE command */
    /*   bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
       bCallBack = FALSE;

       status = usb_mass_ufi_request_sense(pCmd, &req_sense,
          sizeof(REQ_SENSE_DATA_FORMAT));

       // Wait till command comes back
       while (!bCallBack){;}
       if (!bStatus) {
          bbu_printf("...OK\r\n");
       }
    */

    /* Test the VERIFY command */
    bbu_printf("Testing: VERIFY   command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;

    status = usb_mass_ufi_verify(
                 pCmd,
                 0x400, // block address
                 1 //length to be verified
             );

    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */

    /* Test the WRITE 10 command */
    bbu_printf("Testing: WRITE 10 command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;

#if 0
    if (host_device_num == USB_HSIC1_PORT)
    {
        // buff_out = (uchar_ptr) USB_memalloc(0x0F);
        status = usb_mass_ufi_write_10(pCmd, 8, buff_in, 512, 1);

        /* Wait till command comes back */
        while (!bCallBack)
        {
            ;
        }
        if (!bStatus)
        {
            bbu_printf("...OK\r\n");
        } /* Endif */




        simple_test_hsic_speed();
    }
#endif
    /* Test the REQUEST SENSE command */
    bbu_printf("Testing: REQUEST SENSE  command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_request_sense(pCmd, &req_sense,
                                        sizeof(REQ_SENSE_DATA_FORMAT));
    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */



    /* Test the usb_mass_ufi_start_stop command */
    bbu_printf("Testing: START STOP command\r\n");
    //   fflush(stdout);
    bCallBack = FALSE;
    status = usb_mass_ufi_start_stop(pCmd, 0, 1);
    /* Wait till command comes back */
    while (!bCallBack)
    {
        ;
    }
    if (!bStatus)
    {
        bbu_printf("...OK\r\n");
    } /* Endif */


    bbu_printf("+++++++++++++++++++Test done!+++++++++++++++++++++++++++\r\n");
    bbu_printf("                 The Vendor is : ");
    if ((VENDOR[1] == 0x9) && (VENDOR[0] == 0x51))
        bbu_printf(" KINSTON\r\n");
    else
        bbu_printf(" OTHERS\r\n");
    bbu_printf("                 The Capacity is : %d", cap);

    bbu_printf(" MB.\r\n");
    bbu_printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
    bbu_printf(" \r\n\r\n");
    //   fflush(stdout);
    USB_memfree(buff_in);
    USB_memfree(pread_capacity);
    msc_free_buffer();

} /* Endbody */



