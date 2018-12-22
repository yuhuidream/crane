#include "timer_dec.h"
#include "RTC.h"
#include "APBCLK.h"
#include "pmic.h"
#include "PMUA.h"

static void rtc_irq(void *data){

    (void)data;
//Note: The RTC status register is not correct when wake up from lpm in TTC series 
//So we don't check the interrupt status bit since the interrupt already happened.

    RTC->SR  |= SR_HZ_MASK | SR_AL_MASK; // clear the int 
    RTC->AR = 0;
    RTC->SR &= ~(SR_HZE_MASK | SR_ALE_MASK); //disable the interrupt until enabled again.

    bbu_printf("rtc alarm wakeup at %08X\r\n", RTC->CNR);
    return;
}

void rtc_clk_init(void){
    APBClock->RTC_CLK_RST = RTC_CLK_RST_RST | RTC_CLK_RST_APBCLK;
    volatile int i = 0x200;
    while(i--);
    
    APBClock->RTC_CLK_RST = RTC_CLK_RST_APBCLK | RTC_CLK_RST_FNCLK;
    volatile int j = 0x300;
    while(j--);

    APBClock->RTC_CLK_RST = RTC_CLK_RST_APBCLK | RTC_CLK_RST_FNCLK | RTC_CLK_RST_PM_POWER_SENSOR;
    volatile int k = 0x300;
    while(k--);

    RTC->SR  = SR_HZ_MASK | SR_AL_MASK; // clear the int before enable it
                                                
    ISR_Connect(IRQ_MMP_RTC_ALARM, rtc_irq, 0);
    INT_Enable(IRQ_MMP_RTC_ALARM, 0, 15);
            
    RTC->SR &= ~(SR_HZE_MASK | SR_ALE_MASK);
    RTC->CNR = 0;
    RTC->AR  = 0;
}

void rtc_alarm(uint32_t secs){

    RTC->AR  = RTC->CNR + secs;
    RTC->SR  |= SR_ALE_MASK;
    
    bbu_printf("RTC: alarm setup for %d seconds\r\n", secs);
    return;
}

void rtc_disable(void){
    RTC->SR &= ~(SR_HZE_MASK | SR_ALE_MASK);
    RTC->AR  = 0;
    RTC->SR  = SR_HZ_MASK | SR_AL_MASK; // clear the int first
    return;
}

void rtc_test(int argc, uint8_t** argv){
    if(argc == 1){
        bbu_printf("The interval need to be specified\r\n");
        return;
    }else{
        rtc_alarm(conv_dec((char*)argv[1]));
    }

    return;
}

uint32_t pmic_rtc_read(void){
    unsigned int rtc = 0;
    unsigned char rcr = 0; // rtc counter register
    ustica_base_read(0xD1, &rcr);
    rtc |= rcr;
    ustica_base_read(0xD2, &rcr);
    rtc |= rcr << 8;
    ustica_base_read(0xD3, &rcr);
    rtc |= rcr << 16;
    ustica_base_read(0xD4, &rcr);
    rtc |= rcr << 24;
    return rtc;
}

void pmic_rtc_set_expire(int id, unsigned int val){
    unsigned char reg = 0;
    if(id == 1)
        reg = 0xD5;
    else if(id == 2)
        reg = 0xDD;
    
    ustica_base_write(reg,     val & 0xff);
    ustica_base_write(reg + 1, (val & 0xff00) >> 8);
    ustica_base_write(reg + 2, (val & 0xff0000) >> 16);
    ustica_base_write(reg + 3, (val & 0xff000000) >> 24);
    return;        
}
