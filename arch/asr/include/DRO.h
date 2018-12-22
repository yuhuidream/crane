#ifndef __DRO_H__
#define __DRO_H__

typedef struct
{
     __IO uint32_t    CTRL;
     __IO uint32_t    threshold_high;
     __IO uint32_t    threshold_low;
     __IO uint32_t    tgl_cnt;
     __IO uint32_t    idle;
     __IO uint32_t    ring_cnt;
     __IO uint32_t    cnt_ref;
     __IO uint32_t    expt;
     __IO uint32_t    TS_CTRL;
     __IO uint32_t    TS_READ;
     __IO uint32_t    TS_CLR;
     __IO uint32_t    TS_THD;
     __IO uint32_t    TS_DUR;
     __IO uint32_t    TS_CNT;

}DRO_TypeDef;

#define  DRO_BASE  0xD4013200

#define  DRO  (( DRO_TypeDef *) DRO_BASE )

//Algorithm Threshold Register
#define  AR_CIP_ALG_THR_THRESHOLD_MSK                       SHIFT7(0x7)
#define  AR_CIP_ALG_THR_THRESHOLD_BASE                           7
#define  AR_CIP_ALG_THR_ALGORITHM_MSK                       SHIFT0(0x7f)
#define  AR_CIP_ALG_THR_ALGORITHM_BASE                           0


//E-Cipher ASPEC Enable Register
#define  AR_CIP_ASPEC_INVERT_ON                      BIT_1
#define  AR_CIP_ASPEC_ASPEC_EN                       BIT_0



//E-Cipher Length Direction Register
#define  AR_CIP_DIRECTION_DIRECTION                  BIT_20
#define  AR_CIP_DIRECTION_LENGTH_MSK                SHIFT0(0xfffff)
#define  AR_CIP_DIRECTION_LENGTH_BASE                   0

//E-Cipher DMA_ACK, Bypass Control Register
#define  AR_CIP_DMA_ACK_BYP_CTRL_DMA_ACK_BYPASS                         BIT_8
#define  AR_CIP_DMA_ACK_BYP_CTRL_DMA_READ_ACCESSES_MSK                   SHIFT0(0xff)
#define  AR_CIP_DMA_ACK_BYP_CTRL_DMA_READ_ACCESSES_BASE                 0











#endif
