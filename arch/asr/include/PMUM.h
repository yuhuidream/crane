#ifndef __PMUM_H__
#define __PMUM_H__

typedef struct
{
    __IO uint32_t CPCR;
    __IO uint32_t CPSR;
    __IO uint32_t FCCR;
    __IO uint32_t POCR;
    __IO uint32_t POSR;
    __IO uint32_t SUCCR;
    __IO uint32_t VRCR;
    __IO uint32_t RESERVED0;
    __IO uint32_t CPRR;
    __IO uint32_t CCGR;
    __IO uint32_t CRSR;
    __IO uint32_t XDCR;
    __IO uint32_t GPCR;
    __IO uint32_t PLL2CR;
    __IO uint32_t SCCR;    //0x0038
    __IO uint32_t MCCR;
    __IO uint32_t ISCCRX[2];
    __IO uint32_t CWUCRS;
    __IO uint32_t CWUCRM;  //0X004C
    __IO uint32_t RESERVED1[(0x100-0x4c)/4-1];
    __IO uint32_t DSOC;//0X100
    __IO uint32_t RESERVED2[(0x200-0x100)/4-1];
    __IO uint32_t WDTPCR;//0X200
    __IO uint32_t RESERVED3[(0x400-0x200)/4-1];
    __IO uint32_t CMPRX[5];//0X400
    __IO uint32_t RESERVED5[(0x1000-0x410)/4-1];
    __IO uint32_t APCR;//0X1000
    __IO uint32_t APSR;//0X1004
    __IO uint32_t RESERVED6[(0x1020-0x1004)/4-1];
    __IO uint32_t APRR;//0x1020
    __IO uint32_t ACGR;
    __IO uint32_t ARSR;//0X1028
    __IO uint32_t RESERVED7[(0x20)/4-1];
    __IO uint32_t AWUCRS;//0X1048
    __IO uint32_t AWUCRM;
} PMUM_TypeDef;


#define    PMUM_BASE      0xD4050000
#define    PMUM  ((PMUM_TypeDef *) PMUM_BASE )

#endif
