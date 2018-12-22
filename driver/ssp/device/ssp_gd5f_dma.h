#ifndef _SSP_GD5F_DMA_H_
#define _SSP_GD5F_DMA_H_
#include "predefines.h"
#include "DMA.h"
#include "ssp_gd5f.h"

#define	SSP_GD5F_DMA_TX_CHANNEL			6
#define SSP_GD5F_DMA_RX_CHANNEL			7

typedef enum{
	WIPE_2112 = 0,
	WIPE_2048,
	WIPE_64,
	WIPE_16
}DMA_WIPE;

//typedef struct DMA_CMD_BITS_TAG{
//	uint32_t Length				:13; //Bits [12:0]: Length of transfer in Bytes
//	uint32_t Reserved4		    :1; //Bit 13: reserved
//	uint32_t Width			    :2; //Bits [15:14]: Width of onchip peripheral - 0=reserved, 1=1B, 2=2B, 3=4B
//	uint32_t MaxBurstSize	    :2; //Bits [17:16]: Max Burst Size - 0=reserved, 1=8B, 2=16B, 3=32B
//	uint32_t Reserved3		    :3; //Bits [20:18]: reserved
//	uint32_t EndIRQEn		    :1; //Bit 21: end interrupt enable
//	uint32_t StartIRQEn			:1; //Bit 22: start interrupt enable
//	uint32_t AddrMode		    :1; //Bit 23: Addressing mode for descriptor compare - 0=target has addr/1=target has data
//	uint32_t Reserved2		    :1; //Bit 24: reserved
//	uint32_t CmpEn			    :1; //Bit 25: Compare Enable - descriptor branching compare enable
//	uint32_t Reserved1		    :2; //Bits [27:26]: reserved
//	uint32_t FlowTrg		    :1;	//Bit 28: Target flow control - 1=wait for request signal
//	uint32_t FlowSrc		    :1;	//Bit 29: Source flow control - 1=wait for request signal
//	uint32_t IncTrgAddr			:1;	//Bit 30: Target increment - 0=no increment, 1=increment
//	uint32_t IncSrcAddr			:1;	//Bit 31: Source increment - 0=no increment, 1=increment
//}DMA_CMD_BITS, *P_DMA_CMD_BITS;

typedef struct DMA_DESCP_TAG{
    uint32_t ddadr;
    uint32_t dsadr;
    uint32_t dtadr;
    uint32_t dcmd;
}*P_DMA_DESCP, DMA_DESCP;

void ssp_dma_read_gd5f(uint32_t offset, uint32_t* buf_tra, uint32_t* buf_rec,uint32_t* desc_tx, uint32_t* desc_rx, uint32_t len);
void ssp_dma_write_gd5f(uint32_t offset, uint32_t *buf, uint32_t* desc_tx, uint32_t len);



#endif
