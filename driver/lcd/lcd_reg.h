#ifndef _LCD_REG_H_
#define _LCD_REG_H_

#include "predefines.h"

#define LCD_BASE_ADDR 0xD420A000
#define LCD_IRQ 42

#define MASK1 0x1
#define MASK2 0x3
#define MASK3 0x7
#define MASK4 0xF
#define MASK5 0x1F
#define MASK6 0x3F
#define MASK7 0x7F
#define MASK8 0xFF
#define MASK9 0x1FF
#define MASK10 0x3FF
#define MASK11 0x7FF
#define MASK12 0xFFF
#define MASK13 0x1FFF
#define MASK14 0x3FFF
#define MASK15 0x7FFF
#define MASK16 0xFFFF
#define MASK17 0x1FFFF
#define MASK18 0x3FFFF
#define MASK19 0x7FFFF
#define MASK20 0xFFFFF
#define MASK21 0x1FFFFF
#define MASK22 0x3FFFFF
#define MASK23 0x7FFFFF
#define MASK24 0xFFFFFF
#define MASK25 0x1FFFFFF
#define MASK26 0x3FFFFFF
#define MASK27 0x7FFFFFF
#define MASK28 0xFFFFFFF
#define MASK29 0x1FFFFFFF
#define MASK30 0x3FFFFFFF
#define MASK31 0x7FFFFFFF
#define MASK32 0xFFFFFFFF

/*OSD2 layer*/
#define TVG_START_ADDR0 0x34
#define TVG_PITCH 0x3C
#define TVG_OVSA_HPXL_VLN 0x40
#define TVG_HPXL_VLN 0x44
#define TVGZM_HPXL_VLN 0x48
#define TV_COLORKEY_Y 0x70
#define TV_COLORKEY_U 0x74
#define TV_COLORKEY_V 0x78
#define TV_CTRL0 0x80
#define TV_CTRL1 0x84
#define TV_CONTRAST 0x88
#define TV_SATURATION 0x8C
#define TV_CBSH_HUE 0x90

/*image layer*/
#define DMA_START_ADDR_Y0 0xC0
#define DMA_START_ADDR_U0 0xC4
#define DMA_START_ADDR_V0 0xC8
#define DMA_PITCH_YC 0xE0
#define DMA_PITCH_UV 0xE4
#define DMA_OVSA_HPXL_VLN 0xE8
#define DMA_HPXL_VLN 0xEC
#define DMAZM_HPXL_VLN 0xF0

/*OSD1 layer*/
#define GRA_START_ADDR0 0xF4
#define GRA_PITCH 0xFC
#define GRA_OVSA_HPXL_VLN 0x100
#define GRA_HPXL_VLN 0x104
#define GRAZM_HPXL_VLN 0x108

#define PN_V_H_ACTIVE 0x118
#define PN_BLANKCOLOR 0x124

#define PN_COLORKEY_Y 0x130
#define PN_COLORKEY_U 0x134
#define PN_COLORKEY_V 0x138

#define SPI_RXDATA 0x140
#define ISA_RXDATA 0x144
#define SPI_CTRL 0x180
#define SPI_TXDATA 0x184
#define SMPN_CTRL 0x188
#define SLV_PORT 0x18C

#define PN_CTRL0 0x190
#define PN_CTRL1 0x194

#define SRAM_CTRL 0x198
#define SRAM_WRDAT 0x19C

#define PN_CONTRAST 0x1AC
#define PN_SATURATION 0x1B0
#define PN_CBSH_HUE 0x1B4

#define IOPAD_CONTROL 0x1BC

#define IRQ_ENA 0x1C0
#define IRQ_ISR_RAW 0x1C4

#define TOP_CTRL 0x1DC

#define AFA_ALL2ONE 0x1E8
#define DITHER_CTRL 0x1EC
#define DITHER_TBL_DATA 0x1F0

#define MISC_CTRL 0x1F8

#define WDMA_CTRL1 0x200
#define WDMA_CTRL2 0x204
#define WDMA_CTRL3 0x208
#define WDMA_CTRL4 0x20C

#define IRQ_TE_DONE BIT_23
#define IRQ_FRAME_DONE BIT_21
#define IRQ_MCU_DONE BIT_19
#define IRQ_SPI_DONE BIT_18
#define IRQ_WB_DONE BIT_14

void lcd_write(uint32_t reg, uint32_t val);
uint32_t lcd_read(uint32_t reg);
void lcd_set_bits(uint32_t reg, uint32_t bits);
void lcd_clear_bits(uint32_t reg, uint32_t bits);
void lcd_write_bits(uint32_t reg, uint32_t value, uint32_t mask, uint32_t shifts);

#endif
