#include "predefines.h"
#include "mpu.h"
#include "mem_map.h"
#include "cp15.h"
#include "shared_mem.h"
#include "cache_ops.h"

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
  { 1,  0x00000000,   Region_128M,  0x3,  0,  0,  1,   0x3 },  /* DDR for CR5, outer&inner write back & write allocate, non-shared */
//  { 2,  0x00000000,   Region_32K,   0x3,  0,  0,  1,   0x3 },  /* ATCM, outer&inner write back & write allocate, non-shared */
  { 2,  0xB0020000,   Region_64K,   0x3,  0,  0,  1,   0x3 },  /* BTCM, outer&inner write back & write allocate, non-shared */
  { 3,  0xC0000000,   Region_128M,  0x3,  0,  0,  0,   0x1 },  /* AXI peripherals, shareable device */
  { 4,  0xD1DF0000,   Region_64K,   0x3,  1,  0,  0,   0x1 },  /* ARM_R5 L2 */
  { 5,  0xD2000000,   Region_32M,   0x3,  1,  0,  0,   0x1 },  /* SMC alias 0xD2000000 ~ 0xD4000000*/
  { 6,  0xD4000000,   Region_4M,    0x3,  1,  0,  0,   0x1 },
  { 7,  0xD0000000,   Region_32M,   0x3,  1,  0,  0,   0x0 },
  { 8, 0xF9000000,   Region_32K,   0x3,  0,  0,  0,   0x3 },
#ifdef CONFIG_SQU
  { 9,  0xD1000000,   Region_4M,   0x3,  0,  0,  1,   0x3 },  /* SQU1 */
  { 10,  0xD12FFC00,   Region_1K,    0x3,  0,  0,  0,   0x0 },  /* last 1k of SRAM is used for shared memory between CR5 and X1, configure it as strongly ordered */
#else
  { 9,  0xD1000000,   Region_64K,   0x3,  0,  0,  1,   0x3 },  /* SQU1 */
  { 10,  0xD100FC00,   Region_1K,    0x3,  0,  0,  0,   0x0 },  /* last 1k of SRAM is used for shared memory between CR5 and X1, configure it as strongly ordered */
#endif
  
  
//  {10,  0xD4000000,   Region_2M,    0x3,  0,  0,  0,   0x1 },  /* AXI peripherals, 0xD4000000 ~ 0xD4200000 */
//  {11,  0xD4200000,   Region_512K,  0x3,  0,  0,  0,   0x1 },  /* AXI peripherals, 0xD4200000 ~ 0xD4280000 */
//  {12,  0xD4280000,   Region_16K,   0x3,  0,  0,  0,   0x1 },  /* AXI peripherals, 0xD4280000 ~ 0xD4284000 */
//  {13,  0xD4284000,   Region_4K,    0x3,  0,  0,  0,   0x1 },  /* AXI peripherals, 0xD4284000 ~ 0xD4285000 */
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
extern uint32_t ncnb_buffer_start;
extern uint32_t ncnb_buffer_end;
extern uint32_t kern_image_size;

void nezha2_set_mpu(void)
{
   int regionNum, i;
   regionNum = sizeof(regionNezha2)/sizeof(MpuRegion);
   for(i = 0; i < regionNum; i++){
      CR5_mpuSetRegion(&regionNezha2[i]);
   }

   uint32_t addr = ADDR_ALIGN((kern_image_size+CR5_AREA_ST),0x100000) + CR5_MALLOC_CB_LEN;

   MpuRegion ncnbRegion =
   /* id  start address  size          AP    XN  S  TEX   CB */ 
   {i,  addr,   Region_1M,           0x3,  0,  0,  1,   0x0 };
   /* Configure the NCNB buffer for bbu_malloc(1 MB) */
   CR5_mpuSetRegion(&ncnbRegion);

   /* Configure the NCNB buffer in the image area which is used for global variables */
   i++;
   ncnbRegion.id = i;
   ncnbRegion.startAddress = ncnb_buffer_start;
   ncnbRegion.size =Region_4K;
   CR5_mpuSetRegion(&ncnbRegion);

   i++;
   ncnbRegion.id = i;
   ncnbRegion.startAddress = shared_vm_ncnb_start;
   if(shared_vm_ncnb_len == 0x1000000)
      ncnbRegion.size =Region_16M;
   else{
      bbu_printf("VM NCNB buffer size is not supported\n\r");
   }
   //bbu_printf("VM NCNB start: 0x%x\n\r", shared_vm_ncnb_start);
   CR5_mpuSetRegion(&ncnbRegion);


   i++;
   MpuRegion naRegion =
   /* id  start address  size          AP    XN  S  TEX   CB */ 
   {i,  0x1000000,   Region_16M,      0x0,  0,  0,  0,   0x0 };
   CR5_mpuSetRegion(&naRegion);

   if(shared_vm_cb_start < (32<<20)){
      /* Each sub region is 2MB, has 8 sub-regions
      */
      for(uint32_t j = 0; j < 8; j++){
         if( ((j*2+16+2)<<20) < shared_vm_cb_start ){
            CR5_mpuSubregionEnable(i, j);
         }
         else{
            CR5_mpuSubregionDisable(i, j);
         }
      }
   }else{
      // 16~32M is non-accessible for CR5
   }

   i++;
   ncnbRegion.id = i;
   ncnbRegion.startAddress = CORE_SHARE_ST;
   ncnbRegion.size =Region_1M;
   CR5_mpuSetRegion(&ncnbRegion);

   CR5_MpuEnable();
#ifdef CONFIG_CACHE
   enable_caches();
   bbu_printf("CR5 L1 caches are enabled\n\r");
#endif
}

void mpu_config_dump(void)
{
   uint32_t base, size, access;
   bbu_printf("\n\rRegion  BASE   SIZE   ACCESS\n\r");
   for(int i = 0; i < 16; i++){
      CR5_MpuRegionNumberReg_Write(i);
      base = CR5_MpuRegionBaseAddressReg_Read();
      size = CR5_MpuRegionSizeEnableReg_Read();
      access = CR5_MpuRegionAccessCtrlReg_Read();
      bbu_printf("%d  0x%08X 0x%08X 0x%08X\n\r",i,base,size,access);
   }
}
