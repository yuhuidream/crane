#ifndef _GD5F_H_
#define _GD5F_H_
#include "predefines.h"
#include "ssp_dec.h"
#include "clock.h"
#include "qassert.h"


#define SSP_READID_COMMAND_GD5F				0x9F
#define SSP_GET_FEATURE_COMMAND_GD5F		0x0F
#define SSP_SET_FEATURE_COMMAND_GD5F		0x1F
#define SSP_WRITE_ENABLE_COMMAND_GD5F		0x06
#define SSP_WRITE_DISABLE_COMMAND_GD5F		0x04
#define SSP_PAGE_READ_TCACHE_COMMAND_GD5F	0x13
#define SSP_PAGE_READ_FCACHE_COMMAND_GD5F	0x0B
#define SSP_PAGE_WRITE_TCACHE_CONMMAND_GD5F	0x02
#define SSP_PAGE_WRITE_TFLASH_CONMMAND_GD5F	0x10
#define SSP_RESET_COMMAND_GD5F				0xFF
#define	SSP_ERASE_COMMAND_GD5F				0xD8
#define SSP_BYTES_PER_PAGE_GD5F				0x800
#define SSP_STRESS_TEST_COUNT_GD5F			20
#define SSP_STRESS_TEST_BYTES				0x20000
#define SSP_BYTES_PER_BLOCK_GD5F			128

typedef enum{
	SSP_TEST_MODE_W = 0,
	SSP_TEST_MODE_R
}SSP_TEST_MODE;

void ssp_init_gd5f(uint32_t clk);
void ssp_reset_gd5f();
void ssp_read_id_gd5f();
uint8_t ssp_get_feature_gd5f(uint8_t addr);
void ssp_set_feature_gd5f(uint8_t status, uint8_t addr);
void ssp_write_enable_gd5f();
void ssp_write_disable_gd5f();
void ssp_unlock_gd5f();
int ssp_wait_ready_gd5f();
void ssp_write_tail_gd5f(uint32_t offset, uint32_t start_addr, uint32_t len, uint8_t *buf);
void ssp_read_tail_gd5f(uint32_t offset, uint32_t start_addr, uint32_t len, uint8_t *buf);

void ssp_write_gd5f(uint32_t offset, uint32_t start_addr, uint32_t len, uint8_t *buf);
void ssp_read_gd5f(uint32_t offset, uint32_t start_addr, uint32_t len, uint8_t *buf);
void ssp_write_dma_gd5f(uint32_t offset, uint32_t len, uint32_t* buf);
void ssp_read_dma_gd5f(uint32_t offset, uint32_t len, uint32_t* buf);

void ssp_erase_gd5f(uint32_t offset, uint32_t len);
void ssp_stress_test_gd5f(uint32_t count);
void ssp_speed_test(uint32_t mode, uint32_t test_count);
void ssp_stress_test_dma_gd5f(uint32_t count);
void ssp_speed_test_dma(uint32_t mode, uint32_t test_count);

void ssp_test_gd5f(int argc, uint8_t **argv);


#endif
