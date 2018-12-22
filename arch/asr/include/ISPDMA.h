#ifndef __ISPDMA_H__
#define __ISPDMA_H__


typedef struct
{
    __IO uint32_t FBTXN_SDCA;
    __IO uint32_t FBTXN_DCSZ;
    __IO uint32_t FBTXN_CTRL;
    __IO uint32_t FBTXN_DSTSZ;
    __IO uint32_t FBTXN_DSTADDR;
    __IO uint32_t RESERVED0[0X14/4-1];
    __IO uint32_t FBTXN_TMR;
    __IO uint32_t FBTXN_STAT;
 
    __IO uint32_t RESERVED1;
    __IO uint32_t FBTXN_RAMCTRL;
 
    __IO uint32_t FBTXN_DBG1; 
    __IO uint32_t FBTXN_DBG2;//0X338  OFFSET
    __IO uint32_t RESERVED2[(0x400-0x338)/4-1];
}ISPDMA_FBTXN;

typedef struct
{

    __IO uint32_t FBRXN_DCSZ;//0X700  OFFSET
    __IO uint32_t FBRXN_SDCA; 

    __IO uint32_t FBRXN_CTRL;  //0X708 OFFSET
    __IO uint32_t RESERVED15[(0x724-0x708)/4-1];
    __IO uint32_t FBRXN_TMR; //0X724 OFFSET
    __IO uint32_t FBRXN_RAMCTRL;
    __IO uint32_t RESERVED16;
    __IO uint32_t FBRXN_STAT;//0X730 OFFSET

    __IO uint32_t FBRXN_DBG1;
    __IO uint32_t FBRXN_DBG2;//0X738 OFFSET
    __IO uint32_t RESERVED17[(0x800-0x738)/4-1];

}ISPDMA_FBRXN;


typedef struct
{
    __IO uint32_t MAINCTRL;//0X00 offset
    __IO uint32_t DMAENA;
    __IO uint32_t CLKENA; 
    __IO uint32_t  RESERVED0;
    __IO uint32_t IRQRAW; //0x10 OFFSET
    __IO uint32_t IRQMASK;
    __IO uint32_t IRQSTAT; 
    __IO uint32_t  RESERVED1;
    __IO uint32_t INSZ;//0X20 0FFSET
    __IO uint32_t RESERVED2[(0x30-0x20)/4-1];
    __IO uint32_t INPG_CTRL0;//0x30 OFFSET
    __IO uint32_t INPG_CTRL1; 
    __IO uint32_t INPG_CTRL2;
    __IO uint32_t INPG_CTRL3;
    __IO uint32_t ISP_SRAM_CTRL0; 
    __IO uint32_t ISP_SRAM_CTRL1;
    __IO uint32_t ISP_SRAM_CTRL2;
    __IO uint32_t ISP_SRAM_CTRL3; 
    __IO uint32_t ISP_MEM_CTRL; 	
    __IO uint32_t RESERVED3[(0x100-0x50)/4-1];
    __IO uint32_t DISP_SDCA;//0x100 OFFSET
    __IO uint32_t DISP_DCSZ;
    __IO uint32_t DISP_CTRL;
    __IO uint32_t DISP_DSTSZ;  
    __IO uint32_t DISP_DSTADDR;//0x110 OFFSET
    __IO uint32_t DISP_HSIZE;
    __IO uint32_t DISP_VSIZE;
    __IO uint32_t DISP_PITCH;//0X11C  OFFSET
    __IO uint32_t RESERVED4[(0x128-0x11C)/4-1];
    __IO uint32_t DISP_RAMCTRL;//0X128 OFFSET
    __IO uint32_t RESERVED5;	
    __IO uint32_t DISP_STAT; 
    __IO uint32_t DISP_DBG1;
    __IO uint32_t DISP_DBG2;	//138
    __IO uint32_t DISP_CTRL_1;
    __IO uint32_t DISP_SDCA_U;	
    __IO uint32_t DISP_DCSZ_U;
    __IO uint32_t DISP_SDCA_V;
    __IO uint32_t DISP_DCSZ_V;
    __IO uint32_t DISP_DSTADDR_U;	//150
    __IO uint32_t DISP_DSTSZ_U;
    __IO uint32_t DISP_DSTADDR_V;
    __IO uint32_t DISP_DSTSZ_V;
    __IO uint32_t RESERVED61[(0x200-0x15C)/4-1];

    __IO uint32_t CODEC_SDCA; //0X200 OFFSET
    __IO uint32_t CODEC_DCSZ;
    __IO uint32_t CODEC_CTRL;
    __IO uint32_t CODEC_DSTSZ; 
    __IO uint32_t CODEC_DSTADDR;//0X210
    __IO uint32_t CODEC_HSIZE;
    __IO uint32_t CODEC_VSIZE; 
    __IO uint32_t CODEC_PITCH;
    __IO uint32_t CODEC_VBSZ;   //0X220 OFFSET 
    __IO uint32_t RESERVED9;
    __IO uint32_t CODEC_RAMCTRL; 
    __IO uint32_t RESERVED10;
    __IO uint32_t CODEC_STAT;  //0X230  OFFSET
    __IO uint32_t CODEC_DBG1;
    __IO uint32_t CODEC_DBG2; //0x238 OFFSET
    __IO uint32_t CODEC_CTRL_1; 
    __IO uint32_t CODEC_SDCA_U; //0x240 OFFSET
    __IO uint32_t CODEC_DCSZ_U; //0x244 OFFSET
    __IO uint32_t CODEC_SDCA_V; //0x248 OFFSET
    __IO uint32_t CODEC_DCSZ_V; //0x24C OFFSET
    __IO uint32_t CODEC_DSTADDR_U;
    __IO uint32_t CODEC_DSTSZ_U;
    __IO uint32_t CODEC_DSTADDR_V;
    __IO uint32_t CODEC_DSTSZ_V;
    __IO uint32_t RESERVED11[(0x300-0x25C)/4-1];

    __IO ISPDMA_FBTXN FBTXN[4];
    __IO ISPDMA_FBRXN FBRXN[4];

    __IO uint32_t AHB_TO_PS_Data;//0XB00 OFFSET
    __IO uint32_t RESERVED18[(0xB20-0xB00)/4-1];
    __IO uint32_t AHB_TO_PS_CTRL;//0XB20 OFFSET
    __IO uint32_t RESERVED19[(0xB10-0xB00)/4-1];
    __IO uint32_t PGEN_START_CTRL; //0XB30 OFFSET
    __IO uint32_t PGEN_STOP_CTRL;//0XB34 OFFSET
    __IO uint32_t RESERVED20[(0xC00-0xB34)/4-1];
    __IO uint32_t INPSDMA_CTRL;//0XC00 OFFSET
    __IO uint32_t INPSDMA_SRCADDR; 
    __IO uint32_t INPSDMA_SRCSZ;
    __IO uint32_t INPSDMA_PIXSZ;
    __IO uint32_t INPSDMA_RAMCTRL; 
    __IO uint32_t INPSDMA_STAT;
    __IO uint32_t INPSDMA_DBG1;
    __IO uint32_t INPSDMA_DBG2;  //0XC1C OFFSET
    __IO uint32_t RESERVED21[(0xd00-0xc1c)/4-1];
    __IO uint32_t ISPIRQRAW;	 //0XD00 OFFSET  
    __IO uint32_t ISPIRQMASK;
    __IO uint32_t ISPIRQSTAT; 
  
    __IO uint32_t RESERVED22[(0xE00-0xD08)/4-1];
    __IO uint32_t AHBDMA_SDCA; //0XE00 OFFSET
    __IO uint32_t AHBDMA_DCSZ;
    __IO uint32_t AHBDMA_CTRL;//0XE08  OFFSET
    __IO uint32_t RESERVED23[(0xE28-0xE08)/4-1];
    __IO uint32_t AHBDMA_RAMCTRL; //0XE28  OFFSET
    __IO uint32_t RESERVED24;
    __IO uint32_t AHBDMA_STAT;//0XE30 OFFSET
    __IO uint32_t AHBDMA_DGB1;
    __IO uint32_t AHBDMA_DGB2; 

}ISPDMA_TypeDef;


#define    ISPDMA_BASE       0xD420F000


#define    ISPDMA  (( ISPDMA_TypeDef *) ISPDMA_BASE )

//Bitfield Constants Register ISPDMA_MAINCTRL
#define MAINCTRL_CSI2_PAR_RST_MASK                     0x10000000
#define MAINCTRL_CSI2_PAR_RST_SHIFT                    28
#define MAINCTRL_LCD_IF_SEL_MASK                       0x08000000
#define MAINCTRL_LCD_IF_SEL_SHIFT                      27
#define MAINCTRL_EOF_ERR_DIS_MASK                      0x04000000
#define MAINCTRL_EOF_ERR_DIS_SHIFT                     26
#define MAINCTRL_EOL_EOF_DET_MASK                      0x02000000
#define MAINCTRL_EOL_EOF_DET_SHIFT                     25
#define MAINCTRL_CAP_ENA_MASK                          0x01000000
#define MAINCTRL_CAP_ENA_SHIFT                         24
#define MAINCTRL_PARVSYNCINV_MASK                      0x00008000
#define MAINCTRL_PARVSYNCINV_SHIFT                     15
#define MAINCTRL_ISPCLKMUXSEL_MASK                     0x00006000
#define MAINCTRL_ISPCLKMUXSEL_SHIFT                    13
#define MAINCTRL_INPSWAITST_MASK                       0x00001000
#define MAINCTRL_INPSWAITST_SHIFT                      12
#define MAINCTRL_AHBSWWAITST_MASK                      0x00000C00
#define MAINCTRL_AHBSWWAITST_SHIFT                     10
#define MAINCTRL_AHBSRWAITST_MASK                      0x00000300
#define MAINCTRL_AHBSRWAITST_SHIFT                     8
#define MAINCTRL_PIXMUX_MASK                           0x00000080
#define MAINCTRL_PIXMUX_SHIFT                          7
#define MAINCTRL_CSI2END_MASK                          0x00000040
#define MAINCTRL_CSI2END_SHIFT                         6
#define MAINCTRL_INBPP_MASK                            0x00000038
#define MAINCTRL_INBPP_SHIFT                           3
#define MAINCTRL_INPSSEL_MASK                          0x00000007
#define MAINCTRL_INPSSEL_SHIFT                         0






//Bitfield Constants Register ISPDMA_DMAENA
#define DMAENA_FBRX3DMAENA_MASK                      0x00000200
#define DMAENA_FBRX3DMAENA_SHIFT                     9
#define DMAENA_FBRX2DMAENA_MASK                      0x00000100
#define DMAENA_FBRX2DMAENA_SHIFT                     8
#define DMAENA_FBRX1DMAENA_MASK                      0x00000080
#define DMAENA_FBRX1DMAENA_SHIFT                     7
#define DMAENA_FBRX0DMAENA_MASK                      0x00000040
#define DMAENA_FBRX0DMAENA_SHIFT                     6
#define DMAENA_FBTX3DMAENA_MASK                      0x00000020
#define DMAENA_FBTX3DMAENA_SHIFT                     5
#define DMAENA_FBTX2DMAENA_MASK                      0x00000010
#define DMAENA_FBTX2DMAENA_SHIFT                     4
#define DMAENA_FBTX1DMAENA_MASK                      0x00000008
#define DMAENA_FBTX1DMAENA_SHIFT                     3
#define DMAENA_FBTX0DMAENA_MASK                      0x00000004
#define DMAENA_FBTX0DMAENA_SHIFT                     2
#define DMAENA_CODECDMAENA_MASK                      0x00000002
#define DMAENA_CODECDMAENA_SHIFT                     1
#define DMAENA_DISPDMAENA_MASK                       0x00000001
#define DMAENA_DISPDMAENA_SHIFT                      0

//Bitfield Constants Register ISPDMA_CLKENA
#define CLKENA_INPSDMACLKENA_MASK                    0x00003000
#define CLKENA_INPSDMACLKENA_SHIFT                   12
#define CLKENA_CODECCLKENA_MASK                      0x00000C00
#define CLKENA_CODECCLKENA_SHIFT                     10
#define CLKENA_DISPCLKENA_MASK                       0x00000300
#define CLKENA_DISPCLKENA_SHIFT                      8
#define CLKENA_FB3CLKENA_MASK                        0x000000C0
#define CLKENA_FB3CLKENA_SHIFT                       6
#define CLKENA_FB2CLKENA_MASK                        0x00000030
#define CLKENA_FB2CLKENA_SHIFT                       4
#define CLKENA_FB1CLKENA_MASK                        0x0000000C
#define CLKENA_FB1CLKENA_SHIFT                       2
#define CLKENA_FB0CLKENA_MASK                        0x00000003
#define CLKENA_FB0CLKENA_SHIFT                       0



//Bitfield Constants Register ISPDMA_IRQRAW
#define IRQRAW_CSI2BRIDGE_MASK                       0x00020000
#define IRQRAW_CSI2BRIDGE_SHIFT                      17
#define IRQRAW_FBTX3EOF1_MASK                        0x00010000
#define IRQRAW_FBTX3EOF1_SHIFT                       16
#define IRQRAW_FBTX2EOF1_MASK                        0x00008000
#define IRQRAW_FBTX2EOF1_SHIFT                       15
#define IRQRAW_FBTX1EOF1_MASK                        0x00004000
#define IRQRAW_FBTX1EOF1_SHIFT                       14
#define IRQRAW_FBTX0EOF1_MASK                        0x00002000
#define IRQRAW_FBTX0EOF1_SHIFT                       13
#define IRQRAW_CODECEOF_MASK                         0x00001000
#define IRQRAW_CODECEOF_SHIFT                        12
#define IRQRAW_DISPEOF_MASK                          0x00000800
#define IRQRAW_DISPEOF_SHIFT                         11
#define IRQRAW_INPSEOF_MASK                          0x00000400
#define IRQRAW_INPSEOF_SHIFT                         10
#define IRQRAW_FBRX3EOF_MASK                         0x00000200
#define IRQRAW_FBRX3EOF_SHIFT                        9
#define IRQRAW_FBRX2EOF_MASK                         0x00000100
#define IRQRAW_FBRX2EOF_SHIFT                        8
#define IRQRAW_FBRX1EOF_MASK                         0x00000080
#define IRQRAW_FBRX1EOF_SHIFT                        7
#define IRQRAW_FBRX0EOF_MASK                         0x00000040
#define IRQRAW_FBRX0EOF_SHIFT                        6
#define IRQRAW_FBTX3EOF_MASK                         0x00000020
#define IRQRAW_FBTX3EOF_SHIFT                        5
#define IRQRAW_FBTX2EOF_MASK                         0x00000010
#define IRQRAW_FBTX2EOF_SHIFT                        4
#define IRQRAW_FBTX1EOF_MASK                         0x00000008
#define IRQRAW_FBTX1EOF_SHIFT                        3
#define IRQRAW_FBTX0EOF_MASK                         0x00000004
#define IRQRAW_FBTX0EOF_SHIFT                        2
#define IRQRAW_CODECDMAEOF_MASK                      0x00000002
#define IRQRAW_CODECDMAEOF_SHIFT                     1
#define IRQRAW_DISPDMAEOF_MASK                       0x00000001
#define IRQRAW_DISPDMAEOF_SHIFT                      0



//Bitfield Constants Register ISPDMA_IRQMASK
#define IRQMASK_CSI2BRIDGE_MASK                       0x00020000
#define IRQMASK_CSI2BRIDGE_SHIFT                      17
#define IRQMASK_FBTX3EOF1_MASK                        0x00010000
#define IRQMASK_FBTX3EOF1_SHIFT                       16
#define IRQMASK_FBTX2EOF1_MASK                        0x00008000
#define IRQMASK_FBTX2EOF1_SHIFT                       15
#define IRQMASK_FBTX1EOF1_MASK                        0x00004000
#define IRQMASK_FBTX1EOF1_SHIFT                       14
#define IRQMASK_FBTX0EOF1_MASK                        0x00002000
#define IRQMASK_FBTX0EOF1_SHIFT                       13
#define IRQMASK_CODECEOF_MASK                         0x00001000
#define IRQMASK_CODECEOF_SHIFT                        12
#define IRQMASK_DISPEOF_MASK                          0x00000800
#define IRQMASK_DISPEOF_SHIFT                         11
#define IRQMASK_INPSEOF_MASK                          0x00000400
#define IRQMASK_INPSEOF_SHIFT                         10
#define IRQMASK_FBRX3EOF_MASK                         0x00000200
#define IRQMASK_FBRX3EOF_SHIFT                        9
#define IRQMASK_FBRX2EOF_MASK                         0x00000100
#define IRQMASK_FBRX2EOF_SHIFT                        8
#define IRQMASK_FBRX1EOF_MASK                         0x00000080
#define IRQMASK_FBRX1EOF_SHIFT                        7
#define IRQMASK_FBRX0EOF_MASK                         0x00000040
#define IRQMASK_FBRX0EOF_SHIFT                        6
#define IRQMASK_FBTX3EOF_MASK                         0x00000020
#define IRQMASK_FBTX3EOF_SHIFT                        5
#define IRQMASK_FBTX2EOF_MASK                         0x00000010
#define IRQMASK_FBTX2EOF_SHIFT                        4
#define IRQMASK_FBTX1EOF_MASK                         0x00000008
#define IRQMASK_FBTX1EOF_SHIFT                        3
#define IRQMASK_FBTX0EOF_MASK                         0x00000004
#define IRQMASK_FBTX0EOF_SHIFT                        2
#define IRQMASK_CODECDMAEOF_MASK                      0x00000002
#define IRQMASK_CODECDMAEOF_SHIFT                     1
#define IRQMASK_DISPDMAEOF_MASK                       0x00000001
#define IRQMASK_DISPDMAEOF_SHIFT                      0



//Bitfield Constants Register ISPDMA_IRQSTAT
#define IRQSTAT_CSI2BRIDGE_MASK                       0x00020000
#define IRQSTAT_CSI2BRIDGE_SHIFT                      17
#define IRQSTAT_FBTX3EOF1_MASK                        0x00010000
#define IRQSTAT_FBTX3EOF1_SHIFT                       16
#define IRQSTAT_FBTX2EOF1_MASK                        0x00008000
#define IRQSTAT_FBTX2EOF1_SHIFT                       15
#define IRQSTAT_FBTX1EOF1_MASK                        0x00004000
#define IRQSTAT_FBTX1EOF1_SHIFT                       14
#define IRQSTAT_FBTX0EOF1_MASK                        0x00002000
#define IRQSTAT_FBTX0EOF1_SHIFT                       13
#define IRQSTAT_CODECEOF_MASK                         0x00001000
#define IRQSTAT_CODECEOF_SHIFT                        12
#define IRQSTAT_DISPEOF_MASK                          0x00000800
#define IRQSTAT_DISPEOF_SHIFT                         11
#define IRQSTAT_INPSEOF_MASK                          0x00000400
#define IRQSTAT_INPSEOF_SHIFT                         10
#define IRQSTAT_FBRX3EOF_MASK                         0x00000200
#define IRQSTAT_FBRX3EOF_SHIFT                        9
#define IRQSTAT_FBRX2EOF_MASK                         0x00000100
#define IRQSTAT_FBRX2EOF_SHIFT                        8
#define IRQSTAT_FBRX1EOF_MASK                         0x00000080
#define IRQSTAT_FBRX1EOF_SHIFT                        7
#define IRQSTAT_FBRX0EOF_MASK                         0x00000040
#define IRQSTAT_FBRX0EOF_SHIFT                        6
#define IRQSTAT_FBTX3EOF_MASK                         0x00000020
#define IRQSTAT_FBTX3EOF_SHIFT                        5
#define IRQSTAT_FBTX2EOF_MASK                         0x00000010
#define IRQSTAT_FBTX2EOF_SHIFT                        4
#define IRQSTAT_FBTX1EOF_MASK                         0x00000008
#define IRQSTAT_FBTX1EOF_SHIFT                        3
#define IRQSTAT_FBTX0EOF_MASK                         0x00000004
#define IRQSTAT_FBTX0EOF_SHIFT                        2
#define IRQSTAT_CODECDMAEOF_MASK                      0x00000002
#define IRQSTAT_CODECDMAEOF_SHIFT                     1
#define IRQSTAT_DISPDMAEOF_MASK                       0x00000001
#define IRQSTAT_DISPDMAEOF_SHIFT                      0



//Bitfield Constants Register ISPDMA_INSZ
#define INSZ_INPSVSIZE_MASK                        0x1FFF0000
#define INSZ_INPSVSIZE_SHIFT                       16
#define INSZ_INPSHSIZE_MASK                        0x00003FFF
#define INSZ_INPSHSIZE_SHIFT                       0



//Bitfield Constants Register ISPDMA_INPG_CTRL0
#define INPG_CTRL0_PG_MD_MASK                            0x00000018
#define INPG_CTRL0_PG_MD_SHIFT                           3
#define INPG_CTRL0_PG_RUNMD_MASK                         0x00000006
#define INPG_CTRL0_PG_RUNMD_SHIFT                        1
#define INPG_CTRL0_PG_ENA_MASK                           0x00000001
#define INPG_CTRL0_PG_ENA_SHIFT                          0


//Bitfield Constants Register ISPDMA_INPG_CTRL1
#define INPG_CTRL1_PG_BYR1_MASK                          0x0FFF0000
#define INPG_CTRL1_PG_BYR1_SHIFT                         16
#define INPG_CTRL1_PG_BYR0_MASK                          0x00000FFF
#define INPG_CTRL1_PG_BYR0_SHIFT                         0

//Bitfield Constants Register ISPDMA_INPG_CTRL2
#define INPG_CTRL2_PG_BYR3_MASK                          0x0FFF0000
#define INPG_CTRL2_PG_BYR3_SHIFT                         16
#define INPG_CTRL2_PG_BYR2_MASK                          0x00000FFF
#define INPG_CTRL2_PG_BYR2_SHIFT                         0

//Bitfield Constants Register ISPDMA_INPG_CTRL3
#define INPG_CTRL3_INPGVSIZE_MASK                        0x1FFF0000
#define INPG_CTRL3_INPGVSIZE_SHIFT                       16
#define INPG_CTRL3_INPGHSIZE_MASK                        0x00003FFF
#define INPG_CTRL3_INPGHSIZE_SHIFT                       0



//Bitfield Constants Register ISP_SRAM_CTRL0
#define ISP_SRAM_CTRL0_WTC6_MASK                             0xC0000000
#define ISP_SRAM_CTRL0_WTC6_SHIFT                            30
#define ISP_SRAM_CTRL0_RTC6_MASK                             0x30000000
#define ISP_SRAM_CTRL0_RTC6_SHIFT                            28
#define ISP_SRAM_CTRL0_WTC5_MASK                             0x0C000000
#define ISP_SRAM_CTRL0_WTC5_SHIFT                            26
#define ISP_SRAM_CTRL0_RTC5_MASK                             0x03000000
#define ISP_SRAM_CTRL0_RTC5_SHIFT                            24
#define ISP_SRAM_CTRL0_WTC4_MASK                             0x00C00000
#define ISP_SRAM_CTRL0_WTC4_SHIFT                            22
#define ISP_SRAM_CTRL0_RTC4_MASK                             0x00300000
#define ISP_SRAM_CTRL0_RTC4_SHIFT                            20
#define ISP_SRAM_CTRL0_WTC3_MASK                             0x000C0000
#define ISP_SRAM_CTRL0_WTC3_SHIFT                            18
#define ISP_SRAM_CTRL0_RTC3_MASK                             0x00030000
#define ISP_SRAM_CTRL0_RTC3_SHIFT                            16
#define ISP_SRAM_CTRL0_WTC2_MASK                             0x0000C000
#define ISP_SRAM_CTRL0_WTC2_SHIFT                            14
#define ISP_SRAM_CTRL0_RTC2_MASK                             0x00003000
#define ISP_SRAM_CTRL0_RTC2_SHIFT                            12
#define ISP_SRAM_CTRL0_WTC1_MASK                             0x00000C00
#define ISP_SRAM_CTRL0_WTC1_SHIFT                            10
#define ISP_SRAM_CTRL0_RTC1_MASK                             0x00000300
#define ISP_SRAM_CTRL0_RTC1_SHIFT                            8
#define ISP_SRAM_CTRL0_WTC0_MASK                             0x000000C0
#define ISP_SRAM_CTRL0_WTC0_SHIFT                            6
#define ISP_SRAM_CTRL0_RTC0_MASK                             0x00000030
#define ISP_SRAM_CTRL0_RTC0_SHIFT                            4
#define ISP_SRAM_CTRL0_PDWN_MASK                             0x00000001
#define ISP_SRAM_CTRL0_PDWN_SHIFT                            0


//Bitfield Constants Register ISP_SRAM_CTRL1
#define ISP_SRAM_CTRL1_WTC14_MASK                            0xC0000000
#define ISP_SRAM_CTRL1_WTC14_SHIFT                           30
#define ISP_SRAM_CTRL1_RTC14_MASK                            0x30000000
#define ISP_SRAM_CTRL1_RTC14_SHIFT                           28
#define ISP_SRAM_CTRL1_WTC13_MASK                            0x0C000000
#define ISP_SRAM_CTRL1_WTC13_SHIFT                           26
#define ISP_SRAM_CTRL1_RTC13_MASK                            0x03000000
#define ISP_SRAM_CTRL1_RTC13_SHIFT                           24
#define ISP_SRAM_CTRL1_WTC12_MASK                            0x00C00000
#define ISP_SRAM_CTRL1_WTC12_SHIFT                           22
#define ISP_SRAM_CTRL1_RTC12_MASK                            0x00300000
#define ISP_SRAM_CTRL1_RTC12_SHIFT                           20
#define ISP_SRAM_CTRL1_WTC11_MASK                            0x000C0000
#define ISP_SRAM_CTRL1_WTC11_SHIFT                           18
#define ISP_SRAM_CTRL1_RTC11_MASK                            0x00030000
#define ISP_SRAM_CTRL1_RTC11_SHIFT                           16
#define ISP_SRAM_CTRL1_WTC10_MASK                            0x0000C000
#define ISP_SRAM_CTRL1_WTC10_SHIFT                           14
#define ISP_SRAM_CTRL1_RTC10_MASK                            0x00003000
#define ISP_SRAM_CTRL1_RTC10_SHIFT                           12
#define ISP_SRAM_CTRL1_WTC9_MASK                             0x00000C00
#define ISP_SRAM_CTRL1_WTC9_SHIFT                            10
#define ISP_SRAM_CTRL1_RTC9_MASK                             0x00000300
#define ISP_SRAM_CTRL1_RTC9_SHIFT                            8
#define ISP_SRAM_CTRL1_WTC8_MASK                             0x000000C0
#define ISP_SRAM_CTRL1_WTC8_SHIFT                            6
#define ISP_SRAM_CTRL1_RTC8_MASK                             0x00000030
#define ISP_SRAM_CTRL1_RTC8_SHIFT                            4
#define ISP_SRAM_CTRL1_WTC7_MASK                             0x0000000C
#define ISP_SRAM_CTRL1_WTC7_SHIFT                            2
#define ISP_SRAM_CTRL1_RTC7_MASK                             0x00000003
#define ISP_SRAM_CTRL1_RTC7_SHIFT                            0


//Bitfield Constants Register ISP_SRAM_CTRL2
#define ISP_SRAM_CTRL2_WTC22_MASK                            0xC0000000
#define ISP_SRAM_CTRL2_WTC22_SHIFT                           30
#define ISP_SRAM_CTRL2_RTC22_MASK                            0x30000000
#define ISP_SRAM_CTRL2_RTC22_SHIFT                           28
#define ISP_SRAM_CTRL2_WTC21_MASK                            0x0C000000
#define ISP_SRAM_CTRL2_WTC21_SHIFT                           26
#define ISP_SRAM_CTRL2_RTC21_MASK                            0x03000000
#define ISP_SRAM_CTRL2_RTC21_SHIFT                           24
#define ISP_SRAM_CTRL2_WTC20_MASK                            0x00C00000
#define ISP_SRAM_CTRL2_WTC20_SHIFT                           22
#define ISP_SRAM_CTRL2_RTC20_MASK                            0x00300000
#define ISP_SRAM_CTRL2_RTC20_SHIFT                           20
#define ISP_SRAM_CTRL2_WTC19_MASK                            0x000C0000
#define ISP_SRAM_CTRL2_WTC19_SHIFT                           18
#define ISP_SRAM_CTRL2_RTC19_MASK                            0x00030000
#define ISP_SRAM_CTRL2_RTC19_SHIFT                           16
#define ISP_SRAM_CTRL2_WTC18_MASK                            0x0000C000
#define ISP_SRAM_CTRL2_WTC18_SHIFT                           14
#define ISP_SRAM_CTRL2_RTC18_MASK                            0x00003000
#define ISP_SRAM_CTRL2_RTC18_SHIFT                           12
#define ISP_SRAM_CTRL2_WTC17_MASK                            0x00000C00
#define ISP_SRAM_CTRL2_WTC17_SHIFT                           10
#define ISP_SRAM_CTRL2_RTC17_MASK                            0x00000300
#define ISP_SRAM_CTRL2_RTC17_SHIFT                           8
#define ISP_SRAM_CTRL2_WTC16_MASK                            0x000000C0
#define ISP_SRAM_CTRL2_WTC16_SHIFT                           6
#define ISP_SRAM_CTRL2_RTC16_MASK                            0x00000030
#define ISP_SRAM_CTRL2_RTC16_SHIFT                           4
#define ISP_SRAM_CTRL2_WTC15_MASK                            0x0000000C
#define ISP_SRAM_CTRL2_WTC15_SHIFT                           2
#define ISP_SRAM_CTRL2_RTC15_MASK                            0x00000003
#define ISP_SRAM_CTRL2_RTC15_SHIFT                           0





//Bitfield Constants Register ISP_SRAM_CTRL3
#define ISP_SRAM_CTRL3_WTC20_MASK                            0x00C00000
#define ISP_SRAM_CTRL3_WTC20_SHIFT                           22
#define ISP_SRAM_CTRL3_RTC20_MASK                            0x00300000
#define ISP_SRAM_CTRL3_RTC20_SHIFT                           20
#define ISP_SRAM_CTRL3_WTC19_MASK                            0x000C0000
#define ISP_SRAM_CTRL3_WTC19_SHIFT                           18
#define ISP_SRAM_CTRL3_RTC19_MASK                            0x00030000
#define ISP_SRAM_CTRL3_RTC19_SHIFT                           16
#define ISP_SRAM_CTRL3_WTC18_MASK                            0x0000C000
#define ISP_SRAM_CTRL3_WTC18_SHIFT                           14
#define ISP_SRAM_CTRL3_RTC18_MASK                            0x00003000
#define ISP_SRAM_CTRL3_RTC18_SHIFT                           12
#define ISP_SRAM_CTRL3_WTC17_MASK                            0x00000C00
#define ISP_SRAM_CTRL3_WTC17_SHIFT                           10
#define ISP_SRAM_CTRL3_RTC17_MASK                            0x00000300
#define ISP_SRAM_CTRL3_RTC17_SHIFT                           8
#define ISP_SRAM_CTRL3_WTC16_MASK                            0x000000C0
#define ISP_SRAM_CTRL3_WTC16_SHIFT                           6
#define ISP_SRAM_CTRL3_RTC16_MASK                            0x00000030
#define ISP_SRAM_CTRL3_RTC16_SHIFT                           4
#define ISP_SRAM_CTRL3_WTC15_MASK                            0x0000000C
#define ISP_SRAM_CTRL3_WTC15_SHIFT                           2
#define ISP_SRAM_CTRL3_RTC15_MASK                            0x00000003
#define ISP_SRAM_CTRL3_RTC15_SHIFT                           0



//Bitfield Constants Register ISPDMA_DISP_SDCA
#define DISP_SDCA_SDCA_MASK                             0xFFFFFFFC
#define DISP_SDCA_SDCA_SHIFT                            2


//Bitfield Constants Register ISPDMA_DISP_DCSZ
#define DISP_DCSZ_DCSZ_MASK                             0x00FFFFF8
#define DISP_DCSZ_DCSZ_SHIFT                            3


//Bitfield Constants Register ISPDMA_DISP_CTRL
#define DISP_CTRL_FFMAXLVL_MASK                         0x00000F00
#define DISP_CTRL_FFMAXLVL_SHIFT                        8
#define DISP_CTRL_DMABRSTSZ_MASK                        0x00000030
#define DISP_CTRL_DMABRSTSZ_SHIFT                       4
#define DISP_CTRL_DC_ENA_MASK                           0x00000002
#define DISP_CTRL_DC_ENA_SHIFT                          1



//Bitfield Constants Register ISPDMA_DISP_DSTADDR
#define DISP_DSTADDR_DSTADDR_MASK                          0xFFFFFFFF
#define DISP_DSTADDR_DSTADDR_SHIFT                         0


//Bitfield Constants Register ISPDMA_DISP_HSIZE
#define DISP_HSIZE_HSIZE_MASK                            0x03FFFFFF
#define DISP_HSIZE_HSIZE_SHIFT                           0


//Bitfield Constants Register ISPDMA_DISP_VSIZE
#define DISP_VSIZE_VSIZE_MASK                            0x00001FFF
#define DISP_VSIZE_VSIZE_SHIFT                           0




//Bitfield Constants Register ISPDMA_DISP_PITCH
#define DISP_PITCH_PITCH_MASK                            0x00003FFF
#define DISP_PITCH_PITCH_SHIFT                           0


//Bitfield Constants Register ISPDMA_DISP_RAMCTRL
#define DISP_RAMCTRL_DCFF_PDWN_MASK                        0x00000200
#define DISP_RAMCTRL_DCFF_PDWN_SHIFT                       9
#define DISP_RAMCTRL_DCFF_WTC_MASK                         0x00000180
#define DISP_RAMCTRL_DCFF_WTC_SHIFT                        7
#define DISP_RAMCTRL_DCFF_RTC_MASK                         0x00000060
#define DISP_RAMCTRL_DCFF_RTC_SHIFT                        5
#define DISP_RAMCTRL_FF_PDWN_MASK                          0x00000010
#define DISP_RAMCTRL_FF_PDWN_SHIFT                         4
#define DISP_RAMCTRL_FF_WTC_MASK                           0x0000000C
#define DISP_RAMCTRL_FF_WTC_SHIFT                          2
#define DISP_RAMCTRL_FF_RTC_MASK                           0x00000003
#define DISP_RAMCTRL_FF_RTC_SHIFT                          0


//Bitfield Constants Register ISPDMA_DISP_STAT
#define DISP_STAT_PSIFFFSTA_MASK                        0x00000004
#define DISP_STAT_PSIFFFSTA_SHIFT                       2
#define DISP_STAT_DCDMASTAT_MASK                        0x00000002
#define DISP_STAT_DCDMASTAT_SHIFT                       1
#define DISP_STAT_DMASTAT_MASK                          0x00000001
#define DISP_STAT_DMASTAT_SHIFT                         0


//Bitfield Constants Register ISPDMA_DISP_DBG1
#define DISP_DBG1_W_ADDR_MASK                           0xFFFFFC00
#define DISP_DBG1_W_ADDR_SHIFT                          10
#define DISP_DBG1_W_VLD_MASK                            0x00000200
#define DISP_DBG1_W_VLD_SHIFT                           9
#define DISP_DBG1_W_NBYTES_MASK                         0x000001FF
#define DISP_DBG1_W_NBYTES_SHIFT                        0

//Bitfield Constants Register ISPDMA_DISP_DBG2
#define DISP_DBG2_R_ADDR_MASK                           0xFFFFFC00
#define DISP_DBG2_R_ADDR_SHIFT                          10
#define DISP_DBG2_R_VLD_MASK                            0x00000200
#define DISP_DBG2_R_VLD_SHIFT                           9
#define DISP_DBG2_R_NBYTES_MASK                         0x000001FF
#define DISP_DBG2_R_NBYTES_SHIFT                        0


//Bitfield Constants Register ISPDMA_CODEC_SDCA
#define CODEC_SDCA_SDCA_MASK                             0xFFFFFFFC
#define CODEC_SDCA_SDCA_SHIFT                            2

//Bitfield Constants Register ISPDMA_CODEC_DCSZ
#define CODEC_DCSZ_DCSZ_MASK                             0x00FFFFF8
#define CODEC_DCSZ_DCSZ_SHIFT                            3

//Bitfield Constants Register ISPDMA_CODEC_CTRL
#define CODEC_CTRL_FFMAXLVL_MASK                         0x00000F00
#define CODEC_CTRL_FFMAXLVL_SHIFT                        8
#define CODEC_CTRL_VBNUM_MASK                            0x000000C0
#define CODEC_CTRL_VBNUM_SHIFT                           6
#define CODEC_CTRL_DMABRSTSZ_MASK                        0x00000030
#define CODEC_CTRL_DMABRSTSZ_SHIFT                       4
#define CODEC_CTRL_REORD_ENA_MASK                        0x00000008
#define CODEC_CTRL_REORD_ENA_SHIFT                       3
#define CODEC_CTRL_DC_ENA_MASK                           0x00000002
#define CODEC_CTRL_DC_ENA_SHIFT                          1

//Bitfield Constants Register ISPDMA_CODEC_DSTSZ
#define CODEC_DSTSZ_DSTSZ_MASK                            0x03FFFFFF
#define CODEC_DSTSZ_DSTSZ_SHIFT                           0


//Bitfield Constants Register ISPDMA_CODEC_DSTADDR
#define CODEC_DSTADDR_DSTADDR_MASK                          0xFFFFFFFF
#define CODEC_DSTADDR_DSTADDR_SHIFT                         0

//Bitfield Constants Register ISPDMA_CODEC_HSIZE
#define CODEC_HSIZE_HSIZE_MASK                            0x03FFFFFF
#define CODEC_HSIZE_HSIZE_SHIFT                           0


//Bitfield Constants Register ISPDMA_CODEC_VSIZE
#define CODEC_VSIZE_VSIZE_MASK                            0x00001FFF
#define CODEC_VSIZE_VSIZE_SHIFT                           0

//Bitfield Constants Register ISPDMA_CODEC_PITCH
#define CODEC_PITCH_PITCH_MASK                            0x00003FFF
#define CODEC_PITCH_PITCH_SHIFT                           0


//Bitfield Constants Register ISPDMA_CODEC_VBSZ
#define CODEC_VBSZ_VBSZ_MASK                             0x00003FFF
#define CODEC_VBSZ_VBSZ_SHIFT                            0


//Bitfield Constants Register ISPDMA_CODEC_RAMCTRL
#define CODEC_RAMCTRL_DCFF_PDWN_MASK                        0x00000200
#define CODEC_RAMCTRL_DCFF_PDWN_SHIFT                       9
#define CODEC_RAMCTRL_DCFF_WTC_MASK                         0x00000180
#define CODEC_RAMCTRL_DCFF_WTC_SHIFT                        7
#define CODEC_RAMCTRL_DCFF_RTC_MASK                         0x00000060
#define CODEC_RAMCTRL_DCFF_RTC_SHIFT                        5
#define CODEC_RAMCTRL_FF_PDWN_MASK                          0x00000010
#define CODEC_RAMCTRL_FF_PDWN_SHIFT                         4
#define CODEC_RAMCTRL_FF_WTC_MASK                           0x0000000C
#define CODEC_RAMCTRL_FF_WTC_SHIFT                          2
#define CODEC_RAMCTRL_FF_RTC_MASK                           0x00000003
#define CODEC_RAMCTRL_FF_RTC_SHIFT                          0


//Bitfield Constants Register ISPDMA_CODEC_STAT
#define CODEC_STAT_PSIFFFSTA_MASK                        0x00000004
#define CODEC_STAT_PSIFFFSTA_SHIFT                       2
#define CODEC_STAT_DCDMASTAT_MASK                        0x00000002
#define CODEC_STAT_DCDMASTAT_SHIFT                       1
#define CODEC_STAT_DMASTAT_MASK                          0x00000001
#define CODEC_STAT_DMASTAT_SHIFT                         0



//Bitfield Constants Register ISPDMA_CODEC_DBG1
#define CODEC_DBG1_W_ADDR_MASK                           0xFFFFFC00
#define CODEC_DBG1_W_ADDR_SHIFT                          10
#define CODEC_DBG1_W_VLD_MASK                            0x00000200
#define CODEC_DBG1_W_VLD_SHIFT                           9
#define CODEC_DBG1_W_NBYTES_MASK                         0x000001FF
#define CODEC_DBG1_W_NBYTES_SHIFT                        0



//Bitfield Constants Register ISPDMA_CODEC_DBG2
#define CODEC_DBG2_R_ADDR_MASK                           0xFFFFFC00
#define CODEC_DBG2_R_ADDR_SHIFT                          10
#define CODEC_DBG2_R_VLD_MASK                            0x00000200
#define CODEC_DBG2_R_VLD_SHIFT                           9
#define CODEC_DBG2_R_NBYTES_MASK                         0x000001FF
#define CODEC_DBG2_R_NBYTES_SHIFT                        0


//Bitfield Constants Register ISPDMA_FBTXn_SDCA0_3
#define FBTXN_SDCA0_3_SDCA_MASK                             0xFFFFFFFC
#define FBTXN_SDCA0_3_SDCA_SHIFT                            2


//Bitfield Constants Register ISPDMA_FBTXn_DCSZ
#define FBTXN_DCSZ_DCSZ_MASK                             0x00FFFFF8
#define FBTXN_DCSZ_DCSZ_SHIFT                            3


//Bitfield Constants Register ISPDMA_FBTXn_CTRL
#define FBTXN_CTRL_EOFSEL_MASK                           0x00003000
#define FBTXN_CTRL_EOFSEL_SHIFT                          12
#define FBTXN_CTRL_FFMAXLVL_MASK                         0x00000F00
#define FBTXN_CTRL_FFMAXLVL_SHIFT                        8
#define FBTXN_CTRL_DMABRSTSZ_MASK                        0x00000030
#define FBTXN_CTRL_DMABRSTSZ_SHIFT                       4
#define FBTXN_CTRL_DC_ENA_MASK                           0x00000002
#define FBTXN_CTRL_DC_ENA_SHIFT                          1


//Bitfield Constants Register ISPDMA_FBTxn_DSTSZ
#define FBTXN_DSTSZ_DSTSZ_MASK                            0x03FFFFFF
#define FBTXN_DSTSZ_DSTSZ_SHIFT                           0


//Bitfield Constants Register ISPDMA_FBTXn_DSTADDR
#define FBTXN_DSTADDR_DSTADDR_MASK                          0xFFFFFFFF
#define FBTXN_DSTADDR_DSTADDR_SHIFT                         0


//Bitfield Constants Register ISPDMA_FBTXn_TMR
#define FBTXN_TMR_TMR_MASK                              0x00000FFF
#define FBTXN_TMR_TMR_SHIFT                             0


//Bitfield Constants Register ISPDMA_FBTXn_STAT
#define FBTXN_STAT_PSIFFFSTA_MASK                        0x00000004
#define FBTXN_STAT_PSIFFFSTA_SHIFT                       2
#define FBTXN_STAT_DCDMASTAT_MASK                        0x00000002
#define FBTXN_STAT_DCDMASTAT_SHIFT                       1
#define FBTXN_STAT_DMASTAT_MASK                          0x00000001
#define FBTXN_STAT_DMASTAT_SHIFT                         0

//Bitfield Constants Register ISPDMA_FBTXn_RAMCTRL
#define FBTXN_RAMCTRL_DCFF_PDWN_MASK                        0x00000200
#define FBTXN_RAMCTRL_DCFF_PDWN_SHIFT                       9
#define FBTXN_RAMCTRL_DCFF_WTC_MASK                         0x00000180
#define FBTXN_RAMCTRL_DCFF_WTC_SHIFT                        7
#define FBTXN_RAMCTRL_DCFF_RTC_MASK                         0x00000060
#define FBTXN_RAMCTRL_DCFF_RTC_SHIFT                        5
#define FBTXN_RAMCTRL_FF_PDWN_MASK                          0x00000010
#define FBTXN_RAMCTRL_FF_PDWN_SHIFT                         4
#define FBTXN_RAMCTRL_FF_WTC_MASK                           0x0000000C
#define FBTXN_RAMCTRL_FF_WTC_SHIFT                          2
#define FBTXN_RAMCTRL_FF_RTC_MASK                           0x00000003
#define FBTXN_RAMCTRL_FF_RTC_SHIFT                          0


//Bitfield Constants Register ISPDMA_FBTXn_DBG1
#define FBTXN_DBG1_W_ADDR_MASK                           0xFFFFFC00
#define FBTXN_DBG1_W_ADDR_SHIFT                          10
#define FBTXN_DBG1_W_VLD_MASK                            0x00000200
#define FBTXN_DBG1_W_VLD_SHIFT                           9
#define FBTXN_DBG1_W_NBYTES_MASK                         0x000001FF
#define FBTXN_DBG1_W_NBYTES_SHIFT                        0



//Bitfield Constants Register ISPDMA_FBTXn_DBG2
#define FBTXN_DBG2_W_ADDR_MASK                           0xFFFFFC00
#define FBTXN_DBG2_W_ADDR_SHIFT                          10
#define FBTXN_DBG2_W_VLD_MASK                            0x00000200
#define FBTXN_DBG2_W_VLD_SHIFT                           9
#define FBTXN_DBG2_W_NBYTES_MASK                         0x000001FF
#define FBTXN_DBG2_W_NBYTES_SHIFT                        0



//Bitfield Constants Register ISPDMA_FBRXn_DCSZ
#define FBRXN_DCSZ_DCSZ_MASK                             0x00FFFFF8
#define FBRXN_DCSZ_DCSZ_SHIFT                            3

//Bitfield Constants Register ISPDMA_FBRXn_SDCA
#define FBRXN_SDCA_SDCA_MASK                             0x00FFFFFC
#define FBRXN_SDCA_SDCA_SHIFT                            2

//Bitfield Constants Register ISPDMA_FBRXn_CTRL
#define FBRXN_CTRL_DMABRSTSZ_MASK                        0x00000030
#define FBRXN_CTRL_DMABRSTSZ_SHIFT                       4
#define FBRXN_CTRL_DC_ENA_MASK                           0x00000002
#define FBRXN_CTRL_DC_ENA_SHIFT                          1



//Bitfield Constants Register ISPDMA_FBRXn_TMR
#define FBRXN_TMR_TMR_MASK                              0x00000FFF
#define FBRXN_TMR_TMR_SHIFT                             0





//Bitfield Constants Register ISPDMA_FBRXn_RAMCTRL
#define FBRXN_RAMCTRL_DCFF_PDWN_MASK                        0x00000200
#define FBRXN_RAMCTRL_DCFF_PDWN_SHIFT                       9
#define FBRXN_RAMCTRL_DCFF_WTC_MASK                         0x00000180
#define FBRXN_RAMCTRL_DCFF_WTC_SHIFT                        7
#define FBRXN_RAMCTRL_DCFF_RTC_MASK                         0x00000060
#define FBRXN_RAMCTRL_DCFF_RTC_SHIFT                        5
#define FBRXN_RAMCTRL_FF_PDWN_MASK                          0x00000010
#define FBRXN_RAMCTRL_FF_PDWN_SHIFT                         4
#define FBRXN_RAMCTRL_FF_WTC_MASK                           0x0000000C
#define FBRXN_RAMCTRL_FF_WTC_SHIFT                          2
#define FBRXN_RAMCTRL_FF_RTC_MASK                           0x00000003
#define FBRXN_RAMCTRL_FF_RTC_SHIFT                          0




//Bitfield Constants Register ISPDMA_FBRXn_STAT
#define FBRXN_STAT_PSIFFFSTA_MASK                        0x00000004
#define FBRXN_STAT_PSIFFFSTA_SHIFT                       2
#define FBRXN_STAT_DCDMASTAT_MASK                        0x00000002
#define FBRXN_STAT_DCDMASTAT_SHIFT                       1
#define FBRXN_STAT_DMASTAT_MASK                          0x00000001
#define FBRXN_STAT_DMASTAT_SHIFT                         0


//Bitfield Constants Register ISPDMA_FBRXn_DBG1
#define FBRXN_DBG1_W_ADDR_MASK                           0xFFFFFC00
#define FBRXN_DBG1_W_ADDR_SHIFT                          10
#define FBRXN_DBG1_W_VLD_MASK                            0x00000200
#define FBRXN_DBG1_W_VLD_SHIFT                           9
#define FBRXN_DBG1_W_NBYTES_MASK                         0x000001FF
#define FBRXN_DBG1_W_NBYTES_SHIFT                        0



//Bitfield Constants Register ISPDMA_FBRXn_DBG2
#define FBRXN_DBG2_R_ADDR_MASK                           0xFFFFFC00
#define FBRXN_DBG2_R_ADDR_SHIFT                          10
#define FBRXN_DBG2_R_VLD_MASK                            0x00000200
#define FBRXN_DBG2_R_VLD_SHIFT                           9
#define FBRXN_DBG2_R_NBYTES_MASK                         0x000001FF
#define FBRXN_DBG2_R_NBYTES_SHIFT                        0




//Bitfield Constants Register AHB_TO_PS_Data
#define AHB_TO_PS_DATA_LPIL_MASK                             0xC0000000
#define AHB_TO_PS_DATA_LPIL_SHIFT                            30
#define AHB_TO_PS_DATA_PXL_MASK                              0x00000FFF
#define AHB_TO_PS_DATA_PXL_SHIFT                             0



//Bitfield Constants Register AHB_TO_PS_CTRL
#define AHB_TO_PS_CTRL_GO_MASK                               0x00000001
#define AHB_TO_PS_CTRL_GO_SHIFT                              0


//Bitfield Constants Register PGEN_START_CTRL
#define PGEN_START_CTRL_PG_START_MASK                         0xFFFFFFFF
#define PGEN_START_CTRL_PG_START_SHIFT                        0

//Bitfield Constants Register PGEN_STOP_CTRL
#define PGEN_STOP_CTRL_PG_STOP_MASK                          0xFFFFFFFF
#define PGEN_STOP_CTRL_PG_STOP_SHIFT                         0

//Bitfield Constants Register ISPDMA_INPSDMA_CTRL
#define INPSDMA_CTRL_MODE_MASK                             0x000000C0
#define INPSDMA_CTRL_MODE_SHIFT                            6
#define INPSDMA_CTRL_DMABRSTSZ_MASK                        0x00000030
#define INPSDMA_CTRL_DMABRSTSZ_SHIFT                       4
#define INPSDMA_CTRL_BPP_MASK                              0x0000000E
#define INPSDMA_CTRL_BPP_SHIFT                             1
#define INPSDMA_CTRL_DMA_ENA_MASK                          0x00000001
#define INPSDMA_CTRL_DMA_ENA_SHIFT                         0

//Bitfield Constants Register ISPDMA_INPSDMA_SRCADDR
#define INPSDMA_SRCADDR_SRCADDR_MASK                          0xFFFFFFFF
#define INPSDMA_SRCADDR_SRCADDR_SHIFT                         0



//Bitfield Constants Register ISPDMA_INPSDMA_SRCSZ
#define INPSDMA_SRCSZ_SRCSZ_MASK                            0x03FFFFFF
#define INPSDMA_SRCSZ_SRCSZ_SHIFT                           0

//Bitfield Constants Register ISPDMA_INPSDMA_PIXSZ
#define INPSDMA_PIXSZ_VSIZE_MASK                            0x1FFF0000
#define INPSDMA_PIXSZ_VSIZE_SHIFT                           16
#define INPSDMA_PIXSZ_HSIZE_MASK                            0x00003FFF
#define INPSDMA_PIXSZ_HSIZE_SHIFT                           0


//Bitfield Constants Register ISPDMA_INPSDMA_RAMCTRL
#define INPSDMA_RAMCTRL_PDWN_MASK                             0x00000010
#define INPSDMA_RAMCTRL_PDWN_SHIFT                            4
#define INPSDMA_RAMCTRL_WTC_MASK                              0x0000000C
#define INPSDMA_RAMCTRL_WTC_SHIFT                             2
#define INPSDMA_RAMCTRL_RTC_MASK                              0x00000003
#define INPSDMA_RAMCTRL_RTC_SHIFT                             0




//Bitfield Constants Register ISPDMA_INPSDMA_STAT
#define INPSDMA_STAT_EOFSTAT_MASK                          0x00000002
#define INPSDMA_STAT_EOFSTAT_SHIFT                         1
#define INPSDMA_STAT_DMASTAT_MASK                          0x00000001
#define INPSDMA_STAT_DMASTAT_SHIFT                         0



//Bitfield Constants Register ISPDMA_INPSDMA_DBG1
#define INPSDMA_DBG1_W_ADDR_MASK                           0xFFFFFC00
#define INPSDMA_DBG1_W_ADDR_SHIFT                          10
#define INPSDMA_DBG1_W_VLD_MASK                            0x00000200
#define INPSDMA_DBG1_W_VLD_SHIFT                           9
#define INPSDMA_DBG1_W_NBYTES_MASK                         0x000001FF
#define INPSDMA_DBG1_W_NBYTES_SHIFT                        0

//Bitfield Constants Register ISPDMA_INPSDMA_DBG2
#define INPSDMA_DBG2_R_ADDR_MASK                           0xFFFFFC00
#define INPSDMA_DBG2_R_ADDR_SHIFT                          10
#define INPSDMA_DBG2_R_VLD_MASK                            0x00000200
#define INPSDMA_DBG2_R_VLD_SHIFT                           9
#define INPSDMA_DBG2_R_NBYTES_MASK                         0x000001FF
#define INPSDMA_DBG2_R_NBYTES_SHIFT                        0


//Bitfield Constants Register ISP_IRQRAW
#define ISP_IRQRAW_IRQRAW_MASK                           0x00000001
#define ISP_IRQRAW_IRQRAW_SHIFT                          0


//Bitfield Constants Register ISP_IRQMASK
#define ISP_IRQMASK_IRQMASK_MASK                          0x00000001
#define ISP_IRQMASK_IRQMASK_SHIFT                         0


//Bitfield Constants Register ISP_IRQSTAT
#define ISP_IRQSTAT_IRQSTAT_MASK                          0x00000001
#define ISP_IRQSTAT_IRQSTAT_SHIFT                         0

//Bitfield Constants Register ISPDMA_AHBDMA_SDCA
#define AHBDMA_SDCA_SDCA_MASK                             0xFFFFFFFC
#define AHBDMA_SDCA_SDCA_SHIFT                            2

//Bitfield Constants Register ISPDMA_AHBDMA_DCSZ
#define AHBDMA_DCSZ_DCSZ_MASK                             0x00FFFFF8
#define AHBDMA_DCSZ_DCSZ_SHIFT                            3


//Bitfield Constants Register ISPDMA_AHBDMA_CTRL
#define AHBDMA_CTRL_RD_DMABRSTSZ_MASK                     0x000000C0
#define AHBDMA_CTRL_RD_DMABRSTSZ_SHIFT                    6
#define AHBDMA_CTRL_WR_DMABRSTSZ_MASK                     0x00000030
#define AHBDMA_CTRL_WR_DMABRSTSZ_SHIFT                    4
#define AHBDMA_CTRL_DC_ENA_MASK                           0x00000002
#define AHBDMA_CTRL_DC_ENA_SHIFT                          1


//Bitfield Constants Register ISPDMA_AHBDMA_RAMCTRL
#define AHBDMA_RAMCTRL_DCFF_PDFVSSM_MASK                     0x00002000
#define AHBDMA_RAMCTRL_DCFF_PDFVSSM_SHIFT                    13
#define AHBDMA_RAMCTRL_DCFF_PDLVMC_MASK                      0x00001000
#define AHBDMA_RAMCTRL_DCFF_PDLVMC_SHIFT                     12
#define AHBDMA_RAMCTRL_FF_PDFVSSM_MASK                       0x00000800
#define AHBDMA_RAMCTRL_FF_PDFVSSM_SHIFT                      11
#define AHBDMA_RAMCTRL_FF_PDLVMC_MASK                        0x00000400
#define AHBDMA_RAMCTRL_FF_PDLVMC_SHIFT                       10
#define AHBDMA_RAMCTRL_DCFF_PDWN_MASK                        0x00000200
#define AHBDMA_RAMCTRL_DCFF_PDWN_SHIFT                       9
#define AHBDMA_RAMCTRL_DCFF_WTC_MASK                         0x00000180
#define AHBDMA_RAMCTRL_DCFF_WTC_SHIFT                        7
#define AHBDMA_RAMCTRL_DCFF_RTC_MASK                         0x00000060
#define AHBDMA_RAMCTRL_DCFF_RTC_SHIFT                        5
#define AHBDMA_RAMCTRL_FF_PDWN_MASK                          0x00000010
#define AHBDMA_RAMCTRL_FF_PDWN_SHIFT                         4
#define AHBDMA_RAMCTRL_FF_WTC_MASK                           0x0000000C
#define AHBDMA_RAMCTRL_FF_WTC_SHIFT                          2
#define AHBDMA_RAMCTRL_FF_RTC_MASK                           0x00000003
#define AHBDMA_RAMCTRL_FF_RTC_SHIFT                          0


//Bitfield Constants Register ISPDMA_AHBDMA_STAT
#define AHBDMA_STAT_ISP_HRESP_STAT_MASK                   0x00000008
#define AHBDMA_STAT_ISP_HRESP_STAT_SHIFT                  3
#define AHBDMA_STAT_DCDMASTAT_MASK                        0x00000004
#define AHBDMA_STAT_DCDMASTAT_SHIFT                       2
#define AHBDMA_STAT_FF_WR_STAT_MASK                       0x00000002
#define AHBDMA_STAT_FF_WR_STAT_SHIFT                      1
#define AHBDMA_STAT_FF_RD_STAT_MASK                       0x00000001
#define AHBDMA_STAT_FF_RD_STAT_SHIFT                      0


//Bitfield Constants Register ISPDMA_AHBDMA_DGB1
#define AHBDMA_DGB1_W_ADDR_MASK                           0xFFFFFC00
#define AHBDMA_DGB1_W_ADDR_SHIFT                          10
#define AHBDMA_DGB1_W_VLD_MASK                            0x00000200
#define AHBDMA_DGB1_W_VLD_SHIFT                           9
#define AHBDMA_DGB1_W_NBYTES_MASK                         0x000001FF
#define AHBDMA_DGB1_W_NBYTES_SHIFT                        0


//Bitfield Constants Register ISPDMA_AHBDMA_DGB2
#define AHBDMA_DGB2_R_ADDR_MASK                           0xFFFFFC00
#define AHBDMA_DGB2_R_ADDR_SHIFT                          10
#define AHBDMA_DGB2_R_VLD_MASK                            0x00000200
#define AHBDMA_DGB2_R_VLD_SHIFT                           9
#define AHBDMA_DGB2_R_NBYTES_MASK                         0x000001FF
#define AHBDMA_DGB2_R_NBYTES_SHIFT                        0






#endif
