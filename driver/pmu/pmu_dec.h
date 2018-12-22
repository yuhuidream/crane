#ifndef __PMU_DEC_H
#define __PMU_DEC_H

#define PMU_AP_MSA_SYNC_ADDR 0xd1e51000
#define PMU_AP_MSA_SYNC_WORD 0xB6B66B6B

#define CPU_MAGIC 0xdeadbeef
struct lockdown_regs
{
    unsigned int d, i;
};

typedef struct
{
    /* 0x000 */ const unsigned cache_id;
    /* 0x004 */ const unsigned cache_type;
                char padding1[0x0F8];
    /* 0x100 */ volatile unsigned control;
    /* 0x104 */ volatile unsigned aux_control;
    /* 0x108 */ volatile unsigned tag_ram_control;
    /* 0x10C */ volatile unsigned data_ram_control;
                char padding2[0x0F0];
    /* 0x200 */ volatile unsigned ev_counter_ctrl;
    /* 0x204 */ volatile unsigned ev_counter1_cfg;
    /* 0x208 */ volatile unsigned ev_counter0_cfg;
    /* 0x20C */ volatile unsigned ev_counter1;
    /* 0x210 */ volatile unsigned ev_counter0;
    /* 0x214 */ volatile unsigned int_mask;
    /* 0x218 */ const volatile unsigned int_mask_status;
    /* 0x21C */ const volatile unsigned int_raw_status;
    /* 0x220 */ volatile unsigned int_clear;
                char padding3[0x50C];
    /* 0x730 */ volatile unsigned cache_sync;
                char padding4[0x03C];
    /* 0x770 */ volatile unsigned inv_pa;
                char padding5[0x008];
    /* 0x77C */ volatile unsigned inv_way;
                char padding6[0x030];
    /* 0x7B0 */ volatile unsigned clean_pa;
                char padding7[0x004];
    /* 0x7B8 */ volatile unsigned clean_index;
    /* 0x7BC */ volatile unsigned clean_way;
                char padding8[0x030];
    /* 0x7F0 */ volatile unsigned clean_inv_pa;
                char padding9[0x004];
    /* 0x7F8 */ volatile unsigned clean_inv_index;
    /* 0x7FC */ volatile unsigned clean_inv_way;
                char paddinga[0x100];
    /* 0x900 */ volatile struct lockdown_regs lockdown[8];
                char paddingb[0x010];
    /* 0x950 */ volatile unsigned lock_line_en;
    /* 0x954 */ volatile unsigned unlock_way;
                char paddingc[0x2A8];
    /* 0xC00 */ volatile unsigned addr_filtering_start;
    /* 0xC04 */ volatile unsigned addr_filtering_end;
                char paddingd[0x338];
    /* 0xF40 */ volatile unsigned debug_ctrl;
                char paddinge[0x01C];
    /* 0xF60 */ volatile unsigned prefetch_ctrl;
                char paddingf[0x01C];
    /* 0xF80 */ volatile unsigned power_ctrl;
} pl310_registers;


typedef struct
{
    unsigned int control;
    unsigned int aux_control;
    unsigned int tag_ram_control;
    unsigned int data_ram_control;
    unsigned int ev_counter_ctrl;
    unsigned int ev_counter1_cfg;
    unsigned int ev_counter0_cfg;
    unsigned int ev_counter1;
    unsigned int ev_counter0;
    unsigned int int_mask;
    unsigned int lock_line_en;
    struct lockdown_regs lockdown[8];
    unsigned int unlock_way;
    unsigned int addr_filtering_start;
    unsigned int addr_filtering_end;
    unsigned int debug_ctrl;
    unsigned int prefetch_ctrl;
    unsigned int power_ctrl;
} pl310_context;


//===============wake up soures================================
/*Don't change the order/value since the cmd will use the value!*/
typedef enum {
    WAKE_SCK = 0,
    WAKE_GSM,
    WAKE_EDGE,
    WAKE_KP,
    WAKE_WB, //TD or WB
    WAKE_RTC, //5
    WAKE_USB_VBUS,
    WAKE_USB_ID, //it's no longer supported in Emei
    WAKE_USB_LINE,
    WAKE_SDH,
    WAKE_WTD,   //10
    WAKE_PMIC,
    WAKE_SQU,
    WAKE_DDR_IDLE_LOST,
    WAKE_HSIC_RESUME,
    WAKE_USB3,
    WAKE_IPC,   //16
    WAKE_RIPC,  //17
    WAKE_PCIE,  //18
    WAKE_ALL = 0xFF
}e_wake_src;
#define    WAKE_SCK_BIT 	(BIT_0<<WAKE_SCK)
#define    WAKE_GSM_BIT 	(BIT_0<<WAKE_GSM)
#define    WAKE_GPIO_BIT	(BIT_0<<WAKE_GPIO)
#define    WAKE_KP_BIT		(BIT_0<<WAKE_KP)
#define    WAKE_WB_BIT		(BIT_0<<WAKE_WB)
#define    WAKE_RTC_BIT		(BIT_0<<WAKE_RTC)
#define    WAKE_USB_VBUS_BIT	(BIT_0<<WAKE_USB_VBUS)
#define    WAKE_USB_ID_BIT	(BIT_0<<WAKE_USB_ID)
#define    WAKE_USB_LINE_BIT	(BIT_0<<WAKE_USB_LINE)
#define    WAKE_SDH_BIT		(BIT_0<<WAKE_SDH)
#define    WAKE_WTD_BIT		(BIT_0<<WAKE_WTD)
#define    WAKE_PMIC_BIT	(BIT_0<<WAKE_PMIC)
#define    WAKE_SQU_BIT		(BIT_0<<WAKE_SQU)
#define    WAKE_DDR_IDLE_LOST_BIT (BIT_0 << WAKE_DDR_IDLE_LOST)
#define    WAKE_USB3_BIT    (BIT_0<<WAKE_USB3)
#define    WAKE_HSIC_BIT       (BIT_0 <<WAKE_HSIC_RESUME)

void set_wake_src(e_wake_src wake_src, uint32_t time_sec);
void cx_enter(uint32_t wfi_mode, uint32_t cpu, uint32_t sram_off);

void power_save_peripherals(int wake_src_bitmap);
void power_resume_peripherals(int wake_src_bitmap);

void lpm_init(void);

/*Those are defined same as MSA code and SG code for COMM side test
*/

#define  SQU_END        0xD1010000  //step back from the end of the SQU
#define  sg_ind         (*(volatile unsigned int *)(SQU_END-0x4)) //used in Seagull code

#define  bank       (*(volatile unsigned int *)(SQU_END-0x10))
#define  msa_boot_ind   (*(volatile unsigned int *)(SQU_END-0x14))
#define  back_reg1      (*(volatile unsigned int *)(SQU_END-0x18))
#define  back_reg2      (*(volatile unsigned int *)(SQU_END-0x1C))

#define  msa_pp         (*(volatile unsigned int *)(SQU_END-0x20))
#define  msa_mode       (*(volatile unsigned int *)(SQU_END-0x24))
#define  msa_index      (*(volatile unsigned int *)(SQU_END-0x28))
#define  ddr_idle_time  (*(volatile unsigned int *)(SQU_END-0x2C))

#define  msa_memt       (*(volatile unsigned int *)(SQU_END-0x30))
#define  gsm_eq_result  (*(volatile unsigned int *)(SQU_END-0x34)) //GSM equalizer test result
#define  crc_result     (*(volatile unsigned int *)(SQU_END-0x38))
#define  ddr_axi_wake_result    (*(volatile unsigned int *)(SQU_END-0x3C))

//for AP request MSA register read/write: cmd2msa_set, cmd2msa, value2msa, addr2msa
#define cmd2msa_set  (*(volatile unsigned int *)(SQU_END-0x40))
#define cmd2msa      (*(volatile unsigned int *)(SQU_END-0x44))
#define value2msa    (*(volatile unsigned int *)(SQU_END-0x48))
#define addr2msa     (*(volatile unsigned int *)(SQU_END-0x4C))

//for L2 SRAM Vmin test
#define startaddr2msa     (*(volatile unsigned int *)(SQU_END-0x50))
#define endaddr2msa     (*(volatile unsigned int *)(SQU_END-0x54))
#define pattern2msa     (*(volatile unsigned int *)(SQU_END-0x58))
#define errorcnt2msa     (*(volatile unsigned int *)(SQU_END-0x5C))

#define  msa_l2rtc   (*(volatile unsigned int *)(SQU_END-0x60))

//For DSP access DDR latency test
#define latencycount   (*(volatile unsigned int *) (SQU_END-0x64))
#define read2dsp (*(volatile unsigned int *) 0xD1E51000)

extern uint32_t  cp_boot_test;
extern uint32_t  msa_boot_test;
extern uint32_t  digrf3_ext_loop_test;
extern uint32_t  td_crc_test;
extern uint32_t  gsm_eq_test;

#define L2310_ADDR 0xD1DFB000 //according to L2C.h
#define cache_line_size 0x20
#define C_BIT 0x01

#define CPU_DATA_SIZE   0x1000   //FIXME: this size may need to be optimized

extern uint32_t VirtualToPhysical(uint32_t);

/*-----------------------------------------------------------------*/
#define PMU_DATA_SIZE               128
extern void save_performance_monitors(unsigned *pointer);
extern void restore_performance_monitors(unsigned *pointer);
/*-----------------------------------------------------------------*/
#define VFP_DATA_SIZE               288
extern void save_vfp(unsigned *pointer);
extern void restore_vfp(unsigned *pointer);
/*-----------------------------------------------------------------*/
#define DEBUG_DATA_SIZE               128
extern void save_ca9_debug(uint32_t *pointer);
extern void restore_ca9_debug(unsigned *pointer);
/*-----------------------------------------------------------------*/
/* Critical registers saved/restored inside one assembly function.
*/
void power_basic_init(void);
void mfp_power_basic_init(void);
#endif //PMU_DEC_H
