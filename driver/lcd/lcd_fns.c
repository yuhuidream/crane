#include "lcd_fns.h"
#include "lcd_drv.h"
#include <string.h>

#define COMPOSE_TEST

#ifndef COMPOSE_TEST
#define TEMP_BUFFER_ADDR 0xD1296000
#else
#define TEMP_BUFFER_ADDR 0xD12B4000
#endif
int get_format_plane(uint32_t format)
{
	int plane = 1;
	switch(format){
	case LCD_FORMAT_RGB565:
	case LCD_FORMAT_RGB1555:
	case LCD_FORMAT_RGB888_PACKED:
	case LCD_FORMAT_RGB888_UNPACKED:
	case LCD_FORMAT_RGBA888:
	case LCD_FORMAT_RGB888A:
	case LCD_FORMAT_YUV422_PACKED:
		plane = 1;
		break;
	case LCD_FORMAT_YUV422_PLANAR:
	case LCD_FORMAT_YUV420_PLANAR:
		plane = 3;
		break;
	case LCD_FORMAT_YUV420_SEMI:
		plane = 2;
		break;
	}
	return plane;
}

int is_format_yuv(uint32_t format)
{
	int is_yuv = 0;
	switch(format){
	case LCD_FORMAT_RGB565:
	case LCD_FORMAT_RGB1555:
	case LCD_FORMAT_RGB888_PACKED:
	case LCD_FORMAT_RGB888_UNPACKED:
	case LCD_FORMAT_RGBA888:
	case LCD_FORMAT_RGB888A:
		is_yuv = 0;
		break;
	case LCD_FORMAT_YUV422_PACKED:
	case LCD_FORMAT_YUV422_PLANAR:
	case LCD_FORMAT_YUV420_PLANAR:
	case LCD_FORMAT_YUV420_SEMI:
		is_yuv = 1;
		break;
	}
	return is_yuv;
}

int get_uv_height_dividor(uint32_t format)
{
	int dividor = 0;
	switch(format){
	case LCD_FORMAT_RGB565:
	case LCD_FORMAT_RGB1555:
	case LCD_FORMAT_RGB888_PACKED:
	case LCD_FORMAT_RGB888_UNPACKED:
	case LCD_FORMAT_RGBA888:
	case LCD_FORMAT_RGB888A:
		dividor = 0;
		break;
	case LCD_FORMAT_YUV422_PACKED:
	case LCD_FORMAT_YUV422_PLANAR:
		dividor = 1;
		break;
	case LCD_FORMAT_YUV420_PLANAR:
	case LCD_FORMAT_YUV420_SEMI:
		dividor = 2;
		break;
	}
	return dividor;
}

static int get_layer_rgb_data(int position_x, int position_y, int is_first_layer,
	struct s_compose_layer_setting *layer, uint32_t *rgb_data, uint8_t *alpha)
{
	uint32_t data;
	uint8_t al_data;
	uint8_t *psrc8 = NULL;
	uint32_t *psrc32 = NULL;
	
	if(layer->format == LCD_FORMAT_RGB888_PACKED){
		psrc8 = (uint8_t*)(layer->addr + position_y * layer->stride + position_x * 3);
		data = (*psrc8++) & 0xFF; /*b*/
		data = (((*psrc8++) & 0xFF) << 8) | data; /*g*/
		data = (((*psrc8++) & 0xFF) << 16) | data; /*r*/
		if(1 != is_first_layer){
			if(layer->alpha_sel != LCD_ALPHA_LAYER){
				bbu_printf("Error: get_layer_rgb_data, Invalid alpha setting!\r\n");
				return -1;
			}
			al_data = layer->layer_alpha;
		} else {
			al_data = 0;
		}
	} else if(layer->format == LCD_FORMAT_RGB888_UNPACKED){
		psrc32 = (uint32_t*)(layer->addr + position_y * layer->stride + position_x * 4);
		data = (*psrc32++) & 0xFFFFFF;
		if(1 != is_first_layer){
			if(layer->alpha_sel != LCD_ALPHA_LAYER){
				bbu_printf("Error: get_layer_rgb_data, Invalid alpha setting!\r\n");
				return -1;
			}
			al_data = layer->layer_alpha;
		} else {
			al_data = 0;
		}
	} else if(layer->format == LCD_FORMAT_RGBA888){
		psrc32 = (uint32_t*)(layer->addr + position_y * layer->stride + position_x * 4);
		psrc8 = (uint8_t*)(layer->addr + position_y * layer->stride + position_x * 4 + 3);
		data = (*psrc32) & 0xFFFFFF;
		if((1 != is_first_layer) && (layer->alpha_sel == LCD_ALPHA_LAYER)){
			al_data = layer->layer_alpha;
		} else {
			al_data = *psrc8;
		}
	} else if(layer->format == LCD_FORMAT_RGB888A){
		psrc32 = (uint32_t*)(layer->addr + position_y * layer->stride + position_x * 4);
		psrc8 = (uint8_t*)(layer->addr + position_y * layer->stride + position_x * 4);
		data = ((*psrc32++) >> 8) & 0xFFFFFF;
		if((1 != is_first_layer) && (layer->alpha_sel == LCD_ALPHA_LAYER)){
			al_data = layer->layer_alpha;
		} else {
			al_data = *psrc8;
		}
	} else{
		bbu_printf("Error: get_layer_rgb_data, Invalid layer format (0x%x)!\r\n",
			layer->format);
		return -1;
	}
	*rgb_data = data;
	*alpha = al_data;
	return 0;
}

static uint32_t do_compose(uint32_t *src1, uint32_t *src2, uint8_t alpha1,
	uint8_t alpha2, uint8_t alpha_mode, uint8_t dest_format, uint32_t *dest_rgb,
	uint8_t *dest_alpha, int log)
{
	uint32_t result;
	uint8_t r1,g1,b1;
	uint8_t r2,g2,b2;
	uint8_t r3,g3,b3;
	uint32_t r3_temp, g3_temp, b3_temp;
	uint32_t alpha_temp;
	uint8_t *pbyte;

	if(1 == log)
		bbu_printf("do_compose: a1 = %d, a2 = %d\r\n", (int)alpha1, (int)alpha2);

	if(NULL == src2){
		*dest_rgb = (*src1)&0xFFFFFF;
		if(dest_format == LCD_FORMAT_RGB888_PACKED)
			result = (*src1)&0xFFFFFF;
		else if(dest_format == LCD_FORMAT_RGBA888)
			result = (alpha1 << 24) | ((*src1)&0xFFFFFF);
		else if(dest_format == LCD_FORMAT_RGB888A)
			result = ((*src1 << 8) & 0xFFFFFF00) | alpha1;
		else if(dest_format == LCD_FORMAT_RGB888_UNPACKED)
			result = (*src1)&0xFFFFFF;
		else{
			bbu_printf("Error: do_compose, Invalid dest format (0x%x)!\r\n",dest_format);
		}
		return result;
	}

	pbyte = (uint8_t*)src1;
	b1 = *pbyte++;
	g1 = *pbyte++;
	r1 = *pbyte++;

	pbyte = (uint8_t*)src2;
	b2 = *pbyte++;
	g2 = *pbyte++;
	r2 = *pbyte++;

	if(1 == log)
		bbu_printf("r1 = %d, g1 = %d, b1 = %d, r2 = %d, g2 = %d, b2 = %d\r\n",
			(int)r1, (int)g1, (int)b1, (int)r2, (int)g2, (int)b2);

	if(alpha_mode == LCD_ALPHA_MODE_NORMAL){
		r3_temp = (r2 * alpha2 + r1 *(255-alpha2))/255;
		g3_temp = (g2 * alpha2 + g1 *(255-alpha2))/255;
		b3_temp = (b2 * alpha2 + b1 *(255-alpha2))/255;
		alpha_temp = alpha1 + alpha2 - alpha1 * alpha2/255;
	} else if(alpha_mode == LCD_ALPHA_MODE_PRE){
		r3_temp = (r2 * 255 + r1 * (255 - alpha2))/255;
		g3_temp = (g2 * 255 + g1 *(255-alpha2))/255;
		b3_temp = (b2 * 255 + b1 *(255-alpha2))/255;
		alpha_temp = alpha1 + alpha2 - alpha1 * alpha2/255;
	} else{
		r3_temp = (r2 * 255 + r1 * alpha2)/255;
		g3_temp = (g2 * 255 + g1 *alpha2)/255;
		b3_temp = (b2 * 255 + b1 *alpha2)/255;
		alpha_temp = alpha1 + alpha2 - alpha1 * alpha2/255;
		if(1 == log)
			bbu_printf("r3_temp = %d, g3_temp = %d, b3_temp = %d\r\n", r3_temp,
				g3_temp, b3_temp);
	}
	r3 = (r3_temp > 255) ? 255 : r3_temp;
	g3 = (g3_temp > 255) ? 255 : g3_temp;
	b3 = (b3_temp > 255) ? 255 : b3_temp;
	*dest_alpha = (alpha_temp > 255) ? 255 : alpha_temp;

	if(1 == log)
		bbu_printf("r3 = %d, g3 = %d, b3 = %d, alpha= %d\r\n",(int)r3, (int)g3,
			(int)b3, (int)*dest_alpha);

	*dest_rgb = (r3<<16) | (g3<<8) | b3;
	if(dest_format == LCD_FORMAT_RGB888_PACKED)
		result = (r3<<16) | (g3<<8) | b3;
	else if(dest_format == LCD_FORMAT_RGBA888)
		result = (*dest_alpha << 24) | (r3<<16) | (g3<<8) | b3;
	else if(dest_format == LCD_FORMAT_RGB888A)
		result = (r3<<24) | (g3<<16) | (b3<<8) | *dest_alpha;
	else if(dest_format == LCD_FORMAT_RGB888_UNPACKED)
		result = (r3<<16) | (g3<<8) | b3;

	if(1 == log)
		bbu_printf("result = 0x%x\r\n", result);
	return result;	
}

static void dump_compose_setting(struct s_compose_setting *compose_setting)
{
	struct s_compose_layer_setting *layer = NULL;

	bbu_printf("dump_compose_setting ++++++++++\r\n");
	bbu_printf("w = %d, h = %d\r\n", compose_setting->width, compose_setting->height);
	bbu_printf("d_addr = 0x%x, d_stride = %d\r\n", compose_setting->dest_addr,
		compose_setting->dest_stride);
	bbu_printf("d_format = %d, alpha_mode = %d\r\n", (int)compose_setting->dest_format,
		(int)compose_setting->alpha_mode);
	layer = &compose_setting->layer_setting[LCD_LAYER_IMG];
	bbu_printf("img layer setting++++++\r\n");
	bbu_printf("addr = 0x%x, stride = %d\r\n", layer->addr, layer->stride);
	bbu_printf("fomat = %d, alpha_sel = %d\r\n", (int)layer->format, (int)layer->alpha_sel);
	bbu_printf("en = %d, alpha = %d\r\n", (int)layer->layer_en, (int)layer->layer_alpha);
	layer = &compose_setting->layer_setting[LCD_LAYER_OSD1];
	bbu_printf("img layer setting++++++\r\n");
	bbu_printf("addr = 0x%x, stride = %d\r\n", layer->addr, layer->stride);
	bbu_printf("fomat = %d, alpha_sel = %d\r\n", (int)layer->format, (int)layer->alpha_sel);
	bbu_printf("en = %d, alpha = %d\r\n", (int)layer->layer_en, (int)layer->layer_alpha);
	layer = &compose_setting->layer_setting[LCD_LAYER_OSD2];
	bbu_printf("img layer setting++++++\r\n");
	bbu_printf("addr = 0x%x, stride = %d\r\n", layer->addr, layer->stride);
	bbu_printf("fomat = %d, alpha_sel = %d\r\n", (int)layer->format, (int)layer->alpha_sel);
	bbu_printf("en = %d, alpha = %d\r\n", (int)layer->layer_en, (int)layer->layer_alpha);
	bbu_printf("dump_compose_setting ----------\r\n");
}

static void draw_pure_color(uint32_t addr, uint32_t width,
				uint32_t height, uint32_t color, uint32_t align, uint32_t *stride)
{
	uint32_t i,j;
	uint32_t *pdata = (uint32_t*)addr;
	uint32_t porch = 0;

	bbu_printf("draw_pure_color: color = 0x%x\r\n", color);

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	bbu_printf("draw_pure_color: porch = 0x%x, stride = 0x%x\r\n", porch, *stride);

	for(i = 0;i< height; i++){
		for(j=0; j< width; j++){
			*pdata = color;
			pdata++;
		}
		pdata += porch;
	}
}

static void draw_hcb(uint32_t addr, uint32_t width,
				uint32_t height, uint32_t cb_num, uint32_t align, uint32_t *stride)
{
    uint32_t i,j;
    uint32_t *pdata = (uint32_t*)addr;
//    uint32_t data[5] = {0x00FF0000,0x0000FF00, 0x000000FF,0x00FFFFFF, 0x00000000};
    uint32_t data[5] = {0x55FF0000,0x5500FF00, 0x550000FF,0x55FFFFFF, 0x55000000};
    uint32_t cur_index = 0;
    uint32_t line_num;
	uint32_t porch = 0;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	bbu_printf("draw_hcb: porch = 0x%x, stride = 0x%x\r\n", porch, *stride);

    line_num = height/cb_num;

    cur_index = 0;
    for(i = 0;i< height; i++){
        for(j=0; j< width; j++){
            *pdata = data[cur_index];
            pdata++;
        }
        if((i+1) % line_num == 0){
            cur_index++;
            if(cur_index ==5)
                cur_index = 0;
        }		
		pdata += porch;
    }
}

static void draw_vcb(uint32_t addr, uint32_t width,
				uint32_t height, uint32_t cb_num, uint32_t align, uint32_t *stride)
{
    uint32_t i,j;
    uint32_t *pdata = (uint32_t*)addr;
//    uint32_t data[5] = {0x00FF0000,0x0000FF00, 0x000000FF,0x00FFFFFF, 0x00000000};
    uint32_t data[5] = {0xFFFF0000,0xFF00FF00, 0xFF0000FF,0xFFFFFFFF, 0xFF000000};
    uint32_t cur_index = 0;
    uint32_t colum_num;
	uint32_t porch = 0;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	bbu_printf("draw_vcb: porch = 0x%x, stride = 0x%x\r\n", porch, *stride);

    colum_num = width/cb_num;
    for(i = 0;i< height; i++){
        cur_index = 0;
        for(j=0; j< width; j++){
            *pdata = data[cur_index];
            pdata++;
            if((j+1) % colum_num == 0){
                cur_index++;
                if(cur_index ==5)
                    cur_index = 0;
            }
        }
		pdata += porch;
    }
}

static void draw_line(uint32_t addr, uint32_t width,
				uint32_t height, uint32_t align, uint32_t *stride)
{
    uint32_t i,j;
    uint32_t *pdata = (uint32_t*)addr;
    uint32_t data[3] = {0x00FF0000,0x0000FF00, 0x000000FF};
    uint32_t cur_index = 0;
	uint32_t porch = 0;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	bbu_printf("draw_line: porch = 0x%x, stride = 0x%x\r\n", porch, *stride);

    cur_index = 0;

    for(i = 0;i< height; i++){
        for(j=0; j< width; j++){
            *pdata = data[cur_index];
            pdata++;
        }
        cur_index++;
        cur_index = cur_index % 3;
		pdata += porch;
    }
}

static void draw_grey_level(uint32_t addr, uint32_t width,
				uint32_t height, uint32_t align, uint32_t *stride)
{
    uint32_t i,j;
    uint32_t *pdata = (uint32_t*)addr;
	uint32_t porch = 0;
	uint8_t cur_value = 0;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	bbu_printf("draw_grey_level: porch = 0x%x, stride = 0x%x\r\n", porch, *stride);

    for(i = 0;i< height; i++){
        for(j=0; j< width; j++){
            *pdata++ = cur_value | cur_value << 8 | cur_value << 16;
        }
		if((i%4 == 0) && (i > 0)){
        	cur_value++;
        	cur_value = cur_value % 256;
		}
		pdata += porch;
    }
}

static void prepare_argb8888_image(uint32_t addr, uint32_t pattern,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
    switch(pattern){
    case IMG_PATTERN_RED:
        draw_pure_color(addr, width, height, 0xFF0000, align, stride);
        break;
    case IMG_PATTERN_GREEN:
        draw_pure_color(addr, width, height, 0xFF00, align, stride);
        break;
    case IMG_PATTERN_BLUE:
        draw_pure_color(addr, width, height, 0xFF, align, stride);
        break;
    case IMG_PATTERN_HCB_5:
        draw_hcb(addr, width, height, 5, align, stride);
        break;
    case IMG_PATTERN_HCB_2:
        draw_hcb(addr, width, height, 2, align, stride);
        break;
    case IMG_PATTERN_VCB_5:
        draw_vcb(addr, width, height, 5, align, stride);
        break;
    case IMG_PATTERN_VCB_2:
        draw_vcb(addr, width, height, 2, align, stride);
        break;
    case IMG_PATTERN_LINE:
        draw_line(addr, width, height, align, stride);
        break;
	case IMG_PATTERN_GREY_LEVEL:
		draw_grey_level(addr, width, height, align, stride);
		break;
    };
	bbu_printf("prepare_argb8888_image: stride = 0x%x\r\n", *stride);
}

static void argb8888_to_rgb888(uint32_t src_addr, uint32_t dest_addr,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint8_t *psrc, *pdest;
	uint32_t porch;

	*stride = ALIGN(width * 3, align);
	porch = *stride - width * 3;
	bbu_printf("argb8888_to_rgb888: porch = 0x%x, stride = 0x%x\r\n", porch, *stride);

	psrc = (uint8_t*)src_addr;
	pdest = (uint8_t*)dest_addr;


	for(i=0; i<height; i++){
		for(j=0;j<width;j++){
			*pdest++ = *psrc++; /*b*/
			*pdest++ = *psrc++; /*g*/
			*pdest++ = *psrc++; /*r*/
			psrc++; /*alpha*/
		}
		pdest += porch;
	}
}

static void argb8888_to_rgba8888(uint32_t src_addr, uint32_t dest_addr,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint8_t *psrc, *pdest;
	uint32_t porch;
	uint8_t r,g,b,a;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	bbu_printf("argb8888_to_rgba8888: porch = 0x%x, stride = 0x%x\r\n", porch, *stride);

	psrc = (uint8_t*)src_addr;
	pdest = (uint8_t*)dest_addr;


	for(i=0; i<height; i++){
		for(j=0;j<width;j++){
			b = *psrc++; /*b*/
			g = *psrc++; /*g*/
			r = *psrc++; /*r*/
			a = *psrc++; /*alpha*/
			*pdest++ = a;
			*pdest++ = b;
			*pdest++ = g;
			*pdest++ = r;
		}
		pdest += porch;
	}
}


static void argb8888_to_rgb565(uint32_t src_addr, uint32_t dest_addr,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i,j;
	uint8_t *psrc;
	uint16_t *pdest;
	uint8_t r,g,b;
	uint32_t porch;

	*stride = ALIGN(width * 2, align);
	porch = *stride - width * 2;
	bbu_printf("argb8888_to_rgb565: porch = 0x%x, stride = 0x%x\r\n", porch, *stride);

	psrc = (uint8_t*)src_addr;
	pdest = (uint16_t*)dest_addr;

	for(i=0; i<height; i++){
		for(j=0;j<width;j++){
			b = *psrc++; /*b*/
			g = *psrc++; /*g*/
			r = *psrc++; /*r*/
			psrc++; /*alpha*/

			*pdest++ = ((r & 0x1F)<< 11) |
				((g & 0x3F) << 5) | (b & 0x1F);
		}
		pdest += (porch >> 1);
	}
}

static void argb8888_to_rgb1555(uint32_t src_addr, uint32_t dest_addr,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i,j;
	uint8_t *psrc;
	uint16_t *pdest;
	uint8_t r,g,b;
	uint32_t porch;

	*stride = ALIGN(width * 2, align);
	porch = *stride - width * 2;
	bbu_printf("argb8888_to_rgb1555: porch = 0x%x, stride = 0x%x\r\n", porch, *stride);

	psrc = (uint8_t*)src_addr;
	pdest = (uint16_t*)dest_addr;

	for(i=0; i<height; i++){
		for(j=0;j<width;j++){
			b = *psrc++; /*b*/
			g = *psrc++; /*g*/
			r = *psrc++; /*r*/
			psrc++; /*alpha*/

			*pdest++ = ((r & 0x1F)<< 10) |
				((g & 0x1F) << 5) | (b & 0x1F);
		}
		pdest += (porch >> 1);
	}
}

static void argb8888_to_xyuv444_p1(uint32_t src_addr, uint32_t dest_addr,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint8_t *psrc, *pdest;
	uint32_t r,g,b;
	int16_t u,v;
	uint8_t y;
	uint32_t porch;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	bbu_printf("argb8888_to_xyuv444_p1: porch = 0x%x, stride = 0x%x\r\n", porch, *stride);

	psrc = (uint8_t*)src_addr;
	pdest = (uint8_t*)dest_addr;

	for(i=0; i<height; i++){
		for(j=0; j<width; j++){
			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (int16_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (int16_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			u = (u < 0) ? 0 : u;
			u = (u > 255) ? 255 : u;
			v = (v < 0) ? 0 : v;
			v = (v > 255) ? 255 : v;

			*pdest++ = (uint8_t)v;
			*pdest++ = (uint8_t)u;
			*pdest++ = y;
			*pdest++ = 0;
		}
		pdest += porch;
	}
}

static void argb8888_to_yuv420_p2(uint32_t src_addr, uint32_t dest_addr_p1,
					uint32_t *dest_addr_p2, uint32_t width, uint32_t height,
					uint32_t align, uint32_t *stride_p1, uint32_t *stride_p2)
{
	uint32_t i,j;
	uint8_t *psrc, *pdest_p1, *pdest_p2;
	uint32_t r,g,b;
	uint8_t y,u,v;
	uint32_t porch_p1, porch_p2;
	uint32_t addr_p2;

	*stride_p1 = ALIGN(width, align);
	*stride_p2 = ALIGN(width, align);
	porch_p1 = *stride_p1 - width;
	porch_p2 = *stride_p2 - width;
	bbu_printf("argb8888_to_yuv420_p2: p1 = 0x%x, s1 = 0x%x, p2 = 0x%x, s2 = 0x%x\r\n",
		porch_p1, *stride_p1, porch_p2, *stride_p2);

	addr_p2 = dest_addr_p1 + (*stride_p1) * height;
	*dest_addr_p2 = addr_p2;

	psrc = (uint8_t*)src_addr;
	pdest_p1 = (uint8_t*)dest_addr_p1;
	pdest_p2 = (uint8_t*)addr_p2;

	for(j=0; j<height; j++){
		for(i=0; i<width;i+=2){
			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest_p1++ = y;
			if(0 ==  j%2)
				*pdest_p2++ = u;

			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest_p1++ = y;
			if(0 ==  j%2)
				*pdest_p2++ = v;
		}
		pdest_p1 += porch_p1;
		if(0 ==  j%2)
			pdest_p2 += porch_p2;
	}
}

static void argb8888_to_yuv420_p3(uint32_t src_addr, uint32_t dest_addr_p1,
					uint32_t *dest_addr_p2, uint32_t *dest_addr_p3,
					uint32_t width, uint32_t height, uint32_t align,
					uint32_t *stride_p1, uint32_t *stride_p2, uint32_t *stride_p3)
{
	uint32_t i,j;
	uint8_t *psrc, *pdest_p1, *pdest_p2, *pdest_p3;
	uint32_t r,g,b;
	uint8_t y,u,v;
	uint32_t porch_p1, porch_p2, porch_p3;
	uint32_t addr_p2, addr_p3;

	*stride_p1 = ALIGN(width, align);
	*stride_p2 = ALIGN(width/2, align);
	*stride_p3 = ALIGN(width/2, align);
	porch_p1 = *stride_p1 - width;
	porch_p2 = *stride_p2 - width/2;
	porch_p3 = *stride_p3 - width/2;

	addr_p2 = dest_addr_p1 + (*stride_p1) * height;
	addr_p3 = addr_p2 + (*stride_p2) * height/2;
	*dest_addr_p2 = addr_p2;
	*dest_addr_p3 = addr_p3;
	bbu_printf("argb8888_to_yuv420_p3: p1 = 0x%x, s1 = 0x%x, p2 = 0x%x, s2 = 0x%x, p3 = 0x%x, s3 = 0x%x\r\n",
		porch_p1, *stride_p1, porch_p2, *stride_p2, porch_p3, *stride_p3);

	psrc = (uint8_t*)src_addr;
	pdest_p1 = (uint8_t*)dest_addr_p1;
	pdest_p2 = (uint8_t*)addr_p2;
	pdest_p3 = (uint8_t*)addr_p3;

	for(j=0; j<height; j++){
		for(i=0; i<width;i+=2){
			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest_p1++ = y;
			if(0 ==  j%2)
				*pdest_p2++ = u;

			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest_p1++ = y;
			if(0 ==  j%2)
				*pdest_p3++ = v;
		}
		pdest_p1 += porch_p1;
		if(0 ==  j%2) {
			pdest_p2 += porch_p2;
			pdest_p3 += porch_p3;
		}
	}
}

static void argb8888_to_yuv422_p3(uint32_t src_addr, uint32_t dest_addr_p1,
					uint32_t *dest_addr_p2, uint32_t *dest_addr_p3,
					uint32_t width, uint32_t height, uint32_t align,
					uint32_t *stride_p1, uint32_t *stride_p2, uint32_t *stride_p3)
{
	uint32_t i,j;
	uint8_t *psrc, *pdest_p1, *pdest_p2, *pdest_p3;
	uint32_t r,g,b;
	uint8_t y,u,v;
	uint32_t porch_p1, porch_p2, porch_p3;
	uint32_t addr_p2, addr_p3;

	*stride_p1 = ALIGN(width, align);
	*stride_p2 = ALIGN(width/2, align);
	*stride_p3 = ALIGN(width/2, align);
	porch_p1 = *stride_p1 - width;
	porch_p2 = *stride_p2 - width/2;
	porch_p3 = *stride_p3 - width/2;

	addr_p2 = dest_addr_p1 + (*stride_p1) * height;
	addr_p3 = addr_p2 + (*stride_p2) * height;
	*dest_addr_p2 = addr_p2;
	*dest_addr_p3 = addr_p3;
	bbu_printf("argb8888_to_yuv422_p3: p1 = 0x%x, s1 = 0x%x, p2 = 0x%x, s2 = 0x%x, p3 = 0x%x, s3 = 0x%x\r\n",
		porch_p1, *stride_p1, porch_p2, *stride_p2, porch_p3, *stride_p3);

	psrc = (uint8_t*)src_addr;
	pdest_p1 = (uint8_t*)dest_addr_p1;
	pdest_p2 = (uint8_t*)addr_p2;
	pdest_p3 = (uint8_t*)addr_p3;

	for(j=0; j<height; j++){
		for(i=0; i<width;i+=2){
			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest_p1++ = y;
			*pdest_p2++ = u;

			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest_p1++ = y;
			*pdest_p3++ = v;
		}
		pdest_p1 += porch_p1;
		pdest_p2 += porch_p2;
		pdest_p3 += porch_p3;
	}
}

static void argb8888_to_yuv422_p1(uint32_t src_addr, uint32_t dest_addr_p1,
					uint32_t width, uint32_t height, uint32_t align,
					uint32_t *stride_p1)
{
	uint32_t i,j;
	uint8_t *psrc, *pdest_p1;
	uint32_t r,g,b;
	uint8_t y,u,v;
	uint32_t porch_p1;

	*stride_p1 = ALIGN(width * 2, align);
	porch_p1 = *stride_p1 - width * 2;

	bbu_printf("argb8888_to_yuv422_p1: p1 = 0x%x, s1 = 0x%x", porch_p1, *stride_p1);

	psrc = (uint8_t*)src_addr;
	pdest_p1 = (uint8_t*)dest_addr_p1;

	/*format is UYVY*/
	for(j=0; j<height; j++){
		for(i=0; i<width;i+=2){
			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest_p1++ = u;
			*pdest_p1++ = y;

			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest_p1++ = v;
			*pdest_p1++ = y;
		}
		pdest_p1 += porch_p1;
	}
}

int32_t lcd_draw_images(uint32_t addr_p1, uint32_t *addr_p2, uint32_t *addr_p3,
					uint32_t format, uint32_t pattern, uint32_t width,
					uint32_t height, uint32_t *stride_p1, uint32_t *stride_p2,
					uint32_t *stride_p3)
{
	uint32_t stride_temp = 0;

	if((LCD_FORMAT_RGBA888 == format) || (LCD_FORMAT_RGB888_UNPACKED == format)){
		prepare_argb8888_image(addr_p1, pattern, width, height, STRIDE_ALIGN, stride_p1);
		bbu_printf("draw_images: stride = 0x%x\r\n", *stride_p1);
		return 0;
	}

	prepare_argb8888_image(TEMP_BUFFER_ADDR, pattern, width, height, 1, &stride_temp);
	bbu_printf("draw_images: temp stride = 0x%x\r\n", stride_temp);
	switch(format){
	case LCD_FORMAT_RGB888_PACKED:
		argb8888_to_rgb888(TEMP_BUFFER_ADDR, addr_p1, width, height, STRIDE_ALIGN, stride_p1);
		bbu_printf("draw_images: stride = 0x%x\r\n", *stride_p1);
		break;
	case LCD_FORMAT_RGB565:
		argb8888_to_rgb565(TEMP_BUFFER_ADDR, addr_p1, width, height, STRIDE_ALIGN, stride_p1);
		bbu_printf("draw_images: stride = 0x%x\r\n", *stride_p1);
		break;
	case LCD_FORMAT_RGB888A:
		argb8888_to_rgba8888(TEMP_BUFFER_ADDR, addr_p1, width, height, STRIDE_ALIGN, stride_p1);
		bbu_printf("draw_images: stride = 0x%x\r\n", *stride_p1);
		break;
	case LCD_FORMAT_YUV420_SEMI:
		argb8888_to_yuv420_p2(TEMP_BUFFER_ADDR, addr_p1, addr_p2,
			width, height, STRIDE_ALIGN, stride_p1, stride_p2);
		bbu_printf("draw_images: stride = 0x%x, 0x%x\r\n", *stride_p1, *stride_p2);
		break;
	case LCD_FORMAT_YUV422_PLANAR:
		argb8888_to_yuv422_p3(TEMP_BUFFER_ADDR, addr_p1, addr_p2,
			addr_p3, width, height, STRIDE_ALIGN, stride_p1, stride_p2, stride_p3);
		bbu_printf("draw_images: stride = 0x%x, 0x%x, 0x%x\r\n", *stride_p1, *stride_p2, *stride_p3);
		break;
	case LCD_FORMAT_YUV420_PLANAR:
		argb8888_to_yuv420_p3(TEMP_BUFFER_ADDR, addr_p1, addr_p2,
			addr_p3, width, height, STRIDE_ALIGN, stride_p1, stride_p2, stride_p3);
		bbu_printf("draw_images: stride = 0x%x, 0x%x, 0x%x\r\n", *stride_p1, *stride_p2, *stride_p3);
		break;
	case LCD_FORMAT_YUV422_PACKED:
		argb8888_to_yuv422_p1(TEMP_BUFFER_ADDR, addr_p1, width, height,
			STRIDE_ALIGN, stride_p1);
		bbu_printf("draw_images: stride = 0x%x\r\n", *stride_p1);
		break;
	case LCD_FORMAT_RGB1555:
		argb8888_to_rgb1555(TEMP_BUFFER_ADDR, addr_p1, width, height,
			STRIDE_ALIGN, stride_p1);
		bbu_printf("draw_images: stride = 0x%x\r\n", *stride_p1);
		break;
	default:
		bbu_printf("draw_images: Unsupport formate = %d\r\n", format);
	}
	return 0;
}

int32_t lcd_compare_buffers(uint32_t src, uint32_t dest, uint32_t width,
		uint32_t height, uint32_t stride_src, uint32_t stride_dest,uint32_t format, int log)
{
	uint32_t var1,var2;
	uint32_t res, i, j;
	uint8_t *mem_ptr1;
	uint8_t *mem_ptr2;
	uint32_t bpp;

	if((LCD_FORMAT_RGB565 != format) && (LCD_FORMAT_RGB888_PACKED!= format) &&
		(LCD_FORMAT_RGB888A != format) && (LCD_FORMAT_RGBA888 != format)){
		bbu_printf("compare_buffers: only support RGB565 & RGB888_packet & RGB888A (0x%x)\r\n", format);
		return -1;
	}

	if(LCD_FORMAT_RGB565 == format){
		bpp = 2;
	} else if(LCD_FORMAT_RGB888_PACKED == format) {
		bpp = 3;
	} else {
		bpp = 4;
	}

	for(j=0;j<height;j++){
		mem_ptr1 = (uint8_t*)(src + j*stride_src);
		mem_ptr2 = (uint8_t*)(dest + j*stride_dest);

		//bbu_printf("Compare 0x%x, 0x%x+++, j=%d\r\n", mem_ptr1, mem_ptr2, j);
		res = memcmp(mem_ptr1,mem_ptr2,width*bpp);
		if(res !=0) {
			res = 0;
			for(i=0;i<width;i++) {
				if(4 == bpp){
					var1 = *(uint32_t*)mem_ptr1;
					var2 = *(uint32_t*)mem_ptr2;
				} else if (3 == bpp){
					var1 = (*mem_ptr1 << 16) | (*(mem_ptr1 + 1) << 8) | *(mem_ptr1 + 2);
					var2 = (*mem_ptr2 << 16) | (*(mem_ptr2 + 1) << 8) | *(mem_ptr2 + 2);
				} else {
					var1 = (*mem_ptr1 << 8) | *(mem_ptr1 + 1);
					var2 = (*mem_ptr2 << 8) | *(mem_ptr2 + 1);
				}
				if((var1 & 0xFFFFFF)!= (var2 & 0xFFFFFF)) {
					if(1 == log){
						bbu_printf("mismatch: Ref data is 0x%x; Ref address is 0x%x.\r\n",var1,mem_ptr1);
						bbu_printf("	  HW data is 0x%x; HW address is 0x%x.\r\n",var2,mem_ptr2);
					}
					res = -1;
				}
				mem_ptr1 += bpp;
				mem_ptr2 += bpp;
			}
			if(res != 0){
				if(log){
					bbu_printf("Error during check of bytes from address: %d.\r\n",dest + j*stride_dest);
				}
				return -1;
			}
		}
		//bbu_printf("Compare 0x%x, 0x%x---, j=%d\r\n", mem_ptr1, mem_ptr2, j);
	}
	return 0;
}

int lcd_compare_compose_buffer(struct s_compose_setting *compose_setting, int log)
{
	struct s_compose_layer_setting *img_layer = NULL;
	struct s_compose_layer_setting *osd1_layer = NULL;
	struct s_compose_layer_setting *osd2_layer = NULL;

	struct s_compose_layer_setting *first_layer = NULL;
	struct s_compose_layer_setting *second_layer = NULL;
	struct s_compose_layer_setting *third_layer = NULL;

	uint32_t i, j; 
	int ret, ret1 = 0;
	uint32_t src1, src2, result, dest_rgb, dest_addr;
	uint8_t alpha1, alpha2, dest_alpha;
	uint8_t src_r, src_g, src_b, src_a;
	uint8_t dest_r, dest_g, dest_b, dest_a;
	uint8_t *pdest8 = NULL;
	uint32_t *pdest32 = NULL;
	uint32_t count = 0;
	
	if(NULL == compose_setting){
		bbu_printf("Error: lcd_compare_compose_buffer, Invalid paramr\r\n");
		return -1;
	}

	dump_compose_setting(compose_setting);
	img_layer = &compose_setting->layer_setting[LCD_LAYER_IMG];
	osd1_layer = &compose_setting->layer_setting[LCD_LAYER_OSD1];
	osd2_layer = &compose_setting->layer_setting[LCD_LAYER_OSD2];

	if(1 == img_layer->layer_en){
		first_layer = img_layer;
		if(1 == osd1_layer->layer_en){
			second_layer = osd1_layer;
			if(1 == osd2_layer->layer_en){
				third_layer = osd2_layer;
			}
		} else if(1 == osd2_layer->layer_en){
			second_layer = osd2_layer;
		}
	} else if(1 == osd1_layer->layer_en){
		first_layer = osd1_layer;
		if(1 == osd2_layer->layer_en){
			second_layer = osd2_layer;
		}
	} else if(1 == osd2_layer->layer_en){
		first_layer = osd2_layer;
	}

	if(NULL == first_layer){
		bbu_printf("Error: lcd_compare_compose_buffer, No layer is enabled!\r\n");
		return -1;
	}

	for(i = 0; i < compose_setting->height; i++){
		for(j = 0; j < compose_setting->width; j++){
			/*get src1 data*/
			ret = get_layer_rgb_data(j, i, 1, first_layer, &src1, &alpha1);
			if(0 != ret){
				bbu_printf("Error: get_layer_rgb_data fail!\r\n");
				return -1;
			} else {
				if(1 == log)
					bbu_printf("INFO: get src1 %d, %d rgb data is 0x%x, A is%d\r\n",
						j, i, src1, alpha1);
			}
			
			if(NULL == second_layer){
				result = do_compose(&src1, NULL, alpha1, 0, compose_setting->alpha_mode,
				compose_setting->dest_format, &dest_rgb, &dest_alpha, log);
				goto compare;
			}

			/*get src2 data*/
			ret = get_layer_rgb_data(j, i, 0, second_layer, &src2, &alpha2);
			if(0 != ret){
				bbu_printf("Error: get_layer_rgb_data fail!\r\n");
				return -1;
			} else {
				if(1 == log)
					bbu_printf("INFO: get src2 %d, %d rgb data is 0x%x, A is%d\r\n",
						j, i, src2, alpha2);
			}

			result = do_compose(&src1, &src2, alpha1, alpha2, compose_setting->alpha_mode,
				compose_setting->dest_format, &dest_rgb, &dest_alpha, log);
			if(1 == log)
				bbu_printf("INFO: compose 0x%x, 0x%x is 0x%x!\r\n", src1, src2, result);

			if(NULL == third_layer){
				goto compare;
			}

			src1 = dest_rgb;
			alpha1 = dest_alpha;
			/*get src3 data*/
			ret = get_layer_rgb_data(j, i, 0, third_layer, &src2, &alpha2);
			if(0 != ret){
				bbu_printf("Error: get_layer_rgb_data fail!\r\n");
				return -1;
			} else {
				if(1 == log)
					bbu_printf("INFO: get %d, %d rgb data is 0x%x!\r\n", j, i, src2);
			}
			result = do_compose(&src1, &src2, alpha1, alpha2, compose_setting->alpha_mode,
				compose_setting->dest_format, &dest_rgb, &dest_alpha, log);
			if(1 == log)
				bbu_printf("INFO: compose 0x%x, 0x%x is 0x%x!\r\n", src1, src2, result);

compare:
			if(compose_setting->dest_format == LCD_FORMAT_RGB888_PACKED){
				dest_addr = compose_setting->dest_addr + i * compose_setting->dest_stride
					+ j * 3;
				pdest8 = (uint8_t*)dest_addr;
				dest_b = (*pdest8++) & 0xFF;
				dest_g = (*pdest8++) & 0xFF;
				dest_r = (*pdest8) & 0xFF;
				dest_a = 0;
				dest_rgb = (dest_r << 16) | (dest_g << 8) | dest_b;
				result = result & 0xFFFFFF;
				pdest8 = (uint8_t*)&result;
				src_b = (*pdest8++) & 0xFF;
				src_g = (*pdest8++) & 0xFF;
				src_r = (*pdest8) & 0xFF;
				src_a = 0;
			} else if(compose_setting->dest_format == LCD_FORMAT_RGB888A){
				dest_addr = compose_setting->dest_addr + i * compose_setting->dest_stride
					+ j * 4;
				pdest32 = (uint32_t*)dest_addr;
				dest_rgb = *pdest32;
				if(compose_setting->alpha_mode != LCD_ALPHA_MODE_PRE){
					result = result & 0xFFFFFF00;
					dest_rgb = dest_rgb & 0xFFFFFF00;
				}
				pdest8 = (uint8_t*)&dest_rgb;
				dest_a = (*pdest8++) & 0xFF;
				dest_b = (*pdest8++) & 0xFF;
				dest_g = (*pdest8++) & 0xFF;
				dest_r = (*pdest8) & 0xFF;
				pdest8 = (uint8_t*)&result;
				src_a = (*pdest8++) & 0xFF;
				src_b = (*pdest8++) & 0xFF;
				src_g = (*pdest8++) & 0xFF;
				src_r = (*pdest8) & 0xFF;
			} else if(compose_setting->dest_format == LCD_FORMAT_RGBA888){
				dest_addr = compose_setting->dest_addr + i * compose_setting->dest_stride
					+ j * 4;
				pdest32 = (uint32_t*)dest_addr;
				dest_rgb = *pdest32;
				if(compose_setting->alpha_mode != LCD_ALPHA_MODE_PRE){
					result = result & 0xFFFFFF;
					dest_rgb = dest_rgb & 0xFFFFFF;
				}
				pdest8 = (uint8_t*)&dest_rgb;
				dest_b = (*pdest8++) & 0xFF;
				dest_g = (*pdest8++) & 0xFF;
				dest_r = (*pdest8++) & 0xFF;
				dest_a = (*pdest8) & 0xFF;
				pdest8 = (uint8_t*)&result;
				src_b = (*pdest8++) & 0xFF;
				src_g = (*pdest8++) & 0xFF;
				src_r = (*pdest8++) & 0xFF;
				src_a = (*pdest8) & 0xFF;
			} else {
				bbu_printf("Error: Unsupported dest format (0x%x)!\r\n",
					compose_setting->dest_format);
				return -1;
			}
			count ++;

#if 0
			if(result != dest_rgb){
				bbu_printf("Error: Compare fail: (w=%d, h=%d) src = 0x%x, dest = 0x%x\r\n", j, i,
					result, dest_rgb);
				bbu_printf("Error: dest_addr = 0x%x\r\n",dest_addr);
				ret1 = -1;
				return -1;
			}
#else
			if((dest_r < (src_r - 1)) || (dest_r > (src_r + 1)) ||
				(dest_g < (src_g - 1)) || (dest_g > (src_g + 1)) ||
				(dest_b < (src_b - 1)) || (dest_b > (src_b + 1)) ||
				(dest_a < (src_a - 1)) || (dest_a > (src_a + 1))){
				bbu_printf("Error: Compare fail: (w=%d, h=%d) src = 0x%x, dest = 0x%x\r\n", j, i,
					result, dest_rgb);
				bbu_printf("Error: dest_addr = 0x%x\r\n",dest_addr);
				bbu_printf("Error: src_r,g,b,a = %d, %d, %d, %d\r\n",src_r, src_g, src_b, src_a);
				bbu_printf("Error: dest_r,g,b,a = %d, %d, %d, %d\r\n",dest_r, dest_g, dest_b, dest_a);
				ret1 = -1;
				return -1;
			}
#endif
		}
	}
	bbu_printf("INFO: compare %d pixel are same\r\n",count);
	return ret1;
}
