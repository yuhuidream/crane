#ifndef __MSAEIC_H__
#define __MSAEIC_H__

typedef struct
{

    __IO uint32_t      PISMR;
    __IO uint32_t      PICER;
    __IO uint32_t      SISMR_H;
    __IO uint32_t      SISMR_L;
    __IO uint32_t      SICER_H;
    __IO uint32_t      SICER_L;
    __IO uint32_t      SICR_H;
    __IO uint32_t      SICR_L;
    __IO uint32_t      SIGR_H;
    __IO uint32_t      SIGR_L;
    __IO uint32_t      SIOR_H;
    __IO uint32_t      SIOR_L;
    __IO uint32_t      SIOCR_H;
    __IO uint32_t      SIOCR_L;
    __IO uint32_t      SISRR_H;
    __IO uint32_t      SISRR_L;
    __IO uint32_t      DWR_H;
    __IO uint32_t      DWR_L;//0X44
    __IO uint32_t      RESERVED0[2];
    __IO uint32_t      IBR;



} MSAEIC_TypeDef;




#define    MSAEIC_BASE        0xFFD05000


#define     MSAEIC  ((  MSAEIC_TypeDef *) MSAEIC_BASE )












#endif
