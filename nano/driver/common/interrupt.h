#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

//#define ICU_AS_PRIMARY_IC

#define   IPC_PRIORITY       15
#define   DMA_PRIORITY      15

// definition for bit 4 of ICU_INT_0_63_CONF
//#define   FIQ_ROUTE      0x0
//#define   IRQ_ROUTE      0x1

#define   FIQ_ROUTE          0x1
#define   IRQ_ROUTE          0x0
#define   INT_NUM      64

// ICU Hardware Interrupt Assignment
// Description   INT REQ   APB signal   Comments

#define IRQ_MMP_NONE            (-1)
#define IRQ_MMP_CP              (0 +32)    
#define IRQ_MMP_SSP2            (1 +32)    
#define IRQ_MMP_SSP1            (2 +32)    
#define IRQ_MMP_SSP0            (3 +32)    
#define IRQ_MMP_PMIC_INT        (4 +32)    
#define IRQ_MMP_RTC_INT         (5 +32)    
#define IRQ_MMP_RTC_ALARM       (6 +32)    
#define IRQ_MMP_TWSI1           (7 +32)    
#define IRQ_MMP_GPU             (8 +32)    
#define IRQ_MMP_KEYPAD          (9 +32)    
#define IRQ_MMP_DMA2            (10+32)    
#define IRQ_MMP_CCIC_ISP        (11+32)    
#define IRQ_MMP_ONEWIRE         (12+32)    
#define IRQ_MMP_AP1_TIMER1      (13+32)    
#define IRQ_MMP_AP1_TIMER2      (14+32)    
#define IRQ_MMP_ISP_DMA         (15+32)    
#define IRQ_MMP_CP_AP_IPC0      (16+32)    
#define IRQ_MMP_CP_AP_IPC1      (17+32)    
#define IRQ_MMP_CP_AP_IPC2      (18+32)    
#define IRQ_MMP_CP_AP_IPC3      (19+32)    
#define IRQ_MMP_CP_AP_IPC4      (20+32)    
#define IRQ_MMP_AP_CP_IPC0      (21+32)  
#define IRQ_MMP_AP_CP_IPC1      (22+32)    
#define IRQ_MMP_AP_CP_IPC2      (23+32)    
#define IRQ_MMP_AP_CP_IPC3      (24+32)    
#define IRQ_MMP_CODA            (25+32)    
#define IRQ_MMP_AP_CP_INT       (26+32)    
#define IRQ_MMP_UART2           (27+32)    
#define IRQ_MMP_UART3           (28+32)    
#define IRQ_MMP_AP2_TIMER1      (29+32)    
#define IRQ_MMP_AP2_TIMER2      (30+32)    
#define IRQ_MMP_CP_TIMER1       (31+32)    
#define IRQ_MMP_CP_TIMER2       (32+32)    
#define IRQ_MMP_TWSI2           (33+32)    
#define IRQ_MMP_GSSP            (34+32)    
#define IRQ_MMP_WDT             (35+32)    
#define IRQ_MMP_MPMU            (36+32)    
#define IRQ_MMP_CP_FRQ          (37+32)    
#define IRQ_MMP_AP_FRQ          (38+32)    
#define IRQ_MMP_MMC             (39+32)    
#define IRQ_MMP_AEU             (40+32)    
#define IRQ_MMP_LCD             (41+32)    
#define IRQ_MMP_CCIC_IPE        (42+32)    
#define IRQ_MMP_DDR_PERF_CNT    (43+32)    
#define IRQ_MMP_USB             (44+32)    
#define IRQ_MMP_NAND            (45+32)    
#define IRQ_MMP_HIFI_DMA        (46+32)    
#define IRQ_MMP_DMA0            (47+32)    
#define IRQ_MMP_DMA1            (48+32)    
#define IRQ_MMP_GPIO_AP         (49+32)    
#define IRQ_MMP_PAD_EDGE        (50+32)    
#define IRQ_MMP_SPH             (51+32)    
#define IRQ_MMP_RIPC_CP         (52+32)    
#define IRQ_MMP_DSI             (53+32)    
#define IRQ_MMP_TWSI_CP         (54+32)    
#define IRQ_MMP_GPIO_CP         (55+32)    
#define IRQ_MMP_RIPC_AP         (56+32)    
#define IRQ_MMP_PERFM_CP        (57+32)    
#define IRQ_MMP_CORESIGHT_AP0   (58+32)    
#define IRQ_MMP_UART_CP         (59+32)    
#define IRQ_MMP_DR0             (60+32)    
#define IRQ_MMP_CORESIGHT_AP1   (61+32)    
#define IRQ_MMP_FABRIC          (62+32)    
#define IRQ_MMP_SMC             (63+32)  
#define IRQ_MMP_AP3_TIMER1      (64+32)    
#define IRQ_MMP_AP3_TIMER2      (65+32)      
#define IRQ_MMP_HSI             (70+32)   
#define IRQ_MMP_HSI_WAKE  	(71+32)    


/* public methord */
void IRQ_FIQ_init(void);
void IRQ_Glb_Ena(void);
void FIQ_Glb_Ena(void);
void INT_Enable(BU_U32 int_num, uint32_t irq0_fiq1, BU_U32 priority );
void INT_Disable(BU_U32 int_num ); //disable both irq and fiq
void ISR_Connect(BU_U32 int_num, void (*ISR_Handle)(void*), void *);
void ISR_Disconnect(BU_U32 int_num);
void icu_target_set(uint32_t interrupt, uint32_t cpu, uint32_t set);
uint32_t icu_target_get(uint32_t interrupt);
void icu_target_clear(uint32_t interrupt);
void icu_priority_set(uint32_t int_num, uint32_t priority);
uint32_t icu_priority_get(uint32_t int_num);
void icu_set_irq(uint32_t int_num);
void icu_set_fiq(uint32_t int_num);

#ifdef ICU_AS_PRIMARY_IC
void icu_init(void);
void fiq_glb_enable(void);
void irq_glb_enable(void);
void fiq_glb_disable(void);
void irq_glb_disable(void);
void icu_isr_connect(uint32_t int_num, void (*ISR_Handle)(void*), void *ISR_Arg);
void icu_isr_disconnect(uint32_t int_num);
#endif

#endif /* _INTERRUPT_H_ */
