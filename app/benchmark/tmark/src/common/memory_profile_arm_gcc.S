@------------------------------------------------------------------------------
@ * Filename:    profile_arm.s
@ *
@ * Description: Low-level routines for memory performance measurement
@------------------------------------------------------------------------------

    @AREA    |.text|, CODE, READONLY
    @ARM
#ifndef CONFIG_CR5  
@Now we use Linaro baremetal tool chain to compile CR5 codes, 
@this CPU option is not supported
		.cpu marvell-f
#endif
		.fpu softvfp
		.arm
		.text
		.align	2

@------------------------------------------------------------------------------
@ Function profileMemoryRead
@ This code does burst reads, if the core supports burst reads it will load
@ to r3-r10 on one line fetch (32Bytes), The core should initiate
@ the next read while the first read is going on as the data is loaded into a different register.
@ if burst read is not supported, it will be a sequential read to every single registers
@ Call this function with interrupts disabled
@ In one loop the code reads 1KB data to the regirsters.
@ 1st Parameter (r0) = Memory address to read from.
@ 2nd Parameter (r1) = Cache line size (should be 32)
@ 3rd Parameter (r2) = number of 1kB chunks we want read.  The more the better to get accurate
@		       data.  Make sure there is memory for it.
@------------------------------------------------------------------------------
profileMemoryRead:
    @Stack the register values, prior to reading to it
    stmfd   sp!, {r3-r10, lr}
    @subtract the loop count
    sub     r2, r2, #1
profileMemoryRead_loop:
    ldr     r3, [r0], r1
    ldr     r4, [r0], r1
    ldr     r5, [r0], r1
    ldr     r6, [r0], r1
    ldr     r7, [r0], r1
    ldr     r8, [r0], r1
    ldr     r9, [r0], r1
    ldr     r10, [r0], r1
    ldr     r3, [r0], r1
    ldr     r4, [r0], r1
    ldr     r5, [r0], r1
    ldr     r6, [r0], r1
    ldr     r7, [r0], r1
    ldr     r8, [r0], r1
    ldr     r9, [r0], r1
    ldr     r10, [r0], r1
    ldr     r3, [r0], r1
    ldr     r4, [r0], r1
    ldr     r5, [r0], r1
    ldr     r6, [r0], r1
    ldr     r7, [r0], r1
    ldr     r8, [r0], r1
    ldr     r9, [r0], r1
    ldr     r10, [r0], r1
    ldr     r3, [r0], r1
    ldr     r4, [r0], r1
    ldr     r5, [r0], r1
    ldr     r6, [r0], r1
    ldr     r7, [r0], r1
    ldr     r8, [r0], r1
    ldr     r9, [r0], r1
    ldr     r10, [r0], r1
    @subtract the loop count
    subs     r2, r2, #1
    bpl     profileMemoryRead_loop
    @restore the registers
    ldmfd   sp!, {r3-r10, pc}


cleanAndInvalidateCache:
    PUSH     {r4-r7}
    MOV      r2,#0
    MOV      r5,#1
    MOV      r1,#5
    MOV      r12,#0x1d
    LSL      r6,r2,r5
.Llocal21:
    MOV      r0,#0
    ORR      r4,r6,r2,LSL r12
.Llocal22:
    ORR      r3,r4,r0,LSL r1
    MCR      p15,#0x0,r3,c7,c14,#2
    ADD      r0,r0,#1
    CMP      r0,#0x1000
    BCC      .Llocal22
    ADD      r2,r2,#1
    CMP      r2,#8
    BCC      .Llocal21
    MOV      r0,#1
    MOV      r2,#0
    LSL      r5,r0,r5
.Llocal23:
    MOV      r0,#0
    ORR      r4,r5,r2,LSL r12
.Llocal24:
    ORR      r3,r4,r0,LSL r1
    MCR      p15,#0x0,r3,c7,c14,#2
    ADD      r0,r0,#1
    CMP      r0,#0x10000
    BCC      .Llocal24
    ADD      r2,r2,#1
    CMP      r2,#8
    BCC      .Llocal23
    POP      {r4-r7}
    MCR      p15, #0x0,r0,c7,c10,#4
    BX       lr

    .MACRO GetCounter reg
                MRC      p15, #0, \reg, c9, c13, #0 @ $reg <- CCNT
    .ENDM

    .MACRO StopCounter
		mrc p15, 0, r6, c9, c12, #0
		ldr	r7, =0xfffffffe
		and	r6, r6, r7
		mcr p15, 0, r6, c9, c12, #0
    .ENDM

    .MACRO ResetCounter
		mrc p15, 0, r6, c9, c12, #0
		ldr	r7, =0xfffffff0
		and	r6, r6, r7
		mcr p15, 0, r6, c9, c12, #0
		mov r6, #0	@ we always set pcounter 0
		mcr p15, 0, r6, c9, c13, #0
    .ENDM

    .MACRO StartCounter
		mrc p15, 0, r6, c9, c12, #1
		ldr	r7, =0x80000000
		orr	r6, r6, r7
		mcr p15, 0, r6, c9, c12, #1
		mrc p15, 0, r6, c9, c12, #0
		orr	r6, r6, #0x1
		mcr p15, 0, r6, c9, c12, #0
    .ENDM



@ DWORD ReadTest( void *buf, DWORD sz )@
@ r0 - address of buffer
@ r1 - size in bytes
	.global	ReadTest
	.type	ReadTest, %function
ReadTest:
    PUSH     {r4-r7,lr}
    MOV      r4, r0      @ save args
    LSRS     r5, r1, #10 @ # of 1k chunks
    MOVEQ    r0, #0      @ if size is < 1k, return 0
    BEQ      readReturn
@    BL       cleanAndInvalidateCache
    MOV      r0, r4      @ buffer
    MOV      r1, #32     @ cache line size
    MOV      r2, r5      @ # of 1k chunks
@    ResetCounter
@    StartCounter
    BL       profileMemoryRead
@    StopCounter
@    GetCounter r0
readReturn:
    POP      {r4-r7,pc}
