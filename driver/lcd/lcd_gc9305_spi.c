#include "panel_drv.h"

#include "timer_dec.h"
#include "gpio_dec.h"

static int lcd_panel_interface_init(struct panel_spec *self, uint32_t sclk)
{
	struct s_spi_ctx *spi_ctx = NULL;

	bbu_printf("INFO: panel_interface_init, spi+++\r\n");

	if(NULL == self){
		bbu_printf("ERROR: panel_interface_init, Invalid param\r\n");
		return -1;
	}

	if(self->info.spi->line_num == 3){
		if(self->info.spi->interface_id == 1){
			gpio_output_set(24, GPIO_HIGH); /*IM0 = 1 for spi 3-wire interfacd1*/
			gpio_output_set(25, GPIO_LOW); /*IM1 = 0 for spi 3-wire interfacd1*/
			gpio_output_set(26, GPIO_HIGH); /*IM2 =1 for spi 3-wire interfacd1*/
			gpio_output_set(27, GPIO_LOW); /*IM3 = 0 for spi 3-wire interfacd1*/
		} else {
			gpio_output_set(24, GPIO_HIGH); /*IM0 = 1 for spi 3-wire interfacd2*/
			gpio_output_set(25, GPIO_LOW); /*IM1 = 0 for spi 3-wire interfacd2*/
			gpio_output_set(26, GPIO_HIGH); /*IM2 =1 for spi 3-wire interfacd2*/
			gpio_output_set(27, GPIO_HIGH); /*IM3 = 1 for spi 3-wire interfacd2*/
		}
	} else {
		if(self->info.spi->interface_id == 1){
			gpio_output_set(24, GPIO_LOW); /*IM0 = 0 for spi 4-wire interfacd1*/
			gpio_output_set(25, GPIO_HIGH); /*IM1 = 1 for spi 4-wire interfacd1*/
			gpio_output_set(26, GPIO_HIGH); /*IM2 =1 for spi 4-wire interfacd1*/
			gpio_output_set(27, GPIO_LOW); /*IM3 = 0 for spi 4-wire interfacd1*/
		} else {
			gpio_output_set(24, GPIO_LOW); /*IM0 = 0 for spi 4-wire interfacd2*/
			gpio_output_set(25, GPIO_HIGH); /*IM1 = 1 for spi 4-wire interfacd2*/
			gpio_output_set(26, GPIO_HIGH); /*IM2 =1 for spi 4-wire interfacd2*/
			gpio_output_set(27, GPIO_HIGH); /*IM3 = 1 for spi 4-wire interfacd2*/
		}
	}

	spi_ctx = spi_init(sclk, self->info.spi);
	if(NULL == spi_ctx){
		bbu_printf("ERROR: panel_interface_init, spi init fail!\r\n");
		return -1;
	}
	self->panel_if = (void*)spi_ctx;
	return 0;
}


static int lcd_panel_init(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;

	if(NULL == self){
		bbu_printf("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		bbu_printf("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;

	spi_write_cmd(spi_ctx, 0xFE, 8);
	spi_write_cmd(spi_ctx, 0xEF, 8);
	spi_write_cmd(spi_ctx, 0x3A, 8);
	if(self->info.spi->format == SPI_FORMAT_RGB666)
		spi_write_data(spi_ctx, 0x06, 8); //format is RGB666
	else
		spi_write_data(spi_ctx, 0x05, 8); //format is RGB565
	spi_write_cmd(spi_ctx, 0x36, 8);
	spi_write_data(spi_ctx, 0x48, 8);

	spi_write_cmd(spi_ctx, 0xA4, 8);
	spi_write_data(spi_ctx, 0x44, 8);
	spi_write_data(spi_ctx, 0x44, 8);
	spi_write_cmd(spi_ctx, 0xA5, 8);
	spi_write_data(spi_ctx, 0x42, 8);
	spi_write_data(spi_ctx, 0x42, 8);
	spi_write_cmd(spi_ctx, 0xAA, 8);
	spi_write_data(spi_ctx, 0x88, 8);
	spi_write_data(spi_ctx, 0x88, 8);

	spi_write_cmd(spi_ctx, 0xE8, 8);//frame rate is 71.8Hz
	spi_write_data(spi_ctx, 0x11, 8);
	spi_write_data(spi_ctx, 0x0B, 8);
//	spi_write_cmd(spi_ctx, 0xE8, 8);//frame rate is 20Hz
//	spi_write_data(spi_ctx, 0x1F, 8);
//	spi_write_data(spi_ctx, 0x40, 8);
	spi_write_cmd(spi_ctx, 0xE3, 8);//source ps=01
	spi_write_data(spi_ctx, 0x01, 8);
	spi_write_data(spi_ctx, 0x10, 8);
	spi_write_cmd(spi_ctx, 0xFF, 8);
	spi_write_data(spi_ctx, 0x61, 8);

	if(self->info.spi->data_lane_num == 2){
		spi_write_cmd(spi_ctx, 0xE9, 8);
		switch(self->info.spi->format){
		case SPI_FORMAT_RGB565:
			spi_write_data(spi_ctx, 0x08, 8); //RGB565
			break;
		case SPI_FORMAT_RGB666:
			spi_write_data(spi_ctx, 0x09, 8); //RGB666 1 pixel
			break;
		case SPI_FORMAT_RGB666_2_3:
			spi_write_data(spi_ctx, 0x0A, 8); //RGB666 2-3 pixel
			break;
		case SPI_FORMAT_RGB888:
			spi_write_data(spi_ctx, 0x0C, 8); //RGB888 1 pixel
			break;
		case SPI_FORMAT_RGB888_2_3:
			spi_write_data(spi_ctx, 0x0E, 8); //RGB888 2-3 pixel
			break;
		}
	}

	spi_write_cmd(spi_ctx, 0xAC, 8);//ldo enable
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_cmd(spi_ctx, 0xAD, 8);//DIG_VREFAD_VRDD[2]
	spi_write_data(spi_ctx, 0x33, 8);
	spi_write_cmd(spi_ctx, 0xAF, 8);//DIG_VREFAD_VRDD[2]
	spi_write_data(spi_ctx, 0x55, 8);

    /*GAMMA*/
	spi_write_cmd(spi_ctx, 0xF0, 8);
	spi_write_data(spi_ctx, 0x02, 8);
	spi_write_data(spi_ctx, 0x01, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x0E, 8);
	spi_write_data(spi_ctx, 0x14, 8);
	spi_write_cmd(spi_ctx, 0xF1, 8);
	spi_write_data(spi_ctx, 0x01, 8);
	spi_write_data(spi_ctx, 0x02, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_data(spi_ctx, 0x11, 8);
	spi_write_data(spi_ctx, 0x18, 8);
	spi_write_data(spi_ctx, 0x09, 8);
	spi_write_cmd(spi_ctx, 0xF2, 8);
	spi_write_data(spi_ctx, 0x13, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x3E, 8);
	spi_write_data(spi_ctx, 0x03, 8);
	spi_write_data(spi_ctx, 0x04, 8);
	spi_write_data(spi_ctx, 0x50, 8);
	spi_write_cmd(spi_ctx, 0xF3, 8);
	spi_write_data(spi_ctx, 0x0B, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x37, 8);
	spi_write_data(spi_ctx, 0x06, 8);
	spi_write_data(spi_ctx, 0x04, 8);
	spi_write_data(spi_ctx, 0x44, 8);
	spi_write_cmd(spi_ctx, 0xF4, 8);
	spi_write_data(spi_ctx, 0x0B, 8);
	spi_write_data(spi_ctx, 0x19, 8);
	spi_write_data(spi_ctx, 0x16, 8);
	spi_write_data(spi_ctx, 0x24, 8);
	spi_write_data(spi_ctx, 0x24, 8);
	spi_write_data(spi_ctx, 0x0F, 8);
	spi_write_cmd(spi_ctx, 0xF5, 8);
	spi_write_data(spi_ctx, 0x0C, 8);
	spi_write_data(spi_ctx, 0x18, 8);
	spi_write_data(spi_ctx, 0x15, 8);
	spi_write_data(spi_ctx, 0x10, 8);
	spi_write_data(spi_ctx, 0x12, 8);
	spi_write_data(spi_ctx, 0x0F, 8);

	spi_write_cmd(spi_ctx, 0x35, 8);//enable TE
	mdelay(10);
	spi_write_cmd(spi_ctx, 0x11, 8);
	mdelay(120);
	spi_write_cmd(spi_ctx, 0x29, 8);
	mdelay(20);
	return 0;
}

static unsigned int lcd_panel_readid(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
    uint32_t read_id = 0;

	if(NULL == self){
		bbu_printf("ERROR: panel_readid, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		bbu_printf("ERROR: panel_readid, spi has not been inited!\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_read_data(spi_ctx, 0xDA, 8, &read_id, 8);
	if(read_id != 0x0){
		bbu_printf("ERROR: panel_readid, read 0xDA expect 0x0, but receive 0x%x!\r\n",
			read_id);
		return -1;
	}
	bbu_printf("INFO: panel_readid, read 0xDA receive 0x%x!\r\n",read_id);

	spi_read_data(spi_ctx, 0xDB, 8, &read_id, 8);
	if(read_id != 0x93){
		bbu_printf("ERROR: panel_readid, read 0xDB expect 0x93, but receive 0x%x!\r\n",
			read_id);
		return -1;
	}
	bbu_printf("INFO: panel_readid, read 0xDB receive 0x%x!\r\n",read_id);

	spi_read_data(spi_ctx, 0xDC, 8, &read_id, 8);
	if(read_id != 0x05){
		bbu_printf("ERROR: panel_readid, read 0xDC expect 0x05, but receive 0x%x!\r\n",
			read_id);
		return -1;
	}
	bbu_printf("INFO: panel_readid, read 0xDC receive 0x%x!\r\n",read_id);
	
	spi_read_data(spi_ctx, 0x04, 8, &read_id, 24);
	if(read_id != 0x9305){
		bbu_printf("ERROR: panel_readid, read 0x04 expect 0x9305, but receive 0x%x!\r\n",
			read_id);
		return -1;
	}
	bbu_printf("INFO: panel_readid, read 0x04 receive 0x%x!\r\n",read_id);

/*read status*/
	spi_read_data(spi_ctx, 0x09, 8, &read_id, 32);
	bbu_printf("INFO: panel_readid, read 0x09 receive 0x%x!\r\n", read_id);

    return read_id;
}

static int lcd_panel_display_on(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;

	if(NULL == self){
		bbu_printf("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		bbu_printf("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_write_cmd(spi_ctx, 0x11, 8);
	mdelay(120);
	spi_write_cmd(spi_ctx, 0x29, 8);
	mdelay(20);
    return 0;
}

static int lcd_panel_display_off(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;

	if(NULL == self){
		bbu_printf("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		bbu_printf("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_write_cmd(spi_ctx, 0x28, 8);
	mdelay(20);
	spi_write_cmd(spi_ctx, 0x10, 8);
	mdelay(120);
    return 0;
}

static int lcd_panel_invalid(struct panel_spec *self, uint32_t start_x,
	uint32_t start_y,  uint32_t end_x, uint32_t end_y)
{
	struct s_spi_ctx* spi_ctx = NULL;

	if(NULL == self){
		bbu_printf("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		bbu_printf("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_write_cmd(spi_ctx, 0x2A, 8);
	spi_write_data(spi_ctx, ((start_x >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (start_x & 0xFF), 8);
	spi_write_data(spi_ctx, ((end_x >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (end_x & 0xFF), 8);

	spi_write_cmd(spi_ctx, 0x2B, 8);
	spi_write_data(spi_ctx, ((start_y >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (start_y & 0xFF), 8);
	spi_write_data(spi_ctx, ((end_y >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (end_y & 0xFF), 8);

	spi_write_cmd(spi_ctx, 0x2C, 8);
	return 0;
}

static struct panel_operations lcd_gc9305_spi_ops = {
	.panel_interface_init = lcd_panel_interface_init,
	.panel_init = lcd_panel_init,
	.panel_invalid = lcd_panel_invalid,
	.panel_suspend = lcd_panel_display_off,
	.panel_resume = lcd_panel_display_on,
	.panel_readid = lcd_panel_readid,
};

static struct timing_spi lcd_gc9305_spi_timing = {
	.wclk = 5000, /*kHz*/
	.rclk = 5000, /*kHz*/
};

static struct spi_info lcd_gc9305_spi_info = {
	.line_num = 3,
	.interface_id = 1, 
	.data_lane_num = 2,
	.format = SPI_FORMAT_RGB565,
	.device_id = 0,
	.sample_edge = SPI_EDGE_RISING,
	.force_cs = 0,
	.endian = SPI_ENDIAN_MSB,
	.timing = &lcd_gc9305_spi_timing,
};


struct panel_spec lcd_gc9305_spi_spec = {
	.cap = LCD_CAP_NORMAL,
	.width = 240,
	.height = 320,
	.type = LCD_MODE_SPI,
	.te_pol = LCD_POLARITY_POS,
	.info = {
		.spi = &lcd_gc9305_spi_info,
	},
	.panel_if = NULL,
	.ops = &lcd_gc9305_spi_ops,
};

