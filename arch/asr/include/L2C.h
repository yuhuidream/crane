#ifndef _CACHE_L2C310_H_
#define _CACHE_L2C310_H_

/* L2C310 controller */
typedef struct 
{
    __IO uint32_t CID                 ; /* 0x000 reg0_cache_id   */            
    __IO uint32_t CTYPE               ; /* 0x004 reg0_cache_type */
    __IO uint32_t RESERVED0[62];                   
    __IO uint32_t CTRL                ; /* 0x100 reg1_control    */       
    __IO uint32_t AUXCTRL             ; /* 0x104 reg1_aux_control */                     
    __IO uint32_t TRAMCTRL            ; /* 0x108 reg1_tag_ram_control */
    __IO uint32_t DRAMCTRL            ; /* 0x10C reg1_data_ram_control */
    __IO uint32_t RESERVED1[60];
    __IO uint32_t EVCTCTRL            ; /* 0x200 reg2_ev_counter_ctrl */
    __IO uint32_t EVCT1CFG            ; /* 0x204 reg2_ev_counter1_cfg */
    __IO uint32_t EVCT0CFG            ; /* 0x208 reg2_ev_counter0_cfg */
    __IO uint32_t EVCT1               ; /* 0x20C reg2_ev_counter1 */
    __IO uint32_t EVCT0               ; /* 0x210 reg2_ev_counter0 */
    __IO uint32_t INTMASK             ; /* 0x214 reg2_int_mask */
    __O  uint32_t INTMASKST           ; /* 0x218 reg2_int_mask_statuse */
    __O  uint32_t INTRAWST            ; /* 0x21C reg2_int_raw_status */
    __I  uint32_t INTCLR              ; /* 0x220 reg2_int_clear */
    __IO uint32_t RESERVED2[323];    
    __IO uint32_t CSYNC               ; /* 0x730 reg7_cache_sync */
    __IO uint32_t RESERVED3[15];
    __IO uint32_t INVPA               ; /* 0x770 reg7_inv_pa */
    __IO uint32_t RESERVED4[2];
    __IO uint32_t INVWAY              ; /* 0x77C reg7_inv_way */
    __IO uint32_t RESERVED5[12];
    __IO uint32_t CLNPA               ; /* 0x7B0 reg7_clean_pa */
    __IO uint32_t RESERVED6;
    __IO uint32_t CLNIDX              ; /* 0x7B8 reg7_clean_index */
    __IO uint32_t CLNWAY              ; /* 0x7BC reg7_clean_way */
    __IO uint32_t RESERVED7[12];
    __IO uint32_t CLNINVPA            ; /* 0x7F0 reg7_clean_inv_pa */
    __IO uint32_t RESERVED8;
    __IO uint32_t CLNINVIDX           ; /* 0x7F8 reg7_clean_inv_index */
    __IO uint32_t CLNINVWAY           ; /* 0x7FC reg7_clean_inv_way */
    __IO uint32_t RESERVED9[64];
    __IO uint32_t DLKDN0              ; /* 0x900 reg9_d_lockdown0 */
    __IO uint32_t ILKDN0              ; /* 0x904 reg9_i_lockdown0 */
    __IO uint32_t DLKDN1              ; /* 0x908 reg9_d_lockdown1f */
    __IO uint32_t ILKDN1              ; /* 0x90C reg9_i_lockdown1f */
    __IO uint32_t DLKDN2              ; /* 0x910 reg9_d_lockdown2f */
    __IO uint32_t ILKDN2              ; /* 0x914 reg9_i_lockdown2f */
    __IO uint32_t DLKDN3              ; /* 0x918 reg9_d_lockdown3f */
    __IO uint32_t ILKDN3              ; /* 0x91C reg9_i_lockdown3f */
    __IO uint32_t DLKDN4              ; /* 0x920 reg9_d_lockdown4f */
    __IO uint32_t ILKDN4              ; /* 0x924 reg9_i_lockdown4f */
    __IO uint32_t DLKDN5              ; /* 0x928 reg9_d_lockdown5f */
    __IO uint32_t ILKDN5              ; /* 0x92C reg9_i_lockdown5f */
    __IO uint32_t DLKDN6              ; /* 0x930 reg9_d_lockdown6f */
    __IO uint32_t ILKDN6              ; /* 0x934 reg9_i_lockdown6f */
    __IO uint32_t DLKDN7              ; /* 0x938 reg9_d_lockdown7f */
    __IO uint32_t ILKDN7              ; /* 0x93C reg9_i_lockdown7f */   
    __IO uint32_t RESERVED10[4];
    __IO uint32_t LKLNEN              ; /* 0x950 reg9_lock_line_eng */
    __IO uint32_t UNLKWAY             ; /* 0x954 reg9_unlock_wayg */
    __IO uint32_t RESERVED11[170];
    __IO uint32_t AFLTST              ; /* 0xC00 reg12_addr_filtering_start */
    __IO uint32_t AFLTEND             ; /* 0xC04 reg12_addr_filtering_end */
    __IO uint32_t RESERVED12[206];
    __IO uint32_t DBGCTRL             ; /* 0xF40 reg15_debug_ctrl */
    __IO uint32_t RESERVED13[7];
    __IO uint32_t PRFCTRL             ; /* 0xF60 reg15_prefetch_ctrl */
    __IO uint32_t RESERVED14[7];
    __IO uint32_t PWRCTRL             ; /* 0xF80 reg15_power_ctrl */

            
}PL310_TypeDef;

#define    L2C_BASE                     0xD1DFB000                              
#define    PL310  (( PL310_TypeDef *) L2C_BASE )

#define L2CENABLE               BIT_0
#define L2CBRESPEN              BIT_30
#define L2CIPRFEN               BIT_29
#define L2CDPRFEN               BIT_28
#define L2C_I_D_PRF_EN          (L2CIPRFEN|L2CDPRFEN)
#define L2CNSINTACCTRL          BIT_27
#define L2CNSLKDNEN             BIT_26
#define L2CPARITYEN             BIT_21
#define L2C_EVT_M_BUS_EN        BIT_20
#define L2C_FORCE_WA_BASE       23
#define L2C_FORCE_WA_MASK       (0x3<<L2C_FORCE_WA_BASE)
#define L2C_FORCE_WA_EN         BIT_24

#define L2C_STANDBY_EN            BIT_0
#define L2C_DYNAMIC_CLK_GATE_EN   BIT_1

#define L2C_EVT_COUNTER_EN      BIT_0
#define L2C_EVT_COUNTER0_RST    BIT_1
#define L2C_EVT_COUNTER1_RST    BIT_2

#define L2C_EVT_CFG_SRC_MASK      0x0000003C

#endif
