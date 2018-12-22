/*
 * Copyright (c) 2013 Marvell International Ltd.
 *
 * Copyright (c) 2013 ARM Ltd
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the company may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ARM LTD ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ARM LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <machine/cpu-features.h>
#include <machine/asm.h>

        .text
        .fpu    neon

#define CACHE_LINE_SIZE     64

ENTRY(memcpy_a15_new)

       /* Assumes that n >= 0, and dst, src are valid pointers.  Align dest
        * buffer to 32 bytes and use Neon instructions, When less than 64
        * bytes, copy 2 words a time until less than 8 bytes left, then copy a
        * word and then byte by byte. Use ARM instead of Neon to handling
        * trailing bytes for better performance in small size memcpy()
        */

        .save   {r0, lr}
        stmfd   sp!, {r0, lr}

        /* Get copying of tiny blocks out of the way first.  */
        /* Is there at least 4 bytes to copy?  */
        subs    r2, r2, #4
        blo     copy_less_than_4                 /* If n < 4.  */

        /* Check word alignment.  */
        ands    ip, r0, #3                       /* ip = last 2 bits of dst.  */
        bne     dst_not_word_aligned             /* If dst is not word-aligned.  */

dst_word_aligned:
        /* Get here if dst buffer is word-aligned.
           The number of bytes remaining to copy is r2+4.  */

        /* preload immediately the next cache line, which we may need */
        pld         [r1, #(CACHE_LINE_SIZE*0)]
        pld         [r1, #(CACHE_LINE_SIZE*1)]

        /* Is there is at least 64 bytes to copy?  */
        subs    r2, r2, #60
        blo     copy_less_than_64                /* If r2 + 4 < 64.  */

        /* First, align the destination buffer to 32-bytes, to make sure loads
         * and stores 32 bytes don't cross cache line boundary, as they are
         * then more expensive even if the data is in the cache (require two
         * load/store issue cycles instead of one).  If only one of the buffers
         * is not 32-bytes aligned, then it's more important to align dst than
         * src, because there is more penalty for stores than loads that cross
         * cacheline boundary.  This check and realignment are only worth doing
         * if there is a lot to copy.  */

        /* Get here if dst is word aligned,
           i.e., the 2 least significant bits are 0.
           If dst is not 2w aligned (i.e., the 3rd bit is set in dst),
           then copy 1 word (4 bytes).  */
        ands    r3, r0, #4
        beq     dword_aligned
        vld4.8  {d0[0], d1[0], d2[0], d3[0]}, [r1]!
        vst4.8  {d0[0], d1[0], d2[0], d3[0]}, [r0, :32]!
        subs    r2, r2, #4
        blo     copy_less_than_64

dword_aligned:
        /* Get here if dst is 2 words aligned,
           i.e., the 3 least significant bits are 0.
           If dst is not 4w aligned (i.e., the 4th bit is set in dst),
           then copy 2 words (8 bytes).  */
        ands    r3, r0, #8
        beq     qword_aligned
        vld1.8  {d0}, [r1]!
        vst1.8  {d0}, [r0, :64]!
        subs    r2, r2, #8
        blo     copy_less_than_64

qword_aligned:
        /* Get here if dst is 4 words aligned,
           i.e., the 4 least significant bits are 0.
           If dst is not 8w aligned (i.e., the 5th bit is set in dst),
           then copy 4 words (16 bytes).  */
        ands    r3, r0, #16
        beq     eight_word_aligned
        vld1.8  {d0, d1}, [r1]!
        vst1.8  {d0, d1}, [r0, :128]!
        subs    r2, r2, #16
        blo     copy_less_than_64

eight_word_aligned:
        /* Preload all the cache lines we need.
         * NOTE: The number of pld below depends on CACHE_LINE_SIZE,
         * ideally we would increase the distance in the main loop to
         * avoid the goofy code below. In practice this doesn't seem to make
         * a big difference.
         * NOTE: The value CACHE_LINE_SIZE * 4 was chosen through
         * experimentation.
         */
        pld     [r1, #(CACHE_LINE_SIZE*2)]
        pld     [r1, #(CACHE_LINE_SIZE*3)]
        pld     [r1, #(CACHE_LINE_SIZE*4)]

1:      /* The main loop copies 64 bytes at a time */
        vld1.8  {d0  - d3},   [r1]!
        vld1.8  {d4  - d7},   [r1]!
        pld     [r1, #(CACHE_LINE_SIZE*4)]
        subs    r2, r2, #64
        vst1.8  {d0  - d3},   [r0, :128]!
        vst1.8  {d4  - d7},   [r0, :128]!
        bhs     1b

copy_less_than_64:
        /* Get here if less than 64 bytes to copy, -64 <= r2 < 0.
         * Restore the count if there is more than 7 bytes to copy.
         *
         * We use ARM instructions instead of Neon to have better performance
         * for small size memcpy(), only if both src and dest buffer are
         * word-aligned here */
        ands    r3, r1, #3                       /* r3 = last 2 bits of src.  */
        bne     src_not_word_aligned

        /* Temporarily saves r2 to ip register for ldrd instruction */
        adds    ip, r2, #56
        bmi     copy_less_than_8

        /* Copy 8 bytes at a time.  */
2:
        ldrd    r2, r3, [r1], #8
        strd    r2, r3, [r0], #8
        subs    ip, ip, #8
        bhs     2b                            /* If there is more to copy.  */

copy_less_than_8:
        mov     r2, ip                        /* Restore r2 register as size to copy */
        /* Get here if less than 8 bytes to copy, -8 <= r2 < 0.
           Check if there is more to copy.  */
        cmn     r2, #8
        beq     return                        /* If r2 + 8 == 0.  */

        /* Restore the count if there is more than 3 bytes to copy.  */
        adds    r2, r2, #4
        bmi     copy_less_than_4

        /* Copy 4 bytes.  */
        ldr     r3, [r1], #4
        str     r3, [r0], #4

copy_less_than_4:
        /* Get here if less than 4 bytes to copy, -4 <= r2 < 0.  */

        /* Restore the count, check if there is more to copy.  */
        adds    r2, r2, #4
        beq     return                          /* If r2 == 0.  */

        /* Get here with r2 is in {1,2,3}={01,10,11}.  */
        /* Logical shift left r2, insert 0s, update flags.  */
        lsls    r2, r2, #31

        /* Copy byte by byte.
           Condition ne means the last bit of r2 is 0.
           Condition cs means the second to last bit of r2 is set,
           i.e., r2 is 1 or 3.  */
        itt     ne
        ldrneb  r3, [r1], #1
        strneb  r3, [r0], #1

        itttt   cs
        ldrcsb  r3, [r1], #1
        ldrcsb  ip, [r1]
        strcsb  r3, [r0], #1
        strcsb  ip, [r0]

return:
        /* This is the only return point of memcpy.  */
        ldmfd   sp!, {r0, lr}
        bx      lr

dst_not_word_aligned:

       /* Get here when dst is not aligned and ip has the last 2 bits of dst,
          i.e., ip is the offset of dst from word.  The number of bytes that
          remains to copy is r2 + 4, i.e., there are at least 4 bytes to copy.
          Write a partial word (0 to 3 bytes), such that dst becomes
          word-aligned.  */

       /* If dst is at ip bytes offset from a word (with 0 < ip < 4),
          then there are (4 - ip) bytes to fill up to align dst to the next
          word.  */
        rsb     ip, ip, #4                        /* ip = #4 - ip.  */
        cmp     ip, #2

       /* Copy byte by byte with conditionals.  */
        itt     hi
        ldrhib  r3, [r1], #1
        strhib  r3, [r0], #1

        itt     hs
        ldrhsb  r3, [r1], #1
        strhsb  r3, [r0], #1

        ldrb    r3, [r1], #1
        strb    r3, [r0], #1

       /* Update the count.
          ip holds the number of bytes we have just copied.  */
        subs    r2, r2, ip                        /* r2 = r2 - ip.  */
        blo     copy_less_than_4                  /* If r2 < ip.  */

        b       dst_word_aligned

src_not_word_aligned:
        /* Get here if less than 64 bytes to copy, -64 <= r2 < 0.
           Restore the count if there is more than 7 bytes to copy.  */
        adds    r2, r2, #56
        bmi     copy_less_than_8_neon

        /* Copy 8 bytes at a time.  */
2:
        vld1.8  {d0}, [r1]!
        vst1.8  {d0}, [r0]!
        subs    r2, r2, #8
        bhs     2b                            /* If there is more to copy.  */

copy_less_than_8_neon:
        /* Get here if less than 8 bytes to copy, -8 <= r2 < 0.
           Check if there is more to copy.  */
        cmn     r2, #8
        beq     return                        /* If r2 + 8 == 0.  */

        /* Restore the count if there is more than 3 bytes to copy.  */
        adds    r2, r2, #4
        bmi     copy_less_than_4

        /* Copy 4 bytes.  */
        vld4.8  {d0[0], d1[0], d2[0], d3[0]}, [r1]!
        vst4.8  {d0[0], d1[0], d2[0], d3[0]}, [r0]!

        b       copy_less_than_4

END(memcpy_a15_new)
