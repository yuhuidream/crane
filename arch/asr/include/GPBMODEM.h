#ifndef __GPBMODEM_H__
#define __GPBMODEM_H__

typedef struct
{
       __IO uint32_t    MON_CTRL;
       __IO uint32_t    VIT_CLK_SELECT;
       __IO uint32_t    EQU_CLK_SELECT;
       __IO uint32_t    E_CIP_CLK_SELECT;
       __IO uint32_t    DSSP0_CLK_SELECT;
       __IO uint32_t    DSSP1_CLK_SELECT;
       __IO uint32_t    DSSP2_CLK_SELECT;
       __IO uint32_t    DSSP3_CLK_SELECT;
       __IO uint32_t    GSSP_CLK_CTRL;
       __IO uint32_t    TIMERS_CLK_CTRL;
       __IO uint32_t    MIRQ_CLK_CTRL;
       __IO uint32_t    HSL_CLK_CTRL;//0X2C
       __IO uint32_t    RESERVED0;
       __IO uint32_t    WAKE_UP_TCU_CTL;//0X34
       __IO uint32_t    RESERVED1[(0x50-0x34)/4-1];
       __IO uint32_t    GMCGC;
       __IO uint32_t    GMCGC_II;
       __IO uint32_t    RESERVED2[(0x50-0x34)/4-1];
       __IO uint32_t    BACKWARD_COMP_MODE;
 


} GPBMODEM_TypeDef;




#define    GPBMODEM_BASE      0xF0204000

#define     GPBMODEM  ((  GPBMODEM_TypeDef *) GPBMODEM_BASE )







#endif







