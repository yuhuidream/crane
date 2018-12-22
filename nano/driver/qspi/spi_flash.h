#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

struct spi_flash_chip;
struct spi_flash_ops {
	int (*reset)(struct spi_flash_chip *chip);
	int (*read_id)(struct spi_flash_chip *chip, uint8_t *buf);
	int (*read)(struct spi_flash_chip *chip, int addr, int size, uint8_t *rbuf);
	int (*write)(struct spi_flash_chip *chip, int addr, int size, uint8_t *wbuf);
	int (*erase)(struct spi_flash_chip *chip, uint32_t addr, uint32_t len);
	void (*set_read_cmd)(struct spi_flash_chip *chip, uint8_t mode);
	void (*set_write_cmd)(struct spi_flash_chip *chip, uint8_t mode);
};

/**
 * struct spi_flash_chip - SPI-NAND Private Flash Chip Data
 * @name:		name of the chip
 * @spi:		[INTERN] point to spi device structure
 * @mfr_id:		[BOARDSPECIFIC] manufacture id
 * @dev_id:		[BOARDSPECIFIC] device id
 * @read_cache_op:	[REPLACEABLE] Opcode of read from cache
 * @write_cache_op:	[REPLACEABLE] Opcode of program load
 * @write_cache_rdm_op:	[REPLACEABLE] Opcode of program load random
 * @oobbuf:		[INTERN] buffer for read/write oob
 * @size:		[INTERN] the size of chip
 * @block_size:		[INTERN] the size of eraseblock
 * @page_size:		[INTERN] the size of page
 * @oob_size:	[INTERN] the size of page oob size
 * @block_shift:	[INTERN] number of address bits in a eraseblock
 * @page_shift:		[INTERN] number of address bits in a page (column
 *			address bits).
 * @pagemask:		[INTERN] page number mask = number of (pages / chip) - 1
 * @options:		[BOARDSPECIFIC] various chip options. They can partly
 *			be set to inform nand_scan about special functionality.
 * @ecc_strength:	[INTERN] ECC correctability
 * @refresh_threshold:	[INTERN] Bitflip threshold to return -EUCLEAN
 * @ecclayout:		[BOARDSPECIFIC] ECC layout control structure
 *			See the defines for further explanation.
 * @onfi_params:	[INTERN] holds the ONFI page parameter
 */
struct spi_flash_chip {
	char *name;
	struct spi_flash_cmd_cfg *table;
	struct spi_flash_ops *ops;

	uint8_t mfr_id;
	uint8_t dev_id;
	uint8_t read_op;
	uint8_t read_cache_op;
	uint8_t write_op;
	uint8_t write_cache_op;
	uint8_t write_cache_rdm_op;
	uint32_t qpi_dummy_cycles;
	uint32_t spi_freq;
	uint8_t qpi_enabled;

	uint8_t *oobbuf;
	uint64_t size;
	uint32_t block_size;
	uint16_t page_size;
	uint16_t oob_size;
	uint8_t lun_shift;
	uint8_t block_shift;
	uint8_t page_shift;
	uint16_t page_mask;
	uint32_t options;
	uint32_t ecc_strength;
	uint8_t refresh_threshold;
	uint8_t lun;
	uint8_t initialized;
	//struct nand_ecclayout *ecclayout;
	//struct spi_flash_onfi_params onfi_params;
};

#define SPINAND_MAX_ADDR_LEN		4

struct spi_flash_cmd_cfg {
	uint8_t opcode;
	uint8_t addr_bytes;
	uint8_t addr_pins;
	uint8_t mode_bits;
	uint8_t mode_pins;
	uint8_t dummy_cycles;
	uint8_t dummy_pins;
	uint8_t data_pins;
};

#define SPI_CMD_ENTRY(_opcode,_addr_bytes,_addr_pins,_mode_bits, \
			_mode_pins,_dummy_cycles,_dummy_pins,_data_pins) \
	{	\
		.opcode = _opcode, \
		.addr_bytes = _addr_bytes, \
		.addr_pins = _addr_pins, \
		.mode_bits = _mode_bits, \
		.mode_pins = _mode_pins, \
		.dummy_cycles = _dummy_cycles, \
		.dummy_pins = _dummy_pins, \
		.data_pins = _data_pins \
	}

struct spi_flash_cmd {
	uint8_t cmd;
	uint8_t n_addr;		/* Number of address */
	uint8_t addr[SPINAND_MAX_ADDR_LEN];	/* Reg Offset */
	uint32_t n_tx;		/* Number of tx bytes */
	const uint8_t *tx_buf;	/* Tx buf */
	uint32_t n_rx;		/* Number of rx bytes */
	uint8_t *rx_buf;	/* Rx buf */
	uint8_t mode;
	uint8_t qpi_enabled;
	uint32_t qpi_dummy_cycles;
};

/* feature registers */
#define REG_BLOCK_LOCK		0xa0
#define REG_CFG			0xb0
#define REG_STATUS		0xc0
#define REG_DIE_SELECT		0xd0

/* status */
#define STATUS_OIP_MASK		0x01
#define STATUS_CRBSY_MASK	0x80
#define STATUS_READY		(0 << 0)
#define STATUS_BUSY		(1 << 0)

#define STATUS_E_FAIL_MASK	0x04
#define STATUS_E_FAIL		(1 << 2)

#define STATUS_P_FAIL_MASK	0x08
#define STATUS_P_FAIL		(1 << 3)

enum {
	NORMAL_MODE,
	OTP_MODE,
	OTP_PROTECT_MODE,
	SNOR_READ_ENABLE_MODE,
};


enum {
	SPI_READ_X1,
	SPI_READ_X1_FAST,
	SPI_READ_X2,
	SPI_READ_X4,
	SPI_READ_X2_DUAL,
	SPI_READ_X4_QUAD,
	SPI_READ_FASK_QPI,
	SPI_READ_QUAD_QPI,
};

enum {
	SPI_WRITE_X1,
	SPI_WRITE_X4,
	SPI_WRITE_QPI,
};

#define false 0
#define true 1

//#define min(a, b)    ((a) < (b) ? (a) : (b))
//#define max(a, b)    ((a) > (b) ? (a) : (b))

int spi_flash_issue_cmd(struct spi_flash_chip *chip, struct spi_flash_cmd *cmd);

#endif