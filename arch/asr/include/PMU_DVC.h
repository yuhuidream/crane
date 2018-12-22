#ifndef __PMU_DVC_H__
#define __PMU_DVC_H__


typedef struct
{
    __IO uint32_t DVCR;                                //0x00
    __IO uint32_t VL01STR;                           //0x04
    __IO uint32_t VL12STR;                           //0x08
    __IO uint32_t VL23STR;                           //0x0C
    __IO uint32_t VL34STR;                           //0x10
    __IO uint32_t VL45STR;                           //0x14
    __IO uint32_t VL56STR;                           //0x18
    __IO uint32_t VL67STR;                           //0x1C
    __IO uint32_t DVC_AP;                            //0x20
    __IO uint32_t DVC_CP;                            //0x24
    __IO uint32_t DVC_DP;                           //0x28
    __IO uint32_t DVC_APSUB;                           //0x2C 
    __IO uint32_t DVC_CHIP;                               //0x30
    __IO uint32_t RESERVED1[(0x40-0x30)/4-1];
    __IO uint32_t DVC_STATUS;                           //0x40
    __IO uint32_t RESERVED2[(0x50-0x40)/4-1];     
    __IO uint32_t DVC_IMR;                                  //0x50
    __IO uint32_t DVC_ISR;                              //0x54
    __IO uint32_t DVC_DEBUG;                       //0x58
    __IO uint32_t DVC_EXTRA_STR;                //0x5C

}PMU_DVC_TypeDef;

#define    PMU_DVC_BASE       0xd4052000

#define    PMU_DVC  ((  PMU_DVC_TypeDef *) PMU_DVC_BASE )

#endif
