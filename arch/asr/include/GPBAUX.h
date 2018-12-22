#ifndef __GPBAUX_H__
#define __GPBAUX_H__

typedef struct
{
    __IO uint32_t    GPB_MON_MUX;
    __IO uint32_t    C2DM;
    __IO uint32_t    DMAC2_EOCINH_WR;
    __IO uint32_t    GPB_CONTROL;
    __IO uint32_t    GPB_AFE_CONTROL;
    __IO uint32_t    WB_AFE_SMP_CLK_CFG;




} GPBAUX_TypeDef;




#define    GPBAUX_BASE       0xF0240000


#define     GPBAUX  ((  GPBAUX_TypeDef *) GPBAUX_BASE )



#endif
