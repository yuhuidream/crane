/*

	Marvell Optimized Memcopy Routine
	
	Authors:	Marlon A. Moncrieffe
				mamoncri@marvell.com
				
				Huaping Wang
				huaping@marvell.com

	Copyright (C) Marvell International Ltd. and/or its affiliates
*/

#define PLD_CACHELINE_NUM	128 		/* 4KB */

/*
  This macro will copy from an unaligned source to an destination
  by loading from an aligned address and shifting and combining
  the data before storing. 
  
  With each load, address is aligned, but data is not.
  
  r0 = dst, r1 = src, r2 = size 
 */

.macro copy_and_shift_by offset_right, offset_left
	
	/* get started with first word*/
	ldr	ip, [r1], #4				@ we're loading from an aligned address here, but data is not aligned

	/* are we copying more than 32 bytes? */
	cmp     r2, #32
	blt     3f                         		@ no, jump ahead, no need for an additional pld
	pld     [r1, #32]				@ more than 32 bytes means we need an additional pld
	
	/* are we copying more than 64 bytes? */
	cmp     r2,  #64						
	blt     2f             				@ no, jump ahead, no need for an additional pld

	/* main loop of unaligned copy*/
1:	
	pld     [r1, #64]						
2:
	mov	r3, ip, lsr #(\offset_right)		@ align the first part of the data

	/* load 32 bytes of data */
	ldm    	r1!, {r4-r10,ip}			        @ these loads will be from aligned addresses, but the data is still not aligned
	
	/* shift the data in each register */
	orr		r3, r3, r4, lsl #(\offset_left)		@ align and combine the next part of the data

	mov		r4, r4, lsr #(\offset_right)		@ align the first part of the data
	orr		r4, r4, r5, lsl #(\offset_left)		@ align and combine the next part of the data

	mov		r5, r5, lsr #(\offset_right)
	orr		r5, r5, r6, lsl #(\offset_left)

	mov		r6, r6, lsr #(\offset_right)
	orr		r6, r6, r7, lsl #(\offset_left)

	mov		r7, r7, lsr #(\offset_right)
	orr		r7, r7, r8, lsl #(\offset_left)

	mov		r8, r8, lsr #(\offset_right)
	orr		r8, r8, r9, lsl #(\offset_left)

	mov		r9, r9, lsr #(\offset_right)
	orr		r9, r9, r10, lsl #(\offset_left)

	mov		r10, r10, lsr #(\offset_right)
	orr		r10, r10, ip, lsl #(\offset_left)
	
	stm	        r0!, {r3-r10}

	/* check how many bytes we have remaining */
	sub             r2, r2, #32
	cmp             r2, #32

	/* loop again if we have more than 32 remaining */
	bge    	1b 

        /* copy remaining bytes which are less than 32 */
3:
	mov ip, ip, lsr #(\offset_right)	
	and r3, r2, #28				@ bytes remaining that are multiples of 4
	sub r2, r2, r3				@ reduce the remaining byte count
        rsb r3, r3, #32				@ offset we'll use for our jump table

        adr r11, 4f
        add pc, r11, r3, lsl #2

4:	
        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        str r11,[r0], #4
        mov ip, r4, lsr #(\offset_right)

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        str r11,[r0], #4
        mov ip, r4, lsr #(\offset_right)

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        str r11,[r0], #4
        mov ip, r4, lsr #(\offset_right)

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        str r11,[r0], #4
        mov ip, r4, lsr #(\offset_right)

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        str r11,[r0], #4
        mov ip, r4, lsr #(\offset_right)

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        str r11,[r0], #4
        mov ip, r4, lsr #(\offset_right)

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        str r11,[r0], #4
        mov ip, r4, lsr #(\offset_right)

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        str r11,[r0], #4
        mov ip, r4, lsr #(\offset_right)

        mov r4, #(\offset_left)	
        sub r1, r1, r4, lsr #3

        /* copy left bytes which is less than 4*/
        movs   r7, r2, lsl #31
	ldrcsb r3, [r1], #1
	strcsb r3, [r0], #1
	ldrcsb r3, [r1], #1
	strcsb r3, [r0], #1
	ldrmib r3, [r1], #1
	strmib r3, [r0], #1
	
        ldmfd   sp!, {r0, r4-r12, lr}	 
        bx	    lr
 .endm

 
 /* memcpy routine
  mrvl_memcpy(dst, src, size)
original name mrvl_memcpy32*/

	.align
	.global mrvl_memcpy_v3_2
	.func   mrvl_memcpy_v3_2

mrvl_memcpy_v3_2:
    /* are we 256b aligned? */
	mov     ip, #0xff
	and     ip, r2, ip
	cmp     ip, #0
	beq     44f

	stmfd	sp!, {r0, r4-r12, lr}	

	/*prime the pump, so to speak*/
	pld     [r1, #0]
	
	/*less than four bytes we will handle right away*/
	cmp		r2, #4
	blt		copy_4_or_less
	
	/*is the destination address aligned on a 4 byte boundary?*/
	ands	r3, r0, #3	
	beq	dest_aligned

	/*destination is not aligned. find out how much we are off by*/
	rsb	r3, r3, #4					@bytes to copy = 4 - (r0 & #3)
	
	/*should we copy at least two bytes?*/
	tst	r3, #2
	ldrneb	r4, [r1], #1
	ldrneb  r5, [r1], #1
	strneb  r4, [r0], #1
	strneb  r5, [r0], #1
	subne	r2, r2, #2

	/*how about one byte?*/
	tst	r3, #1
	ldrneb	r7, [r1], #1
	strneb 	r7, [r0], #1
	subne	r2, r2, #1

	/*at this point we should be destination aligned (4 byte alignment)*/
dest_aligned:

	/*is the source address aligned on a 4 byte boundary?*/
	ands	r3, r1, #3
	beq	copy_aligned_start

	/*source address is not aligned*/
copy_unaligned:

	/*get the source address word aligned*/
	sub	r1, r1, r3

	/*how much will we have to shift the data by?*/
	cmp	r3, #1
	beq	off_by_1

	cmp	r3, #2
	beq	off_by_2 

off_by_3:	
	copy_and_shift_by 24, 8		@to the right by 24 for the first part of the data, 
					@to the left by 8 for the rest
	
off_by_2:	
	copy_and_shift_by 16, 16

off_by_1:	
	copy_and_shift_by 8, 24


44:	
	stmfd	sp!, {r0, r4-r7}	
	mov     r12, r1     @/make a copy of the current source address
45:  
	mov     r3, #0      @//r3 keeps track of how many cache lines are being locked in the cache (allow at most 256 lines)

	
46:    					@//Loadloop1 is essentially doing just the loads upto 256 cache lines.
	cmp 	r2, #128	@//Check if less than 32 bytes are left to be copied
	bge     47f			@// If the count is more than 32 bytes fetch another line into the cache so jump to shortcut	tst		r2, #256
	cmp 	r3, #4		@// If the load loop ran only atleast once that means storeloop needs to be executed
	bge     48f
	ldmfd	sp!, {r0, r4-r7}	
	bx		lr

  		
47:
	pld		[r1, #0]
	pld		[r1, #32]
	pld		[r1, #64]
	pld		[r1, #96]
	sub     r2, r2, #128
	add     r1, r1, #128
	add     r3, r3, #4		@/keep track of how many lines we are loading
	cmp		r3, #PLD_CACHELINE_NUM

	beq		48f
	b		46b
 
48:
	ldr		r4, [r12], #4
	ldr		r5, [r12], #4
	ldr     r6, [r12], #4
	ldr     r7, [r12], #4
	str		r4, [r0], #4
	str		r5, [r0], #4
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

	@dmb					@//data memory barrier
	
	@mrc		p15, 0, r3, c7, c10, 5
	b		45b
	
	
	
	/* here we are both source and destination aligned */

copy_aligned_start:

    /* are we 256b aligned? */
	mov     r4, #0xff
	and     r4, r2, r4
	cmp     r4, #0
	beq     44b

	/* are we copying more than 32 bytes? */
	cmp     r2, #32
    blt     copy_32_or_less		@ no, jump ahead, no additional preload necessary
	pld     [r1, #32]		@ yes, we have more than 32 bytes, do another preload
	
	/* are we copying more than 64 bytes? */
	cmp     r2, #64
	blt     copy_aligned_wo_pld	@ no, jump ahead, no additional preload necessary

copy_aligned_loop:
	pld     [r1, #64]

copy_aligned_wo_pld:
	ldmia   r1!, {r4 - r11}
	sub    r2, r2, #32
	stmia   r0!, {r4 - r11}
	cmp		r2, #32
	bge     copy_aligned_loop

copy_32_or_less:
    movs   ip, r2, lsl #28
  	ldmcs  r1!, {r4, r5, r6, r7}
	stmcs  r0!, {r4, r5, r6, r7}
	ldmmi  r1!, {r4, r5}
	stmmi  r0!, {r4, r5}
	
copy_4_or_less:
	movs   ip, r2, lsl #30
	ldrcs  r4, [r1],#4
	strcs  r4, [r0],#4
	ldrmib r4, [r1],#1
	ldrmib r5, [r1],#1
	strmib r4, [r0],#1
	strmib r5, [r0],#1
	tst    r2, #0x1
	ldrneb r3, [r1]
	strneb r3, [r0]
	
copy_aligned_end:
	
	ldmfd	sp!, {r0, r4-r12, lr}	
	bx	lr

	.endfunc
