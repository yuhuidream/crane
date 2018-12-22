#include "bbu.h"
#include "ripc.h"
#include "interrupt.h"
#include "timer_dec.h"

static RIPC_STATUS ripc_status[RIPC_NUM] = {0};
static int __ripc_test = 0;

static RIPC_TypeDef *get_ripc_base(int idx)
{
   RIPC_TypeDef *ripc = (RIPC_TypeDef *)0;
   switch(idx){
      case 0:
         ripc = RIPC0;
         break;
      case 1:
         ripc = RIPC1;
         break;
      case 2:
         ripc = RIPC2;
         break;
      case 3:
         ripc = RIPC3;
         break;
      default:
         break;
   }
   return ripc;
}

int request_ripc(int idx, int interrupt)
{
   uint32_t reg = 0;
   RIPC_TypeDef *ripc = get_ripc_base(idx);
   if(ripc == (RIPC_TypeDef *)0){
      bbu_printf(DBG_MSG "RIPC %d doesn't exist\n\r");
      return MV_ERROR;
   }

   reg = ripc->RIPCx_STATUS_REG;

   /* The resource is free, after the read, it become busy */
   if( (reg & 1) == 0 ){
      ripc_status[idx] = BUSY4CP;
      return MV_OK;
   }
   
   /* If the resource is busy, request an interrupt when it becommes free */
   ripc_status[idx] = BUSY4OTHER;
   if(interrupt){
      ripc->RIPCx_INT2_REG = 1;    //CP
   }
   return MV_FAIL;
}

int release_ripc(int idx)
{
   RIPC_TypeDef *ripc = get_ripc_base(idx);
   if(ripc == (RIPC_TypeDef *)0){
      bbu_printf(DBG_MSG "RIPC %d doesn't exist\n\r");
      return MV_ERROR;
   }

   if(ripc_status[idx] == BUSY4CP){
      ripc->RIPCx_STATUS_REG = 1; //Clear the status and free the ripc
      ripc_status[idx] = FREE;
      return MV_OK;
   }else{
      bbu_printf(DBG_MSG "Can't free RIPC %d\n\r", idx);
   }
   return MV_FAIL;
}

static void ripc_isr(void *arg)
{
   (void)arg;
   RIPC_TypeDef *ripc = (RIPC_TypeDef *)0;
   for(int i = 0; i < RIPC_NUM; i++){
      if(ripc_status[i] == BUSY4OTHER){
         ripc = get_ripc_base(i);
         ripc->RIPCx_INT2_REG = 0;    //CP
         ripc_status[i] = FREE;
         if(__ripc_test){
            mdelay(50);
            bbu_printf("\n\r%sripc-->  RIPC %d is release by another core\n\r", BBU_PROMPT, i);
         }
      }
   }
   if(__ripc_test)
      __ripc_test = 0;
}

void ripc_spin_lock(int idx)
{
   do{
      if(request_ripc(idx, 1) == MV_OK){
         return;
      }
   
      //Request RIPC fails
      while(ripc_status[idx] == BUSY4OTHER){
         udelay(1);
      }
   }while(1);
}

void ripc_spin_unlock(int idx)
{
   release_ripc(idx);
}

void ripc_init(void)
{
   //airq_int_enable(AIRQ_RIPC_INT0, ripc_isr, NULL);
   ISR_Connect(IRQ_MMP_RIPC_CP, ripc_isr, NULL);
   INT_Enable(IRQ_MMP_RIPC_CP, 0, 15);
}

int bbu_ripc_test(int argc, uint8_t **argv){
   int ripc = 0;
   int test = 0; //1: request, 0: free
   __ripc_test = 1;
   switch(argc){
      case 3:
         ripc = conv_dec((char *)argv[2]);
      case 2:
         test = conv_dec((char *)argv[1]);
         if(check_help((uint8_t *)argv[1] )){
            bbu_ripc_help();
            return SCREEN_TEST_PASS;
         }
         break;
      default:
         bbu_ripc_help();
         return SCREEN_TEST_PASS;
   }
   if(test){
      if(request_ripc(ripc, 1) == MV_OK){
         bbu_printf("ripc-->  Succeed to request RIPC %d\n\r", ripc);
      }else{
         bbu_printf("ripc-->  Fail to request RIPC %d, request interrupt when it gets free\n\r", ripc);
      }
   }else{
      if(release_ripc(ripc) == MV_OK){
         bbu_printf("ripc-->  Succeed to release RIPC %d\n\r", ripc);
      }else{
         bbu_printf("ripc-->  Fail to release RIPC %d\n\r", ripc);
      }
   }
   return SCREEN_TEST_PASS;
}
