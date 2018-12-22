/*
 *  mmc_ops.c
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */
 
#include "bbu.h"
#include "bbu_malloc.h"
#include "timer_dec.h"
#include "list.h"
#include "SDH.h"
#include "sdh_def.h"
#include "sdh_device.h"
#include "sdh_host.h"
#include "mmc.h"
#include "sdh_mmc.h"
#include "mmc_ops.h"
#include "utils.h"
#include "pmic.h"

#ifndef CONFIG_NANO_BL
static uint32_t mmc_erase_blocks(struct mmc_card *card, uint32_t block_start, 
                    uint32_t block_num, uint32_t erase_type);
#endif

uint32_t mmc_card_init(void)
{	
	struct mmc_card *card;
	uint32_t err;
	
	card = get_mmc_card();
	if (!card) {
		DEBUG_SDH_PRINT("No card exist.\r\n");
		return SDH_NOMEDIUM;
	}
	
	mrvl_sdh_init_for_mmc();

	err = __mmc_card_init_and_ident(card);
	if (err) {
		DEBUG_SDH_PRINT("eMMC card init failed!\r\n");
		return err;
	}

	return SDH_TRUE;	
}

#ifndef CONFIG_NANO_BL
uint32_t mmc_get_cur_partition_blk_num(void)
{
	struct mmc_card *card;	
	uint32_t block_num;
		
	card = get_mmc_card();
	if (!card)
		return 0;
	
	switch (card->partition) {
	case MMC_PARTITION_DEFAULT:
		block_num = (card->dev.capacity * 2);
		break;
	
	case MMC_PARTITION_BOOT1:
	case MMC_PARTITION_BOOT2:
		block_num = card->csd_ext.boot_size_mult << 8;
		break;
	
	/* following partition not supported now */
	case MMC_PARTITION_RPMB:
		block_num = card->csd_ext.rpmb_size_mult << 8;
		break;
	case MMC_PARTITION_GP1:
		block_num = (card->csd_ext.gp_size_mult_gp0[2] << 16) +
		            (card->csd_ext.gp_size_mult_gp0[1] << 8) +
		            (card->csd_ext.gp_size_mult_gp0[0]);
		block_num *= card->csd_ext.hc_wp_grp_size *
		             card->csd_ext.hc_erase_grp_size;
		block_num <<= 8;
		break;
	case MMC_PARTITION_GP2:
		block_num = (card->csd_ext.gp_size_mult_gp1[2] << 16) +
		            (card->csd_ext.gp_size_mult_gp1[1] << 8) +
		            (card->csd_ext.gp_size_mult_gp1[0]);
		block_num *= card->csd_ext.hc_wp_grp_size *
		             card->csd_ext.hc_erase_grp_size;
		block_num <<= 8;
		break;
	case MMC_PARTITION_GP3:
		block_num = (card->csd_ext.gp_size_mult_gp2[2] << 16) +
		            (card->csd_ext.gp_size_mult_gp2[1] << 8) +
		            (card->csd_ext.gp_size_mult_gp2[0]);
		block_num *= card->csd_ext.hc_wp_grp_size *
		             card->csd_ext.hc_erase_grp_size;
		block_num <<= 8;
		break;
	case MMC_PARTITION_GP4:
		block_num = (card->csd_ext.gp_size_mult_gp3[2] << 16) +
		            (card->csd_ext.gp_size_mult_gp3[1] << 8) +
		            (card->csd_ext.gp_size_mult_gp3[0]);
		block_num *= card->csd_ext.hc_wp_grp_size *
		             card->csd_ext.hc_erase_grp_size;
		block_num <<= 8;
		break;
	
	default:
		block_num = 0;
	}
	
	return block_num;
}

/* Erase continuous blocks */
static uint32_t mmc_erase_blocks(struct mmc_card *card, uint32_t block_start, 
                                 uint32_t block_num, uint32_t erase_type)
{
	uint32_t err;
	uint32_t block_end;

	if (block_num == 0) {
		return SDH_TRUE;
	}
		
	err = mmc_set_erase_group_start(card, block_start);
	if (err) {
		DEBUG_SDH_PRINT("Set erase block start failed!\r\n");
		return err;
	}
	
	block_end = block_start + block_num - 1;
	err =  mmc_set_erase_group_end(card, block_end);
	if (err) {
		DEBUG_SDH_PRINT("Set erase block end failed!\r\n");
		return err;
	}
	
	if (erase_type & MMC_TRANS_TYPE_ERASE) {
		err = mmc_start_erase(card, 0x00000000);
	} else if (erase_type & MMC_TRANS_TYPE_SEC_ERASE) {
		if (card->csd_ext.sec_feature_support & BIT_0) {	
			err = mmc_start_erase(card, 0x80000000);
		} else {
			DEBUG_SDH_PRINT("Failed! Secure erase is not supported on this device!\r\n");
			return SDH_FALSE;
		}
	} else if (erase_type & MMC_TRANS_TYPE_TRIM) {
		if (card->csd_ext.sec_feature_support & BIT_4) {
			err = mmc_start_erase(card, 0x00000001);
		} else {
			DEBUG_SDH_PRINT("Failed! Trim is not supported on this device!\r\n");
			return SDH_FALSE;
		}
	} else if (erase_type & MMC_TRANS_TYPE_SEC_TRIM) {		
		if ((card->csd_ext.sec_feature_support & BIT_4) &&
		    (card->csd_ext.sec_feature_support & BIT_0) ) {
			/* Secute Trim step 1*/
			err = mmc_start_erase(card, 0x80000001);
			if (!err) {
				/* Secute Trim step 2*/
				err = mmc_set_erase_group_start(card, block_start);
				if (err) {
					DEBUG_SDH_PRINT("Set erase block start failed!\r\n");
					return err;
				}
	
				block_end = block_start + block_num - 1;
				err =  mmc_set_erase_group_end(card, block_end);
				if (err) {
					DEBUG_SDH_PRINT("Set erase block end failed!\r\n");
					return err;
				}

				err = mmc_start_erase(card, 0x80008000);
			}
		} else {
			DEBUG_SDH_PRINT("Failed! Secure Trim is not supported on this device!\r\n");
			return SDH_FALSE;	
		}
	} else if (erase_type & MMC_TRANS_TYPE_DISCARD) {	
		err = mmc_start_erase(card, 0x00000003);
	} else {
		DEBUG_SDH_PRINT(" Unsupported erase type!\r\n");
		return SDH_FALSE;
	}

	if (err) {
		DEBUG_SDH_PRINT("Error! Erase block %u to %u failed!\r\n", 
		           block_start, block_end);
		return err;
	}	
	
	err = dev_wait_card_ready_for_tran(&(card->dev), 0);
	if (err) {
		DEBUG_SDH_PRINT("CMD13: Read card status failed. Erasing abort!\r\n");
		return err;	
	}

	if (card->dev.current_state & DEV_OUT_OF_RANGE) {
		DEBUG_SDH_PRINT("Error: Erase Out of ranger!\r\n");
		return SDH_FALSE;
	}
	
	if (card->dev.current_state & DEV_ERASE_SEQ_ERROR) {
		DEBUG_SDH_PRINT("Error: Erase sequence error!\r\n");
		return SDH_FALSE;
	}
	
	if (card->dev.current_state & DEV_WP_ERASE_SKIP_ERROR) {
		DEBUG_SDH_PRINT("[Waring]: Erase skip writed protected blocks!\r\n");
	}

	return SDH_TRUE;
}

#define MMC_MAX_ERASE_STEP_BLOCK 8192 /* Size: 4MB */
uint32_t mmc_select_suit_erase_step(struct mmc_card *card)
{
	(void)card;
	uint32_t step;

	step = MMC_MAX_ERASE_STEP_BLOCK;
	return step;
}

/* Erase continuous blocks */
uint32_t mmc_do_erase(struct mmc_ops_para *para)
{
	uint32_t start_addr = para->start_addr;;
	uint32_t block_num = para->size;;
	uint32_t erase_step;
	uint32_t err, i, j;
	
	struct mmc_card *card = get_mmc_card();
	if (!card) {
		DEBUG_SDH_PRINT("Error: No card exist\r\n");
		return 1;
	}
	
#ifdef MMC_DEBUG_ON	
	if ((para->start_addr + block_num) > card->partition_avail_size) {
		
		bbu_printf("block start: %d\r\n", para->start_addr);
		bbu_printf("block number: %d\r\n", block_num);
		bbu_printf("total blocks: %d\r\n", card->partition_avail_size);
		bbu_printf("error: %s: out of range\r\n", __func__);
		return 1;	
	}
#endif	
	start_addr += card->partition_addr_offset;
	/* If capacity lower than 2GB, turn block address to byte address */
	if (card->dev.addr_mode == CARD_ADDRESS_MODE_BYTE) {
		start_addr = start_addr * 512;
	}

	erase_step = mmc_select_suit_erase_step(card);
	for (i = 0, j = 0; i < block_num; i = i + erase_step) {
		uint32_t blocks;
			
		if (i + erase_step <= block_num) {
			blocks = erase_step;
		} else {
			blocks = block_num - i;
		}
		
		err = mmc_erase_blocks(card, i + start_addr, blocks,
		                       para->trans_mode);
		if (err)
			return err;	

		if ((++j)%20 == 0)
			bbu_printf(".");
	}
	
	return 0;
}

uint32_t mmc_write(uint32_t blk_addr, void *pbuf, uint32_t size)
{
	struct mmc_ops_para para;
	uint32_t err;
	uint8_t *temp_buf = pbuf;
	
	/*
	 * transform byte address and byte size to block address and block number
	 */
	blk_addr = blk_addr >> 9;
	size = (size + 511) / 512;

	do {
		para.size = (size >= MAX_BLOCK_NUM) ? MAX_BLOCK_NUM : size;
		para.start_addr = blk_addr;
		para.pbuffer = temp_buf;
		para.trans_mode = MMC_TRANS_TYPE_ADMA;

		err = mmc_do_data_write(&para);
		if (err)
			return err;

		if (size < MAX_BLOCK_NUM)
			break;

		size -= MAX_BLOCK_NUM;
		blk_addr += para.size;
		temp_buf += para.size << 9;
	} while (1);
	
	return SDH_TRUE;
}
#endif

uint32_t mmc_read(uint32_t blk_addr, void *pbuf, uint32_t size)
{
	struct mmc_ops_para para;
	uint32_t err;
	uint8_t *temp_buf = pbuf;
	
	/*
	 * transform byte address and byte size to block address and block number
	 */
	blk_addr = blk_addr >> 9;
	size = (size + 511) / 512;

	do {
		para.size = (size >= MAX_BLOCK_NUM) ? MAX_BLOCK_NUM : size;
		para.start_addr = blk_addr;
		para.pbuffer = temp_buf;
		para.trans_mode = MMC_TRANS_TYPE_ADMA;

		err = mmc_do_data_read(&para);
		if (err)
			return err;

		if (size <= MAX_BLOCK_NUM)
			break;

		size -= MAX_BLOCK_NUM;
		blk_addr += para.size;
		temp_buf += para.size << 9;
	} while (1);

	return SDH_TRUE;
}

/**
 * sd_write_multiple_block for packed command
*/
uint32_t mmc_rw_blks_req(struct SDTransReq *TransReq)
{
	struct mmc_card *card;	
	struct dev_request *drq;
	struct dev_command *cmd;
	struct dev_data *data;
	struct scatterlist *sg;
	uint32_t cmd_arg, i;
	
	card = get_mmc_card();
		
	/* user available area maybe have a offset */
	cmd_arg = ((struct mmc_slist *)(TransReq->table.sgl))->blk_addr;	
	cmd_arg += card->partition_addr_offset;
	
	/* If capacity lower than 2GB, turn block address to byte address */
	if (card->dev.addr_mode == CARD_ADDRESS_MODE_BYTE) {
		cmd_arg = cmd_arg << 9;
	}

	drq = TransReq->drq;
	drq->trans_mode = TransReq->trans_mode;
	cmd = drq->cmd;
	data = drq->data;
	
	if (TransReq->ops_type == MMC_OPS_CARD_WRITE) {
		data->flags = DEV_DATA_WRITE;
		cmd->opcode = COM_CMD25_WRITE_MULTIPLE_BLOCK;
		if (TransReq->table.orig_nents == 1) {
			if ((TransReq->table.sgl)->length == 512)
				cmd->opcode = COM_CMD24_WRITE_BLOCK;
		}
	} else if (TransReq->ops_type == MMC_OPS_CARD_READ) {
		data->flags = DEV_DATA_READ;
		cmd->opcode = COM_CMD18_READ_MULTIPLE_BLOCK;
		if (TransReq->table.orig_nents == 1) {
			if ((TransReq->table.sgl)->length == 512)
				cmd->opcode = COM_CMD17_READ_SINGLE_BLOCK;
		}	
	}

	cmd->arg = cmd_arg;
	cmd->flags = DEV_RSP_R1 | DEV_CMD_ADTC;
	cmd->retries = MMC_CMD_RETRIES;

	if ((cmd->opcode == COM_CMD25_WRITE_MULTIPLE_BLOCK) ||
	    (cmd->opcode == COM_CMD18_READ_MULTIPLE_BLOCK)) {
		if (TransReq->trans_mode & SDH_TRANS_TYPE_SDMA) {
			cmd->auto_cmd = MMC_AUTO_CMD12;
		} else {
			cmd->auto_cmd = MMC_AUTO_CMD23;
		}
	}
	
	data->blksz = 512;  /* Fix block length to 512 Bytes */
	for_each_sg(TransReq->table.sgl, sg, TransReq->table.orig_nents, i) {
		data->blocks += sg->length >> 9;
#ifdef MMC_DEBUG_ON
		if ((((struct mmc_slist *)sg)->blk_addr + (sg->length >> 9)) > 
		                         card->partition_avail_size) {	
			DEBUG_SDH_PRINT("error: %s: out of range!\r\n", __func__);
			DEBUG_SDH_PRINT("start: %d\r\n", ((struct mmc_slist *)sg)->blk_addr);
			DEBUG_SDH_PRINT("number: %d\r\n", sg->length >> 9);
			return SDH_FALSE;
		} 
#endif
	}

	data->byte_per_desc = SD_ADMA_MAX_BYTES_PER_DESC;
	data->sg = TransReq->table.sgl;
	data->sg_len = TransReq->table.orig_nents;
	drq->done = NULL;

	mrvl_sdh_set_timeout(card->dev.host, 2000, (card->dev.host)->sdclk);
	mrvl_sdh_start_request(card->dev.host, drq);

	return SDH_TRUE;
}

uint32_t mmc_wait_blk_rw_req(struct mmc_ops_para *para, uint32_t rw)
{
	struct SDTransReq req;
	struct dev_request drq;
	struct dev_command cmd;
	struct dev_data data;
	struct mmc_slist msg;
	uint32_t err;	

	struct mmc_card *card = get_mmc_card();
	if (!card) {
		DEBUG_SDH_PRINT("Error: No card exist\r\n");
		return SDH_FALSE;
	}

	memset(&drq, 0, sizeof(struct dev_request));
	memset(&cmd, 0, sizeof(struct dev_command));
	memset(&data, 0, sizeof(struct dev_data));
	memset(&msg, 0, sizeof(struct mmc_slist));

	msg.blk_addr = para->start_addr;	
	sg_init_one(&(msg.slist), para->pbuffer, (para->size) << 9);

	drq.cmd = &cmd;
	drq.data = &data;

	req.drq = &drq;
	req.table.sgl = &(msg.slist);
	req.table.orig_nents = 1;
	req.ops_type = rw;
	req.trans_mode = para->trans_mode;

	err = mmc_rw_blks_req(&req);
	if (err)
		return err;
	while (1) {
		mrvl_sdh_wait_req_done(card->dev.host, &drq);
		if (!cmd.error || !cmd.retries || data.error)
			break;
		cmd.retries--;
		cmd.error = 0;
		mrvl_sdh_start_request(card->dev.host, &drq);
	}

	if (cmd.resp[1] & 0x0ffff0001) {
		DEBUG_SDH_PRINT("error: %s: card status: 0x%08x\r\n",
		           __func__, cmd.resp[1]);
	}

	err = dev_blk_req_err_check(&card->dev, &drq);
	if (err)
		return err;

	return (cmd.error || data.error);
}

#ifndef CONFIG_NANO_BL
static uint32_t str_to_number(char *str)
{
	if ((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X'))) {
		return conv_hex(str);
	} else {
		return conv_dec(str);
	}
}

int bbu_mmc_init(int argc, uint8_t **argv)
{
	(void)argc;
	(void)argv;
	uint32_t err;
	
	err = mmc_card_init();
	if (!err)
		bbu_printf("mmc init success\r\n");
	return err;
}

int bbu_mmc_read(int argc, uint8_t **argv)
{
	uint32_t blk_addr;
	void *pbuf;
	uint32_t size;
	uint32_t err;
	uint32_t para_valid;
		
	switch (argc) {
	case 4:
		blk_addr = (uint32_t)str_to_number((char*)argv[1]);
		pbuf = (uint8_t *)str_to_number((char*)argv[2]);
		size = (uint32_t)str_to_number((char*)argv[3]);
		para_valid = 1;
		break;	
	default:
		para_valid = 0;
		break;
	}
	
	if (para_valid) {
		err = mmc_read(blk_addr, pbuf, size);
		if (err) {
			bbu_printf("eMMC read failed!\r\n");
#ifdef CONFIG_MMT
    			bbu_putchar(0x84); 
#endif
			return err;
		} else {
			bbu_printf("eMMC read success!\r\n");
#ifdef CONFIG_MMT
    			bbu_putchar(0x85); 
#endif
		}
	} else {
		bbu_printf("%s --> The argument is not correct!\r\n", argv[0]);
		return SDH_FALSE;	
	}
	
	return SDH_TRUE;

}

int bbu_mmc_write(int argc, uint8_t **argv)
{
	uint32_t blk_addr;
	void *pbuf;
	uint32_t size;
	uint32_t err;
	uint32_t para_valid;
		
	switch (argc) {
	case 4:
		blk_addr = (uint32_t)str_to_number((char*)argv[1]);
		pbuf = (uint8_t *)str_to_number((char*)argv[2]);
		size = (uint32_t)str_to_number((char*)argv[3]);
		para_valid = 1;
		break;	
	default:
		para_valid = 0;
		break;
	}
	
	if (para_valid) {
		err = mmc_write(blk_addr, pbuf, size);
		if (err) {
			bbu_printf("eMMC write failed!\r\n");
			return err;
		} else {
			bbu_printf("eMMC write success!\r\n");
		}
	} else {
		bbu_printf("%s --> The argument is not correct!\r\n", argv[0]);
		return SDH_FALSE;	
	}
	
	return SDH_TRUE;
}

uint32_t mmc_switch_partiton(uint32_t partition)
{
	struct mmc_card *card;
	uint32_t err;
	
	card = get_mmc_card();
	if (!card) {
		DEBUG_SDH_PRINT("error: please init eMMC firstly!\r\n");
		return SDH_FALSE;
	}
	
	err = __mmc_switch_partiton(card, partition);
	if (err) {
		bbu_printf("Partition switch failed!\r\n");
		return err;
	}
	bbu_printf("Switch to Partition %d success!\r\n", card->partition);
	bbu_printf("size (in blocks): 0x%08x\r\n", card->partition_avail_size);
	return SDH_TRUE;
}

void bbu_mmc_switch(int argc, uint8_t **argv)
{
	uint8_t partition;

	switch (argc) {
	case 1:
		mmc_switch_partiton(0);
		break;
	case 2:
		partition = (uint8_t)str_to_number((char*)argv[1]);
		mmc_switch_partiton(partition);
		break;
	default:
		bbu_printf("mswitch --> the arguments are not correct\r\n");
		break;
	}
	return;
}

#ifdef NANO_SDH_ERASE
int bbu_mmc_erase(int argc, uint8_t **argv)
{
	struct mmc_ops_para para;
	uint32_t err, para_valid;
	
	switch (argc) {
	case 1:
		para.size = mmc_get_cur_partition_blk_num();
		para.start_addr = 0;
		para.trans_mode = MMC_TRANS_TYPE_ERASE;
		para_valid = 1;
		break;
	case 3:
		para.size = (uint32_t)str_to_number((char*)argv[1]);
		para.start_addr = (uint32_t)str_to_number((char*)argv[2]);
		para.trans_mode = MMC_TRANS_TYPE_ERASE;
		para_valid = 1;
		break;
	case 4:
		para.size = (uint32_t)str_to_number((char*)argv[1]);
		para.start_addr = (uint32_t)str_to_number((char*)argv[2]);
		para.trans_mode = (uint32_t)str_to_number((char*)argv[3]);
		para_valid = 1;
		break;
	default:
		para_valid = 0;
		break;
	}
	
	if (para_valid) {
		err = mmc_do_erase(&para);
		if (err) {
			bbu_printf("Erase failed!\r\n");
			return err;
		} else {
			bbu_printf("Erase finish!\r\n");
		}
	} else {
		bbu_printf("%s --> The argument is not correct!\r\n", argv[0]);
		return SDH_FALSE;	
	}
	
	return SDH_TRUE;
}
#endif  /* End of #ifdef NANO_SDH_ERASE */
#endif
