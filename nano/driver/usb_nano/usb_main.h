#ifndef _OTGMAIN_H
#define _OTGMAIN_H



#define  DATA_SIZE                  (300)
#define  DEMO_CONFIG_DESC_SIZE      (64)
#define  DEMO_VENDOR_OFFSET         (8)
#define  ARC_VENDOR_ID              (0x0B20)
#define  MANUF_STRING_INDEX         (14)
#define  PRODUCT_STRING_INDEX       (15)
#define  USB_OTG_DESCRIPTOR_TYPE    (9)
#define  MAX_NAK_COUNT              (15)


#define  BIDIRECTIONAL              (0x4d)

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)



typedef pointer _usb_device_instance_handle;       /* item on host's list */
typedef pointer _usb_interface_descriptor_handle;  /* Chapter 9 descriptor*/
typedef pointer _usb_class_intf_handle;            /* interface info.     */

typedef struct class_call_struct
{
    _usb_class_intf_handle  class_intf_handle;
    uint_32                 code_key;

}  CLASS_CALL_STRUCT, _PTR_ CLASS_CALL_STRUCT_PTR;
#if 0
typedef struct device_struct
{
    uint_32                          STATE;
    _usb_device_instance_handle      DEV_HANDLE;
    _usb_interface_descriptor_handle INTF_HANDLE;
    CLASS_CALL_STRUCT                CLASS_INTF;
    boolean                          SUPPORTED;
} DEVICE_STRUCT,  _PTR_ DEVICE_STRUCT_PTR;
#endif




#define  TOTAL_LOGICAL_ADDRESS_BLOCKS     (20)
#define  LENGTH_OF_EACH_LAB               (512)

#define  APP_CONTROL_MAX_PKT_SIZE         (64)
#define  DEV_DESC_MAX_PACKET_SIZE         (7)
#define  EP1_FS_MAX_PACKET_SIZE           (64)
#define  EP1_HS_MAX_PACKET_SIZE           (512)
#define  CFG_DESC_EP1TX_MAX_PACKET_SIZE   (22)
#define  CFG_DESC_EP1RX_MAX_PACKET_SIZE   (29)


#define  USB_DCBWSIGNATURE       (0x43425355)
#define  USB_DCSWSIGNATURE       (0x53425355)
#define  USB_CBW_DIRECTION_BIT   (0x80)

/* USB 1.1 Setup Packet */
typedef struct setup_struct
{
    uint_8      REQUESTTYPE;
    uint_8      REQUEST;
    uint_16     VALUE;
    uint_16     INDEX;
    uint_16     LENGTH;
} SETUP_STRUCT, _PTR_ SETUP_STRUCT_PTR;




#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define BUFFERSIZE      1023


#define INIT_DBUF(x)    ((x.whichbuffer) = 0)
#define TOGGLE_DBUF(x)  ((x.whichbuffer) ^= 1)
#define DBUF(x)         (x.buffer[x.whichbuffer])








#define status()  1

#if 0
#define BU_U32          unsigned int
#define BU_U16          unsigned short
#define BU_U8           unsigned char
#endif
//#define BU_REG_READ(x) (*(volatile BU_U32 *)(x))
//#define BU_REG_WRITE(x,y) ((*(volatile BU_U32 *)(x)) = y )

#define BU_REG_READ16(x) (*(volatile BU_U16 *)(x) & 0xffff)
#define BU_REG_WRITE16(x,y) ((*(volatile BU_U16 *)(x)) = y & 0xffff )

#define BU_REG_READ8(x) (*(volatile BU_U8 *)(x) & 0xff)
#define BU_REG_WRITE8(x,y) ((*(volatile BU_U8 *)(x)) = y & 0xff )

//#define BU_REG_RDMDFYWR(x,y)  (BU_REG_WRITE(x,((BU_REG_READ(x))|y)))

#define  INT_USB 44
#define USB_PRIORITY 15




#endif
