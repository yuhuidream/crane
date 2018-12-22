#ifndef _LCD_DRV_H_
#define _LCD_DRV_H_

#include "predefines.h"
#include "panel_drv.h"

enum{
	LCD_FORMAT_RGB565,
	LCD_FORMAT_RGB1555,
	LCD_FORMAT_RGB888_PACKED,
	LCD_FORMAT_RGB888_UNPACKED,
	LCD_FORMAT_RGBA888, /*ARGB*/
	LCD_FORMAT_YUV422_PACKED,
	LCD_FORMAT_YUV422_PLANAR,
	LCD_FORMAT_YUV420_PLANAR,
	LCD_FORMAT_RGB888A,/*RGBA*/
	LCD_FORMAT_YUV420_SEMI,
	LCD_FORMAT_LIMIT
};

enum{
	LCD_ALPHA_LAYER,
	LCD_ALPHA_PIXEL,
	LCD_ALPHA_LIMIT
};

enum{
	LCD_LAYER_IMG = 0,
	LCD_LAYER_OSD1,
	LCD_LAYER_OSD2,
	LCD_LAYER_LIMIT
};

enum{
	LCD_ALPHA_MODE_NORMAL,
	LCD_ALPHA_MODE_PRE,
	LCD_ALPHA_MODE_SPECIAL,
	LCD_ALPHA_MODE_LIMIT
};

enum{
	LCD_CK_MODE_Y,
	LCD_CK_MODE_U,
	LCD_CK_MODE_V,
	LCD_CK_MODE_YUV,
	LCD_CK_MODE_RGB,
	LCD_CK_MODE_B,
	LCD_CK_MODE_LUMA,
	LCD_CK_MODE_LIMIT
};

enum{
	LCD_DITHER_4X4,
	LCD_DITHER_4X8,
	LCD_DITHER_LIMIT
};

struct s_color_key{
	uint8_t ck_threshold1;
	uint8_t ck_threshold2;
	uint8_t ck_dest_value;
	uint8_t ck_dest_alpha;
};

struct s_color_key_setting{
	struct s_color_key ck_y;
	struct s_color_key ck_u;
	struct s_color_key ck_v;
	uint16_t reserved;
	uint8_t ck_mode;
	uint8_t ck_en;
};

struct s_cbsh_setting{
	uint16_t brightness;
	uint16_t contrast;
	uint16_t saturation;
	uint16_t multiplier;
	uint16_t hue_sin;
	uint16_t hue_cos;
	uint16_t reserved1;
	uint8_t reserved2;
	uint8_t cbsh_en;
};

struct s_dither_setting{
	uint8_t dither_en;
	uint8_t dither_mode;
	uint16_t reserved;
};

struct s_point{
	uint16_t x;
	uint16_t y;
};

struct s_plane_buffer{
	uint32_t addr;
	uint16_t width;
	uint16_t height;
	uint16_t stride;
	uint16_t reserved;
};

struct s_layer_setting{
	uint8_t layer_en;
	uint8_t format;
	uint8_t alpha_sel;
	uint8_t mirror_en;
	struct s_plane_buffer src_buffer[3];
	struct s_point dest_point;
	struct s_color_key_setting ck_setting;
	struct s_cbsh_setting cbsh_setting;
	uint32_t layer_alpha;
};

struct s_wb_setting{
	uint8_t wb_en;
	uint8_t format;
	uint16_t reserved;
	struct s_plane_buffer dest_buffer;
};

struct s_output_setting{
	uint8_t format;
	uint8_t type;
	uint8_t te_pol;
	uint8_t reserved1;
	uint16_t height;
	uint16_t width;
	uint16_t data_lane_num; /*1 or 2*/
	uint16_t reserved2;
};

struct s_lcd_ctx{
	uint32_t base_addr;
	uint8_t alpha_mode;
	uint8_t gamma_en;
	uint16_t reserved;
	uint32_t bg_color;
	struct s_layer_setting layer_setting[3];
	struct s_wb_setting wb_setting;
	struct s_output_setting output_setting;
	struct s_dither_setting dither_setting;
};

void lcd_poweron(void);
void lcd_poweroff(void);
struct s_lcd_ctx* lcd_init(void);
int lcd_enable_layer(struct s_lcd_ctx *lcd_ctx, uint32_t layer_id);
int lcd_disable_layer(struct s_lcd_ctx *lcd_ctx, uint32_t layer_id);
int lcd_refresh(struct s_lcd_ctx *lcd_ctx);
int lcd_after_refresh(struct s_lcd_ctx *lcd_ctx);
int lcd_output_setting(struct s_lcd_ctx *lcd_ctx, struct panel_spec *panel);
void lcd_uninit(struct s_lcd_ctx *lcd_ctx);

#endif /*_LCD_DRV_H_*/
