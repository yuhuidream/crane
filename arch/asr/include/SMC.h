#ifndef __SMC_H__
#define __SMC_H__

typedef struct
{
    __IO uint32_t RESERVED0[0x0020/4];
    __IO uint32_t MSC[4];
    __IO uint32_t SXCNFG[4];
    __IO uint32_t RESERVED01;//0X040
    __IO uint32_t RESERVED1[(0x068-0x040)/4-1];

    __IO uint32_t MEMCLKCFG;//0X068
    __IO uint32_t RESERVED2[(0x090-0x068)/4-1];
    __IO uint32_t CSDFICFG[4];//0X90
    __IO uint32_t RDBUFCFGX[4];
    __IO uint32_t CLK_RET_DEL;
 union{
    __IO uint32_t WRFIFOCFG;
    __IO uint32_t ADV_RET_DEL;
};
   __IO uint32_t RESERVED3[2];
    __IO uint32_t CSADRMAP[4];//0xc0
    __IO uint32_t TCVCFGREG;//0xd0
    __IO uint32_t RESERVED4[3];
    __IO uint32_t WE_AP[4];//0xe0
    __IO uint32_t OE_AP[4];
    __IO uint32_t ADV_AP[4];
    __IO uint32_t WE_DPX[4];
    __IO uint32_t OE_DPX[4];
    __IO uint32_t TCVENX[4];


}SMC_TypeDef;


#define    SMC_BASE       0xD4283800


#define    SMC  (( SMC_TypeDef *)SMC_BASE )



//Bitfield Constants Register SMC_MSC[]
#define MSC[]_WE_GEN_MASK                           0xC0000000
#define MSC[]_WE_GEN_SHIFT                          30
#define MSC[]_WE_D_HO_MASK                          0x0E000000
#define MSC[]_WE_D_HO_SHIFT                         25
#define MSC[]_WE_D_SU_MASK                          0x01C00000
#define MSC[]_WE_D_SU_SHIFT                         22
#define MSC[]_WE_LEN_MASK                           0x003F0000
#define MSC[]_WE_LEN_SHIFT                          16
#define MSC[]_OE_GEN_MASK                           0x0000C000
#define MSC[]_OE_GEN_SHIFT                          14
#define MSC[]_OE_HO_MASK                            0x00003000
#define MSC[]_OE_HO_SHIFT                           12
#define MSC[]_OE_SU_MASK                            0x00000E00
#define MSC[]_OE_SU_SHIFT                           9
#define MSC[]_OE_D_HO_MASK                          0x000001C0
#define MSC[]_OE_D_HO_SHIFT                         6
#define MSC[]_OE_D_SU_MASK                          0x0000003F
#define MSC[]_OE_D_SU_SHIFT                         0




//Bitfield Constants Register SMC_SXCNFG[]
#define SXCNFG[]_SXWA_MASK                             0x00003F00
#define SXCNFG[]_SXWA_SHIFT                            8
#define SXCNFG[]_SXRA_MASK                             0x0000003F
#define SXCNFG[]_SXRA_SHIFT                            0



//Bitfield Constants Register SMC_ADV_DPx
#define ADV_DPX_ADV_VAL_MASK                          0x3FFFFFF0
#define ADV_DPX_ADV_VAL_SHIFT                         4
#define ADV_DPX_EN_ADV_CON_MASK                       0x00000001
#define ADV_DPX_EN_ADV_CON_SHIFT                      0


//Bitfield Constants Register SMC_MEMCLKCFG
#define MEMCLKCFG_CKEN_MASK                             0x00000003
#define MEMCLKCFG_CKEN_SHIFT                            0


//Bitfield Constants Register SMC_CSDFICFG[]
#define CSDFICFG[]_ALTS_MASK                             0xC0000000
#define CSDFICFG[]_ALTS_SHIFT                            30
#define CSDFICFG[]_ALTH_MASK                             0x30000000
#define CSDFICFG[]_ALTH_SHIFT                            28
#define CSDFICFG[]_ALW_MASK                              0x07000000
#define CSDFICFG[]_ALW_SHIFT                             24
#define CSDFICFG[]_RDY_SPEC4_MASK                        0x00800000
#define CSDFICFG[]_RDY_SPEC4_SHIFT                       23
#define CSDFICFG[]_RDY_SPEC3_MASK                        0x00400000
#define CSDFICFG[]_RDY_SPEC3_SHIFT                       22
#define CSDFICFG[]_RDY_SPEC2_1_MASK                      0x00100000
#define CSDFICFG[]_RDY_SPEC2_1_SHIFT                     20
#define CSDFICFG[]_RDY_SPEC0_MASK                        0x00080000
#define CSDFICFG[]_RDY_SPEC0_SHIFT                       19
#define CSDFICFG[]_ADDRBASE_MASK                         0x00030000
#define CSDFICFG[]_ADDRBASE_SHIFT                        16
#define CSDFICFG[]_WRSYNC_MASK                           0x00007000
#define CSDFICFG[]_WRSYNC_SHIFT                          12
#define CSDFICFG[]_RDSYNC_MASK                           0x00000700
#define CSDFICFG[]_RDSYNC_SHIFT                          8
#define CSDFICFG[]_LOWADD_MASK                           0x00000070
#define CSDFICFG[]_LOWADD_SHIFT                          4
#define CSDFICFG[]_RBW_MASK                              0x00000008
#define CSDFICFG[]_RBW_SHIFT                             3
#define CSDFICFG[]_ADDMODE_MASK                          0x00000007
#define CSDFICFG[]_ADDMODE_SHIFT                         0


//Bitfield Constants Register SMC_RDBUFCFGx
#define RDBUFCFGX_PHY_BUF_W_RDY_MASK                    0x00100000
#define RDBUFCFGX_PHY_BUF_W_RDY_SHIFT                   20
#define RDBUFCFGX_BUF_START_ADD_MASK                    0x00030000
#define RDBUFCFGX_BUF_START_ADD_SHIFT                   16
#define RDBUFCFGX_AUTO_CAL_CONT_MASK                    0x00001000
#define RDBUFCFGX_AUTO_CAL_CONT_SHIFT                   12
#define RDBUFCFGX_AUTO_CAL_SINGLE_MASK                  0x00000100
#define RDBUFCFGX_AUTO_CAL_SINGLE_SHIFT                 8
#define RDBUFCFGX_MATCH_SAMPLES_MASK                    0x00000070
#define RDBUFCFGX_MATCH_SAMPLES_SHIFT                   4
#define RDBUFCFGX_MAN_CAL_VAL_MASK                      0x0000000F
#define RDBUFCFGX_MAN_CAL_VAL_SHIFT                     0

//Bitfield Constants Register SMC_CLK_RET_DEL
#define CLK_RET_DEL_FLOP_SEL_MASK                         0x02000000
#define CLK_RET_DEL_FLOP_SEL_SHIFT                        25
#define CLK_RET_DEL_RDY_DLY_VAL_MASK                      0x00F00000
#define CLK_RET_DEL_RDY_DLY_VAL_SHIFT                     20
#define CLK_RET_DEL_DIN_DLY_VAL_MASK                      0x000F0000
#define CLK_RET_DEL_DIN_DLY_VAL_SHIFT                     16
#define CLK_RET_DEL_LLA_DLY_VAL_MASK                      0x0000F000
#define CLK_RET_DEL_LLA_DLY_VAL_SHIFT                     12
#define CLK_RET_DEL_ADV_DLY_VAL_MASK                      0x00000F00
#define CLK_RET_DEL_ADV_DLY_VAL_SHIFT                     8
#define CLK_RET_DEL_SCLK_DLY_VAL_MASK                     0x000000F0
#define CLK_RET_DEL_SCLK_DLY_VAL_SHIFT                    4
#define CLK_RET_DEL_FLOP_CLK_DLY_VAL_MASK                 0x0000000F
#define CLK_RET_DEL_FLOP_CLK_DLY_VAL_SHIFT                0


//Bitfield Constants Register SMC_WRFIFOCFG
#define WRFIFOCFG_USE_FIFO_W_RDY1_MASK                  0x00000010
#define WRFIFOCFG_USE_FIFO_W_RDY1_SHIFT                 4
#define WRFIFOCFG_USE_FIFO_W_RDY0_MASK                  0x00000001
#define WRFIFOCFG_USE_FIFO_W_RDY0_SHIFT                 0

//Bitfield Constants Register SMC_ADV_RET_DEL
#define ADV_RET_DEL_ADVN_SEL_MASK                         0x000000C0
#define ADV_RET_DEL_ADVN_SEL_SHIFT                        6
#define ADV_RET_DEL_ADVN_DEL_1_MASK                       0x00000038
#define ADV_RET_DEL_ADVN_DEL_1_SHIFT                      3
#define ADV_RET_DEL_ADVN_DEL_0_MASK                       0x00000007
#define ADV_RET_DEL_ADVN_DEL_0_SHIFT                      0

//Bitfield Constants Register SMC_CSADRMAP[]
#define CSADRMAP[]_MEMUSED_MASK                          0x10000000
#define CSADRMAP[]_MEMUSED_SHIFT                         28
#define CSADRMAP[]_STADD_MASK                            0x003F0000
#define CSADRMAP[]_STADD_SHIFT                           16
#define CSADRMAP[]_MEMSIZE_MASK                          0x00000F00
#define CSADRMAP[]_MEMSIZE_SHIFT                         8
#define CSADRMAP[]_WR_START_MASK                         0x00000002
#define CSADRMAP[]_WR_START_SHIFT                        1
#define CSADRMAP[]_RD_START_MASK                         0x00000001
#define CSADRMAP[]_RD_START_SHIFT                        0

//Bitfield Constants Register SMC_TCVCFGREG
#define TCVCFGREG_TCVCS3EN                      BIT_3
#define TCVCFGREG_TCVCS2EN                         BIT_2
#define TCVCFGREG_TCVCS1EN_MASK                         0x00000002
#define TCVCFGREG_TCVCS1EN_SHIFT                        1
#define TCVCFGREG_TCVCS0EN_MASK                         0x00000001
#define TCVCFGREG_TCVCS0EN_SHIFT                        0




//Bitfield Constants Register SMC_WE_Ap[]
#define WE_AP[]_WE_AP_VAL_MASK                        0x00000FFF
#define WE_AP[]_WE_AP_VAL_SHIFT                       0




//Bitfield Constants Register SMC_OE_Ap[]
#define OE_AP[]_OE_AP_VAL_MASK                        0x00000FFF
#define OE_AP[]_OE_AP_VAL_SHIFT                       0



//Bitfield Constants Register SMC_ADV_Ap[]
#define ADV_AP[]_ADV_AP_VAL_MASK                       0x00000FFF
#define ADV_AP[]_ADV_AP_VAL_SHIFT                      0


//Bitfield Constants Register SMC_WE_DPx
#define WE_DPX_WE_DP_VAL_MASK                        0x0FFFFFF0
#define WE_DPX_WE_DP_VAL_SHIFT                       4
#define WE_DPX_EN_WE_CON_MASK                        0x00000001
#define WE_DPX_EN_WE_CON_SHIFT                       0



//Bitfield Constants Register SMC_OE_DPx
#define OE_DPX_OE_DP_VAL_MASK                        0x0FFFFFF0
#define OE_DPX_OE_DP_VAL_SHIFT                       4
#define OE_DPX_EN_OE_CON_MASK                        0x00000001
#define OE_DPX_EN_OE_CON_SHIFT                       0


//Bitfield Constants Register SMC_TCVENx
#define TCVENX_TCVEN_VAL_MASK                        0x3FFFFFF0
#define TCVENX_TCVEN_VAL_SHIFT                       4
#define TCVENX_EN_TCVEN_CON_MASK                     0x00000001
#define TCVENX_EN_TCVEN_CON_SHIFT                    0







#endif
