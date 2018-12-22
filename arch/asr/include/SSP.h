#ifndef __SSP_H__
#define __SSP_H__

typedef struct{
    __IO uint32_t SSCR0;    /* 0x0000 SSP Control 0 Register */                   
    __IO uint32_t SSCR1;    /* 0x0004 SSP Control 1 Register */                   
    __IO uint32_t SSSR;     /* 0x0008 SSP Status Register */                      
    __IO uint32_t SSITR;    /* 0x000C SSP Interrupt Test Register */              
    __IO uint32_t SSDDR;    /* 0x0010 SSP Data Register */   
    
    __IO uint32_t RESERVED0[5];                     
    
    __IO uint32_t SSTO;     /* 0x0028 SSP Time Out Register */                    
    __IO uint32_t SSPSP;    /* 0x002C SSP Programmable Serial Protocol Register */
    __IO uint32_t SSTSA;    /* 0x0030 SSP TX Time Slot Active Register */         
    __IO uint32_t SSRSA;    /* 0x0034 SSP RX Time Slot Active Register */         
    __I  uint32_t SSTSS;    /* 0x0038 SSP Time Slot Status Register */    
    __IO uint32_t SSACD;
    __IO uint32_t SSACDD;


    __IO uint32_t RESERVED1[(0x080-0x040)/4-1];
    __IO uint32_t GPS_ID;//0X0080
    __IO uint32_t GPS_CSR;
    __IO uint32_t GPS_TSCNT;
    __IO uint32_t GPS_TSCNT_ADJ;//0X08C
    __IO uint32_t RESERVED2[(0x0C0-0x08C)/4-1];
    __IO uint32_t SPI_MOD;//0X0C0
}SSP_TypeDef;

#define    SSP0_BASE      0xD401B000
#define    SSP0  (( SSP_TypeDef *) SSP0_BASE )

#define    SSP1_BASE      0xD42A0C00
#define    SSP1  (( SSP_TypeDef *) SSP1_BASE )

#define    SSP2_BASE      0xD401C000
#define    SSP2  (( SSP_TypeDef *) SSP2_BASE )

#define    GSSP_BASE      0xD4039000
#define    GSSP  (( SSP_TypeDef *) GSSP_BASE ) 

/*
 *
 *	THE BIT DEFINES
 *
 */
/*	SSP_SSCR0			0x0000	SSP Control Register 0 */
#define	SSP_SSCR0_MOD					BIT_31			/* Mode */
#define	SSP_SSCR0_ACS					BIT_30
#define	SSP_SSCR0_FPCKE					BIT_29			/* FIFO Packing Enable */
#define	SSP_SSCR0_CLEARALWAYS					BIT_28
#define	SSP_SSCR0_MM52					BIT_27
/* Frame Rate Divider Control */
#define	SSP_SSCR0_FRDC_MSK				SHIFT24(0x7)	
#define	SSP_SSCR0_FRDC_BASE				24
/* Transmit FIFO Underrun Interrupt Mask */
#define	SSP_SSCR0_TIM					BIT_23			
/* Receive FIFO Overrun Interrupt Mask */
#define	SSP_SSCR0_RIM					BIT_22			
#define	SSP_SSCR0_NCS					BIT_21
/* Extended Data Size Select */
#define	SSP_SSCR0_EDSS					BIT_20			
#define	SSP_SSCR0_SCR_MSK				SHIFT8(0xfff)	
#define	SSP_SSCR0_SCR_BASE				8
/* Synchronous Serial Port Enable */
#define	SSP_SSCR0_SSE					BIT_7			
#define	SSP_SSCR0_ECS					BIT_6
#define	SSP_SSCR0_FRF_MSK				SHIFT4(0x3)		/* Frame Format */
#define	SSP_SSCR0_FRF_BASE				4
#define	SSP_SSCR0_DSS_MSK				SHIFT0(0xf)		/* Data Size Select */
#define	SSP_SSCR0_DSS_BASE				0

/*	SSP_SSCR1			0x0004	SSP Control Register 1 */
/* TXD Three-state Enable On Last Phase */
#define	SSP_SSCR1_TTELP			BIT_31			
#define	SSP_SSCR1_TTE			BIT_30			/* TXD Three-State Enable */
#define	SSP_SSCR1_EBCEI			BIT_29			/* Enable Bit Count Error Interrupt */
#define	SSP_SSCR1_SCFR			BIT_28			/* Slave Clock Free Running */
#define	SSP_SSCR1_ECRA			BIT_27			/* Enable Clock Request A */
#define	SSP_SSCR1_ECRB			BIT_26			/* Enable Clock Request B */
/* SSP Serial Bit Rate Clock (SSPSCLKx) Direction */
#define	SSP_SSCR1_SCLKDIR		BIT_25			
#define	SSP_SSCR1_SFRMDIR		BIT_24			/* SSP Frame (SSPSFRMx) Direction */
#define	SSP_SSCR1_RWOT			BIT_23			/* Receive Without Transmit */
#define	SSP_SSCR1_TRAIL			BIT_22			/* Trailing Byte */
#define	SSP_SSCR1_TSRE			BIT_21			/* Transmit Service Request Enable */
#define	SSP_SSCR1_RSRE			BIT_20			/* Receive Service Request Enable */
/* Receiver Time-out Interrupt Enable */
#define	SSP_SSCR1_TINTE			BIT_19			
/* Peripheral Trailing Byte Interrupt Enable */
#define	SSP_SSCR1_PINTE			BIT_18			
/*		Bit(s) SSP_SSCR1_RSRV_17 reserved */
#define	SSP_SSCR1_IFS			BIT_16			/* Invert Frame Signal */
#define	SSP_SSCR1_STRF			BIT_15			/* Select FIFO For Efwr */
#define	SSP_SSCR1_EFWR			BIT_14			/* Enable FIFO Write/read */
#define	SSP_SSCR1_RFT_MSK		SHIFT10(0xf)	/* RXFIFO Trigger Threshold */
#define	SSP_SSCR1_RFT_BASE		10
#define	SSP_SSCR1_TFT_MSK		SHIFT6(0xf)		/* TXFIFO Trigger Threshold */
#define	SSP_SSCR1_TFT_BASE		6
/* Motorola SPI SSPSCLK phase setting */
#define	SSP_SSCR1_SPH			BIT_4			
/* Motorola SPI SSPSCLK Polarity Setting */
#define	SSP_SSCR1_SPO			BIT_3			
#define	SSP_SSCR1_LBM			BIT_2			/* Loopback Mode */
#define	SSP_SSCR1_TIE			BIT_1			/* Transmit FIFO Interrupt Enable */
#define	SSP_SSCR1_RIE			BIT_0			/* Receive FIFO Interrupt Enable */

/*	SSP_SSSR			0x0008	SSP Status Register */
#define	SSP_SSSR_OSS					BIT_31			/* Odd Sample Status */
/* TX FIFO Odd Sample Status */
#define	SSP_SSSR_TX_OSS					BIT_30			
/*		Bit(s) SSP_SSSR_RSRV_29_24 reserved */
#define	SSP_SSSR_BCE					BIT_23			/* Bit Count Error */
/* Clock Synchronization Status */
#define	SSP_SSSR_CSS					BIT_22			
#define	SSP_SSSR_TUR					BIT_21			/* Transmit FIFO Underrun */
#define	SSP_SSSR_EOC					BIT_20			/* End Of Chain */
/* Receiver Time-out Interrupt */
#define	SSP_SSSR_TINT					BIT_19			
/* Peripheral Trailing Byte Interrupt */
#define	SSP_SSSR_PINT					BIT_18			
/*		Bit(s) SSP_SSSR_RSRV_17_16 reserved */
#define	SSP_SSSR_RFL_MSK				SHIFT12(0xf)	/* Receive FIFO Level */
#define	SSP_SSSR_RFL_BASE				12
#define	SSP_SSSR_TFL_MSK				SHIFT8(0xf)		/* Transmit FIFO Level */
#define	SSP_SSSR_TFL_BASE				8
#define	SSP_SSSR_ROR					BIT_7			/* Receive FIFO Overrun */
/* Receive FIFO Service Request */
#define	SSP_SSSR_RFS					BIT_6			
/* Transmit FIFO Service Request */
#define	SSP_SSSR_TFS					BIT_5			
#define	SSP_SSSR_BSY					BIT_4			/* SSP Busy */
#define	SSP_SSSR_RNE					BIT_3			/* Receive FIFO Not Empty */
#define	SSP_SSSR_TNF					BIT_2			/* Transmit FIFO Not Full */
/*		Bit(s) SSP_SSSR_RSRV_1_0 reserved */

/*	SSP_SSITR			0x000C	SSP Interrupt Test Register */
/*		Bit(s) SSP_SSITR_RSRV_31_8 reserved */
#define	SSP_SSITR_TROR					BIT_7				/* Test RXFIFO Overrun */
/* Test RXFIFO Service Request */
#define	SSP_SSITR_TRFS					BIT_6				
/* Test TXFIFO Service Request */
#define	SSP_SSITR_TTFS					BIT_5				
/*		Bit(s) SSP_SSITR_RSRV_4_0 reserved */



/*	SSP_SSPSP			0x002C	SSP Programmable Serial Protocol Register */
/*		Bit(s) SSP_SSPSP_RSRV_31 reserved */
#define	SSP_SSPSP_EDMYSTOP_MSK		SHIFT28(0x7)	/* Extended Dummy Stop */
#define	SSP_SSPSP_EDMYSTOP_BASE		28
#define	SSP_SSPSP_EDMYSTRT_MSK		SHIFT26(0x3)	/* Extended Dummy Start */
#define	SSP_SSPSP_EDMYSTRT_BASE		26
/* Frame Sync Relative Timing Bit */
#define	SSP_SSPSP_FSRT				BIT_25			
#define	SSP_SSPSP_DMYSTOP_MSK		SHIFT23(0x3)	/* Dummy Stop */
#define	SSP_SSPSP_DMYSTOP_BASE		23
/*		Bit(s) SSP_SSPSP_RSRV_22 reserved */
#define	SSP_SSPSP_SFRMWDTH_MSK		SHIFT16(0x3f)	/* Serial Frame Width */
#define	SSP_SSPSP_SFRMWDTH_BASE		16
#define	SSP_SSPSP_SFRMDLY_MSK		SHIFT9(0x7f)	/* Serial Frame Delay */
#define	SSP_SSPSP_SFRMDLY_BASE		9
#define	SSP_SSPSP_DMYSTRT_MSK		SHIFT7(0x3)		/* Dummy Start */
#define	SSP_SSPSP_DMYSTRT_BASE		7
#define	SSP_SSPSP_STRTDLY_MSK		SHIFT4(0x7)		/* Start Delay */
#define	SSP_SSPSP_STRTDLY_BASE		4
#define	SSP_SSPSP_ETDS				BIT_3			/* End Of Transfer Data State */
#define	SSP_SSPSP_SFRMP				BIT_2			/* Serial Frame Polarity */
#define	SSP_SSPSP_SCMODE_MSK		SHIFT0(0x3)		/* Serial Bit-rate Clock Mode */
#define	SSP_SSPSP_SCMODE_BASE		0

/* SSP Time Out Registers (SSTO_X) */
#define SSP_SSTO_TIMEOUT_MSK    SHIFT0(0xffffff)
#define SSP_SSTO_TIMEOUT_BASE   0

/*	SSP_SSTSA			0x0030	SSP TX Time Slot Active Register */
/*		Bit(s) SSP_SSTSA_RSRV_31_8 reserved */
#define	SSP_SSTSA_TTSA_MSK				SHIFT0(0xff)		/* TX Time Slot Active */
#define	SSP_SSTSA_TTSA_BASE				0

/*	SSP_SSRSA			0x0034	SSP RX Time Slot Active Register */
/*		Bit(s) SSP_SSRSA_RSRV_31_8 reserved */
#define	SSP_SSRSA_RTSA_MSK				SHIFT0(0xff)		/* RX Time Slot Active */
#define	SSP_SSRSA_RTSA_BASE				0

/*	SSP_SSTSS			0x0038	SSP Time Slot Status Register */
#define	SSP_SSTSS_NMBSY					BIT_31				/* Network Mode Busy */
/*		Bit(s) SSP_SSTSS_RSRV_30_3 reserved */
#define	SSP_SSTSS_TSS_MSK				SHIFT0(0x7)			/* Time Slot Status */
#define	SSP_SSTSS_TSS_BASE				0

/* SSP Audio Clock Divider Registers (SSACD_X) */
#define	SSP_SSACD_SCDX8     BIT_7
#define	SSP_SSACD_ACPS_MSK  SHIFT4(0x7)
#define	SSP_SSACD_ACPS_BASE 4
#define	SSP_SSACD_SCDB     BIT_3
#define	SSP_SSACD_ACDS_MSK  SHIFT4(0x7)
#define	SSP_SSACD_ACDS_BASE 0

/* SSP Audio Clock Dither Divider Registers (SSACDD_X) */
#define	SSP_SSACDD_NUM_MSK  SHIFT16(0x1f)
#define	SSP_SSACDD_NUM_BASE 16
#define	SSP_SSACDD_DEN_MSK  SHIFT4(0xffff)
#define	SSP_SSACDD_DEN_BASE 0








//Bitfield Constants Register SSP_GPS_CSR
#define GPS_CSR_LSB_1ST_MASK                          0x00001000
#define GPS_CSR_LSB_1ST_SHIFT                         12
#define GPS_CSR_SERCLK_FEDGE_MASK                     0x00000800
#define GPS_CSR_SERCLK_FEDGE_SHIFT                    11
#define GPS_CSR_FS_SEL1_0_MASK                        0x00000600
#define GPS_CSR_FS_SEL1_0_SHIFT                       9
#define GPS_CSR_MAX_STRMBITS_MASK                     0x00000180
#define GPS_CSR_MAX_STRMBITS_SHIFT                    7
#define GPS_CSR_GPS_TYPE_MASK                         0x00000070
#define GPS_CSR_GPS_TYPE_SHIFT                        4
#define GPS_CSR_TSYNC_COUNT_RST_MASK                  0x00000008
#define GPS_CSR_TSYNC_COUNT_RST_SHIFT                 3
#define GPS_CSR_SOFT_RESET_MASK                       0x00000004
#define GPS_CSR_SOFT_RESET_SHIFT                      2
#define GPS_CSR_GPS_ON_MASK                           0x00000002
#define GPS_CSR_GPS_ON_SHIFT                          1
#define GPS_CSR_GPS_MODE_MASK                         0x00000001
#define GPS_CSR_GPS_MODE_SHIFT                        0

//Bitfield Constants Register SSP_SPI_MOD
#define SPI_MOD_SPI_MOD_MASK                          0x00000001
#define SPI_MOD_SPI_MOD_SHIFT                         0







#endif
