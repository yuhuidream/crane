#ifndef __DIGRERX_H__
#define __DIGRERX_H__


typedef struct
{
    __IO uint32_t    RX_SW_RST;
    __IO uint32_t    RX_DATA_A_SW_RST;
    __IO uint32_t    RX_DATA_BD_SW_RST;
    __IO uint32_t    RX_DATA_C_SW_RST;
    __IO uint32_t    RX_IFC_PRI_SW_RST;
    __IO uint32_t    RX_RFICUS_PRI_SW_RST;
    __IO uint32_t    RX_RFICR_PRI_SW_RST;
    __IO uint32_t    RX_IFC_DIV_SW_RST;
    __IO uint32_t    RX_RFICUS_DIV_SW_RST;
    __IO uint32_t    RX_RFICR_DIV_SW_RST;
    __IO uint32_t    RX_LCT_TBL_PRI;
    __IO uint32_t    RX_LCT_TBL_DIV;
    __IO uint32_t    RX_DATA_A_THD;
    __IO uint32_t    RX_DATA_BD_THD;
    __IO uint32_t    RX_DATA_C_THD;
    __IO uint32_t    RX_DATA_BD_SEL;
    __IO uint32_t    RX_DATA_DMA_SKIP;
    __IO uint32_t    RX_MODES;
    __IO uint32_t    RX_2_RFIC_SEL;
    __IO uint32_t    RX_FORCE_PHASE;
    __IO uint32_t    RX_IQ_SWAP;
    __IO uint32_t    RX_LATCH_STATUS;
    __IO uint32_t    RX_RD_STATUS;
    __IO uint32_t    RX_CLR_STICKY;
    __IO uint32_t    RX_SHUTDOWN;//0X0060
    __IO uint32_t    RESERVED0[(0xFC-0x60)/4-1];
    __IO uint32_t    RFICR_PRI_PS[0xFC/4+1];
    __IO uint32_t    IFC_PRI_RD;
    __IO uint32_t    IFC_PRI_LAST;
    __IO uint32_t    RFICUS[4];
union{
    __IO uint32_t    WX_PRI_FIFOS_INT_HANDLE;
    __IO uint32_t    RX_PRI_FIFOS_INT_HANDLE;
 };                                       //0x318
    __IO uint32_t    RESERVED1[(0x400-0x318)/4-1];                    
    __IO uint32_t    RFICR_DIV_PS[0xFC/4+1]; //0X0400
    __IO uint32_t    IFC_DIV_RD;
    __IO uint32_t    IFC_DIV_LAST;
    __IO uint32_t    RFICUS_PRI[4];
union{
    __IO uint32_t    WX_DIV_FIFOS_INT_HANDLE;
    __IO uint32_t    RX_DIV_FIFOS_INT_HANDLE;
 };  








} DIGRERX_TypeDef;




#define    DIGRERX_BASE       0xFFAB0000

#define     DIGRERX  ((  DIGRERX_TypeDef *) DIGRERX_BASE )



#endif


