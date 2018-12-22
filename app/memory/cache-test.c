#include "predefines.h"
#include "cache-test.h"
#include "utils.h"
#include "perf_counter.h"
#include <string.h>
#include "bbu.h"
#include "timer_dec.h"
#include "set_mmu.h"
#include "cp15.h"
#ifdef CONFIG_SMP
extern void bbu_eica_asm(void);
extern void bbu_eica_asm_secondary(void);
extern void bbu_dica_asm_secondary(void);
#endif
extern void bbu_dica_asm(void);

static void print_stars(int enter_before, int enter_after);
static void print_test_status(int pass);
static uint32_t getExtraCycle(void);
uint32_t cache_test_buffer;

/******************************************************************************
* Function     :   bbu_eica
*******************************************************************************
*
* Description  :   Enable cache features according to attributes
*
* Parameters   :   IsHexValDetec:   1- Enable according to attributes
*                                   0- Enable all.
*
*                  Value        :   bitwise attributes.
*
******************************************************************************/
#ifdef CONFIG_CACHE
uint32_t cache_enabled = 1;
#else
uint32_t cache_enabled = 0;
#endif

void bbu_eica(uint8_t IsHexValDetec, uint16_t value){
   int timeout = 0x40000;
#ifdef CONFIG_SMP
   (void)IsHexValDetec;
   (void)value;
   if(cache_enabled == 0){
      cache_enabled = 1;
      sgi_broadcast_all_but_self(GIC_INT_SW_11);
      while(timeout--);
      l1_dcache_enable();
      l1_icache_enable();
      __asm__ ("SEV");
   }
#else
     /* if IsHexValDetec == 0, enable all caches */
   if(!IsHexValDetec)
      value = 0xFF;
   if((value & I_CACHE_ENABLE) && !is_i_cache_enabled()){
      l1_icache_invalidate_all();
      l1_icache_enable();
   }
   
   if((value & D_CACHE_ENABLE) && !is_d_cache_enabled()){
      dcache_invalidate_all();
      l1_dcache_enable();
   }
#endif
}

/******************************************************************************
* Function     :   bbu_dica
*******************************************************************************
*
* Description  :    Disable all cache features
*
******************************************************************************/
void bbu_dica(void){
   int timeout = 0x40000;
#ifdef CONFIG_SMP
   if(cache_enabled == 1){
	    cache_enabled = 0;
      sgi_broadcast_all_but_self(GIC_INT_SW_11);
      while(timeout--);
      l1_dcache_disable();
      l1_icache_disable();
      l1_icache_invalidate_all();
	   dcache_clean_invalidate_all();
	   __asm__ ("SEV");
   }
#else
   l1_dcache_disable();
   l1_icache_disable();
   l1_icache_invalidate_all();
   dcache_clean_invalidate_all();
#endif
}


#ifdef CONFIG_SMP
void bbu_eica_secondary_core(void){
   l1_dcache_enable();
   l1_icache_enable();
   __asm__ ("WFE");
}
/* this function should be called in interrupt handler */
void bbu_dica_secondary_core(void){
	   l1_dcache_disable();
	   l1_icache_disable();
	   l1_icache_invalidate_all();
	   l1_dcache_clean_invalidate_all();
	   __asm__ ("WFE");
}

int bbu_do_eica(void){
   return cache_enabled;
}
#endif

/*..........................................................................*/
/* this function is defined in bbu_memt.c */
extern uint32_t memory_test(uint32_t start_addr, uint32_t byte_cnt );
// run the test
static uint32_t diag_sl2_test_ddr2(uint32_t addr, uint32_t cnt)
{
   uint32_t m0, m1, m2;
   Counters l2d_access_counter, l2d_refill_counter, l2d_wb_counter;
   

   perf_counters_reset();

   ConfigPerformanceCounter(L2D_CACHE_ACCESS, &l2d_access_counter);
   ConfigPerformanceCounter(L2D_CACHE_REFILL, &l2d_refill_counter);
   ConfigPerformanceCounter(L2D_CACHE_WB,     &l2d_wb_counter);

   //start counters
   StartAllCounters();

   //memory test
   memory_test(addr,cnt);

   //disable counters
   StopAllCounters();

   //read back the counters
   ReadCounter(l2d_access_counter, &m0);
   bbu_printf("System L2 cache Data access: %d\n\r", m0);
   ReadCounter(l2d_refill_counter, &m1);
   bbu_printf("System L2 cache Data refill: %d\n\r", m1);
   ReadCounter(l2d_wb_counter, &m2);
   bbu_printf("System L2 cache Data write-back: %d\n\r", m2);

   return MV_OK;
}

/******************* System L2 cache test function  *************************/
/*..........................................................................*/

uint32_t bbu_sl2c_test(int argc, uint8_t **argv)
{
   uint32_t addr1, cnt, loop;
   uint32_t i;   

   if( argc < 4 )
   {
      bbu_printf("Usage:\n\r");
      bbu_printf("%s [address] [cnt] [loop]\r\n",argv[0]);
      return MV_FAIL;
   }
   
   addr1 = conv_hex( (char *)argv[1] );
   cnt   = conv_hex( (char *)argv[2] );
   loop  = conv_hex( (char *)argv[3] );

//   l2_cache_clean_invalidate_all();

   bbu_printf("\n\r*******************************************************"
              "****************");
   bbu_printf("\n\r                       System L2 cache test\n\r");
   bbu_printf("Test address: 0x%08X, count: 0x%08X loop: 0x%08X\r\n\n\r", 
              addr1, cnt, loop);

   for(i=0;i<loop;i++)
   {
      bbu_printf(".................Test loop %d.................\n\r", i+1);
      diag_sl2_test_ddr2(addr1,cnt);
   }
   bbu_printf("*******************************************************"
              "****************\n\r");

   return MV_OK;
}

/*..........................................................................*/
static uint32_t bbu_L1_Dcache_test(void)
{
   uint32_t *array1 = NULL;
   uint32_t i;
   uint32_t hit_val = 0, miss_val = 0;
   uint32_t returnVal;

   Counters miss_counter, hit_counter;
   volatile int x=0;

   array1 = (uint32_t *)L1_DCACHE_TEST_ADDRESS;

   //Read test
   ConfigPerformanceCounter(D_CACHE_READ_HIT,  &hit_counter);
   ConfigPerformanceCounter(D_CACHE_READ_MISS, &miss_counter);

   SetCounter(hit_counter,  0x0);
   SetCounter(miss_counter, 0x0);

   dcache_clean_invalidate_all();
   l1_icache_invalidate_all();

   StartAllCounters();

   for (i=0; i<(L1_CACHE_SIZE>>2); i++)
   {
      x = array1[i];
   }

   StopAllCounters();

   ReadCounter(miss_counter, &miss_val);
   ReadCounter(hit_counter,  &hit_val);

   bbu_printf("\n\rTotal reads          = 0x%X\n\r", i);

   hit_val -= miss_val;
   bbu_printf("\n\rL1 D-cache miss+hit        = 0x%X\n\r",miss_val+hit_val);
   bbu_printf("\n\rL1 D-cache miss counter    = 0x%X\n\r",miss_val);
   bbu_printf("\n\rL1 D-cache Hit  counter    = 0x%X\n\r",hit_val);
   bbu_printf("\n\rL1 D-Cache ratio Miss/Hit  = 1 / %.2f\n\r",
              (float)hit_val/(float)miss_val);

   /* We expect retion to be 7 */
   if((float)hit_val/(float)miss_val > 6.0){
      print_test_status(1);
      returnVal = 0;
   }else{   
      print_test_status(0);
      returnVal = 1;
   }
   ReleaseCounter(miss_counter);
   ReleaseCounter(hit_counter);
   (void)x; /* just make compiler happy */
   return returnVal;
}

/*..........................................................................*/
static uint32_t bbu_L1_Icache_test(void)
{
   volatile int i;
   uint32_t miss_val = 0;
   Counters miss_counter;
   uint32_t returnVal;

   ConfigPerformanceCounter(I_CACHE_READ_MISS, &miss_counter);
   SetCounter(miss_counter, 0x0);

   l1_icache_invalidate_all();

   StartAllCounters();

   for (i=0; i<128; i++)
   {
      // Do nothing. Count only index increment
   }

   StopAllCounters();

   ReadCounter(miss_counter, &miss_val);

   bbu_printf("\n\rTotal reads          = 0x%X\n\r", i);
   bbu_printf("\n\rL1 I-Cache miss counter      = 0x%X\n\r",miss_val);

   if(miss_val <= 15){
      print_test_status(1);
      returnVal = 0;
   }else{
      print_test_status(0);
      returnVal = 1;
   }

   ReleaseCounter(miss_counter);

   return returnVal;
}

/*..........................................................................*/
static uint32_t bbu_L2_Dcache_test(int Prefetch)
{

   uint32_t *array1 = NULL;
   int i;
   uint32_t L1_miss, L2_hit, L2_miss;
   Counters L1_miss_counter, L2_hit_counter, L2_req_counter;
   volatile int x=0;
   uint32_t returnVal;
   
   array1 = (uint32_t *)L2_CACHE_TEST_ADDRESS;

   ConfigPerformanceCounter(L2D_CACHE_ACCESS,  &L2_req_counter);
   ConfigPerformanceCounter(L2D_CACHE_REFILL,  &L2_hit_counter);
   ConfigPerformanceCounter(D_CACHE_READ_MISS, &L1_miss_counter);
   
   dcache_clean_invalidate_all();
   l1_icache_invalidate_all();

   SetCounter(L2_req_counter, 0x0);
   SetCounter(L2_hit_counter,  0x0);
   SetCounter(L1_miss_counter, 0x0);
   
   StartAllCounters();

   for (i = 0; i<128; i++)  //TOMER = 0x80 - should be all miss in L1
   {
      l1_dcache_clean_all();
      l1_dcache_invalidate_all();
      x = array1[i];
   }
   (void)x; /* just make compiler happy */

   StopAllCounters();
   ReadCounter(L2_req_counter,  &L2_miss);   /* L2_miss is L2 request count */
   ReadCounter(L2_hit_counter,  &L2_hit);
   ReadCounter(L1_miss_counter, &L1_miss);
   L2_miss -= L2_hit; /* Get real L2 miss count */

   bbu_printf("\n\rTotal reads          = 0x%X\n\r",i);

  /*
   // Round-down L2-misses to fit with L1-misses
   dummy = (L2_hit+L2_miss) - L1_miss;
   L2_miss -= dummy;

   // Round-down L1-misses to fit with total reads
   dummy = L1_miss - (*i);
   L1_miss -= dummy;

   L2_hit =L1_miss -L2_miss;
 */

   L2_miss = L1_miss - L2_hit;
   
   bbu_printf("\n\rL1 D-cache read miss counter = 0x%X\n\r",L1_miss);
   bbu_printf("\n\rL2 cache read miss+hit       = 0x%X\n\r",L2_hit+L2_miss);
   bbu_printf("\n\rL2 read miss counter         = 0x%X\n\r",L2_miss);
   bbu_printf("\n\rL2 read hit  counter         = 0x%X\n\r",L2_hit);
   bbu_printf("\n\rL2 ratio read Miss/Hit       = 1 / %.2f\n\r",
              (float)L2_hit/(float)L2_miss);

   /* We expect ratio to be 7 when prefetch disabled  
      and 15   when prefetch enabled*/
   if((Prefetch == DISABLED && L2_hit/L2_miss > 6) ||
      (Prefetch == ENABLED  && L2_hit/L2_miss > 14) ){
      print_test_status(1);
      returnVal = 0;
   }else{
      print_test_status(0);
      returnVal = 1;
   }

   ReleaseCounter(L1_miss_counter);
   ReleaseCounter(L2_hit_counter);
   ReleaseCounter(L2_req_counter);
   return returnVal;
}

/*..........................................................................*/
static uint32_t bbu_L2_Icache_test(void)
{
   uint32_t i = 0;
   uint32_t L1_miss = 0, L2_miss, L2_hit;
   uint32_t L1_I_cache_miss_overhead;
   uint32_t L2_I_cache_miss_overhead, L2_I_cache_hit_overhead;
   Counters I_cache_miss_counter, L2_read_req_counter, L2_read_hit_counter;
   uint32_t returnVal;

   L1_I_cache_miss_overhead = 0; 
   L2_I_cache_miss_overhead = 0; 
   L2_I_cache_hit_overhead = 0;

   ConfigPerformanceCounter(I_CACHE_READ_MISS, &I_cache_miss_counter);
   ConfigPerformanceCounter(L2D_CACHE_ACCESS,  &L2_read_req_counter);
   ConfigPerformanceCounter(L2D_CACHE_REFILL,  &L2_read_hit_counter); 

   getCounterOverhead(I_cache_miss_counter,  &L1_I_cache_miss_overhead);
   getCounterOverhead(L2_read_hit_counter,   &L2_I_cache_hit_overhead );
   getCounterOverhead(L2_read_req_counter,   &L2_I_cache_miss_overhead);
   
   SetCounter(I_cache_miss_counter, 0x0);
   SetCounter(L2_read_req_counter,  0x0);
   SetCounter(L2_read_hit_counter,  0x0);
   
   
   l1_icache_invalidate_all();
   dcache_clean_invalidate_all();
   StartAllCounters();

   for (i = 0; i<128;i++)
   {
      cp15_reg_write(c7, c5, 0, 0); //l1_icache_invalidate_all();   // In order to fetch from L2$ and not L1$
      dsb();
   }

   StopAllCounters();

   ReadCounter(I_cache_miss_counter,&L1_miss);
   ReadCounter(L2_read_req_counter,&L2_miss);/* L2_miss is L2 request count */
   ReadCounter(L2_read_hit_counter, &L2_hit);
   L1_miss -= L1_I_cache_miss_overhead;
   L2_miss -= L2_I_cache_miss_overhead;
   L2_hit  -= L2_I_cache_hit_overhead;
   L2_miss -= L2_hit;  /* Get real L2 miss count */

   bbu_printf("\n\rL1 I-Cache miss counter  = 0x%X\n\r", L1_miss);
   bbu_printf("\n\rL2-Cache hit+miss        = 0x%X\n\r", L2_hit+L2_miss);
   bbu_printf("\n\rL2-Cache hit counter     = 0x%X\n\r", L2_hit);
   bbu_printf("\n\rL2-Cache miss counter    = 0x%X\n\r", L2_miss);

   if (L2_miss <= 10)   //change 8 to 10 in Emei
   {
      print_test_status(1);
      returnVal = 0;
   }
   else
   {
      print_test_status(0);
      returnVal = 1;
   }

   ReleaseCounter(I_cache_miss_counter);
   ReleaseCounter(L2_read_req_counter);
   ReleaseCounter(L2_read_hit_counter);

   return returnVal;
}

/*..........................................................................*/
static void 
bbu_Cache_Read_Latency_test(uint8_t silent, ReadLatency *readLatency)
{
   //readLatency - Save the results for use in the write Latency Test
   uint32_t i, Extra_Cycles;
   volatile uint32_t x;
   volatile uint32_t sum, t1, t2;

   uint32_t *array1 = (uint32_t *)L2_CACHE_TEST_ADDRESS;

   SetCycleCounter(0x0);
   StartCycleCounter();
   
   //   L1 D-Cache read duration test
   sum = 0;
   bbu_dica();
   bbu_eica(1, D_CACHE_ENABLE | I_CACHE_ENABLE);
   Extra_Cycles = getExtraCycle(); //must be AFTER I cache enable

   dcache_clean_invalidate_all();
   x = array1[0]; //Insert to cache   

   for (i=0; i<(L1_CACHE_SIZE>>2); i++)
   {
      t1 = ReadCycleCounter();
      x  = array1[0];
      t2 = ReadCycleCounter();
      sum += (t2 - t1 - Extra_Cycles);
   }
   
   readLatency->L1_DATA = (float)sum/(float)i;
   
   if(!silent){
      bbu_printf("Average D read  time from L1   = %.8f\n\r",
                 readLatency->L1_DATA);
   }
   
   //   L2 D-Cache read duration test
   sum = 0;
   bbu_dica();
//   l1_dcache_invalidate_all();
//   bbu_eica(1, D_CACHE_ENABLE | L2_CACHE_ENABLE | I_CACHE_ENABLE);
   bbu_eica(1, L2_CACHE_ENABLE | I_CACHE_ENABLE);

   x = array1[0];
   for (i=0; i<10; i++)
   {
//      l1_dcache_clean_invalidate_all();//In order to read from L2$ and not L1$
      t1 = ReadCycleCounter();
      x = array1[0];
      t2 = ReadCycleCounter();
      sum += (t2 - t1 - Extra_Cycles);
   }
   
   readLatency->L2_DATA = (float)sum/(float)i;
   
   if(!silent)
   {
      bbu_printf("Average D read  time from L2   = %.8f\n\r",
                  readLatency->L2_DATA);
   }

   //   DDR read duration test
   sum = 0;
   bbu_dica();
   bbu_eica(1, I_CACHE_ENABLE);

   for (i=0; i<(L2_CACHE_SIZE>>2); i++)
   {
      t1 = ReadCycleCounter();
      x = array1[0];
      t2 = ReadCycleCounter();
      sum += ( t2 - t1 - Extra_Cycles);
   }
   
   readLatency->DDR_DATA = (float)sum / (float)i;
   
   if(!silent){
      bbu_printf("Average D read  time from DDR  = %.8f\n\r",
                  readLatency->DDR_DATA);
   }

   SetCycleCounter(0x0);
   StartCycleCounter();

   //   L1 I-Cache fetch duration test
   sum = 0;
   bbu_dica();
   l1_dcache_invalidate_all();
   bbu_eica(1, D_CACHE_ENABLE | I_CACHE_ENABLE);

   t1 = ReadCycleCounter();
   for (i=0; i<128; i++)
   {
   }
   t2 = ReadCycleCounter();

   sum = t2 - t1 - Extra_Cycles;
   
   readLatency->L1_INST = (float)sum / (float)(i*3);
   
   if(!silent){
      bbu_printf("Average I fetch time from L1   = %.8f\n\r",
                  readLatency->L1_INST);
   }

   //   L2 I-Cache fetch duration test
   sum = 0;
   bbu_dica();
   bbu_eica(1, D_CACHE_ENABLE | I_CACHE_ENABLE | L2_CACHE_ENABLE);

   t1 = ReadCycleCounter();
   for (i=0; i<128; i++)
   {
      t2 = ReadCycleCounter();
      l1_icache_invalidate_all();   // In order to fetch from L2$ and not L1$
      sum += (t2 - t1 - Extra_Cycles);
      t1 = ReadCycleCounter();
   }
   
   readLatency->L2_INST = (float)sum / (float)(i*3);
   
   if(!silent){
      bbu_printf("Average I fetch time from L2   = %.8f\n\r",
                 readLatency->L2_INST);
   }

   //   DDR fetch duration test
   sum = 0;
   bbu_dica();
   t1 = ReadCycleCounter();
   for (i=0; i<128; i++)
   {
      t2 = ReadCycleCounter();
      sum += (t2 - t1 - Extra_Cycles);
      t1 = ReadCycleCounter();
   }

   readLatency->DDR_INST = (float)sum / (float)(i*3);
   
   if(!silent)
   {
      bbu_printf("Average I fetch time from DDR  = %.8f\n\r",
                readLatency->DDR_INST);      
      if(!(readLatency->L1_DATA < readLatency->L2_DATA &&
          readLatency->L2_DATA < readLatency->DDR_DATA)  ||
         !(readLatency->L1_INST < readLatency->L2_INST &&
           readLatency->L2_INST < readLatency->DDR_INST)
         )
      bbu_printf("Read Latency Test Failed\n\r");//TODO - add to BBU errors
      print_stars(2,2);
   }
   (void)x; /* just make compiler happy */
}

/*..........................................................................*/
static void bbu_Cache_Write_Latency_test(ReadLatency *readLatency)
{
   uint32_t i ,   Extra_Cycles;
   volatile uint32_t x;
   uint32_t sum = 0, t1, t2;

   uint32_t *array1 = (uint32_t *)L2_CACHE_TEST_ADDRESS;
   SetCycleCounter(0x0);
   StartCycleCounter();

   //   L1 D-Cache write duration test
   l1_dcache_invalidate_all();
   bbu_eica(1, D_CACHE_ENABLE | I_CACHE_ENABLE);
   Extra_Cycles = getExtraCycle();

   sum = 0;
   x = array1[0];
   
   for (i=0; i<(L2_CACHE_SIZE>>2); i++)
   {
      t1 = ReadCycleCounter();
      array1[0] = 5;
      x = array1[0]; //dummy read to make sure that write is over
      t2 = ReadCycleCounter();
      sum += (t2 - t1 - Extra_Cycles - (uint32_t)readLatency->L1_DATA);
   }

   bbu_printf("Average D write time to L1   = %.8f\n\r",(float)sum/(float)i);


   //   L2 D-Cache write duration test
   sum = 0;
//   bbu_dica();
   bbu_eica(1, L2_CACHE_ENABLE);

   x = array1[0];

   for (i=0; i<10; i++)
   {
      l1_dcache_clean_invalidate_all();//In order to read from L2$ and not L1$
      t1 = ReadCycleCounter();
      array1[0] = 5;
      x = array1[0]; //dummy read to make sure that write is over
      t2 = ReadCycleCounter();
      sum += (t2 - t1 - Extra_Cycles - (uint32_t)readLatency->L2_DATA);
   }

   bbu_printf("Average D write time to L2   = %.8f\n\r",(float)sum / (float)i);


   //   DDR write duration test
   sum = 0;
   bbu_dica();
   bbu_eica(1, I_CACHE_ENABLE);

   for (i=0; i<10; i++)
   {
      t1 = ReadCycleCounter();
      array1[0] = 5;
      x = array1[0]; //dummy read to make sure that write is over
      t2 = ReadCycleCounter();
      sum += (t2 - t1 - Extra_Cycles - (uint32_t)readLatency->DDR_DATA);
   }
   (void)x; /* just make compiler happy */
   bbu_printf("Average D write time to DDR  = %.8f\n\r",(float)sum/(float)i);

   print_stars(2,2);
   bbu_dica();
}

/*****************************************************************************
* Function     :   bbu_cache_test
******************************************************************************
*
* Description  :    BBU cache tests
*
* Parameters   :   IsHexValDetec:   1- Perform tests according to attributes.
*                                   0- Perform all tests.
*
*                  Value        :   bitwise attributes.
*
*****************************************************************************/
uint8_t bbu_cache_test(int argc, uint8_t **argv)
{
   uint32_t returnVal = 0, Value = 0xFF;
   uint8_t result = 0;
   /* TOMER - must run read latency test before write latency test */
   ReadLatency readLatency; 

   /* Analyze the arguments */
   switch(argc){
      case 1:
         Value = 0xFF;
         break;
      case 2:{
         if(check_help(argv[1])){
            bbu_ctst_help();
            return 0;
         }
         if(strcmp((const char*)argv[1], "l1d") == 0)
            Value = L1_DCACHE_TEST;
         else if(strcmp((const char*)argv[1], "l1i") == 0)
            Value = L1_ICACHE_TEST;
         else if(strcmp((const char*)argv[1], "l2d") == 0)
            Value = L2_DCACHE_TEST;
         else if(strcmp((const char*)argv[1], "l2i") == 0)
            Value = L2_ICACHE_TEST;
         else if(strcmp((const char*)argv[1], "rl") == 0)
            Value = CACHE_READ_LATENCY_TEST;
         else if(strcmp((const char*)argv[1], "wl") == 0)
            Value = CACHE_WRITE_LATENCY_TEST;
         else{
            bbu_printf("ctst-->  The argument is wrong\n\r");
            return 0;
         }
         break;
      }
      default:{
         bbu_printf("ctst-->  The argument is wrong\n\r");
         return result;
      }
   }

   bbu_dica();
   bbu_printf("\n\r");
   print_stars(0,1);
   bbu_printf("*              BBU cache test              *\n\r");
   print_stars(0,2);

   bbu_printf("\n\rAPPS side\n\r");
   cache_test_buffer = (uint32_t)bbu_vm_malloc(0x100000, CACHEABLE);
   if(cache_test_buffer == 0){
      bbu_printf("Error: Can't malloc test buffer\n\r");
      return result;
   }
   //print memory region
   returnVal = L1_DCACHE_TEST_ADDRESS;
   bbu_printf("\n\rMemory address: 0x%X\n\r",returnVal);

   /* Requierd in seagull, because the other counters are not enougth */
   ReleaseCycleCounter();

   if ( Value & L1_DCACHE_TEST )
   {
      bbu_printf("Testing L1 D-Cache...\n\r\n\r");
      bbu_dica();
      bbu_eica(1, D_CACHE_ENABLE | I_CACHE_ENABLE);
      returnVal = bbu_L1_Dcache_test();
      result |= (returnVal);
   }

   if ( Value & L1_ICACHE_TEST )
   {
      bbu_printf("Testing L1 I-Cache...\n\r\n\r");
      bbu_dica();
      bbu_eica(1, I_CACHE_ENABLE);
      returnVal = bbu_L1_Icache_test();
      result |= (returnVal<<1);
   }

   if ( Value & L2_ICACHE_TEST )
   {
      bbu_printf("Testing L2 I-Cache...\n\r\n\r");
      bbu_dica();
      bbu_eica(1, D_CACHE_ENABLE|I_CACHE_ENABLE|L2_CACHE_ENABLE|BPU_ENABLE);
      returnVal = bbu_L2_Icache_test();
      result |= (returnVal<<3);
   }


   if ( Value & L2_DCACHE_TEST )
   {
      bbu_printf("Testing L2 D-Cache with prefetch DISABLED...\n\r\n\r");
      bbu_dica();
      bbu_eica(1, D_CACHE_ENABLE|I_CACHE_ENABLE|L2_CACHE_ENABLE|BPU_ENABLE);
      returnVal = bbu_L2_Dcache_test(DISABLED);
      bbu_printf("Testing L2 D-Cache with prefetch ENABLED...\n\r\n\r");
      bbu_dica();
      bbu_eica(1, D_CACHE_ENABLE | I_CACHE_ENABLE |L2_CACHE_ENABLE | \
                  L2_CACHE_PREFETCH_ENABLE|BPU_ENABLE);   
      returnVal |= bbu_L2_Dcache_test(ENABLED);

      result |= (returnVal<<2);
   }

   if ( Value & CACHE_READ_LATENCY_TEST )
   {
      bbu_printf("Testing Cache read latency...\n\r\n\r");
      bbu_dica();
      bbu_Cache_Read_Latency_test(0,&readLatency);
   }

   if ( Value & CACHE_WRITE_LATENCY_TEST )
   {
      if ((Value & CACHE_READ_LATENCY_TEST) == 0)
      {  //gather latency results for write test
         bbu_Cache_Read_Latency_test(1,&readLatency);
      }
      bbu_printf("Testing Cache write latency... \n\r"
                 "WRITE ALLOCATE DISABLED\n\r\n\r");
      bbu_dica();
      bbu_Cache_Write_Latency_test(&readLatency);
#if 0  //Write allocate currently not supported
      bbu_printf("Testing Cache write latency... \n\r"
                  "WRITE ALLOCATE ENABLED\n\r\n\r");
      bbu_eica(1, WRITE_ALLOCATE_ENABLE);
      bbu_Cache_Write_Latency_test(&readLatency);
#endif
   }

   bbu_dica();
   bbu_vm_free((void *)cache_test_buffer);
   return result;
}

/*****************************************************************************
* Print functions
*****************************************************************************/
static void print_stars(int enter_before, int enter_after)
{
   while(enter_before)
   {
      bbu_printf("\n\r");
      enter_before--;
   }
   
   bbu_printf("********************************************");
      
   while(enter_after)
   {
      bbu_printf("\n\r");
      enter_after--;
   }   
}

static void print_test_status(int pass)
{
   bbu_printf("\n\r\n\r###  TEST ");
   
   if(pass)
      bbu_printf("PASSED");
   else
      bbu_printf("FAILED");
   
   bbu_printf(" !!!  ###\n\r\n\r");
   print_stars(1,2);
}

static uint32_t getExtraCycle(void)
{	
#define CA9
#if 0
	UINT32 t1, t2;
	t1 = ReadCycleCounter();
	t2 = ReadCycleCounter();
	return (t2 - t1);
#else
#ifdef CA9
	return 0;
#else
	return 5;
#endif
#endif
}
