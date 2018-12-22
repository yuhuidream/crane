#ifndef H_SMP_CONFIG_H
#define H_SMP_CONFIG_H

#ifdef CONFIG_SMP
@ ---   Unless otherwise specified, CPU1 is the primary CPU
      .equ            STACK_BITS_PER_CPU, 15   @ means total stack size
                                               @ of each core is 0x8000 
      .equ            PRIMARY_CPU, 0
#endif

@ --- Stack memory locations
        .equ            FIQ_Stack_Size, 0x400
        .equ            IRQ_Stack_Size, 0x1000
        .equ            SVC_Stack_Size, 0x400
        .equ            USR_Stack_Size, 0x200
        .equ            ABORT_Stack_Size, 0x400
        .equ            SYS_Stack_Size, 0x5000

/* macro to get core hard id in SMP mode */
      .macro get_smp_hard_id reg0
        MRC     p15, 0, \reg0, c0, c0, 5
        AND     \reg0, \reg0, #0xF
      .endm

/** macro to configurate stacks of different modes of each core
 *  reg0: commonly used to store the stack address
 *  reg1: keep the core smp hard ID
 **/
      .macro config_all_stacks  reg0, reg1
      
#ifdef CONFIG_SMP
        @      Get SMP hard id
        get_smp_hard_id \reg1
#endif
        MSR     CPSR_c, #ABT_MODE | NO_INT
        LDR     \reg0, =__stack_end__

#ifdef CONFIG_SMP
        SUB     \reg0, \reg0, \reg1, lsl #STACK_BITS_PER_CPU
#endif
        mov     SP, \reg0
@ Enter FIQ mode and set up the FIQ stack pointer
        MSR     CPSR_c, #FIQ_MODE | NO_INT
        sub     \reg0, \reg0, #ABORT_Stack_Size
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
      
/** macro used to disbale memory system
 */    
      .macro   disable_mem    reg0
         mrc   p15, 0, \reg0, c1, c0, 0    @ Read CP15 register 1 into reg0
         bic   \reg0, \reg0,  #(1 << 0)    @ Clear MMU enable
         bic   \reg0, \reg0,  #(1 << 2)    @ Disable D-cache
         bic   \reg0, \reg0,  #(1 << 12)   @ Disable I-cache
         mcr   p15, 0, \reg0, c1, c0, 0    @ rite value back
      .endm
      
#endif
      