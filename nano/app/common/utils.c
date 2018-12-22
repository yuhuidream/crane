#include "bbu.h"
#include "utils.h"
#include "timer_dec.h"
#include <string.h>
#include "cp15.h"
#include "print.h"
#include "set_mmu.h"

#ifndef CONFIG_NANO
#include "perf_counter.h"
void POST_CMD2CLI (const char *cmdstr) 
{

  //const char *cmdstr = "ppset 100 1";
  ComCliEvt *sce = Q_NEW(ComCliEvt, COM_RX_CMD_READY_SIG);
  sce->len = sizeof(cmdstr);
  sce->source = 0;  /* differ with command join in auto screen test */
  strcpy((char*)sce->CliBuffer, cmdstr);
  QACTIVE_POST(AO_Cli, (QEvent *)sce, 0);

}
#endif

uint32_t conv_hex( char *s )
{
    uint32_t ui;
    char    *p = s, c;
    int     dcnt;
    
    for ( ui=0, dcnt=0; *p!=0  && dcnt < 10; p++, dcnt++ )
    {
        c = *p;
        if ( c >='0' && c <= '9' )
            ui = (ui<<4) +(c - '0');
        else 
        {
            if ( c >= 'a' && c <= 'f' )
                c = c -'a'+'A';
            if ( c >= 'A' && c <= 'F' )
                ui = (ui<<4) + ( c-'A'+10);
        }
    }
    
    return(ui);
}

uint32_t conv_dec( char *s )
{
    uint32_t    ui;
    char    *p = s, c;
    int     dcnt;
    
    for ( ui=0, dcnt=0; *p!=0  && dcnt < 8; p++, dcnt++ )
    {
        c = *p;
        if ( c >='0' && c <= '9' )
            ui = (ui*10) +(c - '0');
    }
    
    return(ui);
}

#ifdef CONFIG_CR5
/* String utilities. */
uint32_t XpIsHex(char * pStr)
{
    if(pStr[0] == '0' && (pStr[1] == 'X' || pStr[1] == 'x'))
        return 1;
    return 0;
}

uint32_t XpStrToHex(char * pStr)
{
    int len = strlen(pStr), i;
    uint32_t val = 0;
    /* prefix by 0x/0X */
    for(i = 2; i < len; i++)
    {
        if(pStr[i] >= '0' && pStr[i] <= '9')
            val = (val << 4) + (pStr[i] - '0');
        else if(pStr[i] >= 'a' && pStr[i] <= 'f' )
            val = (val << 4) + (pStr[i] - 'a' + 10);
        else if(pStr[i] >= 'A' && pStr[i] <= 'F' )
            val = (val << 4) + (pStr[i] - 'A' + 10);
    }
    return val;
}

uint32_t XpStrToDecimal(char * pStr)
{
    int len = strlen(pStr), i;
    uint32_t val = 0;

    for(i = 0; i < len; i++)
    {
        if(pStr[i] >= '0' && pStr[i] <= '9')
            val = (val * 10) + (pStr[i] - '0');
        else
            DEBUGMSG(DBG_LEVEL,("Xp err: invalid decimal value\r\n"));
    }
    return val;
}

uint32_t XpStrToValue(char * pStr)
{
    if(XpIsHex(pStr))
    {
        return XpStrToHex(pStr);
    }
    else
    {
        return XpStrToDecimal(pStr);
    }
}
#endif

#ifndef CONFIG_NANO
/**
 * print_log = 1: print the MCPS log, 0: don't print the MCPS log
 **/
int measure_core_frequency(int print_log){
   uint32_t time, cycle, freq;
   int total_cycle = 0x100000;
//   int loop_num = total_cycle >> 1;   //each loop use one cycles
   int loop_num = total_cycle;
   if(!is_cycle_counter_enabled()) /* if cycle counter not enabled */
      StartCycleCounter();  /* enable, clear divider */
   time = timer_get_count(0);
   counter_disable(CYCLE_COUNTER_ENABLE_BASE);
   cycle = ReadCycleCounter();
   counter_enable(CYCLE_COUNTER_ENABLE_BASE);
   __asm__ __volatile__(
      "mov r6, %0\n"
      ".align 4\n"
      "loop: subs r6, r6, #1\n"
      "bne loop\n"
      :
      :"r"(loop_num)
      :"r6"
   );
   time = timer_get_count(0) - time;
   counter_disable(CYCLE_COUNTER_ENABLE_BASE);
   cycle = ReadCycleCounter() - cycle;
   if(is_cycle_counter_divider_set())
      cycle *= 64;
   counter_enable(CYCLE_COUNTER_ENABLE_BASE);
   freq = total_cycle/1000;
   freq = freq*TIMER0_TICK_PER_MSEC;
   freq = freq/time;
   if(print_log){
      bbu_printf("\n\rmcps--> MCPS of 0x%X self-decrease to 0 (1 per step) is"
              " : %d MCPS(MHz)\n\r",loop_num,freq);
      bbu_printf("mcps--> Elapse timer count is %d\n\r", time);
   }
   freq = cycle/1000;
   freq = freq*TIMER0_TICK_PER_MSEC;
   freq = freq/time;
   if(print_log){
      bbu_printf("mcps--> Actual processer cycle is 0x%X\n\r",cycle);
      bbu_printf("mcps--> MCPS(Core Frequecy) of current OP is"
                 " : %d MCPS(MHz)\n\r",freq);
   }
   return freq;
}


/* Get the physical address according to the virtual address............... */
uint32_t get_physical(uint32_t va){
   uint32_t cp15_cr, table_base_0, ttb_ctl;
   uint32_t tab_entry, entry_offset;
   uint32_t phy_addr;
   uint32_t *mmu_table;
   /* get the CP15 control register */ 
   get_sys_ctrl(cp15_cr);
   if((cp15_cr & xlli_control_mmu) == 0)
      return MMU_OFF;
   
   /* get Translation Table Base 0 Register */
   get_ttb0(table_base_0);
   get_ttb_ctrl(ttb_ctl);    /* get Translation Table Base Control Register */
   ttb_ctl &= 0x7;           /* the width of the base address held in TTBR0 */
   table_base_0 &= ~((1<<(14-ttb_ctl))-1);

   entry_offset = va >> 20;
   mmu_table = (uint32_t *)table_base_0;
   tab_entry = mmu_table[entry_offset];
   if(tab_entry == 0)
      return INVALID_VIRTUAL_ADDRESS;

   /* Is bit 0 set? */
   if(tab_entry & BIT_0){     /* There is a coarse table entry to deal with */
      mmu_table = (uint32_t *)(tab_entry & 0xFFFFFC00);   /* clear bits 9:0 */
      entry_offset = (va & 0xFF000)>>12;              /* Isolate bits 19:12 */
      tab_entry = mmu_table[entry_offset];
      if(0 == tab_entry)
         return INVALID_VIRTUAL_ADDRESS;
      if(tab_entry & BIT_1){                            /* Is a small page? */
         tab_entry &= 0xFFFFF000;
         phy_addr = tab_entry | (va & 0x00000FFF);
      }
      else{                                  //large page
         tab_entry &= 0xFFFF0000;
         phy_addr = tab_entry | (va & 0x0000FFFF);
      }
   }
   else{     /*Level 1 translation entery detected - easy translation below!*/
      phy_addr = ((va& 0x000FFFFF) | (tab_entry & 0xFFF00000));
   }
   
   return phy_addr;
}

void page_table_walk(uint32_t vaddr){
   uint32_t cp15_cr, table_base_0, ttb_ctl;
   uint32_t tab_entry, entry_offset, tex_base;
   uint32_t phy_addr;
   uint32_t *mmu_table;
   /* get the CP15 control register */ 
   get_sys_ctrl(cp15_cr);
   if((cp15_cr & xlli_control_mmu) == 0){
      bbu_printf("MMU is disabled\n\r");
      return;
   }
   
   /* get Translation Table Base 0 Register */
   get_ttb0(table_base_0);
   get_ttb_ctrl(ttb_ctl);    /* get Translation Table Base Control Register */
   ttb_ctl &= 0x7;           /* the width of the base address held in TTBR0 */
   table_base_0 &= ~((1<<(14-ttb_ctl))-1);
   entry_offset = vaddr >> 20;
   mmu_table = (uint32_t *)table_base_0;
   tab_entry = mmu_table[entry_offset];
   bbu_printf("\n\rPage table walk of virtual address: 0x%08X\n\r", vaddr);
   bbu_printf("-->Translation Table Base: 0x%08X\n\r", table_base_0);
   bbu_printf("-->First level descriptor address: 0x%08X\n\r", 
              &(mmu_table[entry_offset]));
   bbu_printf("-->First level descriptor: 0x%08X\n\r", tab_entry);
   if(tab_entry & BIT_0){
      bbu_printf("-->Need second level walk\n\r");
      mmu_table = (uint32_t *)(tab_entry & 0xFFFFFC00);   /* clear bits 9:0 */
      entry_offset = (vaddr & 0xFF000)>>12;           /* Isolate bits 19:12 */
      tab_entry = mmu_table[entry_offset];
      bbu_printf("-->Second level descriptor address: 0x%08X\n\r",
                &(mmu_table[entry_offset]));
      bbu_printf("-->Second level descriptor: 0x%08X\n\r", tab_entry);
      bbu_printf("\n\r-->Page table walk done: ");
      if(0 == tab_entry){
         bbu_printf("invalid virtual address\n\r");
         return;
      }
      if(tab_entry & BIT_1){                            /* Is a small page? */
         bbu_printf("in a Small page\n\r\n\r");
         phy_addr = (tab_entry & 0xFFFFF000) | (vaddr & 0x00000FFF);
         tex_base = SPAGE_TEX_BASE;
      }
      else{                                  //large page
         bbu_printf("in a Large page\n\r\n\r");
         phy_addr = (tab_entry&0xFFFF0000) | (vaddr & 0x0000FFFF);
         tex_base = LPAGE_TEX_BASE;
      }
   }else{
      bbu_printf("\n\r-->Page table walk done: in a Section\n\r\n\r");
      phy_addr = ((vaddr& 0x000FFFFF) | (tab_entry & 0xFFF00000));
      tex_base = SECTION_TEX_BASE;
      
   }
   bbu_printf("-->Physical address: 0x%08X\n\r", phy_addr);
   bbu_printf("-->Memory attribute: TEX: 0x%X, CB: 0x%X\n\r", 
           (tab_entry>>tex_base) & 0x7, (tab_entry>>2)&3);
}

void qf_reschedule(void)
{
#ifdef CONFIG_SMP
   int core_id = smp_processor_id();
   QF_RESTORE_CONTEXT(core_id);
#else
   QF_RESTORE_CONTEXT();
#endif
}
#endif

uint32_t check_reg(uint8_t *reg_argu){
   if((strcmp((const char*)reg_argu, "/r") == 0) || 
      (strcmp((const char*)reg_argu, "--regdump") == 0))
   {
       return 1;
   }
   return 0;
};


#ifdef CONFIG_NANO
/*
 * some standard library functions, in NANO BBU, we discard standard libraries
 */
int
strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2++)
		if (*s1++ == 0)
			return (0);
	return (*(unsigned char *)s1 - *(unsigned char *)--s2);
}

size_t
strspn(const char *s1, const char *s2)
{
	const char *p = s1, *spanp;
	char c, sc;

	/*
	 * Skip any characters in s2, excluding the terminating \0.
	 */
cont:
	c = *p++;
	for (spanp = s2; (sc = *spanp++) != 0;)
		if (sc == c)
			goto cont;
	return (p - 1 - s1);
}
#define	wsize	sizeof(word)
#define	wmask	(wsize - 1)
typedef	long word;	

void *
memcpy(void *dst0, const void *src0, size_t length)
{
	char *dst = dst0;
	const char *src = src0;
	size_t t;

	if (length == 0 || dst == src)		/* nothing to do */
		goto done;

	/*
	 * Macros: loop-t-times; and loop-t-times, t>0
	 */
#define	TLOOP(s) if (t) TLOOP1(s)
#define	TLOOP1(s) do { s; } while (--t)

	if ((unsigned long)dst < (unsigned long)src) {
		/*
		 * Copy forward.
		 */
		t = (long)src;	/* only need low bits */
		if ((t | (long)dst) & wmask) {
			/*
			 * Try to align operands.  This cannot be done
			 * unless the low bits match.
			 */
			if ((t ^ (long)dst) & wmask || length < wsize)
				t = length;
			else
				t = wsize - (t & wmask);
			length -= t;
			TLOOP1(*dst++ = *src++);
		}
		/*
		 * Copy whole words, then mop up any trailing bytes.
		 */
		t = length / wsize;
		TLOOP(*(word *)dst = *(word *)src; src += wsize; dst += wsize);
		t = length & wmask;
		TLOOP(*dst++ = *src++);
	} else {
		/*
		 * Copy backwards.  Otherwise essentially the same.
		 * Alignment works as before, except that it takes
		 * (t&wmask) bytes to align, not wsize-(t&wmask).
		 */
		src += length;
		dst += length;
		t = (long)src;
		if ((t | (long)dst) & wmask) {
			if ((t ^ (long)dst) & wmask || length <= wsize)
				t = length;
			else
				t &= wmask;
			length -= t;
			TLOOP1(*--dst = *--src);
		}
		t = length / wsize;
		TLOOP(src -= wsize; dst -= wsize; *(word *)dst = *(word *)src);
		t = length & wmask;
		TLOOP(*--dst = *--src);
	}
done:
	return (dst0);
}

/* Not clear what this function is used for, just give a empty function here
  */
void
abort(void)
{
   bbu_printf("In Abort function\n\r");
   while(1);
}
#endif
