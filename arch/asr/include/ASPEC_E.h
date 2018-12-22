#ifndef __ASPEC_E_H__
#define __ASPEC_E_H__

typedef struct
{
       __IO uint32_t   RESERVED0;
       __IO uint32_t   SW_RESET;
       __IO uint32_t   ALG_THR;
       __IO uint32_t   ASPEC;
       __IO uint32_t   DIRECTION;
       __IO uint32_t   COUNT;
       __IO uint32_t   INPUT;
       __IO uint32_t   CIP_KC[4];
       __IO uint32_t   RUNUP_CYCLES;
       __IO uint32_t   STIR_CYCLES;
       __IO uint32_t   CIP_INIT;
       __IO uint32_t   CIP_START;
       __IO uint32_t   CIP_DSP_DONE;

} ASPEC_E_TypeDef;




#define    ASPEC_E_BASE       0xFFA00000


#define     ASPEC_E  ((  ASPEC_E_TypeDef *) ASPEC_E_BASE )


#endif
