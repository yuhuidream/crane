#include "predefines.h"
#include "i2c_dec.h"
#include "interrupt.h"
#include "timer_dec.h"
#include "pmic.h"
#include "ustica.h"

extern int ustica_available;
#if 0
static uint8_t ustica_ldo_set_reg[USTICA_LDO_NUM+1] = {
    [LDO1]= USTICA_LDO1,
    [LDO2]= USTICA_LDO2,  
    [LDO3]= USTICA_LDO3,
    [LDO4]= USTICA_LDO4,  
    [LDO5]= USTICA_LDO5,
    [LDO6]= USTICA_LDO6,  
};
static ustica_ldo_en_reg[USTICA_LDO_NUM+1] = 
{
    [LDO1]= USTICA_LDO_ENABLE_REG1,
    [LDO2]= USTICA_LDO_ENABLE_REG1,  
    [LDO3]= USTICA_LDO_ENABLE_REG2,
    [LDO4]= USTICA_LDO_ENABLE_REG2,  
    [LDO5]= USTICA_LDO_ENABLE_REG3,
    [LDO6]= USTICA_LDO_ENABLE_REG3,  
};

static ustica_ldo_en_bit[USTICA_LDO_NUM+1] = 
{
    [LDO1]= BIT_2,
    [LDO2]= BIT_4,  
    [LDO3]= BIT_4,
    [LDO4]= BIT_3,  
    [LDO5]= BIT_1,
    [LDO6]= BIT_2,  
};
#define USTICA_LDO_SET(x)                   ((ustica_ldo_set_reg[x])&0xFF)
#define USTICA_LDO_ENABLE_REG(x)            ((ustica_ldo_set_reg[x])&0xFF)
#define USTICA_LDO_ENABLE_BIT(x)            ((ustica_ldo_set_reg[x])&0xFF)
#endif

int ustica_base_read(uint8_t reg, uint8_t *val){
   if(ustica_available){
      int ret;
      ret = i2c_read_addr8_byte(USTICA_I2C, USTICA_BASE_ADDRESS, 
                             reg, val, ICR_MODE_STD);
      if(ret == 0) 
         bbu_printf("ustica: i2c rd fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
      return 0;
   }
}

void ustica_base_modify(uint8_t reg, uint8_t val, uint8_t mask){
   if(ustica_available){
      uint8_t tmp;
      ustica_base_read(reg, &tmp);
      tmp &= ~mask;
      tmp |= val;
      ustica_base_write(reg, tmp);
      return;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
      return;
   }    
}

int ustica_base_write(uint8_t reg, uint8_t val){
   if(ustica_available){
      int ret;
      ret =i2c_write_addr8_byte(USTICA_I2C, USTICA_BASE_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      if(ret == 0)
         bbu_printf("ustica: i2c wr fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
      return 0;
   }   
}

int ustica_power_read(uint8_t reg, uint8_t *val){
   if(ustica_available){
      int ret;
      ret = i2c_read_addr8_byte(USTICA_I2C, USTICA_POWER_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      if(ret == 0) 
         bbu_printf("ustica: i2c read fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
      return 0;
   }       
}

int ustica_power_write(uint8_t reg, uint8_t val){
   if(ustica_available){
      int ret;
      ret =i2c_write_addr8_byte(USTICA_I2C, USTICA_POWER_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      if(ret == 0)
         bbu_printf("ustica: i2c wr fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
      return 0;
   }      
}

void ustica_power_modify(uint8_t reg, uint8_t val, uint8_t mask){
   if(ustica_available){
      uint8_t tmp;
      ustica_power_read(reg, &tmp);
      tmp &= ~mask;
      tmp |= val;
      ustica_power_write(reg, tmp);
      return;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
      return;
   }    
}

int ustica_is_vbuck_enabled(VBUCK_CHANNEL vbuck_channel)
{
   if(ustica_available){
      uint8_t tmp;
      ustica_power_read(USTICA_BUCK_ENABLE, &tmp);
      if(tmp & USTICA_VBUCK_ENABLE_BIT(vbuck_channel))
         return 1;
      return 0;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
      return 0;
   }   
}
static int ustica_is_ldo_enabled(LDO_CHANNEL ldo_channel)
{

}
static int ustica_ldo_get_vol(LDO_CHANNEL ldo_channel, LDO_VBUCK_OPT option)
{

}

#if 0
static int ustica_is_ldo_enabled(LDO_CHANNEL ldo_channel)
{
   if(ustica_available){
      uint8_t tmp;
      ustica_power_read(USTICA_LDO_ENABLE_REG(ldo_channel), &tmp);
      if(tmp & USTICA_LDO_ENABLE_BIT(ldo_channel))
         return 1;
      return 0;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
      return 0;
   }  
}

static void ustica_ldo_enable(LDO_CHANNEL ldo_channel){
   if(ustica_available){
      if(!ustica_is_ldo_enabled(ldo_channel)){
         ustica_power_modify(USTICA_LDO_ENABLE_REG(ldo_channel), 
                              USTICA_LDO_ENABLE_BIT(ldo_channel), 0);
         if(ldo_channel == LDO8){
            mdelay(1);
            i2c_init(USTICA_I2C);
         }
      }
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }     
   return;
}

static void ustica_ldo_disable(LDO_CHANNEL ldo_channel){
   if(ustica_available){
      if(ustica_is_ldo_enabled(ldo_channel))
         ustica_power_modify(USTICA_LDO_ENABLE_REG(ldo_channel), 0, 
                              USTICA_LDO_ENABLE_BIT(ldo_channel));
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
   return;
}
#endif

static void ustica_vbuck_enable(VBUCK_CHANNEL vbuck_channel){
   if(ustica_available){
      if(!ustica_is_vbuck_enabled(vbuck_channel))
         ustica_power_modify(USTICA_BUCK_ENABLE, 
                              USTICA_VBUCK_ENABLE_BIT(vbuck_channel), 0);
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
}

static void ustica_vbuck_disable(VBUCK_CHANNEL vbuck_channel){
   if(ustica_available){
      if(ustica_is_vbuck_enabled(vbuck_channel))
         ustica_power_modify(USTICA_BUCK_ENABLE, 0,
                              USTICA_VBUCK_ENABLE_BIT(vbuck_channel));
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
}

#if 0
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
static int ustica_ldo_voltage2setting(LDO_CHANNEL ldo_channel, LDO_VOL vol)
{
   int setting = -1;
   if(ustica_available){
     if(ldo_channel == LDO5 || ldo_channel == LDO6) 
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
      else if(ldo_channel >= LDO1 && ldo_channel < LDO5)
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
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
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
static LDO_VOL ustica_ldo_setting2voltage(LDO_CHANNEL ldo_channel, int  setting)
{
   LDO_VOL vol = -1;
   if(ustica_available){
      uint8_t setting_tmp;

      if(ldo_channel >= LDO1 && ldo_channel < LDO5)
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
      else if(ldo_channel == LDO5 || ldo_channel == LDO6)
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
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
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
static int ustica_ldo_get_set_reg(LDO_CHANNEL ldo_channel, LDO_VBUCK_OPT option)
{
   uint8_t setting = 0;
   if(ustica_available){
      if(ldo_channel >= LDO1 && ldo_channel <= LDO6 )
      {
         switch(option)
         {
            case SLEEP:
               ustica_power_read(USTICA_LDO_SET(ldo_channel), &setting);
               setting >>= 4;
               break;
            case ACTIVE:
               ustica_power_read(USTICA_LDO_SET(ldo_channel), &setting);
               setting &= 0xF;
               break;
               
            default:
               bbu_printf("The option is not correct\n\r");
               return -1; 
               break;
         }
      }
      else
         bbu_printf("The LDO channel does not exist\n\r");
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
   return (int)setting;
}

static int ustica_ldo_get_vol(LDO_CHANNEL ldo_channel, LDO_VBUCK_OPT option)
{
   int setting = ustica_ldo_get_set_reg(ldo_channel, option);
   return (int)ustica_ldo_setting2voltage(ldo_channel, setting);
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
static void ustica_ldo_cfg(int on_off, LDO_CHANNEL ldo_channel,
                            LDO_VOL voltage, LDO_VBUCK_OPT option){
   if(ustica_available){
      int set_val;
      if(on_off){
         set_val = ustica_ldo_voltage2setting(LDO1, voltage);
         if(set_val == -1)
            return;
         set_val &= 0xF;
         
         if(ldo_channel >= LDO1 && ldo_channel <= LDO6){
            switch(option){
               case ACTIVE:
                  ustica_power_modify(USTICA_LDO_SET(ldo_channel), set_val, 0x0F);
                  break;
               case SLEEP:
                  ustica_power_modify(USTICA_LDO_SET(ldo_channel), (set_val << 4), 0xF0);
                  break;
               default:
                  bbu_printf("The option is not correct\n\r");
                  return;
                  break;
            }
         }else{
            bbu_printf("The LDO%d is not supported\n\r", ldo_channel);
         }
         ustica_ldo_enable(ldo_channel);
      }
      else
         ustica_ldo_disable(ldo_channel);
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
}
#endif

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
static uint8_t ustica_vbuck_voltage2setting(uint16_t mv){
   if(ustica_available){
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
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
      return 0;
   }
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
void ustica_vbuck_cfg(int on_off, VBUCK_CHANNEL vbuck_channel, 
                              VBUCK_VOL voltage, LDO_VBUCK_OPT option)
{
   if(ustica_available){
      uint8_t setting;
      if(on_off){
         setting = ustica_vbuck_voltage2setting(voltage);
         if(setting == INVALID_VBUCK_SETTING)
            return;
         switch(vbuck_channel){
            case VBUCK1:{
               switch(option){
                  case SLEEP:
                     ustica_power_modify(USTICA_VBUCK1_SLEEP, setting,0x7F);
                     break;
                  case DVC00:
                     ustica_power_modify(USTICA_VBUCK1_SET0, setting,0x7F);
                     break;
                  case DVC01:
                     ustica_power_modify(USTICA_VBUCK1_SET1, setting,0x7F);
                     break;
                  case DVC10:
                     ustica_power_modify(USTICA_VBUCK1_SET2, setting,0x7F);
                     break;
                  case DVC11:
                     ustica_power_modify(USTICA_VBUCK1_SET3, setting,0x7F);
                     break;
                     
                  default:
                     bbu_printf("Vbuck%d doesn't support option%d\n\r", vbuck_channel, option);
                     break;
               }
               break;
            }
            case VBUCK4:{
               switch(option){
                  case SLEEP:
                     ustica_power_modify(USTICA_VBUCK4_SLEEP, setting,0x7F);
                     break;
                  case DVC00:
                     ustica_power_modify(USTICA_VBUCK4_SET0, setting,0x7F);
                     break;
                  case DVC01:
                     ustica_power_modify(USTICA_VBUCK4_SET1, setting,0x7F);
                     break;
                  case DVC10:
                     ustica_power_modify(USTICA_VBUCK4_SET2, setting,0x7F);
                     break;
                  case DVC11:
                     ustica_power_modify(USTICA_VBUCK4_SET3, setting,0x7F);
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
                     ustica_power_write(USTICA_VBUCK2_SET,   setting);
                     break;
                  case SLEEP:
                     /* Can't directly write as Bit7 of USTICA_VBUCK2_SLEEP is not
                        reserved any more */
                     ustica_power_modify(USTICA_VBUCK2_SLEEP, setting, 0x7F);
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
                     ustica_power_write(USTICA_VBUCK3_SET,   setting);
                     break;
                  case SLEEP:
                     /* Can't directly write as Bit7 of USTICA_VBUCK2_SLEEP is not
                        reserved any more */
                     ustica_power_modify(USTICA_VBUCK3_SLEEP, setting, 0x7F);
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
                     ustica_power_write(USTICA_VBUCK5_SET,   setting);
                     break;
                  case SLEEP:
                     /* Can't directly write as Bit7 of USTICA_VBUCK2_SLEEP is not
                        reserved any more */
                     ustica_power_modify(USTICA_VBUCK5_SLEEP, setting, 0x7F);
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
         ustica_vbuck_enable(vbuck_channel);
      }else
         ustica_vbuck_disable(vbuck_channel);
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
   return; 
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
static uint8_t ustica_vbuck_get_set_reg(VBUCK_CHANNEL vbuck_channel, 
                                  LDO_VBUCK_OPT option)
{
   uint8_t setting = INVALID_VBUCK_SETTING;
   if(ustica_available){
      switch(vbuck_channel){
         case VBUCK1:
            if(option <= DVC11)
               ustica_power_read(USTICA_VBUCK1_SET(option), &setting);
            else if(option == SLEEP)
               ustica_power_read(USTICA_VBUCK1_SLEEP, &setting);
            break;
         case VBUCK2:
            if(option == SLEEP)
               ustica_power_read(USTICA_VBUCK2_SLEEP, &setting);
            else
               ustica_power_read(USTICA_VBUCK2_SET, &setting);
            break;
         case VBUCK3:
            if(option == SLEEP)
               ustica_power_read(USTICA_VBUCK3_SLEEP, &setting);
            else
               ustica_power_read(USTICA_VBUCK3_SET, &setting);
            break;
         case VBUCK4:
            if(option <= DVC11)
               ustica_power_read(USTICA_VBUCK4_SET(option), &setting);
            else if(option == SLEEP)
               ustica_power_read(USTICA_VBUCK4_SLEEP, &setting);
            break;
         case VBUCK5:
            if(option == SLEEP)
               ustica_power_read(USTICA_VBUCK5_SLEEP, &setting);
            else
               ustica_power_read(USTICA_VBUCK5_SET, &setting);
            break;
         default:
            return INVALID_VBUCK_SETTING;
            break;
      }
      if(setting != INVALID_VBUCK_SETTING)
         setting &= 0x7F;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
   return setting;
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
static uint16_t ustica_vbuck_setting2voltage(uint8_t setting){
   if(ustica_available){
      uint16_t mv;
      
      if(setting > 0x7F)
         return INVALID_VBUCK_VOL;
      
      if(setting <= 0x50)
         mv = (setting * 125)/10 + 600;
      else
         mv = (setting - 0x50)*50 + 1600;
      return mv;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
      return 0;
   }
}

int ustica_vbuck_get_vol(VBUCK_CHANNEL vbuck_channel, LDO_VBUCK_OPT option)
{
   uint8_t setting = ustica_vbuck_get_set_reg(vbuck_channel, option);
   return (int)ustica_vbuck_setting2voltage(setting);
}
#if 0
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


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
   uint8_t setting = INVALID_VBUCK_SETTING;
   if(ustica_available){
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
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
   return setting;
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
   uint8_t setting = INVALID_VBUCK_SETTING;
   if(ustica_available){
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
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
   return setting;
}
#endif

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
   if(ustica_available){
      uint8_t dvc_set, setting;
      dvc_set  = ustica_get_dvc_ramp_rate_setting(dvc_ramp);
      setting = ((dvc_set & 0x7) << 3);
   
      switch(vbuck_channel){
         case VBUCK1:
            setting |= ((dvc_pwm   & 0x1) << 6);
            /* Mask 0x78, bit6(pwm) and bit5:3(dvc_set)  */
            ustica_power_modify(USTICA_VBUCK1_MISC_REG1, setting, 0x78);
            setting = dvc_drive & 0xF;
            ustica_power_modify(USTICA_DRIVE_SELECT_REG, setting, 0xF);
            break;
         case VBUCK2:
            setting |= ((dvc_pwm   & 0x1) << 6);
            /* Mask 0x78, bit6(pwm) and bit5:3(dvc_set)  */
            ustica_power_modify(USTICA_VBUCK2_MISC_REG1, setting, 0x78);
            setting = (dvc_drive & 0x1) << 4;
            ustica_power_modify(USTICA_VBUCK2_MISC_REG3, setting, BIT_4);
            break;
         case VBUCK3:
            /* Mask 0x38, bit5:3(dvc_set)  */
            ustica_power_modify(USTICA_VBUCK3_MISC_REG1, setting, 0x38);
            setting = (dvc_drive & 0x1) << 4;
            ustica_power_modify(USTICA_VBUCK3_MISC_REG3, setting, BIT_4);
            break;
         case VBUCK4:
         	/* Mask 0x38, bit5:3(dvc_set)  */
            ustica_power_modify(USTICA_VBUCK4_MISC_REG1, setting, 0x38);
            setting = (dvc_drive & 0xF) << 4;
            ustica_power_modify(USTICA_DRIVE_SELECT_REG, setting, 0xF0);
            break;
         case VBUCK5:
         	/* Mask 0x38, bit5:3(dvc_set)  */
            ustica_power_modify(USTICA_VBUCK5_MISC_REG1, setting, 0x38);
            setting = (dvc_drive & 0x1) << 4;
            ustica_power_modify(USTICA_VBUCK5_MISC_REG3, setting, BIT_4);
            break;
         default:
            break;
      }
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
}

int ustica_get_version(void)
{
   if(ustica_available){
      uint8_t version;
      ustica_base_read(0x0, &version); //identification register 0x0
      return version;
   }else{
      bbu_printf(DBG_MSG "%s is dummy\n\r", __FUNCTION__);
      return 0;
   }
}

void ustica_ldo_cfg(int on_off, LDO_CHANNEL ldo_channel, 
                     LDO_VOL active_vol,LDO_VOL slp_vol){
   return;
}

void pm830_bat_prio_en(int en_dis)
{
	uint8_t val, value, reg;
	int ret;

	if(en_dis)
		value = BIT_1;
	else
		value = 0;

	reg = 0x3f;
	
	//pm830 may have 0xd0 or 0x66 slave address
      	ret = i2c_read_addr8_byte(PI2C, 0xd0, reg, &val, ICR_MODE_STD);
	val &= ~BIT_1;
	val |= value;
      	ret = i2c_write_addr8_byte(PI2C, 0xd0, 
                                reg, val, ICR_MODE_STD);
      	if(ret != 0) {
		bbu_printf("830 bat_prio en ok\n\r");
		return;
	}

      	ret = i2c_read_addr8_byte(PI2C, 0x66, reg, &val, ICR_MODE_STD);
	val &= ~BIT_1;
	val |= value;
      	ret = i2c_write_addr8_byte(PI2C, 0x66, reg, val, ICR_MODE_STD);
      	if(ret == 0)
        	bbu_printf("830 bat_prio en2 err\n\r");
	else
		bbu_printf("830 bat_prio en2 ok\n\r");
}

#if 0
static struct pmic_int_desc pmic_irq_array[]={
		[PMIC_RTC_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PMIC_RTC_INT_MASK,
			.pmic_isr_handler=NULL,
		},
		[PMIC_BAT_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PMIC_BAT_INT_MASK,
			.pmic_isr_handler=NULL,
		},

		[PMIC_EXTON2_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PMIC_EXTON2_INT_MASK,
			.pmic_isr_handler=NULL,
		},
		
		[PMIC_EXTON1_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PMIC_EXTON1_INT_MASK,
			.pmic_isr_handler=NULL,
		},
		
		[PMIC_ONKEY_INT]={
			.stat_reg_offset =0x5,
			.en_reg_offset =0x09,
			.pmic_int_mask=PMIC_ONKEY_INT_MASK,
			.pmic_isr_handler=NULL,
		},
		

		[PMIC_TINT_INT]={
			.stat_reg_offset =0x6,
			.en_reg_offset =0x0A,
			.pmic_int_mask=PMIC_TINT_INT_MASK,
			.pmic_isr_handler=NULL,
		},
		[PMIC_VINLDO_INT]={
			.stat_reg_offset =0x6,
			.en_reg_offset =0x0A,
			.pmic_int_mask=PMIC_VINLDO_INT_MASK,
			.pmic_isr_handler=NULL,
		},

		[PMIC_GPADC1_INT]={
			.stat_reg_offset =0x7,
			.en_reg_offset =0x0B,
			.pmic_int_mask=PMIC_GPADC1_INT_MASK,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPADC0_INT]={
			.stat_reg_offset =0x7,
			.en_reg_offset =0x0B,
			.pmic_int_mask=PMIC_GPADC0_INT_MASK,
			.pmic_isr_handler=NULL,
		},


		[PMIC_GPIO1_INT]={
			.stat_reg_offset =0x8,
			.en_reg_offset =0x0C,
			.pmic_int_mask=PMIC_GPIO1_INT_MASK,
			.pmic_isr_handler=NULL,
		},
		[PMIC_GPIO0_INT]={
			.stat_reg_offset =0x8,
			.en_reg_offset =0x0C,
			.pmic_int_mask=PMIC_GPIO0_INT_MASK,
			.pmic_isr_handler=NULL,
		},	
};

/*
*pmic interrupt related func
*/		
void pmic_int_enable(e_pmic_int pmic_int_idx)
{
	uint8_t value;
	if(ustica_available){
   	if(pmic_int_idx >= MAX_PMIC_INT)
   	{
   		bbu_printf("pmic int_enable failed,error pmic int num\n\r");
   		return;
   	}
   	ustica_base_read(pmic_irq_array[pmic_int_idx].en_reg_offset,&value);
   	value |=pmic_irq_array[pmic_int_idx].pmic_int_mask;
   	ustica_base_write(pmic_irq_array[pmic_int_idx].en_reg_offset,value);
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
}

void pmic_int_disable(e_pmic_int pmic_int_idx)
{
	uint8_t value;
	if(ustica_available){
   	if(pmic_int_idx >= MAX_PMIC_INT)
   	{
   		bbu_printf("pmic int_disable failed,error pmic int num\n\r");
   		return;
   	}
   	
   	ustica_base_read(pmic_irq_array[pmic_int_idx].en_reg_offset,&value);
   	value &= ~(pmic_irq_array[pmic_int_idx].pmic_int_mask);
   	ustica_base_write(pmic_irq_array[pmic_int_idx].en_reg_offset,value);
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
}
void pmic_isr_connect(e_pmic_int pmic_int_idx, void (*ISR_Handle)() )
{
   if(ustica_available){
      if(NULL == ISR_Handle) 
         return;
      /* assign the user defined function*/
      pmic_irq_array[pmic_int_idx].pmic_isr_handler = (void *)ISR_Handle;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
}

void pmic_isr_disconnect(e_pmic_int pmic_int_idx, void (*ISR_Handle)() )
{
   if(ustica_available){
      if(NULL == ISR_Handle) 
         return;
      /* assign the user defined function*/
      if( pmic_irq_array[pmic_int_idx].pmic_isr_handler == ISR_Handle)
      	pmic_irq_array[pmic_int_idx].pmic_isr_handler=NULL;
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
}

void pmic_clear_int(e_pmic_int pmic_int_idx)
{
	uint8_t value;	
	if(ustica_available){
	   ustica_base_read(pmic_irq_array[pmic_int_idx].stat_reg_offset,&value);
    //ustica_base_write(pmic_irq_array[pmic_int_idx].stat_reg_offset,value);
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
}
		
static void (*pmicISR_Handle)(void *);

static void pmic_irq_handler(void *arg)
{
   int i;
   uint8_t value;	
   if(ustica_available){
#if (0)
      bbu_printf("TWSI_CR= 0x%x\r\n", MRVL_I2C3->ICR);
      bbu_printf("TWSI_SR= 0x%x\r\n", MRVL_I2C3->ISR);
   
      while (BIT_2 & (MRVL_I2C3->ISR));
   
      if(!ustica_base_read(0x1,&value)) while(1);
   
      bbu_printf("pmic 0x1 status register = %d\r\n", value);
#endif

      ustica_base_read(pmic_irq_array[PMIC_RTC_INT].stat_reg_offset,&value);
   
      for(i=0;i < 6;i++)
      { 
   
   	if(0 == (pmic_irq_array[i].pmic_int_mask&value))
   		continue;
   	pmic_clear_int(i);
     	/* get ISR entry from table and call it */
          pmicISR_Handle =  (void (*)())(pmic_irq_array[i].pmic_isr_handler);
          if ( pmicISR_Handle )
             (pmicISR_Handle)(arg);   
     }
      
      ustica_base_read(pmic_irq_array[PMIC_TINT_INT].stat_reg_offset,&value);
      for(i=6;i < 10;i++)
      { 
   
   	if(0 == (pmic_irq_array[i].pmic_int_mask&value))
   		continue;
   	pmic_clear_int(i);
     	/* get ISR entry from table and call it */
          pmicISR_Handle =  (void (*)())(pmic_irq_array[i].pmic_isr_handler);
          if ( pmicISR_Handle )
             (pmicISR_Handle)(arg);   
     }
   
      ustica_base_read(pmic_irq_array[PMIC_GPADC1_INT].stat_reg_offset,&value);
      for(i=10;i < 15;i++)
      { 
   
   	if(0 == (pmic_irq_array[i].pmic_int_mask&value))
   		continue;
    	pmic_clear_int(i);
     	/* get ISR entry from table and call it */
          pmicISR_Handle =  (void (*)())(pmic_irq_array[i].pmic_isr_handler);
          if ( pmicISR_Handle )
             (pmicISR_Handle)(arg);   
     }
      
      ustica_base_read(pmic_irq_array[PMIC_GPIO1_INT].stat_reg_offset,&value);
      for(i=15;i < 20;i++)
      { 
   
   	if(0 == (pmic_irq_array[i].pmic_int_mask&value))
   		continue;
   	pmic_clear_int(i);
     	/* get ISR entry from table and call it */
          pmicISR_Handle =  (void (*)())(pmic_irq_array[i].pmic_isr_handler);
          if ( pmicISR_Handle )
             (pmicISR_Handle)(arg);   
     }
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }   
}

void pmic_irq_init(void)
{  
   if(ustica_available){
      ISR_Connect(IRQ_MMP_PMIC_INT, pmic_irq_handler, NULL);
      INT_Enable(IRQ_MMP_PMIC_INT, 0, 15);
   }else{
      bbu_printf(DBG_MSG "%s is dum pmic\n\r", __FUNCTION__);
   }
}
#endif
