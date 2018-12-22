#include "predefines.h"
#include "timer_dec.h"

#define PMU_BASE_ADDR 0xd4282800
#define MULTIMEDIA_CLK_RES_CTRL0 0x4C
#define MULTIMEDIA_CLK_RES_CTRL1 0x50

//#define PMU_CTRL0_VALUE 0x7027F
#define PMU_CTRL0_VALUE 0x7003F
//#define PMU_CTRL1_VALUE 0x2
#define PMU_CTRL1_VALUE 0xC1C1033f

void lcd_poweron(void)
{
	uint32_t reg = 0;
	bbu_printf("lcd_poweron ++\r\n");
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, PMU_CTRL0_VALUE);
	reg = BU_REG_READ(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0);
/*	while (reg != PMU_CTRL0_VALUE){
		udelay(10);
		bbu_printf("wait for power on!\r\n");
		reg = BU_REG_READ(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0);
	}
*/
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1, PMU_CTRL1_VALUE);
	bbu_printf("lcd_poweron 0x%x = 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0,
		reg);
	reg = BU_REG_READ(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1);
/*
	while (reg != PMU_CTRL1_VALUE){
		udelay(10);
		bbu_printf("wait for power on!\r\n");
		reg = BU_REG_READ(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1);
	}
*/
	bbu_printf("lcd_poweron 0x%x = 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1,
		reg);
}

void lcd_poweroff(void)
{
	bbu_printf("lcd_poweroff ++\r\n");
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, 0);
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1, 0);
	bbu_printf("lcd_poweroff --\r\n");
}

