#include "predefines.h"
#include "PWM.h"
#include "pwm_dec.h"
#include "APBCLK.h"
#include "timer_dec.h"

// PWM4 initialization function
void pwm_init() {
  APBClock->PWM2_CLK_RST |=BIT_2;
  udelay(10);
  APBClock->PWM3_CLK_RST |= BIT_1;
  APBClock->PWM3_CLK_RST |= BIT_4;                                  //32kHz
  APBClock->PWM2_CLK_RST |= BIT_0;
  udelay(10);
  APBClock->PWM2_CLK_RST &= (~BIT_2);
  return;
}

// PWM4 disable function
void pwm_dis() {
  APBClock->PWM2_CLK_RST &= (~BIT_0);
  udelay(10);
  APBClock->PWM3_CLK_RST &= (~BIT_1);
  return;
}

/* PWM4 config function
*  50% duty
*/
void pwm_cfg() {
  PWM4->CRx = 0x0;
  PWM4->DCR = 0x1;
  PWM4->PCR = 0x1;
  return;
}

void pwm_test(int pwm) {
  if(pwm_config(pwm, PWM_CLK_13M, 5000, 10000) == 1) 
    bbu_printf("\r\n"BBU_PROMPT"PWM %d Enable!!\r\n", pwm);
  else 
    bbu_printf("\r\n"BBU_PROMPT"PWM %d ERROR!!\r\n", pwm);
}

/* PWM test function
*  the multi-function pin need to be setted
*  clock can be div by PWM_PRESCALE setting
*  clk_x : 0 - 13M, 1 - 32K
*/
int pwm_config(PwmX pwm_x, PwmClk clk_x, int duty_ns, int period_ns) {
    uint16_t pwm_dcr;
    uint16_t pwm_pcr;
    uint16_t pwm_crx;
    int scaled_count_frequency;
    float pwm_div;

    if(duty_ns > period_ns) {
      bbu_printf("\r\n"BBU_PROMPT"Incorrect input!\r\n");
      return -1;
    }

    if(clk_x == PWM_CLK_13M) scaled_count_frequency = 13000000;
    else if(clk_x == PWM_CLK_32K) scaled_count_frequency = 32000;

    pwm_div = (float)scaled_count_frequency / 1000000000.0;
    pwm_pcr = pwm_div * period_ns - 1;
    pwm_dcr = pwm_div * duty_ns;
    
//    bbu_printf("%d\r\n%d\r\n",pwm_dcr,pwm_pcr);
    if(pwm_pcr >= 1024) {
      bbu_printf("\r\n"BBU_PROMPT"Please ckeck the clock!\r\n");
      return -1;
    }
    pwm_crx = PWM_PRESCALE;

    switch(pwm_x) {
        case PWM_ID_0:
           APBClock->PWM0_CLK_RST |=BIT_2;
           udelay(10);
           APBClock->PWM0_CLK_RST |= BIT_1;
           if(clk_x == PWM_CLK_32K) APBClock->PWM0_CLK_RST |= BIT_4;
           else APBClock->PWM0_CLK_RST &= (~BIT_4);
           APBClock->PWM0_CLK_RST |= BIT_0;
           udelay(10);
           APBClock->PWM0_CLK_RST &= (~BIT_2);
           PWM1->CRx = pwm_crx;
           PWM1->DCR = pwm_dcr;
           PWM1->PCR = pwm_pcr;
           break;
       case PWM_ID_1:
           APBClock->PWM0_CLK_RST |=BIT_2;
           udelay(10);
           APBClock->PWM1_CLK_RST |= BIT_1;
           if(clk_x == PWM_CLK_32K) APBClock->PWM1_CLK_RST |= BIT_4;
           else APBClock->PWM1_CLK_RST &= (~BIT_4);
           APBClock->PWM0_CLK_RST |= BIT_0;
           udelay(10);
           APBClock->PWM0_CLK_RST &= (~BIT_2);
           PWM2->CRx = pwm_crx;
           PWM2->DCR = pwm_dcr;
           PWM2->PCR = pwm_pcr;
           break;
      case PWM_ID_2:
           APBClock->PWM2_CLK_RST |=BIT_2;
           udelay(10);
           APBClock->PWM2_CLK_RST |= BIT_1;
           if(clk_x == PWM_CLK_32K) APBClock->PWM2_CLK_RST |= BIT_4;
           else APBClock->PWM2_CLK_RST &= (~BIT_4);
           APBClock->PWM2_CLK_RST |= BIT_0;
           udelay(10);
           APBClock->PWM2_CLK_RST &= (~BIT_2);
           PWM3->CRx = pwm_crx;
           PWM3->DCR = pwm_dcr;
           PWM3->PCR = pwm_pcr;
           break;
      case PWM_ID_3:
           APBClock->PWM2_CLK_RST |=BIT_2;
           udelay(10);
           APBClock->PWM3_CLK_RST |= BIT_1;
           if(clk_x == PWM_CLK_32K) APBClock->PWM3_CLK_RST |= BIT_4;
           else APBClock->PWM3_CLK_RST &= (~BIT_4); 
           APBClock->PWM2_CLK_RST |= BIT_0;
           udelay(10);
           APBClock->PWM2_CLK_RST &= (~BIT_2);
           PWM4->CRx = pwm_crx;
           PWM4->DCR = pwm_dcr;
           PWM4->PCR = pwm_pcr;
           break;
    }

    bbu_printf("\r\n"BBU_PROMPT"PWM%d Enable!!\r\n", pwm_x);
    return 1;
}

