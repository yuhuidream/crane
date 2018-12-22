#ifndef _MCU_DRV_H_
#define _MCU_DRV_H_

#include "predefines.h"

enum{
	MCU_BUS_8080 = 0,
	MCU_BUS_6800 = 1,
	MCU_BUS_LIMIT
};

enum{
	MCU_ENDIAN_LSB = 0,
	MCU_ENDIAN_MSB = 1,
	MCU_ENDIAN_LIMIT
};

enum{
	MCU_PATH_IMAGE = 0,
	MCU_PATH_REGISTER = 1,
	MCU_PATH_LIMIT
};

enum{
	MCU_FORMAT_RGB565 = 0,
	MCU_FORMAT_RGB666 = 1,
	MCU_FORMAT_LIMIT
};

enum{
	MCU_STATUS_UNINIT = 0,
	MCU_STATUS_INIT = 1,
	MCU_STATUS_LIMIT
};

/* MCU LCD specific properties */
struct timing_mcu {
	uint16_t rlpw; /*ns*/
	uint16_t rhpw; /*ns*/
	uint16_t wlpw; /*ns*/
	uint16_t whpw; /*ns*/
};

struct mcu_info {
	uint16_t bus_mode; /*8080, 6800*/
	uint16_t format;
	uint16_t endian;
	uint16_t device_id; /*0, 1*/
	uint16_t force_cs; /*0-disable, 1-enable*/
	uint16_t reserved;
	struct timing_mcu *timing;
};

struct s_mcu_ctx{
	uint32_t base_addr;
	uint32_t sclk; /*KHz*/
	uint16_t cur_path;
	uint16_t cur_cs;
	uint16_t status;
	uint16_t reserved;
	struct mcu_info info;
};

struct s_mcu_ctx* mcu_init(uint32_t sclk, struct mcu_info* info);
int mcu_set_cs(struct s_mcu_ctx *mcu_ctx, uint32_t enable);
int mcu_write_cmd(struct s_mcu_ctx* mcu_ctx, uint32_t cmd, uint32_t bits);
int mcu_write_data(struct s_mcu_ctx* mcu_ctx, uint32_t data, uint32_t bits);
int mcu_read_data(struct s_mcu_ctx* mcu_ctx, uint32_t cmd, uint32_t cmd_bits,
	uint32_t* data,  uint32_t data_bits);
int mcu_write_cmd_data(struct s_mcu_ctx* mcu_ctx, uint32_t cmd,
	uint32_t cmd_bits, uint32_t data, uint32_t data_bits);
int mcu_before_refresh(struct s_mcu_ctx* mcu_ctx);
int mcu_after_refresh(struct s_mcu_ctx* mcu_ctx);
void mcu_uninit(struct s_mcu_ctx* mcu_ctx);

#endif /*_MCU_DRV_H_*/
