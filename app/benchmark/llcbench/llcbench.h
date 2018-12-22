/* Prototypes */

extern void flushall(int);
extern void timer_start(void);
extern void timer_stop(void);
extern double timer_elapsed(void);
#include "timer_dec.h"
/* Macros */

#define FLUSHALL(a) if (!noflush) flushall(a) 
//#define TIMER_START timer_start()
//#define TIMER_STOP timer_stop()
//#define TIMER_ELAPSED timer_elapsed()
#define TIMER_START uint32_t begin_count = timer_get_count(0)
#define TIMER_STOP  uint32_t end_count = timer_get_count(0)
#define TIMER_ELAPSED (double)(TIMER_GET_INTERVAL(end_count, begin_count))*(double)1000/(double)TIMER0_TICK_PER_MSEC

#define assert(x)  x

#ifdef INLINE
#undef INLINE
#define INLINE inline
#else
#define INLINE
#endif

#ifdef REGISTER
#undef REGISTER
#define REGISTER register
#else
#define REGISTER
#endif

//#define LLC_DEBUG
#ifdef LLC_DEBUG
#define DBG(a) a;
#else
#define DBG(a)
#endif

