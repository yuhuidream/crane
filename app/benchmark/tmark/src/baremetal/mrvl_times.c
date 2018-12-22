/*******************************************************************
 *Jolteon PJ4 PMU v7 counter code
 *David Yang, 08,11.02
 ********************************************************************/

//#include <stdio.h>
//#include <stdarg.h>
//#include <time.h>
//#include <sys/times.h>
//#include <unistd.h>
#include "gcc_counter.h"

#define PREFIX_STR "MRVL: "
/**********************************************************/
/*
 * Set default counter number.
 */  
#ifndef MRVL_COUNTER_NO
	#define MRVL_COUNTER_NO 0
#endif /* MRVL_COUNTER_NO */
extern int coreFreqMHz;
extern int measure_using_cycle;
/*
 * Set default counter type.
 */  
#ifndef MRVL_COUNTER_TYPE
	#define MRVL_COUNTER_TYPE C0_CYCLE_COUNT
#endif /* MRVL_COUNTER_TYPE */

#define MRVL_SET_COUNTER(NUM, TYPE)  set_event_PMCNT(NUM, TYPE)
#define MRVL_SET_DIVIDER(NUM)        set_counter_divider(NUM)
#define MRVL_DISABLE_COUNTERS        disable_counters
#define MRVL_ENABLE_COUNTERS         enable_counters
#define MRVL_ENABLE_OVERFLOW_FLAG    enable_overflow_flag
#define MRVL_ENABLE_INTERRUPT        enable_interrupt
#define MRVL_RESET_ALL_COUNTERS      reset_all_counters



#undef MRVL_COUNTER_NO
#define MRVL_COUNTER_NO		MRVL_COUNTER_NO2
#define MRVL_COUNTER_NO0	0
#define MRVL_COUNTER_NO1	1
#define MRVL_COUNTER_NO2	2
#define MRVL_COUNTER_NO3	3
#define MRVL_SET_DIVIDER_1      0
#define MRVL_SET_DIVIDER_16     1


/*
 * mrvl_counter_init
 * David Yang, 08,11,02
 */  
static void __attribute__((constructor)) mrvl_counter_init()
{
	MRVL_DISABLE_COUNTERS();
	MRVL_ENABLE_OVERFLOW_FLAG();
	/*
	 * initialize all event counters
	 */
#if defined(MRVL_PMU) && defined(LOOP_EVENTS)
	printf("%s, %s, %s, %s \n", mode_str[event_type[0]], mode_str[event_type[1]], mode_str[event_type[2]], mode_str[event_type[3]]);
	
	MRVL_SET_COUNTER(MRVL_COUNTER_NO0, event_type[0]);
        MRVL_SET_COUNTER(MRVL_COUNTER_NO1, event_type[1]);
        MRVL_SET_COUNTER(MRVL_COUNTER_NO2, event_type[2]);
        MRVL_SET_COUNTER(MRVL_COUNTER_NO3, event_type[3]);
#endif
	MRVL_ENABLE_INTERRUPT();
	MRVL_SET_DIVIDER(MRVL_SET_DIVIDER_1);
	/*
	 * reset all counters and enable them
	 */
	MRVL_RESET_ALL_COUNTERS();
	MRVL_ENABLE_COUNTERS();
}
/* dummy var for marvell-div */
int int_dummy;

/*
 * mrvl_counter_fini
 */ 
static _mrvl_uint32 ut[2];
static _mrvl_uint32 ccnt[2];
#ifdef MRVL_PMU
static _mrvl_uint32 counter0[2];
static _mrvl_uint32 counter1[2];
static _mrvl_uint32 counter2[2];
static _mrvl_uint32 counter3[2];
#endif //MRVL_PMU
static _mrvl_uint32 uti = 0;
static _mrvl_uint32 event_init = 0 ;


static void __attribute__((destructor)) mrvl_counter_fini()
{
#ifdef MRVL_OVERFLOW_CHECK
    int overflow_bit = read_overflow_flag();
    if ( overflow_bit & 0x80000000 )
	    fprintf (stdout, "CCNT \t overflow happened \n");
    else
	    fprintf (stdout, "CCNT \t overflow didn't happen \n");
    if ( overflow_bit & 0x1 )
	    fprintf (stdout, "Counter 0:%s \t overflow happened \n", mode_str[event_type[0]]);
    else
	    fprintf (stdout, "Counter 0:%s \t overflow didn't happen \n", mode_str[event_type[0]]);
    if ( overflow_bit & 0x2 )
	    fprintf (stdout, "Counter 1:%s \t overflow happened \n", mode_str[event_type[1]]);
    else
	    fprintf (stdout, "Counter 1:%s \t overflow didn't happen \n", mode_str[event_type[1]]);
    if ( overflow_bit & 0x4 )
	    fprintf (stdout, "Counter 2:%s \t overflow happened \n", mode_str[event_type[2]]);
    else
	    fprintf (stdout, "Counter 2:%s \t overflow didn't happen \n", mode_str[event_type[2]]);
    if ( overflow_bit & 0x8 )
	    fprintf (stdout, "Counter 3:%s \t overflow happened \n", mode_str[event_type[3]]);
    else
	    fprintf (stdout, "Counter 3:%s \t overflow didn't happen \n", mode_str[event_type[3]]);
#endif //MRVL_OVERFLOW_CHECK
    MRVL_RESET_ALL_COUNTERS();
}

/*
 * times
 */  
extern unsigned int bkey ;
_mrvl_uint32 mrvl_times ()
{
	_mrvl_uint32 local_time=0;
	(void)event_type;
	(void)event_init;
	if (( uti == 0 )&& (bkey == 1 ))
	{
#ifdef  MRVL_PMU
#ifdef  LOOP_EVENTS
		event_type[0]=common_event[event_init++];
		event_type[1]=common_event[event_init++];
		event_type[2]=common_event[event_init++];
		event_type[3]=common_event[event_init++];
#endif  //LOOP_EVENTS
#endif  //MRVL_PMU

		bkey = 0 ;
		mrvl_counter_init();

	}
	//__buffer->tms_utime = ut[uti] = MRVL_GET_COUNTER(MRVL_COUNTER_NO);
#ifdef  MRVL_PMU	
	get_all_counters(ccnt, counter0, counter1, counter2, counter3, uti);
#else
	get_ccnt(ccnt, uti);
#endif  //MRVL_PMU

	local_time = ut[uti] = ccnt[uti];

	uti ^= 1; /* 0->1->0... */
	if( uti == 0 )
		mrvl_counter_fini();
#ifdef MRVL_PMU
if ( uti == 0 )
{
    /*
    fprintf (stdout, PREFIX_STR "CCNT:               %d\n", ccnt[1]-ccnt[0]);
    fprintf (stdout, PREFIX_STR "Counter 0:%s        %d\n", mode_str[event_type[0]], counter0[1]-counter0[0]);
    fprintf (stdout, PREFIX_STR "Counter 1:%s        %d\n", mode_str[event_type[1]], counter1[1]-counter1[0]);
    fprintf (stdout, PREFIX_STR "Counter 2:%s        %d\n", mode_str[event_type[2]], counter2[1]-counter2[0]);
    fprintf (stdout, PREFIX_STR "Counter 3:%s        %d\n", mode_str[event_type[3]], counter3[1]-counter3[0]);
    */
#ifdef  LOOP_EVENTS
    fprintf (stdout, " %10d, %10d, %10d, %10d, ", counter0[1]-counter0[0], counter1[1]-counter1[0], counter2[1]-counter2[0], counter3[1]-counter3[0]);
#endif
}
#else
#ifdef MRVL_VERBOSE
if ( uti == 0 )
    fprintf (stdout, PREFIX_STR "CCNT:               %d\n", ccnt[1]-ccnt[0]);
#endif //MRVL_VERBOSE
#endif //MRVL_PMU
	if(measure_using_cycle)//#ifdef BAREMETAL_NO_CYCLE
	{
        int_dummy = local_time / bkey;
	return local_time;
	}
	else //#else
	{
        int_dummy = local_time / bkey;
	return local_time / coreFreqMHz;
	}
//#endif
}

#ifdef MRVL_COUNTER_PJ4_LINUX
#include "./mrvlinit_counter/gcc/linux-2.6.23.1-mrv-l2/mrv_module/pcd/user_library/gcc_counter.c"
/**********************************************************/
#endif /* Use Marvell performance counter */

