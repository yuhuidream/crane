#include "bbu.h"
#include "NSSP.h"
#include "ssp_dec.h"
#include "DMA.h"
#include "ssp_lpbk.h"
#include "dma_dec.h"
#include "squ_dma.h"

/*mode, data_len, speed, tx_watermark, rx_watermark*/
static ssp_init_param spi_init_param[] = {
	{SSP_SPI_MODE, SSP_DATA_32BIT, 48000, 1, 7, SPI_MODE_0, 0},
	{SSP_SPI_MODE, SSP_DATA_16BIT, 8000, 0x8, 0x8, SPI_MODE_0, 0},
	{SSP_SPI_MODE, SSP_DATA_18BIT, 16000, 0x8, 0x8, SPI_MODE_0, 0},
	{SSP_SPI_MODE, SSP_DATA_8BIT, 44100, 0x8, 0x8, SPI_MODE_0, 0},
};

int ssp_spi_loopback_test_pio(int port, int ssp_clk)
{
	int max_depth = 0;
	int test_len = 32;
	uint32_t rb_data[32];
	uint32_t pattern_32 = 0x55AA55AA;
	uint32_t pattern;
	uint32_t data_mask;

	max_depth = sizeof(spi_init_param) / sizeof(ssp_init_param);
	for(int i = 0; i < max_depth; i++)
	{
		ssp_init_param *param = &spi_init_param[i];
		if(param->data_length == SSP_DATA_32BIT)
			data_mask = 0xFFFFFFFF;
		else
			data_mask = ((1 << param->data_length) - 1);
		pattern = pattern_32 & data_mask;

		bbu_printf("Loop %d\n\r", i);

		bbu_printf("Step 0\n\r");
		ssp_init_master(port, ssp_clk);
		ssp_port_enable(port, 0);
		ssp_common_config(port, param);
		ssp_set_selfloop(port, 1);
		ssp_port_enable(port, 1);
		bbu_printf("Step 1\n\r");

		if (ssp_fifo_reset(port) && !ssp_get_txfifo_req(port)) {
			bbu_printf("SSP fifo reset faild!\n\r");
			return -1;
		}

		bbu_printf("Step 2\n\r");

		for(int j = 0; j < test_len; j++)
		{
			ssp_write_word(port, pattern);
		}

		bbu_printf("Step 3\n\r");

		mdelay(10);

		for(int j = 0; j < test_len; j++)
		{
			rb_data[i] = ssp_read_word(port) & data_mask;
		}

		bbu_printf("Step 4\n\r");


		for(int j = 0; j < test_len; j++)
		{
			if(rb_data[i] != pattern){
				bbu_printf("Error Data compare mismatch: %d, 0x%x\n\r", rb_data[i]);
				return -1;
			}
		}
	}
	bbu_printf("LoopBack test PASS\n\r");
	return 0;
}

int ssp_spi_loopback_test_dma(int port, int ssp_clk)
{
	uint32_t *p1, *p2, *p3, *p4;
	uint32_t* desc_rx = NULL;
	uint32_t* desc_tx = NULL;
	uint32_t* buf_rx = NULL;
	uint32_t* buf_tx = NULL;
	uint32_t tx_cmd, rx_cmd;
	uint32_t bytes = 0x800;
	uint32_t burst_size = 3; //32 bytes.
	uint32_t cmd_width  = 1; //1 byte.
	int i, j;

	ssp_init_param *param = &spi_init_param[0];
	bbu_printf("\n\rSSP LoopBack DMA mode\n\r");
	buf_rx  = bbu_nc_malloc(bytes+0x10);
	buf_tx  = bbu_nc_malloc(bytes+0x10);
	desc_rx = bbu_nc_malloc(3*16+0x10);
	desc_tx = bbu_nc_malloc(3*16+0x10);
	
	bbu_printf("Step 0\n\r");

	DMA->DCSR[SSP_DMA_RX_CHANNEL] &= ~CSR_RUN;
	DMA->DCSR[SSP_DMA_TX_CHANNEL] &= ~CSR_RUN;

	//align address
	p1 = (uint32_t)buf_rx;
	p2 = (uint32_t)buf_tx;
	p3 = (uint32_t)desc_rx;
	p4 = (uint32_t)desc_tx;
	buf_rx  = (uint32_t*)(((uint32_t)buf_rx & 0xFFFFFFF0) + 0x10);
	buf_tx  = (uint32_t*)(((uint32_t)buf_tx & 0xFFFFFFF0) + 0x10);
	desc_rx = (uint32_t*)(((uint32_t)desc_rx & 0xFFFFFFF0) + 0x10);
	desc_tx = (uint32_t*)(((uint32_t)desc_tx & 0xFFFFFFF0) + 0x10);

	bbu_printf("TX: 0x%x, RX: 0x%x\n\r", (uint32_t)buf_tx, (uint32_t)buf_rx);
	bbu_printf("Desc TX: 0x%x, Desc RX: 0x%x\n\r", (uint32_t)desc_tx, (uint32_t)desc_rx);

	for(i=0; i<bytes/4; i++){
		buf_tx[i] = 0x12345678;
	}

	for(i=0; i<bytes/4; i++){
		buf_rx[i] = 0;
	}


	bbu_printf("Step 1\n\r");
	ssp_init_master(port, ssp_clk);

	ssp_port_enable(port, 0);
	ssp_common_config(port, param);
	ssp_set_dma_rx_enable(port, 1);
	ssp_set_dma_tx_enable(port, 1);
	ssp_set_selfloop(port, 1);
	ssp_trailing_owner_select(port, 1);
	ssp_trailing_set_timeout(port, 0x2000);
	ssp_port_enable(port, 1);

	if (ssp_fifo_reset(port) && !ssp_get_txfifo_req(port)) {
		bbu_printf("SSP fifo reset faild!\n\r");
		return -1;
	}
	bbu_printf("Step 2\n\r");

	ssp_map_channel(port, SSP_DMA_TX_CHANNEL, SSP_DMA_RX_CHANNEL);

	//DMA->DALGN |= (1 << SSP_DMA_RX_CHANNEL);
	//DMA->DALGN |= (1 << SSP_DMA_TX_CHANNEL);

	switch(param->data_length){
		case SSP_DATA_8BIT:
			cmd_width = 1;
			break;
		case SSP_DATA_16BIT:
			cmd_width = 2;
			break;
		case SSP_DATA_32BIT:
			cmd_width = 3;
			break;
		default:
			bbu_printf("Unsupport data length\r\n");
			break;
	}
	
	tx_cmd = bytes|(cmd_width<<CMD_WIDTH_BASE)|(burst_size<<CMD_SIZE_BASE)|(1<<28)|(1<<31);
	rx_cmd = bytes|(cmd_width<<CMD_WIDTH_BASE)|(burst_size<<CMD_SIZE_BASE)|(1<<29)|(1<<30);
	bbu_printf("ssp_get_datar: 0x%x\n\r", ssp_get_datar(port));

	dma_config_descriptor(desc_rx, NULL, ssp_get_datar(port), (uint32_t)buf_rx, rx_cmd, 1);
	dma_config_descriptor(desc_tx, NULL, (uint32_t)buf_tx, ssp_get_datar(port), tx_cmd, 1);

	dma_load_descriptor((P_DMA_DESCP)desc_rx, SSP_DMA_RX_CHANNEL);
	dma_load_descriptor((P_DMA_DESCP)desc_tx, SSP_DMA_TX_CHANNEL);

	DMA->DCSR[SSP_DMA_TX_CHANNEL] |= CSR_RUN;
	DMA->DCSR[SSP_DMA_RX_CHANNEL] |= CSR_RUN;
	bbu_printf("Step 3\n\r");

	while(!(DMA->DCSR[SSP_DMA_TX_CHANNEL] & CSR_STOPINTR));
	while(!(DMA->DCSR[SSP_DMA_RX_CHANNEL] & CSR_STOPINTR));
	bbu_printf("Step 4\n\r");

	for(int j=0; j<bytes/4; j++){
		//bbu_printf("data_temp = %02X\r\ndata_tra = %02X\r\n", data_temp[j], data_tra[j]);
		if(buf_rx[j] != buf_tx[j]){
			bbu_printf("FAIL\r\n");
			goto done;
		}
	}
	bbu_printf("PASS\n\r");

done:
	ssp_flush(port);
	ssp_unmap_channel(port);
	
	bbu_mfree((uint32_t*)p1);
	bbu_mfree((uint32_t*)p2);
	bbu_mfree((uint32_t*)p3);
	bbu_mfree((uint32_t*)p4);
}

int ssp1_loopback_test_dma(int ssp_clk)
{
	SQU_DMA_DESC* desc_rx = NULL;
	SQU_DMA_DESC* desc_tx = NULL;
	uint32_t* buf_rx = NULL;
	uint32_t* buf_tx = NULL;
	uint32_t tx_cmd, rx_cmd;
	uint32_t bytes = 0x800;
	int i, j;
	int port =1;

	ssp_init_param *param = &spi_init_param[0];
	bbu_printf("\n\rSSP1 LoopBack SQU DMA mode\n\r");
	
	bbu_printf("Step 0\n\r");
	//align address
	buf_rx  = (uint32_t*)0xD1001000;
	buf_tx  = (uint32_t*)0xD1002000;
	desc_rx = (SQU_DMA_DESC*)0xD1000100;
	desc_tx = (SQU_DMA_DESC*)0xD1000200;

	bbu_printf("TX: 0x%x, RX: 0x%x\n\r", (uint32_t)buf_tx, (uint32_t)buf_rx);
	bbu_printf("Desc TX: 0x%x, Desc RX: 0x%x\n\r", (uint32_t)desc_tx, (uint32_t)desc_rx);

	for(i=0; i<bytes/4; i++){
		buf_tx[i] = 0x12345678;
	}

	for(i=0; i<bytes/4; i++){
		buf_rx[i] = 0;
	}


	bbu_printf("Step 1\n\r");
	ssp_init_master(port, ssp_clk);

	ssp_port_enable(port, 0);
	ssp_common_config(port, param);
	ssp_set_dma_rx_enable(port, 1);
	ssp_set_dma_tx_enable(port, 1);
	ssp_set_selfloop(port, 1);
	ssp_trailing_owner_select(port, 1);
	ssp_trailing_set_timeout(port, 0x2000);
	ssp_port_enable(port, 1);

	if (ssp_fifo_reset(port) && !ssp_get_txfifo_req(port)) {
		bbu_printf("SSP fifo reset faild!\n\r");
		return -1;
	}
	bbu_printf("Step 2\n\r");

	//ssp_map_channel(port, SSP_DMA_TX_CHANNEL, SSP_DMA_RX_CHANNEL);

	//DMA->DALGN |= (1 << SSP_DMA_RX_CHANNEL);
	//DMA->DALGN |= (1 << SSP_DMA_TX_CHANNEL);

	squ_dma_xfer_stop(0);
	squ_dma_xfer_stop(1);
	squ_dma_common_config(0, 0, 2, 7); //TX, burst_size = 32bytes
	squ_dma_common_config(1, 2, 0, 7); //RX, burst_size = 32bytes
	squ_dma_config_ssp(0, 1);
	squ_dma_config_ssp(1, 1);

#if 1
	squ_dma_chain_mode(0, 1);
	squ_dma_chain_mode(1, 1);

	squ_dma_config_descriptor(desc_tx, 0, (uint32_t)buf_tx, ssp_get_datar(1), bytes, 1);
	squ_dma_config_descriptor(desc_rx, 0, ssp_get_datar(1), (uint32_t)buf_rx, bytes, 1);

	squ_dma_load_descriptor(0, desc_tx);
	squ_dma_load_descriptor(1, desc_rx);
#else
	squ_dma_chain_mode(1, 0);
	squ_dma_chain_mode(1, 0);
	
	squ_dma_non_fecth_mode(0, (uint32_t)buf_tx, ssp_get_datar(1), bytes);
	squ_dma_non_fecth_mode(1, ssp_get_datar(1), (uint32_t)buf_rx, bytes);
#endif
	squ_dma_comp_int_enable(0, 1);
	squ_dma_comp_int_enable(1, 1);
	squ_dma_reg_dump();

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
			goto done;
		}
	}
	bbu_printf("PASS\n\r");

done:
	ssp_flush(port);
}


void bbu_ssp_test_new(uint8_t argc, uint8_t **argv){

	int port, clk, test;
	if(argc < 4)
	{
		bbu_printf("Need more arg\n\r");
		return;
	}

	test = conv_dec(argv[1]);
	port = conv_dec(argv[2]);
	clk = conv_dec(argv[3]);
	switch(test){
		case 0:
			ssp_spi_loopback_test_pio(port, clk);
			break;
		case 1:
			if(port != 1)
				ssp_spi_loopback_test_dma(port, clk);
			else
				ssp1_loopback_test_dma(clk);
			break;
	}

}