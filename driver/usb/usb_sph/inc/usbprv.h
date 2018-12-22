#ifndef __usbprv_h__
#define __usbprv_h__ 1
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
***   This file contains the internal USB specific type definitions
***
**************************************************************************
**END*********************************************************/
#ifndef __USB_OS_MQX__


#define USB_lock()                         _disable_interrupts()
#define USB_unlock()                        _enable_interrupts()
#define USB_memcopy(src,dst,n)               memcpy(dst,src,n)
/**********************************************************
The following lines have been commented because when MQX
is not used, the RTOS abstration layer provides the
implementation of USB_memalloc routine which aligns
the pointers on cache line size.
**********************************************************/
/*
#define USB_memalloc(n)                      malloc(((n) + (-(n) & 31)))
#define USB_memalloc(n)                      malloc(n)
#define USB_Uncached_memalloc(n)             malloc(n)

void * USB_memalloc(uint_32 n);
void  USB_memfree(void * aligned_ptr);

#define USB_memfree(ptr)                     free(ptr)
*/
/*
#ifdef NO_DYNAMIC_MEMORY_ALLOCATION
    #define USB_memfree(ptr)
#endif

#define USB_memzero(ptr,n)                   memset(ptr,0,n)
#define USB_memcopy(src,dst,n)               memcpy(dst,src,n)
#ifdef _DATA_CACHE_
    #define USB_dcache_invalidate()              _dcache_invalidate()
    #define USB_dcache_invalidate_line(p)        _dcache_invalidate_line(p)
    #define USB_dcache_invalidate_mlines(p,n)    _dcache_invalidate_mlines(p,n)
    #define USB_dcache_flush_line(p)             _dcache_flush_line(p)
    #define USB_dcache_flush_mlines(p,n)         _dcache_flush_mlines(p,n)
#else
    #define USB_dcache_invalidate()
    #define USB_dcache_invalidate_line(p)
    #define USB_dcache_invalidate_mlines(p,n)
    #define USB_dcache_flush_line(p)
    #define USB_dcache_flush_mlines(p,n)

#endif




#else
#include "mqx.h"
#include "psp.h"
#define USB_lock()                           _int_disable()
#define USB_unlock()                         _int_enable()
#define USB_memalloc(n)                      _mem_alloc_system_zero(n)
#define USB_Uncached_memalloc(n)             _mem_alloc_system_zero(n)
#define USB_memfree(ptr)                     _mem_free(ptr)
#define USB_memzero(ptr,n)                   _mem_zero(ptr,n)
#define USB_memcopy(src,dst,n)               _mem_copy(src,dst,n)
#define USB_install_isr(vector, isr, data)   _int_install_isr(vector, isr, data)
#ifdef _DATA_CACHE_
    #define USB_dcache_invalidate()              _DCACHE_INVALIDATE()
    #define USB_dcache_invalidate_line(p)        _DCACHE_INVALIDATE_LINE(p)
    #define USB_dcache_invalidate_mlines(p,n)    _DCACHE_INVALIDATE_MLINES(p,n)
    #define USB_dcache_flush_line(p)             _DCACHE_FLUSH_LINE(p)
    #define USB_dcache_flush_mlines(p,n)         _DCACHE_FLUSH_MLINES(p,n)
#else
    #define USB_dcache_invalidate()
    #define USB_dcache_invalidate_line(p)
    #define USB_dcache_invalidate_mlines(p,n)
    #define USB_dcache_flush_line(p)
    #define USB_dcache_flush_mlines(p,n)
#endif

*/

//#define USB_install_isr(vector, isr, data)   USB_int_install_isr(vector, isr, data)

#endif


#endif

/* EOF */

