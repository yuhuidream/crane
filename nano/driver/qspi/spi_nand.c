#include <string.h>
#include "bbu.h"
#include "qspi_host.h"
#include "spi_nand.h"

static struct spi_flash_chip nand_chip;
static const struct spi_flash_cmd_cfg cmd_table[] = {
	/*opcode  addr_bytes  addr_pins mode_bits  mode_pins  dummy_cycles  dummy_pins   data_pins*/
	SPI_CMD_ENTRY(SPINAND_CMD_GET_FEATURE, 1, 1, 0, 0, 0, 0, 1),
	SPI_CMD_ENTRY(SPINAND_CMD_SET_FEATURE, 1, 1, 0, 0, 0, 0, 1),
	SPI_CMD_ENTRY(SPINAND_CMD_PAGE_READ, 3, 1, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINAND_CMD_READ_PAGE_CACHE_RDM, 3, 1, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINAND_CMD_READ_PAGE_CACHE_LAST,	0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINAND_CMD_READ_FROM_CACHE, 2, 1, 0, 0, 8, 1, 1),
	SPI_CMD_ENTRY(SPINAND_CMD_READ_FROM_CACHE_FAST, 2, 1, 0, 0, 8, 1, 1),
	SPI_CMD_ENTRY(SPINAND_CMD_READ_FROM_CACHE_X2, 2, 1, 0, 0, 8, 1, 2),
	SPI_CMD_ENTRY(SPINAND_CMD_READ_FROM_CACHE_DUAL_IO, 2, 2, 0, 0, 4, 2, 2),
	SPI_CMD_ENTRY(SPINAND_CMD_READ_FROM_CACHE_X4, 2, 1, 0, 0, 8, 1, 4),
	SPI_CMD_ENTRY(SPINAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, 4, 0, 0, 4, 4, 4),
	SPI_CMD_ENTRY(SPINAND_CMD_BLK_ERASE, 3, 1, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINAND_CMD_PROG_EXC, 3, 1, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINAND_CMD_PROG_LOAD, 2, 1, 0, 0, 0, 0, 1),
	SPI_CMD_ENTRY(SPINAND_CMD_PROG_LOAD_RDM_DATA, 2, 1, 0, 0, 0, 0, 1),
	SPI_CMD_ENTRY(SPINAND_CMD_PROG_LOAD_X4, 2, 1, 0, 0, 0, 0, 4),
	SPI_CMD_ENTRY(SPINAND_CMD_PROG_LOAD_RDM_DATA_X4, 2, 1, 0, 0, 0, 0, 4),
	SPI_CMD_ENTRY(SPINAND_CMD_WR_ENABLE, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINAND_CMD_WR_DISABLE, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINAND_CMD_READ_ID, 0, 0, 0, 0, 8, 1, 1),
	SPI_CMD_ENTRY(SPINAND_CMD_RESET, 0, 0, 0, 0, 0, 0, 0),
	SPI_CMD_ENTRY(SPINAND_CMD_END, 0, 0, 0, 0, 0, 0, 0),
};

/**
 * spi_nand_read_reg - send command 0Fh to read register
 * @chip: SPI_FLASH device structure
 * @reg; register to read
 * @buf: buffer to store value
 */
static int spi_nand_read_reg(struct spi_flash_chip *chip,
			uint8_t reg, uint8_t *buf)
{
	struct spi_flash_cmd cmd;
	int ret;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINAND_CMD_GET_FEATURE;
	cmd.n_addr = 1;
	cmd.addr[0] = reg;
	cmd.n_rx = 1;
	cmd.rx_buf = buf;

	ret = spi_flash_issue_cmd(chip, &cmd);
	if (ret < 0)
		bbu_printf("err: %d read register %d\n", ret, reg);

	return ret;
}

/**
 * spi_nand_write_reg - send command 1Fh to write register
 * @chip: SPI-NAND device structure
 * @reg; register to write
 * @buf: buffer stored value
 */
static int spi_nand_write_reg(struct spi_flash_chip *chip,
			uint8_t reg, uint8_t *buf)
{
	struct spi_flash_cmd cmd;
	int ret;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINAND_CMD_SET_FEATURE;
	cmd.n_addr = 1;
	cmd.addr[0] = reg;
	cmd.n_tx = 1,
	cmd.tx_buf = buf,

	ret = spi_flash_issue_cmd(chip, &cmd);
	if (ret < 0)
		bbu_printf("err: %d write register %d\n", ret, reg);

	return ret;
}


/**
 * spi_nand_read_status - get status register value
 * @chip: SPI-NAND device structure
 * @status: buffer to store value
 * Description:
 *   After read, write, or erase, the Nand device is expected to set the
 *   busy status.
 *   This function is to allow reading the status of the command: read,
 *   write, and erase.
 *   Once the status turns to be ready, the other status bits also are
 *   valid status bits.
 */
static int spi_nand_read_status(struct spi_flash_chip *chip, uint8_t *status)
{
	return spi_nand_read_reg(chip, REG_STATUS, status);
}

/**
 * spi_nand_get_cfg - get configuration register value
 * @chip: SPI-NAND device structure
 * @cfg: buffer to store value
 * Description:
 *   Configuration register includes OTP config, Lock Tight enable/disable
 *   and Internal ECC enable/disable.
 */
static int spi_nand_get_cfg(struct spi_flash_chip *chip, uint8_t *cfg)
{
	return spi_nand_read_reg(chip, REG_CFG, cfg);
}

/**
 * spi_nand_set_cfg - set value to configuration register
 * @chip: SPI-NAND device structure
 * @cfg: buffer stored value
 * Description:
 *   Configuration register includes OTP config, Lock Tight enable/disable
 *   and Internal ECC enable/disable.
 */
static int spi_nand_set_cfg(struct spi_flash_chip *chip, uint8_t *cfg)
{
	return spi_nand_write_reg(chip, REG_CFG, cfg);
}

/**
 * spi_nand_enable_ecc - enable internal ECC
 * @chip: SPI-NAND device structure
 * Description:
 *   There is one bit( bit 0x10 ) to set or to clear the internal ECC.
 *   Enable chip internal ECC, set the bit to 1
 *   Disable chip internal ECC, clear the bit to 0
 */
static int spi_nand_enable_ecc(struct spi_flash_chip *chip)
{
	uint8_t cfg = 0;

	spi_nand_get_cfg(chip, &cfg);
	if ((cfg & CFG_ECC_MASK) == CFG_ECC_ENABLE)
		return 0;
	cfg |= CFG_ECC_ENABLE;
	return spi_nand_set_cfg(chip, &cfg);
}

/**
 * spi_nand_disable_ecc - disable internal ECC
 * @chip: SPI-NAND device structure
 * Description:
 *   There is one bit( bit 0x10 ) to set or to clear the internal ECC.
 *   Enable chip internal ECC, set the bit to 1
 *   Disable chip internal ECC, clear the bit to 0
 */
static int spi_nand_disable_ecc(struct spi_flash_chip *chip)
{
	uint8_t cfg = 0;

	spi_nand_get_cfg(chip, &cfg);
	if ((cfg & CFG_ECC_MASK) == CFG_ECC_ENABLE) {
		cfg &= ~CFG_ECC_ENABLE;
		return spi_nand_set_cfg(chip, &cfg);
	}
	return 0;
}

static int spi_nand_enable_quad(struct spi_flash_chip *chip)
{
	uint8_t cfg = 0;

	spi_nand_get_cfg(chip, &cfg);
	if ((cfg & CFG_QE_MASK) == CFG_QE_ENABLE)
		return 0;
	cfg |= CFG_QE_ENABLE;
	return spi_nand_set_cfg(chip, &cfg);
}

/**
 * spi_nand_ecc_status - decode status regisger to get ecc info
 * @status: status register value to decode
 * @corrected: bitflip count that ecc corrected
 * @ecc_error: uncorrected bitflip happen or not
 */
static void spi_nand_ecc_status(unsigned int status,
					unsigned int *corrected,
					unsigned int *ecc_error)
{
	unsigned int ecc_status = status & SPI_NAND_MT29F_ECC_MASK;

	*ecc_error = (ecc_status == SPI_NAND_MT29F_ECC_UNCORR);
	switch (ecc_status) {
	case SPI_NAND_MT29F_ECC_0_BIT:
		*corrected = 0;
		break;
	case SPI_NAND_MT29F_ECC_1_3_BIT:
		*corrected = 3;
		break;
	case SPI_NAND_MT29F_ECC_4_6_BIT:
		*corrected = 6;
		break;
	case SPI_NAND_MT29F_ECC_7_8_BIT:
		*corrected = 8;
		break;
	}
}

/**
 * spi_nand_write_enable - send command 06h to enable write or erase the
 * Nand cells
 * @chip: SPI-NAND device structure
 * Description:
 *   Before write and erase the Nand cells, the write enable has to be set.
 *   After the write or erase, the write enable bit is automatically
 *   cleared (status register bit 2)
 *   Set the bit 2 of the status register has the same effect
 */
static int spi_nand_write_enable(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINAND_CMD_WR_ENABLE;

	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nand_read_page_to_cache - send command 13h to read data from Nand to cache
 * @chip: SPI-NAND device structure
 * @page_addr: page to read
 */
static int spi_nand_read_page_to_cache(struct spi_flash_chip *chip,
					uint32_t page_addr)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINAND_CMD_PAGE_READ;
	cmd.n_addr = 3;
	cmd.addr[0] = (uint8_t)(page_addr >> 16);
	cmd.addr[1] = (uint8_t)(page_addr >> 8);
	cmd.addr[2] = (uint8_t)page_addr;

	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nand_read_from_cache - read data out from cache register
 * @chip: SPI-NAND device structure
 * @page_addr: page to read
 * @column: the location to read from the cache
 * @len: number of bytes to read
 * @rbuf: buffer held @len bytes
 * Description:
 *   Command can be 03h, 0Bh, 3Bh, 6Bh, BBh, EBh
 *   The read can specify 1 to (page size + spare size) bytes of data read at
 *   the corresponding locations.
 *   No tRd delay.
 */
static int spi_nand_read_from_cache(struct spi_flash_chip *chip, uint32_t page_addr,
		uint32_t column, uint8_t len, uint8_t *rbuf)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = chip->read_cache_op;
	cmd.n_addr = 2;
	cmd.addr[0] = (uint8_t)(column >> 8);
	cmd.addr[1] = (uint8_t)column;
	cmd.n_rx = len;
	cmd.rx_buf = rbuf;

	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nand_program_data_to_cache - write data to cache register
 * @chip: SPI-NAND device structure
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
static int spi_nand_program_data_to_cache(struct spi_flash_chip *chip,
		uint32_t page_addr, uint32_t column, uint8_t len,
		const uint8_t *wbuf, uint8_t clr_cache)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	if (clr_cache)
		cmd.cmd = chip->write_cache_op;
	else
		cmd.cmd = chip->write_cache_rdm_op;
	cmd.n_addr = 2;
	cmd.addr[0] = (uint8_t)(column >> 8);
	cmd.addr[1] = (uint8_t)column;
	cmd.n_tx = len;
	cmd.tx_buf = wbuf;

	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nand_program_execute - send command 10h to write a page from
 * cache to the Nand array
 * @chip: SPI-NAND device structure
 * @page_addr: the physical page location to write the page.
 * Description:
 *   Need to wait for tPROG time to finish the transaction.
 */
static int spi_nand_program_execute(struct spi_flash_chip *chip, uint32_t page_addr)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINAND_CMD_PROG_EXC;
	cmd.n_addr = 3;
	cmd.addr[0] = (uint8_t)(page_addr >> 16);
	cmd.addr[1] = (uint8_t)(page_addr >> 8);
	cmd.addr[2] = (uint8_t)page_addr;

	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nand_erase_block_erase - send command D8h to erase a block
 * @chip: SPI-NAND device structure
 * @page_addr: the page to erase.
 * Description:
 *   Need to wait for tERS.
 */
static int spi_nand_erase_block(struct spi_flash_chip *chip,
					uint32_t page_addr)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINAND_CMD_BLK_ERASE;
	cmd.n_addr = 3;
	cmd.addr[0] = (uint8_t)(page_addr >> 16);
	cmd.addr[1] = (uint8_t)(page_addr >> 8);
	cmd.addr[2] = (uint8_t)page_addr;

	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nand_wait - wait until the command is done
 * @chip: SPI-NAND device structure
 * @s: buffer to store status register(can be NULL)
 */
static int spi_nand_wait(struct spi_flash_chip *chip, uint8_t *s)
{
	uint8_t status;
	unsigned long ret = -ETIMEDOUT;

	while (1) {
		spi_nand_read_status(chip, &status);
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
 * spi_nand_read_id - send 9Fh command to get ID
 * @chip: SPI_FLASH device structure
 * @buf: buffer to store id
 */
static int spi_nand_read_id(struct spi_flash_chip *chip, uint8_t *buf)
{
	struct spi_flash_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINAND_CMD_READ_ID;
	cmd.n_rx = 2;
	cmd.rx_buf = buf;

	return spi_flash_issue_cmd(chip, &cmd);
}

/**
 * spi_nand_reset - send command FFh to reset chip.
 * @chip: SPI_FLASH device structure
 */
static int spi_nand_reset(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd cmd;
	int i;

	memset(&cmd, 0, sizeof(struct spi_flash_cmd));
	cmd.cmd = SPINAND_CMD_RESET;

	if (spi_flash_issue_cmd(chip, &cmd) < 0)
		bbu_printf("spi_nand reset failed!\n");

	/* elapse 2ms before issuing any other command */
	//udelay(2000);
	for(i=0;i<20;i++);

	return 0;
}

/**
 * spi_nand_change_mode - switch chip to OTP/OTP protect/Normal mode
 * @chip: SPI-NAND device structure
 * @mode: mode to enter
 */
static int spi_nand_change_mode(struct spi_flash_chip *chip, uint8_t mode)
{
	uint8_t cfg;

	spi_nand_get_cfg(chip, &cfg);
	switch (mode) {
	case OTP_MODE:
		cfg = (cfg & ~CFG_OTP_MASK) | CFG_OTP_ENTER;
		break;
	case OTP_PROTECT_MODE:
		cfg = (cfg & ~CFG_OTP_MASK) | CFG_OTP_PROTECT;
		break;
	case SNOR_READ_ENABLE_MODE:
		cfg = (cfg & ~CFG_OTP_MASK) | CFG_SNOR_ENABLE;
		break;
	case NORMAL_MODE:
		cfg = (cfg & ~CFG_OTP_MASK) | CFG_OTP_EXIT;
		break;
	}
	spi_nand_set_cfg(chip, &cfg);

	return 0;
}

/**
 * spi_nand_do_read_page - read page from flash to buffer
 * @chip: spi nand chip structure
 * @page_addr: page address/raw address
 * @column: column address
 * @ecc_off: without ecc or not
 * @corrected: how many bit error corrected
 * @buf: data buffer
 * @len: data length to read
 * Description:
 *   Return -EBADMSG when internal ecc can not correct bitflips.
 *   The command sequence to transfer data from NAND array to output is
 *   follows:
 *      13h (PAGE READ to cache register)
 *      0Fh (GET FEATURES command to read the status)
 *      0Bh/03h/3Bh/6Bh (Read from Cache Xn); or BBh/EBh (Read From
 *      Cache Dual/Quad IO)
 */
static int spi_nand_do_read_page(struct spi_flash_chip *chip, uint32_t page_addr,
				uint32_t column, uint8_t ecc_off,
				uint32_t *corrected, uint8_t *buf, uint8_t len)
{
	int ret;
	//unsigned int ecc_error;
	uint8_t status;

	spi_nand_read_page_to_cache(chip, page_addr);
	ret = spi_nand_wait(chip, &status);
	if (ret < 0) {
		bbu_printf("error %d waiting page 0x%x to cache\n",
			ret, page_addr);
		return ret;
	}
	//if (!ecc_off) {
	//	spi_nand_ecc_status(status, corrected, &ecc_error);
	//	if (ecc_error) {
	//		bbu_printf("internal ECC error reading page 0x%x\n",
	//			page_addr);
	//		ret = -EBADMSG;
	//	}
	//}
	spi_nand_read_from_cache(chip, page_addr, column, len, buf);

	return ret;
}

/**
 * spi_nand_do_write_page - write data from buffer to flash
 * @chip: spi nand chip structure
 * @page_addr: page address/raw address
 * @column: column address
 * @buf: data buffer
 * @len: data length to write
 * @clr_cache: clear cache register with 0xFF or not
 * Description:
 *   Page program sequence is as follows:
 *       06h (WRITE ENABLE)
 *       02h/32h/84h/34h (PROGRAM LOAD (RAMDOM_DATA) Xn)
 *       10h (PROGRAM EXECUTE)
 *       0Fh (GET FEATURE command to read the status)
 *   PROGRAM LOAD Xn instruction will reset the cache resigter with 0xFF,
 *   while PROGRAM LOAD RANDOM DATA Xn instruction will only update the
 *   data bytes that are specified by the command input sequence and the rest
 *   of data in the cache buffer will remain unchanged.
 */
static int spi_nand_do_write_page(struct spi_flash_chip *chip, uint32_t page_addr,
				uint32_t column, const uint8_t *buf,
				uint8_t len, uint8_t clr_cache)
{
	uint8_t status;
	uint8_t p_fail = false;
	int ret = 0;

	spi_nand_write_enable(chip);
	spi_nand_program_data_to_cache(chip, page_addr,
					column, len, buf, clr_cache);
	spi_nand_program_execute(chip, page_addr);
	ret = spi_nand_wait(chip, &status);
	if (ret < 0) {
		bbu_printf("error %d reading page 0x%x from cache\n",
			ret, page_addr);
		return ret;
	}
	if ((status & STATUS_P_FAIL_MASK) == STATUS_P_FAIL) {
		bbu_printf("program page 0x%x failed\n", page_addr);
		p_fail = true;
	}
	if (p_fail)
		ret = -EIO;

	return ret;
}

/**
 * spi_nand_read_pages - read data from flash to buffer
 * @chip: spi nand chip structure
 * @from: offset to read from
 * @ops: oob operations description structure
 * Description:
 *   Normal read function, read one page to buffer before issue
 *   another. Return -EUCLEAN when bitflip is over threshold.
 *   Return -EBADMSG when internal ecc can not correct bitflips.
 */
static int spi_nand_read_pages(struct spi_flash_chip *chip, uint32_t from,
				uint8_t len, uint32_t *retlen, uint8_t *buf,
				uint8_t ecc_off)
{
	int page_addr, page_offset, size;
	int ret;
	unsigned int corrected = 0;
	unsigned int max_bitflip = 0;
	int readlen = len;
	unsigned int failed = 0;
	uint32_t _retlen;

	bbu_printf("%s: from = 0x%012llx, len = %i\n",
			 __func__, from, readlen);
	page_addr = from >> chip->page_shift;
	page_offset = from & chip->page_mask;
	_retlen = 0;

	while (1) {
		size = min(readlen, chip->page_size - page_offset);
		ret = spi_nand_do_read_page(chip, page_addr, page_offset,
					ecc_off, &corrected, buf + _retlen, size);
		if (ret == -EBADMSG) {
			failed++;
		} else if (ret) {
			bbu_printf("error %d reading page 0x%x\n",
				ret, page_addr);
			goto out;
		}
		max_bitflip = max(corrected, max_bitflip);

		_retlen += size;
		readlen -= size;
		page_offset = 0;

		if (!readlen)
			break;
		page_addr++;
	}
out:
	*retlen = _retlen;
	if (max_bitflip >= chip->refresh_threshold)
		ret = -EUCLEAN;
	if (failed)
		ret = -EBADMSG;

	return ret;
}

/**
 * spi_nand_read - [Interface] SPI-NAND read
 * @chip: spi nand device structure
 * @from: offset to read from
 * @len: number of bytes to read
 * @retlen: pointer to variable to store the number of read bytes
 * @buf: the databuffer to put data
 */
int spi_nand_read(struct spi_flash_chip *chip, uint32_t from, uint8_t len,
	uint32_t *retlen, uint8_t *buf, uint8_t ecc_off)
{
	int ret;

	bbu_printf("%s: from = 0x%012llx, len = %i\n",
			 __func__, from, len);
	/* Do not allow reads past end of device */
	if (from >= chip->size) {
		bbu_printf("%s: attempt to read beyond end of device\n",
				__func__);
		return -EINVAL;
	}

	if (ecc_off)
		spi_nand_disable_ecc(chip);

	ret = spi_nand_read_pages(chip, from, len, retlen, buf, ecc_off);

	if (ecc_off)
		spi_nand_enable_ecc(chip);

	return ret;
}

/**
 * spi_nand_write - [Interface] SPI-NAND write
 * @chip: spi nand device structure
 * @to: offset to write to
 * @len: number of bytes to write
 * @retlen: pointer to variable to store the number of written bytes
 * @buf: the data to write
 */
int spi_nand_write(struct spi_flash_chip *chip, uint32_t to, uint8_t len,
	uint8_t *retlen, const uint8_t *buf, uint8_t ecc_off)
{
	int page_addr, page_offset, size;
	int writelen = len;
	int ret = 0;
	uint8_t clr_cache = true;
	uint32_t _retlen;

	bbu_printf("%s: to = 0x%012llx, len = %i\n",
			 __func__, to, writelen);
	/* Do not allow reads past end of device */
	if (to >= chip->size) {
		bbu_printf("%s: attempt to write beyond end of device\n",
				__func__);
		return -EINVAL;
	}

	page_addr = to >> chip->page_shift;
	page_offset = to & chip->page_mask;
	_retlen = 0;

	if (ecc_off)
		spi_nand_disable_ecc(chip);

	while (1) {
		size = min(writelen, chip->page_size - page_offset);
		ret = spi_nand_do_write_page(chip, page_addr, page_offset,
				buf + _retlen, size, clr_cache);
		if (ret) {
			bbu_printf("error %d writing page 0x%x\n",
				ret, page_addr);
			goto out;
		}
		_retlen += size;
		writelen -= size;
		page_offset = 0;
		if (!writelen)
			break;
		page_addr++;
	}
out:
	*retlen = _retlen;
	if (ecc_off)
		spi_nand_enable_ecc(chip);

	return ret;
}

/**
 * spi_nand_erase - [Interface] erase block(s)
 * @chip: spi nand device structure
 * @addr: address that erase start with, should be blocksize aligned
 * @len: length that want to be erased, should be blocksize aligned
 * Description:
 *   Erase one ore more blocks
 *   The command sequence for the BLOCK ERASE operation is as follows:
 *       06h (WRITE ENBALE command)
 *       D8h (BLOCK ERASE command)
 *       0Fh (GET FEATURES command to read the status register)
 */
int spi_nand_erase(struct spi_flash_chip *chip, uint64_t addr, uint64_t len)
{
	int page_addr, pages_per_block;
	uint8_t status;
	int ret = 0;

	bbu_printf("%s: address = 0x%012llx, len = %llu\n",
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
		///* Check if we have a bad block, we do not erase bad blocks! */
		//if (spi_nand_block_isbad(chip, ((uint32_t) page_addr) <<
		//			chip->page_shift)) {
		//	bbu_printf("%s: attempt to erase a bad block at 0x%012llx\n",
		//	__func__, ((uint32_t) page_addr) << chip->page_shift);
		//	goto erase_exit;
		//}
		spi_nand_write_enable(chip);
		spi_nand_erase_block(chip, page_addr);
		ret = spi_nand_wait(chip, &status);
		if (ret < 0) {
			bbu_printf("block erase command wait failed\n");
			goto erase_exit;
		}
		if ((status & STATUS_E_FAIL_MASK) == STATUS_E_FAIL) {
			bbu_printf("erase block 0x%012llx failed\n",
				((uint32_t) page_addr) << chip->page_shift);
			ret = -EIO;
			goto erase_exit;
		}

		/* Increment page address and decrement length */
		len -= (1ULL << chip->block_shift);
		page_addr += pages_per_block;
	}

erase_exit:
	return ret;
}

/**
 * spi_nand_set_rd_wr_op - Chose the best read write command
 * @chip: SPI-NAND device structure
 * Description:
 *   Chose the fastest r/w command according to spi controller's ability.
 * Note:
 *   If 03h/0Bh follows SPI NAND protocol, there is no difference,
 *   while if follows SPI NOR protocol, 03h command is working under
 *   <=20Mhz@3.3V,<=5MHz@1.8V; 0Bh command is working under
 *   133Mhz@3.3v, 83Mhz@1.8V.
 */
static void spi_nand_set_rd_wr_op(struct spi_flash_chip *chip,
			uint32_t op_mode_rx, uint32_t op_mode_tx)
{
	if (op_mode_rx & SPI_OPM_RX_QUAD)
		chip->read_cache_op = SPINAND_CMD_READ_FROM_CACHE_QUAD_IO;
	else if (op_mode_rx & SPI_OPM_RX_DUAL)
		chip->read_cache_op = SPINAND_CMD_READ_FROM_CACHE_DUAL_IO;
	else
		chip->read_cache_op = SPINAND_CMD_READ_FROM_CACHE_FAST;

	if (op_mode_tx & SPI_OPM_TX_QUAD) {
		chip->write_cache_op = SPINAND_CMD_PROG_LOAD_X4;
		chip->write_cache_rdm_op = SPINAND_CMD_PROG_LOAD_RDM_DATA_X4;
	} else {
		chip->write_cache_op = SPINAND_CMD_PROG_LOAD;
		chip->write_cache_rdm_op = SPINAND_CMD_PROG_LOAD_RDM_DATA;
	}
}

/**
 * spi_nand_init - [Interface] Init SPI-NAND device driver
 * @spi: spi device structure
 * @chip_ptr: pointer point to spi nand device structure pointer
 */
int spi_nand_init(void)
{
	uint8_t id[SPINAND_MAX_ID_LEN] = {0};
	struct spi_flash_chip *chip = &nand_chip;

	memset(chip, 0, sizeof(struct spi_flash_chip));
	chip->table = cmd_table;
	chip->size = 64*1024*1024;
	chip->page_shift = 11;
	chip->page_size = 2*1024;
	chip->page_mask = chip->page_size - 1;
	chip->block_size = 64*2*1024;
	chip->block_shift = 17;

	spi_nand_set_rd_wr_op(chip, SPI_OPM_RX, SPI_OPM_TX);
	spi_nand_reset(chip);
	spi_nand_read_id(chip, id);
	spi_nand_enable_quad(chip);
	bbu_printf("SPI-NAND type mfr_id: %x, dev_id: %x\n", id[0], id[1]);

	return 0;
}
