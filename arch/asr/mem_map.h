#ifndef _H_MEM_MAP_H_
#define _H_MEM_MAP_H_

#include "predefines.h"

extern uint32_t vm_ncnb_buffer_start;
extern uint32_t vm_cb_buffer_start;
extern uint32_t vm_cb_buffer_len;

#define DDR_START_ADDR          (BU_REG_READ(0xC0100200)&0xFF800000)
#define DDR_START_ADDR          (0x0)
#define vm_ncnb_buffer_len      (0x100000*16)

#define SECTION_1_MB            (0x100000)
/******************************************************************************
         DDR layout
         0x0000_0000 - 0x0010_0000          OBM
         0x0010_0000 - 0x00F0_0000          CR5

         0x00F0_0000 - 0x0100_0000          CA7 & CR5 shared buffer
         0x0100_0000 - 0x0800_0000          CA7
******************************************************************************/

#define CR5_MALLOC_CB_LEN       (0x400000)
#define CR5_MALLOC_NCNB_LEN     (0x100000)

#define CA7_MALLOC_CB_LEN       (0x600000)
#define CA7_MALLOC_NCNB_LEN     (0x200000)

#define CA7_MALLOC_BUF_LEN      (CA7_MALLOC_CB_LEN+CA7_MALLOC_NCNB_LEN)

#define OBM_AREA_ST             (DDR_START_ADDR)
#define OBM_AREA_LEN            (0x100000)
#define OBM_AREA_NUM            (OBM_AREA_LEN/SECTION_1_MB)

#ifndef CR5_AREA_ST
#define CR5_AREA_ST             (OBM_AREA_ST+OBM_AREA_LEN)
#endif

#ifndef CA7_AREA_ST
#define CA7_AREA_ST             0x1000000
#endif

#define CORE_SHARE_LEN          (0x100000)
#define CORE_SHARE_ST           (0x3F00000)
#define CORE_SHARE_NUM          (CORE_SHARE_LEN/SECTION_1_MB)

#define CR5_AREA_LEN            (CORE_SHARE_ST-CR5_AREA_ST)
#define CR5_AREA_NUM            (CR5_AREA_LEN/SECTION_1_MB)

#if   defined(CONFIG_CA7)
#define CODE_AREA_ST             CA7_AREA_ST
#elif defined(CONFIG_CR5)
#define CODE_AREA_ST             CR5_AREA_ST
#else
#define CODE_AREA_ST
#endif

#define CODE_AREA_LEN           (SMC_CS0_ST - CODE_AREA_ST)
#define CODE_AREA_NUM           (CODE_AREA_LEN/SECTION_1_MB)

/* 0x8000_0000 ~ 0x9000_0000 */
#define SMC_CS0_ST              (0x80000000)
#define SMC_CS0_LEN             (0x10000000)
#define SMC_CS0_NUM             (SMC_CS0_LEN/SECTION_1_MB)

/* 0x9000_0000 ~ 0xA000_0000 */
#define SMC_CS1_ST              (0x90000000)
#define SMC_CS1_LEN             (0x10000000)
#define SMC_CS1_NUM             (SMC_CS1_LEN/SECTION_1_MB)

/* 0xA000_0000 ~ 0xB000_0000 */
#define SMC_CS2_ST              (0xA0000000)
#define SMC_CS2_LEN             (0x10000000)
#define SMC_CS2_NUM             (SMC_CS2_LEN/SECTION_1_MB)

/* 0xB000_0000 ~ 0xC000_0000 */
#define RESERVED1_ST            (0xB0000000)
#define RESERVED1_LEN           (0x10000000)
#define RESERVED1_NUM           (RESERVED1_LEN/SECTION_1_MB)

/* 0xC000_0000 ~ 0xC800_0000 */
#define AHB_PER_ST              (0xC0000000)
#define AHB_PER_LEN             (0x8000000)
#define AHB_PER_NUM             (AHB_PER_LEN/SECTION_1_MB)

/* 0xC800_0000 ~ 0xD100_0000 */
#define RESERVED2_ST            (0xC8000000)
#define RESERVED2_LEN           (0xD1000000-0xC8000000)
#define RESERVED2_NUM           (RESERVED2_LEN/SECTION_1_MB)

/* 0xD100_0000 ~ 0xD1D0_0000 */
#define SQU_RES34_ST             (0xD1000000)
#define SQU_RES34_LEN            (0xD1D00000-0xD1000000)
#define SQU_RES34_NUM            (SQU_RES34_LEN/SECTION_1_MB)


#define SCU_SL2_ST               (0xD1D00000)
#define SCU_SL2_LEN              (0xD1E00000 - 0xD1D00000)
#define SCU_SL2_NUM              (SCU_SL2_LEN/SECTION_1_MB)

/* 0xD1E0_0000 ~ 0xD200_0000 */
#define GB_L2_ST                 (0xD1E00000)
#define GB_L2_LEN                (0xD2000000-0xD1E00000)
#define GB_L2_NUM                (GB_L2_LEN/SECTION_1_MB)

/* 0xD200_0000 ~ 0xD428_5000: AXI peripherals(AXI1, S2) */
/* 0xD428_5000 ~ 0xD42A_0000: Reserved */
/* 0xD42A_0000 ~ 0xD42A_0C00: SQU reg/DMA(AXI, S1, S4) */
/* 0xD42A_0C00 ~ 0xD42A_0D00: HS-SSP(AXI1, S1, S4) */
/* 0xD42A_0D00 ~ 0xF000_0000: Reserved */
#define AXI_PER_ST               (0xD2000000)
#define AXI_PER_LEN              (0xF0000000-0xD2000000)
#define AXI_PER_NUM              (AXI_PER_LEN/SECTION_1_MB)

/* 0xF000_0000 ~ 0xF040_0000: GPB/DPB peripheral(AXI1, S3) */
#define GPB_DPB_ST               (0xF0000000)
#define GPB_DPB_LEN              (0xF0400000-0xF0000000)
#define GPB_DPB_NUM              (GPB_DPB_LEN/SECTION_1_MB)

/* 0xF040_0000 ~ 0xFFD0_0000 */
#define RESERVED5_ST             (0xF0400000)
#define RESERVED5_LEN            (0xFFD00000-0xF0400000)
#define RESERVED5_NUM            (RESERVED5_LEN/SECTION_1_MB)

/* 0xFFD0_0000 ~ 0xFFE0_0000 */
#define SYS_MMR_ST               (0xFFD00000)
#define SYS_MMR_LEN              (0xFFE00000-0xFFD00000)
#define SYS_MMR_NUM              (SYS_MMR_LEN/SECTION_1_MB)

/* 0xFFE0_0000 ~ 0xFFFF_FFFF */
#define ROM_ST                   (0xFFE00000)
#define ROM_LEN                  (0x200000)
#define ROM_NUM                  (ROM_LEN/SECTION_1_MB)

#endif
