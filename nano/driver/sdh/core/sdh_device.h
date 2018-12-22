/*
 *  sdh_device.h
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */
 
#ifndef SDH_DEVICE_H
#define SDH_DEVICE_H



#include "predefines.h"
#include <string.h>
#include <stdlib.h>
#include "qp_port.h"
#include "scatterlist.h"


/* Standard common commands                        type  argument         response */

   /* class 0 --- basic */
#define COM_CMD0_GO_IDLE_STATE               0   /* bc                             */
#define COM_CMD2_ALL_SEND_CID                2   /* bcr                        R2  */
#define COM_CMD4_SET_DSR                     4   /* bc   [31:16] DSR               */
#define COM_CMD7_SELECT_CARD                 7   /* ac   [31:16] RCA           R1  */
#define COM_CMD9_SEND_CSD                    9   /* ac   [31:16] RCA           R2  */
#define COM_CMD10_SEND_CID                  10   /* ac   [31:16] RCA           R2  */
#define COM_CMD12_STOP_TRANSMISSION         12   /* ac                         R1b */
#define COM_CMD13_SEND_STATUS               13   /* ac   [31:16] RCA           R1  */
#define COM_CMD15_GO_INACTIVE_STATE         15   /* ac   [31:16] RCA               */

  /* class 2 --- block read */
#define COM_CMD16_SET_READ_BLOCKLEN         16   /* ac   [31:0] block len      R1  */
#define COM_CMD17_READ_SINGLE_BLOCK         17   /* adtc [31:0] data addr      R1  */
#define COM_CMD18_READ_MULTIPLE_BLOCK       18   /* adtc [31:0] data addr      R1  */
#define COM_CMD22_RESERVED                  22
#define COM_CMD23_SET_READ_BLOCK_COUNT      23   /* ac   [31:0] block count    R1  */

  /* class 4 --- block write */
#define COM_CMD16_SET_WRITE_BLOCKLEN         16   /* ac   [31:0] block len     R1  */
#define COM_CMD23_SET_WRITE_BLOCK_COUNT      23   /* ac   [31:0] block count   R1  */
#define COM_CMD24_WRITE_BLOCK                24   /* adtc [31:0] data addr     R1  */
#define COM_CMD25_WRITE_MULTIPLE_BLOCK       25   /* adtc                      R1  */
#define COM_CMD27_PROGRAM_CSD                27   /* adtc                      R1  */

  /* class 6 --- write protection */
#define COM_CMD28_SET_WRITE_PROT             28   /* ac   [31:0] data addr     R1b */
#define COM_CMD29_CLR_WRITE_PROT             29   /* ac   [31:0] data addr     R1b */
#define COM_CMD30_SEND_WRITE_PROT            30   /* adtc [31:0] wpdata addr   R1  */


  /* class 5 --- erase */
#define COM_CMD38_ERASE                      38   /* ac				R1b */

  /* class 7 --- lock card */
#define COM_CMD42_LOCK_UNLOCK                42   /* adtc                      R1  */

  /* class 8 --- application specific */
#define COM_CMD55_APP_CMD		              55   /* ac   [31:16] RCA          R1 */
#define COM_CMD56_GEN_CMD                    56   /* adtc [0] rd/wr            R1  */


struct dev_request;
struct dev_command;
struct dev_data;
struct sdhc_host;
struct scatterlist;

struct sdh_device
{
	/* basic property */
	struct sdhc_host *host;
	uint8_t		*name;
	uint32_t 	rca;
	uint32_t 	type;  
#define CARD_TYPE_MMC		(1<<0)		/* MMC card */
#define CARD_TYPE_SD		(1<<1)		/* SD card */
#define CARD_TYPE_SDIO		(1<<2)		/* SDIO card */
#define CARD_TYPE_SD_COMBO	(1<<3)		/* SD combo (IO+mem) card */
#define CARD_TYPE_UHS_I		(1<<4)

	uint8_t		addr_mode; 
#define CARD_ADDRESS_MODE_BYTE (1<<0)
#define CARD_ADDRESS_MODE_BLOCK (1<<1)
	
	uint8_t 		ready_for_dat;  /* 1 -- correspond to buffer empty signaling on the bus */
	uint32_t 	capacity;  /* 0 -- standard capacity, 1 -- high or extended capacity */
	uint32_t 	erase_size;  /* in 512bytes */
	uint8_t		erased_byte;  /* value of erased bytes */
	uint32_t 	current_state;	
	uint32_t		current_width;	
	uint32_t		edge_mode; 
#define CARD_BUS_EDGE_MODE_DDR (1<<0)
#define CARD_BUS_EDGE_MODE_SDR (1<<1)
	
	uint16_t	access_mode;
	uint32_t		current_speed;
	uint32_t		raw_scr[2];
	uint32_t 	raw_cid[4];
	uint32_t 	raw_csd[4];
	
	uint8_t selected;
};


#define UNSTUFF_BITS(resp,start,size)					\
({								\
	const uint32_t __size = size;				\
	const uint32_t __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
	const uint32_t __off = 3 - ((start) / 32);			\
	const uint32_t __shft = (start) & 31;			\
	uint32_t __res;						\
								\
	__res = resp[__off] >> __shft;				\
	if (__size + __shft > 32)				\
		__res |= resp[__off-1] << ((32 - __shft) % 32);	\
	__res & __mask;						\
})



struct dev_command
{
	uint32_t opcode;
	uint32_t arg;
	uint32_t resp[4];
	uint32_t flags;		/* expected response type */
#define DEV_RSP_PRESENT	(1 << 0)
#define DEV_RSP_136	(1 << 1)		/* 136 bit response */
#define DEV_RSP_CRC	(1 << 2)		/* expect valid crc */
#define DEV_RSP_BUSY	(1 << 3)		/* card may send busy */
#define DEV_RSP_OPCODE	(1 << 4)		/* response contains opcode */

#define DEV_CMD_MASK	(3 << 5)		/* non-SPI command type */
#define DEV_CMD_AC	(0 << 5)
#define DEV_CMD_ADTC	(1 << 5)
#define DEV_CMD_BC	(2 << 5)
#define DEV_CMD_BCR	(3 << 5)

/*
 * These are the native response types, and correspond to valid bit
 * patterns of the above flags.  One additional valid pattern
 * is all zeros, which means we don't expect a response.
 */
#define DEV_RSP_NONE	(0)
#define DEV_RSP_R1	(DEV_RSP_PRESENT|DEV_RSP_CRC|DEV_RSP_OPCODE)
#define DEV_RSP_R1B	(DEV_RSP_PRESENT|DEV_RSP_CRC|DEV_RSP_OPCODE|DEV_RSP_BUSY)
#define DEV_RSP_R2	(DEV_RSP_PRESENT|DEV_RSP_136|DEV_RSP_CRC)
#define DEV_RSP_R3	(DEV_RSP_PRESENT|DEV_RSP_CRC)
#define DEV_RSP_R4	(DEV_RSP_PRESENT|DEV_RSP_OPCODE|DEV_RSP_CRC)
#define DEV_RSP_R5	(DEV_RSP_PRESENT|DEV_RSP_CRC|DEV_RSP_OPCODE)
#define DEV_RSP_R6	(DEV_RSP_PRESENT|DEV_RSP_CRC|DEV_RSP_OPCODE)
#define DEV_RSP_R7	(DEV_RSP_PRESENT|DEV_RSP_CRC|DEV_RSP_OPCODE)

#define dev_resp_type(cmd)	((cmd)->flags & (DEV_RSP_PRESENT|DEV_RSP_136|DEV_RSP_CRC|DEV_RSP_BUSY|DEV_RSP_OPCODE))


/*
 * These are the command types.
 */
#define dev_cmd_type(cmd)	((cmd)->flags & DEV_CMD_MASK)

	uint32_t retries;
	uint32_t error;		/* command error */
/*
 * Standard errno values are used for errors, but some have specific
 * meaning in the MMC layer:
 *
 * ETIMEDOUT    Card took too long to respond
 * EILSEQ       Basic format problem with the received or sent data
 *              (e.g. CRC check failed, incorrect opcode in response
 *              or bad end bit)
 * EINVAL       Request cannot be performed because of restrictions
 *              in hardware and/or the driver
 * ENOMEDIUM    Host can determine that the slot is empty and is
 *              actively failing requests
 */
	
	uint32_t auto_cmd;
#define SDH_DISABLE_AUTO_CMD	0
#define SDH_AUTO_CMD23	(1<<0)
#define SDH_AUTO_CMD12	(1<<1)

	uint8_t wait_cmd_done;  /* whether to wait command done or not */

	struct dev_request *drq;
};


typedef struct
{
	uint16_t		valid:1;
	uint16_t		end:1;
	uint16_t		dma_int:1;
	uint16_t		reserved1:1;

	uint16_t		act:2;
#define SD_ADMA_ATTR_NOP 	0
#define SD_ADMA_ATTR_TRAN 	2
#define SD_ADMA_ATTR_LINK	3

	uint16_t 	reserved2:10;	
	
	uint16_t		len;
	uint32_t		address;
}__attribute__((__packed__)) adma_descriptor_line_t;


struct dev_data
{
	//uint32_t		timeout_ns;	/* data timeout (in ns, max 80ms) */
	//uint32_t		timeout_clks;	/* data timeout (in clocks) */
	uint32_t		blksz;		/* data block size */
	uint32_t		blocks;		/* number of blocks */
	uint32_t		error;		/* data error */

	uint32_t		flags;
#define DEV_DATA_WRITE	(1 << 8)
#define DEV_DATA_READ	(1 << 9)

	uint32_t		bytes_xfered;
	
	void *desc_malloc_buffer;  /* point to Adma decription, need to free this memory after adma finish */
	adma_descriptor_line_t *first_descriptor;
	uint32_t descriptor_num;
	uint32_t byte_per_desc;

	void *pbuffer;  /* pointer to data which will be sent or received */
	struct scatterlist *sg;
	uint32_t sg_len;
	struct dev_request	*drq;		/* associated request */

}; 

struct dev_request
{
	struct dev_command 	*cmd;
	struct dev_data 	*data;
	uint32_t 		trans_mode;
	void			(*done)(struct dev_request *);/* completion function */
};


struct mmc_slist 
{
	struct scatterlist slist;
	uint32_t blk_addr;
	uint32_t id; /* save request indentification */
	uint32_t *dma_desc_addr;
};

struct mmc_table 
{
	struct scatterlist *sgl;	/* the list */
	unsigned int nents;		/* number of mapped entries */
	unsigned int orig_nents;	/* original size of list */
};


/* card status */
#define DEV_OUT_OF_RANGE BIT_31
#define DEV_ADDRESS_ERROR BIT_30
#define DEV_BLOCK_LEN_ERROR_29
#define DEV_ERASE_SEQ_ERROR BIT_28
#define DEV_ERASE_PARAM_ERROR BIT_27
#define DEV_WP_VIOLATION_ERROR BIT_26
#define DEV_CARD_IS_LOCKED BIT_25
#define DEV_LOCK_UNLOCK_FAILED BIT_24
#define DEV_COM_CRC_ERROR BIT_23
#define DEV_ILLEGAL_COMMAND_ERROR BIT_22

#define DEV_CARD_ECC_FAILED BIT_21
#define DEV_CC_ERROR BIT_20
#define DEV_UNKNOWN_ERROR BIT_19
#define DEV_CSD_OVERWRITE_ERROR BIT_16

#define DEV_WP_ERASE_SKIP_ERROR BIT_15
#define DEV_CARD_ECC_DISABLED BIT_14
#define DEV_ERASE_RESET BIT_13

#define DEV_CURRENT_STATE_MASK SHIFT9(0x0f)
#define DEV_CURRENT_STATE_BASE 9
#define DEV_READY_FOR_DATA BIT_8
#define DEV_SWITCH_ERROR BIT_7
#define DEV_EXCEPTION_EVENT BIT_6
#define DEV_APP_CMD BIT_5
#define DEV_ASK_SEQ_ERROR BIT_3


#define DEV_CURRENT_STATE_MASK SHIFT9(0x0f)

#define DEV_CURRENT_STATE_INACTIVE 	0xff
#define DEV_CURRENT_STATE_IDLE 		0
#define DEV_CURRENT_STATE_READY 		1
#define DEV_CURRENT_STATE_IDENT 		2
#define DEV_CURRENT_STATE_STANDYBY 	3
#define DEV_CURRENT_STATE_TRANSFER 	4
#define DEV_CURRENT_STATE_SEND 		5
#define DEV_CURRENT_STATE_RECEIVE 	6
#define DEV_CURRENT_STATE_PROGRAM 	7
#define DEV_CURRENT_STATE_DISCONNECT 8


#define dev_check_card_status(current_state)	\
({	\
	uint32_t stat;	\
\
	stat = current_state & DEV_CURRENT_STATE_MASK;	\
	stat = stat >>  DEV_CURRENT_STATE_BASE;	\
\
	stat;	\
})

void dev_wait_for_req(struct sdhc_host *host, struct dev_request *drq);
uint32_t dev_send_cmd(struct sdh_device *dev, struct dev_command *cmd, uint32_t retries);
uint32_t dev_wait_for_app_cmd(struct sdh_device *dev, struct dev_command *cmd, uint32_t retries);
uint32_t dev_app_cmd(struct sdh_device *dev);
uint32_t dev_go_idle(struct sdh_device *dev);
uint32_t dev_all_send_cid(struct sdh_device *dev);
uint32_t dev_send_csd(struct sdh_device *dev);
uint32_t dev_select_card(struct sdh_device *dev, uint8_t sel);
uint32_t dev_send_card_status(struct sdh_device *dev, uint8_t in_tuning);
uint32_t dev_stop_transmission(struct sdh_device *dev);
uint32_t dev_wait_card_ready_for_tran(struct sdh_device *dev, uint8_t in_tuning);
uint32_t dev_set_blocklen(struct sdh_device *dev, uint32_t blocklen);
uint32_t dev_start_erase(struct sdh_device *dev);
uint32_t dev_send_tuning_block(struct sdh_device *dev, uint32_t opcode);
uint32_t dev_blk_req_err_check(struct sdh_device *dev, struct dev_request *drq);

#endif


