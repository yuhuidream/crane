#ifndef __host_dev_list_h__
#define __host_dev_list_h__

/**HEADER*********************************************************
-------------------------------------------------------- ---------------------
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
***   This file defines the device list for USB class drivers.
***
***   +--------+
***   \  ROOT  \---+                       List of devices, one entry
***   +--------+   \    +--------+           for each attached device
***                +--->\  NEXT  \---+
***                     \--------\   \    +--------+
***                  +--\ MEMLST \   +--->\  NEXT  \---+
***                  \  \--------\        \--------\   \    +--------+
***   Fixed-size     \  \ Device \     +--\ MEMLST \   +--->\  NULL  \
***     per-device   \  \ struct \     \  \--------\        \--------\
***     memory       \  +--------+     \  \ Device \     +--\ MEMLST \
***                  \                 \  \ struct \     \  \--------\
***                  \                 \  +--------+     \  \ Device \
***                  \                 \                 \  \ struct \
***                  +->+--------+     \                 \  +--------+
***   Lists of other    \  NEXT  \     \                 \
***     memory per   +--\--------\     +->+--------+     \
***     device --    \  \ LENGTH \        \  NULL  \     \
***     e.g. pipes,  \  \--------\        \--------\     +->+--------+
***     buffers,     \  \ Device \        \ LENGTH \        \  NULL  \
***     structs      \  \ malloc \        \--------\        \--------\
***                  \  \ memory \        \ Device \        \ LENGTH \
***                  \  +--------+        \ malloc \        \--------\
***                  \                    \ memory \        \ Device \
***                  \                    +--------+        \ malloc \
***                  +->+--------+                          \ memory \
***                     \  NULL  \                          +--------+
***                     \--------\
***                     \ LENGTH \  Length used to check
***                     \--------\    buffer overrun etc.
***                     \ Device \
***                     \ malloc \
***                     \ memory \
***                     +--------+
***
***   IMPLEMENTATION:
***      Linking list items depends on root pointers behaving like
***        a short entry containing a "next" pointer = NULL when
***        the list is empty.  So next MUST BE FIRST in the structs.
***
**************************************************************************
**END*********************************************************/

#include "usb_desc.h"

#define  MAX_DEV_INTFS  (32)

/* Types of malloc memory used with USB devices */
enum memory_type
{
    USB_MEMTYPE_MIN_VALUE,  /* lowest value - 1     */
    USB_MEMTYPE_CONFIG,     /* device configuration */
    USB_MEMTYPE_INTERFACE,  /* Interface descr */
    USB_MEMTYPE_PIPES,      /* pipe bundle          */
    USB_MEMTYPE_BUFFER,     /* buffer               */
    USB_MEMTYPE_CLASS,      /* class driver struct  */
    USB_MEMTYPE_MAX_VALUE   /* highest value + 1    */
};
typedef enum memory_type memory_type;

typedef struct dev_memory
{
    struct dev_memory _PTR_ next;    /* next memory block or NULL */
    uint_32                 blktype; /* type of data, block usage */
    uint_32                 blksize; /* size of data area following */
#ifdef _DATA_CACHE_
    /* add padding to ensure that cache line boundry end here
    add Enough bytes to make it to cache boundry */
    uint_8                 reserved[USB_CACHE_ALIGN(12) - 12 ];
#endif

#ifdef _DATA_CACHE_
#define  MEM_HEADER_LEN    (sizeof(pointer) + 2*sizeof(uint_32) + USB_CACHE_ALIGN(12) - 12)
#else
#define  MEM_HEADER_LEN    (sizeof(pointer) + 2*sizeof(uint_32))
#endif
    union
    {
        uint_8         data[1];    /* actually [0 ... blksize-1] */
        uchar          bufr[1];    /* uchars [0 ... blksize-1] */
        uint_32        word[1];    /* general-purpose array */
        pointer        bundle[1];  /* pipe bundle */
    } payload;                    /* payload of data */
} DEV_MEMORY, _PTR_ DEV_MEMORY_PTR;

/********************************************************************
Note that device instance structure keeps data buffers inside it.
These buffers are passed to DMA when host does enumeration. This
means that we must ensure that buffers inside this stucture
are aligned in PSP cache line size.
********************************************************************/

/* Fixed-length fields applicable to all devices */
typedef struct dev_instance
{
    struct dev_instance  _PTR_ next;             /* next device, or NULL */
    DEV_MEMORY_PTR             memlist;          /* list of malloc'd memory */
    _usb_host_handle           host;             /* host (several can exist) */
    uint_8                     speed;            /* device speed */
    uint_8                     hub_no;           /* hub # (root hub = 0) */
    uint_8                     port_no;          /* hub's port # (1 - 8) */
    uint_8                     address;          /* USB address, 1 - 127 */

    uint_8                     new_config;       /* non-zero = new config */
    uint_16                    cfg_value;
    uint_8                     num_of_interfaces;

    uint_32                    state;            /* device state */
    _usb_pipe_handle           control_pipe;     /* control pipe handle */
    tr_callback                control_callback; /* control pipe callback */
    pointer                    control_callback_param; /* control callback param */
    uint_32                    class_key;        /* caller verification */

#ifdef _DATA_CACHE_
    /***********************************************************
    44 bytes have been used above. Device descriptor must be
    aligned on a cache boundry so we must ensure that there
    is enough padding before it.
    ***********************************************************/

    uint_8                    RESERVED1[USB_CACHE_ALIGN(44) - 44];
#endif
    DEVICE_DESCRIPTOR         dev_descriptor;   /* device descriptor */

#ifdef _DATA_CACHE_
    uint_8                    RESERVED2[USB_CACHE_ALIGN(18) - 18]; /* complete padding */
#endif

    uchar                     buffer[9];        /* enumeration buffer */
#ifdef _DATA_CACHE_
    uint_8                    RESERVED3[USB_CACHE_ALIGN(9) - 9];  /* complete padding */
#endif

    /* Most-recently referenced interface in get-descriptor call */
    INTERFACE_DESCRIPTOR_PTR   intf_recently;
    /* Interface/s which have been selected (pipes are open) */
    INTERFACE_DESCRIPTOR_PTR   intf_descriptor[MAX_DEV_INTFS];

} DEV_INSTANCE, _PTR_ DEV_INSTANCE_PTR;

/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

extern USB_STATUS usb_dev_list_get_memory(DEV_INSTANCE_PTR, uint_16 mem_size,
        memory_type mem_type, pointer _PTR_ header_ptr);

#ifdef __cplusplus
}
#endif
#define MSC_CLASS

#endif

/* EOF */
