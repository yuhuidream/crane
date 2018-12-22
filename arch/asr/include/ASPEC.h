#ifndef __ASPEC_H__
#define __ASPEC_H__

typedef struct
{

    __IO uint32_t      SW_RST;
    __IO uint32_t      VD_CTL;
    __IO uint32_t      VD_BSZ;
    __IO uint32_t      VD_MPE01;
    __IO uint32_t      VD_MPE23;
    __IO uint32_t      VD_RSCM;
    __IO uint32_t      VD_CFL;
    __IO uint32_t      VD_CRCI;
    __IO uint32_t      VD_CRCM;
    __IO uint32_t      VD_TBST;
    __IO uint32_t      VD_OBRA;
    __IO uint32_t      VD_RSM;
    __IO uint32_t      VD_IBR;
    __IO uint32_t      VD_MRLR;
    __IO uint32_t      VD_MRIDZSM;
    __IO uint32_t      VD_MDL;
    __IO uint32_t      VD_CRCR;
    __IO uint32_t      VD_STATUS;//0X44
    __IO uint32_t      RESERVED0[(0x80-0x44)/4-1];
    __IO uint32_t      ASPEC_OB_START;//0X80



} ASPEC_TypeDef;




#define    ASPEC_BASE       0xFFA01000


#define     ASPEC  ((  ASPEC_TypeDef *) ASPEC_BASE )

//

#endif




