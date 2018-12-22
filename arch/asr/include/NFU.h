#ifndef __NFU_H__
#define __NFU_H__

typedef struct
{

    __IO uint32_t NDCR;
    __IO uint32_t NDTR0CS0;
    __IO uint32_t RESERVED0;
    __IO uint32_t NDTR1CS0;
    __IO uint32_t RESERVED1;	//0x10
    __IO uint32_t NDSR;	
    __IO uint32_t NDPCR;
    __IO uint32_t NDBBR0;
    __IO uint32_t NDBBR1;	//0x20
    __IO uint32_t NDREDEL;
    __IO uint32_t NDECCCTRL;
    __IO uint32_t NDBZCNT;	
    __IO uint32_t NDMUTEX;	//0x30
    __IO uint32_t NDCMDMAT0;
    __IO uint32_t NDCMDMAT1;
    __IO uint32_t NDCMDMAT2;
    __IO uint32_t NDDB;		//0x40
    __IO uint32_t RESERVED3;
    __IO uint32_t NDCB0;
    __IO uint32_t NDCB1;
    __IO uint32_t NDCB2;	//0x50
    __IO uint32_t NDCB3;
    __IO uint32_t NSACR;
    __IO uint32_t NDARBCR;	
    __IO uint32_t NDPTXCSX[7];//0X60-0x7c
  
    __IO uint32_t NDSMDMADST;//0X80
    __IO uint32_t NDSMDMASRC;
    __IO uint32_t NDSMDMACTL;
    __IO uint32_t NDSMNXT;
    __IO uint32_t NDSMINTR;
    __IO uint32_t NDSMINTRMSK;





}NFU_TypeDef;



#define    NFU_BASE      0xD4283000


#define    NFU  (( NFU_TypeDef *) NFU_BASE )




//Bitfield Constants Register NFC_NDCR
#define NDCR_SPARE_EN_MASK                         0x80000000
#define NDCR_SPARE_EN_SHIFT                        31
#define NDCR_ECC_EN_MASK                           0x40000000
#define NDCR_ECC_EN_SHIFT                          30
#define NDCR_DMA_EN_MASK                           0x20000000
#define NDCR_DMA_EN_SHIFT                          29
#define NDCR_ND_RUN_MASK                           0x10000000
#define NDCR_ND_RUN_SHIFT                          28
#define NDCR_DWIDTH_C_MASK                         0x08000000
#define NDCR_DWIDTH_C_SHIFT                        27
#define NDCR_DWIDTH_M_MASK                         0x04000000
#define NDCR_DWIDTH_M_SHIFT                        26
#define NDCR_PAGE_SZ_MASK                          0x03000000
#define NDCR_PAGE_SZ_SHIFT                         24
#define NDCR_SEQ_DIS_MASK                          0x00800000
#define NDCR_SEQ_DIS_SHIFT                         23
#define NDCR_ND_STOP_MASK                          0x00400000
#define NDCR_ND_STOP_SHIFT                         22
#define NDCR_FORCE_CSX_MASK                        0x00200000
#define NDCR_FORCE_CSX_SHIFT                       21
#define NDCR_CLR_PG_CNT_MASK                       0x00100000
#define NDCR_CLR_PG_CNT_SHIFT                      20
#define NDCR_STOP_ON_UNCOR_MASK                    0x00080000
#define NDCR_STOP_ON_UNCOR_SHIFT                   19
#define NDCR_RD_ID_CNT_MASK                        0x00070000
#define NDCR_RD_ID_CNT_SHIFT                       16
#define NDCR_RA_START_MASK                         0x00008000
#define NDCR_RA_START_SHIFT                        15
#define NDCR_PG_PER_BLK_MASK                       0x00006000
#define NDCR_PG_PER_BLK_SHIFT                      13
#define NDCR_ND_ARB_EN_MASK                        0x00001000
#define NDCR_ND_ARB_EN_SHIFT                       12
#define NDCR_RDYM_MASK                             0x00000800
#define NDCR_RDYM_SHIFT                            11
#define NDCR_CS0_PAGEDM_MASK                       0x00000400
#define NDCR_CS0_PAGEDM_SHIFT                      10
#define NDCR_CS1_PAGEDM_MASK                       0x00000200
#define NDCR_CS1_PAGEDM_SHIFT                      9
#define NDCR_CS0_CMDDM_MASK                        0x00000100
#define NDCR_CS0_CMDDM_SHIFT                       8
#define NDCR_CS1_CMDDM_MASK                        0x00000080
#define NDCR_CS1_CMDDM_SHIFT                       7
#define NDCR_CS0_BBDM_MASK                         0x00000040
#define NDCR_CS0_BBDM_SHIFT                        6
#define NDCR_CS1_BBDM_MASK                         0x00000020
#define NDCR_CS1_BBDM_SHIFT                        5
#define NDCR_UNCERRM_MASK                          0x00000010
#define NDCR_UNCERRM_SHIFT                         4
#define NDCR_CORERRM_MASK                          0x00000008
#define NDCR_CORERRM_SHIFT                         3
#define NDCR_WRDREQM_MASK                          0x00000004
#define NDCR_WRDREQM_SHIFT                         2
#define NDCR_RDDREQM_MASK                          0x00000002
#define NDCR_RDDREQM_SHIFT                         1
#define NDCR_WRCMDREQM_MASK                        0x00000001
#define NDCR_WRCMDREQM_SHIFT                       0






//Bitfield Constants Register NFC_NDTR0CS0
#define NDTR0CS0_SELCNTR_MASK                          0x04000000
#define NDTR0CS0_SELCNTR_SHIFT                         26
#define NDTR0CS0_RD_CNT_DEL_MASK                       0x03C00000
#define NDTR0CS0_RD_CNT_DEL_SHIFT                      22
#define NDTR0CS0_TCH_MASK                              0x00380000
#define NDTR0CS0_TCH_SHIFT                             19
#define NDTR0CS0_TCS_MASK                              0x00070000
#define NDTR0CS0_TCS_SHIFT                             16
#define NDTR0CS0_TWH_MASK                              0x00003800
#define NDTR0CS0_TWH_SHIFT                             11
#define NDTR0CS0_TWP_MASK                              0x00000700
#define NDTR0CS0_TWP_SHIFT                             8
#define NDTR0CS0_ETRP_MASK                             0x00000040
#define NDTR0CS0_ETRP_SHIFT                            6
#define NDTR0CS0_TRH_MASK                              0x00000038
#define NDTR0CS0_TRH_SHIFT                             3
#define NDTR0CS0_TRP_MASK                              0x00000007
#define NDTR0CS0_TRP_SHIFT                             0




//Bitfield Constants Register NFC_NDTR1CS0
#define NDTR1CS0_TR_MASK                               0xFFFF0000
#define NDTR1CS0_TR_SHIFT                              16
#define NDTR1CS0_WAIT_MODE_MASK                        0x00008000
#define NDTR1CS0_WAIT_MODE_SHIFT                       15
#define NDTR1CS0_tr_prescale                      BIT_14
#define NDTR1CS0_trhw_x_MSK                     SHIFT8(0x3)
#define NDTR1CS0_trhw_x_BASE
#define NDTR1CS0_TWHR_MASK                             0x000000F0
#define NDTR1CS0_TWHR_SHIFT                            4
#define NDTR1CS0_TAR_MASK                              0x0000000F
#define NDTR1CS0_TAR_SHIFT                             0


//Bitfield Constants Register NFC_NDSR
#define NDSR_ERR_CNT_MASK                          0x001F0000
#define NDSR_ERR_CNT_SHIFT                         16
#define NDSR_TRUSTVIO_MASK                         0x00008000
#define NDSR_TRUSTVIO_SHIFT                        15
#define NDSR_RDY1_MASK                             0x00001000
#define NDSR_RDY1_SHIFT                            12
#define NDSR_RDY0_MASK                             0x00000800
#define NDSR_RDY0_SHIFT                            11
#define NDSR_CS0_PAGED_MASK                        0x00000400
#define NDSR_CS0_PAGED_SHIFT                       10
#define NDSR_CS1_PAGED_MASK                        0x00000200
#define NDSR_CS1_PAGED_SHIFT                       9
#define NDSR_CS0_CMDD_MASK                         0x00000100
#define NDSR_CS0_CMDD_SHIFT                        8
#define NDSR_CS1_CMDD_MASK                         0x00000080
#define NDSR_CS1_CMDD_SHIFT                        7
#define NDSR_CS0_BBD_MASK                          0x00000040
#define NDSR_CS0_BBD_SHIFT                         6
#define NDSR_CS1_BBD_MASK                          0x00000020
#define NDSR_CS1_BBD_SHIFT                         5
#define NDSR_UNCERR_MASK                           0x00000010
#define NDSR_UNCERR_SHIFT                          4
#define NDSR_CORERR_MASK                           0x00000008
#define NDSR_CORERR_SHIFT                          3
#define NDSR_WRDREQ_MASK                           0x00000004
#define NDSR_WRDREQ_SHIFT                          2
#define NDSR_RDDREQ_MASK                           0x00000002
#define NDSR_RDDREQ_SHIFT                          1
#define NDSR_WRCMDREQ_MASK                         0x00000001
#define NDSR_WRCMDREQ_SHIFT                        0



//Bitfield Constants Register NFC_NDPCR
#define NDPCR_PG_CNT_1_MASK                         0x00FF0000
#define NDPCR_PG_CNT_1_SHIFT                        16
#define NDPCR_PG_CNT_0_MASK                         0x000000FF
#define NDPCR_PG_CNT_0_SHIFT                        0



//Bitfield Constants Register NFC_NDREDEL
#define NDREDEL_ND_DIN_SEL_MASK                       0x02000000
#define NDREDEL_ND_DIN_SEL_SHIFT                      25
#define NDREDEL_ND_DATA_DLY_MASK                      0x00000300
#define NDREDEL_ND_DATA_DLY_SHIFT                     8
#define NDREDEL_ND_RECLK_DLY_MASK                     0x000000F0
#define NDREDEL_ND_RECLK_DLY_SHIFT                    4
#define NDREDEL_ND_RE_DLY_MASK                        0x0000000F
#define NDREDEL_ND_RE_DLY_SHIFT                       0




//Bitfield Constants Register NFC_NDECCCTRL
#define NDECCCTRL_ECC_SPARE_MASK                        0x00007F80
#define NDECCCTRL_ECC_SPARE_SHIFT                       7
#define NDECCCTRL_ECC_THRESH_MASK                       0x0000007E
#define NDECCCTRL_ECC_THRESH_SHIFT                      1
#define NDECCCTRL_BCH_EN_MASK                           0x00000001
#define NDECCCTRL_BCH_EN_SHIFT                          0




//Bitfield Constants Register NFC_NDBZCNT
#define NDBZCNT_ND_RDYCNT1_MASK                       SHIFT6(0x3ffffff)
#define NDBZCNT_ND_RDYCNT1_SHIFT                      6
#define NDBZCNT_ND_RDYCNT0_MASK                       0x0000FFFF
#define NDBZCNT_ND_RDYCNT0_SHIFT                      0





//Bitfield Constants Register NFC_NDCMDMAT
#define NDCMDMAT_VALIDCNT_MASK                         0xC0000000
#define NDCMDMAT_VALIDCNT_SHIFT                        30
#define NDCMDMAT_NAKEDDIS1_MASK                        0x20000000
#define NDCMDMAT_NAKEDDIS1_SHIFT                       29
#define NDCMDMAT_ROWADD1_MASK                          0x10000000
#define NDCMDMAT_ROWADD1_SHIFT                         28
#define NDCMDMAT_CMD1_MASK                             0x0FF00000
#define NDCMDMAT_CMD1_SHIFT                            20
#define NDCMDMAT_NAKEDDIS0_MASK                        0x00080000
#define NDCMDMAT_NAKEDDIS0_SHIFT                       19
#define NDCMDMAT_ROWADD0_MASK                          0x00040000
#define NDCMDMAT_ROWADD0_SHIFT                         18
#define NDCMDMAT_CMD0_MASK                             0x0003FC00
#define NDCMDMAT_CMD0_SHIFT                            10
#define NDCMDMAT_NAKEDDIS_MASK                         0x00000200
#define NDCMDMAT_NAKEDDIS_SHIFT                        9
#define NDCMDMAT_ROWADD_MASK                           0x00000100
#define NDCMDMAT_ROWADD_SHIFT                          8
#define NDCMDMAT_CMD_MASK                              0x000000FF
#define NDCMDMAT_CMD_SHIFT                             0





//Bitfield Constants Register NFC_NDCB0
#define NDCB0_CMD_XTYPE_MASK                        0xE0000000
#define NDCB0_CMD_XTYPE_SHIFT                       29
#define NDCB0_LEN_OVRD_MASK                         0x10000000
#define NDCB0_LEN_OVRD_SHIFT                        28
#define NDCB0_RDY_BYP_MASK                          0x08000000
#define NDCB0_RDY_BYP_SHIFT                         27
#define NDCB0_ST_ROW_EN_MASK                        0x04000000
#define NDCB0_ST_ROW_EN_SHIFT                       26
#define NDCB0_AUTO_RS_MASK                          0x02000000
#define NDCB0_AUTO_RS_SHIFT                         25
#define NDCB0_CSEL_MASK                             0x01000000
#define NDCB0_CSEL_SHIFT                            24
#define NDCB0_CMD_TYPE_MASK                         0x00E00000
#define NDCB0_CMD_TYPE_SHIFT                        21
#define NDCB0_CMD_TYPE(x)		(((x) << 21) & NDCB0_CMD_TYPE_MASK)
#define NDCB0_NC_MASK                               0x00100000
#define NDCB0_NC_SHIFT                              20
#define NDCB0_DBC_MASK                              0x00080000
#define NDCB0_DBC_SHIFT                             19
#define NDCB0_ADDR_CYC_MASK                         0x00070000
#define NDCB0_ADDR_CYC_SHIFT                        16
#define NDCB0_ADDR_CYC(x)	(((x) << 16) & NDCB0_ADDR_CYC_MASK)
#define NDCB0_CMD2_MASK                             0x0000FF00
#define NDCB0_CMD2_SHIFT                            8
#define NDCB0_CMD1_MASK                             0x000000FF
#define NDCB0_CMD1_SHIFT                            0



//Bitfield Constants Register NFC_NDCB1
#define NDCB1_ADDR4_MASK                            0xFF000000
#define NDCB1_ADDR4_SHIFT                           24
#define NDCB1_ADDR3_MASK                            0x00FF0000
#define NDCB1_ADDR3_SHIFT                           16
#define NDCB1_ADDR2_MASK                            0x0000FF00
#define NDCB1_ADDR2_SHIFT                           8
#define NDCB1_ADDR1_MASK                            0x000000FF
#define NDCB1_ADDR1_SHIFT                           0






//Bitfield Constants Register NFC_NDCB2
#define NDCB2_ST_MASK_MASK                          0xFF000000
#define NDCB2_ST_MASK_SHIFT                         24
#define NDCB2_ST_CMD_MASK                           0x00FF0000
#define NDCB2_ST_CMD_SHIFT                          16
#define NDCB2_PAGE_COUNT_MASK                       0x0000FF00
#define NDCB2_PAGE_COUNT_SHIFT                      8
#define NDCB2_ADDR5_MASK                            0x000000FF
#define NDCB2_ADDR5_SHIFT                           0




//Bitfield Constants Register NFC_NDCB3
#define NDCB3_ADDR7_MASK                            0xFF000000
#define NDCB3_ADDR7_SHIFT                           24
#define NDCB3_ADDR6_MASK                            0x00FF0000
#define NDCB3_ADDR6_SHIFT                           16
#define NDCB3_NDLENCNT_MASK                         0x0000FFFF
#define NDCB3_NDLENCNT_SHIFT                        0


//SRAM Attributes Control Register
#define NSACR_PWDN                           BIT_4
#define NSACR_WTC_MSK                        SHIFT2(0x3)
#define NSACR_WTC_BASE                        2
#define NSACR_RTC_MSK                           SHIFT0(0x3)
#define NSACR_RTC_BASE                          0



//Bitfield Constants Register NFC_NDPTXCSX
#define NDPTXCSX_VALID_MASK                            0x80000000
#define NDPTXCSX_VALID_SHIFT                           31
#define NDPTXCSX_LOCK_MASK                             0x40000000
#define NDPTXCSX_LOCK_SHIFT                            30
#define NDPTXCSX_TRUSTED_MASK                          0x20000000
#define NDPTXCSX_TRUSTED_SHIFT                         29
#define NDPTXCSX_BLOCKADD_MASK                         0x00FFFFFF
#define NDPTXCSX_BLOCKADD_SHIFT                        0




//Bitfield Constants Register NFC_NDSMDMACTL
#define NDSMDMACTL_OP_MASK                               0x00040000
#define NDSMDMACTL_OP_SHIFT                              18
#define NDSMDMACTL_FORCE_MASK                            0x00002000
#define NDSMDMACTL_FORCE_SHIFT                           13
#define NDSMDMACTL_START_MASK                            0x00001000
#define NDSMDMACTL_START_SHIFT                           12
#define NDSMDMACTL_NDCMD_MASK                            0x00000200
#define NDSMDMACTL_NDCMD_SHIFT                           9
#define NDSMDMACTL_LEN_MASK                              0x000000FF
#define NDSMDMACTL_LEN_SHIFT                             0



#endif
