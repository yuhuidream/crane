/******************************************************************************
 *
 *  (C)Copyright 2005 - 2011 Marvell. All Rights Reserved.
 *
 *  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF MARVELL.
 *  The copyright notice above does not evidence any actual or intended
 *  publication of such source code.
 *  This Module contains Proprietary Information of Marvell and should be
 *  treated as Confidential.
 *  The information in this file is provided for the exclusive use of the
 *  licensees of Marvell.
 *  Such users have the right to use, modify, and incorporate this code into
 *  products for purposes authorized by the license agreement provided they
 *  include this notice and the associated copyright notice with any such
 *  product.
 *  The information in this file is provided "AS IS" without warranty.

 ******************************************************************************
**
**  FILENAME:	TIM.h
**
**  PURPOSE: 	Defines the Trusted/Non-Trusted Image Module
**
**
******************************************************************************/

#ifndef __TIM_H__
#define __TIM_H__
#include "predefines.h"

#define MAXRSAKEYSIZEWORDS 30
#define MAXECCKEYSIZEWORDS 10

/******************* TIM Layout ************************************/
/* Below Presents the Maxiumum Size TIM layout in memory

VERSION_I      	VersionBind;         			//
FLASH_I        	FlashInfo;           			//
uint32_t         	NumImages;           			//
uint32_t         	NumKeys;						//
uint32_t         	SizeOfReserved;					//
IMAGE_INFO 		IMG[MAX_IMAGES];                //
KEY_MOD        	Key[MAX_KEYS];                  //
uint32_t         	Reserved[0x4E8];       			//
PLAT_DS        	TBTIM_DS;                       //
                                                //
Below Presents the Maxiumum Size NTIM layout in memory

VERSION_I   	VersionBind;         			//
FLASH_I     	FlashInfo;           			//
uint32_t      	NumImages;           			//
uint32_t         	NumKeys;						//
uint32_t      	SizeOfReserved;					//
IMAGE_INFO 		IMG[MAX_IMAGES];                //
uint32_t      	Reserved[0xD80];       			//
NTIM, *pNTIM;									//
******************* TIM Layout ************************************/

// TIM Versions
#define TIM_3_2_00			0x30200			// Support for Partitioning
#define TIM_3_3_00			0x30300			// Support for ECDSA-256
#define TIM_3_4_00			0x30400			// Support for ECDSA-521
#define TIM_3_5_00			0x30500			// Support for Encrypted Boot

// B1 TIM
#define MAX_IMAGES				20
#define MAX_KEYS	 			3
#define MAX_TIM_SIZE 			0x2000			//8k
#define TIMBUFFER 				4096    		//4k for a Tim structure size
#define MAXKEYSIZEWORDS		64				// 2048 bits

#define KeyLength_16						16
#define KeyLength_32						32
#define KeyLength_64						64

// Predefined Image Identifiers
#define TIMIDENTIFIER		0x54494D48		// "TIMH"
#define TIMDUALBOOTID		0x54494D44		// "TIMD"
#define WTMIDENTIFIER		0x57544D49		// "WTMI"
#define OBMIDENTIFIER		0x4F424D49		// "OBMI"
#define OBM2IDENTIFIER		0x4F424D32		// "OBM2"
#define MONITORIDENTIFIER	0x4D4F4E49		// "MONI"
#define TZSWIDENTIFIER		0x545A5349		// "TZSI"    This is also used as consumer ID
#define TZIIDENTIFIER		0x545A4949		// "TZII"    This is also used as consumer ID
#define TBRIDENTIFIER		0x54425249		// "TBRI"	 This is also used as consumer ID
#define DKBIDENTIFIER		0x444B4249		// "DKBI"
#define JTAGIDENTIFIER		0x4A544147		// "JTAG"
#define PATCHIDENTIFIER		0x50415443		// "PATC"
#define TCAIDENTIFIER		0x5443414B		// "TCAK"
#define OSLOADERID			0x4F534C4F		// "OSLO"
#define OSLOADERRECOVERYID	0x4F534C52		// "OSLR"
#define PARTIONIDENTIFIER	0x50415254      // "PART"
#define FBFIDENTIFIER       0x46424649      // "FBFI" Should never appear in actual TIM file
#define HSIBOOTID			0x48533939		// "HSII"
#define RELIABLEID			0x52424C49		// "RBLI"
#define FBFIDENTIFIER2      0x46424644      // "FBFD"
#define FBFIDENTIFIER0      0x46424600      // "FBFx"
#define NONTZDTIMID			0x54494D4E		// "TIMN" representing Non-TZ DTIM
#define EMMDIDENTIFIER		0x454d4d44		// "EMMD"

#define TIM_TYPE			0x54494D00		// "TIMx"
#define DTIM_PRIMARY		0x54494D31		// "TIM1"
#define DTIM_RECOVERY		0x54494D32		// "TIM2"
#define DTIM_CP				0x54494D33		// "TIM3"
#define DTIM_MRD			0x54494D34		// "TIM4"

// Defined for DTIM support
typedef enum
{
	TIMH_NOTINC				= 0,
	TIMH_INC				= 1,
	DTIM_PRIMARY_INC		= 2,
	DTIM_RECOVERY_INC		= 3,
	DTIM_CP_INC				= 4,
	TIMH_RECOVERY_INC		= 5,
	// 6-9 reserved
	DTIM_CUSTMOZIEDTYPE1	= 10,
	DTIM_CUSTMOZIEDTYPE2	= 11,
	DTIM_CUSTMOZIEDTYPE3	= 12,
	DTIM_CUSTMOZIEDTYPE4	= 13,
	DTIM_CUSTMOZIEDTYPE5	= 14,
	DTIM_CUSTMOZIEDTYPE6	= 15,
	DTIM_CUSTMOZIEDTYPE7	= 16,
	DTIM_CUSTMOZIEDTYPE8	= 17,
	DTIM_CUSTMOZIEDTYPE9	= 18,
	DTIM_CUSTMOZIEDTYPE10	= 19,
	DTIM_CUSTMOZIEDTYPE11	= 20,
	DTIM_CUSTMOZIEDTYPE12	= 21,
	DTIM_CUSTMOZIEDTYPE13	= 22,
	DTIM_CUSTMOZIEDTYPE14	= 23,
	DTIM_CUSTMOZIEDTYPE15	= 24,
	DTIM_CUSTMOZIEDTYPE16	= 25,
	DTIM_CUSTMOZIEDTYPE17	= 26,
	DTIM_CUSTMOZIEDTYPE18	= 27,
	DTIM_CUSTMOZIEDTYPE19	= 28,
	DTIM_CUSTMOZIEDTYPE20	= 29,

	DTIM_CUSTMOZIED_MAX		= 50
}TIMIncluded_Type;

//Define ID types
//  Use upper 3 bytes as a type identifier and allow up to 256 of a particular type
//
#define TYPEMASK			0xFFFFFF00
#define DDRTYPE				0x444452	 	// "DDR"
#define TIMTYPE				0x54494D		// "TIM"
#define TZRTYPE				0x545A52		// "TZR"
#define GPPTYPE				0x475050		// "GPP"

// WTP Format Recognized Reserved Area Indicator
#define WTPRESERVEDAREAID	0x4F505448      // "OPTH"

// Reserved Area Package Headers
#define TERMINATORID		0x5465726D		// "Term"
#define GPIOID				0x4750494F		// "GPIO"
#define UARTID				0x55415254		// "UART"
#define USBID				0x00555342		// "USB"
#define FORCEUSBID			0x46555342		// "FUSB"
#define RESUMEID			0x5265736D		// "Resm"
#define USBVENDORREQ		0x56524551		// "VREQ"
#define TBR_XFER			0x54425258		// "TBRX"
#define RESUMEBLID			0x52736D32		// "Rsm2"
#define ESCAPESEQID			0x45534353		// "ESCS"
#define ESCSEQID_v2			0x45535632		// "ESV2"	Escape Sequence Version 2
#define OEMCUSTOMID			0x43555354		// "CUST"
#define NOMONITORID			0x4E4F4D43		// "NOMC"
#define COREID				0x434F5245		// "CORE"
#define COREID_V2           0x43525632      // "CRV2"

#define SOCPROFILEID		0x534F4350		// "SOCP"
#define BBMTYPEID			0x42424D54		// "BBMT"
#define QNXBOOT             0x514E5850      // "QNXP"
#define GPP1ID				0x47505031		// "GPP1"  Processed in RESERVEDAREA_State
#define GPP2ID				0x47505032		// "GPP2"  Processed in XFER_State
#define ROMRESUMEID			0x52736D33		// "Rsm3"
#define PINID 				0x50494E50		// "PINP"
#define IMAPID 				0x494D4150		// "IMAP"
#define NOFREQUENCYID		0x4E4F4643		// "NOFC", no frequency change
#define NOKEYDETECTID		0x4E4F4B44		// "NOKD", no key detect

// DDR Related ID's
#define DDRID				0x44447248		// "DDRH"
#define DDRGID				0x44445247		// "DDRG"
#define DDRTID				0x44445254		// "DDRT"
#define DDRCID				0x44445243		// "DDRC"
#define CMCCID				0x434d4343		// "CMCC"

// new DDR configuration related ID's
#define CIDPID              0x43494450      // "CIDP"
#define TZRI				0x545A5249		// "TZRI" Trustzone
#define MAX_SCRATCH_MEMORY_ID	15			// 16 Scratch Memories defined

// Operating mode related ID's
#define FREQID				0x46524551		// "FREQ"
#define VOLTID				0x564f4c54		// "VOLT"
#define OPMODEID			0x4d4f4445		// "MODE"
#define OPDIVID 			0x4f504456		// "OPDV"
#define CLKEID				0x434c4b45		// "CLKE"

//Trustzone
#define TZID                0x545A4944      // "TZID" Field based - Legacy
#define TZON                0x545A4f4E      // "TZON" Register based - Legacy

// USB
#define DESCRIPTOR_RES_ID	0x55534200      // "USB"
#define NUM_USB_DESCRIPTORS			10
#define MAX_USB_STRINGS 			7

typedef enum
{
 USB_DEVICE_DESCRIPTOR   				= (DESCRIPTOR_RES_ID | 0),
 USB_QUALIFIER_DESCRIPTOR   		= (DESCRIPTOR_RES_ID | 1),	// added for Qualifier Descriptor - xxliu
 USB_CONFIG_DESCRIPTOR   				= (DESCRIPTOR_RES_ID | 2),
 USB_OTHERSPEED_DESCRIPTOR   		= (DESCRIPTOR_RES_ID | 3),	// added for Other Speed Descriptor - xxliu
 USB_INTERFACE_DESCRIPTOR 				= (DESCRIPTOR_RES_ID | 4),
 USB_LANGUAGE_STRING_DESCRIPTOR   		= (DESCRIPTOR_RES_ID | 5),
 USB_MANUFACTURER_STRING_DESCRIPTOR   	= (DESCRIPTOR_RES_ID | 6),
 USB_PRODUCT_STRING_DESCRIPTOR   	  	= (DESCRIPTOR_RES_ID | 7),
 USB_SERIAL_STRING_DESCRIPTOR   	  	= (DESCRIPTOR_RES_ID | 8),
 USB_INTERFACE_STRING_DESCRIPTOR   		= (DESCRIPTOR_RES_ID | 9),
 USB_DEFAULT_STRING_DESCRIPTOR			= (DESCRIPTOR_RES_ID | 10),
 USB_ENDPOINT_DESCRIPTOR 				= (DESCRIPTOR_RES_ID | 11)
} USB_DESCRIPTORS;

typedef enum    // use with COREID	0x434F5245 // "CORE"
{
	COREID_MP1 = 0,
	COREID_MP2 = 1,
	COREID_MM  = 2,
	COREID_MP1_MM = 3,
	COREID_MP2_MM = 4,
	COREID_MP3 = 5,
	COREID_MP4 = 6,
	COREID_NONE = 0x4E4F4E45				// "NONE"
}
COREID_T;

typedef enum    // use with COREID_v2  0x43525632 // "CRV2"
{
	CORE_AP0 = 0,
	CORE_AP1 = 1,
	CORE_AP2 = 2,
	CORE_AP3 = 3,
	CORE_AP4 = 4,
	CORE_AP5 = 5,
	CORE_AP6 = 6,
	CORE_AP7 = 7,
	CORE_NONE = 0x4E4F4E45				// "NONE"
}
COREID_V2_T;

typedef enum
{
	INSTR_TEST_FOR_EQ = 1,
	INSTR_TEST_FOR_NE = 2,
	INSTR_TEST_FOR_LT = 3,
	INSTR_TEST_FOR_LTE = 4,
	INSTR_TEST_FOR_GT = 5,
	INSTR_TEST_FOR_GTE = 6,
}
INSTRUCTION_TEST_OPERATORS_T;


// Global Identifiers
#define FFIDENTIFIER		0x00004646		// "FF"
#define ALTIDENTIFIER		0x00414C54		// "ALT"
#define DIFFIDENTIFIER		0x44696666		// "Diff"
#define SEIDENTIFIER		0x00005345		// "SE"
#define U2DIDENTIFIER		0x55534232		// "USB2"
#define PINSIDENTIFIER		0x50696E73		// "Pins"
#define MDOCFORMATID        0x6d444f43      // "mDOC"
#define MDOCBINID           0x6d42944E      // "mBIN"
#define MDOCBDTLID          0x6d424454	    // "mBDT"
#define CI2IDENTIFIER       0x00434932     // "CI2"
#define OTGIDENTIFIER		0x554F5447		// "UOTG"
#define INVALIDID           0x21212121      // "!!!!" Should never appear in actual TIM file

// Miscellanous
#define BINDKEYSIZE			129				// (MAXKEYSIZEWORDS * 2) + 1 for crypto scheme algo
#define FLASHSIGMASK		0x000000FF		// mask off the 8 bits of platform state (only 5 currently used)

/********** WTP Recognized Reserved Area Layout ********************************
*
*	WTPTP_Defined_Reserved_Format_ID    \	  This clues BR, OBM and DKB that the reserved area is in a known format
*	Number of Reserved Area Packages    /	  For each package there is a header, payload size and payload
*
*	Header	 	 		\	  Indicates what type of a Reserved Area Package
*	Size		  		 \	  Size Comprises a single Reserved Area Package
*	Payload		  		 /	  There may be any number of Packages so long as TIM/NTIM < 4KB
*			 			/
*
*	Header		 		\	  The Last one should be a Package with a Terminator Header
*	Size		 		/	  The size should be 8 bytes (the size of this package)
*
**********************************************************************************/
typedef struct
{
 uint32_t WTPTP_Reserved_Area_ID;	  	// This clues BR, OBM and DKB that the reserved area is in a known format
 uint32_t	NumReservedPackages;	  	// For each package there is a header, payload size and payload
}WTP_RESERVED_AREA, *pWTP_RESERVED_AREA;

typedef struct
{
 uint32_t Identifier;					// Identification of this reserved area entry
 uint32_t Size;						// Size  = Payload Size + 2 words (8 bytes).
}WTP_RESERVED_AREA_HEADER, *pWTP_RESERVED_AREA_HEADER;

typedef struct
{
	WTP_RESERVED_AREA_HEADER WRAH;
	uint32_t value;
}OPT_OEM_CUST, *P_OPT_OEM_CUST;

typedef struct
{
 WTP_RESERVED_AREA_HEADER WRAH;
 uint32_t 	bmRequestType;
 uint32_t 	bRequest;
 uint32_t		wValue;
 uint32_t		wIndex;
 uint32_t		wLength;
 uint32_t		wData; 							// First word of the proceeding Data. Note, there may be more traialing data
} USB_VENDOR_REQ, *pUSB_VENDOR_REQ;			// There is no restriction that data need be 32 bit aligned.

typedef struct
{
 volatile int *Addr;
 uint32_t Value;
}GPIO_DEF, *pGPIO_DEF;

typedef struct
{
 uint32_t GPIO_Num;
 uint32_t Value;
}GPIO_PIN_DEF, *pGPIO_PIN_DEF;


typedef struct
{
 WTP_RESERVED_AREA_HEADER WRAH;
 uint32_t Version;					// 0 = not supported, 1 = MBBT v1, 2 = MBBT v2
}BBM_VERSION_REQ, *pBBM_VERSION_REQ;

typedef struct
{
 WTP_RESERVED_AREA_HEADER WRAH;
 uint32_t Port;
 uint32_t Enabled; 					// 1 - Enabled
}OPT_PROTOCOL_SET, *pOPT_PROTCOL_SET;

typedef struct
{
 WTP_RESERVED_AREA_HEADER WRAH;
 uint32_t EscSeqTimeOutMS;  				// Value is interpreted in milli secs.
}OPT_ESCAPE_SEQUENCE, *P_OPT_ESCAPE_SEQUENCE;

typedef struct
{
	uint32_t MFPR_address;
	uint32_t MFPR_value;
} MFPR_PAIR_T, *pMFPR_PAIR_T;

typedef struct
{
	WTP_RESERVED_AREA_HEADER WRAH;
	uint32_t GPIO_number;			//GPIO to check
	uint32_t GPIO_trigger_val;	//value of GPIO to trigger the escape sequence
	uint32_t num_MFPR_pairs;		//number of MFPR pairs contained in this package
	MFPR_PAIR_T mfpr_pairs[1];	//MFPR address/value pairs: array will be of size 'num_MFPR_pairs"
} OPT_ESC_SEQ_v2, *P_OPT_ESC_SEQ_v2;

// Do not remove - used by TBR and BootLoader
#define RESUME_FLAG_MASK 0x55AA55AA

typedef struct
{
 WTP_RESERVED_AREA_HEADER WRAH;
 uint32_t NumGpios;
 pGPIO_DEF	GPIO;
}OPT_GPIO_SET, *pOPT_GPIO_SET;

typedef struct
{
 WTP_RESERVED_AREA_HEADER WRAH;
 uint32_t ResumePackageDDRAddress;				// Address of the Resume Package in DDR
}OPT_TIM_RESUME_INFO, *pOPT_TIM_RESUME_INFO;


/* Do we need this.
typedef struct
{
 WTP_RESERVED_AREA_HEADER WRAH;
 uint32_t ACCR_VALUE;
 uint32_t MDCNFG_VALUE;
 uint32_t DDR_HCAL_VALUE;
 uint32_t MDREFR_VALUE;
}OPT_DDR_SET, *pOPT_DDR_SET;
*/
typedef struct
{
 WTP_RESERVED_AREA_HEADER WRAH;
 uint32_t CoreId;		// See COREID_T
 uint32_t AddressMapping;
}OPT_COREID, *pOPT_COREID;

// new V2 core release package
typedef struct
{
    COREID_V2_T CoreId;
    uint32_t VectorAddress;
}
CORE_RELEASE_PAIR_T, *pCORE_RELEASE_PAIR_T;

typedef struct
{
    WTP_RESERVED_AREA_HEADER      WRAH;
    uint32_t                        num_cores_release_pairs;        // number of cores to release
    CORE_RELEASE_PAIR_T           core_pairs[1];                  // will be size of "num_cores"
}OPT_CORE_RELEASE, *pOPT_CORE_RELEASE;

typedef struct
{
 WTP_RESERVED_AREA_HEADER WRAH;
 uint32_t FuseIndex;		// Currently not used
 uint32_t StoreAddress;
}OPT_SOCPROFILEID, *pOPT_SOCPROFILEID;

// PIN Package
typedef struct
{
	WTP_RESERVED_AREA_HEADER WRAH;
	uint32_t	PinA;	// low 32 bits of 64 bit PIN
	uint32_t	PinB;	// high 32 bits of 64 bit PIN
} PIN, *pPIN;

// OTA related image type
typedef enum
{
   	PrimaryImage 	= 0,                // Primary image
   	RecoveryImage 	= 1,               	// Recovery image
   	CPImage 		= 2,				// CP image
   	
   	MARVELL_RESERVED_MAX = 9,   
	CUSTMOZIEDIMAGE_TYPE1 = 10,
	CUSTMOZIEDIMAGE_TYPE2 = 11,
	CUSTMOZIEDIMAGE_TYPE3 = 12,
	CUSTMOZIEDIMAGE_TYPE4 = 13,
	CUSTMOZIEDIMAGE_TYPE5 = 14,
	CUSTMOZIEDIMAGE_TYPE6 = 15,
	CUSTMOZIEDIMAGE_TYPE7 = 16,
	CUSTMOZIEDIMAGE_TYPE8 = 17,
	CUSTMOZIEDIMAGE_TYPE9 = 18,
	CUSTMOZIEDIMAGE_TYPE10 = 19,
	CUSTMOZIEDIMAGE_TYPE11 = 20,
	CUSTMOZIEDIMAGE_TYPE12 = 21,
	CUSTMOZIEDIMAGE_TYPE13 = 22,
	CUSTMOZIEDIMAGE_TYPE14 = 23,
	CUSTMOZIEDIMAGE_TYPE15 = 24,
	CUSTMOZIEDIMAGE_TYPE16 = 25,
	CUSTMOZIEDIMAGE_TYPE17 = 26,
	CUSTMOZIEDIMAGE_TYPE18 = 27,
	CUSTMOZIEDIMAGE_TYPE19 = 28,
	CUSTMOZIEDIMAGE_TYPE20 = 29,
	
	CUSTMOZIEDIMAGE_TYPE_MAX = 50,
} OTA_IMAGE_TYPE;

// Image Map Info struct of the Image Map ("IMAP") Package
typedef struct
{
 	uint32_t ImageID;                    	// 0x54494Dxx   - secondary tim ID (TIMx)
 	OTA_IMAGE_TYPE ImageType;          	// OTA_IMAGE_TYPE (Primary VS Recovery)
 	uint32_t FlashEntryAddr[2];          	// Reserve 2 words for future compatibility
 	uint32_t PartitionNumber;            	// Partition # in the flash
} IMG_MAP_INFO, *pIMG_MAP_INFO; 

// Image Map ("IMAP") Package
typedef struct
{
 	WTP_RESERVED_AREA_HEADER WRAH;     	// Let it contain "IMAP" as package ID 
 	uint32_t NumberOfMappedImages;       	// Number of images with mapping information
	IMG_MAP_INFO pImgMapInfo[1];       	// First image map information struct
} IMAGE_MAP, *pIMAGE_MAP;   

// On-demand Boot ("ODBT") Package
typedef struct
{
	WTP_RESERVED_AREA_HEADER WRAH;
   	uint32_t MagicWord;				//Allow user to specify the magic word in TIM package
	uint32_t MagicWordAddress;		//Address to look for the Magic word
	uint32_t MagicWordPartition;		//Partition of the Magic Word
	uint32_t NumImages;
	uint32_t ODBTImageList[1];    	//First image ID in new list
} ODBT_PACKAGE, *pODBT_PACKAGE;

// DDR Configuration related structures
typedef struct
{
	unsigned int	ConsumerID;
	unsigned int    NumPackagesToConsume;
	unsigned int	PackageIdentifierList;
} CIDP_ENTRY, *pCIDP_ENTRY;

typedef struct
{
	WTP_RESERVED_AREA_HEADER WRAH;
	unsigned int	NumConsumers;
	CIDP_ENTRY		Consumers;
} CIDP_PACKAGE, *pCIDP_PACKAGE;

typedef struct
{
	unsigned int 	OpId;
	unsigned int	OpValue;
} DDR_OPERATION, *pDDR_OPERATION;

 typedef enum DDR_OPERATION_SPEC_E
{
	DDR_NOP = 0,           		// Enums specify the following:
    DDR_INIT_ENABLE = 1,        // DDR init flag (1 = init)
    DDR_MEMTEST_ENABLE = 2,     // DDR Memtest flag (1 = test)
    DDR_MEMTEST_START_ADDR = 3, // Memtest start addr (Current default is 0)
    DDR_MEMTEST_SIZE = 4,       // NumBytes to test (Current default is 2K)
    DDR_INIT_LOOP_COUNT = 5,    // Loop Count for initialization attempts
    DDR_IGNORE_INST_TO = 6,		// Treat a time out waiting for a bit to set/clear
							  	// as informational only, operation continues
	// New values go above here
	DDR_OPERATION_SPEC_E_MAX
} DDR_OPERATION_SPEC_T;

//
// Generic structures for handling instructions
//	Number of parameters are inherent in the command type according to:
//	Function					Descriptor			InstructionID	P1		P2				P3
//								   Text
//	 Write Register				WRITE				0x1				Addr	 Value			n/a
//   Read Register				READ				0x2				Addr	Num of reads	n/a
//   Delay Specified Time		DELAY				0x3				Value	n/a 			n/a
//   WaitForOperationToSetBit	WAIT_FOR_BIT_SET	0x4				Addr	Mask			TimeOut Value
//   WaitForOperationToClearBit	WAIT_FOR_BIT_CLEAR	0x5				Addr	Mask			TimeOut Value
//   AND value with Register	AND_VAL				0x6				Addr	Value			n/a
//   OR value with Register		OR_VAL				0x7				Addr 	Value			n/a
//

typedef enum INSTRUCTION_OP_CODE_SPEC_E
{
    // Enums specify the following:
	INSTR_NOP = 0,
	// Register instructions
    INSTR_WRITE = 1,
    INSTR_READ  = 2 ,
    INSTR_DELAY = 3,
    INSTR_WAIT_FOR_BIT_SET = 4,
    INSTR_WAIT_FOR_BIT_CLEAR = 5,
    INSTR_AND_VAL = 6,
    INSTR_OR_VAL = 7,
	INSTR_SET_BITFIELD = 8,
	INSTR_WAIT_FOR_BIT_PATTERN = 9,
	INSTR_TEST_IF_ZERO_AND_SET = 10,
	INSTR_TEST_IF_NOT_ZERO_AND_SET = 11,
	// Scratch Memory (SM) instructions
	INSTR_LOAD_SM_ADDR = 12,
	INSTR_LOAD_SM_VAL = 13,
	INSTR_STORE_SM_ADDR = 14,
	INSTR_MOV_SM_SM = 15,
	INSTR_RSHIFT_SM_VAL = 16,
	INSTR_LSHIFT_SM_VAL = 17,
	INSTR_AND_SM_VAL = 18,
	INSTR_OR_SM_VAL = 19,
	INSTR_OR_SM_SM = 20,
	INSTR_AND_SM_SM = 21,
	INSTR_TEST_SM_IF_ZERO_AND_SET = 22,
	INSTR_TEST_SM_IF_NOT_ZERO_AND_SET = 23,
	// Conditional Execution Support
	INSTR_LABEL = 24,
	INSTR_TEST_ADDR_AND_BRANCH = 25,
	INSTR_TEST_SM_AND_BRANCH = 26,
	INSTR_BRANCH = 27,
	// Interpreter Directives
	INSTR_END = 28,
	//Add/Subtract
	INSTR_ADD_SM_VAL = 29,	
	INSTR_ADD_SM_SM = 30,
	INSTR_SUB_SM_VAL = 31,	
	INSTR_SUB_SM_SM = 32,		
 	 // New values go above here
    INSTRUCTION_OP_CODE_E_MAX
} INSTRUCTION_OP_CODE_SPEC_T;

 typedef struct
{
	unsigned int	InstructionId;  // InstructionID determines # of params for defined instructions
	unsigned int   Parameters[1];		// Parameters[ MAX_NUMBER_PARAMS = 5 ]
} INSTRUCTION_S, *pINSTRUCTION_S;

typedef struct
{
	WTP_RESERVED_AREA_HEADER WRAH;
    unsigned int		NumberOperations;
	unsigned int 		NumberInstructions;
	DDR_OPERATION		DDR_Operations;            // DDR_Op[NumberOperations];
	INSTRUCTION_S	 	DDR_Instructions;	// DDR_Instruction[NumberInstructions];
} DDR_PACKAGE, *pDDR_PACKAGE;
// End New DDR Configuration Related structures

// New Trustzone related structures
typedef struct
{
	unsigned int 	OpId;
	unsigned int	OpValue;
} TZ_OPERATION, *pTZ_OPERATION;

typedef enum TZ_OPERATION_SPEC_E
{
    TZ_NOP = 0,              	// Enums specify the following:
    TZ_CONFIG_ENABLE = 1,       // TZ config enable flag (1 = configure)
	TZ_IGNORE_INST_TO = 6, 		// Treat a time out waiting for a bit to set/clear
								// as informational only, operation continues
    // New values go above here
    TZ_OPERATION_SPEC_E_MAX
} TZ_OPERATION_SPEC_T;

typedef struct
{
	WTP_RESERVED_AREA_HEADER WRAH;
    unsigned int		NumberOperations;
	unsigned int 		NumberInstructions;
	TZ_OPERATION		TZ_Operations;      // TZ_Op[NumberOperations];
	INSTRUCTION_S	 	TZ_Instructions;	// TZ_Instruction[NumberInstructions];
} TZ_PACKAGE, *pTZ_PACKAGE;
// End Trustzone related structures

// General Purpose Patch (GPPx) related structures
typedef struct
{
	unsigned int 	OpId;
	unsigned int	OpValue;
} GPP_OPERATION, *pGPP_OPERATION;

typedef enum GPP_OPERATION_SPEC_E
{
    GPP_NOP = 0,              	// Enums specify the following:
	GPP_IGNORE_INST_TO = 6,		// Treat a time out waiting for a bit to set/clear
								// as informational only, operation continues
    // New values go above here
    GPP_OPERATION_SPEC_E_MAX
} GPP_OPERATION_SPEC_T;

typedef struct
{
	WTP_RESERVED_AREA_HEADER WRAH;
    unsigned int		NumberOperations;
	unsigned int 		NumberInstructions;
	GPP_OPERATION		GPP_Operations;     // GPP_Op[NumberOperations];
	INSTRUCTION_S	 	GPP_Instructions;	// GPP_Instruction[NumberInstructions];
} GPP_PACKAGE, *pGPP_PACKAGE;
// End General Purpose Patch related structures


/****  Used only for backwards compatability!!! ***/
typedef struct
{
 uint32_t Identifier;
 uint32_t PortType;
 uint32_t Port;
 uint32_t GPIOPresent;
 uint32_t NumGpios;
 pGPIO_DEF	GPIO;
}OPT_SET, *pOPT_SET;

/*********************************************************************************/

typedef enum
{
	PlatformVerificationKey,
	NetworkOperatorCAKey,
	SoftwareUpdateCAKey,
	DRMDeviceCAKey,
	OEMReservedKey1,
	OEMReservedKey2,
	NUMKEYMODULES
}
KEYMODULES_T;

typedef enum
{
	Marvell_DS =		0x00000000,
	PKCS1_v1_5_Caddo =	0x00000001,
	PKCS1_v2_1_Caddo =	0x00000002,
	PKCS1_v1_5_Ippcp =	0x00000003,
	PKCS1_v2_1_Ippcp =	0x00000004,
	ECDSA_256 =			0x00000005,
	ECDSA_521 =			0x00000006,
	AES_TB_CTS_ECB128 =	0x0001E000,
	AES_TB_CTS_ECB256 =	0x0001E001,
	AES_TB_CTS_ECB192 =	0x0001E002,
	AES_TB_CTS_CBC128 =	0x0001E004,
	AES_TB_CTS_CBC256 =	0x0001E005,
	AES_TB_CTS_CBC192 =	0x0001E006,
	DUMMY_ENALG =		0x7FFFFFFF
}
ENCRYPTALGORITHMID_T;

#define Intel_DS Marvell_DS

typedef enum
{
	SHA160 = 0x00000014,   //20
	SHA256 = 0x00000020,   //32
	SHA512 = 0x00000040,   //64
	DUMMY_HASH = 0x7FFFFFFF
}
HASHALGORITHMID_T;

typedef struct
{
 uint32_t Version;
 uint32_t	Identifier;					// "TIMH"
 uint32_t Trusted;					// 1- Trusted, 0 Non
 uint32_t IssueDate;
 uint32_t OEMUniqueID;
} VERSION_I, *pVERSION_I;			// 0x10 bytes

typedef struct
{
 uint32_t WTMFlashSign;
 uint32_t WTMEntryAddr;
 uint32_t WTMEntryAddrBack;
 uint32_t WTMPatchSign;
 uint32_t WTMPatchAddr;
 uint32_t BootFlashSign;
} FLASH_I, *pFLASH_I;				// 0x10 bytes

typedef struct
{
 uint32_t ImageID;						// Indicate which Image
 uint32_t NextImageID;					// Indicate next image in the chain
 uint32_t FlashEntryAddr;					// Block numbers for NAND
 uint32_t LoadAddr;
 uint32_t ImageSize;
 uint32_t ImageSizeToHash;
 HASHALGORITHMID_T HashAlgorithmID;		// See HASHALGORITHMID_T
 uint32_t Hash[16];						// Reserve 512 bits for the hash
 uint32_t PartitionNumber;
 ENCRYPTALGORITHMID_T EncAlgorithmID;	// See ENCRYPTALGORITHMID_T
 uint32_t EncryptStartOffset;
 uint32_t EncryptSize;
} IMAGE_INFO_3_5_0, *pIMAGE_INFO_3_5_0;			// 0x60 bytes


typedef struct
{
 uint32_t ImageID;					// Indicate which Image
 uint32_t NextImageID;				// Indicate next image in the chain
 uint32_t FlashEntryAddr;			 	// Block numbers for NAND
 uint32_t LoadAddr;
 uint32_t ImageSize;
 uint32_t ImageSizeToHash;
 HASHALGORITHMID_T HashAlgorithmID;            // See HASHALGORITHMID_T
 uint32_t Hash[16];					// Reserve 512 bits for the hash
 uint32_t PartitionNumber;
} IMAGE_INFO_3_4_0, *pIMAGE_INFO_3_4_0;			// 0x60 bytes

typedef struct
{
 uint32_t ImageID;					// Indicate which Image
 uint32_t NextImageID;				// Indicate next image in the chain
 uint32_t FlashEntryAddr;			 	// Block numbers for NAND
 uint32_t LoadAddr;
 uint32_t ImageSize;
 uint32_t ImageSizeToHash;
 HASHALGORITHMID_T HashAlgorithmID;            // See HASHALGORITHMID_T
 uint32_t Hash[8];					// Reserve 256 bits for the hash
 uint32_t PartitionNumber;			// This is new for V3.2.0
} IMAGE_INFO_3_2_0, *pIMAGE_INFO_3_2_0;			// 0x40 bytes

typedef struct
{
 uint32_t	KeyID;						// Associate an ID with this key
 HASHALGORITHMID_T HashAlgorithmID;    // See HASHALGORITHMID_T
 uint32_t KeySize;					// Specified in bits
 uint32_t PublicKeySize;				// Specified in bits
 uint32_t RSAPublicExponent[MAXRSAKEYSIZEWORDS]; // Contents depend on PublicKeySize
 uint32_t RSAModulus[MAXRSAKEYSIZEWORDS]; // Up to 2K bits
 uint32_t KeyHash[8]; 				// Reserve 256 bits for the hash
} KEY_MOD_3_2_0, *pKEY_MOD_3_2_0;				// 0x22C bytes

#if RVCT
#pragma anon_unions
#endif
typedef struct
{
 uint32_t KeyID;				// Associate an ID with this key
 HASHALGORITHMID_T HashAlgorithmID;     // See HASHALGORITHMID_T
 uint32_t KeySize;			// Specified in bits
 uint32_t PublicKeySize;		// Specified in bits
 ENCRYPTALGORITHMID_T EncryptAlgorithmID;	// See ENCRYPTALGORITHMID_T;
 union
 {
	struct
	{
 		uint32_t RSAPublicExponent[MAXRSAKEYSIZEWORDS];	// Contents depend on PublicKeySize
		uint32_t RSAModulus[MAXRSAKEYSIZEWORDS];			// Up to 2K bits
	}Rsa;

	struct
	{
        uint32_t PublicKeyCompX[MAXECCKEYSIZEWORDS]; // Contents depend on PublicKeySize
        uint32_t PublicKeyCompY[MAXECCKEYSIZEWORDS]; // Up to 521 bits
        // Pad this struct so it remains consistent with RSA struct
	    uint32_t Reserved[(2*MAXRSAKEYSIZEWORDS)-(2*MAXECCKEYSIZEWORDS)];
	}Ecdsa;
 };

 uint32_t KeyHash[8]; 				// Reserve 256 bits for the hash
} KEY_MOD_3_3_0, *pKEY_MOD_3_3_0;		//

typedef struct
{
 uint32_t KeyID;				// Associate an ID with this key
 HASHALGORITHMID_T HashAlgorithmID;    // See HASHALGORITHMID_T
 uint32_t KeySize;			// Specified in bits
 uint32_t PublicKeySize;		// Specified in bits
 ENCRYPTALGORITHMID_T EncryptAlgorithmID; // See ENCRYPTALGORITHMID_T;
 union
 {
	struct
	{
 		uint32_t RSAPublicExponent[MAXRSAKEYSIZEWORDS];	// Contents depend on PublicKeySize
		uint32_t RSAModulus[MAXRSAKEYSIZEWORDS];			// Up to 2K bits
	}Rsa;

	struct
	{
        uint32_t PublicKeyCompX[MAXECCKEYSIZEWORDS]; // Contents depend on PublicKeySize
        uint32_t PublicKeyCompY[MAXECCKEYSIZEWORDS]; // Up to 521 bits
        // Pad this struct so it remains consistent with RSA struct
	    uint32_t Reserved[(2*MAXRSAKEYSIZEWORDS)-(2*MAXECCKEYSIZEWORDS)];
	}Ecdsa;
    
    struct
    {
        uint32_t EncryptedHashRSAPublicExponent[MAXRSAKEYSIZEWORDS];  // Contents depend on PublicKeySize
        uint32_t EncryptedHashRSAModulus[MAXRSAKEYSIZEWORDS];         // Up to 2K bits
    }EncryptedRsa;

    struct
    {
        uint32_t EncryptedHashPublicKeyCompX_R[MAXECCKEYSIZEWORDS]; // Contents depend on PublicKeySize
        uint32_t EncryptedHashPublicKeyCompX_S[MAXECCKEYSIZEWORDS]; // Contents depend on PublicKeySize

        uint32_t EncryptedHashPublicKeyCompY_R[MAXECCKEYSIZEWORDS]; // Up to 521 bits
        uint32_t EncryptedHashPublicKeyCompY_S[MAXECCKEYSIZEWORDS]; // Up to 521 bits

        // Pad this struct so it remains consistent with encrypted RSA struct
        uint32_t Reserved[(2 * MAXRSAKEYSIZEWORDS)-(4 * MAXECCKEYSIZEWORDS)];
    }EncryptedEcdsa;
 };

 uint32_t KeyHash[16]; 				// Reserve 512 bits for the hash
} KEY_MOD_3_4_0, *pKEY_MOD_3_4_0;		//


typedef struct
{
 ENCRYPTALGORITHMID_T DSAlgorithmID;   // See ENCRYPTALGORITHMID_T
 HASHALGORITHMID_T HashAlgorithmID; // See HASHALGORITHMID_T
 uint32_t KeySize;	          // Specified in bits
 uint32_t Hash[8];			  // Reserve 256 bits for optional key hash
 union 	// Note that this union should not be included as part of the hash for TIM in the Digital Signature
 {
	struct
	{
		uint32_t RSAPublicExponent[MAXRSAKEYSIZEWORDS];
		uint32_t RSAModulus[MAXRSAKEYSIZEWORDS];           	// Up to 2K bits
		uint32_t RSADigS[MAXRSAKEYSIZEWORDS];				// Contains TIM Hash
	}Rsa;

	struct
	{
		uint32_t ECDSAPublicKeyCompX[MAXECCKEYSIZEWORDS]; // Allow for 544 bits (17 words, 68 bytes for use with EC-521)
		uint32_t ECDSAPublicKeyCompY[MAXECCKEYSIZEWORDS];
		uint32_t ECDSADigS_R[MAXECCKEYSIZEWORDS];
		uint32_t ECDSADigS_S[MAXECCKEYSIZEWORDS];
		// Pad this struct so it remains consistent with RSA struct
		uint32_t Reserved[(MAXRSAKEYSIZEWORDS*3)-(MAXECCKEYSIZEWORDS*4)];
	} Ecdsa;
 };
} PLAT_DS, *pPLAT_DS;

// Constant part of the TIMs
typedef struct
{
 VERSION_I      VersionBind;         			// 0
 FLASH_I        FlashInfo;           			// 0x10
 uint32_t         NumImages;           			// 0x20
 uint32_t         NumKeys;						// 0x24
 uint32_t         SizeOfReserved;					// 0x28
} CTIM, *pCTIM;									// 0x2C

// TIM structure for use by DKB/OBM/BootROM
typedef struct
{
 pCTIM				pConsTIM;			// Constant part
 pIMAGE_INFO_3_5_0	pImg;				// Pointer to Images
 pKEY_MOD_3_4_0		pKey;				// Pointer to Keys
 uint32_t *				pReserved;			// Pointer to Reserved Area
 pPLAT_DS       	pTBTIM_DS;			// Pointer to Digital Signature
} TIM, *pTIM;

// NTIM structure for use by DKB/OBM/BootROM
typedef struct
{
 pCTIM				pConsTIM;			// Constant part
 pIMAGE_INFO_3_5_0	pImg;				// Pointer to Images
 uint32_t *				pReserved;			// Pointer to Reserved Area
} NTIM, *pNTIM;


/* tim.c Function Proto's */
uint32_t SetTIMPointers( uint8_t * StartAddr, TIM *pTIM_h);
uint32_t LoadTim(uint8_t *TIMArea, TIM *pTIM_h, uint32_t SRAMLoad);
pIMAGE_INFO_3_4_0 ReturnPImgPtr(pTIM pTIM_h, uint32_t ImageNumber);
uint32_t ReturnImgPartitionNumber(pTIM pTIM_h, pIMAGE_INFO_3_4_0 pImg);
uint32_t GetTIMValidationStatus();
void SetTIMValidationStatus(uint32_t status);
pIMAGE_INFO_3_4_0 FindImageInTIM(pTIM, uint32_t);
pKEY_MOD_3_4_0 FindKeyInTIM(pTIM pTIM_h, uint32_t ImageKeyID);
pWTP_RESERVED_AREA_HEADER FindPackageInReserved (uint32_t * Retval, pTIM pTIM_h, uint32_t Identifier);
// These 2 functions allow for multiple instances of one "TYPE" of
// package to be retrieved from the reserved area of the TIM
//-----------------------------------------------------------------------------------------------------
pWTP_RESERVED_AREA_HEADER FindFirstPackageTypeInReserved (uint32_t * Retval, pTIM pTIM_h, uint32_t Identifier);
pWTP_RESERVED_AREA_HEADER FindNextPackageTypeInReserved (uint32_t * Retval);
pCIDP_ENTRY FindMyConsumerArray(pTIM pTIM_h, uint32_t CID);
uint32_t CheckReserved (pTIM pTIM_h);
// Returns a pointer to the static tim in tim.c
pTIM GetTimPointer();
uint32_t get_image_number(TIM *pTIM_h);
#endif
