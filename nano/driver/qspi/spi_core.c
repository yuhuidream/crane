#include "bbu.h"
#include "qspi_common.h"
#include "qspi_host.h"
#include "spi_flash.h"
#include "utils.h"
#include "spi_nand.h"
#include "spi_nor.h"

static struct spi_flash_cmd_cfg *spi_flash_lookup_cmd_cfg_table(uint8_t opcode,
				struct spi_flash_cmd_cfg *table)
{
	struct spi_flash_cmd_cfg *index = table;

	for (; index->opcode != SPINAND_CMD_END; index++) {
		if (index->opcode == opcode)
			return index;
	}

	bbu_printf("Invalid spi nand opcode %x\n", opcode);

	return NULL;
}

/*
 * spi_flash_issue_cmd - to process a command to send to the SPI-NAND
 * @spi: spi device structure
 * @cmd: command structure
 *
 *    Set up the command buffer to send to the SPI controller.
 *    The command buffer has to initialized to 0.
 */
int spi_flash_issue_cmd(struct spi_flash_chip *chip, struct spi_flash_cmd *cmd)
{
	struct spi_flash_cmd_cfg *cmd_cfg = NULL;
	int seq_id;

	cmd_cfg = spi_flash_lookup_cmd_cfg_table(cmd->cmd, chip->table);
	if (!cmd_cfg)
		return -EINVAL;

	if (chip->qpi_enabled) {
		cmd->qpi_enabled = chip->qpi_enabled;
		cmd->qpi_dummy_cycles = chip->qpi_dummy_cycles;
	}

	seq_id = qspi_config_lookup_table(cmd, cmd_cfg);
	if (seq_id < 0)
		return seq_id;
	return qspi_start_cmd(cmd, seq_id);
}

int bbu_qspi_test(int argc, uint8_t **argv)
{
	struct spi_flash_chip *chip;
	int i;

	qspi_host_init();

	chip = spi_nor_init();
	if (!chip) {
		bbu_printf("SPI Nor flash init failed\r\n");
		return -1;
	}

	//spi_nor_test(chip);
	//spi_nor_test_qpi(chip);
	return 0;
}

int bbu_qspi_test2(void)
{
	struct spi_flash_chip *chip;

	qspi_host_init();

	chip = spi_nor_init();
	if (!chip) {
		bbu_printf("SPI Nor flash init failed\r\n");
		return -1;
	}

	//spi_nor_test(chip);
	return 0;
}

