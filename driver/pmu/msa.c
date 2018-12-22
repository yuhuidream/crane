#include "bbu.h"
#include "pmu_dec.h"


static void bbu_dpeek_32(uint32_t addr)
{
   uint32_t data;
   while(cmd2msa_set != 0);
    addr2msa = addr;
    cmd2msa = 0x1;
    cmd2msa_set = 0xfeedbeef;
   while(cmd2msa_set != 0);
   data = value2msa;
   bbu_printf("0x%08X: 0x%08X\r\n", addr, data);
}

static void bbu_dpoke_32(uint32_t addr, uint32_t data)
{
   while(cmd2msa_set != 0);
    addr2msa = addr;
    value2msa = data;
    cmd2msa = 0x2;
    cmd2msa_set = 0xfeedbeef;
   while(cmd2msa_set != 0);
    bbu_printf("poke--> The value 0x%08X was deposited at"
                          " location 0x%08X\n\r", data, addr);
}

static void bbu_dfmem_cmd(uint32_t startaddr, uint32_t endaddr, uint32_t pid)
{
   while(cmd2msa_set != 0);
    startaddr2msa = startaddr & (~0x3);
    endaddr2msa = (endaddr & (~0x3)) - 4;
    pattern2msa = pid;
    cmd2msa = 0x3;
    cmd2msa_set = 0xfeedbeef;
   while(cmd2msa_set != 0);
    bbu_printf("dfmem--> The pattern %d was deposited at memory area [0x%08X,  0x%08X]\n\r", pid, startaddr, endaddr);
}

static void bbu_dvmem_cmd(uint32_t startaddr, uint32_t endaddr, uint32_t pid)
{
   errorcnt2msa = 0xdead; //a non-zero value, msa will update it to actual value.
   while(cmd2msa_set != 0);
    startaddr2msa = startaddr & (~0x3);
    endaddr2msa = (endaddr & (~0x3)) - 4;
    pattern2msa = pid;
    cmd2msa = 0x4;
    cmd2msa_set = 0xfeedbeef;
   while(cmd2msa_set != 0);
   if(errorcnt2msa)
      bbu_printf("dvmem-> There are 0x%x mismatch error locations v.s. pattern.\r\n", errorcnt2msa);
   else
      bbu_printf("dvmem-> All the read back values match the pattern written.\r\n");
}

static void bbu_dmemt_cmd(uint32_t startaddr, uint32_t endaddr, uint32_t iters)
{
   errorcnt2msa = 0xdead; //a non-zero value, msa will update it to actual value.
   while(cmd2msa_set != 0);
    startaddr2msa = startaddr & (~0x3);
    endaddr2msa = (endaddr & (~0x3)) - 4;
    pattern2msa = iters;
    cmd2msa = 0x5;
    cmd2msa_set = 0xfeedbeef;
   while(cmd2msa_set != 0);
   if(errorcnt2msa)
      bbu_printf("dmemt-> There are 0x%x mismatch errors v.s. pattern written at area[0x%08X, 0x%08x]\r\n",
                       errorcnt2msa, startaddr2msa, endaddr2msa);
   else
      bbu_printf("dmemt-> All the read back values match the pattern written.\r\n");
}

static void bbu_drc_32(void)
{
   uint32_t data;
   while(cmd2msa_set != 0);
    cmd2msa = 0x6;
    cmd2msa_set = 0xfeedbeef;
   while(cmd2msa_set != 0);
   bbu_printf("MSA cost %d cyles reading 0x%08X to its L2 SRAM\r\n", latencycount, 0x3000000);
}

static void bbu_dwc_32(void)
{
   uint32_t data;
   while(cmd2msa_set != 0);
    cmd2msa = 0x7;
    cmd2msa_set = 0xfeedbeef;
   while(cmd2msa_set != 0);
   bbu_printf("MSA cost %d cyles writting a value from L2 SRAM to 0x%08X\r\n", latencycount, 0x3000000);
}

void bbu_dpeek(int argc, uint8_t **argv)
{
   uint32_t addr;
   bbu_printf("\n\r");
   switch(argc){
      case 1:{
         bbu_printf("dpeek--> No address specified.\n\r");
         break;
      }
      case 2:{
         if(check_help(argv[1])){
             bbu_printf("usage: dpeek  ADDRESS \r\n");
            break;
         }
         addr = conv_hex((char*)argv[1]);
         bbu_dpeek_32(addr);
         break;
      }
      default:
         break;
   }
   return;
}

/* ........................................................................ */
void bbu_dpoke(int argc, uint8_t **argv)
{
   uint32_t addr, data;
   bbu_printf("\n\r");
   switch(argc){
      case 1:
      case 2:{
         if(check_help(argv[1])){
            bbu_printf("usage: dpoke  ADDRESS DATA \r\n");
            break;
         }
         bbu_printf("dpoke--> Command format is dpoke ADDRESS, DATA"
                 " (2 arguments required)\n\r");
         break;
      }
      case 3:{
         addr = conv_hex((char*)argv[1]);
         data = conv_hex((char*)argv[2]);
         bbu_dpoke_32(addr, data);
         break;
      }
      default:
         break;
   }
   return;
}

void bbu_dfmem(int argc, uint8_t **argv)
{
   uint32_t startaddr, endaddr, pid;
   bbu_printf("\n\r");
   switch(argc){
      case 1:
      case 2:
      case 3:{
         if(check_help(argv[1])){
            bbu_printf("usage: dfmem  start_addr   end_addr   pattern_id\r\n");
            bbu_printf("pattern_id: the pattern to write will be choosed based on the pattern_id\r\n");
            bbu_printf("    1: 0x5A5A5A5A; 2: 0xA5A5A5A5; 3: pattern = address; others: pattern = pattern_id\r\n");
            break;
         }
            bbu_printf("usage: dfmem  start_addr   end_addr   pattern_id\r\n");
         break;
      }
      case 4:{
         startaddr = conv_hex((char*)argv[1]);
         endaddr = conv_hex((char*)argv[2]);
         pid =  conv_hex((char*)argv[3]);
         bbu_dfmem_cmd(startaddr, endaddr, pid);
         break;
      }
      default:
         break;
   }
   return;
}

void bbu_dvmem(int argc, uint8_t **argv)
{
   uint32_t startaddr, endaddr, pid;
   bbu_printf("\n\r");
   switch(argc){
      case 1:
      case 2:
      case 3:{
         if(check_help(argv[1])){
            bbu_printf("usage: dvmem  start_addr   end_addr   pattern_id\r\n");
            bbu_printf("Note: this command should be used after dfmem command.\r\n");
            bbu_printf("pattern_id: the pattern to verify will be choosed based on the pattern_id\r\n");
            bbu_printf("    1: 0x5A5A5A5A; 2: 0xA5A5A5A5; 3: pattern = address; others: pattern = pattern_id\r\n");
            break;
         }
            bbu_printf("usage: dfmem  start_addr   end_addr   pattern_id\r\n");
            bbu_printf("Note: this command should be used after dfmem command.\r\n");
         break;
      }
      case 4:{
         startaddr = conv_hex((char*)argv[1]);
         endaddr = conv_hex((char*)argv[2]);
         pid =  conv_hex((char*)argv[3]);
         bbu_dvmem_cmd(startaddr, endaddr, pid);
         break;
      }
      default:
         break;
   }
   return;
}

void bbu_dmemt(int argc, uint8_t **argv)
{
   uint32_t startaddr, endaddr, iters;
   bbu_printf("\n\r");
   switch(argc){
      case 1:
      case 2:
      case 3:{
         if(check_help(argv[1])){
            bbu_printf("usage: dmemt  start_addr   end_addr   iterations\r\n");
            bbu_printf("iterations: the pattern to write will be switched back and forth between 0xA5A5A5A5 and 0x5A5A5A5A at different iteration\r\n");
            bbu_printf("suggest to let iterations >=2 \r\n");
            break;
         }
            bbu_printf("usage: dmemt  start_addr   end_addr   iterations\r\n");
         break;
      }
      case 4:{
         startaddr = conv_hex((char*)argv[1]);
         endaddr = conv_hex((char*)argv[2]);
         iters =  conv_hex((char*)argv[3]);
         bbu_dmemt_cmd(startaddr, endaddr, iters);
         break;
      }
      default:
         break;
   }
   return;
}

void bbu_drc(int argc, uint8_t **argv)
{
   uint32_t addr;
   bbu_printf("\n\r");
   switch(argc){
      case 1:{
         bbu_drc_32();
         break;
      }
      default:
         break;
   }
   return;
}

/* ........................................................................
right now, the write value to ADDRESS is from #define read2dsp (*(volatile unsigned int *) 0xD1E20004)
*/
void bbu_dwc(int argc, uint8_t **argv)
{
   uint32_t addr;
   bbu_printf("\n\r");
   switch(argc){
      case 1:{
         bbu_dwc_32();
         break;
      }
      default:
         break;
   }
   return;
}
