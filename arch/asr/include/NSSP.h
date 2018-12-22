#ifndef __SSP_H__
#define __SSP_H__

typedef struct{
    __IO uint32_t TOP_CTRL;
    __IO uint32_t FIFO_CTRL;
    __IO uint32_t INT_EN;    //0x8
    __IO uint32_t TO;        //0xc
    __IO uint32_t DATAR;     //0x10
    __IO uint32_t STATUS;    //0x14
    __IO uint32_t PSP_CTRL;   //0x18
    __IO uint32_t NETWORK_CTRL;   //0x1C
    __IO uint32_t NETWORK_STATUS;   //0x20
    __IO uint32_t RWOT_CTRL;        //0x24
    __IO uint32_t RWOT_CCM;         //0x28
    __IO uint32_t RWOT_CVWRn;       //0x2c
}SSP_TypeDef;

#define    SSP0_BASE      0xD401B000
#define    SSP0  (( SSP_TypeDef *) SSP0_BASE )

#define    SSP1_BASE      0xD42A0C00
#define    SSP1  (( SSP_TypeDef *) SSP1_BASE )

#define    SSP2_BASE      0xD401C000
#define    SSP2  (( SSP_TypeDef *) SSP2_BASE )

#define    GSSP_BASE      0xD4039000
#define    GSSP  (( SSP_TypeDef *) GSSP_BASE ) 

/* bits definitions for register REG_SSP_TOP_CTRL */
#define SSP_TTELP                                         ( BIT_18 )
#define SSP_TTE                                           ( BIT_17 )
#define SSP_SCFR                                          ( BIT_16 )
#define SSP_IFS                                           ( BIT_15 )
#define SSP_HOLD_FRAME_LOW                                ( BIT_14 )
#define SSP_TRAIL                                         ( BIT_13 )
#define SSP_LBM                                           ( BIT_12 )
#define SSP_SPH                                           ( BIT_11 )
#define SSP_SPO                                           ( BIT_10 )
#define SSP_DSS(_X_)                                     ( (_X_) << 5 & (BIT_5|BIT_6|BIT_7|BIT_8|BIT_9) )
#define SSP_SFRMDIR                                       ( BIT_4 )
#define SSP_SCLKDIR                                       ( BIT_3 )
#define SSP_FRF(_X_)                                     ( (_X_) << 1 & (BIT_1 |BIT_2) )
#define SSP_SSE                                           ( BIT_0 )


/* bits definitions for register REG_SSP_FIFO_CTRL */
#define SSP_STRF                                          ( BIT_19 )
#define SSP_EFWR                                          ( BIT_18 )
#define SSP_RXFIFO_AUTO_FULL_CTRL                         ( BIT_17 )
#define SSP_FPCKE                                         ( BIT_16 )
#define SSP_TXFIFO_WR_ENDIAN(_X_)                        ( (_X_) << 14 & (BIT_14|BIT_15) )
#define SSP_RXFIFO_RD_ENDIAN(_X_)                        ( (_X_) << 12 & (BIT_12|BIT_13) )
#define SSP_RSRE                                          ( BIT_11 )
#define SSP_TSRE                                          ( BIT_10 )
#define SSP_RFT(_X_)                                     ( (_X_) << 5 & (BIT_5|BIT_6|BIT_7|BIT_8|BIT_9) )
#define SSP_TFT(_X_)                                     ( (_X_) & (BIT_0|BIT_1|BIT_2|BIT_3|BIT_4) )

/* bits definitions for register REG_SSP_STATUS */
#define SSP_OSS                                           ( BIT_23 )
#define SSP_TX_OSS                                        ( BIT_22 )
#define SSP_BCE                                           ( BIT_21 )
#define SSP_ROR                                           ( BIT_20 )
#define SSP_RFL(_X_)                                     ( (_X_) << 15 & (BIT_15|BIT_16|BIT_17|BIT_18|BIT_19) )
#define SSP_SSSR_RFL_MSK                                      (0x1f << 15)  /* Receive FIFO Level */
#define SSP_RNE                                           ( BIT_14 )
#define SSP_RFS                                           ( BIT_13 )
#define SSP_TUR                                           ( BIT_12 )
#define SSP_TFL(_X_)                                     ( (_X_) << 7 & (BIT_7|BIT_8|BIT_9|BIT_10|BIT_11) )
#define SSP_SSSR_TFL_MSK                                      (0x1f << 7)   /* Transmit FIFO Level */
#define SSP_TNF                                           ( BIT_6 )
#define SSP_TFS                                           ( BIT_5 )
#define SSP_EOC                                           ( BIT_4 )
#define SSP_TINT                                          ( BIT_3 )
#define SSP_PINT                                          ( BIT_2 )
#define SSP_CSS                                           ( BIT_1 )
#define SSP_BSY                                           ( BIT_0 )

/* bits definitions for register REG_SSP_PSP_CTRL */
#define SSP_EDMYSTOP(_X_)                                ( (_X_) << 27 & (BIT_27|BIT_28|BIT_29) )
#define SSP_DMYSTOP(_X_)                                 ( (_X_) << 25 & (BIT_25|BIT_26) )
#define SSP_EDMYSTRT(_X_)                                ( (_X_) << 23 & (BIT_23|BIT_24) )
#define SSP_DMYSTRT(_X_)                                 ( (_X_) << 21 & (BIT_21|BIT_22) )
#define SSP_STRTDLY(_X_)                                 ( (_X_) << 18 & (BIT_18|BIT_19|BIT_20) )
#define SSP_SFRMWDTH(_X_)                                ( (_X_) << 12 & (BIT_12|BIT_13|BIT_14|BIT_15|BIT_16|BIT_17) )
#define SSP_SSPSP_SFRMWDTH_BASE                               12
#define SSP_SSPSP_SFRMWDTH_MSK                                (BIT_12|BIT_13|BIT_14|BIT_15|BIT_16|BIT_17)

#define SSP_SFRMDLY(_X_)                                 ( (_X_) << 5 & (BIT_5|BIT_6|BIT_7|BIT_8|BIT_9|BIT_10|BIT_11) )
#define SSP_SFRMP                                         ( BIT_4 )
#define SSP_FSRT                                          ( BIT_3 )
#define SSP_ETDS                                          ( BIT_2 )
#define SSP_SCMODE(_X_)                                  ( (_X_) & (BIT_0|BIT_1) )
#endif