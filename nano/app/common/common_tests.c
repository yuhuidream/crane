#include "predefines.h"
#include <string.h>
#include "bbu.h"
#include "pmic.h"
#include "timer_dec.h"
#include "CIU.h"
#include "common_tests.h"
#include "i2c_dec.h"
#include "MCU.h"

#ifndef CONFIG_NANO
#include "spinlock.h"
#include "gpio_dec.h"
#include "utils.h"
#include "tri_led.h"
#include "gpio_dec.h"
#include "cache-test.h"
#include "vfp.h"
#include "gic_dec.h"
#ifdef CONFIG_HSI
#include "hsi_dec.h"
#endif
/* ........................................................................ */
void bbu_gpio(int argc, uint8_t **argv)
{
   uint32_t value;
   uint32_t reg, bit;
   switch(argc){
      case 1:
         bbu_printf("Usage: gpio [/x] <gpio_pin>, "
                    "type gpio /h for detail help\n\r");
         break;
      case 2:
         if(check_help(argv[1])){
            bbu_gpio_help();
         }else
            bbu_printf("Usage: gpio [/x] <gpio_pin>, "
                       "type gpio /h for detail help\n\r");
         break;
      case 3:
         value = conv_dec((char*)argv[2]);
         if(!strcmp((const char*)argv[1], "/o")){
            gpio_set_output(value);
            bbu_printf("gpio--> GPIO line specified "
                       "above set to OUTPUT.\n\r");
         }
         else if(!strcmp((const char*)argv[1], "/i")){
            gpio_set_input(value);
            bbu_printf("gpio--> GPIO line specified above set to INPUT.\n\r");
         }
         else if(!strcmp((const char*)argv[1], "/s")){
            gpio_output_high(value);
            bbu_printf("gpio--> GPIO bit specified above set HIGH.\n\r");
         }
         else if(!strcmp((const char*)argv[1], "/c")){
            gpio_output_low(value);
            bbu_printf("gpio--> GPIO bit specified above set LOW.\n\r");
         }
         else if(!strcmp((const char*)argv[1], "/d")){
            if(gpio_get_direction(value))
               bbu_printf("gpio--> GPIO %d is configured as output.\n\r",value);
            else
               bbu_printf("gpio--> GPIO %d is configured as input.\n\r",value);
         }
         else if(!strcmp((const char*)argv[1], "/l")){
            reg = value/32;
            bit = value%32;
            if(reg >= 6){
               bbu_printf("gpio--> GPIO pin index is out of range\n\r");
               return;
            }
            reg = gpio_get_pin_level_register(reg);
            if(reg & (1<<bit)){
               bbu_printf("gpio--> GPIO %d pin level is HIGH\n\r", value);
            }else{
               bbu_printf("gpio--> GPIO %d pin level is LOW\n\r", value);
            }
         }
         else
            bbu_printf("Usage: gpio [/x] <gpio_pin>, "
                       "type gpio /h for detail help\n\r");
         break;
      default:
         bbu_printf("Usage: gpio [/x] <gpio_pin>, "
                    "type gpio /h for detail help\n\r");
         break;
   }

   return;
}

/* ........................................................................ */
void bbu_ledt(int argc, uint8_t **argv)
{
   uint32_t status = 0, config = 0;
   status = tri_led_status();
   if(argc == 1){
      bbu_printf("ledt-->  Current status: ");
      if(status & LED_R_ON)  bbu_printf("Red On, ");
      else bbu_printf("Red Off, ");
      if(status & LED_B_ON)  bbu_printf("Blue On, ");
      else bbu_printf("Blue Off, ");
      if(status & LED_G_ON)  bbu_printf("Green On. ");
      else bbu_printf("Green Off. ");
      bbu_printf("\n\r");
      return;
   }

   argc -- ;
   argv ++;

   if(check_help(*argv))
      return bbu_ledt_help();

   while(argc > 0 ){
      if(!strcmp((const char*)*argv, "off")){
         bbu_printf("ledt-->  LEDs are turned off\n\r");
         tri_led_off(0xf);
         return ;
      }

      if(!strcmp((const char*)*argv, "r") ||
      	 !strcmp((const char*)*argv, "red"))
         config |= LED_R_ON;

      if(!strcmp((const char*)*argv, "b") ||
      	 !strcmp((const char*)*argv, "blue"))
         config |= LED_B_ON;

      if(!strcmp((const char*)*argv, "g") ||
      	 !strcmp((const char*)*argv, "green"))
         config |= LED_G_ON;
      argc -- ;
      argv ++;
   }

   if(config == 0){
      bbu_printf("ledt-->  The arguments are all wrong\n\r");
      bbu_ledt_help();
      return;
   }
   tri_led_off(0xf);
   tri_led_on(config);
   bbu_printf("ledt-->  Turn on LED: ");
   if(config & LED_R_ON) bbu_printf("Red ");
   if(config & LED_B_ON) bbu_printf("Blue ");
   if(config & LED_G_ON) bbu_printf("Green ");
   bbu_printf("\n\r"BBU_PROMPT);

   return;
}


/* ........................................................................ */
void bbu_eica_test(int argc, uint8_t **argv)
{
#ifdef CONFIG_SMP
   (void)argc;(void)argv;
   bbu_eica(0, 0);
   bbu_printf("L1 I&D cache, L2 cache and BPU enabled\n\r");
#else
   if(argc == 1){
      bbu_eica(0,0);
      bbu_printf("L1 I-cache, D-cache, BPU and L2 cache (prefetch) enabled\n\r");
   }
   else{
      if(check_help(argv[1])){
         bbu_eica_help();
      }
      else if(strcmp((const char*)argv[1], "d") == 0){
         bbu_eica(1, D_CACHE_ENABLE);
         bbu_printf("L1 D-cache and L2 cache enabled\n\r");
      }
      else if(strcmp((const char*)argv[1], "i") == 0){
         bbu_eica(1, I_CACHE_ENABLE);
         bbu_printf("L1 I-cache enabled\n\r");
      }
      else
         bbu_printf("The cache type not correct!\n\r");
   }
#endif
   return;
}

/* ........................................................................ */
void bbu_dica_test(int argc, uint8_t **argv)
{
#ifdef CONFIG_SMP
   (void)argc;(void)argv;
   bbu_dica();
   bbu_printf("L1 I&D cache, L2 cache disabled\n\r");
#else
   if((argc == 2) && (check_help(argv[1]))){
      bbu_dica_help();
   }
   else{
      bbu_dica();
      bbu_printf("L1 I&D cache, L2 cache disabled\n\r");
   }
   return;
#endif
}

#ifndef CONFIG_CPU_AO
/* ........................................................................ */
void bbu_ivfp(int argc, uint8_t **argv)
{
   int core = smp_processor_id();
   if(argc == 1){
      if(is_vfp_enabled())
      {
         bbu_printf("Core%d NEON/VFP current state: enabled\n\r", core);
      }else{
         bbu_printf("Core%d NEON/VFP current state: disabled\n\r", core);
      }
   }else{
      if(check_help(argv[1]))
         bbu_ivfp_help();
      else if(!strcmp((const char*)argv[1], "1") ||
      	 !strcmp((const char*)argv[1], "on")){
         if(!is_vfp_enabled())
            vfp_enable();
         bbu_printf("Core%d NEON/VFP is enabled\n\r", core);
      }else if(!strcmp((const char*)argv[1], "0") ||
      	       !strcmp((const char*)argv[1], "off")){
         if(is_vfp_enabled())
            vfp_disable();
         bbu_printf("Core%d NEON/VFP is disabled\n\r", core);
      }else if(!strcmp((const char*)argv[1], "d") ||
      	       !strcmp((const char*)argv[1], "dump")){
      	 if(!is_vfp_enabled()){
      	    bbu_printf("Can't dump registers of Core%d NEON/VFP as "
      	                 "it's disabled\n\r", core);
      	 }else{
            vfp_reg_dump();
          }
      }else
         bbu_printf("Unsupported option\n\r");
   }
   bbu_printf(BBU_PROMPT);
   return;
}
#else
static spinlock_t ivfp_lock = {0};
static void* __bbu_ivfp(void* info)
{
   int mode = (int)info;
   uint8_t c = smp_processor_id();
   spin_lock(&ivfp_lock);
   switch(mode){
      case 0:
         if(is_vfp_enabled())
         {
            bbu_printf("Core%d VFP current state: enabled\n\r", c);
         }else{
            bbu_printf("Core%d VFP current state: disabled\n\r", c);
         }
         break;
      case 1:
         if(!is_vfp_enabled())
            vfp_enable();
         bbu_printf("Core%d VFP is enabled\n\r", c);
         break;
      case 2:
         if(is_vfp_enabled())
            vfp_disable();
         bbu_printf("Core%d VFP is disabled\n\r", c);
         break;
      case 3:
         if(!is_vfp_enabled()){
            bbu_printf("Can't dump registers of Core%d NEON/VFP as "
      	                 "it's disabled\n\r",c);
      	 }else{
            vfp_reg_dump();
          }
         break;
   }
   spin_unlock(&ivfp_lock);
   return (void *)0;
}
/* ........................................................................ */
void bbu_ivfp(int argc, uint8_t **argv)
{
   void *info;
   uint8_t cpu = 1 << smp_processor_id();
   bbu_printf("\n\r");
   switch(argc){
      case 1:
         info = (void *)0;
         core_start_parallel(__bbu_ivfp, info, 0xF);
         break;
      case 3:
         if(!strcmp((const char*)argv[2], "all")){
            cpu = (1<<CORE_NUM) - 1;
         }
         else{
            cpu = conv_dec((char *)argv[2]);
            if(cpu > CORE_NUM){
               bbu_printf("CPU id is out of range\n\r"BBU_PROMPT);
               return;
            }
            cpu = 1<<cpu;
         }
      case 2:
         if(check_help(argv[1]))
            bbu_ivfp_help();
         else if(!strcmp((const char*)argv[1], "1") ||
         	 !strcmp((const char*)argv[1], "on")){
         	info = (void *)1;
            core_start_parallel(__bbu_ivfp, info, cpu);
         }else if(!strcmp((const char*)argv[1], "0") ||
         	       !strcmp((const char*)argv[1], "off")){
            info = (void *)2;
            core_start_parallel(__bbu_ivfp, info, cpu);
         }else if(!strcmp((const char*)argv[1], "d") ||
         	       !strcmp((const char*)argv[1], "dump")){
            info = (void *)3;
            core_start_parallel(__bbu_ivfp, info, cpu);
         }else
            bbu_printf("Unsupported option\n\r");
         break;
      default:
         bbu_ivfp_help();
         break;
   }
   return;
}
#endif

/* ........................................................................ */
void bbu_clear_screen(void)
{
   bbu_printf("%c[H", 27);   //clear screen
   bbu_printf("%c[J", 27);   //go to top
}

/* ........................................................................ */
void bbu_ddrp(int argc, uint8_t **argv){
   uint8_t prio[8]; int port;
   switch(argc){
      case 1:
         bbu_printf("\n\rddrp-->  Current ports' priority is: "
                    "(0,1: low; 2:high; 3:critical)\n\r");
         for(port = 0; port < DDR_PORT_INVALID; port++){
            prio[0] = ddr_get_port_prio(port);
            bbu_printf("ddrp-->  DDR port priority is %d\n\r", port, prio[0]);
         }
      case 2:
         if(check_help(argv[1])){
            bbu_ddrp_help();
            break;
         }
         port =conv_dec((char *)argv[1]);
         if(port >= DDR_PORT_INVALID)
            bbu_printf("ddrp-->  Port number is out of range\n\r");
         else{
            prio[0] = ddr_get_port_prio(port);
            bbu_printf("ddrp-->  DDR port priority is %d\n\r", port, prio[0]);
         }
         break;
      case 3:
         port =conv_dec((char *)argv[1]);
         if(port >= DDR_PORT_INVALID){
            bbu_printf("ddrp-->  Port number is out of range\n\r");
            break;
         }
         prio[0] =conv_dec((char *)argv[2]);
         if(prio[0] >= DDR_PRIO_CRITICAL){
            bbu_printf("ddrp-->  Priority is out of range\n\r");
            break;
         }
         ddr_set_port_prio(port, prio[0]);
         bbu_printf("ddrp-->  DDR port priority is set to %d\n\r", port, prio[0]);
         break;
      case 10:
         if( strcmp((const char*)argv[1], "all") ){
            bbu_ddrp_help();
            break;
         }
         for(port = 0; port < DDR_PORT_INVALID; port++){
            prio[port] = conv_dec((char *)argv[2+port]);
            if(prio[port] >= DDR_PRIO_CRITICAL){
               bbu_printf("ddrp-->  Priority is out of range\n\r");
               return;
            }
         }
         ddr_set_port_prio_all(prio);
         bbu_printf("ddrp-->  All the 8 ports' prioritys are updated\n\r");
         break;
      default:
         bbu_ddrp_help();
         break;
   }
   return;
}

/* ........................................................................ */
void bbu_ddr_power_test(int argc, uint8_t **argv){
   int chip, power_state,zpr,znr,value;
   int phy_zpr, phy_znr;
   unsigned char bank_mask = 0;

   switch(argc){
      case 1:
      case 3:
         bbu_printf("error cmd formart\n\r");
	 break;

      case 2:
         if(check_help(argv[1])){
            bbu_ddr_power_help();
            break;
         }
	 if(strcmp((const char*)argv[1], "dump") == 0)
		dump_ddr_power_status();

	 else if((const char*)strcmp((const char*)argv[1], "mmusage") == 0){
		mem_dump_range_status();
	 }
	 else if((const char*)strcmp((const char*)argv[1], "testvm") == 0){
	 	test_vm();
	 }
	 else if((const char*)strcmp((const char*)argv[1], "dpmm") == 0){
	 	dump_used_vmas();
		dump_free_vmas();
	 }
	 else if((const char*)strcmp((const char*)argv[1], "testp") == 0){
		ddr_power_set_test();
	 }
	 else if((const char*)strcmp((const char*)argv[1], "cpumask") == 0){
		cpumask_test();
	 }
	 else if((const char*)strcmp((const char*)argv[1], "autor") == 0){
		set_ddr_auto_self_refresh(0);
	 }
	 else{
	 	bbu_printf("error cmd formart\n\r");
	 }
         break;

      case 4:
	 if(strcmp((const char*)argv[1], "set") == 0){

  	  chip =conv_dec((char *)argv[2]);
         if(chip > 3 || chip < 0){
            bbu_printf("chip number out of range\n\r");
            break;
         }
         power_state =conv_dec((char *)argv[3]);
         if(chip > 4 || chip < 0){
            bbu_printf("power state out of range\n\r");
            break;
         }
	 set_ddr_power_state(chip,power_state);
         break;
	 }else if(strcmp((const char*)argv[1], "pasr") == 0){
	 chip =conv_dec((char *)argv[2]);
         if(chip > 3 || chip < 0){
            bbu_printf("chip number out of range\n\r");
            break;
         }
         bank_mask =conv_hex((char *)argv[3]);
	 set_ddr_pasr(chip,bank_mask);
         break;
     } else if(strcmp((const char*)argv[1], "cal") == 0){
	 	 zpr =conv_dec((char *)argv[2]);
	 	 znr =conv_dec((char *)argv[3]);
	 	 bbu_printf("\n\rzpr is %d\n\r",zpr);
	 	 bbu_printf("znr is %d\n\r",znr);

	 	 value = BU_REG_READ(0xc010023c);
	 	 bbu_printf("0xc010023c: 0x%x\n\r",value);
	 	 value |= BIT_16;
	 	 BU_REG_WRITE(0xc010023c,value);
	 	 udelay(20);
	 	 value = BU_REG_READ(0xc010024c);
	 	 bbu_printf("0xc010024c: 0x%x\n\r",value);
 		 if(((value&BIT_3)!=0) || ((value&BIT_2)==0)) {
 		 	 bbu_printf("zpr/znr error\n\r");
 		 	 return;
 		 }

 		 phy_zpr = ((value >> 20)&0xf);
 		 phy_znr = ((value >> 16)&0xf);

 		 bbu_printf("phy_zpr: %d , phy_znr: %d\n\r", phy_zpr, phy_znr);

 		 if((zpr > (phy_zpr+3)) || (znr > (phy_znr+3))) {
 		 	bbu_printf("error zpr/znr, zpr/znr should <= %d/%d\n\r",
 		 			phy_zpr+3,phy_znr+3);
 		    return;
 		 }

 		 value = BU_REG_READ(0xc0100230);
	 	 bbu_printf("0xc0100230: 0x%x\n\r",value);
 		 value = BU_REG_READ(0xc0100230);
 		 value &= 0xfffff00f;
 		 value |= (zpr<<4);
 		 value |= (znr << 8);
 		 BU_REG_WRITE(0xc0100230,value);
 		 value = BU_REG_READ(0xc0100230);
	 	 bbu_printf("0xc0100230: 0x%x\n\r",value);

 		 value = BU_REG_READ(0xc0100234);
	 	 bbu_printf("0xc0100234: 0x%x\n\r",value);
 		 value = BU_REG_READ(0xc0100234);
 		 value &= 0xfffff00f;
 		 value |= (zpr<<4);
 		 value |= (znr << 8);
 		 BU_REG_WRITE(0xc0100234,value);
 		 value = BU_REG_READ(0xc0100234);
	 	 bbu_printf("0xc0100234: 0x%x\n\r",value);

	 	 value = BU_REG_READ(0xc0100238);
	 	 bbu_printf("0xc0100238: 0x%x\n\r",value);
	 	 value = BU_REG_READ(0xc0100238);
 		 value &= 0xffffff00;
 		 value |= (zpr<<0);
 		 value |= (znr << 4);
 		 BU_REG_WRITE(0xc0100238,value);
 		 value = BU_REG_READ(0xc0100238);
	 	 bbu_printf("0xc0100238: 0x%x\n\r",value);

	 	 bbu_printf("now disable pad_cal_auto\n\r");
	 	 value = BU_REG_READ(0xc010023c);
	 	 bbu_printf("0xc010023c: 0x%x\n\r",value);
	 	 value &= ~BIT_16;
	 	 BU_REG_WRITE(0xc010023c,value);
	 	 value = BU_REG_READ(0xc010023c);
	 	 bbu_printf("0xc010023c: 0x%x\n\r",value);
         break;
	 }else{
	 	bbu_printf("error cmd formart\n\r");
	 	break;
	 }
      default:
         bbu_ddr_power_help();
         break;
   }
   return;
}

#ifdef CONFIG_HSI
/*
*hsi_test_argv[0]:
*hsi_test_argv[1]:start_channel
*hsi_test_argv[2]:test mode
*hsi_test_argv[3]:burst_size
*hsi_test_argv[4]:end_channel
*hsi_test_argv[5]:is_desc,pass 0
*hsi_test_argv[6]:is_lpbk
*hsi_test_argv[7]:int_num, pass 0
*/
char hsi_test_argv[MAX_PARMS][MAX_CHARS_PER_PARM] = {
		{"0"},
        {"1"},
        {"1"}, /*dma int mode*/
        {"6"},
        {"0"},
        {"0"},
        {"0"},
        {"0"},
};

/* ........................................................................ */
void bbu_hsi_test(int argc, uint8_t **argv){
   int i,retval;

   switch(argc){
      case 1:
         bbu_hsi_help();
	 	 break;

      case 2:
         if(check_help(argv[1])){
            bbu_hsi_help();
            break;
         }

	 	if(strcmp((const char*)argv[1], "1") == 0) {

	    for(i = 0;i < 1; i++)
	    {
	    	sprintf(&hsi_test_argv[1][0],"%d",0);
	    	sprintf(&hsi_test_argv[4][0],"%d",7);
			retval = mipihsi_test(8, hsi_test_argv);
			if(retval == MV_OK)
				bbu_printf("HSI PASS.\n\r");
			else
				bbu_printf("HSI FAIL.\n\r");
		}

	 	}else{
	 		bbu_printf("error cmd formart\n\r");
		}
         break;

      default:
         bbu_hsi_help();
         break;
   }
   return;
}
#endif
/* ........................................................................ */
void bbu_ddrw(int argc, uint8_t **argv){
   uint8_t wght[8];
   int port;
   switch(argc){
      case 1:
         bbu_printf("\n\rddrw-->  Current ports' weight is: "
                    "(0~15)\n\r");
         for(port = 0; port < DDR_PORT_INVALID; port++){
            wght[0] = ddr_get_port_weight(port);
            bbu_printf("ddrw-->  DDR port weight is %d\n\r", port, wght[0]);
         }
      case 2:
         if(check_help(argv[1])){
            bbu_ddrw_help();
            break;
         }
         port =conv_dec((char *)argv[1]);
         if(port >= DDR_PORT_INVALID)
            bbu_printf("ddrw-->  Port number is out of range\n\r");
         else{
            wght[0] = ddr_get_port_weight(port);
            bbu_printf("ddrw-->  DDR port weight is %d\n\r", port, wght[0]);
         }
         break;
      case 3:
         port =conv_dec((char *)argv[1]);
         if(port >= DDR_PORT_INVALID){
            bbu_printf("ddrw-->  Port number is out of range\n\r");
            break;
         }
         wght[0] =conv_dec((char *)argv[2]);
         if(wght[0] >= 0x10){
            bbu_printf("ddrw-->  weight is out of range\n\r");
            break;
         }
         ddr_set_port_weight(port, wght[0]);
         bbu_printf("ddrw-->  DDR port weight is set to %d\n\r", port, wght[0]);
         break;
      case 10:
         if(strcmp((const char*)argv[1], "all")){
            bbu_ddrw_help();
            break;
         }
         for(port = 0; port < DDR_PORT_INVALID; port++){
            wght[port] = conv_dec((char *)argv[2+port]);
            if(wght[port] >= 0x10){
               bbu_printf("ddrw-->  weight is out of range\n\r");
               return;
            }
         }
         ddr_set_port_weight_all(wght);
         bbu_printf("ddrw-->  All the 8 ports' weights are updated\n\r");
         break;
      default:
         bbu_ddrw_help();
         break;
   }
   return;
}

/* ........................................................................ */
void bbu_durm(int argc, uint8_t **argv){
   switch(argc){
      case 1:
         bbu_durm_help();
         break;
      case 2:
         if(strcmp((const char*)argv[1], "start") == 0 ||
         	  strcmp((const char*)argv[1], "1") == 0)
            ddr_utilization_measure_start(7);
         else if(strcmp((const char*)argv[1], "update") == 0 ||
         	       strcmp((const char*)argv[1], "2") == 0){
         	     bbu_durm_help();
         }
         else if(strcmp((const char*)argv[1], "stop") == 0 ||
         	       strcmp((const char*)argv[1], "0") == 0)
         	  ddr_utilization_measure_stop();
         else
            bbu_durm_help();
         break;
      case 3:
         if(strcmp((const char*)argv[1], "start") == 0 ||
         	  strcmp((const char*)argv[1], "1") == 0){
            uint8_t div = conv_dec((char *)argv[2]);
            ddr_utilization_measure_start(div);
         }
         break;
      default:
         bbu_durm_help();
         break;
   }
}

/* ........................................................................ */
void bbu_xtc_test(int argc, uint8_t **argv){
   uint32_t xtc;

   switch(argc){
      case 1:
         bbu_printf("error cmd formart\n\r");
	  break;

      case 2:
         if(check_help(argv[1]))
         {
            bbu_xtc_help();
            break;
         }
         else if(strcmp((const char*)argv[1], "/d") == 0)
         {
             bbu_printf("\n\r******************* XTC reg dump ********************\n\r");
             bbu_printf("        MC_CONF(0x%x): 	0x%x\n\r",&CIU->MC_CONF,CIU->MC_CONF);
             bbu_printf("     GPU_2D_XTC(0x%x): 	0x%x\n\r",&CIU->GPU_2D_XTC,CIU->GPU_2D_XTC);
              bbu_printf("    GPU_3D_XTC(0x%x): 	0x%x\n\r",&CIU->GPU_3D_XTC,CIU->GPU_3D_XTC);
             bbu_printf("    VPU_XTC_REG(0x%x): 	0x%x\n\r",&CIU->VPU_XTC_REG,CIU->VPU_XTC_REG);
             bbu_printf("CPU_CONF_SRAM_0(0x%x): 	0x%x\n\r",&CIU->CA7_CPU_CONF_SRAM_0,CIU->CA7_CPU_CONF_SRAM_0);
             bbu_printf("CPU_CONF_SRAM_1(0x%x): 	0x%x\n\r",&CIU->CA7_CPU_CONF_SRAM_1,CIU->CA7_CPU_CONF_SRAM_1);
		 	 bbu_printf("***************** XTC reg dump end ******************\n\r");
         }
         else
         {
            bbu_printf("error cmd formart\n\r");
         }
         break;

       case 3:
	  	if(strcmp((const char*)argv[1], "gcrf2p") == 0)
		{
            xtc = conv_dec((char *)argv[2]);
            if(xtc > 3)
            	xtc = 3;
            CIU->GPU_2D_XTC &= 0xFFF000FF; //Clear bit8->bit19;
            CIU->GPU_3D_XTC &= 0xFFF000FF; //Clear bit8->bit19;
            bbu_printf("xtc: set gcrf2p to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->GPU_2D_XTC |= 0x00055500;
            	CIU->GPU_3D_XTC |= 0x00055500;
            	break;

            	case 2:
            	CIU->GPU_2D_XTC |= 0x000AAA00;
            	CIU->GPU_3D_XTC |= 0x000AAA00;
            	break;

            	case 3:
            	CIU->GPU_2D_XTC |= 0x000FFF00;
            	CIU->GPU_3D_XTC |= 0x000FFF00;
            	break;
            }

		}
		else if(strcmp((const char*)argv[1], "gcrf1p") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->GPU_2D_XTC &= 0xFFFFFF00; //Clear bit0->bit7;
            CIU->GPU_3D_XTC &= 0xFFFFFF00; //Clear bit0->bit7;
            bbu_printf("xtc: set gcrf1p to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->GPU_2D_XTC |= 0x00000055;
            	CIU->GPU_3D_XTC |= 0x00000055;
            	break;

            	case 2:
            	CIU->GPU_2D_XTC |= 0x000000AA;
            	CIU->GPU_3D_XTC |= 0x000000AA;
            	break;

            	case 3:
            	CIU->GPU_2D_XTC |= 0x000000FF;
            	CIU->GPU_3D_XTC |= 0x000000FF;
            	break;
            }
		}

		else if(strcmp((const char*)argv[1], "vpurf1p") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->VPU_XTC_REG &= 0xFFFFFFF0; //Clear bit0->bit7;
            bbu_printf("xtc: set vpurf1p to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->VPU_XTC_REG |= 0x00000005;
            	break;

            	case 2:
            	CIU->VPU_XTC_REG |= 0x0000000A;
            	break;

            	case 3:
            	CIU->VPU_XTC_REG |= 0x0000000F;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "vpurf2p") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->VPU_XTC_REG &= 0xFFFFFF0F; //Clear bit0->bit7;
            bbu_printf("xtc: set vpusr2p to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->VPU_XTC_REG |= 0x00000050;
            	break;

            	case 2:
            	CIU->VPU_XTC_REG |= 0x000000A0;
            	break;

            	case 3:
            	CIU->VPU_XTC_REG |= 0x000000F0;
            	break;
            }
		}

		else if(strcmp((const char*)argv[1], "vpusrrtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->VPU_XTC_REG &= 0xFFFFCCFF; //Clear bit0->bit7;
            bbu_printf("xtc: set vpusrRTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->VPU_XTC_REG |= 0x00001100;
            	break;

            	case 2:
            	CIU->VPU_XTC_REG |= 0x00002200;
            	break;

            	case 3:
            	CIU->VPU_XTC_REG |= 0x00003300;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "vpusrwtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->VPU_XTC_REG &= 0xFFFF33FF; //Clear bit0->bit7;
            bbu_printf("xtc: set vpusrWTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->VPU_XTC_REG |= 0x00004400;
            	break;

            	case 2:
            	CIU->VPU_XTC_REG |= 0x00008800;
            	break;

            	case 3:
            	CIU->VPU_XTC_REG |= 0x0000CC00;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "l1xtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_CONF_SRAM_0 &= 0x00000FFF; //Clear bit12->bit32;
            bbu_printf("xtc: set L1XTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0x55555000;
            	break;

            	case 2:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0xAAAAA000;
            	break;

            	case 3:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0xFFFFF000;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "mmuxtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_CONF_SRAM_0 &= 0xFFFFF0FF; //Clear bit8->bit11;
            bbu_printf("xtc: set mmuXTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0x00000500;
            	break;

            	case 2:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0x00000A00;
            	break;

            	case 3:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0x00000F00;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "brxtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_CONF_SRAM_0 &= 0xFFFFFF0F; //Clear bit12->bit32;
            bbu_printf("xtc: set branch_memXTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0x00000050;
            	break;

            	case 2:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0x000000A0;
            	break;

            	case 3:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0x000000F0;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "hbxtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_CONF_SRAM_0 &= 0xFFFFFFF0; //Clear bit12->bit32;
            bbu_printf("xtc: set history_buffer_memXTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0x00000005;
            	break;

            	case 2:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0x0000000A;
            	break;

            	case 3:
            	CIU->CA7_CPU_CONF_SRAM_0 |= 0x0000000F;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "scuxtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_CONF_SRAM_1 &= 0xFFFF0FFF; //Clear bit12->bit15;
            bbu_printf("xtc: set scuXTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_CONF_SRAM_1 |= 0x00005000;
            	break;

            	case 2:
            	CIU->CA7_CPU_CONF_SRAM_1 |= 0x0000A000;
            	break;

            	case 3:
            	CIU->CA7_CPU_CONF_SRAM_1 |= 0x0000F000;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "l2rtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_CONF_SRAM_1 &= 0xFFFFFCCC; //Clear bit12->bit15;
            bbu_printf("xtc: set L2RTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_CONF_SRAM_1 |= 0x00000111;
            	break;

            	case 2:
            	CIU->CA7_CPU_CONF_SRAM_1 |= 0x00000222;
            	break;

            	case 3:
            	CIU->CA7_CPU_CONF_SRAM_1 |= 0x00000333;
            	break;
            }
		}

		else if(strcmp((const char*)argv[1], "l2wtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_CONF_SRAM_1 &= 0xFFFFF333; //Clear bit12->bit15;
            bbu_printf("xtc: set L2WTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_CONF_SRAM_1 |= 0x00000444;
            	break;

            	case 2:
            	CIU->CA7_CPU_CONF_SRAM_1 |= 0x00000888;
            	break;

            	case 3:
            	CIU->CA7_CPU_CONF_SRAM_1 |= 0x00000CCC;
            	break;
            }
		}

		else if(strcmp((const char*)argv[1], "mcxtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->MC_CONF &= 0xFC3FFFFF; //Clear bit22->bit25;
            bbu_printf("xtc: set mcXTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->MC_CONF |= 0x01400000;
            	break;

            	case 2:
            	CIU->MC_CONF |= 0x02800000;
            	break;

            	case 3:
            	CIU->MC_CONF |= 0x03C00000;
            	break;
            }
		}
		break;

      default:
         bbu_xtc_help();
         break;
   }

   bbu_printf("PASS\n\r");
   return;
}
#endif                                         /* end of ifndef CONFIG_NANO */

/* ........................................................................ */
uint32_t get_chip_id(void)
{
   return CIU->CHIP_ID;
}

/* ........................................................................ */
int bbu_pid(uint32_t *type, uint32_t *step)
{
   uint32_t chip_id = get_chip_id();
   uint32_t pid = chip_id & 0xFFFF;

   *type = pid;
   bbu_printf("Marvell Processor: ");
   switch(pid){
      case 0x1822:
         pid = 0x1822;
         break;
      case 0x1826:
         pid = 0x1826;
         break;
      default:
         bbu_printf("unknown chip id 0x%X\n\r", pid);
         return SCREEN_TEST_FIAL;
   }
   bbu_printf("PXA%X, Revision: ", pid);
   chip_id = (chip_id >> 16) & 0xFF; /* Get revision */
   *step = chip_id;
   switch(chip_id){
      case 0xA0:
         bbu_printf("A0\n\r");
         break;
      case 0xF0:
          bbu_printf("Z1\n\r");
         break;
      case 0xF2:
          bbu_printf("Z2\n\r");
         break;
      case 0xF3:
          bbu_printf("Z3\n\r");
         break;
      default:
         bbu_printf("Unknown 0x%X\n\r", chip_id);
         break;
   }
   return SCREEN_TEST_PASS;
}

#ifndef CONFIG_SWP_DDR_ONLY
int bbu_pid_test(int argc, uint8_t **argv)
{
   (void)argc;
   (void)argv;
   uint32_t cpu_type = 0, cpu_step = 0;
   bbu_pid(&cpu_type, &cpu_step);
   return 0;
}
#endif

#ifdef QF_STACK_PER_AO
int bbu_stack(int argc, uint8_t **argv){
   if(argc == 3){
      QActive *act = (QActive *)ao_info_translate((void *)argv[2], 1);
      if(act == 0){
         bbu_printf("This AO doesn't exist\n\r");
         bbu_stack_help();
         return 0;
      }
      if(!strcmp((const char*)argv[1], "o")){
         if(QA_stkCheckOverflow(act))
            bbu_printf("AO %s stack overflows\n\r", argv[2]);
         else
            bbu_printf("AO %s stack may not overflow\n\r", argv[2]);
      }else if(!strcmp((const char*)argv[1], "s")){
         int size = QA_stkUsedSize(act);
         bbu_printf("%d bytes have been used of AO %s stack\n\r",
                    size, argv[2]);
      }else if(!strcmp((const char*)argv[1], "i")){
         bbu_printf("The stack of AO %s is from 0x%08X to 0x%08X(%dKB)\n\r",
                   argv[2], act->stk, act->stk - act->stkSize,
                   act->stkSize/0x400);
      }else{
         bbu_printf("The <opt> is not correct\n\r");
         bbu_stack_help();
      }
   }else{
      bbu_stack_help();
   }
   return 1;
}
#endif

int bbu_print_level = INFO_LEVEL;
#ifndef CONFIG_NANO
/* ........................................................................ */
int bbu_print_switch(int argc, uint8_t **argv){
   if(argc == 2){
      if(check_help((uint8_t *)argv[1]))
         bbu_print_help();
      else if(!strcmp((const char*)argv[1], "dbg")){
         bbu_print_level = DBG_LEVEL;
         bbu_printf("Set as DEBUG print level\n\r");
      }
      else if(!strcmp((const char*)argv[1], "info")){
      	 bbu_print_level = INFO_LEVEL;
      	 bbu_printf("Set as INFOMATION print level\n\r");
      }
      else if(!strcmp((const char*)argv[1], "crit")){
         bbu_print_level = CRIT_LEVEL;
         bbu_printf("Set as CRITICAL print level\n\r");
      }
      else if(!strcmp((const char*)argv[1], "off") ){
         bbu_print_level = CRIT_LEVEL+1;
         bbu_printf("All level logs are turned off\n\r");
      }
      else{
         uint8_t level = conv_dec((char *)argv[1]);
         if(level <= (CRIT_LEVEL+1)){
            bbu_print_level = level;
            bbu_printf("Print level is set to %d\n\r",level);
         }else
            bbu_printf("The level %d is out of range\n\r", level);
      }
   }else{
      bbu_print_help();
   }
   return MV_OK;
}
#endif

static int get_board_version(void)
{
    return NZA3_DKB_V10;
}

static int board_version = NZA3_DKB_V10;

void bbu_bid(void)
{
   	board_version = get_board_version();
   	bbu_printf("Board Version:");
      	switch(board_version){
         case NZA3_DKB_V10:
         	bbu_printf("NEZHA3_DKB V1.0\n\r");
         	break;

         default:
            board_version = NZA3_DKB_V10;
            break;
      }
}

int board_is_nza3_dkb_v10(void)
{
	if(board_version == NZA3_DKB_V10)
	{
		return 1;
	}else{
		return 0;
	}
}
