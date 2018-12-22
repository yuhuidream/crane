#ifndef H_SQU_DMA_H
#define H_SQU_DMA_H
#include "predefines.h"

typedef struct 
{
	uint32_t byte_count;
	uint32_t src_addr;
	uint32_t dest_addr;
	uint32_t next_desc;
}SQU_DMA_DESC, *pSQU_DMA_DESC;


#define CTRL_SRCDIR_MASK		(0x3 << 2)
#define CTRL_SRCDIR_BASE        2
#define CTRL_DESTDIR_MASK		(0x3 << 4)
#define CTRL_DESTDIR_BASE       4
#define CTRL_BUSRT_MASK			(0x7 << 6)
#define CTRL_BURST_BASE         6
#define CTRL_CHAINMOD			BIT_9
#define CTRL_INTMODE			BIT_10
#define CTRL_TRANSMOD			BIT_11
#define CTRL_CHANEN				BIT_12
#define CTRL_FETCHND			BIT_13
#define CTRL_CHANACT			BIT_14
#define CTRL_SDA				BIT_15
#define CTRL_CDE				BIT_17
#define CTRL_ABR				BIT_20
#define CTRL_SSPMODE			BIT_21


#endif