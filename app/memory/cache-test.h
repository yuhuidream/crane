#ifndef _H_CACHE_TEST_H_
#define _H_CACHE_TEST_H_
#include "mem_map.h"
// List of attriburs for bbu_eica():
#define D_CACHE_ENABLE                          (BIT_0)
#define I_CACHE_ENABLE                          (BIT_1)
#define L2_CACHE_ENABLE                         (BIT_2)
#define L2_CACHE_PREFETCH_ENABLE                (BIT_3)
#define WRITE_ALLOCATE_ENABLE                   (BIT_4)
#define BPU_ENABLE                              (BIT_5)

#define ENABLED  1
#define DISABLED 0

// List of attriburs for bbu_cache_test():
#define L1_DCACHE_TEST                           (BIT_0)
#define L1_ICACHE_TEST                           (BIT_1)
#define L2_DCACHE_TEST                           (BIT_2)
#define L2_ICACHE_TEST                           (BIT_3)
#define CACHE_READ_LATENCY_TEST                  (BIT_4)
#define CACHE_WRITE_LATENCY_TEST                 (BIT_5)


void bbu_eica(uint8_t IsHexValDetec, uint16_t value);
void bbu_dica(void);
uint8_t bbu_cache_test(int argc, uint8_t **argv);
uint32_t bbu_sl2c_test(int argc, uint8_t **argv);

/****************************************************************************/
/* according to Emei Block diagram, L1 I$&D$ are both 32KB, L2 & is 512KB */
#define L1_CACHE_SIZE            (32  * 1024)
#define L2_CACHE_SIZE            (512 * 1024)
#define L1_DCACHE_TEST_ADDRESS    cache_test_buffer
#define L2_CACHE_TEST_ADDRESS     cache_test_buffer
/****************************************************************************/

typedef struct {
   float L1_DATA; 
   float L1_INST;
   float L2_DATA;
   float L2_INST;
   float DDR_DATA;
   float DDR_INST;
}ReadLatency;

#include "cache_ops.h"

#endif
