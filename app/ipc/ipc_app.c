#include "bbu.h"
#include "ipc_app.h"
#include <string.h>
#include "ipc_dec.h"

Q_DEFINE_THIS_FILE

typedef struct IPCTag{
   QActive super;
   QEvt *pending;
}IPC;

static QState ipc_initial(IPC *me, QEvt const *e);
static QState ipc_idle(IPC *me, QEvt const *e);
static QState ipc_receiving(IPC *me, QEvt const *e);
static QState ipc_sending(IPC *me, QEvt const *e);

static IPC l_ipc;

/* global objects.......................................................... */
QActive * const AO_IPC = (QActive *)&l_ipc;         /*opaque pointer to I2C */

void IPC_ctor(void){
   IPC *me = &l_ipc;
   QActive_ctor(&me->super, (QStateHandler)&ipc_initial);
}

/*..........................................................................*/
static QState ipc_initial(IPC *me, QEvt const *e){
   (void)me;
   (void)e;
   return Q_TRAN(&ipc_idle);
}

/*..........................................................................*/
static QState ipc_idle(IPC *me, QEvt const *e){
   switch(e->sig){
      case Q_ENTRY_SIG:{
         return Q_HANDLED();
      }
      case IPC_RREQ_SIG:{
         bbu_printf(DBG_MSG "Receive RECV REQ SIG\n\r");
         static QEvt reqEvt = QEVT_INIT(IPC_RECV_SIG, 0, 0);
         QACTIVE_POST(AO_IPC, &reqEvt, 0);
         return Q_TRAN(&ipc_receiving);
      }
      case IPC_SREQ_SIG:{
         bbu_printf(DBG_MSG "Receive SEND REQ SIG\n\r");
         if(((IpcEvt *)e)->fifo){
            QIPC_postFIFO(((IpcEvt *)e)->target, (QEvt const *)(((IpcEvt *)e)->ipce));
         }
         me->pending = (QEvt *)(((IpcEvt *)e)->ipce);
         bbu_printf(DBG_MSG "Send CMD to %d\n\r", ((IpcEvt *)e)->target);
         ipc_send_cmd(IPC_QEVENT_CMD, ((IpcEvt *)e)->target);
         return Q_TRAN(&ipc_sending);
      }
      
   }
   return Q_SUPER(&QHsm_top);
}

/*..........................................................................*/
static QState ipc_receiving(IPC *me, QEvt const *e){
   QEvt * ipcEvt, *taskEvt;
   switch(e->sig){
      case IPC_RECV_SIG:{
         bbu_printf(DBG_MSG "Receive CMD from %s\n\r", THAT_CORE);
         ipcEvt = (QEvt *)QEQueue_get(OwnQueue);
         switch(ipcEvt->sig){
            case COM_RX_CMD_READY_SIG:{
               taskEvt = (QEvt *)Q_NEW(ComCliEvt, COM_RX_CMD_READY_SIG);
               ((ComCliEvt *)taskEvt)->len = ((ComCliEvt *)ipcEvt)->len;
               ((ComCliEvt *)taskEvt)->source = ((ComCliEvt *)ipcEvt)->source;
               strcpy((char*)((ComCliEvt *)taskEvt)->CliBuffer, 
                      (char *)((ComCliEvt *)ipcEvt)->CliBuffer);
               QACTIVE_POST(AO_Cli, (QEvt *)taskEvt, 0);
               break;
            }
            case CO_STRESS_PASS_SIG:{
               taskEvt = (QEvt *)Q_NEW(QEvt, ipcEvt->sig);
               QACTIVE_POST(AO_SCHED, (QEvt *)taskEvt, 0);
               break;
            }
            case CO_STRESS_FAIL_SIG:{
               taskEvt = (QEvt *)Q_NEW(QEvt, ipcEvt->sig);
               QACTIVE_POST(AO_SCHED, (QEvt *)taskEvt, 0);
               break;
            }
            case BBU_TEST_SIG:{
               break;
            }
         }
         bbu_printf(DBG_MSG "Send ACK to %s\n\r", THAT_CORE);
         ipc_send_ack((uint32_t)ipcEvt, ((IpcEvt *)e)->target);
         QMPool_put(SharedPool_, (void *)ipcEvt);
         return Q_TRAN(&ipc_idle);
      }
      case IPC_RREQ_SIG:{
         //defer here
         return Q_HANDLED();
      }
   }
   return Q_SUPER(&QHsm_top);
}

/*..........................................................................*/
static QState ipc_sending(IPC *me, QEvt const *e){
   switch(e->sig){
      case IPC_SACK_SIG:{
         
         return Q_TRAN(&ipc_idle);
      }
   }
   return Q_SUPER(&QHsm_top);
}