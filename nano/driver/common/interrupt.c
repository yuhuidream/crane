#include "predefines.h"
#include "system_mrvl.h"
#include "interrupt.h"
#include "ICU_T.h"
#include "ICU.h"
#include "ICU2_T.h"
#include "gic_dec.h"
#include "bbu.h"

#ifndef CONFIG_NANO_NO_INT
void icu_target_set(uint32_t interrupt, uint32_t cpu, uint32_t set){
   uint32_t mask;
   /* get real interrupt ID on ICU, input is ID on GIC */
   interrupt = interrupt-32;
   mask = ICU->INT_CFGx[interrupt];
   if(set){
      mask |= (1 << (cpu+6));
   }else{
      mask &= ~(1 << (cpu+6));
   }
   ICU->INT_CFGx[interrupt] = mask;
}

void icu_priority_set(uint32_t int_num, uint32_t priority){
   ICU->INT_CFGx[int_num-32] |= (priority & 0xF);
}

uint32_t icu_priority_get(uint32_t int_num){
   return (ICU->INT_CFGx[int_num-32] & 0xF);
}

void icu_set_irq(uint32_t int_num){
   ICU->INT_CFGx[int_num-32] |= BIT_4;
}

void icu_set_fiq(uint32_t int_num){
   ICU->INT_CFGx[int_num-32] &= ~BIT_4;
}

void INT_Enable(uint32_t int_num, uint32_t irq0_fiq1, uint32_t priority)
{
    int coreId;

    /* In single-core mode, we route the interrupt to the running core  */
    coreId = smp_processor_id();

    GIC_ISR_ENABLE(int_num, priority, coreId);

    if(int_num >= 32){
    if(priority == 0)           //if priority is 0, it mask interrupt.
        priority = 1;

/*ICU route to cores for wake up at SMP mode. 
  When GIC is enabled i.e. the ICU pass-through is enabled, ICU interrupt is bypassed.
  At reset GIC's pass-through bit is disabed by default, ICU interrupt can
  go through GIC->GlobalMask(cleared at wake)->CPU. but CPSR.IF is disabed,
  so CPU is wake up but it will not handle the interrupt. At that time, we restore
  the GIC context and it will enable the icu_pass-through switch. after that,
  we enable CPSR interrupt, interrupt is go through GIC to CPU ISR.
  */
/*If we want to wake up two cores at wake up, we route both 
  FIXME: we may fix the wake up sequence for simple in ca9_idle.c later*/
/* We route the interrupt in ICU as we do on GIC */
        icu_target_set(int_num, coreId, 1);
        icu_priority_set(int_num, priority);
        if(irq0_fiq1)
           icu_set_fiq(int_num);
        else
           icu_set_irq(int_num);

    }
}



void ISR_Connect(uint32_t int_num, void (*ISR_Handle)(void*), void *ISR_Arg)
{
    GIC_ISR_CONNECT(int_num, ISR_Handle, ISR_Arg);
}

#endif

extern void gic_isr(void);

void AP_FIQ_isr(void)
{
      gic_isr();
}


/* 
NOTE1: 
  Here we don't cover FIQs which are normally reserved for a single interrupt source that 
  requires a fast response time, e.g. DMA specially used to move blocks of memory, timer validation
  and hard real-time appliations. Thus, there are usually 1~2 FIQs enabled. If necessary, it should be handled in
  start.S directly to save the extra jump overhead.
 
  In one word, the FIQ exception is used for 1~2 specific applications, leaving the IRQ exception for
  more general operating system activities.
 
  In MMP series, the priority is only 4bits (0x0~0xf), not like nevo has same number of priority as interrupt numbers
 
NOTE2: Combined Interrupt Entries of MMP3
  Interrupts[58],[57],[55], [51],[42], [35],[30],[18], [17],[8],[6], [5], and [4] are combined interrupt entries. 
  For each of those interrupt entries, there are multiple interrupt inputs that are masked and ORed. By 
  default, all of those interrupt inputs are masked, software needs to clear the proper interrupt mask 
  bits to see the interrupt. There are separate interrupt status registers for each of these interrupt 
  entries.
  For those interrupts, call the INT_Enable and ISR_Connect functions with equivalent IRQ number directly.
 
NOTE3: For DMA interrupts. 
  DMA has its own secondary interrupt handling functions. 
  */
  
