/*
 * arch/arm/plat-pxa/include/plat/mfp.h
 *
 *   Common Multi-Function Pin Definitions
 *
 * Copyright (C) 2007 Marvell International Ltd.
 *
 * 2007-8-21: eric miao <eric.miao@marvell.com>
 *            initial version
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef __ASM_PLAT_MFP_H
#define __ASM_PLAT_MFP_H

#define mfp_to_gpio(m)	((m) % 256)
#define __iomem   
#define __init  

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ARRAY_AND_SIZE(x)	(x), ARRAY_SIZE(x)

/* list of all the configurable MFP pins */
enum {
	MFP_PIN_INVALID = -1,

	MFP_PIN_GPIO0 = 0,
	MFP_PIN_GPIO1,
	MFP_PIN_GPIO2,
	MFP_PIN_GPIO3,
	MFP_PIN_GPIO4,
	MFP_PIN_GPIO5,
	MFP_PIN_GPIO6,
	MFP_PIN_GPIO7,
	MFP_PIN_GPIO8,
	MFP_PIN_GPIO9,
	MFP_PIN_GPIO10,
	MFP_PIN_GPIO11,
	MFP_PIN_GPIO12,
	MFP_PIN_GPIO13,
	MFP_PIN_GPIO14,
	MFP_PIN_GPIO15,
	MFP_PIN_GPIO16,
	MFP_PIN_GPIO17,
	MFP_PIN_GPIO18,
	MFP_PIN_GPIO19,
	MFP_PIN_GPIO20,
	MFP_PIN_GPIO21,
	MFP_PIN_GPIO22,
	MFP_PIN_GPIO23,
	MFP_PIN_GPIO24,
	MFP_PIN_GPIO25,
	MFP_PIN_GPIO26,
	MFP_PIN_GPIO27,
	MFP_PIN_GPIO28,
	MFP_PIN_GPIO29,
	MFP_PIN_GPIO30,
	MFP_PIN_GPIO31,
	MFP_PIN_GPIO32,
	MFP_PIN_GPIO33,
	MFP_PIN_GPIO34,
	MFP_PIN_GPIO35,
	MFP_PIN_GPIO36,
	MFP_PIN_GPIO37,
	MFP_PIN_GPIO38,
	MFP_PIN_GPIO39,
	MFP_PIN_GPIO40,
	MFP_PIN_GPIO41,
	MFP_PIN_GPIO42,
	MFP_PIN_GPIO43,
	MFP_PIN_GPIO44,
	MFP_PIN_GPIO45,
	MFP_PIN_GPIO46,
	MFP_PIN_GPIO47,
	MFP_PIN_GPIO48,
	MFP_PIN_GPIO49,
	MFP_PIN_GPIO50,
	MFP_PIN_GPIO51,
	MFP_PIN_GPIO52,
	MFP_PIN_GPIO53,
	MFP_PIN_GPIO54,
	MFP_PIN_GPIO55,
	MFP_PIN_GPIO56,
	MFP_PIN_GPIO57,
	MFP_PIN_GPIO58,
	MFP_PIN_GPIO59,
	MFP_PIN_GPIO60,
	MFP_PIN_GPIO61,
	MFP_PIN_GPIO62,
	MFP_PIN_GPIO63,
	MFP_PIN_GPIO64,
	MFP_PIN_GPIO65,
	MFP_PIN_GPIO66,
	MFP_PIN_GPIO67,
	MFP_PIN_GPIO68,
	MFP_PIN_GPIO69,
	MFP_PIN_GPIO70,
	MFP_PIN_GPIO71,
	MFP_PIN_GPIO72,
	MFP_PIN_GPIO73,
	MFP_PIN_GPIO74,
	MFP_PIN_GPIO75,
	MFP_PIN_GPIO76,
	MFP_PIN_GPIO77,
	MFP_PIN_GPIO78,
	MFP_PIN_GPIO79,
	MFP_PIN_GPIO80,
	MFP_PIN_GPIO81,
	MFP_PIN_GPIO82,
	MFP_PIN_GPIO83,
	MFP_PIN_GPIO84,
	MFP_PIN_GPIO85,
	MFP_PIN_GPIO86,
	MFP_PIN_GPIO87,
	MFP_PIN_GPIO88,
	MFP_PIN_GPIO89,
	MFP_PIN_GPIO90,
	MFP_PIN_GPIO91,
	MFP_PIN_GPIO92,
	MFP_PIN_GPIO93,
	MFP_PIN_GPIO94,
	MFP_PIN_GPIO95,
	MFP_PIN_GPIO96,
	MFP_PIN_GPIO97,
	MFP_PIN_GPIO98,
	MFP_PIN_GPIO99,
	MFP_PIN_GPIO100,
	MFP_PIN_GPIO101,
	MFP_PIN_GPIO102,
	MFP_PIN_GPIO103,
	MFP_PIN_GPIO104,
	MFP_PIN_GPIO105,
	MFP_PIN_GPIO106,
	MFP_PIN_GPIO107,
	MFP_PIN_GPIO108,
	MFP_PIN_GPIO109,
	MFP_PIN_GPIO110,
	MFP_PIN_GPIO111,
	MFP_PIN_GPIO112,
	MFP_PIN_GPIO113,
	MFP_PIN_GPIO114,
	MFP_PIN_GPIO115,
	MFP_PIN_GPIO116,
	MFP_PIN_GPIO117,
	MFP_PIN_GPIO118,
	MFP_PIN_GPIO119,
	MFP_PIN_GPIO120,
	MFP_PIN_GPIO121,
	MFP_PIN_GPIO122,
	MFP_PIN_GPIO123,
	MFP_PIN_GPIO124,
	MFP_PIN_GPIO125,
	MFP_PIN_GPIO126,
	MFP_PIN_GPIO127,

	/* additional pins on MMP3 */
	MFP_PIN_ANT_SW4,
	MFP_PIN_SM_ADV,
	MFP_PIN_ND_RDY1,
	MFP_PIN_SM_ADVMUX,	
	MFP_PIN_SM_BE1,
	MFP_PIN_SM_CS0,
	MFP_PIN_SM_CS1,
	MFP_PIN_ND_CS1,

	MFP_PIN_nXCVREN,
	MFP_PIN_DF_CLE_nOE,
	MFP_PIN_DF_nADV1_ALE,
	MFP_PIN_DF_SCLK_E,
	MFP_PIN_DF_SCLK_S,
	MFP_PIN_nBE0,
	MFP_PIN_nBE1,
	MFP_PIN_DF_nADV2_ALE,
	MFP_PIN_DF_INT_RnB,
	MFP_PIN_DF_nCS0,
	MFP_PIN_DF_nCS1,
	MFP_PIN_nLUA,
	MFP_PIN_nLLA,
	MFP_PIN_DF_nWE,
	MFP_PIN_DF_ALE_nWE,
	MFP_PIN_DF_nRE_nOE,
	MFP_PIN_DF_ADDR0,
	MFP_PIN_DF_ADDR1,
	MFP_PIN_DF_ADDR2,
	MFP_PIN_DF_ADDR3,
	MFP_PIN_DF_IO0,
	MFP_PIN_DF_IO1,
	MFP_PIN_DF_IO2,
	MFP_PIN_DF_IO3,
	MFP_PIN_DF_IO4,
	MFP_PIN_DF_IO5,
	MFP_PIN_DF_IO6,
	MFP_PIN_DF_IO7,
	MFP_PIN_DF_IO8,
	MFP_PIN_DF_IO9,
	MFP_PIN_DF_IO10,
	MFP_PIN_DF_IO11,
	MFP_PIN_DF_IO12,
	MFP_PIN_DF_IO13,
	MFP_PIN_DF_IO14,
	MFP_PIN_DF_IO15,
	MFP_PIN_DF_nCS0_SM_nCS2,
	MFP_PIN_DF_nCS1_SM_nCS3,
	MFP_PIN_SM_nCS0,
	MFP_PIN_SM_nCS1,
	MFP_PIN_DF_WEn,
	MFP_PIN_DF_REn,
	MFP_PIN_DF_CLE_SM_OEn,
	MFP_PIN_DF_ALE_SM_WEn,
	MFP_PIN_DF_RDY0,
	MFP_PIN_DF_RDY1,

	MFP_PIN_SM_SCLK,
	MFP_PIN_SM_BE0,
	MFP_PIN_SM_RDY,

	MFP_PIN_MMC1_DAT7,
	MFP_PIN_MMC1_DAT6,
	MFP_PIN_MMC1_DAT5,
	MFP_PIN_MMC1_DAT4,
	MFP_PIN_MMC1_DAT3,
	MFP_PIN_MMC1_DAT2,
	MFP_PIN_MMC1_DAT1,
	MFP_PIN_MMC1_DAT0,
	MFP_PIN_MMC1_CMD,
	MFP_PIN_MMC1_CLK,
	MFP_PIN_MMC1_CD,
	MFP_PIN_MMC1_WP,

	/* additional pins on PXA910 */
	MFP_PIN_VCXOREQ,

	/* additional pins on PXA930 */
	MFP_PIN_GSIM_UIO,
	MFP_PIN_GSIM_UCLK,
	MFP_PIN_GSIM_UDET,
	MFP_PIN_GSIM_nURST,
	MFP_PIN_PMIC_INT,
	MFP_PIN_RDY,

	/* additional pins on MMP2 */
	MFP_PIN_TWSI1_SCL,
	MFP_PIN_TWSI1_SDA,
	MFP_PIN_TWSI4_SCL,
	MFP_PIN_TWSI4_SDA,
	MFP_PIN_CLK_REQ,

	MFP_PIN_PWM4,
	MFP_PIN_USIM_UCLK,      
	MFP_PIN_USIM_UIO,       
	MFP_PIN_USIM_nURST,    
    	MFP_PIN_TDS_DIO0               ,
    	MFP_PIN_TDS_DIO1               ,
    	MFP_PIN_TDS_DIO2               ,
    	MFP_PIN_TDS_DIO3               ,
    	MFP_PIN_TDS_DIO4               ,
    	MFP_PIN_TDS_DIO5               ,
    	MFP_PIN_TDS_DIO6               ,
    	MFP_PIN_TDS_DIO7               ,
    	MFP_PIN_TDS_CLK                ,
    	MFP_PIN_TDS_DIO8               ,
   	MFP_PIN_TDS_DIO9               ,    
    	MFP_PIN_TDS_PAON               ,
    	MFP_PIN_TDS_PACTRL             ,
    	MFP_PIN_TDS_RXON               ,
    	MFP_PIN_TDS_TXON               ,
    	MFP_PIN_TDS_LNACTRL            ,
    	MFP_PIN_TDS_MIXCTRL            ,
    	MFP_PIN_TDS_TRXSW              ,
   	MFP_PIN_TDS_RXREV              ,
   	MFP_PIN_TDS_TXREV              ,
	MFP_PIN_RF_CONT_4              ,
	MFP_PIN_PA_MODE                ,
	MFP_PIN_MAX,
};

/*
 * a possible MFP configuration is represented by a 32-bit integer
 *
 * bit  0.. 9 - MFP Pin Number (1024 Pins Maximum)
 * bit 10..12 - Alternate Function Selection
 * bit 13..15 - Drive Strength
 * bit 16..18 - Low Power Mode State
 * bit 19..20 - Low Power Mode Edge Detection
 * bit 21..23 - Run Mode Pull State
 * bit 24..26 - Sleep state --added for sleep control
 *
 * to facilitate the definition, the following macros are provided
 *
 * MFP_CFG_DEFAULT - default MFP configuration value, with
 * 		  alternate function = 0,
 * 		  drive strength = fast 3mA (MFP_DS03X)
 * 		  low power mode = default
 * 		  edge detection = none
 *
 * MFP_CFG	- default MFPR value with alternate function
 * MFP_CFG_DRV	- default MFPR value with alternate function and
 * 		  pin drive strength
 * MFP_CFG_LPM	- default MFPR value with alternate function and
 * 		  low power mode
 * MFP_CFG_X	- default MFPR value with alternate function,
 * 		  pin drive strength and low power mode
 */

typedef unsigned long mfp_cfg_t;

#define MFP_PIN(x)		((x) & 0x3ff)

#define MFP_AF0			(0x0 << 10)
#define MFP_AF1			(0x1 << 10)
#define MFP_AF2			(0x2 << 10)
#define MFP_AF3			(0x3 << 10)
#define MFP_AF4			(0x4 << 10)
#define MFP_AF5			(0x5 << 10)
#define MFP_AF6			(0x6 << 10)
#define MFP_AF7			(0x7 << 10)
#define MFP_AF_MASK		(0x7 << 10)
#define MFP_AF(x)		(((x) >> 10) & 0x7)

#define MFP_DS01X		(0x0 << 13)
#define MFP_DS02X		(0x1 << 13)
#define MFP_DS03X		(0x2 << 13)
#define MFP_DS04X		(0x3 << 13)
#define MFP_DS06X		(0x4 << 13)
#define MFP_DS08X		(0x5 << 13)
#define MFP_DS10X		(0x6 << 13)
#define MFP_DS13X		(0x7 << 13)
#define MFP_DS_MASK		(0x7 << 13)
#define MFP_DS(x)		(((x) >> 13) & 0x7)
#define MFP_VERY_SLOW		MFP_DS01X
#define MFP_SLOW		MFP_DS02X
#define MFP_MEDIUM		MFP_DS03X
#define MFP_FAST		MFP_DS04X

#define MFP_LPM_DEFAULT		(0x0 << 16)
#define MFP_LPM_FLOAT		(0x1 << 16)
#define MFP_LPM_INPUT		(0x2 << 16)
#define MFP_LPM_DRIVE_LOW	(0x3 << 16)
#define MFP_LPM_DRIVE_HIGH	(0x4 << 16)
//#define MFP_LPM_PULL_LOW	(0x3 << 16)
//#define MFP_LPM_PULL_HIGH	(0x4 << 16)
#define MFP_LPM_STATE_MASK	(0x7 << 16)
#define MFP_LPM_STATE(x)	(((x) >> 16) & 0x7)

#define MFP_LPM_EDGE_NONE	(0x0 << 19)
#define MFP_LPM_EDGE_RISE	(0x1 << 19)
#define MFP_LPM_EDGE_FALL	(0x2 << 19)
#define MFP_LPM_EDGE_BOTH	(0x3 << 19)
#define MFP_LPM_EDGE_MASK	(0x3 << 19)
#define MFP_LPM_EDGE(x)		(((x) >> 19) & 0x3)

#define MFP_PULL_NONE		(0x0 << 21)
#define MFP_PULL_LOW		(0x1 << 21)
#define MFP_PULL_HIGH		(0x2 << 21)
#define MFP_PULL_BOTH		(0x3 << 21)
#define MFP_PULL_FLOAT		(0x4 << 21)
#define MFP_PULL_MASK		(0x7 << 21)
#define MFP_PULL(x)		(((x) >> 21) & 0x7)

/*sleep state*/

#define MFP_SLEEP_DEFAULT	(0x2 << 24)
#define MFP_SLEEP_NOSLEEP	(0x1 << 24)
#define MFP_SLEEP_INPUT		(0x2 << 24)
#define MFP_SLEEP_DRIVE_HIGH	(0x3 << 24)
#define MFP_SLEEP_DRIVE_LOW	(0x4 << 24)

#define MFP_SLEEP_MASK		(0x7 << 24)
#define MFP_SLEEP(x)		(((x) >> 24) & 0x7)

#define MFP_DRIVE_VERY_SLOW	(0x0 << 13)
#define MFP_DRIVE_SLOW		(0x1 << 13)
#define MFP_DRIVE_MEDIUM	(0x2 << 13)
#define MFP_DRIVE_FAST		(0x3 << 13)


#define MFP_CFG(pin, af)		\
	(MFP_LPM_DEFAULT | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_MEDIUM)

#define MFP_CFG_SLEEP(pin, af, sleep)		\
	(MFP_LPM_DEFAULT | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_MEDIUM | MFP_##sleep)
	
#define MFP_CFG_DRV(pin, af, drv)	\
	(MFP_LPM_DEFAULT | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_##drv)
	
#define MFP_CFG_DRV_SLEEP(pin, af, drv,sleep)	\
	(MFP_LPM_DEFAULT | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_##drv | MFP_##sleep)
	
#define MFP_CFG_PULL(pin, af, pull)	\
	(MFP_LPM_DEFAULT | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_MEDIUM | MFP_##pull)

#define MFP_CFG_PULL_SLEEP(pin, af, pull, sleep)	\
	(MFP_LPM_DEFAULT | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_MEDIUM | MFP_##pull | MFP_##sleep)

#define MFP_CFG_DRV_PULL(pin, af, drv, pull)	\
	(MFP_LPM_DEFAULT | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_##drv | MFP_##pull)

#define MFP_CFG_PULL_HIGH(pin, af)	\
   (MFP_PULL_HIGH | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_MEDIUM)

#define MFP_CFG_PULL_LOW(pin, af)	\
   (MFP_PULL_LOW | MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_MEDIUM)

#define MFP_CFG_DRV_PULL_SLEEP(pin, af, drv, pull,sleep)	\
	(MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_DRIVE_##drv | MFP_##pull|MFP_##sleep)

/*#if defined(CONFIG_PXA3xx) || defined(CONFIG_PXA93x)	\
	|| defined(CONFIG_PXA95x) || defined(CONFIG_ARCH_MMP)*/
/*
 * each MFP pin will have a MFPR register, since the offset of the
 * register varies between processors, the processor specific code
 * should initialize the pin offsets by mfp_init()
 *
 * mfp_init_base() - accepts a virtual base for all MFPR registers and
 * initialize the MFP table to a default state
 *
 * mfp_init_addr() - accepts a table of "mfp_addr_map" structure, which
 * represents a range of MFP pins from "start" to "end", with the offset
 * begining at "offset", to define a single pin, let "end" = -1.
 *
 * use
 *
 * MFP_ADDR_X() to define a range of pins
 * MFP_ADDR()   to define a single pin
 * MFP_ADDR_END to signal the end of pin offset definitions
 */
struct mfp_addr_map {
	unsigned int	start;
	unsigned int	end;
	unsigned long	offset;
};

#define MFP_ADDR_X(start, end, offset) \
	{ MFP_PIN_##start, MFP_PIN_##end, offset }

#define MFP_ADDR(pin, offset) \
	{ MFP_PIN_##pin, -1, offset }

#define MFP_ADDR_END	{ MFP_PIN_INVALID, 0, 0 }

void __init mfp_init_base(unsigned long mfpr_base);
void __init mfp_init_addr(struct mfp_addr_map *map);

/*
 * mfp_{read, write}()	- for direct read/write access to the MFPR register
 * mfp_config()		- for configuring a group of MFPR registers
 * mfp_config_lpm()	- configuring all low power MFPR registers for suspend
 * mfp_config_run()	- configuring all run time  MFPR registers after resume
 */
unsigned long mfp_read(int mfp);
void mfp_write(int mfp, unsigned long mfpr_val);
void mfp_config(unsigned long *mfp_cfgs, int num);
void mfp_config_run(void);
void mfp_config_lpm(void);
void __init mfp_init_addr(struct mfp_addr_map *map);
void __init mfp_init_base(unsigned long mfpr_base);
void mfp_clr(int mfp, unsigned long mask);
void mfp_set(int mfp, unsigned long mask);
//#endif /* CONFIG_PXA3xx || CONFIG_PXA93x || CONFIG_PXA95x || CONFIG_ARCH_MMP */

void lpm_mfpr_edge_config(int mfp, unsigned long val);
#define mfpr_edge_config(__mfp, __val) lpm_mfpr_edge_config(__mfp, __val)

void mfpr_sleep_config(int mfp, unsigned long val);
void lpm_mfpr_state_config(int mfp, unsigned long val);
     	
#define mfpr_sleep_and_state_config(__mfp_pin,__state_val) \
do{  \
     if(__state_val ==MFP_LPM_DRIVE_LOW || \
     	__state_val ==MFP_LPM_DRIVE_HIGH) {\
     	mfpr_sleep_config(__mfp_pin,MFP_SLEEP_SLEEP);  \
     	lpm_mfpr_state_config(__mfp_pin, __state_val); }else{\
     	mfpr_sleep_config(__mfp_pin,MFP_SLEEP_SLEEP);  \
     	lpm_mfpr_state_config(__mfp_pin, MFP_LPM_FLOAT); \
     	} \
}while(0) 
	
void mfpr_pull_config(int mfp, unsigned long val);
void mfpr_drive_strength_config(int mfp, unsigned long val);

#define mfpr_pull_drive_config(__mfp_pin, __pull_val, __drive_val) \
do {  \
    mfpr_pull_config(__mfp_pin, __pull_val);  \
    mfpr_drive_strength_config(__mfp_pin, __drive_val);  \
}while(0)

#define mfpr_pull_af_config(__mfp_pin, __pull_val, __af_val) \
do {  \
    mfpr_function_config(__mfp_pin, __af_val);  \
    mfpr_pull_config(__mfp_pin, __pull_val);  \
}while(0)


void mfpr_function_config(int mfp, unsigned long val);
void mfpr_config_pin(int mfp, unsigned long val);
uint32_t mfp_config_to_mfpr(unsigned long mfp_config);
void mfp_config_lpm_array(unsigned long *lpm_mfp_cfgs, int num); /*config an array of lpm cfgs*/
#endif /* __ASM_PLAT_MFP_H */
