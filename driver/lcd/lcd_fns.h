#ifndef _LCD_FNS_H_
#define _LCD_FNS_H_

#include "predefines.h"

#define STRIDE_ALIGN 16
#define ALIGN(value, base) (((value) + ((base) - 1)) & ~((base) - 1))

enum image_pattern
{
	IMG_PATTERN_RED = 0,
	IMG_PATTERN_GREEN,
	IMG_PATTERN_BLUE,
	IMG_PATTERN_HCB_5,
	IMG_PATTERN_HCB_2,
	IMG_PATTERN_VCB_5,
	IMG_PATTERN_VCB_2,
	IMG_PATTERN_LINE,
	IMG_PATTERN_GREY_LEVEL,
	IMG_PATTERN_AFBC_RGB,
	IMG_PATTERN_AFBC_YUV,
	IMG_PATTERN_MAX
};

struct s_compose_layer_setting{
	uint32_t addr;
	uint8_t format;
	uint8_t alpha_sel;
	uint8_t layer_en;
	uint8_t layer_alpha;
	uint32_t stride;
};

struct s_compose_setting{
	uint32_t width;
	uint32_t height;
	struct s_compose_layer_setting layer_setting[3];
	uint32_t dest_addr;
	uint32_t dest_stride;
	uint8_t dest_format;
	uint8_t alpha_mode;
	uint16_t reserved;
};

int get_format_plane(uint32_t format);
int is_format_yuv(uint32_t format);
int get_uv_height_dividor(uint32_t format);

int32_t lcd_draw_images(uint32_t addr_p1, uint32_t *addr_p2, uint32_t *addr_p3,
					uint32_t format, uint32_t pattern, uint32_t width,
					uint32_t height, uint32_t *stride_p1, uint32_t *stride_p2,
					uint32_t *stride_p3);
int32_t lcd_compare_buffers(uint32_t src, uint32_t dest, uint32_t width,
		uint32_t height, uint32_t stride_src, uint32_t stride_dest,
		uint32_t format, int log);
int lcd_compare_compose_buffer(struct s_compose_setting *compose_setting, int log);

#endif /*_LCD_FNS_H_*/
