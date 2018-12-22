#ifndef __arc_h__
#define __arc_h__
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
***  This file contains ARM-specific defines. The file is solely for compilations
*** and most customers will need to redfine the code in this file and in arm.c to
*** make it work on their system.
***                                                               
**************************************************************************
**END*********************************************************/

/*--------------------------------------------------------------------------*/
/*
**                            STANDARD TYPES
*/

/*
**  The following typedefs allow us to minimize portability problems
**  due to the various C compilers (even for the same processor) not
**  agreeing on the sizes of "int"s and "short int"s and "longs".
*/

#define _PTR_      *
#define _CODE_PTR_ *
#include  "string.h"
#include <stdio.h>
#include "predefines.h"
#include "set_mmu.h"
#include "bbu_malloc.h"
#include "USB.h"


#include "cache_ops.h"

#define _DATA_CACHE_
//#define __USB_OTG__

typedef char _PTR_                    char_ptr;    /* signed character       */
typedef unsigned char  uchar, _PTR_   uchar_ptr;   /* unsigned character     */

typedef signed   char   int_8, _PTR_   int_8_ptr;   /* 8-bit signed integer   */
typedef unsigned char  uint_8, _PTR_   uint_8_ptr;  /* 8-bit signed integer   */

typedef          short int_16, _PTR_   int_16_ptr;  /* 16-bit signed integer  */
typedef unsigned short uint_16, _PTR_  uint_16_ptr; /* 16-bit unsigned integer*/

typedef          long  int_32, _PTR_   int_32_ptr;  /* 32-bit signed integer  */
typedef unsigned long  uint_32, _PTR_  uint_32_ptr; /* 32-bit unsigned integer*/

typedef unsigned long  boolean;  /* Machine representation of a boolean */

typedef void _PTR_     pointer;  /* Machine representation of a pointer */

/* IEEE single precision floating point number (32 bits, 8 exponent bits) */
typedef float          ieee_single;

/* IEEE double precision floating point number (64 bits, 11 exponent bits) */
typedef double         ieee_double;
typedef pointer usb_device_handle;    //by tongchao
/*--------------------------------------------------------------------------*/
/*
**                          STANDARD CONSTANTS
**
**  Note that if standard 'C' library files are included after types.h,
**  the defines of TRUE, FALSE and NULL may sometimes conflict, as most
**  standard library files do not check for previous definitions.
*/

#ifdef  FALSE
   #undef  FALSE
#endif
#define FALSE ((boolean)0)

#ifdef  TRUE
   #undef  TRUE
#endif
#define TRUE ((boolean)1) 

#ifdef  NULL
   #undef  NULL
#endif

#ifdef __cplusplus
   #define NULL (0)
#else
   #define NULL ((pointer)0)
#endif


//typedef  uint_32  USB_REGISTER;

#define  BSP_VUSB20_HOST_BASE_ADDRESS0     USB_OTG_HS_BASE//USB_SPH_BASE//(USB_SPH_BASE+0x100)//+0x40)// (USB_OTG_BASE+0x100) //(0xd4208100)	//(0xFC5100)
#define  HSIC_BASE_ADDRESS0                            USB_SPH_BASE
#define  ICUSB_BASE_ADDRESS0 				USB_IC_USB_BASE
//#define  BSP_VUSB20_HOST_VECTOR0             (5)
//#define  ARC_JMP_TO_ADDR_INSTRN              ((uint_32)(0x381F0000))

//#define  BSP_VUSB20_DEVICE_BASE_ADDRESS0     (0xd4208100)	//(0xFC5100)
#define  BSP_VUSB20_DEVICE_VECTOR0           (5)
#define  ARC_JMP_TO_ADDR_INSTRN              ((uint_32)(0x381F0000))

//#define  BSP_VUSB20_OTG_BASE_ADDRESS0        (0xFC5100)
#define  BSP_VUSB20_OTG_VECTOR0              (5)
#define  BSP_VUSB20_OTG_USB_OFFSET           (0x70)



/* The ARC is little-endian, just like USB */
#define USB_uint_16_low(x)                   ((x) & 0xFF)
#define USB_uint_16_high(x)                  (((x) >> 8) & 0xFF)



/*********************************************************
Data caching related macros
**********************************************************/
#ifdef _DATA_CACHE_
	#define USB_Uncached			     		 volatile 
#else
	#define USB_Uncached	  volatile
#endif
/* Macro for aligning the EP queue head to 32 byte boundary */
#define USB_MEM32_ALIGN(n)                   ((n) + (-(n) & 31))
#define USB_CACHE_ALIGN(n)     		     USB_MEM32_ALIGN(n)	


#define PSP_CACHE_LINE_SIZE        (32)
#if PSP_CACHE_LINE_SIZE
#define PSP_MEMORY_ALIGNMENT       (PSP_CACHE_LINE_SIZE-1)
#else
#define PSP_MEMORY_ALIGNMENT       (3)
#endif
#define PSP_MEMORY_ALIGNMENT_MASK  (~PSP_MEMORY_ALIGNMENT)

#define _psp_set_aux(aux_reg,value) \
   _sr((unsigned)(value),(unsigned)(aux_reg))

#define _psp_get_aux(aux_reg) \
   (uint_32)_lr((unsigned)(aux_reg))

#define PSP_AUX_DC_IVDC                    (0x047)
#define PSP_AUX_DC_IVDL                    (0x04A)
#define PSP_AUX_DC_FLDL                    (0x04C)
#define PSP_AUX_DC_CTRL_FLUSH_STATUS       (0x00000100)
#define PSP_AUX_DC_CTRL_SUCCESS            (0x00000004)
#define PSP_AUX_DC_CTRL                    (0x048)

#define  OTG_INTERRUPT_ROUTINE_KEYWORD
#define DEVICE_INTERRUPT_ROUTINE_KEYWORD
#define HOST_INTERRUPT_ROUTINE_KEYWORD
#define  INTERRUPT_ROUTINE_KEYWORD

#define  DISABLE_INTERRUPTS()    

#define  ENABLE_INTERRUPTS()     

#ifdef __cplusplus
extern "C" {
#endif
#ifndef __USB_OS_MQX__
extern void USB_int_install_isr(uint_8, 
   INTERRUPT_ROUTINE_KEYWORD void (_CODE_PTR_ )(void), pointer);
extern void _disable_interrupts(void);
extern void _enable_interrupts(void);

#ifdef _DATA_CACHE_
	void _dcache_invalidate(void);
	void _dcache_invalidate_line(pointer);
	
	void _dcache_flush_mlines(pointer, int);
	void _dcache_flush_line(pointer);
	
	void _dcache_invalidate_mlines(pointer, int);
#endif

#ifndef NO_DYNAMIC_MEMORY_ALLOCATION
 extern	void * USB_memalloc(uint_32 n);
 extern	void * USB_Uncached_memalloc(uint_32 n);
// extern void  USB_memfree(void * aligned_ptr);
#endif

#endif
#ifdef __cplusplus
}
#endif

#if 0
#define BU_U32 unsigned int
#define BU_U16 unsigned short
#define BU_U8 unsigned char


#endif

#define BU_REG_READ16(x) (*(volatile BU_U16 *)(x) & 0xffff)
#define BU_REG_WRITE16(x,y) ((*(volatile BU_U16 *)(x)) = y & 0xffff )

#define BU_REG_READ8(x) (*(volatile BU_U8 *)(x) & 0xff)
#define BU_REG_WRITE8(x,y) ((*(volatile BU_U8 *)(x)) = y & 0xff )

//#define BU_REG_RDMDFYWR(x,y)  (BU_REG_WRITE(x,((BU_REG_READ(x))|y)))







extern void BBU_usWait(uint_32 us);
extern void BBU_putstr(uint_8 *sptr);
extern void BBU_puthexd(uint_32 hexvalue);
extern void BBU_puthexw(uint_16 hexvalue);
extern unsigned char *MRVL_USB_memalloc( int size) ;//by tongchao
#define DEBUG_LOG_TRACE BBU_putstr
#define USB_memalloc(bufferlen)                    bbu_nc_malloc(bufferlen)//bbu_vm_malloc(bufferlen,NON_CACHEABLE)
#define USB_Uncached_memalloc(bufferlen)    bbu_nc_malloc(bufferlen)        //  bbu_vm_malloc(bufferlen,NON_CACHEABLE)
#define USB_memfree               bbu_mfree


//void USB_dcache_flush_mlines( void *pMem ,uint_32 size);
//void USB_dcache_invalidate_mlines( void *pMem, uint_32 size);    

#define USB_dcache_flush_mlines dcache_clean_range
#define USB_dcache_invalidate_mlines dcache_invalidate_range
extern void USB_dev_memzero(void *ptr, int n);


#define  epSendType              0x80
#define  epReceiveType          0x00
#define USB_ISR_MODE
//#define LINUX_CDC   
//#define USB_UART
//#define USB_UART_DEFAULT

#endif
