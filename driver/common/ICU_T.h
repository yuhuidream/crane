#ifndef ICU_H_
#define ICU_H_


/*
 *
 *	THE BASE ADDRESSES
 *
 */
#define	ICU_BASE	0xD4282000

/*
 *
 *	THE REGISTER DEFINES
 *
 */
#define	ICU_IRQ_0_TO_63_CONF	(ICU_BASE+0x0000)	/* IRQ 0 to 63 Configuration
													 *			Registers
													 *			Start
													 */
#define	ICU_CP_FIQ_NUM		(ICU_BASE+0x0100)	/* 32 bit	FIQ Selected
													 *			Interrupt
													 *			Number
													 *			Register
													 */
#define	ICU_CP_IRQ_NUM		(ICU_BASE+0x0104)	/* 32 bit	IRQ Selected
													 *			Interrupt
													 *			Number
													 *			Register
													 */
#define	ICU_A9C0_FIQ_NUM	(ICU_BASE+0x0108)	/* 32 bit	IRQ2 Selected
													 *			Interrupt
													 *			Number
													 *			Register
													 */
#define	ICU_A9C0_IRQ_NUM		(ICU_BASE+0x010C)	/* 32 bit	Global IRQ0
													 *			Mask Register
													 */
#define	ICU_CP_GBL_INT_MSK		(ICU_BASE+0x0110)	/* 32 bit	Global IRQ1
													 *			Mask Register
													 */
#define	ICU_A9C0_GBL_INT_MSK		(ICU_BASE+0x0114)	/* 32 bit	Global IRQ2
													 *			Mask Register
													 */
#define	ICU_DMA_CP_INT_MSK		(ICU_BASE+0x0118)	/* 32 bit	DMA IRQ0 Mask
													 *			Register
													 */
#define	ICU_DMA_A9C0_INT_MSK		(ICU_BASE+0x011C)	/* 32 bit	DMA IRQ1 Mask
													 *			Register
													 */
#define	ICU_DMA_CP_INT_STATUS		(ICU_BASE+0x0120)	/* 32 bit	DMA IRQ2 Mask
													 *			Register
													 */
#define	ICU_DMA_A9C0_INT_STATUS		(ICU_BASE+0x0124)	/* 32 bit	DMA IRQ0 Status
													 *			Register
													 */
#define	ICU_INT_STATUS_0		(ICU_BASE+0x0128)	/* 32 bit	DMA IRQ1 Status
													 *			Register
													 */
#define	ICU_INT_STATUS_1		(ICU_BASE+0x012C)	/* 32 bit	DMA IRQ2 Status
													 *			Register
													 */
#define	ICU_ARM_INT_MSK			(ICU_BASE+0x0130)	/* 32 bit	IRQ0 Status
													 *			Register 0 
													 */
#define	ICU_ARM_INT_STATUS		(ICU_BASE+0x0134)	/* 32 bit	IRQ0 Status
													 *			Register 1 
													 */
#define	ICU_PA_CLR			(ICU_BASE+0x0138)	/* 32 bit	IRQ1 Status
													 *			Register 0 
													 */
#define	ICU_A9C1_FIQ_NUM		(ICU_BASE+0x013C)	/* 32 bit	IRQ1 Status
													 *			Register 1 
													 */
#define	ICU_A9C1_IRQ_NUM		(ICU_BASE+0x0140)	/* 32 bit	IRQ2 Status
													 *			Register 0 
													 */
#define	ICU_A9C1_GBL_INT_MSK		(ICU_BASE+0x0144)	/* 32 bit	IRQ2 Status
													 *			Register 1 
													 */
#define	ICU_DMA_A9C1_INT_MSK		(ICU_BASE+0x0148)	/* 32 bit	INT4 Status
													 *			Register 
													 */
#define	ICU_DMA_A9C1_INT_STATUS		(ICU_BASE+0x014C)	/* 32 bit	INT5 Status
													 *			Register 
													 */
#if 0
#define	ICU_INT_17_STATUS		(ICU_BASE+0x0158)	/* 32 bit	INT17 Status
													 *			Register 
													 */
#define	ICU_INT_35_STATUS		(ICU_BASE+0x015C)	/* 32 bit	INT35 Status
													 *			Register 
													 */
#define	ICU_INT_51_STATUS		(ICU_BASE+0x0160)	/* 32 bit	INT51 Status
													 *			Register 
													 */
#define	ICU_INT_INVERT  		(ICU_BASE+0x0164)	/* 32 bit	INT invert 
													 *			Register 
													 */																 																 																 																 																 													 
#define	ICU_INT_4_MASK  		(ICU_BASE+0x0168)	/* 32 bit	INT4 MASK
													 *			Register 
													 */
#define	ICU_INT_5_MASK  		(ICU_BASE+0x016C)	/* 32 bit	INT5 MASK
													 *			Register 
													 */
#define	ICU_INT_17_MASK  		(ICU_BASE+0x0170)	/* 32 bit	INT17 MASK
													 *			Register 
													 */
#define	ICU_INT_35_MASK  		(ICU_BASE+0x0174)	/* 32 bit	INT35 MASK
													 *			Register 
													 */
#define	ICU_INT_51_MASK  		(ICU_BASE+0x0178)	/* 32 bit	INT51 MASK
													 *			Register 
													 */	
#define	ICU_INT_55_MASK  		(ICU_BASE+0x017C)	/* 32 bit	INT55 MASK
													 *			Register 
													 */			
#define	ICU_INT_57_MASK  		(ICU_BASE+0x0180)	/* 32 bit	INT57 MASK
													 *			Register 
													 */	
#define	ICU_INT_55_STATUS		(ICU_BASE+0x0184)	/* 32 bit	INT55 Status
													 *			Register 
													 */
#define	ICU_INT_57_STATUS		(ICU_BASE+0x0188)	/* 32 bit	INT57 Status
													 *			Register 
													 */	
#define	ICU_INT_6_MASK			(ICU_BASE+0x01A4)	/* 32 bit	INT6 Mask
													 *			Register 
													 */
#define	ICU_INT_8_MASK			(ICU_BASE+0x01A8)	/* 32 bit	INT8 Mask
													 *			Register 
													 */
#define	ICU_INT_18_MASK			(ICU_BASE+0x01AC)	/* 32 bit	INT18 Mask
													 *			Register 
													 */
#define	ICU_INT_30_MASK			(ICU_BASE+0x01B0)	/* 32 bit	INT30 Mask
													 *			Register 
													 */
#define	ICU_INT_42_MASK			(ICU_BASE+0x01B4)	/* 32 bit	INT42 Mask
													 *			Register 
													 */
#define	ICU_INT_58_MASK			(ICU_BASE+0x01B8)	/* 32 bit	INT58 Mask
													 *			Register 
													 */
#define	ICU_INT_6_STATUS		(ICU_BASE+0x01BC)	/* 32 bit	INT6 Status
													 *			Register 
													 */	
#define	ICU_INT_8_STATUS		(ICU_BASE+0x01C0)	/* 32 bit	INT8 Status
													 *			Register 
													 */	
#define	ICU_INT_18_STATUS		(ICU_BASE+0x01C4)	/* 32 bit	INT18 Status
													 *			Register 
													 */	
#define	ICU_INT_30_STATUS		(ICU_BASE+0x01C8)	/* 32 bit	INT30 Status
													 *			Register 
													 */	
#define	ICU_INT_42_STATUS		(ICU_BASE+0x01CC)	/* 32 bit	INT42 Status
													 *			Register 
													 */		
                                                     											 																		 												 													 															 													 												 													 													 													 
#define	ICU_INT_58_STATUS		(ICU_BASE+0x01D0)	/* 32 bit	INT58 Status
													 *			Register 
													 */	
                                                                                                          											 																		 												 													 															 													 												 													 													 													 
#define	ICU_FIQ_0_TO_63_CONF	(ICU_BASE+0x0200)	/* FIQ 0 to 63 Configuration
													 *			Registers
													 *			Start
													 */
#define	ICU_FIQ_SEL_INT_NUM		(ICU_BASE+0x0300)	/* 32 bit	FIQ0 Selected
													 *			Interrupt
													 *			Number
													 *			Register
													 */
#define	ICU_GBL_FIQ_MSK			(ICU_BASE+0x0304)	/* 32 bit	Global FIQ0
													 *			Mask Register
													 */
#define	ICU_DMA_FIQ_MSK			(ICU_BASE+0x0308)	/* 32 bit	DMA FIQ0 Mask
													 *			Register
													 */
#define	ICU_DMA_FIQ_STATUS		(ICU_BASE+0x030C)	/* 32 bit	DMA FIQ0 Status
													 *			Register
													 */
#define	ICU_FIQ_STATUS_0		(ICU_BASE+0x0310)	/* 32 bit	FIQ0 Status
													 *			Register 0 
													 */
#define	ICU_FIQ_STATUS_1		(ICU_BASE+0x0314)	/* 32 bit	FIQ0 Status
													 *			Register 1 
													 */
#endif
/*
 *
 *	THE BIT DEFINES
 *
 */
/*	ICU_IRQ_0_TO_63_CONF	0x0000	IRQ 0 to 63 Configuration Registers */
/*		Bit(s) ICU_IRQ_0_TO_63_CONF_RSRV_31_9 reserved */
#define	ICU_IRQ_0_TO_63_CONF_CPU1_INT			BIT_7				/* IRQ2_EN */
#define	ICU_IRQ_0_TO_63_CONF_CPU0_INT			BIT_6				/* IRQ1_EN */
#define	ICU_IRQ_0_TO_63_CONF_CP_INT			BIT_5				/* IRQ0_EN */
#define ICU_IRQ_0_TO_63_CONF_IRQ_FIQ			BIT_4
#define	ICU_IRQ_0_TO_63_CONF_PRIORITY_MSK		SHIFT0(0xf)			/* Priority */
#define	ICU_IRQ_0_TO_63_CONF_PRIORITY_BASE		0
#define	ICU_IRQ_0_TO_63_CONF_IRQ_FIQ_MSK		1

/*	ICU_IRQ0_SEL_INT_NUM	0x0100	IRQ0 Selected Interrupt Number Register */
/*		Bit(s) ICU_IRQ0_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	ICU_CP_FIQ_NUM_INT_PENDING			BIT_6				
/* Selected Interrupt Number */
#define	ICU_CP_FIQ_NUM_SEL_INT_NUM_MSK			SHIFT0(0x3f)		
#define	ICU_CP_FIQ_INT_NUM_SEL_INT_NUM_BASE		0

/*	ICU_IRQ1_SEL_INT_NUM	0x0104	IRQ1 Selected Interrupt Number Register */
/*		Bit(s) ICU_IRQ1_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	ICU_CP_IRQ_NUM_INT_PENDING			BIT_6				
/* Selected Interrupt Number */
#define	ICU_CP_IRQ_NUM_SEL_INT_NUM_MSK			SHIFT0(0x3f)		
#define	ICU_CP_IRQ_NUM_SEL_INT_NUM_BASE			0

/*	ICU_IRQ2_SEL_INT_NUM	0x0108	IRQ2 Selected Interrupt Number Register */
/*		Bit(s) ICU_IRQ2_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	ICU_A9C0_IRQ_NUM_INT_PENDING			BIT_6				
/* Selected Interrupt Number */
#define	ICU_A9C0_IRQ_NUM_SEL_INT_NUM_MSK		SHIFT0(0x3f)		
#define	ICU_A9C0_IRQ_NUM_SEL_INT_NUM_BASE		0

#define	ICU_A9C0_FIQ_NUM_INT_PENDING			BIT_6				
/* Selected Interrupt Number */
#define	ICU_A9C0_FIQ_NUM_SEL_INT_NUM_MSK		SHIFT0(0x3f)		
#define	ICU_A9C0_FIQ_NUM_SEL_INT_NUM_BASE		0

#define	ICU_A9C1_IRQ_NUM_INT_PENDING			BIT_6				
/* Selected Interrupt Number */
#define	ICU_A9C1_IRQ_NUM_SEL_INT_NUM_MSK		SHIFT0(0x3f)		
#define	ICU_A9C1_IRQ_NUM_SEL_INT_NUM_BASE		0

#define	ICU_A9C1_FIQ_NUM_INT_PENDING			BIT_6				
/* Selected Interrupt Number */
#define	ICU_A9C1_FIQ_NUM_SEL_INT_NUM_MSK		SHIFT0(0x3f)		
#define	ICU_A9C1_FIQ_NUM_SEL_INT_NUM_BASE		0

/*	ICU_GBL_IRQ0_MSK		0x010C	Global IRQ0 Mask Register */
/*		Bit(s) ICU_GBL_IRQ0_MSK_RSRV_31_1 reserved */
#define	ICU_CP_GBL_INT_MSK_FIQ_MSK				BIT_1			/* Mask FIQ0 */
#define	ICU_CP_GBL_INT_MSK_IRQ_MSK				BIT_0			/* Mask IRQ0 */
	
/*	ICU_GBL_IRQ1_MSK		0x0110	Global IRQ1 Mask Register */
/*		Bit(s) ICU_GBL_IRQ1_MSK_RSRV_31_1 reserved */
#define	ICU_A9C0_GBL_INT_MSK_FIQ_MSK				BIT_1			/* Mask FIQ1 */
#define	ICU_A9C0_GBL_INT_MSK_IRQ_MSK				BIT_0			/* Mask IRQ1 */

/*	ICU_GBL_IRQ2_MSK		0x0114	Global IRQ2 Mask Register */
/*		Bit(s) ICU_GBL_IRQ2_MSK_RSRV_31_1 reserved */
#define	ICU_A9C1_GBL_INT_MSK_FIQ_MSK				BIT_1			/* Mask FIQ2 */
#define	ICU_A9C1_GBL_INT_MSK_IRQ_MSK				BIT_0			/* Mask IRQ2 */

/*	ICU_DMA_IRQ0_MSK		0x0118	DMA IRQ0 Mask Register */
/*		Bit(s) ICU_DMA_IRQ0_MSK_RSRV_31_20 reserved */
/* VDMA IRQ0 Mask */
#define	ICU_DMA_IRQ0_MSK_VDMA_IRQ0_MSK_MSK				SHIFT22(0x3)	
#define	ICU_DMA_IRQ0_BASE_VDMA_IRQ0_BASE_BASE			22
/* ADMA IRQ0 Mask */
#define	ICU_DMA_IRQ0_MSK_ADMA_IRQ0_MSK_MSK				SHIFT18(0xf)	
#define	ICU_DMA_IRQ0_BASE_ADMA_IRQ0_BASE_BASE			18
/* MDMA IRQ0 Mask */
#define	ICU_DMA_IRQ0_MSK_MDMA_IRQ0_MSK_MSK				SHIFT16(0x3)	
#define	ICU_DMA_IRQ0_BASE_MDMA_IRQ0_BASE_BASE			16
/* PDMA IRQ0 Mask */
#define	ICU_DMA_IRQ0_MSK_PDMA_IRQ0_MSK_MSK				SHIFT0(0xffff)	
#define	ICU_DMA_IRQ0_BASE_PDMA_IRQ0_BASE_BASE			0

/*	ICU_DMA_IRQ1_MSK		0x011C	DMA IRQ1 Mask Register */
/*		Bit(s) ICU_DMA_IRQ1_MSK_RSRV_31_20 reserved */
/* VDMA IRQ1 Mask */
#define	ICU_DMA_IRQ1_MSK_VDMA_IRQ1_MSK_MSK				SHIFT22(0x3)	
#define	ICU_DMA_IRQ1_BASE_VDMA_IRQ1_BASE_BASE			22
/* ADMA IRQ1 Mask */
#define	ICU_DMA_IRQ1_MSK_ADMA_IRQ1_MSK_MSK				SHIFT18(0xf)	
#define	ICU_DMA_IRQ1_BASE_ADMA_IRQ1_BASE_BASE			18
/* MDMA IRQ1 Mask */
#define	ICU_DMA_IRQ1_MSK_MDMA_IRQ1_MSK_MSK				SHIFT16(0x3)	
#define	ICU_DMA_IRQ1_BASE_MDMA_IRQ1_BASE_BASE			16
/* PDMA IRQ1 Mask */
#define	ICU_DMA_IRQ1_MSK_PDMA_IRQ1_MSK_MSK				SHIFT0(0xffff)	
#define	ICU_DMA_IRQ1_BASE_PDMA_IRQ1_BASE_BASE			0

/*	ICU_DMA_IRQ2_MSK		0x0120	DMA IRQ2 Mask Register */
/*		Bit(s) ICU_DMA_IRQ2_MSK_RSRV_31_20 reserved */
/* VDMA IRQ2 Mask */
#define	ICU_DMA_IRQ2_MSK_VDMA_IRQ2_MSK_MSK				SHIFT22(0x3)	
#define	ICU_DMA_IRQ2_BASE_VDMA_IRQ2_BASE_BASE			22
/* ADMA IRQ2 Mask */
#define	ICU_DMA_IRQ2_MSK_ADMA_IRQ2_MSK_MSK				SHIFT18(0xf)	
#define	ICU_DMA_IRQ2_BASE_ADMA_IRQ2_BASE_BASE			18
/* MDMA IRQ2 Mask */
#define	ICU_DMA_IRQ2_MSK_MDMA_IRQ2_MSK_MSK				SHIFT16(0x3)	
#define	ICU_DMA_IRQ2_BASE_MDMA_IRQ2_BASE_BASE			16
/* PDMA IRQ2 Mask */
#define	ICU_DMA_IRQ2_MSK_PDMA_IRQ2_MSK_MSK				SHIFT0(0xffff)	
#define	ICU_DMA_IRQ2_BASE_PDMA_IRQ2_BASE_BASE			0

/*	ICU_DMA_IRQ0_STATUS		0x0124	DMA IRQ0 Status Register */
/*		Bit(s) ICU_DMA_IRQ0_STATUS_RSRV_31_20 reserved */
/* VDMA IRQ0 Status */
#define	ICU_DMA_IRQ0_STATUS_VDMA_IRQ0_STATUS_MSK			SHIFT18(0x3)	
#define	ICU_DMA_IRQ0_STATUS_VDMA_IRQ0_STATUS_BASE			22
/* ADMA IRQ0 Status */
#define	ICU_DMA_IRQ0_STATUS_ADMA_IRQ0_STATUS_MSK			SHIFT18(0xf)	
#define	ICU_DMA_IRQ0_STATUS_ADMA_IRQ0_STATUS_BASE			18
/* MDMA IRQ0 Status */
#define	ICU_DMA_IRQ0_STATUS_MDMA_IRQ0_STATUS_MSK			SHIFT16(0x3)	
#define	ICU_DMA_IRQ0_STATUS_MDMA_IRQ0_STATUS_BASE			16
/* PDMA IRQ0 Status */
#define	ICU_DMA_IRQ0_STATUS_PDMA_IRQ0_STATUS_MSK			SHIFT0(0xffff)	
#define	ICU_DMA_IRQ0_STATUS_PDMA_IRQ0_STATUS_BASE			0

/*	ICU_DMA_IRQ1_STATUS		0x0128	DMA IRQ1 Status Register */
/*		Bit(s) ICU_DMA_IRQ1_STATUS_RSRV_31_20 reserved */
/* VDMA IRQ1 Status */
#define	ICU_DMA_IRQ1_STATUS_VDMA_IRQ1_STATUS_MSK			SHIFT18(0x3)	
#define	ICU_DMA_IRQ1_STATUS_VDMA_IRQ1_STATUS_BASE			22
/* ADMA IRQ1 Status */
#define	ICU_DMA_IRQ1_STATUS_ADMA_IRQ1_STATUS_MSK			SHIFT18(0xf)	
#define	ICU_DMA_IRQ1_STATUS_ADMA_IRQ1_STATUS_BASE			18
/* MDMA IRQ1 Status */
#define	ICU_DMA_IRQ1_STATUS_MDMA_IRQ1_STATUS_MSK			SHIFT16(0x3)	
#define	ICU_DMA_IRQ1_STATUS_MDMA_IRQ1_STATUS_BASE			16
/* PDMA IRQ1 Status */
#define	ICU_DMA_IRQ1_STATUS_PDMA_IRQ1_STATUS_MSK			SHIFT0(0xffff)	
#define	ICU_DMA_IRQ1_STATUS_PDMA_IRQ1_STATUS_BASE			0

/*	ICU_DMA_IRQ2_STATUS		0x012C	DMA IRQ2 Status Register */
/*		Bit(s) ICU_DMA_IRQ2_STATUS_RSRV_31_20 reserved */
/* VDMA IRQ2 Status */
#define	ICU_DMA_IRQ2_STATUS_VDMA_IRQ2_STATUS_MSK			SHIFT18(0x3)	
#define	ICU_DMA_IRQ2_STATUS_VDMA_IRQ2_STATUS_BASE			22
/* ADMA IRQ2 Status */
#define	ICU_DMA_IRQ2_STATUS_ADMA_IRQ2_STATUS_MSK			SHIFT18(0xf)	
#define	ICU_DMA_IRQ2_STATUS_ADMA_IRQ2_STATUS_BASE			18
/* MDMA IRQ2 Status */
#define	ICU_DMA_IRQ2_STATUS_MDMA_IRQ2_STATUS_MSK			SHIFT16(0x3)	
#define	ICU_DMA_IRQ2_STATUS_MDMA_IRQ2_STATUS_BASE			16
/* PDMA IRQ2 Status */
#define	ICU_DMA_IRQ2_STATUS_PDMA_IRQ2_STATUS_MSK			SHIFT0(0xffff)	
#define	ICU_DMA_IRQ2_STATUS_PDMA_IRQ2_STATUS_BASE			0

/*	ICU_IRQ0_STATUS_0		0x0130	IRQ0 Status Register 0 */
/* IRQ0 Status [31:0] */
#define	ICU_IRQ0_STATUS_0_IRQ0_STATUS_0_MSK			SHIFT0(0xffffffff)	
#define	ICU_IRQ0_STATUS_0_IRQ0_STATUS_0_BASE		0

/*	ICU_IRQ0_STATUS_1		0x0134	IRQ0 Status Register 1 */
/* IRQ0 Status [63:32] */
#define	ICU_IRQ0_STATUS_1_IRQ0_STATUS_1_MSK			SHIFT0(0xffffffff)	
#define	ICU_IRQ0_STATUS_1_IRQ0_STATUS_1_BASE		0

/*	ICU_IRQ1_STATUS_0		0x0138	IRQ1 Status Register 0 */
/* IRQ1 Status [31:0] */
#define	ICU_IRQ1_STATUS_0_IRQ1_STATUS_0_MSK			SHIFT0(0xffffffff)	
#define	ICU_IRQ1_STATUS_0_IRQ1_STATUS_0_BASE		0

/*	ICU_IRQ1_STATUS_1		0x013C	IRQ1 Status Register 1 */
/* IRQ1 Status [63:32] */
#define	ICU_IRQ1_STATUS_1_IRQ1_STATUS_1_MSK			SHIFT0(0xffffffff)	
#define	ICU_IRQ1_STATUS_1_IRQ1_STATUS_1_BASE		0

/*	ICU_IRQ2_STATUS_0		0x0140	IRQ2 Status Register 0 */
/* IRQ2 Status [31:0] */
#define	ICU_IRQ2_STATUS_0_IRQ2_STATUS_0_MSK			SHIFT0(0xffffffff)	
#define	ICU_IRQ2_STATUS_0_IRQ2_STATUS_0_BASE		0

/*	ICU_IRQ2_STATUS_1		0x0144	IRQ2 Status Register 1 */
/* IRQ2 Status [63:32] */
#define	ICU_IRQ2_STATUS_1_IRQ2_STATUS_1_MSK			SHIFT0(0xffffffff)	
#define	ICU_IRQ2_STATUS_1_IRQ2_STATUS_1_BASE		0

// to add

/*	ICU_FIQ_0_TO_63_CONF	0x0200	FIQ 0 to 63 Configuration Registers */
/*		Bit(s) ICU_FIQ_0_TO_63_CONF_RSRV_31_5 reserved */
#define	ICU_FIQ_0_TO_63_CONF_FIQ0_EN			BIT_4				/* FIQ0_EN */
#define	ICU_FIQ_0_TO_63_CONF_PRIORITY_MSK		SHIFT0(0xf)			/* Priority */
#define	ICU_FIQ_0_TO_63_CONF_PRIORITY_BASE		0

/*	ICU_FIQ_SEL_INT_NUM		0x0300	FIQ0 Selected Interrupt Number Register */
/*		Bit(s) ICU_FIQ_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	ICU_FIQ_SEL_INT_NUM_INT_PENDING				BIT_6				
/* Selected Interrupt Number */
#define	ICU_FIQ_SEL_INT_NUM_SEL_INT_NUM_MSK			SHIFT0(0x3f)		
#define	ICU_FIQ_SEL_INT_NUM_SEL_INT_NUM_BASE		0

/*	ICU_GBL_FIQ_MSK			0x0304	Global FIQ0 Mask Register */
/*		Bit(s) ICU_GBL_FIQ_MSK_RSRV_31_1 reserved */
#define	ICU_GBL_FIQ_MSK_FIQ_MSK				BIT_0				/* Mask FIQ */

/*	ICU_DMA_FIQ_MSK			0x0308	DMA FIQ0 Mask Register */
/*		Bit(s) ICU_DMA_FIQ_MSK_RSRV_31_20 reserved */
/* PCIEDMA FIQ Mask */
#define	ICU_DMA_FIQ_MSK_PCIEDMA_FIQ_MSK_MSK			SHIFT18(0x3)	
#define	ICU_DMA_FIQ_BASE_PCIEDMA_FIQ_BASE_BASE		18
/* MDMA FIQ Mask */
#define	ICU_DMA_FIQ_MSK_MDMA_FIQ_MSK_MSK			SHIFT16(0x3)	
#define	ICU_DMA_FIQ_BASE_MDMA_FIQ_BASE_BASE			16
/* PDMA FIQ Mask */
#define	ICU_DMA_FIQ_MSK_PDMA_FIQ_MSK_MSK			SHIFT0(0xffff)	
#define	ICU_DMA_FIQ_BASE_PDMA_FIQ_BASE_BASE			0

/*	ICU_DMA_FIQ_STATUS		0x030C	DMA FIQ0 Status Register */
/*		Bit(s) ICU_DMA_FIQ_STATUS_RSRV_31_20 reserved */
/* PCIEDMA FIQ Status */
#define	ICU_DMA_FIQ_STATUS_PCIEDMA_FIQ_STATUS_MSK		SHIFT18(0x3)	
#define	ICU_DMA_FIQ_STATUS_PCIEDMA_FIQ_STATUS_BASE		18
/* MDMA FIQ Status */
#define	ICU_DMA_FIQ_STATUS_MDMA_FIQ_STATUS_MSK			SHIFT16(0x3)	
#define	ICU_DMA_FIQ_STATUS_MDMA_FIQ_STATUS_BASE			16
/* PDMA FIQ Status */
#define	ICU_DMA_FIQ_STATUS_PDMA_FIQ_STATUS_MSK			SHIFT0(0xffff)	
#define	ICU_DMA_FIQ_STATUS_PDMA_FIQ_STATUS_BASE			0

/*	ICU_FIQ_STATUS_0		0x0310	FIQ0 Status Register 0 */
/* FIQ Status [31:0] */
#define	ICU_FIQ_STATUS_0_FIQ_STATUS_0_MSK		SHIFT0(0xffffffff)	
#define	ICU_FIQ_STATUS_0_FIQ_STATUS_0_BASE		0

/*	ICU_FIQ_STATUS_1		0x0314	FIQ0 Status Register 1 */
/* FIQ Status [63:32] */
#define	ICU_FIQ_STATUS_1_FIQ_STATUS_1_MSK		SHIFT0(0xffffffff)	
#define	ICU_FIQ_STATUS_1_FIQ_STATUS_1_BASE		0

/* -------------------- */

#endif /*ICU_H_*/
