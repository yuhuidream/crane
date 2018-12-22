#ifndef _PROCIDA_H_
#define _PROCIDA_H_

#define PROCIDA_VERSION_B1      0x62
#define PROCIDA_VERSION_C0      0x63

#define PROCIDA_I2C             PI2C

#define PROCIDA_LDO_SET(x)             ((PROCIDA_LDO1_3+x-1)&0xFF)
#define PROCIDA_LDO_ENABLE_REG(x)      ((x < LDO9)? \
                                       PROCIDA_LDO_ENABLE_REG1 :((x < LDO17) ? \
                                       PROCIDA_LDO_ENABLE_REG2 :PROCIDA_LDO_ENABLE_REG3))
#define PROCIDA_LDO_ENABLE_BIT(x)      ((x < LDO9)? (1<<(x-1)):((x < LDO17)? (1<<(x-9)):(1<<(x-17))))
   
#define PROCIDA_VBUCK_ENABLE_BIT(x)    (1<<(x-1))

#define PROCIDA_VBUCK1_SET(opt)      (PROCIDA_VBUCK1_SET0 + opt)
#define PROCIDA_VBUCK4_SET(opt)      (PROCIDA_VBUCK4_SET0 + opt)

/* Power registers */
#define PROCIDA_LDO1_1               0x08
#define PROCIDA_LDO1_2               0x09
#define PROCIDA_LDO1_3               0x0A
#define PROCIDA_LDO2                 0x0B
#define PROCIDA_LDO3                 0x0C
#define PROCIDA_LDO4                 0x0D
#define PROCIDA_LDO5                 0x0E
#define PROCIDA_LDO6                 0x0F
#define PROCIDA_LDO7                 0x10
#define PROCIDA_LDO8                 0x11
#define PROCIDA_LDO9                 0x12
#define PROCIDA_LDO10                0x13
#define PROCIDA_LDO11                0x14
#define PROCIDA_LDO12                0x15
#define PROCIDA_LDO13                0x16
#define PROCIDA_LDO14                0x17
#define PROCIDA_LDO15                0x18
#define PROCIDA_LDO16                0x19
#define PROCIDA_LDO17                0x1A
#define PROCIDA_LDO18                0x1B
#define PROCIDA_LDO19                0x1C

#define PROCIDA_VBUCK1_SLEEP         0x30
#define PROCIDA_VBUCK2_SLEEP         0x31
#define PROCIDA_VBUCK3_SLEEP         0x32
#define PROCIDA_VBUCK4_SLEEP         0x33
#define PROCIDA_VBUCK5_SLEEP         0x34

#define PROCIDA_AUDIO_MODE_CFG1      0x38
#define PROCIDA_AUDIO_MODE_CFG2      0x39
#define PROCIDA_DRIVE_SELECT_REG     0x3A
#define PROCIDA_VBUCK1_SET0          0x3C
#define PROCIDA_VBUCK1_SET1          0x3D
#define PROCIDA_VBUCK1_SET2          0x3E
#define PROCIDA_VBUCK1_SET3          0x3F

#define PROCIDA_VBUCK2_SET           0x40
#define PROCIDA_VBUCK3_SET           0x41
#define PROCIDA_VBUCK4_SET0          0x42
#define PROCIDA_VBUCK4_SET1          0x43
#define PROCIDA_VBUCK4_SET2          0x44
#define PROCIDA_VBUCK4_SET3          0x45
#define PROCIDA_VBUCK5_SET           0x46

#define PROCIDA_BUCK_ENABLE          0x50
#define PROCIDA_LDO_ENABLE_REG1      0x51
#define PROCIDA_LDO_ENABLE_REG2      0x52
#define PROCIDA_LDO_ENABLE_REG3      0x53
#define PROCIDA_BUCK_ENABLE2         0x55
#define PROCIDA_LDO_ENABLE2_REG1     0x56
#define PROCIDA_LDO_ENABLE2_REG2     0x57
#define PROCIDA_LDO_ENABLE2_REG3     0x58

#define PROCIDA_BUCK_SLEEP_REG1      0x5A
#define PROCIDA_BUCK_SLEEP_REG2      0x5B
#define PROCIDA_LDO_SLEEP_REG1       0x5C
#define PROCIDA_LDO_SLEEP_REG2       0x5D
#define PROCIDA_LDO_SLEEP_REG3       0x5E
#define PROCIDA_LDO_SLEEP_REG4       0x5F
#define PROCIDA_LDO_SLEEP_REG5       0x60
#define PROCIDA_BUCK_GROUP1          0x64
#define PROCIDA_BUCK_GROUP2          0x65
#define PROCIDA_BUCK_GROUP3          0x66

#define PROCIDA_LDO_GROUP1           0x68
#define PROCIDA_LDO_GROUP2           0x69
#define PROCIDA_LDO_GROUP3           0x6A
#define PROCIDA_LDO_GROUP4           0x6B
#define PROCIDA_LDO_GROUP5           0x6C
#define PROCIDA_LDO_GROUP6           0x6D
#define PROCIDA_LDO_GROUP7           0x6E
#define PROCIDA_LDO_GROUP8           0x6F
#define PROCIDA_LDO_GROUP9           0x70
#define PROCIDA_LDO_GROUP10          0x71

#define PROCIDA_VBUCK1_MISC_REG1     0x78
#define PROCIDA_VBUCK1_MISC_REG3     0x7A
#define PROCIDA_VBUCK2_MISC_REG1     0x7B
#define PROCIDA_VBUCK2_MISC_REG3     0x7D
#define PROCIDA_VBUCK3_MISC_REG1     0x7E
#define PROCIDA_VBUCK3_MISC_REG3     0x80
#define PROCIDA_VBUCK4_MISC_REG1     0x81
#define PROCIDA_VBUCK4_MISC_REG3     0x83
#define PROCIDA_VBUCK5_MISC_REG1     0x84
#define PROCIDA_VBUCK5_MISC_REG3     0x86

#define 	PROCIDA_CLASSD_OC_INT_MASK  BIT_5
#define 	PROCIDA_RTC_INT_MASK			BIT_4
#define 	PROCIDA_BAT_INT_MASK			BIT_3
#define 	PROCIDA_CHG_INT_MASK			BIT_2
#define 	PROCIDA_EXTON_INT_MASK		BIT_1
#define 	PROCIDA_ONKEY_INT_MASK		BIT_0
#define 	PROCIDA_TINT_INT_MASK		BIT_3
#define 	PROCIDA_VCHG_INT_MASK		BIT_2
#define 	PROCIDA_VSYS_INT_MASK		BIT_1
#define 	PROCIDA_VBAT_INT_MASK		BIT_0
#define 	PROCIDA_GPADC4_INT_MASK		BIT_4
#define 	PROCIDA_GPADC3_INT_MASK		BIT_3
#define 	PROCIDA_GPADC2_INT_MASK		BIT_2
#define 	PROCIDA_GPADC1_INT_MASK		BIT_1
#define 	PROCIDA_GPADC0_INT_MASK		BIT_0
#define 	PROCIDA_GPIO4_INT_MASK		BIT_4
#define 	PROCIDA_GPIO3_INT_MASK		BIT_3
#define 	PROCIDA_GPIO2_INT_MASK		BIT_2
#define 	PROCIDA_GPIO1_INT_MASK		BIT_1
#define 	PROCIDA_GPIO0_INT_MASK		BIT_0

int procida_base_read(uint8_t reg, uint8_t *val);
int procida_base_write(uint8_t reg, uint8_t val);
int procida_gpadc_read(uint8_t reg, uint8_t *val);
int procida_gpadc_write(uint8_t reg, uint8_t val);
int procida_audio_read(uint8_t reg, uint8_t *val);
int procida_audio_write(uint8_t reg, uint8_t val);
int procida_get_version(void);
void procida_dev_init(void);

/*pmic_int_idx is declared in enum PMIC_INT_DESC*/
void procida_irq_init(void);


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
                            uint32_t dvc_drive);

/*****************************************************************************
* Function     :    procida_get_dvc_ramp_rate_setting
******************************************************************************
*
* Description  :    Use to get the setting value of Buck DVC ramp rate
* Parameters   :    ramp_rate: 195~25000 uV/us

  Return       :    Setting Value for  BKn_DVC_SET 0x0 ~ 0x7
*
*****************************************************************************/
uint8_t procida_get_dvc_ramp_rate_setting(int ramp_rate);

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
uint8_t procida_get_start_ramp_time_setting(int ramp_time);


#endif
