#ifndef __PMUA_H__
#define __PMUA_H__

typedef struct
{
    __IO uint32_t  CC_CP;                //0x0
    __IO uint32_t  CC_AP;                //0x4
    __IO uint32_t  DM_CC_CP;             //0x8
    __IO uint32_t  DM_CC_AP;             //0xC
    __IO uint32_t  FC_TIMER;             //0x10
    __IO uint32_t  CP_IDLE_CFG;          //0x14
    __IO uint32_t  AP_IDLE_CFG;          //0x18
    __IO uint32_t  SQU_CLK_GATE_CTRL;    //0x1C
    __IO uint32_t  IRE_CLK_GATE_CTRL;    //0x20
    __IO uint32_t  RESERVED0;            //0x24
    __IO uint32_t  CCIC_CLK_GATE_CTRL;   //0x28
    __IO uint32_t  FBRC0_CLK_GATE_CTRL;  //0x2C
    __IO uint32_t  FBRC1_CLK_GATE_CTRL;  //0x30
    __IO uint32_t  USB_CLK_GATE_CTRL;    //0x34
    __IO uint32_t  RESERVED1[2];            //0x38-0x3C
    __IO uint32_t  PMU_CLK_GATE_CTRL;    //0x40
    __IO uint32_t  LTE_DMA_AXI2MC0_CLK_RES_CTRL;     //0x44
    __IO uint32_t  AXI_CLK_RES_CTRL;     //0x48
    __IO uint32_t  HSI_CLK_RES_CTRL;     //0x4C
    __IO uint32_t  CCIC_CLK_RES_CTRL;    //0x50
    __IO uint32_t  SDH0_CLK_RES_CTRL;    //0x54
    __IO uint32_t  SDH1_CLK_RES_CTRL;    //0x58
    __IO uint32_t  USB_CLK_RES_CTRL;     //0x5C
    __IO uint32_t  QSPI_CLK_RES_CTRL;      //0x60
    __IO uint32_t  DMA_CLK_RES_CTRL;     //0x64
    __IO uint32_t  AES_CLK_RES_CTRL;     //0x68
    __IO uint32_t  MCB_CLK_RES_CTRL;     //0X6C
    __IO uint32_t  CP_IMR;           //0x70
    __IO uint32_t  CP_IRWC;          //0x74
    __IO uint32_t  CP_ISR;           //0x78
    __IO uint32_t  SD_ROT_WAKE_CLR;      //0X7C
    __IO uint32_t  PMU_FBRC_CLK;         //0x80
    __IO uint32_t  PWR_STBL_TIMER;       //0X84
    __IO uint32_t  DEBUG_REG;            //0x88
    __IO uint32_t  SRAM_PWR_DWN;         //0x8C
    __IO uint32_t  CORE_STATUS;          //0x90
    __IO uint32_t  RES_FRM_SLP_CLR;      //0x94
    __IO uint32_t  AP_IMR;               //0x98
    __IO uint32_t  AP_IRWC;              //0x9C
    __IO uint32_t  AP_ISR;               //0xA0
    __IO uint32_t  DX8_CLK_RES_CTRL;     //0XA4
    __IO uint32_t  PMU_VPRO_PWRDWN;      //0xA8
    __IO uint32_t  DTC_CLK_RES_CTRL;     //0xAC
    __IO uint32_t  MC_HW_SLP_TYPE;       //0XB0
    __IO uint32_t  MC_SLP_REQ_AP;        //0XB4
    __IO uint32_t  MC_SLP_REQ_CP;        //0xb8
    __IO uint32_t  MC_SLP_REQ_MSA;       //0xbc
    __IO uint32_t  MC_MC_SLP_TYPE;       //0XC0
    __IO uint32_t  PLL_SEL_STATUS;       //0xc4
    __IO uint32_t  SYNC_MODE_BYPASS;     //0xC8
    __IO uint32_t  GC_CLK_RES_CTRL;      //0xCC
    __IO uint32_t  GC_PWRDWN;            //0xd0  GPU_3D_PWRDWN
    __IO uint32_t  SMC_CLK_RES_CTRL;     //0xD4
    __IO uint32_t  PWR_CTRL_REG;         //0xD8
    __IO uint32_t  PWR_BLK_TMR_REG;      //0xDC
    __IO uint32_t  SDH2_CLK_RES_CTRL;    //0xE0
    __IO uint32_t  SDH_NOM_DENOM_CTRL;      //0xE4
    __IO uint32_t  RESERVED2[2];            //0xE8-0XEC
    __IO uint32_t  PWR_STATUS_REG;       //0xF0
    __IO uint32_t  MCK4_CTRL;    //0xF4
    __IO uint32_t  RESERVED3[(0x100-0xF4)/4-1];
    __IO uint32_t  CC2_AP;               //0x100
    __IO uint32_t  CC2_CP;               //0x104
    __IO uint32_t  TRACE_CONFIG;         //0x108
    __IO uint32_t  CP_CLK_CTRL;          //0x10C
    __IO uint32_t  AP_CLK_CTRL;          //0x110
    __IO uint32_t  RESERVED4[(0x120-0x110)/4-1];
    __IO uint32_t  CA7MP_IDLE_CFG0;      //0x120
    __IO uint32_t  CA7_CORE0_IDLE_CFG;   //0x124
    __IO uint32_t  RESERVED5[(0x140-0x124)/4-1];
    __IO uint32_t  DVC_DEBUG;           //0x140
	__IO uint32_t  ACLK_CTRL;            //0x144
	__IO uint32_t  DDR_CKPHY_PLL1_CTRL1;  //0x148
	__IO uint32_t  DDR_CKPHY_PLL1_CTRL2; //0x14c
    __IO uint32_t  RESERVED6[(0x15c-0x14c)/4-1];
	__IO uint32_t  CKPHY_FC_CTRL;         //0x15c
} PMUA_TypeDef;

#define     PMUA_BASE      0xD4282800
#define     PMUA ((  PMUA_TypeDef *)  PMUA_BASE )


//Bitfield Constants Register  CC_CP
#define CC_CP_CP_RD_ST_CLEAR_MASK                   0x80000000
#define CC_CP_CP_RD_ST_CLEAR_SHIFT                  31
#define CC_CP_ACLK_FC_SHORTCUT_MASK                 0x40000000
#define CC_CP_ACLK_FC_SHORTCUT_SHIFT                30
#define CC_CP_DCLK_FC_SHORTCUT_MASK                 0x20000000
#define CC_CP_DCLK_FC_SHORTCUT_SHIFT                29
#define CC_CP_CORE_FC_SHORTCUT_MASK                 0x10000000
#define CC_CP_CORE_FC_SHORTCUT_SHIFT                28
#define CC_CP_CP_ALLOW_SPD_CHG_MASK                 0x08000000
#define CC_CP_CP_ALLOW_SPD_CHG_SHIFT                27
#define CC_CP_BUS_FREQ_CHG_REQ_MASK             0x04000000
#define CC_CP_BUS_FREQ_CHG_REQ_SHIFT            26
#define CC_CP_DDR_FREQ_CHG_REQ_MASK                 0x02000000
#define CC_CP_DDR_FREQ_CHG_REQ_SHIFT                25
#define CC_CP_CP_FREQ_CHG_REQ_MASK                  0x01000000
#define CC_CP_CP_FREQ_CHG_REQ_SHIFT                 24
#define CC_CP_ASYNC5                              BIT_23
#define CC_CP_ASYNC4                              BIT_22
#define CC_CP_ASYNC3_1                              BIT_21
#define CC_CP_ASYNC3                              BIT_20
#define CC_CP_ASYNC2                              BIT_19
#define CC_CP_ASYNC1                              BIT_18
#define CC_CP_BUS_CLK_DIV_MASK                  0x00038000
#define CC_CP_BUS_CLK_DIV_SHIFT                 15
#define CC_CP_DDR_CLK_DIV_MASK                      0x00007000
#define CC_CP_DDR_CLK_DIV_SHIFT                     12
#define CC_CP_XP_CLK_DIV_MASK                      SHIFT9(0x7)
#define CC_CP_XP_CLK_DIV_SHIFT                     9
#define CC_CP_BIU_CLK_DIV_MASK                     SHIFT6(0x7)
#define CC_CP_BIU_CLK_DIV_SHIFT                     6
#define CC_CP_BUS_MC_CLK_DIV_MASK                    SHIFT3(0x7)
#define CC_CP_BUS_MC_CLK_DIV_SHIFT                    3
#define CC_CP_CORE_CLK_DIV_MASK                     0x00000007
#define CC_CP_CORE_CLK_DIV_SHIFT                    0



//Bitfield Constants Register  CC_PJ
#define CC_AP_AP_RD_ST_CLEAR_MASK                   0x80000000
#define CC_AP_AP_RD_ST_CLEAR_SHIFT                  31
#define CC_AP_ACLK_FC_SHORTCUT_MASK                 0x40000000
#define CC_AP_ACLK_FC_SHORTCUT_SHIFT                30
#define CC_AP_DCLK_DYN_FC_MASK                 0x20000000
#define CC_AP_DCLK_DYN_FC_SHIFT                29
#define CC_AP_CORE_DYN_FC_MASK                   0x10000000
#define CC_AP_CORE_DYN_FC_SHIFT                  28
#define CC_AP_AP_ALLOW_SPD_CHG_MASK                 0x08000000
#define CC_AP_AP_ALLOW_SPD_CHG_SHIFT                27
#define CC_AP_BUS_FREQ_CHG_REQ_MASK             0x04000000
#define CC_AP_BUSFREQ_CHG_REQ_SHIFT            26
#define CC_AP_DDR_FREQ_CHG_REQ_MASK                 0x02000000
#define CC_AP_DDR_FREQ_CHG_REQ_SHIFT                25
#define CC_AP_AP_FREQ_CHG_REQ_MASK                  0x01000000
#define CC_AP_AP_FREQ_CHG_REQ_SHIFT                 24
#define CC_CP_ASYNC5                              BIT_23
#define CC_CP_ASYNC4                              BIT_22
#define CC_CP_ASYNC3_1                              BIT_21
#define CC_CP_ASYNC3                              BIT_20
#define CC_CP_ASYNC2                              BIT_19
#define CC_CP_ASYNC1                              BIT_18
#define CC_AP_BUS_CLK_DIV_MASK                  0x00038000
#define CC_AP_BUS_CLK_DIV_SHIFT                 15
#define CC_AP_DDR_CLK_DIV_MASK                      0x00007000
#define CC_AP_DDR_CLK_DIV_SHIFT                     12
#define CC_AP_L2_CLK_DIV_MASK                    0x00000E00
#define CC_AP_L2_CLK_DIV_SHIFT                   9
#define CC_AP_BIU_CLK_DIV_MASK                     SHIFT6(0x7)
#define CC_AP_BIU_CLK_DIV_SHIFT                     6
#define CC_AP_BUS_MC_CLK_DIV_MASK                    SHIFT3(0x7)
#define CC_AP_BUS_MC_CLK_DIV_SHIFT                    3
#define CC_AP_CORE_CLK_DIV_MASK                     0x00000007
#define CC_AP_CORE_CLK_DIV_SHIFT                    0


//Bitfield Constants Register  DM_CC_SP
#define DM_CC_CP_ACLK_FC_DONE             BIT_29
#define DM_CC_CP_DCLK_FC_DONE              BIT_28
#define DM_CC_CP_AP_FC_DONE               BIT_27
#define DM_CC_CP_CP_FC_DONE              BIT_26
#define DM_CC_CP_AP_RD_STATUS_MASK                     0x02000000
#define DM_CC_CP_AP_RD_STATUS_SHIFT                    25
#define DM_CC_CP_CP_RD_STATUS_MASK                     0x01000000
#define DM_CC_CP_CP_RD_STATUS_SHIFT                    24
#define DM_CC_CP_ASYNC5                              BIT_23
#define DM_CC_CP_ASYNC4                              BIT_22
#define DM_CC_CP_ASYNC3_1                              BIT_21
#define DM_CC_CP_ASYNC3                              BIT_20
#define DM_CC_CP_ASYNC2                              BIT_19
#define DM_CC_CP_ASYNC1                              BIT_18
#define DM_CC_CP_BUS_CLK_DIV_MASK                  0x00038000
#define DM_CC_CP_BUS_CLK_DIV_SHIFT                 15
#define DM_CC_CP_DDR_CLK_DIV_MASK                      0x00007000
#define DM_CC_CP_DDR_CLK_DIV_SHIFT                     12
#define DM_CC_CP_XP_CLK_DIV_MASK                    0x00000E00
#define DM_CC_CP_XP_CLK_DIV_SHIFT                   9
#define DM_CC_CP_BIU_CLK_DIV_MASK                     SHIFT6(0x7)
#define DM_CC_CP_BIU_CLK_DIV_SHIFT                     6
#define DM_CC_CP_BUS_MC_CLK_DIV_MASK                    SHIFT3(0x7)
#define DM_CC_CP_BUS_MC_CLK_DIV_SHIFT                    3
#define DM_CC_CP_CORE_CLK_DIV_MASK                     0x00000007
#define DM_CC_CP_CORE_CLK_DIV_SHIFT                    0



//Bitfield Constants Register  DM_CC_PJ
#define DM_CC_AP_ACLK_FC_DONE             BIT_29
#define DM_CC_AP_DCLK_FC_DONE              BIT_28
#define DM_CC_AP_AP_FC_DONE               BIT_27
#define DM_CC_AP_CP_FC_DONE              BIT_26
#define DM_CC_AP_AP_RD_STATUS_MASK                     0x02000000
#define DM_CC_AP_AP_RD_STATUS_SHIFT                    25
#define DM_CC_AP_CP_RD_STATUS_MASK                     0x01000000
#define DM_CC_AP_CP_RD_STATUS_SHIFT                    24
#define DM_CC_AP_ASYNC5                              BIT_23
#define DM_CC_AP_ASYNC4                              BIT_22
#define DM_CC_AP_ASYNC3_1                              BIT_21
#define DM_CC_AP_ASYNC3                              BIT_20
#define DM_CC_AP_ASYNC2                              BIT_19
#define DM_CC_AP_ASYNC1                              BIT_18
#define DM_CC_AP_BUS_CLK_DIV_MASK                  0x00038000
#define DM_CC_AP_BUS_CLK_DIV_SHIFT                 15
#define DM_CC_AP_DDR_CLK_DIV_MASK                      0x00007000
#define DM_CC_AP_DDR_CLK_DIV_SHIFT                     12
#define DM_CC_AP_L2_CLK_DIV_MASK                    0x00000E00
#define DM_CC_AP_L2_CLK_DIV_SHIFT                   9
#define DM_CC_AP_BIU_CLK_DIV_MASK                     SHIFT6(0x7)
#define DM_CC_AP_BIU_CLK_DIV_SHIFT                     6
#define DM_CC_AP_BUS_MC_CLK_DIV_MASK                    SHIFT3(0x7)
#define DM_CC_AP_BUS_MC_CLK_DIV_SHIFT                    3
#define DM_CC_AP_CORE_CLK_DIV_MASK                     0x00000007
#define DM_CC_AP_CORE_CLK_DIV_SHIFT                    0






//Bitfield Constants Register  FC_TIMER

#define FC_TIMER_FC_TIMER_MASK                         SHIFT3(0x1fffffff)
#define FC_TIMER_FC_TIMER_SHIFT                        3
#define FC_TIMER_FC_TIMER_SEL_MASK                     0x00000006
#define FC_TIMER_FC_TIMER_SEL_SHIFT                    1
#define FC_TIMER_FC_TIMER_EN_MASK                      0x00000001
#define FC_TIMER_FC_TIMER_EN_SHIFT                     0


//Bitfield Constants Register  SP_IDLE_CFG
#define     CP_IDLE_CFG_SEA_SRAM_WAIT_MASK                   SHIFT27(0x1f)
#define     CP_IDLE_CFG_SEA_SRAM_WAIT_BASE                  27
#define     CP_IDLE_CFG_SEA_SRAM_VMCVSSM_MSK              SHIFT23(0xf)
#define      CP_IDLE_CFG_SEA_SRAM_VMCVSSM_BASE           23
#define CP_IDLE_CFG_IDLE_CFG_CP_DIS_MC_SW_REQ           BIT_21
#define CP_IDLE_CFG_IDLE_CFG_CP_MC_WAKE_EN          BIT_20
#define CP_IDLE_CFG_IDLE_CFG_CP_L2_PWR_SW      BIT_19
#define CP_IDLE_CFG_IDLE_CFG_CP_L1_PWR_SW       BIT_18
#define CP_IDLE_CFG_IDLE_CFG_CP_PWR_SW_MSK          SHIFT16(0x3)
#define CP_IDLE_CFG_IDLE_CFG_CP_PWR_SW_BASE         16
#define CP_IDLE_CFG_IDLE_CFG_CP_DIS_L2_SLP2         BIT_15
#define CP_IDLE_CFG_IDLE_CFG_CP_DIS_L2_SLP1         BIT_14
#define CP_IDLE_CFG_IDLE_CFG_CP_DIS_SLP2                BIT_13
#define CP_IDLE_CFG_IDLE_CFG_CP_DIS_SLP1                BIT_12
#define CP_IDLE_CFG_IDLE_CFG_CP_MASK_JTAG_IDLE      BIT_11
#define CP_IDLE_CFG_IDLE_CFG_CP_L2_CLK_DIS          BIT_10
#define CP_IDLE_CFG_IDLE_CFG_CP_L2_RESETN               BIT_9
#define CP_IDLE_CFG_IDLE_CFG_CP_L2_ISOB             BIT_8
#define CP_IDLE_CFG_IDLE_CFG_CP_L2_PWR_OFF          BIT_7
#define CP_IDLE_CFG_IDLE_CFG_CP_SRAM_PWRDWN         BIT_6
#define CP_IDLE_CFG_IDLE_CFG_CP_PWRDWN              BIT_5
#define CP_IDLE_CFG_IDLE_CFG_CP_DTCM_PDWN_DIS                                BIT_2
#define CP_IDLE_CFG_IDLE_CFG_MASK_WFI_CHECK                                BIT_0



/*      Bit(s) PMUA_SEA_IDLE_CFG_RSRV_4_0 reserved */

/*  AP_IDLE_CFG         0x0018  <var Processor: Mohawk> Idle
 *                                      Configuration Register
 */
/*      Bit(s) AP_IDLE_CFG_RSRV_31_22 reserved */

#define AP_IDLE_CFG_MOH_DIS_MC_SW_REQ           BIT_21
#define AP_IDLE_CFG_MOH_MC_WAKE_EN          BIT_20
#define AP_IDLE_CFG_AP_L2_PWR_SW                   BIT_19
#define AP_IDLE_CFG_AP_L1_PWR_SW                  BIT_18
#define AP_IDLE_CFG_AP_PWR_SW_MSK           SHIFT16(0x3)
#define AP_IDLE_CFG_AP_PWR_SW_BASE          16
#define AP_IDLE_CFG_AP_DIS_L2_SLP2          BIT_15
#define AP_IDLE_CFG_AP_DIS_L2_SLP1          BIT_14
#define AP_IDLE_CFG_AP_DIS_SLP2             BIT_13
#define AP_IDLE_CFG_AP_DIS_SLP1             BIT_12
#define AP_IDLE_CFG_AP_MASK_JTAG_IDLE       BIT_11
#define AP_IDLE_CFG_AP_L2_CLK_DIS           BIT_10
#define AP_IDLE_CFG_AP_L2_RESETN                BIT_9
#define AP_IDLE_CFG_AP_L2_ISOB              BIT_8
#define AP_IDLE_CFG_AP_L2_PWR_OFF           BIT_7
#define AP_IDLE_CFG_AP_SRAM_PWRDWN          BIT_6
#define AP_IDLE_CFG_AP_PWRDWN               BIT_5
#define AP_IDLE_CFG_AP_IDLE                 BIT_1
#define AP_IDLE_CFG_MASK_WFI_CHECK                     BIT_0


/*  PMUA_SQU_CLK_GATE_CTRL      0x001C  SQU Dynamic Clock Gate Control Register */
#define SQU_CLK_GATE_CTRL_MP3_MODE_EN             BIT_30
#define SQU_CLK_GATE_CTRL_SQU_REFCLK_GATE_DIS             BIT_29
#define SQU_CLK_GATE_CTRL_SQU_REFCLK_SW_EN             BIT_28
#define SQU_CLK_GATE_CTRL_SQU_BANK4CLK_GATE_DIS             BIT_21
#define SQU_CLK_GATE_CTRL_SQU_BANK4CLK_SW_EN             BIT_20
#define SQU_CLK_GATE_CTRL_SQU_BANK3CLK_GATE_DIS             BIT_19
#define SQU_CLK_GATE_CTRL_SQU_BANK3CLK_SW_EN             BIT_18
#define SQU_CLK_GATE_CTRL_SQU_BANK2CLK_GATE_DIS             BIT_17
#define SQU_CLK_GATE_CTRL_SQU_BANK2CLK_SW_EN             BIT_16
#define SQU_CLK_GATE_CTRL_SQU_BANK1CLK_GATE_DIS             BIT_15
#define SQU_CLK_GATE_CTRL_SQU_BANK1CLK_SW_EN             BIT_14
#define SQU_CLK_GATE_CTRL_SQU_BANK0CLK_GATE_DIS             BIT_13
#define SQU_CLK_GATE_CTRL_SQU_BANK0CLK_SW_EN             BIT_12
#define SQU_CLK_GATE_CTRL_SQU_S8CLK_GATE_DIS             BIT_11
#define SQU_CLK_GATE_CTRL_SQU_S8CLK_SW_EN             BIT_10
#define SQU_CLK_GATE_CTRL_SQU_S4CLK_GATE_DIS             BIT_9
#define SQU_CLK_GATE_CTRL_SQU_S4CLK_SW_EN             BIT_8
#define SQU_CLK_GATE_CTRL_SQU_S1CLK_GATE_DIS             BIT_7
#define SQU_CLK_GATE_CTRL_SQU_S1CLK_SW_EN             BIT_6
#define SQU_CLK_GATE_CTRL_SQU_FMBISTCLK_GATE_DIS             BIT_5
#define SQU_CLK_GATE_CTRL_SQU_FMBISTCLK_SW_EN             BIT_4
#define SQU_CLK_GATE_CTRL_SQU_ROMCLK_GATE_DIS             BIT_3
#define SQU_CLK_GATE_CTRL_SQU_ROMCLK_SW_EN             BIT_2
#define SQU_CLK_GATE_CTRL_SQU_CLK_GATE_DIS             BIT_1
#define SQU_CLK_GATE_CTRL_SQU_CLK_SW_EN             BIT_0


//Image Rotation Engine Dynamic Clock Gate Control Register
#define PMU_IRE_CLK_GATE_CTRL_IRE_ACLK_GATE_DIS           BIT_5
#define PMU_IRE_CLK_GATE_CTRL_IRE_ACLK_SW_EN           BIT_4
#define PMU_IRE_CLK_GATE_CTRL_IRE_CLK_GATE_DIS           BIT_3
#define PMU_IRE_CLK_GATE_CTRL_IRE_CLK_SW_EN           BIT_2
#define PMU_IRE_CLK_GATE_CTRL_IRE_HCLK_GATE_DIS           BIT_1
#define PMU_IRE_CLK_GATE_CTRL_IRE_HCLK_SW_EN           BIT_0




//Bitfield Constants Register  CCIC_CLK_GATE_CTRL
#define CCIC_CLK_GATE_CTRL_CSI_LAN3CLK_GATE_DIS        BIT_15
#define CCIC_CLK_GATE_CTRL_CSI_LAN3CLK_SW_EN              BIT_14
#define CCIC_CLK_GATE_CTRL_CSI_LANE2CLK_GATE_DIS         BIT_13
#define CCIC_CLK_GATE_CTRL_CSI_LANE2CLK_SW_EN           BIT_12
#define CCIC_CLK_GATE_CTRL_CSI_LANE1CLK_GATE_DIS          BIT_11
#define CCIC_CLK_GATE_CTRL_CSI_LANE1CLK_SW_EN            BIT_10
#define CCIC_CLK_GATE_CTRL_CSI_LAN0CLK_GATE_DIS                 BIT_9
#define CCIC_CLK_GATE_CTRL_CSI_LAN0CLK_SW_EN                BIT_8
#define CCIC_CLK_GATE_CTRL_CSI2_CLK_GATE_DIS                BIT_7
#define CCIC_CLK_GATE_CTRL_CSI2_CLK_SW_EN               BIT_6
#define CCIC_CLK_GATE_CTRL_CCIC_HCLK_GATE_DIS                BIT_5
#define CCIC_CLK_GATE_CTRL_CCIC_HCLK_SW_EN            BIT_4
#define CCIC_CLK_GATE_CTRL_CCIC_VCLK_GATE_DIS              BIT_3
#define CCIC_CLK_GATE_CTRL_CCIC_VCLK_SW_EN                 BIT_2
#define CCIC_CLK_GATE_CTRL_CCIC_ACLK_GATE_DIS          BIT_1
#define CCIC_CLK_GATE_CTRL_CCIC_ACLK_SW_EN           BIT_0


//Fabric0 Dynamic Clock Gate Control Register
#define FBRC0_CLK_GATE_CTRL_FBCR0_DTCCLK_SW_EN            BIT_27
#define FBRC0_CLK_GATE_CTRL_FBCR0_DTCCLK_GATE_DIS            BIT_26
#define FBRC0_CLK_GATE_CTRL_FBCR0_S6CLK_SW_EN            BIT_25
#define FBRC0_CLK_GATE_CTRL_FBCR0_S6CLK_GATE_DIS            BIT_24
#define FBRC0_CLK_GATE_CTRL_FBCR0_S5CLK_SW_EN            BIT_23
#define FBRC0_CLK_GATE_CTRL_FBCR0_S5CLK_GATE_DIS            BIT_22
#define FBRC0_CLK_GATE_CTRL_FBCR0_S4CLK_SW_EN            BIT_21
#define FBRC0_CLK_GATE_CTRL_FBCR0_S4CLK_GATE_DIS           BIT_20
#define FBRC0_CLK_GATE_CTRL_FBCR0_S3CLK_SW_EN            BIT_19
#define FBRC0_CLK_GATE_CTRL_FBCR0_S3CLK_GATE_DIS           BIT_18
#define FBRC0_CLK_GATE_CTRL_FBCR0_S2CLK_SW_EN            BIT_17
#define FBRC0_CLK_GATE_CTRL_FBCR0_S2CLK_GATE_DIS            BIT_16
#define FBRC0_CLK_GATE_CTRL_FBCR0_S1CLK_SW_EN            BIT_15
#define FBRC0_CLK_GATE_CTRL_FBCR0_S1CLK_GATE_DIS            BIT_14
#define FBRC0_CLK_GATE_CTRL_FBCR0_S0CLK_SW_EN            BIT_13
#define FBRC0_CLK_GATE_CTRL_FBCR0_S0CLK_GATE_DIS            BIT_12
#define FBRC0_CLK_GATE_CTRL_FBCR0_M4CLK_SW_EN            BIT_11
#define FBRC0_CLK_GATE_CTRL_FBCR0_M4CLK_GATE_DIS            BIT_10
#define FBRC0_CLK_GATE_CTRL_FBCR0_M3CLK_SW_EN            BIT_9
#define FBRC0_CLK_GATE_CTRL_FBCR0_M3CLK_GATE_DIS            BIT_8
#define FBRC0_CLK_GATE_CTRL_FBCR0_M2CLK_SW_EN            BIT_7
#define FBRC0_CLK_GATE_CTRL_FBCR0_M2CLK_GATE_DIS            BIT_6
#define FBRC0_CLK_GATE_CTRL_FBCR0_M1CLK_SW_EN            BIT_5
#define FBRC0_CLK_GATE_CTRL_FBCR0_M1CLK_GATE_DIS            BIT_4
#define FBRC0_CLK_GATE_CTRL_FBCR0_M0CLK_SW_EN            BIT_3
#define FBRC0_CLK_GATE_CTRL_FBCR0_M0CLK_GATE_DIS            BIT_2
#define FBRC0_CLK_GATE_CTRL_FBCR0_CLK_SW_EN            BIT_1
#define FBRC0_CLK_GATE_CTRL_FBCR0_CLK_GATE_DIS            BIT_0


//Fabric1 Dynamic Clock Gate Control Register
#define FBRC1_CLK_GATE_CTRL_FBCR1_S12CLK_SW_EN                         BIT_19
#define FBRC1_CLK_GATE_CTRL_FBCR1_S12CLK_GATE_DIS                         BIT_18
#define FBRC1_CLK_GATE_CTRL_FBCR1_S11CLK_SW_EN                         BIT_17
#define FBRC1_CLK_GATE_CTRL_FBCR1_S11CLK_GATE_DIS                         BIT_16
#define FBRC1_CLK_GATE_CTRL_FBCR1_S10CLK_SW_EN                         BIT_15
#define FBRC1_CLK_GATE_CTRL_FBCR1_S10CLK_GATE_DIS                         BIT_14
#define FBRC1_CLK_GATE_CTRL_FBCR1_M15CLK_SW_EN                         BIT_13
#define FBRC1_CLK_GATE_CTRL_FBCR1_M15CLK_GATE_DIS                         BIT_12
#define FBRC1_CLK_GATE_CTRL_FBCR1_M14CLK_SW_EN                         BIT_11
#define FBRC1_CLK_GATE_CTRL_FBCR1_M14CLK_GATE_DIS                         BIT_10
#define FBRC1_CLK_GATE_CTRL_FBCR1_M13CLK_SW_EN                         BIT_9
#define FBRC1_CLK_GATE_CTRL_FBCR1_M13CLK_GATE_DIS                         BIT_8
#define FBRC1_CLK_GATE_CTRL_FBCR1_M12CLK_SW_EN                         BIT_7
#define FBRC1_CLK_GATE_CTRL_FBCR1_M12CLK_GATE_DIS                         BIT_6
#define FBRC1_CLK_GATE_CTRL_FBCR1_M11CLK_SW_EN                         BIT_5
#define FBRC1_CLK_GATE_CTRL_FBCR1_M11CLK_GATE_DIS                         BIT_4
#define FBRC1_CLK_GATE_CTRL_FBCR1_M10CLK_SW_EN                         BIT_3
#define FBRC1_CLK_GATE_CTRL_FBCR1_M10CLK_GATE_DIS                         BIT_2
#define FBRC1_CLK_GATE_CTRL_FBCR1_CLK_SW_EN                         BIT_1
#define FBRC1_CLK_GATE_CTRL_FBCR1_CLK_GATE_DIS                         BIT_0





//Bitfield Constants Register  USB_CLK_GATE_CTRL
#define USB_CLK_GATE_CTRL_SPH_ACLK_GATE_DIS                              BIT_23
#define USB_CLK_GATE_CTRL_SPH_ACLK_SW_EN                              BIT_22
#define USB_CLK_GATE_CTRL_SPH_RXCLKA_GATE_DIS                              BIT_21
#define USB_CLK_GATE_CTRL_SPH_RXCLKA_SW_EN                              BIT_20
#define USB_CLK_GATE_CTRL_SPH_TXCLKB_GATE_DIS                              BIT_19
#define USB_CLK_GATE_CTRL_SPH_TXCLKB_SW_EN                              BIT_18
#define USB_CLK_GATE_CTRL_SPH_RXCLKB_GATE_DIS                              BIT_17
#define USB_CLK_GATE_CTRL_SPH_RXCLKB_SW_EN                              BIT_16
#define USB_CLK_GATE_CTRL_SPH_TXCLKA_GATE_DIS                              BIT_15
#define USB_CLK_GATE_CTRL_SPH_TXCLKA_SW_EN                              BIT_14
#define USB_CLK_GATE_CTRL_SPH_CLK_GATE_DIS                             BIT_13
#define USB_CLK_GATE_CTRL_SPH_CLK_SW_EN                              BIT_12
#define USB_CLK_GATE_CTRL_USB_ACLK_GATE_DIS                              BIT_11
#define USB_CLK_GATE_CTRL_USB_ACLK_SW_EN                              BIT_10
#define USB_CLK_GATE_CTRL_USB_RXCLKA_GATE_DIS                              BIT_9
#define USB_CLK_GATE_CTRL_USB_RXCLKA_SW_EN                              BIT_8
#define USB_CLK_GATE_CTRL_USB_TXCLKB_GATE_DIS                              BIT_7
#define USB_CLK_GATE_CTRL_USB_TXCLKB_SW_EN                              BIT_6
#define USB_CLK_GATE_CTRL_USB_RXCLKB_GATE_DIS                              BIT_5
#define USB_CLK_GATE_CTRL_USB_RXCLKB_SW_EN                              BIT_4
#define USB_CLK_GATE_CTRL_USB_TXCLKA_GATE_DIS                              BIT_3
#define USB_CLK_GATE_CTRL_USB_TXCLKA_SW_EN                              BIT_2
#define USB_CLK_GATE_CTRL_USB_CLK_GATE_DIS                              BIT_1
#define USB_CLK_GATE_CTRL_USB_CLK_SW_EN                              BIT_0



//ISP/DXO Clock/Reset Control Register
#define ISP_CLK_RES_CTRL_ISP_HW_MODE                  BIT_15
#define ISP_CLK_RES_CTRL_ISP_SLEEP2                  BIT_14
#define ISP_CLK_RES_CTRL_ISP_SLEEP1                  BIT_13
#define ISP_CLK_RES_CTRL_ISP_ISOB                  BIT_12
#define ISP_CLK_RES_CTRL_ISP_AHB_CLKEN                  BIT_11
#define ISP_CLK_RES_CTRL_ISP_AHB_RSTN                 BIT_10
#define ISP_CLK_RES_CTRL_ISP_AXI_CLKEN                  BIT_9
#define ISP_CLK_RES_CTRL_ISP_AXI_RESETN                  BIT_8
#define ISP_CLK_RES_CTRL_ISP_CLK_FC_REQ                  BIT_7
#define ISP_CLK_RES_CTRL_ISP_CLK_DIV_MSK                  SHIFT4(0x7)
#define ISP_CLK_RES_CTRL_ISP_CLK_DIV_BASE                4
#define ISP_CLK_RES_CTRL_ISP_CLK_SEL_MSK                  SHIFT2(0x3)
#define ISP_CLK_RES_CTRL_ISP_CLK_SEL_BASE                 2
#define ISP_CLK_RES_CTRL_ISP_CLK_EN                  BIT_1
#define ISP_CLK_RES_CTRL_ISP_CLK_RSTN                  BIT_0




//Bitfield Constants Register  CLK_GATE_CTRL
#define CLK_GATE_CTRL_ULPI_CLK_SW_EN                   BIT_25
#define CLK_GATE_CTRL_32K_CLK_SW_EN                   BIT_24
#define CLK_GATE_CTRL_1M_CLK_SW_EN         BIT_21
#define CLK_GATE_CTRL_62P4M_CLK_SW_EN        BIT_20
#define CLK_GATE_CTRL_312M_CLK_SW_EN          BIT_19
#define CLK_GATE_CTRL_REFCLK_SW_EN         BIT_18
#define CLK_GATE_CTRL_SDH_ACLK_SW_EN            BIT_17
#define CLK_GATE_CTRL_SDH_ACLK_GATE_DIS           BIT_16
#define CLK_GATE_CTRL_APSYS_SMCLK_SW_EN            BIT_15
#define CLK_GATE_CTRL_APSYS_SMCLK_GATE_DIS           BIT_14
#define CLK_GATE_CTRL_APCORE_SMCLK_SW_EN         BIT_13
#define CLK_GATE_CTRL_APCORE_SMCLK_GATE_DIS       BIT_12
#define CLK_GATE_CTRL_CPCORE_SMCLK_SW_EN         BIT_11
#define CLK_GATE_CTRL_CPCORE_SMCLK_GATE_DIS       BIT_10
#define CLK_GATE_CTRL_ACLK_SW_EN              BIT_9
#define CLK_GATE_CTRL_ACLK_GATE_DIS            BIT_8
#define CLK_GATE_CTRL_DCLK_SW_EN             BIT_7
#define CLK_GATE_CTRL_DCLK_GATE_DIS            BIT_6
#define CLK_GATE_CTRL_APCORE_CLK_SW_EN                  BIT_5
#define CLK_GATE_CTRL_APCORE_CLK_GATE_DIS                BIT_4
#define CLK_GATE_CTRL_CPCORE_CLK_SW_EN                  BIT_3
#define CLK_GATE_CTRL_CPCORE_CLK_GATE_DIS                 BIT_2
#define CLK_GATE_CTRL_REF_CLK_GATE_DIS                  BIT_1
#define CLK_GATE_CTRL_REF_CLK_SW_EN               BIT_0


//DSI Clock/Reset Control Register
#define DSI_CLK_RES_CTRL_DSI_PHYSLOW_PRER_MSK                    SHIFT6(0x1f)
#define DSI_CLK_RES_CTRL_DSI_PHYSLOW_PRER_BASE                     6
#define DSI_CLK_RES_CTRL_DSI_PHYSLOWCLK_EN                     BIT_5
#define DSI_CLK_RES_CTRL_DSI_PHYSLOW_RST                     BIT_4
#define DSI_CLK_RES_CTRL_DSI_ESCCLK_RESET                    BIT_3
#define DSI_CLK_RES_CTRL_DSI_ESC_EN                     BIT_2
#define DSI_CLK_RES_CTRL_DSI_ESC_SEL_MSK                     SHIFT0(3)
#define DSI_CLK_RES_CTRL_DSI_ESC_SEL_BASE                     0



//Bitfield Constants Register  IRE_CLK_RES_CTRL
#define IRE_CLK_RES_CTRL_IRE_AXICLK_EN_MASK                    0x00000008
#define IRE_CLK_RES_CTRL_IRE_AXICLK_EN_SHIFT                   3
#define IRE_CLK_RES_CTRL_IRE_AXI_RST_MASK                      0x00000001
#define IRE_CLK_RES_CTRL_IRE_AXI_RST_SHIFT                     0


/*  PMUA_LCD_CLK_RES_CTRL       0x004C  LCD Clock/Reset Control Register */

#define LCD_CLK_RES_CTRL_LCD_ACLK_FC_REQ                           BIT_22
#define LCD_CLK_RES_CTRL_LCD_ACLK_DIV_MSK                          SHIFT19(0x7)
#define LCD_CLK_RES_CTRL_LCD_ACLK_DIV_BASE                          19
#define LCD_CLK_RES_CTRL_LCD_ACLK_SEL_MSK                           SHIFT17(0x3)
#define LCD_CLK_RES_CTRL_LCD_ACLK_SEL_BASE                           17
#define LCD_CLK_RES_CTRL_LCD_ACLK_SW_RST                           BIT_16
#define LCD_CLK_RES_CTRL_LCD_PST_DIV_MSK                           SHIFT10(0x1f)
#define LCD_CLK_RES_CTRL_LCD_PST_DIV_BASE                           10
#define LCD_CLK_RES_CTRL_LCD_PST_CKEN                           BIT_9
#define LCD_CLK_RES_CTRL_LCD_PST_OUTDIS                           BIT_8
#define LCD_CLK_RES_CTRL_LCD_CLK_SEL            BIT_6
#define LCD_CLK_RES_CTRL_LCD_CLK_EN         BIT_4
#define LCD_CLK_RES_CTRL_LCD_AXICLK_EN          BIT_3
/*      Bit(s) PMUA_LCD_CLK_RES_CTRL_RSRV_2 reserved */
#define LCD_CLK_RES_CTRL_LCD_RST                BIT_1
#define LCD_CLK_RES_CTRL_LCD_AXI_RST            BIT_0









//Bitfield Constants Register  DISPLAY1_CLK_RES_CTRL
#define DISPLAY1_CLK_RES_CTRL_DSI_PHYSLOW_PRER_MASK                 0x000F8000
#define DISPLAY1_CLK_RES_CTRL_DSI_PHYSLOW_PRER_SHIFT                15
#define DISPLAY1_CLK_RES_CTRL_HDMI_I2SCLK_EN_MASK                   0x00004000
#define DISPLAY1_CLK_RES_CTRL_HDMI_I2SCLK_EN_SHIFT                  14
#define DISPLAY1_CLK_RES_CTRL_HDMI_REFCLK_EN_MASK                   0x00002000
#define DISPLAY1_CLK_RES_CTRL_HDMI_REFCLK_EN_SHIFT                  13
#define DISPLAY1_CLK_RES_CTRL_DSI_ESCCLK_EN_MASK                    0x00001000
#define DISPLAY1_CLK_RES_CTRL_DSI_ESCCLK_EN_SHIFT                   12
#define DISPLAY1_CLK_RES_CTRL_CLK_DIV_SEL_MASK                      0x00000F00
#define DISPLAY1_CLK_RES_CTRL_CLK_DIV_SEL_SHIFT                     8
#define DISPLAY1_CLK_RES_CTRL_CLK_SEL_MASK                          0x000000C0
#define DISPLAY1_CLK_RES_CTRL_CLK_SEL_SHIFT                         6
#define DISPLAY1_CLK_RES_CTRL_DSI_PHY_SLOW_CLK_EN_MASK              0x00000020
#define DISPLAY1_CLK_RES_CTRL_DSI_PHY_SLOW_CLK_EN_SHIFT             5
#define DISPLAY1_CLK_RES_CTRL_CLK_EN_MASK                           0x00000010
#define DISPLAY1_CLK_RES_CTRL_CLK_EN_SHIFT                          4
#define DISPLAY1_CLK_RES_CTRL_AXICLK_EN_MASK                        0x00000008
#define DISPLAY1_CLK_RES_CTRL_AXICLK_EN_SHIFT                       3
#define DISPLAY1_CLK_RES_CTRL_DSI_PHY_SLOW_RST_MASK                 0x00000004
#define DISPLAY1_CLK_RES_CTRL_DSI_PHY_SLOW_RST_SHIFT                2
#define DISPLAY1_CLK_RES_CTRL_RST_MASK                              0x00000002
#define DISPLAY1_CLK_RES_CTRL_RST_SHIFT                             1
#define DISPLAY1_CLK_RES_CTRL_AXI_RST_MASK                          0x00000001
#define DISPLAY1_CLK_RES_CTRL_AXI_RST_SHIFT                         0





/*  CCIC_CLK_RES_CTRL       0x0050  CCIC Clock/Reset Control Register */
#define CCIC_CLK_RES_CTRL_CI_FNC_CLK_DIV_MSK          SHIFT18(0x7)
#define CCIC_CLK_RES_CTRL_CI_FNC_CLK_DIV_BASE           18
#define CCIC_CLK_RES_CTRL_CI_CLK_SEL_MSK            SHIFT16(0x3)
#define CCIC_CLK_RES_CTRL_CI_CLK_SEL_BASE             16
#define CCIC_CLK_RES_CTRL_CI_CLK_FC_REQ            BIT_15
#define CCIC_CLK_RES_CTRL_CCIC_PHYSLOW_PRER_MSK     SHIFT10(0x1f)
#define CCIC_CLK_RES_CTRL_CCIC_PHYSLOW_PRER_BASE        10
#define CCIC_CLK_RES_CTRL_CCIC_PHYSLOWCLK_EN            BIT_9
#define CCIC_CLK_RES_CTRL_CCIC_PHYSLOW_RST              BIT_8
#define CCIC_CLK_RES_CTRL_CCIC_PHYCLK_SEL               BIT_7
#define CCIC_CLK_RES_CTRL_CCIC_CLK_SEL                  BIT_6
#define CCIC_CLK_RES_CTRL_CCIC_PHYCLK_EN                BIT_5
#define CCIC_CLK_RES_CTRL_CCIC_CLK_EN                   BIT_4
#define CCIC_CLK_RES_CTRL_CCIC_AXICLK_EN                BIT_3
#define CCIC_CLK_RES_CTRL_CCIC_PHY_RST                  BIT_2
#define CCIC_CLK_RES_CTRL_CCIC_RST                      BIT_1
#define CCIC_CLK_RES_CTRL_CCIC_AXI_RST                  BIT_0


/*  EM_CLK_RES_CTRL       0x004C  EMMC5.0 Clock/Reset Control
 *                                      Register
 */
/* EM EPHY Pad bypass/test mode SW Enable */
#define EM_CLK_RES_CTRL_EM_PHY_TMS_SW        BIT_31
#define EM_CLK_RES_CTRL_EM_PHY_TOE_SW        BIT_30
#define EM_CLK_RES_CTRL_EM_PHY_VREF          BIT_29
#define EM_CLK_RES_CTRL_EM_PHY_V18EN         BIT_28
#define EM_CLK_RES_CTRL_EM_PHY_LP_SEL        BIT_27
/* EMMC5.0 1248 MHz Input Clock Enable */
#define EM_CLK_RES_CTRL_EM_1248M_CLK_EN      BIT_15
/* EMMC5.0 1248 MHz Input Clock Frequency Divisor */
#define EM_CLK_RES_CTRL_EM_1248M_CLK_DIV_MSK     SHIFT12(0x7)
#define EM_CLK_RES_CTRL_EM_1248M_CLK_DIV_BASE    12
#define EM_CLK_RES_CTRL_EM_CLK_FC_REQ       BIT_11
#define EM_CLK_RES_CTRL_EM_CLK_DIV_MSK      SHIFT8(0x7)
#define EM_CLK_RES_CTRL_EM_CLK_DIV_BASE     8
/*      Bit(s) EM_CLK_RES_CTRL_RSRV_31_7 reserved */
/* EM Clock Select */
#define EM_CLK_RES_CTRL_EM_CLK_SEL_MSK      SHIFT6(0x3)
#define EM_CLK_RES_CTRL_EM_CLK_SEL_BASE     6
/*      Bit(s) EM_CLK_RES_CTRL_RSRV_5 reserved */
/* EM Peripheral Clock Enable */
#define EM_CLK_RES_CTRL_EM_CLK_EN           BIT_4
/* EM AXI Clock Enable */
#define EM_CLK_RES_CTRL_EM_AXICLK_EN        BIT_3
/*      Bit(s) EM_CLK_RES_CTRL_RSRV_2 reserved */
/* EM Peripheral Reset */
#define EM_CLK_RES_CTRL_EM_RST              BIT_1
/* EM AXI Reset */
#define EM_CLK_RES_CTRL_EM_AXI_RST          BIT_0



/*  SDH0_CLK_RES_CTRL       0x0054  SDIO Host 0 Clock/Reset Control
 *                                      Register
 */
/* SDH 1248 MHz Input Clock Enable */
#define SDH0_CLK_RES_CTRL_SDH_1248M_CLK_EN      BIT_15
/* SDH 1248 MHz Input Clock Frequency Divisor */
#define SDH0_CLK_RES_CTRL_SDH_1248M_CLK_DIV_MSK     SHIFT12(0x7)
#define SDH0_CLK_RES_CTRL_SDH_1248M_CLK_DIV_BASE    12
#define SDH0_CLK_RES_CTRL_SDH0_CLK_FC_REQ       BIT_11
#define SDH0_CLK_RES_CTRL_SDH0_CLK_DIV_MSK      SHIFT8(0x7)
#define SDH0_CLK_RES_CTRL_SDH0_CLK_DIV_BASE     8
/*      Bit(s) SDH0_CLK_RES_CTRL_RSRV_31_7 reserved */
/* SDIO Host 0 Clock Select */
#define SDH0_CLK_RES_CTRL_SDH0_CLK_SEL_MSK      SHIFT6(0x3)
#define SDH0_CLK_RES_CTRL_SDH0_CLK_SEL_BASE     6
/*      Bit(s) SDH0_CLK_RES_CTRL_RSRV_5 reserved */
/* SDIO Host 0 Peripheral Clock Enable */
#define SDH0_CLK_RES_CTRL_SDH0_CLK_EN           BIT_4
/* SDIO Host 0 AXI Clock Enable */
#define SDH0_CLK_RES_CTRL_SDH0_AXICLK_EN        BIT_3
/*      Bit(s) SDH0_CLK_RES_CTRL_RSRV_2 reserved */
/* SDIO Host 0 Peripheral Reset */
#define SDH0_CLK_RES_CTRL_SDH0_RST              BIT_1
/* SDIO Host 0 AXI Reset */
#define SDH0_CLK_RES_CTRL_SDH0_AXI_RST          BIT_0



/*  PMUA_SDH1_CLK_RES_CTRL      0x0058  SDIO Host 1 Clock/Reset Control
 *                                      Register
 */

#define SDH1_CLK_RES_CTRL_SDH1_CLK_FC_REQ       BIT_11
#define SDH1_CLK_RES_CTRL_SDH1_CLK_DIV_MSK      SHIFT8(0x7)
#define SDH1_CLK_RES_CTRL_SDH1_CLK_DIV_BASE     8
/* SDIO Host 1 Clock Select */
#define SDH1_CLK_RES_CTRL_SDH1_CLK_SEL_MSK      SHIFT6(0x3)
#define SDH1_CLK_RES_CTRL_SDH1_CLK_SEL_BASE     6
/*      Bit(s) SDH1_CLK_RES_CTRL_RSRV_5 reserved */
/* SDIO Host 1 Peripheral Clock Enable */
#define SDH1_CLK_RES_CTRL_SDH1_CLK_EN           BIT_4
/*      Bit(s) SDH1_CLK_RES_CTRL_RSRV_3 reserved */
/*      Bit(s) SDH1_CLK_RES_CTRL_RSRV_2 reserved */
/* SDIO Host 1 Peripheral Reset */
#define SDH1_CLK_RES_CTRL_SDH1_RST              BIT_1
/*      Bit(s) SDH1_CLK_RES_CTRL_RSRV_0 reserved */



/*  PMUA_SDH2_CLK_RES_CTRL      0x00E0  SDIO Host 2 Clock/Reset Control
 *                                      Register
 */
#define SDH2_CLK_RES_CTRL_SDH2_CLK_FC_REQ       BIT_11
#define SDH2_CLK_RES_CTRL_SDH2_CLK_DIV_MSK      SHIFT8(0x7)
#define SDH2_CLK_RES_CTRL_SDH2_CLK_DIV_BASE     8
/* SDIO Host 2 Clock Select */
#define SDH2_CLK_RES_CTRL_SDH2_CLK_SEL_MSK      SHIFT6(0x3)
#define SDH2_CLK_RES_CTRL_SDH2_CLK_SEL_BASE     6
/*      Bit(s) SDH2_CLK_RES_CTRL_RSRV_5 reserved */
/* SDIO Host 2 Peripheral Clock Enable */
#define SDH2_CLK_RES_CTRL_SDH2_CLK_EN           BIT_4
/*      Bit(s) SDH2_CLK_RES_CTRL_RSRV_3 reserved */
/*      Bit(s) SDH2_CLK_RES_CTRL_RSRV_2 reserved */
/* SDIO Host 2 Peripheral Reset */
#define SDH2_CLK_RES_CTRL_SDH2_RST              BIT_1
/*      Bit(s) SDH2_CLK_RES_CTRL_RSRV_0 reserved */


//* PMUA_USB_CLK_RES_CTRL       0x005C  USB Clock/Reset Control Register */
/*      Bit(s) USB_CLK_RES_CTRL_RSRV_31_5 reserved */
/* USB SPH AXI Clock Enable */
#define USB_CLK_RES_CTRL_USB_SPH_AXICLK_EN      BIT_6
/* USB AXI Clock Enable */
#define USB_CLK_RES_CTRL_USB_AXICLK_EN          BIT_3
/*      Bit(s) USB_CLK_RES_CTRL_RSRV_2 reserved */
/* USB SPH AXI Reset */
#define USB_CLK_RES_CTRL_USB_SPH_AXI_RST        BIT_5
/* USB AXI Reset */
#define USB_CLK_RES_CTRL_USB_AXI_RST            BIT_0





/*  PMUA_NF_CLK_RES_CTRL        0x0060  NAND Flash Clock/Reset Control Register */
/*      Bit(s) NF_CLK_RES_CTRL_RSRV_31_9 reserved */
/* NAND Flash ECC Reset */
#define NF_CLK_RES_CTRL_NF_ECC_RES          BIT_8
/* NAND Flash ECC Clock Enable */
#define NF_CLK_RES_CTRL_NF_ECC_CLK_EN       BIT_7
/* NAND Flash Clock Select */
#define NF_CLK_RES_CTRL_NF_CLK_SEL          BIT_6
/*      Bit(s) NF_CLK_RES_CTRL_RSRV_5 reserved */
/* NAND Flash Peripheral Clock Enable */
#define NF_CLK_RES_CTRL_NF_CLK_EN           BIT_4
/* NAND Flash AXI Clock Enable */
#define NF_CLK_RES_CTRL_NF_AXICLK_EN        BIT_3
/*      Bit(s) NF_CLK_RES_CTRL_RSRV_2 reserved */
/* NAND Flash Peripheral Reset */
#define NF_CLK_RES_CTRL_NF_RST              BIT_1
/* NAND Flash AXI Reset */
#define NF_CLK_RES_CTRL_NF_AXI_RST          BIT_0



/*  PMUA_DMA_CLK_RES_CTRL       0x0064  DMA Clock/Reset Control Register */
/*      Bit(s) DMA_CLK_RES_CTRL_RSRV_31_4 reserved */
/* DMA AXI Clock Enable */
#define DMA_CLK_RES_CTRL_DMA_AXICLK_EN          BIT_3
/*      Bit(s) DMA_CLK_RES_CTRL_RSRV_2_1 reserved */
/* DMA AXI Reset */
#define DMA_CLK_RES_CTRL_DMA_AXI_RST            BIT_0



/*  GEU_CLK_RES_CTRL        0x0068  GEU Clock/Reset Control Register */
/*      Bit(s) GEU_CLK_RES_CTRL_RSRV_31_4 reserved */
/* GEU AXI Clock Enable */
#define AES_CLK_RES_CTRL_AES_AXICLK_EN          BIT_3
/*      Bit(s) GEU_CLK_RES_CTRL_RSRV_2_1 reserved */
/* GEU AXI Reset */
#define AES_CLK_RES_CTRL_AES_AXI_RST            BIT_0



/*  MCB Clock/Reset Control Register */


#define MCB_CLK_RES_CTRL_ACLK_SW_RST                BIT_1

#define MCB_CLK_RES_CTRL_DCLK_SW_RST        BIT_0


/*  PMUA_SEA_IMR                0x0070  PMU <var Processor: Osprey> Interrupt
 */

#define CP_IMR_AP_IDLE_WAKE_INTR_MASK                                BIT_7
#define CP_IMR_CP_IDLE_WAKE_INTR                               BIT_6
#define CP_IMR_ACLK_FC_DONE_INTR                               BIT_5
#define CP_IMR_DCLK_FC_DONE_INTR                               BIT_4
#define CP_IMR_AP_FC_DONE_INTR                               BIT_3
#define CP_IMR_CP_FC_DONE_INTR                               BIT_2
#define CP_IMR_AP_FC_INTR_MASK              BIT_1
#define CP_IMR_CP_FC_INTR_MASK              BIT_0

/*  PMUA_SEA_IRWC               0x0074  PMU <var Processor: Osprey> Interrupt
 */

#define CP_IREC_AP_IDLE_WAKE_INTR                               BIT_7
#define CP_IREC_CP_IDLE_WAKE_INTR                               BIT_6
#define CP_IREC_ACLK_FC_DONE_INTR                               BIT_5
#define CP_IREC_DCLK_FC_DONE_INTR                               BIT_4
#define CP_IREC_AP_FC_DONE_INTR                               BIT_3
#define CP_IREC_CP_FC_DONE_INTR                               BIT_2
#define CP_IREC_AP_FC_INTR              BIT_1
#define CP_IREC_CP_FC_INTR              BIT_0


/*  PMUA_SEA_ISR                0x0078  PMU <var Processor: Osprey> Interrupt
 */

#define CP_ISR_AP_IDLE_WAKE_INTR                               BIT_7
#define CP_ISR_CP_IDLE_WAKE_INTR                               BIT_6
#define CP_ISR_ACLK_FC_DONE_INTR                               BIT_5
#define CP_ISR_DCLK_FC_DONE_INTR                               BIT_4
#define CP_ISR_AP_FC_DONE_INTR                               BIT_3
#define CP_ISR_CP_FC_DONE_INTR                               BIT_2
#define CP_ISR_AP_FC_ISR                BIT_1
#define CP_ISR_CP_FC_ISR                BIT_0



/*  PMUA_SD_ROT_WAKE_CLR        0x007C  SDIO/Rotary Wake Clear Register */
#define SD_ROT_WAKE_CLR_charge_detect_int_mask      BIT_17
#define SD_ROT_WAKE_CLR_charge_detect_int_clr       BIT_16
#define SD_ROT_WAKE_CLR_USB_VBUS_STS            BIT_15
#define SD_ROT_WAKE_CLR_USB_CHRG_INT_MASK       BIT_14
#define SD_ROT_WAKE_CLR_USB_CHRG_INT_CLR        BIT_13
#define SD_ROT_WAKE_CLR_USB_WK_MASK                     SHIFT9(0xf)
#define SD_ROT_WAKE_CLR_USB_WK_BASE                     9
#define SD_ROT_WAKE_CLR_USB_WK_EN           BIT_8
#define SD_ROT_WAKE_CLR_USB_ID_WK_EN            BIT_7
#define SD_ROT_WAKE_CLR_USB_WK_CLR          BIT_6
#define SD_ROT_WAKE_CLR_USB_VBUS_WK_EN          BIT_5
#define SD_ROT_WAKE_CLR_USB_VBUS_WK_CLR         BIT_4
#define SD_ROT_WAKE_CLR_EN                          BIT_3
#define SD_ROT_WAKE_CLR_HSI_WK_CLR          BIT_2
#define SD_ROT_WAKE_CLR_SDH1_WK_CLR         BIT_1
#define SD_ROT_WAKE_CLR_SDH0_WK_CLR         BIT_0


/*  PMUA_PWR_STBL_TIMER         0x0084  Power Stable Timer Register */
/*      Bit(s) PMUA_PWR_STBL_TIMER_RSRV_31_24 reserved */
/* Clock Prescaler */
#define PWR_STBL_TIMER_PWR_CLK_PRE_MSK              SHIFT16(0xff)
#define PWR_STBL_TIMER_PWR_CLK_PRE_BASE         16
/* Power up Stable Timer */
#define PWR_STBL_TIMER_PWR_UP_STBL_TIMER_MSK        SHIFT8(0xff)
#define PWR_STBL_TIMER_PWR_UP_STBL_TIMER_BASE       8
/* Power Down Stable Timer */
#define PWR_STBL_TIMER_PWR_DWN_STBL_TIMER_MSK       SHIFT0(0xff)
#define PWR_STBL_TIMER_PWR_DWN_STBL_TIMER_BASE      0




//PMU_DEBUG_REG
#define         PMU_DEBUG_REG_CORE1_GBL_IRQ_MASK_CLR_DIS           BIT_31
#define         PMU_DEBUG_REG_CORE0_GBL_IRQ_MASK_CLR_DIS           BIT_30
#define         PMU_DEBUG_REG_CP_GBL_IRQ_MASK_CLR_DIS           BIT_29
#define         PMU_DEBUG_REG_MASK_GC_CLOCK_OFF_CHECK           BIT_27
#define         PMU_DEBUG_REG_MIPI_DPHY_VDD_DVM_VALID          BIT_26
#define         PMU_DEBUG_REG_MIPI_DPHY_VDD_ANA_VALID           BIT_25
#define         PMU_DEBUG_REG_FUNC_PLL_BYPASS           BIT_24
#define         PMU_DEBUG_REG_MASK_MC_SW_IDLE_INDICATION           BIT_23
#define         PMU_DEBUG_REG_AP_WFI_FC           BIT_22
#define         PMU_DEBUG_REG_CP_WFI_FC           BIT_21
#define         PMU_DEBUG_REG_MASK_MC_CLK_ON_CHECK          BIT_20
#define         PMU_DEBUG_REG_MASK_MC_CLK_OFF_CHECK           BIT_19
#define         PMU_DEBUG_REG_MASK_AP_JTAG_IDLE_INDICATION          BIT_18
#define         PMU_DEBUG_REG_MASK_CP_JTAG_IDLE_INDICATION           BIT_17
#define         PMU_DEBUG_REG_MASK_MAIN_CLK_ON_CHECK           BIT_16
#define         PMU_DEBUG_REG_MASK_MAIN_CLK_OFF_CHECK           BIT_15
#define         PMU_DEBUG_REG_MASK_MC_IDLE_INDICATION           BIT_14
#define         PMU_DEBUG_REG_MASK_DMA_IDLE_INDICATION           BIT_13
#define         PMU_DEBUG_REG_MASK_AP_IDLE_IND           BIT_12
#define         PMU_DEBUG_REG_MASK_CP_IDLE_IND           BIT_11
#define         PMU_DEBUG_REG_MASK_AP_CLK_ON_ACK_FOR_IDLE           BIT_10
#define         PMU_DEBUG_REG_MASK_AP_CLK_OFF_ACK_FOR_IDLE           BIT_9
#define         PMU_DEBUG_REG_MASK_CP_CLK_ON_ACK_FOR_IDLE          BIT_8
#define         PMU_DEBUG_REG_MASK_CP_CLK_OFF_ACK_FOR_IDLE           BIT_7
#define         PMU_DEBUG_REG_MASK_ACLK_CLK_OFF_ACK           BIT_6
#define         PMU_DEBUG_REG_MASK_DCLK_CLK_OFF_ACK           BIT_5
#define         PMU_DEBUG_REG_MASK_AP_CLK_OFF_ACK           BIT_4
#define         PMU_DEBUG_REG_MASK_CP_CLK_OFF_ACK           BIT_3
#define         PMU_DEBUG_REG_MASK_MC_HALT           BIT_2
#define         PMU_DEBUG_REG_MASK_AP_HALT           BIT_1
#define         PMU_DEBUG_REG_MASK_CP_HALT           BIT_0





//Core SRAM Power Down Register      0X8C
#define         SRAM_PWR_DWN_CP_l2_low_leakage_en                                     BIT_21
#define         SRAM_PWR_DWN_CP_l2_low_leakage_dis                                     BIT_20
#define         SRAM_PWR_DWN_AP_l2_low_leakage_en                                     BIT_19
#define         SRAM_PWR_DWN_AP_l2_low_leakage_dis                                    BIT_18
#define         SRAM_PWR_DWN_SQU_SRAM_PDWN_CP                                    BIT_17
#define         SRAM_PWR_DWN_SQU_SRAM_PDWN_AP                                     BIT_16
#define         SRAM_PWR_DWN_AP_low_leakage_en                                     BIT_9
#define         SRAM_PWR_DWN_AP_low_leakage_dis                                     BIT_8
#define         SRAM_PWR_DWN_CP_low_leakage_en                                     BIT_1
#define         SRAM_PWR_DWN_CP_low_leakage_dis                                     BIT_0



/*  PMUA_CORE_STATUS            0x0090  Core Status Register */
#define         CORE_STATUS_AP_MPSUB_IDLE                     BIT_7
#define         CORE_STATUS_AP_MPSUB_WFI                     BIT_6
#define         CORE_STATUS_AP_CORE1_IDLE                     BIT_5
#define         CORE_STATUS_AP_CORE1_WFI                     BIT_4
#define         CORE_STATUS_AP_CORE0_IDLE                     BIT_3
#define         CORE_STATUS_AP_CORE0_WFI                     BIT_2
#define         CORE_STATUS_CP_IDLE                     BIT_1
#define         CORE_STATUS_CP_WFI                     BIT_0



/*  PMUA_RES_FRM_SLP_CLR        0x0094  Resume from Sleep Clear Register */

/* Clear Resume from Sleep Indication */
#define RES_FRM_SLP_CLR_CLR_RSM_FRM_SLP     BIT_0

//  PMUA_MOH_IMR                0x0098  PMU <var Processor: Mohawk> Interrupt
#define AP_IMR_AP_CORE1_IDLE_WAKE_INTR_MASK                               BIT_8
#define AP_IMR_AP_CORE0_IDLE_WAKE_INTR_MASK                               BIT_7
#define AP_IMR_CP_IDLE_WAKE_INTR_MASK                               BIT_6
#define AP_IMR_ACLK_FC_DONE_INTR_MASK                               BIT_5
#define AP_IMR_DCLK_FC_DONE_INTR_MASK                               BIT_4
#define AP_IMR_AP_FC_DONE_INTR_MASK                               BIT_3
#define AP_IMR_CP_FC_DONE_INTR_MASK                               BIT_2
#define AP_IMR_AP_FC_INTR_MASK                              BIT_1
#define AP_IMR_CP_FC_INTR_MASK                               BIT_0

//PMU AP Interrupt READ/WRITE Clear Register
#define AP_IRWC_AP_CORE1_IDLE_WAKE_INTR_MASK                               BIT_8
#define AP_IRWC_AP_CORE0_IDLE_WAKE_INTR_MASK                               BIT_7
#define AP_IRWC_CP_IDLE_WAKE_INTR_MASK                               BIT_6
#define AP_IRWC_ACLK_FC_DONE_INTR_MASK                               BIT_5
#define AP_IRWC_DCLK_FC_DONE_INTR_MASK                               BIT_4
#define AP_IRWC_AP_FC_DONE_INTR_MASK                               BIT_3
#define AP_IRWC_CP_FC_DONE_INTR_MASK                               BIT_2
#define AP_IRWC_AP_FC_INTR_MASK                              BIT_1
#define AP_IRWC_CP_FC_INTR_MASK                               BIT_0

//PMU AP Interrupt Status Register 0X0A0
#define AP_ISR_AP_CORE1_IDLE_WAKE_INTR_MASK                               BIT_8
#define AP_ISR_AP_CORE0_IDLE_WAKE_INTR_MASK                               BIT_7
#define AP_ISR_CP_IDLE_WAKE_INTR_MASK                               BIT_6
#define AP_ISR_ACLK_FC_DONE_INTR_MASK                               BIT_5
#define AP_ISR_DCLK_FC_DONE_INTR_MASK                               BIT_4
#define AP_ISR_AP_FC_DONE_INTR_MASK                               BIT_3
#define AP_ISR_CP_FC_DONE_INTR_MASK                               BIT_2
#define AP_ISR_AP_FC_INTR_MASK                              BIT_1
#define AP_ISR_CP_FC_INTR_MASK                               BIT_0


//VPU(7542) Clock/Reset Control Register
#define VPU_CLK_RES_CTRL_VPU_REF_SEL_MSK                  SHIFT26(0x3)
#define VPU_CLK_RES_CTRL_VPU_REF_SEL_BASE                   26
#define VPU_CLK_RES_CTRL_VPU_REFCLK_EN                   BIT_25
#define VPU_CLK_RES_CTRL_VPU_REFCLK_RESET                   BIT_24
#define VPU_CLK_RES_CTRL_VPU_ACLK_FC_REQ                   BIT_21
#define VPU_CLK_RES_CTRL_VPU_FNC_FC_REQ                   BIT_20
#define VPU_CLK_RES_CTRL_DX8_HW_MODE                   BIT_19
#define VPU_CLK_RES_CTRL_DX8_SLEEP2                   BIT_18
#define VPU_CLK_RES_CTRL_DX8_SLEEP1                   BIT_17
#define VPU_CLK_RES_CTRL_DX8_ISOB                   BIT_16
#define VPU_CLK_RES_CTRL_VPU_ACLK_DIV_MSK                   SHIFT13(0x7)
#define VPU_CLK_RES_CTRL_VPU_ACLK_DIV_BASE                    13
#define VPU_CLK_RES_CTRL_VPU_ACLK_SEL_MSK                   SHIFT11(0x3)
#define VPU_CLK_RES_CTRL_VPU_ACLK_SEL_BASE                  11
#define VPU_CLK_RES_CTRL_VPU_FNC_CLK_DIV_MSK                   SHIFT8(0x7)
#define VPU_CLK_RES_CTRL_VPU_FNC_CLK_DIV_BASE                   8
#define VPU_CLK_RES_CTRL_VPU_FNC_SEL_MSK                  SHIFT6(0x3)
#define VPU_CLK_RES_CTRL_VPU_FNC_SEL_BASE                  6
#define VPU_CLK_RES_CTRL_VPU_AHBCLK_EN                   BIT_5
#define VPU_CLK_RES_CTRL_VPU_FNC_CLK_EN                   BIT_4
#define VPU_CLK_RES_CTRL_VPU_AXICLK_EN                   BIT_3
#define VPU_CLK_RES_CTRL_VPU_AHB_RST                   BIT_2
#define VPU_CLK_RES_CTRL_VPU_FNC_RST1                   BIT_1
#define VPU_CLK_RES_CTRL_VPU_AXI_RST                   BIT_0


/*  PMUA_DTC_CLK_RES_CTRL       0x00AC  DTC Clock/Reset Control Register */


#define DTC_CLK_RES_CTRL_DTC_AXICLK_EN          BIT_3
#define DTC_CLK_RES_CTRL_DTC_AXI_RST            BIT_0

//Memory Controller Hardware Sleep Type Register
#define MC_HW_SLP_TYPE_ddr3_4x_clk_mode               BIT_7
#define MC_HW_SLP_TYPE_mc_reg_table_en               BIT_6
#define MC_HW_SLP_TYPE_mc_reg_table_num_MSK          SHIFT3(0x7)
#define MC_HW_SLP_TYPE_mc_reg_table_num_BASE               3
#define MC_HW_SLP_TYPE_MC_HW_SLP_TYPE_MSK               SHIFT0(0x3)
#define MC_HW_SLP_TYPE_MC_HW_SLP_TYPE_BASE               0


/*  PMUA_MC_SLP_REQ_MOH         0x00B4  Memory Controller <var Processor:
 *                                      Mohawk> Sleep Request Register
 */
/*      Bit(s) PMUA_MC_SLP_REQ_MOH_RSRV_31_2 reserved */
/* Memory Controller Low Power Acknowledge */
#define MC_SLP_REQ_AP_MC_SLP_ACK            BIT_1
/* Memory Controller <var Processor: Mohawk> Sleep Request */
#define MC_SLP_REQ_AP_MC_SLP_REQ_AP     BIT_0




/*  PMUA_MC_SLP_REQ_MOH         0x00B8  Memory Controller <var Processor:
 *                                      Mohawk> Sleep Request Register
 */
/*      Bit(s) PMUA_MC_SLP_REQ_MOH_RSRV_31_2 reserved */
/* Memory Controller Low Power Acknowledge */
#define MC_SLP_REQ_CP_MC_SLP_ACK            BIT_1
/* Memory Controller <var Processor: Mohawk> Sleep Request */
#define MC_SLP_REQ_CP_MC_SLP_REQ_CP     BIT_0




/*  PMUA_MC_SLP_REQ_MOH         0x00BC  Memory Controller <var Processor:
 *                                      Mohawk> Sleep Request Register
 */
/*      Bit(s) PMUA_MC_SLP_REQ_MOH_RSRV_31_2 reserved */
/* Memory Controller Low Power Acknowledge */
#define MC_SLP_REQ_MSA_MC_SLP_ACK           BIT_1
/* Memory Controller <var Processor: Mohawk> Sleep Request */
#define MC_SLP_REQ_MSA_MC_SLP_REQ_MSA       BIT_0



/*  PMUA_PLL_SEL_STATUS         0x00C4  PLL Clock Select Status Register */
/*      Bit(s) PMUA_PLL_SEL_STATUS_RSRV_31_8 reserved */
/* BUS Clock PLL Select */
#define PLL_SEL_STATUS_ACLK_PLL_SEL_MSK     SHIFT6(0x3)
#define PLL_SEL_STATUS_ACLK_PLL_SEL_BASE        6
/* Memory Clock PLL Select */
#define PLL_SEL_STATUS_DCLK_PLL_SEL_MSK     SHIFT4(0x3)
#define PLL_SEL_STATUS_DCLK_PLL_SEL_BASE        4
/* <var Processor: Mohawk> PLL Select */
#define PLL_SEL_STATUS_MOH_PLL_SEL_MSK          SHIFT2(0x3)
#define PLL_SEL_STATUS_MOH_PLL_SEL_BASE     2
/* <var Processor: Osprey> PLL Select */
#define PLL_SEL_STATUS_SEA_PLL_SEL_MSK          SHIFT0(0x3)
#define PLL_SEL_STATUS_SEA_PLL_SEL_BASE     0


//SYNC MODE BYPASS Register
#define SYNC_MODE_BYPASS_mc_sync_bypass                BIT_2
#define SYNC_MODE_BYPASS_ap_sync_bypass                BIT_1
#define SYNC_MODE_BYPASS_cp_sync_bypass                BIT_0


/*  PMUA_GC_CLK_RES_CTRL        0x00CC  2D/3D Graphics Controller Clock/Reset
 *                                      Control Register
 */
#define GC_CLK_RES_CTRL_GC_ACLK_SEL_MSK                      SHIFT20(0x3)
#define GC_CLK_RES_CTRL_GC_ACLK_SEL_BASE                      20
#define GC_CLK_RES_CTRL_GC_aclk_DIV_MSK                       SHIFT17(0x7)
#define GC_CLK_RES_CTRL_GC_aclk_DIV_BASE                     17
#define GC_CLK_RES_CTRL_GC_ACLK_FC_REQ                      BIT_16
#define GC_CLK_RES_CTRL_GC_FNC_FC_REQ                       BIT_15
#define GC_CLK_RES_CTRL_GC_fnc_clk_DIV_MSK                      SHIFT12(0x7)
#define GC_CLK_RES_CTRL_GC_fnc_clk_DIV_BASE                     12
#define GC_CLK_RES_CTRL_HW_MODE                      BIT_11
#define GC_CLK_RES_CTRL_GC_SLEEP2                      BIT_10
#define GC_CLK_RES_CTRL_GC_SLEEP1                       BIT_9
#define GC_CLK_RES_CTRL_GC_ISOB                       BIT_8
#define GC_CLK_RES_CTRL_GC_CLK_SEL_MSK          SHIFT6(0x3)
#define       GC_CLK_RES_CTRL_GC_CLK_SEL_BASE       6
#define GC_CLK_RES_CTRL_gc_HCLK_EN                       BIT_5
#define GC_CLK_RES_CTRL_GC_CLK_EN               BIT_4
#define GC_CLK_RES_CTRL_GC_AXICLK_EN            BIT_3
#define GC_CLK_RES_CTRL_gc_HCLK_RST                      BIT_2
#define GC_CLK_RES_CTRL_GC_RST1             BIT_1
#define GC_CLK_RES_CTRL_GC_AXI_RST              BIT_0



/*  PMUA_SMC_CLK_RES_CTRL       0x00D4  SMC Clock/Reset Control Register */


#define SMC_CLK_RES_CTRL_SMC_CLK_SEL_MSK            SHIFT6(0x3)
#define SMC_CLK_RES_CTRL_SMC_CLK_SEL_SHIFT               6

#define SMC_CLK_RES_CTRL_SMC_CLK_EN         BIT_4
#define SMC_CLK_RES_CTRL_SMC_AXICLK_EN          BIT_3
#define SMC_CLK_RES_CTRL_SMC_RST                BIT_1
#define SMC_CLK_RES_CTRL_SMC_AXI_RST            BIT_0




//Power Control Register 0X00D8
#define PWR_CTRL_REG_ISP_AUTO_PWR_ON          BIT_4
#define PWR_CTRL_REG_DX8_AUTO_PWR_ON          BIT_2
#define PWR_CTRL_REG_GC_AUTO_PWR_ON          BIT_0

//SDH2 Clock/Reset Control Register
#define SDH2_CLK_RES_CTRL_SDH2_CLK_FC_REQ            BIT_11
#define SDH2_CLK_RES_CTRL_SDH2_CLK_DIV_MSK             SHIFT8(0x7)
#define SDH2_CLK_RES_CTRL_SDH2_CLK_DIV_SHIFT             8
#define SDH2_CLK_RES_CTRL_SDH2_CLK_SEL             BIT_6
#define SDH2_CLK_RES_CTRL_SDH2_CLK_EN             BIT_4
#define SDH2_CLK_RES_CTRL_SDH2_RST             BIT_1





//CA7 MP Idle Configuration Register for Core1
#define CA7MP_IDLE_CFG1_DIS_MP_SCU_SLP                              BIT_20
#define CA7MP_IDLE_CFG1_DIS_MP_L2_SLP                              BIT_19
#define CA7MP_IDLE_CFG1_DIS_MP_SLP                              BIT_18
#define CA7MP_IDLE_CFG1_MP_SCU_PWR_OFF                              BIT_17
#define CA7MP_IDLE_CFG1_MP_L2_PWR_OFF                              BIT_16
#define CA7MP_IDLE_CFG1_MASK_SRAM_REPAIR_DONE_CHECK                              BIT_12
#define CA7MP_IDLE_CFG1_MASK_CLK_OFF_CHECK                              BIT_11
#define CA7MP_IDLE_CFG1_MASK_CLK_STBL_CHECK                              BIT_10
#define CA7MP_IDLE_CFG1_MASK_JTAG_IDLE_CHECK                              BIT_9
#define CA7MP_IDLE_CFG1_MASK_MP_IDLE_CHECK                              BIT_8
#define CA7MP_IDLE_CFG1_ICU_INT_WAKEUP_CORE_EN                              BIT_7
#define CA7MP_IDLE_CFG1_MP_UDR_EN                              BIT_6
#define CA7MP_IDLE_CFG1_DIS_MC_SW_REQ                              BIT_5
#define CA7MP_IDLE_CFG1_MP_MC_WAKE_EN                              BIT_4
#define CA7MP_IDLE_CFG1_MP_SCU_SRAM_PWRDWN                              BIT_3
#define CA7MP_IDLE_CFG1_MP_L2_SRAM_PWRDWN                              BIT_2
#define CA7MP_IDLE_CFG1_MP_PWRDWN                              BIT_1
#define CA7MP_IDLE_CFG1_MP_IDLE                             BIT_0



//AP Clock Control Register2
#define CC2_AP_PERI_CLK_DIS                                 BIT_3
#define CC2_AP_PERI_CLK_DIV_MSK                           SHIFT0(0x7)
#define CC2_AP_PERI_CLK_DIV_BASE                         0

#define CC2_AP_CPU0_POR_RST          BIT_16
#define CC2_AP_CPU0_SW_RST           BIT_17
#define CC2_AP_CPU0_DBG_RST          BIT_18

#define CC2_AP_CPU1_POR_RST          BIT_19
#define CC2_AP_CPU1_SW_RST           BIT_20
#define CC2_AP_CPU1_DBG_RST          BIT_21

#define CC2_AP_CPU2_POR_RST          BIT_22
#define CC2_AP_CPU2_SW_RST           BIT_23
#define CC2_AP_CPU2_DBG_RST          BIT_24

#define CC2_AP_CPU3_POR_RST          BIT_25
#define CC2_AP_CPU3_SW_RST           BIT_26
#define CC2_AP_CPU3_DBG_RST          BIT_27

//Trace clock control Register  0X108
#define TRACE_CONFIG_PCLKDBG_DIV_MSK                            SHIFT19(0x7)
#define TRACE_CONFIG_PCLKDBG_DIV_BASE                           19
#define TRACE_CONFIG_DBG_CLK_SEL_MSK                            SHIFT17(0x3)
#define TRACE_CONFIG_DBG_CLK_SEL_BASE                            17
#define TRACE_CONFIG_SWRST                            BIT_16
#define TRACE_CONFIG_TRACE_CLK_FC_REQ                            BIT_15
#define TRACE_CONFIG_TPIU_CLK_SEL                            BIT_11
#define TRACE_CONFIG_TRACE_CLK_DIV_MSK                            SHIFT8(0x7)
#define TRACE_CONFIG_TRACE_CLK_DIV_BASE                            8
#define TRACE_CONFIG_TRACE_CLK_SEL_MSK                            SHIFT6(0x3)
#define TRACE_CONFIG_TRACE_CLK_SEL_BASE                            6
#define TRACE_CONFIG_DBG_CLK_FC_REQ                            BIT_5
#define TRACE_CONFIG_TRACE_CLK_EN                            BIT_4
#define TRACE_CONFIG_DBGCLK_EN                            BIT_3
#define TRACE_CONFIG_ATCLK_DIV_MSK                            SHIFT0(0x7)
#define TRACE_CONFIG_ATCLK_DIV_BASE                            0



//CA7 MP Idle Configuration Register for Core0
#define CA7MP_IDLE_CFG0_DIS_MP_SCU_SLP                          BIT_20
#define CA7MP_IDLE_CFG0_DIS_MP_L2_SLP                          BIT_19
#define CA7MP_IDLE_CFG0_DIS_MP_SLP                          BIT_18
#define CA7MP_IDLE_CFG0_MP_SCU_PWR_OFF                          BIT_17
#define CA7MP_IDLE_CFG0_MP_L2_PWR_OFF                          BIT_16
#define CA7MP_IDLE_CFG0_MASK_SRAM_REPAIR_DONE_CHECK                          BIT_12
#define CA7MP_IDLE_CFG0_MASK_CLK_OFF_CHECK                          BIT_11
#define CA7MP_IDLE_CFG0_MASK_CLK_STBL_CHECK                          BIT_10
#define CA7MP_IDLE_CFG0_MASK_JTAG_IDLE_CHECK                          BIT_9
#define CA7MP_IDLE_CFG0_MASK_MP_IDLE_CHECK                          BIT_8
#define CA7MP_IDLE_CFG0_ICU_INT_WAKEUP_CORE_EN                          BIT_7
#define CA7MP_IDLE_CFG0_MP_UDR_EN                          BIT_6
#define CA7MP_IDLE_CFG0_DIS_MC_SW_REQ                          BIT_5
#define CA7MP_IDLE_CFG0_MP_MC_WAKE_EN                          BIT_4
#define CA7MP_IDLE_CFG0_MP_SCU_SRAM_PWRDWN                          BIT_3
#define CA7MP_IDLE_CFG0_MP_L2_SRAM_PWRDWN                          BIT_2
#define CA7MP_IDLE_CFG0_MP_PWRDWN                          BIT_1
#define CA7MP_IDLE_CFG0_MP_IDLE                          BIT_0


//CA7 Core0 Idle Configuration Register
#define CA7_CORE0_IDLE_CFG_DIS_CORE_L1_SLP                    BIT_19
#define CA7_CORE0_IDLE_CFG_DIS_CORE_SLP                    BIT_18
#define CA7_CORE0_IDLE_CFG_MASK_CLK_OFF_CHECK                    BIT_11
#define CA7_CORE0_IDLE_CFG_MASK_CLK_STBL_CHECK                    BIT_10
#define CA7_CORE0_IDLE_CFG_MASK_JTAG_IDLE_CHECK                    BIT_9
#define CA7_CORE0_IDLE_CFG_MASK_CORE_WFI_IDLE_CHECK                    BIT_8
#define CA7_CORE0_IDLE_CFG_MASK_GIC_nFIQ_TO_CORE                    BIT_4
#define CA7_CORE0_IDLE_CFG_MASK_GIC_nIRQ_TO_CORE                    BIT_3
#define CA7_CORE0_IDLE_CFG_CORE_L1_SRAM_PWRDWN                    BIT_2
#define CA7_CORE0_IDLE_CFG_CORE_PWRDWN                    BIT_1
#define CA7_CORE0_IDLE_CFG_CORE_IDLE                    BIT_0


//CA7 Core1 Idle Configuration Register  0X128
#define CA7_CORE1_IDLE_CFG_DIS_CORE_L1_SLP               BIT_19
#define CA7_CORE1_IDLE_CFG_DIS_CORE_SLP               BIT_18
#define CA7_CORE1_IDLE_CFG_MASK_CLK_OFF_CHECK               BIT_11
#define CA7_CORE1_IDLE_CFG_MASK_CLK_STBL_CHECK               BIT_10
#define CA7_CORE1_IDLE_CFG_MASK_JTAG_IDLE_CHECK               BIT_9
#define CA7_CORE1_IDLE_CFG_MASK_CORE_WFI_IDLE_CHECK               BIT_8
#define CA7_CORE1_IDLE_CFG_MASK_GIC_nFIQ_TO_CORE              BIT_4
#define CA7_CORE1_IDLE_CFG_MASK_GIC_nIRQ_TO_CORE               BIT_3
#define CA7_CORE1_IDLE_CFG_CORE_L1_SRAM_PWRDWN               BIT_2
#define CA7_CORE1_IDLE_CFG_CORE_PWRDWN               BIT_1
#define CA7_CORE1_IDLE_CFG_CORE_IDLE               BIT_0







#endif
