#include <stdlib.h>
#include <string.h>
#include "mem_config.h"
#include "tmark.h"

/* Header files needed by BBU related codes */
#include "cache-test.h"
#include "set_mmu.h"
#include "timer_dec.h"
#include "perf_counter.h"
#include "system_mrvl.h"
#include "utils.h"
#include "ppset.h"

extern uint32_t timer_get_count(int id);
#define GetTimerCount timer_get_count

#ifndef TMARK_SRC_ADDRESS
#define TMARK_SRC_ADDRESS  BBU_TMARK_BUFFER_START
#endif


#ifndef TMARK_DEST_ADDRESS
#define TMARK_DEST_ADDRESS (BBU_TMARK_BUFFER_START + BBU_TMARK_BUFFER_LEN/2)
#endif

int latency[6] = {0};
float throughput[3] = {0};
int measure_using_cycle = 0;
int coreFreqMHz = 0;
int sl2_enable = 0;
unsigned int bufferSize = BUFFER_SIZE;
unsigned int L2MissBufSize = BUFFER_SIZE;
struct node* link_buffer;
struct node_64* link_buffer_64;

void flush_all_cache(void){
     l1_icache_invalidate_all();
     dcache_clean_invalidate_all();
}

void dump_info(char *title, void *src_addr, void *dst_addr, int buf_size, int iters, int stride, unsigned long total_time)
{
   float lat, t;
   if(measure_using_cycle)
      /* use cycles for total_time */
      lat = (float)total_time/iters/(buf_size/stride);
   else
      /* use microseonds(us) for total_time */
      lat = (float)total_time*coreFreqMHz/iters/(buf_size/stride);
   if(strstr(title, "Single load")) {
      lat -= 1;
   }
   if(measure_using_cycle)
   {
      /* use cycles for total_time */
      bbu_printf("%s,\t0x%08x, 0x%08x, %10.2f, %10d, %10d, %10lu, %10d, %10.2f\n\r",
         title, src_addr, dst_addr, (float)buf_size/1024, iters, stride,
         total_time,
         (int)lat,
         (float)buf_size*iters/total_time*(1000000000./1024./1024.));
   }
   else
   {
      /* use microseonds(us) for total_time */
      TMARK_Printf("%s,\t0x%08x, 0x%08x, %10.2f, %10d, %10d, %10lu, %10d, %10.2f\n\r",
         title, src_addr, dst_addr, (float)buf_size/1024, iters, stride,
         total_time,
         (int)lat,
         (float)buf_size*iters/1024./1024./total_time*1000000.);
   }

   if(strcmp(title, "Single load #32") == 0) {
             if(buf_size == L1HIT_BUF_SIZE) {
         latency[0] = (int)lat;
             } else if (buf_size == L2HIT_BUF_SIZE) {
         latency[1] = (int)lat;
      }
      /* Change DDR latency calculation from single load #32 to single load #64 */
      /*
      else if (buf_size == L2MISS_BUF_SIZE) {
         index = 2;
             }
      */
   /* DDR latency case, pick up non-32 byte aligned single load #64 case */
   } else if (strcmp(title, "Single load #64") == 0 && ((unsigned int)src_addr - ALIGN_256((unsigned int)src_addr-0x30) == 32)) {
      latency[2] = (int)lat;
   } else if (strcmp(title, "Single load Linked #32") == 0){
      if(buf_size == L1HIT_BUF_SIZE) {
         latency[3] = (int)lat;
      } else if (buf_size == L2HIT_BUF_SIZE) {
         latency[4] = (int)lat;
      }
   } else  if (strcmp(title, "Single load linked #64") == 0 && ((unsigned int)src_addr - ALIGN_256((unsigned int)src_addr-0x30) == 32)) {
      latency[5] = (int)lat;
   } else if (strcmp(title, "Pipelined Load #32") == 0) {
   if(measure_using_cycle)
      throughput[0] = (float)buf_size*iters/total_time*(1000000000./1024./1024.);
   else
      throughput[0] = (float)buf_size*iters/1024./1024./total_time*1000000.;
   } else if (strstr(title, "Coalesced Store") != 0 && buf_size == (int)L2MissBufSize ) {
   if(measure_using_cycle)
      t = (float)buf_size*iters/total_time*(1000000000./1024./1024.);
   else
      t = (float)buf_size*iters/1024./1024./total_time*1000000.;
      if(t > throughput[1]) {
         strcpy(t1, title);
         throughput[1] = t;
      }
   } else if(strstr(title, "Memcpy") != 0 && buf_size ==  (int)L2MissBufSize) {
   if(measure_using_cycle)
      t = (float)buf_size*iters/total_time*(1000000000./1024./1024.);
   else
      t = (float)buf_size*iters/1024./1024./total_time*1000000.;
      if(t > throughput[2]) {
         strcpy(t2, title);
         throughput[2] = t;
      }
   }

}

void dump_analysis()
{
   TMARK_Printf("\r\n\r***************************************************************\r\n\r");
   TMARK_Printf("TMark Result (reference only and pls check above log carefully).\r\n\r");
   TMARK_Printf("***************************************************************\r\n\r");
   TMARK_Printf("Optimistic latency number\r\n\r");
   TMARK_Printf("L1 hit latency (cycle), %10d\r\n\r", latency[0]);
   TMARK_Printf("L2 hit latency (cycle), %10d\r\n\r", latency[1]);
   TMARK_Printf("Memory latency (cycle) (**1), %10d\r\n\r", latency[2]);
   TMARK_Printf("===================================================================\r\n\r");
   TMARK_Printf("True dependancy (Every load is dependant on previous load so latency hiding cannot occur\r\n\r");
   TMARK_Printf("True-depend-L1 hit latency (cycle), %10d\r\n\r", latency[3]);
   TMARK_Printf("True-depend-L2 hit latency (cycle), %10d\r\n\r", latency[4]);
   TMARK_Printf("True-depend-Memory latency (cycle) (**1), %10d\r\n\r", latency[5]);
if(measure_using_cycle){
   TMARK_Printf("Load throughput (MB/giga cycle), %10.2f (%s)\r\n\r", throughput[0], "*coreFreqGHz => MB/s");
   TMARK_Printf("Store throughput (MB/giga cycle),%10.2f in %s\r\n\r", throughput[1], t1);
   TMARK_Printf("Memcpy throughput(MB/giga cycle),%10.2f in %s\r\n\r", throughput[2], t2);
}
else {
   TMARK_Printf("Load throughput (MB/s), %10.2f\r\n\r", throughput[0]);
   TMARK_Printf("Store throughput (MB/s),%10.2f in %s\r\n\r", throughput[1], t1);
   TMARK_Printf("Memcpy throughput(MB/s),%10.2f in %s\r\n\r", throughput[2], t2);
}
#ifdef CONFIG_CACHE_TAUROS3_PREFETCH_ENABLE
   TMARK_Printf("(**1)\n\r");
   TMARK_Printf("\tFor PJ4B SL2, when prefetch offset is odd, cache will always hit after one\n\r");
   TMARK_Printf("\tpoint because of the design of access pattern, so you will see the prefetch\n\r");
   TMARK_Printf("\thiding the memory latency. When prefetch offset is even, cache will always\n\r");
   TMARK_Printf("\tmiss because of the design of access pattern, so you won't see the prefetch\n\r");
   TMARK_Printf("\teffect from this test but it is functional. Please try to set offset to odd\n\r");
   TMARK_Printf("\tto check it again.\n\r");
#endif

}

int TMark_main(int argc, char **argv)
{
   char *src_ddr, *dst_ddr, *src_ddr_a[4], *dst_ddr_a[4];
   void *src_ddr_malloc, *dst_ddr_malloc;
   int  count, step, val;
   unsigned int i,j;
   unsigned int Begin_Time, End_Time;
   (void)dst_ddr_a;
#ifdef ENABLE_LPAE
   TMARK_Printf("LPAE enable.\n\r");
#endif

   /* reset global arguments */
   memset(latency, 0, sizeof(latency));
   memset(throughput, 0, sizeof(throughput));
   measure_using_cycle = 0;
   coreFreqMHz = 0;
   bufferSize = BUFFER_SIZE;
   L2MissBufSize = BUFFER_SIZE;
   sl2_enable = 0;

   bbu_printf("\n\r");

   uint32_t stexcb = MEMATTR_STEXCB;

#if 0
   uint32_t cache_enable = 0;
#endif

   /* Analyze the arguments */
   for(int i = 1; i < argc; i++)
   {
      if(check_help(argv[i])){
         bbu_tmark_help();
         return;
      }
      else if(!strcmp((const char*)argv[i], "-m")){
         stexcb = strtoul((const char *)argv[i+1], 0, 16);
         i++;
      }
      else if(!strcmp((const char*)argv[i], "-b")){
         bufferSize = atoi((const char *)argv[i+1])*1024;
         L2MissBufSize = bufferSize;
         i++;
      }
#if 0
      else if(!strcmp((const char*)argv[i], "-l1"))
         cache_enable |= (I_CACHE_ENABLE | D_CACHE_ENABLE);
      else if(!strcmp((const char*)argv[i], "-l2"))
         cache_enable |= L2_CACHE_ENABLE;
      else if(!strcmp((const char*)argv[i], "-bpu"))
         cache_enable |= BPU_ENABLE;
      else if(!strcmp((const char*)argv[i], "-l2p"))
         cache_enable |= L2_CACHE_PREFETCH_ENABLE;
      else if(!strcmp((const char*)argv[i], "-all"))
         cache_enable = 0xff;
#endif
      else if(!strcmp((const char*)argv[i], "-cycle"))
         measure_using_cycle = 1;
   }

//   if(OP_INVALID == user_cop[PLL_CORE]){
//      bbu_printf("Can't Get Current PP, measure PCLK directly\n\r");
      coreFreqMHz = measure_core_frequency(0);  /* Get current core frequency */
//   }else{
//      coreFreqMHz = cr5_value[user_cop[PLL_CORE]].pclk;
//   }

   TMARK_Printf("Current CPU frequency is %dMHz\n\r", coreFreqMHz);

   /* Reconfigurate caches and BPU if needed */
#if 0
   if(cache_enable){
      bbu_dica();
      bbu_eica(1, cache_enable);
   }
   else
      bbu_printf("SL2 cache not enabled, ");
#endif

   bbu_printf("bufferSize %d Kbyte\n\r", bufferSize / 1024);
   __asm__ volatile("mrc p15, 0, %0, c1, c0, 0": "=r"(val));
   bbu_printf("System Control: 0x%08x\n\r", val);
   __asm__ volatile("mrc p15, 0, %0, c1, c0, 1": "=r"(val));
   bbu_printf("Auxiliary Control: 0x%08x\n\r", val);

#ifdef BAREMETAL
   if (SMP == 1)
     bbu_printf ("SMP Enabled, ");
#if defined(ENABLE_TCL2)
   bbu_printf("TCL2 enabled, ");
#endif
#if ENABLE_LPAE
   bbu_printf("Memory Attributes: S_TEST[1:0]=0x%X, S_CODE[1:0]=0x%X Attr_TEST=0x%X Attr_CODE=0x%X,\n\r",
             (LPAE_SHR_TEST)&0x3,(LPAE_SHR_CODE)&0x3,(LPAE_ATTR_TEST)&0xFF, (LPAE_ATTR_CODE)&0xFF);
	#ifdef PAGE_TABLE_4KB_ALLOCATE
	   bbu_printf("Data section using 4k page allocate\n\r");
	#else
	   bbu_printf("Data section using 2M page allocate\n\r");
	#endif
#elif defined(ENABLE_TEXREMAP)
   bbu_printf("\n\rTEX remap enable. Memory Attributes: Shareability=%X, ", (TRE_MEMATTR & 0x1000) >> 12);
   if(TRE_MEMATTR & 0x1000) {
      if(TRE_MEMATTR & 0x100)   {
         bbu_printf("Inner Shareable\n\r");
      } else   {
         bbu_printf("Outer Shareable\n\r");
      }
   }
   bbu_printf("Cache attribute: inner cacheable=%d, outer cacheable=%d\n\r", (TRE_MEMATTR & 0x30) >> 4,TRE_MEMATTR & 0x3);
	#ifdef PAGE_TABLE_4KB_ALLOCATE
	   bbu_printf("Data section using 4k page allocate\n\r");
	#else
	   bbu_printf("Data section using 1M page allocate\n\r");
	#endif
#else
   bbu_printf("Memory Attributes: S=%d, TEX=%d, C=%d, B=%d\n\r",
         (stexcb>>5)&1,
         (stexcb>>2)&7,
         (stexcb>>1)&1,
         (stexcb>>0)&1);
#endif

#endif
#ifndef BAREMETAL
   if(coreFreqMHz == 0) {
      /* dummmy code for marvell-div */
      coreFreqMHz = coreFreqMHz / val ;
      bbu_printf("Error Please set core freq by command line (./tmark [core_freq_MHz]) or define CORE_FREQ in Makefile\n\r");
      return -1;
   }
#endif

#ifdef RTL

   #ifndef MEM_SRC
   #error "MEM_SRC is not defined!"
   #endif

   #ifndef MEM_DST
   #error "MEM_DST id not defined!"
   #endif
      src_ddr_malloc = MEM_SRC;
      dst_ddr_malloc = MEM_DST;
#else
   #ifdef BAREMETAL
      /*
       * allocate a dummy 1MB of memory to avoid the usage of first 1MB by
       * dst_ddr_malloc and src_ddr_malloc
       */
   #ifdef ENABLE_LPAE
      /* LPAE Level-2 block is 2MB */
      dummy_malloc = bbu_malloc(0x200000);
   #else
      //dummy_malloc = bbu_malloc(0x100000);
   #endif
   #endif

   dst_ddr_malloc = (char *)bbu_vm_malloc(bufferSize+256, CACHEABLE);
   src_ddr_malloc = (char *)bbu_vm_malloc(bufferSize+256, CACHEABLE);
   uint32_t numPage = ((bufferSize+256+0xFFFFF) & (~0xFFFFF))/0x100000;

   if(dst_ddr_malloc==0 || src_ddr_malloc==0)
      bbu_printf("SRC and DST address error\n\r");
   memset(src_ddr_malloc, 0, bufferSize+256);
   memset(dst_ddr_malloc, 0, bufferSize+256);
#endif

   if((src_ddr_malloc == NULL) || (dst_ddr_malloc == NULL))
   {
      bbu_printf("fail to allocate the memory\n\r");
      return -1;
   }

   src_ddr = (char*)ALIGN_256(src_ddr_malloc);
   dst_ddr = (char*)ALIGN_256(dst_ddr_malloc);
   bbu_printf("SRC:0x%X DST:0x%X\n\r",src_ddr,dst_ddr);
   src_ddr_a[0] = src_ddr;
   src_ddr_a[1] = src_ddr + 0x10;
   src_ddr_a[2] = src_ddr + 0x20;
   src_ddr_a[3] = src_ddr + 0x30;
   dst_ddr_a[0] = dst_ddr;
   dst_ddr_a[1] = dst_ddr + 0x10;
   dst_ddr_a[2] = dst_ddr + 0x20;
   dst_ddr_a[3] = dst_ddr + 0x30;
   dump_banner();
   /* dummp code for marvell-div */
   latency[0] = (int)src_ddr / (int)src_ddr_a[3];
   latency[1] = (int)src_ddr / (int)dst_ddr;
   latency[2] = (int)dst_ddr / (int)src_ddr_a[2];

   for(step =0; step < EVENT_MAXIMUM; step=step+4)
   {
      dump_info_header();

#if     SINGLE_LDR_32
      /* case1: for L1 hit latency */
      /* Size=16K  */
      flush_all_cache();
      count = (MIN(bufferSize, L1HIT_BUF_SIZE))/CACHELINE_SIZE;
#ifdef DRY_RUN
      single_ldr_32(src_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0; i<MAX(LOOP_TIME, 1000) ;i++)
      {
         single_ldr_32(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Single load #32", src_ddr, 0, count*CACHELINE_SIZE, MAX(LOOP_TIME, 1000), CACHELINE_SIZE, End_Time-Begin_Time);


#ifdef BAREMETAL
      /* case1 DSB: for L1 hit latency */
      /* Size=16K  */
      flush_all_cache();
      count = (MIN(bufferSize, L1HIT_BUF_SIZE))/CACHELINE_SIZE;

      Begin_Time = mrvl_times();
      for(i=0; i<MAX(LOOP_TIME, 1000) ;i++)
      {
         single_ldr_32_DSB(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Single load DSB #32", src_ddr, 0, count*CACHELINE_SIZE, MAX(LOOP_TIME, 1000), CACHELINE_SIZE, End_Time-Begin_Time);
#endif /* BAREMETAL */
      /* case1 linked list: for L1 hit latency */
      /* Size=16K  */
      flush_all_cache();
      count = (MIN(bufferSize, L1HIT_BUF_SIZE))/CACHELINE_SIZE;
      link_buffer = (struct node*)(src_ddr);
      for ( i = 0; i < bufferSize / 32; i++)
      {
        link_buffer[i].next = (link *)&link_buffer[i+1];
      }
      Begin_Time = mrvl_times();
      for(i = 0; i < MAX(LOOP_TIME, 1000); i++)
                {
          single_ldr_32_linked(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Single load Linked #32", src_ddr, 0, count*CACHELINE_SIZE, MAX(LOOP_TIME, 1000), CACHELINE_SIZE, End_Time-Begin_Time);

      /* case2: for L2 hit latency */
      /* Size=64K */
      flush_all_cache();
      count = (MIN(bufferSize, L2HIT_BUF_SIZE))/CACHELINE_SIZE;
#ifdef DRY_RUN
      single_ldr_32(src_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<MAX(LOOP_TIME, 1000);i++)
      {
         single_ldr_32(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Single load #32", src_ddr, 0, count*CACHELINE_SIZE, MAX(LOOP_TIME, 1000), CACHELINE_SIZE, End_Time-Begin_Time);
#ifdef BAREMETAL
      /* case2: for L2 hit latency */
      /* Size=64K */
      flush_all_cache();
      count = (MIN(bufferSize, L2HIT_BUF_SIZE))/CACHELINE_SIZE;

      Begin_Time = mrvl_times();
      for(i=0;i<MAX(LOOP_TIME, 1000);i++)
      {
         single_ldr_32_DSB(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Single load DSB #32", src_ddr, 0, count*CACHELINE_SIZE, MAX(LOOP_TIME, 1000), CACHELINE_SIZE, End_Time-Begin_Time);
#endif /* BAREMETAL */

      /* case2 linked list: for L2 hit latency */
      /* Size=64K  */
      flush_all_cache();
      count = (MIN(bufferSize, L2HIT_BUF_SIZE))/CACHELINE_SIZE;
      link_buffer = (struct node*)(src_ddr);
      for (i = 0; i < bufferSize / 32 -1; i++)
      {
        link_buffer[i].next = (link *)&link_buffer[i+1];
      }
      Begin_Time = mrvl_times();
      for(i = 0; i < MAX(LOOP_TIME, 1000); i++)
                {
          single_ldr_32_linked(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Single load Linked #32", src_ddr, 0, count*CACHELINE_SIZE, MAX(LOOP_TIME, 1000), CACHELINE_SIZE, End_Time-Begin_Time);

      /* case 3: DDR latency */
      /* 1MB */
      flush_all_cache();
      count = (MIN(bufferSize, L2MissBufSize))/CACHELINE_SIZE;
#ifdef DRY_RUN
      single_ldr_32(src_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++)
      {
         single_ldr_32(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Single load #32", src_ddr, 0, count*CACHELINE_SIZE, LOOP_TIME, CACHELINE_SIZE, End_Time-Begin_Time);
#ifdef BAREMETAL
      /* case 3: DDR latency */
      /* 1MB */
      flush_all_cache();
      count = (MIN(bufferSize, L2MissBufSize))/CACHELINE_SIZE;

      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++)
      {
         single_ldr_32_DSB(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Single load DSB #32", src_ddr, 0, count*CACHELINE_SIZE, LOOP_TIME, CACHELINE_SIZE, End_Time-Begin_Time);
#endif /* BAREMETAL */
      /* case3 linked list: DDR latency */
      /* Size=L2MissBufSize  */
      flush_all_cache();
      count = (MIN(bufferSize, L2MissBufSize))/CACHELINE_SIZE;
      link_buffer = (struct node*)(src_ddr);
      for (i = 0; i < bufferSize / 32; i++)
      {
        link_buffer[i].next = (link *)&link_buffer[i+1];
      }
      Begin_Time = mrvl_times();
      for(i = 0; i < LOOP_TIME; i++)
                {
          single_ldr_32_linked(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Single load Linked #32", src_ddr, 0, count*CACHELINE_SIZE, LOOP_TIME, CACHELINE_SIZE, End_Time-Begin_Time);

#endif  /* SINGLE_LDR_32 */

#if     SINGLE_LDR_64
      /*
       *Test1: for single_ldr_64
       * David Yang, 08.11.02
       */
      /* for alignment cases */
      flush_all_cache();
      count = (MIN(bufferSize, L2MissBufSize))/CACHEBLOCK_SIZE;
      for(j=0;j<4;j++) {
#ifdef DRY_RUN
         single_ldr_64(src_ddr_a[j],count);
#endif
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            single_ldr_64(src_ddr_a[j],count);
         }
         End_Time = mrvl_times();
         dump_info("Single load #64", src_ddr_a[j], 0, count*CACHEBLOCK_SIZE, LOOP_TIME, CACHEBLOCK_SIZE, End_Time-Begin_Time);
      }

      /*
       *Test1: for single_ldr_Linked_64
       * Aladdin Chang, 2010.05.04
       */
      /* for alignment cases */
      flush_all_cache();
      count = (MIN(bufferSize, L2MissBufSize))/CACHEBLOCK_SIZE;
      for(j=0;j<4;j++) {
#ifdef DRY_RUN
         single_ldr_64(src_ddr_a[j],count);
#endif
         link_buffer_64 = (struct node_64*)(src_ddr_a[j]);
         for (i = 0; i < bufferSize / 64 - 1; i++)
         {
            link_buffer_64[i].next = (link_64 *)&link_buffer_64[i+1];
         }
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            single_ldr_64_linked(src_ddr_a[j],count);
         }
         End_Time = mrvl_times();
         dump_info("Single load linked #64", src_ddr_a[j], 0, count*CACHEBLOCK_SIZE, LOOP_TIME, CACHEBLOCK_SIZE, End_Time-Begin_Time);
      }

#endif  /* SINGLE_LDR_64 */

      /*
       *Test2: for single_ldr_4
       * David Yang, 08.11.02
       */
#if     SINGLE_LDR_4
      flush_all_cache();
      count = bufferSize/4;
#ifdef DRY_RUN
      single_ldr_4(src_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++)
      {
         single_ldr_4(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Single load #4", src_ddr, 0, count*4, LOOP_TIME, 4, End_Time-Begin_Time);
#endif  /* SINGLE_LDR_4 */

      /*
       *Test3: for pipe_ldr_4
       * David Yang, 08.11.02
       */
#if PIPE_LDR_4
      flush_all_cache();
      count=bufferSize/4;
#ifdef DRY_RUN
      pipe_ldr_4(src_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_ldr_4(src_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Pipelined load #4", src_ddr, 0, count*4, LOOP_TIME, 4, End_Time-Begin_Time);
#endif /* PIPE_LDR_4 */

      /*
       *Test4: for pipe_ldrd_8
       * David Yang, 08.11.02
       */
#if PIPE_LDRD_8
      flush_all_cache();
      count=bufferSize/8;
#ifdef DRY_RUN
      pipe_ldrd_8(src_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_ldrd_8(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Pipelined ldrd #8", src_ddr, 0, count*8, LOOP_TIME, 8, End_Time-Begin_Time);
#endif /* PIPE_LDRD_8 */

      /*
       *Test5: for pipe_ldr_32
       * David Yang, 08.11.02
       */
#if PIPE_LDR_32
      flush_all_cache();
      count = bufferSize/CACHELINE_SIZE;
#ifdef DRY_RUN
      pipe_ldr_32(src_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_ldr_32(src_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Pipelined Load #32", src_ddr, 0, count*CACHELINE_SIZE, LOOP_TIME, CACHELINE_SIZE, End_Time-Begin_Time);

      flush_all_cache();
      count = bufferSize/CACHELINE_SIZE;
#ifdef DRY_RUN
      pipe_pld_32(src_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_pld_32(src_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Pipelined pld #32", src_ddr, 0, count*CACHELINE_SIZE, LOOP_TIME, CACHELINE_SIZE, End_Time-Begin_Time);

      flush_all_cache();
      count = bufferSize/CACHELINE_SIZE;
#ifdef DRY_RUN
      pipe_pld_32(src_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_ldr_32_WOBU(src_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Pipelined ldr W/O BU #32", src_ddr, 0, count*CACHELINE_SIZE, LOOP_TIME, CACHELINE_SIZE, End_Time-Begin_Time);

#endif /* PIPE_LDR_32 */

      /*
       *Test6: for pipe_ldr_64
       * David Yang, 08.11.02
       */
#if PIPE_LDR_64
      flush_all_cache();
      count = bufferSize/CACHEBLOCK_SIZE;
#ifdef DRY_RUN
      pipe_ldr_64(src_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_ldr_64(src_ddr,count);
      }
      End_Time = mrvl_times();
      dump_info("Pipelined load #64", src_ddr, 0, count*CACHEBLOCK_SIZE, LOOP_TIME, CACHEBLOCK_SIZE, End_Time-Begin_Time);
#endif /* PIPE_LDR_64 */

      /*
       *Test7: for single_str_4
       * David Yang, 08.11.02
       */
#if SINGLE_STR_4
      flush_all_cache();
      count = bufferSize/4;
#ifdef DRY_RUN
      single_store_4(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         single_store_4(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Single Store #4", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);
#endif /* SINGLE_STR_4 */


      /*
                 *Test7_1: for pipe_str_4
                 * write 64K buffer, totally hit L2, and miss L1
                 * David Yang, 09.03.11
                 */
#if COALESCE_STR_4
      flush_all_cache();
      count = (MIN(bufferSize, L2HIT_BUF_SIZE))/4;
#ifdef DRY_RUN
      pipe_str_4(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_str_4(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced Store (STR)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

                /*
                 *Test7_1: for pipe_str_4
                 * David Yang, 09.03.11
                 */
      flush_all_cache();
      count = bufferSize/4;
#ifdef DRY_RUN
      pipe_str_4(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_str_4(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced Store (STR)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);
#endif /* COALESCE_STR_4 */

      /*
       *Test8: for pipe_strd_8
       * David Yang, 08.11.02
       */
#if COALESCE_STRD_8
      flush_all_cache();
      count=bufferSize/8;
#ifdef DRY_RUN
      pipe_strd_8(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_strd_8(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced Store (STRD)", 0, dst_ddr, count*8, LOOP_TIME, 8, End_Time-Begin_Time);
#endif /* COALESCE_STRD_8 */

      /*
       *Test9: for pipe_strd_8_postincr
       * David Yang, 08.11.02
       */
#if COALESCE_STRD_8_POSTINCR
      flush_all_cache();
      count=bufferSize/8;
#ifdef DRY_RUN
      pipe_strd_8_postincr(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_strd_8_postincr(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced Store (STRD post incr)", 0, dst_ddr, count*8, LOOP_TIME, 8, End_Time-Begin_Time);
#endif /* COALESCE_STRD_8_POSTINCR  */

      /*
       *Test10: for coalesce_store_4_stm
       * David Yang, 08.11.02
       */
#if COALESCE_STM_4
      flush_all_cache();
      count=bufferSize/4;
#ifdef DRY_RUN
      pipe_store_4_stm_2(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         coalesce_store_4_stm_2(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced Store (STM 2 reg list)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

      flush_all_cache();
      count=bufferSize/4;
#ifdef DRY_RUN
      coalesce_store_4_stm_4(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         coalesce_store_4_stm_4(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced Store (STM 4 reg list)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

      flush_all_cache();
      count=bufferSize/4;
#ifdef DRY_RUN
      coalesce_store_4_stm_8(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         coalesce_store_4_stm_8(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced Store (STM 8 reg list)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);
#endif /* COALESCE_STM_4 */

      /*Memcpy  Test*/
#if     MEMCPY_TEST
      /*
       *Test11: for memcopy
       * David Yang, 08.11.02
       */
      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            memcpy(dst_ddr, src_ddr, j);
         }
         End_Time = mrvl_times();
         dump_info("Memcpy (system default)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }

      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            memcpy_feroceon(dst_ddr, src_ddr, j);
         }
         End_Time = mrvl_times();
         dump_info("Memcpy (feroceon)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }

      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            memcpy_xsc_v5(dst_ddr, src_ddr, j);
         }
         End_Time = mrvl_times();
         dump_info("Memcpy (xscv5)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }
#ifndef  _TMARK_WCE
      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            memcpy_glibc_opt(dst_ddr, src_ddr, j);
         }
         End_Time = mrvl_times();
         dump_info("Memcpy (glibc_opt)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }

      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            memcpy_fast(dst_ddr, src_ddr, j);
         }
         End_Time = mrvl_times();
         dump_info("Memcpy (mrvl_fast)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }

#if !defined(VEXPRESS) && !defined(ENABLE_TCL2) && defined(ENABLE_SL2) && defined(CPU_V7)
      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            memcpy_32k(dst_ddr, src_ddr, j);
         }
            End_Time = mrvl_times();
         dump_info("Memcpy (memcpy_32k)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }
#endif /* if !defined(VEXPRESS) */

      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            mrvl_memcpy_v2(dst_ddr, src_ddr, j);
         }
            End_Time = mrvl_times();
         dump_info("Memcpy (memcpy_v2)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }

      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            mrvl_memcpy_v3(dst_ddr, src_ddr, j);
         }
            End_Time = mrvl_times();
         dump_info("Memcpy (memcpy_v3)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }

      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            mrvl_memcpy_v3_2(dst_ddr, src_ddr, j);
         }
            End_Time = mrvl_times();
         dump_info("Memcpy (mrvl_memcpy_v3_2)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }

      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            mrvl_memcpy4(dst_ddr, src_ddr, j);
         }
            End_Time = mrvl_times();
         dump_info("Memcpy (mrvl_memcpy4)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }

#ifdef PJ4B_MP
      for(j=4096;j<=L2MissBufSize;j*=2) {
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++)
         {
            mrvl_memcpy_v7(dst_ddr, src_ddr, j);
         }
            End_Time = mrvl_times();
         dump_info("Memcpy (mrvl_memcpy_v7)", src_ddr, dst_ddr, j, LOOP_TIME, 1, End_Time-Begin_Time);
      }
#endif

#endif /*_TMARK_WCE */
#endif



#if MISC_TEST
      /* ReadTest from RIM */
      flush_all_cache();
      count = (MIN(bufferSize, L2MissBufSize));

      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++)
      {
        /* @ DWORD ReadTest( void *buf, DWORD sz )@
           @ r0 - address of buffer
           @ r1 - size in bytes
        */
         ReadTest(src_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("ReadTest form RIM", src_ddr, 0, count, LOOP_TIME, CACHELINE_SIZE, End_Time-Begin_Time);

                /*
                 * pipe_strh_4
                 */
      flush_all_cache();
      count = bufferSize/4;
#ifdef DRY_RUN
      pipe_strh_4(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_strh_4(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced_Store Halfword (STRH)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

                /*
                 * pipe_strb_4_1
                 */
      flush_all_cache();
      count = bufferSize/4;
#ifdef DRY_RUN
      pipe_strb_4_1(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_strb_4_1(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced_Store Byte #1 (STRB)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

                /*
                 * pipe_strb_4_2
                 */
      flush_all_cache();
      count = bufferSize/4;
#ifdef DRY_RUN
      pipe_strb_4_2(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_strb_4_2(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced_Store Byte #2 (STRB)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

                /*
                 * pipe_str_1_8_7
                 */
      flush_all_cache();
      count = bufferSize/4;
#ifdef DRY_RUN
      pipe_str_1_8_7(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_str_1_8_7(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced_Store 1-8-7", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

                /*
                 * pipe_str_9_7
                 */
      flush_all_cache();
      count = bufferSize/4;
#ifdef DRY_RUN
      pipe_str_9_7(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         pipe_str_9_7(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced_Store 9-7", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

      /*
       * test store 7 skip 1
       */
      flush_all_cache();
      count=bufferSize/4;
#ifdef DRY_RUN
      coalesce_store_4_stm_7_skip_1(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         coalesce_store_4_stm_7_skip_1(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Coalesced_Store (STM 7 skip 1)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

      /*
       * single_str_4_min_1
       */
      flush_all_cache();
      count = bufferSize/4;
#ifdef DRY_RUN
      single_store_4_min_1(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         single_store_4_min_1(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Single_Store #4 (min1)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

      /*
       * single_str_4_min_1
       */
      flush_all_cache();
      count = bufferSize/4;
#ifdef DRY_RUN
      single_store_4_min_1(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         single_store_4_min_1(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Single_Store #4 (min1)", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

      /*
       * combine_store_4
       */
      flush_all_cache();
      count=bufferSize/4;
#ifdef DRY_RUN
      combine_store_4(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         combine_store_4(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Combine_Store_4", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

      /*
       * hop_store_4_1
       */
      flush_all_cache();
      count=bufferSize/4;
#ifdef DRY_RUN
      hop_store_4_1(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         hop_store_4_1(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Hop_Store_4_1", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

      /*
       * hop_store_4_2
       */
      flush_all_cache();
      count=bufferSize/4;
#ifdef DRY_RUN
      hop_store_4_2(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         hop_store_4_2(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Hop_Store_4_2", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

      /*
       * hop_store_4_3
       */
      flush_all_cache();
      count=bufferSize/4;
#ifdef DRY_RUN
      hop_store_4_3(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         hop_store_4_3(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Hop_Store_4_3", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);

      /*
       * hop_store_4_4
       */
      flush_all_cache();
      count=bufferSize/4;
#ifdef DRY_RUN
      hop_store_4_4(dst_ddr,count);
#endif
      Begin_Time = mrvl_times();
      for(i=0;i<LOOP_TIME;i++){
         hop_store_4_4(dst_ddr, count);
      }
      End_Time = mrvl_times();
      dump_info("Hop_Store_4_4", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);
#if DELAY_STORE
      /*
       * delay_store_4
       */
      for (j=1; j < 50; j++) {
         flush_all_cache();
         count=bufferSize/4;
         Begin_Time = mrvl_times();
         for(i=0;i<LOOP_TIME;i++){
            delay_store_4(dst_ddr, count, j);
         }
         End_Time = mrvl_times();
         bbu_printf("j=%d: ", j);
         dump_info("Delay_Store_4", 0, dst_ddr, count*4, LOOP_TIME, 4, End_Time-Begin_Time);
      }
#endif
#endif /* MISC_TEST */
      dump_analysis();
   }

   if(src_ddr_malloc)
      bbu_vm_free(src_ddr_malloc);
   if(dst_ddr_malloc)
      bbu_vm_free(dst_ddr_malloc);

   return 0;
}
/* EOF */

