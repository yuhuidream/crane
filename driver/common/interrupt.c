#include "predefines.h"
#include "system_mrvl.h"
#include "interrupt.h"
#include "ICU_T.h"
#include "ICU.h"
#include "ICU2_T.h"
#include "bbu.h"

#define INT_ENABLE_BIT    BIT_5

void icu_target_set(uint32_t interrupt, uint32_t cpu, uint32_t set){
   uint32_t mask;
   (void)cpu;
   mask = ICU->INT_CFGx[interrupt];
   if(set){
      mask |= INT_ENABLE_BIT;
   }else{
      mask &= ~INT_ENABLE_BIT;
   }
   ICU->INT_CFGx[interrupt] = mask;
}
//
//void icu_target_set_all(uint32_t interrupt)
//{
//   uint32_t mask;
//   mask = ICU->INT_CFGx[interrupt];
//   mask |= (((1<<CORE_NUM)-1)<<6);
//   ICU->INT_CFGx[interrupt] = mask;
//}

void icu_target_clear(uint32_t interrupt){
   uint32_t mask;
   mask = ICU->INT_CFGx[interrupt];
   mask &= ~INT_ENABLE_BIT;
   ICU->INT_CFGx[interrupt] = mask;
}

uint32_t icu_target_get(uint32_t interrupt){
   uint32_t mask;
   mask = ICU->INT_CFGx[interrupt];
   mask = mask >> 6;
   mask &= (BIT_6 | BIT_5);
   return mask;
}

void icu_priority_set(uint32_t int_num, uint32_t priority){
   ICU->INT_CFGx[int_num] |= (priority & 0xF);
}

uint32_t icu_priority_get(uint32_t int_num){
   return (ICU->INT_CFGx[int_num] & 0xF);
}

void icu_set_irq(uint32_t int_num){
   ICU->INT_CFGx[int_num] |= BIT_4;
}

void icu_set_fiq(uint32_t int_num){
   ICU->INT_CFGx[int_num] &= ~BIT_4;
}

void INT_Enable(uint32_t int_num, uint32_t irq0_fiq1, uint32_t priority)
{
    int coreId;
/**
 * In non-secure world, the non-secure access to IPR will be (BIT_5 | value)
 * So if the value of priority is 15(most of our interrupt is using value)
 * The actual set priority of the interrupt is 16+15=31
 * That won't be higher than the value in PMR, the interrupt won't to sent 
 * to CPU interface, therefore, here we just LSR the "priority"  by 1 offset 
 * to increase the interrupt priority
 */
#ifdef CONFIG_TZ
    priority = (priority >> 1);
#endif

#ifdef CONFIG_SMP   
  /* In Dual-core mode, we first check if this interrupt is tied to a specific AO
     If yes, we route the interrupt to the core which the AO belong to
     If no, we route the interrrupt to core 0 as default */
    coreId = QC_irqRelatedTo(int_num);
    if(coreId == INVALID_CORE)
       coreId = 0;
    if(IRQ_MMP_CORESIGHT_AP1 == int_num)
       coreId = 1;
#else  
    /* In single-core mode, we route the interrupt to the running core  */
    coreId = smp_processor_id();
#endif

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


void INT_Disable(uint32_t int_num )
{
    icu_target_clear(int_num);
    icu_priority_set(int_num, 0);
}

void ISR_Connect(uint32_t int_num, void (*ISR_Handle)(void*), void *ISR_Arg)
{
    icu_isr_connect(int_num, ISR_Handle, ISR_Arg);
}

void ISR_Disconnect(uint32_t int_num)
{
    icu_isr_disconnect(int_num);
}


void *ICU_ISR_JumpTbl[INT_NUM];
void *ICU_ISR_ArgTbl[INT_NUM];

void icu_init(void){
   int i;
  for(i = 0; i < INT_NUM; i++){
     ICU_ISR_JumpTbl[i] = NULL;
     ICU_ISR_ArgTbl[i] = NULL;
#ifdef CONFIG_CA7
     ICU->INT_CFGx[i] &= ~BIT_6;
#else
     ICU->INT_CFGx[i] &= ~BIT_5;
#endif
  }

#ifdef CONFIG_CA7
   ICU->MOHAWK_GBL_IRQ_MSK |= 0x3;
#else
   ICU->SEAGULL_GBL_IRQ_MSK |= 0x3; //Global mask IRQ and FIQ
#endif
}


void icu_isr_connect(uint32_t int_num, void (*ISR_Handle)(void*), void *ISR_Arg){
   ICU_ISR_JumpTbl[int_num] = (void *)ISR_Handle;
   ICU_ISR_ArgTbl[int_num] = ISR_Arg;
}

void icu_isr_disconnect(uint32_t int_num){
   ICU_ISR_JumpTbl[int_num] = NULL;
   ICU_ISR_ArgTbl[int_num] = NULL;
}

void irq_glb_enable(void)
{
#ifdef CONFIG_CA7
   ICU->MOHAWK_GBL_IRQ_MSK &= ~BIT_1;
#else
   ICU->SEAGULL_GBL_IRQ_MSK &= ~BIT_1;
#endif     
}

void irq_glb_disable(void)
{
#ifdef CONFIG_CA7
   ICU->MOHAWK_GBL_IRQ_MSK |= BIT_1;
#else
   ICU->SEAGULL_GBL_IRQ_MSK |= BIT_1;
#endif    
}

void fiq_glb_enable(void)
{
#ifdef CONFIG_CA7
   ICU->MOHAWK_GBL_IRQ_MSK &= ~BIT_0;
#else
   ICU->SEAGULL_GBL_IRQ_MSK &= ~BIT_0;
#endif
}

void fiq_glb_disable(void)
{
#ifdef CONFIG_CA7
   ICU->MOHAWK_GBL_IRQ_MSK |= BIT_0;
#else
   ICU->SEAGULL_GBL_IRQ_MSK |= BIT_0;
#endif
}

int GetIntStatus_IRQ()
{
    volatile uint32_t reg;
    int int_num = IRQ_MMP_NONE;  

#ifdef CONFIG_CA7
    reg = ICU->MOHAWK_IRQ_SEL_INT_NUM;
#else
    reg = ICU->SEAGULL_IRQ_SEL_INT_NUM;
#endif

    /* checking the pending bit */
    if(reg & INT_PENDING_BIT){
       /* mask out the pending bit to get low 5 bit INT num */
       int_num = reg & 0x3F;
    }

    return int_num;
}

int GetIntStatus_FIQ()
{
    volatile uint32_t reg;
    int int_num = IRQ_MMP_NONE;

#ifdef CONFIG_CA7
    reg = ICU->MOHAWK_FIQ_SEL_INT_NUM;
#else
    reg = ICU->SEAGULL_FIQ_SEL_INT_NUM;
#endif

    /* checking the pending bit */
    if(reg & INT_PENDING_BIT){
       /* mask out the pending bit to get low 5 bit INT num */
       int_num = reg & 0x3F;
    }

    return int_num;
}

void icu_irq_isr(void)
{
#if 1
    int int_num;
    void (*ISR_Handle)(void*);
    void *ISR_Arg;
    
    /* read interrupt status */     
    int_num = GetIntStatus_IRQ();
    
    /* checking the pending bit */
    if ( int_num !=  IRQ_MMP_NONE )
    {
        /* get ISR entry from table and call it */
            //DEBUGMSG(MMP2_DBG_LEVEL,("IRQ%d\n",int_num));
        ISR_Handle =  (void (*)())(ICU_ISR_JumpTbl[int_num]);
        ISR_Arg = (void *)ICU_ISR_ArgTbl[int_num];

        if ( ISR_Handle )
            (ISR_Handle)(ISR_Arg);
    }
#else
    airq_isr();
#endif
}

void AP_FIQ_isr(void)
{
    int int_num;

    void (*ISR_Handle)(void*);
    void *ISR_Arg;

    /* read interrupt status */     
    int_num = GetIntStatus_FIQ();

    /* checking the pending bit */
    if ( int_num != IRQ_MMP_NONE )
    {
        
        /* get ISR entry from table and call it */
        ISR_Handle =  (void (*)())(ICU_ISR_JumpTbl[int_num]);
        ISR_Arg = (void *)ICU_ISR_ArgTbl[int_num];
        
        if ( ISR_Handle )
            (ISR_Handle)(ISR_Arg);
    }
}

void irq_target_clear(int irq)
{
  icu_target_clear(irq);
}

void irq_target_set(int irq, int target)
{
  icu_target_set(irq, target, 1);
}

int irq_target_get(int irq)
{
  return icu_target_get(irq);
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
  
