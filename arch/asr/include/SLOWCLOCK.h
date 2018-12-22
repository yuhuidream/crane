#ifndef __SLOWCLOCK_H__
#define __SLOWCLOCK_H__

typedef struct
{
    __IO uint32_t      STAT_CTRL;
    __IO uint32_t      WAKEUP_REL;
    __IO uint32_t      WAKEUP_ABS;
    __IO uint32_t      TCU_WAKEUP_REL;
    __IO uint32_t      TCU_WAKEUP_ABS;
    __IO uint32_t      SCNT;
    __IO uint32_t      SSNAP;
    __IO uint32_t      FSNAP;
    __IO uint32_t      SCNT_INT;
    __IO uint32_t      SCNT_LOAD;
    __IO uint32_t      D2_LEFT;
    __IO uint32_t      MPMU_GSM_MVT_CTL;
  




}SLOWCLOCK_TypeDef;






#define    SLOWCLOCK_BASE       0xD4038000


#define    SLOWCLOCK  (( SLOWCLOCK_TypeDef *)SLOWCLOCK_BASE )

//Status Control Register
#define    STAT_CTRL_D2_STAT                                  BIT_4
#define    STAT_CTRL_EARLY_WAKEUP                                  BIT_3
#define    STAT_CTRL_ABSOLUTE_RELATIVE_SELECT                                  BIT_2
#define    STAT_CTRL_D2_EN                                  BIT_1
#define    STAT_CTRL_SCK_SW_RST                                  BIT_0



//MPMU_GSM_MVT_CTL Register
#define    MPMU_GSM_MVT_CTL_AIRQ_BB_WAKE_CLR                                       BIT_3
#define    MPMU_GSM_MVT_CTL_SELECT                                       BIT_2
#define    MPMU_GSM_MVT_CTL_RESET                                       BIT_1













#endif

