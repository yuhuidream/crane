#ifndef h_pmic_h
#define h_pmic_h


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

#define LDO_ON     1
#define LDO_OFF    0
#define VBUCK_ON   1
#define VBUCK_OFF  0

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
#define VBUCK1V05    (1050)
#define VBUCK1V1     (1100)
#define VBUCK1V15    (1150)
#define VBUCK1V2		(1200)
#define VBUCK1V8		(1800)
#define VBUCK2V0     (2000)
#define VBUCK2V2		(2200)
#define VBUCK2V4     (2400)
#define VBUCK2V6     (2600)
#define VBUCK2V8		(2800)
#define VBUCK3V0     (3000)
#define VBUCK3V3		(3300)

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
   DIALOG,
   PIANOSA,
   USTICA,
}PMIC_TYPE;


#ifdef PMIC_PROCIDA
#include "procida.h"
int pmic_is_procida(void);
#define pmic_ldo_cfg         procida_ldo_cfg
#define pmic_vbuck_cfg       procida_vbuck_cfg
#define pmic_vbuck_get_vol   procida_vbuck_get_vol
#define pmic_ldo_get_vol     procida_ldo_get_vol
#define pmic_is_ldo_on       procida_is_ldo_enabled
#define pmic_is_vbuck_on     procida_is_vbuck_enabled
#endif

#ifdef PMIC_DIALOG
#include "dialog.h"
int pmic_is_dialog(void);
#define pmic_ldo_cfg         dialog_ldo_cfg
#define pmic_vbuck_cfg       dialog_vbuck_cfg
#define pmic_vbuck_get_vol   dialog_vbuck_get_vol
#define pmic_ldo_get_vol     dialog_ldo_get_vol
#define pmic_is_ldo_on       dialog_is_ldo_on
#define pmic_is_vbuck_on     dialog_is_vbuck_on
#endif

#ifdef PMIC_DUMMY
int pmic_is_dummy(void);
#define pmic_ldo_cfg         dummy_ldo_cfg
#define pmic_vbuck_cfg       dummy_vbuck_cfg
#define pmic_vbuck_get_vol   dummy_vbuck_get_vol
#define pmic_ldo_get_vol     dummy_ldo_get_vol
#define pmic_is_ldo_on       dummy_is_ldo_on
#define pmic_is_vbuck_on     dummy_is_vbuck_on
#endif

#ifdef PMIC_USTICA
#include "ustica.h"
int pmic_is_dialog(void);
#define pmic_ldo_cfg         ustica_ldo_cfg
#define pmic_vbuck_cfg       ustica_vbuck_cfg
#define pmic_vbuck_get_vol   ustica_vbuck_get_vol
#define pmic_ldo_get_vol     ustica_ldo_get_vol
#define pmic_is_ldo_on       ustica_is_ldo_enabled
#define pmic_is_vbuck_on     ustica_is_vbuck_enabled
#endif

#define pmic_ldo_on(channel, vol, opt)   pmic_ldo_cfg(LDO_ON, channel, vol, opt)
#define pmic_ldo_off(channel)            pmic_ldo_cfg(LDO_OFF, channel, 0, 0)

#define pmic_vbuck_on(channel, vol, opt) pmic_vbuck_cfg(VBUCK_ON, channel, vol, opt)
#define pmic_vbuck_off(channel)          pmic_vbuck_cfg(VBUCK_OFF, channel, 0, 0)

int pmic_is_procida(void);
int pmic_is_dialog(void);
int pmic_is_dummy(void);
int pmic_is_pianosa(void);
int pmic_is_ustica(void);

#include "ustica.h"

#endif
