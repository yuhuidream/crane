#ifndef __DTC_H__
#define __DTC_H__

typedef struct
{
  
    __IO uint32_t  DTC_MPR_HIGHn_ADDR;
    __IO uint32_t  DTC_MPR_LOWn_ADDR;

}DTC_MPR_ADDR;






typedef struct
{
    __IO uint32_t          DTC_DOCR_CSA0;
    __IO uint32_t          DTC_DOCR_CTRL0;
    __IO uint32_t          DTC_DOCR_CSA1;
    __IO uint32_t          DTC_DOCR_CTRL1;
    __IO uint32_t          DTC_DOCR_SW_BYPASS;
    __IO uint32_t          RESERVED0[3];
union{
    __IO uint32_t          DTC_F8_DTCCR_NDP;
    __IO uint32_t          DTC_F9_DTCCR_NDP;
};
union{
    __IO uint32_t          DTC_F8_DTCCR_SA;
    __IO uint32_t          DTC_F9_DTCCR_SA;
 };
union{
   __IO uint32_t          DTC_F8_DTCCR_DA;
   __IO uint32_t          DTC_F9_DTCCR_DA;
   };
union{
   __IO uint32_t          DTC_F8_DTCCR_CONTROL;
   __IO uint32_t          DTC_F9_DTCCR_CONTROL;
   };
union{
   __IO uint32_t          DTC_F8_DTCCR_HEADER_DATA;
   __IO uint32_t          DTC_F9_DTCCR_HEADER_DATA;
    };

union{
   __IO uint32_t          DTC_F8_DTCCR_BIT_OFFSET;
   __IO uint32_t          DTC_F9_DTCCR_BIT_OFFSET;

   };
union{
   __IO uint32_t          DTC_F8_DTCCR_COUNT_C;
   __IO uint32_t          DTC_F9_DTCCR_COUNT_C;
    };
union{
   __IO uint32_t          DTC_F8_DTCCR_CK_ADDR;
   __IO uint32_t          DTC_F9_DTCCR_CK_ADDR;
    };//0x003c
    __IO uint32_t          RESERVED1[(0x0050-0x003C)/4-1];
    __IO uint32_t          DTC_CKIK_1;//0x0050
    __IO uint32_t          DTC_CKIK_2;
    __IO uint32_t          DTC_CKIK_3;
    __IO uint32_t          DTC_CKIK_4;
    __IO uint32_t          DTC_DISR_REG;
    __IO uint32_t          RECERVED2[3];
    __IO uint32_t          DTC_MAC_I0_REG;
    __IO uint32_t          DTC_F9_MAC_I1_REG;
    __IO uint32_t          RECERVED3[2];
    __IO uint32_t          DTC_MPR_ADDR dtc_MPR_ADDR[3];//0X80-0X94
    __IO uint32_t          RECERVED4[2];
    __IO uint32_t          DTC_READ_REG;
    __IO uint32_t          DTC_WRITE_REG;
    __IO uint32_t          RESERVED5[(0xC0-0xA4)/4-1];
union{
    __IO uint32_t          DTC_FIFO_READ;
    __IO uint32_t          DTC_FIFO_WRITE;
};


}DTC_TypeDef;


#define    DTC_BASE       0xD4284000


#define    DTC  (( DTC_TypeDef *)DTC_BASE )


//DTC Control Register 0/1
#define    DTC_DOCR_CTRL0_1_FIFO_OUT_AF_SIZE_MSK                      SHIFT25(0xf)                     
#define    DTC_DOCR_CTRL0_1_FIFO_OUT_AF_SIZE_BASE                      25
#define    DTC_DOCR_CTRL0_1_FIFO_OUT_AE_SIZE_MSK                   SHIFT21(0xf)                    
#define    DTC_DOCR_CTRL0_1_FIFO_OUT_AE_SIZE_BASE                      21
#define    DTC_DOCR_CTRL0_1_FIFO_IN_AF_SIZE_MSK                       SHIFT17(0xf)
#define    DTC_DOCR_CTRL0_1_FIFO_IN_AF_SIZE_BASE                       17
#define    DTC_DOCR_CTRL0_1_FIFO_IN_AE_SIZE_MSK                      SHIFT13(0xf) 
#define    DTC_DOCR_CTRL0_1_FIFO_IN_AE_SIZE_BASE                       13
#define    DTC_DOCR_CTRL0_1_CHn_Start                                      BIT_5
#define    DTC_DOCR_CTRL0_1_Done Indication                       BIT_4
#define    DTC_DOCR_CTRL0_1_CHn_CEE_MSK                       SHIFT2(0x3)
#define    DTC_DOCR_CTRL0_1_CHn_CEE_BASE                      2
#define    DTC_DOCR_CTRL0_1_CHn_F8/F9                      BIT_1
#define    DTC_DOCR_CTRL0_1_CHn_DIR                       BIT_0


//DTC Software Bypass Register
#define    DTC_DOCR_SW_BYPASS_BNDRY_EN                           BIT_12
#define    DTC_DOCR_SW_BYPASS_CK_BE_LE_SEL                           BIT_11
#define    DTC_DOCR_SW_BYPASS_SW_OVR                           BIT_10
#define    DTC_DOCR_SW_BYPASS_HW_EN                           BIT_9
#define    DTC_DOCR_SW_BYPASS_TEST_EN                           BIT_8
#define    DTC_DOCR_SW_BYPASS_TEST_MUX_SEL_MSK                            SHIFT5(0x7)
#define    DTC_DOCR_SW_BYPASS_TEST_MUX_SEL_BASE                           5
#define    DTC_DOCR_SW_BYPASS_EHT                           BIT_4
#define    DTC_DOCR_SW_BYPASS_EHT_EN                           BIT_3
#define    DTC_DOCR_SW_BYPASS_SWM                           BIT_2
#define    DTC_DOCR_SW_BYPASS_SSC                           BIT_1
#define    DTC_DOCR_SW_BYPASS_SSM                           BIT_0


//F8 Control Register
#define   DTC_F8_DTCCR_CONTROL_F8_RCS                           BIT_30
#define   DTC_F8_DTCCR_CONTROL_F8_CB                            BIT_29
#define   DTC_F8_DTCCR_CONTROL_F8_CONT                            BIT_28
#define   DTC_F8_DTCCR_CONTROL_F8_CO                            SHIFT24(0x7)
#define   DTC_F8_DTCCR_CONTROL_F8_CO                              24
#define   DTC_F8_DTCCR_CONTROL_F8_BEARER                            SHIFT16(0x3f)
#define   DTC_F8_DTCCR_CONTROL_F8_BEARER                            16
#define   DTC_F8_DTCCR_CONTROL_F8_LEN                            SHIFT0(0xffff)
#define   DTC_F8_DTCCR_CONTROL_F8_LEN                            0



//F8 Header Register
#define   DTC_F8_DTCCR_HEADER_DATA_F8_HDR_SIZE_MSK                   SHIFT28(0xf)
#define   DTC_F8_DTCCR_HEADER_DATA_F8_HDR_SIZE_BASE                        28
#define   DTC_F8_DTCCR_HEADER_DATA_F8_HDR_DATA_MSK                    SHIFT0(0x7fffff)
#define   DTC_F8_DTCCR_HEADER_DATA_F8_HDR_DATA_BASE                      0


//F8 Bit Offset Register
#define   DTC_F8_DTCCR_BIT_OFFSET_F8_IBO_MSK                       SHIFT16(0xffff)
#define   DTC_F8_DTCCR_BIT_OFFSET_F8_IBO_BASE                          16
#define   DTC_F8_DTCCR_BIT_OFFSET_F8_OBO_MSK                         SHIFT0(0xffff)
#define   DTC_F8_DTCCR_BIT_OFFSET_F8_OBO_BASE                      0


//DTC Interrupt Status Register
#define   DTC_DISR_REG_FOF_FIFO_Out_Full                                  BIT_5
#define   DTC_DISR_REG_MP_ERR1                                                BIT_4
#define   DTC_DISR_REG_MP_ERR0                                                BIT_3
#define   DTC_DISR_REG_MERR1                                                BIT_2
#define   DTC_DISR_REG_MERR0                                                BIT_1
#define   DTC_DISR_REG_SERR                                                BIT_0





#endif










