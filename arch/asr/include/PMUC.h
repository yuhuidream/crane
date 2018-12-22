#ifndef __PMUC_H__
#define __PMUC_H__


typedef struct
{
    __IO uint32_t CCCR;
    __IO uint32_t CCSR;
    __IO uint32_t GCCCR;
    __IO uint32_t RESERVED0[(0x0100-0x0008)/4-1];
    __IO uint32_t MPCR;
    __IO uint32_t RESERVED1[(0x0114-0x0100)/4-1];
    __IO uint32_t CMCR;
    __IO uint32_t CPSR;
    __IO uint32_t RESERVED2[(0x0130-0x118)/4-1];
    __IO uint32_t CIER;
    __IO uint32_t RESERVED3[3];
    __IO uint32_t CGDES;
    __IO uint32_t RESERVED4[(0x0200-0x0140)/4-1];
    __IO uint32_t CSSR;


}PMUC_TypeDef;

#define    PMUC_BASE       0xF00E0000

#define     PMUC  ((  PMUMCCU_TypeDef *) PMUC_BASE )


#endif
