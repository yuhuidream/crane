#ifndef _DUMMU_FNS_H_
#define _DUMMU_FNS_H_

#if defined(HARDWARE_EVB) //aquilac evb
#include <stdbool.h>
#include "plat_types.h"
#else //crane fpga
#include "predefines.h"
#endif

typedef unsigned long  bool;  /* Machine representation of a boolean */
#define true 1
#define false 0

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

enum dp650_format{
	DP650_FORMAT_ARGB2101010 = 0x0,
	DP650_FORMAT_ARGB8888 = 0x8,
	DP650_FORMAT_ABGR8888 = 0x9,
	DP650_FORMAT_RGB888 = 0x18,
	DP650_FORMAT_BGR888 = 0x19,
	DP650_FORMAT_RGB565 = 0x22,
	DP650_FORMAT_XYUV444_P1 = 0x28,
	DP650_FORMAT_YUV420_P2 = 0x2E,
	DP650_FORMAT_YUV420_P3 = 0x2F,
	DP650_FORMAT_YUYV = 0x30,
	DP650_FORMAT_YVYU = 0x32,
	DP650_FORMAT_UYVY = 0x34,
	DP650_FORMAT_VYUY = 0x38,
	DP650_FORMAT_MAX,
};

int32_t draw_images(uint32_t addr_p1, uint32_t *addr_p2, uint32_t *addr_p3,
					uint32_t format, uint32_t pattern,
					uint32_t width, uint32_t height, bool afbc,
					uint32_t *stride_p1, uint32_t *stride_p2, uint32_t *stride_p3);
int32_t compare_buffers(uint32_t src, uint32_t dest, uint32_t width,
							uint32_t height, uint32_t stride, uint32_t format, bool log);
#endif /*_DUMMU_FNS_H_*/
