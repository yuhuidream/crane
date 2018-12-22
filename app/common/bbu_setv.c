#include "predefines.h"
#include "pmic.h"
#include "utils.h"
#include "common_tests.h"
#include "bbu.h"
#include <string.h>
#include <stdlib.h>

extern void vbuck1_dvc_init(void);
extern void vbuck3_dvc_init(void);
extern void vbuck4_dvc_init(void);

/* set VBUCK3 to PWM mode */
static void pwm_mode_enable(void){
    uint8_t tmp;
   
    //AVS bit
    pm8xx_power_read(0x7e, &tmp);
    tmp |= BIT_6;
    pm8xx_power_write(0x7e, tmp);
    
    //Force PWM mode
    pm8xx_power_read(0x7f, &tmp);
    tmp |= BIT_4;
    pm8xx_power_write(0x7f, tmp);
}

static void pwm_mode_disable(void){
    uint8_t tmp;
    
    //AVS bit
    pm8xx_power_read(0x7e, &tmp);
    tmp &= ~BIT_6;
    pm8xx_power_write(0x7e, tmp);
    
    //PWM mode
    pm8xx_power_read(0x7f, &tmp);
    tmp &= ~BIT_4;
    pm8xx_power_write(0x7f, tmp);
}

static int is_pwm_mode_enabled(void){
    uint8_t tmp1, tmp2;
    pm8xx_power_read(0x7e, &tmp1);
    pm8xx_power_read(0x7f, &tmp2);
    if((tmp1&BIT_6)&&(tmp2&BIT_4))
        return 1;
    return 0;
}

static int setv_pwm_test(int argc, uint8_t **argv)
{
   if(pmic_is_ustica()){
      if(argc == 2){
         if(is_pwm_mode_enabled())
            bbu_printf("setv-->  Vbuck3 force PWM mode is ON\n\r");
         else
            bbu_printf("setv-->  Vbuck3 force PWM mode is OFF\n\r");
      }else if(argc == 3){
         if(!strcmp((const char*)argv[2], "on")){
            pwm_mode_enable();
            bbu_printf("setv-->  Turn ON Vbuck3 force PWM mode\n\r");
         }else if(!strcmp((const char*)argv[2], "off")){
            pwm_mode_disable();
            bbu_printf("setv-->  Turn OFF Vbuck3 force PWM mode\n\r");
         }else{
            if(is_pwm_mode_enabled())
               bbu_printf("setv-->  Vbuck3 force PWM mode is ON\n\r");
            else
               bbu_printf("setv-->  Vbuck3 force PWM mode is OFF\n\r");
         }
      }
      return SCREEN_TEST_PASS;
   }else{
      bbu_printf("setv-->  PWM test is not supported in current PMIC\n\r");
      return SCREEN_TEST_FIAL;
   }
}

static int setv_get_vbuck_channel(uint8_t *argv){
   int v_channel = -1;
   if(!strcmp((const char*)argv, "buck1") || 
      !strcmp((const char*)argv, "1"))
      v_channel = VBUCK1;
   else if(!strcmp((const char*)argv, "buck2") || 
            !strcmp((const char*)argv, "2"))
      v_channel = VBUCK2;
   else if(!strcmp((const char*)argv, "buck3") || 
            !strcmp((const char*)argv, "3"))
      v_channel = VBUCK3;
   else if(!strcmp((const char*)argv, "buck4") || 
            !strcmp((const char*)argv, "4"))
      v_channel = VBUCK4;
   else if(!strcmp((const char*)argv, "buck5") || 
            !strcmp((const char*)argv, "5"))
      v_channel = VBUCK5;
   return v_channel;
}

static int display_vbuck_vol(VBUCK_CHANNEL v_channel)
{
   int vol_mv;
   if(!pmic_is_dummy()){
      if(pmic_is_vbuck_on(v_channel))
         bbu_printf("\n\rsetv-->  The V_BUCK%d is on\n\r", v_channel);
      else
         bbu_printf("\n\rsetv-->  The V_BUCK%d is off\n\r", v_channel);
   }

   if(pmic_is_ustica()){
      if(v_channel == VBUCK2 ||  v_channel == VBUCK3
            || v_channel == VBUCK5 )
      {
         vol_mv = pmic_vbuck_get_vol(v_channel, ACTIVE);
         bbu_printf("setv-->  The V_BUCK%d is %d mv in active mode and ", 
                        v_channel, vol_mv);
         vol_mv = pmic_vbuck_get_vol(v_channel, SLEEP);
         bbu_printf("%d mv in sleep mode\n\r", vol_mv);

         if(v_channel == VBUCK3){
            if(is_pwm_mode_enabled())
                bbu_printf("setv-->  Vbuck3 is in force PWM mode\n\r");
            else
                bbu_printf("setv-->  Vbuck3 is NOT in force PWM mode\n\r");
          }
      }
      else if(v_channel == VBUCK1 ||  v_channel == VBUCK4)
      {
         for(int i = DVC00; i <= DVC11; i++){
            vol_mv = pmic_vbuck_get_vol(v_channel, i);
            bbu_printf("setv-->  The V_BUCK%d is %d mv", 
                         v_channel, vol_mv);
            bbu_printf(" when DVC2 = %d and DVC1 = %d\n\r", 
                        (i & 0x2) >>1, (i & 0x1));
         }
         vol_mv = pmic_vbuck_get_vol(v_channel, SLEEP);
         bbu_printf("setv-->  The V_BUCK%d is %d mv in sleep mode\n\r", 
                     v_channel, vol_mv);
#if 0                    
        if(v_channel == VBUCK1){
            vol_mv = pmic_vbuck_get_vol(v_channel, AUDIO);
            bbu_printf("setv-->  The V_BUCK%d is %d mv in audio mode\n\r", 
                     v_channel, vol_mv);
        }
#endif                     

      } 
      return SCREEN_TEST_PASS;
   }
      
   if(pmic_is_procida()){
      if(v_channel == VBUCK2 ||  v_channel == VBUCK3 || 
         v_channel == VBUCK5 )
      {
         vol_mv = pmic_vbuck_get_vol(v_channel, ACTIVE);
         bbu_printf("setv-->  The V_BUCK%d is %d mv in active mode and ", 
                        v_channel, vol_mv);
         vol_mv = pmic_vbuck_get_vol(v_channel, SLEEP);
         bbu_printf("%d mv in sleep mode\n\r", vol_mv);
         if(v_channel == VBUCK3){
            if(is_pwm_mode_enabled())
                bbu_printf("setv-->  Vbuck3 is in force PWM mode\n\r");
            else
                bbu_printf("setv-->  Vbuck3 is NOT in force PWM mode\n\r");
          }
      }
      else if(v_channel == VBUCK1 ||  v_channel == VBUCK4 )
      {
         for(int i = DVC00; i <= DVC11; i++){
            vol_mv = pmic_vbuck_get_vol(v_channel, i);
            bbu_printf("setv-->  The V_BUCK%d is %d mv", 
                         v_channel, vol_mv);
            bbu_printf(" when DVC2 = %d and DVC1 = %d\n\r", 
                        (i & 0x2) >>1, (i & 0x1));
         }
         vol_mv = pmic_vbuck_get_vol(v_channel, SLEEP);
         bbu_printf("setv-->  The V_BUCK%d is %d mv in sleep mode\n\r", 
                     v_channel, vol_mv);
         vol_mv = pmic_vbuck_get_vol(v_channel, AUDIO);
         bbu_printf("setv-->  The V_BUCK%d is %d mv in audio mode\n\r", 
                     v_channel, vol_mv);
      }
      return SCREEN_TEST_PASS;
   }

   bbu_printf("setv-->  No PMIC on this board\n\r");
   return SCREEN_TEST_FIAL;
}

static LDO_VBUCK_OPT get_ldo_vbuck_option(uint8_t *argv)
{
   LDO_VBUCK_OPT opt;
   if(!strcmp((const char*)argv, "dvc11") || 
        !strcmp((const char*)argv, "11") )
      opt = DVC11;
   else if(!strcmp((const char*)argv, "dvc10") || 
             !strcmp((const char*)argv, "10"))
      opt = DVC10;
   else if(!strcmp((const char*)argv, "dvc01") || 
             !strcmp((const char*)argv, "01"))
      opt = DVC01;
   else if(!strcmp((const char*)argv, "dvc00") || 
             !strcmp((const char*)argv, "00"))
      opt = DVC00;
   else if(!strcmp((const char*)argv, "audio"))
      opt = AUDIO;
   else if(!strcmp((const char*)argv, "sleep"))
      opt = SLEEP;
   else if(!strcmp((const char*)argv, "active"))
      opt = ACTIVE;
   else
      opt = INVALID_OPT;
   return opt;
}


static void print_option_name(LDO_VBUCK_OPT option)
{
   switch(option){
      case DVC00:
         bbu_printf("DVC00");
         break;
      case DVC01:
         bbu_printf("DVC01");
         break;
      case DVC10:
         bbu_printf("DVC10");
         break;
      case DVC11:
         bbu_printf("DVC11");
         break;
      case AUDIO:
         bbu_printf("AUDIO");
         break;
      case SLEEP:
         bbu_printf("SLEEP");
         break;
      case ACTIVE:
         bbu_printf("ACTIVE");
         break;
      default:
         bbu_printf("UNSUPPORT");
         break;
   }
}

static int set_vbuck_vol(VBUCK_CHANNEL v_channel, int vol_mv, LDO_VBUCK_OPT option)
{
   int upper, lower, vol_mv_rb;
  
   if(pmic_is_ustica() || pmic_is_procida()){
      upper = (v_channel == VBUCK1)? 1400:3300;
      lower = 600;
   }
   else{
      bbu_printf("setv-->  No PMIC on this board\n\r");
      return SCREEN_TEST_FIAL;
   }

   if((vol_mv > upper) || (vol_mv < lower)){
      bbu_printf("setv-->  The voltage is out of range for Vbuck%d, "
                 "should in [%d, %d]\n\r", v_channel, lower, upper);
      return SCREEN_TEST_FIAL;
   }
   
   if(pmic_is_ustica()|| pmic_is_procida()){
      if(option == INVALID_OPT){
         if(v_channel == VBUCK2 ||  v_channel == VBUCK3 || 
            v_channel == VBUCK5 )
         {
            pmic_vbuck_cfg(VBUCK_ON, v_channel, vol_mv, SLEEP);
            pmic_vbuck_cfg(VBUCK_ON, v_channel, vol_mv, ACTIVE);
            vol_mv_rb = pmic_vbuck_get_vol(v_channel, ACTIVE);
         }
         else{
            for(int opt = DVC00; opt <= DVC11; opt++)
               pmic_vbuck_cfg(VBUCK_ON, v_channel, vol_mv, opt);
            vol_mv_rb = pmic_vbuck_get_vol(v_channel, DVC11);   
         }
         bbu_printf("setv-->  V_BUCK%d is set to %d mv", v_channel, vol_mv_rb);
         if( v_channel == VBUCK1 || v_channel == VBUCK4 )
            bbu_printf(" in all dvc(00,01,10,11) modes");
         else
            bbu_printf(" in both active and sleep mode");
         bbu_printf("\n\r");
         return SCREEN_TEST_PASS;      
      }else{
         pmic_vbuck_cfg(VBUCK_ON, v_channel, vol_mv, option);
         vol_mv_rb = pmic_vbuck_get_vol(v_channel, option);
         bbu_printf("setv-->  V_BUCK%d is set to %d mv in ", v_channel, vol_mv_rb);
         print_option_name(option);
         bbu_printf(" mode\n\r");
         return SCREEN_TEST_PASS;
      }
   }

   return SCREEN_TEST_PASS;
}

static int vbuck_dvc_test(VBUCK_CHANNEL v_channel)
{
#if 0
   if(pmic_is_ustica() || pmic_is_procida()){
      if(v_channel == VBUCK1){
         vbuck1_dvc_init();
      }
      else if(v_channel == VBUCK4){
         vbuck4_dvc_init();
      }else{
         bbu_printf("setv-->  VBUCK%d doesn't have dvc setting\n\r", v_channel);
         return SCREEN_TEST_FIAL;
      }
      bbu_printf("setv-->  VBUCK%d is set to default value\n\r", v_channel);
      return SCREEN_TEST_PASS;
   }
#endif   
   bbu_printf("setv-->  DVC set to default is not supported in CP side\n\r");
   return SCREEN_TEST_FIAL;
}

/*......................... BBU setv test function..........................*/
int bbu_setv_test(int argc, uint8_t **argv){
   int v_channel;
   int vol_mv;
   LDO_VBUCK_OPT opt = DVC11;
   

   if(argc > 2 && !strcmp((const char*)argv[1], "pwm"))
      return setv_pwm_test(argc, argv);

   switch(argc){
      case 1:
         bbu_setv_help();
         return SCREEN_TEST_PASS; 
         break;
      case 2:
         v_channel = setv_get_vbuck_channel(argv[1]);
         if(v_channel != -1){
            return display_vbuck_vol(v_channel);
         }else{
             bbu_printf("setv-->  This <v_buck> is not supported\n\r");
             return SCREEN_TEST_FIAL;
         }
         break;
      case 3:
         v_channel = setv_get_vbuck_channel(argv[1]);
         if(v_channel != -1){
            if(!strcmp((const char*)argv[2], "off"))
            {
               pmic_vbuck_off(v_channel);
               bbu_printf("setv-->  The V_BUCK%d is turned off\n\r",v_channel);
               return SCREEN_TEST_PASS;
            }else if(!strcmp((const char*)argv[2], "dvc") || 
                     !strcmp((const char*)argv[2], "default"))
            {
               return vbuck_dvc_test(v_channel);
            }else{
               vol_mv = atoi((const char*)argv[2]);
               return set_vbuck_vol(v_channel, vol_mv, INVALID_OPT);
            }
         }else{
            bbu_printf("setv-->  The <v_buck> is not supported\n\r");
            return SCREEN_TEST_FIAL;
         }
         break;
      case 4:
         v_channel = setv_get_vbuck_channel(argv[1]);
         if(v_channel != -1){
            opt = get_ldo_vbuck_option(argv[3]);
            if(opt != INVALID_OPT){
               vol_mv = atoi((const char*)argv[2]);
               return set_vbuck_vol(v_channel, vol_mv, opt);
            }
            else{
               bbu_printf("setv-->  Unsupported option\n\r");
               return SCREEN_TEST_FIAL;
            }
         }else{
            bbu_printf("setv-->  The <v_buck> is not supported\n\r");
            return SCREEN_TEST_FIAL;
         }
         break;
   }

   return SCREEN_TEST_PASS;
}

static int display_ldo_vol(LDO_CHANNEL ldo_channel)
{
   int vol_mv;
   if(!pmic_is_dummy()){
      if(pmic_is_ldo_on(ldo_channel))
         bbu_printf("\n\rsldo-->  The LDO%d is on\n\r", ldo_channel);
      else
         bbu_printf("\n\rsetv-->  The LDO%d is off\n\r", ldo_channel);
   }

   if(pmic_is_ustica()){
      if(ldo_channel <= LDO6 && ldo_channel >= LDO1)
      {
         vol_mv = pmic_ldo_get_vol(ldo_channel, SLEEP);
         bbu_printf("sldo-->  LDO%d is set to %4d mv in sleep state\n\r",
                     ldo_channel, vol_mv);
         vol_mv = pmic_ldo_get_vol(ldo_channel, ACTIVE);
         bbu_printf("sldo-->  LDO%d is set to %4d mv in active state\n\r",
                     ldo_channel, vol_mv);
      }
      else{
         bbu_printf("sldo-->  The LDO channel does not exist\n\r");
         return SCREEN_TEST_FIAL;
      }
   }
   
   if(pmic_is_procida()){
      if(ldo_channel == LDO1)
      {
         for(int opt = DVC00; opt <= AUDIO; opt++){
            vol_mv = pmic_ldo_get_vol(ldo_channel, opt);
            bbu_printf("sldo-->  LDO2 is set to %4d mv in ", vol_mv);
            print_option_name(opt);
            bbu_printf(" mode\n\r");
         }
      }
      else if(ldo_channel <= LDO19 && ldo_channel > LDO1)
      {
         vol_mv = pmic_ldo_get_vol(ldo_channel, SLEEP);
         bbu_printf("sldo-->  LDO%d is set to %4d mv in sleep state\n\r",
                     ldo_channel, vol_mv);
         vol_mv = pmic_ldo_get_vol(ldo_channel, ACTIVE);
         bbu_printf("sldo-->  LDO%d is set to %4d mv in active state\n\r",
                     ldo_channel, vol_mv);
      }
      else{
         bbu_printf("sldo-->  The LDO channel does not exist\n\r");
         return SCREEN_TEST_FIAL;
      }
   }
   return SCREEN_TEST_PASS;
}
static int get_ldo_channel(uint8_t *argv)
{
   int channel = -1;
   channel = conv_dec(( char*)argv);
   return channel;
}

static int ldo_dvc_test(LDO_CHANNEL channel)
{

   if(pmic_is_procida()){
      if(channel == LDO1){
         //ldo1_dvc_init();
         bbu_printf("sldo-->  LDO1 is set to default value\n\r");
         return SCREEN_TEST_PASS;
      }else{
         bbu_printf("sldo--> LDO%d doesn't support this option\n\r", channel);
         return SCREEN_TEST_FIAL;
      }
   }
      
   bbu_printf("sldo-->  DVC setting is not supported\n\r");
   return SCREEN_TEST_FIAL;
}

static int set_ldo_vol(LDO_CHANNEL channel, int vol_mv, LDO_VBUCK_OPT option)
{
   int vol_mv_rb;
   if(pmic_is_procida()){
      if(option == INVALID_OPT){
         if(channel == LDO1)
         {
            for(int opt = DVC00; opt <= DVC11; opt++)
               pmic_ldo_on(LDO1, vol_mv, opt);
            vol_mv_rb = pmic_ldo_get_vol(LDO1, DVC11);
            bbu_printf("sldo-->  LDO1 is set to %4d mv in all dvc(00,01,10,11) modes\n\r",vol_mv);
         }
         else if(channel <= LDO19 && channel > LDO1)
         {
            pmic_ldo_on(channel, vol_mv, SLEEP);
            pmic_ldo_on(channel, vol_mv, ACTIVE);
            vol_mv_rb = pmic_ldo_get_vol(channel, ACTIVE);
            bbu_printf("sldo-->  LDO%d is set to %4d mv in sleep and "
                       "active state\n\r",channel,vol_mv);
         }
         else{
            bbu_printf("sldo-->  The LDO channel does not exist\n\r");
            return SCREEN_TEST_FIAL;
         }
         return SCREEN_TEST_PASS;      
      }else{
         pmic_ldo_on(channel, vol_mv, option);
         vol_mv_rb = pmic_ldo_get_vol(channel, option);
         bbu_printf("sldo-->  LDO%d is set to %d mv in ", channel, vol_mv_rb);
         print_option_name(option);
         bbu_printf(" mode\n\r");
         return SCREEN_TEST_PASS;
      }
   }
   if(pmic_is_ustica()){
      if(option == INVALID_OPT){
         if(channel <= LDO6 && channel >= LDO1)
         {
            pmic_ldo_on(channel, vol_mv, SLEEP);
            pmic_ldo_on(channel, vol_mv, ACTIVE);
            vol_mv_rb = pmic_ldo_get_vol(channel, ACTIVE);
            bbu_printf("sldo-->  LDO%d is set to %4d mv in sleep and "
                       "active state\n\r",channel,vol_mv);
         }
         else{
            bbu_printf("sldo-->  The LDO channel does not exist\n\r");
            return SCREEN_TEST_FIAL;
         }
         return SCREEN_TEST_PASS;      
      }else{
         pmic_ldo_on(channel, vol_mv, option);
         vol_mv_rb = pmic_ldo_get_vol(channel, option);
         bbu_printf("sldo-->  LDO%d is set to %d mv in ", channel, vol_mv_rb);
         print_option_name(option);
         bbu_printf(" mode\n\r");
         return SCREEN_TEST_PASS;
      }
   }
   
   return SCREEN_TEST_PASS;
}

/*......................... BBU sldo test function..........................*/
int bbu_sldo_test(int argc, uint8_t **argv){
   int channel;
   int vol_mv;
   LDO_VBUCK_OPT opt = DVC11;

   switch(argc){
      case 1:
         bbu_sldo_help();
         return SCREEN_TEST_PASS; 
         break;
      case 2:
         channel = get_ldo_channel(argv[1]);
         if(channel != -1){
            return display_ldo_vol(channel);
         }
         break;
      case 3:
         channel = get_ldo_channel(argv[1]);
         if(channel != -1){
            if(!strcmp((const char*)argv[2], "off"))
            {
               pmic_ldo_off(channel);
               bbu_printf("sldo-->  The LDO%d is turned off\n\r",channel);
               return SCREEN_TEST_PASS;
            }else if(!strcmp((const char*)argv[2], "dvc") || 
                     !strcmp((const char*)argv[2], "default"))
            {
               return ldo_dvc_test(channel);
            }else{
               vol_mv = atoi((const char*)argv[2]);
               return set_ldo_vol(channel, vol_mv, INVALID_OPT);
            }
         }else{
            bbu_printf("setv-->  The <v_buck> is not supported\n\r");
            return SCREEN_TEST_FIAL;
         }
         break;
      case 4:
         channel = get_ldo_channel(argv[1]);
         if(channel != -1){
            opt = get_ldo_vbuck_option(argv[3]);
            if(opt != INVALID_OPT){
               vol_mv = atoi((const char*)argv[2]);
               return set_ldo_vol(channel, vol_mv, opt);
            }
            else{
               bbu_printf("sldo-->  Unsupported option\n\r");
               return SCREEN_TEST_FIAL;
            }
         }else{
            bbu_printf("sldo-->  The <ldo_channel> is not supported\n\r");
            return SCREEN_TEST_FIAL;
         }
         break;
   }
   return SCREEN_TEST_PASS;
}

/*..........................................................................*/
int bbu_pmic_revision(void){
   uint8_t id = 0;
   int ret = 0;
   ret = pm8xx_base_read(0x0, &id);
   if(ret == 0)
      return ret;
#if 0
   switch(id){
      case 0x64:
         bbu_printf("PMIC revision is Procida D0\n\r");
         ret = 1;
         break;
      case 0x63:
         bbu_printf("PMIC revision is Procida C0\n\r");
         ret = 1;
         break;
      default:
         bbu_printf("Unknown Procida revision\n\r");
         ret = 0;
         break;
   }
#else
	bbu_printf("PMIC revision is Ustica Rev.%d Stepping.%d\n\r",0xe&id,0x1f&id);
#endif
   return ret;
}

int bbu_pmic_func(int argc, uint8_t **argv)
{
    int result = SCREEN_TEST_PASS;
    int onkey_idx, press_time;
    
    switch(argc){
        case 1:
            result = bbu_pmic_revision();
            break;
        case 4:
            if(!strcmp((const char*)argv[1], "onkey"))
            {   
                onkey_idx = conv_dec((char*)argv[2]);
                press_time = conv_dec((char*)argv[3]);
                if(onkey_idx != 0 || onkey_idx != 1)
                {
                    bbu_printf("choose onkey 0 or 1\n\r"); 
                    break;  
                }
                
                if(press_time < 1 || press_time > 16)
                {
                    bbu_printf("choose onkey press time 1~16\n\r");
                    break;
                }
                
                ustica_onkey_enable(onkey_idx, press_time);
            } 
            else
                bbu_pmic_help();
            
            break;
        default:
            bbu_pmic_help();
            break;
    }
    
    return result;
}

/*..........................................................................*/
#define HIGHEST_VOL  14250   /* 14500*0.1 mv */
int LOWEST_VOL = 12000;      /* 12000*0.1 mv */
static int vbuck1_vol = 0;
static int vbuck1_increase = 1;  /* 1: self-increase, 0: self-decrease */
/* Increase Vbuck1 from 1250 to 1450 mv, then reset to 1250mv */
int vbuck1_self_incre_decre(void){
   int opt;
   if(vbuck1_vol == 0)  /* vbuck1_vol not initialized */
      vbuck1_vol = LOWEST_VOL;
   if(vbuck1_vol <= HIGHEST_VOL && vbuck1_vol >= LOWEST_VOL)
      for(opt = DVC00; opt <= AUDIO; opt++)
         pmic_vbuck_cfg(VBUCK_ON, VBUCK1, ((vbuck1_vol+5)/10), opt);
   bbu_printf("VCC_MAIN is set to %d.%d mv\n\r",vbuck1_vol/10,vbuck1_vol%10);
   if(vbuck1_increase)
      vbuck1_vol += 125;
   else
      vbuck1_vol -= 125;
   if(vbuck1_vol == (HIGHEST_VOL+125)){
      vbuck1_vol = (HIGHEST_VOL-125);
      vbuck1_increase = 0;
   }else if(vbuck1_vol == (LOWEST_VOL-125)){
      vbuck1_vol = (LOWEST_VOL+125);
      vbuck1_increase = 1;
   }
   return 1;
}

static int not_first_vbuck1 = 0;
/* Increase Vbuck1 from 1250 to 1450 mv, then reset to 1250mv */
int vbuck1_self_increase(void){
   int opt;
   int ret = 0;
   if(vbuck1_vol == 0)  /* vbuck1_vol not initialized */
      vbuck1_vol = LOWEST_VOL;
   if(vbuck1_vol <= HIGHEST_VOL && vbuck1_vol >= LOWEST_VOL)
      for(opt = DVC00; opt <= AUDIO; opt++)
         pmic_vbuck_cfg(VBUCK_ON, VBUCK1, ((vbuck1_vol+5)/10), opt);
   bbu_printf("VCC_MAIN is set to %d.%d mv\n\r",vbuck1_vol/10,vbuck1_vol%10);
   if((vbuck1_vol == LOWEST_VOL) && not_first_vbuck1)
      ret = 1;
   vbuck1_vol += 125;
   if(vbuck1_vol == (HIGHEST_VOL+125)){
      vbuck1_vol = LOWEST_VOL;
      not_first_vbuck1 = 1;
   }
   return ret;
}

static int not_first_vbuck3 = 0;
static int vbuck3_vol = 0;
static int vbuck3_increase = 1;  /* 1: self-increase, 0: self-decrease */
/* Increase Vbuck3 from 1250 to 1450 mv, then reset to 1250mv */
int vbuck3_self_incre_decre(void){
   int opt;
   if(vbuck3_vol == 0)  /* vbuck1_vol not initialized */
      vbuck3_vol = LOWEST_VOL;
   if(vbuck3_vol <= HIGHEST_VOL && vbuck3_vol >= LOWEST_VOL)
      for(opt = DVC00; opt <= AUDIO; opt++)
         pmic_vbuck_cfg(VBUCK_ON, VBUCK3, ((vbuck3_vol+5)/10), opt);
   bbu_printf("VCC_MAIN is set to %d.%d mv\n\r",vbuck3_vol/10,vbuck3_vol%10);
   if(vbuck3_increase)
      vbuck3_vol += 125;
   else
      vbuck3_vol -= 125;
   if(vbuck3_vol == (HIGHEST_VOL+125)){
      vbuck3_vol = (HIGHEST_VOL-125);
      vbuck3_increase = 0;
   }else if(vbuck3_vol == (LOWEST_VOL-125)){
      vbuck3_vol = (LOWEST_VOL+125);
      vbuck3_increase = 1;
   }
   return 1;
}

/* Increase Vbuck3 from 1250 to 1450 mv, then reset to 1250mv */
int vbuck3_self_increase(void){
   int opt;
   int ret = 0;
   if(vbuck3_vol == 0)  /* vbuck1_vol not initialized */
      vbuck3_vol = LOWEST_VOL;
   if(vbuck3_vol <= HIGHEST_VOL && vbuck3_vol >= LOWEST_VOL)
      for(opt = DVC00; opt <= AUDIO; opt++)
         pmic_vbuck_cfg(VBUCK_ON, VBUCK3, ((vbuck3_vol+5)/10), opt);
   bbu_printf("VCC_MAIN is set to %d.%d mv\n\r",vbuck3_vol/10,vbuck3_vol%10);
   if((vbuck3_vol == LOWEST_VOL) && not_first_vbuck3)
      ret = 1;
   vbuck3_vol += 125;
   if(vbuck3_vol == (HIGHEST_VOL+125)){
      vbuck3_vol = LOWEST_VOL;
      not_first_vbuck3 = 1;
   }
   return ret;
}
static int not_first_vbuck5 = 0;
static int vbuck5_vol = 0;
static int vbuck5_increase = 1;  /* 1: self-increase, 0: self-decrease */
/* Increase Vbuck5 from 1250 to 1450 mv, then reset to 1250mv */
int vbuck5_self_incre_decre(void){
   int opt;
   if(vbuck5_vol == 0)  /* vbuck5_vol not initialized */
      vbuck5_vol = LOWEST_VOL;
   if(vbuck5_vol <= HIGHEST_VOL && vbuck5_vol >= LOWEST_VOL)
      for(opt = DVC00; opt <= AUDIO; opt++)
         pmic_vbuck_cfg(VBUCK_ON, VBUCK5, ((vbuck5_vol+5)/10), opt);
   bbu_printf("VCC_MAIN is set to %d.%d mv\n\r",vbuck5_vol/10,vbuck5_vol%10);
   if(vbuck5_increase)
      vbuck5_vol += 125;
   else
      vbuck5_vol -= 125;
   if(vbuck5_vol == (HIGHEST_VOL+125)){
      vbuck5_vol = (HIGHEST_VOL-125);
      vbuck5_increase = 0;
   }else if(vbuck5_vol == (LOWEST_VOL-125)){
      vbuck5_vol = (LOWEST_VOL+125);
      vbuck5_increase = 1;
   }
   return 1;
}

/* Increase Vbuck5 from 1250 to 1450 mv, then reset to 1250mv */
int vbuck5_self_increase(void){
   int opt;
   int ret = 0;
   if(vbuck5_vol == 0)  /* vbuck1_vol not initialized */
      vbuck5_vol = LOWEST_VOL;
   if(vbuck5_vol <= HIGHEST_VOL && vbuck5_vol >= LOWEST_VOL)
      for(opt = DVC00; opt <= AUDIO; opt++)
         pmic_vbuck_cfg(VBUCK_ON, VBUCK5, ((vbuck5_vol+5)/10), opt);
   bbu_printf("VCC_MAIN is set to %d.%d mv\n\r",vbuck5_vol/10,vbuck5_vol%10);
   if((vbuck5_vol == LOWEST_VOL) && not_first_vbuck5)
      ret = 1;
   vbuck5_vol += 125;
   if(vbuck5_vol == (HIGHEST_VOL+125)){
      vbuck5_vol = LOWEST_VOL;
      not_first_vbuck5 = 1;
   }
   return ret;
}

int set_lowest_vol_for_vbuck1_self_change(uint32_t low)
{
   if(low >= (HIGHEST_VOL/10))
      return 0;  /* the lowest voltage should not exceed HIGHEST_VOL */
   
   LOWEST_VOL = (int)(low*10);
   vbuck1_vol = 0;
   return 1;
}

int set_lowest_vol_for_vbuck3_self_change(uint32_t low)
{
   if(low >= (HIGHEST_VOL/10))
      return 0;  /* the lowest voltage should not exceed HIGHEST_VOL */
   
   LOWEST_VOL = (int)(low*10);
   vbuck3_vol = 0;
   return 1;
}

int set_lowest_vol_for_vbuck5_self_change(uint32_t low)
{
   if(low >= (HIGHEST_VOL/10))
      return 0;  /* the lowest voltage should not exceed HIGHEST_VOL */
   
   LOWEST_VOL = (int)(low*10);
   vbuck5_vol = 0;
   return 1;
}
