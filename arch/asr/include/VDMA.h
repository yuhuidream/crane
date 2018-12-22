#ifndef __VDMA_H__
#define __VDMA_H__

typedef struct
{

       __IO uint32_t RESERVED0[(0x300)/4];

       __IO uint32_t ARBR_CTRL;
       __IO uint32_t IRQR;
       __IO uint32_t IRQM;
       __IO uint32_t IRQS;
       __IO uint32_t MDMA_ARBR_CTRL;//0X310
       __IO uint32_t RESERVED01[0x10/4-1];
       __IO uint32_t DC_SADDR0;//0X320


       __IO uint32_t DC_SZ0;
       __IO uint32_t CTRL0;
       __IO uint32_t SRC_SZ0;
       __IO uint32_t SA0;

       __IO uint32_t DA0;
       __IO uint32_t SZ0;
       __IO uint32_t PITCH0;
       __IO uint32_t ROT_CTRL0;
       __IO uint32_t RAM_CTRL0_0;
       __IO uint32_t RAM_CTRL1_0;//0X348
       __IO uint32_t RESERVED1[(0x3A0-0x348)/4-1];

       __IO uint32_t DC_SADDR1;//0X3A0

       __IO uint32_t DC_SZ1;
       __IO uint32_t CTRL1;
       __IO uint32_t SRC_SZ1;

       __IO uint32_t SA1;
       __IO uint32_t DA1;
       __IO uint32_t SZ1;
       __IO uint32_t PITCH1;
       __IO uint32_t ROT_CTRL1;
       __IO uint32_t RAM_CTRL0_1;
       __IO uint32_t RAM_CTRL1_1;






}VDMA_TypeDef;



#define    VDMA_BASE      0xD420B000
#define    VDMA  (( VDMA_TypeDef *) VDMA_BASE )


//Bitfield Constants Register VDMA_ARBR_CTRL
#define ARBR_CTRL_LCDIF_SYNC_DIS_MASK                   0x00040000
#define ARBR_CTRL_LCDIF_SYNC_DIS_SHIFT                  18
#define ARBR_CTRL_POSTED_WR_MASK                        0x00020000
#define ARBR_CTRL_POSTED_WR_SHIFT                       17
#define ARBR_CTRL_FORCE_GNT_MASK                        0x00006000
#define ARBR_CTRL_FORCE_GNT_SHIFT                       13

#define ARBR_CTRL_REQ_ACK_SEL_MASK                      0x00000100
#define ARBR_CTRL_REQ_ACK_SEL_SHIFT                     8
#define ARBR_CTRL_NUM_REQ_MASK                          0x000000F0
#define ARBR_CTRL_NUM_REQ_SHIFT                         4
#define ARBR_CTRL_PG_SZ_MASK                            0x0000000C
#define ARBR_CTRL_PG_SZ_SHIFT                           2
#define ARBR_CTRL_ARB_SCH_MASK                          0x00000003
#define ARBR_CTRL_ARB_SCH_SHIFT                         0




//Bitfield Constants Register VDMA_IRQR
#define IRQR_CH1_ROT_DONE_IRQR_MASK                0x00200000
#define IRQR_CH1_ROT_DONE_IRQR_SHIFT               21
#define IRQR_CH1_DMAW_FFFULL_IRQR_MASK             0x00100000
#define IRQR_CH1_DMAW_FFFULL_IRQR_SHIFT            20
#define IRQR_CH1_DMAR_FF2FULL_IRQR_MASK            0x00080000
#define IRQR_CH1_DMAR_FF2FULL_IRQR_SHIFT           19
#define IRQR_CH1_DMAR_FF1FULL_IRQR_MASK            0x00040000
#define IRQR_CH1_DMAR_FF1FULL_IRQR_SHIFT           18
#define IRQR_CH1_DMAR_FF0FULL_IRQR_MASK            0x00020000
#define IRQR_CH1_DMAR_FF0FULL_IRQR_SHIFT           17
#define IRQR_CH1_DC_IRQR_MASK                      0x00010000
#define IRQR_CH1_DC_IRQR_SHIFT                     16
#define IRQR_CH0_ROT_DONE_IRQR_MASK                0x00000020
#define IRQR_CH0_ROT_DONE_IRQR_SHIFT               5
#define IRQR_CH0_DMAW_FFFULL_IRQR_MASK             0x00000010
#define IRQR_CH0_DMAW_FFFULL_IRQR_SHIFT            4
#define IRQR_CH0_DMAR_FF2FULL_IRQR_MASK            0x00000008
#define IRQR_CH0_DMAR_FF2FULL_IRQR_SHIFT           3
#define IRQR_CH0_DMAR_FF1FULL_IRQR_MASK            0x00000004
#define IRQR_CH0_DMAR_FF1FULL_IRQR_SHIFT           2
#define IRQR_CH0_DMAR_FF0FULL_IRQR_MASK            0x00000002
#define IRQR_CH0_DMAR_FF0FULL_IRQR_SHIFT           1
#define IRQR_CH0_DC_IRQR_MASK                      0x00000001
#define IRQR_CH0_DC_IRQR_SHIFT                     0





//Bitfield Constants Register VDMA_IRQM
#define IRQM_CH1_ROT_DONE_IRQM_MASK                0x00200000
#define IRQM_CH1_ROT_DONE_IRQM_SHIFT               21
#define IRQM_CH1_DMAW_FFFULL_IRQM_MASK             0x00100000
#define IRQM_CH1_DMAW_FFFULL_IRQM_SHIFT            20
#define IRQM_CH1_DMAR_FF2FULL_IRQM_MASK            0x00080000
#define IRQM_CH1_DMAR_FF2FULL_IRQM_SHIFT           19
#define IRQM_CH1_DMAR_FF1FULL_IRQM_MASK            0x00040000
#define IRQM_CH1_DMAR_FF1FULL_IRQM_SHIFT           18
#define IRQM_CH1_DMAR_FF0FULL_IRQM_MASK            0x00020000
#define IRQM_CH1_DMAR_FF0FULL_IRQM_SHIFT           17
#define IRQM_CH1_DC_IRQM_MASK                      0x00010000
#define IRQM_CH1_DC_IRQM_SHIFT                     16
#define IRQM_CH0_ROT_DONE_IRQM_MASK                0x00000020
#define IRQM_CH0_ROT_DONE_IRQM_SHIFT               5
#define IRQM_CH0_DMAW_FFFULL_IRQM_MASK             0x00000010
#define IRQM_CH0_DMAW_FFFULL_IRQM_SHIFT            4
#define IRQM_CH0_DMAR_FF2FULL_IRQM_MASK            0x00000008
#define IRQM_CH0_DMAR_FF2FULL_IRQM_SHIFT           3
#define IRQM_CH0_DMAR_FF1FULL_IRQM_MASK            0x00000004
#define IRQM_CH0_DMAR_FF1FULL_IRQM_SHIFT           2
#define IRQM_CH0_DMAR_FF0FULL_IRQM_MASK            0x00000002
#define IRQM_CH0_DMAR_FF0FULL_IRQM_SHIFT           1
#define IRQM_CH0_DC_IRQM_MASK                      0x00000001
#define IRQM_CH0_DC_IRQM_SHIFT                     0




//Bitfield Constants Register VDMA_IRQS
#define IRQS_CH1_ROT_DONE_IRQS_MASK                0x00200000
#define IRQS_CH1_ROT_DONE_IRQS_SHIFT               21
#define IRQS_CH1_DMAW_FFFULL_IRQS_MASK             0x00100000
#define IRQS_CH1_DMAW_FFFULL_IRQS_SHIFT            20
#define IRQS_CH1_DMAR_FF2FULL_IRQS_MASK            0x00080000
#define IRQS_CH1_DMAR_FF2FULL_IRQS_SHIFT           19
#define IRQS_CH1_DMAR_FF1FULL_IRQS_MASK            0x00040000
#define IRQS_CH1_DMAR_FF1FULL_IRQS_SHIFT           18
#define IRQS_CH1_DMAR_FF0FULL_IRQS_MASK            0x00020000
#define IRQS_CH1_DMAR_FF0FULL_IRQS_SHIFT           17
#define IRQS_CH1_DC_IRQS_MASK                      0x00010000
#define IRQS_CH1_DC_IRQS_SHIFT                     16
#define IRQS_CH0_ROT_DONE_IRQS_MASK                0x00000020
#define IRQS_CH0_ROT_DONE_IRQS_SHIFT               5
#define IRQS_CH0_DMAW_FFFULL_IRQS_MASK             0x00000010
#define IRQS_CH0_DMAW_FFFULL_IRQS_SHIFT            4
#define IRQS_CH0_DMAR_FF2FULL_IRQS_MASK            0x00000008
#define IRQS_CH0_DMAR_FF2FULL_IRQS_SHIFT           3
#define IRQS_CH0_DMAR_FF1FULL_IRQS_MASK            0x00000004
#define IRQS_CH0_DMAR_FF1FULL_IRQS_SHIFT           2
#define IRQS_CH0_DMAR_FF0FULL_IRQS_MASK            0x00000002
#define IRQS_CH0_DMAR_FF0FULL_IRQS_SHIFT           1
#define IRQS_CH0_DC_IRQS_MASK                      0x00000001
#define IRQS_CH0_DC_IRQS_SHIFT                     0







//Bitfield Constants Register VDMA_MDMA_ARBR_CTRL

#define MDMA_ARBR_CTRL_FORCE_GNT_MASK                        0x00006000
#define MDMA_ARBR_CTRL_FORCE_GNT_SHIFT                       13
#define MDMA_ARBR_CTRL_NUM_REQ_MASK                          0x000000F0
#define MDMA_ARBR_CTRL_NUM_REQ_SHIFT                         4
#define MDMA_ARBR_CTRL_PG_SZ_MASK                            0x0000000C
#define MDMA_ARBR_CTRL_PG_SZ_SHIFT                           2
#define MDMA_ARBR_CTRL_ARB_SCH_MASK                          0x00000003
#define MDMA_ARBR_CTRL_ARB_SCH_SHIFT                         0






//Bitfield Constants Register VDMA_DC_SADDR0
#define DC_SADDR0_DC_SADDR_MSB_MASK                     0xFFFFFFF8
#define DC_SADDR0_DC_SADDR_MSB_SHIFT                    3
#define DC_SADDR0_DC_SADDR_LSB_MASK                     0x00000007
#define DC_SADDR0_DC_SADDR_LSB_SHIFT                    0






//Bitfield Constants Register VDMA_DC_SZ0
#define DC_SZ0_DC_SZ_MSB_MASK                        0x0000FFF8
#define DC_SZ0_DC_SZ_MSB_SHIFT                       3
#define DC_SZ0_DC_SZ_LSB_MASK                        0x00000007
#define DC_SZ0_DC_SZ_LSB_SHIFT                       0







//Bitfield Constants Register VDMA_CTRL0
#define CTRL0_ROT_MODE_MASK                         0x00020000
#define CTRL0_ROT_MODE_SHIFT                        17
#define CTRL0_DST_LOC_MASK                          0x00010000
#define CTRL0_DST_LOC_SHIFT                         16
#define CTRL0_SQU_LN_SZ_MASK                        0x0000FF00
#define CTRL0_SQU_LN_SZ_SHIFT                       8
#define CTRL0_WBS_MASK                              0x000000C0
#define CTRL0_WBS_SHIFT                             6
#define CTRL0_RBS_MASK                              0x00000030
#define CTRL0_RBS_SHIFT                             4
#define CTRL0_ROT_ENA_MASK                          0x00000004
#define CTRL0_ROT_ENA_SHIFT                         2
#define CTRL0_DC_ENA_MASK                           0x00000002
#define CTRL0_DC_ENA_SHIFT                          1
#define CTRL0_CH_ENA_MASK                           0x00000001
#define CTRL0_CH_ENA_SHIFT                          0



//Bitfield Constants Register VDMA_SRC_SZ0
#define SRC_SZ0_SRC_SZ_MSB_MASK                       0x00FFFFFE
#define SRC_SZ0_SRC_SZ_MSB_SHIFT                      1
#define SRC_SZ0_SRC_SZ_LSB_MASK                       0x00000001
#define SRC_SZ0_SRC_SZ_LSB_SHIFT                      0



//Bitfield Constants Register VDMA_SA0
#define SA0_SA_MSB_MASK                           0xFFFFFFFE
#define SA0_SA_MSB_SHIFT                          1
#define SA0_SA_LSB_MASK                           0x00000001
#define SA0_SA_LSB_SHIFT                          0




//Bitfield Constants Register VDMA_DA0
#define DA0_DA_MSB_MASK                           0xFFFFFFFE
#define DA0_DA_MSB_SHIFT                          1
#define DA0_DA_LSB_MASK                           0x00000001
#define DA0_DA_LSB_SHIFT                          0




//Bitfield Constants Register VDMA_SZ0
#define SZ0_VSIZE_MASK                            0xFFFF0000
#define SZ0_VSIZE_SHIFT                           16
#define SZ0_HSIZE_MASK                            0x0000FFFF
#define SZ0_HSIZE_SHIFT                           0



//Bitfield Constants Register VDMA_PITCH0
#define PITCH0_DST_PITCH_MASK                        0xFFFF0000
#define PITCH0_DST_PITCH_SHIFT                       16
#define PITCH0_SRC_PITCH_MASK                        0x0000FFFF
#define PITCH0_SRC_PITCH_SHIFT                       0

//Bitfield Constants Register VDMA_ROT_CTRL0
#define ROT_CTRL0_YCBCR_422_ROT_OUT_FMT_MASK            0x00600000
#define ROT_CTRL0_YCBCR_422_ROT_OUT_FMT_SHIFT           21
#define ROT_CTRL0_ANGLE_MASK                            0x00003800
#define ROT_CTRL0_ANGLE_SHIFT                           11
#define ROT_CTRL0_YCBCR_END_MASK                        0x00000600
#define ROT_CTRL0_YCBCR_END_SHIFT                       9
#define ROT_CTRL0_YCBCR_FMT_MASK                        0x00000180
#define ROT_CTRL0_YCBCR_FMT_SHIFT                       7
#define ROT_CTRL0_ROT_MODE_MASK                         0x00000060
#define ROT_CTRL0_ROT_MODE_SHIFT                        5
#define ROT_CTRL0_BPP_MASK                              0x0000001C
#define ROT_CTRL0_BPP_SHIFT                             2
#define ROT_CTRL0_EN_MASK                               0x00000001
#define ROT_CTRL0_EN_SHIFT                              0


//Bitfield Constants Register VDMA_RAM_CTRL0_0
#define RAM_CTRL0_0_DMAR_FF1_WTC_MASK                     0x0000C000
#define RAM_CTRL0_0_DMAR_FF1_WTC_SHIFT                    14
#define RAM_CTRL0_0_DMAR_FF1_RTC_MASK                     0x00003000
#define RAM_CTRL0_0_DMAR_FF1_RTC_SHIFT                    12
#define RAM_CTRL0_0_DC_FF_WTC_MASK                        0x00000C00
#define RAM_CTRL0_0_DC_FF_WTC_SHIFT                       10
#define RAM_CTRL0_0_DC_FF_RTC_MASK                        0x00000300
#define RAM_CTRL0_0_DC_FF_RTC_SHIFT                       8
#define RAM_CTRL0_0_FF_PDWN_MASK                          0x00000004
#define RAM_CTRL0_0_FF_PDWN_SHIFT                         2
#define RAM_CTRL0_0_DMAR_FF1_PDWN_MASK                    0x00000002
#define RAM_CTRL0_0_DMAR_FF1_PDWN_SHIFT                   1
#define RAM_CTRL0_0_DC_FF_PDWN_MASK                       0x00000001
#define RAM_CTRL0_0_DC_FF_PDWN_SHIFT                      0



//Bitfield Constants Register VDMA_RAM_CTRL1_0
#define RAM_CTRL1_0_SRAM7_RTC_MASK                        0xC0000000
#define RAM_CTRL1_0_SRAM7_RTC_SHIFT                       30
#define RAM_CTRL1_0_SRAM7_WTC_MASK                        0x30000000
#define RAM_CTRL1_0_SRAM7_WTC_SHIFT                       28
#define RAM_CTRL1_0_SRAM6_RTC_MASK                        0x0C000000
#define RAM_CTRL1_0_SRAM6_RTC_SHIFT                       26
#define RAM_CTRL1_0_SRAM6_WTC_MASK                        0x03000000
#define RAM_CTRL1_0_SRAM6_WTC_SHIFT                       24
#define RAM_CTRL1_0_SRAM5_RTC_MASK                        0x00C00000
#define RAM_CTRL1_0_SRAM5_RTC_SHIFT                       22
#define RAM_CTRL1_0_SRAM5_WTC_MASK                        0x00300000
#define RAM_CTRL1_0_SRAM5_WTC_SHIFT                       20
#define RAM_CTRL1_0_SRAM4_RTC_MASK                        0x000C0000
#define RAM_CTRL1_0_SRAM4_RTC_SHIFT                       18
#define RAM_CTRL1_0_SRAM4_WTC_MASK                        0x00030000
#define RAM_CTRL1_0_SRAM4_WTC_SHIFT                       16
#define RAM_CTRL1_0_SRAM3_RTC_MASK                        0x0000C000
#define RAM_CTRL1_0_SRAM3_RTC_SHIFT                       14
#define RAM_CTRL1_0_SRAM3_WTC_MASK                        0x00003000
#define RAM_CTRL1_0_SRAM3_WTC_SHIFT                       12
#define RAM_CTRL1_0_SRAM2_RTC_MASK                        0x00000C00
#define RAM_CTRL1_0_SRAM2_RTC_SHIFT                       10
#define RAM_CTRL1_0_SRAM2_WTC_MASK                        0x00000300
#define RAM_CTRL1_0_SRAM2_WTC_SHIFT                       8
#define RAM_CTRL1_0_SRAM1_RTC_MASK                        0x000000C0
#define RAM_CTRL1_0_SRAM1_RTC_SHIFT                       6
#define RAM_CTRL1_0_SRAM1_WTC_MASK                        0x00000030
#define RAM_CTRL1_0_SRAM1_WTC_SHIFT                       4
#define RAM_CTRL1_0_SRAM0_RTC_MASK                        0x0000000C
#define RAM_CTRL1_0_SRAM0_RTC_SHIFT                       2
#define RAM_CTRL1_0_SRAM0_WTC_MASK                        0x00000003
#define RAM_CTRL1_0_SRAM0_WTC_SHIFT                       0




#endif
