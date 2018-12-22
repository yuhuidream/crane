#include "bbu.h"
#include "IPC.h"
#include "ipc_dec.h"
#include "interrupt.h"
#include "APBCLK.h"
#include <string.h>
#include "ipc_app.h"

static void ipc_ack_isr(void *arg);
static void ipc_cmd_isr(void *arg);
static void ipc_msg_isr(void *arg);
static void ipc_gp_isr(void *arg);

static int ipc_testing = 0;
static uint32_t __iir = 0;

void ipc_init(void)
{
   ISR_Connect(IRQ_MMP_CEVA2CP_IPC0, ipc_ack_isr, NULL);
   INT_Enable(IRQ_MMP_CEVA2CP_IPC0, IRQ_ROUTE, 15);
   ISR_Connect(IRQ_MMP_CEVA2CP_IPC1, ipc_cmd_isr, NULL);
   INT_Enable(IRQ_MMP_CEVA2CP_IPC1, IRQ_ROUTE, 15);
   ISR_Connect(IRQ_MMP_CEVA2CP_IPC2, ipc_msg_isr, NULL);
   INT_Enable(IRQ_MMP_CEVA2CP_IPC2, IRQ_ROUTE, 15);
   ISR_Connect(IRQ_MMP_CEVA2CP_IPC3, ipc_gp_isr, NULL);
   INT_Enable(IRQ_MMP_CEVA2CP_IPC3, IRQ_ROUTE, 15);
    //PMUCP->CPSS_CLK_GATE_CTRL |= (BIT_14 | BIT_17);
}

void ipc_send_ack(uint32_t ack_type, IpcTarget core)
{
   switch(core)
   {
      case IPC2AP:
         IPC->WDR  = ack_type;
         IPC->ISRW = IPC_SET_ACK_INT;
         break;
      case IPC2DSP:
         IPCCP->WDR  = ack_type;
         IPCCP->ISRW = IPC_SET_ACK_INT;
         break;
   }
}

void ipc_send_cmd(uint32_t cmd_type, IpcTarget core)
{
   switch(core)
   {
      case IPC2AP:
         IPC->WDR  = cmd_type;
         IPC->ISRW = IPC_SET_CMD_INT;
         break;
      case IPC2DSP:
         IPCCP->WDR  = cmd_type;
         IPCCP->ISRW = IPC_SET_CMD_INT;
         break;
   }
}

void ipc_send_msg(uint32_t *msg_address, IpcTarget core)
{
   switch(core)
   {
      case IPC2AP:
         IPC->WDR  = (uint32_t)msg_address;
         IPC->ISRW = IPC_SET_MSG_INT;
         break;
      case IPC2DSP:
         IPCCP->WDR  = (uint32_t)msg_address;
         IPCCP->ISRW = IPC_SET_MSG_INT;
         break;
   }
}

void ipc_send_gp(uint32_t gp_type, IpcTarget core)
{
   switch(core)
   {
      case IPC2AP:
         IPC->WDR  = gp_type;
         IPC->ISRW = IPC_SET_GP_INT;
         break;
      case IPC2DSP:
         IPCCP->WDR  = gp_type;
         IPCCP->ISRW = IPC_SET_GP_INT;
         break;
   }
}

static void ipc_cmd_isr(void *arg)
{
   (void)arg;
   uint32_t rdr, iir;
   IPC_TypeDef *ipc;
   IpcTarget core = (int)arg;
   switch(core){
      case IPC2DSP:
         ipc = IPCCP;
         break;
      case IPC2AP:
         ipc = IPC;
         break;
   }
   ipc->IIR = 0;
   rdr = ipc->RDR;
   iir = ipc->IIR;

   if(iir & IPC_SET_CMD_INT){
      //bbu_printf("%s receive command 0x%08X from %s\n\r", THIS_CORE, rdr, THAT_CORE);
      ipc->ICR = IPC_SET_CMD_INT;
      switch(rdr){
         case IPC_TEST_BEGIN:
            ipc_send_ack(IPC_TEST_END, (IpcTarget)arg);
            break;
         case IPC_QEVENT_CMD:
            qipc_cmd_handler();
            break;
      }
   }else{
      bbu_printf("Error, %s enter wrong IPC ISR ipc_cmd_isr\n\r", THIS_CORE);
      ipc->ICR = iir;
   }
}

static void ipc_msg_isr(void *arg)
{
   (void)arg;
   uint32_t rdr, iir;
   IPC_TypeDef *ipc;
   IpcTarget core = (int)arg;
   switch(core){
      case IPC2DSP:
         ipc = IPCCP;
         break;
      case IPC2AP:
         ipc = IPC;
         break;
   }
   ipc->IIR = 0;
   rdr = ipc->RDR;
   iir = ipc->IIR;

   if(iir & IPC_SET_MSG_INT){
      //DBG_PRINT(ipc_testing, "%s receive msg 0x%08X from %s\n\r", THIS_CORE, rdr, THAT_CORE);
      ipc->ICR = IPC_SET_MSG_INT;
      if(rdr == IPC_TEST_BEGIN)
         ipc_send_ack(IPC_TEST_END, (IpcTarget)arg);
   }else{
      bbu_printf("Error, %s enter wrong IPC ISR ipc_msg_isr\n\r", THIS_CORE);
      ipc->ICR = iir;
   }
}

static void ipc_ack_isr(void *arg)
{
   (void)arg;
   uint32_t rdr, iir;
   IPC_TypeDef *ipc;
   IpcTarget core = (int)arg;
   switch(core){
      case IPC2DSP:
         ipc = IPCCP;
         break;
      case IPC2AP:
         ipc = IPC;
         break;
   }
   ipc->IIR = 0;
   rdr = ipc->RDR;
   iir = ipc->IIR;

   if(iir & IPC_SET_ACK_INT){
      //DBG_PRINT(ipc_testing, "%s receive ack 0x%08X from %s\n\r", THIS_CORE, rdr, THAT_CORE);
      ipc->ICR = IPC_SET_ACK_INT;
      switch(rdr){
         case IPC_TEST_BEGIN:
            ipc_send_ack(IPC_TEST_END, (IpcTarget)arg);
            break;
         case IPC_TEST_END:
            __iir = IPC_TEST_END;
            break;
         default:
            qipc_ack_handler();
            break;
      }
   }else{
      bbu_printf("Error, %s enter wrong IPC ISR ipc_ack_isr\n\r", THIS_CORE);
      ipc->ICR = iir;
   }
}

static void ipc_gp_isr(void *arg)
{
   (void)arg;
   uint32_t rdr, iir;
   IPC_TypeDef *ipc;
   IpcTarget core = (int)arg;
   switch(core){
      case IPC2DSP:
         ipc = IPCCP;
         break;
      case IPC2AP:
         ipc = IPC;
         break;
   }
   ipc->IIR = 0;
   rdr = ipc->RDR;
   iir = ipc->IIR;

   if(iir & IPC_SET_GP_INT){
      //DBG_PRINT(ipc_testing, "%s receive gp IPC 0x%08X from %s\n\r", THIS_CORE, rdr, THAT_CORE);
      ipc->ICR = IPC_SET_GP_INT;
      if(rdr == IPC_TEST_BEGIN)
         ipc_send_ack(IPC_TEST_END, (IpcTarget)arg);
   }else{
      bbu_printf("Error, %s enter wrong IPC ISR ipc_gp_isr\n\r", THIS_CORE);
      ipc->ICR = iir;
   }
}

int bbu_ipc_test(int argc, uint8_t **argv)
{
   __iir = 0;
   int time_out = 0x4000;
   if(!strcmp((const char*)argv[1], "cmd")){
      bbu_printf("%s send command to %s\n\r", THIS_CORE, THAT_CORE);
      ipc_send_cmd(IPC_TEST_BEGIN, IPC2AP);
   }else if(!strcmp((const char*)argv[1], "ack")){
      bbu_printf("%s send ack to %s\n\r", THIS_CORE, THAT_CORE);
      ipc_send_ack(IPC_TEST_BEGIN, IPC2AP);
   }else if(!strcmp((const char*)argv[1], "msg")){
      bbu_printf("%s send msg to %s\n\r", THIS_CORE, THAT_CORE);
      ipc_send_msg((uint32_t *)IPC_TEST_BEGIN, IPC2AP);
   }else if(!strcmp((const char*)argv[1], "gp")){
      bbu_printf("%s send gp IPC to %s\n\r", THIS_CORE, THAT_CORE);
      ipc_send_gp(IPC_TEST_BEGIN, IPC2AP);
   }else{
      bbu_ipc_test_help();
      return SCREEN_TEST_PASS;
   }
   while(time_out){
      if(__iir == IPC_TEST_END){
         __iir = 0;
         break;
      }else{
         cpu_cycle_delay(10);
      }
      time_out --;
   }
   if(time_out == 0){
      bbu_printf("Test fail, can't get reply in time\n\r");
      return SCREEN_TEST_FIAL;
   }else{
      bbu_printf("Test pass, get reply from %s\n\r", THAT_CORE);
   }
   return SCREEN_TEST_PASS;
}
