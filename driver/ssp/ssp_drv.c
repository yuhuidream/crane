#include "bbu.h"
#include "NSSP.h"
#include "PMUA.h"
#include "PMUM.h"
#include "SQU.h"
#include "DMA.h"
#include "ssp_dec.h"

static SSP_TypeDef *ssp_ports[3] = {SSP0, SSP1, SSP2};

#define SYSCLK_EN            (0x1<<31)
#define SYSCLK_GEN_156M       (1 << 30)
#define SYSCLK_GEN_26M       (0 << 30)
#define BITCLK_EN            (1 << 29)
#define DIV_2                (0 << 27)
#define DIV_4                (1 << 27)
#define DIV_6                (2 << 27)
#define DIV_8                (3 << 27)
#define DENOM                (1 << 15)
#define NOM                  (3 << 0)

#define SSP_DATA_16_BIT		(15 << 5)
#define SSP_DATA_8_BIT		(7 << 5)
#define SSP_DATA_32_BIT		(31 << 5)
#define SSP_DATA_18_BIT		(17 << 5)
#define TIMEOUT 100000

#define ssp_port(x)  ssp_ports[x]

static void __ssp_port_enable(int port, int ssp_clk)
{
	switch(port){
		case 0:
			APBClock->SSP0_CLK_RST = SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|SSP0_CLK_RST_RST|(ssp_clk<<4);
			//bbu_printf("APBClock->SSP0_CLK_RST = 0x%08X\r\n", SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|SSP0_CLK_RST_RST|(ssp_clk<<4));
			udelay(1);
			APBClock->SSP0_CLK_RST = SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|(ssp_clk<<4);
			//bbu_printf("APBClock->SSP0_CLK_RST = 0x%08X\r\n", SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|(ssp_clk<<4));
			break;
		case 1:
			PMUA->SQU_CLK_GATE_CTRL &= ~(0x1<<30); //[30]=0: audio_island_clk selects aclk, default 266Mhz for Real-Core)
			SQU->CTRL_0 |= (0x7<<2);   //[4:2]=0x7: APB Clock Div is 7
			PMUM->FCCR &= ~(0x1<<28);  //[28]=0: i2s clock divider select 312Mhz; [28]=1: select VCTCXO(default 26Mhz); 
			//Step1: disable sysclk and bitclck to set the related parameters
			PMUM->ISCCRX[1] = SYSCLK_GEN_156M | DIV_8 | DENOM | NOM;
			 //Step2: enable sysclk and bitclk
			PMUM->ISCCRX[1] |= (SYSCLK_EN | BITCLK_EN);
			//Step3: wait a little time here for new setting clock div valid. if we do step1&step2 in one step, the previous
			//DENOM & NOM setting will influnce the new clock div, which may need wait much more time for the new setting
			//clock div valid(500ns VS 10,000ns)
			udelay(1);

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
}

static void __ssp_reset_port(int port)
{
	ssp_port(port)->TOP_CTRL = 0;
	ssp_port(port)->FIFO_CTRL = 0;
	ssp_port(port)->PSP_CTRL = 0;
	ssp_port(port)->INT_EN = 0x30;
}

void ssp_config_master(int port, int is_master)
{
	if(is_master)
		ssp_port(port)->TOP_CTRL &= ~(SSP_SFRMDIR | SSP_SCLKDIR);
	else
		ssp_port(port)->TOP_CTRL |= (SSP_SFRMDIR | SSP_SCLKDIR);
}

void ssp_init_master(int port, int ssp_clk)
{
	__ssp_port_enable(port, ssp_clk);
	__ssp_reset_port(port);
	udelay(5);
	ssp_config_master(port, 1);
}


void ssp_port_enable(int port, int is_enable)
{
	uint32_t reg;
	volatile int k;

	if (is_enable)
		ssp_port(port)->TOP_CTRL |= SSP_SSE;
	else
		ssp_port(port)->TOP_CTRL &= ~SSP_SSE;

	udelay(5);
}

void ssp_set_selfloop(int port, int is_enable)
{
	if (is_enable)
		ssp_port(port)->TOP_CTRL |= SSP_LBM;
	else
		ssp_port(port)->TOP_CTRL &= ~SSP_LBM;
}

void ssp_set_dma_tx_enable(int port, int is_enable)
{
	if (is_enable)
		ssp_port(port)->FIFO_CTRL |= SSP_TSRE;
	else
		ssp_port(port)->FIFO_CTRL &= ~SSP_TSRE;
}

void ssp_set_dma_rx_enable(int port, int is_enable)
{
	if (is_enable)
		ssp_port(port)->FIFO_CTRL |= SSP_RSRE;
	else
		ssp_port(port)->FIFO_CTRL &= ~SSP_RSRE;
}


int ssp_config(int port, uint32_t mode, uint32_t flags, uint32_t psp_flags, uint32_t speed)
{
	uint32_t top_ctrl = ssp_port(port)->TOP_CTRL;
	uint32_t fifo_ctrl = ssp_port(port)->FIFO_CTRL;
	uint32_t psp_ctrl = ssp_port(port)->PSP_CTRL;

	top_ctrl &= ~(SSP_FRF(0x3) | SSP_DSS(0x1f));
	fifo_ctrl &= ~(SSP_TXFIFO_WR_ENDIAN(0x3) | SSP_RXFIFO_RD_ENDIAN(0x3) | SSP_RFT(0x1f) | SSP_TFT(0x1f));
	psp_ctrl &= ~(SSP_EDMYSTOP(0x7) | SSP_DMYSTOP(0x3) | SSP_EDMYSTRT(0x3) | SSP_DMYSTRT(0x3) | SSP_STRTDLY(0x7)
				| SSP_SFRMWDTH(0x3f) | SSP_SFRMDLY(0x7f) | SSP_SCMODE(0x3));

	ssp_port(port)->TOP_CTRL = speed | mode | top_ctrl;
	ssp_port(port)->FIFO_CTRL = flags | fifo_ctrl;
	ssp_port(port)->PSP_CTRL = psp_flags | psp_ctrl;
	ssp_port(port)->INT_EN = 0;

	return 0;
}

int ssp_get_txfifo_req(int port)
{
	return (ssp_port(port)->STATUS & SSP_TFS)? 1: 0;
}

int ssp_get_rxfifo_req(int port)
{
	return (ssp_port(port)->STATUS & SSP_RFS)? 1: 0;
}

int ssp_recvbuffer_isempty(int port)
{
	return !(ssp_port(port)->STATUS & SSP_RNE);
}

int ssp_sendbuffer_isfull(int port)
{
	return !(ssp_port(port)->STATUS & SSP_TNF);
}

int ssp_fifo_reset(int port)
{
	uint32_t sssr_reg = ssp_port(port)->STATUS;

	if (sssr_reg & SSP_SSSR_TFL_MSK)
		return -1;

	if ((sssr_reg & SSP_TNF) == 0)
		return -1;

	if ((sssr_reg & SSP_SSSR_RFL_MSK) >> 15 != 0x1f)
		return -1;

	if (sssr_reg & SSP_RNE)
		return -1;

	return 0;
}

void ssp_common_config(int port, ssp_init_param *param)
{
	uint32_t flags, speed, psp_flags, mode;
	flags = SSP_TFT(param->tx_watermark) | SSP_RFT(param->rx_watermark);
	speed = 0;
	psp_flags = 0;
	mode = param->mode;
	switch(param->data_length){
		case 8:
			speed |= SSP_DATA_8_BIT;
			break;
		case 18:
			speed |= SSP_DATA_18_BIT;
			break;
		case 32:
			speed |= SSP_DATA_32_BIT;
			break;
		case 16:
		default:
			speed |= SSP_DATA_16_BIT;
	}
	speed |= (SSP_TTELP | SSP_TTE);
	//disable int
	ssp_config(port, mode, flags, psp_flags, speed);

}

uint32_t ssp_get_datar(int port)
{
	return (&(ssp_port(port)->DATAR));
}

int ssp_write_word(int port, int data)
{
	int timeout = TIMEOUT;
	while (!(ssp_port(port)->STATUS & SSP_TNF)) {
		if (!--timeout){
			bbu_printf("ssp_write_word read timeout\n\r");
			return -1;
		}
	}

	ssp_port(port)->DATAR = data;
	return 0;
}

int ssp_read_word(int port)
{
	int timeout = TIMEOUT;

	while (!(ssp_port(port)->STATUS & SSP_RNE)) {
		if (!--timeout) {
			bbu_printf("ssp_read_word read timeout\n\r");
			return -1;
		}
	}

	return ssp_port(port)->DATAR;
}

int ssp_flush(int port)
{
	volatile uint32_t val;
	int timeout = TIMEOUT * 2;

	/* Verify TX FIFO is empty */
	while( ( (ssp_port(port)->STATUS & SSP_SSSR_TFL_MSK) &&  (ssp_port(port)->STATUS & SSP_TNF) ) || 
		(!(ssp_port(port)->STATUS & SSP_SSSR_TFL_MSK) && !((ssp_port(port)->STATUS & SSP_TNF))));

	do {
		while (ssp_port(port)->STATUS & SSP_RNE) {
			if (!--timeout)
				return -1;
			val = ssp_port(port)->DATAR;
		}
		if (!--timeout)
			return -1;
	} while (ssp_port(port)->STATUS & SSP_BSY);

	return 0;
}



void ssp_map_channel( int port, int tx_ch, int rx_ch){
	int rx_req, tx_req;
	switch(port)
	{
		case 0:
			rx_req = 52;
			tx_req = 53;
			break;
		case 2:
			rx_req = 60;
			tx_req = 61;
	}

	DMA->DRCMR1[rx_req] = RCMR_MAPVLD | rx_ch;
	DMA->DRCMR1[tx_req] = RCMR_MAPVLD | tx_ch;
	return;
}

void ssp_unmap_channel( int port, int tx_ch, int rx_ch){
	int rx_req, tx_req;
	switch(port)
	{
		case 0:
			rx_req = 52;
			tx_req = 53;
			break;
		case 2:
			rx_req = 60;
			tx_req = 61;
	}

	DMA->DRCMR1[rx_req] = 0;
	DMA->DRCMR1[tx_req] = 0;
	return;
}

void ssp_trailing_owner_select(int port, int owner)
{
	if (owner)  //DMA
		ssp_port(port)->TOP_CTRL |= SSP_TRAIL;
	else
		ssp_port(port)->TOP_CTRL &= ~SSP_TRAIL;
}

void ssp_trailing_set_timeout(int port, uint32_t timeout)
{
	ssp_port(port)->TO = timeout;
}