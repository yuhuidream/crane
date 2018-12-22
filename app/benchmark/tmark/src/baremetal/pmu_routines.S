@********************************************************************
@Jolteon PMU v7 initialization Code
@David Yang, 08, 11, 02
@********************************************************************

@ pmu.s

    .text 
    .align 5
 	
 	
    .global	enable_counters
    .global	disable_counters
    .global	reset_CCNT
    .global	reset_PMU
    .global	set_counter_divider
    .global	set_event_PMCNT
    .global 	read_CCNT
    .global	read_PMCNT
    .global     enable_overflow_flag
    .global     disable_overflow_flag
    .global     read_overflow_flag
    .global     enable_interrupt
    .global     disable_interrupt

enable_counters:
    MRC         p15, 0, r0, c9, c12, 0  	@ Read PMNC Register
    ORR         r0,  r0,#0x1                    @ set E bit, this enables CCNT and all PMU counters
    MCR         p15, 0, r0, c9, c12, 0  	@ Write PMNC Register

    MRC         p15, 0, r0, c9, c12, 1  	@ Read CNTENS Register
    ORR         r0,  r0,#0x80000000             @ set C bit, enable CCNT
    ORR         r0,  r0,#0xF                    @ set P0-P3 bit, enable PMCNT0-3
    MCR         p15, 0, r0, c9, c12, 1  	@ Write CNTENS Register

    MOV		pc,  lr
    
disable_counters:
    MRC         p15, 0, r0, c9, c12, 0 	 	@ Read PMNC Register
    BIC         r0,  r0,#0x1                    @ clear enable bit
    MCR         p15, 0, r0, c9, c12, 0  	@ Write PMNC Register
    
    MRC         p15, 0, r0, c9, c12, 2  	@ Read CNTENC Register
    ORR         r0,  r0,#0x80000000		@ set C bit, disable CCNT
    ORR         r0,  r0,#0xF                    @ set P0-P3 bit, disable PMCNT0-3
    MCR         p15, 0, r0, c9, c12, 2  	@ Write CNTENC Register

    MOV         pc,  lr

enable_overflow_flag:
    MRC         p15, 0, r0, c9, c12, 3          @ Read FLAG Register
    ORR         r0,  r0, #0x8000000f            @ set C & P0~P3 bit
    MCR         p15, 0, r0, c9, c12,3           @ write flag register

    MOV         pc, lr

disable_overflow_flag:
    MRC         p15, 0, r0, c9, c12, 3          @ Read FLAG Register
    BIC         r0,  r0, #0x8000000f            @ clear C & P0~P3 bit
    MCR         p15, 0, r0, c9, c12,3           @ write flag register

    MOV         pc, lr

read_overflow_flag:
    MRC         p15, 0, r0, c9, c12, 3          @ Read FLAG Register
    ADD         r0, r0, #1
    SUB         r0, r0, #1
    
    MOV         pc, lr

enable_interrupt:
    MRC         p15, 0, r0, c9, c14, 1          @ read Interrupt enable set register
    ORR         r0,  r0, #0x8000000f            @ set C & P0~P3 bit
    MCR         p15, 0, r0, c9, c14, 1          @ Write Interrupt enable set register

    MOV         pc, lr

disable_interrupt:
    MRC         p15, 0, r0, c9, c14, 1          @ read Interrupt enable set register
    BIC         r0,  r0, #0x8000000f            @ clear  C & P0~P3 bit
    MCR         p15, 0, r0, c9, c14, 1          @ Write Interrupt enable set register

    MOV         pc, lr

@r0 -- PMCNTx
@r1 -- event ID
set_event_PMCNT: 
    MCR         p15, 0, r0, c9, c12, 5  	@ write PMNXSEL to select PMCNT0
    MCR         p15, 0, r1, c9, c13, 1  	@ write EVTSEL register
    MOV		pc,  lr

reset_CCNT: 
    MRC 	p15, 0, r0, c9, c12, 0 	@ Read PMNC Register
    ORR		r0,  r0,#0x4			@ set C bit, reset CCNT
    MCR 	p15, 0, r0, c9, c12, 0 	@ Write PMNC Register
    mov         pc,  lr
 	
reset_PMU: 	
    MRC 	p15, 0, r0, c9, c12, 0 	@ Read PMNC Register
    ORR		r0,  r0,#0x6			@ set P and C bit, reset PMU counters
    MCR 	p15, 0, r0, c9, c12, 0 	@ Write PMNC Register
    mov         pc,  lr
 	
set_counter_divider: 
    MRC 	p15, 0, r1, c9, c12, 0 	@ Read PMNC Register
    CMP		r0,  #1
    ORREQ	r1,  r1,#0x8			@ set D bit, count every 64 CPU cycles
    BICNE	r1,  r1,#0x8			@ clear D bit, count every CPU cycle
    MCR 	p15, 0, r1, c9, c12, 0 	@ Write PMNC Register
    mov         pc,  lr
 	
read_CCNT: 	
    MRC 	p15, 0, r0, c9, c13, 0 	@ Read CCNT Register 	
    mov         pc,  lr
 	
@r0: PMCNT[r0] 	
read_PMCNT: 
    MCR		p15, 0, r0, c9, c12, 5	@ write PMNXSEL to select PMCNT[r0]
    MRC		p15, 0, r0, c9, c13, 2	@ Read PMCNT[sel] selected by PMNXSEL
    mov         pc,  lr
	 	
    .end
