#include "predefines.h"
#include "utils.h"
#include <string.h>
#include "mem_tests.h"
#include "timer_dec.h"
#include "set_mmu.h"

void bbu_peek_32(uint32_t addr, int num_of_reads)
{
   uint32_t data;
   int i;
   if(0 == num_of_reads)
   {
      data = *(volatile uint32_t *) addr;
      bbu_printf("0x%08X: 0x%08X\r\n", addr, data);
   }
   else
   {
      bbu_printf("0x%08X: ", addr);
      for(i = 0; i < num_of_reads; i++)
      {
         data = *(volatile uint32_t *) addr;
         addr += 4;
         bbu_printf("0x%08X ", data);
         if((num_of_reads%4 != 0)?(((i+1)%4) == 0):((((i+1)%4) == 0)&&((i+1)!=num_of_reads)))
            bbu_printf("\n\r0x%08X: ", addr);
      }
      bbu_printf("\n\r");
   }
}

void bbu_poke_32(uint32_t addr, uint32_t data)
{
    *(volatile uint32_t *) addr = data;
    bbu_printf("poke--> The value 0x%08X was deposited at"
                          " location 0x%08X\n\r", data, addr);
}

#ifndef CONFIG_SWP_DDR_ONLY
/* Below is for 16-bit operation. */

void bbu_peek_16(uint32_t addr, int num_of_reads)
{
   uint16_t data;
   int i;
   if(0 == num_of_reads)
   {
      data = *(volatile uint16_t *) addr;
      bbu_printf("0x%08X: 0x%04X\r\n", addr, data);
   }
   else
   {
      bbu_printf("0x%08X: ", addr);
      for(i = 0; i < num_of_reads; i++)
      {
         data = *(volatile uint16_t *) addr;
         addr += 2;
         bbu_printf("0x%04X ", data);
         if((num_of_reads%4 != 0)?(((i+1)%4) == 0):((((i+1)%4) == 0)&&((i+1)!=num_of_reads)))
            bbu_printf("\n\r0x%08X: ", addr);
      }
      bbu_printf("\r\n");
   }
}



void bbu_poke_16(uint32_t addr, uint16_t data)
{
   *(volatile uint16_t *) addr = data;
   bbu_printf("poke--> The value 0x%02X was deposited at byte"
                             " location 0x%08X\n\r", data, addr);
}

/* Below is for 8-bit operation. */

void bbu_peek_8(uint32_t addr, int num_of_reads)
{
   uint8_t data;
   int i;

   if(0 == num_of_reads)
   {
      data = *(volatile uint8_t *)addr;
      bbu_printf("0x%08X: 0x%02X\r\n", addr, data);
   }
   else
   {
      bbu_printf("0x%08X: ", addr);
      for(i = 0; i < num_of_reads; i++)
      {
         data = *(volatile uint8_t *) addr;
         addr += 1;
         bbu_printf("0x%02X ", data);
         if((num_of_reads%4 != 0)?(((i+1)%4) == 0):((((i+1)%4) == 0)&&((i+1)!=num_of_reads)))
            bbu_printf("\n\r0x%08X: ", addr);
      }
      bbu_printf("\r\n");
   }
}

void bbu_poke_8(uint32_t addr, uint8_t data)
{
   *(volatile uint8_t *) addr = data;
   bbu_printf("poke--> The value 0x%04X was deposited at word"
                             " location 0x%08X\n\r", data, addr);
}
#endif

#ifndef CONFIG_NANO
void  bfill ( char *s, int cnt, char ch)
{
   int i;
   
   for ( i=0; i<cnt; i++ )
      *s++ = ch;
}

static void printf_byte(uint32_t data){
	bbu_printf("%02X ", data&0xFF);
	bbu_printf("%02X ", (data>>8)&0xFF);
	bbu_printf("%02X ", (data>>16)&0xFF);
	bbu_printf("%02X ", (data>>24)&0xFF);
}

/**************************************************************************
*
* dl - display memory - 32 bits wide
*
* Display contents of memory, starting at adrs.  Memory is displayed in
* words.  The number of words displayed defaults to 64.  If
* nwords is non-zero, that number of words is printed, rounded up to
* the nearest number of full lines.  That number then becomes the default.
*/


uint32_t    *last_32adrs = (uint32_t *)0;

void mem_dump (
    uint32_t *adrs,    /* address to display */
    int nbytes,        /* number of words to print. */
    int isbytePrint    /* if force to be in byte format */
    )              /* If 0, print 64 or last specified. */
{
   char    ascii [17];
   int    nwords;
   int    byte;
   uint32_t    mask=3;
   uint32_t   data;
   uint8_t   b;
   uint8_t   i;

   ascii [16] = '\0';      /* put an EOS on the string */

   nwords = nbytes/4;

   if (nwords == 0)
      nwords = 32;   /* no count specified: use current byte count */

   if (adrs == (uint32_t *)0xFFFFFFFF)
     adrs = last_32adrs;   /* no address specified: use last address */
   if (adrs == (uint32_t *)0xFFFFFFFF){
      bbu_printf("dump--> Last address doesn't exist\n\r");
      return;
   }

   adrs = (uint32_t *) ((uint32_t) adrs & ~mask);   /* round adrs down to word boundary */

    /* print leading spaces on first line */

   bfill ((char *) ascii, 16, '.');

   bbu_printf("%08X:  ", (uint32_t) adrs & ~0xf);

   for (byte = 0; byte < (int)((uint32_t) adrs & 0xf); byte++)
   {
      bbu_printf("  ");
      if ( (byte & mask) == mask )
         bbu_printf(" ");
      if (byte == 7)
         bbu_printf(" ");   /* extra space between words 3 and 4 */

      ascii [byte] = ' ';
   }

   /* print out all the words */

   while (nwords-- > 0)
   {
      if (byte == 16)
      {
         /* end of line:
          *   print out ascii format values and address of next line */
         if(!isbytePrint)
            bbu_printf("  *%16s*\n\r%08X:  ", ascii, (uint32_t) adrs);
         else
            bbu_printf("\n\r%08X:  ",(uint32_t) adrs);

         bfill ((char *) ascii, 16, '.');   /* clear out ascii buffer */
         byte = 0;            /* reset word count */
      }

      data = (uint32_t) *adrs;
      if(isbytePrint)
         printf_byte(data);
      else
         bbu_printf("%08X ", data);

      if (  byte == 4  )
         bbu_printf(" ");   /* extra space between words 3 and 4 */

      for (i = 0; i < 4; i++)
      {
         b = (uint8_t) (data >> ((3-i) * 8));
         if ( b >= 0x20 && b <= 0x7e )
         ascii [byte+i] = b;
      }

      adrs++;
      byte += 4;
   }

   /* print remainder of last line */

   for (; byte < 16; byte++)
   {
      bbu_printf("  ");
      if ( (byte & mask) == mask )
         bbu_printf(" ");   /* space between words */
      if (byte == 7)
         bbu_printf(" ");   /* extra space between words 3 and 4 */
      ascii [byte] = ' ';
   }
   if(!isbytePrint)
      bbu_printf("  *%16s*\n\r", ascii);   /* print out ascii format values */
   else
      bbu_printf("\n\r");
   last_32adrs = adrs;
   return;
}

void bbu_mem_fill(uint32_t *start_address,
                  uint32_t data,
                  int      byte_count)
{
   byte_count &= ~0x3; //4-byte align
   for(int i = 0; i < (byte_count/4); i++)
      *(start_address++) = data;
    return;
}

int bbu_mem_copy(uint32_t *source_address,
                 uint32_t *dest_address,
                 int       byte_count)
{
   byte_count &= ~0x3; //4-byte align
   for(int i = 0; i < (byte_count/4); i++)
      *dest_address ++ = *source_address++;
   for(int i = 0; i < (byte_count/4); i++)
      if(*(--dest_address) != *(--source_address))
         return 0;
   return 1;
}

#define INC_VALUE 0x11111111

int bbu_mem_std_test(uint32_t *start_address, int byte_count){

   uint32_t *address_bk;
   uint32_t *end_address;
   uint32_t addr2;
   uint32_t value = 0;
   int error_count = 0;
   uint32_t timer_count = 0;
   int sec, msec;
   start_address = (uint32_t *)(((uint32_t)start_address + 3) &(~ 3));  //4-byte align
   end_address   = (uint32_t *)((uint32_t)start_address + byte_count);
   address_bk = start_address;
   addr2 = ((uint32_t)start_address) & 0xFFE00000;
   bbu_printf("memt--> Writing bit slamming data. * = 2MB written.\n\r");
   
   /* It's better to use Timer 2(1.0KHZ) */
   timer_count = timer_get_count(0);
   
   /* Write the inital test pattern to memory */
   while(start_address <= end_address){
      value += INC_VALUE;
      *start_address++ = value;
      if((((uint32_t)start_address) & 0xFFE00000) > addr2)
      {
      	addr2 = (((uint32_t)start_address) & 0xFFE00000);
      	bbu_printf("*");
      }
   }
   
   /*  Read data back from memory - report any errors to the user */
   bbu_printf("\n\rmemt--> Reading bit slamming data. * = 2MB of data compared.\n\r");
   start_address = address_bk;
   addr2 = ((uint32_t)start_address) & 0xFFE00000;
   value = 0;
   while(start_address <= end_address){
      value += INC_VALUE;
      if((((uint32_t)start_address) & 0xFFE00000) > addr2)
      {
      	addr2 = (((uint32_t)start_address) & 0xFFE00000);
      	bbu_printf("*");
      }
      if(*start_address != value)
      {
         if(error_count == 0){
            bbu_printf("\n\rmemt--> Memory Error(s) detected!\n\r");
            bbu_printf("      ADDRESS    EXPECTED     ACTUAL   ERROR BIT(S)\n\r");
            bbu_printf("    ==========  ==========  ==========  ==========\n\r");
         }
         bbu_printf("    0x%08X  0x%08X  0x%08X  0x%08X\n\r",(uint32_t)start_address, 
                    value, *start_address, (*start_address)^value);
         error_count ++;
         if(error_count == 60){
            bbu_printf("memt--> Error count = 60. Test terminated by BBU.\n\r");
            return SCREEN_TEST_FIAL;
         }
      }
      start_address ++;
   }
   
   /* Write bit smashing complement test data to memory. This data is the
      complement of the data used in the first test loop above. */
   if(error_count > 0)
      bbu_printf("\n");
   bbu_printf("\n\rmemt--> Writing bit slamming complement data. * = 2MB written.\n\r");
   
   start_address = address_bk;
   addr2 = ((uint32_t)start_address) & 0xFFE00000;
   value = 0;   
   while(start_address <= end_address){
      value += INC_VALUE;
      *start_address++ = (~value);
      if((((uint32_t)start_address) & 0xFFE00000) > addr2)
      {
      	addr2 = (((uint32_t)start_address) & 0xFFE00000);
      	bbu_printf("*");
      }
   }
   
   /* Complement test data written - read the data back and check for errors
      r3 still contains 0x11111111 from previous testing */
   bbu_printf("\n\rmemt--> Reading bit slamming complement data. * = 2MB of data compared.\n\r");
   start_address = address_bk;
   addr2 = ((uint32_t)start_address) & 0xFFE00000;
   value = 0;
   error_count = 0;
   while(start_address <= end_address){
      value += INC_VALUE;
      if((((uint32_t)start_address) & 0xFFE00000) > addr2)
      {
      	addr2 = (((uint32_t)start_address) & 0xFFE00000);
      	bbu_printf("*");
      }
      if(*start_address != (~value))
      {
         if(error_count == 0){
            bbu_printf("\n\rmemt--> Memory Error(s) detected!\n\r");
            bbu_printf("      ADDRESS    EXPECTED     ACTUAL   ERROR BIT(S)\n\r");
            bbu_printf("    ==========  ==========  ==========  ==========\n\r");
         }
         bbu_printf("    0x%08X  0x%08X  0x%08X  0x%08X\n\r",(uint32_t)start_address, 
                    ~value, *start_address, (*start_address)^(~value));
         error_count ++;
         if(error_count == 60){
            bbu_printf("memt--> Error count = 60. Test terminated by BBU.\n\r");
            return SCREEN_TEST_FIAL;
         }
      }
      start_address ++;
   }
   timer_count = timer_get_count(0) - timer_count;
   msec = count_2_msec(timer_count);
   sec = msec / 1000;
   msec = msec % 1000;
   bbu_printf("\n\rmemt--> Elapsed test time = %d.%d seconds \n\r", sec, msec);
   return SCREEN_TEST_PASS;
}
#endif

/* ........................................................................ */
void bbu_peek(int argc, uint8_t **argv)
{
   uint32_t addr;
   int length;
   bbu_printf("\n\r");
   switch(argc){
      case 1:{
         bbu_printf("peek--> No address specified.\n\r");
         break;
      }
      case 2:{
         if(check_help(argv[1])){
#ifndef CONFIG_NANO
            bbu_peek_help();
#endif
            break;
         }
         addr = conv_hex((char*)argv[1]);
         bbu_peek_32(addr, 0);
         break;
      }
      case 3:{
         addr = conv_hex((char*)argv[2]);
#ifndef CONFIG_SWP_DDR_ONLY
         if(strcmp((const char*)argv[1], "/b") == 0){
            bbu_peek_8(addr, 0);
         }
         else if(strcmp((const char*)argv[1], "/w") == 0){
            bbu_peek_16(addr, 0);
         }
         else
#endif
         {
            length = conv_dec((char *)argv[2]);
            addr = conv_hex((char*)argv[1]);
            bbu_peek_32(addr, length);
         }
         break;
      }
      case 4:{
         addr = conv_hex((char*)argv[2]);
         length = conv_dec((char *)argv[3]);
#ifndef CONFIG_SWP_DDR_ONLY
         if(strcmp((const char*)argv[1], "/b") == 0)
            bbu_peek_8(addr, length);
         else if(strcmp((const char*)argv[1], "/w") == 0)
            bbu_peek_16(addr, length);
         else
            bbu_printf("peek--> The option is not correct\n\r");
#endif        
         break;
      }
      default:
         break;
   }
   return;
}

/* ........................................................................ */
void bbu_poke(int argc, uint8_t **argv)
{
   uint32_t addr, data;
   bbu_printf("\n\r");
   switch(argc){
      case 1:
      case 2:{
         if(check_help(argv[1])){
#ifndef CONFIG_NANO
            bbu_poke_help();
#endif
            break;
         }
         bbu_printf("poke--> Command format is poke [/x=]ADDRESS,DATA"
                 " (2 arguments required)\n\r");
         break;
      }
      case 3:{
         addr = conv_hex((char*)argv[1]);
         data = conv_hex((char*)argv[2]);
         bbu_poke_32(addr, data);
         break;
      }
      case 4:{
         addr = conv_hex((char*)argv[2]);
         data = conv_hex((char*)argv[3]);
#ifndef CONFIG_SWP_DDR_ONLY       
         if(strcmp((const char*)argv[1], "/b") == 0){
            bbu_poke_8(addr, data);
         }
         else if(strcmp((const char*)argv[1], "/w") == 0){
            bbu_poke_16(addr, data);
         }
         else
         {
            bbu_printf("poke--> The option is not correct\n\r");
         }
#endif
         break;
      }
      default:
         break;
   }
   return;
}
#ifndef CONFIG_NANO
/* ........................................................................ */
void bbu_dump(int argc, uint8_t **argv)
{
   uint32_t addr;
   int length;
   bbu_printf("\n\r");
   switch(argc){
      case 1:{
         mem_dump((uint32_t *)0xFFFFFFFF, 0, 0);
         break;
      }
      case 2:{
         if(check_help(argv[1])){
            bbu_dump_help();
            break;
         }
         if(strcmp((const char*)argv[1], "/b") == 0){    
            mem_dump((uint32_t *)0xFFFFFFFF, 0, 1);
         }
         else if(strcmp((const char*)argv[1], "last") == 0){    
            mem_dump((uint32_t *)0xFFFFFFFF, 0, 0);
         }
         else{
            addr = conv_hex((char*)argv[1]);
            mem_dump((uint32_t *)addr, 0 ,0);
         }
         break;
      }
      case 3:{
         if(strcmp((const char*)argv[1], "/b") == 0){
            if(strcmp((const char*)argv[2], "last") == 0)
               mem_dump((uint32_t *)0xFFFFFFFF, 0, 1);
            else{
               addr = conv_hex((char*)argv[2]);
               mem_dump((uint32_t *)addr, 0, 1);
            }
         }
         else{
            length = XpStrToValue((char *)argv[2]);
            if(strcmp((const char*)argv[1], "last") == 0)
               mem_dump((uint32_t *)0xFFFFFFFF, length, 0);
            else{
               addr = conv_hex((char*)argv[1]);
               mem_dump((uint32_t *)addr, length, 0);
            }
         }
         break;
      }
      case 4:{
         addr = conv_hex((char*)argv[2]);
         length = XpStrToValue((char *)argv[3]);
         if(strcmp((const char*)argv[1], "/b") == 0){
            if(strcmp((const char*)argv[2], "last") == 0)
               mem_dump((uint32_t *)0xFFFFFFFF, length, 1);
            else
               mem_dump((uint32_t *)addr, length, 1);
         }
         else
            bbu_printf("dump--> The option is not correct\n\r");
         break;
      }
   }
   return;
}


/* ........................................................................ */
void bbu_fmem(int argc, uint8_t **argv)
{
   uint32_t start_addr;
   uint32_t data;
   int byte_count;
   switch(argc){
      case 2:
         if(check_help(argv[1])){
            bbu_fmem_help();
            break;
         }
      case 3:
      case 1:{
         bbu_printf("fmem--> Enter Starting address, Data to be "
                    "written, byte count on command line.\n\r");
         break;
      }
      case 4:{               
         start_addr = conv_hex((char*)argv[1]);
         data = conv_hex((char*)argv[2]);
         byte_count = XpStrToValue((char*)argv[3]);
         if(INVALID_VIRTUAL_ADDRESS == get_physical(start_addr)){
            bbu_printf("fmem--> There is no physical address "
                       "defined for that starting address.\n\r");
         }
         else if(byte_count < 4)
            bbu_printf("fmem--> Fill length must be greater "
                       "than 3 bytes!\n\r");
         else{
            bbu_mem_fill((uint32_t *)start_addr, data, byte_count);
            bbu_printf("fmem--> Requested area of memory was written.\n\r");
         }
         break;
      }
      default:{
         bbu_printf("fmem--> Too many arguments\n\r");
         break;
      }
   }
}

/* ........................................................................ */
void bbu_memcopy(int argc, uint8_t **argv)
{
   uint32_t source_addr;
   uint32_t dest_addr;
   int byte_count;
   switch(argc){
      case 2: 
         if(check_help(argv[1])){
            bbu_mem_copy_help();
            break;
         }
      case 1:
      case 3:
         bbu_printf("mem_copy--> Wrong number of parameters "
                    "entered. See mem_copy /h\n\r");
         break;
      case 4:
         source_addr = conv_hex((char*)argv[1]);
         dest_addr = conv_hex((char*)argv[2]);
         byte_count = XpStrToValue((char*)argv[3]);
         if(INVALID_VIRTUAL_ADDRESS == get_physical(source_addr))
            bbu_printf("mem_copy--> Source address is not mapped "
                       "to a valid physical address.\n\r");
         else if(INVALID_VIRTUAL_ADDRESS == get_physical(dest_addr))
            bbu_printf("mem_copy--> Destination address is not mapped"
                       " to a valid physical address.\n\r");
         else if(byte_count < 4)
            bbu_printf("mem_copy--> Copy length must be greater "
                       "than 3 bytes!\n\r");
         else{
            if(bbu_mem_copy((uint32_t *)source_addr, \
            	              (uint32_t *)dest_addr, byte_count))
               bbu_printf("mem_copy--> Copy is complete and verified.\n\r");
            else
               bbu_printf("mem_copy--> Error in copy verification at "
                          "source address address\n\r");
         }
         break;
      default:
         bbu_printf("mem_copy--> Too many arguments\n\r");
         break;
   }
   return;
}

/* ........................................................................ */
int bbu_memt(int argc, uint8_t **argv)
{
   uint32_t start_address;
   int byte_count;
   int ret = SCREEN_TEST_FIAL;
   switch(argc){
      case 2:
         if(check_help(argv[1])){
            bbu_memt_help();
            ret = SCREEN_TEST_PASS;
            break;
         }
      case 1:{
         bbu_printf("memt--> Requires two parameters: Starting_address, "
                    "Number_of_bytes.\n\r");
         ret = SCREEN_TEST_FIAL;
         break;
      }
      case 3:{
         byte_count = conv_hex((char*)argv[2]);
         if(!strcmp((char *)argv[1], "dynamic") || 
            !strcmp((char *)argv[1], "d")){
            start_address = (uint32_t)bbu_vm_malloc(byte_count, CACHEABLE);
            if(!start_address){
               bbu_printf("memt-->  Can't allocate desired size test buffer\n\r");
               return SCREEN_TEST_FIAL;
            }
         }else{
            start_address = conv_hex((char *)argv[1]);
            if( !is_test_buffer_safe((uint32_t)start_address)){
               return SCREEN_TEST_FIAL;
            }
         }
         if(INVALID_VIRTUAL_ADDRESS == get_physical(start_address)){
            bbu_printf("memt--> There is no physical address "
                       "defined for that starting address.\n\r");
            ret = SCREEN_TEST_FIAL;
         }
         else{
            bbu_printf("\n\r  Standard BBU memory test from 0x%08X to 0x%08X\n\r", 
                       start_address, start_address+byte_count);
            ret = bbu_mem_std_test((uint32_t *)start_address, byte_count);
            if(is_mem_allocated(start_address))
               bbu_vm_free((void *)start_address);
         }
         break;
      }
      default:{
         bbu_printf("memt--> Too many arguments\n\r");
         ret = 3;
         break;
      }
   }
   return ret;
}

/* ........................................................................ */
void bbu_vtpa(int argc, uint8_t **argv)
{
   uint32_t addr, data;
   if(argc == 1){
      bbu_printf("vtpa--> Virtual address required.\n\r");
   }
   else{
      addr = conv_hex((char*)argv[1]);
      data = get_physical(addr);
      if(data == MMU_OFF)
         bbu_printf("vtpa--> The MMU is not enabled. "
                    "All addresses are physical.\n\r");
      else if(data == INVALID_VIRTUAL_ADDRESS)
         bbu_printf("vtpa--> That virtual address is not "
                    "mapped to a physical address!\n\r");
      else
         page_table_walk(addr);
   }
   return;
}
#endif
