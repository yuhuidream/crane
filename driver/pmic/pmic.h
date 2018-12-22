#ifndef h_pmic_h
#define h_pmic_h

#include "predefines.h"
#include "interrupt.h"
#include "timer_dec.h"

#define PM8XX_I2C             PI2C
#define PM8XX_BASE_ADDRESS    0x60
#define PM8XX_POWER_ADDRESS   0x62
#define PM8XX_GPADC_ADDRESS   0x64
#define PM8XX_TEST_ADDRESS    0x6E
#define PM8XX_AUDIO_ADDRESS   0x70

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
   LDO15,
   LDO16,
   LDO17,
   LDO18,
   LDO19,
   LDO20,
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

struct pmic_int_desc{
	uint8_t stat_reg_offset;
	uint8_t en_reg_offset;
	uint8_t pmic_int_mask;
	uint8_t irq_valid;
	void (*pmic_isr_handler)(void*);
};

typedef enum {
	PMIC_CLASSD_OC_INT = 0,
	PMIC_FL_ALARM_INT,
	PMIC_USE_XO_INT,
	PMIC_RTC_INT,
	PMIC_BAT_INT,
	PMIC_CHG_INT,
	PMIC_EXTON_INT,
	PMIC_SW_OC_INT,	
	PMIC_EXTON1_INT,
	PMIC_EXTON2_INT,
	PMIC_VINLDO_INT,
	PMIC_LDO_PGOOD_INT,	
	PMIC_BUCK_PGOOD_INT,	
	PMIC_ONKEY_INT,
	PMIC_TINT_INT,
	PMIC_ITEMP_INT,
	PMIC_VCHG_INT,
	PMIC_VSYS_INT,
	PMIC_VBAT_INT,
	PMIC_HS_DET_INT,
	PMIC_MIC_DET_INT,
	PMIC_GPADC4_INT,
	PMIC_GPADC3_INT,
	PMIC_GPADC2_INT,
	PMIC_GPADC1_INT,
	PMIC_GPADC0_INT,
	PMIC_GPIO4_INT,
	PMIC_GPIO3_INT,
	PMIC_GPIO2_INT,
	PMIC_GPIO1_INT,
	PMIC_GPIO0_INT,
	MAX_PMIC_INT,
}e_pmic_int;
		
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

#define LDO_CFG_ACTIVE   BIT_0
#define LDO_CFG_SLEEP    BIT_1

typedef int  LDO_VOL;
typedef int  VBUCK_VOL;

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
#define VBUCK1V05    		(1050)
#define VBUCK1V1     		(1100)
#define VBUCK1V15    		(1150)
#define VBUCK1V2		(1200)
#define VBUCK1V8		(1800)
#define VBUCK2V0     		(2000)
#define VBUCK2V2		(2200)
#define VBUCK2V4     		(2400)
#define VBUCK2V6     		(2600)
#define VBUCK2V8		(2800)
#define VBUCK3V0     		(3000)
#define VBUCK3V3		(3300)

#define DVC00_DRIVE_EN  BIT_0
#define DVC01_DRIVE_EN  BIT_1
#define DVC10_DRIVE_EN  BIT_2
#define DVC11_DRIVE_EN  BIT_3

#define INVALID_VBUCK_SETTING   0xFF
#define INVALID_VBUCK_VOL       0xFFFF
#define INVALID_SVC_VOL       	0xFFFFFFFF  //uv
#define INVALID_SVC_SETTING     0x0  //uv

typedef enum LDO_VBUCK_OPT{
   DVC00 = 0,
   DVC01,
   DVC10,
   DVC11,
   SLEEP,                 //just for ldo1
   AUDIO,                 //just for ldo1
   ACTIVE,
   INVALID_OPT,
}LDO_VBUCK_OPT;

typedef enum PMIC_TYPE{
   DUMMY_PMIC = 0,
   PROCIDA,
   USTICA,
}PMIC_TYPE;

typedef enum PMIC_VENDOR{
   DUMMY_VENDOR = 0,
   PMIC_MARVELL,
   PMIC_DIALOG,
}PMIC_VENDOR;

struct pmic_callback {
	void (* __pmic_ldo_cfg)(int, LDO_CHANNEL, LDO_VOL, LDO_VBUCK_OPT);
	void (* __pmic_vbuck_cfg)(int, VBUCK_CHANNEL, VBUCK_VOL, LDO_VBUCK_OPT);
	int  (* __pmic_vbuck_get_vol)(VBUCK_CHANNEL, LDO_VBUCK_OPT);
	int  (* __pmic_ldo_get_vol)(LDO_CHANNEL, LDO_VBUCK_OPT);
	int  (* __pmic_is_vbuck_on)(VBUCK_CHANNEL);
	int  (* __pmic_is_ldo_on)(LDO_CHANNEL);
};

int pmic_is_ldo_on(LDO_CHANNEL channel);
int pmic_is_vbuck_on(VBUCK_CHANNEL channel);
int pmic_ldo_get_vol(LDO_CHANNEL channel, LDO_VBUCK_OPT option);
int pmic_vbuck_get_vol(VBUCK_CHANNEL channel, LDO_VBUCK_OPT option);
void pmic_vbuck_cfg(int on_off, VBUCK_CHANNEL channel, VBUCK_VOL mv, LDO_VBUCK_OPT option);
void pmic_ldo_cfg(int on_off, LDO_CHANNEL channel, LDO_VOL mv, LDO_VBUCK_OPT option);


#define pmic_ldo_on(channel, vol, opt)   pmic_ldo_cfg(LDO_ON, channel, vol, opt)
#define pmic_ldo_off(channel)            pmic_ldo_cfg(LDO_OFF, channel, 0, 0)

#define pmic_vbuck_on(channel, vol, opt) pmic_vbuck_cfg(VBUCK_ON, channel, vol, opt)
#define pmic_vbuck_off(channel)          pmic_vbuck_cfg(VBUCK_OFF, channel, 0, 0)

/*pmic_int_idx is declared in enum PMIC_INT_DESC*/
void pmic_int_enable(e_pmic_int pmic_int_idx);
void pmic_int_disable(e_pmic_int pmic_int_idx);
void pmic_isr_connect(e_pmic_int pmic_int_idx, void (*ISR_Handle)());
void pmic_isr_disconnect(e_pmic_int pmic_int_idx, void (*ISR_Handle)());
void pmic_clear_int(e_pmic_int pmic_int_idx);
void pmic_irq_init(void);

void pmic_init(void);
int pmic_is_procida(void);
int pmic_is_dummy(void);
int pmic_is_pm8xx();
int pm8xx_base_read(uint8_t reg, uint8_t *val);
int pm8xx_base_write(uint8_t reg, uint8_t val);
void pm8xx_base_modify(uint8_t reg, uint8_t val, uint8_t mask);
int pm8xx_power_read(uint8_t reg, uint8_t *val);
int pm8xx_power_write(uint8_t reg, uint8_t val);
void pm8xx_power_modify(uint8_t reg, uint8_t val, uint8_t mask);
int pm8xx_gpadc_read(uint8_t reg, uint8_t *val);
int pm8xx_gpadc_write(uint8_t reg, uint8_t val);
void pm8xx_gpadc_modify(uint8_t reg, uint8_t val, uint8_t mask);
int pm8xx_audio_write(uint8_t reg, uint8_t val);
int pm8xx_audio_read(uint8_t reg, uint8_t *val);
int pm8xx_test_read(uint8_t reg, uint8_t *val);
int pm8xx_test_write(uint8_t reg, uint8_t val);
int pm8xx_enable_ldo_sleep_mode(LDO_CHANNEL ldo_channel);
int pm8xx_disable_ldo_sleep_mode(LDO_CHANNEL ldo_channel);

#include "procida.h"
#include "ustica.h"

#define 	PM8XX_LDO_SLEEP_MASK_1		(0x3 << 0)
#define 	PM8XX_LDO_SLEEP_MASK_2		(0x3 << 2)
#define 	PM8XX_LDO_SLEEP_MASK_3		(0x3 << 4)
#define 	PM8XX_LDO_SLEEP_MASK_4		(0x3 << 6)

#define 	PM8XX_LDO_2_SLP_CTRL_REG(ldo)	(0x5C + ldo/4)
#endif
