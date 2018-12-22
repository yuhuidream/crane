/******************************************************************************
**	(C) Copyright 2007 Marvell International Ltd. ?**	All Rights Reserved
******************************************************************************/

/******************************************************************************
**
**  COPYRIGHT (C) 2002, 2003 Intel Corporation.
**
**  This software as well as the software described in it is furnished under
**  license and may only be used or copied in accordance with the terms of the
**  license. The information in this file is furnished for informational use
**  only, is subject to change without notice, and should not be construed as
**  a commitment by Intel Corporation. Intel Corporation assumes no
**  responsibility or liability for any errors or inaccuracies that may appear
**  in this document or any software that may be provided in association with
**  this document.
**  Except as permitted by such license, no part of this document may be
**  reproduced, stored in a retrieval system, or transmitted in any form or by
**  any means without the express written consent of Intel Corporation.
**
**  FILENAME:	PlatformConfig.h
**
**  PURPOSE: 	Platform specific header to set parameters
**
**
******************************************************************************/

#ifndef __platform_config_h
#define __platform_config_h

/*#include "Typedef.h"
#include "Errors.h"
#include "ProtocolManager.h"
#include "misc.h"
#include "pmic.h"
#include "I2C.h"
#include "product_id.h"
#include "obm2osl.h"
#include "tim.h"*/
//
// Clock in nanoseconds. Not likely to be changing all that often...
//
#define DFC_WORKAROUND 0
#define NAND_CONTROLLER_CLOCK  156  // MHz
// Bit 19 of control register must be written as 0
#define DFC_INITIAL_CONTROL 0xCC021FFF
#define	DFC_INITIAL_TIMING0	0x003F3F3F
#define DFC_INITIAL_TIMING1 0x1FF0C0FF //tR set to 52.4 usec, enable wait_mode, prescale

#define TIMOffset_CS0						0x00018000 // offset for TIM on CS0

//     GPIO
#define GPLRx (volatile unsigned long *)( 0x40E00000 )  // GPIO pin-level register GPIO<31:0>
#define GPLRy (volatile unsigned long *)( 0x40E00004 )  // GPIO pin-level register GPIO<63:32>
#define GPLRz (volatile unsigned long *)( 0x40E00008 )  // GPIO pin-level register GPIO<95:64>
#define GPLRa (volatile unsigned long *)( 0x40E00100 )  // GPIO pin-level register GPIO<127:96>
#define GPLRb (volatile unsigned long *)( 0x40E00104 )  // GPIO pin-level register GPIO<159:128>
#define GPLRc (volatile unsigned long *)( 0x40E00108 )  // GPIO pin-level register GPIO<191:160>

#define GPDRx (volatile unsigned long *)( 0x40E0000C )  // GPIO pin direction register GPIO<31:0>
#define GPDRy (volatile unsigned long *)( 0x40E00010 )  // GPIO pin direction register GPIO<63:32>
#define GPDRz (volatile unsigned long *)( 0x40E00014 )  // GPIO pin direction register GPIO<80:64>
#define GPDRa (volatile unsigned long *)( 0x40E0010C )  // GPIO pin direction register GPIO<127:96>
#define GPDRb (volatile unsigned long *)( 0x40E00110 )  // GPIO pin direction register GPIO<159:128>
#define GPDRc (volatile unsigned long *)( 0x40E00114 )  // GPIO pin direction register GPIO<191:160>

#define GPSRx (volatile unsigned long *)( 0x40E00018 )  // GPIO pin output set register GPIO<31:0>
#define GPSRy (volatile unsigned long *)( 0x40E0001C )  // GPIO pin output set register GPIO<63:32>
#define GPSRz (volatile unsigned long *)( 0x40E00020 )  // GPIO pin output set register GPIO<80:64>
#define GPSRa (volatile unsigned long *)( 0x40E00118 )  // GPIO pin output set register GPIO<127:96>
#define GPSRb (volatile unsigned long *)( 0x40E0011C )  // GPIO pin output set register GPIO<159:128>
#define GPSRc (volatile unsigned long *)( 0x40E00120 )  // GPIO pin output set register GPIO<191:160>

#define GPCRx (volatile unsigned long *)( 0x40E00024 )  // GPIO pin output clear register GPIO<31:0>
#define GPCRy (volatile unsigned long *)( 0x40E00028 )  // GPIO pin output clear register GPIO <63:32>
#define GPCRz (volatile unsigned long *)( 0x40E0002C )  // GPIO pin output clear register GPIO <80:64>
#define GPCRa (volatile unsigned long *)( 0x40E00124 )  // GPIO pin output clear register GPIO <127:96>
#define GPCRb (volatile unsigned long *)( 0x40E00128 )  // GPIO pin output clear register GPIO <159:128>
#define GPCRc (volatile unsigned long *)( 0x40E0012C )  // GPIO pin output clear register GPIO <191:160>

#define GRERx (volatile unsigned long *)( 0x40E00030 )  // GPIO rising-edge detect register GPIO<31:0>
#define GRERy (volatile unsigned long *)( 0x40E00034 )  // GPIO rising-edge detect register GPIO<63:32>
#define GRERz (volatile unsigned long *)( 0x40E00038 )  // GPIO rising-edge detect register GPIO<80:64>
#define GRERa (volatile unsigned long *)( 0x40E00130 )  // GPIO rising-edge detect register GPIO<127:96>
#define GRERb (volatile unsigned long *)( 0x40E00134 )  // GPIO rising-edge detect register GPIO<159:128>
#define GRERc (volatile unsigned long *)( 0x40E00138 )  // GPIO rising-edge detect register GPIO<191:160>

#define GFERx (volatile unsigned long *)( 0x40E0003C )  // GPIO falling-edge detect register GPIO<31:0>
#define GFERy (volatile unsigned long *)( 0x40E00040 )  // GPIO falling-edge detect register GPIO<63:32>
#define GFERz (volatile unsigned long *)( 0x40E00044 )  // GPIO falling-edge detect register GPIO<80:64>
#define GFERa (volatile unsigned long *)( 0x40E0013C )  // GPIO falling-edge detect register GPIO<127:96>
#define GFERb (volatile unsigned long *)( 0x40E00140 )  // GPIO falling-edge detect register GPIO<159:128>
#define GFERc (volatile unsigned long *)( 0x40E00144 )  // GPIO falling-edge detect register GPIO<191:160>

#define GEDRx (volatile unsigned long *)( 0x40E00048 )  // GPIO edge detect status register GPIO<31:0>
#define GEDRy (volatile unsigned long *)( 0x40E0004C )  // GPIO edge detect status register GPIO<63:32>
#define GEDRz (volatile unsigned long *)( 0x40E00050 )  // GPIO edge detect status register GPIO<80:64>
#define GEDRa (volatile unsigned long *)( 0x40E00148 )  // GPIO edge detect status register GPIO<127:96>
#define GEDRb (volatile unsigned long *)( 0x40E0014C )  // GPIO edge detect status register GPIO<159:128>
#define GEDRc (volatile unsigned long *)( 0x40E00150 )  // GPIO edge detect status register GPIO<191:160>

#define ASCR	(volatile unsigned long *)(0x40F40000 ) // Application Subsystem Power Status/Configuration Register
#define ARSR	(volatile unsigned long *)(0x40F40004 ) // Application Subsystem Reset Status Register
#define AD3SR	(volatile unsigned long *)(0x40F4000C ) // Application Subsystem Wake-Up from D3 Status Register
#define AD3R	(volatile unsigned long *)(0x40F40030 ) // Application Subsystem D3 Configuration Register
#define UP2OCR (volatile unsigned long *)(0x40600020)
#define CHIPBOOTTYPE (volatile unsigned int *)(0x40F50200)
#define PLATFORM_CHIPBOOTTYPE_FUSE_REG 2
#define UDCCR_REG (volatile unsigned long *)(0x40600000) // USB1.1 client control register
#define D0CKEN_A (volatile unsigned long *)(0x4134000C )        // D0 mode Clock Enable Register A

// Use USB A Session fuse bit mask
#define FUSE_USE_USB_A_SESSION_MASK  (1 << 23)

#define ASCR_RDH_BASE	31
#define ASCR_RDH_MASK	(0x1<<31)
#define ASCR_RSVD_MASK	0x7FFF88F8 


// USB PLL's
#define USB2_PLL_PHYID		(volatile unsigned long *)		(0x42405E00)     // phy id register
#define USB2_PLL_CTRL_0		(volatile unsigned long *)		(0x42405E04)     // phy control register
#define USB2_PLL_CTRL_1		(volatile unsigned long *) 		(0x42405E08)	 // phy control register
#define USB2_PLL_CTRL_2		(volatile unsigned long *) 		(0x42405E0C)	 // phy control register
#define USB2_TX_CH_CTRL_0	(volatile unsigned long *) 		(0x42405E10)	 // 
#define USB2_TX_CH_CTRL_1	(volatile unsigned long *) 		(0x42405E14)	 // 
#define USB2_TX_CH_CTRL_2	(volatile unsigned long *) 		(0x42405E18)	 // 
#define USB2_TX_CH_CTRL_3	(volatile unsigned long *) 		(0x42405E1C)	 // 
#define USB2_RX_CH_CTRL_0	(volatile unsigned long *) 		(0x42405E20)	 // 
#define USB2_RX_CH_CTRL_1	(volatile unsigned long *) 		(0x42405E24)	 // 
#define USB2_RX_CH_CTRL_2	(volatile unsigned long *) 		(0x42405E28)	 // 
#define USB2_RX_CH_CTRL_3	(volatile unsigned long *) 		(0x42405E2C)	 // 
#define USB2_ANALOG_CTRL_0	(volatile unsigned long *) 		(0x42405E30)	 // 
#define USB2_ANALOG_CTRL_1	(volatile unsigned long *) 		(0x42405E34)	 // 
#define USB2_ANALOG_CTRL_2	(volatile unsigned long *) 		(0x42405E38)	 // 
#define USB2_DIGITAL_CTRL_0	(volatile unsigned long *) 		(0x42405E3C)	 // 
#define USB2_DIGITAL_CTRL_1	(volatile unsigned long *) 		(0x42405E40)	 // 
#define USB2_DIGITAL_CTRL_2	(volatile unsigned long *) 		(0x42405E44)	 // 
#define USB2_DIGITAL_CTRL_3	(volatile unsigned long *) 		(0x42405E48)	 // 
#define USB2_OTG_CTRL_0		(volatile unsigned long *) 		(0x42405E5C)	 // 
#define USB2_DIGITAL_CTRL_4	(volatile unsigned long *) 		(0x42405E64)	 // 


/*		USB2_ANALOG_CTRL	*/
#define USB2_ANA_PU_BASE				14
/*		USB2_OTG_CTRL_0		*/
#define USB2_OTG_PU_BASE				3
/*		UTMI_CTRL			*/
#define UTMI_CTRL_PWR_UP_BASE			0
#define UTMI_CTRL_PLL_PWR_UP_BASE		1
#define UTMI_CTRL_PU_REF_BASE			20


/* USB2_DIGITAL_CTRL_4 */	
#define USB2_PLL_VDD12_BASE			   12
#define USB2_PLL_VDD12_MASK			   (0x3<<12)
#define USB2_DPDM_SWAP_CTRL_BASE	   15 
#define USB2_DPDM_SWAP_CTRL_MASK	   (0x1<<15)


/* USB2_ANALOG_CTRL_1 */
#define USB2_R_RORATE_SEL_BASE		   7
#define USB2_R_RORATE_SEL_MASK		   (0x1<<7)
#define USB2_ANALOG_CTRL_1_RSVD_MASK   0xFFFF8000



// Pad Power Supply and Control Register Addresses.
// Using the PAD Name as the address name to be accessed.
// Defining only relevant ones to the boot ROM
#define MD0 		(volatile unsigned long *)( 0x40E10000 )  // MD0 Pad
#define MD1 		(volatile unsigned long *)( 0x40E10004 )  // MD1 Pad
#define nCS0 		(volatile unsigned long *)( 0x40E100C0 )  //
#define nCS1 		(volatile unsigned long *)( 0x40E100C4 )  //
#define EMPI_SCLK 	(volatile unsigned long *)( 0x40E100CC )  //

#define U2DOTGCR_	(volatile unsigned long *)( 0x54100020 )  //U2D OTG Control Register
#define U2DOTGICR_	(volatile unsigned long *)( 0x54100024 )  //U2D OTG Interrupt Control Register
#define U2DOTGUCR_	(volatile unsigned long *)( 0x54100030 )  //U2D OTG ULPI Control Register

// Tavor UART GPIO definition
#define GPIO47_TAVOR (volatile unsigned long *)( 0x40E102BC )  // FFUART_DSR
#define GPIO48_TAVOR (volatile unsigned long *)( 0x40E102B4 )  // FFUART_DTR
#define GPIO49_TAVOR (volatile unsigned long *)( 0x40E102B8 )  // FFUART_RI
#define GPIO50_TAVOR (volatile unsigned long *)( 0x40E102C8 )  // FFUART_DCD
#define GPIO51_TAVOR (volatile unsigned long *)( 0x40E102C0 )  // FFUART_CTS
#define GPIO52_TAVOR (volatile unsigned long *)( 0x40E102C4 )  // FFUART_RTS
#define GPIO53_TAVOR (volatile unsigned long *)( 0x40E102D0 )  // FFUART_RXD
#define GPIO54_TAVOR (volatile unsigned long *)( 0x40E102CC )  // FFUART_TXD
#define GPIO101_TAVOR (volatile unsigned long *)( 0x40E10408 )  // ALT UART TXD
#define GPIO100_TAVOR (volatile unsigned long *)( 0x40E10414 )  // ALT UART RXD
// Tavor USB PORT_2 GPIO definition
#define GPIO70_TAVOR (volatile unsigned long *)( 0x40E102A0 )  // USB_P2_4
#define GPIO71_TAVOR (volatile unsigned long *)( 0x40E10294 )  // USB_P2_8
#define GPIO72_TAVOR (volatile unsigned long *)( 0x40E10298 )  // USB_P2_5
#define GPIO73_TAVOR (volatile unsigned long *)( 0x40E1029C )  // USB_P2_2
#define GPIO74_TAVOR (volatile unsigned long *)( 0x40E102A8 )  // USB_P2_7
#define GPIO75_TAVOR (volatile unsigned long *)( 0x40E102A4 )  // USB_P2_1
#define GPIO76_TAVOR (volatile unsigned long *)( 0x40E102B0 )  // USB_P2_3
#define GPIO77_TAVOR (volatile unsigned long *)( 0x40E102AC )  // USB_P2_6

// Tavor ULPI GPIO's
#define GPIO30 		(volatile unsigned long *)( 0x40E10618 )  // ULPI Data 0
#define GPIO31 		(volatile unsigned long *)( 0x40E10610 )  // .
#define GPIO33 		(volatile unsigned long *)( 0x40E1061C )  // .
#define GPIO34 		(volatile unsigned long *)( 0x40E10620 )  // .
#define GPIO35 		(volatile unsigned long *)( 0x40E10628 )  // .
#define GPIO36 		(volatile unsigned long *)( 0x40E1062C )  // .
#define GPIO37 		(volatile unsigned long *)( 0x40E10630 )  // ULPI Data 6
#define GPIO38		(volatile unsigned long *)( 0x40E10634 )  // ULPI_CLK
#define GPIO39 		(volatile unsigned long *)( 0x40E10638 )  // ULPI STP
#define GPIO40 		(volatile unsigned long *)( 0x40E1063C )  // ULPI Data 7
#define GPIO41 		(volatile unsigned long *)( 0x40E10614 )  // ULPI DIR
#define GPIO42 		(volatile unsigned long *)( 0x40E10624 )  // ULPI NXT


#define GPDR1_32_63	0x40E00010
#define GPLR1_32_63	0x40E00004
#define GPSR1_32_63 0x40E0001C	
#define GPCR1_32_63 0x40E00028

#define GPIO152     (volatile unsigned long *)( 0x40E10624 )  // RDY

//
// Relevant and common GPIO memory mapped register needed for configuring flash
// signal list.
//
#define SMEMC_BASE	   0x4A000000

typedef struct 
{
  volatile int *    registerAddr;
  unsigned int      regValue;
  unsigned int      defaultValue;
}CS_REGISTER_PAIR_S, *P_CS_REGISTER_PAIR_S;

//Prototypes
UINT_T ChipSelect0(void);
UINT_T ChipSelect2(void);
void ChipSelectDFC(void);
void RestoreDefaultConfig(void);
void SaveDefaultConfig(void);
void PlatformInitDefaultClock(void);


//UART
UINT_T PlatformUARTConfig(void);


#if (USBCI)
//CI2 USB Functions
void PlatformCI2Init(void);
UINT_T PlatformCI2Transmit(UINT_T, UINT8_T*, UINT_T);
void PlatformCI2Shutdown(void);
void PlatformCI2InterruptHandler(void);

/* USBCI PHY structure - until Eshel chip (phy at top level) */
#define USB_PHY_BASE_ADDR            0x5550A000
typedef struct _usb_phy_registers
{
    unsigned int   U2PPLL;     	     //0x00 - USB2 PHY PLL Control
	unsigned int   U2PTX;     	     //0x04 - USB2 PHY TX Control
	unsigned int   U2PRX;     	     //0x08 - USB2 PHY RX Control
	unsigned int   U2PIVREF;         //0x0C - USB2 PHY IVREF Control
	unsigned int   PHY_T0;           //0x10 - phy control register
	unsigned int   U2PT1;            //0x14 - USB2 PHY Test 1 Control
	unsigned int   U2PT2;            //0x18 - USB2 PHY Test 2 Control
	unsigned int   U2PT3;            //0x1C - USB2 PHY Test 3 Control
	unsigned int   U2PT4;            //0x20 - USB2 PHY Test 4 Control
	unsigned int   U2PT5;            //0x24 - USB2 PHY Test 5 Control
	unsigned int   U2PID;            //0x28 - USB2 PHY ID Register
	unsigned int   U2PRS;            //0x2C - USB2 PHY Reserve Register
} usb_phy_registers;

/* USBCI PHY structure - from Eshel chip (phy as module) */
#define USB_PHY_MODULE_BASE_ADDR     0x42405E00
typedef struct _usb_phy_module
{
    unsigned int   PHYID;     	     //0x00 - PHY Identification Register
	unsigned int   PLL_CTRL_0;     	 //0x04 - PLL Control Register0
	unsigned int   PLL_CTRL_1;     	 //0x08 - PLL Control Register1
	unsigned int   PLL_CTRL_2;     	 //0x0C - PLL Control Register2
	unsigned int   TX_CH_CTRL_0;     //0x10 - TX Channel Control register0
	unsigned int   TX_CH_CTRL_1;     //0x14 - TX Channel Control register1
	unsigned int   TX_CH_CTRL_2;     //0x18 - TX Channel Control register2
	unsigned int   TX_CH_CTRL_3;     //0x1C - TX Channel Control register3
	unsigned int   RX_CH_CTRL_0;     //0x20 - RX Channel Control register0
	unsigned int   RX_CH_CTRL_1;     //0x24 - RX Channel Control register0
	unsigned int   RX_CH_CTRL_2;     //0x28 - RX Channel Control register0
	unsigned int   RX_CH_CTRL_3;     //0x2C - RX Channel Control register0
	unsigned int   ANALOG_CTRL_0;    //0x30 - Analog Top Control register0
	unsigned int   ANALOG_CTRL_1;    //0x34 - Analog Top Control register1
	unsigned int   ANALOG_CTRL_2;    //0x38 - Analog Top Control register2
	unsigned int   DIGITAL_CTRL_0;   //0x3C - Digital Control register0
	unsigned int   DIGITAL_CTRL_1;   //0x40 - Digital Control register1
	unsigned int   DIGITAL_CTRL_2;   //0x44 - Digital Control register2
	unsigned int   DIGITAL_CTRL_3;   //0x48 - Digital Control register3
	unsigned int   TEST_CTRL_0 ;     //0x4C - Test Control and status register0
	unsigned int   TEST_CTRL_1 ;     //0x50 - Test Control and status register1
	unsigned int   TEST_CTRL_2 ;     //0x54 - Test Control and status register2
	unsigned int   CHARGERDTCT_CTRL; //0x58 - Charger detector control register
	unsigned int   OTG_CTRL_0;       //0x5C - OTG control register0
	unsigned int   PHY_MON_STATUS;   //0x60 - Read PHY_MON[15:0]
	unsigned int   DIGITAL_CTRL_4;   //0x64 - Digital Control register4
	unsigned int   reserved1[3];     //0x68 - 0x74 - reserved
	unsigned int   INTERNALCID_0;    //0x78 - usb2_icid_reg0
	unsigned int   INTERNALCID_1;    //0x7C - usb2_icid_reg1
} usb_phy_module;

/* usb_phy_module bits */
#define USB2_PLL_FBDIV_SHIFT				0
#define USB2_PLL_REFDIV_SHIFT				8
#define USB2_PLL_REFDIV_SHIFT_NEW_PHY		9

#define USB2_PLL_VDD12_SHIFT				12
#define USB2_PLL_VDD18_SHIFT				14


#define USB2_PLL_FBDIV_MASK					0x00FF
#define USB2_PLL_REFDIV_MASK				0x0F00


#define USB2_PLL_FBDIV_MASK_NEW_PHY			0x01FF
#define USB2_PLL_REFDIV_MASK_NEW_PHY		0x3E00


#define USB2_PLL_PLLVDD12_MASK				(0x3 << 12)
#define USB2_PLL_PLLVDD18_MASK				(0x3 << 14)

#define USB2_PLL_CAL12_SHIFT				0
#define USB2_PLL_VCOCALL_START				2
#define USB2_PLL_CLK_BLK_EN_SHIFT			3
#define USB2_PLL_KVCO_SHIFT					4
#define USB2_PLL_KVCO_EXE_SHIFT				7
#define USB2_PLL_ICP_SHIFT					8
#define USB2_PLL_LOCKDET_ISEL_SHIFT			11
#define USB2_PLL_LOCK_BYPASS_SHIFT			12
#define USB2_PLL_PU_PLL_SHIFT				13
#define USB2_PLL_CONTROL_BY_PIN_SHIFT		14
#define USB2_PLL_READY_SHIFT				15
#define USB2_PLL_CALI12_MASK				(0x3)
#define USB2_PLL_KVCO_MASK					(0x7 << 4)
#define USB2_PLL_ICP_MASK					(0x7 << 8)
#define USB2_PLL_LOCK_BYPASS_MASK			(0x1 << 12)
#define USB2_PLL_PU_PLL_MASK				(0x1 << 13)
#define USB2_PLL_CONTROL_BY_PIN_MASK		(0x1 << 14)
#define USB2_PLL_READY_MASK					(0x1 << 15)

#define USB2_TX_EXT_FS_RCAL_SHIFT			0
#define USB2_TX_HS_RCAL_SHIFT				4
#define USB2_TX_IMPCAL_VTH_SHIFT			8
#define USB2_TX_EXT_FS_RCAL_EN				11
#define USB2_TX_EXT_HS_RCAL_EN_SHIFT		12
#define USB2_TX_RCAL_START_SHIFT			13
#define USB2_TX_DATA_BLOCK_EN				14
#define USB2_TX_EXT_FS_RCAL_MASK			(0xf)
#define USB2_TX_HS_RCAL_MASK				(0xf << 4)
#define USB2_TX_IMPCAL_VTH_MASK   			(0x7 << 8)
#define USB2_TX_EXT_FS_RCAL_EN_MASK			(0x1 << 11)
#define USB2_TX_EXT_HS_RCAL_EN_MASK			(0x1 << 12)
#define USB2_TX_RCAL_START_MASK				(0x1 << 13)
#define USB2_TX_DATA_BLOCK_EN_MASK			(0x1 << 14)

#define USB2_TX_CK60_PHSEL_SHIFT			0
#define USB2_TX_AMP_SHIFT					4
#define USB2_TX_LOW_VDD_EN_SHIFT			7
#define USB2_TX_VDD12_SHIFT					8
#define USB2_TX_VDD15_SHIFT					10
#define USB2_TX_CK60_PHSEL_MASK				(0xf)
#define USB2_TX_AMP_MASK					(0x7 << 4)
#define USB2_TX_LOWVDD_MASK					(0x1 << 7)
#define USB2_TX_VDD12_MASK     				(0x3 << 8)
#define USB2_TX_VDD15_MASK					(0x3 << 10)

#define USB2_RX_INTP_SHIFT					0
#define USB2_RX_LPF_COEF_SHIFT				2
#define USB2_RX_SQ_THRESH_SHIFT    			4
#define USB2_RX_DISCON_THRESH_SHITF			8
#define USB2_RX_SQ_LENGTH_SHIFT				10
#define USB2_RX_ACQ_LENGTH_SHIFT			12
#define USB2_RX_USQ_LENGTH_SHIFT			14
#define USB2_RX_PHASE_FREEZE_DLY_SHIFT		15
#define USB2_RX_INTP_MASK					(0x3)
#define USB2_RX_LPF_COEF_MAKS				(0x3 << 2)
#define USB2_RX_SQ_THRESH_MASK				(0xf << 4)
#define USB2_RX_DISCON_THREASH_MASK			(0x3 << 8)
#define USB2_RX_SQ_LENGTH_MASK				(0x3 << 10)
#define USB2_RX_ACQ_LENGTH_MASK				(0x3 << 12)
#define USB2_RX_USQ_LENGTH_MASK				(0x1 << 14)
#define USB2_RX_PHASE_FREEZE_DLY_MASK		(0x1 << 15)

#define USB2_RX_S2T03_DLY_SEL_SHITF			0
#define USB2_RX_CDR_FASTLOCK_EN_SHIFT		2
#define USB2_RX_CDR_COEF_SEL_SHIFT			3
#define USB2_RX_EDGE_DET_SEL_SHIFT			4
#define USB2_RX_DATA_BLOCK_LENGTH_SHIFT		6
#define USB2_RX_CAP_SEL_SHIFT				8
#define USB2_RX_EDGE_DET_EN_SHIFT			11
#define USB2_RX_DATA_BLOCK_EN_SHIFT			12
#define USB2_RX_EARLY_VOS_ON_EN_SHIFT		13
#define USB2_RX_S2T03_DLY_SEL_MASK			(0x3)
#define USB2_RX_CDR_FASTLOCK_EN_MASK		(0x1 << 2)
#define USB2_RX_CDR_COEF_SEL_MASK			(0x1 << 3)
#define USB2_RX_EDGE_DET_SEL_MASK			(0x3 << 4)
#define USB2_RX_DATA_BLOCK_LENGTH_MASK		(0x3 << 6)
#define USB2_RX_CAP_SEL_MASK				(0x7 << 8)
#define USB2_RX_EDGE_DET_EN_MASK			(0x1 << 11)
#define USB2_RX_DATA_BLOCK_EN_MASK			(0x1 << 12)
#define USB2_RX_EARLY_VOS_ON_EN_MASK		(0x1 << 13)

#define USB2_RX_VDD12_SHIFT					(0)
#define USB2_RX_VDD18_SHIFT					(2)
#define USB2_RX_SQ_ALWAYS_ON_SHIFT			(4)
#define USB2_RX_SQ_BUFFER_EN_SHIFT			(5)
#define USB2_RX_SAMPLER_CTRL_SHIFT			(6)
#define USB2_RX_SQ_CM_SHIFT					(7)
#define USB2_RX_USQ_FILTER_SHIFT			(8)
#define USB2_RX_VDD12_MASK					(0x3)
#define USB2_RX_VDD18_MASK					(0x3 << 2)
#define USB2_RX_SQ_ALWAYS_ON_MASK			(0x1 << 4)
#define USB2_RX_SQ_BUFFER_EN_MASK			(0x1 << 5)
#define USB2_RX_SAMPLER_CTRL_MASK			(0x1 << 6)
#define USB2_RX_SQ_CM_MASK					(0x1 << 7)
#define USB2_RX_USQ_FILTER_MASK				(0x1 << 8)

#define USB2_ANA_PU_ANA_SHIFT				14
#define USB2_R_ROTATE_SEL					7
#define USB2_R_ROTATE_SELMASK				(0x1 << 7)
#define USB2_OTG_PU_OTG_SHIFT				3

#define USB2_CTRL_PWR_UP_SHIFT				0
#define USB2_CTRL_PLL_PWR_UP_SHIFT			1
#define USB2_CTRL_PU_REF_SHIFT				20

#endif



// MMC
// for new eMMC on Nevo C0
#if SDHC2_SUPPORTED

// nevo c0 sdhc2
#define MM4_CTRL1_BASE_ADDR 0x55000000 /* controller 1 @*/
#define MM4_CTRL2_BASE_ADDR 0x55100000 /* controller 2 @*/
#define MM4_CTRL3_BASE_ADDR 0x55200000 /* controller 3 @*/
#define MM4_CTRL4_BASE_ADDR 0x55300000 /* controller 4 @*/

#endif

// i2c and PMIC
/************************************************************************************/
/************************************************************************************/

#define OBM_CONF_I2C_EMULATOR_CLK_GPIO				GPIO_165
#define OBM_CONF_I2C_EMULATOR_DATA_GPIO				GPIO_166


#define OBM_CONF_I2C_EMULATOR_CLK_MFPR_ADDR			0x40e10504
#define OBM_CONF_I2C_EMULATOR_DATA_MFPR_ADDR		0x40e10508
#define OBM_CONF_I2C_EMULATOR_CLK_MFPR_VALUE		0xd8c0
#define OBM_CONF_I2C_EMULATOR_DATA_MFPR_VALUE		0xd8c0



// 
#define PJ4_CPU_CONF_REG 	   	    (volatile unsigned long *)(0x58110008)

// PJ4 Trimming registers
#define PJ4_CPU_SRAM_SPD      (volatile unsigned long *)(0x58110010 )
#define PJ4_CPU_SRAM_SPD2     (volatile unsigned long *)(0x58110028 )
#define PJ4_CPU_L2C_SRAM_SPD  (volatile unsigned long *)(0x58110018 )

#define UP2OCR  (volatile unsigned long *)(0x40600020)

#define PMCR    (volatile unsigned long *)(0x40F50000)
#define PSR     (volatile unsigned long *)(0x40F50004)
#define PCFR    (volatile unsigned long *)(0x40F5000C)
#define PWER   	(volatile unsigned long *)(0x40F50010)
#define CSER   	(volatile unsigned long *)(0x40F5001C)
#define WDTSR  	(volatile unsigned long *)(0x40F50030)
#define PVCR  	(volatile unsigned long *)(0x40F50100)

#define PCFR_CLR_SL_ROD     	(0xFFFFFFFD)          // Must write 0xA to bits [17:14]
#define PCFR_WRM            	(0x00028000)          // Must write 0xA to bits [17:14]
#define PWER_WERTC_S3       	(0x80000005)
#define PVCR_VCSA_BIT       	(0x00004000)          // Bit 14
#define PVCR_SLV_ADDR_MASK  	(0x0000003F)          //

#define WDTSR_SB2_ADDR_SHIFT	(0x00000008)
#define WDTSR_SB2_ADDR_MASK		(0x0000ff00)

#define SERVICE_FUSES_REG2_ADD 	    ((volatile unsigned long *)(0x40F50208))    // Service fuse register 2
#define FUSES_QUALIFIER_ID_MASK			(0x03800000)
#define FUSES_QUALIFIER_ID_POS 			(23)
#define FUSES_EVALUATOR_MASK  			(0x0007FE00)
#define FUSES_EVALUATOR_POS 			(9)



#define UDCCR_REG 					 (volatile unsigned long *)(0x40600000) 	// USB1.1 client control register
#define OSCC						 (volatile unsigned long *)(0x41350000) 	// Oscillator Configuration Register
#define DMEM_MDCNFG_PHYSICAL_BASE 	 (volatile unsigned long *)(0x48100000)	    //
#define MDTAC                        (volatile unsigned long *)(0x48100010)
#define DDR_HCAL    			     (volatile unsigned long *)(0x48100060)
#define AGENP 						 (volatile unsigned long *)(0x40f4002c)		// DDR PLL control

#define ACCR     					 (volatile unsigned long *)(0x41340000) 	// Application SubSystem Clock Config register
#define ACSR     					 (volatile unsigned long *)(0x41340004) 	// Application SubSystem Clock Status register
#define D0CKEN_B 					 (volatile unsigned long *)(0x41340010) 	// D0 mode Clock Enable Register B
#define ACCR1    					 (volatile unsigned long *)(0x41340020) 	// Application Subsystem Clock Configuration Register 1
#define ACCR0    					 (volatile unsigned long *)(0x41340050) 	// Application Subsystem Clock Configuration Register 0 for Nevo C0
#define D0CKEN_C 					 (volatile unsigned long *)(0x41340024) 	// D0 mode Clock Enable Register C
#define XCLKCFG     	             (volatile unsigned long *)(0x41340034)     // Core Clock Configuration Register
#define XCLKCFG_MG1_B0               (volatile unsigned long *)(0x41340040)     // Core Clock Configuration Register
#define AVCR         				 (volatile unsigned long *)(0x40f50094)
/* Eshel B0 specific registers */
#define ACSCR     					 (volatile unsigned long *)(0x41340044) 	// Application SubSystem Clock Config register
#define ACSSR     					 (volatile unsigned long *)(0x41340048) 	// Application SubSystem Clock Status register
/* Nevo specific ACCU registers */
#define DDR_CLK_PROF	  	  		 (volatile unsigned long *)(0x41340028)
#define DDR_FC_CTRL                  (volatile unsigned long *)(0x4134002c)
#define DDR_SC_CTRL2                 (volatile unsigned long *)(0x40A000E4)

/* Nevo B0 specific registers */
#define VLSCR                        (volatile unsigned long *)(0x40F5005C)     //MPMU Voltage Level Select Control Register
#define AVLCR                        (volatile unsigned long *)(0x40F400A8)

#define VLSCR_SELECT_BIT             (BIT8)

//------------------------------------------------
//ygvili for dual rail chip
#define VLSCR_LVL0_SINGLE_RAIL_BIT_OFFSET 9
#define VLSCR_LVL1_SINGLE_RAIL_BIT_OFFSET 10
#define VLSCR_LVL2_SINGLE_RAIL_BIT_OFFSET 11
#define VLSCR_LVL3_SINGLE_RAIL_BIT_OFFSET 12
#define VLSCR_LPM_SINGLE_RAIL_BIT_OFFSET 13

#define VLSCR_LEVEL_SOURCE_SELECET_VCC_MAIN 1
#define AVLCR_LEVEL_BIT_OFFSET              1
#define AVLCR_LEVEL_BIT_SIZE                2
#define AVLCR_VC_GO_BIT_OFFSET              0


#define AVLCR_LEVEL_BIT_VOLTAGE_LEVEL_0     0x0
#define AVLCR_LEVEL_BIT_VOLTAGE_LEVEL_1     0x1
#define AVLCR_LEVEL_BIT_VOLTAGE_LEVEL_2     0x2
#define AVLCR_LEVEL_BIT_VOLTAGE_LEVEL_3     0x3
//------------------------------------------------


#if (SDHC2_SUPPORTED)
#define D0CKEN_C_MMC1_CLK_ENABLE_MASK ((1<<19) | /* AXI clock enable*/ \
                                       (1<<18) | /* island MMC USB system bus clock enable */ \
                                       (1<<1))   /* MMC1 clock enable */
#endif
#define ACCR1_OTG_PHY_ENABLE_MASK 	 ((1<<12) | (1<<11) | (1<<10)) 				// mask to OR on bits in ACCR1 for OTG PHY enable
#define D0CKEN_C_OTG_CLK_ENABLE_MASK ((1<<10) | (1<<6) )        				// mask to enable OTG AXI bus & Peripheral clocks
#define ACSR_USB_PLL_LOCK_MASK 		 (1<<27)                          			// mask to check status of USB_PLL LOCK state
#define USB2_PHY_T0 				 (volatile unsigned long *)(0x5550A010)     // phy control register
#define USB2_PHY_T0_OTG_CONTROL_BIT  (1 << 28)                   				// set phy control to slave from OTG
#define USB2_U2PIVREF 				 (volatile unsigned long *)(0x5550A00C)		// USB PHY RX control
#define USB2_U2PRX 					 (volatile unsigned long *)(0x5550A008)		// USB PHY RX control
#define USB2_U2PTX 					 (volatile unsigned long *)(0x5550A004)		// USB PHY tX control
#define USB2_U2PPLL 				 (volatile unsigned long *)(0x5550A000)		// USB PHY PLL control
#define USB2_U2PRS 					 (volatile unsigned long *)(0x5550A02C)		// USB PHY tX control
#define	USB2_PLL_READY				 BIT23				/* PLL_READY */
#define	USB2_PLL_VCOCAL_START		 BIT21				/* VCOCAL_START */
#define USB2_REG_RCAL_START			 BIT13				// REG_RCAL_START
#define USB2_U2PPLL_INIT 			 0xfe819eeb
#define USB2_U2PTX_INIT				 0x41e10fc3
#define USB2_U2PRX_INIT				 0xe31c82e1
#define	USB2_U2PIVREF_INIT			 0x2000017e
#define SQ_THRESH_MASK               0xFFFFFF3F 								//SQ Thresh mask fo U2PRX register to set SQ_THRESH to 3
#define U2OMCR                       (volatile unsigned long *)(0x55502300)		/* USB2 Mux Control Register */
#define XCS(n)                       ((n) <<10)                                 /* XCVR Clock Select */

#define OSCC_PDIV 				0x00080000

#define SMEMC_BASE	   0x4A000000

#define SMEMC_MSC0_ADDR	   		((volatile unsigned long*)(SMEMC_BASE | 0x0008))
#define SMEMC_MSC1_ADDR	   		((volatile unsigned long*)(SMEMC_BASE | 0x000C))
#define SMEMC_CLKCFG_ADDR	   	((volatile unsigned long*)(SMEMC_BASE | 0x0068))
#define SMEMC_CSADRCFG_ADDR	   	((volatile unsigned long*)(SMEMC_BASE | 0x008C))

typedef enum
{
	ACSCR_104_MHZ,
	ACSCR_156_MHZ,
	ACSCR_208_MHZ,
	ACSCR_312_MHZ,
	ACSCR_416_MHZ
} ACSCR_VALUE;

typedef struct   arbelResetRoutineS
{
	VUINT32_T   Arbel_ResetRoutine[8];    /* LONGs - 32 bit */
} ARBEL_ResetRoutineS;

typedef struct   GbResetRoutineS
{
	VUINT16_T   Gb_ResetRoutine[16];      /* Shorts */
} GB_ResetRoutineS;

#define RESET_ROUTINE_NEVO_GB_ADDR            (0x80000020)
#define RESET_ROUTINE_NEVO_ARBEL_ADDR         (0x80000000)


//----------------------------------------------------
// Nevo C0 Apps Power Management Unit (Apps PMU)
//----------------------------------------------------
#define APMU_REG_ADRESS_BASE		(0x40F40000)
#define APMU_TMP_CTRL_REG           (volatile unsigned long *)(APMU_REG_ADRESS_BASE +0xC8)
#define APMU_TMP_TH_REG             (volatile unsigned long *)(APMU_REG_ADRESS_BASE +0xCC)
#define APMU_TMP_INTR_REG           (volatile unsigned long *)(APMU_REG_ADRESS_BASE +0xD0)
#define APMU_TMP_CFG_REG            (volatile unsigned long *)(APMU_REG_ADRESS_BASE +0xD4)

// Nevo C0 APMU thermal sensor register fields definition
#define APMU_TMP_CTRL_EN_DIS_BIT_OFFSET     0           //RW
#define APMU_TMP_CTRL_OWM_BIT_OFFSET        1           //RW
#define APMU_TMP_CTRL_TSR_OFFSET            2           //RO
#define APMU_TMP_CTRL_T_VALID_BIT_OFFSET    11          //RO
#define APMU_TMP_CTRL_TSR_MASK              (0x7FC)

#define APMU_TMP_TH_OVWF_OFFSET             0
#define APMU_TMP_TH_OHTIF_OFFSET            10
#define APMU_TMP_TH_OLTIF_OFFSET            20
#define APMU_TMP_TH_OVWF_MASK               (0x1FF)
#define APMU_TMP_TH_OHTIF_MASK              (0x1FF)
#define APMU_TMP_TH_OLTIF_MASK              (0x1FF)

#define APMU_TMP_INTR_TMPL_IE_BIT_OFFSET        0
#define APMU_TMP_INTR_TMPH_IE_BIT_OFFSET        1
#define APMU_TMP_INTR_TMPL_IS_BIT_OFFSET        2
#define APMU_TMP_INTR_TMPH_IS_BIT_OFFSET        3

typedef enum {
	e_PROTECTED,
	e_UNPROTECTED
} e_TSU_MODE ;

//APMU TSU enumerations
typedef enum {
	APMU_OLTIF_TH = 0,		//Over Heating Low Threshold
	APMU_OHTIF_TH,			//Over Heating High Threshold Value
	APMU_OVWF_TH				//WDT Reset Temp Over Heating Threshold
} e_TSU_THRESHOLD_INDEX;


UINT_T PlatformDisablePMICWatchDog(void);
UINT_T PlatformSetThermalSensor(UINT_T temp_value);
void PlatformGetFuses(pFUSE_SET pFuses);
void OBM2OSL_Init(void);

//external prototypes
extern UINT_T GetOSCR0(void);
extern UINT_T OSCR0IntervalInSec(UINT_T Before, UINT_T After);
extern UINT_T OSCR0IntervalInMilli(UINT_T Before, UINT_T After);
extern UINT_T OSCR0IntervalInMicro(UINT_T Before, UINT_T After);
extern void DisableIrqInterrupts();


#endif
