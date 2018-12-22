/*
 *  sdh_host.h
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */
 
#ifndef SD_MMC_HOST_H
#define SD_MMC_HOST_H

#include "predefines.h"
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "list.h"
#include "SDH.h"
#include "mmc_ops.h"
#include "sdh_def.h"

struct dev_request;


#define SD_VDD_170_195		0x00000080	/* 	VDD voltage 1.65 ~ 1.95 	*/
#define SD_VDD_20_21		0x00000100	/* 	VDD voltage 2.0  ~ 2.1 		*/
#define SD_VDD_21_22		0x00000200	/* 	VDD voltage 2.1  ~ 2.2 		*/
#define SD_VDD_22_23		0x00000400	/* 	VDD voltage 2.2  ~ 2.3 		*/
#define SD_VDD_23_24		0x00000800	/* 	VDD voltage 2.3  ~ 2.4 		*/
#define SD_VDD_24_25		0x00001000	/* 	VDD voltage 2.4  ~ 2.5 		*/
#define SD_VDD_25_26		0x00002000	/* 	VDD voltage 2.5  ~ 2.6 		*/
#define SD_VDD_26_27		0x00004000	/* 	VDD voltage 2.6  ~ 2.7 		*/
#define SD_VDD_27_28		0x00008000	/* 	VDD voltage 2.7  ~ 2.8 		*/
#define SD_VDD_28_29		0x00010000	/* 	VDD voltage 2.8  ~ 2.9 		*/
#define SD_VDD_29_30		0x00020000	/* 	VDD voltage 2.9  ~ 3.0 		*/
#define SD_VDD_30_31		0x00040000	/* 	VDD voltage 3.0  ~ 3.1 		*/
#define SD_VDD_31_32		0x00080000	/* 	VDD voltage 3.1  ~ 3.2 		*/
#define SD_VDD_32_33		0x00100000	/*	VDD voltage 3.2  ~ 3.3 		*/
#define SD_VDD_33_34		0x00200000	/* 	VDD voltage 3.3  ~ 3.4 		*/
#define SD_VDD_34_35		0x00400000	/* 	VDD voltage 3.4  ~ 3.5 		*/
#define SD_VDD_35_36		0x00800000	/* 	VDD voltage 3.5  ~ 3.6 		*/

#define SD_OCR_CCS_BIT30 ((1 << 30)(UL))

#define MRVL_SDH_REG16_READ(base, reg_name) 	\
({	\
	uint16_t *addr;	\
						\
	addr = (uint16_t *)(base + offsetof(SD_TypeDef, reg_name));	\
	_readw(addr);	\
})


#define MRVL_SDH_REG32_READ(base, reg_name) 	\
({	\
	uint32_t *addr;	\
						\
	addr = (uint32_t *)(base + offsetof(SD_TypeDef, reg_name));	\
	_readl(addr);	\
})


#define MRVL_SDH_REG16_WRITE(base, reg_name, val)		\
do {	\
	uint16_t *addr;	\
						\
	addr = (uint16_t *)(base + offsetof(SD_TypeDef, reg_name));	\
	_writew((uint16_t)val, addr);	\
} while(0)


#define MRVL_SDH_REG32_WRITE(base, reg_name, val)		\
do {	\
	uint32_t *addr;	\
						\
	addr = (uint32_t *)(base + offsetof(SD_TypeDef, reg_name));	\
	_writel((uint32_t)val, addr);	\
} while(0)

#define MAX_CARD_NUMBER 5
#define MRVL_SDH_BASE_CLK 200000000UL  /* in Hz */
struct sdhc_host
{
	struct sdh_device *dev[MAX_CARD_NUMBER];
	struct dev_request *drq;
	uint32_t base_addr;  
	uint8_t card_num;

	uint32_t cap;
#define SDH_HOST_CAP_1V2 (1<<0)
#define SDH_HOST_CAP_1V8 (1<<1)
	
	uint32_t dma_in_use;
	uint32_t total_bytes_left;
	uint32_t sdma_sys_addr;
	
	volatile uint32_t transfer_complete;

	uint32_t host_version;
#define SD_HOST_SPEC_VERSION_1_0 	0
#define SD_HOST_SPEC_VERSION_2_0		1
#define SD_HOST_SPEC_VERSION_3_0		2

	uint8_t clk_source;
	uint32_t first_div;
	uint32_t second_div;
	uint32_t base_clk;
	uint8_t base_clk_ready;
	uint8_t base_shared;
	uint8_t max_first_div;
	uint32_t max_second_div;
	uint16_t expected_rx_window;
	uint16_t expected_tx_window;
	
	uint32_t initialized;
	uint32_t suspend;
	uint32_t sdclk;
	uint8_t sig_vol;

	uint16_t tx_point;
	uint16_t tx_window;
	uint16_t tx_tune_step;
	uint16_t rx_tune_step;

	uint32_t def_timing_ddr50;
	uint32_t def_timing_sdr50;
	uint32_t def_timing_sdr104;
#define SDH_GET_RX(c) ((c) & 0xffff)
#define SDH_GET_TX(c) (((c) >> 16) & 0xffff)
#define SDH_SET_RX(c, v)	\
do {	\
	(c) = (c & ~0xffff) | (v) & 0xffff;	\
} while(0);

#define SDH_SET_TX(c, v)	\
do {	\
	(c) = (c & ~0xffff0000) | ((v) & 0xffff << 16);	\
} while(0);

	uint32_t hw_tune;
};

#define SD_ADMA_MAX_BYTES_PER_DESC (1<<16)

#define MRVL_EMEI_SDH_MAX_CURRENT 300  /* in mA */

#define MRVL_SDH_HOST_SDMA 	0
#define MRVL_SDH_HOST_ADMA1	1
#define MRVL_SDH_HOST_ADMA2	2

#define SDH_TRANS_TYPE_INT	(1<<0)  /* if use interrupt, this bit should be set. default is poll mode */
#define SDH_TRANS_TYPE_PIO	(1<<1)
#define SDH_TRANS_TYPE_SDMA	(1<<2)  /* supported now, except for packed command */
#define SDH_TRANS_TYPE_ADMA	(1<<3)	
#define SDH_TRANS_REQ_FROM_AO_SD (1<<7)  /* For calling from AO, this bit should be set, because the result is returned through events */

/* this flag should locate beyond 6bits, because low 6bits are used by cmd index */
#define COM_SDH_TX_TUNNING (1<<7)
#define COM_SDH_RX_TUNNING (1<<6)

#define SDH_TUNING_ONE_DIMENSION_TX_RX (1<<0)
#define SDH_TUNING_ONE_DIMENSION_RX    (1<<1)
#define SDH_TUNING_ONE_DIMENSION_TX    (1<<2)
#define SDH_RX_HW_TUNING               (1<<3)
#define SDH_NO_TUNING                  (0)

struct sdhc_host *get_sdh_host(void);

uint32_t mrvl_sdh_clk_stop(struct sdhc_host *host);
uint32_t mrvl_sdh_clk_start(struct sdhc_host *host);
uint32_t mrvl_sdh_force_pad_clk_on(struct sdhc_host *host);
uint32_t mrvl_sdh_force_pad_clk_off(struct sdhc_host *host);
uint32_t mrvl_sdh_pad_clk_auto_run(struct sdhc_host *host);
uint32_t mrvl_sdh_power_off(struct sdhc_host *host);
uint32_t mrvl_sdh_power_on(struct sdhc_host *host);
uint32_t mrvl_sdh_set_max_vol(struct sdhc_host *host);
uint32_t mrvl_sdh_change_bit_mode(struct sdhc_host *host, uint32_t width);
uint32_t mrvl_sdh_supply_clk(struct sdhc_host *host, uint32_t clk_freq_hz, uint8_t mode);
uint32_t mrvl_sdh_host_data_level(struct sdhc_host *host);
uint32_t mrvl_sdh_control_1v8_enable(struct sdhc_host *host, uint8_t on);
void mrvl_sdh_start_request(struct sdhc_host *host, struct dev_request *drq);
void mrvl_sdh_wait_req_done(struct sdhc_host *host, struct dev_request *drq);
uint32_t mrvl_sdh_softreset_data_line(struct sdhc_host *host);
uint32_t mrvl_sdh_softreset_cmd_line(struct sdhc_host *host);
uint32_t mrvl_sdh_host_softrest_all(struct sdhc_host *host);
uint32_t mrvl_sdh_change_host_bit_mode(struct sdhc_host *host, uint32_t width);
uint32_t mrvl_sdh_set_host_sig_vol(struct sdhc_host *host, uint8_t vol);
uint32_t sd_card_supply_vol_turn_on(struct sdhc_host *host);
uint32_t sd_card_supply_vol_turn_off(struct sdhc_host *host);
uint32_t mrvl_sdh_clk_enable(struct sdhc_host *host);
uint32_t mrvl_sdh_clk_disable(struct sdhc_host *host);
uint32_t mrvl_sdh_select_uhs_mode(struct sdhc_host *host, uint8_t mode);
uint32_t mrvl_sdh_dma_int_handle_for_sdma(struct sdhc_host *host);

#define SD_HOST_VOLTAGE_3V3 (1<<0)
#define SD_HOST_VOLTAGE_3V0 (1<<1)
#define SD_HOST_VOLTAGE_1V8 (1<<2)

enum
{
	SDH_CLOCK_SOURCE_416MHZ = 0,
	SDH_CLOCK_SOURCE_624MHZ = 1,
	SDH_CLOCK_SOURCE_48MHZ = 2,
	SDH_CLOCK_SOURCE_800MHZ = 3,
	SDH_CLOCK_SOURCE_NUM,
};


uint32_t mrvl_sdh_pmu_switch_clock(struct sdhc_host *host);
uint32_t mrvl_sdh_cmd_done(struct sdhc_host *host);
uint32_t mrvl_sdh_unregister_device(struct sdh_device *dev);
void mrvl_sdh_error_handle(struct sdhc_host *host);
uint32_t mrvl_sdh_pio_xfer_data(struct sdhc_host *host);
uint32_t mrvl_sdh_end_of_data(struct sdhc_host *host);
uint32_t mrvl_sdh_register_device(struct sdh_device *dev);
uint32_t mrvl_sdh_unregister_device(struct sdh_device *dev);
uint32_t mrvl_sdh_xfer_done(struct sdhc_host *host);
uint32_t mrvl_sdh_set_timeout(struct sdhc_host *host, uint32_t time_ms, uint32_t sd_clk);
uint32_t mrvl_sdh_set_host_driver_strength(struct sdhc_host *host, uint8_t strength);
void sdh_config_int_stat(struct sdhc_host *host, uint32_t clear, uint32_t set);
void sdh_config_int_sig(struct sdhc_host *host, uint32_t clear, uint32_t set);
uint32_t sdh_tx_rx_clock_tuning(struct sdh_device *dev, uint8_t search_mode);

#define TX_DELAY_MAX 0x1ff
#define RX_DELAY_MAX 0x1ff
#define MAX_VALID_TUNING_WINDOW 30

/* There are 3 slots in SDH, slot 1 is for SD card */
enum
{
	SD_CARD_SLOT_NUM = 0,
	EMMC_CARD_SLOT_NUM = 1,
	SDIO_CARD_SLOT_NUM = 2,
	MRVL_SDH_SLOT_NUM,
};

#define SDH_BUS_SPEED_DEFAULT (1<<0)
#define SDH_BUS_SPEED_SDR_CLK_50M (1<<1)
#define SDH_BUS_SPEED_SDR_CLK_100M (1<<2)
#define SDH_BUS_SPEED_SDR_CLK_208M (1<<3)
#define SDH_BUS_SPEED_DDR_CLK_50M (1<<4)
#define SDH_BUS_SPEED_SEARCH (1<<5)


#if defined(MMC_DEBUG_ON) || defined(SD_DEBUG_ON)
#define DEBUG_SDH_PRINT(fmt, args...) bbu_printf(fmt, ##args)
#else
#define DEBUG_SDH_PRINT(fmt, args...)
#endif

//#define CONIFG_SDH_PTS
#define MAX_BLOCK_NUM 2048
#define ADMA_DESCRIPTOR_BUF_SIZE (((MAX_BLOCK_NUM*512+SD_ADMA_MAX_BYTES_PER_DESC)/SD_ADMA_MAX_BYTES_PER_DESC + 1)*sizeof(adma_descriptor_line_t))

static inline uint32_t mrvl_sdh_disable_card_interrupt(struct sdhc_host *host)
{
	uint16_t ir_en;

	/* Set card interrupt status enable to 0, clearing interrupt requeset to the system */
	ir_en = MRVL_SDH_REG16_READ(host->base_addr, NORMAL_INT_STATUS_EN);
	ir_en &= ~NORMAL_INT_STATUS_EN_CARD_INT_EN;
	MRVL_SDH_REG16_WRITE(host->base_addr, NORMAL_INT_STATUS_EN, ir_en);

	return SDH_TRUE;
}

static inline uint32_t mrvl_sdh_enable_card_interrupt(struct sdhc_host *host)
{
	uint16_t ir_en;

	/* Set card interrupt status enable to 1, allowing interrupt requeset to the system */
	ir_en = MRVL_SDH_REG16_READ(host->base_addr, NORMAL_INT_STATUS_EN);
	ir_en |= NORMAL_INT_STATUS_EN_CARD_INT_EN;
	MRVL_SDH_REG16_WRITE(host->base_addr, NORMAL_INT_STATUS_EN, ir_en);

	return SDH_TRUE;
}

static inline uint32_t mrvl_sdh_check_1v8_stat(struct sdhc_host *host)
{
	uint16_t ctrl;

	ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL_2);
	return ((ctrl & HOST_CTRL_2_SDH_V18_EN) ? 0 : 1);
}

#endif

