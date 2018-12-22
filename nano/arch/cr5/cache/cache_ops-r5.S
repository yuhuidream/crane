
      .equ L2C_310_BASE_ADDR,  0xD1DFB000

      .macro      dcache_line_size reg,tmp
      mrc	p15, 0, \tmp, c0, c0, 1		@ read ctr
      lsr	\tmp, \tmp, #16
      and	\tmp, \tmp, #0xf      		@ cache line size encoding
      mov	\reg, #4                   @ bytes per word
      mov	\reg, \reg, lsl \tmp   		@ actual cache line size
      .endm

      .text
      .code 32


@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global enable_caches 

@NOT SUPPORTED: enable_caches: FUNCTION
enable_caches:
      STMDB   sp!,{r0-r1}
      MRC     p15, 0, r1, c1, c0, 0       @ read CP15 register 1
      ORR     r1, r1, #(0x1 << 12)        @ enable I Cache
      ORR     r1, r1, #(0x1 << 2)         @ enable D Cache
      DSB
      MCR     p15, 0, r0, c15, c5, 0      @ Invalidate entire data cache
      MCR     p15, 0, r0, c7, c5, 0       @ Invalidate entire instruction cache
      MCR     p15, 0, r0, c1, c0, 0       @ write CP15 register 1
      LDMIA   sp!,{r0-r1}
      BX    lr

@NOT SUPPORTED:       ENDFUNC

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global disable_caches 

@NOT SUPPORTED: disable_caches: FUNCTION
disable_caches:
      STMDB   sp!,{r0-r1}
      MRC     p15, 0, r1, c1, c0, 0             @ read CP15 register 1
      BIC     r1, r1, #(0x1 << 12)              @ enable I Cache
      BIC     r1, r1, #(0x1 << 2)               @ enable D Cache
      DSB
      MCR     p15, 0, r0, c7, c5, 0             @ Invalidate entire instruction cache
      MCR     p15, 0, r0, c1, c0, 0             @ write CP15 register 1
      BL      l1_dcache_clean_invalidate_all    @Clean and invalidate entire data cache
      LDMIA   sp!,{r0-r1}
      BX    lr

@NOT SUPPORTED:       ENDFUNC
      
@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global l1_icache_enable 

@NOT SUPPORTED: l1_icache_enable: FUNCTION
l1_icache_enable:
      STMDB   sp!,{r0-r1}
      MRC     p15, 0, r1, c1, c0, 0       @ read CP15 register 1
      ORR     r1, r1, #(0x1 << 12)        @ enable I Cache
      DSB
      MCR     p15, 0, r0, c7, c5, 0       @ Invalidate entire instruction cache
      MCR     p15, 0, r0, c1, c0, 0       @ write CP15 register 1
      LDMIA   sp!,{r0-r1}
      BX    lr

@NOT SUPPORTED:       ENDFUNC
      

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global l1_icache_disable 

@NOT SUPPORTED: l1_icache_disable: FUNCTION
l1_icache_disable:
      STMDB   sp!,{r0-r1}
      MRC     p15, 0, r1, c1, c0, 0       @ read CP15 register 1
      BIC     r1, r1, #(0x1 << 12)        @ enable I Cache
      DSB
      MCR     p15, 0, r0, c7, c5, 0       @ Invalidate entire instruction cache
      MCR     p15, 0, r0, c1, c0, 0       @ write CP15 register 1
      LDMIA   sp!,{r0-r1}
      BX    lr

@NOT SUPPORTED:       ENDFUNC


@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global l1_dcache_enable 

@NOT SUPPORTED: l1_dcache_enable: FUNCTION
l1_dcache_enable:
      STMDB   sp!,{r0-r1}
      MRC     p15, 0, r1, c1, c0, 0       @ read CP15 register 1
      ORR     r1, r1, #(0x1 << 2)         @ enable D Cache
      DSB
      MCR     p15, 0, r0, c15, c5, 0      @ Invalidate entire data cache
      MCR     p15, 0, r0, c1, c0, 0       @ write CP15 register 1
      LDMIA   sp!,{r0-r1}
      BX    lr

@NOT SUPPORTED:       ENDFUNC
      

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;      
      .global l1_dcache_disable 

@NOT SUPPORTED: l1_dcache_disable: FUNCTION
l1_dcache_disable:
      STMDB   sp!,{r0-r1}
      MRC     p15, 0, r1, c1, c0, 0             @ read CP15 register 1
      BIC     r1, r1, #(0x1 << 2)               @ enable D Cache
      DSB
      MCR     p15, 0, r0, c1, c0, 0             @ write CP15 register 1
      BL      l1_dcache_clean_invalidate_all    @Clean and invalidate entire data cache
      LDMIA   sp!,{r0-r1}
      BX      lr

@NOT SUPPORTED:       ENDFUNC


@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global l1_dcache_clean_all 

@NOT SUPPORTED: l1_dcache_clean_all:  FUNCTION
l1_dcache_clean_all:
      stmdb   sp!,{r0-r11}
      MRC     p15, 1, r0, c0, c0, 1     @ Read CLIDR
      ANDS    r3, r0, #0x7000000
      MOV     r3, r3, LSR #23           @ Cache level value (naturally aligned)
      BEQ     clean_dcache_finished
      MOV     r10, #0

clean_dcache_loop1:
      ADD     r2, r10, r10, LSR #1      @ Work out 3xcachelevel
      MOV     r1, r0, LSR r2            @ bottom 3 bits are the Cache type for this level
      AND     r1, r1, #7                @ get those 3 bits alone
      CMP     r1, #2
      BLT     clean_dcache_skip         @ no cache or only instruction cache at this level
      MCR     p15, 2, r10, c0, c0, 0    @ write the Cache Size selection register
      ISB                               @ ISB to sync the change to the CacheSizeID reg
      MRC     p15, 1, r1, c0, c0, 0     @ reads current Cache Size ID register
      AND     r2, r1, #7                @ extract the line length field
      ADD     r2, r2, #4                @ add 4 for the line length offset (log2 16 bytes)
      LDR     r4, =0x3FF
      ANDS    r4, r4, r1, LSR #3        @ R4 is the max number on the way size (right aligned)
      CLZ     r5, r4                    @ R5 is the bit position of the way size increment
      LDR     r7, =0x00007FFF
      ANDS    r7, r7, r1, LSR #13       @ R7 is the max number of the index size (right aligned)

clean_dcache_loop2:
      MOV     r9, R4                    @ R9 working copy of the max way size (right aligned)

clean_dcache_loop3:
      ORR     r11, r10, r9, LSL r5      @ factor in the way number and cache number into R11
      ORR     r11, r11, r7, LSL r2      @ factor in the index number
      MCR     p15, 0, r11, c7, c10, 2   @ DCCSW - clean by set/way
      SUBS    r9, r9, #1                @ decrement the way number
      BGE     clean_dcache_loop3
      SUBS    r7, r7, #1                @ decrement the index
      BGE     clean_dcache_loop2

clean_dcache_skip:
      ADD     r10, r10, #2              @ increment the cache number
      CMP     r3, r10
      BGT     clean_dcache_loop1

      mov   r10, #0                     @ swith back to cache level 0
      mcr   p15, 2, r10, c0, c0, 0      @ select current cache level in cssr
      dsb
      isb
clean_dcache_finished:
      ldmia   sp!,{r0-r11}
      BX      lr

@NOT SUPPORTED:       ENDFUNC      


@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global l1_dcache_clean_invalidate_all 

@NOT SUPPORTED: l1_dcache_clean_invalidate_all:   FUNCTION
l1_dcache_clean_invalidate_all:
      stmdb   sp!,{r0-r11}
      dmb
      MRC     p15, 1, r0, c0, c0, 1     @ Read CLIDR
      ANDS    r3, r0, #0x7000000
      MOV     r3, r3, LSR #23           @ Cache level value (naturally aligned)
      BEQ     clean_invalidate_dcache_finished
      MOV     r10, #0

clean_invalidate_dcache_loop1:
      ADD     r2, r10, r10, LSR #1      @ Work out 3xcachelevel
      MOV     r1, r0, LSR r2            @ bottom 3 bits are the Cache type for this level
      AND     r1, r1, #0x7              @ get those 3 bits alone
      CMP     r1, #2
      BLT     clean_invalidate_dcache_skip @ no cache or only instruction cache at this level
      MCR     p15, 2, r10, c0, c0, 0    @ write the Cache Size selection register
      ISB                               @ ISB to sync the change to the CacheSizeID reg
      MRC     p15, 1, r1, c0, c0, 0     @ reads current Cache Size ID register
      AND     r2, r1, #0x7              @ extract the line length field
      ADD     r2, r2, #4                @ add 4 for the line length offset (log2 16 bytes)
      LDR     r4, =0x3FF
      ANDS    r4, r4, r1, LSR #3        @ R4 is the max number on the way size (right aligned)
      CLZ     r5, r4                    @ R5 is the bit position of the way size increment
      LDR     r7, =0x00007FFF
      ANDS    r7, r7, r1, LSR #13       @ R7 is the max number of the index size (right aligned)

clean_invalidate_dcache_loop2:
      MOV     r9, R4                    @ R9 working copy of the max way size (right aligned)

clean_invalidate_dcache_loop3:
      ORR     r11, r10, r9, LSL r5      @ factor in the way number and cache number into R11
      ORR     r11, r11, r7, LSL r2      @ factor in the index number
      dsb
      MCR     p15, 0, r11, c7, c14, 2   @ DCCISW - clean and invalidate by set/way
      SUBS    r9, r9, #1                @ decrement the way number
      BGE     clean_invalidate_dcache_loop3
      SUBS    r7, r7, #1                @ decrement the index
      BGE     clean_invalidate_dcache_loop2

clean_invalidate_dcache_skip:
      ADD     r10, r10, #2              @ increment the cache number
      CMP     r3, r10
      BGT     clean_invalidate_dcache_loop1

      mov   r10, #0                     @ swith back to cache level 0
      mcr   p15, 2, r10, c0, c0, 0      @ select current cache level in cssr
      dsb
      isb
clean_invalidate_dcache_finished:
      ldmia   sp!,{r0-r11}
      BX      lr

@NOT SUPPORTED:       ENDFUNC
 
 
 @;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;     
      .global l1_dcache_invalidate_all 

@NOT SUPPORTED: l1_dcache_invalidate_all:   FUNCTION
l1_dcache_invalidate_all:
      mov      r0, #0
      MCR      p15, 0, r0, c15, c5, 0      @ Invalidate entire data cache
      BX       lr

@NOT SUPPORTED:       ENDFUNC


@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global l1_dcache_clean_range 

@NOT SUPPORTED: l1_dcache_clean_range: FUNCTION
l1_dcache_clean_range:
      STMDB    sp!,{r0-r3}
	   dcache_line_size r2, r3
	   add	r1, r0, r1
	   sub	r3, r2, #1
	   bic	r0, r0, r3
	   dsb
	   nop
1:
	   mcr	p15, 0, r0, c7, c10, 1      		@ clean D / U line
	   add	r0, r0, r2
	   cmp	r0, r1
	   blo	1b
	   dsb
	   LDMIA    sp!,{r0-r3}
	   BX       lr

@NOT SUPPORTED: 	   ENDFUNC

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global l1_dcache_clean_invalidate_range 

@NOT SUPPORTED: l1_dcache_clean_invalidate_range:    FUNCTION
l1_dcache_clean_invalidate_range:
      STMDB    sp!,{r0-r3}
	   dcache_line_size r2, r3
	   add	r1, r0, r1
	   sub	r3, r2, #1
	   bic	r0, r0, r3
   .ifdef CONFIG_SMP
	   dsb
   .else
	   nop
   .endif
1:
	   mcr	p15, 0, r0, c7, c14, 1		@ clean & invalidate D line / unified line
	   add	r0, r0, r2
	   cmp	r0, r1
	   blo	1b
	   dsb
	   LDMIA    sp!,{r0-r3}
	   BX       lr

@NOT SUPPORTED: 	   ENDFUNC

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global l1_dcache_invalidate_range 

@NOT SUPPORTED: l1_dcache_invalidate_range:    FUNCTION
l1_dcache_invalidate_range:
      STMDB    sp!,{r0-r3}
	   dcache_line_size r2, r3
	   add	r1, r0, r1
	   sub	r3, r2, #1
	   tst	r0, r3
	   bic	r0, r0, r3
	   dsb
	   nop
	   mcrne	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line

	   tst	r1, r3
	   bic	r1, r1, r3
	   mcrne	p15, 0, r1, c7, c14, 1		@ clean & invalidate D / U line
1:
	   mcr	p15, 0, r0, c7, c6, 1		@ invalidate D / U line
	   add	r0, r0, r2
	   cmp	r0, r1
	   blo	1b
	   dsb
	   LDMIA    sp!,{r0-r3}
	   BX       lr

@NOT SUPPORTED: 	   ENDFUNC

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global l1_icache_invalidate_all   

@NOT SUPPORTED: l1_icache_invalidate_all:   FUNCTION
l1_icache_invalidate_all:
      MCR     p15, 0, r0, c7, c5, 0       @ Invalidate entire instruction cache
      BX       lr
      
@NOT SUPPORTED:       ENDFUNC


@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global bpu_enable   

@NOT SUPPORTED: bpu_enable:    FUNCTION
bpu_enable:
      MOV     r0, #0
      MCR     p15, 0, r0, c7, c5, 6       @ BPIALL, Invalidate all branch predictors
      MRC     p15, 0, r0, c1, c0, 1       @ Read ACTLR
      BIC     r0, r0, #(0x1 << 17)        @ Clear RSDIS bit 17 to enable return stack
      BIC     r0, r0, #(0x1 << 16)        @ Clear BP bit 15 and BP bit 16:
      BIC     r0, r0, #(0x1 << 15)        @ Normal operation, BP is taken from the global history table.
      MCR     p15, 0, r0, c1, c0, 1       @ Write ACTLR
      BX       lr
      
@NOT SUPPORTED:       ENDFUNC

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global bpu_disable   

@NOT SUPPORTED: bpu_disable:    FUNCTION
bpu_disable:
      MOV     r0, #0
      MCR     p15, 0, r0, c7, c5, 6       @ BPIALL, Invalidate all branch predictors
      MRC     p15, 0, r0, c1, c0, 1       @ Read ACTLR
      ORR     r0, r0, #(0x1 << 17)        @ Set RSDIS bit 17 to disable return stack
      ORR     r0, r0, #(0x1 << 16)        @ Set BP[16:15] to b10
      BIC     r0, r0, #(0x1 << 15)        @ Branch always not taken and history table updates disabled
      MCR     p15, 0, r0, c1, c0, 1       @ Write ACTLR
      BX       lr
      
@NOT SUPPORTED:       ENDFUNC

@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      .global bpu_invalidate_all   

@NOT SUPPORTED: bpu_invalidate_all:   FUNCTION
bpu_invalidate_all:
      MOV     r0, #0
      MCR     p15, 0, r0, c7, c5, 6       @ BPIALL, Invalidate all branch predictors
      BX       lr
      
@NOT SUPPORTED:       ENDFUNC

   .align 
   .global dcache_clean_invalidate_all
dcache_clean_invalidate_all:
      stmdb   sp!,{r0-r11}
/******************************* Clean 11 dcache ***************************************/
   MRC     p15, 1, r0, c0, c0, 1     @ Read CLIDR
   ANDS    r3, r0, #0x7000000
   MOV     r3, r3, LSR #23           @ Cache level value (naturally aligned)
   BEQ     entire_clean_dcache_finished
   MOV     r10, #0

entire_clean_dcache_loop1:
   ADD     r2, r10, r10, LSR #1      @ Work out 3xcachelevel
   MOV     r1, r0, LSR r2            @ bottom 3 bits are the Cache type for this level
   AND     r1, r1, #7                @ get those 3 bits alone
   CMP     r1, #2
   BLT     entire_clean_dcache_skip         @ no cache or only instruction cache at this level
   MCR     p15, 2, r10, c0, c0, 0    @ write the Cache Size selection register
   ISB                               @ ISB to sync the change to the CacheSizeID reg
   MRC     p15, 1, r1, c0, c0, 0     @ reads current Cache Size ID register
   AND     r2, r1, #7                @ extract the line length field
   ADD     r2, r2, #4                @ add 4 for the line length offset (log2 16 bytes)
   LDR     r4, =0x3FF
   ANDS    r4, r4, r1, LSR #3        @ R4 is the max number on the way size (right aligned)
   CLZ     r5, r4                    @ R5 is the bit position of the way size increment
   LDR     r7, =0x00007FFF
   ANDS    r7, r7, r1, LSR #13       @ R7 is the max number of the index size (right aligned)

entire_clean_dcache_loop2:
   MOV     r9, R4                    @ R9 working copy of the max way size (right aligned)

entire_clean_dcache_loop3:
   ORR     r11, r10, r9, LSL r5      @ factor in the way number and cache number into R11
   ORR     r11, r11, r7, LSL r2      @ factor in the index number
   MCR     p15, 0, r11, c7, c10, 2   @ DCCSW - clean by set/way
   SUBS    r9, r9, #1                @ decrement the way number
   BGE     entire_clean_dcache_loop3
   SUBS    r7, r7, #1                @ decrement the index
   BGE     entire_clean_dcache_loop2

entire_clean_dcache_skip:
   ADD     r10, r10, #2              @ increment the cache number
   CMP     r3, r10
   BGT     entire_clean_dcache_loop1

   mov   r10, #0                     @ swith back to cache level 0
   mcr   p15, 2, r10, c0, c0, 0      @ select current cache level in cssr
   dsb
   isb
entire_clean_dcache_finished:
#ifdef CONFIG_L2_CACHE   

/************************** clean and invalidate l2 cache*************************************/

   dsb                  @ensure ordering with previous memory accesses
   ldr r1,=L2C_310_BASE_ADDR
   mov r0,#0xff
   orr r0, r0, #0xff00
   str r0, [r1, #0x7fc]       @clean and invalidate by way
loop3333:
   ldr r0, [r1, #0x7fc]
   cmp r0, #0
   bne loop3333
   dsb

/* l2 cache sync */
   dsb
   ldr r1, =L2C_310_BASE_ADDR
   mov r0,#0
   str r0, [r1, #0x730]       @sync
loop213:
   ldr r0, [r1, #0x730]
   cmp r0, #0
   bne loop213
   dsb      
#endif
/************************** clean and invalidate l1 d cache*************************************/
   MRC     p15, 1, r0, c0, c0, 1     @ Read CLIDR
   ANDS    r3, r0, #0x7000000
   MOV     r3, r3, LSR #23           @ Cache level value (naturally aligned)
   BEQ     entire_clean_invalidate_dcache_finished
   MOV     r10, #0

entire_clean_invalidate_dcache_loop1:
   ADD     r2, r10, r10, LSR #1      @ Work out 3xcachelevel
   MOV     r1, r0, LSR r2            @ bottom 3 bits are the Cache type for this level
   AND     r1, r1, #0x7              @ get those 3 bits alone
   CMP     r1, #2
   BLT     entire_clean_invalidate_dcache_skip @ no cache or only instruction cache at this level
   MCR     p15, 2, r10, c0, c0, 0    @ write the Cache Size selection register
   ISB                               @ ISB to sync the change to the CacheSizeID reg
   MRC     p15, 1, r1, c0, c0, 0     @ reads current Cache Size ID register
   AND     r2, r1, #0x7              @ extract the line length field
   ADD     r2, r2, #4                @ add 4 for the line length offset (log2 16 bytes)
   LDR     r4, =0x3FF
   ANDS    r4, r4, r1, LSR #3        @ R4 is the max number on the way size (right aligned)
   CLZ     r5, r4                    @ R5 is the bit position of the way size increment
   LDR     r7, =0x00007FFF
   ANDS    r7, r7, r1, LSR #13       @ R7 is the max number of the index size (right aligned)

entire_clean_invalidate_dcache_loop2:
   MOV     r9, R4                    @ R9 working copy of the max way size (right aligned)

entire_clean_invalidate_dcache_loop3:
   ORR     r11, r10, r9, LSL r5      @ factor in the way number and cache number into R11
   ORR     r11, r11, r7, LSL r2      @ factor in the index number
   MCR     p15, 0, r11, c7, c14, 2   @ DCCISW - clean and invalidate by set/way
   SUBS    r9, r9, #1                @ decrement the way number
   BGE     entire_clean_invalidate_dcache_loop3
   SUBS    r7, r7, #1                @ decrement the index
   BGE     entire_clean_invalidate_dcache_loop2

entire_clean_invalidate_dcache_skip:
   ADD     r10, r10, #2              @ increment the cache number
   CMP     r3, r10
   BGT     entire_clean_invalidate_dcache_loop1

   mov   r10, #0                     @ swith back to cache level 0
   mcr   p15, 2, r10, c0, c0, 0      @ select current cache level in cssr
   dsb
   isb
entire_clean_invalidate_dcache_finished:
   ldmia   sp!,{r0-r11}
   BX      lr
   

      .align 
   .global dcache_clean_all
dcache_clean_all:
      stmdb   sp!,{r0-r11}
/******************************* Clean 11 dcache ***************************************/
   MRC     p15, 1, r0, c0, c0, 1     @ Read CLIDR
   ANDS    r3, r0, #0x7000000
   MOV     r3, r3, LSR #23           @ Cache level value (naturally aligned)
   BEQ     entire2_clean_dcache_finished
   MOV     r10, #0

entire2_clean_dcache_loop1:
   ADD     r2, r10, r10, LSR #1      @ Work out 3xcachelevel
   MOV     r1, r0, LSR r2            @ bottom 3 bits are the Cache type for this level
   AND     r1, r1, #7                @ get those 3 bits alone
   CMP     r1, #2
   BLT     entire2_clean_dcache_skip         @ no cache or only instruction cache at this level
   MCR     p15, 2, r10, c0, c0, 0    @ write the Cache Size selection register
   ISB                               @ ISB to sync the change to the CacheSizeID reg
   MRC     p15, 1, r1, c0, c0, 0     @ reads current Cache Size ID register
   AND     r2, r1, #7                @ extract the line length field
   ADD     r2, r2, #4                @ add 4 for the line length offset (log2 16 bytes)
   LDR     r4, =0x3FF
   ANDS    r4, r4, r1, LSR #3        @ R4 is the max number on the way size (right aligned)
   CLZ     r5, r4                    @ R5 is the bit position of the way size increment
   LDR     r7, =0x00007FFF
   ANDS    r7, r7, r1, LSR #13       @ R7 is the max number of the index size (right aligned)

entire2_clean_dcache_loop2:
   MOV     r9, R4                    @ R9 working copy of the max way size (right aligned)

entire2_clean_dcache_loop3:
   ORR     r11, r10, r9, LSL r5      @ factor in the way number and cache number into R11
   ORR     r11, r11, r7, LSL r2      @ factor in the index number
   MCR     p15, 0, r11, c7, c10, 2   @ DCCSW - clean by set/way
   SUBS    r9, r9, #1                @ decrement the way number
   BGE     entire2_clean_dcache_loop3
   SUBS    r7, r7, #1                @ decrement the index
   BGE     entire2_clean_dcache_loop2

entire2_clean_dcache_skip:
   ADD     r10, r10, #2              @ increment the cache number
   CMP     r3, r10
   BGT     entire2_clean_dcache_loop1

   mov   r10, #0                     @ swith back to cache level 0
   mcr   p15, 2, r10, c0, c0, 0      @ select current cache level in cssr
   dsb
   isb
entire2_clean_dcache_finished:
   
#ifdef CONFIG_L2_CACHE
/************************** clean l2 cache*************************************/

   dsb                  @ensure ordering with previous memory accesses
   ldr r1,=L2C_310_BASE_ADDR
   mov r0,#0xff
   orr r0, r0, #0xff00
   str r0, [r1, #0x7bc]       @clean by way
loop3333111:
   ldr r0, [r1, #0x7bc]
   cmp r0, #0
   bne loop3333111
   dsb

/* l2 cache sync */
   dsb
   ldr r1, =L2C_310_BASE_ADDR
   mov r0,#0
   str r0, [r1, #0x730]       @sync
loop22213:
   ldr r0, [r1, #0x730]
   cmp r0, #0
   bne loop22213
   dsb
#endif

   ldmia   sp!,{r0-r11}
   BX      lr
   


   .align 
   .global dcache_invalidate_all
dcache_invalidate_all:
   stmdb   sp!,{r0-r11}
   
/**************************  invalidate l2 cache*************************************/
#ifdef CONFIG_L2_CACHE
   dsb                  @ensure ordering with previous memory accesses
   ldr r1,=L2C_310_BASE_ADDR
   mov r0,#0xff
   orr r0, r0, #0xff00
   str r0, [r1, #0x77c]    @invalidate by way
loop13333111:
   ldr r0, [r1, #0x77c]
   cmp r0, #0
   bne loop13333111
   dsb

/* l2 cache sync */
   dsb
   ldr r1, =L2C_310_BASE_ADDR
   mov r0,#0
   str r0, [r1, #0x730]       @sync
loop122213:
   ldr r0, [r1, #0x730]
   cmp r0, #0
   bne loop122213
   dsb
#endif
   
/******************************* Invalidate 11 dcache ***************************************/
   MRC     p15, 1, r0, c0, c0, 1     @ Read CLIDR
   ANDS    r3, r0, #0x7000000
   MOV     r3, r3, LSR #23           @ Cache level value (naturally aligned)
   BEQ     entire3_invalidate_dcache_finished
   MOV     r10, #0

entire3_invalidate_dcache_loop1:
   ADD     r2, r10, r10, LSR #1      @ Work out 3xcachelevel
   MOV     r1, r0, LSR r2            @ bottom 3 bits are the Cache type for this level
   AND     r1, r1, #7                @ get those 3 bits alone
   CMP     r1, #2
   BLT     entire3_invalidate_dcache_skip @ no cache or only instruction cache at this level
   MCR     p15, 2, r10, c0, c0, 0    @ write the Cache Size selection register
   ISB                               @ ISB to sync the change to the CacheSizeID reg
   MRC     p15, 1, r1, c0, c0, 0     @ reads current Cache Size ID register
   AND     r2, r1, #0x7              @ extract the line length field
   ADD     r2, r2, #4                @ add 4 for the line length offset (log2 16 bytes)
   LDR     r4, =0x3FF
   ANDS    r4, r4, r1, LSR #3        @ R4 is the max number on the way size (right aligned)
   CLZ     r5, r4                    @ R5 is the bit position of the way size increment
   LDR     r7, =0x00007FFF
   ANDS    r7, r7, r1, LSR #13       @ R7 is the max number of the index size (right aligned)

entire3_invalidate_dcache_loop2:
   MOV     r9, R4                    @ R9 working copy of the max way size (right aligned)

entire3_invalidate_dcache_loop3:
   ORR     r11, r10, r9, LSL r5      @ factor in the way number and cache number into R11
   ORR     r11, r11, r7, LSL r2      @ factor in the index number
   MCR     p15, 0, r11, c7, c6, 2    @ DCISW - invalidate by set/way
   SUBS    r9, r9, #1                @ decrement the way number
   BGE     entire3_invalidate_dcache_loop3
   SUBS    r7, r7, #1                @ decrement the index
   BGE     entire3_invalidate_dcache_loop2

entire3_invalidate_dcache_skip:
   ADD     r10, r10, #2              @ increment the cache number
   CMP     r3, r10
   BGT     entire3_invalidate_dcache_loop1

   mov   r10, #0                     @ swith back to cache level 0
   mcr   p15, 2, r10, c0, c0, 0      @ select current cache level in cssr
   dsb
   isb
entire3_invalidate_dcache_finished:
   ldmia   sp!,{r0-r11}
   BX      lr
      
