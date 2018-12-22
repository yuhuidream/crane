/*
 * linux/arch/arm/plat-pxa/mfp.c
 *
 *   Multi-Function Pin Support
 *
 * Copyright (C) 2007 Marvell Internation Ltd.
 *
 * 2007-08-21: eric miao <eric.miao@marvell.com>
 *             initial version
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */
#include "predefines.h"
#include "mfp.h"

#define CONFIG_ARCH_PXA		1

#define REG_READ(x) (*(volatile unsigned int *)(x))
#define REG_WRITE(y,x) ((*(volatile unsigned int *)(x)) = y)

//#define MFPR_SIZE	(PAGE_SIZE)

/* MFPR register bit definitions */
#define MFPR_PULL_SEL		(0x1 << 15)
#define MFPR_PULLUP_EN		(0x1 << 14)
#define MFPR_PULLDOWN_EN	(0x1 << 13)
#define MFPR_SLEEP_SEL		(0x1 << 9)
#define MFPR_SLEEP_SEL_BIT3     (0x1 << 3)
#define MFPR_SLEEP_OE_N		(0x1 << 7)
#define MFPR_EDGE_CLEAR		(0x1 << 6)
#define MFPR_EDGE_FALL_EN	(0x1 << 5)
#define MFPR_EDGE_RISE_EN	(0x1 << 4)

#define MFPR_SLEEP_DATA(x)	(((x) & 0x1) << 8)
#define MFPR_DRIVE(x)		(((x) & 0x7) << 10)
#define MFPR_AF_SEL(x)		(((x) & 0x7) << 0)

#define MFPR_EDGE_NONE		(MFPR_EDGE_CLEAR)
#define MFPR_EDGE_RISE		(MFPR_EDGE_RISE_EN)
#define MFPR_EDGE_FALL		(MFPR_EDGE_FALL_EN)
#define MFPR_EDGE_BOTH		(MFPR_EDGE_RISE | MFPR_EDGE_FALL)

/*
 * Table that determines the low power modes outputs, with actual settings
 * used in parentheses for don't-care values. Except for the float output,
 * the configured driven and pulled levels match, so if there is a need for
 * non-LPM pulled output, the same configuration could probably be used.
 *
 * Output value  sleep_oe_n  sleep_data  pullup_en  pulldown_en  pull_sel
 *                 (bit 7)    (bit 8)    (bit 14)     (bit 13)   (bit 15)
 *
 * Drive 0          0          0           0           X(1)       0
 * Drive 1          0          1           X(1)        0	  0
 * Pull hi (1)      1          X(1)        1           0	  0
 * Pull lo (0)      1          X(0)        0           1	  0
 * Z (float)        1          X(0)        0           0	  0
 */
#define MFPR_LPM_DRIVE_LOW	(MFPR_SLEEP_DATA(0))
#define MFPR_LPM_DRIVE_HIGH    	(MFPR_SLEEP_DATA(1))
//#define MFPR_LPM_DRIVE_LOW	(MFPR_SLEEP_DATA(0) | MFPR_PULLDOWN_EN|MFPR_PULL_SEL)
//#define MFPR_LPM_DRIVE_HIGH    	(MFPR_SLEEP_DATA(1) | MFPR_PULLUP_EN|MFPR_PULL_SEL)

#define MFPR_LPM_PULL_LOW      	((0x1<<3)|(0x1<<9)|MFPR_SLEEP_DATA(0))
#define MFPR_LPM_PULL_HIGH     	((0x1<<3)|(0x1<<9)|MFPR_SLEEP_DATA(1))
#define MFPR_LPM_FLOAT         	((0x1<<3)|(0x1<<9)|MFPR_SLEEP_OE_N)  /*input z*/
#define MFPR_LPM_DEFAULT        ((0x0<<3)|(0x0<<9))  		   /* no sleep */
#define MFPR_LPM_INPUT        	MFPR_LPM_FLOAT     /*input z*/
#define MFPR_LPM_MASK		(0xe080)

/*
 * The pullup and pulldown state of the MFP pin at run mode is by default
 * determined by the selected alternate function. In case that some buggy
 * devices need to override this default behavior,  the definitions below
 * indicates the setting of corresponding MFPR bits
 *
 * Definition       pull_sel  pullup_en  pulldown_en
 * MFPR_PULL_NONE       0         0        0
 * MFPR_PULL_LOW        1         0        1
 * MFPR_PULL_HIGH       1         1        0
 * MFPR_PULL_BOTH       1         1        1
 * MFPR_PULL_FLOAT	1         0        0
 */
#define MFPR_PULL_NONE		(0)
#define MFPR_PULL_LOW		(MFPR_PULL_SEL | MFPR_PULLDOWN_EN)
#define MFPR_PULL_BOTH		(MFPR_PULL_LOW | MFPR_PULLUP_EN)
#define MFPR_PULL_HIGH		(MFPR_PULL_SEL | MFPR_PULLUP_EN)
#define MFPR_PULL_FLOAT		(MFPR_PULL_SEL)

/*added for sleep control*/
#define MFPR_SLEEP_DEFAULT	((0x0<<3)|(0x0<<9))
#define MFPR_SLEEP_NOSLEEP 	((0x0<<3)|(0x0<<9))
#define MFPR_SLEEP_INPUT        ((0x1<<3)|(0x1<<9)|(0x1<<7))
#define MFPR_SLEEP_DRIVE_HIGH   ((0x1<<3)|(0x1<<9)|(0x1<<8))
#define MFPR_SLEEP_DRIVE_LOW    ((0x1<<3)|(0x1<<9)|(0x0<<8))

//#define MFPR_SLEEP_OUT_HIGH	(MFPR_SLEEP_DATA(1))
//#define MFPR_SLEEP_OUT_LOW	(MFPR_SLEEP_DATA(0))
//#define MFPR_SLEEP_IN		(MFPR_SLEEP_OE_N)


/* mfp_spin_lock is used to ensure that MFP register configuration
 * (most likely a read-modify-write operation) is atomic, and that
 * mfp_table[] is consistent
 */
//static DEFINE_SPINLOCK(mfp_spin_lock);

#define MFPR_DRIVE_VERY_SLOW	(0x0<<11)
#define MFPR_DRIVE_SLOW		(0x1<<11)
#define MFPR_DRIVE_MEDIUM	(0x2<<11)
#define MFPR_DRIVE_FAST		(0x3<<11)
#define MFPR_DRIVE_MASK         (0x3<<11)

#define MFPR_AF_MASK (0x7<<0)

static void __iomem  *mfpr_mmio_base;


struct mfp_pin {
	unsigned long	config;		/* -1 for not configured */
	unsigned long	mfpr_off;	/* MFPRxx Register offset */
	unsigned long	mfpr_run;	/* Run-Mode Register Value */
	unsigned long	mfpr_lpm;	/* Low Power Mode Register Value */
};

static struct mfp_pin mfp_table[MFP_PIN_MAX];

/* mapping of MFP_LPM_* definitions to MFPR_LPM_* register bits */
static const unsigned long mfpr_lpm[] = {
	MFPR_LPM_DEFAULT,	
	MFPR_LPM_FLOAT	,
	MFPR_LPM_INPUT	,
	MFPR_LPM_DRIVE_LOW,	
	MFPR_LPM_DRIVE_HIGH,	
};

/* mapping of MFP_PULL_* definitions to MFPR_PULL_* register bits */
static const unsigned long mfpr_pull[] = {
	MFPR_PULL_NONE,
	MFPR_PULL_LOW,
	MFPR_PULL_HIGH,
	MFPR_PULL_BOTH,
	MFPR_PULL_FLOAT,
};

/* mapping of MFP_LPM_EDGE_* definitions to MFPR_EDGE_* register bits */
static const unsigned long mfpr_edge[] = {
	MFPR_EDGE_NONE,
	MFPR_EDGE_RISE,
	MFPR_EDGE_FALL,
	MFPR_EDGE_BOTH,
};

/* mapping of MFP_SLEEP_* definitions to MFPR_SLEEP_* register bits */
static const unsigned long mfpr_sleep[] = {
 	MFPR_SLEEP_DEFAULT,
 	MFPR_SLEEP_NOSLEEP,
	MFPR_SLEEP_INPUT,
 	MFPR_SLEEP_DRIVE_HIGH,
	MFPR_SLEEP_DRIVE_LOW,
};

/* mapping of MFP_* definitions to MFPR_DRIVE_* register bits */
static const unsigned long mfpr_ds[] = {
	MFPR_DRIVE_VERY_SLOW,
	MFPR_DRIVE_SLOW,
	MFPR_DRIVE_MEDIUM,
	MFPR_DRIVE_FAST,
};
#define mfpr_readl(off)			\
	REG_READ(mfpr_mmio_base + (off))

#define mfpr_writel(off, val)		\
	REG_WRITE(val, mfpr_mmio_base + (off))

#define mfp_configured(p)	((long)((p)->config) != -1)

/*
 * perform a read-back of any valid MFPR register to make sure the
 * previous writings are finished
 */
static unsigned long mfpr_off_readback;
#define mfpr_sync()	(void)REG_READ(mfpr_mmio_base + mfpr_off_readback)

static inline void __mfp_config_run(struct mfp_pin *p)
{
	if (mfp_configured(p))
		//mfpr_writel(p->mfpr_off, p->mfpr_run);
		((*(volatile unsigned int *)(mfpr_mmio_base+p->mfpr_off)) = p->mfpr_run);
}

static inline void __mfp_config_lpm(struct mfp_pin *p)
{
	if (mfp_configured(p)) {
		unsigned long mfpr_clr = (p->mfpr_run & ~MFPR_EDGE_BOTH) | MFPR_EDGE_CLEAR;
		if (mfpr_clr != p->mfpr_run)
			mfpr_writel(p->mfpr_off, mfpr_clr);
		if (p->mfpr_lpm != mfpr_clr)
			mfpr_writel(p->mfpr_off, p->mfpr_lpm);
	}
}

void mfp_config(unsigned long *mfp_cfgs, int num)
{
	int i, drv_b11 = 0, no_lpm = 0;

	/* mmp plat is different with mg1-mg2-nevo plat.*/
#ifdef CONFIG_ARCH_MMP
	//if (cpu_is_pxa910() || cpu_is_mmp2() || cpu_is_mmp3())
		drv_b11 = 1;
	//if (cpu_is_pxa168() || cpu_is_pxa910())
		//no_lpm = 1;
#elif defined(CONFIG_ARCH_PXA)
		drv_b11 = 1;
#endif

	for (i = 0; i < num; i++, mfp_cfgs++) {
		unsigned long tmp, c = *mfp_cfgs;
		struct mfp_pin *p;
		int pin, af, drv, lpm, edge, pull,sleep;

		pin = MFP_PIN(c);
		//Q_ASSERT(pin < MFP_PIN_MAX);
		p = &mfp_table[pin];

		af  = MFP_AF(c);
		drv = MFP_DS(c);
		lpm = MFP_LPM_STATE(c);
		edge = MFP_LPM_EDGE(c);
		pull = MFP_PULL(c);
		sleep = MFP_SLEEP(c);

		if (drv_b11)
			drv = drv << 1;
		if (no_lpm)
			lpm = 0;
	
		/* run-mode pull settings will conflict with MFPR bits of
		 * low power mode state,  calculate mfpr_run and mfpr_lpm
		 * individually if pull != MFP_PULL_NONE
		 */
		tmp = MFPR_AF_SEL(af) | MFPR_DRIVE(drv);		

		p->mfpr_run = tmp | mfpr_pull[pull] | mfpr_edge[edge]|mfpr_sleep[sleep];
		p->mfpr_lpm = p->mfpr_run;

		p->config = c; __mfp_config_run(p);
	}

	mfpr_sync();
}

unsigned long mfp_read(int mfp)
{
	unsigned long val;
	//Q_ASSERT(pin < MFP_PIN_MAX);
	val = mfpr_readl(mfp_table[mfp].mfpr_off);

	return val;
}

void mfp_write(int mfp, unsigned long val)
{
	//Q_ASSERT(pin < MFP_PIN_MAX);
	mfpr_writel(mfp_table[mfp].mfpr_off, val);
	mfpr_sync();
}

void mfp_set(int mfp, unsigned long mask)
{
	unsigned long tmp = mfp_read(mfp);

	tmp |= mask;
	mfp_write(mfp, tmp);
}

void mfp_clr(int mfp, unsigned long mask)
{
	unsigned long tmp = mfp_read(mfp);

	tmp &= ~mask;
	mfp_write(mfp, tmp);
}

void __init mfp_init_base(unsigned long mfpr_base)
{
	unsigned int i;

	/* initialize the table with default - unconfigured */
	for (i = 0; i < ARRAY_SIZE(mfp_table); i++)
		mfp_table[i].config = -1;

	mfpr_mmio_base = (void __iomem *)mfpr_base;
}

void __init mfp_init_addr(struct mfp_addr_map *map)
{
	struct mfp_addr_map *p;
	unsigned long offset=0;
	unsigned int i;

	/* mfp offset for readback */
	mfpr_off_readback = map[0].offset;

	for (p = map; (int)(p->start) != MFP_PIN_INVALID; p++) {
		offset = p->offset;
		i = p->start;

		do {
			mfp_table[i].mfpr_off = offset;
			mfp_table[i].mfpr_run = 0;
			mfp_table[i].mfpr_lpm = 0;
			offset += 4; i++;
		} while ((i <= p->end) && ((int)p->end != -1));
	}
}

void mfp_config_lpm(void)
{
	struct mfp_pin *p = &mfp_table[0];
	unsigned int pin;

	for (pin = 0; pin < ARRAY_SIZE(mfp_table); pin++, p++)
		__mfp_config_lpm(p);
}

void mfp_config_run(void)
{
	struct mfp_pin *p = &mfp_table[0];
	unsigned int pin;

	for (pin = 0; pin < ARRAY_SIZE(mfp_table); pin++, p++)
		__mfp_config_run(p);
}

void mfpr_pull_config(int mfp, unsigned long val)
{
	mfp_cfg_t m;
	m = mfp_read(mfp);
	m &= ~(MFPR_PULL_SEL|MFPR_PULLUP_EN|MFPR_PULLDOWN_EN);
	m |= mfpr_pull[MFP_PULL(val)];
	mfp_write(mfp, m);
	m =mfp_read(mfp);
}

void mfpr_sleep_config(int mfp, unsigned long val)
{
	mfp_cfg_t m;
	m = mfp_read(mfp);

	/*clear the old cfg*/
	m &= ~(BIT_3|BIT_9|BIT_7|BIT_8);
	m |= mfpr_sleep[MFP_SLEEP(val)];
	mfp_write(mfp, m);
	//m =mfp_read(mfp);
}

void lpm_mfpr_edge_config(int mfp, unsigned long val)
{
	mfp_cfg_t m;
	m = mfp_read(mfp);
	m &= ~(MFPR_EDGE_CLEAR | MFPR_EDGE_FALL_EN
						 | MFPR_EDGE_RISE_EN);
	m |= mfpr_edge[MFP_LPM_EDGE(val)];
	mfp_write(mfp, m);
	m =mfp_read(mfp);
}

void lpm_mfpr_state_config(int mfp, unsigned long val)
{
	mfp_cfg_t m;
	m = mfp_read(mfp);
	
	//if(val == MFP_LPM_DEFAULT)
	//	return;
		
	/*clear the old cfg*/
	m &= ~(BIT_3|BIT_9|BIT_7|BIT_8);
			
	m |= mfpr_lpm[MFP_LPM_STATE(val)];
	mfp_write(mfp, m);
	//m =mfp_read(mfp);
}

void mfpr_config_pin(int mfp, unsigned long val)
{
	int drv_b11 = 0, no_lpm = 0;
	unsigned long tmp, c = val;
	int pin, af, drv, lpm, edge, pull,sleep;
	uint32_t mfpr_value;
        mfp_cfg_t m;

	/* mmp plat is different with mg1-mg2-nevo plat.*/
#ifdef CONFIG_ARCH_MMP
	//if (cpu_is_pxa910() || cpu_is_mmp2() || cpu_is_mmp3())
		drv_b11 = 1;
	//if (cpu_is_pxa168() || cpu_is_pxa910())
		//no_lpm = 1;
#elif defined(CONFIG_ARCH_PXA)
		drv_b11 = 1;
#endif
	pin = MFP_PIN(c);
	af  = MFP_AF(c);
	drv = MFP_DS(c);
	lpm = MFP_LPM_STATE(c);
	edge = MFP_LPM_EDGE(c);
	pull = MFP_PULL(c);
	sleep = MFP_SLEEP(c);

	if (drv_b11)
		drv = drv << 1;
	if (no_lpm)
		lpm = 0;
	
	tmp = MFPR_AF_SEL(af) | MFPR_DRIVE(drv);
	mfpr_value = tmp | mfpr_pull[pull]| mfpr_edge[edge]|mfpr_sleep[sleep];

	mfp_write(mfp, mfpr_value);
	m = mfp_read(mfp);
}

void mfpr_drive_strength_config(int mfp, unsigned long val)
{
	mfp_cfg_t m;
	m = mfp_read(mfp);

	/*clear the old cfg*/
	m &= ~(MFPR_DRIVE_MASK);
	m |= mfpr_ds[MFP_DS(val)];
	mfp_write(mfp, m);
	m =mfp_read(mfp);
}

void mfpr_function_config(int mfp, unsigned long val)
{
	mfp_cfg_t m;
	m = mfp_read(mfp);

	/*clear the old cfg*/
	m &= ~(MFPR_AF_MASK);
	m |= MFP_AF(val);
	mfp_write(mfp, m);
	m =mfp_read(mfp);
}

/*convert an mfp config value to mfpr format value*/
uint32_t mfp_config_to_mfpr(unsigned long mfp_config)
{
	int drv_b11 = 0, no_lpm = 0;
	unsigned long tmp, c = mfp_config;
	int pin, af, drv, lpm, edge, pull,sleep;
	uint32_t mfpr_value;

	/* mmp plat is different with mg1-mg2-nevo plat.*/
#ifdef CONFIG_ARCH_MMP
	//if (cpu_is_pxa910() || cpu_is_mmp2() || cpu_is_mmp3())
		drv_b11 = 1;
	//if (cpu_is_pxa168() || cpu_is_pxa910())
		//no_lpm = 1;
#elif defined(CONFIG_ARCH_PXA)
		drv_b11 = 1;
#endif
	pin = MFP_PIN(c);
	af  = MFP_AF(c);
	drv = MFP_DS(c);
	lpm = MFP_LPM_STATE(c);
	edge = MFP_LPM_EDGE(c);
	pull = MFP_PULL(c);
	sleep = MFP_SLEEP(c);

	if (drv_b11)
		drv = drv << 1;
	if (no_lpm)
		lpm = 0;

	tmp = MFPR_AF_SEL(af) | MFPR_DRIVE(drv);
	mfpr_value = tmp | mfpr_pull[pull] | mfpr_edge[edge]|mfpr_sleep[sleep];
	return mfpr_value;	
}

/*
*this function should be invoked only when function mfp_init_addr is previously 
*invoked to init the mfp_table
*/
void mfp_config_lpm_array(unsigned long *lpm_mfp_cfgs, int num)
{
	int i, drv_b11 = 0, no_lpm = 0;

	/* mmp plat is different with mg1-mg2-nevo plat.*/
#ifdef CONFIG_ARCH_MMP
	//if (cpu_is_pxa910() || cpu_is_mmp2() || cpu_is_mmp3())
		drv_b11 = 1;
	//if (cpu_is_pxa168() || cpu_is_pxa910())
		//no_lpm = 1;
#elif defined(CONFIG_ARCH_PXA)
		drv_b11 = 1;
#endif

	for (i = 0; i < num; i++, lpm_mfp_cfgs++) {
		unsigned long tmp, c = *lpm_mfp_cfgs;
		struct mfp_pin *p;
		int pin, af, drv, lpm, edge, pull,sleep;

		pin = MFP_PIN(c);
		//Q_ASSERT(pin < MFP_PIN_MAX);
		p = &mfp_table[pin];

		af  = MFP_AF(c);
		drv = MFP_DS(c);
		lpm = MFP_LPM_STATE(c);
		edge = MFP_LPM_EDGE(c);
		pull = MFP_PULL(c);
		sleep = MFP_SLEEP(c);

		if (drv_b11)
			drv = drv << 1;
		if (no_lpm)
			lpm = 0;
	
		tmp = MFPR_AF_SEL(af) | MFPR_DRIVE(drv);		

		p->mfpr_lpm = tmp | mfpr_pull[pull] | mfpr_edge[edge]|mfpr_sleep[sleep];
		p->config = c; __mfp_config_lpm(p);
	}
	mfpr_sync();
}




