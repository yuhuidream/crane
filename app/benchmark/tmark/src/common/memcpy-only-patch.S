/*
* memcpy.S
*
*  Copyright (C) 2004, Intel Corporation
*
* This software program is licensed subject to the GNU General Public License (GPL).
* Version 2, June 1991, available at http://www.fsf.org/copyleft/gpl.html
*
*  This is a ASM optimized memcpy, memmove & bcopy function
*
*/
/*
#include <sysdep.h>
#include <endian.h>
*/
#define PLD(code...)    code
#define PLD_CACHELINE_NUM	64 		/* 2KB */
		.text
		.global memcpy_glibc_opt
memcpy_glibc_opt:
		stmfd 	sp!, {r0,r4-r11, lr}
1:
		mov 	r5, r1
		and	r5, r5, #~0x3
	PLD(	pld	[r5, #0]		)
	PLD(	pld	[r5, #0x20]		)
	PLD(	pld	[r5, #0x40]		)
		cmp 	r2, #4
		bls 	42f

		rsb	r4, r0, #0
		ands	r4, r4, #0x2
		ldrneb 	r5, [r1], #1
		ldrneb	r6, [r1], #1
		subne	r2, r2, #2
		strneb	r5, [r0], #1
		strneb  r6, [r0], #1

		ands 	r4, r0, #0x1
		ldrneb	r5, [r1], #1
		subne	r2, r2, #1
		strneb	r5, [r0], #1

		and	r3, r1, #3
		cmp 	r3, #3
		beq	34f
		cmp	r3, #2
		beq	24f
		cmp	r3, #1
		beq	14f

@The source and destination are word aligned.  We get an easy job.
2:

		and	r4, r0, #0x1C
		rsb	r4, r4, #32

		and	r5, r2, #0x1C
		cmp	r4, r2
		movhi	r4, r5

		cmp	r4, #0
		beq	5f

		rsb 	r3, r4, #32
		and	r3, r3, #0x1C
		sub 	r2, r2, r4

		adr 	r12, 3f
		add 	pc, r12, r3

3:
		ldr 	r4, [r1], #4
		ldr 	r5, [r1], #4
		ldr 	r6, [r1], #4
		ldr 	r7, [r1], #4
		ldr 	r8, [r1], #4
		ldr 	r9, [r1], #4
		ldr 	r10,[r1], #4
		ldr 	r11,[r1], #4

@Now jump into the store table
		adr 	r12, 4f
		add	pc, r12, r3

4:
		str	r4, [r0], #4
		str	r5, [r0], #4
		str	r6, [r0], #4
		str	r7, [r0], #4
		str	r8, [r0], #4
		str	r9, [r0], #4
		str	r10,[r0], #4
		str	r11,[r0], #4
@We are now cache line aligned on source
@/////////////////////////////////////////////////////////////////////////////////
@///////////////////////////////Aashish's Changes Start//////////////////////////
@/////////////////////////////////////////////////////////////////////////////////
5:
@     		mov     r10, r0      	@/make a copy of the current destination addr
		mov     r12, r1      	@/make a copy of the current source address
45:

		mov     r3, #0      	@//r3 keeps track of how many cache lines are being locked in the cache (allow at most 256 lines)

46:    					@//Loadloop1 is essentially doing just the loads upto 256 cache lines.
		cmp 	r2, #128	@//Check if less than 32 bytes are left to be copied
		bge     47f		@// If the count is more than 32 bytes fetch another line into the cache so jump to shortcut
		cmp 	r3, #4		@// If the load loop ran only atleast once that means storeloop needs to be executed
		bge     48f
		b	6f

47:
	PLD(	pld	[r1, #0]	)
	PLD(	pld	[r1, #32]	)
	PLD(	pld	[r1, #64]	)
	PLD( 	pld	[r1, #96]	)
		sub     r2, r2, #128
		add     r1, r1, #128
		add     r3, r3, #4		@/keep track of how many lines we are loading
		cmp	r3, #PLD_CACHELINE_NUM
@    		ldreqb  r4, [r1,#-32]          @/Should lock at most 1 way of the cache which is 8K bytes (256 lines)
		beq	48f
		b       46b

48:
		ldr	r4, [r12], #4
		ldr	r5, [r12], #4
		ldr     r6, [r12], #4
		ldr     r7, [r12], #4
		str	r4, [r0], #4
		str	r5, [r0], #4
		str     r6, [r0], #4
		str     r7, [r0], #4

ldr     r4, [r12], #4
ldr     r5, [r12], #4
ldr     r6, [r12], #4
ldr     r7, [r12], #4
str     r4, [r0], #4
str     r5, [r0], #4
str     r6, [r0], #4
str     r7, [r0], #4

ldr     r4, [r12], #4
ldr     r5, [r12], #4
ldr     r6, [r12], #4
ldr     r7, [r12], #4
str     r4, [r0], #4
str     r5, [r0], #4
str     r6, [r0], #4
str     r7, [r0], #4

ldr     r4, [r12], #4
ldr     r5, [r12], #4
ldr     r6, [r12], #4
ldr     r7, [r12], #4
str     r4, [r0], #4
str     r5, [r0], #4
str     r6, [r0], #4
str     r7, [r0], #4
		subs	r3, r3, #2	 @//Downcount the number of cache lines stored to the destination
		bne     48b
		b 	45b

@/////////////////////////////////////////////////////////////////////////////////
@////////////////////////////Aashish's Changes End/////////////////////////////
@/////////////////////////////////////////////////////////////////////////////////
6:
		cmp	r2, #32*4
		bls	7f
	PLD(	pld	[r1, #0x80]		)
7:

@Now we finish up the copy without any preloads.  The data should have already
@been loaded into the caches
8:
		cmp 	r2, #32
		bmi	9f

		ldmia	r1!, {r4-r11}
		stmia	r0!, {r4-r11}
		sub	r2, r2, #32
		b	8b

9:
		ands	r3, r2, #0x1C
		beq	42f
		sub	r2, r2, r3
		rsb	r3, r3, #32
		adr	r12, 10f
		add 	pc, r12, r3

10:
		ldr 	r4, [r1], #4
		ldr 	r5, [r1], #4
		ldr 	r6, [r1], #4
		ldr 	r7, [r1], #4
		ldr 	r8, [r1], #4
		ldr 	r9, [r1], #4
		ldr 	r10,[r1], #4
		ldr 	r11,[r1], #4

		adr 	r12, 11f
		add	pc, r12, r3

11:
		str	r4, [r0], #4
		str	r5, [r0], #4
		str	r6, [r0], #4
		str	r7, [r0], #4
		str	r8, [r0], #4
		str	r9, [r0], #4
		str	r10,[r0], #4
		str	r11,[r0], #4

		rsb	r2, r2, #4
		adr 	r12, 12f
		add	pc, r12, r2, LSL #2

12:
		ldrb	r3, [r1], #1
		ldrb	r4, [r1], #1
		ldrb	r5, [r1], #1
		ldrb	r6, [r1], #1
		adr	r12, 13f
		add	pc, r12, r2, LSL #2

13:
		strb	r3, [r0], #1
		strb	r4, [r0], #1
		strb	r5, [r0], #1
		strb	r6, [r0], #1

		ldmfd sp!, {r0,r4-r11, pc}


@The source and destination are not aligned.  We're going to have
@to load and shift data from a temporary buffer.  Stuff needs to be
@shifted to the right by 8 bits to align properly
14:

		and 	r3, r1, #~0x3
		ldr	lr, [r3], #4
		mov	lr, lr, LSR #8

		and	r4, r0, #0x1C
		rsb	r4, r4, #32

		and	r5, r2, #0x1C
		cmp	r4, r2
		movhi	r4, r5

		cmp	r4, #0
		beq	16f
		rsb 	r6, r4, #32
		and	r6, r6, #0x1C

		sub 	r2, r2, r4

		adr 	r12, 15f
		add 	pc, r12, r6, LSL #2

15:
		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

@We are now cache line aligned.
16:
		cmp 	r2, #(32*4 + 32)
		bmi 	17f
	PLD(	pld	[r3, #0x60]		)
	PLD(	pld	[r3, #0x80]		)
		ldmia	r3!, {r4, r5, r6, r7, r8, r9, r10, r11}
		orr	r1,lr, r4, LSL #24
		mov	lr, r4, LSR #8
		orr	r4, lr, r5, LSL #24
		mov	lr, r5, LSR #8

		orr	r5, lr, r6, LSL #24
		mov	lr, r6, LSR #8

		orr	r6, lr, r7, LSL #24
		mov	lr, r7, LSR #8

		orr	r7, lr, r8, LSL #24
		mov	lr, r8, LSR #8

		orr	r8, lr, r9, LSL #24
		mov	lr, r9, LSR #8

		orr	r9, lr, r10, LSL #24
		mov	lr, r10, LSR #8

		orr	r10, lr, r11, LSL #24
		mov	lr, r11, LSR #8

		stmia	r0!, {r1, r4, r5, r6, r7, r8, r9, r10}

		ldmia	r3!, {r4, r5, r6, r7, r8, r9, r10, r11}

		orr	r1,lr, r4, LSL #24
		mov	lr, r4, LSR #8
		orr	r4, lr, r5, LSL #24
		mov	lr, r5, LSR #8

		orr	r5, lr, r6, LSL #24
		mov	lr, r6, LSR #8

		orr	r6, lr, r7, LSL #24
		mov	lr, r7, LSR #8

		orr	r7, lr, r8, LSL #24
		mov	lr, r8, LSR #8

		orr	r8, lr, r9, LSL #24
		mov	lr, r9, LSR #8

		orr	r9, lr, r10, LSL #24
		mov	lr, r10, LSR #8

		orr	r10, lr, r11, LSL #24
		mov	lr, r11, LSR #8

		stmia	r0!, {r1, r4, r5, r6, r7, r8, r9, r10}

		sub	r2, r2, #64

		b	16b

17:
		cmp	r2, #32*4
		bls	18f
	PLD(	pld	[r3, #0x80]		)

18:

19:
		cmp 	r2, #32
		bmi	20f
		ldmia	r3!, {r4, r5, r6, r7, r8, r9, r10, r11}
		orr	r1,lr, r4, LSL #24
		mov	lr, r4, LSR #8
		orr	r4, lr, r5, LSL #24
		mov	lr, r5, LSR #8

		orr	r5, lr, r6, LSL #24
		mov	lr, r6, LSR #8

		orr	r6, lr, r7, LSL #24
		mov	lr, r7, LSR #8

		orr	r7, lr, r8, LSL #24
		mov	lr, r8, LSR #8

		orr	r8, lr, r9, LSL #24
		mov	lr, r9, LSR #8

		orr	r9, lr, r10, LSL #24
		mov	lr, r10, LSR #8

		orr	r10, lr, r11, LSL #24
		mov	lr, r11, LSR #8

		stmia	r0!, {r1, r4, r5, r6, r7, r8, r9, r10}

		sub	r2, r2, #32
		b	19b

20:

		ands	r6, r2, #0x1C
		subeq	r1, r3, #3
		beq	42f
		sub	r2, r2, r6
		rsb	r6, r6, #32
		adr	r12, 21f
		add 	pc, r12, r6, LSL #2

21:
		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #24
		str 	r12,[r0], #4
		mov	lr, r4, LSR #8

		sub	r1, r3, #3
		rsb	r2, r2, #4
		adr 	r12, 22f
		add	pc, r12, r2, LSL #2

22:
		ldrb	r3, [r1], #1
		ldrb	r4, [r1], #1
		ldrb	r5, [r1], #1
		ldrb	r6, [r1], #1
		adr	r12, 23f
		add	pc, r12, r2, LSL #2
23:
		strb	r3, [r0], #1
		strb	r4, [r0], #1
		strb	r5, [r0], #1
		strb	r6, [r0], #1

		ldmfd sp!, {r0,r4-r11, pc}
@The source and destination are not aligned.  We're going to have to load
@and shift data from a temporary buffer.  Stuff needs to be shifted to the
@right by 16 bits to align properly
24:
		and 	r3, r1, #~0x3
		ldr	lr, [r3], #4
		mov	lr, lr, LSR #16

		and	r4, r0, #0x1C
		rsb	r4, r4, #32

		and	r5, r2, #0x1C
		cmp	r4, r2
		movhi	r4, r5

		cmp	r4, #0
		beq	26f

		rsb 	r6, r4, #32
		and	r6, r6, #0x1C

		sub 	r2, r2, r4

		adr 	r12, 25f
		add 	pc, r12, r6, LSL #2

25:
		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

26:
		cmp 	r2, #(32*4 + 32)
		bmi 	27f
	PLD(	pld	[r3, #0x60]		)
	PLD(	pld	[r3, #0x80]		)

		ldmia	r3!, {r4, r5, r6, r7, r8, r9, r10, r11}
		orr	r1,lr, r4, LSL #16
		mov	lr, r4, LSR #16
		orr	r4, lr, r5, LSL #16
		mov	lr, r5, LSR #16

		orr	r5, lr, r6, LSL #16
		mov	lr, r6, LSR #16

		orr	r6, lr, r7, LSL #16
		mov	lr, r7, LSR #16

		orr	r7, lr, r8, LSL #16
		mov	lr, r8, LSR #16

		orr	r8, lr, r9, LSL #16
		mov	lr, r9, LSR #16

		orr	r9, lr, r10, LSL #16
		mov	lr, r10, LSR #16

		orr	r10, lr, r11, LSL #16
		mov	lr, r11, LSR #16

		stmia	r0!, {r1, r4, r5, r6, r7, r8, r9, r10}

		ldmia	r3!, {r4, r5, r6, r7, r8, r9, r10, r11}

		orr	r1,lr, r4, LSL #16
		mov	lr, r4, LSR #16
		orr	r4, lr, r5, LSL #16
		mov	lr, r5, LSR #16

		orr	r5, lr, r6, LSL #16
		mov	lr, r6, LSR #16

		orr	r6, lr, r7, LSL #16
		mov	lr, r7, LSR #16

		orr	r7, lr, r8, LSL #16
		mov	lr, r8, LSR #16

		orr	r8, lr, r9, LSL #16
		mov	lr, r9, LSR #16

		orr	r9, lr, r10, LSL #16
		mov	lr, r10, LSR #16

		orr	r10, lr, r11, LSL #16
		mov	lr, r11, LSR #16

		stmia	r0!, {r1, r4, r5, r6, r7, r8, r9, r10}

		sub	r2, r2, #64
		b 	26b

27:
		cmp	r2, #32*4
		bls	28f
	PLD(	pld	[r3, #0x80]		)

28:

29:
		cmp 	r2, #32
		bmi	30f

		ldmia	r3!, {r4, r5, r6, r7, r8, r9, r10, r11}

		orr	r1,lr, r4, LSL #16
		mov	lr, r4, LSR #16
		orr	r4, lr, r5, LSL #16
		mov	lr, r5, LSR #16

		orr	r5, lr, r6, LSL #16
		mov	lr, r6, LSR #16

		orr	r6, lr, r7, LSL #16
		mov	lr, r7, LSR #16

		orr	r7, lr, r8, LSL #16
		mov	lr, r8, LSR #16

		orr	r8, lr, r9, LSL #16
		mov	lr, r9, LSR #16

		orr	r9, lr, r10, LSL #16
		mov	lr, r10, LSR #16

		orr	r10, lr, r11, LSL #16
		mov	lr, r11, LSR #16

		stmia	r0!, {r1, r4, r5, r6, r7, r8, r9, r10}

		sub	r2, r2, #32
		b	29b

30:
		ands	r6, r2, #0x1C
		subeq	r1, r3, #2
		beq	42f
		sub	r2, r2, r6
		rsb	r6, r6, #32
		adr	r12, 31f
		add 	pc, r12, r6, LSL #2

31:
		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #16
		str 	r12,[r0], #4
		mov	lr, r4, LSR #16

		sub	r1, r3, #2
		rsb	r2, r2, #4
		adr 	r12, 32f
		add	pc, r12, r2, LSL #2

32:
		ldrb	r3, [r1], #1
		ldrb	r4, [r1], #1
		ldrb	r5, [r1], #1
		ldrb	r6, [r1], #1
		adr	r12, 33f
		add	pc, r12, r2, LSL #2
33:
		strb	r3, [r0], #1
		strb	r4, [r0], #1
		strb	r5, [r0], #1
		strb	r6, [r0], #1

		ldmfd sp!, {r0,r4-r11, pc}

@The source and destination are not aligned.  We're going to have to load
@and shift data from a temporary buffer.  Stuff needs to be shifted to the
@right by 24 bits to align properly
34:

		and 	r3, r1, #~0x3
		ldr	lr, [r3], #4
		mov	lr, lr, LSR #24

		and	r4, r0, #0x1C
		rsb	r4, r4, #32
		and	r5, r2, #0x1C
		cmp	r4, r2
		movhi	r4, r5

		cmp	r4, #0
		beq	36f

		rsb 	r6, r4, #32
		and	r6, r6, #0x1C
		sub 	r2, r2, r4

		adr 	r12, 35f
		add 	pc, r12, r6, LSL #2

35:
		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

36:
		cmp 	r2, #(32*4 + 32)
		bmi 	37f
	PLD(	pld	[r3, #0x60]		)
	PLD(	pld	[r3, #0x80]		)

		ldmia	r3!, {r4, r5, r6, r7, r8, r9, r10, r11}
		orr	r1,lr, r4, LSL #8
		mov	lr, r4, LSR #24

		orr	r4, lr, r5, LSL #8
		mov	lr, r5, LSR #24

		orr	r5, lr, r6, LSL #8
		mov	lr, r6, LSR #24

		orr	r6, lr, r7, LSL #8
		mov	lr, r7, LSR #24

		orr	r7, lr, r8, LSL #8
		mov	lr, r8, LSR #24

		orr	r8, lr, r9, LSL #8
		mov	lr, r9, LSR #24

		orr	r9, lr, r10, LSL #8
		mov	lr, r10, LSR #24

		orr	r10, lr, r11, LSL #8
		mov	lr, r11, LSR #24

		stmia	r0!, {r1, r4, r5, r6, r7, r8, r9, r10}

		ldmia	r3!, {r4, r5, r6, r7, r8, r9, r10, r11}

		orr	r1,lr, r4, LSL #8
		mov	lr, r4, LSR #24
		orr	r4, lr, r5, LSL #8
		mov	lr, r5, LSR #24

		orr	r5, lr, r6, LSL #8
		mov	lr, r6, LSR #24

		orr	r6, lr, r7, LSL #8
		mov	lr, r7, LSR #24

		orr	r7, lr, r8, LSL #8
		mov	lr, r8, LSR #24

		orr	r8, lr, r9, LSL #8
		mov	lr, r9, LSR #24

		orr	r9, lr, r10, LSL #8
		mov	lr, r10, LSR #24

		orr	r10, lr, r11, LSL #8
		mov	lr, r11, LSR #24

		stmia	r0!, {r1, r4, r5, r6, r7, r8, r9, r10}

		sub	r2, r2, #64
		b 	36b
37:
		cmp	r2, #32*4
		bls	38f
	PLD(	pld	[r3, #0x80]		)

38:

39:
		cmp 	r2, #32
		bmi	40f
		ldmia	r3!, {r4, r5, r6, r7, r8, r9, r10, r11}

		orr	r1,lr, r4, LSL #8
		mov	lr, r4, LSR #24
		orr	r4, lr, r5, LSL #8
		mov	lr, r5, LSR #24

		orr	r5, lr, r6, LSL #8
		mov	lr, r6, LSR #24

		orr	r6, lr, r7, LSL #8
		mov	lr, r7, LSR #24

		orr	r7, lr, r8, LSL #8
		mov	lr, r8, LSR #24

		orr	r8, lr, r9, LSL #8
		mov	lr, r9, LSR #24

		orr	r9, lr, r10, LSL #8
		mov	lr, r10, LSR #24

		orr	r10, lr, r11, LSL #8
		mov	lr, r11, LSR #24

		stmia	r0!, {r1, r4, r5, r6, r7, r8, r9, r10}

		sub	r2, r2, #32
		b	39b

40:

		ands	r6, r2, #0x1C
		subeq	r1, r3, #1
		beq	42f
		sub	r2, r2, r6
		rsb	r6, r6, #32
		adr	r12, 41f
		add 	pc, r12, r6, LSL #2

41:
		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24

		ldr 	r4, [r3], #4
		orr	r12,lr, r4, LSL #8
		str 	r12,[r0], #4
		mov	lr, r4, LSR #24
		sub	r1, r3, #1
42:
		rsb	r2, r2, #4
		adr 	r12, 	43f
		add	pc, r12, r2, LSL #2

43:
		ldrb	r3, [r1], #1
		ldrb	r4, [r1], #1
		ldrb	r5, [r1], #1
		ldrb	r6, [r1], #1
		adr	r12, 44f
		add	pc, r12, r2, LSL #2
44:
		strb	r3, [r0], #1
		strb	r4, [r0], #1
		strb	r5, [r0], #1
		strb	r6, [r0], #1

		ldmfd sp!, {r0,r4-r11, pc}

	.end
