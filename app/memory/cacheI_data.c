#include "predefines.h"
#include "cacheI.h"
#include "core_tests.h"

//uint32_t cacheI_testArea[(16*1024+128)/4];
//#define CACHE_AREA     &cacheI_testArea[0]

/*Note: Need to adjust according to Emei memory map */
#define CACHE_AREA_L1     (uint32_t*)0x10000000
#define CACHE_AREA_L2     (uint32_t*)0x10000000

/*Note: Need to adjust according to Emei memory map */
s_cacheI_dataIn  cacheI_dataIn[CASHE_I__NUM_TESTS] =
{
  //                                 "add r0,#1"           cntrR0=((sizeBytes/4)-2-4)*loops
  // startAddr          sizeBytes      codePattern  loops  {cntrR0  accessSlow  accessFast spare}
  //0: CASHE_I__SHORT_32WORDS: 
    {CACHE_AREA_L1,           32*4,    0xE2800001,    2,   { 52,        0,         0,       0} },

  //1: CASHE_I__L1ONLY:
    {CACHE_AREA_L1,          16*1024,  0xE2800001,    2,   {  0 ,       0,         0,       0} },

  //2: CASHE_I__L1L2:
    {CACHE_AREA_L2,          64*1024,  0xE2800001,    2,   {  0 ,       0,         0,       0} },

  // startAddr          sizeBytes   codePattern  loops  {cntrR0  accessSlow  accessFast }
  //3: CASHE_I__SRAM_APP:
    {(uint32_t*)0x5C008000,  0x10000,  0xE2800001,    1,   {  0 ,       0,         0,       0} },

  //4: CASHE_I__SRAM_COM: tbd
    {(uint32_t*)0xd1010000, 256*1024,  0xE2800001,    1,   {  0 ,       0,         0,       0} }, //TBD
  //5: CASHE_I__SQU:
    {(uint32_t*)0xd1010000,  0x10000,  0xE2800001,    1,   {  0 ,       0,         0,       0} },
  //6: CASHE_I__ITCM:
    {(uint32_t*)0,            0x8000,  0xE2800001,    1,   {  0 ,       0,         0,       0} },
  //7: CASHE_I__DTCM:
    {(uint32_t*)0xD2000000,   0x8000,  0xE2800001,    1,   {  0 ,       0,         0,       0} }

};/*END*/