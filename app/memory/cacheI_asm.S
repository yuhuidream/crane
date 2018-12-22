@------------------------------------------------------------
@ (C) Copyright [2006-2008] Marvell International Ltd.
@ All Rights Reserved
@------------------------------------------------------------

@****************************************************************************/
@
@ DESCRIPTION:  Wrapper to call the Cashe-Instruction test
@
@****************************************************************************/


@
@ void cacheI_runAsmWrapper(uint32_t* cntrR0, 
@                           uint32_t* accessSlow, 
@                           uint32_t* accessFast, 
@                           uint32_t* runAddr)
@
     .text
     .code 32
     .align 5

@@============================================================================
@@
      .global cacheI_runAsmWrapper
      .func cacheI_runAsmWrapper
cacheI_runAsmWrapper:

      @@ Free registers R0,R1,R2,R3 for further using:
      @@   r0=cntrr0
      @@   r1=ReadCycleCounter after first loop
      @@   r2=ReadCycleCounter after second loop
      @@   r3=run adresses
      @@   r7=ReadCycleCounter on beginning
      @@   r8=max & decrement loop-index
      @@
      stmdb  r13!,{r4-r10,r14}    @ Keep R4,R5,R6 on stack (aligned num of regs)
      mov    r4, r0  @ address &cntrR0     in R4
      ldr    r0,[r0] @  set init value instead of address
      mov    r5, r1  @ address &accessSlow in R5
      mov    r6, r2  @ address &accessFast in R6
      
      blx    r3      @ jump to the "runAddr"
      @@@@ ======== Test is running =======

      sub    r2, r2,r1
      sub    r1, r1,r7
      str    r0, [r4]  @ cntrR0     to address &cntrR0    
      str    r1, [r5]  @ accessSlow to address &accessSlow
      str    r2, [r6]  @ accessFast to address &accessFast
      
      ldmia  r13!,{r4-r10,pc}

      .endfunc

@@============================================================================
@@
      .align 4      @ Insures double word allignment
      
      .global cacheI_runCache_crtx
      .func   cacheI_runCache_crtx
cacheI_runCache_crtx:
  @@_____align_32
      @@   r0=cntrr0
      @@   r1=ReadCycleCounter after first loop
      @@   r2=ReadCycleCounter after second loop
      @@   r3=run adresses
      @@   r7=ReadCycleCounter on beginning
      @@   r8=max & decrement loop-index
      @@
      mov    r8, #2      @ MAX loops
      mov    r2, #0
      add    r3, #0x20   @ set ptr to the "cacheI_runCache_loop_crtx"
      nop @ Invalidate is from API (just before assembly)
      nop
      nop
      MCR p15, 0, R0, c7, c5 , 0   @ Invalidate I-cache L1 
      MRC p15, 0, R7, c9, c13, 0   @ ReadCycleCounter
  
  @@_____align_32
  @@ Here is the BEGIN of tested Array
      .global cacheI_runCache_loop_crtx
cacheI_runCache_loop_crtx:
      mov  r1, r2   @ keep "previous" loop result
      sub  r8, #1   @ ++loop
      add  r0, #1   @@ -- pattern_begin
      add  r0, #1
      add  r0, #1
      add  r0, #1
      add  r0, #1
      add  r0, #1   @@_____align_32
      add  r0, #1   @@
      add  r0, #1
      add  r0, #1
      add  r0, #1
      add  r0, #1
      add  r0, #1
      add  r0, #1
      add  r0, #1   @@_____align_32
      add  r0, #1   @@
      add  r0, #1
      add  r0, #1
      add  r0, #1
      add  r0, #1
      add  r0, #1
      add  r0, #1
      add  r0, #1   @@_____align_32
      add  r0, #1   @@
      add  r0, #1
      add  r0, #1
      add  r0, #1
    .global cacheI_runCache_loop_exit_crtx
cacheI_runCache_loop_exit_crtx:
   @@ size()= 4cmd_dwords
      MRC p15, 0, R2, c9, c13, 0   @ ReadCycleCounter
      cmp  r8, #0     @ loop exit condition
      movne  pc, r3   @ = bne cacheI_runCache_loop_crtx
      nop             @ !!! prevent next-command Cache-loading on first loop

  @@ Here is the END of tested Array
  @@_____align_32
cacheI_runCache_exit_crtx:
      bx LR
cacheI_runCache_end_crtx:

      .endfunc

@@============================================================================
@@
      .align   4     @ Insures double word allignment
      .global cacheD_runCache
      .func   cacheD_runCache
cacheD_runCache:
  @@_____align_32
      @@   r0=run addresses
      @@   r1=size
     @@   return r0=checksum
      @stmfd   sp!,    {lr,r1-r5}            @ Save link register on the stack
      stmfd   sp!,     {r1-r5, lr}
      mov r4,  #0
      mov r5,  r0
      add r2,  r0,  r1
      mov r0,  #0
tag1:    
      ldr r3,  [r5],  #32
      add r0,  r0,  r3
      cmp r5,  r2
      blt tag1      

      @@@@@@ldmfd   sp!,    {pc,r1-r5}            @ Return to caller
      ldmfd   sp!,    {r1-r5, pc}
 
      .endfunc

     .align   4     @ Insures double word allignment
      
     .end
