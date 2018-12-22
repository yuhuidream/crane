/*
 *  sdh_mmc.h
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */

#ifndef SDH_MMC_H
#define SDH_MMC_H

#include "predefines.h"
#include <string.h>
#include <stdlib.h>

struct sdh_device;
struct sdhc_host;
struct mmc_card;

#define switch_power_on_3v0_for_emmc()         pmic_ldo_cfg(LDO_ON, LDO3, 3000, ACTIVE)
#define switch_power_on_1v8_for_emmc()         pmic_ldo_cfg(LDO_ON, LDO3, 1800, ACTIVE)
#define switch_power_off_from_emmc()           pmic_ldo_cfg(LDO_OFF, LDO3, 3000, ACTIVE)
#define switch_power_on_3v0_for_emmc_io()      
#define switch_power_on_1v8_for_emmc_io()      pmic_vbuck_cfg(VBUCK_ON, VBUCK2, 1800, ACTIVE)
#define switch_power_on_1v2_for_emmc_io()      
#define switch_power_off_from_emmc_io()        


uint32_t mrvl_sdh_init_for_mmc(void);
uint32_t mmc_supply_vol_turn_off(struct sdhc_host *host);
uint32_t mmc_supply_vol_turn_on(struct sdhc_host *host);

#endif
