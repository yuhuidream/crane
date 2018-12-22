
    .text
    .code 32
    .global _begin
    .func   _begin

_begin:

     b       reset

@/////////////////////////////////////////////////////////////////////////////
        .align 4
reset:

@Enable TCM
        ldr     r1, =0xd4282ca0
        ldr     r0, [r1]
        orr     r0, r0, #(1<<11)
        orr     r0, r0, #(1<<10)
        orr     r0, r0, #(1<<9)
        str     r0, [r1]
        dsb

        ldr     r0, =0xd4282c24
        mov     r1, #0
        str     r1, [r0]

        mov     r0, #0
        ldr     r1, =0x100000
        ldr     r2, =0x110000
1:
        ldr     r3, [r1], #4
        str     r3, [r0], #4
        cmp     r1, r2
        blt     1b

        ldr     r0, =0xB0020000
        ldr     r1, =0x110000
        ldr     r2, =0x120000
2:
        ldr     r3, [r1], #4
        str     r3, [r0], #4
        cmp     r1, r2
        blt     2b

        mov     pc, #0

        .end

