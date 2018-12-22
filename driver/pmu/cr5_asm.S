#define mode_usr        0x10
#define mode_fiq        0x11
#define mode_irq        0x12
#define mode_svc        0x13
#define mode_abt        0x17
#define mode_und        0x1b
#define mode_sys        0x1f

@ca9_enter_c2(&pl310, &debug_context, sram_off)

        .global ca9_enter_c2
ca9_enter_c2:
        stmfd   sp!, {r4-r12,lr}
        mov     r8,  r0 @r6 contains save address of L2 cache controllers
        mov     r7,  r2 @r7 contains sram_off
        mov     r9,  r1 @r9 contains the debug context save addr

@r2 contains warm rest branch address
        ldr     r2, =cr5_return_addr

@ copy reset handler to SRAM
        adr     r3, reset_chunk
        ldrd    r0, r1, [r3]
        strd    r0, r1, [r2]

        adr     r0, ca9_c2_restore
        str     r0, [r2, #0x08] @overwritten c2_restore
        add     r0, r2, #0x10  @save two words space for the restore_data&CPUID,i.e. the end address of the save.

        mov     r5, r0
@r0 is where we start to save and let it be the input of restore function
        str     r0, [r2, #0x0c]  @overwritten the c2_data

@stm is default as stmia

@ Save CPU registers
/*
   1. We have no assumption on the enter mode of lpm.
   2. The first and last save sate including all the r4-r12,cpsr registers which are shared in most modes
     so that it can restored in first place at exit lpm.
     Those mode in the middle can just save mode specific registers.
*/
        stm     r0!, {r4-r12}   @we don't assumption current mode, just save the common mode registers
        mrs     r1, cpsr
        stm     r0!, {r1}       @save the CPSR which contains current mode as well.
        mov     r2,  r1 @back up the cpsr in r2
@below is the mode specific registers
        cps     #mode_abt
        mrs     r1, spsr
        stm     r0!,{r1,sp,lr}
        cps     #mode_und
        mrs     r1, spsr
        stm     r0!,{r1,sp,lr}
        cps     #mode_sys
        stm     r0!,{sp,lr}  @there is no sprsr_sys. the CPSR is already saved at the beginning
        cps     #mode_irq
        mrs     r1, spsr
        stm     r0!,{r1,sp,lr}
        cps     #mode_fiq
        mrs     r1, spsr
        stm     r0!,{r1,r8-r12,sp,lr}
        cps     #mode_svc
        mrs     r1,spsr
        stm     r0!,{r1,sp,lr}

        msr     cpsr_c, r2  @return to current mode when core enter ca9_enter_c2 function.

@ Save cp15

        mov     r1, #0
        mov     r6, #0x10
save_mpu:
        mcr     p15,0,r1,c6,c2,0        @ Memory Region Number Register
        mrc     p15,0,r2,c6,c1,0        @ Region Base Register
        mrc     p15,0,r3,c6,c1,2        @ Region Size and Enable Register
        mrc     p15,0,r4,c6,c1,4        @ Region Access Control Register
        stm     r0!, {r2-r4}
        add     r1, r1, #1
        cmp     r1, r6
        bne     save_mpu

        mrc     p15,0,r1,c15,c0,0       @ SACTLR Secondary Auxiliary Control Register
        mrc     p15,0,r2,c1,c0,1        @ ACTLR
        mrc     p15,0,r3,c1,c0,0        @ SCTLR
        mrc     p15,0,r4,c1,c0,2        @ CPACR
        stm     r0!, {r1-r4}

        mrc     p15,0,r1,c9,c1,0        @ BTCM Region Register
        mrc     p15,0,r2,c9,c1,1        @ ATCM Region Register
        mrc     p15,0,r3,c11,c0,0       @ Slave Port Control Register
        mrc     p15,0,r4,c13,c0,1       @ CONTEXTIDR
        stm     r0!, {r1-r4}

        mrc     p15,0,r1,c13,c0,2       @ User read/write Thread and Proc. ID Register
        mrc     p15,0,r2,c13,c0,3       @ Read User Read Only Thread and Proc. ID Register
        mrc     p15,0,r3,c13,c0,4       @ Read Privileged Only Thread and Proc. ID Register
        mrc     p15,0,r4,c15,c0,1       @ Normal AXI Peripheral Interface Region
        stm     r0!, {r1-r4}

        mrc     p15,0,r1,c15,c0,2       @ Virtual AXI Peripheral Interface Region
        mrc     p15,0,r2,c15,c0,3       @ AHB Peripheral Interface Region
        stm     r0!, {r1-r2}

/*
 FIXME: Cache maintaince should be done at place after which there is no Data (thus Dcache) read/write operation.
 So need to write in assembly which will not use stack(C funtion call will use stack thus Dcache).

 FIXME: data consitency is very important.  Disable L1 Dcache means the owner core should exit SMP coherency and
 don't operate shared memory any more (i.e. C2) after clean its caches. Otherwise, L1 Dcache can't be disabled with owner active.
 That will cause data inconsistent(core0-->memory, core1-->D cache).
 So, caches should be at same ON/OFF state within a SCU/SMP cluster, otherwise, SCU can't make data consistency.

 Note: If flush(clean+inva) L1 Dcache at core reset when cache is not enabled yet, wfi will not take effect.
 enable L1 Dcache without inva will also cause same issue. The only correct seq. is inva. then enable it.
 */

@ C2 entry
#if 0
        @cache maintaince and exit coherency


#ifdef CONFIG_CACHE
        .extern l1_dcache_clean_invalidate_all
        ands  r0, r7, #0x4  @bit_2, l1 retain or power down
        blne  l1_dcache_clean_invalidate_all
#endif


    /*disable data cache by SCTLR.C bit*/
        mrc p15, 0, r1, c1, c0, 0
        bic r1, r1, #0x4
        mcr p15, 0, r1, c1, c0, 0
        clrex

   /*if the last core enter C2, clean and inva L2 or
    related part to be used at reset before L2 reenabled*/
 @r7 contains sram_off
        .extern set_l2_per_cpu
        mov     r0, r7
        bl  set_l2_per_cpu
#endif
        .extern set_scu_cpsr
        mov     r0, r7
        bl  set_scu_cpsr

        mov    r3,  #0x0 @clear r3 to init the ind of cx_fail
        isb
        dsb
        wfi
@ in case a wakeup is pending, CPU will fall through without being reset!
        mov     r0, r5 @ setup the saved register base and restore as usual
        mov     r3, #0xFF @ind of cx_fail

@ return here from reset chunk

@C2 exit should not use stack space before MMU and cache is enabled because C2 entry
@used stack at function call and the stack is in cache (L2) and it's hard to flush(clean+inva) the stack at C2 entry
@The code size is small at C2 exit, so we just make sure stack is not used before MMU and L2 is enabled.
@Otherwise, the other core may cause the L2 swap out the content to DDR at anytime so that data in stack corrupted.

ca9_c2_restore:
@when exit lpm, it should be SVC mode.  whatever, as long as it's not FIQ mode

        mrc p15, 0, r1, c1, c0, 0
        bic r1, r1, #1 << 13    @ Vint low
        mcr p15, 0, r1, c1, c0, 0


        ldm     r0!, {r4-r12}   @restore the common registers r4-r12.
        ldm     r0!, {r2}       @restore current mode as well. r2 contains the CPSR of current mode
        bic     r2, r2, #0x100  @enable imprecise data abort

@below restore the mode specific registers
        cps     #mode_abt
        ldm     r0!,{r1,sp,lr}
        msr     spsr, r1
        cps     #mode_und
        ldm     r0!,{r1,sp,lr}
        msr     spsr, r1
        cps     #mode_sys
        ldm     r0!,{sp,lr}  @there is no sprsr_sys. the CPSR is already saved at the beginning
        cps     #mode_irq
        ldm     r0!,{r1,sp,lr}
        msr     spsr, r1
        cps     #mode_fiq
        ldm     r0!,{r1,r8-r12,sp,lr}
        msr     spsr, r1
        cps     #mode_svc
        ldm     r0!,{r1,sp,lr}
        msr     spsr, r1
@restore mode
@we may actually do restore in reverse order if necessary.
        msr     cpsr_c, r2

        mov    r10,  r3 @ind of cx_fail, r3 can be used below

        mov     r5, r0
        mov     r0, r9
@        bl      restore_ca9_debug @corrupted r0~r4, no stack usage
        mov     r0, r5

        isb
@enable BPU and i cache at exit to speed up the process.
@SCTLR bit 11 for BPU, bit 12 for I cache. the whole SCTLR register will be restored later.
@        mrc   p15, 0, r1, c1, c0, 0
@        orr   r1, r1,#0x1800
@        mcr   p15, 0, r1, c1, c0, 0
@        isb

@ Restore cp15

        @retore MPU configuration before enable it
        mov     r1, #0
        mov     r6, #0x10
restore_mpu:
        ldm     r0!, {r2-r4}
        mcr     p15,0,r1,c6,c2,0        @ Memory Region Number Register
        mcr     p15,0,r2,c6,c1,0        @ Region Base Register
        mcr     p15,0,r3,c6,c1,2        @ Region Size and Enable Register
        mcr     p15,0,r4,c6,c1,4        @ Region Access Control Register
        add     r1, r1, #1
        cmp     r1, r6
        bne     restore_mpu

        ldm     r0!, {r1-r4}
        mcr     p15,0,r1,c15,c0,0       @ SACTLR Secondary Auxiliary Control Register
        mcr     p15,0,r2,c1,c0,1        @ ACTLR
        @mcr     p15,0,r3,c1,c0,0        @ SCTLR, MPU may be re-enabled here
        mov     r9, r3
        mcr     p15,0,r4,c1,c0,2        @ CPACR

        ldm     r0!, {r1-r4}
        mcr     p15,0,r1,c9,c1,0        @ BTCM Region Register
        mcr     p15,0,r2,c9,c1,1        @ ATCM Region Register
        mcr     p15,0,r3,c11,c0,0       @ Slave Port Control Register
        mcr     p15,0,r4,c13,c0,1       @ CONTEXTIDR

        ldm     r0!, {r1-r4}
        mcr     p15,0,r1,c13,c0,2       @ User read/write Thread and Proc. ID Register
        mcr     p15,0,r2,c13,c0,3       @ Read User Read Only Thread and Proc. ID Register
        mcr     p15,0,r3,c13,c0,4       @ Read Privileged Only Thread and Proc. ID Register
        mcr     p15,0,r4,c15,c0,1       @ Normal AXI Peripheral Interface Region

        ldm     r0!, {r1-r2}
        mcr     p15,0,r1,c15,c0,2       @ Virtual AXI Peripheral Interface Region
        mcr     p15,0,r2,c15,c0,3       @ AHB Peripheral Interface Region

        dsb     @ Ensure all previous loads/stores have completed
        isb


@Restore L2 controller
        .extern  restore_pl310
        mov     r0, r8
        mov     r1, r7
        bl      restore_pl310

        ldr     r6, return_addr         @ this addr is with MMU on
        isb
        dsb
   
        mov     r1, #0      
        mcr     p15, 0, r1, c7, c5, 6       @ BPIALL, Invalidate all branch predictors
        mcr     p15, 0, r1, c15, c5, 0      @ Invalidate D cache
        mcr     p15, 0, r1, c7, c5, 0       @ Invalidate I cache
        

@Restore SCTLR, I cache, D cache and MPU will be enbaled here
        mcr     p15,0,r9,c1,c0,0        @ SCTLR, MPU may be re-enabled here
        dsb
        isb

        .extern cx_fail
        cmp     r10, #0xFF
        bleq      cx_fail

        bx      r6
return_on_exit:
        ldmfd   sp!,{r4-r12,pc}
return_addr:
        .long   return_on_exit

@ reset handler for C2 exit
reset_chunk:
        ldr     r0, c2_data
        ldr     pc, c2_restore
c2_restore:
        .long   0
c2_data:
        .long   0

        .global save_performance_monitors
save_performance_monitors:
        PUSH    {r4, r8, r9, r10}

        @ Ignore:
        @        Count Enable Clear Register
        @        Software Increment Register
        @        Interrupt Enable Clear Register

        MRC     p15,0,r8,c9,c12,0       @ PMon: Control Register
        BIC     r1,r8,#1
        MCR     p15,0,r1,c9,c12,0       @ disable counter updates from here
        ISB                             @ 0b0 => PMCR<0>
        MRC     p15,0,r9,c9,c12,3       @ PMon: Overflow Flag Status Reg
        MRC     p15,0,r10,c9,c12,5      @ PMon: Event Counter Selection Reg
        STM     r0!, {r8-r10}
        UBFX    r9,r8,#11,#5            @ extract # of event counters, N
        TST     r9, r9
        BEQ     L1

L0:     SUBS    r9,r9,#1                @ decrement N
        MCR     p15,0,r9,c9,c12,5       @ PMon: select CounterN
        ISB
        MRC     p15,0,r3,c9,c13,1       @ PMon: save Event Type register
        MRC     p15,0,r4,c9,c13,2       @ PMon: save Event Counter register
        STM     r0!, {r3,r4}
        BNE     L0

L1:     MRC     p15,0,r1,c9,c13,0       @ PMon: Cycle Count Register
        MRC     p15,0,r2,c9,c14,0       @ PMon: User Enable Register
        MRC     p15,0,r3,c9,c14,1       @ PMon: Interrupt Enable Set Reg
        MRC     p15,0,r4,c9,c12,1       @ PMon: Count Enable Set Register
        STM     r0!, {r1-r4}

        POP     {r4, r8, r9, r10}
        bx      lr

        .global restore_performance_monitors
restore_performance_monitors:
        PUSH    {r4-r5, r8-r10, lr}
        @ NOTE: all counters disabled by PMCR<0> == 0 on reset

        @ Restore performance counters
        LDM     r0!,{r8-r10}    @ recover first block of PMon context
                                @ (PMCR, PMOVSR, PMSELR)
        MOV     r1, #0          @ generate register of all 0's
        MVN     r2, #0          @ generate register of all 1's
        MCR     p15,0,r2,c9,c14,2       @ disable all counter related interrupts
        MCR     p15,0,r2,c9,c12,3       @ clear all overflow flags
        ISB

        UBFX    r12,r8,#11,#5   @ extract # of event counters, N (0-31)
        TST     r12, r12
        BEQ     L20
        MOV     r3, r12         @ for N >0, generate a 2nd copy of N
        MOV     r4, #1
        LSL     r4, r4, r3
        SUB     r4, r4, #1      @ set bits<N-1:0> to all 1's

L00:    SUBS    r3,r3,#1            @ decrement N
        MCR     p15,0,r3,c9,c12,5   @ select Event CounterN
        ISB
        MRC     p15,0,r5,c9,c13,1   @ read Event Type register
        BFC     r5,#0,#8
        MCR     p15,0,r5,c9,c13,1   @ set Event Type to 0x0
        MCR     p15,0,r2,c9,c13,2   @ set Event Counter to all 1's
        ISB
        BNE     L00

        MOV     r3, #1
        BIC     r5, r9, #1<<31
        MCR     p15,0,r5,c9,c12,1       @ enable Event Counters
                                        @ (PMOVSR bits set)
        MCR     p15,0,r3,c9,c12,0       @ set the PMCR global enable bit
        ISB
        MCR     p15,0,r9,c9,c12,4   @ set event count overflow bits
        ISB
        MCR     p15,0,r4,c9,c12,2   @ disable Event Counters

        @ restore the event counters
L10:    SUBS    r12,r12,#1          @ decrement N
        MCR     p15,0,r12,c9,c12,5  @ select Event CounterN
        ISB
        LDM     r0!,{r3-r4}
        MCR     p15,0,r3,c9,c13,1   @ restore Event Type
        MCR     p15,0,r4,c9,c13,2   @ restore Event Counter
        ISB
        BNE     L10

L20:    TST     r9, #0x80000000         @ check for cycle count overflow flag
        BEQ     L40
        MCR     p15,0,r2,c9,c13,0       @ set Cycle Counter to all 1's
        ISB
        MOV     r3, #0x80000000
        MCR     p15,0,r3,c9,c12,1       @ enable the Cycle Counter
        ISB

L30:    MRC     p15,0,r4,c9,c12,3       @ check cycle count overflow now set
        MOVS    r4,r4                   @ test bit<31>
        BPL     L30
        MCR     p15,0,r3,c9,c12,2       @ disable the Cycle Counter

L40:    MCR     p15,0,r1,c9,c12,0       @ clear the PMCR global enable bit
        ISB

        @ restore the remaining PMon registers
        LDM     r0!,{r1-r4}
        MCR     p15,0,r1,c9,c13,0       @ restore Cycle Count Register
        MCR     p15,0,r2,c9,c14,0       @ restore User Enable Register
        MCR     p15,0,r3,c9,c14,1       @ restore Interrupt Enable Set Reg
        MCR     p15,0,r4,c9,c12,1       @ restore Count Enable Set Register
        MCR     p15,0,r10,c9,c12,5      @ restore Event Counter Selection
        ISB
        MCR     p15,0,r8,c9,c12,0       @ restore the PM Control Register
        ISB

        POP     {r4-r5, r8-r10, pc}

        @ Debug: see DDI0388F, 10.2.1
        .global save_ca9_debug
save_ca9_debug:
        push    {r1-r4}
        @mrc     p14, 0, r1, c0, c6, 0 @DBGWFAR
        mrc     p14, 0, r2, c0, c7, 0 @DBGVCR
        @mrc     p14, 0, r3, c0, c0, 2 @DBGDTRRXext
        mrc     p14, 0, r4, c0, c2, 2 @DBGDSCRext
        stm     r0!,{r1-r4}
        @mrc     p14, 0, r1, c0, c3, 2 @DBGDTRTXext
        mrc     p14, 0, r2, c0, c0, 4 @DBGBVR0
        mrc     p14, 0, r3, c0, c1, 4 @DBGBVR1
        mrc     p14, 0, r4, c0, c2, 4 @DBGBVR2
        stm     r0!,{r1-r4}
        mrc     p14, 0, r1, c0, c3, 4 @DBGBVR3
        mrc     p14, 0, r2, c0, c4, 4 @DBGBVR4
        mrc     p14, 0, r3, c0, c5, 4 @DBGBVR5
        mrc     p14, 0, r4, c0, c0, 5 @DBGBCRn
        stm     r0!,{r1-r4}
        mrc     p14, 0, r1, c0, c1, 5 @DBGBCRn
        mrc     p14, 0, r2, c0, c2, 5 @DBGBCRn
        mrc     p14, 0, r3, c0, c3, 5 @DBGBCRn
        mrc     p14, 0, r4, c0, c4, 5 @DBGBCRn
        stm     r0!,{r1-r4}
        mrc     p14, 0, r1, c0, c5, 5 @DBGBCRn
        mrc     p14, 0, r2, c0, c0, 6 @DBGWVRn
        mrc     p14, 0, r3, c0, c1, 6 @DBGWVRn
        mrc     p14, 0, r4, c0, c2, 6 @DBGWVRn
        stm     r0!,{r1-r4}
        mrc     p14, 0, r1, c0, c3, 6 @DBGWVRn
        mrc     p14, 0, r2, c0, c0, 7 @DBGWCRn
        mrc     p14, 0, r3, c0, c1, 7 @DBGWCRn
        mrc     p14, 0, r4, c0, c2, 7 @DBGWCRn
        stm     r0!,{r1-r4}
        mrc     p14, 0, r1, c0, c3, 7 @DBGWCRn
        mrc     p14, 0, r2, c7, c8, 6 @DBGCLAIMSET
        mrc     p14, 0, r3, c7, c9, 6 @DBGCLAIMCLR
        mrc     p14, 0, r4, c1, c4, 1 @DBGBXVRn
        stm     r0!,{r1-r4}
        mrc     p14, 0, r1, c1, c5, 1 @DBGBXVRn
        mrc     p14, 0, r2, c1, c4, 4 @DBGPRCR
        mrc     p14, 0, r3, c1, c3, 4 @DBGOSDLR
        stm     r0!,{r1-r4}
        pop     {r1-r4}
        bx      lr


/*This function is used at C2 exit and before MMU enable(all cache not workable).
We can't use stack which may be cached in L2 before C2 entry*/
restore_ca9_debug:
        ldm     r0!,{r1-r4}
        @mcr     p14, 0, r1, c0, c6, 0 @DBGWFAR
        mcr     p14, 0, r2, c0, c7, 0 @DBGVCR
        @mcr     p14, 0, r3, c0, c0, 2 @DBGDTRRXext
        mcr     p14, 0, r4, c0, c2, 2 @DBGDSCRext

        ldm     r0!,{r1-r4}
        @mcr     p14, 0, r1, c0, c3, 2 @DBGDTRTXext
        mcr     p14, 0, r2, c0, c0, 4 @DBGBVR0
        mcr     p14, 0, r3, c0, c1, 4 @DBGBVR1
        mcr     p14, 0, r4, c0, c2, 4 @DBGBVR2

        ldm     r0!,{r1-r4}
        mcr     p14, 0, r1, c0, c3, 4 @DBGBVR3
        mcr     p14, 0, r2, c0, c4, 4 @DBGBVR4
        mcr     p14, 0, r3, c0, c5, 4 @DBGBVR5
        mcr     p14, 0, r4, c0, c0, 5 @DBGBCRn

        ldm     r0!,{r1-r4}
        mcr     p14, 0, r1, c0, c1, 5 @DBGBCRn
        mcr     p14, 0, r2, c0, c2, 5 @DBGBCRn
        mcr     p14, 0, r3, c0, c3, 5 @DBGBCRn
        mcr     p14, 0, r4, c0, c4, 5 @DBGBCRn

        ldm     r0!,{r1-r4}
        mcr     p14, 0, r1, c0, c5, 5 @DBGBCRn
        mcr     p14, 0, r2, c0, c0, 6 @DBGWVRn
        mcr     p14, 0, r3, c0, c1, 6 @DBGWVRn
        mcr     p14, 0, r4, c0, c2, 6 @DBGWVRn

        ldm     r0!,{r1-r4}
        mcr     p14, 0, r1, c0, c3, 6 @DBGWVRn
        mcr     p14, 0, r2, c0, c0, 7 @DBGWCRn
        mcr     p14, 0, r3, c0, c1, 7 @DBGWCRn
        mcr     p14, 0, r4, c0, c2, 7 @DBGWCRn

        ldm     r0!,{r1-r4}
        mcr     p14, 0, r1, c0, c3, 7 @DBGWCRn
        mcr     p14, 0, r2, c7, c8, 6 @DBGCLAIMSET
        mcr     p14, 0, r3, c7, c9, 6 @DBGCLAIMCLR
        mcr     p14, 0, r4, c1, c4, 1 @DBGBXVRn

        ldm     r0!,{r1-r4}
        mcr     p14, 0, r1, c1, c5, 1 @DBGBXVRn
        mcr     p14, 0, r2, c1, c4, 4 @DBGPRCR
        mcr     p14, 0, r3, c1, c3, 4 @DBGOSDLR

        bx      lr

