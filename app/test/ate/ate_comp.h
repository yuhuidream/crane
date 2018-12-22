/*
 *  ate_comp.h
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */
 
#ifndef ATE_COMP_H
#define ATE_COMP_H

#include "predefines.h"
#include "qp_port.h"
#include "bbu.h"

typedef struct ATETag {
	QHsm super;

	QActive *father;
	QTimeEvt TestTimeEvt;					/* time event for screen test */
	QEQueue requestQueue;    /* native QF queue for deferred request events */
	QEvent const *requestQSto[6];      /* storage for deferred queue buffer */	
	
	uint8_t all_in_one;
	
} ATE_COMP;

extern ATE_COMP l_ate_comp;
QHsm *ATE_COMP_ctor(void);


#endif
