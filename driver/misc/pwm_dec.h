#ifndef _PWM_DE_H_
#define _PWM_DE_H_

#define PWM_PRESCALE		        0	//CLK div 0~63

typedef enum PwmX {
  PWM_ID_0,
  PWM_ID_1,
  PWM_ID_2,
  PWM_ID_3
}PwmX;

typedef enum PwmClk {
  PWM_CLK_13M,
  PWM_CLK_32K
}PwmClk;

/* Use the VCXO_OUT pin as PWM4
*  PWM4 output wave can be got through the TP422
*/
int pwm_config(PwmX pwm_x, PwmClk clk_x, int duty_ns, int period_ns);
void pwm_dis(void);
void pwm_test(int pwm);
void pwm_cfg(void);
void pwm_init(void);


#endif

