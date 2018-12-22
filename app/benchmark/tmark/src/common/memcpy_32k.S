
	.extern mrvl_fast
        .align
#ifdef ARM9_NO_STD_REPLACEMENT1
        .global arm9_memcpy_32k
arm9_memcpy_32k:
#else
        .global memcpy_32k
memcpy_32k:
#endif
        stmfd        sp!, {r4,r5,r6,lr}
            mov r6,r2
            mov r4,r0
            mov r5,r1

201:

            cmp r6, #0x8000
            bls 202f
            mov r2, #0x8000
            bl memcpy_fast
            mov  r0,r4
            add  r4,r4,#0x8000
            mrs r1,CPSR
            ands r1,r1,#0xF          /* are we privledged mode? */
            mcrrne p6,1,r4,r0,c6     /* if so batch the memory writes through L2 (note this only works with the architectural SL2 in V7 mode) */
            subs r6,r6,#0x8000
            add  r5,r5,#0x8000
            mov r0,r4
            mov r2,r6
            mov r1,r5
            b 201b

202:

            /* last iteration */

            bl memcpy_fast
        ldmfd        sp!, {r4,r5,r6,lr}

            bx lr
