#ifndef __USB_PHY_H__
#define __USB_PHY_H__

typedef struct
{
       __IO uint32_t	PLLCONTROL0;					/* 0x0000 32 bit	USB2 PHY PLL Register */
	__IO uint32_t	PLLCONTROL1;					/* 0x0004 32 bit	PLLCONTROLregister1 */
	__IO uint32_t	CALIBRATIONCONTROL;			/* 0x0008 32 bit	calibration control
										 *			register */
      __IO uint32_t	USB2_TX_REG0;					/* 0x000c 32 bit	TX channel control
										 *			register0
										 */									
	__IO uint32_t	RESERVED0[1];

	__IO uint32_t	USB2_RX_REG0;					/* 0x0014 32 bit	RX channel control
										 *			register0
										 */
	__IO uint32_t	USB2_RX_REG1;					/* 0x0018 32 bit	RX channel control
										 *			register1
										 */
	__IO uint32_t	USB2_DIG_REG0;					/* 0x001c 32 bit	digital control
										 *			register 0 
										 */
	__IO uint32_t	USB2_DIG_REG1;					/* 0x0020 32 bit	digital control
										 *			register 1 
										 */
	__IO uint32_t	USB2_TEST_REG0;					/* 0x0024 32 bit	usb2 test control
										 *			register 0 
										 */
	__IO uint32_t	USB2_TEST_REG1;					/* 0x0028 32 bit	usb2 test control
										 *			register 1 
										 */
	__IO uint32_t	RESERVED;						/* 0x002c 32 bit	Reserved */
	__IO uint32_t	RESERVED1[1];
	__IO uint32_t	USB2_OTG_REG;						/* 0x0034 32 bit	usb2otg control register */
	__IO uint32_t	USB2_CHGDTC;					/* 0x0038 32 bit	usb2charger detector
										 *			control register
										 */
	__IO uint32_t	RESERVED2[33];
	__IO uint32_t	IP_REVISION;					/* 0x00c0 32 bit	ip_revision */
	__IO uint32_t	USB_CTL;						/* 0x00c4 32 bit	usb_ctl */
	__IO uint32_t	USB_INT_MISC;					/* 0x00c8 32 bit	usb_int_misc */
	__IO uint32_t	DBG_CTL;						/* 0x00cc 32 bit	dbg_ctl */
	__IO uint32_t	A_PHY1;							/* 0x00d0 32 bit	a_phy1 */
	__IO uint32_t	A_PHY2;							/* 0x00d4 32 bit	a_phy2 */                      
}MAVL_USB_PHY_TypeDef;

#define    USB_PHY_BASE                          0xD4207000                         
#define    MAVL_USB_PHY  (( MAVL_USB_PHY_TypeDef *) USB_PHY_BASE )

/*
 *
 *	THE BIT DEFINES
 *
 */
/*	PLLCONTROL0			0x0000	USB2 PHY PLL Register */
#define	PLLCONTROL0_PLL_READY			BIT_31			/* pll_ready */
#define	PLLCONTROL0_R_ROTATE			BIT_30			/* R_ROTATE */
#define	PLLCONTROL0_SEL_LPFR_MSK		SHIFT28(0x3)	/* SEL_LPFR */
#define	PLLCONTROL0_SEL_LPFR_BASE		28
/*		Bit(s) PLLCONTROL0_RSRV_ reserved */
#define	PLLCONTROL0_FBDIV_MSK			SHIFT16(0x1ff)	/* FBDIV */
#define	PLLCONTROL0_FBDIV_BASE			16
#define	PLLCONTROL0_VDDL_MSK			SHIFT14(0x3)	/* VDDL */
#define	PLLCONTROL0_VDDL_BASE			14
#define	PLLCONTROL0_VDDM_MSK			SHIFT12(0x3)	/* VDDM */
#define	PLLCONTROL0_VDDM_BASE			12
#define	PLLCONTROL0_ICP_MSK			SHIFT8(0xf)		/* ICP */
#define	PLLCONTROL0_ICP_BASE			8
/*		Bit(s) PLLCONTROL0_RSRV_ reserved */
#define	PLLCONTROL0_REFDIV_MSK		SHIFT0(0x7f)	/* REFDIV */
#define	PLLCONTROL0_REFDIV_BASE			0

/*	PLLCONTROL1			0x0004	PLLCONTROLregister1 */
/*		Bit(s) PLLCONTROL1_RSRV_ reserved */
#define	PLLCONTROL1_PLL_STRESS_TEST		BIT_14				/* PLL_STRESS_TEST */
#define	PLLCONTROL1_CLK_BLK_EN			BIT_13				/* CLK_BLK_EN */
#define	PLLCONTROL1_REFCLK_SEL			BIT_12				/* REFCLK_SEL */
#define	PLLCONTROL1_CLK160M_EN			BIT_11				/* CLK160M_EN */
#define	PLLCONTROL1_TXCLK_EN				BIT_10				/* TXCLK_EN */
/*		Bit(s) PLLCONTROL1_RSRV_ reserved */
/*		Bit(s) PLLCONTROL1_RSRV_ reserved */
#define	PLLCONTROL1_TESTMON_PLL_MSK			SHIFT8(0xf)		/* ICP */
#define	PLLCONTROL1_TESTMON_PLL_BASE			8
#define	PLLCONTROL1_PLL_SUSPEND_EN		BIT_3				/* PLL_SUSPEND_EN */
#define	PLLCONTROL1_PLL_RESET				BIT_2				/* PLL_RESET */
#define	PLLCONTROL1_PU_PLL_BY_REG			BIT_1				/* PU_PLL_BY_REG */
#define	PLLCONTROL1_PU_PLL				BIT_0				/* PU_PLL */

/*	CALIBRATIONCONTROL1	0x0008	CALIBRATIONCONTROLregister */
#define	CALIBRATIONCONTROL1_PLLCAL_DONE			BIT_31			/* PLLCAL_DONE */
#define	CALIBRATIONCONTROL1_CURRENT_KVCO_MSK		SHIFT28(0x7)	/* CURRENT_KVCO */
#define	CALIBRATIONCONTROL1_CURRENT_KVCO_BASE		28
#define	CALIBRATIONCONTROL1_CURRENT_IMP_MSK		SHIFT24(0xf)	/* CURRENT_IMP */
#define	CALIBRATIONCONTROL1_CURRENT_IMP_BASE		24
#define	CALIBRATIONCONTROL1_IMPCAL_DONE			BIT_23			/* IMPCAL_DONE */
#define	CALIBRATIONCONTROL1_PLLCAL_START			BIT_22			/* PLLCAL_START */
#define	CALIBRATIONCONTROL1_PLLCAL_MSK			SHIFT20(0x3)	/* PLLCAL */
#define	CALIBRATIONCONTROL1_PLLCAL_BASE				20
#define	CALIBRATIONCONTROL1_KVCO_EXT				BIT_19			/* KVCO_EXT */
#define	CALIBRATIONCONTROL1_KVCO_MSK				SHIFT16(0x7)	/* KVCO */
#define	CALIBRATIONCONTROL1_KVCO_BASE				16
#define	CALIBRATIONCONTROL1_VCOCAL_POL			BIT_15			/* VCOCAL_POL */
#define	CALIBRATIONCONTROL1_IMPCAL_POL			BIT_14			/* IMPCAL_POL */
#define	CALIBRATIONCONTROL1_IMPCAL_START			BIT_13			/* IMPCAL_START */
#define	CALIBRATIONCONTROL1_EXT_RCAL_EN			BIT_12			/* EXT_RCAL_EN */
/* EXT_FS_IMP_EN */
#define	CALIBRATIONCONTROL1_EXT_FS_IMP_EN			BIT_11			
#define	CALIBRATIONCONTROL1_IMPCAL_VTH_MSK		SHIFT8(0x7)		/* IMPCAL_VTH */
#define	CALIBRATIONCONTROL1_IMPCAL_VTH_BASE			8
#define	CALIBRATIONCONTROL1_EXT_RCAL_MSK			SHIFT4(0xf)		/* EXT_RCAL */
#define	CALIBRATIONCONTROL1_EXT_RCAL_BASE			4
#define	CALIBRATIONCONTROL1_EXT_FS_IMP_MSK		SHIFT0(0xf)		/* EXT_FS_IMP */
#define	CALIBRATIONCONTROL1_EXT_FS_IMP_BASE			0

/*	usb2_tx_reg0			0x0010	TX channel control register0 */
/*		Bit(s) USB2_TX_REG0_RSRV_ reserved */
#define	USB2_TX_REG0_PU_BY_REG				BIT_25			/* PU_BY_REG */
#define	USB2_TX_REG0_PU_ANA					BIT_24			/* PU_ANA */
#define	USB2_TX_REG0_PU_VDDR18				BIT_23			/* PU_VDDR18 */
#define	USB2_TX_REG0_AMP_MSK				SHIFT20(0x7)	/* AMP */
#define	USB2_TX_REG0_AMP_BASE				20
#define	USB2_TX_REG0_IMP_SEL_LS_MSK			SHIFT16(0xf)	/* IMP_SEL_LS */
#define	USB2_TX_REG0_IMP_SEL_LS_BASE		16
#define	USB2_TX_REG0_DRV_EN_LS_MSK			SHIFT12(0xf)	/* DRV_EN_LS */
#define	USB2_TX_REG0_DRV_EN_LS_BASE			12
#define	USB2_TX_REG0_FSDRV_EN_MSK			SHIFT8(0xf)		/* FSDRV_EN */
#define	USB2_TX_REG0_FSDRV_EN_BASE			8
/*		Bit(s) USB2_TX_REG0_RSRV_ reserved */
#define	USB2_TX_REG0_LOWVDD_EN				BIT_6			/* LOWVDD_EN */
#define	USB2_TX_REG0_HS_SR_SEL_MSK			SHIFT4(0x3)		/* HS_SR_SEL */
#define	USB2_TX_REG0_HS_SR_SEL_BASE			4
#define	USB2_TX_REG0_HSDRV_EN_MSK			SHIFT0(0xf)		/* HSDRV_EN */
#define	USB2_TX_REG0_HSDRV_EN_BASE			0

/*	usb2_tx_reg1			0x0010	TX channel control register1 */
/*		Bit(s) USB2_TX_REG1_RSRV_ reserved */

/*	usb2_rx_reg0			0x0014	RX channel control register0 */
/*		Bit(s) USB2_RX_REG0_RSRV_ reserved */
#define	USB2_RX_REG0_CHIRPMODE_SEL			BIT_29			/* CHIRPMODE_SEL */
#define	USB2_RX_REG0_SQ_ANA_DTC_SEL			BIT_28			/* SQ_ANA_DTC_SEL */
#define	USB2_RX_REG0_SQ_ANA_VREF_MSK		SHIFT26(0x3)	/* SQ_ANA_VREF */
#define	USB2_RX_REG0_SQ_ANA_VREF_BASE		26
#define	USB2_RX_REG0_RESET_EXT				BIT_25			/* RESET_EXT */
#define	USB2_RX_REG0_RESET_EXT_EN			BIT_24			/* RESET_EXT_EN */
/*		Bit(s) USB2_RX_REG0_RSRV_ reserved */
#define	USB2_RX_REG0_STRESS_TEST_MODE		BIT_19			/* STRESS_TEST_MODE */
/*		Bit(s) USB2_RX_REG0_RSRV_ reserved */
#define	USB2_RX_REG0_SQ_VHL_SEL				BIT_16			/* SQ_VHL_SEL */
#define	USB2_RX_REG0_SQ_DET_EN				BIT_15			/* SQ_DET_EN */
#define	USB2_RX_REG0_SQ_CM_SEL				BIT_14			/* SQ_CM_SEL */
#define	USB2_RX_REG0_SQ_DLY_SEL_MSK			SHIFT12(0x3)	/* SQ_DLY_SEL */
#define	USB2_RX_REG0_SQ_DLY_SEL_BASE		12
/*		Bit(s) USB2_RX_REG0_RSRV_ reserved */
#define	USB2_RX_REG0_LINESTATE_EN			BIT_10			/* LINESTATE_EN */
#define	USB2_RX_REG0_DISCON_THRESH_MSK		SHIFT8(0x3)		/* DISCON_THRESH */
#define	USB2_RX_REG0_DISCON_THRESH_BASE		8
#define	USB2_RX_REG0_RXVDDL_MSK				SHIFT6(0x3)		/* RXVDDL */
#define	USB2_RX_REG0_RXVDDL_BASE			6
#define	USB2_RX_REG0_RXVDD18_MSK			SHIFT4(0x3)		/* RXVDD18 */
#define	USB2_RX_REG0_RXVDD18_BASE			4
#define	USB2_RX_REG0_SQ_THRESH_MSK			SHIFT0(0xf)		/* SQ_THRESH */
#define	USB2_RX_REG0_SQ_THRESH_BASE			0

/*	usb2_rx_reg1			0x0018	RX channel control register1 */
/* REG_SQCAL_DONE */
#define	USB2_RX_REG1_REG_SQCAL_DONE				BIT_31				
/*		Bit(s) USB2_RX_REG1_RSRV_ reserved */
/* SQ_DPDM_AMP_SEL */
#define	USB2_RX_REG1_SQ_DPDM_AMP_SEL_MSK		SHIFT6(0x3)			
#define	USB2_RX_REG1_SQ_DPDM_AMP_SEL_BASE		6
/* REG_SQ_UD_SWAP */
#define	USB2_RX_REG1_REG_SQ_UD_SWAP				BIT_5				
/* REG_SQCAL_START */
#define	USB2_RX_REG1_REG_SQCAL_START			BIT_4				
/* EXT_SQ_AMP_CAL_EN */
#define	USB2_RX_REG1_EXT_SQ_AMP_CAL_EN			BIT_3				
/* EXT_SQ_AMP_CAL */
#define	USB2_RX_REG1_EXT_SQ_AMP_CAL_MSK			SHIFT0(0x7)			
#define	USB2_RX_REG1_EXT_SQ_AMP_CAL_BASE		0

/*	usb2_dig_reg0			0x001c	digital control register 0 */
/* BITSTAFFING_ERROR */
#define	USB2_DIG_REG0_BITSTAFFING_ERROR			BIT_31			
/* LOSS_OF_SYNC_ERROR */
#define	USB2_DIG_REG0_LOSS_OF_SYNC_ERROR		BIT_30			
#define	USB2_DIG_REG0_FIFO_OVF_ERROR			BIT_29			/* FIFO_OVF_ERROR */
/*		Bit(s) USB2_DIG_REG0_RSRV_ reserved */
#define	USB2_DIG_REG0_CORE_UTMI_SEL				BIT_26			/* CORE_UTMI_SEL */
#define	USB2_DIG_REG0_SE1_FILT_SEL				BIT_25			/* SE1_FILT_SEL */
#define	USB2_DIG_REG0_SE0_FILT_SEL				BIT_24			/* SE0_FILT_SEL */
#define	USB2_DIG_REG0_EDGE_OPT_EN				BIT_23			/* EDGE_OPT_EN */
#define	USB2_DIG_REG0_STOP_AT_RXERR				BIT_22			/* STOP_AT_RXERR */
#define	USB2_DIG_REG0_RX_RUNAWAY_EN				BIT_21			/* RX_RUNAWAY_EN */
#define	USB2_DIG_REG0_LOSSOFSYNC_EN				BIT_20			/* LOSSOFSYNC_EN */
#define	USB2_DIG_REG0_FIFOOVF_EN				BIT_19			/* FIFOOVF_EN */
#define	USB2_DIG_REG0_SQ_FILT_MSK				SHIFT16(0x7)	/* SQ_FILT */
#define	USB2_DIG_REG0_SQ_FILT_BASE				16
/*		Bit(s) USB2_DIG_REG0_RSRV_ reserved */
#define	USB2_DIG_REG0_SQ_BLK_MSK				SHIFT12(0x7)	/* SQ_BLK */
#define	USB2_DIG_REG0_SQ_BLK_BASE				12
#define	USB2_DIG_REG0_EARLY_TX_ENABLE			BIT_11			/* EARLY_TX_ENABLE */
#define	USB2_DIG_REG0_RXFILT2_EN				BIT_10			/* RXFILT2_EN */
#define	USB2_DIG_REG0_RXFILT1_EN				BIT_9			/* RXFILT1_EN */
#define	USB2_DIG_REG0_LONG_EOP					BIT_8			/* LONG_EOP */
#define	USB2_DIG_REG0_PLL_LOCK_BYPASS			BIT_7			/* PLL_LOCK_BYPASS */
#define	USB2_DIG_REG0_CLK_SUSPEND_EN			BIT_6			/* CLK_SUSPEND_EN */
#define	USB2_DIG_REG0_HOST_DISCON_SEL1			BIT_5			/* HOST_DISCON_SEL1 */
#define	USB2_DIG_REG0_HOST_DISCON_SEL0			BIT_4			/* HOST_DISCON_SEL0 */
/* SYNCDET_WINDOW_EN */
#define	USB2_DIG_REG0_SYNCDET_WINDOW_EN			BIT_3			
#define	USB2_DIG_REG0_SYNCDET_WIN32				BIT_2			/* SYNCDET_WIN32 */
#define	USB2_DIG_REG0_SYNC_NUM_MSK				SHIFT0(0x3)		/* SYNC_NUM */
#define	USB2_DIG_REG0_SYNC_NUM_BASE				0

/*	usb2_dig_reg1			0x0020	digital control register 1 */
/*		Bit(s) USB2_DIG_REG1_RSRV_ reserved */
/* REG_LSIDLE_PRE */
#define	USB2_DIG_REG1_REG_LSIDLE_PRE				BIT_30			
/* REG_CLK_OUT_DLY */
#define	USB2_DIG_REG1_REG_CLK_OUT_DLY_MSK			SHIFT22(0xff)	
#define	USB2_DIG_REG1_REG_CLK_OUT_DLY_BASE			22
/* REG_SAMPLER_PREOFF */
#define	USB2_DIG_REG1_REG_SAMPLER_PREOFF			BIT_21			
/* REG_SAMPLER_ON */
#define	USB2_DIG_REG1_REG_SAMPLER_ON				BIT_20			
/* REG_SAMPLEROFF_DLY */
#define	USB2_DIG_REG1_REG_SAMPLEROFF_DLY_MSK		SHIFT16(0xf)	
#define	USB2_DIG_REG1_REG_SAMPLEROFF_DLY_BASE		16
/* REG_SWAP_DPDM */
#define	USB2_DIG_REG1_REG_SWAP_DPDM					BIT_15			
#define	USB2_DIG_REG1_REG_LAT_UTMI					BIT_14			/* REG_LAT_UTMI */
/* REG_OPMODE_SEL */
#define	USB2_DIG_REG1_REG_OPMODE_SEL				BIT_13			
/* REG_FLSMODELP_EN */
#define	USB2_DIG_REG1_REG_FLSMODELP_EN				BIT_12			
/* REG_FREERUNCLK_EN */
#define	USB2_DIG_REG1_REG_FREERUNCLK_EN				BIT_11			
/* REG_NOVBUS_DPDM00 */
#define	USB2_DIG_REG1_REG_NOVBUS_DPDM00				BIT_10			
/*		Bit(s) USB2_DIG_REG1_RSRV_ reserved */
/* REG_FS_EOP_MODE */
#define	USB2_DIG_REG1_REG_FS_EOP_MODE				BIT_7			
/* REG_DP_PULLDOWN */
#define	USB2_DIG_REG1_REG_DP_PULLDOWN				BIT_6			
/* REG_DM_PULLDOWN */
#define	USB2_DIG_REG1_REG_DM_PULLDOWN				BIT_5			
/* REG_ARC_DPDM_MODE */
#define	USB2_DIG_REG1_REG_ARC_DPDM_MODE				BIT_4			
/* REG_FS_RX_ERROR_MODE2 */
#define	USB2_DIG_REG1_REG_FS_RX_ERROR_MODE2			BIT_3			
/* REG_FS_RX_ERROR_MODE1 */
#define	USB2_DIG_REG1_REG_FS_RX_ERROR_MODE1			BIT_2			
/* REG_FS_RX_ERROR_MODE */
#define	USB2_DIG_REG1_REG_FS_RX_ERROR_MODE			BIT_1			
/* REG_FORCE_END_EN */
#define	USB2_DIG_REG1_REG_FORCE_END_EN				BIT_0			

/*	usb2_test_reg0			0x0024	usb2 test control register 0 */
/* TEST_FAIL */
#define	USB2_TEST_REG0_TEST_FAIL						BIT_31			
/* TEST_DONE */
#define	USB2_TEST_REG0_TEST_DONE						BIT_30			
/* REG_TEST_EN */
#define	USB2_TEST_REG0_REG_TEST_EN						BIT_29			
/* REG_TEST_DIG_LPBK */
#define	USB2_TEST_REG0_REG_TEST_DIG_LPBK				BIT_28			
/* REG_TEST_ANA_LPBK */
#define	USB2_TEST_REG0_REG_TEST_ANA_LPBK				BIT_27			
/* REG_TEST_BYPASS */
#define	USB2_TEST_REG0_REG_TEST_BYPASS					BIT_26			
/* REG_TEST_UTMI_SEL */
#define	USB2_TEST_REG0_REG_TEST_UTMI_SEL				BIT_25			
/*		Bit(s) USB2_TEST_REG0_RSRV_ reserved */
/* REG_TEST_TX_BITSTUFF_EN */
#define	USB2_TEST_REG0_REG_TEST_TX_BITSTUFF_EN			BIT_23			
/* REG_TEST_SUSPENDM */
#define	USB2_TEST_REG0_REG_TEST_SUSPENDM				BIT_22			
/* REG_TEST_RESET */
#define	USB2_TEST_REG0_REG_TEST_RESET					BIT_21			
/* REG_TEST_TERM_SELECT */
#define	USB2_TEST_REG0_REG_TEST_TERM_SELECT				BIT_20			
/* REG_TEST_XVCR_SELECT */
#define	USB2_TEST_REG0_REG_TEST_XVCR_SELECT_MSK			SHIFT18(0x3)	
#define	USB2_TEST_REG0_REG_TEST_XVCR_SELECT_BASE		18
/* REG_TEST_OP_MODE */
#define	USB2_TEST_REG0_REG_TEST_OP_MODE_MSK				SHIFT16(0x3)	
#define	USB2_TEST_REG0_REG_TEST_OP_MODE_BASE			16
/*		Bit(s) USB2_TEST_REG0_RSRV_ reserved */
/* REG_TEST_SKIP */
#define	USB2_TEST_REG0_REG_TEST_SKIP_MSK				SHIFT12(0x7)	
#define	USB2_TEST_REG0_REG_TEST_SKIP_BASE				12
/*		Bit(s) USB2_TEST_REG0_RSRV_ reserved */
/* REG_TEST_MODE */
#define	USB2_TEST_REG0_REG_TEST_MODE_MSK				SHIFT8(0x3)		
#define	USB2_TEST_REG0_REG_TEST_MODE_BASE				8
/* REG_TEST_TX_PATTERN */
#define	USB2_TEST_REG0_REG_TEST_TX_PATTERN_MSK			SHIFT0(0xff)	
#define	USB2_TEST_REG0_REG_TEST_TX_PATTERN_BASE			0

/*	usb2_test_reg1			0x0028	usb2 test control register 1 */
/*		Bit(s) USB2_TEST_REG1_RSRV_ reserved */
#define	USB2_TEST_REG1_TEST_LEN_MSK			SHIFT0(0x7ff)		/* TEST_LEN */
#define	USB2_TEST_REG1_TEST_LEN_BASE		0

/*	Reserved				0x002c	Reserved */
/*		Bit(s) RESERVED_RSRV_ reserved */
/*		Bit(s) RESERVED_RSRV_ reserved */

/*	usb2_otg				0x0034	usb2otg control register */
/*		Bit(s) USB2_OTG_RSRV_ reserved */
#define	USB2_OTG_OTG_CONTROL_BY_PIN		BIT_5				/* OTG_CONTROL_BY_PIN */
#define	USB2_OTG_PU_OTG					BIT_4				/* PU_OTG */
/*		Bit(s) USB2_OTG_RSRV_ reserved */
#define	USB2_OTG_TESTMON_OTG_MSK		SHIFT0(0x7)			/* TESTMON_OTG */
#define	USB2_OTG_TESTMON_OTG_BASE		0

/*	usb2_chgdtc				0x0038	usb2charger detector control register */
/*		Bit(s) USB2_CHGDTC_RSRV_ reserved */
/* ENABLE_SWITCH_DM */
#define	USB2_CHGDTC_ENABLE_SWITCH_DM			BIT_13				
/* ENABLE_SWITCH_DP */
#define	USB2_CHGDTC_ENABLE_SWITCH_DP			BIT_12				
#define	USB2_CHGDTC_VSRC_CHARGE_MSK				SHIFT10(0x3)		/* VSRC_CHARGE */
#define	USB2_CHGDTC_VSRC_CHARGE_BASE			10
#define	USB2_CHGDTC_VDAT_CHARGE_MSK				SHIFT8(0x3)			/* VDAT_CHARGE */
#define	USB2_CHGDTC_VDAT_CHARGE_BASE			8
/* CDP_DM_AUTO_SWITCH */
#define	USB2_CHGDTC_CDP_DM_AUTO_SWITCH			BIT_7				
/* DP_DM_SWAP_CTRL */
#define	USB2_CHGDTC_DP_DM_SWAP_CTRL				BIT_6				
#define	USB2_CHGDTC_PU_CHRG_DTC					BIT_5				/* PU_CHRG_DTC */
#define	USB2_CHGDTC_PD_EN						BIT_4				/* PD_EN */
#define	USB2_CHGDTC_DCP_EN						BIT_3				/* DCP_EN */
#define	USB2_CHGDTC_CDP_EN						BIT_2				/* CDP_EN */
/* TESTMON_CHRGDTC */
#define	USB2_CHGDTC_TESTMON_CHRGDTC_MSK			SHIFT0(0x3)			
#define	USB2_CHGDTC_TESTMON_CHRGDTC_BASE		0

/*	ip_revision				0x00c0	ip_revision */
/* phy_ip_revision[15:0] */
#define	IP_REVISION_PHY_IP_REVISION_MSK		SHIFT16(0xffff)	
#define	IP_REVISION_PHY_IP_REVISION_BASE		16
/* ip_revision[15:0] */
#define	IP_REVISION_IP_REVISION_MSK			SHIFT0(0xffff)	
#define	IP_REVISION_IP_REVISION_BASE			0

/*	usb_ctl					0x00c4	usb_ctl */
/*		Bit(s) USB_CTL_RSRV_ reserved */
#define USB_CTL_REG_OTG_SUSPENDM_EN         BIT_13
#define USB_CTL_REG_IDDQ_TEST				BIT_12
#define	USB_CTL_REG_OPMODE_MSK				SHIFT10(0x3)		/* reg_opmode */
#define	USB_CTL_REG_OPMODE_BASE				10
#define	USB_CTL_REG_XCVR_SELECT_MSK			SHIFT8(0x3)			/* reg_xcvr_select */
#define	USB_CTL_REG_XCVR_SELECT_BASE		8
#define	USB_CTL_REG_TERM_SELECT				BIT_7				/* reg_term_select */
#define	USB_CTL_REG_SEL						BIT_6				/* reg_sel */
#define	USB_CTL_VBUSVALID_CTL_MSK			SHIFT4(0x3)			/* vbusvalid_ctl */
#define	USB_CTL_VBUSVALID_CTL_BASE			4
#define	USB_CTL_OTG_SEL						BIT_3				/* otg_sel */
#define	USB_CTL_PU_PLL						BIT_2				/* pu_pll */
#define	USB_CTL_PU_OTG						BIT_1				/* pu_otg */
#define	USB_CTL_PU							BIT_0				/* pu */

/*	usb_int_misc			0x00c8	usb_int_misc */
/*		Bit(s) USB_INT_MISC_RSRV_ reserved */
#define	USB_INT_MISC_CHRG_INT_CLR			BIT_6				/* chrg_int_clr */
#define	USB_INT_MISC_CHRG_INT_EN			BIT_5				/* chrg_int_en */
#define	USB_INT_MISC_CHRG_DTC_OUT			BIT_4				/* CHRG_DTC_OUT */
#define	USB_INT_MISC_RESUME_INT				BIT_3				/* resume_int */
#define	USB_INT_MISC_VBUSDTC_OUT			BIT_2				/* VBUSDTC_OUT */
/* usb_resume_int_en */
#define	USB_INT_MISC_USB_RESUME_INT_EN		BIT_1				
#define	USB_INT_MISC_CORE INTERRUPT			BIT_0				/* core interrupt */

/*	dbg_ctl					0x00cc	dbg_ctl */
/*		Bit(s) DBG_CTL_RSRV_ reserved */
#define	DBG_CTL_DBG_SEL_MSK			SHIFT0(0xffff)	/* dbg_sel */
#define	DBG_CTL_DBG_SEL_BASE		0

/*	a_phy1					0x00d0	a_phy1 */
/*		Bit(s) A_PHY1_RSRV_ reserved */
#define	A_PHY1_CLK_SEL				BIT_10				/* clk_sel */
/*		Bit(s) A_PHY1_RSRV_ reserved */
#define	A_PHY1_SRAM_RTC_MSK			SHIFT6(0x3)			/* SRAM_RTC */
#define	A_PHY1_SRAM_RTC_BASE		6
#define	A_PHY1_SRAM_WTC_MSK			SHIFT4(0x3)			/* SRAM_WTC */
#define	A_PHY1_SRAM_WTC_BASE		4
/*		Bit(s) A_PHY1_RSRV_ reserved */
#define	A_PHY1_INT_MASK_SEL			BIT_2				/* int_mask_sel */
#define	A_PHY1_SRAM_PDWN			BIT_1				/* SRAM_PDWN */
#define	A_PHY1_PU_SRAM_CTL			BIT_0				/* PU_SRAM_CTL */

/*	a_phy2					0x00d4	a_phy2 */
/*		Bit(s) A_PHY2_RSRV_ reserved */
#define	A_PHY2_TX_BITSTUFF_EN		BIT_0				/* TX_BITSTUFF_EN */




#endif
