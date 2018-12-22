#include "bbu.h"
#include "SQU.h"
#include "squ_dma.h"

void squ_dma_config_descriptor(SQU_DMA_DESC* desc, SQU_DMA_DESC* next, uint32_t src, uint32_t dest, int size, int stop)
{
	desc->byte_count = size;
	desc->next_desc = next;
	desc->src_addr = src;
	desc->dest_addr = dest;
	if(stop){
		desc->next_desc = 0;
	}
}

void squ_dma_load_descriptor(int channel, SQU_DMA_DESC *desc)
{
	SQU->CHAN_NEXT_DESC_PTR[channel] = (uint32_t)desc;
	SQU->CHAN_CTRL[channel] |= CTRL_FETCHND;
}


void squ_dma_config_alignment(int channel, int enable)
{
	if(enable)
		SQU->CHAN_CTRL[channel] |= CTRL_SDA;
	else
		SQU->CHAN_CTRL[channel] &= ~CTRL_SDA;
}

void squ_dma_config_ssp(int channel, int enable)
{
	if(enable)
		SQU->CHAN_CTRL[channel] |= CTRL_SSPMODE;
	else
		SQU->CHAN_CTRL[channel] &= ~CTRL_SSPMODE;
}

void squ_dma_xfer_start(int channel)
{
	SQU->CHAN_CTRL[channel] |= CTRL_CHANEN;
}

void squ_dma_xfer_stop(int channel)
{
	SQU->CHAN_CTRL[channel] &= ~CTRL_CHANEN;
}

void squ_dma_chain_mode(int channel, int enable)
{
	if(enable)
		SQU->CHAN_CTRL[channel] &= ~CTRL_CHAINMOD;
	else
		SQU->CHAN_CTRL[channel] |= CTRL_CHAINMOD;
}

void squ_dma_non_fecth_mode(int channel, uint32_t src, uint32_t dest, int size)
{
	SQU->CHAN_SRC_ADDR[channel] = src;
	SQU->CHAN_DEST_ADDR[channel] = dest;
	SQU->CHAN_BYTE_CNT[channel] = size;
	squ_dma_chain_mode(channel, 0);
}

int squ_dma_xfer_comp(int channel)
{
	return (SQU->CHAN_INT_STATUS[channel] & BIT_0);
}

void squ_dma_comp_int_enable(int channel, int enable)
{
	if(enable)
		SQU->CHAN_INT_MASK[channel] |= BIT_0;
	else
		SQU->CHAN_INT_MASK[channel] &= ~BIT_0;
}


/*
burst_size:
0x5 = 1 byte
0x6 = 2 bytes
0x0 = 4 bytes
0x1 = 8 bytes
0x3 = 16 bytes
0x7 = 32 bytes

src_inc/dest_inc:
0x0 = Increment source/destination address
0x1 = Decrement source/destination address
0x2 = Hold in the same value
0x3 = Reserved
 */
void squ_dma_common_config(int channel, int src_inc, int dest_inc, int burst_size)
{
	int val;
	val = SQU->CHAN_CTRL[channel];

	val &= ~CTRL_SRCDIR_MASK;
	val &= ~CTRL_DESTDIR_MASK;
	val &= ~CTRL_BUSRT_MASK;
	val |= (src_inc<<CTRL_SRCDIR_BASE);
	val |= (dest_inc<<CTRL_DESTDIR_BASE);
	val |= (burst_size<<CTRL_BURST_BASE);

	SQU->CHAN_CTRL[channel] = val;
}

void squ_dma_trans_mode(int channel, int enable)
{
	if(enable)
		SQU->CHAN_CTRL[channel] |= CTRL_TRANSMOD;
	else
		SQU->CHAN_CTRL[channel] &= ~CTRL_TRANSMOD;
}

int squ_dma_test(int mode)
{
	SQU_DMA_DESC* desc_rx = NULL;
	SQU_DMA_DESC* desc_tx = NULL;
	uint32_t *buf_rx = NULL, *buf_rx_unaligned;
	uint32_t *buf_tx = NULL, *buf_tx_unaligned;
	uint32_t *buf_mx = NULL, *buf_mx_unaligned;
	uint32_t bytes = 0x800;
	int i, j;

	bbu_printf("\n\rSSP1 LoopBack SQU DMA mode\n\r");
	
	bbu_printf("Step 0\n\r");
	//align address
	#ifdef CONFIG_SQU
	buf_rx_unaligned  = (uint32_t*)bbu_nc_malloc(bytes+32);
	buf_tx_unaligned  = (uint32_t*)bbu_nc_malloc(bytes+32);
	buf_mx_unaligned  = (uint32_t*)bbu_nc_malloc(bytes+32);
	buf_rx  = ADDR_ALIGN(((uint32_t)buf_rx_unaligned), 32);
	buf_tx  = ADDR_ALIGN(((uint32_t)buf_tx_unaligned), 32);
	buf_mx  = ADDR_ALIGN(((uint32_t)buf_mx_unaligned), 32);
	desc_rx = (SQU_DMA_DESC*)bbu_nc_malloc(sizeof(SQU_DMA_DESC));
	desc_tx = (SQU_DMA_DESC*)bbu_nc_malloc(sizeof(SQU_DMA_DESC));
	#else
	buf_rx  = (uint32_t*)0xD1001000;
	buf_tx  = (uint32_t*)0xD1002000;
	buf_mx  = (uint32_t*)0xD1003000;
	desc_rx = (SQU_DMA_DESC*)0xD1000100;
	desc_tx = (SQU_DMA_DESC*)0xD1000200;
	#endif
	bbu_printf("TX: 0x%x, RX: 0x%x\n\r", (uint32_t)buf_tx, (uint32_t)buf_rx);
	bbu_printf("Desc TX: 0x%x, Desc RX: 0x%x\n\r", (uint32_t)desc_tx, (uint32_t)desc_rx);

	for(i=0; i<bytes/4; i++){
		buf_tx[i] = 0x12345678;
		buf_mx[i] = 0;
		buf_rx[i] = 0;
	}

	bbu_printf("Step 1\n\r");

	squ_dma_xfer_stop(0);
	squ_dma_xfer_stop(1);
	squ_dma_common_config(0, 0, 0, 7); //TX, burst_size = 32bytes
	squ_dma_common_config(1, 0, 0, 7); //RX, burst_size = 32bytes
	squ_dma_config_ssp(0, 0);
	squ_dma_config_ssp(1, 0);
	squ_dma_trans_mode(0, 1);
	squ_dma_trans_mode(1, 1);

	if(mode){
		bbu_printf("Fetch Mode\n\r");
		squ_dma_chain_mode(0, 1);
		squ_dma_chain_mode(1, 1);
	
		squ_dma_config_descriptor(desc_tx, 0, (uint32_t)buf_tx, (uint32_t)buf_mx, bytes, 1);
		squ_dma_config_descriptor(desc_rx, 0, (uint32_t)buf_mx, (uint32_t)buf_rx, bytes, 1);
	
		squ_dma_load_descriptor(0, desc_tx);
		squ_dma_load_descriptor(1, desc_rx);
	}else{
		bbu_printf("Non-Fetch Mode\n\r");
		squ_dma_chain_mode(1, 0);
		squ_dma_chain_mode(1, 0);
	
		squ_dma_non_fecth_mode(0, (uint32_t)buf_tx, (uint32_t)buf_mx, bytes);
		squ_dma_non_fecth_mode(1, (uint32_t)buf_mx, (uint32_t)buf_rx, bytes);
	}

	squ_dma_comp_int_enable(0, 1);
	squ_dma_comp_int_enable(1, 1);

	bbu_printf("Step 3\n\r");
	squ_dma_xfer_start(0);
	squ_dma_xfer_start(1);
	squ_dma_reg_dump();

	while(1){
		if(squ_dma_xfer_comp(0) && squ_dma_xfer_comp(1))
			break;
		udelay(1);
	}

	bbu_printf("Step 4\n\r");

	for(int j=0; j<bytes/4; j++){
		//bbu_printf("data_temp = %02X\r\ndata_tra = %02X\r\n", data_temp[j], data_tra[j]);
		if(buf_rx[j] != buf_tx[j]){
			bbu_printf("FAIL\r\n");
			return;
		}
	}
	bbu_mfree(buf_tx_unaligned);
	bbu_mfree(buf_rx_unaligned);
	bbu_mfree(buf_mx_unaligned);
	bbu_mfree(desc_tx);
	bbu_mfree(desc_rx);
	bbu_printf("PASS\n\r");
	return;
}

void squ_dma_reg_dump(void)
{
	bbu_printf("SQU registers:\n\r");
	for(int channel = 0; channel < 2; channel ++){
		bbu_printf("CHAN_CTRL %d: 0x%x 0x%x\n\r", channel, &(SQU->CHAN_CTRL[channel]), SQU->CHAN_CTRL[channel]);
		bbu_printf("CHAN_BYTE_CNT %d: 0x%x 0x%x\n\r", channel, &(SQU->CHAN_BYTE_CNT[channel]), SQU->CHAN_BYTE_CNT[channel]);
		bbu_printf("CHAN_SRC_ADDR %d: 0x%x 0x%x\n\r", channel, &(SQU->CHAN_SRC_ADDR[channel]), SQU->CHAN_SRC_ADDR[channel]);
		bbu_printf("CHAN_DEST_ADDR %d: 0x%x 0x%x\n\r", channel, &(SQU->CHAN_DEST_ADDR[channel]), SQU->CHAN_DEST_ADDR[channel]);
		bbu_printf("CHAN_NEXT_DESC_PTR %d: 0x%x 0x%x\n\r", channel, &(SQU->CHAN_NEXT_DESC_PTR[channel]), SQU->CHAN_NEXT_DESC_PTR[channel]);
		bbu_printf("CHAN_CURR_DESC_PTR %d: 0x%x 0x%x\n\r", channel, &(SQU->CHAN_CURR_DESC_PTR[channel]), SQU->CHAN_CURR_DESC_PTR[channel]);
		bbu_printf("CHAN_INT_MASK %d: 0x%x 0x%x\n\r", channel, &(SQU->CHAN_INT_MASK[channel]), SQU->CHAN_INT_MASK[channel]);
		bbu_printf("CHAN_INT_STATUS %d: 0x%x 0x%x\n\r", channel, &(SQU->CHAN_INT_STATUS[channel]), SQU->CHAN_INT_STATUS[channel]);
	}
}