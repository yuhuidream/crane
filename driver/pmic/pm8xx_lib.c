#include "predefines.h"
#include "i2c_dec.h"
#include "pmic.h"

int pm8xx_base_read(uint8_t reg, uint8_t *val)
{
   if(pmic_is_pm8xx()){
      int ret;
      ret = i2c_read_addr8_byte(PM8XX_I2C, PM8XX_BASE_ADDRESS, 
                             reg, val, ICR_MODE_STD);
      if(ret == 0) 
         bbu_printf("pm8xx: i2c read fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }
}

int pm8xx_base_write(uint8_t reg, uint8_t val)
{
   if(pmic_is_pm8xx()){
      int ret;
      ret =i2c_write_addr8_byte(PM8XX_I2C, PM8XX_BASE_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      if(ret == 0)
         bbu_printf("pm8xx: i2c write fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }   
}

void pm8xx_base_modify(uint8_t reg, uint8_t val, uint8_t mask)
{
   if(pmic_is_pm8xx()){
      uint8_t tmp;
      pm8xx_base_read(reg, &tmp);
      tmp &= ~mask;
      tmp |= val;
      pm8xx_base_write(reg, tmp);
      return;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return;
   }
}

int pm8xx_gpadc_read(uint8_t reg, uint8_t *val)
{
   if(pmic_is_pm8xx()){
      int ret;
      ret = i2c_read_addr8_byte(PM8XX_I2C, PM8XX_GPADC_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      if(ret == 0) 
         bbu_printf("pm8xx: i2c read fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }     
}

int pm8xx_gpadc_write(uint8_t reg, uint8_t val)
{
   if(pmic_is_pm8xx()){
      int ret;
      ret =i2c_write_addr8_byte(PM8XX_I2C, PM8XX_GPADC_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      if(ret == 0)
         bbu_printf("pm8xx: i2c write fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }      
}

void pm8xx_gpadc_modify(uint8_t reg, uint8_t val, uint8_t mask)
{
   if(pmic_is_pm8xx()){
      uint8_t tmp;
      pm8xx_gpadc_read(reg, &tmp);
      tmp &= ~mask;
      tmp |= val;
      pm8xx_gpadc_write(reg, tmp);
      return;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return;
   }
}

int pm8xx_test_read(uint8_t reg, uint8_t *val)
{
   if(pmic_is_pm8xx()){
      int ret;
      ret = i2c_read_addr8_byte(PM8XX_I2C, PM8XX_TEST_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      if(ret == 0) 
         bbu_printf("pm8xx: i2c read fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }     
}

int pm8xx_test_write(uint8_t reg, uint8_t val)
{
   if(pmic_is_pm8xx()){
      int ret;
      ret =i2c_write_addr8_byte(PM8XX_I2C, PM8XX_TEST_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      if(ret == 0)
         bbu_printf("pm8xx: i2c write fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }      
}

int pm8xx_power_read(uint8_t reg, uint8_t *val)
{
   if(pmic_is_pm8xx()){
      int ret;
      ret = i2c_read_addr8_byte(PM8XX_I2C, PM8XX_POWER_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      if(ret == 0) 
         bbu_printf("pm8xx: i2c read fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }       
}

int pm8xx_power_write(uint8_t reg, uint8_t val)
{
   if(pmic_is_pm8xx()){
      int ret;
      ret =i2c_write_addr8_byte(PM8XX_I2C, PM8XX_POWER_ADDRESS, 
                                reg, val, ICR_MODE_STD);
      if(ret == 0)
         bbu_printf("pm8xx: i2c write fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   }      
}

void pm8xx_power_modify(uint8_t reg, uint8_t val, uint8_t mask)
{
   if(pmic_is_pm8xx()){
      uint8_t tmp;
      pm8xx_power_read(reg, &tmp);
      tmp &= ~mask;
      tmp |= val;
      pm8xx_power_write(reg, tmp);
      return;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return;
   }
}

int pm8xx_audio_read(uint8_t reg, uint8_t *val)
{
   if(pmic_is_pm8xx()){
      int ret;
      ret = i2c_read_addr8_byte(PM8XX_I2C, PM8XX_AUDIO_ADDRESS, reg, val, ICR_MODE_STD);
      if(ret == 0) 
         bbu_printf("pm8xx_audio_read: i2c read fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   } 
}

int pm8xx_audio_write(uint8_t reg, uint8_t val)
{
   if(pmic_is_pm8xx()){
      int ret;
      ret =i2c_write_addr8_byte(PM8XX_I2C, PM8XX_AUDIO_ADDRESS, reg, val, ICR_MODE_STD);
      if(ret == 0)
         bbu_printf("pm8xx_audio_write: i2c write fail\n\r");
      return ret;
   }else{
      bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
      return 0;
   } 
}

int pm8xx_enable_ldo_sleep_mode(LDO_CHANNEL ldo_channel)
{
	uint8_t reg, val, mask, enable_val;
	
	if(pmic_is_procida() && (ldo_channel < LDO1 || ldo_channel > LDO14)) {
		bbu_printf("ldo %d does not exit\n\r", ldo_channel);
		return -1;
	}
	if(pmic_is_ustica() && (ldo_channel < LDO1 || ldo_channel > LDO6)) {
		bbu_printf("ldo %d does not exit for ustica\n\r", ldo_channel);
		return -1;
	}
	
	reg = PM8XX_LDO_2_SLP_CTRL_REG(ldo_channel);
	switch(ldo_channel % 4)
	{
		case 0: 
			mask = PM8XX_LDO_SLEEP_MASK_4;
			enable_val = (0x2 << 6);
			break;
		case 1:
			mask = PM8XX_LDO_SLEEP_MASK_1;
			enable_val = (0x2 << 0);
			break;
		case 2: 
			mask = PM8XX_LDO_SLEEP_MASK_2;
			enable_val = (0x2 << 2);
			break;
		case 3: 
			mask = PM8XX_LDO_SLEEP_MASK_3;
			enable_val = (0x2 << 4);
			break;
	}

	pm8xx_power_read(reg, &val);
	val &= ~mask;
	val |= enable_val;
	pm8xx_power_write(reg, val);	
	return 0;
}

int pm8xx_disable_ldo_sleep_mode(LDO_CHANNEL ldo_channel)
{
	uint8_t reg, val, mask, disable_val;
	
	if(pmic_is_procida() && (ldo_channel < LDO1 || ldo_channel > LDO14)) {
		bbu_printf("ldo %d does not exit for procida\n\r", ldo_channel);
		return -1;
	}

	if(pmic_is_ustica() && (ldo_channel < LDO1 || ldo_channel > LDO6)) {
		bbu_printf("ldo %d does not exit for ustica\n\r", ldo_channel);
		return -1;
	}

	reg = PM8XX_LDO_2_SLP_CTRL_REG(ldo_channel);
	switch(ldo_channel % 4)
	{
		case 0: 
			mask = PM8XX_LDO_SLEEP_MASK_4;
			disable_val = (0x3 << 6);
			break;
		case 1:
			mask = PM8XX_LDO_SLEEP_MASK_1;
			disable_val = (0x3 << 0);
			break;
		case 2: 
			mask = PM8XX_LDO_SLEEP_MASK_2;
			disable_val = (0x3 << 2);
			break;
		case 3: 
			mask = PM8XX_LDO_SLEEP_MASK_3;
			disable_val = (0x3 << 4);
			break;
	}
	pm8xx_power_read(reg, &val);
	val &= ~mask;
	val |= disable_val;
	pm8xx_power_write(reg, val);	
	return 0;
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
        	bbu_printf("830 bat_prio en err\n\r");
	else
		bbu_printf("830 bat_prio en ok\n\r");
}

