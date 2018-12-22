#ifndef __ASPMIRQ_H__
#define __ASPMIRQ_H__

typedef struct
{

    __IO uint32_t INT_CLR[48];//Offset: 0x0000-0x00BC
    __IO uint32_t RESERVED0[(0x00F8-0x00BC)/4-1];
    __IO uint32_t EDGE_LEVEL_LO24;
    __IO uint32_t EDGE_LEVEL_HI24;




}ASPMIRQ_Typedef;



#define    ASPMIRQ_BASE       0xFFA03800
#define    ASPMIRQ  ((  ASPMIRQ_TypeDef *) ASPMIRQ_BASE )



#endif
