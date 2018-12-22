#ifndef __MSADMA_H__
#define __MSADMA_H__
typedef struct
{
    __IO uint32_t     CSAR_PRI;
    __IO uint32_t     CDAR_PRI;
    __IO uint32_t     CBCR_PRI;
    __IO uint32_t     CCR_PRI;


}DMA_AR;





typedef struct
{
    __IO uint32_t      DCR;
    __IO uint32_t      RESERVED0;
    __IO uint32_t      DESR;
    __IO uint32_t      RESERVED1;
    __IO uint32_t      DMA_AR  dma_AR[8];//0X10-0X8C
    __IO uint32_t      CSAR_SEC[4];
    __IO uint32_t      CDAR_SEC[4];
    __IO uint32_t      CBCR_SEC[4];
    __IO uint32_t      CCR_SEC[4];
 


} MSADMA_TypeDef;




#define    MSADMA1_BASE        0xFFD01000
#define    MSADMA2_BASE        0xFFD02000

#define     MSADMA1  ((  MSADMA_TypeDef *) MSADMA1_BASE )

#define     MSADMA2  ((  MSADMA_TypeDef *) MSADMA2_BASE )







#endif
