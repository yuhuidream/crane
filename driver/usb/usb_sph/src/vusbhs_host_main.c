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
***   This file contains the main low-level Host API functions specific to
***   the VUSB chip.
***
**************************************************************************
**END*********************************************************/
#include "hostapi.h"
#include "usbprv_host.h"
//#include "usb_board.h"

//#define INT_USB 44

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif


#ifndef __USB_OS_MQX__
//extern boolean IN_ISR;
boolean  IN_ISR = FALSE;
#endif

#ifdef __USB_OTG__
#include "otgapi.h"
#include "usbprv_otg.h"
extern USB_OTG_STATE_STRUCT_PTR usb_otg_state_struct_ptr;
#endif
volatile boolean ignore = FALSE;
#include "USBSPH_MISC.h"
#include "APBSPARE.h"
#include "AIB.h"
//extern void * USB_memalloc(uint_32 n);
#include "PMUA.h"
//#define TEST_WRITE

#include "pmic.h"
#include "APBCLK.h"
#include "i2c_dec.h"
#include "gpio_dec.h"
#include "timer_dec.h"
#include "USB_HSIC_PHY.h"
#include "USB.h"

#include "usb_hsic.h"
#include "usb_wakeup.h"
#include "msc_commands.h"
#ifdef TEST_WRITE
#define USB_REGISTER_TO_WRITE  (volatile uint_32 *) 0xFC5184
#endif
uint_8 ic_choose_vol = FALSE;
//SSS#define    C0_8618
extern void  usb2_phy_init(void);
/**************************************************************************
 Local Functions. Note intended to be called from outside this file
**************************************************************************/

static void _usb_ehci_process_qh_list_tr_complete(
    _usb_host_handle                 handle,
    ACTIVE_QH_MGMT_STRUCT_PTR        active_list_member_ptr
);

static void _usb_ehci_process_qh_interrupt_tr_complete(
    _usb_host_handle                 handle,
    ACTIVE_QH_MGMT_STRUCT_PTR        active_list_member_ptr
);

static void _usb_ehci_process_itd_tr_complete
(
    _usb_host_handle                 handle
);

static void _usb_ehci_process_sitd_tr_complete
(
    _usb_host_handle                 handle
);

USB_STATUS  _usb_hci_init_driver_memory
(
    _usb_host_handle     handle
);

inline void _disable_interrupts()
{
};

inline void _enable_interrupts()
{
};
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hci_vusb20_send_data
*  Returned Value : None
*  Comments       :
*        Send the data
*END*-----------------------------------------------------------------*/
USB_STATUS  _usb_hci_vusb20_send_data
(
    /* [IN] the USB Host state structure */
    _usb_host_handle              handle,

    /* [IN] The pipe descriptor to queue */
    PIPE_DESCRIPTOR_STRUCT_PTR    pipe_descr_ptr,

    /* [IN] the transfer parameters struct */
    PIPE_TR_STRUCT_PTR            current_pipe_tr_struct_ptr
)
{
    /* Body */
    USB_STATUS status;

    //bbu_printf("Send Data\r\n");
    status = _usb_hci_ehci_queue_pkts(handle, pipe_descr_ptr, current_pipe_tr_struct_ptr);
    return status;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hci_vusb20_send_setup
*  Returned Value : USB_STATUS
*  Comments       :
*        Send Setup packet
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_hci_vusb20_send_setup
(
    /* [IN] the USB Host state structure */
    _usb_host_handle              handle,

    /* [IN] The pipe descriptor to queue */
    PIPE_DESCRIPTOR_STRUCT_PTR    pipe_descr_ptr,

    /* [IN] the transfer parameters struct */
    PIPE_TR_STRUCT_PTR            current_pipe_tr_struct_ptr
)
{
    /* Body */
    USB_STATUS status;
    // clean warning BU_U32 *tp;
    // clean warning int i;

    //  bbu_printf("call hci_send_setup!\r\n"));
#if 0
    tp = (BU_U32 *)pipe_descr_ptr;

    for ( i = 0; i < 9; i++, tp++ )
        bbu_printf("%x=%x\r\n");

    bbu_printf("\r\n\r\n"));
    tp = (BU_U32 *)current_pipe_tr_struct_ptr;

    for ( i = 0; i < 11; i++, tp++ )
bbu_printf("%x=%x\r\n");

bbu_printf("\r\n\r\n"));

for ( i = 0; i < 2; i++, tp++ )
    bbu_printf("SETUP Packet %x=%x\r\n");
#endif

    status = _usb_hci_ehci_queue_pkts(handle, pipe_descr_ptr, current_pipe_tr_struct_ptr);
#if 0
    bbu_printf("\r\n0x80000140 = %x\r\n",);
    bbu_printf("0x80000158 = %x\r\n");
    bbu_printf("0x80000184 = %x\r\n");
#endif
    return status;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hci_vusb20_recv_data
*  Returned Value : None
*  Comments       :
*        Receive data
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_hci_vusb20_recv_data
(
    /* [IN] the USB Host state structure */
    _usb_host_handle              handle,

    /* [IN] The pipe descriptor to queue */
    PIPE_DESCRIPTOR_STRUCT_PTR    pipe_descr_ptr,

    /* [IN] the transfer parameters struct */
    PIPE_TR_STRUCT_PTR            current_pipe_tr_struct_ptr
)
{
    /* Body */
    USB_STATUS status;

    status = _usb_hci_ehci_queue_pkts(handle, pipe_descr_ptr, current_pipe_tr_struct_ptr);

    return status;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hci_ehci_queue_pkts
*  Returned Value : status of the transfer queuing.
*  Comments       :
*        Queue the packet in the hardware
*END*-----------------------------------------------------------------*/

USB_STATUS  _usb_hci_ehci_queue_pkts
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle,

    /* The pipe descriptor to queue */
    PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr,

    /* [IN] the transfer parameters struct */
    PIPE_TR_STRUCT_PTR               current_pipe_tr_struct_ptr
)
{
    /* Body */
    USB_STATUS  status;

    /* Queue the packets */
    switch (pipe_descr_ptr->PIPETYPE)
    {
    case USB_ISOCHRONOUS_PIPE:
        status = _usb_ehci_add_isochronous_xfer_to_periodic_schedule_list(handle,
                 pipe_descr_ptr, current_pipe_tr_struct_ptr);
        break;
    case USB_INTERRUPT_PIPE:
        status = _usb_ehci_add_interrupt_xfer_to_periodic_list(handle,
                 pipe_descr_ptr, current_pipe_tr_struct_ptr);
        break;
    case USB_CONTROL_PIPE:
    case USB_BULK_PIPE:
        /* Queue it on the asynchronous schedule list */
        status = _usb_ehci_add_xfer_to_asynch_schedule_list(handle, pipe_descr_ptr,
                 current_pipe_tr_struct_ptr);
        break;
    default:
        status = USB_STATUS_ERROR;
        break;
    } /* EndSwitch */

    return status;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_init_Q_HEAD
*  Returned Value : None
*  Comments       :
*        Initialize the Queue Head. This routine initializes a queue head
*  for interrupt/Bulk/Control transfers.
*END*-----------------------------------------------------------------*/
void _usb_ehci_init_Q_HEAD
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle,

    /* The pipe descriptor to queue */
    PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr,

    /* [IN] the queue head to initialize */
    EHCI_QH_STRUCT_PTR               QH_ptr,

    /* [IN] the previous queue head to link to */
    EHCI_QH_STRUCT_PTR               prev_QH_ptr,

    /* [IN] the first QTD to queue for this Queue head */
    EHCI_QTD_STRUCT_PTR              first_QTD_ptr
)
{
    /* Body */
    //   USB_HOST_STATE_STRUCT_PTR     usb_host_ptr;
    uint_32                       control_ep_flag = 0, split_completion_mask = 0;
    uint_32                       interrupt_schedule_mask = 0;
    uint_32                       data_toggle_control = 0, item_type = 0;
    uint_32                       /* clean warning time_for_xaction = 0, */H_bit = 0;
    uint_8                        mult = 0/* clean warning period = 0*/;
    handle = handle;
    //  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    /******************************************************************************
     PREPARE THE BASIC FIELDS OF A QUEUE HEAD DEPENDING UPON THE PIPE TYPE
    *******************************************************************************/
#ifdef _DATA_CACHE_
    /********************************************************
    USB Memzero does not bypass the cache and hence explicit
    setting to zero is required in software
    ********************************************************/
    QH_ptr->HORIZ_LINK_PTR    = 0;
    QH_ptr->EP_CAPAB_CHARAC1  = 0;
    QH_ptr->EP_CAPAB_CHARAC2  = 0;
    QH_ptr->CURR_QTD_LINK_PTR = 0;
    QH_ptr->NEXT_QTD_LINK_PTR = 0;
    QH_ptr->ALT_NEXT_QTD_LINK_PTR = 0;
    QH_ptr->STATUS       = 0;
    QH_ptr->BUFFER_PTR_0 = 0;
    QH_ptr->BUFFER_PTR_1 = 0;
    QH_ptr->BUFFER_PTR_2 = 0;
    QH_ptr->BUFFER_PTR_3 = 0;
    QH_ptr->BUFFER_PTR_4 = 0;
#else
    USB_dev_memzero((void *)QH_ptr, 12 * sizeof(uint_32));
#endif

    switch (pipe_descr_ptr->PIPETYPE)
    {

    case  USB_CONTROL_PIPE:
        if (pipe_descr_ptr->SPEED != USB_SPEED_HIGH)
        {
            control_ep_flag = 1;
        }
        item_type = EHCI_FRAME_LIST_ELEMENT_TYPE_QH;
        H_bit = (prev_QH_ptr == QH_ptr) ? 1 : 0;
        data_toggle_control = 1;
        break;

    case  USB_INTERRUPT_PIPE:
        data_toggle_control = 0;
        break;

    case  USB_BULK_PIPE:
        item_type = EHCI_FRAME_LIST_ELEMENT_TYPE_QH;
        H_bit = (prev_QH_ptr == QH_ptr) ? 1 : 0;
        data_toggle_control = 0;
        break;

    }

    if (pipe_descr_ptr->SPEED == USB_SPEED_HIGH)
    {
        mult = pipe_descr_ptr->TRS_PER_UFRAME;
    }

    /******************************************************************************
     PREPARE THE Fields that are same for all pipes.
    *******************************************************************************/

    QH_ptr->PIPE_DESCR_FOR_THIS_QH = (pointer)pipe_descr_ptr;
    pipe_descr_ptr->QH_FOR_THIS_PIPE = (pointer)QH_ptr;

    QH_ptr->CURR_QTD_LINK_PTR = 0;
    QH_ptr->ALT_NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;
    QH_ptr->STATUS = 0;
    QH_ptr->BUFFER_PTR_0 = 0;
    QH_ptr->BUFFER_PTR_1 = 0;
    QH_ptr->BUFFER_PTR_2 = 0;
    QH_ptr->BUFFER_PTR_3 = 0;
    QH_ptr->BUFFER_PTR_4 = 0;


    /****************************************************************************
     copy the fields inside the QH
    *****************************************************************************/


    /* Initialize the endpoint capabilities registers */
    QH_ptr->EP_CAPAB_CHARAC1 = (
                                   ((uint_32)pipe_descr_ptr->NAK_COUNT << EHCI_QH_NAK_COUNT_RL_BITS_POS) |
                                   (control_ep_flag << EHCI_QH_EP_CTRL_FLAG_BIT_POS) |
                                   (pipe_descr_ptr->MAX_PACKET_SIZE << EHCI_QH_MAX_PKT_SIZE_BITS_POS) |
                                   (H_bit << EHCI_QH_HEAD_RECLAMATION_BIT_POS) |
                                   (data_toggle_control << EHCI_QH_DTC_BIT_POS) |
                                   (pipe_descr_ptr->SPEED << EHCI_QH_SPEED_BITS_POS) |
                                   ((uint_32)pipe_descr_ptr->ENDPOINT_NUMBER << EHCI_QH_EP_NUM_BITS_POS) |
                                   pipe_descr_ptr->DEVICE_ADDRESS);

    QH_ptr->EP_CAPAB_CHARAC2 = (
                                   (mult << EHCI_QH_HIGH_BW_MULT_BIT_POS) |
                                   (pipe_descr_ptr->HUB_PORT_NUM << EHCI_QH_HUB_PORT_NUM_BITS_POS) |
                                   (pipe_descr_ptr->HUB_DEVICE_ADDR << EHCI_QH_HUB_ADDR_BITS_POS) |
                                   (split_completion_mask << EHCI_QH_SPLIT_COMPLETION_MASK_BITS_POS) |
                                   interrupt_schedule_mask);

    /****************************************************************************
     Queue the transfers now by updating the pointers
    *****************************************************************************/

    switch (pipe_descr_ptr->PIPETYPE)
    {
    case  USB_CONTROL_PIPE:
        QH_ptr->HORIZ_LINK_PTR = (uint_32)prev_QH_ptr->HORIZ_LINK_PTR;
        /* Link the new Queue Head to the previous Queue Head */
        prev_QH_ptr->HORIZ_LINK_PTR =
            ((uint_32)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS));

        QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)first_QTD_ptr;

        break;
    case  USB_BULK_PIPE:
        QH_ptr->HORIZ_LINK_PTR = (uint_32)prev_QH_ptr->HORIZ_LINK_PTR;
        /* Link the new Queue Head to the previous Queue Head */
        prev_QH_ptr->HORIZ_LINK_PTR =
            ((uint_32)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS));

        QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)first_QTD_ptr;

        break;
    case  USB_INTERRUPT_PIPE:
        QH_ptr->HORIZ_LINK_PTR = EHCI_QUEUE_HEAD_POINTER_T_BIT;
        QH_ptr->NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;
        break;
    }

    return;

} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_init_QTD
*  Returned Value : None
*  Comments       :
*        Initialize the QTD
*END*-----------------------------------------------------------------*/
void _usb_ehci_init_QTD
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle,

    /* [IN] the address of the QTD to initialize */
    EHCI_QTD_STRUCT_PTR              QTD_ptr,


    /* The buffer start address for the QTD */
    uchar_ptr                        buffer_start_address,

    /* The token value */
    uint_32                          token
)
{
    /* Body */

#ifdef _DATA_CACHE_
    /********************************************************
    USB Memzero does not bypass the cache and hence explicit
    setting to zero is required in software
    ********************************************************/
    QTD_ptr->NEXT_QTD_PTR        =   0;
    QTD_ptr->ALT_NEXT_QTD_PTR    =   0;
    QTD_ptr->TOKEN               =   0;
    QTD_ptr->BUFFER_PTR_0        =   0;
    QTD_ptr->BUFFER_PTR_1        =   0;
    QTD_ptr->BUFFER_PTR_2        =   0;
    QTD_ptr->BUFFER_PTR_3        =   0;
    QTD_ptr->BUFFER_PTR_4        =   0;
#else
    /* Zero the QTD. Leave the scratch pointer */
    USB_dev_memzero((void *)QTD_ptr, 8 * sizeof(uint_32));
#endif

    /* Initialize the QTD */
    QTD_ptr->SCRATCH_PTR->PRIVATE = handle;

    /* Set the Terminate bit */
    QTD_ptr->NEXT_QTD_PTR = EHCI_QTD_T_BIT;

    /* Set the Terminate bit */
    QTD_ptr->ALT_NEXT_QTD_PTR = EHCI_QTD_T_BIT;

    QTD_ptr->BUFFER_PTR_0 = (uint_32)buffer_start_address;

    /* 4K apart buffer page pointers */
    QTD_ptr->BUFFER_PTR_1 = (QTD_ptr->BUFFER_PTR_0 + 4096);
    QTD_ptr->BUFFER_PTR_2 = (QTD_ptr->BUFFER_PTR_1 + 4096);
    QTD_ptr->BUFFER_PTR_3 = (QTD_ptr->BUFFER_PTR_2 + 4096);
    QTD_ptr->BUFFER_PTR_4 = (QTD_ptr->BUFFER_PTR_3 + 4096);

    QTD_ptr->TOKEN = token;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_init_Q_element
*  Returned Value : None
*  Comments       :
*        Initialize the Queue Element descriptor(s)
*END*-----------------------------------------------------------------*/
uint_32 _usb_ehci_init_Q_element
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle,

    /* The pipe descriptor to queue */
    PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr,

    /* [IN] the transfer parameters struct */
    PIPE_TR_STRUCT_PTR               current_pipe_tr_struct_ptr
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR        usb_host_ptr;
    EHCI_QTD_STRUCT_PTR              QTD_ptr, prev_QTD_ptr, first_QTD_ptr;
    uint_32                          pid_code, data_toggle, token, num_of_packets;
    uint_32                          total_length, qtd_length/* clean warning , *tp*/;
    uchar_ptr                        buff_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    /* Get a QTD from the queue */
    EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

    if (!QTD_ptr)
    {
        return USB_STATUS_TRANSFER_IN_PROGRESS;
    } /* Endif */

    usb_host_ptr->QTD_ENTRIES--;

    /* The data 0/1 for this pipe */
    data_toggle = (pipe_descr_ptr->NEXTDATA01 <<
                   EHCI_QTD_DATA_TOGGLE_BIT_POS);

    /* The very first QTD */
    prev_QTD_ptr = first_QTD_ptr = QTD_ptr;

    if (pipe_descr_ptr->PIPETYPE == USB_CONTROL_PIPE)
    {
        pid_code = EHCI_QTD_PID_SETUP;
        /* Setup packet is always 8 bytes */
        token = (((uint_32)8 << EHCI_QTD_LENGTH_BIT_POS) |
                 data_toggle | pid_code | EHCI_QTD_STATUS_ACTIVE);

        /* Initialize a QTD for Setup phase */
        _usb_ehci_init_QTD(handle, QTD_ptr,
                           (uchar_ptr)&current_pipe_tr_struct_ptr->setup_packet,
                           token);
        QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (pointer)pipe_descr_ptr;
        QTD_ptr->TR_FOR_THIS_QTD = (pointer)current_pipe_tr_struct_ptr;

        /* Get a QTD from the queue for the data phase or a status phase */
        EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

        if (!QTD_ptr)
        {
            return USB_STATUS_TRANSFER_IN_PROGRESS;
        } /* Endif */

        usb_host_ptr->QTD_ENTRIES--;

        /* The data phase QTD chained to the Setup QTD */
        prev_QTD_ptr->NEXT_QTD_PTR = (uint_32)QTD_ptr;
        data_toggle = EHCI_QTD_DATA_TOGGLE;

        if (current_pipe_tr_struct_ptr->SEND_PHASE == TRUE)
        {
            /* Host to Device data phase */
            if (current_pipe_tr_struct_ptr->TX_LENGTH)
            {
                /* Anything to send ? */
                pid_code = EHCI_QTD_PID_OUT;
                token = ((current_pipe_tr_struct_ptr->TX_LENGTH << EHCI_QTD_LENGTH_BIT_POS) | data_toggle |
                         pid_code | EHCI_QTD_STATUS_ACTIVE);
                bbu_printf("length=%x!\r\n");

                /* Initialize the QTD for OUT data phase */
                _usb_ehci_init_QTD(handle, QTD_ptr,
                                   current_pipe_tr_struct_ptr->TX_BUFFER, token);

                QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (pointer)pipe_descr_ptr;
                QTD_ptr->TR_FOR_THIS_QTD = (pointer)current_pipe_tr_struct_ptr;

                /* Use the QTD to chain the next QTD */
                prev_QTD_ptr = QTD_ptr;

                /* Get a QTD from the queue for status phase */
                EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

                if (!QTD_ptr)
                {
                    return USB_STATUS_TRANSFER_IN_PROGRESS;
                } /* Endif */

                usb_host_ptr->QTD_ENTRIES--;

                /* Chain the status phase QTD to the data phase QTD */
                prev_QTD_ptr->NEXT_QTD_PTR = (uint_32)QTD_ptr;
            } /* Endif */
            /* Zero length IN */
            pid_code = EHCI_QTD_PID_IN;
        }
        else
        {

            if (current_pipe_tr_struct_ptr->RX_LENGTH)
            {
                pid_code = EHCI_QTD_PID_IN;
                token = ((current_pipe_tr_struct_ptr->RX_LENGTH << EHCI_QTD_LENGTH_BIT_POS) | data_toggle |
                         pid_code | EHCI_QTD_STATUS_ACTIVE);

                /* Initialize the QTD for the IN data phase */
                _usb_ehci_init_QTD(handle, QTD_ptr,
                                   current_pipe_tr_struct_ptr->RX_BUFFER, token);
                QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (pointer)pipe_descr_ptr;
                QTD_ptr->TR_FOR_THIS_QTD = (pointer)current_pipe_tr_struct_ptr;

                /* Use thie QTD to chain the next QTD */
                prev_QTD_ptr = QTD_ptr;

                /* Get a QTD from the queue for status phase */
                EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

                if (!QTD_ptr)
                {
                    return USB_STATUS_TRANSFER_IN_PROGRESS;
                } /* Endif */

                usb_host_ptr->QTD_ENTRIES--;

                /* Chain the status phase QTD to the data phase QTD */
                prev_QTD_ptr->NEXT_QTD_PTR = (uint_32)QTD_ptr;
            } /* Endif */
            /* Zero-length OUT token */
            pid_code = EHCI_QTD_PID_OUT;
        } /* Endif */

        /* Already chained so add a qTD for status phase */
        /* Initialize the QTD for the status phase -- Zero length opposite
        ** direction packet
        */
        token = ((0 << EHCI_QTD_LENGTH_BIT_POS) |
                 data_toggle | EHCI_QTD_IOC | pid_code | EHCI_QTD_STATUS_ACTIVE);
        _usb_ehci_init_QTD(handle, QTD_ptr, NULL, token);
        QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (pointer)pipe_descr_ptr;
        QTD_ptr->TR_FOR_THIS_QTD = (pointer)current_pipe_tr_struct_ptr;
    }
    else
    {

        /* Save the next data toggle based on the number of packets
        ** for this transfer
        */
        if (pipe_descr_ptr->DIRECTION)
        {
            total_length = current_pipe_tr_struct_ptr->TX_LENGTH;
            buff_ptr = current_pipe_tr_struct_ptr->TX_BUFFER;
            pid_code = EHCI_QTD_PID_OUT;
        }
        else
        {
            total_length = current_pipe_tr_struct_ptr->RX_LENGTH;
            buff_ptr = current_pipe_tr_struct_ptr->RX_BUFFER;
            pid_code = EHCI_QTD_PID_IN;
        } /* Endif */

        do
        {

            /* The data 0/1 for this pipe */
            data_toggle = (pipe_descr_ptr->NEXTDATA01 <<
                           EHCI_QTD_DATA_TOGGLE_BIT_POS);

            if (total_length > VUSB_EP_MAX_LENGTH_TRANSFER)
            {
                token = ((VUSB_EP_MAX_LENGTH_TRANSFER  <<
                          EHCI_QTD_LENGTH_BIT_POS) | data_toggle |
                         pid_code | EHCI_QTD_DEFAULT_CERR_VALUE |
                         EHCI_QTD_STATUS_ACTIVE);
                total_length -= VUSB_EP_MAX_LENGTH_TRANSFER;
                qtd_length = VUSB_EP_MAX_LENGTH_TRANSFER;
            }
            else
            {
                token = ((total_length << EHCI_QTD_LENGTH_BIT_POS) |
                         data_toggle | EHCI_QTD_IOC | pid_code |
                         EHCI_QTD_DEFAULT_CERR_VALUE | EHCI_QTD_STATUS_ACTIVE);
                qtd_length = total_length;
                total_length = 0;
            } /* Endif */

            if (qtd_length < pipe_descr_ptr->MAX_PACKET_SIZE)
            {
                num_of_packets = 1;
            }
            else
            {
                /* Deliberately using a divide */
                num_of_packets =
                    ((qtd_length / pipe_descr_ptr->MAX_PACKET_SIZE) +
                     ((total_length % pipe_descr_ptr->MAX_PACKET_SIZE) ? 1 : 0));
            } /* Endif */

            pipe_descr_ptr->NEXTDATA01 ^= (num_of_packets & 0x1);

            /* Initialize the QTD for the OUT data transaction */
            _usb_ehci_init_QTD(handle, QTD_ptr, buff_ptr, token);
            QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (pointer)pipe_descr_ptr;
            QTD_ptr->TR_FOR_THIS_QTD = (pointer)current_pipe_tr_struct_ptr;
            buff_ptr += qtd_length;

            if (total_length)
            {

                /* Use the QTD to chain the next QTD */
                prev_QTD_ptr = QTD_ptr;

                /* Get a QTD from the queue for status phase */
                EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

                if (!QTD_ptr)
                {
                    return USB_STATUS_TRANSFER_IN_PROGRESS;
                } /* Endif */

                usb_host_ptr->QTD_ENTRIES--;

                /* Chain the status phase QTD to the data phase QTD */
                prev_QTD_ptr->NEXT_QTD_PTR = (uint_32)QTD_ptr;
            } /* Endif */
        }
        while (total_length);   /* EndWhile */
    } /* Endif */

    //   tp = (uint_32 *)first_QTD_ptr;
    //   bbu_printf("token=%x %x\r\n", *(tp+2), *(tp+18) ));
    return ((uint_32)first_QTD_ptr);

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_add_xfer_to_asynch_schedule_list
*  Returned Value : USB Status
*  Comments       :
*        Queue the packet in the EHCI hardware Asynchronous schedule list
*END*-----------------------------------------------------------------*/

uint_32 _usb_ehci_add_xfer_to_asynch_schedule_list
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle,

    /* The pipe descriptor to queue */
    PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr,

    /* [IN] the transfer parameters struct */
    PIPE_TR_STRUCT_PTR               current_pipe_tr_struct_ptr
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    VUSB20_REG_STRUCT_PTR                        dev_ptr;
    EHCI_QH_STRUCT_PTR                           QH_ptr = NULL;
    EHCI_QH_STRUCT_PTR                           prev_QH_ptr = NULL;
    EHCI_QH_STRUCT_PTR                           first_QH_ptr;
    EHCI_QTD_STRUCT_PTR                          first_QTD_ptr, temp_QTD_ptr;
    PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_for_queue = NULL;
    ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr, temp_list_ptr;
    uint_32                                      cmd_val;
    boolean                                      init_async_list = FALSE;
    boolean                                      found_existing_q_head = FALSE;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    /* All Control and Bulk Transfers */
    /* First initialize all QTDs and then initialize the QHs */

    /* Initialize the QTDs for the Queue Head */
    first_QTD_ptr = (EHCI_QTD_STRUCT_PTR)_usb_ehci_init_Q_element(handle,
                    pipe_descr_ptr, current_pipe_tr_struct_ptr);

    /* If the Asynch Schedule is disabled then initialize a new list */
    if (((!(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD &
            EHCI_USBCMD_ASYNC_SCHED_ENABLE)) &&
            (!(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS &
               EHCI_STS_ASYNCH_SCHEDULE))) || (!usb_host_ptr->ACTIVE_ASYNC_LIST_PTR))
    {
        init_async_list = TRUE;
    }
    else
    {
        first_QH_ptr = QH_ptr = (EHCI_QH_STRUCT_PTR)
                                ((uint_32)usb_host_ptr->ACTIVE_ASYNC_LIST_PTR->QH_PTR &
                                 EHCI_QH_HORIZ_PHY_ADDRESS_MASK);

        while (QH_ptr)
        {
            pipe_for_queue = (PIPE_DESCRIPTOR_STRUCT_PTR)QH_ptr->PIPE_DESCR_FOR_THIS_QH;

            if (pipe_for_queue == pipe_descr_ptr)
            {
                /* found an existing QHEAD for this endpoint */
                found_existing_q_head = TRUE;
                break;
            }
            else
            {
                prev_QH_ptr = QH_ptr;
                QH_ptr = (EHCI_QH_STRUCT_PTR)((uint_32)QH_ptr->HORIZ_LINK_PTR &
                                              EHCI_QH_HORIZ_PHY_ADDRESS_MASK);
                if (((uint_32)QH_ptr == (uint_32)first_QH_ptr))
                {
                    break;
                } /* Endif */
            } /* Endif */
        } /* Endwhile */
    } /* Endif */

    if (!found_existing_q_head)
    {

        /* Allocate a new queue head and link it */
        EHCI_QH_QGET(usb_host_ptr->QH_HEAD, usb_host_ptr->QH_TAIL, QH_ptr);

        if (!QH_ptr)
        {
            return USBERR_ALLOC;
        } /* Endif */

        usb_host_ptr->QH_ENTRIES--;

        active_list_member_ptr =
            (ACTIVE_QH_MGMT_STRUCT_PTR)USB_memalloc(sizeof(ACTIVE_QH_MGMT_STRUCT));

        if (!active_list_member_ptr)
        {
            return USBERR_ALLOC;
        } /* Endif */

        /**************Not required, done by init Q head routine *****
        pipe_descr_ptr->QH_FOR_THIS_PIPE = (pointer)QH_ptr;
        **********************************************************/

        if (init_async_list)
        {
            prev_QH_ptr = (EHCI_QH_STRUCT_PTR)QH_ptr;
            usb_host_ptr->ACTIVE_ASYNC_LIST_PTR = active_list_member_ptr;
        }
        else
        {
            temp_list_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
            while (temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR)
            {
                temp_list_ptr = temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
            } /* EndWhile */
            temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = active_list_member_ptr;
        } /* Endif */

        /* Initialize the Queue head */
        _usb_ehci_init_Q_HEAD(handle, pipe_descr_ptr, (EHCI_QH_STRUCT_PTR)QH_ptr,
                              (EHCI_QH_STRUCT_PTR)prev_QH_ptr, first_QTD_ptr);
    }
    else
    {

        /* Get the pointer to the member of the active list */
        active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
        while (active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR)
        {
            active_list_member_ptr = active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
        } /* EndWhile */

        /* Queue head is already on the active list. Simply queue the transfer */
        /* Queue the transfer onto the relevant queue head */
        if (QH_ptr->NEXT_QTD_LINK_PTR & EHCI_QTD_T_BIT)
        {
            /* No alternate descriptor */
            QH_ptr->ALT_NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;
            QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)first_QTD_ptr;
        }
        else
        {
            temp_QTD_ptr = (EHCI_QTD_STRUCT_PTR)(QH_ptr->NEXT_QTD_LINK_PTR & ~EHCI_QTD_T_BIT);
            while (!(temp_QTD_ptr->NEXT_QTD_PTR & EHCI_QTD_T_BIT))
            {
                temp_QTD_ptr = (EHCI_QTD_STRUCT_PTR)(temp_QTD_ptr->NEXT_QTD_PTR & ~EHCI_QTD_T_BIT);
            } /* EndWhile */
            if (QH_ptr->NEXT_QTD_LINK_PTR & EHCI_QTD_T_BIT)
            {
                /* No alternate descriptor */
                QH_ptr->ALT_NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;
                QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)first_QTD_ptr;
            }
            else
            {
                temp_QTD_ptr->NEXT_QTD_PTR = (uint_32)first_QTD_ptr;
            } /* Endif */
        } /* Endif */

    } /* Endif */

    if (init_async_list)
    {
        /* Write the QH address to the Current Async List Address */
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CURR_ASYNC_LIST_ADDR = (uint_32)QH_ptr;
    } /* Endif */

    active_list_member_ptr->QH_PTR = (EHCI_QH_STRUCT_PTR)QH_ptr;
    active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)first_QTD_ptr;
    active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = NULL;

    /* Enable the Asynchronous schedule if:
    ** if asynchronous_schedule_enable == asynchronous_schedule_status
    */
    cmd_val = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD;
    if ((cmd_val & EHCI_USBCMD_ASYNC_SCHED_ENABLE) ==
            (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS &
             EHCI_STS_ASYNCH_SCHEDULE))
    {
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD = (cmd_val |
                EHCI_USBCMD_ASYNC_SCHED_ENABLE);
    } /* Endif */

    return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_port_change
*  Returned Value : None
*  Comments       :
*        Process port change
*END*-----------------------------------------------------------------*/

boolean _usb_ehci_process_port_change
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
    VUSB20_REG_STRUCT_PTR               dev_ptr;
    VUSB20_REG_STRUCT_PTR               cap_dev_ptr;
    uint_8                              i, total_port_numbers;
    uint_32                             port_control, status;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    cap_dev_ptr = (VUSB20_REG_STRUCT_PTR)
                  _bsp_get_usb_capability_register_base(usb_host_ptr->DEV_NUM);

    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    total_port_numbers =
        cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS & EHCI_HCS_PARAMS_N_PORTS;


    for (i = 0; i < total_port_numbers; i++)
    {
        port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i];

        if (port_control & EHCI_PORTSCX_CONNECT_STATUS_CHANGE)
        {

            /* Turn on the 125 usec uframe interrupt. This effectively
            ** starts the timer to count 125 usecs
            */
            dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR |=  EHCI_INTR_SOF_UFRAME_EN;
            do
            {
                if (port_control & EHCI_PORTSCX_CONNECT_STATUS_CHANGE)
                {
                    usb_host_ptr->UFRAME_COUNT = 0;
                    port_control =
                        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i];
                    port_control &= ~EHCI_PORTSCX_W1C_BITS;
                    port_control |= EHCI_PORTSCX_CONNECT_STATUS_CHANGE;
                    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i] = port_control;
                } /* Endif */
                status = (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS &
                          dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR);
                if (status & EHCI_STS_SOF_COUNT)
                {
                    /* Increment the 125 usecs count */
                    usb_host_ptr->UFRAME_COUNT++;
                    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS = status;
                } /* Endif */
                port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i];
            }
            while (usb_host_ptr->UFRAME_COUNT != 2);
            /* Turn off the 125 usec uframe interrupt. This effectively
            ** stops the timer to count 125 usecs
            */
            dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR &=
                ~EHCI_INTR_SOF_UFRAME_EN;

            port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i];

            usb_host_ptr->UFRAME_COUNT = 0;

            /* We waited to check for stable current connect status */
            if (port_control & EHCI_PORTSCX_CURRENT_CONNECT_STATUS)
            {
                /* Attach on port i */
                /* send change report to the hub-driver */
                /* The hub driver does GetPortStatus and identifies the new connect */
                /* usb_host_ptr->ROOT_HUB_DRIVER(handle, hub_num, GET_PORT_STATUS); */
                /* reset and enable the port */
                if (usb_host_ptr->DEV_NUM != USB_HSIC1_PORT)
                    _usb_ehci_reset_and_enable_port(handle, i);
            }
            else
            {
                /* Detach on port i */
                /* send change report to the hub-driver */
                /* usb_host_ptr->ROOT_HUB_DRIVER(handle, hub_num, GET_PORT_STATUS); */

                /* clear the connect status change */
                port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i];
                port_control &= ~EHCI_PORTSCX_W1C_BITS;
                port_control |= EHCI_PORTSCX_CONNECT_STATUS_CHANGE;
                dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i] = port_control;

                /* Disable the asynch and periodic schedules */
                dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD &=
                    ~(EHCI_USBCMD_ASYNC_SCHED_ENABLE |
                      EHCI_USBCMD_PERIODIC_SCHED_ENABLE);

                if ((_usb_host_check_service((pointer)usb_host_ptr,
                                             USB_SERVICE_DETACH, 0)) == USB_OK)
                {
                    _usb_host_call_service(usb_host_ptr, USB_SERVICE_DETACH, 0);
                }
                else
                {
                    /* call device detach (host pointer, speed, hub #, port #) */
                    usb_dev_list_detach_device((pointer)usb_host_ptr, 0, (i + 1));
                } /* Endif */

                if (!i)
                {
                    return TRUE;
                } /* Endif */
            } /* Endif */
        } /* Endif */

        if (port_control & EHCI_PORTSCX_PORT_FORCE_RESUME)
        {
            port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i];
            port_control &= ~EHCI_PORTSCX_W1C_BITS;
            port_control &= ~EHCI_PORTSCX_PORT_FORCE_RESUME;
            _usb_host_call_service((pointer)usb_host_ptr, USB_SERVICE_HOST_RESUME, 0);
        } /* Endif */

        /* Hardware rev 4.0 require changes on port control bits. Enable patch_3 to
        take affect of required changes.*/

        //if (usb_host_ptr->DEV_NUM == USB_HSIC1_PORT)
        //if (hsic_phy_end_bit < 2)
        //    return;
#if 1   //def PATCH_3
        if (port_control & EHCI_PORTSCX_PORT_ENABLE)
#else
        if ((port_control & EHCI_PORTSCX_PORT_EN_DIS_CHANGE) &&
                (port_control & EHCI_PORTSCX_PORT_ENABLE))
#endif
        {
            /* reset process complete */
            /* Report the change to the hub driver and enumerate */
            usb_host_ptr->SPEED = ((port_control & VUSBHS_SPEED_MASK) >> VUSBHS_SPEED_BIT_POS);
            usb_host_ptr->PORT_NUM = (i + 1);

            /* Now wait for reset recovery */
            usb_host_ptr->RESET_RECOVERY_TIMER = (USB_RESET_RECOVERY_DELAY * 8 * 100);

            /* Turn on the 125 usec uframe interrupt. This effectively
            ** starts the timer to count 125 usecs
            */
            dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR |=
                EHCI_INTR_SOF_UFRAME_EN;
        } /* Endif */

    } /* Endfor */

    return FALSE;

} /* EndBody */

#ifdef HNP_HARDWARE_ASSISTANCE

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_hnp_hW_assisted_detach
*  Returned Value : None
*  Comments       :
*        Processes a device detach under HNP hardware assistance.
* Note that this routine assumes root hub and port 0
*END*-----------------------------------------------------------------*/

void _usb_ehci_process_hnp_hW_assisted_detach
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    if ((_usb_host_check_service((pointer)usb_host_ptr,
                                 USB_SERVICE_DETACH, 0)) == USB_OK)
    {
        _usb_host_call_service(usb_host_ptr, USB_SERVICE_DETACH, 0);
    }
    else
    {
        /* call device detach (host pointer, speed, hub #, port #) */
        usb_dev_list_detach_device((pointer)usb_host_ptr, 0, 1);
    }

    return;

} /* EndBody */

#endif

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_reset_and_enable_port
*  Returned Value : None
*  Comments       :
*        Reset and enabled the port
*END*-----------------------------------------------------------------*/

void _usb_ehci_reset_and_enable_port
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle,

    /* [IN] the port number */
    uint_8                  port_number
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
    VUSB20_REG_STRUCT_PTR               dev_ptr;
    uint_32                             port_status_control;
    //uint_32                     auto_reset;
    // clean warning USB_STATUS                          status;
    // clean warning boolean                             auto_reset = FALSE;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    dev_ptr      = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    /* Check the line status bit in the PORTSC register */
    port_status_control =
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[port_number];

    port_status_control &= ~EHCI_PORTSCX_W1C_BITS;

#ifdef __USB_OTG__
    /* reset should wait for 100 Ms debouce period before starting*/
    uint_32                                      auto_reset;
    if ((usb_otg_state_struct_ptr->STATE_STRUCT_PTR->STATE != A_SUSPEND) &&
            (usb_otg_state_struct_ptr->STATE_STRUCT_PTR->STATE != B_WAIT_ACON))
    {
        uint_32 i;

        for (i = 0; i < USB_DEBOUNCE_DELAY; i++)
        {
            _usb_host_delay(handle, 1);

            /* Check the line status bit in the PORTSC register */
            port_status_control =
                dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[port_number];

            if (!(port_status_control & EHCI_PORTSCX_CURRENT_CONNECT_STATUS))
            {
                return;
            } /* Endif */
        } /* Endfor */
    }


    /*if auto reset bit is OFF, drive a reset */
    IS_AUTO_RESET_ON(dev_ptr, auto_reset);

    if (!auto_reset)
    {
        /*
        ** Start the reset process
        */
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[port_number] =
            (port_status_control | EHCI_PORTSCX_PORT_RESET);
    }
    /*ensure that auto reset is off */
    AUTO_RESET_OFF(dev_ptr);

#else
    /*
    ** Start the reset process
    */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[port_number] =
        (port_status_control | EHCI_PORTSCX_PORT_RESET);

#endif




#if 0
    /* when reset is driven, hardware takes atleast 50 mili second
    to reset. We take this time to initialize the memory required
    by driver */
    status = _usb_hci_init_driver_memory(handle);
#endif


} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_process_reset_recovery_done
*  Returned Value : None
*  Comments       :
*        Reset and enabled the port
*END*-----------------------------------------------------------------*/

void _usb_host_process_reset_recovery_done
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
    VUSB20_REG_STRUCT_PTR               dev_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    /* call device attach (host pointer, speed, hub #, port #) */
    if ((_usb_host_check_service((pointer)usb_host_ptr,
                                 USB_SERVICE_ATTACH, usb_host_ptr->SPEED)) == USB_OK)
    {
        _usb_host_call_service(usb_host_ptr, USB_SERVICE_ATTACH, usb_host_ptr->SPEED);
    }
    else
    {
        usb_dev_list_attach_device((pointer)usb_host_ptr,
                                   usb_host_ptr->SPEED, 0, usb_host_ptr->PORT_NUM);
    } /* Endif */
    /* Turn off the 125 usec uframe interrupt. This effectively
    ** stops the timer to count 125 usecs
    */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR &=
        ~EHCI_INTR_SOF_UFRAME_EN;
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hci_vusb20_isr
*  Returned Value : None
*  Comments       :
*        Service all the interrupts in the VUSB1.1 hardware
*END*-----------------------------------------------------------------*/
void   _usb_ehci_error_process( _usb_host_handle handle)
{

    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr;
    EHCI_QH_STRUCT_PTR                           QH_ptr;
    EHCI_QTD_STRUCT_PTR                          QTD_ptr;
    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    /***************************************************************************
     SEARCH THE NON PERIODIC LIST FIRST
    ***************************************************************************/

    /* Get the head of the active queue head */
    active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;

    if (active_list_member_ptr)
    {
        bbu_printf(" error_process_qh_list_tr_complete \r\n");

        QH_ptr = active_list_member_ptr->QH_PTR;
        //Get the first QTD for this Queue head
        QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;
        //This is a valid qTD
        if (((uint_32)QTD_ptr) & EHCI_QTD_T_BIT)
            bbu_printf("(error)QTD_ptr) is 0x%x\r\n", (uint_32)QTD_ptr);
        /* bbu_printf("(error)QTD_ptr) is 0x%x\r\n",(uint_32)QTD_ptr);

            bbu_printf("_usb_ehci_process_qh_error: QTD =%x\r\n\
                                  Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n\r",
                                  QTD_ptr->TOKEN,
                                  ((QTD_ptr->TOKEN)&0xFF),
                                  ((QTD_ptr->TOKEN) >> 8)&0x3,
                                  ((QTD_ptr->TOKEN) >> 10) &0x3,
                                  ((QTD_ptr->TOKEN) >> 12)&0x7,
                                  ((QTD_ptr->TOKEN) >> 15)&0x1,
                                  ((QTD_ptr->TOKEN) >> 16)&0x7FFF,
                                  ((QTD_ptr->TOKEN)&EHCI_QTD_DATA_TOGGLE) >>31);*/

        _usb_ehci_process_qh_list_tr_complete(handle, active_list_member_ptr);
    }


    //_usb_ehci_process_qh_list_tr_complete(handle,active_list_member_ptr);









}
#ifdef __USB_OS_MQX__
void _usb_hci_vusb20_isr
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle
)
#else
void HOST_INTERRUPT_ROUTINE_KEYWORD _usb_hci_vusb20_isr
(
    void
)
#endif
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
    VUSB20_REG_STRUCT_PTR               dev_ptr;
    uint_32                             status;
    // clean warning volatile int    i;


#ifdef __USB_OTG__
#ifdef   HNP_HARDWARE_ASSISTANCE
    OTG_STRUCT_PTR  otg_reg;
    boolean  auto_hnp;
#endif
#endif

#ifdef __USB_OS_MQX__
    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
#else
#ifndef __USB_OTG__
    IN_ISR = TRUE;
#endif
    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)usb_host_state_struct_ptr;
#endif

    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    /* We use a while loop to process all the interrupts while we are in the
     ** loop so that we don't miss any interrupts
     */
    //  while (TRUE)
    //  {

    /* Check if any of the enabled interrupts are asserted */
    status = (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS &
              dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR);

    //bbu_printf(" USB INTR STATUS : \r\n");
    //TODO: the status is always 0, USB_INTR=0x37  USBSTS=0x88
    if (!status)
    {
        /* No more interrupts pending to be serviced */
        usbID_wk_isr();
        return ;
        //break;
    } /* Endif */

    //bbu_printf(" USB INTR STATUS : ");
    //BBU_puthexd(status);
    ///BBU_puthexd(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR);
    //BBU_puthexd(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS);
    //bbu_printf("\r\n");


    /* Clear all enabled interrupts */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS = status;

    if (status & EHCI_STS_SOF_COUNT)
    {
        /* Waiting for an interval of 10 ms for reset recovery */
        if (usb_host_ptr->RESET_RECOVERY_TIMER)
        {
            usb_host_ptr->RESET_RECOVERY_TIMER--;
            if (!usb_host_ptr->RESET_RECOVERY_TIMER)
            {
                bbu_printf("EHCI_STS_SOF_COUNT: _usb_host_process_reset_recovery_done\r\n");
                _usb_host_process_reset_recovery_done((pointer)usb_host_ptr);
            } /* Endif */
        } /* Endif */
    } /* Endif */

    if (status & EHCI_STS_ASYNCH_ADVANCE)
    {
        /* Process the asynch advance */
    } /* Endif */

    //    if (status & EHCI_STS_HOST_SYS_ERROR) {
    /* Host system error. Controller halted. Inform the upper layers */
    //    _usb_host_call_service((pointer)usb_host_ptr, USB_SERVICE_SYSTEM_ERROR, 0);
    // } /* Endif */

    if (status & EHCI_STS_FRAME_LIST_ROLLOVER)
    {
        /* Process frame list rollover */
    } /* Endif */

    if (status & EHCI_STS_RECLAIMATION)
    {
        /* Process reclaimation */
    } /* Endif */

    if (status & EHCI_STS_PORT_CHANGE)
    {
        /* if it is aprocess port change under HNP, just return
        because it is an spurious interrupt */
        bbu_printf("Port Change Interrupt\r\n");
#ifdef __USB_OTG__
#ifdef   HNP_HARDWARE_ASSISTANCE
        auto_hnp = FALSE;
        otg_reg = ((USB_OTG_STATE_STRUCT_PTR)usb_otg_state_struct_ptr)->OTG_REG_PTR;
        IS_AUTO_HNP_ON(otg_reg, auto_hnp);
        if (auto_hnp)
        {
            /* Body */
            ignore = TRUE;
            return;
        } /* Endbody */
#endif
#endif


        /* Process the port change detect */
        if (_usb_ehci_process_port_change((pointer)usb_host_ptr))
        {
            /* There was a detach on port 0 so we should return */
            return;
        } /* Endif */
        /* Should return if there was a detach on OTG port */
    } /* Endif */

    if (status & EHCI_STS_USB_INTERRUPT )
    {
        /* Process the USB transaction completion and transaction error
        ** interrupt
        */
        //   bbu_printf(" EHCI_STS_USB_INTERRUPT \r\n");
        _usb_ehci_process_tr_complete((pointer)usb_host_ptr);

#ifdef DEBUG_INFO
        //bbu_printf("TR completed\r\n");
#endif

    } /* Endif */

    if (status & EHCI_STS_USB_ERROR )
    {
        /* Process the USB transaction completion and transaction error
        ** interrupt
        */

        //  _usb_ehci_process_tr_complete((pointer)usb_host_ptr);
        bbu_printf(" EHCI_STS_USB_ERROR!!!!! \r\n");
#ifdef DEBUG_INFO
        //  bbu_printf("TR completed\r\n");
#endif

    } /* Endif */

    // } /* EndWhile */

#ifndef __USB_OS_MQX__
#ifndef __USB_OTG__
    IN_ISR = FALSE;
#endif
#endif

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hci_vusb20_init
*  Returned Value : error or USB_OK
*  Comments       :
*        Initialize the VUSB_HS controller
*END*-----------------------------------------------------------------*/

#define GPIO_USB_PW_PIN             0
#define GPIO_USB_PW_MASK            (1 << GPIO_USB_PW_PIN)

//#define SET_USB_PW(val) msi_wl_WriteGPIOs(GPIO_USB_PW_PIN, val)
extern void print_phy(void);
//extern void pm830_otg_en(int enable_disable);



#if 0
void otg_pm830_en(int enable_disable)
{
    uint8_t reg;
    int ret;
    // struct charger_driver *drv = &pm830_charger_driver;
    if (enable_disable)
    {
        //charger_read(drv, 0x3c, &reg);
        ret = i2c_read_addr8_byte(PI2C, 0xd0, 0x3c, &reg, ICR_MODE_STD);
        if (ret == 0)
            bbu_printf("charger_read: i2c read fail,   i2c: %d, i2c_slave_addr: 0x%x\n\r", PI2C, 0xd0);
        reg |= BIT_7;
        //charger_write(drv, 0x3c, reg);
        ret = i2c_write_addr8_byte(PI2C, 0xd0, 0x3c, reg, ICR_MODE_STD);
        if (ret == 0)
            bbu_printf("charger_read: i2c read fail,   i2c: %d, i2c_slave_addr: 0x%x\n\r", PI2C, 0xd0);
        bbu_printf("enable OTG mode ... \n\r");
    }
    else
    {
        //charger_read(drv, 0x3c, &reg);
        ret = i2c_read_addr8_byte(PI2C, 0xd0, 0x3c, &reg, ICR_MODE_STD);
        if (ret == 0)
            bbu_printf("charger_read: i2c read fail,   i2c: %d, i2c_slave_addr: 0x%x\n\r", PI2C, 0xd0);
        reg &= ~BIT_7;
        //charger_write(drv, 0x3c, reg);
        ret = i2c_write_addr8_byte(PI2C, 0xd0, 0x3c, reg, ICR_MODE_STD);
        if (ret == 0)
            bbu_printf("charger_read: i2c read fail,   i2c: %d, i2c_slave_addr: 0x%x\n\r", PI2C, 0xd0);
        bbu_printf("disable OTG mode ... \n\r");

    }
}

void _closk_ic_usb_int(void)
{

    USB_MISC_SPH->USBSPH_MISC_CTRL &= (~MISC_CTRL_xcvr_udrdis);
    USB_MISC_SPH->USBSPH_MISC_CTRL |= MISC_CTRL_pll_lock;



}

void  _turn_power_18(void)
{
    AIBAux->USIM_IO_REG |= USIM_IO_REG_PDB;
    AIBAux->USIM_IO_REG |= USIM_IO_REG_V18EN;



}


void  _turn_power_33(void)
{
    if ((ic_choose_vol == FALSE))
    {
        mdelay(25);
        if (ic_choose_vol == TRUE)
            bbu_printf("support 1.8v power\r\n");
        else
        {
            ic_choose_vol = TRUE;
            AIBAux->USIM_IO_REG &= (USIM_IO_REG_V18EN | USIM_IO_REG_V25EN);
            bbu_printf("support 3.3v power\r\n");

        }
    }

}


void usb_host_clk_enable(void)//by tongchao

{
    PMUA_TypeDef *pmua = PMUA;

    pmua->USB_CLK_RES_CTRL |= USB_CLK_RES_CTRL_USB_SPH_AXICLK_EN;

    pmua->USB_CLK_RES_CTRL |= USB_CLK_RES_CTRL_USB_SPH_AXI_RST;

}
#endif


static void timer_irq(void);


void set_vbus(int enable)
{
#define FAN5405_CHARGER_SLAVE_ADDR  0xD4

#define FAN5405_CHARGER_ENABLE 1
#define FAN5405_CHARGER_DISABLE 0

#define REG_CONTROL0        0x00
#define REG_CONTROL1        0x01
#define REG_OREG            0x02
#define REG_IC_INFO         0x03
#define REG_IBAT            0x04
#define REG_SP_CHARGER      0x05
#define REG_SAFEFY          0x6
#define REG_MONITOR         0x10

#define FAN5405_ERROR_CHARGING  -2
#define FAN5405_CHARGING_ITERM 1
#define FAN5405_30mA_LINEAR_CHARGING 2
#define FAN5405_ICHARGER_LOOP_CHARGING 3
#define FAN5405_IBUS_LOOP_CHARGING 4
#define FAN5405_CONSTANT_LOOP_CHARGING 5



    uint8_t value, value1;
    uint32_t mode = ICR_MODE_STD;

    i2c_read_addr8_byte(PI2C, FAN5405_CHARGER_SLAVE_ADDR, REG_CONTROL1, &value, mode);
    i2c_read_addr8_byte(PI2C, FAN5405_CHARGER_SLAVE_ADDR, REG_OREG, &value1, mode);

    gpio_output_high(GPIO21);

    if (enable)
    {
        value1 |= BIT_0;
        i2c_write_addr8_byte(PI2C, FAN5405_CHARGER_SLAVE_ADDR, REG_OREG, value1, mode);
        // fan5405_otg_enabled = 1;
        //fan5405_t32_kick_notify();
    }
    else
    {

        value1 &= ~BIT_0;
        value &= 0xFC;
        i2c_write_addr8_byte(PI2C, FAN5405_CHARGER_SLAVE_ADDR, REG_OREG, value1, mode);
        i2c_write_addr8_byte(PI2C, FAN5405_CHARGER_SLAVE_ADDR, REG_CONTROL1, value, mode);
        //fan5405_otg_enabled = 0;
    }

    start_timer(20000, timer_irq, TIMER_PERIODIC);

}
static void reset_t32sec(void)
{
    uint8_t value;
    uint32_t mode = ICR_MODE_STD;
    //value = BIT_7|BIT_6;  //value =BIT_6;
    i2c_read_addr8_byte(PI2C, FAN5405_CHARGER_SLAVE_ADDR, REG_CONTROL0, &value, mode);
    value = value | BIT_7;
    i2c_write_addr8_byte(PI2C, FAN5405_CHARGER_SLAVE_ADDR, REG_CONTROL0, value, mode);
}

static void timer_irq(void)
{

    reset_t32sec() ;
    // bbu_printf("timer irq 20s \r\n ");
}

extern void usb_sph_phy_host_private(void);

uint_8  _usb_hci_vusb20_init
(
    /* the USB Host State structure */
    _usb_host_handle     handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    VUSB20_REG_STRUCT_PTR                        dev_ptr;
    VUSB20_REG_STRUCT_PTR                        cap_dev_ptr;
    uint_8                                       vector;
    uint_32                                      frame_list_size_bits, port_control;
    // uint_32                  i;
    USB_STATUS                                   status;

    // uint_32                                      /* clean warning delay, */temp;
#ifndef __USB_OTG__
    // uint_32                                      port_control;
#endif
    //  unsigned int ttc_chip_id;
    // int is_ttc_y0 = 0;

    udelay(100);
    //  temp = BU_REG_READ( CYG_DEVICE_USBPHY_CTRL );
    //  BU_REG_WRITE( CYG_DEVICE_USBPHY_CTRL, temp | (1<<REG_BIT_PU_PLL) | (1<<REG_BIT_PU)) ;




    // usb_host_clk_enable();
    // usb2_phy_init();
    // _closk_ic_usb_int();




    //  bbu_printf("enable OTG mode ... \n\r");


    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    if ( usb_host_ptr->DEV_NUM == 0)
    {
        usb2_phy_init();
        // set_vbus(1);
    }
    //  else if(usb_host_ptr->DEV_NUM==1)
    //      {
    //  usb_sph_phy_init(0);
    //          }


    cap_dev_ptr  = (VUSB20_REG_STRUCT_PTR)
                   _bsp_get_usb_capability_register_base(usb_host_ptr->DEV_NUM);

    /* Get the base address of the VUSB_HS registers */
    usb_host_ptr->DEV_PTR = (pointer) ((uint_32)cap_dev_ptr +
                                       (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.CAPLENGTH_HCIVER &
                                        EHCI_CAP_LEN_MASK));
    // bbu_printf("CAPLENGTH_HCIVER address is 0x%x\r\n",&(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.CAPLENGTH_HCIVER));
    //bbu_printf("0xc0001100 is 0x%x\r\n",*test&0xff);

    //bbu_printf("0xd4208100 is 0x%x\r\n",*test1&0xff);
    // bbu_printf("usb_host_ptr->DEV_PTR=0x%x\r\n",usb_host_ptr->DEV_PTR);
    dev_ptr = usb_host_ptr->DEV_PTR;

    /*Initialize the memory required for driver to operate */
#if 1
    status = _usb_hci_init_driver_memory(handle);
    if (status != USB_OK) return status;
#endif


#ifndef NO_ISR_INSTALLATION
    /* Get the interrupt vector number for the VUSB_HS host */
    vector = _bsp_get_usb_vector(usb_host_ptr->DEV_NUM);

#ifndef __USB_OTG__

#ifdef __USB_OS_MQX__
    if (!(USB_install_isr(vector, _usb_hci_vusb20_isr, (pointer)usb_host_ptr)))
    {
        return USBERR_INSTALL_ISR;
    } /* Endbody */
#else
    //USB_install_isr(vector, _usb_hci_vusb20_isr, (pointer)usb_host_ptr);
#endif /* USB_OS_MQX */

#endif /* __USB_OTG__ */

#endif /* NO_ISR_INSTALLATION */

    //IRQ_Glb_Ena();
    //  INT_Enable( INT_USB, IRQ_ROUTE, USB_PRIORITY );
    //ISR_Connect(INT_USB, _usb_hci_vusb20_isr );

    /*******************************************************************
     Set the size of frame list in CMD register
    *******************************************************************/
    usb_host_ptr->FRAME_LIST_SIZE = MAX_EHCI_FRAME_LIST_SIZE;
    frame_list_size_bits = MAX_EHCI_FRAME_LIST_BITS;

    /*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
     HARDWARE  REGISTERS INITIALIZATION STUFF
    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
    USB_lock();

    /*LED CHECK AFTER DEVICE SHUTDOWN but BEFORE HOST CONTROLLER RESET*/
#ifdef TEST_WRITE
    /* LED check point */
    (*USB_REGISTER_TO_WRITE) |= 0xC000;
#endif
    if (
        (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS & EHCI_STS_HC_HALTED)
#ifdef __USB_OTG__
#ifdef HNP_HARDWARE_ASSISTANCE
        || (usb_otg_state_struct_ptr->STATE_STRUCT_PTR->B_HNP_ENABLE == TRUE)
#endif
#endif
    )
    {
        /* Reset the controller to get default values */
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD = EHCI_CMD_CTRL_RESET;

        while (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD &
                EHCI_CMD_CTRL_RESET)
        {
            /* Wait for the controller reset to complete */
        } /* EndWhile */
    } /* Endif */

    /* Stop the controller */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD &=
        ~EHCI_CMD_RUN_STOP;

    /*LED CHECK AFTER HOST CONTROLLER RESET BUT BEFORE MODE SWITCH*/
#ifdef TEST_WRITE
    /* LED check point */
    (*USB_REGISTER_TO_WRITE) |= 0xC000;
#endif


    /* Configure the VUSBHS has a host controller */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_MODE =
        VUSBHS_MODE_CTRL_MODE_HOST;

    /*LED CHECK AFTER MODE SWITCH*/
#ifdef TEST_WRITE
    (*USB_REGISTER_TO_WRITE) |= 0xC000;
#endif

    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CTRLDSSEGMENT = 0;

    bbu_printf("cap_dev_ptr is 0x%x\r\n", cap_dev_ptr);
    /* 4-Gigabyte segment where all of the  interface data structures are allocated. */
    /* If no 64-bit addressing capability then this is zero */
    if ((cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCC_PARAMS &
            EHCI_HCC_PARAMS_64_BIT_ADDR_CAP))
    {

        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CTRLDSSEGMENT = 0;
    }
    else
    {
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CTRLDSSEGMENT =
            EHCI_DATA_STRUCTURE_BASE_ADDRESS;
    } /* Endif */

    //#ifdef __USB_OTG__
    if (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS &
            VUSB20_HCS_PARAMS_PORT_POWER_CONTROL_FLAG)
    {
        port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0];
        port_control &= ~EHCI_PORTSCX_W1C_BITS;
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0] =
            (port_control | EHCI_PORTSCX_PORT_POWER);
    }
    // dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0] |=
    //    EHCI_PORTSCX_PORT_POWER;
    uint_32          cmd_val;
    if (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCC_PARAMS & EHCI_HCC_PARAMS_ASYNC_PARK_CAP)
    {
        cmd_val = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD;
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD = cmd_val | EHCI_CMD_ASYNCHRNOUS_PARK_ENABLE |
                EHCI_CMD_ASYNCHRONOUS_COUNT;

    } /* Endif */
    //#endif


    //TODO: check this
    //dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR = VUSB20_HOST_INTR_EN_BITS | 0x00000080;
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR = VUSB20_HOST_INTR_EN_BITS;//0xBF;


    if (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCC_PARAMS &
            EHCI_HCC_PARAMS_PGM_FRM_LIST_FLAG)
    {
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD =
            (EHCI_INTR_NO_THRESHOLD_IMMEDIATE | frame_list_size_bits | EHCI_CMD_RUN_STOP);

    }
    else
    {
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD =
            (EHCI_INTR_NO_THRESHOLD_IMMEDIATE | EHCI_CMD_RUN_STOP);
    } /* Endif */

    //APBSPARE->PLL5_SW_CTRL |= (IC_USB_N_PD_EN|IC_USB_P_PD_EN);


    USB_unlock();

    /* route all ports to the EHCI controller */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CONFIG_FLAG = 1;

#if 0
    bbu_printf("USB_INTR is 0x%x\r\n", & (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR));
    bbu_printf("USB_INTR is 0x%x\r\n", (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR));
    bbu_printf("USB_CMD is 0x%x\r\n", & (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD));
    bbu_printf("USB_CMD is 0x%x\r\n", (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD));
    bbu_printf("PORTSCX[0] is 0x%x\r\n", & (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0]));
    bbu_printf("PORTSCX[0] is 0x%x\r\n", (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0]));
#endif


    //  print_phy();
    //#endif
    return USB_OK;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hci_init_driver_memory
*  Returned Value : error or USB_OK
*  Comments       :
*        Initialize all the memory required by Host controller driver to
* operate.
*END*-----------------------------------------------------------------*/

USB_STATUS  _usb_hci_init_driver_memory
(
    /* the USB Host State structure */
    _usb_host_handle     handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    //   VUSB20_REG_STRUCT_PTR                        dev_ptr;
    EHCI_QH_STRUCT_PTR                           QH_ptr;
    EHCI_QTD_STRUCT_PTR                          QTD_ptr;
    uint_32                                      i;
    SCRATCH_STRUCT_PTR                           temp_scratch_ptr;
    // clean warning uint_32_ptr                                  temp_periodic_list_ptr = NULL;
    pointer                                      total_mem_ptr = NULL;
    uint_32                                      total_non_periodic_memory = 0;
    pointer                                      non_periodic_mem_ptr = NULL;
    uint_32                                      total_periodic_memory = 0;
    pointer                                      periodic_mem_ptr = NULL;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    //  dev_ptr = usb_host_ptr->DEV_PTR;



    /*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
       ALL CONTROLLER DRIVER MEMORY NEEDS are allocated here.
    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

    total_non_periodic_memory += (MAX_QH_DESCRS * sizeof(EHCI_QH_STRUCT)) + 32;
    total_non_periodic_memory += (MAX_QTD_DESCRS * sizeof(EHCI_QTD_STRUCT)) + 32;
    total_non_periodic_memory += sizeof(SCRATCH_STRUCT) * MAX_QTD_DESCRS;
    total_non_periodic_memory += sizeof(SCRATCH_STRUCT) * MAX_QH_DESCRS;

    total_periodic_memory += usb_host_ptr->FRAME_LIST_SIZE * sizeof(uint_8_ptr) * 8 * sizeof(uint_8);
    total_periodic_memory += (sizeof(EHCI_FRAME_LIST_ELEMENT_POINTER)) * usb_host_ptr->FRAME_LIST_SIZE + 4096;

    /*memory required by high-speed Iso transfers */
    total_periodic_memory += (MAX_ITD_DESCRS * sizeof(EHCI_ITD_STRUCT)) + 32;
    total_periodic_memory +=  sizeof(SCRATCH_STRUCT) * MAX_ITD_DESCRS;
    total_periodic_memory +=  sizeof(LIST_NODE_STRUCT) * MAX_ITD_DESCRS;
    /*memory required by full-speed Iso transfers */
    total_periodic_memory +=  (MAX_SITD_DESCRS * sizeof(EHCI_SITD_STRUCT)) + 32;
    total_periodic_memory +=  sizeof(SCRATCH_STRUCT) * MAX_SITD_DESCRS;
    total_periodic_memory +=  sizeof(LIST_NODE_STRUCT) * MAX_SITD_DESCRS;

    //   total_mem_ptr = USB_Uncached_memalloc(total_non_periodic_memory+total_periodic_memory);
    total_mem_ptr = (pointer)USB_memalloc(total_non_periodic_memory + total_periodic_memory);
    if (!total_mem_ptr)
    {
        return USBERR_ALLOC;
    }

#ifdef _DATA_CACHE_
    /* memzero the whole memory */
    USB_dev_memzero(total_mem_ptr, (total_non_periodic_memory + total_periodic_memory));
    USB_dcache_flush_mlines(
        (uint_32)total_mem_ptr,
        (total_non_periodic_memory + total_periodic_memory)
    );
#endif

    /* store the memory block pointer in device handle*/
    usb_host_ptr->CONTROLLER_MEMORY = total_mem_ptr;

    /*divide memory between periodic and non periodic memories */
    non_periodic_mem_ptr = total_mem_ptr;
    periodic_mem_ptr = (uint_8_ptr) total_mem_ptr + total_non_periodic_memory;



    /*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
       NON-PERIODIC MEMORY DISTRIBUTION STUFF
    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/




    usb_host_ptr->ASYNC_LIST_BASE_ADDR = usb_host_ptr->QH_BASE_PTR =
            (pointer) non_periodic_mem_ptr;

    usb_host_ptr->ALIGNED_ASYNCLIST_ADDR = usb_host_ptr->QH_ALIGNED_BASE_PTR =
            (pointer)USB_MEM32_ALIGN((uint_32)usb_host_ptr->ASYNC_LIST_BASE_ADDR);



    non_periodic_mem_ptr =  (uint_8_ptr) non_periodic_mem_ptr +
                            (MAX_QH_DESCRS * sizeof(EHCI_QH_STRUCT)) + 32;
    usb_host_ptr->QTD_BASE_PTR = (EHCI_QTD_STRUCT_PTR) non_periodic_mem_ptr;

    /* Align the QTD base to 32 byte boundary */
    usb_host_ptr->QTD_ALIGNED_BASE_PTR = (EHCI_QTD_STRUCT_PTR)
                                         USB_MEM32_ALIGN((uint_32)usb_host_ptr->QTD_BASE_PTR);



    non_periodic_mem_ptr =  (uint_8_ptr) non_periodic_mem_ptr +
                            (MAX_QTD_DESCRS * sizeof(EHCI_QTD_STRUCT)) + 32;
    usb_host_ptr->QTD_SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR) non_periodic_mem_ptr;


    non_periodic_mem_ptr =  (uint_8_ptr) non_periodic_mem_ptr +
                            sizeof(SCRATCH_STRUCT) * MAX_QTD_DESCRS;
    usb_host_ptr->QH_SCRATCH_STRUCT_BASE =  (SCRATCH_STRUCT_PTR)non_periodic_mem_ptr;



    QH_ptr = usb_host_ptr->QH_ALIGNED_BASE_PTR;

    temp_scratch_ptr = usb_host_ptr->QH_SCRATCH_STRUCT_BASE;

    for (i = 0; i < MAX_QH_DESCRS; i++)
    {
        QH_ptr->SCRATCH_PTR = temp_scratch_ptr;
        /* Set the dTD to be invalid */
        QH_ptr->HORIZ_LINK_PTR = EHCI_QUEUE_HEAD_POINTER_T_BIT;
        _usb_hci_vusb20_free_QH((pointer)usb_host_ptr, (pointer)QH_ptr);
        QH_ptr++;
        temp_scratch_ptr++;
    } /* Endfor */

    QTD_ptr = usb_host_ptr->QTD_ALIGNED_BASE_PTR;

    temp_scratch_ptr = usb_host_ptr->QTD_SCRATCH_STRUCT_BASE;

    /* Enqueue all the QTDs */
    for (i = 0; i < MAX_QTD_DESCRS; i++)
    {
        QTD_ptr->SCRATCH_PTR = temp_scratch_ptr;
        /* Set the QTD to be invalid */
        QTD_ptr->NEXT_QTD_PTR = EHCI_QTD_T_BIT;
        _usb_hci_vusb20_free_QTD((pointer)usb_host_ptr, (pointer)QTD_ptr);
        QTD_ptr++;
        temp_scratch_ptr++;
    } /* Endfor */



    /*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
       BANDWIDTH MEMORY DISTRIBUTION STUFF
    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/


    /*********************************************************************
    Allocating the memory to store periodic bandwidth. A periodic BW list
    is a two dimensional array with dimension (frame list size x 8 u frames).

    Also note that the following could be a large allocation of memory
    The max value stored in a location will be 125 micro seconds and so we
    use uint_8
    *********************************************************************/

    usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR = (uint_8_ptr) periodic_mem_ptr;
    periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr +
                       usb_host_ptr->FRAME_LIST_SIZE * sizeof(uint_8_ptr) * 8 * sizeof(uint_8);

    USB_dev_memzero(usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR,
                    usb_host_ptr->FRAME_LIST_SIZE *  \
                    sizeof(uint_8_ptr) * \
                    8 *            \
                    sizeof(uint_8)  \
                   );



    usb_host_ptr->PERIODIC_LIST_BASE_ADDR = periodic_mem_ptr;
    periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr +
                       (sizeof(EHCI_FRAME_LIST_ELEMENT_POINTER)) *
                       usb_host_ptr->FRAME_LIST_SIZE + 4096;
    usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR =
        (pointer)USB_MEM4096_ALIGN((uint_32)usb_host_ptr->PERIODIC_LIST_BASE_ADDR);

    /*make sure that periodic list is uninitialized */
    usb_host_ptr->PERIODIC_LIST_INITIALIZED = FALSE;

    /* Initialize the list of active structures to NULL initially */
    usb_host_ptr->ACTIVE_ASYNC_LIST_PTR = NULL;
    usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR = NULL;


    /*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
       HIGH SPEED ISO TRANSFERS MEMORY ALLOCATION STUFF
    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

    usb_host_ptr->ITD_BASE_PTR = (EHCI_ITD_STRUCT_PTR) periodic_mem_ptr;
    periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr +
                       (MAX_ITD_DESCRS * sizeof(EHCI_ITD_STRUCT)) + 32;


    /* Align the ITD base to 32 byte boundary */
    usb_host_ptr->ITD_ALIGNED_BASE_PTR = (EHCI_ITD_STRUCT_PTR)
                                         USB_MEM32_ALIGN((uint_32)usb_host_ptr->ITD_BASE_PTR);


    usb_host_ptr->ITD_SCRATCH_STRUCT_BASE =  (SCRATCH_STRUCT_PTR) periodic_mem_ptr;
    periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr +
                       sizeof(SCRATCH_STRUCT) * MAX_ITD_DESCRS;


    usb_host_ptr->ITD_LIST_INITIALIZED = FALSE;

    /*****************************************************************************
      ITD QUEUING PREPARATION
    *****************************************************************************/

    /* memory for doubly link list of nodes that keep active ITDs. Head and Tail point to
    same place when list is empty */
    usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR = periodic_mem_ptr;
    usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR = periodic_mem_ptr;
    USB_dev_memzero(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
                    sizeof(LIST_NODE_STRUCT)*MAX_ITD_DESCRS);

    periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr +
                       sizeof(LIST_NODE_STRUCT) * MAX_ITD_DESCRS;


    /*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
       FULL SPEED ISO TRANSFERS MEMORY ALLOCATION STUFF
    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

    /* Allocate the Split-transactions Isochronous Transfer Descriptors:
    ** 32 bytes aligned
    */
    usb_host_ptr->SITD_BASE_PTR = (EHCI_SITD_STRUCT_PTR) periodic_mem_ptr;
    periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr +
                       (MAX_SITD_DESCRS * sizeof(EHCI_SITD_STRUCT)) + 32;


    /* Align the SITD base to 32 byte boundary */
    usb_host_ptr->SITD_ALIGNED_BASE_PTR = (EHCI_SITD_STRUCT_PTR)
                                          USB_MEM32_ALIGN((uint_32)usb_host_ptr->SITD_BASE_PTR);

    usb_host_ptr->SITD_SCRATCH_STRUCT_BASE =  (SCRATCH_STRUCT_PTR) periodic_mem_ptr;
    periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr +
                       sizeof(SCRATCH_STRUCT) * MAX_SITD_DESCRS;

    usb_host_ptr->SITD_LIST_INITIALIZED = FALSE;

    /*****************************************************************************
      SITD QUEUING PREPARATION
    *****************************************************************************/

    /* memory for doubly link list of nodes that keep active SITDs. Head and Tail point to
    same place when list is empty */
    usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR = periodic_mem_ptr;
    usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR = periodic_mem_ptr;

    USB_dev_memzero(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
                    sizeof(LIST_NODE_STRUCT)*MAX_SITD_DESCRS);

    periodic_mem_ptr = (uint_8_ptr) periodic_mem_ptr +
                       sizeof(LIST_NODE_STRUCT) * MAX_SITD_DESCRS;

    return USB_OK;

}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hci_vusb20_free_QTD
*  Returned Value : void
*  Comments       :
*        Enqueues an QTD onto the free QTD ring.
*
*END*-----------------------------------------------------------------*/

void _usb_hci_vusb20_free_QTD
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle,

    /* [IN] the QTD to enqueue */
    pointer                 QTD_ptr
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    /*
    ** This function can be called from any context, and it needs mutual
    ** exclusion with itself.
    */
    USB_lock();

    ((EHCI_QTD_STRUCT_PTR)QTD_ptr)->NEXT_QTD_PTR = EHCI_QTD_T_BIT;

    /*
    ** Add the QTD to the free QTD queue (linked via PRIVATE) and
    ** increment the tail to the next descriptor
    */
    EHCI_QTD_QADD(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL,
                  (EHCI_QTD_STRUCT_PTR)QTD_ptr);
    usb_host_ptr->QTD_ENTRIES++;

    USB_unlock();

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_hci_vusb20_free_QH
*  Returned Value : void
*  Comments       :
*        Enqueues a QH onto the free QH ring.
*
*END*-----------------------------------------------------------------*/

void _usb_hci_vusb20_free_QH
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle,

    /* [IN] the QH to enqueue */
    pointer                 QH_ptr
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    /*
    ** This function can be called from any context, and it needs mutual
    ** exclusion with itself.
    */
    USB_lock();

    /*
    ** Add the QH to the free QH queue (linked via PRIVATE) and
    ** increment the tail to the next descriptor
    */

    EHCI_QH_QADD(usb_host_ptr->QH_HEAD, usb_host_ptr->QH_TAIL,
                 (EHCI_QH_STRUCT_PTR)QH_ptr);
    usb_host_ptr->QH_ENTRIES++;

    USB_unlock();

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_free_resources
*  Returned Value : none
*  Comments       :
*        Frees the controller specific resources for a given pipe
*END*-----------------------------------------------------------------*/

void _usb_ehci_free_resources
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle,

    /* The pipe descriptor to queue */
    PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr, temp_ptr;
    ACTIVE_QH_MGMT_STRUCT_PTR                    prev_ptr = NULL, next_ptr = NULL;
    EHCI_QH_STRUCT_PTR                           QH_ptr, temp_QH_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    QH_ptr = pipe_descr_ptr->QH_FOR_THIS_PIPE;


    if (pipe_descr_ptr->PIPETYPE > USB_INTERRUPT_PIPE)
    {

        /************************************************************
        Close the pipe on Async transfers (Bulk and Control)
        ************************************************************/
        /* Get the head of the active queue head */
        active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;


        while (active_list_member_ptr)
        {

            temp_QH_ptr = (EHCI_QH_STRUCT_PTR)
                          ((uint_32)active_list_member_ptr->QH_PTR &
                           EHCI_QH_HORIZ_PHY_ADDRESS_MASK);

            next_ptr = active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
            temp_ptr = active_list_member_ptr;
            _usb_hci_vusb20_free_QH(handle, (pointer) QH_ptr);

            if (temp_QH_ptr == QH_ptr)
            {
                if (prev_ptr)
                {
                    prev_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = next_ptr;
                }
                else
                {
                    active_list_member_ptr = next_ptr;
                    usb_host_ptr->ACTIVE_ASYNC_LIST_PTR = next_ptr;
                } /* Endif */
                /* Found it */
                USB_memfree(temp_ptr);

                break;
            } /* Endif */

            prev_ptr = active_list_member_ptr;

            active_list_member_ptr =
                active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
        } /* Endwhile */
    }
    else if (pipe_descr_ptr->PIPETYPE == USB_INTERRUPT_PIPE)
    {
        _usb_ehci_close_interrupt_pipe(handle, pipe_descr_ptr);
    }
    else if (pipe_descr_ptr->PIPETYPE == USB_ISOCHRONOUS_PIPE)
    {
        _usb_ehci_close_isochronous_pipe(handle, pipe_descr_ptr);
    }

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_tr_complete
*  Returned Value : None
*  Comments       :
*     Process the Transaction Done interrupt on the EHCI hardware. Note that this
*     routine should be improved later. It is needless to search all the lists
*     since interrupt will belong to only one of them at one time.
*
*END*-----------------------------------------------------------------*/

void _usb_ehci_process_tr_complete
(
    /* [IN] the USB Host state structure */
    _usb_host_handle           handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    /***************************************************************************
     SEARCH THE NON PERIODIC LIST FIRST
    ***************************************************************************/

    /* Get the head of the active queue head */
    active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
    if (active_list_member_ptr)
    {
        //bbu_printf(" _usb_ehci_process_qh_list_tr_complete \r\n");
        _usb_ehci_process_qh_list_tr_complete(handle, active_list_member_ptr);
    }

    /***************************************************************************
     SEARCH THE INTERRUPT LIST
    ***************************************************************************/

    /* Get the head of the active structures for periodic list*/
    active_list_member_ptr = usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;
    if (active_list_member_ptr)
    {
        bbu_printf(" _usb_ehci_process_qh_interrupt_tr_complete \r\n");
        _usb_ehci_process_qh_interrupt_tr_complete(handle, active_list_member_ptr);
    }

    /***************************************************************************
     SEARCH THE HIGH SPEED ISOCHRONOUS LIST
    ***************************************************************************/
    if (usb_host_ptr->HIGH_SPEED_ISO_QUEUE_ACTIVE)
    {
        bbu_printf(" _usb_ehci_process_itd_tr_complete \r\n");
        _usb_ehci_process_itd_tr_complete(handle);
    }

    /***************************************************************************
     SEARCH THE FULL SPEED ISOCHRONOUS LIST
    ***************************************************************************/
    if (usb_host_ptr->FULL_SPEED_ISO_QUEUE_ACTIVE)
    {
        bbu_printf(" _usb_ehci_process_sitd_tr_complete \r\n");
        _usb_ehci_process_sitd_tr_complete(handle);
    }
#if 0
    static int i = 0;
    if (i <= 7)
    {
        i++;
        // usb_controller_read();
        // bbu_printf("status[0] is 0x%x\r\n",(MAVL_USB_SPH_DEV->U2XUSBSTS));

    }
#endif
} /* Endbody */

/* EOF */




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_qh_list_tr_complete
*  Returned Value : None
*  Comments       :
*     Search the asynchronous or interrupt list to see which QTD had finished and
*     Process the interrupt.
*
*END*-----------------------------------------------------------------*/

static void _usb_ehci_process_qh_list_tr_complete
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle,

    ACTIVE_QH_MGMT_STRUCT_PTR        active_list_member_ptr
)
{
    /* Body */
    //   USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    //   VUSB20_REG_STRUCT_PTR                        dev_ptr;
    EHCI_QH_STRUCT_PTR                           QH_ptr;
    EHCI_QTD_STRUCT_PTR                          QTD_ptr;
    EHCI_QTD_STRUCT_PTR                          temp_QTD_ptr;
    PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_descr_ptr = NULL;
    PIPE_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
    boolean                                      first_phase = FALSE;
    uint_32                                      total_req_bytes = 0;
    uint_32                                      remaining_bytes = 0;
    uint_32                                      errors = 0, status = 0;
    uchar_ptr                                    buffer_ptr = NULL;
    boolean                                      active_qtd_found = FALSE;

    //  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    //  dev_ptr = usb_host_ptr->DEV_PTR;

    /* Check all transfer descriptors on all active queue heads */
    do
    {
        /* Get the queue head from the active list */
        QH_ptr = active_list_member_ptr->QH_PTR;
        /* Get the first QTD for this Queue head */
        QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;
        /* This is a valid qTD */
        while ((!(((uint_32)QTD_ptr) & EHCI_QTD_T_BIT)) &&
                (active_qtd_found == FALSE))
        {

            if (!(QTD_ptr->TOKEN & EHCI_QTD_STATUS_ACTIVE))
            {

                /* Update the active list */
                active_list_member_ptr->FIRST_QTD_PTR =
                    (EHCI_QTD_STRUCT_PTR)QTD_ptr->NEXT_QTD_PTR;


#if 0
                //  bbu_printf("QTD done Token=%x,Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n" );
#endif

                /* Get the pipe descriptor for this transfer */
                pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)QTD_ptr->PIPE_DESCR_FOR_THIS_QTD;
                pipe_tr_struct_ptr = (PIPE_TR_STRUCT_PTR)QTD_ptr->TR_FOR_THIS_QTD;

                /* Check for errors */
                if (QTD_ptr->TOKEN & EHCI_QTD_ERROR_BITS_MASK)
                {
                    errors |= (QTD_ptr->TOKEN & EHCI_QTD_ERROR_BITS_MASK);
                    QTD_ptr->TOKEN &= ~EHCI_QTD_ERROR_BITS_MASK;
                    status = USBERR_TR_FAILED;
                } /* Error */

                /* Check if STALL or endpoint halted because of errors */
                if (QTD_ptr->TOKEN & EHCI_QTD_STATUS_HALTED)
                {
                    errors |= EHCI_QTD_STATUS_HALTED;
                    status = USBERR_ENDPOINT_STALLED;
                    QTD_ptr->TOKEN &= ~EHCI_QTD_STATUS_HALTED;
                    QH_ptr->STATUS = 0;
                } /* Endif */

                if (pipe_descr_ptr->PIPETYPE == USB_CONTROL_PIPE)
                {

                    if (QTD_ptr->TOKEN & EHCI_QTD_PID_SETUP)
                    {
                        first_phase = TRUE;
                    }
                    else if (first_phase)
                    {
                        first_phase = FALSE;
                        if (pipe_tr_struct_ptr->SEND_PHASE)
                        {
                            total_req_bytes = pipe_tr_struct_ptr->TX_LENGTH;
                            buffer_ptr = pipe_tr_struct_ptr->TX_BUFFER;
                            pipe_tr_struct_ptr->SEND_PHASE = FALSE;
                        }
                        else
                        {
                            total_req_bytes = pipe_tr_struct_ptr->RX_LENGTH;
                            buffer_ptr = pipe_tr_struct_ptr->RX_BUFFER;
                        } /* Endif */
                        /* Increment the total number of bytes sent/received */
                        remaining_bytes = ((QTD_ptr->TOKEN & EHCI_QTD_LENGTH_BIT_MASK) >>
                                           EHCI_QTD_LENGTH_BIT_POS);
                    } /* Endif */
                }
                else
                {

                    if (pipe_descr_ptr->DIRECTION)
                    {
                        total_req_bytes = pipe_tr_struct_ptr->TX_LENGTH;
                        buffer_ptr = pipe_tr_struct_ptr->TX_BUFFER;
                    }
                    else
                    {
                        total_req_bytes = pipe_tr_struct_ptr->RX_LENGTH;
                        buffer_ptr = pipe_tr_struct_ptr->RX_BUFFER;
                    } /* Endif */
                    remaining_bytes = ((QTD_ptr->TOKEN & EHCI_QTD_LENGTH_BIT_MASK) >>
                                       EHCI_QTD_LENGTH_BIT_POS);

                } /* Endif */



                if (QTD_ptr->TOKEN & EHCI_QTD_IOC)
                {
                    /* total number of bytes sent/received */
                    pipe_descr_ptr->SOFAR += (total_req_bytes - remaining_bytes);
                    pipe_tr_struct_ptr->status = USB_STATUS_IDLE;
                } /* Endif */

                temp_QTD_ptr = QTD_ptr;

                QTD_ptr = (EHCI_QTD_STRUCT_PTR)QTD_ptr->NEXT_QTD_PTR;

                /* Queue the transfer onto the relevant queue head */
                QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)QTD_ptr;

                /* Dequeue the used QTD */
                _usb_hci_vusb20_free_QTD(handle, (pointer)temp_QTD_ptr);

                /* queue the transfer only if it is a valid transfer */

                /* Queue the transfer onto the relevant queue head */
                QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)QTD_ptr;

                if (QH_ptr->NEXT_QTD_LINK_PTR & EHCI_QTD_T_BIT)
                {
                    /* No alternate descriptor */
                    QH_ptr->ALT_NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;

                    /* Clear the status field */
                    QH_ptr->STATUS &= EHCI_QH_TR_OVERLAY_DT_BIT;
                } /* Endif */

                /* Update the new first transfer descriptor for the
                ** processed queue
                */
                active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)QTD_ptr;

                /* Now mark it as unused so that next request can use it */
                pipe_tr_struct_ptr->TR_INDEX = 0;

                if (pipe_tr_struct_ptr->status == USB_STATUS_IDLE)
                {
                    /* Transfer done. Call the callback function for this
                    ** transaction if there is one (usually true).
                    */
                    if (pipe_tr_struct_ptr->CALLBACK != NULL)
                    {
#ifdef DEBUG_INFO
                        bbu_printf("_usb_ehci_process_qh_list_tr_complete: Callback\r\n");
#endif
                        pipe_tr_struct_ptr->CALLBACK((pointer)pipe_descr_ptr,
                                                     pipe_tr_struct_ptr->CALLBACK_PARAM,
                                                     buffer_ptr,
                                                     (total_req_bytes - remaining_bytes),
                                                     status);

                        /* If the application enqueued another request on this pipe
                        ** in this callback context then it will be at the end of the list
                        */
                    } /* Endif */
                    errors = 0;
                    status = 0;
                } /* Endif */


            } /* Endif */
            else
            {
                /*******************************************
                If we found an active transfer we should break
                out of the loop.
                *******************************************/
                bbu_printf("it is a active transfer\r\n");
                active_qtd_found = TRUE;
                break;
            }


        } /* EndWhile */

#ifdef DEBUG_INFO
        bbu_printf("_usb_ehci_process_qh_list_tr_complete: next active QH\r\n");
#endif

        active_list_member_ptr =  active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

    }
    while (active_list_member_ptr);   /* While there are more queue
                                       ** heads in the active list */


} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_qh_interrupt_tr_complete
*  Returned Value : None
*  Comments       :
*     Search the interrupt list to see which QTD had finished and
*     Process the interrupt.
*
*END*-----------------------------------------------------------------*/

static void _usb_ehci_process_qh_interrupt_tr_complete
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle,

    ACTIVE_QH_MGMT_STRUCT_PTR        active_list_member_ptr
)
{
    /* Body */
    //  USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    //   VUSB20_REG_STRUCT_PTR                        dev_ptr;
    EHCI_QH_STRUCT_PTR                           QH_ptr;
    EHCI_QTD_STRUCT_PTR                          QTD_ptr;
    EHCI_QTD_STRUCT_PTR                          temp_QTD_ptr;
    PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_descr_ptr = NULL;
    PIPE_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
    // clean warning boolean                                      first_phase = FALSE;
    uint_32                                      total_req_bytes = 0;
    uint_32                                      remaining_bytes = 0;
    uint_32                                      errors = 0, status = 0;
    uchar_ptr                                    buffer_ptr = NULL;

    //  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    //  dev_ptr = usb_host_ptr->DEV_PTR;

    /* Check all transfer descriptors on all active queue heads */
    do
    {
        /* Get the queue head from the active list */
        QH_ptr = active_list_member_ptr->QH_PTR;
        /* Get the first QTD for this Queue head */
        QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;


        while ((!(((uint_32)QTD_ptr) & EHCI_QTD_T_BIT)) && (QTD_ptr != NULL))
        {
            /* This is a valid qTD */


#ifdef DEBUG_INFO
            /*    printf("_usb_ehci_process_qh_interrupt_tr_complete: QTD =%x\n\
                         Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n",
                         QTD_ptr->TOKEN,
                         ((QTD_ptr->TOKEN)&0xFF),
                         ((QTD_ptr->TOKEN) >> 8)&0x3,
                         ((QTD_ptr->TOKEN) >> 10) &0x3,
                         ((QTD_ptr->TOKEN) >> 12)&0x7,
                         ((QTD_ptr->TOKEN) >> 15)&0x1,
                         ((QTD_ptr->TOKEN) >> 16)&0x7FFF,
                         ((QTD_ptr->TOKEN)&EHCI_QTD_DATA_TOGGLE) >>31);
                         */
#endif

            if (!(QTD_ptr->TOKEN & EHCI_QTD_STATUS_ACTIVE))
            {

                /* Get the pipe descriptor for this transfer */
                pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)QTD_ptr->\
                                 PIPE_DESCR_FOR_THIS_QTD;
                pipe_tr_struct_ptr = (PIPE_TR_STRUCT_PTR)QTD_ptr->\
                                     TR_FOR_THIS_QTD;

                /* Check for errors */
                if (QTD_ptr->TOKEN & EHCI_QTD_ERROR_BITS_MASK)
                {
                    errors |= (QTD_ptr->TOKEN & EHCI_QTD_ERROR_BITS_MASK);
                    QTD_ptr->TOKEN &= ~EHCI_QTD_ERROR_BITS_MASK;
                    status = USBERR_TR_FAILED;
                } /* Error */

                /* Check if STALL or endpoint halted because of errors */
                if (QTD_ptr->TOKEN & EHCI_QTD_STATUS_HALTED)
                {
                    errors |= EHCI_QTD_STATUS_HALTED;
                    status = USBERR_ENDPOINT_STALLED;
                    QTD_ptr->TOKEN &= ~EHCI_QTD_STATUS_HALTED;
                    QH_ptr->STATUS = 0;
                } /* Endif */

                if (pipe_descr_ptr->DIRECTION)
                {
                    total_req_bytes = pipe_tr_struct_ptr->TX_LENGTH;
                    buffer_ptr = pipe_tr_struct_ptr->TX_BUFFER;
                }
                else
                {
                    total_req_bytes = pipe_tr_struct_ptr->RX_LENGTH;
                    buffer_ptr = pipe_tr_struct_ptr->RX_BUFFER;
                } /* Endif */
                remaining_bytes = ((QTD_ptr->TOKEN & EHCI_QTD_LENGTH_BIT_MASK) >>
                                   EHCI_QTD_LENGTH_BIT_POS);

#ifdef DEBUG_INFO
                /*      printf("_usb_ehci_process_qh_interrupt_tr_complete: Requested Bytes = %d\
                      ,Remaining bytes = %d,",total_req_bytes,remaining_bytes);
                      */
#endif

                if (QTD_ptr->TOKEN & EHCI_QTD_IOC)
                {
                    /* total number of bytes sent/received */
                    pipe_descr_ptr->SOFAR += (total_req_bytes - remaining_bytes);
                    pipe_tr_struct_ptr->status = USB_STATUS_IDLE;
                } /* Endif */

                temp_QTD_ptr = QTD_ptr;

                QTD_ptr = (EHCI_QTD_STRUCT_PTR)QTD_ptr->NEXT_QTD_PTR;

                /* Queue the transfer onto the relevant queue head */
                QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)QTD_ptr;
                if (QH_ptr->NEXT_QTD_LINK_PTR & EHCI_QTD_T_BIT)
                {
                    /* No alternate descriptor */
                    QH_ptr->ALT_NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;

                    /* Clear the status field */
                    QH_ptr->STATUS &= EHCI_QH_TR_OVERLAY_DT_BIT;
                } /* Endif */


                active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)QTD_ptr;

                /* Dequeue the used QTD */
                _usb_hci_vusb20_free_QTD(handle, (pointer)temp_QTD_ptr);

                /* Now mark it as unused so that next request can use it */
                pipe_tr_struct_ptr->TR_INDEX = 0;

                if (pipe_tr_struct_ptr->status == USB_STATUS_IDLE)
                {
                    /* Transfer done. Call the callback function for this
                    ** transaction if there is one (usually true).
                    */
                    if (pipe_tr_struct_ptr->CALLBACK != NULL)
                    {
                        pipe_tr_struct_ptr->CALLBACK((pointer)pipe_descr_ptr,
                                                     pipe_tr_struct_ptr->CALLBACK_PARAM,
                                                     buffer_ptr,
                                                     (total_req_bytes - remaining_bytes),
                                                     status);

                        /* If the application enqueued another request on this pipe
                        ** in this callback context then it will be at the end of the list
                        */
                    } /* Endif */
                    errors = 0;
                    status = 0;
                } /* Endif */


            } /* Endif */
            else
            {
                /* Body */

                break;
            } /* Endbody */


        } /* EndWhile */

        active_list_member_ptr =
            active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

    }
    while (active_list_member_ptr);   /* While there are more queue
                                       ** heads in the active list */


} /* Endbody */


#ifdef __USB_OTG__

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_delay
*  Returned Value : None
*  Comments       :
*        Delay for specified number of milliseconds.
*END*-----------------------------------------------------------------*/

void _usb_host_delay
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle,

    /* [IN] time to wait in ms */
    uint_32 delay
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR              usb_host_ptr;
    VUSB20_REG_STRUCT _PTR_                dev_ptr;
    uint_32                                start_frame = 0, curr_frame = 0, diff = 0, i = 0, j = 0;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    dev_ptr      = (VUSB20_REG_STRUCT _PTR_)usb_host_ptr->DEV_PTR;

    /* Get the frame number (not the uframe number */
    start_frame = _usb_ehci_get_frame_number(handle);

    /*wait till frame numer exceeds by delay mili seconds.*/
    do
    {
        curr_frame = _usb_ehci_get_frame_number(handle);
        i++;
        if (curr_frame != start_frame)
        {
            diff++;
            start_frame =  curr_frame;
            j++;
        }

    }
    while (diff < delay);


#if 0

    volatile uint_32              delay_count;
    OTG_STRUCT_PTR                otg_reg;
    uint_32                       otg_status;
    uint_32                       curr_toggle_bit = 0;
    uint_32                       new_toggle_bit = 0;
    volatile VUSB20_REG_STRUCT _PTR_       dev_ptr;
    uint_32
    USB_OTG_STATE_STRUCT_PTR usb_otg_ptr = (USB_OTG_STATE_STRUCT_PTR)usb_otg_state_struct_ptr;
    int test1[101], test2[101];
    int i = 0, j = 0;

    otg_reg = usb_otg_ptr->OTG_REG_PTR;
    dev_ptr = usb_otg_ptr->USB_REG_PTR;
    delay_count = delay;

    /*On host we should use Frindex to count time.*/


    /* Forced to use OTG timer because no other timer available */
    /*START_TIMER(otg_reg);*/

    /* Get the status of enabled interrupts */
    /*otg_status = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.OTGSC;*/

    /*
    check the status of the toggle bit.
    curr_toggle_bit = otg_status & VUSBHS_OTGSC_1MST;
    */
    /*while (delay_count)
    {
    */
    /*
       wait till toggle bit changes
       do
       {
    */
    /* Get the status of enabled interrupts */
    /*otg_status = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.OTGSC;*/

    /*check the status of the toggle bit.*/
    /*new_toggle_bit = otg_status & VUSBHS_OTGSC_1MST;*/

    /*j++;*/

    /*}while(new_toggle_bit == curr_toggle_bit);*/

    /* curr_toggle_bit = new_toggle_bit;*/
    /*test1[i] = curr_toggle_bit;*/


    /*delay_count--;*/


    j++;

    if (otg_status & VUSBHS_OTGSC_1MSIS)
        /*if (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.OTGSC & VUSBHS_OTGSC_1MSIS) */
    {
        /* Clear the timer*/
        i++;
        test1[i] = otg_status;
        otg_status |=  VUSBHS_OTGSC_1MSIS;
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.OTGSC = otg_status;
        test2[i] = otg_status;

        /*         otg_status &= ~VUSBHS_OTGSC_1MSIS; */
        /*delay_count--; */
        /*}*/


    }
    /*STOP_TIMER(otg_reg);*/

#endif


} /* Endbody */


#endif

/* EOF */




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_itd_tr_complete
*  Returned Value : None
*  Comments       :
*     Search the ITD list to see which ITD had finished and
*     Process the interrupt.
*
*END*-----------------------------------------------------------------*/

static void _usb_ehci_process_itd_tr_complete
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    //   VUSB20_REG_STRUCT _PTR_                      dev_ptr;
    EHCI_ITD_STRUCT_PTR                          ITD_ptr;
    PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_descr_ptr = NULL;
    PIPE_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
    uint_32                                      /* clean warning total_req_bytes = 0,*/ no_of_scheduled_slots;
    /* clean warning uint_32                                      remaining_bytes = 0;*/
    uint_32                                      /* clean warning errors = 0, */status = 0;
    uchar_ptr                                    buffer_ptr = NULL;
    LIST_NODE_STRUCT_PTR                         node_ptr, prev_node_ptr, next_node_ptr;
    uint_8                                       transaction_number, page_number;
    boolean                                      pending_transaction;
    uint_32                                      offset, length_transmitted;
    uint_32_ptr                                  prev_link_ptr = NULL, next_link_ptr = NULL;


    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    // dev_ptr = usb_host_ptr->DEV_PTR;


    /******************************************************************
    Search the ITD list starting from head till we find inactive nodes.
    Note that for Head there is no previous node so we can disntiguish
    it from rest of the list.
    ******************************************************************/
    prev_node_ptr = node_ptr =  usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR;

    /* loop till current node is active or node is a head node*/
    while ((prev_node_ptr->next_active && (prev_node_ptr->next != NULL))
            || ((node_ptr->prev == NULL) && (node_ptr->member != NULL)))
    {

        ITD_ptr =  (EHCI_ITD_STRUCT_PTR) node_ptr->member;
#ifdef  __USB_OTG__

#ifdef HOST_TESTING
        /*
        usb_otg_state_struct_ptr->STATUS_STARTS[usb_otg_state_struct_ptr->LOG_ITD_COUNT]
                        = ITD_ptr->status;
        */
#endif
#endif


        pipe_tr_struct_ptr =  (PIPE_TR_STRUCT_PTR) ITD_ptr->PIPE_TR_DESCR_FOR_THIS_ITD;

        pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR) ITD_ptr->PIPE_DESCR_FOR_THIS_ITD;

        /* assume that no transactions are pending on this ITD */
        pending_transaction = FALSE;

        no_of_scheduled_slots = 0;

        /**************************************************************
        Check the status of every transaction inside the ITD.
        **************************************************************/
        for (transaction_number = 0; transaction_number < 8; transaction_number++)
        {

            /**************************************************************
            Note that each iteration of this loop checks the micro frame
            number on which transaction is scheduled. If a micro frame was
            not allocated for this pipe, we don't need to check it. But
            caution is that, there could be a transaction that was too
            small and even though many bandwidth slots are available
            but this transaction was finished only in 1 of the slots. Thus
            we also keep a check of how many transactions were allocated for
            this ITD.
            **************************************************************/

            if ((pipe_descr_ptr->BWIDTH_SLOTS[transaction_number]) &&
                    (no_of_scheduled_slots < ITD_ptr->number_of_transactions))
            {


                no_of_scheduled_slots++;

                status = ITD_ptr->TR_STATUS_CTL_LIST[transaction_number]
                         & EHCI_ITD_STATUS;

                /* if transaction is not active and IOC was set we look in to it else we move on */
                if ((!(status & EHCI_ITD_STATUS_ACTIVE)) &&
                        (ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] & EHCI_ITD_IOC_BIT))
                {

                    /* send callback to app with the status*/
                    if (pipe_tr_struct_ptr->CALLBACK != NULL)
                    {
                        offset =  ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] &
                                  EHCI_ITD_BUFFER_OFFSET;

                        page_number =  (ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] &
                                        EHCI_ITD_PAGE_NUMBER) >> EHCI_ITD_PG_SELECT_BIT_POS;

                        length_transmitted = (ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] &
                                              EHCI_ITD_LENGTH_TRANSMITTED) >> EHCI_ITD_LENGTH_BIT_POS;

                        buffer_ptr = (uchar_ptr) ((ITD_ptr->BUFFER_PAGE_PTR_LIST[page_number] &
                                                   EHCI_ITD_BUFFER_POINTER) >> EHCI_ITD_PG_SELECT_BIT_POS);


                        pipe_tr_struct_ptr->CALLBACK(
                            (pointer)pipe_descr_ptr,
                            pipe_tr_struct_ptr->CALLBACK_PARAM,
                            (buffer_ptr + offset),
                            length_transmitted,
                            status);
                    }


                }
                /* if IOC is set and status is active, we have a pending transaction */
                else if ((status & EHCI_ITD_STATUS_ACTIVE) &&
                         (ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] & EHCI_ITD_IOC_BIT))
                {
                    /* This means that this ITD has a pending transaction */
                    pending_transaction = TRUE;
                }


            }

        }



        /* If this ITD is done with all transactions, time to free it */
        if (!pending_transaction)
        {

            /* if we are freeing a head node, we move node_ptr to next head
            or else we move normally*/

            if (node_ptr ==  usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR)
            {
                /*free this node */
                EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
                                     usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR,
                                     node_ptr);

                prev_node_ptr = node_ptr = usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR;
            }
            else
            {
                /*save next node */
                next_node_ptr = node_ptr->next;

                /*free current node */
                EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
                                     usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR,
                                     node_ptr);

                /*move to next node now */
                node_ptr = next_node_ptr;
                prev_node_ptr = node_ptr->prev;

            }

            /*(*ITD_ptr->prev_data_struct_ptr) = ITD_ptr->next_data_struct_value;*/
#ifdef  __USB_OTG__
#ifdef HOST_TESTING
            usb_otg_state_struct_ptr->STATUS[usb_otg_state_struct_ptr->LOG_ITD_COUNT]
                = status;

            USB_memcopy((uchar_ptr)ITD_ptr,
                        &usb_otg_state_struct_ptr->LOG_INTERRUPT_ITDS[usb_otg_state_struct_ptr->LOG_ITD_COUNT]
                        , 8);

            usb_otg_state_struct_ptr->LOG_ITD_COUNT++;
            if (usb_otg_state_struct_ptr->LOG_ITD_COUNT > HOST_LOG_MOD)
                usb_otg_state_struct_ptr->LOG_ITD_COUNT = 0;

#endif
#endif

            /*remove the ITD from periodic list */
            prev_link_ptr = next_link_ptr =  ITD_ptr->frame_list_ptr;
            /*iterate the list while we find valid pointers (1 means invalid pointer) */
            while (!((*next_link_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
            {
                /*if a pointer matches our ITD we remove it from list*/
                if (((*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK) ==  (uint_32) ITD_ptr)
                {
                    (*prev_link_ptr) = (uint_32)ITD_ptr->NEXT_LINK_PTR;
                    break;
                }

                prev_link_ptr = next_link_ptr;
                next_link_ptr = (uint_32_ptr) ((*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK);
            }



            /* free the ITD used */
            _usb_hci_vusb20_free_ITD((pointer)usb_host_ptr, (pointer)ITD_ptr);

            /* subtract on how many ITDs are pending from this transfer */
            pipe_tr_struct_ptr->no_of_itds_sitds -= 1;

            /* if all ITDS are served free the TR INDEX */
            if (pipe_tr_struct_ptr->no_of_itds_sitds == 0)
            {
                /* Mark TR as unused so that next request can use it */
                pipe_tr_struct_ptr->TR_INDEX = 0;

            }

        }
        else
        {
            /* move to next ITD in the list */
            prev_node_ptr = node_ptr;
            node_ptr = node_ptr->next;

        }



    }/* end while loop */


} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_sitd_tr_complete
*  Returned Value : None
*  Comments       :
*     Search the SITD list to see which SITD had finished and
*     Process the interrupt.
*
*END*-----------------------------------------------------------------*/

static void _usb_ehci_process_sitd_tr_complete
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    //   VUSB20_REG_STRUCT _PTR_             dev_ptr;
    EHCI_SITD_STRUCT_PTR                SITD_ptr;
    PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_descr_ptr = NULL;
    PIPE_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
    uint_32                                      status = 0;
    uchar_ptr                                    buffer_ptr = NULL;
    LIST_NODE_STRUCT_PTR                         node_ptr, prev_node_ptr, next_node_ptr;
    uint_32                                      length_transmitted;
    uint_32_ptr                                  prev_link_ptr = NULL, next_link_ptr = NULL;


    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    //dev_ptr = usb_host_ptr->DEV_PTR;

    /******************************************************************
    Search the SITD list starting from head till we find inactive nodes.
    Note that for Head there is no previous node so we can disntiguish
    it from rest of the list.
    ******************************************************************/
    prev_node_ptr = node_ptr =  usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR;

    /* loop till current node is active or node is a head node*/
    while ((prev_node_ptr->next_active && (prev_node_ptr->next != NULL))
            || ((node_ptr->prev == NULL) && (node_ptr->member != NULL)))
    {

        SITD_ptr =  (EHCI_SITD_STRUCT_PTR) node_ptr->member;

        pipe_tr_struct_ptr =  (PIPE_TR_STRUCT_PTR) SITD_ptr->PIPE_TR_DESCR_FOR_THIS_SITD;

        pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR) SITD_ptr->PIPE_DESCR_FOR_THIS_SITD;

        /*grab the status and check it */
        status = SITD_ptr->TRANSFER_STATE & EHCI_SITD_STATUS;

        /* if transaction is not active we look in to it else we move on */
        if (!(status & EHCI_SITD_STATUS_ACTIVE))
        {

            /* send callback to app with the status*/
            if (pipe_tr_struct_ptr->CALLBACK != NULL)
            {

                length_transmitted = SITD_ptr->TRANSFER_STATE & EHCI_SITD_LENGTH_TRANSMITTED;

                buffer_ptr = (uchar_ptr) (SITD_ptr->BUFFER_PTR_0 & EHCI_SITD_BUFFER_POINTER);


                pipe_tr_struct_ptr->CALLBACK(
                    (pointer)pipe_descr_ptr,
                    pipe_tr_struct_ptr->CALLBACK_PARAM,
                    buffer_ptr,
                    length_transmitted,
                    status);
            }


            /*********************************************************************
             Since status is Non active for this SITD, time to delete it.
            *********************************************************************/

            /* if we are freeing a head node, we move node_ptr to next head
            or else we move normally */

            if (node_ptr ==  usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR)
            {
                /*free this node */
                EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
                                     usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
                                     node_ptr);

                prev_node_ptr = node_ptr = usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR;
            }
            else
            {
                /*save next node */
                next_node_ptr = node_ptr->next;

                /*free current node */
                EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
                                     usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
                                     node_ptr);

                /*move to next node now */
                node_ptr = next_node_ptr;
                prev_node_ptr = node_ptr->prev;

            }

#ifdef  __USB_OTG__

#ifdef HOST_TESTING
            usb_otg_state_struct_ptr->STATUS[usb_otg_state_struct_ptr->LOG_SITD_COUNT]
                = status;

            USB_memcopy((uchar_ptr)SITD_ptr,
                        &usb_otg_state_struct_ptr->LOG_INTERRUPT_SITDS[usb_otg_state_struct_ptr->LOG_SITD_COUNT]
                        , 44);

            usb_otg_state_struct_ptr->LOG_SITD_COUNT++;
            if (usb_otg_state_struct_ptr->LOG_SITD_COUNT > HOST_LOG_MOD)
                usb_otg_state_struct_ptr->LOG_SITD_COUNT = 0;

#endif
#endif

            /*remove the SITD from periodic list */
            prev_link_ptr = next_link_ptr =  SITD_ptr->frame_list_ptr;
            /*iterate the list while we find valid pointers (1 means invalid pointer) */
            while (!((*next_link_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
            {
                /*if a pointer matches our SITD we remove it from list*/
                if (((*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK) ==  (uint_32) SITD_ptr)
                {
                    (*prev_link_ptr) = (uint_32)SITD_ptr->NEXT_LINK_PTR;
                    break;
                }

                prev_link_ptr = next_link_ptr;
                next_link_ptr = (uint_32_ptr) ((*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK);
            }



            /* free the SITD used */
            _usb_hci_vusb20_free_SITD((pointer)usb_host_ptr, (pointer)SITD_ptr);

            /* subtract on how many SITDs are pending from this transfer */
            pipe_tr_struct_ptr->no_of_itds_sitds -= 1;

            /* if all SITDS are served free the TR INDEX */
            if (pipe_tr_struct_ptr->no_of_itds_sitds == 0)
            {
                /* Mark TR as unused so that next request can use it */
                pipe_tr_struct_ptr->TR_INDEX = 0;

            }



        }
        /* else move on to the next node in the queue */
        else
        {

            prev_node_ptr = node_ptr;
            node_ptr = node_ptr->next;

        }


#if 0
        /* save the next node */
        next_node_ptr =

            /*free this node */
            EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
                                 usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
                                 node_ptr);

        /* subtract on how many ITDs are pending from this transfer */
        pipe_tr_struct_ptr->no_of_itds_sitds -= 1;

        /* if all ITDS are served free the TR INDEX */
        if (pipe_tr_struct_ptr->no_of_itds_sitds == 0)
        {
            /* Mark TR as unused so that next request can use it */
            pipe_tr_struct_ptr->TR_INDEX = 0;

        }

        /* if we just freed a head node, we must update our list head */
        if (node_ptr->prev == NULL)
        {
            prev_node_ptr = node_ptr = next_node_ptr;
        }
        else /* we freed some non head node so we move on normally*/
        {
            /* move to next ITD in the list */
            node_ptr = next_node_ptr;
            prev_node_ptr = node_ptr->prev;

        }

#endif

    }/* end while loop */


} /* Endbody */

/* EOF */

