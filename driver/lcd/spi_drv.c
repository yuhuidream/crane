#include "lcd_reg.h"
#include "spi_drv.h"

#include "timer_dec.h"

struct s_spi_ctx g_spi_ctx;
int g_spi_cmd_done = 1;

static int set_spi_clk(uint32_t src_clk, uint32_t spi_clk)
{
	uint32_t dividor;

	dividor = src_clk/spi_clk;
	if(dividor > 0xFF){
		bbu_printf("ERROR: src_clk/clk can't large than 0xFF\r\n");
		return -1;
	}
	lcd_write_bits(SPI_CTRL, dividor, MASK8, 24);

	bbu_printf("Expect spi_clk = %d KHz, Real spi_clk = %d KHz\r\n",
		spi_clk, src_clk/dividor);
	
	return 0;
}

static void set_spi_path(struct s_spi_ctx* spi_ctx, uint32_t path)
{
	if(path == SPI_PATH_IMAGE)
		lcd_set_bits(MISC_CTRL, BIT_0);
	else
		lcd_clear_bits(MISC_CTRL, BIT_0);
	spi_ctx->cur_path = path;
}

static void trigger_spi(void)
{
	g_spi_cmd_done = 0;
	lcd_set_bits(SPI_CTRL, BIT_0);
	while(g_spi_cmd_done != 1){
		udelay(10);
		bbu_printf("Info: trigger_spi, wait to finish\r\n");
	}
	lcd_clear_bits(SPI_CTRL, BIT_0);
}

struct s_spi_ctx* spi_init(uint32_t sclk, struct spi_info *info)
{
	int ret;
	int dividor;
	struct s_spi_ctx *spi_ctx = &g_spi_ctx;
	int reg = 0;

	if(info == NULL || info->timing == NULL){
		bbu_printf("ERROR: spi_init, Invalid param!\r\n");
		return NULL;
	}

	if((info->format != SPI_FORMAT_RGB565) &&
		(info->format != SPI_FORMAT_RGB666) && (info->data_lane_num == 1)){
		bbu_printf("ERROR: spi_init, Format error!\r\n");
		return NULL;
	}

	dividor = sclk/info->timing->rclk;
	if((dividor > 0xFF) || (dividor < 2)){
		bbu_printf("ERROR: spi_init, Invalid read timing!\r\n");
		return NULL;
	}

	dividor = sclk/info->timing->wclk;
	if((dividor > 0xFF) || (dividor < 2)){
		bbu_printf("ERROR: spi_init, Invalid write timing!\r\n");
		return NULL;
	}

	memset(spi_ctx, 0, sizeof(struct s_spi_ctx));

	spi_ctx->base_addr = LCD_BASE_ADDR;
	spi_ctx->sclk = sclk;
	memcpy(&spi_ctx->info, info, sizeof(struct spi_info));

	if(info->interface_id == 1)
		reg |= BIT_1;

	if(info->device_id == 1)
		reg |= BIT_2;

	if(info->endian == SPI_ENDIAN_LSB)
		reg |= BIT_5 | BIT_4;

	if(info->sample_edge == SPI_EDGE_FALLING)
		reg |= BIT_7;

	/*enable spi*/
	reg |= BIT_3;
	lcd_write(SPI_CTRL, reg);

	reg = 0;
	if(info->line_num == 3)
		reg |= BIT_3;

	if(info->data_lane_num == 2)
		reg |= BIT_2;

	reg |= BIT_1; /*should be set, otherwith, color will error*/

	if(info->format == SPI_FORMAT_RGB666_2_3)
		reg |= BIT_5;
	else if(info->format == SPI_FORMAT_RGB888_2_3)
		reg |= BIT_4;
	lcd_write(MISC_CTRL, reg);

	/*spi need set bit13, otherwith will lost some data*/
	/*spi need set bit6, otherwith color will error*/
	/*spi need open mcu, otherwidth can't send image data*/
	//lcd_set_bits(SMPN_CTRL, BIT_13 | BIT_6 | BIT_0);
	lcd_set_bits(SMPN_CTRL, BIT_13 | BIT_0);

	switch(info->format){
	case SPI_FORMAT_RGB565:
		if(info->data_lane_num == 2)
			lcd_write_bits(SMPN_CTRL, 5, MASK4, 8);
		else
			lcd_write_bits(SMPN_CTRL, 2, MASK4, 8);
		break;
	case SPI_FORMAT_RGB666:
		if(info->data_lane_num == 2)
			lcd_write_bits(SMPN_CTRL, 4, MASK4, 8);
		else
			lcd_write_bits(SMPN_CTRL, 1, MASK4, 8);
		break;
	case SPI_FORMAT_RGB666_2_3:
		lcd_write_bits(SMPN_CTRL, 6, MASK4, 8);
		break;
	case SPI_FORMAT_RGB888:
		lcd_write_bits(SMPN_CTRL, 3, MASK4, 8);
		break;
	case SPI_FORMAT_RGB888_2_3:
		lcd_write_bits(SMPN_CTRL, 0, MASK4, 8);
		break;
	default:
		bbu_printf("ERROR: spi_init, Invalid format!\r\n");
		return NULL;
	}
	
 	/*set write clk as default*/
	ret = set_spi_clk(sclk, info->timing->wclk);
	if(-1 == ret){
		bbu_printf("ERROR: spi_init, set spi clk error!\r\n");
		return NULL;
	}

	/*set register path as default*/
	set_spi_path(spi_ctx, SPI_PATH_REGISTER);
	spi_ctx->status = SPI_STATUS_INIT;
	return spi_ctx;
}

int spi_set_cs(struct s_spi_ctx *spi_ctx, uint32_t enable)
{
	if(NULL == spi_ctx){
		bbu_printf("ERROR: spi_set_cs, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		bbu_printf("ERROR: spi_set_cs, Invalid mcu status\r\n");
		return -1;
	}

	if(spi_ctx->info.force_cs == 0){
		bbu_printf("ERROR: spi_set_cs, Not force CS mode, can't set CS!\r\n");
		return -1;
	}

	if(enable)
		lcd_set_bits(SPI_CTRL, BIT_6);
	else
		lcd_clear_bits(SPI_CTRL, BIT_6);
	spi_ctx->cur_cs = enable;
	return 0;
}

int spi_write_cmd(struct s_spi_ctx *spi_ctx, uint32_t cmd, uint32_t bits)
{
	uint32_t wcmd, wbits;

	if(NULL == spi_ctx || bits > 32 || 0 == bits){
		bbu_printf("ERROR: spi_write_cmd, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		bbu_printf("ERROR: spi_write_cmd, Invalid mcu status\r\n");
		return -1;
	}

	if((spi_ctx->info.force_cs == 1) && (spi_ctx->cur_cs == 0)){
		bbu_printf("ERROR: spi_write_cmd, Invalid CS status\r\n");
		return -1;
	}

	if(spi_ctx->cur_path == SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);

	if(3 == spi_ctx->info.line_num){/*3 line mode*/
		if(bits == 32){
			bbu_printf("ERROR: spi_write_cmd, too many write bits for 3 line mode!\r\n");
			return -1;
		}
		wbits = bits + 1;
		wcmd = cmd;/*0 -command, 1-data*/
	} else { /*4 line mode*/
		wbits = bits;
		wcmd = cmd;
	}

	lcd_write(SPI_TXDATA, wcmd);
	lcd_write_bits(SPI_CTRL, wbits - 1, MASK16, 8);
	trigger_spi();
	return 0;
}

int spi_write_data(struct s_spi_ctx *spi_ctx, uint32_t data, uint32_t bits)
{
	uint32_t wdata, wbits;

	if(NULL == spi_ctx || bits > 32 || 0 == bits){
		bbu_printf("ERROR: spi_write_data, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		bbu_printf("ERROR: spi_write_data, Invalid mcu status\r\n");
		return -1;
	}

	if((spi_ctx->info.force_cs == 1) && (spi_ctx->cur_cs == 0)){
		bbu_printf("ERROR: spi_write_data, Invalid CS status\r\n");
		return -1;
	}

	if(spi_ctx->cur_path == SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);

	if(3 == spi_ctx->info.line_num){
		if(bits == 32){
			bbu_printf("ERROR: spi_write_data, too many write bits for 3 line mode!\r\n");
			return -1;
		}
		wbits = bits + 1;
		wdata = (1 << bits) | data;/*0 -command, 1-data*/
	} else { /*4 line mode*/
		wbits = bits;
		wdata = data;
	}

	lcd_write(SPI_TXDATA, wdata);
	lcd_write_bits(SPI_CTRL, wbits - 1, MASK16, 8);
	trigger_spi();
	return 0;
}

int spi_read_data(struct s_spi_ctx *spi_ctx, uint32_t cmd, uint32_t cmd_bits,
	uint32_t *data,  uint32_t data_bits)
{
	uint32_t wcmd, wbits, rbits;
	int ret;

	if(NULL == spi_ctx || cmd_bits > 32 || data_bits > 32){
		bbu_printf("ERROR: spi_read_data, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		bbu_printf("ERROR: spi_read_data, Invalid mcu status\r\n");
		return -1;
	}

	if((spi_ctx->info.force_cs == 1) && (spi_ctx->cur_cs == 0)){
		bbu_printf("ERROR: spi_read_data, Invalid CS status\r\n");
		return -1;
	}

	ret = set_spi_clk(spi_ctx->sclk, spi_ctx->info.timing->rclk);
	if(-1 == ret){
		bbu_printf("ERROR: spi_read_data, set spi clk error!\r\n");
		return -1;
	}

	if(spi_ctx->cur_path == SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);

	if(data_bits > 8)
		rbits = data_bits;
	else
		rbits = data_bits -1;
		

	if(3 == spi_ctx->info.line_num){
		if(cmd == 32){
			bbu_printf("ERROR: spi_read_data, too many write bits for 3 line mode!\r\n");
			return -1;
		}
		wbits = cmd_bits;
		wcmd = cmd;/*0 -command, 1-data*/
	} else { /*4 line mode*/
		wbits = cmd_bits + 1;
		wcmd = cmd;
	}

	lcd_write(SPI_TXDATA, wcmd);
	lcd_write_bits(SPI_CTRL, wbits, MASK8, 8);
	lcd_write_bits(SPI_CTRL, rbits, MASK8, 16);
	trigger_spi();

	*data = lcd_read(SPI_RXDATA);

 	/*set write clk as default*/
	ret = set_spi_clk(spi_ctx->sclk, spi_ctx->info.timing->wclk);
	if(-1 == ret){
		bbu_printf("ERROR: spi_read_data, set spi clk error!\r\n");
		return -1;
	}
	return 0;
}

int spi_before_refresh(struct s_spi_ctx *spi_ctx)
{
	if(NULL == spi_ctx){
		bbu_printf("ERROR: spi_before_refresh, Invalid param\r\n");
		return -1;
	}
	
	if(SPI_STATUS_INIT != spi_ctx->status){
		bbu_printf("ERROR: spi_before_refresh, Invalid spi status\r\n");
		return -1;
	}

	if(spi_ctx->cur_path != SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_IMAGE);

	if(2 == spi_ctx->info.data_lane_num){
		/*2 data lane*/
		switch(spi_ctx->info.format){
		case SPI_FORMAT_RGB565:
			lcd_write_bits(SPI_CTRL, 15, MASK8, 8);
			break;
		case SPI_FORMAT_RGB666:
			lcd_write_bits(SPI_CTRL, 17, MASK8, 8);
			break;
		case SPI_FORMAT_RGB666_2_3:
			lcd_write_bits(SPI_CTRL, 11, MASK8, 8);
			break;
		case SPI_FORMAT_RGB888:
			lcd_write_bits(SPI_CTRL, 23, MASK8, 8);
			break;
		case SPI_FORMAT_RGB888_2_3:
			lcd_write_bits(SPI_CTRL, 15, MASK8, 8);
			break;
		}
	} else{
		/*1 data lane*/
		lcd_write_bits(SPI_CTRL, 7, MASK8, 8);
	}
	return 0;

}

int spi_after_refresh(struct s_spi_ctx *spi_ctx)
{
	if(NULL == spi_ctx){
		bbu_printf("ERROR: spi_after_refresh, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		bbu_printf("ERROR: spi_after_refresh, Invalid spi status\r\n");
		return -1;
	}

	if(spi_ctx->cur_path != SPI_PATH_REGISTER)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);
	return 0;
}


void spi_uninit(struct s_spi_ctx *spi_ctx)
{
	if(NULL == spi_ctx){
		bbu_printf("ERROR: spi_uninit, Invalid param\r\n");
		return;
	}

	lcd_write(SPI_CTRL, 0);
	lcd_write(MISC_CTRL, 0);
	spi_ctx->status = SPI_STATUS_UNINIT;
}
