#ifndef __PPSET_H
#define __PPSET_H

//enum is uint8_t by default.
//Common OP index for all units of clock users
//OP0 is boot OP, it's not counted in the formal OPs
typedef enum op_tag{
    OP0 = 0, //BOOT OP
    OP1,
    OP_CSMAX = OP1,
    OP2,
    OP_PLLMAX = OP2,
    OP3,
    OP4,
    OP5,
    OP_AXIMAX = OP5,
    OP_CAXIMAX = OP5,
    OP_LAXIMAX = OP5,
    OP6,
    OP_DDRMAX = OP6,
    OP_CPMAX = OP6,
    OP7,
    OP8,
    OP_COREMAX = OP8,
    OP_UNCHANGE,
    OP_INVALID
}op_index;

typedef enum pll_tag{
    PLL1=1,
    PLL2,
    PLL_END=PLL2,
    PLL_INVALID
}pll_src;

//We also use this definition for AP side DVC vote collection
//besides the PLL user statistics for PLL2
typedef enum user_tag{
    PLL_CORE = 0,
    PLL_DDR,
#ifdef CONFIG_CA7
    PLL_AXI,
    PLL_CORESIGHT,
#endif
#ifdef CONFIG_CR5
    PLL_CAXI,
    PLL_LAXI,
    PLL_MSA,
#endif
    PLL_USER_INVALID
}pll_user;

#define   PLL_USER_NUM  (uint32_t)PLL_USER_INVALID //4

//Note: Common PLL  OPs for PLL2
typedef struct pll_freq_tag {
    const char *label;
    uint32_t pll_out;
    uint32_t pll_outp;
    uint32_t pll_ref; //PLLxCR, pll_ref | pll_fb
    uint32_t pll_fb;
    uint32_t pll_ctrl; //for those sources other than PLL1, APB_SPARE
}pll_freq;

typedef struct pll_num_tag {
    uint32_t number;  //2
    uint32_t pll_out;
    uint32_t pll_outp;
}pll_dump;

//AP CORE, ISP
typedef struct core_freq_tag {
    const char *label;
    uint32_t pclk; //core clk  or  ISP fclk
    uint32_t busclk;  //AXI_M0, bus clock
    uint32_t memclk;//AXI_M1, mem_clock
    pll_src   pll_source;
    uint32_t pll_value;
    op_index pll_op; //the PLL operating settings
    uint32_t fc_pllsel; //one of FCCR bit field, clock source
    uint32_t fc_div; // pmua_cc_ap, trace_config, GC/VPU PMU registers
    uint32_t dvc; //the dvc container, decided by profile/fab
    uint64_t dvcs; //all possible dvc among profiles, for Z3
    uint64_t dvcsa; //for A0
}core_freq;

#ifdef CONFIG_CA7
//Coresight/Debug
typedef struct cs_freq_tag {
    const char *label;
    uint32_t atclk; //is synced to trace_clk, share one FC_request
    uint32_t dbg_clk; //use another FC frequest
    uint32_t trace_clk;
    uint32_t trace_config;
    uint32_t dvc; //the dvc container, decided by profile/fab
    uint64_t dvcs; //all possible dvc among profiles.
}cs_freq;
#endif

/*Note
1,customer may choose different DDR type affecting PLL freq values
thus affecting freq source available  for other units
use mask attribute to identify them

2,CORE  may has different maximum freq support per our Market/PE
core can do 2-stepping DFC to avoid MFC, identified by pll_cop for other units
so we don't need extra mask for CORE freq.
Similarly, individual unit can change to any OP as long as PLL_OP no conflict
However, that's for test purpose in BBU, for FFOS, it's not recommned to do so
if the current source is shared. it's OK for dedicated PLL source in FFOS.

3,As for a combination of OPs is possible or not, check PLL_OP same or not
if it's doable, put a new PLL_OP in the pll_set array.
a high level wrapper can be used to contain the PPs of popular OP combinations
*/

#define   DDR400M  BIT_1
#define   DDR533M  BIT_2
#define   DDR_ALL  (DDR400M | DDR533M)
#define   DDR_N400M DDR533M

//Common for other units like (CP), DDR, AXI
typedef struct dfc_freq_tag {
    const char *label;
    uint32_t xclk;
    pll_src   pll_source;
    uint32_t pll_value;
    op_index pll_op; //the PLL operating settings
    uint32_t fc_pllsel; //FCCR related bits, clock source
    uint32_t fc_div; // pmua_cc_ap, trace_config
    uint32_t dvc; //the dvc container, decided by profile/fab; for z1z2, it's real value
    uint64_t dvcs; //all possible dvc among profiles; z3a0 only
    uint64_t dvcsa; //for A0
}dfc_freq;

typedef struct ddr_bound_tag {
    uint32_t valid_mask; //each bit stands for a case
    uint32_t mc_reg_table; //index and enable, for DDR only
    op_index aclk_op; //AXI clock bound to DDR OP
}ddr_binder;

typedef struct mck5_table_entry_tag{
    uint32_t reg_data;// Reg Table Data Reg0, [31:0] regdata
    uint32_t offset_pause_end;// Reg Table Data Reg1,[17] end, [16] pause, [11:0] regoffset
    uint32_t table_sel_addr;// Reg Table Control Reg, [7:5] regtable sel, [4:0] SRAM table addr
}mck5_table_entry;

#define NA 0  //stand for not defined since it's not used

#ifdef CONFIG_CA7
extern core_freq ca7_value[];
extern dfc_freq ddr_value[];
extern dfc_freq aclk_value[];
#else
extern core_freq cr5_value[];
extern dfc_freq *caclk_value;
extern dfc_freq *laclk_value;
#endif
extern op_index user_cop[PLL_USER_NUM];
extern uint32_t  ddr_type ;
extern ddr_binder  ddr_bvalue[];

/*This should be called by PMU_AO init*/
void pp_init(void);
void dump_pmu_reg(void);
int bbu_ppset_test(int argc, uint8_t **argv);
int bbu_opset_test(int argc, uint8_t **argv);
op_index ddrop_get(void);
#ifdef CONFIG_CA7
uint32_t adfc(op_index core_top, op_index ddr_top, op_index axi_top);
void csdfc(uint32_t top);
op_index aop_get(void);
op_index axiop_get(void);
#else
uint32_t cdfc(op_index core_top, op_index ddr_top, op_index axi_top);
void laxidfc(uint32_t top);
op_index cop_get(void);
op_index caxiop_get(void);
op_index laxiop_get(void);
#endif

uint32_t getprofile(uint32_t uifuses);

#endif //ppset_h

