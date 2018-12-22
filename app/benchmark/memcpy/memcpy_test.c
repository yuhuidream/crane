#include "bbu.h"
#include "string.h"
#include "stdio.h"
#include "perf_counter.h"
#include "ppset.h"
extern void memcpy_a15_new(void *dest, void *src, size_t n);
extern void memcpy_a15_old(void *dest, void *src, size_t n);
extern void memcpy_jb4_2(void *dest, void *src, size_t n);
extern void memcpy_jb4_3(void *dest, void *src, size_t n);

typedef void (*memcpy_t)(void*, void*, size_t);

typedef struct
{
	memcpy_t op;
	char name[32];
}memcpy_test_y;

memcpy_test_y memcpys[] = 
{
	{memcpy_a15_new,   "Carton opt"      },
	{memcpy_a15_old,   "memcpy.a15"      },
	{memcpy_jb4_2,     "Marvell opt"     },
	{memcpy_jb4_3,     "JB4.3 default"   },
	{(memcpy_t)memcpy, "Default"         },
};



static uint32_t memcpy_sub_test(int test, uint32_t nbytes, 
	                            uint32_t iters, int coreFreqMHz, 
	                            uint32_t src_off, uint32_t dest_off, 
	                            int auto_test)
{
	uint32_t total_size;
	void *src_bk, *src, *dest_bk, *dest;
	int cycle, divider;
	double speed;

	memcpy_t memcpy_used = memcpys[test].op;

	StopCycleCounter();
	SetCycleCounter(0);
	cycle_counter_divider_set();
	cycle = ReadCycleCounter();
	if(cycle != 0)
	{
		bbu_printf("Error£¡Cycle Counter clear fail\n\r");
		return 0;
	}
	src_bk  = bbu_malloc(nbytes + 256 + 256);
	dest_bk = bbu_malloc(nbytes + 256 + 256);
	src  = (void *)(ADDR_ALIGN((uint32_t)src_bk,  256) + src_off);
	dest = (void *)(ADDR_ALIGN((uint32_t)dest_bk, 256) + dest_off);
	//bbu_printf("src: 0x%08X, dest: 0x%08X\n\r", src, dest);
	memset(src, 'x', nbytes);
	memcpy_used(dest, src, nbytes);

	perf_counters_enable();
	counter_enable(CYCLE_COUNTER_ENABLE_BASE);

	for(uint32_t i = 0; i < iters; i++)
	{
		memcpy_used(dest, src, nbytes);
	}
	StopCycleCounter();
	cycle = ReadCycleCounter();

	if(is_cycle_counter_divider_set()){
		divider = 64;
	}
	else{
		divider = 1;
	}
	total_size = nbytes * iters;
	speed = ((double)total_size*(double)1000000*(double)coreFreqMHz)/((double)cycle*divider);
	speed = speed/(1024*1024);
	bbu_mfree(src_bk);
	bbu_mfree(dest_bk);

	if(!auto_test){
		bbu_printf("Memory copy(%s) from 0x%08X to 0x%08X\n\r", memcpys[test].name, (uint32_t)src, (uint32_t)dest);
		bbu_printf("iters: %d, nbytes: %d, core frequency: %dMHz\n\r", iters, nbytes, coreFreqMHz);
		bbu_printf("used CPU cycle: 0x%X\n\r", cycle*divider);
		bbu_printf("Time measured using cycle counter: %.2f\n\r", (double)(cycle*divider)/(double)(coreFreqMHz*1000000));
		bbu_printf("speed %f MB/s\n\r", speed);
	}

	return (uint32_t)speed;
}

static void memcpy_auto_test(int test, uint32_t src_off, uint32_t dest_off)
{
	uint32_t start_nbytes = 8;
	uint32_t end_nbytes   = 8*1024*1024; //8MB
	uint32_t total_nbytes = 512*1024*1024;
	uint32_t *result;
	uint32_t nbytes, idx;
	int main_loops = 0;
	int coreFreqMHz;
	int iters = 0;

	//if(OP_INVALID == user_cop[PLL_CORE]){
	//	bbu_printf("Can't Get Current PP, measure PCLK directly\n\r");
		coreFreqMHz = measure_core_frequency(0);  /* Get current core frequency */
	//}else{
	//	coreFreqMHz = cr5_value[user_cop[PLL_CORE]].pclk;
	//}
	for(uint32_t i = start_nbytes; i <= end_nbytes; i*=2)
	{
		main_loops ++;
	}
	result = (uint32_t *)bbu_malloc(main_loops * sizeof(uint32_t));
	bbu_printf("%s Memcpy Auto test Begin\n\r", memcpys[test].name);
	for(nbytes = start_nbytes, idx = 0; nbytes <= end_nbytes; nbytes *= 2, idx++)
	{
		iters = total_nbytes/nbytes;
		result[idx] = memcpy_sub_test(test, nbytes, iters, coreFreqMHz, src_off, dest_off, 1);
		if(nbytes < 1024)
			bbu_printf("  %3d    ", nbytes);
		else if(nbytes < 1024*1024)
			bbu_printf("%3dKB    ", nbytes/1024);
		else
			bbu_printf("%3dMB    ", nbytes/1024/1024);
		bbu_printf("%4d MB/s\n\r", result[idx]);
	}
	for(int i = 0; i < main_loops; i ++)
		bbu_printf("%3d\n\r", result[i]);

	bbu_mfree((void *)result);
}

static void memcpy_test_help(uint8_t **argv)
{
	bbu_printf("\n\rSub test\n\r");
	bbu_printf("%s <test> <bytes> <iters> <src_off> <dest_off>\n\r", (char *)argv[0]);
	bbu_printf("Auto test\n\r");
	bbu_printf("%s auto <test> <src_off> <dest_off>\n\r", (char *)argv[0]);
	bbu_printf("<test>: Memcpy types\n\r");
	for(size_t i = 0; i < sizeof(memcpys)/sizeof(memcpys[0]); i++)
	bbu_printf("    %d : %s\n\r", i, memcpys[i].name);
}

//memcpy <test> <bytes> <iters> <src_off> <dest_off>
//memcpy auto <test> <src_off> <dest_off>
void memcpy_test(int argc, uint8_t **argv)
{
	uint32_t dest_off = 0, src_off = 0, iters = 0, nbytes = 0, test = 0;
	uint32_t coreFreqMHz;
	int auto_test = 0;
	
	if(OP_INVALID == user_cop[PLL_CORE]){
		bbu_printf("Can't Get Current PP, measure PCLK directly\n\r");
		coreFreqMHz = measure_core_frequency(0);  /* Get current core frequency */
	}else{
		coreFreqMHz = cr5_value[user_cop[PLL_CORE]].pclk;
	}

	if((argc > 1) && (!strcmp((char *)argv[1], "auto")))
	{
		auto_test = 1;
	}

	switch(argc){
		case 6:
			dest_off = XpStrToValue((char *)argv[5]);
		case 5:
			if(auto_test)
				dest_off = XpStrToValue((char *)argv[4]);
			else
				src_off = XpStrToValue((char *)argv[4]);
		case 4:
			if(auto_test)
				src_off = XpStrToValue((char *)argv[3]);
			else
				iters = XpStrToValue((char *)argv[3]);
		case 3:
			if(auto_test){
				test = conv_dec((char *)argv[2]);
			}
			else
				nbytes = XpStrToValue((char *)argv[2]);
		case 2:
			if(check_help(argv[1])){
				memcpy_test_help(argv);
				return;
			}
			if(!auto_test)
				test = conv_dec((char *)argv[1]);
			break;
		default:
			memcpy_test_help(argv);
			break;
	}

	if(dest_off > 256)
		dest_off = 0;
	if(src_off > 256)
		src_off = 0;

	if(test >= sizeof(memcpys)/sizeof(memcpys[0]))
	{
		bbu_printf("test is out of range\n\r");
	}
	
	if(auto_test)
	{
		memcpy_auto_test(test, src_off, dest_off);
		return;
	}

	if(!(iters && nbytes))  //these are must
	{
		bbu_printf("iters and nbytes must be non-zero\n\r");
		return;
	}

	memcpy_sub_test(test, nbytes, iters, coreFreqMHz, src_off, dest_off, 0);

}
