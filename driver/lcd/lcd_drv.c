#include "lcd_drv.h"
#include "lcd_reg.h"
#include "lcd_fns.h"
#include "timer_dec.h"
#include "interrupt.h"
#include <string.h>


struct s_lcd_ctx g_lcd_ctx;
int g_frame_done = 1;
int g_wb_done = 1;

extern int g_spi_cmd_done;
extern int g_mcu_cmd_done;

#define DITHER_TB_4X4_INDEX0       (0x6e4ca280)
#define DITHER_TB_4X4_INDEX1       (0x5d7f91b3)
#define DITHER_TB_4X8_INDEX0       (0xb391a280)
#define DITHER_TB_4X8_INDEX1       (0x7f5d6e4c)
#define DITHER_TB_4X8_INDEX2       (0x80a291b3)
#define DITHER_TB_4X8_INDEX3       (0x4c6e5d7f)

void lcd_write(uint32_t reg, uint32_t val)
{
	bbu_printf("lcd_write: 0x%x write 0x%x\r\n", reg, val);
	BU_REG_WRITE(LCD_BASE_ADDR + reg, val);
}

uint32_t lcd_read(uint32_t reg)
{
	return BU_REG_READ(LCD_BASE_ADDR + reg);
}

void lcd_set_bits(uint32_t reg, uint32_t bits)
{
	lcd_write(reg, lcd_read(reg) | bits);
}

void lcd_clear_bits(uint32_t reg, uint32_t bits)
{
	lcd_write(reg, lcd_read(reg) & ~bits);
}

void lcd_write_bits(uint32_t reg, uint32_t value, uint32_t mask, uint32_t shifts)
{
	uint32_t reg_val;
	reg_val = lcd_read(reg);
	reg_val &= ~(mask << shifts);
	reg_val |= (value << shifts);
	lcd_write(reg, reg_val);
}

static void lcd_isr_handler(void *arg)
{
	struct s_lcd_ctx *lcd_ctx = (struct s_lcd_ctx*)arg;
	int reg;

	if(NULL == lcd_ctx){
		bbu_printf("ERROR: lcd_isr_handler, Invalid param!\r\n");
		return;
	}

	reg = lcd_read(IRQ_ISR_RAW);
	bbu_printf("INFO: lcd_isr_handler, irq = 0x%x!\r\n", reg);
	lcd_write(IRQ_ISR_RAW, 0);

	if(0 != (reg & IRQ_MCU_DONE))
		g_mcu_cmd_done = 1;
	if(0 != (reg & IRQ_SPI_DONE))
		g_spi_cmd_done = 1;
	if(0 != (reg & IRQ_FRAME_DONE))
		g_frame_done = 1;
	if(0 != (reg & IRQ_WB_DONE))
		g_wb_done = 1;
}

static void lcd_irq_en(struct s_lcd_ctx *lcd_ctx)
{
	lcd_write(IRQ_ENA, IRQ_FRAME_DONE|IRQ_MCU_DONE|IRQ_SPI_DONE|IRQ_WB_DONE);
	ISR_Connect(LCD_IRQ, lcd_isr_handler, lcd_ctx);
	INT_Enable(LCD_IRQ, 0, 15);	
}

static void lcd_irq_dis(void)
{
	lcd_write(IRQ_ENA, 0);
	ISR_Disconnect(LCD_IRQ);
	INT_Disable(LCD_IRQ);	
}

static int lcd_check_config(struct s_lcd_ctx *lcd_ctx)
{
	struct s_wb_setting *wb = &lcd_ctx->wb_setting;

	if((wb->format != LCD_FORMAT_RGB565) && (wb->format != LCD_FORMAT_RGB888_PACKED)
		&& (wb->format != LCD_FORMAT_RGB888A) && (wb->format != LCD_FORMAT_RGBA888)){
		bbu_printf("ERROR: lcd_check_config, Invalid wb format (%d)!\r\n", wb->format);
		return -1;
	}

	if(1 == lcd_ctx->dither_setting.dither_en){
		if(lcd_ctx->output_setting.type == LCD_MODE_SPI){
			if((lcd_ctx->output_setting.format == SPI_FORMAT_RGB888) ||
				(lcd_ctx->output_setting.format == SPI_FORMAT_RGB888_2_3)){
				bbu_printf("ERROR: lcd_check_config, Invalid dither setting\r\n");
				return -1;
			}
		}
	}
	//TO BE DEFINED
	return 0;
}

static void lcd_config_OSD2(struct s_lcd_ctx *lcd_ctx)
{
	struct s_layer_setting *layer = &(lcd_ctx->layer_setting[LCD_LAYER_OSD2]);
	int reg = 0;
	struct s_color_key *ck = NULL;

	if(0 == layer->layer_en){
		lcd_write(TV_CTRL0, 0);
		return;
	}

	lcd_write(TVG_START_ADDR0, layer->src_buffer[0].addr);
	lcd_write(TVG_PITCH, layer->src_buffer[0].stride);
	lcd_write(TVG_OVSA_HPXL_VLN, layer->dest_point.x|(layer->dest_point.y<<16));
	reg = (layer->src_buffer[0].height<<16) | layer->src_buffer[0].width;
	lcd_write(TVG_HPXL_VLN, reg);
	lcd_write(TVGZM_HPXL_VLN, reg);

	switch(layer->format){
	case LCD_FORMAT_RGB565:
		reg = 0 << 16 | BIT_23;
		break;
	case LCD_FORMAT_RGB1555:
		reg = 1 << 16 | BIT_23;
		break;
	case LCD_FORMAT_RGB888_PACKED:
		reg = 2 << 16;
		break;
	case LCD_FORMAT_RGB888_UNPACKED:
		reg = 3 << 16;
		break;
	case LCD_FORMAT_RGBA888:
		reg = 4 << 16;
		break;
	case LCD_FORMAT_YUV422_PACKED:
		reg = (5 << 16) | BIT_9;
		break;
	case LCD_FORMAT_RGB888A:
		reg = 0xB << 16;
		break;
	default:
		bbu_printf("Warning: lcd_config_OSD2, Invalid layer format (%d)!\r\n", layer->format);
		reg = 0 << 16;
	}

	if(0 == is_format_yuv(layer->format))
		reg |= BIT_12; /*RB swap as default*/
	reg |= BIT_8; /*layer enable*/
	lcd_write(TV_CTRL0, reg);

	if(layer->alpha_sel == LCD_ALPHA_LAYER){
		lcd_clear_bits(TV_CTRL1, BIT_16);
		lcd_write_bits(TV_CTRL1, layer->layer_alpha, MASK8, 8);
	} else {
		lcd_set_bits(TV_CTRL1, BIT_16);
	}

	if(layer->ck_setting.ck_en){
		ck = &layer->ck_setting.ck_y;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(TV_COLORKEY_Y, reg);

		ck = &layer->ck_setting.ck_u;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(TV_COLORKEY_U, reg);

		ck = &layer->ck_setting.ck_v;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(TV_COLORKEY_V, reg);

		switch(layer->ck_setting.ck_mode){
		case LCD_CK_MODE_Y:
			reg = 1;
			break;
		case LCD_CK_MODE_U:
			reg = 2;
			break;
		case LCD_CK_MODE_V:
			reg = 4;
			break;
		case LCD_CK_MODE_YUV:
			reg = 5;
			break;
		case LCD_CK_MODE_RGB:
			reg = 3;
			break;
		case LCD_CK_MODE_B:
			reg = 7;
			break;
		case LCD_CK_MODE_LUMA:
			reg = 6;
			break;
		default:
			reg = 0;
		}
		lcd_write_bits(TV_CTRL1, reg, MASK3, 24);
		lcd_set_bits(TV_CTRL1, BIT_21);
	} else {
		lcd_clear_bits(TV_CTRL1, BIT_21);
	}

	if(layer->cbsh_setting.cbsh_en){
		lcd_write(TV_CONTRAST, (layer->cbsh_setting.brightness << 16) |
			layer->cbsh_setting.contrast);
		lcd_write(TV_SATURATION, (layer->cbsh_setting.multiplier << 16) |
			layer->cbsh_setting.saturation);
		lcd_write(TV_CBSH_HUE, (layer->cbsh_setting.hue_sin << 16) |
			layer->cbsh_setting.hue_cos);
		lcd_set_bits(TV_CTRL0, BIT_29);
	} else {
		lcd_clear_bits(TV_CTRL0, BIT_29);
	}

	if(layer->mirror_en){
		lcd_set_bits(IOPAD_CONTROL, BIT_19);
	} else {
		lcd_clear_bits(IOPAD_CONTROL, BIT_19);
	}
}

static void lcd_config_OSD1(struct s_lcd_ctx *lcd_ctx)
{
	struct s_layer_setting *layer = &(lcd_ctx->layer_setting[LCD_LAYER_OSD1]);
	int reg = 0;
	struct s_color_key *ck = NULL;

	if(0 == layer->layer_en){
		lcd_clear_bits(PN_CTRL0, BIT_8);
		return;
	}

	lcd_write(GRA_START_ADDR0, layer->src_buffer[0].addr);
	lcd_write(GRA_PITCH, layer->src_buffer[0].stride);
	lcd_write(GRA_OVSA_HPXL_VLN, layer->dest_point.x|(layer->dest_point.y<<16));
	reg = (layer->src_buffer[0].height<<16) | layer->src_buffer[0].width;
	lcd_write(GRA_HPXL_VLN, reg);
	lcd_write(GRAZM_HPXL_VLN, reg);

	switch(layer->format){
	case LCD_FORMAT_RGB565:
		lcd_write_bits(PN_CTRL0, 0, MASK4, 16);
		lcd_set_bits(PN_CTRL1, BIT_23);
		break;
	case LCD_FORMAT_RGB1555:
		lcd_write_bits(PN_CTRL0, 1, MASK4, 16);
		lcd_set_bits(PN_CTRL1, BIT_23);
		break;
	case LCD_FORMAT_RGB888_PACKED:
		lcd_write_bits(PN_CTRL0, 2, MASK4, 16);
		break;
	case LCD_FORMAT_RGB888_UNPACKED:
		lcd_write_bits(PN_CTRL0, 3, MASK4, 16);
		break;
	case LCD_FORMAT_RGBA888:
		lcd_write_bits(PN_CTRL0, 4, MASK4, 16);
		break;
	case LCD_FORMAT_YUV422_PACKED:
		lcd_write_bits(PN_CTRL0, 5, MASK4, 16);
		lcd_set_bits(PN_CTRL0, BIT_1);
		break;
	case LCD_FORMAT_RGB888A:
		lcd_write_bits(PN_CTRL0, 0xB, MASK4, 16);
		break;
	default:
		bbu_printf("Warning: lcd_config_OSD1, Invalid layer format (%d)!\r\n", layer->format);
		lcd_write_bits(PN_CTRL0, 0x0, MASK4, 16);
	}

	if(1 == is_format_yuv(layer->format))
		lcd_set_bits(PN_CTRL0, BIT_8);/*layer enable*/
	else
		lcd_set_bits(PN_CTRL0, BIT_8 | BIT_12);/*layer enable, RB swap as default*/

	if(layer->alpha_sel == LCD_ALPHA_LAYER){
		lcd_clear_bits(PN_CTRL1, BIT_16);
		lcd_write_bits(PN_CTRL1, layer->layer_alpha, MASK8, 8);
	} else {
		lcd_set_bits(PN_CTRL1, BIT_16);
	}

	if(layer->ck_setting.ck_en){
		ck = &layer->ck_setting.ck_y;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(PN_COLORKEY_Y, reg);

		ck = &layer->ck_setting.ck_u;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(PN_COLORKEY_U, reg);

		ck = &layer->ck_setting.ck_v;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(PN_COLORKEY_V, reg);

		switch(layer->ck_setting.ck_mode){
		case LCD_CK_MODE_Y:
			reg = 1;
			break;
		case LCD_CK_MODE_U:
			reg = 2;
			break;
		case LCD_CK_MODE_V:
			reg = 4;
			break;
		case LCD_CK_MODE_YUV:
			reg = 5;
			break;
		case LCD_CK_MODE_RGB:
			reg = 3;
			break;
		case LCD_CK_MODE_B:
			reg = 7;
			break;
		case LCD_CK_MODE_LUMA:
			reg = 6;
			break;
		default:
			reg = 0;
		}
		lcd_write_bits(PN_CTRL1, reg, MASK3, 24);
		lcd_set_bits(TV_CTRL1, BIT_19);
	} else {
		lcd_clear_bits(TV_CTRL1, BIT_19);
	}

	if(layer->cbsh_setting.cbsh_en){
		lcd_write(PN_CONTRAST, (layer->cbsh_setting.brightness << 16) |
			layer->cbsh_setting.contrast);
		lcd_write(PN_SATURATION, (layer->cbsh_setting.multiplier << 16) |
			layer->cbsh_setting.saturation);
		lcd_write(PN_CBSH_HUE, (layer->cbsh_setting.hue_sin << 16) |
			layer->cbsh_setting.hue_cos);
		lcd_set_bits(PN_CTRL0, BIT_29);
	} else {
		lcd_clear_bits(PN_CTRL0, BIT_29);
	}

	if(layer->mirror_en){
		lcd_set_bits(IOPAD_CONTROL, BIT_15);
	} else {
		lcd_clear_bits(IOPAD_CONTROL, BIT_15);
	}
}

static void lcd_config_img(struct s_lcd_ctx *lcd_ctx)
{
	struct s_layer_setting *layer = &(lcd_ctx->layer_setting[LCD_LAYER_IMG]);
	int reg = 0;
	int plane = 0;

	if(0 == layer->layer_en){
		lcd_clear_bits(PN_CTRL0, BIT_0);
		return;
	}

	lcd_write(DMA_OVSA_HPXL_VLN, layer->dest_point.x|(layer->dest_point.y<<16));
	reg = (layer->src_buffer[0].height<<16) | layer->src_buffer[0].width;
	lcd_write(DMA_HPXL_VLN, reg);
	lcd_write(DMAZM_HPXL_VLN, reg);

	lcd_write(DMA_START_ADDR_Y0, layer->src_buffer[0].addr);
	lcd_write(DMA_PITCH_YC, layer->src_buffer[0].stride);

	plane = get_format_plane(layer->format);
	bbu_printf("layer plane is %d\r\n", plane);
	if(3 == plane){ /*v plane*/
		lcd_write(DMA_START_ADDR_V0, layer->src_buffer[2].addr);
		lcd_write(DMA_START_ADDR_U0, layer->src_buffer[1].addr);
		lcd_write(DMA_PITCH_UV, layer->src_buffer[2].stride << 16 |
			layer->src_buffer[1].stride);
	}
	if(plane == 2){ /*u plane*/
		lcd_write(DMA_START_ADDR_U0, layer->src_buffer[1].addr);
		lcd_write(DMA_PITCH_UV, layer->src_buffer[1].stride);
	}

	switch(layer->format){
	case LCD_FORMAT_RGB565:
		lcd_write_bits(PN_CTRL0, 0, MASK4, 20);
		lcd_set_bits(PN_CTRL1, BIT_23);
		break;
	case LCD_FORMAT_RGB1555:
		lcd_write_bits(PN_CTRL0, 1, MASK4, 20);
		lcd_set_bits(PN_CTRL1, BIT_23);
		break;
	case LCD_FORMAT_RGB888_PACKED:
		lcd_write_bits(PN_CTRL0, 2, MASK4, 20);
		break;
	case LCD_FORMAT_RGB888_UNPACKED:
		lcd_write_bits(PN_CTRL0, 3, MASK4, 20);
		break;
	case LCD_FORMAT_RGBA888:
		lcd_write_bits(PN_CTRL0, 4, MASK4, 20);
		break;
	case LCD_FORMAT_YUV422_PACKED:
		lcd_write_bits(PN_CTRL0, 5, MASK4, 20);
		lcd_set_bits(PN_CTRL0, BIT_1);
		break;
	case LCD_FORMAT_YUV422_PLANAR:
		lcd_write_bits(PN_CTRL0, 6, MASK4, 20);
		lcd_set_bits(PN_CTRL0, BIT_1);
		break;
	case LCD_FORMAT_YUV420_PLANAR:
		lcd_write_bits(PN_CTRL0, 7, MASK4, 20);
		lcd_set_bits(PN_CTRL0, BIT_1);
		break;
	case LCD_FORMAT_RGB888A:
		lcd_write_bits(PN_CTRL0, 0xB, MASK4, 20);
		break;
	case LCD_FORMAT_YUV420_SEMI:
		lcd_write_bits(PN_CTRL0, 0xC, MASK4, 20);
		lcd_set_bits(PN_CTRL0, BIT_1);
		break;
	default:
		bbu_printf("Warning: lcd_config_OSD2, Invalid layer format (%d)!\r\n", layer->format);
		reg = 0 << 16;
	}

	if(1 == is_format_yuv(layer->format))
		lcd_set_bits(PN_CTRL0, BIT_0); /*enable layer*/
	else
		lcd_set_bits(PN_CTRL0, BIT_0 | BIT_4); /*enable layer, RB swap as default*/

	if(layer->cbsh_setting.cbsh_en){
		lcd_write(PN_CONTRAST, (layer->cbsh_setting.brightness << 16) |
			layer->cbsh_setting.contrast);
		lcd_write(PN_SATURATION, (layer->cbsh_setting.multiplier << 16) |
			layer->cbsh_setting.saturation);
		lcd_write(PN_CBSH_HUE, (layer->cbsh_setting.hue_sin << 16) |
			layer->cbsh_setting.hue_cos);
		lcd_set_bits(PN_CTRL0, BIT_29);
	} else {
		lcd_clear_bits(PN_CTRL0, BIT_29);
	}

	if(layer->mirror_en){
		lcd_set_bits(IOPAD_CONTROL, BIT_13);
	} else {
		lcd_clear_bits(IOPAD_CONTROL, BIT_13);
	}
}

static void lcd_config_wb(struct s_lcd_ctx *lcd_ctx)
{
	struct s_wb_setting *wb = &lcd_ctx->wb_setting;
	int reg;

	if(0 == wb->wb_en){
		lcd_write(WDMA_CTRL1, 0);
		return;
	}

	reg = (wb->dest_buffer.stride << 16) | (16 << 8) | BIT_0;
	switch(wb->format){
	case LCD_FORMAT_RGB565:
		reg |= (0 << 4);
		lcd_write_bits(SMPN_CTRL, 5, MASK4, 8);
		break;
	case LCD_FORMAT_RGB888_PACKED:
		reg |= (1 << 4);
		lcd_write_bits(SMPN_CTRL, 3, MASK4, 8);
		break;
	case LCD_FORMAT_RGB888A:
		reg |= (3 << 4);
		lcd_write_bits(SMPN_CTRL, 3, MASK4, 8);
		break;
	case LCD_FORMAT_RGBA888:
		reg |= (2 << 4);
		lcd_write_bits(SMPN_CTRL, 3, MASK4, 8);
		break;
	default:
		bbu_printf("ERROR: lcd_config_wb: Unsupported format 0x%x\r\n", wb->format);
		return;
	}
	lcd_write(WDMA_CTRL1, reg);
	reg = (wb->dest_buffer.height << 16) | wb->dest_buffer.width;
	lcd_write(WDMA_CTRL2, reg);
	lcd_write(WDMA_CTRL3, wb->dest_buffer.addr);
	lcd_write(WDMA_CTRL4, 0x84440);
}

static void lcd_config_gamma(struct s_lcd_ctx *lcd_ctx)
{
	int i,j;
	uint32_t reg;

	bbu_printf("INFO: lcd_config_gamma +++\r\n");
	if(0 == lcd_ctx->gamma_en){
		lcd_clear_bits(PN_CTRL0, BIT_30);
		return;
	}

	for(i=0;i<3;i++){
		for(j=0;j<256;j++){
			lcd_write(SRAM_WRDAT, 255 - j);
			reg = BIT_15 | (i << 8) | j;
			lcd_write(SRAM_CTRL, reg);
		}
	}

	lcd_set_bits(PN_CTRL0, BIT_30);
}

static void lcd_config_dither(struct s_lcd_ctx *lcd_ctx)
{
	uint32_t reg;
	int is_rgb565 = 0;
	struct s_dither_setting *dither = &lcd_ctx->dither_setting;
	struct s_output_setting *output = &lcd_ctx->output_setting;

	bbu_printf("INFO: lcd_config_dither +++\r\n");
	if(0 == dither->dither_en){
		lcd_clear_bits(DITHER_CTRL, BIT_0);
		return;
	}

	if(output->type == LCD_MODE_MCU){
		if(output->format == MCU_FORMAT_RGB565)
			is_rgb565 = 1;
	} else {
		if(output->format == SPI_FORMAT_RGB565)
			is_rgb565 = 1;
	}

	if(dither->dither_mode == LCD_DITHER_4X8){
		reg = (0 << 16) | BIT_1 | BIT_0;
		if(1 == is_rgb565)
			reg |= BIT_4;
		else
			reg |= BIT_5;
		lcd_write(DITHER_CTRL, reg);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X8_INDEX0);
	
		lcd_write_bits(DITHER_CTRL, 1, MASK2, 16);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X8_INDEX1);
		lcd_write_bits(DITHER_CTRL, 2, MASK2, 16);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X8_INDEX2);
		lcd_write_bits(DITHER_CTRL, 3, MASK2, 16);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X8_INDEX3);
	} else {
		reg = (0 << 16) | BIT_0;
		if(1 == is_rgb565)
			reg |= BIT_4;
		else
			reg |= BIT_5;
		lcd_write(DITHER_CTRL, reg);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X4_INDEX0);
		lcd_write_bits(DITHER_CTRL, 1, MASK2, 16);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X4_INDEX1);
	}
}

static void lcd_config_output(struct s_lcd_ctx *lcd_ctx)
{
	int reg = 0;
	struct s_output_setting *output = &lcd_ctx->output_setting;

	bbu_printf("INFO: lcd_config_output +++\r\n");

	reg = ((output->height)<<16) | output->width;
	lcd_write(PN_V_H_ACTIVE, reg);
	lcd_write(TOP_CTRL, 0x400000);
	reg = lcd_read(PN_CTRL1);
	reg &= ~BIT_31;
	if(lcd_ctx->wb_setting.wb_en)
		reg |= (BIT_28 | BIT_29 | BIT_30);
	else
		reg &= ~(BIT_28 | BIT_29 | BIT_30);
	if(output->te_pol == LCD_POLARITY_POS)
		reg &= ~BIT_27;
	else
		reg |= BIT_27;
	reg |= 0x81;
	lcd_write(PN_CTRL1, reg);
	lcd_set_bits(PN_CTRL0, BIT_27);

	if(0 == lcd_ctx->wb_setting.wb_en){
		if(output->type == LCD_MODE_SPI){
			switch(output->format){
			case SPI_FORMAT_RGB565:
				if(output->data_lane_num == 2)
					lcd_write_bits(SMPN_CTRL, 5, MASK4, 8);
				else
					lcd_write_bits(SMPN_CTRL, 2, MASK4, 8);
				break;
			case SPI_FORMAT_RGB666:
				if(output->data_lane_num == 2)
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
			}

		} else {
			if(output->format == MCU_FORMAT_RGB565){
				lcd_write_bits(SMPN_CTRL, 2, MASK4, 8);
			} else {
				lcd_write_bits(SMPN_CTRL, 1, MASK4, 8);
			}
		}
	}
	bbu_printf("INFO: lcd_config_output ---\r\n");	
}

static void lcd_start(int wb)
{
	int *condition = NULL;
	if(1 == wb)
		condition = &g_wb_done;
	else
		condition = &g_frame_done;
	*condition = 0;
	lcd_set_bits(PN_CTRL1, BIT_31);
	while(*condition != 1){
		mdelay(20);
		bbu_printf("Info: lcd_start, wait to finish\r\n");
	}
	lcd_clear_bits(PN_CTRL1, BIT_31);
}

struct s_lcd_ctx* lcd_init(void)
{
	struct s_lcd_ctx *lcd_ctx = &g_lcd_ctx;

	memset(lcd_ctx, 0, sizeof(struct s_lcd_ctx));

	lcd_ctx->base_addr = LCD_BASE_ADDR;

	lcd_irq_en(lcd_ctx);
	return lcd_ctx;
}

int lcd_enable_layer(struct s_lcd_ctx *lcd_ctx, uint32_t layer_id)
{
	if(NULL == lcd_ctx){
		bbu_printf("ERROR: lcd_enable_layer, Invalid param!\r\n");
		return -1;
	}

	if(layer_id >= LCD_LAYER_LIMIT){
		bbu_printf("ERROR: lcd_enable_layer, Invalid layer_id!\r\n");
		return -1;
	}

	lcd_ctx->layer_setting[layer_id].layer_en = 1;
	return 0;
}

int lcd_disable_layer(struct s_lcd_ctx *lcd_ctx, uint32_t layer_id)
{
	if(NULL == lcd_ctx){
		bbu_printf("ERROR: lcd_enable_layer, Invalid param!\r\n");
		return -1;
	}

	if(layer_id >= LCD_LAYER_LIMIT){
		bbu_printf("ERROR: lcd_enable_layer, Invalid layer_id!\r\n");
		return -1;
	}

	lcd_ctx->layer_setting[layer_id].layer_en = 0;
	return 0;
}

int lcd_refresh(struct s_lcd_ctx *lcd_ctx)
{
	int ret;
	bbu_printf("INFO: lcd_refresh +++\r\n ");

	if(NULL == lcd_ctx){
		bbu_printf("ERROR: lcd_refresh, Invalid param!\r\n");
		return -1;
	}

	ret = lcd_check_config(lcd_ctx);
	if(0 != ret){
		bbu_printf("ERROR: lcd_refresh, Invalid config!\r\n");
		return -1;
	}

	lcd_config_OSD2(lcd_ctx);
	lcd_config_OSD1(lcd_ctx);
	lcd_config_img(lcd_ctx);
	lcd_config_wb(lcd_ctx);
	lcd_config_output(lcd_ctx);
	lcd_config_gamma(lcd_ctx);
	lcd_config_dither(lcd_ctx);

	if((lcd_ctx->layer_setting[LCD_LAYER_IMG].layer_en == 0) &&
		(lcd_ctx->layer_setting[LCD_LAYER_OSD1].layer_en == 0) &&
		(lcd_ctx->layer_setting[LCD_LAYER_OSD2].layer_en == 0)){
		bbu_printf("INFO: set force bg color\r\n ");
		lcd_set_bits(PN_CTRL0, BIT_24);
		//lcd_write(TVG_PITCH, 0x20);
	} else {
		bbu_printf("INFO: clean force bg color\r\n ");
		lcd_clear_bits(PN_CTRL0, BIT_24);
	}

	lcd_write(PN_BLANKCOLOR, lcd_ctx->bg_color);
	switch(lcd_ctx->alpha_mode){
	case LCD_ALPHA_MODE_NORMAL:
		lcd_write(AFA_ALL2ONE, 0);
		break;
	case LCD_ALPHA_MODE_PRE:
		lcd_write(AFA_ALL2ONE, 1);
		break;
	case LCD_ALPHA_MODE_SPECIAL:
		lcd_write(AFA_ALL2ONE, 2);
		break;
	}
	if(lcd_ctx->wb_setting.wb_en)
		lcd_start(1);
	else
		lcd_start(0);
	bbu_printf("INFO: lcd_refresh ---\r\n ");
	return 0;
}

int lcd_after_refresh(struct s_lcd_ctx *lcd_ctx)
{
	bbu_printf("INFO: lcd_after_refresh +++\r\n");

	if(NULL == lcd_ctx){
		bbu_printf("ERROR: lcd_after_refresh, Invalid param!\r\n");
		return -1;
	}
	lcd_ctx->layer_setting[LCD_LAYER_IMG].layer_en = 0;
	lcd_ctx->layer_setting[LCD_LAYER_OSD1].layer_en = 0;
	lcd_ctx->layer_setting[LCD_LAYER_OSD2].layer_en = 0;

	lcd_write(TV_CTRL0, 0);
	lcd_clear_bits(PN_CTRL0, BIT_8);
	lcd_clear_bits(PN_CTRL0, BIT_0);
	return 0;
}

int lcd_output_setting(struct s_lcd_ctx *lcd_ctx, struct panel_spec *panel)
{
	struct s_output_setting *output = NULL;

	bbu_printf("INFO: lcd_output_setting +++\r\n");

	if((NULL == lcd_ctx) || (NULL == panel)){
		bbu_printf("ERROR: lcd_output_setting, Invalid param!\r\n");
		return -1;
	}

	output = &lcd_ctx->output_setting;
	output->height = panel->height;
	output->width = panel->width;
	output->type = panel->type;
	output->te_pol = panel->te_pol;
	if(output->type == LCD_MODE_SPI){
		output->format = panel->info.spi->format;		
		output->data_lane_num = panel->info.spi->data_lane_num;
	} else {
		output->format = panel->info.mcu->format;
	}
	bbu_printf("INFO: lcd_output_setting ---\r\n");
	return 0;
}


void lcd_uninit(struct s_lcd_ctx *lcd_ctx)
{
	if(NULL == lcd_ctx){
		bbu_printf("ERROR: lcd_uninit, Invalid param!\r\n");
		return;
	}

	lcd_irq_dis();
}

