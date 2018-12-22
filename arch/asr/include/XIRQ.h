#ifndef __XIRQ_H__
#define __XIRQ_H__

typedef struct
{
    __IO uint32_t       SWIENABLE;
    __IO uint32_t       RIENABLE;
    __IO uint32_t       XSWIENABLE;
    __IO uint32_t       EIRQENABLE;//0X000C
    __IO uint32_t       RESERVED0[(0x0020-0x000C)/4-1];
    __IO uint32_t       HWIENABLEn[8];   //0x20-0x3C
    __IO uint32_t       SWIENABLECLR;//0X0040
    __IO uint32_t       RIENABLECLR;
    __IO uint32_t       EIRQENABLECLR;//0X0048
    __IO uint32_t       RESERVED11[(0x60-0x48)/4-1];
    __IO uint32_t       HWIENABLECLRn[8]; //0x60-0x7C
    __IO uint32_t       CIP;//0X0080
    __IO uint32_t       CIN;
    __IO uint32_t       AIM;
    __IO uint32_t       SYSREG;
    __IO uint32_t       SWICARRYSTATUS;
    __IO uint32_t       RICARRYSTATUS;
    __IO uint32_t       XSWICARRYSTATUS;
    __IO uint32_t       CINVALUE;
    __IO uint32_t       SWIPRIORITYn[16];
    __IO uint32_t       RIPRIORITYn[8];//0X00E0-0X00FC
    __IO uint32_t       RESERVED2[(0x0120-0x00FC)/4-1];
    __IO uint32_t       XSWIPRIORITY;//0X0120
    __IO uint32_t       RESERVED3[(0x0140-0x0120)/4-1];
    __IO uint32_t       EXSWIPRIORITY;//0X0140
    __IO uint32_t       ERIPRIORITY;
    __IO uint32_t       ESWPRIORITY;//0X148
    __IO uint32_t       RESERVED4[(0x300-0x148)/4-1];
    __IO uint32_t       MIRQ_DUMMY_READ;//0X300
    __IO uint32_t       RESERVED5[(0x100)/4-1];
    __IO uint32_t       SWICOUNTn[16];
    __IO uint32_t       RIREGISTER;//0X440
    __IO uint32_t       RESERVED6[(0x460-0x440)/4-1];
    __IO uint32_t       XSWICOUNT;//0X460
    __IO uint32_t       XSWIASSERT;
    __IO uint32_t       IRQSTATCTL;//0X468
    __IO uint32_t       RESERVED7[(0x08F0-0x0468)/4-1];
    __IO uint32_t       EDGE_OR_LEVEL0;//0X08F0
    __IO uint32_t       EDGE_OR_LEVEL1;
    __IO uint32_t       STIKY_CLR0;
    __IO uint32_t       STIKY_CLR1;



}XIRQ_TypeDef;



#define    AIRQ_BASE      0xD4031000

#define    AIRQ  (( XIRQ_TypeDef *) AIRQ_BASE )

#define    MIRQ_BASE      0xFFC0C000

#define    MIRQ  (( XIRQ_TypeDef *) MIRQ_BASE )



/*	SWIENABLE			0x0000	Software Interrupt xIRQ Enable Set Register */
/* Software Interrupt Enable */
#define	SWIENABLE_SWI_ENB_SET_MSK		SHIFT00(0xffff)	
#define	SWIENABLE_SWI_ENB_SET_BASE		00

/*	RIENABLE			0x0004	Register Interrupt xIRQ Enable Set Register */
/* Register Interrupt Enable Set */
#define	RIENABLE_RI_ENB_SET_MSK			SHIFT00(0xff)	
#define	RIENABLE_RI_ENB_SET_BASE		00

/*	XSWIENABLE			0x0008	Cross-Assert Interrupt xIRQ Enable Set Register */
#define	XSWIENABLE_XSWI_ENB		BIT_0	/* Cross-Assert Interrupt Enable Set */

/*	EIRQENABLE			0x000C	Error Interrupt xIRQ Enable Set Register */
/* Error Interrupt Enable Set */
#define	EIRQENABLE_EI_ENB_SET_MSK		SHIFT00(0x7)	
#define	EIRQENABLE_EI_ENB_SET_BASE		00

/*	L1_HWIENABLEn		0x0020	MIRQ/AIRQ: Hardware Interrupt Bank n xIRQ
 *								Enable Set Register
 */
/* Hardware Interrupt Bank n Enable Set */
#define	HWIENABLEN_HWI_N_ENB_SET_MSK		SHIFT00(0xffff)	
#define	HWIENABLEN_HWI_N_ENB_SET_BASE		00

/*	SWIENABLECLR		0x0040	Software Interrupt xIRQ Enable Clr Register */
/* Software Interrupt Enable Clear */
#define	SWIENABLECLR_SWI_ENB_CLR_MSK		SHIFT00(0xffff)	
#define	SWIENABLECLR_SWI_ENB_CLR_BASE		00

/*	RIENABLECLR			0x0044	Register Interrupt xIRQ Enable Clr Register */
/* Register Interrupt Enable Clear */
#define	RIENABLECLR_RI_ENB_CLR_MSK		SHIFT00(0xff)	
#define	RIENABLECLR_RI_ENB_CLR_BASE		00

/*	EIRQENABLECLR		0x0048	Error Interrupt xIRQ Enable Clr Register */
/* Error Interrupt Enable Clear */
#define	EIRQENABLECLR_EI_ENB_CLR_MSK		SHIFT00(0x7)	
#define	EIRQENABLECLR_EI_ENB_CLR_BASE		00

/*	L1_HWIENABLECLRn	0x0060	MIRQ/AIRQ: Hardware Interrupt Bank n xIRQ
 *								Enable Clr Register
 */
/* Hardware Interrupt Bank n Enable Clear */
#define	HWIENABLECLRN_HWI_N_ENB_CLR_MSK			SHIFT00(0xffff)	
#define	HWIENABLECLRN_HWI_N_ENB_CLR_BASE		00

/*	CIP					0x0080	Current Interrupt Priority Register */
#define	CIP_CIP_MSK			SHIFT00(0xf)	/* Current Interrupt Priority */
#define	CIP_CIP_BASE		00

/*	CIN					0x0084	Current Interrupt Number Register */
#define	CIN_CIN_MSK			SHIFT00(0x7f)	/* Current Interrupt Number */
#define	CIN_CIN_BASE		00

/*	AIM					0x0088	Active Interrupt Mask Register */
#define	AIM_AIM_MSK			SHIFT00(0xffff)	/* Active Interrupt Mask */
#define	AIM_AIM_BASE		00

/*	SYSREG				0x008C	System Register */
#define	SYSREG_IRQCTRL_RST		BIT_0	/* IRQCtrl Reset flag */

/*	SWICARRYSTATUS		0x0090	Software Interrupt Carry Status Register */
/* Software Interrupt Carry Status */
#define	SWICARRYSTATUS_SWI_CS_MSK		SHIFT00(0xffff)	
#define	SWICARRYSTATUS_SWI_CS_BASE		00

/*	RICARRYSTATUS		0x0094	Register Interrupt Carry Status Register */
/* Register Interrupt Carry Status */
#define	RICARRYSTATUS_RI_CS_MSK			SHIFT00(0xff)	
#define	RICARRYSTATUS_RI_CS_BASE		00

/*	XSWICARRYSTATUS		0x0098	Cross-Assertion Interrupt Carry Status Register */
/* Cross-Assertion Interrupt Carry Status */
#define	XSWICARRYSTATUS_XSWI_CS		BIT_0	

/*	CINVALUE			0x009C	Current Interrupt Number Value Register */
#define	CINVALUE_CINV_MSK		SHIFT00(0x7f)	/* Current Interrupt Number Value */
#define	CINVALUE_CINV_BASE		00

/*	SWIPRIORITYn		0x00A0	Software Interrupt n Priority Register */
/* Software Interrupt n Priority */
#define	SWIPRIORITYN_SWIPN_MSK		SHIFT00(0xf)	
#define	SWIPRIORITYN_SWIPN_BASE		00

/*	RIPRIORITYn			0x00E0	Register Interrupt n Priority Register */
/* Register Interrupt n Priority */
#define	RIPRIORITYN_RIPN_MSK		SHIFT00(0xf)	
#define	RIPRIORITYN_RIPN_BASE		00

/*	XSWIPRIORITY		0x0120	Cross-Assert Interrupt Priority Register */
/* Cross-Assert Interrupt Priority */
#define	XSWIPRIORITY_XSWIP_MSK		SHIFT00(0xf)	
#define	XSWIPRIORITY_XSWIP_BASE		00

/*	EXSWIPRIORITY		0x0140	Cross-Assert Error Interrupt Priority Register */
/* Cross-Assert Error Interrupt Priority */
#define	EXSWIPRIORITY_XSWI_EIRQP_MSK		SHIFT00(0xf)	
#define	EXSWIPRIORITY_XSWI_EIRQP_BASE		00

/*	ERIPRIORITY			0x0144	Register Error Interrupt Priority Register */
/* Register Error Interrupt Priority */
#define	ERIPRIORITY_RI_EIRQP_MSK		SHIFT00(0xf)	
#define	ERIPRIORITY_RI_EIRQP_BASE		00

/*	ESWPRIORITY			0x0148	Software Error Interrupt Priority Register */
/* Software Error Interrupt Priority */
#define	ESWPRIORITY_SWI_EIRQP_MSK		SHIFT00(0xf)	
#define	ESWPRIORITY_SWI_EIRQP_BASE		00

/*	L1_HWPRIORITYn		0x0200	MIRQ/AIRQ: Hardware Interrupt n Priority
 *								Register
 */
/* Hardware Interrupt n Priority */
#define	HWPRIORITYN_HWIPN_MSK		SHIFT00(0xf)	
#define	HWPRIORITYN_HWIPN_BASE		00

/*	MIRQ_DUMMY_READ		0x0300	MIRQ Dummy Read Register */

/*	SWICOUNTn			0x0400	Software Interrupt n Count Register */
#define	SWICOUNTN_SWICN_MSK			SHIFT00(0x1f)	/* Software Interrupt n Count */
#define	SWICOUNTN_SWICN_BASE		00

/*	RIREGISTER			0x0440	RI Register */
#define	RIREGISTER_RIR_MSK		SHIFT00(0xff)	/* RIR */
#define	RIREGISTER_RIR_BASE		00

/*	XSWICOUNT			0x0460	Cross-Assert Interrupt Count Register */
#define	XSWICOUNT_XSWIC_MSK			SHIFT00(0x1f)	/* Cross-Assert Interrupt Count */
#define	XSWICOUNT_XSWIC_BASE		00

/*	XSWIASSERT			0x0464 	Cross-Assert Interrupt Assert Register */
#define	XSWIASSERT_XSWIA		BIT_0	/* Cross-Assert Interrupt Assert */

/*	IRQSTATCTL			0x0468	IRQ Status Control Register */
#define	IRQSTATCTL_IRQSTATCTRL_MSK		SHIFT00(0x3)	/* IRQ Status Control */
#define	IRQSTATCTL_IRQSTATCTRL_BASE		00







#endif
