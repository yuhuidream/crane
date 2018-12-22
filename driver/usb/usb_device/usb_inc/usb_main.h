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

#define TEST_J      1
#define TEST_K      2
#define TEST_SE0_NAK    3
#define TEST_PACKET 4
#define TEST_FORCE_EN   5

typedef pointer _usb_device_instance_handle;       /* item on host's list */
typedef pointer _usb_interface_descriptor_handle;  /* Chapter 9 descriptor*/
typedef pointer _usb_class_intf_handle;            /* interface info.     */

typedef struct class_call_struct
{
   _usb_class_intf_handle  class_intf_handle;
   uint_32                 code_key;
   
}  CLASS_CALL_STRUCT, _PTR_ CLASS_CALL_STRUCT_PTR;
#if 0
typedef struct device_struct {
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
typedef struct setup_struct {
   uint_8      REQUESTTYPE;
   uint_8      REQUEST;
   uint_16     VALUE;
   uint_16     INDEX;
   uint_16     LENGTH;
} SETUP_STRUCT, _PTR_ SETUP_STRUCT_PTR;

#ifdef DUPLICATES
/* USB Command Block Wrapper */
typedef struct cbw_struct {
   uint_32  DCBWSIGNATURE;
   uint_32  DCBWTAG;
   uint_32  DCBWDATALENGTH;
   uchar    BMCBWFLAGS;
   /* 4 MSBs bits reserved */
   uchar    BCBWCBLUN;
   /* 3 MSB reserved */
   uchar    BCBWCBLENGTH;
   uchar    CBWCB[16];
} CBW_STRUCT, _PTR_ CBW_STRUCT_PTR;

/* USB Command Status Wrapper */
typedef struct csw_struct {
   uint_32  DCSWSIGNATURE;
   uint_32  DCSWTAG;
   uint_32  DCSWDATARESIDUE;
   uchar    BCSWSTATUS;
} CSW_STRUCT, _PTR_ CSW_STRUCT_PTR;
#endif

/* USB Mass storage Inquiry Command */
typedef struct mass_storage_inquiry {
   uchar    OPCODE;
   uchar    LUN;
   uchar    PAGE_CODE;
   uchar    RESERVED1;
   uchar    ALLOCATION_LENGTH;
   uchar    RESERVED2[7];
} MASS_STORAGE_INQUIRY_STRUCT, _PTR_ MASS_STORAGE_INQUIRY_PTR;

/* USB Mass storage READ CAPACITY Data */
typedef struct mass_storage_read_capacity {
   uchar    LAST_LOGICAL_BLOCK_ADDRESS[4];
   uchar    BLOCK_LENGTH_IN_BYTES[4];
} MASS_STORAGE_READ_CAPACITY_STRUCT, _PTR_ MASS_STORAGE_READ_CAPACITY_STRUCT_PTR;

/* USB Mass storage Device information */
typedef struct mass_storage_device_info {
   uchar    PERIPHERAL_DEVICE_TYPE;    /* Bits 0-4. All other bits reserved */
   uchar    RMB;                       /* Bit 7. All other bits reserved */
   uchar    ANSI_ECMA_ISO_VERSION;     /* ANSI: bits 0-2, ECMA: bits 3-5, 
                                       ** ISO: bits 6-7 
                                       */
   uchar    RESPONSE_DATA_FORMAT;      /* bits 0-3. All other bits reserved */
   uchar    ADDITIONAL_LENGTH;         /* For UFI device: always set to 0x1F */
   uchar    RESERVED1[3];
   uchar    VENDOR_INFORMATION[8];
   uchar    PRODUCT_ID[16];
   uchar    PRODUCT_REVISION_LEVEL[4];
} MASS_STORAGE_DEVICE_INFO_STRUCT, _PTR_ MASS_STORAGE_DEVICE_INFO_PTR;

#define MIN(a,b) ((a) < (b) ? (a) : (b))      

#define BUFFERSIZE      1023

typedef struct double_buffer_struct { /* Body */
   uint_8      buffer[2][BUFFERSIZE];
   boolean     whichbuffer;
} DOUBLE_BUFFER_STRUCT, _PTR_ DOUBLE_BUFFER_STRUCT_PTR;

#define INIT_DBUF(x)    ((x.whichbuffer) = 0)
#define TOGGLE_DBUF(x)  ((x.whichbuffer) ^= 1)
#define DBUF(x)         (x.buffer[x.whichbuffer])








#define status()  1

#if 0
#define BU_U32 			unsigned int 
#define BU_U16 			unsigned short 
#define BU_U8 			unsigned char 
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
