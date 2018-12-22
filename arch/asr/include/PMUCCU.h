#ifndef __PMUCCU_H__
#define __PMUCCU_H__


typedef struct
{
    __IO uint32_t CCCR;
    __IO uint32_t CCSR;
    __IO uint32_t GCCCR;
    __IO uint32_t RESERVED0[(0x0100-0x0008)/4-1];
    __IO uint32_t MPCR;
    __IO uint32_t RESERVED1[(0x0114-0x0100)/4-1];
    __IO uint32_t CMCR;
    __IO uint32_t MPSR;
    __IO uint32_t RESERVED2;
    __IO uint32_t CWER;
    __IO uint32_t RESERVED3;
    __IO uint32_t Cwesr_wakeup_en;
    __IO uint32_t RESERVED4;
    __IO uint32_t CIER;
    __IO uint32_t RESERVED5[3];
    __IO uint32_t CGDES;
    __IO uint32_t RESERVED6[(0x0200-0x0140)/4-1];
    __IO uint32_t CSSR;


}PMUCCU_TypeDef;

#define    PMUCCU_BASE       0xF00E0000

#define     PMUCCU  ((  PMUMCCU_TypeDef *) PMUCCU_BASE )


#endif
