#include "panel_drv.h"

#include "timer_dec.h"
#include "gpio_dec.h"

static int lcd_panel_interface_init(struct panel_spec *self, uint32_t sclk)
{
	struct s_mcu_ctx *mcu_ctx = NULL;

	if(NULL == self){
		bbu_printf("ERROR: panel_interface_init, Invalid param\r\n");
		return -1;
	}

	gpio_output_set(24, GPIO_LOW); /*IM0 = 0 for mcu*/
	gpio_output_set(25, GPIO_LOW); /*IM1 = 0 for mcu*/
	gpio_output_set(26, GPIO_LOW); /*IM2 = 0 for mcu*/
	gpio_output_set(27, GPIO_LOW); /*IM3 = 0 for mcu*/

	mcu_ctx = mcu_init(sclk, self->info.mcu);
	if(NULL == mcu_ctx){
		bbu_printf("ERROR: panel_interface_init, mcu init fail!\r\n");
		return -1;
	}
	self->panel_if = (void*)mcu_ctx;
	return 0;
}


static int lcd_panel_init(struct panel_spec *self)
{
	struct s_mcu_ctx* mcu_ctx = NULL;

	if(NULL == self){
		bbu_printf("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		bbu_printf("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;

	mcu_write_cmd(mcu_ctx, 0xFE, 8);
	mcu_write_cmd(mcu_ctx, 0xEF, 8);
	if(self->info.mcu->format == MCU_FORMAT_RGB666)
		mcu_write_cmd_data(mcu_ctx, 0x3a, 8, 0x06, 8); //format is RGB666
	else
		mcu_write_cmd_data(mcu_ctx, 0x3A, 8, 0x05, 8); //format is RGB565
	mcu_write_cmd_data(mcu_ctx, 0x36, 8, 0x48, 8);

	mcu_write_cmd_data(mcu_ctx, 0xA4, 8, 0x44, 8);
	mcu_write_data(mcu_ctx, 0x44, 8);
	mcu_write_cmd_data(mcu_ctx, 0xA5, 8, 0x42, 8);
	mcu_write_data(mcu_ctx, 0x42, 8);
	mcu_write_cmd_data(mcu_ctx, 0xAA, 8, 0x88, 8);
	mcu_write_data(mcu_ctx, 0x88, 8);

	mcu_write_cmd_data(mcu_ctx, 0xe8, 8, 0x11, 8);//frame rate is 71.8Hz
	mcu_write_data(mcu_ctx, 0x0b, 8);
//	mcu_write_cmd_data(mcu_ctx, 0xE8, 8, 0x1F, 8);//frame rate is 20Hz
//	mcu_write_data(mcu_ctx, 0x40, 8);
	mcu_write_cmd_data(mcu_ctx, 0xE3, 8, 0x01, 8); //source ps=01
	mcu_write_data(mcu_ctx, 0x10, 8);
	mcu_write_cmd_data(mcu_ctx, 0xFF, 8, 0x61, 8);

	mcu_write_cmd_data(mcu_ctx, 0xAC, 8, 0x00, 8); //ldo enable
	mcu_write_cmd_data(mcu_ctx, 0xAD, 8, 0x33, 8); //DIG_VREFAD_VRDD[2]
	mcu_write_cmd_data(mcu_ctx, 0xAF, 8, 0x55, 8); //DIG_VREFAD_VRDD[2]

    /*GAMMA*/
	mcu_write_cmd_data(mcu_ctx, 0xF0, 8, 0x02, 8);
	mcu_write_data(mcu_ctx, 0x01, 8);
	mcu_write_data(mcu_ctx, 0x00, 8);
	mcu_write_data(mcu_ctx, 0x0A, 8);
	mcu_write_data(mcu_ctx, 0x0E, 8);
	mcu_write_data(mcu_ctx, 0x14, 8);
	mcu_write_cmd_data(mcu_ctx, 0xF1, 8, 0x01, 8);
	mcu_write_data(mcu_ctx, 0x02, 8);
	mcu_write_data(mcu_ctx, 0x00, 8);
	mcu_write_data(mcu_ctx, 0x11, 8);
	mcu_write_data(mcu_ctx, 0x18, 8);
	mcu_write_data(mcu_ctx, 0x09, 8);
	mcu_write_cmd_data(mcu_ctx, 0xF2, 8, 0x13, 8);
	mcu_write_data(mcu_ctx, 0x0A, 8);
	mcu_write_data(mcu_ctx, 0x3E, 8);
	mcu_write_data(mcu_ctx, 0x03, 8);
	mcu_write_data(mcu_ctx, 0x04, 8);
	mcu_write_data(mcu_ctx, 0x50, 8);
	mcu_write_cmd_data(mcu_ctx, 0xF3, 8, 0x0B, 8);
	mcu_write_data(mcu_ctx, 0x0A, 8);
	mcu_write_data(mcu_ctx, 0x37, 8);
	mcu_write_data(mcu_ctx, 0x06, 8);
	mcu_write_data(mcu_ctx, 0x04, 8);
	mcu_write_data(mcu_ctx, 0x44, 8);
	mcu_write_cmd_data(mcu_ctx, 0xF4, 8, 0x0B, 8);
	mcu_write_data(mcu_ctx, 0x19, 8);
	mcu_write_data(mcu_ctx, 0x16, 8);
	mcu_write_data(mcu_ctx, 0x24, 8);
	mcu_write_data(mcu_ctx, 0x24, 8);
	mcu_write_data(mcu_ctx, 0x0F, 8);
	mcu_write_cmd_data(mcu_ctx, 0xF5, 8, 0x0C, 8);
	mcu_write_data(mcu_ctx, 0x18, 8);
	mcu_write_data(mcu_ctx, 0x15, 8);
	mcu_write_data(mcu_ctx, 0x10, 8);
	mcu_write_data(mcu_ctx, 0x12, 8);
	mcu_write_data(mcu_ctx, 0x0F, 8);

	mcu_write_cmd(mcu_ctx, 0x35, 8); //enable TE

	mcu_write_cmd(mcu_ctx, 0x11, 8);
	mdelay(120);
	mcu_write_cmd(mcu_ctx, 0x29, 8);
	mdelay(20);
	return 0;
}

static unsigned int lcd_panel_readid(struct panel_spec *self)
{
	struct s_mcu_ctx* mcu_ctx = NULL;
    uint32_t read_id = 0;

	if(NULL == self){
		bbu_printf("ERROR: panel_readid, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		bbu_printf("ERROR: panel_readid, mcu has not been inited!\r\n");
		return -1;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_read_data(mcu_ctx, 0xDA, 8, &read_id, 8);
	if(read_id != 0x0){
		bbu_printf("ERROR: panel_readid, read 0xDA expect 0x0, but receive 0x%x!\r\n",
			read_id);
		return -1;
	}
	bbu_printf("INFO: panel_readid, read 0xDA receive 0x%x!\r\n",read_id);
	

	mcu_read_data(mcu_ctx, 0xDB, 8, &read_id, 8);
	if(read_id != 0x93){
		bbu_printf("ERROR: panel_readid, read 0xDB expect 0x93, but receive 0x%x!\r\n",
			read_id);
		return -1;
	}
	bbu_printf("INFO: panel_readid, read 0xDB receive 0x%x!\r\n",read_id);

	mcu_read_data(mcu_ctx, 0xDC, 8, &read_id, 8);
	if(read_id != 0x05){
		bbu_printf("ERROR: panel_readid, read 0xDC expect 0x05, but receive 0x%x!\r\n",
			read_id);
		return -1;
	}
	bbu_printf("INFO: panel_readid, read 0xDC receive 0x%x!\r\n",read_id);

	mcu_read_data(mcu_ctx, 0x04, 8, &read_id, 24);
	if(read_id != 0x9305){
		bbu_printf("ERROR: panel_readid, read 0x04 expect 0x9305, but receive 0x%x!\r\n",
			read_id);
		return -1;
	}
	bbu_printf("INFO: panel_readid, read 0x04 receive 0x%x!\r\n",read_id);

/*read status
	mcu_read_data(mcu_ctx, 0x09, 8, &read_id, 32);
	bbu_printf("INFO: panel_readid, read 0x09 receive 0x%x!\r\n", read_id);
*/

    return read_id;
}

static int lcd_panel_display_on(struct panel_spec *self)
{
	struct s_mcu_ctx* mcu_ctx = NULL;

	if(NULL == self){
		bbu_printf("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		bbu_printf("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_write_cmd(mcu_ctx, 0x11, 8);
	mdelay(120);
	mcu_write_cmd(mcu_ctx, 0x29, 8);
	mdelay(20);
    return 0;
}

static int lcd_panel_display_off(struct panel_spec *self)
{
	struct s_mcu_ctx* mcu_ctx = NULL;

	if(NULL == self){
		bbu_printf("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		bbu_printf("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_write_cmd(mcu_ctx, 0x28, 8);
	mdelay(20);
	mcu_write_cmd(mcu_ctx, 0x10, 8);
	mdelay(120);
    return 0;
}

static int lcd_panel_invalid(struct panel_spec *self, uint32_t start_x,
	uint32_t start_y, uint32_t end_x, uint32_t end_y)
{
	struct s_mcu_ctx* mcu_ctx = NULL;

	bbu_printf("INFO: lcd_panel_invalid++\r\n");

	if(NULL == self){
		bbu_printf("ERROR: lcd_panel_invalid, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		bbu_printf("ERROR: lcd_panel_invalid, mcu has not been inited!\r\n");
		return -1;
	}
	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_write_cmd_data(mcu_ctx, 0x2A, 8, ((start_x >> 8) & 0xFF), 8);
	mcu_write_data(mcu_ctx, (start_x & 0xFF), 8);
	mcu_write_data(mcu_ctx, ((end_x >> 8) & 0xFF), 8);
	mcu_write_data(mcu_ctx, (end_x & 0xFF), 8);

	mcu_write_cmd_data(mcu_ctx, 0x2B, 8, ((start_y >> 8) & 0xFF), 8);
	mcu_write_data(mcu_ctx, (start_y & 0xFF), 8);
	mcu_write_data(mcu_ctx, ((end_y >> 8) & 0xFF), 8);
	mcu_write_data(mcu_ctx, (end_y & 0xFF), 8);

	mcu_write_cmd(mcu_ctx, 0x2C, 8);
	return 0;
}

static struct panel_operations lcd_gc9305_mcu_ops = {
	.panel_interface_init = lcd_panel_interface_init,
	.panel_init = lcd_panel_init,
	.panel_invalid = lcd_panel_invalid,
	.panel_suspend = lcd_panel_display_off,
	.panel_resume = lcd_panel_display_on,
	.panel_readid = lcd_panel_readid,
};

static struct timing_mcu lcd_gc9305_mcu_timing = {
 	.rlpw = 200,
	.rhpw = 200,
 	.wlpw = 15,
	.whpw = 15,
};

static struct mcu_info lcd_gc9305_mcu_info = {
	.bus_mode = MCU_BUS_8080,
	.format = MCU_FORMAT_RGB565,
	.endian = MCU_ENDIAN_LSB,
	.device_id = 0,
	.force_cs = 0,
	.timing = &lcd_gc9305_mcu_timing,
};


struct panel_spec lcd_gc9305_mcu_spec = {
	.cap = LCD_CAP_NORMAL,
	.width = 240,
	.height = 320,
	.type = LCD_MODE_MCU,
	.te_pol = LCD_POLARITY_POS,
	.info = {
		.mcu = &lcd_gc9305_mcu_info,
	},
	.panel_if = NULL,
	.ops = &lcd_gc9305_mcu_ops,
};
