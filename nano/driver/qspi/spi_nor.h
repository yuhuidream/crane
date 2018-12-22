
#ifndef _SPI_NOR_H
#define _SPI_NOR_H

#include "spi_flash.h"

/*
 * Standard SPI-NOR flash commands
 */
#define SPINOR_CMD_EN_RST			0x66
#define SPINOR_CMD_RESET			0x99
#define SPINOR_CMD_READ_STATUS1			0x05
//#define SPINOR_CMD_READ_CONFIG			0x15
#define SPINOR_CMD_READ_STATUS2			0x15 //0x35 //0x15
#define SPINOR_CMD_WRITE_STATUS			0x01
#define SPINOR_CMD_READ				0x03
#define SPINOR_CMD_READ_FAST			0x0b
#define SPINOR_CMD_READ_FAST_X2			0x3b
#define SPINOR_CMD_READ_FAST_X4			0x6b
#define SPINOR_CMD_READ_FAST_DUAL_IO		0xbb
#define SPINOR_CMD_READ_FAST_QUAD_IO		0xeb
#define SPINOR_CMD_PROG				0x02
#define SPINOR_CMD_PROG_X4			0x32
#define SPINOR_CMD_READ_ID			0x9f
#define SPINOR_CMD_READ_QPIID			0xaf
#define SPINOR_CMD_WR_DISABLE			0x04
#define SPINOR_CMD_WR_ENABLE			0x06
#define SPINOR_CMD_SECTOR_ERASE_4K		0x20 // 4K-bytes
#define SPINOR_CMD_BLK_ERASE_32K		0x52 // 32K-bytes
#define SPINOR_CMD_BLK_ERASE_64K		0xd8 // 64K-bytes
#define SPINOR_CMD_CHIP_ERASE			0xc7
#define SPINOR_CMD_ENABLE_QPI			0x35 //0x38 //0x35
#define SPINOR_CMD_DISABLE_QPI			0xf5 //0xff //0xf5
#define SPINOR_CMD_SET_READ_PARA		0xc0
#define SPINOR_CMD_END				0x0

#define STATUS1_BUSY	(0x1)
#define STATUS1_WEL	(0x1 << 1)
#define STATUS1_BP0	(0x1 << 2)
#define STATUS1_BP1	(0x1 << 3)
#define STATUS1_BP2	(0x1 << 4)
#define STATUS1_TB	(0x1 << 5)
#define STATUS1_SEC	(0x1 << 6)
#define STATUS1_SRP0	(0x1 << 7)

#define STATUS2_SRP1	(0x1)
#define STATUS2_QE	(0x1 << 1)
#define STATUS2_LB1	(0x1 << 3)
#define STATUS2_LB2	(0x1 << 4)
#define STATUS2_LB3	(0x1 << 5)
#define STATUS2_CMP	(0x1 << 6)
#define STATUS2_SUS	(0x1 << 7)

/*SPI NAND chip options*/
#define SPINOR_NEED_PLANE_SELECT	(1 << 0)
#define SPINOR_NEED_DIE_SELECT	(1 << 1)

#define SPINOR_MFR_MICRON		0x2C
#define SPINOR_MAX_ID_LEN		3

struct spi_flash_chip *spi_nor_init(void);
int spi_nor_test(struct spi_flash_chip *chip);

#endif
