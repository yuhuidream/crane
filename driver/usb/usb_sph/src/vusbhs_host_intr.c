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
***   chip for interrupt transfer
***
**************************************************************************
**END*********************************************************/
#include "hostapi.h"
#include "usbprv_host.h"

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif

//extern void * USB_memalloc(uint_32 n);

extern uint_32 _usb_ehci_calculate_uframe_tr_time(uint_32, uint_8);
extern uint_32 _usb_ehci_init_Q_element(_usb_host_handle,
                                        PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern void _usb_ehci_init_Q_HEAD(_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR,
                                  EHCI_QH_STRUCT_PTR, EHCI_QH_STRUCT_PTR, EHCI_QTD_STRUCT_PTR);

extern void unlink_periodic_data_structure_from_frame(
    USB_Uncached uint_32 _PTR_,
    USB_Uncached uint_32 _PTR_);

extern void reclaim_band_width(_usb_host_handle,
                               uint_32,
                               USB_Uncached uint_32 _PTR_,
                               PIPE_DESCRIPTOR_STRUCT_PTR);



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_add_interrupt_xfer_to_periodic_list
*  Returned Value : None
*  Comments       :
*        Queue the transfer in the EHCI hardware Periodic schedule list
*END*-----------------------------------------------------------------*/
uint_32 _usb_ehci_add_interrupt_xfer_to_periodic_list
(
    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle,

    /* The pipe descriptor to queue */
    PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr,

    /* [IN] the transfer parameters struct */
    PIPE_TR_STRUCT_PTR               pipe_tr_ptr
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    VUSB20_REG_STRUCT_PTR                        dev_ptr;
    ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr, temp_list_ptr;

    EHCI_QH_STRUCT_PTR                           QH_ptr = NULL;
    // clean warning EHCI_QH_STRUCT_PTR                           prev_QH_ptr = NULL;
    EHCI_QTD_STRUCT_PTR                          first_QTD_ptr, temp_QTD_ptr;
    // clean warning PIPE_DESCRIPTOR_STRUCT_PTR                   pipe_for_queue = NULL;
    uint_32                                      cmd_val, sts_val;
    // clean warning uint_32                                      H_bit = 1;
    // clean warning uint_32                                      interrupt_sched_mask = 1;
    // clean warning boolean                                      init_periodic_list = FALSE;
    boolean                                      found_existing_q_head = FALSE;

    /* QH initialization fields */
    // clean warning uint_32                       control_ep_flag = 0;
    // clean warning uint_32                       split_completion_mask = 1;
    // clean warning uint_32                       data_toggle_control = 0, item_type = 0;
    // clean warning uint_8                        mult = 0, period = 0;
    // clean warning uint_32_ptr                   temp_frame_list_ptr = NULL;


    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    /****************************************************************************
    QTD MAKING
    ****************************************************************************/

    /* Initialize the QTDs for the Queue Head */
    first_QTD_ptr = (EHCI_QTD_STRUCT_PTR)_usb_ehci_init_Q_element(
                        handle, pipe_descr_ptr, pipe_tr_ptr
                    );

#ifdef DEBUG_INFO
    /* BBU_putstr("QTD queued Top QTD Token=%x\n\
              Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n",
              first_QTD_ptr->TOKEN,
              ((first_QTD_ptr->TOKEN)&0xFF),
              ((first_QTD_ptr->TOKEN) >> 8)&0x3,
              ((first_QTD_ptr->TOKEN) >> 10) &0x3,
              ((first_QTD_ptr->TOKEN) >> 12)&0x7,
              ((first_QTD_ptr->TOKEN) >> 15)&0x1,
              ((first_QTD_ptr->TOKEN) >> 16)&0x7FFF,
              ((first_QTD_ptr->TOKEN)&EHCI_QTD_DATA_TOGGLE) >>31);
              */
#endif


    /****************************************************************************
    Obtain the QH for this pipe
    ****************************************************************************/
    QH_ptr = (EHCI_QH_STRUCT_PTR) pipe_descr_ptr->QH_FOR_THIS_PIPE;


    /****************************************************************************
    Ensure that this QH is in the list of active QHs for interrupt pipe
    ****************************************************************************/

    /******************************************************************
    If active list does not exist, we make a new list and this is the
    first member of the list. Otherwise we append the list at the end.
    *******************************************************************/
    if (usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR == NULL)
    {

        active_list_member_ptr =  (ACTIVE_QH_MGMT_STRUCT_PTR) \
                                  USB_memalloc(sizeof(ACTIVE_QH_MGMT_STRUCT));
        USB_dev_memzero (active_list_member_ptr, sizeof(ACTIVE_QH_MGMT_STRUCT));

        if (!active_list_member_ptr)
        {
            return USBERR_ALLOC;
        }

        usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR = active_list_member_ptr;

        /****************************************************************
        Connect the QH with the active list
        ****************************************************************/
        active_list_member_ptr->QH_PTR = (EHCI_QH_STRUCT_PTR)QH_ptr;
        active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)first_QTD_ptr;

        /****************************************************************
        Connect the QH with the QTD
        ****************************************************************/
        QH_ptr->ALT_NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;
        QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)first_QTD_ptr;

    }
    else
    {

        /****************************************************************
        search the list to find if this QH aleady exists in the list. If
        not, allocate a new list member and add to the list or else move
        on with no action.
        ****************************************************************/

        temp_list_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                        usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;

        do
        {
            if (temp_list_ptr->QH_PTR == QH_ptr)
            {
                found_existing_q_head = TRUE;
                break;
            }
            temp_list_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                            temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

        }
        while (temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR) ;




        /****************************************************************
        If no QH not found a new list memeber or connect QTDs to the existing one
        ****************************************************************/


        if (!found_existing_q_head)
        {

            active_list_member_ptr =  (ACTIVE_QH_MGMT_STRUCT_PTR) \
                                      USB_memalloc(sizeof(ACTIVE_QH_MGMT_STRUCT));
            USB_dev_memzero (active_list_member_ptr, sizeof(ACTIVE_QH_MGMT_STRUCT));

            if (!active_list_member_ptr)
            {
                return USBERR_ALLOC;
            }

            temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = active_list_member_ptr;

            /****************************************************************
            Connect the QH with the active list
            ****************************************************************/
            active_list_member_ptr->QH_PTR = (EHCI_QH_STRUCT_PTR)QH_ptr;
            active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)first_QTD_ptr;

            /****************************************************************
            Connect the QH with the QTD
            ****************************************************************/
            QH_ptr->ALT_NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;
            QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)first_QTD_ptr;

        }
        else
        {
            /****************************************************************
            update the active interrupt list now.
            ****************************************************************/
            temp_QTD_ptr = (EHCI_QTD_STRUCT_PTR)temp_list_ptr->FIRST_QTD_PTR;

            if ((((uint_32)temp_QTD_ptr) & EHCI_QTD_T_BIT) | (temp_QTD_ptr == NULL))
            {
                temp_list_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)first_QTD_ptr;
            }
            else
            {
                while (!(temp_QTD_ptr->NEXT_QTD_PTR & EHCI_QTD_T_BIT))
                {
                    temp_QTD_ptr = (EHCI_QTD_STRUCT_PTR)temp_QTD_ptr->NEXT_QTD_PTR;
                }

                temp_QTD_ptr->NEXT_QTD_PTR = (uint_32)first_QTD_ptr;

            } /*else*/

            /****************************************************************
            This case is arrived when the QH is active and there is a
            possibility that it may also have active QTDs.
            ****************************************************************/
            if (QH_ptr->NEXT_QTD_LINK_PTR & EHCI_QTD_T_BIT)
            {
                QH_ptr->ALT_NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;
                QH_ptr->NEXT_QTD_LINK_PTR = (uint_32)first_QTD_ptr;
            }



        }/*else*/

    } /*else */

#ifdef DEBUG_INFO
    /*  printf("_usb_ehci_add_interrupt_xfer_to_periodic_list: QH =%x\n\
               Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n",
               first_QTD_ptr->TOKEN,
               ((first_QTD_ptr->TOKEN)&0xFF),
               ((first_QTD_ptr->TOKEN) >> 8)&0x3,
               ((first_QTD_ptr->TOKEN) >> 10) &0x3,
               ((first_QTD_ptr->TOKEN) >> 12)&0x7,
               ((first_QTD_ptr->TOKEN) >> 15)&0x1,
               ((first_QTD_ptr->TOKEN) >> 16)&0x7FFF,
               ((first_QTD_ptr->TOKEN)&EHCI_QTD_DATA_TOGGLE) >>31);
               */
#endif


    /****************************************************************************
    if periodic schedule is not already enabled, enable it.
    ****************************************************************************/
    sts_val = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS;

    if (!(sts_val & EHCI_STS_PERIODIC_SCHEDULE))
    {

        cmd_val = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD;

        /****************************************************************************
        write the address of the periodic list in to the periodic base register
        ****************************************************************************/
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PERIODIC_LIST_BASE_ADDR =
            (uint_32) usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR;

        /****************************************************************************
        wait until we can enable  the periodic schedule.
        ****************************************************************************/
        while ((cmd_val & EHCI_USBCMD_PERIODIC_SCHED_ENABLE) !=
                (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS & EHCI_STS_PERIODIC_SCHEDULE));


        /****************************************************************************
        enable the schedule now.
        ****************************************************************************/

        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD =
            (cmd_val | EHCI_USBCMD_PERIODIC_SCHED_ENABLE);
    }
    return USB_OK;

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_close_interrupt_pipe
*  Returned Value : None
*  Comments       :
*        Close the Interrupt pipe and update the bandwidth list.
Here are the notes. In EHCI, closing an interrupt pipe involves removing
the queue head from the periodic list to make sure that none of the
frames refer to this queue head any more. It is also important to remember
that we must start removing the queue head link from a safe place which
is not currently being executed by EHCi controller. Apart from this we
should free all QTDs associated with it and QH Managements structure.
*END*-----------------------------------------------------------------*/

void _usb_ehci_close_interrupt_pipe (

    /* [IN] the USB Host state structure */
    _usb_host_handle                 handle,

    /* The pipe descriptor to queue */
    PIPE_DESCRIPTOR_STRUCT_PTR       pipe_descr_ptr

)
{
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    EHCI_QH_STRUCT_PTR                           QH_ptr, temp_QH_ptr;
    ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr;
    ACTIVE_QH_MGMT_STRUCT_PTR                    temp_active_list_member_ptr, prev_active_list_member_ptr;
    EHCI_QTD_STRUCT_PTR                          QTD_ptr, temp_QTD_ptr;
    uint_8_ptr                                   frame_list_bw_ptr;
    USB_Uncached uint_32 _PTR_                   periodic_list_base_ptr;
#if 1
    USB_Uncached uint_32 _PTR_                   transfer_data_struct_ptr;
    USB_Uncached uint_32 _PTR_                   prev_transfer_data_struct_ptr;
#else
    USB_Uncached uint_32_ptr                     transfer_data_struct_ptr;
    USB_Uncached uint_32_ptr                     prev_transfer_data_struct_ptr;
#endif
    EHCI_ITD_STRUCT_PTR                          ITD_ptr;
    EHCI_SITD_STRUCT_PTR                         SITD_ptr;
    EHCI_FSTN_STRUCT_PTR                         FSTN_ptr;
    uint32_t                                          i;
    boolean                                      unlinked;

    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    /****************************************************************************
    Obtain the QH for this pipe
    ****************************************************************************/
    QH_ptr = (EHCI_QH_STRUCT_PTR) pipe_descr_ptr->QH_FOR_THIS_PIPE;

    if (QH_ptr == NULL) return;


    /****************************************************************************
    First Search the periodic list and unlink this QH from the list.
    ****************************************************************************/

    frame_list_bw_ptr       = usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR;
    periodic_list_base_ptr  = (USB_Uncached uint_32 _PTR_)(usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR);

    /*******************************************************************
    Start from fram 0 till end of the list and unlink the QH if found. Note
    that we should not unlink when the QH is active but current code does
    not take this in account.
    ********************************************************************/

    for (i = 0; i < usb_host_ptr->FRAME_LIST_SIZE; i++)
    {

        transfer_data_struct_ptr = (USB_Uncached uint_32 _PTR_) periodic_list_base_ptr + i;
        prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
        unlinked = FALSE;

        /*******************************************************************
        Traverse the frame i and unlink the QH if found in it.
        ********************************************************************/
        while ((!(*((USB_Uncached uint_32 _PTR_)transfer_data_struct_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
                && (unlinked == FALSE))
        {

            switch (EHCI_GET_TYPE(transfer_data_struct_ptr))
            {
            case EHCI_ELEMENT_TYPE_ITD:
                ITD_ptr = (EHCI_ITD_STRUCT_PTR)((*transfer_data_struct_ptr)&
                                                EHCI_HORIZ_PHY_ADDRESS_MASK);
                prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                transfer_data_struct_ptr = (pointer)ITD_ptr->NEXT_LINK_PTR;
                break;
            case EHCI_ELEMENT_TYPE_QH:

                temp_QH_ptr = (EHCI_QH_STRUCT_PTR)((*transfer_data_struct_ptr) &
                                                   EHCI_HORIZ_PHY_ADDRESS_MASK);

                if (temp_QH_ptr ==  QH_ptr)
                {
                    /*************************************************************
                    Unlink this QH now.
                    *************************************************************/
                    USB_lock();
                    reclaim_band_width(
                        handle,
                        i,
                        (USB_Uncached uint_32 _PTR_) transfer_data_struct_ptr,
                        pipe_descr_ptr);
                    unlink_periodic_data_structure_from_frame (
                        (USB_Uncached uint_32 _PTR_) prev_transfer_data_struct_ptr,
                        (USB_Uncached uint_32 _PTR_) transfer_data_struct_ptr);
                    USB_unlock();
                    /*exit out of loop since there can be only 1 QH of same pipe in this frame */
                    unlinked = TRUE;
                }
                prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                transfer_data_struct_ptr = (pointer)temp_QH_ptr->HORIZ_LINK_PTR;
                break;
            case EHCI_ELEMENT_TYPE_SITD:
                SITD_ptr = (EHCI_SITD_STRUCT_PTR)((*transfer_data_struct_ptr) &
                                                  EHCI_HORIZ_PHY_ADDRESS_MASK);
                prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                transfer_data_struct_ptr = (pointer)SITD_ptr->NEXT_LINK_PTR;
                break;
            case EHCI_ELEMENT_TYPE_FSTN:
                FSTN_ptr = (EHCI_FSTN_STRUCT_PTR)((*transfer_data_struct_ptr) &
                                                  EHCI_HORIZ_PHY_ADDRESS_MASK);
                prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                transfer_data_struct_ptr = (pointer)FSTN_ptr->NORMAL_PATH_LINK_PTR;
                break;
            default:
                break;
            } /* EndSwitch */

        } /*while */

    }  /* for */



    /****************************************************************************
    Search the list of active interrupt transfers to find if this QH aleady exists
    in the list. If it exists we should free QH and all QTD associated with it.
    **************************************************************************/

    active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                             usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;

    /*point to itself to start with */
    prev_active_list_member_ptr = active_list_member_ptr;

    /*count the number of elements in the list, if None, set the list to NULL*/
    i = 0;

    while (active_list_member_ptr)
    {
        if (active_list_member_ptr->QH_PTR == QH_ptr)
        {

            /* don't want to get interrupted when changing the QTD and QH list */
            USB_lock();
            /***********************************************
            Do the freeing stuff
            disable this QH so that there are no more QTDs executed.
            ***********************************************************/
            QH_ptr->NEXT_QTD_LINK_PTR = EHCI_QTD_T_BIT;

            /***********************************************
            free all the QTDs if any.
            ************************************************/
            QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;
            while ((!(((uint_32)QTD_ptr) & EHCI_QTD_T_BIT)) &&  (QTD_ptr != NULL))
            {
                temp_QTD_ptr = QTD_ptr;
                QTD_ptr = (EHCI_QTD_STRUCT_PTR)QTD_ptr->NEXT_QTD_PTR;
                _usb_hci_vusb20_free_QTD(handle, (pointer)temp_QTD_ptr);
            }

            /*free QH */
            _usb_hci_vusb20_free_QH(handle, (pointer) QH_ptr);

            /*ensure that all pointers are NULL in this structure */
            active_list_member_ptr->QH_PTR = NULL;
            active_list_member_ptr->FIRST_QTD_PTR = NULL;

            /* store the pointer temporarily before freeing it */
            temp_active_list_member_ptr = active_list_member_ptr;

            /* move on to the next member of the list */
            active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                                     active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;


            /* we should free this ACTIVE Q MGMT structure */
            if (prev_active_list_member_ptr != prev_active_list_member_ptr)
            {
                /*connect previous to next */
                ((ACTIVE_QH_MGMT_STRUCT_PTR)prev_active_list_member_ptr)->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR =
                    temp_active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

                USB_memfree(temp_active_list_member_ptr);
            }
            else   /* means first element of the list */
            {
                USB_memfree(temp_active_list_member_ptr);
            }

            USB_unlock();


            break;
        }
        else
        {
            prev_active_list_member_ptr = active_list_member_ptr;
            active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                                     active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
            i++;
        }

    }

    if (i == 0) /* if list is empty make sure to initialize the pointer */
    {
        usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR = NULL;
    }



    return;

}




/* EOF */
