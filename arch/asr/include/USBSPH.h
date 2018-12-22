#ifndef __USBSPH_H__
#define __USBSPH_H__

typedef struct
{

    __I  uint32_t U2XID                 ; /* 0x000 Identification Register */                       
    __I  uint32_t U2XHWGENERAL          ; /* 0x004 General Hardware Parameters Register */          
    __I  uint32_t U2XHWHOST             ; /* 0x008 Host Hardware Parameters Register */             
    __I  uint32_t U2XHWDEVICE           ; /* 0x00C Device Hardware Parameters Register */           
    __I  uint32_t U2XHWTXBUF            ; /* 0x010 TX Buffer Hardware Parameters Register */        
    __I  uint32_t U2XHWRXBUF            ; /* 0x014 RX Buffer Hardware Parameters Register */   
    __IO uint32_t RESERVED0[(0x100-0x014)/4-1];
    __I  uint16_t U2XCAPLENGTH          ; /* 0x100 Capability Length Register */                    
    __I  uint16_t U2XHCIVERSION         ; /* 0x102 Host Interface Version Number (EHCI) Register */ 
    __I  uint32_t U2XHCSPARAMS          ; /* 0x104 Host Controller Structural Parameters (EHCI) Register */         
    __I  uint32_t U2XHCCPARAMS          ; /* 0x108 Host Controller Capability Parameters (EHCI) Register */  
    
    __IO uint32_t RESERVED2[5];
           
    __I  uint32_t U2XDCIVERSION         ; /* 0x120 Device Interface Version Number Register */                      
    __I  uint32_t U2XDCCPARAMS          ; /* 0x124 Device Controller Capability Parameters Register */   
    __IO uint32_t RESERVED3[6];
               
    __IO uint32_t U2XUSBCMD             ; /* 0x140 USB Command Register */                       
    __IO uint32_t U2XUSBSTS             ; /* 0x144 USB Status Register */                        
    __IO uint32_t U2XUSBINTR            ; /* 0x148 USB Interrupt Enable Register */              
    __IO uint32_t U2XFRINDEX            ; /* 0x14C USB Frame Index Register */       
    
    __IO uint32_t RESERVED4;
    
    union
    {            
        __IO uint32_t U2XPERLISTBASE        ; /* 0x154 Periodic List Base Register - HOST */         
        __IO uint32_t U2XDEVICEADDR         ; /* 0x154 USB Device Address Register - DEVICE */                      
    };
    union
    {
        __IO uint32_t U2XASYNCLISTADDR      ; /* 0x158 Next Asynchronous List Address Register - HOST */            
        __IO uint32_t U2XEPLISTADDR         ; /* 0x158 Address at Endpoint List in Memory Register -DEVICE */       
    };
    __IO uint32_t U2XTTCTRL             ; /* 0x15C TT Status and Control Register - HOST */                   
    __IO uint32_t U2XBURSTSIZE          ; /* 0x160 Programmable Burst Size Register */                        
    __IO uint32_t U2XTXFILLTUNING       ; /* 0x164 Host Transmit Pre-Buffer Packet Tuning Register */     
    
    __IO uint32_t RESERVED5;
        
    __IO uint32_t U2XICUSB              ; /* 0x16C Host FS Inter-Chip Register */   
    
    __IO uint32_t RESERVED6[2];
                  
    __IO uint32_t U2XEPNAK              ; /* 0x178 Endpoint NAK Register */                       
    __IO uint32_t U2XEPNAKEN            ; /* 0x17C Endpoint NAK Enable Register */                
    __I  uint32_t U2XCONFIGFLAG         ; /* 0x180 Configured Flag Register */                    
    __IO uint32_t U2XPORTSC             ; /* 0x184 Port Status/Control Register */   
    
    __IO uint32_t RESERVED7[7];
                 
    __IO uint32_t U2XOTGSC              ; /* 0x1A4 On-The-Go (OTG) Status and Control Register */ 
    __IO uint32_t U2XUSBMODE            ; /* 0x1A8 USB Device Mode Register */ 



}USBSPH_TypeDef;



#define    USBSPH1_BASE       0xF0001000
#define    USBSPH1  (( USBSPH_TypeDef *) USBSPH1_BASE )

#define    USBSPH2_BASE       0xF0002000
#define    USBSPH2  (( USBSPH_TypeDef *) USBSPH2_BASE )

#define    USBFSIC_BASE       0xF0003000
#define    USBFSIC  (( USBSPH_TypeDef *) USBFSIC_BASE )



/*
 *
 *	THE BIT DEFINES
 *
 */
/*	USB_ID							0x0000	Identification Register */
/*		Bit(s) USB_ID_RSRV_31_24 reserved */
#define	USB_ID_REVISION_MSK			SHIFT16(0xff)	/* Revision Number of the Core */
#define	USB_ID_REVISION_BASE		16
/*		Bit(s) USB_ID_RSRV_15_14 reserved */
#define	USB_ID_Always1_MSK			SHIFT14(0x3)	
#define	USB_ID_Always1_BASE		14
/* Ones Complement Version of ID[5:0] */
#define	USB_ID_NID5_0_MSK			SHIFT8(0x3f)	
#define	USB_ID_NID5_0_BASE			8
/*		Bit(s) USB_ID_RSRV_7_6 reserved */
#define	USB_ID_ID_MSK				SHIFT0(0x3f)	/* Configuration Number */
#define	USB_ID_ID_BASE				0


/*	USB_HWGENERAL					0x0004	General Hardware Parameters
 *											Register
 */
/*		Bit(s) USB_HWGENERAL_RSRV_31_13 reserved */
#define	USB_HWGENERAL_SM_MSK				SHIFT11(0x3)		/* SM */
#define	USB_HWGENERAL_SM_BASE				11
#define	USB_HWGENERAL_PHYM_MSK				SHIFT6(0xf)			/* PHYM */
#define	USB_HWGENERAL_PHYM_BASE				6
#define	USB_HWGENERAL_PHYW_MSK				SHIFT4(0x3)			/* PHYW */
#define	USB_HWGENERAL_PHYW_BASE				4
#define	USB_HWGENERAL_BWT					BIT_3				/* BWT */
#define	USB_HWGENERAL_CLKC_MSK				SHIFT1(0x3)			/* CLKC */
#define	USB_HWGENERAL_CLKC_BASE				1
#define	USB_HWGENERAL_RT					BIT_0				/* RT */


/*	USB_HWHOST						0x0008	Host Hardware Parameters Register */
#define	USB_HWHOST_TTPER_MSK			SHIFT24(0xff)	/* TTPER */
#define	USB_HWHOST_TTPER_BASE			24
#define	USB_HWHOST_TTASY_MSK			SHIFT16(0xff)	/* TTASY */
#define	USB_HWHOST_TTASY_BASE			16
/*		Bit(s) USB_HWHOST_RSRV_15_4 reserved */
#define	USB_HWHOST_NPORT_MSK			SHIFT1(0x7)		/* NPORT */
#define	USB_HWHOST_NPORT_BASE			1
#define	USB_HWHOST_HC					BIT_0			/* HC */


/*	USB_HWDEVICE					0x000C	Device Hardware Parameters Register */
/*		Bit(s) USB_HWDEVICE_RSRV_31_6 reserved */
#define	USB_HWDEVICE_DEVEP_MSK			SHIFT1(0x1f)		/* DEVEP */
#define	USB_HWDEVICE_DEVEP_BASE			1
#define	USB_HWDEVICE_DC					BIT_0				/* Device Capable */

/*	USB_HWTXBUF						0x0010	TX Buffer Hardware Parameters
 *											Register
 */
#define	USB_HWTXBUF_TXLCR				BIT_31			/* TXLCR */
/*		Bit(s) USB_HWTXBUF_RSRV_30_24 reserved */
#define	USB_HWTXBUF_TXCHANADD_MSK		SHIFT16(0xff)	/* TXCHANADD */
#define	USB_HWTXBUF_TXCHANADD_BASE		16
#define	USB_HWTXBUF_TXADD_MSK			SHIFT8(0xff)	/* TXADD */
#define	USB_HWTXBUF_TXADD_BASE			8
#define	USB_HWTXBUF_TCBURST_MSK			SHIFT0(0xff)	/* TCBURST */
#define	USB_HWTXBUF_TCBURST_BASE		0

/*	USB_HWRXBUF						0x0014	RX Buffer Hardware Parameters
 *											Register
 */
/*		Bit(s) USB_HWRXBUF_RSRV_31_16 reserved */
#define	USB_HWRXBUF_RXADD_MSK			SHIFT8(0xff)	/* RXADD */
#define	USB_HWRXBUF_RXADD_BASE			8
#define	USB_HWRXBUF_RXBURST_MSK			SHIFT0(0xff)	/* RXBURST */
#define	USB_HWRXBUF_RXBURST_BASE		0


/*	USB_CAPLENGTH					0x0100	Capability Register Length - EHCI
 *											Compliant Register
 */
/*		Bit(s) USB_CAPLENGTH_RSRV_15_8 reserved */
#define	USB_CAPLENGTH_CAPLENGTH_MSK			SHIFT0(0xff)	/* CAPLENGTH */
#define	USB_CAPLENGTH_CAPLENGTH_BASE		0

/*	USB_HCIVERSION					0x0102	Host Interface Version Number -
 *											EHCI Compliant Register
 */
#define	USB_HCIVERSION_HCIVERSION_MSK		SHIFT0(0xffff)	/* HCIVERSION */
#define	USB_HCIVERSION_HCIVERSION_BASE		0






/*	USB_HCSPARAMS					0x0104	Host Ctrl. Structural Parameters
 *											EHCI Compliant with
 */
/*		Bit(s) USB_HCSPARAMS_RSRV_31_28 reserved */
/* Number of Transaction Translators (N_TT) */
#define	USB_HCSPARAMS_N_TT_MSK				SHIFT24(0xf)	
#define	USB_HCSPARAMS_N_TT_BASE				24
/* Number of Ports per Transaction Translator (N_PTT) */
#define	USB_HCSPARAMS_N_PTT_MSK				SHIFT20(0xf)	
#define	USB_HCSPARAMS_N_PTT_BASE			20
/*		Bit(s) USB_HCSPARAMS_RSRV_19_17 reserved */
/* Port Indicators (P INDICATOR) */
#define	USB_HCSPARAMS_PI					BIT_16			
/* Number of Companion Controller (N_CC) */
#define	USB_HCSPARAMS_N_CC_MSK				SHIFT12(0xf)	
#define	USB_HCSPARAMS_N_CC_BASE				12
/* Number of Ports per Companion Controller */
#define	USB_HCSPARAMS_N_PCC_MSK				SHIFT8(0xf)		
#define	USB_HCSPARAMS_N_PCC_BASE			8
/*		Bit(s) USB_HCSPARAMS_RSRV_7_5 reserved */
#define	USB_HCSPARAMS_PPC					BIT_4			/* Port Power Control */
/* Number of Downstream Ports */
#define	USB_HCSPARAMS_N_PORTS_MSK			SHIFT0(0xf)		
#define	USB_HCSPARAMS_N_PORTS_BASE			0

/*	USB_HCCPARAMS					0x0108	Host Ctrl. Capability Parameters
 *											EHCI Compliant
 */
/*		Bit(s) USB_HCCPARAMS_RSRV_31_16 reserved */
/* EHCI Extended Capabilities Pointer */
#define	USB_HCCPARAMS_EECP_MSK				SHIFT8(0xff)	
#define	USB_HCCPARAMS_EECP_BASE				8
/* Isochronous Scheduling Threshold */
#define	USB_HCCPARAMS_IST_MSK				SHIFT4(0xf)		
#define	USB_HCCPARAMS_IST_BASE				4
/*		Bit(s) USB_HCCPARAMS_RSRV_3 reserved */
/* Asynchronous Schedule Park Capability */
#define	USB_HCCPARAMS_ASP					BIT_2			
/* Programmable Frame List Flag */
#define	USB_HCCPARAMS_PFL					BIT_1			
/* 64-bit Addressing Capability */
#define	USB_HCCPARAMS_ADC					BIT_0			

/*	USB_DCIVERSION					0x0120	Dev. Interface Version Number
 *											Register
 */
/*		Bit(s) USB_DCIVERSION_RSRV_31_16 reserved */
#define	USB_DCIVERSION_DCIVERSION_MSK		SHIFT0(0xffff)	/* DCIVERSION */
#define	USB_DCIVERSION_DCIVERSION_BASE		0

/*	USB_DCCPARAMS					0x0124	Device Ctrl. Capability Parameters
 *											Register
 */
/*		Bit(s) USB_DCCPARAMS_RSRV_31_9 reserved */
#define	USB_DCCPARAMS_HC					BIT_7	//NEVO C0			/* Host Capable */
#define	USB_DCCPARAMS_DC					BIT_8	//NEVO C0			/* Device Capable */
/*		Bit(s) USB_DCCPARAMS_RSRV_6_5 reserved */
/* Device Endpoint Number */
#define	USB_DCCPARAMS_DEN_MSK				SHIFT0(0x1f)		
#define	USB_DCCPARAMS_DEN_BASE				0




/*	USB_CMD							0x0140	USB Command Register */
/*		Bit(s) USB_CMD_RSRV_31_24 reserved */
#define	USB_CMD_ITC_MSK				SHIFT16(0xff)	/* Interrupt Threshold Control */
#define	USB_CMD_ITC_BASE			16
#define	USB_CMD_FS					BIT_15			/* Frame List Size */
#define	USB_CMD_ATDTW				BIT_12   //NEVO C0			/* Add dTD TripWire */
#define	USB_CMD_SUTW				BIT_13			/* Setup TripWire */
/*		Bit(s) USB_CMD_RSRV_12 reserved */
/* Asynchronous Schedule Park Mode Enable */
#define	USB_CMD_ASPE				BIT_11			
/*		Bit(s) USB_CMD_RSRV_10 reserved */
/* Asynchronous Schedule Park Mode Count */
#define	USB_CMD_ASP_MSK				SHIFT8(0x3)		
#define	USB_CMD_ASP_BASE			8
/*		Bit(s) USB_CMD_RSRV_7 reserved */
/* Interrupt on Async Advance Doorbell */
#define	USB_CMD_LR                                      BIT_7
#define	USB_CMD_IAA					BIT_6			
#define	USB_CMD_ASE					BIT_5			/* Asynchronous Schedule Enable */
#define	USB_CMD_PSE					BIT_4			/* Periodic Schedule Enable */
#define	USB_CMD_FS_MSK				SHIFT2(0x3)		/* Frame List Size */
#define	USB_CMD_FS_BASE				2
#define	USB_CMD_RST					BIT_1			/* Controller Reset (RESET) */
#define	USB_CMD_RS					BIT_0			/* Run/Stop */

/*	USB_STS							0x0144	USB Status Register */
/*		Bit(s) USB_STS_RSRV_31_17 reserved */
#define	USB_STS_TI1				BIT_25  //NEVO C0
#define	USB_STS_TI0				BIT_24  //NEVO C0
#define	USB_STS_UPI				BIT_19  //NEVO C0
#define	USB_STS_UAI				BIT_18  //NEVO C0

#define	USB_STS_NAKI				BIT_16			/* NAK Interrupt Bit */
#define	USB_STS_AS					BIT_15			/* Asynchronous Schedule Status */
#define	USB_STS_PS					BIT_14			/* Periodic Schedule Status */
#define	USB_STS_RCL					BIT_13			/* Reclamation */
#define	USB_STS_HCH					BIT_12			/* HCHaIted */
/*		Bit(s) USB_STS_RSRV_11 reserved */
#define	USB_STS_ULPII				BIT_10			/* ULPI Interrupt */
/*		Bit(s) USB_STS_RSRV_9 reserved */
#define	USB_STS_SLI					BIT_8			/* DCSuspend */
#define	USB_STS_SRI					BIT_7			/* SOF Received */
#define	USB_STS_URI					BIT_6			/* USB Reset Received */
#define	USB_STS_AAI_0B_08H			BIT_5			/* Interrupt on Async Advance */
/*		Bit(s) USB_STS_RSRV_4 reserved */
#define	USB_STS_SEI				BIT_4  //NEVO C0
#define	USB_STS_FRI					BIT_3			/* Frame List Rollover */
#define	USB_STS_PCI					BIT_2			/* Port Change Detect */
/* USB Error Interrupt (USBERRINT) */
#define	USB_STS_UEI					BIT_1			
#define	USB_STS_UI					BIT_0			/* USB Interrupt (USBINT) */

/*	USB_INTR						0x0148	USB Interrupt Enable Register */
/*		Bit(s) USB_INTR_RSRV_31_17 reserved */
#define	USB_INTR_TIE1				BIT_25  //NEVO C0
#define	USB_INTR_TIE0				BIT_24  //NEVO C0
#define	USB_INTR_UPIE				BIT_19  //NEVO C0
#define	USB_INTR_UAIE				BIT_18  //NEVO C0


#define	USB_INTR_NAKE					BIT_16			/* NAK Interrupt Enable */
/*		Bit(s) USB_INTR_RSRV_15_9 reserved */
#define	USB_INTR_SLE					BIT_8			/* Sleep Enable */
#define	USB_INTR_SRE					BIT_7			/* SOF Received Enable */
#define	USB_INTR_URE					BIT_6			/* USB Reset Enable */
/* Interrupt on Async Advance Enable */
#define	USB_INTR_AAE					BIT_5			
#define	USB_INTR_SEE					BIT_4			/* System Error Enable */
/* Frame List Rollover Enable */
#define	USB_INTR_FRE					BIT_3			
/* Port Change Detect Enable */
#define	USB_INTR_PCE					BIT_2			
/* USB Error Interrupt Enable */
#define	USB_INTR_UEE					BIT_1			
#define	USB_INTR_UE						BIT_0			/* USB Interrupt Enable */

/*	USB_FRINDEX						0x014C	USB Frame Index Register */
/*		Bit(s) USB_FRINDEX_RSRV_31_14 reserved */
#define	USB_FRINDEX_FRINDEX_MSK			SHIFT0(0x3fff)		/* Frame Index */
#define	USB_FRINDEX_FRINDEX_BASE		0

/*	USB_PERIODICLISTBASE			0x0154	Host Controller Frame List Base
 *											Address Register (Host Mode)
 */
/* Base Address (Low) */
#define	USB_PERIODICLISTBASE_BASEADR_MSK		SHIFT12(0xfffff)	
#define	USB_PERIODICLISTBASE_BASEADR_BASE		12
/*		Bit(s) USB_PERIODICLISTBASE_RSRV_11_0 reserved */

/*	USB_DEVICEADDR					0x0154	Device Controller USB Device
 *											Address Register (Device Mode)
 */
#define	USB_DEVICEADDR_USBADR_MSK			SHIFT25(0x7f)		/* Device Address */
#define	USB_DEVICEADDR_USBADR_BASE			25
/* Device Address Advance */
#define	USB_DEVICEADDR_USBADRA				BIT_24				
/*		Bit(s) USB_DEVICEADDR_RSRV_23_0 reserved */

/*	USB_ASYNCLISTADDR				0x0158	Host Controller Next Asynchronous
 *											List Address Register
 */
/* Link Pointer Low (LPL) */
#define	USB_ASYNCLISTADDR_ASYBASE_MSK		SHIFT5(0x7ffffff)	
#define	USB_ASYNCLISTADDR_ASYBASE_BASE		5
/*		Bit(s) USB_ASYNCLISTADDR_RSRV_4_0 reserved */

/*	USB_ENDPOINTLISTADDR			0x0158	Address at Endpoint List in Memory
 *											Register
 */
/* Endpoint List Pointer (Low) */
#define	USB_ENDPOINTLISTADDR_EPBASE_MSK			SHIFT11(0x1fffff)	
#define	USB_ENDPOINTLISTADDR_EPBASE_BASE		11
/*		Bit(s) USB_ENDPOINTLISTADDR_RSRV_10_0 reserved */

//TT Status and Control Register - HOST (U2XTTCTRL)  NEVO C0
#define	USB_U2XTTCTRL_TTHA_MSK      SHIFT24(0x7f)
#define	USB_U2XTTCTRL_TTHA_BASE    24


/*	USB_BURSTSIZE					0x0160	Programmable Burst Size Register -
 *											Host Controller
 */
/*		Bit(s) USB_BURSTSIZE_RSRV_31_17 reserved */
/* Programmable TX Burst Length */
#define	USB_BURSTSIZE_TXPBURST_MSK			SHIFT8(0x1ff)	
#define	USB_BURSTSIZE_TXPBURST_BASE			8
/* Programmable RX Burst Length */
#define	USB_BURSTSIZE_RXPBURST_MSK			SHIFT0(0xff)	
#define	USB_BURSTSIZE_RXPBURST_BASE			0

//Host Transmit Pre-Buffer Packet Tuning Register (U2XTXFILLTUNING) NEVO C0
#define	USB_U2XTXFILLTUNING_TXFIFOTHRES_MSK                SHIFT16(0x3f)
#define	USB_U2XTXFILLTUNING_TXFIFOTHRES_BASE              16
#define	USB_U2XTXFILLTUNING_TXSCHEALTH_MSK                SHIFT8(0x1f)
#define	USB_U2XTXFILLTUNING_TXSCHEALTH_BASE              8
#define	USB_U2XTXFILLTUNING_TXSCHOH_MSK                SHIFT0(0xff)
#define	USB_U2XTXFILLTUNING_TXSCHOH_BASE              0

//Host FS Inter-Chip Register (U2XICUSB)  NEVO C0
#define	USB_U2XICUSB_IC_EN   BIT_3
#define	USB_U2XICUSB_IC_VDD_MSK  SHIFT0(0x7)
#define	USB_U2XICUSB_IC_VDD_BASE  0

/*	USB_ENDPTNAK					0x0178	Endpoint NAK Register */
#define	USB_ENDPTNAK_EPTN_MSK		SHIFT16(0xffff)	/* TX Endpoint NAK */
#define	USB_ENDPTNAK_EPTN_BASE		16
#define	USB_ENDPTNAK_EPRN_MSK		SHIFT0(0xffff)	/* RX Endpoint NAK */
#define	USB_ENDPTNAK_EPRN_BASE		0

/*	USB_ENDPTNAKEN					0x017C	Endpoint NAK Enable Register */
#define	USB_ENDPTNAKEN_EPTNE_MSK		SHIFT16(0xffff)	/* TX Endpoint NAK Enable */
#define	USB_ENDPTNAKEN_EPTNE_BASE		16
#define	USB_ENDPTNAKEN_EPRNE_MSK		SHIFT0(0xffff)	/* RX Endpoint NAK Enable */
#define	USB_ENDPTNAKEN_EPRNE_BASE		0

/*	USB_PORTSC						0x0184	Port Status/Control Register */
#define	USB_PORTSC_PTS_MSK			SHIFT30(0x3)	/* Parallel Transceiver Select */
#define	USB_PORTSC_PTS_BASE			30
/*		Bit(s) USB_PORTSC_RSRV_29 reserved */
#define	USB_PORTSC_PTW				BIT_28			/* Parallel Transceiver Width */
#define	USB_PORTSC_PSPD_MSK			SHIFT26(0x3)	/* Port Speed */
#define	USB_PORTSC_PSPD_BASE		26
/*		Bit(s) USB_PORTSC_RSRV_25 reserved */
/* Port Force Full Speed Connect */
#define	USB_PORTSC_PFSC				BIT_24			
/* PHY Low Power Suspend - Clock Disable (PLPSCD) */
#define	USB_PORTSC_PHCD				BIT_23			
/* Wake on Disconnect Enable (WKDSCNNT_E) */
#define	USB_PORTSC_WKOC				BIT_22			
/* Wake on Disconnect Enable (WKDSCNNT_E) */
#define	USB_PORTSC_WKDC				BIT_21			
/* Wake on Connect Enable (WKCNNT_E) */
#define	USB_PORTSC_WKCN				BIT_20			
#define	USB_PORTSC_PTC_MSK			SHIFT16(0xf)	/* Port Test Control */
#define	USB_PORTSC_PTC_BASE			16
#define	USB_PORTSC_PIC_MSK			SHIFT14(0x3)	/* Port Indicator Control */
#define	USB_PORTSC_PIC_BASE			14
#define	USB_PORTSC_PO				BIT_13   //NEVO C0	
/*		Bit(s) USB_PORTSC_RSRV_13 reserved */
#define	USB_PORTSC_PP				BIT_12			/* Port Power */
#define	USB_PORTSC_LS_MSK			SHIFT10(0x3)	/* Line Status */
#define	USB_PORTSC_LS_BASE			10
#define	USB_PORTSC_HSP				BIT_9			/* High-Speed Port */
#define	USB_PORTSC_PR				BIT_8			/* Port Reset */
#define	USB_PORTSC_SUSP				BIT_7			/* Suspend */
#define	USB_PORTSC_FPR				BIT_6			/* Force Port Resume */
#define	USB_PORTSC_OCC				BIT_5			/* Over-current Change */
#define	USB_PORTSC_OCA				BIT_4			/* Over-current Active */
#define	USB_PORTSC_PEC				BIT_3			/* Port Enable/Disable Change */
#define	USB_PORTSC_PE				BIT_2			/* Port Enabled/Disabled */
#define	USB_PORTSC_CSC				BIT_1			/* Connect Status Change */
#define	USB_PORTSC_CCS				BIT_0			/* Current Connect Status */

/*	USB_OTGSC						0x01A4	OTG Status and Control Register */
/*		Bit(s) USB_OTGSC_RSRV_31 reserved */
#define	USB_OTGSC_DPIE			BIT_30	/* Data Pulse Interrupt Enable */
#define	USB_OTGSC_1MSE			BIT_29	/* 1 millisecond timer Interrupt Enable */
#define	USB_OTGSC_BSEIE			BIT_28	/* B Session End Interrupt Enable */
#define	USB_OTGSC_BSVIE			BIT_27	/* B Session Valid Interrupt Enable */
#define	USB_OTGSC_ASVIE			BIT_26	/* A Session Valid Interrupt Enable */
#define	USB_OTGSC_AVVIE			BIT_25	/* A VBus Valid Interrupt Enable */
#define	USB_OTGSC_IDIE			BIT_24	/* USB ID Interrupt Enable */
/*		Bit(s) USB_OTGSC_RSRV_23 reserved */
#define	USB_OTGSC_DPIS			BIT_22	/* Data Pulse Interrupt Status */
#define	USB_OTGSC_1MSS			BIT_21	/* 1 millisecond timer Interrupt Status */
#define	USB_OTGSC_BSEIS			BIT_20	/* B Session End Interrupt Status */
#define	USB_OTGSC_BSVIS			BIT_19	/* B Session Valid Interrupt Status */
#define	USB_OTGSC_ASVIS			BIT_18	/* A Session Valid Interrupt Status */
#define	USB_OTGSC_AVVIS			BIT_17	/* A VBus Valid Interrupt Status */
#define	USB_OTGSC_IDIS			BIT_16	/* USB ID Interrupt Status */
/*		Bit(s) USB_OTGSC_RSRV_15 reserved */
#define	USB_OTGSC_DPS			BIT_14	/* Data Bus Pulsing Status */
#define	USB_OTGSC_1MST			BIT_13	/* 1 millisecond timer toggle */
#define	USB_OTGSC_BSE			BIT_12	/* B Session End */
#define	USB_OTGSC_BSV			BIT_11	/* B Session Valid */
#define	USB_OTGSC_ASV			BIT_10	/* A Session Valid */
#define	USB_OTGSC_AVV			BIT_9	/* A VBus Valid */
#define	USB_OTGSC_ID			BIT_8	/* USB ID */
/* Hardware Assist B-Disconnect to A-Connect */
#define	USB_OTGSC_HABA			BIT_7	
#define	USB_OTGSC_HADP			BIT_6	/* Hardware Assist Data Pulse */
#define	USB_OTGSC_IDPU			BIT_5	/* ID Pullup */
#define	USB_OTGSC_DP			BIT_4	/* Data Pulsing */
#define	USB_OTGSC_OT			BIT_3	/* OTG Termination */
#define	USB_OTGSC_HAAR			BIT_2	/* Hardware Assist Auto-Reset */
#define	USB_OTGSC_VC			BIT_1	/* VBUS Charge */
#define	USB_OTGSC_VD			BIT_0	/* VBUS_Discharge */

/*	USB_MODE						0x01A8	USB Device Mode Register */
/*		Bit(s) USB_MODE_RSRV_31_5 reserved */
#define	USB_MODE_VBPS				BIT_5   //NEVO C0	
#define	USB_MODE_SDIS				BIT_4				/* Stream Disable Mode */
#define	USB_MODE_SLOM				BIT_3				/* Setup Lockout Mode */
#define	USB_MODE_ES					BIT_2				/* Endian Select */
#define	USB_MODE_CM_MSK				SHIFT0(0x3)			/* Controller Mode */
#define	USB_MODE_CM_BASE			0





#endif
