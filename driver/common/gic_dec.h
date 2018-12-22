#ifndef __INC_GIC_H
#define __INC_GIC_H

#include "GIC.h"

#define GIC_INT_NUMS        128

//SPI: Shared Periperial Interrupts ID32-ID127
//Those are 0-95 interrupts from SOC ICU+Moltres CPU subsystem interrupts
//In current BBU, we use the legacy IRQ interrupts ID 31 to handle interrrupts with ICU.

#define NO_GIC_INT_PENDING  -1

// PPI: Private peripherial Interrupts assignment for 16-31 
#define GIC_INT_FIQx        28
#define GIC_INT_TMR         29
#define GIC_INT_WD          30
#define GIC_INT_ICU         31
//The PPIs in the range ID16-ID27 are unimplemented.
 
/*
a set of 4 timers per CPU is defined   
A physical timer for use in Kernel modes/Secure Privileged modes   (CNTP_)    Banked 
-This register is banked to allow the secure side to have its own copy   
-Non-secure Kernel access to this Timer controlled using a Hyp Control register   
A virtual timer for use in Kernel modes           (CNTV_)        Common 
A physical timer for use in Hyp mode              (CNTHP_)       Common  

Each CPU makes the output of each timer as an output pin to the system, and 
to a dedicated private peripheral interrupt for that CPU. 
For each CPU the same interrupt number should be used for the same timer. 
*/

//It looks like PPI IDs 27-24 respectively are for CNTHP, CNTV, CNTPNS, and CNTPS.

#define GIC_INT_CNTHP       27
#define GIC_INT_CNTV        26
#define GIC_INT_CNTPNS      25
#define GIC_INT_CNTPS       24

//SGI: Software generated interrupt ID0-15
#define GIC_INT_SW_0    0
#define GIC_INT_SW_1    1
#define GIC_INT_SW_2    2
#define GIC_INT_SW_3    3
#define GIC_INT_SW_4    4
#define GIC_INT_SW_5    5
#define GIC_INT_SW_6    6
#define GIC_INT_SW_7    7
#define GIC_INT_SW_8    8
#define GIC_INT_SW_9    9
#define GIC_INT_SW_10   10
#define GIC_INT_SW_11   11
#define GIC_INT_SW_12   12
#define GIC_INT_SW_13   13
#define GIC_INT_SW_14   14
#define GIC_INT_SW_15   15

/* Functions */
void gic_icu_enable(void);
void GIC_ISR_ENABLE(uint32_t int_num, uint32_t priority, uint8_t coreNum );
void GIC_ISR_DISABLE(uint32_t int_num);
void GIC_ISR_CONNECT(uint32_t int_num, void (*ISR_Handle)(void*), void *ISR_Arg);
void GIC_ISR_DISCONNECT(uint32_t int_num);
void gic_interface_init(void);
int gic_distributor_init(void);
uint32_t gic_priority_set(uint32_t interrupt, uint32_t priority);
uint32_t gic_priority_get(uint32_t interrupt);
void gic_target_set(uint32_t interrupt, uint32_t cpu, uint32_t set);
uint32_t gic_target_get(uint32_t interrupt);
void gic_target_clear(uint32_t interrupt);
void gic_interrupt_enable(uint32_t interrupt, uint32_t enable);
uint32_t gic_running_priority(void);
uint32_t gic_highest_pending_ID_get(void);
uint32_t gic_highest_pending_source_get(void);
uint32_t gic_binary_point_set(uint32_t binary_point);
uint32_t gic_binary_point_get(void);
uint32_t gic_priority_mask_set(uint32_t priority_mask);
uint32_t gic_priority_mask_get(void);
int gic_enabled(void);
int gic_set_type(uint32_t int_num, uint32_t type);
void sgi_send(uint32_t target_mask, uint32_t interrupt);
void sgi_broadcast_self(uint32_t interrupt);
void sgi_broadcast_all_but_self(uint32_t interrupt);
void gic_migrate_irqs(uint8_t coreId);
void gic_restore_irqs(uint8_t coreId);
void gic_trigger_sw_int(int int_id, int cpuID);
void gic_interface_disable(void);
void gic_trigger_sw_int_list(int int_id, int cpuIDlist);
void gic_enable_sgi(int int_num);
void global_sgi_enable(void);
void gic_group_config(uint32_t interrupt, uint32_t group);


/*
 * Description:
 * Mask for interrupt ID (used with acknowledge, highest pending, software and end of interrupt registers
 */
#define INTERRUPT_MASK     0x000003FF 

/**
 * \brief ID of 'spurious' interrupt
 *
 * This ID is returned when acknowledging an interrupt, if no interrupt is pending.
 * This is also returned when acknowledging an interrupt that has become masked due
 * to priority change but was already pending (sent to the CPU).
 */
#define SPURIOUS_INTERRUPT 1023

/*
 * Number of interrupts implemented in the GIC
 *
 * The number of interrupts that the Distributor can handle is IMPLEMENTATION DEFINED.
 * It can be up to 1020, with up to 4 CPUs being supported (expansion space reserved for
 * up to 8 CPUs)
 */

#define   INT_NUMS_SECOND      32


/*
 * Description:
 * Mask for Inter Processor Interrupt ID
 */
#define IPI_MASK           0x0000000F


/*
 * Description:
 * Mask for interrupt ID (used with acknowledge, highest pending, software and end of interrupt registers
 */
#define INTERRUPT_MASK     0x000003FF  


/*
 * Description:
 * Mask for CPU source ID (used with highest pending and end of interrupt registers
 */
#define CPUID_MASK         0x00000C00


/*
 * Description:
 * Mask for Binary Point register
 */
#define BINARY_POINT_MASK  0x00000007


/*
 * Description:
 * Mask for Priority Mask and Running Interrupt registers
 */
#define PRIORITY_MASK_MASK 0x000000F0


/*
 * Description:
 * Shift required to locate the CPU_ID field in the interface registers
 */
#define CPUID_SHIFT        10


/*
 * Description:
 * Shift required to locate the priority field in the interface registers
 */
#define PRIORITY_SHIFT      4


/*
 * Description:
 * Shift required to locate the CPU target list field in the Software Interrupt register
 */
#define IPI_TARGET_SHIFT   16


/*
 * Description:
 * Shift required to locate the target list filter field in the Software Interrupt register
 */
#define IPI_TARGET_FILTER_SHIFT 24


/**
 * Target list filter for Software Interrupt register
 */
#define USE_TARGET_LIST   0x0 /**< Interrupt sent to the CPUs listed in CPU Target List */
#define ALL_BUT_SELF      0x1 /**< CPU Target list ignored, interrupt is sent to all but requesting CPU */
#define SELF              0x2 /**< CPU Target list ignored, interrupt is sent to requesting CPU only */

/**
 * Enable/Disable macros (used as a parameter in interrupt_enable() )
 */
#define ENABLE        1 /**< Interrupt enable. Used with gic_interrupt_enable() primitive. */
#define DISABLE       0 /**< Interrupt disable. Used with gic_interrupt_enable() primitive. */


/**
 * Set/Unset macros (used as a parameter in interrupt_target_set() )
 */
#define SET           1 /**< Set macro (used as a parameter in gic_target_set() */
#define UNSET         0 /**< Un-Set macro (used as a parameter in gic_target_set() */

#define IRQ_TYPE_LEVEL_HIGH  1
#define IRQ_TYPE_EDGE_RISING 0

#define GIC_EMEI_GLOBAL_TIMER         (27)
#define GIC_EMEI_FIQx                 (28)
#define GIC_EMEI_PRIVATE_TIMER        (29)
#define GIC_EMEI_WD                   (30)
#define GIC_EMEI_IRQx                 (31)
//
#define GIC_EMEI_NONE                 (-1)
//#define GIC_EMEI_AIRQ                 (32)
//#define GIC_EMEI_SSP2                 (33)
//#define GIC_EMEI_SSP1                 (34)
//#define GIC_EMEI_SSP0                 (35)
//#define GIC_EMEI_PMIC                 (36)
//#define GIC_EMEI_RTC                  (37)
//#define GIC_EMEI_RTC_ALARM            (38)
//#define GIC_EMEI_I2C1                 (39)
//#define GIC_EMEI_GPU                  (40)
//#define GIC_EMEI_KEYPAD               (41)
//#define GIC_EMEI_DMA2                 (42)
//#define GIC_EMEI_DXO_ENGINE           (43)
//#define GIC_EMEI_ONEWIRE              (44)
//#define GIC_EMEI_TIMER1_1             (45)
//#define GIC_EMEI_TIMER1_2             (46)
//#define GIC_EMEI_ISP_DMA              (47)
//#define GIC_EMEI_IPC_AP0              (48)
//#define GIC_EMEI_IPC_AP1              (49)
//#define GIC_EMEI_IPC_AP2              (50)
//#define GIC_EMEI_IPC_AP3              (51)
//#define GIC_EMEI_IPC_AP4              (52)
//#define GIC_EMEI_IPC_CP0              (53)
//#define GIC_EMEI_IPC_CP1              (54)
//#define GIC_EMEI_IPC_CP2              (55)
//#define GIC_EMEI_IPC_CP3              (56)
//#define GIC_EMEI_CODA                 (57)
//#define GIC_EMEI_DDR                  (58)
//#define GIC_EMEI_UART2                (59)
//#define GIC_EMEI_UART3                (60)
//#define GIC_EMEI_AP2_TIMER1           (61)
//#define GIC_EMEI_AP2_TIMER2           (62)
//#define GIC_EMEI_CP_TIMER1            (63)
//#define GIC_EMEI_CP_TIMER2            (64)
//#define GIC_EMEI_I2C2                 (65)
//#define GIC_EMEI_GSSP                 (66)
//#define GIC_EMEI_WDT                  (67)
//#define GIC_EMEI_PMUM                 (68)
//#define GIC_EMEI_SEAGULL_FRQ          (69)
//#define GIC_EMEI_MOHAWK_FRQ           (70)
//#define GIC_EMEI_MMC                  (71)
//#define GIC_EMEI_AEU                  (72)
//#define GIC_EMEI_LCD                  (73)
//#define GIC_EMEI_CCIC_IPE             (74)
//#define GIC_EMEI_ROTATION             (75)
//#define GIC_EMEI_USB1                 (76)
//#define GIC_EMEI_NAND                 (77)
//#define GIC_EMEI_HIFI_DMA             (78)
//#define GIC_EMEI_DMA0                 (79)
//#define GIC_EMEI_DMA1                 (80)
//#define GIC_EMEI_GPIO_AP              (81)
//#define GIC_EMEI_GPIO_EDGE            (82)
//#define GIC_EMEI_SPH                  (83)
//#define GIC_EMEI_IPC_SRV0_SEAGULL     (84)
//#define GIC_EMEI_DSI                  (85)
//#define GIC_EMEI_I2C3                 (86)
//#define GIC_EMEI_GPIO_CP              (87)
//#define GIC_EMEI_IPC_SRV0_MOHAWK      (88)
//#define GIC_EMEI_PERF_COUNTER         (89)
//#define GIC_EMEI_CORESIGHT            (90)
//#define GIC_EMEI_UART1_CP             (91)
//#define GIC_EMEI_DRO_TEMP_SENSOR      (92)
//#define GIC_EMEI_CORESIGHT2           (93)
//#define GIC_EMEI_FABRIC_TIMEOUT       (94)
//#define GIC_EMEI_SM                   (95)


#endif  /* __INC_GIC_H */



    

