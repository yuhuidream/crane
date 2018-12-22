#ifndef __MSA_BASE_H__
#define __MSA_BASE_H__

typedef struct
{
    __IO uint32_t     SMMLR;
    __IO uint32_t     SDTESR;
    __IO uint32_t     SDTEAR;//0X008
    __IO uint32_t     RESERVED0[(0x1000-0x0008)/4-1];
    __IO uint32_t     PBRCR;//0X1000
    __IO uint32_t     PBDESR;
    __IO uint32_t     RESERVED1[(0x2000-0x1004)/4-1];
    __IO uint32_t     AWCR;
    __IO uint32_t     AWTCR;
    __IO uint32_t     AAHCR;
 


} MSA_BASE_TypeDef;




#define    MSA_BASE_BASE        0xFFD06000


#define     MSA_BASE  ((  MSA_BASE_TypeDef *) MSA_BASE_BASE )


#endif
