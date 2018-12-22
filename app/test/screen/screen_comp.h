/*
 *  screen_comp.h
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */
 
#ifndef SCREEN_COMP_H
#define SCREEN_COMP_H

#include "predefines.h"
#include "qp_port.h"
#include "bbu.h"

typedef struct SCRTag {
	QHsm super;

	QActive *father;
	QTimeEvt ScreenTestTimeEvt;					/* time event for screen test */
	QEQueue requestQueue;    /* native QF queue for deferred request events */
	QEvent const *requestQSto[6];      /* storage for deferred queue buffer */	
	
} SCREEN_COMP;

extern SCREEN_COMP l_screen_comp;

QHsm *SCREEN_COMP_ctor(void);



#endif
