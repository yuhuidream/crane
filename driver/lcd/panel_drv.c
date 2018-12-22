#include "panel_drv.h"
#include "lcd_drv.h"
#include "lcd_reg.h"

#include "timer_dec.h"
#include "gpio_dec.h"

extern struct panel_spec lcd_gc9305_mcu_spec;
extern struct panel_spec lcd_gc9305_spi_spec;
extern struct panel_spec lcd_dummy_mcu_spec;
extern struct panel_spec lcd_dummy_spi_spec;

struct panel_spec* panels[] = {
	&lcd_gc9305_mcu_spec,
	&lcd_gc9305_spi_spec,
	&lcd_dummy_mcu_spec,
	&lcd_dummy_spi_spec,
};

static void lcd_reset(void)
{
	lcd_set_bits(SMPN_CTRL, BIT_3);
	mdelay(20);
	lcd_clear_bits(SMPN_CTRL, BIT_3);
	mdelay(120);
}

struct panel_spec* get_panel_info(int id)
{
	int panel_id;
	if((id >= 0) && (id < PANLE_MAX))
		panel_id = id;
	else
		panel_id = GC9305_MCU; /*use GC9305 as default*/
	return panels[panel_id];
}

int panel_init(struct panel_spec* panel, uint32_t sclk)
{
	unsigned int id = 0;

	bbu_printf("INFO: panel_init +++\r\n");

	if(NULL == panel){
		bbu_printf("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	/*for fpga only*/
	gpio_init();
	if(panel->type == LCD_MODE_MCU)
		gpio_output_set(21, GPIO_LOW); /*select the fpga output to mcu*/
	else
		gpio_output_set(21, GPIO_HIGH); /*select the fpga output to spi*/

	if(NULL != panel->ops->panel_interface_init){
		panel->ops->panel_interface_init(panel, sclk);
	}

	if(LCD_CAP_FAKE != (panel->cap & LCD_CAP_FAKE)){
		lcd_reset();

#if 1
		if(NULL != panel->ops->panel_readid){
			id = panel->ops->panel_readid(panel);
			bbu_printf("INFO: panel_init: panel id is 0x%x\r\n", id);
		}
#endif

		if(NULL != panel->ops->panel_init){
			panel->ops->panel_init(panel);
		}
	}
	bbu_printf("INFO: panel_init ---\r\n");
	return 0;
}

int panel_before_refresh(struct panel_spec* panel, uint32_t start_x,
						uint32_t start_y, uint32_t height, uint32_t width)
{
	int ret;
	bbu_printf("INFO: panel_before_refresh +++\r\n");

	if(panel->ops->panel_invalid){
		panel->ops->panel_invalid(panel, start_x, start_y, start_x + width -1,
			start_y + height - 1);
	}

	if(panel->type == LCD_MODE_MCU)
		ret = mcu_before_refresh((struct s_mcu_ctx *)panel->panel_if);
	else
		ret = spi_before_refresh((struct s_spi_ctx *)panel->panel_if);
	bbu_printf("INFO: panel_before_refresh ---\r\n");
	return ret;
}

int panel_after_refresh(struct panel_spec* panel)
{
	int ret;
	bbu_printf("INFO: panel_after_refresh +++\r\n");

	if(panel->type == LCD_MODE_MCU)
		ret = mcu_after_refresh((struct s_mcu_ctx *)panel->panel_if);
	else
		ret = spi_after_refresh((struct s_spi_ctx *)panel->panel_if);
	bbu_printf("INFO: panel_after_refresh ---\r\n");
	return ret;
}

void panel_uninit(struct panel_spec *panel)
{
	if(NULL == panel){
		bbu_printf("ERROR: panel_init, Invalid param\r\n");
		return;
	}

	lcd_reset();
}
