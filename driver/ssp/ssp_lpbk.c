#include "ssp_lpbk.h"
#include "dma_dec.h"
#include "interrupt.h"
#include "ssp_dec.h"

static uint32_t ssp_clk = SSP_CLK_26M;
static uint32_t data_length = SSP_DATA_LEN_32;
static uint32_t ssp_index = 2;
static uint32_t bytes = 0x800;
uint32_t p1 = 0;
uint32_t p2 = 0;
uint32_t p3 = 0;
uint32_t p4 = 0;
uint32_t* buf_rx  = NULL;
uint32_t* buf_tx  = NULL;
uint32_t mem_inited = 0;

uint32_t ssp0_rx_channel = 9;
uint32_t ssp0_tx_channel = 10;
uint32_t ssp2_rx_channel = 11;
uint32_t ssp2_tx_channel = 12;

static const char *clk_str[] = {
	"6.5M", "13M", "26M", "52M"
};
static const char *data_length_str[] = {
	"8 bits", "16 bits", "18 bits", "32 bits"
};

static SSP_UNIT ssp_table[] = {
	{SSP0, IRQ_MMP_SSP0}, 
	{SSP1, 0}, 
	{SSP2, IRQ_MMP_SSP2}
};

static void ssp_flush(){
	uint32_t temp = 0;

	do{
		while(SSP2->SSSR & SSP_SSSR_RNE){
			temp = SSP2->SSDDR;
		}
	}while(SSP2->SSSR & SSP_SSSR_BSY);
	SSP2->SSSR |= SSP_SSSR_ROR;//clear the rx fifo irq.
	(void)temp;

	return;
}

static void ssp_map_channel(){
	if (cpu_is_pxa1826_z3()|cpu_is_pxa1826_a0())
	{
		ssp0_rx_channel = 9;
		ssp0_tx_channel = 10;
		ssp2_rx_channel = 11;
		ssp2_tx_channel = 12;
	}
	if (cpu_is_pxa1826_z1z2())
	{
		ssp0_rx_channel = 52;
		ssp0_tx_channel = 53;
		ssp2_rx_channel = 60;
		ssp2_tx_channel = 61;
	}

	switch(ssp_index){
	case 0:
		DMA->DRCMR1[ssp0_rx_channel] = RCMR_MAPVLD | SSP_DMA_RX_CHANNEL;//map rx dma channel.
		DMA->DRCMR1[ssp0_tx_channel] = RCMR_MAPVLD | SSP_DMA_TX_CHANNEL;//map tx dma channel.
		break;
	case 2:
		DMA->DRCMR1[ssp2_rx_channel] = RCMR_MAPVLD | SSP_DMA_RX_CHANNEL;//map rx dma channel.
		DMA->DRCMR1[ssp2_tx_channel] = RCMR_MAPVLD | SSP_DMA_TX_CHANNEL;//map tx dma channel.
		break;
	default:
		break;
	}
	
	return;
}

static void ssp_dismap_channel(){
	switch(ssp_index){
	case 0:
		DMA->DRCMR1[52] &= ~RCMR_MAPVLD;//unmap rx dma channel.
		DMA->DRCMR1[53] &= ~RCMR_MAPVLD;//unmap tx dma channel.
		break;
	case 2:
		DMA->DRCMR1[60] &= ~RCMR_MAPVLD;//unmap rx dma channel.
		DMA->DRCMR1[61] &= ~RCMR_MAPVLD;//unmap tx dma channel.
		break;
	default:
		break;
	}
	
	return;
}

static void ssp_init(){
	bbu_printf("SSP Init. Source Clock: %s  Unit: %d\r\n", clk_str[ssp_clk], ssp_index);
	//reset the APBclk.
	switch(ssp_index){
	case 0:
		APBClock->SSP0_CLK_RST = SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|SSP0_CLK_RST_RST|(ssp_clk<<4);
		//bbu_printf("APBClock->SSP0_CLK_RST = 0x%08X\r\n", SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|SSP0_CLK_RST_RST|(ssp_clk<<4));
		udelay(1);
		APBClock->SSP0_CLK_RST = SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|(ssp_clk<<4);
		//bbu_printf("APBClock->SSP0_CLK_RST = 0x%08X\r\n", SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|(ssp_clk<<4));
		break;
	case 1:
		APBClock->SSP1_CLK_RST = SSP1_CLK_RST_APBCLK|SSP1_CLK_RST_FNCLK|SSP1_CLK_RST_RST|(ssp_clk<<4);
		udelay(1);
		APBClock->SSP1_CLK_RST = SSP1_CLK_RST_APBCLK|SSP1_CLK_RST_FNCLK|(ssp_clk<<4);
		break;
	case 2:
		APBClock->SSP2_CLK_RST = SSP2_CLK_RST_APBCLK|SSP2_CLK_RST_FNCLK|SSP2_CLK_RST_RST|(ssp_clk<<4);
		//bbu_printf("APBClock->SSP2_CLK_RST = 0x%08X\r\n", SSP2_CLK_RST_APBCLK|SSP2_CLK_RST_FNCLK|SSP2_CLK_RST_RST|(ssp_clk<<4));
		udelay(1);
		APBClock->SSP2_CLK_RST = SSP2_CLK_RST_APBCLK|SSP2_CLK_RST_FNCLK|(ssp_clk<<4);
		//bbu_printf("APBClock->SSP2_CLK_RST = 0x%08X\r\n", SSP2_CLK_RST_APBCLK|SSP2_CLK_RST_FNCLK|(ssp_clk<<4));
	default:
		break;
	}
	
	SSP_UNIT ssp = ssp_table[ssp_index];

	ssp.base->SSCR0 = 0;
	//bbu_printf("ssp.base->SSCR0 = 0\r\n");
	udelay(5);
	switch(data_length){
	case SSP_DATA_LEN_8:
		ssp.base->SSCR0 = 0x7;
		//bbu_printf("ssp.base->SSCR0 = 0x7\r\n");
		break;
	case SSP_DATA_LEN_16:
		ssp.base->SSCR0 = 0xF;
		//bbu_printf("ssp.base->SSCR0 = 0xF\r\n");
		break;
	case SSP_DATA_LEN_18:
		ssp.base->SSCR0 = 0x1|SSP_SSCR0_EDSS;
		//bbu_printf("ssp.base->SSCR0 = 0x%08X\r\n", 0x1|SSP_SSCR0_EDSS);
		break;
	case SSP_DATA_LEN_32:
		ssp.base->SSCR0 = 0xF|SSP_SSCR0_EDSS;
		//bbu_printf("ssp.base->SSCR0 = 0x%08X\r\n", 0xF|SSP_SSCR0_EDSS);
		break;
	default:
		bbu_printf("Unsupport data length\r\n");
		break;
	}
	
	ssp.base->SSCR0 |= SSP_SSCR0_RIM|SSP_SSCR0_TIM;
	bbu_printf("ssp.base->SSCR0 = 0x%08X\r\n", ssp.base->SSCR0 | SSP_SSCR0_RIM|SSP_SSCR0_TIM);

	if (ssp_clk == SSP_CLK_52M)
	{
		if (cpu_is_pxa1826_z1z2())
		{
			ssp.base->SSCR0 |= BIT_27;
		}
	}
	
	SSP2->SSCR1 = SSP_SSCR1_TTELP|
	              SSP_SSCR1_TTE  |
	              SSP_SSCR1_TRAIL|
	              SSP_SSCR1_TSRE |
	              SSP_SSCR1_RSRE |
	              (0x07<<SSP_SSCR1_RFT_BASE)|
	              (0x01<<SSP_SSCR1_TFT_BASE);
	// bbu_printf("SSP2->SSCR1 = 0x%08X\r\n", SSP_SSCR1_TTELP|
	//               SSP_SSCR1_TTE  |
	//               SSP_SSCR1_TRAIL|
	//               SSP_SSCR1_TSRE |
	//               SSP_SSCR1_RSRE |
	//               (0x07<<SSP_SSCR1_RFT_BASE)|
	//               (0x01<<SSP_SSCR1_TFT_BASE));
	SSP2->SSTO = SSP_RX_TIMEOUT;
	//bbu_printf("SSP2->SSTO = 0x%08X\r\n", SSP_RX_TIMEOUT);
	//*(uint32_t*)0xD401C044 |= BIT_8;

	ssp.base->SSCR0 |= SSP_SSCR0_SSE;
	//bbu_printf("ssp.base->SSCR0 = 0x%08X\r\n", ssp.base->SSCR0 | SSP_SSCR0_SSE);
	
	return;
}

static void ssp_lpbk_irq(){
	SSP_UNIT ssp = ssp_table[ssp_index];

	dma_clr_irq(SSP_DMA_RX_CHANNEL);
	INT_Disable(IRQ_MMP_DMA1);
	uint32_t j = 0;
	for(j=0; j<bytes/4; j++){
		//bbu_printf("data_temp = %02X\r\ndata_tra = %02X\r\n", data_temp[j], data_tra[j]);
		if(buf_rx[j] != buf_tx[j]){
			bbu_printf("FAIL\r\n");
			goto done;
			//Q_ASSERT(0);
		}
	}
	
	bbu_printf("PASS\r\n");
	
	done:
		ssp.base->SSSR   = SSP_SR_CLR;
		ssp.base->SSCR1 &= ~SSP_CR1_IE;
		ssp.base->SSTO   = 0;
		ssp.base->SSCR0  = 0;
		ssp_flush();
		ssp_dismap_channel();
	
		bbu_mfree((uint32_t*)p1);
		bbu_mfree((uint32_t*)p2);
		bbu_mfree((uint32_t*)p3);
		bbu_mfree((uint32_t*)p4);
		mem_inited = 0;
}

void ssp_lpbk_test(){
	uint32_t* desc_rx = NULL;
	uint32_t* desc_tx = NULL;
	
	SSP_UNIT ssp = ssp_table[ssp_index];
	
	DMA->DCSR[SSP_DMA_RX_CHANNEL] &= ~CSR_RUN;
	DMA->DCSR[SSP_DMA_TX_CHANNEL] &= ~CSR_RUN;

	if(!mem_inited){
		buf_rx  = bbu_nc_malloc(bytes+4);
		buf_tx  = bbu_nc_malloc(bytes+4);
		desc_rx = bbu_nc_malloc(3*16);
		desc_tx = bbu_nc_malloc(3*16);
		
		//align address
		p1 = (uint32_t)buf_rx;
		p2 = (uint32_t)buf_tx;
		p3 = (uint32_t)desc_rx;
		p4 = (uint32_t)desc_tx;

		buf_rx  = (uint32_t*)(((uint32_t)buf_rx & 0xFFFFFFF0) + 0x10);
		buf_tx  = (uint32_t*)(((uint32_t)buf_tx & 0xFFFFFFF0) + 0x10);
		desc_rx = (uint32_t*)(((uint32_t)desc_rx & 0xFFFFFFF0) + 0x10);
		desc_tx = (uint32_t*)(((uint32_t)desc_tx & 0xFFFFFFF0) + 0x10);
		mem_inited = 1;
	}else{
		ssp.base->SSSR   = SSP_SR_CLR;
		ssp.base->SSCR1 &= ~SSP_CR1_IE;
		ssp.base->SSTO   = 0;
		ssp.base->SSCR0  = 0;
		ssp_flush();
	}
	
	memset(buf_rx, 0, bytes);
	uint32_t i = 0;
	for(i=0; i<bytes/4; i++){
		buf_tx[i] = 0xFFFF0000;
	}
	
	bbu_printf("TX Address is 0x%08X\r\n", buf_tx);
	bbu_printf("RX Address is 0x%08X\r\n", buf_rx);
	
	//*(uint32_t *)0xd4282864 = 0x1 | (0x1<<3);  //Enable DMA clock
	
	ssp_init();
	
	bbu_printf("DATA transfer length: %d bytes  Data width: %s\r\n", bytes, data_length_str[data_length]);
	
	ssp_map_channel();

	uint32_t tx_cmd = 0;
	uint32_t rx_cmd = 0;

	DMA->DALGN |= (1 << SSP_DMA_RX_CHANNEL);
	DMA->DALGN |= (1 << SSP_DMA_TX_CHANNEL);
	
	uint32_t burst_size = 4; //64 bytes.
	uint32_t cmd_width  = 1; //1 byte.
	
	switch(data_length){
	case SSP_DATA_LEN_8:
		cmd_width = 1;
		break;
	case SSP_DATA_LEN_16:
		cmd_width = 2;
		break;
	case SSP_DATA_LEN_32:
		cmd_width = 3;
		break;
	default:
		bbu_printf("Unsupport data length\r\n");
		break;
	}
	
	tx_cmd = bytes|(cmd_width<<CMD_WIDTH_BASE)|(burst_size<<CMD_SIZE_BASE)|(1<<28)|(1<<31);
	rx_cmd = bytes|(cmd_width<<CMD_WIDTH_BASE)|(burst_size<<CMD_SIZE_BASE)|(1<<29)|(1<<30);
	//bbu_printf("DMA TX command: 0x%08X\r\n",tx_cmd);
	//bbu_printf("DMA RX command: 0x%08X\r\n",rx_cmd);
	
	dma_config_descriptor(desc_rx, NULL, (uint32_t)(&(ssp.base->SSDDR)), (uint32_t)buf_rx, rx_cmd, 1);
	dma_config_descriptor(desc_tx, NULL, (uint32_t)buf_tx, (uint32_t)(&(ssp.base->SSDDR)), tx_cmd, 1);

	dma_load_descriptor((P_DMA_DESCP)desc_rx, SSP_DMA_RX_CHANNEL);
	dma_load_descriptor((P_DMA_DESCP)desc_tx, SSP_DMA_TX_CHANNEL);
	
	//dma_stop_irq_en(SSP_DMA_RX_CHANNEL);
	//dma_clr_irq(SSP_DMA_RX_CHANNEL);
	
	//ISR_Connect(IRQ_MMP_DMA1, ssp_lpbk_irq, NULL);
	//INT_Enable(IRQ_MMP_DMA1, 0, 15);

	DMA->DCSR[SSP_DMA_RX_CHANNEL] |= CSR_RUN;
	DMA->DCSR[SSP_DMA_TX_CHANNEL] |= CSR_RUN;
	//bbu_printf("DMA->DCSR[SSP_DMA_RX_CHANNEL] = 0x%08X\r\n", DMA->DCSR[SSP_DMA_RX_CHANNEL] | CSR_RUN);
	//bbu_printf("DMA->DCSR[SSP_DMA_TX_CHANNEL] = 0x%08X\r\n", DMA->DCSR[SSP_DMA_TX_CHANNEL] | CSR_RUN);

	while(!(DMA->DCSR[SSP_DMA_TX_CHANNEL] & CSR_STOPINTR));
	while(!(DMA->DCSR[SSP_DMA_RX_CHANNEL] & CSR_STOPINTR));
	// {
	// 	bbu_printf("DMA->DCSR[SSP_DMA_RX_CHANNEL]: 0x%08X\r\n", DMA->DCSR[SSP_DMA_RX_CHANNEL]);
	// 	bbu_printf("DMA->DCSR[SSP_DMA_TX_CHANNEL]: 0x%08X\r\n", DMA->DCSR[SSP_DMA_TX_CHANNEL]);
	// 	bbu_printf("SSP2->SSSR: 0x%08X\r\n", SSP2->SSSR);
	// }

	for(int j=0; j<bytes/4; j++){
		//bbu_printf("data_temp = %02X\r\ndata_tra = %02X\r\n", data_temp[j], data_tra[j]);
		if(buf_rx[j] != buf_tx[j]){
			bbu_printf("FAIL\r\n");
			goto done;
		}
	}
	
	bbu_printf("PASS\r\n");

	done:
		ssp.base->SSSR   = SSP_SR_CLR;
		ssp.base->SSCR1 &= ~SSP_CR1_IE;
		ssp.base->SSTO   = 0;
		ssp.base->SSCR0  = 0;
		ssp_flush();
		ssp_dismap_channel();
	
		bbu_mfree((uint32_t*)p1);
		bbu_mfree((uint32_t*)p2);
		bbu_mfree((uint32_t*)p3);
		bbu_mfree((uint32_t*)p4);
		mem_inited = 0;

	return;
}

void ssp_set_clk(uint32_t clk){
	switch(clk){
	case SSP_CLK_6_5M:
	case SSP_CLK_13M:
	case SSP_CLK_26M:
	case SSP_CLK_52M:
		ssp_clk = clk;
		bbu_printf("SSP Clock has been set to %s\r\n", clk_str[clk]);
		break;
	default:
		bbu_printf("Unsupport clock source\r\n");
		break;
	}
}

void ssp_set_data_width(uint32_t width){
	switch(width){
	case SSP_DATA_LEN_8:
	case SSP_DATA_LEN_16:
	case SSP_DATA_LEN_32:
		data_length = width;
		bbu_printf("SSP Data width has been set to %s\r\n", data_length_str[width]);
		break;
	default:
		bbu_printf("Unsupport Data width\r\n");
		break;
	}
}

static void bbu_ssplpbk_help(){
	bbu_printf("\r\nUsage: ssplpbk <command> [args]\r\n");
	bbu_printf("1. ssplpbk lpbk             -- ssp loopback test\r\n");
	bbu_printf("2. ssplpbk clk <clk>        -- set ssp loopback bit clock source\r\n");
	bbu_printf("           <clk>        -- 0~3\r\n");
	bbu_printf("3. ssplpbk daw <data width> -- set ssp loopback bit clock source\r\n");
	bbu_printf("           <data width> -- 0~2\r\n");
	bbu_printf("BBU> ");
	
	return;
}

void bbu_ssp_test(uint8_t argc, uint8_t **argv){
	uint32_t t = 0;
	switch(argc){
	case 0:
	case 1:
		bbu_ssplpbk_help();
		break;
	case 2:
		if(check_help(argv[1])){
			bbu_ssplpbk_help();
			break;
		}
		if(!strcmp((const char*)argv[1], "lpbk")){
			ssp_lpbk_test();
			break;
		}
		bbu_ssplpbk_help();
		break;
	case 3:
		if(!strcmp((const char*)argv[1], "clk")){
			t = conv_dec((char*)argv[2]);
			ssp_set_clk(t);
			break;
		}
		if(!strcmp((const char*)argv[1], "daw")){
			t = conv_dec((char*)argv[2]);
			ssp_set_data_width(t);
			break;
		}
		bbu_ssplpbk_help();
		break;
	default:
		bbu_ssplpbk_help();
		break;
	}
}





