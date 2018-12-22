#ifndef __MSAHSL_H__
#define __MSAHSL_H__

typedef struct
{

    __IO uint32_t  LASTDATA;
    __IO uint32_t  DATA;
    __IO uint32_t  HEADER;
    __IO uint32_t  SINGLE;
    __IO uint32_t  CFG;//0X10
    __IO uint32_t  RESERVED0[3];
    __IO uint32_t  SWENABLE0;//0X20
    __IO uint32_t  SWENABLE1;
    __IO uint32_t  SWENABLE2;
    __IO uint32_t  SWENABLE3;
    __IO uint32_t  TCUENABLE;
    __IO uint32_t  STACK;




} MSAHSL_TypeDef;




#define    MSAHSL_BASE       0xFFC0CD00


#define     MSAHSL  ((  MSAHSL_TypeDef *) MSAHSL_BASE )




















#endif
