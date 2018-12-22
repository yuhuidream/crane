#ifndef __MEM_HOTPLUG_H__
#define __MEM_HOTPLUG_H__
#include "bbu.h"
#include "predefines.h"

#define MEM_CHIP_SIZE (BIT_0 << 29)
#define SIZE_PER_BANK (BIT_0 << 26) //64MB per bank, 8 banks per chip

typedef enum{
   DDR_RANGE_RESERVED = 0,/*reserved DDR range, can not sleep*/
   DDR_RANGE_ALLOCATED_USED,
   DDR_RANGE_ALLOCATED_UNUSED,
   DDR_RANGE_NOT_ALLOCATED,
}DDR_RANGE_STATE;

struct mem_ranges_desc{
   char mem_usage[32]; //mem usage
   uint32_t addr_start;   //vAddress of the mem range
   uint32_t addr_len;   //mem len
   uint32_t mem_range_state;   //mem type		
   uint32_t bank_map;   //mem bank bit map
};
extern void mem_malloc_free_hook(uint32_t addr_start, uint32_t addr_len, int aquire_release);
#endif
