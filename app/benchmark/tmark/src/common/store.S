    .title	Store Latency Test

    .global	single_store_4
    .global	single_store_4_min_1
    .global	combine_store_4
    .global	hop_store_4_1
    .global	hop_store_4_2
    .global	hop_store_4_3
    .global	hop_store_4_4
    .global	delay_store_4
    .global	pipe_str_4
    .global	pipe_strh_4
    .global	pipe_strb_4_1
    .global	pipe_strb_4_2
    .global	pipe_str_1_8_7
    .global	pipe_str_9_7
    .global	pipe_strd_8
    .global	pipe_strd_8_postincr
    .global     coalesce_store_4_stm_2
    .global     coalesce_store_4_stm_4
    .global     coalesce_store_4_stm_8
    .global     coalesce_store_4_stm_7_skip_1

    .text
    .align 5

single_store_4:

    STMFD   sp!, {r4-r7}    @PUSH

Loop_single_store_4:
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2

    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2

    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2

    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2

    SUBS    r1, r1, #16

    BGT     Loop_single_store_4

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

single_store_4_min_1:

    STMFD   sp!, {r4-r7}    @PUSH

1:
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2

    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #6

    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2

    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #6

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

combine_store_4:

    STMFD   sp!, {r4-r7}    @PUSH

1:
	@@ 1-1-2-4
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2
    stmia   r0!, {r2-r3}
    stmia   r0!, {r2-r5}

	@@ 4-2-1-1
    stmia   r0!, {r2-r5}
    stmia   r0!, {r2-r3}
    STR     r4, [r0],#2
    add     r0, r0, #2
    STR     r4, [r0],#2
    add     r0, r0, #2

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

hop_store_4_1:

    STMFD   sp!, {r4-r7}    @PUSH

1:
	@@ 0-2-4-6, 1-3-5-7
    STR     r4, [r0],#2		@ word#0
    add     r0, r0, #6
    STR     r4, [r0],#2		@ word#2
    add     r0, r0, #6
    STR     r4, [r0],#2		@ word#4
    add     r0, r0, #6
    STR     r4, [r0],#2		@ word#6
    sub     r0, r0, #22		@ 26-22=4 (word#1)

    STR     r4, [r0],#2		@ word#1
    add     r0, r0, #6
    STR     r4, [r0],#2		@ word#3
    add     r0, r0, #6
    STR     r4, [r0],#2		@ word#5
    add     r0, r0, #6
    STR     r4, [r0],#2		@ word#7
    add     r0, r0, #2

	/* 7-6-5-4, 1-0-3-2 */
    add     r0, r0, #28
    STR     r4, [r0],#2		@ word#7
    sub     r0, r0, #6
    STR     r4, [r0],#2		@ word#6
    sub     r0, r0, #6
    STR     r4, [r0],#2		@ word#5
    sub     r0, r0, #6
    STR     r4, [r0],#2		@ word#4
    sub     r0, r0, #14		@ 16+2-14=4(word#1)

    STR     r4, [r0],#2		@ word#1
    sub     r0, r0, #6
    STR     r4, [r0],#2		@ word#0
    add     r0, r0, #10
    STR     r4, [r0],#2		@ word#3
    sub     r0, r0, #6
    STR     r4, [r0],#2		@ word#2
    add     r0, r0, #22

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

hop_store_4_2:

    STMFD   sp!, {r4-r7}    @PUSH

1:
    add     r0, r0, #8
    stmia   r0!, {r2-r5}	@ 4
    stmia   r0!, {r2-r3}	@ 2

    stmia   r0!, {r2-r5}	@ 4
    stmia   r0!, {r2-r3}	@ 2
    stmia   r0!, {r2-r3}	@ 2
    sub     r0, r0, #64
    stmia   r0!, {r2-r3}	@ 2
    add     r0, r0, #56

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

hop_store_4_3:

    STMFD   sp!, {r4-r7}    @PUSH

1:
    stmia   r0!, {r2-r5}	@ 4
    ldr	    r6, [r0, #16]
    stmia   r0!, {r2-r5}	@ 4

    stmia   r0!, {r2-r5}	@ 4
    ldr	    r6, [r0, #16]
    stmia   r0!, {r2-r5}	@ 4

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

hop_store_4_4:

    STMFD   sp!, {r4-r7}    @PUSH

1:
    stmia   r0!, {r2-r5}	@ 4
    strd    r2, [r0, #16]
    stmia   r0!, {r2-r5}	@ 4

    add     r0, r0, #8		@ already written
    stmia   r0!, {r2-r3}	@ 2
    stmia   r0!, {r2-r5}	@ 4

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

delay_store_4:

    STMFD   sp!, {r4-r7}    @PUSH

1:
    mov     r3, r2
    stmia   r0!, {r2-r5}	@ 4
    stmia   r0!, {r2-r5}	@ 4

    stmia   r0!, {r2-r5}	@ 4
    stmia   r0!, {r2-r3}	@ 2
2:  subs    r3, r3, #1
    bne     2b
    stmia   r0!, {r2-r3}	@ 2

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

pipe_strd_8:
    STMFD	sp!, {r4-r9}	@PUSH

Loop_pipe_strd_8:

    STRD     r2, [r0], #8
    STRD     r4, [r0], #8
    STRD     r6, [r0], #8
    STRD     r8, [r0], #8

    STRD     r2, [r0], #8
    STRD     r4, [r0], #8
    STRD     r6, [r0], #8
    STRD     r8, [r0], #8

    STRD     r2, [r0], #8
    STRD     r4, [r0], #8
    STRD     r6, [r0], #8
    STRD     r8, [r0], #8

    STRD     r2, [r0], #8
    STRD     r4, [r0], #8
    STRD     r6, [r0], #8
    STRD     r8, [r0], #8

    SUBS     r1, r1, #16

    BGT      Loop_pipe_strd_8

    LDMFD    sp!, {r4-r9}	@POP
    BX	     lr

pipe_strd_8_postincr:
    STMFD       sp!, {r4-r9}    @PUSH

Loop_pipe_strd_8_postincr:

    STRD     r2, [r0]
    STRD     r4, [r0, #8]
    STRD     r6, [r0, #16]
    STRD     r8, [r0, #24]

    STRD     r2, [r0, #32]
    STRD     r4, [r0, #40]
    STRD     r6, [r0, #48]
    STRD     r8, [r0, #56]

    STRD     r2, [r0, #64]
    STRD     r4, [r0, #72]
    STRD     r6, [r0, #80]
    STRD     r8, [r0, #88]

    STRD     r2, [r0, #96]
    STRD     r4, [r0, #104]
    STRD     r6, [r0, #112]
    STRD     r8, [r0, #120]


    SUBS     r1, r1, #16
    ADD	     r0, r0, #128
    BGT      Loop_pipe_strd_8_postincr

    LDMFD    sp!, {r4-r9}       @POP
    BX       lr

pipe_str_4:
    STMFD   sp!, {r4-r7}    @PUSH

Loop_pipe_store_4:
    STR     r4, [r0], #4
    STR     r5, [r0], #4
    STR     r6, [r0], #4
    STR     r7, [r0], #4

    STR     r4, [r0], #4
    STR     r5, [r0], #4
    STR     r6, [r0], #4
    STR     r7, [r0], #4

    STR     r4, [r0], #4
    STR     r5, [r0], #4
    STR     r6, [r0], #4
    STR     r7, [r0], #4

    STR     r4, [r0], #4
    STR     r5, [r0], #4
    STR     r6, [r0], #4
    STR     r7, [r0], #4

    SUBS    r1, r1, #16

    BGT     Loop_pipe_store_4

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr


pipe_strh_4:
    STMFD   sp!, {r4-r7}    @PUSH

1:

    .rept 8
    STRH     r4, [r0], #2
    STRH     r5, [r0], #2
    STRH     r6, [r0], #2
    STRH     r7, [r0], #2
    .endr

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

pipe_strb_4_1:
    STMFD   sp!, {r4-r7}    @PUSH

1:
    .rept 16
    STRB     r4, [r0], #1
    STRB     r5, [r0], #1
    STRB     r6, [r0], #1
    STRB     r7, [r0], #1
    .endr

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

pipe_strb_4_2:
    STMFD   sp!, {r4-r7}    @PUSH

1:
    .rept 2
    stmia   r0!, {r2-r8}    @ 7 words

    STRB     r4, [r0], #1   @ 1 word
    STRB     r5, [r0], #1
    STRB     r6, [r0], #1
    STRB     r7, [r0], #1
    .endr

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

pipe_str_1_8_7:
    STMFD   sp!, {r4-r7}    @PUSH

1:
    STRB     r4, [r0], #1   @ 1 word
    STRB     r5, [r0], #1
    STRB     r6, [r0], #1
    STRB     r7, [r0], #1
    stmia   r0!, {r2-r9}    @ 8 words
    stmia   r0!, {r2-r8}    @ 7 words

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

pipe_str_9_7:
    STMFD   sp!, {r4-r7}    @PUSH

1:
    stmia   r0!, {r2-r10}   @ 9 words
    stmia   r0!, {r2-r8}    @ 7 words

    SUBS    r1, r1, #16

    BGT     1b

    LDMFD   sp!, {r4-r7}    @POP
    BX	    lr

coalesce_store_4_stm_2:
	STMFD   sp!,{r4-r11,lr}     @PUSH

loop32stm_2:
	stmia   r0!, {r2-r3}
	stmia   r0!, {r2-r3}
	subs    r1, r1, #4
	bgt	loop32stm_2

	ldmfd   sp!, {r4-r11,lr}
        bx      lr               @POP

coalesce_store_4_stm_4:
	STMFD   sp!,{r4-r11,lr}     @PUSH

loop32stm_4:
	stmia   r0!, {r2-r5}
	stmia   r0!, {r2-r5}
	subs    r1, r1, #8
	bgt	loop32stm_4

	ldmfd   sp!, {r4-r11,lr}
        bx      lr               @POP

coalesce_store_4_stm_8:
	STMFD   sp!,{r4-r11,lr}     @PUSH

loop32stm_8:
	stmia   r0!, {r2-r9}
	stmia   r0!, {r2-r9}
	subs    r1, r1, #16
	bgt	loop32stm_8

	ldmfd   sp!, {r4-r11,lr}
        bx      lr               @POP

coalesce_store_4_stm_7_skip_1:
	STMFD   sp!,{r4-r11,lr}     @PUSH

1:
	stmia   r0!, {r2-r8}
	add	r0, r0, #4
	stmia   r0!, {r2-r8}
	add	r0, r0, #4
	subs    r1, r1, #16
	bgt	1b

	ldmfd   sp!, {r4-r11,lr}
        bx      lr               @POP

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ r0 = src/dst pointer   r2 =temp variable offset = #8
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
load_store_4:
	ldr     r2, [r0], #4
	str     r2, [r0], #4
	ldr     r2, [r0], #4
	str     r2, [r0], #4
	ldr     r2, [r0], #4
	str     r2, [r0], #4
	ldr     r2, [r0], #4
	str     r2, [r0], #4

	ldr     r2, [r0], #4
	str     r2, [r0], #4
	ldr     r2, [r0], #4
	str     r2, [r0], #4

	ldr     r2, [r0], #4
	str     r2, [r0], #4
	ldr     r2, [r0], #4
	str     r2, [r0], #4

	subs    r1,r1,#16
	bgt     load_store_4
	mov 	pc,lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ r0 = src/dst pointer   r2 =temp variable offset = #8
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
load_store_32:
	ldr     r2, [r0], #32
	str     r2, [r0], #32
	ldr     r2, [r0], #32
	str     r2, [r0], #32
	ldr     r2, [r0], #32
	str     r2, [r0], #32
	ldr     r2, [r0], #32
	str     r2, [r0], #32

	ldr     r2, [r0], #32
	str     r2, [r0], #32
	ldr     r2, [r0], #32
	str     r2, [r0], #32

	ldr     r2, [r0], #32
	str     r2, [r0], #32
	ldr     r2, [r0], #32
	str     r2, [r0], #32

	subs    r1,r1,#16
	bgt     load_store_32
	mov 	pc,lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ r0 = src/dst pointer   r2 =temp variable offset = #8
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
load_store_8:
	ldr     r2, [r0], #8
	str     r2, [r0], #8
	ldr     r2, [r0], #8
	str     r2, [r0], #8
	ldr     r2, [r0], #8
	str     r2, [r0], #8
	ldr     r2, [r0], #8
	str     r2, [r0], #8

	ldr     r2, [r0], #8
	str     r2, [r0], #8
	ldr     r2, [r0], #8
	str     r2, [r0], #8

	ldr     r2, [r0], #8
	str     r2, [r0], #8
	ldr     r2, [r0], #8
	str     r2, [r0], #8

	subs    r1,r1,#16
	bgt     load_store_8
	mov 	pc,lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ r0 = src/dst pointer   r2 =temp variable offset = #16
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
load_store_16:
	ldr     r2, [r0], #16
	str     r2, [r0], #16
	ldr     r2, [r0], #16
	str     r2, [r0], #16
	ldr     r2, [r0], #16
	str     r2, [r0], #16
	ldr     r2, [r0], #16
	str     r2, [r0], #16

	ldr     r2, [r0], #16
	str     r2, [r0], #16
	ldr     r2, [r0], #16
	str     r2, [r0], #16

	ldr     r2, [r0], #16
	str     r2, [r0], #16
	ldr     r2, [r0], #16
	str     r2, [r0], #16

	subs    r1,r1,#16
	bgt     load_store_16
	mov 	pc,lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ r0 = src pointer r1=dst pointer  r3 =temp variable offset = #8
@ r2= loop count
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
load_store_interleave_4:
	ldr     r3, [r0], #64
	str     r4, [r1], #64
	ldr     r3, [r0], #64
	str     r4, [r1], #64
	ldr     r3, [r0], #64
	str     r4, [r1], #64
	ldr     r3, [r0], #64
	str     r4, [r1], #64

	ldr     r3, [r0], #64
	str     r4, [r1], #64
	ldr     r3, [r0], #64
	str     r4, [r1], #64

	ldr     r3, [r0], #64
	str     r4, [r1], #64
	ldr     r3, [r0], #64
	str     r4, [r1], #64

	subs    r2,r2,#16
	bgt     load_store_interleave_4
	mov 	pc,lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ r0 = src pointer r1=dst pointer  r3 =temp variable offset = #8
@ r2= loop count
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
load_store_interleave_32:
	ldr     r3, [r0, #128]
	str     r4, [r0, #128]
	ldr     r3, [r0, #128]
	str     r4, [r0, #128]
	ldr     r3, [r0, #128]
	str     r4, [r0, #128]
	ldr     r3, [r0, #128]
	str     r4, [r0, #128]

	ldr     r3, [r0, #128]
	str     r4, [r0, #128]
	ldr     r3, [r0, #128]
	str     r4, [r0, #128]

	ldr     r3, [r0, #128]
	str     r4, [r0, #128]
	ldr     r3, [r0, #128]
	str     r4, [r0, #128]

	subs    r2,r2,#16
	bgt     load_store_interleave_32
	mov 	pc,lr

    .end
@EOF
