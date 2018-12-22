#include "ssp_gd5f_dma.h"
#include "dma_dec.h"
Q_DEFINE_THIS_MODULE("ssp_gd5f_dma.c")

//#define SSP_TX_DELAY		400

static void ssp_map_channel(){
	DMA->DRCMR1[60] = RCMR_MAPVLD | SSP_GD5F_DMA_RX_CHANNEL;//map rx dma channel.
	DMA->DRCMR1[61] = RCMR_MAPVLD | SSP_GD5F_DMA_TX_CHANNEL;//map tx dma channel.
	
	return;
}

static void ssp_dismap_channel(){
	DMA->DRCMR1[60] &= ~RCMR_MAPVLD;//unmap rx dma channel.
	DMA->DRCMR1[61] &= ~RCMR_MAPVLD;//unmap tx dma channel.
	
	return;
}

extern uint32_t ssp_clk_52m;
static void ssp_dma_read_page_gd5f(uint32_t offset,
                                   uint32_t* buf_tra,
                                   uint32_t* buf_rec,
                                   uint32_t* desc_tx,
                                   uint32_t* desc_rx){
	//page read to cache.
	uint8_t cmd1[4];
	cmd1[0] = SSP_PAGE_READ_TCACHE_COMMAND_GD5F;
	cmd1[1] = 0;
	cmd1[2] = (offset>>8)&0xFF;
	cmd1[3] = offset&0xFF;
	ssp_xfer(8*4, NULL, cmd1, SSP_XFER_BE);

	int stat_chk = ssp_wait_ready_gd5f();
	if(!stat_chk){
		bbu_printf("SSP: wait status error!\r\n");
		goto error;
	}

	//read data from cache.
	uint8_t cmd2[4];
	cmd2[0] = SSP_PAGE_READ_FCACHE_COMMAND_GD5F;
	cmd2[1] = 0x40;
	cmd2[2] = 0;
	cmd2[3] = 0xFF;
	ssp_xfer(8*4, NULL, cmd2, SSP_XFER_BEGIN);

	//transfer 32 bit data in DMA mode.
	SSP2->SSCR0 &= ~SSP_SSCR0_SSE;
	SSP2->SSCR1 = 0xC0700880;
	SSP2->SSTO = SSP_RX_TIMEOUT;
	if(ssp_clk_52m){
		SSP2->SSCR0 = 0x08d0008f;
	}else{
		SSP2->SSCR0 = 0x00d0008f;
	}
	//mdelay(10);
	//bbu_printf("SSP2->SSCR0 is %08X\r\n", SSP2->SSCR0);
	
	*(uint32_t *)0xd4282864 = 0x1 | (0x1<<3);  //Enable DMA clock
	ssp_map_channel();

	uint32_t tx2_cmd = 0;
	uint32_t rx2_cmd = 0;

	DMA->DALGN |= (1 << SSP_GD5F_DMA_RX_CHANNEL);
	DMA->DALGN |= (1 << SSP_GD5F_DMA_TX_CHANNEL);
	
	tx2_cmd = 0x800|(3<<CMD_WIDTH_BASE)|(1<<CMD_SIZE_BASE)|(1<<28)|(1<<31);
	rx2_cmd = 0x800|(3<<CMD_WIDTH_BASE)|(1<<CMD_SIZE_BASE)|(1<<29)|(1<<30);
	
	dma_config_descriptor(desc_rx, NULL, (uint32_t)(&(SSP2->SSDDR)), (uint32_t)buf_rec, rx2_cmd, 1);
	dma_config_descriptor(desc_tx, NULL, (uint32_t)buf_tra, (uint32_t)(&(SSP2->SSDDR)), tx2_cmd, 1);

	dma_load_descriptor((P_DMA_DESCP)desc_rx, SSP_GD5F_DMA_RX_CHANNEL);
	dma_load_descriptor((P_DMA_DESCP)desc_tx, SSP_GD5F_DMA_TX_CHANNEL);

	DMA->DCSR[SSP_GD5F_DMA_TX_CHANNEL] |= CSR_RUN;
	DMA->DCSR[SSP_GD5F_DMA_RX_CHANNEL] |= CSR_RUN;

	while(!(DMA->DCSR[SSP_GD5F_DMA_TX_CHANNEL] & CSR_STOPINTR));
	while(!(DMA->DCSR[SSP_GD5F_DMA_RX_CHANNEL] & CSR_STOPINTR));
//	udelay(SSP_TX_DELAY);
	
	ssp_xfer(0, NULL, NULL, SSP_XFER_END);
	ssp_dismap_channel();

	return;

	error:
		bbu_printf("SSP: DMA read data error\r\n");

		return;
}

static void ssp_dma_write_page_gd5f(uint32_t offset, uint32_t* buf, uint32_t* desc_tx){
	//send the command to write data to cache.
	uint8_t cmd1[3];
	cmd1[0] = SSP_PAGE_WRITE_TCACHE_CONMMAND_GD5F;
	cmd1[1] = 0;
	cmd1[2] = 0;
	ssp_xfer(8*3, NULL, cmd1, SSP_XFER_BEGIN);
	
	SSP2->SSCR0 &= ~SSP_SSCR0_SSE;
	SSP2->SSCR1 = 0xC0200080;
	SSP2->SSTO = SSP_RX_TIMEOUT;
	if(ssp_clk_52m){
		SSP2->SSCR0 = 0x08d0008f;
	}else{
		SSP2->SSCR0 = 0x00d0008f;
	}
	//mdelay(10);
	//bbu_printf("SSP2->SSCR0 is %08X\r\n", SSP2->SSCR0);
	*(uint32_t *)0xd4282864 = 0x1 | (0x1<<3);  //Enable DMA clock

	ssp_map_channel();
	DMA->DALGN |= (1 << SSP_GD5F_DMA_TX_CHANNEL);

	uint32_t tx1_cmd = 0;
	tx1_cmd = 0x800|(3<<CMD_WIDTH_BASE)|(1<<CMD_SIZE_BASE)|(1<<28)|(1<<31);
	dma_config_descriptor(desc_tx, NULL, (uint32_t)buf, (uint32_t)(&(SSP2->SSDDR)), tx1_cmd, 1);
	dma_load_descriptor((P_DMA_DESCP)desc_tx, SSP_GD5F_DMA_TX_CHANNEL);

	DMA->DCSR[SSP_GD5F_DMA_TX_CHANNEL] |= CSR_RUN;
	
	while(!(DMA->DCSR[SSP_GD5F_DMA_TX_CHANNEL] & CSR_STOPINTR));
	while(((SSP2->SSSR) & SSP_SSSR_TFL_MSK));
//	udelay(SSP_TX_DELAY);

	ssp_xfer(0, NULL, NULL, SSP_XFER_END);
	ssp_dismap_channel();
	ssp_write_enable_gd5f();

	//write data to flash.
	uint8_t cmd2[4];
	cmd2[0] = SSP_PAGE_WRITE_TFLASH_CONMMAND_GD5F;
	cmd2[1] = 0;
	cmd2[2] = (offset>>8)&0xFF;
	cmd2[3] = offset&0xFF;
	ssp_xfer(8*4, NULL, cmd2, SSP_XFER_BE);

	int stat_chk = ssp_wait_ready_gd5f();
	if(!stat_chk){
		bbu_printf("SSP: status error!\r\n");
	}

	return;
}

static void ssp_dma_read_gd5f_(uint32_t offset,
                               uint32_t* buf_tra,
                               uint32_t* buf_rec,
                               uint32_t* desc_tx,
                               uint32_t* desc_rx,
                               uint32_t len){
	
	uint32_t pages = 0;
	pages = len/SSP_BYTES_PER_PAGE_GD5F;
	
	uint32_t i = 0;
	for(i=0; i<pages; i++){
		ssp_dma_read_page_gd5f(offset, buf_tra, buf_rec, desc_tx, desc_rx);
		//buf_tra = buf_tra+SSP_BYTES_PER_PAGE_GD5F;//point the next page memory.
		buf_rec = buf_rec+SSP_BYTES_PER_PAGE_GD5F/4;
		offset++;
	}

	return;
}

static void ssp_dma_write_gd5f_(uint32_t offset, uint32_t *buf, uint32_t* desc_tx, uint32_t len){
	uint32_t pages = 0;
	pages = len/SSP_BYTES_PER_PAGE_GD5F;
	
	uint32_t i = 0;
	for(i=0; i<pages; i++){
		ssp_dma_write_page_gd5f(offset, buf, desc_tx);
		buf = buf+SSP_BYTES_PER_PAGE_GD5F/4;//point the next page memory.
		offset++;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void ssp_speed_test_dma(uint32_t mode, uint32_t test_count){
	uint32_t i = 0;
	uint32_t interval = 0;
	uint32_t t = 0;

	uint32_t* rec_data_array = NULL;
	uint32_t* tra_data_array = NULL;
	uint32_t* desc_rx = NULL;
	uint32_t* desc_tx = NULL;

	rec_data_array = bbu_nc_malloc(2052);
	tra_data_array = bbu_nc_malloc(2052);
	desc_rx = bbu_nc_malloc(3*4);
	desc_tx = bbu_nc_malloc(3*4);

	//align address
	uint32_t p1 = (uint32_t)rec_data_array;
	uint32_t p2 = (uint32_t)tra_data_array;
	uint32_t p3 = (uint32_t)desc_rx;
	uint32_t p4 = (uint32_t)desc_tx;
	rec_data_array = (uint32_t*)(((uint32_t)rec_data_array & 0xFFFFFFF0) + 0x10);
	tra_data_array = (uint32_t*)(((uint32_t)tra_data_array & 0xFFFFFFF0) + 0x10);
	desc_rx = (uint32_t*)(((uint32_t)desc_rx & 0xFFFFFFF0) + 0x10);
	desc_tx = (uint32_t*)(((uint32_t)desc_tx & 0xFFFFFFF0) + 0x10);
	memset(tra_data_array, 0, SSP_BYTES_PER_PAGE_GD5F);
	
	switch(mode){
		case SSP_TEST_MODE_W:{
			interval = timer_get_count(0);
			for(i=0; i<test_count; i++){
				ssp_dma_write_gd5f_(0, tra_data_array, desc_tx, SSP_STRESS_TEST_BYTES);
			}
			t = timer_get_count(0) - interval;
			float speed_w = SSP_BYTES_PER_BLOCK_GD5F*test_count*3250000/t;
			bbu_printf("SSP: write speed is %.2fKbyte/s\r\n", speed_w);
			break;
		}
		case SSP_TEST_MODE_R:{
			interval = timer_get_count(0);
			for(i=0; i<test_count; i++){
				ssp_dma_read_gd5f_(0, tra_data_array, rec_data_array, desc_tx, desc_rx, SSP_STRESS_TEST_BYTES);
			}
			t = timer_get_count(0) - interval;
			float speed_r = SSP_BYTES_PER_BLOCK_GD5F*test_count*3250000/t;
			bbu_printf("SSP: read speed is %.2fKbyte/s\r\n", speed_r);
			break;
		}
		default:
			break;
	}

	bbu_mfree((uint32_t*)p1);
	bbu_mfree((uint32_t*)p2);
	bbu_mfree((uint32_t*)p3);
	bbu_mfree((uint32_t*)p4);
	return;
}

void ssp_stress_test_dma_gd5f(uint32_t count){
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;

	uint32_t* rec_data_array = NULL;
	uint32_t* tra_data_array = NULL;
	uint32_t* temp_data = NULL;
	uint32_t* desc_rx = NULL;
	uint32_t* desc_tx = NULL;

	rec_data_array = bbu_nc_malloc(SSP_STRESS_TEST_BYTES+4);
	tra_data_array = bbu_nc_malloc(SSP_BYTES_PER_PAGE_GD5F+4);
	temp_data = bbu_nc_malloc(SSP_STRESS_TEST_BYTES+4);
	desc_rx = bbu_nc_malloc(3*4);
	desc_tx = bbu_nc_malloc(3*4);

	//align address
	uint32_t p1 = (uint32_t)rec_data_array;
	uint32_t p2 = (uint32_t)tra_data_array;
	uint32_t p3 = (uint32_t)desc_rx;
	uint32_t p4 = (uint32_t)desc_tx;
	uint32_t p5 = (uint32_t)temp_data;

	rec_data_array = (uint32_t*)(((uint32_t)rec_data_array & 0xFFFFFFF0) + 0x10);
	tra_data_array = (uint32_t*)(((uint32_t)tra_data_array & 0xFFFFFFF0) + 0x10);
	temp_data = (uint32_t*)(((uint32_t)temp_data & 0xFFFFFFF0) + 0x10);
	desc_rx = (uint32_t*)(((uint32_t)desc_rx & 0xFFFFFFF0) + 0x10);
	desc_tx = (uint32_t*)(((uint32_t)desc_tx & 0xFFFFFFF0) + 0x10);

	memset(tra_data_array, 0, SSP_BYTES_PER_PAGE_GD5F);
	for(i=0; i<SSP_STRESS_TEST_BYTES/4; i++){
		temp_data[i] = i;
	}

	for(k=0; k<count; k++){
		ssp_erase_gd5f(0, 1);
		ssp_dma_write_gd5f_(0, temp_data, desc_tx, SSP_STRESS_TEST_BYTES);
		memset(rec_data_array, 0, SSP_STRESS_TEST_BYTES);
		ssp_dma_read_gd5f_(0, tra_data_array, rec_data_array, desc_tx, desc_rx, SSP_STRESS_TEST_BYTES);

		for(j=0; j<SSP_STRESS_TEST_BYTES/4; j++){
			//bbu_printf("data_temp = %02X\r\ndata_tra = %02X\r\n", data_temp[j], data_tra[j]);
			if(temp_data[j] != rec_data_array[j]){
				bbu_printf("SSP: stress test error\r\n");
				bbu_mfree((uint32_t*)p1);
				bbu_mfree((uint32_t*)p2);
				bbu_mfree((uint32_t*)p3);
				bbu_mfree((uint32_t*)p4);
				bbu_mfree((uint32_t*)p5);
				Q_ASSERT(0);
			}
		}
		bbu_printf(".");
	}
	
	bbu_printf("\r\nSSP: stress test success\r\n");
	bbu_mfree((uint32_t*)p1);
	bbu_mfree((uint32_t*)p2);
	bbu_mfree((uint32_t*)p3);
	bbu_mfree((uint32_t*)p4);
	bbu_mfree((uint32_t*)p5);

	return;
}

void ssp_write_dma_gd5f(uint32_t offset, uint32_t len, uint32_t* buf){
	uint32_t* desc_tx = NULL;
	desc_tx = bbu_nc_malloc(3*4);
	//align address
	uint32_t p1 = (uint32_t)desc_tx;
	desc_tx = (uint32_t*)(((uint32_t)desc_tx & 0xFFFFFFF0) + 0x10);

	ssp_dma_write_gd5f_(offset, buf, desc_tx, len);
	
	bbu_mfree((uint32_t*)p1);

	return;
}

void ssp_read_dma_gd5f(uint32_t offset, uint32_t len, uint32_t* buf){
	uint32_t* desc_rx = NULL;
	uint32_t* desc_tx = NULL;
	uint32_t* tra_data_array = NULL;

	tra_data_array = bbu_nc_malloc(SSP_BYTES_PER_PAGE_GD5F+4);
	desc_rx = bbu_nc_malloc(3*4);
	desc_tx = bbu_nc_malloc(3*4);
	//align address
	uint32_t p1 = (uint32_t)tra_data_array;
	uint32_t p2 = (uint32_t)desc_rx;
	uint32_t p3 = (uint32_t)desc_tx;
	tra_data_array = (uint32_t*)(((uint32_t)tra_data_array & 0xFFFFFFF0) + 0x10);
	desc_rx = (uint32_t*)(((uint32_t)desc_rx & 0xFFFFFFF0) + 0x10);
	desc_tx = (uint32_t*)(((uint32_t)desc_tx & 0xFFFFFFF0) + 0x10);

	memset(tra_data_array, 0, SSP_BYTES_PER_PAGE_GD5F);

	ssp_dma_read_gd5f_(offset, tra_data_array, buf, desc_tx, desc_rx, len);

	bbu_mfree((uint32_t*)p1);
	bbu_mfree((uint32_t*)p2);
	bbu_mfree((uint32_t*)p3);

	return;
}









