#include "bbu.h"
#include "ipc_app.h"
#include "ipc_dec.h"
#include <string.h>

Q_DEFINE_THIS_FILE

QMPool  * const SharedPool_  = (QMPool *)SHARE_POOL_CTRL;
QEQueue * const APQueue_     = (QEQueue *)CA7_QUEUE_CTRL;
QEQueue * const CPQueue_     = (QEQueue *)CR5_QUEUE_CTRL;
QEQueue * const DSPQueue_    = (QEQueue *)DSP_QUEUE_CTRL;

/*..........................................................................*/
//CA7 need to initialize Queues and shared pool
int ipc_app_init(void)
{
   if(SHARE_POOL_END > (CORE_SHARE_MEM_HEAD+CORE_SHARE_MEM_LEN))
   {
      bbu_printf("Error: the reserved shared buffer is not enough\n\r");
      return MV_FAIL;
   }

   /* initialize the AP/CP/DSP shared memory pool */
   QMPool_init(SharedPool_, (void *)SHARE_POOL_BUF, SHARE_POOL_SIZE,
               MAX_EVENT_LEN);

   /* initialize the AP queue */
   QEQueue_init(APQueue_, (QEvt const **)CA7_QUEUE_BUF, CA7_QUEUE_LEN);

   /* initialize the CP queue */
   QEQueue_init(CPQueue_, (QEvt const **)CR5_QUEUE_BUF, CR5_QUEUE_LEN);

   /* initialize the DSP queue */
   QEQueue_init(DSPQueue_, (QEvt const **)DSP_QUEUE_BUF, DSP_QUEUE_LEN);

   return MV_OK;
}

/*..........................................................................*/
QEvt *QIPC_New_(QEvtSize const evtSize, enum_t const sig)
{
   QEvt *e;
   if(evtSize > MAX_EVENT_LEN){
      bbu_printf("QEvtSize %d exceed the MAX value\n\r", evtSize);
      return (QEvt *)0;
   }

   e = (QEvt *)QMPool_get(SharedPool_, 1);
   if(e != (QEvt *)NULL)
      e->sig = sig;

   return (QEvt *)e;
}

/*..........................................................................*/
uint8_t QIPC_postFIFO(IpcTarget core, QEvt const * const e)
{
    QEQueueCtr nFree;          /* temporary to avoid UB for volatile access */
    QEQueueCtr margin = 0;
    uint8_t status;

    Q_REQUIRE(e != (QEvt const *)0);                 /* event must be valid */

    QF_CRIT_STAT_TYPE state;
    QF_CRIT_ENTRY(state);
    QEQueue *me;
    switch(core)
    {
        case IPC2AP:
            me = APQueue_;
            break;
        case IPC2CP:
            me = CPQueue_;
            break;
        case IPC2DSP:
            me = DSPQueue_;
            break;
        default:
            bbu_printf("Fatal error\n\r");
            return 0;
            break;
    }

    nFree = me->nFree;                   /* get volatile into the temporary */

    if (nFree > (QEQueueCtr)margin) {         /* required margin available? */

        if (e->poolId_ != (uint8_t)0) {              /* is it a pool event? */
            ++(((QEvt *)e)->refCtr_);
        }

        --nFree;                             /* one free entry just used up */
        me->nFree = nFree;                           /* update the volatile */
        if (me->nMin > nFree) {
            me->nMin = nFree;                      /* update minimum so far */
        }

        if (me->frontEvt == (QEvt const *)0) {      /* was the queue empty? */
            me->frontEvt = e;                     /* deliver event directly */
        }
        else {    /* queue was not empty, insert event into the ring-buffer */
                                /* insert event into the ring buffer (FIFO) */
            me->ring[me->head] = e;     /* insert e into buffer */
            if (me->head == (QEQueueCtr)0) {      /* need to wrap the head? */
                me->head = me->end;                          /* wrap around */
            }
            --me->head;
        }
        status = (uint8_t)1;                   /* event posted successfully */
    }else {
        Q_ASSERT(margin != (uint16_t)0);    /* can tollerate dropping evts? */

        status = (uint8_t)0;
    }
    QF_CRIT_EXIT(state);

    return status;
}

/*..........................................................................*/
void QIPC_sendFIFO(IpcTarget core, QEvt const * const e)
{
    IpcEvt *ipce= Q_NEW(IpcEvt, IPC_SREQ_SIG);
    ipce->ipce = (QEvt *)e;
    ipce->fifo = 1;
    ipce->target = core;
    QACTIVE_POST(AO_IPC, (QEvt *)ipce, 0);
}

/*..........................................................................*/
void qipc_cmd_handler(IpcTarget core)
{
   QEvt *e;
   e = (QEvt *)Q_NEW(QEvt, IPC_RREQ_SIG);
   bbu_printf(DBG_MSG "POST Recv REQ SIG\n\r");
   QACTIVE_POST(AO_IPC, e, 0);
   bbu_printf(DBG_MSG "Send ACK IPC\n\r");
   ipc_send_ack((uint32_t)e, core);
}

void qipc_ack_handler(uint32_t ack, IpcTarget core)
{
   bbu_printf(DBG_MSG"Receive ACK IPC from %d\n\r", core);
   IpcEvt *e = (IpcEvt *)Q_NEW(IpcEvt, IPC_SACK_SIG);
   e->ipce = (QEvt *)ack;
   QACTIVE_POST(AO_IPC, e, 0);
   bbu_printf(DBG_MSG"Send ACK to IPC AO of %d\n\r", core);
}


#if 0
int QIPC_postFIFO(QEvt const * const e, QEvtSize const evtSize){
   QEvt *ipce;
   ipce = QIPC_New_(evtSize, 0);
   if(ipce == NULL){
      bbu_printf(DBG_MSG"Error: can't allocate IPC event\n\r");
      return 0;
   }

   memcpy((void *)ipce, (const void*)e,(size_t)evtSize);

   __QIPC_postFIFO(ipce);

   return 1;
}
#endif
