/*
 * arch/marvell/trace/fault.h
*
 * Reporting exceptions taken to PL1 modes
*/

struct fsr_info 
{
	uint8_t	(*fn)(uint32_t addr, uint32_t fsr, struct pt_regs *regs);  /* Used to resume from certain fault, reserved for future use */
	const char *name;
} ;

void do_DataAbort(uint32_t addr, uint32_t fsr, struct pt_regs *regs);
void do_PrefetchAbort(uint32_t addr, uint32_t fsr, struct pt_regs *regs);

