#ifndef _MEM_CONFIG_H
#define _MEM_CONFIG_H
//#include <stdarg.h>
//#include <stdio.h>
#include <string.h>
#include "predefines.h"
/* =============================================
 * Configurable macros start */
//#define VERBOSE
//#define DRY_RUN

/* =============================================
 * For a system with L2 cache size > 512MB, we
 * should use BUFFER_SIZE > (L2C_SIZE*2) to make
 * sure that DDR latency is measured correctly.
 * =============================================*/
#if L2C_SIZE_MB <= 512
#define BUFFER_SIZE	(1024*1024)
#else
#define BUFFER_SIZE	(L2C_SIZE_MB*1024*2)
#endif
#define LOOP_TIME       100
#define EVENT_MAXIMUM 	4

/* For log */
#define MAX_BUF_SIZE    1024            /* in KB */
#define TMARK_LOG_FILE  "tmark.csv"
//FILE * flog = NULL;

/* Test unit enable/disable macros
 *  * set to 1 for enable,
 *   * set to 0 for disable
 *    */
/* Load */
#define SINGLE_LDR_4                    1
#define SINGLE_LDR_32                   1
#define SINGLE_LDR_64                   1
#define PIPE_LDR_4                      1
#define PIPE_LDRD_8                     1
#define PIPE_LDR_32                     1
#define PIPE_LDR_64                     1
/* Store */
#define SINGLE_STR_4                    1
#define COALESCE_STR_4                  1
#define COALESCE_STRD_8			1
#define COALESCE_STRD_8_POSTINCR	0
#define COALESCE_STM_4                  1
#define DELAY_STORE 			0
/* Memcpy */
#define MEMCPY_TEST                     1
/* Misc. */
#define MISC_TEST			1

/* Configurable macros end
 * =============================================*/

#define CACHELINE_SIZE	(32)
#define CACHEBLOCK_SIZE (CACHELINE_SIZE*2)
#define L1HIT_BUF_SIZE	(16*1024)
#define L2HIT_BUF_SIZE	(64*1024)
#define L2MISS_BUF_SIZE BUFFER_SIZE

#define MIN(a, b)       ((a>b)?b:a)
#define MAX(a, b)       ((a>b)?a:b)

#define ALIGN_32(addr)  (unsigned int)((((unsigned int)addr + 32) & (~31)))
#define ALIGN_128(addr) (unsigned int)((((unsigned int)addr + 128) & (~127)))
#define ALIGN_256(addr) (unsigned int)((((unsigned int)addr + 256) & (~255)))

#ifndef BAREMETAL
extern unsigned int dtime(void);
#define mrvl_times dtime
#define flush_all_cache()
#endif /* !BAREMETAL */
extern void dump_tmark_banner();
extern int measure_using_cycle;
unsigned int bkey = 1;
char t1[100];
char t2[100];


#define TMARK_Printf bbu_printf

void dump_banner()
{
        TMARK_Printf("********************************************************************\r\n");
        TMARK_Printf("        Marvell Tmark Benchmark (Memory system)\r\n");
        TMARK_Printf("         for the Marvel Xscale Microprocessor.\r\n");
        TMARK_Printf("         Copyright (c) 2006 Intel Corporation.\r\n");
        TMARK_Printf("          2007-2009 Marvell International Ltd. \r\n");
        TMARK_Printf("                All Rights Reserved.\r\n");
        TMARK_Printf("********************************************************************\r\n\r\n");
}

void dump_info_header()
{
if(measure_using_cycle)		//#ifdef BAREMETAL_NO_CYCLE
	TMARK_Printf("Title, Src Addr, Dst Addr, Buf Size(KB), Outer Loops, Stride(Byte), Total Time(cycles), Latency(cycle), Throughput(MB/giga cycle)\n\r");
else //#else
	TMARK_Printf("Title, Src Addr, Dst Addr, Buf Size(KB), Outer Loops, Stride(Byte), Total Time(us), Latency(cycle), Throughput(MB/s)\n\r");
//#endif
}


#endif /* _MEM_CONFIG_H */

/* EOF */

