#include "panel_drv.h"

static int lcd_panel_interface_init(struct panel_spec *self, uint32_t sclk)
{
	struct s_spi_ctx *spi_ctx = NULL;

	if(NULL == self){
		bbu_printf("ERROR: panel_interface_init, Invalid param");
		return -1;
	}

	spi_ctx = spi_init(sclk, self->info.spi);
	if(NULL == spi_ctx){
		bbu_printf("ERROR: panel_interface_init, spi init fail!");
		return -1;
	}
	self->panel_if = (void*)spi_ctx;
	return 0;
}

static struct panel_operations lcd_dummy_spi_ops = {
	.panel_interface_init = lcd_panel_interface_init,
	.panel_init = NULL,
	.panel_invalid = NULL,
	.panel_suspend = NULL,
	.panel_resume = NULL,
	.panel_readid = NULL,
};

static struct timing_spi lcd_dummy_spi_timing = {
	.wclk = 5000, /*kHz*/
	.rclk = 5000, /*kHz*/
};

static struct spi_info lcd_dummy_spi_info = {
	.line_num = 3,
	.interface_id = 1, 
	.data_lane_num = 2,
	.format = SPI_FORMAT_RGB565,
	.device_id = 0,
	.sample_edge = SPI_EDGE_RISING,
	.force_cs = 0,
	.endian = SPI_ENDIAN_MSB,
	.timing = &lcd_dummy_spi_timing,
};


struct panel_spec lcd_dummy_spi_spec = {
	.cap = LCD_CAP_FAKE,
	.width = 240,
	.height = 320,
	.type = LCD_MODE_SPI,
	.te_pol = LCD_POLARITY_POS,
	.info = {
		.spi = &lcd_dummy_spi_info,
	},
	.panel_if = NULL,
	.ops = &lcd_dummy_spi_ops,
};

