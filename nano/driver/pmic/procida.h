#ifndef _PROCIDA_H_
#define _PROCIDA_H_

typedef enum{
MMI_V1_720P = 0,
MMI_V2_720P,		//0.6V
MMI_V2_1080P,		//0.9V
}MMI_ID;
uint8_t get_mmi_id(void);
#define PROCIDA_MEAS_ENABLE1	0x1
#define PROCIDA_MEAS_ENABLE2	0x2
#define PROCIDA_GPADC_MISC1	0x5
#define PROCIDA_GPADC_MISC2	0x6
#define PROCIDA_GPADC0_MEAS1	0x54
#define PROCIDA_GPADC0_MEAS2	0x55
#define PROCIDA_GPADC1_MEAS1	0x56
#define PROCIDA_GPADC1_MEAS2	0x57
#define PROCIDA_GPADC2_MEAS1	0x58
#define PROCIDA_GPADC2_MEAS2	0x59
#define PROCIDA_GPADC3_MEAS1	0x5A
#define PROCIDA_GPADC3_MEAS2	0x5B
#define PROCIDA_GPADC4_MEAS1	0x5C
#define PROCIDA_GPADC4_MEAS2	0x5D

#define PROCIDA_BASE_ADDRESS    0x60
#define PROCIDA_POWER_ADDRESS   0x62
#define PROCIDA_GPADC_ADDRESS   0x64
#define PROCIDA_AUDIO_ADDRESS   0x70

#define PROCIDA_VERSION_B1      0x62
#define PROCIDA_VERSION_C0      0x63

#define PROCIDA_I2C             PI2C

typedef enum LDO_CHANNEL{
   LDO_INVALID = 0,
   LDO1,
   LDO2,
   LDO3,
   LDO4,
   LDO5,
   LDO6,
   LDO7,
   LDO8,
   LDO9,
   LDO10,
   LDO11,
   LDO12,
   LDO13,
   LDO14,
#if 1
   LDO15,
   LDO16,
   LDO17,
   LDO18,
   LDO19,
#endif
   LDO_MAX,
}LDO_CHANNEL;

typedef enum VBUCK_CHANNEL{
   VBUCK_INVALID = 0,
   VBUCK1,
   VBUCK2,
   VBUCK3,
   VBUCK4,
   VBUCK5,
   VBUCK_MAX,
}VBUCK_CHANNEL;

typedef enum SVC_CHANNEL{
   SVC_INVALID = 0,
   SVC1,
   SVC2,
}SVC_CHANNEL;


typedef int LDO_VOL;
#define LDO0V6    (600)
#define LDO0V65   (650)
#define LDO0V7    (700)
#define LDO0V75   (750)
#define LDO0V8    (800)
#define LDO0V85   (850)
#define LDO0V9    (900)
#define LDO0V95   (950)
#define LDO1V0    (1000)
#define LDO1V05   (1050)
#define LDO1V1    (1100)
#define LDO1V15   (1150)
#define LDO1V2    (1200)
#define LDO1V25   (1250)
#define LDO1V3    (1300)
#define LDO1V4    (1400)
#define LDO1V5    (1500)
#define LDO1V7    (1700)
#define LDO1V8    (1800)
#define LDO1V85   (1850)
#define LDO1V9    (1900)
#define LDO2V0    (2000)
#define LDO2V1    (2100)
#define LDO2V5    (2500)
#define LDO2V6    (2600)
#define LDO2V7    (2700)
#define LDO2V75   (2750)
#define LDO2V8    (2800)
#define LDO2V85   (2850)
#define LDO2V9    (2900)
#define LDO3V0    (3000)
#define LDO3V1    (3100)
#define LDO3V3    (3300)

#define VBUCK0V6		(600)
#define VBUCK0V8		(800)
#define VBUCK1V05   	(1050)
#define VBUCK1V1    	(1100)
#define VBUCK1V15   	(1150)
#define VBUCK1V2		(1200)
#define VBUCK1V8		(1800)
#define VBUCK2V0    	(2000)
#define VBUCK2V2		(2200)
#define VBUCK2V4    	(2400)
#define VBUCK2V6    	(2600)
#define VBUCK2V8		(2800)
#define VBUCK3V0    	(3000)
#define VBUCK3V3		(3300)

typedef enum LDO_VBUCK_OPT{
   DVC00 = 0,
   DVC01,
   DVC10,
   DVC11,
   SLEEP,                 //just for ldo1
   AUDIO,                 //just for ldo1
}LDO_VBUCK_OPT;

#define LDO_CFG_ACTIVE   BIT_0
#define LDO_CFG_SLEEP    BIT_1
#define LDO_CFG_BOTH     (LDO_CFG_NORMAL | LDO_CFG_SLEEP)

#define VBUCK_VOL    int

#define INVALID_VBUCK_SETTING   0xFF
#define INVALID_VBUCK_VOL       0xFFFF
#define INVALID_SVC_VOL       0xFFFFFFFF  //uv
#define INVALID_SVC_SETTING       0x0  //uv

#define LDO_ON     1
#define LDO_OFF    0
#define VBUCK_ON   1
#define VBUCK_OFF  0
#define SVC_ON     1
#define SVC_OFF    0
#define LVSW_200mA_OC_ENABLE 	1
#define LVSW_200mA_OC_DISABLE 	0
#define LVSW_30mA_LIMIT_ENABLE 	 1
#define LVSW_30mA_LIMIT_DISABLE  0

#define PROCIDA_LDO_SET(x)             ((LDO1 == x)? \
										PROCIDA_LDO1:((PROCIDA_LDO2_2+x-2)&0xFF))
#define PROCIDA_LDO_ENABLE_REG(x)      ((x < LDO9)? \
                                       PROCIDA_LDO_ENABLE_REG1:PROCIDA_LDO_ENABLE_REG2)
                                       
#define PROCIDA_LDO_ENABLE_BIT(x)      ((x < LDO9)? (1<<(x-1)):(1<<(x-9)))

#define PROCIDA_SVC_ENABLE_BIT(x)      (1<<(x-1))
 
#define PROCIDA_VBUCK_ENABLE_BIT(x)    (1<<(x-1))

#define PROCIDA_VBUCK1_SET(opt)      (PROCIDA_VBUCK1_SET0 + opt)
#define PROCIDA_VBUCK3_SET(opt)      (PROCIDA_VBUCK3_SET0 + opt)
#define PROCIDA_VBUCK5_SET(opt)      (PROCIDA_VBUCK5_SET0 + opt)

/* Power registers */
#define PROCIDA_LDO1               	 0x08

#define PROCIDA_LDO2_1               0x0A
#define PROCIDA_LDO2_2               0x0B
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

#if 0
#define PROCIDA_LDO15                0x18
#define PROCIDA_LDO16                0x19
#define PROCIDA_LDO17                0x1A
#define PROCIDA_LDO18                0x1B
#define PROCIDA_LDO19                0x1C
#endif

#define PROCIDA_VBUCK1_SLEEP         0x30
#define PROCIDA_VBUCK2_SLEEP         0x31
#define PROCIDA_VBUCK3_SLEEP         0x32
#define PROCIDA_VBUCK4_SLEEP         0x33
#define PROCIDA_VBUCK5_SLEEP         0x34

#define PROCIDA_AUDIO_MODE_CFG1      0x38
#define PROCIDA_DRIVE_SELECT_REG1    0x3A
#define PROCIDA_DRIVE_SELECT_REG2    0x3B

#define PROCIDA_VBUCK1_SET0          0x3C
#define PROCIDA_VBUCK1_SET1          0x3D
#define PROCIDA_VBUCK1_SET2          0x3E
#define PROCIDA_VBUCK1_SET3          0x3F

#define PROCIDA_VBUCK2_SET           0x40

#define PROCIDA_VBUCK3_SET0          0x41
#define PROCIDA_VBUCK3_SET1          0x42
#define PROCIDA_VBUCK3_SET2          0x43
#define PROCIDA_VBUCK3_SET3          0x44

#define PROCIDA_VBUCK4_SET           0x45

#define PROCIDA_VBUCK5_SET0          0x46
#define PROCIDA_VBUCK5_SET1          0x47
#define PROCIDA_VBUCK5_SET2          0x48
#define PROCIDA_VBUCK5_SET3          0x49

#define PROCIDA_BUCK_ENABLE          0x50
#define PROCIDA_LDO_ENABLE_REG1      0x51
#define PROCIDA_LDO_ENABLE_REG2      0x52
#define PROCIDA_MISC_ENABLE_REG      0x54

#define PROCIDA_BUCK_ENABLE2         0x55
#define PROCIDA_LDO_ENABLE2_REG1     0x56
#define PROCIDA_LDO_ENABLE2_REG2     0x57
//#define PROCIDA_LDO_ENABLE2_REG3   0x58
#define PROCIDA_MISC_ENABLE_REG3     0x59

#define PROCIDA_BUCK_SLEEP_REG1      0x5A
#define PROCIDA_BUCK_SLEEP_REG2      0x5B
#define PROCIDA_LDO_SLEEP_REG1       0x5C
#define PROCIDA_LDO_SLEEP_REG2       0x5D
#define PROCIDA_LDO_SLEEP_REG3       0x5E
#define PROCIDA_LDO_SLEEP_REG4       0x5F
//#define PROCIDA_LDO_SLEEP_REG5     0x60

#define PROCIDA_SVC_SLEEP_REG        0x63
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

#define PROCIDA_SVC_GROUP1           0x76
#define PROCIDA_SW_GROUP1            0x77

#define PROCIDA_VBUCK1_MISC_REG1     0x78
#define PROCIDA_VBUCK1_MISC_REG2     0x79

#define PROCIDA_VBUCK2_MISC_REG1     0x7B
#define PROCIDA_VBUCK2_MISC_REG2     0x7C

#define PROCIDA_VBUCK3_MISC_REG1     0x7E
#define PROCIDA_VBUCK3_MISC_REG2     0x7F

#define PROCIDA_VBUCK4_MISC_REG1     0x81
#define PROCIDA_VBUCK4_MISC_REG2     0x82

#define PROCIDA_VBUCK5_MISC_REG1     0x84
#define PROCIDA_VBUCK5_MISC_REG2     0x85

#define PROCIDA_SWITCH1_MISC_REG     0x8E
#define PROCIDA_BUCK_SH_CTRL_REG     0x8F

#define PROCIDA_LDO_MISC_REG1        0x90
#define PROCIDA_LDO_MISC_REG2        0x92
#define PROCIDA_LDO_MISC_REG3        0x93
#define PROCIDA_LDO_MISC_REG4        0x94
#define PROCIDA_LDO_MISC_REG5        0x95
#define PROCIDA_LDO_MISC_REG6        0x96
#define PROCIDA_LDO_MISC_REG7        0x97
#define PROCIDA_LDO_MISC_REG8        0x98

#define PROCIDA_SVC1_SLP_SET         0xA0
#define PROCIDA_SVC2_SLP_SET         0xA1

#define PROCIDA_SVC1_AUDIO_SET       0xA8
#define PROCIDA_SVC2_AUDIO_SET       0xA9

#define PROCIDA_SVC1_SET0          	 0xB0
#define PROCIDA_SVC1_SET1          	 0xB1
#define PROCIDA_SVC1_SET2          	 0xB2
#define PROCIDA_SVC1_SET3          	 0xB3

#define PROCIDA_SVC2_SET0          	 0xB4
#define PROCIDA_SVC2_SET1          	 0xB5
#define PROCIDA_SVC2_SET2          	 0xB6
#define PROCIDA_SVC2_SET3          	 0xB7

#define DVC00_DRIVE_EN  			 BIT_0
#define DVC01_DRIVE_EN  		     BIT_1
#define DVC10_DRIVE_EN               BIT_2
#define DVC11_DRIVE_EN               BIT_3


typedef enum {
	PMIC_FL_ALARM_INT = 0,
	PMIC_USE_XO_INT,
	PMIC_CLASSD_OC_INT,
	PMIC_RTC_INT,
	PMIC_BAT_INT,
	PMIC_CHG_INT,
	PMIC_ONKEY_INT,
	
	PMIC_SW_OC_INT,	
	PMIC_LDO_PGOOD_INT,	
	PMIC_BUCK_PGOOD_INT,	
	PMIC_ITEMP_INT,	
	PMIC_VSYS_INT,
	PMIC_VBAT_INT,
	
	PMIC_HS_DET_INT,
	PMIC_MIC_DET_INT,
	PMIC_GPADC3_INT,
	PMIC_GPADC2_INT,
	PMIC_GPADC1_INT,
	PMIC_GPADC0_INT,
#if 0
	PMIC_GPIO4_INT,
	PMIC_GPIO3_INT,
	PMIC_GPIO2_INT,
	PMIC_GPIO1_INT,
	PMIC_GPIO0_INT,
#endif
	MAX_PMIC_INT
}e_pmic_int;

#define 	PMIC_FL_ALARM_INT_MASK	  	BIT_7	
#define 	PMIC_USE_XO_INT_MASK	  	BIT_6
#define 	PMIC_CLASSD_OC_INT_MASK  	BIT_5
#define 	PMIC_RTC_INT_MASK			BIT_4
#define 	PMIC_BAT_INT_MASK			BIT_3
#define 	PMIC_CHG_INT_MASK			BIT_2
//#define 	PMIC_EXTON_INT_MASK			BIT_1
#define 	PMIC_ONKEY_INT_MASK			BIT_0

#define 	PMIC_SW_OC_INT_MASK			BIT_6
#define 	PMIC_LDO_PGOOD_INT_MASK		BIT_5
#define 	PMIC_BUCK_PGOOD_INT_MASK	BIT_4
#define 	PMIC_ITEMP_INT_MASK			BIT_3
//#define 	PMIC_VCHG_INT_MASK			BIT_2
#define 	PMIC_VSYS_INT_MASK			BIT_1
#define 	PMIC_VBAT_INT_MASK			BIT_0

#define 	PMIC_HS_DET_INT_MASK		BIT_5
#define 	PMIC_MIC_DET_INT_MASK		BIT_4
#define 	PMIC_GPADC3_INT_MASK		BIT_3
#define 	PMIC_GPADC2_INT_MASK		BIT_2
#define 	PMIC_GPADC1_INT_MASK		BIT_1
#define 	PMIC_GPADC0_INT_MASK		BIT_0
#if 0
#define 	PMIC_GPIO4_INT_MASK			BIT_4
#define 	PMIC_GPIO3_INT_MASK			BIT_3
#define 	PMIC_GPIO2_INT_MASK			BIT_2
#define 	PMIC_GPIO1_INT_MASK			BIT_1
#define 	PMIC_GPIO0_INT_MASK			BIT_0
#endif
	
struct pmic_int_desc{
	uint8_t stat_reg_offset;
	uint8_t en_reg_offset;
	uint8_t pmic_int_mask;
	void (*pmic_isr_handler)(void*);
};

void pmic_init(void);
int procida_base_read(uint8_t reg, uint8_t *val);
int procida_base_write(uint8_t reg, uint8_t val);
int procida_power_read(uint8_t reg, uint8_t *val);
int procida_power_write(uint8_t reg, uint8_t val);
int procida_gpadc_read(uint8_t reg, uint8_t *val);
int procida_gpadc_write(uint8_t reg, uint8_t val);
int procida_audio_read(uint8_t reg, uint8_t *val);
int procida_audio_write(uint8_t reg, uint8_t val);
void procida_power_modify(uint8_t reg, uint8_t val, uint8_t mask);
int is_vbuck_enabled(VBUCK_CHANNEL vbuck_channel);
int is_ldo_enabled(LDO_CHANNEL ldo_channel);
int procida_get_version(void);

/*pmic_int_idx is declared in enum PMIC_INT_DESC*/
void pmic_int_enable(e_pmic_int pmic_int_idx);
void pmic_int_disable(e_pmic_int pmic_int_idx);
void pmic_isr_connect(e_pmic_int pmic_int_idx, void (*ISR_Handle)());
void pmic_isr_disconnect(e_pmic_int pmic_int_idx, void (*ISR_Handle)());
void pmic_clear_int(e_pmic_int pmic_int_idx);
void pmic_irq_init(void);


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
int procida_ldo_voltage2setting(LDO_CHANNEL ldo_channel, LDO_VOL vol);

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
LDO_VOL procida_ldo_setting2voltage(LDO_CHANNEL ldo_channel, int  setting);

/*****************************************************************************
* Function     :    procida_ldo_get_set_reg
******************************************************************************
*
* Description  :    Get LDO set register value

* Parameters   :    (1)ldo_channel: LDO1 ~ LDO19
                    (2)option: Just for LDO1
                                AUDIO_MODE_EN,  DVC00(0),DVC01(1),
                                DVC10(2),DVC11(3), LDO1_SLP
  Return       :    LDO set register value
*
*****************************************************************************/
int procida_ldo_get_set_reg(LDO_CHANNEL ldo_channel, LDO_VBUCK_OPT option);

/*****************************************************************************
* Function     :    procida_ldo_cfg
******************************************************************************
*
* Description  :    Procida LDO config function
*
* Parameters   :    (1)on_off: LDO_ON(1): turn on; LDO_OFF(0): turn off
                    (2)ldo_channel: LDO1~LDO19
                    (3)active_vol: active voltage 600 - 3300 (mv)
                    (4)slp_vol:    sleep voltage 600 - 3300 (mv)
*
*****************************************************************************/
void procida_ldo_cfg(int on_off, LDO_CHANNEL ldo_channel, 
                     LDO_VOL active_vol,LDO_VOL slp_vol);


/*****************************************************************************
* Function     :    procida_vbuck_cfg
******************************************************************************
*
* Description  :    Procida VBUCK config function used for VBUCK 2, 3, 5
*
* Parameters   :    (1)on_off: VBUCK_ON(1): turn on; VBUCK_OFF(0): turn off
                    (2)vbuck_channel: VBUCK2 VBUCK3 VBUCK5
                    (3)active_vol: 600 - 3950 (mv) voltage in active mode
                    (3)slp_vol: 600 - 3950 (mv) voltage in sleep mode
*
*****************************************************************************/
void procida_vbuck_cfg(int on_off, VBUCK_CHANNEL vbuck_channel, 
                       VBUCK_VOL active_vol, VBUCK_VOL slp_vol);


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
void procida_vbuck_cfg2(int on_off, VBUCK_CHANNEL vbuck_channel, 
                       VBUCK_VOL voltage, LDO_VBUCK_OPT option);
                   
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
void procida_ldo2_cfg(int on_off, LDO_VOL voltage, LDO_VBUCK_OPT option);


/*****************************************************************************
* Function     :    procida_vbuck_voltage2setting
******************************************************************************
*
* Description  :    Use to get the setting value for VBUCK Set Register 
                    according to the to be set voltage in mv
* Parameters   :     mv from 600 to 3950

  Return       :    Setting Value for  VBUCK Set Register from 0x00 to 0x7F
*
*****************************************************************************/
uint8_t procida_vbuck_voltage2setting(uint16_t mv);


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
uint16_t procida_vbuck_setting2voltage(uint8_t setting);

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
uint8_t procida_vbuck_get_set_reg(VBUCK_CHANNEL vbuck_channel, 
                                  LDO_VBUCK_OPT option);


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
/*****************************************************************************
* Function     :    procida_svc_cfg
******************************************************************************
*
* Description  :    Procida SVC config function used for SVC1, SVC2
*
* Parameters   :    (1)on_off: SVC_ON(1): turn on; SVC_OFF(0): turn off
                    (2)vbuck_channel: SVC1, SVC2
                    (3)voltage: 200000 - 750000 (uV)
                    (3)option: DVC00(0),DVC01(1),DVC10(2),DVC11(3), AUDIO,
                               SLEEP
*
*****************************************************************************/
void procida_svc_cfg(int on_off, SVC_CHANNEL svc_channel, 
						uint32_t voltage, int option);
/*****************************************************************************
* Function     :    procida_svc_voltage2setting
******************************************************************************
*
* Description  :    Use to get the setting value for SVC Set Register 
                    according to the to be set voltage in uv
* Parameters   :    uv from 200000 to 750000

  Return       :    Setting Value for  VBUCK Set Register from 0x00 to 0x7F
*
*****************************************************************************/
uint8_t procida_svc_voltage2setting(uint32_t uv);
/*****************************************************************************
* Function     :    procida_svc_setting2voltage
******************************************************************************
*
* Description  :    Use to get the vlotage according to SVC 
                    Set Register value
* Parameters   :    Setting Value for  SVC Set Register from 0x00 to 0x7F

  Return       :    mv from 200000 to 750000
*
*****************************************************************************/
/*****************************************************************************
* Function     :    procida_lvsw_200mA_oc_enable_disable
******************************************************************************
*
* Description  :    enable/disable the LVSW 200 over current protection

* Parameters   :    (1)enable: LVSW_200mA_OC_ENABLE/LVSW_200mA_OC_DISABLE

  Return       :    void
*
*****************************************************************************/
void procida_lvsw_200mA_oc_enable_disable(int enable_disable);

/*****************************************************************************
* Function     :    procida_lvsw_30mA_oc_enable_disable
******************************************************************************
*
* Description  :    enable/disable the LVSW 30mA current limit
				    when the OC is triggered
				    
* Parameters   :    (1)enable: LVSW_30mA_LIMIT_ENABLE/LVSW_30mA_LIMIT_DISABLE

  Return       :    void
*
*****************************************************************************/
void procida_lvsw_30mA_limit_enable_disable(int enable_disable);

uint32_t procida_svc_setting2voltage(uint8_t setting);
uint8_t procida_get_start_ramp_time_setting(int ramp_time);
void procida_sw1_enable(void);
void procida_sw1_disable(void);

#define procida_ldo_on(ldo, active_vol, slp_vol)        \
                           procida_ldo_cfg(LDO_ON, ldo, active_vol, slp_vol)

#define procida_ldo_off(ldo)            procida_ldo_cfg(LDO_OFF, ldo, 0, 0)

#define procida_vbuck24_on(vbuck, active_vol, slp_vo)   \
                        procida_vbuck_cfg(VBUCK_ON,vbuck, active_vol, slp_vo)

#define procida_vbuck135_on(vbuck, voltage, opt)   \
                          procida_vbuck_cfg2(VBUCK_ON,vbuck, voltage, opt)

#define procida_vbuck_off(vbuck)      procida_vbuck_cfg(VBUCK_OFF,vbuck, 0, 0)

#define procida_ldo2_on(vol, opt)           procida_ldo2_cfg(LDO_ON, vol, opt)
#define procida_ldo2_off()                  procida_ldo2_cfg(LDO_OFF, 0, 0)

#endif
