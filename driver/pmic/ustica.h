#ifndef _USTICA_H_
#define _USTICA_H_

#define USTICA_LDO_NUM LDO6

#define USTICA_VERSION_A0      		0x69

#define USTICA_I2C             		PI2C

#define LDO_CFG_ACTIVE   		BIT_0
#define LDO_CFG_SLEEP    		BIT_1
#define LDO_CFG_BOTH     		(LDO_CFG_NORMAL | LDO_CFG_SLEEP)

#define INVALID_VBUCK_SETTING   	0xFF
#define INVALID_VBUCK_VOL       	0xFFFF

#define USTICA_VBUCK_ENABLE_BIT(x)    	(1<<(x-1))

#define USTICA_VBUCK1_SET(opt)      	(USTICA_VBUCK1_SET0 + opt)
#define USTICA_VBUCK4_SET(opt)      	(USTICA_VBUCK4_SET0 + opt)

/* Power registers */
#define USTICA_LDO1                 0x0C
#define USTICA_LDO2                 0x0E
#define USTICA_LDO3                 0x16
#define USTICA_LDO4                 0x15
#define USTICA_LDO5                 0x1B
#define USTICA_LDO6                 0x1C

#define USTICA_VBUCK1_SLEEP         0x30
#define USTICA_VBUCK2_SLEEP         0x31
#define USTICA_VBUCK3_SLEEP         0x32
#define USTICA_VBUCK4_SLEEP         0x33
#define USTICA_VBUCK5_SLEEP         0x34

#define USTICA_DRIVE_SELECT_REG     0x3A
#define USTICA_VBUCK1_SET0          0x3C
#define USTICA_VBUCK1_SET1          0x3D
#define USTICA_VBUCK1_SET2          0x3E
#define USTICA_VBUCK1_SET3          0x3F

#define USTICA_VBUCK2_SET           0x40
#define USTICA_VBUCK3_SET           0x41
#define USTICA_VBUCK4_SET0          0x42
#define USTICA_VBUCK4_SET1          0x43
#define USTICA_VBUCK4_SET2          0x44
#define USTICA_VBUCK4_SET3          0x45
#define USTICA_VBUCK5_SET           0x46

#define USTICA_BUCK_ENABLE          0x50 

#define USTICA_LDO_ENABLE_REG1      0x51    //BIT2:LDO1,BIT4:LDO2
#define USTICA_LDO_ENABLE_REG2      0x52    //BIT3:LDO4,BIT4:LDO3??
#define USTICA_LDO_ENABLE_REG3      0x53    //BIT1:LDO5,BIT2:LDO6

#define USTICA_BUCK_ENABLE2         0x55

#define USTICA_LDO_ENABLE2_REG1     0x56
#define USTICA_LDO_ENABLE2_REG2     0x57
#define USTICA_LDO_ENABLE2_REG3     0x58

#define USTICA_BUCK_SLEEP_REG1      0x5A
#define USTICA_BUCK_SLEEP_REG2      0x5B

#define USTICA_LDO_SLEEP_REG1       0x5C
#define USTICA_LDO_SLEEP_REG2       0x5D
#define USTICA_LDO_SLEEP_REG3       0x5E
#define USTICA_LDO_SLEEP_REG4       0x5F
#define USTICA_LDO_SLEEP_REG5       0x60

#define USTICA_BUCK_GROUP1          0x64
#define USTICA_BUCK_GROUP2          0x65
#define USTICA_BUCK_GROUP3          0x66

#define USTICA_LDO_GROUP2           0x69
#define USTICA_LDO_GROUP3           0x6A
#define USTICA_LDO_GROUP6           0x6D
#define USTICA_LDO_GROUP7           0x6E
#define USTICA_LDO_GROUP9           0x70
#define USTICA_LDO_GROUP10          0x71

#define USTICA_VBUCK1_MISC_REG1     0x78
#define USTICA_VBUCK1_MISC_REG3     0x7A
#define USTICA_VBUCK2_MISC_REG1     0x7B
#define USTICA_VBUCK2_MISC_REG3     0x7D
#define USTICA_VBUCK3_MISC_REG1     0x7E
#define USTICA_VBUCK3_MISC_REG3     0x80
#define USTICA_VBUCK4_MISC_REG1     0x81
#define USTICA_VBUCK4_MISC_REG3     0x83
#define USTICA_VBUCK5_MISC_REG1     0x84
#define USTICA_VBUCK5_MISC_REG3     0x86

#define DVC00_DRIVE_EN  BIT_0
#define DVC01_DRIVE_EN  BIT_1
#define DVC10_DRIVE_EN  BIT_2
#define DVC11_DRIVE_EN  BIT_3

	
#define 	USTICA_RTC_INT_MASK		BIT_4
#define 	USTICA_BAT_INT_MASK		BIT_3
#define 	USTICA_EXTON2_INT_MASK		BIT_2
#define 	USTICA_EXTON1_INT_MASK		BIT_1
#define 	USTICA_ONKEY_INT_MASK		BIT_0

#define 	USTICA_TINT_INT_MASK		BIT_3
#define 	USTICA_VINLDO_INT_MASK		BIT_1

#define 	USTICA_GPADC1_INT_MASK		BIT_1
#define 	USTICA_GPADC0_INT_MASK		BIT_0

#define 	USTICA_GPIO1_INT_MASK		BIT_1
#define 	USTICA_GPIO0_INT_MASK		BIT_0

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
                            uint32_t dvc_drive);

/*****************************************************************************
* Function     :    ustica_get_dvc_ramp_rate_setting
******************************************************************************
*
* Description  :    Use to get the setting value of Buck DVC ramp rate
* Parameters   :    ramp_rate: 195~25000 uV/us

  Return       :    Setting Value for  BKn_DVC_SET 0x0 ~ 0x7
*
*****************************************************************************/
uint8_t ustica_get_dvc_ramp_rate_setting(int ramp_rate);

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
uint8_t ustica_get_start_ramp_time_setting(int ramp_time);
#endif
