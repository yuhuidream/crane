#include<predefines.h>
#include<PMUM.h>
#include "PMUA.h"
#include "timer_dec.h"
#include "smp.h"

static void pc_reset2entry(void *info)
{
   (void)info;
   __asm__ __volatile__ (
		".extern _entry\n"
		"ldr r3, =_entry\n"
		"mov pc, r3\n"
		:
		:
		:"r3"
		);
}

static volatile int disable_feeding_watchdog = 0;
#if 0
void pmic_rtc_wake_pmic(int timeout_sec)
{
		uint8_t reg;
		uint32_t rtc_val = 0;
			
		pm8xx_base_read(0xD1,&reg);
		rtc_val |= (reg << 0);
		//bbu_printf("rtc val: 0x%0X\n\r",reg);
		pm8xx_base_read(0xD2,&reg);
		rtc_val |= (reg << 8);
		//bbu_printf("rtc val: 0x%0X\n\r",reg);
		pm8xx_base_read(0xD3,&reg);
		rtc_val |= (reg << 16);
		//bbu_printf("rtc val: 0x%0X\n\r",reg);
		pm8xx_base_read(0xD4,&reg);
		rtc_val |= (reg << 24);
		//bbu_printf("rtc val: 0x%0X\n\r",reg);
		//bbu_printf("rtc val: 0x%0X\n\r",rtc_val);
			
		rtc_val += timeout_sec;	//4 seconds to expire

		reg = ((rtc_val&0xff) >> 0);
		pm8xx_base_write(0xD5,reg);
		
		reg = ((rtc_val&0xff00) >> 8);
		pm8xx_base_write(0xD6,reg);
		
		reg = ((rtc_val&0xff0000) >> 16);
		pm8xx_base_write(0xD7,reg);
		
		reg = ((rtc_val&0xff000000) >> 24);
		pm8xx_base_write(0xD8,reg);		

		pm8xx_base_read(0xD0,&reg);
		reg |= BIT_0;
		pm8xx_base_write(0xD0,reg);	
}
#endif

void pmic_watchdog_timer_reset(void)
{
	uint8_t reg;
	
	if(disable_feeding_watchdog)
		return;
	
	pm8xx_base_read(0x1D,&reg);
	reg |= BIT_1;
	pm8xx_base_write(0x1D,reg);
	
	//pmic_rtc_wake_pmic(12);
}

void pmic_watchdog_enable(void)
{
	uint8_t reg;
	uint32_t rtc_val = 0;
	bbu_printf("PMIC watchdog reset enable...\n\r");

	//set timer, 8s.
	//reg 0x0e on pm820
	pm8xx_base_read(0x0E,&reg);
	reg &= 0x07;
	reg |=0xC0;
	if(disable_feeding_watchdog == 0)
		reg |= 0x18;
	pm8xx_base_write(0x0E,reg);
	
	//wd_pin_dis
	pm8xx_base_read(0xD0,&reg);
	reg |= BIT_6;
	pm8xx_base_write(0xD0,reg);

#if 0	
	//use RTC to wakeup the PMIC
	if(disable_feeding_watchdog == 1) {	
		pmic_rtc_wake_pmic(4);
	}
#endif
	//watchdog enable
	pm8xx_base_read(0x1D,&reg);
	reg &= ~BIT_0;
	pm8xx_base_write(0x1D,reg);

	//fault_wu_en set as 1
	pm8xx_base_modify(0xE7, BIT_2, BIT_2);
	pm8xx_base_modify(0xE7, BIT_3, BIT_3);
#if 0
	if(disable_feeding_watchdog == 1)		
	{
		pm8xx_base_read(0x14,&reg);
		reg |= BIT_5;
		pm8xx_base_write(0x14,reg);			
	}
#endif
}

void pmic_reset()
{
	bbu_printf("pmic reset\n\r");
	//fault_wu_en set as 1
	pm8xx_base_modify(0xE7, BIT_2, BIT_2);
	pm8xx_base_modify(0xE7, BIT_3, BIT_3);
	mdelay(1);

	//pmic_rtc_wake_pmic(4);

	//sw_pdown, 0x0d on pm820
	pm8xx_base_modify(0x0D, BIT_5, BIT_5);
}

void mmp_arch_reset(char mode)
{
	uint8_t reg;
	
	switch (mode) {
	case 1:
	   	on_each_cpu(pc_reset2entry, 0, 0);
		break;
	case 2:	//w
		bbu_printf("watch dog reset...\n\r");
		mdelay(10);
		do_wdt_reset();
		break;
	case 3:	//g
		//do_gpio_reset();
		break;
	case 4: 
		//pmic watchdog reset
		disable_feeding_watchdog = 1;
		pmic_reset();
		break;
	default:
		break;
	}
}










