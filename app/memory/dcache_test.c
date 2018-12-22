/*****************************************************************************
this test purpose:

1. verify if D$ and L2 is working (foo foo test. rely on latency of 
   hit comparing latency of miss)
2. check the cache memory. reading known value and calculate checksum 
   that can be verify offline ( different patterns can be used)
3. give good sense on D$ cache miss latency, D$ cache hit (both D$ and L2) 
   latency in core cycle

notes:
- best if the RW,ZI,RO of this test will be in a region with 
  instruction cache enable and data cache disable.
- the test assume that the core cycle counter is running.

- the test receive:
testBuffer - the location of the buffer that will be used for the test
pattern - the pattern that will be written to cache dword
readDword  - which DWORD in cahce line will be used for the test ( values 0-7)
L2cacheDataTest - any value other then 0 means that 
                  L2 cache will be tested too.
cacheSize - the size of the cache under test. for most of the cases it 
            will be sizeof(testBuffer)
*****************************************************************************/
#include "predefines.h"
#include "qf_port.h"
#include "cache_ops.h" //Same API as in RTOS
#include "perf_counter.h"
#include "core_tests.h"
#include "utils.h"

#define DWORD_IN_CACHE_LINE (8) 
#define TEST_ARR_SIZE (4*1024)
#define DIFF_WRAP_AROUND(t1,t2) ((t1>=t2)?(t1-t2):(t1+(0xffffffff-t2)))

extern uint32_t cacheD_runCache(uint32_t Address,uint32_t Size);

#define DataCacheCleanAndInvalidateWithoutL2  l1_dcache_clean_invalidate_all

/* ........................................................................ */
int dcacheFooTest(dcacheparams *par, PRINT_MODE silent)
{
   uint32_t t1,t2,t3,t4=0,t5=0;
   uint32_t cpsr,i;
   uint32_t checksum1=0,checksum2=0,checksum3=0;
   int    result,pass,fail;
   uint32_t *testBuf=(uint32_t *)(((uint32_t)(par->address)+32)&0xFFFFFFE0);
#if 0         
   if(!par->L2cacheDataTest)
   {
      InvalidateAllDataCache();
      //L2 disable;
      DisableL2Cache();
   }
#endif //0
   cpsr=disable_irq_fiq();
   
   StartCycleCounter();
   
   // Check Valid configuration before test
   t1 = ReadCycleCounter();
   if(t1 == 0){
      BBU_putstr("Cache-D Test: ERROR, Cycle counter is not running\n\r");
      return MV_ERROR;
   }

   /* init the test array with know data*/
   for(i=0;i<par->cacheSize/4;i++)
      testBuf[i]=par->pattern;
   
   //CleanAllDataCache(); /* done for L2 too*/
   dcache_clean_all();
   dcache_invalidate_all();

   t1=ReadCycleCounter();

   /* all reads should miss D$ */
   checksum1=cacheD_runCache((uint32_t)(par->address+par->readDword)+0x20,
                             (uint32_t)(par->cacheSize));
   //for(i=0;i<par->cacheSize/4;i+=DWORD_IN_CACHE_LINE)
   //   checksum1+=testBuf[i+par->readDword];
         
   t2=ReadCycleCounter();   
 
   /* most of reads should hit the D$*/
   checksum2=cacheD_runCache((uint32_t)(par->address+par->readDword)+0x20,
                             (uint32_t)(par->cacheSize));
   //for(i=0;i<par->cacheSize/4;i+=DWORD_IN_CACHE_LINE)
   //   checksum2+=testBuf[i+par->readDword];
      
   t3=ReadCycleCounter();
   
   if(par->L2cacheDataTest){
      DataCacheCleanAndInvalidateWithoutL2();
      t4=ReadCycleCounter();   
      /* all reads should miss D$ and hit L2 cache*/
      checksum3=cacheD_runCache((uint32_t)(par->address+par->readDword)+0x20,
                                (uint32_t)(par->cacheSize));
      //for(i=0;i<par->cacheSize/4;i+=DWORD_IN_CACHE_LINE)
      //   checksum3+=testBuf[i+par->readDword];
      t5=ReadCycleCounter();
   }
   
   restore_irq_fiq(cpsr);
   
   /* print the results*/   
   if(silent == FULL_MODE){
      BBU_putstr("dcacheFooTest -checksum memory = ");
      BBU_puthexw(checksum1);
      BBU_putstr(", checksum L1D$= ");
      BBU_puthexw(checksum2);
   }
    // Issue an ERROR if checksum is wrong
   if(checksum1 != checksum2){
      BBU_putstr("\n\rCache-D Test: ERROR, wrong checksum!\n\r");
      return MV_ERROR;
   }
   
   if(par->L2cacheDataTest){
      if(silent == FULL_MODE){
         BBU_putstr(", checksum L2$= ");
         BBU_puthexw(checksum3);
      }
      // Issue an ERROR if checksum is wrong
      if(checksum1 != checksum2)
      {
         BBU_putstr("\n\rCache-D Test: ERROR, wrong checksum!\n\r");
         return MV_ERROR;
      }
   }
   if(silent == FULL_MODE){
      BBU_crlf();
      BBU_putstr("first loop latency = ");
      BBU_puthexw(64*(DIFF_WRAP_AROUND(t2,t1)));
      BBU_putstr(" core cycle; avr 32bit load that miss the D$ is ");
      BBU_puthexw((64*(DIFF_WRAP_AROUND(t2,t1)))/(par->cacheSize/32));
      BBU_crlf();
      
      BBU_putstr("second loop latency = ");
      BBU_puthexw(64*(DIFF_WRAP_AROUND(t3,t2)));
      BBU_putstr(" core cycle; avr 32bit load that hit D$ is ");
      BBU_puthexw((64*(DIFF_WRAP_AROUND(t3,t2))/(par->cacheSize/32)));
      BBU_crlf();   
      
      if(par->L2cacheDataTest){
         BBU_putstr("third loop latency = ");
         BBU_puthexw(64*(DIFF_WRAP_AROUND(t5,t4)));
         BBU_putstr(" core cycle; avr 32bit load "
                    "that miss D$ and hit L2$ is ");
         BBU_puthexw((64*(DIFF_WRAP_AROUND(t5,t4))/(par->cacheSize/32)));   
         BBU_crlf();
      }
   }
   /* Figure if test passed, Failed or passed with warnings - 
      Multiple all by 100 to avoid floating */
   if(par->L2cacheDataTest){
      /* For L2 test, we assume that if L2 is enabled 
         there is a ratio of ~1/2 HIT/MISS
         For now, above 1.5 is passed, between 1.5 and 1.1 is warning 
         and below 1.1 is FAIL - TBD values according to SPECS */
      result = (100*(64*(DIFF_WRAP_AROUND(t2,t1)))/(par->cacheSize/32))/  \
                    (64*(DIFF_WRAP_AROUND(t5,t4))/(par->cacheSize/32));
      pass = 150;
      fail = 110;
   }else{
      /* For L1 test, we assume that if L1 is enabled 
         there is a ratio of ~1/8 HIT/MISS
         For now, above 7.0 is passed, between 7.0 and 3.0 is warning 
         and below 3.0 is FAIL - TBD values according to SPECS */
      result = (100*(64*(DIFF_WRAP_AROUND(t2,t1)))/(par->cacheSize/32))/  \
                    (64*(DIFF_WRAP_AROUND(t3,t2))/(par->cacheSize/32));
      pass = 700;
      fail = 300;
   }
   
   if(result < pass){
      if(result > fail){
         if(silent == FULL_MODE)
            BBU_putstr("Cache-D Test: WARNING, miss:hit ratio is low.\n\r");
      }else{
         BBU_putstr("Cache-D Test: ERROR, miss==hit - "
                    "Cache is not working!\n\r");
         return 1;
      }
   }
   
   if(silent == FULL_MODE)
      BBU_putstr("Cache-D Test: PASSED.\n\r");
      
   return 0;   
}
