#include "predefines.h"
#include "qp_port.h"
#include "bbu.h"
#include "utils.h"   
#include <stdlib.h>
#include <string.h>
#include "bsp.h"
#include "mem_hotplug.h"
#include "gpio_dec.h"
#include "timer_dec.h"
#include "qspinlock.h"
#include "mfp.h"
#include "set_mmu.h"
#include "vm_manager.h"

extern struct mem_ranges_desc  bbu_mem_ranges[MEM_RANGE_MAX];

static unsigned int mem_bank_inuse_bitmap = 0;

void mem_dump_range_status(void)
{
	unsigned int  i;
	
	bbu_printf("========================emei mem map========================\n\r");
	for(i=0;i<ARRAY_SIZE(bbu_mem_ranges);i++)
	{
		
		bbu_printf("%-16s: [0x%x(%dM)->0x%x(%dM)]  size:%dM\n\r                map:0x%x,state: ",bbu_mem_ranges[i].mem_usage
					,bbu_mem_ranges[i].addr_start
					,(bbu_mem_ranges[i].addr_start>>20)
					,bbu_mem_ranges[i].addr_start+bbu_mem_ranges[i].addr_len
					,((bbu_mem_ranges[i].addr_start+bbu_mem_ranges[i].addr_len)>>20)
					,(bbu_mem_ranges[i].addr_len>>20)
					,bbu_mem_ranges[i].bank_map);
		switch (bbu_mem_ranges[i].mem_range_state)
		{
			case DDR_RANGE_RESERVED:
				bbu_printf("mem reserved\n\r");
				break;
			case DDR_RANGE_ALLOCATED_UNUSED:
				bbu_printf("mem unused\n\r");
				break;
			case DDR_RANGE_ALLOCATED_USED:
				bbu_printf("mem inuse\n\r");	
			case DDR_RANGE_NOT_ALLOCATED:
				bbu_printf("mem alloc buff\n\r");	
				break;
			default:
				bbu_printf("mem unkown\n\r");
				break;
		}
		
	}

	for(i=0;i<16;i++)
	{
		if((mem_bank_inuse_bitmap&(BIT_0 << i)) != 0)
			bbu_printf("bank %d inuse or reserved\n\r",i);
		else
			bbu_printf("bank %d unused\n\r",i);
	}

	//bbu_printf("kernel image size is %x %dM",kern_image_size,(kern_image_size >> 20));	
	bbu_printf("===========================================================\n\r");
}

unsigned int get_mem_usage_bankmap(void)
{
	return mem_bank_inuse_bitmap;
}

int is_chip_1_unused(void)
{
		if((mem_bank_inuse_bitmap&0xff00) == 0)
			return 1;
		else 
			return 0;
}

int is_chip_0_unused(void)
{
		if((mem_bank_inuse_bitmap&0xff) == 0)
			return 1;
		else 
			return 0;
}

//NOTE: should use spin lock to protect this funtion
void update_mem_range_bitmap(unsigned int addr_start, unsigned int addr_len,int aquire_release)
{
	int j,k;
	unsigned int bank_usage_bitmap = 0;
	(void)aquire_release;
	
	if((addr_start +addr_len) > (MEM_CHIP_SIZE << 1))
	{
		bbu_printf("%s:error ddr range specified\n\r",__FUNCTION__);
		return;
	}
	
	for(j=0,k=(addr_len+addr_start%SIZE_PER_BANK);
				k>0; k-=SIZE_PER_BANK,j++)
	{
					bank_usage_bitmap |= 
						(BIT_0<<(addr_start/SIZE_PER_BANK+j));
	}
				
	mem_bank_inuse_bitmap |= bank_usage_bitmap;
/*
	for(i=0;i<ARRAY_SIZE(bbu_mem_ranges);i++)
	{
		if(bbu_mem_ranges[i].addr_start== addr_start)
		{
				if(ACQUIRE_MEM_RANGE == aquire_release)
					bbu_mem_ranges[i].bank_map |=bank_usage_bitmap;
				
				if(RELEASE_MEM_RANGE == aquire_release)
					bbu_mem_ranges[i].bank_map &= ~bank_usage_bitmap;
		}
		
		mem_bank_inuse_bitmap |= bbu_mem_ranges[i].bank_map;
		
	}
*/
	
}

void mem_range_init(void)
{
	int j,k;
	uint32_t i, bank_usage_bitmap = 0;
#if 0
	bbu_mem_ranges[1].addr_start = CODE_AREA_ST;
	bbu_mem_ranges[1].addr_len = (((kern_image_size >>20)+1)<<20);	
	bbu_mem_ranges[2].addr_start = bbu_malloc_buffer;
	bbu_mem_ranges[2].addr_len = MALLOC_BUF_LEN;	
	bbu_mem_ranges[3].addr_start = bbu_malloc_buffer+MALLOC_BUF_LEN;
	if(board_is_discrete_v10())
		bbu_mem_ranges[3].addr_len = DDR_MEM_SIZE_512M - bbu_mem_ranges[3].addr_start;
	else
		bbu_mem_ranges[3].addr_len = DDR_MEM_SIZE_1G - bbu_mem_ranges[3].addr_start;	
#endif
	for(i = 0; i<ARRAY_SIZE(bbu_mem_ranges); i++)
	{
		//bbu_printf("i =%d\n\r",i);
		if(bbu_mem_ranges[i].mem_range_state == DDR_RANGE_RESERVED
			|| bbu_mem_ranges[i].mem_range_state == DDR_RANGE_ALLOCATED_USED)
		{

			for(j=0,k=(bbu_mem_ranges[i].addr_len
				+bbu_mem_ranges[i].addr_start%SIZE_PER_BANK);
				k>0; k-=SIZE_PER_BANK,j++)
			{
					bank_usage_bitmap |= 
						(BIT_0<<(bbu_mem_ranges[i].addr_start/SIZE_PER_BANK+j));
			}
				
			bbu_mem_ranges[i].bank_map = bank_usage_bitmap;
			
		}else{
			bbu_mem_ranges[i].bank_map = 0;
		}
	
	}
	
	mem_bank_inuse_bitmap |=bank_usage_bitmap;
}


/*..........................................................................*/
/*
void MEM_HOTPLUG_ctor(void) {
   mem_hotplug *me = &l_mem_hotplug;
   QActive_ctor(&me->super, (QStateHandler)&mem_hotplug_initial);
}
*/

/*..........................................................................*/
void mem_hotplug_init(void)
{
    vma_init();
}

/*
void mem_post_acquire_release_sig(uint32_t addr_start, uint32_t addr_len, int aquire_release)
{
	MEMReqEvt * memevt;
	
    	if( ACQUIRE_MEM_RANGE == aquire_release)	
    		memevt = Q_NEW(MEMReqEvt,MEM_ACQUIRE_SIG);
	else if(RELEASE_MEM_RANGE == aquire_release)
		memevt = Q_NEW(MEMReqEvt,MEM_RELEASE_SIG);
	else{
		bbu_printf("%s error sig\n\r",__FUNCTION__);
		return;
	}
	
    	memevt->addr_start = addr_start;
     	memevt->addr_len=addr_len;
	memevt->req_type=aquire_release;	
	
    	QACTIVE_POST(AO_MEM_HOTPLUG,(QEvent *)memevt, 0);   
}
*/

/*..........................................................................*/   
void mem_malloc_free_hook(uint32_t addr_start, uint32_t addr_len, int aquire_release)
{
	if(addr_len !=0)
		update_mem_range_bitmap(addr_start, addr_len, aquire_release);
	else 
		bbu_printf("%s: error addr_len\n\r",__FUNCTION__);
}

