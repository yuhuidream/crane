/*
 *  sdh_host.c
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */

#include "bbu.h"
#include "timer_dec.h"
#include "bbu_malloc.h"
#include "interrupt.h"
#include "pmic.h"
#include "PMUA.h"
#include "cache_ops.h"
#include "gpio_dec.h"
#include "scatterlist.h"
#include "sdh_def.h"
#include "sdh_device.h"
#include "sdh_host.h"
#include "mmc.h"
#include "sdh_mmc.h"
#include "AIB.h"
#include "APBCLK.h"

/* internal use function */
static uint32_t mrvl_sdh_release_desc_table(struct dev_request *drq);
static uint32_t mrvl_sdh_dma_select(struct sdhc_host *host, uint32_t dma_mode);
static uint32_t mrvl_sdh_prepare_pio(struct sdhc_host *host);
static uint32_t mrvl_sdh_pmu_reset_peripheral(struct sdhc_host *host);
static uint32_t mrvl_sdh_pmu_release_peripheral_reset(struct sdhc_host *host);

static uint8_t adma_descriptor_buffer[ADMA_DESCRIPTOR_BUF_SIZE];

static uint32_t sdh_clk_source[SDH_CLOCK_SOURCE_NUM] = 
{
	[SDH_CLOCK_SOURCE_624MHZ] = 624000000,
	[SDH_CLOCK_SOURCE_416MHZ] = 416000000,
};

static struct sdhc_host emei_sdh[MRVL_SDH_SLOT_NUM] =
{
	[SD_CARD_SLOT_NUM] = 
	{
		.base_addr = SD0_BASE,
		.cap = SDH_HOST_CAP_1V2 | SDH_HOST_CAP_1V8,
		.card_num = 0,
		.clk_source = SDH_CLOCK_SOURCE_416MHZ,  /* default use 416MHz as clock source */
		.first_div = 2,  /* default base clock setted to 416MHz/2 = 208MHZ */
		.base_clk_ready = 0,  /* default base clock is shut down */
		.base_shared = 0,  /* eden base clock is shared among all these SDH slots */
		.max_first_div = 8,
		.max_second_div = 0x3ff * 2,
		.expected_rx_window = MAX_VALID_TUNING_WINDOW,
		.expected_tx_window = MAX_VALID_TUNING_WINDOW,
		.def_timing_sdr50 = 0x40,
	},
	
	[EMMC_CARD_SLOT_NUM] = 
	{
		.base_addr = SD2_BASE,
		.cap = SDH_HOST_CAP_1V8,
		.card_num = 0,
		.clk_source = SDH_CLOCK_SOURCE_416MHZ,  /* default use 416MHz as clock source */
		.first_div = 2,  /* default base clock setted to 416MHz/2 = 208MHZ */
		.base_clk_ready = 0,  /* default base clock is shut down */
		.base_shared = 0,  /* eden base clock is shared among all these SDH slots */
		.max_first_div = 8,
		.max_second_div = 0x3ff * 2,
		.expected_rx_window = MAX_VALID_TUNING_WINDOW,
		.expected_tx_window = MAX_VALID_TUNING_WINDOW,
		.def_timing_ddr50 = 0x7f << 16 | 0x0,
		.def_timing_sdr104 =  0x7d << 16 | 0x44,
		.tx_tune_step = 1,
		.rx_tune_step = 10,
	},
	
	[SDIO_CARD_SLOT_NUM] = 
	{
		.base_addr = SD1_BASE,
		.card_num = 0,
		.clk_source = SDH_CLOCK_SOURCE_416MHZ,  /* default use 416MHz as clock source */
		.first_div = 2,  /* default base clock setted to 416MHz/2 = 208MHZ */
		.base_clk_ready = 0,  /* default base clock is shut down */
		.base_shared = 0,  /* eden base clock is shared among all these SDH slots */
		.max_first_div = 8,
		.max_second_div = 0x3ff * 2,
		.expected_rx_window = MAX_VALID_TUNING_WINDOW,
		.expected_tx_window = MAX_VALID_TUNING_WINDOW,
	},	      	      
} ;
//struct sdhc_host *sdh = &emei_sdh;

struct sdhc_host *get_sdh_host(void)
{
	return (struct sdhc_host *)emei_sdh;
} 

static uint32_t sdh_update_clock_info(struct sdhc_host *host)
{
	struct sdhc_host *host_base = get_sdh_host();
	struct sdhc_host *host_temp;
	uint32_t i;
	
	if (host->base_shared) {
		for (i = 0; i < MRVL_SDH_SLOT_NUM; i++) {
			host_temp = host_base + i;
			if (host_temp != host) {
				host_temp->base_clk = host->base_clk;
				host_temp->clk_source = host->clk_source;
				host_temp->first_div = host->first_div;
			}	
		}
		DEBUG_SDH_PRINT("Warning: base clock is shared, will affect all SDH controller!\r\n");
	}
	return SDH_TRUE;
}

static uint32_t mrvl_sdh_send_command(struct sdhc_host *host, struct dev_command *cmd)
{
	uint16_t cmdreg;
	uint16_t resp_type;
	uint16_t crc_en;
	uint16_t have_data;
	uint16_t cmd_type = 0;
	uint16_t cmd_index_check_en;
	
	/* 
	 * Set response type, index check enable, CRC Check enable in command register 
	 */
	switch (dev_resp_type(cmd)) {
	case DEV_RSP_NONE:
		resp_type = 0;
		crc_en = 0;
		cmd_index_check_en = 0;
		break;

	case DEV_RSP_R1: /* For R1, R4, R5, R6, R7, resp_type = 2 */
		resp_type = 2;
		crc_en = 1;
		cmd_index_check_en = 1;
		break;

	case DEV_RSP_R2:
		resp_type = 1;
		crc_en = 1;
		cmd_index_check_en = 0;		
		break;

	case DEV_RSP_R3:
		resp_type = 2;
		crc_en = 0;
		cmd_index_check_en = 0;
		break;

	case DEV_RSP_R1B:
		resp_type = 3;
		crc_en = 1;
		cmd_index_check_en = 1;		
		break;
		
	default:
		resp_type = 0;
		crc_en = 0;
		cmd_index_check_en = 0;
		DEBUG_SDH_PRINT("err: not supported response type\r\n");
		break;
	}
	
	if (cmd->opcode == COM_CMD12_STOP_TRANSMISSION) {
		cmd_type = 3;
	} else {
		cmd_type = 0;
	}

	/* Data present select */
	if (dev_cmd_type(cmd) == DEV_CMD_ADTC)
			have_data = 1;
	else
			have_data = 0;

	cmdreg = (resp_type) | (crc_en << 3) | (cmd_index_check_en << 4) |
				(have_data << 5) | (cmd_type << 6) | ((cmd->opcode & 0x3f) << 8);

	/* set argument of command */
	MRVL_SDH_REG32_WRITE(host->base_addr, ARG_LOW, cmd->arg);  
	DEBUG_SDH_PRINT("CMD%d: argument -- 0x%x\r\n", cmd->opcode, cmd->arg);
	
	/* Write back to kick off command */
	MRVL_SDH_REG16_WRITE(host->base_addr, CMD, cmdreg);

	return SDH_TRUE;
}

uint32_t mrvl_sdh_xfer_done(struct sdhc_host *host)
{
	struct dev_data *data = host->drq->data;
	
	if ((data != NULL) && (host->drq->trans_mode & SDH_TRANS_TYPE_ADMA)) {
		mrvl_sdh_release_desc_table(host->drq);
	}	
		
	/* Send result back to A0_SDH */
	if ((host->drq->done) != NULL) {
		DEBUG_SDH_PRINT("[Begin]: (host->drq->done)(host->drq)\r\n");
		(host->drq->done)(host->drq);
	}
	
	host->dma_in_use = 0;
	host->transfer_complete = 1;
	host->drq = NULL;	
	
	return SDH_TRUE;
}

uint32_t mrvl_sdh_end_of_data(struct sdhc_host *host) 
{
	struct dev_request *drq = host->drq;
	struct dev_command *cmd = drq->cmd;
	struct dev_data *data = drq->data;

	if (cmd->opcode == COM_CMD12_STOP_TRANSMISSION) {
		uint8_t timeout = 50;

		mrvl_sdh_softreset_cmd_line(host);
		mrvl_sdh_softreset_data_line(host);

		do {
			if (mrvl_sdh_host_data_level(host) == 0x0f)
				break;
			if (!timeout) {
				DEBUG_SDH_PRINT("Error: PRESENT_STATE_2=0x%x after CMD12!\r\n", 
					MRVL_SDH_REG16_READ(host->base_addr, PRESENT_STATE_2));
				break;
			}
			udelay(1);
			timeout--;
		} while(1);
	}

	if (host && host->dma_in_use) {
		host->total_bytes_left = 0;
	}

	if (host != NULL && drq != NULL && data != NULL) {		
		if (cmd->auto_cmd & SDH_AUTO_CMD12) {
			cmd->resp[1] = MRVL_SDH_REG16_READ(host->base_addr, RESP_7) << 16 | 
			               MRVL_SDH_REG16_READ(host->base_addr, RESP_6);
		}	
	}

	if (data != NULL) {
		DEBUG_SDH_PRINT("CMD%d: %u blocks transfer completed\r\n", 
		    drq->cmd->opcode, data->blocks);
	}

	mrvl_sdh_xfer_done(host);	

	return SDH_TRUE;
}

uint32_t mrvl_sdh_pio_xfer_data(struct sdhc_host *host)
{
	struct dev_data *data = host->drq->data;
	struct scatterlist *sg;
	uint32_t *pdata;
	uint32_t i;
	uint32_t total_bytes = (data->blocks) * (data->blksz);
	uint32_t consumed_bytes = total_bytes - host->total_bytes_left;

	for_each_sg(data->sg, sg, data->sg_len, i) {
		if (consumed_bytes < sg->length)
			break;
		else 
			consumed_bytes -= sg->length;
	}

	if (data->flags & DEV_DATA_READ) {
		do {
			pdata = (uint32_t *)(sg->base_address) + consumed_bytes/4 ;
			for (i = 0; i < data->blksz/4; i++) {
				uint32_t rd_dat;
			
				rd_dat = MRVL_SDH_REG32_READ(host->base_addr, BUFFER_DATA_PORT_0);	
				/* If target address is not aligned to 4 bytes, read in bytes */
				if ((uint32_t)(pdata + i) & 0x3) {
					*((uint8_t *)(pdata + i)) = (uint8_t)(rd_dat & 0x000000ff);
					*((uint8_t *)(pdata + i) + 1) = (uint8_t)((rd_dat & 0x0000ff00) >> 8);
					*((uint8_t *)(pdata + i) + 2) = (uint8_t)((rd_dat & 0x00ff0000) >> 16);
					*((uint8_t *)(pdata + i) + 3) = (uint8_t)((rd_dat & 0xff000000) >> 24);
				
				} else {
					*(pdata + i)= rd_dat;
				}	
				{
					/* Fix PIO large data read timeout error */
					uint32_t j;
					for (j = 0; j < 20; j++) {
						__nop();
					}
				}
			}
			host->total_bytes_left -= data->blksz;	
			consumed_bytes += data->blksz;
			if (consumed_bytes >= sg->length) {
				consumed_bytes = 0;
				sg = sg_next(sg);
			}
		} while (host->total_bytes_left && 
			 MRVL_SDH_REG32_READ(host->base_addr, PRESENT_STATE_1) &
		                             PRESENT_STATE_1_BUFFER_RD_EN);
	} else if (data->flags & DEV_DATA_WRITE) {

		while (MRVL_SDH_REG32_READ(host->base_addr, PRESENT_STATE_1) & 
		       PRESENT_STATE_1_BUFFER_WR_EN) {
			pdata = (uint32_t *)(sg->base_address) + consumed_bytes/4 ;
			for (i = 0; i < data->blksz/4; i++) {	
				uint32_t wr_dat;
			
				/* If target address is not aligned to 4 bytes, write in bytes */
				if ((uint32_t)(pdata + i) & 0x3) {
					wr_dat = (uint32_t)(*((uint8_t *)(pdata + i)));
					wr_dat |= (uint32_t)(*((uint8_t *)(pdata + i) + 1)) << 8;
					wr_dat |= (uint32_t)(*((uint8_t *)(pdata + i) + 2)) << 16;
					wr_dat |= (uint32_t)(*((uint8_t *)(pdata + i) + 3)) << 24;
				
				} else {
					wr_dat = *(pdata + i);
				}
			
				MRVL_SDH_REG32_WRITE(host->base_addr, BUFFER_DATA_PORT_0, wr_dat);			
			}	
			host->total_bytes_left -= data->blksz;	
			consumed_bytes += data->blksz;
			if (consumed_bytes >= sg->length) {
				consumed_bytes = 0;
				sg = sg_next(sg);
			}
		}
	}

	if (host->total_bytes_left) {
		return SDH_FALSE;
	}

	return SDH_TRUE;
}

static uint32_t mrvl_sdh_create_desc_table(struct dev_request *drq)
{
	struct dev_data *data ;
	struct scatterlist *sg;
	uint32_t desc_num;
	uint32_t total_bytes;	
	uint32_t i, j;
	adma_descriptor_line_t *p_record;
	uint16_t bytes_per_desc;
	
	data = drq->data;
	if (data->byte_per_desc == 0) {
		data->byte_per_desc = SD_ADMA_MAX_BYTES_PER_DESC;
	}
	/* 
	 * For up to 65536 bytes, length area of descriptor should be zero
	 */
	bytes_per_desc = (data->byte_per_desc == SD_ADMA_MAX_BYTES_PER_DESC) ? 0 : (data->byte_per_desc);	
	desc_num = 0;
	
	for_each_sg(data->sg, sg, data->sg_len, j) {	
		total_bytes = sg->length;
		desc_num += total_bytes / (data->byte_per_desc);
		if (total_bytes % (data->byte_per_desc))
			desc_num++;	
	}
	DEBUG_SDH_PRINT("\r\nTotal %d scatterlists\r\n", j);
	
	data->descriptor_num = desc_num;
	/* allocate one more descriptor for align use */
	//p_record = bbu_malloc(sizeof(adma_descriptor_line_t) * (desc_num + 1));	
	//data->desc_malloc_buffer = p_record;
	if (sizeof(adma_descriptor_line_t) * (desc_num + 1) > ADMA_DESCRIPTOR_BUF_SIZE ) {
		bbu_printf("Error: ADMA descriptort buffer not enough\r\n");
		DEBUG_SDH_PRINT("desc_num + 1=%d, sizeof(adma_descriptor_line_t)=%d\r\n", 
		    desc_num + 1, sizeof(adma_descriptor_line_t));
		return SDH_FALSE;
	} else {
		p_record = (adma_descriptor_line_t *)adma_descriptor_buffer;
	}


	/* ADMA require address of descriptor table to be 4bytes aligin */
	if (((uint32_t)p_record % 4) !=0) {
		uint32_t record_temp;
		p_record++;
		record_temp = (uint32_t)p_record;
		record_temp &= 0x0FFFFFFFC;
		p_record = (adma_descriptor_line_t *)record_temp;
	}
	data->first_descriptor = p_record;
			
	DEBUG_SDH_PRINT("\r\nTotal descriptor number: %d\r\n", desc_num);
	DEBUG_SDH_PRINT("\r\nDescriptor location: 0x%x\r\n", p_record);

	for_each_sg(data->sg, sg, data->sg_len, j) {	
		total_bytes = sg->length;
		desc_num = total_bytes / (data->byte_per_desc);
		if (total_bytes % (data->byte_per_desc))
			desc_num++;	

		for (i = 0; i < desc_num; i++) {

			if (i == desc_num - 1) {
				if (j == data->sg_len - 1)
					(p_record + i)->end = 1;
				else
					(p_record + i)->end = 0;
				(p_record + i)->len = total_bytes - ((desc_num-1) * data->byte_per_desc);
			} else {
				(p_record + i)->len = bytes_per_desc;	
				(p_record + i)->end = 0;
			}

			if (i == 0) {
				(p_record + i)->address = (uint32_t)(sg->base_address);
			} else if (i >= 1) {
				(p_record + i)->address = (p_record + i-1)->address + data->byte_per_desc;
			}
		
			(p_record + i)->dma_int = 0;			
			(p_record + i)->reserved1 = 0;
			(p_record + i)->reserved2 = 0;
			(p_record + i)->valid = 1;
			(p_record + i)->act = SD_ADMA_ATTR_TRAN;	
#if 0
			{
				uint32_t resp[4];
				uint32_t *desc_temp;
			
				desc_temp = (uint32_t *)(p_record + i);		
				resp[2] = *(desc_temp + 1);
				resp[3] = *desc_temp;

				bbu_printf("The %dth descriptor:\r\n", i + 1);
				bbu_printf("Valid: 0x%x\r\n", UNSTUFF_BITS(resp, 0, 1));
				bbu_printf("End: 0x%x\r\n", UNSTUFF_BITS(resp, 1, 1));
				bbu_printf("Int: 0x%x\r\n", UNSTUFF_BITS(resp, 2, 1));
				bbu_printf("Act: 0x%x\r\n", UNSTUFF_BITS(resp, 4, 2));
				bbu_printf("Length: 0x%x\r\n", UNSTUFF_BITS(resp, 16, 16));
				bbu_printf("Address: 0x%x\r\n", UNSTUFF_BITS(resp, 32, 32));
			}
#endif
		}
		p_record += desc_num;
	}

	dcache_clean_range((uint32_t)(data->first_descriptor), 
	    sizeof(adma_descriptor_line_t) * (data->descriptor_num));

	return SDH_TRUE;
}

static uint32_t mrvl_sdh_release_desc_table(struct dev_request *drq) 
{
	struct dev_data *data = drq->data;	
	
	if (data->desc_malloc_buffer != NULL) {
		//bbu_mfree(data->desc_malloc_buffer);
		DEBUG_SDH_PRINT("Release description malloc success: 0x%x\r\n", 
		                 data->desc_malloc_buffer);

		data->desc_malloc_buffer = NULL;
	}

	return SDH_TRUE;	
}

static uint32_t mrvl_sdh_prepare_dma(struct sdhc_host *host) /* support ADMA2 and SDMA */
{
	struct dev_data *data = host->drq->data;
#ifndef CONFIG_SDH_PTS
	struct scatterlist *sg;
	uint32_t i;
#endif
	uint32_t para;
		
	if (host->drq->trans_mode & SDH_TRANS_TYPE_ADMA) {
		mrvl_sdh_dma_select(host, MRVL_SDH_HOST_ADMA2);
		DEBUG_SDH_PRINT("ADMA mode, total_bytes: %d\r\n", host->total_bytes_left);
		
		mrvl_sdh_create_desc_table(host->drq);
		MRVL_SDH_REG32_WRITE(host->base_addr, ADMA_SYS_ADDR_1, (data->first_descriptor));
	} else if (host->drq->trans_mode & SDH_TRANS_TYPE_SDMA) {
		mrvl_sdh_dma_select(host, MRVL_SDH_HOST_SDMA);
		DEBUG_SDH_PRINT("SDMA mode, total_bytes: %d\r\n", host->total_bytes_left);

		MRVL_SDH_REG32_WRITE(host->base_addr, SYS_ADDR_LOW, data->sg->base_address);
		host->sdma_sys_addr = data->sg->base_address;
		//bbu_printf("Initial system address: 0x%x\r\n", sg->base_address);
	} else {
		DEBUG_SDH_PRINT("Error: Unsupported DMA mode!\r\n");
		return SDH_FALSE;
	}
#ifndef CONFIG_SDH_PTS	
	/*
	 * For DMA transfer, need to do cache clean and invaidate operation before read finished
	 * Avoiding buffer's cache-line length alignment problem
	 */	
	if (data->flags & DEV_DATA_WRITE) {
		for_each_sg(data->sg, sg, data->sg_len, i) {
			dcache_clean_range((uint32_t)(sg->base_address), sg->length);
		}
	}

	if ((host->drq->trans_mode &
	     (SDH_TRANS_TYPE_ADMA | SDH_TRANS_TYPE_SDMA)) && 
	    (data->flags & DEV_DATA_READ)){
		for_each_sg(data->sg, sg, data->sg_len, i) {
			dcache_invalidate_range((uint32_t)(sg->base_address), sg->length);	
		}
	}
	
#endif
	/*
	 * Attention:
	 *  For DMA mode, must clear bit0 of SD_CFG_FIFO_PARAM
	*/	
	para = MRVL_SDH_REG32_READ(host->base_addr, CFG_FIFO_PARAM);
	if (para & BIT_0) {
		para &= ~BIT_0;
		MRVL_SDH_REG32_WRITE(host->base_addr, CFG_FIFO_PARAM, para);
	}

	return SDH_TRUE;
}

static uint32_t mrvl_sdh_dma_select(struct sdhc_host *host, uint32_t dma_mode)
{
	uint16_t ctrl;
	
	ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL);

	if ((ctrl & HOST_CTRL_DMA_SEL_MSK) !=
	    (dma_mode << HOST_CTRL_DMA_SEL_BASE)) {
		ctrl &= (~HOST_CTRL_DMA_SEL_MSK);
		ctrl |= (dma_mode << HOST_CTRL_DMA_SEL_BASE);
		MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL, ctrl);
	}
	
	return SDH_TRUE;
}

void sdh_config_int_stat(struct sdhc_host *host, uint32_t clear, uint32_t set)
{
	uint32_t ier;

	ier = MRVL_SDH_REG32_READ(host->base_addr, NORMAL_INT_STATUS_EN);
	ier &= ~clear;
	ier |= set;
	MRVL_SDH_REG32_WRITE(host->base_addr, NORMAL_INT_STATUS_EN, ier);
}

void sdh_config_int_sig(struct sdhc_host *host, uint32_t clear, uint32_t set)
{
	uint32_t ier;

	ier = MRVL_SDH_REG32_READ(host->base_addr, NORMAL_INT_STATUS_INT_EN);
	ier &= ~clear;
	ier |= set;
	MRVL_SDH_REG32_WRITE(host->base_addr, NORMAL_INT_STATUS_INT_EN, ier);
}

static void sdh_config_transfer_irqs(struct sdhc_host *host)
{
	uint32_t pio_irqs = NORMAL_INT_STATUS_EN_RD_RDY_EN |
	                    NORMAL_INT_STATUS_EN_TX_RDY_EN;
	uint32_t dma_irqs = NORMAL_INT_STATUS_EN_DMA_INT_EN |
	                    ERROR_INT_STATUS_EN_ADMA_ERR_EN << 16;

	if (host->dma_in_use) {
		sdh_config_int_stat(host, pio_irqs, dma_irqs);
		if (host->drq->trans_mode & SDH_TRANS_TYPE_INT) {
			sdh_config_int_sig(host, pio_irqs, dma_irqs);
		}
	} else {
		sdh_config_int_stat(host, dma_irqs, pio_irqs);
		if (host->drq->trans_mode & SDH_TRANS_TYPE_INT) {
			sdh_config_int_sig(host, dma_irqs, pio_irqs);
		}
	}
}

static void sdh_config_common_irqs(struct sdhc_host *host)
{
	if (host->drq->trans_mode & SDH_TRANS_TYPE_INT) {
		sdh_config_int_sig(host, 0,
			(NORMAL_INT_STATUS_INT_EN_CMD_COMPLETE_INT_EN |
			 NORMAL_INT_STATUS_INT_EN_XFER_COMPLETE_INT_EN) |
			(ERROR_INT_STATUS_INT_EN_CMD_TIMEOUT_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_CMD_CRC_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_CMD_END_BIT_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_CMD_INDEX_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_DATA_TIMEOUT_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_RD_DATA_CRC_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_RD_DATA_END_BIT_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_AUTO_CMD12_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_TUNE_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_AXI_RESP_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_CPL_TIMEOUT_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_CRC_STATUS_ERR_INT_EN) << 16);
	} else {
		sdh_config_int_sig(host,
			(NORMAL_INT_STATUS_INT_EN_CMD_COMPLETE_INT_EN |
			 NORMAL_INT_STATUS_INT_EN_XFER_COMPLETE_INT_EN) |
			(ERROR_INT_STATUS_INT_EN_CMD_TIMEOUT_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_CMD_CRC_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_CMD_END_BIT_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_CMD_INDEX_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_DATA_TIMEOUT_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_RD_DATA_CRC_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_RD_DATA_END_BIT_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_AUTO_CMD12_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_TUNE_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_AXI_RESP_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_CPL_TIMEOUT_ERR_INT_EN |
			 ERROR_INT_STATUS_INT_EN_CRC_STATUS_ERR_INT_EN) << 16,
			0);
	}
}

static void mrvl_sdh_prepare_data(struct sdhc_host *host)
{
	struct dev_data *data = host->drq->data;

	sdh_config_common_irqs(host);
	if (data == NULL) {
		host->dma_in_use = 0;		
		host->total_bytes_left = 0;

		MRVL_SDH_REG16_WRITE(host->base_addr, BLOCK_SIZE, 0);
		MRVL_SDH_REG16_WRITE(host->base_addr, BLOCK_COUNT, 0);
		return;
	}

	host->total_bytes_left = (data->blocks * data->blksz);	
	DEBUG_SDH_PRINT("block number = %d, block_size = %d\r\n",
		data->blocks, data->blksz);

	/* Only do DMA for entire blocks */
	if (host->drq->trans_mode & 
	    (SDH_TRANS_TYPE_ADMA | SDH_TRANS_TYPE_SDMA)) {
		host->dma_in_use = 1;							  
	} else if (host->drq->trans_mode & SDH_TRANS_TYPE_PIO)  {
		host->dma_in_use = 0;
	} else {	 /* Otherwise will default to pio mode */
		uint32_t mode;
		
		mode = host->drq->trans_mode;
		host->dma_in_use = 0;
		
		DEBUG_SDH_PRINT("Unknown transfer mode!\r\n");		
		host->drq->trans_mode = SDH_TRANS_TYPE_PIO;
		if (mode & SDH_TRANS_REQ_FROM_AO_SD)
			host->drq->trans_mode = SDH_TRANS_REQ_FROM_AO_SD;
		if (mode & SDH_TRANS_TYPE_INT)
			host->drq->trans_mode |= SDH_TRANS_TYPE_INT;	
	}

	if (host->dma_in_use)
		mrvl_sdh_prepare_dma(host);
	else
		mrvl_sdh_prepare_pio(host);

	MRVL_SDH_REG16_WRITE(host->base_addr, BLOCK_SIZE, data->blksz);
	MRVL_SDH_REG16_WRITE(host->base_addr, BLOCK_COUNT, data->blocks & 0xffff);

	sdh_config_transfer_irqs(host);
}

static void sdh_set_transfer_mode(struct sdhc_host *host)
{
	struct dev_data *data = host->drq->data;
	struct dev_command *cmd = host->drq->cmd;
	uint32_t mode = 0;

	if (!data){
		MRVL_SDH_REG16_WRITE(host->base_addr, TRANSFER_MODE, 0);
		return;
	}

	if (data->blocks > 1) {
		mode |= TRANSFER_MODE_MULTI_BLK_SEL;	
		if (data->blocks < (1 << 16))
			mode |= TRANSFER_MODE_BLK_CNT_EN;
		else {
			if (host->drq->trans_mode &
				(SDH_TRANS_TYPE_PIO | SDH_TRANS_TYPE_SDMA)) {
				DEBUG_SDH_PRINT("Error: PIO/SDMA cann't support "
				           "more than 65535 blocks!\r\n");
				return;
			}
		}
	}

	/* Auto cmd enable or not should be focus here */	
	if (host->drq->cmd->auto_cmd & SDH_AUTO_CMD23) {
		mode |= TRANSFER_MODE_AUTO_CMD23_EN;
		/* 
		 * Set block count for auto cmd23 
		 *  need to set bit30 for packed commands
		 */
		if ((cmd->opcode == COM_CMD25_WRITE_MULTIPLE_BLOCK && 
		    (data->sg_len == 1) && (data->sg->length == 512)) ||
		    (data->sg_len > 1))
			MRVL_SDH_REG32_WRITE(host->base_addr,
				SYS_ADDR_LOW, data->blocks | BIT_30);  
		else
			MRVL_SDH_REG32_WRITE(host->base_addr,
				SYS_ADDR_LOW, data->blocks);  
		DEBUG_SDH_PRINT("Auto CMD23 Enable\r\n");
	} else if (host->drq->cmd->auto_cmd & SDH_AUTO_CMD12) {
		mode |= TRANSFER_MODE_AUTO_CMD12_EN;	
		DEBUG_SDH_PRINT("Auto CMD12 Enable\r\n");
	}

	if (data->flags & DEV_DATA_READ)
		mode |= TRANSFER_MODE_TO_HOST_DIR;
	if (host->dma_in_use)
		mode |= TRANSFER_MODE_DMA_EN;

	MRVL_SDH_REG16_WRITE(host->base_addr, TRANSFER_MODE, mode);	
}

void mrvl_sdh_error_handle(struct sdhc_host *host)
{
	struct dev_request *drq;
	uint16_t err_status, err_enable;
	uint16_t print_err_info;

	drq = host->drq;
	drq->cmd->error = 0;
	if (drq->data)
		drq->data->error = 0;

	/* Disable error interrupt */
	err_enable = MRVL_SDH_REG16_READ(host->base_addr,
		ERROR_INT_STATUS_INT_EN);
	if (err_enable)
		MRVL_SDH_REG16_WRITE(host->base_addr,
			ERROR_INT_STATUS_INT_EN, 0x0);
 loop:
	err_status = MRVL_SDH_REG16_READ(host->base_addr, ERROR_INT_STATUS);
	if (!err_status)
		goto out;
	DEBUG_SDH_PRINT("SDH Error interrupt status: 0x%x\r\n", err_status);

	if (drq->cmd->opcode != MMC_CMD21_SEND_TUNING_BLOCK &&
	    drq->cmd->opcode != COM_CMD12_STOP_TRANSMISSION &&
	    !(drq->cmd->opcode & COM_SDH_TX_TUNNING) &&
	    !(drq->cmd->opcode & COM_SDH_RX_TUNNING)) {
		print_err_info = 1;
	} else {
		print_err_info = 0;
	}

	if (err_status & ERROR_INT_STATUS_CRC_STATUS_ERR) {
		drq->data->error = SDH_ILSEQ;
		if (print_err_info) {
			DEBUG_SDH_PRINT("Error: CMD%d CRC Status Error!\r\n", 
			           (drq->cmd->opcode & 0x3f));	
		}
	}

	if (err_status & ERROR_INT_STATUS_CPL_TIMEOUT_ERR) {
		drq->cmd->error = SDH_TIMEDOUT;
		if (print_err_info) {	
			DEBUG_SDH_PRINT("Error: CMD%d Command completion signal timeout!\r\n", 
			           (drq->cmd->opcode & 0x3f));	
		}	
	}

	if (err_status & ERROR_INT_STATUS_AXI_RESP_ERR) {
		drq->cmd->error = SDH_ILSEQ;
		if (print_err_info) {
			DEBUG_SDH_PRINT("Error: CMD%d AXI bus response error!\r\n", 
			            (drq->cmd->opcode & 0x3f));
		}	
	}

	if (err_status & ERROR_INT_STATUS_TUNE_ERR) {
		uint16_t ctrl;

		drq->cmd->error = SDH_ILSEQ;
		ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL_2);
		ctrl &= ~HOST_CTRL_2_SAMPLING_CLK_SEL;
		MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL_2, ctrl);

		mdelay(10);
		if (print_err_info) {
			DEBUG_SDH_PRINT("Error: CMD%d Tuning error!\r\n", 
			           (drq->cmd->opcode & 0x3f));	
		}
	}

	if (err_status & ERROR_INT_STATUS_ADMA_ERR) {
		drq->data->error = SDH_ILSEQ;
		// if (print_err_info) {
		// 	uint32_t adma_error_status;

		// 	adma_error_status = MRVL_SDH_REG16_READ(host->base_addr, ADMA_ERROR_STATUS);
		// 	DEBUG_SDH_PRINT("Error: CMD%d ADMA error!\r\n", 
		// 	           (drq->cmd->opcode & 0x3f));
		// 	DEBUG_SDH_PRINT("ADMA Error Status Register: 0x%x\r\n", adma_error_status);
		// }		
	}

	if (err_status & ERROR_INT_STATUS_AUTO_CMD12_ERR) {
		uint16_t auto_err;

		drq->data->error = SDH_ILSEQ;
		auto_err = MRVL_SDH_REG16_READ(host->base_addr,
			AUTO_CMD12_ERROR_STATUS);
		MRVL_SDH_REG16_WRITE(host->base_addr,
			AUTO_CMD12_ERROR_STATUS, auto_err);  	
		if (print_err_info) {
			DEBUG_SDH_PRINT("Auto %s error for CMD%d , "
				"AUTO_CMD12_ERROR_STATUS=0x%x\r\n",
				(drq->cmd->auto_cmd & SDH_AUTO_CMD23)
					? "CMD23" : "CMD12",
				(drq->cmd->opcode & 0x3f), auto_err);	
		}
	}

	if (err_status & ERROR_INT_STATUS_RD_DATA_END_BIT_ERR) {
		drq->data->error = SDH_ILSEQ;
		if (print_err_info) {
			DEBUG_SDH_PRINT("Error: CMD%d Data end bit error!\r\n", 
			           (drq->cmd->opcode & 0x3f));	
		}
	}

	if (err_status & ERROR_INT_STATUS_RD_DATA_CRC_ERR) {
		drq->data->error = SDH_ILSEQ;			
		if (print_err_info) {
			DEBUG_SDH_PRINT("Error: CMD%d Data CRC error!\r\n", 
			           (drq->cmd->opcode & 0x3f));	
		}
	}

	if (err_status & ERROR_INT_STATUS_DATA_TIMEOUT_ERR) {
		drq->data->error = SDH_TIMEDOUT;
		if (print_err_info) {
			DEBUG_SDH_PRINT("Error: CMD%d Data timeout!\r\n", 
			           (drq->cmd->opcode & 0x3f));	
		}
	}

	if (err_status & ERROR_INT_STATUS_CMD_INDEX_ERR) {
		drq->cmd->error = SDH_ILSEQ;
		if (print_err_info) {
			DEBUG_SDH_PRINT("Error: CMD%d Command index error!\r\n", 
			           (drq->cmd->opcode & 0x3f));	
		}
	}

	if (err_status & ERROR_INT_STATUS_CMD_END_BIT_ERR) {
		drq->cmd->error = SDH_ILSEQ;
		if (print_err_info) {
			DEBUG_SDH_PRINT("Error: CMD%d Command end bit error!\r\n", 
			           (drq->cmd->opcode & 0x3f));	
		}
	}

	if (err_status & ERROR_INT_STATUS_CMD_CRC_ERR) {
		drq->cmd->error = SDH_ILSEQ;
		if (print_err_info) {
			DEBUG_SDH_PRINT("Error: CMD%d Command CRC error!\r\n", 
			           (drq->cmd->opcode & 0x3f));	
		}
	}

	if (err_status & ERROR_INT_STATUS_CMD_TIMEOUT_ERR) {
		drq->cmd->error = SDH_TIMEDOUT;
		if (print_err_info) {
			DEBUG_SDH_PRINT("Error: CMD%d Command timeout!\r\n", 
			           (drq->cmd->opcode & 0x3f));	
		}
	}

	if (drq->cmd->error)
		mrvl_sdh_softreset_cmd_line(host);

	if (drq->data && drq->data->error)
		mrvl_sdh_softreset_data_line(host);

	udelay(40);
	/* Clear interrupt flag */
	MRVL_SDH_REG16_WRITE(host->base_addr, ERROR_INT_STATUS, err_status);
	goto loop;
 out:
	/* Enable error interrupt */
	if (err_enable)
		MRVL_SDH_REG16_WRITE(host->base_addr,
			ERROR_INT_STATUS_INT_EN, err_enable);
}

uint32_t mrvl_sdh_dma_int_handle_for_sdma(struct sdhc_host *host)
{
	uint32_t pdata;
	
	pdata = host->sdma_sys_addr;
	pdata &= ~0xfff;
	pdata += BIT_12;  /* SDMA generate interrupt at every 4K boundary */
	host->sdma_sys_addr = pdata;
	MRVL_SDH_REG32_WRITE(host->base_addr, SYS_ADDR_LOW, pdata);
	DEBUG_SDH_PRINT("New System Address: 0x%x\r\n", pdata);
	
	return SDH_TRUE;

};

static void sdh_wait_done_inquire(struct sdhc_host *host)
{
	struct dev_request *drq = host->drq;
	struct dev_data *data = drq->data;
	uint32_t end_command = 0, end_transfer = 0, transfer_err = 0;
	uint16_t ir_status, err_status;

	do {
		ir_status = MRVL_SDH_REG16_READ(host->base_addr, NORMAL_INT_STATUS);
		/* clear interrupt flag */
		MRVL_SDH_REG16_WRITE(host->base_addr, NORMAL_INT_STATUS, ir_status);  

		if (ir_status & NORMAL_INT_STATUS_CMD_COMPLETE) {
			err_status = MRVL_SDH_REG16_READ(host->base_addr, ERROR_INT_STATUS);
			if (!(err_status & ERROR_INT_STATUS_CMD_TIMEOUT_ERR))
				end_command = 1;
		}		

		if (ir_status &
		    (NORMAL_INT_STATUS_RX_RDY | NORMAL_INT_STATUS_TX_RDY)) {
			if (host->total_bytes_left && !(host->dma_in_use))
				mrvl_sdh_pio_xfer_data(host);
			/* 
			 * Fix: CMD19 for eMMC is bus write test, CMD19 for SD is RX tuning, so
			 *      we need to distinguish between them
			 */
			if ((ir_status & NORMAL_INT_STATUS_RX_RDY) &&
			    ((drq->cmd->opcode & 0x3f) == MMC_CMD21_SEND_TUNING_BLOCK)) {
			     	end_transfer = 1;
			}
		}

		if (data && (!end_transfer) && (ir_status & NORMAL_INT_STATUS_DMA_INT)) {		           
			if (drq->trans_mode & SDH_TRANS_TYPE_SDMA) {
				mrvl_sdh_dma_int_handle_for_sdma(host);
			} else if (drq->trans_mode & SDH_TRANS_TYPE_ADMA) {
				/* Reserved for ADMA DMA interrupt handle */
			}
		}

		if (ir_status & NORMAL_INT_STATUS_XFER_COMPLETE) {
			DEBUG_SDH_PRINT("Data transfer complete interrupt from CMD%d!\r\n", 
			                drq->cmd->opcode);
			MRVL_SDH_REG16_WRITE(host->base_addr, NORMAL_INT_STATUS,
			                     ERROR_INT_STATUS_DATA_TIMEOUT_ERR);  
			end_transfer = 1;
		}

		if (ir_status & NORMAL_INT_STATUS_ERR_INT) {
#if 0  /************* For debug, used to dump register when error happen **************/
			if (drq->cmd->opcode != SD_CMD19_SEND_TUNING_BLOCK &&
				    drq->cmd->opcode != MMC_CMD21_SEND_TUNING_BLOCK &&
				    !(drq->cmd->opcode & COM_SDH_TX_TUNNING) &&
				    !(drq->cmd->opcode & COM_SDH_RX_TUNNING)) {
				//mmc_dump_register();
				sd_dump_register();
				while(1);
			}
#endif	/******************************************************************************/	
			mrvl_sdh_error_handle(host);
			transfer_err = 1;
		}

		if (end_command) {
			end_command = 0;
			if (mrvl_sdh_cmd_done(host)) {
				break;
			}
		}

		if (end_transfer) {
			end_transfer = 0;
			mrvl_sdh_end_of_data(host);
			break;
		}

		if (transfer_err) {
			transfer_err = 0;
			mrvl_sdh_xfer_done(host);
			break;
		}
	} while (1);

	return;
}

void mrvl_sdh_start_request(struct sdhc_host *host, struct dev_request *drq)
{
	struct dev_command *cmd = drq->cmd;	
	struct dev_data *data = drq->data;
	uint32_t time_ct = 200;
	uint16_t mask, err_stat;

	host->drq = drq;
	host->transfer_complete = 0;

	mask = PRESENT_STATE_1_CMD_INHIBIT_CMD;
	if ((data != NULL) || (cmd->flags & DEV_RSP_BUSY))
		mask |= PRESENT_STATE_1_CMD_INHIBIT_DAT;

	/* We shouldn't wait for data inihibit for stop commands, even
	   though they might use busy signaling */
	if (cmd->opcode & COM_CMD12_STOP_TRANSMISSION)
		mask &= ~PRESENT_STATE_1_CMD_INHIBIT_DAT;

	while (MRVL_SDH_REG16_READ(host->base_addr, PRESENT_STATE_1) & mask) {
		if (!time_ct) {
			DEBUG_SDH_PRINT("err: host don't release inihibit bits\r\n");
			return;
		}
		time_ct--;
		mdelay(1);
	}

	/* For safe, clear last error status if exist */
	err_stat = MRVL_SDH_REG16_READ(host->base_addr, ERROR_INT_STATUS);
	if (err_stat)
	       MRVL_SDH_REG16_WRITE(host->base_addr, ERROR_INT_STATUS, err_stat);

	mrvl_sdh_prepare_data(host);
	sdh_set_transfer_mode(host);
	mrvl_sdh_send_command(host, cmd);	
}

void mrvl_sdh_wait_req_done(struct sdhc_host *host, struct dev_request *drq)
{
	struct dev_command *cmd = drq->cmd;	
	struct dev_data *data = drq->data;

	if (drq->trans_mode & SDH_TRANS_TYPE_INT) {
		while (!(host->transfer_complete)) {
			if (cmd->error || (data && data->error))
				return;
		}
	} else {	
		sdh_wait_done_inquire(host);
	}
}

uint32_t mrvl_sdh_set_timeout(struct sdhc_host *host, uint32_t time_ms, uint32_t sd_clk)
{
	uint32_t count;
	uint32_t base_clk = 1;
	uint16_t i, k;
		
	count = time_ms * (sd_clk/4000);

	for (i=0; i<12; i++) {
		base_clk = base_clk << 1;
	}

	for (i = 13, k = 0; i < 28; i++, k++) {
		base_clk = base_clk << 1;

		if (base_clk >= count) {
			MRVL_SDH_REG16_WRITE(host->base_addr, TIMEOUT_CTRL_SW_RESET, k);
			DEBUG_SDH_PRINT("clock ctrl: k = %d\r\n", k);
			
			return SDH_TRUE;
		}		
	}
	DEBUG_SDH_PRINT("clock ctrl: k = %d\r\n", k);	
	MRVL_SDH_REG16_WRITE(host->base_addr, TIMEOUT_CTRL_SW_RESET, k-1);

	return SDH_INVAL;

}

uint32_t mrvl_sdh_force_pad_clk_on(struct sdhc_host *host)
{
	uint32_t clk_ctrl;
	
	clk_ctrl = MRVL_SDH_REG32_READ(host->base_addr, FIFO_PARAM);
	clk_ctrl |= FIFO_PARAM_OVRRD_CLK_OEN | FIFO_PARAM_FORCE_CLK_ON;
	MRVL_SDH_REG32_WRITE(host->base_addr, FIFO_PARAM, clk_ctrl);

	return SDH_TRUE;
}

uint32_t mrvl_sdh_force_pad_clk_off(struct sdhc_host *host)
{
	uint32_t clk_ctrl;
	
	clk_ctrl = MRVL_SDH_REG32_READ(host->base_addr, FIFO_PARAM);
	clk_ctrl |= FIFO_PARAM_OVRRD_CLK_OEN;
	clk_ctrl &= ~FIFO_PARAM_FORCE_CLK_ON;
	MRVL_SDH_REG32_WRITE(host->base_addr, FIFO_PARAM, clk_ctrl);

	return SDH_TRUE;
}

uint32_t mrvl_sdh_pad_clk_auto_run(struct sdhc_host *host)
{
	uint32_t clk_ctrl;
	
	clk_ctrl = MRVL_SDH_REG32_READ(host->base_addr, FIFO_PARAM);
	clk_ctrl &= ~(FIFO_PARAM_OVRRD_CLK_OEN | FIFO_PARAM_FORCE_CLK_ON);
	MRVL_SDH_REG32_WRITE(host->base_addr, FIFO_PARAM, clk_ctrl);

	return SDH_TRUE;
}

uint32_t mrvl_sdh_clk_start(struct sdhc_host *host)
{
	volatile uint16_t clk_ctrl;

	clk_ctrl = MRVL_SDH_REG16_READ(host->base_addr, CLOCK_CTRL);
	clk_ctrl |= CLOCK_CTRL_INT_CLK_EN;
	/* internal clk start to oscillate */
	MRVL_SDH_REG16_WRITE(host->base_addr, CLOCK_CTRL, clk_ctrl);

	/* wait internal clock enable before enable sd clock */
	while (!((clk_ctrl = MRVL_SDH_REG16_READ(host->base_addr, CLOCK_CTRL)) & CLOCK_CTRL_INT_CLK_STABLE));

   	clk_ctrl |= CLOCK_CTRL_SD_CLK_EN;  /* enbale sdclk output */	
	MRVL_SDH_REG16_WRITE(host->base_addr, CLOCK_CTRL, clk_ctrl);

	return SDH_TRUE;
}

uint32_t mrvl_sdh_clk_stop(struct sdhc_host *host)
{
	uint16_t clk_ctrl;
	//volatile uint16_t psr;
	
	/* shall not stop sd clock when a transaction is occuring on the sd bus */
	//while((psr = MRVL_SDH_REG16_READ(host->base_addr, PRESENT_STATE_1)) & 0x0fffc);
	//while((psr = MRVL_SDH_REG16_READ(host->base_addr, PRESENT_STATE_1)) & 0x03);
	
	clk_ctrl = MRVL_SDH_REG16_READ(host->base_addr, CLOCK_CTRL);
	//clk_ctrl &= ~(CLOCK_CTRL_SD_CLK_EN | CLOCK_CTRL_INT_CLK_EN);
	clk_ctrl &= ~CLOCK_CTRL_SD_CLK_EN;
	MRVL_SDH_REG16_WRITE(host->base_addr, CLOCK_CTRL, clk_ctrl);

	return SDH_TRUE;
}

uint32_t mrvl_sdh_clk_enable(struct sdhc_host *host) 
{
	uint16_t clk_ctrl;

	clk_ctrl = MRVL_SDH_REG16_READ(host->base_addr, CLOCK_CTRL);
	clk_ctrl |= CLOCK_CTRL_SD_CLK_EN;
	MRVL_SDH_REG16_WRITE(host->base_addr, CLOCK_CTRL, clk_ctrl);

	return SDH_TRUE;
}

uint32_t mrvl_sdh_clk_disable(struct sdhc_host *host) 
{
	uint16_t clk_ctrl;

	clk_ctrl = MRVL_SDH_REG16_READ(host->base_addr, CLOCK_CTRL);
	clk_ctrl &= ~CLOCK_CTRL_SD_CLK_EN;
	MRVL_SDH_REG16_WRITE(host->base_addr, CLOCK_CTRL, clk_ctrl);
	
	return SDH_TRUE;
}

static uint32_t mrvl_sdh_find_clk_div(struct sdhc_host *host, uint32_t clk_freq_hz, uint8_t mode)
{
	uint32_t freq_diff;
	uint32_t i;
	uint32_t second_div;
	uint32_t base;
	
	base = host->base_clk;
	freq_diff = sdh_clk_source[host->clk_source];
		
	/* 
	 * Keep base clock not lower than double expected clock for DDR mode 
	 */	
	if ((base < clk_freq_hz * 2) && (mode & SDH_BUS_SPEED_DDR_CLK_50M)) {
		DEBUG_SDH_PRINT("Failed: for ddr mode, base clock should be more than or equal to twice bus clock!\r\n");
		return SDH_FALSE;
	}
	
	for (i = 0; i <= host->max_second_div/2; i++) {
		uint32_t clk_generated;
		if (i == 0) {
			clk_generated = base;
		} else {
			clk_generated = base/(i * 2);
		}
		
		if (clk_generated <= clk_freq_hz) {
			if (freq_diff >= (clk_freq_hz - clk_generated)) {
				freq_diff = clk_freq_hz - clk_generated;
				second_div = i * 2;
			}
			break;
		}
	}

	if (i > host->max_second_div/2) {
		DEBUG_SDH_PRINT("Error: Demanded frequency (%dKHz) is two small\r\n", clk_freq_hz);
		return SDH_FALSE;
	}
	
	host->second_div = second_div;
	host->sdclk = clk_freq_hz - freq_diff;
	
	DEBUG_SDH_PRINT("Source clock: %dKHz, base clock: %dKHz, bus clock: %dKHz\r\n", 
	            sdh_clk_source[host->clk_source]/1000, host->base_clk/1000, host->sdclk/1000);
	
	return SDH_TRUE;
}


static void sdh_config_def_timing(struct sdhc_host *host, uint8_t mode)
{
	uint32_t rx_cfg, tx_cfg;
	uint16_t ctrl;

	/* reset hardware tuning circuit for safe */
	ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL_2);
	ctrl &= ~(HOST_CTRL_2_EXE_TUNING | HOST_CTRL_2_SAMPLING_CLK_SEL);
	MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL_2, ctrl);

	if (mode & (SDH_BUS_SPEED_DEFAULT | SDH_BUS_SPEED_SDR_CLK_50M)) {
		/* Fix the Tx Hold time problem in DS/HS mode */
		tx_cfg = MRVL_SDH_REG32_READ(host->base_addr, TX_CFG_REG);
		tx_cfg &= ~TX_CFG_REG_TX_MUX_SEL;
		tx_cfg |= TX_CFG_REG_TX_INT_CLK_SEL;
		MRVL_SDH_REG32_WRITE(host->base_addr, TX_CFG_REG, tx_cfg);
		return;
	} else {
		tx_cfg = MRVL_SDH_REG32_READ(host->base_addr, TX_CFG_REG);
		tx_cfg &= ~TX_CFG_REG_TX_INT_CLK_SEL;
		MRVL_SDH_REG32_WRITE(host->base_addr, TX_CFG_REG, tx_cfg);
	}

	rx_cfg = 0;
	tx_cfg = 0;
	if (mode & SDH_BUS_SPEED_SDR_CLK_208M) {
		if (host->def_timing_sdr104) {
			rx_cfg = SDH_GET_RX(host->def_timing_sdr104);
			rx_cfg <<= RX_CFG_REG_SDCLK_DELAY_BASE;
			tx_cfg = SDH_GET_TX(host->def_timing_sdr104);
			tx_cfg <<= TX_CFG_REG_TX_HOLD_DELAY1_BASE;
		}
	} else if (mode & SDH_BUS_SPEED_DDR_CLK_50M) {
		if (host->def_timing_ddr50) {
			rx_cfg = SDH_GET_RX(host->def_timing_ddr50);
			rx_cfg <<= RX_CFG_REG_SDCLK_DELAY_BASE;
			tx_cfg = SDH_GET_TX(host->def_timing_ddr50);
			tx_cfg <<= TX_CFG_REG_TX_HOLD_DELAY0_BASE;
		}
	} else if (mode & SDH_BUS_SPEED_SDR_CLK_100M) {
		if (host->def_timing_sdr50) {
			rx_cfg = SDH_GET_RX(host->def_timing_sdr50);
			rx_cfg <<= RX_CFG_REG_SDCLK_DELAY_BASE;
			tx_cfg = SDH_GET_TX(host->def_timing_sdr50);
			tx_cfg <<= TX_CFG_REG_TX_HOLD_DELAY0_BASE;
		}
	}

	/*
	 * If user do not set tx/rx timing, will reset tx/rx config
	 * register to 0x0
	 */
	if (rx_cfg) {
		rx_cfg |= 0x1 << RX_CFG_REG_SDCLK_SEL1_BASE;
	}
	MRVL_SDH_REG32_WRITE(host->base_addr, RX_CFG_REG, rx_cfg);

	if (tx_cfg) {
		tx_cfg |= TX_CFG_REG_TX_MUX_SEL;
	}
	MRVL_SDH_REG32_WRITE(host->base_addr, TX_CFG_REG, tx_cfg);
}

uint32_t mrvl_sdh_select_uhs_mode(struct sdhc_host *host, uint8_t mode)
{
	uint16_t ctrl;
	uint16_t ce_ata;
	uint8_t hs;
	
	ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL_2);
	ctrl &= ~HOST_CTRL_2_UHS_MODE_SEL_MSK;
	
	hs = 0;
	if (mode & SDH_BUS_SPEED_SDR_CLK_208M) {
		ctrl |= 0x03;
	} else if (mode & SDH_BUS_SPEED_SDR_CLK_100M) {
		ctrl |= 0x02;
	} else if (mode & SDH_BUS_SPEED_DDR_CLK_50M) {
		ctrl |= 0x04;
	} else if (mode & SDH_BUS_SPEED_SDR_CLK_50M) {
		ctrl |= 0x01;
		hs = 1;
	} else if (mode & SDH_BUS_SPEED_DEFAULT) {
		ctrl |= 0x00;
	}

	/* For eMMC HS200 mode */
	ce_ata = MRVL_SDH_REG16_READ(host->base_addr, CE_ATA_2);
	if (ce_ata & CE_ATA_2_MMC_CARD) {
		if (mode & SDH_BUS_SPEED_SDR_CLK_208M) {
			ce_ata |= CE_ATA_2_MMC_HS200;	
		} else {
			ce_ata &= ~CE_ATA_2_MMC_HS200;
		}
		MRVL_SDH_REG16_WRITE(host->base_addr, CE_ATA_2 , ce_ata);
	}
	
	mrvl_sdh_clk_disable(host);

	MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL_2, ctrl);
	
	/* Enable host high speed bit for SDR 52MHz mode */
	ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL);
	if (hs) {
		ctrl |= HOST_CTRL_HI_SPEED_EN; 
	} else {
		ctrl &= ~HOST_CTRL_HI_SPEED_EN; 	
	}
	MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL, ctrl);

	sdh_config_def_timing(host, mode);

	mrvl_sdh_clk_enable(host);
	
	return SDH_TRUE;
}

uint32_t mrvl_sdh_supply_clk(struct sdhc_host *host, uint32_t clk_freq_hz, uint8_t mode)
{
	uint32_t second_div;
	uint8_t freq_sel_lo;
	uint8_t freq_sel_hi;
	uint16_t freq_sel;
	volatile uint16_t clk_ctrl;
	uint32_t err;
	
	DEBUG_SDH_PRINT("Expected SD bus clock: %dKHz\r\n", clk_freq_hz/1000);

	err = mrvl_sdh_find_clk_div(host, clk_freq_hz, mode);
	if (err) {
		DEBUG_SDH_PRINT("Expected clock not supported. Give up\r\n");
		return err;
	}
	
	second_div = host->second_div / 2;
	freq_sel_lo = second_div & 0x00ff;
	freq_sel_hi = (second_div & 0x0300) >> 2;
	freq_sel = (freq_sel_lo << 8) | freq_sel_hi;

	clk_ctrl = MRVL_SDH_REG16_READ(host->base_addr, CLOCK_CTRL);
	
	clk_ctrl &= 0x003f;
	clk_ctrl |= freq_sel;

	mrvl_sdh_clk_stop(host);  
	MRVL_SDH_REG16_WRITE(host->base_addr, CLOCK_CTRL, clk_ctrl);	
	mrvl_sdh_clk_start(host);

	/* set data timeout couter value */
	if (mrvl_sdh_set_timeout(host, 1500, host->sdclk)) {
		DEBUG_SDH_PRINT("[SDH] Warning: Required data time out value too large\r\n");
	}	

	mrvl_sdh_select_uhs_mode(host, mode);
	
	return SDH_TRUE;
}

uint32_t mrvl_sdh_host_cmd_level(struct sdhc_host *host)
{
    uint16_t psr;
    
    psr = MRVL_SDH_REG16_READ(host->base_addr, PRESENT_STATE_2);
    return ((psr & PRESENT_STATE_2_CMD_LEVEL) ? 1 : 0);
    
}

uint32_t mrvl_sdh_host_data_level(struct sdhc_host *host)
{
    uint16_t psr;
    
    psr = MRVL_SDH_REG16_READ(host->base_addr, PRESENT_STATE_2);
    return ((psr & PRESENT_STATE_2_DAT_LEVEL_MSK) >> PRESENT_STATE_2_DAT_LEVEL_BASE);
}

uint32_t mrvl_sdh_control_1v8_enable(struct sdhc_host *host, uint8_t on)
{
	uint16_t ctrl;
	
	/* set to 1 after 10ms */
	ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL_2);
	if (on) {
		ctrl |= HOST_CTRL_2_SDH_V18_EN;
	} else {
		ctrl &= ~HOST_CTRL_2_SDH_V18_EN;
	}
	MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL_2, ctrl);

	return SDH_TRUE;
}

uint32_t mrvl_sdh_set_host_driver_strength(struct sdhc_host *host, uint8_t strength)
{
	uint16_t ctrl;

	ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL_2);
	ctrl &= ~HOST_CTRL_2_DRV_STRENGTH_SEL_MSK;
	ctrl |= strength << HOST_CTRL_2_DRV_STRENGTH_SEL_BASE;	
	MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL_2, ctrl);
	
	return SDH_TRUE;
}

uint32_t mrvl_sdh_power_on(struct sdhc_host *host)
{
	uint16_t ctrl;

	ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL);
	ctrl |= HOST_CTRL_SD_BUS_POWER;
	MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL, ctrl);

	DEBUG_SDH_PRINT("HOST_CTRL: 0x%x. SD Bus Power On!\r\n", 
	    MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL));
	return SDH_TRUE;
}

uint32_t mrvl_sdh_power_off(struct sdhc_host *host)
{
	uint16_t host_ctrl;

	host_ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL);
	host_ctrl &= ~HOST_CTRL_SD_BUS_POWER;
	MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL, host_ctrl);
	
	DEBUG_SDH_PRINT("mrvl_sdh_power_off: HOST_CTRL -- 0x%x\r\n",  
	   MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL));
	return SDH_TRUE;
}

/* 
 * Change host data transfer width, 1 -- 1bit, 4 -- 4bit
*/
uint32_t mrvl_sdh_change_host_bit_mode(struct sdhc_host *host, uint32_t width)
{
	uint16_t ctrl;
	uint16_t mode = 0;

	if (width == 1)
		mode = 0 << 1;
	else if (width == 4)   
		mode = 1 << 1;
	else if (width == 8) 
		mode = 1 << 5;
	else
		bbu_printf("[Warining]: Unsupported bus width, change to 1bit!\r\n");

	mrvl_sdh_disable_card_interrupt(host);    
		
	ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL);
	ctrl &= ~(HOST_CTRL_DATA_WIDTH | HOST_CTRL_EX_DATA_WIDTH);
	ctrl |= mode;

	MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL, ctrl);
	if (ctrl != MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL)) {
		DEBUG_SDH_PRINT("Error: mrvl_sdh_change_host_bit_mode: write register HOST_CTRL failed\r\n");
	}
	mrvl_sdh_enable_card_interrupt(host);
	
	return SDH_TRUE;
}

/* Get host supported voltage */
static inline uint32_t mrvl_sdh_get_host_voltage_cap(struct sdhc_host *host)
{
	uint16_t cap;
	uint16_t mask;
	uint16_t result;

	cap = MRVL_SDH_REG16_READ(host->base_addr, CAPABILITIES_2);

	mask =(CAPABILITIES_2_VLG_18_SUPPORT | CAPABILITIES_2_VLG_30_SUPPORT |
	    CAPABILITIES_2_VLG_33_SUPPORT);

	result = (mask & cap) >> 8;

	return result; 
}

static void sdh_set_io_power_domain(struct sdhc_host *host, uint8_t vol)
{
	uint32_t io_reg;

	if (vol & (SD_HOST_VOLTAGE_3V3 | SD_HOST_VOLTAGE_3V0)){
		io_reg = SDMMC_IO_REG_PDB;
	} else if (vol & SD_HOST_VOLTAGE_1V8){
		io_reg = SDMMC_IO_REG_V18EN | SDMMC_IO_REG_PDB;
	} else {
		DEBUG_SDH_PRINT("Err: unsupported voltage!");
		return;
	}

	/*
	 * Should change io power domain when switching voltage if this sdh
	 * slot locate in a software-configurable power domain
	 */
	if (host->base_addr == SD0_BASE) {
		APBClock->ASFAR = 0xBABA;
		APBClock->ASSAR = 0xEB10;
		AIBAux->SDMMC_IO_REG = io_reg;
	}
}

uint32_t mrvl_sdh_set_host_sig_vol(struct sdhc_host *host, uint8_t vol)
{
	uint16_t ctrl;
	uint16_t pw_stat;
	
	ctrl = MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL);
	ctrl &= ~HOST_CTRL_SD_BUS_VLT_MSK;
	
	if (ctrl & HOST_CTRL_SD_BUS_POWER) {
		mrvl_sdh_power_off(host);  /* First power off before change sd supply voltage */
		pw_stat = 1;
	} else {
		pw_stat = 0;
	}

	sdh_set_io_power_domain(host, vol);
	if (vol == SD_HOST_VOLTAGE_3V3) {  /* max support voltage is 3.3v */
		ctrl |= 7 << HOST_CTRL_SD_BUS_VLT_BASE;
		MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL, ctrl);
		DEBUG_SDH_PRINT("set host power to 3.3v\r\n");    
	} else if (vol == SD_HOST_VOLTAGE_3V0) {  /* max support voltage is 3.0v */
		ctrl |= 6 << HOST_CTRL_SD_BUS_VLT_BASE;    
		MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL, ctrl);
		DEBUG_SDH_PRINT("set host power to 3.0v\r\n");
	} else if (vol == SD_HOST_VOLTAGE_1V8) {  /* max support voltage is 1.8v */
		ctrl |= 5 << HOST_CTRL_SD_BUS_VLT_BASE;
		MRVL_SDH_REG16_WRITE(host->base_addr, HOST_CTRL, ctrl);
		DEBUG_SDH_PRINT("set host power to 1.8v\r\n");
	} else {
		DEBUG_SDH_PRINT("Error supported voltage\r\n");
	}
	
	if (pw_stat) {
		mrvl_sdh_power_on(host);
	}
	host->sig_vol = vol;
	
	DEBUG_SDH_PRINT("mrvl_sdh_set_host_sig_vol:HOST_CTRL_2 -- 0x%x\r\n", 
	    MRVL_SDH_REG16_READ(host->base_addr, HOST_CTRL_2));
	
	return SDH_TRUE;
}

static uint32_t mrvl_sdh_prepare_pio(struct sdhc_host *host)
{
	uint32_t para;
	
	/*
	 * Attention:
	 *  For PIO mode, must set bit0 of SD_CFG_FIFO_PARAM, especially 
	 *  for device working attempt at HS200/SDR104 mode
	 */	
	para = MRVL_SDH_REG32_READ(host->base_addr, CFG_FIFO_PARAM);
	if (!(para & BIT_0)) {
		para |= BIT_0;
		MRVL_SDH_REG32_WRITE(host->base_addr, CFG_FIFO_PARAM, para);
	}
	
	DEBUG_SDH_PRINT("para: 0x%x, CFG_FIFO_PARAM: 0x%x\r\n", para, 
	    MRVL_SDH_REG32_READ(host->base_addr, CFG_FIFO_PARAM));
	DEBUG_SDH_PRINT("PIO Mode, total bytes: %d\r\n", host->total_bytes_left);

	return SDH_TRUE;
}

uint32_t mrvl_sdh_cmd_done(struct sdhc_host *host)
{
	struct dev_command *cmd;
	
	cmd = host->drq->cmd;

	if (cmd->flags & DEV_RSP_PRESENT) {
		if (cmd->flags & DEV_RSP_136) {
			cmd->resp[0] = (MRVL_SDH_REG16_READ(host->base_addr, RESP_7) << 24) | 
			               (MRVL_SDH_REG16_READ(host->base_addr, RESP_6) << 8) | 
			               (MRVL_SDH_REG16_READ(host->base_addr, RESP_5) >> 8);
			cmd->resp[1] = (MRVL_SDH_REG16_READ(host->base_addr, RESP_5) << 24) | 
			               (MRVL_SDH_REG16_READ(host->base_addr, RESP_4) << 8) | 
			               (MRVL_SDH_REG16_READ(host->base_addr, RESP_3) >> 8);
			cmd->resp[2] = (MRVL_SDH_REG16_READ(host->base_addr, RESP_3) << 24) | 
			               (MRVL_SDH_REG16_READ(host->base_addr, RESP_2) << 8) | 
			               (MRVL_SDH_REG16_READ(host->base_addr, RESP_1) >> 8);
			cmd->resp[3] = (MRVL_SDH_REG16_READ(host->base_addr, RESP_1) << 24) | 
			               (MRVL_SDH_REG16_READ(host->base_addr, RESP_0) << 8);
		} else {
			cmd->resp[0] = (MRVL_SDH_REG16_READ(host->base_addr, RESP_1) << 16) | 
			               (MRVL_SDH_REG16_READ(host->base_addr, RESP_0));	
		}
	}
	
	DEBUG_SDH_PRINT("Receive response from CMD%d\r\n", (host->drq->cmd->opcode & 0x3f));
	
	if (!(host->drq->data) && !(cmd->flags & DEV_RSP_BUSY)) {
		mrvl_sdh_xfer_done(host);	
		return SDH_FALSE;
	}

	return SDH_TRUE;
}

uint32_t mrvl_sdh_softreset_data_line(struct sdhc_host *host)
{
	uint16_t ctrl;
	
	do {  /* Wait data line non-active before reset data line */
		ctrl = MRVL_SDH_REG16_READ(host->base_addr, PRESENT_STATE_1);
		if (!(ctrl & PRESENT_STATE_1_DAT_ACTIVE)) {
			break;
		} else {
			DEBUG_SDH_PRINT("Data line active!\r\n");
		}
	} while(1);

	mdelay(1);  /* Give some extra delay for device to release DAT0 line */
	
	ctrl = MRVL_SDH_REG16_READ(host->base_addr, TIMEOUT_CTRL_SW_RESET);
	ctrl |= TIMEOUT_CTRL_SW_RESET_SW_RST_DAT;	
	MRVL_SDH_REG16_WRITE(host->base_addr, TIMEOUT_CTRL_SW_RESET, ctrl);
	
	do {
		mdelay(1);
		ctrl = MRVL_SDH_REG16_READ(host->base_addr, TIMEOUT_CTRL_SW_RESET);
		if (!(ctrl & TIMEOUT_CTRL_SW_RESET_SW_RST_DAT)) {
			break;
		}
	
	} while(1);
	
	return SDH_TRUE;
}

uint32_t mrvl_sdh_softreset_cmd_line(struct sdhc_host *host)
{
	uint16_t ctrl;
	
	ctrl = MRVL_SDH_REG16_READ(host->base_addr, TIMEOUT_CTRL_SW_RESET);
	ctrl |= TIMEOUT_CTRL_SW_RESET_SW_RST_CMD;	
	MRVL_SDH_REG16_WRITE(host->base_addr, TIMEOUT_CTRL_SW_RESET, ctrl);

	do {
		mdelay(1);
		ctrl = MRVL_SDH_REG16_READ(host->base_addr, TIMEOUT_CTRL_SW_RESET);
		if (!(ctrl & TIMEOUT_CTRL_SW_RESET_SW_RST_CMD)) {
			break;
		}
	
	} while(1);
	return SDH_TRUE;
}

uint32_t mrvl_sdh_host_softrest_all(struct sdhc_host *host)
{
	uint16_t ctrl;
	
	ctrl = MRVL_SDH_REG16_READ(host->base_addr, TIMEOUT_CTRL_SW_RESET);
	ctrl |= TIMEOUT_CTRL_SW_RESET_SW_RST_ALL;	
	MRVL_SDH_REG16_WRITE(host->base_addr, TIMEOUT_CTRL_SW_RESET, ctrl);
	
	do {
		mdelay(1);
		ctrl = MRVL_SDH_REG16_READ(host->base_addr, TIMEOUT_CTRL_SW_RESET);
		if (!(ctrl & TIMEOUT_CTRL_SW_RESET_SW_RST_ALL)) {
			break;
		}
	
	} while(1);
	
	return SDH_TRUE;
}

static uint32_t mrvl_sdh_pmu_release_all_axi_reset(struct sdhc_host *host)
{
	(void)host;
	PMUA->SDH0_CLK_RES_CTRL |= SDH0_CLK_RES_CTRL_SDH0_AXI_RST;
	return SDH_TRUE;
}

static uint32_t mrvl_sdh_pmu_reset_peripheral(struct sdhc_host *host)
{
	if (host->base_addr == SD0_BASE) {
		PMUA->SDH0_CLK_RES_CTRL &= ~SDH0_CLK_RES_CTRL_SDH0_RST;
	} else if (host->base_addr == SD1_BASE) {
		PMUA->SDH1_CLK_RES_CTRL &= ~SDH1_CLK_RES_CTRL_SDH1_RST;
	} else if (host->base_addr == SD2_BASE) {
		PMUA->SDH2_CLK_RES_CTRL &= ~SDH2_CLK_RES_CTRL_SDH2_RST;
	}

	return SDH_TRUE;
}

static uint32_t mrvl_sdh_pmu_release_peripheral_reset(struct sdhc_host *host)
{
	if (host->base_addr == SD0_BASE) {
		PMUA->SDH0_CLK_RES_CTRL |= SDH0_CLK_RES_CTRL_SDH0_RST;
	} else if (host->base_addr == SD1_BASE) {
		PMUA->SDH1_CLK_RES_CTRL |= SDH1_CLK_RES_CTRL_SDH1_RST;
	} else if (host->base_addr == SD2_BASE) {
		PMUA->SDH2_CLK_RES_CTRL |= SDH2_CLK_RES_CTRL_SDH2_RST;
	}

	return SDH_TRUE;
}

static uint32_t mrvl_sdh_pmu_enable_peripheral_clk(struct sdhc_host *host)
{
	if (host->base_addr == SD0_BASE) {
		PMUA->SDH0_CLK_RES_CTRL |= SDH0_CLK_RES_CTRL_SDH0_CLK_EN;
	} else if (host->base_addr == SD1_BASE) {
		PMUA->SDH1_CLK_RES_CTRL |= SDH1_CLK_RES_CTRL_SDH1_CLK_EN;
	} else if (host->base_addr == SD2_BASE) {
		PMUA->SDH2_CLK_RES_CTRL |= SDH2_CLK_RES_CTRL_SDH2_CLK_EN;
	}

	return SDH_TRUE;
}

static uint32_t mrvl_sdh_pmu_disable_peripheral_clk(struct sdhc_host *host)
{
	if (host->base_addr == SD0_BASE) {
		PMUA->SDH0_CLK_RES_CTRL &= ~SDH0_CLK_RES_CTRL_SDH0_CLK_EN;
	} else if (host->base_addr == SD1_BASE) {
		PMUA->SDH1_CLK_RES_CTRL &= ~SDH1_CLK_RES_CTRL_SDH1_CLK_EN;
	} else if (host->base_addr == SD2_BASE) {
		PMUA->SDH2_CLK_RES_CTRL &= ~SDH2_CLK_RES_CTRL_SDH2_CLK_EN;
	}

	return SDH_TRUE;
}

static uint32_t mrvl_sdh_pmu_enable_all_axi_clk(struct sdhc_host *host)
{
	(void)host;
	PMUA->SDH0_CLK_RES_CTRL |= SDH0_CLK_RES_CTRL_SDH0_AXICLK_EN;
	return SDH_TRUE;
}

uint32_t mrvl_sdh_pum_set_clock(struct sdhc_host *host)
{
	uint8_t source = host->clk_source;
	uint8_t div = host->first_div;
	uint8_t time_ct = 50;
	uint32_t ctrl;

	if (host->base_addr == SD0_BASE) {
		/* Set clock source and divide */
		ctrl = PMUA->SDH0_CLK_RES_CTRL;
		ctrl &= ~(SDH0_CLK_RES_CTRL_SDH0_CLK_SEL_MSK | 
		          SDH0_CLK_RES_CTRL_SDH0_CLK_DIV_MSK);
		ctrl |= (source << SDH0_CLK_RES_CTRL_SDH0_CLK_SEL_BASE) | 
		        ((div - 1) << SDH0_CLK_RES_CTRL_SDH0_CLK_DIV_BASE);
		PMUA->SDH0_CLK_RES_CTRL = ctrl;	

		/* Request to change clock */
		PMUA->SDH0_CLK_RES_CTRL |= SDH0_CLK_RES_CTRL_SDH0_CLK_FC_REQ;	
		do {
			ctrl = PMUA->SDH0_CLK_RES_CTRL;
			if (!(ctrl & SDH0_CLK_RES_CTRL_SDH0_CLK_FC_REQ))
				break;
			if (!(--time_ct)) {
				DEBUG_SDH_PRINT("Error: Clock Frequency Change "
				           "Request not finished\r\n");
				break;
			}
			mdelay(10);
		} while(1);
	} else if (host->base_addr == SD1_BASE) {
		/* Set clock source and divide */
		ctrl = PMUA->SDH1_CLK_RES_CTRL;
		ctrl &= ~(SDH1_CLK_RES_CTRL_SDH1_CLK_SEL_MSK | 
		          SDH1_CLK_RES_CTRL_SDH1_CLK_DIV_MSK);
		ctrl |= (source << SDH1_CLK_RES_CTRL_SDH1_CLK_SEL_BASE) | 
		        ((div - 1) << SDH1_CLK_RES_CTRL_SDH1_CLK_DIV_BASE);
		PMUA->SDH1_CLK_RES_CTRL = ctrl;		

		/* Request to change clock */
		PMUA->SDH1_CLK_RES_CTRL |= SDH1_CLK_RES_CTRL_SDH1_CLK_FC_REQ;	
		do {
			ctrl = PMUA->SDH1_CLK_RES_CTRL;
			if (!(ctrl & SDH1_CLK_RES_CTRL_SDH1_CLK_FC_REQ))
				break;
			if (!(--time_ct)) {
				DEBUG_SDH_PRINT("Error: Clock Frequency Change "
				           "Request not finished\r\n");
				break;
			}
			mdelay(10);
		} while(1);
	} else if (host->base_addr == SD2_BASE) {
		/* Set clock source and divide */
		ctrl = PMUA->SDH2_CLK_RES_CTRL;
		ctrl &= ~(SDH2_CLK_RES_CTRL_SDH2_CLK_SEL_MSK | 
		          SDH2_CLK_RES_CTRL_SDH2_CLK_DIV_MSK);
		ctrl |= (source << SDH2_CLK_RES_CTRL_SDH2_CLK_SEL_BASE) | 
		        ((div - 1) << SDH2_CLK_RES_CTRL_SDH2_CLK_DIV_BASE);
		PMUA->SDH2_CLK_RES_CTRL = ctrl;		

		/* Request to change clock */
		PMUA->SDH2_CLK_RES_CTRL |= SDH2_CLK_RES_CTRL_SDH2_CLK_FC_REQ;	
		do {
			ctrl = PMUA->SDH2_CLK_RES_CTRL;
			if (!(ctrl & SDH2_CLK_RES_CTRL_SDH2_CLK_FC_REQ))
				break;
			if (!(--time_ct)) {
				DEBUG_SDH_PRINT("Error: Clock Frequency Change "
				           "Request not finished\r\n");
				break;
			}
			mdelay(10);
		} while(1);
	}

	return SDH_TRUE;
}

uint32_t mrvl_sdh_pmu_switch_clock(struct sdhc_host *host)
{
	if (host->first_div > host->max_first_div) {
		DEBUG_SDH_PRINT("Failed: Required base clock is two small for source clock!\r\n");
		return SDH_FALSE;
	}
	
	if (host->first_div == 0) {
		DEBUG_SDH_PRINT("Failed: denominator used to generate base clock is zero!\r\n");
		return SDH_FALSE;
	}
	host->base_clk = sdh_clk_source[host->clk_source] / host->first_div;	
	
	/* Put peripheral in reset state and disable peripheral and axi clock */
	mrvl_sdh_pmu_reset_peripheral(host);
	mrvl_sdh_pmu_disable_peripheral_clk(host); 	
	
	/* Set clock source and divide */
	mrvl_sdh_pum_set_clock(host);

	/* Enable AXI clock and Release AXI reset */
	mrvl_sdh_pmu_enable_all_axi_clk(host);
	mrvl_sdh_pmu_release_all_axi_reset(host);
	
	/* Enable peripheral clock and Release peripheral reset */
	mrvl_sdh_pmu_enable_peripheral_clk(host);
	mrvl_sdh_pmu_release_peripheral_reset(host);
	
	DEBUG_SDH_PRINT("PMUA->SDH0_CLK_RES_CTRL: 0x%x\r\n", PMUA->SDH0_CLK_RES_CTRL);
	DEBUG_SDH_PRINT("PMUA->SDH1_CLK_RES_CTRL: 0x%x\r\n", PMUA->SDH1_CLK_RES_CTRL);
	DEBUG_SDH_PRINT("PMUA->SDH2_CLK_RES_CTRL: 0x%x\r\n", PMUA->SDH2_CLK_RES_CTRL);
	DEBUG_SDH_PRINT("PMU Set SDH clock Finished!\r\n");

	host->base_clk_ready = 1;
	sdh_update_clock_info(host);
	
	return SDH_TRUE;
}

uint32_t mrvl_sdh_register_device(struct sdh_device *dev)
{
	struct sdhc_host *host;
	uint8_t i;
	
	host = dev->host;
	for (i = 0; i < MAX_CARD_NUMBER; i++) {
		if (host->dev[i] == dev) {
			DEBUG_SDH_PRINT("************** Card has been in SDH host slot %d! *********\r\n", i);
			break;
		}
		
		if (host->dev[i] == NULL) {
			host->dev[i] = dev;
			DEBUG_SDH_PRINT("************** SDH host slot %d has card! *********\r\n", i);

			break;
		}
	}
	if (i >= MAX_CARD_NUMBER) {
		DEBUG_SDH_PRINT("[Error]: SDH has reached maximum card number!\r\n");
		return SDH_FALSE;
	}

	DEBUG_SDH_PRINT("%s card has been registered!\r\n", dev->name);
	return SDH_TRUE;
}

uint32_t mrvl_sdh_unregister_device(struct sdh_device *dev)
{
	struct sdhc_host *host;
	uint8_t i;

	host = dev->host;
	for (i = 0; i < MAX_CARD_NUMBER; i++) {
		if (strcmp((char *)(host->dev[i]->name), (char *)(dev->name)) == 0) {
			host->dev[i] = NULL;
			break;
		}
	}

	if (i >= MAX_CARD_NUMBER) {
		DEBUG_SDH_PRINT("[Error]: Seclected card does not exist in this SDH!\r\n");
		return SDH_FALSE;
	}

	DEBUG_SDH_PRINT("%s card has been unregistered!\r\n", dev->name);
	return SDH_TRUE;
}
