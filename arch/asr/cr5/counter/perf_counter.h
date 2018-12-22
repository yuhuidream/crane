#ifndef _H_PERFORMANCE_COUNTER_H_
#define _H_PERFORMANCE_COUNTER_H_

#include "bbu.h"

typedef struct CycleCountTAG{
   uint32_t  count;
   uint32_t  overflow;
}CycleCount;

typedef struct CycleCntDataTAG{
   CycleCount total[CORE_NUM];
   CycleCount idle[CORE_NUM];
   CycleCount busy[CORE_NUM];
   CycleCount ao[CORE_NUM][QF_MAX_ACTIVE+1];
   uint32_t   prev[CORE_NUM];
   uint32_t   overflow[CORE_NUM];
}CycleCntData;

typedef enum{
	COUNTER0 = 0,
	COUNTER1,
	COUNTER2,           /* CR5 only has four performance counters */
	COUNTER_MAX
}Counters;

typedef enum{
	CYCLE_COUNT = 0,
	D_CACHE_READ_HIT,
	D_CACHE_READ_MISS,
	D_CACHE_WRITE_HIT,
	D_CACHE_WRITE_MISS,
	I_CACHE_READ_MISS,
	L2D_CACHE_ACCESS,
	L2D_CACHE_REFILL,
	L2D_CACHE_WB,
	NO_EVENT
}EventType;

typedef struct{
	uint32_t event_type;
	uint32_t event_cfg;
}EventConfig;

#define FREE 0
#define BUSY 1

#define EVENTS_EXPORT_ENABLE        BIT_4
#define CYCLE_COUNTER_DIVIDER_SET   BIT_3
#define CYCLE_COUNTER_RESET         BIT_2
#define EVENT_COUNTERS_RESET        BIT_1
#define COUNTERS_ALL_ENABLE         BIT_0
#define CYCLE_COUNTER_ENABLE_BASE   31
#define CYCLE_COUNTER_ENABLE        BIT_31

/******************* Performance Counters operation functions ***************/
int ConfigPerformanceCounter(EventType event, Counters *counter_idx);
int SetCounter(Counters counter_idx, uint32_t value);
int StartCounter(Counters counter_idx);
int StopCounter(Counters counter_idx);
int ReadCounter(Counters counter_idx, uint32_t *value);
int ReleaseCounter(Counters counter_idx);
void StartAllCounters(void);
void StopAllCounters(void);
int getCounterOverhead(Counters counter_idx , uint32_t* overhead);

/************************* Cycle counter operation functions ****************/
void StartCycleCounter(void);
void SetCycleCounter(uint32_t value);
uint32_t ReadCycleCounter(void);
void ReleaseCycleCounter(void);
void StopCycleCounter(void);

/***************************** basic functions ******************************/
void counter_enable(uint32_t counter_idx);
void counter_disable(uint32_t counter_idx);
void counter_disable_all(void);
void cycle_counter_reset(void);
int is_cycle_counter_enabled(void);
int is_cycle_counter_divider_set(void);
void cycle_counter_divider_set(void);
void cycle_counter_divider_clear(void);
uint32_t cycle_counter_read(void);
void cycle_counter_write(uint32_t value);
void perf_counters_enable(void);
void perf_counters_disable(void);
void perf_counters_reset(void);
void perf_counter_select(uint32_t counter_idx);
void perf_counter_event_select(uint32_t event_idx);
uint32_t perf_counter_read(void);
void perf_counter_write(uint32_t value);
void events_export_enable(void);
void events_export_disable(void);
void perf_counter_int_enable(uint32_t counter_idx);
void perf_counter_int_disable(uint32_t counter_idx);
void cycle_counter_int_enable(void);
void cycle_counter_int_disable(void);
int perf_counter_overflowed(uint32_t counter_idx);
void perf_counter_clear_overflow(uint32_t counter_idx);
int cycle_counter_overflowed(void);
void cycle_counter_clear_overflow(void);
void counters_clear_overflow_all(void);
void pxa988_perf_mon_unit_init(void);


#define L1D_RW_HMR_STAT_  \
      uint32_t read_hit = 0, read_miss = 0, write_hit=0, write_miss=0;   \
      Counters read_hit_counter, read_miss_counter, write_hit_counter, write_miss_counter;

#define L1D_RW_HMR_BEGIN_()   \
   do{  \
      ConfigPerformanceCounter(D_CACHE_READ_HIT, &read_hit_counter);   \
      ConfigPerformanceCounter(D_CACHE_READ_MISS, &read_miss_counter);  \
      ConfigPerformanceCounter(D_CACHE_WRITE_HIT, &write_hit_counter);   \
      ConfigPerformanceCounter(D_CACHE_WRITE_MISS, &write_miss_counter);  \
      SetCounter(read_hit_counter, 0);   \
      SetCounter(read_miss_counter, 0);  \
      SetCounter(write_hit_counter, 0);  \
      SetCounter(write_miss_counter, 0);  \
      StartCounter(read_hit_counter);   \
      StartCounter(read_miss_counter);   \
      StartCounter(write_hit_counter);   \
      StartCounter(write_miss_counter);   \
   }while(0)

#define L1D_RW_HMR_END_()   \
   do{   \
      StopCounter(read_hit_counter);      \
      StopCounter(read_miss_counter);     \
      StopCounter(write_hit_counter);     \
      StopCounter(write_miss_counter);    \
      ReadCounter(read_hit_counter, &read_hit);    \
      ReadCounter(read_miss_counter, &read_miss);     \
      ReadCounter(write_hit_counter, &write_hit);     \
      ReadCounter(write_miss_counter, &write_miss);      \
      ReleaseCounter(read_hit_counter);      \
      ReleaseCounter(read_miss_counter);     \
      ReleaseCounter(write_hit_counter);     \
      ReleaseCounter(write_miss_counter);    \
      bbu_printf("Read Hit Time: %d\n\r", read_hit);   \
      bbu_printf("Read Miss Time: %d\n\r", read_miss);  \
      bbu_printf("Write Hit Time: %d\n\r", write_hit);   \
      bbu_printf("Write Miss Time: %d\n\r", write_miss);   \
      bbu_printf("Read Hit Rate: %6.4f\%\n\r", ((float)read_hit/(float)(read_hit+read_miss))*100); \
      bbu_printf("Read Hit Rate: %6.4f\%\n\r", ((float)write_hit/(float)(write_hit+write_miss))*100); \
   }while(0)

//bbu_printf("****** L1D Cache Read/Write Hit/Miss Rate Measurement End ******\n\r");  

#define L2_RW_HMR_STAT_  \
      uint32_t l2_access = 0, l2_refill = 0;   \
      Counters l2_access_counter, l2_refill_counter;

#define L2_RW_HMR_BEGIN_()  \
   do{  \
      ConfigPerformanceCounter(L2D_CACHE_ACCESS, &l2_access_counter);   \
      ConfigPerformanceCounter(L2D_CACHE_REFILL, &l2_refill_counter);  \
      SetCounter(l2_access_counter, 0);   \
      SetCounter(l2_refill_counter, 0);  \
      StartCounter(l2_access_counter);   \
      StartCounter(l2_refill_counter);   \
   }while(0)

#define L2_RW_HMR_END_()   \
   do{   \
      StopCounter(l2_access_counter);      \
      StopCounter(l2_refill_counter);     \
      ReadCounter(l2_access_counter, &l2_access);    \
      ReadCounter(l2_refill_counter, &l2_refill);     \
      ReleaseCounter(l2_access_counter);      \
      ReleaseCounter(l2_refill_counter);     \
      bbu_printf("L2 Cache Access Time: %d\n\r", l2_access);   \
      bbu_printf("L2 Cache Refill Time: %d\n\r", l2_refill);  \
   }while(0)

#endif
