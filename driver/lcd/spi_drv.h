#ifndef _SPI_DRV_H_
#define _SPI_DRV_H_

#include "predefines.h"

enum{
	SPI_EDGE_RISING = 0,
	SPI_EDGE_FALLING = 1,
	SPI_EDGE_LIMIT
};

enum{
	SPI_ENDIAN_LSB = 0,
	SPI_ENDIAN_MSB = 1,
	SPI_ENDIAN_LIMIT
};

enum{
	SPI_PATH_IMAGE = 0,
	SPI_PATH_REGISTER = 1,
	SPI_PATH_LIMIT
};

enum{
	SPI_FORMAT_RGB565 = 0,
	SPI_FORMAT_RGB666 = 1,
	SPI_FORMAT_RGB666_2_3 = 2,
	SPI_FORMAT_RGB888 = 3,
	SPI_FORMAT_RGB888_2_3 = 4,
	SPI_FORMAT_LIMIT
};

enum{
	SPI_STATUS_UNINIT = 0,
	SPI_STATUS_INIT = 1,
	SPI_STATUS_LIMIT
};


struct timing_spi {
	uint32_t wclk; /*KHz*/
	uint32_t rclk; /*KHz*/
};

struct spi_info{
	uint16_t line_num; /*3 or 4*/
	uint16_t interface_id; /*1 or 2*/
	uint16_t data_lane_num; /*1 or 2*/
	uint16_t format;
	uint16_t device_id; /*0 or 1*/
	uint16_t sample_edge;
	uint16_t force_cs; /*0-disable, 1-enable*/
	uint16_t endian;
	uint16_t reserved;
	struct timing_spi *timing;
};

struct s_spi_ctx{
	uint32_t base_addr;
	uint32_t sclk; /*KHz*/
	uint16_t cur_path;
	uint16_t cur_cs;
	uint16_t status;
	uint16_t reserved;
	struct spi_info info;
};

struct s_spi_ctx* spi_init(uint32_t sclk, struct spi_info *info);
int spi_set_cs(struct s_spi_ctx *spi_ctx, uint32_t enable);
int spi_write_cmd(struct s_spi_ctx *spi_ctx, uint32_t cmd, uint32_t bits);
int spi_write_data(struct s_spi_ctx *spi_ctx, uint32_t data, uint32_t bits);
int spi_read_data(struct s_spi_ctx *spi_ctx, uint32_t cmd, uint32_t cmd_bits,
	uint32_t *data,  uint32_t data_bits);
int spi_before_refresh(struct s_spi_ctx *spi_ctx);
int spi_after_refresh(struct s_spi_ctx *spi_ctx);
void spi_uninit(struct s_spi_ctx *mcu_ctx);

#endif /*_SPI_DRV_H_*/
