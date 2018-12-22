#ifndef __APBEC_H__
#define __APBEC_H__

typedef struct
{
   __IO uint32_t       AR_CIP_OB;
   __IO uint32_t       AR_CIP_SW_RESET;
   __IO uint32_t       AR_CIP_ALG_THR;
   __IO uint32_t       AR_CIP_ASPEC;
   __IO uint32_t       AR_CIP_DIRECTION;
   __IO uint32_t       AR_CIP_COUNT;
   __IO uint32_t       AR_CIP_INPUT;
   __IO uint32_t       AR_CIP_KC[4];
   __IO uint32_t       AR_CIP_RUNUP_CYCLES;
   __IO uint32_t       AR_CIP_STIR_CYCLES;
   __IO uint32_t       AR_CIP_INIT;
   __IO uint32_t       AR_CIP_START;
   __IO uint32_t       AR_CIP_DSP_DONE;//0X3C
   __IO uint32_t       RESERVED0[(0x50-0x3C)/4-1];
   __IO uint32_t       AR_CIP_DMA_ACK_BYP_CTRL;//0X50
   __IO uint32_t       RESERVED1[(0x100-0x050)/4-1];
   __IO uint32_t       AR_CIP_OB_FOR_DMA;//0X100



}APBEC_TypeDef;

#define  APBEC_BASE  0xD4034000

#define  APBEC  (( APBEC_TypeDef *) APBEC_BASE )

//Algorithm Threshold Register
#define  CIP_ALG_THR_THRESHOLD_MSK                     SHIFT7(0x7)
#define  CIP_ALG_THR_THRESHOLD_BASE                    7
#define  CIP_ALG_THR_ALGORITHM_MSK                       SHIFT0(0x7f)
#define  CIP_ALG_THR_ALGORITHM_BASE                       0


//E-Cipher ASPEC Enable Register
#define  AR_CIP_ASPEC_INVERT_ON                      BIT_1
#define  AR_CIP_ASPEC_ASPEC_EN                       BIT_0


//E-Cipher Length Direction Register
#define  AR_CIP_DIRECTION_DIRECTION                   BIT_20
#define  AR_CIP_DIRECTION_LENGTH_MSK                   SHIFT0(0xfffff)
#define  AR_CIP_DIRECTION_LENGTH_BASE                  0



//E-Cipher DMA_ACK, Bypass Control Register
#define  AR_CIP_DMA_ACK_BYP_CTRL_DMA_ACK_BYPASS
#define  AR_CIP_DMA_ACK_BYP_CTRL_DMA_DMA_READ_ACCESSES_MSK
#define  AR_CIP_DMA_ACK_BYP_CTRL_DMA_DMA_READ_ACCESSES_BASE





#endif

