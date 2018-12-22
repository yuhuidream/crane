#if defined(HARDWARE_EVB) //aquilac evb
#include <string.h>
#include "plat_basic_api.h"
#else //crane fpga

#endif
#include "dp650_fns.h"
#include "scaler_rotation.h"

#if 0
#define TEMP_BUFFER_ADDR 0x2D000000
#else /* crane project */
#define TEMP_BUFFER_ADDR 0xD1200000 /* must above stack end address */
#endif

static void draw_pure_color(uint32_t addr, uint32_t width,
			    uint32_t height, uint32_t color, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint32_t *pdata = (uint32_t *)addr;
	uint32_t porch = 0;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	dprint("draw_pure_color: porch = 0x%x, stride = 0x%x\n", porch, *stride);

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*pdata = color;
			pdata++;
		}
		pdata += porch;
	}
}

static void draw_hcb(uint32_t addr, uint32_t width,
		     uint32_t height, uint32_t cb_num, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint32_t *pdata = (uint32_t *)addr;
	uint32_t data[5] = {0x00FF0000, 0x0000FF00, 0x000000FF, 0x00FFFFFF, 0x00000000};
	uint32_t cur_index = 0;
	uint32_t line_num;
	uint32_t porch = 0;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	dprint("draw_hcb: porch = 0x%x, stride = 0x%x\n", porch, *stride);

	line_num = height / cb_num;

	cur_index = 0;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*pdata = data[cur_index];
			pdata++;
		}
		if ((i + 1) % line_num == 0) {
			cur_index++;
			if (cur_index == 5)
				cur_index = 0;
		}
		pdata += porch;
	}
}

static void draw_vcb(uint64_t addr, uint32_t width,
		     uint32_t height, uint32_t cb_num, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint32_t *pdata = (uint32_t *)addr;
	uint32_t data[5] = {0x00FF0000, 0x0000FF00, 0x000000FF, 0x00FFFFFF, 0x00000000};
	uint32_t cur_index = 0;
	uint32_t colum_num;
	uint32_t porch = 0;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	dprint("draw_vcb: porch = 0x%x, stride = 0x%x\n", porch, *stride);

	colum_num = width / cb_num;
	for (i = 0; i < height; i++) {
		cur_index = 0;
		for (j = 0; j < width; j++) {
			*pdata = data[cur_index];
			pdata++;
			if ((j + 1) % colum_num == 0) {
				cur_index++;
				if (cur_index == 5)
					cur_index = 0;
			}
		}
		pdata += porch;
	}
}

static void draw_line(uint32_t addr, uint32_t width,
		      uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint32_t *pdata = (uint32_t *)addr;
	uint32_t data[3] = {0x00FF0000, 0x0000FF00, 0x000000FF};
	uint32_t cur_index = 0;
	uint32_t porch = 0;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	dprint("draw_line: porch = 0x%x, stride = 0x%x\n", porch, *stride);

	cur_index = 0;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
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
	uint32_t i, j;
	uint32_t *pdata = (uint32_t *)addr;
	uint32_t porch = 0;
	uint8_t cur_value = 0;

	*stride = ALIGN(width * 4, align);
	porch = *stride - width * 4;
	dprint("draw_grey_level: porch = 0x%x, stride = 0x%x\n", porch, *stride);

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*pdata++ = cur_value | cur_value << 8 | cur_value << 16;
		}
		if ((i % 4 == 0) && (i > 0)) {
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

    dprint("prepare_argb8888_image: %dx%d stride = 0x%x\n", width, height, *stride);
}

static void argb8888_to_rgb888(uint32_t src_addr, uint32_t dest_addr,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint8_t *psrc, *pdest;
	uint32_t porch;

	*stride = ALIGN(width * 3, align);
	porch = *stride - width * 3;
	dprint("argb8888_to_rgb888: porch = 0x%x, stride = 0x%x\n", porch, *stride);

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
	dprint("argb8888_to_rgb565: porch = 0x%x, stride = 0x%x\n", porch, *stride);

	psrc = (uint8_t*)src_addr;
	pdest = (uint16_t*)dest_addr;

	for(i=0; i<height; i++){
		for(j=0;j<width;j++){
			b = *psrc++; /*b*/
			g = *psrc++; /*g*/
			r = *psrc++; /*r*/
			psrc++; /*alpha*/

			*pdest++ = ((r & 0x1F)<< 11) |
				((g & 0x2F) << 5) | (b & 0x1F);
		}
		pdest += (porch >> 1);
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
	dprint("argb8888_to_yuv420_p2: p1 = 0x%x, s1 = 0x%x, p2 = 0x%x, s2 = 0x%x\n",
		porch_p1, *stride_p1, porch_p2, *stride_p2);

	addr_p2 = dest_addr_p1 + (*stride_p1) * height;
	*dest_addr_p2 = addr_p2;

	psrc = (uint8_t*)src_addr;
	pdest_p1 = (uint8_t*)dest_addr_p1;
	pdest_p2 = (uint8_t*)addr_p2;

	dprint("argb8888_to_yuv420_p2: addr_p1 = 0x%x, addr_p2 = 0x%x, width=%d, height=%d\n",
		pdest_p1, pdest_p2, width, height);

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
					uint32_t *dest_addr_p2, uint32_t *dest_addr_p3, uint32_t width, uint32_t height,
					uint32_t align, uint32_t *stride_p1, uint32_t *stride_p2, uint32_t *stride_p3)
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

	dprint("argb8888_to_yuv420_p3: p1 = 0x%x, s1 = 0x%x, p2 = 0x%x, s2 = 0x%x, p3 = 0x%x, s3 = 0x%x\n",
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

/* YUV422: yuyv */
static void argb8888_to_yuyv(uint32_t src_addr, uint32_t dest_addr,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint8_t *psrc, *pdest;
	uint32_t r,g,b;
	uint8_t y,u,v;
	uint32_t porch;

	*stride = ALIGN(width * 2, align);
	porch = *stride - width * 2;
	dprint("%s: porch = 0x%x, stride = 0x%x\n", __func__, porch, *stride);

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
			*pdest++ = y;
			*pdest++ = u;

			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest++ = y;
			*pdest++ = v;
		}
		pdest += porch;
	}
}

/* YUV422: yvyu */
static void argb8888_to_yvyu(uint32_t src_addr, uint32_t dest_addr,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint8_t *psrc, *pdest;
	uint32_t r,g,b;
	uint8_t y,u,v;
	uint32_t porch;

	*stride = ALIGN(width * 2, align);
	porch = *stride - width * 2;
	dprint("%s: porch = 0x%x, stride = 0x%x\n", __func__, porch, *stride);

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
			*pdest++ = y;
			*pdest++ = v;

			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest++ = y;
			*pdest++ = u;
		}
		pdest += porch;
	}
}

/* YUV422: uyvy */
static void argb8888_to_uyvy(uint32_t src_addr, uint32_t dest_addr,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint8_t *psrc, *pdest;
	uint32_t r,g,b;
	uint8_t y,u,v;
	uint32_t porch;

	*stride = ALIGN(width * 2, align);
	porch = *stride - width * 2;
	dprint("%s: porch = 0x%x, stride = 0x%x\n", __func__, porch, *stride);

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
			*pdest++ = u;
			*pdest++ = y;

			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest++ = v;
			*pdest++ = y;
		}
		pdest += porch;
	}
}

/* YUV422: vyuy */
static void argb8888_to_vyuy(uint32_t src_addr, uint32_t dest_addr,
					uint32_t width, uint32_t height, uint32_t align, uint32_t *stride)
{
	uint32_t i, j;
	uint8_t *psrc, *pdest;
	uint32_t r,g,b;
	uint8_t y,u,v;
	uint32_t porch;

	*stride = ALIGN(width * 2, align);
	porch = *stride - width * 2;
	dprint("%s: porch = 0x%x, stride = 0x%x\n", __func__, porch, *stride);

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
			*pdest++ = v;
			*pdest++ = y;

			b = (uint32_t)*psrc++;
			g = (uint32_t)*psrc++;
			r = (uint32_t)*psrc++;
			psrc++;

			y = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
			u = (uint8_t)(128 + ((128 * b - 43 * r - 85 * g) >> 8));
			v = (uint8_t)(128  + ((128 * r - 107 * g - 21 * b ) >> 8));
			*pdest++ = u;
			*pdest++ = y;
		}
		pdest += porch;
	}
}

int32_t draw_images(uint32_t addr_p1, uint32_t *addr_p2, uint32_t *addr_p3,
					uint32_t format, uint32_t pattern,
					uint32_t width, uint32_t height, bool afbc,
					uint32_t *stride_p1, uint32_t *stride_p2, uint32_t *stride_p3)
{
	uint32_t stride_temp = 0;

	if(afbc){
		dprint("draw_images: not support afbc format!\n");
		return -1;
	}

	if(DP650_FORMAT_ARGB8888 == format){
		prepare_argb8888_image(addr_p1, pattern, width, height, STRIDE_ALIGN, stride_p1);
		dprint("draw_images: stride = 0x%x\n", *stride_p1);
		return 0;
	}

	prepare_argb8888_image(TEMP_BUFFER_ADDR, pattern, width, height, 1, &stride_temp);
	dprint("draw_images: temp stride = 0x%x\n", stride_temp);
	switch(format){
	case DP650_FORMAT_RGB888:
		argb8888_to_rgb888(TEMP_BUFFER_ADDR, addr_p1, width, height, STRIDE_ALIGN, stride_p1);
		dprint("draw_images: stride = 0x%x\n", *stride_p1);
		break;
	case DP650_FORMAT_RGB565:
		argb8888_to_rgb565(TEMP_BUFFER_ADDR, addr_p1, width, height, STRIDE_ALIGN, stride_p1);
		dprint("draw_images: stride = 0x%x\n", *stride_p1);
		break;
	case DP650_FORMAT_YUV420_P2:
		argb8888_to_yuv420_p2(TEMP_BUFFER_ADDR, addr_p1, addr_p2,
			width, height, STRIDE_ALIGN, stride_p1, stride_p2);
		dprint("draw_images: stride = 0x%x, 0x%x\n", *stride_p1, *stride_p2);
		break;
	case DP650_FORMAT_YUV420_P3:
		argb8888_to_yuv420_p3(TEMP_BUFFER_ADDR, addr_p1, addr_p2,
			addr_p3, width, height, STRIDE_ALIGN, stride_p1, stride_p2, stride_p3);
		dprint("draw_images: stride = 0x%x, 0x%x, 0x%x\n", *stride_p1, *stride_p2, *stride_p3);
		break;
	case DP650_FORMAT_YUYV:
		argb8888_to_yuyv(TEMP_BUFFER_ADDR, addr_p1, width, height, STRIDE_ALIGN, stride_p1);
		dprint("draw_images: stride = 0x%x\n", *stride_p1);
		break;
	case DP650_FORMAT_YVYU:
		argb8888_to_yvyu(TEMP_BUFFER_ADDR, addr_p1, width, height, STRIDE_ALIGN, stride_p1);
		dprint("draw_images: stride = 0x%x\n", *stride_p1);
		break;
	case DP650_FORMAT_UYVY:
		argb8888_to_uyvy(TEMP_BUFFER_ADDR, addr_p1, width, height, STRIDE_ALIGN, stride_p1);
		dprint("draw_images: stride = 0x%x\n", *stride_p1);
		break;
	case DP650_FORMAT_VYUY:
		argb8888_to_vyuy(TEMP_BUFFER_ADDR, addr_p1, width, height, STRIDE_ALIGN, stride_p1);
		dprint("draw_images: stride = 0x%x\n", *stride_p1);
		break;
	default:
		dprint("%s: invalid format 0x%x\n", __func__, format);
		return -1;
	}
	return 0;
}

int32_t compare_buffers(uint32_t src, uint32_t dest, uint32_t width,
							uint32_t height, uint32_t stride, uint32_t format, bool log)
{
	uint32_t var1,var2;
	uint32_t res, i, j;
	uint8_t *mem_ptr1;
	uint8_t *mem_ptr2;
	uint32_t bpp;

	if((DP650_FORMAT_ABGR8888 != format) && (DP650_FORMAT_ARGB8888 != format) &&
		(DP650_FORMAT_BGR888 != format) && (DP650_FORMAT_RGB888 != format)){
		dprint("compare_buffers: only support ARGB8888 & RGB888 & BGR888 (0x%x)\n", format);
		return -1;
	}

	if((DP650_FORMAT_ABGR8888 == format) || (DP650_FORMAT_ARGB8888 == format)){
		bpp = 4;
	} else{
		bpp = 3;
	}

	for(j=0;j<height;j++){
		mem_ptr1 = (uint8_t*)(src + j*stride);
		mem_ptr2 = (uint8_t*)(dest + j*stride);

		//dprint("Compare 0x%x, 0x%x+++, j=%d\n", mem_ptr1, mem_ptr2, j);
		res = memcmp(mem_ptr1,mem_ptr2,width*bpp);
		if(res !=0) {
			res = 0;
			for(i=0;i<width;i++) {
				if(4 == bpp){
					var1 = *(uint32_t*)mem_ptr1;
					var2 = *(uint32_t*)mem_ptr2;
				} else {
					var1 = *mem_ptr1 << 16 | *(mem_ptr1 + 1) << 16 | *(mem_ptr1 + 2);
					var2 = *mem_ptr2 << 16 | *(mem_ptr2 + 1) << 16 | *(mem_ptr2 + 2);
				}
				if((var1 & 0xFFFFFF)!= (var2 & 0xFFFFFF)) {
					if(log){
						dprint("mismatch: Ref data is 0x%x; Ref address is 0x%x.\n",var1,mem_ptr1);
						dprint("		   HW data is	0x%x; HW address is  0x%x.\n",var2,mem_ptr2);
					}
					res = -1;
				}
				mem_ptr1+=bpp;
				mem_ptr2+=bpp;
			}
			if(res != 0){
				if(log){
					dprint("Error during check of bytes from address: %d.\n",src + j*stride);
				}
				return -1;
			}
		}
		//dprint("Compare 0x%x, 0x%x---, j=%d\n", mem_ptr1, mem_ptr2, j);
	}
	return 0;
}
