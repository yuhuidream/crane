#ifndef __SCU_REG_H__
#define __SCU_REG_H__

typedef struct
{
   __IO uint32_t CONTROL;         /* 0x000 SCU Control Register (R/W) */
   __IO uint32_t CONFIG;          /* 0x004 SCU Control Register (RO) */
   __IO uint32_t STATUS;          /* 0x008 SCU CPU Status Register (R/W) */
   __IO uint32_t INV_ALL;         /* 0x00C SCU Invalidate ALl Register (R/W) */
   __IO uint32_t RESERVED0[(0x40-0xC)/4-1];
   __IO uint32_t FILTER_START;    /* 0x40 Filtering Start Address Register */
   __IO uint32_t FILTER_END;      /* 0x44 Filtering End Address Register */
   __IO uint32_t RESERVED1[(0x50-0x44)/4-1];
   __IO uint32_t SAC;             /* 0x50 SCU Access Control (SAC) Register */
   __IO uint32_t SNSAC;           /* 0x54 SCU Non-secure Access Control (SNSAC) Register */
}SCU_TypeDef;

#define    SCU_BASE      0xD1DFE000
#define    SCU  (( SCU_TypeDef *) SCU_BASE )

#endif
