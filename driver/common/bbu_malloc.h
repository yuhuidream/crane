#ifndef _H_BBU_MALLOC_H_
#define _H_BBU_MALLOC_H_
#include "qp_port.h"
#include "predefines.h"

/* this macro used to indicate that if memory overrun mechanism is used */
#define BBU_MEM_OVERRUN

#define BBU_MALLOC_CB_BUF_LEN    CR5_MALLOC_CB_LEN
#define BBU_MALLOC_NCNB_BUF_LEN  CR5_MALLOC_NCNB_LEN

typedef uint32_t MBlockSize;

/* type of memory pool used for malloc */
typedef struct MemAllocPoolTag{
    uint32_t blockSize;            //bytes
    uint32_t poolSize;
}MemAllocPool;

/* type of memory block used for malloc */
typedef struct MemBlockTag{
    uint32_t allocSize_;  /* The real requested size */
    uint8_t  poolId_;
    uint8_t  refCtr_;
    uint16_t reserved_;   /* not used for now */
}MemBlock;

#define BBU_MPOOL(cache)     ((cache)?BBU_mpool_:BBU_mpoolNc_)
#define BBU_POOL_MAX(cache)  ((cache)?BBU_maxMpool_:BBU_maxNcMpool_)

/* this macro defines the type of memory pool used for malloc */
#define BBU_MPOOL_TYPE_    QMPool
/* this macro defines the max number of memory pool used for malloc */
#define BBU_MAX_MPOOL      16
#define BBU_NC_MAX_MPOOL   4

/* this macro used to get block size of a memory pool used for malloc */
#define BBU_MPOOL_BLOCK_SIZE_(p_)    ((p_).blockSize)
/* this macro used to get the pool ID which a memory block belongs to */
#define MEM_POOL_ID(_b)         (((MemBlock *)(_b))->poolId_)
/* this macro used to get reference count of a memory block */
#define MBLOCK_REF_CTR(_b)      (((MemBlock *)(_b))->refCtr_)
#define MBLOCK_REF_CTR_SET(_b)  (((MemBlock *)(_b))->refCtr_ = 1)
#define MBLOCK_REF_CTR_CLR(_b)  (((MemBlock *)(_b))->refCtr_ = 0)
#define MBLOCK_IS_ALLOC(_b)     (((MemBlock *)(_b))->refCtr_)
#define MBLOCK_IS_FREE(_b)      ( ((MemBlock *)(_b))->refCtr_ == 0)

/* this macro used to get the real requested size of a memory block */
#define MBLOCK_ALLOC_SIZE(_b)  (((MemBlock *)(_b))->allocSize_)

#ifndef BBU_MEM_OVERRUN
/* 1B: pool id, 1B: refCtr 2B:reserved 4B: allocSize */
#define HEAD_UNUSED_WORDS      (2)
#else
/* 1B: pool id, 1B: refCtr 2B:reserved 4B: allocSize 4B: not used */
/* Neon load/store instruction need 8-byte align */
#define HEAD_UNUSED_WORDS      (4)
#endif

#define UNUSED_BLOCK_BYTES     (HEAD_UNUSED_WORDS*4+4)

#define BBU_MPOOL_INIT_(p_, poolSto_, poolSize_, evtSize_) \
    (QMPool_init(&(p_), poolSto_, poolSize_, evtSize_))

#define BBU_MPOOL_GET_(p_, b_)       ((b_) = (void *)QMPool_get(&(p_), 1))
#define BBU_MPOOL_PUT_(p_, e_)       (QMPool_put(&(p_), e_))

#ifndef ADDR_ALIGN
#define ADDR_ALIGN(addr_, align_)         ((addr_+align_-1)&(~(align_-1)))
#endif

#define BYTE(x)                (x)
#define KBYTE(x)            (1024*x)
#define MBYTE(x)            (1024*1024*x)

#define MBLOCK_OVERRUN_END(bPtr_, cache_)    \
  (*(uint32_t *)((uint32_t)bPtr_ + BBU_MPOOL(cache_)[(MEM_POOL_ID(bPtr_)-1)].blockSize-4))

#define MBLOCK_OVERRUN_START(bPtr_)   \
    (*(uint32_t *)((uint32_t)bPtr_ + 8))

#define MEM_OVERRUN_VALUE     0xBDBDBDBD
#define BACKWARD_OVERRUN      BIT_0
#define FORWARD_OVERRUN       BIT_1

#ifdef CONFIG_SMP
#define BBU_MALLOC_LOCK()       spin_lock(&malloc_lock)
#define BBU_MALLOC_UNLOCK()     spin_unlock(&malloc_lock)
#else
#define BBU_MALLOC_LOCK()
#define BBU_MALLOC_UNLOCK()
#endif

void BBU_mallocInit(void);
void *bbu_malloc(MBlockSize bSize);
void *bbu_nc_malloc(MBlockSize bSize);
void bbu_mfree(void *bPtr);
void *bbu_calloc(uint32_t n,  uint32_t size);
void *bbu_realloc(void *bPtr, MBlockSize bSize);
extern void * bbu_vm_malloc(uint32_t len,unsigned int mem_flags);
extern void * bbu_vm_realloc(void * old_buffer,uint32_t new_size);
extern void bbu_vm_free(void *p);
extern void mem_hotplug_init(void);
extern int is_mem_allocated(uint32_t addr, uint32_t addr_end);
extern int is_mem_invalid(uint32_t addr);
/*
* try to get aquire_gcu_buffer_lock, ret 1 if lock free
* else  ret 0; if acquired the lock, need to release the
* lock when the gcu buffer is not in use
*/
#define NO_OWNER 0
#define OWNER_GCU_AO 1
#define OWNER_MTSP_AO 2

extern int acquire_gcu_buffer_lock(int applicant);
extern void release_gcu_buffer_lock(int applicant);

/* check the addr ragion to use bbu_free or bbu_vm_free */
#define is_vm_addr(__addr) (is_mem_allocated(__addr))
#endif
