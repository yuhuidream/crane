
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
#include "ascii.h"
#include "spinlock.h"
#include "smp.h"
#include "timer_dec.h"
#include "APBCON.h"

extern int GetCPUID(void);
static uint8_t  l_ffuart_fifo_irq_handler;
#ifdef Q_SPY_EXT
static uint8_t  l_ffuart_qspy_fifo_irq_handler;
#endif

#define UARTCLK 	14745600

#ifdef Q_SPY
#define QSPY_UART_IRQ   IRQ_MMP_UART_CP
#endif

uint32_t uart_baudrate = BAUDRATE_115200;

static spinlock_t core_yell_lock = {0};
void core_yell_alive(void)
{
    int core = smp_processor_id();
    spin_lock(&(core_yell_lock));
    bbu_putchar(core+48);
    bbu_putchar(' ');
    spin_unlock(&(core_yell_lock));
}

void ffuart_fifo_irq_handler()
{
    uint32_t uart_iir = DBG_UART->IIR;

    //the Rx timeout interrupt indicates there are trailing bytes, need to follow the steps in manual
    //step1: read the LSR and check for errors (this is same for all cases and highest interrupt inside UART)
    uint32_t uart_lsr= DBG_UART->LSR;
    if (uart_lsr & (LSR_FIFOE|LSR_FE|LSR_PE|LSR_OE)) {
        bbu_printf("LSR error : %x\n",uart_lsr);
    }

    if ((uart_iir & IIR_IID10_MSK)==SHIFT1(0x2)) {//If Rx FIFO/timeout interrupt 
        ComRxEvt *cre = Q_NEW(ComRxEvt,COM_RX_REQUEST_SIG); 
        if (uart_iir & IIR_TOD) { //timeout interrupt
            //step2: disable the Rx timeout interrupt
            DBG_UART->IER &= ~IER_RTOIE;
            //step3: receive data available
            for (cre->len =0; uart_lsr & LSR_DR;  ) {
                uint32_t uart_for=0,i=0;
                uart_for = DBG_UART->FOR;
                // read out the rest of characters in FIFO
                for (i=0;i<uart_for;i++,cre->len++) {
                    cre->data[cre->len] = DBG_UART->RBR;
                    //work around the issue UART always get a Null character after CR
                    if(platform_is_z1() && (cre->data[cre->len] == 0)){
                        cre->len--;
                    }
                }
                //step4:read LSR, check for errors; and go to step3 if LSR[DR] is set else go to step 5.
                uart_lsr= DBG_UART->LSR;
                if (uart_lsr & (LSR_FIFOE|LSR_FE|LSR_PE|LSR_OE)) {
                    bbu_printf("LSR error again : %x\n",uart_lsr);
                }

            }
            if(cre->data[0] == ASCII_AT)
            {
               bbu_printf("\n\rCurrent alive cores: ");
#ifdef CONFIG_SMP
               sgi_broadcast_all_but_self(GIC_INT_SW_10);
#endif
               core_yell_alive();
               mdelay(10);
               bbu_printf("\n\r"BBU_PROMPT);
               cre->super.sig = MAX_SIG;
#ifdef MULTI_UART
               (QEvent *)cre->uart_id_ = UART_AP_ID;
#endif
               QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_fifo_irq_handler);
            }
#if defined(QF_CONTEXT_CTRL) && !defined(CONFIG_QK)
            else if(cre->data[0] == ASCII_CTRL_C){
               cre->super.sig = MAX_SIG;
#ifdef MULTI_UART
               (QEvent *)cre->uart_id_ = UART_AP_ID;
#endif
               QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_fifo_irq_handler);
               int coreId = smp_processor_id();
               //Clear interrupt of UART and GIC before restore context
               DBG_UART->IER |= IER_RTOIE;
               bbu_putstr("\r\n"BBU_PROMPT, 7);
               QF_RESTORE_CONTEXT(coreId);
            }
#ifdef CONFIG_SMP
            else if(cre->data[0] == ASCII_CTRL_D){
               cre->super.sig = MAX_SIG;
#ifdef MULTI_UART
               (QEvent *)cre->uart_id_ = UART_AP_ID;
#endif
               QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_fifo_irq_handler);
               int coreId = smp_processor_id();
               //Clear interrupt of UART and GIC before restore context
               DBG_UART->IER |= IER_RTOIE;
               bbu_putstr("\r\n"BBU_PROMPT, 7);
               sgi_broadcast_all_but_self(GIC_INT_SW_13);
               QF_RESTORE_CONTEXT(coreId);
            }
#endif
#endif
            else
#ifdef MULTI_UART
            {
               (QEvent *)cre->uart_id_ = UART_AP_ID;
               QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_fifo_irq_handler);
		    }
#else
            {
               if(cre->len > 0)
                  QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_fifo_irq_handler);
               else{
                  cre->super.sig = MAX_SIG;
                  QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_fifo_irq_handler);
               }
            }
#endif
            //step5: no more data in FIFO,re-enable Receive Timeout
            DBG_UART->IER |= IER_RTOIE;
        }
        else { //If Rx FIFO threshold interrupt  
            for (cre->len =0; DBG_UART->LSR & LSR_DR; cre->len++) {
                cre->data[cre->len] = DBG_UART->RBR;
                //work around the issue UART always get a Null character after CR
                if(platform_is_z1() && (cre->data[cre->len] == 0)){
                    cre->len--;
                }
            }
#ifdef MULTI_UART
            (QEvent *)cre->uart_id_ = UART_AP_ID;
#endif
            if(cre->len > 0)
                QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_fifo_irq_handler);
            else{
                cre->super.sig = MAX_SIG;
                QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_fifo_irq_handler);
            }
            //QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_fifo_irq_handler);
        }
    }
    else if ((uart_iir & IIR_IID10_MSK)==SHIFT1(0x1)) {  //If Tx FIFO low interrupt
        //send COM_DATA_REQUEST_SIG event to console
        static QEvent const TxReqEvt = QEVT_INIT(COM_DATA_REQUEST_SIG, 0 ,0);
#ifdef MULTI_UART
        (QEvent *)cre->uart_id_ = UART_AP_ID;
#endif
        QACTIVE_POST(AO_Console, &TxReqEvt,&l_ffuart_fifo_irq_handler);
    }
    else {
    }

}

void lpm_ffuart(void)
{
    //disable clock

    //lpm pin-mux if true
}

void setup_ffuart(int baudrate)
{
    uint32_t  br_div;
    __IO uint32_t reg;
    uint32_t uart_clk;
    int tmp;
	
    APBCON->UCER |= (UCER_CLOCK_ENABLE|UCER_BUS_CLOCK_ENABLE);
#ifdef CONFIG_CPUART
    APBClock->UART1_CLK_RST = UART0_CLK_RST_FNCLKSEL_MSK;
    APBClock->UART1_CLK_RST = UART0_CLK_RST_FNCLK | UART0_CLK_RST_APBCLK | BIT_4;
#else
    APBClock->UART0_CLK_RST = UART0_CLK_RST_FNCLKSEL_MSK;
    APBClock->UART0_CLK_RST = UART0_CLK_RST_FNCLK | UART0_CLK_RST_APBCLK | BIT_4;
#endif
    if(platform_is_haps())
        uart_clk = 13000000;
    else
        uart_clk= UARTCLK;

#ifndef CONFIG_NATO
    br_div = uart_clk / 16 / baudrate;
#else
    br_div = 7;
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
    DBG_UART->FCR = FCR_TRFIFOE;

    reg = DBG_UART->RBR;

    /* Clear receive buffer RBR. */
    reg = DBG_UART->LSR;

    /* Clear modem status register. */
    reg = DBG_UART->MSR;

    /* Setup baudrate. Aways 8bit character. */
    DBG_UART->LCR = LCR_DLAB | LCR_WLS10_MSK;
    /* If both the Divisor Latch Low and Divisor Latch High Registers need to
      be changed, write the Divisor Latch High Register first, read back its 
      value, then write the Divisor Latch Low Register. 
      This guarantees internal timings/coherency. 
    */
    DBG_UART->DLH = (br_div>>8) & 0xff;
    tmp = DBG_UART->DLH;
    DBG_UART->DLL = br_div & DLL_DLL_MSK;

    /* UART : regular functional */
    DBG_UART->LCR = LCR_WLS10_MSK;

    if(platform_is_zebu() || platform_is_z1())
        BU_REG_WRITE(0xd4017010, 0x3);

    DBG_UART->IER |= IER_UUE;
    (void)tmp;
    (void)reg;
}

void setup_ffuart_interrupt(void)
{
#ifdef CONFIG_CP_ALONE
    ISR_Connect(DBG_UART_IRQ, ffuart_fifo_irq_handler, NULL);
    INT_Enable(DBG_UART_IRQ, IRQ_ROUTE, 15);
#endif
    //airq_int_enable(AIRQ_UART_INT, ffuart_fifo_irq_handler, NULL);
        /* Interrupt trigger level =32 bytes & reset recieve fifo & tx+rx fifo enable */
    DBG_UART->FCR = (FCR_ITL_MSK | FCR_TRFIFOE);

    /* Receiver Data Available and timeout Interrupt Enable */  
    DBG_UART->IER |= (IER_RAVIE | IER_RTOIE);

    /* Enable the uart intr to ICU */
    DBG_UART->MCR |= MCR_OUT2;
}
/* 
int get_char(int uart_num)                                                                                                            .
                                                                                                                                      .
This function is different in QP context, the requested AO should send a event with its AO and SIG to CLI_AO. 
CLI_AO need to handle the request event in the idle leaf state and transfer to some wait_4_char state. 
The receiving state will post self a get_char_event once the cce->len==1 in between the two CR events. 
Then wait_4_char state will post the char to who requested it and return to idle state again.               
*/

/* 
int put_char(int uart_num, int u8c) 
 
This function is defined in printer driver by using AO_Console.
*/
#ifdef Q_SPY
void setup_qspy_ffuart(int baudrate)
{
    uint32_t  br_div;
    __IO uint32_t reg;
    int tmp;
    tmp = baudrate;

    APBCON->UCER |= (UCER_CLOCK_ENABLE|UCER_BUS_CLOCK_ENABLE);

    br_div = UARTCLK / 16 / baudrate;
    QSPY_UART->LCR = 0;
    QSPY_UART->IER = 0;

    QSPY_UART->FCR = 0;
    QSPY_UART->FCR = FCR_TRFIFOE;
    reg = QSPY_UART->RBR;
    reg = QSPY_UART->LSR;
    reg = QSPY_UART->MSR;

    QSPY_UART->LCR = LCR_DLAB | LCR_WLS10_MSK;
    QSPY_UART->DLH = (br_div>>8) & 0xff;
    QSPY_UART->DLL = br_div & DLL_DLL_MSK;

    QSPY_UART->LCR = LCR_WLS10_MSK;
    QSPY_UART->IER |= IER_UUE;
}

#ifdef Q_SPY_EXT
static void ffuart_qspy_fifo_irq_handler()
{
    uint32_t uart_iir = QSPY_UART->IIR;
    uint32_t uart_lsr= QSPY_UART->LSR;
    if (uart_lsr & (LSR_FIFOE|LSR_FE|LSR_PE|LSR_OE)) {
        bbu_printf("LSR error : %x\n",uart_lsr);
    }

    if ((uart_iir & IIR_IID10_MSK)==SHIFT1(0x2)) {
        ComRxEvt *cre = Q_NEW(ComRxEvt,COM_RX_REQUEST_SIG); 
        if (uart_iir & IIR_TOD) { 
            QSPY_UART->IER &= ~IER_RTOIE;
            for (cre->len =0; uart_lsr & LSR_DR;  ) {
                uint32_t uart_for=0,i=0;
                uart_for = QSPY_UART->FOR;
                for (i=0;i<uart_for;i++,cre->len++) {
                    cre->data[cre->len] = QSPY_UART->RBR;
                }
                uart_lsr= QSPY_UART->LSR;
                if (uart_lsr & (LSR_FIFOE|LSR_FE|LSR_PE|LSR_OE)) {
                    bbu_printf("LSR error again : %x\n",uart_lsr);
                }

            }
            if(cre->data[0] == ASCII_AT)
            {
               int core = GetCPUID();
               bbu_putstr("core", 4);
               bbu_putchar(core+47);
               bbu_putstr(" alive\n\r", 8);
               cre->super.sig = MAX_SIG;
#ifdef MULTI_UART
               (QEvent *)cre->uart_id_ = UART_CP_ID;
#endif
               QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_qspy_fifo_irq_handler);
#ifdef CONFIG_SMP
               sgi_broadcast_all_but_self(GIC_INT_SW_10);
#endif
            }
#ifdef QF_CONTEXT_CTRL
            else if(cre->data[0] == ASCII_CTRL_C){
               cre->super.sig = MAX_SIG;
#ifdef MULTI_UART
               (QEvent *)cre->uart_id_ = UART_CP_ID;
#endif
               QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_qspy_fifo_irq_handler);
               int coreId = smp_processor_id();
               //Clear interrupt of UART and GIC before restore context
               DBG_UART->IER |= IER_RTOIE;
               bbu_putstr("\r\n"BBU_PROMPT, 7);
               QF_RESTORE_CONTEXT(coreId);
            }
#endif
            else{
#ifdef MULTI_UART
               (QEvent *)cre->uart_id_ = UART_CP_ID;
#endif
               QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_qspy_fifo_irq_handler);
	        }
            QSPY_UART->IER |= IER_RTOIE;
        }
        else {   
            for (cre->len =0; QSPY_UART->LSR & LSR_DR; cre->len++) {
                cre->data[cre->len] = QSPY_UART->RBR;
            }
#ifdef MULTI_UART
            (QEvent *)cre->uart_id_ = UART_CP_ID;
#endif
            QACTIVE_POST(AO_Cli,(QEvent *)cre, &l_ffuart_qspy_fifo_irq_handler);
        }
    }
    else if ((uart_iir & IIR_IID10_MSK)==SHIFT1(0x1)) {
        static QEvent TxReqEvt = QEVT_INIT(COM_DATA_REQUEST_SIG, 0 ,0);
#ifdef MULTI_UART
               (QEvent *)cre->uart_id_ = UART_CP_ID;
#endif
        QACTIVE_POST(AO_Console, &TxReqEvt,&l_ffuart_qspy_fifo_irq_handler);
    }
    else {
        bbu_printf("This interrupt is not used in current UART app!\n");
    }

}

void setup_qspy_ffuart_interrupt(void)
{
    INT_Enable(QSPY_UART_IRQ, IRQ_ROUTE, 15);
    ISR_Connect(QSPY_UART_IRQ, ffuart_qspy_fifo_irq_handler, NULL);

    QSPY_UART->FCR = (FCR_ITL_MSK | FCR_TRFIFOE);

    QSPY_UART->IER |= (IER_RAVIE | IER_RTOIE);

    QSPY_UART->MCR |= MCR_OUT2;

    QS_FUN_DICTIONARY(&l_ffuart_qspy_fifo_irq_handler);
}
#endif
#endif

