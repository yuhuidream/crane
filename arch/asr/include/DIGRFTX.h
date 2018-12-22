#ifndef __DIGRFTX_H__
#define __DIGRFTX_H__

typedef struct
{
    __IO uint32_t    SW;
    __IO uint32_t    TCU;
    __IO uint32_t    RTU;
    __IO uint32_t    TIMER_TRIG;

}TX_PAYLOAD;

typedef struct
{
    __IO uint32_t    SW;
    __IO uint32_t    TCU;
    __IO uint32_t    RTU;
    __IO uint32_t    TIMER_TRIG;

}TX_PAYLOAD8;


typedef struct
{
    __IO uint32_t    SW;
    __IO uint32_t    TCU;
    __IO uint32_t    RTU;
    __IO uint32_t    TIMER_TRIG;

}RX_PAYLOAD;

typedef struct
{
    __IO uint32_t    SW;
    __IO uint32_t    TCU;
    __IO uint32_t    RTU;
    __IO uint32_t    TIMER_TRIG;

}RX_PAYLOAD8;


typedef struct
{
   
    __IO uint32_t    TX_SW_RESET;
    __IO uint32_t    TX_DATA_SW_RESET;
    __IO uint32_t    TX_CLOCK_RATE;
    __IO uint32_t    TX_GSM_LCT;
    __IO uint32_t    TX_LATCH_STATUS;
    __IO uint32_t    TX_RD_STATUS;
    __IO uint32_t    TX_PADS_CONFIG;
    __IO uint32_t    TX_SHUTDOWN;
    __IO uint32_t    TX_CLR_STICKY;
    __IO uint32_t    TX_PLL_SEL;//0X0024
    __IO uint32_t    RESERVED0[(0x0040-0x0024)/4-1];
    __IO uint32_t    FIFO_DMA_IF;//0X0040
    __IO uint32_t    FIFO_SW_CFG;
    __IO uint32_t    FIFO_SW_ACK;
    __IO uint32_t    RESERVED1[(0x188-0x048)/4-1];//need be re-writed
    __IO uint32_t    PRI_THD;
    __IO uint32_t    RESERVED2[(0x1C0-0x188)/4-1];
    __IO uint32_t    DIV_FIFO;
    __IO uint32_t    DIV_LCT;
    __IO uint32_t    DIV_THD;
    __IO uint32_t    RESERVED3[(0x200-0x1C8)/4-1];
    __IO uint32_t    CFG_TAS_SR;//0X200
    __IO uint32_t    CFG_TAS_WAIT;
    __IO uint32_t    CFG_FIFOS;
    __IO uint32_t    RESERVED4[(0x240-0x208)/4-1];
    __IO uint32_t    FC_CONFIG_REQ;
    __IO uint32_t    FC_FORCE_STATE;
    __IO uint32_t    FC_FORCE_SHFT_REG;
    __IO uint32_t    FC_FORCE_STATUS;
    __IO uint32_t    FC_PRI_DIV_PAD;//0X0250
    __IO uint32_t    RESERVED5[(0x400-0x0250)/4-1];
    __IO uint32_t    TX_PAYLOAD  tx_Payload[16];
    __IO uint32_t    TX_PAYLOAD8 tx_Payload8[16];
    __IO uint32_t    TAS_PRI_LCT;
    __IO uint32_t    TAS_PRI_ENTRY_CFG;//0X604
    __IO uint32_t    RESERVED6[(0x800-0x0604)/4-1];
    __IO uint32_t    RX_PAYLOAD  rx_Payload[16];
    __IO uint32_t    RX_PAYLOAD8 rx_Payload8[16];
    __IO uint32_t    TAS_DIV_LCT;
    __IO uint32_t    TAS_DIV_ENTRY_CFG;
 
} DIGRFTX_TypeDef;




#define    DIGRFTX_BASE       0xFFAA0000

#define     DIGRFTX  ((  DIGRFTX_TypeDef *) DIGRFTX_BASE )




#endif





