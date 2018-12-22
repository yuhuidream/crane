#include<predefines.h>
#include<PMUM.h>
#include "PMUA.h"
#include "timer_dec.h"

#ifndef CONFIG_NANO
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
#endif

void pmic_reset()
{
	bbu_printf("pmic reset\n\r");
	//fault_wu_en set as 1
	ustica_base_modify(0xE7, BIT_2, BIT_2);
	ustica_base_modify(0xE7, BIT_3, BIT_3);
	mdelay(1);

	//pmic_rtc_wake_pmic(4);

	//sw_pdown, 0x0d on pm820
	ustica_base_modify(0x0D, BIT_5, BIT_5);
}

void mmp_arch_reset(char mode)
{
	switch (mode) {
#ifndef CONFIG_NANO
	case 1:
	   on_each_cpu(pc_reset2entry, 0, 0);
		break;
#endif
	case 2:	//w
		pmic_reset();
		//if pmic reset fails, then call wdt reset
		mdelay(1000);
		bbu_printf("watch dog reset...\n\r");
		mdelay(10);
		do_wdt_reset();
		break;
#ifndef CONFIG_NANO
	case 3:	//g
		//do_gpio_reset();
#endif
	default:
		break;
	}
}

int bbu_reset(int argc, uint8_t **argv)
{
    mmp_arch_reset(2);
}
