#ifndef __PWM_H__
#define __PWM_H__

typedef struct
{



    __IO uint32_t CRx;
    __IO uint32_t DCR;
    __IO uint32_t PCR;



}PWM_TypeDef;






#define    PWM1_BASE      0xD401A000
#define    PWM1  (( PWM_TypeDef *) PWM1_BASE )


#define    PWM2_BASE      0xD401A400
#define    PWM2  (( PWM_TypeDef *) PWM2_BASE )

#define    PWM3_BASE      0xD401A800
#define    PWM3  (( PWM_TypeDef *) PWM3_BASE )


#define    PWM4_BASE      0xD401AC00
#define    PWM4  (( PWM_TypeDef *) PWM4_BASE )



//Bitfield Constants Register PWM_CRx
#define CRX_SD_MASK                               0x00000040
#define CRX_SD_SHIFT                              6
#define CRX_PRESCALE_MASK                         0x0000003F
#define CRX_PRESCALE_SHIFT                        0


//Bitfield Constants Register PWM_PCR
#define PCR_PV_MASK                               0x000003FF
#define PCR_PV_SHIFT                              0
#endif
