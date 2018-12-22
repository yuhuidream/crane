/* $Id: cachebench.c,v 1.9 2008/05/23 12:52:47 mucci Exp $ */

#include "bbu_malloc.h"
#include "llcbench.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define ARRAY(x,i,j,m) *(x+((j)*m)+(i))

#if defined(USE_DOUBLE)
#ifndef DATATYPE
#define DATATYPE double
#define DATASTRING "double"
#endif
#elif defined(USE_INT)
#ifndef DATATYPE
#define DATATYPE int
#define DATASTRING "int"
#endif
#elif defined(USE_CHAR)
#ifndef DATATYPE
#define DATATYPE char
#define DATASTRING "char"
#endif
#elif defined(USE_FLOAT)
#ifndef DATATYPE
#define DATATYPE float
#define DATASTRING "float"
#endif
#endif

#ifndef DATATYPE
#define DATATYPE double
#define DATASTRING "double"
#endif

#define READONLY 1<<0
#define WRITEONLY 1<<1
#define READWRITE 1<<2
#define NOCALIBRATE 1<<3
#define GUESSCACHESIZE 1<<4
#define MEMORYSET 1<<5
#define MEMORYCOPY 1<<6
#define HANDTUNED 1<<7

/* DEFAULT SETTINGS */

#define CACHE_MIN_BITS (8) /* 1<<bits is smallest cache in bytes */
#define CACHE_MAX_BITS (24) /* 1<<bits is largest cache in bytes */
#define CACHE_MIN (1L<<CACHE_MIN_BITS)
#define CACHE_MAX (1L<<CACHE_MAX_BITS)
#define RESOLUTION (2)
#define TIME_SLOTS (2L*(CACHE_MAX_BITS-CACHE_MIN_BITS+1))
#define MEASURE_TIME 5
#define DOUBLE_NS(x) ((double)x)*1.0e9
#define THRESHOLD (0.9) /* 1st level must be faster than 2nd level */
#define REPEAT_COUNT 2

extern char *optarg;

static long logmemsize = CACHE_MAX_BITS, memsize = 1L<<CACHE_MAX_BITS, repeat_count = REPEAT_COUNT;
static long resolution = RESOLUTION, timeslots, duration = MEASURE_TIME, type = NOCALIBRATE;
static long noflush = 0;
static int timer_used;

volatile long keepgoing = 0;

static int my_isatty(int fd)
{
  (void)fd;
  return 0;    /* Force results to flush quicker. */
  //return isatty(fd);
}

long go(long a)
{
  if (a==-1)
    return(keepgoing);
  else
    return(keepgoing = a);
}

#if 0
void handler(int a)
{
  free_timer(timer_used);
  go(0);
}
#endif
extern void free_timer(int index);

void handler(void)
{
  free_timer(timer_used);
  go(0);
}

void compute_stats(long i, long j, double refcnt, double overhead_per_ref,
		   double *times, double *bws, double *percents, long *sizes,
		   double tmicrosec, long datasize)
{
  if (tmicrosec > 0.0)
    {
  ARRAY(times,i,j,repeat_count) = 
    ((tmicrosec*1000.0) - (refcnt*overhead_per_ref))/refcnt;
  
  ARRAY(bws,i,j,repeat_count) = 
    ((refcnt*datasize)/(1024.0*1024.0)) / 
    ((ARRAY(times,i,j,repeat_count)*refcnt)/1.0E9);
    }

  if (i == 0)
    {
      if (j == 0)
	ARRAY(percents,i,j,repeat_count) = 1.0;
      else
	{
	  ARRAY(percents,i,j,repeat_count) = 
	    ARRAY(times,i,j,repeat_count) / 
	    ARRAY(times,i,j-1,repeat_count);
	}
    }
  else
    {
      if (j == 0)
	{
	  ARRAY(percents,i,j,repeat_count) = 
	    ARRAY(times,i,j,repeat_count) / 
	    ARRAY(times,i-1,repeat_count-1,repeat_count);
	}
      else
	{
	  ARRAY(percents,i,j,repeat_count) = 
	    ARRAY(times,i,j,repeat_count) / 
	    ARRAY(times,i,j-1,repeat_count);
	}
    }

  if (my_isatty(1))
    {
      bbu_printf("%-15ld %-15.2f %-15.2f %-15.2f\n\r", 
	       sizes[i],
	       ARRAY(times,i,j,repeat_count),
	       ARRAY(bws,i,j,repeat_count),
	       ARRAY(percents,i,j,repeat_count));
    }
  else
    bbu_printf("%ld %10.6f\n\r", 
	   sizes[i],
	   ARRAY(bws,i,j,repeat_count));
}

void compute_cache_sizes(double *times, double *bws, double *percents, long *sizes)
{
  long i, maxb = 0, maxa = 0, cachea, cacheb;
  (void)bws;

  /* Look for 2 highest percentages */

  for (i=0; i < timeslots; i++)
    if (percents[i] > percents[maxa])
      maxa = i;

  for (i=0; i < timeslots; i++)
    {
      if ((i != maxa) && (percents[i] > percents[maxb]))
	{
	  maxb = i;
	}
    }

  bbu_printf("\n\r\t\t\tCache Capacity Analysis\n\r\n\r");
  
  if (1.0/percents[maxa] >= THRESHOLD)
    {
      bbu_printf("No L1 limit found, must be larger than %ld bytes.\n\r",memsize);
      return;
    }
    
  /* Set them to the index if the entry for that cache size */
  /* Remember our percents are relative to the previous entry */

  cachea = sizes[maxa-1];
  cacheb = sizes[maxb-1];

  if (cachea > cacheb)
    bbu_printf("Level 1 Cache: %ld bytes\n\r",cachea);
  else
    {
      bbu_printf("Level 1 Cache: %ld bytes\n\r",cachea);
      if ((times[maxa]/times[maxb] < THRESHOLD) && (1.0/percents[maxb] < THRESHOLD))
	bbu_printf("Level 2 Cache: %ld bytes\n\r",cacheb);
    }
}

long usage(long argc, char **argv)
{
  long c;
  long errflg = 0;

  while ((c = getopt(argc, argv, "m:d:hrwbcsptx:e:N")) != -1)
    switch (c) {
    case 'm':
      if (((logmemsize = atol(optarg)) < 0) || (logmemsize <= CACHE_MIN_BITS))
	errflg++;
      memsize = 1L<<logmemsize;
      break;
    case 'd':
      if ((duration = atoi(optarg)) < 0)
	errflg++;
      break;
    case 's':
      type |= MEMORYSET;
      break; 
    case 'p':
      type |= MEMORYCOPY;
      break; 
    case 'r':
      type |= READONLY;
      break; 
    case 'h':
      errflg++;
      break; 
    case 't':
      type |= HANDTUNED;
      break;
    case 'w':
      type |= WRITEONLY;
      break; 
    case 'b':
      type |= READWRITE;
      break; 
    case 'c':
      type ^= NOCALIBRATE;
      break; 
    case 'g':
      type |= GUESSCACHESIZE;
      break; 
    case 'x':
      if ((resolution = atoi(optarg)) < 0)
	errflg++;
      resolution++; /* Correct for my usage */
      break; 
    case 'e':
      if ((repeat_count = atoi(optarg)) <= 0)
	errflg++;
      break; 
    case 'N':
      noflush = 1;
      break; 
    case '?':
      errflg++;
      break; }

  if ((type & (READONLY|WRITEONLY|READWRITE|MEMORYCOPY|MEMORYSET)) == 0)
    {
      if (my_isatty(1))
	type |= READONLY|WRITEONLY|READWRITE|MEMORYCOPY|MEMORYSET;
      else
	type |= READWRITE;
    }
  
  if (errflg) 
    {
      bbu_printf("Usage: %s -rwbtsp [-x #] [-m #] [-d #] [-e #]\n\r",argv[0]);
      bbu_printf("\t -r Read benchmark\n\r");
      bbu_printf("\t -w Write benchmark\n\r");
      bbu_printf("\t -b Read/Modify/Write benchmark\n\r");
      bbu_printf("\t -t Use hand tuned versions of the above\n\r");
      bbu_printf("\t -s memset() benchmark\n\r");
      bbu_printf("\t -p memcpy() benchmark\n\r");
      /* bbu_printf("\t -c Enable calibration code\n\r"); 
      bbu_printf("\t -g Enable cache size guessing code\n\r"); */
      bbu_printf("\t -x Number of measurements between powers of 2.\n\r");
      bbu_printf("\t -m Specify the log2(available physical memory)\n\r");
      bbu_printf("\t -d Number of seconds per iteration\n\r");
      bbu_printf("\t -e Repeat count per cache size\n\r\n\r");
      bbu_printf("\t -N Do not flush the cache between problem sizes.\n\r");
      bbu_printf("Datatype used is %s, %ld bytes.\n\r",DATASTRING,(long)sizeof(DATATYPE));
      bbu_printf("Defaults if  tty: -rwbsp -x%d -m%d -d%d -e%d\n\r",
	      RESOLUTION-1,CACHE_MAX_BITS,MEASURE_TIME,REPEAT_COUNT);
      bbu_printf("Defaults if file: -b   -x%d -m%d -d%d -e1\n\r",
	      RESOLUTION-1,CACHE_MAX_BITS,MEASURE_TIME);
      //exit(1);
      return -1;
    }

  timeslots = resolution*(logmemsize - CACHE_MIN_BITS) + 1;

  DBG(bbu_printf("%ld %ld %ld %ld\n\r",logmemsize,memsize,duration,timeslots)); 

  return(type);
}

#if 0
void fake_out_optimizations(DATATYPE *x, long bytes)
{
  static long fd = -1;

  if (fd == -1)
    assert(fd=open("/dev/null",O_WRONLY));
  assert(write(fd,(void *)x,bytes>sizeof(DATATYPE)?sizeof(DATATYPE):bytes)!=-1);
}
#endif

/* double calibrate_benchmark_ronly(REGISTER DATATYPE *x, REGISTER long limit, long *oloops, double *ous)
{
  REGISTER long index = 0, loops = 0;
  REGISTER DATATYPE sum = 0, foolem = 0;
  
  flushall(x);
  keepgoing = 1;
  //assert(signal(SIGALRM,handler) != SIG_ERR);
  limit -= 4; foolem = (DATATYPE)limit;

  //alarm(duration);
  TIMER_START;
again:
  sum += foolem + foolem+(DATATYPE)1 + foolem+(DATATYPE)2 + foolem+(DATATYPE)3;
  if (((index+=4) < limit) && (keepgoing))
    goto again;
  else if (keepgoing)
    {
      index = 0;
      loops++;
      goto again;
    }
  TIMER_STOP;
  index += 4;

  x[0] = (DATATYPE)sum;
  x[1] = (DATATYPE)index;
  //fake_out_optimizations(x,2*sizeof(DATATYPE));

  *oloops = loops;
  *ous = TIMER_ELAPSED;
  return(((double)loops*(double)limit)+(double)index);  
}

double calibrate_benchmark_wonly(REGISTER DATATYPE *x, REGISTER long limit, long *oloops, double *ous)
{
  REGISTER long index = 0, loops = 0;
  REGISTER DATATYPE sum1 = (DATATYPE)0, sum2 = (DATATYPE)0, sum3 = (DATATYPE)0, sum4 = (DATATYPE)0;

  FLUSHALL(1);
  keepgoing = 1;
  //assert(signal(SIGALRM,handler) != SIG_ERR);
  limit -= 4; 

  //alarm(duration);
  TIMER_START;
again:
  sum1++;
  sum2++;
  sum3++;
  sum4++;
  if (((index+=4) < limit) && (keepgoing))
    goto again;
  else if (keepgoing)
    {
      index = 0;
      loops++;
      goto again;
    }
  TIMER_STOP;
  index += 4;

  x[0] = (DATATYPE)sum1;
  x[1] = (DATATYPE)sum2;
  //fake_out_optimizations(x,2*sizeof(DATATYPE));
    
  return(((double)loops*(double)limit)+(double)index);
} 

double calibrate_benchmark(REGISTER DATATYPE *x, REGISTER long to_do_loops, REGISTER long limit, double *ous)
{
  REGISTER long index = 0, loops = 0;
  REGISTER DATATYPE sum1 = (DATATYPE)0;

  TIMER_START;
  while (loops < to_do_loops)
    {
      for (index = 0; index < limit; index++)
	{
	  sum1++;
	}
      loops++;
    }
  TIMER_STOP;

  x[0] = (DATATYPE)sum1;
  x[1] = (DATATYPE)index;
  //fake_out_optimizations(x,2*sizeof(DATATYPE));
    
  *ous = TIMER_ELAPSED;
  {
    double refcnt = ((double)loops*(double)limit)+(double)index;
    DBG(bbu_printf("C: %ld loops at limit %ld took %f us, %f refs.\n\r",loops,limit,*ous,refcnt));
    return(refcnt);
  }
} */

double benchmark_cache_ronly(REGISTER DATATYPE *x, REGISTER long limit, long *oloops, double *ous)
{
  REGISTER long index = 0, loops = 0;
  REGISTER DATATYPE sum = (DATATYPE)0;

  FLUSHALL(1);
  keepgoing = 1;
  //assert(signal(SIGALRM,handler) != SIG_ERR);

  //alarm(duration);
  timer_used = start_timer(1000*duration, handler, TIMER_ONE_SHOT);
  TIMER_START;

#ifdef SOLARIS
  while (go(-1))
#else
  while (keepgoing)
#endif  
    {
      for  (index = 0; index < limit; index++)
	{
	  sum += x[index];
	}
      loops++;
    }

  TIMER_STOP;

  x[0] = (DATATYPE)sum;
  x[1] = (DATATYPE)index;
  //fake_out_optimizations(x,2*sizeof(DATATYPE));

  *oloops = loops;
  *ous = TIMER_ELAPSED;
  {
    double refcnt = ((double)loops*(double)limit);
    DBG(bbu_printf("T: %ld loops at limit %ld took %f us, %f refs.\n\r",loops,limit,*ous,refcnt));
    return(refcnt);
  }
}

double hand_benchmark_cache_ronly(REGISTER DATATYPE *x, REGISTER long limit, long *oloops, double *ous)
{
  REGISTER long index = 0, loops = 0;
  REGISTER DATATYPE sum = (DATATYPE)0;
  REGISTER DATATYPE sum2 = (DATATYPE)0;

  FLUSHALL(1);
  keepgoing = 1;
  //assert(signal(SIGALRM,handler) != SIG_ERR);
  limit -= 8;

  //alarm(duration);
  timer_used = start_timer(1000*duration, handler, TIMER_ONE_SHOT);
  TIMER_START;

again:
  sum += x[index] + x[index+1] + x[index+2] + x[index+3];
  sum2 += x[index+4] + x[index+5] + x[index+6] + x[index+7];
  if ((index += 8) < limit)
    goto again;
  else if (keepgoing)
    {
      index = 0;
      loops++;
      goto again;
    }

  TIMER_STOP;
  index += 8;

  x[0] = (DATATYPE)sum + (DATATYPE)sum2;
  x[1] = (DATATYPE)index;
  //fake_out_optimizations(x,2*sizeof(DATATYPE));

  *oloops = loops;
  *ous = TIMER_ELAPSED;
  {
    double refcnt = ((double)loops*(double)limit);
    DBG(bbu_printf("T: %ld loops at limit %ld took %f us, %f refs.\n\r",loops,limit,*ous,refcnt));
    return(refcnt);
  }
}

double benchmark_cache_wonly(REGISTER DATATYPE *x, REGISTER long limit, long *oloops, double *ous)
{
  REGISTER long index = 0, loops = 0;
  REGISTER DATATYPE wval = (DATATYPE)0xf;

  FLUSHALL(1);
  keepgoing = 1;
  //assert(signal(SIGALRM,handler) != SIG_ERR);
  wval = (DATATYPE)limit;

  //alarm(duration);
  timer_used = start_timer(1000*duration, handler, TIMER_ONE_SHOT);
  TIMER_START;

  while (keepgoing)
    {
      for  (index = 0; index < limit; index++)
	x[index] = wval;
      loops++;
    }

  TIMER_STOP;

  //fake_out_optimizations(x,limit*sizeof(DATATYPE));

  *oloops = loops;
  *ous = TIMER_ELAPSED;
  {
    double refcnt = ((double)loops*(double)limit);
    DBG(bbu_printf("T: %ld loops at limit %ld took %f us, %f refs.\n\r",loops,limit,*ous,refcnt));
    return(refcnt);
  }
}

double hand_benchmark_cache_wonly(REGISTER DATATYPE *x, REGISTER long limit, long *oloops, double *ous)
{
  REGISTER long index = 0, loops = 0;
  REGISTER DATATYPE wval = (DATATYPE)0xf;

  FLUSHALL(1);
  keepgoing = 1;
  //assert(signal(SIGALRM,handler) != SIG_ERR);
  limit -= 8; wval = (DATATYPE)limit;

  //alarm(duration);
  timer_used = start_timer(1000*duration, handler, TIMER_ONE_SHOT);
  TIMER_START;

again:
  x[index] = wval;
  x[index+1] = wval;
  x[index+2] = wval;
  x[index+3] = wval;
  x[index+4] = wval;
  x[index+5] = wval;
  x[index+6] = wval;
  x[index+7] = wval;
  if ((index+=8) < limit)
    goto again;
  else if (keepgoing)
    {
      index = 0;
      loops++;
      goto again;
    }

  TIMER_STOP;
  index += 8;

  //fake_out_optimizations(x,limit*sizeof(DATATYPE));

  *oloops = loops;
  *ous = TIMER_ELAPSED;
  {
    double refcnt = ((double)loops*(double)limit);
    DBG(bbu_printf("T: %ld loops at limit %ld took %f us, %f refs.\n\r",loops,limit,*ous,refcnt));
    return(refcnt);
  }
}

double benchmark_cache(REGISTER DATATYPE *x, REGISTER long limit, long *oloops, double *ous)
{
  REGISTER long index = 0, loops = 0;
  
  FLUSHALL(1);
  keepgoing = 1;
  //assert(signal(SIGALRM,handler) != SIG_ERR);

  //alarm(duration);
  timer_used = start_timer(1000*duration, handler, TIMER_ONE_SHOT);
  TIMER_START;

  while (keepgoing)
    {
      for (index = 0; index < limit; index++)
	x[index]++;
      loops++;
    }

  TIMER_STOP;

  //fake_out_optimizations(x,limit*sizeof(DATATYPE));

  *oloops = loops;
  *ous = TIMER_ELAPSED;
  {
    double refcnt = ((double)loops*(double)limit);
    DBG(bbu_printf("T: %ld loops at limit %ld took %f us, %f refs.\n\r",loops,limit,*ous,refcnt));
    return(refcnt);
  }
}

double hand_benchmark_cache(REGISTER DATATYPE *x, REGISTER long limit, long *oloops, double *ous)
{
  REGISTER long index = 0, loops = 0;
  
  FLUSHALL(1);
  keepgoing = 1;
  //assert(signal(SIGALRM,handler) != SIG_ERR);

  //alarm(duration);
  timer_used = start_timer(1000*duration, handler, TIMER_ONE_SHOT);
  TIMER_START;

again:
  x[index]++;
  x[index+1]++;
  x[index+2]++;
  x[index+3]++;
  x[index+4]++;
  x[index+5]++;
  x[index+6]++;
  x[index+7]++;
  if ((index+=8) <= limit-8)
    goto again;
  else if (keepgoing)
    {
      index = 0;
      loops++;
      goto again;
    }

  TIMER_STOP;

  //fake_out_optimizations(x,limit*sizeof(DATATYPE));

  *oloops = loops;
  *ous = TIMER_ELAPSED;
  {
    double refcnt = ((double)loops*(double)limit);
    DBG(bbu_printf("T: %ld loops at limit %ld took %f us, %f refs.\n\r",loops,limit,*ous,refcnt));
    return(refcnt);
  }
}


double benchmark_cache_memory_copy(REGISTER void *x, REGISTER void *y, REGISTER long bytes, long *oloops, double *ous)
{
  REGISTER long loops = 0;
  
  FLUSHALL(1);
  keepgoing = 1;
  //assert(signal(SIGALRM,handler) != SIG_ERR);

  //alarm(duration);
  timer_used = start_timer(1000*duration, handler, TIMER_ONE_SHOT);
  TIMER_START;

  while (keepgoing)
    {
      memcpy(x,y,bytes);
      loops++;
    }

  TIMER_STOP;

  //fake_out_optimizations(x,bytes);
  //fake_out_optimizations(y,bytes);

  *ous = TIMER_ELAPSED;
  *oloops = loops;
  return((double)loops*(double)bytes);
}

double benchmark_cache_memory_set(REGISTER void *x, REGISTER long bytes, long *oloops, double *ous)
{
  REGISTER long loops = 0;
  
  FLUSHALL(1);
  keepgoing = 1;
  //assert(signal(SIGALRM,handler) != SIG_ERR);

  //alarm(duration);
  timer_used = start_timer(1000*duration, handler, TIMER_ONE_SHOT);
  TIMER_START;

  while (keepgoing)
    {
      memset(x,0xf0,bytes);
      loops++;
    }

  TIMER_STOP;

  //fake_out_optimizations(x,bytes);

  *ous = TIMER_ELAPSED;
  *oloops = loops;
  return((double)loops*(double)bytes);
}

void initialize_sizes(long *sizes)
{
  long i,j;
  
  for (j=0; j<timeslots; j+=resolution)
    {
      sizes[j] = 1L<<(CACHE_MIN_BITS+j/resolution);
      DBG(bbu_printf("POW: %ld %ld\n\r",j,sizes[j]));

      for (i=1;i<resolution;i++)
	{
	  if (j+i < timeslots)
	    {
	      sizes[j+i] = sizes[j] + i*(sizes[j]/resolution);
	      sizes[j+i] = sizes[j+i] - sizes[j+i]%(long)sizeof(DATATYPE);
	      DBG(bbu_printf("SUB: %ld %ld\n\r",j+i,sizes[j+i]));
	    }
	}
    }
}

void do_memory_copy(long *sizes, void *x, double *times, double *bws, double *percents)
{
  long limit, j, i, tloops;
  double refcnt, overhead_per_ref = 0.0, tmicrosec;
  /* double nullcnt = 0.0, cmicrosec = 0.0; */
  void *y;

  assert(y = (void *)bbu_malloc(memsize));
  memset(y,0x0f,memsize);

  if (my_isatty(1))
    {
      bbu_printf("\n\r\t\t%sMemory Copy Library Cache Test\n\r\n\r",
	     (!(type & NOCALIBRATE)) ? "Calibrated " : "");
      bbu_printf("C Size\t\tNanosec\t\tMB/sec\t\t%% Chnge\n\r");
      bbu_printf("-------\t\t-------\t\t-------\t\t-------\n\r");
    }

  for (i = 0; i < timeslots; i++) 
    {
      limit = sizes[i];
      for (j = 0; j < repeat_count; j++)
	{
	  refcnt = benchmark_cache_memory_copy(x, y, limit, &tloops, &tmicrosec) * 2.0;
	  
	  /* if (type & NOCALIBRATE)
	    {
	      nullcnt = 0.0;
	      overhead_per_ref = 0.0;
	      cmicrosec = 0.0;
	    }
	  else
	    {
	      nullcnt = calibrate_benchmark_cache_memory_copy(x, tloops, limit, &cmicrosec);
	      overhead_per_ref = (cmicrosec*1000.0) / nullcnt;
	      DBG(bbu_printf("C: %f ns per ref.\n\r",overhead_per_ref));
	    } */

	  compute_stats(i,j,refcnt,overhead_per_ref,times,bws,percents,sizes,tmicrosec,1);
	}
    }
  
  /* if ((my_isatty(1))&&(type&GUESSCACHESIZE))
    compute_cache_sizes(times,bws,percents,sizes); */

  bbu_mfree(y);
}

void do_memory_set(long *sizes, DATATYPE *x, double *times, double *bws, double *percents)
{
  long limit, j, i, tloops;
  double refcnt, overhead_per_ref = 0.0, tmicrosec;
  /* double nullcnt = 0.0, cmicrosec = 0.0; */

  if (my_isatty(1))
    {
      bbu_printf("\n\r\t\t%sMemory Set Library Cache Test\n\r\n\r",
	     ((!(type & NOCALIBRATE)) ? "Calibrated " : ""));

      bbu_printf("C Size\t\tNanosec\t\tMB/sec\t\t%% Chnge\n\r");
      bbu_printf("-------\t\t-------\t\t-------\t\t-------\n\r");
    }

  for (i = 0; i < timeslots; i++) 
    {
      limit = sizes[i];
      for (j = 0; j < repeat_count; j++)
	{
	  refcnt = benchmark_cache_memory_set(x, limit, &tloops, &tmicrosec);
	  
	  /* if (type & NOCALIBRATE)
	    {
	      nullcnt = 0.0;
	      overhead_per_ref = 0.0;
	      cmicrosec = 0.0;
	    }
	  else
	    {
	      nullcnt = calibrate_benchmark_cache_memory_copy(x, tloops, limit, &cmicrosec);
	      overhead_per_ref = (cmicrosec*1000.0) / nullcnt;
	      DBG(bbu_printf("C: %f ns per ref.\n\r",overhead_per_ref));
	    } */

	  compute_stats(i,j,refcnt,overhead_per_ref,times,bws,percents,sizes,tmicrosec,1);
	}
    }
  
  /* if ((my_isatty(1))&&(type&GUESSCACHESIZE))
    compute_cache_sizes(times,bws,percents,sizes); */
}

void do_read_only(long *sizes, DATATYPE *x, double *times, double *bws, double *percents)
{
  long limit, j, i, tloops;
  double refcnt, overhead_per_ref = 0.0, tmicrosec;
  /* double nullcnt = 0.0, cmicrosec = 0.0; */

  if (my_isatty(1))
    {
      bbu_printf("\n\r\t\t%s%s%s Read Cache Test\n\r\n\r",
	     ((type & HANDTUNED) ? "Tuned " : ""),
	     ((!(type & NOCALIBRATE)) ? "Calibrated " : ""), DATASTRING);

      bbu_printf("C Size\t\tNanosec\t\tMB/sec\t\t%% Chnge\n\r");
      bbu_printf("-------\t\t-------\t\t-------\t\t-------\n\r");
    }

  for (i = 0; i < timeslots; i++) 
    {
      limit = sizes[i] / (long)sizeof(DATATYPE);
      for (j = 0; j < repeat_count; j++)
	{
	  if (type & HANDTUNED)
	    refcnt = hand_benchmark_cache_ronly(x, limit, &tloops, &tmicrosec);
	  else
	    refcnt = benchmark_cache_ronly(x, limit, &tloops, &tmicrosec);

	  /* if (type & NOCALIBRATE)
	    {
	      nullcnt = 0.0;
	      overhead_per_ref = 0.0;
	      cmicrosec = 0.0;
	    }
	  else
	    {
	      nullcnt = calibrate_benchmark(x, tloops, limit, &cmicrosec);
	      overhead_per_ref = (cmicrosec*1000.0) / nullcnt;
	      DBG(bbu_printf("C: %f ns per ref.\n\r",overhead_per_ref));
	    } */

	  compute_stats(i,j,refcnt,overhead_per_ref,times,bws,percents,sizes,tmicrosec,sizeof(DATATYPE));
	}
    }
      
  /* if ((my_isatty(1))&&(type&GUESSCACHESIZE))
    compute_cache_sizes(times,bws,percents,sizes); */
}

void do_write_only(long *sizes, DATATYPE *x, double *times, double *bws, double *percents)
{
  long limit, j, i, tloops;
  double refcnt, overhead_per_ref = 0.0, tmicrosec;
  /* double nullcnt = 0.0, cmicrosec = 0.0; */

  if (my_isatty(1))
    {
      bbu_printf("\n\r\t\t%s%s%s Write Cache Test\n\r\n\r",
	     ((type & HANDTUNED) ? "Tuned " : ""),
	     ((!(type & NOCALIBRATE)) ? "Calibrated " : ""), DATASTRING);

      bbu_printf("C Size\t\tNanosec\t\tMB/sec\t\t%% Chnge\n\r");
      bbu_printf("-------\t\t-------\t\t-------\t\t-------\n\r");
    }

  for (i = 0; i < timeslots; i++) 
    {
      limit = sizes[i] / (long)sizeof(DATATYPE);
      for (j = 0; j < repeat_count; j++)
	{
	  if (type & HANDTUNED)
	    refcnt = hand_benchmark_cache_wonly(x, limit, &tloops, &tmicrosec);
	  else
	    refcnt = benchmark_cache_wonly(x, limit, &tloops, &tmicrosec);

	  /* if (type & NOCALIBRATE)
	    {
	      nullcnt = 0.0;
	      overhead_per_ref = 0.0;
	      cmicrosec = 0.0;
	    }
	  else
	    {
	      nullcnt = calibrate_benchmark(x, tloops, limit, &cmicrosec);
	      overhead_per_ref = (cmicrosec*1000.0) / nullcnt;
	      DBG(bbu_printf("C: %f ns per ref.\n\r",overhead_per_ref));
	    } */
	  
	  compute_stats(i,j,refcnt,overhead_per_ref,times,bws,percents,sizes,tmicrosec,sizeof(DATATYPE));
	}
    }
  
  /* if ((my_isatty(1))&&(type&GUESSCACHESIZE))
    compute_cache_sizes(times,bws,percents,sizes); */
}

void do_read_write(long *sizes, DATATYPE *x, double *times, double *bws, double *percents)
{
  long limit, j, i, tloops;
  double refcnt, overhead_per_ref = 0.0, tmicrosec;
  /* double nullcnt = 0.0, cmicrosec = 0.0; */

  if (my_isatty(1))
    {
      bbu_printf("\n\r\t\t%s%s%s RMW Cache Test\n\r\n\r",
	     ((type & HANDTUNED) ? "Tuned " : ""),
	     ((!(type & NOCALIBRATE)) ? "Calibrated " : ""), DATASTRING);

      bbu_printf("C Size\t\tNanosec\t\tMB/sec\t\t%% Chnge\n\r");
      bbu_printf("-------\t\t-------\t\t-------\t\t-------\n\r");
    }

  for (i = 0; i < timeslots; i++) 
    {
      limit = sizes[i] / (long)sizeof(DATATYPE);

      for (j = 0; j < repeat_count; j++)
	{
	  if (type & HANDTUNED)
	    refcnt = hand_benchmark_cache(x, limit, &tloops, &tmicrosec) * 2.0;
	  else
	    refcnt = benchmark_cache(x, limit, &tloops, &tmicrosec) * 2.0;
  
	  /* if (type & NOCALIBRATE)
	    {
	      nullcnt = 0.0;
	      overhead_per_ref = 0.0;
	      cmicrosec = 0.0;
	    }
	  else
	    {
	      nullcnt = calibrate_benchmark(x, tloops, limit, &cmicrosec);
	      nullcnt *= 2; 
	      overhead_per_ref = (cmicrosec*1000.0) / nullcnt;
	      DBG(bbu_printf("C: %f ns per ref.\n\r",overhead_per_ref));
	    } */
	  
	  compute_stats(i,j,refcnt,overhead_per_ref,times,bws,percents,sizes,tmicrosec,sizeof(DATATYPE));
	}
    }
  
  /* if ((my_isatty(1))&&(type&GUESSCACHESIZE))
    compute_cache_sizes(times,bws,percents,sizes); */
}

int cachebench_main(int argc, char **argv) 
{
  DATATYPE *x;
  long *sizes;
  double *times, *bws, *percents;
  type = NOCALIBRATE;

  type = usage(argc, argv);
  if(type == -1)
   return 0;

  assert(sizes = (long *)bbu_malloc(timeslots*sizeof(long)));
  memset(sizes,0x00,(timeslots*sizeof(long)));
  assert(times = (double *)bbu_malloc(timeslots*repeat_count*sizeof(double)));
  memset(times,0x00,(timeslots*repeat_count*sizeof(double)));
  assert(bws = (double *)bbu_malloc(timeslots*repeat_count*sizeof(double)));
  memset(bws,0x00,(timeslots*repeat_count*sizeof(double)));
  assert(percents = (double *)bbu_malloc(timeslots*repeat_count*sizeof(double)));
  memset(percents,0x00,(timeslots*repeat_count*sizeof(double)));
  assert(x = (DATATYPE *)bbu_malloc((size_t)memsize));
  memset((void *)x,0x00,memsize);

  initialize_sizes(sizes);

  /* Measure cache */

  if (type & MEMORYSET)
    {
      do_memory_set(sizes,x,times,bws,percents);
    }

  if (type & MEMORYCOPY)
    {
      do_memory_copy(sizes,x,times,bws,percents);
    }

  if (type & READONLY)
    {
      do_read_only(sizes,x,times,bws,percents);
    }

  if (type & WRITEONLY)
    {
      do_write_only(sizes,x,times,bws,percents);
    }

  if (type & READWRITE)
    {
      do_read_write(sizes,x,times,bws,percents);
    }

  FLUSHALL(0);
  bbu_mfree((void *)sizes);
  bbu_mfree((void *)times);
  bbu_mfree((void *)bws);
  bbu_mfree((void *)percents);
  bbu_mfree((void *)x);
  return 0;
}
