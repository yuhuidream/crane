#ifndef __VM_MANAGER_h__
#define __VM_MANAGER_h__

#include "qspinlock.h"

enum MEM_RANGE_TYPE{
	MEM_RANGE_GCU = 0,
	MEM_RANGE_CODE_AREA,
	MEM_RANGE_MALLOC,
	MEM_RANGE_CP_CODE,
	MEM_RANGE_CP_MALLOC,
	MEM_RANGE_AP_CP_BUFF,
	MEM_RANGE_AP_CODE,
	MEM_RANGE_AP_MALLOC,
	MEM_RANGE_ALLOC_BUFF_NCNB,
	MEM_RANGE_ALLOC_BUFF, /*cacheable buffer*/
	MEM_RANGE_MAX,
};

/* PAGE_SHIFT determines the page size, use 4kB page*/
#define PAGE_SHIFT	20   //TODO,1M section mode
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))
#define PAGE_ALIGN(addr)	(((addr)+PAGE_SIZE-1)&PAGE_MASK)

/*
 * mem space: 1GB
 */
 
 #define DDR_MEM_SIZE_1G 	(0x40000000UL)
 #define DDR_MEM_SIZE_512M 	(0x20000000UL)
 

/* Look up the first VMA which intersects the interval start_addr..end_addr-1,
 *NULL if none.  Assume start_addr < end_addr. 
 */
 
 //extern struct vm_area_struct * find_vma(struct mm_struct * mm, unsigned long addr);
 
/*
 * These are the virtual MM functions - maping and
 * unmapping it, pointer to the functions called when a allocation ocurs. 
 */
 
struct vm_area_struct;
 
struct vm_operations_struct {
	void (*mmap)(struct vm_area_struct *area, unsigned long addr_start, unsigned long addr_len);
	void (*munmap)(struct vm_area_struct *area, unsigned long addr_start, unsigned long addr_len);
};

/*
 * This struct defines a memory VMM memory area. There is one of these
 * per VM-area.  A VM area is any part of the virtual memory
 * space.
 */
struct vm_area_struct {
	struct mm_struct * vm_mm;	/* VM area parameters */
	unsigned long vm_start;
	unsigned long vm_end;
	unsigned long real_alloc_size;
	unsigned char vm_area_owner;
	/* linked list of VM areas per task, sorted by address */
	struct vm_area_struct *vm_next;
	unsigned int vm_flags;/*used to determine MMU options*/
	struct vm_operations_struct * vm_ops;
};

struct mm_struct {
	QSpinlock mm_lock;
	struct vm_area_struct *free_list;		/* free list of VMAs */
	struct vm_area_struct *used_list;		/* used list of VMAs */
	struct vm_area_struct *minimum_free_vma;	/* last find_vma result */
	unsigned long size_minimum_free_vma;
	struct vm_area_struct *mmap_avl;
	unsigned long total_vm;
	unsigned long free_vm_size;
	unsigned long used_vm_size;
};
#define ALIGN_1MB(x) ((x+0xFFFFF)&0xFFF00000)
enum {
    CPU_CORE_CA7 = 0,    
    CPU_CORE_CR5,
    NUM_OF_CPUS,
};
#endif
