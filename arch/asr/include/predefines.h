#ifndef __INC_PREDEFINES_H
#define __INC_PREDEFINES_H

#include <stdint.h>        /* exact-width integers, WG14/N843 C99, 7.18.1.1 */

/*
typedef signed char     int8_t
typedef unsigned char   uint8_t
typedef signed int  int16_t
typedef unsigned int    uint16_t
typedef signed long int     int32_t
typedef unsigned long int   uint32_t
typedef signed long long int    int64_t
typedef unsigned long long int  uint64_t
*/

#define BU_REG_READ(x) (*(volatile uint32_t *)(x))
#define BU_REG_WRITE(x,y) ((*(volatile uint32_t *)(x)) = (y) )
#define BU_REG_RDMDFYWR(x,y)  (BU_REG_WRITE((x),((BU_REG_READ((x)))|(y))))
#define BU_REG_RDCLEAR(x,y)  (BU_REG_WRITE((x),((BU_REG_READ((x)))&(~(y)))))

#define BU_REG_READB(x) (*(volatile uint8_t *)(x))
#define BU_REG_WRITEB(x,y) ((*(volatile uint8_t *)(x)) = y )
#define BU_REG_RDMDFYWRB(x,y)  (BU_REG_WRITE(x,((BU_REG_READ(x))|y)))
#define BU_REG_RDCLEARB(x,y)  (BU_REG_WRITE(x,((BU_REG_READ(x))&(~y))))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define BU_U32          uint32_t
#define BU_U16          uint16_t
#define BU_U8           uint8_t

#define __nop() do { __asm__ __volatile__("nop" : : : "memory"); } while(0)
#define   dsb() do { __asm__ __volatile__("dsb" : : : "memory"); } while(0)
#define   dmb() do { __asm__ __volatile__("dmb" : : : "memory"); } while(0)
#define   isb() do { __asm__ __volatile__ ("isb" : : : "memory");} while(0)
#define   wfi() do { __asm__ __volatile__("wfi"); } while(0)
#define   wfe() do { __asm__ __volatile__("wfe"); } while(0)

#define   min(x,y)    ((x) < (y) ? x : y)
#define   max(x,y)    ((x) > (y) ? x : y)

#define   ATTRIBUTE_NCNB          __attribute__((section("ncnbBuffer")))

#ifndef ADDR_ALIGN
#define ADDR_ALIGN(addr_, align_)         ((addr_+align_-1)&(~(align_-1)))
#endif

typedef struct
{
    BU_U32 address;
    BU_U32 bitVal;
}REG_ADDR_BIT_VAL;


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

extern int bbu_printf(const char *fmt, ...);

// printf debug macro.
#define ON_OFF 1  //0

extern int bbu_print_level;

#define DBG_LEVEL     1
#define INFO_LEVEL    2
#define CRIT_LEVEL    3
#define MASK_LEVEL    4

#define DBG_MSG          "<1>"
#define INFO_MSG         "<2>"
#define CRIT_MSG         "<3>"

#define DBG_PRINT(cond,str, ...) ((void)((cond)?(bbu_printf (str, ##__VA_ARGS__)),1:0))
#ifdef CONFIG_MP3_POWER_TEST
#define MP3_PWR_LOG "<0>"
#else
#define MP3_PWR_LOG
#endif
//e.g. DBG_PRINT(ON_OFF, ("\n"));

#ifdef __ARMCC_VERSION
#define __PRAGMA_ALIGN(N)       __align(N)
#else /*  __GNUC__ & Marvell Compiler */
#define __PRAGMA_ALIGN(N)       __attribute__ ((aligned(N)))
#endif

/* The following is a list of Marvell status    */
#define MV_ERROR            (-1)
#define MV_OK               (0x00)  /* Operation succeeded                   */
#define MV_FAIL             (0x01)  /* Operation failed                      */
#define MV_BAD_VALUE        (0x02)  /* Illegal value (general)               */
#define MV_OUT_OF_RANGE     (0x03)  /* The value is out of range             */
#define MV_BAD_PARAM        (0x04)  /* Illegal parameter in function called  */
#define MV_BAD_PTR          (0x05)  /* Illegal pointer value                 */
#define MV_BAD_SIZE         (0x06)  /* Illegal size                          */
#define MV_BAD_STATE        (0x07)  /* Illegal state of state machine        */
#define MV_SET_ERROR        (0x08)  /* Set operation failed                  */
#define MV_GET_ERROR        (0x09)  /* Get operation failed                  */
#define MV_CREATE_ERROR     (0x0A)  /* Fail while creating an item           */
#define MV_NOT_FOUND        (0x0B)  /* Item not found                        */
#define MV_NO_MORE          (0x0C)  /* No more items found                   */
#define MV_NO_SUCH          (0x0D)  /* No such item                          */
#define MV_TIMEOUT          (0x0E)  /* Time Out                              */
#define MV_NO_CHANGE        (0x0F)  /* Parameter(s) is already in this value */
#define MV_NOT_SUPPORTED    (0x10)  /* This request is not support           */
#define MV_NOT_IMPLEMENTED  (0x11)  /* Request supported but not implemented */
#define MV_NOT_INITIALIZED  (0x12)  /* The item is not initialized           */
#define MV_NO_RESOURCE      (0x13)  /* Resource not available (memory ...)   */
#define MV_FULL             (0x14)  /* Item is full (Queue or table etc...)  */
#define MV_EMPTY            (0x15)  /* Item is empty (Queue or table etc...) */
#define MV_INIT_ERROR       (0x16)  /* Error occured while INIT process      */
#define MV_HW_ERROR         (0x17)  /* Hardware error                        */
#define MV_TX_ERROR         (0x18)  /* Transmit operation not succeeded      */
#define MV_RCV_ERROR        (0x19)  /* Recieve operation not succeeded       */
#define MV_NOT_READY        (0x1A)  /* The other side is not ready yet       */
#define MV_ALREADY_EXIST    (0x1B)  /* Tried to create existing item         */
#define MV_OUT_OF_CPU_MEM   (0x1C)  /* Cpu memory allocation failed.         */
#define MV_NOT_STARTED      (0x1D)  /* Not started yet         */
#define MV_BUSY             (0x1E)  /* Item is busy.                         */
#define MV_TERMINATE        (0x1F)  /* Item terminates it's work.            */
#define MV_NOT_ALIGNED      (0x20)  /* Wrong alignment                       */
#define MV_NOT_ALLOWED      (0x21)  /* Operation NOT allowed                 */


#define MV_TRUE             1
#define MV_FALSE            0

#ifndef NULL
#define NULL ((void*)0)
#endif

//#define uint32_t                unsigned int
//#define uint16_t                unsigned short
//#define uint8_t                 unsigned char
//
//#define int32_t                 signed int
//#define int16_t                 signed short
//#define int8_t                  signed char


/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
  #define     __I     volatile           /*!< defines 'read only' permissions                 */
#else
  #define     __I     volatile const     /*!< defines 'read only' permissions                 */
#endif
#define     __O     volatile             /*!< defines 'write only' permissions                */
#define     __IO    volatile             /*!< defines 'read / write' permissions              */

#define TIM_ADDRESS     0xd1000000
#define GNSS_IMAGE_ID   0x47505349
#define OSLO            0x4F534C4F
#define SLTS            0x534C5453
#define ATES            0x41544553
#define MSAI            0x4D534149
#define SGLI            0x53474C49

#endif /* __INC_PREDEFINES_H */
