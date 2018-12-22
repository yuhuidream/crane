# 1 "/home/zcyang/work/pmu_experiment/events/memcpyv5.S"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "/home/zcyang/work/pmu_experiment/events/memcpyv5.S"
 @ GET lubbock_cotulla_def.s
 @ GET cotulla_def.s
 @ GET boot.s
    .global memcpy_xsc_v5
    .text
    .align 5

@
@
memcpy_xsc_v5:
 @
    stmfd sp!, {r3-r12, lr}

@
@
preload_setup:
@
    @pld [r1]
    .word 0xf5d1f000

@
@
    cmp r2, #4
    bls finish

    @pld [r1,#32]
    @pld [r1,#64]
    .word 0xf5d1f020
    .word 0xf5d1f040


@
    rsb r4, r0, #0
    add r4, r4, #4
    and r4, r4, #0x2 @
    cmp r4, #2
    ldreqb r5, [r1], #1
    ldreqb r6, [r1], #1 @
    subeq r2, r2, #2 @
    streqb r5, [r0], #1 @
    streqb r6, [r0], #1 @
          @
    and r4, r0, #0x1 @
    cmp r4, #1
    ldreqb r5, [r1], #1 @
    subeq r2, r2, #1 @
    streqb r5, [r0], #1 @

@
@
@
@
@
    and r3, r1, #3
    cmp r3, #3 @
    beq memcpyoffby3 @
    cmp r3, #2 @
    beq memcpyoffby2 @
    cmp r3, #1 @
    beq memcpyoffby1 @

@
memcpyoffby0:

@
@
@
@
    and r4, r1, #0x1C @
    rsb r4, r4, #0 @
    add r4, r4, #32 @
          @
    and r5, r2, #0x1C @
    cmp r4, r2 @
    movhi r4, r5 @
          @

@
@
@
    rsb r3, r4, #0
    add r3, r3, #32
    and r3, r3, #0x1C
@

@
    sub r2, r2, r4

@
@
    adr r12, offby0cachelinealignload
    add pc, r12, r3

offby0cachelinealignload: @
    ldr r4, [r1], #4 @
    ldr r5, [r1], #4 @
    ldr r6, [r1], #4 @
    ldr r7, [r1], #4 @
    ldr r8, [r1], #4
    ldr r9, [r1], #4
    ldr r10,[r1], #4
    ldr r11,[r1], #4

@
    adr r12, offby0cachelinealignstore
    add pc, r12, r3

offby0cachelinealignstore:
    str r4, [r0], #4
    str r5, [r0], #4
    str r6, [r0], #4
    str r7, [r0], #4
    str r8, [r0], #4
    str r9, [r0], #4
    str r10,[r0], #4
    str r11,[r0], #4

@
@
offby0mainloop:
    cmp r2, #(32*3 + 32) @
    bmi offby0endofmainloop

@
    @pld [r1, #32*3]
    .word 0xf5d1f060

@
    @ldrd r4, [r1], #8
    @ldrd r6, [r1], #8
    @ldrd r8, [r1], #8
    .word 0xe0c140d8
    .word 0xe0c160d8
    .word 0xe0c180d8

    str r4, [r0], #4
    str r5, [r0], #4

    @ldrd r10,[r1], #8
    .word 0xe0c1a0d8
@
    str r6, [r0], #4
    str r7, [r0], #4
    str r8, [r0], #4
    str r9, [r0], #4
    str r10,[r0], #4
    str r11,[r0], #4

    sub r2, r2, #32 @
    b offby0mainloop

offby0endofmainloop:
@
    cmp r2, #32*3
    bls offby0nopreload
    @pld [r1, #32*3]
    .word 0xf5d1f060

offby0nopreload:

@
@
@
offby0finishcachelines:
    cmp r2, #32
    bmi offby0endoffinishcachelines

    @ldrd r4, [r1], #8
    @ldrd r6, [r1], #8
    @ldrd r8, [r1], #8

    .word 0xe0c140d8
    .word 0xe0c160d8
    .word 0xe0c180d8

    str r4, [r0], #4
    str r5, [r0], #4

    @ldrd r10,[r1], #8
    .word 0xe0c1a0d8
@
    str r6, [r0], #4
    str r7, [r0], #4
    str r8, [r0], #4
    str r9, [r0], #4
    str r10,[r0], #4
    str r11,[r0], #4

    sub r2, r2, #32 @
    b offby0finishcachelines

offby0endoffinishcachelines:

@
@
    and r3, r2, #0x1C @
    cmp r3, #0
    beq finish @
    sub r2, r2, r3 @
    rsb r3, r3, #32 @

    adr r12, offby0finishload @
    add pc, r12, r3

offby0finishload: @
    ldr r4, [r1], #4 @
    ldr r5, [r1], #4 @
    ldr r6, [r1], #4
    ldr r7, [r1], #4
    ldr r8, [r1], #4
    ldr r9, [r1], #4
    ldr r10,[r1], #4
    ldr r11,[r1], #4

@
    adr r12, offby0finishstore
    add pc, r12, r3

offby0finishstore:
    str r4, [r0], #4
    str r5, [r0], #4
    str r6, [r0], #4
    str r7, [r0], #4
    str r8, [r0], #4
    str r9, [r0], #4
    str r10,[r0], #4
    str r11,[r0], #4

    b finish

@
@
@
memcpyoffby1:

@
    and r3, r1, #~0x3
@
    ldr lr, [r3], #4
    mov lr, lr, LSR #8


@
@
@
@
    and r4, r3, #0x1C @
    rsb r4, r4, #0 @
    add r4, r4, #32 @
          @
    and r5, r2, #0x1C @
    cmp r4, r2 @
    movhi r4, r5 @
          @

@
@
@
    rsb r6, r4, #0
    add r6, r6, #32
    and r6, r6, #0x1C
@

@
    sub r2, r2, r4

@
@
    adr r12, offby1cachelinealignload
    add pc, r12, r6, LSL #2 @

@
@
offby1cachelinealignload: @
    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8


@
@
offby1mainloop:
    cmp r2, #(32*3 + 32) @
    bmi offby1endofmainloop

    @pld [r3, #32*3]
    .word 0xf5d3f060

@
    @ldrd r4, [r3], #8
    @ldrd r6, [r3], #8
    @ldrd r8, [r3], #8
    .word 0xe0c340d8
    .word 0xe0c360d8
    .word 0xe0c380d8


    orr r10,lr, r4, LSL #24
    mov lr, r4, LSR #8
    str r10, [r0], #4

    orr r10, lr, r5, LSL #24
    mov lr, r5, LSR #8
    str r10, [r0], #4

    @ldrd r4, [r3], #8
    .word 0xe0c340d8
    orr r10, lr, r6, LSL #24
    mov lr, r6, LSR #8
    str r10, [r0], #4

    orr r10, lr, r7, LSL #24
    mov lr, r7, LSR #8
    str r10, [r0], #4

    orr r10, lr, r8, LSL #24
    mov lr, r8, LSR #8
    str r10, [r0], #4

    orr r10, lr, r9, LSL #24
    mov lr, r9, LSR #8
    str r10, [r0], #4

    orr r10, lr, r4, LSL #24
    mov lr, r4, LSR #8
    str r10, [r0], #4

    orr r10, lr, r5, LSL #24
    mov lr, r5, LSR #8
    str r10, [r0], #4

    sub r2, r2, #32 @
    b offby1mainloop

offby1endofmainloop:
@
    cmp r2, #32*3
    bls offby1nopreload
    @pld [r1, #32*3]
    .word 0xf5d1f060

offby1nopreload:

@
@
@
offby1finishcachelines:
    cmp r2, #32
    bmi offby1endoffinishcachelines

@
    @ldrd r4, [r3], #8
    @ldrd r6, [r3], #8
    .word 0xe0c340d8
    .word 0xe0c360d8
    orr r10,lr, r4, LSL #24

    @ldrd r8, [r3], #8
    .word 0xe0c380d8
    mov lr, r4, LSR #8
    str r10, [r0], #4

    orr r10, lr, r5, LSL #24
    mov lr, r5, LSR #8
    str r10, [r0], #4

    @ldrd r4, [r3], #8
    .word 0xe0c340d8

    orr r10, lr, r6, LSL #24
    mov lr, r6, LSR #8
    str r10, [r0], #4

    orr r10, lr, r7, LSL #24
    mov lr, r7, LSR #8
    str r10, [r0], #4

    orr r10, lr, r8, LSL #24
    mov lr, r8, LSR #8
    str r10, [r0], #4

    orr r10, lr, r9, LSL #24
    mov lr, r9, LSR #8
    str r10, [r0], #4

    orr r10, lr, r4, LSL #24
    mov lr, r4, LSR #8
    str r10, [r0], #4

    orr r10, lr, r5, LSL #24
    mov lr, r5, LSR #8
    str r10, [r0], #4

    sub r2, r2, #32 @
    b offby1finishcachelines

offby1endoffinishcachelines:

@
@
    and r6, r2, #0x1C @
    cmp r6, #0
    subeq r1, r3, #3 @
    beq finish @
    sub r2, r2, r6 @
    rsb r6, r6, #32 @

    adr r12, offby1finishload @
    add pc, r12, r6, LSL #2 @

offby1finishload: @
    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #24
    str r12,[r0], #4
    mov lr, r4, LSR #8

    sub r1, r3, #3 @

    b finish

@
@
@
memcpyoffby2:

@
    and r3, r1, #~0x3
@
    ldr lr, [r3], #4
    mov lr, lr, LSR #16


@
@
@
@
    and r4, r3, #0x1C @
    rsb r4, r4, #0 @
    add r4, r4, #32 @
          @
    and r5, r2, #0x1C @
    cmp r4, r2 @
    movhi r4, r5 @
          @

@
@
@
    rsb r6, r4, #0
    add r6, r6, #32
    and r6, r6, #0x1C
@

@
    sub r2, r2, r4

@
@
    adr r12, offby2cachelinealignload
    add pc, r12, r6, LSL #2 @

@
@
offby2cachelinealignload: @
    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

@
@
offby2mainloop:
    cmp r2, #(32*3 + 32) @
    bmi offby2endofmainloop

    @pld [r3, #32*3]
    .word 0xf5d3f060
@
    @ldrd r4, [r3], #8
    @ldrd r6, [r3], #8
    .word 0xe0c340d8
    .word 0xe0c360d8
    orr r10,lr, r4, LSL #16

    @ldrd r8, [r3], #8
    .word 0xe0c380d8

    mov lr, r4, LSR #16
    str r10, [r0], #4

    orr r10, lr, r5, LSL #16
    mov lr, r5, LSR #16
    str r10, [r0], #4

    @ldrd r4, [r3], #8
    .word 0xe0c340d8

    orr r10, lr, r6, LSL #16
    mov lr, r6, LSR #16
    str r10, [r0], #4

    orr r10, lr, r7, LSL #16
    mov lr, r7, LSR #16
    str r10, [r0], #4

    orr r10, lr, r8, LSL #16
    mov lr, r8, LSR #16
    str r10, [r0], #4

    orr r10, lr, r9, LSL #16
    mov lr, r9, LSR #16
    str r10, [r0], #4

    orr r10, lr, r4, LSL #16
    mov lr, r4, LSR #16
    str r10, [r0], #4

    orr r10, lr, r5, LSL #16
    mov lr, r5, LSR #16
    str r10, [r0], #4

    sub r2, r2, #32 @
    b offby2mainloop

offby2endofmainloop:
@
    cmp r2, #32*3
    bls offby2nopreload
    @pld [r1, #32*3]
    .word 0xf5d1f060

offby2nopreload:

@
@
@
offby2finishcachelines:
    cmp r2, #32
    bmi offby2endoffinishcachelines

    @ldrd r4, [r3], #8
    @ldrd r6, [r3], #8
    .word 0xe0c340d8
    .word 0xe0c360d8

    orr r10,lr, r4, LSL #16

    @ldrd r8, [r3], #8
    .word 0xe0c380d8

    mov lr, r4, LSR #16
    str r10, [r0], #4

    orr r10, lr, r5, LSL #16
    mov lr, r5, LSR #16
    str r10, [r0], #4

    @ldrd r4, [r3], #8
    .word 0xe0c340d8

    orr r10, lr, r6, LSL #16
    mov lr, r6, LSR #16
    str r10, [r0], #4

    orr r10, lr, r7, LSL #16
    mov lr, r7, LSR #16
    str r10, [r0], #4

    orr r10, lr, r8, LSL #16
    mov lr, r8, LSR #16
    str r10, [r0], #4

    orr r10, lr, r9, LSL #16
    mov lr, r9, LSR #16
    str r10, [r0], #4

    orr r10, lr, r4, LSL #16
    mov lr, r4, LSR #16
    str r10, [r0], #4

    orr r10, lr, r5, LSL #16
    mov lr, r5, LSR #16
    str r10, [r0], #4

    sub r2, r2, #32 @
    b offby2finishcachelines

offby2endoffinishcachelines:

@
@
    and r6, r2, #0x1C @
    cmp r6, #0
    subeq r1, r3, #2 @
    beq finish @
    sub r2, r2, r6 @
    rsb r6, r6, #32 @

    adr r12, offby2finishload @
    add pc, r12, r6, LSL #2 @

offby2finishload: @
    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #16
    str r12,[r0], #4
    mov lr, r4, LSR #16

    sub r1, r3, #2 @

    b finish

@
@
@
memcpyoffby3:

@
    and r3, r1, #~0x3
@
    ldr lr, [r3], #4
    mov lr, lr, LSR #24


@
@
@
@
    and r4, r3, #0x1C @
    rsb r4, r4, #0 @
    add r4, r4, #32 @
          @
    and r5, r2, #0x1C @
    cmp r4, r2 @
    movhi r4, r5 @
          @

@
@
@
    rsb r6, r4, #0
    add r6, r6, #32
    and r6, r6, #0x1C
@

@
    sub r2, r2, r4

@
@
    adr r12, offby3cachelinealignload
    add pc, r12, r6, LSL #2 @

@
@
offby3cachelinealignload: @
    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

@
@
offby3mainloop:
    cmp r2, #(32*3 + 32) @
    bmi offby3endofmainloop

    @pld [r3, #32*3]
    .word 0xf5d3f060
@
    @ldrd r4, [r3], #8
    @ldrd r6, [r3], #8
    .word 0xe0c340d8
    .word 0xe0c360d8

    orr r10,lr, r4, LSL #8

    @ldrd r8, [r3], #8
    .word 0xe0c380d8

    mov lr, r4, LSR #24
    str r10, [r0], #4

    orr r10, lr, r5, LSL #8
    mov lr, r5, LSR #24
    str r10, [r0], #4

    @ldrd r4, [r3], #8
    .word 0xe0c340d8

    orr r10, lr, r6, LSL #8
    mov lr, r6, LSR #24
    str r10, [r0], #4

    orr r10, lr, r7, LSL #8
    mov lr, r7, LSR #24
    str r10, [r0], #4

    orr r10, lr, r8, LSL #8
    mov lr, r8, LSR #24
    str r10, [r0], #4

    orr r10, lr, r9, LSL #8
    mov lr, r9, LSR #24
    str r10, [r0], #4

    orr r10, lr, r4, LSL #8
    mov lr, r4, LSR #24
    str r10, [r0], #4

    orr r10, lr, r5, LSL #8
    mov lr, r5, LSR #24
    str r10, [r0], #4

    sub r2, r2, #32 @
    b offby3mainloop

offby3endofmainloop:
@
    cmp r2, #32*3
    bls offby3nopreload
    @pld [r1, #32*3]
    .word 0xf5d1f060

offby3nopreload:

@
@
@
offby3finishcachelines:
    cmp r2, #32
    bmi offby3endoffinishcachelines

    @ldrd r4, [r3], #8
    @ldrd r6, [r3], #8
    .word 0xe0c340d8
    .word 0xe0c360d8

    orr r10,lr, r4, LSL #8

    @ldrd r8, [r3], #8
    .word 0xe0c380d8

    mov lr, r4, LSR #24
    str r10, [r0], #4

    orr r10, lr, r5, LSL #8
    mov lr, r5, LSR #24
    str r10, [r0], #4

    @ldrd r4, [r3], #8
    .word 0xe0c340d8

    orr r10, lr, r6, LSL #8
    mov lr, r6, LSR #24
    str r10, [r0], #4

    orr r10, lr, r7, LSL #8
    mov lr, r7, LSR #24
    str r10, [r0], #4

    orr r10, lr, r8, LSL #8
    mov lr, r8, LSR #24
    str r10, [r0], #4

    orr r10, lr, r9, LSL #8
    mov lr, r9, LSR #24
    str r10, [r0], #4

    orr r10, lr, r4, LSL #8
    mov lr, r4, LSR #24
    str r10, [r0], #4

    orr r10, lr, r5, LSL #8
    mov lr, r5, LSR #24
    str r10, [r0], #4

    sub r2, r2, #32 @
    b offby3finishcachelines

offby3endoffinishcachelines:

@
@
    and r6, r2, #0x1C @
    cmp r6, #0
    subeq r1, r3, #1 @
    beq finish @
    sub r2, r2, r6 @
    rsb r6, r6, #32 @

    adr r12, offby3finishload @
    add pc, r12, r6, LSL #2 @

offby3finishload: @
    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    ldr r4, [r3], #4
    orr r12,lr, r4, LSL #8
    str r12,[r0], #4
    mov lr, r4, LSR #24

    sub r1, r3, #1 @

    b finish

@
finish:
    rsb r2, r2, #4 @
    adr r12, finishload
    add pc, r12, r2, LSL #2 @

finishload:
    ldrb r3, [r1], #1
    ldrb r4, [r1], #1
    ldrb r5, [r1], #1
    ldrb r6, [r1], #1

    adr r12, finishstore
    add pc, r12, r2, LSL #2

finishstore:
    strb r3, [r0], #1
    strb r4, [r0], #1
    strb r5, [r0], #1
    strb r6, [r0], #1

@
    ldmfd sp!, {r3-r12, pc}


  .end
