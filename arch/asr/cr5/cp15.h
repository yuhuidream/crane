#ifndef _H_CP15_H_
#define _H_CP15_H_


/* ***************************************************************************
 * Usage :
 * cp15_read(0, c0, c1, 0, value);
 ****************************************************************************/
#define cp15_read(opcode1,primary_reg, secondary_reg, opcode2, value) \
	__asm__ __volatile__("mrc	p15, " #opcode1 ", %0, " #primary_reg ", \
	                     " #secondary_reg "," #opcode2 "\n"	: "=r" (value) \
	                     		:: "memory");

#define cp15_write(opcode1, primary_reg, secondary_reg, opcode2, value) \
	__asm__ __volatile__("mcr	p15, " #opcode1 ", %0, " #primary_reg ", \
	                     " #secondary_reg ", " #opcode2 "\n"	: : "r" (value)\
	                     	 : "memory");

/* ***************************************************************************
 * Usage :
 * cp15_reg_read(c0, c1, 0, value);
 ****************************************************************************/
#define cp15_reg_read(primary_reg, secondary_reg, opcode, value) \
	__asm__ __volatile__("mrc	p15, 0, %0, " #primary_reg ", " #secondary_reg ",\
	                     " #opcode "\n"	: "=r" (value)	:: "memory");

#define cp15_reg_write(primary_reg, secondary_reg, opcode, value) \
	__asm__ __volatile__("mcr	p15, 0, %0, " #primary_reg ", " #secondary_reg ", \
	                     " #opcode "\n"	: : "r" (value) : "memory");

/* ***************************************************************************
 * Usage :
 * cp15_reg_read2(0, 1, 0, value);
 ****************************************************************************/
#define cp15_reg_read2(primary_reg, secondary_reg, opcode, value) \
	__asm__ __volatile__("mrc	p15, 0, %0, c" #primary_reg ", c" #secondary_reg ",\
	                     " #opcode "\n"	: "=r" (value)	:: "memory");

#define cp15_reg_write2(primary_reg, secondary_reg, opcode, value) \
	__asm__ __volatile__("mcr	p15, 0, %0, c" #primary_reg ", c" #secondary_reg ",\
	                     " #opcode "\n"	: : "r" (value) : "memory");

/* ***************************************************************************
 * Usage :
 * cp15_reg_read3("c0", "c1", "0", value);
 ****************************************************************************/
#define cp15_reg_read3(primary_reg, secondary_reg, opcode, value) \
	__asm__ __volatile__("mrc	p15, 0, %0, " primary_reg ", " secondary_reg",\
	                     " opcode "\n"	: "=r" (value)	:: "memory");

#define cp15_reg_write3(primary_reg, secondary_reg, opcode, value) \
	__asm__ __volatile__("mcr	p15, 0, %0, " primary_reg ", " secondary_reg",\
	                     " opcode "\n"	: : "r" (value)	 : "memory");

/****************************************************************************/


/******************* CP15 registers read and write macros *******************/
#define get_main_id(x)     cp15_reg_read(c0, c0, 0, x)
#define get_cache_type(x)  cp15_reg_read(c0, c0, 1, x)
#define get_tcm_type(x)    cp15_reg_read(c0, c0, 2, x)
#define get_tlb_type(x)    cp15_reg_read(c0, c0, 3, x)
#define get_silicon_id(x)  cp15_reg_read(c0, c0, 7, x)

#define get_processor_id(x)         cp15_reg_read(c0, c0, 5, x)
#define get_processor_id_ext(x)     cp15_read(1, c15, c12, 5, x)

#define get_ttb0(x)        cp15_reg_read (c2, c0, 0, x)
#define set_ttb0(x)        cp15_reg_write(c2, c0, 0, x)

#define get_ttb0(x)        cp15_reg_read (c2, c0, 0, x)
#define set_ttb0(x)        cp15_reg_write(c2, c0, 0, x)

#define get_ttb1(x)        cp15_reg_read (c2, c0, 1, x)
#define set_ttb1(x)        cp15_reg_write(c2, c0, 1, x)

#define get_fcse(x)        cp15_reg_read (c13, c0, 0, x)
#define set_fcse(x)        cp15_reg_write(c13, c0, 0, x)

#define get_sys_ctrl(x)    cp15_reg_read (c1, c0, 0, x)
#define set_sys_ctrl(x)    cp15_reg_write(c1, c0, 0, x)

#define get_aux_ctrl(x)    cp15_reg_read (c1, c0, 1, x)
#define set_aux_ctrl(x)    cp15_reg_write(c1, c0, 1, x)

#define get_ttb_ctrl(x)    cp15_reg_read (c2, c0, 2, x)
#define set_ttb_ctrl(x)    cp15_reg_write(c2, c0, 2, x)

#define get_copro_access(x)  cp15_reg_read (c1, c0, 2, x)
#define set_copro_access(x)  cp15_reg_write(c1, c0, 2, x)

#define get_domain_ctrl(x)   cp15_reg_read (c3, c0, 0, x)
#define set_domain_ctrl(x)   cp15_reg_write(c3, c0, 0, x)

#define get_ns_access_ctrl(x) cp15_reg_read (c1, c1, 2, x)
#define set_ns_access_ctrl(x) cp15_reg_write(c1, c1, 2, x)

#if 0 //L1 cache lockdown not supported in Cortex-A9
#define get_icache_lockdown(x)  cp15_reg_read (c9, c0, 1, x)
#define set_icache_lockdown(x)  cp15_reg_write(c9, c0, 1, x)

#define get_dcache_lockdown(x)  cp15_reg_read (c9, c0, 0, x)
#define set_dcache_lockdown(x)  cp15_reg_write(c9, c0, 0, x)
#endif
#define dsb_cp15() cp15_reg_write(c7, c10, 4, 0)
#define dmb_cp15() cp15_reg_write(c7, c10, 5, 0)
#define isb_cp15() cp15_reg_write(c7, c5,  4, 0)

#endif