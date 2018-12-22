#ifndef _BBU_CLOCK_H_
#define _BBU_CLOCK_H_

#include "predefines.h"

typedef struct BBU_TIME_T{
   uint32_t day;
   uint32_t hour;
   uint32_t min;
   uint32_t sec;
   uint32_t msec;
   uint32_t usec;
}BBU_TIME, *P_BBU_TIME;

//get escaped time(usec) from BBU start.
uint64_t bbu_get_usec(void);

//get escaped time(msec) from BBU start
uint64_t bbu_get_msec(void);

//get escaped time(sec) from BBU start
uint64_t bbu_get_sec(void);

//get bbu system time
void bbu_get_system_time(P_BBU_TIME p_time);

//get string of the current clock from BBU start
void bbu_strftime(char *time_buffer);

//print the current clock from BBU start.
void print_current_time(void);

#endif
