#include "lcd_reg.h"
#include "mcu_drv.h"

#include "timer_dec.h"

struct s_mcu_ctx g_mcu_ctx;
int g_mcu_cmd_done = 1;

static void set_mcu_path(struct s_mcu_ctx *mcu_ctx, uint32_t path)
{
	if(path == MCU_PATH_IMAGE)
		lcd_clear_bits(SMPN_CTRL, BIT_4);
	else
		lcd_set_bits(SMPN_CTRL, BIT_4);
	mcu_ctx->cur_path = path;
}

static void trigger_mcu(uint32_t value)
{
	g_mcu_cmd_done = 0;
	lcd_write(SLV_PORT, value);
	while(g_mcu_cmd_done != 1){
		udelay(10);
		bbu_printf("Info: trigger_mcu, wait to finish\r\n");
	}
}

struct s_mcu_ctx* mcu_init(uint32_t sclk, struct mcu_info *info)
{
	struct s_mcu_ctx *mcu_ctx = &g_mcu_ctx;
	uint32_t reg = 0,scycle,rl,rh,wl,wh;

	bbu_printf("INFO: mcu_init+++\r\n");

	if(info == NULL || info->timing == NULL){
		bbu_printf("ERROR: mcu_init, Invalid param\r\n");
		return NULL;
	}

	scycle = 1000000/sclk; /*ns*/
	rl = (info->timing->rlpw + scycle - 1) / scycle - 1;
	rh = (info->timing->rhpw + scycle - 1) / scycle - 1;
	wl = (info->timing->wlpw + scycle - 1) / scycle - 1;
	wh = (info->timing->whpw + scycle - 1) / scycle - 1;
	if((rl > 15) || (rh > 15) || (wl > 15) || (wh > 15)){
		bbu_printf("ERROR: mcu_init, can't get mcu's clk!\r\n");
		return NULL;
	}

	memset(mcu_ctx, 0, sizeof(struct s_mcu_ctx));

	mcu_ctx->base_addr = LCD_BASE_ADDR;
	mcu_ctx->sclk = sclk;
	memcpy(&mcu_ctx->info, info, sizeof(struct mcu_info));

	reg = rl<<28 | rh<<24 | wl<<20 | wh<<16;

	if(info->format == MCU_FORMAT_RGB565){
		reg |= (2 << 8);
	} else {
		reg |= (1 << 8);
	}

	if(info->endian == MCU_ENDIAN_MSB)
		reg |= BIT_6;

	if(info->device_id == 0)
		reg &= ~BIT_5;

	if(info->bus_mode == MCU_BUS_8080)
		reg &= ~BIT_2;

	reg |= BIT_0; /*enable mcu interface*/
	lcd_write(SMPN_CTRL, reg);

	lcd_set_bits(MISC_CTRL, BIT_1); /*should be set, otherwith color will error*/

	lcd_set_bits(PN_CTRL0, BIT_25); /*delay cs rising edge one cycle*/

	/*use register path as default*/
	set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);
	mcu_ctx->status = MCU_STATUS_INIT;
	bbu_printf("INFO: mcu_init---\r\n");
	return mcu_ctx;
}

int mcu_set_cs(struct s_mcu_ctx *mcu_ctx, uint32_t enable)
{
	if(NULL == mcu_ctx){
		bbu_printf("ERROR: mcu_set_cs, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		bbu_printf("ERROR: mcu_set_cs, Invalid mcu status\r\n");
		return -1;
	}

	if(mcu_ctx->info.force_cs == 0){
		bbu_printf("ERROR: mcu_set_cs, Not force CS mode, can't set CS!\r\n");
		return -1;
	}

	if(enable)
		lcd_set_bits(SMPN_CTRL, BIT_1);
	else
		lcd_clear_bits(SMPN_CTRL, BIT_1);
	mcu_ctx->cur_cs = enable;
	return 0;
}

int mcu_write_cmd(struct s_mcu_ctx *mcu_ctx, uint32_t cmd, uint32_t bits)
{
	int reg;
	if(NULL == mcu_ctx){
		bbu_printf("ERROR: mcu_write_cmd, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		bbu_printf("ERROR: mcu_write_cmd, Invalid mcu status\r\n");
		return -1;
	}

	if((mcu_ctx->info.force_cs == 1) && (mcu_ctx->cur_cs == 0)){
		bbu_printf("ERROR: mcu_write_cmd, Invalid CS status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path == MCU_PATH_IMAGE)
		set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);

	switch(bits){
	case 8:
		reg = (cmd & 0xFF) | BIT_8;
		trigger_mcu(reg);
		break;
	case 16:
		reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
		trigger_mcu(reg);
		break;
	case 24:
		reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
		trigger_mcu(reg);
		reg = (cmd & 0xFF0000) | BIT_8;
		trigger_mcu(reg);
		break;
	case 32:
		reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
		trigger_mcu(reg);
		reg = (cmd & 0xFF0000) | BIT_8 | ((cmd & 0xFF000000)<<8) | BIT_24;
		trigger_mcu(reg);
		break;
	default:
		bbu_printf("ERROR: mcu_write_cmd, Invalid bits!\r\n");
		return -1;
	}
	return 0;
}

int mcu_write_data(struct s_mcu_ctx *mcu_ctx, uint32_t data, uint32_t bits)
{
	int reg;
	if(NULL == mcu_ctx){
		bbu_printf("ERROR: mcu_write_data, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		bbu_printf("ERROR: mcu_write_data, Invalid mcu status\r\n");
		return -1;
	}

	if((mcu_ctx->info.force_cs == 1) && (mcu_ctx->cur_cs == 0)){
		bbu_printf("ERROR: mcu_write_cmd, Invalid CS status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path == MCU_PATH_IMAGE)
		set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);

	switch(bits){
	case 8:
		reg = (data & 0xFF) | BIT_8 | BIT_15;
		trigger_mcu(reg);
		break;
	case 16:
		reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
			| BIT_31;
		trigger_mcu(reg);
		break;
	case 24:
		reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
			| BIT_31;
		trigger_mcu(reg);
		reg = (data & 0xFF0000) | BIT_8;
		trigger_mcu(reg);
		break;
	case 32:
		reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
			| BIT_31;
		trigger_mcu(reg);
		reg = (data & 0xFF0000) | BIT_8 | BIT_15 | ((data & 0xFF000000)<<8)
			| BIT_24 | BIT_31;
		trigger_mcu(reg);
		break;
	default:
		bbu_printf("ERROR: mcu_write_data, Invalid bits!\r\n");
		return -1;
	}
	return 0;
}


int mcu_read_data(struct s_mcu_ctx *mcu_ctx, uint32_t cmd, uint32_t cmd_bits,
	uint32_t *data,  uint32_t data_bits)
{
	int reg;
	int readdata;
	if(NULL == mcu_ctx){
		bbu_printf("ERROR: mcu_read_data, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		bbu_printf("ERROR: mcu_read_data, Invalid mcu status\r\n");
		return -1;
	}

	if((mcu_ctx->info.force_cs == 1) && (mcu_ctx->cur_cs == 0)){
		bbu_printf("ERROR: mcu_write_cmd, Invalid CS status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path == MCU_PATH_IMAGE)
		set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);

	switch(cmd_bits){
	case 8:
	case 24:
		if(cmd_bits == 8){
			reg = (cmd & 0xFF) | BIT_8;
		} else {
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg);
			reg = (cmd & 0xFF0000) | BIT_8;
		}
		switch(data_bits){
		case 8:
			reg |= BIT_25 | BIT_31;
			trigger_mcu(reg);//The first read is dummy
			reg = BIT_9 | BIT_15;
			trigger_mcu(reg);
			*data = lcd_read(ISA_RXDATA) & MASK8;
			break;
		case 16:
			reg |= BIT_25 | BIT_31;
			trigger_mcu(reg);//The first read is dummy
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg);
			*data = lcd_read(ISA_RXDATA) & MASK16;
			break;
		case 24:
			reg |= BIT_25 | BIT_31;
			trigger_mcu(reg);//The first read is dummy
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg);
			readdata = lcd_read(ISA_RXDATA) & MASK16;
			reg = BIT_9 | BIT_15;
			trigger_mcu(reg);
			*data = (readdata << 8) | (lcd_read(ISA_RXDATA) & MASK8);
			break;
		case 32:
			reg |= BIT_25 | BIT_31;
			trigger_mcu(reg);//The first read is dummy
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg);
			readdata = lcd_read(ISA_RXDATA) & MASK16;
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg);
			*data = (readdata << 16) | (lcd_read(ISA_RXDATA) & MASK16);
			break;
		default:
			bbu_printf("ERROR: mcu_read_data, Invalid bits!\r\n");
			return -1;
		}
		break;
	case 16:
	case 32:
		if(cmd_bits == 16){
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg);
		} else {
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg);
			reg = (cmd & 0xFF0000) | BIT_8 | ((cmd & 0xFF000000)<<8) | BIT_24;
			trigger_mcu(reg);
		}
		switch(data_bits){
		case 8:
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg);//The first read is dummy
			*data = (lcd_read(ISA_RXDATA) & MASK16) >> 8;
			break;
		case 16:
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg);//The first read is dummy
			readdata = (lcd_read(ISA_RXDATA) & MASK16) >> 8;
			reg = BIT_9 | BIT_15;
			trigger_mcu(reg);
			*data = (readdata << 8)|(lcd_read(ISA_RXDATA) & MASK8);
			break;
		case 24:
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg);//The first read is dummy
			readdata = (lcd_read(ISA_RXDATA) & MASK16) >> 8;
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg);
			*data = (readdata << 16)|(lcd_read(ISA_RXDATA) & MASK16);
			break;
		case 32:
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg);//The first read is dummy
			readdata = (lcd_read(ISA_RXDATA) & MASK16) >> 8;
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg);
			readdata = (readdata << 16)|(lcd_read(ISA_RXDATA) & MASK16);
			reg = BIT_9 | BIT_15;
			trigger_mcu(reg);
			*data = (readdata << 8)|(lcd_read(ISA_RXDATA) & MASK8);
			break;
		default:
			bbu_printf("ERROR: mcu_read_data, Invalid bits!\r\n");
			return -1;
		}
		break;
	default:
		bbu_printf("ERROR: mcu_read_data, Invalid bits!\r\n");
		return -1;
	}
	return 0;
}

int mcu_write_cmd_data(struct s_mcu_ctx *mcu_ctx, uint32_t cmd,
	uint32_t cmd_bits, uint32_t data, uint32_t data_bits)
{
	int reg;
	if(NULL == mcu_ctx){
		bbu_printf("ERROR: mcu_write_cmd_data, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		bbu_printf("ERROR: mcu_write_cmd_data, Invalid mcu status\r\n");
		return -1;
	}

	if((mcu_ctx->info.force_cs == 1) && (mcu_ctx->cur_cs == 0)){
		bbu_printf("ERROR: mcu_write_cmd, Invalid CS status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path == MCU_PATH_IMAGE)
		set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);

	switch(cmd_bits){
	case 8:
	case 24:
		if(cmd_bits == 8){
			reg = (cmd & 0xFF) | BIT_8;
		} else {
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg);
			reg = (cmd & 0xFF0000) | BIT_8;
		}
		switch(data_bits){
		case 8:
			reg |= ((data & 0xFF)<<16) | BIT_24 | BIT_31;
			trigger_mcu(reg);
			break;
		case 16:
			reg |= ((data & 0xFF)<<16) | BIT_24 | BIT_31;
			trigger_mcu(reg);
			reg = ((data & 0xFF00) >> 8) | BIT_8 | BIT_15;
			trigger_mcu(reg);
			break;
		case 24:
			reg |= ((data & 0xFF)<<16) | BIT_24 | BIT_31;
			trigger_mcu(reg);
			reg = ((data & 0xFF00) >> 8) | BIT_8 | BIT_15 |
				((data & 0xFF0000)) | BIT_24 | BIT_31;
			trigger_mcu(reg);
			break;
		case 32:
			reg |= ((data & 0xFF)<<16) | BIT_24 | BIT_31;
			trigger_mcu(reg);
			reg = ((data & 0xFF00) >> 8) | BIT_8 | BIT_15 |
				((data & 0xFF0000)) | BIT_24 | BIT_31;
			trigger_mcu(reg);
			reg = (data & 0xFF000000) | BIT_8 | BIT_15;
			trigger_mcu(reg);
			break;
		default:
			bbu_printf("ERROR: mcu_write_cmd_data, Invalid bits!\r\n");
			return -1;
		}
		break;
	case 16:
	case 32:
		if(cmd_bits == 16){
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg);
		} else {
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg);
			reg = (cmd & 0xFF0000) | BIT_8 | ((cmd & 0xFF000000)<<8) | BIT_24;
			trigger_mcu(reg);
		}
		switch(data_bits){
		case 8:
			reg = (data & 0xFF) | BIT_8 | BIT_15;
			trigger_mcu(reg);
			break;
		case 16:
			reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
				| BIT_31;
			trigger_mcu(reg);
			break;
		case 24:
			reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
				| BIT_31;
			trigger_mcu(reg);
			reg = (data & 0xFF0000) | BIT_8;
			trigger_mcu(reg);
			break;
		case 32:
			reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
				| BIT_31;
			trigger_mcu(reg);
			reg = (data & 0xFF0000) | BIT_8 | BIT_15 | ((data & 0xFF000000)<<8)
				| BIT_24 | BIT_31;
			trigger_mcu(reg);
			break;
		default:
			bbu_printf("ERROR: mcu_write_cmd_data, Invalid bits!\r\n");
			return -1;
		}
		break;
	default:
		bbu_printf("ERROR: mcu_write_cmd_data, Invalid bits!\r\n");
		return -1;
	}
	return 0;
}

int mcu_before_refresh(struct s_mcu_ctx *mcu_ctx)
{
	if(NULL == mcu_ctx){
		bbu_printf("ERROR: mcu_before_refresh, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		bbu_printf("ERROR: mcu_before_refresh, Invalid mcu status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path != MCU_PATH_IMAGE)
		set_mcu_path(mcu_ctx, MCU_PATH_IMAGE);
	return 0;
}

int mcu_after_refresh(struct s_mcu_ctx *mcu_ctx)
{
	if(NULL == mcu_ctx){
		bbu_printf("ERROR: mcu_before_refresh, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		bbu_printf("ERROR: mcu_before_refresh, Invalid mcu status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path != MCU_PATH_REGISTER)
		set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);
	return 0;
}


void mcu_uninit(struct s_mcu_ctx *mcu_ctx)
{
	if(NULL == mcu_ctx){
		bbu_printf("ERROR: mcu_uninit, Invalid param\r\n");
		return;
	}

	lcd_write(SMPN_CTRL, 0);
	mcu_ctx->status = MCU_STATUS_UNINIT;
}

