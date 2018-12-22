   .text
   .code 32

/* void transfer_ctrl(uint32_t dest_address)
*/
   .global transfer_ctrl
   .func transfer_ctrl
transfer_ctrl:
   MOV r1, #0
   MCR p15, 0, r1, c7, c5, 6     @ Invalidate all branch predictors
   MCR p15, 0, r1, c15, c5, 0    @ Invalidate entire data cache
   MCR p15, 0, r1, c7, c5, 0     @ Invalidate entire instruction cache
   DSB
   mov  	pc, r0
   
   .size   transfer_ctrl, . - transfer_ctrl
   .endfunc