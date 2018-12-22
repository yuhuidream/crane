#ifndef h_mrvl_amp_h
#define h_mrvl_amp_h

#include "mem_map.h"

#define CR5_BRANCH_ADDR          (CR5_AREA_ST)

#define PMUM_APRR                (PMUM_BASE+0x1020)
#define PMUM_APRR_CPR            BIT_0


#define ATCM_BASE_ADDRESS        (0xB0010000)
#define ATCM_LEN                 (0x8000)

#define BTCM_BASE_ADDRESS        (0xB0020000)
#define BTCM_LEN                 (0x8000)

typedef enum Cr5BootTypeTag{
   FROM_HIGH_VECTOR,
   FROM_LOW_DDR,
   FROM_LOW_TCM,
}Cr5BootType;

void boot_up_seagull(Cr5BootType type, uint32_t address, int check);
void seagull_say_hello(void);

#endif
