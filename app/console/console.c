/*****************************************************************************
* Product: BBU
* 
* Purpose: Console AO
* 
* Copyright (C) 2010-2011 Marvell MTBL-Hefei. All rights reserved.
*
* Author: Lianghu Xu  email: xlianghu@marvell.com
* 
*****************************************************************************/
#include "predefines.h"
#include "UART.h"
#include "qp_port.h"
#include "bsp.h"
#include "bbu.h"
#ifdef Q_SPY_EXT
    enum AppRecords {                 /* application-specific trace records */
        OUTPUT_DATA = QS_USER
    };
#endif
Q_DEFINE_THIS_FILE

#define CBUF_SIZE 8192*32    /*keep the lenght as power of two, so that the wrap is easy*/
/*..........................................................................*/
typedef struct ConsoleTag {                      /* the Console active object */
    QActive super;                                   /* derive from QActive */
    uint8_t TxBuf[CBUF_SIZE];
    uint32_t rIndex;
    uint32_t wIndex;
} Console;

//#define TX_DATA_REQ_INT
                                          /* hierarchical state machine ... */
static QState Console_initial       (Console *me, QEvent const *e);
static QState Console_buffering     (Console *me, QEvent const *e);
static QState Console_processing    (Console *me, QEvent const *e);
static QState Console_idle          (Console *me, QEvent const *e);
static QState Console_printing      (Console *me, QEvent const *e);

static void console_start_up(void);

static Console l_console;                          /* the sole instance of the Console active object */ 

/*Public-scope objects------------------------------------------------------------*/
QActive * const AO_Console = (QActive *)&l_console; /*opaque pointer to Console AO*/ 

/*..........................................................................*/
void Console_ctor(void) {
    Console *me = &l_console;
    QActive_ctor(&me->super, (QStateHandler)&Console_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState Console_initial(Console *me, QEvent const *e) {
    (void)e;               /* avoid compiler warning about unused parameter */
    QS_OBJ_DICTIONARY(&l_console);

//    QS_FUN_DICTIONARY(&QHsm_top);
    QS_FUN_DICTIONARY(&Console_initial);
    QS_FUN_DICTIONARY(&Console_buffering);
    QS_FUN_DICTIONARY(&Console_idle);
    QS_FUN_DICTIONARY(&Console_processing);
    QS_FUN_DICTIONARY(&Console_printing);

//    QS_SIG_DICTIONARY(Q_INIT_SIG,                me);                 
    QS_SIG_DICTIONARY(COM_PRINT_REQUEST_SIG,     me);
    QS_SIG_DICTIONARY(COM_TX_DATA_READY_SIG,     me);
    QS_SIG_DICTIONARY(COM_DATA_REQUEST_SIG,      me);
    me->rIndex = 0;
    me->wIndex = 0;
    
    console_start_up();

    return Q_TRAN(&Console_buffering);
}

/*..........................................................................*/
QState Console_buffering(Console *me, QEvent const *e) {
    switch (e->sig) {
        case Q_INIT_SIG: {
            return Q_TRAN(&Console_processing);
        }
        case COM_PRINT_REQUEST_SIG: {

#ifdef Q_SPY_EXT
			QF_INT_DISABLE();
            QS_BEGIN(OUTPUT_DATA,NULL)
            char temp[MAX_LINE_LEN];
            for(int i = 0; i<((ComTxEvt const *)e)->len; i++){
                temp[i] = (uint8_t)(((ComTxEvt const *)e)->ConsoleBuffer[i]);
           	}
            temp[((ComTxEvt const *)e)->len] = '\0';
            QS_STR(temp);
            QS_END() 
            QF_INT_ENABLE();
#else
			static QEvent reminderEvt =QEVT_INIT(COM_TX_DATA_READY_SIG, 0, 0 );
            for(int i = 0; i<((ComTxEvt const *)e)->len; i++){
              me->TxBuf[me->wIndex++] = ((ComTxEvt const *)e)->ConsoleBuffer[i];
              me->wIndex &= (CBUF_SIZE - 1);
              Q_ASSERT(((me->wIndex + 1)&(CBUF_SIZE - 1)) != me->rIndex); //buffer is not full
              //delay()?? here we may need to add some delay to hold the write speed VS UART rate
            }
            QACTIVE_POST((QActive *)me, &reminderEvt, me);
#endif
            return Q_HANDLED();
        }

    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Console_processing(Console *me, QEvent const *e) {
    switch (e->sig) {
        case Q_INIT_SIG: {
            return Q_TRAN(&Console_idle);
        }
    }
    return Q_SUPER(&Console_buffering);
}
/*..........................................................................*/
QState Console_idle(Console *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case COM_TX_DATA_READY_SIG: {
            return Q_TRAN(&Console_printing);
        }
    }
    return Q_SUPER(&Console_processing);
}
/*..........................................................................*/
QState Console_printing(Console *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
#ifdef CONFIG_MMT
            uint32_t length2write;
            uint32_t i;
            length2write = (me->wIndex < me->rIndex)? (CBUF_SIZE - me->rIndex + me->wIndex):(me->wIndex-me->rIndex);
            for(i=0; i<length2write; i++)
            {
                put_char(me->TxBuf[me->rIndex++]);
                me->rIndex &= (CBUF_SIZE - 1);
            }

            static QEvent reminderEvt2 =QEVT_INIT(COM_PRINT_DONE_SIG, 0, 0 );
            QACTIVE_POST((QActive *)me, &reminderEvt2, me);
#else
            //Upon entry printing state, Tx FIFO should be empty, just make sure here
            while(!(DBG_UART->LSR & LSR_TEMT)); 
            //write 64 bytes from the TxBuf into UART FIFO, if it's below 64bytes, just write up
            uint32_t byte_num;
            uint32_t length2write;
            uint32_t i;

            byte_num = (me->wIndex < me->rIndex)? (CBUF_SIZE - me->rIndex + me->wIndex):(me->wIndex-me->rIndex);

    #ifdef TX_DATA_REQ_INT
            length2write = (byte_num > 64)? 64: byte_num;
    #else
            length2write = byte_num;
    #endif
            for(i=0; i<length2write; i++)
            {
    #ifdef TX_DATA_REQ_INT
                DBG_UART->THR = me->TxBuf[me->rIndex++];
    #else
                while(!(DBG_UART->LSR & LSR_TEMT)); 
                DBG_UART->THR = me->TxBuf[me->rIndex++];
    #endif
                me->rIndex &= (CBUF_SIZE - 1);
            }
    #ifdef TX_DATA_REQ_INT
            /*Transmit Data Request Interrupt Enable */  
            DBG_UART->IER |= IER_TIE;
    #else
            static QEvent reminderEvt2 =QEVT_INIT(COM_PRINT_DONE_SIG, 0, 0 );
            QACTIVE_POST((QActive *)me, &reminderEvt2, me);
    #endif
#endif
            return Q_HANDLED();
        }
        case COM_DATA_REQUEST_SIG: {
#ifdef TX_DATA_REQ_INT
            //write 32(or left) bytes from the TxBuf into UART FIFO
            uint32_t byte_num;
            uint32_t length2write;
            uint32_t i;

            /*Transmit Data Request Interrupt disable*/
            DBG_UART->IER &= ~IER_TIE;

            byte_num = (me->wIndex < me->rIndex)? (CBUF_SIZE - me->rIndex + me->wIndex):(me->wIndex - me->rIndex);
            length2write = (byte_num > 32)? 32: byte_num;

            for(i=0; i<length2write; i++)
            {
                /*make sure Data Request Interrupt disable before write to THR register, 
                  otherwise, the interrupt may be triggered with each write and run out of console queue.
                  */
	        DBG_UART->THR = me->TxBuf[me->rIndex++];
                me->rIndex &= (CBUF_SIZE - 1);
            }
            if(byte_num > 32)
            {   
                /*Transmit Data Request Interrupt Enable */  
                DBG_UART->IER |= IER_TIE;
            }
            else
            {
                return Q_TRAN(&Console_idle);
            }
#else
            Q_ASSERT(0);  //Should never reach here
#endif
            return Q_HANDLED();

        }
#ifndef TX_DATA_REQ_INT
        case COM_PRINT_DONE_SIG:{
            return Q_TRAN(&Console_idle);
        }
#endif
    }
    return Q_SUPER(&Console_processing);
}

static int console_on = 0;

void console_start_up(void)
{
   console_on = 1;
}

int is_console_on(void)
{
   return console_on;
}

void console_shut_down(void)
{
   console_on = 0;
}
