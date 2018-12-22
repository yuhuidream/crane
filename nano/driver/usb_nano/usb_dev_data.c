#include "usb_devapi.h"
#include "usb_prv_dev.h"
#include "usb_types.h"
#include "usb_device.h"


#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif
#include "cache_ops.h"
#define _DATA_CACHE_
//#define __VUSBHS__
//#define _DEVICE_DEBUG_
extern void usb_device_free_XD(pointer);
extern uint_8 usb_dci_vusb20_recv_data
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] The transfer descriptor address */
    XD_STRUCT_PTR              xd_ptr
);


extern uint_8 usb_dci_vusb20_send_data
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] The transfer descriptor address */
    XD_STRUCT_PTR              xd_ptr
);

extern void usb_dci_vusb20_free_dTD
(
    /* [IN] the dTD to enqueue */
    pointer  dTD_ptr
);

uint_8 usb_dci_vusb20_cancel_transfer
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
);














/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_recv_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Receives data on a specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 usb_device_recv_data
(
    /* [IN] the USB_USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] buffer to receive data */
    uchar_ptr                  buff_ptr,

    /* [IN] length of the transfer */
    uint_32                    size
)
{
    /* Body */



#ifdef __VUSBHS__
    uint_8                           error = USB_OK;
    XD_STRUCT_PTR                    xd_ptr;
    USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
    // fl: add dev_ptr
    // VUSB20_REG_STRUCT_PTR            dev_ptr;
    //uint_32                            bit_pos;


#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_recv_data\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    //dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR; // fl:

#ifdef _DATA_CACHE_
    /********************************************************
    If system has a data cache, it is assumed that buffer
    passed to this routine will be aligned on a cache line
    boundry. The following code will invalidate the
    buffer before passing it to hardware driver.
    ********************************************************/
    //USB_dcache_invalidate_mlines((pointer)buff_ptr,size);

    dcache_invalidate_range((uint32_t)buff_ptr, size);

#endif

    USB_lock();

    if (!usb_dev_ptr->XD_ENTRIES)
    {
        USB_unlock();
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_recv_data, transfer in progress\r\n");
#endif

        return USB_STATUS_TRANSFER_IN_PROGRESS;
    } /* Endif */

    /* Get a transfer descriptor for the specified endpoint
    ** and direction
    */
    USB_XD_QGET(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, xd_ptr);

    usb_dev_ptr->XD_ENTRIES--;

    /* Initialize the new transfer descriptor */
    xd_ptr->EP_NUM = ep_num;
    xd_ptr->BDIRECTION = USB_RECV;
    xd_ptr->WTOTALLENGTH = size;
    xd_ptr->WSOFAR = 0;
    xd_ptr->WSTARTADDRESS = buff_ptr;
    xd_ptr->BSTATUS = USB_STATUS_TRANSFER_ACCEPTED;

#ifdef __USB_OS_MQX__
    error = ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)\
             usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_RECV(handle, xd_ptr);
#else
    error = usb_dci_vusb20_recv_data(handle, xd_ptr);
#endif

    //bit_pos = 0;

    USB_unlock();

    if (error)
    {
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_recv_data, receive failed\r\n");
#endif
        return USBERR_RX_FAILED;
    } /* Endif */

    return error;
#elif defined __VUSB32__                //no used
    XD_STRUCT_PTR              pxd;
    USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

#ifdef _DATA_CACHE_
    /********************************************************
    If system has a data cache, it is assumed that buffer
    passed to this routine will be aligned on a cache line
    boundry. The following code will invalidate the
    buffer before passing it to hardware driver.
    ********************************************************/
    USB_dcache_invalidate_mlines((pointer)buff_ptr, size);

#endif

    USB_lock();
    pxd = &usb_dev_ptr->XDRECV[ep_num];

    /*
    ** Check if the endpoint is disabled
    */
    if (pxd->STATUS == USB_STATUS_DISABLED)
    {
        USB_unlock();
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_recv_data, receive failed\r\n");
#endif
        return USBERR_RX_FAILED;
    } /* Endif */

    if ((pxd->TYPE == USB_CONTROL_ENDPOINT) && (pxd->SETUP_BUFFER_QUEUED))
    {
        pxd->SETUP_BUFFER_QUEUED = FALSE;
        usb_dev_ptr->XDSEND[ep_num].SETUP_BUFFER_QUEUED = FALSE;
        if (size)
        {
            usb_device_cancel_transfer(handle, ep_num, USB_RECV);
        }
        else
        {
#ifdef _DEVICE_DEBUG_
            bbu_printf("usb_device_recv_data, SUCCESSFUL\r\n");
#endif
            return USB_OK;
        } /* Endif */
    } /* Endif */

    /*
    ** Check if any transfer is in progress on this endpoint
    */
    if ((pxd->STATUS == USB_STATUS_TRANSFER_IN_PROGRESS) ||
            (pxd->STATUS == USB_STATUS_TRANSFER_PENDING))
    {
        USB_unlock();

#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_recv_data, transfer in progress\r\n");
#endif

        return USBERR_TRANSFER_IN_PROGRESS;
    } /* Endif */

    if (pxd->TYPE == USB_ISOCHRONOUS_ENDPOINT)
    {
        /* if Isochronous */
        if (size > pxd->MAXPACKET)
        {
            /*
            ** if asked for more than maxPacket
            ** limit request to maxPacket
            */
            size = pxd->MAXPACKET;
        } /* Endif */
    } /* Endif */

    /*
    ** Queue the transfer if the endpoint is enabled and is not busy.
    ** Note that in the case of a stalled endpoint the transfer will
    ** be queued even though it can not complete without host
    ** intervention.
    */
    pxd->STARTADDRESS = buff_ptr;
    pxd->NEXTADDRESS  = buff_ptr;
    pxd->TODO = size;
    pxd->SOFAR = 0;
    pxd->UNACKNOWLEDGEDBYTES = size;
    pxd->DIRECTION = USB_RECV;

    /* return successful transfer initiation status. */
    pxd->STATUS = USB_STATUS_TRANSFER_PENDING;

#ifdef __USB_OS_MQX__
    ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_dev_ptr->CALLBACK_STRUCT_PTR)->\
    DEV_RECV(handle, USB_RECV, ep_num);
#else
    usb_dci_vusb11_submit_transfer(handle, USB_RECV, ep_num);
#endif

    /*
    ** If the endpoint is currently stalled notify the user after the
    ** transfer has been queued.  By queuing the transfer the
    ** operation to revert the stall condition may be separated from
    ** the the data transfer code.
    */
    if (pxd->STATUS == USB_STATUS_STALLED)
    {
        USB_unlock();
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_recv_data, STALLED endpoint\r\n");
#endif
        return USBERR_ENDPOINT_STALLED;
    } /* Endif */

    USB_unlock();

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_recv_data, SUCCESSFUL\r\n");
#endif

    return USB_OK;
#endif
} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_send_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sends data on a specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 usb_device_send_data
(
    /* [IN] the USB_USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] buffer to send */
    uchar_ptr                  buff_ptr,

    /* [IN] length of the transfer */
    uint_32                    size
)
{
    /* Body */


#ifdef __VUSBHS__
    uint_8                           error;
    XD_STRUCT_PTR                    xd_ptr;
    USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_send_data\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
#ifdef _DATA_CACHE_
    /********************************************************
    If system has a data cache, it is assumed that buffer
    passed to this routine will be aligned on a cache line
    boundry. The following code will flush the
    buffer before passing it to hardware driver.
    ********************************************************/

    dcache_clean_range((uint32_t)buff_ptr, size);

    //USB_dcache_flush_mlines((pointer)buff_ptr,size);    //tong
#endif

    USB_lock();

    if (!usb_dev_ptr->XD_ENTRIES)
    {
        USB_unlock();
        bbu_printf("usb_device_send_data, transfer in progress\r\n");
#ifdef _DEVICE_DEBUG_

#endif
        return USB_STATUS_TRANSFER_IN_PROGRESS;
    } /* Endif */

    /* Get a transfer descriptor */
    USB_XD_QGET(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, xd_ptr);

    usb_dev_ptr->XD_ENTRIES--;

    /* Initialize the new transfer descriptor */
    xd_ptr->EP_NUM = ep_num;
    xd_ptr->BDIRECTION = USB_SEND;
    xd_ptr->WTOTALLENGTH = size;
    xd_ptr->WSOFAR = 0;
    xd_ptr->WSTARTADDRESS = buff_ptr;
    xd_ptr->BSTATUS = USB_STATUS_TRANSFER_ACCEPTED;

#ifdef __USB_OS_MQX__
    error = ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)\
             usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SEND(handle, xd_ptr);
#else
    error = usb_dci_vusb20_send_data(handle, xd_ptr);

#endif
    USB_unlock();

    if (error)
    {
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_send_data, transfer failed\r\n");
#endif
        return USBERR_TX_FAILED;
    } /* Endif */

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_send_data, SUCCESSFUL\r\n");
#endif
    return error;

#elif defined __VUSB32__
    XD_STRUCT_PTR              pxd;
    USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
#ifdef _DATA_CACHE_
    /********************************************************
    If system has a data cache, it is assumed that buffer
    passed to this routine will be aligned on a cache line
    boundry. The following code will flush the
    buffer before passing it to hardware driver.
    ********************************************************/
    USB_dcache_flush_mlines((pointer)buff_ptr, size);
#endif

    USB_lock();
    pxd = &usb_dev_ptr->XDSEND[ep_num];

    /*
    ** Check if the endpoint is disabled
    */
    if (pxd->STATUS == USB_STATUS_DISABLED)
    {
        USB_unlock();
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_send_data, transfer failed\r\n");
#endif
        return USBERR_TX_FAILED;
    } /* Endif */

    /*
    ** Check if any transfer is in progress on this endpoint
    */
    if (pxd->STATUS == USB_STATUS_TRANSFER_IN_PROGRESS)
    {
        USB_unlock();
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_send_data, transfer in progress\r\n");
#endif
        return USBERR_TRANSFER_IN_PROGRESS;
    } /* Endif */

    if (pxd->TYPE == USB_ISOCHRONOUS_ENDPOINT)
    {
        /* if Isochronous */
        if (size > pxd->MAXPACKET)
        {
            /*
            ** if asked for more than maxPacket
            ** limit request to maxPacket
            */
            size = pxd->MAXPACKET;
        } /* Endif */
    } /* Endif */

    /* The endpoint is enabled and no transfers are pending so queue the
    ** transmit transfer.
    */
    pxd->STARTADDRESS = buff_ptr;
    pxd->NEXTADDRESS  = buff_ptr;
    pxd->TODO = size;
    pxd->SOFAR = 0;
    pxd->UNACKNOWLEDGEDBYTES = size;
    pxd->DIRECTION = USB_SEND;

    /* return successful transfer initiation status. */
    pxd->STATUS = USB_STATUS_TRANSFER_PENDING;

#ifdef __USB_OS_MQX__
    ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_dev_ptr->CALLBACK_STRUCT_PTR)->\
    DEV_SEND(handle, USB_SEND, ep_num);
#else
    usb_dci_vusb11_submit_transfer(handle, USB_SEND, ep_num);
#endif

    /*
    ** If the endpoint is currently stalled notify the user after the
    ** transfer has been queued.  By queuing the transfer the
    ** operation to revert the stall condition may be separated from
    ** the the data transfer code.
    */
    if (pxd->STATUS == USB_STATUS_STALLED)
    {
        USB_unlock();
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_send_data, endpoint stalled\r\n");
#endif
        return USBERR_ENDPOINT_STALLED;
    } /* Endif */

    USB_unlock();

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_send_data,SUCCESSFUL\r\n");
#endif

    return USB_OK;
#endif
} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_cancel_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 usb_device_cancel_transfer
(
    /* [IN] the USB_USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
)
{
    /* Body */
    uint_8                        error = USB_OK;
    // USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

#ifdef _DEVICE_DEBUG_
    //bbu_printf("usb_device_cancel_transfer.\r\n");
#endif
    //bbu_printf("usb_device_cancel_transfer.\r\n");
    // usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    USB_lock();

    /* Cancel transfer on the specified endpoint for the specified
    ** direction
    */
#ifdef __VUSBHS__
#ifdef __USB_OS_MQX__
    error = ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)\
             usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_CANCEL_TRANSFER(handle, ep_num, direction);
#else
    error = usb_dci_vusb20_cancel_transfer(handle, ep_num, direction);
#endif
#elif defined __VUSB32__
#ifdef __USB_OS_MQX__
    ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_dev_ptr->CALLBACK_STRUCT_PTR)->\
    DEV_CANCEL_TRANSFER(handle, direction, ep_num);
#else
    usb_dci_vusb11_cancel_transfer(handle, direction, ep_num);
#endif
#else
#error "One of __VUSH32__ or __VUSBHS__ must be defined"
#endif

    USB_unlock();

#ifdef _DEVICE_DEBUG_
    //   bbu_printf("usb_device_cancel_transfer, SUCCESSFUL.\r\n");
#endif
    return error;

} /* EndBody */




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_cancel_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        Cancels a transfer
*
*END*-----------------------------------------------------------------*/
uint_8 usb_dci_vusb20_cancel_transfer
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

    MAVL_USB_TypeDef_PTR                new_dev_ptr; //by tongchao

    VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr, check_dTD_ptr;
    VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
    XD_STRUCT_PTR                                xd_ptr;
    uint_32                                      temp, bit_pos;

#ifdef _DEVICE_DEBUG_
    // bbu_printf("_usb_dci_vusb20_cancel_transfer\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR; //by tongchao

    bit_pos = (1 << (16 * direction + ep_num));
    temp = (2 * ep_num + direction);

    ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)new_dev_ptr->U2XEPLISTADDR + temp;

    /* Unlink the dTD */
    dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];

    if (dTD_ptr)
    {
        check_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)((uint_32)dTD_ptr->NEXT_TR_ELEM_PTR &
                        VUSBHS_TD_ADDR_MASK);

        if (dTD_ptr->SIZE_IOC_STS & VUSBHS_TD_STATUS_ACTIVE)
        {

            new_dev_ptr->U2XEPFLUSH = bit_pos;

            while (new_dev_ptr->U2XEPFLUSH & bit_pos)
            {
                /* ENDPTFLUSH bit should be cleared to indicate this
                ** operation is complete
                */
            }

            while (new_dev_ptr->U2XEPSTAT & bit_pos)
            {
                /* Write 1 to the Flush register */
                new_dev_ptr->U2XEPFLUSH = bit_pos;
                /* Wait until flushing completed */

                while (new_dev_ptr->U2XEPFLUSH & bit_pos)
                {
                    /* ENDPTFLUSH bit should be cleared to indicate this
                    ** operation is complete
                    */
                } /* EndWhile */
            } /* EndWhile */
        } /* Endif */

        /* Retire the current dTD */
        dTD_ptr->SIZE_IOC_STS = 0;
        dTD_ptr->NEXT_TR_ELEM_PTR = VUSBHS_TD_NEXT_TERMINATE;

        /* The transfer descriptor for this dTD */
        xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
        dTD_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
        /* Free the dTD */
        usb_dci_vusb20_free_dTD((pointer)dTD_ptr);

        /* Update the dTD head and tail for specific endpoint/direction */
        if (!check_dTD_ptr)
        {
            usb_dev_ptr->EP_DTD_HEADS[temp] = NULL;
            usb_dev_ptr->EP_DTD_TAILS[temp] = NULL;
            if (xd_ptr)
            {
                xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
                /* Free the transfer descriptor */
                usb_device_free_XD((pointer)xd_ptr);
            } /* Endif */
            /* No other transfers on the queue */
            ep_queue_head_ptr->NEXT_DTD_PTR = VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;
            ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;
        }
        else
        {
            usb_dev_ptr->EP_DTD_HEADS[temp] = check_dTD_ptr;

            if (xd_ptr)
            {
                if ((uint_32)check_dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD != (uint_32)xd_ptr)
                {
                    xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
                    /* Free the transfer descriptor */
                    usb_device_free_XD((pointer)xd_ptr);
                } /* Endif */
            } /* Endif */

            if (check_dTD_ptr->SIZE_IOC_STS & VUSBHS_TD_STATUS_ACTIVE)
            {

                /* Start CR 1015 */
                /* Prime the Endpoint */

                new_dev_ptr->U2XEPPRIME = bit_pos;

                if (!(new_dev_ptr->U2XEPSTAT & bit_pos))

                {
                    while (new_dev_ptr->U2XEPPRIME & bit_pos)
                    {
                    }

                    if (new_dev_ptr->U2XEPSTAT & bit_pos)
                    {
                        /* The endpoint was not not primed so no other transfers on
                        ** the queue
                        */
                        goto done;
                    } /* Endif */
                }
                else
                {
                    goto done;
                } /* Endif */

                /* No other transfers on the queue */
                ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)check_dTD_ptr;
                ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;

                /* Prime the Endpoint */
                new_dev_ptr->U2XEPPRIME = bit_pos;
            } /* Endif */
        } /* Endif */
    } /* Endif */
    return USB_OK;
done:

    return USB_OK;

    /* End CR 1015 */

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("_usb_dci_vusb20_cancel_transfer, SUCCESSFUL\r\n");
#endif

} /* EndBody */















