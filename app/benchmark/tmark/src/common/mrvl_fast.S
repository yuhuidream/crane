/* Copyright (C) Marvell International Ltd. and/or its affiliates */

/**
 * Helper macro for memcpy function, it can copy data from source (r1) to
 * destination (r0) buffers fixing alignment in the process. Destination
 * buffer should be aligned already (4 bytes alignment is required.
 * Size of the block to copy is in r2 register
 */
.macro  UNALIGNED_MEMCPY shift
        sub	r1, #(\shift)
        ldr	ip, [r1], #4
        tst	r0, #4
        beq	100f
        mov	r3, ip, lsr #(\shift * 8)
        ldr	ip, [r1], #4
        sub	r2, r2, #4
        orr	r3, r3, ip, asl #(32 - \shift * 8)
        str	r3, [r0], #4
100:
        tst	r0, #8
        beq	101f
        mov	r3, ip, lsr #(\shift * 8)
        ldmia	r1!, {r4, ip}
        sub	r2, r2, #8
        orr	r3, r3, r4, asl #(32 - \shift * 8)
        mov	r4, r4, lsr #(\shift * 8)
        orr	r4, r4, ip, asl #(32 - \shift * 8)
        stmia	r0!, {r3-r4}
101:
        cmp	r2, #32
        blt	2f
        pld	[r1, #48]
1:
        pld	[r1, #80]
        subs	r2, r2, #32
        blt	2f
        mov	r3, ip, lsr #(\shift * 8)
        ldmia	r1!, {r4 - r10, ip}
        orr	r3, r3, r4, asl #(32 - \shift * 8)
        mov	r4, r4, lsr #(\shift * 8)
        orr	r4, r4, r5, asl #(32 - \shift * 8)
        mov	r5, r5, lsr #(\shift * 8)
        orr	r5, r5, r6, asl #(32 - \shift * 8)
        mov	r6, r6, lsr #(\shift * 8)
        orr	r6, r6, r7, asl #(32 - \shift * 8)
        mov	r7, r7, lsr #(\shift * 8)
        orr	r7, r7, r8, asl #(32 - \shift * 8)
        mov	r8, r8, lsr #(\shift * 8)
        orr	r8, r8, r9, asl #(32 - \shift * 8)
        mov	r9, r9, lsr #(\shift * 8)
        orr	r9, r9, r10, asl #(32 - \shift * 8)
        mov	r10, r10, lsr #(\shift * 8)
        orr	r10, r10, ip, asl #(32 - \shift * 8)
        stmia	r0!, {r3 - r10}
        bgt	1b

2:      /* copy remaining data */
	pld	[sp, #0]
        tst	r2, #16
	beq	103f
        mov	r3, ip, lsr #(\shift * 8)
        ldmia	r1!, {r4-r6, ip}
        orr	r3, r3, r4, asl #(32 - \shift * 8)
        mov	r4, r4, lsr #(\shift * 8)
        orr	r4, r4, r5, asl #(32 - \shift * 8)
        mov	r5, r5, lsr #(\shift * 8)
        orr	r5, r5, r6, asl #(32 - \shift * 8)
        mov	r6, r6, lsr #(\shift * 8)
        orr	r6, r6, ip, asl #(32 - \shift * 8)
        stmia	r0!, {r3 - r6}
103:
	tst	r2, #8
	beq	105f
	mov	r3, ip, lsr #(\shift * 8)
	ldmia	r1!, {r4, ip}
	orr	r3, r3, r4, asl #(32 - \shift * 8)
	mov	r4, r4, lsr #(\shift * 8)
	orr	r4, r4, ip, asl #(32 - \shift * 8)
	stmia	r0!, {r3-r4}
105:
	tst	r2, #4
	beq	106f
	mov	r3, ip, lsr #(\shift * 8)
	ldr	ip, [r1], #4
	orr	r3, r3, ip, asl #(32 - \shift * 8)
	str	r3, [r0], #4
106:
	sub     r1, r1, #(4 - \shift)
	tst     r2, #2
	beq	107f
	ldrb	r3, [r1], #1
	ldrb	r4, [r1], #1
	strb	r3, [r0], #1
	strb	r4, [r0], #1
107:
	tst	r2, #1
	beq	108f
	ldrb	r3, [r1], #1
	strb	r3, [r0], #1
108:
	ldmfd	sp!, {r0, r4 - r11, lr}
	bx	lr
.endm

/**
 * void *memcpy_fast(void *_dst, void *_src, size_t _count)
 *
 * Function that is similar to standard memcpy function
 * @param _dst   - pointer to destination buffer
 * @param _src   - pointer to source buffer
 * @param _count - number of bytes to copy
 *
 * Because we align memcpy to 2^5, let it stand alone to
 * avoid the native .text alignment.
 */


       @ Export Functions
       .global memcpy_fast
       .text
       .align 5
       .type	memcpy_fast, %function
memcpy_fast:
	cmp	r2, #20
	blt	.Lmemcpy_small_block
	stmfd	sp!, {r0, r4 - r11, lr}
	ands	ip, r0, #3
	pld	[r1, #0]
	bne	9f
	ands	ip, r1, #3
	bne	10f

.Lmemcpy_dst_4bytes_aligned_src0:
        /* both source and destination are 4 bytes aligned */
1:
	subs	r2, r2, #(32)
	blt	5f
	mov	r4, #0xff
	add	r2, r2, #32
	and	r4, r2, r4
	cmp	r4, #0
	beq	.Lmemcpy_256B_aligned
	sub	r2, r2, #(32)
	pld	[r1, #28]
	subs	r2, r2, #64
	blt	3f
2:
	pld	[r1, #60]
	pld	[r1, #92]
	ldmia	r1!, {r3 - r10}
	subs	r2, r2, #32
	blt	100f
	stmia	r0!, {r3 - r10}
	ldmia	r1!, {r3 - r10}
	subs	r2, r2, #32
100:
	pld	[sp, #0]
	stmia	r0!, {r3 - r10}
	bge	2b
3:
	ldmia	r1!, {r3 - r10}
	adds	r2, r2, #32
	blt	101f
	stmia	r0!, {r3 - r10}
	ldmia	r1!, {r3 - r10}
101:
	stmia	r0!, {r3 - r10}
	beq	108f
5:
	tst	r2, #16
	beq	6f
	ldmia	r1!, {r3, r4, r5, r6}
	stmia	r0!, {r3, r4, r5, r6}
6:
	tst	r2, #8
	beq	7f
	ldmia	r1!, {r3, r4}
	stmia	r0!, {r3, r4}
7:
	tst	r2, #4
	beq	8f
	ldr	r3, [r1], #4
	str	r3, [r0], #4
8:
	tst	r2, #2
	beq	107f
        ldrb	r3, [r1], #1
        ldrb	r4, [r1], #1
        strb	r3, [r0], #1
        strb	r4, [r0], #1
107:
	tst     r2, #1
	beq	108f
	ldrb	r3, [r1], #1
	strb	r3, [r0], #1
108:
	ldmfd	sp!, {r0, r4 - r11, lr}
	bx lr

        /* copy data until destination address is 4 bytes aligned */
9:	tst	r0, #1
	beq	100f
        ldrb	r3, [r1], #1
        sub	r2, r2, #1
        strb	r3, [r0], #1
100:
	tst	r0, #2
	beq	101f
	ldrb	r3, [r1], #1
	ldrb	r4, [r1], #1
	sub	r2, r2, #2
	orr	r3, r3, r4, asl #8
	strh	r3, [r0], #2
101:
	ands	ip, r1, #3
	beq	.Lmemcpy_dst_4bytes_aligned_src0

        /* destination address is 4 bytes aligned */
        /* now we should handle 4 cases of source address alignment */
10:	tst	r1, #1
	bne	.Lmemcpy_dst_4bytes_aligned_src1or3

.Lmemcpy_dst_4bytes_aligned_src2:
	UNALIGNED_MEMCPY 2

.Lmemcpy_dst_4bytes_aligned_src1or3:
	tst	r1, #2
	bne	.Lmemcpy_dst_4bytes_aligned_src3

.Lmemcpy_dst_4bytes_aligned_src1:
	UNALIGNED_MEMCPY 1

.Lmemcpy_dst_4bytes_aligned_src3:
	UNALIGNED_MEMCPY 3

.Lmemcpy_small_block:
	stmfd	sp!, {r0, r4}
1:
	subs	r2, r2, #3
	blt	100f
	ldrb	ip, [r0]
	ldrb	r3, [r1], #1
	ldrb	r4, [r1], #1
	ldrb	ip, [r1], #1
	strb	r3, [r0], #1
	strb	r4, [r0], #1
	strb	ip, [r0], #1
	b	1b
100:
	adds	r2, r2, #2
	mov	ip, r0
	beq	101f
	blt	102f
	ldrb	r3, [r1], #1
	strb	r3, [ip], #1
101:
	ldrb	r3, [r1], #1
	strb	r3, [ip], #1
102:
	ldmfd	sp!, {r0, r4}
	bx	lr

.Lmemcpy_256B_aligned:
	pld	[r1, #0]
	pld	[r1, #0x20]
	pld	[r1, #0x40]
	pld	[r1, #0x60]
	pld	[r1, #0x80]
	pld	[r1, #0xA0]
	pld	[r1, #0xC0]
	pld	[r1, #0xE0]
.LLoop:
	pld	[r1, #0x100]
	pld	[r1, #0x120]
	pld	[r1, #0x140]
	pld	[r1, #0x160]
	ldm	r1!, {r4-r11}
	stm	r0!, {r4-r11}
	ldm	r1!, {r4-r11}
	stm	r0!, {r4-r11}
	ldm	r1!, {r4-r11}
	stm	r0!, {r4-r11}
	ldm	r1!, {r4-r11}
	stm	r0!, {r4-r11}
	pld	[r1, #0x100]
	pld	[r1, #0x120]
	pld	[r1, #0x140]
	pld	[r1, #0x160]
	pld	[r1, #0x180]
	pld	[r1, #0x200]
	ldm	r1!, {r4-r11}
	stm	r0!, {r4-r11}
	ldm	r1!, {r4-r11}
	stm	r0!, {r4-r11}
	ldm	r1!, {r4-r11}
	stm	r0!, {r4-r11}
	ldm	r1!, {r4-r11}
	stm	r0!, {r4-r11}
	subs	r2, r2, #256
	bne	.LLoop
	ldmfd	sp!, {r0, r4 - r11, lr}
	bx	lr
	.size   memcpy_fast, . - memcpy_fast
