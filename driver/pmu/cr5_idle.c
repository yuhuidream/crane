/*  cmd_ca9_idle.c: PXA988 C1 and C2 entry/exit load/save SW routines
 *
 *  Copyright (C) 2012 MARVELL Corporation (xlianghu@marvell.com)
 *
 */
#include "bbu.h"
#include "ppset.h"
#include "PMUA.h"
#include "PMUM.h"
#include "ICU.h"
#include "CIU.h"
#include "cache_ops.h"
#include "pmu_dec.h"

// Main function is ca9_enter_idle().

uint32_t cr5_return_addr[CPU_DATA_SIZE/sizeof(uint32_t)];

/*************************************NOTE!************************************************/
/*SCU: it seems nothing is necessary to backup in SCU registers.*/
/*TODO:Also need to save/restore the CPU local timers, global timer, SOC timer(A0 version) if used*/


/*-----------------------------------------------------------------*/

/*
             Cortex-A7   Cortex-A9    Cortex-A15
D$              PIPT        PIPT        PIPT
I$              VIPT        VIPT        PIPT
Line(words)     8           8           16
*/
/*
D cache always only take effect after MMU is enabled, while I cache can work with MMU off.
D cache need MMU to make the rules first.  I$ is read only, no matter.
*/
void set_l2_per_cpu(uint32_t sram_off)
{
#ifdef CONFIG_CACHE
   dcache_clean_range((uint32_t)cr5_return_addr, CPU_DATA_SIZE);
#ifdef CONFIG_L2_CACHE
   if (sram_off&BIT_3) //BIT_3 1:L2 SRAM power off, 0: L2 SRAM retain.
   {
      l2_cache_clean_invalidate_all();
   }
#endif
#endif
}

pl310_context pl310;

static void save_pl310(pl310_context* context, unsigned pl310_address)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;
    int i;

    /* TODO: are all these registers are present in earlier PL310 versions? */
    context->control = pl310->control;
    context->aux_control = pl310->aux_control;
    context->tag_ram_control = pl310->tag_ram_control;
    context->data_ram_control = pl310->data_ram_control;
    context->ev_counter_ctrl = pl310->ev_counter_ctrl;
    context->ev_counter1_cfg = pl310->ev_counter1_cfg;
    context->ev_counter0_cfg = pl310->ev_counter0_cfg;
    context->ev_counter1 = pl310->ev_counter1;
    context->ev_counter0 = pl310->ev_counter0;
    context->int_mask = pl310->int_mask;
    context->lock_line_en = pl310->lock_line_en;

    for (i=0; i<8; ++i)
    {
    /* Anton: original code copied 64-bit at once so LDM instruction
    was generated. This causes SLVERR and data abort per DDI0246F, 3.1.
    context->lockdown[i] = pl310->lockdown[i];*/
    context->lockdown[i].d = pl310->lockdown[i].d;
    context->lockdown[i].i = pl310->lockdown[i].i;
    }

#if (0)
    context->addr_filtering_start = pl310->addr_filtering_start;
    context->addr_filtering_end = pl310->addr_filtering_end;
#endif
    context->debug_ctrl = pl310->debug_ctrl;
    context->prefetch_ctrl = pl310->prefetch_ctrl;
    context->power_ctrl = pl310->power_ctrl;
    /* Sync the main memory on the saved context data as L2 won't be on
    during restore */
    dcache_clean_range((uint32_t)context, sizeof(pl310_context));
}

void restore_pl310(pl310_context* context, uint32_t sram_off)
{
    pl310_registers *pl310 = (pl310_registers *)L2310_ADDR;
    uint32_t i;
    uint32_t l2off;
    //while(1);

    l2off = (sram_off&BIT_1)? BIT_2:0; //1:L2 SRAM power off, 0: L2 SRAM retain.
    
    /* We may need to disable the PL310 if the boot code has turned it on */
    if (pl310->control)
    {
        /* Wait for the cache to be idle, then disable */
        pl310->cache_sync = 0;
        dsb();
        pl310->control = 0;
    }
    
    /* TODO: are all these registers present in earlier PL310 versions? */
    pl310->aux_control = context->aux_control;
    pl310->tag_ram_control = context->tag_ram_control;
    pl310->data_ram_control = context->data_ram_control;
    pl310->ev_counter_ctrl = context->ev_counter_ctrl;
    pl310->ev_counter1_cfg = context->ev_counter1_cfg;
    pl310->ev_counter0_cfg = context->ev_counter0_cfg;
    pl310->ev_counter1 = context->ev_counter1;
    pl310->ev_counter0 = context->ev_counter0;
    pl310->int_mask = context->int_mask;
    pl310->lock_line_en = context->lock_line_en;

    for (i=0; i<8; ++i)
    {
    /* Anton: original code copied 64-bit at once so LDM instruction
    was generated. This causes SLVERR and data abort per DDI0246F, 3.1.
    pl310->lockdown[i] = context->lockdown[i]; */
    pl310->lockdown[i].d = context->lockdown[i].d;
    pl310->lockdown[i].i = context->lockdown[i].i;
    }
#if (0)
    pl310->addr_filtering_start = context->addr_filtering_start;
    pl310->addr_filtering_end = context->addr_filtering_end;
#endif
    pl310->debug_ctrl = context->debug_ctrl;
    pl310->prefetch_ctrl = context->prefetch_ctrl;
    pl310->power_ctrl = context->power_ctrl;
    dsb();
    
    /*
     * If the RAMs were powered off, we need to invalidate the cache
     */
    if (l2off)
    {
        //inv_pl310(L2310_ADDR);
        l2_cache_invalidate_all();
    }
    
    pl310->control = context->control;
    dsb();
}

/*
wfi_mode = 0;  //0: wfe, 1: wfi-c1, 2: c13, 3: c2. BIT_4 to differ Cx and Mx
Note: Mx stands for state with all cores in C2x.
Other Mx can be get by combine Cx, dedicated routine is not necessary
sram_off = 0;  //BIT0: L1  BIT1: L2, BIT2: SCU
*/

void c2_save(uint32_t sram_off)
{

    unsigned pmu_context[PMU_DATA_SIZE/sizeof(unsigned)];
    uint32_t debug_context[DEBUG_DATA_SIZE/sizeof(uint32_t)];
    uint32_t reset_addr=0;

    /*Set address in the warm reset register which will be jumped to at wake up
      No matter the CIU CR5_CONF.VINITHI0 setting is low or high, bootrom (high)
      or OBM(low) will jump to
    */
    CIU->SW_BRANCH_ADDR = (uint32_t)cr5_return_addr;
    bbu_printf(MP3_PWR_LOG"CR5 core saving its context and entering C2...\r\n");
    //dcache_clean_range((uint32_t)&vfp_context, VFP_DATA_SIZE);
    //save_ca9_debug(&debug_context[0]);
    //debug will be restored before L2&L1 enabled at very start
    //dcache_clean_range((uint32_t)&debug_context[0], DEBUG_DATA_SIZE);
    //save_performance_monitors(&pmu_context[0]);
    //dcache_clean_range((uint32_t)&pmu_context[0], PMU_DATA_SIZE);
    save_pl310(&pl310, L2310_ADDR); 

    ca9_enter_c2((uint32_t *)&pl310, &debug_context[0], sram_off);
    //in AP silent reset, it will reset CP once exit C13/C2,  below prints will not output

    PMUM->CCGR = 0xFFFFFFFF;
    bbu_printf(MP3_PWR_LOG"CR5 core has restored from C2.\r\n");

#ifdef CONFIG_CP_ALONE
    if(CIU->SYS_BOOT_CNTRL&BIT_14) {
        bbu_printf(MP3_PWR_LOG"The PMU AP (Dragon+Seagull) has gone through a sleep and wake up cycle!\r\n");
        PMUA->RES_FRM_SLP_CLR = 0; //clear the SYS_BOOT_CNTRL[14] bit
    }
#endif

    CIU->SW_BRANCH_ADDR = 0; //default is not C2 exit
    //restore_performance_monitors(&pmu_context[0]);
    //bbu_printf("C2 exit: PMUM->APRR=0x%x, PMUCP->CPRR=0x%x\r\n", PMUM->APRR,   PMUCP->CPRR);
    //bbu_printf("C2 exit: PMUM->CPSR=0x%x, PMUCP->CPSR=0x%x\r\n", PMUM->CPSR,   PMUCP->CPSR);
    //bbu_printf("C2 exit: PMUCP->CSWCR=0x%x\r\n", PMUCP->CSWCR);
}

void cx_fail(void)
{
    bbu_printf("CR5 is not exiting from D2 but C2 without power off...\r\n");
}

    //According to DE(Haiping Lu), those can be set for all lpm modes.
    //1. Program PL310 power ctrl Register (reg15) to set standby_mode_en bit
    //and dynamic_clk_gating_en bit
    /* Enable L2 cache standby: this is for all lpm modes */
    //*(volatile unsigned *)0xD1DFBF80 = 3; //L2C PWRCTRL
    //2. Program SCU Control register to set SCU standby enable bit
    //*(volatile unsigned *)0xd1dfe000 |=BIT_5;

void set_scu_cpsr(uint32_t sram_off)
{
   uint32_t l1_off;
   uint32_t cpu;
   uint32_t v;

#ifdef CONFIG_CACHE

   //Note: if we want to use flush range, we need to check all shared variable/memory shared by cores
   l1_dcache_clean_all(); 
   l2_cache_clean_range((uint32_t)cr5_return_addr, CPU_DATA_SIZE);
   save_pl310(&pl310, L2310_ADDR);
   if (sram_off&BIT_1) //BIT_1 1:L2 SRAM power off, 0: L2 SRAM retain.
   {
      l2_cache_disable(); //it means clean+inva+disable
   }
   else{

      l2_cache_sync();
   }
   //here will disable L1 D cache and exit SMP
   l1_dcache_invalidate_all();
   __asm volatile(
  /*
   * Invalidate and Turn off L1 D cache
   */
   "mrc p15, 0, %0, c1, c0, 0\n"
   "bic %0, %0, %1\n"
   "mcr p15, 0, %0, c1, c0, 0\n"
     : "=&r" (v)
     : "Ir" (0x4)
     : "cc", "memory");
#endif
}
