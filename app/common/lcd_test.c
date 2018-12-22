#include "lcd_test.h"
#include <string.h>

#include "panel_drv.h"
#include "lcd_drv.h"
#include "lcd_fns.h"

enum{
	LCD_STATUS_STOP,
	LCD_STATUS_RUNNING,
	LCD_STATUS_LIMIT
};

#define SRC_BUFFER_1_ADDR 0xD1200000
//#define SRC_BUFFER_2_ADDR 0xD12A4000
#define SRC_BUFFER_2_ADDR 0xd1278000
#define SRC_BUFFER_YUV420_ADDR 0xD12E1000
#define SRC_BUFFER_WB_ADDR 0xd1296000

#if 0
/*240 * 256 for compose test*/
#define COMPOSE_SIZE 0x3C000
//#define SRC_COM_1_ADDR 0xD12B4000
#define SRC_COM_1_ADDR 0xD1200000
#define SRC_COM_2_ADDR SRC_COM_1_ADDR + COMPOSE_SIZE /*0xd123c000*/
#define SRC_COM_3_ADDR SRC_COM_2_ADDR + COMPOSE_SIZE /*0xd1278000*/
#define SRC_COM_WB_ADDR SRC_COM_3_ADDR + COMPOSE_SIZE /*0xd12B4000*/
#else
/*240 * 100 for compose test*/
#define COMPOSE_SIZE 0x17700
#define SRC_COM_1_ADDR 0xD1200000
#define SRC_COM_2_ADDR SRC_COM_1_ADDR + COMPOSE_SIZE /*0xd1217700*/
#define SRC_COM_3_ADDR SRC_COM_2_ADDR + COMPOSE_SIZE /*0xd122EE00*/
#define SRC_COM_WB_ADDR SRC_COM_3_ADDR + COMPOSE_SIZE /*0xd1246500*/
#endif

int g_lcd_status = LCD_STATUS_STOP;
struct s_lcd_ctx *g_plcd = NULL;
struct panel_spec *g_ppanel = NULL;

static void lcd_test_clean(void)
{
	if((g_lcd_status == LCD_STATUS_RUNNING) && (NULL != g_plcd)
		&& (NULL != g_ppanel)){
		bbu_printf("stop last test first!\r\n ");
		panel_uninit(g_ppanel);
		lcd_uninit(g_plcd);
		lcd_poweroff();
		g_ppanel = NULL;
		g_plcd = NULL;
		g_lcd_status = LCD_STATUS_STOP;
	}
}

static int lcd_test_case1(void)
{
	struct panel_spec *panel = NULL;
	struct s_lcd_ctx *lcd = NULL;
	struct s_layer_setting *layer = NULL;
	int ret = -1;
	uint32_t stride_p1 = 0, stride_p2 = 0, stride_p3 = 0;
	int i = 0;
	uint32_t addr_p2 = 0, addr_p3 = 0;
	uint32_t addr_p2_t = 0, addr_p3_t = 0;
	uint32_t plane = 1;
	uint32_t test_format = LCD_FORMAT_RGB565;
	int dividor = 1;

	bbu_printf("lcd_test_case1 enter!\r\n");

	lcd_test_clean();
	lcd_poweron();
	panel = get_panel_info(GC9305_MCU);
	if(NULL == panel){
		bbu_printf("ERR: get_panel_info fail\r\n");
		return -1;
	}
	g_ppanel = panel;

	lcd = lcd_init();
	if(NULL == lcd){
		bbu_printf("ERR: lcd_init fail\r\n");
		return -1;
	}
	g_plcd = lcd;

	ret = panel_init(panel, 10000);
	if(0 != ret){
		bbu_printf("ERR: panel_init fail\r\n");
		return -1;
	}

	ret = lcd_output_setting(lcd, panel);
	if(0 != ret){
		bbu_printf("ERR: lcd_output_setting fail\r\n");
		return -1;
	}

	bbu_printf("INFO: before lcd_draw_images\r\n");
//	lcd_draw_images(SRC_BUFFER_1_ADDR,NULL,NULL,LCD_FORMAT_RGB1555,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,NULL,NULL,LCD_FORMAT_RGB1555,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
//	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2,&addr_p3,LCD_FORMAT_YUV422_PLANAR,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,&stride_p2,&stride_p3);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,&addr_p2_t,&addr_p3_t,LCD_FORMAT_YUV422_PLANAR,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,&stride_p2,&stride_p3);
//	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2,NULL,LCD_FORMAT_YUV420_SEMI,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,&stride_p2,NULL);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,&addr_p2_t,NULL,LCD_FORMAT_YUV420_SEMI,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,&stride_p2,NULL);
//	lcd_draw_images(SRC_BUFFER_1_ADDR,NULL,NULL,LCD_FORMAT_YUV422_PACKED,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,NULL,NULL,LCD_FORMAT_YUV422_PACKED,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2,&addr_p3,test_format,
		IMG_PATTERN_GREY_LEVEL,panel->width,panel->height,&stride_p1,&stride_p2,&stride_p3);
	lcd_draw_images(SRC_BUFFER_2_ADDR,&addr_p2_t,&addr_p3_t,test_format,
		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,&stride_p2,&stride_p3);
	bbu_printf("INFO: after lcd_draw_images, s1, s2, s3 = %d, %d, %d \r\n", stride_p1,
		stride_p2, stride_p3);

	lcd->bg_color = 0xFF;
	lcd->alpha_mode = LCD_ALPHA_MODE_NORMAL;
	lcd->gamma_en = 0;
	lcd->dither_setting.dither_en = 0;
	lcd->dither_setting.dither_mode = LCD_DITHER_4X8;

	layer = &lcd->layer_setting[LCD_LAYER_OSD1];
	layer->format = test_format;
	layer->alpha_sel = LCD_ALPHA_LAYER;
	layer->layer_alpha = 0xFF;
	layer->src_buffer[0].addr = SRC_BUFFER_1_ADDR;
	layer->src_buffer[0].width = panel->width;//8;//
	layer->src_buffer[0].height = panel->height;//8;//
	layer->src_buffer[0].stride = stride_p1;//32;//
	plane = get_format_plane(test_format);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3;
		layer->src_buffer[2].width = panel->width;
		layer->src_buffer[2].height = panel->height;
		layer->src_buffer[2].stride = stride_p3;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2;
		layer->src_buffer[1].width = panel->width;
		layer->src_buffer[1].height = panel->height;
		layer->src_buffer[1].stride = stride_p2;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 1;
	layer->ck_setting.ck_mode = LCD_CK_MODE_B;
	layer->ck_setting.ck_y.ck_threshold1 = 15;
	layer->ck_setting.ck_y.ck_threshold2 = 30;
	layer->ck_setting.ck_y.ck_dest_value = 0xFF;
	layer->ck_setting.ck_y.ck_dest_alpha = 0xFF;
	layer->ck_setting.ck_u.ck_threshold1 = 15;
	layer->ck_setting.ck_u.ck_threshold2 = 30;
	layer->ck_setting.ck_u.ck_dest_value = 0xFF;
	layer->ck_setting.ck_u.ck_dest_alpha = 0xFF;
	layer->ck_setting.ck_v.ck_threshold1 = 15;
	layer->ck_setting.ck_v.ck_threshold2 = 30;
	layer->ck_setting.ck_v.ck_dest_value = 0xFF;
	layer->ck_setting.ck_v.ck_dest_alpha = 0xFF;
	layer->cbsh_setting.cbsh_en = 0;
	layer->mirror_en = 0;

	dividor = get_uv_height_dividor(layer->format);

#if 1
	for(i=0;i<50;i++){
	if(i%2 ==0){
		lcd->dither_setting.dither_en = 1;
		if(0 == layer->mirror_en){
			layer->src_buffer[0].addr = SRC_BUFFER_1_ADDR;
			if(plane == 3)
				layer->src_buffer[2].addr = addr_p3;
			if(plane != 1)
				layer->src_buffer[1].addr = addr_p2;
		} else { /*mirror*/
			layer->src_buffer[0].addr = SRC_BUFFER_1_ADDR + stride_p1 *
				(panel->height - 1);
			if(plane == 3){
				layer->src_buffer[2].addr = addr_p3 + stride_p3 *
					(panel->height / dividor - 1);
			}
			if(plane != 1)
				layer->src_buffer[1].addr = addr_p2 + stride_p2 *
					(panel->height / dividor - 1);
			bbu_printf("src_addr = 0x%x, 0x%x, 0x%x\r\n", layer->src_buffer[0].addr,
				layer->src_buffer[1].addr, layer->src_buffer[2].addr);
		}
	} else {
		lcd->dither_setting.dither_en = 0;
		if(0 == layer->mirror_en){
			layer->src_buffer[0].addr = SRC_BUFFER_2_ADDR;
			if(plane == 3)
				layer->src_buffer[2].addr = addr_p3_t;
			if(plane != 1)
				layer->src_buffer[1].addr = addr_p2_t;
		} else { /*mirror*/
			layer->src_buffer[0].addr = SRC_BUFFER_2_ADDR + stride_p1 *
				(panel->height - 1);
			if(plane == 3){
				layer->src_buffer[2].addr = addr_p3_t + stride_p3 *
					(panel->height / dividor - 1);
			}
			if(plane != 1)
				layer->src_buffer[1].addr = addr_p2_t + stride_p2 *
					(panel->height / dividor - 1);
		}
	}
	lcd_enable_layer(lcd, LCD_LAYER_OSD1);
	lcd_disable_layer(lcd, LCD_LAYER_OSD2);
	lcd_disable_layer(lcd, LCD_LAYER_IMG);

	bbu_printf("INFO: refresh %d frame!\r\n", i);
	ret = panel_before_refresh(panel, 0, 0, panel->height, panel->width);
	if(0 != ret){
		bbu_printf("ERR: panel_before_refresh fail\r\n");
		return -1;
	}
	ret = lcd_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_refresh fail\r\n");
		return -1;
	}
#if 1
	ret = lcd_after_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_after_refresh fail\r\n");
		return -1;
	}

	ret = panel_after_refresh(panel);
	if(0 != ret){
		bbu_printf("ERR: panel_after_refresh fail\r\n");
		return -1;
	}
#endif
	}
#endif
	return 0;
}

static int lcd_test_case2(void)
{
	struct panel_spec *panel = NULL;
	struct s_lcd_ctx *lcd = NULL;
	struct s_layer_setting *layer = NULL;
	int ret = -1;
	uint32_t stride_p1_c1 = 0, stride_p2_c1 = 0, stride_p3_c1 = 0;
	uint32_t stride_p1_c2 = 0, stride_p2_c2 = 0, stride_p3_c2 = 0;
	uint32_t stride_p1_c3 = 0, stride_p2_c3 = 0, stride_p3_c3 = 0;
	int i = 0;
	uint32_t addr_p2_c1 = 0, addr_p3_c1 = 0;
	uint32_t addr_p2_c2 = 0, addr_p3_c2 = 0;
	uint32_t addr_p2_c3 = 0, addr_p3_c3 = 0;
	uint32_t plane = 1;
	uint32_t test_format_c1 = LCD_FORMAT_RGBA888;/*osd2 layer*/
	uint32_t test_format_c2 = LCD_FORMAT_RGB888A;/*osd1 layer*/
	uint32_t test_format_c3 = LCD_FORMAT_YUV420_PLANAR; /*img layer*/

	bbu_printf("lcd_test_case2 enter!\r\n ");

	lcd_test_clean();
	lcd_poweron();
	panel = get_panel_info(GC9305_SPI);
	if(NULL == panel){
		bbu_printf("ERR: get_panel_info fail\r\n ");
		return -1;
	}
	g_ppanel = panel;

	lcd = lcd_init();
	if(NULL == lcd){
		bbu_printf("ERR: lcd_init fail\r\n ");
		return -1;
	}
	g_plcd = lcd;

	ret = panel_init(panel, 10000);
	if(0 != ret){
		bbu_printf("ERR: panel_init fail\r\n ");
		return -1;
	}

	ret = lcd_output_setting(lcd, panel);
	if(0 != ret){
		bbu_printf("ERR: lcd_output_setting fail\r\n ");
		return -1;
	}

	bbu_printf("INFO: before lcd_draw_images\r\n ");
	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2_c1,&addr_p3_c1,test_format_c1,
		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1_c1,&stride_p2_c1,&stride_p3_c1);
	lcd_draw_images(SRC_BUFFER_2_ADDR,&addr_p2_c2,&addr_p3_c2,test_format_c2,
		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1_c2,&stride_p2_c2,&stride_p3_c2);
	lcd_draw_images(SRC_BUFFER_YUV420_ADDR,&addr_p2_c3,&addr_p3_c3,test_format_c3,
		IMG_PATTERN_VCB_2,panel->width,panel->height,&stride_p1_c3,&stride_p2_c3,&stride_p3_c3);
	bbu_printf("INFO: after lcd_draw_images\r\n ");

	lcd->bg_color = 0xFF;
	lcd->alpha_mode = LCD_ALPHA_MODE_SPECIAL;
	lcd->gamma_en = 0;

	layer = &lcd->layer_setting[LCD_LAYER_IMG];
	layer->format = test_format_c3;
	layer->alpha_sel = LCD_ALPHA_LAYER;
	layer->layer_alpha = 0xFF;
	layer->src_buffer[0].addr = SRC_BUFFER_YUV420_ADDR;
	layer->src_buffer[0].width = panel->width;
	layer->src_buffer[0].height = panel->height;
	layer->src_buffer[0].stride = stride_p1_c3;
	plane = get_format_plane(test_format_c3);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3_c3;
		layer->src_buffer[2].width = panel->width;
		layer->src_buffer[2].height = panel->height;
		layer->src_buffer[2].stride = stride_p3_c3;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2_c3;
		layer->src_buffer[1].width = panel->width;
		layer->src_buffer[1].height = panel->height;
		layer->src_buffer[1].stride = stride_p2_c3;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 0;
	layer->cbsh_setting.cbsh_en = 0;

	layer = &lcd->layer_setting[LCD_LAYER_OSD1];
	layer->format = test_format_c2;
	layer->alpha_sel = LCD_ALPHA_LAYER;
	layer->layer_alpha = 0x80;
	layer->src_buffer[0].addr = SRC_BUFFER_2_ADDR;
	layer->src_buffer[0].width = panel->width;
	layer->src_buffer[0].height = panel->height;
	layer->src_buffer[0].stride = stride_p1_c2;
	plane = get_format_plane(test_format_c2);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3_c2;
		layer->src_buffer[2].width = panel->width;
		layer->src_buffer[2].height = panel->height;
		layer->src_buffer[2].stride = stride_p3_c2;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2_c2;
		layer->src_buffer[1].width = panel->width;
		layer->src_buffer[1].height = panel->height;
		layer->src_buffer[1].stride = stride_p2_c2;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 0;
	layer->cbsh_setting.cbsh_en = 0;

	layer = &lcd->layer_setting[LCD_LAYER_OSD2];
	layer->format = test_format_c1;
	layer->alpha_sel = LCD_ALPHA_LAYER;
	layer->layer_alpha = 0x80;
	layer->src_buffer[0].addr = SRC_BUFFER_1_ADDR;
	layer->src_buffer[0].width = panel->width;
	layer->src_buffer[0].height = panel->height;
	layer->src_buffer[0].stride = stride_p1_c1;
	plane = get_format_plane(test_format_c1);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3_c1;
		layer->src_buffer[2].width = panel->width;
		layer->src_buffer[2].height = panel->height;
		layer->src_buffer[2].stride = stride_p3_c1;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2_c1;
		layer->src_buffer[1].width = panel->width;
		layer->src_buffer[1].height = panel->height;
		layer->src_buffer[1].stride = stride_p2_c1;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 0;
	layer->cbsh_setting.cbsh_en = 0;

#if 1
	lcd_enable_layer(lcd, LCD_LAYER_IMG);
	lcd_enable_layer(lcd, LCD_LAYER_OSD1);
	lcd_enable_layer(lcd, LCD_LAYER_OSD2);

	bbu_printf("INFO: refresh %d frame!\r\n", i);
	ret = panel_before_refresh(panel, 0, 0, panel->height, panel->width);
	if(0 != ret){
		bbu_printf("ERR: panel_before_refresh fail\r\n ");
		return -1;
	}
	ret = lcd_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_refresh fail\r\n ");
		return -1;
	}
#if 1
	ret = lcd_after_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_after_refresh fail\r\n ");
		return -1;
	}

	ret = panel_after_refresh(panel);
	if(0 != ret){
		bbu_printf("ERR: panel_after_refresh fail\r\n ");
		return -1;
	}
#endif
#endif
	return 0;
}

static int lcd_test_case3(void)
{
	struct panel_spec *panel = NULL;
	struct s_lcd_ctx *lcd = NULL;
	struct s_layer_setting *layer = NULL;
	struct s_wb_setting *wb = NULL;
	int ret = -1;
	uint32_t stride_p1 = 0, stride_p2 = 0, stride_p3 = 0;
	int i = 0;
	uint32_t addr_p2 = 0, addr_p3 = 0;
	uint32_t plane = 1;
	uint32_t bpp = 1;
	uint32_t test_format = LCD_FORMAT_RGBA888;
	uint32_t *p32 = NULL;

	bbu_printf("lcd_test_case1 enter!\r\n ");

	if(test_format == LCD_FORMAT_RGB565){
		bpp = 2;
	} else if(test_format == LCD_FORMAT_RGB888_PACKED){
		bpp = 3;
	} else if((test_format == LCD_FORMAT_RGB888A) ||
			(test_format == LCD_FORMAT_RGBA888)){
		bpp = 4;
	} else {
		bbu_printf("ERR: unsupported format (%d)\r\n ", test_format);
		return -1;
	}

	lcd_test_clean();
	lcd_poweron();
	panel = get_panel_info(DUMMY_MCU);
	if(NULL == panel){
		bbu_printf("ERR: get_panel_info fail\r\n ");
		return -1;
	}
	g_ppanel = panel;

	lcd = lcd_init();
	if(NULL == lcd){
		bbu_printf("ERR: lcd_init fail\r\n ");
		return -1;
	}
	g_plcd = lcd;

	ret = panel_init(panel, 10000);
	if(0 != ret){
		bbu_printf("ERR: panel_init fail\r\n ");
		return -1;
	}

	ret = lcd_output_setting(lcd, panel);
	if(0 != ret){
		bbu_printf("ERR: lcd_output_setting fail\r\n ");
		return -1;
	}

	bbu_printf("INFO: before lcd_draw_images\r\n ");
//	lcd_draw_images(SRC_BUFFER_1_ADDR,NULL,NULL,LCD_FORMAT_RGB1555,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,NULL,NULL,LCD_FORMAT_RGB1555,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
//	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2,&addr_p3,LCD_FORMAT_YUV422_PLANAR,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,&stride_p2,&stride_p3);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,&addr_p2_t,&addr_p3_t,LCD_FORMAT_YUV422_PLANAR,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,&stride_p2,&stride_p3);
//	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2,NULL,LCD_FORMAT_YUV420_SEMI,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,&stride_p2,NULL);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,&addr_p2_t,NULL,LCD_FORMAT_YUV420_SEMI,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,&stride_p2,NULL);
//	lcd_draw_images(SRC_BUFFER_1_ADDR,NULL,NULL,LCD_FORMAT_YUV422_PACKED,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,NULL,NULL,LCD_FORMAT_YUV422_PACKED,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2,&addr_p3,test_format,
		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,&stride_p2,&stride_p3);
//	lcd_draw_images(SRC_COM_1_ADDR,&addr_p2,&addr_p3,test_format,
//		IMG_PATTERN_VCB_5,240,256,&stride_p1,&stride_p2,&stride_p3);
//	memset(SRC_BUFFER_2_ADDR, 0x55, panel->width * panel->height * stride_p1);
	memset((void*)SRC_BUFFER_2_ADDR, 0x55, 1000);
	memset((void*)(SRC_BUFFER_2_ADDR + 240*320*4), 0x55, 2000);
	bbu_printf("INFO: after lcd_draw_images\r\n ");

	lcd->bg_color = 0xFF0000;
	lcd->alpha_mode = LCD_ALPHA_MODE_PRE;
	lcd->gamma_en = 0;
	lcd->dither_setting.dither_en = 0;
	lcd->dither_setting.dither_mode = LCD_DITHER_4X8;

	layer = &lcd->layer_setting[LCD_LAYER_IMG];
	layer->format = test_format;
	layer->alpha_sel = LCD_ALPHA_LAYER;
	layer->layer_alpha = 0xFF;
	layer->src_buffer[0].addr = SRC_COM_1_ADDR;//SRC_BUFFER_1_ADDR;
	layer->src_buffer[0].width = panel->width;//240;//100;//8;//
	layer->src_buffer[0].height = panel->height;//256;//100;//8;//
	layer->src_buffer[0].stride = stride_p1;//32;//
	plane = get_format_plane(test_format);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3;
		layer->src_buffer[2].width = panel->width;
		layer->src_buffer[2].height = panel->height;
		layer->src_buffer[2].stride = stride_p3;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2;
		layer->src_buffer[1].width = panel->width;
		layer->src_buffer[1].height = panel->height;
		layer->src_buffer[1].stride = stride_p2;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 0;
	layer->cbsh_setting.cbsh_en = 0;

	wb = &lcd->wb_setting;
	wb->format = test_format;
	wb->dest_buffer.addr = SRC_BUFFER_2_ADDR;//SRC_BUFFER_2_ADDR;
	wb->dest_buffer.height = panel->height;//256;//100;//
	wb->dest_buffer.width = panel->width;//240;//100;//
	wb->dest_buffer.stride = panel->width * bpp;//panel->width * bpp + 8;//temp modify for ASIC bug
	wb->wb_en = 1;

	for(i = 0; i < 2; i++){
#if 1
	lcd_enable_layer(lcd, LCD_LAYER_IMG);
	lcd_disable_layer(lcd, LCD_LAYER_OSD1);
	lcd_disable_layer(lcd, LCD_LAYER_OSD2);

	bbu_printf("INFO: Jessica:refresh %d frame!\r\n", i);
	ret = panel_before_refresh(panel, 0, 0, panel->height, panel->width);
	if(0 != ret){
		bbu_printf("ERR: panel_before_refresh fail\r\n ");
		return -1;
	}
	ret = lcd_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_refresh fail\r\n ");
		return -1;
	}
#if 1
	ret = lcd_after_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_after_refresh fail\r\n ");
		return -1;
	}

	ret = panel_after_refresh(panel);
	if(0 != ret){
		bbu_printf("ERR: panel_after_refresh fail\r\n ");
		return -1;
	}
#endif
	
#if 1
	if(1 == wb->wb_en){
		ret = lcd_compare_buffers(SRC_BUFFER_1_ADDR, SRC_BUFFER_2_ADDR, panel->width, 
			panel->height, stride_p1, panel->width * bpp, test_format, 1);
//		ret = lcd_compare_buffers(SRC_COM_1_ADDR, SRC_BUFFER_2_ADDR, 240, 
//			256, stride_p1, 240 * bpp, test_format, 1);
//		ret = lcd_compare_buffers(SRC_BUFFER_1_ADDR, SRC_BUFFER_2_ADDR, panel->width, 
//			panel->height, stride_p1, panel->width * bpp + 8, test_format, 1);
//		ret = lcd_compare_buffers(SRC_BUFFER_1_ADDR, SRC_BUFFER_2_ADDR, 100, 
//			100, stride_p1, panel->width * bpp + 8, test_format, 1);
		if(0 != ret){
			bbu_printf("ERR: compare buffer fail!\r\n ");
			return -1;
		} else {
			bbu_printf("INO: compare buffer OK!\r\n ");
			memset((void*)SRC_BUFFER_2_ADDR, 0x55, 1000);
		}
	}
#endif
#endif
	}
	return 0;
}

static int lcd_test_case4(void)
{
	struct panel_spec *panel = NULL;
	struct s_lcd_ctx *lcd = NULL;
	struct s_layer_setting *layer = NULL;
	struct s_wb_setting *wb = NULL;
	int ret = -1;
	uint32_t stride_p1 = 0, stride_p2 = 0, stride_p3 = 0;
	int i = 0;
	uint32_t addr_p2 = 0, addr_p3 = 0;
	uint32_t plane = 1;
	uint32_t bpp = 1;
	uint32_t test_format = LCD_FORMAT_RGB565;

	bbu_printf("lcd_test_case1 enter!\r\n ");

	if(test_format == LCD_FORMAT_RGB565){
		bpp = 2;
	} else if(test_format == LCD_FORMAT_RGB888_PACKED){
		bpp = 3;
	} else if((test_format == LCD_FORMAT_RGB888A) ||
			(test_format == LCD_FORMAT_RGBA888)){
		bpp = 4;
	} else {
		bbu_printf("ERR: unsupported format (%d)\r\n ", test_format);
		return -1;
	}

	lcd_test_clean();
	lcd_poweron();
	panel = get_panel_info(GC9305_MCU);
	if(NULL == panel){
		bbu_printf("ERR: get_panel_info fail\r\n ");
		return -1;
	}
	g_ppanel = panel;

	lcd = lcd_init();
	if(NULL == lcd){
		bbu_printf("ERR: lcd_init fail\r\n ");
		return -1;
	}
	g_plcd = lcd;

	ret = panel_init(panel, 10000);
	if(0 != ret){
		bbu_printf("ERR: panel_init fail\r\n ");
		return -1;
	}

	ret = lcd_output_setting(lcd, panel);
	if(0 != ret){
		bbu_printf("ERR: lcd_output_setting fail\r\n ");
		return -1;
	}

	bbu_printf("INFO: before lcd_draw_images\r\n ");
//	lcd_draw_images(SRC_BUFFER_1_ADDR,NULL,NULL,LCD_FORMAT_RGB1555,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,NULL,NULL,LCD_FORMAT_RGB1555,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
//	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2,&addr_p3,LCD_FORMAT_YUV422_PLANAR,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,&stride_p2,&stride_p3);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,&addr_p2_t,&addr_p3_t,LCD_FORMAT_YUV422_PLANAR,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,&stride_p2,&stride_p3);
//	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2,NULL,LCD_FORMAT_YUV420_SEMI,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,&stride_p2,NULL);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,&addr_p2_t,NULL,LCD_FORMAT_YUV420_SEMI,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,&stride_p2,NULL);
//	lcd_draw_images(SRC_BUFFER_1_ADDR,NULL,NULL,LCD_FORMAT_YUV422_PACKED,
//		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
//	lcd_draw_images(SRC_BUFFER_2_ADDR,NULL,NULL,LCD_FORMAT_YUV422_PACKED,
//		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1,NULL,NULL);
	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2,&addr_p3,LCD_FORMAT_RGB888A,
		IMG_PATTERN_GREY_LEVEL,panel->width,panel->height,&stride_p1,&stride_p2,&stride_p3);
//	memset(SRC_BUFFER_2_ADDR, 0x55, panel->width * panel->height * stride_p1);
	memset((void*)SRC_BUFFER_2_ADDR, 0x55, 1000);
	bbu_printf("INFO: after lcd_draw_images\r\n ");

	lcd->bg_color = 0xFF0000;
	lcd->alpha_mode = LCD_ALPHA_MODE_PRE;
	lcd->gamma_en = 0;
	lcd->dither_setting.dither_en = 1;
	lcd->dither_setting.dither_mode = LCD_DITHER_4X8;

	layer = &lcd->layer_setting[LCD_LAYER_IMG];
	layer->format = LCD_FORMAT_RGB888A;
	layer->alpha_sel = LCD_ALPHA_LAYER;
	layer->layer_alpha = 0xFF;
	layer->src_buffer[0].addr = SRC_BUFFER_1_ADDR;
	layer->src_buffer[0].width = panel->width;//100;//8;//
	layer->src_buffer[0].height = panel->height;//100;//8;//
	layer->src_buffer[0].stride = stride_p1;//32;//
	plane = get_format_plane(test_format);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3;
		layer->src_buffer[2].width = panel->width;
		layer->src_buffer[2].height = panel->height;
		layer->src_buffer[2].stride = stride_p3;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2;
		layer->src_buffer[1].width = panel->width;
		layer->src_buffer[1].height = panel->height;
		layer->src_buffer[1].stride = stride_p2;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 0;
	layer->cbsh_setting.cbsh_en = 0;

	wb = &lcd->wb_setting;
	wb->format = test_format;
	wb->dest_buffer.addr = SRC_BUFFER_2_ADDR;
	wb->dest_buffer.height = panel->height;//100;//
	wb->dest_buffer.width = panel->width;//100;//
	wb->dest_buffer.stride = panel->width * bpp;//temp modify for ASIC bug
	wb->wb_en = 1;

#if 1
	lcd_enable_layer(lcd, LCD_LAYER_IMG);
	lcd_disable_layer(lcd, LCD_LAYER_OSD1);
	lcd_disable_layer(lcd, LCD_LAYER_OSD2);

	bbu_printf("INFO: refresh %d frame!\r\n", i);
	ret = panel_before_refresh(panel, 0, 0, panel->height, panel->width);
	if(0 != ret){
		bbu_printf("ERR: panel_before_refresh fail\r\n ");
		return -1;
	}
	ret = lcd_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_refresh fail\r\n ");
		return -1;
	}
#if 1
	ret = lcd_after_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_after_refresh fail\r\n ");
		return -1;
	}

	ret = panel_after_refresh(panel);
	if(0 != ret){
		bbu_printf("ERR: panel_after_refresh fail\r\n ");
		return -1;
	}
#endif
#if 0
	if(1 == wb->wb_en){
//		ret = lcd_compare_buffers(SRC_BUFFER_1_ADDR, SRC_BUFFER_2_ADDR, panel->width, 
//			panel->height, stride_p1, panel->width * bpp + 8, test_format, 1);
		ret = lcd_compare_buffers(SRC_BUFFER_1_ADDR, SRC_BUFFER_2_ADDR, 100, 
			100, stride_p1, panel->width * bpp + 8, test_format, 1);
		if(0 != ret){
			bbu_printf("ERR: compare buffer fail!\r\n ");
			return -1;
		} else {
			bbu_printf("ERR: compare buffer OK!\r\n ");
		}
	}
#endif
#endif
	return 0;
}

static int lcd_test_case5(void)
{
	struct panel_spec *panel = NULL;
	struct s_lcd_ctx *lcd = NULL;
	struct s_layer_setting *layer = NULL;
	int ret = -1;
	uint32_t stride_p1_c1 = 0, stride_p2_c1 = 0, stride_p3_c1 = 0;
	uint32_t stride_p1_c2 = 0, stride_p2_c2 = 0, stride_p3_c2 = 0;
	int i = 0;
	uint32_t addr_p2_c1 = 0, addr_p3_c1 = 0;
	uint32_t addr_p2_c2 = 0, addr_p3_c2 = 0;
	uint32_t plane = 1;
	uint32_t test_format_c1 = LCD_FORMAT_RGB888A;/*osd1 layer*/
	uint32_t test_format_c2 = LCD_FORMAT_YUV420_PLANAR; /*img layer*/

	bbu_printf("lcd_test_case5 enter!\r\n ");

	lcd_test_clean();
	lcd_poweron();
	panel = get_panel_info(GC9305_SPI);
	if(NULL == panel){
		bbu_printf("ERR: get_panel_info fail\r\n ");
		return -1;
	}
	g_ppanel = panel;

	lcd = lcd_init();
	if(NULL == lcd){
		bbu_printf("ERR: lcd_init fail\r\n ");
		return -1;
	}
	g_plcd = lcd;

	ret = panel_init(panel, 10000);
	if(0 != ret){
		bbu_printf("ERR: panel_init fail\r\n ");
		return -1;
	}

	ret = lcd_output_setting(lcd, panel);
	if(0 != ret){
		bbu_printf("ERR: lcd_output_setting fail\r\n ");
		return -1;
	}

	bbu_printf("INFO: before lcd_draw_images\r\n ");
	lcd_draw_images(SRC_BUFFER_1_ADDR,&addr_p2_c1,&addr_p3_c1,test_format_c1,
		IMG_PATTERN_VCB_5,panel->width,panel->height,&stride_p1_c1,&stride_p2_c1,&stride_p3_c1);
	lcd_draw_images(SRC_BUFFER_2_ADDR,&addr_p2_c2,&addr_p3_c2,test_format_c2,
		IMG_PATTERN_HCB_5,panel->width,panel->height,&stride_p1_c2,&stride_p2_c2,&stride_p3_c2);
	bbu_printf("INFO: after lcd_draw_images\r\n ");

	lcd->bg_color = 0xFF;
	lcd->alpha_mode = LCD_ALPHA_MODE_NORMAL;
	lcd->gamma_en = 0;

	layer = &lcd->layer_setting[LCD_LAYER_IMG];
	layer->format = test_format_c2;
	layer->alpha_sel = LCD_ALPHA_LAYER;
	layer->layer_alpha = 0xFF;
	layer->src_buffer[0].addr = SRC_BUFFER_2_ADDR;
	layer->src_buffer[0].width = panel->width;
	layer->src_buffer[0].height = panel->height;
	layer->src_buffer[0].stride = stride_p1_c2;
	plane = get_format_plane(test_format_c2);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3_c2;
		layer->src_buffer[2].width = panel->width;
		layer->src_buffer[2].height = panel->height;
		layer->src_buffer[2].stride = stride_p3_c2;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2_c2;
		layer->src_buffer[1].width = panel->width;
		layer->src_buffer[1].height = panel->height;
		layer->src_buffer[1].stride = stride_p2_c2;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 0;
	layer->cbsh_setting.cbsh_en = 0;

	layer = &lcd->layer_setting[LCD_LAYER_OSD1];
	layer->format = test_format_c1;
	layer->alpha_sel = LCD_ALPHA_PIXEL;
	layer->layer_alpha = 0x80;
	layer->src_buffer[0].addr = SRC_BUFFER_1_ADDR;
	layer->src_buffer[0].width = panel->width;
	layer->src_buffer[0].height = panel->height;
	layer->src_buffer[0].stride = stride_p1_c1;
	plane = get_format_plane(test_format_c1);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3_c1;
		layer->src_buffer[2].width = panel->width;
		layer->src_buffer[2].height = panel->height;
		layer->src_buffer[2].stride = stride_p3_c1;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2_c1;
		layer->src_buffer[1].width = panel->width;
		layer->src_buffer[1].height = panel->height;
		layer->src_buffer[1].stride = stride_p2_c1;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 1;
	layer->ck_setting.ck_mode = LCD_CK_MODE_RGB;
	layer->ck_setting.ck_y.ck_threshold1 = 0;
	layer->ck_setting.ck_y.ck_threshold2 = 0;
	layer->ck_setting.ck_y.ck_dest_value = 0xFF;
	layer->ck_setting.ck_y.ck_dest_alpha = 0x0;
	layer->ck_setting.ck_u.ck_threshold1 = 0;
	layer->ck_setting.ck_u.ck_threshold2 = 0;
	layer->ck_setting.ck_u.ck_dest_value = 0xFF;
	layer->ck_setting.ck_u.ck_dest_alpha = 0x0;
	layer->ck_setting.ck_v.ck_threshold1 = 255;
	layer->ck_setting.ck_v.ck_threshold2 = 255;
	layer->ck_setting.ck_v.ck_dest_value = 0x0;
	layer->ck_setting.ck_v.ck_dest_alpha = 0x0;
	layer->cbsh_setting.cbsh_en = 0;

#if 1
	lcd_enable_layer(lcd, LCD_LAYER_OSD1);
	lcd_enable_layer(lcd, LCD_LAYER_IMG);
	lcd_disable_layer(lcd, LCD_LAYER_OSD2);

	bbu_printf("INFO: refresh %d frame!\r\n", i);
	ret = panel_before_refresh(panel, 0, 0, panel->height, panel->width);
	if(0 != ret){
		bbu_printf("ERR: panel_before_refresh fail\r\n ");
		return -1;
	}
	ret = lcd_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_refresh fail\r\n ");
		return -1;
	}
#if 1
	ret = lcd_after_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_after_refresh fail\r\n ");
		return -1;
	}

	ret = panel_after_refresh(panel);
	if(0 != ret){
		bbu_printf("ERR: panel_after_refresh fail\r\n ");
		return -1;
	}
#endif
#endif
	return 0;
}


static int lcd_test_case6(void)
{
	struct panel_spec *panel = NULL;
	struct s_lcd_ctx *lcd = NULL;
	struct s_layer_setting *layer = NULL;
	struct s_wb_setting *wb = NULL;
	int ret = -1;
	uint32_t stride_p1_c1 = 0, stride_p2_c1 = 0, stride_p3_c1 = 0;
	uint32_t stride_p1_c2 = 0, stride_p2_c2 = 0, stride_p3_c2 = 0;
	uint32_t stride_p1_c3 = 0, stride_p2_c3 = 0, stride_p3_c3 = 0;
	int i = 0;
	uint32_t addr_p2_c1 = 0, addr_p3_c1 = 0;
	uint32_t addr_p2_c2 = 0, addr_p3_c2 = 0;
	uint32_t addr_p2_c3 = 0, addr_p3_c3 = 0;
	uint32_t plane = 1;
	uint32_t test_format_c1 = LCD_FORMAT_RGB888A;/*osd2 layer*/
	uint32_t test_format_c2 = LCD_FORMAT_RGB888A;/*osd1 layer*/
	uint32_t test_format_c3 = LCD_FORMAT_RGB888A; /*img layer*/
	uint32_t test_format_wb = LCD_FORMAT_RGB888A; /*wb*/
	uint32_t test_width = 240;
	uint32_t test_height = 100;
	uint32_t bpp = 1;
	struct s_compose_setting compose_setting;
	struct s_compose_layer_setting *compose_layer = NULL;

	bbu_printf("lcd_test_case6 enter!\r\n ");

	if(test_format_wb == LCD_FORMAT_RGB565){
		bpp = 2;
	} else if(test_format_wb == LCD_FORMAT_RGB888_PACKED){
		bpp = 3;
	} else if((test_format_wb == LCD_FORMAT_RGB888A) ||
			(test_format_wb == LCD_FORMAT_RGBA888)){
		bpp = 4;
	} else {
		bbu_printf("ERR: unsupported format (%d)\r\n ", test_format_wb);
		return -1;
	}

	lcd_test_clean();
	lcd_poweron();
	panel = get_panel_info(DUMMY_MCU);
	if(NULL == panel){
		bbu_printf("ERR: get_panel_info fail\r\n ");
		return -1;
	}
	g_ppanel = panel;
	panel->height = test_height;
	panel->width = test_width;

	lcd = lcd_init();
	if(NULL == lcd){
		bbu_printf("ERR: lcd_init fail\r\n ");
		return -1;
	}
	g_plcd = lcd;

	ret = panel_init(panel, 10000);
	if(0 != ret){
		bbu_printf("ERR: panel_init fail\r\n ");
		return -1;
	}

	ret = lcd_output_setting(lcd, panel);
	if(0 != ret){
		bbu_printf("ERR: lcd_output_setting fail\r\n ");
		return -1;
	}

	bbu_printf("INFO: before lcd_draw_images\r\n ");
//osd2 layer
	lcd_draw_images(SRC_COM_1_ADDR,&addr_p2_c1,&addr_p3_c1,test_format_c1,
		IMG_PATTERN_VCB_5,test_width,test_height,&stride_p1_c1,&stride_p2_c1,&stride_p3_c1);
//osd1 layer
	lcd_draw_images(SRC_COM_2_ADDR,&addr_p2_c2,&addr_p3_c2,test_format_c2,
		IMG_PATTERN_HCB_5,test_width,test_height,&stride_p1_c2,&stride_p2_c2,&stride_p3_c2);
//img layer
	lcd_draw_images(SRC_COM_3_ADDR,&addr_p2_c3,&addr_p3_c3,test_format_c3,
		IMG_PATTERN_VCB_2,test_width,test_height,&stride_p1_c3,&stride_p2_c3,&stride_p3_c3);
//wb buffer
	memset((void*)SRC_COM_WB_ADDR, 0x55, 240*100*4);
	bbu_printf("INFO: after lcd_draw_images\r\n ");

	lcd->bg_color = 0xFF;
	lcd->alpha_mode = LCD_ALPHA_MODE_SPECIAL;
	lcd->gamma_en = 0;

	layer = &lcd->layer_setting[LCD_LAYER_IMG];
	layer->format = test_format_c3;
	layer->alpha_sel = LCD_ALPHA_LAYER;
	layer->layer_alpha = 0x80;
	layer->src_buffer[0].addr = SRC_COM_3_ADDR;//SRC_COM_2_ADDR;//
	layer->src_buffer[0].width = test_width;
	layer->src_buffer[0].height = test_height;
	layer->src_buffer[0].stride = stride_p1_c3;
	plane = get_format_plane(test_format_c3);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3_c3;
		layer->src_buffer[2].width = test_width;
		layer->src_buffer[2].height = test_height;
		layer->src_buffer[2].stride = stride_p3_c3;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2_c3;
		layer->src_buffer[1].width = test_width;
		layer->src_buffer[1].height = test_height;
		layer->src_buffer[1].stride = stride_p2_c3;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 0;
	layer->cbsh_setting.cbsh_en = 0;

	layer = &lcd->layer_setting[LCD_LAYER_OSD1];
	layer->format = test_format_c2;
	layer->alpha_sel = LCD_ALPHA_LAYER;
	layer->layer_alpha = 0x40;
	layer->src_buffer[0].addr = SRC_COM_2_ADDR;//SRC_COM_1_ADDR;//
	layer->src_buffer[0].width = test_width;
	layer->src_buffer[0].height = test_height;
	layer->src_buffer[0].stride = stride_p1_c2;
	plane = get_format_plane(test_format_c2);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3_c2;
		layer->src_buffer[2].width = test_width;
		layer->src_buffer[2].height = test_height;
		layer->src_buffer[2].stride = stride_p3_c2;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2_c2;
		layer->src_buffer[1].width = test_width;
		layer->src_buffer[1].height = test_height;
		layer->src_buffer[1].stride = stride_p2_c2;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 0;
	layer->cbsh_setting.cbsh_en = 0;

	layer = &lcd->layer_setting[LCD_LAYER_OSD2];
	layer->format = test_format_c1;
	layer->alpha_sel = LCD_ALPHA_LAYER;
	layer->layer_alpha = 0x80;
	layer->src_buffer[0].addr = SRC_COM_1_ADDR;
	layer->src_buffer[0].width = test_width;
	layer->src_buffer[0].height = test_height;
	layer->src_buffer[0].stride = stride_p1_c1;
	plane = get_format_plane(test_format_c1);
	if(plane == 3){
		layer->src_buffer[2].addr = addr_p3_c1;
		layer->src_buffer[2].width = test_width;
		layer->src_buffer[2].height = test_height;
		layer->src_buffer[2].stride = stride_p3_c1;
	}
	if(plane != 1){
		layer->src_buffer[1].addr = addr_p2_c1;
		layer->src_buffer[1].width = test_width;
		layer->src_buffer[1].height = test_height;
		layer->src_buffer[1].stride = stride_p2_c1;
	}
	layer->dest_point.x = 0;
	layer->dest_point.y = 0;
	layer->ck_setting.ck_en = 0;
	layer->cbsh_setting.cbsh_en = 0;


	wb = &lcd->wb_setting;
	wb->format = test_format_wb;
	wb->dest_buffer.addr = SRC_COM_WB_ADDR;//SRC_COM_2_ADDR;//
	wb->dest_buffer.height = test_height;//100;//
	wb->dest_buffer.width = test_width;//100;//
	wb->dest_buffer.stride = test_width * bpp;//panel->width * bpp + 8;//temp modify for ASIC bug
	wb->wb_en = 1;

#if 1
	lcd_enable_layer(lcd, LCD_LAYER_OSD2);
	lcd_enable_layer(lcd, LCD_LAYER_OSD1);
	lcd_disable_layer(lcd, LCD_LAYER_IMG);

	if(1 == wb->wb_en){
		compose_setting.width = wb->dest_buffer.width;
		compose_setting.height = wb->dest_buffer.height;
		compose_setting.dest_addr = wb->dest_buffer.addr;
		compose_setting.dest_stride = wb->dest_buffer.stride;
		compose_setting.dest_format = wb->format;
		compose_setting.alpha_mode = lcd->alpha_mode;
		compose_layer = &compose_setting.layer_setting[LCD_LAYER_IMG];
		layer = &lcd->layer_setting[LCD_LAYER_IMG];
		bbu_printf("Jessica: img_layer_en = %d, addr = 0x%x\r\n", (int)layer->layer_en, layer->src_buffer[0].addr);
		compose_layer->layer_en = layer->layer_en;
		if(compose_layer->layer_en){
			compose_layer->addr = layer->src_buffer[0].addr;
			compose_layer->format = layer->format;
			compose_layer->stride = layer->src_buffer[0].stride;
			compose_layer->alpha_sel = layer->alpha_sel;
			compose_layer->layer_alpha = layer->layer_alpha;
		}
		bbu_printf("Jessica: compose_layer_en = %d, addr = 0x%x\r\n",
			(int)compose_layer->layer_en, compose_layer->addr);
		compose_layer = &compose_setting.layer_setting[LCD_LAYER_OSD1];
		layer = &lcd->layer_setting[LCD_LAYER_OSD1];
		compose_layer->layer_en = layer->layer_en;
		if(compose_layer->layer_en){
			compose_layer->addr = layer->src_buffer[0].addr;
			compose_layer->format = layer->format;
			compose_layer->stride = layer->src_buffer[0].stride;
			compose_layer->alpha_sel = layer->alpha_sel;
			compose_layer->layer_alpha = layer->layer_alpha;
		}
		compose_layer = &compose_setting.layer_setting[LCD_LAYER_OSD2];
		layer = &lcd->layer_setting[LCD_LAYER_OSD2];
		compose_layer->layer_en = layer->layer_en;
		if(compose_layer->layer_en){
			compose_layer->addr = layer->src_buffer[0].addr;
			compose_layer->format = layer->format;
			compose_layer->stride = layer->src_buffer[0].stride;
			compose_layer->alpha_sel = layer->alpha_sel;
			compose_layer->layer_alpha = layer->layer_alpha;
		}
	}

	ret = panel_before_refresh(panel, 0, 0, panel->height, panel->width);
	if(0 != ret){
		bbu_printf("ERR: panel_before_refresh fail\r\n ");
		return -1;
	}
	ret = lcd_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_refresh fail\r\n ");
		return -1;
	}
	bbu_printf("INFO: refresh %d frame!\r\n", i);
#if 1
	ret = lcd_after_refresh(lcd);
	if(0 != ret){
		bbu_printf("ERR: lcd_after_refresh fail\r\n ");
		return -1;
	}

	ret = panel_after_refresh(panel);
	if(0 != ret){
		bbu_printf("ERR: panel_after_refresh fail\r\n ");
		return -1;
	}
#endif
#if 1
	if(1 == wb->wb_en){
		ret = lcd_compare_compose_buffer(&compose_setting, 1);
		if(0 != ret){
			bbu_printf("ERR: compare buffer fail!\r\n ");
			return -1;
		} else {
			bbu_printf("INFO: compare buffer OK!\r\n ");
		}
	}
#endif

#endif
	return 0;
}



int lcd_test_entry(int test_case)
{
	int ret;
	switch(test_case){
	case 1:
		ret = lcd_test_case1();
		break;
	case 2:
		ret = lcd_test_case2();
		break;
	case 3:
		ret = lcd_test_case3();
		break;
	case 4:
		ret = lcd_test_case4();
		break;
	case 5:
		ret = lcd_test_case5();
		break;
	case 6:
		ret = lcd_test_case6();
		break;
	default:
		ret = lcd_test_case1();
	}
	return ret;
}

