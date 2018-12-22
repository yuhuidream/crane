
/*** Description:
***  This file contains the VUSB_HS Device Controller interface functions.
***
**************************************************************************
**END*********************************************************/
#include "usb_devapi.h"
#include "usb_prv_dev.h"

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif

#define  USB_TEST_MODE_TEST_PACKET_LENGTH   (53)

/* Test packet for Test Mode : TEST_PACKET. USB 2.0 Specification section 7.1.20 */
#if 0
static uint_8 test_packet[USB_TEST_MODE_TEST_PACKET_LENGTH] =
{
    /* Synch */
    /* DATA 0 PID */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
    0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF,
    0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF,
    0xEF, 0xF7, 0xFB, 0xFD, 0x7E
};
#endif
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_assert_resume
*  Returned Value : None
*  Comments       :
*        Resume signalling for remote wakeup
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_assert_resume
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

    uint_32                                      temp;
    MAVL_USB_TypeDef_PTR            new_dev_ptr;  //by tongchao



#ifdef _DEVICE_DEBUG_
    bbu_printf("_usb_dci_vusb20_assert_resume");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;


    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;

    temp = new_dev_ptr->U2XPORTSC[0];
    temp &= ~EHCI_PORTSCX_W1C_BITS;
    temp |= EHCI_PORTSCX_PORT_FORCE_RESUME;
    new_dev_ptr->U2XPORTSC[0] = temp;
    /* Port change interrupt will be asserted at the end of resume
    ** operation
    */

#ifdef _DEVICE_DEBUG_
    bbu_printf("_usb_dci_vusb20_assert_resume, SUCCESSFUL");
#endif

} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_stall_endpoint
*  Returned Value : None
*  Comments       :
*        Stalls the specified endpoint
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_stall_endpoint
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

    VUSB20_EP_QUEUE_HEAD_STRUCT _PTR_            ep_queue_head_ptr;


    MAVL_USB_TypeDef_PTR           new_dev_ptr;    //by tongchao

#ifdef _DEVICE_DEBUG_
    bbu_printf("_usb_dci_vusb20_stall_endpoint");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;


    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR; //by tongchao


    ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)new_dev_ptr->U2XEPLISTADDR + 2 * ep_num + direction; //by tongchao
    /* Stall the endpoint for Rx or Tx and set the endpoint type */
    if (ep_queue_head_ptr->MAX_PKT_LENGTH & VUSB_EP_QUEUE_HEAD_IOS)
    {

        new_dev_ptr->U2XEPCTRL[ep_num] |= (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);

    }
    else
    {

        new_dev_ptr->U2XEPCTRL[ep_num] |= (direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL);


    } /* Endif */

#ifdef _DEVICE_DEBUG_
    bbu_printf("_usb_dci_vusb20_stall_endpoint, SUCCESSFUL");
#endif

} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_unstall_endpoint
*  Returned Value : None
*  Comments       :
*        Unstall the specified endpoint in the specified direction
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_unstall_endpoint
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



    MAVL_USB_TypeDef_PTR           new_dev_ptr;//by tongchao
#ifdef _DEVICE_DEBUG_
    bbu_printf("_usb_dci_vusb20_unstall_endpoint");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;



    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR; //by tongchao

    new_dev_ptr->U2XEPCTRL[ep_num] &= (direction ? ~EHCI_EPCTRL_TX_EP_STALL : ~EHCI_EPCTRL_RX_EP_STALL);
#ifdef _DEVICE_DEBUG_
    bbu_printf("_usb_dci_vusb20_unstall_endpoint, SUCCESSFUL");
#endif

} /* EndBody */



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_dci_vusb20_get_endpoint_status
* Returned Value : None
* Comments       :
*     Gets the endpoint status
*
*END*--------------------------------------------------------------------*/
uint_8 usb_dci_vusb20_get_endpoint_status
(
    /* [IN] Handle to the USB device */
    usb_device_handle   handle,

    /* [IN] Endpoint to get */
    uint_8               ep
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

    MAVL_USB_TypeDef_PTR           new_dev_ptr;//by tongchao
#ifdef _DEVICE_DEBUG_
    //  bbu_printf("_usb_dci_vusb20_get_endpoint_status");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR; //by tongchao
#ifdef _DEVICE_DEBUG_
    //  bbu_printf("_usb_dci_vusb20_get_endpoint_status, SUCCESSFUL");
#endif
    // bbu_printf("new_dev_ptr->U2XEPCTRL[ep]   is 0x%x\r\n",new_dev_ptr->U2XEPCTRL[ep]);
    return ((new_dev_ptr->U2XEPCTRL[ep] &
             (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL)) ? 1 : 0);

} /* EndBody */




/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_dci_vusb20_set_test_mode
* Returned Value : None
* Comments       :
*     sets/resets the test mode
*
*END*--------------------------------------------------------------------*/
#if 0
void usb_dci_vusb20_set_test_mode
(
    /* [IN] Handle to the USB device */
    usb_device_handle handle,

    /* [IN] Test mode */
    uint_16 test_mode
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    uint_32                                      temp;


    MAVL_USB_TypeDef_PTR           new_dev_ptr;//by tongchao
#ifdef _DEVICE_DEBUG_
    bbu_printf("_usb_dci_vusb20_set_test_mode");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;


    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR; //by tongchao


    temp = new_dev_ptr->U2XEPCTRL[0];
    new_dev_ptr->U2XEPCTRL[0] &= (temp | EHCI_EPCTRL_TX_DATA_TOGGLE_RST);


    if (test_mode == USB_TEST_MODE_TEST_PACKET)
    {
        usb_device_send_data(handle, 0, test_packet,
                             USB_TEST_MODE_TEST_PACKET_LENGTH);
    } /* Endif */


    temp = new_dev_ptr->U2XPORTSC[0];      //by tongchao
    temp &= ~EHCI_PORTSCX_W1C_BITS;

    new_dev_ptr->U2XPORTSC[0] = (temp |  ((uint_32)test_mode << 8));      //bit 16:19
#ifdef _DEVICE_DEBUG_
    bbu_printf("_usb_dci_vusb20_set_test_mode, SUCCESSFUL");
#endif


} /* EndBody */

#endif
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_dci_vusb20_set_endpoint_status
* Returned Value : None
* Comments       :
*     Sets the endpoint registers e.g. to enable TX, RX, control
*
*END*--------------------------------------------------------------------*/
void usb_dci_vusb20_set_endpoint_status
(
    /* [IN] Handle to the USB device */
    usb_device_handle   handle,

    /* [IN] Endpoint to set */
    uint_8               ep,

    /* [IN] Endpoint characteristics */
    uint_8               stall
)
{
    /* Body */

    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

    MAVL_USB_TypeDef_PTR           new_dev_ptr;//by tongchao
#ifdef _DEVICE_DEBUG_
    bbu_printf("_usb_dci_vusb20_set_endpoint_status");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR; //by tongchao

    if (stall)
    {

        new_dev_ptr->U2XEPCTRL[ep] |=
            (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);

    }
    else
    {

        new_dev_ptr->U2XEPCTRL[ep] &=
            ~(EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);

    } /* Endif */
    // bbu_printf("_usb_set_endpoint_status,new_dev_ptr->U2XEPCTRL is 0x%x\r\n",new_dev_ptr->U2XEPCTRL[ep]);
#ifdef _DEVICE_DEBUG_
    bbu_printf("_usb_dci_vusb20_set_endpoint_status, SUCCESSFUL \r\n");
#endif

} /* EndBody */








