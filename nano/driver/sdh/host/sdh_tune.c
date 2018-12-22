/*
 *  sdh_tune.c
 *
 *  Copyright (C) 2014 Marvell Corporation
 *
 */

#include "bbu.h"
#include "utils.h"
#include "bbu_malloc.h"
#include "list.h"
#include "SDH.h"
#include "sdh_def.h"
#include "sdh_device.h"
#include "sdh_host.h"
#include "mmc.h"

static uint32_t sdh_do_rx_sw_tuning(struct sdh_device *dev)
{
	struct sdhc_host *host = dev->host;
	int32_t i, start, max_delay, min_delay, step;
	uint32_t rx_cfg = 0;
	uint32_t exit_tuning = 0;
	uint32_t err;
	uint32_t opcode;

	host->hw_tune = 0;
#if 0
	if (dev->type & CARD_TYPE_SD)
		opcode = SD_CMD19_SEND_TUNING_BLOCK;
	else
		opcode = MMC_CMD21_SEND_TUNING_BLOCK;
#else
	opcode = MMC_CMD21_SEND_TUNING_BLOCK;
#endif
	start = max_delay = min_delay = -1;
	step = host->rx_tune_step;
	if (!step)
		step = 1;
	for (i = 0; i <= RX_DELAY_MAX; i = i + step) {
		rx_cfg = MRVL_SDH_REG32_READ(host->base_addr, RX_CFG_REG);
		rx_cfg &= ~RX_CFG_REG_SDCLK_DELAY_MSK;
		rx_cfg |= (i << RX_CFG_REG_SDCLK_DELAY_BASE);
		MRVL_SDH_REG32_WRITE(host->base_addr, RX_CFG_REG, rx_cfg);

		err = dev_send_tuning_block(dev, opcode);
		if (err) {
			DEBUG_SDH_PRINT("-");
			if ((start != -1) &&
			    (i - step - start > max_delay - min_delay)) {
				min_delay = start;
				max_delay = i - step;
				DEBUG_SDH_PRINT("\r\nstart: 0x%x, end: 0x%x\r\n",
					min_delay, max_delay);
				if (exit_tuning)
					break;
			}
			start = -1;
		} else {				
			if (start == -1)
				start = i;
			/* Exit when current window beyond host->expected_rx_window */
			if (i - start >= host->expected_rx_window) {
				exit_tuning = 1;
			}

			DEBUG_SDH_PRINT("+");
		} 
	}
	if ((start != -1) && (i - step - start > max_delay - min_delay)) {
		min_delay = start;
		max_delay = i - step;
	}

	if (max_delay == -1) {
		DEBUG_SDH_PRINT("\r\nRX configuration failed\r\n");
		return SDH_FALSE;
	}		

	rx_cfg = MRVL_SDH_REG32_READ(host->base_addr, RX_CFG_REG);
	rx_cfg &= ~RX_CFG_REG_SDCLK_DELAY_MSK;
	rx_cfg |= (min_delay + max_delay)/2 << RX_CFG_REG_SDCLK_DELAY_BASE;

	MRVL_SDH_REG32_WRITE(host->base_addr, RX_CFG_REG, rx_cfg);
	DEBUG_SDH_PRINT("\r\nFinal RX Delay: start=0x%x, end=0x%x\r\n",
	           min_delay, max_delay);
	return SDH_TRUE;	
}

uint32_t sdh_tx_rx_clock_tuning(struct sdh_device *dev, uint8_t search_mode)
{
	struct sdhc_host *host = dev->host; 
	uint32_t rx_cfg_orig, tx_cfg_orig;
	uint32_t rx_cfg, tx_cfg;		
	uint32_t err = 0;

	DEBUG_SDH_PRINT("Start to tuning Tx/Rx delay element:\r\n");
	DEBUG_SDH_PRINT("TX Step: %d, RX Step: %d\r\n",
		host->tx_tune_step, host->rx_tune_step);
	DEBUG_SDH_PRINT("Note: \"-\" --> bad Rx delay, \"+\" --> good Rx delay\r\n");
	DEBUG_SDH_PRINT("      \"#\" --> bad Tx delay, \"*\" --> good Tx delay\r\n");

	/* Save original value of TX/RX register so that can resume if tuning failed */
	tx_cfg_orig = MRVL_SDH_REG32_READ(host->base_addr, TX_CFG_REG);
	rx_cfg_orig = MRVL_SDH_REG32_READ(host->base_addr, RX_CFG_REG);

	if (search_mode & (SDH_TUNING_ONE_DIMENSION_TX_RX |
	                    SDH_TUNING_ONE_DIMENSION_RX)) {
		/* select RX clock before tuning TX delay */
		rx_cfg = rx_cfg_orig & (~0xf);	
		rx_cfg |= 0x1 << 2 | 0;
		MRVL_SDH_REG32_WRITE(host->base_addr, RX_CFG_REG, rx_cfg);
	}

	if (search_mode & (SDH_TUNING_ONE_DIMENSION_TX_RX |
	                   SDH_TUNING_ONE_DIMENSION_TX)) {
		tx_cfg = MRVL_SDH_REG32_READ(host->base_addr, TX_CFG_REG);
		tx_cfg |= 0x1 << 31;	
		MRVL_SDH_REG32_WRITE(host->base_addr, TX_CFG_REG, tx_cfg);
	}

	if (search_mode & SDH_TUNING_ONE_DIMENSION_RX) { 
		err = sdh_do_rx_sw_tuning(dev);
	}

	if (err) {
		DEBUG_SDH_PRINT("\r\nTuning failed!\r\n");
		MRVL_SDH_REG32_WRITE(host->base_addr, TX_CFG_REG, tx_cfg_orig);
		MRVL_SDH_REG32_WRITE(host->base_addr, RX_CFG_REG, rx_cfg_orig);	
		return err;	
	} else {
		tx_cfg = MRVL_SDH_REG32_READ(host->base_addr, TX_CFG_REG);
		rx_cfg = MRVL_SDH_REG32_READ(host->base_addr, RX_CFG_REG);
		DEBUG_SDH_PRINT("\r\nTuning success! tx_cfg: 0x%x, rx_cfg: 0x%x\r\n",
			tx_cfg, rx_cfg);	
	}
	return SDH_TRUE;
}
