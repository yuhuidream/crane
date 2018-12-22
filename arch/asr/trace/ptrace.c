/*
 * arch/marvell/ptrace.c
*
 * This file contain functions to output related information to help debug when exception happen.
 * "bbu_exception_trace" is called in arch/marvell/startup_mrvl.s, which use stack to transport 
 * regs in "struct pt_regs". 
*/

#include "predefines.h"
#include "bbu.h"
#include "ptrace.h"
#include "fault.h"
#include "timer_dec.h"

static void regs_arm_psr_print(struct pt_regs *regs);
static uint8_t excep_back_trace(struct pt_regs *regs, uint32_t entry);
static void regs_stack_check_and_print(uint32_t fp, uint32_t sp);
static void regs_dump_cpu_register(struct pt_regs *regs);
static uint8_t check_fp_is_valid(uint32_t fp, uint32_t sp);
extern void console_shut_down(void);

/* 
 * Global variable to store cpu related registers when exception occures 
*/
volatile struct pt_regs g_pt_regs;  

static const struct pt_regs_offset regoffset_table[] = 
{  
	/* 
	 * Table to store register name and its offset in g_pt_regs 
	*/
	REG_OFFSET_NAME(r0),
	REG_OFFSET_NAME(r1),
	REG_OFFSET_NAME(r2),
	REG_OFFSET_NAME(r3),
	REG_OFFSET_NAME(r4),
	REG_OFFSET_NAME(r5),
	REG_OFFSET_NAME(r6),
	REG_OFFSET_NAME(r7),
	REG_OFFSET_NAME(r8),
	REG_OFFSET_NAME(r9),
	REG_OFFSET_NAME(r10),
	REG_OFFSET_NAME(fp),
	REG_OFFSET_NAME(ip),
	REG_OFFSET_NAME(sp),
	REG_OFFSET_NAME(lr),
	REG_OFFSET_NAME(pc),
	REG_OFFSET_NAME(cpsr),
	REG_OFFSET_END,
};

/*
 * Function to record information when exception occures.
 * (1) registers content
 * (2) function call tree
 * (3) stack content when exception occures
*/
void bbu_exception_trace(struct pt_regs *regs,uint32_t exp_mode, uint32_t fsr, uint32_t far)
{
	uint32_t excep_point;  /* used to trace every caller jump location from callee */

	stress_test_fail("Exception");

#if CONFIG_MMT
	bbu_printf("Enter exception trace, exit as in MMT mode\n\r");
	qf_reschedule();
	dis_feeding_watchdog();
#endif
	console_shut_down();
	
	bbu_printf("\r\nOops......\r\n");
	mdelay(1500);  /* delay 2s so that current bbu_printf finish, avoiding disorder print of UART */

	/* 
	 * Judge what exception happend, record current PC value depend exception happen location 
	*/
	print_dividing_line();
	bbu_printf("\r\n\t\t");
	excep_point = regs->ARM_pc - 4;
	if(exp_mode == 0x01)
	{
		bbu_printf("Undefined exception happen!\r\n");
	}
	else if(exp_mode == 0x02)
	{
		bbu_printf("Prefetch abort happen!\r\n");
		do_PrefetchAbort(far, fsr, regs);
	}
	else if(exp_mode == 0x03)
	{
		bbu_printf("Data abort exception happen!\r\n");
		do_DataAbort(far, fsr, regs);
		excep_point = regs->ARM_pc - 8;
	}
	else if(exp_mode == 0x04)
	{
		bbu_printf("Reserved exception happen!\r\n");
	}
	else if(exp_mode == 0x05)
	{
		bbu_printf("\r\n\t\tAssert Error! Enter SVC mode to debug!\r\n");
	}
	else
	{
		bbu_printf("Something Unknown happen!\r\n");
		stress_test_fail("Unknown Error");
	}
	print_dividing_line();

	/* Output CPU related registers */
	regs_dump_cpu_register(regs);

	print_dividing_line();
	/* CPU PSR analyze */
	regs_arm_psr_print(regs);

	print_dividing_line();
	/* track cause of exception happen */
	(void)excep_back_trace(regs, excep_point);

	print_dividing_line();
	/* check whether stack is overflow or not, and dump stack data */
	regs_stack_check_and_print(regs->ARM_fp, regs->ARM_sp);

	print_dividing_line();
	stress_test_fail("Exception");
	if(exp_mode != 0x05)
	{
		bbu_printf("\r\n\r\nException Error!\r\n");
		bbu_printf("\r\nNot handled! Please Restart...... \r\n");
		bbu_printf("\r\n"BBU_PROMPT);
	#if  defined(CONFIG_MLUCAS) && defined(CONFIG_SMP)
		extern int mlucas_screen_idx;
		int core = smp_processor_id();
		if(QC_getCurrentAO(core) == AO_STRESS && mlucas_screen_idx != -1)
			screen_cmd_ack(SCREEN_TEST_FIAL, mlucas_screen_idx);
	#endif
	#ifdef CONFIG_CR5
    	cp_assert_cp_handler();
	#endif

		for(;;);  /* Let cpu stop here when debug, maybe reset cpu when released */
	}
}


/*
 * For different SVC argument, finish different system call, reserved for future use
*/
void svc_call_handle(struct pt_regs *regs)
{
	uint32_t svc_arg = (*(uint32_t *)(regs->ARM_lr - 4)) && 0x00ffffff;
	//bbu_printf("SVC call argument = %d\r\n", svc_arg);
	switch (svc_arg)
	{
		/* Can add svc system call service below according different arguments */
		case 1:
		{
			bbu_exception_trace(regs, 5, 0, 0);			
			break;
		} 
			
		default:
		{
			break;
		}
	}
}


static void regs_dump_cpu_register(struct pt_regs *regs)
{
	uint32_t index = 0;
	struct pt_regs_offset *p_regoffset = (struct pt_regs_offset *)regoffset_table;

	bbu_printf("\r\nCPU: %d\r\n\r\n",(GetCPUID()-1));
	while(p_regoffset->name != NULL)
	{	
		bbu_printf("%s: 0x%-10x\t", p_regoffset->name, *((uint32_t*)((uint8_t*)regs + p_regoffset->offset)));
		p_regoffset++;

		if((++index)%3 == 0)
			bbu_printf("\r\n");
	}
	bbu_printf("\r\n");
}

/*
 * Check fp in exception point is valid or not in case that some funtion dosen't conform
 * ATPCS with frame pointer, especially assemble code.
*
 * For example, if a funtion conform ATPCS with frame pointer, especially assemble code. 
 * Before each funtion go ahead, should have following similar code to save related register.
 *   mov	ip, sp
 *	  push	{fp, ip, lr, pc}
 *	  sub	fp, ip, #4
*/
static uint8_t check_fp_is_valid(uint32_t fp, uint32_t sp)
{
	/*
	 * "fp" may be changed by code which dose not conform above principle.
	 * so should avoid change "fp" if we want this funtion join in exception back trace.
	 * Following judgement just can find if "fp" is abnormal, but if pass thest judgements
	 * does not show fp is valid.
	*
	 * this is a try-best judgement, which means back trace information may be not corrent under
	 * some circumstance, but it is little probability.
	*/
	uint32_t father_sp;

	if(fp%4 != 0)
	{	
		//bbu_printf("\r\nfp=0x%x is not aligned with 4 bytes\r\n",fp);
		return 0;	
	}
	father_sp = *(uint32_t*)(fp - 8);
	if( father_sp != (fp + 4) || (fp < sp))
	{
		//bbu_printf("father_sp=%x  fp=%x\r\n",father_sp,fp);
		return 0;
	}
	return 1;
}
/*
 * When all the code obey ATPCS with frame pointer, can use FP to back trace
*/
#define EXCEPTION_BACKTRACE_LIMIT 50  /* means can trace up to 50 levels function call tree */
static uint8_t excep_back_trace(struct pt_regs *regs, uint32_t entry)
{
	uint32_t index;
	uint32_t fp;
	uint32_t sp;	
	uint32_t branch_addr;

	fp = regs->ARM_fp;  /* Fetch exception location register FP, delivered from exception entry */
	sp = regs->ARM_sp;
	branch_addr = (uint32_t)get_brch_addr();

	bbu_printf("\r\nBack_Trace:\r\n [Exception start]");
	index = 0;
	do
	{
		if(!check_fp_is_valid(fp, sp))
		{
			bbu_printf("......\r\nBack_Trace: Give up! \r\n");
			bbu_printf("\r\nCurrent function in the call tree doesn't conform ATPCS with frame pointer!\r\n ");
			return 0;
		}
		/* 
		 * Output function call tree to where exception happen, using FP an LR regiser in stack 
		*/
		if(index++%4 == 0 && index/4 >0)
			bbu_printf("\r\n ", entry);
		//bbu_printf("---->[0x%x, 0x%x]", entry, fp);
		bbu_printf("---->[0x%x]", entry);
		
		if(entry == branch_addr)
		{			
			bbu_printf("---->[End: Entry of main]\r\n");
			break;
		}

		entry = *(uint32_t*)(fp - 4)-4;
		sp = *(uint32_t*)(fp - 8);
		fp = *(uint32_t*)(fp - 12);
	} while(index < EXCEPTION_BACKTRACE_LIMIT);

	bbu_printf("\r\nNote:\r\n");
	bbu_printf("  Can use back_trace address above to find which function and which line in final executable file.\r\n");
	bbu_printf("  For example, use \"objdump\" and \"addr2line\" in your toolchain to operate on bbu.elf \r\n");

	return 1;
}

#define EXCEPTION_STACK_DUMP_LIMIT 80
static void regs_stack_check_and_print(uint32_t fp, uint32_t sp)
{
	uint32_t index;

	if(!check_fp_is_valid(fp, sp))
	{
		fp = sp + (EXCEPTION_STACK_DUMP_LIMIT-1)*4 ;	
		bbu_printf("\r\nRegister fp is not used as a frame pointer");
		bbu_printf("\r\nDump recent %d words from stack top:\r\n",EXCEPTION_STACK_DUMP_LIMIT);
	}
	/* Output stack value to help judge probem origin */
	index = 0;
	bbu_printf("\r\nException stack(sp to fp):  0x%x to 0x%x, ascending", sp, fp);
	while(fp >= sp)
	{
		if((index++)%5 == 0)
		{	
			bbu_printf("\r\n");
			bbu_printf("(%d) 0x%x: ", (index/5 + 1), sp);
		}	
		bbu_printf("0x%-10x", *(uint32_t *)sp);
		sp = sp + 4;
		if( index > EXCEPTION_STACK_DUMP_LIMIT )
		{
			bbu_printf("\r\n......");
			break;
		}
	}
	bbu_printf("\r\n");
}


/*
 * ARM PSR analyze
*/
static void regs_arm_psr_print(struct pt_regs *regs)
{
	bbu_printf("\r\nPSR: ");

	/*
	 * Flag "NZCV", capitalization means flag available.
	 * In the contrary, lowercase means invaild.
	*/
	if(regs->ARM_cpsr & PSR_N_BIT)
		bbu_printf("N");
	else
		bbu_printf("n");

	if(regs->ARM_cpsr & PSR_Z_BIT)
		bbu_printf("Z");
	else
		bbu_printf("z");

	if(regs->ARM_cpsr & PSR_C_BIT)
		bbu_printf("C");
	else
		bbu_printf("c");

	if(regs->ARM_cpsr & PSR_V_BIT)
		bbu_printf("V");
	else
		bbu_printf("v");
	bbu_printf("  ");

	/* CPU big or little endian check */
	if(regs->ARM_cpsr & PSR_E_BIT)
		bbu_printf("Big-endian");
	else
		bbu_printf("Little-endian");

	/* Exception point interrupt status */
	bbu_printf("  IRQs:");
	if(regs->ARM_cpsr & PSR_I_BIT)
		bbu_printf("off");
	else
		bbu_printf("on");

	bbu_printf("  FIQs:");
	if(regs->ARM_cpsr & PSR_F_BIT)
		bbu_printf("off");
	else
		bbu_printf("on");

	/* ARM instrucion set state */
	bbu_printf("  ISA:");
	if(isa_mode(regs) == PSR_IST_ENCODE_ARM)
		bbu_printf("ARM");
	else if(isa_mode(regs) == PSR_IST_ENCODE_Thumb)
		bbu_printf("Thumb");
	else if(isa_mode(regs) == PSR_IST_ENCODE_Jazelle)
		bbu_printf("Jazelle");
	else if(isa_mode(regs) == PSR_IST_ENCODE_ThumbEE)
		bbu_printf("ThumbEE");	
	else
		bbu_printf("Unknown");	

	/* CPU mode in exception point */
	bbu_printf("  Mode:");
	if(processor_mode(regs) == USR_MODE)
		bbu_printf("USR");
	else if(processor_mode(regs) == FIQ_MODE)
		bbu_printf("FIQ");
	else if(processor_mode(regs) == IRQ_MODE)
		bbu_printf("IRQ");
	else if(processor_mode(regs) == SVC_MODE)
		bbu_printf("SVC");
	else if(processor_mode(regs) == ABT_MODE)
		bbu_printf("ABT");
	else if(processor_mode(regs) == UND_MODE)
		bbu_printf("UND");	
	else if(processor_mode(regs) == SYSTEM_MODE)
		bbu_printf("SYSTEM");	
	else
		bbu_printf("UNPREDICTABLE");	

	bbu_printf("\r\n\r\nNote: For flag \"NZCV\",lowercase means invaild.\r\n");
}


