#ifndef __DMA_H__
#define __DMA_H__

#define DMA_MAX_CHANNELS_NUM    32

typedef struct{
    __IO uint32_t DDADR;                    /* 0x200 DMA Descriptor Address Register [] */
    __IO uint32_t DSADR;                    /* 0x204 DMA Source Address Register [] */
    __IO uint32_t DTADR;                    /* 0x208 DMA Target Address Register [] */
    __IO uint32_t DCMD;                     /* 0x20C DMA Command Register [] */
}DMA_Descriptor_TypeDef;

typedef struct{
    __IO uint32_t DCSR[DMA_MAX_CHANNELS_NUM];                 /* 0x0000  */
    
    __IO uint32_t RESERVED0[(0xa0-0x7c)/4-1];
    
    __IO uint32_t DALGN;                    /* 0x00A0 */
    __IO uint32_t DPCSR;                    //0xA4
    __IO uint32_t RESERVED1[14];
    
    __IO uint32_t DQSR0;                    /* 0x00E0 */
    __IO uint32_t DQSR1;
    __IO uint32_t DQSR2;
    
    __IO uint32_t RESERVED2;
    
    __IO uint32_t DINT;                     /* 0x00F0 */
    
    __IO uint32_t RESERVED3[3];
    
    __IO uint32_t DRCMR1[64];                /* 0x0100 - 0x1FC */
    __IO DMA_Descriptor_TypeDef DMA_DESC[DMA_MAX_CHANNELS_NUM]; /* DMA Descriptor List 0x200 - 0x3FC */
    __IO uint32_t DRCMR2[36];                /* 0x1100 - 0x118C */
 
}DMA_TypeDef;

#define    DMA_BASE                     0xD4000000                              
#define    DMA  (( DMA_TypeDef *) DMA_BASE )

/*	DMA Channel Control/Status Registers 0-15 (DCSRx) */
#define	CSR_RUN					BIT_31			/* Run */
#define	CSR_NODESCFETCH			BIT_30			/* No-Descriptor Fetch */
#define	CSR_STOPIRQEN			BIT_29			/* Stop interrupt enabled */
#define	CSR_EORIRQEN            BIT_28		    /* Setting the End-of-Receive interrupt enable */	
#define	CSR_EORJMPEN            BIT_27		    /* Jump to the next descriptor on EOR */	
#define	CSR_EORSTOPEN			BIT_26			/* Stop channel on EOR */
#define	CSR_SETCMPST            BIT_25		    /* Set descriptor compare status */	
#define	CSR_CLRCMPST            BIT_24			/* Clear descriptor compare status */
#define	CSR_RASIRQEN            BIT_23			/* Request after channel stopped interrupt enable */
#define	CSR_MASKRUN				BIT_22			/* MASKRUN */
#define	CSR_CMPST				BIT_10			/* Descriptor compare status */
#define	CSR_EORINT				BIT_9			/* End of Receive Interrupt */
#define	CSR_REQPEND				BIT_8			/* Request pending */
#define	CSR_RASINTR				BIT_4			/* Request after channel stopped */
#define	CSR_STOPINTR            BIT_3			/* Stop interrupt */
#define	CSR_ENDINTR				BIT_2			/* End interrupt */
#define	CSR_STARTINTR			BIT_1			/* Start interrupt */
#define	CSR_BUSERRINTR			BIT_0			/* Bus error interrupt */


/* Alignment control for channel x */
#define	ALGN_DALGN_MSK                  SHIFT0(0xffff)	
#define	ALGN_DALGN_BASE			        0


/*	DMA_PCSR	0x00A4	DMA Programmed I/O Control Status Register */
/* Activate posted writes and split reads */
#define	PCSR_BRGSPLIT			        BIT_31	
#define	PCSR_BRGBUSY			        BIT_0				/* Bridge busy status */


/* Clearing pending request */
#define	RQSR0_CLR                       BIT_8				
/*		Bit(s) DMA_RQSR0_RSRV_7_5 reserved */
#define	RQSR0_REQPEND_MSK               SHIFT0(0x1f)		/* Request pending */
#define	RQSR0_REQPEND_BASE              0

/*	DMA_INT		0x00F0	DMA Interrupt Register */
#define	INT_CHLINTR_MSK                 SHIFT0(0xffff)	/* Channel interrupt */
#define	INT_CHLINTR_BASE		        0

/*		Bit(s) DMA_RCMR_RSRV_31_8 reserved */
#define	RCMR_MAPVLD				        BIT_7				/* Map valid channel */
/*		Bit(s) DMA_RCMR_RSRV_6_5 reserved */
#define	RCMR_CHLNUM_MSK			        SHIFT0(0x1f)		/* Channel number */
#define	RCMR_CHLNUM_BASE                0


/* Descriptor address */
#define	DADR_DESCRIPTOR_ADDRESS_MSK		SHIFT4(0xfffffff)	
#define	DADR_DESCRIPTOR_ADDRESS_BASE    4
/*		Bit(s) DMA_DADR_RSRV_3_2 reserved */
/* Enable Descriptor Branch */
#define	DADR_BREN                       BIT_1				
#define	DADR_STOP						BIT_0				/* Stop */

/*	DMA_SADRx	0x0204	DMA Source Address Registers 0-15 */
#define	SADR_SRCADDR_MSK		        SHIFT3(0x1fffffff)	/* Source address */
#define	SADR_SRCADDR_BASE		        3
#define	SADR_SRCADDR2			        BIT_2				/* SRCADDR2 */
#define	SADR_SRCADDR0_MSK		        SHIFT0(0x3)			/* SRCADDR0 */
#define	SADR_SRCADDR0_BASE		        0


/*	DMA_TADRx	0x0208	DMA Target Address Registers 0-15 */
#define	TADR_TRGADDR_MSK		        SHIFT3(0x1fffffff)	/* Target address */
#define	TADR_TRGADDR_BASE		        3
#define	TADR_TRGADDR2			        BIT_2				/* TRGADDR2 */
#define	TADR_TRGADDR0_MSK		        SHIFT0(0x3)			/* TRGADDR0 */
#define	TADR_TRGADDR0_BASE		        0


/*	DMA_CMDx	0x020C	DMA Command Registers 0-15 */
#define	CMD_INCSRCADDR				    BIT_31			    /* Source address increment */
#define	CMD_INCTRGADDR				    BIT_30			    /* Target address increment */
#define	CMD_FLOWSRC				        BIT_29			    /* Source flow control */
#define	CMD_FLOWTRG				        BIT_28			    /* Target flow control */
/*		Bit(s) DMA_CMD_RSRV_27_26 reserved */
/* Descriptor compare enable */
#define	CMD_CMPEN					    BIT_25			
/*		Bit(s) DMA_CMD_RSRV_24 reserved */
#define	CMD_ADDRMODE				    BIT_23			    /* ADDRMODE */
#define	CMD_STARTIRQEN				    BIT_22			    /* STARTIRQEN */
#define	CMD_ENDIRQEN				    BIT_21			    /* End interrupt enable */
/*		Bit(s) DMA_CMD_RSRV_20_19 reserved */                   
#define	CMD_SIZE_MSK				    SHIFT16(0x7)	    /* Maximum burst size */
#define	CMD_SIZE_BASE				    16                  
#define	CMD_WIDTH_MSK				    SHIFT14(0x3)	    /* WIDTH */
#define	CMD_WIDTH_BASE				    14
/*		Bit(s) DMA_CMD_RSRV_13 reserved */
/* Length of the transfer in bytes */
#define	CMD_LEN_MSK				        SHIFT0(0x1fff)	
#define	CMD_LEN_BASE				    0

#endif
