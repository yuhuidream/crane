#ifndef __RTC_H__
#define __RTC_H__

typedef struct
{


    __IO uint32_t CNR;
    __IO uint32_t AR;
    __IO uint32_t SR;
    __IO uint32_t TR;
    __IO uint32_t PR;
    __IO uint32_t BRN[5];


}RTC_TypeDef;


#define    RTC_BASE      0xD4010000


#define    RTC  (( RTC_TypeDef *) RTC_BASE )

//Bitfield Constants Register RTC_SR
#define SR_HZE_MASK                              0x00000008
#define SR_HZE_SHIFT                             3
#define SR_ALE_MASK                              0x00000004
#define SR_ALE_SHIFT                             2
#define SR_HZ_MASK                               0x00000002
#define SR_HZ_SHIFT                              1
#define SR_AL_MASK                               0x00000001
#define SR_AL_SHIFT                              0


//Bitfield Constants Register RTC_TR
#define TR_LCK_MASK                              0x80000000
#define TR_LCK_SHIFT                             31
#define TR_D9_D0_MASK                            0x03FF0000
#define TR_D9_D0_SHIFT                           16
#define TR_C150_C0_MASK                          0x0000FFFF
#define TR_C150_C0_SHIFT                         0

//Bitfield Constants Register RTC_PR
#define PR_SW_ALARM_MASK                         0x00000001
#define PR_SW_ALARM_SHIFT                        0

#endif
