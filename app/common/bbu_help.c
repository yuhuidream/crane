#include "predefines.h"
#include "bbu.h"
#include <string.h>

void bbu_peek_help(void){
   bbu_printf("\n\r peek - Examine the data at the specified address.\n\r");
   bbu_printf(" Usage: peek [/x=] <address> <num>\n\r");
   bbu_printf("  Where x may be used to specify a PEEK at a byte or word address.\n\r");
   bbu_printf("  /b= address specifies a byte (8-bits) of data to be read.\n\r");
   bbu_printf("  /w= address specifies a half-word (16-bits) of data to be read.\n\r");
   bbu_printf("  An address with no switch defaults to 32-bits of data to be read.\n\r");
   bbu_printf("  <address>   -- The address to be examined.\n\r");
   bbu_printf("  <num>       -- The number of data to be examined.\n\r");
   return;
}

void bbu_poke_help(void){
   bbu_printf("\n\r poke - Deposits byte, half word, or word data into the specified address.\n\r");
   bbu_printf(" Usage: poke [/x=] <address> <data>\n\r");
   bbu_printf("  Where x may be used to specify a POKE at a byte or word address.\n\r");
   bbu_printf("  /b= address specifies a byte (8-bits) of data is to be deposited.\n\r");
   bbu_printf("  /w= address specifies a half-word (16-bits) of data is to be deposited.\n\r");
   bbu_printf("  An address with no switch defaults to 32-bits of data to be deposited.\n\r");
   bbu_printf("  <address>   -- The address where the data is to be deposited\n\r");
   bbu_printf("  <data>      -- The data to be deposited at the specified address.\n\r");
   return;
}

void bbu_sbit_help(void)
{
    bbu_printf("\n\r sbit - Set bits of register or memory\n\r");
    bbu_printf(" Usage: sbit <address> <bit> [<bit>] ... \n\r");
    bbu_printf("  <address>  -- The address of register or memory in 32Bit mode\n\r");
    bbu_printf("  <bit>      -- Bit position to be set\n\r");
    bbu_printf("             -- Can support more than one <bit> one time\n\r");
    bbu_printf("  e.g.: 'sbit 0x100000 1 10 13' will set bit 1, 10, 13 of value in 0x100000\n\r");
}

void bbu_cbit_help(void)
{
    bbu_printf("\n\r cbit - Clear bits of register or memory\n\r");
    bbu_printf(" Usage: cbit <address> <bit> [<bit>] ... \n\r");
    bbu_printf("  <address>  -- The address of register or memory in 32Bit mode\n\r");
    bbu_printf("  <bit>      -- Bit position to be cleared\n\r");
    bbu_printf("             -- Can support more than one <bit> one time\n\r");
    bbu_printf("  e.g.: 'cbit 0x100000 1 10 13' will clear bit 1, 10, 13 of value in 0x100000\n\r");
}

void bbu_rbit_help(void)
{
    bbu_printf("\n\r rbit - Check bits of register or memory\n\r");
    bbu_printf(" Usage: rbit <address> [<bit>] [<bit>] ... \n\r");
    bbu_printf("  <address>  -- The address of register or memory in 32Bit mode\n\r");
    bbu_printf("  <bit>      -- Bit position to be checked\n\r");
    bbu_printf("             -- Can support more than one <bit> one time\n\r");
    bbu_printf("  e.g.: 'rbit 0x100000 1 10 13' will check bit 1, 10, 13 of value in 0x100000\n\r");
    bbu_printf(" Note: if there is not <bit>, will just display bitmap of value in <address>\n\r");
}

#ifdef CONFIG_TZ
void bbu_speek_help(void){
   bbu_printf("\n\r speek - Request secure world service to examine the data \n\r"
                  "         at the specified address.\n\r");
   bbu_printf(" Usage: speek [/x=] <address>\n\r");
   bbu_printf("  Where x may be used to specify a PEEK at a byte or word address.\n\r");
   bbu_printf("  /b= address specifies a byte (8-bits) of data to be read.\n\r");
   bbu_printf("  /w= address specifies a half-word (16-bits) of data to be read.\n\r");
   bbu_printf("  An address with no switch defaults to 32-bits of data to be read.\n\r");
   bbu_printf("  <address>   -- The address to be examined.\n\r");
   return;
}

void bbu_spoke_help(void){
   bbu_printf("\n\r spoke - Request secure world service to deposits byte, half word, \n\r"
                  "         or word data into the specified address.\n\r");
   bbu_printf(" Usage: spoke [/x=] <address> <data>\n\r");
   bbu_printf("  Where x may be used to specify a POKE at a byte or word address.\n\r");
   bbu_printf("  /b= address specifies a byte (8-bits) of data is to be deposited.\n\r");
   bbu_printf("  /w= address specifies a half-word (16-bits) of data is to be deposited.\n\r");
   bbu_printf("  An address with no switch defaults to 32-bits of data to be deposited.\n\r");
   bbu_printf("  <address>   -- The address where the data is to be deposited\n\r");
   bbu_printf("  <data>      -- The data to be deposited at the specified address.\n\r");
   return;
}

#endif

void bbu_fmem_help(void){
   bbu_printf("\n\r fmem - Fill selected memory with data\n\r");
   bbu_printf(" Usage: fmem <address> <data> <byte_count>\n\r");
   bbu_printf("  <address>      -- Starting address where data is to be written.\n\r");
   bbu_printf("  <data>         -- Data to be written to the selected memory area.\n\r");
   bbu_printf("  <byte_count>   -- The number of bytes to be written.\n\r");
   bbu_printf(" Note: <byte_count> should be in 4-Byte increments. BBU clears bits 0:1 of this\n\r");
   bbu_printf("       entry before starting the copy.\n\r");
}

void bbu_mem_copy_help(void){
   bbu_printf("\n\r mem_copy -  - Copy data from one memory location to another.\n\r");
   bbu_printf("  The mem_copy command is used copy data from one memory (or register) area\n\r");
   bbu_printf("  to another area in memory.\n\r");
   bbu_printf(" Usage: mem_copy <source_addr> <dest_addr> <byte_count>\n\r");
   bbu_printf("  <source_addr>  -- Starting address to copy data from.\n\r");
   bbu_printf("  <dest_addr>    -- Starting address where data is to be copied to.\n\r");
   bbu_printf("  <byte_count>   -- Number of bytes to be copied.\n\r");
   bbu_printf(" Note: <byte_count> should be in 4-Byte increments. BBU clears bits 0:1 of this\n\r");
   bbu_printf("       entry before starting the copy.\n\r");
}

void bbu_xtc_help(void){
   bbu_printf(" Usage: xtc <xtc_type> <xtc_value>\n\r");
   bbu_printf("  <xtc_type>     -- rtc/wtc type of SOC sram\n\r");
   bbu_printf("                 --gcrf2p: 	gcu rf2pxtc\n\r");
   bbu_printf("                 --gcrf1p: 	gcu rf1pxtc\n\r");
   bbu_printf("                 --vpurf2p: 	vpu rf2pxtc\n\r");
   bbu_printf("                 --vpurf1p: 	vpu rf1pxtc\n\r");
   bbu_printf("                 --vpusrrtc: vpu srrtc\n\r");
   bbu_printf("                 --vpusrwtc: vpu srwtc\n\r");
   bbu_printf("                 --l1xtc: 	l1 cache xtc\n\r");
   bbu_printf("                 --mmuxtc	mmu sram xtc\n\r");
   bbu_printf("                 --brxtc: 	BTB buffer xtc\n\r");
   bbu_printf("                 --hbxtc		history buffer xtc\n\r");
   bbu_printf("                 --scuxtc	scu sram xtc\n\r");
   bbu_printf("                 --l2rtc: 	l2 cache rtc\n\r");
   bbu_printf("                 --l2wtc		l2 cache wtc\n\r");
   bbu_printf("                 --mcxtc		MC fifo sram xtc\n\r");
   bbu_printf("                 **/d		dump xtc regs\n\r");
   bbu_printf("  <xtc_value>    -- range: [0 ~ 3]\n\r");
}

void bbu_memt_help(void){
   bbu_printf("\n\r memt - Standard BBU memory test routine.\n\r");
   bbu_printf("  The MEMT command performs the standard BBU memoy test. This test starts\n\r");
   bbu_printf("  with the pattern 0x1111_11111 and adds this value to each succeding memory\n\r");
   bbu_printf("  location until the specified number of bytes has been written. The code\n\r");
   bbu_printf("  reads the data back then then does the test again using the complement\n\r");
   bbu_printf("  of the previous pattern. Be aware that BBU running in DDR memory and use mmap cmd to\n\r");
   bbu_printf("  get the memory map and use those memory left to do the test.\n\r");
   bbu_printf(" Usage: memt <address> <byte_count>\n\r");
   bbu_printf("  <address>     -- Starting address for the test.\n\r");
   bbu_printf("  <byte_count>  -- Number of bytes(hexadecimal) to be tested.\n\r");
   bbu_printf(" Note: If <address> = dynamic(d), will dynamically allocate a buffer "
              "\n\r       of desired size for test\n\r");
}

void bbu_dump_help(void){
   bbu_printf("\n\r dump - Dump the contents of multiple memory or register locations.\n\r");
   bbu_printf(" Usage: dump [/b=] <address>, <byte_count>\n\r");
   bbu_printf("  /b= address will force the output to be in byte format.\n\r");
   bbu_printf("  <address>      -- The starting address of the dump\n\r");
   bbu_printf("  <byte_count>   -- The number of bytes to be dumped, take dec/hex"
              " according to the prefix.\n\r");
   bbu_printf("  If <address> is 'last', continue dump with last address.\n\r");
   bbu_printf("     e.g. dump last 100\n\r");
   bbu_printf("  If no dword_count entered, it's default to be 32.\n\r");
   return;
}

void bbu_lpmset_help(void){
   bbu_printf(" lpmset - test the core low power modes entry/exit.\n\r");
   bbu_printf(" lpmset mode <sram_off> [<wake_src>] [<value>]\n\r");
   bbu_printf(" mode: 0 or 1-C12, 2-C13, 3-C2x\n\r");
   bbu_printf("   CR5 will vote all CPCR bits, depends on AP to vote system wide states\n\r");
   bbu_printf(" sram_off:  BIT0: L1  BIT1: L2\n\r");
   bbu_printf(" wake_src: 5: timer, 10: watchdog, 11: PMIC, 16: IPC, 17: RIPC, default is 16\n\r");
   bbu_printf(" value: when wake_src is 5 & 10, it's time in msec\n\r");
   bbu_printf("        when wake_src is 17(RIPC), it's RIPC index(0~3)\n\r");
   return;
}

void bbu_ipwk_help(void){
    bbu_printf(" ipwk - use one core to wake up other cores in the mult-core system.\n\r");
    bbu_printf(" ipwk <core_id_2wk> \n\r");
    bbu_printf(" core_id_2wk: 0: core 0, 1: core 1\n\r");
    return;
}

void bbu_ctst_help(void){
   bbu_printf("\n\r ctst - Cache test command for different cases.\n\r");
   bbu_printf(" Usage: ctst [<index>]\n\r");
   bbu_printf(" index = l1d : L1 D-cache test case\n\r");
   bbu_printf("         l1i : L1 I-cache test case\n\r");
   bbu_printf("         l2d : L2 D-cache test case\n\r");
   bbu_printf("         l2i : L2 I-cache test case\n\r");
   bbu_printf("         rl  : Cache read latency test case\n\r");
   bbu_printf("         wl  : Cache write latency test case\n\r");
   bbu_printf(" Note: If index not entered, all the cases will be performed\n\r");
}

void bbu_eica_help(void){
    bbu_printf("\n\r eica - Caches enable command.\n\r");
    bbu_printf(" Usage: eica [<option>]\n\r");
    bbu_printf(" option = i   : L1 I-cache enable\n\r");
    bbu_printf("          d   : L1 D-cache and integrated L2 cache enable\n\r");
    bbu_printf(" Note: If option not entered, all the caches enabled\n\r");
}

void bbu_dica_help(void){
    bbu_printf("\n\r dica - All caches disable command.\n\r");
    bbu_printf(" Usage: No argument needed,\n\r");
}

void bbu_omxt_help(void){
    bbu_printf("\n\r omxt - Command of OpenMAX DL conformance test cases for NEON.\n\r");
    bbu_printf(" Usage: omxt <domain> <index>\n\r");
    bbu_printf("  <domain>: ac -- Audio coding domain test cases\n\r");
    bbu_printf("            ic -- Image coding domain test cases\n\r");
    bbu_printf("            ip -- Image processing domain test cases\n\r");
    bbu_printf("            sp -- Signal processing domain test cases\n\r");
    bbu_printf("            vc -- Video coding domain test cases\n\r");
    bbu_printf("  <index>: 0~27(ac) -- 0~17 : AAC sub-domain, 18~27: MP3 sub-domain\n\r");
    bbu_printf("           0~18(ic) -- 0~18 : JPEG sub-domain)\n\r");
    bbu_printf("           0~40(ip) -- 0~4  : Bitmap manipulation sub-domain\n\r");
    bbu_printf("                       5~14 : Post processing sub-domain\n\r");
    bbu_printf("                       15~40: Color space conversion sub-domain\n\r");
    bbu_printf("           0~38(sp) \n\r");
    bbu_printf("           0~30(vc) -- 0~2  : MPEG-4 common sub-domain\n\r");
    bbu_printf("                       3~14 : MPEG-4 part 2 sub-domain\n\r");
    bbu_printf("                       15~30: MPEG-4 part 10 sub-domain\n\r");
    bbu_printf(" e.g.: omxt ip 24: run the 24th cases of ip domain\n\r");
    bbu_printf(" Note: If no <index> entered, will run all tests of a specific domain\n\r");
    bbu_printf("       If <domain> = all, all the OpenMAX tests will be run\n\r");
}

void bbu_ucbt_help(void){
    bbu_printf("\n\r ucbt - Command of a port of ucbtest to validate VFP.\n\r");
    bbu_printf(" Usage: ucbt <option> [<function>]...\n\r");
    bbu_printf("  <option>: \n\r"
               "     par  -- 'Paranoia' test case by Prof. W. Kahan\n\r");
    bbu_printf("     mul  -- generates multiplication test cases\n\r");
    bbu_printf("     sqr  -- generates sqrt test cases\n\r");
    bbu_printf("     div  -- generates division test cases\n\r");
    bbu_printf("     lib  -- hex test vectors for library functions\n\r");
    bbu_printf("     fpi  -- determines an approximation to a system's approximate value of PI\n\r");
    bbu_printf("     pi   -- attempts to determine a rational approximation A/B close to a\n\r");
    bbu_printf("             system's approximate value of PI used in trigonometric argument reduction, and\n\r");
    bbu_printf("             tests whether the system's approximation is used consistently\n\r");
    bbu_printf("     beef -- testing sin, cos, atan, exp, expm1, log, and log1p\n\r");
    bbu_printf("  <function>: sin, cos, atan, exp, em1, log, l1p\n\r");
    bbu_printf("              only needed when the <option> is beef\n\r");
    bbu_printf("              If <function> = all, all the 7 functions will be tested\n\r");
}

void bbu_vfpt_help(void){
   bbu_printf("\n\r vfpt - Command of a port of TestFloat Release 2a by John R. Hauser\n\r");
   bbu_printf(" Usage: vfpt [<option>...] <function>\n\r"
              "  <option>:  (* is default)\n\r");
   bbu_printf("    -list            --List all testable functions and exit.\n\r"
              "    -level <num>     --Testing level <num> (1 or 2).\n\r");
   bbu_printf(" *  -level 1\n\r");
   bbu_printf("    -errors <num>    --Stop each function test after <num> errors.\n\r"
              " *  -errors 20\n\r");
   bbu_printf("    -errorstop       --Exit after first function with any error.\n\r");
#if 0  //for BBU system, should not run these test cases for ever
   bbu_printf("    -forever         --Test one function repeatedly (implies `-level 2').\n\r");
#endif
   bbu_printf("    -checkNaNs       --Check for bitwise correctness of NaN results.\n\r");
#ifdef FLOATX80
   bbu_printf("    -precision32     --Only test rounding precision equivalent to float32.\n\r");
   bbu_printf("    -precision64     --Only test rounding precision equivalent to float64.\n\r");
   bbu_printf("    -precision80     --Only test maximum rounding precision.\n\r");
#endif
   bbu_printf("    -nearesteven     --Only test rounding to nearest/even.\n\r");
   bbu_printf("    -tozero          --Only test rounding to zero.\n\r");
   bbu_printf("    -down            --Only test rounding down.\n\r");
   bbu_printf("    -up              --Only test rounding up.\n\r");
   bbu_printf("    -tininessbefore  --Underflow tininess detected before rounding.\n\r");
   bbu_printf("    -tininessafter   --Underflow tininess detected after rounding.\n\r");
   bbu_printf("  <function>:\n\r"
              "    int32_to_<float>                 <float>_add   <float>_eq\n\r");
   bbu_printf("    <float>_to_int32                 <float>_sub   <float>_le\n\r"
              "    <float>_to_int32_round_to_zero   <float>_mul   <float>_lt\n\r");
#ifdef BITS64
   bbu_printf("    int64_to_<float>                 <float>_div   <float>_eq_signaling\n\r");
   bbu_printf("    <float>_to_int64                 <float>_rem   <float>_le_quiet\n\r");
   bbu_printf("    <float>_to_int64_round_to_zero                 <float>_lt_quiet\n\r"
              "    <float>_to_<float>\n\r");
   bbu_printf("    <float>_round_to_int\n\r"
              "    <float>_sqrt\n\r");
#else
   bbu_printf("    <float>_to_<float>               <float>_div   <float>_eq_signaling\n\r");
   bbu_printf("    <float>_round_to_int             <float>_rem   <float>_le_quiet\n\r");
   bbu_printf("    <float>_sqrt                                   <float>_lt_quiet\n\r");
#endif
   bbu_printf("    -all1            --All 1-operand functions.\n\r");
   bbu_printf("    -all2            --All 2-operand functions.\n\r");
   bbu_printf("    -all             --All functions.\n\r"
              "  <float>:\n\r");
   bbu_printf("    float32          --Single precision.\n\r"
              "    float64          --Double precision.\n\r");
#ifdef FLOATX80
   bbu_printf("    floatx80         --Extended double precision.\n\r");
#endif
#ifdef FLOAT128
   bbu_printf("    float128         --Quadruple precision.\n\r"BBU_PROMPT);
#endif
}

void bbu_tmark_help(void){
   bbu_printf("\n\r tmark - Command of Marvell Tmark Benchmark (Memory system)\n\r");
   bbu_printf(" Usage: tmark [<option>...]\n\r"
              "  <option>:  (* is default)\n\r");
   bbu_printf("   -cycle         --  Use cycle to mearsure throughput and time, otherwise use second\n\r");
   bbu_printf("   -m <stexcb>    --  Set the test memory section's attribute to <stexcd>\n\r");
   bbu_printf("                  --  <stexcd>: S(Bit[6]), TEX(Bit[5:2]), C(Bit[1]), B(Bit[0])\n\r");
   bbu_printf(" * -m 0x1f        --  S: 0, TEX: 0x7, C: 1, B: 1\n\r");
   bbu_printf("   -b <buf_size>  --  <buf_size>: 2*L2$ size, Kbytes\n\r");
   bbu_printf(" * -b 1024        --  For Helan/Eden, L2$ is integrated 512KB\n\r");
#if 0
   bbu_printf("   -l1            --  Enable L1 I-cache and D-cache\n\r");
   bbu_printf("   -l2            --  Enable L2 cache\n\r");
   bbu_printf("   -bpu           --  Enable program flow prediction\n\r");
   bbu_printf("   -l2p           --  Enable L2 cache prefetch\n\r");
   bbu_printf("   -all           --  Equal to use -l1, -l2, -bpu, and -l2p simultaneously\n\r");
   bbu_printf(" Note: If no caches related options entered, just use current settings\n\r");
#endif
}

void bbu_gpio_help(void)
{
   bbu_printf("\n\r gpio - Set/Clear GPIO direction bit, set output bit level\n\r");
   bbu_printf(" Usage: gpio [/x] <gpio_pin>\n\r");
   bbu_printf("        where x is one of the following options:\n\r");
   bbu_printf("     /c       -- to Clear the selected GPIO bit\n\r");
   bbu_printf("     /i       -- to set GPIO direction to Input\n\r");
   bbu_printf("     /l       -- to display the level of the GPIO pin\n\r");
   bbu_printf("     /o       -- to set GPIO direction to Output\n\r");
   bbu_printf("     /s       -- to Set the selected GPIO bit\n\r");
   bbu_printf("     /d       -- to display the selected GPIO direction(input/output)\n\r");
   bbu_printf("  <gpio_pin>  -- GPIO number to alter the state of.\n\r");
   bbu_printf(" NOTE: This line must be set as a GPIO for this command to work. In MOST\n\r");
   bbu_printf("       cases this means the alternate function must be set to zero first.\n\r");
}

void bbu_pmic_help(void)
{
   bbu_printf("\n\r pmic - test pmic functions\n\r");
   bbu_printf(" Usage: pmic [function] [args...]>\n\r");
   bbu_printf("        function: if NULL print the PMIC version\n\r");
   bbu_printf("                  onkey: test onkey\n\r");
   bbu_printf("                  arg1->onkey idx[0,1], arg2->press time[1~16]\n\r");
}

void bbu_ledt_help(void)
{
   bbu_printf("\n\r ledt - Turn on/off LEDs\n\r");
   bbu_printf(" Usage: ledt <led_color...>\n\r");
   bbu_printf("  led_color   -  red(r) or blue(b) or green(g)\n\r");
   bbu_printf(" NOTE: The led_color can be a list of LED colors, seperated by space\n\r");
   bbu_printf("       If no argument enterd, display the current LEDs' status\n\r");
   bbu_printf(" e.g.: 'ledt r b': Turn on red and blue LEDs\n\r");
   bbu_printf("       'ledt off' can be used to turn off the LEDs\n\r");
}

void bbu_lsen_help(void)
{
   bbu_printf("\n\r lsen - Initialize and test light sensor\n\r");
   bbu_printf(" Usage: lsen <opt>\n\r");
   bbu_printf("        where <opt> is one of the following options:\n\r");
   bbu_printf("      i   -  Initialize Light sensor\n\r");
   bbu_printf("      p   -  Power down Light sensor\n\r");
   bbu_printf("      d   -  Dump data of Light sensor\n\r");
}

void bbu_gsen_help(void)
{
   bbu_printf("\n\r gsen - Initialize and test gravity sensor and e-compass\n\r");
   bbu_printf(" Usage: gsen <opt>\n\r");
   bbu_printf("        where <opt> is one of the following options:\n\r");
   bbu_printf("      i   -  Initialize gravity sensor and e-compass\n\r");
   bbu_printf("      p   -  Power down gravity sensor and e-compass\n\r");
   bbu_printf("      d   -  Dump data of gravity sensor and e-compass\n\r");
}

void bbu_dhry_help(void)
{
   bbu_printf("\n\r dhry - Dhrystone benchmark test command\n\r");
   bbu_printf(" Usage: dhry <loop>\n\r");
}

void bbu_tsen_help(void)
{
   bbu_printf("\n\r tsen - Test thermal sensor in different modes\n\r");
   bbu_printf(" Usage: tsen <mode> [<int_id>] [<threshold>] [<direction>]\n\r");
   bbu_printf("    <mode> is one of the following options:\n\r");
   bbu_printf("      p   -  Test thermal sensor in polling mode\n\r");
   bbu_printf("      i   -  Test thermal sensor interrupt mode\n\r");
   bbu_printf("      r   -  Put thermal sensor into reset state\n\r");
   bbu_printf("      wdt -  Enable watchdog reset\n\r");
   bbu_printf("   <int_id>     - 0, 1, 2, default is 0\n\r");
   bbu_printf("                - Due to Si bug, only interrupt 0 can work now\n\r");
   bbu_printf("   <threshold>  - 80 ~ 120 Celsius, only used when <opt> = i/wdt, default is 110\n\r");
   bbu_printf("   <direction>  - 0(below), 1(above), default is 1\n\r");
   bbu_printf(" Note: For the 'wdt' mode, <int_id> is not needed\n\r");

}

void bbu_ttdm_help(void)
{
   bbu_printf("\n\r ttdm - Temperature-Time dynamic measurement\n\r");
   bbu_printf(" Usage: ttdm [<secs>] [<temp>]\n\r");
   bbu_printf("    <secs>   - interval that temperature and time measured periodically\n\r"
              "               default is 1(seconds), 0 will stop the measurement\n\r");
   bbu_printf("    <temp>   - Threshold temperature, when exceed this, records\n\r"
              "                will be printed out, default is 110(°C),\n\r");
   bbu_printf(" Note: You can use 'ttdm stop' to stop and print the records anytime\n\r");
   bbu_printf("       If need to change <temp>, <secs> must be provided\n\r");
}

void bbu_gyro_help(void)
{
   bbu_printf("\n\r gyro - Initialize and test gyroscope\n\r");
   bbu_printf(" Usage: gyro <opt>\n\r");
   bbu_printf("        where <opt> is one of the following options:\n\r");
   bbu_printf("      i   -  Initialize gyroscope\n\r");
   bbu_printf("      p   -  Power down gyroscope\n\r");
   bbu_printf("      d   -  Dump data of gyroscope\n\r");
}
void bbu_ddrw_help(void)
{
   bbu_printf("\n\r ddrw - Set/display DDR port priority\n\r");
   bbu_printf(" Usage: ddrp <port> [<weight>]\n\r");
   bbu_printf("   port      -  For 0 to 8\n\r");
   bbu_printf("   weight    -  0 ~ 15\n\r");
   bbu_printf(" Note: If no arguments entered, display all ports' weights\n\r");
   bbu_printf("       If just <port> entered, display this port's weight\n\r");
   bbu_printf("       If the first argument is 'all', please follow it with all the 8\n\r");
   bbu_printf("         ports' weights(seperated by space)\n\r");
}
void bbu_ddrp_help(void)
{
   bbu_printf("\n\r ddrp - Set/display DDR port priority\n\r");
   bbu_printf(" Usage: ddrp <port> [<priority>]\n\r");
   bbu_printf("   port      -  For 0 to 8\n\r");
   bbu_printf("   priority  -  1,0: low; 2: high; 3: critical\n\r");
   bbu_printf(" Note: If no arguments entered, display all ports' priorities\n\r");
   bbu_printf("       If just <port> entered, display this port's priority\n\r");
   bbu_printf("       If the first argument is 'all', please follow it with all the 8\n\r");
   bbu_printf("         ports' priorities(seperated by space)\n\r");
}

void bbu_hsi_help(void)
{
   bbu_printf("\n\r hsi - hsi test\n\r");
   bbu_printf(" Usage: hsi <test> [<arg>]\n\r");
   bbu_printf("   test:     - 1, 2\n\r");
}

void bbu_durm_help(void)
{
   bbu_printf("\n\r durm - DDR utilization rate measurement\n\r");
   bbu_printf(" Usage: durm <opt> [<clk_div>]\n\r");
   bbu_printf("   opt       -  start(1) : start the measurement\n\r");
   bbu_printf("             -  stop(0)  : stop the measurement and print result\n\r");
   bbu_printf("   clk_div   -  1~7 : clock div for total cycles, can be set when start\n\r");
   bbu_printf("                      the measurement, default value is 7\n\r");
}

extern void print_all_stress_tests(void);
void bbu_stress_help(void)
{
   bbu_printf("\n\r stress - Start/stop BBU stress test\n\r");
   bbu_printf(" Note: Please set 'ppset' and 'setv' before start stress test\n\r");
   bbu_printf(" Usage: stress [<type>] [<opt>] [<min>]\n\r");  // [<voltage>]
   bbu_printf("  <type> is one of following tests(default is 'ddr'):\n\r");
   bbu_printf("      ");
   print_all_stress_tests();
   bbu_printf("\n\r");
   //bbu_printf("      cpu    -  CPU stress test \n\r");
   bbu_printf("  <opt> is one of the following options:\n\r");
   bbu_printf("      abort   -  Stop the stress test \n\r");
   bbu_printf("      init    -  Initialize the stress test before restart it\n\r");
   bbu_printf("      auto    -  Run auto stress test, vol/freq will be changed periodically\n\r");
   //bbu_printf("      voltage -  Lowest voltage of vbuck1 in auto mode, 1000~1425 mv\n\r");
   bbu_printf("  <min> The time the stress test will last, default value is 3 minutes\n\r");
   bbu_printf("        If no error reported during <min> minutes, the stress test pass\n\r");
   bbu_printf(" e.g.: 1, 'stress', start ddr stress test and it will last for 3 minutes(default)\n\r");
   bbu_printf("       2, 'stress cpu', start cpu stress test and it will last for 3 minutes\n\r");
   bbu_printf("       3, 'stress 2', start ddr stress test and it will last for 2 minutes\n\r");
   bbu_printf("       4, 'stress power 2', start power stress test and it will last for 2 minutes\n\r");
   bbu_printf("       5, 'stress power auto 2', start power stress test in auto mode, vol/freq \n\r"
              "           will be changed every 2 minutes\n\r");
   bbu_printf("       6, 'stress power list', dump POWER stress test\n\r");
   bbu_printf(BBU_PROMPT);
}

#ifndef CONFIG_CPU_AO
void bbu_ivfp_help(void){
   bbu_printf("\n\r ivfp - Enable/disable NEON(VFP)\n\r");
   bbu_printf(" Usage: ivfp [<opt>]\n\r");
   bbu_printf("      where <opt> is one of the following options:\n\r");
   bbu_printf("      on  or 1   -  Enable NEON(VFP) \n\r");
   bbu_printf("      off or 0   -  Disable NEON(VFP)\n\r");
   bbu_printf("      dump or d  -  Dump NEON(VFP) registers\n\r");
   bbu_printf(" Note: if no <opt> entered, display current state of NEON(VFP)\n\r");
}
#else
void bbu_ivfp_help(void){
   bbu_printf("\n\r ivfp - Enable/disable NEON(VFP) of different core\n\r");
   bbu_printf(" Usage: ivfp [<opt>] [<core>]\n\r");
   bbu_printf("    <opt>:\n\r");
   bbu_printf("      on  or 1   -  Enable NEON(VFP) \n\r");
   bbu_printf("      off or 0   -  Disable NEON(VFP)\n\r");
   bbu_printf("      dump or d  -  Dump NEON(VFP) registers\n\r");
   bbu_printf("    <core>: 0 ~ %d\n\r", CORE_NUM);
   bbu_printf(" Note:\n\r"
              "    1. If no <opt> entered, display state of NEON(VFP) of all cores\n\r");
   bbu_printf("    2. If <core> is 'all', <opt> applies to NEON(VFP) of all cores\n\r");
   bbu_printf(BBU_PROMPT);
}
#endif

void bbu_print_help(void){
   bbu_printf("\n\r print - Set the print level\n\r");
   bbu_printf(" Usage: print <level>\n\r");
   bbu_printf("     where <level> is one of the following levels:\n\r");
   bbu_printf("     dbg    -  Set print level as DBG_LEVEL\n\r");
   bbu_printf("     crit   -  Set print level as CRIT_LEVEl\n\r");
   bbu_printf("     info   -  Set print level as INFO_LEVEL\n\r");
   bbu_printf("     off    -  Turn off all the logs of these levels\n\r");
   bbu_printf(" Note: Can also set the print level to an immediate directly\n\r");
   bbu_printf("       Such as 'print 1', <level> can be 0~4\n\r");
}

void bbu_commt_help(void)
{
   bbu_printf("\n\r commt - MSA silent reset test, supporting tests below at reboot.\n\r");
   bbu_printf(" Usage: commt [<mode> <test_index> <test_loops> <pp_index>]\n\r");
   bbu_printf("   <mode> is one of the following options:\n\r");
   bbu_printf("      1/3 - test and put MSA side into C2, modem clock gating enabled.\n\r");
   bbu_printf("         1: modem off with subdomains off \n\r"
              "         0x<H>3: modem on with specified subdomains on/off, different H value gives different modes below,\n\r"
              "             0x03: all subdomains off \n\r"
              "             0x13: TD mode with WB&LTE-PCC&LTE-SCC subdomains power and clock off \n\r"
              "             0x23: WB mode with TD&LTE-PCC&LTE-SCC subdomains power and clock off \n\r"
              "             0x33: LTE R9 mode with TD&WB&LTE-SCC subdomains power and clock off \n\r"
              "             0x43: LTE R10 mode with TD&WB subdomains power and clock off \n\r"
              "             0x53: all subdomains on with all clocks on (autogating is still enabled)\n\r"
              "             0x63: all subdomains on with all clocks off (both CG and autogating enabled)\n\r"
              "             0x73: all subdomains on with all clocks off, even modem source is gated, pure leakage mode\n\r"
              "             0x83: all subdomains on with all clocks on (no gating is enabled)\n\r");
   bbu_printf("      0/2  - test and put MSA side into while(1) loop\n\r");
   bbu_printf("         0: MSA loop in SQU(test_index=1) or L2 (test_index=0) SRAM, polling commands from AP.\r\n"
              "            dpoke, dpeek, dfmem, dvmem, dmemt, drc, dwc. each command has its own help, eg. 'dpoke /h' \r\n");
   bbu_printf("         2: MSA loop in L2 and when AP SYS go to deeper than M2 level, unvote DDR_IDLE\n\r");
   bbu_printf("   <test_index>: for MSA\n\r");
   bbu_printf("         0/1: boot only\n\r"
              "         4: GSM equalizer unit test\n\r");
   bbu_printf("   <test_loops>: optional. set the pass criteria in loops, default is 1\r\n");
   bbu_printf("   <pp_index>: optional. set the MSA PP after this reset, default is 4\r\n");
   bbu_printf("         pp1: 312MHz; pp2: 416MHz; pp3: 624MHz;\r\n");
   bbu_printf(" Note:MSA always start execution from DDR\n\r");
   bbu_printf(" Note:CR5 side accept its own test from console input\n\r");
   bbu_printf(" use ppset and setv to set the PP and voltage before commt test if necessary.\n\r");
}

/*................. BBU sldo test command help information..................*/
void bbu_sldo_help(void){
   bbu_printf("\n\r sldo - Command to set LDO voltage.\n\r");
   bbu_printf(" Usage: sldo <ldo_channel> [<voltage>] [<set_mode>]\n\r");
   bbu_printf("  <ldo_channel>: 1 ~ 14\n\r");
   bbu_printf("  <voltage>:\n\r");
   bbu_printf("       600 ~ 1500 for LDO 12\n\r");
   bbu_printf("      1200 ~ 3300 for LDO 3 ~ 11\n\r");
   bbu_printf("      1700 ~ 3300 for LDO 1, 2, 13\n\r");
   bbu_printf("      1700 ~ 2800 for LDO 14\n\r");
   bbu_printf("  <set_mode>: Only for setting LDO 2\n\r");
   bbu_printf("    dvc00 or 00  --  DVC2 = 0 and DVC1 = 0\n\r");
   bbu_printf("    dvc01 or 01  --  DVC2 = 0 and DVC1 = 1\n\r");
   bbu_printf("    dvc10 or 10  --  DVC2 = 1 and DVC1 = 0\n\r");
   bbu_printf("    dvc11 or 11  --  DVC2 = 1 and DVC1 = 1\n\r");
   bbu_printf("    sleep  --  Setting used in SLEEP state\n\r");
   bbu_printf("    audio  --  Setting used in AUDIO_MODE_EN field\n\r");
   bbu_printf(" Note: 1. If just <ldo_channel> entered, its voltage "
                            "will be displayed\n\r");
   bbu_printf("       2. If no <set_mode> entered, all dvc(00,01,10,11) modes \n\r"
              "          will be set to the same voltage\n\r");
   bbu_printf("       3. Use: 'sldo <ldo_channel> off' to turn "
                            "off specific LDO\n\r");
   bbu_printf("       4. Use: 'sldo 1 dvc/default' to reset LDO1 "
                      "to default value\n\r");
   return;
}

void bbu_curm_help(void)
{
   bbu_printf("\n\r curm - CPU utilization rate measurement\n\r");
   bbu_printf(" Usage: curm <opt> [<div>]\n\r");
   bbu_printf("   <opt>    -  start(1) : start the measurement\n\r");
   bbu_printf("            -  stop(0)  : stop the measurement and print result\n\r");
   bbu_printf("   <div>    -  1 : set cycle div to 64 (default)\n\r");
   bbu_printf("               0 : clear cycle div \n\r");
}

void bbu_ddr_power_help(void)
{
   bbu_printf("\n\r pwddr - Dump and set DDR power state\n\r");
   bbu_printf(" Usage: pwddr <opt> <arg1> <arg2>\n\r");
   bbu_printf("      opt   -  dump: dump power status, not need arg1 and arg2\n\r");
   bbu_printf("            -  mmusage: dump ddr range status, not need arg1 and arg2\n\r");
   bbu_printf("            -  dpmm: dump ddr alloc status, not need arg1 and arg2\n\r");
   bbu_printf("            -  testvm: test bbu_vm_malloc&bbu_vm_free\n\r");
   bbu_printf("            -  set:  set ddr power status\n\r");
   bbu_printf("            -  testp:  power func test\n\r");
   bbu_printf("            -  autor:  auto self-refresh for DDR3[extended mode]\n\r");
   bbu_printf("            -  cpumask: show cpu mask\n\r");
   bbu_printf("            -  cal: pad calibration test\n\r");
   bbu_printf("            -   	   arg1: zpr(<=9)\n\r");
   bbu_printf("            -   	   arg2: znr(<=7)\n\r");
   bbu_printf("      arg1  -  DDR chip number <0,1,2,3>\n\r");
   bbu_printf("      arg2  -  power state < 0: active, 1: self-refresh, 2: active power down\n\r");
   bbu_printf("            -  3: precharge power down, 4: deep power down, should not set to inuse chip >\n\r");
   bbu_printf("            -  ddr3 not support deep power down\n\r");
   bbu_printf("      opt   -  pasr: partial self refresh\n\r");
   bbu_printf("      arg1  -  DDR chip number <0,1,2,3>\n\r");
   bbu_printf("      arg2  -  8 bits bank mask int hex format\n\r");
}

#ifdef QF_STACK_PER_AO
void bbu_stack_help(void){
   bbu_printf("\n\r stack - AO stack overflow and used size check command\n\r");
   bbu_printf(" Usage: stack <opt> <AO>\n\r");
   bbu_printf("   <opt>    -  o : Check if an AO stack overflow or not\n\r");
   bbu_printf("            -  s : Check how many bytes have been used of an AO stack\n\r");
   bbu_printf("            -  i : Print the information of an AO stack\n\r");
   bbu_printf("   <AO>     -  AO name, type 'set_ao' to get the list\n\r");
}
#endif

void bbu_pwm_help(void) {
   bbu_printf("\n\r pwm3 - Test pwm function\n\r");
   bbu_printf(" Usage: pwm3 <arg1> <arg2> <arg3>\n\r");
   bbu_printf("      arg1  -  clk_x <0 : 13M, 1 : 32K>\n\r");
   bbu_printf("      arg2  -  duty in ns\n\r");
   bbu_printf("      arg3  -  period time in ns\n\r");
}

/* ........................................................................ */
void bbu_mstp_help(void)
{
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
}

/*................. BBU setv test command help information..................*/
void bbu_setv_help(void){
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
}

void bbu_ppset_help(void){
   bbu_printf(" ppset - change the core/ddr/axi OP to a specified combination.\n\r");
   bbu_printf(" ppset /r : dump pmu register.\n\r");
   bbu_printf(" ppset /c : check the current PP freq values are correct or not V.S. PP definition.\n\r");
   bbu_printf(" ppset  core_op  ddr_op  axi_op\n\r");
   bbu_printf(" core_op: the targeting OP of  AP core\n\r");
   bbu_printf(" ddr_op: the targeting OP of DDR \n\r");
   bbu_printf(" axi_op: the targeting OP of CP AXI bus\n\r");
   return;
}

void bbu_opset_help(void){
   bbu_printf(" opset - change a component's OP to the specified one.\n\r");
   bbu_printf(" opset /c : check the current OP freq values are correct or not V.S. PP definition.\n\r");
   bbu_printf(" opset  component  op_num  <count> <op1  op2>\n\r");
   bbu_printf(" component: the object to change frequency, can be 'core', 'ddr' , 'axi'  or 'laxi' \n\r");
   bbu_printf("                   In addition, the command  'opset pll  /c'  can be used to dump PLL status\r\n");
   bbu_printf(" For core, ddr, axi and laxi, there are more test cases below\r\n");

   bbu_printf(" op_num: if it's < MAX_OP_NUM, change OP to op_num\n\r");
   bbu_printf("              if it's > MAX_OP_NUM, 100: random test, 200: seq test,\n\r"
                   "                         300: back2back test, 400:endless random test\n\r");
   bbu_printf(" count: if op_num =100/200/300, the test count\n\r");
   bbu_printf(" op1 op2: if op_num=300, op1 and op2 B2B change test\n\r");
   return;
}

void bbu_uart_help(void)
{
    bbu_printf(" uart - Display and change UART owner\n\r");
    bbu_printf(" Usage: uart <owner> \n\r");
    bbu_printf("  <owner>: \n\r");
    bbu_printf("    ap or ca7 - Set CA7 as UART owner\n\r");
    bbu_printf("    cp or cr5 - Set CR5 as UART owner\n\r");
    bbu_printf(" Note: if <owner> is not entered, will display current UART owner\n\r");
    return;
}

void bbu_ripc_help(void)
{
   bbu_printf(" ripc - Resource IPC request/release test\n\r");
   bbu_printf(" Usage: ripc <test> [<index>]\n\r");
   bbu_printf("    <test>: 1 or 0. 1, request RIPC; 0, release RIPC\n\r");
   bbu_printf("    <index>: 0 ~ 3.\n\r");
}

void bbu_ipc_test_help(void){
   bbu_printf("\n\r ipc - Test different IPCs before AP and CP\n\r");
   bbu_printf(" Usage: ipc <type>\n\r");
   bbu_printf("     <type>:  cmd, ack, gp, msg\n\r");
}



int check_help(uint8_t *help_argu){
   if((strcmp((const char*)help_argu, "/h") == 0) ||
      (strcmp((const char*)help_argu, "--help") == 0))
      return 1;
   return 0;
};

void bbu_timer_help(void){
   bbu_printf("\n\rtimer - Command to test timers.");
   bbu_printf("\r\nUsage: timer <command> <index> <unit> <run mode> <interval>\r\n");
   bbu_printf("    <command>: \n\r"
              "      start  -- start timer\n\r");
   bbu_printf("      stop   -- stop timer\n\r");
   bbu_printf("    <index>: timer index\n\r");
   bbu_printf("    <unit>: timer unit\n\r");
   bbu_printf("    <run mode>: \n\r"
              "      0  -- periodic\n\r");
   bbu_printf("      1  -- one shot\n\r");
   bbu_printf("    <interval>: timer interval\n\r");
   bbu_printf("Note: timer start all <run mode > <interval> can start all timers.\r\n"
              "      timer stop all can stop all timers.\r\n");

   return;
}
