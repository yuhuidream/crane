	.title RTL MEMMORY FUNCTIONS
	.global	mymemset
	.global mymemset_update
	.global	mymemcpy

	.text
	.align 5
@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
@;void mymemset(char * dst, char element, int length)
@;added by david yang
@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
mymemset:
	STMFD	sp!, {r4-r6,lr}

	ORR	r1,r1,r1,LSL #8
	ORR	r1,r1,r1,LSL #16
	MOV	r3,#16
	MOV	r4,r1
	MOV	r5,r1
	MOV	r6,r1
	CMP	r2,r3
	BLT	OneByte_set
Loop_set:
	@ STMIA	r0!,{r1,r4,r5,r6}
	STR     r1, [r0], #4
	STR     r4, [r0], #4
	STR     r5, [r0], #4
	STR     r6, [r0], #4
	ADD	r3,r3,#16
	CMP	r3,r2
	BLT	Loop_set

OneByte_set:
	SUB	r3,r3,#16
Loop2_set:
	CMP	r3,r2
	BEQ	End_set
	STRB	r1,[r0],#1
	ADD	r3,r3,#1
	B	Loop2_set

End_set:
	LDMFD	sp!, {r4-r6,pc}

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
@;void mymemset(char * dst, char element, int length, int total_size)
@;added by david yang
@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
mymemset_update:
	STMFD	sp!, {r4-r10,lr}

	ORR	r1,r1,r1,LSL #8
	ORR	r1,r1,r1,LSL #16
	MOV     r8, #1000
	MOV     r10, r3
	MOV     r7, r2
	MOV	r3,#32
	MOV	r4,r1
	MOV	r5,r1
	MOV	r6,r1
	CMP	r2,r3
	BLT	OneByte_set_up
Loop_set_e:
	mov     r2, r7
	mov     r9, r8
	ldr     r3, [r10], #32
	mov     r3, #32
Loop_set_i:
	@ STMIA	r0!,{r1,r4,r5,r6}
	STR     r1, [r0], #4
	STR     r4, [r0], #4
	STR     r5, [r0], #4
	STR     r6, [r0], #4
	STR     r1, [r0], #4
	STR     r4, [r0], #4
	STR     r5, [r0], #4
	STR     r6, [r0], #4
	ADD	r3,r3,#32
	CMP	r3,r2
	BLT	Loop_set_i
	subs    r9, r9, #1
	bgt     Loop_set_e

OneByte_set_up:
	SUB	r3,r3,#32
Loop2_set_up:
	CMP	r3,r2
	BEQ	End_set_up
	STRB	r1,[r0],#1
	ADD	r3,r3,#1
	B	Loop2_set_up

End_set_up:
	LDMFD	sp!, {r4-r10,pc}

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
@;   void mymemcpy( char * dst, char * src, unsigned int length)
@;   added by david yang
@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.equ   MULTICOPYSIZE,  16
	.equ   COPYBASESIZE,   4

mymemcpy:
        stmfd      sp!, {r4-r6,r10-r12,lr}      @ ;Push registers
        cmp        r2,#3   			@ ;Check if size is smaller than 0x3
        ble        Unaligned_copy_std
        and	   r6,r0,#3
        and	   r5,r1,#3
        cmp	   r6,r5			@ ;Src%0x3 ?= dest%0x3
        bne	   Unaligned_copy_std
        cmp	   r6,#0			@ ;Src%0x3 ?=0
        beq	   Start_copy_std
Align_copy_std:
        ldrb       r12,[r1],#1			@ ;Copy Src%0x3 byte by byte
        subs       r6,r6,#1
        strb	   r12,[r0],#1
        bne	   Align_copy_std
Start_copy_std:
        mov	   r4,#MULTICOPYSIZE   		@ ;(size-(Src%0x3)) ?>= 0xc
        sub	   r2,r2,r5
        cmp	   r2,r4
        blt	   OneByte_copy_std
Loop_copy_std:
        ldr	   r12,[r1],#COPYBASESIZE  	@ ;Copy word by word from src to dest
        add	   r4,r4,#MULTICOPYSIZE
        ldr        r11,[r1],#COPYBASESIZE
        ldr        r10,[r1],#COPYBASESIZE
	ldr        r3, [r1],#COPYBASESIZE
        str	   r12,[r0],#COPYBASESIZE
        cmp	   r4,r2
        str        r11,[r0],#COPYBASESIZE
        str        r10,[r0],#COPYBASESIZE
	str        r3, [r0],#COPYBASESIZE
        blt	   Loop_copy_std
OneByte_copy_std:
        sub	   r4,r4,#MULTICOPYSIZE       	@ ;Copy the remain size byte by byte
        cmp	   r4,r2
        beq	   End_copy_std
Loop2_copy_std:
        ldrb	   r12,[r1],#1
        add	   r4,r4,#1
        cmp	   r4,r2
        strb	   r12,[r0],#1
        beq	   End_copy_std
        b	   Loop2_copy_std
Unaligned_copy_std:
        movs       r4,r2			@ ;Unaligned_copy means Copy byte by byte
ULoop_copy_std:
        ldrb       r12,[r1],#1
        beq        End_copy_std
        subs       r4,r4,#1
        strb       r12,[r0],#1
        bne        ULoop_copy_std
End_copy_std:
        ldmfd	   sp!, {r4-r6,r10-r12,pc}	@ ;Pop registers

	.end
