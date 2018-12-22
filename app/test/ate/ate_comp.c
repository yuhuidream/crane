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
#include "ate_test.h"
#include "ate_comp.h"


Q_DEFINE_THIS_FILE


/* hierarchical state machine ............................................. */
static QState ate_initial(ATE_COMP *me, QEvent const *e);
static QState ate_processing(ATE_COMP *me, QEvent const *e);
static QState ate_is_runing(ATE_COMP *me, QEvent const *e);
static QState ate_is_busy(ATE_COMP *me, QEvent const *e);

ATE_COMP l_ate_comp;


QHsm *ATE_COMP_ctor(void)
{
	ATE_COMP *me = &l_ate_comp;
		
	QEQueue_init(&me->requestQueue, me->requestQSto, Q_DIM(me->requestQSto));
	QHsm_ctor(&me->super, (QStateHandler)&ate_initial);	
	QTimeEvt_ctor(&me->TestTimeEvt, ATE_TEST_TIMEOUT_SIG);

	return (QHsm *)me;
}

static QState ate_initial(ATE_COMP *me, QEvent const *e)
{
	(void)e;
	
	return Q_TRAN(&ate_processing);
}

static QState ate_processing(ATE_COMP *me, QEvent const *e)
{
	ATEReqEvt *evt = (ATEReqEvt *)e;
	
	switch(e->sig) {
		case Q_ENTRY_SIG: {
			QActive_recall(me->father, &me->requestQueue);
			DEBUG_ATE_PRINT("QActive_recall from ATE test request queue\r\n");
			DEBUG_ATE_PRINT("Enter QState ate_processing!\r\n");
			return Q_HANDLED();
		}
				
		case ATE_TEST_REQ_SIG: {
			if (ate_start_test(evt->group_id) == ATE_TRUE) {
				return Q_TRAN(&ate_is_runing);
			} else {
				return Q_HANDLED();
			}
		}
		
		case ATE_TEST_CTRL_SIG: {
			ComCmdEvt *evt = (ComCmdEvt *)e;
			
			if (evt->argc == 1) {
				ate_test_request(ATE_MAX_CASE_INDEX);
			} else if (evt->argc == 2) {			
				if (strcmp((const char*)evt->argv[1], "-d") == 0) {
					ate_test_cmd_display();
				} else {
					uint32_t id = conv_dec((char*)evt->argv[1]);
					ate_test_request(id);
				}
			} else {
				bbu_printf("ate---> The argument is not correct!\r\n");
			}

			return Q_HANDLED();
		}
	}
	return Q_SUPER(&QHsm_top);
}

static QState ate_is_runing(ATE_COMP *me, QEvent const *e)
{
	ATEAckEvt *evt = (ATEAckEvt *)e;

	switch(e->sig) {	
		case Q_ENTRY_SIG: {
			DEBUG_ATE_PRINT("Enter QState ate_is_runing!\r\n");
			return Q_HANDLED();
		}
		
		case ATE_TEST_RESULI_SIG: {
			if (collect_ate_result(evt)) {
				return Q_TRAN(&ate_processing);
			} else {
				return Q_HANDLED();
			}
		}
		
		case ATE_TEST_TIMEOUT_SIG: {
			ate_timeout_handle();
			return Q_HANDLED();
		}
	}

	return Q_SUPER(&ate_is_busy);
}

static QState ate_is_busy(ATE_COMP *me, QEvent const *e)
{
	switch(e->sig) {
		case Q_ENTRY_SIG: {
			DEBUG_ATE_PRINT("Enter QState ate_is_busy!\r\n");
			return Q_HANDLED();
		}
		
		case ATE_TEST_REQ_SIG: {
			if (QEQueue_getNFree(&me->requestQueue) > 0) {
				QActive_defer(me->father, &me->requestQueue, e);
				DEBUG_ATE_PRINT("QActive_defer to ATE test request queue\r\n");
			} else 
				bbu_printf("Request ignored.\r\n");			
			return Q_HANDLED();
		}
	}

	return Q_SUPER(&QHsm_top);
}

