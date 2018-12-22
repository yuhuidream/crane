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

*** Description: Implementation of mass storage class specification.
***
**************************************************************************
**END*********************************************************/

#include "usb_mass_bo.h"

/* Linked list of instances of mass storage interfaces */
static USB_MASS_CLASS_INTF_STRUCT_PTR mass_anchor = NULL;

/* Private functions */
static void usb_class_mass_call_back_cbw(pointer, pointer, uchar_ptr, uint_32,
        uint_32);
static void usb_class_mass_call_back_dphase(pointer, pointer, uchar_ptr,
        uint_32, uint_32);
static void usb_class_mass_call_back_csw(pointer, pointer, uchar_ptr, uint_32,
        uint_32);
static void usb_class_mass_reset_callback(_usb_pipe_handle, pointer, uchar_ptr,
        uint_32, uint_32);
static USB_STATUS usb_class_mass_pass_on_usb(USB_MASS_CLASS_INTF_STRUCT_PTR);
static USB_STATUS usb_class_mass_clear_bulk_pipe_on_usb(
    USB_MASS_CLASS_INTF_STRUCT_PTR);


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_init
* Returned Value : None
* Comments       :
*     This function initializes the mass storage class
*
*END*--------------------------------------------------------------------*/

void usb_class_mass_init
(
    /* [IN] structure with USB pipe information on the interface */
    PIPE_BUNDLE_STRUCT_PTR      pbs_ptr,

    /* [IN] printer call struct pointer */
    CLASS_CALL_STRUCT_PTR       ccs_ptr
)
{
    /* Body */
    USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr = ccs_ptr->class_intf_handle;
    //DEV_INSTANCE_PTR                 dev_ptr;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_init\r\n");
#endif

    mass_anchor = NULL;

    /* Pointer validity-checking, clear memory, init header */
    if (usb_host_class_intf_init(pbs_ptr, intf_ptr, &mass_anchor))
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_class_mass_init\r\n");
#endif
        return;
    }

    USB_lock();
    ccs_ptr->code_key = 0;
    ccs_ptr->code_key = usb_host_class_intf_validate( ccs_ptr );
    if (ccs_ptr->code_key == 0) goto Bad_Exit;

    /* Start filling up the members of interface handle */
    //dev_ptr = (DEV_INSTANCE_PTR)intf_ptr->DEV_INST ;

    intf_ptr->DEV_INST         = pbs_ptr->dev_handle;
    intf_ptr->INTF_DESC        = pbs_ptr->intf_handle;
    intf_ptr->HOST_HANDLE      = ((DEV_INSTANCE_PTR)pbs_ptr->dev_handle)->host;
    intf_ptr->INTERFACE_NUM =
        ((INTERFACE_DESCRIPTOR_PTR)intf_ptr->INTF_DESC)->bInterfaceNumber;

    intf_ptr->CONTROL_PIPE = usb_hostdev_get_pipe_handle(pbs_ptr,
                             USB_CONTROL_PIPE, 0);
    intf_ptr->BULK_IN_PIPE = usb_hostdev_get_pipe_handle(pbs_ptr,
                             USB_BULK_PIPE, USB_RECV);
    intf_ptr->BULK_OUT_PIPE = usb_hostdev_get_pipe_handle(pbs_ptr,
                              USB_BULK_PIPE, USB_SEND);

    if (intf_ptr->CONTROL_PIPE    &&
            intf_ptr->BULK_IN_PIPE     &&
            intf_ptr->BULK_OUT_PIPE)
    {
        /* Initialize the queue for storing the local copy of interface handles */
        usb_class_mass_q_init(intf_ptr);
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("usb_class_mass_init, Q init failed\r\n");
#endif

        return;
    } /* Endif */

Bad_Exit:
#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_init, bad exit\r\n");
#endif
    USB_dev_memzero(intf_ptr, sizeof(USB_MASS_CLASS_INTF_STRUCT_PTR));
    ccs_ptr->class_intf_handle = NULL;
    ccs_ptr->code_key = 0;
    USB_unlock();
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_mass_storage_device_command
* Returned Value : ERROR STATUS error code
*                  USB_OK - means that command has been succefully queued in class
*                  driver queue (or has been passed to USB, if there is not other
*                  command pending)
* Comments       :
*     This routine is called by the protocol layer to execute the command
*     defined in protocol API. It can also be directly called by a user
*     application if they wish to make their own commands (vendor specific) for
*     sending to a mass storage device.
END*--------------------------------------------------------------------*/

USB_STATUS usb_class_mass_storage_device_command
(
    /* Handle to class struct with the key */
    CLASS_CALL_STRUCT_PTR      ccs_ptr,

    /*Command */
    COMMAND_OBJECT_PTR         cmd_ptr
)
{
    /* Body */
    USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr;
    int                              temp;
    USB_STATUS                       status = USBERR_NO_INTERFACE;
    CBW_STRUCT_PTR                   pCbw = (CBW_STRUCT_PTR) cmd_ptr->CBW_PTR;
    boolean                          empty;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_storage_device_command\r\n");
#endif

    /* Pointer validity-checking, clear memory, init header */
    USB_lock();
    if (usb_host_class_intf_validate(ccs_ptr))
    {
        intf_ptr = (USB_MASS_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle;
        status = usb_hostdev_validate (intf_ptr->DEV_INST);
    } /* Endif */

    if (status == USB_OK)
    {
        /* Fill in the remaining CBW fields as per the USB Mass Storage specs */
        htou32(pCbw->DCBWSIGNATURE, CBW_SIGNATURE);

        /* CBW is ready so queue it and update status */
        empty = USB_CLASS_MASS_IS_Q_EMPTY(intf_ptr);
        temp = usb_class_mass_q_insert(intf_ptr, cmd_ptr);
        if (temp >= 0)
        {
            cmd_ptr->STATUS = STATUS_QUEUED_IN_DRIVER;

            /* The tag for the command packet is its queue number. */
            htou32(pCbw->DCBWTAG, temp);

            /*
            ** If queue was empty send CBW to USB immidiately, otherwise it will be
            ** taken from queue later by a callback function
            */
            if (empty)
            {
                status = usb_class_mass_pass_on_usb(intf_ptr);
            } /* Endif */
        }
        else
        {
            status = USB_MASS_QUEUE_FULL;
        } /* Endif */
    } /* Endif */
    USB_unlock();

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_storage_device_command, SUCCESSFUL\r\n");
#endif

    return status;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_call_back_cbw
* Returned Value : None
* Comments       :
*     This routine is called when the CBW phase of the currently pending command
*     has finished.
*
*END*--------------------------------------------------------------------*/

void usb_class_mass_call_back_cbw
(
    /* [IN] Pipe on which CBW call was made */
    _usb_pipe_handle pipe_handle,

    /* [IN] Pointer to the class instance */
    pointer          user_parm,

    /* [IN] Pointer to data buffer */
    uchar_ptr        buffer,

    /* [IN] Length of data transfered */
    uint_32          length_data_transfered,

    /* [IN] Status of the transfer */
    uint_32          status
)
{
    /* Body */
    USB_MASS_CLASS_INTF_STRUCT_PTR   plocal_intf;
    COMMAND_OBJECT_PTR               cmd_ptr = NULL;
    pipe_handle = pipe_handle;
    buffer = buffer;
#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_call_back_cbw\r\n");
#endif

    if (user_parm)
    {
        /* Get the pointer to the pending request */
        plocal_intf = (USB_MASS_CLASS_INTF_STRUCT_PTR)user_parm;
        usb_class_mass_get_pending_request(plocal_intf, &cmd_ptr);
    } /* Endif */

    /* If there is no user_parm or cmd_ptr we return */
    if (!cmd_ptr)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_class_mass_call_back_cbw, bad parameter\r\n");
#endif
        return;
    } /* Endif */

    /* Check status of CBW phase and go to the next phase (Data or Status) */
    if (status == USB_OK)
    {
        /* Verify if the length of the data transferred was correct */
        if (length_data_transfered != CBW_SIZE)
        {
            if (cmd_ptr->RETRY_COUNT < MAX_RETRIAL_ATTEMPTS)
            {
                cmd_ptr->RETRY_COUNT++;
                cmd_ptr->STATUS = STATUS_QUEUED_IN_DRIVER;
                cmd_ptr->PREV_STATUS =  cmd_ptr->STATUS; /* preserve the status */
                status = usb_class_mass_reset_recovery_on_usb(plocal_intf);
            }
            else
            {
                status = STATUS_CANCELLED; /* cannot keep repeating */
            } /* Endif */
        }
        else
        {
            /* Everything is normal, go to next phase of pending request */
            cmd_ptr->STATUS = STATUS_FINISHED_CBW_ON_USB;
            status = usb_class_mass_pass_on_usb(plocal_intf);
        } /* Endif */
    } /* Endif */

    if ((status != USB_OK) && (status != USBERR_TRANSFER_IN_PROGRESS))
    {
        /*
        ** Host must do a reset recovery for the device as per the spec. We also
        ** notify the application of the failure, remove the command from the
        ** queue and move on the the next command queued
        */
        cmd_ptr->STATUS = STATUS_CANCELLED;
        cmd_ptr->CALLBACK(USB_MASS_FAILED_IN_COMMAND, plocal_intf, cmd_ptr,
                          cmd_ptr->TR_BUF_LEN);
        usb_class_mass_deleteq(plocal_intf);
        status = usb_class_mass_pass_on_usb(plocal_intf);
    } /* Endif */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_call_back_cbw, SUCCESSFUL\r\n");
#endif

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_call_back_dphase
* Returned Value : None
* Comments       :
*     This routine is called by the bus driver when the data phase completes.
*     If status is USB_OK, this routine will pass on the next routine called
*     usb_class_mass_pass_on_usb() which will queue the status phase. If failure
*     the routine will either try to dequeue the packet, or send a Reset device
*     command or will try to requeue the packet for next phase.
*
*END*--------------------------------------------------------------------*/

void usb_class_mass_call_back_dphase
(
    /* [IN] Pipe on which CBW call was made */
    _usb_pipe_handle pipe_handle,

    /* [IN] Pointer to the class instance */
    pointer          user_parm,

    /* [IN] Pointer to data buffer */
    uchar_ptr        buffer,

    /* [IN] Length of data transfered */
    uint_32          length_data_transfered,

    /* [IN] Status of the transfer */
    uint_32          status
)
{
    /* Body */
    USB_MASS_CLASS_INTF_STRUCT_PTR   plocal_intf;
    COMMAND_OBJECT_PTR               cmd_ptr = NULL;
    pipe_handle = pipe_handle;
    buffer = buffer;
#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_call_back_dphase\r\n");
#endif

    if (user_parm)
    {
        /* Get the pointer to the pending request */
        plocal_intf = (USB_MASS_CLASS_INTF_STRUCT_PTR) user_parm;
        usb_class_mass_get_pending_request(plocal_intf, &cmd_ptr);
    } /* Endif */

    if (!cmd_ptr)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_class_mass_call_back_dphase, bad parameter\r\n");
#endif
        return;
    } /* Endif */

    /* Save length transferred during data phase */
    cmd_ptr->TR_BUF_LEN = length_data_transfered;

    if (status == USB_OK)
    {
        /* Everything is OK, update status and go to next phase */
        cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB;
        status = usb_class_mass_pass_on_usb(plocal_intf);

    }
    else if (status == USBERR_ENDPOINT_STALLED)
    {

        if (cmd_ptr->RETRY_COUNT < MAX_RETRIAL_ATTEMPTS)
        {
            cmd_ptr->RETRY_COUNT++;
            cmd_ptr->STATUS = STATUS_QUEUED_IN_DRIVER;
            cmd_ptr->PREV_STATUS =  cmd_ptr->STATUS; /* preserve the status */

            status = usb_class_mass_clear_bulk_pipe_on_usb(plocal_intf);
        }
        else
        {
            status = STATUS_CANCELLED; /* cannot keep repeating */
        } /* Endif */
    } /* Endif */

    if ((status != USB_OK) && (status != USBERR_ENDPOINT_STALLED) &&
            (status != USBERR_TRANSFER_IN_PROGRESS))
    {
        /* Command didn't succeed. Notify application and cleanup */
        cmd_ptr->STATUS = STATUS_CANCELLED;
        cmd_ptr->CALLBACK(USB_MASS_FAILED_IN_DATA, plocal_intf, cmd_ptr,
                          cmd_ptr->TR_BUF_LEN);
        usb_class_mass_deleteq(plocal_intf);

        /* Go to the next command, if any */
        status = usb_class_mass_pass_on_usb(plocal_intf);
    } /* Endif */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_call_back_dphase, SUCCESSFUL\r\n");
#endif

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_call_back_csw
* Returned Value : None
* Comments       : This routine is called by the bus driver when the
*      status phase completes on mass storage device. Please see the specs of
*      USB storage device to check what actions should be taken depending upon
*      the status returned by the device.
*
*END*--------------------------------------------------------------------*/

void usb_class_mass_call_back_csw
(
    /* [IN] Pipe on which CBW call was made */
    _usb_pipe_handle pipe_handle,

    /* [IN] Pointer to the class instance */
    pointer          user_parm,

    /* [IN] Pointer to data buffer */
    uchar_ptr        buffer,

    /* [IN] Length of data transfered */
    uint_32          length_data_transfered,

    /* [IN] Status of the transfer */
    uint_32          status
)
{
    /* Body */
    USB_MASS_CLASS_INTF_STRUCT_PTR   plocal_intf = NULL;
    COMMAND_OBJECT_PTR               cmd_ptr = NULL;
    CSW_STRUCT_PTR                   pCsw = NULL;
    uint_32                          tmp1, tmp2, tmp3;
    boolean                          proc_next = FALSE;
    pipe_handle = pipe_handle;
    buffer = buffer;
#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_call_back_csw\r\n");
#endif

    if (user_parm)
    {
        /* Get the pointer to the pending request */
        plocal_intf =  (USB_MASS_CLASS_INTF_STRUCT_PTR) user_parm;
        usb_class_mass_get_pending_request(plocal_intf, &cmd_ptr);
    } /* Endif */

    if (!cmd_ptr)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_class_mass_call_back_csw, bad param\r\n");
#endif
        return;
    } /* Endif */

    /* Finish transaction and queue next command */
    if (status == USB_OK)
    {
        pCsw = (CSW_STRUCT_PTR) cmd_ptr->CSW_PTR;
        tmp1 = utoh32(pCsw->DCSWTAG);
        tmp2 = utoh32(cmd_ptr->CBW_PTR->DCBWTAG);
        tmp3 = utoh32(pCsw->DCSWSIGNATURE);

        /* Size must be verified, as well as the signature and the tags */
        if ((length_data_transfered != sizeof(CSW_STRUCT)) ||
                (tmp3 != CSW_SIGNATURE) || (tmp1 != tmp2))
        {
            if (cmd_ptr->RETRY_COUNT < MAX_RETRIAL_ATTEMPTS)
            {
                cmd_ptr->RETRY_COUNT++;
                cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB;
                cmd_ptr->PREV_STATUS =  cmd_ptr->STATUS; /* preserve the status */

                status = usb_class_mass_reset_recovery_on_usb(plocal_intf);
            }
            else
            {
                status = STATUS_CANCELLED; /* cannot keep repeating */
            } /* Endif */

            if ((status != USB_OK)  && (status != USBERR_TRANSFER_IN_PROGRESS))
            {
                proc_next = TRUE;
            } /* Endif */
        }
        else
        {
            /* Check value of status field in CSW */
            switch (pCsw->BCSWSTATUS)
            {
            case CSW_STATUS_GOOD:
                /* Command succeed. Notify application and cleanup */
                cmd_ptr->STATUS = STATUS_COMPLETED;
                proc_next = TRUE;
                break;

            case CSW_STATUS_FAILED:
                /* Command failed. Notify application and cleanup */
                cmd_ptr->STATUS = STATUS_FAILED_IN_CSW;
                proc_next = TRUE;
                break;

            default:
                break;
            } /* Endswitch */
        } /* Endif */
    }
    else if (status == USBERR_ENDPOINT_STALLED)
    {
        if (cmd_ptr->RETRY_COUNT < MAX_RETRIAL_ATTEMPTS)
        {
            cmd_ptr->RETRY_COUNT++;
            cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB;
            cmd_ptr->PREV_STATUS =  cmd_ptr->STATUS; /* preserve the status */

            status = usb_class_mass_clear_bulk_pipe_on_usb(plocal_intf);
        }
        else
        {
            status = STATUS_CANCELLED; /* cannot keep repeating */
        } /* Endif */

        if ((status != USB_OK) && (status != USBERR_TRANSFER_IN_PROGRESS))
        {
            /* Command didn't succeed. Notify application and cleanup */
            cmd_ptr->STATUS = STATUS_CANCELLED;
            proc_next = TRUE;
        } /* Endbody */
    }
    else
    {
        /* Command didn't succeed. Notify application and cleanup */
        cmd_ptr->STATUS = STATUS_CANCELLED;
        proc_next = TRUE;
    } /* Endif */

    /* Should we cleanup and process the next command? */
    if (proc_next)
    {
        cmd_ptr->CALLBACK(status, plocal_intf, cmd_ptr, cmd_ptr->TR_BUF_LEN);
        usb_class_mass_deleteq(plocal_intf);

        /* Send next command, if any */
        if (!USB_CLASS_MASS_IS_Q_EMPTY(plocal_intf))
        {
            status = usb_class_mass_pass_on_usb( plocal_intf);
        } /* Endbody */
    } /* Endif */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_call_back_csw, SUCCESSFUL\r\n");
#endif

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_pass_on_usb
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This routine gets the pending request from class driver queue and finds
*     what needs to be done by looking at the status (cmd_ptr->STATUS). If a
*     phase has been completed, it will queue the next phase. If there is no
*     pending request in the queue, it will just return.
*     NOTE: This functions should only be called by a callback or withing a
*     USB_lock() USB_unlock() block!
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_mass_pass_on_usb
(
    /* [IN] Interface handle */
    USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr
)
{
    /* Body */
    COMMAND_OBJECT_PTR      cmd_ptr = NULL;
    TR_INIT_PARAM_STRUCT    tr_init;
    USB_STATUS              status = USB_OK;
    uint_8                  tmp;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_pass_on_usb\r\n");
#endif

    /* Nothing can be done if there is nothing pending */
    usb_class_mass_get_pending_request(intf_ptr, &cmd_ptr);
    if (cmd_ptr == NULL)
    {
        USB_unlock();
#ifdef _HOST_DEBUG_
        bbu_printf("usb_class_mass_pass_on_usb, no matching request\r\n");
#endif
        return USB_MASS_NO_MATCHING_REQUEST;
    } /* Endif */

    /* Determine the appropriate action based on the phase */
    switch (cmd_ptr->STATUS)
    {
    case STATUS_QUEUED_IN_DRIVER:
        /* means that CBW needs to be sent.*/
        usb_hostdev_tr_init(&tr_init, usb_class_mass_call_back_cbw,
                            (pointer)intf_ptr);
        tr_init.TX_BUFFER = (uchar_ptr)cmd_ptr->CBW_PTR;
        tr_init.TX_LENGTH = sizeof(CBW_STRUCT);

        status = _usb_host_send_data(intf_ptr->HOST_HANDLE,
                                     intf_ptr->BULK_OUT_PIPE, &tr_init);
        break;

    case STATUS_FINISHED_CBW_ON_USB:
        /* Determine next phase (DATA or STATUS), length and direction */
        if (utoh32(cmd_ptr->CBW_PTR->DCBWDATATRANSFERLENGTH) > 0)
        {

            /* Commen TR setup for IN or OUT direction */
            usb_hostdev_tr_init(&tr_init, usb_class_mass_call_back_dphase,
                                (pointer)intf_ptr);

            tmp = (cmd_ptr->CBW_PTR->BMCBWFLAGS) & MASK_NON_DIRECTION_BITS;
            switch (tmp)
            {
            case DIRECTION_OUT:
                /* Make a TR with DATA Phase call back.*/
                tr_init.TX_BUFFER = (uchar_ptr)cmd_ptr->DATA_BUFFER;
                tr_init.TX_LENGTH = cmd_ptr->BUFFER_LEN;

                status = _usb_host_send_data(intf_ptr->HOST_HANDLE,
                                             intf_ptr->BULK_OUT_PIPE, &tr_init);
                break;

            case DIRECTION_IN:
                /* Make a TR with DATA call back.*/
                tr_init.RX_BUFFER = (uchar_ptr)cmd_ptr->DATA_BUFFER;
                tr_init.RX_LENGTH = cmd_ptr->BUFFER_LEN;

                status = _usb_host_recv_data(intf_ptr->HOST_HANDLE,
                                             intf_ptr->BULK_IN_PIPE, &tr_init);
                break;

            default:
                break;
            } /* Endswitch */
            break;
        } /* Endif */

        /*
        ** else case:
        ** No data trannsfer is expected of the request. Fall through to
        ** STATUS phase
        */

    case STATUS_FINISHED_DPHASE_ON_USB:
        /* Make a TR and send it with STATUS call back */
        usb_hostdev_tr_init(&tr_init, usb_class_mass_call_back_csw,
                            (pointer)intf_ptr);
        tr_init.RX_BUFFER = (uchar_ptr)cmd_ptr->CSW_PTR;
        tr_init.RX_LENGTH = sizeof(CSW_STRUCT);

        status = _usb_host_recv_data(intf_ptr->HOST_HANDLE,
                                     intf_ptr->BULK_IN_PIPE, &tr_init);
        break;

    case STATUS_FINISHED_CSW_ON_USB: /* No action */
    case STATUS_FAILED_IN_CSW:       /* Should never happen */
    default:
        break;
    } /* Endswitch */

    if (status == USB_STATUS_TRANSFER_QUEUED)
    {
        status = USB_OK;
    } /* Endif */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_pass_on_usb, SUCCESSFUL\r\n");
#endif

    return status;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_clear_bulk_pipe_on_usb
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This routine gets the pending request from class driver queue and sends
      the CLEAR_FEATURE command on control pipe. This routine is called when
      status phase of the pending command fails and class driver decides to
      clear the pipes. If there is no pending request in the queue, it will
      just return.
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_mass_clear_bulk_pipe_on_usb
(
    USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr
)
{
    /* Body */
    USB_STATUS                    status = USB_OK;
    PIPE_INIT_PARAM_STRUCT_PTR    pBulk_pipe = NULL;
    COMMAND_OBJECT_PTR            cmd_ptr = NULL;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_clear_bulk_pipe_on_usb\r\n");
#endif

    /* Nothing can be done if there is nothing pending */
    usb_class_mass_get_pending_request(intf_ptr, &cmd_ptr);
    if (cmd_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_class_mass_clear_bulk_pipe_on_usb, no matching request\r\n");
#endif
        return USB_MASS_NO_MATCHING_REQUEST;
    } /* Endif */

    /* Determine pipe (Bulk IN or Bulk OUT) to clear */
    if ((cmd_ptr->CBW_PTR->BMCBWFLAGS & MASK_NON_DIRECTION_BITS ) == DIRECTION_IN)
    {
        pBulk_pipe = (PIPE_INIT_PARAM_STRUCT_PTR)intf_ptr->BULK_IN_PIPE;
    }
    else
    {
        pBulk_pipe = (PIPE_INIT_PARAM_STRUCT_PTR)intf_ptr->BULK_OUT_PIPE;
    } /* Endif */

    status = _usb_host_register_ch9_callback(intf_ptr->DEV_INST,
             usb_class_mass_reset_callback, (pointer)intf_ptr);

    if (status == USB_OK)
    {
        /* Send a CLEAR PIPE command */
        cmd_ptr->STATUS = STATUS_CLEAR_BULK_PIPE;
        status = _usb_host_ch9_clear_feature(intf_ptr->DEV_INST,
                                             REQ_TYPE_ENDPOINT, pBulk_pipe->ENDPOINT_NUMBER, ENDPOINT_HALT);
    } /* Endif */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_clear_bulk_pipe_on_usb, SUCCESSFUL\r\n");
#endif

    return status;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_reset_recovery_on_usb
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This routine gets the pending request from class driver queue and sends
*     the RESET command on control pipe. This routine is called when
*     a phase of the pending command fails and class driver decides to
*     reset the device. If there is no pending request in the queue, it will
*     just return. This routine registers a call back for control pipe commands
*     to ensure that pending command is queued again.
*     NOTE: This functions should only be called by a callback or withing a
*     USB_lock() USB_unlock() block!
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_mass_reset_recovery_on_usb
(
    USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr
)
{
    /* Body */
    // clean warning PIPE_INIT_PARAM_STRUCT_PTR pPipe = NULL;
    COMMAND_OBJECT_PTR         cmd_ptr = NULL;
    // clean warning DEV_INSTANCE_PTR           dev_ptr = (DEV_INSTANCE_PTR)intf_ptr->DEV_INST;
    USB_STATUS                 status = USB_OK;
    USB_SETUP                  request;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_reset_recovery_on_usb\r\n");
#endif

    /* Nothing can be done if there is nothing pending*/
    usb_class_mass_get_pending_request(intf_ptr, &cmd_ptr);
    if (cmd_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_class_mass_reset_recovery_on_usb,no matching request\r\n");
#endif
        return USB_MASS_NO_MATCHING_REQUEST;
    } /* Endif */

    /* BULK device mass storage reset */
    request.BMREQUESTTYPE   = REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_OUT;
    request.BREQUEST        = MASS_STORAGE_RESET;
    htou16(request.WVALUE, 0);
    htou16(request.WINDEX, intf_ptr->INTERFACE_NUM);
    htou16(request.WLENGTH, 0);

    cmd_ptr->STATUS = STATUS_RESET_DEVICE;
    status = _usb_hostdev_cntrl_request (intf_ptr->DEV_INST, &request, NULL,
                                         usb_class_mass_reset_callback, (pointer)intf_ptr);

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_reset_recovery_on_usb,SUCCESSFUL\r\n");
#endif

    return status;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_getmaxlun_bulkonly
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This is a class specific command. See the documentation of the USB
*     mass storage specifictaion to learn how this command works. This command
*     is used the get the Number of Logical Units on the device. Caller will
*     use the LUN number to direct the commands (as a part of CBW)
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_mass_getmaxlun_bulkonly
(
    CLASS_CALL_STRUCT_PTR      ccs_ptr,
    uint_8_ptr                 pLUN,
    tr_callback                callback
)
{
    /* Body */
    USB_STATUS                       status = USBERR_NO_INTERFACE;
    USB_SETUP                        request;
    USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_getmaxlun_bulkonly\r\n");
#endif

    /* Pointer validity-checking, clear memory, init header */
    USB_lock();
    if (usb_host_class_intf_validate(ccs_ptr))
    {
        intf_ptr = (USB_MASS_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle;
        status = usb_hostdev_validate(intf_ptr->DEV_INST);
    } /* Endif */

    if (status)
    {
        USB_unlock();

#ifdef _HOST_DEBUG_
        bbu_printf("usb_class_mass_getmaxlun_bulkonly, error status\r\n");
#endif
        return status;
    } /* Endif */

    /* Get the number of logical units on the device */
    request.BMREQUESTTYPE = REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_IN;
    request.BREQUEST = GET_MAX_LUN;
    htou16(request.WVALUE, 0);
    htou16(request.WINDEX, intf_ptr->INTERFACE_NUM);
    htou16(request.WLENGTH, 1);

    /* Send request */
    status = _usb_hostdev_cntrl_request (intf_ptr->DEV_INST,
                                         &request, (uchar_ptr)pLUN, callback, NULL);
    USB_unlock();

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_getmaxlun_bulkonly, SUCCESSFUL\r\n");
#endif
    return status;

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_reset_callback
* Returned Value : None
* Comments       :
*     Callback used when resetting and clearing endpoints
*
*END*--------------------------------------------------------------------*/

static void usb_class_mass_reset_callback
(
    /* [IN] pointer to pipe */
    _usb_pipe_handle  pipe_handle,

    /* [IN] user-defined parameter */
    pointer           user_parm,

    /* [IN] buffer address */
    uchar_ptr         buffer,

    /* [IN] length of data transferred */
    uint_32           length_data_transfered,

    /* [IN] status, hopefully USB_OK or USB_DONE */
    uint_32           status
)
{
    /* Body */

    USB_MASS_CLASS_INTF_STRUCT_PTR   plocal_intf;
    PIPE_INIT_PARAM_STRUCT_PTR       pPipe;
    COMMAND_OBJECT_PTR               cmd_ptr = NULL;
    pipe_handle = pipe_handle;
    buffer = buffer;
    length_data_transfered = length_data_transfered;
#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_reset_callback\r\n");
#endif

    if (user_parm)
    {
        /* Get the pointer to the pending request */
        plocal_intf = (USB_MASS_CLASS_INTF_STRUCT_PTR)user_parm;
        usb_class_mass_get_pending_request(plocal_intf, &cmd_ptr);
    } /* Endif */

    if (!cmd_ptr)
    {
#ifdef _HOST_DEBUG_
        bbu_printf("usb_class_mass_reset_callback, bad param\r\n");
#endif
        return;
    } /* Endif */

    /* Check the status of the reset and act accordingly */
    if (status == USB_OK)
    {
        switch (cmd_ptr->STATUS)
        {
        case STATUS_RESET_DEVICE:
            /* Reset the Bulk IN pipe */
            pPipe = (PIPE_INIT_PARAM_STRUCT_PTR)plocal_intf->BULK_IN_PIPE;
            if (!_usb_host_register_ch9_callback(plocal_intf->DEV_INST,
                                                 usb_class_mass_reset_callback, plocal_intf))
            {
                status = _usb_host_ch9_clear_feature(plocal_intf->DEV_INST,
                                                     REQ_TYPE_ENDPOINT, pPipe->ENDPOINT_NUMBER, ENDPOINT_HALT);
                cmd_ptr->STATUS = STATUS_RESET_BULK_IN;
            } /* Endif */
            break;

        case STATUS_RESET_BULK_IN:
            /* Reset the Bulk OUT pipe*/
            pPipe = (PIPE_INIT_PARAM_STRUCT_PTR)plocal_intf->BULK_OUT_PIPE;
            if (!_usb_host_register_ch9_callback(plocal_intf->DEV_INST,
                                                 usb_class_mass_reset_callback, plocal_intf))
            {
                status = _usb_host_ch9_clear_feature(plocal_intf->DEV_INST,
                                                     REQ_TYPE_ENDPOINT, pPipe->ENDPOINT_NUMBER, ENDPOINT_HALT);
                cmd_ptr->STATUS   =   STATUS_RESET_BULK_OUT;
            } /* Endif */
            break;

        case STATUS_RESET_BULK_OUT:
        case  STATUS_CLEAR_BULK_PIPE:
            cmd_ptr->STATUS = cmd_ptr->PREV_STATUS; /* restore status */
            status = usb_class_mass_pass_on_usb( plocal_intf);
            break;

        default:
            status = usb_class_mass_reset_recovery_on_usb(plocal_intf);
            break;
        } /* Endswitch */

    }
    else if (status == USBERR_ENDPOINT_STALLED)
    {

        cmd_ptr->STATUS = STATUS_CANCELLED;
        cmd_ptr->CALLBACK(status, plocal_intf, cmd_ptr, cmd_ptr->TR_BUF_LEN);
        usb_class_mass_deleteq(plocal_intf);

        /* go to the next packet if any */
        usb_class_mass_pass_on_usb(plocal_intf);
    } /* Endif */

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_reset_callback, SUCCESSFUL\r\n");
#endif

} /* Endbody */

/* EOF */

