#include "bbu.h"
#include "timer_dec.h"
#include "qspi_host.h"
#include "spi_nand.h"
#include "utils.h"
#include "PMUA.h"

static void qspi_enter_mode(uint32_t mode)
{
	uint32_t mcr;

	mcr = _readl(QSPI0_MCR);
	if (mode == QSPI_NORMAL_MODE)
		mcr &= ~(0x1 << 14);
	else if (mode == QSPI_DISABLE_MODE)
		mcr |= (0x1 << 14);
	_writel(mcr, QSPI0_MCR);
}

static void qspi_reset(void)
{
	uint32_t reg;
	uint32_t i;

	/* qspi softreset first */
	reg = _readl(QSPI0_MCR);
	reg |= 0x3;
	_writel(reg, QSPI0_MCR);
	for(i=0;i<20;i++); //mdelay(10);
	reg &= ~0x3;
	_writel(reg, QSPI0_MCR);	
}

static void qspi_clk_enable(void)
{
	PMUA->QSPI_CLK_RES_CTRL |= 0x1 << 4 | 0x1 << 3;;
	PMUA->QSPI_CLK_RES_CTRL |= 0x3;
}

static void qspi_clk_disable(void)
{
	PMUA->QSPI_CLK_RES_CTRL &= ~(0x1 << 4 | 0x1 << 3);
	PMUA->QSPI_CLK_RES_CTRL &= ~0x3;
}

static void qspi_set_func_clk(int freq)
{
	int clk_sel;

	qspi_clk_disable();

	switch(freq) {
	case 26000000:
		clk_sel = QSPI_FUNC_CLK_26MHZ;
		break;
	case 52000000:
		clk_sel = QSPI_FUNC_CLK_52MHZ;
		break;
	case 78000000:
		clk_sel = QSPI_FUNC_CLK_78MHZ;
		break;
	case 104000000:
		clk_sel = QSPI_FUNC_CLK_104MHZ;
		break;
	case 156000000:
		clk_sel = QSPI_FUNC_CLK_156MHZ;
		break;
	case 208000000:
		clk_sel = QSPI_FUNC_CLK_208MHZ;
		break;
	case 312000000:
		clk_sel = QSPI_FUNC_CLK_312MHZ;
		break;
	case 416000000:
		clk_sel = QSPI_FUNC_CLK_416MHZ;
		break;
	default:
		clk_sel = QSPI_FUNC_CLK_26MHZ;
		break;

	}

	PMUA->QSPI_CLK_RES_CTRL &= ~(0x7 << 6);
	PMUA->QSPI_CLK_RES_CTRL |= clk_sel << 6;

	qspi_clk_enable();
}

int qspi_host_init(void)
{
	uint32_t reg;

	/* Enable qspi clk, and release reset */
	qspi_set_func_clk(26000000);
	//_writel(0x30, 0xd4282844);
	//_writel(0x30, 0xd4282868);

	/* qspi softreset first */
	qspi_reset();
	/*
	 * clock settings
	 */
	qspi_enter_mode(QSPI_DISABLE_MODE);
	/* sampled by sfif_clk_b; half cycle delay; */
	_writel(0x0, QSPI0_SMPR);
	_writel(0x202, QSPI0_FLSHCR);

	/* Give the default source address */
	_writel(QSPI0_FLASH_A1_BASE, QSPI0_SFAR);

	/* config the ahb buffer */
	_writel(0x20, QSPI0_BUF0IND);
	_writel(0x40, QSPI0_BUF1IND);
	_writel(0x60, QSPI0_BUF2IND);

	/* AHB Master port */
	_writel(0x1000, QSPI0_BUF0CR); // CPU A9
	_writel(0x1006, QSPI0_BUF1CR); // CPU M4
	_writel(0x1003, QSPI0_BUF2CR); // SDMA
	_writel(0x80001002, QSPI0_BUF3CR); // Other masters

	/* Set flash memory map */
	_writel(QSPI0_FLASH_A1_TOP & 0xfffffc00, QSPI0_SFA1AD);
	_writel(QSPI0_FLASH_A2_TOP & 0xfffffc00, QSPI0_SFA2AD);
	_writel(QSPI0_FLASH_B1_TOP & 0xfffffc00, QSPI0_SFB1AD);
	_writel(QSPI0_FLASH_B2_TOP & 0xfffffc00, QSPI0_SFB2AD);

	/*
	 * ISD3FB, ISD2FB, ISD3FA, ISD2FA = 1; ENDIAN = 0x3; END_CFG=0x3 
	 * DELAY_CLK4X_EN = 1
	 */
	reg = _readl(QSPI0_MCR);
	reg &= ~0x000f000c;
	reg |= 0x000f000c;
	_writel(reg, QSPI0_MCR);

	/* Module enabled */
	qspi_enter_mode(QSPI_NORMAL_MODE);

	/* Read using the IP Bus registers QSPI_RBDR0 to QSPI_RBDR31*/
	_writel(0x1 << 8, QSPI0_RBCT);
	return 0;
}

static int qspi_find_free_lut(void)
{
	return 1;
}

static void qspi_lock_lut(void)
{
	uint32_t lckcr;

	lckcr = _readl(QSPI0_LCKCR);
	if (lckcr & 0x1)
		return;
	_writel(0x5af05af0, QSPI0_LUTKEY);
	_writel(0x1, QSPI0_LCKCR);
}

static void qspi_unlock_lut(void)
{
	uint32_t lckcr;

	lckcr = _readl(QSPI0_LCKCR);
	if (lckcr & 0x2)
		return;

	_writel(0x5af05af0, QSPI0_LUTKEY);
	_writel(0x2, QSPI0_LCKCR);
}

int qspi_config_lookup_table(struct spi_flash_cmd *cmd,
				struct spi_flash_cmd_cfg *cmd_cfg)
{
	uint8_t pins[] = {0, QSPI_PAD_1X, QSPI_PAD_2X, 0, QSPI_PAD_4X};
	uint16_t lut_entry[8];
	uint32_t seq_id;
	uint32_t lut_value;
	uint8_t seq = 0, i;
	uint32_t reg;
	uint8_t mode_instr;
	uint8_t dummy_cycles;

	seq_id = qspi_find_free_lut();
	if (seq_id < 0)
		return -ENODEV;

	/* Reset the IP sequence pointers */
	reg = _readl(QSPI0_SPTRCLR);
	reg |= 0x1 << 8;
	_writel(reg, QSPI0_SPTRCLR);

	/* 
	 * Set Lookup table entry: CMD, ADDR, MODE, DUMMY, DATA, etc
	 */
	if (cmd->qpi_enabled)
		lut_entry[seq++] = QSPI_INSTR_CMD << 10 |
				   QSPI_PAD_4X << 8 | cmd->cmd;
	else
		lut_entry[seq++] = QSPI_INSTR_CMD << 10 |
				   QSPI_PAD_1X << 8 | cmd->cmd;
	if (cmd_cfg->addr_bytes)
		lut_entry[seq++] = QSPI_INSTR_ADDR << 10 |
				   pins[cmd_cfg->addr_pins] << 8 |
				   cmd_cfg->addr_bytes*8;
	if (cmd_cfg->mode_bits) {
		if (cmd_cfg->mode_bits == 2) {
			mode_instr = QSPI_INSTR_MODE2;
			cmd->mode &= 0x3;
		} else if (cmd_cfg->mode_bits == 4) {
			mode_instr = QSPI_INSTR_MODE4;
			cmd->mode &= 0xf;
		} else {
			mode_instr = QSPI_INSTR_MODE;
		}
		lut_entry[seq++] = mode_instr << 10 |
				   pins[cmd_cfg->mode_pins] << 8 |
				   cmd->mode;
	}
	if (cmd_cfg->dummy_cycles) {
		//if (cmd->qpi_enabled)
		//	dummy_cycles = cmd->qpi_dummy_cycles;
		//else
		//	dummy_cycles = cmd_cfg->dummy_cycles;
		dummy_cycles = cmd_cfg->dummy_cycles;
		//if (cmd->qpi_enabled && cmd_cfg->mode_bits)
		//	dummy_cycles -= cmd_cfg->mode_bits/cmd_cfg->mode_pins;

		lut_entry[seq++] = QSPI_INSTR_DUMMY << 10 |
				   pins[cmd_cfg->dummy_pins] << 8 |
				   dummy_cycles;
	}
	if (cmd->rx_buf) {
		lut_entry[seq++] = QSPI_INSTR_READ << 10 |
				   pins[cmd_cfg->data_pins] << 8 |
				   (cmd->n_rx & 0xff);
		/* Add JMP_ON_CS for read */
		lut_entry[seq++] = QSPI_INSTR_JMP_ON_CS << 10;

	} else if (cmd->tx_buf) {
		lut_entry[seq++] = QSPI_INSTR_WRITE << 10 |
				pins[cmd_cfg->data_pins] << 8 |
				(cmd->n_tx & 0xff);		
	}
	/* Add stop at the end */
	lut_entry[seq++] = QSPI_INSTR_STOP << 10;

	for (i = 0; i < seq/2; i++) {
		lut_value = lut_entry[i*2] | (lut_entry[i*2 + 1] << 16);
		_writel(lut_value, QSPI0_LUT0 + seq_id*0x10 + i*0x4);
	}
	if (seq % 2) {
		lut_value =lut_entry[seq - 1];
		_writel(lut_value, QSPI0_LUT0 + seq_id*0x10 + (seq/2)*0x4);
	}

	//for (i = 0; i < seq; i++)
	//	bbu_printf("seq=%d, lut_entry[%d]=0x%x\n\r", seq, i, lut_entry[i]);

	for(i = 0; i < 4; i++) {
		bbu_printf("0x%x : 0x%x\r\n", QSPI0_LUT0 + seq_id*0x10 + i*0x4,
			_readl(QSPI0_LUT0 + seq_id*0x10 + i*0x4));
	}

	return seq_id;
}

static void qspi_wait_cmd(struct spi_flash_cmd *cmd)
{
	uint32_t fr;

	do {
		fr = _readl(QSPI0_FR);
		if (fr & (0x1 << 23)) {
			bbu_printf("Err: Illegal Instruction Error Flag\r\n");
			break;
		}
		if (fr & (0x1 << 11)) {
			bbu_printf("Err: IP Command Usage Error Flag\r\n");
			break;
		}
		if (fr & (0x1 << 7)) {
			bbu_printf("Err: IP Command Trigger during AHB Access Error Flag\r\n");
			break;
		}
		if (fr & (0x1 << 6)) {
			bbu_printf("Err: IP Command Trigger could not be executed Error Flag\r\n");
			break;
		}
		if (fr & (0x1 << 4)) {
			bbu_printf("Err: IP Command Trigger during AHB Grant Error Flag\r\n");
			break;
		}
		if (fr & 0x1) {
			_writel(0x1, QSPI0_FR);
			break;
		}
	} while(1);

	if (cmd->rx_buf && (fr & (0x1 << 17)))
		bbu_printf("Error: RX Buffer Overflow\r\n");
	if (cmd->tx_buf && (fr & (0x1 << 26)))
		bbu_printf("Error: TX Buffer Underrun Flag\r\n");
}

static void qspi_poll_rx_buff(struct spi_flash_cmd *cmd, int wmrk)
{
	uint32_t rx_bytes = 0;
	uint32_t sr;
	uint32_t data;
	uint32_t rdbfl;
	int i;

	//bbu_printf("%s: %d rx_len=0x%x\r\n", __func__, __LINE__, cmd->n_rx);
	do {
		/* Check RXWE flag for data comming */
		sr = _readl(QSPI0_FR);
		if (sr & (0x1 << 16)) {
			for (i = 0; i <= wmrk; i++) {
				data = _readl(QSPI0_RBDR0 + i*4);
				if (cmd->n_rx - rx_bytes >= 4) {
					memcpy(&cmd->rx_buf[rx_bytes], &data, 4);
					rx_bytes += 4;
				} else {
					memcpy(&cmd->rx_buf[rx_bytes], &data,
						cmd->n_rx - rx_bytes);
					rx_bytes = cmd->n_rx;
				}
			}
			/* Set RBDF to trigger RX Buffer POP */
			_writel(0x1 << 16, QSPI0_FR);

			if (rx_bytes == cmd->n_rx) {
				break;
			} else if ((cmd->n_rx - rx_bytes + 3)/4 < (wmrk + 1)) {
				/* Left bytes < wmrk will not trigger RXWE */
				//bbu_printf("cmd->n_rx=%d, rx_bytes=%d, wmrk=%d\r\n",
				//	cmd->n_rx, rx_bytes, wmrk);
				break;
			}
		}
	} while(1);

	/* Wait cmd to be finished */
	qspi_wait_cmd(cmd);
	if (rx_bytes < cmd->n_rx) {
		rdbfl = _readl(QSPI0_RBSR);
		rdbfl = (rdbfl >> 8) & 0x3f;
		for (i = 0; i <= rdbfl; i++) {
			data = _readl(QSPI0_RBDR0 + i*4);
			if (cmd->n_rx - rx_bytes >= 4) {
				memcpy(&cmd->rx_buf[rx_bytes], &data, 4);
				rx_bytes += 4;
			} else {
				memcpy(&cmd->rx_buf[rx_bytes], &data,
					cmd->n_rx - rx_bytes);
				rx_bytes = cmd->n_rx;
			}
		}
	}

	if (rx_bytes < cmd->n_rx) {
		bbu_printf("Error: Not read enough data: rx_bytes=%d, cmd->n_rx=%d\r\n",
			rx_bytes, cmd->n_rx);
	}
	return;
}

static void qspi_fill_tx_buff(struct spi_flash_cmd *cmd, int sent_cnt)
{
	int left_bytes;
	int tx_cnt;
	uint32_t reg;
	uint32_t data;

	tx_cnt = (cmd->n_tx + QSPI_TX_BUFF_POP_MIN - 1) / QSPI_TX_BUFF_POP_MIN;
	tx_cnt = (tx_cnt*QSPI_TX_BUFF_POP_MIN)/4;

	while (sent_cnt < tx_cnt) {
		reg = _readl(QSPI0_SR);
		if (!(reg & (0x1 << 27))) {
			left_bytes = cmd->n_tx - sent_cnt*4;

			if (left_bytes <= 0) {
				data = 0;
			} else if (left_bytes < 4) {
				data = 0;
				memcpy(&data, &cmd->tx_buf[sent_cnt*4],
					left_bytes);
			} else {
				data = (*(uint32_t *)&cmd->tx_buf[sent_cnt*4]);
			}
			_writel(data, QSPI0_TBDR);
			//bbu_printf("%s: %d: QSPI0_TBDR 0x%x\r\n", __func__, sent_cnt, data);
			sent_cnt++;
		}
	}

	qspi_wait_cmd(cmd);
	return;
}

int qspi_start_cmd(struct spi_flash_cmd *cmd, int seq_id)
{
	uint32_t tmp;
	uint32_t ipcr;
	uint32_t sfar;
	uint32_t mcr;
	uint32_t data;
	int tx_cnt;
	int wmrk = 0, i;
	int left_bytes;

	if (cmd->n_rx) {
		/* Clear RX FIFO. Invalidate the RX Buffer */
		mcr = _readl(QSPI0_MCR);
		mcr |= 0x1 << 10;  //CLR_RXF
		_writel(mcr, QSPI0_MCR);

		/* Set RX Buffer Watermark */
		if (cmd->n_rx <= (QSPI_RX_BUFF_MAX*4))
			wmrk = (cmd->n_rx + 3)/4 - 1;
		else
			wmrk = 0xf; /* Water Mark: 16*4byte */
		tmp = _readl(QSPI0_RBCT);
		tmp &= ~0x1f;
		tmp |= wmrk;
		_writel(tmp, QSPI0_RBCT);
		//bbu_printf("%s: wmrk=%d tmp=0x%x\r\n", __func__, wmrk, tmp);
	} else if (cmd->n_tx) {
		/* Clear TX FIFO/Buffer */
		mcr = _readl(QSPI0_MCR);
		mcr |= 0x1 << 11;  //CLR_TXF
		_writel(mcr, QSPI0_MCR);
		/* Set QSPI_SFACR[CAS] QSPI_SFACR[WA] if needed */

		tx_cnt = (cmd->n_tx + QSPI_TX_BUFF_POP_MIN - 1) / QSPI_TX_BUFF_POP_MIN;
		tx_cnt = (tx_cnt*QSPI_TX_BUFF_POP_MIN)/4;
		/*
		 * Provide initial data for the program
		 * command into the circular buffer
		 */
		if (tx_cnt > QSPI_TX_BUFF_MAX)
			tx_cnt = QSPI_TX_BUFF_MAX;

		for (i = 0; i < tx_cnt; i++) {
			left_bytes = cmd->n_tx - i*4;
			if (left_bytes <= 0) {
				data = 0;
			} else if (left_bytes < 4) {
				data = 0;
				memcpy(&data, &cmd->tx_buf[i*4], left_bytes);
			} else {
				data = (*(uint32_t *)&cmd->tx_buf[i*4]);
			}
			_writel(data, QSPI0_TBDR);
			//bbu_printf("%s: %d: QSPI0_TBDR 0x%x\r\n", __func__, i, data);
		}
	}

	/* Set flash address to be accessed */
	sfar = QSPI0_FLASH_A1_BASE;
	for (i = 0; i < cmd->n_addr; i++)
		sfar |= cmd->addr[i] << (i*8);
	_writel(sfar, QSPI0_SFAR);
	//bbu_printf("QSPI0_SFAR: 0x%x\r\n", _readl(QSPI0_SFAR));

	/* trigger command */
	ipcr = (seq_id & 0xf) << 24;
	if (cmd->rx_buf)
		ipcr |= (cmd->n_rx & 0xffff);
	else if (cmd->tx_buf)
		ipcr |= (cmd->n_tx & 0xffff);
	_writel(ipcr, QSPI0_IPCR);

	/* receive rx data */
	if (cmd->n_rx)
		qspi_poll_rx_buff(cmd, wmrk);
	else if (cmd->n_tx)
		qspi_fill_tx_buff(cmd, tx_cnt);
	else
		qspi_wait_cmd(cmd);
	return 0;
}
