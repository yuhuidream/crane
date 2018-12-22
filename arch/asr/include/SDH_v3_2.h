#ifndef __SD_v_3_2_H__
#define __SD_v_3_2_H__

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
      
    __IO uint16_t PRESET_VALUE_FOR_INIT          ; /* 0x0060 Preset Value Register for Initialization */
    __IO uint16_t PRESET_VALUE_FOR_DS            ; /* 0x0062 Preset Value Register for Default Speed */ 
    __IO uint16_t PRESET_VALUE_FOR_HS            ; /* 0x0064 Preset Value Register for High Speed */    
    __IO uint16_t PRESET_VALUE_FOR_SDR12         ; /* 0x0066 Preset Value Register for SDR12 */         
    __IO uint16_t PRESET_VALUE_FOR_SDR25         ; /* 0x0068 Preset Value Register for SDR25 */         
    __IO uint16_t PRESET_VALUE_FOR_SDR50         ; /* 0x006A Preset Value Register for SDR50 */         
    __IO uint16_t PRESET_VALUE_FOR_SDR104        ; /* 0x006C Preset Value Register for SDR104 */        
    __IO uint16_t PRESET_VALUE_FOR_DDR50         ; /* 0x006E Preset Value Register for DDR50 */  
    
    __IO uint16_t RESERVED1[56];
           
    __IO uint32_t SHARED_BUS_CTRL                ; /* 0x00E0 Shared Bus Control Register */    
    
    __IO uint16_t RESERVED2[12];

    __IO uint16_t SLOT_INT_STATUS                ; /* 0x00FC Slot Interrupt Status Register */          
    __IO uint16_t HOST_CTRL_VER                  ; /* 0x00FE Host Control Version Register */           

    __IO uint32_t SDHC_IPID                      ; /* 0x0100 SDHC IP Core ID Register */            
    __IO uint32_t SDHC_SYS_CFG_INFO              ; /* 0x0104 SDHC System Conguration Information Register */   
    __IO uint32_t SDHC_SYS_OP_CTRL               ; /* 0x0108 SDHC System Operation Control Register */    
    __IO uint32_t SDHC_SYS_EXT_OP_CTRL           ; /* 0x010C SDHC System Extend Operation Control Register */ 
    __IO uint32_t SDHC_TEST_OUT                  ; /* 0x0110 SDHC Test Output Register */  
    __IO uint32_t SDHC_TESTOUT_MUXSEL            ; /* 0x0114 SDHC Test Output Mux Select Register */  

    __IO uint32_t RESERVED3[2];

    __IO uint16_t SDHC_SLOT_EXT_INT_STATUS       ; /* 0x0120 SDHC Slot Extend Interrupt Status Register */  
    __IO uint16_t SDHC_SLOT_EXT_ERR_STATUS       ; /* 0x0122 SDHC Slot Extend Error Status Register */  
    __IO uint16_t SDHC_SLOT_EXT_INT_STATUS_EN    ; /* 0x0124 SDHC Slot Extend Interrupt Status Enable Register */  
    __IO uint16_t SDHC_SLOT_EXT_ERR_STATUS_EN    ; /* 0x0126 SDHC Slot Extend Error Status Enable Register */  

    __IO uint32_t SDHC_SLOT_OP_STATUS_CTRL       ; /* 0x0128 SDHC Slot Operation Control and Status Register */  
    __IO uint32_t SDHC_SLOT_FIFO_CTRL            ; /* 0x012C SDHC Slot FIFO Register */  
    __IO uint32_t SDHC_SLOT_eMMC_CTRL            ; /* 0x0130 SDHC Slot eMMC Mode Control Register */  

    __IO uint32_t RESERVED4[4];

    __IO uint32_t SDHC_SLOT_RETUNING_REQ_CTRL    ; /* 0x0144 SDHC Slot Retuning Request Control Register */
 
    __IO uint32_t RESERVED5[1];

    __IO uint32_t SDHC_SLOT_EXT_PRESENT_STATE    ; /* 0x014C SDHC Slot Extend Present State Register */
    __IO uint32_t SDHC_SLOT_DLL_CUR_DLY_VAL      ; /* 0x0150 SDHC Slot DLL Current Delay Value Register */
    __IO uint32_t SDHC_SLOT_TUNING_CUR_DLY_VAL   ; /* 0x0154 SDHC Slot Tuning Current Delay Value Register */
    __IO uint32_t SDHC_SLOT_STROBE_CUR_DLY_VAL   ; /* 0x0158 SDHC Slot Strobe Current Delay Value Register */
    __IO uint32_t SDHC_SLOT_SUB_CMD_CTRL         ; /* 0x015C SDHC Slot Sub Command Control */
    __IO uint32_t SDHC_SLOT_CQ_TASK_INFO         ; /* 0x160 SDHC Slot Command Queue Task Information */

    __IO uint32_t RESERVED6[(0x170-0x160)/4-1];

    __IO uint32_t EMMC_PHY_TIMING_ADJUST         ; /* 0x0170 eMMC PHY Timing Adjust */
    __IO uint32_t EMMC_PHY_FUNC_ONTROL           ; /* 0x0174 eMMC PHY Func Control */
    __IO uint32_t EMMC_PHY_PAD_CONTROL           ; /* 0x0178 eMMC PHY Pad Control */
    __IO uint32_t EMMC_PHY_PAD_CONTROL1          ; /* 0x017C eMMC PHY Pad Control 1 */
    __IO uint32_t EMMC_PHY_PAD_CONTROL2          ; /* 0x0180 eMMC PHY Pad Control 2 */
    __IO uint32_t EMMC_PHY_DLL_CONTROL           ; /* 0x0184 eMMC PHY DLL Control */
    __IO uint32_t EMMC_LOGIC_TIMING_ADJUST       ; /* 0x0188 eMMC Logic Timing Adjust */
    __IO uint32_t EMMC_LOGIC_TIMING_ADJUST_LOW   ; /* 0x018C eMMC Logic Timing Adjust For Low Speed */
    __IO uint32_t PHY_LB_FUNC_CONFIG             ; /* 0x0190 eMMC PHY Loopback Func Config */
    __IO uint32_t PHY_LB_DATA_PATTERN            ; /* 0x0194 eMMC PHY Loopback Data Pattern */
    __IO uint32_t PHY_LB_FUNC_CONTROL            ; /* 0x0198 eMMC PHY Loopback Func Control */
    __IO uint32_t PHY_LB_ERR_STATUS              ; /* 0x019C eMMC PHY Loopback Err Status */
    __IO uint32_t PHY_LB_COMP_CNT                ; /* 0x01A0 eMMC PHY Loopback Comparison Count */

    __IO uint32_t RESERVED7[(0x1E0-0x1A0)/4-1];

    __IO uint32_t TP_CONTROL                     ; /* SDHC Timing Parameter Controlling */
    __IO uint32_t TP_MONITOR                     ; /* SDHC Timing Parameter Monitoring */

    __IO uint32_t RESERVED8[(0x200-0x1E4)/4-1];

    __IO uint32_t CQ_VER                         ; /* 0x200 CQVER-Command Queuing Version */
    __IO uint32_t CQ_CAP                         ; /* 0x204 CQCAP-Command Queuing Capabilities */
    __IO uint32_t CQ_CONFIG                      ; /* 0x208 CQCFG-Command Queuing Configuration */
    __IO uint32_t CQ_CONTROL                     ; /* 0x20C CQCTL-SDHC Command Queuing Control */
    __IO uint32_t CQ_INT_STATUS                  ; /* 0x210 CQIS-Command Queuing Interrupt Status */
    __IO uint32_t CQ_INT_STATUS_EN               ; /* 0x214 CQISTE-Command Queuing Interrupt Status Enable */
    __IO uint32_t CQ_INT_SIGNAL_EN               ; /* 0x218 CQISGE-Command Queuing Interrupt Signal Enable */
    __IO uint32_t CQ_INT_COALESCING              ; /* 0x21C CQIC-Command Queuing Interrupt Coalescing */
    __IO uint32_t CQ_TDL_BASE                    ; /* 0x220 CQTDLBA-Command Queuing Task Descriptor List Base Address*/
    __IO uint32_t CQ_TDL_BASE_UP                 ; /* 0x224 CQLBAU-Command Queuing Task Descriptor List Base Address Upper 32 bits */
    __IO uint32_t CQ_DOORBELL                    ; /* 0x228 CQTDBR-Command Queuing Task Doorbell */
    __IO uint32_t CQ_TCN                         ; /* 0x22C CQTCN-Command Queuing Task Completion Notification */
    __IO uint32_t CQ_QUEUE_STATUS                ; /* 0x230 CQDQS-Command Queuing Device Queue Status */
    __IO uint32_t CQ_PENDING_TASKS               ; /* 0x234 CQDPT-Command Queuing Device Pending Tasks */
    __IO uint32_t CQ_TASK_CLEAR                  ; /* 0x238 CQTCLR-Command Queuing Task Clear */

    __IO uint32_t RESERVED9[(0x240-0x238)/4-1];

    __IO uint32_t CQ_SQS_CONF1                   ; /* 0x240 CQSST-Command Queuing Configuration 1 */
    __IO uint32_t CQ_SQS_CONF2                   ; /* 0x244 CQSSBC-Command Queuing Configuration 2 */
    __IO uint32_t CQ_DCMD_RESP                   ; /* 0x248 CQCRDCT-Command Queuing Command Response for
Direct-Command Task */

    __IO uint32_t RESERVED10[(0x250-0x248)/4-1];

    __IO uint32_t CQ_RESP_ERR_MASK               ; /* 0x250 CQRMEM-Command Queuing Response Mode Error
Mask */
    __IO uint32_t CQ_TASK_ERR_INFO               ; /* 0x254 CQTERRI-Command Queuing Task Error Information */
    __IO uint32_t CQ_CMD_RESP_INDEX              ; /* 0x258 CQCRI-Command Queuing Command Response Index */
    __IO uint32_t CQ_CMD_RESP_ARG                ; /* 0x25C CQCRA-Command Queuing Command Response Argument */


}SD_v3_2_TypeDef;


#define    SD3_BASE      0xD4290000
#define    SD3  (( SD_v3_2_TypeDef *) SD3_BASE )

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

/*  HOST_CTRL_VER               0x00FE  Host Control Version Register */
/* Marvell Version Number */
#define	HOST_CTRL_VER_VENDOR_VER_MSK   SHIFT8(0xff)
#define	HOST_CTRL_VER_VENDOR_VER_BASE   8
/* SD Host Specification Number */
#define	HOST_CTRL_VER_VER_MSK   SHIFT0(0xff)
#define	HOST_CTRL_VER_VER_BASE   0

/*  SDHC_IPID                           0x0100  xml_file/SDHOST_customer SDHC
 *                                              IP Core ID Register
 */
#define SDHC_IPID_SDHC_IPID_IPRELTAG_MSK            SHIFT12(0xfffff)        /* SDHC IP Release Tag */
#define SDHC_IPID_SDHC_IPID_IPRELTAG_BASE           12
#define SDHC_IPID_SHDC_IPID_MAIN_MSK                SHIFT0(0xfff)           /* SDHC IP ID */
#define SDHC_IPID_SHDC_IPID_MAIN_BASE               0

/*  SDHC_SYS_CFG_INFO                   0x0104  xml_file/SDHOST_customer SDHC
 *                                              System Configuration Information
 *                                              Register
 */
/* SDIO Card Inserted */
#define SDHC_SYS_CFG_INFO_SDIO_INSERTED_MSK                     SHIFT24(0xff)       
#define SDHC_SYS_CFG_INFO_SDIO_INSERTED_BASE                    24
/* Slot type indication for eMMC */
#define SDHC_SYS_CFG_INFO_SLOT_TYPE_EMMC_MSK                    SHIFT16(0xff)       
#define SDHC_SYS_CFG_INFO_SLOT_TYPE_EMMC_BASE                   16
/* Slot type indication for SD or SDIO or MMC */
#define SDHC_SYS_CFG_INFO_SLOT_TYPE_SD_SDIO_MMC_MSK             SHIFT8(0xff)        
#define SDHC_SYS_CFG_INFO_SLOT_TYPE_SD_SDIO_MMC_BASE            8
/* The Number of Slots Supported in This Controller. */
#define SDHC_SYS_CFG_INFO_NUM_SUPPORTED_SLOT_MSK                SHIFT0(0xff)        
#define SDHC_SYS_CFG_INFO_NUM_SUPPORTED_SLOT_BASE               0

/*  SDHC_SYS_OP_CTRL                    0x0108  xml_file/SDHOST_customer SDHC
 *                                              System Operation Control Register
 */
#define SDHC_SYS_OP_CTRL_SLOT_PRIORITY_LEVEL_MSK            SHIFT24(0xff)       /* Slot Priority Assignment */
#define SDHC_SYS_OP_CTRL_SLOT_PRIORITY_LEVEL_BASE           24
#define SDHC_SYS_OP_CTRL_DMA_BURST_LEN_MSK                  SHIFT21(0x3)        /* DMA Burst Length */
#define SDHC_SYS_OP_CTRL_DMA_BURST_LEN_BASE                 21
#define SDHC_SYS_OP_CTRL_AUTO_CLKGATE_DISABLE               BIT_20              /* Disable Automatic Clock Gating */
#define SDHC_SYS_OP_CTRL_SDCLK_IDLEOFF_CNTR_MSK             SHIFT16(0xf)        /* The Number of Idle Cycles Required Before Turning off SDCLK for Power Saving. */
#define SDHC_SYS_OP_CTRL_SDCLK_IDLEOFF_CNTR_BASE            16
#define SDHC_SYS_OP_CTRL_SDCLK_IDLEOFF_ENABLE_MSK           SHIFT8(0xff)        /* Enable SDCLK off During Idle Cycles for Power Saving. */
#define SDHC_SYS_OP_CTRL_SDCLK_IDLEOFF_ENABLE_BASE          8
#define SDHC_SYS_OP_CTRL_SLOT_ENABLE_MSK                    SHIFT0(0xff)        /* Slot Enable */
#define SDHC_SYS_OP_CTRL_SLOT_ENABLE_BASE                   0

/*  SDHC_SYS_EXT_OP_CTRL                0x010C  xml_file/SDHOST_customer SDHC
 *                                              System Extend Operation Control
 *                                              Register
 */
/* Parallel Transfer Mode for Each Slot */
#define SDHC_SYS_EXT_OP_CTRL_PARALLEL_TRAN_MODE_SLOTS_MSK           SHIFT0(0xff)            
#define SDHC_SYS_EXT_OP_CTRL_PARALLEL_TRAN_MODE_SLOTS_BASE          0

/*  SDHC_TEST_OUT                       0x0110  xml_file/SDHOST_customer SDHC
 *                                              Test Output Register
 */
#define SDHC_TEST_OUT_TEST_OUTPUT_MSK           SHIFT0(0xff)            /* Test Output */
#define SDHC_TEST_OUT_TEST_OUTPUT_BASE          0

/*  SDHC_TESTOUT_MUXSEL                 0x0114  xml_file/SDHOST_customer SDHC
 *                                              Test Output Mux Select Register
 */
#define SDHC_TESTOUT_MUXSEL_TEST_MUXSEL_MSK             SHIFT0(0xff)            /* Test Mux Select */
#define SDHC_TESTOUT_MUXSEL_TEST_MUXSEL_BASE            0

/*  SDHC_SLOT_EXT_INT_STATUS            0x0120  xml_file/SDHOST_customer SDHC
 *                                              Slot Extend Interrupt Status
 *                                              Register
 */
/* Master DLL UNLOCK Event */
#define SDHC_SLOT_EXT_INT_STATUS_UNLOCK_EVENT                       BIT_5               
/* Master DLL LOCK Event */
#define SDHC_SLOT_EXT_INT_STATUS_LOCK_EVENT                         BIT_4               
/* eMMC Boot Acknowledgement Receive Status */
#define SDHC_SLOT_EXT_INT_STATUS_EMMC_BOOT_ACK_RCVD                 BIT_0               

/*  SDHC_SLOT_EXT_ERR_STATUS            0x0122  xml_file/SDHOST_customer SDHC
 *                                              Slot Extend Error Status Register
 */
/* AXI Slave Interface Read Channel Slave Error */
#define SDHC_SLOT_EXT_ERR_STATUS_AXI_SLVIF_RD_SLVERR                BIT_15
/* AXI Slave Interface Write Channel Slave Error */
#define SDHC_SLOT_EXT_ERR_STATUS_AXI_SLVIF_WR_SLVERR                BIT_14
/* Task Descriptor Fetch Error */
#define SDHC_SLOT_EXT_ERR_STATUS_TASK_FETCH_ERR                     BIT_13
/* Auto Retuning Error */
#define SDHC_SLOT_EXT_ERR_STATUS_AUTO_RETUNING_ERR                  BIT_7
/* eMMC Boot Acknowledge Error */
#define SDHC_SLOT_EXT_ERR_STATUS_EMMC_BOOT_ACK_ERR                  BIT_6               
/* AXI Master Interface Read Channel Decode Error. */
#define SDHC_SLOT_EXT_ERR_STATUS_AXI_MSTIF_RD_DECERR                BIT_5               
/* AXI Master Interface Read Channel Slave Error. */
#define SDHC_SLOT_EXT_ERR_STATUS_AXI_MSTIF_RD_SLVERR                BIT_4               
/* AXI Master Interface Read Channel Incompatible Response Error. */
#define SDHC_SLOT_EXT_ERR_STATUS_AXI_MSTIF_RD_ICPRESP               BIT_3               
/* AXI Master Interface Write Channel Decode Error. */
#define SDHC_SLOT_EXT_ERR_STATUS_AXI_MSTIF_WR_DECERR                BIT_2               
/* AXI Master Interface Write Channel Slave Error. */
#define SDHC_SLOT_EXT_ERR_STATUS_AXI_MSTIF_WR_SLVERR                BIT_1               
/* AXI Master Interface Write Channel Incompatible Response Error. */
#define SDHC_SLOT_EXT_ERR_STATUS_AXI_MSTIF_WR_ICPRESP               BIT_0               

/*  SDHC_SLOT_EXT_INT_STATUS_EN         0x0124  xml_file/SDHOST_customer SDHC
 *                                              Slot Extend Interrupt Status Enable
 *                                              Register
 */
/* UNLOCK Event Int Enable */
#define SDHC_SLOT_EXT_INT_STATUS_EN_UNLOCK_EVENT_EN                     BIT_5               
/* LOCK Event Int Enable */
#define SDHC_SLOT_EXT_INT_STATUS_EN_LOCK_EVENT_EN                       BIT_4               
/* eMMC Boot Acknowledgement Receive Status Enable */
#define SDHC_SLOT_EXT_INT_STATUS_EN_EMMC_BOOT_ACK_RCVD_EN               BIT_0               

/*  SDHC_SLOT_EXT_ERR_STATUS_EN         0x0126  xml_file/SDHOST_customer SDHC
 *                                              Slot Extend Error Status Enable
 *                                              Register
 */
/* AXI Slave Interface Read Channel Slave Error Enable */
#define SDHC_SLOT_EXT_ERR_STATUS_AXI_SLVIF_RD_SLVERR_EN                     BIT_15
/* AXI Slave Interface Write Channel Slave Error Enable */
#define SDHC_SLOT_EXT_ERR_STATUS_AXI_SLVIF_WR_SLVERR_EN                     BIT_14
/* Task Descriptor Fetch Error Enable */
#define SDHC_SLOT_EXT_ERR_STATUS_TASK_FETCH_ERR_EN                          BIT_13
/* Auto Retuning Error Enable */
#define SDHC_SLOT_EXT_ERR_STATUS_AUTO_RETUNING_ERR_EN                       BIT_7
/* eMMC Boot Acknowledge Error Enable */
#define SDHC_SLOT_EXT_ERR_STATUS_EN_EMMC_BOOT_ACK_ERR_EN                    BIT_6               
/* AXI Master Interface Read Channel Decode Error Enable */
#define SDHC_SLOT_EXT_ERR_STATUS_EN_AXI_MSTIF_RD_DECERR_EN                  BIT_5               
/* AXI Master Interface Read Channel Slave Error Enable */
#define SDHC_SLOT_EXT_ERR_STATUS_EN_AXI_MSTIF_RD_SLVERR_EN                  BIT_4               
/* AXI Master Interface Read Channel Incompatible Response Error Enable */
#define SDHC_SLOT_EXT_ERR_STATUS_EN_AXI_MSTIF_RD_ICPRESP_EN                 BIT_3               
/* AXI Master Interface Write Channel Decode Error Enable. */
#define SDHC_SLOT_EXT_ERR_STATUS_EN_AXI_MSTIF_WR_DECERR_EN                  BIT_2               
/* AXI Master Interface Write Channel Slave Error Enable. */
#define SDHC_SLOT_EXT_ERR_STATUS_EN_AXI_MSTIF_WR_SLVERR_EN                  BIT_1               
/* AXI Master Interface Write Channel Incompatible Response Error Enable. */
#define SDHC_SLOT_EXT_ERR_STATUS_EN_AXI_MSTIF_WR_ICPRESP_EN                 BIT_0               

/*  SDHC_SLOT_OP_STATUS_CTRL            0x0128  xml_file/SDHOST_customer SDHC
 *                                              Slot Operation Control and Status
 *                                              Register
 */
/* Auto FIFO Reset Enable */
#define SDHC_SLOT_OP_STATUS_CTRL_AUTO_FIFO_RST_EN                       BIT_24
/* The Number of Sampling Points of a Valid Sampling Window */
#define SDHC_SLOT_OP_STATUS_CTRL_CONSECUTIVE_TIMES_MSK                  SHIFT16(0x7)        
#define SDHC_SLOT_OP_STATUS_CTRL_CONSECUTIVE_TIMES_BASE                 16
/* A Tuning Step Delay */
#define SDHC_SLOT_OP_STATUS_CTRL_TUNING_STEP_MSK                        SHIFT12(0xf)        
#define SDHC_SLOT_OP_STATUS_CTRL_TUNING_STEP_BASE                       12
/* Choose Inverse Clock for Sampling */
#define SDHC_SLOT_OP_STATUS_CTRL_FORCE_SEL_INVERSE_CLK                  BIT_11              
/* Programmable Fixed Delay Value in Fixed Delay Tuning. */
#define SDHC_SLOT_OP_STATUS_CTRL_TUNING_PROG_FIXED_DELAY_MSK            SHIFT0(0x7ff)       
#define SDHC_SLOT_OP_STATUS_CTRL_TUNING_PROG_FIXED_DELAY_BASE           0

/*  SDHC_SLOT_FIFO_CTRL                 0x012C  xml_file/SDHOST_customer SDHC
 *                                              Slot FIFO Register
 */
#define SDHC_SLOT_FIFO_CTRL_FIFO_CQE_MEM_PDLVMC         BIT_22
#define SDHC_SLOT_FIFO_CTRL_FIFO_CQE_MEM_PDFVSSM        BIT_21
#define SDHC_SLOT_FIFO_CTRL_FIFO_CQE_MEM_PDWN           BIT_20
#define SDHC_SLOT_FIFO_CTRL_FIFO_CQE_MEM_WTC_MSK        SHIFT18(0x3)
#define SDHC_SLOT_FIFO_CTRL_FIFO_CQE_MEM_WTC_BASE       18
#define SDHC_SLOT_FIFO_CTRL_FIFO_CQE_MEM_RTC_MSK        SHIFT16(0x3)
#define SDHC_SLOT_FIFO_CTRL_FIFO_CQE_MEM_RTC_BASE       16
#define SDHC_SLOT_FIFO_CTRL_FIFO_PDLVMC                 BIT_8                   /* PDLVMC */
#define SDHC_SLOT_FIFO_CTRL_FIFO_PDFVSSM                BIT_7                   /* PDFVSSM */
#define SDHC_SLOT_FIFO_CTRL_FIFO_PWDN                   BIT_6                   /* PWDN */
#define SDHC_SLOT_FIFO_CTRL_FIFO_CS                     BIT_5                   /* FIFO CS */
#define SDHC_SLOT_FIFO_CTRL_FIFO_CLK                    BIT_4                   /* FIFO Clock */
#define SDHC_SLOT_FIFO_CTRL_FIFO_WTC_MSK                SHIFT2(0x3)             /* WTC */
#define SDHC_SLOT_FIFO_CTRL_FIFO_WTC_BASE               2
#define SDHC_SLOT_FIFO_CTRL_FIFO_RTC_MSK                SHIFT0(0x3)             /* RTC */
#define SDHC_SLOT_FIFO_CTRL_FIFO_RTC_BASE               0

/*  SDHC_SLOT_EMMC_CTRL                 0x0130  xml_file/SDHOST_customer SDHC
 *                                              Slot eMMC Mode Control Register
 */
/* eMMC Resp Strobe Enable */
#define SDHC_SLOT_EMMC_CTRL_ENABLE_RESP_STROBE                  BIT_25              
/* eMMC DATA Strobe Enable */
#define SDHC_SLOT_EMMC_CTRL_ENABLE_DATA_STROBE                  BIT_24              
/* eMMC Hardware Reset Control */
#define SDHC_SLOT_EMMC_CTRL_SET_EMMC_RSTN                       BIT_16              
/* Disable Read Data CRC */
#define SDHC_SLOT_EMMC_CTRL_DISABLE_RD_DATA_CRC                 BIT_14              
/* Disable Write Data CRC Status Token */
#define SDHC_SLOT_EMMC_CTRL_DISABLE_CRC_STAT_TOKEN              BIT_13              
/* Boot DDR Mode Enable */
#define SDHC_SLOT_EMMC_CTRL_BOOT_DDR_ENABLE                     BIT_12              
/* eMMC Boot Acknowledge */
#define SDHC_SLOT_EMMC_CTRL_BOOT_ACK_ENABLE                     BIT_11              
/* Execute eMMC Alternative Boot */
#define SDHC_SLOT_EMMC_CTRL_EXE_ALTER_BOOT                      BIT_10              
/* Terminate eMMC Default Boot */
#define SDHC_SLOT_EMMC_CTRL_TERMINATE_DFLT_BOOT                 BIT_9               
/* Execute eMMC Default Boot */
#define SDHC_SLOT_EMMC_CTRL_EXE_DEFAULT_BOOT                    BIT_8
/* eMMC Driver Strength Select */
#define SDHC_SLOT_EMMC_CTRL_EMMC_DRV_STRENGTH_SEL_MSK           SHIFT4(0x7)         
#define SDHC_SLOT_EMMC_CTRL_EMMC_DRV_STRENGTH_SEL_BASE          4
/* eMMC IO Voltage (Vccq) Select */
#define SDHC_SLOT_EMMC_CTRL_IO_VOLTAGE_SEL_MSK                  SHIFT0(0x3)         
#define SDHC_SLOT_EMMC_CTRL_IO_VOLTAGE_SEL_BASE                 0

/*  SDHC_SLOT_STROBE_DLY_CTRL           0x0140  xml_file/SDHOST_customer SDHC
 *                                              Slot Strobe Delay Control Register
 */
/* Delay Value for Strobe Signal */
#define SDHC_SLOT_STROBE_DLY_CTRL_STROBE_DELAY_FIXED_MSK            SHIFT0(0xffff)      
#define SDHC_SLOT_STROBE_DLY_CTRL_STROBE_DELAY_FIXED_BASE           0

/*  SDHC_SLOT_RETUNING_REQ_CTRL         0x0144  xml_file/SDHOST_customer SDHC
 *                                              Slot Retuning Request Control Register
 */
#define SDHC_SLOT_RETUNING_REQ_CTRL_RETUNING_THRESHOLD_MSK          SHIFT8(0xff)
#define SDHC_SLOT_RETUNING_REQ_CTRL__RETUNING_THRESHOLD_BASE        8
#define SDHC_SLOT_RETUNING_REQ_CTRL_RETUNING_COMPATIBLE             BIT_0


/*  SDHC_SLOT_EXT_PRESENT_STATE         0x014C  xml_file/SDHOST_customer SDHC
 *                                              Slot Extend Present State Register
 */
/* Pos Or Neg Select for Tuning */
#define SDHC_SLOT_EXT_PRESENT_STATE_POS_OR_NEG_SEL                      BIT_28              
/* Consecutive Tuning Success Times */
#define SDHC_SLOT_EXT_PRESENT_STATE_SUCCESS_TIMES_MSK                   SHIFT24(0xf)        
#define SDHC_SLOT_EXT_PRESENT_STATE_SUCCESS_TIMES_BASE                  24
/* Delay Value for Tuning Step From DLL */
#define SDHC_SLOT_EXT_PRESENT_STATE_TUNING_STEP_FROM_DLL_MSK            SHIFT16(0xff)       
#define SDHC_SLOT_EXT_PRESENT_STATE_TUNING_STEP_FROM_DLL_BASE           16
/* Resp in Asynchronous FIFO */
#define SDHC_SLOT_EXT_PRESENT_STATE_RESP_IN_ASYNC_FIFO                  BIT_9               
/* Data in Asynchronous FIFO */
#define SDHC_SLOT_EXT_PRESENT_STATE_DATA_IN_ASYNC_FIFO                  BIT_8               
/* Lock State */
#define SDHC_SLOT_EXT_PRESENT_STATE_LOCK_STATE                          BIT_0               

/*  SDHC_SLOT_DLL_CUR_DLY_VAL           0x0150  xml_file/SDHOST_customer SDHC
 *                                              Slot DLL Current Delay Value
 *                                              Register
 */
#define SDHC_SLOT_DLL_CUR_DLY_VAL_DVAL_IN_DLL_MSK           SHIFT0(0xffff)      /* Delay value of Main Delay Line */
#define SDHC_SLOT_DLL_CUR_DLY_VAL_DVAL_IN_DLL_BASE          0

/*  SDHC_SLOT_TUNING_CUR_DLY_VAL        0x0154  xml_file/SDHOST_customer SDHC
 *                                              Slot Tuning Current Delay Value
 *                                              Register
 */
/* Delay Value for Tuning Circuit */
#define SDHC_SLOT_TUNING_CUR_DLY_VAL_DVAL_TUNING_LINE_MSK           SHIFT0(0xffff)      
#define SDHC_SLOT_TUNING_CUR_DLY_VAL_DVAL_TUNING_LINE_BASE          0

/*  SDHC_SLOT_STROBE_CUR_DLY_VAL        0x0158  xml_file/SDHOST_customer SDHC
 *                                              Slot Strobe Current Delay Value
 *                                              Register
 */
/* Delay value For Strobe Signal Delay Line */
#define SDHC_SLOT_STROBE_CUR_DLY_VAL_DVAL_STROBE_MSK            SHIFT0(0xffff)      
#define SDHC_SLOT_STROBE_CUR_DLY_VAL_DVAL_STROBE_BASE           0

/*  SDHC_SLOT_SUB_CMD_CTRL              0x015C  xml_file/SDHOST_customer SDHC
 *                                              Slot Sub Command Control
 */
#define SDHC_SLOT_SUB_CMD_CTRL_BM_MSK               SHIFT0(0xffff)      /* Byte Margin */
#define SDHC_SLOT_SUB_CMD_CTRL_BM_BASE              0

/*  EMMC_PHY_TIMING_ADJUST              0x0160  xml_file/SDHOST_customer eMMC
 *                                              PHY Timing Adjust
 */
/* Host Mode Initialization */
#define EMMC_PHY_TIMING_ADJUST_PHY_INITIALIZAION                    BIT_31              
/* QSP After RDRST */
#define EMMC_PHY_TIMING_ADJUST_QSP_AFTER_RDRST_MSK                  SHIFT24(0xf)        
#define EMMC_PHY_TIMING_ADJUST_QSP_AFTER_RDRST_BASE                 24
/* QSP Phase Select */
#define EMMC_PHY_TIMING_ADJUST_QSP_PHASE_SELECT                     BIT_18              
/* QSN Phase Select */
#define EMMC_PHY_TIMING_ADJUST_QSN_PHASE_SELECT                     BIT_17              
/* Polarity for WR_LD0 and WR_LD1 */
#define EMMC_PHY_TIMING_ADJUST_POLARITY_WR_LD0_LD1                  BIT_16              
/* Wait Cycles before using PHY */
#define EMMC_PHY_TIMING_ADJUST_WAIT_CYCLE_BEFORE_USING_MSK          SHIFT12(0xf)        
#define EMMC_PHY_TIMING_ADJUST_WAIT_CYCLE_BEFORE_USING_BASE         12
/* Duartion for FC_SYNC_EN */
#define EMMC_PHY_TIMING_ADJUST_FC_SYNC_EN_DURATION_MSK              SHIFT8(0xf)         
#define EMMC_PHY_TIMING_ADJUST_FC_SYNC_EN_DURATION_BASE             8
/* Duration between FC_SYNC_RST and FC_SYNC_RST */
#define EMMC_PHY_TIMING_ADJUST_FC_SYNC_RST_EN_DURATION_MSK          SHIFT4(0xf)         
#define EMMC_PHY_TIMING_ADJUST_FC_SYNC_RST_EN_DURATION_BASE         4
/* Duration for FC_SYNC_RST */
#define EMMC_PHY_TIMING_ADJUST_FC_SYNC_RST_DURATION_MSK             SHIFT0(0xf)         
#define EMMC_PHY_TIMING_ADJUST_FC_SYNC_RST_DURATION_BASE            0

/*  EMMC_PHY_FUNC_CONTROL               0x0164  xml_file/SDHOST_customer eMMC
 *                                              PHY Func Control
 */
#define EMMC_PHY_FUNC_CONTROL_QS_GATE_DLY_CTRL_MSK          SHIFT24(0x7f)       /* QS Gating Delay Control */
#define EMMC_PHY_FUNC_CONTROL_QS_GATE_DLY_CTRL_BASE         24
#define EMMC_PHY_FUNC_CONTROL_PHY_RESERVE_MSK               SHIFT17(0x3f)       /* PHY Reserve Bits */
#define EMMC_PHY_FUNC_CONTROL_PHY_RESERVE_BASE              17
#define EMMC_PHY_FUNC_CONTROL_CMD_DDRMODE                   BIT_16              /* CMD DDR Mode */
#define EMMC_PHY_FUNC_CONTROL_DQ_DDRMODE_MSK                SHIFT8(0xff)        /* DQ DDR Mode */
#define EMMC_PHY_FUNC_CONTROL_DQ_DDRMODE_BASE               8
#define EMMC_PHY_FUNC_CONTROL_CMD_MODE                      BIT_7               /* CMD Receiver Input Mode */
#define EMMC_PHY_FUNC_CONTROL_DQ_MODE                       BIT_6               /* DQ Receiver Input Mode */
#define EMMC_PHY_FUNC_CONTROL_CMD_ASYNC_MODE                BIT_5               /* CMD Asynchonous Read Mode */
#define EMMC_PHY_FUNC_CONTROL_DQ_ASYNC_MODE                 BIT_4               /* DQ Asynchonous Read Mode */
#define EMMC_PHY_FUNC_CONTROL_RFIFO_BYP                     BIT_3               /* Read FIFO Bypass */
#define EMMC_PHY_FUNC_CONTROL_DSER1X                        BIT_2               /* Serialization Mode 1x */
#define EMMC_PHY_FUNC_CONTROL_FC_WRST_SEL_MSK               SHIFT0(0x3)         /* Synchronization Reset Phase Select */
#define EMMC_PHY_FUNC_CONTROL_FC_WRST_SEL_BASE              0

/*  EMMC_PHY_PAD_CONTROL                0x0168  xml_file/SDHOST_customer eMMC
 *                                              PHY Pad Control
 */
#define EMMC_PHY_PAD_CONTROL_OEP                            BIT_29              /* Ouput Enable For QSP */
#define EMMC_PHY_PAD_CONTROL_OEN                            BIT_28              /* Ouput Enable For QSN */
#define EMMC_PHY_PAD_CONTROL_REC_EN_MSK                     SHIFT24(0xf)        /* Receiver Enable En */
#define EMMC_PHY_PAD_CONTROL_REC_EN_BASE                    24
#define EMMC_PHY_PAD_CONTROL_FC_QSN_RCVTYPE                 BIT_15              /* QSN Receiver Type */
#define EMMC_PHY_PAD_CONTROL_FC_QSP_RCVTYPE                 BIT_14              /* QSP Receiver Type */
#define EMMC_PHY_PAD_CONTROL_FC_CMD_RCVTYPE                 BIT_13              /* CMD Receiver Type */
#define EMMC_PHY_PAD_CONTROL_FC_DQ_RCVTYPE                  BIT_12              /* DQ Receiver Type */
#define EMMC_PHY_PAD_CONTROL_QSN_PWRDWN                     BIT_10              /* QSN Pad Power Down Control */
#define EMMC_PHY_PAD_CONTROL_QSP_PWRDWN                     BIT_9               /* QSP Pad Power Down Control */
#define EMMC_PHY_PAD_CONTROL_CMD_PWRDWN                     BIT_8               /* CMD Pad Power Down Control */
#define EMMC_PHY_PAD_CONTROL_DQ_PWRDWN_MSK                  SHIFT0(0xff)        /* DQ Pad Power Down Control */
#define EMMC_PHY_PAD_CONTROL_DQ_PWRDWN_BASE                 0

/*  EMMC_PHY_PAD_CONTROL1              0x016C  xml_file/SDHOST_customer eMMC
 *                                              PHY Pad Control 2
 */
#define EMMC_PHY_PAD_CONTROL1_FC_QSN_PU                     BIT_26              /* QSN Pullup Enable */
#define EMMC_PHY_PAD_CONTROL1_FC_QSP_PU                     BIT_25              /* QSP Pullup Enable */
#define EMMC_PHY_PAD_CONTROL1_FC_CMD_PU                     BIT_24              /* CMD Pullup Enable */
#define EMMC_PHY_PAD_CONTROL1_FC_DQ_PU_MSK                 SHIFT16(0xff)              /* DQ Pullup Enable */
#define EMMC_PHY_PAD_CONTROL1_FC_DQ_PU_BASE                 16              /* DQ Pullup Enable */
#define EMMC_PHY_PAD_CONTROL1_FC_QSN_PD                     BIT_10              /* QSN Pulldown Enable */
#define EMMC_PHY_PAD_CONTROL1_FC_QSP_PD                     BIT_9              /* QSP Pulldown Enable */
#define EMMC_PHY_PAD_CONTROL1_FC_CMD_PD                     BIT_8              /* CMD Pulldown Enable */
#define EMMC_PHY_PAD_CONTROL1_FC_DQ_PD_MSK                  SHIFT0(0xff)        /* DQ Pulldown Enable */
#define EMMC_PHY_PAD_CONTROL1_FC_DQ_PD_BASE                 0                   /* DQ Pulldown Enable */

/*  EMMC_PHY_PAD_CONTROL2               0x016C  xml_file/SDHOST_customer eMMC
 *                                              PHY Pad Control 2
 */
#define EMMC_PHY_PAD_CONTROL2_ZD                    BIT_18              /* Reserved Bit */
#define EMMC_PHY_PAD_CONTROL2_ODT_SEL1              BIT_17              /* ODT SEL1 */
#define EMMC_PHY_PAD_CONTROL2_ODT_SEL0              BIT_16              /* ODT SEL0 */
#define EMMC_PHY_PAD_CONTROL2_ZNR_MSK               SHIFT8(0x1f)        /* N Driver Strength */
#define EMMC_PHY_PAD_CONTROL2_ZNR_BASE              8
#define EMMC_PHY_PAD_CONTROL2_ZPR_MSK               SHIFT0(0x1f)        /* P Driver Strength */
#define EMMC_PHY_PAD_CONTROL2_ZPR_BASE              0

/*  EMMC_PHY_DLL_CONTROL                0x0170  xml_file/SDHOST_customer eMMC
 *                                              PHY DLL Control
 */
/* DLL Enable */
#define EMMC_PHY_DLL_CONTROL_DLL_ENABLE                         BIT_31              
/* DLL Update Strobe */
#define EMMC_PHY_DLL_CONTROL_DLL_REFCLK_SEL                     BIT_30
/* DLL Slave Phase Select 1 */
#define EMMC_PHY_DLL_CONTROL_DLL_PHSEL1_MSK                     SHIFT24(0x3f)       
#define EMMC_PHY_DLL_CONTROL_DLL_PHSEL1_BASE                    24
#define EMMC_PHY_DLL_CONTROL_DLL_UPDATE                         BIT_23
/* DLL Slave Phase Select 0 */
#define EMMC_PHY_DLL_CONTROL_DLL_PHSEL0_MSK                     SHIFT16(0x3f)       
#define EMMC_PHY_DLL_CONTROL_DLL_PHSEL0_BASE                    16
/* DLL Delay Chain Test Low Bits */
#define EMMC_PHY_DLL_CONTROL_DLL_DELAY_TEST_LOWER_MSK           SHIFT8(0xff)        
#define EMMC_PHY_DLL_CONTROL_DLL_DELAY_TEST_LOWER_BASE          8
/* DLL Delay Chain Test Upper Bits */
#define EMMC_PHY_DLL_CONTROL_DLL_DELAY_TEST_UP_MSK              SHIFT6(0x3)         
#define EMMC_PHY_DLL_CONTROL_DLL_DELAY_TEST_UP_BASE             6
/* DLL Fast Lock Mode */
#define EMMC_PHY_DLL_CONTROL_DLL_FAST_LOCK                      BIT_5               
/* DLL MC Idle Status */
#define EMMC_PHY_DLL_CONTROL_DLL_MC_IDLE                        BIT_4               
/* DLL Gain 2x */
#define EMMC_PHY_DLL_CONTROL_DLL_GAIN2X                         BIT_3               
/* DLL Auto Update Enable */
#define EMMC_PHY_DLL_CONTROL_DLL_AUTO_UPDATE_EN                 BIT_2               
/* DLL Test Mode */
#define EMMC_PHY_DLL_CONTROL_DLL_TEST_EN                        BIT_1               
/* DLL Bypass Enable */
#define EMMC_PHY_DLL_CONTROL_DLL_BYPASS_EN                      BIT_0               

/*  EMMC_LOGIC_TIMING_ADJUST            0x0174  xml_file/SDHOST_customer eMMC
 *                                              Logic Timing Adjust
 */
/* PHY Read Dealy Param */
#define EMMC_LOGIC_TIMING_ADJUST_PHY_RD_CLK_PARAM_MSK               SHIFT12(0xf)        
#define EMMC_LOGIC_TIMING_ADJUST_PHY_RD_CLK_PARAM_BASE              12
/* PHY Stop CLK Param */
#define EMMC_LOGIC_TIMING_ADJUST_PHY_STOP_CLK_PARAM_MSK             SHIFT8(0xf)         
#define EMMC_LOGIC_TIMING_ADJUST_PHY_STOP_CLK_PARAM_BASE            8
/* PHY Write Delay Param */
#define EMMC_LOGIC_TIMING_ADJUST_PHY_WR_DLY_PARAM_MSK               SHIFT4(0xf)         
#define EMMC_LOGIC_TIMING_ADJUST_PHY_WR_DLY_PARAM_BASE              4
/* PHY Resp Delay Param */
#define EMMC_LOGIC_TIMING_ADJUST_PHY_RESP_DLY_PARAM_MSK             SHIFT0(0xf)         
#define EMMC_LOGIC_TIMING_ADJUST_PHY_RESP_DLY_PARAM_BASE            0

/*  PHY_LB_FUNC_CONFIG                  0x0178  xml_file/SDHOST_customer eMMC
 *                                              PHY Loopback Func Config
 */
#define PHY_LB_FUNC_CONFIG_LB_MODE_EN                   BIT_31              /* Loopback Mode Enable */
#define PHY_LB_FUNC_CONFIG_LB_PATTERN_SEL               BIT_30              /* Loopback Pattern Select */
#define PHY_LB_FUNC_CONFIG_NEED_QSP                     BIT_29              /* Need QSP */
#define PHY_LB_FUNC_CONFIG_NEED_QSN                     BIT_28              /* Need QSN */
#define PHY_LB_FUNC_CONFIG_INV_QSP                      BIT_27              /* Inverted QSP */
#define PHY_LB_FUNC_CONFIG_INV_QSN                      BIT_26              /* Inverted QSN */
#define PHY_LB_FUNC_CONFIG_CMD_LB_MASK                  BIT_24              /* CMD Loopback Test Mask */
#define PHY_LB_FUNC_CONFIG_DQ_LB_MASK_MSK               SHIFT16(0xff)       /* DQ Loopback Test Mask */
#define PHY_LB_FUNC_CONFIG_DQ_LB_MASK_BASE              16
#define PHY_LB_FUNC_CONFIG_FILTER_BITS_CNT_MSK          SHIFT12(0xf)        /* Filter Bits Cnt */
#define PHY_LB_FUNC_CONFIG_FILTER_BITS_CNT_BASE         12
#define PHY_LB_FUNC_CONFIG_QSP_AFTER_RDRST_MSK          SHIFT8(0xf)         /* QSP after RDRST */
#define PHY_LB_FUNC_CONFIG_QSP_AFTER_RDRST_BASE         8
#define PHY_LB_FUNC_CONFIG_RDQ_SEL_DLY_MSK              SHIFT4(0xf)         /* RDQ_SEL_DLY */
#define PHY_LB_FUNC_CONFIG_RDQ_SEL_DLY_BASE             4
#define PHY_LB_FUNC_CONFIG_RDQ_OUT_DLY_MSK              SHIFT0(0xf)         /* RDQ_OUT_DLY */
#define PHY_LB_FUNC_CONFIG_RDQ_OUT_DLY_BASE             0

/*  PHY_LB_DATA_PATTERN                 0x017C  xml_file/SDHOST_customer eMMC
 *                                              PHY Loopback Data Pattern
 */
/* reset data pattern */
#define PHY_LB_DATA_PATTERN_RESET_PATTERN                   BIT_31                  
/* write data pattern */
#define PHY_LB_DATA_PATTERN_WRITE_PATTERN                   BIT_30                  
/* Lookback data pattern */
#define PHY_LB_DATA_PATTERN_LB_DATA_PATTERN_MSK             SHIFT0(0x3ffff)         
#define PHY_LB_DATA_PATTERN_LB_DATA_PATTERN_BASE            0

/*  PHY_LB_FUNC_CONTROL                 0x0180  xml_file/SDHOST_customer eMMC
 *                                              PHY Loopback Func Control
 */
/* Clear Loopback Err Status */
#define PHY_LB_FUNC_CONTROL_CLEAR_LB_ERR_STATUS                 BIT_31                  
/* Start Stuck-At 0 Detection for QSN */
#define PHY_LB_FUNC_CONTROL_START_STUCK0_DET_QSN                BIT_4                   
/* Start Stuck-At 1 Detection for QSN */
#define PHY_LB_FUNC_CONTROL_START_STUCK1_DET_QSN                BIT_3                   
/* Start Stuck-At 0 Detection for QSP */
#define PHY_LB_FUNC_CONTROL_START_STUCK0_DET_QSP                BIT_2                   
/* Start Stuck-At 1 Detection for QSP */
#define PHY_LB_FUNC_CONTROL_START_STUCK1_DET_QSP                BIT_1                   
/* Start Loopback Test */
#define PHY_LB_FUNC_CONTROL_START_LP_TEST                       BIT_0                   

/*  PHY_LB_ERR_STATUS                   0x0184  xml_file/SDHOST_customer eMMC
 *                                              PHY Loopback Err Status
 */
#define PHY_LB_ERR_STATUS_STUCK_0_DET_ERR_QSN               BIT_31              /* Stuck-At 0 Detection Err for QSN */
#define PHY_LB_ERR_STATUS_STUCK_1_DET_ERR_QSN               BIT_30              /* Stuck-At 1 Detection Err for QSN */
#define PHY_LB_ERR_STATUS_STUCK_0_DET_ERR_QSP               BIT_29              /* Stuck-At 0 Detection Err for QSP */
#define PHY_LB_ERR_STATUS_STUCK_1_DET_ERR_QSP               BIT_28              /* Stuck-At 1 Detection Err for QSP */
#define PHY_LB_ERR_STATUS_DM_EVEN_PIN_ERR                   BIT_24              /* DM Even Pin Err */
#define PHY_LB_ERR_STATUS_DQ_EVEN_PIN_ERR_MSK               SHIFT16(0xff)       /* DQ Even Pins Err */
#define PHY_LB_ERR_STATUS_DQ_EVEN_PIN_ERR_BASE              16
#define PHY_LB_ERR_STATUS_DM_ODD_PIN_ERR                    BIT_8               /* DM Odd Pin Err */
#define PHY_LB_ERR_STATUS_DQ_ODD_PIN_ERR_MSK                SHIFT0(0xff)        /* DQ Odd Pins Err */
#define PHY_LB_ERR_STATUS_DQ_ODD_PIN_ERR_BASE               0

/*  PHY_LB_COMP_CNT                     0x0188  xml_file/SDHOST_customer eMMC
 *                                              PHY Loopback Comparison Count
 */
#define PHY_LB_COMP_CNT_COMP_CNT_MSK            SHIFT0(0xffffffff)      /* Comparison Count */
#define PHY_LB_COMP_CNT_COMP_CNT_BASE           0		


#endif
