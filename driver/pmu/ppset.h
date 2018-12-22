#ifndef __PPSET_H
#define __PPSET_H

//#define OP_INVALID  0xFF   //enum is uint8_t by default.

typedef enum op_tag{
    OP0 = 0,
    OP1,
    OP2,
    OP3,
    OP4,
    OP5,
    OP6,
    OP7,
    OP8,
    OP9,
    OP_MAX,
    OP_INVALID
}op_index;

typedef struct core_freq_tag {
    const char *label;
    uint32_t pclk;
    uint32_t memclk;
    uint32_t busclk;
    uint32_t pll2;
    uint32_t pll2p;
    uint32_t dvc;
}core_freq;

//Note: dclk_1x is CR5's MEMCLK, so change CR5's freq means change dclk

typedef struct dfc_setting_tag {
    uint32_t pll2ref;
    uint32_t pll2fb;
    uint32_t apb_spare2; //for PLL2
    uint32_t pmum_fccr;
    uint32_t pmua_cc_ap_cp;
    uint32_t mc_reg_table; //index and enable
}dfc_setting;

typedef struct axi_freq_tag {
    const char *label;
    uint32_t aclk;
    uint32_t dvc;
}axi_freq;

typedef struct axi_setting_tag {
    uint32_t pmum_fccr;
    uint32_t pmua_cc_ap_cp;
}axi_setting;

//Coresight/Debug
typedef struct cs_freq_tag {
    const char *label;
    uint32_t atclk; //is synced to trace_clk, share one FC_request
    uint32_t dbg_clk; //use another FC frequest
    uint32_t trace_clk;
    uint32_t dvc;
}cs_freq;

typedef struct cs_setting_tag {
    uint32_t trace_config;
}cs_setting;

typedef struct mck5_table_entry_tag{
    uint32_t reg_data;// Reg Table Data Reg0, [31:0] regdata
    uint32_t offset_pause_end;// Reg Table Data Reg1,[17] end, [16] pause, [11:0] regoffset
    uint32_t table_sel_addr;// Reg Table Control Reg, [7:5] regtable sel, [4:0] SRAM table addr
}mck5_talbe_entry;

#define NA 0  //stand for not defined since it's not used

#ifdef CONFIG_CA7
extern op_index c_aop, c_csop;
#endif
extern op_index c_cop, c_axiop;
#ifdef CONFIG_CA7
extern core_freq ca7_value[];
extern cs_freq cs_value[];
#endif
extern core_freq cr5_value[];
extern axi_freq aclk_value[];

/*
Note: since AP wants to know the DDR OP and it's controlled by CP in Nezha2.
Let CP use the CIU->SW_SCRATCH register to  save the current DDR OP.
and AP read the register to get the OP
*/

/*This should be called by PMU_AO init*/
void pp_init(void);
void dump_pmu_reg(void);
#ifdef CONFIG_CA7
uint32_t adfc(uint32_t top);
#endif
uint32_t cdfc(uint32_t top);
uint32_t axidfc(uint32_t top);
void csdfc(uint32_t top);
int bbu_ppset_test(int argc, uint8_t **argv);

#endif //ppset_h

