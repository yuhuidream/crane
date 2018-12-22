#ifndef __DDR_POWER_H__
#define __DDR_POWER_H__
#include "MCU.h"

typedef enum{
   ACTIVE_STATE = 0,
   SELF_REFRESH,
   AUTO_ACTIVE_POWER_DOWN,
   AUTO_PRECHG_POWER_DOWN,
   DEEP_POWER_DOWN,
   ERROR_POWER_STATE,
   AUTO_SELF_REFRESH,
}DDR_POWER_STATE;

#define ACQUIRE_MEM_RANGE 		1
#define RELEASE_MEM_RANGE 		2

#define RUNNING_MODE 			1
#define HOME_MODE				2
#define STANDBY_VCTCXO_OFF_MODE 3 
#define STANDBY_VCTCXO_ON_MODE  4

//#define DEBUG_VM_MALLOC
#ifdef DEBUG_VM_MALLOC
	#define pr_debug(fmt,args...)  bbu_printf(fmt,##args)
#else
	#define 	  pr_debug(fmt,args...)   do{}while(0)
#endif

int  get_ddr_power_state(int chip);
void set_ddr_power_state(int chip,DDR_POWER_STATE state);
void dump_ddr_power_status(void);
int get_ddr_chip_nr(void);
void set_ddr_pasr(int chip, unsigned int bank_map);
void set_ddr_segment_pasr(int chip, unsigned int segment_map);
void set_ddr_pasr(int chip, unsigned int bank_map);
void set_ddr_pasr_all(void);
void  vma_init(void);
void dump_free_vmas(void);
void dump_used_vmas(void);
extern void mem_dump_range_status();
extern unsigned int get_mem_usage_bankmap(void);
extern int is_chip_1_unused(void);
extern int is_chip_0_unused(void);
extern void update_mem_range_bitmap(unsigned int addr_start, unsigned int addr_len,int aquire_release);
void bbu_vm_malloc_prepare(void);
void mem_hotplug_set_ddr_power(int power_mode);
extern void test_vm(void);
void ddr_power_set_test(void);
void set_ddr_auto_self_refresh(int chip);
#endif

