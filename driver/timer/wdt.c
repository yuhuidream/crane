#include "bbu.h"
#include "PMU_TIME.h"
#include "PMUM.h"
#include "APBCLK.h"
#include "pmic.h"

#define WDT_GEN_INT     0
#define WDT_GEN_RESET   1

#define WDT_TIME    (( PMU_TIME_TypeDef *) 0xD4080000)

static void wdt_access(void)
{
    WDT_TIME->WFAR = 0xbaba;
    WDT_TIME->WSAR = 0xeb10;
}

static void wdt_enable(unsigned int reset_int)
{
    wdt_access();
    WDT_TIME->WMER = (reset_int << 1) | 0x1;                
}

static void wdt_set_match(unsigned int match)
{
    wdt_access();
    WDT_TIME->WMR = 0xffff & match;
}

static void wdt_reset_counter(void)
{
    wdt_access();
    WDT_TIME->WCR = 0x1;
}

static void wdt_irq(void *data)
{
    data = data;
    wdt_access();
    WDT_TIME->WICR = 1; // clear wdt int
    wdt_access();
    bbu_printf("%s: wvr 0x%08x, match 0x%08x, wsr %d\r\n",__FUNCTION__, WDT_TIME->WVR, WDT_TIME->WMR, WDT_TIME->WSR);
    wdt_access();
    WDT_TIME->WMER &= ~0x1; // disable wdt
}

void wdt_test(unsigned int seconds, unsigned int reset_int)
{
    // wdt 13Mhz
    PMUM->WDTPCR = 0x7;
    udelay(10);
    PMUM->WDTPCR = 0x3;
    udelay(100);
    PMUM->APRR |= 0x10;

    ISR_Connect(IRQ_MMP_WDT, wdt_irq, NULL);
    INT_Enable(IRQ_MMP_WDT, IRQ_ROUTE, 15);
    
    wdt_reset_counter();
    // the wdt timer seems only use 256hz
    // see the emei register TimersCP section for details
    wdt_set_match(seconds * 256);
    wdt_enable(reset_int);
    
    //write PMIC blank register to indicate watchdog reset.
    //bbu_printf("set 0xEF to 1\n\r");
    if(platform_is_realsi())
	   pm8xx_base_write(0xEF, 0x1);
}

void do_wdt_reset(void)
{
	wdt_test(2, WDT_GEN_RESET);
}

#ifdef CONFIG_MMT
void wdt_reset(){
    wdt_reset_counter();
}

void wdt_start(int sec){

    *(int *) (0xd401e080) = 0xD0C1;
	*(int*)(0xd401900c ) &=  ~(0x1<<3);
	udelay(1);	//can't be removed.
	wdt_test(sec, WDT_GEN_RESET);
}
#endif
