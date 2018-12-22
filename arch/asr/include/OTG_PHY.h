#ifndef __OTG_PHY_H__
#define __OTG_PHY_H__

typedef struct
{

    __IO uint32_t usb2_phy_id_reg0;
    __IO uint32_t usb2_pll_reg0;
    __IO uint32_t usb2_pll_reg1;
    __IO uint32_t RESERVED0;
    __IO uint32_t usb2_tx_reg0;
    __IO uint32_t usb2_tx_reg1;
    __IO uint32_t usb2_tx_reg2;
    __IO uint32_t RESERVED1;
    __IO uint32_t usb2_rx_reg0;
    __IO uint32_t usb2_rx_reg1;
    __IO uint32_t usb2_rx_reg2;
    __IO uint32_t RESERVED2;
    __IO uint32_t usb2_ana_reg0;//0x0030
    __IO uint32_t usb2_ana_reg1;
    __IO uint32_t RESERVED3;
    __IO uint32_t usb2_dig_reg0;
    __IO uint32_t usb2_dig_reg1;
    __IO uint32_t usb2_dig_reg2;
    __IO uint32_t RESERVED4;
    __IO uint32_t usb2_test_reg0;
    __IO uint32_t usb2_test_reg1;
    __IO uint32_t RESERVED5;
    __IO uint32_t usb2_charger_reg0;
    __IO uint32_t usb2_otg_reg0;//0x005C
    __IO uint32_t usb2_phy_mon0;
    __IO uint32_t usb2_reserve_reg0;  //0X64
    __IO uint32_t RESERVED6[(0x78-0x64)/4-1];
    __IO uint32_t usb2_icid_reg0;  //0X78
    __IO uint32_t usb2_icid_reg1;



}OTG_PHY_TypeDef;



#define    OTG_PHY_BASE      0x00000000  //???


#define    OTG_PHY  (( OTG_PHY_TypeDef *) OTG_PHY_BASE )




//Bitfield Constants Register usb2_phy_id_reg0
#define PHY_ID_REG0_CID0_MASK                             0xFF00
#define PHY_ID_REG0_CID0_SHIFT                            8
#define PHY_ID_REG0_CID1_MASK                             0x00FF
#define PHY_ID_REG0_CID1_SHIFT                            0

//Bitfield Constants Register usb2_pll_reg0
#define PLL_REG0_PLLVDD18_MASK                         0xC000
#define PLL_REG0_PLLVDD18_SHIFT                        14
#define PLL_REG0_REFDIV_MASK                           0x3E00
#define PLL_REG0_REFDIV_SHIFT                          9
#define PLL_REG0_FBDIV_MASK                            0x01FF
#define PLL_REG0_FBDIV_SHIFT                           0



//Bitfield Constants Register usb2_pll_reg1
#define PLL_REG1_PLL_READY_MASK                        0x8000
#define PLL_REG1_PLL_READY_SHIFT                       15
#define PLL_REG1_PLL_CONTROL_BY_PIN_MASK               0x4000
#define PLL_REG1_PLL_CONTROL_BY_PIN_SHIFT              14
#define PLL_REG1_PU_PLL_MASK                           0x2000
#define PLL_REG1_PU_PLL_SHIFT                          13
#define PLL_REG1_PLL_LOCK_BYPASS_MASK                  0x1000
#define PLL_REG1_PLL_LOCK_BYPASS_SHIFT                 12
#define PLL_REG1_LOCKDET_ISEL_MASK                     0x0800
#define PLL_REG1_LOCKDET_ISEL_SHIFT                    11
#define PLL_REG1_ICP_MASK                              0x0700
#define PLL_REG1_ICP_SHIFT                             8
#define PLL_REG1_KVCO_EXT_MASK                         0x0080
#define PLL_REG1_KVCO_EXT_SHIFT                        7
#define PLL_REG1_KVCO_MASK                             0x0070
#define PLL_REG1_KVCO_SHIFT                            4
#define PLL_REG1_CLK_BLK_EN_MASK                       0x0008
#define PLL_REG1_CLK_BLK_EN_SHIFT                      3
#define PLL_REG1_VCOCAL_START_MASK                     0x0004
#define PLL_REG1_VCOCAL_START_SHIFT                    2
#define PLL_REG1_PLLCAL12_MASK                         0x0003
#define PLL_REG1_PLLCAL12_SHIFT                        0

//Bitfield Constants Register usb2_tx_reg0
#define TX_REG0_RCAL_START_MASK                       0x2000
#define TX_REG0_RCAL_START_SHIFT                      13
#define TX_REG0_EXT_HS_RCAL_EN_MASK                   0x1000
#define TX_REG0_EXT_HS_RCAL_EN_SHIFT                  12
#define TX_REG0_EXT_FS_RCAL_EN_MASK                   0x0800
#define TX_REG0_EXT_FS_RCAL_EN_SHIFT                  11
#define TX_REG0_IMPCAL_VTH_MASK                       0x0700
#define TX_REG0_IMPCAL_VTH_SHIFT                      8
#define TX_REG0_EXT_HS_RCAL_MASK                      0x00F0
#define TX_REG0_EXT_HS_RCAL_SHIFT                     4
#define TX_REG0_EXT_FS_RCAL_MASK                      0x000F
#define TX_REG0_EXT_FS_RCAL_SHIFT                     0


//Bitfield Constants Register usb2_tx_reg1
#define TX_REG1_TXVDD15_MASK                          0x0C00
#define TX_REG1_TXVDD15_SHIFT                         10
#define TX_REG1_TXVDD12_MASK                          0x0300
#define TX_REG1_TXVDD12_SHIFT                         8
#define TX_REG1_LOWVDD_EN_MASK                        0x0080
#define TX_REG1_LOWVDD_EN_SHIFT                       7
#define TX_REG1_AMP_MASK                              0x0070
#define TX_REG1_AMP_SHIFT                             4
#define TX_REG1_CK60_PHSEL_MASK                       0x000F
#define TX_REG1_CK60_PHSEL_SHIFT                      0

//Bitfield Constants Register usb2_tx_reg2
#define TX_REG2_DRV_SLEWRATE_MASK                     0x0C00
#define TX_REG2_DRV_SLEWRATE_SHIFT                    10
#define TX_REG2_IMP_CAL_DLY_MASK                      0x0300
#define TX_REG2_IMP_CAL_DLY_SHIFT                     8
#define TX_REG2_FSDRV_EN_MASK                         0x00F0
#define TX_REG2_FSDRV_EN_SHIFT                        4
#define TX_REG2_HSDRV_EN_MASK                         0x000F
#define TX_REG2_HSDRV_EN_SHIFT                        0

//Bitfield Constants Register usb2_rx_reg0
#define RX_REG0_PHASE_FREEZE_DLY_MASK                 0x8000
#define RX_REG0_PHASE_FREEZE_DLY_SHIFT                15
#define RX_REG0_USQ_LENGTH_MASK                       0x4000
#define RX_REG0_USQ_LENGTH_SHIFT                      14
#define RX_REG0_ACQ_LENGTH_MASK                       0x3000
#define RX_REG0_ACQ_LENGTH_SHIFT                      12
#define RX_REG0_SQ_LENGTH_MASK                        0x0C00
#define RX_REG0_SQ_LENGTH_SHIFT                       10
#define RX_REG0_DISCON_THRESH_MASK                    0x0300
#define RX_REG0_DISCON_THRESH_SHIFT                   8
#define RX_REG0_SQ_THRESH_MASK                        0x00F0
#define RX_REG0_SQ_THRESH_SHIFT                       4
#define RX_REG0_LPF_COEF_MASK                         0x000C
#define RX_REG0_LPF_COEF_SHIFT                        2
#define RX_REG0_INTPI_MASK                            0x0003
#define RX_REG0_INTPI_SHIFT                           0


//Bitfield Constants Register usb2_rx_reg1
#define RX_REG1_EARLY_VOS_ON_EN_MASK                  0x2000
#define RX_REG1_EARLY_VOS_ON_EN_SHIFT                 13
#define RX_REG1_RXDATA_BLOCK_EN_MASK                  0x1000
#define RX_REG1_RXDATA_BLOCK_EN_SHIFT                 12
#define RX_REG1_EDGE_DET_EN_MASK                      0x0800
#define RX_REG1_EDGE_DET_EN_SHIFT                     11
#define RX_REG1_CAP_SEL_MASK                          0x0700
#define RX_REG1_CAP_SEL_SHIFT                         8
#define RX_REG1_RXDATA_BLOCK_LENGTH_MASK              0x00C0
#define RX_REG1_RXDATA_BLOCK_LENGTH_SHIFT             6
#define RX_REG1_EDGE_DET_SEL_MASK                     0x0030
#define RX_REG1_EDGE_DET_SEL_SHIFT                    4
#define RX_REG1_CDR_COEF_SEL_MASK                     0x0008
#define RX_REG1_CDR_COEF_SEL_SHIFT                    3
#define RX_REG1_CDR_FASTLOCK_EN_MASK                  0x0004
#define RX_REG1_CDR_FASTLOCK_EN_SHIFT                 2
#define RX_REG1_S2TO3_DLY_SEL_MASK                    0x0003
#define RX_REG1_S2TO3_DLY_SEL_SHIFT                   0



//Bitfield Constants Register usb2_rx_reg2
#define RX_REG2_USQ_FILTER_MASK                       0x0100
#define RX_REG2_USQ_FILTER_SHIFT                      8
#define RX_REG2_SQ_CM_SEL_MASK                        0x0080
#define RX_REG2_SQ_CM_SEL_SHIFT                       7
#define RX_REG2_SAMPLER_CTRL_MASK                     0x0040
#define RX_REG2_SAMPLER_CTRL_SHIFT                    6
#define RX_REG2_SQ_BUFFER_EN_MASK                     0x0020
#define RX_REG2_SQ_BUFFER_EN_SHIFT                    5
#define RX_REG2_SQ_ALWAYS_ON_MASK                     0x0010
#define RX_REG2_SQ_ALWAYS_ON_SHIFT                    4
#define RX_REG2_RXVDD18_MASK                          0x000C
#define RX_REG2_RXVDD18_SHIFT                         2
#define RX_REG2_RXVDD12_MASK                          0x0003
#define RX_REG2_RXVDD12_SHIFT                         0


//Bitfield Constants Register usb2_ana_reg0
#define ANA_REG0_BG_VSEL_MASK                          0x0300
#define ANA_REG0_BG_VSEL_SHIFT                         8
#define ANA_REG0_DIG_SEL_MASK                          0x00C0
#define ANA_REG0_DIG_SEL_SHIFT                         6
#define ANA_REG0_TOPVDD18_MASK                         0x0030
#define ANA_REG0_TOPVDD18_SHIFT                        4
#define ANA_REG0_VDD_USB2_DIG_TOP_SEL_MASK             0x0008
#define ANA_REG0_VDD_USB2_DIG_TOP_SEL_SHIFT            3
#define ANA_REG0_IPTAT_SEL_MASK                        0x0007
#define ANA_REG0_IPTAT_SEL_SHIFT                       0


//Bitfield Constants Register usb2_ana_reg1
#define ANA_REG1_PU_ANA_MASK                           0x4000
#define ANA_REG1_PU_ANA_SHIFT                          14
#define ANA_REG1_ANA_CONTROL_BY_PIN_MASK               0x2000
#define ANA_REG1_ANA_CONTROL_BY_PIN_SHIFT              13
#define ANA_REG1_SEL_LPFR_MASK                         0x1000
#define ANA_REG1_SEL_LPFR_SHIFT                        12
#define ANA_REG1_V2I_EXT_MASK                          0x0800
#define ANA_REG1_V2I_EXT_SHIFT                         11
#define ANA_REG1_V2I_MASK                              0x0700
#define ANA_REG1_V2I_SHIFT                             8
#define ANA_REG1_R_ROTATE_SEL_MASK                     0x0080
#define ANA_REG1_R_ROTATE_SEL_SHIFT                    7
#define ANA_REG1_STRESS_TEST_MODE_MASK                 0x0040
#define ANA_REG1_STRESS_TEST_MODE_SHIFT                6
#define ANA_REG1_TESTMON_ANA_MASK                      0x003F
#define ANA_REG1_TESTMON_ANA_SHIFT                     0



//Bitfield Constants Register usb2_dig_reg0
#define DIG_REG0_FIFO_UF_MASK                          0x8000
#define DIG_REG0_FIFO_UF_SHIFT                         15
#define DIG_REG0_FIFO_OV_MASK                          0x4000
#define DIG_REG0_FIFO_OV_SHIFT                         14
#define DIG_REG0_FS_EOP_MODE_MASK                      0x2000
#define DIG_REG0_FS_EOP_MODE_SHIFT                     13
#define DIG_REG0_HOST_DISCON_SEL1_MASK                 0x1000
#define DIG_REG0_HOST_DISCON_SEL1_SHIFT                12
#define DIG_REG0_HOST_DISCON_SEL0_MASK                 0x0800
#define DIG_REG0_HOST_DISCON_SEL0_SHIFT                11
#define DIG_REG0_FORCE_END_EN_MASK                     0x0400
#define DIG_REG0_FORCE_END_EN_SHIFT                    10
#define DIG_REG0_EARLY_TX_EN_MASK                      0x0200
#define DIG_REG0_EARLY_TX_EN_SHIFT                     9
#define DIG_REG0_SYNCDET_WINDOW_EN_MASK                0x0100
#define DIG_REG0_SYNCDET_WINDOW_EN_SHIFT               8
#define DIG_REG0_CLK_SUSPEND_EN_MASK                   0x0080
#define DIG_REG0_CLK_SUSPEND_EN_SHIFT                  7
#define DIG_REG0_HS_DRIBBLE_EN_MASK                    0x0040
#define DIG_REG0_HS_DRIBBLE_EN_SHIFT                   6
#define DIG_REG0_SYNC_NUM_MASK                         0x0030
#define DIG_REG0_SYNC_NUM_SHIFT                        4
#define DIG_REG0_FIFO_FILL_NUM_MASK                    0x000F
#define DIG_REG0_FIFO_FILL_NUM_SHIFT                   0


//Bitfield Constants Register usb2_dig_reg1
#define DIG_REG1_FS_RX_ERROR_MODE2_MASK                0x8000
#define DIG_REG1_FS_RX_ERROR_MODE2_SHIFT               15
#define DIG_REG1_FS_RX_ERROR_MODE1_MASK                0x4000
#define DIG_REG1_FS_RX_ERROR_MODE1_SHIFT               14
#define DIG_REG1_FS_RX_ERROR_MODE_MASK                 0x2000
#define DIG_REG1_FS_RX_ERROR_MODE_SHIFT                13
#define DIG_REG1_CLK_OUT_SEL_MASK                      0x1000
#define DIG_REG1_CLK_OUT_SEL_SHIFT                     12
#define DIG_REG1_EXT_TX_CLK_SEL_MASK                   0x0800
#define DIG_REG1_EXT_TX_CLK_SEL_SHIFT                  11
#define DIG_REG1_ARC_DPDM_MODE_MASK                    0x0400
#define DIG_REG1_ARC_DPDM_MODE_SHIFT                   10
#define DIG_REG1_DP_PULLDOWN_MASK                      0x0200
#define DIG_REG1_DP_PULLDOWN_SHIFT                     9
#define DIG_REG1_DM_PULLDOWN_MASK                      0x0100
#define DIG_REG1_DM_PULLDOWN_SHIFT                     8
#define DIG_REG1_SYNC_IGNORE_SQ_MASK                   0x0080
#define DIG_REG1_SYNC_IGNORE_SQ_SHIFT                  7
#define DIG_REG1_SQ_RST_RX_MASK                        0x0040
#define DIG_REG1_SQ_RST_RX_SHIFT                       6
#define DIG_REG1_MON_SEL_MASK                          0x003F
#define DIG_REG1_MON_SEL_SHIFT                         0

//Bitfield Constants Register usb2_dig_reg2
#define DIG_REG2_PAD_STRENGTH__4_0_MASK                0x1F00
#define DIG_REG2_PAD_STRENGTH__4_0_SHIFT               8
#define DIG_REG2_LONG_EOP_MASK                         0x0020
#define DIG_REG2_LONG_EOP_SHIFT                        5
#define DIG_REG2_DISABLE_EL16_MASK                     0x0008
#define DIG_REG2_DISABLE_EL16_SHIFT                    3
#define DIG_REG2_ALIGN_FS_OUTEN_MASK                   0x0004
#define DIG_REG2_ALIGN_FS_OUTEN_SHIFT                  2
#define DIG_REG2_HS_HDL_SYNC_MASK                      0x0002
#define DIG_REG2_HS_HDL_SYNC_SHIFT                     1
#define DIG_REG2_FS_HDL_OPMD_MASK                      0x0001
#define DIG_REG2_FS_HDL_OPMD_SHIFT                     0


//Bitfield Constants Register usb2_test_reg0
#define TEST_REG0_TEST_DIG_LPBK_MASK                    0x8000
#define TEST_REG0_TEST_DIG_LPBK_SHIFT                   15
#define TEST_REG0_TEST_LPBK_EN_MASK                     0x4000
#define TEST_REG0_TEST_LPBK_EN_SHIFT                    14
#define TEST_REG0_TEST_LENGTH__MASK                     0x3000
#define TEST_REG0_TEST_LENGTH__SHIFT                    12
#define TEST_REG0_TEST_BYPASS_MASK                      0x0800
#define TEST_REG0_TEST_BYPASS_SHIFT                     11
#define TEST_REG0_TEST_MODE_MASK                        0x0700
#define TEST_REG0_TEST_MODE_SHIFT                       8
#define TEST_REG0_TEST_TX_PATTERN_MASK                  0x00FF
#define TEST_REG0_TEST_TX_PATTERN_SHIFT                 0

//Bitfield Constants Register usb2_test_reg1
#define TEST_REG1_TEST_DONE_MASK                        0x8000
#define TEST_REG1_TEST_DONE_SHIFT                       15
#define TEST_REG1_TEST_FLAG_MASK                        0x4000
#define TEST_REG1_TEST_FLAG_SHIFT                       14
#define TEST_REG1_TEST_EN_MASK                          0x2000
#define TEST_REG1_TEST_EN_SHIFT                         13
#define TEST_REG1_TEST_RESET_MASK                       0x1000
#define TEST_REG1_TEST_RESET_SHIFT                      12
#define TEST_REG1_TEST_SKIP__2_0_MASK                   0x0700
#define TEST_REG1_TEST_SKIP__2_0_SHIFT                  8
#define TEST_REG1_TEST_UTMI_SEL_MASK                    0x0080
#define TEST_REG1_TEST_UTMI_SEL_SHIFT                   7
#define TEST_REG1_TEST_SUSPENDM_MASK                    0x0040
#define TEST_REG1_TEST_SUSPENDM_SHIFT                   6
#define TEST_REG1_TEST_TX_BITSTUFF_EN_MASK              0x0020
#define TEST_REG1_TEST_TX_BITSTUFF_EN_SHIFT             5
#define TEST_REG1_TEST_TERM_SELECT_MASK                 0x0010
#define TEST_REG1_TEST_TERM_SELECT_SHIFT                4
#define TEST_REG1_TEST_OP_MODE_MASK                     0x000C
#define TEST_REG1_TEST_OP_MODE_SHIFT                    2
#define TEST_REG1_TEST_XCVR_SELECT_MASK                 0x0003
#define TEST_REG1_TEST_XCVR_SELECT_SHIFT                0


//Bitfield Constants Register usb2_charger_reg0
#define CHARGER_REG0_ENABLE_SWITCH_MASK                    0x0008
#define CHARGER_REG0_ENABLE_SWITCH_SHIFT                   3
#define CHARGER_REG0_PU_CHRG_DTC_MASK                      0x0004
#define CHARGER_REG0_PU_CHRG_DTC_SHIFT                     2
#define CHARGER_REG0_TESTMON_CHRGDTC_MASK                  0x0003
#define CHARGER_REG0_TESTMON_CHRGDTC_SHIFT                 0


//Bitfield Constants Register usb2_otg_reg0
#define OTG_REG0_OTG_CONTROL_BY_PIN_MASK               0x0010
#define OTG_REG0_OTG_CONTROL_BY_PIN_SHIFT              4
#define OTG_REG0_PU_OTG_MASK                           0x0008
#define OTG_REG0_PU_OTG_SHIFT                          3
#define OTG_REG0_TESTMON_OTG_MASK                      0x0007
#define OTG_REG0_TESTMON_OTG_SHIFT                     0


//Bitfield Constants Register usb2_phy_mon0
#define PHY_MON0_PHY_MON_MASK                          0xFFFF
#define PHY_MON0_PHY_MON_SHIFT                         0


//Bitfield Constants Register usb2_reserve_reg0
#define RESERVE_REG0_PLLVDD12_MASK                         0x3000
#define RESERVE_REG0_PLLVDD12_SHIFT                        12

//Bitfield Constants Register usb2_icid_reg1
#define ICID_REG1_PHY_OTG_MASK                          0x0080
#define ICID_REG1_PHY_OTG_SHIFT                         7
#define ICID_REG1_PHY_CHG_DTC_MASK                      0x0040
#define ICID_REG1_PHY_CHG_DTC_SHIFT                     6
#define ICID_REG1_PHY_HSIC_MASK                         0x0020
#define ICID_REG1_PHY_HSIC_SHIFT                        5
#define ICID_REG1_PHY_ULPI_MASK                         0x0010
#define ICID_REG1_PHY_ULPI_SHIFT                        4
#define ICID_REG1_DIG_REGULATOR_MASK                    0x0008
#define ICID_REG1_DIG_REGULATOR_SHIFT                   3
#define ICID_REG1_PHY_MULTIPORT_MASK                    0x0001
#define ICID_REG1_PHY_MULTIPORT_SHIFT                   0










#endif
