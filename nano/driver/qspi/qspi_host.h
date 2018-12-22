#ifndef _QSPI_HOST_H
#define _QSPI_HOST_H
#include "qspi_common.h"
#include "spi_flash.h"

struct spi_flash_cmd;

enum {
	QSPI_NORMAL_MODE,
	QSPI_DISABLE_MODE,
	QSPI_STOP_MODE,
};

enum {
	QSPI_FUNC_CLK_26MHZ,
	QSPI_FUNC_CLK_52MHZ,
	QSPI_FUNC_CLK_78MHZ,
	QSPI_FUNC_CLK_104MHZ,
	QSPI_FUNC_CLK_156MHZ,
	QSPI_FUNC_CLK_208MHZ,
	QSPI_FUNC_CLK_312MHZ,
	QSPI_FUNC_CLK_416MHZ,
};

#define QSPI_RX_BUFF_MAX 32
#define QSPI_TX_BUFF_MAX 32
#define QSPI_TX_BUFF_POP_MIN 16

int qspi_config_lookup_table(struct spi_flash_cmd *cmd,
				struct spi_flash_cmd_cfg *cmd_cfg);
int qspi_host_init(void);
int qspi_start_cmd(struct spi_flash_cmd *cmd, int seq_id);

#endif