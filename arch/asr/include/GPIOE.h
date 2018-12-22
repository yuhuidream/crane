#ifndef __GPIOE_H__
#define __GPIOE_H__


typedef struct
{
    __IO uint32_t RER0;
    __IO uint32_t RER1;
    __IO uint32_t RER2;
    __IO uint32_t RER3;
    __IO uint32_t RER4;
    __IO uint32_t RER5;



}GPIOE_TypeDef;


#define    GPIOE_BASE      0xD4019800


#define    GPIOE  (( GPIOE_TypeDef *) GPIOE_BASE )





#endif
