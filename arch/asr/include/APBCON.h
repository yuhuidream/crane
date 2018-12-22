#ifndef __APBCON_H__
#define __APBCON_H__

typedef struct
{

    __IO uint32_t    TCTS;
    __IO uint32_t    THALT;
    __IO uint32_t    RESERVED0;
    __IO uint32_t    GBS;
    __IO uint32_t    APB_GAMPMCR;
    __IO uint32_t    TCER;
    __IO uint32_t    ACER;
    __IO uint32_t    UCER;
    __IO uint32_t    USCER1;
    __IO uint32_t    USCER2;
    __IO uint32_t    ICER;
    __IO uint32_t    ECCER;
    __IO uint32_t    TICER;
    __IO uint32_t    GCER;
    __IO uint32_t    AICER;
    __IO uint32_t    SCCR;




}APBCON_TypeDef;






#define    APBCON_BASE        0xD403B000


#define    APBCON  (( APBCON_TypeDef *)APBCON_BASE )



//TCU Clock Trigger Control Register   //Hasn't field    
#define   APB_GAMPMCR_                          
#define   APB_GAMPMCR_                          
#define   APB_GAMPMCR_                          
#define   APB_GAMPMCR_                          
#define   APB_GAMPMCR_                          
#define   APB_GAMPMCR_                          
#define   APB_GAMPMCR_                          
#define   APB_GAMPMCR_                          
#define   APB_GAMPMCR_                          





//TCU Clock Control Register
#define  TCER_SW_RESET                              BIT_2
#define  TCER_CLOCK_ENABLE                          BIT_1


//AIRQ Clock Control Register
#define  ACER_SW_RESET                              BIT_2
#define  ACER_CLOCK_ENABLE                          BIT_1


//UART Clock Control Register
#define  UCER_SW_RESET                              BIT_2
#define  UCER_CLOCK_ENABLE                          BIT_1
#define  UCER_BUS_CLOCK_ENABLE                      BIT_0


//USIM1 Clock Control Register
#define  USCER1_SW_RESET                              BIT_2
#define  USCER1_CLOCK_ENABLE                          BIT_1
#define  USCER1_BUS_CLOCK_ENABLE                      BIT_0

//USIM2 Clock Control Register
#define  USCER2_SW_RESET                              BIT_2
#define  USCER2_CLOCK_ENABLE                          BIT_1
#define  USCER2_BUS_CLOCK_ENABLE                      BIT_0

//TWSI Clock Control Register
#define  ICER_SW_RESET                              BIT_2
#define  ICER_CLOCK_ENABLE                          BIT_1
#define  ICER_BUS_CLOCK_ENABLE                      BIT_0


//ECCER Clock Control Register
#define  ECCER_SW_RESET                              BIT_2
#define  ECCER_CLOCK_ENABLE                          BIT_1

//APB Clock Control Register
#define  TICER_SW_RESET                              BIT_2
#define  TICER_CLOCK_ENABLE                          BIT_1
#define  TICER_BUS_CLOCK_ENABLE                      BIT_0

//GCER Clock Control Register
#define  GCER_SW_RESET                              BIT_2
#define  GCER_CLOCK_ENABLE                          BIT_1
#define  GCER_BUS_CLOCK_ENABLE                      BIT_0


//AICER Clock Control Register
#define  AICER_SW_RESET                              BIT_2
#define  AICER_CLOCK_ENABLE                          BIT_1



//SCCR Clock Control Register
#define  SCCR_SW_RESET                              BIT_2
#define  SCCR_CLOCK_ENABLE                          BIT_1






#endif






