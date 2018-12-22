#ifndef _SSP_LPBK_H_
#define _SSP_LPBK_H_
#include "predefines.h"
#include "SSP.h"
#include "APBCLK.h"
#include "timer_dec.h"
#include "cache_ops.h"
#include "gpio_dec.h"
#include "mfp-emei.h"

#define SSP_TIMEOUT_MAX					(0x2000)
#define SSP_RX_TIMEOUT					(0x2000)
#define SSP_CR1_IE						(SSP_SSCR1_TINTE|SSP_SSCR1_TIE|SSP_SSCR1_RIE)
#define SSP_SR_CLR						(SSP_SSSR_ROR|SSP_SSSR_TINT)

#define	SSP_DMA_TX_CHANNEL				6
#define SSP_DMA_RX_CHANNEL				7

typedef struct SSP_UNIT_T{
	SSP_TypeDef* base;
	uint32_t irq;
}SSP_UNIT;

typedef struct DMA_DESCP_TAG{
	uint32_t ddadr;
	uint32_t dsadr;
	uint32_t dtadr;
	uint32_t dcmd;
}*P_DMA_DESCP, DMA_DESCP;

void ssp_lpbk_test();

void ssp_set_clk(uint32_t clk);

void ssp_set_data_width(uint32_t width);

void bbu_ssp_test(uint8_t argc, uint8_t **argv);

#endif
