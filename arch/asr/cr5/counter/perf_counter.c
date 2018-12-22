#include "predefines.h"
#include "perf_counter.h"
#include "cache_ops.h"
#include "cp15.h"
#include <string.h>

uint32_t perf_counters[COUNTER_MAX] = {FREE};

static EventConfig event_cfg[] = 
{
   {CYCLE_COUNT,         0x00000011},
   {D_CACHE_READ_HIT,    0x00000004},
   {D_CACHE_READ_MISS,   0x00000003},
   {D_CACHE_WRITE_HIT,   0x00000004},
   {D_CACHE_WRITE_MISS,  0x00000003},
   {I_CACHE_READ_MISS,   0x00000001},
   {L2D_CACHE_ACCESS,    0x00000016},
   {L2D_CACHE_REFILL,    0x00000017},
   {L2D_CACHE_WB,        0x00000018},
   {NO_EVENT,            0x00000000},
};

static uint32_t get_event_config(EventType event)
{
   int i = 0;
   while(event_cfg[i].event_type != (uint32_t)NO_EVENT)
   {
      if(event_cfg[i].event_type == (uint32_t)event)
         return event_cfg[i].event_cfg;
      i++;
   }
   return (uint32_t)MV_ERROR;
}


/******************* Performance Counters operation functions ***************/
/* Just configure the cp15 performance counters, no include SL2C counters */
int ConfigPerformanceCounter(EventType event, Counters *counter_idx)
{
   int i;
   if(event >= NO_EVENT)
      return MV_ERROR;
   for(i = COUNTER_MAX -1; i >= 0; i--)
   {
      if(perf_counters[i] == FREE)
      {
         uint32_t config_val = get_event_config(event);
         perf_counter_select((Counters)i);
         perf_counter_event_select(config_val);
         perf_counters[i] = BUSY;
         *counter_idx = (Counters)i;
         return MV_OK;         
      }
   }
   return MV_ERROR;
}

/* ........................................................................ */
int SetCounter(Counters counter_idx, uint32_t value)
{
   if(counter_idx >= COUNTER_MAX || perf_counters[counter_idx] == FREE)
      return MV_ERROR;
   perf_counter_select(counter_idx);
   perf_counter_write(value);
   return MV_OK;
}

/* ........................................................................ */
int StartCounter(Counters counter_idx)
{
   if(counter_idx >= COUNTER_MAX || perf_counters[counter_idx] == FREE)
      return MV_ERROR;
   perf_counters_enable();
   counter_enable(counter_idx);
   return MV_OK;
}

/* ........................................................................ */
int StopCounter(Counters counter_idx)
{
   if(counter_idx >= COUNTER_MAX || perf_counters[counter_idx] == FREE)
      return MV_ERROR;
   counter_disable(counter_idx);
   return MV_OK;
}

/* ........................................................................ */
int ReadCounter(Counters counter_idx, uint32_t *value)
{
   if(counter_idx >= COUNTER_MAX)
      return MV_ERROR;
   perf_counter_select(counter_idx);
   *value = perf_counter_read();
   return MV_OK;
}

/* ........................................................................ */
int ReleaseCounter(Counters counter_idx)
{
   if(counter_idx >= COUNTER_MAX || perf_counters[counter_idx] == FREE)
      return MV_ERROR;
   perf_counter_select(counter_idx);
   perf_counter_event_select(0x0);
   perf_counters[counter_idx] = FREE;
   return MV_OK;
}

/* ........................................................................ */
void StartAllCounters(void)
{
   uint32_t reg_val;
   perf_counters_enable();
   cp15_reg_read(c9, c12, 1, reg_val);
   /* 0xF: bit[0~3] CA7 only has 4 performance counters */
   cp15_reg_write(c9, c12, 1, reg_val | 0xF);
}

/* ........................................................................ */
void StopAllCounters(void)
{
   uint32_t reg_val;
   /* 0xF: bit[0~3] CA7 only has 4 performance counters */
   cp15_reg_read(c9, c12, 2, reg_val);
   cp15_reg_write(c9, c12, 2, reg_val | 0xF);
}

/************************* Cycle counter operation functions ****************/
void StartCycleCounter(void)
{
   perf_counters_enable();
   cycle_counter_divider_clear();
   counter_enable(CYCLE_COUNTER_ENABLE_BASE);
}

void SetCycleCounter(uint32_t value)
{
   StopCycleCounter();
   cycle_counter_write(value);
}

uint32_t ReadCycleCounter(void)
{
   return cycle_counter_read();
}

void ReleaseCycleCounter(void)
{
   return;
}

void StopCycleCounter(void)
{
   counter_disable(CYCLE_COUNTER_ENABLE_BASE);
}

/***************************** basic functions ******************************/
void counter_enable(uint32_t counter_idx)
{
   uint32_t reg_val;
   /* PMCNTENSET, Performance Monitors Count Enable Set register */
   cp15_reg_read(c9, c12, 1, reg_val);
   reg_val |= (1<<counter_idx);
   cp15_reg_write(c9, c12, 1, reg_val);
}

void counter_disable(uint32_t counter_idx)
{
   uint32_t reg_val;
   /* PMCNTENCLR, Performance Monitors Count Enable Clear register */
   cp15_reg_read(c9, c12, 2, reg_val);
   reg_val |= (1<<counter_idx);
   cp15_reg_write(c9, c12, 2, reg_val);
}

int is_cycle_counter_enabled(void)
{
   uint32_t reg_val;
   /* PMCNTENCLR, Performance Monitors Count Enable Clear register */
   cp15_reg_read(c9, c12, 1, reg_val);
   return (reg_val & CYCLE_COUNTER_ENABLE);
}

void counter_disable_all(void){
   cp15_reg_write(c9, c12, 2, 0xFFFFFFFF);
}

void cycle_counter_divider_set(void)
{
   uint32_t reg_val;
   /* PMCR, Performance Monitors Control Register */
   cp15_reg_read(c9, c12, 0, reg_val);
   reg_val |= CYCLE_COUNTER_DIVIDER_SET;  /* Bit 3 */
   cp15_reg_write(c9, c12, 0, reg_val);
}

int is_cycle_counter_divider_set(void)
{
   uint32_t reg_val;
   /* PMCR, Performance Monitors Control Register */
   cp15_reg_read(c9, c12, 0, reg_val);
   return (reg_val & CYCLE_COUNTER_DIVIDER_SET);
}

void cycle_counter_divider_clear(void)
{
   uint32_t reg_val;
   /* PMCR, Performance Monitors Control Register */
   cp15_reg_read(c9, c12, 0, reg_val);
   reg_val &= ~CYCLE_COUNTER_DIVIDER_SET;  /* Bit 3 */
   cp15_reg_write(c9, c12, 0, reg_val);
}

uint32_t cycle_counter_read(void)
{
   uint32_t reg_val;
   /* PMCCNTR, Performance Monitors Cycle Count Register, */
   cp15_reg_read(c9, c13, 0, reg_val);
   return reg_val;
}

void cycle_counter_write(uint32_t value)
{
   /* PMCCNTR, Performance Monitors Cycle Count Register, */
   cp15_reg_write(c9, c13, 0, value);
}

void cycle_counter_reset(void)
{
   uint32_t reg_val;
   cp15_reg_read(c9, c12, 0, reg_val);
   reg_val |= CYCLE_COUNTER_RESET;
   cp15_reg_write(c9, c12, 0, reg_val);
}

void perf_counters_enable(void)
{
   uint32_t reg_val;
   /* PMCR, Performance Monitors Control Register */
   cp15_reg_read(c9, c12, 0, reg_val);
   reg_val |= COUNTERS_ALL_ENABLE;
   cp15_reg_write(c9, c12, 0, reg_val);
}

void perf_counters_disable(void)
{
   uint32_t reg_val;
   cp15_reg_read(c9, c12, 0, reg_val);
   reg_val &= ~COUNTERS_ALL_ENABLE;
   cp15_reg_write(c9, c12, 0, reg_val);
}

void perf_counters_reset(void)
{
   uint32_t reg_val;
   cp15_reg_read(c9, c12, 0, reg_val);
   reg_val |= EVENT_COUNTERS_RESET;
   cp15_reg_write(c9, c12, 0, reg_val);
}

void perf_counter_select(uint32_t counter_idx)
{
   /* PMSELR, Performance Monitors Event Counter Selection Register */
   cp15_reg_write(c9, c12, 5, counter_idx);
}

void perf_counter_event_select(uint32_t event_idx)
{
   /* PMXEVTYPER, Performance Monitors Event Type Select Register */
   cp15_reg_write(c9, c13, 1, event_idx);
}

uint32_t perf_counter_read(void)
{
   uint32_t value;
   /* PMXEVCNTR, Performance Monitors Event Count Register */
   cp15_reg_read(c9, c13, 2, value);
   return value;
}

void perf_counter_write(uint32_t value)
{
   /* PMXEVCNTR, Performance Monitors Event Count Register */
   cp15_reg_write(c9, c13, 2, value);
}

void events_export_enable(void)
{
   uint32_t reg_val;
   cp15_reg_read(c9, c12, 0, reg_val);
   reg_val |= EVENTS_EXPORT_ENABLE;
   cp15_reg_write(c9, c12, 0, reg_val);
}

void events_export_disable(void)
{
  uint32_t reg_val;
   cp15_reg_read(c9, c12, 0, reg_val);
   reg_val &= ~EVENTS_EXPORT_ENABLE;
   cp15_reg_write(c9, c12, 0, reg_val);
}

void perf_counter_int_enable(uint32_t counter_idx)
{
   cp15_reg_write(c9, c14, 1, (1<<counter_idx));
}

void perf_counter_int_disable(uint32_t counter_idx)
{
   cp15_reg_write(c9, c14, 2, (1<<counter_idx));
}

void cycle_counter_int_enable(void)
{
   cp15_reg_write(c9, c14, 1, BIT_31);
}

void cycle_counter_int_disable(void)
{
   cp15_reg_write(c9, c14, 2, BIT_31);
}

int perf_counter_overflowed(uint32_t counter_idx)
{
   uint32_t value;
   cp15_reg_read(c9, c12, 3, value);
   if(value & (1<<counter_idx))
      return 1;
   return 0;
}

void perf_counter_clear_overflow(uint32_t counter_idx)
{
   cp15_reg_write(c9, c12, 3, (1<<counter_idx));
}

int cycle_counter_overflowed(void)
{
   uint32_t value;
   cp15_reg_read(c9, c12, 3, value);
   if(value & BIT_31)
      return 1;
   return 0;
}

void cycle_counter_clear_overflow(void)
{
   cp15_reg_write(c9, c12, 3, BIT_31);
}

void counters_clear_overflow_all(void){
   cp15_reg_write(c9, c12, 3, 0xFFFFFFFF);
}

/* ........................................................................ */
int getCounterOverhead(Counters counter_idx , uint32_t* overhead)
{
   int ret;
   
   ret = SetCounter(counter_idx, 0x0);

   #if 0
   l1_dcache_clean_all();
   l2_cache_clean_all();
   l1_dcache_invalidate_all();
   l2_cache_invalidate_all();
   l1_icache_invalidate_all();
   #endif // check if needed

   StartAllCounters();
   StopAllCounters();
   ret = ReadCounter(counter_idx, overhead);
   ret = SetCounter(counter_idx,  0x0);

   return ret;
}

/*..........................................................................*/
void nezha2_cr5_pmu_init(void){

   counter_disable_all();
   perf_counters_reset();
   cycle_counter_reset();
   counters_clear_overflow_all();
}
