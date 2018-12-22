/*

	Marvell Optimized Memcopy Routine
	
	Authors:	Marlon A. Moncrieffe
			mamoncri@marvell.com
				
			Huaping Wang
			huaping@marvell.com

	Copyright (C) Marvell International Ltd. and/or its affiliates
*/

/*
  This macro will copy from an unaligned source to an destination
  by loading from an aligned address and shifting and combining
  the data before storing. 
  
  With each load, address is aligned, but data is not.
  
  r0 = dst, r1 = src, r2 = size 
 */

.macro copy_and_shift_by offset_right, offset_left
	ldr	ip, [r1], #4				@ we're loading from an aligned address here, but data is not aligned
	cmp     r2, #32
	pld     [r1, #0x80]
	blt     4f                         		
        cmp    	r2, #16384                              @ if data size is less than 16k, no cache clean operation.
        pld     [r1, #0xA0]		
        pld     [r1, #0xC0]	
        blt    	2f                      
  /* The size of data to be copied is larger than 16k, we will do cache clean operation every 8k data copy */
        mov  	lr, r0
	sub  	r2, r2, #16384
        mov  	r11, #256       
        /* Data preload, shift and copy loop, in 8k granularity */ 
1:     
	subs    r11, r11, #1
	mov	r3, ip, lsr #(\offset_right)		@ align the first part of the data
	ldm    	r1!, {r4-r10,ip}			@ these loads will be from aligned addresses, but the data is still not aligned
	orr	r3, r3, r4, lsl #(\offset_left)		@ align and combine the next part of the data
	mov	r4, r4, lsr #(\offset_right)		@ align the first part of the data
	orr	r4, r4, r5, lsl #(\offset_left)		@ align and combine the next part of the data
	mov	r5, r5, lsr #(\offset_right)
	orr	r5, r5, r6, lsl #(\offset_left)
	mov	r6, r6, lsr #(\offset_right)
	orr	r6, r6, r7, lsl #(\offset_left)
	mov	r7, r7, lsr #(\offset_right)
	orr	r7, r7, r8, lsl #(\offset_left)
	mov	r8, r8, lsr #(\offset_right)
	orr	r8, r8, r9, lsl #(\offset_left)
	mov	r9, r9, lsr #(\offset_right)
	orr	r9, r9, r10, lsl #(\offset_left)
	mov	r10, r10, lsr #(\offset_right)
	orr	r10, r10, ip, lsl #(\offset_left)
	stm	r0!, {r3-r10}                           @store the shifted data to the 4-byte aligned destination
	pld     [r1, #0xE0]
	bne     1b
	subs    r2, r2, #8192
        dmb                                             @drain MBU for data write                                
        /* clean 8k data to memory */
        sub     r0, r0, #32
        mcrr    p6, 1, r0, lr, c6       
        add     r0, r0, #32
        dmb
        movge   lr, r0
        movge   r11, #256
        bge     1b

        add   r2, r2, #16384
               
  /* copy remaining bytes which are less than 16k, cache clean operation is not needed */ 
2:       
        sub     r2, r2, #32
3:
	subs    r2, r2, #32			
	mov	r3, ip, lsr #(\offset_right)		@ align the first part of the data
	ldm    	r1!, {r4-r10,ip}			@ these loads will be from aligned addresses, but the data is still not aligned
	orr	r3, r3, r4, lsl #(\offset_left)		@ align and combine the next part of the data
	mov	r4, r4, lsr #(\offset_right)		@ align the first part of the data
	orr	r4, r4, r5, lsl #(\offset_left)		@ align and combine the next part of the data
	mov	r5, r5, lsr #(\offset_right)
	orr	r5, r5, r6, lsl #(\offset_left)
	mov	r6, r6, lsr #(\offset_right)
	orr	r6, r6, r7, lsl #(\offset_left)
	mov	r7, r7, lsr #(\offset_right)
	orr	r7, r7, r8, lsl #(\offset_left)
	mov	r8, r8, lsr #(\offset_right)
	orr	r8, r8, r9, lsl #(\offset_left)
	mov	r9, r9, lsr #(\offset_right)
	orr	r9, r9, r10, lsl #(\offset_left)
	mov	r10, r10, lsr #(\offset_right)
	orr	r10, r10, ip, lsl #(\offset_left)
	stm	r0!, {r3-r10}
	pld     [r1, #0xE0]
	bge    	3b 
        add     r2, r2, #32      

  /* copy remaining bytes which are less than 32 */
4:
	and    	r3, r2, #28				@ bytes remaining that are multiples of 4
	mov    	ip, ip, lsr #(\offset_right)	
	sub 	r2, r2, r3				@ reduce the remaining byte count
        rsb 	r3, r3, #32				@ offset we'll use for our jump table

        adr 	r11, 5f
        add 	pc, r11, r3, lsl #2
5:	
        ldr 	r4, [r1], #4
        orr 	r11, ip, r4, lsl #(\offset_left)
        mov 	ip, r4, lsr #(\offset_right)
        str 	r11,[r0], #4

        ldr 	r4, [r1], #4
        orr 	r11, ip, r4, lsl #(\offset_left)
        mov 	ip, r4, lsr #(\offset_right)
        str 	r11,[r0], #4

        ldr 	r4, [r1], #4
        orr 	r11, ip, r4, lsl #(\offset_left)
        mov 	ip, r4, lsr #(\offset_right)
        str 	r11,[r0], #4

        ldr 	r4, [r1], #4
        orr 	r11, ip, r4, lsl #(\offset_left)
        mov 	ip, r4, lsr #(\offset_right)
        str 	r11,[r0], #4

        ldr 	r4, [r1], #4
        orr 	r11, ip, r4, lsl #(\offset_left)
        mov 	ip, r4, lsr #(\offset_right)
        str 	r11,[r0], #4

        ldr 	r4, [r1], #4
        orr 	r11, ip, r4, lsl #(\offset_left)
        mov 	ip, r4, lsr #(\offset_right)
        str 	r11,[r0], #4

        ldr 	r4, [r1], #4
        orr 	r11, ip, r4, lsl #(\offset_left)
        mov 	ip, r4, lsr #(\offset_right)
        str 	r11,[r0], #4

        ldr 	r4, [r1], #4
        orr 	r11, ip, r4, lsl #(\offset_left)
        mov 	ip, r4, lsr #(\offset_right)
        str 	r11,[r0], #4

        mov 	r4, #(\offset_left)	
        sub 	r1, r1, r4, lsr #3

  /* copy remaining bytes which are less than 4*/
        movs   	r7, r2, lsl #31
	ldrcsb 	r3, [r1], #1
	ldrcsb 	r4, [r1], #1
	strcsb 	r3, [r0], #1
	strcsb 	r4, [r0], #1
	ldrmib 	r3, [r1], #1
	strmib 	r3, [r0], #1
	
        ldmfd   sp!, {r0, r4-r12, lr}	 
        bx	lr
 .endm

 
 /* memcpy routine
  mrvl_memcpy(dst, src, size)*/

	.align
	.global mrvl_memcpy_v7
	.func   mrvl_memcpy_v7

mrvl_memcpy_v7:

	cmp     r2, #6
	blt     copy_less_than_6B
	stmfd	sp!, {r0, r4-r12,lr}
	pld     [r1, #0x0]
	/*is the destination address aligned on a 4 byte boundary?*/
	ands	r3, r0, #3
	pld     [r1, #0x20]
	bne	41f             
42:
	/*is the source address aligned on a 4 byte boundary?*/
	ands	r3, r1, #3
	pld     [r1, #0x40] 
	bne	43f

	subs    r2, r2, #32
	pld     [r1, #0x60]
	beq     51f
	bgt     52f

        /* less than 32 bytes left to copy*/
61:     
	tst	r2, #16
	beq	62f
	ldmia	r1!, {r3, r4, r5, r6}
	stmia	r0!, {r3, r4, r5, r6}

62:     tst     r2, #8
	beq	63f
	ldmia	r1!, {r3, r4}
	stmia	r0!, {r3, r4}

63:     tst     r2, #4
	beq	64f
	ldr	r3, [r1], #4
	str	r3, [r0], #4

64:
	tst     r2, #2
	beq     65f
        ldrh    r3, [r1], #2
	strh    r3, [r0], #2
65:
	tst     r2, #1
        beq	100f
        ldrb    r3, [r1], #1
        strb    r3, [r0], #1

100:	
	ldmfd	sp!, {r0, r4-r12, lr}	
	bx	lr

       /* copy 32 bytes */
51:
	ldmia   r1!, {r3 - r10}
        stmia   r0!, {r3 - r10}
	
	ldmfd   sp!, {r0, r4-r12, lr}
	bx      lr
	
52:
	pld    [r1, #0x80]
        cmp    r2, #16384 
        pld    [r1, #0xA0]
	addgt  r2, r2, #32
        bgt    copy_larger_than_16k
	pld    [r1, #0xC0]
 
  /* The size of data to be copied is less than 16k, no need to do cache clean op */
71:
	subs    r2, r2, #32
	ldmia   r1!, {r3 - r10}
        stmia   r0!, {r3 - r10}
        pld     [r1, #0xE0]
        bge     71b

	b 61b
	
copy_less_than_6B:
        stmfd  sp!, {r0, r4}

85:     subs  r2, r2, #3
	blt   86f
        ldrb  r3, [r1], #1
        ldrb  r4, [r1], #1
        ldrb  ip, [r1], #1
        strb  r3, [r0], #1
        strb  r4, [r0], #1
        strb  ip, [r0], #1
	b 85b
86:	
        adds  r2, r2, #2
	mov   ip, r0
        beq   87f
        blt   88f     
        ldrb  r3, [r1], #1
        strb  r3, [ip], #1
87:
        ldrb  r3, [r1], #1
        strb  r3, [ip], #1
88:
        ldmfd sp!, {r0, r4}
        bx    lr

  /* The size of data to be copied is larger than 16k, will do cache clean operation */	
copy_larger_than_16k:

        /* preload data ahead */
        pld  	[r1, #0xC0]
        mov   	lr, r0
	sub   	r2, r2, #16384
        mov   	r3, #256

93:     /* copy and preload 8k data */
        subs    r3, r3, #1
	ldmia   r1!, {r4 - r11}
	stmia   r0!, {r4 - r11}
	pld     [r1, #0xE0]
	bne     93b
	subs    r2, r2, #8192	
        /* drain MBU for data write */
        dmb                    	
        /* clean 8k data to memory */
        sub     r0, r0, #32
        mcrr    p6, 1, r0, lr, c6       
        add     r0, r0, #32
        dmb
        movge   lr, r0
        movge   r3, #256
        bge     93b
        add     r2, r2, #16384
        
  /* The size of data to be copied is less than 16k */	
        sub     r2, r2, #32       
        b       71b

41:    /*destination is not aligned.*/
	rsb	r3, r3, #4		       @ bytes to copy = 4 - (r0 & #3)
	sub     r2, r2, r3       
        movs    r4, r3, lsl #31
	ldrcsb  r4, [r1], #1
	ldrcsb  r5, [r1], #1
	strcsb  r4, [r0], #1
	strcsb  r5, [r0], #1
	ldrmib  r4, [r1], #1
	strmib  r4, [r0], #1                   @ destination is aligned now
        b 42b  

        /* destination is aligned and source address is not aligned here.*/
43:
	sub	r1, r1, r3                     @ get the source address word aligned
	/*how much will we have to shift the data by?*/
	cmp	r3, #1
	pld     [r1, #0x60]
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
	
        .endfunc
