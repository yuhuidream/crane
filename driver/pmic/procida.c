#include "predefines.h"
#include "pmic.h"
#include "i2c_dec.h"
#include "interrupt.h"
#include "timer_dec.h"

static int procida_is_vbuck_enabled(VBUCK_CHANNEL vbuck_channel)
{
   if(pmic_is_procida()){
      uint8_t tmp;
      pm8xx_power_read(PROCIDA_BUCK_ENABLE, &tmp);
      if(tmp & PROCIDA_VBUCK_ENABLE_BIT(vbuck_channel))
         return 1;
      return 0;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }   
}

static int procida_is_ldo_enabled(LDO_CHANNEL ldo_channel)
{
   if(pmic_is_procida()){
      uint8_t tmp;
      pm8xx_power_read(PROCIDA_LDO_ENABLE_REG(ldo_channel), &tmp);
      if(tmp & PROCIDA_LDO_ENABLE_BIT(ldo_channel))
         return 1;
      return 0;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }  
}

static void procida_ldo_enable(LDO_CHANNEL ldo_channel){
   if(pmic_is_procida()){
      if(!procida_is_ldo_enabled(ldo_channel)){
         pm8xx_power_modify(PROCIDA_LDO_ENABLE_REG(ldo_channel), 
                              PROCIDA_LDO_ENABLE_BIT(ldo_channel), 0);
         if(ldo_channel == LDO8){
            mdelay(1);
            i2c_init(PROCIDA_I2C);
         }
      }
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }     
   return;
}

static void procida_ldo_disable(LDO_CHANNEL ldo_channel){
   if(pmic_is_procida()){
      if(procida_is_ldo_enabled(ldo_channel))
         pm8xx_power_modify(PROCIDA_LDO_ENABLE_REG(ldo_channel), 0, 
                              PROCIDA_LDO_ENABLE_BIT(ldo_channel));
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
   return;
}

static void procida_vbuck_enable(VBUCK_CHANNEL vbuck_channel){
   if(pmic_is_procida()){
      if(!procida_is_vbuck_enabled(vbuck_channel))
         pm8xx_power_modify(PROCIDA_BUCK_ENABLE, 
                              PROCIDA_VBUCK_ENABLE_BIT(vbuck_channel), 0);
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
}

static void procida_vbuck_disable(VBUCK_CHANNEL vbuck_channel){
   if(pmic_is_procida()){
      if(procida_is_vbuck_enabled(vbuck_channel))
         pm8xx_power_modify(PROCIDA_BUCK_ENABLE, 0,
                              PROCIDA_VBUCK_ENABLE_BIT(vbuck_channel));
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
}


/*****************************************************************************
* Function     :    procida_ldo_voltage2setting
******************************************************************************
*
* Description  :    Use to get the setting value for LDO Set Register 
                    according to the to be set voltage in mv
* Parameters   :    mv from 600 to 3300

  Return       :    Setting Value for  VBUCK Set Register from 0x0 to 0xF
*
*****************************************************************************/
static int procida_ldo_voltage2setting(LDO_CHANNEL ldo_channel, LDO_VOL vol)
{
   int setting = -1;
   if(pmic_is_procida()){
      if(ldo_channel == LDO1)
      {
         if     (vol >= LDO0V6 && vol < LDO0V65)  setting = 0x0;
         else if(vol >= LDO0V65 && vol < LDO0V7 ) setting = 0x1;
         else if(vol >= LDO0V7  && vol < LDO0V75) setting = 0x2;
         else if(vol >= LDO0V75 && vol < LDO0V8 ) setting = 0x3;
         else if(vol >= LDO0V8  && vol < LDO0V85) setting = 0x4;
         else if(vol >= LDO0V85 && vol < LDO0V9 ) setting = 0x5;
         else if(vol >= LDO0V9  && vol < LDO0V95) setting = 0x6;
         else if(vol >= LDO0V95 && vol < LDO1V0 ) setting = 0x7;
         else if(vol >= LDO1V0  && vol < LDO1V05) setting = 0x8;
         else if(vol >= LDO1V05 && vol < LDO1V1 ) setting = 0x9;
         else if(vol >= LDO1V1  && vol < LDO1V15) setting = 0xA;
         else if(vol >= LDO1V15 && vol < LDO1V2 ) setting = 0xB;
         else if(vol >= LDO1V2  && vol < LDO1V3 ) setting = 0xC;
         else if(vol >= LDO1V3  && vol < LDO1V4 ) setting = 0xD;
         else if(vol >= LDO1V4  && vol < LDO1V5 ) setting = 0xE;
         else if(vol >= LDO1V5)                   setting = 0xF;
         else bbu_printf("The voltage is out of range\n\r");
      }
      else if(ldo_channel == LDO2) /* In Procida C0, LDO2 is changed */
      {
         if     (vol >= LDO1V7  && vol < LDO1V8 ) setting = 0x0;
         else if(vol >= LDO1V8  && vol < LDO1V9 ) setting = 0x1;
         else if(vol >= LDO1V9  && vol < LDO2V0 ) setting = 0x2;
         else if(vol >= LDO2V0  && vol < LDO2V1 ) setting = 0x3;
         else if(vol >= LDO2V1  && vol < LDO2V5 ) setting = 0x4;
         else if(vol >= LDO2V5  && vol < LDO2V7 ) setting = 0x5;
         else if(vol >= LDO2V7  && vol < LDO2V8 ) setting = 0x6;
         else if(vol >= LDO2V8)                   setting = 0x7;
         else bbu_printf("The voltage is out of range\n\r");
      }
      if(ldo_channel > LDO2 && ldo_channel < LDO18)
      {
         if     (vol >= LDO1V2  && vol < LDO1V25) setting = 0x0;
         else if(vol >= LDO1V25 && vol < LDO1V7 ) setting = 0x1;
         else if(vol >= LDO1V7  && vol < LDO1V8 ) setting = 0x2;
         else if(vol >= LDO1V8  && vol < LDO1V85) setting = 0x3;
         else if(vol >= LDO1V85 && vol < LDO1V9 ) setting = 0x4;
         else if(vol >= LDO1V9  && vol < LDO2V5 ) setting = 0x5;
         else if(vol >= LDO2V5  && vol < LDO2V6 ) setting = 0x6;
         else if(vol >= LDO2V6  && vol < LDO2V7 ) setting = 0x7;
         else if(vol >= LDO2V7  && vol < LDO2V75) setting = 0x8;
         else if(vol >= LDO2V75 && vol < LDO2V8 ) setting = 0x9;
         else if(vol >= LDO2V8  && vol < LDO2V85) setting = 0xA;
         else if(vol >= LDO2V85 && vol < LDO2V9 ) setting = 0xB;
         else if(vol >= LDO2V9  && vol < LDO3V0 ) setting = 0xC;
         else if(vol >= LDO3V0  && vol < LDO3V1 ) setting = 0xD;
         else if(vol >= LDO3V1  && vol < LDO3V3 ) setting = 0xE;
         else if(vol >= LDO3V3)                   setting = 0xF;
         else bbu_printf("The voltage is out of range\n\r");
      }
      else if(ldo_channel >= LDO18)
      {
         if     (vol >= LDO1V7  && vol < LDO1V8) setting = 0x0;
         else if(vol >= LDO1V8  && vol < LDO1V9) setting = 0x1;
         else if(vol >= LDO1V9  && vol < LDO2V5) setting = 0x2;
         else if(vol >= LDO2V5  && vol < LDO2V8) setting = 0x3;
         else if(vol >= LDO2V8  && vol < LDO2V9) setting = 0x4;
         else if(vol >= LDO2V9  && vol < LDO3V1) setting = 0x5;
         else if(vol >= LDO3V1  && vol < LDO3V3) setting = 0x6;
         else if(vol >= LDO3V3)                  setting = 0x7;
         else bbu_printf("The voltage is out of range\n\r");
      }
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
   return setting;
}

/*****************************************************************************
* Function     :    procida_ldo_setting2voltage
******************************************************************************
*
* Description  :    Use to get the vlotage according to LDO 
                    Set Register value
* Parameters   :    Setting Value for  VBUCK Set Register from 0x0 to 0xF

  Return       :    mv from 600 to 3300
*
*****************************************************************************/
static LDO_VOL procida_ldo_setting2voltage(LDO_CHANNEL ldo_channel, int  setting)
{
   LDO_VOL vol = -1;
   if(pmic_is_procida()){
      uint8_t setting_tmp;
      if(ldo_channel == LDO1)
      {
         setting_tmp = setting & 0xF;
         switch(setting_tmp)
         {
            case 0x0: vol = LDO0V6 ; break;
            case 0x1: vol = LDO0V65; break;
            case 0x2: vol = LDO0V7 ; break;
            case 0x3: vol = LDO0V75; break;
            case 0x4: vol = LDO0V8 ; break;
            case 0x5: vol = LDO0V85; break;
            case 0x6: vol = LDO0V9 ; break;
            case 0x7: vol = LDO0V95; break;
            case 0x8: vol = LDO1V0 ; break;
            case 0x9: vol = LDO1V05; break;
            case 0xA: vol = LDO1V1 ; break;
            case 0xB: vol = LDO1V15; break;
            case 0xC: vol = LDO1V2 ; break;
            case 0xD: vol = LDO1V3 ; break;
            case 0xE: vol = LDO1V4 ; break;
            case 0xF: vol = LDO1V5 ; break;
            default: bbu_printf("The setting is out of range\n\r");
               break;
         }
      }
      else if(ldo_channel == LDO2) /* In Procida C0, LDO2 is changed */
      {
         setting_tmp = setting & 0x7;
         switch(setting_tmp)
         {
            case 0x7: vol = LDO2V8 ; break;
            case 0x6: vol = LDO2V7 ; break;
            case 0x5: vol = LDO2V5 ; break;
            case 0x4: vol = LDO2V1 ; break;
            case 0x3: vol = LDO2V0 ; break;
            case 0x2: vol = LDO1V9 ; break;
            case 0x1: vol = LDO1V8 ; break;
            case 0x0: vol = LDO1V7 ; break;
            default: bbu_printf("The setting is out of range\n\r");
               break;
         }
      }
      else if(ldo_channel > LDO2 && ldo_channel < LDO18)
      {
         setting_tmp = setting & 0xF;
         switch(setting_tmp)
         {
            case 0x0: vol = LDO1V2 ; break;
            case 0x1: vol = LDO1V25; break;
            case 0x2: vol = LDO1V7 ; break;
            case 0x3: vol = LDO1V8 ; break;
            case 0x4: vol = LDO1V85; break;
            case 0x5: vol = LDO1V9 ; break;
            case 0x6: vol = LDO2V5 ; break;
            case 0x7: vol = LDO2V6 ; break;
            case 0x8: vol = LDO2V7 ; break;
            case 0x9: vol = LDO2V75; break;
            case 0xA: vol = LDO2V8 ; break;
            case 0xB: vol = LDO2V85; break;
            case 0xC: vol = LDO2V9 ; break;
            case 0xD: vol = LDO3V0 ; break;
            case 0xE: vol = LDO3V1 ; break;
            case 0xF: vol = LDO3V3 ; break;
            default: bbu_printf("The setting is out of range\n\r");
               break;
         }
      }
      else if(ldo_channel >= LDO18)
      {
         setting_tmp = setting & 0x7;
         switch(setting_tmp)
         {
            case 0x0: vol = LDO1V7 ; break;
            case 0x1: vol = LDO1V8 ; break;
            case 0x2: vol = LDO1V9 ; break;
            case 0x3: vol = LDO2V5 ; break;
            case 0x4: vol = LDO2V8 ; break;
            case 0x5: vol = LDO2V9 ; break;
            case 0x6: vol = LDO3V1 ; break;
            case 0x7: vol = LDO3V3 ; break;
            default: bbu_printf("The setting is out of range\n\r");
               break;
         }
      }
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
   return vol;
}

/*****************************************************************************
* Function     :    procida_ldo_get_set_reg
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
static int procida_ldo_get_set_reg(LDO_CHANNEL ldo_channel, LDO_VBUCK_OPT option)
{
   uint8_t setting = 0;
   if(pmic_is_procida()){
      if(ldo_channel == LDO1)
      {
         switch(option)
         {
            case SLEEP:
               pm8xx_power_read(PROCIDA_LDO1_1, &setting);
               setting >>= 4;
               break;
            case DVC00:
               pm8xx_power_read(PROCIDA_LDO1_1, &setting);
               setting &= 0xF;
               break;
            case DVC10:
               pm8xx_power_read(PROCIDA_LDO1_2, &setting);
               setting >>= 4;
               break;
            case DVC01:
               pm8xx_power_read(PROCIDA_LDO1_2, &setting);
               setting &= 0xF;
               break;
            case AUDIO:
               pm8xx_power_read(PROCIDA_LDO1_3, &setting);
               setting >>= 4;
               break;
            case DVC11:
               pm8xx_power_read(PROCIDA_LDO1_3, &setting);
               setting &= 0xF;
               break;
            default:
               bbu_printf("The option is not correct\n\r");
               return -1; 
               break;
         }
      }
      else if(ldo_channel <= LDO19 && ldo_channel > LDO1){
         pm8xx_power_read(PROCIDA_LDO_SET(ldo_channel), &setting);
         if(option == SLEEP){
            setting >>= 4;
         }else{
            setting &= 0xF;
         }
      }
      else
         bbu_printf("The LDO channel does not exist\n\r");
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
   return (int)setting;
}

static int procida_ldo_get_vol(LDO_CHANNEL ldo_channel, LDO_VBUCK_OPT option)
{
   int setting = procida_ldo_get_set_reg(ldo_channel, option);
   return (int)procida_ldo_setting2voltage(ldo_channel, setting);
}

/*****************************************************************************
* Function     :    procida_ldo1_cfg
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
static void procida_ldo_cfg(int on_off, LDO_CHANNEL ldo_channel,
                            LDO_VOL voltage, LDO_VBUCK_OPT option){
   if(pmic_is_procida()){
      int set_val;
      if(on_off){
         set_val = procida_ldo_voltage2setting(ldo_channel, voltage);
         if(set_val == -1)
            return;
         set_val &= 0xF;
         if(ldo_channel == LDO1){
            switch(option){
               case SLEEP:
                  pm8xx_power_modify(PROCIDA_LDO1_1, (set_val << 4), 0xF0);
                  break;
               case DVC00:
                  pm8xx_power_modify(PROCIDA_LDO1_1, set_val, 0x0F);
                  break;
               case DVC10:
                  pm8xx_power_modify(PROCIDA_LDO1_2, (set_val << 4), 0xF0);
                  break;
               case DVC01:
                  pm8xx_power_modify(PROCIDA_LDO1_2, set_val, 0x0F);
                  break;
               case AUDIO:
                  pm8xx_power_modify(PROCIDA_LDO1_3, (set_val << 4), 0xF0);
                  break;
               case DVC11:
                  pm8xx_power_modify(PROCIDA_LDO1_3, set_val, 0x0F);
                  break;
               default:
                  bbu_printf("The option is not correct\n\r");
                  return;
                  break;
            }
         }else if(ldo_channel <= LDO19){
            switch(option){
               case ACTIVE:
                  pm8xx_power_modify(PROCIDA_LDO_SET(ldo_channel), set_val, 0x0F);
                  break;
               case SLEEP:
                  pm8xx_power_modify(PROCIDA_LDO_SET(ldo_channel), (set_val << 4), 0xF0);
                  break;
               default:
                  bbu_printf("The option is not correct\n\r");
                  return;
                  break;
            }
         }else{
            bbu_printf("The LDO%d is not supported\n\r", ldo_channel);
         }
         procida_ldo_enable(ldo_channel);
      }
      else
         procida_ldo_disable(ldo_channel);
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
}


/*****************************************************************************
* Function     :    procida_vbuck_voltage2setting
******************************************************************************
*
* Description  :    Use to get the setting value for VBUCK Set Register 
                    according to the to be set voltage in mv
* Parameters   :    mv from 600 to 3950

  Return       :    Setting Value for  VBUCK Set Register from 0x00 to 0x7F
*
*****************************************************************************/
static uint8_t procida_vbuck_voltage2setting(uint16_t mv){
   if(pmic_is_procida()){
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
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }
}

/*****************************************************************************
* Function     :    procida_vbuck_cfg2
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
static void procida_vbuck_cfg(int on_off, VBUCK_CHANNEL vbuck_channel, 
                              VBUCK_VOL voltage, LDO_VBUCK_OPT option)
{
   if(pmic_is_procida()){
      uint8_t setting;
      if(on_off){
         setting = procida_vbuck_voltage2setting(voltage);
         if(setting == INVALID_VBUCK_SETTING)
            return;
         switch(vbuck_channel){
            case VBUCK1:{
               switch(option){
                  case AUDIO:
                     pm8xx_power_modify(PROCIDA_AUDIO_MODE_CFG1, setting,0x7F);
                     break;
                  case SLEEP:
                     pm8xx_power_modify(PROCIDA_VBUCK1_SLEEP, setting,0x7F);
                     break;
                  case DVC00:
                     pm8xx_power_modify(PROCIDA_VBUCK1_SET0, setting,0x7F);
                     break;
                  case DVC01:
                     pm8xx_power_modify(PROCIDA_VBUCK1_SET1, setting,0x7F);
                     break;
                  case DVC10:
                     pm8xx_power_modify(PROCIDA_VBUCK1_SET2, setting,0x7F);
                     break;
                  case DVC11:
                     pm8xx_power_modify(PROCIDA_VBUCK1_SET3, setting,0x7F);
                     break;
                  default:
                     bbu_printf("Vbuck%d doesn't support option%d\n\r", vbuck_channel, option);
                     break;
               }
               break;
            }
            case VBUCK4:{
               switch(option){
                  case AUDIO:
                     pm8xx_power_modify(PROCIDA_AUDIO_MODE_CFG2, setting,0x7F);
                     break;
                  case SLEEP:
                     pm8xx_power_modify(PROCIDA_VBUCK4_SLEEP, setting,0x7F);
                     break;
                  case DVC00:
                     pm8xx_power_modify(PROCIDA_VBUCK4_SET0, setting,0x7F);
                     break;
                  case DVC01:
                     pm8xx_power_modify(PROCIDA_VBUCK4_SET1, setting,0x7F);
                     break;
                  case DVC10:
                     pm8xx_power_modify(PROCIDA_VBUCK4_SET2, setting,0x7F);
                     break;
                  case DVC11:
                     pm8xx_power_modify(PROCIDA_VBUCK4_SET3, setting,0x7F);
                     break;
                  default:
                     bbu_printf("Vbuck%d doesn't support option%d\n\r", vbuck_channel, option);
                     break;
               }
               break;
            }
            case VBUCK2:{
               switch(option){
                  case ACTIVE:
                     pm8xx_power_write(PROCIDA_VBUCK2_SET,   setting);
                     break;
                  case SLEEP:
                     /* Can't directly write as Bit7 of PROCIDA_VBUCK2_SLEEP is not
                        reserved any more */
                     pm8xx_power_modify(PROCIDA_VBUCK2_SLEEP, setting, 0x7F);
                     break;
                  default:
                     bbu_printf("Vbuck%d doesn't support option%d\n\r", vbuck_channel, option);
                     break;
               }
               break;
            }
            case VBUCK3:{
               switch(option){
                  case ACTIVE:
                     pm8xx_power_write(PROCIDA_VBUCK3_SET,   setting);
                     break;
                  case SLEEP:
                     /* Can't directly write as Bit7 of PROCIDA_VBUCK2_SLEEP is not
                        reserved any more */
                     pm8xx_power_modify(PROCIDA_VBUCK3_SLEEP, setting, 0x7F);
                     break;
                  default:
                     bbu_printf("Vbuck%d doesn't support option%d\n\r", vbuck_channel, option);
                     break;
               }
               break;
            }
            case VBUCK5:{
               switch(option){
                  case ACTIVE:
                     pm8xx_power_write(PROCIDA_VBUCK5_SET,   setting);
                     break;
                  case SLEEP:
                     /* Can't directly write as Bit7 of PROCIDA_VBUCK2_SLEEP is not
                        reserved any more */
                     pm8xx_power_modify(PROCIDA_VBUCK5_SLEEP, setting, 0x7F);
                     break;
                  default:
                     bbu_printf("Vbuck%d doesn't support option%d\n\r", vbuck_channel, option);
                     break;
               }
               break;
            }
            default:
               bbu_printf("This VBUCK%d not exist\n\r", vbuck_channel);
               return;
               break;
         }
         procida_vbuck_enable(vbuck_channel);
      }else
         procida_vbuck_disable(vbuck_channel);
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
   return; 
}


/*****************************************************************************
* Function     :    procida_vbuck_get_set_reg
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
static uint8_t procida_vbuck_get_set_reg(VBUCK_CHANNEL vbuck_channel, 
                                  LDO_VBUCK_OPT option)
{
   uint8_t setting = INVALID_VBUCK_SETTING;
   if(pmic_is_procida()){
      switch(vbuck_channel){
         case VBUCK1:
            if(option <= DVC11)
               pm8xx_power_read(PROCIDA_VBUCK1_SET(option), &setting);
            else if(option == AUDIO)
               pm8xx_power_read(PROCIDA_AUDIO_MODE_CFG1, &setting);
            else if(option == SLEEP)
               pm8xx_power_read(PROCIDA_VBUCK1_SLEEP, &setting);
            break;
         case VBUCK2:
            if(option == SLEEP)
               pm8xx_power_read(PROCIDA_VBUCK2_SLEEP, &setting);
            else
               pm8xx_power_read(PROCIDA_VBUCK2_SET, &setting);
            break;
         case VBUCK3:
            if(option == SLEEP)
               pm8xx_power_read(PROCIDA_VBUCK3_SLEEP, &setting);
            else
               pm8xx_power_read(PROCIDA_VBUCK3_SET, &setting);
            break;
         case VBUCK4:
            if(option <= DVC11)
               pm8xx_power_read(PROCIDA_VBUCK4_SET(option), &setting);
            else if(option == AUDIO)
               pm8xx_power_read(PROCIDA_AUDIO_MODE_CFG2, &setting);
            else if(option == SLEEP)
               pm8xx_power_read(PROCIDA_VBUCK4_SLEEP, &setting);
            break;
         case VBUCK5:
            if(option == SLEEP)
               pm8xx_power_read(PROCIDA_VBUCK5_SLEEP, &setting);
            else
               pm8xx_power_read(PROCIDA_VBUCK5_SET, &setting);
            break;
         default:
            return INVALID_VBUCK_SETTING;
            break;
      }
      if(setting != INVALID_VBUCK_SETTING)
         setting &= 0x7F;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
   return setting;
}
/*****************************************************************************
* Function     :    procida_vbuck_setting2voltage
******************************************************************************
*
* Description  :    Use to get the vlotage according to VBUCK 
                    Set Register value
* Parameters   :    Setting Value for  VBUCK Set Register from 0x00 to 0x7F

  Return       :    mv from 600 to 3950
*
*****************************************************************************/
static uint16_t procida_vbuck_setting2voltage(uint8_t setting){
   if(pmic_is_procida()){
      uint16_t mv;
      
      if(setting > 0x7F)
         return INVALID_VBUCK_VOL;
      
      if(setting <= 0x50)
         mv = (setting * 125)/10 + 600;
      else
         mv = (setting - 0x50)*50 + 1600;
      return mv;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }
}

static int procida_vbuck_get_vol(VBUCK_CHANNEL vbuck_channel, LDO_VBUCK_OPT option)
{
   uint8_t setting = procida_vbuck_get_set_reg(vbuck_channel, option);
   return (int)procida_vbuck_setting2voltage(setting);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
* Function     :    procida_get_dvc_ramp_rate_setting
******************************************************************************
*
* Description  :    Use to get the setting value of Buck DVC ramp rate
* Parameters   :    ramp_rate: 195~25000 uV/us

  Return       :    Setting Value for  BKn_DVC_SET 0x0 ~ 0x7
*
*****************************************************************************/
uint8_t procida_get_dvc_ramp_rate_setting(int ramp_rate)
{
   uint8_t setting = INVALID_VBUCK_SETTING;
   if(pmic_is_procida()){
      if(ramp_rate < 195 || ramp_rate > 25000)
         return INVALID_VBUCK_SETTING;
      if(ramp_rate >= 195 && ramp_rate < 390)          setting = 0x0;
      else if(ramp_rate >= 390 && ramp_rate < 780)     setting = 0x1;
      else if(ramp_rate >= 780 && ramp_rate < 1560)    setting = 0x2;
      else if(ramp_rate >= 1560 && ramp_rate < 3120)   setting = 0x3;
      else if(ramp_rate >= 3120 && ramp_rate < 6250)   setting = 0x4;
      else if(ramp_rate >= 6250 && ramp_rate < 12500)  setting = 0x5;
      else if(ramp_rate >= 12500 && ramp_rate < 25000) setting = 0x6;
      else if(ramp_rate >= 25000)                      setting = 0x7;
      else bbu_printf("Procida: the ramp rate is out of range\n\r");
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
   return setting;
}


/*****************************************************************************
* Function     :    procida_get_start_ramp_time_setting
******************************************************************************
*
* Description  :    Use to get the setting value of Buck reference soft
                    start ramping time
* Parameters   :    ramp_time: 112 ~ 14336us

  Return       :    Setting Value for  BKn_RAMP_SET 0x0 ~ 0x7
*
*****************************************************************************/
uint8_t procida_get_start_ramp_time_setting(int ramp_time)
{
   uint8_t setting = INVALID_VBUCK_SETTING;
   if(pmic_is_procida()){
      if(ramp_time < 112 || ramp_time > 14336)
         return INVALID_VBUCK_SETTING;
      if(ramp_time >= 112 && ramp_time < 224)          setting = 0x0;
      else if(ramp_time >= 224 && ramp_time < 448)     setting = 0x1;
      else if(ramp_time >= 448 && ramp_time < 896)     setting = 0x2;
      else if(ramp_time >= 896 && ramp_time < 1792)    setting = 0x3;
      else if(ramp_time >= 1792 && ramp_time < 3584)   setting = 0x4;
      else if(ramp_time >= 3584 && ramp_time < 7168)   setting = 0x5;
      else if(ramp_time >= 7168 && ramp_time < 14336)  setting = 0x6;
      else if(ramp_time >= 14336)                      setting = 0x7;
      else bbu_printf("Procida: the ramp time is out of range\n\r");
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
   return setting;
}


/*****************************************************************************
* Function     :    procida_vbuck_misc_cfg
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
void procida_vbuck_misc_cfg(VBUCK_CHANNEL vbuck_channel, 
                            int dvc_pwm,
                            int dvc_ramp, 
                            uint32_t dvc_drive)
{
   if(pmic_is_procida()){
      uint8_t dvc_set, setting;
      dvc_set  = procida_get_dvc_ramp_rate_setting(dvc_ramp);
      setting = ((dvc_set & 0x7) << 3);
   
      switch(vbuck_channel){
         case VBUCK1:
            setting |= ((dvc_pwm   & 0x1) << 6);
            /* Mask 0x78, bit6(pwm) and bit5:3(dvc_set)  */
            pm8xx_power_modify(PROCIDA_VBUCK1_MISC_REG1, setting, 0x78);
            setting = dvc_drive & 0xF;
            pm8xx_power_modify(PROCIDA_DRIVE_SELECT_REG, setting, 0xF);
            break;
         case VBUCK2:
            setting |= ((dvc_pwm   & 0x1) << 6);
            /* Mask 0x78, bit6(pwm) and bit5:3(dvc_set)  */
            pm8xx_power_modify(PROCIDA_VBUCK2_MISC_REG1, setting, 0x78);
            setting = (dvc_drive & 0x1) << 4;
            pm8xx_power_modify(PROCIDA_VBUCK2_MISC_REG3, setting, BIT_4);
            break;
         case VBUCK3:
            /* Mask 0x38, bit5:3(dvc_set)  */
            pm8xx_power_modify(PROCIDA_VBUCK3_MISC_REG1, setting, 0x38);
            setting = (dvc_drive & 0x1) << 4;
            pm8xx_power_modify(PROCIDA_VBUCK3_MISC_REG3, setting, BIT_4);
            break;
         case VBUCK4:
         	/* Mask 0x38, bit5:3(dvc_set)  */
            pm8xx_power_modify(PROCIDA_VBUCK4_MISC_REG1, setting, 0x38);
            setting = (dvc_drive & 0xF) << 4;
            pm8xx_power_modify(PROCIDA_DRIVE_SELECT_REG, setting, 0xF0);
            break;
         case VBUCK5:
         	/* Mask 0x38, bit5:3(dvc_set)  */
            pm8xx_power_modify(PROCIDA_VBUCK5_MISC_REG1, setting, 0x38);
            setting = (dvc_drive & 0x1) << 4;
            pm8xx_power_modify(PROCIDA_VBUCK5_MISC_REG3, setting, BIT_4);
            break;
         default:
            break;
      }
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
}

int procida_get_version(void)
{
   if(pmic_is_procida()){
      uint8_t version;
      pm8xx_base_read(0x0, &version); //identification register 0x0
      return version;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }
}

struct pmic_callback procida_callback = {
	 .__pmic_ldo_cfg = procida_ldo_cfg,
	 .__pmic_vbuck_cfg = procida_vbuck_cfg,
	 .__pmic_vbuck_get_vol = procida_vbuck_get_vol,
	 .__pmic_ldo_get_vol = procida_ldo_get_vol,
	 .__pmic_is_vbuck_on = procida_is_vbuck_enabled,
	 .__pmic_is_ldo_on  = procida_is_ldo_enabled,
};

extern struct pmic_int_desc *pmic_irq_array;
static struct pmic_int_desc procida_irq_array[]={
		[PMIC_CLASSD_OC_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PROCIDA_CLASSD_OC_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_RTC_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PROCIDA_RTC_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_BAT_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PROCIDA_BAT_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_CHG_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PROCIDA_CHG_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_EXTON_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PROCIDA_EXTON_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_ONKEY_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PROCIDA_ONKEY_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_TINT_INT]={
			.stat_reg_offset =0x6,
			.en_reg_offset =0x0A,
			.pmic_int_mask=PROCIDA_TINT_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_VCHG_INT]={
			.stat_reg_offset =0x6,
			.en_reg_offset =0x0A,
			.pmic_int_mask=PROCIDA_VCHG_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_VSYS_INT]={
			.stat_reg_offset =0x6,
			.en_reg_offset =0x0A,
			.pmic_int_mask=PROCIDA_VSYS_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_VBAT_INT]={
			.stat_reg_offset =0x6,
			.en_reg_offset =0x0A,
			.pmic_int_mask=PROCIDA_VBAT_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPADC4_INT]={
			.stat_reg_offset =0x7,
			.en_reg_offset =0x0B,
			.pmic_int_mask=PROCIDA_GPADC4_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPADC3_INT]={
			.stat_reg_offset =0x7,
			.en_reg_offset =0x0B,
			.pmic_int_mask=PROCIDA_GPADC3_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPADC2_INT]={
			.stat_reg_offset =0x7,
			.en_reg_offset =0x0B,
			.pmic_int_mask=PROCIDA_GPADC2_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPADC1_INT]={
			.stat_reg_offset =0x7,
			.en_reg_offset =0x0B,
			.pmic_int_mask=PROCIDA_GPADC1_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPADC0_INT]={
			.stat_reg_offset =0x7,
			.en_reg_offset =0x0B,
			.pmic_int_mask=PROCIDA_GPADC0_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPIO4_INT]={
			.stat_reg_offset =0x8,
			.en_reg_offset =0x0C,
			.pmic_int_mask=PROCIDA_GPIO4_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPIO3_INT]={
			.stat_reg_offset =0x8,
			.en_reg_offset =0x0C,
			.pmic_int_mask=PROCIDA_GPIO3_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPIO2_INT]={
			.stat_reg_offset =0x8,
			.en_reg_offset =0x0C,
			.pmic_int_mask=PROCIDA_GPIO2_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPIO1_INT]={
			.stat_reg_offset =0x8,
			.en_reg_offset =0x0C,
			.pmic_int_mask=PROCIDA_GPIO1_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPIO0_INT]={
			.stat_reg_offset =0x8,
			.en_reg_offset =0x0C,
			.pmic_int_mask=PROCIDA_GPIO0_INT_MASK,
			.irq_valid = 0,
			.pmic_isr_handler=NULL,
		},	
};

static void (*pmicISR_Handle)(void *);

static void procida_irq_handler(void *arg)
{
   int i;
   uint8_t value,reg_off;	
   if(pmic_is_procida()){
#if (0)
      bbu_printf("TWSI_CR= 0x%x\r\n", MRVL_I2C3->ICR);
      bbu_printf("TWSI_SR= 0x%x\r\n", MRVL_I2C3->ISR);
   
      while (BIT_2 & (MRVL_I2C3->ISR));
   
      if(!pm8xx_base_read(0x1,&value))while(1);
   
      bbu_printf("pmic 0x1 status register = %d\r\n", value);
#endif
      reg_off = procida_irq_array[PMIC_CLASSD_OC_INT].stat_reg_offset;
      pm8xx_base_read(reg_off,&value);
      for(i = 0; i < MAX_PMIC_INT; i++)
      {
	   	if(procida_irq_array[i].irq_valid == 0 
			|| procida_irq_array[i].stat_reg_offset != reg_off
			|| 0 == (procida_irq_array[i].pmic_int_mask&value))
   			continue;
   		pmic_clear_int(i);
     		/* get ISR entry from table and call it */
          	pmicISR_Handle =  (void (*)())(procida_irq_array[i].pmic_isr_handler);
          	if ( pmicISR_Handle )
             		(pmicISR_Handle)(arg);   
      }
      reg_off = procida_irq_array[PMIC_TINT_INT].stat_reg_offset;
      pm8xx_base_read(reg_off,&value);
      for(i = 0; i < MAX_PMIC_INT; i++)
      {
	   	if(procida_irq_array[i].irq_valid == 0 
			|| procida_irq_array[i].stat_reg_offset != reg_off
			|| 0 == (procida_irq_array[i].pmic_int_mask&value))
   			continue;
   		pmic_clear_int(i);
     		/* get ISR entry from table and call it */
          	pmicISR_Handle =  (void (*)())(procida_irq_array[i].pmic_isr_handler);
          	if ( pmicISR_Handle )
             		(pmicISR_Handle)(arg);   
      }
      reg_off = procida_irq_array[PMIC_GPADC4_INT].stat_reg_offset;
      pm8xx_base_read(reg_off,&value);
      for(i = 0; i < MAX_PMIC_INT; i++)
      {
	   	if(procida_irq_array[i].irq_valid == 0 
			|| procida_irq_array[i].stat_reg_offset != reg_off
			|| 0 == (procida_irq_array[i].pmic_int_mask&value))
   			continue;
   		pmic_clear_int(i);
     		/* get ISR entry from table and call it */
          	pmicISR_Handle =  (void (*)())(procida_irq_array[i].pmic_isr_handler);
          	if ( pmicISR_Handle )
             		(pmicISR_Handle)(arg);   
      }
      reg_off = procida_irq_array[PMIC_GPIO4_INT].stat_reg_offset;
      pm8xx_base_read(reg_off,&value);
      for(i = 0; i < MAX_PMIC_INT; i++)
      {
	   	if(procida_irq_array[i].irq_valid == 0 
			|| procida_irq_array[i].stat_reg_offset != reg_off
			|| 0 == (procida_irq_array[i].pmic_int_mask&value))
   			continue;
   		pmic_clear_int(i);
     		/* get ISR entry from table and call it */
          	pmicISR_Handle =  (void (*)())(procida_irq_array[i].pmic_isr_handler);
          	if ( pmicISR_Handle )
             		(pmicISR_Handle)(arg);   
      }
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }   
}

void procida_irq_init(void)
{  
   if(pmic_is_procida()){
      pmic_irq_array = procida_irq_array;
      ISR_Connect(IRQ_MMP_PMIC_INT, procida_irq_handler, NULL);
      INT_Enable(IRQ_MMP_PMIC_INT, 0, 15);
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
}

void procida_dev_init(void)
{
	uint8_t val,chip_id,reg;

    	bbu_printf("procida device init ...\n\r");
    	//set POWER_HOLD to enable DVC and reset audio related registers to zero
    	//NOTE: this should be before buck1 DVC init, otherwise the the Vcc_main 
    	//will be set to DVC00 voltage, causing boot hang.
    	pm8xx_base_modify(0x0D, 0x80, 0x80);

    	//Disable PMIC watchdog
    	pm8xx_base_write(0x1D, 0x01);

    	//Increase the current limit of Buck1, otherwise PMIC can't survive mlucas+GC MMO6 stress.
    	//set VBUCK1 to full drive,VBUCK1->VCC_MAIN
    	pm8xx_power_modify(0x3A, 0x0f, 0x0f);

    	//SLEEP mode, set VBUCK1 to sleep mode when sleepn is deasserted
    	//VBUCK2,VBUCK3,VBUCK4,VBUCK5: always active, LDO2: always active
    	pm8xx_power_modify(0x5A, 0x01, 0x3);
   	//pm8xx_power_write(0x5B, 0x03);

    	//tune the drive setting for the buck1 to get high efficency during different DVCs, ramp rates,etc.
    	//ustica_vbuck_misc_cfg(VBUCK1,0,12500,DVC00_DRIVE_EN|DVC01_DRIVE_EN|DVC10_DRIVE_EN|DVC11_DRIVE_EN);

    	//set POWER_HOLD to enalbe DVC and reset audio related registers to zero
    	//pm8xx_base_write(0x0D, 0xC0);


    	//set USE_XO=1 to indicate that the 32k XO is settled (usually 1s after SOD power-up,
    	//bootrom/obm should cost that already, so we don't wait here.)
    	//According to PMIC datasheet chapter 5.2 table 24: the 32k XO Tstart = 1s typical and 10s maximum.
    	pm8xx_base_modify(0xD0, 0x80, 0x80);

    	//and disable FLL OSC_FREERUN mode to lock PMIC bucks/etc to use the XO clock instead
    	pm8xx_base_modify(0x50, 0x0, 0x3); //OSC_FREERUN ->0
    	pm8xx_base_modify(0x55, 0x0, 0x3); //OSCD_FREERUN ->0

    	//select the internal filter capacitance value to 22pF for XTAL1 and XTAL2 pins so that 32KHz is stable
    	#if 1  //for some customer like SS has external capacitor, this setting will cause PMIC deadlock
    	pm8xx_base_modify(0xE8, 0x70, 0x70);
    	#endif

    	//32k_OUTx_SEL: output PMIC internal 32KHz or 32KHz from XO (if USE_XO=1). also enable Power up good monoitor
    	//The set of 32k_OUT_SEL is kept in "power-down" state, it resets to default only in RTC POR.
    	pm8xx_base_modify(0xE2, 0x15, 0x3f);

    	//set MEAS_EN_SLP to enable GPADC low power mode to save power
    	pm8xx_gpadc_modify(0x6, 0x33, 0x33);
    	pm8xx_gpadc_modify(0x2, 0x30, 0x30);

#ifdef CONFIG_MMT
    	pm830_bat_prio_en(1);
#endif
}

