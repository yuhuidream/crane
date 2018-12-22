#include "predefines.h"
#include "procida.h"
#include "i2c_dec.h"
#include "interrupt.h"
#include "timer_dec.h"
static int pmic_available = 0;

int procida_base_read(uint8_t reg, uint8_t *val){
   if(pmic_available){
      int ret;
      ret = i2c_read_addr8_byte(PROCIDA_I2C, PROCIDA_BASE_ADDRESS, 
                             reg, val, ICR_MODE_STD);
      //if(ret == 0) 
      //   bbu_printf("procida: i2c read fail\n\r");
      return ret;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }
}

int procida_base_write(uint8_t reg, uint8_t val){
   if(pmic_available){
      int ret;
      ret =i2c_write_addr8_byte(PROCIDA_I2C, PROCIDA_BASE_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      //if(ret == 0)
      //   bbu_printf("procida: i2c write fail\n\r");
      return ret;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }   
}

int procida_gpadc_read(uint8_t reg, uint8_t *val){
   if(pmic_available){
      int ret;
      ret = i2c_read_addr8_byte(PROCIDA_I2C, PROCIDA_GPADC_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      //if(ret == 0) 
      //   bbu_printf("procida: i2c read fail\n\r");
      return ret;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }     
}

int procida_gpadc_write(uint8_t reg, uint8_t val){
   if(pmic_available){
      int ret;
      ret =i2c_write_addr8_byte(PROCIDA_I2C, PROCIDA_GPADC_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      //if(ret == 0)
      //   bbu_printf("procida: i2c write fail\n\r");
      return ret;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }      
}

int procida_power_read(uint8_t reg, uint8_t *val){
   if(pmic_available){
      int ret;
      ret = i2c_read_addr8_byte(PROCIDA_I2C, PROCIDA_POWER_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      //if(ret == 0) 
      //   bbu_printf("procida: i2c read fail\n\r");
      return ret;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }       
}

int procida_power_write(uint8_t reg, uint8_t val){
   if(pmic_available){
      int ret;
      ret =i2c_write_addr8_byte(PROCIDA_I2C, PROCIDA_POWER_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      //if(ret == 0)
      //   bbu_printf("procida: i2c write fail\n\r");
      return ret;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }      
}

void procida_power_modify(uint8_t reg, uint8_t val, uint8_t mask){
   if(pmic_available){
      uint8_t tmp;
      procida_power_read(reg, &tmp);
      tmp &= ~mask;
      tmp |= val;
      procida_power_write(reg, tmp);
      return;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return;
   }    
}

static void procida_ldo_enable(LDO_CHANNEL ldo_channel){
   if(pmic_available){
      if(!is_ldo_enabled(ldo_channel)){
         procida_power_modify(PROCIDA_LDO_ENABLE_REG(ldo_channel), 
                              PROCIDA_LDO_ENABLE_BIT(ldo_channel), 0);
         if(ldo_channel == LDO8){
            mdelay(1);
            i2c_init(PROCIDA_I2C);
         }
      }
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }     
   return;
}

static void procida_ldo_disable(LDO_CHANNEL ldo_channel){
   if(pmic_available){
      if(is_ldo_enabled(ldo_channel))
         procida_power_modify(PROCIDA_LDO_ENABLE_REG(ldo_channel), 0, 
                              PROCIDA_LDO_ENABLE_BIT(ldo_channel));
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
   return;
}

static void procida_vbuck_enable(VBUCK_CHANNEL vbuck_channel){
   if(pmic_available){
      if(!is_vbuck_enabled(vbuck_channel))
         procida_power_modify(PROCIDA_BUCK_ENABLE, 
                              PROCIDA_VBUCK_ENABLE_BIT(vbuck_channel), 0);
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
}

static void procida_vbuck_disable(VBUCK_CHANNEL vbuck_channel){
   if(pmic_available){
      if(is_vbuck_enabled(vbuck_channel))
         procida_power_modify(PROCIDA_BUCK_ENABLE, 0,
                              PROCIDA_VBUCK_ENABLE_BIT(vbuck_channel));
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
}

int is_vbuck_enabled(VBUCK_CHANNEL vbuck_channel)
{
   if(pmic_available){
      uint8_t tmp;
      procida_power_read(PROCIDA_BUCK_ENABLE, &tmp);
      if(tmp & PROCIDA_VBUCK_ENABLE_BIT(vbuck_channel))
         return 1;
      return 0;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }
}

int is_ldo_enabled(LDO_CHANNEL ldo_channel)
{
   if(pmic_available){
      uint8_t tmp;
      procida_power_read(PROCIDA_LDO_ENABLE_REG(ldo_channel), &tmp);
      if(tmp & PROCIDA_LDO_ENABLE_BIT(ldo_channel))
         return 1;
      return 0;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   } 
}

int procida_get_version(void)
{
   if(pmic_available){
      uint8_t version;
      procida_base_read(0x0, &version); //identification register 0x0
      return version;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
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
int procida_ldo_voltage2setting(LDO_CHANNEL ldo_channel, LDO_VOL vol)
{
   int setting = -1;
   if(pmic_available){
      if(ldo_channel == LDO12)
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
         //else bbu_printf("The voltage is out of range\n\r");
      }
      else if(ldo_channel == LDO1 || ldo_channel == LDO2 
      			||ldo_channel == LDO13) 
      {
         if     (vol >= LDO1V7  && vol < LDO1V8 ) setting = 0x0;
         else if(vol >= LDO1V8  && vol < LDO1V9 ) setting = 0x1;
         else if(vol >= LDO1V9  && vol < LDO2V5 ) setting = 0x2;
         else if(vol >= LDO2V5  && vol < LDO2V7 ) setting = 0x3;
         else if(vol >= LDO2V7  && vol < LDO2V8 ) setting = 0x4;
         else if(vol >= LDO2V8  && vol < LDO3V1 ) setting = 0x5;
         else if(vol >= LDO3V1  && vol < LDO3V3 ) setting = 0x6;
         else if(vol >= LDO3V3)                   setting = 0x7;
         //else bbu_printf("The voltage is out of range\n\r");
      }
      else if(ldo_channel == LDO14 ) /* use only 3 bits */
      {
         if     (vol >= LDO1V7  && vol < LDO1V8 ) setting = 0x0;
         else if(vol >= LDO1V8  && vol < LDO1V9 ) setting = 0x1;
         else if(vol >= LDO1V9  && vol < LDO2V0 ) setting = 0x2;
         else if(vol >= LDO2V0  && vol < LDO2V1 ) setting = 0x3;
         else if(vol >= LDO2V1  && vol < LDO2V5 ) setting = 0x4;
         else if(vol >= LDO2V5  && vol < LDO2V7 ) setting = 0x5;
         else if(vol >= LDO2V7  && vol < LDO2V8 ) setting = 0x6;
         else if(vol >= LDO2V8)                   setting = 0x7;
         //else bbu_printf("The voltage is out of range\n\r");
      }
      if(ldo_channel > LDO2 && ldo_channel < LDO12)
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
         //else bbu_printf("The voltage is out of range\n\r");
      }
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
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
LDO_VOL procida_ldo_setting2voltage(LDO_CHANNEL ldo_channel, int  setting)
{
   LDO_VOL vol = -1;
   if(pmic_available){
      uint8_t setting_tmp;
      if(ldo_channel == LDO12)
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
            default: //bbu_printf("The setting is out of range\n\r");
               break;
         }
      }
      else if(ldo_channel > LDO2 && ldo_channel < LDO12)
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
            default: //bbu_printf("The setting is out of range\n\r");
               break;
         }
      }
      else if(ldo_channel == LDO1 || ldo_channel == LDO2 
      			||ldo_channel == LDO13)
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
            default: //bbu_printf("The setting is out of range\n\r");
               break;
         }
      }   
      else if(ldo_channel == LDO14)
      {
         setting_tmp = setting & 0x7;
         switch(setting_tmp)
         {
            case 0x0: vol = LDO1V7 ; break;
            case 0x1: vol = LDO1V8 ; break;
            case 0x2: vol = LDO1V9 ; break;
            case 0x3: vol = LDO2V0 ; break;
            case 0x4: vol = LDO2V1 ; break;
            case 0x5: vol = LDO2V5 ; break;
            case 0x6: vol = LDO2V7 ; break;
            case 0x7: vol = LDO2V8 ; break;
            default: //bbu_printf("The setting is out of range\n\r");
               break;
         }
      }
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   } 
   return vol;
}

/*****************************************************************************
* Function     :    procida_ldo_get_set_reg
******************************************************************************
*
* Description  :    Get LDO set register value

* Parameters   :    (1)ldo_channel: LDO1 ~ LDO14
                    (2)option: Just for LDO2
                                AUDIO,  DVC00(0),DVC01(1),
                                DVC10(2),DVC11(3), SLEEP
  Return       :    LDO set register value
*
*****************************************************************************/
int procida_ldo_get_set_reg(LDO_CHANNEL ldo_channel, LDO_VBUCK_OPT option)
{
   uint8_t setting = 0;
   if(pmic_available){
      if(ldo_channel == LDO2) {
         switch(option)
         {
            case SLEEP:
               procida_power_read(PROCIDA_LDO2_2, &setting);
               setting >>= 4;
               break;
   
            case AUDIO:
               procida_power_read(PROCIDA_LDO2_1, &setting);
               setting >>= 4;
               break;
                          
            case DVC00:
            case DVC01:
            case DVC10:
            case DVC11:
               procida_power_read(PROCIDA_LDO2_2, &setting);
               setting &= 0xF;
               break;
    
            default:
               //bbu_printf("The option is not correct\n\r");
               return -1; 
         }
      }  
      else if(ldo_channel <= LDO14 && ldo_channel >= LDO1)
         procida_power_read(PROCIDA_LDO_SET(ldo_channel), &setting);
      //else
         //bbu_printf("The LDO channel does not exist\n\r");
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   } 
   return (int)setting;
}

/*****************************************************************************
* Function     :    procida_ldo2_cfg
******************************************************************************
*
* Description  :    Procida LDO2 config function
*
* Parameters   :    (1)on_off: LDO_ON(1): turn on; LDO_OFF(0): turn off
                    (2)voltage: 600 - 1500 mv
                    (2)option: DVC00, DVC01, DVC10,DVC11,
                               SLEEP,AUDIO
*
*****************************************************************************/
//void procida_ldo1_cfg(int on_off, LDO_VOL voltage, LDO_VBUCK_OPT option){
//}

void procida_ldo2_cfg(int on_off, LDO_VOL voltage, LDO_VBUCK_OPT option){
   int set_val;
   if(pmic_available){
      if(on_off){
         set_val = procida_ldo_voltage2setting(LDO2, voltage);
         if(set_val == -1)
            return;
            
         set_val &= 0xF;
         switch(option){
            case SLEEP:
               procida_power_modify(PROCIDA_LDO2_2, (set_val << 4), 0xF0);
               break;
               
            case AUDIO:
               procida_power_modify(PROCIDA_LDO2_1, (set_val << 4), 0xF0);
               break;         
               
            case DVC00:
            case DVC10:
            case DVC01:
            case DVC11:
               procida_power_modify(PROCIDA_LDO2_2, set_val, 0x0F);
               break;
               
            default:
               //bbu_printf("The option is not correct\n\r");
               return;
         }
         procida_ldo_enable(LDO2);
      }
      else
         procida_ldo_disable(LDO2);
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   } 
}


/*****************************************************************************
* Function     :    procida_ldo_cfg
******************************************************************************
*
* Description  :    Procida LDO config function
*
* Parameters   :    (1)on_off: LDO_ON(1): turn on; LDO_OFF(0): turn off
                    (2)ldo_channel: LDO1~LDO14
                    (3)active_vol: 600 - 3300 (mv) voltage in active mode
                    (4)slp_vol:    600 - 3300 (mv) voltage in sleep mode
*
*****************************************************************************/
void procida_ldo_cfg(int on_off, LDO_CHANNEL ldo_channel, 
                     LDO_VOL active_vol,LDO_VOL slp_vol){
   int set_val1, set_val2;
   if(pmic_available){
      if(ldo_channel > LDO14 || ldo_channel < LDO1){
         //bbu_printf("This LDO not exist\n\r");
         return;
      }
      
      if(on_off){
         if(ldo_channel == LDO2){
            //bbu_printf("LDO2 cann't be configured by this function\n\r");
            return;
         }
         set_val1 = procida_ldo_voltage2setting(ldo_channel, active_vol);
         set_val2 = procida_ldo_voltage2setting(ldo_channel, slp_vol);
         if(set_val1 == -1 || set_val2 == -1){
            return;
         }
         set_val1 |= (set_val2<<4);
         
         //LDO14 only use 3 bits
         if(ldo_channel == LDO14)
         	 set_val1 &= 0x77;
         
         procida_power_write(PROCIDA_LDO_SET(ldo_channel), set_val1);
         procida_ldo_enable(ldo_channel);
      }
      else
         procida_ldo_disable(ldo_channel);
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   } 
   return;
}

/*****************************************************************************
* Function     :    procida_vbuck_cfg
******************************************************************************
*
* Description  :    Procida VBUCK config function used for VBUCK 2,4
*
* Parameters   :    (1)on_off: VBUCK_ON(1): turn on; VBUCK_OFF(0): turn off
                    (2)vbuck_channel: VBUCK2 VBUCK4
                    (3)active_vol: 600 - 3950 (mv) voltage in active mode
                    (3)slp_vol: 600 - 3950 (mv) voltage in sleep mode
*
*****************************************************************************/
void procida_vbuck_cfg(int on_off, VBUCK_CHANNEL vbuck_channel, 
                       VBUCK_VOL active_vol, VBUCK_VOL slp_vol){
   uint8_t setting1, setting2;
   if(pmic_available){
      if(on_off)
      {
         if(vbuck_channel == VBUCK1 || vbuck_channel == VBUCK3 ||
         	 vbuck_channel == VBUCK5) {
            //bbu_printf("Vbuck%d cann't be configured by this function\n\r",
            //         vbuck_channel);
            return;
         }
         
         setting1 = procida_vbuck_voltage2setting(active_vol);
         setting2 = procida_vbuck_voltage2setting(slp_vol);
         if(setting1 == INVALID_VBUCK_SETTING || 
         	 setting2 == INVALID_VBUCK_SETTING)
         	 return;
         	 
         switch(vbuck_channel){
            case VBUCK2:
               procida_power_write(PROCIDA_VBUCK2_SET,   setting1);
               /* Can't directly write as Bit7 of PROCIDA_VBUCK2_SLEEP is not
                  reserved any more */
               procida_power_modify(PROCIDA_VBUCK2_SLEEP, setting2, 0x7F);
               break;
            case VBUCK4:
               procida_power_write(PROCIDA_VBUCK4_SET,   setting1);
               /* Can't directly write as Bit7 of PROCIDA_VBUCK3_SLEEP is not
                  reserved any more */
               procida_power_modify(PROCIDA_VBUCK4_SLEEP, setting2, 0x7F);
               break;
            default:
               //bbu_printf("This VBUCK not exist\n\r");
               return;
         }
         procida_vbuck_enable(vbuck_channel);
      }
      else
         procida_vbuck_disable(vbuck_channel);
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   } 
   return;   
}

/*****************************************************************************
* Function     :    procida_vbuck_cfg2
******************************************************************************
*
* Description  :    Procida VBUCK config function used for VBUCK1, VBUCK3, VBUCK5
*
* Parameters   :    (1)on_off: VBUCK_ON(1): turn on; VBUCK_OFF(0): turn off
                    (2)vbuck_channel: VBUCK1, VBUCK3, VBUCK5
                    (3)voltage: 600 - 3950 (mv)
                    (3)option: DVC00(0),DVC01(1),DVC10(2),DVC11(3), AUDIO,
                               SLEEP
*
*****************************************************************************/
void procida_vbuck_cfg2(int on_off, VBUCK_CHANNEL vbuck_channel, 
                       VBUCK_VOL voltage, LDO_VBUCK_OPT option)
{
   uint8_t setting;
   if(pmic_available){
      if(on_off){
         if(!(vbuck_channel == VBUCK1 || vbuck_channel == VBUCK3 ||
         	 	vbuck_channel == VBUCK5)){
            //bbu_printf("Vbuck%d cann't be configured by this function\n\r",
            //            vbuck_channel);
            return;
         }
         
         setting = procida_vbuck_voltage2setting(voltage);
         if(setting == INVALID_VBUCK_SETTING)
            return;
            
         if(vbuck_channel == VBUCK1){
            switch(option){
               case AUDIO:
                  procida_power_modify(PROCIDA_AUDIO_MODE_CFG1, setting,0x7F);
                  break;
               case SLEEP:
                  procida_power_modify(PROCIDA_VBUCK1_SLEEP, setting,0x7F);
                  break;
               case DVC00:
                  procida_power_modify(PROCIDA_VBUCK1_SET0, setting,0x7F);
                  break;
               case DVC01:
                  procida_power_modify(PROCIDA_VBUCK1_SET1, setting,0x7F);
                  break;
               case DVC10:
                  procida_power_modify(PROCIDA_VBUCK1_SET2, setting,0x7F);
                  break;
               case DVC11:
                  procida_power_modify(PROCIDA_VBUCK1_SET3, setting,0x7F);
                  break;
               default:
                  break;
            }
         }else if(vbuck_channel == VBUCK3) {   /* Vbuck 3 , no audio mode */
            switch(option){
               case SLEEP:
                  procida_power_modify(PROCIDA_VBUCK3_SLEEP, setting,0x7F);
                  break;
               case DVC00:
                  procida_power_modify(PROCIDA_VBUCK3_SET0, setting,0x7F);
                  break;
               case DVC01:
                  procida_power_modify(PROCIDA_VBUCK3_SET1, setting,0x7F);
                  break;
               case DVC10:
                  procida_power_modify(PROCIDA_VBUCK3_SET2, setting,0x7F);
                  break;
               case DVC11:
                  procida_power_modify(PROCIDA_VBUCK3_SET3, setting,0x7F);
                  break;
               default:
                  break;
            }
         } else if(vbuck_channel == VBUCK5) {   /* Vbuck 5, no audio mode */
            switch(option){
               case SLEEP:
                  procida_power_modify(PROCIDA_VBUCK5_SLEEP, setting,0x7F);
                  break;
               case DVC00:
                  procida_power_modify(PROCIDA_VBUCK5_SET0, setting,0x7F);
                  break;
               case DVC01:
                  procida_power_modify(PROCIDA_VBUCK5_SET1, setting,0x7F);
                  break;
               case DVC10:
                  procida_power_modify(PROCIDA_VBUCK5_SET2, setting,0x7F);
                  break;
               case DVC11:
                  procida_power_modify(PROCIDA_VBUCK5_SET3, setting,0x7F);
                  break;
               default:
                  break;
            }
         }
         procida_vbuck_enable(vbuck_channel);
      }else
         procida_vbuck_disable(vbuck_channel);
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   } 
   return; 
}

#ifndef CONFIG_NANO
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
                         1)for VBUCK2,4, it just has one bit, 0 or 1,
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
   if(pmic_available){
      uint8_t dvc_set, setting;
      dvc_set  = procida_get_dvc_ramp_rate_setting(dvc_ramp);
      setting = ((dvc_set & 0x7) << 3);
   
      switch(vbuck_channel){
         case VBUCK1:
            setting = ((dvc_set & 0x7) << 3);         
            /* bit5:3(dvc_set)  */
            procida_power_modify(PROCIDA_VBUCK1_MISC_REG1, setting, 0x38);
          
            setting = ((dvc_pwm   & 0x1) << 4);
            /* Mask 0x10, bit4(pwm)*/
            procida_power_modify(PROCIDA_VBUCK1_MISC_REG2, setting, BIT_4);
            
            setting = dvc_drive & 0xF;
            procida_power_modify(PROCIDA_DRIVE_SELECT_REG1, setting, 0xF);
            break;
            
         case VBUCK2:
            setting = ((dvc_set & 0x7) << 3);         
            /* bit5:3(dvc_set)  */
            procida_power_modify(PROCIDA_VBUCK2_MISC_REG1, setting, 0x38);
            setting = ((dvc_pwm   & 0x1) << 4);
            /* Mask 0x10, bit4(pwm)*/
            procida_power_modify(PROCIDA_VBUCK2_MISC_REG2, setting, BIT_4);
            break;
            
         case VBUCK3:
            setting = ((dvc_set & 0x7) << 3);         
            /* bit5:3(dvc_set)  */
            procida_power_modify(PROCIDA_VBUCK3_MISC_REG1, setting, 0x38);
          
            setting = ((dvc_pwm   & 0x1) << 4);
            /* Mask 0x10, bit4(pwm)*/
            procida_power_modify(PROCIDA_VBUCK3_MISC_REG2, setting, BIT_4);
            
            setting = ((dvc_drive & 0xF) << 4);
            procida_power_modify(PROCIDA_DRIVE_SELECT_REG1, setting, 0xF0);
            break;
            
         case VBUCK4:
            setting = ((dvc_set & 0x7) << 3);         
            /* bit5:3(dvc_set)  */
            procida_power_modify(PROCIDA_VBUCK4_MISC_REG1, setting, 0x38);
            setting = ((dvc_pwm   & 0x1) << 4);
            /* Mask 0x10, bit4(pwm)*/
            procida_power_modify(PROCIDA_VBUCK4_MISC_REG2, setting, BIT_4);
            break;
            
         case VBUCK5:
            setting = ((dvc_set & 0x7) << 3);         
            /* bit5:3(dvc_set)  */
            procida_power_modify(PROCIDA_VBUCK5_MISC_REG1, setting, 0x38);
          
            setting = ((dvc_pwm   & 0x1) << 4);
            /* Mask 0x10, bit4(pwm)*/
            procida_power_modify(PROCIDA_VBUCK5_MISC_REG2, setting, BIT_4);
            
            setting = dvc_drive & 0xF;
            procida_power_modify(PROCIDA_DRIVE_SELECT_REG2, setting, 0xF);
            break;
            
         default:
            break;
      }
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   } 
}
#endif

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
uint8_t procida_vbuck_voltage2setting(uint16_t mv){
   if(pmic_available){
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
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   } 
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
uint16_t procida_vbuck_setting2voltage(uint8_t setting){
   uint16_t mv;
   if(pmic_available){
      if(setting > 0x7F)
         return INVALID_VBUCK_VOL;
      
      if(setting <= 0x50)
         mv = (setting * 125)/10 + 600;
      else
         mv = (setting - 0x50)*50 + 1600;
      return mv;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   } 
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
                        b) For VBUCK 2 4
                           SLEEP means setting in sleep mode
                           Otherwise means setting in active mode

  Return       :    Vbuck set register value
*
*****************************************************************************/
uint8_t procida_vbuck_get_set_reg(VBUCK_CHANNEL vbuck_channel, 
                                  LDO_VBUCK_OPT option)
{
   uint8_t setting = INVALID_VBUCK_SETTING;
   if(pmic_available){
      switch(vbuck_channel){
         case VBUCK1:
            if(option <= DVC11)
               procida_power_read(PROCIDA_VBUCK1_SET(option), &setting);
            else if(option == AUDIO)
               procida_power_read(PROCIDA_AUDIO_MODE_CFG1, &setting);
            else if(option == SLEEP)
               procida_power_read(PROCIDA_VBUCK1_SLEEP, &setting);
            break;
            
         case VBUCK2:
            if(option == SLEEP)
               procida_power_read(PROCIDA_VBUCK2_SLEEP, &setting);
            else
               procida_power_read(PROCIDA_VBUCK2_SET, &setting);
            break;
            
         case VBUCK3:
            if(option <= DVC11)
               procida_power_read(PROCIDA_VBUCK3_SET(option), &setting);
            else if(option == SLEEP)
               procida_power_read(PROCIDA_VBUCK3_SLEEP, &setting);
            break;
            
         case VBUCK4:
            if(option == SLEEP)
               procida_power_read(PROCIDA_VBUCK4_SLEEP, &setting);
            else
               procida_power_read(PROCIDA_VBUCK4_SET, &setting);
            break;
            
         case VBUCK5:
            if(option <= DVC11)
               procida_power_read(PROCIDA_VBUCK5_SET(option), &setting);
            else if(option == SLEEP)
               procida_power_read(PROCIDA_VBUCK5_SLEEP, &setting);
            break;
            
         default:
            return INVALID_VBUCK_SETTING;
      }
      if(setting != INVALID_VBUCK_SETTING)
         setting &= 0x7F;
   }else{
      //bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }        
   return setting;
}


void pmic_init(void)
{
   uint8_t version = 0;
   int ret;
   /* Read PMIC version and check if there is a pmic on this board */
   ret = i2c_read_addr8_byte(PROCIDA_I2C, PROCIDA_BASE_ADDRESS, 
                              0, &version, ICR_MODE_STD);
   if(ret == I2C_TIMEOUT){
      pmic_available = 0;
      bbu_printf("NO PMIC\n\r");
   }
   else
      pmic_available = 1;
}

