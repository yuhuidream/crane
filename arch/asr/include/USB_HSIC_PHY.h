#ifndef __USB_HSIC_PHY_H__
#define __USB_HSIC_PHY_H__

typedef struct
{

       __IO uint32_t	HSIC_ID;				/* 0x0000 32 bit	HSIC PHY ID reg */
	__IO uint32_t	HSIC_PAD_CTL;			/* 0x0004 32 bit	HSIC pad ctl reg */
	__IO uint32_t	HSIC_CTL0;				/* 0x0008 32 bit	HSIC ctl reg0 */
	__IO uint32_t	HSIC_DIGCTL;			/* 0x000c 32 bit	HSIC digctl reg */
	__IO uint32_t	HSIC_TEST0;				/* 0x0010 32 bit	HSIC test reg0 */
	__IO uint32_t	HSIC_TEST1;				/* 0x0014 32 bit	HSIC test reg1 */
	__IO uint32_t	HSIC_TEST2;				/* 0x0018 32 bit	HSIC test reg0 */
	__IO uint32_t	HSIC_PLL_CTRL0;			/* 0x001c 32 bit	HSIC pll control reg0 */
	__IO uint32_t	HSIC_PLL_CTRL1;			/* 0x0020 32 bit	HSIC pll control reg1 */
	__IO uint32_t	USB_CTL0;				/* 0x0024 32 bit	usb_ctl */
	__IO uint32_t	HSIC_INT_CTL;			/* 0x0028 32 bit	hsic_int_ctl */
	__IO uint32_t	USB_CTL1;				/* 0x002c 32 bit	usb_ctl */
	__IO uint32_t	USB_CTL2;				/* 0x0030 32 bit	usb_ctl */


}USB_HSIC_PHY_TypeDef,*USB_HSIC_PHY_TypeDef_PTR;



#define    USB_HSIC1_PHY_BASE      0xD4206000
#define    USB_HSIC1_PHY  (( USB_HSIC_PHY_TypeDef *) USB_HSIC1_PHY_BASE )




/*
 *
 *	THE BIT DEFINES
 *
 */
/*	hsic_id			0x0000	HSIC PHY ID reg */
#define	HSIC_ID_HSIC_ID_MSK			SHIFT0(0xffffffff)	/* hsic_id */
#define	HSIC_ID_HSIC_ID_BASE		0

/*	hsic_pad_ctl	0x0004	HSIC pad ctl reg */
/*		Bit(s) HSIC_PAD_CTL_RSRV_ reserved */
#define	HSIC_PAD_CTL_PLLVDDM_MSK		SHIFT14(0x3)	/* PLLVDDM */
#define	HSIC_PAD_CTL_PLLVDDM_BASE		14
#define	HSIC_PAD_CTL_PLLVDDL_MSK		SHIFT12(0x3)	/* PLLVDDL */
#define	HSIC_PAD_CTL_PLLVDDL_BASE		12
/*		Bit(s) HSIC_PAD_CTL_RSRV_ reserved */
#define	HSIC_PAD_CTL_CONN_BYP_OPMD0		BIT_4			/* CONN_BYP_OPMD0 */
#define	HSIC_PAD_CTL_V12EN				BIT_3			/* V12EN */
#define	HSIC_PAD_CTL_LOW_VDDB_R			BIT_2			/* LOW_VDDB_R */
#define	HSIC_PAD_CTL_SR_MSK				SHIFT0(0x3)		/* SR */
#define	HSIC_PAD_CTL_SR_BASE			0

/*	hsic_ctl0		0x0008	HSIC ctl reg0 */
#define	HSIC_CTL0_H2S_CONNECT_INT				BIT_31			/* H2S_CONNECT_INT */
#define	HSIC_CTL0_H2S_HS_READY_INT				BIT_30			/* H2S_HS_READY_INT */
/* S2H_SWAP_STB_DAT_PAD */
#define	HSIC_CTL0_S2H_SWAP_STB_DAT_PAD			BIT_29			
/* S2H_PHY_CONNECT_EN */
#define	HSIC_CTL0_S2H_PHY_CONNECT_EN			BIT_28			
#define	HSIC_CTL0_S2H_STOBE_DLY_MSK				SHIFT24(0xf)	/* S2H_STOBE_DLY */
#define	HSIC_CTL0_S2H_STOBE_DLY_BASE			24
#define	HSIC_CTL0_S2H_DATA_DLY_MSK				SHIFT20(0xf)	/* S2H_DATA_DLY */
#define	HSIC_CTL0_S2H_DATA_DLY_BASE				20
#define	HSIC_CTL0_S2H_RXCLK_DLY_MSK				SHIFT16(0xf)	/* S2H_RXCLK_DLY */
#define	HSIC_CTL0_S2H_RXCLK_DLY_BASE			16
/* S2H_DRV_IDLE_4FSK */
#define	HSIC_CTL0_S2H_DRV_IDLE_4FSK				BIT_15			
/* S2H_DRV_SE0_4RESUME */
#define	HSIC_CTL0_S2H_DRV_SE0_4RESUME			BIT_14			
/* S2H_BUSRESET_TIME */
#define	HSIC_CTL0_S2H_BUSRESET_TIME_MSK			SHIFT8(0x3f)	
#define	HSIC_CTL0_S2H_BUSRESET_TIME_BASE		8
#define	HSIC_CTL0_S2H_HSIC_ENABLE				BIT_7			/* S2H_HSIC_ENABLE */
#define	HSIC_CTL0_S2H_RX_ALWAYS_ON				BIT_6			/* S2H_RX_ALWAYS_ON */
#define	HSIC_CTL0_S2H_BUS_RESET_EN				BIT_5			/* S2H_BUS_RESET_EN */
/* S2H_PLL_LOCK_BYPASS */
#define	HSIC_CTL0_S2H_PLL_LOCK_BYPASS			BIT_4			
/* S2H_HW_CONNECT_BYP */
#define	HSIC_CTL0_S2H_HW_CONNECT_BYP			BIT_3			
/* S2H_SW_CONNECT_BYP */
#define	HSIC_CTL0_S2H_SW_CONNECT_BYP			BIT_2			
/* S2H_CONNECT_WIDTH */
#define	HSIC_CTL0_S2H_CONNECT_WIDTH_MSK			SHIFT0(0x3)		
#define	HSIC_CTL0_S2H_CONNECT_WIDTH_BASE		0

/*	hsic_digctl		0x000c	HSIC digctl reg */
/*		Bit(s) HSIC_DIGCTL_RSRV_ reserved */
/* S2H_FIX_SNPS_OPMODE */
#define	HSIC_DIGCTL_S2H_FIX_SNPS_OPMODE			BIT_22			
#define	HSIC_DIGCTL_S2H_MON_SEL_MSK				SHIFT16(0x3f)	/* S2H_MON_SEL */
#define	HSIC_DIGCTL_S2H_MON_SEL_BASE			16
/* S2H_ARC_DPDM_MODE */
#define	HSIC_DIGCTL_S2H_ARC_DPDM_MODE			BIT_15			
#define	HSIC_DIGCTL_S2H_DM_PULLDOWN				BIT_14			/* S2H_DM_PULLDOWN */
#define	HSIC_DIGCTL_S2H_DP_PULLDOWN				BIT_13			/* S2H_DP_PULLDOWN */
/* S2H_TX_BITSTUFF_EN */
#define	HSIC_DIGCTL_S2H_TX_BITSTUFF_EN			BIT_12			
/* S2H_EARLY_TX_ENABLE */
#define	HSIC_DIGCTL_S2H_EARLY_TX_ENABLE			BIT_11			
/* S2H_RXERROR_RST_EN */
#define	HSIC_DIGCTL_S2H_RXERROR_RST_EN			BIT_10			
#define	HSIC_DIGCTL_S2H_EOP_RST_EN				BIT_9			/* S2H_EOP_RST_EN */
#define	HSIC_DIGCTL_S2H_FIFO_SQ_RST				BIT_8			/* S2H_FIFO_SQ_RST */
#define	HSIC_DIGCTL_S2H_HS_HDL_SYNC				BIT_7			/* S2H_HS_HDL_SYNC */
#define	HSIC_DIGCTL_S2H_UTMI_LAT				BIT_6			/* S2H_UTMI_LAT */
#define	HSIC_DIGCTL_S2H_SYNC_NUM_MSK			SHIFT4(0x3)		/* S2H_SYNC_NUM */
#define	HSIC_DIGCTL_S2H_SYNC_NUM_BASE			4
/* S2H_FIFO_FILL_NUM */
#define	HSIC_DIGCTL_S2H_FIFO_FILL_NUM_MSK		SHIFT0(0xf)		
#define	HSIC_DIGCTL_S2H_FIFO_FILL_NUM_BASE		0

/*	hsic_test0		0x0010	HSIC test reg0 */
/* H2S_TEST_DONE */
#define	HSIC_TEST0_H2S_TEST_DONE					BIT_31			
/* H2S_TEST_FAIL */
#define	HSIC_TEST0_H2S_TEST_FAIL					BIT_30			
/* S2H_TEST_TX_BITSTUFF_EN */
#define	HSIC_TEST0_S2H_TEST_TX_BITSTUFF_EN			BIT_29			
/* S2H_TEST_TERM_SELECT */
#define	HSIC_TEST0_S2H_TEST_TERM_SELECT				BIT_28			
/* S2H_TEST_OP_MODE */
#define	HSIC_TEST0_S2H_TEST_OP_MODE_MSK				SHIFT26(0x3)	
#define	HSIC_TEST0_S2H_TEST_OP_MODE_BASE			26
/* S2H_TEST_XVCR_SELECT */
#define	HSIC_TEST0_S2H_TEST_XVCR_SELECT_MSK			SHIFT24(0x3)	
#define	HSIC_TEST0_S2H_TEST_XVCR_SELECT_BASE		24
/* S2H_TEST_TX_PATTERN */
#define	HSIC_TEST0_S2H_TEST_TX_PATTERN_MSK			SHIFT16(0xff)	
#define	HSIC_TEST0_S2H_TEST_TX_PATTERN_BASE			16
/* S2H_TEST_RESET */
#define	HSIC_TEST0_S2H_TEST_RESET					BIT_15			
/* S2H_TEST_SUSPENDM */
#define	HSIC_TEST0_S2H_TEST_SUSPENDM				BIT_14			
/*		Bit(s) HSIC_TEST0_RSRV_ reserved */
#define	HSIC_TEST0_S2H_TEST_EN						BIT_9			/* S2H_TEST_EN */
/* S2H_TEST_BYPASS */
#define	HSIC_TEST0_S2H_TEST_BYPASS					BIT_8			
/* S2H_TEST_UTMI_SEL */
#define	HSIC_TEST0_S2H_TEST_UTMI_SEL				BIT_7			
/* S2H_TEST_SKIP */
#define	HSIC_TEST0_S2H_TEST_SKIP_MSK				SHIFT4(0x7)		
#define	HSIC_TEST0_S2H_TEST_SKIP_BASE				4
/* S2H_TEST_DIG_LPBK */
#define	HSIC_TEST0_S2H_TEST_DIG_LPBK				BIT_3			
/* S2H_TEST_PAD_LPBK */
#define	HSIC_TEST0_S2H_TEST_PAD_LPBK				BIT_2			
/* S2H_TEST_MODE */
#define	HSIC_TEST0_S2H_TEST_MODE_MSK				SHIFT0(0x3)		
#define	HSIC_TEST0_S2H_TEST_MODE_BASE				0

/*	hsic_test1		0x0014	HSIC test reg1 */
#define	HSIC_TEST1_H2S_HSIC_MON_MSK			SHIFT16(0xffff)	/* H2S_HSIC_MON */
#define	HSIC_TEST1_H2S_HSIC_MON_BASE		16
/*		Bit(s) HSIC_TEST1_RSRV_ reserved */
#define	HSIC_TEST1_S2H_TEST_LENGTH_MSK		SHIFT0(0x7ff)	/* S2H_TEST_LENGTH */
#define	HSIC_TEST1_S2H_TEST_LENGTH_BASE		0

/*	hsic_test0		0x0018	HSIC test reg0 */
/*		Bit(s) HSIC_TEST0_RSRV_ reserved */
#define	HSIC_TEST2_H2S_IMP_SEL_MSK		SHIFT12(0xf)	/* H2S_IMP_SEL */
#define	HSIC_TEST2_H2S_IMP_SEL_BASE		12
#define	HSIC_TEST2_H2S_IMPCAL_DONE		BIT_11			/* H2S_IMPCAL_DONE */
/*		Bit(s) HSIC_TEST0_RSRV_ reserved */
#define	HSIC_TEST2_S2H_IMPCAL_START		BIT_9			/* S2H_IMPCAL_START */
#define	HSIC_TEST2_EXT_IMP_EN			BIT_8			/* EXT_IMP_EN */
#define	HSIC_TEST2_EXT_IMP_MSK			SHIFT4(0xf)		/* EXT_IMP */
#define	HSIC_TEST2_EXT_IMP_BASE			4
#define	HSIC_TEST2_UD_INVERSE			BIT_3			/* UD_INVERSE */
#define	HSIC_TEST2_IMPCAL_VTH_MSK		SHIFT0(0x7)		/* IMPCAL_VTH */
#define	HSIC_TEST2_IMPCAL_VTH_BASE		0

/*	hsic_pll_ctrl0	0x001c	HSIC pll control reg0 */
#define	HSIC_PLL_CTRL0_TESTMON_PLL_MSK		SHIFT28(0xf)	/* TESTMON_PLL */
#define	HSIC_PLL_CTRL0_TESTMON_PLL_BASE		28
#define	HSIC_PLL_CTRL0_SEL_LPFR_MSK			SHIFT26(0x3)	/* SEL_LPFR */
#define	HSIC_PLL_CTRL0_SEL_LPFR_BASE		26
#define	HSIC_PLL_CTRL0_STRESS_TEST_MODE		BIT_25			/* STRESS_TEST_MODE */
#define	HSIC_PLL_CTRL0_REFCLK_SEL			BIT_24			/* REFCLK_SEL */
/*		Bit(s) HSIC_PLL_CTRL0_RSRV_ reserved */
#define	HSIC_PLL_CTRL0_REFDIV_MSK			SHIFT9(0x7f)	/* REFDIV */
#define	HSIC_PLL_CTRL0_REFDIV_BASE			9
#define	HSIC_PLL_CTRL0_FBDIV_MSK			SHIFT0(0x1ff)	/* FBDIV */
#define	HSIC_PLL_CTRL0_FBDIV_BASE			0

/*	hsic_pll_ctrl1	0x0020	HSIC pll control reg1 */
/*		Bit(s) HSIC_PLL_CTRL1_RSRV_ reserved */
#define	HSIC_PLL_CTRL1_H2S_PLL_LOCK					BIT_15			/* H2S_PLL_LOCK */
/* S2H_EXT_CLK480M_READY */
#define	HSIC_PLL_CTRL1_S2H_EXT_CLK480M_READY		BIT_14			
/* S2H_EXT_CLK480M_SEL */
#define	HSIC_PLL_CTRL1_S2H_EXT_CLK480M_SEL			BIT_13			
/* S2H_CLK120M_EN */
#define	HSIC_PLL_CTRL1_S2H_CLK120M_EN				BIT_12			
/* S2H_CLK160M_EN */
#define	HSIC_PLL_CTRL1_S2H_CLK160M_EN				BIT_11			
#define	HSIC_PLL_CTRL1_S2H_PU_PLL					BIT_10			/* S2H_PU_PLL */
#define	HSIC_PLL_CTRL1_CTUNE_MSK					SHIFT8(0x3)		/* CTUNE */
#define	HSIC_PLL_CTRL1_CTUNE_BASE					8
/*		Bit(s) HSIC_PLL_CTRL1_RSRV_ reserved */
#define	HSIC_PLL_CTRL1_KVCO_MSK						SHIFT4(0x7)		/* KVCO */
#define	HSIC_PLL_CTRL1_KVCO_BASE					4
#define	HSIC_PLL_CTRL1_ICP_MSK						SHIFT0(0xf)		/* ICP */
#define	HSIC_PLL_CTRL1_ICP_BASE						0

/*	usb_ctl			0x0024	usb_ctl */
#define	USB_CTL_OTG_PLL_READY			BIT_31				/* OTG_PLL_READY */
/*		Bit(s) USB_CTL_RSRV_ reserved */
#define	USB_CTL_IDDIG_CTL_MSK			SHIFT7(0x3)			/* iddig_ctl */
#define	USB_CTL_IDDIG_CTL_BASE			7
#define	USB_CTL_DPPULLDOWN_CTL_MSK		SHIFT5(0x3)			/* dppulldown_ctl */
#define	USB_CTL_DPPULLDOWN_CTL_BASE		5
#define	USB_CTL_SRAM_PDWN				BIT_4				/* SRAM_PDWN */
#define	USB_CTL_SRAM_RTC_MSK			SHIFT2(0x3)			/* SRAM_RTC */
#define	USB_CTL_SRAM_RTC_BASE			2
#define	USB_CTL_SRAM_WTC_MSK			SHIFT0(0x3)			/* SRAM_WTC */
#define	USB_CTL_SRAM_WTC_BASE			0

/*	hsic_int_ctl	0x0028	hsic_int_ctl */
/*		Bit(s) HSIC_INT_CTL_RSRV_ reserved */
/* resume_int_en */
#define	HSIC_INT_CTL_RESUME_INT_EN				BIT_11				
/* phy_ready_int_en */
#define	HSIC_INT_CTL_PHY_READY_INT_EN			BIT_10				
/* phy_connect_int_en */
#define	HSIC_INT_CTL_PHY_CONNECT_INT_EN			BIT_9				
/*		Bit(s) HSIC_INT_CTL_RSRV_ reserved */
/* resume_int_status */
#define	HSIC_INT_CTL_RESUME_INT_STATUS			BIT_6				
/*		Bit(s) HSIC_INT_CTL_RSRV_ reserved */
/* phy_ready_int_status */
#define	HSIC_INT_CTL_PHY_READY_INT_STATUS		BIT_2				
/* phy_connect_int_status */
#define	HSIC_INT_CTL_PHY_CONNECT_INT_STATUS		BIT_1				
#define	HSIC_INT_CTL_USB_CORE_INT				BIT_0				/* usb_core_int */

/*	usb_ctl			0x002c	usb_ctl */
/*		Bit(s) USB_CTL_RSRV_ reserved */
#define	USB_CTL_CLK_SEL			BIT_10				/* clk_sel */
/*		Bit(s) USB_CTL_RSRV_ reserved */
#define	USB_CTL_INT_MASK		BIT_2				/* int_mask */
/*		Bit(s) USB_CTL_RSRV_ reserved */

/*	usb_ctl			0x0030	usb_ctl */
#define	USB_CTL_OTG_PLL_READY		BIT_31			/* OTG_PLL_READY */
/*		Bit(s) USB_CTL_RSRV_ reserved */
#define	USB_CTL_DBG_CTL_MSK			SHIFT12(0xf)	/* dbg_ctl */
#define	USB_CTL_DBG_CTL_BASE		12
/*		Bit(s) USB_CTL_RSRV_ reserved */
#define	USB_CTL_DBG_SEL				BIT_0			/* dbg_sel */



/* -------------------- */

#endif
