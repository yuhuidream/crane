#include "bbu_malloc.h"
#include "bbu.h"
#include "set_mmu.h"
#include "qf_port.h"
#include "cache_ops.h"
#include "spinlock.h"
#include "qassert.h"
#include <string.h>
#include <stdlib.h>

extern uint32_t bbu_malloc_buffer;
uint32_t bbu_malloc_buffer_botton = 0;

Q_DEFINE_THIS_MODULE("bbu_malloc")

#ifdef CONFIG_CA7
/* Memory pools used for bbu_malloc, everytime the pools and their length are
* changed, the macro BBU_MALLOC_CB_BUF_LEN and BBU_MAX_MPOOL need to be tuned
* accordingly.
*/
static MemAllocPool mem_alloc_pool[] = {
    {BYTE(128),    KBYTE(512)},      //0.5MB
    {BYTE(256),    KBYTE(512)},      //0.5MB
    {BYTE(512),    KBYTE(512)},      //0.5MB
    {KBYTE(4),     KBYTE(512)},      //0.5MB
    {KBYTE(8),     KBYTE(512)},      //0.5MB
    {KBYTE(16),    KBYTE(512)},      //0.5MB
    {KBYTE(32),    KBYTE(512)},      //0.5MB
    {KBYTE(64),    KBYTE(512)},      //0.5MB
    {KBYTE(128),   MBYTE(1)},        //1MB
    {KBYTE(256),   MBYTE(1)},        //1MB
    {0, 0}
};

/* Memory pools used for bbu_nc_malloc, everytime the pools and their length are
* changed, the macro BBU_MALLOC_NCNB_BUF_LEN and BBU_NC_MAX_MPOOL need to be tuned
* accordingly.
*/
static MemAllocPool mem_nc_alloc_pool[] = {
    {BYTE(128),    KBYTE(512)},      //0.5MB
    {BYTE(256),    KBYTE(512)},      //0.5MB
    {BYTE(512),    KBYTE(512)},      //0.5MB
    {KBYTE(4),     KBYTE(512)},      //0.5MB
    {0, 0}
};
#endif

#ifdef CONFIG_CR5
/* Memory pools used for bbu_malloc..., for CR5, it's 6MB */
static MemAllocPool mem_alloc_pool[] = {
    {BYTE(256),    KBYTE(512)},      //0.5MB
    {KBYTE(4),     KBYTE(512)},      //0.5MB
    {KBYTE(16),    KBYTE(512)},      //0.5MB
    {KBYTE(64),    KBYTE(512)},      //0.5MB
    {KBYTE(128),   KBYTE(512)},      //0.5MB
    {KBYTE(256),   KBYTE(512)},      //0.5MB
    {KBYTE(512),   MBYTE(1)},        //1MB
    {0, 0}
};

/* Memory pools used for bbu_nc_malloc... */
static MemAllocPool mem_nc_alloc_pool[] = {
    {BYTE(128),    KBYTE(256)},      //0.25MB
    {BYTE(256),    KBYTE(256)},      //0.25MB
    {BYTE(512),    KBYTE(256)},      //0.25MB
    {KBYTE(4),     KBYTE(256)},      //0.25MB
    {0, 0}
};
#endif

static BBU_MPOOL_TYPE_ BBU_mpool_[BBU_MAX_MPOOL];
static BBU_MPOOL_TYPE_ BBU_mpoolNc_[BBU_NC_MAX_MPOOL];

static uint8_t BBU_maxMpool_;
static uint8_t BBU_maxNcMpool_;

uint32_t bbu_malloc_buffer;
static uint32_t bbu_nc_malloc_buffer;

extern uint32_t kern_image_size;

/*..........................................................................*/
static void 
__bbu_mpoolInit(void *mpoolSto, uint32_t mpoolSize, MBlockSize bSize){
    Q_REQUIRE(BBU_maxMpool_ < (uint8_t)Q_DIM(BBU_mpool_));
             /* please initialize memory pools in ascending order of bSize: */
    Q_REQUIRE((BBU_maxMpool_ == (uint8_t)0)
           || (BBU_MPOOL_BLOCK_SIZE_(BBU_mpool_[BBU_maxMpool_ - 1]) < bSize));
                /* perfom the platform-dependent initialization of the pool */
    BBU_MPOOL_INIT_(BBU_mpool_[BBU_maxMpool_], mpoolSto, mpoolSize, bSize);
    ++BBU_maxMpool_;                                       /* one more pool */
}

/*..........................................................................*/
static void
__bbu_mpoolNcInit(void *mpoolSto, uint32_t mpoolSize, MBlockSize bSize){
    Q_REQUIRE(BBU_maxNcMpool_ < (uint8_t)Q_DIM(BBU_mpoolNc_));
             /* please initialize memory pools in ascending order of bSize: */
    Q_REQUIRE((BBU_maxNcMpool_ == (uint8_t)0) || 
          (BBU_MPOOL_BLOCK_SIZE_(BBU_mpoolNc_[BBU_maxNcMpool_ - 1]) < bSize));
                /* perfom the platform-dependent initialization of the pool */
    BBU_MPOOL_INIT_(BBU_mpoolNc_[BBU_maxNcMpool_], mpoolSto, mpoolSize,bSize);
    ++BBU_maxNcMpool_;                                     /* one more pool */
}

/*..........................................................................*/
static uint32_t
bbu_mpoolInit(MemAllocPool *pools, uint32_t startAddr, int cache){
   uint8_t idx = 0;
   uint32_t mpoolSto = startAddr;
   while(pools[idx].blockSize !=0 &&
         pools[idx].poolSize !=0){
        ((cache)?__bbu_mpoolInit:__bbu_mpoolNcInit)
                       ((void *)mpoolSto, pools[idx].poolSize, 
                                          pools[idx].blockSize);
        mpoolSto += pools[idx].poolSize;
        /* Neon load/store instruction need 8-byte align */
        mpoolSto = ADDR_ALIGN(mpoolSto,8);
        idx++;
        //bbu_printf(".");
    }
    return mpoolSto;
}

/*..........................................................................*/
void BBU_mallocInit(void){
    BBU_maxMpool_ = 0;
    BBU_maxNcMpool_ = 0;

    bbu_malloc_buffer = ADDR_ALIGN((kern_image_size+CODE_AREA_ST),0x100000);

                                              /* Initialize cacheable pools */
    bbu_nc_malloc_buffer = 
                    bbu_mpoolInit(mem_alloc_pool, bbu_malloc_buffer, 1);
    if((bbu_nc_malloc_buffer - bbu_malloc_buffer) != BBU_MALLOC_CB_BUF_LEN)
    {
       bbu_printf("FATAL ERROR: bbu malloc cacheable "
                  "buffer length doesn't match\n\r");
       Q_ASSERT(0);
    }

    bbu_malloc_buffer_botton = 
                    bbu_mpoolInit(mem_nc_alloc_pool, bbu_nc_malloc_buffer, 0);

    if((bbu_malloc_buffer_botton - bbu_nc_malloc_buffer) != 
        BBU_MALLOC_NCNB_BUF_LEN)
    {
       bbu_printf("FATAL ERROR: bbu malloc non-cacheable "
                  "buffer length doesn't match\n\r");
       Q_ASSERT(0);
    }
}

#ifdef CONFIG_SMP
spinlock_t malloc_lock  = {0};
#endif
/*..........................................................................*/
static void *__bbu_malloc(MBlockSize bSize, int cache) {
    void *bPtr = 0;
    MBlockSize bSizeBak = bSize;
    bSize += UNUSED_BLOCK_BYTES;
                                     /* get which kind of pool to allocate? */
    BBU_MPOOL_TYPE_ *bbu_mpool = BBU_MPOOL(cache);
    uint8_t bbu_pool_max = BBU_POOL_MAX(cache);
    QF_CRIT_STAT_TYPE state;
    QF_CRIT_ENTRY(state);
    BBU_MALLOC_LOCK();

    if(bSize > BBU_MPOOL_BLOCK_SIZE_(bbu_mpool[bbu_pool_max-1])){
       BBU_MALLOC_UNLOCK();
       QF_CRIT_EXIT(state);
       bPtr = (void *)bbu_vm_malloc(bSize, cache);
       return bPtr;
    }

              /* find the pool index that fits the requested event size ... */
    uint8_t idx = (uint8_t)0;
    while (bSize > BBU_MPOOL_BLOCK_SIZE_(bbu_mpool[idx])) {
        ++idx;
                                      /* cannot run out of registered pools */
        Q_ASSERT(idx < bbu_pool_max);
    }

    while(idx < bbu_pool_max){
        BBU_MPOOL_GET_(bbu_mpool[idx], bPtr);
        /* if the suitable pool doesn't have free block, try to get a
        *  bigger block from next pool 
        */
        if(bPtr == (void *)0){
            ++idx;
        }else{
            break;
        }
    }

    if(bPtr == (void *)0){
        BBU_MALLOC_UNLOCK();
        QF_CRIT_EXIT(state);
        bPtr = (void *)bbu_vm_malloc(bSize, cache);
        return bPtr;
    }

    MEM_POOL_ID(bPtr) = (uint8_t)(idx+1); /* store the pool ID in the block */
    MBLOCK_REF_CTR_SET(bPtr);                 /* Set mem block is allocated */
    MBLOCK_ALLOC_SIZE(bPtr) = bSizeBak;

#ifdef BBU_MEM_OVERRUN
    MBLOCK_OVERRUN_START(bPtr)        = MEM_OVERRUN_VALUE;
    MBLOCK_OVERRUN_END(bPtr, cache)   = MEM_OVERRUN_VALUE;
#endif

                 /* First 8 bytes used to store pool, refCtr and alloc size */
    bPtr = (void *)((uint32_t *)bPtr + HEAD_UNUSED_WORDS);

    BBU_MALLOC_UNLOCK();
    QF_CRIT_EXIT(state);
    return bPtr;
}

/*..........................................................................*/
void bbu_mfree(void *bPtr){

    QF_CRIT_STAT_TYPE state;
    QF_CRIT_ENTRY(state);
    BBU_MALLOC_LOCK();
    if(( bPtr == NULL || (uint32_t)bPtr < bbu_malloc_buffer)){
        bbu_printf(DBG_MSG"Free an invalid buffer 0x%08X\n\r",(uint32_t)bPtr);
        BBU_MALLOC_UNLOCK();
        QF_CRIT_EXIT(state);
        return;
    }

                                 /* address is not in the malloc pool range */
    if((uint32_t)bPtr >= bbu_malloc_buffer_botton){
        BBU_MALLOC_UNLOCK();
        QF_CRIT_EXIT(state);
        bbu_vm_free(bPtr);
        return;
    }

    int cache = 1;                                 /* is a cacheable block? */
    if((uint32_t)bPtr >= bbu_nc_malloc_buffer)
        cache = 0;

    BBU_MPOOL_TYPE_ *bbu_mpool = BBU_MPOOL(cache);
    uint8_t bbu_pool_max = BBU_POOL_MAX(cache);

    bPtr = (void *)((uint32_t *)bPtr - HEAD_UNUSED_WORDS);

    if(MBLOCK_IS_ALLOC(bPtr)){
        MBLOCK_REF_CTR_CLR(bPtr);                  /* Set mem block as free */
        uint8_t idx = (uint8_t)(MEM_POOL_ID(bPtr) - 1);

        if(idx < bbu_pool_max)
           QMPool_put(&bbu_mpool[idx], (void *)bPtr);
    }
    BBU_MALLOC_UNLOCK();
    QF_CRIT_EXIT(state);
}

/*..........................................................................*/
void *bbu_realloc(void *bPtr, MBlockSize bSize)
{
   Q_REQUIRE( (uint32_t)bPtr == 0 || ( (uint32_t)bPtr > bbu_malloc_buffer 
           && (uint32_t)bPtr < bbu_malloc_buffer_botton ) );
   
   void *newPtr, *bPtrOrig;
   
   if((uint32_t)bPtr == 0)
      return bbu_malloc(bSize);
                                                  /* Real pointer to MBlock */
   bPtrOrig = (void *)((uint32_t *)bPtr - HEAD_UNUSED_WORDS);
   
   uint8_t idx = (uint8_t)(MEM_POOL_ID(bPtrOrig) - 1);

   /* This Block is still enough for the realloc request */
   if(BBU_MPOOL_BLOCK_SIZE_(BBU_mpool_[idx])>=(bSize+UNUSED_BLOCK_BYTES))
      return bPtr;
   
   /* If the origin block is not enough, malloc a new buffer and copy the
      data of old buffer to the new one, then free the old one */
   newPtr = bbu_malloc(bSize);
   memcpy(newPtr, bPtr, MBLOCK_ALLOC_SIZE(bPtrOrig));
   bbu_mfree(bPtr);
   
   return newPtr;
}

/*..........................................................................*/
void *bbu_calloc(uint32_t n, uint32_t size)
{
   MBlockSize bSize;
   uint8_t *bPtr;
   uint32_t i = 0;
   
   bSize = n*size;
   bPtr = (uint8_t *)bbu_malloc(bSize);
   
   for(i=0; i<bSize; i++)
      *(bPtr+i) = 0;

   return ((void *)bPtr);
}

/*..........................................................................*/
void *bbu_malloc(MBlockSize bSize){
    return __bbu_malloc(bSize, CACHEABLE);
}

void *bbu_nc_malloc(MBlockSize bSize){
    return __bbu_malloc(bSize, NON_CACHEABLE);
}

/*..........................................................................*/
static int bbu_malloc_ref_ctr(void *bPtr){
    bPtr = (void *)((uint32_t *)bPtr - HEAD_UNUSED_WORDS);
    switch(MBLOCK_REF_CTR(bPtr)){
        case 0:
            bbu_printf("malloc-->  This memory block is free\n\r");
            break;
        case 1:
            bbu_printf("malloc-->  This memory block is allocated\n\r");
            break;
        default:
            bbu_printf("malloc-->  This is not a valid memory block or "
                       "overrun\n\r");
            break;
    }
    return MBLOCK_REF_CTR(bPtr);
}

/*..........................................................................*/
#ifdef BBU_MEM_OVERRUN
static int bbu_malloc_check_overrun(void *bPtr){
    int result = 0;
    int cache = 1;
    
    bPtr = (void *)((uint32_t *)bPtr - HEAD_UNUSED_WORDS);
    if(MBLOCK_REF_CTR(bPtr) != 1){
        bbu_printf("malloc-->  No need to check overrun for a non-allocated"
                   " block\n\r");
        return 0;
    }
    if((uint32_t)bPtr>= bbu_nc_malloc_buffer)
        cache = 0;
    if(MBLOCK_OVERRUN_END(bPtr, cache)   !=MEM_OVERRUN_VALUE)
        result |= FORWARD_OVERRUN;
    if(MBLOCK_OVERRUN_START(bPtr) != MEM_OVERRUN_VALUE)
        result |= BACKWARD_OVERRUN;
    bbu_printf("malloc-->  This memory block is ");
    if(result & FORWARD_OVERRUN){
        bbu_printf("forward ");
        if(result & BACKWARD_OVERRUN)
           bbu_printf("and backward ");
    }        
    else if(result & BACKWARD_OVERRUN)
        bbu_printf("backward ");
    else
        bbu_printf("not ");
    bbu_printf("overrun\n\r");
    return result;
}
#endif

/*..........................................................................*/
static void bbu_mfree_test(void *bPtr){
    if((uint32_t)bPtr < bbu_malloc_buffer || 
       (uint32_t)bPtr > bbu_malloc_buffer_botton)
        bbu_printf("malloc-->  This is not a valid allocated "
                   "block address\n\r");
    else{
        bbu_mfree(bPtr);
        bbu_printf("malloc-->  This memory block is freed now\n\r");
    }
    
    return;
}

/*..........................................................................*/
static void bbu_malloc_pool_info(void){
    int idx = 0;
    bbu_printf("\n\r  Block-Size  Total-Size   Free-Block   "
               "Total-Block   Start-Address   End-Address   Cacheable?\n\r");
    while(mem_alloc_pool[idx].blockSize !=0 
          &&mem_alloc_pool[idx].poolSize !=0){
        bbu_printf("   0x%06XB   0x%06XB      %-10d   ",
                 mem_alloc_pool[idx].blockSize,
                 mem_alloc_pool[idx].poolSize, BBU_mpool_[idx].nFree);
        bbu_printf("%-10d  0x%08X      0x%08X       Yes\n\r", 
                 BBU_mpool_[idx].nTot, 
                 (uint32_t)BBU_mpool_[idx].start, 
                 (uint32_t)BBU_mpool_[idx].end+mem_alloc_pool[idx].blockSize);
        idx ++;
    }
    idx = 0;
    while(mem_nc_alloc_pool[idx].blockSize !=0 
          &&mem_nc_alloc_pool[idx].poolSize !=0){
        bbu_printf("   0x%06XB   0x%06XB      %-10d   ",
                 mem_nc_alloc_pool[idx].blockSize,
                 mem_nc_alloc_pool[idx].poolSize, BBU_mpoolNc_[idx].nFree);
        bbu_printf("%-10d  0x%08X      0x%08X       No\n\r", 
            BBU_mpoolNc_[idx].nTot, 
            (uint32_t)BBU_mpoolNc_[idx].start, 
            (uint32_t)BBU_mpoolNc_[idx].end+mem_nc_alloc_pool[idx].blockSize);
        idx ++;
    }
}

/*..........................................................................*/
static void bbu_malloc_test_help(void){
    bbu_printf("\n\r malloc - BBU memory allocate and free test command\n\r");
    bbu_printf(" Usage: malloc [<option>] [<data>]\n\r");
    bbu_printf("  <option>: \n\r"
               "     -a  -- Allocate a cacheable memory block \n\r");
    bbu_printf("     -n  -- Allocate a non-cachable memory block \n\r");
    bbu_printf("     -f  -- Free a memory block  \n\r");
    bbu_printf("     -r  -- Check if a memory block is free or not\n\r");
#ifdef BBU_MEM_OVERRUN
    bbu_printf("     -o  -- Check if a memory block is overrun\n\r");
#endif
    bbu_printf("     -i  -- Print the bbu malloc pool infomation\n\r");
    bbu_printf("  <data>:\n\r"
#ifdef BBU_MEM_OVERRUN
               "     address -- when option is -f, -r, -o\n\r");
#else
               "     address -- when option is -f, -r\n\r");
#endif
    bbu_printf("     size    -- when option is -a\n\r");
}

/*..........................................................................*/
void bbu_malloc_test(int argc, uint8_t **argv){
    uint32_t size = 0, addr = 0;
    switch(argc){
        case 1:
            BBU_mallocInit();
            //bbu_printf("malloc-->  Too few arguments\n\r");
            break;
        case 2:
            if(!strcmp((const char*)argv[1], "/h") || 
                 !strcmp((const char*)argv[1], "--help")){
                 bbu_malloc_test_help();
            }
            else if(!strcmp((const char*)argv[1], "-i")){
               bbu_malloc_pool_info();
            }
            else if(!strcmp((const char*)argv[1], "-a") ||
                    !strcmp((const char*)argv[1], "-f") ||
                    !strcmp((const char*)argv[1], "-n") ||
                    !strcmp((const char*)argv[1], "-r") ||
                    !strcmp((const char*)argv[1], "-o")){
               bbu_printf("malloc-->  Need one more argument for size"
                          " or address\n\r");
            }
            else
                bbu_printf("malloc-->  The option is not correct\n\r");
            break;
        case 3:
            if(strcmp((const char*)argv[1], "-a") == 0){
                size = XpStrToValue((char *)argv[2]);
                addr = (uint32_t)bbu_malloc(size);
                if(addr)
                      bbu_printf("malloc-->  The allocated memory address is "
                                 "0x%08X\n\r", addr);
                else
                      bbu_printf("malloc-->  Can not allocate requested"
                                 " memory\n\r");
            }
            else if(strcmp((const char*)argv[1], "-n") == 0){
                size = XpStrToValue((char *)argv[2]);
                addr = (uint32_t)bbu_nc_malloc(size);
                if(addr)
                      bbu_printf("malloc-->  The allocated memory address is "
                                 "0x%08X\n\r", addr);
                else
                      bbu_printf("malloc-->  Can not allocate requested"
                                 " memory\n\r");
            }
            else if(strcmp((const char*)argv[1], "-f") == 0){
                addr = strtoul((const char*)argv[2],0,16);
                bbu_mfree_test((void *)addr);
            }
            else if(strcmp((const char*)argv[1], "-r") == 0){
                addr = strtoul((const char*)argv[2],0,16);
                bbu_malloc_ref_ctr((void *)addr);
            }
#ifdef BBU_MEM_OVERRUN
            else if(strcmp((const char*)argv[1], "-o") == 0){
                addr = strtoul((const char*)argv[2],0,16);
                bbu_malloc_check_overrun((void *)addr);
            }
#endif
            else
                bbu_printf("malloc-->  The option is not correct\n\r");
           break;
        default:
            bbu_printf("malloc-->  Too many arguments\n\r");
    }
    bbu_printf(BBU_PROMPT);
}
