#ifndef h_mpu_h
#define h_mpu_h

typedef struct MpuRegionTag{
   uint32_t id;
   uint32_t startAddress;
   uint32_t size;
   uint8_t  AP;
   uint8_t  XN;
   uint8_t  S;
   uint8_t  TEX;
   uint8_t  CB;
}MpuRegion;

/************************************************************************
  AP bit   Privileged     User
  values   permissions    permissions   Description
**************************************************************************
  b000     No access     No access    All accesses generate a permission fault
  b001     Read/write    No access    Privileged access only
  b010     Read/write    Read-only    Writes in User mode generate permission faults
  b011     Read/write    Read/write   Full access
  b100     UNP           UNP          Reserved
**************************************************************************/
#define P_NA_U_NA    0x0
#define P_RW_U_NA    0x1
#define P_RW_U_RO    0x2
#define P_WR_U_RW    0x3

#define Region_32B  (0x4L)  // (b00100)
#define Region_64B  (0x5L)  // (b00101)
#define Region_128B (0x6L)  // (b00110)
#define Region_256B (0x7L)  // (b00111)
#define Region_512B (0x8L)  // (b01000)
#define Region_1K   (0x9L)  // (b01001)
#define Region_2K   (0xAL)  // (b01010)
#define Region_4K   (0xBL)  // (b01011)
#define Region_8K   (0xCL)  // (b01100)
#define Region_16K  (0xDL)  // (b01101)
#define Region_32K  (0xEL)  // (b01110)
#define Region_64K  (0xFL)  // (b01111)
#define Region_128K (0x10L) // (b10000)
#define Region_256K (0x11L) // (b10001)
#define Region_512K (0x12L) // (b10010)
#define Region_1M   (0x13L) // (b10011)
#define Region_2M   (0x14L) // (b10100)
#define Region_4M   (0x15L) // (b10101)
#define Region_8M   (0x16L) // (b10110)
#define Region_16M  (0x17L) // (b10111)
#define Region_32M  (0x18L) // (b11000)
#define Region_64M  (0x19L) // (b11001)
#define Region_128M (0x1AL) // (b11010)
#define Region_256M (0x1BL) // (b11011)
#define Region_512M (0x1CL) // (b11100)
#define Region_1G   (0x1DL) // (b11101)
#define Region_2G   (0x1EL) // (b11110)
#define Region_4G   (0x1FL) // (b11111)

/* MPU Region Size and Enable Registers */
#define MPU_SIZE_BASE   1
#define MPU_REGION_ENA  1
#define MPU_SRD_BASE    8   /*Sub-region base*/

/* MPU Region Access Control Registers */
#define MPU_AP_BASE     8
#define MPU_ADDR_MASK  (0xFFFFFFE0)
#define MPU_CB_BASE     0
#define MPU_S_BASE      2
#define MPU_TEX_BASE    3
#define MPU_AP_BASE     8
#define MPU_XN_BASE     12

uint32_t CR5_MpuRegionSizeEnableReg_Read(void);
uint32_t CR5_MpuRegionAccessCtrlReg_Read(void);
uint32_t CR5_MpuRegionNumberReg_Read(void);
uint32_t CR5_MpuRegionBaseAddressReg_Read(void);
uint32_t CR5_MpuTypeReg_Read(void);
void CR5_MpuRegionSizeEnableReg_Write(uint32_t val);
void CR5_MpuRegionAccessCtrlReg_Write(uint32_t val);
void CR5_MpuRegionNumberReg_Write(uint32_t val);
void CR5_MpuRegionBaseAddressReg_Write(uint32_t val);
void CR5_MpuEnable(void);
void CR5_mpuSubregionEnable(int region, int subregion);
void CR5_mpuSubregionDisable(int region, int subregion);

#endif

