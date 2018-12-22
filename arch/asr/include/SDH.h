#ifndef __SD_H__
#define __SD_H__

typedef struct
{

    __IO uint16_t SYS_ADDR_LOW                   ; /* 0x0000 System Address Low Register */             
    __IO uint16_t SYS_ADDR_HIGH                  ; /* 0x0002 System Address High Register */            
    __IO uint16_t BLOCK_SIZE                     ; /* 0x0004 Block Size Register */                     
    __IO uint16_t BLOCK_COUNT                    ; /* 0x0006 Block Count Register */                    
    __IO uint16_t ARG_LOW                        ; /* 0x0008 Argument Low Register */                   
    __IO uint16_t ARG_HIGH                       ; /* 0x000A Argument High Register */                  
    __IO uint16_t TRANSFER_MODE                  ; /* 0x000C Transfer Mode Register */                  
    __IO uint16_t CMD                            ; /* 0x000E Command Register */                        
    __IO uint16_t RESP_0                         ; /* 0x0010 Response Register 0 */                     
    __IO uint16_t RESP_1                         ; /* 0x0012 Response Register 1 */                     
    __IO uint16_t RESP_2                         ; /* 0x0014 Response Register 2 */                     
    __IO uint16_t RESP_3                         ; /* 0x0016 Response Register 3 */                     
    __IO uint16_t RESP_4                         ; /* 0x0018 Response Register 4 */                     
    __IO uint16_t RESP_5                         ; /* 0x001A Response Register 5 */                     
    __IO uint16_t RESP_6                         ; /* 0x001C Response Register 6 */                     
    __IO uint16_t RESP_7                         ; /* 0x001E Response Register 7 */                     
    __IO uint16_t BUFFER_DATA_PORT_0             ; /* 0x0020 Buffer Data Port 0 Register */             
    __IO uint16_t BUFFER_DATA_PORT_1             ; /* 0x0022 Buffer Data Port 1 Register */             
    __IO uint16_t PRESENT_STATE_1                ; /* 0x0024 Present State Register 1 */                
    __IO uint16_t PRESENT_STATE_2                ; /* 0x0026 Present State Register 2 */                
    __IO uint16_t HOST_CTRL                      ; /* 0x0028 Host Control Register */                   
    __IO uint16_t BLOCK_GAP_CTRL                 ; /* 0x002A Block Gap Control Register */              
    __IO uint16_t CLOCK_CTRL                     ; /* 0x002C Clock Control Register */                  
    __IO uint16_t TIMEOUT_CTRL_SW_RESET          ; /* 0x002E Timeout Control/Software Reset Register */ 
    __IO uint16_t NORMAL_INT_STATUS              ; /* 0x0030 Normal Interrupt Status Register */        
    __IO uint16_t ERROR_INT_STATUS               ; /* 0x0032 Error Interrupt Status Register */                        
    __IO uint16_t NORMAL_INT_STATUS_EN           ; /* 0x0034 Normal Interrupt Status Enable Register */                
    __IO uint16_t ERROR_INT_STATUS_EN            ; /* 0x0036 Error Interrupt Status Enable Register */                 
    __IO uint16_t NORMAL_INT_STATUS_INT_EN       ; /* 0x0038 Normal Interrupt Status Interrupt Enable Register */      
    __IO uint16_t ERROR_INT_STATUS_INT_EN        ; /* 0x003A Error Interrupt Status Interrupt Enable Register */       
    __IO uint16_t AUTO_CMD12_ERROR_STATUS        ; /* 0x003C Auto CMD12 Error Status Register */        
    __IO uint16_t HOST_CTRL_2                       ; /* 0x003E Host Control 2 Register */                 
    __IO uint16_t CAPABILITIES_1                 ; /* 0x0040 Capabilities Register 1 */                 
    __IO uint16_t CAPABILITIES_2                 ; /* 0x0042 Capabilities Register 2 */                 
    __IO uint16_t CAPABILITIES_3                 ; /* 0x0044 Capabilities Register 3 */                 
    __IO uint16_t CAPABILITIES_4                 ; /* 0x0046 Capabilities Register 4 */                 
    __IO uint16_t MAX_CURRENT_1                  ; /* 0x0048 Maximum Current Register 1 */              
    __IO uint16_t MAX_CURRENT_2                  ; /* 0x004A Maximum Current Register 2 */              
    __IO uint16_t MAX_CURRENT_3                  ; /* 0x004C Maximum Current Register 3 */              
    __IO uint16_t MAX_CURRENT_4                  ; /* 0x004E Maximum Current Register 4 */              
    __IO uint16_t FORCE_EVENT_AUTO_CMD12_ERROR   ; /* 0x0050 Force Event Auto cmd12 Error Register */   
    __IO uint16_t FORCE_EVENT_FOR_ERROR_STATUS   ; /* 0x0052 Force Event for Error Status Register */   
    __IO uint16_t ADMA_ERROR_STATUS              ; /* 0x0054 ADMA Error Status Register */      
    
    __IO uint16_t RESERVED0;
            
    __IO uint16_t ADMA_SYS_ADDR_1                ; /* 0x0058 ADMA System Address Register 1 */          
    __IO uint16_t ADMA_SYS_ADDR_2                ; /* 0x005A ADMA System Address Register 2 */          
    __IO uint16_t ADMA_SYS_ADDR_3                ; /* 0x005C ADMA System Address Register 3 */          
    __IO uint16_t ADMA_SYS_ADDR_4                ; /* 0x005E ADMA System Address Register 4 */ 
  
#if 0 
    __IO uint16_t RESERVED1[(0xE0-0x5E)/2-1];
    __IO uint32_t FIFO_PARAM ;                        //0xE0            
    __IO uint16_t SPI_MODE;                        //00e4
    __IO uint16_t CLOCK_AND_BURST_SIZE_SETUP     ; /* 0x0e6 Clock and Burst Size Setup Register */     
    __IO uint16_t CE_ATA_1                       ; /* 0x0e8 CE-ATA Register 1 */                       
    __IO uint16_t CE_ATA_2                       ; /* 0x0ea CE-ATA Register 2 */    
    __IO uint16_t PAD_IO_SETUP                   ; /* 0x0ec PAD I/O Setup Register */    
    __IO uint16_t RESERVED2[(0xfc-0xec)/2-1];

#else        
    __IO uint16_t PRESET_VALUE_FOR_INIT             ; /* 0x0060 Preset Value Register for Initialization */
    __IO uint16_t PRESET_VALUE_FOR_DS               ; /* 0x0062 Preset Value Register for Default Speed */ 
    __IO uint16_t PRESET_VALUE_FOR_HS               ; /* 0x0064 Preset Value Register for High Speed */    
    __IO uint16_t PRESET_VALUE_FOR_SDR12            ; /* 0x0066 Preset Value Register for SDR12 */         
    __IO uint16_t PRESET_VALUE_FOR_SDR25            ; /* 0x0068 Preset Value Register for SDR25 */         
    __IO uint16_t PRESET_VALUE_FOR_SDR50            ; /* 0x006A Preset Value Register for SDR50 */         
    __IO uint16_t PRESET_VALUE_FOR_SDR104           ; /* 0x006C Preset Value Register for SDR104 */        
    __IO uint16_t PRESET_VALUE_FOR_DDR50            ; /* 0x006E Preset Value Register for DDR50 */  
    
    __IO uint16_t RESERVED1[56];
           
    __IO uint32_t SHARED_BUS_CTRL                   ; /* 0x00E0 Shared Bus Control Register */    
    
    __IO uint16_t RESERVED2[12];

    __IO uint16_t SLOT_INT_STATUS                   ; /* 0x00FC Slot Interrupt Status Register */          
    __IO uint16_t HOST_CTRL_VER                     ; /* 0x00FE Host Control Version Register */           

    __IO uint32_t CFG_FIFO_PARAM                    ; /* 0x0100 SD Extra Parameters Register */            
    __IO uint32_t FIFO_PARAM                        ; /* 0x0104 FIFO Parameters Register */                
    __IO uint16_t SPI_MODE                          ; /* 0x0108 SPI Mode Register */                       
    __IO uint16_t CLOCK_AND_BURST_SIZE_SETUP        ; /* 0x010A Clock and Burst Size Setup Register */     
    __IO uint16_t CE_ATA_1                          ; /* 0x010C CE-ATA Register 1 */                       
    __IO uint16_t CE_ATA_2                          ; /* 0x010E CE-ATA Register 2 */                       
    __IO uint32_t PAD_IO_SETUP                      ; /* 0x0110 PAD I/O Setup Register */                  
    __IO uint32_t RX_CFG_REG                        ; /* 0x0114 RX Configuration Register */                  
    __IO uint32_t TX_CFG_REG                        ; /* 0x0118 TX Configuration Register */
    __IO uint32_t TUNING_CFG_REG                    ; /* 0x011C TX Configuration Register */

    __IO uint16_t RESERVED3[16];

    __IO uint32_t TUNING_CFG2_REG                   ; /* 0x0140 TUNING CONFIG Register */
    __IO uint32_t TRS2RCV_TIMING_REG                ; /* 0x0144 TRANSIMIT TO RECEIVE TIMING PARAM Regsiter */
    __IO uint32_t FLC_FUNC_REG;                     ; /* 0x0148 FLC Function Register */

#endif             







}SD_TypeDef;


#define    SD0_BASE      0xD4280000
#define    SD0  (( SD_TypeDef *) SD0_BASE )

#define    SD1_BASE      0xD4280800
#define    SD1  (( SD_TypeDef *) SD1_BASE )

#define    SD2_BASE      0xD4281000
#define    SD2  (( SD_TypeDef *) SD2_BASE )

#define    FLC_MMC_BASE      0xD427F000
#define    MMC_FLC  (( SD_TypeDef *) FLC_MMC_BASE )

/*
 *
 *	THE BIT DEFINES
 *
 */
/*	SD_SYS_ADDR_LOW					0x0000	System Address Low Register */
#define	SYS_ADDR_LOW_DMA_ADDR_L_MSK		SHIFT0(0xffff)	/* DMA Address Low */
#define	SYS_ADDR_LOW_DMA_ADDR_L_BASE		0

/*	SD_SYS_ADDR_HIGH				0x0002	System Address High Register */
#define	SYS_ADDR_HIGH_DMA_ADDR_H_MSK			SHIFT0(0xffff)	/* DMA Address High */
#define	SYS_ADDR_HIGH_DMA_ADDR_H_BASE		0

/*	SD_BLOCK_SIZE					0x0004	Block Size Register */
/*		Bit(s) SD_BLOCK_SIZE_RSRV_15 reserved */
/* Host DMA Buffer Boundary */
#define	BLOCK_SIZE_HOST_DMA_BDRY_MSK			SHIFT12(0x7)	
#define	BLOCK_SIZE_HOST_DMA_BDRY_BASE		12
#define	BLOCK_SIZE_BLOCK_SIZE_MSK			SHIFT0(0xfff)	/* Block Size */
#define	BLOCK_SIZE_BLOCK_SIZE_BASE			0

/*	SD_BLOCK_COUNT					0x0006	Block Count Register */
#define	BLOCK_COUNT_BLOCK_COUNT_MSK		SHIFT0(0xffff)	/* Block Count */
#define	BLOCK_COUNT_BLOCK_COUNT_BASE		0

/*	SD_ARG_LOW						0x0008	Argument Low Register */
#define	ARG_LOW_ARG_L_MSK		SHIFT0(0xffff)	/* Argument Low */
#define	ARG_LOW_ARG_L_BASE		0

/*	SD_ARG_HIGH						0x000A	Argument High Register */
#define	ARG_HIGH_ARG_H_MSK		SHIFT0(0xffff)	/* Argument High */
#define	ARG_HIGH_ARG_H_BASE		0

/*	SD_TRANSFER_MODE				0x000C	Transfer Mode Register */
/*		Bit(s) SD_TRANSFER_MODE_RSRV_15_6 reserved */
/* Multiple Block Select */
#define	TRANSFER_MODE_MULTI_BLK_SEL		BIT_5			
/* Data Transfer Direction Select */
#define	TRANSFER_MODE_TO_HOST_DIR		BIT_4			
/*		Bit(s) SD_TRANSFER_MODE_RSRV_3 reserved */
#define	TRANSFER_MODE_AUTO_CMD_MSK			SHIFT2(0x03)
#define	TRANSFER_MODE_AUTO_CMD_BASE		2	
#define	TRANSFER_MODE_AUTO_CMD23_EN		BIT_3
#define	TRANSFER_MODE_AUTO_CMD12_EN		BIT_2			/* Auto CMD12 Enable */
#define	TRANSFER_MODE_BLK_CNT_EN			BIT_1			/* Block Count Enable */
#define	TRANSFER_MODE_DMA_EN				BIT_0			/* DMA Enable */

/*	SD_CMD							0x000E	Command Register */
/*		Bit(s) CMD_RSRV_15_14 reserved */
#define	CMD_CMD_INDEX_MSK		SHIFT8(0x3f)	/* Command Index */
#define	CMD_CMD_INDEX_BASE		8
#define	CMD_CMD_TYPE_MSK			SHIFT6(0x3)		/* Command Type */
#define	CMD_CMD_TYPE_BASE		6
#define	CMD_DATA_PRESENT			BIT_5			/* Data Present */
#define	CMD_CMD_INDEX_CHK_EN		BIT_4			/* Command Index Check Enable */
#define	CMD_CMD_CRC_CHK_EN		BIT_3			/* Command CRC Check Enable */
/*		Bit(s) CMD_RSRV_2 reserved */
/* Response Type Select for SD/SD in SPI Modes */
#define	CMD_RESP_TYPE_MSK		SHIFT0(0x3)		
#define	CMD_RESP_TYPE_BASE		0

/*	SD_RESP_0						0x0010	Response Register 0 */
#define	RESP_0_RESP0_MSK			SHIFT0(0xffff)	/* Response 0 */
#define	RESP_0_RESP0_BASE		0

/*	SD_RESP_1						0x0012	Response Register 1 */
#define	RESP_1_RESP1_MSK			SHIFT0(0xffff)	/* Response 1 */
#define	RESP_1_RESP1_BASE		0

/*	SD_RESP_2						0x0014	Response Register 2 */
#define	RESP_2_RESP2_MSK			SHIFT0(0xffff)	/* Response 2 */
#define	RESP_2_RESP2_BASE		0

/*	SD_RESP_3						0x0016	Response Register 3 */
#define	RESP_3_RESP3_MSK			SHIFT0(0xffff)	/* Response 3 */
#define	RESP_3_RESP3_BASE		0

/*	SD_RESP_4						0x0018	Response Register 4 */
#define	RESP_4_RESP4_MSK			SHIFT0(0xffff)	/* Response 4 */
#define	RESP_4_RESP4_BASE		0

/*	SD_RESP_5						0x001A	Response Register 5 */
#define	RESP_5_RESP5_MSK			SHIFT0(0xffff)	/* Response 5 */
#define	RESP_5_RESP5_BASE		0

/*	SD_RESP_6						0x001C	Response Register 6 */
#define	RESP_6_RESP6_MSK			SHIFT0(0xffff)	/* Response 6 */
#define	RESP_6_RESP6_BASE		0

/*	SD_RESP_7						0x001E	Response Register 7 */
#define	RESP_7_RESP7_MSK			SHIFT0(0xffff)	/* Response 7 */
#define	RESP_7_RESP7_BASE		0

/*	SD_BUFFER_DATA_PORT_0			0x0020	Buffer Data Port 0 Register */
/* Processor Data 0 */
#define	BUFFER_DATA_PORT_0_CPU_DATA0_MSK			SHIFT0(0xffff)	
#define	BUFFER_DATA_PORT_0_CPU_DATA0_BASE		0

/*	SD_BUFFER_DATA_PORT_1			0x0022	Buffer Data Port 1 Register */
/* Processor Data 1 */
#define	BUFFER_DATA_PORT_1_CPU_DATA1_MSK			SHIFT0(0xffff)	
#define	BUFFER_DATA_PORT_1_CPU_DATA1_BASE		0

/*	SD_PRESENT_STATE_1				0x0024	Present State Register 1 */
/*		Bit(s) SD_PRESENT_STATE_1_RSRV_15_12 reserved */
/* Buffer Read Enable */
#define	PRESENT_STATE_1_BUFFER_RD_EN			BIT_11			
/* Buffer Write Enable */
#define	PRESENT_STATE_1_BUFFER_WR_EN			BIT_10			
#define	PRESENT_STATE_1_RX_ACTIVE			BIT_9			/* Rx Active */
#define	PRESENT_STATE_1_TX_ACTIVE			BIT_8			/* Tx Active */
/*		Bit(s) PRESENT_STATE_1_RSRV_7_3 reserved */
#define	PRESENT_STATE_1_RETUNING_REQ                 BIT_3   //NEVO C0 
#define	PRESENT_STATE_1_DAT_ACTIVE			BIT_2			/* Data Line Active */
/* Command Inhibit Data */
#define	PRESENT_STATE_1_CMD_INHIBIT_DAT		BIT_1			
/* Command Inhibit Command */
#define	PRESENT_STATE_1_CMD_INHIBIT_CMD		BIT_0			

/*	PRESENT_STATE_2				0x0026	Present State Register 2 */
/*		Bit(s) PRESENT_STATE_2_RSRV_15_9 reserved */
/* MMC1_CMD Line Signal Level */
#define	PRESENT_STATE_2_CMD_LEVEL			BIT_8			
/* MMC1_DAT[3:0] Line Signal Level */
#define	PRESENT_STATE_2_DAT_LEVEL_MSK		SHIFT4(0xf)		
#define	PRESENT_STATE_2_DAT_LEVEL_BASE		4
#define	PRESENT_STATE_2_WRITE_PROT			BIT_3			/* Write Protect */
#define	PRESENT_STATE_2_CARD_DET				BIT_2			/* Card Detect */
#define	PRESENT_STATE_2_CARD_STABLE			BIT_1			/* Card Stable */
#define	PRESENT_STATE_2_CARD_INSERTED		BIT_0			/* Card Inserted */

/*	HOST_CTRL					0x0028	Host Control Register */
/*		Bit(s) HOST_CTRL_RSRV_15_12 reserved */
#define	HOST_CTRL_SD_BUS_VLT_MSK			SHIFT9(0x7)		/* SD Bus Voltage */
#define	HOST_CTRL_SD_BUS_VLT_BASE		9
#define	HOST_CTRL_SD_BUS_POWER			BIT_8			/* SD Bus Power */
/* Card Detect Signal Selection */
#define	HOST_CTRL_CARD_DET_S				BIT_7			
/* Card Detect Test Level */
#define	HOST_CTRL_CARD_DET_L				BIT_6			
/* Extended Data Transfer Width */
#define	HOST_CTRL_EX_DATA_WIDTH		BIT_5			
/*		Bit(s) HOST_CTRL_RSRV_5 reserved */
#define	HOST_CTRL_DMA_SEL_MSK			SHIFT3(0x3)		/* DMA Select */
#define	HOST_CTRL_DMA_SEL_BASE			3
/* Extend Data Output Enable */
#define	HOST_CTRL_HI_SPEED_EN			BIT_2			
#define	HOST_CTRL_DATA_WIDTH				BIT_1			/* Data Width */
#define	HOST_CTRL_LED_CTRL				BIT_0			/* LED Control */

/*	BLOCK_GAP_CTRL				0x002A	Block Gap Control Register */
/*		Bit(s) BLOCK_GAP_CTRL_RSRV_15_11 reserved */
/* Wakeup on Card Removal */
#define	BLOCK_GAP_CTRL_W_REMOVAL					BIT_10			
/* Wakeup on Card Insertion */
#define	BLOCK_GAP_CTRL_W_INSERTION				BIT_9			
/* Wakeup on Card Interrupt */
#define	BLOCK_GAP_CTRL_W_CARD_INT				BIT_8			
/*		Bit(s) BLOCK_GAP_CTRL_RSRV_7_4 reserved */
/* Block Gap Interrupt */
#define	BLOCK_GAP_CTRL_INT_BLK_GAP				BIT_3			
/* Read Wait Control */
#define	BLOCK_GAP_CTRL_RD_WAIT_CTL				BIT_2			
/* Continue Request */
#define	BLOCK_GAP_CTRL_CONT_REQ					BIT_1			
/* Stop At Block Gap Request */
#define	BLOCK_GAP_CTRL_STOP_AT_BLOCK_GAP_REQ		BIT_0			

/*	CLOCK_CTRL					0x002C	Clock Control Register */
/* SDCLK Frequency Select */
#define	CLOCK_CTRL_SD_FREQ_SEL_LO_MSK		SHIFT8(0xff)	
#define	CLOCK_CTRL_SD_FREQ_SEL_LO_BASE		8
/*		Bit(s) SD_CLOCK_CTRL_RSRV_7_3 reserved */
#define	CLOCK_CTRL_SD_FREQ_SEL_HI_MSK		SHIFT6(0xff)	
#define	CLOCK_CTRL_SD_FREQ_SEL_HI_BASE		6
#define	CLOCK_CTRL_CLK_GEN_SEL                   BIT_5
#define	CLOCK_CTRL_SD_CLK_EN				BIT_2			/* SDCLK Clock Enable */
/* Internal Clock Stable */
#define	CLOCK_CTRL_INT_CLK_STABLE		BIT_1			
/* Internal Clock Enable */
#define	CLOCK_CTRL_INT_CLK_EN			BIT_0			

/*	SD_TIMEOUT_CTRL_SW_RESET		0x002E	Timeout Control/Software Reset
 *											Register
 */
/*		Bit(s) SD_TIMEOUT_CTRL_SW_RESET_RSRV_15_11 reserved */
/* Soft Reset for Data Port of Logic */
#define	TIMEOUT_CTRL_SW_RESET_SW_RST_DAT				BIT_10			
/* Soft Reset for Command Part of Logic */
#define	TIMEOUT_CTRL_SW_RESET_SW_RST_CMD				BIT_9			
/* Software Reset for All */
#define	TIMEOUT_CTRL_SW_RESET_SW_RST_ALL				BIT_8			
/*		Bit(s) TIMEOUT_CTRL_SW_RESET_RSRV_7_4 reserved */
/* Timeout Value */
#define	TIMEOUT_CTRL_SW_RESET_TIMEOUT_VALUE_MSK		SHIFT0(0xf)		
#define	TIMEOUT_CTRL_SW_RESET_TIMEOUT_VALUE_BASE		0

/*	NORMAL_INT_STATUS			0x0030	Normal Interrupt Status Register */
#define	NORMAL_INT_STATUS_ERR_INT			BIT_15			/* Error Interrupt */
/*		Bit(s) NORMAL_INT_STATUS_RSRV_14_9 reserved */
#define	NORMAL_INT_STATUS_RETUNING_INT                 BIT_12   //NEVO C0
#define	NORMAL_INT_STATUS_INT_C                 BIT_11   //NEVO C0
#define	NORMAL_INT_STATUS_INT_B                 BIT_10   //NEVO C0
#define	NORMAL_INT_STATUS_INT_A                 BIT_9   //NEVO C0
#define	NORMAL_INT_STATUS_CARD_INT			BIT_8			/* Card Interrupt */
/* Card Removal Interrupt */
#define	NORMAL_INT_STATUS_CARD_REM_INT		BIT_7			
/* Card Insertion Interrupt */
#define	NORMAL_INT_STATUS_CARD_INS_INT		BIT_6			
#define	NORMAL_INT_STATUS_RX_RDY				BIT_5			/* Rx Ready */
#define	NORMAL_INT_STATUS_TX_RDY				BIT_4			/* Tx Ready */
#define	NORMAL_INT_STATUS_DMA_INT			BIT_3			/* DMA Interrupt */
#define	NORMAL_INT_STATUS_BLOCK_GAP_EVT		BIT_2			/* Block Gap Event */
/* Transfer Complete */
#define	NORMAL_INT_STATUS_XFER_COMPLETE		BIT_1			
#define	NORMAL_INT_STATUS_CMD_COMPLETE		BIT_0			/* Command Complete */

/*	ERROR_INT_STATUS				0x0032	Error Interrupt Status Register */
/* CRC Status Error */
#define	ERROR_INT_STATUS_CRC_STATUS_ERR			BIT_15			
/* Command Completion Signal Timeout Error */
#define	ERROR_INT_STATUS_CPL_TIMEOUT_ERR			BIT_14			
/* AXI Bus Response Error */
#define	ERROR_INT_STATUS_AXI_RESP_ERR			BIT_13			
/* SPI Mode Error */
#define	ERROR_INT_STATUS_SPI_ERR					BIT_12			
/*		Bit(s) ERROR_INT_STATUS_RSRV_11_10 reserved */
#define	ERROR_INT_STATUS_TUNE_ERR				BIT_10 //NEVO C0
#define	ERROR_INT_STATUS_ADMA_ERR				BIT_9			/* ADMA Error */
/* Auto CMD12 Error */
#define	ERROR_INT_STATUS_AUTO_CMD12_ERR			BIT_8			
/* Current Limit Error */
#define	ERROR_INT_STATUS_CUR_LIMIT_ERR			BIT_7			
/* ReadDataEnd Bit Error */
#define	ERROR_INT_STATUS_RD_DATA_END_BIT_ERR		BIT_6			
/* Read Data CRC Error */
#define	ERROR_INT_STATUS_RD_DATA_CRC_ERR			BIT_5			
/* Data Timeout Error */
#define	ERROR_INT_STATUS_DATA_TIMEOUT_ERR		BIT_4			
/* Command Index Error */
#define	ERROR_INT_STATUS_CMD_INDEX_ERR			BIT_3			
/* Command End Bit Error */
#define	ERROR_INT_STATUS_CMD_END_BIT_ERR			BIT_2			
/* Command CRC Error */
#define	ERROR_INT_STATUS_CMD_CRC_ERR				BIT_1			
/* Command Timeout Error */
#define	ERROR_INT_STATUS_CMD_TIMEOUT_ERR			BIT_0			

/*	NORMAL_INT_STATUS_EN			0x0034	Normal Interrupt Status Enable
 *											Register
 */
/*		Bit(s) NORMAL_INT_STATUS_EN_RSRV_15_9 reserved */
#define	NORMAL_INT_STATUS_RETUNE_INT_EN				BIT_12 //NEVO C0
#define	NORMAL_INT_STATUS_INT_C_INT_EN				BIT_11 //NEVO C0
#define	NORMAL_INT_STATUS_INT_B_INT_EN				BIT_10 //NEVO C0
#define	NORMAL_INT_STATUS_INT_A_INT_EN				BIT_9 //NEVO C0
/* Card Interrupt Enable */
#define	NORMAL_INT_STATUS_EN_CARD_INT_EN				BIT_8			
/* Card Removal Status Enable */
#define	NORMAL_INT_STATUS_EN_CARD_REM_EN				BIT_7			
/* Card Insertion Status Enable */
#define	NORMAL_INT_STATUS_EN_CARD_INS_EN				BIT_6			
/* Buffer Read Ready Enable */
#define	NORMAL_INT_STATUS_EN_RD_RDY_EN				BIT_5			
/* Buffer Write Ready Enable */
#define	NORMAL_INT_STATUS_EN_TX_RDY_EN				BIT_4			
/* DMA Interrupt Enable */
#define	NORMAL_INT_STATUS_EN_DMA_INT_EN				BIT_3			
/* Block Gap Event Enable */
#define	NORMAL_INT_STATUS_EN_BLOCK_GAP_EVT_EN		BIT_2			
/* Transfer Complete Enable */
#define	NORMAL_INT_STATUS_EN_XFER_COMPLETE_EN		BIT_1			
/* Command Complete Enable */
#define	NORMAL_INT_STATUS_EN_CMD_COMPLETE_EN			BIT_0			

/*	ERROR_INT_STATUS_EN			0x0036	Error Interrupt Status Enable
 *											Register
 */
/* CRC Status Error Enable */
#define	ERROR_INT_STATUS_EN_CRC_STATUS_ERR_EN			BIT_15			
/* CPL Timeout Error Enable */
#define	ERROR_INT_STATUS_EN_CPL_TIMEOUT_ERR_EN			BIT_14			
/* AXI Response Error Enable */
#define	ERROR_INT_STATUS_EN_AXI_RESP_ERR_EN				BIT_13			
/* SPI Error Enable */
#define	ERROR_INT_STATUS_EN_SPI_ERR_EN					BIT_12			
/*		Bit(s) ERROR_INT_STATUS_EN_RSRV_11_10 reserved */
#define	ERROR_INT_STATUS_EN_TUNING_ERR_EN					BIT_10  //NEVO C0
/* ADMA Error Enable */
#define	ERROR_INT_STATUS_EN_ADMA_ERR_EN					BIT_9			
/* Auto CMD12 Error Enable */
#define	ERROR_INT_STATUS_EN_AUTO_CMD12_ERR_EN			BIT_8			
/* Current Limit Error Enable */
#define	ERROR_INT_STATUS_EN_CUR_LIM_ERR_EN				BIT_7			
/* Data End Bit Error Enable */
#define	ERROR_INT_STATUS_EN_RD_DATA_END_BIT_ERR_EN		BIT_6			
/* Data CRC Error Enable */
#define	ERROR_INT_STATUS_EN_RD_DATA_CRC_ERR_EN			BIT_5			
/* Data Timeout Error Enable */
#define	ERROR_INT_STATUS_EN_DATA_TIMEOUT_ERR_EN			BIT_4			
/* Command Index Error Enable */
#define	ERROR_INT_STATUS_EN_CMD_INDEX_ERR_EN				BIT_3			
/* Command End Bit Error Enable */
#define	ERROR_INT_STATUS_EN_CMD_END_BIT_ERR_EN			BIT_2			
/* Command CRC Error Enable */
#define	ERROR_INT_STATUS_EN_CMD_CRC_ERR_EN				BIT_1			
/* Command Timeout Error Enable */
#define	ERROR_INT_STATUS_EN_CMD_TIMEOUT_ERR_EN			BIT_0			

/*	NORMAL_INT_STATUS_INT_EN		0x0038	Normal Interrupt Status Interrupt
 *											Enable Register
 */
/*		Bit(s) NORMAL_INT_STATUS_INT_EN_RSRV_15_9 reserved */
#define	NORMAL_INT_STATUS_INT_EN_RETUNE_INT_INT_EN				BIT_12 //NEVO C0
#define	NORMAL_INT_STATUS_INT_EN_INT_C_INT_INT_EN				BIT_11 //NEVO C0
#define	NORMAL_INT_STATUS_INT_EN_INT_B_INT_INT_EN				BIT_10 //NEVO C0
#define	NORMAL_INT_STATUS_INT_EN_INT_A_INT_INT_EN				BIT_9 //NEVO C0


/* Card Interrupt Interrupt Enable */
#define	NORMAL_INT_STATUS_INT_EN_CARD_INT_INT_EN				BIT_8			
/* Card Removal Interrupt Enable */
#define	NORMAL_INT_STATUS_INT_EN_CARD_REM_INT_EN				BIT_7			
/* Card Insertion Interrupt Enable */
#define	NORMAL_INT_STATUS_INT_EN_CARD_INS_INT_EN				BIT_6			
/* Buffer Read Ready Interrupt Enable */
#define	NORMAL_INT_STATUS_INT_EN_RX_RDY_INT_EN				BIT_5			
/* Buffer Write Ready Interrupt Enable */
#define	NORMAL_INT_STATUS_INT_EN_TX_RDY_INT_EN				BIT_4			
/* DMA Interrupt Interrupt Enable */
#define	NORMAL_INT_STATUS_INT_EN_DMA_INT_INT_EN				BIT_3			
/* Block Gap Event Interrupt Enable */
#define	NORMAL_INT_STATUS_INT_EN_BLOCK_GAP_EVT_INT_EN		BIT_2			
/* Transfer Complete Interrupt Enable */
#define	NORMAL_INT_STATUS_INT_EN_XFER_COMPLETE_INT_EN		BIT_1			
/* Command Complete Interrupt Enable */
#define	NORMAL_INT_STATUS_INT_EN_CMD_COMPLETE_INT_EN			BIT_0			

/*	ERROR_INT_STATUS_INT_EN		0x003A	Error Interrupt Status Interrupt
 *											Enable Register
 */
/* CRC Status Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_CRC_STATUS_ERR_INT_EN			BIT_15			
/* CPL Timeout Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_CPL_TIMEOUT_ERR_INT_EN			BIT_14			
/* AXI Response Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_AXI_RESP_ERR_INT_EN				BIT_13			
/* SPI Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_SPI_ERR_INT_EN					BIT_12			
/*		Bit(s) ERROR_INT_STATUS_INT_EN_RSRV_11_10 reserved */
#define	ERROR_INT_STATUS_INT_EN_TUNE_ERR_INT_EN					BIT_10 //NEVO C0
/* ADMA Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_ADMA_ERR_INT_EN					BIT_9			
/* Auto CMD12 Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_AUTO_CMD12_ERR_INT_EN			BIT_8			
/* Current Limit Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_CUR_LIM_ERR_INT_EN				BIT_7			
/* Data End Bit Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_RD_DATA_END_BIT_ERR_INT_EN		BIT_6			
/* Data CRC Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_RD_DATA_CRC_ERR_INT_EN			BIT_5			
/* Data Timeout Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_DATA_TIMEOUT_ERR_INT_EN			BIT_4			
/* Command Index Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_CMD_INDEX_ERR_INT_EN				BIT_3			
/* Command End Bit Interrupt Error Enable */
#define	ERROR_INT_STATUS_INT_EN_CMD_END_BIT_ERR_INT_EN			BIT_2			
/* Command CRC Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_CMD_CRC_ERR_INT_EN				BIT_1			
/* Command Timeout Error Interrupt Enable */
#define	ERROR_INT_STATUS_INT_EN_CMD_TIMEOUT_ERR_INT_EN			BIT_0			

/*	AUTO_CMD12_ERROR_STATUS		0x003C	Auto CMD12 Error Status Register */
/*		Bit(s) AUTO_CMD12_ERROR_STATUS_RSRV_15_8 reserved */
/* Command Not Issued Due to auto_cmd12 Error */
#define	AUTO_CMD12_ERROR_STATUS_CMD_NOT_ISSUED				BIT_7			
/*		Bit(s) AUTO_CMD12_ERROR_STATUS_RSRV_6_5 reserved */
/* Auto CMD12 Error */
#define	AUTO_CMD12_ERROR_STATUS_AUTO_CMD12_INDEX_ERR			BIT_4			
/* Auto CMD12 End Bit Error */
#define	AUTO_CMD12_ERROR_STATUS_AUTO_CMD12_END_BIT_ERR		BIT_3			
/* Auto CMD12 CRC Error */
#define	AUTO_CMD12_ERROR_STATUS_AUTO_CMD12_CRC_ERR			BIT_2			
/* Auto CMD12 Timeout Error */
#define	AUTO_CMD12_ERROR_STATUS_AUTO_CMD12_TIMEOUT_ERR		BIT_1			
/* Auto CMD12 Not Executed */
#define	AUTO_CMD12_ERROR_STATUS_AUTO_CMD12_NOT_EXE			BIT_0			


//Host Control 2 Register (HOST_CTRL_2)  NEVO C0
#define	HOST_CTRL_2_PRE_VAL_EN  BIT_15
#define	HOST_CTRL_2_ASYNC_INT_EN  BIT_14
#define	HOST_CTRL_2_SAMPLING_CLK_SEL  BIT_7
#define	HOST_CTRL_2_EXE_TUNING  BIT_6
#define	HOST_CTRL_2_SDH_V18_EN  BIT_3
#define	HOST_CTRL_2_DRV_STRENGTH_SEL_MSK  SHIFT4(0x3)
#define	HOST_CTRL_2_DRV_STRENGTH_SEL_BASE 4
#define	HOST_CTRL_2_UHS_MODE_SEL_MSK  SHIFT0(0x7)
#define	HOST_CTRL_2_UHS_MODE_SEL_BASE 0

/*	CAPABILITIES_1				0x0040	Capabilities Register 1 */
/*		Bit(s) CAPABILITIES_1_RSRV_15_14 reserved */
#define	CAPABILITIES_1_BASE_FREQ_MSK			SHIFT8(0x3f)	/* Base Frequency */
#define	CAPABILITIES_1_BASE_FREQ_BASE		8
#define	CAPABILITIES_1_TIMEOUT_UNIT			BIT_7			/* Timeout Unit */
/*		Bit(s) CAPABILITIES_1_RSRV_6 reserved */
/* Timeout Frequency */
#define	CAPABILITIES_1_TIMEOUT_FREQ_MSK		SHIFT0(0x3f)	
#define	CAPABILITIES_1_TIMEOUT_FREQ_BASE		0

/*	CAPABILITIES_2				0x0042	Capabilities Register 2 */
/*		Bit(s) CAPABILITIES_2_RSRV_15_13 reserved */
/* 64-bit System Bus Support */
#define	CAPABILITIES_2_CFG_SLOT_TYPE_MSK		SHIFT14(0x3)
#define	CAPABILITIES_2_CFG_SLOT_TYPE_BASE            14
#define	CAPABILITIES_2_ASYNC_INT_SUPPORT		BIT_13
#define	CAPABILITIES_2_SYS_BUS_64_SUPPORT		BIT_12			
/*		Bit(s) CAPABILITIES_2_RSRV_11 reserved */
/* Voltage Support 1.8V */
#define	CAPABILITIES_2_VLG_18_SUPPORT			BIT_10			
/* Voltage Support 3.0V */
#define	CAPABILITIES_2_VLG_30_SUPPORT			BIT_9			
/* Voltage Support 3.3V */
#define	CAPABILITIES_2_VLG_33_SUPPORT			BIT_8			
/* Suspend Resume Support */
#define	CAPABILITIES_2_SUS_RES_SUPPORT			BIT_7			
#define	CAPABILITIES_2_SDMA_SUPPORT				BIT_6			/* SDMA Support */
/* High Speed Support */
#define	CAPABILITIES_2_HI_SPEED_SUPPORT			BIT_5			
/* ADMA1 Support */
#define	CAPABILITIES_2_ADMA1_SUPPORT				BIT_4			
/* ADMA2 Support */
#define	CAPABILITIES_2_ADMA2_SUPPORT				BIT_3			
/*		Bit(s) CAPABILITIES_2_RSRV_2 reserved */
/* Maximum Block Length */
#define	CAPABILITIES_2_MAX_BLK_LEN_MSK			SHIFT0(0x3)		
#define	CAPABILITIES_2_MAX_BLK_LEN_BASE			0

/*	CAPABILITIES_3				0x0044	Capabilities Register 3 */
/*		Bit(s) CAPABILITIES_3_RSRV_15_0 reserved   NEVO C0*/
#define	CAPABILITIES_3_RETUNE_MODES_MSK SHIFT14(0x3)
#define	CAPABILITIES_3_RETUNE_MODES_BASE 14
#define	CAPABILITIES_3_SDR50_TUNE         BIT_13
#define	CAPABILITIES_3_DRV_TYPE_D         BIT_6
#define	CAPABILITIES_3_DRV_TYPE_C         BIT_5
#define	CAPABILITIES_3_DRV_TYPE_A         BIT_4
#define	CAPABILITIES_3_DDR50_SUPPORT         BIT_2
#define	CAPABILITIES_3_SDR104_SUPPORT         BIT_1
#define	CAPABILITIES_3_SDR50_SUPPORT         BIT_0
#define	CAPABILITIES_3_TMR_RETUNE_MSK SHIFT8(0xf)
#define	CAPABILITIES_3_TMR_RETUNE_BASE 8


/*	CAPABILITIES_4				0x0046	Capabilities Register 4 */
/*		Bit(s) CAPABILITIES_4_RSRV_15_0 reserved NEVO C0*/
#define	CAPABILITIES_4_CLK_MULTIPLIER_MSK       SHIFT0(0xff)
#define	CAPABILITIES_4_CLK_MULTIPLIER_BASE     0

/*	MAX_CURRENT_1				0x0048	Maximum Current Register 1 */
/* Maximum Current for 3.0V */
#define	MAX_CURRENT_1_MAX_CUR_30_MSK			SHIFT8(0xff)	
#define	MAX_CURRENT_1_MAX_CUR_30_BASE		8
/* Maximum Current for 3.3V */
#define	MAX_CURRENT_1_MAX_CUR_33_MSK			SHIFT0(0xff)	
#define	MAX_CURRENT_1_MAX_CUR_33_BASE		0

/*	MAX_CURRENT_2				0x004A	Maximum Current Register 2 */
/*		Bit(s) MAX_CURRENT_2_RSRV_15_8 reserved */
/* Maximum Current for 1.8V */
#define	MAX_CURRENT_2_MAX_CUR_18_MSK			SHIFT0(0xff)	
#define	MAX_CURRENT_2_MAX_CUR_18_BASE		0

/*	MAX_CURRENT_3				0x004C	Maximum Current Register 3 */
/*		Bit(s) MAX_CURRENT_3_RSRV_15_0 reserved */

/*	MAX_CURRENT_4				0x004E	Maximum Current Register 4 */
/*		Bit(s) MAX_CURRENT_4_RSRV_15_0 reserved */

/*	FORCE_EVENT_AUTO_CMD12_ERROR	0x0050	Force Event Auto cmd12 Error
 *											Register
 */
/*		Bit(s) FORCE_EVENT_AUTO_CMD12_ERROR_RSRV_15_8 reserved */
/* Force Event for Command not Issued by Auto Cmd12 Error */
#define	FORCE_EVENT_AUTO_CMD12_ERROR_F_ACMD12_ISSUE_ERR		BIT_7			
/*		Bit(s) FORCE_EVENT_AUTO_CMD12_ERROR_RSRV_6_5 reserved */
/* Force Event for Auto Cmd12 Index Error */
#define	FORCE_EVENT_AUTO_CMD12_ERROR_F_ACMD12_INDEX_ERR		BIT_4			
/* Force Event for Auto Cmd12 End Bit Error */
#define	FORCE_EVENT_AUTO_CMD12_ERROR_F__ACMD12_EBIT_ERR		BIT_3			
/* Force Event for Auto Cmd12 CRC Error */
#define	FORCE_EVENT_AUTO_CMD12_ERROR_F_ACMD12_CRC_ERR		BIT_2			
/* Force Event for Auto Cmd12 Timeout Error */
#define	FORCE_EVENT_AUTO_CMD12_ERROR_F_ACMD12_TO_ERR			BIT_1			
/* Force Event for Auto Cmd12 Not Executed Error */
#define	FORCE_EVENT_AUTO_CMD12_ERROR_F_ACMD12_NEXE_ERR		BIT_0			

/*	FORCE_EVENT_FOR_ERROR_STATUS	0x0052	Force Event for Error Status
 *											Register
 */
/* Force Event for CRC Status Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_CRC_STATUS_ERR		BIT_15			
/* Force Event for CPL Timeout Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_CPL_TIMEOUT_ERR		BIT_14			
/* Force Event for AXI Response Bit Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_AXI_RESP_ERR			BIT_13			
/* Force Event for SPI Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_SPI_ERR				BIT_12			
/*		Bit(s) FORCE_EVENT_FOR_ERROR_STATUS_RSRV_11_10 reserved */
/* Force Event for ADMA Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_ADMA_ERR				BIT_9			
/* Force Event for Auto Cmd12 Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_ACMD12_ERR			BIT_8			
/* Force Event for Current Limit Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_CURRENT_ERR			BIT_7			
/* Force Event for Data End Bit Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_DAT_END_BIT_ERR		BIT_6			
/* Force Event for Data CRC Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_DAT_CRC_ERR			BIT_5			
/* Force Event for Data Timeout Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_DAT_TO_ERR			BIT_4			
/* Force Event for Command Index Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_CMD_INDEX_ERR			BIT_3			
/* Force Event for Command End Bit Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_CMD_END_BIT_ERR		BIT_2			
/* Force Event for Command CRC Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_CMD_CRC_ERR			BIT_1			
/* Force Event for Command Timeout Error */
#define	FORCE_EVENT_FOR_ERROR_STATUS_F_CMD_TO_ERR			BIT_0			

/*	ADMA_ERROR_STATUS			0x0054	ADMA Error Status Register */
/*		Bit(s) ADMA_ERROR_STATUS_RSRV_15_3 reserved */
/* ADMA Length Mismatch Error */
#define	ADMA_ERROR_STATUS_ADMA_LEN_ERR			BIT_2			
/* ADMA Error State */
#define	ADMA_ERROR_STATUS_ADMA_STATE_MSK			SHIFT0(0x3)		
#define	ADMA_ERROR_STATUS_ADMA_STATE_BASE		0

/*	ADMA_SYS_ADDR_1				0x0058	ADMA System Address Register 1 */
/* ADMA System Address */
#define	ADMA_SYS_ADDR_1_ADMA_SYS_ADDR_MSK		SHIFT0(0xffff)	
#define	ADMA_SYS_ADDR_1_ADMA_SYS_ADDR_BASE		0

/*	ADMA_SYS_ADDR_2				0x005A	ADMA System Address Register 2 */
/* ADMA System Address */
#define	ADMA_SYS_ADDR_2_ADMA_SYS_ADDR_MSK		SHIFT0(0xffff)	
#define	ADMA_SYS_ADDR_2_ADMA_SYS_ADDR_BASE		0

/*	ADMA_SYS_ADDR_3				0x005C	ADMA System Address Register 3 */
/* ADMA System Address */
#define	ADMA_SYS_ADDR_3_ADMA_SYS_ADDR_MSK		SHIFT0(0xffff)	
#define	ADMA_SYS_ADDR_3_ADMA_SYS_ADDR_BASE		0

/*	ADMA_SYS_ADDR_4				0x005E	ADMA System Address Register 4 */
/* ADMA System Address */
#define	ADMA_SYS_ADDR_4_ADMA_SYS_ADDR_MSK		SHIFT0(0xffff)	
#define	ADMA_SYS_ADDR_4_ADMA_SYS_ADDR_BASE		0

//Preset Value Register for Initialization (PRESET_VALUE_FOR_INIT) NEVO C0
#define	PRESET_VALUE_FOR_INIT_DRV_STRENGTH_VAL_MSK    SHIFT14(0x3)  
#define	PRESET_VALUE_FOR_INIT_DRV_STRENGTH_VAL_BASE    14
#define	PRESET_VALUE_FOR_INIT_CLKGEN_SEL_VAL        BIT_10
#define	PRESET_VALUE_FOR_INIT_SDCLK_FREQ_SEL_VAL_MSK    SHIFT0(0x3ff)  
#define	PRESET_VALUE_FOR_INIT_SDCLK_FREQ_SEL_VAL_BASE    0



//Preset Value Register for Default Speed (PRESET_VALUE_FOR_DS) NEVO C0
#define	PRESET_VALUE_FOR_DS_DRV_STRENGTH_VAL_MSK    SHIFT14(0x3)  
#define	PRESET_VALUE_FOR_DS_DRV_STRENGTH_VAL_BASE    14
#define	PRESET_VALUE_FOR_DS_CLKGEN_SEL_VAL        BIT_10
#define	PRESET_VALUE_FOR_DS_SDCLK_FREQ_SEL_VAL_MSK    SHIFT0(0x3ff)  
#define	PRESET_VALUE_FOR_DS_SDCLK_FREQ_SEL_VAL_BASE    0

//Preset Value Register for High Speed (PRESET_VALUE_FOR_HS) NEVO C0
#define	PRESET_VALUE_FOR_HS_DRV_STRENGTH_VAL_MSK    SHIFT14(0x3)  
#define	PRESET_VALUE_FOR_HS_DRV_STRENGTH_VAL_BASE    14
#define	PRESET_VALUE_FOR_HS_CLKGEN_SEL_VAL        BIT_10
#define	PRESET_VALUE_FOR_HS_SDCLK_FREQ_SEL_VAL_MSK    SHIFT0(0x3ff)  
#define	PRESET_VALUE_FOR_HS_SDCLK_FREQ_SEL_VAL_BASE    0


//Preset Value Register for SDR12 (PRESET_VALUE_FOR_SDR12) NEVO C0
#define	PRESET_VALUE_FOR_SDR12_DRV_STRENGTH_VAL_MSK    SHIFT14(0x3)  
#define	PRESET_VALUE_FOR_SDR12_DRV_STRENGTH_VAL_BASE    14
#define	PRESET_VALUE_FOR_SDR12_CLKGEN_SEL_VAL        BIT_10
#define	PRESET_VALUE_FOR_SDR12_SDCLK_FREQ_SEL_VAL_MSK    SHIFT0(0x3ff)  
#define	PRESET_VALUE_FOR_SDR12_SDCLK_FREQ_SEL_VAL_BASE    0

//Preset Value Register for SDR25 (PRESET_VALUE_FOR_SDR25) NEVO C0
#define	PRESET_VALUE_FOR_SDR25_DRV_STRENGTH_VAL_MSK    SHIFT14(0x3)  
#define	PRESET_VALUE_FOR_SDR25_DRV_STRENGTH_VAL_BASE    14
#define	PRESET_VALUE_FOR_SDR25_CLKGEN_SEL_VAL        BIT_10
#define	PRESET_VALUE_FOR_SDR25_SDCLK_FREQ_SEL_VAL_MSK    SHIFT0(0x3ff)  
#define	PRESET_VALUE_FOR_SDR25_SDCLK_FREQ_SEL_VAL_BASE    0



//Preset Value Register for SDR50 (PRESET_VALUE_FOR_SDR50) nevo c0
#define	PRESET_VALUE_FOR_SDR50_DRV_STRENGTH_VAL_MSK    SHIFT14(0x3)  
#define	PRESET_VALUE_FOR_SDR50_DRV_STRENGTH_VAL_BASE    14
#define	PRESET_VALUE_FOR_SDR50_CLKGEN_SEL_VAL        BIT_10
#define	PRESET_VALUE_FOR_SDR50_SDCLK_FREQ_SEL_VAL_MSK    SHIFT0(0x3ff)  
#define	PRESET_VALUE_FOR_SDR50_SDCLK_FREQ_SEL_VAL_BASE    0


//Preset Value Register for SDR104 (PRESET_VALUE_FOR_SDR104) NEVO C0
#define	PRESET_VALUE_FOR_SDR104_DRV_STRENGTH_VAL_MSK    SHIFT14(0x3)  
#define	PRESET_VALUE_FOR_SDR104_DRV_STRENGTH_VAL_BASE    14
#define	PRESET_VALUE_FOR_SDR104_CLKGEN_SEL_VAL        BIT_10
#define	PRESET_VALUE_FOR_SDR104_SDCLK_FREQ_SEL_VAL_MSK    SHIFT0(0x3ff)  
#define	PRESET_VALUE_FOR_SDR104_SDCLK_FREQ_SEL_VAL_BASE    0

//Preset Value Register for DDR50 (PRESET_VALUE_FOR_DDR50)  NEVO C0
#define	PRESET_VALUE_FOR_DDR50_DRV_STRENGTH_VAL_MSK    SHIFT14(0x3)  
#define	PRESET_VALUE_FOR_DDR50_DRV_STRENGTH_VAL_BASE    14
#define	PRESET_VALUE_FOR_DDR50_CLKGEN_SEL_VAL        BIT_10
#define	PRESET_VALUE_FOR_DDR50_SDCLK_FREQ_SEL_VAL_MSK    SHIFT0(0x3ff)  
#define	PRESET_VALUE_FOR_DDR50_SDCLK_FREQ_SEL_VAL_BASE    0

//Shared Bus Control Register (SHARED_BUS_CTRL)NEVO C0
#define	SHARED_BUS_CTRL_BEND_PWR_CTRL_MSK SHIFT24(0x7f)
#define	SHARED_BUS_CTRL_BEND_PWR_CTRL_BASE  24
#define	SHARED_BUS_CTRL_INT_PIN_SEL_MSK   SHIFT20(0x7)
#define	SHARED_BUS_CTRL_INT_PIN_SEL_BASE    20
#define	SHARED_BUS_CTRL_CLK_PIN_SEL_MSK   SHIFT16(0x7)
#define	SHARED_BUS_CTRL_CLK_PIN_SEL_BASE 16
#define	SHARED_BUS_CTRL_BUS_WIDTH_PRESET_MSK   SHIFT8(0x7f)
#define	SHARED_BUS_CTRL_BUS_WIDTH_PRESET_BASE 8
#define	SHARED_BUS_CTRL_NUM_INT_PINS_MSK   SHIFT4(0x3)
#define	SHARED_BUS_CTRL_NUM_INT_PINS_BASE 4
#define	SHARED_BUS_CTRL_NUM_CLK_PINS_MSK   SHIFT0(0x7)
#define	SHARED_BUS_CTRL_NUM_CLK_PINS_BASE 0

//Host Control Version Register (HOST_CTRL_VER)NEVO C0
#define	HOST_CTRL_VER_VENDOR_VER_MSK   SHIFT8(0xff)
#define	HOST_CTRL_VER_VENDOR_VER_BASE   8
#define	HOST_CTRL_VER_VER_MSK   SHIFT0(0xff)
#define	HOST_CTRL_VER_VER_BASE   0

//SD Extra Parameters Register (CFG_FIFO_PARAM)NEVO C0
#define	CFG_FIFO_PARAM_GEN_PAD_CLK_CNT_MSK  SHIFT24(0xff)  
#define	CFG_FIFO_PARAM_GEN_PAD_CLK_CNT_BASE   24
#define	CFG_FIFO_PARAM_GEN_PAD_CLK_ON      BIT_6
#define	CFG_FIFO_PARAM_SQU_FULL_CHK     BIT_5
#define	CFG_FIFO_PARAM_SQU_EMPTY_CHK      BIT_4
#define	CFG_FIFO_PARAM_BOOT_ACK      BIT_3








/*	FIFO_PARAM					0x00E0	FIFO Parameters Register */
/*		Bit(s) FIFO_PARAM_RSRV_31_11 reserved */

#define	FIFO_PARAM_PRE_GATE_CLK_CNT_MSK       SHIFT16(0xf)   // NEVO C0 
#define	FIFO_PARAM_PRE_GATE_CLK_CNT_BASE     16
#define	FIFO_PARAM_PDLVMC              BIT_14   // NEVO C0 
#define	FIFO_PARAM_PDFVSSM              BIT_13   // NEVO C0 
#define	FIFO_PARAM_FORCE_CLK_ON              BIT_12   // NEVO C0 
#define	FIFO_PARAM_OVRRD_CLK_OEN             BIT_11   // NEVO C0 

/* Disable PAD SD Clock Gating */
#define	FIFO_PARAM_DIS_PAD_SD_CLK_GATE		BIT_10				
/* Clock Gate On */
#define	FIFO_PARAM_CLK_GATE_ON				BIT_9				
/* Clock Gate Ctl */
#define	FIFO_PARAM_CLK_GATE_CTL				BIT_8				
/*		Bit(s) FIFO_PARAM_RSRV_7 reserved */
#define	FIFO_PARAM_USE_DAT3						BIT_7	
#define	FIFO_PARAM_PDWN						BIT_6				/* Power Down */
#define	FIFO_PARAM_FIFO_CS					BIT_5				/* FIFO CS */
#define	FIFO_PARAM_FIFO_CLK					BIT_4				/* FIFO Clock */
#define	FIFO_PARAM_WTC_MSK					SHIFT2(0x3)			/* WTC */
#define	FIFO_PARAM_WTC_BASE					2
#define	FIFO_PARAM_RTC_MSK					SHIFT0(0x3)			/* RTC */
#define	FIFO_PARAM_RTC_BASE					0

/*	SPI_MODE						0x00E4	SPI Mode Register */
/*		Bit(s) SPI_MODE_RSRV_15_13 reserved */
#define	SPI_MODE_SPI_ERR_TOKEN_MSK		SHIFT8(0x1f)	/* SPI Error Token */
#define	SPI_MODE_SPI_ERR_TOKEN_BASE		8
/*		Bit(s) SPI_MODE_RSRV_7_1 reserved */
#define	SPI_MODE_SPI_EN					BIT_0			/* Enable SPI Mode */

/*	CLOCK_AND_BURST_SIZE_SETUP	0x00E6	Clock and Burst Size Setup Register */
/*		Bit(s) CLOCK_AND_BURST_SIZE_SETUP_RSRV_15_14 reserved */
/* clk Delay Value */
#define	CLOCK_AND_BURST_SIZE_SETUP_WR_OSTDG				BIT_15	//NEVO C0
#define	CLOCK_AND_BURST_SIZE_SETUP_RD_OSTDG				BIT_14	//NEVO C0

#define	CLOCK_AND_BURST_SIZE_SETUP_SDCLK_DELAY_MSK		SHIFT10(0xf)	
#define	CLOCK_AND_BURST_SIZE_SETUP_SDCLK_DELAY_BASE		10
/* clk Select */
#define	CLOCK_AND_BURST_SIZE_SETUP_SDCLK_SEL_MSK			SHIFT8(0x3)		
#define	CLOCK_AND_BURST_SIZE_SETUP_SDCLK_SEL_BASE		8
/* Write Endian */
#define	CLOCK_AND_BURST_SIZE_SETUP_WR_ENDIAN				BIT_7			
/* Read Endian */
#define	CLOCK_AND_BURST_SIZE_SETUP_RD_ENDIAN				BIT_6			
/* AXI Non-post Write */
#define	CLOCK_AND_BURST_SIZE_SETUP_AXI_NON_POST_WR		BIT_5			
/* AXI priority arbitration request */
#define	CLOCK_AND_BURST_SIZE_SETUP_PRIORITY				BIT_4			
/* FIFO Threshold */
#define	CLOCK_AND_BURST_SIZE_SETUP_DMA_SIZE_MSK			SHIFT2(0x3)		
#define	CLOCK_AND_BURST_SIZE_SETUP_DMA_SIZE_BASE			2
/* DMA Burst Size on the AXI Fabric */
#define	CLOCK_AND_BURST_SIZE_SETUP_BRST_SIZE_MSK			SHIFT0(0x3)		
#define	CLOCK_AND_BURST_SIZE_SETUP_BRST_SIZE_BASE		0

/*	CE_ATA_1						0x00E8	CE-ATA Register 1 */
/*		Bit(s) CE_ATA_1_RSRV_15_14 reserved */
/* Command Completion Signal Timeout Value */
#define	CE_ATA_1_CPL_TIMEOUT_MSK			SHIFT0(0x3fff)	
#define	CE_ATA_1_CPL_TIMEOUT_BASE		0

/*	CE_ATA_2						0x00EA	CE-ATA Register 2 */
/* Check Command Completion Signal */
#define	CE_ATA_2_CHK_CPL					BIT_15			
/* Send Command Completion Disable Signal */
#define	CE_ATA_2_SND_CPL					BIT_14			
#define	CE_ATA_2_CEATA_CARD				BIT_13			/* CE-ATA Card */
#define	CE_ATA_2_MMC_CARD				BIT_12			/* MMC Card */
/*		Bit(s) CE_ATA_2_RSRV_11 reserved */
#define CE_ATA_2_MMC_HS200    BIT_10  /* MMC HS200 mode enable bit */
#define	CE_ATA_2_MMC_DDR					BIT_9			/* MMC Double Data Rate */
#define	CE_ATA_2_MMC_WIDTH				BIT_8			/* MMC Width */
/*		Bit(s) CE_ATA_2_RSRV_7 reserved */
#define	CE_ATA_2_CPL_COMPLETE			BIT_6			/* cpl_complete */
#define	CE_ATA_2_CPL_COMPLETE_EN			BIT_5			/* cpl_complete Enable */
/* cpl_complete Interrupt Enable */
#define	CE_ATA_2_CPL_COMPLETE_INT_EN		BIT_4	
#define	CE_ATA_2_MISC_INT		BIT_2           //NEVO C0
#define	CE_ATA_2_MISC_INT_EN	        BIT_1         //NEVO C0
#define	CE_ATA_2_MISC_INT_INT_EN		BIT_0         //NEVO C0

		
/*		Bit(s) CE_ATA_2_RSRV_3_0 reserved */

/*	PAD_IO_SETUP					0x00EC	PAD I/O Setup Register */
/*		Bit(s) PAD_IO_SETUP_RSRV_15_1 reserved */
/* SD Voltage Configuration */
#define	PAD_IO_SETUP_SDH_V18_EN			BIT_0			




//RX Configuration Register (RX_CFG_REG)NEVO C0
#define	RX_CFG_REG_TUNING_DLY_INC_MSK  SHIFT17(0x3ff)
#define	RX_CFG_REG_TUNING_DLY_INC_BASE   18
#define RX_CFG_REG_SDCLK_DELAY_MSK SHIFT8(0x3ff)
#define RX_CFG_REG_SDCLK_DELAY_BASE 8
#define	RX_CFG_REG_SDCLK_SEL1_MSK  SHIFT2(0x3)
#define	RX_CFG_REG_SDCLK_SEL1_BASE   2
#define	RX_CFG_REG_SDCLK_SEL0_MSK  SHIFT0(0x3)
#define	RX_CFG_REG_SDCLK_SEL0_BASE   0


//TX Configuration Register (TX_CFG_REG) NEVO C0
#define	TX_CFG_REG_TX_MUX_SEL				    BIT_31	
#define	TX_CFG_REG_TX_INT_CLK_SEL				 BIT_30	
#define	TX_CFG_REG_TX_HOLD_DELAY1_MSK  SHIFT16(0x3ff)
#define	TX_CFG_REG_TX_HOLD_DELAY1_BASE 16
#define	TX_CFG_REG_TX_HOLD_DELAY0_MSK  SHIFT0(0x3ff)
#define	TX_CFG_REG_TX_HOLD_DELAY0_BASE  0

// TUNING CONFIG Register 0x11c
#define TUNING_CFG_REG_TUNING_SUCCESS_CNT_MSK	SHIFT18(0x3f)
#define TUNING_CFG_REG_TUNING_SUCCESS_CNT_BASE	24
#define TUNING_CFG_REG_TUNING_CLK_DLY_MSK	SHIFT18(0x3ff)
#define TUNING_CFG_REG_TUNING_CLK_DLY_BASE	14
#define TUNING_CFG_REG_TUNING_WD_CNT_MSK	SHIFT18(0x3f)
#define TUNING_CFG_REG_TUNING_WD_CNT_BASE	8
#define TUNING_CFG_REG_TUNING_TT_CNT_MSK	SHIFT18(0xff)
#define TUNING_CFG_REG_TUNING_TT_CNT_BASE	0

// FLC Function Register 0x148
#define FLC_FUNC_REG_FLC_SW_RST         BIT_31
#define FLC_FUNC_REG_FLC_DADDR_MODE     BIT_9
#define FLC_FUNC_REG_FLC_HW_RESUME_EN   BIT_8
#define FLC_FUNC_REG_FLC_ABORT_INT      BIT_3
#define FLC_FUNC_REG_FLC_INT_EN         BIT_2
#define FLC_FUNC_REG_FLC_ABORT_EN       BIT_1
#define FLC_FUNC_REG_FLC_EN             BIT_0

/*	SLOT_INT_STATUS				0x00FC	Slot Interrupt Status Register */
/*		Bit(s) SLOT_INT_STATUS_RSRV_15_4 reserved */
/* Interrupt Line for Slot 3 */
#define	SLOT_INT_STATUS_SLOT_INT3			BIT_3			
/* Interrupt Line for Slot 2 */
#define	SLOT_INT_STATUS_SLOT_INT2			BIT_2			
/* Interrupt Line for Slot 1 */
#define	SLOT_INT_STATUS_SLOT_INT1			BIT_1			
/* Interrupt Line for Slot 0 */
#define	SLOT_INT_STATUS_SLOT_INT0			BIT_0			

/*	HOST_CTRL_VER				0x00FE	Host Control Version Register */
/* Marvell Version Number */
#define	HOST_CTRL_VER_VENDOR_VER_MSK			SHIFT8(0xff)	
#define	HOST_CTRL_VER_VENDOR_VER_BASE		8
/* SD Host Specification Number */
#define	HOST_CTRL_VER_SD_VER_MSK				SHIFT0(0xff)	
#define	HOST_CTRL_VER_SD_VER_BASE			0

#endif
