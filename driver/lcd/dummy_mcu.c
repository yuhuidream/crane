#include "panel_drv.h"

static int lcd_panel_interface_init(struct panel_spec *self, uint32_t sclk)
{
	struct s_mcu_ctx *mcu_ctx = NULL;

	if(NULL == self){
		bbu_printf("ERROR: panel_interface_init, Invalid param");
		return -1;
	}

	mcu_ctx = mcu_init(sclk, self->info.mcu);
	if(NULL == mcu_ctx){
		bbu_printf("ERROR: panel_interface_init, mcu init fail!");
		return -1;
	}
	self->panel_if = (void*)mcu_ctx;
	return 0;
}

static struct panel_operations lcd_dummy_mcu_ops = {
	.panel_interface_init = lcd_panel_interface_init,
	.panel_init = NULL,
	.panel_invalid = NULL,
	.panel_suspend = NULL,
	.panel_resume = NULL,
	.panel_readid = NULL,
};

static struct timing_mcu lcd_dummy_mcu_timing = {
 	.rlpw = 200,
	.rhpw = 200,
 	.wlpw = 15,
	.whpw = 15,
};

static struct mcu_info lcd_dummy_mcu_info = {
	.bus_mode = MCU_BUS_8080,
	.format = MCU_FORMAT_RGB565,
	.endian = MCU_ENDIAN_MSB,
	.device_id = 0,
	.force_cs = 0,
	.timing = &lcd_dummy_mcu_timing,
};


struct panel_spec lcd_dummy_mcu_spec = {
	.cap = LCD_CAP_FAKE,
	.width = 240,
	.height = 320,
	.type = LCD_MODE_MCU,
	.te_pol = LCD_POLARITY_POS,
	.info = {
		.mcu = &lcd_dummy_mcu_info,
	},
	.panel_if = NULL,
	.ops = &lcd_dummy_mcu_ops,
};
