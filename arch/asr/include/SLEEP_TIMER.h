#ifndef __SLEEP_TIMER_H__
#define __SLEEP_TIMER_H__

typedef struct
{

    __IO uint32_t SCTR;
    __IO uint32_t FCTR;
    __IO uint32_t VSTR;
    __IO uint32_t PSTR;
    __IO uint32_t ESLR;
    __IO uint32_t VSCR;
    __IO uint32_t SLCR;





}SLEEP_TIMER;


#define    SLEEP_TIMER_BASE      0xFFA60000

#define     SLEEP_TIMER  ((  SLEEP_TIMER_TypeDef *) SLEEP_TIMER_BASE )






#endif
