/*
 *  mmc.c
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */

#include "bbu.h"
#include "bbu_malloc.h"
#include "timer_dec.h"
#include "pmic.h"
#include "scatterlist.h"
#include "SDH.h"
#include "sdh_def.h"
#include "sdh_device.h"
#include "sdh_host.h"
#include "mmc.h"
#include "sdh_mmc.h"
#include "mmc_ops.h"

static struct mmc_card my_mmc_card;
static struct mmc_card *g_mmc_card = &my_mmc_card;

// static const unsigned int taac_exp[] = 
// {
// 	1,	10,	100,	1000,	10000,	100000,	1000000, 10000000,
// };

// static const unsigned int taac_mant[] = 
// {
// 	0,	10,	12,	13,	15,	20,	25,	30,
// 	35,	40,	45,	50,	55,	60,	70,	80,
// };

// static const unsigned char tran_mant[] = 
// {
// 	0,	10,	12,	13,	15,	20,	25,	30,
// 	35,	40,	45,	50,	55,	60,	70,	80,
// };

/**
 * mmc_send_app_op_cond - start CMD1 to ask card to send its Operating Condition Register.
 * @ocr: host OCR without busy state
 * @rocr: pionter used to store OCR obtained in the response
*/
uint32_t mmc_send_op_cond(struct mmc_card *card, uint32_t ocr, uint32_t *rocr)
{
	struct dev_command cmd;
	uint32_t err;
	uint32_t i;
	
	memset(&cmd, 0, sizeof(struct dev_command));

	cmd.opcode = MMC_CMD1_SEND_OP_COND;
	cmd.arg = ocr;
	cmd.flags = DEV_RSP_R3 | DEV_CMD_BCR;

	for (i = 100; i; i--) {
		err = dev_send_cmd(&(card->dev), &cmd, MMC_CMD_RETRIES);
		if (err)
			break;
					
		if (ocr == 0)
			break;
			
		/* otherwise wait until reset completes */
		if ((cmd.resp[0]) & BIT_31) {
			break;
		}

		err = SDH_TIMEDOUT;
		mdelay(10); /* need to adjust depending on sd card specification */
	}

	if ((!err) && rocr)
		*rocr = cmd.resp[0];

	return err;
}

/**
 * mmc_send_relative_addr - start CMD3 to assign relative address to card
*/
uint32_t mmc_send_relative_addr(struct mmc_card *card, uint16_t rca)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));

	card->dev.rca = rca;
	cmd.opcode = MMC_CMD3_SET_RELATIVE_ADDR;
	cmd.arg = rca << 16;
	cmd.flags = DEV_RSP_R1 | DEV_CMD_AC;
	
	err = dev_send_cmd(&(card->dev), &cmd, MMC_CMD_RETRIES);
	if(err)
		return err;

	card->dev.rca = rca;
	card->dev.current_state = cmd.resp[0]; 
	
	return err;
}

uint32_t mmc_select_card(struct mmc_card *card)
{
	return dev_select_card(&(card->dev), 1);
}

uint32_t mmc_deselect_cards(struct mmc_card *card)
{
	return dev_select_card(&(card->dev), 0);
}

/**
 * mmc_switch_mode_extcsd - start CMD6 to switch mode of operation or modify EXT_CSD registers
*/
uint32_t mmc_switch_mode_extcsd(struct mmc_card *card, uint8_t access, uint8_t index, uint8_t value, uint8_t cmd_set)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));

	cmd.opcode = MMC_CMD6_SWITCH;
	cmd.arg = ((access & 0x3) << 24) | (index << 16) | (value << 8) | cmd_set;
	cmd.flags = DEV_RSP_R1B | DEV_CMD_AC;

	DEBUG_MMC_PRINT("CMD6 argument: 0x%x\r\n", cmd.arg);
	err = dev_send_cmd(&(card->dev), &cmd, MMC_CMD_RETRIES);
	if (err) {
		DEBUG_SDH_PRINT("CMD6 failed!\r\n");
		return err;
	}

	err = dev_wait_card_ready_for_tran(&card->dev, 0);
	if (err) {
		DEBUG_SDH_PRINT("%s: failed to return to trans status!\r\n",
		           __func__);
		return err;	
	}

	if (card->dev.current_state & DEV_SWITCH_ERROR) {
		DEBUG_SDH_PRINT("!!!!!!!!!!!! CMD6 Switch Error !!!!!!!!!\r\n");
		return SDH_FALSE;
	}

	return SDH_TRUE;
}

/**
 * mmc_send_ext_csd - start CMD8 to ask device send its EXT_CSD register as a block of data
*/
uint32_t mmc_send_ext_csd(struct mmc_card *card, uint8_t *rcsd)
{
	struct dev_request drq;
	struct dev_command cmd;
	struct dev_data data;
	struct scatterlist sg;
	uint32_t err;

	memset(&drq, 0, sizeof(struct dev_request));
	memset(&cmd, 0, sizeof(struct dev_command));
	memset(&data, 0, sizeof(struct dev_data));

	drq.cmd = &cmd;
	drq.data = &data;
	drq.trans_mode = MMC_TRANS_TYPE_ADMA; 

	cmd.opcode = MMC_CMD8_SEND_EXT_CSD;
	cmd.arg = 0;
	cmd.flags = DEV_RSP_R1 | DEV_CMD_ADTC;
	cmd.retries = MMC_CMD_RETRIES;

	data.blksz = 512;
	data.blocks = 1;
	data.flags = DEV_DATA_READ;
	data.byte_per_desc = SD_ADMA_MAX_BYTES_PER_DESC;
	data.sg = &sg;
	data.sg_len = 1;

	sg_init_one(&sg, rcsd, data.blksz);
			
	mrvl_sdh_set_timeout(card->dev.host, 250, (card->dev.host)->sdclk);
	dev_wait_for_req(card->dev.host, &drq);

	err = dev_blk_req_err_check(&card->dev, &drq);
	if (err)
		return err;

	return (cmd.error || data.error);
}

/* Bus test data pattern */
static const uint8_t testdata_8bit[8] = { 0x55, 0xaa, 0, 0, 0, 0, 0, 0 };
static const uint8_t testdata_4bit[4] = { 0x5a, 0, 0, 0 };
static const uint8_t testdata_1bit[1] = { 0x80 };
	
uint32_t mmc_send_bus_test(struct mmc_card *card, uint8_t opcode, uint8_t width)
{
	struct dev_request drq;
	struct dev_command cmd;
	struct dev_data data;
	struct scatterlist sg;
	uint8_t data_buf[8];
	uint8_t *test_buf;
	uint32_t err, i;

	memset(&drq, 0, sizeof(struct dev_request));
	memset(&cmd, 0, sizeof(struct dev_command));
	memset(&data, 0, sizeof(struct dev_data));

	if (width == 8)
		test_buf = (uint8_t *)testdata_8bit;
	else if (width == 4)
		test_buf = (uint8_t *)testdata_4bit;
	else if (width == 1)
		test_buf = (uint8_t *)testdata_1bit;
	else {
		DEBUG_SDH_PRINT("Invalid bus width: %d\r\n", width);
		return SDH_FALSE;
	}

	if (opcode == MMC_CMD19_BUSTEST_W)
		memcpy(data_buf, test_buf, width);
	else
		memset(data_buf, 0, width);

	drq.cmd = &cmd;
	drq.data = &data;
	drq.trans_mode = MMC_TRANS_TYPE_PIO;
	
	cmd.opcode = opcode;
	cmd.arg = 0;
	cmd.flags = DEV_RSP_R1 | DEV_CMD_ADTC;
	cmd.retries = MMC_CMD_RETRIES;
	
	data.blksz = width;
	data.blocks = 1;
	
	if (opcode == MMC_CMD14_BUSTEST_R) {
		data.flags = DEV_DATA_READ;
	} else {
		data.flags = DEV_DATA_WRITE;
	}
	data.sg = &sg;
	data.sg_len = 1;

	sg_init_one(&sg, data_buf, data.blksz);
			
	mrvl_sdh_set_timeout(card->dev.host, 500, (card->dev.host)->sdclk);
	dev_wait_for_req(card->dev.host, &drq);
	err = cmd.error || data.error;
	
	if (opcode == MMC_CMD14_BUSTEST_R) {
		for (i = 0; i < width / 4; i++) {
			if ((test_buf[i] ^ data_buf[i]) != 0xff) {
				err = SDH_FALSE;
				break;
			}
		}
	}

	return err;
}

uint32_t mmc_bus_test(struct mmc_card *card, uint8_t bus_width)
{
	uint8_t err, width;

	if (bus_width == MMC_BUS_WIDTH_BIT8)
		width = 8;
	else if (bus_width == MMC_BUS_WIDTH_BIT4)
		width = 4;
	else if (bus_width == MMC_BUS_WIDTH_BIT1)
		return 0; /* no need for test */
	else
		return SDH_FALSE;

	/*
	 * Ignore errors from BUS_TEST_W.  BUS_TEST_R will fail if there
	 * is a problem.  This improves chances that the test will work.
	 */
	mmc_send_bus_test(card, MMC_CMD19_BUSTEST_W, width);
	err = mmc_send_bus_test(card, MMC_CMD14_BUSTEST_R, width);
	return err;
}

/**
 * mmc_set_erase_group_start - Use CMD35 to set address of the first group to be erased
*/
uint32_t mmc_set_erase_group_start(struct mmc_card *card, uint32_t block_start)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));

	cmd.opcode = MMC_CMD35_ERASE_GROUP_START;
	cmd.arg = block_start;
	cmd.flags = DEV_RSP_R1 | DEV_CMD_AC;

	err = dev_send_cmd(&(card->dev), &cmd, MMC_CMD_RETRIES);
	if (err)
		return err;

	if (cmd.resp[0] & DEV_OUT_OF_RANGE) {
		DEBUG_SDH_PRINT("CMD35: Erase Out of ranger error!\r\n");
		return SDH_FALSE;
	}

	return SDH_TRUE;
}

/**
 * mmc_set_erase_group_end - Use CMD36 to set address of the last group to be erased
*/
uint32_t mmc_set_erase_group_end(struct mmc_card *card, uint32_t block_end)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));

	cmd.opcode = MMC_CMD36_ERASE_GROUP_END;
	cmd.arg = block_end;
	cmd.flags = DEV_RSP_R1 | DEV_CMD_AC;

	err = dev_send_cmd(&(card->dev), &cmd, MMC_CMD_RETRIES);
	if (err)
		return err;

	return SDH_TRUE;
}


/**
 * mmc_start_erase - start CMD38 to earse all previously selected write blocks
*/
uint32_t mmc_start_erase(struct mmc_card *card, uint32_t argu)
{
	struct dev_command cmd;
	uint32_t err;

	memset(&cmd, 0, sizeof(struct dev_command));

	cmd.opcode = COM_CMD38_ERASE;
	cmd.arg = argu;	
	cmd.flags = DEV_RSP_R1B | DEV_CMD_AC;

	mrvl_sdh_set_timeout(card->dev.host, 1000, (card->dev.host)->sdclk);
	err = dev_send_cmd(&(card->dev), &cmd, MMC_CMD_RETRIES);
	if (err)
		return err;

	if (cmd.resp[0] & DEV_WP_ERASE_SKIP_ERROR) {
		DEBUG_SDH_PRINT("[Waring]: Erase skip writed protected blocks!\r\n");
	}
	
	return SDH_TRUE;
}

void mmc_decode_cid(struct mmc_card *card)
{
	uint32_t *resp;

	resp = card->dev.raw_cid;
	memset(&card->cid, 0, sizeof(struct mmc_cid));

	/*
	 * SD doesn't currently have a version field so we will
	 * have to assume we can parse this.
	 */
	card->cid.mid			= UNSTUFF_BITS(resp, 120, 8);
	card->cid.cbx			= UNSTUFF_BITS(resp, 112, 2);
	card->cid.oid			= UNSTUFF_BITS(resp, 104, 8);
	card->cid.pnm[0]		= UNSTUFF_BITS(resp, 96, 8);
	card->cid.pnm[1]		= UNSTUFF_BITS(resp, 88, 8);
	card->cid.pnm[2]		= UNSTUFF_BITS(resp, 80, 8);
	card->cid.pnm[3]		= UNSTUFF_BITS(resp, 72, 8);
	card->cid.pnm[4]		= UNSTUFF_BITS(resp, 64, 8);
	card->cid.pnm[5]		= UNSTUFF_BITS(resp, 56, 8);
	card->cid.prv_n		= UNSTUFF_BITS(resp, 52, 4);
	card->cid.prv_m		= UNSTUFF_BITS(resp, 48, 4);
	card->cid.psn			= UNSTUFF_BITS(resp, 16, 32);
	card->cid.mdt_month	= UNSTUFF_BITS(resp, 12, 4);
	card->cid.mdt_year	= UNSTUFF_BITS(resp, 8, 4);
	
	if (card->csd_ext.ext_csd_rev > 4) {
		card->cid.mdt_year += 2013; /* mmc cards year offset */
	} else {
		card->cid.mdt_year += 1997; /* mmc cards year offset */
	}
	return;
}


/*
 * Given a 128-bit response, decode to our card CSD structure.
 */
static int mmc_decode_csd(struct mmc_card *card)
{
	struct mmc_csd *csd = &(card->csd);
	uint32_t e, m, csd_struct;
	uint32_t *resp = card->dev.raw_csd;

	csd_struct = UNSTUFF_BITS(resp, 126, 2);
	csd->structure = csd_struct;
	csd->spec_vers = UNSTUFF_BITS(resp, 122, 4);
	
	//m = UNSTUFF_BITS(resp, 115, 4);
	//e = UNSTUFF_BITS(resp, 112, 3);
	//csd->taac_ns	 = (taac_exp[e] * taac_mant[m] + 9) / 10;
	//csd->nsac_clks	 = UNSTUFF_BITS(resp, 104, 8) * 100;

	//m = UNSTUFF_BITS(resp, 99, 4);
	//e = UNSTUFF_BITS(resp, 96, 3);
	//csd->max_dtr	  = taac_exp[e] * tran_mant[m];
	//csd->cmd_class	  = UNSTUFF_BITS(resp, 84, 12);

	e = UNSTUFF_BITS(resp, 47, 3);
	m = UNSTUFF_BITS(resp, 62, 12);

	csd->capacity	  = (1 + m) << (e + 2);

	csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
	if (m != 0xfff || card->dev.addr_mode == CARD_ADDRESS_MODE_BYTE) {  /* for device density up to 2GB */
		csd->capacity	  = ((csd->capacity) << (csd->read_blkbits)) >> 10;  /* in Kbytes */
	}

	csd->read_bl_partial = UNSTUFF_BITS(resp, 79, 1);
	csd->write_blk_misalign = UNSTUFF_BITS(resp, 78, 1);
	csd->read_blk_misalign = UNSTUFF_BITS(resp, 77, 1);

	csd->dsr_imp = UNSTUFF_BITS(resp, 76, 1);

	csd->vdd_r_curr_min = UNSTUFF_BITS(resp, 59, 3);
	csd->vdd_r_curr_max = UNSTUFF_BITS(resp, 56, 3);
	csd->vdd_w_curr_min = UNSTUFF_BITS(resp, 53, 3);
	csd->vdd_w_curr_max = UNSTUFF_BITS(resp, 50, 3);

	m = UNSTUFF_BITS(resp, 42, 5);
	e = UNSTUFF_BITS(resp, 37, 5);
	csd->erase_grp_size = m;
	csd->erase_grp_mul = e;
	csd->erase_size = (m + 1) * (e + 1);
	
	csd->wp_grp_size = UNSTUFF_BITS(resp, 32, 5);
	csd->wp_grp_en = UNSTUFF_BITS(resp, 31, 1);
	csd->default_ecc = UNSTUFF_BITS(resp, 29, 2);

	csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
	csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
	csd->write_bl_partial = UNSTUFF_BITS(resp, 21, 1);

	/* parameter for mmc card only */	
	csd->file_format_grp =  UNSTUFF_BITS(resp, 16, 1);
	csd->copy = UNSTUFF_BITS(resp, 14, 1);
	csd->perm_write_protect = UNSTUFF_BITS(resp, 13, 1);
	csd->tmp_write_protect = UNSTUFF_BITS(resp, 12, 1);
	csd->file_format = UNSTUFF_BITS(resp, 10, 2);
	csd->ecc = UNSTUFF_BITS(resp, 8, 2);

	return SDH_TRUE;
}


/*
 * Given a 128-bit response, decode to our card CSD structure.
 */
static uint32_t mmc_decode_ext_csd(struct mmc_card *card)
{
	struct mmc_csd_ext *csd_ext = &(card->csd_ext);
	uint32_t err, i;
	uint8_t pext_csd[512];

	memset(pext_csd, 0x0, 512);
	err = mmc_send_ext_csd(card, pext_csd);
	if (err)
		return err;
	
	csd_ext->ext_security_err = pext_csd[505];
	csd_ext->s_cmd_set = pext_csd[504];
	
	csd_ext->hpi_features = pext_csd[503];
	csd_ext->bkops_support = pext_csd[502];
	csd_ext->max_packed_read = pext_csd[501];
	csd_ext->max_packed_writes = pext_csd[500];
	csd_ext->data_tag_support = pext_csd[499];
	csd_ext->tag_unit_size = pext_csd[498];
	csd_ext->tag_res_size = pext_csd[497];
	csd_ext->context_cap = pext_csd[496];
	csd_ext->large_unit_size_m1 = pext_csd[495];
	csd_ext->ext_support = pext_csd[494];

	csd_ext->cache_size = pext_csd[249 + 0] << 0 |
	                      pext_csd[249 + 1] << 8 |
	                      pext_csd[249 + 2] << 16 |
	                      pext_csd[249 + 3] << 24;
	csd_ext->generic_cmd6_time = pext_csd[248];
	csd_ext->power_off_long_tim = pext_csd[247];
	csd_ext->bkops_status = pext_csd[246];
	csd_ext->correctly_prg_sect = pext_csd[242 + 0] << 0 |
	                              pext_csd[242 + 1] << 8 |
	                              pext_csd[242 + 2] << 16 |
	                              pext_csd[242 + 3] << 24;
	csd_ext->ini_timeout_ap = pext_csd[241];
	csd_ext->pwr_cl_ddr_52_360 = pext_csd[239];
	csd_ext->pwr_cl_ddr_52_195 = pext_csd[238];
	csd_ext->pwr_cl_ddr_200_195 = pext_csd[237];
	csd_ext->pwr_cl_ddr_200_130 = pext_csd[236];
	csd_ext->min_perf_ddr_w_8_52 = pext_csd[235];
	csd_ext->min_perf_ddr_r_8_52 = pext_csd[234];
	csd_ext->trim_mult = pext_csd[232];
	csd_ext->sec_feature_support = pext_csd[231];

	csd_ext->sec_erase_mult = pext_csd[230];
	csd_ext->sec_trim_mult = pext_csd[229];
	csd_ext->boot_info = pext_csd[228];
	csd_ext->boot_size_mult = pext_csd[226];
	csd_ext->acc_size = pext_csd[225];
	csd_ext->hc_erase_grp_size = pext_csd[224];
	csd_ext->erase_timeout_mult = pext_csd[223];
	csd_ext->rel_wr_sec_c = pext_csd[222];
	csd_ext->hc_wp_grp_size = pext_csd[221];
	csd_ext->s_c_vcc = pext_csd[220];
	csd_ext->s_c_vccq = pext_csd[219];
	csd_ext->s_a_timeout = pext_csd[217];

	csd_ext->sec_count = pext_csd[212 + 0] << 0 |
	                     pext_csd[212 + 1] << 8 |
	                     pext_csd[212 + 2] << 16 |
	                     pext_csd[212 + 3] << 24;
	if (card->dev.addr_mode == CARD_ADDRESS_MODE_BLOCK) {  /* for device density greater than 2GB */
		card->dev.capacity = csd_ext->sec_count / 2;  /* in KBytes */ 
	}

	csd_ext->min_perf_w_8_52 = pext_csd[210];
	csd_ext->min_perf_r_8_52 = pext_csd[209];
	csd_ext->min_perf_w_8_26_4_52 = pext_csd[208];
	csd_ext->min_perf_r_8_26_4_52 = pext_csd[207];
	csd_ext->min_perf_w_4_26 = pext_csd[206];
	csd_ext->min_perf_r_4_26 = pext_csd[205];
	csd_ext->pwr_cl_26_360 = pext_csd[203];
	csd_ext->pwr_cl_52_360 = pext_csd[202];
	csd_ext->pwr_cl_26_195 = pext_csd[201];
	csd_ext->pwr_cl_52_195 = pext_csd[200];
	csd_ext->partition_switch_time = pext_csd[199];
	csd_ext->out_of_interrupt_time = pext_csd[198];
	csd_ext->driver_strength = pext_csd[197];
	csd_ext->device_type = pext_csd[196];
	csd_ext->csd_str_ver = pext_csd[194];
	csd_ext->ext_csd_rev = pext_csd[192];

	csd_ext->cmd_set = pext_csd[191];
	csd_ext->cmd_set_rev = pext_csd[189];
	csd_ext->power_class = pext_csd[187];
	csd_ext->hs_timing = pext_csd[185];
	csd_ext->bus_width = pext_csd[183];
	csd_ext->erased_mem_cont = pext_csd[181];
	csd_ext->partition_config = pext_csd[179];
	csd_ext->boot_config_prot = pext_csd[178];
	csd_ext->boot_bus_conditions = pext_csd[177];
	csd_ext->erase_group_def = pext_csd[175];
	csd_ext->boot_wp_status = pext_csd[174];
	csd_ext->boot_wp = pext_csd[173];
	csd_ext->user_wp = pext_csd[171];
	csd_ext->fw_config = pext_csd[169];
	csd_ext->rpmb_size_mult = pext_csd[168];
	csd_ext->wr_rel_set = pext_csd[167];
	csd_ext->wr_rel_param = pext_csd[166];
	csd_ext->sanitize_start = pext_csd[165];
	csd_ext->bkops_start = pext_csd[164];
	csd_ext->bkops_en = pext_csd[163];
	csd_ext->rst_n_function = pext_csd[162];
	csd_ext->hpi_mgmt = pext_csd[161];
	csd_ext->partition_support = pext_csd[160];

	csd_ext->max_enh_size_mult[0] = pext_csd[157 + 0];
	csd_ext->max_enh_size_mult[1] = pext_csd[157 + 1];
	csd_ext->max_enh_size_mult[2] = pext_csd[157 + 2];

	csd_ext->partition_attribute = pext_csd[156];
	csd_ext->partition_setting_completed = pext_csd[155];

	csd_ext->gp_size_mult_gp0[0] = pext_csd[143 + 0];
	csd_ext->gp_size_mult_gp0[1] = pext_csd[143 + 1];
	csd_ext->gp_size_mult_gp0[2] = pext_csd[143 + 2];
	csd_ext->gp_size_mult_gp1[0] = pext_csd[146 + 0];
	csd_ext->gp_size_mult_gp1[1] = pext_csd[146 + 1];
	csd_ext->gp_size_mult_gp1[2] = pext_csd[146 + 2];
	csd_ext->gp_size_mult_gp2[0] = pext_csd[149 + 0];
	csd_ext->gp_size_mult_gp2[1] = pext_csd[149 + 1];
	csd_ext->gp_size_mult_gp2[2] = pext_csd[149 + 2];
	csd_ext->gp_size_mult_gp3[0] = pext_csd[152 + 0];
	csd_ext->gp_size_mult_gp3[1] = pext_csd[152 + 1];
	csd_ext->gp_size_mult_gp3[2] = pext_csd[152 + 2];
	
	csd_ext->enh_size_mult[0] = pext_csd[140 + 0];
	csd_ext->enh_size_mult[1] = pext_csd[140 + 1];
	csd_ext->enh_size_mult[2] = pext_csd[140 + 2];
	
	csd_ext->enh_start_addr = pext_csd[136 + 0] << 0 |
	                          pext_csd[136 + 1] << 8 |
	                          pext_csd[136 + 2] << 16 |
	                          pext_csd[136 + 3] << 24;
	csd_ext->sec_bad_blk_mgmnt = pext_csd[134];
	csd_ext->tcase_support = pext_csd[132];
	csd_ext->periodic_wakeup = pext_csd[131];
	csd_ext->program_cid_csd_ddr_support = pext_csd[130];
	csd_ext->native_sector_size = pext_csd[63];
	csd_ext->use_native_sector = pext_csd[62];
	csd_ext->data_sector_size = pext_csd[61];
	csd_ext->ini_timeout_emu = pext_csd[60];
	csd_ext->class_6_ctrl = pext_csd[59];
	csd_ext->dyncap_needed = pext_csd[58];
	
	csd_ext->exception_events_ctrl[0] = pext_csd[56 + 0];
	csd_ext->exception_events_ctrl[1] = pext_csd[56 + 1];
	csd_ext->exception_events_status[0] = pext_csd[54 + 0];
	csd_ext->exception_events_status[1] = pext_csd[54 + 1];

	csd_ext->ext_partitions_attribute = pext_csd[52 + 0] << 0 |
	                                    pext_csd[136 + 1] << 8;

	for (i = 0; i < 15; i++)
		csd_ext->context_conf[i] = pext_csd[37 + i];
	
	csd_ext->packed_command_status = pext_csd[36];
	csd_ext->packed_failure_index = pext_csd[35];
	csd_ext->power_off_notification = pext_csd[34];
	csd_ext->cache_ctrl = pext_csd[33];
	csd_ext->flush_cache = pext_csd[32];

	/* update current partition */
	card->partition = csd_ext->partition_config & 0x7;
	return SDH_TRUE;
}

struct mmc_card *get_mmc_card(void)
{
	return g_mmc_card;
}

uint32_t mmc_set_hs_timing(struct mmc_card *card, uint8_t timing, uint8_t driver_type)
{
	uint32_t err;
	uint8_t value;
	
	if (driver_type & MMC_DRIVER_STRENGTH_DEFAULT) {
		err = mmc_decode_ext_csd(card);
		if (err) {
			DEBUG_SDH_PRINT("Failed: mmc_decode_ext_csd\r\n");
			return err;
		}	
		value = card->csd_ext.hs_timing;
		driver_type = (value & 0xf0) >> 4;
		DEBUG_MMC_PRINT("[Warining]: Keep current driver strength!\r\n");
	} else if (!(card->csd_ext.driver_strength & driver_type)) {
		DEBUG_SDH_PRINT("[Warning]: Do not support this driver strength type!\r\n");
		return SDH_INVAL;
	}

	value = (driver_type << 4) | timing;
	err = mmc_switch_mode_extcsd(card, 0x3, 185, value, 0);
	if (err) {
		DEBUG_SDH_PRINT("Failed: mmc_switch_mode_extcsd\r\n");
		return err;
	}	

	mrvl_sdh_set_host_driver_strength(card->dev.host, driver_type);

	err = mmc_decode_ext_csd(card);
	if (err) {
		DEBUG_SDH_PRINT("Failed: mmc_decode_ext_csd\r\n");
		return err;
	}	
	
	if (value != card->csd_ext.hs_timing) {
		DEBUG_SDH_PRINT("[Error] Write %d to extended CSD register HS_TIMING[185] failed! HS_TIMING[185]: %d\r\n", 
			value, card->csd_ext.hs_timing);
		return SDH_FALSE;
	}

	return SDH_TRUE;	
}


uint32_t mmc_set_bus_width(struct mmc_card *card, uint8_t width, uint8_t ddr_mode)
{
	uint32_t err;
	uint8_t value;
	
	if (width == MMC_BUS_WIDTH_BIT1 && ddr_mode == CARD_BUS_EDGE_MODE_SDR) {
		value = 0;
	} else if (width == MMC_BUS_WIDTH_BIT4 && ddr_mode == CARD_BUS_EDGE_MODE_SDR) {
		value = 1;
	} else if (width == MMC_BUS_WIDTH_BIT4 && ddr_mode == CARD_BUS_EDGE_MODE_DDR) {
		value = 5;
	} else if (width == MMC_BUS_WIDTH_BIT8 && ddr_mode == CARD_BUS_EDGE_MODE_SDR) {
		value = 2;
	} else if (width == MMC_BUS_WIDTH_BIT8 && ddr_mode == CARD_BUS_EDGE_MODE_DDR) {
		value = 6;
	} else {
		DEBUG_SDH_PRINT("[Warning]: Not support current bus width setting!\r\n");
		return SDH_INVAL;
	}

	err = mmc_switch_mode_extcsd(card, 0x3, 183, value, 0);
	if (err)
		return err;
	
	card->dev.current_width = width;
	return SDH_TRUE;
}

/**
 * __mmc_init_and_ident - sd card initialization and identificaiton process
*
 * Command Sequence:
 *	CMD0 -> CMD8 (To support SD 2.0 HC) -> ACMD41 -> CMD11-> CMD2(CID) -> CMD3(RCA)
*/
uint32_t __mmc_card_init_and_ident(struct mmc_card *card)
{
	uint32_t err;
	uint32_t ocr, rocr;
	uint8_t idx = 0, bus_width = 0;	
	uint8_t access_mode, edge_mode, timing;
	uint32_t clock_freq;

	uint8_t bus_widths[] = {
		MMC_BUS_WIDTH_BIT8,
		MMC_BUS_WIDTH_BIT4,
		MMC_BUS_WIDTH_BIT1
	};

	/* Set mmc clk to default speed 200KHz for card in identification mode */
	mrvl_sdh_supply_clk(card->dev.host, MMC_INIT_FREQ, MMC_BUS_SPEED_HS26);  

	/* CMD0 with argument 0x0 to reset sd card to idle state */
	(void)dev_go_idle(&(card->dev));
		
	/* After reset, sd bus width change to 1bit(default) */
	card->dev.current_width = 1;
	mrvl_sdh_change_host_bit_mode(card->dev.host, card->dev.current_width);

	err = mmc_send_op_cond(card, 0, &rocr);	
	if (err) {
		DEBUG_SDH_PRINT("[Error]: CMD1 failed!\r\n");
		return err;
	}
	
	/* 
	 * Host support voltage located in 2.7~3.6V , 1.7~1.95V, and sector mode 
	 */
	ocr = (1 << 30) | (rocr & SD_VDD_28_29);	
	
	/* CMD1 */
	err = mmc_send_op_cond(card, ocr, &rocr);	
	if (err) {
		DEBUG_SDH_PRINT("Error: VDD of this card not support selected voltage range!\r\n");
		return err;
	}
	DEBUG_SDH_PRINT("eMMC OCR register: 0x%x\r\n", rocr);
	
	/* check whether card is in sector mode or byte mode */
	if (rocr & BIT_30) {
		card->dev.addr_mode = CARD_ADDRESS_MODE_BLOCK;
	} else {
		card->dev.addr_mode = CARD_ADDRESS_MODE_BYTE;
	}
	
	/* CMD2 */
	err = dev_all_send_cid(&(card->dev));
	if (err) 
		return err;
	mmc_decode_cid(card);
	
	/* CMD3 */
	(void)mmc_send_relative_addr(card, 0x55);

	/* CMD9 to get CSD of sd card */
	err = dev_send_csd(&(card->dev));
	if (err)
		return err;
	mmc_decode_csd(card);

	/* CMD7 to select the card */
	err = dev_select_card(&(card->dev), 1);
	if (err)
		return err;

	/* CMD8 to get EXT_CSD of mmc card */
	err = mmc_decode_ext_csd(card);
	if (err)
		return err;
#ifndef CONFIG_NANO_BL
	/* check current state is tran or not after CMD7 */
	err = dev_send_card_status(&(card->dev), 0);
	if (err)
		return err;		
	DEBUG_MMC_PRINT("Card current state: %d\r\n", dev_check_card_status(card->dev.current_state));
#endif	
	/* always use 512 bytes blocks */
	(void)dev_set_blocklen(&(card->dev), 512);  

	/* Change to supported maximum bus width */
	for (; idx < ARRAY_SIZE(bus_widths); idx++) {
		bus_width = bus_widths[idx];

		err = mmc_set_bus_width(card, bus_width, CARD_BUS_EDGE_MODE_SDR);
		if (!err) {
			mrvl_sdh_change_host_bit_mode(card->dev.host, bus_width);
			err = mmc_bus_test(card, bus_width);
			if (!err) {
				DEBUG_SDH_PRINT("Bus test procedure find out max bus width: %d-bit\r\n", bus_width);
				DEBUG_SDH_PRINT("Change bus width to %d-bit !\r\n", bus_width);
				break;
			}
		}
	}

#ifdef MMC_CAP_HS200
	if ((card->csd_ext.device_type & MMC_DEVICE_TYPE_HS200_SDR_1V8) &&
	    (idx < 2)) {
		timing = MMC_TIMEING_INTERFACE_HS200;
		edge_mode = CARD_BUS_EDGE_MODE_SDR;
		access_mode = MMC_BUS_SPEED_HS200;
		clock_freq = 208000000;
	} else 
#endif
	if (card->csd_ext.device_type & MMC_DEVICE_TYPE_HS52_SDR_ALL) {
		timing = MMC_TIMEING_INTERFACE_HS;
		if (card->csd_ext.device_type &
			(MMC_DEVICE_TYPE_HS52_DDR_1V2 |
			 MMC_DEVICE_TYPE_HS52_DDR_1V8_3V)) {
			edge_mode = CARD_BUS_EDGE_MODE_DDR;
			access_mode = MMC_BUS_SPEED_HS52_DDR;
		} else {
			edge_mode = CARD_BUS_EDGE_MODE_SDR;
			access_mode = MMC_BUS_SPEED_HS52_SDR;		
		}
		clock_freq = 52000000;
	} else {
		timing = MMC_TIMEING_INTERFACE_BACKWARDS;
		edge_mode = CARD_BUS_EDGE_MODE_SDR;
		access_mode = MMC_BUS_SPEED_HS26;
		clock_freq = 26000000;
	}

	/* Must enable 1.8v, Tx delay of HS200 mode need this */
	mrvl_sdh_control_1v8_enable(card->dev.host, 1);
	err = mmc_set_hs_timing(card, timing, MMC_DRIVER_STRENGTH_DEFAULT);
	if (err) {
		bbu_printf("Failed to switch to selected speed mode!\r\n");
		return err;
	}	

	/* Change to DDR mode if supported */
	if (edge_mode & CARD_BUS_EDGE_MODE_DDR) {
		mmc_set_bus_width(card, card->dev.current_width,
			CARD_BUS_EDGE_MODE_DDR);
		DEBUG_SDH_PRINT("Enable DDR bus mode!\r\n");
	}
	card->dev.access_mode = access_mode;
	mrvl_sdh_select_uhs_mode(card->dev.host, access_mode);
	mrvl_sdh_supply_clk(card->dev.host, clock_freq, access_mode);

#ifdef MMC_CAP_HS200
	if (access_mode == MMC_BUS_SPEED_HS200) {
		err = sdh_tx_rx_clock_tuning(&card->dev,
			SDH_TUNING_ONE_DIMENSION_RX);
		if (err)
			return err;
	}
#endif

#ifndef CONFIG_NANO_BL
	/* after init, eMMC is in user partition default */
	card->partition = MMC_PARTITION_DEFAULT;
	card->partition_addr_offset = 0;
	card->partition_avail_size = mmc_get_cur_partition_blk_num();
#endif			
	return SDH_TRUE;
}

#ifndef CONFIG_NANO_BL
/**
 * __mmc_switch_partiton - eMMC partition switch function
 * @card - eMMC will be operated
 * @partition - select corresponding partition which eMMC will switch to
 * @high_end - flag used to decide whether to just use high-end part of this 
 *             partition, only valid for user area partition now
 */
uint32_t __mmc_switch_partiton(struct mmc_card *card, uint32_t partition)
{
	uint8_t partition_config;
	uint32_t err;
	
	if (partition >= MMC_TOTAL_PARTITION_NUM) {
		DEBUG_SDH_PRINT("error: %s: partition id out of range\r\n",
		           __func__);
		return 1;
	}
	
	/* obtain current partition config */
	err = mmc_decode_ext_csd(card);
	if (err) {
		DEBUG_SDH_PRINT("error: %s: failed to read extended CSD\r\n",
		           __func__);
		return err;
	}
	
	partition_config = (card->csd_ext).partition_config;
	partition_config &= ~0x7;
	partition_config |= partition;
	
	/* write back expected partition config */
	err = mmc_switch_mode_extcsd(card, 0x3, 179, partition_config, 0);
	if (err) {
		DEBUG_SDH_PRINT("error: %s: failed to change PARTITION_ACCESS\r\n",
		           __func__);
		return err;
	}

	card->partition = partition;
	card->partition_addr_offset = 0;
	card->partition_avail_size = mmc_get_cur_partition_blk_num();
	
	return 0;	
}
#endif
