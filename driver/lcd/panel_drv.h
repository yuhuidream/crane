#ifndef _PANEL_DRV_H_
#define _PANEL_DRV_H_

#include "predefines.h"
#include "spi_drv.h"
#include "mcu_drv.h"

typedef enum{
	GC9305_MCU = 0,
	GC9305_SPI,
	DUMMY_MCU,
	DUMMY_SPI,
	PANLE_MAX
}PANEL_ID;

enum{
	LCD_CAP_NORMAL = 0,
	LCD_CAP_FAKE = 1,
	LCD_CAP_POWERON = 2,
	LCD_CAP_LIMIT
};

enum{
	LCD_MODE_MCU = 0,
	LCD_MODE_SPI = 1,
	LCD_MODE_LIMIT
};

enum{
	LCD_POLARITY_POS = 0,
	LCD_POLARITY_NEG,
	LCD_POLARITY_LIMIT
};

struct panel_spec;

/* LCD operations */
struct panel_operations {
	int (*panel_interface_init)(struct panel_spec *self, uint32_t sclk);
	int (*panel_init)(struct panel_spec *self);
	int (*panel_invalid)(struct panel_spec *self, uint32_t start_x,
						uint32_t start_y,  uint32_t end_x, uint32_t end_y);
	int (*panel_suspend)(struct panel_spec *self);
	int (*panel_resume)(struct panel_spec *self);
	unsigned int (*panel_readid)(struct panel_spec *self);
};

/* LCD abstraction */
struct panel_spec {
	unsigned int cap;
	unsigned short width;
	unsigned short height;
	unsigned short type; /*mcu, spi*/
	uint16_t te_pol;
	union{
		struct mcu_info *mcu;
		struct spi_info *spi;
	}info;
	void *panel_if;
	struct panel_operations *ops;
};

struct panel_spec* get_panel_info(int id);
int panel_init(struct panel_spec* panel, uint32_t sclk);
int panel_before_refresh(struct panel_spec* panel, uint32_t start_x,
						uint32_t start_y, uint32_t height, uint32_t width);
int panel_after_refresh(struct panel_spec* panel);
void panel_uninit(struct panel_spec *panel);


#endif /*_PANEL_DRV_H_*/
