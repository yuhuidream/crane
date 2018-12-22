/*----------------------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
----------------------------------------------------------------------------*/

/*****************************************************************************
 * Filename: cacheI.c
 *
 * DESCRIPTION:
 *  Cache Instruction bbu-test - main file
 ****************************************************************************/

/*----------- External include files ---------------------------------------*/
#include <string.h>
/*----------- Local include files ------------------------------------------*/
#include "predefines.h"
#include "utils.h"
#include "cacheI.h"
#include "cache_ops.h"    //Same API as in RTOS 
#include "core_tests.h"
#include "perf_counter.h"

uint32_t* casheI_codeGenerate(uint32_t tstNo, uint32_t* areaAddr);

static uint32_t* memReserve(uint32_t* reqStartAddr, uint32_t sizeBytes);
static void  memRestore(void);
static void  reportResultText(char* txt);
static void  reportResult(uint32_t tstNo,
                          s_cacheI_dataIn*  dataIn,
                          s_cacheI_dataOut* dataOut );

extern void cacheI_runAsmWrapper(uint32_t* cntrR0, 
                                 uint32_t* accessSlow, 
                                 uint32_t* accessFast, 
                                 uint32_t* runAddr);




/****************************************************************************/
/*****  CORE-LIB          ***************************************************/
/****************************************************************************/

/*****  CORE-LIB END ********************************************************/


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
uint8_t casheI(uint32_t repeat, uint8_t silent)
{
   uint8_t result = 0;
   
   if(repeat == 0)
   {
      repeat = 1;
      silent = FULL_MODE;
   }
   while(repeat--)
   {
      result |= casheI_tstNo(CASHE_I__SHORT_32WORDS, silent);
      result |= casheI_tstNo(CASHE_I__L1ONLY, silent);
      result |= casheI_tstNo(CASHE_I__L1L2,   silent);
      //result |= casheI_tstNo(CASHE_I__ITCM, silent);
   }
   
   return result;
}

/* ........................................................................ */
int casheI_tstNo (uint32_t tstNo, uint8_t silent)
{
   uint32_t*  areaAddr;
   uint32_t*  runAddr;
   uint32_t   cntrR0 = 0;
   uint32_t   accessSlow = 0;
   uint32_t   accessFast = 0;
   uint32_t   cycleCntr;
   int result=0, pass=0, fail=0;
   s_cacheI_dataOut dataOut;

   s_cacheI_dataIn* dataIn = &cacheI_dataIn[tstNo];

   StartCycleCounter();
   
   // Check Valid configuration before test
   cycleCntr = ReadCycleCounter();
   if(cycleCntr == 0)
   {
      reportResultText("Cache-I Test: ERROR, "
                       "Cycle counter is not running\n\r");
      return MV_ERROR;
   }

   if(!is_i_cache_enabled()){
      reportResultText("Cache-I Test: L1 cache is disabled\n\r");
      return MV_ERROR;
   }
   areaAddr = memReserve(dataIn->startAddr, dataIn->sizeBytes);
   
   runAddr = casheI_codeGenerate(tstNo, areaAddr);

   //Clean and Invalidate L2 Cache for L2 test
//   if(tstNo == CASHE_I__L1L2)
//   {
//      l1_dcache_clean_all();
//      l2_cache_invalidate_all();
//      l1_dcache_clean_invalidate_all();
//   }
   
//   /* Just needed for cortex-a9 */
//   l1_dcache_clean_all();
//   l2_cache_invalidate_all();
//   l1_dcache_clean_invalidate_all();
//   
   dcache_clean_invalidate_all();

   reportResultText("Cache-I Test: started\n\r");
   cacheI_runAsmWrapper(&cntrR0, &accessSlow, &accessFast, runAddr);
   /* accessSlow is the core cycles spent when code are in memory
      accessFast is the core cycles spend when code are mostly in cache
      cacheI_runAsmWrapper will run the code generate by casheI_codeGenerate
      twice, firstly the codes are all in memory, secondly the codes are 
      mostly cached after the first running
      cntrR0 = 2*patternSize, patternSize is in casheI_codeGenerate
   */
   //sprintf(buf,"Cache-I: size=%-6d, ++cntr=%-6d, access slow/fast=%4d/%d\n\r",
   //               dataIn->sizeBytes, cntrR0,    accessSlow, accessFast);
   //reportResultText(buf);

   if(cycleCntr == ReadCycleCounter()){
      if(silent == FULL_MODE){
         reportResultText("Cache-I Test: WARNING is not incremented, "
                          "result may be invalid\n\r");
      }
   }
      
   memRestore();
   dataOut.cntrR0 = cntrR0;
   dataOut.accessSlow = accessSlow;
   dataOut.accessFast = accessFast;
   
   if(silent == FULL_MODE)
      reportResult(tstNo, dataIn, &dataOut);
      
   /* Figure if test passed, Failed or passed with warnings - 
      Multiple all by 100 to avoid floating */
   result = (100*accessSlow)/accessFast;
   if(tstNo == CASHE_I__L1L2){
      /* For L2 test, we assume that if L2 is enabled 
         there is a ratio of ~1/2 HIT/MISS
         For now, above 1.5 is passed, between 1.5 and 1.1 is warning 
         and below 1.1 is FAIL - TBD values according to SPECS */
      pass = 150;
      fail = 110;
   }else if(tstNo == CASHE_I__L1ONLY){
      /* For L1 test, we assume that if L1 is enabled 
         there is a ratio of ~1/8 HIT/MISS
         For now, above 7.0 is passed, between 7.0 and 3.0 is warning 
         and below 3.0 is FAIL - TBD values according to SPECS 
         Check in Spec - In crtx-A9 each miss brings more lines to the cache,
         so seccond round is 4 times better*/
      pass = 200;
      fail = 100;
   }

   if(result < pass){
      if(result > fail){
         if(silent == FULL_MODE)
            reportResultText("Cache-I Test: WARNING, miss:hit"
                             " ratio is low.\n\r");
      }else{
         reportResultText("Cache-I Test: ERROR, miss==hit - "
                          "Cache is not working!\n\r");
         return MV_ERROR;
      }
   }
   
   if(silent == FULL_MODE)
      reportResultText("Cache-I Test: PASSED.\n\r");

   return MV_OK;   
}

/* ........................................................................ */
uint32_t* memReserve(uint32_t* reqStartAddr, uint32_t sizeBytes)
{
   //uint32_t reserveSize = sizeBytes+2*32;
   (void)sizeBytes; //avoid warning
   return(reqStartAddr);
}

/* ........................................................................ */
void  memRestore(void)
{
}

/* ........................................................................ */
void reportResultText(char* txt)
{
   BBU_putstr(txt);
}

/* ........................................................................ */
void reportResult (uint32_t tstNo,
                   s_cacheI_dataIn*  dataIn,
                   s_cacheI_dataOut* dataOut )
{
   (void)dataIn; //avoid warning
   BBU_putstr("Cache-I Test: tstNo=");
   BBU_puthexw(tstNo);
   BBU_putstr(", ++cntr=");
   BBU_puthexw(dataOut->cntrR0);
   BBU_putstr(", access slow/fast=");
   BBU_puthexw(dataOut->accessSlow);
   BBU_putstr("/");
   BBU_puthexw(dataOut->accessFast);
   BBU_crlf();
   return;
}


/*****************************************************************************
******  cacheI_runAsmWrapper()  code GENERATORs     **************************
******************************************************************************
*/

/*****************************************************************************
void cacheI_runAsmWrapper(uint32_t* cntrR0, 
                          uint32_t* accessSlow, 
                          uint32_t* accessFast, 
                          uint32_t* runAddr)
     cacheI_runCache(uint32_t cntrR0, 
                     uint32_t accessSlow, 
                     uint32_t accessFast, 
                     uint32_t runAddr)
------------------------------------------------------------------------------
cacheI_runCache:
    8 cmd_words - preparation for loop: "loops", 
      CacheInvalidate, ReadCycleCounter
cacheI_runCache_loop:
    2 cmd_words - auxiliary for loop-beginning
   (size-2-4) PATTERN_words
    4 cmd_words - auxiliary for loop-exit/end
cacheI_runCache_exit:
     bx LR
cacheI_runCache_end
*****************************************************************************/
extern void    cacheI_runCache_crtx(void);
extern uint32_t  cacheI_runCache_loop_crtx;
extern uint32_t  cacheI_runCache_loop_exit_crtx;
extern uint32_t  cacheI_runCache_end_crtx;

/* ........................................................................ */
uint32_t* codeGenerateCache(uint32_t* areaAddr, s_cacheI_dataIn* dataIn)
{
   uint32_t  *pSrc;
   uint32_t  *pDst;
   int     i, patternSize;

   pSrc = (uint32_t*)cacheI_runCache_crtx;

   pDst = areaAddr;

   while(pSrc < &cacheI_runCache_loop_crtx)
      *pDst++ = *pSrc++;

   *areaAddr = 0xE3A08000 + dataIn->loops;  // mov r8, #2  (or #1)

   *pDst++ = *pSrc++;     // mov  r1, r2   ; keep "previous" loop result
   *pDst++ = *pSrc++;     // sub  r8, #1   ; ++loop

   patternSize = dataIn->sizeBytes / sizeof(uint32_t) - 2 - 4;
   //BTW: the expected  dataIn->cntrR0 = patternSize * dataIn->loops

   for(i=0; i < patternSize; i++)
      *pDst++ = dataIn->codePattern;

   pSrc = (uint32_t*)&cacheI_runCache_loop_exit_crtx;

   *pDst++ = *pSrc++;     // ReadCycleCounter
   *pDst++ = *pSrc++;     // loop condition test
   *pDst++ = *pSrc++;     // loop condition exec
   *pDst++ = *pSrc++;     /* nop to prevent next-command 
                             Cache-loading on first loop */
   *pDst++ = *pSrc++;     // bx LR   to return

   return(areaAddr);
}

/* ........................................................................ */
uint32_t* codeGenerateL1only(uint32_t* areaAddr)
{
   s_cacheI_dataIn*  pIn = &cacheI_dataIn[CASHE_I__L1ONLY];
   uint32_t*           runAddr = areaAddr;
   uint32_t i;

   for(i=0; i< (pIn->sizeBytes + 2*(32/sizeof(uint32_t))); i++)
      areaAddr[i] = pIn->codePattern;

   //Cache-I Invalidate just before areaAddr+32bytes

   //Make loop
   runAddr = areaAddr + 32/sizeof(uint32_t) - 2;
   return(runAddr);
}

/* ........................................................................ */
uint32_t* codeGenerateL1L2(uint32_t* areaAddr)
{
   uint32_t* runAddr = areaAddr;
   return(runAddr);
}

/* ........................................................................ */
uint32_t* casheI_codeGenerate(uint32_t tstNo, uint32_t* areaAddr)
{
   s_cacheI_dataIn* dataIn = &cacheI_dataIn[tstNo];
   uint32_t* runAddr = NULL;
   switch(tstNo)
   {
      case CASHE_I__SHORT_32WORDS:
      case CASHE_I__L1ONLY:
      case CASHE_I__L1L2:
         runAddr = codeGenerateCache(areaAddr, dataIn);
         break;
      default:
         break;
   }
   return(runAddr);
}
