#ifndef __USIM_H__
#define __USIM_H__

typedef struct
{
    __I  uint32_t RBR    ; /* 0x0000 USIM Receive Buffer Register */  
    __O  uint32_t THR    ; /* 0x0004 USIM Transmit Holding Register */   
    __IO uint32_t IER    ; /* 0x0008 USIM Interrupt Enable Register */   
    __IO uint32_t IIR    ; /* 0x000C USIM Interrupt Identification Register */   
    __O  uint32_t FCR    ; /* 0x0010 USIM FIFO Control Register */   
    __I  uint32_t FSR    ; /* 0x0014 USIM FIFO Status Register */   
    __IO uint32_t ECR    ; /* 0x0018 USIM Error Control Register */   
    __IO uint32_t LCR    ; /* 0x001C USIM Line Control Register */   
    __IO uint32_t USCCR  ; /* 0x0020 USIM SmartCard Control Register */ 
    __I  uint32_t LSR    ; /* 0x0024 USIM Line Status Register */   
    __IO uint32_t EGTR   ; /* 0x0028 USIM Extra Guard Time Register */  
    __IO uint32_t BGTR   ; /* 0x002C USIM Block Guard Time Register */  
    __IO uint32_t TOR    ; /* 0x0030 USIM Time-Out Register */   
    __IO uint32_t CLKR   ; /* 0x0034 USIM Clock Register */  
    __IO uint32_t DLR    ; /* 0x0038 USIM Divisor Latch Register */   
    __IO uint32_t FLR    ; /* 0x003C USIM Factor Latch Register */   
    __IO uint32_t CWTR   ; /* 0x0040 USIM Character Waiting Time Register */  
    __IO uint32_t BWTR   ; /* 0x0044 USIM Block Waiting Time Register */             
}USIM_TypeDef;



#define    USIM1_BASE      0xD4032000
#define    USIM2_BASE      0xD4033000

#define    USIM1  (( USIM_TypeDef *) USIM1_BASE )
#define    USIM2  (( USIM_TypeDef *) USIM2_BASE )
/*
 *
 *	THE BIT DEFINES
 *
 */
/*	USIM_RBR	0x0000	USIM Receive Buffer Register */
/*		Bit(s) USIM_RBR_RSRV_31_9 reserved */
#define	RBR_PERR				BIT_8				/* Parity Error */
#define	RBR_RB_MSK				SHIFT0(0xff)		/* RB[7:0] */
#define	RBR_RB_BASE			0

/*	USIM_THR	0x0004	USIM Transmit Holding Register */
/*		Bit(s) USIM_THR_RSRV_31_8 reserved */
#define	THR_TB_MSK				SHIFT0(0xff)		/* TB[7:0] */
#define	THR_TB_BASE			0

/*	USIM_IER	0x0008	USIM Interrupt Enable Register */
/*		Bit(s) USIM_IER_RSRV_31_16 reserved */
/* DMA Transmitter Request Enable */
#define	IER_DMA_TX					BIT_15			
/* DMA Receiver Request Enable */
#define	IER_DMA_RX					BIT_14			
/* Enable Dma Receiver Requests In Event Of Time-out Situation */
#define	IER_DMA_TIME				BIT_13			
/*		Bit(s) IER_RSRV_12_11 reserved */
#define	IER_SMARTCARD_DET			BIT_10			/* SmartCard Detection */
/* Transmitter Data Refill Interrupt */
#define	IER_TDR					BIT_9			
/* Receiver Data Ready Interrupt */
#define	IER_RDR					BIT_8			
/*		Bit(s) USIM_IER_RSRV_7 reserved */
/* Block Waiting Time Interrupt */
#define	IER_BWT					BIT_6			
/* Character Waiting Time Interrupt */
#define	IER_CWT					BIT_5			
/* Receiver Time-Out Interrupt */
#define	IER_TIMEO					BIT_4			
#define	IER_FRAMERR				BIT_3			/* Framing Error Interrupt */
/* T=0 Error Interrupt Enable */
#define	IER_TOERR					BIT_2			
/* Parity Error Interrupt Enable */
#define	IER_PERR					BIT_1			
/* Receiver Data Overrun Interrupt Enable */
#define	IER_OVRN					BIT_0			

/*	USIM_IIR	0x000C	USIM Interrupt Identification Register */
/*		Bit(s) USIM_IIR_RSRV_31_11 reserved */
#define	IIR_SMARTCARD_DET			BIT_10				/* SmartCard Detection */
/* Transmitter Data Refill Interrupt */
#define	IIR_TDR					BIT_9				
/* Receiver Data Ready Interrupt */
#define	IIR_RDR					BIT_8				
/*		Bit(s) USIM_IIR_RSRV_7 reserved */
/* Block Waiting Time Interrupt */
#define	IIR_BWT					BIT_6				
/* Character Waiting Time Interrupt */
#define	IIR_CWT					BIT_5				
/* Receiver Time Out Interrupt */
#define	IIR_TIMEO					BIT_4				
/* Framing Error Interrupt */
#define	IIR_FRAMERR				BIT_3				
#define	IIR_TOERR					BIT_2				/* T=0 Error Interrupt */
/* Parity Error Interrupt */
#define	IIR_PERR					BIT_1				
/* Receiver Data Overrun Interrupt */
#define	IIR_OVRN					BIT_0				

/*	USIM_FCR	0x0010	USIM FIFO Control Register */
/*		Bit(s) USIM_FCR_RSRV_31_9 reserved */
/* Transmitter Interrupt Trigger Level (Threshold) */
#define	FCR_TX_TL				BIT_8				
/* Receiver Interrupt Trigger Level (Threshold) */
#define	FCR_RX_TL_MSK			SHIFT6(0x3)			
#define	FCR_RX_TL_BASE			6
/*		Bit(s) USIM_FCR_RSRV_5_4 reserved */
#define	FCR_PEM				BIT_3				/* Parity Error Mask */
#define	FCR_TX_HOLD			BIT_2				/* Transmit Hold */
#define	FCR_RESETTF			BIT_1				/* Reset TXFIFO */
#define	FCR_RESETRF			BIT_0				/* Reset RXFIFO */

/*	USIM_FSR	0x0014	USIM FIFO Status Register */
/*		Bit(s) USIM_FSR_RSRV_31_15 reserved */
#define	FSR_PERR_NUM_MSK			SHIFT10(0x1f)		/* Parity Error Number */
#define	FSR_PERR_NUM_BASE			10
#define	FSR_TX_LENGTH_MSK			SHIFT5(0x1f)		/* TXFIFO Length */
#define	FSR_TX_LENGTH_BASE			5
#define	FSR_RX_LENGTH_MSK			SHIFT0(0x1f)		/* RXFIFO Length */
#define	FSR_RX_LENGTH_BASE			0

/*	USIM_ECR	0x0018	USIM Error Control Register */
/*		Bit(s) USIM_ECR_RSRV_31_8 reserved */
/* Repeat Character Transmission */
#define	ECR_T0_REPEAT			BIT_7				
#define	ECR_T0_CLR				BIT_6				/* Clear T=0 Error */
/*		Bit(s) USIM_ECR_RSRV_5 reserved */
/* Parity Error Trigger Level */
#define	ECR_PE_TL_MSK			SHIFT3(0x3)			
#define	ECR_PE_TL_BASE			3
/*		Bit(s) USIM_ECR_RSRV_2 reserved */
/* T=0 Error Trigger Level (Threshold) */
#define	ECR_T0ERR_TL_MSK		SHIFT0(0x3)			
#define	ECR_T0ERR_TL_BASE		0

/*	USIM_LCR	0x001C	USIM Line Control Register */
/*		Bit(s) USIM_LCR_RSRV_31_5 reserved */
#define	LCR_TX_T1				BIT_4				/* Transmitter Protocol */
#define	LCR_RX_T1				BIT_3				/* Receiver Protocol */
#define	LCR_EPS				BIT_2				/* Even Parity Select (EPS) */
/* Transmit/Receive Bit Order */
#define	LCR_ORDER				BIT_1				
#define	LCR_INVERSE			BIT_0				/* Bit inversion */

/*	USIM_USCCR	0x0020	USIM SmartCard Control Register */
/*		Bit(s) USIM_USCCR_RSRV_31_5 reserved */
#define	USCCR_TXD_FORCE			BIT_4				/* Force TXD */
/*		Bit(s) USIM_USCCR_RSRV_3 reserved */
#define	USCCR_VCC_MSK				SHIFT1(0x3)			/* SmartCard Voltage */
#define	USCCR_VCC_BASE				1
#define	USCCR_RST_SMARTCARD_N		BIT_0				/* SmartCard Reset */

/*	USIM_LSR	0x0024	USIM Line Status Register */
/*		Bit(s) USIM_LSR_RSRV_31_16 reserved */
/* Reflects Sampled Data from UIO Pin */
#define	LSR_RXD					BIT_15			
#define	LSR_RX_WORKING				BIT_14			/* Receiver Working */
#define	LSR_TX_WORKING				BIT_13			/* Transmitter Working */
#define	LSR_RX_EMPTY_N				BIT_12			/* RXFIFO Not Empty */
#define	LSR_TDR					BIT_11			/* Transmitter Data Refill */
/*		Bit(s) USIM_LSR_RSRV_10_7 reserved */
#define	LSR_BWT					BIT_6			/* Block Waiting Time */
#define	LSR_CWT					BIT_5			/* Character Waiting Time */
#define	LSR_TIMEO					BIT_4			/* Receiver Time Out */
#define	LSR_FRAMERR				BIT_3			/* Framing Error */
#define	LSR_TOERR					BIT_2			/* T=0 Error */
#define	LSR_PERR					BIT_1			/* Parity Error */
/* Receiver Data Overrun Error */
#define	LSR_OVRN					BIT_0			

/*	USIM_EGTR	0x0028	USIM Extra Guard Time Register */
/*		Bit(s) USIM_EGTR_RSRV_31_8 reserved */
/* Extra Guard Time Moments */
#define	EGTR_EGTM_MSK				SHIFT0(0xff)		
#define	EGTR_EGTM_BASE				0

/*	USIM_BGTR	0x002C	USIM Block Guard Time Register */
/*		Bit(s) USIM_BGTR_RSRV_31_8 reserved */
#define	BGTR_BGT_MSK				SHIFT0(0xff)		/* Block Guard Time */
#define	BGTR_BGT_BASE				0

/*	USIM_TOR	0x0030	USIM Time-Out Register */
/*		Bit(s) USIM_TOR_RSRV_31_8 reserved */
#define	TOR_TO_MSK				SHIFT0(0xff)		/* Time Out */
#define	TOR_TO_BASE			0

/*	USIM_CLKR	0x0034	USIM Clock Register */
/*		Bit(s) USIM_CLKR_RSRV_31_16 reserved */
/* Stop USIM Controller Internal Clock */
#define	CLKR_STOP_CLK_USIM			BIT_15			
#define	CLKR_STOP_LEVEL			BIT_14			/* Stop Level */
#define	CLKR_STOP_UCLK				BIT_13			/* Stop SmartCard Clock */
#define	CLKR_RQST					BIT_12			/* Clock Change Request */
/*		Bit(s) USIM_CLKR_RSRV_11_8 reserved */
#define	CLKR_DIVISOR_MSK			SHIFT0(0xff)	/* Clock Divisor */
#define	CLKR_DIVISOR_BASE			0

/*	USIM_DLR	0x0038	USIM Divisor Latch Register */
/*		Bit(s) USIM_DLR_RSRV_31_16 reserved */
#define	DLR_DIVISOR_MSK			SHIFT0(0xffff)	/* Baud Divisor */
#define	DLR_DIVISOR_BASE			0

/*	USIM_FLR	0x003C	USIM Factor Latch Register */
/*		Bit(s) USIM_FLR_RSRV_31_8 reserved */
#define	FLR_FACTOR_MSK			SHIFT0(0xff)		/* Baud Factor */
#define	FLR_FACTOR_BASE		0

/*	USIM_CWTR	0x0040	USIM Character Waiting Time Register */
/*		Bit(s) USIM_CWTR_RSRV_31_16 reserved */
#define	CWTR_CWT_MSK				SHIFT0(0xffff)	/* Character Waiting Time */
#define	CWTR_CWT_BASE				0

/*	USIM_BWTR	0x0044	USIM Block Waiting Time Register */
/*		Bit(s) USIM_BWTR_RSRV_31_16 reserved */
#define	BWTR_BWT_MSK				SHIFT0(0xffff)	/* Block Waiting Time */
#define	BWTR_BWT_BASE				0

#endif
