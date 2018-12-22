/*

	Marvell Optimized Memcopy Routine
	
	Authors:	Marlon A. Moncrieffe
				mamoncri@marvell.com
				
				Huaping Wang
				huaping@marvell.com

	Copyright (C) Marvell International Ltd. and/or its affiliates
*/

#define PLD_CACHELINE_NUM	128 		/* 4KB */
#define L2C_SIZE                262144

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
	sub             r2, r2, #32
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
	cmp             r2, #32

	/* loop again if we have more than 32 remaining */
	bge    	1b 

        /* copy remaining bytes which are less than 32 */
3:
	and r3, r2, #28				@ bytes remaining that are multiples of 4
	mov ip, ip, lsr #(\offset_right)	
	sub r2, r2, r3				@ reduce the remaining byte count
        rsb r3, r3, #32				@ offset we'll use for our jump table

        adr r11, 4f
        add pc, r11, r3, lsl #2

4:	
        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        mov ip, r4, lsr #(\offset_right)
        str r11,[r0], #4

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        mov ip, r4, lsr #(\offset_right)
        str r11,[r0], #4

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        mov ip, r4, lsr #(\offset_right)
        str r11,[r0], #4

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        mov ip, r4, lsr #(\offset_right)
        str r11,[r0], #4

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        mov ip, r4, lsr #(\offset_right)
        str r11,[r0], #4

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        mov ip, r4, lsr #(\offset_right)
        str r11,[r0], #4

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        mov ip, r4, lsr #(\offset_right)
        str r11,[r0], #4

        ldr r4, [r1], #4
        orr r11, ip, r4, lsl #(\offset_left)
        mov ip, r4, lsr #(\offset_right)
        str r11,[r0], #4

        mov r4, #(\offset_left)	
        sub r1, r1, r4, lsr #3

        /* copy left bytes which is less than 4*/
        movs   r7, r2, lsl #31
	ldrcsb r3, [r1], #1
	ldrcsb r4, [r1], #1
	strcsb r3, [r0], #1
	strcsb r4, [r0], #1
	ldrmib r3, [r1], #1
	strmib r3, [r0], #1
	
        ldmfd   sp!, {r0, r4-r12}	 
        bx	    lr
 .endm

 
 /* memcpy routine
  mrvl_memcpy(dst, src, size)*/

	.align
	.global mrvl_memcpy4
	.func   mrvl_memcpy4

mrvl_memcpy4:

	cmp     r2, #6
	blt     copy_small_block

        stmfd	sp!, {r0, r4-r12}

	/*is the destination address aligned on a 4 byte boundary?*/
	ands	r3, r0, #3
	pld     [r1, #0]
	bne	41f              
42:
	/*is the source address aligned on a 4 byte boundary?*/
	ands	r3, r1, #3
	bne	43f

	subs    r2, r2, #32
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
	ldmfd	sp!, {r0, r4-r12}	
	bx	lr

51:
	ldmia   r1!, {r3 - r10}
        stmia   r0!, {r3 - r10}
	
	ldmfd   sp!, {r0, r4-r12}
	bx      lr
	
52:
	pld    [r1, #32]
        cmp    r2, #L2C_SIZE
        bgt    copy_large_block

71:
	pld     [r1, #64]
	subs    r2, r2, #32
	ldmia   r1!, {r3 - r10}
        stmia   r0!, {r3 - r10}
        bge     71b

	b 61b
	
	
copy_small_block:
        stmfd  sp!, {r0, r4}

85:     subs  r2, r2, #3
	ldrb  ip, [r0]
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

	
copy_large_block:
      /* For large block copy. we preload 4k data first, then do the copy.
	 Preload and data copy are in two different loops*/
	add   r2, r2, #32
	mov   r12, r1
	mov   r3,  #0
	
      /* Preload  PLD_CACHLINE_NUM lines data into cache */
91:	
	cmp  r2, #128          
	bge  92f
	cmp  r3, #2
	bge  93f
	cmp  r2, #32
	pld  [r1, #0]
	beq  94f
	blt  61b   
	cmp  r2, #64
	pld  [r1, #0x20]
	bls  94f
	cmp  r2, #96
	pld  [r1, #0x40]
	bls  94f
        pld  [r1, #0x60]
	b    94f    
	
92:
	pld  [r1, #0]
	pld  [r1, #0x20]
	pld  [r1, #0x40]
	pld  [r1, #0x60]
	add   r3, r3, #4
	sub   r2, r2, #128
	cmp   r3, #PLD_CACHELINE_NUM
	add   r1, r1, #128
        blt   91b
	
      /* Finish data preloading and start to copy data here*/
93:
	ldmia   r12!, {r4 - r11}
	stmia   r0!,  {r4 - r11}
	subs    r3, r3, #2
        ldmia   r12!, {r4 - r11}
	stmia   r0!,  {r4 - r11} 
	bne     93b
	b       91b      

       /* Data remaining is less than 128 bytes. Do in this loop*/
94:
	ldmia   r1!, {r4 - r11}
	sub     r2, r2, #32
	stmia   r0!, {r4 - r11}
	cmp	r2, #32
	bge     94b

       /* Data remaining is less than 32 bytes.*/
        b       61b 


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
