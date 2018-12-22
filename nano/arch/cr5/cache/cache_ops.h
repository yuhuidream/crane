#ifndef _H_CACHE_OPS_H_
#define _H_CACHE_OPS_H_

#ifdef CONFIG_CACHE
/**
 * Function:    l1_dcache_clean_all
 * Description: Clean entire L1 D cache by set/way
 *              This has an effect only on the processor that performs the operation.
 * Argument:    None
 * Return:      None
**/
extern void l1_dcache_clean_all(void);

/**
 * Function:    l1_dcache_invalidate_all
 * Description: Invalidate entire L1 D cache by set/way
 *              This has an effect only on the processor that performs the operation.
 * Argument:    None
 * Return:      None
**/
extern void l1_dcache_invalidate_all(void);

/**
 * Function:    l1_dcache_invalidate_all
 * Description: flush(clean and invalidate) entire L1 D cache by set/way
 *              This has an effect only on the processor that performs the operation.
 * Argument:    None
 * Return:      None
**/
extern void l1_dcache_clean_invalidate_all(void);

/**
 * Function:    l1_icache_invalidate_all
 * Description: Invalidate entire L1 I cache to point of unification
 *              This has an effect only on the processor that performs the operation.
 * Argument:    None
 * Return:      None
**/
extern void l1_icache_invalidate_all(void);

/**
 * Function:    l1_icache_invalidate_all_is
 * Description: Invalidate entire L1 I cache to point of unification of all the processors
 *              in the same Inner Shareable shareability domain
 * Argument:    None
 * Return:      None
**/
extern void l1_icache_invalidate_all_is(void);

/**
 * Function:    l1_icache_invalidate_line
 * Description: Invalidate L1 I cache line by MVA
 * Argument:    MVA(Modified virtual address)
 * Return:      None
**/
void l1_icache_invalidate_line(uint32_t mva);

/**
 * Function:    l1_dcache_invalidate_line
 * Description: Invalidate L1 D cache line by MVA
 * Argument:    MVA(Modified virtual address)
 * Return:      None
**/
void l1_dcache_invalidate_line(uint32_t mva);

/**
 * Function:    l1_dcache_clean_line
 * Description: Clean L1 D cache line by MVA
 * Argument:    MVA(Modified virtual address)
 * Return:      None
**/
void l1_dcache_clean_line(uint32_t mva);

/**
 * Function:    l1_dcache_clean_invalidate_line
 * Description: Flush(clean and invalidate) L1 D cache line by MVA
 * Argument:    MVA(Modified virtual address)
 * Return:      None
**/
void l1_dcache_clean_invalidate_line(uint32_t mva);

/**
 * Function:    l1_icache_invalidate_range
 * Description: Invalidate L1 I cache line range by MVA to point of unification
 * Argument:    start_mva: start address, byteNum: range length
 * Return:      None
**/
void l1_icache_invalidate_range(uint32_t start_mva, uint32_t byteNum);

/**
 * Function:    l1_dcache_clean_range
 * Description: Clean L1 D cache line range by MVA to point of coherency
 * Argument:    start_mva: start address, byteNum: range length
 * Return:      None
**/
void l1_dcache_clean_range(uint32_t start_mva, uint32_t byteNum);

/**
 * Function:    l1_dcache_invalidate_range
 * Description: Invalidate L1 D cache line range by MVA to point of coherency
 * Argument:    start_mva: start address, byteNum: range length
 * Return:      None
**/
void l1_dcache_invalidate_range(uint32_t start_mva, uint32_t byteNum);

/**
 * Function:    l1_dcache_clean_invalidate_range
 * Description: Flush (clean and invalidate) L1 D cache line range by MVA to point of coherency
 * Argument:    start_mva: start address, byteNum: range length
 * Return:      None
**/
void l1_dcache_clean_invalidate_range(uint32_t start_mva, uint32_t byteNum);

/**
 * Function:    l1_dcache_clean_set_way
 * Description: Clean L1 D cache line by Set/Way
 *              This has an effect only on the processor that performs the operation.
 * Argument:    set_way: set index
 * Return:      None
**/
void l1_dcache_clean_set_way(uint32_t set_way);

/**
 * Function:    l1_dcache_clean_set_way
 * Description: Invalidate L1 D cache line by Set/Way
 *              This has an effect only on the processor that performs the operation.
 * Argument:    set_way: set index
 * Return:      None
**/
void l1_dcache_invalidate_set_way(uint32_t set_way);

/**
 * Function:    l1_dcache_clean_set_way
 * Description: Flush (clean and invalidate) L1 D cache line by Set/Way
 *              This has an effect only on the processor that performs the operation.
 * Argument:    set_way: set index
 * Return:      None
**/
void l1_dcache_clean_invalidate_set_way(uint32_t set_way);

/**
 * Function:    dcache_clean_invalidate_all
 * Description: Flush(clean and invalidate) entire L1 D cache and L2 cache
 * Argument:    set_way: set index
 * Return:      None
**/
extern void dcache_clean_invalidate_all(void);

/**
 * Function:    dcache_clean_all
 * Description: Clean entire L1 D cache and L2 cache
 * Argument:    set_way: set index
 * Return:      None
**/
extern void dcache_clean_all(void);

/**
 * Function:    dcache_invalidate_all
 * Description: Invalidate entire L1 D cache and L2 cache
 * Argument:    set_way: set index
 * Return:      None
**/
extern void dcache_invalidate_all(void);

/**
 * Function:    dcache_clean_invalidate_range
 * Description: Flush (clean and invalidate) L1 D cache & L2 cache line range by MVA
 * Argument:    start_mva: start address, byteNum: range length
 * Return:      None
**/
#define dcache_clean_invalidate_range(start_mva, byteNum)   \
        do{            \
            l1_dcache_clean_range(start_mva, byteNum); \
            l2_cache_clean_invalidate_range(start_mva, byteNum); \
            l1_dcache_clean_invalidate_range(start_mva, byteNum); \
         }while(0)

/**
 * Function:    dcache_clean_range
 * Description: Clean L1 D cache & L2 cache line range by MVA
 * Argument:    start_mva: start address, byteNum: range length
 * Return:      None
**/
#define dcache_clean_range(start_mva, byteNum)   \
        do{            \
            l1_dcache_clean_range(start_mva, byteNum); \
            l2_cache_clean_range(start_mva, byteNum); \
         }while(0)

/**
 * Function:    dcache_invalidate_range
 * Description: Invalidate L1 D cache & L2 cache line range by MVA
 * Argument:    start_mva: start address, byteNum: range length
 * Return:      None
**/
#define dcache_invalidate_range(start_mva, byteNum)   \
        do{            \
            l2_cache_invalidate_range(start_mva, byteNum); \
            l1_dcache_invalidate_range(start_mva, byteNum); \
         }while(0)

/**
 * Macro:       flush_cache_all
 * Description: Flush (clean & invalidate) l1 caches (D&I cache)
 *              This has an effect only on the processor that performs the operation.
 * Argument:    None
 * Return:      None
**/
#define flush_cache_all()                  \
                     do{                    \
	                     l1_dcache_clean_invalidate_all(); \
	                     l1_icache_invalidate_all(); \
	                   }while(0)

/**
 * Macro:       close_cache_all
 * Description: Close l1 caches (D&I cache)
 *              This has an effect only on the processor that performs the operation.
 * Argument:    None
 * Return:      None
**/
#define close_cache_all()                  \
                    do{     \
	                     l1_dcache_disable();   \
	                     l1_icache_disable();   \
	                  }while(0)

/**
 * Function:    l1_icache_enable
 * Description: Enable L1 D Cache
 * Argument:    None
 * Return:      None
**/
extern void l1_icache_enable(void);

/**
 * Function:    l1_dcache_enable
 * Description: Enable L1 I Cache
 * Argument:    None
 * Return:      None
**/
extern void l1_dcache_enable(void);

/**
 * Function:    enable_caches
 * Description: Enable L1 I & D cache
 * Argument:    None
 * Return:      None
**/
extern void enable_caches(void);


/**
 * Function:    l1_dcache_disable
 * Description: Disable L1 D cache
 * Argument:    None
 * Return:      None
**/
extern void l1_dcache_disable(void);

/**
 * Function:    l1_icache_disable
 * Description: Disable L1 I cache
 * Argument:    None
 * Return:      None
**/
extern void l1_icache_disable(void);

#define dcache_clean_line(mva)  \
   do{   \
      l1_dcache_clean_line(mva);   \
      l2_cache_clean_line(mva);   \
   }while(0)

#define dcache_invalidate_line(mva)  \
   do{   \
      l2_cache_invalidate_line(mva);   \
      l1_dcache_invalidate_line(mva);   \
   }while(0)

#define dcache_clean_invalidate_line(mva)  \
   do{   \
      l1_dcache_clean_line(mva);   \
      l2_cache_clean_invalidate_line(mva);   \
      l1_dcache_clean_invalidate_line(mva);   \
   }while(0)
#else
#define l1_dcache_clean_all()                          do{}while(0)                       
#define l1_dcache_invalidate_all()                     do{}while(0)                       
#define l1_dcache_clean_invalidate_all()               do{}while(0)                       
#define l1_icache_invalidate_all()                     do{}while(0)                       
#define l1_icache_invalidate_all_is()                  do{}while(0)
#define dcache_clean_line(mva)                         do{(void)mva;}while(0)
#define dcache_clean_invalidate_line(mva)              do{(void)mva;}while(0) 
#define dcache_invalidate_line(mva)                    do{(void)mva;}while(0) 
#define l1_icache_invalidate_line(mva)                 do{(void)mva;}while(0)             
#define l1_dcache_invalidate_line(mva)                 do{(void)mva;}while(0)             
#define l1_dcache_clean_line(mva)                      do{(void)mva;}while(0)             
#define l1_dcache_clean_invalidate_line(mva)           do{(void)mva;}while(0)             
#define l1_icache_invalidate_range(mva, byteNum)       do{(void)(mva);(void)(byteNum);}while(0)
#define l1_dcache_clean_range(mva, byteNum)            do{(void)(mva);(void)(byteNum);}while(0)
#define l1_dcache_invalidate_range(mva, byteNum)       do{(void)(mva);(void)(byteNum);}while(0)
#define l1_dcache_clean_invalidate_range(mva, byteNum) do{(void)(mva);(void)(byteNum);}while(0)
#define l1_dcache_clean_set_way(set_way)               do{(void)set_way;}while(0)         
#define l1_dcache_invalidate_set_way(set_way)          do{(void)set_way;}while(0)         
#define l1_dcache_clean_invalidate_set_way(set_way)    do{(void)set_way;}while(0)         
#define dcache_clean_invalidate_all()                  do{}while(0)                       
#define dcache_clean_all()                             do{}while(0)                       
#define dcache_invalidate_all()                        do{}while(0)                       
#define dcache_clean_invalidate_range(mva, byteNum)    do{(void)(mva);(void)(byteNum);}while(0)
#define dcache_clean_range(mva, byteNum)               do{(void)(mva);(void)(byteNum);}while(0)
#define dcache_invalidate_range(mva, byteNum)          do{(void)(mva);(void)(byteNum);}while(0)
#define flush_cache_all()                              do{}while(0) 
#define close_cache_all()                              do{}while(0) 
#define l1_icache_enable()                             do{}while(0)
#define l1_dcache_enable()                             do{}while(0)
#define enable_caches()                                do{}while(0)
#define l1_dcache_disable()                            do{}while(0)
#define l1_icache_disable()                            do{}while(0)
#endif

#ifdef CONFIG_L2_CACHE

void l2c310_lock_clean(void);

/**
 * Function:    l2_cache_clean_all
 * Description: clean entire L2 cache by set/way
 * Argument:    None
 * Return:      None
**/
extern void l2_cache_clean_all(void);

/**
 * Function:    l2_cache_invalidate_all
 * Description: Invalidate entire L2 cache by set/way
 * Argument:    None
 * Return:      None
**/
extern void l2_cache_invalidate_all(void);

/**
 * Function:    l2_cache_clean_invalidate_all
 * Description: flush(clean and invalidate) entire L2 cache by set/way
 * Argument:    None
 * Return:      None
**/
extern void l2_cache_clean_invalidate_all(void);

/**
 * Function:    l2_cache_clean_line
 * Description: Clean L2 cache line by MVA, MVA will be transferred to PA inside
 * Argument:    mva : Modified virtual address
 * Return:      None
**/
void l2_cache_clean_line(uint32_t mva);


/**
 * Function:    l2_cache_invalidate_line
 * Description: Invalidate L2 cache line by MVA, MVA will be transferred to PA inside
 * Argument:    mva : Modified virtual address
 * Return:      None
**/
void l2_cache_invalidate_line(uint32_t mva);

/**
 * Function:    l2_cache_clean_invalidate_line
 * Description: Flush(clean and invalidate) L2 cache line by MVA, MVA will be transferred to PA inside
 * Argument:    mva : Modified virtual address
 * Return:      None
**/
void l2_cache_clean_invalidate_line(uint32_t mva);

/**
 * Function:    l2_cache_clean_range
 * Description: Clean L2 cache line range by MVA, MVA will be transferred to PA inside
 * Argument:    start_mva: start address, byteNum: range length
 * Return:      None
**/
void l2_cache_clean_range(uint32_t start_mva, uint32_t byteNum);

/**
 * Function:    l2_cache_invalidate_range
 * Description: Invalidate L2 cache line range by MVA, MVA will be transferred to PA inside
 * Argument:    start_mva: start address, byteNum: range length
 * Return:      None
**/
void l2_cache_invalidate_range(uint32_t start_mva, uint32_t byteNum);

/**
 * Function:    l2_cache_invalidate_range
 * Description: Flush(clean and invalidate) L2 cache line range by MVA, MVA will be transferred to PA inside
 * Argument:    start_mva: start address, byteNum: range length
 * Return:      None
**/
void l2_cache_clean_invalidate_range(uint32_t start_mva, uint32_t byteNum);
/**
 * Function:    l2_cache_enable
 * Description: Enable L2 cache
 * Argument:    None
 * Return:      None
**/
extern void l2_cache_enable(void);

/**
 * Function:    l2_cache_disable
 * Description: Disable L2 cache
 * Argument:    None
 * Return:      None
**/
extern void l2_cache_disable(void);

extern void l2_cache_prefetch_enable(void);
extern void l2_cache_prefetch_disable(void);
void l2_cache_standby_enable(void);
void l2_cache_standby_disable(void);
void l2_cache_sync(void);
void sl2c_dump_info(void);
void pxa988_l2_cache_init(void);
void SetL2TagDataLatencies(uint32_t value);
void l2_cache_init(uint32_t config);
#else
#define l2_cache_clean_all()                              do{}while(0)
#define l2_cache_invalidate_all()                         do{}while(0)
#define l2_cache_clean_invalidate_all()                   do{}while(0)
#define l2_cache_clean_line(mva)                          do{(void)mva;}while(0)
#define l2_cache_invalidate_line(mva)                     do{(void)mva;}while(0)
#define l2_cache_clean_invalidate_line(mva)               do{(void)mva;}while(0)
#define l2_cache_clean_range(mva, byteNum)                do{(void)(mva);(void)(byteNum);}while(0)
#define l2_cache_invalidate_range(mva, byteNum)           do{(void)(mva);(void)(byteNum);}while(0)
#define l2_cache_clean_invalidate_range(mva, byteNum)     do{(void)(mva);(void)(byteNum);}while(0)
#define l2_cache_enable()                                 do{}while(0)
#define l2_cache_disable()                                do{}while(0)
#define l2_cache_prefetch_enable()                        do{}while(0)
#define l2_cache_prefetch_disable()                       do{}while(0)
#define l2_cache_standby_enable()                         do{}while(0)
#define l2_cache_standby_disable()                        do{}while(0)
#define l2_cache_sync()                                   do{}while(0)
#define sl2c_dump_info()                                  do{}while(0)
#define pxa988_l2_cache_init()                            do{}while(0)
#define SetL2TagDataLatencies(value)                      do{(void)value;}while(0)
#define l2_cache_init(config)                             do{(void)config;}while(0)
#define l2c310_lock_clean()                               do{}while(0)
#endif

/**
 * Function:    is_i_cache_enabled
 * Description: Check if L1 I cache is enabled or not
 * Argument:    None
 * Return:      1: enabled; 0: not enabled
**/
extern int is_i_cache_enabled(void);

/**
 * Function:    is_d_cache_enabled
 * Description: Check if L1 D cache is enabled or not
 * Argument:    None
 * Return:      1: enabled; 0: not enabled
**/
extern int is_d_cache_enabled(void);


#define CP15_ICACHE_ENABLE            BIT_12
#define CP15_BPU_ENABLE               BIT_11
#define CP15_DCACHE_ENABLE            BIT_2
#define L1_ICACHE_LINE_SIZE           32
#define L1_DCACHE_LINE_SIZE           32
#define L2C_CACHE_LINE_SIZE           32

/* config macros for l2_cache_init */
#define L2_CACHE_DUMP_INFO BIT_0
#define L2_EVT_MON_ON      BIT_1
#define L2_PREFETCH_ON     BIT_2
#define L2_FORCE_WA_ON     BIT_3

int is_l2_cache_prefetch_enabled(void);
int is_l2_cache_evt_monitor_enabled(void);
int is_l2_cache_enabled(void);
uint32_t l2_cache_va_2_pa(uint32_t va);

/* L2 Cache performance counter related functions */
typedef enum{
   L2C_COUNTER0 = 0,
   L2C_COUNTER1,
   L2C_COUNTER_INVALID
}SL2C_EVENT_COUNTERS;

typedef enum
{
   SL2_event_counter_disabled = 0,    /* b0000 */
   SL2_c0_eviction_from_l2,           /* b0001 */
   SL2_M0_dataread_hit_dcache,        /* b0010 */
   SL2_M0_dataread_req_dcache,        /* b0011 */
   SL2_M0_datawrite_hit,              /* b0100 */
   SL2_M0_datawrite_req,              /* b0101 */
   SL2_M0_datawrite_req_writethrough, /* b0110 */
   SL2_M1_dataread_hit_icache,        /* b0111 */
   SL2_M1_dataread_req_icache,        /* b1000 */
   SL2_WA_alloc_causedby_write_miss,  /* b1001 */
   SL2_PF_prefetch_linefill_to_l2,    /* b1010 */
                      /* Below two are reserved
                        according to new spec */
   SL2_M2_dataread_hit_mmu,         
   SL2_M2_dataread_req_mmu,
   SL2_counter_disabled   
}SL2_EVENTS;

int l2c_evt_counter_cfg(uint8_t counter, uint32_t cfg_val);
int l2c_evt_counter_preload(uint8_t counter, uint32_t pre_num);
int l2c_evt_counter_read(uint8_t counter, uint32_t *count_num);
int l2c_evt_counters_enable_all(void);
int l2c_evt_counters_disable_all(void);
int l2c_evt_counter_reset(uint8_t counter);
int l2c_evt_counters_reset_all(void);
int l2c_evt_counter_release(uint8_t counter);

/**
 * Function:    bpu_invalidate_all
 * Description: Invalidate all branch predictors
 *              This has an effect only on the processor that performs the operation.
 * Argument:    None
 * Return:      None
**/
void bpu_invalidate_all(void);

/**
 * Function:    bpu_invalidate_all_is
 * Description: Invalidate all branch predictors Inner Shareable
 * Argument:    None
 * Return:      None
**/
void bpu_invalidate_all_is(void);

/**
 * Function:    bpu_enable
 * Description: Enable program flow prediction
 * Argument:    None
 * Return:      None
**/
extern void bpu_enable(void);

/**
 * Function:    bpu_disable
 * Description: Disable program flow prediction
 * Argument:    None
 * Return:      None
**/
extern void bpu_disable(void);
/**
 * Function:    is_bpu_enabled
 * Description: Check if program flow prediction is enabled or not
 * Argument:    None
 * Return:      1: enabled; 0: not enabled
**/
extern int is_bpu_enabled(void);
#endif
