#include "bbu.h"
#include "perf_counter.h"
#include "interrupt.h"
#include "timer_dec.h"
#include "smp.h"
#include "spinlock.h"

CycleCntData cycle_data;
static int curm_start = 0;
static int cycle_div = 1;
extern void *ao_info_translate(void *ao_name, int n2p);

/*..........................................................................*/
static void perf_counter_isr(void *argv){
   uint8_t core = (uint8_t)(uint32_t)argv;
   if(cycle_counter_overflowed()){
      cycle_data.overflow[core] ++;
      bbu_printf(DBG_MSG "overflow happens on core%d\n\r", core);
      cycle_counter_clear_overflow();
   }else{
      bbu_printf(DBG_MSG "Not overflow happens\n\r");
   }
}

/*..........................................................................*/
void cycle_counter_dump(void* info){
   (void)info;
   if(is_cycle_counter_enabled()){
      counter_disable(CYCLE_COUNTER_ENABLE_BASE);
      uint32_t count = cycle_counter_read();
      bbu_printf("CPU cycle counter: 0x%08X on core%d\n\r", count,GetCPUID()-1);
      counter_enable(CYCLE_COUNTER_ENABLE_BASE);
   }else{
      bbu_printf("CPU cycle counter is not enabled\n\r");
   }
}

#if CORE_NUM > 2
static spinlock_t curm_lock = {0};
#endif
/*..........................................................................*/
void cpu_utilization_init(void* info){
   (void)info;
   int core = smp_processor_id();
#ifdef CONFIG_SMP
//   QActive *act = QC_getCurrentAO(core);
//   if(act){
//      ao_cycle_record_end(act->prio, core);
//      ao_cycle_record_begin(act->prio, core);
//   }
#endif
#if CORE_NUM > 2
   spin_lock(&(curm_lock));
#endif

   bbu_printf("\r******* CPU%d utilization rate measurement start"
              " *******\n\r", core);
#if CORE_NUM > 2
   spin_unlock(&(curm_lock));
#endif

   cycle_counter_reset();

   if(cycle_div){
      cycle_counter_divider_set();
   }else{
      cycle_counter_divider_clear();
   }

   cycle_counter_write(0);

   memset(&(cycle_data), 0x0, sizeof(CycleCntData));
   INT_Enable(IRQ_MMP_PERFM_CP, 0, 15);
   ISR_Connect(IRQ_MMP_PERFM_CP, perf_counter_isr, (void *)core);

   perf_counters_enable();

   counter_enable(CYCLE_COUNTER_ENABLE_BASE);

   cycle_counter_int_enable();
}

/*..........................................................................*/
void ao_cycle_record_begin(uint8_t prio, uint8_t core){
   (void)prio;
   if(curm_start){
      counter_disable(CYCLE_COUNTER_ENABLE_BASE);
      cycle_data.prev[core] = cycle_counter_read();
      /* disable interrupt and save */
      if(cycle_data.overflow[core]){
         cycle_data.total[core].overflow += cycle_data.overflow[core];
         cycle_data.overflow[core] = 0;
      }
      /* restore interrupt */
      counter_enable(CYCLE_COUNTER_ENABLE_BASE);
   }
}

/*..........................................................................*/
void ao_cycle_record_end(uint8_t prio, uint8_t core){
   if(curm_start){
      uint32_t count, temp;
      counter_disable(CYCLE_COUNTER_ENABLE_BASE);
      count = cycle_counter_read();
   
      cycle_data.total[core].overflow += cycle_data.overflow[core];
   
      if(count > cycle_data.prev[core]){
         temp = count - cycle_data.prev[core];
         count = temp;
      }else{
         if(cycle_data.overflow[core]){
            cycle_data.overflow[core] -- ;
            temp = 0xFFFFFFFF - cycle_data.prev[core];
            temp += (count + 1);
            count = temp;
         }else{
            bbu_printf(DBG_MSG "ERROR: cycle count works abnormally "
                       "on core%d\n\r", core);
            bbu_printf(DBG_MSG "Previous: 0x%08X\n\r", cycle_data.prev[core]);
            bbu_printf(DBG_MSG "Current: 0x%08X\n\r", count);
         }
      }
      temp = 0xFFFFFFFF - count;
      temp += 1;
      if(temp < cycle_data.ao[core][prio].count){    /* overflow? */
         cycle_data.ao[core][prio].count -= temp;     /* yes */
         cycle_data.ao[core][prio].overflow ++;
      }else{
         cycle_data.ao[core][prio].count += count;
      }
   
      cycle_data.ao[core][prio].overflow += cycle_data.overflow[core];
      cycle_data.overflow[core] = 0;
      
      counter_enable(CYCLE_COUNTER_ENABLE_BASE);
   }
}

/*..........................................................................*/
void cpu_utilization_stop(void* info){
   double rate, busy, total;
   (void)info;
   uint8_t core;
   uint32_t count;
   core = GetCPUID()-1;
#ifdef CONFIG_SMP
   QActive *act = QC_getCurrentAO(core);
   if(act){
      curm_start = 1;
      ao_cycle_record_end(act->prio, core);
      curm_start = 0;
   }
#endif
   counter_disable(CYCLE_COUNTER_ENABLE_BASE);
   count = cycle_counter_read();
   cycle_data.total[core].count = count;
   cycle_data.total[core].overflow +=  cycle_data.overflow[core];
   total = (double)(cycle_data.total[core].count)/0x10000 + \
                    cycle_data.total[core].overflow*0x10000;
   if(cycle_div)
      total *= cycle_div;
#if CORE_NUM > 2
   spin_lock(&(curm_lock));
#endif
   bbu_printf("\r***********    CPU%d utilization rate "
              "measurement stop    ***********\n\r", core);
   bbu_printf("     AO   priority         Total Cycle         "
              "CPU utilization rate\n\r");
   for(int i = 0; i <= QF_MAX_ACTIVE; i++){
      if(QF_active_[i] != 0){  /* a valid AO */
         busy = (double)(cycle_data.ao[core][i].count)/0x10000+\
                         cycle_data.ao[core][i].overflow*0x10000;
         rate = (busy*100)/total;
         bbu_printf(" %8s   %2d", ao_info_translate(QF_active_[i], 0), i);
         bbu_printf("       0x%08X+0x100000000*%d      ",
                    cycle_data.ao[core][i].count, 
                    cycle_data.ao[core][i].overflow);
         bbu_printf("%10.8f\n\r", rate);
      }
   }
#if CORE_NUM > 2
   spin_unlock(&(curm_lock));
#endif
}

/*..........................................................................*/
void bbu_curm(int argc, uint8_t **argv)
{
   switch(argc){
      case 1:
         bbu_curm_help();
         break;
      case 2:
         if(!strcmp((const char*)argv[1], "start") ||
              !strcmp((const char*)argv[1], "1")){
              //pxa988_cti_init();
              curm_start = 1;
              on_each_cpu(cpu_utilization_init, NULL, 1);
         }
         else if(!strcmp((const char*)argv[1], "stop") ||
              !strcmp((const char*)argv[1], "0")){
              if(curm_start){
                 curm_start = 0;
                 on_each_cpu(cpu_utilization_stop, NULL, 1);
                 cycle_div = 1; /* set as default value */
              }else{
                 bbu_printf("Please start the curm test first\n\r");
              }
         }else if(!strcmp((const char*)argv[1], "d")){
            on_each_cpu(cycle_counter_dump, NULL, 1);
         }else{
            bbu_curm_help();
         }
         break;
      case 3:
         if(!strcmp((const char*)argv[1], "start") ||
              !strcmp((const char*)argv[1], "1")){
              curm_start = 1;
              //pxa988_cti_init();
              cycle_div = conv_dec((char *)argv[2]);
              on_each_cpu(cpu_utilization_init, NULL, 1);
         }else{
            bbu_curm_help();
         }
         break;
      default:
         bbu_curm_help();
         break;
   }
}


/*..........................................................................*/
#if 0          /* Not used any more */
void cpu_cycle_idle_entry(void){
   if(curm_start){
      uint32_t count, busy;
      uint8_t core = GetCPUID()-1;
      counter_disable(CYCLE_COUNTER_ENABLE_BASE);
      count = cycle_counter_read() >> 1;
      busy  = (cycle_data.busy[core].count >> 1) +count;
      if(busy > 0x80000000){
         cycle_data.busy[core].count = (busy - 0x80000000)<<1;
         cycle_data.busy[core].overflow ++;
      }else{
         cycle_data.busy[core].count = busy<<1;
      }
      cycle_data.busy[core].overflow += cycle_data.overflow[core];
      cycle_data.overflow[core] = 0;
      cycle_counter_write(0); /* clear cycle counter */
      cycle_counter_int_enable();
      counter_enable(CYCLE_COUNTER_ENABLE_BASE);
   }
}

void cpu_cycle_idle_exit(void){
   if(curm_start){
      uint32_t count, idle;
      uint8_t core = GetCPUID()-1;
      counter_disable(CYCLE_COUNTER_ENABLE_BASE);
      count = cycle_counter_read() >> 1;
      idle  = (cycle_data.idle[core].count >> 1) + count;
      if(idle > 0x80000000){
         cycle_data.idle[core].count = (idle - 0x80000000)<<1;
         cycle_data.idle[core].overflow ++;
      }else{
         cycle_data.idle[core].count = idle << 1;
      }
      cycle_data.idle[core].overflow += cycle_data.overflow[core];
      cycle_data.overflow[core] = 0;
      cycle_counter_write(0);             /* clear cycle counter */
      cycle_counter_int_enable();
      counter_enable(CYCLE_COUNTER_ENABLE_BASE);
   }
}
#endif
