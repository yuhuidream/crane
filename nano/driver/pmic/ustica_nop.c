#include "predefines.h"
#include "pmic.h"
#include "i2c_dec.h"
#include "interrupt.h"
#include "timer_dec.h"

int ustica_base_read(uint8_t reg, uint8_t *val){
   (void)reg;
   (void)val;
   return 0;
}

int ustica_base_write(uint8_t reg, uint8_t val){
   (void)reg;
   (void)val;
   return 0;
}

int ustica_gpadc_read(uint8_t reg, uint8_t *val){
   (void)reg;
   (void)val;
   return 0;
}

int ustica_gpadc_write(uint8_t reg, uint8_t val){
   (void)reg;
   (void)val;
   return 0;
}

int ustica_power_read(uint8_t reg, uint8_t *val){
   (void)reg;
   (void)val;
   return 0;
}

int ustica_power_write(uint8_t reg, uint8_t val){
   (void)reg;
   (void)val;
   return 0;
}

void ustica_power_modify(uint8_t reg, uint8_t val, uint8_t mask){
   (void)reg;
   (void)val;
   (void)mask;
   return;
}

int is_vbuck_enabled(VBUCK_CHANNEL vbuck_channel)
{
   (void)vbuck_channel;
   return 0;
}

int is_ldo_enabled(LDO_CHANNEL ldo_channel)
{
   (void)ldo_channel;
   return 0;
}

int ustica_get_version(void)
{
   return 0;
}

/*****************************************************************************
* Function     :    ustica_ldo_voltage2setting
******************************************************************************
*
* Description  :    Use to get the setting value for LDO Set Register 
                    according to the to be set voltage in mv
* Parameters   :    mv from 600 to 3300

  Return       :    Setting Value for  VBUCK Set Register from 0x0 to 0xF
*
*****************************************************************************/
int ustica_ldo_voltage2setting(LDO_CHANNEL ldo_channel, LDO_VOL vol)
{
   (void)ldo_channel;
   (void)vol;
   int setting = -1;

   return setting;
}

/*****************************************************************************
* Function     :    ustica_ldo_setting2voltage
******************************************************************************
*
* Description  :    Use to get the vlotage according to LDO 
                    Set Register value
* Parameters   :    Setting Value for  VBUCK Set Register from 0x0 to 0xF

  Return       :    mv from 600 to 3300
*
*****************************************************************************/
LDO_VOL ustica_ldo_setting2voltage(LDO_CHANNEL ldo_channel, int  setting)
{
   (void)ldo_channel;
   (void)setting;
   LDO_VOL vol = -1;

   return vol;
}

/*****************************************************************************
* Function     :    ustica_ldo_get_set_reg
******************************************************************************
*
* Description  :    Get LDO set register value

* Parameters   :    (1)ldo_channel: LDO1 ~ LDO19
                    (2)option: Just for LDO1
                                AUDIO,  DVC00(0),DVC01(1),
                                DVC10(2),DVC11(3), SLEEP
  Return       :    LDO set register value
*
*****************************************************************************/
int ustica_ldo_get_set_reg(LDO_CHANNEL ldo_channel, LDO_VBUCK_OPT option)
{
   (void)ldo_channel;
   (void)option;
   return 0;
}

/*****************************************************************************
* Function     :    ustica_ldo1_cfg
******************************************************************************
*
* Description  :    Procida LDO1 config function
*
* Parameters   :    (1)on_off: LDO_ON(1): turn on; LDO_OFF(0): turn off
                    (2)voltage: 600 - 1500 mv
                    (2)option: DVC00, DVC01, DVC10,DVC11,
                               SLEEP,AUDIO
*
*****************************************************************************/
void ustica_ldo1_cfg(int on_off, LDO_VOL voltage, LDO_VBUCK_OPT option){
   (void)on_off;
   (void)voltage;
   (void)option;
   return; 
}


/*****************************************************************************
* Function     :    ustica_ldo_cfg
******************************************************************************
*
* Description  :    Procida LDO config function
*
* Parameters   :    (1)on_off: LDO_ON(1): turn on; LDO_OFF(0): turn off
                    (2)ldo_channel: LDO1~LDO19
                    (3)active_vol: 600 - 3300 (mv) voltage in active mode
                    (4)slp_vol:    600 - 3300 (mv) voltage in sleep mode
*
*****************************************************************************/
void ustica_ldo_cfg(int on_off, LDO_CHANNEL ldo_channel, 
                     LDO_VOL active_vol,LDO_VOL slp_vol){
   (void)on_off;
   (void)ldo_channel;
   (void)active_vol;
   (void)slp_vol;   
   return;
}

/*****************************************************************************
* Function     :    ustica_vbuck_cfg
******************************************************************************
*
* Description  :    Procida VBUCK config function used for VBUCK 2,3,5
*
* Parameters   :    (1)on_off: VBUCK_ON(1): turn on; VBUCK_OFF(0): turn off
                    (2)vbuck_channel: VBUCK2 VBUCK3 VBUCK5
                    (3)active_vol: 600 - 3950 (mv) voltage in active mode
                    (3)slp_vol: 600 - 3950 (mv) voltage in sleep mode
*
*****************************************************************************/
void ustica_vbuck_cfg(int on_off, VBUCK_CHANNEL vbuck_channel, 
                       VBUCK_VOL active_vol, VBUCK_VOL slp_vol){
   (void)on_off;
   (void)vbuck_channel;
   (void)active_vol;
   (void)slp_vol;
   return;   
}

/*****************************************************************************
* Function     :    ustica_vbuck_cfg2
******************************************************************************
*
* Description  :    Procida VBUCK config function used for VBUCK1 and VBUCK4
*
* Parameters   :    (1)on_off: VBUCK_ON(1): turn on; VBUCK_OFF(0): turn off
                    (2)vbuck_channel: VBUCK1, VBUCK4
                    (3)voltage: 600 - 3950 (mv)
                    (3)option: DVC00(0),DVC01(1),DVC10(2),DVC11(3), AUDIO,
                               SLEEP
*
*****************************************************************************/
void ustica_vbuck_cfg2(int on_off, VBUCK_CHANNEL vbuck_channel, 
                       VBUCK_VOL voltage, LDO_VBUCK_OPT option)
{
   (void)on_off;
   (void)vbuck_channel;
   (void)voltage;
   (void)option;
   return; 
}

/*****************************************************************************
* Function     :    ustica_vbuck_misc_cfg
******************************************************************************
*
* Description  :    Procida VBUCK  misc config function
*
* Parameters   :    (1)vbuck_channel: VBUCK1~5
                    (2)dvc_pwm: 1 or 0, 1 means to force the buck 
                           to operate in PWM mode during DVC transients,
                           Just for VBUCK1 and VBUCK2
                    (3)dvc_ramp: 195~25000(uv/us), Control for DVC Ramp Rate
                    (4)dvc_drive: 
                         1)for VBUCK2,3,5, it just has one bit, 0 or 1,
                             0: turn on 50% driver, 1: turn on 100% driver
                         2)for VBUCK1 and VBUCK4, it has 4 bits, each bit
                           indicates full drive(bit = 1) or half drive of 
                           different DVC mode
                           bit0 - DVC2=0, DVC1=0. use DVC00_DRIVE_EN
                           bit1 - DVC2=0, DVC1=1. use DVC01_DRIVE_EN
                           bit2 - DVC2=1, DVC1=0. use DVC10_DRIVE_EN
                           bit3 - DVC2=1, DVC1=1. use DVC11_DRIVE_EN
                           e.g. : DVC00_DRIVE_EN | DVC01_DRIVE_EN
*
*****************************************************************************/
void ustica_vbuck_misc_cfg(VBUCK_CHANNEL vbuck_channel, 
                            int dvc_pwm,
                            int dvc_ramp, 
                            uint32_t dvc_drive)
{
   (void)vbuck_channel;
   (void)dvc_pwm;
   (void)dvc_ramp;
   (void)dvc_drive;
   return;
}

/*****************************************************************************
* Function     :    ustica_vbuck_voltage2setting
******************************************************************************
*
* Description  :    Use to get the setting value for VBUCK Set Register 
                    according to the to be set voltage in mv
* Parameters   :    mv from 600 to 3950

  Return       :    Setting Value for  VBUCK Set Register from 0x00 to 0x7F
*
*****************************************************************************/
uint8_t ustica_vbuck_voltage2setting(uint16_t mv){
   uint8_t value;
   /* Max Vbuck voltage of Emei is 3.3v and Min is 0.6V except vbuck1 */
   if(mv < 600 || mv > 3300)  
      return INVALID_VBUCK_SETTING;
   /* from 0x00 to 0x4F VOUT step is 12.5mv, range is from 0 to 1.6v */
   if(mv <= 1600){
      value = (mv * 10 - 6000) / 125;
   }
   else
      value = ((mv - 1600) / 50) + 0x50;
   return value & 0x7F;
}

/*****************************************************************************
* Function     :    ustica_vbuck_setting2voltage
******************************************************************************
*
* Description  :    Use to get the vlotage according to VBUCK 
                    Set Register value
* Parameters   :    Setting Value for  VBUCK Set Register from 0x00 to 0x7F

  Return       :    mv from 600 to 3950
*
*****************************************************************************/
uint16_t ustica_vbuck_setting2voltage(uint8_t setting){
   uint16_t mv;
   
   if(setting > 0x7F)
      return INVALID_VBUCK_VOL;
   
   if(setting <= 0x50)
      mv = (setting * 125)/10 + 600;
   else
      mv = (setting - 0x50)*50 + 1600;
   return mv;
}

/*****************************************************************************
* Function     :    ustica_get_dvc_ramp_rate_setting
******************************************************************************
*
* Description  :    Use to get the setting value of Buck DVC ramp rate
* Parameters   :    ramp_rate: 195~25000 uV/us

  Return       :    Setting Value for  BKn_DVC_SET 0x0 ~ 0x7
*
*****************************************************************************/
uint8_t ustica_get_dvc_ramp_rate_setting(int ramp_rate)
{
   (void)ramp_rate;
   return 0;
}


/*****************************************************************************
* Function     :    ustica_get_start_ramp_time_setting
******************************************************************************
*
* Description  :    Use to get the setting value of Buck reference soft
                    start ramping time
* Parameters   :    ramp_time: 112 ~ 14336us

  Return       :    Setting Value for  BKn_RAMP_SET 0x0 ~ 0x7
*
*****************************************************************************/
uint8_t ustica_get_start_ramp_time_setting(int ramp_time)
{
   (void)ramp_time;
   return 0;
}

/*****************************************************************************
* Function     :    ustica_vbuck_get_set_reg
******************************************************************************
*
* Description  :    Get Vbuck set register value

* Parameters   :    (1)vbuck_channel: VBUCK1~VBUCK5
                    (2)option: 
                        a) For VBUCK1 and VBUCK4
                               DVC00(0),DVC01(1),DVC10(2),DVC11(3)
                               AUDIO SLEEP
                        b) For VBUCK 2 3 5
                           SLEEP means setting in sleep mode
                           Otherwise means setting in active mode

  Return       :    Vbuck set register value
*
*****************************************************************************/
uint8_t ustica_vbuck_get_set_reg(VBUCK_CHANNEL vbuck_channel, 
                                  LDO_VBUCK_OPT option)
{
   (void)vbuck_channel;
   (void)option;
   return INVALID_VBUCK_SETTING;
}

int ustica_audio_read(uint8_t reg, uint8_t *val){
   (void)reg;
   (void)val;
   return 0;
}

int ustica_audio_write(uint8_t reg, uint8_t val){
   (void)reg;
   (void)val;
   return 0;
}


/*
*pmic interrupt related func
*/		
void pmic_int_enable(e_pmic_int pmic_int_idx)
{
   (void)pmic_int_idx;
   return;
}

void pmic_int_disable(e_pmic_int pmic_int_idx)
{
   (void)pmic_int_idx;
   return;
}
void pmic_isr_connect(e_pmic_int pmic_int_idx, void (*ISR_Handle)() )
{
   (void)pmic_int_idx;
   (void)ISR_Handle;
   return;
}

void pmic_isr_disconnect(e_pmic_int pmic_int_idx, void (*ISR_Handle)() )
{
   (void)pmic_int_idx;
   (void)ISR_Handle;
   return;
}

void pmic_clear_int(e_pmic_int pmic_int_idx)
{
   (void)pmic_int_idx;
   return;
}

void pmic_irq_init(void)
{  
   return;
}
