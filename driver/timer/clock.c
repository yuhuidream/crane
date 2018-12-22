#include "predefines.h"
#include "timer_dec.h"
#include "RTC.h"

/* These two macros need to be defined according to different timers */
#define COUNT_PER_USEC    (26)
#define COUNT_PER_MSEC    (26000)
#define COUNT_PER_SEC     (26000000)

void print_current_time(void){
	uint64_t total_usec_from_start = generic_timer_read()/COUNT_PER_USEC;
	uint32_t hour = total_usec_from_start/3600000000;
	uint32_t min  = (total_usec_from_start%3600000000)/60000000;
	uint32_t sec  = (total_usec_from_start%60000000)/1000000;
	uint32_t msec = (total_usec_from_start%1000000)/1000;
	uint32_t usec = total_usec_from_start%1000;
	bbu_printf("\n\r%02d:%02d:%02d.%03d.%03d from BBU start\n\r", 
	           hour, min, sec, msec, usec);
               
	return;
}

uint64_t bbu_get_usec(void){
	uint64_t total_usec_from_start = generic_timer_read();
	
	return (total_usec_from_start/COUNT_PER_USEC);
}

uint64_t bbu_get_msec(void){
	uint64_t total_usec_from_start = generic_timer_read();
	uint64_t msec = (long)(total_usec_from_start/COUNT_PER_MSEC);
	
	return msec;
}

uint64_t bbu_get_sec(void){
	uint64_t total_usec_from_start = generic_timer_read();
	uint64_t sec = (long)(total_usec_from_start/COUNT_PER_SEC);
	
	return sec;
}

void bbu_get_system_time(P_BBU_TIME p_time){
	if(p_time == NULL){
		bbu_printf("The input time point struct is invlid\r\n");
		return;
	}
	
	BBU_TIME t;
	uint64_t total_usec = generic_timer_read()/COUNT_PER_USEC;
	t.day  = (total_usec%5184000000000)/216000000000;
	t.hour = (total_usec%216000000000)/3600000000;
	t.min  = (total_usec%3600000000)/60000000;
	t.sec  = (total_usec%60000000)/1000000;
	t.msec = (total_usec%1000000)/1000;
	t.usec = total_usec%1000;

	memcpy(p_time, &t, sizeof(BBU_TIME));

	return;
}

extern int bbu_sprintf (char *out, const char *format, ...);

void bbu_strftime(char *time_buffer){
	uint64_t total_usec_from_start = generic_timer_read()/COUNT_PER_USEC;
	uint32_t hour = total_usec_from_start/3600000000;
	uint32_t min  = (total_usec_from_start%3600000000)/60000000;
	uint32_t sec  = (total_usec_from_start%60000000)/1000000;
	uint32_t msec = (total_usec_from_start%1000000)/1000;
	uint32_t usec = total_usec_from_start%1000;
	bbu_sprintf(time_buffer, "BBU TIME: %02d:%02d:%02d.%04d.%04d",
	       hour, min, sec, msec, usec);

	return;
}

