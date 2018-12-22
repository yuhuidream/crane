#ifndef __GIC_H__
#define __GIC_H__

/*
 * Interrupt Distributor access structure
 *
 */
typedef struct
{
    __IO uint32_t DCR;                   /* 0x000 Interrupt Distributor Control Register (R/W) */
    __IO uint32_t ICTR;                  /* 0x004 Interrupt Controller Type Register (RO) */
    __IO uint32_t IIDR;                  /* 0x008 Distributor Implementer Identification Register */
    __IO uint32_t RESERVED1[29];         /* 0x00C - 0x07C Reserved */
    __IO uint32_t IGR[5];                /* 0x080 Interrupt Group Registers, only 5 available in Helan */
    __IO uint32_t IGR_RESERVED[32-5];
    __IO uint32_t ISER[5];               /* 0x100 Interrupt Set/Clear Enable Registers (R/W), only 5 available in Helan */
    __IO uint32_t ISER_RESERVED[32-5];
    __IO uint32_t ICER[5];               /* only 5 available in Helan */
    __IO uint32_t ICER_RESERVED[32-5];
    __IO uint32_t ISPR[5];               /* 0x200 Interrupt Set/Clear Pending Registers (R/W), only 5 available in Helan */
    __IO uint32_t ISPR_RESERVED[32-5];
    __IO uint32_t ICPR[5];               /* only 5 available in Helan */
    __IO uint32_t ICPR_RESERVED[32-5];
    __IO uint32_t ISAR[5];               /* 0x300 Interrupt Set-Active Registers (R/W), only 5 available in Helan */
    __IO uint32_t ISAR_RESERVED[32-5];
    __IO uint32_t ICAR[5];               /* 0x380 Interrupt Clear-Active Registers (R/W), only 5 available in Helan  */
    __IO uint32_t ICAR_RESERVED2[32-5];
    __IO uint32_t IPR[40];               /* 0x400 Interrupt Priority Registers (R/W) */
    __IO uint32_t IPR_RESERVED[215];
    __IO uint32_t RESERVED3;             /* 0x7FC */
    __IO uint32_t IPTR[40];              /* 0x800  Interrupt CPU Target Registers (R/W) */
    __IO uint32_t IPTR_RESERVED[215];
    __IO uint32_t RESERVED4;             /* 0xBFC */
    __IO uint32_t ICFR[10];              /* 0xC00 Interrupt Configuration Registers (R/W), only 10 available in Helan */
    __IO uint32_t ICFR_RESERVED[64-10];
    __IO uint32_t PPISR;                 /* 0xD00, Private Peripheral Interrupt Status Register */
    __IO uint32_t SPISR[15];             /* 0xD04~0xD3C, Shared Peripheral Interrupt Status Registers */
    __IO uint32_t RESERVED5[112];        /* 0xD40 Reserved */
    __IO uint32_t SGIR;                  /* 0xF00 Software Interrupt Register (RO) */
    __IO uint32_t RESERVED6[3];
    __IO uint32_t SGICPR[4];             /* 0xF10-0xF1C, SGI Clear-Pending Registers */
    __IO uint32_t SGISPR[4];             /* 0xF20-0xF2C, SGI Set-Pending Registers */
    __IO uint32_t RESERVED7[40];
    __IO uint32_t PIDR4;                 /* 0xFD0, Peripheral ID4 Register */
    __IO uint32_t PIDR5;                 /* 0xFD4, Peripheral ID5 Register */
    __IO uint32_t PIDR6;                 /* 0xFD8, Peripheral ID6 Register */
    __IO uint32_t PIDR7;                 /* 0xFDC, Peripheral ID7 Register */
    __IO uint32_t PIDR0;                 /* 0xFE0, Peripheral ID0 Register */
    __IO uint32_t PIDR1;                 /* 0xFE4, Peripheral ID1 Register */
    __IO uint32_t PIDR2;                 /* 0xFE8, Peripheral ID2 Register */
    __IO uint32_t PIDR3;                 /* 0xFEC, Peripheral ID3 Register */
    __IO uint32_t CIDR0;                 /* 0xFF0, Component ID0 Register */
    __IO uint32_t CIDR1;                 /* 0xFF4, Component ID1 Register */
    __IO uint32_t CIDR2;                 /* 0xFF8, Component ID2 Register */
    __IO uint32_t CIDR3;                 /* 0xFFC, Component ID3 Register */
}GIC_ICD_TypeDef;

#define GIC_ICD_BASE      (0xD1DF9000)
#define GIC_ICD     (( GIC_ICD_TypeDef *) GIC_ICD_BASE )


/*
 * CPU Interface access structure
 *
 * These registers are aliased for each CPU
 */
typedef struct
{
    __IO uint32_t ICR;                /* 0x00 Control Register (R/W) */
    __IO uint32_t PMR;                /* 0x04 Priority Mask Register (R/W) */
    __IO uint32_t BPR;                /* 0x08 Binary Point Register (R/W) */
    __IO uint32_t IAR;                /* 0x0C Interrupt Acknowledge Register (R) */
    __IO uint32_t EOIR;               /* 0x10 End of Interrupt Register (W) */
    __IO uint32_t RPR;                /* 0x14 Running Priority Register (R) */
    __IO uint32_t HPIR;               /* 0x18 Highest Pending Interrupt Register (R) */
    __IO uint32_t ABPR;               /* 0x1C Aliased Binary Point Register */
    __IO uint32_t AIAR;               /* 0x20 Aliased Interrupt Acknowledge Register(R) */
    __IO uint32_t AEOIR;              /* 0x24 Aliased End of Interrupt Register(W) */
    __IO uint32_t AHPIR;              /* 0x28 Aliased Highest Priority Pending Interrupt Register(R) */
    __IO uint32_t RESERVED0[41];
    __IO uint32_t APR[4];             /* 0xD0-0xDC Active Priority Register(R/W)*/
    __IO uint32_t NSAPR;              /* 0xE0, Non-secure Active Priority Register */
    __IO uint32_t RESERVED1[6];
    __IO uint32_t IIDR;               /* 0xFC CPU Interface Identification Register  */
    __IO uint32_t RESERVED2[960];
    __IO uint32_t DIR;                /* 0x1000 Deactivate Interrupt Register */
} GIC_ICC_TypeDef;

#define GIC_ICC_BASE      (0xD1DFA000)
#define GIC_ICC     (( GIC_ICC_TypeDef *) GIC_ICC_BASE )

#endif 
