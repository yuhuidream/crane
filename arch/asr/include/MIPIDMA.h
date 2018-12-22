#ifndef __MIPIDMA_H__
#define __MIPIDMA_H__

typedef struct
{
    __IO uint32_t   DMA_DDADR;
    __IO uint32_t   DMA_DSADR;
    __IO uint32_t   DMA_DTADR;
    __IO uint32_t   DMA_DCMD;

}DMA_REG_TypeDef;


typedef struct
{
    __IO uint32_t   DMA_DCSRx[32]; //0x0000 -0x7c
    __IO uint32_t   RESERVED0[(0x0F0-0x07c)/4-1];
    __IO uint32_t   DMA_DINT;//0X0F0
    __IO uint32_t   RESERVED1[(0x100-0x0F0)/4-1];
    __IO uint32_t   DMA_DRCMRx[32];//0X0100-0X17C
    __IO uint32_t   DMA_DLPR;//0X180
    __IO uint32_t   RESERVED3[(0x200-0x180)/4-1];
    DMA_REG_TypeDef DAM_ADR_CMD[32];   //0X200


}MIPIDMA_TypeDef;


#define    MIPIDMA_BASE       0xD4206000
#define    AUDIO_MIPIDMA_BASE 0xC0FFE000

#define    MIPIDMA        (( MIPIDMA_TypeDef *) MIPIDMA_BASE )
#define    AUDIO_MIPIDMA  (( MIPIDMA_TypeDef *) AUDIO_MIPIDMA_BASE )


//Bitfield Constants Register DMA_DCSRx
#define MIPIDMA_DMA_DCSRX_RUN_MASK                              0x80000000
#define MIPIDMA_DMA_DCSRX_RUN_SHIFT                             31
#define MIPIDMA_DMA_DCSRX_NODESCFETCH_MASK                      0x40000000
#define MIPIDMA_DMA_DCSRX_NODESCFETCH_SHIFT                     30
#define MIPIDMA_DMA_DCSRX_STOPIRQEN_MASK                        0x20000000
#define MIPIDMA_DMA_DCSRX_STOPIRQEN_SHIFT                       29
#define MIPIDMA_DMA_DCSRX_RASIRQEN_MASK                         0x00800000
#define MIPIDMA_DMA_DCSRX_RASIRQEN_SHIFT                        23
#define MIPIDMA_DMA_DCSRX_MASKRUN_MASK                          0x00400000
#define MIPIDMA_DMA_DCSRX_MASKRUN_SHIFT                         22
#define MIPIDMA_DMA_DCSRX_REQPEND_MASK                          0x00000100
#define MIPIDMA_DMA_DCSRX_REQPEND_SHIFT                         8
#define MIPIDMA_DMA_DCSRX_RASINTR_MASK                          0x00000010
#define MIPIDMA_DMA_DCSRX_RASINTR_SHIFT                         4
#define MIPIDMA_DMA_DCSRX_STOPINTR_MASK                         0x00000008
#define MIPIDMA_DMA_DCSRX_STOPINTR_SHIFT                        3
#define MIPIDMA_DMA_DCSRX_ENDINTR_MASK                          0x00000004
#define MIPIDMA_DMA_DCSRX_ENDINTR_SHIFT                         2
#define MIPIDMA_DMA_DCSRX_STARTINTR_MASK                        0x00000002
#define MIPIDMA_DMA_DCSRX_STARTINTR_SHIFT                       1
#define MIPIDMA_DMA_DCSRX_BUSERRINTR_MASK                       0x00000001
#define MIPIDMA_DMA_DCSRX_BUSERRINTR_SHIFT                      0


//Bitfield Constants Register DMA_DRCMRx
#define MIPIDMA_DMA_DRCMRX_MAPVLD_MASK                           0x00000080
#define MIPIDMA_DMA_DRCMRX_MAPVLD_SHIFT                          7
#define MIPIDMA_DMA_DRCMRX_REQNUM_MASK                           0x0000001F
#define MIPIDMA_DMA_DRCMRX_REQNUM_SHIFT                          0

//Bitfield Constants Register DMA_DLPR
#define MIPIDMA_DMA_DLPR_CHAN_SET3_MASK                        0x0000F000
#define MIPIDMA_DMA_DLPR_CHAN_SET3_SHIFT                       12
#define MIPIDMA_DMA_DLPR_CHAN_SET2_MASK                        0x00000F00
#define MIPIDMA_DMA_DLPR_CHAN_SET2_SHIFT                       8
#define MIPIDMA_DMA_DLPR_CHAN_SET1_MASK                        0x000000F0
#define MIPIDMA_DMA_DLPR_CHAN_SET1_SHIFT                       4
#define MIPIDMA_DMA_DLPR_CHAN_SET0_MASK                        0x0000000F
#define MIPIDMA_DMA_DLPR_CHAN_SET0_SHIFT                       0



//Bitfield Constants Register DMA_DDADR[]
#define MIPIDMA_DMA_DDADR_DESCRIPTOR_ADDRESS_MASK               0xFFFFFFF0
#define MIPIDMA_DMA_DDADR_DESCRIPTOR_ADDRESS_SHIFT              4
#define MIPIDMA_DMA_DDADR_STOP_MASK                             0x00000001
#define MIPIDMA_DMA_DDADR_STOP_SHIFT                            0



//Bitfield Constants Register DMA_DSADR[]
#define MIPIDMA_DMA_DSADR_SRCADDR_MASK                          0xFFFFFFF8
#define MIPIDMA_DMA_DSADR_SRCADDR_SHIFT                         3


//Bitfield Constants Register DMA_DTADR[]
#define MIPIDMA_DMA_DTADR_TRGADDR_MASK                          0xFFFFFFF8
#define MIPIDMA_DMA_DTADR_TRGADDR_SHIFT                         3



//Bitfield Constants Register DMA_DCMD[]
#define MIPIDMA_DMA_DCMD_INCSRCADDR_MASK                       0x80000000
#define MIPIDMA_DMA_DCMD_INCSRCADDR_SHIFT                      31
#define MIPIDMA_DMA_DCMD_INCTRGADDR_MASK                       0x40000000
#define MIPIDMA_DMA_DCMD_INCTRGADDR_SHIFT                      30
#define MIPIDMA_DMA_DCMD_FLOWSRC_MASK                          0x20000000
#define MIPIDMA_DMA_DCMD_FLOWSRC_SHIFT                         29
#define MIPIDMA_DMA_DCMD_FLOWTRG_MASK                          0x10000000
#define MIPIDMA_DMA_DCMD_FLOWTRG_SHIFT                         28
#define MIPIDMA_DMA_DCMD_STARTIRQEN_OR_RESERVED_MASK           0x00400000
#define MIPIDMA_DMA_DCMD_STARTIRQEN_OR_RESERVED_SHIFT          22
#define MIPIDMA_DMA_DCMD_ENDIRQEN_MASK                         0x00200000
#define MIPIDMA_DMA_DCMD_ENDIRQEN_SHIFT                        21
#define MIPIDMA_DMA_DCMD_SIZE_MASK                             0x00030000
#define MIPIDMA_DMA_DCMD_SIZE_SHIFT                            16
#define MIPIDMA_DMA_DCMD_OVERREAD_MASK                         0x00002000
#define MIPIDMA_DMA_DCMD_OVERREAD_SHIFT                        13
#define MIPIDMA_DMA_DCMD_LEN_MASK                              0x00001FFF
#define MIPIDMA_DMA_DCMD_LEN_SHIFT                             0


#endif
