#ifndef __LCD_H__
#define __LCD_H__

typedef struct{
    __IO uint32_t FIFO_COMMAND;     //0x804/904
    __IO uint32_t FIFO_DATA;        //0x808/908
    __IO uint32_t FIFO_CONTROL;     //0x80C/90C
    __IO uint32_t WR_PARAMS;        //0x810/910
    __IO uint32_t RD_PARAMS;        //0x814/914
    __IO uint32_t RESERVE0[2];      //0x818/918~0x81C/91C
    __IO uint32_t YUV2RGB_FORMAT;   //0x920
    __IO uint32_t YUV2RGB_RESET;    //0x924
    __IO uint32_t RESERVE1[2];      //0x928~0x92C
    __IO uint32_t SPECIAL_FORMAT;   //0x930
    __IO uint32_t RESERVE2[11];     //0x934~0x95C
    __IO uint32_t STATUS;           //0x860/960
    __IO uint32_t INT_MASK;         //0x864/964
    __IO uint32_t RESERVE3[39];     //0x868~0x900/968~A00
}LCD_CHANNEL;

typedef struct
{
    __IO uint32_t RESERVE0[503];    //0~0x7D8
    __IO uint32_t INT_PREQ_SW;      //0x7DC
    __IO uint32_t RD_INT_MASK;      //0x7E0
    __IO uint32_t RD_INT_STAT;      //0x7E4
    __IO uint32_t CS_SWAP;          //0x7E8
    __IO uint32_t CS2_RD_DATA;      //0x7EC
    __IO uint32_t CS1_RD_DATA;      //0x7F0
    __IO uint32_t CS0_RD_DATA;      //0x7F4
    __IO uint32_t READ_OPER;        //0x7F8
    __IO uint32_t DEVICE_SELECT;    //0x7FC
    __IO uint32_t RESERVE1;         //0x800
    LCD_CHANNEL LCD_CHANNEL[2];
}LCD_TypeDef;

#define    LCD_BASE      0xD4013000
#define    LCD  (( LCD_TypeDef *) LCD_BASE )

#endif
