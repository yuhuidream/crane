#ifndef __disk_h__
#define __disk_h__
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
***  This file contains the USB Disk example application specific defines.
***                                                               
**************************************************************************
**END*********************************************************/
#include "usb_define.h"
#include "usb_device.h"
#define  EP1_FS_MAX_PACKET_SIZE           (64)
#define  EP1_HS_MAX_PACKET_SIZE           (512)

/**********modified by cosmo*****************/

#define  TOTAL_LOGICAL_ADDRESS_BLOCKS    20480 //  20480*512=10M ,we assign 10M space to   simulate USB device memory.
#define  LENGTH_OF_EACH_LAB               (512)

#define  USB_DCBWSIGNATURE       (0x43425355)
#define  USB_DCSWSIGNATURE       (0x53425355)
#define  USB_CBW_DIRECTION_BIT   (0x80)

#define DISKBUFFERSIZE            (2048)

/* USB Command Block Wrapper */
typedef struct cbw_struct {
   U32  DCBWSIGNATURE;
   U32  DCBWTAG;
   U32  DCBWDATALENGTH;
   U8    BMCBWFLAGS;
   /* 4 MSBs bits reserved */
   U8    BCBWCBLUN;
   /* 3 MSB reserved */
   U8    BCBWCBLENGTH;
   U8    CBWCB[16];
} CBW_STRUCT, * CBW_STRUCT_PTR;

/* USB Command Status Wrapper */
typedef  struct csw_struct {
   U32  DCSWSIGNATURE;
   U32  DCSWTAG;
   U32  DCSWDATARESIDUE;
   U8    BCSWSTATUS;
} CSW_STRUCT, * CSW_STRUCT_PTR;

/* USB Mass storage Inquiry Command */
typedef struct mass_storage_inquiry {
   U8    OPCODE;
   U8    LUN;
   U8    PAGE_CODE;
   U8    RESERVED1;
   U8    ALLOCATION_LENGTH;
   U8    RESERVED2[7];
} MASS_STORAGE_INQUIRY_STRUCT, * MASS_STORAGE_INQUIRY_PTR;

/* USB Mass storage READ CAPACITY Data */
typedef struct mass_storage_read_capacity {
   U8    LAST_LOGICAL_BLOCK_ADDRESS[4];
   U8    BLOCK_LENGTH_IN_BYTES[4];
} MASS_STORAGE_READ_CAPACITY_STRUCT, *MASS_STORAGE_READ_CAPACITY_STRUCT_PTR;

/* USB Mass storage Device information */
typedef struct mass_storage_device_info {
   U8    PERIPHERAL_DEVICE_TYPE;    /* Bits 0-4. All other bits reserved */
   U8    RMB;                       /* Bit 7. All other bits reserved */
   U8    ANSI_ECMA_ISO_VERSION;     /* ANSI: bits 0-2, ECMA: bits 3-5, 
                                       ** ISO: bits 6-7 
                                       */
   U8    RESPONSE_DATA_FORMAT;      /* bits 0-3. All other bits reserved */
   U8    ADDITIONAL_LENGTH;         /* For UFI device: always set to 0x1F */
   U8    RESERVED1[3];
   U8    VENDOR_INFORMATION[8];
   U8    PRODUCT_ID[16];
   U8    PRODUCT_REVISION_LEVEL[4];
} MASS_STORAGE_DEVICE_INFO_STRUCT, * MASS_STORAGE_DEVICE_INFO_PTR;
extern int bbu_putchar(char u8c);
#endif
/* EOF */
