#ifndef __PMU_DEC_H
#define __PMU_DEC_H

#define PMU_AP_MSA_SYNC_ADDR 0xd1e51000
#define PMU_AP_MSA_SYNC_WORD 0xB6B66B6B

#define CPU_MAGIC 0xdeadbeef

typedef struct
{
    uint32_t DCR;
    uint32_t ISER[5];
    uint32_t IPR[40];
    uint32_t IPTR[40];
    uint32_t ICFR[10];
} gic_icd_context;

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
    WAKE_WTD,
    WAKE_PMIC,
    WAKE_SQU,
    WAKE_DDR_IDLE_LOST,
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


void set_wake_src(e_wake_src wake_src, uint32_t time_sec);
void cx_enter(uint32_t wfi_mode, uint32_t cpu, uint32_t sram_off);

void power_save_peripherals(int wake_src_bitmap);
void power_resume_peripherals(int wake_src_bitmap);

void lpm_init(void);

/*Those are defined same as MSA code and SG code for COMM side test
0xd1010000~0xd1010060 is used by COMM test, 0xd1010100~0xd1010500 used for CP Vmax test*/
#define  msa_result     (*(volatile unsigned int *)0xD101000C)
#define  msa_loops      (*(volatile unsigned int *)0xD1010010)
#define  back_reg1      (*(volatile unsigned int *)0xD1010004)
#define  back_reg2      (*(volatile unsigned int *)0xD1010008)

#define  msa_pp         (*(volatile unsigned int *)0xD1010018)
#define  msa_mode       (*(volatile unsigned int *)0xD101001c)
#define  msa_index      (*(volatile unsigned int *)0xD1010020)
#define  ddr_idle_time  (*(volatile unsigned int *)0xD1010024)

#define  sg_ind         (*(volatile unsigned int *)0xD1010028) //used in Seagull code
#define  sg_mode        (*(volatile unsigned int *)0xD101002c) //used in Seagull code
#define  sg_index       (*(volatile unsigned int *)0xD1010060) //used in Seagull code
#define  td_wb_ind      (*(volatile unsigned int *)0xD1010064) //set in MSA, 1: TD, 2:WB, 0:unknown
#define  msa_memt       (*(volatile unsigned int *)0xD1010068)
#define  gsm_eq_result  (*(volatile unsigned int *)0xD101006c) //GSM equalizer test result

#define  crc_result     (*(volatile unsigned int *)0xD1010030)

#define  ddr_axi_wake_result    (*(volatile unsigned int *)0xD1010014)

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
