#ifndef __THSESOR_H__
#define __THSESOR_H__

typedef struct
{

    __IO uint32_t PCTRL;                 //0x0 Pin control register
    __IO uint32_t LCTRL;                 //0x4 Logic control register
    __IO uint32_t PSTATUS;               //0x8 Pin status register
    __IO uint32_t LSTATUS;               //0xC Logic control register
    __IO uint32_t RSTATUS;               //0x10 RST status register
    __IO uint32_t THD01;                 //0x14 threshold 01 register
    __IO uint32_t THD23;                 //0x18 threshold 23 register

}THSESOR_TypeDef;


#define    THSESOR_BASE      0xD4013300


#define    TSEN  (( THSESOR_TypeDef *) THSESOR_BASE )

/* TSEN_PCTRL */
#define TSEN_CHOP_EN_BASE    (14)
#define TSEN_CHOP_EN_MASK    (0x3)
#define TSEN_ISO_EN          BIT_3
#define TSEN_EN              BIT_2
#define TSEN_START           BIT_1
#define TSEN_RESET           BIT_0

/* TSEN_LCTRL */
#define TSEN_AUTO_INTERVAL_BASE     (16)
#define TSEN_AUTO_INTERVAL_MASK      0xFFFF
#define TSEN_RDY_INT_ENABLE          BIT_11
#define TSEN_CKN_BIT                 BIT_10
#define TSEN_WDT_DIRECTION           BIT_9
#define TSEN_WDT_ENABLE              BIT_8
#define TSEN_INT2_DIRECTION          BIT_7
#define TSEN_INT2_ENABLE             BIT_6
#define TSEN_INT1_DIRECTION          BIT_5
#define TSEN_INT1_ENABLE             BIT_4
#define TSEN_INT0_DIRECTION          BIT_3
#define TSEN_INT0_ENABLE             BIT_2
#define TSEN_AUTO_MODE_BASE          (0)
#define TSEN_AUTO_MODE_MASK          (0x3)

/* TSEN_PSTATUS */
#define TSEN_RDY                     BIT_24
#define TSEN_DATA_RAW_BASE           (12)
#define TSEN_DATA_RAW_MASK           (0xFFF)
#define TSEN_DATA_BASE               (0)
#define TSEN_DATA_MASK               (0xFFF)

/* TSEN_LSTATUS */
#define TSEN_INT2                     BIT_15
#define TSEN_INT1                     BIT_14
#define TSEN_INT0                     BIT_13
#define TSEN_RDY_INT                  BIT_12

/* TSEN_RSTATUS */
#define TSEN_WDT_FLAG                (12)
#define TSEN_DATA_WDT_BASE           (0)
#define TSEN_DATA_WDT_MASK           (0xFFF)

/* TSEN_THD01 */
#define TSEN_THD1_BASE               (12)
#define TSEN_THD1_MASK               (0xFFF)
#define TSEN_THD0_BASE               (0)
#define TSEN_THD0_MASK               (0xFFF)

/* TSEN_THD23 */
#define TSEN_WDT_THD_BASE            (12)
#define TSEN_WDT_THD_MASK            (0xFFF)
#define TSEN_THD2_BASE               (0)
#define TSEN_THD2_MASK               (0xFFF)



#endif
