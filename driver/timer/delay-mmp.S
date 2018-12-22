/*
This is start from MMP3.  used for delays. while OS tick use a different timer
*/
    .text
    .code 32
    .global __tdelay
    .func   __tdelay

__tdelay:

/*
 * r4 - cycles we have to wait
 * r5 - cycles before udelay
 * r0 - input ticks to be delayed@the source clock, e.g. 3.25MHz
 */
        stmdb   sp!, {r4, r5, r6, lr}
        mov     r4, r0
        mrrc    p15, 0, r0, r1, c14  @64bit count, while we assume just use low 32bit
        mov     r5, r0
loop:
        mrrc    p15, 0, r0, r1, c14
        rsb     r0, r5, r0
        cmp     r0, r4
        bcc     loop
end:
        ldmia   sp!, {r4, r5, r6, pc}
    .endfunc


@ Delay some CPU cycle routine, not costant
    .global __delay
    .func   __delay
__delay:
        subs  r0, r0, #1
        bhi __delay
        mov pc, lr
    .endfunc

