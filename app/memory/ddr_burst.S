	.text
	.global ddr_burst8_mode
	.func ddr_burst8_mode
ddr_burst8_mode:
	@	r0 points to source adderess
	@	r1 points to destination adderess
	stmdb   R13!, {R0 - R11}
	ldmia r0!, {r2 - r9}
	stmia r1!, {r2 - r9}
	ldmia R13!, {R0 - R11}
	MOV     pc,lr           @ Return
	.endfunc