#ifndef _H_MEM_MAP_H_
#define _H_MEM_MAP_H_


#define SECTION_1_MB            (0x100000)

#define VECTOR_START            (0x00000000)
/*   graphic buffer  32M   */
#define GCU_BUF_ST              (0x02000000)
#define GCU_BUF_LEN             (0x02000000)
#define GCU_BUF_NUM             (GCU_BUF_LEN/SECTION_1_MB)

#define CODE_AREA_ST            (0x08000000)
#define CODE_AREA_LEN           (SMC_CS0_ST - CODE_AREA_ST)
#define CODE_AREA_NUM           (CODE_AREA_LEN/SECTION_1_MB)


/* BBU malloc buffer 20M */
#define MALLOC_BUF_LEN          (0x01400000)

/*  LCD frame buffer 8M = 1.5m * 5 */
#define LCD_BUF_LEN             (0x00c00000)

/* ISP buffer 64M */
#ifdef CONFIG_FVTS_FS
#define ISP_BUF_LEN             (0x10000000)
#elif CONFIG_FVTS
#define ISP_BUF_LEN             (0x02000000)	//32M.
#else
#define ISP_BUF_LEN             (0x0d100000)
#endif

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

#define MMU_CA9_SMP0_TABLE_START    (CA9_0_TBL_ST)
#define MMU_CA9_SMP1_TABLE_START    (CA9_1_TBL_ST)
#define MMU_PJMM_TABLE_START        (0x04D00000 - 0x10000)

#define MMU_CA9_SMP0_L2TABLE_START   
#define MMU_CA9_SMP1_L2TABLE_START


#define BBU_TMARK_BUFFER_START TMARK_BUF_ST
#define BBU_TMARK_BUFFER_LEN  TMARK_BUF_LEN

#endif
