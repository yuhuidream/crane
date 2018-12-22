.title  Load Latency Test

	.global single_ldr_32_linked
	.global single_ldr_64_linked
	.global single_ldr_4
	.global single_ldr_32
	.global single_ldr_32_DSB
	.global single_ldr_64
	.global	pipe_ldr_32
	.global pipe_pld_32
	.global pipe_ldr_32_WOBU
	.global pipe_ldr_64
	.global pipe_ldr_32_loop12
	.global pipe_ldr_4
	.global pipe_ldrd_8
	.global calibrate_overhead
	.global coalesce_load_4_ldm_2
	.global coalesce_load_4_ldm_4
	.global coalesce_load_4_ldm_8

#ifdef CPU_V6
	#define dsb mcr p15, 0, r0, c7, c10, 4
#endif	

	.text
	.align 5

@*****************************************************
@ For all routines below:
@ 	r0: Points to src buffer to be loaded
@ 	r1: ldr times
@ Return:
@	None
@*****************************************************
calibrate_overhead:
	stmfd   sp!,{r4-r11}
	ldmfd  	sp!, {r4-r11}
	bx	lr

coalesce_load_4_ldm_2:
STMFD   sp!,{r4-r11}     @PUSH

loop32ldm_2:
ldmia   r0!, {r2-r3}
ldmia   r0!, {r2-r3}
subs    r1, r1, #4
bgt     loop32ldm_2

ldmfd   sp!, {r4-r11}
bx      lr               @POP

coalesce_load_4_ldm_4:
STMFD   sp!,{r4-r11}     @PUSH

loop32ldm_4:
ldmia   r0!, {r2-r5}
ldmia   r0!, {r2-r5}
subs    r1, r1, #8
bgt     loop32ldm_4

ldmfd   sp!, {r4-r11}
bx      lr               @POP

coalesce_load_4_ldm_8:
STMFD   sp!,{r4-r11}     @PUSH

loop32ldm_8:
ldmia   r0!, {r2-r9}
ldmia   r0!, {r2-r9}
subs    r1, r1, #16
bgt     loop32ldm_8

ldmfd   sp!, {r4-r11}
bx      lr               @POP


single_ldr_32_linked:
	ldr	r0, [r0]
	subs	r1, #16
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	bgt     single_ldr_32_linked
	BX      lr

single_ldr_64_linked:
	ldr	r0, [r0]
	subs	r1, #16
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr	r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	ldr     r0, [r0]
	bgt     single_ldr_64_linked
	BX      lr

single_ldr_32:
	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1

	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1

	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1

	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1
	LDR     r2, [r0], #32
	add	r2, r2,#1

	SUBS    r1, r1, #16
	BGT     single_ldr_32
	BX	lr

single_ldr_32_DSB:
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb

	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb

	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb

	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb
	LDR     r2, [r0], #32
	add	r2, r2,#1
	dsb

	SUBS    r1, r1, #16
	BGT     single_ldr_32_DSB
	BX	lr

single_ldr_64:
	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1

	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1

	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1

	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1
	LDR     r2, [r0], #64
	add	r2, r2,#1

	SUBS    r1, r1, #16
	BGT     single_ldr_64
	BX	lr

single_ldr_4:
	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1

	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1

	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1

	LDR 	r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1
	LDR     r2, [r0], #4
	add	r2, r2,#1

	SUBS    r1, r1, #16
	BGT     single_ldr_4
	BX	lr

@******************************************************************************
@Manzano core has 12-entry memory buffer which can holds up to 12 cache line
@load to external memory, each entry has 3 pend entries.
@******************************************************************************


pipe_ldr_32:

STMFD	sp!, {r4-r11}	@PUSH

Loop_pipe_ldr_32:

LDR     r4, [r0], #32
LDR     r5, [r0], #32
LDR     r6, [r0], #32
LDR     r7, [r0], #32

LDR     r8,  [r0], #32
LDR     r9,  [r0], #32
LDR     r10, [r0], #32
LDR     r11, [r0], #32

LDR     r4, [r0], #32
LDR     r5, [r0], #32
LDR     r6, [r0], #32
LDR     r7, [r0], #32

LDR     r8,  [r0], #32
LDR     r9,  [r0], #32
LDR     r10, [r0], #32
LDR     r11, [r0], #32

add     r1, #16
SUBS    r1, r1, #32

BGT     Loop_pipe_ldr_32

LDMFD   sp!, {r4-r11}	@POP
BX	    lr


pipe_ldr_32_WOBU:

STMFD	sp!, {r4-r11}	@PUSH

Loop_pipe_ldr_32_WOBU:

LDR     r4, [r0, #0]
LDR     r5, [r0, #32]
LDR     r6, [r0, #64]
LDR     r7, [r0, #96]

LDR     r8,  [r0, #128]
LDR     r9,  [r0, #160]
LDR     r10, [r0, #192]
LDR     r11, [r0, #224]

LDR     r4, [r0, #256]
LDR     r5, [r0, #288]
LDR     r6, [r0, #320]
LDR     r7, [r0, #352]

LDR     r8,  [r0, #384]
LDR     r9,  [r0, #416]
LDR     r10, [r0, #448]
LDR     r11, [r0, #480]

add     r0, #512
SUBS    r1, r1, #16

BGT     Loop_pipe_ldr_32_WOBU

LDMFD   sp!, {r4-r11}	@POP
BX	    lr


pipe_pld_32:

STMFD	sp!, {r4-r11}	@PUSH

Loop_pipe_pld_32:

pld     [r0, #0]
pld     [r0, #32]
pld     [r0, #64]
pld     [r0, #96]

pld     [r0, #128]
pld     [r0, #160]
pld     [r0, #192]
pld     [r0, #224]

pld     [r0, #256]
pld     [r0, #288]
pld     [r0, #320]
pld     [r0, #352]

pld     [r0, #384]
pld     [r0, #416]
pld     [r0, #448]
pld     [r0, #480]

ADD	r0, #512
SUBS    r1, r1, #16

BGT     Loop_pipe_pld_32

LDMFD   sp!, {r4-r11}	@POP
BX	    lr

pipe_ldr_64:
STMFD    sp!, {r4-r11}	@PUSH

Loop_pipe_ldr_64:
LDR     r4, [r0], #64
LDR     r5, [r0], #64
LDR     r6, [r0], #64
LDR     r7, [r0], #64

LDR     r8,  [r0], #64
LDR     r9,  [r0], #64
LDR     r10, [r0], #64
LDR     r11, [r0], #64

LDR     r4, [r0], #64
LDR     r5, [r0], #64
LDR     r6, [r0], #64
LDR     r7, [r0], #64

LDR     r8,  [r0], #64
LDR     r9,  [r0], #64
LDR     r10, [r0], #64
LDR     r11, [r0], #64

SUBS    r1, r1, #16

BGT     Loop_pipe_ldr_64
LDMFD   sp!, {r4-r11}	@POP
BX	    lr


pipe_ldr_32_loop12:
STMFD   sp!, {r4-r11, lr}   @PUSH

Loop_pipe_ldr_32_loop12:
LDR     r4, [r0], #32
LDR     r5, [r0], #32
LDR     r6, [r0], #32
LDR     r7, [r0], #32

LDR     r8,  [r0], #32
LDR     r9,  [r0], #32
LDR     r10, [r0], #32
LDR     r11, [r0], #32

LDR     r2, [r0], #32
LDR     r3, [r0], #32
LDR     r12, [r0], #32
LDR     r14, [r0], #32

LDR     r4, [r0], #32
LDR     r5, [r0], #32
LDR     r6, [r0], #32
LDR     r7, [r0], #32

LDR     r8,  [r0], #32
LDR     r9,  [r0], #32
LDR     r10, [r0], #32
LDR     r11, [r0], #32

LDR     r2, [r0], #32
LDR     r3, [r0], #32
LDR     r12, [r0], #32
LDR     r14, [r0], #32

LDR     r2, [r0], #32
LDR     r3, [r0], #32
LDR     r12, [r0], #32
LDR     r14, [r0], #32

LDR     r4, [r0], #32
LDR     r5, [r0], #32
LDR     r6, [r0], #32
LDR     r7, [r0], #32

LDR     r8,  [r0], #32
LDR     r9,  [r0], #32
LDR     r10, [r0], #32
LDR     r11, [r0], #32

LDR     r2, [r0], #32
LDR     r3, [r0], #32
LDR     r12, [r0], #32
LDR     r14, [r0], #32

LDR     r2, [r0], #32
LDR     r3, [r0], #32
LDR     r12, [r0], #32
LDR     r14, [r0], #32

LDR     r4, [r0], #32
LDR     r5, [r0], #32
LDR     r6, [r0], #32
LDR     r7, [r0], #32

LDR     r8,  [r0], #32
LDR     r9,  [r0], #32
LDR     r10, [r0], #32
LDR     r11, [r0], #32

LDR     r2, [r0], #32
LDR     r3, [r0], #32
LDR     r12, [r0], #32
LDR     r14, [r0], #32

SUBS    r1, r1, #48

BGT     Loop_pipe_ldr_32_loop12

LDMFD   sp!, {r4-r11, lr}   @POP
BX	    lr

pipe_ldr_4:
STMFD   sp!, {r4-r9}   @PUSH

Loop_pipe_ldr_4:
LDR     r2, [r0], #4
LDR     r3, [r0], #4
LDR     r4, [r0], #4
LDR     r5, [r0], #4

LDR     r6, [r0], #4
LDR     r7, [r0], #4
LDR     r8, [r0], #4
LDR     r9, [r0], #4

LDR     r2, [r0], #4
LDR     r3, [r0], #4
LDR     r4, [r0], #4
LDR     r5, [r0], #4

LDR     r6, [r0], #4
LDR     r7, [r0], #4
LDR     r8, [r0], #4
LDR     r9, [r0], #4

SUBS    r1, r1, #16

BGT     Loop_pipe_ldr_4
LDMFD   sp!, {r4-r9}   @POP
BX	    lr

pipe_ldrd_8:
STMFD   sp!, {r4-r9}   @PUSH

Loop_pipe_ldrd_8:
LDRD     r2, [r0], #8
LDRD     r4, [r0], #8
LDRD     r6, [r0], #8
LDRD     r8, [r0], #8

LDRD     r2, [r0], #8
LDRD     r4, [r0], #8
LDRD     r6, [r0], #8
LDRD     r8, [r0], #8

LDRD     r2, [r0], #8
LDRD     r4, [r0], #8
LDRD     r6, [r0], #8
LDRD     r8, [r0], #8

LDRD     r2, [r0], #8
LDRD     r4, [r0], #8
LDRD     r6, [r0], #8
LDRD     r8, [r0], #8

SUBS     r1, r1, #16

BGT      Loop_pipe_ldrd_8

LDMFD    sp!, {r4-r9}   @POP
BX	     lr


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@test case for DMC BUS OCCUPIED EVENTS
@ copy the address from ro to r1, which is target address
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
load_dcm_d32:
	STMFD   sp!, {r4-r10}   @PUSH
load_dcm_interleave:
	ldr     r3, [r0], #32
	ldr     r4, [r1], #32
	ldr     r5, [r0], #32
	ldr     r6, [r1], #32

	ldr     r7, [r0], #32
	ldr     r8, [r1], #32
	ldr     r9, [r0], #32
	ldr     r2, [r1], #32

	ldr     r3, [r0], #32
	ldr     r4, [r1], #32
	ldr     r5, [r0], #32
	ldr     r6, [r1], #32

	ldr     r7, [r0], #32
	ldr     r8, [r1], #32
	ldr     r9, [r0], #32
	ldr     r10, [r1], #32
	subs    r2,r2,#16
	bgt     load_dcm_interleave
	LDMFD   sp!, {r4-r10}   @POP
	BX	lr

	.end

@EOF
