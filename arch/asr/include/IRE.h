#ifndef __IRE_H__
#define __IRE_H__


typedef struct
{
   __IO uint32_t CTRL0 ;//0x0000 OFFSET 
   __IO uint32_t CTRL1 ;//0x0004 OFFSET 
   __IO uint32_t RESERVED0[2];
   __IO uint32_t SRC_Y0_ADDR;//0x0010 OFFSET
   __IO uint32_t SRC_Y1_ADDR;
   __IO uint32_t SRC_Y2_ADDR;
   __IO uint32_t SRC_U0_ADDR;
   __IO uint32_t SRC_U1_ADDR;
   __IO uint32_t SRC_U2_ADDR;
   __IO uint32_t SRC_V0_ADDR;
   __IO uint32_t SRC_V1_ADDR;
   __IO uint32_t SRC_V2_ADDR;//0x0030 OFFSET
   __IO uint32_t SIZE;         
   __IO uint32_t PRE_PITCH;                           
   __IO uint32_t DST_Y0_ADDR;//0x003c OFFSET
   __IO uint32_t DST_Y1_ADDR;
   __IO uint32_t DST_Y2_ADDR;
   __IO uint32_t DST_U0_ADDR;
   __IO uint32_t DST_U1_ADDR;
   __IO uint32_t DST_U2_ADDR;
   __IO uint32_t DST_V0_ADDR;
   __IO uint32_t DST_V1_ADDR;
   __IO uint32_t DST_V2_ADDR;//0x005C OFFSET
   __IO uint32_t POST_PITCH;//0x0060 OFFSET
   __IO uint32_t RESERVED1[3];
   __IO uint32_t SRAM_CTRL; //0x0070 OFFSET
   __IO uint32_t RESERVED2[3];
   __IO uint32_t IRQ_RAW;  //0x80 OFFSET
   __IO uint32_t IRQ_MASK;
   __IO uint32_t IRQ_STAT;//0x88 OFFSET
   __IO uint32_t RESERVED3[(0x100-0x88)/4-1];
   __IO uint32_t LE_SRC_ADDR;//0X100 OFFSET
   __IO uint32_t LE_LASTID_ADDR;
   __IO uint32_t LE_GETID_ADDR;
   __IO uint32_t LE_PUTID_ADDR;



}IRE_TypeDef;


#define    IRE_BASE      0xD420C000


#define    IRE  (( IRE_TypeDef *) IRE_BASE )

//Control Register 0
#define CTRL0_DMA_POSTED_SEL   BIT_29
#define CTRL0_LE_ENA          BIT_28
#define CTRL0_FRM_NUM_CTRL   BIT_27
#define CTRL0_OUTSTD_WR_ENA   BIT_23
#define CTRL0_RAM_PDWN   BIT_18
#define CTRL0_EN           BIT_0
#define CTRL0_ENDIS_MODE_MSK   SHIFT30(0x3)      
#define CTRL0_ENDIS_MODE_BASE    30 
#define CTRL0_YCBCR_422_ROT_OUT_FMT_MSK   SHIFT21(0x3)      
#define CTRL0_YCBCR_422_ROT_OUT_FMT_BASE    21 
#define CTRL0_NUMFRM_MSK   SHIFT19(0x3)      
#define CTRL0_NUMFRM_BASE    19 
#define CTRL0_HRDY_WCNT_MSK   SHIFT16(0x3)      
#define CTRL0_HRDY_WCNT_BASE    16 
#define CTRL0_HRDY_RCNT_MSK   SHIFT14(0x3)      
#define CTRL0_HRDY_RCNT_BASE    14 
#define CTRL0_YCBCR_END_MSK   SHIFT9(0x3)      
#define CTRL0_YCBCR_END_BASE    9 
#define CTRL0_YCBCR_FMT_MSK   SHIFT7(0x3)      
#define CTRL0_YCBCR_FMT_BASE    7 
#define CTRL0_ROT_MODE_MSK   SHIFT5(0x3)      
#define CTRL0_ROT_MODE_BASE    5 
#define CTRL0_ANGLE_MSK   SHIFT11(0x7)      
#define CTRL0_ANGLE_BASE    11 
#define CTRL0_BPP_MSK   SHIFT2(0x7)      
#define CTRL0_BPP_BASE    2 

//Size Register
#define SIZE_HEIGHT_MSK      SHIFT16(0xfff)
#define SIZE_HEIGHT_BASE     16
#define SIZE_WIDTH_MSK      SHIFT0(0xfff)
#define SIZE_WIDTH_BASE     0

//IRE_PRE_PITCH
#define PRE_PITCH_CBCR_PREPITCH_MSK       SHIFT16(0x3fff)
#define PRE_PITCH_CBCR_PREPITCH_BASE          16
#define PRE_PITCH_Y_PREPITCH_MSK       SHIFT0(0x3fff)
#define PRE_PITCH_Y_PREPITCH_BASE          0

//Post-Rotate Pitch Register
#define POST_PITCH_CBCR_POSTPITCH_MSK         SHIFT16(0x3fff)
#define POST_PITCH_CBCR_POSTPITCH_BASE            16
#define POST_PITCH_Y_POSTPITCH_MSK         SHIFT0(0x3fff)
#define POST_PITCH_Y_POSTPITCH_BASE           0


//SRAM Control
#define SRAM_CTRL_SRAM7_RTC_MSK             SHIFT30(0x3)
#define SRAM_CTRL_SRAM7_RTC_BASE                30
#define SRAM_CTRL_SRAM7_WTC_MSK             SHIFT28(0x3)
#define SRAM_CTRL_SRAM7_WTC_BASE                28
#define SRAM_CTRL_SRAM6_RTC_MSK             SHIFT26(0x3)
#define SRAM_CTRL_SRAM6_RTC_BASE                26
#define SRAM_CTRL_SRAM6_WTC_MSK             SHIFT24(0x3)
#define SRAM_CTRL_SRAM6_WTC_BASE                24
#define SRAM_CTRL_SRAM5_RTC_MSK             SHIFT22(0x3)
#define SRAM_CTRL_SRAM5_RTC_BASE                22
#define SRAM_CTRL_SRAM5_WTC_MSK             SHIFT20(0x3)
#define SRAM_CTRL_SRAM5_WTC_BASE                20
#define SRAM_CTRL_SRAM4_WTC_MSK             SHIFT18(0x3)
#define SRAM_CTRL_SRAM4_WTC_BASE                18
#define SRAM_CTRL_SRAM4_RTC_MSK             SHIFT16(0x3)
#define SRAM_CTRL_SRAM4_RTC_BASE                16
#define SRAM_CTRL_SRAM3_WTC_MSK             SHIFT14(0x3)
#define SRAM_CTRL_SRAM3_WTC_BASE                14
#define SRAM_CTRL_SRAM3_RTC_MSK             SHIFT12(0x3)
#define SRAM_CTRL_SRAM3_RTC_BASE                12
#define SRAM_CTRL_SRAM2_WTC_MSK             SHIFT10(0x3)
#define SRAM_CTRL_SRAM2_WTC_BASE                10
#define SRAM_CTRL_SRAM2_RTC_MSK             SHIFT8(0x3)
#define SRAM_CTRL_SRAM2_RTC_BASE                8
#define SRAM_CTRL_SRAM1_WTC_MSK             SHIFT6(0x3)
#define SRAM_CTRL_SRAM1_WTC_BASE                6
#define SRAM_CTRL_SRAM1_RTC_MSK             SHIFT4(0x3)
#define SRAM_CTRL_SRAM1_RTC_BASE                4
#define SRAM_CTRL_SRAM0_WTC_MSK             SHIFT2(0x3)
#define SRAM_CTRL_SRAM0_WTC_BASE                2
#define SRAM_CTRL_SRAM0_RTC_MSK             SHIFT0(0x3)
#define SRAM_CTRL_SRAM0_RTC_BASE                0



//IRQ RAW Register
#define IRQ_RAW_AXI_WR_ERR_IRQR         BIT_6
#define IRQ_RAW_AXI_RD_ERR_IRQR         BIT_5
#define IRQ_RAW_EOF2_IRQR               BIT_2
#define IRQ_RAW_EOF1_IRQR               BIT_1
#define IRQ_RAW_EOF0_IRQR               BIT_0


//IRQ MASK Register
#define IRQ_MASK_AXI_WR_ERR_IRQW         BIT_6
#define IRQ_MASK_AXI_RD_ERR_IRQW         BIT_5
#define IRQ_MASK_EOF2_IRQW               BIT_2
#define IRQ_MASK_EOF1_IRQW               BIT_1
#define IRQ_MASK_EOF0_IRQW               BIT_0


//IRQ STAT Register
#define IRQ_STAT_AXI_WR_ERR_IRQW         BIT_6
#define IRQ_STAT_AXI_RD_ERR_IRQW         BIT_5
#define IRQ_STAT_EOF2_IRQW               BIT_2
#define IRQ_STAT_EOF1_IRQW               BIT_1
#define IRQ_STAT_EOF0_IRQW               BIT_0



#endif
