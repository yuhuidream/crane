@; Copyright ARM Ltd 1999. All rights reserved.

@ --- Software configuration

@       .equ            VECTOR_TBL_BASEADDRESS_SE,0x0
@       .equ            VECTOR_TBL_BASEADDRESS_NS,0x300000
        .equ            FIQ_Stack_Size, 0x400
        .equ            IRQ_Stack_Size, 0x1000
        .equ            SVC_Stack_Size, 0x400
        .equ            USR_Stack_Size, 0x200
        .equ            ABORT_Stack_Size, 0x400
        .equ            SYS_Stack_Size, 0x5000
        .equ            UND_Stack_Size, 0x400

/** macro to configurate stacks of different modes of each core
 *  reg0: commonly used to store the stack address
 *  reg1: keep the core smp hard ID
 **/
      .macro config_all_stacks  reg0, reg1

        MSR     CPSR_c, #ABT_MODE | NO_INT
        LDR     \reg0, =__stack_end__

        mov     SP, \reg0
@ Enter UND mode and set up the UND stack pointer
        MSR     CPSR_c, #UND_MODE | NO_INT
        sub     \reg0, \reg0, #ABORT_Stack_Size
        mov     SP, \reg0

@ Enter FIQ mode and set up the FIQ stack pointer
        MSR     CPSR_c, #FIQ_MODE | NO_INT
        sub     \reg0, \reg0, #UND_Stack_Size
        mov     SP, \reg0
@ Enter IRQ mode and set up the IRQ stack pointer
        MSR     CPSR_c, #IRQ_MODE | NO_INT
        sub     \reg0, \reg0, #FIQ_Stack_Size
        mov     SP, \reg0
@ Enter SVC mode and set up the SVC stack pointer
        MSR     CPSR_c, #SVC_MODE | NO_INT
        sub     \reg0, \reg0, #IRQ_Stack_Size
        mov     SP, \reg0
@ Enter SYS mode and set up the SYS stack pointer
        MSR     CPSR_c, #SYS_MODE | NO_INT
        sub     \reg0, \reg0, #SVC_Stack_Size
        mov     SP, \reg0

@ Enter USR mode and set up the SVC stack pointer
@       MSR     CPSR_c,,, #USR_MODE | NO_INT
@       ldr \reg0, =__stack_start__
@       add  \reg0, \reg0, #USR_Stack_Size
@       mov SP, \reg0
      .endm

@ --- Standard definitions of mode bits and interrupt (I & F) flags in PSRs

        .equ            USR_MODE,0x10
        .equ            FIQ_MODE,0x11
        .equ            IRQ_MODE,0x12
        .equ            SVC_MODE,0x13
        .equ            ABT_MODE,0x17
        .equ            UND_MODE,0x1B
        .equ            SYS_MODE,0x1F
#ifdef  CONFIG_QK
        .equ            FIQ_PRIO,0xFF
        .equ            IRQ_PRIO,(0xFF - 1)
#endif
        .equ            NO_IRQ, 0x80 @ when I bit is set, IRQ is disabled
        .equ            NO_FIQ, 0x40 @ when F bit is set, FIQ is disabled
        .equ            NO_INT, 0xC0   /* mask to disable interrupts (FIQ and IRQ) */

        @Use CIU SW_SCRATCH_REG to store software reset flag for now
        .equ            SW_RESET_FLAG, 0xD4282CE8 @SW_SCRATCH_REG

/* constant to pre-fill the stack */
        .equ            STACK_FILL,     0xAAAAAAAA

		  @ Size of struct pt_regs, store cpu related register
		  .equ 				S_FRAME_SIZE, (17*4)

    .text
    .code 32
    .global _entry
    .func   _entry

_entry:

     b       reset
     LDR     pc,vectorUndefined
     LDR     pc,vectorSWI
     LDR     pc,vectorPrefetchAbort
     LDR     pc,vectorDataAbort
     LDR     pc,vectorReserved
     LDR     pc,vectorIRQ
     LDR     pc,vectorFIQ

@ ---------- Jump table ------------------------------
vectorUndefined:     .word     coreUndefinedHandler
vectorSWI:           .word     coreSWIHandler
vectorPrefetchAbort: .word     corePrefetchAbortHandler
vectorDataAbort:     .word     coreDataAbortHandler
vectorReserved:      .word     coreReservedHandler
vectorIRQ:           .word     coreIRQHandler
vectorFIQ:           .word     coreFIQHandler
_end_vector_table:

    .size   _entry, . - _entry
    .endfunc

        /* The copyright notice embedded prominently at the begining */
    .string "Copyright (c) Marvell. All Rights Reserved."
    .align 4
    @.balignl 16,0xdeadbeef       /* re-align to the word boundary */


@These are defined in the board-specific linker script.

    .globl _code_entry
    _code_entry: .word __code_entry

    .globl _atcm_code_end
    _atcm_code_end: .word __atcm_code_end

    .globl _bss_start
    _bss_start: .word __bss_start

    .globl _bss_end
    _bss_end:   .word _end

    .globl __stack_end__
    __stack_end__: .word __stack_end__

    .globl __stack_start__
    __stack_start__: .word __stack_start__

    .globl ncnb_buffer_start
    ncnb_buffer_start: .word __ncnbBuffer_start__

    .globl ncnb_buffer_end
    ncnb_buffer_end:   .word __ncnbBuffer_end__

    .globl ao_stack_start
    ao_stack_start: .word __ao_stack_start__

    .globl ao_stack_end
    ao_stack_end:   .word __ao_stack_end__

    .globl kern_image_size
    kern_image_size:   .word (__kern_size__ - CR5_AREA_ST)

coreUndefinedHandler:
     B    QF_undef                      @ assert

coreSWIHandler:
     B    QF_swi                        @ assert

corePrefetchAbortHandler:
     B    QF_pAbort                     @ assert

coreDataAbortHandler:
     B    QF_dAbort                     @ assert

coreReservedHandler:
     B    QF_reserved                   @ assert

coreIRQHandler:
#ifdef  CONFIG_QK
     B    QK_irq                        @ branch IRQ handler
#else
     B    QF_irq                        @ branch IRQ handler
#endif

coreFIQHandler:
#ifdef  CONFIG_QK
     B    QK_fiq                        @ branch FIQ handler
#else
     B    QF_fiq                        @ branch FIQ handler
#endif


@/////////////////////////////////////////////////////////////////////////////
        .align 4
reset:
        @Enable TCM
        ldr     r1, =0xd4282ca0
        ldr     r0, [r1]
        orr     r0, r0, #(1<<11)
        orr     r0, r0, #(1<<10)
        orr     r0, r0, #(1<<9)
        str     r0, [r1]
        dsb

        @copy TCM code area to TCM after it is enabled
        mov     r1, #CR5_AREA_ST
        mov     r0, #0
        ldr     r2, _atcm_code_end
atcm_code_copy:
        ldr     r3, [r1], #4
        str     r3, [r0], #4
        cmp     r0, r2
        blt     atcm_code_copy

        @ relocate vector
        @ r1 : src; r2 : end; r3 : dest; r0 : data value
        ldr     r1, =_entry
        ldr     r2, =_end_vector_table
        mov     r3, #0
        cmp     r3, r1
        beq     relocate_done
relocate:
        ldr     r0, [r1], #4            @ read
        str     r0, [r3], #4            @ write
        cmp     r1, r2                  @ done?
        blt     relocate                       @ br if not
relocate_done:

#if 0
@ //CSWCR(0xd02500AC), CR5 is exiting D2 or not (in NZ3 design, CR5 and MSA are power down/up at the same time)
        ldr     r0, =_entry
        ldr     r1, =_remap   
        sub   r0,  r1, r0
        ldr     r1, _code_entry
        add    r0,  r0, r1
        mov   pc, r0

_remap:

        ldr     r0, =0xd02500ac
        ldr     r1, [r0]
        ands   r2, r1, #(1<<28)
        orrne  r1, r1, #(1<<29)
        strne  r1, [r0]
        ldrne  pc, =cr5_return_addr
#endif

@==================================================================
@ Disable MPU and caches
@==================================================================

@ Disable MPU and cache in case it was left enabled from an earlier run
@ This does not need to be done from a cold reset

        MRC     p15, 0, r0, c1, c0, 0       @ Read CP15 Control Register
        @MPU should be configured to run in SQU, should not disable MPU here
#ifndef CONFIG_SQU
        BIC     r0, r0, #0x1                @Disable MPU (M bit)
#endif
        BIC     r0, r0, #0x4                @ Disable data cache (C bit)
        BIC     r0, r0, #0x1000             @ Disable instruction cache (I bit)
        BIC     r0, r0, #0x2000             @ ormal exception vectors selected, address range = 0x00000000-0x0000001C
        DSB                                 @ Ensure all previous loads/stores have completed
        MCR     p15, 0, r0, c1, c0, 0       @ Write CP15 Control Register
        ISB                                 @ Ensure subsequent insts execute wrt new MPU settings

@==================================================================
@ Disable Branch prediction
@==================================================================

@ In the Cortex-R5, the Z-bit of the SCTLR does not control the program flow prediction.
@ Some control bits in the ACTLR control the program flow and prefetch features instead.
@ These are enabled by default, but are shown here for completeness.

        MRC     p15, 0, r0, c1, c0, 1       @ Read ACTLR
        ORR     r0, r0, #(0x1 << 17)        @ Enable RSDIS bit 17 to disable the return stack
        ORR     r0, r0, #(0x1 << 16)        @ Clear BP bit 15 and set BP bit 16:
        BIC     r0, r0, #(0x1 << 15)        @ Branch always not taken and history table updates disabled
        MCR     p15, 0, r0, c1, c0, 1       @ Write ACTLR

/* --- Initialise stack pointer registers----------
 In the QP based BBU, assuem it's always running in SYSTEM mode and only
 the SYSYTEM mode stack is used except we do special test for other modes.
*/
        config_all_stacks  r0, r1

@==================================================================
@ Enable Branch prediction
@==================================================================

@ In the Cortex-R5, the Z-bit of the SCTLR does not control the program flow prediction.
@ Some control bits in the ACTLR control the program flow and prefetch features instead.
@ These are enabled by default, but are shown here for completeness.

        MRC     p15, 0, r0, c1, c0, 1       @ Read ACTLR
        BIC     r0, r0, #(0x1 << 17)        @ Clear RSDIS bit 17 to enable return stack
        BIC     r0, r0, #(0x1 << 16)        @ Clear BP bit 15 and BP bit 16:
        BIC     r0, r0, #(0x1 << 15)        @ Normal operation, BP is taken from the global history table.
        MCR     p15, 0, r0, c1, c0, 1       @ Write ACTLR

		 .extern nezha2_set_mpu
        @  Setup and Enable MPU
#ifndef CONFIG_SQU
        @MPU is already enabled if CR5 runs in SQU
        BL   nezha2_set_mpu
#endif

clear_bss:
        ldr     r0, _bss_start          /* find start of bss segment        */
        ldr     r1, _bss_end            /* stop here                        */
        mov     r2, #0                  /* clear                            */

clbss_l:
        str     r2, [r0]                /* clear loop...                    */
        add     r0, r0, #4
        cmp     r0, r1
        ble     clbss_l


    /* Fill the .stack section */
        LDR     r1,=__stack_start__
        LDR     r2,=__stack_end__
        LDR     r3,=STACK_FILL
1:
        CMP     r1,r2
        STMLTIA r1!,{r3}
        BLT     1b

        bl      enable_irq_fiq

    /* Enter the C/C++ code */
        .extern main
		@
		@ In order to enable exception back trace to main funtion, add following code
		@ to init fp according ATPCS with frame pointer.
		@
        mov		ip, sp
        push	{fp, ip, lr, pc}
        sub		fp, ip, #4

        LDR     r12,=main
        MOV     lr,pc           /* set the return address */
JUMP_TO_MAIN:
        BX      r12             /* the target code can be ARM or THUMB */

        SWI     0xFFFFFF        /* cause exception if main() ever returns */


@/////////////////////////////////////////////////////////////////////////////

@ IRQ service

#ifdef  CONFIG_QK
/*****************************************************************************
* void QK_irq(void);
*/
    .global QK_irq
    .func   QK_irq
    .align  3
QK_irq:
#else
/*****************************************************************************
* void QF_irq(void);
*/
    .global QF_irq
    .func   QF_irq
    .align  3
QF_irq:
#endif
/* IRQ entry {{{ */
    @.extern do_irq
    .extern icu_irq_isr
    @.extern soc_icu_isr

    /* at this point, the CPU is in IRQ mode. r13 (sp) and r14 (lr) in
        this mode have replaced the user mode r13 and r14 and will be
        swapped out again when we enter system mode. We are going to
        save r0 and r1 into the IRQ mode r13 and r14 registers, so
        that we can restore them later. We are going to trash r13 (the
        irq stack pointer) in order to free up some other registers.
        Why not just push r0 and r1 onto the IRQ stack and pass the
        IRQ stack address to system mode through r0? The method used
        here is faster because it avoids memory references. */

    MOV     r13,r0              /* save r0 in r13_IRQ */
    SUB     r0,lr,#4            /* put return address in r0_SYS */
    MOV     lr,r1               /* save r1 in r14_IRQ (lr) */
    MRS     r1,spsr             /* put the SPSR in r1_SYS */

    /* we are about to switch to system mode.  This will swap out the
        r13_irq and r14_irq registers and replace them with the user mode
        r13 and r14 (sp and lr) registers (user mode and system mode use
        the same r13 and r14 ) */

    MSR     cpsr_c,#(SYS_MODE | NO_IRQ) /* SYSTEM, no IRQ, but FIQ enabled! */
    STMFD   sp!,{r0,r1}         /* save SPSR and PC on SYS stack */
    STMFD   sp!,{r2-r3,r12,lr}  /* save APCS-clobbered regs on SYS stack */
        /* the sys/usr stack now contains the user program context:
                spsr
                return address (pc)
                r14 (link register)
                r12
                r3
                r2 <- top of stack
        */

/* now we give the IRQ mode a pointer to the system/user mode stack, and
        adjust the system/user mode stack for what IRQ mode is going to put
        there */
    MOV     r0,sp               /* make the sp_SYS visible to IRQ mode */
    SUB     sp,sp,#(2*4)        /* make room for stacking (r0_SYS, r1_SYS) */

/* now we can switch back to IRQ mode and save the link register and
        stack pointer onto the system/user mode stack */

    MSR     cpsr_c,#(IRQ_MODE | NO_IRQ) /* IRQ mode, IRQ disabled */
    STMFD   r0!,{r13,r14}       /* finish saving the context (r0_SYS,r1_SYS)*/
#ifdef  CONFIG_QK
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
/* IRQ entry }}} */

    LDR     r0,=QK_intNest_     /* load address in already saved r0 */
#ifdef CONFIG_SMP
    get_smp_hard_id r12
    ADD     r0, r0, r12
#endif
    LDRB    r12,[r0]            /* load original QK_intNest_ into saved r12 */
    ADD     r12,r12,#1          /* increment the nesting level */
    STRB    r12,[r0]            /* store the value in QK_intNest_ */

    MSR     cpsr_c,#(SYS_MODE | NO_IRQ) /* enable FIQ */
#else
        /* the SYS/USER stack now contains the user program context:
                spsr
                return address (pc) (r15)
                r14 (link register)
                r12
                r3
                r2
                r1
                r0 <- top of stack ( stack grows downward )
        */

/* now we switch back to system mode and call the C IRQ handler */

    MSR     cpsr_c,#(SYS_MODE | NO_IRQ) /* SYSTEM mode, IRQ disabled */
/* IRQ entry }}} */
#endif

    /* NOTE: BSP_irq might re-enable IRQ interrupts (the FIQ is enabled
    * already), if IRQs are prioritized by the interrupt controller.
    */
   @ LDR     r12,=do_irq
    LDR      r12,=icu_irq_isr
   @ LDR      r12,=soc_icu_isr
    MOV     lr,pc               /* copy the return address to link register */
    BX      r12                 /* call the C IRQ-handler (ARM/THUMB) */

#ifdef  CONFIG_QK
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */
    LDR     r0,=QK_intNest_     /* load address */
#ifdef CONFIG_SMP
    get_smp_hard_id r12
    ADD     r0, r0, r12
#endif
    LDRB    r12,[r0]            /* load original QK_intNest_ into saved r12 */
    SUBS    r12,r12,#1          /* decrement the nesting level */
    STRB    r12,[r0]            /* store the value in QK_intNest_ */
    BNE     QK_irq_exit         /* branch if interrupt nesting not zero */

    LDR     r12,=QK_schedPrio_
    MOV     lr,pc               /* copy the return address to link register */
    BX      r12                 /* call QK_schedPrio_ (ARM/THUMB) */
    CMP     r0,#0               /* check the returned priority */
    BEQ     QK_irq_exit         /* branch if priority zero */

    LDR     r12,=QK_sched_
    MOV     lr,pc               /* copy the return address to link register */
    BX      r12                 /* call QK_sched_ (ARM/THUMB) */

QK_irq_exit:
/* IRQ exit {{{ */              /* IRQ/FIQ disabled--return from scheduler */

#else
/* IRQ exit {{{ */

/* we have now returned from the C irq handler */
/* make sure we are in system mode, with no interrupts, and prepare to
        change to IRQ mode. Put a pointer to the usr/sys stack into
        r0, so IRQ mode can access our stack, which should contain the
        following user context:*/
        /*
                spsr
                return address (pc)
                r14 (link register)
                r12
                r3
                r2
                r1
                r0
         */

    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */
#endif

    MOV     r0,sp               /* make sp_SYS visible to IRQ mode */
    ADD     sp,sp,#(8*4)        /* fake unstacking 8 registers from sp_SYS */

    MSR     cpsr_c,#(IRQ_MODE | NO_INT) /* IRQ mode, both IRQ/FIQ disabled */
    MOV     sp,r0               /* copy sp_SYS to sp_IRQ */
    LDR     r0,[sp,#(7*4)]      /* load the saved SPSR from the stack */
    MSR     spsr_cxsf,r0        /* copy it into spsr_IRQ */

    LDR     r0,[sp,#(5*4)]      /* load link register from the SYS stack */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */
    MOV     lr, r0              /* In case we use SVC mode instead of SYS mode */
                                 /* lr_svc is not lr_sys, ^ is for lr_sys/usr*/
    MSR     cpsr_c,#(IRQ_MODE | NO_INT) /* IRQ mode, both IRQ/FIQ disabled */

@   LDMFD   sp,{r0-r3,r12,lr}^  /* unstack all saved USER/SYSTEM registers */
    LDMFD   sp,{r0-r3,r12}   /* unstack all saved USER/SYSTEM registers */
                                /* note: lr in the prev instruction refers */
                                /* to the user/sys lr, even though we are  */
                                /* in IRQ mode (that's what the ^ means)  */
    @NOP                         /* can't access banked reg immediately *//*not access two banked register continuously now*/
    LDR     lr,[sp,#(6*4)]      /* load return address from the SYS stack */
    MOVS    pc,lr               /* return restoring CPSR from SPSR */
                                /* and swapping in the user mode sp and lr */
/* IRQ exit }}} */
#ifdef  CONFIG_QK
    .size   QK_irq, . - QK_irq
#else
    .size   QF_irq, . - QF_irq
#endif
    .endfunc


@ FIQ service
#ifdef  CONFIG_QK
/*****************************************************************************
* void QK_fiq(void);
*/
    .global QK_fiq
    .func   QK_fiq
    .align  3
QK_fiq:
#else
/*****************************************************************************
* void QF_fiq(void);
*/
    .global QF_fiq
    .func   QF_fiq
    .align  3
QF_fiq:
#endif
/* FIQ entry {{{ */
    @.extern do_fiq
    .extern AP_FIQ_isr
    MOV     r13,r0              /* save r0 in r13_FIQ */
    SUB     r0,lr,#4            /* put return address in r0_SYS */
    MOV     lr,r1               /* save r1 in r14_FIQ (lr) */
    MRS     r1,spsr             /* put the SPSR in r1_SYS */

    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
    STMFD   sp!,{r0,r1}         /* save SPSR and PC on SYS stack */
    STMFD   sp!,{r2-r3,r12,lr}  /* save APCS-clobbered regs on SYS stack */
    MOV     r0,sp               /* make the sp_SYS visible to FIQ mode */
    SUB     sp,sp,#(2*4)        /* make room for stacking (r0_SYS, SPSR) */

    MSR     cpsr_c,#(FIQ_MODE | NO_INT) /* FIQ mode, IRQ/FIQ disabled */
    STMFD   r0!,{r13,r14}       /* finish saving the context (r0_SYS,r1_SYS)*/

    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
/* FIQ entry }}} */

#ifdef  CONFIG_QK
    LDR     r0,=QK_intNest_     /* load address in already saved r0 */
    LDRB    r12,[r0]            /* load original QK_intNest_ into saved r12 */
    ADD     r12,r12,#1          /* increment interrupt nesting */
    STRB    r12,[r0]            /* store the value in QK_intNest_ */
#endif

    /* NOTE:
    * Because FIQ is typically NOT prioritized by the interrupt controller
    * BSP_fiq must not enable IRQ/FIQ to avoid priority inversions!
    */
    @LDR     r12,=do_fiq
    LDR     r12,=AP_FIQ_isr
    MOV     lr,pc               /* store the return address */
    BX      r12                 /* call the C FIQ-handler (ARM/THUMB)*/

#ifdef  CONFIG_QK
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */
    LDR     r0,=QK_intNest_     /* load address */
    LDRB    r12,[r0]            /* load original QK_intNest_ into saved r12 */
    SUBS    r12,r12,#1          /* decrement the nesting level */
    STRB    r12,[r0]            /* store the value in QK_intNest_ */
    BNE     QK_fiq_exit         /* branch if interrupt nesting not zero */

    LDR     r0,[sp,#(7*4)]      /* load the saved SPSR from the stack */
    AND     r0,r0,#0x1F         /* isolate the SPSR mode bits in r0 */
    CMP     r0,#IRQ_MODE        /* see if we interrupted IRQ mode */
    BEQ     QK_fiq_exit         /* branch if interrutped IRQ */

    /* We have interrupted a task. Call QK scheduler to handle preemptions */
    LDR     r12,=QK_schedPrio_
    MOV     lr,pc               /* copy the return address to link register */
    BX      r12                 /* call QK_schedPrio_ (ARM/THUMB) */
    CMP     r0,#0               /* check the returned priority */
    BEQ     QK_fiq_exit         /* branch if priority zero */

    LDR     r12,=QK_sched_
    MOV     lr,pc               /* copy the return address to link register */
    BX      r12                 /* call QK_sched_ (ARM/THUMB) */

QK_fiq_exit:
/* FIQ exit {{{ */              /* both IRQ/FIQ disabled (see NOTE above) */
#else
/* FIQ exit {{{ */              /* both IRQ/FIQ disabled (see NOTE above) */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */
#endif
    MOV     r0,sp               /* make sp_SYS visible to FIQ mode */
    ADD     sp,sp,#(8*4)        /* fake unstacking 8 registers from sp_SYS */

    MSR     cpsr_c,#(FIQ_MODE | NO_INT) /* FIQ mode, IRQ/FIQ disabled */
    MOV     sp,r0               /* copy sp_SYS to sp_FIQ */
    LDR     r0,[sp,#(7*4)]      /* load the saved SPSR from the stack */
    MSR     spsr_cxsf,r0        /* copy it into spsr_FIQ */

    LDR     r0,[sp,#(5*4)]      /* load link register from the SYS stack */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */
    MOV     lr, r0              /* In case we use SVC mode instead of SYS mode */
                                 /* lr_svc is not lr_sys, ^ is for lr_sys/usr*/
    MSR     cpsr_c,#(FIQ_MODE | NO_INT) /* FIQ mode, both IRQ/FIQ disabled */

   @LDMFD   sp,{r0-r3,r12,lr}^  /* unstack all saved USER/SYSTEM registers */
    LDMFD   sp,{r0-r3,r12}  /* unstack all saved USER/SYSTEM registers */
   @ NOP                         /* can't access banked reg immediately */ /*not access two banked register continuously now*/
    LDR     lr,[sp,#(6*4)]      /* load return address from the SYS stack */
    MOVS    pc,lr               /* return restoring CPSR from SPSR */
/* FIQ exit }}} */

#ifdef  CONFIG_QK
    .size   QK_fiq, . - QK_fiq
#else
    .size   QF_fiq, . - QF_fiq
#endif
    .endfunc

/*****************************************************************************/
	@
	@ macro to store exception point register information to sys mode stack in struct pt_regs
	@
	.macro sys_entry, excep_mode
	stmfd 	sp, {r0,lr}	@ store exception info to exception stack
	mrs 	r0, spsr
	str 	r0, [sp, #-12]
	sub 	r0, sp, #4			@ through r0 to access exception mode stack in sys mode
	msr   cpsr_c, #(SYS_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */

	sub 	sp, sp, #(S_FRAME_SIZE-4)
	tst 	sp, #4			@ test original stack alignment, sp need to be
								@ 8 bytes alignment according ATPCS
	subeq	sp, sp, #4

	stmia sp, {r1-r12}	@ first store r1 to r12
	ldr 	r2, [r0, #-4]
	add 	r1, sp, #(S_FRAME_SIZE-4)
	mov 	r5, r1
	addeq	r1, r1, #4
	str 	r2, [sp, #-4]!	@ restore r0
	ldr 	r3, [r0]			@ restore exception point address
	ldr 	r4, [r0, #-8]	@ restore spsr
	mov 	r2, lr			@ restore lr

	@
	@ We are now ready to fill in the remaining blanks on the stack:
	@
	@  r1 - sp_sys
	@  r2 - lr_sys
	@  r3 - lr_<exception>
	@  r4 - spsr_<exception>
	@
	stmfd r5, {r1-r4} 	@ save to stack of sys mode in struct pt_regs
	mov 	r0, sp
	mov 	r1, #\excep_mode

	.if \excep_mode == 3
	@
	@ Obtain information about current aborted instruction.
	@
	mrc	p15, 0, r2, c5, c0, 0		@ get FSR
	mrc	p15, 0, r3, c6, c0, 0		@ get FAR
	.elseif \excep_mode == 2
	@
	@ Obtain information about current prefetch abort.
	@
	mrc	p15, 0, r3, c6, c0, 2		@ get IFAR
	mrc	p15, 0, r2, c5, c0, 1		@ get IFSR
	.endif

	.endm

/*****************************************************************************/
	@
	@ Return back to exception point from exception handler.
	@ Usually "bbu_exception_trace" do not return to exception,
	@ there is a endless loop in it.
	@

	.macro sys_exit, excep_mode=0
	msr     cpsr_c, #(SYS_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */
	ldr	r0, [sp, #(13*4)]			@ top of the stack
	tst	r0, #4				@ orig stack 8-byte aligned?

	ldmfd	sp!, {r0-r12}
	ldr	lr,[sp, #8]
	add	sp, sp, #(4*4)
	addne	sp, sp, #4			@ not aligned, add 4 to sp to resume original sp

	.if ((\excep_mode == 2) || (\excep_mode == 3))
	msr	cpsr_c, #(ABT_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */
	.elseif (\excep_mode == 1)
	msr	cpsr_c, #(UND_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */
	.elseif (\excep_mode == 5)
	msr	cpsr_c, #(SVC_MODE | NO_INT) /* make sure IRQ/FIQ are disabled */
	.else
	b       .		@ abnormal mode, just halt machine
	.endif
	ldr	lr, [sp, #-4]
	ldr	r0, [sp, #-8]

	@
	@ Return back from the exception handle
	@
	.if \excep_mode == 3
	sub 	lr, lr, #8
	.elseif \excep_mode == 5	@ For SVC, return to address of the instruction after the SVC instruction
	sub 	lr, lr, #0
	.else
	sub 	lr, lr, #4
	.endif
	movs	pc,lr
	.endm

/*****************************************************************************
* void QF_reset(void);
*/
    .global QF_reset
    .func   QF_reset
    .align  3
QF_reset:
    LDR     r0,=Csting_reset
    B       QF_except
    .size   QF_reset, . - QF_reset
    .endfunc

/*****************************************************************************
* void QF_undef(void);
*/
    .global QF_undef
    .func   QF_undef
    .align  3
QF_undef:
	 sys_entry 1
	 BL       bbu_exception_trace
	 sys_exit 1
    .size   QF_undef, . - QF_undef
    .endfunc

/*****************************************************************************
* void QF_swi(void);
*/
    .global QF_swi
    .func   QF_swi
    .align  3
QF_swi:
	 sys_entry 5
	 BL       svc_call_handle
	 sys_exit 5
    .size   QF_swi, . - QF_swi
    .endfunc

/*****************************************************************************
* void QF_pAbort(void);
*/
    .global QF_pAbort
    .func   QF_pAbort
    .align  3
QF_pAbort:
	 sys_entry 2
	 BL       bbu_exception_trace
	 sys_exit 2
    .size   QF_pAbort, . - QF_pAbort
    .endfunc

/*****************************************************************************
* void QF_dAbort(void);
*/
    .global QF_dAbort
    .func   QF_dAbort
    .align  3
QF_dAbort:
	 sys_entry 3
	 BL       bbu_exception_trace
	 sys_exit 3
    .size   QF_dAbort, . - QF_dAbort
    .endfunc

/*****************************************************************************
* void QF_reserved(void);
*/
    .global QF_reserved
    .func   QF_reserved
    .align  3
QF_reserved:
	 sys_entry 4
	 BL       bbu_exception_trace
	 sys_exit 4
    .size   QF_reserved, . - QF_reserved
    .endfunc

/*****************************************************************************
* void QF_except(void);
*/
    .global QF_except
    .func   QF_except
    .align  3
QF_except:
    /* r0 is set to the string with the exception name */
    SUB     r1,lr,#4            /* set line number to the exception address */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
    LDR     r12,=Q_onAssert
    MOV     lr,pc               /* store the return address */
    BX      r12                 /* call the assertion-handler (ARM/THUMB) */
    /* the assertion handler should not return, but in case it does
    * hang up the machine in this endless loop
    */
    B       .

Csting_reset:       .string  "Reset"
Csting_undef:       .string  "Undefined"
Csting_swi:         .string  "Software Int"
Csting_pAbort:      .string  "Prefetch Abort"
Csting_dAbort:      .string  "Data Abort"
Csting_reserved:    .string  "Reserved"

    .size   QF_except, . - QF_except
    .endfunc

@///////////////////////with the assumption that BBU running in SYS mode//////////////

@ Enable IRQ and FIQ
        .global enable_irq_fiq
        .func   enable_irq_fiq
enable_irq_fiq:
        MSR     cpsr_c, #(SYS_MODE)
        BX  lr
        .endfunc

        .global disable_irq_fiq
        .func   disable_irq_fiq
disable_irq_fiq:
        MRS     r0,CPSR /*return the original CPSR*/
        MSR     cpsr_c,#(SYS_MODE | NO_INT) /* disable both IRQ/FIQ */
        BX  lr
        .endfunc

        .global restore_irq_fiq
        .func   restore_irq_fiq
restore_irq_fiq:
        msr CPSR_c, r0
        bx  lr
        .endfunc


@ Set exception vector low
        .global vint_low
        .func   vint_low
vint_low:
        STMFD r13!, {r0, r14}
        mrc p15, 0, r0, c1, c0, 0
        bic r0, r0, #1 << 13    @ Vint low
        mcr p15, 0, r0, c1, c0, 0
        LDMFD r13!, {r0, pc}
        .endfunc

@ Get CPUID
        .global GetCPUID
        .func GetCPUID
GetCPUID:
        STMFD r13!, {r1, r14}
        mrc p15, 0, r1, c0, c0, 5
        and  r0, r1, #0xf
        add r0, r0, #1
        LDMFD r13!, {r1, pc}
        .endfunc

        .global smp_processor_id
        .func smp_processor_id
smp_processor_id:
        mrc p15, 0, r0, c0, c0, 5
        and  r0, r0, #0xf
        BX     lr
        .endfunc

		@
		@get address where reset handle jump to main of app
		@
		.global get_brch_addr
		.func get_brch_addr
      .align  3
get_brch_addr:
		ldr	r0, =JUMP_TO_MAIN
		mov	pc, lr
		.size   get_brch_addr, . - get_brch_addr
		.endfunc

		@
		@ check stack is overflow or not
		@
		.global sys_stack_check
		.func sys_stack_check
		.align  3
sys_stack_check:
		STMFD	r13!, {r3, r4}
		ldr	r4, =__stack_start__
		cmp	r4, r1
		bhi	check_ilegal
		ldr	r3, =__stack_end__
		sub	r3, r3, #ABORT_Stack_Size
		sub	r3, r3, #FIQ_Stack_Size
		sub	r3, r3, #IRQ_Stack_Size
      sub	r3, r3, #SVC_Stack_Size
		cmp	r3, r0
		blo	check_ilegal
check_legal:
		LDMFD	r13!, {r3, r4}
		mov	r0, #0
		mov	pc, lr
check_ilegal:
		LDMFD	r13!, {r3, r4}
		mov	r0, #1
		mov	pc, lr
		.endfunc

		.align 4
		.global cpu_cycle_delay
		.func   cpu_cycle_delay
cpu_cycle_delay:
loop:  subs r0, r0, #1
		bne  loop
		bx   lr
        .end

		.align 4
		.global dead_loop
		.func dead_loop
dead_loop:
		b	.
		.endfunc
