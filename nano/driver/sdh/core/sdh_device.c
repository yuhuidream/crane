/*
 *  sdh_device.c
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */
 
#include "sdh_def.h"
#include "sdh_device.h"
#include "sdh_host.h"
#include "timer_dec.h"
#include "SDH.h"
#include "list.h"
#include "pmic.h"
#include "bbu_malloc.h"
#include "scatterlist.h"

#define DEV_CMD_RETRIES 3
static uint32_t dev_blk_err_recovery(struct sdh_device *dev);

/**
 * dev_wait_for_req - start a request and wait for completion
 * @host: sdh host to start command
 * @drq: dev request to start
 *
 * Start a new MMC custom command request for a host, and wait
 * for the command to complete. Does not attempt to parse the
 * response.
 */
void dev_wait_for_req(struct sdhc_host *host, struct dev_request *drq)
{
	struct dev_command *cmd = drq->cmd;
	struct dev_data *data = drq->data;

	mrvl_sdh_start_request(host, drq);
	while (1) {
		mrvl_sdh_wait_req_done(host, drq);
		if (!cmd->error || !cmd->retries || (data && data->error))
			break;

		cmd->retries--;
		cmd->error = 0;
		mrvl_sdh_start_request(host, drq);
	}
}

/**
 * dev_send_cmd -  start a command and wait for completion
 * @cmd: command to start
*
 * Start a new command for a host, and wait for the command
 * to complete. Return any error that occured while the command
 * was executing. Do not attempt to parse the respone.
 */

uint32_t dev_send_cmd(struct sdh_device *dev, struct dev_command *cmd, uint32_t retries)
{
	struct dev_request drq;

	/* Put host APIs here to realize commands */
	memset(&drq, 0, sizeof(struct dev_request));
	memset(cmd->resp, 0, sizeof(cmd->resp));
	cmd->retries = retries;
	
	drq.cmd = cmd;
	drq.data = NULL;
	drq.done = NULL;

	dev_wait_for_req(dev->host, &drq);

	return (cmd->error);
}

/**
 * dev_go_idle - start CMD0 to reset card
*
 * Resets all cards to idle state. 
*/
uint32_t dev_go_idle(struct sdh_device *dev)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));

	cmd.opcode = COM_CMD0_GO_IDLE_STATE;
	cmd.arg = 0;
	cmd.flags = DEV_RSP_NONE | DEV_CMD_BC;
	
	err = dev_send_cmd(dev, &cmd, DEV_CMD_RETRIES);
	dev->current_state = cmd.resp[0];
	
	return err;
}


/**
 * dev_app_cmd - start CMD55 to execute application-specific commands
*
 * Indicates to the card that the next command is an application specific
 * command rather than a standard command. 
*/
uint32_t dev_app_cmd(struct sdh_device *dev)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));

	cmd.opcode = COM_CMD55_APP_CMD;

	if (dev) {
		cmd.arg = ((dev->rca) & 0xffff) << 16;
		cmd.flags = DEV_RSP_R1 | DEV_CMD_AC;
	} else {
		cmd.arg = 0;
		cmd.flags = DEV_RSP_R1 | DEV_CMD_BCR;
	}
	
	err = dev_send_cmd(dev, &cmd, 0);
	if(err)
		return err;

	dev->current_state = cmd.resp[0];
	return err;
}


/**
 *	dev_wait_for_app_cmd - start an application command and wait for completion
 *	@card: Card to send COM_CMD55_APP_CMD to
 *	@cmd: command to start
 *	@retries: maximum number of retries
 *
 *	Sends a COM_CMD55_APP_CMD, checks the card response, sends the command
 *	in the parameter and waits for it to complete. Return any error
 *	that occurred while the command was executing.  Do not attempt to
 *	parse the response.
 */
uint32_t dev_wait_for_app_cmd(struct sdh_device *dev,
	struct dev_command *cmd, uint32_t retries)
{
	struct dev_request drq;
	uint32_t i, err;

	/*
	 * We have to resend COM_CMD55_APP_CMD for each attempt so
	 * we cannot use the retries field in dev_command.
	 */
	for (i = 0;i <= retries;i++) {
		err = dev_app_cmd(dev);
		if (err) 
			continue;

		memset(&drq, 0, sizeof(struct dev_request));
		memset(cmd->resp, 0, sizeof(cmd->resp));
		cmd->retries = 0;

		drq.cmd = cmd;
		drq.data = NULL;

		dev_wait_for_req(dev->host, &drq);

		err = cmd->error;
		if (!err)
			break;
	}

	return err;
}



/**
 * dev_all_send_cid - start CMD2 to ask card to send the CID numbers
*/
uint32_t dev_all_send_cid(struct sdh_device *dev)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));

	cmd.opcode = COM_CMD2_ALL_SEND_CID;
	cmd.arg = 0;
	cmd.flags = DEV_RSP_R2 | DEV_CMD_BCR;
	
	err = dev_send_cmd(dev, &cmd, DEV_CMD_RETRIES);
	if(err)
		return err;

	memcpy(dev->raw_cid, cmd.resp, sizeof(cmd.resp[0]) * 4);
	return SDH_TRUE;
}



/**
 * sd_select_card - start CMD7 to toggle a card between the stand-by and 
 * transfer states or between programming and disconnect states.
*/
uint32_t dev_select_card(struct sdh_device *dev, uint8_t sel)
{
	struct dev_command cmd;
	uint32_t err;

	if (sel && dev->selected) {
		DEBUG_SDH_PRINT("card has been already selected!\r\n");
		return 0;
	}
	if (!sel && !dev->selected) {
		DEBUG_SDH_PRINT("card has been already deselected!\r\n");
		return 0;
	}
	memset(&cmd, 0, sizeof(struct dev_command));
	
	cmd.opcode = COM_CMD7_SELECT_CARD;
	
	if (sel) {
		cmd.arg = ((dev->rca) & 0xffff) << 16;
		cmd.flags = DEV_RSP_R1B | DEV_CMD_AC;
	} else {
		cmd.arg = 0;	/* If not designate card, just deselect all card */
		cmd.flags = DEV_RSP_NONE | DEV_CMD_AC;	
	}
	
	err = dev_send_cmd(dev, &cmd, DEV_CMD_RETRIES);
	if(err)
		return err;

	if (sel)
		dev->selected = 1;
	else
		dev->selected = 0;
	dev->current_state = cmd.resp[0]; 
	
	return err;
}


/**
 * dev_send_csd - start CMD9 to ask card to send interface condition, include
 * host supply voltage information and whether card supports voltage.
*/
uint32_t dev_send_csd(struct sdh_device *dev)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));
	
	cmd.opcode = COM_CMD9_SEND_CSD;
	cmd.arg = ((dev->rca) & 0xffff) << 16;
	cmd.flags = DEV_RSP_R2 | DEV_CMD_AC;
	
	err = dev_send_cmd(dev, &cmd, DEV_CMD_RETRIES);
	if(err)
		return err;

	memcpy(dev->raw_csd, cmd.resp, sizeof(cmd.resp[0]) * 4);

	return err;
}

uint32_t dev_send_stop(struct sdh_device *dev)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));

	cmd.opcode = COM_CMD12_STOP_TRANSMISSION;
	cmd.flags = DEV_RSP_R1B | DEV_CMD_AC;
	err = dev_send_cmd(dev, &cmd, 5);
	return err;
}

/**
 * dev_send_card_status - start CMD13 to ask addressed card sends its status register.
*/
uint32_t dev_send_card_status(struct sdh_device *dev, uint8_t in_tuning)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));
	
	if (in_tuning) {
		cmd.opcode = COM_CMD13_SEND_STATUS | COM_SDH_TX_TUNNING;
	} else {
		cmd.opcode = COM_CMD13_SEND_STATUS;
	}
	cmd.arg = ((dev->rca) & 0xffff) << 16;
	cmd.flags = DEV_RSP_R1 | DEV_CMD_AC;

	err = dev_send_cmd(dev, &cmd, DEV_CMD_RETRIES);
	if(err)
		return err;

	dev->current_state = cmd.resp[0];
	
	if (cmd.resp[0] & 0x0ffff0001) {
		DEBUG_SDH_PRINT("Exist error in 32-bit card status: 0x%x\r\n", cmd.resp[0]);
	}
	DEBUG_SDH_PRINT("CMD13 response: 32-bit card status -- 0x%x\r\n", cmd.resp[0]);

	if (cmd.resp[0] & DEV_CARD_IS_LOCKED) {
		DEBUG_SDH_PRINT("Error: Card has been locked!\r\n");
		return SDH_INVAL;
	}	

	return err;
}

uint32_t dev_wait_card_ready_for_tran(struct sdh_device *dev, uint8_t in_tuning)
{
	uint32_t timeout = 10000;
	uint32_t err;

	do {
		dev->current_state = 0xffffffff;
		err = dev_send_card_status(dev, in_tuning);
		if(err)
			return err;
		
		if (dev_check_card_status(dev->current_state) ==
			DEV_CURRENT_STATE_PROGRAM) {
			DEBUG_SDH_PRINT("Card is programming...\r\n");
		}

		if ((dev->current_state & DEV_READY_FOR_DATA) == 0) {
			DEBUG_SDH_PRINT("Waiting Card ready for data...\r\n");
		}
				
		if ((dev_check_card_status(dev->current_state) ==
			DEV_CURRENT_STATE_TRANSFER) && 
		    (dev->current_state & DEV_READY_FOR_DATA)) {
			break;
		}		

		if (!timeout) {
			DEBUG_SDH_PRINT("err: stuck in programming state, "
				"card status:0x%x\r\n", dev->current_state);
			return SDH_FALSE;
		}
		timeout--;
		mdelay(1);
	} while (1);
	
	return SDH_TRUE;
}

/**
 * dev_set_blocklen -  Start CMD16 to set block length for SCSD
*/
uint32_t dev_set_blocklen(struct sdh_device *dev, uint32_t blocklen)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));

	cmd.opcode = COM_CMD16_SET_READ_BLOCKLEN;
	cmd.arg = blocklen;
	cmd.flags = DEV_RSP_R1 | DEV_CMD_AC;

	err = dev_send_cmd(dev, &cmd, DEV_CMD_RETRIES);
	return err;
}

#ifdef CONFIG_NANO_BL
/* RX tunning pattern for HS200/SDR104 mode */
static const uint8_t tuning_pattern_8bit[] = 
{
	0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00,  0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc, 0xcc,  0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff, 0xff,  0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee, 0xff,
	0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd, 0xdd,  0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xbb,  0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff, 0xff,  0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee, 0xff,
	0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00,  0x00, 0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc,  0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff,  0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee,
	0xff, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd,  0xdd, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff,  0xbb, 0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff,  0xff, 0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee, 
};

static const uint8_t tuning_pattern_4bit[] = 
{
	0xff, 0x0f, 0xff, 0x00,   0xff, 0xcc, 0xc3, 0xcc,   0xc3, 0x3c, 0xcc, 0xff,   0xfe, 0xff, 0xfe, 0xef,
	0xff, 0xdf, 0xff, 0xdd,   0xff, 0xfb, 0xff, 0xfb,   0xbf, 0xff, 0x7f, 0xff,   0x77, 0xf7, 0xbd, 0xef,
	0xff, 0xf0, 0xff, 0xf0,   0x0f, 0xfc, 0xcc, 0x3c,   0xcc, 0x33, 0xcc, 0xcf,   0xff, 0xef, 0xff, 0xee,
	0xff, 0xfd, 0xff, 0xfd,   0xdf, 0xff, 0xbf, 0xff,   0xbb, 0xff, 0xf7, 0xff,   0xf7, 0x7f, 0x7b, 0xde,
};

/**
 * dev_send_tuning_block - start CMD21/CMD19 to ask device send tuning pattern 
 * for HS200/SDR104 optimal sampling poing detection
 */
uint32_t dev_send_tuning_block(struct sdh_device *dev, uint32_t opcode)
{
	struct dev_request drq;
	struct dev_command cmd;
	struct dev_data data;
	struct scatterlist sg;
	uint8_t rpattern[128];
	uint8_t *tuning_pattern;

	memset(&drq, 0, sizeof(struct dev_request));
	memset(&cmd, 0, sizeof(struct dev_command));
	memset(&data, 0, sizeof(struct dev_data));

	drq.cmd = &cmd;
	drq.data = &data;
	drq.trans_mode = MMC_TRANS_TYPE_PIO;

	cmd.opcode = opcode;
	cmd.arg = 0;
	cmd.flags = DEV_RSP_R1 | DEV_CMD_ADTC;

	if (dev->current_width == 4) {
		data.blksz = 64;
		tuning_pattern = (uint8_t *)tuning_pattern_4bit;
	} else if (dev->current_width == 8) {
		data.blksz = 128;
		tuning_pattern = (uint8_t *)tuning_pattern_8bit;
	} else {
		bbu_printf("err: CMD%d not support in current bus width: %d\r\n",
		            opcode & 0x3f, dev->current_width);
		return SDH_FALSE;
	}

	memset(rpattern, 0, data.blksz);

	data.blocks = 1;
	data.flags = DEV_DATA_READ;
	data.byte_per_desc = SD_ADMA_MAX_BYTES_PER_DESC;
	data.sg = &sg;
	data.sg_len = 1;

	sg_init_one(&sg, rpattern, data.blksz);

	mrvl_sdh_set_timeout(dev->host, 50, dev->host->sdclk);
	dev_wait_for_req(dev->host, &drq);
	if (cmd.error || data.error) {
		return SDH_FALSE;
	}

	if (!dev->host->hw_tune) {
		uint8_t i;
		for (i = 0; i < data.blksz; i++) {
			if (rpattern[i] != tuning_pattern[i]) {
				return SDH_INVAL;
			}
		}
	}
	return SDH_TRUE;
}
#endif

static uint32_t dev_blk_err_recovery(struct sdh_device *dev)
{
	uint32_t err;

	err = dev_send_card_status(dev, 0);
	if (err)
		return err;

	if ((dev_check_card_status(dev->current_state) ==
		DEV_CURRENT_STATE_RECEIVE) ||
	    (dev_check_card_status(dev->current_state)) ==
		DEV_CURRENT_STATE_SEND) {

		err = dev_send_stop(dev);
		if (err)
			return err;
	}
	return err;
}

uint32_t dev_blk_req_err_check(struct sdh_device *dev, struct dev_request *drq)
{
	struct dev_command *cmd = drq->cmd;
	struct dev_data *data = drq->data;
	uint32_t err;

	if (cmd->error || (data && data->error)) {
		err = dev_blk_err_recovery(dev);
		if (err) {
			bbu_printf("err: unrecoverable sdh error happen\r\n");
			return err;
		}
	}

	err = dev_wait_card_ready_for_tran(dev, 0);	
	if (err) {
		bbu_printf("err: failed to return to transfer state\r\n");
		return err;
	}

	return err;
}
