#ifndef _SSP_DEC_H_
#define _SSP_DEC_H_
#include "predefines.h"
#include "SSP.h"
#include "APBCLK.h"
#include "timer_dec.h"
#include "cache_ops.h"
#include "gpio_dec.h"
#include "mfp-emei.h"

#define SSP_TIMEOUT_MAX					(0x2000)
#define SSP_RX_TIMEOUT					(0x2000)
#define SSP_CS							(GPIO25)
#define SSP_CR1_IE						(SSP_SSCR1_TINTE|SSP_SSCR1_TIE|SSP_SSCR1_RIE)
#define SSP_SR_CLR						(SSP_SSSR_ROR|SSP_SSSR_TINT)

#define SSP_XFER_BEGIN					0x01
#define SSP_XFER_END					0x02
#define SSP_XFER_BE						0x03

#define	SSP_DMA_TX_CHANNEL				6
#define SSP_DMA_RX_CHANNEL				7

typedef enum{
	SSP_CLK_6_5M = 0,
	SSP_CLK_13M,
	SSP_CLK_26M,
	SSP_CLK_52M
}SSP_CLOCK;

typedef enum{
	SSP_DATA_LEN_8 = 0,
	SSP_DATA_LEN_16,
	SSP_DATA_LEN_18,
	SSP_DATA_LEN_32
}SSP_DATA_LEN;

typedef struct {
	unsigned int mode;
	unsigned int data_length;
	unsigned int speed;
	unsigned int tx_watermark;
	unsigned int rx_watermark;
	unsigned int fmt;
	unsigned int i2s_width;
	int irq;
}ssp_init_param;

#define SPI_CPHA    0x01	/* clock phase */
#define SPI_CPOL    0x02	/* clock polarity */
#define SPI_MODE_0  (0|0)	/* (original MicroWire) */
#define SPI_MODE_1  (0|SPI_CPHA)
#define SPI_MODE_2  (SPI_CPOL|0)
#define SPI_MODE_3  (SPI_CPOL|SPI_CPHA)

/*psp params*/
#define SSP_SPI_MODE			(0x0 << 1)
#define SSP_SSP_MODE			(0x1 << 1)
#define SSP_PSP_MODE			(0x3 << 1)
#define SSP_ONCHIP_CLOCK		(0x0 << 6)
#define SSP_EXTER_CLOCK		(0x1 << 6)
#define SSP_DATA_16BIT		16
#define SSP_DATA_8BIT		8
#define SSP_DATA_32BIT		32
#define SSP_DATA_18BIT		18


void ssp_dec_init(SSP_CLOCK clk, SSP_DATA_LEN data_len);
void ssp_cs_activate();
void ssp_cs_deactivate();
//void ssp_flush();
int ssp_xfer(uint32_t len, uint8_t *data_in, uint8_t *data_out, uint32_t flag);

uint8_t ssp_stop();




#endif
