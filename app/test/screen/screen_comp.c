/*
 *  ate_comp.c
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */


#include "qp_port.h"
#include "bsp.h"
#include "utils.h"
#include "bbu.h"
#include "timer_dec.h"
#include "screen_comp.h"
#include "screen_test.h"


Q_DEFINE_THIS_FILE


/* hierarchical state machine ............................................. */
static QState screen_initial(SCREEN_COMP *me, QEvent const *e);
static QState screen_processing(SCREEN_COMP *me, QEvent const *e);
static QState screen_is_runing(SCREEN_COMP *me, QEvent const *e);
static QState screen_is_busy(SCREEN_COMP *me, QEvent const *e);

SCREEN_COMP l_screen_comp;


QHsm *SCREEN_COMP_ctor(void)
{
	SCREEN_COMP *me = &l_screen_comp;
		
	QEQueue_init(&me->requestQueue, me->requestQSto, Q_DIM(me->requestQSto));
	QHsm_ctor(&me->super, (QStateHandler)&screen_initial);	
	QTimeEvt_ctor(&me->ScreenTestTimeEvt, SCREEN_TIMEOUT_SIG);

	return (QHsm *)me;
}

static QState screen_initial(SCREEN_COMP *me, QEvent const *e)
{
	(void)e;
	
	return Q_TRAN(&screen_processing);
}

static QState screen_processing(SCREEN_COMP *me, QEvent const *e)
{
	ComCmdEvt *cmde = (ComCmdEvt *)e;
	
	switch(e->sig) {
		case Q_ENTRY_SIG: {
			QActive_recall(me->father, &me->requestQueue);
			//bbu_printf("QActive_recall from Screen test request queue\r\n");
			return Q_HANDLED();
		}
		
		case SCREEN_TEST_START_SIG: {
			if(screen_handler(cmde->argc, cmde->argv)){
				return Q_TRAN(&screen_is_runing);
			} else {		
				return Q_HANDLED();
			}
		}
	}

	return Q_SUPER(&QHsm_top);
}


static QState screen_is_runing(SCREEN_COMP *me, QEvent const *e)
{
	switch(e->sig) {	
		case SCREEN_TEST_ACK_SIG:{
			if (collect_screen_result((const ScreenCmdAck *)e)) {
				return Q_TRAN(&screen_processing);
			} else {
				return Q_HANDLED();
			}
		}
		
		case SCREEN_TIMEOUT_SIG:{
			screen_timeout_handle();
			return Q_HANDLED();
		}	
	}

	return Q_SUPER(&screen_is_busy);
}

static QState screen_is_busy(SCREEN_COMP *me, QEvent const *e)
{
	switch(e->sig) {
		case SCREEN_TEST_START_SIG: {
			if (QEQueue_getNFree(&me->requestQueue) > 0) {
				QActive_defer(me->father, &me->requestQueue, e);
				//bbu_printf("QActive_defer to Screen test request queue\r\n");
			} else 
				bbu_printf("Request ignored.\r\n");
			return Q_HANDLED();
		}
	}

	return Q_SUPER(&QHsm_top);
}






