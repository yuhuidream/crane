
#ifndef _SPI_NAND_H
#define _SPI_NAND_H

#include "spi_flash.h"

/*
 * Standard SPI-NAND flash commands
 */
#define SPINAND_CMD_RESET			0xff
#define SPINAND_CMD_GET_FEATURE			0x0f
#define SPINAND_CMD_SET_FEATURE			0x1f
#define SPINAND_CMD_PAGE_READ			0x13
#define SPINAND_CMD_READ_PAGE_CACHE_RDM		0x30
#define SPINAND_CMD_READ_PAGE_CACHE_LAST	0x3f
#define SPINAND_CMD_READ_FROM_CACHE		0x03
#define SPINAND_CMD_READ_FROM_CACHE_FAST	0x0b
#define SPINAND_CMD_READ_FROM_CACHE_X2		0x3b
#define SPINAND_CMD_READ_FROM_CACHE_DUAL_IO	0xbb
#define SPINAND_CMD_READ_FROM_CACHE_X4		0x6b
#define SPINAND_CMD_READ_FROM_CACHE_QUAD_IO	0xeb
#define SPINAND_CMD_BLK_ERASE			0xd8
#define SPINAND_CMD_PROG_EXC			0x10
#define SPINAND_CMD_PROG_LOAD			0x02
#define SPINAND_CMD_PROG_LOAD_RDM_DATA		0x84
#define SPINAND_CMD_PROG_LOAD_X4		0x32
#define SPINAND_CMD_PROG_LOAD_RDM_DATA_X4	0x34
#define SPINAND_CMD_READ_ID			0x9f
#define SPINAND_CMD_WR_DISABLE			0x04
#define SPINAND_CMD_WR_ENABLE			0x06
#define SPINAND_CMD_END				0x0

/* feature registers */
#define REG_BLOCK_LOCK		0xa0
#define REG_CFG			0xb0
#define REG_STATUS		0xc0
#define REG_DIE_SELECT		0xd0

/*Configuration register defines*/
#define CFG_QE_MASK		0x01
#define CFG_QE_ENABLE		0x01
#define CFG_ECC_MASK		0X10
#define CFG_ECC_ENABLE		0x10
#define CFG_LOT_MASK		0x20
#define CFG_LOT_ENABLE		0x20
#define CFG_OTP_MASK		0xc2
#define CFG_OTP_ENTER		0x40
#define CFG_OTP_EXIT		0x00
#define CFG_OTP_PROTECT	0xc0
#define CFG_SNOR_ENABLE	0x82

#define SPI_NAND_MT29F_ECC_MASK		0x70
#define SPI_NAND_MT29F_ECC_0_BIT		0x00
#define SPI_NAND_MT29F_ECC_1_3_BIT	0x10
#define SPI_NAND_MT29F_ECC_4_6_BIT	0x30
#define SPI_NAND_MT29F_ECC_7_8_BIT	0x50
#define SPI_NAND_MT29F_ECC_UNCORR	0x20

/*SPI NAND chip options*/
#define SPINAND_NEED_PLANE_SELECT	(1 << 0)
#define SPINAND_NEED_DIE_SELECT	(1 << 1)

#define SPINAND_MFR_MICRON		0x2C
#define SPINAND_MAX_ID_LEN		3

/* Ability of SPI Controllers */
#define SPI_OPM_RX_QUAD		1 << 3 /* Support 4 pin RX */
#define SPI_OPM_RX_DUAL		1 << 1 /* Support 2 pin RX */
#define SPI_OPM_RX		1 << 0 /* Support 1 pin RX */
#define SPI_OPM_TX_QUAD		1 << 1 /* Support 4 pin TX */
#define SPI_OPM_TX		1 << 0 /* Support 1 pin TX */

/**
 * struct mtd_oob_ops - oob operation operands
 * @mode:	operation mode
 * @len:	number of data bytes to write/read
 * @retlen:	number of data bytes written/read
 * @ooblen:	number of oob bytes to write/read
 * @oobretlen:	number of oob bytes written/read
 * @ooboffs:	offset of oob data in the oob area (only relevant when
 *		mode = MTD_OPS_PLACE_OOB or MTD_OPS_RAW)
 * @datbuf:	data buffer - if NULL only oob data are read/written
 * @oobbuf:	oob data buffer
 *
 * Note, it is allowed to read more than one OOB area at one go, but not write.
 * The interface assumes that the OOB write requests program only one page's
 * OOB area.
 */
//struct mtd_oob_ops {
//	unsigned int	mode;
//	uint32_t	len;
//	uint32_t	retlen;
//	uint32_t	ooblen;
//	uint32_t	oobretlen;
//	uint32_t	ooboffs;
//	uint8_t		*datbuf;
//	uint8_t		*oobbuf;
//};

#endif