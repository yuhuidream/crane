#ifndef __UART_H__
#define __UART_H__

typedef struct
{
    union
    {
        __I  uint32_t RBR     ; /* 0x0000 offset Receive Buffer Register (for LCR[DLAB]=0, read-only) */     
        __O  uint32_t THR     ; /* 0x0000 offsetTransmit Holding Register (for LCR[DLAB]=0, writeonly) */   
        __IO uint32_t DLL     ; /* 0x0000 offset Divisor Latch Low Byte Register (LCR[DLAB]=1) */    
    };        
    union
    {
        __IO uint32_t DLH     ; /* 0x0004 offset Divisor Latch High Byte Register (LCR[DLAB]=1) */           
        __IO uint32_t IER     ; /* 0x0004 offset Interrupt Enable Register (LCR[DLAB]=0) */                  
    };
    union
    {
        __I  uint32_t IIR     ; /* 0x0008 offset Interrupt Identification Register (read-only) */            
        __O  uint32_t FCR     ; /* 0x0008 offset FIFO Control Register (write-only) */                       
    };
    __IO uint32_t LCR     ; /* 0x000C offset Line Control Register */                      
    __IO uint32_t MCR     ; /* 0x0010 offset Modem Control Register */                     
    __IO uint32_t LSR     ; /* 0x0014 offset Line Status Register */                       
    __IO uint32_t MSR     ; /* 0x0018 offset Modem Status Register */                      
    __IO uint32_t SCR     ; /* 0x001C offset Scratchpad Register */                        
    __IO uint32_t ISR     ; /* 0x0020 offset Infrared Selection Register */                
    __IO uint32_t FOR     ; /* 0x0024 offset Receive FIFO Occupancy Register */            
    __IO uint32_t ABR     ; /* 0x0028 offset Auto-baud Control Register */                 
    __IO uint32_t ACR     ; /* 0x002C offset Auto-baud Count Register */                   
}UART_TypeDef;


#define    UART0_BASE      0xD4036000   //cp uart
#define    UART1_BASE      0xD4017000


#define    UART0  (( UART_TypeDef *) UART0_BASE )
#define    UART1  (( UART_TypeDef *) UART1_BASE )

#if defined(Q_SPY)&&defined(Q_SPY_EXT)
#define    QSPY_UART     UART0
#endif
#if defined(CONFIG_CPUART)
#define    DBG_UART      UART0
#else
#define    DBG_UART      UART1
#endif
/*
 *
 *	THE BIT DEFINES
 *
 */
/*	UART_RBR	0x0000	Receive Buffer Register */
#define	RBR_BYTE_3_MSK			SHIFT24(0xff)	/* Byte 3 */
#define	RBR_BYTE_3_BASE		24
#define	RBR_BYTE_1_MSK			SHIFT8(0xff)	/* Byte 1 */
#define	RBR_BYTE_1_BASE		8
#define	RBR_BYTE_0_MSK			SHIFT0(0xff)	/* Byte 0 */
#define	RBR_BYTE_0_BASE		0

/*	UART_THR	0x0000	Transmit Holding Register */
#define	THR_BYTE_3_MSK			SHIFT24(0xff)	/* Byte 3 */
#define	THR_BYTE_3_BASE		24
#define	THR_BYTE_1_MSK			SHIFT8(0xff)	/* Byte 1 */
#define	THR_BYTE_1_BASE		8
#define	THR_BYTE_0_MSK			SHIFT0(0xff)	/* Byte 0 */
#define	THR_BYTE_0_BASE		0

/*	UART_DLL	0x0000	Divisor Latch Low Byte Register */
/*		Bit(s) UART_DLL_RSRV_31_8 reserved */
#define	DLL_DLL_MSK			SHIFT0(0xff)		/* Divisor Latch Low */
#define	DLL_DLL_BASE			0

/*	UART_DLH	0x0004	Divisor Latch High Byte Register */
/*		Bit(s) UART_DLH_RSRV_31_8 reserved */
#define	DLH_DLH_MSK			SHIFT0(0xff)		/* Divisor Latch High */
#define	DLH_DLH_BASE			0

/*	UART_IER	0x0004	Interrupt Enable Register */
#define	IER_HSE				BIT_8				/* High Speed UART Enable */
#define	IER_DMAE				BIT_7				/* DMA Requests Enable */
#define	IER_UUE				BIT_6				/* UART Unit Enable */
#define	IER_NRZE				BIT_5				/* NRZ Coding Enable */
/* Receiver Time-out Interrupt Enable */
#define	IER_RTOIE				BIT_4				
#define	IER_MIE				BIT_3				/* Modem Interrupt Enable */
/* Receiver Line Status Interrupt Enable */
#define	IER_RLSE				BIT_2				
/* Transmit Data Request Interrupt Enable */
#define	IER_TIE				BIT_1				
/* Receiver Data Available Interrupt Enable */
#define	IER_RAVIE				BIT_0				

/*	UART_IIR	0x0008	Interrupt Identification Register */
/*		Bit(s) UART_IIR_RSRV_31_8 reserved */
#define	IIR_FIFOES10_MSK		SHIFT6(0x3)			/* FIFO Mode Enable Status */
#define	IIR_FIFOES10_BASE		6
/* DMA End of Descriptor Chain */
#define	IIR_EOC				BIT_5				
#define	IIR_ABL				BIT_4				/* Auto-baud Lock */
#define	IIR_TOD				BIT_3				/* Time Out Detected */
#define	IIR_IID10_MSK			SHIFT1(0x3)			/* Interrupt Source Encoded */
#define	IIR_IID10_BASE			1
#define	IIR_NIP				BIT_0				/* Interrupt Pending */

/*	UART_FCR	0x0008	FIFO Control Register */
/*		Bit(s) UART_FCR_RSRV_31_8 reserved */
/* Interrupt Trigger Level (threshold) */
#define	FCR_ITL_MSK			SHIFT6(0x3)			
#define	FCR_ITL_BASE			6
#define	FCR_BUS				BIT_5				/* 32-Bit Peripheral Bus */
#define	FCR_TRAIL				BIT_4				/* Trailing Bytes */
/* Transmitter Interrupt Level */
#define	FCR_TIL				BIT_3				
#define	FCR_RESETTF			BIT_2				/* Reset Transmit FIFO */
#define	FCR_RESETRF			BIT_1				/* Reset Receive FIFO */
/* Transmit and Receive FIFO Enable */
#define	FCR_TRFIFOE			BIT_0				

/*	UART_LCR	0x000C	Line Control Register */
/*		Bit(s) UART_LCR_RSRV_31_8 reserved */
#define	LCR_DLAB				BIT_7				/* Divisor Latch Access Bit */
#define	LCR_SB					BIT_6				/* Set Break */
#define	LCR_STKYP				BIT_5				/* Sticky Parity */
#define	LCR_EPS				BIT_4				/* Even Parity Select */
#define	LCR_PEN				BIT_3				/* Parity Enable */
#define	LCR_STB				BIT_2				/* Stop Bits */
#define	LCR_WLS10_MSK			SHIFT0(0x3)			/* Word Length Select */
#define	LCR_WLS10_BASE			0

/*	UART_MCR	0x0010	Modem Control Register */
/*		Bit(s) UART_MCR_RSRV_31_6 reserved */
#define	MCR_AFE				BIT_5				/* Auto-flow Control Enable */
#define	MCR_LOOP				BIT_4				/* Loopback Mode */
#define	MCR_OUT2				BIT_3				/* OUT2 Signal Control */
#define	MCR_OUT1				BIT_2				/* Test Bit */
#define	MCR_RTS				BIT_1				/* Request to Send */
#define	MCR_DTR				BIT_0				/* Data Terminal Ready */

/*	UART_LSR	0x0014	Line Status Register */
/*		Bit(s) UART_LSR_RSRV_31_8 reserved */
#define	LSR_FIFOE				BIT_7				/* FIFO Error Status */
#define	LSR_TEMT				BIT_6				/* Transmitter Empty */
#define	LSR_TDRQ				BIT_5				/* Transmit Data Request */
#define	LSR_BI					BIT_4				/* Break Interrupt */
#define	LSR_FE					BIT_3				/* Framing Error */
#define	LSR_PE					BIT_2				/* Parity Error */
#define	LSR_OE					BIT_1				/* Overrun Error */
#define	LSR_DR					BIT_0				/* Data Ready */

/*	UART_MSR	0x0018	Modem Status Register */
/*		Bit(s) UART_MSR_RSRV_31_8 reserved */
#define	MSR_DCD				BIT_7				/* Data Carrier Detect */
#define	MSR_RI					BIT_6				/* Ring Indicator */
#define	MSR_DSR				BIT_5				/* Data Set Ready */
#define	MSR_CTS				BIT_4				/* Clear to Send */
/* Delta Data Carrier Detect */
#define	MSR_DDCD				BIT_3				
/* Trailing Edge Ring Indicator */
#define	MSR_TERI				BIT_2				
#define	MSR_DDSR				BIT_1				/* Delta Data Set Ready */
#define	MSR_DCTS				BIT_0				/* Delta Clear to Send */

/*	UART_SCR	0x001C	Scratchpad Register */
/*		Bit(s) UART_SCR_RSRV_31_8 reserved */
#define	SCR_SCRATCHPAD_MSK			SHIFT0(0xff)		/* SCRATCHPAD */
#define	SCR_SCRATCHPAD_BASE		0

/*	UART_ISR	0x0020	Infrared Selection Register */
/*		Bit(s) UART_ISR_RSRV_31_5 reserved */
#define	ISR_RXPL				BIT_4				/* Receive Data Polarity */
#define	ISR_TXPL				BIT_3				/* Transmit Data Polarity */
/* Transmit Pulse Width Select */
#define	ISR_XMODE				BIT_2				
#define	ISR_RCVEIR				BIT_1				/* Receiver SIR Enable */
#define	ISR_XMITIR				BIT_0				/* Transmitter SIR Enable */

/*	UART_FOR	0x0024	Receive FIFO Occupancy Register */
/*		Bit(s) UART_FOR_RSRV_31_6 reserved */
#define	FOR_BYTE_COUNT_MSK			SHIFT0(0x3f)		/* BYTE COUNT */
#define	FOR_BYTE_COUNT_BASE		0

/*	UART_ABR	0x0028	Auto-Baud Control Register */
/*		Bit(s) UART_ABR_RSRV_31_4 reserved */
#define	ABR_ABT				BIT_3				/* ABT */
#define	ABR_ABUP				BIT_2				/* ABUP */
#define	ABR_ABLIE				BIT_1				/* ABLIE */
#define	ABR_ABE				BIT_0				/* ABE */

/*	UART_ACR	0x002C	Auto-Baud Count Register */
/*		Bit(s) ACR_RSRV_31_16 reserved */
#define	ACR_COUNT_VALUE_MSK		SHIFT0(0xffff)	/* COUNT VALUE */
#define	ACR_COUNT_VALUE_BASE		0

#endif

