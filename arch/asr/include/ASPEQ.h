#ifndef __ASPEQ_H__
#define __ASPEQ_H__

typedef struct
{
   
    __IO uint32_t  Hx_SLOT1_REG[0x18/4+1];//Offset: 0x00-0x18
    __IO uint32_t  Hx_SQR_REG_S1[(0x34-0x1C)/4+1];
    __IO uint32_t  Hx_SLOT2_REG[0x18/4+1];
    __IO uint32_t  Hx_SQR_REG_S2[0x18/4+1];
    __IO uint32_t  EQU_MU0_REG;
    __IO uint32_t  EQU_MU6_REG;
    __IO uint32_t  HD_S1_REG[0x14/4+1];
    __IO uint32_t  HD_S2_REG[0x14/4+1];
    __IO uint32_t  SWRES_REG;
    __IO uint32_t  CONT_REG;
    __IO uint32_t  STATUS_REG;
    __IO uint32_t  MODE0_REG;
    __IO uint32_t  MODE1_SLOT1_REG;
    __IO uint32_t  MODE1_SLOT2_REG;
    __IO uint32_t  DBG_DATA_REG;
    __IO uint32_t  RESUME_REG;
    __IO uint32_t  REQ_REG;
    __IO uint32_t  CURR_SYM_REG;
    __IO uint32_t  NV_SLOT1_REG0;
    __IO uint32_t  NV_SLOT1_REG1;
    __IO uint32_t  NV_SLOT2_REG0;
    __IO uint32_t  NV_SLOT2_REG1;
    __IO uint32_t  RESERVED0[(0x100-0xDC)/4-1];
    __IO uint32_t  LLR_REG_S1[0xAC/4+1];
    __IO uint32_t  RESERVED1[(0x200-0x1AC)/4-1];
    __IO uint32_t  LLR_REG_S2[0xAC/4+1];




} ASPEQ_TypeDef;




#define    ASPEQ_BASE       0xFFA02000


#define     ASPEQ  ((  ASPEQ_TypeDef *) ASPEQ_BASE )




#endif








