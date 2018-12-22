/*
 *  sdh_mmc.c
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */

#include "bbu.h"
#include "bbu_malloc.h"
#include "timer_dec.h"
#include "pmic.h"
#include "utils.h"
#include "interrupt.h"
#include "list.h"
#include "PMUA.h"
#include "cache_ops.h"
#include "gpio_dec.h"
#include "SDH.h"
#include "sdh_def.h"
#include "sdh_device.h"
#include "sdh_host.h"
#include "mmc.h"
#include "sdh_mmc.h"
#include "mmc_ops.h"

uint32_t mmc_supply_vol_turn_on(struct sdhc_host *host)
{
	mrvl_sdh_power_off(host);
	switch_power_on_3v0_for_emmc();
	mrvl_sdh_power_on(host);
	
	return SDH_TRUE;
}

uint32_t mmc_supply_vol_turn_off(struct sdhc_host *host)
{
	mrvl_sdh_power_off(host);
	switch_power_off_from_emmc();
	
	return SDH_TRUE;
}



uint32_t mmc_card_attach(struct sdhc_host *host)
{
	struct mmc_card *card = get_mmc_card();
	
	mrvl_sdh_set_host_sig_vol(host, SD_HOST_VOLTAGE_1V8);
	mmc_supply_vol_turn_on(host);

	(void)memset(card, 0, sizeof(struct mmc_card));

	card->dev.name = (uint8_t *)"eMMC";	
	card->dev.host = host;
	card->dev.type = CARD_TYPE_MMC;

	mrvl_sdh_register_device(&(card->dev));

	return SDH_TRUE;
}

uint32_t mmc_card_remove(struct sdhc_host *host)
{
	struct mmc_card *card = get_mmc_card();

	if (card) {
		DEBUG_MMC_PRINT("Card is already pulled out.\r\n");
		return SDH_TRUE;
	}	
	DEBUG_MMC_PRINT("SD Card Removed!\r\n");
	
	mrvl_sdh_power_off(host);
	mmc_supply_vol_turn_off(host);
	
	mrvl_sdh_unregister_device(&(card->dev));
	card->dev.host = NULL;
		
	return SDH_TRUE;
}



uint32_t mrvl_sdh_init_for_mmc(void)
{
	struct sdhc_host *host = get_sdh_host();
	//struct sdh_device *dev;
	uint32_t err;
	uint32_t ctrl1;
	uint16_t ctrl2;

	host += EMMC_CARD_SLOT_NUM;
	
 	err = mrvl_sdh_pmu_switch_clock(host);
 	if (err)
 		return SDH_FALSE;
 	
 	mrvl_sdh_softreset_data_line(host);
 	mrvl_sdh_softreset_cmd_line(host);
 	mrvl_sdh_host_softrest_all(host);
 	
	/* VBUCK2 default is already on */
	//switch_power_on_1v8_for_emmc_io();
	//mrvl_sdh_supply_clk(host, MMC_INIT_FREQ, MMC_BUS_SPEED_HS26);
		
	/* Enable related interrupt */
	sdh_config_int_stat(host, 0xffffffff,
		(NORMAL_INT_STATUS_EN_CMD_COMPLETE_EN |
		 NORMAL_INT_STATUS_EN_XFER_COMPLETE_EN |
		 NORMAL_INT_STATUS_RETUNE_INT_EN) |
		(ERROR_INT_STATUS_EN_CMD_TIMEOUT_ERR_EN |
		 ERROR_INT_STATUS_EN_CMD_CRC_ERR_EN |
		 ERROR_INT_STATUS_EN_CMD_END_BIT_ERR_EN |
		 ERROR_INT_STATUS_EN_CMD_INDEX_ERR_EN |
		 ERROR_INT_STATUS_EN_DATA_TIMEOUT_ERR_EN |
		 ERROR_INT_STATUS_EN_RD_DATA_CRC_ERR_EN |
		 ERROR_INT_STATUS_EN_RD_DATA_END_BIT_ERR_EN |
		 ERROR_INT_STATUS_EN_AUTO_CMD12_ERR_EN |
		 ERROR_INT_STATUS_EN_TUNING_ERR_EN |
		 ERROR_INT_STATUS_EN_AXI_RESP_ERR_EN |
		 ERROR_INT_STATUS_EN_CPL_TIMEOUT_ERR_EN |
		 ERROR_INT_STATUS_EN_CRC_STATUS_ERR_EN) << 16);

	/* 
	 * Fix the Tx Hold time problem in DS/HS mode 
	 */
	ctrl1 = MRVL_SDH_REG32_READ(host->base_addr, TX_CFG_REG);
	ctrl1 |= TX_CFG_REG_TX_INT_CLK_SEL; 
	MRVL_SDH_REG32_WRITE(host->base_addr, TX_CFG_REG, ctrl1);

	/* For mmc card */
	ctrl2 = MRVL_SDH_REG16_READ(host->base_addr, CE_ATA_2);
	ctrl2 |= CE_ATA_2_MMC_CARD;
	MRVL_SDH_REG16_WRITE(host->base_addr, CE_ATA_2 , ctrl2);

	mmc_card_attach(host);
	
	DEBUG_MMC_PRINT("PRESENT_STATE: 0x%x\r\n", 
	    MRVL_SDH_REG32_READ(host->base_addr, PRESENT_STATE_1));
	DEBUG_MMC_PRINT("HOST_CTRL: 0x%x. SD Bus Power On/off!\r\n", 
	    MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL));
	host->initialized = 1;
	
	return SDH_TRUE;
}