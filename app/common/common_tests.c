#include "predefines.h"
#include <string.h>
#include "bbu.h"
#include "pmic.h"
#include "timer_dec.h"
#include "CIU.h"
#include "common_tests.h"
#include "i2c_dec.h"
#include "spinlock.h"
#include "gpio_dec.h"
#include "utils.h"
#include "cache-test.h"
#include "cp15.h"
#include "ipc_app.h"
#include "interrupt.h"
#include "uart_dec.h"

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


/* ........................................................................ */
void bbu_clear_screen(void)
{
   bbu_printf("%c[H", 27);   //clear screen
   bbu_printf("%c[J", 27);   //go to top
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
             bbu_printf("CA7_CPU_SRAM_CONF_0(0x%x): 	0x%x\n\r",&CIU->CA7_CPU_SRAM_CONF_0,CIU->CA7_CPU_SRAM_CONF_0);
             bbu_printf("CA7_CPU_SRAM_CONF_1(0x%x): 	0x%x\n\r",&CIU->CA7_CPU_SRAM_CONF_1,CIU->CA7_CPU_SRAM_CONF_1);
		 	 bbu_printf("***************** XTC reg dump end ******************\n\r");
         }
         else
         {
            bbu_printf("error cmd formart\n\r");
         }
         break;

       case 3:
        if(strcmp((const char*)argv[1], "l1xtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_SRAM_CONF_0 &= 0x00000FFF; //Clear bit12->bit32;
            bbu_printf("xtc: set L1XTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0x55555000;
            	break;

            	case 2:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0xAAAAA000;
            	break;

            	case 3:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0xFFFFF000;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "mmuxtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_SRAM_CONF_0 &= 0xFFFFF0FF; //Clear bit8->bit11;
            bbu_printf("xtc: set mmuXTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0x00000500;
            	break;

            	case 2:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0x00000A00;
            	break;

            	case 3:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0x00000F00;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "brxtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_SRAM_CONF_0 &= 0xFFFFFF0F; //Clear bit12->bit32;
            bbu_printf("xtc: set branch_memXTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0x00000050;
            	break;

            	case 2:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0x000000A0;
            	break;

            	case 3:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0x000000F0;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "hbxtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_SRAM_CONF_0 &= 0xFFFFFFF0; //Clear bit12->bit32;
            bbu_printf("xtc: set history_buffer_memXTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0x00000005;
            	break;

            	case 2:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0x0000000A;
            	break;

            	case 3:
            	CIU->CA7_CPU_SRAM_CONF_0 |= 0x0000000F;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "scuxtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_SRAM_CONF_1 &= 0xFFFF0FFF; //Clear bit12->bit15;
            bbu_printf("xtc: set scuXTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_SRAM_CONF_1 |= 0x00005000;
            	break;

            	case 2:
            	CIU->CA7_CPU_SRAM_CONF_1 |= 0x0000A000;
            	break;

            	case 3:
            	CIU->CA7_CPU_SRAM_CONF_1 |= 0x0000F000;
            	break;
            }
		}
		else if(strcmp((const char*)argv[1], "l2rtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_SRAM_CONF_1 &= 0xFFFFFCCC; //Clear bit12->bit15;
            bbu_printf("xtc: set L2RTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_SRAM_CONF_1 |= 0x00000111;
            	break;

            	case 2:
            	CIU->CA7_CPU_SRAM_CONF_1 |= 0x00000222;
            	break;

            	case 3:
            	CIU->CA7_CPU_SRAM_CONF_1 |= 0x00000333;
            	break;
            }
		}

		else if(strcmp((const char*)argv[1], "l2wtc") == 0)
		{
        	xtc = conv_dec((char *)argv[2]);

            if(xtc > 3)
            	xtc = 3;

            CIU->CA7_CPU_SRAM_CONF_1 &= 0xFFFFF333; //Clear bit12->bit15;
            bbu_printf("xtc: set L2WTC to %d\n\r",xtc);

            switch(xtc)
            {
            	case 0:
            	break;

            	case 1:
            	CIU->CA7_CPU_SRAM_CONF_1 |= 0x00000444;
            	break;

            	case 2:
            	CIU->CA7_CPU_SRAM_CONF_1 |= 0x00000888;
            	break;

            	case 3:
            	CIU->CA7_CPU_SRAM_CONF_1 |= 0x00000CCC;
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
   switch(pid){
      case 0x1822:
      case 0x1826:
      case 0x1802:
      case 0x6731:
         break;
      default:
         bbu_printf("ASR Processor: unknown chip id 0x%X\n\r", pid);
         return SCREEN_TEST_FIAL;
   }
   bbu_printf("ASR Processor: PXA%X, Revision: ", pid);
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
   cp15_reg_read(c0, c0, 0, pid);
   pid &= 0xFFF0;
   pid >>= 4;
   if(pid == 0xC15){   /* Current Core is CR5 */
       bbu_printf("Current core is Cortex-R5\n\r");
   }else if(pid == 0xC07){
       bbu_printf("Current core is Cortex-A7\n\r");
   }else{
       bbu_printf("Can't identify current core\n\r");
   }
   return SCREEN_TEST_PASS;
}

int bbu_pid_test(int argc, uint8_t **argv)
{
   (void)argc;
   (void)argv;
   uint32_t cpu_type = 0, cpu_step = 0;
   bbu_pid(&cpu_type, &cpu_step);
   return 0;
}

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

static int get_board_version(void)
{
#if 0
	uint32_t version_flag = 0;

	if(gpio_get_pin_level(GPIO126) == 1)
		version_flag |= BIT_0;
	if(gpio_get_pin_level(GPIO0) == 1)
		version_flag |= BIT_1;
	if(gpio_get_pin_level(GPIO2) == 1)
		version_flag |= BIT_2;
	switch(version_flag)
	{
		case 0:
			return NZA3_DKB_V10;
		default:
			return NZA3_DKB_V10;
	}
#endif
	uint8_t value;
	int raw_value,value_32;

	/*read GPADC3*/
        if(pmic_is_procida())
	{
		pm8xx_gpadc_read(0x5A, &value);
		raw_value = (value<<4);
		pm8xx_gpadc_read(0x5B, &value);
		raw_value |=(value&0xf) ;
	
		value_32 = (1400 *raw_value)>>12;
		//bbu_printf("Gpadc3 minivolts: %d mv\n\r",value_32);

	    	//V20: 1.2V
	    	if((value_32 > 500)&&(value_32 < 750))
		    return NZA3_DKB_V20B;
    		else if((value_32 > 1100)&&(value_32 < 1275))
		    return NZA3_DKB_V20A;
		else 
		    return NZA3_DKB_V21A;
	}

	return NZA3_DKB_V10;

}

static int board_version = NZA3_DKB_V10;
void bbu_bid(void)
{
   	board_version = get_board_version();
   	bbu_printf("Marvell DKB Board Version:");
      	switch(board_version){
         case NZA3_DKB_V10:
         	bbu_printf("NEZHA3 DKB V1.0\n\r");
         	break;
         case NZA3_DKB_V20A:
         	bbu_printf("NEZHA3 DKB V2.0A\n\r");
         	break;
         case NZA3_DKB_V20B:
         	bbu_printf("NEZHA3 DKB V2.1B\n\r");
         	break;
         case NZA3_DKB_V21A:
         	bbu_printf("NEZHA3 DKB V2.1A\n\r");
         	break;
         default:
            board_version = NZA3_DKB_V10;
            bbu_printf("Use NEZHA3_DKB_V10 by default\n\r");
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

int board_is_nza3_dkb_v20a(void)
{
	if(board_version == NZA3_DKB_V20A)
	{
		return 1;
	}else{
		return 0;
	}
}

int board_is_nza3_dkb_v21a(void)
{
	if(board_version == NZA3_DKB_V21A)
	{
		return 1;
	}else{
		return 0;
	}
}

int board_is_nza3_dkb_v20b(void)
{
	if(board_version == NZA3_DKB_V20B)
	{
		return 1;
	}else{
		return 0;
	}
}

#ifdef CONFIG_NATO
#include "ICU_NATO.h"
#else
#include "ICU.h"
#endif

#include "interrupt.h"

#ifdef CONFIG_CR5
extern int uart_polling;
#endif

int bbu_uart_switch(int argc, uint8_t **argv)
{
    uint32_t val;
    uint32_t reg;
    cp15_reg_read(c0, c0, 0, val);
    val &= 0xFFF0;
    val >>= 4;
    switch(argc){
        case 1:
            if(val == 0xC15){   /* Current Core is CR5 */
                bbu_printf("uart-->  Current core is Cortex-R5\n\r");
            }else if(val == 0xC07){
                bbu_printf("uart-->  Current core is Cortex-A7\n\r");
            }else{
                bbu_printf("uart-->  Can't identify current core\n\r");
            }
            break;
        case 2:
            if((!strcmp((const char*)argv[1], "ap")) ||
               (!strcmp((const char*)argv[1], "ca7")))
            {
               uart_polling = 0;
               POST_CMD2CLI("cmda uart update");
            }else if((!strcmp((const char*)argv[1], "cp")) ||
                    (!strcmp((const char*)argv[1], "cr5")))
            {
               if(UART_OWNER != 2){
                  bbu_printf("Should not happen\n\r");
               }
               bbu_printf("uart-->  UART owner is set to be CP\n\r");
            }else if((!strcmp((const char*)argv[1], "dsp")) ||
                    (!strcmp((const char*)argv[1], "msa")))
            {
               uart_polling = 0;
               POST_CMD2CLI("cmdd uart update");
            }else if(!strcmp((const char*)argv[1], "update"))
            {
               uart_polling = 1;
               UART_OWNER = 2;
               bbu_printf("uart-->  UART owner is set to be CP\n\r");
            }else{
                bbu_uart_help();
            }
            break;
        default:
            bbu_uart_help();
            break;
    }
    return SCREEN_TEST_PASS;
}

int bbu_sctoc_help(int argc, uint8_t **argv)
{
   char cmdstr[256] = {0};
   int len, idx = 0;
   for(int i = 1; i < argc; i++){
      len = strlen(argv[i]);
      for(int j = 0; j < len; j++){
         cmdstr[idx++] = argv[i][j];
      }
      cmdstr[idx++] = ' ';
   }
   cmdstr[idx++] = '\0';
   ComCliEvt *sce = QIPC_NEW(ComCliEvt, COM_RX_CMD_READY_SIG);
   sce->len = sizeof(cmdstr);
   sce->source = 0;  /* differ with command join in auto screen test */
   strcpy((char*)sce->CliBuffer, cmdstr);
   QIPC_sendFIFO(IPC2AP, (QEvent *)sce);
   return SCREEN_TEST_PASS;
}

int bbu_scmd2msa(int argc, uint8_t **argv)
{
   char cmdstr[256] = {0};
   int len, idx = 0;
   for(int i = 1; i < argc; i++){
      len = strlen(argv[i]);
      for(int j = 0; j < len; j++){
         cmdstr[idx++] = argv[i][j];
      }
      cmdstr[idx++] = ' ';
   }
   cmdstr[idx++] = '\0';
   ComCliEvt *sce = QIPC_NEW(ComCliEvt, COM_RX_CMD_READY_SIG);
   sce->len = sizeof(cmdstr);
   sce->source = 0;  /* differ with command join in auto screen test */
   strcpy((char*)sce->CliBuffer, cmdstr);
   QIPC_sendFIFO(IPC2DSP, (QEvent *)sce);
   return SCREEN_TEST_PASS;
}

int cpu_is_ca7(void)
{
   uint32_t cid;
   cp15_reg_read(c0, c0, 0, cid);
   cid &= 0xFFF0;
   cid >>= 4;
   return (cid == 0xC07);
}


int cpu_is_cr5(void)
{
   uint32_t cid;
   cp15_reg_read(c0, c0, 0, cid);
   cid &= 0xFFF0;
   cid >>= 4;
   return (cid == 0xC15);
}

static display_32_bitmap(uint32_t val)
{
   bbu_printf("\r31  28 27  24 23  20 19  16 15  12 11  8   7  4   3  0\n\r");
   for(int i = 32; i > 0; i--)
   {
      if(((i)%4) == 0)
         bbu_printf("[");
      if(val & (1 << (i-1)))
         bbu_printf("1");
      else
         bbu_printf("0");
      if(((i-1)%4) == 0)
         bbu_printf("] ");
   }
   bbu_printf("\n\r");
}

int bbu_sbit_test(int argc, uint8_t **argv){
   if(argc < 3)
   {
      bbu_sbit_help();
      return SCREEN_TEST_PASS;
   }

   uint32_t reg, val, bit;
   reg = XpStrToValue((char *)argv[1]);
   val = BU_REG_READ(reg);
   bbu_printf("sbit-->  0x%08X:0x%08X\n\r", reg, val);
   display_32_bitmap(val);

   for(int i = 2; i < argc; i++)
   {
      bit = conv_dec((char *)argv[i]);
      val |= (1 << bit);
      bbu_printf("sbit-->  Bit %d of 0x%08X is set\n\r", bit, reg);
   }
   BU_REG_WRITE(reg, val);

   bbu_printf("sbit-->  0x%08X:0x%08X\n\r", reg, val);
   display_32_bitmap(val);

   return SCREEN_TEST_PASS;
}

int bbu_cbit_test(int argc, uint8_t **argv){
   if(argc < 3)
   {
      bbu_cbit_help();
      return SCREEN_TEST_PASS;
   }

   uint32_t reg, val, bit;
   reg = XpStrToValue((char *)argv[1]);
   val = BU_REG_READ(reg);
   bbu_printf("cbit-->  0x%08X:0x%08X\n\r", reg, val);
   display_32_bitmap(val);

   for(int i = 2; i < argc; i++)
   {
      bit = conv_dec((char *)argv[i]);
      val &= ~(1 << bit);
      bbu_printf("cbit-->  Bit %d of 0x%08X is cleared\n\r", bit, reg);
   }
   BU_REG_WRITE(reg, val);

   bbu_printf("cbit-->  0x%08X:0x%08X\n\r", reg, val);
   display_32_bitmap(val);

   return SCREEN_TEST_PASS;
}

int bbu_rbit_test(int argc, uint8_t **argv){
   if(argc < 2)
   {
      bbu_rbit_help();
      return SCREEN_TEST_PASS;
   }

   uint32_t reg, val, bit;
   reg = XpStrToValue((char *)argv[1]);
   val = BU_REG_READ(reg);
   bbu_printf("rbit-->  0x%08X:0x%08X\n\r", reg, val);
   display_32_bitmap(val);

   if(argc < 3)
      return SCREEN_TEST_PASS;

   for(int i = 2; i < argc; i++)
   {
      bit = conv_dec((char *)argv[i]);
      if(val & (1 << bit))
         bbu_printf("rbit-->  Bit %d of 0x%08X is set\n\r", bit, reg);
      else
         bbu_printf("rbit-->  Bit %d of 0x%08X is cleared\n\r", bit, reg);
   }

   return SCREEN_TEST_PASS;
}

#if 0
void screen_cmd_ack(uint8_t result, uint8_t cmd_index)
{

}
#endif