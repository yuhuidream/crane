#ifndef __MFPR_H__
#define __MFPR_H__

typedef struct
{
   __IO uint32_t RESERVED0;   //0x000
   __IO uint32_t ND_IO15;       //0x004
   __IO uint32_t ND_IO14;       //0x008
   __IO uint32_t ND_IO13;       //0x00C
   __IO uint32_t ND_IO12;       //0x010
   __IO uint32_t ND_IO11;       //0x014
   __IO uint32_t ND_IO10;       //0x018
   __IO uint32_t ND_IO9;       //0x01C
   __IO uint32_t ND_IO8;       //0x020
   __IO uint32_t ND_IO7;       //0x024
   __IO uint32_t ND_IO6;       //0x028
   __IO uint32_t ND_IO5;       //0x02C
   __IO uint32_t ND_IO4;       //0x030
   __IO uint32_t ND_IO3;       //0x034
   __IO uint32_t ND_IO2;       //0x038   
   __IO uint32_t ND_IO1;       //0x03C
   __IO uint32_t ND_IO0;       //0x040
   __IO uint32_t ND_nCS0_SM_nCS2;       //0x044
   __IO uint32_t ND_nCS1_SM_nCS3;       //0x048
   __IO uint32_t SM_nCS0;       //0x04C
   __IO uint32_t SM_nCS1;       //0x050
   __IO uint32_t ND_nWE;       //0x054
   __IO uint32_t ND_nRE;       //0x058
   __IO uint32_t ND_CLE_SM_nOE;       //0x05C
   __IO uint32_t ND_ALE_SM_nWE;       //0x060
   __IO uint32_t SM_SCLK;       //0x064
   __IO uint32_t ND_RDY0;       //0x068
   __IO uint32_t SM_nBE0;       //0x06C
   __IO uint32_t SM_nBE1;       //0x070
   __IO uint32_t SM_ADV;       //0x074
   __IO uint32_t ND_RDY1;       //0x078
   __IO uint32_t SM_ADVMUX;       //0x07C
   __IO uint32_t SM_RDY;       //0x080
   __IO uint32_t MMC1_DAT7;       //0x084
   __IO uint32_t MMC1_DAT6;       //0x088
   __IO uint32_t MMC1_DAT5;       //0x08C
   __IO uint32_t MMC1_DAT4;       //0x090
   __IO uint32_t MMC1_DAT3;       //0x094
   __IO uint32_t MMC1_DAT2;       //0x098
   __IO uint32_t MMC1_DAT1;       //0x09C
   __IO uint32_t MMC1_DAT0;       //0x0A0
   __IO uint32_t MMC1_CMD;       //0x0A4
   __IO uint32_t MMC1_CLK;       //0x0A8
   __IO uint32_t MMC1_CD;       //0x0AC
   __IO uint32_t MMC1_WP;       //0x0B0
   __IO uint32_t PRI_TDI;       //0x0B4
   __IO uint32_t PRI_TMS;       //0x0B8
   __IO uint32_t PRI_TCK;       //0x0BC
   __IO uint32_t PRI_TDO;       //0x0C0
   __IO uint32_t RESERVED1;   //0x0C4
   __IO uint32_t SLAVE_RESET_OUT_N;       //0x0C8
   __IO uint32_t CLK_REQ;       //0x0CC
   __IO uint32_t GPIO_124;       //0x0D0
   __IO uint32_t VCXO_REQ;       //0x0D4
   __IO uint32_t VCXO_OUT;       //0x0D8
   __IO uint32_t GPIO_00[55];       //0x0DC~0x1B4, GPIO[0:54]
   __IO uint32_t GPIO_67[32];       //0x1B8~0x234, GPIO[67:98]
   __IO uint32_t GPIO_100[10];     //0x238~0x25C, GPIO[100:109]
   __IO uint32_t ANT_SW1;       //0x260
   __IO uint32_t ANT_SW2;       //0x264
   __IO uint32_t ANT_SW3;       //0x268
   __IO uint32_t ANT_SW4;       //0x26C
   __IO uint32_t PA_MODE;       //0x270
   __IO uint32_t RF_CONT_4;       //0x274
   __IO uint32_t GSM_STROBE;       //0x278
   __IO uint32_t GSM_CCLK;       //0x27C
   __IO uint32_t GSM_CEN;       //0x280
   __IO uint32_t GSM_CDATA;       //0x284
   __IO uint32_t RXTXEN;       //0x288
   __IO uint32_t RXTXDATA;       //0x28C
   __IO uint32_t RESERVED2;   //0x290
   __IO uint32_t SYSCLKEN;   //0x294
   __IO uint32_t GPIO_110[7];   //0x298~0x2B0, GPIO[110:116]
   __IO uint32_t AFE_RXIQ[8];   //0x2B4~0x2D0, AFE_RXIQ[0:7]
   __IO uint32_t AFE_RX_CLK;   //0x2D4
   __IO uint32_t AFE_TXIQ[10];   //0x2D8~0x2FC, AFE_TXIQ[0:9]
   __IO uint32_t AFE_TX_CLK;   //0x300
   __IO uint32_t CONT_CLK;     //0x304
   __IO uint32_t CONT_DATA0;     //0x308
   __IO uint32_t CONT_DATA1;     //0x30C
   __IO uint32_t CONT_RD_WR;     //0x310
   __IO uint32_t CONT_READ_INT;     //0x314
   __IO uint32_t DAC_ST456;     //0x318
   __IO uint32_t ADC_ST;     //0x31C
   __IO uint32_t USIM_UCLK;     //0x320
   __IO uint32_t USIM_UIO;     //0x324
   __IO uint32_t USIM_nURST;     //0x328
   __IO uint32_t SLEEP_IND;     //0x32C

}MFPR_TypeDef;


#define  MFPR_BASE      0xD401E000
#define  MFPR  (( MFPR_TypeDef *) MFPR_BASE )


 //Multi-Function Pin Control Register x
#define  AF_SEL_MSK       SHIFT0(0x7)
#define  AF_SEL_BASE       0
#define  SLEEP_SEL1    BIT_3
#define  EDGE_RISE_EN    BIT_4
#define  EDGE_FALL_EN    BIT_5
#define  EDGE_CLEAR      BIT_6
#define  SLEEP_DIR       BIT_7
#define  SLEEP_DATA      BIT_8
#define  SLEEP_SEL0    BIT_9
#define  DRIVE_MSK      SHIFT11(0x3)
#define  DRIVE_BASE         11 
#define  PULLDN_EN          BIT_13
#define  PULLUP_EN          BIT_14
#define  PULL_SEL          BIT_15

#endif 
