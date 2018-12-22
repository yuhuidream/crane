#include "predefines.h"
#include "MCU.h"
#include "ddr_dec.h"
#include "interrupt.h"
#include "system_mrvl.h"
#include "ddr_power.h"
#include "timer_dec.h"
#include "errno_dec.h"
#include "mem_hotplug.h"
#include "mfp.h"
#include "vm_manager.h"
#include "mem_map.h"
#include "set_mmu.h"
#include "cache_ops.h"
#include "qspinlock.h"
#include "ripc.h"
#include "sm_malloc.h"
#include "shared_mem.h"
//#include <stdio.h> /*memcpy*/

#if 0 
/* use the macros in set_mmu.h */
#define BUFF_CACHEABLE_BUFFABLE  				(BIT_1|BIT_0)
#define BUFF_NONE_CACHEABLE_BUFFABLE  		(BIT_0)
#define BUFF_CACHEABLE_NONE_BUFFABLE  		(BIT_1)
#define BUFF_TEK 	(BIT_2)
#define BUFF_NONE_TEK 	(0)
#endif


struct mem_ranges_desc  bbu_mem_ranges[]={
   /*DDR memory*/
  /* mem_usage,  mem_addr,      mem_len,    mem_range_state */
   [MEM_RANGE_GCU] 		  = {"gcu_data",  0x00000000,    0x8000000, DDR_RANGE_RESERVED,			0},   /* code area(cb)*/
   [MEM_RANGE_CODE_AREA]  = {"code_area", CODE_AREA_ST,  0,         DDR_RANGE_RESERVED,			0},   /* code area(cb)*/
   [MEM_RANGE_MALLOC]	  = {"malloc_buf",0,             0,         DDR_RANGE_RESERVED,			0},   /* bbu malloc buffer(cb) */  
   [MEM_RANGE_ALLOC_BUFF_NCNB] = {"alloc_buff_ncnb",0, 	     0,         DDR_RANGE_NOT_ALLOCATED,	0},   /* allocateable buffer */
   [MEM_RANGE_ALLOC_BUFF] = {"alloc_buff",0, 	         0,         DDR_RANGE_NOT_ALLOCATED,	0},   /* allocateable buffer */
};
#if 0
static struct mm_struct bbu_ncnb_mm_struct={
	.mm_lock = Q_SPIN_LOCK_INITIALIZER,
};

static struct mm_struct bbu_cb_mm_struct={
	.mm_lock = Q_SPIN_LOCK_INITIALIZER,
};
#endif
static struct mm_struct ** bbu_ncnb_mm_struct = VM_MAMAGER_BUFF;
static struct mm_struct ** bbu_cb_mm_struct = (VM_MAMAGER_BUFF+4);

static volatile int gcu_buffer_lock = 1;
static QSpinlock gcu_buffer_spin_lock = Q_SPIN_LOCK_INITIALIZER;
static int gcu_buffer_owner = NO_OWNER;

extern int cpu_is_cr5(void);
extern int cpu_is_ca7(void);
static char *cpu_name[NUM_OF_CPUS] ={
    [CPU_CORE_CA7] = "CA7",
    [CPU_CORE_CR5] = "CR5",
};

int acquire_gcu_buffer_lock(int applicant)
{
	Q_SPIN_LOCK(&gcu_buffer_spin_lock);
	request_vm_manager();
	
	if(gcu_buffer_lock == 1)
	{
		gcu_buffer_lock = 0;
		gcu_buffer_owner = applicant;
		dsb();
		Q_SPIN_UNLOCK(&gcu_buffer_spin_lock);
		release_vm_manager();
		return 1;
	}else if(applicant == gcu_buffer_owner){
		Q_SPIN_UNLOCK(&gcu_buffer_spin_lock);
		release_vm_manager();
		return 1;
	}else{
		Q_SPIN_UNLOCK(&gcu_buffer_spin_lock);
		release_vm_manager();
		return 0;
	}
}

void release_gcu_buffer_lock(int applicant)
{	
	Q_SPIN_LOCK(&gcu_buffer_spin_lock);
	request_vm_manager();
	
	if(applicant != gcu_buffer_owner)
	{
		Q_SPIN_UNLOCK(&gcu_buffer_spin_lock);
		release_vm_manager();
		return;
	}
	gcu_buffer_lock = 1;
	gcu_buffer_owner = NO_OWNER;
	dsb();
	Q_SPIN_UNLOCK(&gcu_buffer_spin_lock);
	release_vm_manager();
}

static struct mm_struct * get_mm_by_vaddr(uint32_t vaddr)
{
    if(vaddr >= bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_start
       && vaddr < (bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_start
                    +bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_len))
        return NCNB_BUFF_PTR; 
    else
        return CB_BUFF_PTR;
}
/* do mmu map for mem range*/
static void vm_mmap(struct vm_area_struct *area, unsigned long addr_start, unsigned long addr_len)
{

/*
	Region region_one;
	region_one.vAddress= addr_start;
	region_one.pAddress= addr_start;
	region_one.pageSize = SECTIONPAGE;
	region_one.numPages = addr_len/SECTION_1_MB;
	region_one.AP = 3;
	region_one.APX = 0;
	region_one.TEX = (area->vm_flags&0x4); 
	region_one.CB = (area->vm_flags&0x3); //
	region_one.PT =&ptEmei[0];
	mmuMapRegion(&region_one);
*/	
	uint32_t address, attribute;
	
	switch(area->vm_flags)
	{
		case CACHEABLE:
			mmu_set_region_cb(addr_start,(addr_len >>20));
			break;
		case NON_CACHEABLE:
		
			address = addr_start;
			
			while(address < (addr_start + (addr_len&0xfff00000)))
			{
			   attribute = mmu_get_mem_attribute(address);
				if(attribute != STRONGLY_ORDERED &&
				   attribute != NON_SHAREABLE_DEVICE &&
				   attribute != SHAREABLE_DEVICE &&
				   attribute != OI_NON_CACHEABLE)
				{
					dcache_clean_invalidate_all();
					break;	
				}
				address += SECTION_1_MB;
			}
			
			mmu_set_region_ncnb(addr_start,(addr_len >>20));
			break;
		default:
			bbu_printf("wrong mmu option: please choose CACHEABLE or NON_CACHEABLE\n\r");
	}
	
}

/* do mmu unmap for mem range*/
static void vm_munmap(struct vm_area_struct *area, unsigned long addr_start, unsigned long addr_len)
{
	(void)area;
	(void)addr_start;
	(void)addr_len;
}

static struct vm_operations_struct bbu_vm_ops = {
	.mmap =&vm_mmap,
	.munmap=&vm_munmap,
};

static int get_cur_cpu(void)
{
	if(cpu_is_ca7())
	    return CPU_CORE_CA7;
	else
	    return CPU_CORE_CR5;
}	  
  
static  struct vm_area_struct * find_free_vma_intersection(struct mm_struct * mm, unsigned long len)
{
	struct vm_area_struct * vma;
	
	
	if((mm==NULL)||(mm->free_list ==NULL)||(len == 0))
		return NULL;
#if 0
	if(mm->size_minimum_free_vma >=len)
		return mm->minimum_free_vma;
#endif

	for(vma =mm->free_list; vma!=NULL;vma=vma->vm_next)
	{
		if((vma->vm_end -vma->vm_start) >=len)
			return vma;
	}

	return NULL;
}


/* Check system  has enough memory to allocate a
 * new virtual mapping.
 */
int vm_enough_memory(struct mm_struct *mm, unsigned long len)
{
	/* Stupid algorithm to decide if we have enough memory: while
	 * simple, it hopefully works in most obvious cases.. Easy to
	 * fool it, but this should catch most mistakes.
	 */
	if(mm->total_vm >= len)
		return 1;
	else 
	{
		pr_debug("total_vm is 0x%x, aquired len is 0x%x\n\r",mm->total_vm,len); 
		return 0;
	}
}

#define vm_avl_empty	(struct vm_area_struct *) NULL

/* Same as find_vma, but also return a pointer to the previous VMA in *pprev. */
struct vm_area_struct * find_free_vma_prev(struct mm_struct * mm, unsigned long addr,
				      struct vm_area_struct **pprev)
{
	if (mm) {
		if (!mm->mmap_avl) {
			/* Go through the linear list. */
			struct vm_area_struct * prev = NULL;
			struct vm_area_struct * vma = mm->free_list;
			while (vma && vma->vm_end <= addr) {
				prev = vma;
				vma = vma->vm_next;
			}
			*pprev = prev;
			return vma;
		}
	}
	
	*pprev = NULL;
	return NULL;
}


struct vm_area_struct * find_used_vma_prev(struct mm_struct * mm, unsigned long addr,
				      struct vm_area_struct **pprev)
{
	if (mm) {
		if (!mm->mmap_avl) {
			/* Go through the linear list. */
			struct vm_area_struct * prev = NULL;
			struct vm_area_struct * vma = mm->used_list;
			while (vma && vma->vm_end <= addr) {
				prev = vma;
				vma = vma->vm_next;
			}
			*pprev = prev;
			return vma;
		}
	}
	
	*pprev = NULL;
	return NULL;
}


/* Build the AVL corresponding to the VMA list.currently set to NULL */
void build_mmap_avl(struct mm_struct * mm)
{
	mm->mmap_avl = NULL;
}

/* Merge the list of memory segments if possible.
 * assume that the list is ordered by address.
 * We don't need to traverse the entire list, only those segments
 * which intersect or are adjacent to a given interval.
 *
 * We must already hold the mm semaphore when we get here..
 */
void merge_vmas (struct mm_struct * mm, struct vm_area_struct *prev, struct vm_area_struct *vma)
{
	(void)mm;
		
	if(prev->vm_end == vma->vm_start)
	{
		prev->vm_end = vma->vm_end;
		prev->vm_next =vma ->vm_next;

		sm_free(vma);
	}	
}

struct vm_area_struct *  split_vma(struct mm_struct *mm, struct vm_area_struct *vmp,struct vm_area_struct *prev_vmp,unsigned long len)
{
		struct vm_area_struct *mpnt;
		(void)prev_vmp;

		mpnt = sm_malloc(sizeof(struct vm_area_struct));
		if(!mpnt)
		{
			bbu_printf("%s: sm_malloc failed\n\r",__FUNCTION__);
			return NULL;
		}
			
	    //bbu_printf("%s:sm_malloc: addr: %x\n\r",__func__,mpnt);
	
		mpnt->vm_start = vmp->vm_start+len;
		mpnt->vm_end = vmp->vm_end;
		mpnt->vm_ops = &bbu_vm_ops;
		mpnt->vm_mm  = mm;
		mpnt ->vm_next = vmp->vm_next;

		vmp ->vm_end = vmp->vm_start+len;
		vmp->vm_next =mpnt;
		
		return mpnt; 
}

struct vm_area_struct * remove_free_vma(struct mm_struct *mm, struct vm_area_struct *vmp,struct vm_area_struct *prev_vmp,unsigned int len)
{
	/* insert a new vma after vma */
	struct vm_area_struct *next_vma;
	
       if((vmp->vm_end - vmp->vm_start) > len)
              next_vma =split_vma(mm, vmp, prev_vmp,len);
	else 
		next_vma = vmp->vm_next;
	
#if 0
	vma =mm->free_list;
	while(vma)
	{
		bbu_printf("vma: [0x%x(%dM)->0x%x(%dM)]\n\r"
					,vma->vm_start,vma->vm_start >>20
					,vma->vm_end,vma->vm_end >>20);
		vma =vma->vm_next;
	}
#endif
	
	if(prev_vmp == NULL)
	{
		mm->free_list =next_vma;

	}else{
		prev_vmp ->vm_next = next_vma;
	}

#if 0
	vma =mm->free_list;
	while(vma)
	{
		bbu_printf("vma: [0x%x(%dM)->0x%x(%dM)]\n\r"
					,vma->vm_start,vma->vm_start >>20
					,vma->vm_end,vma->vm_end >>20);
		vma =vma->vm_next;
	}
#endif

	return vmp;
}

struct vm_area_struct * remove_used_vma(struct mm_struct *mm, struct vm_area_struct *vmp, struct vm_area_struct *prev_vmp,unsigned  int len)
{
	(void)len;

	/* insert a new vma after vma */
	struct vm_area_struct *next_vma;
	next_vma = vmp->vm_next;
	
	if(prev_vmp == NULL)
	{
		mm->used_list =next_vma;
	}else{
		prev_vmp ->vm_next = next_vma;
	}
	
	return vmp;
}

void move_vma_to_free_list(struct mm_struct *mm, struct vm_area_struct *mpnt)
{
	struct vm_area_struct *prev, *vma;
	
	mpnt ->vm_next =NULL;

	if(!mm->free_list)
	{
		mm->free_list = mpnt;
		return ;
	}
	
	vma = find_free_vma_prev(mm, mpnt->vm_start, &prev);

	if(prev ==NULL)
	{
		if(vma !=NULL)
		{
			mpnt->vm_next = vma;
			mm->free_list = mpnt;
			merge_vmas(mm, mpnt, vma);
		}else{
			mm->free_list = mpnt;
		}
		
	}else{ 

		if(vma !=NULL){
			mpnt->vm_next =vma;
			prev->vm_next =mpnt;
			merge_vmas(mm, mpnt, vma);
			merge_vmas(mm, prev, mpnt);
		}else{
			mpnt->vm_next =NULL;
			prev->vm_next =mpnt;
			merge_vmas(mm, prev, mpnt);
		}
	}
}

void insert_vm_struct(struct mm_struct *mm, unsigned long addr_start, unsigned long len)
{
	struct vm_area_struct * mpnt, *vma,*pprev;

	mpnt=sm_malloc(sizeof(struct vm_area_struct));
	
	if(!mpnt)
	{
		pr_debug("%s: sm_malloc failed\n\r",__FUNCTION__);
		return;
	}
	
	pr_debug("%s:sm_malloc: addr: %x\n\r",__func__,mpnt);
	
	mpnt->vm_start = addr_start;
	mpnt->vm_end = (addr_start+len);
	
	/*NOTE: ca7 & cr5 should not call the same bbu_vm_ops */
	mpnt->vm_ops =&bbu_vm_ops;  
	mpnt->vm_mm  = mm;
	mpnt ->vm_next = NULL;

	vma = find_free_vma_prev(mm, addr_start, &pprev);

	pr_debug("%s: vma: 0x%x,pprev: 0x%x\n\r",__FUNCTION__,vma,pprev);
	
	if(pprev ==NULL)
	{
		if(vma !=NULL)
		{
			mpnt->vm_next = vma;
			mm->free_list = mpnt;
			merge_vmas(mm, mpnt, vma);
		}else{
			mm->free_list = mpnt;
		}
	}else{ 

		if(vma !=NULL){
			mpnt->vm_next =vma;
			pprev->vm_next =mpnt;
			merge_vmas(mm, mpnt, vma);
			merge_vmas(mm, pprev, mpnt);
		}else{
			mpnt->vm_next =NULL;
			pprev->vm_next =mpnt;
			merge_vmas(mm, pprev, mpnt);
		}
	}
}
void move_vma_to_used_list(struct mm_struct *mm, struct vm_area_struct *mpnt)
{
	struct vm_area_struct *prev, *vma;
	
	mpnt->vm_next =NULL;
	
	if(mm->used_list == NULL)
	{
		pr_debug("mm_used_list NULL\n\r");
		pr_debug("after insert vma used list: [0x%x(%dM)->0x%x(%dM)]\n\r"
					,mpnt->vm_start,mpnt->vm_start >>20
					,mpnt->vm_end,mpnt->vm_end >>20);
		
		mm->used_list = mpnt;
		return;
	}
	
	vma = find_used_vma_prev(mm, mpnt->vm_start, &prev);
	if(prev ==NULL)
	{
		pr_debug("prev is NULL\n\r");
		if(vma !=NULL)
		{
			pr_debug("vma is not NULL\n\r");
			mpnt->vm_next = vma;
			mm->used_list= mpnt;
			//merge_vmas(mm, mpnt, vma);
		}else{
			pr_debug("prev is NULL\n\r");
			mm->used_list = mpnt;
		}
			
	}else{ 
		pr_debug("prev is not null\n\r");
		if(vma !=NULL){
			pr_debug("vma is not  NULL\n\r");
			mpnt->vm_next =vma;
			prev->vm_next =mpnt;
			//merge_vmas(mm, mpnt, vma);
			//merge_vmas(mm, prev, mpnt);
		}else{
			pr_debug("vma is NULL\n\r");
			mpnt->vm_next =NULL;
			prev->vm_next =mpnt;
			//merge_vmas(mm, prev, mpnt);
		}
	}	
}

void * bbu_vm_malloc(uint32_t len,unsigned int mem_flags)
{
	struct mm_struct * mm;
	struct vm_area_struct * vma, * pprev, *vma2;

	len = PAGE_ALIGN(len);
	if (!len)
		return NULL;
	
	pr_debug("%s :len is %x\n\r",__FUNCTION__,len);
	
	if((mem_flags== NON_CACHEABLE) 
	    ||(mem_flags == NON_CACHE_BUFFER))
	    mm = NCNB_BUFF_PTR;
	else
	    mm = CB_BUFF_PTR;    
	    
	Q_SPIN_LOCK(&mm->mm_lock);
    request_vm_manager();
	    
	if (!vm_enough_memory(mm, len))
	{
		bbu_printf("%s :mem not enough\n\r",__FUNCTION__);
		goto out_error;
	}
	if(mm->free_list == NULL)
	{
		bbu_printf("no mem to allocate\n\r");
		goto out_error;
	}
	
	vma =find_free_vma_intersection(mm, len);
	if(vma == NULL)
	{
	       pr_debug("%s :can not find available vma\n\r",__FUNCTION__);
		goto out_error;	
	}

#ifdef CONFIG_MEM_HOTPLUG
	bbu_vm_malloc_prepare();
#endif

	find_free_vma_prev(mm, vma->vm_start,&pprev);

	vma2 = remove_free_vma(mm, vma, pprev,len);
	vma2->vm_flags = mem_flags;	
	
	//current do not invoke this function
	//vma2->vm_ops->mmap(vma2,vma2->vm_start,len);

	mem_malloc_free_hook(vma2->vm_start, len, ACQUIRE_MEM_RANGE);
	vma2->real_alloc_size = len;
	
	if(cpu_is_ca7())
	    vma2->vm_area_owner = CPU_CORE_CA7;
	else
	    vma2->vm_area_owner = CPU_CORE_CR5;
	    
	move_vma_to_used_list(mm,vma2);
	mm->total_vm -= len;
	mm->used_vm_size += len;
	mm->free_vm_size -= len;
	
	Q_SPIN_UNLOCK(&mm->mm_lock);
	release_vm_manager();
	return (void *)(vma->vm_start);
	
out_error:
	Q_SPIN_UNLOCK(&mm->mm_lock);
	release_vm_manager();
	return NULL;
}


void * bbu_vm_realloc(void * old_buffer,uint32_t new_size)
{
	struct mm_struct * mm;
	struct vm_area_struct * vma;
	uint32_t * new_addr;
	
	Q_SPIN_LOCK(&mm->mm_lock);
	request_vm_manager();
	
	vma =mm->used_list;
	
	while(vma)
	{
		if((((uint32_t)old_buffer) >= vma->vm_start)&&(((uint32_t)old_buffer) < vma->vm_end))
		{
			/*find the vma structure*/
			break;
		}	
		vma = vma->vm_next;
	}
	
	/* can not find the vma */
	if(vma == NULL) {
		bbu_printf("wrong addr parameter, please double check\n\r");
		goto out_error;
	}
	
	/* the vma is enough, then return*/
	if(new_size <= (vma->vm_end - vma->vm_start)) {
		vma->real_alloc_size = new_size; 
		Q_SPIN_UNLOCK(&mm->mm_lock);
		release_vm_manager();
		return old_buffer;
	} 
		
	Q_SPIN_UNLOCK(&mm->mm_lock);
	release_vm_manager();
	/* alloc a new vma */
	new_addr = bbu_vm_malloc(new_size,vma->vm_flags);
	if(new_addr == NULL) {
		return 	NULL;
	}
	
	memcpy(new_addr,old_buffer,vma->real_alloc_size);
	dsb();
	bbu_vm_free(old_buffer);
	old_buffer = new_addr;
	return old_buffer;

out_error:
	Q_SPIN_UNLOCK(&mm->mm_lock);
	release_vm_manager();
	return NULL;
}

void bbu_vm_free(void *p)
{
	struct mm_struct * mm;
	struct vm_area_struct * vma, * pprev;
	unsigned int  len;
	
	if (p == NULL)
		return ;
	
	//pr_debug("free addr is 0x%x\n\r",p);
	
	mm = get_mm_by_vaddr(p);

	Q_SPIN_LOCK(&mm->mm_lock);
	request_vm_manager();
	
	vma = find_used_vma_prev(mm,(unsigned long)p,&pprev);
	if(vma ==NULL){
		pr_debug("%s :free wrong buff\n\r",__FUNCTION__);
		Q_SPIN_UNLOCK(&mm->mm_lock);
		release_vm_manager();
		return; 
	}else{
		pr_debug("delete vma addr:[0x%x->0x%x]\n\r",vma->vm_start,vma->vm_end);
	}
	
	if(vma->vm_area_owner != get_cur_cpu())
    {
        bbu_printf("error!! vma is not freed by its owner cpu\n");
        return;
    }
    	
	if((vma->vm_start ) != ( (unsigned long )p))
	{
		 bbu_printf("%s :free wrong buff\n\r",__FUNCTION__);
		 Q_SPIN_UNLOCK(&mm->mm_lock);
		 release_vm_manager();
		 return;
	}
	
	len = (vma->vm_end -vma->vm_start);
	pr_debug("free mem len is 0x%x\n\r",len);
	
	remove_used_vma(mm, vma, pprev,len);
	
	mem_malloc_free_hook(vma->vm_start, len, RELEASE_MEM_RANGE);

	//vma->vm_ops->munmap(vma,vma->vm_start,len);
	vma->real_alloc_size = 0;
	
	move_vma_to_free_list(mm,vma);
	mm->total_vm += len;
	mm->free_vm_size += len;
	mm->used_vm_size -= len;
	p = NULL;
	
	Q_SPIN_UNLOCK(&mm->mm_lock);
    release_vm_manager();
	return;
}

void dump_free_vmas(void)
{
	struct mm_struct * mm = CB_BUFF_PTR;
	struct vm_area_struct * vma;
	
	bbu_printf("\n\r**************** CACHEABLE MEM ******************\n\r");
	bbu_printf("*total free mm size: %d MB\n\r",(mm->free_vm_size) >>20);
	bbu_printf("*total used mm size: %d MB\n\r",(mm->used_vm_size) >>20);
	bbu_printf("*************************************************\n\r");
	pr_debug("%s:mm: 0x%x\n\r",__func__,mm);
	
	bbu_printf("\n\r===========cacheable free mem vmas===============\n\r");
	Q_SPIN_LOCK(&mm->mm_lock);
	request_vm_manager();	
	vma =mm->free_list;
	pr_debug("%s:vma: 0x%x\n\r",__func__,vma);
	while(vma)
	{
	    //bbu_printf("vma: 0x%X\n\r",vma); 
		bbu_printf("vma: [0x%x(%dM)->0x%x(%dM)]\n\r"
					,vma->vm_start,vma->vm_start >>20
					,vma->vm_end,vma->vm_end >>20);
		vma = vma->vm_next;
	}
	
	Q_SPIN_UNLOCK(&mm->mm_lock);
	release_vm_manager();
	
	bbu_printf("=================================================\n\r");
	bbu_printf("\n\r==============cacheable used mem vmas============\n\r");
	
	Q_SPIN_LOCK(&mm->mm_lock);
	request_vm_manager();
	
	vma =mm->used_list;
	pr_debug("%s:vma: 0x%x\n\r",__func__,vma);
	while(vma)
	{
		bbu_printf("vma: [0x%x(%dM)->0x%x(%dM)], owner: %s\n\r"
					,vma->vm_start,vma->vm_start >>20
					,vma->vm_end,vma->vm_end >>20, cpu_name[min(vma->vm_area_owner,(NUM_OF_CPUS-1))]);
		vma =vma->vm_next;
	}
	
	Q_SPIN_UNLOCK(&mm->mm_lock);
	release_vm_manager();
	bbu_printf("=================================================\n\r");
}

void dump_used_vmas(void)
{
	struct mm_struct * mm = NCNB_BUFF_PTR;
	struct vm_area_struct * vma;
	
	bbu_printf("\n\r************** NON CACHEABLE MEM ****************\n\r");
	bbu_printf("*total free mm size: %d MB\n\r",(mm->free_vm_size) >>20);
	bbu_printf("*total used mm size: %d MB\n\r",(mm->used_vm_size) >>20);
	bbu_printf("*************************************************\n\r");
	bbu_printf("\n\r==========non_cacheable free mem vmas============\n\r");
	pr_debug("%s:mm: 0x%x\n\r",__func__,mm);	
	Q_SPIN_LOCK(&mm->mm_lock);
	request_vm_manager();
	vma =mm->free_list;
	pr_debug("%s:vma: 0x%x\n\r",__func__,vma);
	while(vma)
	{
	    //bbu_printf("vma: 0x%X\n\r",vma);
	    
		bbu_printf("vma: [0x%x(%dM)->0x%x(%dM)]\n\r"
					,vma->vm_start,vma->vm_start >>20
					,vma->vm_end,vma->vm_end >>20);
		vma =vma->vm_next;
	}
	
	Q_SPIN_UNLOCK(&mm->mm_lock);
	release_vm_manager();
	
	bbu_printf("=================================================\n\r");
	bbu_printf("\n\r==========non_cacheable used mem vmas============\n\r");
	
	Q_SPIN_LOCK(&mm->mm_lock);
	request_vm_manager();
	vma =mm->used_list;
	pr_debug("%s:vma: 0x%x\n\r",__func__,vma);
	while(vma)
	{
		bbu_printf("vma: [0x%x(%dM)->0x%x(%dM)], owner: %s\n\r"
					,vma->vm_start,vma->vm_start >>20
					,vma->vm_end,vma->vm_end >>20, cpu_name[min(vma->vm_area_owner,(NUM_OF_CPUS-1))]);
		vma =vma->vm_next;
	}
	
	Q_SPIN_UNLOCK(&mm->mm_lock);
	release_vm_manager();
	bbu_printf("=================================================\n\r");
}

static int ddr_size_mb()
{
	uint32_t reg, ddr_size_in_MB = 0,cs_size;
	
	reg = BU_REG_READ(0xC0100200);
	if(BIT_0&reg)
	{
		cs_size = (1<<(((reg&0X1F0000)>>16)-4));
		ddr_size_in_MB += cs_size;
	}
	
	reg = BU_REG_READ(0xC0100204);
	if(BIT_0&reg)
	{
		cs_size = (1<<(((reg&0X1F0000)>>16)-4));
		ddr_size_in_MB += cs_size;
	}
	
	bbu_printf("ddr size: %d MB\n\r",ddr_size_in_MB);
	return ddr_size_in_MB;
}

int is_mem_invalid(uint32_t addr_end)
{
	/*	
 	*if addr is out of DDR range, return 1
 	*/
 	if(addr_end > (ddr_size_mb() << 20))
 		return 1;
 			
 	return 0;
}

int is_mem_allocated(uint32_t addr_st,uint32_t addr_end)
{
	struct mm_struct * mm;
	struct vm_area_struct * vma;	
	
	mm = get_mm_by_vaddr(addr_st);
	
	Q_SPIN_LOCK(&mm->mm_lock);
	request_vm_manager();
	
	vma = mm->used_list;
	while(vma)
	{
		if(((addr_st >= vma->vm_start)&&(addr_st < vma->vm_end))
			|| ((addr_end > vma->vm_start)&&(addr_end <= vma->vm_end)))
		{
			Q_SPIN_UNLOCK(&mm->mm_lock);
			release_vm_manager();
			return 1;
		}	
		vma =vma->vm_next;
	}
	
	Q_SPIN_UNLOCK(&mm->mm_lock);
	release_vm_manager();
	return 0;
}


void vm_minimum_free_area_init(struct mm_struct *mm)
{
	(void)mm;
}

extern uint32_t bbu_malloc_buffer;
extern uint32_t bbu_malloc_buffer_botton;

#if CONFIG_CA7
void  vma_init(void)
{
	unsigned int i;
	struct mm_struct * mm;
	
	sm_malloc_init();
	bbu_ncnb_mm_struct = VM_MAMAGER_BUFF;
    bbu_cb_mm_struct = (VM_MAMAGER_BUFF+4);
    
    pr_debug("bbu_ncnb_mm_struct = 0x%x\n\r",bbu_ncnb_mm_struct);
    pr_debug("bbu_cb_mm_struct = 0x%x\n\r",bbu_cb_mm_struct);
    
#if 0
	bbu_mem_ranges[MEM_RANGE_CODE_AREA].addr_start 	= CODE_AREA_ST;
	bbu_mem_ranges[MEM_RANGE_CODE_AREA].addr_len   	= (((kern_image_size >>20)+1)<<20);	
	bbu_mem_ranges[MEM_RANGE_MALLOC].addr_start    	= bbu_malloc_buffer;
	bbu_mem_ranges[MEM_RANGE_MALLOC].addr_len 	   	= bbu_malloc_buffer_botton - bbu_malloc_buffer;	
#endif	
	
	bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF].addr_start	= ALIGN_1MB(vm_cb_buffer_start);
	bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF].addr_len 	= (vm_cb_buffer_len&0xFFF00000);	
	
	bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_start	= ALIGN_1MB(vm_ncnb_buffer_start);
	bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_len 	= (vm_ncnb_buffer_len&0xFFF00000);	
	
#if 0
	bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_start	= ALIGN_1MB(vm_ncnb_buffer_start);
	bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_len 	= 
		min(CORE_SHARE_ST - bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_start,
			vm_ncnb_buffer_len);	
	if((bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_len + vm_ncnb_buffer_len)
			< CORE_SHARE_ST)
	{
		bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF].addr_start	
			=  ALIGN_1MB(bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_start 
				+ vm_ncnb_buffer_len);
		bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF].addr_len 	= CORE_SHARE_ST - bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF].addr_start;
	}
#endif
    //bbu_printf("&vm_ncnb_buffer_start:0x%x\n\r",&vm_ncnb_buffer_start);	
	//bbu_printf("vm_ncnb_buffer_start:0x%x\n\r",vm_ncnb_buffer_start);
	//bbu_printf("vm_ncnb_buffer_len:0x%x\n\r",vm_ncnb_buffer_len);
	mm = (struct mm_struct *)sm_malloc(sizeof(struct mm_struct));
	if(mm != NULL)
	{
	    NCNB_BUFF_PTR = mm;
	    pr_debug("mm = 0x%x,NCNB_BUFF_PTR = 0x%x\n\r",mm,NCNB_BUFF_PTR);
	}
	else
	{
	    bbu_printf("%s: error1: sm_malloc failed\n\r",__func__);
	}
	
	mm = (struct mm_struct *)sm_malloc(sizeof(struct mm_struct));
	if(mm != NULL)
	{
	    CB_BUFF_PTR = mm;
	    pr_debug("mm = 0x%x,CB_BUFF_PTR = 0x%x\n\r",mm,CB_BUFF_PTR);
	}
	else
	{
	    bbu_printf("%s: error2: sm_malloc failed\n\r",__func__);
	}
	
	mm = CB_BUFF_PTR;		
	mm->free_list =NULL;
	mm->used_list =NULL;
	build_mmap_avl(mm);
	mm->minimum_free_vma = NULL;
	
	for(i=0; i<ARRAY_SIZE(bbu_mem_ranges); i++)
	{
		if(bbu_mem_ranges[i].mem_range_state!=DDR_RANGE_ALLOCATED_USED 
			&& bbu_mem_ranges[i].mem_range_state!=DDR_RANGE_RESERVED
			&& i == MEM_RANGE_ALLOC_BUFF)
		{
			insert_vm_struct(mm, bbu_mem_ranges[i].addr_start, bbu_mem_ranges[i].addr_len);
			mm->total_vm +=bbu_mem_ranges[i].addr_len;
			mm->free_vm_size = bbu_mem_ranges[i].addr_len;
			mm->used_vm_size = 0;
		}
	}
	
	vm_minimum_free_area_init(mm);
	
	mm = NCNB_BUFF_PTR;		
	mm->free_list = NULL;
	mm->used_list = NULL;
	build_mmap_avl(mm);
	mm->minimum_free_vma = NULL;
	
	for(i=0; i<ARRAY_SIZE(bbu_mem_ranges); i++)
	{
		if(bbu_mem_ranges[i].mem_range_state!=DDR_RANGE_ALLOCATED_USED 
			&& bbu_mem_ranges[i].mem_range_state!=DDR_RANGE_RESERVED
			&& i == MEM_RANGE_ALLOC_BUFF_NCNB)
		{
			insert_vm_struct(mm, bbu_mem_ranges[i].addr_start, bbu_mem_ranges[i].addr_len);
			mm->total_vm +=bbu_mem_ranges[i].addr_len;
			mm->free_vm_size = bbu_mem_ranges[i].addr_len;
			mm->used_vm_size = 0;
		}
	}
	
	vm_minimum_free_area_init(mm);
		
	pr_debug("vm inited\n\r");
	
}
#else
void  vma_init(void)
{
	unsigned int i;
      
	bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF].addr_start	= ALIGN_1MB(shared_vm_cb_start);
	bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF].addr_len 	= (shared_vm_cb_len&0xFFF00000);	
	bbu_printf("cacheable buff: [0x%x, 0x%x]\n\r",
			bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF].addr_start,
			bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF].addr_start
			+ bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF].addr_len);
			
	bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_start	= ALIGN_1MB(shared_vm_ncnb_start);
	bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_len 	= (shared_vm_ncnb_len&0xFFF00000);	
	bbu_printf("non-cacheable buff: [0x%x, 0x%x]\n\r",
			bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_start,
			bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_start
			+ bbu_mem_ranges[MEM_RANGE_ALLOC_BUFF_NCNB].addr_len);
}
#endif
void test_vm()
{
#if 0
	dump_free_vmas();
	dump_used_vmas();

	int i;
	unsigned char *p1,*p2,*p3,*p4;
	
	bbu_printf("malloc p1: 0x4\n\r");
	p1=bbu_vm_malloc(0x4, CACHEABLE);
	if(p1 == NULL)
	{
		bbu_printf("return P1 NULL\n\r");
	}else{
		
		bbu_printf("return P1= 0x%x\n\r",p1);
	}
	
	bbu_printf("malloc p2: 0x1\n\r");
	p2=bbu_vm_malloc(0x1, CACHEABLE);
	if(p2 == NULL)
	{
		bbu_printf("return P2 NULL\n\r");
	}else{
		
		bbu_printf("return P2= 0x%x\n\r",p2);
	}
	
	bbu_printf("malloc p3: 0x40000\n\r");
	p3=bbu_vm_malloc(0x40000,CACHEABLE);
	if(p3 == NULL)
	{
		bbu_printf("return P3 NULL\n\r");
	}else{
		
		bbu_printf("return P3= 0x%x\n\r",p3);
	}	
	bbu_printf("malloc p4: 0x400003\n\r");
	p4=bbu_vm_malloc(0x400003,CACHEABLE);
	if(p4 == NULL)
	{
		bbu_printf("return P4 NULL\n\r");
	}else{
		
		bbu_printf("return P4= 0x%x\n\r",p4);
	}	
	
	dump_free_vmas();
	dump_used_vmas();
	
	bbu_printf("free p1\n\r");
	bbu_vm_free(p1);
	
	bbu_printf("free p2\n\r");
	bbu_vm_free(p2);
	
	bbu_printf("free p3\n\r");
	bbu_vm_free(p3);
	
	bbu_printf("free p4\n\r");
	bbu_vm_free(p4);
	
	dump_free_vmas();
	dump_used_vmas();
	
	p2=bbu_vm_malloc(0x1, CACHEABLE);
	if(p2 == NULL)
	{
		bbu_printf("return P2 NULL\n\r");
	}else{
		
		bbu_printf("return P2= 0x%x\n\r",p2);
	}
	
	bbu_printf("malloc p3: 0x40000\n\r");
	p3=bbu_vm_malloc(0x40000,CACHEABLE);
	if(p3 == NULL)
	{
		bbu_printf("return P3 NULL\n\r");
	}else{
		
		bbu_printf("return P3= 0x%x\n\r",p3);
	}
	
	dump_free_vmas();
	dump_used_vmas();
	
	bbu_printf("free p4\n\r");
	bbu_vm_free(p4);
	for(i=0 ;i<20; i++)
		bbu_vm_malloc(1000+i*10, CACHEABLE);
		
	if(is_mem_allocated(0))
		bbu_printf("address 0 is malloced\n\r");
	else
		bbu_printf("address 0 is not malloced\n\r");
		
	if(is_mem_allocated(0x8600000))
		bbu_printf("address 0x8600000 is malloced\n\r");
	else
		bbu_printf("address 0x8600000 is not malloced\n\r");
		
	if(is_mem_allocated(0x9800000))
		bbu_printf("address 0x9800000 is malloced\n\r");
	else
		bbu_printf("address 0x9800000 is not malloced\n\r");
		
	if(is_mem_allocated(0x9900000))
		bbu_printf("address 0x9900000 is malloced\n\r");
	else
		bbu_printf("address 0x9900000 is not malloced\n\r");

	if(is_mem_allocated(0xae00000))
		bbu_printf("address 0xae00000 is malloced\n\r");
	else
		bbu_printf("address 0xae00000 is not malloced\n\r");
		
	dump_free_vmas();
	dump_used_vmas();
	
	
	bbu_printf("=========bbu_vm_realloc test begin========\n\r");
	bbu_printf("now malloc 4 byte buffer");
	p1=bbu_vm_malloc(0x4, CACHEABLE);
	if(p1 == NULL)
	{
		bbu_printf("return P1 NULL\n\r");
	}else{
		
		bbu_printf("return P1= 0x%x\n\r",p1);
		bbu_printf("write 0x1234 to %x\n\r",p1);
		BU_REG_WRITE(p1,0x1234);
	}
	
	dump_free_vmas();
	dump_used_vmas();
	
	
	
	bbu_printf("now realloc 100 byte buffer\n\r");
	p1 = bbu_vm_realloc(p1,100);
	if(p1 == NULL)
	{
		bbu_printf("return P1 NULL\n\r");
	}else{
		
		bbu_printf("return P1= 0x%x\n\r",p1);
		bbu_printf("content at %x is: %x\n\r",p1, BU_REG_READ(p1));
		bbu_printf("write 0x1234 to %x\n\r",p1+96);
		BU_REG_WRITE(p1+96,0x1234);
	}
	dump_free_vmas();
	dump_used_vmas();


	bbu_printf("now realloc 0x100000 byte buffer\n\r");
	p1 = bbu_vm_realloc(p1,0x100000);
	if(p1 == NULL)
	{
		bbu_printf("return P1 NULL\n\r");
	}else{
		
		bbu_printf("return P1= 0x%x\n\r",p1);
		bbu_printf("content at %x is: %x\n\r",p1+96, BU_REG_READ(p1+96));
		bbu_printf("write 0x1234 to %x\n\r",p1+0xffffc);
		BU_REG_WRITE(p1+0xffffc,0x1234);
	}
	dump_free_vmas();
	dump_used_vmas();
	
	
	bbu_printf("now realloc 0x100004 byte buffer\n\r");
	p1 = bbu_vm_realloc(p1,0x100004);
	if(p1 == NULL)
	{
		bbu_printf("return P1 NULL\n\r");
	}else{
		
		bbu_printf("return P1= 0x%x\n\r",p1);
		bbu_printf("content at %x is: %x\n\r",p1+0xffffc, BU_REG_READ(p1+0xffffc));
		bbu_printf("write 0x1234 to %x\n\r",p1+0x100000);
		BU_REG_WRITE(p1+0x100000,0x1234);
	}
	dump_free_vmas();
	dump_used_vmas();	
	
	bbu_printf("now realloc 0x10004 byte buffer\n\r");
	p1 = bbu_vm_realloc(p1,0x100004);
	if(p1 == NULL)
	{
		bbu_printf("return P1 NULL\n\r");
	}else{
		
		bbu_printf("return P1= 0x%x\n\r",p1);
	}
	
	bbu_printf("content at %x is: %x\n\r",p1, BU_REG_READ(p1));
	bbu_printf("content at %x is: %x\n\r",p1+96, BU_REG_READ(p1+96));
	bbu_printf("content at %x is: %x\n\r",p1+0xffffc, BU_REG_READ(p1+0xffffc));
	bbu_printf("content at %x is: %x\n\r",p1+0x100000, BU_REG_READ(p1+0x100000));
	
	dump_free_vmas();
	dump_used_vmas();
		
	bbu_printf("now free p1 memory\n\r");
	bbu_vm_free(p1);
	
	dump_free_vmas();
	dump_used_vmas();
	
	bbu_printf("==========bbu_vm_realloc test end=========\n\r");
#endif
}


