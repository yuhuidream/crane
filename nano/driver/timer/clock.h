#ifndef _BBU_CLOCK_H_
#define _BBU_CLOCK_H_

#include "predefines.h"

typedef struct{
   uint8_t  day;
   uint8_t  hour;
   uint8_t  min;
   uint8_t  sec;
   uint16_t msec;
}bbu_time_t;

/*.................. get escaped time(msec) from BBU start .................*/
long  bbu_get_msec(void);

/*.................. get escaped time(sec) from BBU start ..................*/
double bbu_get_sec(void);

/*............. get string of the current clock from BBU start .............*/
void bbu_strftime(char *time_buffer);

/*................. print the current clock from BBU start .................*/
void print_current_time(void);


void bbu_update_clock(void);
void bbu_init_clock(void);
#endif