/* Copyright (C) Marvell International Ltd. and/or its affiliates

   This software file (the ``File'') is owned and distributed by
   Marvell International Ltd. and/or its affiliates (``Marvell'')
   under the terms and conditions of the General Public License Version 2,
   June 1991 (the ``GPL License''), a copy of which is available along with
   the File in the license.txt file or by writing to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   or on the worldwide web at <http://www.gnu.org/licenses/gpl.txt>.

   THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND,
   AND THE IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR
   A PARTICULAR PURPOSE ARE EXPRESSLY DISCLAIMED.  The GPL License
   provides additional details about this warranty disclaimer. */

/* Copyright (C) 2006 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   Contributed by MontaVista Software, Inc. (written by Nicolas Pitre)

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* fastmem-arm9.S - memset/memcpy functions optimized for ARM926EJ-S
   and probably all other cores from ARM9 family.

   Copyright (C) 2006  Siarhei Siamashka <ssvb@users.sourceforge.net>

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.
*/

/**
 * void *arm9_memset(void *_dst, int _c, size_t count)
 *
 * Faster replacement for standard memset function.
 * @param _dst   - pointer to data buffer
 * @param _c     - filler 8-bit value
 * @param _count - number of filler 8-bit values to be saved into buffer
 */
        .align
#ifdef ARM9_NO_STD_REPLACEMENT1
        .global arm9_memset
       @ .func   arm9_memset
arm9_memset:
#else
        .global memset_feroceon
        @.func   memset_feroceon
memset_feroceon:
#endif
        mov     ip, r0

        cmp     r2, #4
        blt     3f
        /* Alignment is unknown */
        tst     ip, #1
        beq		100f
        strb  r1, [ip], #1
        sub   r2, r2, #1
100:
        /* Now we are 16-bit aligned (need to upgrade 'c' to 16-bit) */
        and     r1, r1, #255
        orr     r1, r1, r1, asl #8
        tst     ip, #2
        beq		101f
        strh  r1, [ip], #2
        sub   r2, r2, #2
101:
        /* Now we are 32-bit aligned (need to upgrade 'c' to 32-bit) */
        orr     r1, r1, r1, asl #16
        mov     r3, r1
        cmp     r2, #16
        blt     2f
        tst     ip, #4
        beq		102f
        str   r1, [ip], #4
        sub   r2, r2, #4
102:
        tst     ip, #8
        beq		103f
        stmia ip!, {r1, r3}
        sub   r2, r2, #8
103:
        /* Now we are 16-byte aligned */
        stmfd   sp!, {r4, r5}
        mov     r4, r1
        mov     r5, r1
1:      /* Copy 4 32-bit values twice per loop iteration */
        subs    r2, r2, #16
        blt		104f
        stmia ip!, {r1, r3, r4, r5}
        subs  r2, r2, #16
        blt		104f
        stmia ip!, {r1, r3, r4, r5}
        b     1b
104:
        add     r2, r2, #16
        ldmfd	sp!, {r4, r5}
2:      /* Copy up to 3 remaining 32-bit values */
        tst     r2, #8
        beq		105f
        stmia ip!, {r1, r3}
105:
        tst     r2, #4
        beq		106f
        str   r1, [ip], #4
106:
        and     r2, r2, #3
3:      /* Copy up to 3 remaining bytes */
        subs    r2, r2, #1
        strgeb  r1, [ip], #1
        subs    r2, r2, #1
        strgeb  r1, [ip], #1
        subs    r2, r2, #1
        strgeb  r1, [ip], #1

        bx      lr
        @.endfunc


/*
 * Data preload for architectures that support it (ARM V5TE and above)
 */
#if (!defined (__ARM_ARCH_2__) && !defined (__ARM_ARCH_3__) \
     && !defined (__ARM_ARCH_3M__) && !defined (__ARM_ARCH_4__) \
     && !defined (__ARM_ARCH_4T__) && !defined (__ARM_ARCH_5__) \
     && !defined (__ARM_ARCH_5T__))
#define PLD(code...) code
#define pld(code...) code
#else
define PLD(code...)
define pld(code...)
#endif

/**
 * Helper macro for memcpy function, it can copy data from source (r1) to
 * destination (r0) buffers fixing alignment in the process. Destination
 * buffer should be aligned already (4 bytes alignment is required.
 * Size of the block to copy is in r2 register
 */
.macro  UNALIGNED_MEMCPY shift
        sub     r1, #(\shift)
        ldr     ip, [r1], #4

        tst     r0, #4
        beq		100f
        mov   r3, ip, lsr #(\shift * 8)
        ldr   ip, [r1], #4
        sub   r2, r2, #4
        orr   r3, r3, ip, asl #(32 - \shift * 8)
        str   r3, [r0], #4
100:
        tst     r0, #8
        beq		101f
        mov   r3, ip, lsr #(\shift * 8)
        ldmia r1!, {r4, ip}
        sub   r2, r2, #8
        orr   r3, r3, r4, asl #(32 - \shift * 8)
        mov   r4, r4, lsr #(\shift * 8)
        orr   r4, r4, ip, asl #(32 - \shift * 8)
        stmia r0!, {r3-r4}
101:
        cmp     r2, #32
        blt     2f
        pld     [r1, #48]
1:
        pld     [r1, #80]
        subs    r2, r2, #32
        blt		2f
        mov   r3, ip, lsr #(\shift * 8)
        ldmia r1!, {r4 - r10, ip}
        orr   r3, r3, r4, asl #(32 - \shift * 8)
        mov   r4, r4, lsr #(\shift * 8)
        orr   r4, r4, r5, asl #(32 - \shift * 8)
        mov   r5, r5, lsr #(\shift * 8)
        orr   r5, r5, r6, asl #(32 - \shift * 8)
        mov   r6, r6, lsr #(\shift * 8)
        orr   r6, r6, r7, asl #(32 - \shift * 8)
        mov   r7, r7, lsr #(\shift * 8)
        orr   r7, r7, r8, asl #(32 - \shift * 8)
        mov   r8, r8, lsr #(\shift * 8)
        orr   r8, r8, r9, asl #(32 - \shift * 8)
        mov   r9, r9, lsr #(\shift * 8)
        orr   r9, r9, r10, asl #(32 - \shift * 8)
        mov   r10, r10, lsr #(\shift * 8)
        orr   r10, r10, ip, asl #(32 - \shift * 8)
        stmia r0!, {r3 - r10}
        bgt     1b

2:      /* copy remaining data */
		pld [sp, #0]

        tst     r2, #16
		beq		103f
        mov   r3, ip, lsr #(\shift * 8)
        ldmia r1!, {r4-r6, ip}
        orr   r3, r3, r4, asl #(32 - \shift * 8)
        mov   r4, r4, lsr #(\shift * 8)
        orr   r4, r4, r5, asl #(32 - \shift * 8)
        mov   r5, r5, lsr #(\shift * 8)
        orr   r5, r5, r6, asl #(32 - \shift * 8)
        mov   r6, r6, lsr #(\shift * 8)
        orr   r6, r6, ip, asl #(32 - \shift * 8)
        stmia r0!, {r3 - r6}
103:

        tst     r2, #8
        beq		105f
        mov   r3, ip, lsr #(\shift * 8)
        ldmia r1!, {r4, ip}
        orr   r3, r3, r4, asl #(32 - \shift * 8)
        mov   r4, r4, lsr #(\shift * 8)
        orr   r4, r4, ip, asl #(32 - \shift * 8)
        stmia r0!, {r3-r4}
105:

        tst     r2, #4
        beq		106f
        mov   r3, ip, lsr #(\shift * 8)
        ldr   ip, [r1], #4
        orr   r3, r3, ip, asl #(32 - \shift * 8)
        str   r3, [r0], #4
106:
        sub     r1, r1, #(4 - \shift)

        tst     r2, #2
		beq		107f
        ldrb  r3, [r1], #1
        ldrb  r4, [r1], #1
        strb  r3, [r0], #1
        strb  r4, [r0], #1
107:

        tst     r2, #1
        beq		108f
        ldrb  r3, [r1], #1
        strb  r3, [r0], #1
108:

        ldmfd	sp!, {r0, r4, r5, r6, r7, r8, r9, r10}

        bx      lr
.endm



/**
 * void *arm9_memcpy(void *_dst, void *_src, size_t _count)
 *
 * Function that is similar to standard memcpy function
 * @param _dst   - pointer to destination buffer
 * @param _src   - pointer to source buffer
 * @param _count - number of bytes to copy
 */
        .align
#ifdef ARM9_NO_STD_REPLACEMENT1
        .global arm9_memcpy
        @.func   arm9_memcpy
arm9_memcpy:
#else
        .global memcpy_feroceon
        @.func   memcpy_feroceon
memcpy_feroceon:
#endif
        cmp     r2, #20
        blt     memcpy_small_block

        stmfd	sp!, {r0, r4, r5, r6, r7, r8, r9, r10}

		ands	ip, r0, #3
	PLD(	pld	[r1, #0]		)
		bne	9f
		ands	ip, r1, #3
		bne	10f

memcpy_dst_4bytes_aligned_src0:
        /* both source and destination are 4 bytes aligned */
1:		subs	r2, r2, #(32)
		blt	5f

	PLD(	pld	[r1, #28]		)
	PLD(	subs	r2, r2, #64		)
	PLD(	blt	3f			)
2:	PLD(	pld	[r1, #60]		)
	PLD(	pld	[r1, #92]		)
		ldmia	r1!, {r3 - r10}
		subs	r2, r2, #32
		blt		100f
		stmia	r0!, {r3 - r10}
		ldmia	r1!, {r3 - r10}
		subs	r2, r2, #32
100:
		pld [sp, #0]

		stmia	r0!, {r3 - r10}
		bge	2b
3:	PLD(	ldmia	r1!, {r3 - r10}	)
	PLD(	adds	r2, r2, #32		)
	PLD(	blt		101f		)
	PLD(	stmia	r0!, {r3 - r10}	)
	PLD(	ldmia	r1!, {r3 - r10}	)
101:PLD(	stmia	r0!, {r3 - r10}	)
		beq		108f

5:		tst		r2, #16
		beq		6f
		ldmia	r1!, {r3, r4, r5, r6}
		stmia	r0!, {r3, r4, r5, r6}

6:		tst		r2, #8
		beq		7f
		ldmia	r1!, {r3, r4}
		stmia	r0!, {r3, r4}

7:		tst		r2, #4
		beq		8f
		ldr		r3, [r1], #4
		str		r3, [r0], #4

8:      tst     r2, #2
		beq		107f
        ldrb  r3, [r1], #1
        ldrb  r4, [r1], #1
        strb  r3, [r0], #1
        strb  r4, [r0], #1
107:

        tst     r2, #1
        beq		108f
        ldrb  r3, [r1], #1
        strb  r3, [r0], #1

108:
		ldmfd	sp!, {r0, r4, r5, r6, r7, r8, r9, r10}
		bx lr

        /* copy data until destination address is 4 bytes aligned */
9:		tst     r0, #1
		beq		100f
        ldrb  r3, [r1], #1
        sub   r2, r2, #1
        strb  r3, [r0], #1
100:
		tst     r0, #2
		beq		101f
        ldrb  r3, [r1], #1
        ldrb  r4, [r1], #1
        sub   r2, r2, #2
        orr   r3, r3, r4, asl #8
        strh  r3, [r0], #2
101:
		ands	ip, r1, #3
		beq		memcpy_dst_4bytes_aligned_src0

        /* destination address is 4 bytes aligned */
        /* now we should handle 4 cases of source address alignment */
10:		tst     r1, #1
        bne     memcpy_dst_4bytes_aligned_src1or3

memcpy_dst_4bytes_aligned_src2:
        UNALIGNED_MEMCPY 2

memcpy_dst_4bytes_aligned_src1or3:
        tst    r1, #2
        bne    memcpy_dst_4bytes_aligned_src3
memcpy_dst_4bytes_aligned_src1:
        UNALIGNED_MEMCPY 1

memcpy_dst_4bytes_aligned_src3:
        UNALIGNED_MEMCPY 3

memcpy_small_block:
        stmfd  sp!, {r0, r4}

1:      subs   r2, r2, #3
		blt	   100f
        ldrb ip, [r0]
        ldrb r3, [r1], #1
        ldrb r4, [r1], #1
        ldrb ip, [r1], #1
        strb r3, [r0], #1
        strb r4, [r0], #1
        strb ip, [r0], #1
        b    1b

100:
        adds   r2, r2, #2
        mov    ip, r0
        beq    101f
        blt    102f

        ldrb r3, [r1], #1
        strb r3, [ip], #1
101:
        ldrb r3, [r1], #1
        strb r3, [ip], #1
102:

		ldmfd sp!, {r0, r4}
        bx     lr

        .end
