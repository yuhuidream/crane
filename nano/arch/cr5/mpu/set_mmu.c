#include "bbu.h"
#include "set_mmu.h"

void mmu_set_region_ncnb(uint32_t vAddr, uint32_t mBytes)
{
    (void)vAddr;
    (void)mBytes;
}

void mmu_set_region_cb(uint32_t vAddr, uint32_t mBytes)
{
    (void)vAddr;
    (void)mBytes;
}

int is_test_buffer_safe(uint32_t start)
{
    (void)start;
    return 1;
}

int is_gcu_buffer(uint32_t addr)
{
    (void)addr;
    return 0;
}

int mmu_get_mem_attribute(uint32_t address)
{
    return OI_WB_NO_WA;
}
