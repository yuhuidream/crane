#ifndef h_ipc_app_h
#define h_ipc_app_h

#include "ipc_dec.h"
#include "shared_mem.h"


extern QMPool  * const SharedPool_;
extern QEQueue * const APQueue_;
extern QEQueue * const CPQueue_;

#define OwnQueue    CPQueue_

#define QIPC_NEW(evtT_, sig_) \
   ((evtT_ *)QIPC_New_((QEvtSize)sizeof(evtT_), (sig_)))

typedef struct IpcEvtTag{
   QEvt super;
   QEvt *ipce;
   int  fifo;
   int  target;
}IpcEvt;

uint8_t QIPC_postFIFO(IpcTarget core, QEvt const * const e);
void QIPC_sendFIFO(IpcTarget core, QEvt const * const e);

#endif
