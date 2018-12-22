
/*
 *  Below is for Nevo UART
 */
//#include "nevo.h"
#include "predefines.h"
#include "UART.h"
#include "APBCLK.h"
#include "interrupt.h"
#include "bbu.h"
#include "uart_dec.h"
#include "PMR.h"
#include "gic_dec.h"
#include "ascii.h"

extern int GetCPUID(void);
#define DBG_UART_IRQ IRQ_MMP_UART2
#define UARTCLK     14745600


uint32_t uart_baudrate = BAUDRATE_115200;

void setup_ffuart(int baudrate)
{
    uint32_t  br_div;
    __IO uint32_t reg;
    int tmp;
    int uart_clk;
    tmp = baudrate;
	
    //enable uart 1 clock 
    APBClock->UART0_CLK_RST = UART0_CLK_RST_FNCLKSEL_MSK;			//Reset DBG_UART clock
    APBClock->UART0_CLK_RST = UART0_CLK_RST_FNCLK | UART0_CLK_RST_APBCLK | BIT_4;	//Enable DBG_UART clock

    if(platform_is_haps())
        uart_clk = 13000000;
    else
        uart_clk = UARTCLK;

#ifdef CONFIG_NATO
    br_div = 7;
#else
    br_div = uart_clk / 16 / baudrate;
#endif
    if(platform_is_zebu() || platform_is_z1()){
        br_div = 1;
    
        reg = BU_REG_READ(0xd4051024);
        reg |= 0x100;
        BU_REG_WRITE(0xd4051024, reg);
    
        reg = 0;
        while (reg++ < 0x400);
    
        BU_REG_WRITE(0xd4015000, 0x3);
        reg = BU_REG_READ(0xd4017004);
        reg |= 0x100;
        BU_REG_WRITE(0xd4017004, reg);
    }
    //set up pin-mux is already done in systeminit.
    
    // uart_init_apbc_clk(uart_num); //This is for TTC series, not necessary for PTK series

    // according to DM vol3, 9.4.7 programming baud-Rate Generator

    /* Reset: Disbale interrupt. */
    DBG_UART->LCR = 0;

    DBG_UART->IER = 0;

    /* Clear input and output FIFO. */
    DBG_UART->FCR = 0;

    /* Fifo control register. */
//    DBG_UART->FCR = FCR_TRFIFOE | (1 << 3);

    /* Clear receive buffer RBR. */
    reg = DBG_UART->LSR;
    while(reg & LSR_DR)
    {
        reg = DBG_UART->RBR;
        reg = DBG_UART->LSR;
    }

    /* Clear modem status register. */
    reg = DBG_UART->MSR;

    DBG_UART->IER &= ~IER_UUE;

    /* Fifo control register. */
    DBG_UART->FCR = FCR_TRFIFOE;

    /* Setup baudrate. Aways 8bit character. */
    DBG_UART->LCR = LCR_DLAB | LCR_WLS10_MSK;
    DBG_UART->DLH = (br_div>>8) & 0xff;
    tmp = DBG_UART->DLH;
    DBG_UART->DLL = br_div & DLL_DLL_MSK;
    /* UART : regular functional */
    DBG_UART->LCR = LCR_WLS10_MSK;

    reg = DBG_UART->LCR;
    reg &= ~LCR_DLAB;
    DBG_UART->LCR = reg;

    if(platform_is_zebu() || platform_is_z1())
        BU_REG_WRITE(0xd4017010, 0x3);

    DBG_UART->IER |= IER_UUE;
}
extern  void cli_uart_handler(void);
void setup_ffuart_interrupt(void)
{
#ifndef CONFIG_NANO_NO_INT
    INT_Enable(DBG_UART_IRQ, IRQ_ROUTE, 15);
    //ISR_Connect(DBG_UART_IRQ, cli_uart_handler, NULL);
#endif    

    /* Interrupt trigger level =32 bytes & reset recieve fifo & tx+rx fifo enable */
    DBG_UART->FCR = (FCR_ITL_MSK | FCR_TRFIFOE);

    /* Receiver Data Available and timeout Interrupt Enable */  
    DBG_UART->IER |= (IER_RAVIE | IER_RTOIE);

    /* Enable the uart intr to ICU */
    DBG_UART->MCR |= MCR_OUT2;

}
