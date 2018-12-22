#include "bbu.h"
#include <string.h>
#include "utils.h"
#include "ppset.h"
#include "PMUA.h"
#include "PMUM.h"
#include "gpio_dec.h"
#include "APBSPARE.h"
#include "pmic.h"
#include "timer_dec.h"
#include "geu_fuse.h"
#include "common_tests.h"
#include "GEU.h"
#include "CIU.h"
#include "LCD.h"
#include "MFPR.h"
#include "PMUCP.h"
#include "SQU.h"

uint32_t max_dvc = 0;  //The current max requested DVC level at AP side.
uint32_t DVC_level[2];
uint32_t user_dvc_vote[PLL_USER_NUM]; //maintain current vote
op_index user_cop[PLL_USER_NUM]; //maintain current OP

//from fuse.c
extern uint32_t uiDRO_SVT;
extern uint32_t uiProfile;

static uint32_t verbose_off = 0; //0: with more logs, 1: no logs to make test stress/save-time.

#ifdef CONFIG_HWDFC
/*
When HW-DFC is enabled, user_cop[PLL_DDR] maintains the AP side DFC_LEVEL request.
The actual DDR DFC_LEVEL is NOT the same thing because it may changed by CP/DP request.
*/
#endif

//For the usage statistics of  dynmaics PLL sources (there is only PLL2 in NZ3)
//each pll_user register its usage pll_stat |=  (1<< PLL_DDR) ;
uint32_t pll_stat = 0;
//maintain PLL2 current OP which is in using
uint32_t pll_cop = 0;

//>Seagull: PLL1_416M, PLL1_624M, PLL1_832M, PLL2
//>pclk: PLL1_832, PLL1_1248, PLL2, PLL2P
//>dclk_2x: PLL1_416, PLL1_624, PLL2, PLL2P
//<dclk_1x is what we say DDR speed>
//>aclk: PLL1_416M, PLL1_624M, PLL2, PLL1_312.

//PLL2 possible configurations, I call it PLL's  OP

/*
Fvco must be [1.5G, 3G], NZ3 does not support PLL SSC function control.
N = FVCO/4/(REFCLK/M) = (Fvco/104)*M
M = 1~3 with Fref=26M.
Fvco = (REFCLK / M) * 4*N = 104*N/M;
Fout(CLKOUT) = Fvco / VCODIV
M: pll_ref,Reference clock divider
N: pll_fb, Feedback clock divider
The higher N/M, the better jitter performance
VCODIV: VCODIV_SEL_SE[3:0]/VCODIV_SEL_DIFF[3:0],
*/
static uint32_t setup_pll(pll_src pll_num, op_index pll_top);

static  pll_freq pll_set[] = {
 /*label,  pll_out, pll_outp, pll_ref, pll_fb,   pll_ctrl*/
   {"OP0",  0,         0,          NA,      NA,       NA}, //for PLL1 sources
   {"OP1", 800,        0,          3,      46,    0x1010D0E3},
   {"OP2", 1066,       0,          2,      41,    0x1010E063},
};

//pclk: PLL1_832, PLL1_1248, PLL2, PLL2P
//Note: memclk(axi_m1) must be equal or integer multiple of busclk (aclk_m0) freq!!!!
//General rules in DE's timing requirement is axi_m1=1/2pclk, axi_m0=1/4pclk
core_freq ca7_value[] = {
   /*label,  pclk,   busclk, memclk, pll_source, pll_value,pll_op, fccr[31:29], cc_ap[8:0], dvc, dvcs{profile15..profile0}, dvcsa*/
    {"OP0",   416,    104,    208,     PLL1,      832,      OP0,     0x0<<29,     0xC9,      0,   0x1111100000000001, 0x0000000000000000},
    {"OP1",   156,    78,      78,     PLL1,      1248,     OP0,     0x1<<29,     0x4F,      0,   0x0000000000000000, 0x0000000000000000},
    {"OP2",   208,    104,    104,     PLL1,      832,      OP0,     0x0<<29,     0x4B,      0,   0x0000000000000000, 0x0000000000000000},
    {"OP3",   312,    156,    156,     PLL1,      1248,     OP0,     0x1<<29,     0x4B,      0,   0x0000000000000000, 0x0000000000000000},
    {"OP4",   416,    208,    208,     PLL1,      832,      OP0,     0x0<<29,     0x49,      0,   0x1111100000000001, 0x0000000000000000},
    {"OP5",   624,    156,    312,     PLL1,      1248,     OP0,     0x1<<29,     0xC9,      1,   0x1111111111111001, 0x1111111111110001},
    {"OP6",   832,    208,    416,     PLL1,      832,      OP0,     0x0<<29,     0xC8,      2,   0x2222222222222222, 0x2222222222222222},
    {"OP7",   1066,   266,    533,     PLL2,      1066,     OP2,     0x2<<29,     0xC8,      3,   0x3333333333333333, 0x3333333333333333},
    {"OP8",   1248,   312,    624,     PLL1,      1248,     OP0,     0x1<<29,     0xC8,      3,   0x3333333333333333, 0x3333333333333333},
};

//We have 32 MCK5 tables, can support 30 OPs for DDR at max beside the 1 tables for lpm exit
//The last table (0x1F) is common for all PPs.
//However, the HW-DFC only supporty 8 levels in the register.
//OPs with same freq may have different clock soruce
//TODO: New idea:Can we read the DDR register to identify the type and speed
//TODO: we need to setup tables at bring up stage without SWD/OBM
uint32_t  ddr_type = DDR533M;   // ddr_type&ddr_bvalue[op].valid_mask to check the OP is valid or not
static void hw_dfc_set (op_index ddr_top);

//dclk_2x: PLL1_416, PLL1_624, PLL2, PLL2P
dfc_freq ddr_value[] = {
   /*label, xclk,pll_source, pll_value, pll_op, fccr[24:23], cc_ap[14:12], dvc, dvcs{profile15..profile0}, dvcsa*/
    {"OP0",  104,  PLL1,      416,      OP0,     0x0<<23,     0x1<<12,      0,  0x0000000000000000, 0x0000000000000000},
    {"OP1",  156,  PLL1,      624,      OP0,     0x1<<23,     0x1<<12,      0,  0x0000000000000000, 0x0000000000000000},
    {"OP2",  208,  PLL1,      416,      OP0,     0x0<<23,     0x0<<12,      0,  0x0000000000000000, 0x0000000000000000},
    {"OP3",  312,  PLL1,      624,      OP0,     0x1<<23,     0x0<<12,      0,  0x0000000000000000, 0x0000000000000000},
    {"OP4",  400,  PLL2,      800,      OP1,     0x2<<23,     0x0<<12,      0,  0x0000000000000000, 0x0000000000000000},
    {"OP5",  533,  PLL2,      1066,     OP2,     0x2<<23,     0x0<<12,      1,  0x1111111111000001, 0x1111111111000001},
    {"OP6",  416,  PLL1,      832,      OP0,     0x3<<23,     0x0<<12,      0,  0x0000000000000000, 0x0000000000000000},
};

//Note: if we change DDR OP define, we should change gc_aclk OP define at the same time
ddr_binder  ddr_bvalue[] = {
  /*valid_mask, PMUA_MC_HW_SLP_TYPE[7:3], aclk_op*/
    {DDR_ALL,       0x1<<3,                OP2},
    {DDR_ALL,       0x2<<3,                OP3},
    {DDR_ALL,       0x3<<3,                OP4},
    {DDR_ALL,       0x4<<3,                OP4},
    {DDR_ALL,       0x5<<3,                OP4}, //here support ALL for test purpose, DMFC
    {DDR533M,       0x6<<3,                OP4},
    {DDR533M,       0x7<<3,                OP4},
};

#define   GET_CHIP_DVL4OP_Zx(p, n) \
do {\
  for (uint32_t i = 0; i<=n; i++) \
    (p[i].dvc) = (((p[i].dvcs) >> (4*uiProfile))&0xF); \
} while(0)

#define   GET_CHIP_DVL4OP_Ax(p, n) \
do {\
  for (uint32_t i = 0; i<=n; i++) \
    (p[i].dvc) = (((p[i].dvcsa) >> (4*uiProfile))&0xF); \
} while(0)

#define   HWDFC_LEVEL(n)    PMUA->DFC_LEVEL[n]
#define   HWDFC_LEVEL_SET(n) \
              ((ddr_value[n].dvc << ((cpu_is_pxa1826_a0()) ? 16:12)) | (ddr_bvalue[n].mc_reg_table << 4) \
              | (ddr_value[n].fc_div>>8) | ddr_value[n].fc_pllsel>>23)

#ifdef CONFIG_HWDFC
static void hw_dfc_init()
{
    HWDFC_LEVEL(0) = HWDFC_LEVEL_SET(0);
    HWDFC_LEVEL(1) = HWDFC_LEVEL_SET(1);
    HWDFC_LEVEL(2) = HWDFC_LEVEL_SET(2);
    HWDFC_LEVEL(3) = HWDFC_LEVEL_SET(3);
    HWDFC_LEVEL(4) = HWDFC_LEVEL_SET(4);
    HWDFC_LEVEL(5) = HWDFC_LEVEL_SET(5);
    HWDFC_LEVEL(6) = HWDFC_LEVEL_SET(6);
    //DCLK is in OP0 by SW method already, here trigger a HW-DFC to assure the initil state as DFC_LEVEL0
    hw_dfc_set(OP0);

    /*Set the default register settings for PLL2 in case someone(AP/CP/DP)
    request DFC_LEVEL use PLL2. HW will force the PLL power up if it's not up yet.
    SW need to prepare the register settings in advance.
    We'll support PLL2 change via parking OP for test purpose and do it
    when CP does not require PLL2 DFC_LEVEL (we can control it for test purpose but not in real system).
    */
    /*
    HW-DFC will automatically check/force vote PLL source on/off, and vote HW-DVC.
    SW can vote to shutdown it. HW-DFC will vote to power it up.
    */
    if (ddr_type == DDR400M)
    {
        setup_pll(PLL2, OP1);
        setup_pll(PLL2, OP0);
    }
    else
    {
        setup_pll(PLL2, OP2);
        setup_pll(PLL2, OP0);
    }
}
#endif

//aclk: PLL1_416M, PLL1_624M, PLL2, 312M
//choose: use PLL1 only (PLL1_416M, PLL1_624M)
dfc_freq aclk_value[] = {
   /*label  xclk,  pll_source, pll_value, pll_op, fccr[19], cc_ap[17:15],  dvc, dvcs{profile15..profile0}, dvcsa*/
    {"OP0", 104,     PLL1,      416,       OP0,    0x0<<19,    0x3<<15,     0,  0x0000000000000000, 0x0000000000000000},
    {"OP1", 78,      PLL1,      624,       OP0,    0x1<<19,    0x7<<15,     0,  0x0000000000000000, 0x0000000000000000},
    {"OP2", 104,     PLL1,      416,       OP0,    0x0<<19,    0x3<<15,     0,  0x0000000000000000, 0x0000000000000000},
    {"OP3", 156,     PLL1,      624,       OP0,    0x1<<19,    0x3<<15,     0,  0x0000000000000000, 0x0000000000000000},
    {"OP4", 208,     PLL1,      416,       OP0,    0x0<<19,    0x1<<15,     0,  0x0000000000000000, 0x0000000000000000},
    {"OP5", 312,     PLL1,      624,       OP0,    0x1<<19,    0x1<<15,     1,  0x1111110000000001, 0x1111111000000001}
};

//atclk is synced to trace_clk and share same FC request bit
//OR 0x00008020 to trigger both freq change of trace_clk and dbg_clk
//Since we only have two OPs for the debug clocks.
//coresight: PLL1_416, PLL1_624, PLL2, PLL2P, <use PLL1 only>
cs_freq cs_value[] = {
   /*label atclk, dbg_clk, trace_clk,  |trace_config, dvc, dvcs{profile15..profile0}*/
    {"OP0", 104,   52,      208,         0x00080103,   0,  0x0000000000000000},
    {"OP1", 416,   208,     208,         0x00080100,   0,  0x0000000000000000}
};


static void adfc_set(op_index core_top, op_index ddr_top, op_index axi_top);

static uint32_t vcodiv_sel2value(uint32_t vcodiv_sel)
{
    uint32_t vcodiv;

    switch(vcodiv_sel) {
    case 0:
        vcodiv = 1;
        break;
    case 1:
        vcodiv = 2;
        break;
    case 2:
        vcodiv = 4;
        break;
    case 3:
        vcodiv = 8;
        break;
    case 4:
        vcodiv = 16;
        break;
    case 5:
        vcodiv = 32;
        break;
    case 6:
        vcodiv = 64;
        break;
    case 7:
        vcodiv = 128;
        break;
    default:
        vcodiv = 0;
        bbu_printf("invalid VCODIV_SEL value\r\n");
        break;
    }
    return vcodiv;
}

static uint32_t abs_diff(uint32_t value1, uint32_t value2)
{
   if (value1<value2) {
      return (value2 - value1);
   } else {
      return (value1 - value2);
   }
}

void get_pll(pll_dump *pll_struct)
{
    uint32_t  pllcr = 0, pll_sw_crtl = 0;
    uint32_t  pllfb = 0, pllref = 0, vcodiv_se = 0, vcodiv_diff = 0;
    uint32_t  fbdiv_pos = 5, refdiv_pos = 0, power_pos = 19; //PLL3/4 as default

    if (pll_struct->number == 2) //PLL2 in Z1
    {
        pllcr = PMUM->PLL2CR;
        pll_sw_crtl = APBSPARE->PLL2_SW_CTRL;
        fbdiv_pos = 10;
        refdiv_pos = 19;
        power_pos = 8;
    }
    else
    {

        bbu_printf("Wrong PLL number, not exist!\r\n");
        return;
    }

    if ((pllcr & BIT_9) && ((pllcr & (1 << power_pos)) == 0))
    {
        //PLL is off
        Q_ASSERT(pll_cop == 0);
        pll_struct->pll_out = 0;
        pll_struct->pll_outp = 0;
        return;
    }

    pllfb = (pllcr >> fbdiv_pos) & 0x1FF; //N
    pllref = (pllcr >> refdiv_pos) & 0x1F; //M
    vcodiv_se = (pll_sw_crtl >> 20) & 0x7;
    vcodiv_se = vcodiv_sel2value(vcodiv_se);
    pll_struct->pll_out = ((pllfb * 104) / pllref) / vcodiv_se;
    if (pll_sw_crtl & BIT_23)
    {
        vcodiv_diff = (pll_sw_crtl >> 17) & 0x7;
        vcodiv_diff = vcodiv_sel2value(vcodiv_diff);
        pll_struct->pll_outp = ((pllfb * 104) / pllref) / vcodiv_diff;
    }
    else
    {
        pll_struct->pll_outp = 0;
    }
}

//So, we need to follow below procedure in our code:
//1, clear PMUM_POCR[FORCE], to allow clocks turning off to save power.
//This is especially useful in Chip Sleep mode.
//2, set the PMUM_ACGR to disable unused clocks. I'm using 0xA452.
//3, while enabling PLL2, we need to do 3-steps writing
//                Step 1) write ACGR to enable PLL2
//Step 2) Write PLL2CR with 0x22EA00, leave the "EN" bit cleared.
//                Step 3) Write PLL2CR again with 0x22EB00, set the "EN" bit.
//   Then the PLL2 will be enable. You can do freq-chg to PLL2 or VCTCXO 26M mode after this.
//In SOC active mode, PLL2 ON/OFF is controlled by SW and PMU take control in low power modes

/*pll_num: 2
  pll_top: the target OP of PLLx
  if let pll_top = OP0, then try to close the PLL*/
/*+++++++++++ONLY this function can change PLL in the whole SOC+++++++*/
static uint32_t setup_pll(pll_src pll_num, op_index pll_top)
{
    __IO  uint32_t *pllcr, *pll_sctrl;
    uint32_t  fbdiv_pos=5, refdiv_pos=0, power_pos=19; //PLL3/4 as default
    uint32_t  posr_pos = 0;

    if(pll_cop == pll_top)
    {
       // bbu_printf("the PLL%d is already enabled at desired OP%d.\r\n", pll_num, pll_top);
        return MV_OK;
    }
    else if((pll_cop > OP0)&&(pll_stat != 0)) //it's in use
    {
        //if HW-DFC is enabled,  the PLL2 should not be used CP/DP's DFC_LELVEL.
        //Otherwise, it can't support DFC via parking.
        //We can let CP change it's DFC_LEVEL request as well for test purpose
        if (!(pll_stat & (~(1<<PLL_DDR | 1<<PLL_CORE))))
        {//When PLL2 is only used by DDR or CORE or both.
           if (pll_stat & (1 << PLL_DDR))
           {
#ifdef CONFIG_HWDFC
            //CAUTION: make sure COMMT-HWDFC test is not going on with index=9
            //run a COMMT non-HWDFC test if previous COMMT test is with index=9.
            hw_dfc_set (OP3);
#else
            adfc_set(OP_UNCHANGE, OP3, OP_UNCHANGE); //use 312M as ddr park
#endif
#ifndef CONFIG_MMT
            if (!verbose_off)bbu_printf("DDR parking dfc\r\n");
#endif
           }
            //for test purpose
            if(pll_stat & (1 << PLL_CORE))
            {
            adfc_set(OP6, OP_UNCHANGE, OP_UNCHANGE); //use 832M as core park
#ifndef CONFIG_MMT
            if (!verbose_off)bbu_printf("core parking dfc\r\n");
#endif
            }
        }
        else
        {
#ifndef CONFIG_MMT
        bbu_printf("the PLL2 is in using at different OP%d\r\n Please use PLL1 as park OP\r\n", pll_cop);
#endif
        Q_ASSERT(pll_stat > 0); //someone is using the pll2
        return MV_BUSY;
        }
     }
     else
     {
        if(pll_top == OP0)
          {
#ifndef CONFIG_MMT
            if (!verbose_off)bbu_printf("power off the PLL2 which is not used\r\n");
#endif
          }
        else
          {
#ifndef CONFIG_MMT
            if (!verbose_off)bbu_printf("setup the PLL2 to OP%d...\r\n", pll_top);
#endif
          }
     }

     if (pll_num  == 2)
      {
        pllcr = (__IO uint32_t *)&PMUM->PLL2CR;
        pll_sctrl =  (__IO uint32_t *)&APBSPARE->PLL2_SW_CTRL;

        PMUM->PLL2CR |= BIT_9; //switch to SW control
        fbdiv_pos=10;
        refdiv_pos=19;
        power_pos = 8;
        posr_pos = 29;
      }
      else
      {
#ifndef CONFIG_MMT
        bbu_printf("Wrong PLL number, can't setup!\r\n");
#endif
        return MV_FAIL;
      }

      //step 1: disable the target PLL (reset will assert once power down per DE)
      *pllcr &= ~(1<<power_pos);

      // in case trying to turn off the PLL.
      if(pll_top == OP0)
      {
        pll_cop = pll_top;
        PMUM->PLL2CR &= (~BIT_9); //return to HW control in case HW-DFC
        return MV_OK;
      }
#ifdef CONFIG_HWDFC
      else
      {
        /*The PLLx_PU and DFC_PLLx_PU are ORed to control the PLLx power on/off
         When HW-DFC is enabled and DFC_PLLx_PU is asserted, the PLL will still keep locked.
         Here we need to make sure PLLx is really power off  before going to change the PLLx.
         If this assert is violated, there must be something wrong on the PLL allocation*/
        while((1<<posr_pos)&PMUM->POSR);
      }
#endif
      //step 2: the new PLL dividers
      *pllcr = ((*pllcr) & (~((0x1ff<<fbdiv_pos) | (0x1f<<refdiv_pos)))) | (pll_set[pll_top].pll_ref << refdiv_pos) | (pll_set[pll_top].pll_fb << fbdiv_pos);
      *pll_sctrl = pll_set[pll_top].pll_ctrl;

      //step 3: power up and wait PLL to lock (50uS)
      *pllcr |= (1<<power_pos);
      //udelay(50);
      while(((1<<posr_pos)&PMUM->POSR) == 0);

      //step 5: return to PMU control(only for PLL2)
      if (pll_num  == 2)
      {
    //in current design, HW will auto power up PLL2 once it's switch to HW control
    //it just control the PLL2 like PLL1. so step3  may be not necessary actually
        PMUM->PLL2CR &= ~(BIT_9 | BIT_8);

      }
//#ifdef  CONFIG_PPSET_CHECK
#if  1
      //step 6: check and verify, update the pll_cop
      pll_dump pll_str;
      pll_str.number = pll_num;
      get_pll(&pll_str);
     if ((abs_diff(pll_set[pll_top].pll_out, pll_str.pll_out)<26)&&
         (abs_diff(pll_set[pll_top].pll_outp, pll_str.pll_outp)<26)){
         pll_cop = pll_top;
        }
     else
     {
         pll_cop = OP_INVALID;
#ifndef CONFIG_MMT
         bbu_printf("PLL setup error \r\n") ;
#endif
         return MV_FAIL;
     }
#else
      pll_cop = pll_top;
#endif
      return MV_OK;
}


op_index aop_check_dump(int32_t top)
{
    uint32_t pll_dragon = 0, read_reg = 0, temp_reg = 0;
    uint32_t apclk = 0, abusclk = 0, amemclk = 0;
    pll_dump pll_str;
    op_index core_cop;

    read_reg = PMUA->PLL_SEL_STATUS;
    pll_dragon = ((read_reg & 0xC) >> 2);

    switch (pll_dragon)
    {
    case 0:
        pll_dragon = 832;
        break;
    case 1:
        pll_dragon = 1248;
        break;
    case 2:
        pll_str.number = 2;
        get_pll(&pll_str);
        pll_dragon = pll_str.pll_out;
        break;
    case 3:
        pll_str.number = 2;
        get_pll(&pll_str);
        pll_dragon = pll_str.pll_outp;
        break;
    }

    //Read the current status
    read_reg = PMUA->DM_CC_AP;
    PMUA->CC_AP |= BIT_31;
    PMUA->CC_AP &= ~BIT_31;

    temp_reg = read_reg & 7;
    apclk = pll_dragon / (temp_reg + 1) ;

    temp_reg = (read_reg & 0x38) >> 3;
    amemclk = apclk / (temp_reg + 1) ;

    temp_reg = (read_reg & 0x1C0) >> 6;
    abusclk = apclk / (temp_reg + 1) ;

    uint32_t i = 0;
    while (i <= (uint32_t)OP_COREMAX)
    {
        if ((abs_diff(ca7_value[i].pclk, apclk) < 26) &&
        (abs_diff(ca7_value[i].memclk, amemclk) < 26) &&
        (abs_diff(ca7_value[i].busclk, abusclk) < 26))
        {
            core_cop = (op_index)i;
           // if ((core_cop == OP0) && (top == OP0 || top == OP2))core_cop = top;
            bbu_printf("The current CA7 core works at OP%d: pclk = %d, memclk= %d, busclk = %d\r\n",
            core_cop, apclk, amemclk, abusclk);
            break;
        }
        else
        {
            i++;
            core_cop = OP_INVALID;
        }
    }

    user_cop[PLL_CORE] = core_cop;
    if(user_cop[PLL_CORE] == OP_INVALID)bbu_printf("Warning: the current CA7 freq is INVALID OP!\r\n");
    return user_cop[PLL_CORE];
}

op_index ddrop_check_dump(op_index top)
{
    uint32_t pll_dclk = 0, read_reg = 0, temp_reg = 0;
    uint32_t dclk = 0;  //dclk_1x
    pll_dump pll_str;
    op_index  ddr_cop; //it's volatile OP, all AP/CP/DP can change it with HW-DFC

    read_reg = PMUA->PLL_SEL_STATUS;
    pll_dclk = (read_reg & 0x30) >> 4;

    switch (pll_dclk)
    {
    case 0:
        pll_dclk = 416;
        break;
    case 1:
        pll_dclk = 624;
        break;
    case 2:
        pll_str.number = 2;
        get_pll(&pll_str);
        pll_dclk = pll_str.pll_out;
        break;
    case 3:
        if(cpu_is_pxa1826_zx()) {
          pll_str.number = 2;
          get_pll(&pll_str);
          pll_dclk = pll_str.pll_outp;
        } else {
          pll_dclk = 832;
        }
        break;
    }

   //Read the current status
    read_reg = PMUA->DM_CC_AP;
    PMUA->CC_AP |= BIT_31;
    PMUA->CC_AP &= ~BIT_31;
    temp_reg = (read_reg & 0x7000) >> 12;

    dclk = pll_dclk / (temp_reg + 1) /2;

    uint32_t i = 0;
    while (i<= (uint32_t)OP_DDRMAX) {
            if(abs_diff(ddr_value[i].xclk, dclk)<13) {
              ddr_cop = (op_index)i;
           // if ((ddr_cop == OP0) &&(top == OP0 || top == OP1))ddr_cop = top; // lowest OP0 and OP1 has same dclk.
#ifdef CONFIG_HWDFC
            Q_ASSERT(ddr_cop >= top);
            user_cop[PLL_DDR] = top; //the request of AP is fullfilled.
#else
            user_cop[PLL_DDR] = ddr_cop;
#endif
            bbu_printf("The current DDR freq is OP%d: dclk = %d\r\n", ddr_cop, dclk);
              break;
          }
        else
        {
            i++;
            ddr_cop = OP_INVALID;
        }
    }
    if(ddr_cop == OP_INVALID)
      {
        user_cop[PLL_DDR] = OP_INVALID;
        bbu_printf("Warning: the current DDR freq is INVALID OP!\r\n");
      }
      return ddr_cop;
}

op_index axiop_check_dump(op_index top)
{
    uint32_t pll_aclk = 0, read_reg = 0, temp_reg = 0;
    uint32_t aclk = 0;
    pll_dump pll_str;
    op_index  axi_cop;

    read_reg = PMUA->PLL_SEL_STATUS;
    pll_aclk = (read_reg & 0xC0) >> 6 ;

    switch (pll_aclk)
    {
    case 0:
        pll_aclk = 416;
        break;
    case 1:
        pll_aclk = 624;
        break;
    case 2:
        pll_str.number = 2;
        get_pll(&pll_str);
        pll_aclk = pll_str.pll_out;
        break;
    case 3:
        pll_aclk = 312;
        break;
    }

   //Read the current status
    read_reg = PMUA->DM_CC_AP;
    PMUA->CC_AP |= BIT_31;
    PMUA->CC_AP &= ~BIT_31;
    temp_reg = (read_reg & 0x38000) >> 15;

    aclk = pll_aclk / (temp_reg + 1) ;

    uint32_t i = 0;
    while (i <= (uint32_t)OP_AXIMAX)
    {
        if (abs_diff(aclk_value[i].xclk, aclk) < 26)
        {
            axi_cop = (op_index)i;
            if ((axi_cop == OP0) && (top == OP0 || top == OP2))axi_cop = top;
            bbu_printf("The current AXI freq is OP%d: aclk = %d\r\n", axi_cop, aclk);
            break;
        }
        else
        {
            i++;
            axi_cop = OP_INVALID;
        }
    }

    user_cop[PLL_AXI] = axi_cop;
    if(user_cop[PLL_AXI] == OP_INVALID)bbu_printf("Warning: the current AXI freq is INVALID OP!\r\n");

    return user_cop[PLL_AXI];
}


static void dump_pllop( unsigned int op_index)
{
        pll_freq *md = &(pll_set[op_index]);

        bbu_printf("%d(%s)---", op_index, md->label);
        bbu_printf("pll_out:%d pll_outp:%d\r\n",
                        md->pll_out, md->pll_outp);
}

void dump_pll_list(void)
{
    unsigned int i;
    bbu_printf("PLL OP list \r\n");
    for (i=0; i <= OP_PLLMAX; i++)
    {
        dump_pllop(i);
    }
}

static void dump_aop( unsigned int op_index)
{
        core_freq *md = &(ca7_value[op_index]);

        bbu_printf("%d(%s)---", op_index, md->label);
        bbu_printf("pclk:%d memclk:%d busclk:%d"
                        " pll_source:%d pll_op:%d dvc:%d\r\n",
                        md->pclk, md->memclk, md->busclk,
                        md->pll_source, md->pll_op, md->dvc);
}

//as a help function information
void dump_aop_list(void)
{
    unsigned int i;
    bbu_printf("AP core OP list \r\n");
    for (i=0; i <= OP_COREMAX; i++)
    {
        dump_aop(i);
    }
}

op_index aop_get(void)
{
  return user_cop[PLL_CORE];
}

op_index ddrop_get(void)
{
  return  ddrop_check_dump(user_cop[PLL_DDR]);
}

op_index axiop_get(void)
{
  return user_cop[PLL_AXI];
}

static void dump_cur_aop(void)
{
    bbu_printf("CA7 Current Operating Point is %d\r\n", user_cop[PLL_CORE]);
    dump_aop(user_cop[PLL_CORE]);
}

static void dump_ddrop( unsigned int op_index)
{
    dfc_freq *md = &(ddr_value[op_index]);

    bbu_printf("%d(%s)---", op_index, md->label);
    bbu_printf("dclk:%d pll_source:%d pll_op:%d  dvc:%d\r\n",
                    md->xclk, md->pll_source, md->pll_op, md->dvc);
}

//as a help function information
void dump_ddrop_list(void)
{
    unsigned int i;
    bbu_printf("DDR OP list \r\n");
    for (i=0; i <= OP_DDRMAX; i++)
    {
        dump_ddrop(i);
    }
}

void dump_cur_ddrop(void)
{
    bbu_printf("Current DDR OP requested by AP is OP%d\r\n", user_cop[PLL_DDR]);
    dump_ddrop(user_cop[PLL_DDR]);
#ifdef  CONFIG_HWDFC
    op_index ddr_cop;
    ddr_cop = ddrop_check_dump(user_cop[PLL_DDR]);
    Q_ASSERT(ddr_cop == ((PMUA->DFC_STATUS & 0xE) >> 1));
    if (ddr_cop != user_cop[PLL_DDR])
      {
        bbu_printf("The actual DDR freq is OP%d\r\n", ddr_cop);
        dump_ddrop(ddr_cop);
      }
#endif
}

static void dump_axiop( unsigned int op_index)
{
    dfc_freq *md = &(aclk_value[op_index]);

    bbu_printf("%d(%s)---", op_index, md->label);
    bbu_printf("aclk:%d dvc:%d\r\n", md->xclk, md->dvc);
}

//as a help function information
void dump_axiop_list(void)
{
    unsigned int i;
    bbu_printf("AXI OP list \r\n");
    for (i=0; i <= OP_AXIMAX; i++)
    {
        dump_axiop(i);
    }
}

void dump_cur_axiop(void)
{
    bbu_printf("AXI Current Operating Point is %d\r\n", user_cop[PLL_AXI]);
    dump_axiop(user_cop[PLL_AXI]);
}

#ifdef CONFIG_HWDFC
static void hw_dfc_set (op_index ddr_top)
{
    uint32_t read_data = 0;
    //Caution: this function must be called with PMU_DM_CC_AP  AP read status released.
    //Otherwise, the DFC_STATUS bit will think there is HW_DFC progress ongoing and HW can't get the lock.
    if (ddr_top == OP_UNCHANGE)
        return;
    else
        Q_ASSERT(ddr_top <= OP_DDRMAX);

    //+++Step3: set up correct PMUA_DEBUG value.
    read_data = PMUA->DEBUG_REG;

    read_data |= (BIT_21 | BIT_22); //allow AP FC when CP in wfi

    //only AP can reset/release CP, so CP is in reset state or not is certain during FC process.
    if (PMUM->APRR & 0x1)   //CP is hold in reset
    {
        /* Change the AP freq clk, ignore CP, this is for AP standalone debug */
        read_data |= (BIT_0 | BIT_3);
    }
    else
    {
        //CP is already released
        /* Need to wait CP's ACK on clock off */
        read_data &= ~BIT_3;
    }

    PMUA->DEBUG_REG = read_data;

    //+++Step8:Seagull and AP allow freq. change voting
    //PMUA->CC_CP |= BIT_27;
    //Note: PMUCP DFC_CP[15]/DFC_DP[15] default to 1, allowed
    PMUA->CC_AP |= BIT_27;

    PMUA->DFC_AP = (((uint32_t)ddr_top) << 1) | 1;
    while (PMUA->DFC_AP & BIT_0); //wait the DFC finish.
    return;
}
#endif

/* 1,excluse between AP and CP
  Because AP and CP shared the same FC state machine in Emei.
  The two dummy registers are used to make sure that AP and CP does FC exclusively.
  So, each PP change process need to get the control of fc_sm at the very beginning.
   2. excluse between AP cores via spinlock
*/
static void get_fc_sm()
{
  while (PMUA->DM_CC_AP & BIT_24) {
  //if fail to get control once, clear the flag to avoid deadlock with CP
    PMUA->CC_AP |= (BIT_31);
    PMUA->CC_AP &= ~(BIT_31);
  }
}

static void put_fc_sm()
{
    uint32_t read_data = 0;

    read_data = PMUA->CC_AP;
    read_data = (read_data & 0x08FFFFFF) | BIT_31; //RD_ST clear
    PMUA->CC_AP = read_data;
    PMUA->CC_AP = read_data & 0x08FFFFFF;
}

/*AP core/ddr/axi freq change function, the caller should make sure the x_top are valid OP index*/
static void adfc_set(op_index core_top, op_index ddr_top, op_index axi_top)
{
    uint32_t read_data = 0 , fc_trigger = 0;

    if ((core_top == OP_UNCHANGE) && (ddr_top == OP_UNCHANGE) && (axi_top == OP_UNCHANGE))
    {
      return ;
    }

    //Now we get the control of fc_sm, CP will not change any PLL2/3/4 settings.
    //fc_lock protect the share fc_sm among all the core/ddr/axi DFC in AP and CP.
    get_fc_sm();

    //+++Step3: set up correct PMUA_DEBUG value.
    read_data = PMUA->DEBUG_REG;

    read_data |= (BIT_21|BIT_22); //allow AP FC when CP in wfi

    //only AP can reset/release CP, so CP is in reset state or not is certain during FC process.
    if (PMUM->APRR & 0x1) { //CP is hold in reset
        /* Change the AP freq clk, ignore CP, this is for AP standalone debug */
        read_data |= (BIT_0|BIT_3);
    }
    else
    { //CP is already released
        /* Need to wait CP's ACK on clock off */
        read_data &= ~BIT_3;
    }

    PMUA->DEBUG_REG = read_data;

    //+++Step8:Seagull and AP allow freq. change voting
    //PMUA->CC_CP |= BIT_27;
    //Note: PMUCP DFC_CP[15]/DFC_DP[15] default to 1, allowed
    PMUA->CC_AP |= BIT_27;

    /*per DE: all PMUM registers are in APB side(26MHz domain) while PMUA registers are in AXI side(alck domain).
     The time gap between the write response of FCCR and the exact update of FCCR is more than six 26MHz cycles.
     WR: make sure to do a FCCR read after write (RAW) to make sure it's really updated for application.
     That's SOC issue.  while dsb() only regulate the core's behavior, no use here.
    */
    //+++Step4:Set the clock source and dividers which only take effect after issue FC request
    if(ddr_top != OP_UNCHANGE)
      {
        read_data = PMUM->FCCR & ~(BIT_24 | BIT_23);
        PMUM->FCCR = read_data | ddr_value[ddr_top].fc_pllsel;
        PMUA->CC_AP &= ~0x7000; //[14:12]
        PMUA->CC_AP |= ddr_value[ddr_top].fc_div;

        PMUA->MC_HW_SLP_TYPE &= ~0xF8; //[7:3]
        PMUA->MC_HW_SLP_TYPE |= ddr_bvalue[ddr_top].mc_reg_table;
        fc_trigger |= (ddr_top != OP_UNCHANGE) ?  BIT_25 : 0;
      }
    if(core_top != OP_UNCHANGE)
      {
        read_data = PMUM->FCCR & ~(BIT_31 | BIT_30 | BIT_29);
        PMUM->FCCR = read_data | ca7_value[core_top].fc_pllsel;
        PMUA->CC_AP &= ~0x1ff; //[8:0]
        PMUA->CC_AP |= ca7_value[core_top].fc_div;
        fc_trigger |= (core_top != OP_UNCHANGE) ? BIT_24: 0;
      }
    if(axi_top != OP_UNCHANGE)
      {
        read_data = PMUM->FCCR & ~(BIT_25 | BIT_19);
        PMUM->FCCR = read_data | aclk_value[axi_top].fc_pllsel;
        PMUA->CC_AP &= ~0x38000; //[17:15]
        PMUA->CC_AP |= aclk_value[axi_top].fc_div;
        fc_trigger |= (axi_top != OP_UNCHANGE) ? BIT_26: 0;
      }
    read_data = PMUM->FCCR; //dummy read

    //+++Step5:enable FC done interrupt by AP/BUS/DDR Intr mask
    //BIT1 is set at T after all BIT3|BIT4|BIT5 is set. T is PMU internal mataince time after FC is done.
    PMUA->AP_IMR |= BIT_1; //BIT1=BIT3|BIT4|BIT5@delay T
    PMUA->AP_IRWC &= ~BIT_1; //interrupt set to write 0 clear
    PMUA->AP_ISR &= ~BIT_1; //clear the interrupt of previous DFC (in MMT, nano will do HW-DFC where ISR is set)

    //+++Step6:  kick off the sm_fc
   // fc_trigger = (ddr_top != OP_UNCHANGE) ?  BIT_25 : (axi_top != OP_UNCHANGE) ? BIT_26 : BIT_24;
   /*there is a bug until Helan2_Z1, i.e. JIRA Helan2-65
     The higher level trigger only updates clock source but not the divider.
     Thus, we still need to set the specific trigger indiviually.  It will be fixed in the future products
     The trigger is distributed in the above code
     */
    PMUA->CC_AP |= (fc_trigger | 0xFC0000);  //[23:18]

    //+++Step7: wait for AP freq change done (there are handshake between PMU and cores/axi/ddr)
    while ( !(BIT_1 & PMUA->AP_ISR) ) ;
    //+++Step8: Clear the AP freq change done interrupt status
    //write 1 to bit has no effect, write 0 clear. It's decided by AP_IRWC register
    PMUA->AP_ISR &= ~BIT_1;

    //clear of RD_ST to indicate APP FC is done
    //This bit and the FC request bits should have already been cleared by HW???
    put_fc_sm();

    //+++Step9(opt):calibrate the system tick count according to new PP if the CPU local timers are used.
}

static void update_memory_xtc(uint32_t pclk)
{
  if(cpu_is_pxa1826_z1z2())
  {
     CIU->CA7_CPU_SRAM_CONF_1 = 0x00008088; //L2 RTC/WTC
     if (pclk <= 312)
       CIU->CA7_CPU_SRAM_CONF_0 = 0x00000000; //L1 RTC/WTC
     else if (pclk <= 416)
       CIU->CA7_CPU_SRAM_CONF_0 = 0x00444444; //L1 RTC/WTC
     else if (pclk <= 624)
       CIU->CA7_CPU_SRAM_CONF_0 = (uiDRO_SVT<=378) ?  0x00999999 : 0x00555555; //L1 RTC/WTC
     else if (pclk <= 832)
       CIU->CA7_CPU_SRAM_CONF_0 = (uiDRO_SVT<=378) ?  0x00dddddd : 0x00999999; //L1 RTC/WTC
     else //1066 and 1248
       CIU->CA7_CPU_SRAM_CONF_0 = 0x00dddddd; //L1 RTC/WTC
  } else { //z3&A0
     if(cpu_is_pxa1826_z3())
      CIU->CA7_CPU_SRAM_CONF_1 = 0x00001088; //L2 RTC/WTC
     else //A0
      CIU->CA7_CPU_SRAM_CONF_1 = (pclk <= 832) ? 0x1088 : 0x5088;

     if (pclk <= 416)
      CIU->CA7_CPU_SRAM_CONF_0 = 0x00111111; //L1 RTC/WTC
     else if (pclk <= 624)
      CIU->CA7_CPU_SRAM_CONF_0 = 0x00555555; //L1 RTC/WTC
     else if (pclk <= 1066)
      CIU->CA7_CPU_SRAM_CONF_0 = 0x00999999; //L1 RTC/WTC
     else
      CIU->CA7_CPU_SRAM_CONF_0 = 0x00AAAAAA; //L1 RTC/WTC
  }
}

#define VOL_LEVELS      4
#define NUM_PROFILES    16

/* FIXME: must remove the table after has fuse info !!! */
struct svtrng {
  uint32_t min;
  uint32_t max;
  uint32_t profile;
};

static struct svtrng svtrngtb_z1[] = {
  {1, 301, 14},
  {302, 314, 13},
  {315, 326, 12},
  {327, 338, 11},
  {339, 350, 10},
  {351, 363, 9},
  {364, 375, 8},
  {376, 387, 7},
  {388, 400, 6},
  {401, 412, 5},
  {413, 424, 4},
  {425, 437, 3},
  {438, 449, 2},
  {450, 0xfff,1}
  /* NOTE: by default use profile 0 */
};

static struct svtrng svtrngtb[] = {
  {1  , 310, 15},
  {311, 322, 14},
  {323, 335, 13},
  {336, 348, 12},
  {349, 360, 11},
  {361, 373, 10},
  {374, 379, 9},
  {380, 386, 8},
  {387, 392, 7},
  {393, 398, 6},
  {399, 405, 5},
  {406, 411, 4},
  {412, 417, 3},
  {418, 424, 2},
  {425, 0xfff, 1}
  /* NOTE: by default use profile 0 */
};

static uint32_t convert_svtdro2profile(uint32_t uisvtdro)
{
  uint32_t profile = 0, idx;

  if(cpu_is_pxa1826_z1z2())
  {
    for (idx = 0; idx < ARRAY_SIZE(svtrngtb_z1); idx++) {
      if (uisvtdro >= svtrngtb_z1[idx].min &&
        uisvtdro <= svtrngtb_z1[idx].max) {
        profile = svtrngtb_z1[idx].profile;
        break;
      }
    }
  }
  else
  {
    for (idx = 0; idx < ARRAY_SIZE(svtrngtb); idx++) {
      if (uisvtdro >= svtrngtb[idx].min &&
        uisvtdro <= svtrngtb[idx].max) {
        profile = svtrngtb[idx].profile;
        break;
      }
    }
  }
  return profile;
}

uint32_t getprofile(uint32_t uifuses)
{
    uint32_t uitemp = 3, uitemp2 = 1, profile = 0;
    uint32_t i;

  if(cpu_is_pxa1826_z1z2())
    return  convert_svtdro2profile(uiDRO_SVT);
  else if(uifuses)
  {
    for (i = 1; i < NUM_PROFILES; i++) {
            uitemp |= uitemp2 << i;
            if (uitemp == uifuses)
                    profile = i;
    }
    return profile; //default is 0
  }
  else
  {
    return  convert_svtdro2profile(uiDRO_SVT);
  }
}

static int svc_table_z1[][VOL_LEVELS] = {
  {1013, 1075, 1188, 1300}, /* profile 0 */
  {913, 963, 1025, 1138}, /* profile 1 */
  {913, 963, 1025, 1150}, /* profile 2 */
  {925, 975, 1038, 1163}, /* profile 3 */
  {938, 988, 1050, 1175}, /* profile 4 */
  {950, 1000, 1063, 1188},  /* profile 5 */
  {950, 1000, 1075, 1200},  /* profile 6 */
  {963, 1013, 1088, 1213},  /* profile 7 */
  {963, 1025, 1100, 1225},  /* profile 8 */
  {975, 1038, 1113, 1238},  /* profile 9 */
  {988, 1038, 1125, 1250},  /* profile 10 */
  {988, 1050, 1150, 1263},  /* profile 11 */
  {1000, 1063, 1163, 1275}, /* profile 12 */
  {1000, 1075, 1175, 1288}, /* profile 13 */
  {1013, 1075, 1188, 1300}, /* profile 14 */
};

static int svc_table_z3[][VOL_LEVELS] = {
  {1025, 1100, 1175, 1300}, /* profile 0 */
  { 975, 1013, 1038, 1163}, /* profile 1 */
  { 975, 1025, 1050, 1163},
  { 975,  988, 1050, 1175},
  { 975,  988, 1063, 1188},
  { 975, 1000, 1063, 1188}, /* profile 5 */
  { 975, 1000, 1075, 1200},
  { 975, 1013, 1075, 1200},
  { 988, 1013, 1088, 1213},
  { 988, 1025, 1100, 1225},
  { 988, 1038, 1113, 1238}, /* profile 10 */
  { 975, 1050, 1125, 1250},
  { 988, 1063, 1138, 1263},
  {1000, 1075, 1150, 1275},
  {1013, 1088, 1163, 1300},
  {1025, 1100, 1175, 1300}  /* profile 15 */
};

static int svc_table_a0[][VOL_LEVELS] = {
  {1025, 1088,  1150,  1300}, /* profile 0 */
  {975,  988,   1025,  1163}, /* profile 1 */
  {975,  1000,  1038,  1163},
  {975,  1000,  1038,  1175},
  {975,  988,   1050,  1188},
  {975,  988,   1050,  1188}, /* profile 5 */
  {975,  1000,  1063,  1200},
  {975,  1000,  1063,  1200},
  {975,  1013,  1075,  1213},
  {975,  1013,  1075,  1225},
  {988,  1025,  1088,  1238}, /* profile 10 */
  {975,  1038,  1100,  1250},
  {988,  1050,  1113,  1263},
  {1000  1063,  1125,  1275},
  {1013  1075,  1138,  1288},
  {1025  1088,  1150,  1300}  /* profile 15 */
};

/* In case SW DVC case,  CP can't adjust the voltage and AP is the owner
 And even the DVL0 should be higher enough to cover all CP/DP PPs.
 That's why we need HW-DVC!!!!
*/

uint32_t * svc_init_table;

/* This function must be called after svc_init_table is initialized
*/
/* VBUCK1 supplies VCC_MAIN_CORE */
void vbuck1_dvc_init(void)
{
   pmic_vbuck_cfg(VBUCK_ON, VBUCK1, svc_init_table[DVC00], DVC00);
   pmic_vbuck_cfg(VBUCK_ON, VBUCK1, svc_init_table[DVC01], DVC01);
   pmic_vbuck_cfg(VBUCK_ON, VBUCK1, svc_init_table[DVC10], DVC10);
   pmic_vbuck_cfg(VBUCK_ON, VBUCK1, svc_init_table[DVC11], DVC11);
   pmic_vbuck_cfg(VBUCK_ON, VBUCK1, 700, SLEEP);
}

void vbuck2_dvc_init(void)
{
   pmic_vbuck_cfg(VBUCK_ON, VBUCK2, 1800, ACTIVE);
   pmic_vbuck_cfg(VBUCK_ON, VBUCK2, 1800, SLEEP);
}

/* VBUCK2 should always set to 1.2V*/
void vbuck3_dvc_init(void)
{
   pmic_vbuck_cfg(VBUCK_ON, VBUCK3, 1200, ACTIVE);
   pmic_vbuck_cfg(VBUCK_ON, VBUCK3, 1200, SLEEP);
}

void vbuck4_dvc_init(void)
{
   pmic_vbuck_cfg(VBUCK_ON, VBUCK4, 1800, DVC00);
   pmic_vbuck_cfg(VBUCK_ON, VBUCK4, 1800, DVC01);
   pmic_vbuck_cfg(VBUCK_ON, VBUCK4, 1800, DVC10);
   pmic_vbuck_cfg(VBUCK_ON, VBUCK4, 1800, DVC11);
}

static void soc_dvc_init(void)
{
    //Here all units core/ddr/vpu/gpu share 4 levels provided by PMIC.
    //We set the nominal values provided by SV team (=Vmin+3%) while Vmax=Vnom_highest+3%
    uint32_t profile = 0;

    profile = uiProfile;
    if(cpu_is_pxa1826_z1z2())
      svc_init_table = &(svc_table_z1[profile][0]);
    else if(cpu_is_pxa1826_z3())
      svc_init_table = &(svc_table_z3[profile][0]);
    else if(cpu_is_pxa1826_a0())
      svc_init_table = &(svc_table_a0[profile][0]);
    else
      bbu_printf("Error: unsupported stepping!\r\n");

#ifdef CONFIG_HWDVC     //HW-DVC
    //10uS for SOC to PMIC DVC pin response/debounce
    PMUM->DVC_EXTRA_STR = 26*15; //it's said 15us is more sure since PMIC diverse.
    /*
    1,all above is just a change request to PMU!
    2,deeper lpm modes has high priority vote in PMU view
    */
    //Here assumes the PMIC set the ramp rate to 12.5mV/uS, otherwise need change!
  if (cpu_is_pxa1826_a0())
  {
    PMUM->VL01STR = (((svc_init_table[DVC01] - svc_init_table[DVC00]) * 2 + 24) / 25) * 26;
    PMUM->VL12STR = (((svc_init_table[DVC10] - svc_init_table[DVC01]) * 2 + 24) / 25) * 26;
    PMUM->VL23STR = (((svc_init_table[DVC11] - svc_init_table[DVC10]) * 2 + 24) / 25) * 26;
  } else {
    PMUM->VL01STR_Zx = (((svc_init_table[DVC01] - svc_init_table[DVC00]) * 2 + 24) / 25) * 26;
    PMUM->VL12STR_Zx = (((svc_init_table[DVC10] - svc_init_table[DVC01]) * 2 + 24) / 25) * 26;
    PMUM->VL23STR_Zx = (((svc_init_table[DVC11] - svc_init_table[DVC10]) * 2 + 24) / 25) * 26;
  }
  //Enable the DVC from SOC side,  it will really take effect after  PMIC enable DVC!
    PMUM->DVCR = 0x3;
#endif
}

/*
*NOTE: be careful about the DVC[1:0] initial value, at the first bring up stage,
****************************************************************************************
*                       Power allocation on HelanLTE DKB
****************************************************************************************
*   VBUCK1 -->  Vcc_main,   default 1.1V, default on,
*   VBUCK2 -->  RF,         default 1.8V, default on, should always on
*   VBUCK3 -->  VDD2...     default 1.2V, default on, should always on
*   VBUCK4 -->  GPIO...,    default 1.8V, default on, should always on
*   VBUCK5 -->  RF PA,      default 1.1V, default off
*     LDO2 -->  AVDD18_PLL, default 1.8V, default off, should always on
****************************************************************************************
*/

static void pmic_dvc_init(void)
{
#ifndef PMIC_DUMMY
    uint8_t val;
    //set POWER_HOLD to enable DVC and reset audio related registers to zero
    //NOTE: this should be before buck1 DVC init, otherwise the the Vcc_main
    //will be set to DVC00 voltage, causing boot hang.
    ustica_base_modify(0x0D, 0xC0, 0xC0);

    //Disable PMIC watchdog
    ustica_base_write(0x1D, 0x01);

    //Increase the current limit of Buck1, otherwise PMIC can't survive mlucas+GC MMO6 stress.
    //set VBUCK1 to full drive,VBUCK1->VCC_MAIN
    ustica_power_modify(0x3A, 0x0f, 0x0f);

    //set USE_XO=1 to indicate that the 32k XO is settled (usually 1s after SOD power-up,
    //bootrom/obm should cost that already, so we don't wait here.)
    //According to PMIC datasheet chapter 5.2 table 24: the 32k XO Tstart = 1s typical and 10s maximum.
    ustica_base_modify(0xD0, 0x80, 0x80);

    //and disable FLL OSC_FREERUN mode to lock PMIC bucks/etc to use the XO clock instead
    ustica_base_modify(0x50, 0x00, 0x3); //OSC_FREERUN ->0
    ustica_base_modify(0x55, 0x00, 0x3); //OSCD_FREERUN ->0

    //select the internal filter capacitance value to 22pF for XTAL1 and XTAL2 pins so that 32KHz is stable
    #if 1  //for some customer like SS has external capacitor, this setting will cause PMIC deadlock
    ustica_base_modify(0xE8, 0x70, 0x70);
    #endif

    //32k_OUTx_SEL: output PMIC internal 32KHz or 32KHz from XO (if USE_XO=1). also enable Power up good monoitor
    //The set of 32k_OUT_SEL is kept in "power-down" state, it resets to default only in RTC POR.
    ustica_base_modify(0xE2, 0x5, 0xf);

    //VBUCK1,4&LDO1 have DVC control, others have sleep/active difference
    //Here we rely on the defalut ramp rate 12.5mV/uS, or need to support set it
    vbuck1_dvc_init();
    udelay(50);
    vbuck2_dvc_init();
    vbuck3_dvc_init();
    vbuck4_dvc_init();

    ustica_base_read(0x10, &val);
    //bbu_printf("pup log: 0x%2x\n\r", val);
    ustica_base_read(0xE5, &val);
    bbu_printf("pdown log1: 0x%02x\n\r", val);
    ustica_base_write(0xE5, val);
    ustica_base_read(0xE6, &val);
    bbu_printf("pdown log2: 0x%02x\n\r", val);
    ustica_base_write(0xE6, val);

    //switching the host 32k to XO clock for accuracy(+/-30ppm)
    PMUM->SCCR |= 0x1;
    pm830_bat_prio_en(1);
#endif
}

/*each DVC_AP/DVC_CP register has only one entry funtion to update.
Only those PP change function can call these dvc functions.
The PP change function are spinlock pretected so that the dvc functions are pretected.
*/

//assuming DVC[2:1] relates to level[2:1] value and high level conrespondes to high voltage.
//For AP: initiator use the enum index in pll_user type.

static uint32_t dvc_vote_check(pll_user initiator, op_index top)
{
    uint32_t  max_dvc_next = 0;  //DVL0 as start

    switch (initiator)
    {
      case PLL_CORE:
            user_dvc_vote[PLL_CORE] = ca7_value[top].dvc;
            break;
      case PLL_DDR:
            user_dvc_vote[PLL_DDR] = ddr_value[top].dvc;
            break;
      case PLL_AXI:
            user_dvc_vote[PLL_AXI] = aclk_value[top].dvc;
            break;
      case PLL_CORESIGHT:
            user_dvc_vote[PLL_CORESIGHT] = cs_value[top].dvc;
            break;
      default: //unknow initator
            user_dvc_vote[initiator] = 0; //DVL0
            break;
    }

    for (uint32_t i = 0; i < PLL_USER_NUM; i++)
    {
        if (user_dvc_vote[i] > max_dvc_next) max_dvc_next = user_dvc_vote[i];
    }

    return max_dvc_next;
}

/*This should be called by PMU_AO init*/
void pp_init()
{
    int i;

    /*
    BIT_10: 0-enable table-based DDR clock change
    BIT_11: 0- use 2:1 mode for LPDDR2/3, set in BLF file instead
    BIT_8:  0-enable the lpm exit ddr reg table 0
    BIT_9: 0- enable auto DLL update during HW DVC
     */
    PMUA->MC_HW_SLP_TYPE &= ~(BIT_8 | BIT_10);

    //AP_DMA XTC
    CIU->AP_DMA_AXI_XTC = 0x10;
    //SQU->CTRL_0 = 0x10000000; //bootrom do it, otherwise hang since nano in SQU

    /* VDDL = 5 for A0+ stepping */
    if (!(cpu_is_pxa1826_z3() || cpu_is_pxa1826_z1z2())) {
	    for (i = 0; i <= OP_PLLMAX; i++) {
		if(pll_set[i].pll_out != 0)
			pll_set[i].pll_ctrl |= BIT_3;
	    }
    }

#ifndef CONFIG_HWDVC
//both AP and CP should know the GPIO for SW DVC
    DVC_level[1] = GPIO127; //SM_BE1,FUNCTION 0
    DVC_level[0] = GPIO13;  //SM_SCLK, FUNCTION 3
#endif

    user_cop[PLL_CORE] = OP0;
    user_cop[PLL_DDR] = OP0;
    user_cop[PLL_AXI] = OP0;
    user_cop[PLL_CORESIGHT] = OP0;

    //write initial dvc value
    soc_dvc_init(); //set up the SOC DVC init settings, both SW-DVC or HW-DVC

#ifdef CONFIG_HWDFC
    //HW-DFC always assume the system using DFC_LEVEL0 at power up.
    adfc_set(OP_UNCHANGE, OP0, OP_UNCHANGE);
    bbu_printf("the ddr changed from OP%d to OP0\r\n", user_cop[PLL_DDR]);
#endif

    aop_check_dump(OP0);
    ddrop_check_dump(OP0);
    axiop_check_dump(OP0);

    if(user_cop[PLL_CORE] == OP_INVALID)
      {
        adfc_set(OP0, OP_UNCHANGE, OP_UNCHANGE);
        bbu_printf("the core changed from INVALID_OP to OP0\r\n");
        aop_check_dump(OP0);
       }
    if(user_cop[PLL_CORE] != OP0)
    {
        bbu_printf("Note:current Core OP is not OP0, but OP%d\r\n", user_cop[PLL_CORE]);
        if((ca7_value[user_cop[PLL_CORE]].pll_source) > PLL1)
        {
          pll_stat |= (1<<PLL_CORE);
          pll_cop = ca7_value[user_cop[PLL_CORE]].pll_op;
        }
    }
    if(user_cop[PLL_DDR] == OP_INVALID)
      {
        adfc_set(OP_UNCHANGE, OP0, OP_UNCHANGE);
        bbu_printf("the ddr changed from INVALID_OP to OP0\r\n");
        ddrop_check_dump(OP0);
      }
    if(user_cop[PLL_DDR] != OP0)
    {
        bbu_printf("Note:current DDR OP is not OP0, but OP%d\r\n", user_cop[PLL_DDR]);
        if((ddr_value[user_cop[PLL_DDR]].pll_source) > PLL1)
        {
          pll_stat |= (1<<PLL_DDR);
          pll_cop = ddr_value[user_cop[PLL_DDR]].pll_op;
        }
    }
    if(user_cop[PLL_AXI] == OP_INVALID)
      {
        adfc_set(OP_UNCHANGE, OP_UNCHANGE, OP0);
        bbu_printf("the axi changed from INVALID_OP to OP0\r\n");
        axiop_check_dump(OP0);
      }
    if(user_cop[PLL_AXI] != OP0)
    {
        bbu_printf("Note:current AXI OP is not OP0, but OP%d\r\n", user_cop[PLL_AXI]);
        if((aclk_value[user_cop[PLL_AXI]].pll_source) > PLL1)
        {
          pll_stat |= (1<<PLL_AXI);
          pll_cop = aclk_value[user_cop[PLL_AXI]].pll_op;
        }
    }

 /*Note: we direclty do the CS change to PP0 for simple*/
    PMUA->TRACE_CONFIG &= ~0x003E07C7;
    PMUA->TRACE_CONFIG |= cs_value[0].trace_config;
    PMUA->TRACE_CONFIG |= 0x00008020;
    bbu_printf("Coresight OP changed to OP0\r\n");
    user_cop[PLL_CORESIGHT] = OP0;

   /*Prepare the DVC settings before enable the DVC mechanism.*/
    if(!cpu_is_pxa1826_z1z2())
    {
        GET_CHIP_DVL4OP_Zx(cs_value, OP_CSMAX);
        if(cpu_is_pxa1826_z3())
        {
          GET_CHIP_DVL4OP_Zx(ca7_value, OP_COREMAX);
          GET_CHIP_DVL4OP_Zx(aclk_value, OP_AXIMAX);
          GET_CHIP_DVL4OP_Zx(ddr_value, OP_DDRMAX);
        }
        else //A0
        {
          GET_CHIP_DVL4OP_Ax(ca7_value, OP_COREMAX);
          GET_CHIP_DVL4OP_Ax(aclk_value, OP_AXIMAX);
          GET_CHIP_DVL4OP_Ax(ddr_value, OP_DDRMAX);
        }
    }
    user_dvc_vote[PLL_CORE] = ca7_value[user_cop[PLL_CORE]].dvc;
    user_dvc_vote[PLL_DDR] = ddr_value[user_cop[PLL_DDR]].dvc;
    user_dvc_vote[PLL_AXI] = aclk_value[user_cop[PLL_AXI]].dvc;
    user_dvc_vote[PLL_CORESIGHT] = cs_value[user_cop[PLL_CORESIGHT]].dvc;

//collect the votes, use coresight as initator, it's fine since we just chagned it to OP0 which is determined.
    max_dvc = dvc_vote_check(PLL_CORESIGHT, OP0);

#ifndef CONFIG_HWDVC
//for SW DVC and assume CP side boot OP need DVL1.
  if(max_dvc < 1) max_dvc = 1;
#endif

//nezha3 1.0 dkb
    if(ustica_get_version() >= 0x69)
    {
	#ifdef CONFIG_HWDVC     //HW-DVC
  #ifdef PMIC_DUMMY
	    MFPR->SM_nBE1 = 0x9040; //SM_nBE1,function 0,no pull
	    MFPR->SM_SCLK = 0x9043; //SM_SCLK,function 3,no pull
  #else
	    MFPR->SM_nBE1 = 0x9046; //SM_nBE1,function 6,no pull
	    MFPR->SM_SCLK = 0x9045; //SM_SCLK,function 5,no pull
  #endif
      if(cpu_is_pxa1826_a0()) {
        PMUM->DVC_AP = (PMUM->DVC_AP & (~(0xF << 8))) | BIT_15 | (max_dvc << 8);
      } else {
        PMUM->DVC_AP = (PMUM->DVC_AP & (~(0x7 << 4))) | BIT_7 | (max_dvc << 4);
      }
	#else    //GPIO-LEVEL based DVC, Function 0,
	    MFPR->SM_nBE1 = 0x9040; //SM_nBE1,function 0,no pull
	    MFPR->SM_SCLK = 0x9043; //SM_SCLK,function 3,no pull
	    gpio_output_set(DVC_level[1], (max_dvc & 0x2) >>1);  //DVC2 init value
	    gpio_output_set(DVC_level[0], max_dvc & 0x1);  //DVC1 init value
	    //In GPIO module, MOS transistor is used to set high/low ouput.
	    gpio_set_output(DVC_level[1]); //enable output
	    gpio_set_output(DVC_level[0]);
	#endif
    } else {//2.0x dkb use DTS_PAON/TDS_PACTRL as DVC1/DVC0
	#ifdef CONFIG_HWDVC     //HW-DVC
      #ifdef PMIC_DUMMY
      *(volatile uint32_t *)(0xd401e2E0) = 0x000090C1; //func1, DVC1, GPIO78
      *(volatile uint32_t *)(0xd401e2E4) = 0x000090C1; //func1, DVC0, GPIO79
      #else
      *(volatile uint32_t *)(0xd401e2E0) = 0x000090c2; //func2, DVC1, TDS_PAON
	    *(volatile uint32_t *)(0xd401e2E4) = 0x000090c2; //func2, DVC0, TDS_PACTRL
      #endif
      if(cpu_is_pxa1826_a0()) {
        PMUM->DVC_AP = (PMUM->DVC_AP & (~(0xF << 8))) | BIT_15 | (max_dvc << 8);
      } else {
        PMUM->DVC_AP = (PMUM->DVC_AP & (~(0x7 << 4))) | BIT_7 | (max_dvc << 4);
      }
	#else    //GPIO-LEVEL based DVC, Function 0,
	    *(volatile uint32_t *)(0xd401e2E0) = 0x000090C1; //func1, DVC1, GPIO78
	    *(volatile uint32_t *)(0xd401e2E4) = 0x000090C1; //func1, DVC0, GPIO79
	    DVC_level[1] = GPIO78;
	    DVC_level[0] = GPIO79;
	    gpio_output_set(DVC_level[1], (max_dvc & 0x2) >>1);  //DVC2 init value
	    gpio_output_set(DVC_level[0], max_dvc & 0x1);  //DVC1 init value
	    //In GPIO module, MOS transistor is used to set high/low ouput.
	    gpio_set_output(DVC_level[1]); //enable output
	    gpio_set_output(DVC_level[0]);
	#endif
    }

    pmic_dvc_init(); //set up DVC,etc. and enable DVC here (both SW/HW)
#ifdef CONFIG_HWDFC
    hw_dfc_init();
#endif
}


/*
1, In case SW DVC case,  CP can't adjust the voltage and AP is the owner
And even the DVL0 should be higher enough to cover all CP/DP PPs.
 That's why we need HW-DVC!!!!

2, Since we only have one buck for power supply all those users.
So those DVC update requests have to serialized or in sync via spin lock.

3, Each DVC update need some time (DVC counter),
we have to poll the status bit to make sure it's done
*/

void update_voltage_dvcl2h (pll_user initiator, op_index top)
{
    uint32_t  max_dvc_next = 0, initiator_T = 0, top_T = OP_INVALID;

    initiator_T = initiator;
    top_T = top;

    max_dvc_next = dvc_vote_check(initiator_T, top_T);

    if(max_dvc_next == max_dvc)
    {
        return;
    }
    else
    {
#ifndef CONFIG_MMT
        if (!verbose_off)bbu_printf("l2h:The max_dvc_next = %d\r\n ", max_dvc_next);
#endif
    }

    Q_ASSERT (max_dvc_next <= (uint32_t) DVC11);

    if (max_dvc_next < max_dvc)
    {
    #ifndef CONFIG_MMT
        bbu_printf("ERROR: wrong voltage direction\r\n");
    #endif
    }
    else
    {
#ifdef CONFIG_HWDVC     //HW-DVC
        //Note, need to write DVC_AP in one step!!!
        PMUM->DVC_IMR |= BIT_0;
        if(cpu_is_pxa1826_a0()) {
          PMUM->DVC_AP = (PMUM->DVC_AP & (~(0xF << 8))) | BIT_15 | (max_dvc_next << 8) ;
        } else {
          PMUM->DVC_AP = (PMUM->DVC_AP & (~(0x7 << 4))) | BIT_7 | (max_dvc_next << 4) ;
        }
        while ( !(BIT_0 & PMUM->DVC_ISR) ) ;
        PMUM->DVC_ISR = ~BIT_0; //write 0 clear, write 1 has no effect
    #ifndef CONFIG_MMT
        if (!verbose_off)bbu_printf("The DVC_STATUS = 0x%x\r\n", PMUM->DVC_STATUS);
    #endif
#else
        if ((max_dvc_next ^ max_dvc) == 0x3)
        {
            gpio_output_set(DVC_level[1], (max_dvc_next & 0x2) >> 1);
            gpio_output_set(DVC_level[0], max_dvc_next & 0x1);
        }
        else if ((max_dvc_next ^ max_dvc) == 0x2)
        {
            gpio_output_set(DVC_level[1], (max_dvc_next & 0x2) >> 1);
        }
        else
        {
            gpio_output_set(DVC_level[0], max_dvc_next & 0x1);
        }
        udelay(10);
#endif
        max_dvc =  max_dvc_next;
    }

}

void update_voltage_dvch2l (uint32_t initiator, uint32_t top)
{
    uint32_t  max_dvc_next = 0, initiator_T = 0, top_T = OP_INVALID;

    initiator_T = initiator;
    top_T = top;

   max_dvc_next = dvc_vote_check(initiator_T, top_T);

    if(max_dvc_next == max_dvc)
    {
        return;
    }
    else
    {
#ifndef CONFIG_MMT
      if (!verbose_off)bbu_printf("h2l:The max_dvc_next = %d\r\n ", max_dvc_next);
#endif
    }

    if(max_dvc_next > (uint32_t) DVC11)
    {
#ifndef CONFIG_MMT
            bbu_printf("ERROR:invalid dvc_next\r\n");
#endif
    }
    else if (max_dvc_next > max_dvc)
    {
#ifndef CONFIG_MMT
        bbu_printf("ERROR: wrong voltage direction\r\n");
#endif
    }
    else
    {
#ifdef CONFIG_HWDVC     //HW-DVC
        PMUM->DVC_IMR |= BIT_0;
        //Note, need to write DVC_AP in one step or value first and set the vote bit later!!!
        if(cpu_is_pxa1826_a0()) {
          PMUM->DVC_AP = (PMUM->DVC_AP & (~(0xF << 8))) | BIT_15 | (max_dvc_next << 8);
        } else {
          PMUM->DVC_AP = (PMUM->DVC_AP & (~(0x7 << 4))) | BIT_7 | (max_dvc_next << 4);
        }
        while ( !(BIT_0 & PMUM->DVC_ISR) ) ;
        PMUM->DVC_ISR = ~BIT_0; //write 0 clear, write 1 has no effect
#ifndef CONFIG_MMT
        if (!verbose_off)bbu_printf("The DVC_STATUS = 0x%x\r\n", PMUM->DVC_STATUS);
#endif
#else
        if ((max_dvc_next ^ max_dvc) == 0x3)
        {
            gpio_output_set(DVC_level[0], max_dvc_next & 0x1);
            gpio_output_set(DVC_level[1], ((max_dvc_next) & 0x2) >> 1);
        }
        else if ((max_dvc_next ^ max_dvc) == 0x2)
        {
            gpio_output_set(DVC_level[1], (max_dvc_next & 0x2) >> 1);
        }
        else
        {
            gpio_output_set(DVC_level[0], max_dvc_next & 0x1);
        }
        udelay(10);
        //With SW-DVC, we need to make sure CP can work at the lowest DVL
#endif
        max_dvc =  max_dvc_next;
    }
}

/*1,spinlock makes sure there is only one entity of this function is in executing at any time,
  2,better to be in critial section (not interrupt),
  3.return value is OP_INVALID or MV_OK
  4.It's the wrapper of adfc_set
  5.!!!!!We assume AXI always use PLL1 for simple!!!
  */
uint32_t adfc(op_index core_top, op_index ddr_top, op_index axi_top)
{
    op_index pll_top=OP0;
    op_index core_top_ex=OP_UNCHANGE, ddr_top_ex=OP_UNCHANGE, axi_top_ex=OP_UNCHANGE;
    op_index core_top_real=OP_UNCHANGE, ddr_top_real=OP_UNCHANGE;
    pll_src pll_num = PLL1;
    uint32_t  return_value = MV_FAIL;
    uint32_t flags = 0;

    if((core_top == user_cop[PLL_CORE]) &&
      (ddr_top == user_cop[PLL_DDR]) &&
      (axi_top == user_cop[PLL_AXI]))
    {
        bbu_printf("CA7/DDR/AXI already at OP%d, OP%d, OP%d!\r\n", core_top, ddr_top, axi_top);
        dump_cur_aop();
        dump_cur_ddrop();
        dump_cur_axiop();
        return MV_OK;
    }

    //Let core_top_ex has OP_UNCHANGE if any but core_top stands for the actual OP num
    core_top_ex = core_top;
    core_top_real = core_top;
    ddr_top_ex = ddr_top;
    ddr_top_real = ddr_top;
    axi_top_ex = axi_top;
    if (core_top == user_cop[PLL_CORE])core_top_ex = OP_UNCHANGE;
    if (ddr_top == user_cop[PLL_DDR]) ddr_top_ex = OP_UNCHANGE;
    if (axi_top == user_cop[PLL_AXI])axi_top_ex = OP_UNCHANGE;
    if (core_top == OP_UNCHANGE)core_top_real = user_cop[PLL_CORE];
    if (ddr_top == OP_UNCHANGE)ddr_top_real = user_cop[PLL_DDR];

    if(core_top_ex != OP_UNCHANGE)
      {
        if (!verbose_off)bbu_printf("CA7 OP changing from %d to %d \r\n", user_cop[PLL_CORE], core_top_ex);
      }
    if(ddr_top_ex != OP_UNCHANGE)
      {
        if(ddr_type & ddr_bvalue[ddr_top_ex].valid_mask)
        {
          if (!verbose_off)bbu_printf("DDR OP changing from %d to %d \r\n", user_cop[PLL_DDR], ddr_top_ex);
        }
        else
        {
          bbu_printf("Warning: DDR OP%d is unsupported for current DDR cat.\r\n", ddr_top_ex);
          ddr_top_ex = OP_UNCHANGE;
        }
      }
    if(axi_top_ex != OP_UNCHANGE)
    {
      if (!verbose_off)bbu_printf("AXI OP changing from %d to %d \r\n", user_cop[PLL_AXI], axi_top_ex);
    }

    //Now get the exclusive access among all AP cores and this core will not repsonse to interrupts.
    flags = disable_irq_fiq();

    //+++Step1: increase the voltages (and XTC for core) if it's raise PP
    if(core_top_ex != OP_UNCHANGE)
    {
      if(ca7_value[core_top_ex].dvc > ca7_value[user_cop[PLL_CORE]].dvc)update_voltage_dvcl2h (PLL_CORE, core_top_ex);
    //+++Step1-1: change the L1/L2 XTC values
      if(ca7_value[core_top_ex].pclk > ca7_value[user_cop[PLL_CORE]].pclk)update_memory_xtc(ca7_value[core_top_ex].pclk);
    }
    if(ddr_top_ex != OP_UNCHANGE)
    {
       if(ddr_value[ddr_top_ex].dvc > ddr_value[user_cop[PLL_DDR]].dvc)update_voltage_dvcl2h (PLL_DDR, ddr_top_ex);
    }
    if(axi_top_ex != OP_UNCHANGE)
    {
       if(aclk_value[axi_top_ex].dvc > aclk_value[user_cop[PLL_AXI]].dvc)update_voltage_dvcl2h (PLL_AXI, axi_top_ex);
       Q_ASSERT(aclk_value[axi_top_ex].pll_source == PLL1); //AXI use PLL1 only
    }

    //+++Step2: setup PLL  and use intermediate PP to avoid MFC if necessary
    /*
    Note:PMUA_AP_IMR[3]/PMUA_AP_ISR[3] is only valid for fc_sm FC, not valid for quick dynamic FC.
    We don't need it at this case because there are no hankshake between core and PMU,
    core will go on after finish and no need to wait here.
    */

    //if core, ddr, axi use same pll other than PLL1, then must use same PLL's OP.
    if ((core_top_ex != OP_UNCHANGE) || (ddr_top_ex != OP_UNCHANGE))
    {
        switch ((1<<ca7_value[core_top_real].pll_source)&(1<<ddr_value[ddr_top_real].pll_source))
        {
          case 1<<PLL1:
              pll_stat  &= ~((1<<PLL_CORE) | (1<<PLL_DDR));
              break;
          case 1<<PLL2:
              if (ca7_value[core_top_real].pll_op != ddr_value[ddr_top_real].pll_op)
              {
#ifndef CONFIG_MMT
                bbu_printf("Warning: same PLL%d can't change to different OP\r\n", ca7_value[core_top_real].pll_source);
#endif
                restore_irq_fiq(flags);
                return MV_ERROR;
              }
              pll_num = ca7_value[core_top_real].pll_source;
              pll_top = ca7_value[core_top_real].pll_op;
              if(MV_OK != setup_pll(pll_num, pll_top))
                {
                  restore_irq_fiq(flags);
                  return MV_ERROR;
                }
              pll_stat |= (1<<PLL_CORE) | (1<<PLL_DDR);
              break;
          case 0: //source is different
              if(ca7_value[core_top_real].pll_source != PLL1)
              {
                pll_num = ca7_value[core_top_real].pll_source;
                pll_top = ca7_value[core_top_real].pll_op;
                if(MV_OK != setup_pll(pll_num, pll_top))
                {
                  restore_irq_fiq(flags);
                  return MV_ERROR;
                }
                pll_stat |= (1<<PLL_CORE);
              }
              else
              {
                pll_stat &= ~(1<<PLL_CORE); //PLL2
              }
              if(ddr_value[ddr_top_real].pll_source != PLL1)
              {
                pll_num = ddr_value[ddr_top_real].pll_source;
                pll_top = ddr_value[ddr_top_real].pll_op;
                if(MV_OK != setup_pll(pll_num, pll_top))
                {
                  restore_irq_fiq(flags);
                  return MV_ERROR;
                }
              pll_stat |= (1<<PLL_DDR);
              }
              else
              {
                pll_stat &= ~(1<<PLL_DDR); //PLL2
              }
              break;
        }
    }

    //+++Step3~11 the actual DFC process.
    //bbu_printf("core_top = %d, ddr_top = %d, axi_top = %d\r\n", core_top_ex, ddr_top_ex, axi_top_ex);
#ifdef CONFIG_HWDFC
    //When HW_DFC is enabled, the DDR freq is already changed in the start with HW-DFC
    hw_dfc_set(ddr_top_ex);
    adfc_set(core_top_ex, OP_UNCHANGE, axi_top_ex);
#else
    adfc_set(core_top_ex, ddr_top_ex, axi_top_ex);
#endif

#ifndef CONFIG_DBG
    //FIXME: when use the console AO to ouput, it will need this delay for any B2B freq change stress
    //It seems the UART interrupt is lost and hang (disable_irq_fiq/restore_irq_fiq bouncing causes UART or CPU hang
    //because we rely on the UART FIFO timeout interrupt to output log in console AO.)
     if (!verbose_off)udelay(1000);
#endif

    //+++Step12: change the L1/L2 XTC values
    //+++Step13: decrease the voltage
    if (core_top_ex != OP_UNCHANGE)
    {
        if (((ca7_value[user_cop[PLL_CORE]].pll_source) != ca7_value[core_top_ex].pll_source)
        && ((ca7_value[user_cop[PLL_CORE]].pll_source) != PLL1))
        {
          //  pll_num = ca7_value[user_cop[PLL_CORE]].pll_source;
            pll_stat  &= ~(1 << PLL_CORE);
        }
        if (ca7_value[core_top_ex].pclk < ca7_value[user_cop[PLL_CORE]].pclk)update_memory_xtc(ca7_value[core_top_ex].pclk);
        if (ca7_value[core_top_ex].dvc < ca7_value[user_cop[PLL_CORE]].dvc)update_voltage_dvch2l (PLL_CORE, core_top_ex);
    }
    if (ddr_top_ex != OP_UNCHANGE)
    {
        if (((ddr_value[user_cop[PLL_DDR]].pll_source) != ddr_value[ddr_top_ex].pll_source)
                && ((ddr_value[user_cop[PLL_DDR]].pll_source) != PLL1))
        {
          //  pll_num = ddr_value[user_cop[PLL_DDR]].pll_source;
            pll_stat  &= ~(1 << PLL_DDR);
        }
        if (ddr_value[ddr_top_ex].dvc < ddr_value[user_cop[PLL_DDR]].dvc)update_voltage_dvch2l (PLL_DDR, ddr_top_ex);
    }
    if (axi_top_ex != OP_UNCHANGE)
    {
        if (aclk_value[axi_top_ex].dvc < aclk_value[user_cop[PLL_AXI]].dvc)update_voltage_dvch2l (PLL_AXI, axi_top_ex);
    }

    //+++Step14: Disable PLL2 if not in use by SW control method.
    if((pll_stat == 0)&&(pll_cop!=0)) setup_pll(PLL2, OP0);

    //+++Step15: Get current OP and compare with Target OP, if not equal, then fails.
    //compare and cpp update inside
    return_value = MV_OK;

    if(core_top_ex != OP_UNCHANGE)
    {
#ifdef CONFIG_PPSET_CHECK
        if (core_top_ex != aop_check_dump(core_top_ex))
        {
            bbu_printf("Core freq error\r\n");
            return_value = MV_FAIL;
        }
#endif
        user_cop[PLL_CORE]  = core_top_ex;
    }
    if (ddr_top_ex != OP_UNCHANGE)
    {
#ifdef CONFIG_PPSET_CHECK
#ifdef CONFIG_HWDFC
        Q_ASSERT((ddrop_check_dump(ddr_top_ex)) == ((PMUA->DFC_STATUS & 0xE) >> 1));
#endif
        ddrop_check_dump(ddr_top_ex);
        if (ddr_top_ex != user_cop[PLL_DDR])
        {
            bbu_printf("DDR freq error\r\n");
            return_value = MV_FAIL;
        }
#endif
        user_cop[PLL_DDR]  = ddr_top_ex;
    }
    if (axi_top_ex != OP_UNCHANGE)
    {
#ifdef CONFIG_PPSET_CHECK
        if (axi_top_ex != axiop_check_dump(axi_top_ex))
        {
            bbu_printf("AXI freq error \r\n");
            return_value = MV_FAIL;
        }
#endif
        user_cop[PLL_AXI]  = axi_top_ex;
    }

    //+++Step16, release the spinlock and this core can repsonse to interrupts.
    restore_irq_fiq(flags);

    return return_value;
}

/*Must make sure there is only one entity of this function is in executing at any time,
  better to be in critial section, return value is OP_INVALID or MV_OK*/
void csdfc(uint32_t top)
{
/*Coresight freq change use its own state-machine, not shared with the Main fsm*/
    bbu_printf("changing Coresight freq to OP%d\r\n", top);
    PMUA->TRACE_CONFIG &= ~0x003E07C7;
    PMUA->TRACE_CONFIG |= cs_value[top].trace_config;
    if(cs_value[top].dvc > cs_value[user_cop[PLL_CORESIGHT]].dvc)update_voltage_dvcl2h(PLL_CORESIGHT, top);
    PMUA->TRACE_CONFIG |= 0x00008020;
    if(cs_value[top].dvc < cs_value[user_cop[PLL_CORESIGHT]].dvc)update_voltage_dvch2l(PLL_CORESIGHT, top);
    user_cop[PLL_CORESIGHT] = top;
}

/*following code generates a pseudorandom number between 0 and n,
  Need to write a function using inline asm.

//r is the current random seed
//a is the multiplier (eg 0x91E6D6A5)
//n is the random number range (0...n-1)
//t is a scratch register
MLA r, a, r, a ; iterate random number generator, r=a*r+a
UMULL t, s, r, n ; s = (r*n)/2^32;  r*n low 32bit->t, high 32bit->s
//r is the new random seed
//s is the random result in range 0 ... n-1

*/

uint32_t random(uint32_t n)
{
    static uint32_t seed=0x1;
    uint32_t a = 0x91E6D6A5;
    uint32_t s = 0;

    __asm volatile("mla %[seed], %[multiplier], %[seed], %[multiplier]"
                        :[seed] "+r" (seed)
                        :[multiplier] "r" (a)
                        :"cc","memory");
    __asm volatile("umull r3, %[result], %[seed], %[range]"
                        :[result] "=r" (s)
                        :[seed] "r" (seed), [range]"r"(n)
                        :"cc","r3","memory");
    return s;
}

/*PP change test function
  test_mode:  if it's < PP_NUM, then it's just one PP change test, test_mode is the target PP number.
  if test_mode > PP_NUM, we have some special test modes, test_count limits the PP change count number*/

//for PP test of core/ddr/axi together
static uint32_t pp_test(uint32_t argc, uint8_t *argv[])
{
    op_index core_mode, ddr_mode, axi_mode;

    (void)argc;

    if((strcmp((const char*)argv[1], "/c") == 0) ||
       (strcmp((const char*)argv[1], "--check") == 0))
    {
         dump_cur_aop();
         dump_cur_ddrop();
         dump_cur_axiop();
#ifdef CONFIG_HWDVC
         bbu_printf("HW_DVC_STATUS = 0x%x\r\n", PMUM->DVC_STATUS);
#endif
#ifdef CONFIG_HWDFC
         bbu_printf("HW_DFC_STATUS = 0x%x\r\n", PMUA->DFC_STATUS);
#endif
         return MV_OK;
    }

    core_mode = XpStrToValue((char*)argv[1]);
    ddr_mode = XpStrToValue((char*)argv[2]);
    axi_mode = XpStrToValue((char*)argv[3]);

    if((core_mode <= OP_COREMAX)&&
       (ddr_mode <= OP_DDRMAX)&&
       (axi_mode <= OP_AXIMAX))
    {
        return adfc(core_mode, ddr_mode, axi_mode);
    }
    return MV_ERROR;
}

//for OP test
static uint32_t op_test(uint32_t argc, uint8_t *argv[])
{
    uint32_t i = 0, pp = 0;
    op_index max_op;
    uint32_t test_mode;
    uint32_t test_count, pp1, pp2;

    (void)argc;

    if(strcmp((const char*)argv[1], "core") == 0)
    {
        if(strcmp((const char*)argv[2], "/c") == 0)
        {
          dump_cur_aop();
          return MV_OK;
        }
        max_op =  OP_COREMAX;
        test_mode = XpStrToValue((char*)argv[2]);
        if(test_mode <= max_op)
        {
            verbose_off = 0;
            return adfc((op_index)test_mode, OP_UNCHANGE, OP_UNCHANGE);
        }
        //otherwise, it's for specific test
        verbose_off = 1;
        test_count = XpStrToValue((char*)argv[3]);
        switch (test_mode) {

        case 100:  //random stress test
            for(i=0; i<test_count; i++)
            {
                pp = random(max_op);
                adfc(pp, OP_UNCHANGE, OP_UNCHANGE);
            }
            break;

        case 200:  //sequence stress test
            for(i=0; i<test_count; i++)
            {
                uint32_t j=0;
                for (j=0;j<=max_op;j++) {
                       adfc(max_op-1-j, OP_UNCHANGE, OP_UNCHANGE);
                    }
               for (j=0;j<max_op;j++) {
                       adfc(j, OP_UNCHANGE, OP_UNCHANGE);
                    }
            }
            break;

        case 300:  // back to back stress test between two PPs.
            pp1 = XpStrToValue((char*)argv[4]);
            pp2 = XpStrToValue((char*)argv[5]);
            for(i=0; i<test_count; i++) {
                 adfc(pp1, OP_UNCHANGE, OP_UNCHANGE);
                 adfc(pp2, OP_UNCHANGE, OP_UNCHANGE);
            }
            break;
        case 400:  //random stress test
            while(1)
            {
                pp = random(max_op);
                adfc(pp, OP_UNCHANGE, OP_UNCHANGE);
            }
        default :
            bbu_printf("\r\nunsupported CA7 OP test! \r\n");
            return MV_ERROR;
            break;
        }
    }

    if(strcmp((const char*)argv[1], "ddr") == 0)
    {
        if(strcmp((const char*)argv[2], "/c") == 0)
        {
          dump_cur_ddrop();
          return MV_OK;
        }
        max_op =  OP_DDRMAX;
        test_mode = XpStrToValue((char*)argv[2]);
        if(test_mode <= max_op)
        {
            verbose_off = 0;
            return adfc(OP_UNCHANGE, test_mode, OP_UNCHANGE);
        }
        //otherwise, it's for specific test
        verbose_off = 1;
        test_count = XpStrToValue((char*)argv[3]);
        switch (test_mode) {

        case 100:  //random stress test
            for(i=0; i<test_count; i++)
            {
                pp = random(max_op);
                adfc(OP_UNCHANGE, pp, OP_UNCHANGE);
            }
            break;

        case 200:  //sequence stress test
            for(i=0; i<test_count; i++)
            {
                uint32_t j=0;
                for (j=0;j<=max_op;j++) {
                       adfc(OP_UNCHANGE, max_op-1-j, OP_UNCHANGE);
                    }
               for (j=0;j<max_op;j++) {
                       adfc(OP_UNCHANGE, j, OP_UNCHANGE);
                    }
            }
            break;

        case 300:  // back to back stress test between two PPs.
            pp1 = XpStrToValue((char*)argv[4]);
            pp2 = XpStrToValue((char*)argv[5]);
            for(i=0; i<test_count; i++) {
                 adfc(OP_UNCHANGE, pp1, OP_UNCHANGE);
                 adfc(OP_UNCHANGE, pp2, OP_UNCHANGE);
                 //adfc(OP_UNCHANGE, OP7, OP_UNCHANGE);
            }
            break;
        case 400:  //random stress test
            while(1)
            {
                pp = random(max_op);
                adfc(OP_UNCHANGE, pp, OP_UNCHANGE);
            }
            break;
        default :
            bbu_printf("\r\nunsupported DDR  OP test! \r\n");
            return MV_ERROR;
            break;
        }
    }

    if(strcmp((const char*)argv[1], "axi") == 0)
    {
        if(strcmp((const char*)argv[2], "/c") == 0)
        {
          dump_cur_axiop();
          return MV_OK;
        }
        max_op = OP_AXIMAX;
        test_mode = XpStrToValue((char*)argv[2]);
        if(test_mode <= max_op)
        {
            verbose_off = 0;
            return adfc(OP_UNCHANGE, OP_UNCHANGE, test_mode);
        }
        //otherwise, it's for specific test
        verbose_off = 1;
        test_count = XpStrToValue((char*)argv[3]);
        switch (test_mode) {

        case 100:  //random stress test
            for(i=0; i<test_count; i++)
            {
                pp = random(max_op);
                adfc(OP_UNCHANGE, OP_UNCHANGE, pp);
            }
            break;

        case 200:  //sequence stress test
            for(i=0; i<test_count; i++)
            {
                uint32_t j=0;
                for (j=0;j<=max_op;j++) {
                       adfc(OP_UNCHANGE, OP_UNCHANGE, max_op-1-j);
                    }
               for (j=0;j<max_op;j++) {
                       adfc(OP_UNCHANGE, OP_UNCHANGE, j);
                    }
            }
            break;

        case 300:  // back to back stress test between two PPs.
            pp1 = XpStrToValue((char*)argv[4]);
            pp2 = XpStrToValue((char*)argv[5]);
            for(i=0; i<test_count; i++) {
                 adfc(OP_UNCHANGE, OP_UNCHANGE, pp1);
                 adfc(OP_UNCHANGE, OP_UNCHANGE, pp2);
            }
            break;
        case 400:  //random stress test
            while(1)
            {
                pp = random(max_op);
                adfc(OP_UNCHANGE, OP_UNCHANGE, pp);
            }
            break;
        default :
            bbu_printf("\r\nunsupported AXI OP test! \r\n");
            return MV_ERROR;
            break;
        }
    }

    if(strcmp((const char*)argv[1], "pll") == 0)
    {
        if(strcmp((const char*)argv[2], "/c") == 0)
        {
          bbu_printf("pll_cop = 0x%x\r\n", pll_cop);
          bbu_printf("pll_stat = 0x%x\r\n", pll_stat);
          dump_pll_list();
          return MV_OK;
        }
        max_op = OP_PLLMAX;
        uint32_t pll_num = 0; //only can 2
        pll_num = XpStrToValue((char*)argv[2]);
        test_mode = XpStrToValue((char*)argv[3]);
        if(test_mode <= max_op)
        {
            return setup_pll(pll_num, test_mode);
        }
   }

   return MV_OK;
}

uint32_t csop_test(uint32_t argc, uint8_t *argv[])
{
    uint32_t test_mode;

    (void)argc;

    if((strcmp((const char*)argv[2], "/c") == 0) ||
       (strcmp((const char*)argv[2], "--check") == 0)){
        bbu_printf("The current Coresight at OP%d, atclk = %d, dbg_clk= %d, trace_clk = %d\r\n",
                    user_cop[PLL_CORESIGHT], cs_value[user_cop[PLL_CORESIGHT]].atclk, cs_value[user_cop[PLL_CORESIGHT]].dbg_clk, cs_value[user_cop[PLL_CORESIGHT]].trace_clk);
         return MV_OK;
    }

    test_mode = XpStrToValue((char*)argv[2]);

    if(test_mode <= OP_CSMAX)
    {
        csdfc(test_mode);
        return MV_OK;
    }
    else
    {
        bbu_printf("unsupported coresight OP\r\n");
        return MV_ERROR;
    }

}

static void BBU_REG_DUMP(char *sptr, uint32_t *reg_addr, uint32_t hexvalue)
{
    bbu_printf("%s(%x) = %x \r\n", sptr, reg_addr, hexvalue);
}

static void dump_app_fcreg()
{

    BBU_REG_DUMP("DM_CC_CP           ",  (uint32_t*)&PMUA->DM_CC_CP , PMUA->DM_CC_CP);

    PMUA->CC_CP |= BIT_31;
    PMUA->CC_CP &= ~BIT_31;

    BBU_REG_DUMP("DM_CC_AP           ",  (uint32_t*)&PMUA->DM_CC_AP , PMUA->DM_CC_AP);

    PMUA->CC_AP |= BIT_31;
    PMUA->CC_AP &= ~BIT_31;

    return;
}

void dump_app_pmureg()
{
    bbu_printf("\r\n*************Dragon : APP PMU Register Dump Begin ************\r\n");
    BBU_REG_DUMP("CC_CP              ", (uint32_t*)&PMUA->CC_CP              , PMUA->CC_CP              );
    BBU_REG_DUMP("CC_AP              ", (uint32_t*)&PMUA->CC_AP              , PMUA->CC_AP              );
    //BBU_REG_DUMP("DM_CC_CP           ",(uint32_t*)PMUA->DM_CC_CP           , PMUA->DM_CC_CP           );
    //BBU_REG_DUMP("DM_CC_AP           ",(uint32_t*)PMUA->DM_CC_AP           , PMUA->DM_CC_AP           );
    dump_app_fcreg();

    BBU_REG_DUMP("MC_HW_SLP_TYPE     ", (uint32_t*)&PMUA->MC_HW_SLP_TYPE     , PMUA->MC_HW_SLP_TYPE     );
    BBU_REG_DUMP("PLL_SEL_STATUS     ", (uint32_t*)&PMUA->PLL_SEL_STATUS     , PMUA->PLL_SEL_STATUS     );
    BBU_REG_DUMP("TRACE_CONFIG       ", (uint32_t*)&PMUA->TRACE_CONFIG       , PMUA->TRACE_CONFIG       );
    BBU_REG_DUMP("CA7MP_IDLE_CFG0    ", (uint32_t*)&PMUA->CA7MP_IDLE_CFG0    , PMUA->CA7MP_IDLE_CFG0    );
    BBU_REG_DUMP("CA7_CORE0_IDLE_CFG ", (uint32_t*)&PMUA->CA7_CORE0_IDLE_CFG , PMUA->CA7_CORE0_IDLE_CFG );
    BBU_REG_DUMP("DFC_AP             ", (uint32_t*)&PMUA->DFC_AP       , PMUA->DFC_AP       );
    BBU_REG_DUMP("DFC_STATUS         ", (uint32_t*)&PMUA->DFC_STATUS       , PMUA->DFC_STATUS       );
    BBU_REG_DUMP("DFC_LEVEL0         ", (uint32_t*)&PMUA->DFC_LEVEL[0]       , PMUA->DFC_LEVEL[0]       );
    BBU_REG_DUMP("DFC_LEVEL1         ", (uint32_t*)&PMUA->DFC_LEVEL[1]       , PMUA->DFC_LEVEL[1]       );
    BBU_REG_DUMP("DFC_LEVEL2         ", (uint32_t*)&PMUA->DFC_LEVEL[2]       , PMUA->DFC_LEVEL[2]       );
    BBU_REG_DUMP("DFC_LEVEL3         ", (uint32_t*)&PMUA->DFC_LEVEL[3]       , PMUA->DFC_LEVEL[3]       );
    BBU_REG_DUMP("DFC_LEVEL4         ", (uint32_t*)&PMUA->DFC_LEVEL[4]       , PMUA->DFC_LEVEL[4]       );
    BBU_REG_DUMP("DFC_LEVEL5         ", (uint32_t*)&PMUA->DFC_LEVEL[5]       , PMUA->DFC_LEVEL[5]       );
    BBU_REG_DUMP("DFC_LEVEL6         ", (uint32_t*)&PMUA->DFC_LEVEL[6]       , PMUA->DFC_LEVEL[6]       );
    BBU_REG_DUMP("DFC_LEVEL7         ", (uint32_t*)&PMUA->DFC_LEVEL[7]       , PMUA->DFC_LEVEL[7]       );
    bbu_printf("*************Dragon : APP PMU Register Dump End ************\r\n");
    return;
}



void dump_main_pmureg()
{
    bbu_printf("*************Dragon : MAIN PMU Register Dump Begin ************\r\n");
    BBU_REG_DUMP("CPSR     ", (uint32_t*)&PMUM->CPSR     , PMUM->CPSR     );
    BBU_REG_DUMP("FCCR     ", (uint32_t*)&PMUM->FCCR     , PMUM->FCCR     );
    BBU_REG_DUMP("POCR     ", (uint32_t*)&PMUM->POCR     , PMUM->POCR     );
    BBU_REG_DUMP("POSR     ", (uint32_t*)&PMUM->POSR     , PMUM->POSR     );
    BBU_REG_DUMP("SUCCR    ", (uint32_t*)&PMUM->SUCCR    , PMUM->SUCCR    );
    BBU_REG_DUMP("VRCR     ", (uint32_t*)&PMUM->VRCR     , PMUM->VRCR     );
    BBU_REG_DUMP("PLL2CR   ", (uint32_t*)&PMUM->PLL2CR   , PMUM->PLL2CR   );
    BBU_REG_DUMP("SCCR     ", (uint32_t*)&PMUM->SCCR     , PMUM->SCCR     );
    BBU_REG_DUMP("ISCCRX[0]", (uint32_t*)&PMUM->ISCCRX[0], PMUM->ISCCRX[0]);
    BBU_REG_DUMP("ISCCRX[1]", (uint32_t*)&PMUM->ISCCRX[1], PMUM->ISCCRX[1]);
    BBU_REG_DUMP("DSOC     ", (uint32_t*)&PMUM->DSOC     , PMUM->DSOC     );
    BBU_REG_DUMP("WDTPCR   ", (uint32_t*)&PMUM->WDTPCR   , PMUM->WDTPCR   );
    BBU_REG_DUMP("APCR     ", (uint32_t*)&PMUM->APCR     , PMUM->APCR     );
    BBU_REG_DUMP("APSR     ", (uint32_t*)&PMUM->APSR     , PMUM->APSR     );
    BBU_REG_DUMP("APRR     ", (uint32_t*)&PMUM->APRR     , PMUM->APRR     );
    BBU_REG_DUMP("ACGR     ", (uint32_t*)&PMUM->ACGR     , PMUM->ACGR     );
    BBU_REG_DUMP("ARSR     ", (uint32_t*)&PMUM->ARSR     , PMUM->ARSR     );
    BBU_REG_DUMP("AWUCRS   ", (uint32_t*)&PMUM->AWUCRS   , PMUM->AWUCRS   );
    BBU_REG_DUMP("AWUCRM   ", (uint32_t*)&PMUM->AWUCRM   , PMUM->AWUCRM   );
    BBU_REG_DUMP("DVCR     ", (uint32_t*)&PMUM->DVCR , PMUM->DVCR );
    if(cpu_is_pxa1826_a0()) {
    BBU_REG_DUMP("VL01STR  ", (uint32_t*)&PMUM->VL01STR     , PMUM->VL01STR     );
    BBU_REG_DUMP("VL12STR  ", (uint32_t*)&PMUM->VL12STR     , PMUM->VL12STR     );
    BBU_REG_DUMP("VL23STR  ", (uint32_t*)&PMUM->VL23STR     , PMUM->VL23STR     );
    BBU_REG_DUMP("VL34STR  ", (uint32_t*)&PMUM->VL34STR     , PMUM->VL34STR     );
    BBU_REG_DUMP("VL45STR  ", (uint32_t*)&PMUM->VL45STR     , PMUM->VL45STR     );
    } else {
    BBU_REG_DUMP("VL01STR  ", (uint32_t*)&PMUM->VL01STR_Zx  , PMUM->VL01STR_Zx  );
    BBU_REG_DUMP("VL12STR  ", (uint32_t*)&PMUM->VL12STR_Zx  , PMUM->VL12STR_Zx  );
    BBU_REG_DUMP("VL23STR  ", (uint32_t*)&PMUM->VL23STR_Zx  , PMUM->VL23STR_Zx  );
    BBU_REG_DUMP("VL34STR  ", (uint32_t*)&PMUM->VL34STR_Zx  , PMUM->VL34STR_Zx  );
    BBU_REG_DUMP("VL45STR  ", (uint32_t*)&PMUM->VL45STR_Zx  , PMUM->VL45STR_Zx  );
    }
    BBU_REG_DUMP("DVC_AP   ", (uint32_t*)&PMUM->DVC_AP   , PMUM->DVC_AP   );
    BBU_REG_DUMP("DVC_APSUB", (uint32_t*)&PMUM->DVC_APSUB, PMUM->DVC_APSUB );
    BBU_REG_DUMP("DVC_CHIP ", (uint32_t*)&PMUM->DVC_CHIP     , PMUM->DVC_CHIP     );
    BBU_REG_DUMP("DVC_STAUTS", (uint32_t*)&PMUM->DVC_STATUS     , PMUM->DVC_STATUS     );
    BBU_REG_DUMP("DVC_IMR  ", (uint32_t*)&PMUM->DVC_IMR     , PMUM->DVC_IMR    );
    BBU_REG_DUMP("DVC_ISR  ", (uint32_t*)&PMUM->DVC_ISR     , PMUM->DVC_ISR     );
    BBU_REG_DUMP("DVC_DEBUG", (uint32_t*)&PMUM->DVC_DEBUG     , PMUM->DVC_DEBUG     );
    BBU_REG_DUMP("DVC_EXTRA_STR", (uint32_t*)&PMUM->DVC_EXTRA_STR   , PMUM->DVC_EXTRA_STR   );

    bbu_printf("*************Dragon : MAIN PMU Register Dump End ************\r\n");
    return;
}

void dump_pll_ctrlreg()
{
    bbu_printf("*************Dragon : APB PLL Control Register Dump Begin ************\r\n");
    BBU_REG_DUMP("PLL2_SW_CTRL     ", (uint32_t*)&APBSPARE->PLL2_SW_CTRL, APBSPARE->PLL2_SW_CTRL);
    BBU_REG_DUMP("PLL2CR   ", (uint32_t*)&PMUM->PLL2CR   , PMUM->PLL2CR   );
    BBU_REG_DUMP("FCCR     ", (uint32_t*)&PMUM->FCCR     , PMUM->FCCR     );
    BBU_REG_DUMP("CC_AP              ", (uint32_t*)&PMUA->CC_AP              , PMUA->CC_AP              );
    bbu_printf("*************Dragon : APB PLL Control Register Dump End ************\r\n");
    return;
}

void dump_pmu_reg(void)
{
    dump_main_pmureg();
    dump_app_pmureg();
    dump_pll_ctrlreg();
}

/*
ppset <OP_device>  <OP_index>
OP_device: core/axi/cs
OP_index: 0..n, "/c", "help", "/h"
*/

int bbu_ppset_test(int argc, uint8_t **argv)
{
    bbu_printf("\n\r");
    switch(argc){
        case 1:{
            bbu_printf("ppset--> No PP specified.\n\r");
            bbu_ppset_help();
            break;
            }
        default:{
            if(check_help(argv[1])){
                bbu_ppset_help();
                dump_aop_list();
                dump_ddrop_list();
                dump_axiop_list();
            }
            else if(check_reg(argv[1])){
                dump_pmu_reg();
            }
            else{

                if(MV_OK == pp_test(argc, argv))              //if pp_test success, send the ack to screen test hanlder via AO_cli
                {
                       return 1;
                      //screen_cmd_ack(1, scr_cmd_index);
                }
            }
          break;
      }
  }
    return 0;
}


int bbu_opset_test(int argc, uint8_t **argv)
{
    bbu_printf("\n\r");
    switch(argc){
        case 1:{
            bbu_printf("opset--> No OP specified.\n\r");
            //bbu_opset_help();
            break;
            }
        default:{
            if(check_help(argv[1])){
                //bbu_opset_help();
                dump_aop_list();
                dump_ddrop_list();
                dump_axiop_list();
            }
            else{
                    if((strcmp((const char*)argv[1], "cs") == 0))
                    {
                        if(MV_OK == csop_test(argc, argv))
                    //if pp_test success, send the ack to screen test hanlder via AO_cli
                        {
                            return 1;
                            //screen_cmd_ack(1, scr_cmd_index);
                        }
                    }

                    if(MV_OK == op_test(argc, argv))              //if pp_test success, send the ack to screen test hanlder via AO_cli
                    {
                        return 1;
                        //screen_cmd_ack(1, scr_cmd_index);
                    }
            }
            break;
        }
    }
    return 0;
}
