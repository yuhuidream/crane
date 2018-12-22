#include "predefines.h"
#include "bbu.h"
#include <string.h>

void bbu_peek_help(void){
#ifndef CONFIG_NANO_NOLOG
   bbu_printf("\n\r peek - Examine the data at the specified address.\n\r");
   bbu_printf(" Usage: peek [/x=] <address> <num>\n\r");
   bbu_printf("  Where x may be used to specify a PEEK at a byte or word address.\n\r");
   bbu_printf("  /b= address specifies a byte (8-bits) of data to be read.\n\r");
   bbu_printf("  /w= address specifies a half-word (16-bits) of data to be read.\n\r");
   bbu_printf("  An address with no switch defaults to 32-bits of data to be read.\n\r");
   bbu_printf("  <address>   -- The address to be examined.\n\r");
   bbu_printf("  <num>       -- The number of data to be examined.\n\r");
#endif
   return;
}

void bbu_poke_help(void){
#ifndef CONFIG_NANO_NOLOG
   bbu_printf("\n\r poke - Deposits byte, half word, or word data into the specified address.\n\r");
   bbu_printf(" Usage: poke [/x=] <address> <data>\n\r");
   bbu_printf("  Where x may be used to specify a POKE at a byte or word address.\n\r");
   bbu_printf("  /b= address specifies a byte (8-bits) of data is to be deposited.\n\r");
   bbu_printf("  /w= address specifies a half-word (16-bits) of data is to be deposited.\n\r");
   bbu_printf("  An address with no switch defaults to 32-bits of data to be deposited.\n\r");
   bbu_printf("  <address>   -- The address where the data is to be deposited\n\r");
   bbu_printf("  <data>      -- The data to be deposited at the specified address.\n\r");
#endif
   return;
}



/* ........................................................................ */
void bbu_mstp_help(void)
{
#ifndef CONFIG_NANO_NOLOG
   bbu_printf("\n\r mtsp - Memory Test Subroutine Package\n\r");
   bbu_printf("  The MTSP command can run a suite of up to 30 "
              "different typs of\n\r");
   bbu_printf("  memory tests using this one command. For a list "
              "of the memory tests\n\r");
   bbu_printf("  refer to the BBU User's Guide. Avoid running "
              "this test in memory\n\r");
   bbu_printf("  locations where BBU is running!\n\r");
   bbu_printf(" Usage: mtsp <address> <size> <tests> [<test times>]\n\r");
   bbu_printf("  address    = Starting address of the memory "
              "area to be tested.\n\r");
   bbu_printf("  size       = Size of memory to be tested, in bytes.\n\r");
   bbu_printf("  tests      = Each bit set will select a specified "
              "memory test, Use the\n\r");
   bbu_printf("              value 0xFFFF_FFFF to run all memory tests.\n\r");
   bbu_printf("  test times = number for repeating tests. default: 1\n\r");
   bbu_printf(" Note: If <address> = dynamic(d), will dynamically allocate a buffer "
              "\n\r       of desired size for test\n\r");
#endif
}

/*................. BBU setv test command help information..................*/
void bbu_setv_help(void){
#ifndef CONFIG_NANO_NOLOG
   bbu_printf("\n\r setv - Command to set V_BUCK voltage.\n\r");
   bbu_printf(" Usage: setv <v_buck> [<voltage>] [<set_mode>]\n\r");
   bbu_printf("  <v_buck>: \n\r"
              "    buck1 or 1  -- V_BUCK1 for Vcc_main\n\r");
   bbu_printf("    buck2 or 2  -- V_BUCK2\n\r"); 
   bbu_printf("    buck3 or 3  -- V_BUCK3\n\r");
   bbu_printf("    buck4 or 4  -- V_BUCK4\n\r");
   bbu_printf("    buck5 or 5  -- V_BUCK5\n\r");
   bbu_printf("  <voltage>: 600 ~ 3950 (mv), voltage step is 12.5 mv\n\r");
   bbu_printf("             If <voltage> is not an integer, please round it\n\r");
   bbu_printf("  <set_mode>: Only for setting V_BUCK1,V_BUCK3 and V_BUCK5\n\r");
   bbu_printf("    dvc00 or 00  --  DVC2 = 0 and DVC1 = 0\n\r");
   bbu_printf("    dvc01 or 01  --  DVC2 = 0 and DVC1 = 1\n\r");
   bbu_printf("    dvc10 or 10  --  DVC2 = 1 and DVC1 = 0\n\r");
   bbu_printf("    dvc11 or 11  --  DVC2 = 1 and DVC1 = 1\n\r"); 
   bbu_printf("    sleep  --  Setting used in SLEEP state\n\r");
   bbu_printf("    audio  --  Setting used in AUDIO_MODE_EN field\n\r");
   bbu_printf(" Note: 1. If just <v_buck> entered, its voltage "
                      "will be displayed\n\r");
   bbu_printf("       2. If no <set_mode> entered, all dvc(00,01,10,11) modes \n\r"
              "          will be set to the same voltage\n\r");
   bbu_printf("       3. Use: 'setv <v_buck> off' to turn "
                      "off specific V_BUCK\n\r");
   bbu_printf("       4. Use: 'setv 1/4 dvc/default' to reset Vbuck1 or Vbuck4 \n\r"
              "          to default value\n\r");
   bbu_printf("       5. Use: 'setv pwm' or 'setv pwm on/off' to display or turn on/off\n\r"
              "          Vbuck3 force PWM mode\n\r");
   return;
#endif
}


void bbu_ppset_help(void){
#ifndef CONFIG_NANO_NOLOG
   bbu_printf(" ppset - change the AP system PP to defined one.\n\r");
   #ifndef CONFIG_NANO
   bbu_printf(" ppset /r or ppset --regdump: dump pmu register.\n\r");
   #endif
   bbu_printf(" ppset /c or ppset --check: check the current PP freq values correct or not V.S. PP definition.\n\r");
   bbu_printf(" ppset pp_mode <pp_count> <pp_arg1 pp_arg2>\n\r");
   bbu_printf(" pp_mode: if it's < PP_NUM, then just change PP to pp_mode\n\r");
   bbu_printf("          if it's > PP_NUM, 100: random test, 200: seq test,\n\r"
              "                            300: back2back test, 400:endless random test\n\r");
   bbu_printf(" pp_count: if pp_mode =100/200/300, the test count\n\r");
   bbu_printf(" pp_arg: if pp_mode=300, pp_arg1 and pp_arg2 for B2B test\n\r");
   return;
#endif
}

void bbu_timer_help(void){
#ifndef CONFIG_NANO_NOLOG
   bbu_printf("\n\rdelay - Command to test timers.");
   bbu_printf("\r\nUsage: delay <interval>(s)\r\n");
#endif

   return;
}

int check_help(uint8_t *help_argu){
   if((strcmp((const char*)help_argu, "/h") == 0) || 
      (strcmp((const char*)help_argu, "--help") == 0))
      return 1;
   return 0;
};
