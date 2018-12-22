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
    ustica_power_read(0x7e, &tmp);
    tmp |= BIT_6;
    ustica_power_write(0x7e, tmp);
    
    //Force PWM mode
    ustica_power_read(0x7f, &tmp);
    tmp |= BIT_4;
    ustica_power_write(0x7f, tmp);
}

static void pwm_mode_disable(void){
    uint8_t tmp;
    
    //AVS bit
    ustica_power_read(0x7e, &tmp);
    tmp &= ~BIT_6;
    ustica_power_write(0x7e, tmp);
    
    //PWM mode
    ustica_power_read(0x7f, &tmp);
    tmp &= ~BIT_4;
    ustica_power_write(0x7f, tmp);
}

static int is_pwm_mode_enabled(void){
    uint8_t tmp1, tmp2;
    ustica_power_read(0x7e, &tmp1);
    ustica_power_read(0x7f, &tmp2);
    if((tmp1&BIT_6)&&(tmp2&BIT_4))
        return 1;
    return 0;
}

static int setv_pwm_test(int argc, uint8_t **argv)
{
   if(pmic_is_ustica()){
      if(argc == 2){
         if(is_pwm_mode_enabled())
            bbu_printf("setv-->buck3 force PWM ON\n\r");
         else
            bbu_printf("setv-->buck3 force PWM OFF\n\r");
      }else if(argc == 3){
         if(!strcmp((const char*)argv[2], "on")){
            pwm_mode_enable();
            bbu_printf("setv-->Turn ON Vbuck3 force PWM mode\n\r");
         }else if(!strcmp((const char*)argv[2], "off")){
            pwm_mode_disable();
            bbu_printf("setv-->Turn OFF Vbuck3 force PWM mode\n\r");
         }else{
            if(is_pwm_mode_enabled())
               bbu_printf("setv-->Vbuck3 force PWM ON\n\r");
            else
               bbu_printf("setv-->Vbuck3 force PWM OFF\n\r");
         }
      }
      return SCREEN_TEST_PASS;
   }else{
      bbu_printf("setv-->PWM test is not supported\n\r");
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
         bbu_printf("\n\rsetv-->The BUCK%d is on\n\r", v_channel);
      else
         bbu_printf("\n\rsetv-->The BUCK%d is off\n\r", v_channel);
   }

   if(pmic_is_ustica()){
      if(v_channel == VBUCK2 ||  v_channel == VBUCK3
            || v_channel == VBUCK5 )
      {
         vol_mv = pmic_vbuck_get_vol(v_channel, ACTIVE);
         bbu_printf("setv-->The BUCK%d is %d mv in active mode and ", 
                        v_channel, vol_mv);
         vol_mv = pmic_vbuck_get_vol(v_channel, SLEEP);
         bbu_printf("%d mv in sleep mode\n\r", vol_mv);

         if(v_channel == VBUCK3){
            if(is_pwm_mode_enabled())
                bbu_printf("setv-->buck3 is in force PWM mode\n\r");
            else
                bbu_printf("setv-->buck3 is NOT in force PWM mode\n\r");
          }
      }
      else if(v_channel == VBUCK1 ||  v_channel == VBUCK4)
      {
         for(int i = DVC00; i <= DVC11; i++){
            vol_mv = pmic_vbuck_get_vol(v_channel, i);
            bbu_printf("setv-->The BUCK%d is %d mv", 
                         v_channel, vol_mv);
            bbu_printf(" when DVC2 = %d and DVC1 = %d\n\r", 
                        (i & 0x2) >>1, (i & 0x1));
         }
         vol_mv = pmic_vbuck_get_vol(v_channel, SLEEP);
         bbu_printf("setv-->The BUCK%d is %d mv in sleep mode\n\r", 
                     v_channel, vol_mv);                    
      } 
      return SCREEN_TEST_PASS;
   }
      
   if(pmic_is_dialog()){
      vol_mv = pmic_vbuck_get_vol(v_channel, 0);
      bbu_printf("setv-->The BUCK%d is %d mv\n\r", 
                     v_channel, vol_mv);
      return SCREEN_TEST_PASS;
   }
   bbu_printf("setv-->No PMIC on this board\n\r");
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
  
   if(pmic_is_ustica()){
      upper = (v_channel == VBUCK1)? 1800:3300;
      lower = 600;
   }

   else if(pmic_is_dialog()){
      upper = (v_channel == VBUCK1)? 1400:2075;
      lower = (v_channel == VBUCK1)? 600:500;
   }else{
      bbu_printf("setv-->No PMIC on this board\n\r");
      return SCREEN_TEST_FIAL;
   }

   if((vol_mv > upper) || (vol_mv < lower)){
      bbu_printf("setv-->The voltage is out of range for Vbuck%d, "
                 "should in [%d, %d]\n\r", v_channel, lower, upper);
      return SCREEN_TEST_FIAL;
   }
   
   if(pmic_is_ustica()){
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
         bbu_printf("setv-->BUCK%d is set to %d mv", v_channel, vol_mv_rb);
         if( v_channel == VBUCK1 || v_channel == VBUCK4 )
            bbu_printf(" in all dvc(00,01,10,11) modes");
         else
            bbu_printf(" in both active and sleep mode");
         bbu_printf("\n\r");
         return SCREEN_TEST_PASS;      
      }else{
         pmic_vbuck_cfg(VBUCK_ON, v_channel, vol_mv, option);
         vol_mv_rb = pmic_vbuck_get_vol(v_channel, option);
         bbu_printf("setv-->BUCK%d is set to %d mv in ", v_channel, vol_mv_rb);
         print_option_name(option);
         bbu_printf(" mode\n\r");
         return SCREEN_TEST_PASS;
      }
   }

   if(pmic_is_dialog()){
      pmic_vbuck_cfg(VBUCK_ON, v_channel, vol_mv, 0);
      vol_mv_rb = pmic_vbuck_get_vol(v_channel, 0);
      bbu_printf("setv-->BUCK%d is set to %d mv\n\r", v_channel, vol_mv_rb);
      return SCREEN_TEST_PASS;
   }
   return SCREEN_TEST_PASS;
}

static int vbuck_dvc_test(VBUCK_CHANNEL v_channel)
{
   if(pmic_is_ustica()){
      if(v_channel == VBUCK1){
         //vbuck1_dvc_init();
      }
      else if(v_channel == VBUCK4){
         //vbuck4_dvc_init();
      }else{
         bbu_printf("setv-->VBUCK%d doesn't have dvc setting\n\r", v_channel);
         return SCREEN_TEST_FIAL;
      }
      bbu_printf("setv-->VBUCK%d is set to default value\n\r", v_channel);
      return SCREEN_TEST_PASS;
   }
   
   bbu_printf("setv-->DVC setting is not supported\n\r");
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
             bbu_printf("setv-->This <BUCK> is not supported\n\r");
             return SCREEN_TEST_FIAL;
         }
         break;
      case 3:
         v_channel = setv_get_vbuck_channel(argv[1]);
         if(v_channel != -1){
            if(!strcmp((const char*)argv[2], "off"))
            {
               pmic_vbuck_off(v_channel);
               bbu_printf("setv-->The BUCK%d is turned off\n\r",v_channel);
               return SCREEN_TEST_PASS;
            }else if(!strcmp((const char*)argv[2], "dvc") || 
                     !strcmp((const char*)argv[2], "default"))
            {
               return vbuck_dvc_test(v_channel);
            }else{
               vol_mv = conv_dec((char*)argv[2]);
               return set_vbuck_vol(v_channel, vol_mv, INVALID_OPT);
            }
         }else{
            bbu_printf("setv-->The <BUCK> is not supported\n\r");
            return SCREEN_TEST_FIAL;
         }
         break;
      case 4:
         v_channel = setv_get_vbuck_channel(argv[1]);
         if(v_channel != -1){
            opt = get_ldo_vbuck_option(argv[3]);
            if(opt != INVALID_OPT){
               vol_mv = conv_dec((const char*)argv[2]);
               return set_vbuck_vol(v_channel, vol_mv, opt);
            }
            else{
               bbu_printf("setv-->Unsupported option\n\r");
               return SCREEN_TEST_FIAL;
            }
         }else{
            bbu_printf("setv-->The <BUCK> is not supported\n\r");
            return SCREEN_TEST_FIAL;
         }
         break;
   }

   return SCREEN_TEST_PASS;
}
#if 0
/*..........................................................................*/
int bbu_pmic_revision(void){
   uint8_t id = 0;
   int ret = 0;
   ret = ustica_base_read(0x0, &id);
   if(ret == 0)
      return ret;
	bbu_printf("PMIC revision is Ustica Rev.%d Stepping.%d\n\r",0xe&id,0x1f&id);
   return ret;
}
#endif
