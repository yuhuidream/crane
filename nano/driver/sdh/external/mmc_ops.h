/*
 *  mmc_ops.h
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */

#ifndef MMC_OPS_H
#define MMC_OPS_H

#include "qp_port.h"
#include "sdh_host.h"
#include "mmc.h"

struct mmc_card;
struct dev_request;
struct SDTransReq;
struct scatterlist;

struct mmc_ops_para
{
	uint32_t    start_addr;  /* in blocks */
	uint32_t    size;
	void        *pbuffer;
	uint8_t     trans_mode;
/*
 * if use interrupt, this bit should be set. default 
 * is poll mode
 */
#define MMC_TRANS_TYPE_INT    SDH_TRANS_TYPE_INT  
#define MMC_TRANS_TYPE_PIO    SDH_TRANS_TYPE_PIO

/* Supported now, except for packed command */
#define MMC_TRANS_TYPE_SDMA   SDH_TRANS_TYPE_SDMA  
#define MMC_TRANS_TYPE_ADMA   SDH_TRANS_TYPE_ADMA	
/*
 * For calling from AO, this bit should be set, because the 
 * result is returned through events
 */
#define MMC_TRANS_REQ_FROM_AO_SD SDH_TRANS_REQ_FROM_AO_SD  

/* eMMC erase type definition */
#define MMC_TRANS_TYPE_ERASE        (1<<0)	
#define MMC_TRANS_TYPE_SEC_ERASE    (1<<1)	
#define MMC_TRANS_TYPE_TRIM         (1<<2)
#define MMC_TRANS_TYPE_SEC_TRIM     (1<<3)
#define MMC_TRANS_TYPE_DISCARD      (1<<4)
};

/* len is in blocks */
#define mmc_config_ops_para(offset_addr, buf, len, mode) 	\
{	\
	.start_addr = offset_addr,	\
	.size = len,	\
	.pbuffer = (uint32_t *)buf,	\
	.trans_mode = mode,	\
}

/* macro for eMMC operation type */
#define MMC_OPS_CARD_READ     0
#define MMC_OPS_CARD_WRITE    1
#define MMC_OPS_CARD_ERASE    2

/*
 * eMMC direct init/read/write/erase interface
 * In blocks
 */
/* define this macro to enable packed command */
#define CONFIG_EN_PACK_CMD
#define MMC_MAX_PACK_NUM 64

uint32_t mmc_get_cur_partition_blk_num(void);

uint32_t mmc_card_init(void);

uint32_t mmc_wait_blk_rw_req(struct mmc_ops_para *para, uint32_t rw);
#define mmc_do_data_read(para) mmc_wait_blk_rw_req(para, MMC_OPS_CARD_READ)
#define mmc_do_data_write(para) mmc_wait_blk_rw_req(para, MMC_OPS_CARD_WRITE)

uint32_t mmc_do_erase(struct mmc_ops_para *para);

/*
 * eMMC direct init/read/write/erase interface
 * In bytes
 */
uint32_t mmc_read(uint32_t blk_addr, void *pbuf, uint32_t size);

//#define MMC_DEBUG_ON
#if defined(MMC_DEBUG_ON)
#define DEBUG_MMC_PRINT(fmt, args...) bbu_printf(fmt, ##args)
#else
#define DEBUG_MMC_PRINT(fmt, args...)
#endif

#endif
