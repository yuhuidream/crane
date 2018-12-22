#include "ssp_dec.h"

uint32_t ssp_clk_52m = 0;
static int null_writer(){
	int retry_count = SSP_TIMEOUT_MAX;

	while((--retry_count) && (!(SSP2->SSSR & SSP_SSSR_TNF)));
	if(retry_count == 0)
		return 0;

	SSP2->SSDDR = 0;
	
	return 1;
}

static int null_reader(){
	uint32_t temp = 0;
	int retry_count = SSP_TIMEOUT_MAX;

	while((--retry_count) && (!(SSP2->SSSR & SSP_SSSR_RNE)));
	if(retry_count == 0)
		return 0;

	temp = SSP2->SSDDR;
	(void)temp;

	return 1;
}

static int u8_writer(uint8_t tx_data){
	int retry_count = SSP_TIMEOUT_MAX;

	while((--retry_count) && (!(SSP2->SSSR & SSP_SSSR_TNF)));
	if(retry_count == 0)
		return 0;

	SSP2->SSDDR = tx_data;
	
	return 1;
}

static int u8_reader(uint8_t *rx_data){
	int retry_count = SSP_TIMEOUT_MAX;

	while((--retry_count) && (!(SSP2->SSSR & SSP_SSSR_RNE)));
	if(retry_count == 0)
		return 0;

	*(uint8_t*)rx_data = SSP2->SSDDR;

	return 1;
}
/*
static int u16_writer(uint16_t tx_data){
	int retry_count = SSP_TIMEOUT_MAX;

	while((--retry_count) && (!(SSP2->SSSR & SSP_SSSR_TNF)));
	if(retry_count == 0)
		return 0;

	SSP2->SSDDR = tx_data;
	
	return 1;
}

static int u16_reader(uint16_t *rx_data){
	int retry_count = SSP_TIMEOUT_MAX;

	while((--retry_count) && (!(SSP2->SSSR & SSP_SSSR_RNE)));
	if(retry_count == 0)
		return 0;

	*(uint16_t*)rx_data = SSP2->SSDDR;

	return 1;
}

static int u32_writer(uint32_t tx_data){
	int retry_count = SSP_TIMEOUT_MAX;

	while((--retry_count) && (!(SSP2->SSSR & SSP_SSSR_TNF)));
	if(retry_count == 0)
		return 0;

	SSP2->SSDDR = tx_data;
	
	return 1;
}

static int u32_reader(uint32_t *rx_data){
	int retry_count = SSP_TIMEOUT_MAX;

	while((--retry_count) && (!(SSP2->SSSR & SSP_SSSR_RNE)));
	if(retry_count == 0)
		return 0;

	*(uint32_t*)rx_data = SSP2->SSDDR;

	return 1;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#define DATA_LEN            8

void ssp_cs_activate(){
	gpio_output_low(SSP_CS);

	return;
}
void ssp_cs_deactivate(){
	gpio_output_high(SSP_CS);

	return;
}
#if 0
void ssp_flush(){
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
#endif

void ssp_dec_init(SSP_CLOCK clk, SSP_DATA_LEN data_len){
	//reset the APBclk.
	APBClock->SSP2_CLK_RST = SSP2_CLK_RST_APBCLK|SSP2_CLK_RST_FNCLK|SSP2_CLK_RST_RST|(clk<<4);
	udelay(1);
	APBClock->SSP2_CLK_RST = SSP2_CLK_RST_APBCLK|SSP2_CLK_RST_FNCLK|(clk<<4);

	gpio_set_output(SSP_CS);
	gpio_output_high(SSP_CS);

	SSP2->SSCR0 = 0;
	udelay(5);
	switch(data_len){
		case SSP_DATA_LEN_8:
			SSP2->SSCR0 = 0x7;
			break;
		case SSP_DATA_LEN_16:
			SSP2->SSCR0 = 0xF;
			break;
		case SSP_DATA_LEN_18:
			SSP2->SSCR0 = 0x1|SSP_SSCR0_EDSS;
			break;
		case SSP_DATA_LEN_32:
			SSP2->SSCR0 = 0xF|SSP_SSCR0_EDSS;
			break;
		default:
			bbu_printf("Unsupport data length\r\n");
			break;
	}
	if(clk==3){
		ssp_clk_52m = 1;
		SSP2->SSCR0 |= SSP_SSCR0_MM52;
	}

	return;
}

int ssp_xfer(uint32_t len, uint8_t *data_in, uint8_t *data_out, uint32_t flag){
	int ret = 0;
	//uint32_t retry = 0;
	uint32_t count = 0;

	if(len == 0)
		goto done;
	if(len % DATA_LEN){
		flag |= SSP_XFER_END;
		goto done;
	}
	
	if(flag & SSP_XFER_BEGIN){
		ssp_cs_activate();
		SSP2->SSCR0 &= ~SSP_SSCR0_SSE;
		SSP2->SSTO = SSP_RX_TIMEOUT;
		SSP2->SSCR1 = SSP_SSCR1_TTELP | SSP_SSCR1_TTE;
		if(ssp_clk_52m){
			SSP2->SSCR0 = SSP_SSCR0_TIM  |
			              SSP_SSCR0_RIM  |
			              SSP_SSCR0_MM52 |
			              7              |
			              SSP_SSCR0_SSE;
		}else{
			SSP2->SSCR0 = SSP_SSCR0_TIM  |
			              SSP_SSCR0_RIM  |
			              7              |
			              SSP_SSCR0_SSE;
		}
	}

	count = len/DATA_LEN;
	uint32_t i = 0;
	for(i=0; i<count; i++){
		//retry = SSP_TIMEOUT_MAX;
		//write tx data.
		if(data_out){
			ret = u8_writer(data_out[i]);
		}else{
			ret = null_writer();
		}
		if(!ret)
			break;
		
/*		while((SSP2->SSSR & SSP_SSSR_BSY)&retry--){*/
/*			udelay(1);*/
/*		}*/
		while((SSP2->SSSR & SSP_SSSR_BSY));
		//read rx data.
		if(data_in){
			ret = u8_reader(&data_in[i]);
		}else{
			ret = null_reader();
		}
		if(!ret)
			break;
	}

	done:
		if(flag & SSP_XFER_END){
			SSP2->SSSR = SSP_SR_CLR;
			SSP2->SSCR1 &= ~SSP_CR1_IE;
			SSP2->SSTO = 0;
			SSP2->SSCR0 = 0;
			if(ssp_clk_52m){
				SSP2->SSCR0 = SSP_SSCR0_MM52;//ssp use 52M clock.
			}else{
				SSP2->SSCR0 = 0;
			}
			ssp_cs_deactivate();
		}
		//bbu_printf("SSP2->SSCR0 = 0x%08X\r\n", SSP2->SSCR0);

	return ret;
}

