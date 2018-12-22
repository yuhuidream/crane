#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#define   IPC_PRIORITY       15
#define   DMA_PRIORITY      15

// definition for bit 4 of ICU_INT_0_63_CONF
//#define   FIQ_ROUTE      0x0
//#define   IRQ_ROUTE      0x1

#define   FIQ_ROUTE          0x1
#define   IRQ_ROUTE          0x0
#define   INT_NUM      64

#define   INT_PENDING_BIT     BIT_7  //In Crane, it's bit 7
// ICU Hardware Interrupt Assignment
// Description   INT REQ   APB signal   Comments

#define IRQ_MMP_NONE            (-1)
#define IRQ_MMP_CP              (0 )    
#define IRQ_MMP_SSP2            (1 )    
#define IRQ_MMP_SSP1            (2 )    
#define IRQ_MMP_SSP0            (3 )    
#define IRQ_MMP_PMIC_INT        (4 )    
#define IRQ_MMP_RTC_INT         (5 )    
#define IRQ_MMP_RTC_ALARM       (6 )    
#define IRQ_MMP_TWSI1           (7 )    
//#define IRQ_AP_INT              (8 )    
#define IRQ_MMP_KEYPAD          (9 )    
#define IRQ_MMP_USIM1           (10)    
#define IRQ_MMP_USIM2           (11)    
#define IRQ_MMP_ONEWIRE         (12)    
#define IRQ_MMP_AP1_TIMER1      (13)    
#define IRQ_MMP_AP1_TIMER2      (14)    
#define IRQ_MMP_USB3            (15)    
#define IRQ_MMP_CEVA2CP_IPC0    (16)    
#define IRQ_MMP_CEVA2CP_IPC1    (17)    
#define IRQ_MMP_CEVA2CP_IPC2    (18)    
#define IRQ_MMP_CEVA2CP_IPC3    (19)    
#define IRQ_MMP_IPC_PMUC_WAKE   (20)    
#define IRQ_MMP_CP2CEVA_IPC0    (21)  
#define IRQ_MMP_CP2CEVA_IPC1    (22)    
#define IRQ_MMP_CP2CEVA_IPC2    (23)    
#define IRQ_MMP_CP2CEVA_IPC3    (24)
#define IRQ_MMP_IPC_PMUA_WAKE   (25)
#define IRQ_MMP_PCIE            (25)
#define IRQ_MMP_MC_INT          (26)    
#define IRQ_MMP_UART1           (27)    
#define IRQ_MMP_UART2           (28)    
#define IRQ_MMP_AP2_TIMER1      (29)    
#define IRQ_MMP_AP2_TIMER2      (30)    
#define IRQ_MMP_CP_TIMER1       (31)    
#define IRQ_MMP_CP_TIMER2       (32)    
#define IRQ_MMP_TSEN            (33)    
#define IRQ_MMP_GSSP            (34)    
#define IRQ_MMP_WDT             (35)    
#define IRQ_MMP_MPMU            (36)    
#define IRQ_MMP_PMU_IRQ2        (37)    
#define IRQ_MMP_PMU_IRQ         (38)    
#define IRQ_MMP_MMC             (39)    
#define IRQ_MMP_AEU             (40)    
#define IRQ_MMP_HSI             (41)    
#define IRQ_MMP_LCD             (42)    
#define IRQ_MMP_NEWUART         (43)    
#define IRQ_MMP_USB             (44)    
#define IRQ_MMP_QSPI            (45)    
#define IRQ_MMP_DC              (46)    
#define IRQ_MMP_DMA0            (47)    
#define IRQ_MMP_DMA1            (48)    
#define IRQ_MMP_GPIO_AP         (49)    
#define IRQ_MMP_CTI_CP          (50)    
#define IRQ_MMP_SPH             (51)    
#define IRQ_MMP_RIPC_CP         (52)    
#define IRQ_MMP_USB_BATTERY     (53)    
#define IRQ_MMP_TWSI_CP         (54)    
#define IRQ_MMP_GPIO_CP         (55)    
#define IRQ_MMP_RIPC_AP         (56)    
#define IRQ_MMP_PERFM_CP        (57)    
#define IRQ_MMP_IPE             (58)    
#define IRQ_MMP_UART_CP         (59)    
#define IRQ_MMP_GPIO_EDGE_DET   (60)    
#define IRQ_MMP_OFFLINE         (61)    
#define IRQ_MMP_FABRIC          (62)    
#define IRQ_MMP_SMC             (63) 
#define IRQ_MMP_PMIC_WAKEUP		(64)
#define IRQ_MMP_DTC_CH0			(68)
#define IRQ_MMP_DTC_CH1			(69)
#define IRQ_MMP_DTC_ERROR		(70)

/* public methord */
void IRQ_FIQ_init(void);
void IRQ_Glb_Ena(void);
void FIQ_Glb_Ena(void);
void INT_Enable(BU_U32 int_num, uint32_t irq0_fiq1, BU_U32 priority );
void INT_Disable(BU_U32 int_num ); //disable both irq and fiq
void ISR_Connect(BU_U32 int_num, void (*ISR_Handle)(void*), void *);
void ISR_Disconnect(BU_U32 int_num);
void icu_target_set(uint32_t interrupt, uint32_t cpu, uint32_t set);
void icu_target_set_all(uint32_t interrupt);
uint32_t icu_target_get(uint32_t interrupt);
void icu_target_clear(uint32_t interrupt);
void icu_priority_set(uint32_t int_num, uint32_t priority);
uint32_t icu_priority_get(uint32_t int_num);
void icu_set_irq(uint32_t int_num);
void icu_set_fiq(uint32_t int_num);
void irq_target_clear(int irq);
void irq_target_set(int irq, int target);
int irq_target_get(int irq);

void icu_init(void);
void fiq_glb_enable(void);
void irq_glb_enable(void);
void fiq_glb_disable(void);
void irq_glb_disable(void);
void icu_isr_connect(uint32_t int_num, void (*ISR_Handle)(void*), void *ISR_Arg);
void icu_isr_disconnect(uint32_t int_num);


#endif /* _INTERRUPT_H_ */
