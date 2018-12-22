#ifndef __APBTCU_H__
#define __APBTCU_H__

typedef struct
{
    __IO uint32_t           TCU_FIFO_INSTR;
    __IO uint32_t           TCU_NUM_INST;
    __IO uint32_t           TCU_STATUS;
    __IO uint32_t           TCU_EXCEPTION_ENABLE;
    __IO uint32_t           TCU_CONTROL;
    __IO uint32_t           TCU_NEARLY_FULL_LEVEL;
    __IO uint32_t           TCU_NEARLY_EMPTY_LEVEL;
    __IO uint32_t           TCU_WRITE_IDX;
    __IO uint32_t           TCU_READ_IDX;
    __IO uint32_t           TCU_PRESCALE;
    __IO uint32_t           TCU_DELTA_TIMER;
    __IO uint32_t           RESERVED0;
    __IO uint32_t           TCU_EVENT_LOG;
    __IO uint32_t           RECERVED1[3];
    __IO uint32_t           TCU_CLEAR_GPOA;
    __IO uint32_t           TCU_SET_GPOA;
    __IO uint32_t           TCU_CLEAR_GPOA;
    __IO uint32_t           TCU_SET_GPOB;//0x004C
    __IO uint32_t           RECERVED2[(0x400-0x004C)/4-1];
    __IO uint32_t           INSTRUCTIONRAM;//0X400
  


}APBTCU_TypeDef;






#define    APBTCU_BASE       0xD4030000


#define    APBTCU  (( APBTCU_TypeDef *)APBTCU_BASE )

//TCU Status Register
#define   TCU_STATUS_SHORTWAIT_FAIL                       BIT_6
#define   TCU_STATUS_CONCURRENCY_LIMIT                       BIT_5
#define   TCU_STATUS_UNDERFLOW                       BIT_4
#define   TCU_STATUS_UNDERFLOW_WARN                       BIT_3
#define   TCU_STATUS_OVERFLOW                       BIT_2
#define   TCU_STATUS_NEARLY_EMPTY                       BIT_1
#define   TCU_STATUS_NEARLYFULL                       BIT_0


//TCU Control Register
#define   TCU_CONTROL_LOGENABLE                          BIT_3
#define   TCU_CONTROL_FASTSTEP                          BIT_2
#define   TCU_CONTROL_FLUSH                          BIT_1
#define   TCU_CONTROL_RESET                          BIT_0






#endif
