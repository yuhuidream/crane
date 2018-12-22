#include <string.h>
#include "bbu.h"
#include "qspi_host.h"
#include "spi_nor.h"

static uint8_t read_buf[256];
static struct spi_flash_chip nor_chip;

static struct spi_flash_cmd_cfg cmd_table[] = {
	/*opcode  addr_bytes  addr_pins  mode_bits  mode_pins  dummy_cycles  dummy_pins  data_pins*/
	SPI_CMD_ENTRY(SPINOR_CMD_READ_STATUS1, 0, 0, 0, 0, 0, 0, 1),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_STATUS2, 0, 0, 0, 0, 0, 0, 1),
	SPI_CMD_ENTRY(SPINOR_CMD_WRITE_STATUS, 0, 0, 0, 0, 0, 0, 1),
	SPI_CMD_ENTRY(SPINOR_CMD_READ, 3, 1, 0, 0, 0, 0, 1),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_FAST, 3, 1, 0, 0, 8, 1, 1),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_FAST_X2, 3, 1, 0, 0, 8, 1, 2),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_FAST_X4, 3, 1, 0, 0, 8, 1, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_FAST_DUAL_IO, 3, 2, 8, 2, 0, 0, 2),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_FAST_QUAD_IO, 3, 4, 8, 4, 4, 4, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_SECTOR_ERASE_4K, 3, 1, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_BLK_ERASE_32K, 3, 1, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_BLK_ERASE_64K, 3, 1, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_CHIP_ERASE, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_PROG, 3, 1, 0, 0, 0, 0, 1),
	SPI_CMD_ENTRY(SPINOR_CMD_PROG_X4, 3, 1, 0, 0, 0, 0, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_WR_ENABLE, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_WR_DISABLE, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_ID, 0, 0, 0, 0, 0, 0,1),
	SPI_CMD_ENTRY(SPINOR_CMD_ENABLE_QPI, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_EN_RST, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_RESET, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_END, 0, 0, 0, 0, 0, 0, 0),
};

static struct spi_flash_cmd_cfg cmd_table_qpi[] = {
	/*opcode  addr_bytes  addr_pins  mode_bits  mode_pins  dummy_cycles  dummy_pins  data_pins*/
	SPI_CMD_ENTRY(SPINOR_CMD_READ_STATUS1, 0, 0, 0, 0, 0, 0, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_STATUS2, 0, 0, 0, 0, 0, 0, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_WRITE_STATUS, 0, 0, 0, 0, 0, 0, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_FAST, 3, 4, 0, 0, 4, 4, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_FAST_QUAD_IO, 3, 4, 0, 0, 6, 4, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_SECTOR_ERASE_4K, 3, 4, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_BLK_ERASE_32K,	3, 4, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_BLK_ERASE_64K,	3, 4, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_CHIP_ERASE, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_PROG, 3, 4, 0, 0, 0, 0, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_WR_ENABLE, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_WR_DISABLE, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_ID, 0, 0, 0, 0, 0, 0, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_READ_QPIID, 0, 0, 0, 0, 0, 0, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_DISABLE_QPI, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_SET_READ_PARA, 0, 0, 0, 0, 0, 0, 4),
	SPI_CMD_ENTRY(SPINOR_CMD_EN_RST, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_RESET, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINOR_CMD_END, 0, 0, 0, 0, 0, 0, 0),
};

/**
 * spi_nor_read_id - send 9Fh command to get ID
 * @chip: SPI_FLASH device structure
 * @buf: buffer to store id
 */
static int spi_nor_read_id(struct spi_flash_chip *chip, uint8_t *buf)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_READ_ID;
	cmd.n_rx = 3;
	cmd.rx_buf = buf;
	return spi_flash_issue_cmd(chip, &cmd);
}

static int spi_nor_read_qpiid(struct spi_flash_chip *chip, uint8_t *buf)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_READ_QPIID;
	cmd.n_rx = 3;
	cmd.rx_buf = buf;
	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nor_reset - send command to reset chip.
 * @chip: SPI_FLASH device structure
 */
static int spi_nor_reset(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;
	int i;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_EN_RST;
	if (spi_flash_issue_cmd(chip, &cmd) < 0)
		bbu_printf("spi_nor reset failed!\n");

	cmd.cmd = SPINOR_CMD_RESET;
	if (spi_flash_issue_cmd(chip, &cmd) < 0)
		bbu_printf("spi_nor reset failed!\n");
	/* elapse 2ms before issuing any other command */
	//udelay(2000);
	for(i=0;i<20;i++);
	return 0;
}

/**
 * spi_nor_write_enable - send command 06h to enable write or erase the
 * Nand cells
 * @chip: SPI_FLASH device structure
 * Description:
 *   Before write and erase the Nand cells, the write enable has to be set.
 *   After the write or erase, the write enable bit is automatically
 *   cleared (status register bit 2)
 *   Set the bit 2 of the status register has the same effect
 */
static int spi_nor_write_enable(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_WR_ENABLE;
	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nor_read_status1 - get status register value
 * @chip: SPI_FLASH device structure
 * @status: buffer to store value
 * Description:
 *   After read, write, or erase, the Nand device is expected to set the
 *   busy status.
 *   This function is to allow reading the status of the command: read,
 *   write, and erase.
 *   Once the status turns to be ready, the other status bits also are
 *   valid status bits.
 */
static int spi_nor_read_status1(struct spi_flash_chip *chip, uint8_t *status)
{
	struct spi_flash_cmd cmd;
	int ret;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_READ_STATUS1;
	cmd.n_rx = 1;
	cmd.rx_buf = status;
	ret = spi_flash_issue_cmd(chip, &cmd);
	if (ret < 0)
		bbu_printf("err: read register %d\n", ret);
	return ret;
}

static int spi_nor_read_status2(struct spi_flash_chip *chip, uint8_t *status)
{
	struct spi_flash_cmd cmd;
	int ret;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_READ_STATUS2;
	cmd.n_rx = 1;
	cmd.rx_buf = status;
	ret = spi_flash_issue_cmd(chip, &cmd);
	if (ret < 0)
		bbu_printf("err: read register %d\n", ret);
	return ret;
}

/**
 * spi_nor_write_reg - send command 1Fh to write register
 * @chip: SPI_FLASH device structure
 * @reg; register to write
 * @buf: buffer stored value
 */
static int spi_nor_write_status(struct spi_flash_chip *chip, uint8_t *buf)
{
	struct spi_flash_cmd cmd;
	int ret;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_WRITE_STATUS;
	cmd.n_tx = 2,
	cmd.tx_buf = buf,

	ret = spi_flash_issue_cmd(chip, &cmd);
	if (ret < 0)
		bbu_printf("err: %d write register\n", ret);
	return ret;
}

/**
 * spi_nor_wait - wait until the command is done
 * @chip: SPI_FLASH device structure
 * @s: buffer to store status register(can be NULL)
 */
static int spi_nor_wait(struct spi_flash_chip *chip, uint8_t *s)
{
	uint8_t status;
	unsigned long ret = -ETIMEDOUT;

	while (1) {
		spi_nor_read_status1(chip, &status);
		if ((status & STATUS_OIP_MASK) == STATUS_READY) {
			ret = 0;
			goto out;
		}
	}
out:
	if (s)
		*s = status;
	return ret;
}

/**
 * spi_nor_erase_block_erase - send command D8h to erase a block
 * @chip: SPI_FLASH device structure
 * @page_addr: the page to erase.
 * Description:
 *   Need to wait for tERS.
 */
static int spi_nor_erase_sector(struct spi_flash_chip *chip,
					uint32_t page_addr)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_SECTOR_ERASE_4K;
	cmd.n_addr = 3;
	cmd.addr[0] = (uint8_t)(page_addr >> 16);
	cmd.addr[1] = (uint8_t)(page_addr >> 8);
	cmd.addr[2] = (uint8_t)page_addr;
	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nor_erase_block_erase - send command D8h to erase a block
 * @chip: SPI_FLASH device structure
 * @page_addr: the page to erase.
 * Description:
 *   Need to wait for tERS.
 */
static int spi_nor_erase_block(struct spi_flash_chip *chip,
					uint32_t page_addr)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_BLK_ERASE_64K;
	cmd.n_addr = 3;
	cmd.addr[0] = (uint8_t)(page_addr >> 16);
	cmd.addr[1] = (uint8_t)(page_addr >> 8);
	cmd.addr[2] = (uint8_t)page_addr;
	return spi_flash_issue_cmd(chip, &cmd);
}

static int spi_nor_erase_chip(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_CHIP_ERASE;
	return spi_flash_issue_cmd(chip, &cmd);
}

static int spi_nor_erase_all(struct spi_flash_chip *chip)
{
	uint8_t status;
	int ret = 0;

	spi_nor_write_enable(chip);
	spi_nor_erase_chip(chip);
	ret = spi_nor_wait(chip, &status);
	if (ret < 0) {
		bbu_printf("chip erase command wait failed\n");
		return ret;
	}
	return 0;
}

/**
 * spi_nor_erase - [Interface] erase block(s)
 * @chip: spi flash device structure
 * @addr: address that erase start with, should be blocksize aligned
 * @len: length that want to be erased, should be blocksize aligned
 * Description:
 *   Erase one ore more blocks
 *   The command sequence for the BLOCK ERASE operation is as follows:
 *       06h (WRITE ENBALE command)
 *       D8h (BLOCK ERASE command)
 *       0Fh (GET FEATURES command to read the status register)
 */
int spi_nor_erase(struct spi_flash_chip *chip, uint32_t addr, uint32_t len)
{
	int page_addr, pages_per_block;
	uint8_t status;
	int ret = 0;

	bbu_printf("%s: address = 0x%x, len = %d\r\n",
			 __func__, addr, len);
	/* check address align on block boundary */
	if (addr & (chip->block_size - 1)) {
		bbu_printf("%s: Unaligned address\n", __func__);
		return -EINVAL;
	}

	if (len & (chip->block_size - 1)) {
		bbu_printf("%s: Length not block aligned\n", __func__);
		return -EINVAL;
	}

	/* Do not allow erase past end of device */
	if ((len + addr) > chip->size) {
		bbu_printf("%s: Erase past end of device\n", __func__);
		return -EINVAL;
	}

	pages_per_block = 1 << (chip->block_shift - chip->page_shift);
	page_addr = addr >> chip->page_shift;
	while (len) {
		spi_nor_write_enable(chip);
		spi_nor_erase_block(chip, page_addr);
		ret = spi_nor_wait(chip, &status);
		if (ret < 0) {
			bbu_printf("block erase command wait failed\n");
			goto erase_exit;
		}

		/* Increment page address and decrement length */
		len -= (0x1 << chip->block_shift);
		page_addr += pages_per_block;
	}

erase_exit:
	return ret;
}

static int spi_nor_read(struct spi_flash_chip *chip, int addr,
			int size, uint8_t *rbuf)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = chip->read_op;
	cmd.n_addr = 3;
	cmd.addr[0] = (uint8_t)((addr >> 16) & 0xff);
	cmd.addr[1] = (uint8_t)((addr >> 8) & 0xff);
	cmd.addr[2] = (uint8_t)(addr & 0xff);
	cmd.n_rx = size;
	cmd.rx_buf = rbuf;
	cmd.mode = 0xff;
	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nor_program_data_to_cache - write data to cache register
 * @chip: SPI_FLASH device structure
 * @page_addr: page to write
 * @column: the location to write to the cache
 * @len: number of bytes to write
 * @wrbuf: buffer held @len bytes
 * @clr_cache: clear cache register or not
 * Description:
 *   Command can be 02h, 32h, 84h, 34h
 *   02h and 32h will clear the cache with 0xff value first
 *   Since it is writing the data to cache, there is no tPROG time.
 */
static int spi_nor_program_data(struct spi_flash_chip *chip,
		uint32_t addr, int len, const uint8_t *wbuf)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = chip->write_op;
	cmd.n_addr = 3;
	cmd.addr[0] = (uint8_t)((addr >> 16) & 0xff);
	cmd.addr[1] = (uint8_t)((addr >> 8) & 0xff);;
	cmd.addr[2] = (uint8_t)(addr & 0xff);;
	cmd.n_tx = len;
	cmd.tx_buf = wbuf;
	return spi_flash_issue_cmd(chip, &cmd);
}

static int spi_nor_write(struct spi_flash_chip *chip, int addr,
			int size, uint8_t *buf)
{
	uint8_t status;
	int ret = 0;

	spi_nor_write_enable(chip);
	spi_nor_program_data(chip, addr, size, buf);
	ret = spi_nor_wait(chip, &status);
	if (ret < 0) {
		bbu_printf("error %d reading page 0x%x from cache\n",
			ret, addr);
		return ret;
	}
	return ret;	
}

static void spi_nor_set_read_cmd(struct spi_flash_chip *chip, uint8_t mode)
{
	if (chip->qpi_enabled &&
	    (mode != SPI_READ_FASK_QPI &&
	     mode != SPI_READ_QUAD_QPI))
		mode = SPI_READ_FASK_QPI;

	switch(mode) {
		case SPI_READ_X1:
			chip->read_op = SPINOR_CMD_READ;
			break;
		case SPI_READ_X1_FAST:
		case SPI_READ_FASK_QPI:
			chip->read_op = SPINOR_CMD_READ_FAST;
			break;
		case SPI_READ_X2:
			chip->read_op = SPINOR_CMD_READ_FAST_X2;
			break;
		case SPI_READ_X4:
			chip->read_op = SPINOR_CMD_READ_FAST_X4;
			break;
		case SPI_READ_X2_DUAL:
			chip->read_op = SPINOR_CMD_READ_FAST_DUAL_IO;
			break;
		case SPI_READ_X4_QUAD:
		case SPI_READ_QUAD_QPI:
			chip->read_op = SPINOR_CMD_READ_FAST_QUAD_IO;
			break;
		default:
			chip->read_op = SPINOR_CMD_READ_FAST;
			break;
	}
}

static void spi_nor_set_write_cmd(struct spi_flash_chip *chip, uint8_t mode)
{
	if (chip->qpi_enabled)
		mode = SPI_WRITE_QPI;

	switch(mode) {
		case SPI_WRITE_X1:
		case SPI_WRITE_QPI:
			chip->write_op = SPINOR_CMD_PROG;
			break;
		case SPI_WRITE_X4:
			chip->write_op = SPINOR_CMD_PROG_X4;
			break;
		default:
			chip->write_op = SPINOR_CMD_PROG;
			break;
	}
}

static int spi_set_read_parameters(struct spi_flash_chip *chip, uint8_t *buf)
{
	struct spi_flash_cmd cmd;
	int ret = 0;

	if (!chip->qpi_enabled)
		return 0;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_SET_READ_PARA;
	cmd.n_tx = 1,
	cmd.tx_buf = buf,
	ret = spi_flash_issue_cmd(chip, &cmd);
	if (ret < 0)
		bbu_printf("err: %d set_read_parameters\n", ret);
	return ret;
}

/* Macronix use status register bit s6 for QE enable */
static int spi_nor_enable_quad_s6(struct spi_flash_chip *chip)
{
	uint8_t data[2];
	uint8_t status;
	int ret;

	spi_nor_read_status1(chip, &data[0]);
	spi_nor_read_status2(chip, &data[1]);
	if (data[0] & (0x1 << 6)) {
		bbu_printf("QE already set: bit6\r\n");
		return 0;
	}

	data[0] |= 0x1 << 6;
	spi_nor_write_enable(chip);
	spi_nor_write_status(chip, data);
	ret = spi_nor_wait(chip, &status);
	if (ret < 0) {
		bbu_printf("error %d write status register\n", ret);
		return ret;
	}

	data[0] = 0;
	spi_nor_read_status1(chip, &data[0]);
	if (!(data[0] & (0x1 << 6))) {
		bbu_printf("Failed to enable quad, maybe not supported\r\n");
		return -1;
	}
	return 0;
}

/* Winbond/Gigadevice use status register bit S9 for QE enable */
static int spi_nor_enable_quad_s9(struct spi_flash_chip *chip)
{
	uint8_t data[2];
	uint8_t status;
	int ret;

	spi_nor_read_status1(chip, &data[0]);
	spi_nor_read_status2(chip, &data[1]);
	if (data[1] & STATUS2_QE) {
		bbu_printf("QE already set: bit9\r\n");
		return 0;
	}

	data[1] |= STATUS2_QE;
	spi_nor_write_enable(chip);
	spi_nor_write_status(chip, data);
	ret = spi_nor_wait(chip, &status);
	if (ret < 0) {
		bbu_printf("error %d write status register\n", ret);
		return ret;
	}

	data[1] = 0;
	spi_nor_read_status2(chip, &data[1]);
	if (!(data[1] & STATUS2_QE)) {
		bbu_printf("Failed to enable quad, maybe not supported\r\n");
		return -1;
	}
	return 0;
}

static int spi_nor_enable_quad(struct spi_flash_chip *chip)
{
	if(chip->mfr_id == 0xc2)
		return spi_nor_enable_quad_s6(chip);
	else
		return spi_nor_enable_quad_s9(chip);
}

static int spi_nor_disable_qpi(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;
	int ret = 0;

	if (!chip->qpi_enabled)
		return 0;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_DISABLE_QPI;
	ret = spi_flash_issue_cmd(chip, &cmd);
	if (ret < 0)
		return ret;

	chip->qpi_enabled = 0;
	chip->table = cmd_table;
	return 0;
}

static int spi_nor_enable_qpi(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;
	int ret = 0;
	uint8_t cfg;

	if (chip->qpi_enabled)
		return 0;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINOR_CMD_ENABLE_QPI;
	ret = spi_flash_issue_cmd(chip, &cmd);
	if (ret < 0)
		return ret;

	chip->qpi_enabled = 1;
	chip->table = cmd_table_qpi;

	/* TODO
	 * Dummy cycles depend on busf freq and voltage
	 */	
	chip->qpi_dummy_cycles = 6;
	//ret = spi_set_read_parameters(chip, &cfg);
	//if (ret < 0)
	//	return ret;

	spi_nor_set_read_cmd(chip, SPI_READ_FASK_QPI);
	spi_nor_set_write_cmd(chip, SPI_WRITE_QPI);
	return 0;
}

static const struct spi_flash_ops qspi_ops = {
	.reset = spi_nor_reset,
	.read_id = spi_nor_read_id,
	.read = spi_nor_read,
	.write = spi_nor_write,
	.erase = spi_nor_erase,
	.set_read_cmd = spi_nor_set_read_cmd,
	.set_write_cmd = spi_nor_set_write_cmd,
};

static int blocksize = 64*1024;
/**
 * spi_nor_init - [Interface] Init SPI_FLASH device driver
 * @spi: spi device structure
 * @chip_ptr: pointer point to spi nand device structure pointer
 */
struct spi_flash_chip *spi_nor_init(void)
{
	struct spi_flash_chip *chip = &nor_chip;
	uint8_t id[SPINOR_MAX_ID_LEN] = {0};

	if (chip->initialized) {
		if (chip->qpi_enabled)
			spi_nor_disable_qpi(chip);
		spi_nor_reset(chip);
	}

	memset(chip, 0, sizeof(struct spi_flash_chip));
	chip->ops = (void *)&qspi_ops;
	chip->table = cmd_table;

	chip->size = 64*1024*1024;
	chip->block_size = 64*1024;
	chip->block_shift = 16;
	chip->page_shift = 8;
	chip->page_size = 1 << 8;
	chip->page_mask = chip->page_size - 1;
	chip->spi_freq = 104000000;

	spi_nor_set_read_cmd(chip, SPI_READ_X1);
	spi_nor_set_write_cmd(chip, SPI_WRITE_X1);

	spi_nor_reset(chip);
	spi_nor_read_id(chip, id);
	bbu_printf("SPI-NOR: blk_size=%d %d, mfr_id: %d, dev_id: 0x%d_%d\r\n",
			blocksize, 1234, id[0], id[2], id[1]);
	spi_nor_enable_quad(chip);
	chip->initialized = 1;
	return chip;
}

int spi_nor_do_read(int addr, int size, uint8_t *rbuf)
{
	struct spi_flash_chip *chip = &nor_chip;

	return spi_nor_read(chip, addr, size, read_buf);
}

int spi_nor_do_write(int addr, int size, uint8_t *buf)
{
	struct spi_flash_chip *chip = &nor_chip;
	int len, ret;

	while (size) {
		len = chip->page_size - (addr & chip->page_mask);
		len = min(len, size);

		ret = spi_nor_write(chip, addr, len, buf);
		if (ret < 0) {
			bbu_printf("page program failed\n");
			break;
		}

		/* Increment page address and decrement length */
		size -= len;
		addr += len;
		buf += len;
	}
	return ret;
}

int spi_nor_do_erase(int addr, int size)
{
	struct spi_flash_chip *chip = &nor_chip;
	int ret, end_blk_addr;

	end_blk_addr = (addr + size) & (chip->block_size - 1);
	addr &= chip->block_size - 1;

	while(addr <= end_blk_addr) {
		ret = spi_nor_erase(chip, addr, chip->block_size);
		if (ret < 0) {
			bbu_printf("block erase failed\n");
			break;
		}
		addr += chip->block_size;
	}
	return ret;
}

int spi_nor_test(struct spi_flash_chip *chip)
{
	int i;

	spi_nor_erase_all(chip);
	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_erase(chip, 0, (uint32_t)(64*1024));
	bbu_printf("\r\n%s: %d- data after erase-SPI_READ_X1 ---\r\n", __func__, __LINE__);
	spi_nor_set_read_cmd(chip, SPI_READ_X1);
	spi_nor_read(chip, 0x0, 32, read_buf);
	for (i = 0; i < 32; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = i;
	spi_nor_set_write_cmd(chip, SPI_WRITE_X1);
	spi_nor_write(chip, 0x0, 128, read_buf);

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_X1);
	spi_nor_read(chip, 0x0, 128, read_buf);
	bbu_printf("\r\n%s: %d--- read--\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_X2);
	spi_nor_read(chip, 0x0, 128, read_buf);
	bbu_printf("\r\n%s: %d--- read_fast_x2--\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_X2_DUAL);
	spi_nor_read(chip, 0x0, 128, read_buf);
	bbu_printf("\r\n%s: %d--- read_fast_dual--\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_X4_QUAD);
	spi_nor_read(chip, 0x0, 128, read_buf);
	bbu_printf("\r\n%s: %d-- read_fast_quad--\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = i;
	spi_nor_set_write_cmd(chip, SPI_WRITE_X4);
	spi_nor_write(chip, 1024, 128, read_buf);

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_X4_QUAD);
	spi_nor_read(chip, 1024, 128, read_buf);
	bbu_printf("\r\n%s: %d-- read_fast_quad for write_x4--\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	spi_nor_erase(chip, 0, (uint32_t)(64*1024));

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_read(chip, 1024, 256, read_buf);
	bbu_printf("\r\n%s: %d-- read_fast_quad 255 after erase--\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = 0xaa;
	spi_nor_set_write_cmd(chip, SPI_WRITE_X4);
	spi_nor_write(chip, 1024, 128, read_buf);

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_X1);
	spi_nor_read(chip, 1024, 256, read_buf);
	bbu_printf("\r\n%s: %d--- read after write 0xaa----\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_X2);
	spi_nor_read(chip, 1024, 256, read_buf);
	bbu_printf("\r\n%s: %d--- read_x2 after write 0xaa----\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_X4);
	spi_nor_read(chip, 1024, 256, read_buf);
	bbu_printf("\r\n%s: %d---- read_fast_x4 after write 0xaa--\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_X2_DUAL);
	spi_nor_read(chip, 1024, 256, read_buf);
	bbu_printf("\r\n%s: %d--- read_fast_dual after write 0xaa--\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_X4_QUAD);
	spi_nor_read(chip, 1024, 256, read_buf);
	bbu_printf("\r\n%s: %d-- read_fast_quad after write 0xaa--\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	return 0;
}

int spi_nor_test_qpi(struct spi_flash_chip *chip)
{
	int i;

	spi_nor_enable_qpi(chip);
	spi_nor_erase_all(chip);
	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_erase(chip, 0, (uint32_t)(64*1024));
	bbu_printf("%s: %d- data after erase-SPI_READ_FASK_QPI ---\r\n", __func__, __LINE__);
	spi_nor_set_read_cmd(chip, SPI_READ_FASK_QPI);
	spi_nor_read(chip, 0x0, 32, read_buf);
	for (i = 0; i < 32; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	bbu_printf("%s: %d- data after erase- qpi---\r\n", __func__, __LINE__);
	spi_nor_set_read_cmd(chip, SPI_READ_QUAD_QPI);
	spi_nor_read(chip, 0x0, 32, read_buf);
	for (i = 0; i < 32; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = i;
	spi_nor_set_write_cmd(chip, SPI_WRITE_QPI);
	spi_nor_write(chip, 0x0, 128, read_buf);

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_QUAD_QPI);
	spi_nor_read(chip, 0x0, 128, read_buf);
	bbu_printf("%s: %d----c read_fast_SPI_READ_QUAD_QPI-\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	for (i = 0; i < 256; i++)
		read_buf[i] = 0;
	spi_nor_set_read_cmd(chip, SPI_READ_FASK_QPI);
	spi_nor_read(chip, 0x0, 128, read_buf);
	bbu_printf("%s: %d--- read- SPI_READ_FASK_QPI-\r\n", __func__, __LINE__);
	for (i = 0; i < 256; i++) {
		if (i && (i%16==0))
			bbu_printf("\r\n");
		bbu_printf("%d ", read_buf[i]);
	}

	return 0;
}
