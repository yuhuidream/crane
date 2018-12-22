// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2002  -  All rights reserved
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
*  description:   USB Mass Storage driver header file
*
*  notes:
*
*
*$PVCS Comment: USB 2.0 Mass storage driver header file
*
*$END Comment.
*$Log:   M:/Data Storage/Reference Design/Firmware/mDes/src/USBTask.h_v  $
*
*
*    Rev 1.0   April 5 2003 01:31:20   kc
* Initial revision.
*
*-----------------------------------------------------------------------------*/
#ifndef _USB_H_
#define _USB_H_

#include "define.h"

/*
 * Descriptor types ... USB 2.0 spec table 9.5
 */
#define C0_8618

#define USB_DT_DEVICE                         0x01
#define USB_DT_CONFIG                         0x02
#define USB_DT_STRING                         0x03
#define USB_DT_INTERFACE                      0x04
#define USB_DT_ENDPOINT                0x05
#define USB_DT_DEVICE_QUALIFIER     0x06
#define USB_DT_OTHER_SPEED_CONFIG   0x07
#define USB_DT_INTERFACE_POWER      0x08
/* these are from a minor usb 2.0 revision (ECN) */
#define USB_DT_OTG                               0x09
#define USB_DT_DEBUG                          0x0a
#define USB_DT_INTERFACE_ASSOCIATION    0x0b

/* conventional codes for class-specific descriptors */
#define USB_DT_CS_DEVICE        0x21
#define USB_DT_CS_CONFIG        0x22
#define USB_DT_CS_STRING        0x23
#define USB_DT_CS_INTERFACE 0x24
#define USB_DT_CS_ENDPOINT  0x25

/*
 * Device and/or Interface Class codes
 * as found in bDeviceClass or bInterfaceClass
 * and defined by www.usb.org documents
 */
#define USB_CLASS_PER_INTERFACE 0x00    /* for DeviceClass */
#define USB_CLASS_AUDIO         0x01
#define USB_CLASS_COMM                 0x02
#define USB_CLASS_HID                  0x03
#define USB_CLASS_PHYSICAL             0x05
#define USB_CLASS_STILL_IMAGE       0x06
#define USB_CLASS_PRINTER              0x07
#define USB_CLASS_MASS_STORAGE  0x08
#define USB_CLASS_HUB                  0x09
#define USB_CLASS_CDC_DATA      0x0a
#define USB_CLASS_CSCID         0x0b    /* chip+ smart card */
#define USB_CLASS_CONTENT_SEC       0x0d    /* content security */
#define USB_CLASS_VIDEO         0x0e
#define USB_CLASS_APP_SPEC             0xfe
#define USB_CLASS_VENDOR_SPEC       0xff

/* These IDs have been donated by NetChip Technologies for open source
 * development.  DO NOT REUSE THESE IDs with any other driver!!
 * Ever!!  Instead: allocate your own, using normal USB-IF procedures.
 */
#define DRIVER_VENDOR_ID    0x05A3  // Marvell
#define DRIVER_PRODUCT_ID   0x8380
#define DRIVER_RELEASE_ID   0x0100  //device driver release number: 0.1.0.0

/***************************************************************************
 * USB String Descriptors
 ***************************************************************************/
#define STRING_MANUFACTURER 1
#define STRING_PRODUCT             2
#define STRING_SERIAL              3
/******modified by cosmo*****************/
#define STRING_CONFIG              4
//#define STRING_CONFIG            0
#define STRING_INTERFACE           5

/* There is only one configuration. */
#define CONFIG_VALUE        1

/* from config descriptor bmAttributes */
#define USB_CONFIG_ATT_ONE      (1 << 7)    /* must be set */
#define USB_CONFIG_ATT_SELFPOWER    (1 << 6)    /* self powered */
#define USB_CONFIG_ATT_WAKEUP   (1 << 5)    /* can wakeup */

/***************************************************************************
 * USB Protocol Data
 ***************************************************************************/
/* USB protocol value = the transport method */
#define USB_PR_CBI  0x00        // Control/Bulk/Interrupt
#define USB_PR_CB   0x01        // Control/Bulk w/o interrupt
#define USB_PR_BULK 0x50        // Bulk-only

/* USB subclass value = the protocol encapsulation */
#define USB_SC_RBC  0x01        // Reduced Block Commands (flash)
#define USB_SC_8020 0x02        // SFF-8020i, MMC-2, ATAPI (CD-ROM)
#define USB_SC_QIC  0x03        // QIC-157 (tape)
#define USB_SC_UFI  0x04        // UFI (floppy)
#define USB_SC_8070 0x05        // SFF-8070i (removable)
#define USB_SC_SCSI 0x06        // Transparent SCSI

/***************************************************************************
 * USB IP Core Data
 ***************************************************************************/
#define USB_MAX_CTRL_PAYLOAD   (64)
/********modified by cosmo*************/
#define SUPPORT_ENDPOINT_NUM  (0x10)     /* BBIC2 USB function only supports 4 endpoints. */
//#define SUPPORT_ENDPOINT_NUM  (4)     /* BBIC2 USB function only supports 4 endpoints. */

#define  MAX_EP_TR_DESCRS       (32)
#define  MAX_XDS_FOR_TR_CALLS  (32)
/********modified by cosmo*************/
#define  USB_MAX_ENDPOINTS      (0x10)     /* BBIC2 USB function only supports 4 endpoints. */
//#define  USB_MAX_ENDPOINTS        (4)     /* BBIC2 USB function only supports 4 endpoints. */


#define MIN(a,b)         ((a) < (b) ? (a) : (b))

/* The ARC is little-endian, just like USB */
#define USB_uint_16_low(x)             ((x) & 0xFF)
#define USB_uint_16_high(x)            (((x) >> 8) & 0xFF)

//usbd.h
#define  USB_DEBOUNCE_DELAY                  (50)
#define  USB_RESET_RECOVERY_DELAY       (10)
#define  USB_RESET_DELAY            (4) // (6)  //unit: ms

/* Error codes */
#define  USB_OK                                                         (0x00)
#define  USBERR_ALLOC                                             (0x01)
#define  USBERR_BAD_STATUS                                  (0x02)
#define  USBERR_CLOSED_SERVICE                           (0x03)
#define  USBERR_OPEN_SERVICE                               (0x04)
#define  USBERR_INVALID_NUM_OF_ENDPOINTS      (0x05)
#define  USBERR_ENDPOINT_DISABLED                     (0x06)
#define  USBERR_TRANSFER_IN_PROGRESS               (0x07)
#define  USBERR_ENDPOINT_STALLED                       (0x08)
#define  USBERR_UNKNOWN_TRANSFER_STATUS      (0x09)
#define  USBERR_ALLOC_STATE                                 (0x0A)
#define  USBERR_ALLOC_TRANSFER_DESCRIPTORS   (0x0B)
#define  USBERR_DRIVER_INSTALL_FAILED               (0x0C)
#define  USBERR_DRIVER_NOT_INSTALLED                (0x0D)
#define  USBERR_INSTALL_ISR                                   (0x0E)
#define  USBERR_INVALID_DEVICE_NUM                    (0x0F)
#define  USBERR_ALLOC_SERVICE                               (0x1F)
#define  USBERR_INIT_FAILED                                    (0x2F)
#define  USBERR_SHUTDOWN                                       (0x3F)
#define  USBERR_INVALID_PIPE_HANDLE                   (0x4F)
#define  USBERR_ALLOC_PIPE_DESCR                         (0x5F)
#define  USBERR_OPEN_PIPE_FAILED                         (0x6F)
#define  USBERR_INIT_DATA                                       (0x7F)
#define  USBERR_SRP_REQ_INVALID_STATE               (0x8F)
#define  USBERR_MAX_EPS_NOT_SUPPORTED             (0x9F)
#define  USBERR_ALLOC_BUFF                                     (0xA0)
#define  USBERR_TX_FAILED                                       (0xA1)
#define  USBERR_RX_FAILED                                       (0xA2)
#define  USBERR_TR_CANCEL_FAILED                         (0xA3)
#define  USBERR_EP_INIT_FAILED                              (0xA4)
#define  USBERR_EP_DEINIT_FAILED                          (0xA5)
#define  USBERR_TR_FAILED                                       (0xA6)
#define  USBERR_BANDWIDTH_ALLOC_FAILED           (0xA7)
/* Error Codes for lower-layer */
#define  USBERR_ALLOC_EP_QUEUE_HEAD                 (0xA8)
#define  USBERR_ALLOC_TR                                        (0xA9)
#define  USBERR_ALLOC_DTD_BASE                            (0xAA)
#define  USBERR_CLASS_DRIVER_INSTALL                 (0xAB)

/* USB device state definitions */
#define  USB_STATE_UNKNOWN                   (0xff)
#define  USB_STATE_ATTACH                       (0x04)
#define  USB_STATE_POWERED                    (0x03)
#define  USB_STATE_DEFAULT                     (0x02)
#define  USB_STATE_ADDRESS                     (0x01)
#define  USB_STATE_CONFIG                       (0x00)
#define  USB_STATE_SUSPEND                     (0x80)
#define  USB_STATE_AUTH                           (0x40)

#define  USB_SELF_POWERED                      (0x01)
#define  USB_REMOTE_WAKEUP                   (0x02)

/* Bus Control values */
#define  USB_NO_OPERATION                       (0x00)
#define  USB_ASSERT_BUS_RESET                (0x01)
#define  USB_DEASSERT_BUS_RESET            (0x02)
#define  USB_ASSERT_RESUME                     (0x03)
#define  USB_DEASSERT_RESUME                 (0x04)
#define  USB_SUSPEND_SOF                         (0x05)
#define  USB_RESUME_SOF                           (0x06)

/* possible values of XD->bStatus */
#define  USB_STATUS_IDLE                                    (0)
#define  USB_STATUS_TRANSFER_ACCEPTED         (1)
#define  USB_STATUS_TRANSFER_PENDING           (2)
#define  USB_STATUS_TRANSFER_IN_PROGRESS   (3)
#define  USB_STATUS_ERROR                                  (4)
#define  USB_STATUS_DISABLED                            (5)
#define  USB_STATUS_STALLED                              (6)
#define  USB_STATUS_TRANSFER_QUEUED             (7)

#define  USB_RECV                            (0)
#define  USB_SEND                            (1)

#define  USB_HOST_DONT_ZERO_TERMINATE       (0x1)
#define  USB_DEVICE_DONT_ZERO_TERMINATE    (0x1)
#define  USB_DEVICE_DONT_RESET_TOGGLE        (0x2)

#define  USB_SPEED_FULL                      (0)
#define  USB_SPEED_LOW                       (1)
#define  USB_SPEED_HIGH                      (2)

/* labels for each of the USB endpoints */
#define EP0     (0)     /* the control path */
#define EP1     (1)     /* the data path in & out */
#define EP2     (2)     /* the data path in */
#define EP3     (3)     /* the data path out */

#define  BOT        1       /* Bulk-Only Transport (BOT) */
#define  CBIT       2       /*Control/Bulk/Interrupt Transport (CBIT and CBT) */

#define APP_CONTROL_MAX_PKT_SIZE            (64)
#define DEV_DESC_MAX_PACKET_SIZE            (7)
#define FS_MAX_PACKET_SIZE                         (64)
#define HS_MAX_PACKET_SIZE                          (512)//  //This can be modified later, it may be 1~1024.
#define CFG_DESC_EP2_OUT_MAX_PACKET_SIZE (22)
#define CFG_DESC_EP3_IN_MAX_PACKET_SIZE   (29)
#define USB_TEST_MODE_TEST_PACKET_LENGTH (53)
#define CONFIG_DESC_NUM_INTERFACES            (4)

/* This must be counted manually and updated with the descriptor */
/* 1*Config(9) + 1*Interface(9) + 2*Endpoint(7) = 32 bytes */
#define CONFIG_DESC_SIZE                    (32)

/* Endpoint types */
#define  USB_CONTROL_ENDPOINT             (0)
#define  USB_ISOCHRONOUS_ENDPOINT     (1)
#define  USB_BULK_ENDPOINT                    (2)
#define  USB_INTERRUPT_ENDPOINT          (3)

/* Informational Request/Set Types */
#define  USB_STATUS_DEVICE_STATE          (0x01)
#define  USB_STATUS_INTERFACE             (0x02)
#define  USB_STATUS_ADDRESS               (0x03)
#define  USB_STATUS_CURRENT_CONFIG        (0x04)
#define  USB_STATUS_SOF_COUNT             (0x05)
#define  USB_STATUS_DEVICE                (0x06)
#define  USB_STATUS_TEST_MODE             (0x07)
#define  USB_STATUS_ENDPOINT              (0x10)
#define  USB_STATUS_ENDPOINT_NUMBER_MASK  (0x0F)

#define  USB_TEST_MODE_TEST_PACKET        (0x0400)

/* VUSB2.0 DCI specific defines */

/* VUSBHS specific defines */
#define  VUSBHS_MAX_PORTS                          (8)
#define  EHCI_CAP_LEN_MASK                         (0x000000FF)

/* Command Register Bit Masks */
#define  EHCI_CMD_RUN_STOP                         (0x00000001)
#define  EHCI_CMD_CTRL_RESET                       (0x00000002)
#define  EHCI_CMD_SETUP_TRIPWIRE            (0x00002000)
#define  EHCI_CMD_ADD_TRIPWIRE                  (0x00004000)

/* Mode Register Bit Masks */
#define  VUSBHS_MODE_CTRL_MODE_IDLE                (0x00000000)
#define  VUSBHS_MODE_CTRL_MODE_DEV                 (0x00000002)
#define  VUSBHS_MODE_CTRL_MODE_HOST                (0x00000003)
#define  VUSBHS_MODE_BIG_ENDIAN                    (0x00000004)
#define  VUSBHS_MODE_SLOM                               (0x00000008)

/* Interrupt Status Register Masks */
#define  EHCI_STS_INT                              (0x00000001)
#define  EHCI_STS_ERR                              (0x00000002)
#define  EHCI_STS_PORT_CHANGE                      (0x00000004)
#define  EHCI_STS_RESET                            (0x00000040)
#define  EHCI_STS_SOF                              (0x00000080)
#define  EHCI_STS_NAK                              (0x00010000)
#define  EHCI_STS_SUSPEND                          (0x00000100)

/* Endpoint Queue Head Bit Masks */
#define  VUSB_EP_QUEUE_HEAD_IOS                    (0x00008000)
#define  VUSB_EP_QUEUE_HEAD_IOC                    (0x00008000)
#define  VUSB_EP_QUEUE_HEAD_INT                    (0x00000100)
#define  VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE         (0x00000001)
#define  VUSB_EP_QUEUE_HEAD_MAX_PKT_LEN_POS        (16)
#define  VUSB_EP_QUEUE_HEAD_ZERO_LEN_TER_SEL       (0x20000000)
#define  VUSB_EP_QUEUE_HEAD_MULT_POS               (30)
#define  VUSB_EP_MAX_LENGTH_TRANSFER               (0x4000) //The maximum recommended transfer size for one dTD is 16K, for 5 pages locate.

#define  VUSB_EP_QUEUE_HEAD_STATUS_ACTIVE          (0x00000080)

#define  VUSBHS_TD_NEXT_TERMINATE                  (0x00000001)
#define  VUSBHS_TD_IOC                             (0x00008000)
#define  VUSBHS_TD_STATUS_ACTIVE                   (0x00000080)
#define  VUSBHS_TD_RESERVED_FIELDS                 (0x00007F00)
#define  VUSBHS_TD_ERROR_MASK                      (0x68)
#define  VUSBHS_TD_ADDR_MASK                       (0xFFFFFFE0)
#define  VUSBHS_TD_LENGTH_BIT_POS                  (16)

#define  EHCI_EPCTRL_TX_DATA_TOGGLE_RST            (0x00400000)
#define  EHCI_EPCTRL_TX_EP_STALL                   (0x00010000)
#define  EHCI_EPCTRL_RX_EP_STALL                   (0x00000001)
#define  EHCI_EPCTRL_RX_DATA_TOGGLE_RST            (0x00000040)
#define  EHCI_EPCTRL_RX_ENABLE                     (0x00000080)
#define  EHCI_EPCTRL_TX_ENABLE                     (0x00800000)
#define  EHCI_EPCTRL_CONTROL                       (0x00000000)
#define  EHCI_EPCTRL_ISOCHRONOUS                   (0x00040000)
#define  EHCI_EPCTRL_BULK                          (0x00080000)
#define  EHCI_EPCTRL_INT                           (0x000C0000)
#define  EHCI_EPCTRL_TX_TYPE                       (0x000C0000)
#define  EHCI_EPCTRL_RX_TYPE                       (0x0000000C)
#define  EHCI_EPCTRL_DATA_TOGGLE_INHIBIT           (0x00000020)
#define  EHCI_EPCTRL_TX_EP_TYPE_SHIFT              (18)
#define  EHCI_EPCTRL_RX_EP_TYPE_SHIFT              (2)

#define  EHCI_PORTSCX_PORT_POWER                   (0x00001000)
#define  EHCI_PORTSCX_LINE_STATUS_BITS             (0x00000C00)
#define  EHCI_PORTSCX_LINE_STATUS_SE0              (0x00000000)
#define  EHCI_PORTSCX_LINE_STATUS_KSTATE           (0x00000400)
#define  EHCI_PORTSCX_LINE_STATUS_JSTATE           (0x00000800)
#define  EHCI_PORTSCX_PORT_HIGH_SPEED              (0x00000200)
#define  EHCI_PORTSCX_PORT_RESET                   (0x00000100)
#define  EHCI_PORTSCX_PORT_SUSPEND                 (0x00000080)
#define  EHCI_PORTSCX_PORT_FORCE_RESUME            (0x00000040)
#define  EHCI_PORTSCX_PORT_EN_DIS_CHANGE           (0x00000008)
#define  EHCI_PORTSCX_PORT_ENABLE                  (0x00000004)
#define  EHCI_PORTSCX_CONNECT_STATUS_CHANGE        (0x00000002)
#define  EHCI_PORTSCX_CURRENT_CONNECT_STATUS       (0x00000001)

#define  VUSBHS_PORTSCX_PORT_SPEED_FULL            (0x00000000)
#define  VUSBHS_PORTSCX_PORT_SPEED_LOW             (0x04000000)
#define  VUSBHS_PORTSCX_PORT_SPEED_HIGH            (0x08000000)
#define  VUSBHS_SPEED_MASK                         (0x0C000000)
#define  VUSBHS_SPEED_BIT_POS                      (26)

#define  EHCI_PORTSCX_W1C_BITS                     (0x2A)

#define  VUSB_EP_TR_PACKET_SIZE                    (0x7FFF0000)

#define  VUSBHS_FRINDEX_MS_MASK                    (0xFFFFFFF8)
#define  VUSBHS_ADDRESS_BIT_SHIFT                  (25)

#define  VUSB20_MAX_ENDPTS_SUPPORTED               (0x1F)

#define  VUSB20_HOST_INTR_EN_BITS                  (0x37)

/* Interrupt enable bit masks */
#define  EHCI_IER_NAK                            (0x00010000)
#define  EHCI_IER_SUSPEND                          (0x00000100)
#define  EHCI_IER_SOF_RECEIVE                      (0x00000080)
#define  EHCI_IER_RESET                            (0x00000040)
#define  EHCI_IER_ASYNCH_ADVANCE                   (0x00000020)
#define  EHCI_IER_HOST_SYS_ERROR                   (0x00000010)
#define  EHCI_IER_FRAME_LIST_ROLLOVER              (0x00000008)
#define  EHCI_IER_PORT_CHANGE                      (0x00000004)
#define  EHCI_IER_USB_ERROR                        (0x00000002)
#define  EHCI_IER_USB_INTERRUPT                    (0x00000001)



#define  INIT_PHY_PLL_CTL           0xabc26eeb  //0x2A01Aeeb    //768
#define  INIT_PHY_TX_CTL            0x41610fc3  //0x6107c3  //0x137C3           //0x117C1       //0x107C3      //1 0000 1111 1100 0011
#define  INIT_PHY_RX_CTL            0xe31c8271  //0x6d1c82a3 //0x6D1C82A1       //0x7D5D0185   //111 1101 0101 1101 0000 0010 1000 0101
#define  INIT_PHY_IVREF_CTL         0x0000007e  //0x43a         //0x635        //110 0011 0101
#define  INIT_PHY_REG_TEST0         0x10c01800  //0xC09A21     //1100 0000 1001 1010 0000 0000 
#define  INIT_PHY_REG_TEST1         0xde000000  //0x00000000   //0000 0000 0000 0000 0000 0000 0000 0000 
#define  INIT_PHY_REG_TEST2         0x00000000  //0x0 
#define  INIT_PHY_RESERVE           0x0
#define  INIT_USB_INT               0x0
#define  INIT_DBG_CTL               0x9



//@Hongji
// Bit descriptions
//#define CARD_USB_CMD              32'h
#define REG_BIT_RST               1
#define REG_BIT_RS                0

//#define CARD_USB_CTL              32'h
#define REG_UTMI_CERR_CNT         27
#define REG_UTMI_SELECT           8
#define REG_OPMODE                6  //7:6           
#define REG_TERM_SELECT           5
#define REG_XCVR_SELECT           4
#define UDU2_TEST_MODE            2  //3:2        
#define REG_BIT_PU_PLL            1
#define REG_BIT_PU                0

//#define CARD_PHY_REG_TEST1        32'h
#define REG_TEST_HS_HDL_SYNC      31
#define REG_TEST_FS_HDL_SYNC      30
#define REG_TEST_DIG_LPBK         20
#define REG_TEST_SKIP             17 //19:17           
#define REG_TEST_FLAG             16
#define REG_TEST_DONE             15
#define REG_TEST_LENGTH           13 //14:13           
#define REG_TEST_TX_PATTERN        5 //12:5           
#define REG_TEST_MODE              3 //4:3           
#define REG_TEST_LPBK_EN           2
#define REG_TEST_BYPASS            1
#define REG_TEST_EN                0

//#define CARD_USB_INT              32'h
#define REG_LINE_INT              3
#define REG_LINE_INT_EN           1
#define REG_CORE_INT              0




typedef  struct
{
    U32 MAX_PKT_LENGTH;      /* Bits 16..26 Bit 15 is Interrupt On Setup  */

    U32 CURR_DTD_PTR;        /* Current dTD Pointer */
    U32 NEXT_DTD_PTR;        /* Next dTD Pointer */
    U32 SIZE_IOC_INT_STS;    /* Total bytes (16..30), IOC (15),  INT (8), STS (0-7) */

    U32 BUFF_PTR0;           /* Buffer pointer Page 0 (12-31) */
    U32 BUFF_PTR1;           /* Buffer pointer Page 1 (12-31) */
    U32 BUFF_PTR2;           /* Buffer pointer Page 2 (12-31) */
    U32 BUFF_PTR3;           /* Buffer pointer Page 3 (12-31) */
    U32 BUFF_PTR4;           /* Buffer pointer Page 4 (12-31) */
    U32 RESERVED1;
    U8 SETUP_BUFFER[8];      /* 8 bytes of setup data that follows the Setup PID */

    U8 RESERVED2[16];        // 16 filler bytes to align on 64 byte boundries
}  VUSB20_EP_QUEUE_HEAD_STRUCT, *VUSB20_EP_QUEUE_HEAD_STRUCT_PTR;

typedef  struct
{
    pointer PRIVATE;
    void (*FREE) (pointer);
    pointer XD_FOR_THIS_DTD;
}  SCRATCH_STRUCT, *SCRATCH_STRUCT_PTR;

typedef  struct ep_tr_struct
{
    U32 NEXT_TR_ELEM_PTR;    /*  Memory address of next  dTD to be processed (5-31)
                                 **  and the T (bit 0) indicating pointer validity */

    U32 SIZE_IOC_STS;        /* total bytes (16-30), IOC (15), Status (0-7) */
    U32 BUFF_PTR0;           /* Buffer pointer Page 0 */
    U32 BUFF_PTR1;           /* Buffer pointer Page 1 */
    U32 BUFF_PTR2;           /* Buffer pointer Page 2 */
    U32 BUFF_PTR3;           /* Buffer pointer Page 3 */
    U32 BUFF_PTR4;           /* Buffer pointer Page 4 */
    SCRATCH_STRUCT_PTR SCRATCH_PTR;
}  VUSB20_EP_TR_STRUCT, *VUSB20_EP_TR_STRUCT_PTR;

/***************************************
**
** Data structures
**
****************************************/

typedef  struct xd_struct
{
    U8 EP_NUM;               /* Endpoint number */
    U8 BDIRECTION;           /* Direction : Send/Receive */
    U8 EP_TYPE;              /* Type of the endpoint: Ctrl, Isoch, Bulk, int */
    U8 BSTATUS;              /* Current transfer status */
    LP_U8 WSTARTADDRESS;    /* Address of first byte */
    U32 WTOTALLENGTH;        /* Number of bytes to send/recv */
    U32 WSOFAR;              /* Number of bytes recv'd so far */
    U16 WMAXPACKETSIZE;      /* Max Packet size */
    BOOLEAN DONT_ZERO_TERMINATE;
    SCRATCH_STRUCT_PTR SCRATCH_PTR;
}  XD_STRUCT, *XD_STRUCT_PTR;

/* The USB Device State Structure */
typedef  struct
{
    BOOLEAN BUS_RESETTING;      /* Device is being reset  */
    BOOLEAN TRANSFER_PENDING;   /* Transfer pending ? */
    pointer CALLBACK_STRUCT_PTR;
    //SERVICE_STRUCT_PTR   SERVICE_HEAD_PTR;       /* Head struct  address of  registered services  */

    XD_STRUCT_PTR TEMP_XD_PTR;  /* Temp xd for ep init */
    XD_STRUCT_PTR XD_BASE;
    XD_STRUCT_PTR XD_HEAD;      /* Head Transaction descriptors */
    XD_STRUCT_PTR XD_TAIL;      /* Tail Transaction descriptors */
    XD_STRUCT_PTR PENDING_XD_PTR;       /* pending transfer */

    U32 XD_ENTRIES;
    U32 txtran;
    U32 rxtran;
    U32 txcomplete;
    U32 rxcomplete;

    VUSB20_EP_QUEUE_HEAD_STRUCT_PTR EP_QUEUE_HEAD_PTR;  /* Endpoint Queue head */


    VUSB20_EP_TR_STRUCT_PTR DTD_BASE_PTR;       /* Aligned transfer descriptor pool address */
    VUSB20_EP_TR_STRUCT_PTR DTD_HEAD;
    VUSB20_EP_TR_STRUCT_PTR DTD_TAIL;
    VUSB20_EP_TR_STRUCT_PTR EP_DTD_HEADS[USB_MAX_ENDPOINTS * 2];
    VUSB20_EP_TR_STRUCT_PTR EP_DTD_TAILS[USB_MAX_ENDPOINTS * 2];

    SCRATCH_STRUCT_PTR XD_SCRATCH_STRUCT_BASE;
    SCRATCH_STRUCT_PTR TD_SCRATCH_STRUCT_BASE;

    /* These fields are kept only for USB_shutdown() */
    U16 USB_STATE;
    U16 USB_DEVICE_STATE;
    U16 USB_SOF_COUNT;
    U16 DTD_ENTRIES;
    U16 ERRORS;
    U16 USB_DEV_STATE_B4_SUSPEND;
    U16 dTDErrorCount;
    U8 SPEED;                /* Low Speed, High Speed,Full Speed */
    U8 MAX_ENDPOINTS;        /* Max endpoints supported by this device */
    U8 USB_CURR_CONFIG;
    U8 DEVICE_ADDRESS;
}  USB_DEV_STATE_STRUCT, *USB_DEV_STATE_STRUCT_PTR;

/* USB 1.1 Setup Packet */
typedef  struct setup_struct
{
    U8 REQUESTTYPE;
    U8 REQUEST;
    U16 VALUE;
    U16 INDEX; //If it's related to an endpoint, it has the same meaning as the "bEndpointAddress" section of endpoint descriptor.
    U16 LENGTH;
}  SETUP_STRUCT, *SETUP_STRUCT_PTR;

/* USB 1.1 Setup Packet */
typedef  struct setup_struct_vendor
{
    U8 REQUESTTYPE;
    U8 REQUEST;
    U32 REG_OFFSET;
    U16 LENGTH;
}  SETUP_STRUCT_VENDOR, *SETUP_STRUCT_VENDOR_PTR;
//------------------------------------------------------------
/***************************************
**
** Code macros
**
***************************************/

#define USB_XD_QADD(head,tail,XD)      \
    if ((head) == NULL) {         \
        (head) = (XD);            \
    } else {                      \
        (tail)->SCRATCH_PTR->PRIVATE =(pointer) (XD);   \
    }                  \
    (tail) = (XD);               \
    (XD)->SCRATCH_PTR->PRIVATE = NULL

#define USB_XD_QGET(head,tail,XD)      \
    (XD) = (head);               \
    if (head) {                   \
        (head) = (XD_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
        if ((head) == NULL) {      \
            (tail) = NULL;          \
        }          \
    }

#define EHCI_DTD_QADD(head,tail,dTD)      \
    if ((head) == NULL) {         \
        (head) = (dTD);            \
    } else {                      \
        (tail)->SCRATCH_PTR->PRIVATE =(pointer) (dTD);   \
    }               \
    (tail) = (dTD);               \
    (dTD)->SCRATCH_PTR->PRIVATE = NULL

#define EHCI_DTD_QGET(head,tail,dTD)      \
    (dTD) = (head);               \
    if (head) {                   \
        (head) = (VUSB20_EP_TR_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
        if ((head) == NULL) {      \
            (tail) = NULL;          \
        }             \
    }

#define USB_BUF_QADD(head,tail,bufp)      \
    if ((head) == NULL) {         \
        (head) = (bufp);            \
    } else {                      \
        (tail)->NextTxBuf =(pointer) (bufp);   \
    }                  \
    (tail) = (bufp);               \
    (bufp)->NextTxBuf = NULL

#define USB_BUF_QGET(head,tail,bufp)      \
    (bufp) = (head);               \
    if (head) {                   \
        (head) = ((head)->NextTxBuf);  \
        if ((head) == NULL) {      \
            (tail) = NULL;          \
        }               \
    }



#endif
