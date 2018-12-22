

// ============================================================================
//
//     Copyright (c) Marvell Corporation 2013  -  All rights reserved
//
//  This computer program contains confidential and proprietary information,
//  and  may NOT  be reproduced or transmitted, in whole or in part,  in any
//  form,  or by any means electronic, mechanical, photo-optical, or  other-
//  wise, and  may NOT  be translated  into  another  language  without  the
//  express written permission from Marvell Corporation.
//
// ============================================================================
// =      C O M P A N Y      P R O P R I E T A R Y      M A T E R I A L       =
// ============================================================================

/*
-------------------------------------------------------------------------------
*
*
*  description:   USB ACM(serial)  driver  file
*
*  author:chaotong@marvell.com
*
*
*
*
*-----------------------------------------------------------------------------*/

#include "predefines.h"
#include "bbu.h"
#include "qp_port.h"
#include "usb_device.h"
#include "timer_dec.h"

#include"USB_PHY.h"

#include "usb_prv_dev.h"
#include "usb_define.h"
#include "usb_devapi.h"
#include "usb_main.h"
#include "USB.h"
#include <string.h>
#include "interrupt.h"
#include "usb_serial.h"
#include "set_mmu.h"

//#define USB_DEBUG
#if defined(USB_DEBUG)
#define  _DEVICE_DEBUG_
#endif

//#define  _USB_CHARGER
#define USB_PRIORITY    15
static uint_8 ep_number = 1;

static uint_8 ep_isrNumber = 3; //            2    ;
static uint_8 serBuffer_length = 56; //32;
static uint_8  count_in = FALSE;
static uint8_t *buffer_out;
static uint8_t *buffer_in;
static uint8_t  debug_serial_begin = 0;

//static uint8_t *buffer_out2;
//static uint8_t *buffer_in2;
//static uint8_t  qspy_serial_begin=0;
static uint_8  init_log[1024] = {0};
uint32_t  initLength = 0;
#if 0
LINE_CODING linecoding =
{
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* no. of bits 8*/
};
static   LINE_CODING  LINE_CODE_SET[8] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
#endif
/***************************************************************************
 * USB String Descriptors
 ***************************************************************************/
#define STRING_MANUFACTURER 1
#define STRING_PRODUCT             2
#define STRING_SERIAL              3


#define SET_ADDRESS_HARDWARE_ASSISTANCE

char                 global_c;
void (*usb_ISR_Handle)(void);

static usb_device_handle   usb_dev_handle;



static volatile uchar    device_speed = 1;

/*The following global variables are used for USB electrical testing
in which device should switch to test mode under a control command*/

static volatile boolean  usb_ENTER_TEST_MODE = FALSE;
static volatile uint_16  usb_test_mode_index = 0;
__attribute__ ((aligned (32)))



#define DEVICE_DESCRIPTOR_SIZE 18
/* Device descriptors are always 18 bytes */



__attribute__ ((aligned (4)))
static     uint_8  usb_DevDesc[18] =
{
    sizeof (usb_DevDesc),                             /* Length of DevDesc */
    1,                             /* "Device" Type of descriptor */
    0, 2,                                                  /* BCD USB version */
    //0x10,0x01,
    0x02,       //   0x00,   /* communition class */
    0x00,                                                  /* Device Subclass is indicated in the interface descriptors */
    0x00,                                                  /* Mass storage devices do not use class-specific protocols */
    // 0xEF,   /* bDeviceClass: CDC */
    //  0x02,   /* bDeviceSubClass */
    //   0x01,   /* bDeviceProtocol */
    64,           /* Max packet size */
    //0x10,
    USB_uint_16_low (DRIVER_VENDOR_ID), USB_uint_16_high (DRIVER_VENDOR_ID),   /* Vendor ID */////////////////////////////////////////////
    USB_uint_16_low (DRIVER_PRODUCT_ID), USB_uint_16_high (DRIVER_PRODUCT_ID),   /* Product ID *///////////////////////////////////////////
    USB_uint_16_low (0x0001), USB_uint_16_high (0x0001),///////////////////////////
    /* BCD Bootloader version */
    /**********modified by cosmo****************/
    //0,
    //0,
    //  0,
    0,// STRING_MANUFACTURER,                   /* Manufacturer string index *//////////////////////////////////////////////////////
    STRING_PRODUCT,                            /* Product string index *////////////////////////////////////////////////////////
    0,// STRING_SERIAL,                               /* Serial number string index *////////////////////////////////////////////////////
    //  0,
    0x1                                                  /* Number of configurations available */////////////////////////////////////////
};




#define CONFIG_DESC_NUM_INTERFACES  (1)

#define  CONFIG_DESC_SIZE     (75-8)// (67+66+8)

static uint_8 usb_ConfigDesc[CONFIG_DESC_SIZE] =
{

    9,      /* bLength   Length of this descriptor */
    2,      /* bDescType   2=CONFIGURATION */

    CONFIG_DESC_SIZE & 0xff, CONFIG_DESC_SIZE >> 8,
    0x02,      /* bNumInterfaces   Number of interfaces */

    /* USB_CONFIG_VAL: */
    0x01,   /* bConfigValue   Configuration Value */
    /********modified by cosmo************************************/
    //   0x04,   /* iConfig   String Index for this config = #01 */
    0x0,
    /********************************************************************************/
    0x80,   /* bmAttributes   attributes - self powered */
    0x0a,      /* MaxPower   self-powered draws 0 mA from the bus. */
    /*   IIIII  FFFFF   CCCC   000
    **     I    F      C      0   0
    **     I    FFF    C      0   0
    **     I    F      C      0   0
    **   IIIII  F       CCCC   000
    */
    // IAD
#if 0
    0x08,   // bLength: Interface Descriptor size
    0x0B,       // bDescriptorType: IAD
    acmInterfaceNum,    // bFirstInterface
    0x02,   // bInterfaceCount
    0x02,   // bFunctionClass: CDC
    0x02,   // bFunctionSubClass
    0x01,   // bFunctionProtocol
    0x02,   // iFunction
#endif
    /* Interface Descriptor*/
    //#ifdef LINUX_CDC
    0x09,   /* blength   Length of this desriptor */
    4,      /* bDescriptorType   4=INTERFACE */
    acmInterfaceNum,      /* bIfaceNum   interface number */
    0,      /* bAltSetting   Alternate setting 0 (no others) */
    1,      /* bNumEndpoints   Number of endpoints= use EP1 only */
    0x02,   /* bIfaceClass   Interface class 02=cdc class */
    0x02,   /* bIfaceSubClass   ACM */
    0x01,   /* bIfaceProtocol   AT commands*/
    0x02,      /* iInterface */

    //Header Function Descriptor

    0x05,
    0x24,
    0x00,      //subtype = header
    0x00,
    0x01,

    //Call Manangement Func Desc
    0x05,
    0x24,
    0x01,// subtype = Call management
    0x03, //bmcapabilities; MS driver usbser.sys seems to ignore this bit and sends AT over the data,interface anyway
    0x01,

    //Abstract Control Func Desc
    0x04,
    0x24,
    0x02,
    0x02,

    /*Union Functional Descriptor*/
    0x05,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x06,   /* bDescriptorSubtype: Union func desc */
    acmInterfaceNum,//0x00,   /* bMasterInterface: Communication class interface */
    acmInterfaceNum + 0x01, /* bSlaveInterface0: Data Class Interface */

    /*Endpoint 2 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    0x05,   /* bDescriptorType: Endpoint */
    0x83,   /* bEndpointAddress: (IN2) */
    0x03,   /* bmAttributes: Interrupt */
    0x0f,      /* wMaxPacketSize: */
    0x00,
    0x09,   /* bInterval: */
    //#endif

    /*Data class interface descriptor*/
    0x09,   /* bLength: Endpoint Descriptor size */
    0x04,  /* bDescriptorType: */
    acmInterfaceNum + 0x01, /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints: Two endpoints used */
    0x0A,   /* bInterfaceClass: CDC */
    0x00,   /* bInterfaceSubClass: */
    0x00,   /* bInterfaceProtocol: */
    0x00,   /* iInterface: */

    /*Endpoint 1 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    0x05,   /* bDescriptorType: Endpoint */
    (0x80 | 0x01), /* bEndpointAddress: (IN1) */
    0x02,   /* bmAttributes: Bulk */
    USB_uint_16_low(VIRTUAL_COM_PORT_DATA_SIZE),
    USB_uint_16_high(VIRTUAL_COM_PORT_DATA_SIZE),
    0x00   ,/* bInterval */

    /*Endpoint 1 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    0x05,   /* bDescriptorType: Endpoint */
    0x01,   /* bEndpointAddress: (OUT1) receive*/
    0x02,   /* bmAttributes: Bulk */
    USB_uint_16_low(VIRTUAL_COM_PORT_DATA_SIZE),
    USB_uint_16_high(VIRTUAL_COM_PORT_DATA_SIZE),
    0x00,   /* bInterval: ignore for Bulk transfer */


};




#define OTHER_SPEED_CONFIG_DESC_SIZE  CONFIG_DESC_SIZE




//static uchar USB_IF_ALT[4] = {0, 0, 0, 0};

/* number of strings in the table not including 0 or n. */
static const uint_8 usb_USB_STR_NUM = 6;

/*
** if the number of strings changes, look for usb_USB_STR_0 everywhere and make
** the obvious changes.  It should be found in 3 places.
*/

static uint_16 usb_USB_STR_0[ 2] = {0x300 + sizeof(usb_USB_STR_0), 0x0409};
static const uint_16 usb_USB_STR_1[18] = {0x300 + sizeof(usb_USB_STR_1),
                                          'M', 'A', 'R', 'V', 'E', 'L', 'L'
                                         };
static  const uint_16 usb_USB_STR_2[18 + 4] = {0x300 + sizeof(usb_USB_STR_2),
                                               'B', 'B', 'U', ' ', 'U', 'S', 'B', ' ', 'S', 'E', 'R', 'I', 'A', 'L', ' ', 'D', 'e', 'v', 'i', 'c', 'e'
                                              };

static uint_16 usb_USB_STR_3[ 5] = {0x300 + sizeof(usb_USB_STR_3),
                                    '#', '0', '2'
                                   };
static uint_16 usb_USB_STR_4[ 4] = {0x300 + sizeof(usb_USB_STR_4),
                                    '#', '0', '2'
                                   };
static uint_16 usb_USB_STR_5[ 4] = {0x300 + sizeof(usb_USB_STR_5),
                                    '_', 'A', '1'
                                   };
static uint_16 usb_USB_STR_6[15] = {0x300 + sizeof(usb_USB_STR_6),
                                    'Y', 'o', 'u', 'r', ' ', 'n', 'a', 'm', 'e', ' ', 'h', 'e', 'r', 'e'
                                   };
static uint_16 usb_USB_STR_n[17] = {0x300 + sizeof(usb_USB_STR_n),
                                    'B', 'A', 'D', ' ', 'S', 'T', 'R', 'I', 'N', 'G', ' ', 'I', 'n', 'd', 'e', 'x'
                                   };
#define USB_STRING_ARRAY_SIZE  8
static const uint_8_ptr usb_USB_STRING_DESC[USB_STRING_ARRAY_SIZE] =
{
    (uint_8_ptr)((pointer)usb_USB_STR_0),
    (uint_8_ptr)((pointer)usb_USB_STR_1),
    (uint_8_ptr)((pointer)usb_USB_STR_2),
    (uint_8_ptr)((pointer)usb_USB_STR_3),
    (uint_8_ptr)((pointer)usb_USB_STR_4),
    (uint_8_ptr)((pointer)usb_USB_STR_5),
    (uint_8_ptr)((pointer)usb_USB_STR_6),
    (uint_8_ptr)((pointer)usb_USB_STR_n)
};

uint_16        usb_usb_status;
uint_8         usb_endpoint, usb_if_status;
uint_8         usb_data_to_send;
uint_16        usb_sof_count;

SETUP_STRUCT   usb_local_setup_packet;


/**************************************************************************
DESCRIPTORS DESCRIPTORS DESCRIPTORS DESCRIPTORS DESCRIPTORS DESCRIPTORS
**************************************************************************/
#define DEVICE_DESCRIPTOR_SIZE 18


/*************tongchao*************/
extern void USB_malloc_init(void);
extern void usb_DiskBufInit(void);
extern uint_8 usb_dci_vusb20_send_data(usb_device_handle handle, XD_STRUCT_PTR ptr);
/**************************************************************************
The following code defines MASS STORAGE DISK storage area as an extern
if file system is enabled or it defines hardware codes array with boot
sector area. This array will bw used as a storage disk area if
MFS is not enabled. In general in this example, MFS is always enabled
at compile time and boot sector array is just provided for example
to customers. You may like to omit this array in your final application
code.
**************************************************************************/





/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  :usb_device_send_
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sends data on a specified endpoint.
*
*END*-----------------------------------------------------------------*/
static U8 usb_device_send_ep0_zero (  usb_device_handle handle, U8 wait)
{
    XD_STRUCT_PTR xd_ptr;
    U8 status = USB_OK;

    USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;


    MAVL_USB_TypeDef_PTR         new_dev_ptr;                  //by tongchao

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;



    if (!usb_dev_ptr->XD_ENTRIES)
    {
        //  ASSERT(FALSE);
        return USB_STATUS_TRANSFER_IN_PROGRESS;
    }

    /* added by yqian according to P86 5.4.3.1.2, poll this register to assure the relative bit of ep0 has been cleared
    before priming for the status stage.*/

    //while ((dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT) & 0x1);
    if ((new_dev_ptr->U2XEPSETUPSTAT) & 0x1) new_dev_ptr->U2XEPSETUPSTAT = 1;                            // by tongchao


    //while (BB_REGS32(CYG_DEVICEusb_ENDPSETSTS) & 0x1)  ;

    /* Get a transfer descriptor */
    USB_XD_QGET (usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, xd_ptr);

    // ASSERT( xd_ptr != NULL );

    usb_dev_ptr->XD_ENTRIES--;

    /* Initialize the new transfer descriptor */
    xd_ptr->EP_NUM = 0;
    xd_ptr->BDIRECTION = USB_SEND;
    xd_ptr->WTOTALLENGTH = 0;
    xd_ptr->WSTARTADDRESS = (LP_U8) usb_DevDesc;
    xd_ptr->BSTATUS = USB_STATUS_TRANSFER_ACCEPTED;

    usb_dci_vusb20_send_data(handle, xd_ptr);
    if (wait)
    {

        while (  ( new_dev_ptr->U2XEPCOMPLETE & 0x10001) == 0)    // by tongchao
        {
            udelay(5);
            //  bbu_printf("new_dev_ptr->U2XEPCOMPLETE is 222 0x%x\r\n",new_dev_ptr->U2XEPCOMPLETE);
            /* Check if there was USB Reset */

            if (   (new_dev_ptr->U2XUSBSTS)  & EHCI_STS_RESET)   //by tongchao
            {
                return USBERR_SHUTDOWN;
            }

            /* Check if there was new Setup Request
               Even if this case is not common,
               we should prepare abnormal behavior of Host side
            */

            if (      (new_dev_ptr->U2XEPSETUPSTAT) & 1)                       //by tongchao
            {
                usb_device_cancel_transfer (handle, 0, USB_SEND);

                status = USBERR_TX_FAILED;
                break;
            }
        }
        usb_dci_vusb20_process_tr_complete (handle);
    }
    return status;
}



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_ch9GetStatus
* Returned Value : None
* Comments       :
*     Chapter 9 GetStatus command
*     WVALUE=Zero
*     WINDEX=Zero
*     WLENGTH=1
*     DATA=bmERR_STAT
*     The GET_STATUS command is used to read the bmERR_STAT register.
*
*     Return the status based on the bRrequestType bits:
*     device (0) = bit 0 = 1 = self powered
*                  bit 1 = 0 = DEVICE_REMOTE_WAKEUP which can be modified
*     with a SET_FEATURE/CLEAR_FEATURE command.
*     interface(1) = 0000.
*     endpoint(2) = bit 0 = stall.
*     static uint_8_ptr pData;
*
*     See section 9.4.5 (page 190) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_ch9SetAddress
* Returned Value : None
* Comments       :
*     Chapter 9 SetAddress command
*     We setup a TX packet of 0 length ready for the IN token
*     Once we get the TOK_DNE interrupt for the IN token, then
*     we change the ADDR register and go to the ADDRESS state.
*     See section 9.4.6 (page 192) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/
static void ch9SetAddress
(
    /* USB handle */
    usb_device_handle handle,

    /* Is it a Setup phase? */
    boolean setup,

    /* The setup packet pointer */
    SETUP_STRUCT_PTR setup_ptr
)
{
    /* Body */
    static uint_8 new_address;
    // uint_32  max_pkt_size;
    USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;
    uint_32 usb_state;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;



    if (setup)
    {

        if ((usb_local_setup_packet.REQUESTTYPE == 0)
                && (usb_local_setup_packet.INDEX == 0)
                && (usb_local_setup_packet.LENGTH == 0))
        {
            /* OK, it is Valid */

        }
        else
        {
            /* Even if this case is not specified in USB spec,
               DUT should STALL for robustness.
            */
            usb_device_stall_endpoint (handle, 0, USB_RECV);
            return;
        }

        usb_state = usb_dev_ptr->USB_STATE;
        if ((usb_state == USB_STATE_CONFIG) )//||
            // (usb_state == USB_STATE_AUTH))
        {
            /* Even if this case is not specified in USB spec,
               DUT should STALL for robustness.
            */

            usb_device_stall_endpoint (handle, 0, USB_RECV);
            return;
        }
        else
        {
            /* OK, it is Valid */
        }

        if (usb_local_setup_packet.VALUE > 127)
        {
            /* Even if this case is not specified in USB spec,
               DUT should STALL for robustness.
            */

            usb_device_stall_endpoint (handle, 0, USB_RECV);
            return;

        }


        /*******************************************************
        if hardware assitance is enabled for set_address (see
        hardware rev for details) we need to do the set_address
        before queuing the status phase.
        *******************************************************/

        //usb_device_send_data(handle, 0, (uchar_ptr)((pointer)&usb_DevDesc),MIN(setup_ptr->LENGTH, DEVICE_DESCRIPTOR_SIZE));

        //status stage

        if (usb_device_send_ep0_zero (handle, 1) == USB_OK) //Only after status stage can the request be accepted.
        {

            new_address = setup_ptr->VALUE;
            //bbu_printf("new address :%d ",new_address);

            //bbu_printf("\r\n");
            //  usb_dev_state.DEVICE_ADDRESS = i;
            /* The address bits are past bit 25-31. Set the address */

#ifdef SET_ADDRESS_HARDWARE_ASSISTANCE
            usb_device_set_status(handle, USB_STATUS_ADDRESS, new_address);
#endif

            usb_dev_ptr->USB_CURR_CONFIG = 0;

            if ( usb_dev_ptr->DEVICE_ADDRESS != 0)
            {
                usb_dev_ptr->USB_STATE = USB_STATE_ADDRESS;
            }
            else
            {
                usb_dev_ptr->USB_STATE = USB_STATE_DEFAULT;
            }
        }

        /* status phase */
        usb_device_send_data(handle, 0, 0, 0);

    }
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_ch9GetDescription
* Returned Value : None
* Comments       :
*     Chapter 9 GetDescription command
*     The Device Request can ask for Device/Config/string/interface/endpoint
*     descriptors (via WVALUE). We then post an IN response to return the
*     requested descriptor.
*     And then wait for the OUT which terminates the control transfer.
*     See section 9.4.3 (page 189) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/

static void ch9GetDescription
(
    /* USB handle */
    usb_device_handle handle,

    /* Is it a Setup phase? */
    boolean setup,

    /* The setup packet pointer */
    SETUP_STRUCT_PTR setup_ptr
)
{
    /* Body */
    // uint_32  max_pkt_size;
    //   volatile int test=0;
    //uint_32 len;



    if (setup)
    {
        /* Load the appropriate string depending on the descriptor requested.*/
        switch (setup_ptr->VALUE & 0xFF00)
        {

        case 0x0100:

            //len = MIN(setup_ptr->LENGTH, DEVICE_DESCRIPTOR_SIZE);
#ifdef _DEVICE_DEBUG_
            //  bbu_printf("device description  0x0100\r\n");
#endif
            usb_device_send_data(handle, 0, (uchar_ptr)((pointer)&usb_DevDesc), MIN(setup_ptr->LENGTH, DEVICE_DESCRIPTOR_SIZE));

            break;

        case 0x0200:
            /* Set the Max Packet Size in the config and other speed config */

            //max_pkt_size = EP1_HS_MAX_PACKET_SIZE;

            usb_ConfigDesc[1] = 2;            //send the regular configDesc descripter

            //len = setup_ptr->LENGTH;
#ifdef _DEVICE_DEBUG_
            // bbu_printf("device description  0x0200  setup_ptr->LENGTH  is %d  \r\n  ",setup_ptr->LENGTH);
#endif
            usb_device_send_data(handle, 0, (uchar_ptr)((pointer)&usb_ConfigDesc), MIN(setup_ptr->LENGTH, CONFIG_DESC_SIZE));

            break;

        case 0x0300:
            ///BBU_putstr( "Send String \r\n");
#ifdef _DEVICE_DEBUG_
            // bbu_printf("device description  0x0300\r\n");
#endif
            if ((setup_ptr->VALUE & 0x00FF) > usb_USB_STR_NUM)
            {
                usb_device_send_data(handle, 0, usb_USB_STRING_DESC[usb_USB_STR_NUM + 1],
                                     MIN(setup_ptr->LENGTH, usb_USB_STRING_DESC[usb_USB_STR_NUM + 1][0]));
            }
            else
            {
                usb_device_send_data(handle, 0,
                                     usb_USB_STRING_DESC[setup_ptr->VALUE & 0x00FF],
                                     MIN(setup_ptr->LENGTH,
                                         usb_USB_STRING_DESC[setup_ptr->VALUE & 0x00FF][0]));
            } /* Endif */

            break;

        case 0x600:

            break;

        case 0x700:


            usb_ConfigDesc[1] = 7;
            usb_device_send_data(handle, 0, (uchar_ptr)usb_ConfigDesc,
                                 MIN(setup_ptr->LENGTH, CONFIG_DESC_SIZE));
            break;

        default:
            usb_device_stall_endpoint(handle, 0, 0);
            return;
        } /* Endswitch */
        /* status phase */
        usb_device_recv_data(handle, 0, 0, 0);
    } /* Endif */
    return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_ch9SetDescription
* Returned Value : None
* Comments       :
*     Chapter 9 SetDescription command
*     See section 9.4.8 (page 193) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_ch9GetConfig
* Returned Value : None
* Comments       :
*     Chapter 9 GetConfig command
*     See section 9.4.2 (page 189) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_ch9SetConfig
* Returned Value : None
* Comments       :
*     Chapter 9 SetConfig command
*
*END*--------------------------------------------------------------------*/
static void ch9SetConfig
(
    /* USB handle */
    usb_device_handle handle,

    /* Is it a Setup phase? */
    boolean setup,

    /* The setup packet pointer */
    SETUP_STRUCT_PTR setup_ptr
)
{
    /* Body */
    uint_16 usb_state;
    uint_32  max_pkt_size;
    USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    if (setup)
    {

        if ((usb_local_setup_packet.REQUESTTYPE == 0)
                && (usb_local_setup_packet.INDEX == 0)
                && (usb_local_setup_packet.LENGTH == 0)
                && (usb_local_setup_packet.VALUE & 0xFF00) == 0)
        {
            /* OK, it is Valid */
        }
        else
        {
            /* Even if this case is not specified in USB spec,
               DUT should STALL for robustness. */
            usb_device_stall_endpoint (handle, 0, USB_RECV);
            return;
        }

        usb_state = usb_dev_ptr->USB_STATE;
        if ((usb_state == USB_STATE_CONFIG)
                || (usb_state == USB_STATE_ADDRESS))
            //  || (usb_state == USB_STATE_AUTH))
        {
            /* OK, it is Valid */
            //bbu_printf(" usb_state   in usb_ch9SetConfig  is %d\r\n",usb_state);
        }
        else
        {
            /* Even if this case is not specified in USB spec,
               DUT should STALL for robustness. */
            usb_device_stall_endpoint (handle, 0, USB_RECV);
            return;
        }

        if ((setup_ptr->VALUE & 0x00FF) > 1)
        {
            /* generate stall */
            usb_device_stall_endpoint(handle, 0, 0);
            return;
        } /* Endif */
        // bbu_printf("setup_ptr->VALUE n usb_ch9SetConfig  is %d  \r\n",setup_ptr->VALUE);
        /* 0 indicates return to unconfigured state */
        if ((setup_ptr->VALUE & 0x00FF) == 0)
        {

            /* clear the currently selected config value */


            usb_device_set_status(handle, USB_STATUS_CURRENT_CONFIG, 0);
            usb_device_set_status(handle, USB_STATUS_DEVICE_STATE,  USB_STATE_ADDRESS);

#ifdef DEBUG_SEND
            // usb_device_cancel_transfer (handle, BULK_IN_EP, USB_SEND);
#endif
            usb_device_cancel_transfer (handle, ep_number, USB_RECV);
            usb_device_cancel_transfer (handle, ep_isrNumber, USB_SEND);
            /* status stage */

            usb_device_send_ep0_zero (handle, 0);
            return;
            /* status phase */

        } /* Endif */


        /*
        ** If the configuration value (setup_ptr->VALUE & 0x00FF) differs
        ** from the current configuration value, then endpoints must be
        ** reconfigured to match the new device configuration
        */
        usb_device_get_status(handle, USB_STATUS_CURRENT_CONFIG,  &usb_state);
        //  bbu_printf(" usb_state 22222  in usb_ch9SetConfig  is %d\r\n",usb_state);
        if (usb_state != (setup_ptr->VALUE & 0x00FF))
        {
            /* Reconfigure endpoints here */
            switch (setup_ptr->VALUE & 0x00FF)
            {

            /* Reconfigure endpoints here */
            case 1:

                max_pkt_size = VIRTUAL_COM_PORT_DATA_SIZE;            // high speed value

                /* Set_Config is the last step of enumeration process, here starts to activate other endpoint for application. */
                /****************************modified by cosmo*EP1-->ep_num*******************/
                usb_device_init_endpoint(handle, ep_isrNumber, ISR_EP_SIZE, USB_SEND, USB_INTERRUPT_ENDPOINT, USB_DEVICE_DONT_ZERO_TERMINATE);

                usb_device_init_endpoint(handle, ep_number, max_pkt_size, USB_RECV, USB_BULK_ENDPOINT, USB_DEVICE_DONT_ZERO_TERMINATE);
                usb_device_init_endpoint(handle, ep_number, max_pkt_size, USB_SEND, USB_BULK_ENDPOINT, USB_DEVICE_DONT_ZERO_TERMINATE);


                if (usb_device_get_transfer_status(handle, ep_number, USB_RECV) == USB_STATUS_IDLE)
                {
                    // usb_device_send_data(handle,ep_number,buffer_in,1);
                    usb_device_send_data(handle, ep_number, init_log, initLength);

                    usb_device_recv_data(handle, ep_number, buffer_out, serBuffer_length);

                }

                break;

            default:
                usb_device_stall_endpoint (handle, 0, USB_RECV);
                return;
                //break;
            } /* Endswitch */
            usb_device_set_status(handle, USB_STATUS_CURRENT_CONFIG, setup_ptr->VALUE & 0x00FF);
            usb_device_set_status(handle, USB_STATUS_DEVICE_STATE, USB_STATE_CONFIG);
            usb_device_send_ep0_zero (handle, 0);
            /* status phase */
            //  bbu_printf(" usb_state 33333 in usb_ch9SetConfig  is %d\r\n",usb_state);
            /* End of enumeration. Success */
            return;
        } /* Endif */
        else
        {
            usb_device_set_status(handle, USB_STATUS_DEVICE_STATE, USB_STATE_CONFIG);
            usb_device_send_ep0_zero (handle, 0);

        } /* Endif */
    }
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_ch9GetInterface
* Returned Value : None
* Comments       :
*     Chapter 9 GetInterface command
*     See section 9.4.4 (page 190) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_ch9SetInterface
* Returned Value : None
* Comments       :
*     Chapter 9 SetInterface command
*     See section 9.4.10 (page 195) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_ch9SynchFrame
* Returned Value :
* Comments       :
*     Chapter 9 SynchFrame command
*     See section 9.4.11 (page 195) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/
#if 0
static uint8_t ModemComm0SerialStateNotification [MODEM_COMM_NOTIFY_SERIAL_STATE_LENGTH] =
{
    0xa1, 0x20, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00
};
static void set_code_line(uint_16  index, LINE_CODING  line_codeing)
{
    if (index > 4)
    {
        // bbu_printf("index is so large\r\n");
        return ;
    }
    LINE_CODE_SET[index] = line_codeing;

}

static void get_code_line(uint_16  index)
{
    if (index > 4)
    {
        //bbu_printf("index is so large\r\n");
        return ;
    }

    linecoding = LINE_CODE_SET[index];


}
/*FUNCTION*----------------------------------------------------------------   bulk -only protocl*/

static uint16_t Ch9SetLine_INDEX = 100;
static uint8_t SetLine_Time = 0;
static uint8_t USB2_SetLine_Status = 0;
static uint8_t ctrl_out_data_stage_buff[128] = {0};
static uint16_t ControlLineSt[4] = {0, 0, 0, 0};

static void clear_serial_flag(usb_device_handle handle, uint8_t isrNum)
{

    handle = handle;
    if (isrNum == ep_isrNumber)
    {
        debug_serial_begin = 0;


    }
    //else if(isrNum==BULK_ISR_EP)
    //  {
    //   qspy_serial_begin=0;


    //  }
}
static void set_serial_flag(usb_device_handle handle, uint8_t isrNum)
{

    handle = handle;
    if (isrNum == ep_isrNumber)
    {
        debug_serial_begin = 1;


    }
    //  else if(isrNum==BULK_ISR_EP)
    //      {
    //       qspy_serial_begin=1;


    //      }
}

static void ModemCommNotifySerialState(uint_8 ep_num, usb_device_handle handle)
{
#ifdef _DEVICE_DEBUG_
    // bbu_printf("send ModemComm0SerialStateNotification\r\n");
#endif
    usb_device_send_data(handle, ep_num, ModemComm0SerialStateNotification, MODEM_COMM_NOTIFY_SERIAL_STATE_LENGTH);

}
static void ch9Class(  usb_device_handle handle, boolean setup, SETUP_STRUCT_PTR setup_ptr)
{
    /* Body */

    //   USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;

    // usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    uint16_t Control_Line_Status = 0;
    uint16_t old_line_Status = 0;
    uint16_t Control_Line_interface = 100;
    uint8_t   isrNum = 0;
    //if(setup_ptr->INDEX==0)
    isrNum = ep_isrNumber;

    //  int i;
    //bbu_printf("class request is 0x%x\r\n",setup_ptr->REQUEST);
    if (setup)
    {
        switch (setup_ptr->REQUEST)
        {

        case SET_COMM_FEATURE:
            // status phase
            usb_device_send_data(handle, 0, 0, 0);
            return;
        case GET_COMM_FEATURE:
            // status phase
            usb_device_recv_data(handle, 0, 0, 0);
            return;
        case CLEAR_COMM_FEATURE:
            // status phase
            usb_device_send_data(handle, 0, 0, 0);
            return;

        case GET_LINE_CODING:

            if (setup_ptr->LENGTH == 0)
            {
                //bbu_printf("get line coding length is zreo!\r\n");
                usb_device_stall_endpoint(handle, 0, 0);
            }

            else
            {
                get_code_line(setup_ptr->INDEX);
                usb_device_send_data(handle, 0, (uint8_t *)&linecoding, setup_ptr->INDEX);
                // status phase
                usb_device_recv_data(handle, 0, 0, 0);
            }
            return;

        case SET_LINE_CODING:
            if (setup_ptr->LENGTH == 0)
            {
                // bbu_printf("get line coding length is zreo!\r\n");
                usb_device_stall_endpoint(handle, 0, 0);
            }
            else
            {
                Ch9SetLine_INDEX = setup_ptr->INDEX;
                SetLine_Time++;
                // bbu_printf("setup_ptr->LENGTH is %d\r\n",setup_ptr->LENGTH);
                usb_device_recv_data(handle, 0, ctrl_out_data_stage_buff, setup_ptr->LENGTH);

            }
            if (SetLine_Time > 1)
                //  bbu_printf("Error receive con LineCode\r\n");
                USB2_SetLine_Status = 1;



            return;
        case SET_CONTROL_LINE_STATE:

            Control_Line_Status = setup_ptr->VALUE;
            Control_Line_interface = setup_ptr->INDEX;
            old_line_Status = ControlLineSt[Control_Line_interface];
            //bbu_printf("Control_Line_Status=%d,Control_Line_interface is %d\r\n",Control_Line_Status,Control_Line_interface);
            if ((Control_Line_Status != old_line_Status))
            {

                ControlLineSt[Control_Line_interface] = Control_Line_Status;
                if (Control_Line_Status != 0)
                {
                    ModemComm0SerialStateNotification[8] |= (ACM_CTRL_DSR | ACM_CTRL_DCD);
                    ModemCommNotifySerialState(isrNum, handle);
                    set_serial_flag( handle,  isrNum);
                }
                else
                {

                    ModemComm0SerialStateNotification[8] &= ~(ACM_CTRL_DCD | ACM_CTRL_DSR);
                    ModemCommNotifySerialState(isrNum, handle);
                    clear_serial_flag(handle, isrNum);

                }



            }
            // status phase

            usb_device_send_data(handle, 0, 0, 0);
            return;
        default :
            usb_device_stall_endpoint(handle, 0, 0);
            return;
        }
    }
    else
    {
        switch (setup_ptr->REQUEST)

        {
        case SET_LINE_CODING:
            if (1 == USB2_SetLine_Status)
            {

                // for(i=0;i<7;i++)
                //bbu_printf("ctrl_out_data_stage_buff[i] is %d\r\n",ctrl_out_data_stage_buff[i]);
                linecoding.bitrate = (ctrl_out_data_stage_buff[0]) | (ctrl_out_data_stage_buff[1] << 8) |
                                     (ctrl_out_data_stage_buff[2] << 16) | (ctrl_out_data_stage_buff[3] << 24);
                linecoding.format = ctrl_out_data_stage_buff[4];
                linecoding.paritytype = ctrl_out_data_stage_buff[5];
                linecoding.datatype = ctrl_out_data_stage_buff[6];
                SetLine_Time = 0;
                set_code_line(Ch9SetLine_INDEX, linecoding);
                USB2_SetLine_Status = 0;
                // status phase
                usb_device_send_data(handle, 0, 0, 0);

            }

            return;
        }
    }
}

#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_service_ep0
* Returned Value : None
* Comments       :
*     Called upon a completed endpoint 0 (USB 1.1 Chapter 9) transfer
*
*END*--------------------------------------------------------------------*/
static void usb_service_ep0
(
    /* [IN] Handle of the USB device */
    usb_device_handle   handle,

    /* [IN] Is it a setup packet? */
    boolean              setup,

    /* [IN] Direction of the transfer.  Is it transmit? */
    uint_8               direction,

    /* [IN] Pointer to the data buffer */
    uint_8_ptr           buffer,

    /* [IN] Length of the transfer */
    uint_32              length,

    /* [IN] Error, if any */
    uint_8               error
)
{
    /* Body */
    (void)direction;
    (void)buffer;
    (void) length;
    (void)error;
    //   boolean  class_request = FALSE;

    /***********by tongchao*************/
    USB_DEV_STATE_STRUCT_PTR    usb_dev_ptr;
    MAVL_USB_TypeDef_PTR new_dev_ptr;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;

    //  bbu_printf("usb_service_ep0.\r\n");
#if 0
    if (setup)
    {
        usb_device_read_setup_data(handle, 0, (uchar_ptr)&usb_local_setup_packet);
    }
    else if (class_request)
    {
        class_request = FALSE;
        /* Finish your class or vendor request here */
        return;
    } /* Endif */

#elif 1

    if (setup)
    {

        if ((new_dev_ptr->U2XEPSETUPSTAT) & 1)                //by tongchao
            usb_device_read_setup_data (handle, 0, (uchar_ptr)&usb_local_setup_packet);

        else
            return;                                        // no new setup, just return
    }
#endif

    switch (usb_local_setup_packet.REQUESTTYPE & 0x60)
    {

    case 0x00:
        switch (usb_local_setup_packet.REQUEST)
        {

        case 0x0:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nch9GetStatus\r\n\r\n");
#endif
            //ch9GetStatus(handle, setup, &usb_local_setup_packet);
            break;

        case 0x1:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nch9ClearFeature\r\n\r\n");
#endif
            //  ch9ClearFeature(handle, setup, &usb_local_setup_packet);
            break;

        case 0x3:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nch9SetFeature\r\n\r\n");
#endif
            // ch9SetFeature(handle, setup, &usb_local_setup_packet);
            break;

        case 0x5:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nch9SetAddress\r\n\r\n");
#endif
            ch9SetAddress(handle, setup, &usb_local_setup_packet);
            break;

        case 0x6:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nusb_ch9GetDescription\r\n\r\n");
#endif
            ch9GetDescription(handle, setup, &usb_local_setup_packet);
            break;

        case 0x7:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nch9SetDescription\r\n\r\n");
#endif
            //   ch9SetDescription(handle, setup, &usb_local_setup_packet);
            break;

        case 0x8:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nch9GetConfig\r\n\r\n");
#endif
            // ch9GetConfig(handle, setup, &usb_local_setup_packet);
            break;

        case 0x9:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nch9SetConfig\r\n\r\n");
#endif
            ch9SetConfig(handle, setup, &usb_local_setup_packet);
            break;

        case 0xa:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nch9GetInterface\r\n\r\n");
#endif
            //  ch9GetInterface(handle, setup, &usb_local_setup_packet);
            break;

        case 0xb:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nch9SetInterface\r\n\r\n");
#endif
            //    ch9SetInterface(handle, setup, &usb_local_setup_packet);
            break;

        case 0xc:
#ifdef _DEVICE_DEBUG_
            //bbu_printf("\r\nch9SynchFrame\r\n\r\n");
#endif
            // ch9SynchFrame(handle, setup, &usb_local_setup_packet);
            break;

        default:
            usb_device_stall_endpoint(handle, 0, 0);
            break;

        } /* Endswitch */

        break;

    case 0x20:
        /* class specific request */
        //  class_request = TRUE;
#ifdef _DEVICE_DEBUG_
        //bbu_printf("\r\nch9class request\r\n\r\n");
#endif
        /* Queue your own packet for class data here */

        // ch9Class(handle, setup, &usb_local_setup_packet);
        /* For Get Max LUN use any of these responses*/
        // usb_device_stall_endpoint(handle, 0, 0);
        return;

    case 0x40:
        /* vendor specific request */
#ifdef _DEVICE_DEBUG_
        // bbu_printf("\r\nch9Vendor\r\n\r\n");
#endif
        //   ch9Vendor(handle, setup, &usb_local_setup_packet);
        break;

    default:
        usb_device_stall_endpoint(handle, 0, 0);
        break;

    } /* Endswitch */

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : detect_speed
* Returned Value : None
* Comments       :
*     Called when the speed is set
*
*END*--------------------------------------------------------------------*/

void usb_isrEp_service
(
    /* [IN] Handle of the USB device */
    usb_device_handle   handle,

    /* [IN] Is it a setup packet? */
    boolean              setup,

    /* [IN] Direction of the transfer.  Is it transmit? */
    uint_8               direction,

    /* [IN] Pointer to the data buffer */
    uint_8_ptr           buffer,

    /* [IN] Length of the transfer */
    uint_32              length,

    /* [IN] Error, if any */
    uint_8               error



)
{
    /* Body */

    //DEBUGMSG(TTC_DBG_LEVEL,("iso_service_ep1\r\n"));
    /* queue an advance receive at ISO endpoint */
    //   _usb_speaker_device_recv_data(handle, ENDPOINT_SPEAKER,
    //                         ISO_BUFFER,ISO_BUFFER_SIZE);
    //    uint_32 i;
    (void)handle;
    (void)setup;
    (void)direction;
    (void)buffer;
    (void)length;
    (void)error;

    if (direction)
    {

        //ModemCommNotifySerialState(handle);
    }
    return;
} /* Endbody */




extern void uart_char_handler(char c);

static void usb_uart_handle(uint_8_ptr buffer, uint_32 length)
{
    uint_8 c;
    uint_32 i = 0;
    if (length > serBuffer_length)
    {
        bbu_printf("length is beyond usb ser length\r\n");

    }
    else
    {
        for (; i < length; i++)
        {
            c = buffer[i];
            uart_char_handler(c);
        }
    }
}

void usb_bulkEp_service
(
    /* [IN] Handle of the USB device */
    usb_device_handle   handle,

    /* [IN] Is it a setup packet? */
    boolean              setup,

    /* [IN] Direction of the transfer.  Is it transmit? */
    uint_8               direction,

    /* [IN] Pointer to the data buffer */
    uint_8_ptr           buffer,

    /* [IN] Length of the transfer */
    uint_32              length,

    /* [IN] Error, if any */
    uint_8               error



)

{

    (void)handle;
    (void)setup;
    (void)error;

    if (direction)
    {

        debug_serial_begin = 1;

        count_in = TRUE;

        //                bbu_printf("buffer is %d\r\n",*buffer);

    }
    else
    {

        usb_uart_handle(buffer, length);



        memset(buffer_out, 0, serBuffer_length);
        usb_device_recv_data(handle, ep_number, buffer_out, serBuffer_length);
    }





}












//FUNCTION*----------------------------------------------------------------
/*
* Function Name  : usb_reset_ep0
* Returned Value : None
* Comments       :
*     Called upon a bus reset event.  Initialises the control endpoint.
*
*END*--------------------------------------------------------------------*/
static void usb_reset_ep0
(
    /* [IN] Handle of the USB device */
    usb_device_handle   handle,

    /* [IN] Unused */
    boolean              setup,

    /* [IN] Unused */
    uint_8               direction,

    /* [IN] Unused */
    uint_8_ptr           buffer,

    /* [IN] Unused */
    uint_32              length,

    /* [IN] Error, if any */
    uint_8               error
)
{
    /* Body */
    //uint_32 state;

    (void)setup;
    (void)direction;
    (void)buffer;
    (void)length;
    (void)error;


    ///  BBU_putstr("usb_reset_ep0.\r\n");

    /* on a reset always ensure all transfers are cancelled on all EP*/
    // usb_device_cancel_transfer(handle, 0, USB_RECV);
    // usb_device_cancel_transfer(handle, 0, USB_SEND);
    // usb_device_cancel_transfer(handle, 1, USB_RECV);
    // usb_device_cancel_transfer(handle, 1, USB_SEND);

    // usbusb_get_status(appusb_handle, USBusb_STATE, &state);
#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_reset_ep0.\r\n");
#endif
    usb_device_init_endpoint(handle, 0, usb_DevDesc[DEV_DESC_MAX_PACKET_SIZE], 0,
                             USB_CONTROL_ENDPOINT, 0);
    usb_device_init_endpoint(handle, 0, usb_DevDesc[DEV_DESC_MAX_PACKET_SIZE], 1,
                             USB_CONTROL_ENDPOINT, 0);


    // if (state >= B_IDLE)
    //{
    // usb_usb_set_status(app_usb_handle, USB_usb_B_BUS_REQ, FALSE);
    //} /* Endif */

    return;
} /* EndBody */

static void usb_cancel_allEndpoint_transfer
(
    /* [IN] Handle of the USB device */
    usb_device_handle   handle,

    /* [IN] Is it a setup packet? */
    boolean              setup,

    /* [IN] Direction of the transfer.  Is it transmit? */
    uint_8               direction,

    /* [IN] Pointer to the data buffer */
    uint_8_ptr           buffer,

    /* [IN] Length of the transfer */
    uint_32              length,

    /* [IN] Error, if any */
    uint_8               error



)
{
    (void)setup;
    (void)handle;
    (void)direction;
    (void)buffer;
    (void)length;
    (void)error;

    /* Inform the application so that it can cancel all previously queued transfers */
    usb_device_cancel_transfer(handle, 0, USB_RECV);
    usb_device_cancel_transfer(handle, 0, USB_SEND);
    usb_device_cancel_transfer(handle, ep_isrNumber, USB_SEND);





    usb_device_cancel_transfer(handle, ep_number, USB_RECV);
    usb_device_cancel_transfer(handle, ep_number, USB_SEND);

}



/**
    epnum: 0-15
    mode:  0 for high speed, 1 for full speed, default is 0
    media: 0: DDR, 1: SD card
*/
extern volatile U32 DISK_MEDIA ; //0: DDR, 1: SD card              = 0;
/*
static void usb_isr(void *usb)
{
   usb=usb;
if ( usb_ISR_Handle )
            (usb_ISR_Handle)();
}
extern void ISR_Connect(uint32_t int_num, void (*ISR_Handle)(void*), void *ISR_Arg);
extern void INT_Enable(uint32_t int_num, uint32_t irq0_fiq1, uint32_t priority);

static void usb_isr_enable(void)
{
   ISR_Connect(IRQ_MMP_USB,usb_isr,NULL);
   INT_Enable(IRQ_MMP_USB,  0,  0xf);//enable USB interrupt
}
*/
/*static void usb_isr_disable(void)

{

    ISR_Disconnect(IRQ_MMP_USB);
   INT_Disable(IRQ_MMP_USB);//disable USB interrupt
}*/




uint_8 usb_serial_init (U32 epnum);



extern uint_8    usb_success_bit;
extern  uint_8 init_bit;
extern void    *NONE_Cache_ADDR;
void   usb_serial_malloc(void)
{


    buffer_in = (uint_8 *)   (NONE_Cache_ADDR);
    buffer_out = (uint_8 *)(NONE_Cache_ADDR + 8);
    buffer_in[0] = '\n';

}

void usb_free_serialBuff(void)
{

    debug_serial_begin = 0;

}
volatile  uchar FORCE_FULL_SPEED = FALSE;

uint_8 usb_serial_init (U32 epnum)

{
    /* Body */

    uint_8               error;
    //uint_32              state=0;

    //FORCE_FULL_SPEED = TRUE;
    FORCE_FULL_SPEED = FALSE;

    /********************added by cosmo*ep_num**********************/
    ep_number = epnum;



    /***************************************************************/
    //    bbu_printf("Initialize USB serial device. \r\n");


    if (ep_number == 0)
    {
        ep_number = 1;
    }

    /********************added by cosmo*ep_num**********************/
    // bbu_printf("The serial endpoints number :%d ",ep_number);
    // bbu_putchar(ep_number);
    //  bbu_printf(" \r\n");
    /***************************************************************/

    USB_malloc_init();
    //usb_DiskBufInit(); //Init buffer for disk application
    usb_serial_malloc();
    usb_ISR_Handle = (void (*)())usb_dci_vusb20_isr;

    /* Initialize the USB device interface */
    error = usb_device_init(0, &usb_dev_handle, 4);

    if (error != USB_OK)
    {
        //   bbu_printf("nUSB Device Initialization failed.\r\n ");

    } /* Endif */

#ifndef SMALL_CODE_SIZE

    // error = usb_device_register_service(usb_dev_handle, USB_SERVICE_EP0,service_ep0);
    error = usb_device_register_service(usb_dev_handle, USB_SERVICE_EP0, usb_service_ep0);
    if (error != USB_OK)
    {
        //   bbu_printf("USB Device Service Registration failed. \r\n");

    } /* Endif */

    /* error =  usb_device_register_service(usb_dev_handle, ep_number, usb_service_bulk_endpoint);
      if (error != USB_OK) {
         bbu_printf("nUSB Device Service Registration failed. \r\n");

      } *//* Endif */

    error =  usb_device_register_service(usb_dev_handle, ep_number, usb_bulkEp_service);
    if (error != USB_OK)
    {
        //   bbu_printf("nUSB Device Service Registration failed. \r\n");

    } /* Endif */
    // error =  usb_device_register_service(usb_dev_handle, 2, usb_bulkEp_service);

    error =  usb_device_register_service(usb_dev_handle, ep_isrNumber, usb_isrEp_service);
    if (error != USB_OK)
    {
        //   bbu_printf("nUSB Device Service Registration failed. \r\n");

    } /* Endif */


    error = usb_device_register_service(usb_dev_handle, USB_SERVICE_BUS_RESET, usb_reset_ep0);
    if (error != USB_OK)
    {
        //  bbu_printf("USB Device Service Registration failed. \r\n");

    } /* Endif */

    error = usb_device_register_service(usb_dev_handle, USB_SERVICE_RESET_ENPOINT, usb_cancel_allEndpoint_transfer);
    if (error != USB_OK)
    {
        //  bbu_printf("USB Device Service Registration failed. \r\n");

    } /* Endif */

#if 0
    error =  usb_device_register_service(usb_dev_handle, BULK_IN_EP, usb_bulkEp_service);
    if (error != USB_OK)
    {
        bbu_printf("nUSB Device Service Registration failed. \r\n");

    } /* Endif */
#endif

#endif //SMALL_CODE_SIZE   

    ///   usb_usb_get_status(app_usb_handle, USB_usb_STATE, &state);


#ifndef SMALL_CODE_SIZE
    /* we must register for bus suspend interrupt for B or A peripheral devices */

#endif //SMALL_CODE_SIZE   

    // usb_isr_enable();


    init_bit = error;
    return error;

} /* Endbody */
#if 0
int usb_qspy_put_char(int u8c)
{

#ifdef USB_UART
    if (!qspy_serial_begin)
        return u8c;

    while (!count_in2);
    count_in2 = FALSE;
    *buffer_in2 = u8c;
    usb_device_handle handle = (usb_device_handle)getUSBhandle();
    usb_device_send_data(handle, BULK_IN_EP, buffer_in2, 1);


    return u8c;

#endif




}
#endif

uint_8 readUsbInitFlag(void)
{

    usb_dci_vusb20_isr();

    return debug_serial_begin;


}

int usb_debug_put_char(int u8c)
{
    int i = 0, timeout = 0x40000;

#ifdef USB_UART
    if (!debug_serial_begin)
    {
        //  usb_qspy_put_char(u8c);

        init_log[initLength] = u8c;
        initLength++;
        return u8c;
    }
    // while(!count_in);

    while (i < timeout)
    {
        usb_dci_vusb20_isr();
        if (count_in == TRUE)
            break;
        else
        {
            i++;
            udelay(1);
        }
    }

    count_in = FALSE;
    *buffer_in = u8c;
    //bbu_printf("u8c is %d\r\n",u8c);
    usb_device_handle handle = (usb_device_handle)getUSBhandle();
    usb_device_send_data(handle, ep_number, buffer_in, 1);




    //    usb_qspy_put_char(u8c);

    return u8c;

#endif




}














