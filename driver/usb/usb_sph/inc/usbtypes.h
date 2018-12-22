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

typedef  uint_32  USB_REGISTER;

//#define  BSP_VUSB20_HOST_BASE_ADDRESS0       (0xd4208100) //(0xFC5100)
//#define  BSP_VUSB20_HOST_VECTOR0             (5)
//#define  ARC_JMP_TO_ADDR_INSTRN              ((uint_32)(0x381F0000))

#define  BSP_VUSB20_DEVICE_BASE_ADDRESS0     (0xd4208100)   //(0xFC5100)
#define  BSP_VUSB20_DEVICE_VECTOR0           (5)
#define  ARC_JMP_TO_ADDR_INSTRN              ((uint_32)(0x381F0000))

#define  BSP_VUSB20_OTG_BASE_ADDRESS0        (0xFC5100)
#define  BSP_VUSB20_OTG_VECTOR0              (5)
#define  BSP_VUSB20_OTG_USB_OFFSET           (0x70)

/*define the memory address for USB stack if heap is not supported */
#ifdef NO_DYNAMIC_MEMORY_ALLOCATION
/*THE MEMORY LOCATION WHERE STACK WILL OPERATE ON */
// fl:  #define  USB_STACK_MEMORY_START               0x200000
#define  USB_STACK_MEMORY_START               0xd1005000
/*THE MEMORY LOCATIONS WHERE HARDWARE DRIVER WILL OPERATE ON */
#define  USB_DRIVER_MEMORY_START              0xd1002000    //0x150000
/*QH address should be aligned on a 2K boundry (bits 0-10 should be 0)*/
// fl:  #define  USB_DRIVER_QH_BASE                   0xC0000000    //0x160000
#define  USB_DRIVER_QH_BASE                   0xd1003000    //0x160000
/*DTD address should be aligned on a 4 byte boundry */
//fl:   #define  USB_DRIVER_DTD_BASE                  0xC0001000    //0x170000
#define  USB_DRIVER_DTD_BASE                  0xd1003100    //0x170000
// fl:  #define  USB_DRIVER_SCRATCH_STRUCT_BASE       0xC0004000    //0x180000
#define  USB_DRIVER_SCRATCH_STRUCT_BASE       0xd1003800    //0x180000

//fl:   #define  USB_DISK_APP_MEM_BASE                0xC0008000    //0x100000
#define  USB_DISK_APP_MEM_BASE                0xd1004000    //0x100000


#endif

/* The ARC is little-endian, just like USB */
#define USB_uint_16_low(x)                   ((x) & 0xFF)
#define USB_uint_16_high(x)                  (((x) >> 8) & 0xFF)



/*********************************************************
Data caching related macros
**********************************************************/
#ifdef _DATA_CACHE_
#define USB_Uncached                         volatile _Uncached
#else
#define USB_Uncached      volatile
#endif
/* Macro for aligning the EP queue head to 32 byte boundary */
#define USB_MEM32_ALIGN(n)                   ((n) + (-(n) & 31))
#define USB_CACHE_ALIGN(n)               USB_MEM32_ALIGN(n)


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
/* void _dcache_flush_mlines(pointer, uint_32); */
void _dcache_flush_mlines(pointer, int);
void _dcache_flush_line(pointer);
/* void _dcache_invalidate_mlines(pointer, uint_32); */
void _dcache_invalidate_mlines(pointer, int);
#endif

#ifndef NO_DYNAMIC_MEMORY_ALLOCATION
extern void *USB_memalloc(uint_32 n);
extern void *USB_Uncached_memalloc(uint_32 n);
extern void  USB_memfree(void *aligned_ptr);
#endif

#endif
#ifdef __cplusplus
}
#endif


#define BU_U32 unsigned int
#define BU_U16 unsigned short
#define BU_U8 unsigned char

#define BU_REG_READ(x) (*(volatile BU_U32 *)(x))
#define BU_REG_WRITE(x,y) ((*(volatile BU_U32 *)(x)) = y )

#define BU_REG_READ16(x) (*(volatile BU_U16 *)(x) & 0xffff)
#define BU_REG_WRITE16(x,y) ((*(volatile BU_U16 *)(x)) = y & 0xffff )

#define BU_REG_READ8(x) (*(volatile BU_U8 *)(x) & 0xff)
#define BU_REG_WRITE8(x,y) ((*(volatile BU_U8 *)(x)) = y & 0xff )

#define BU_REG_RDMDFYWR(x,y)  (BU_REG_WRITE(x,((BU_REG_READ(x))|y)))



#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)
#define BIT_4 (1 << 4)
#define BIT_5 (1 << 5)
#define BIT_6 (1 << 6)
#define BIT_7 (1 << 7)
#define BIT_8 (1 << 8)
#define BIT_9 (1 << 9)
#define BIT_10 (1 << 10)
#define BIT_11 (1 << 11)
#define BIT_12 (1 << 12)
#define BIT_13 (1 << 13)
#define BIT_14 (1 << 14)
#define BIT_15 (1 << 15)
#define BIT_16 (1 << 16)
#define BIT_17 (1 << 17)
#define BIT_18 (1 << 18)
#define BIT_19 (1 << 19)
#define BIT_20 (1 << 20)
#define BIT_21 (1 << 21)
#define BIT_22 (1 << 22)
#define BIT_23 (1 << 23)
#define BIT_24 (1 << 24)
#define BIT_25 (1 << 25)
#define BIT_26 (1 << 26)
#define BIT_27 (1 << 27)
#define BIT_28 (1 << 28)
#define BIT_29 (1 << 29)
#define BIT_30 (1 << 30)
#define BIT_31 ((unsigned)1 << 31)

#define SHIFT0(Val)  (Val)
#define SHIFT1(Val)  ((Val) << 1)
#define SHIFT2(Val)  ((Val) << 2)
#define SHIFT3(Val)  ((Val) << 3)
#define SHIFT4(Val)  ((Val) << 4)
#define SHIFT5(Val)  ((Val) << 5)
#define SHIFT6(Val)  ((Val) << 6)
#define SHIFT7(Val)  ((Val) << 7)
#define SHIFT8(Val)  ((Val) << 8)
#define SHIFT9(Val)  ((Val) << 9)
#define SHIFT10(Val) ((Val) << 10)
#define SHIFT11(Val) ((Val) << 11)
#define SHIFT12(Val) ((Val) << 12)
#define SHIFT13(Val) ((Val) << 13)
#define SHIFT14(Val) ((Val) << 14)
#define SHIFT15(Val) ((Val) << 15)
#define SHIFT16(Val) ((Val) << 16)
#define SHIFT17(Val) ((Val) << 17)
#define SHIFT18(Val) ((Val) << 18)
#define SHIFT19(Val) ((Val) << 19)
#define SHIFT20(Val) ((Val) << 20)
#define SHIFT21(Val) ((Val) << 21)
#define SHIFT22(Val) ((Val) << 22)
#define SHIFT23(Val) ((Val) << 23)
#define SHIFT24(Val) ((Val) << 24)
#define SHIFT25(Val) ((Val) << 25)
#define SHIFT26(Val) ((Val) << 26)
#define SHIFT27(Val) ((Val) << 27)
#define SHIFT28(Val) ((Val) << 28)
#define SHIFT29(Val) ((Val) << 29)
#define SHIFT30(Val) ((Val) << 30)
#define SHIFT31(Val) ((Val) << 31)



extern BBU_usWait(uint_32 us);
extern BBU_putstr(uint_8 *sptr);
extern BBU_puthexd(uint_32 hexvalue);
extern BBU_puthexw(uint_16 hexvalue);

#define DEBUG_LOG_TRACE BBU_putstr
#define USB_memalloc  MRVL_USB_memalloc
#define USB_Uncached_memalloc MRVL_USB_memalloc

#endif
