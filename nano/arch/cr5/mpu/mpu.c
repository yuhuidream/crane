#include "predefines.h"
#include "mpu.h"

/***********************************************************************************
  TEX[2:0]   C   B    Description                   Memory type        Page Shareable
 ***********************************************************************************
     000     0   0    Strongly-ordered               Strongly-ordered   Shareable
                 1    Shareable Device               Device             Shareable
             1   0    Outer& Inner WT, no WA         Normal             S bit
                 1    Outer& Inner WB, no WA         Normal             S bit
  ---------------------------------------------------------------------------------
     001     0   0    Outer & Inner Non-cacheable    Normal             S bit
                 1    Reserved                       -                  -
             1   0    IMPLEMENTATION DEFINED
                 1    Outer& Inner WB, WA            Normal             S bit
  ---------------------------------------------------------------------------------
     010     0   0    Non-shareable Device           Device            Non-shareable
                 1    Reserved                       -                  -
             1   x    Reserved                       -                  -
  ---------------------------------------------------------------------------------
     011     x   x    Reserved                       -                  -
  ---------------------------------------------------------------------------------
     1BB     A   A    Cacheable memory:              Normal             S bit
                      AA = Inner attributec
                      BB = Outer attribute
   --------------------------------------------------------------------------------
   Note: WT, Write Through    WB, Write Back     WA, Write Allocate
************************************************************************************/

MpuRegion regionNezha2[] = 
{
/* id  start address  size          AP    XN  S  TEX   CB */
  { 0,  0x00000000,   Region_4G,    0x0,  0,  0,  0,   0x0 },  /* Backgroud */
  { 1,  0x00000000,   Region_128M,  0x3,  0,  0,  1,   0x3 },  /* DDR, outer&inner write back & write allocate, non-shared */
  { 2,  0x00000000,   Region_32K,   0x3,  0,  0,  1,   0x3 },  /* ATCM, outer&inner write back & write allocate, non-shared */
  { 3,  0xB0020000,   Region_32K,   0x3,  0,  0,  1,   0x3 },  /* BTCM, outer&inner write back & write allocate, non-shared */
  { 4,  0xC0000000,   Region_128M,  0x3,  0,  0,  0,   0x1 },  /* AXI peripherals, shareable device */
  { 5,  0xD1000000,   Region_64K,   0x3,  0,  0,  1,   0x3 },  /* SQU1 */
  { 6,  0xD1100000,   Region_64K,   0x3,  0,  0,  1,   0x3 },  /* SQU2 */
  { 7,  0xD2000000,   Region_32M,   0x3,  0,  0,  0,   0x1 },  /* SMC alias 0xD2000000 ~ 0xD4000000*/
  { 8,  0xD4000000,   Region_2M,    0x3,  0,  0,  0,   0x1 },  /* AXI peripherals, 0xD4000000 ~ 0xD4200000 */
  { 9,  0xD4200000,   Region_512K,  0x3,  0,  0,  0,   0x1 },  /* AXI peripherals, 0xD4200000 ~ 0xD4280000 */
  {10,  0xD4280000,   Region_16K,   0x3,  0,  0,  0,   0x1 },  /* AXI peripherals, 0xD4280000 ~ 0xD4284000 */
  {11,  0xD4284000,   Region_4K,    0x3,  0,  0,  0,   0x1 },  /* AXI peripherals, 0xD4284000 ~ 0xD4285000 */
  {12,  0xD1DF0000,   Region_64K,   0x3,  0,  0,  0,   0x1 },  /* ARM_R5 L2 */
  {13,  0xD1000000,   Region_64K,   0x3,  0,  0,  0,   0x0 },  /* core shared buffer */
  {14,  0xD0237000,   Region_4K,    0x3,  0,  0,  0,   0x1 },  /* core shared buffer */
};

void CR5_mpuSetRegion(MpuRegion *region)
{
   uint32_t val;
   CR5_MpuRegionNumberReg_Write(region->id);

   CR5_MpuRegionBaseAddressReg_Write((region->startAddress)&MPU_ADDR_MASK);

   val = (region->size <<MPU_SIZE_BASE) | MPU_REGION_ENA;
   CR5_MpuRegionSizeEnableReg_Write(val);

   val = ((region->CB) << MPU_CB_BASE )   | \
         ((region->S)  << MPU_S_BASE  )   | \
         ((region->TEX)<< MPU_TEX_BASE)   | \
         ((region->AP) << MPU_AP_BASE )   | \
         ((region->XN) << MPU_XN_BASE );
   CR5_MpuRegionAccessCtrlReg_Write(val);
}

void CR5_mpuSubregionEnable(int region, int subregion)
{
	uint32_t val;
	val = 1 << (subregion+MPU_SRD_BASE);
	CR5_MpuRegionNumberReg_Write(region);    /* select the region */

	val = CR5_MpuRegionSizeEnableReg_Read();
	val &= ~(1 << (subregion+MPU_SRD_BASE)); /* Clear to 0 to enable */
	CR5_MpuRegionSizeEnableReg_Write(val);
}

void CR5_mpuSubregionDisable(int region, int subregion)
{
	uint32_t val;
	val = 1 << (subregion+MPU_SRD_BASE);
	CR5_MpuRegionNumberReg_Write(region);    /* select the region */

	val = CR5_MpuRegionSizeEnableReg_Read();
	val |= (1 << (subregion+MPU_SRD_BASE)); /* Set to 1 to disable */
	CR5_MpuRegionSizeEnableReg_Write(val);
}

void nezha2_set_mpu(void)
{
   int regionNum, i;
   regionNum = sizeof(regionNezha2)/sizeof(MpuRegion);
   for(i = 0; i < regionNum; i++){
      CR5_mpuSetRegion(&regionNezha2[i]);
   }
   CR5_MpuEnable();
   enable_caches();
}

