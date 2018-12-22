#ifndef ICU2_H_
#define ICU2_H_

/*
 *
 *	THE BASE ADDRESSES
 *
 */
#define	ICU2_BASE	0xD4284000

/*
 *
 *	THE REGISTER DEFINES
 *
 */
#define	ICU2_IRQ_0_TO_63_CONF	(ICU2_BASE+0x0000)	/* IRQ 0 to 63 Configuration
													 *			Registers
													 *			Start
													 */
#define	ICU2_IRQ3_SEL_INT_NUM	(ICU2_BASE+0x0100)	/* 32 bit	IRQ3 Selected
													 *			Interrupt
													 *			Number
													 *			Register
													 */
#define	ICU2_IRQ4_SEL_INT_NUM	(ICU2_BASE+0x0104)	/* 32 bit	IRQ4 Selected
													 *			Interrupt
													 *			Number
													 *			Register
													 */
#define	ICU2_IRQ5_SEL_INT_NUM	(ICU2_BASE+0x0108)	/* 32 bit	IRQ5 Selected
													 *			Interrupt
													 *			Number
													 *			Register
													 */
#define	ICU2_GBL_IRQ3_MSK		(ICU2_BASE+0x010C)	/* 32 bit	Global IRQ3
													 *			Mask Register
													 */
#define	ICU2_GBL_IRQ4_MSK		(ICU2_BASE+0x0110)	/* 32 bit	Global IRQ4
													 *			Mask Register
													 */
#define	ICU2_GBL_IRQ5_MSK		(ICU2_BASE+0x0114)	/* 32 bit	Global IRQ5
													 *			Mask Register
													 */
#define	ICU2_DMA_IRQ3_MSK		(ICU2_BASE+0x0118)	/* 32 bit	DMA IRQ3 Mask
													 *			Register
													 */
#define	ICU2_DMA_IRQ4_MSK		(ICU2_BASE+0x011C)	/* 32 bit	DMA IRQ4 Mask
													 *			Register
													 */
#define	ICU2_DMA_IRQ5_MSK		(ICU2_BASE+0x0120)	/* 32 bit	DMA IRQ5 Mask
													 *			Register
													 */
#define	ICU2_DMA_IRQ3_STATUS	(ICU2_BASE+0x0124)	/* 32 bit	DMA IRQ3 Status
													 *			Register
													 */
#define	ICU2_DMA_IRQ4_STATUS	(ICU2_BASE+0x0128)	/* 32 bit	DMA IRQ4 Status
													 *			Register
													 */
#define	ICU2_DMA_IRQ5_STATUS	(ICU2_BASE+0x012C)	/* 32 bit	DMA IRQ5 Status
													 *			Register
													 */
#define	ICU2_IRQ3_STATUS_0		(ICU2_BASE+0x0130)	/* 32 bit	IRQ3 Status
													 *			Register 0 
													 */
#define	ICU2_IRQ3_STATUS_1		(ICU2_BASE+0x0134)	/* 32 bit	IRQ3 Status
													 *			Register 1 
													 */
#define	ICU2_IRQ4_STATUS_0		(ICU2_BASE+0x0138)	/* 32 bit	IRQ4 Status
													 *			Register 0 
													 */
#define	ICU2_IRQ4_STATUS_1		(ICU2_BASE+0x013C)	/* 32 bit	IRQ4 Status
													 *			Register 1 
													 */
#define	ICU2_IRQ5_STATUS_0		(ICU2_BASE+0x0140)	/* 32 bit	IRQ5 Status
													 *			Register 0 
													 */
#define	ICU2_IRQ5_STATUS_1		(ICU2_BASE+0x0144)	/* 32 bit	IRQ5 Status
													 *			Register 1 
													 */
#define	ICU2_IRQ6_SEL_INT_NUM	(ICU2_BASE+0x018C)	/* 32 bit	IRQ6 Selected
													 *			Interrupt
													 *			Number
													 *			Register
													 */
#define	ICU2_GBL_IRQ6_MSK		(ICU2_BASE+0x0190)	/* 32 bit	Global IRQ6
													 *			Mask Register
													 */
#define	ICU2_DMA_IRQ6_MSK		(ICU2_BASE+0x0194)	/* 32 bit	DMA IRQ6 Mask
													 *			Register
													 */	
#define	ICU2_DMA_IRQ6_STATUS	(ICU2_BASE+0x0198)	/* 32 bit	DMA IRQ6 Status
													 *			Register
													 */
#define	ICU2_IRQ6_STATUS_0		(ICU2_BASE+0x019C)	/* 32 bit	IRQ6 Status
													 *			Register 0 
													 */
#define	ICU2_IRQ6_STATUS_1		(ICU2_BASE+0x01A0)	/* 32 bit	IRQ6 Status
													 *			Register 1 
													 */
													 
/*
 *
 *	THE BIT DEFINES
 *
 */
/*	ICU2_IRQ_0_TO_63_CONF	0x0000	IRQ 0 to 63 Configuration Registers */
/*		Bit(s) ICU2_IRQ_0_TO_63_CONF_RSRV_31_9 reserved */
#define	ICU2_IRQ_0_TO_63_CONF_IRQ6_EN			BIT_7				/* IRQ6_EN */
#define	ICU2_IRQ_0_TO_63_CONF_IRQ5_EN			BIT_6				/* IRQ5_EN */
#define	ICU2_IRQ_0_TO_63_CONF_IRQ4_EN			BIT_5				/* IRQ4_EN */
#define	ICU2_IRQ_0_TO_63_CONF_IRQ3_EN			BIT_4				/* IRQ3_EN */
#define	ICU2_IRQ_0_TO_63_CONF_PRIORITY_MSK		SHIFT0(0xf)			/* Priority */
#define	ICU2_IRQ_0_TO_63_CONF_PRIORITY_BASE		0

/*	ICU2_IRQ3_SEL_INT_NUM	0x0100	IRQ3 Selected Interrupt Number Register */
/*		Bit(s) ICU2_IRQ3_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	ICU2_IRQ3_SEL_INT_NUM_INT_PENDING			BIT_6				
/* Selected Interrupt Number */
#define	ICU2_IRQ3_SEL_INT_NUM_SEL_INT_NUM_MSK		SHIFT0(0x3f)		
#define	ICU2_IRQ3_SEL_INT_NUM_SEL_INT_NUM_BASE		0

/*	ICU2_IRQ4_SEL_INT_NUM	0x0104	IRQ4 Selected Interrupt Number Register */
/*		Bit(s) ICU2_IRQ4_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	ICU2_IRQ4_SEL_INT_NUM_INT_PENDING			BIT_6				
/* Selected Interrupt Number */
#define	ICU2_IRQ4_SEL_INT_NUM_SEL_INT_NUM_MSK		SHIFT0(0x3f)		
#define	ICU2_IRQ4_SEL_INT_NUM_SEL_INT_NUM_BASE		0

/*	ICU2_IRQ5_SEL_INT_NUM	0x0108	IRQ5 Selected Interrupt Number Register */
/*		Bit(s) ICU2_IRQ5_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	ICU2_IRQ5_SEL_INT_NUM_INT_PENDING			BIT_6				
/* Selected Interrupt Number */
#define	ICU2_IRQ5_SEL_INT_NUM_SEL_INT_NUM_MSK		SHIFT0(0x3f)		
#define	ICU2_IRQ5_SEL_INT_NUM_SEL_INT_NUM_BASE		0

/*	ICU2_GBL_IRQ3_MSK		0x010C	Global IRQ3 Mask Register */
/*		Bit(s) ICU2_GBL_IRQ3_MSK_RSRV_31_1 reserved */
#define	ICU2_GBL_IRQ3_MSK_IRQ3_MSK				BIT_0				/* Mask IRQ3 */

/*	ICU2_GBL_IRQ4_MSK		0x0110	Global IRQ4 Mask Register */
/*		Bit(s) ICU2_GBL_IRQ4_MSK_RSRV_31_1 reserved */
#define	ICU2_GBL_IRQ4_MSK_IRQ4_MSK				BIT_0				/* Mask IRQ4 */

/*	ICU2_GBL_IRQ5_MSK		0x0114	Global IRQ5 Mask Register */
/*		Bit(s) ICU2_GBL_IRQ5_MSK_RSRV_31_1 reserved */
#define	ICU2_GBL_IRQ5_MSK_IRQ5_MSK				BIT_0				/* Mask IRQ5 */													 												 											 													 															 													 												 													 													 													 

/*	ICU2_DMA_IRQ3_MSK		0x0118	DMA IRQ3 Mask Register */
/*		Bit(s) ICU2_DMA_IRQ3_MSK_RSRV_31_20 reserved */
/* VDMA IRQ3 Mask */
#define	ICU2_DMA_IRQ3_MSK_VDMA_IRQ3_MSK_MSK				SHIFT22(0x3)	
#define	ICU2_DMA_IRQ3_BASE_VDMA_IRQ3_BASE_BASE			22
/* ADMA IRQ3 Mask */
#define	ICU2_DMA_IRQ3_MSK_ADMA_IRQ3_MSK_MSK				SHIFT18(0xf)	
#define	ICU2_DMA_IRQ3_BASE_ADMA_IRQ3_BASE_BASE			18
/* MDMA IRQ3 Mask */
#define	ICU2_DMA_IRQ3_MSK_MDMA_IRQ3_MSK_MSK				SHIFT16(0x3)	
#define	ICU2_DMA_IRQ3_BASE_MDMA_IRQ3_BASE_BASE			16
/* PDMA IRQ3 Mask */
#define	ICU2_DMA_IRQ3_MSK_PDMA_IRQ3_MSK_MSK				SHIFT0(0xffff)	
#define	ICU2_DMA_IRQ3_BASE_PDMA_IRQ3_BASE_BASE			0

/*	ICU2_DMA_IRQ4_MSK		0x011C	DMA IRQ4 Mask Register */
/*		Bit(s) ICU2_DMA_IRQ4_MSK_RSRV_31_20 reserved */
/* VDMA IRQ4 Mask */
#define	ICU2_DMA_IRQ4_MSK_VDMA_IRQ4_MSK_MSK				SHIFT22(0x3)	
#define	ICU2_DMA_IRQ4_BASE_VDMA_IRQ4_BASE_BASE			22
/* ADMA IRQ4 Mask */
#define	ICU2_DMA_IRQ4_MSK_ADMA_IRQ4_MSK_MSK				SHIFT18(0xf)	
#define	ICU2_DMA_IRQ4_BASE_ADMA_IRQ4_BASE_BASE			18
/* MDMA IRQ4 Mask */
#define	ICU2_DMA_IRQ4_MSK_MDMA_IRQ4_MSK_MSK				SHIFT16(0x3)	
#define	ICU2_DMA_IRQ4_BASE_MDMA_IRQ4_BASE_BASE			16
/* PDMA IRQ4 Mask */
#define	ICU2_DMA_IRQ4_MSK_PDMA_IRQ4_MSK_MSK				SHIFT0(0xffff)	
#define	ICU2_DMA_IRQ4_BASE_PDMA_IRQ4_BASE_BASE			0

/*	ICU2_DMA_IRQ5_MSK		0x0120	DMA IRQ5 Mask Register */
/*		Bit(s) ICU2_DMA_IRQ5_MSK_RSRV_31_20 reserved */
/* VDMA IRQ5 Mask */
#define	ICU2_DMA_IRQ5_MSK_VDMA_IRQ5_MSK_MSK				SHIFT22(0x3)	
#define	ICU2_DMA_IRQ5_BASE_VDMA_IRQ5_BASE_BASE			22
/* ADMA IRQ5 Mask */
#define	ICU2_DMA_IRQ5_MSK_ADMA_IRQ5_MSK_MSK				SHIFT18(0xf)	
#define	ICU2_DMA_IRQ5_BASE_ADMA_IRQ5_BASE_BASE			18
/* MDMA IRQ5 Mask */
#define	ICU2_DMA_IRQ5_MSK_MDMA_IRQ5_MSK_MSK				SHIFT16(0x3)	
#define	ICU2_DMA_IRQ5_BASE_MDMA_IRQ5_BASE_BASE			16
/* PDMA IRQ5 Mask */
#define	ICU2_DMA_IRQ5_MSK_PDMA_IRQ5_MSK_MSK				SHIFT0(0xffff)	
#define	ICU2_DMA_IRQ5_BASE_PDMA_IRQ5_BASE_BASE			0

/*	ICU2_DMA_IRQ3_STATUS		0x0124	DMA IRQ3 Status Register */
/*		Bit(s) ICU2_DMA_IRQ3_STATUS_RSRV_31_20 reserved */
/* VDMA IRQ3 Status */
#define	ICU2_DMA_IRQ3_STATUS_VDMA_IRQ3_STATUS_MSK			SHIFT18(0x3)	
#define	ICU2_DMA_IRQ3_STATUS_VDMA_IRQ3_STATUS_BASE			22
/* ADMA IRQ3 Status */
#define	ICU2_DMA_IRQ3_STATUS_ADMA_IRQ3_STATUS_MSK			SHIFT18(0xf)	
#define	ICU2_DMA_IRQ3_STATUS_ADMA_IRQ3_STATUS_BASE			18
/* MDMA IRQ3 Status */
#define	ICU2_DMA_IRQ3_STATUS_MDMA_IRQ3_STATUS_MSK			SHIFT16(0x3)	
#define	ICU2_DMA_IRQ3_STATUS_MDMA_IRQ3_STATUS_BASE			16
/* PDMA IRQ3 Status */
#define	ICU2_DMA_IRQ3_STATUS_PDMA_IRQ3_STATUS_MSK			SHIFT0(0xffff)	
#define	ICU2_DMA_IRQ3_STATUS_PDMA_IRQ3_STATUS_BASE			0

/*	ICU2_DMA_IRQ4_STATUS		0x0128	DMA IRQ4 Status Register */
/*		Bit(s) ICU2_DMA_IRQ4_STATUS_RSRV_31_20 reserved */
/* VDMA IRQ4 Status */
#define	ICU2_DMA_IRQ4_STATUS_VDMA_IRQ4_STATUS_MSK			SHIFT18(0x3)	
#define	ICU2_DMA_IRQ4_STATUS_VDMA_IRQ4_STATUS_BASE			22
/* ADMA IRQ4 Status */
#define	ICU2_DMA_IRQ4_STATUS_ADMA_IRQ4_STATUS_MSK			SHIFT18(0xf)	
#define	ICU2_DMA_IRQ4_STATUS_ADMA_IRQ4_STATUS_BASE			18
/* MDMA IRQ4 Status */
#define	ICU2_DMA_IRQ4_STATUS_MDMA_IRQ4_STATUS_MSK			SHIFT16(0x3)	
#define	ICU2_DMA_IRQ4_STATUS_MDMA_IRQ4_STATUS_BASE			16
/* PDMA IRQ4 Status */
#define	ICU2_DMA_IRQ4_STATUS_PDMA_IRQ4_STATUS_MSK			SHIFT0(0xffff)	
#define	ICU2_DMA_IRQ4_STATUS_PDMA_IRQ4_STATUS_BASE			0

/*	ICU2_DMA_IRQ5_STATUS		0x012C	DMA IRQ5 Status Register */
/*		Bit(s) ICU2_DMA_IRQ5_STATUS_RSRV_31_20 reserved */
/* VDMA IRQ5 Status */
#define	ICU2_DMA_IRQ5_STATUS_VDMA_IRQ5_STATUS_MSK			SHIFT18(0x3)	
#define	ICU2_DMA_IRQ5_STATUS_VDMA_IRQ5_STATUS_BASE			22
/* ADMA IRQ5 Status */
#define	ICU2_DMA_IRQ5_STATUS_ADMA_IRQ5_STATUS_MSK			SHIFT18(0xf)	
#define	ICU2_DMA_IRQ5_STATUS_ADMA_IRQ5_STATUS_BASE			18
/* MDMA IRQ5 Status */
#define	ICU2_DMA_IRQ5_STATUS_MDMA_IRQ5_STATUS_MSK			SHIFT16(0x3)	
#define	ICU2_DMA_IRQ5_STATUS_MDMA_IRQ5_STATUS_BASE			16
/* PDMA IRQ5 Status */
#define	ICU2_DMA_IRQ5_STATUS_PDMA_IRQ5_STATUS_MSK			SHIFT0(0xffff)	
#define	ICU2_DMA_IRQ5_STATUS_PDMA_IRQ5_STATUS_BASE			0

/*	ICU2_IRQ3_STATUS_0		0x0130	IRQ3 Status Register 0 */
/* IRQ3 Status [31:0] */
#define	ICU2_IRQ3_STATUS_0_IRQ3_STATUS_0_MSK			SHIFT0(0xffffffff)	
#define	ICU2_IRQ3_STATUS_0_IRQ3_STATUS_0_BASE		0

/*	ICU2_IRQ3_STATUS_1		0x0134	IRQ3 Status Register 1 */
/* IRQ3 Status [63:32] */
#define	ICU2_IRQ3_STATUS_1_IRQ3_STATUS_1_MSK			SHIFT0(0xffffffff)	
#define	ICU2_IRQ3_STATUS_1_IRQ3_STATUS_1_BASE		0

/*	ICU2_IRQ4_STATUS_0		0x0138	IRQ4 Status Register 0 */
/* IRQ4 Status [31:0] */
#define	ICU2_IRQ4_STATUS_0_IRQ4_STATUS_0_MSK			SHIFT0(0xffffffff)	
#define	ICU2_IRQ4_STATUS_0_IRQ4_STATUS_0_BASE		0

/*	ICU2_IRQ4_STATUS_1		0x013C	IRQ4 Status Register 1 */
/* IRQ4 Status [63:32] */
#define	ICU2_IRQ4_STATUS_1_IRQ4_STATUS_1_MSK			SHIFT0(0xffffffff)	
#define	ICU2_IRQ4_STATUS_1_IRQ4_STATUS_1_BASE		0

/*	ICU2_IRQ5_STATUS_0		0x0140	IRQ5 Status Register 0 */
/* IRQ5 Status [31:0] */
#define	ICU2_IRQ5_STATUS_0_IRQ5_STATUS_0_MSK			SHIFT0(0xffffffff)	
#define	ICU2_IRQ5_STATUS_0_IRQ5_STATUS_0_BASE		0

/*	ICU2_IRQ5_STATUS_1		0x0144	IRQ5 Status Register 1 */
/* IRQ5 Status [63:32] */
#define	ICU2_IRQ5_STATUS_1_IRQ5_STATUS_1_MSK			SHIFT0(0xffffffff)	
#define	ICU2_IRQ5_STATUS_1_IRQ5_STATUS_1_BASE		0

/*	ICU2_IRQ6_SEL_INT_NUM	0x018C	IRQ6 Selected Interrupt Number Register */
/*		Bit(s) ICU2_IRQ6_SEL_INT_NUM_RSRV_31_7 reserved */
/* Interrupt Pending */
#define	ICU2_IRQ6_SEL_INT_NUM_INT_PENDING			BIT_6				
/* Selected Interrupt Number */
#define	ICU2_IRQ6_SEL_INT_NUM_SEL_INT_NUM_MSK		SHIFT0(0x3f)		
#define	ICU2_IRQ6_SEL_INT_NUM_SEL_INT_NUM_BASE		0

/*	ICU2_GBL_IRQ6_MSK		0x0190	Global IRQ6 Mask Register */
/*		Bit(s) ICU2_GBL_IRQ6_MSK_RSRV_31_1 reserved */
#define	ICU2_GBL_IRQ6_MSK_IRQ6_MSK				BIT_0				/* Mask IRQ6 */

/*	ICU2_DMA_IRQ6_MSK		0x0194	DMA IRQ6 Mask Register */
/*		Bit(s) ICU2_DMA_IRQ6_MSK_RSRV_31_20 reserved */
/* VDMA IRQ6 Mask */
#define	ICU2_DMA_IRQ6_MSK_VDMA_IRQ6_MSK_MSK				SHIFT22(0x3)	
#define	ICU2_DMA_IRQ6_BASE_VDMA_IRQ6_BASE_BASE			22
/* ADMA IRQ6 Mask */
#define	ICU2_DMA_IRQ6_MSK_ADMA_IRQ6_MSK_MSK				SHIFT18(0xf)	
#define	ICU2_DMA_IRQ6_BASE_ADMA_IRQ6_BASE_BASE			18
/* MDMA IRQ6 Mask */
#define	ICU2_DMA_IRQ6_MSK_MDMA_IRQ6_MSK_MSK				SHIFT16(0x3)	
#define	ICU2_DMA_IRQ6_BASE_MDMA_IRQ6_BASE_BASE			16
/* PDMA IRQ6 Mask */
#define	ICU2_DMA_IRQ6_MSK_PDMA_IRQ6_MSK_MSK				SHIFT0(0xffff)	
#define	ICU2_DMA_IRQ6_BASE_PDMA_IRQ6_BASE_BASE			0

/*	ICU2_DMA_IRQ6_STATUS		0x0198	DMA IRQ6 Status Register */
/*		Bit(s) ICU2_DMA_IRQ6_STATUS_RSRV_31_20 reserved */
/* VDMA IRQ6 Status */
#define	ICU2_DMA_IRQ6_STATUS_VDMA_IRQ6_STATUS_MSK			SHIFT18(0x3)	
#define	ICU2_DMA_IRQ6_STATUS_VDMA_IRQ6_STATUS_BASE			22
/* ADMA IRQ6 Status */
#define	ICU2_DMA_IRQ6_STATUS_ADMA_IRQ6_STATUS_MSK			SHIFT18(0xf)	
#define	ICU2_DMA_IRQ6_STATUS_ADMA_IRQ6_STATUS_BASE			18
/* MDMA IRQ6 Status */
#define	ICU2_DMA_IRQ6_STATUS_MDMA_IRQ6_STATUS_MSK			SHIFT16(0x3)	
#define	ICU2_DMA_IRQ6_STATUS_MDMA_IRQ6_STATUS_BASE			16
/* PDMA IRQ6 Status */
#define	ICU2_DMA_IRQ6_STATUS_PDMA_IRQ6_STATUS_MSK			SHIFT0(0xffff)	
#define	ICU2_DMA_IRQ6_STATUS_PDMA_IRQ6_STATUS_BASE			0
			
/*	ICU2_IRQ6_STATUS_0		0x019C	IRQ6 Status Register 0 */
/* IRQ6 Status [31:0] */
#define	ICU2_IRQ6_STATUS_0_IRQ6_STATUS_0_MSK			SHIFT0(0xffffffff)	
#define	ICU2_IRQ6_STATUS_0_IRQ6_STATUS_0_BASE		0

/*	ICU2_IRQ6_STATUS_1		0x01A0	IRQ6 Status Register 1 */
/* IRQ6 Status [63:32] */
#define	ICU2_IRQ6_STATUS_1_IRQ6_STATUS_1_MSK			SHIFT0(0xffffffff)	
#define	ICU2_IRQ6_STATUS_1_IRQ6_STATUS_1_BASE		0
													 
#endif /*ICU2_H_*/
