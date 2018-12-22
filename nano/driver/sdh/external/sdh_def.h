/*
 *  sdh_error.h
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */

#ifndef SDH_ERROR_H
#define SDH_ERROR_H

#include "qp_port.h"
#include "predefines.h"
#include "scatterlist.h"

enum {
	SDH_TRUE = 0,
	SDH_FALSE,
	SDH_TIMEDOUT,
	SDH_ILSEQ,
	SDH_INVAL,
	SDH_NOMEDIUM,
	SDH_UNSTABLE,
	SDH_1V8_SWITCH_FAIL,
} sdh_error;

struct SDTransReq
{
	uint8_t ops_type;  /* read, write, and erase */
	uint8_t trans_mode;
	struct sg_table table; 
	struct dev_request *drq;
};

#endif
