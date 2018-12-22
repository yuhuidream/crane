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
#include "common_tests.h"
#include "GEU.h"
#include "CIU.h"
#include "LCD.h"
#include "MFPR.h"
#include "PMU_DVC.h"

/*!!!!!!!!!!!!!!!!!!!!!Note: AP and CP has different copy of the below variables!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Which means AP only has real time value of c_aop,  need to update/get the c_cop, c_axiop value when CP not changing them.
and vice-versa. */
uint32_t aop_num = 0, cop_num = 0, axiop_num = 0, csop_num = 0;
#ifdef CONFIG_CA7
op_index c_aop=OP_INVALID,  c_csop = OP_INVALID;
#endif
op_index c_cop = OP_INVALID, c_axiop = OP_INVALID;

uint32_t max_dvc = 0;  //The current max requested DVC level at AP or CP side.
uint32_t  DVC_level[2];

#ifdef CONFIG_CA7
core_freq ca7_value[] = {
   /*label pclk,memclk,busclk,pll2,pll2p,dvc*/
    {"OP0", 416, 208,   104,   0,   0,  0}, //PP0.0
    {"OP1", 208, 104,   104,   0,   0,  0}, //PP1.1
    {"OP2", 312, 156,   156,   0,   0,  0}, //PP2.1
    {"OP3", 416, 208,   208,   0,   0,  0}, //PP3.2
    {"OP4", 624, 312,   156,   0,   0,  1}, //PP4.2
    {"OP5", 797, 398,   199,   797, 0,  2},
    {"OP6", 832, 416,   208,   0,   0,  2}, //PP6.0, PP6.1
    {"OP7", 1057,528,   264,   1057,0,  3},
    {"OP8", 1248,624,   312,   0,  0,  3}, //PP7.0
};

//the ASYNC bits has no use in AP side
static  dfc_setting ca7_reg[] = {
 /*pll2ref,pll2fb,apb_spare2, fccr[31:29],  0x09FC0000|cc_ap[8:0]  mc_reg_table*/
   {NA ,    NA ,  NA        ,  0x0<<29,           0xC9,               NA},   //OP0
   {NA ,    NA ,  NA        ,  0x0<<29,           0x4B,               NA},   //OP1
   {NA ,    NA ,  NA        ,  0x1<<29,           0x4B,               NA},   //OP2
   {NA ,    NA ,  NA        ,  0x0<<29,           0x49,               NA},   //OP3
   {NA ,    NA ,  NA        ,  0x1<<29,           0xC9,               NA},   //OP4
   {3  ,    46,   0x1012D0E3,  0x2<<29,           0xC8,               NA},   //OP5
   {NA ,    NA ,  NA        ,  0x0<<29,           0xC8,               NA},   //OP6
   {3  ,    61,   0x1012E0E3,  0x2<<29,           0xC8,               NA},   //OP7   
   {NA ,    NA ,  NA        ,  0x1<<29,           0xC8,               NA},   //OP8
};
#endif

core_freq cr5_value[] = {
   /*label pclk, memclk/dclk_1x,busclk,pll2,pll2p,dvc*/
    {"OP0", 416,      208,        104,   0,   0,  0}, //PP0.0
    {"OP1", 208,      104,        104,   0,   0,  0}, //PP1.1
    {"OP2", 312,      156,        156,   0,   0,  0}, //PP2.1
    {"OP3", 416,      208,        208,   0,   0,  0}, //PP3.2
    {"OP4", 624,      312,        156,   0,   0,  1}, //PP4.2
    {"OP5", 797,      398,        199,   797, 0,  2},//PP6.0, PP6.1, PP7.0
    {"OP6", 832,      416,        208,   0,   0,  2}, 
    {"OP7", 1057,     528,        264,   1057,0,  3},
};

/*
Note: since AP wants to know the DDR OP and it's controlled by CP in Nezha2.
Let CP use the CIU->SW_SCRATCH register to  save the current DDR OP.
and AP read the register to get the OP
 */

//Note: dclk_1x = CR5's MEMCLK, so change CR5's freq means change dclk
static  dfc_setting cr5_reg[] = {
 /*pll2ref,pll2fb,apb_spare2, fccr[27:26], 0x09F80000|cc_cp[8:0]  mc_reg_table*/
   {NA ,    NA ,  NA        ,  0x0<<26,              0xC9,           0x18},   //OP0
   {NA ,    NA ,  NA        ,  0x0<<26,              0x4B,           0x8 },   //OP1
   {NA ,    NA ,  NA        ,  0x1<<26,              0x4B,           0x10},   //OP2
   {NA ,    NA ,  NA        ,  0x0<<26,              0x49,           0x18},   //OP3
   {NA ,    NA ,  NA        ,  0x1<<26,              0xC9,           0x20},   //OP4
   {3  ,    46,   0x1012D0E3,  0x2<<26,              0xC8,           0x28},   //OP5
   {NA ,    NA ,  NA        ,  0x0<<26,              0xC8,           0x28},   //OP6
   {3  ,    61,   0x1012E0E3,  0x2<<26,              0xC8,           0x30},   //OP7   
};

axi_freq aclk_value[] = {
   /*label aclk, dvc*/
    {"OP0", 104, 0},
    {"OP1", 156, 0},
    {"OP2", 208, 0},
};

static  axi_setting aclk_reg[] = {
 /*fccr[25|19], cc_cp[17:15]*/
   {0x0<<19,     0x3<<15},   //OP0
   {0x1<<19,     0x3<<15},   //OP1
   {0x0<<19,     0x1<<15},   //OP2
};

#ifdef CONFIG_CA7
cs_freq cs_value[] = {
   /*label atclk, dbg_clk, trace_clk, dvc*/
    {"OP0", 104,   52,      208,      0},
    {"OP1", 416,   208,     208,      0},
};
//atclk is synced to trace_clk and share same FC request bit
//OR 0x00008020 to trigger both freq change of trace_clk and dbg_clk
//Since we only have two OPs for the debug clocks.
static  cs_setting cs_reg[] = {
 /*|trace_config*/
   {0x00080103},   //OP0,
   {0x00080100},   //OP1
};
#endif

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

void get_pll2(uint32_t *pll2, uint32_t *pll2p)
{
    volatile uint32_t temp;
    uint32_t  pll2fb, pll2ref, vcodiv_se, vcodiv_diff;
    uint32_t  refclk = 26;
/*
Fout(CLKOUT) = (4*(N/M) * Fref ) / VCODIV
Fvco = Fout * VCO_SEL[3:0] ;
FVCO = (REFCLK / M) * 4*N >=1.0G
N = FVCO/4/(REFCLK/M)
M: Reference Divider is fixed to 1~3 with Fref=26M.
The higher N/M, the better jitter performance
VCODIV: VCODIV_SEL_SE[3:0]/VCODIV_SEL_DIFF[3:0],
*/
    temp = PMUM->PLL2CR;

    if(temp&0x100) {
    pll2fb = (temp>>10)&0x1FF; //N
    pll2ref= (temp>>19)&0x1F; //M, should be 3

    temp = APBSPARE->PLL2_SW_CTRL;
    vcodiv_se = (temp>>20)& 0x7;
    vcodiv_se = vcodiv_sel2value(vcodiv_se);
    *pll2=((pll2fb*refclk*4)/pll2ref)/vcodiv_se;

    vcodiv_diff = (temp>>17)& 0x7;
    vcodiv_diff = vcodiv_sel2value(vcodiv_diff);
    *pll2p=((pll2fb*refclk*4)/pll2ref)/vcodiv_diff;
    }
    else
    {
        *pll2=0;
        *pll2p=0;
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

//Set up PLL2
void hw_setup_pll2(uint32_t pll2ref, uint32_t pll2fb, uint32_t apb_spare2)
{
    uint32_t temp;

    temp = PMUM->PLL2CR;

    if((temp&0x100)&&((temp&0xF80000)>>19==pll2ref)&&((temp&0x7FC00)>>10==pll2fb)&&
       (APBSPARE->PLL2_SW_CTRL==apb_spare2)){
       /*We actually will not come here if PLL2 is on and already has the expected values.
         Alternatively, we may use PLL2 reference count to keep the info.
         Here we judge it again by PLL2CR[8], may not be correct.
       */
        if(PMUM->PLL2CR & 0x100) 
            bbu_printf("the PLL2 is already enabled at desired value.\r\n");
        else
            bbu_printf("The PLL2 value check is not good enough,we should not get here.\r\n");
        return;
    }
    else
    {
        temp = PMUM->PLL2CR;
        temp |= 0x200; //SW control PLL2
        PMUM->PLL2CR = temp;
        temp &= (~0x100); //disable PLL2 first
        PMUM->PLL2CR = temp;

        udelay(50);

        temp &= (~0xfffc00); //empty [23:10]

        temp |= ((pll2ref<<19)|(pll2fb<<10));
        PMUM->PLL2CR = temp;

        APBSPARE->PLL2_SW_CTRL = apb_spare2;

        PMUM->PLL2CR |= 0x100;  //Enable
        PMUM->PLL2CR &= ~0x200; //return the controls to PMU HW

        udelay(100);
    }

}

static void  sw_turnoff_pll2()
{
    uint32_t temp;
    bbu_printf("turning off PLL2\r\n");
    temp = PMUM->PLL2CR;
    temp |= 0x200; //SW control PLL2
    PMUM->PLL2CR = temp;
    temp &= (~0x100); //disable PLL2 first
    PMUM->PLL2CR = temp;
}


static uint32_t abs_diff(uint32_t value1, uint32_t value2)
{
   if (value1<value2) {
      return (value2 - value1);
   } else {
      return (value1 - value2);
   }
}

/*
Collect all the AP and CP freqs and print out the infos, not include MSA.
*/
#ifdef CONFIG_CA7
op_index aop_get_dump(core_freq *ap_op)
{
    uint32_t pll_dragon, read_reg, temp_reg;
    uint32_t pll2=0, pll2p=0;

    read_reg = PMUA->PLL_SEL_STATUS;
    pll_dragon = (read_reg & 0xC) >> 2;

    switch (pll_dragon)
    {
    case 0:
        pll_dragon = 832;
        break;
    case 1:
        pll_dragon = 1248;
        break;
    case 2:
        get_pll2(&pll_dragon, &pll2p);
        break;
    case 3:
        get_pll2(&pll2, &pll_dragon);
        break;
    }

    //Read the current status
    read_reg = PMUA->DM_CC_AP;
    PMUA->CC_AP |= BIT_31;
    PMUA->CC_AP &= ~BIT_31;

    temp_reg = read_reg & 7;
    ap_op->pclk =pll_dragon / (temp_reg + 1) ;

    temp_reg = (read_reg & 0x00000038) >> 3;
    ap_op->memclk =ap_op->pclk / (temp_reg + 1) ;

    temp_reg = (read_reg & 0x000001C0) >> 6;
    ap_op->busclk =ap_op->pclk / (temp_reg + 1) ;

    uint32_t i = 0;
    while (i<aop_num) { //FIXME:different OP have different MAX.
        if ((abs_diff(ca7_value[i].pclk, ap_op->pclk)<5)&&
            (abs_diff(ca7_value[i].memclk, ap_op->memclk)<5)&&
            (abs_diff(ca7_value[i].busclk, ap_op->busclk)<5)) {
            ap_op->dvc = ca7_value[i].dvc;
            c_aop = (op_index)i;
            break;
        }
        else
        {
            i++;
            c_aop = OP_INVALID;
        }
    }

    bbu_printf("The current CA7 freqs is pclk = %d, memclk= %d, busclk = %d\r\n",
                 ap_op->pclk,ap_op->memclk,ap_op->busclk);
    return c_aop;
}

static uint32_t check_aop(int32_t top)
{
    int32_t aop, dvc_value;
    core_freq temp;

    aop = aop_get_dump(&temp);

    if (aop == OP_INVALID)
        {
            bbu_printf("Warning: CA7 freqs are INVALID OP\r\n");
            return OP_INVALID;
        }

    if (aop == top) {
        #if 0  //FIXME: need to enable HW DVC
        dvc_value = ((gpio_get_pin_level(DVC_level[1]))<<1) + gpio_get_pin_level(DVC_level[0]);
        if(dvc_value < ca7_value[aop].dvc)
            bbu_printf("Warning:the actual DVC value is not set as expected!\r\n");
        #endif
        bbu_printf("The CA7 is running at OP%d\r\n", aop);
        c_aop = top;
        return MV_OK;
    }
    else {
       bbu_printf("Warning: the CA7 OP changed but the freqs are not expected.\r\n");
        c_aop = aop;
       return MV_FAIL;
    }

}

static void dump_aop( unsigned int op_index)
{
        core_freq *md = &(ca7_value[op_index]);

        bbu_printf("%d(%s)---", op_index, md->label);
        bbu_printf("pclk:%d memclk:%d busclk:%d"
                        " pll2:%d pll2p:%d dvc:%d\r\n",
                        md->pclk, md->memclk, md->busclk,
                        md->pll2, md->pll2p, md->dvc);
}

//as a help function information
void dump_aop_list(void)
{
    unsigned int i;
    bbu_printf("AP core OP list \r\n");
    for (i=0; i < aop_num; i++)
    {
        dump_aop(i);
    }
}

void dump_cur_aop(void)
{
    bbu_printf("CA7 Current Operating Point is %d\r\n", c_aop);
    dump_aop(c_aop);
}
#endif

#ifdef CONFIG_CR5
op_index cop_get_dump(core_freq *cp_op)
{
    uint32_t pll_cp, read_reg, temp_reg;
    uint32_t pll2=0, pll2p=0;

    read_reg = PMUA->PLL_SEL_STATUS;
    pll_cp = read_reg & 0x3;

    switch (pll_cp)
    {
    case 0:
        pll_cp = 832;
        break;
    case 1:
        pll_cp = 1248;
        break;
    case 2:
        get_pll2(&pll_cp, &pll2p);
        break;
    case 3:
        get_pll2(&pll2, &pll_cp);
        break;
    }

    //Read the current status
    read_reg = PMUA->DM_CC_CP;
    PMUA->CC_CP |= BIT_31;
    PMUA->CC_CP &= ~BIT_31;

    temp_reg = read_reg & 7;
    cp_op->pclk =pll_cp / (temp_reg + 1) ;

    temp_reg = (read_reg & 0x00000038) >> 3;
    cp_op->memclk =cp_op->pclk / (temp_reg + 1) ;

    if(!(cpu_is_pxa1822_z1())) //Z1 bug NEZHA2-15
    {
    temp_reg = (read_reg & 0x000001C0) >> 6;
    cp_op->busclk =cp_op->pclk / (temp_reg + 1) ;
    }
    else //fake one for Z1
    {
      read_reg = PMUA->CC_CP;
      temp_reg = (read_reg & 0x000001C0) >> 6;
      cp_op->busclk =cp_op->pclk / (temp_reg + 1) ;
    }

    uint32_t i = 0;
    while (i<cop_num) { //FIXME:different OP have different MAX.
        if ((abs_diff(cr5_value[i].pclk, cp_op->pclk)<5)&&
            (abs_diff(cr5_value[i].memclk, cp_op->memclk)<5)&&
            (abs_diff(cr5_value[i].busclk, cp_op->busclk)<5)) {
            cp_op->dvc = cr5_value[i].dvc;
            c_cop = (op_index)i;
            CIU->SW_SCRATCH = c_cop;
            break;
        }
        else
        {
            i++;
            c_cop = OP_INVALID;
            CIU->SW_SCRATCH = c_cop;            
        }
    }

    bbu_printf("The current CR5 freqs is pclk = %d, memclk|dclk_1x= %d, busclk = %d\r\n",
                 cp_op->pclk,cp_op->memclk,cp_op->busclk);
    return c_cop;
}

static uint32_t check_cop(int32_t top)
{
    int32_t cop, dvc_value;
    core_freq temp;

    cop = cop_get_dump(&temp);

    if (cop == OP_INVALID)
        {
            bbu_printf("Warning: CR5 freqs are INVALID OP\r\n");
            return OP_INVALID;
        }

    if (cop == top) {
        #if 0  //FIXME: need to enable HW DVC
        dvc_value = ((gpio_get_pin_level(DVC_level[1]))<<1) + gpio_get_pin_level(DVC_level[0]);
        if(dvc_value < cr5_value[aop].dvc)
            bbu_printf("Warning:the actual DVC value is not set as expected!\r\n");
        #endif
        bbu_printf("The CR5 is running at OP%d\r\n", cop);
        c_cop = top;
        CIU->SW_SCRATCH = c_cop;        
        return MV_OK;
    }
    else {
       bbu_printf("ERROR: the CR5 OP changed but the freqs are not expected.\r\n");
        c_cop = cop;
        CIU->SW_SCRATCH = c_cop;        
       return MV_FAIL;
    }

}

static void dump_cop( unsigned int op_index)
{
        core_freq *md = &(cr5_value[op_index]);

        bbu_printf("%d(%s)---", op_index, md->label);
        bbu_printf("pclk:%d memclk:%d busclk:%d"
                        " pll2:%d pll2p:%d dvc:%d\r\n",
                        md->pclk, md->memclk, md->busclk,
                        md->pll2, md->pll2p, md->dvc);
}

//as a help function information
void dump_cop_list(void)
{
    unsigned int i;

    bbu_printf("CP core OP list \r\n");
    for (i=0; i < cop_num; i++)
    {
        dump_cop(i);
    }
}

void dump_cur_cop(void)
{
    bbu_printf("CR5 Current Operating Point is %d\r\n", c_cop);
    dump_cop(c_cop);
}

#endif

op_index axiop_get_dump(axi_freq *axi_op)
{
    uint32_t pll_aclk, read_reg, temp_reg;
    uint32_t pll2p=0;

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
        get_pll2(&pll_aclk, &pll2p);
        break;
    case 3:
        pll_aclk = 312;
        break;
    }


#ifdef CONFIG_CA7
   //Read the current status
    read_reg = PMUA->DM_CC_AP;
    PMUA->CC_AP |= BIT_31;
    PMUA->CC_AP &= ~BIT_31;
    temp_reg = (read_reg & 0x00038000) >> 15;
#endif

#ifdef CONFIG_CR5
    //Read the current status
    read_reg = PMUA->DM_CC_CP;
    PMUA->CC_CP |= BIT_31;
    PMUA->CC_CP &= ~BIT_31;
    if(!(cpu_is_pxa1822_z1())) //A0
    {
        temp_reg = (read_reg & 0x00038000) >> 15;
    }
    else //Z1 bug NEZHA2-15
    {
        temp_reg = (read_reg & 0x00007000) >> 12;
    }
#endif

    axi_op->aclk = pll_aclk / (temp_reg + 1) ;

    uint32_t i = 0;
    while (i<axiop_num) { //FIXME:different OP have different MAX.
        if (abs_diff(aclk_value[i].aclk, axi_op->aclk)<5) {
            axi_op->dvc = aclk_value[i].dvc;
            c_axiop = (op_index)i;
            break;
        }
        else
        {
            i++;
            c_axiop = OP_INVALID;
        }
    }

    bbu_printf("The current AXI freq is aclk = %d\r\n",axi_op->aclk);
    return c_axiop;
}

static uint32_t check_axiop(int32_t top)
{
    int32_t axiop, dvc_value;
    axi_freq temp;

    axiop = axiop_get_dump(&temp);

    if (axiop == OP_INVALID)
        {
            bbu_printf("Warning: AXI freq is INVALID OP\r\n");
            return OP_INVALID;
        }

    if (axiop == top) {
        #if 0  //FIXME: need to enable HW DVC
        dvc_value = ((gpio_get_pin_level(DVC_level[1]))<<1) + gpio_get_pin_level(DVC_level[0]);
        if(dvc_value < cr5_value[aop].dvc)
            bbu_printf("Warning:the actual DVC value is not set as expected!\r\n");
        #endif
        bbu_printf("The AXI is running at OP%d\r\n", axiop);
        c_axiop = top;
        return MV_OK;
    }
    else {
       bbu_printf("ERROR: the AXI freq changed but the values are not expected.\r\n");
       c_axiop = axiop;
       return MV_FAIL;
    }
}

static void dump_axiop( unsigned int op_index)
{
    axi_freq *md = &(aclk_value[op_index]);

    bbu_printf("%d(%s)---", op_index, md->label);
    bbu_printf("aclk:%d dvc:%d\r\n", md->aclk, md->dvc);
}

//as a help function information
void dump_axiop_list(void)
{
    unsigned int i;
    bbu_printf("AXI OP list \r\n");
    for (i=0; i < axiop_num; i++)
    {
        dump_axiop(i);
    }
}

void dump_cur_axiop(void)
{
    bbu_printf("AXI Current Operating Point is %d\r\n", c_axiop);
    dump_axiop(c_axiop);
}

#ifdef CONFIG_CA7
/*AP core freq change function*/
static void adfc_set(uint32_t top)
{
    uint32_t read_data;
    uint32_t FCCR_MASK = 0x1C00FFFF; //only change dragon_core/ddr/axi, don't touch Seagull and I2S clock

    //+++Step4: set up correct PMUA_DEBUG value.
    read_data = PMUA->DEBUG_REG;

    read_data |= 0x00660001;

    //only AP can reset/release CP, so CP is in reset state or not is certain during FC process.
    if (PMUM->APRR & 0x1) { //CP is hold in reset
        /* Change the AP freq clk, ignore CP, this is for AP standalone debug */
        read_data |= 0x9;
    }
    else { //CP is already released
        /* Need to wait CP's ACK on halt and clock off */
        read_data &= ~0x9;
    }

    PMUA->DEBUG_REG = read_data;

    //+++Step6:Set the PMUM_FCCR value, which only take effect after the frequency change command issued.
    PMUM->FCCR = (PMUM->FCCR & FCCR_MASK)| ca7_reg[top].pmum_fccr;
    /*per DE: all PMUM registers are in APB side(26MHz domain) while PMUA registers are in AXI side(alck domain).
     The time gap between the write response of FCCR and the exact update of FCCR is more than six 26MHz cycles.
     WR: make sure to do a FCCR read after write (RAW) to make sure it's really updated for application.
     That's SOC issue.  while dsb() only regulate the core's behavior, no use here.
    */
    read_data = PMUM->FCCR;
    //+++Step7:enable FC done interrupt by AP/BUS/DDR Intr mask
    //BIT1 is set at T after all BIT3|BIT4|BIT5 is set. T is PMU internal mataince time after FC is done.
    PMUA->AP_IMR |= BIT_1; //BIT1=BIT3|BIT4|BIT5@delay T

    //+++Step8:Seagull allow freq. change voting
    PMUA->CC_CP |= (0x1 << 27);

    //+++Step9: AP set for clock dividers and kick off the sm_fc
    PMUA->CC_AP &= ~0x1ff;
    PMUA->CC_AP |= ca7_reg[top].pmua_cc_ap_cp;
    PMUA->CC_AP |= 0x01000000;

    //+++Step10: wait for AP freq change done (there are handshake between PMU and cores/axi/ddr)
    while ( !(BIT_1 & PMUA->AP_ISR) ) ;

    //+++Step11(opt):calibrate the system tick count according to new PP if the CPU local timers are used.

    //+++Step12: Clear the AP freq change done interrupt status
    //write 1 to bit has no effect, write 0 clear
    PMUA->AP_ISR &= 0xFFFFFFFD;

}
#endif

#ifdef CONFIG_CR5
static void cdfc_set(uint32_t top)
{
    uint32_t read_data;
    //+++Step4: set up correct PMUA_DEBUG value.
    //core1_wfi_fc, allow AP side FC when CP is in WFI, BIT[22:21] always set. it's different to reset related bits
    PMUA->DEBUG_REG |= 0x00660001 ;

    PMUA->MC_HW_SLP_TYPE &= 0xFFFFFF87;
    PMUA->MC_HW_SLP_TYPE |= cr5_reg[top].mc_reg_table;

    PMUA->CC_AP |= 0x08000000;
    PMUA->CC_CP |= 0x08000000;
    PMUM->FCCR  &= ~(3<<26);
    PMUM->FCCR  |= cr5_reg[top].pmum_fccr;
    read_data = PMUM->FCCR;

    PMUA->CP_IMR = 0x1;
    PMUA->CP_IRWC &= ~0x1;
    PMUA->CC_CP &= ~0x1ff;
    PMUA->CC_CP |= cr5_reg[top].pmua_cc_ap_cp;
    PMUA->CC_CP |= 0x01000000;

    while( (PMUA->CP_ISR & 0x1) == 0 );
    PMUA->CP_ISR &= ~0x1;
}
#endif

static void axidfc_set(uint32_t top)
{
    uint32_t read_data;
    PMUA->DEBUG_REG |= 0x00660001 ;

    PMUA->CC_AP |= BIT_27;  
    PMUA->CC_CP |= BIT_27;
    PMUM->FCCR  &= ~(1<<25);
    PMUM->FCCR  &= ~(1<<19);
    PMUM->FCCR  |= aclk_reg[top].pmum_fccr;

    read_data = PMUM->FCCR;

#ifdef CONFIG_CR5
    PMUA->CP_IMR = 0x1;
    PMUA->CP_IRWC &= ~0x1;
    PMUA->CC_CP &= ~0x38000;
    PMUA->CC_CP |= aclk_reg[top].pmua_cc_ap_cp;
    PMUA->CC_CP |= BIT_26;
    while( (PMUA->CP_ISR & 0x1) == 0 );
    PMUA->CP_ISR &= ~0x1;
#endif

#ifdef CONFIG_CA7
    PMUA->AP_IMR = 0x2;
    PMUA->AP_IRWC &= ~0x2;
    PMUA->CC_AP &= ~0x38000;
    PMUA->CC_AP |= aclk_reg[top].pmua_cc_ap_cp;
    PMUA->CC_AP |= BIT_26;
    while( (PMUA->AP_ISR & 0x2) == 0 );
    PMUA->AP_ISR &= ~0x2;
#endif
}

/*
You don’t need release CP from reset state.
You can let AP write PMU_CC_CP to trigger CP freq change
Only through this CC_CP dclk can be changed!
*/

static void get_fc_sm()
{
  uint32_t  stepping = 0;
    /*Because AP and CP shared the same FC state machine in Emei. FCCR is shared register.
    The two dummy registers are used to make sure that AP and CP does FC exclusively.
    So, each PP change process need to get the control of fc_sm at the very beginning.
   */
#ifdef CONFIG_CA7
    while (PMUA->DM_CC_AP & BIT_24) {
      //if fail to get control once, clear the flag to avoid deadlock with CP
        PMUA->CC_AP |= (BIT_31);
        PMUA->CC_AP &= ~(BIT_31);
    }
#endif

#ifdef CONFIG_CR5
    if(cpu_is_pxa1822_z1())
    {
      stepping = BIT_22;
    }
    else
    {
      stepping = BIT_25;
    }

    while (PMUA->DM_CC_CP & stepping) {
      //if fail to get control once, clear the flag to avoid deadlock with CP
        PMUA->CC_CP |= (BIT_31);
        PMUA->CC_CP &= ~(BIT_31);
    }
#endif
}

static void put_fc_sm()
{
    uint32_t read_data, temp_data;

#ifdef CONFIG_CA7
    read_data = PMUA->CC_AP;
    temp_data = (read_data & 0x0AFFFFFF) | BIT_31; //RD_ST clear
    PMUA->CC_AP = temp_data;
    PMUA->CC_AP = temp_data & 0x0AFFFFFF;
#endif

#ifdef CONFIG_CR5
    read_data = PMUA->CC_CP;
    temp_data = (read_data & 0x0AFBFFFF) | BIT_31; //RD_ST clear
    PMUA->CC_CP = temp_data;
    PMUA->CC_CP = temp_data & 0x0AFBFFFF;
#endif

}

static void update_memory_xtc(uint32_t pclk)
{
#if 0 //FIXME
  if(pclk>=628) //628MHz~1066MHz
  {
    CIU->CA7_CPU_SRAM_CONF_0 = 0x02666666; //L1 RTC/WTC
    CIU->CA7_CPU_SRAM_CONF_1 = 0x00006265; //L2 RTC/WTC
  }
  else //<=624MHz
  {
    CIU->CA7_CPU_SRAM_CONF_0 = 0x02222222; //L1 RTC/WTC
    CIU->CA7_CPU_SRAM_CONF_1 = 0x00002221; //L2 RTC/WTC
  }
#endif
}

#define NUM_PROFILES    9
#define VOL_LEVELS      4
static uint32_t getProfile()
{
    uint32_t uiFuses = 0, uiProfile = 0;
    uint32_t uiTemp = 3, uiTemp2 = 3;
    uint32_t i;

    /* bit 240 ~ 255 for Profile information */
    uiFuses = ((GEU->BLOCK0_RESERVED_1) >> 16) & 0x0000FFFF;

    for (i = 0; i < NUM_PROFILES; i++) {
            uiTemp |= uiTemp2 << (i * 2);
            if (uiTemp == uiFuses)
                    uiProfile = i + 1;
    }

    return uiProfile; //default is 0
}

uint32_t svc_table_z1[NUM_PROFILES][VOL_LEVELS] = {
    {950, 975, 1050, 1125},       /* profile 0 */
    {950, 975, 1050, 1125},       /* profile 1 */
    {950, 975, 1050, 1125},       /* profile 2 */
    {950, 975, 1050, 1125},       /* profile 3 */
    {950, 975, 1050, 1125},       /* profile 4 */
    {950, 975, 1050, 1125},       /* profile 5 */
    {950, 975, 1050, 1125},       /* profile 6 */
    {950, 975, 1050, 1125},       /* profile 7 */
    {950, 975, 1050, 1125},       /* profile 8 */
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

    if (cpu_is_pxa1822_z1()||cpu_is_pxa1822_z2())
    {
        profile = getProfile();
        bbu_printf("The SVC profile is %d\r\n", profile);
        svc_init_table = &(svc_table_z1[profile][0]);
    }
#ifdef CONFIG_DVC     //HW-DVC    
#ifdef CONFIG_CA7
    //10uS for SOC to PMIC DVC pin response/debounce
    PMU_DVC->DVC_EXTRA_STR = 260;
    /*
    1,all above is just a change request to PMU!
    2,deeper lpm modes has high priority vote in PMU view
    */
    //Here assumes the PMIC set the ramp rate to 12.5mV/uS, otherwise need change!
    PMU_DVC->VL01STR = (((svc_init_table[DVC01] - svc_init_table[DVC00]) * 2 + 24) / 25) * 26;
    PMU_DVC->VL12STR = (((svc_init_table[DVC10] - svc_init_table[DVC01]) * 2 + 24) / 25) * 26;
    PMU_DVC->VL23STR = (((svc_init_table[DVC11] - svc_init_table[DVC10]) * 2 + 24) / 25) * 26;

    //Enable the DVC from SOC side,  it will really take effect after  PMIC enable DVC!
    PMU_DVC->DVCR = 0x3;
#endif    
#endif    
}

/*
*NOTE: be careful about the DVC[1:0] initial value, at the first bring up stage,
****************************************************************************************
*						Power allocation on HelanLTE DKB
****************************************************************************************
*	VBUCK1 -->  Vcc_main, 	default 1.1V, default on,
*	VBUCK2 -->  RF, 		default 1.8V, default on, should always on
*	VBUCK3 -->  VDD2... 	default 1.2V, default on, should always on
*	VBUCK4 -->  GPIO...,	default 1.8V, default on, should always on
*	VBUCK5 -->  RF PA,		default 1.1V, default off
*	  LDO2 -->  AVDD18_PLL,	default 1.8V, default off, should always on
****************************************************************************************
*/

#ifdef CONFIG_CA7
static void pmic_dvc_init(void)
{
    //set POWER_HOLD to enable DVC and reset audio related registers to zero
	//NOTE: this should be before buck1 DVC init, otherwise the the Vcc_main
	//will be set to DVC00 voltage, causing boot hang.
    //ustica_base_write(0x0D, 0xC0);

    //VBUCK1,4&LDO1 have DVC control, others have sleep/active difference
    //Here we rely on the defalut ramp rate 12.5mV/uS, or need to support set it
    vbuck1_dvc_init();
    udelay(50);
    vbuck2_dvc_init();
    vbuck3_dvc_init();
    vbuck4_dvc_init();
#if 0
    //Increase the current limit of Buck1, otherwise PMIC can't survive mlucas+GC MMO6 stress.
    //set VBUCK1 to full drive,VBUCK1->VCC_MAIN
    ustica_power_write(0x3A, 0x0f);

    //SLEEP mode, set VBUCK1 to sleep mode when sleepn is deasserted
    //VBUCK2,VBUCK3,VBUCK4,VBUCK5: always active, LDO2: always active
    ustica_power_write(0x5A, 0xFD);
	//ustica_power_write(0x5B, 0x03);

    //tune the drive setting for the buck1 to get high efficency during different DVCs, ramp rates,etc.
    //ustica_vbuck_misc_cfg(VBUCK1,0,12500,DVC00_DRIVE_EN|DVC01_DRIVE_EN|DVC10_DRIVE_EN|DVC11_DRIVE_EN);

    //set POWER_HOLD to enalbe DVC and reset audio related registers to zero
    //ustica_base_write(0x0D, 0xC0);

    //Disable PMIC watchdog
    ustica_base_write(0x1D, 0x01);

    //set USE_XO=1 to indicate that the 32k XO is settled (usually 1s after SOD power-up,
    //bootrom/obm should cost that already, so we don't wait here.)
    //According to PMIC datasheet chapter 5.2 table 24: the 32k XO Tstart = 1s typical and 10s maximum.
    ustica_base_write(0xD0, 0x80);

    //and disable FLL OSC_FREERUN mode to lock PMIC bucks/etc to use the XO clock instead
    ustica_base_write(0x50, 0x0E); //OSC_FREERUN ->0
    ustica_base_write(0x55, 0x0E); //OSCD_FREERUN ->0

    //select the internal filter capacitance value to 22pF for XTAL1 and XTAL2 pins so that 32KHz is stable
    #if 1  //for some customer like SS has external capacitor, this setting will cause PMIC deadlock
    ustica_base_write(0xE8, 0x70);
    #endif

    //32k_OUTx_SEL: output PMIC internal 32KHz or 32KHz from XO (if USE_XO=1). also enable Power up good monoitor
    //The set of 32k_OUT_SEL is kept in "power-down" state, it resets to default only in RTC POR.
    ustica_base_write(0xE2, 0x45);

    //Enable buck and LDO clock gating and disable voltage change during low poewr mode to save power
    ustica_base_write(0x22, 0x80);

    //set MEAS_EN_SLP to enable GPADC low power mode to save power
    ustica_gpadc_write(0x6, 0x11);
#endif
    //switching the host 32k to XO clock for accuracy(+/-30ppm)
    PMUM->SCCR |= 0x1;

#ifdef CONFIG_MMT
    //Reset the PMIC watchdog---it's disabled by HW on board on Emei DKB.
	pmic_watchdog_enable();
#endif
}

#endif  //CONFIG_CA7

/*This should be called by PMU_AO init*/
void pp_init()
{
    core_freq temp_aop, temp_cop;
    axi_freq temp_axi_op;

#ifdef CONFIG_CA7
    aop_num = sizeof(ca7_value)/sizeof(core_freq);
    csop_num = sizeof(cs_value)/sizeof(cs_freq);    
#endif
    cop_num = sizeof(cr5_value)/sizeof(core_freq);
    axiop_num = sizeof(aclk_value)/sizeof(axi_freq);

    axiop_get_dump(&temp_axi_op);

/*BIT_25: let AXI clock does not trigger core clock at the same time.
  Because the sm_fc design let axi fc trigger both ap&cp core fc.
  In Nezha2 case, cp core fc means dclk fc, it need the reg tables 
  are set and assigned for current dclk if reg-table based FC bit is set.
  The best decouple solution is to set BIT_25 to not trigger core fc
  
  CC_CP&CC_AP[24:26,30]: those bits are mutexed by DM_CC_CP/AP, once the
  fc_sm is controlled by AP, it can't change CC_CP[24:26,30], and vice-versa.*/

    PMUA->CC_CP |= BIT_25;
    PMUA->CC_AP |= BIT_25;  

#ifdef CONFIG_CR5
    /*
    BIT_7: enable table-based DDR clock change
    BIT_8: use 2:1 mode for LPDDR2
    BIT_9: enable the lpm exit ddr reg table 0
     */
    PMUA->MC_HW_SLP_TYPE &= ~(BIT_9 | BIT_7 | BIT_8);

    cop_get_dump(&temp_cop);

    if(c_cop == OP_INVALID)
    {
        bbu_printf("Note:current CR5/DDR OP is not official defined.\r\n");
        cdfc_set(0);
        c_cop = 0;
    }
    CIU->SW_SCRATCH = c_cop; 

    CIU->CR5_SRAM_CONF = 0x44444445; //L1 RTC/WTC
    CIU->SEAGULL_L2C_SRAM_CONF= 0x00000044; //L2 RTC/WTC
        
    max_dvc = cr5_value[c_cop].dvc;
    if(max_dvc !=1)bbu_printf("Warning: the boot up PP DVL  assumption is wrong!\r\n");
   // assume CP side boot OP require dvl1, may change later
#endif

#ifndef CONFIG_DVC     //both AP and CP should know the GPIO for SW DVC
    DVC_level[1] = GPIO127; //SM_BE1,FUNCTION 0
    DVC_level[0] = GPIO13;  //SM_SCLK, FUNCTION 3
#endif

    //write initial dvc value
    soc_dvc_init(); //set up the SOC DVC init settings, both SW-DVC or HW-DVC

#ifdef CONFIG_CA7    //Below works only need to be done by CA7
//Note: make sure CP is released after  pp_init so that CP can get correct c_axiop
    if (c_axiop == OP_INVALID)
    {
        bbu_printf("Note:current AXI OP is not official defined.\r\n");
        axidfc_set(0);
        c_axiop = 0;
    }

    aop_get_dump(&temp_aop);
    if (c_aop == OP_INVALID)
    {
        bbu_printf("Note:current CA7 OP is not official defined.\r\n");
        adfc_set(0); //should not use adfc invoking DVC here
        c_aop = 0;
    }

    /*The RTC/WTC settings charaterized from SV, can decrease the Vmin
      Here is the initial settings, it can change on the fly along with PPSET*/
    CIU->CA7_CPU_SRAM_CONF_0 = 0x00444444; //L1 RTC/WTC
    CIU->CA7_CPU_SRAM_CONF_1 = 0x00008088; //L2 RTC/WTC

    //csop_get_dump(&temp_csop);
 /*Note: we direclty do the CS change to PP0 for simple*/
    PMUA->TRACE_CONFIG &= ~0x003E07C7;
    PMUA->TRACE_CONFIG |= cs_reg[0].trace_config;
    PMUA->TRACE_CONFIG |= 0x00008020;
    bbu_printf("Coresight OP changed to OP0\r\n");
    c_csop = 0;

   /*Prepare the DVC settings before enable the DVC mechanism.*/
  max_dvc = ca7_value[c_aop].dvc;
  if(max_dvc < aclk_value[c_axiop].dvc) max_dvc = aclk_value[c_axiop].dvc;
  if(max_dvc < cs_value[c_csop].dvc) max_dvc = cs_value[c_csop].dvc;  
  if(max_dvc < 1) max_dvc = 1; //for SW DVC and assume CP side boot OP need DVL1.
  
#ifdef CONFIG_DVC     //HW-DVC
    MFPR->SM_nBE1 = 0x9046; //SM_nBE1,function 6,no pull
    MFPR->SM_SCLK = 0x9045; //SM_SCLK,function 5,no pull
    PMU_DVC->DVC_AP = 0x80 | (max_dvc << 4);
 // assume CP side boot OP require dvl1, may change later
    PMU_DVC->DVC_CP = 0x80 | (1 << 4);      
#else    //GPIO-LEVEL based DVC, Function 0,
    //*(volatile uint32_t *)(0xd401e070) = 0x9040; //SM_BE1,function 0,no pull
    //*(volatile uint32_t *)(0xd401e064) = 0x9043; //SM_SCLK,function 3,no pull
    MFPR->SM_nBE1 = 0x9040; //SM_nBE1,function 0,no pull
    MFPR->SM_SCLK = 0x9043; //SM_SCLK,function 3,no pull

    gpio_output_set(DVC_level[1], (max_dvc & 0x2) >>1);  //DVC2 init value
    gpio_output_set(DVC_level[0], max_dvc & 0x1);  //DVC1 init value
    //In GPIO module, MOS transistor is used to set high/low ouput.
    gpio_set_output(DVC_level[1]); //enable output
    gpio_set_output(DVC_level[0]);
#endif
    pmic_dvc_init(); //set up DVC,etc. and enable DVC here
#endif
}

/*each DVC_AP/DVC_CP register has only one entry funtion to update.
Need spinlock to pretect  if we have multi-core in AP side.
For Nezha2,  just put the process in critical section is enough*/
//assuming DVC[2:1] relates to level[2:1] value and high level conrespondes to high voltage.
//For AP:
//          initiator: 1-core, 2-AXI, 3-Coresight
//For CP:
//          initator:  1-core/DDR
// In case SW DVC case,  CP can't adjust the voltage and AP is the owner
// And even the DVL0 should be higher enough to cover all CP/DP PPs.
// That's why we need HW-DVC!!!!
void update_voltage_dvcl2h (uint32_t initiator, uint32_t top)
{
    uint32_t  max_dvc_next;

#ifdef CONFIG_CA7
    if (1 == initiator) //core
    {
        max_dvc_next = ca7_value[top].dvc;
        if (max_dvc_next < aclk_value[c_axiop].dvc)  max_dvc_next = aclk_value[c_axiop].dvc;
        if (max_dvc_next < cs_value[c_csop].dvc) max_dvc_next = cs_value[c_csop].dvc;
    }
    else if (2 == initiator) //AXI
    {
        max_dvc_next = aclk_value[top].dvc;
        if (max_dvc_next < ca7_value[c_aop].dvc)  max_dvc_next = ca7_value[c_aop].dvc;
        if (max_dvc_next < cs_value[c_csop].dvc) max_dvc_next = cs_value[c_csop].dvc;
    }
    else if (3 == initiator) //Coresight
    {
        max_dvc_next = cs_value[top].dvc;
        if (max_dvc_next < ca7_value[c_aop].dvc)  max_dvc_next = ca7_value[c_aop].dvc;
        if (max_dvc_next < aclk_value[c_axiop].dvc)  max_dvc_next = aclk_value[c_axiop].dvc;
    }
    else
    {
        bbu_printf("Warning: unsuppoted DVC initiator!\r\n");
    }
#endif

#ifdef CONFIG_CR5
    if (1 == initiator) //core
    {
        max_dvc_next = cr5_value[top].dvc;
    }
    else
    {
        bbu_printf("Warning: unsuppoted DVC initiator!\r\n");
    }
#endif

    bbu_printf("l2h:The max_dvc_next = %d\r\n ", max_dvc_next);

    if(max_dvc_next > (uint32_t) DVC11) 
    {
        bbu_printf("ERROR:invalid dvc_next\r\n");
    }
    else if (max_dvc_next < max_dvc)
    {
        bbu_printf("ERROR: wrong voltage direction\r\n");
    }
    else
    {
#ifdef CONFIG_DVC     //HW-DVC
        //Note, need to write DVC_AP in one step!!!
#ifdef CONFIG_CA7
        PMU_DVC->DVC_IMR |= BIT_0;
        PMU_DVC->DVC_AP = (PMU_DVC->DVC_AP & (~(0x7 << 4))) | 0x80 | (max_dvc_next << 4) ;
        while ( !(BIT_0 & PMU_DVC->DVC_ISR) ) ; 
        PMU_DVC->DVC_ISR = ~BIT_0; //write 0 clear, write 1 has no effect
        bbu_printf("The DVC_STATUS = 0x%x\r\n", PMU_DVC->DVC_STATUS);
#endif
#ifdef CONFIG_CR5
        PMU_DVC->DVC_IMR |= BIT_1;        
        PMU_DVC->DVC_CP = (PMU_DVC->DVC_CP & (~(0x7 << 4))) | 0x80 | (max_dvc_next << 4) ;
        while ( !(BIT_1 & PMU_DVC->DVC_ISR) ) ; 
        PMU_DVC->DVC_ISR = ~BIT_1; //write 0 clear, write 1 has no effect
        bbu_printf("The DVC_STATUS = 0x%x\r\n", PMU_DVC->DVC_STATUS);        
#endif
#else
#ifdef CONFIG_CA7
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
#endif
        max_dvc =  max_dvc_next;
    }

}

void update_voltage_dvch2l (uint32_t initiator, uint32_t top)
{
    uint32_t  max_dvc_next;

#ifdef CONFIG_CA7
    if (1 == initiator) //core
    {
        max_dvc_next = ca7_value[top].dvc;
        if (max_dvc_next < aclk_value[c_axiop].dvc)  max_dvc_next = aclk_value[c_axiop].dvc;
        if (max_dvc_next < cs_value[c_csop].dvc) max_dvc_next = cs_value[c_csop].dvc;
    }
    else if (2 == initiator) //AXI
    {
        max_dvc_next = aclk_value[top].dvc;
        if (max_dvc_next < ca7_value[c_aop].dvc)  max_dvc_next = ca7_value[c_aop].dvc;
        if (max_dvc_next < cs_value[c_csop].dvc) max_dvc_next = cs_value[c_csop].dvc;
    }
    else if (3 == initiator) //Coresight
    {
        max_dvc_next = cs_value[top].dvc;
        if (max_dvc_next < ca7_value[c_aop].dvc)  max_dvc_next = ca7_value[c_aop].dvc;
        if (max_dvc_next < aclk_value[c_axiop].dvc)  max_dvc_next = aclk_value[c_axiop].dvc;
    }
    else
    {
        bbu_printf("Warning: unsuppoted DVC initiator!\r\n");
    }
#endif

#ifdef CONFIG_CR5
    if (1 == initiator) //core
    {
        max_dvc_next = cr5_value[top].dvc;
    }
    else
    {
        bbu_printf("Warning: unsuppoted DVC initiator!\r\n");
    }
#endif

    bbu_printf("h2l:The max_dvc_next = %d\r\n ", max_dvc_next);

    if(max_dvc_next > (uint32_t) DVC11) 
    {
            bbu_printf("ERROR:invalid dvc_next\r\n");
    }
    else if (max_dvc_next > max_dvc)
    {
        bbu_printf("ERROR: wrong voltage direction\r\n");
    }
    else
    {
#ifdef CONFIG_DVC     //HW-DVC
        //Note, need to write DVC_AP in one step or value first and set the vote bit later!!!
#ifdef CONFIG_CA7
        PMU_DVC->DVC_AP = (PMU_DVC->DVC_AP & (~(0x7 << 4))) | 0x80 | (max_dvc_next << 4) ;
#endif
#ifdef CONFIG_CR5
        PMU_DVC->DVC_CP = (PMU_DVC->DVC_CP & (~(0x7 << 4))) | 0x80 | (max_dvc_next << 4) ;
#endif
#else
#ifdef CONFIG_CA7
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
#endif
        //With SW-DVC, we need to make sure CP can work at the lowest DVL
#endif
        max_dvc =  max_dvc_next;
    }
}


#ifdef CONFIG_CA7
/*Must make sure there is only one entity of this function is in executing at any time,
  better to be in critial section, return value is OP_INVALID or MV_OK*/
uint32_t adfc(uint32_t top)
{
    uint32_t pll2=0, pll2p=0; //identify whether pll2 is needed
    uint32_t flags, fccr;

    if(top == c_aop)
    {
        bbu_printf("CA7 is already at OP %d!\r\n", c_aop);
        dump_cur_aop();
        return c_aop;
    }

    bbu_printf("CA7 OP changing from %d to %d \r\n", c_aop, top);

    /*In decoupled DFC solution, we should predefine the freq values of PLLs, NO MFC here!
    Just need to check the PLL2 is needed or not. We assume 0 is OFF state of PLL.
    */
   if(ca7_value[top].pll2 || ca7_value[top].pll2p) {
            //get the current pll2 and pll2p value
    get_pll2(&pll2, &pll2p);
    if(((abs_diff(ca7_value[top].pll2, pll2)>=5)&&pll2&&ca7_value[top].pll2) ||
    ((abs_diff(ca7_value[top].pll2p, pll2p)>=5)&&pll2p&&ca7_value[top].pll2p)){
      bbu_printf("FAIL:PLL2 is used at another frequency pll2 = %d, pll2p= %d already!\r\n", pll2,pll2p);
      bbu_printf("In decoupled DFC solution, PLLs shoud take fixed values, avoid MFC.\r\n");
      bbu_printf("For test purpose, please switch all clock to PLL1 based first\r\n");
      return c_aop;
      }
      else
      {
            //+++Step3: set up PLL2 or PLL3 or PLL1_1248 according to pp_reg table,
    //use HW control method so that it on/off with PLL1 at LPM
        bbu_printf("Enable PLL2=%d, PLL2P=%d \r\n", ca7_value[top].pll2, ca7_value[top].pll2p);
        hw_setup_pll2(ca7_reg[top].pll2ref, ca7_reg[top].pll2fb, ca7_reg[top].apb_spare2);
      }
    }

    get_fc_sm();
    flags = disable_irq_fiq();
    //Now we get the control of fc_sm, we can start to do the FC from here

    //+++Step1: increase the voltages if it's raise PP using the GPIO/DVC mechanism
    if(ca7_value[top].dvc > ca7_value[c_aop].dvc)
    {
      update_voltage_dvcl2h (1, top);        
    }    
    //+++Step1-1: change the L1/L2 XTC values
    if(ca7_value[top].pclk > ca7_value[c_aop].pclk)
    {
      update_memory_xtc(ca7_value[top].pclk);
    }
    //+++Step2: use intermediate PP to avoid MFC if necessary
    /*
    Note:PMUA_AP_IMR[3]/PMUA_AP_ISR[3] is only valid for fc_sm FC, not valid for quick dynamic FC.
    We don't need it at this case because there are no hankshake between core and PMU,
    core will go on after finish and no need to wait here.
    */

    //+++Step4~12 the actual DFC process.
    adfc_set(top);

    //+++Step13: Disable PLL2 if not in use by SW control method.
    //We assume only CA7 and CR5+DDR will use PLL2
    fccr = PMUM->FCCR;
    bbu_printf("fccr = 0x%x\r\n", fccr);
    if(((fccr & BIT_30)==0)&&(((fccr & BIT_25)!=1)||((fccr & BIT_19) !=0))&&
       ((fccr & BIT_27)==0)) {
        sw_turnoff_pll2();
    }

    //+++Step14-pre: change the L1/L2 XTC values
    if(ca7_value[top].pclk < ca7_value[c_aop].pclk)
    {
      update_memory_xtc(ca7_value[top].pclk);
    }

    if(ca7_value[top].dvc < ca7_value[c_aop].dvc)
    {
      update_voltage_dvch2l(1, top);  
    }
    
    //+++Step15(opt):clear of RD_ST to indicate APP FC is done
    //This bit and the FC request bits should have already been cleared by hardware at this time,
    //Here we just do it to make sure
    put_fc_sm();

    restore_irq_fiq(flags);

    //+++Step16: Get current OP and compare with Target OP, if not equal, then fails.
    //compare and cpp update inside
    return (check_aop(top));
}
#endif

#ifdef CONFIG_CR5

/*Must make sure there is only one entity of this function is in executing at any time,
  better to be in critial section, return value is OP_INVALID or MV_OK*/
uint32_t cdfc(uint32_t top)
{
    uint32_t pll2=0, pll2p=0; //identify whether pll2 is needed
    uint32_t flags, fccr;

    if(top == c_cop)
    {
        bbu_printf("CR5 is already at OP %d!\r\n", c_cop);
        dump_cur_cop();
        return c_cop;
    }

    bbu_printf("CR5 OP changing from %d to %d \r\n", c_cop, top);

    /*In decoupled DFC solution, we should predefine the freq values of PLLs, NO MFC here!
    Just need to check the PLL2 is needed or not. We assume 0 is OFF state of PLL.
    */
   if(cr5_value[top].pll2 || cr5_value[top].pll2p) {
            //get the current pll2 and pll2p value
        get_pll2(&pll2, &pll2p);
        if(((abs_diff(cr5_value[top].pll2, pll2)>=5)&&pll2&&cr5_value[top].pll2) ||
        ((abs_diff(cr5_value[top].pll2p, pll2p)>=5)&&pll2p&&cr5_value[top].pll2p)){
        bbu_printf("FAIL:PLL2 is used at another frequency pll2 = %d, pll2p= %d already!\r\n", pll2,pll2p);
        bbu_printf("In decoupled DFC solution, PLLs shoud take fixed values, avoid MFC.\r\n");
        bbu_printf("For test purpose, please switch all clock to PLL1 based first\r\n");
        return c_cop;
        }
        else
        {
            //+++Step3: set up PLL2 or PLL3 or PLL1_1248 according to pp_reg table,
    //use HW control method so that it on/off with PLL1 at LPM
            hw_setup_pll2(cr5_reg[top].pll2ref, cr5_reg[top].pll2fb, cr5_reg[top].apb_spare2);
        }
    }

    get_fc_sm();
    flags = disable_irq_fiq();
    //Now we get the control of fc_sm, we can start to do the FC from here

    //+++Step1: increase the voltages if it's raise PP using the GPIO/DVC mechanism
    if(cr5_value[top].dvc > cr5_value[c_cop].dvc)
    {
      update_voltage_dvcl2h(1, top);         
    }    
    //+++Step1-1: change the L1/L2 XTC values
    if(cr5_value[top].pclk > cr5_value[c_cop].pclk)
    {
      update_memory_xtc(cr5_value[top].pclk);
    }
    //+++Step2: use intermediate PP to avoid MFC if necessary
    /*
    Note:PMUA_AP_IMR[3]/PMUA_AP_ISR[3] is only valid for fc_sm FC, not valid for quick dynamic FC.
    We don't need it at this case because there are no hankshake between core and PMU,
    core will go on after finish and no need to wait here.
    */

    //+++Step4~12 the actual DFC process.
    cdfc_set(top);

    //+++Step13: Disable PLL2 if not in use by SW control method.
    //We assume only CA7 and CR5+DDR will use PLL2
    fccr = PMUM->FCCR;
    if(((fccr & BIT_30)==0)&&(((fccr & BIT_25)!=1)||((fccr & BIT_19) !=0))&&
       ((fccr & BIT_27)==0)) {
        sw_turnoff_pll2();
    }

    //+++Step14-pre: change the L1/L2 XTC values
    if(cr5_value[top].pclk < cr5_value[c_cop].pclk)
    {
      update_memory_xtc(cr5_value[top].pclk);
    }

    if(cr5_value[top].dvc < cr5_value[c_cop].dvc)
    {
      update_voltage_dvch2l(1, top); 
    }
    //+++Step15(opt):clear of RD_ST to indicate APP FC is done
    //This bit and the FC request bits should have already been cleared by hardware at this time,
    //Here we just do it to make sure
    put_fc_sm();

    restore_irq_fiq(flags);

    //+++Step16: Get current OP and compare with Target OP, if not equal, then fails.
    //compare and cpp update inside
    return (check_cop(top));
}

#endif

/*Must make sure there is only one entity of this function is in executing at any time,
  better to be in critial section, return value is OP_INVALID or MV_OK*/
uint32_t axidfc(uint32_t top)
{
    uint32_t flags;
    axi_freq temp;

    c_axiop = axiop_get_dump(&temp); //we support both AP, CP to change aclk, so need get c_axiop in real time

    if(top == c_axiop)
    {
        bbu_printf("ACLK is already at OP %d!\r\n", c_axiop);
        dump_cur_axiop();
        return c_axiop;
    }

    bbu_printf("AXI OP changing from %d to %d \r\n", c_axiop, top);

    /*In decoupled DFC solution, we should predefine the freq values of PLLs, NO MFC here!
    Just need to check the PLL2 is needed or not. We assume 0 is OFF state of PLL.
    ACLK will not use PLL2 clock in the OPs defined
    */
    get_fc_sm();
    flags = disable_irq_fiq();
    //Now we get the control of fc_sm, we can start to do the FC from here

    //+++Step1: increase the voltages if it's raise PP using the GPIO/DVC mechanism
    if(aclk_value[top].dvc > aclk_value[c_axiop].dvc)update_voltage_dvcl2h(2, top);

    //+++Step4~12 the actual DFC process.
    axidfc_set(top);

   if(aclk_value[top].dvc < aclk_value[c_axiop].dvc)update_voltage_dvch2l(2, top);

    //+++Step15(opt):clear of RD_ST to indicate APP FC is done
    //This bit and the FC request bits should have already been cleared by hardware at this time,
    //Here we just do it to make sure
    put_fc_sm();

    restore_irq_fiq(flags);

    //+++Step16: Get current OP and compare with Target OP, if not equal, then fails.
    //compare and cpp update inside
    return (check_axiop(top));
}

#ifdef CONFIG_CA7
/*Must make sure there is only one entity of this function is in executing at any time,
  better to be in critial section, return value is OP_INVALID or MV_OK*/
void csdfc(uint32_t top)
{
    uint32_t flags;
/*Coresight freq change use its own state-machine, not shared with the Main fsm*/
    bbu_printf("Coresight OP changing\r\n");

    flags = disable_irq_fiq();
    PMUA->TRACE_CONFIG &= ~0x003E07C7;
    PMUA->TRACE_CONFIG |= cs_reg[top].trace_config;
    if(cs_value[top].dvc > cs_value[c_csop].dvc)update_voltage_dvcl2h(3, top);    
    PMUA->TRACE_CONFIG |= 0x00008020;
    if(cs_value[top].dvc < cs_value[c_csop].dvc) update_voltage_dvch2l(3, top);    
    restore_irq_fiq(flags);
    bbu_printf("Coresight OP changed to OP%d\r\n", top);
    c_csop = top;
}
#endif

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

#ifdef CONFIG_CA7
/*PP change test function
  test_mode:  if it's < PP_NUM, then it's just one PP change test, test_mode is the target PP number.
  if test_mode > PP_NUM, we have some special test modes, test_count limits the PP change count number*/
uint32_t aop_test(uint32_t argc, uint8_t *argv[])
{
    uint32_t i = 0, pp = 0;
    uint32_t test_mode, test_count, pp1, pp2;

    (void)argc;

    if((strcmp((const char*)argv[2], "/c") == 0) ||
       (strcmp((const char*)argv[2], "--check") == 0)){
         dump_cur_aop();
         return MV_OK;
    }

    test_mode = XpStrToValue((char*)argv[2]);

    if(test_mode < aop_num)
    {
        adfc(test_mode);
        return MV_OK;
    }

    //otherwise, it's for specific test
    test_count = XpStrToValue((char*)argv[3]);

    switch (test_mode) {

    case 100:  //random stress test
        for(i=0; i<test_count; i++)
        {
            pp = random(aop_num);
            adfc(pp);
        }
        break;

    case 200:  //sequence stress test
        for(i=0; i<test_count; i++)
        {
            uint32_t j=0;

            for (j=0;j<aop_num;j++) {
                    adfc(aop_num-1-j);
                }

            for (j=0;j<aop_num;j++) {
                    adfc(j);
                }
        }
        break;

    case 300:  // back to back stress test between two PPs.
        pp1 = XpStrToValue((char*)argv[4]);
        pp2 = XpStrToValue((char*)argv[5]);
        for(i=0; i<test_count; i++) {
            adfc(pp1);
            adfc(pp2);
        }
        break;
    default :
        bbu_printf("\r\nunsupported CA7 OP test! \r\n");
        break;
    }

    return MV_OK;
}
#endif

#ifdef CONFIG_CR5
uint32_t cop_test(uint32_t argc, uint8_t *argv[])
{
    uint32_t i = 0, pp = 0;
    uint32_t test_mode, test_count, pp1, pp2;

    (void)argc;

    if((strcmp((const char*)argv[2], "/c") == 0) ||
       (strcmp((const char*)argv[2], "--check") == 0)){
         dump_cur_cop();
         return MV_OK;
    }

    test_mode = XpStrToValue((char*)argv[2]);

    if(test_mode < cop_num)
    {
        cdfc(test_mode);
        return MV_OK;
    }

    //otherwise, it's for specific test
    test_count = XpStrToValue((char*)argv[3]);

    switch (test_mode) {

    case 100:  //random stress test
        for(i=0; i<test_count; i++)
        {
            pp = random(cop_num);
            cdfc(pp);
        }
        break;

    case 200:  //sequence stress test
        for(i=0; i<test_count; i++)
        {
            uint32_t j=0;

            for (j=0;j<cop_num;j++) {
                    cdfc(cop_num-1-j);
                }

            for (j=0;j<cop_num;j++) {
                    cdfc(j);
                }
        }
        break;

    case 300:  // back to back stress test between two PPs.
        pp1 = XpStrToValue((char*)argv[4]);
        pp2 = XpStrToValue((char*)argv[5]);
        for(i=0; i<test_count; i++) {
            cdfc(pp1);
            cdfc(pp2);
        }
        break;
    default :
        bbu_printf("\r\nunsupported CR5 OP test! \r\n");
        break;
    }

    return MV_OK;
}
#endif

uint32_t axiop_test(uint32_t argc, uint8_t *argv[])
{
    uint32_t i = 0, pp = 0;
    uint32_t test_mode, test_count, pp1, pp2;

    (void)argc;

    if((strcmp((const char*)argv[2], "/c") == 0) ||
       (strcmp((const char*)argv[2], "--check") == 0)){
         dump_cur_axiop();
         return MV_OK;
    }

    test_mode = XpStrToValue((char*)argv[2]);

#ifdef CONFIG_CR5
    bbu_printf("Warning: AXI DFC should be done be AP side, it's DVL is handled by AP\r\n");
#endif

    if(test_mode < axiop_num)
    {
        axidfc(test_mode);
        return MV_OK;
    }

    //otherwise, it's for specific test
    test_count = XpStrToValue((char*)argv[3]);

    switch (test_mode) {

    case 100:  //random stress test
        for(i=0; i<test_count; i++)
        {
            pp = random(axiop_num);
            axidfc(pp);
        }
        break;

    case 200:  //sequence stress test
        for(i=0; i<test_count; i++)
        {
            uint32_t j=0;

            for (j=0;j<axiop_num;j++) {
                    axidfc(axiop_num-1-j);
                }

            for (j=0;j<axiop_num;j++) {
                    axidfc(j);
                }
        }
        break;

    case 300:  // back to back stress test between two PPs.
        pp1 = XpStrToValue((char*)argv[4]);
        pp2 = XpStrToValue((char*)argv[5]);
        for(i=0; i<test_count; i++) {
            axidfc(pp1);
            axidfc(pp2);
        }
        break;
    default :
        bbu_printf("\r\nunsupported AXI OP test! \r\n");
        break;
    }

    return MV_OK;
}

#ifdef CONFIG_CA7
uint32_t csop_test(uint32_t argc, uint8_t *argv[])
{
    uint32_t test_mode;

    (void)argc;

    if((strcmp((const char*)argv[2], "/c") == 0) ||
       (strcmp((const char*)argv[2], "--check") == 0)){
        bbu_printf("The current Coresight at OP%d, atclk = %d, dbg_clk= %d, trace_clk = %d\r\n",
                    c_csop, cs_value[c_csop].atclk, cs_value[c_csop].dbg_clk, cs_value[c_csop].trace_clk);
         return MV_OK;
    }

    test_mode = XpStrToValue((char*)argv[2]);

    if(test_mode <csop_num)
    {
        csdfc(test_mode);
        return MV_OK;
    }
    else
    {
        bbu_printf("unsupported coresight OP\r\n");
    }

    return MV_OK;
}
#endif

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

    BBU_REG_DUMP("FC_TIMER           ", (uint32_t*)&PMUA->FC_TIMER           , PMUA->FC_TIMER           );
    BBU_REG_DUMP("CP_IDLE_CFG        ", (uint32_t*)&PMUA->CP_IDLE_CFG        , PMUA->CP_IDLE_CFG        );
    BBU_REG_DUMP("AP_IDLE_CFG        ", (uint32_t*)&PMUA->AP_IDLE_CFG        , PMUA->AP_IDLE_CFG        );
    BBU_REG_DUMP("SQU_CLK_GATE_CTRL  ", (uint32_t*)&PMUA->SQU_CLK_GATE_CTRL  , PMUA->SQU_CLK_GATE_CTRL  );
    BBU_REG_DUMP("IRE_CLK_GATE_CTRL  ", (uint32_t*)&PMUA->IRE_CLK_GATE_CTRL  , PMUA->IRE_CLK_GATE_CTRL  );
    BBU_REG_DUMP("CCIC_CLK_GATE_CTRL ", (uint32_t*)&PMUA->CCIC_CLK_GATE_CTRL , PMUA->CCIC_CLK_GATE_CTRL );
    BBU_REG_DUMP("FBRC0_CLK_GATE_CTRL", (uint32_t*)&PMUA->FBRC0_CLK_GATE_CTRL, PMUA->FBRC0_CLK_GATE_CTRL);
    BBU_REG_DUMP("FBRC1_CLK_GATE_CTRL", (uint32_t*)&PMUA->FBRC1_CLK_GATE_CTRL, PMUA->FBRC1_CLK_GATE_CTRL);
    BBU_REG_DUMP("USB_CLK_GATE_CTRL  ", (uint32_t*)&PMUA->USB_CLK_GATE_CTRL  , PMUA->USB_CLK_GATE_CTRL  );
    //BBU_REG_DUMP("ISP_CLK_RES_CTRL   ", (uint32_t*)&PMUA->ISP_CLK_RES_CTRL   , PMUA->ISP_CLK_RES_CTRL   );
    BBU_REG_DUMP("PMU_CLK_GATE_CTRL  ", (uint32_t*)&PMUA->PMU_CLK_GATE_CTRL  , PMUA->PMU_CLK_GATE_CTRL  );
    //BBU_REG_DUMP("DSI_CLK_RES_CTRL   ", (uint32_t*)&PMUA->DSI_CLK_RES_CTRL   , PMUA->DSI_CLK_RES_CTRL   );
    BBU_REG_DUMP("HSI_CLK_RES_CTRL   ", (uint32_t*)&PMUA->HSI_CLK_RES_CTRL   , PMUA->HSI_CLK_RES_CTRL   );
    //BBU_REG_DUMP("LCD_CLK_RES_CTRL   ", (uint32_t*)&PMUA->LCD_CLK_RES_CTRL   , PMUA->LCD_CLK_RES_CTRL   );
    BBU_REG_DUMP("CCIC_CLK_RES_CTRL  ", (uint32_t*)&PMUA->CCIC_CLK_RES_CTRL  , PMUA->CCIC_CLK_RES_CTRL  );
    BBU_REG_DUMP("SDH0_CLK_RES_CTRL  ", (uint32_t*)&PMUA->SDH0_CLK_RES_CTRL  , PMUA->SDH0_CLK_RES_CTRL  );
    BBU_REG_DUMP("SDH1_CLK_RES_CTRL  ", (uint32_t*)&PMUA->SDH1_CLK_RES_CTRL  , PMUA->SDH1_CLK_RES_CTRL  );
    BBU_REG_DUMP("USB_CLK_RES_CTRL   ", (uint32_t*)&PMUA->USB_CLK_RES_CTRL   , PMUA->USB_CLK_RES_CTRL   );
    BBU_REG_DUMP("QSPI_CLK_RES_CTRL    ", (uint32_t*)&PMUA->QSPI_CLK_RES_CTRL    , PMUA->QSPI_CLK_RES_CTRL    );
    BBU_REG_DUMP("DMA_CLK_RES_CTRL   ", (uint32_t*)&PMUA->DMA_CLK_RES_CTRL   , PMUA->DMA_CLK_RES_CTRL   );
    BBU_REG_DUMP("AES_CLK_RES_CTRL   ", (uint32_t*)&PMUA->AES_CLK_RES_CTRL   , PMUA->AES_CLK_RES_CTRL   );
    BBU_REG_DUMP("MCB_CLK_RES_CTRL   ", (uint32_t*)&PMUA->MCB_CLK_RES_CTRL   , PMUA->MCB_CLK_RES_CTRL   );
    BBU_REG_DUMP("PMU_CP_IMR         ", (uint32_t*)&PMUA->CP_IMR             , PMUA->CP_IMR         );
    BBU_REG_DUMP("PMU_CP_IRWC        ", (uint32_t*)&PMUA->CP_IRWC            , PMUA->CP_IRWC        );
    BBU_REG_DUMP("PMU_CP_ISR         ", (uint32_t*)&PMUA->CP_ISR             , PMUA->CP_ISR         );
    BBU_REG_DUMP("SD_ROT_WAKE_CLR    ", (uint32_t*)&PMUA->SD_ROT_WAKE_CLR    , PMUA->SD_ROT_WAKE_CLR    );
    BBU_REG_DUMP("PMU_FBRC_CLK       ", (uint32_t*)&PMUA->PMU_FBRC_CLK       , PMUA->PMU_FBRC_CLK       );
    BBU_REG_DUMP("PWR_STBL_TIMER     ", (uint32_t*)&PMUA->PWR_STBL_TIMER     , PMUA->PWR_STBL_TIMER     );
    BBU_REG_DUMP("DEBUG_REG          ", (uint32_t*)&PMUA->DEBUG_REG          , PMUA->DEBUG_REG          );
    BBU_REG_DUMP("SRAM_PWR_DWN       ", (uint32_t*)&PMUA->SRAM_PWR_DWN       , PMUA->SRAM_PWR_DWN       );
    BBU_REG_DUMP("CORE_STATUS        ", (uint32_t*)&PMUA->CORE_STATUS        , PMUA->CORE_STATUS        );
    BBU_REG_DUMP("RES_FRM_SLP_CLR    ", (uint32_t*)&PMUA->RES_FRM_SLP_CLR    , PMUA->RES_FRM_SLP_CLR    );
    BBU_REG_DUMP("AP_IMR             ", (uint32_t*)&PMUA->AP_IMR             , PMUA->AP_IMR             );
    BBU_REG_DUMP("AP_IRWC            ", (uint32_t*)&PMUA->AP_IRWC            , PMUA->AP_IRWC            );
    BBU_REG_DUMP("AP_ISR             ", (uint32_t*)&PMUA->AP_ISR             , PMUA->AP_ISR             );
    //BBU_REG_DUMP("VPU_CLK_RES_CTRL   ", (uint32_t*)&PMUA->VPU_CLK_RES_CTRL   , PMUA->VPU_CLK_RES_CTRL   );
    BBU_REG_DUMP("DTC_CLK_RES_CTRL   ", (uint32_t*)&PMUA->DTC_CLK_RES_CTRL   , PMUA->DTC_CLK_RES_CTRL   );
    BBU_REG_DUMP("MC_HW_SLP_TYPE     ", (uint32_t*)&PMUA->MC_HW_SLP_TYPE     , PMUA->MC_HW_SLP_TYPE     );
    BBU_REG_DUMP("MC_SLP_REQ_AP      ", (uint32_t*)&PMUA->MC_SLP_REQ_AP      , PMUA->MC_SLP_REQ_AP      );
    BBU_REG_DUMP("MC_SLP_REQ_CP      ", (uint32_t*)&PMUA->MC_SLP_REQ_CP      , PMUA->MC_SLP_REQ_CP      );
    BBU_REG_DUMP("MC_SLP_REQ_MSA     ", (uint32_t*)&PMUA->MC_SLP_REQ_MSA     , PMUA->MC_SLP_REQ_MSA     );
    BBU_REG_DUMP("MC_MC_SLP_TYPE     ", (uint32_t*)&PMUA->MC_MC_SLP_TYPE     , PMUA->MC_MC_SLP_TYPE     );
    BBU_REG_DUMP("PLL_SEL_STATUS     ", (uint32_t*)&PMUA->PLL_SEL_STATUS     , PMUA->PLL_SEL_STATUS     );
    BBU_REG_DUMP("SYNC_MODE_BYPASS   ", (uint32_t*)&PMUA->SYNC_MODE_BYPASS   , PMUA->SYNC_MODE_BYPASS   );
    BBU_REG_DUMP("GC_CLK_RES_CTRL    ", (uint32_t*)&PMUA->GC_CLK_RES_CTRL    , PMUA->GC_CLK_RES_CTRL    );
    BBU_REG_DUMP("SMC_CLK_RES_CTRL   ", (uint32_t*)&PMUA->SMC_CLK_RES_CTRL   , PMUA->SMC_CLK_RES_CTRL   );
    BBU_REG_DUMP("PWR_CTRL_REG       ", (uint32_t*)&PMUA->PWR_CTRL_REG       , PMUA->PWR_CTRL_REG       );
    BBU_REG_DUMP("PWR_BLK_TMR_REG    ", (uint32_t*)&PMUA->PWR_BLK_TMR_REG    , PMUA->PWR_BLK_TMR_REG    );
    BBU_REG_DUMP("SDH2_CLK_RES_CTRL  ", (uint32_t*)&PMUA->SDH2_CLK_RES_CTRL  , PMUA->SDH2_CLK_RES_CTRL  );
    //BBU_REG_DUMP("CA7MP_IDLE_CFG1    ", (uint32_t*)&PMUA->CA7MP_IDLE_CFG1    , PMUA->CA7MP_IDLE_CFG1    );
    BBU_REG_DUMP("MCK4_CTRL          ", (uint32_t*)&PMUA->MCK4_CTRL          , PMUA->MCK4_CTRL          );
    BBU_REG_DUMP("PWR_STATUS_REG     ", (uint32_t*)&PMUA->PWR_STATUS_REG     , PMUA->PWR_STATUS_REG     );
    BBU_REG_DUMP("CC2_AP             ", (uint32_t*)&PMUA->CC2_AP             , PMUA->CC2_AP             );
    //BBU_REG_DUMP("DM_CC2_AP          ", (uint32_t*)&PMUA->DM_CC2_AP          , PMUA->DM_CC2_AP          );
    BBU_REG_DUMP("TRACE_CONFIG       ", (uint32_t*)&PMUA->TRACE_CONFIG       , PMUA->TRACE_CONFIG       );
    BBU_REG_DUMP("CA7MP_IDLE_CFG0    ", (uint32_t*)&PMUA->CA7MP_IDLE_CFG0    , PMUA->CA7MP_IDLE_CFG0    );
    BBU_REG_DUMP("CA7_CORE0_IDLE_CFG ", (uint32_t*)&PMUA->CA7_CORE0_IDLE_CFG , PMUA->CA7_CORE0_IDLE_CFG );
    //BBU_REG_DUMP("CA7_CORE1_IDLE_CFG ", (uint32_t*)&PMUA->CA7_CORE1_IDLE_CFG , PMUA->CA7_CORE1_IDLE_CFG );
    //BBU_REG_DUMP("CA7_CORE2_IDLE_CFG ", (uint32_t*)&PMUA->CA7_CORE2_IDLE_CFG , PMUA->CA7_CORE2_IDLE_CFG );
    //BBU_REG_DUMP("CA7_CORE3_IDLE_CFG ", (uint32_t*)&PMUA->CA7_CORE3_IDLE_CFG , PMUA->CA7_CORE3_IDLE_CFG );
    //BBU_REG_DUMP("CA7_CORE_WAKEUP[0] ", (uint32_t*)&PMUA->CA7_CORE_WAKEUP[0] , PMUA->CA7_CORE_WAKEUP[0] );
    //BBU_REG_DUMP("CA7_CORE_WAKEUP[1] ", (uint32_t*)&PMUA->CA7_CORE_WAKEUP[1] , PMUA->CA7_CORE_WAKEUP[1] );
    //BBU_REG_DUMP("CA7_CORE_WAKEUP[2] ", (uint32_t*)&PMUA->CA7_CORE_WAKEUP[2] , PMUA->CA7_CORE_WAKEUP[2] );
    //BBU_REG_DUMP("CA7_CORE_WAKEUP[3] ", (uint32_t*)&PMUA->CA7_CORE_WAKEUP[3] , PMUA->CA7_CORE_WAKEUP[3] );
    BBU_REG_DUMP("DVC_DEBUG          ", (uint32_t*)&PMUA->DVC_DEBUG          , PMUA->DVC_DEBUG          );
    //BBU_REG_DUMP("CA7MP_IDLE_CFG2 	 ", (uint32_t*)&PMUA->CA7MP_IDLE_CFG2    , PMUA->CA7MP_IDLE_CFG2    );
    //BBU_REG_DUMP("CA7MP_IDLE_CFG3    ", (uint32_t*)&PMUA->CA7MP_IDLE_CFG3    , PMUA->CA7MP_IDLE_CFG3    );
    bbu_printf("*************Dragon : APP PMU Register Dump End ************\r\n");
    return;
}



void dump_main_pmureg()
{
    bbu_printf("*************Dragon : MAIN PMU Register Dump Begin ************\r\n");
    BBU_REG_DUMP("CPCR     ", (uint32_t*)&PMUM->CPCR     , PMUM->CPCR     );
    BBU_REG_DUMP("CPSR     ", (uint32_t*)&PMUM->CPSR     , PMUM->CPSR     );
    BBU_REG_DUMP("FCCR     ", (uint32_t*)&PMUM->FCCR     , PMUM->FCCR     );
    BBU_REG_DUMP("POCR     ", (uint32_t*)&PMUM->POCR     , PMUM->POCR     );
    BBU_REG_DUMP("POSR     ", (uint32_t*)&PMUM->POSR     , PMUM->POSR     );
    BBU_REG_DUMP("SUCCR    ", (uint32_t*)&PMUM->SUCCR    , PMUM->SUCCR    );
    BBU_REG_DUMP("VRCR     ", (uint32_t*)&PMUM->VRCR     , PMUM->VRCR     );
//    BBU_REG_DUMP("PLL3CR   ", (uint32_t*)&PMUM->PLL3CR   , PMUM->PLL3CR   );
    BBU_REG_DUMP("CPRR     ", (uint32_t*)&PMUM->CPRR     , PMUM->CPRR     );
    BBU_REG_DUMP("CCGR     ", (uint32_t*)&PMUM->CCGR     , PMUM->CCGR     );
    BBU_REG_DUMP("CRSR     ", (uint32_t*)&PMUM->CRSR     , PMUM->CRSR     );
    BBU_REG_DUMP("XDCR     ", (uint32_t*)&PMUM->XDCR     , PMUM->XDCR     );
    BBU_REG_DUMP("GPCR     ", (uint32_t*)&PMUM->GPCR     , PMUM->GPCR     );
    BBU_REG_DUMP("PLL2CR   ", (uint32_t*)&PMUM->PLL2CR   , PMUM->PLL2CR   );
    BBU_REG_DUMP("SCCR     ", (uint32_t*)&PMUM->SCCR     , PMUM->SCCR     );
    BBU_REG_DUMP("MCCR     ", (uint32_t*)&PMUM->MCCR     , PMUM->MCCR     );
    BBU_REG_DUMP("ISCCRX[0]", (uint32_t*)&PMUM->ISCCRX[0], PMUM->ISCCRX[0]);
    BBU_REG_DUMP("ISCCRX[1]", (uint32_t*)&PMUM->ISCCRX[1], PMUM->ISCCRX[1]);
    BBU_REG_DUMP("CWUCRS   ", (uint32_t*)&PMUM->CWUCRS   , PMUM->CWUCRS   );
    BBU_REG_DUMP("CWUCRM   ", (uint32_t*)&PMUM->CWUCRM   , PMUM->CWUCRM   );
    BBU_REG_DUMP("DSOC     ", (uint32_t*)&PMUM->DSOC     , PMUM->DSOC     );
    BBU_REG_DUMP("WDTPCR   ", (uint32_t*)&PMUM->WDTPCR   , PMUM->WDTPCR   );
    BBU_REG_DUMP("CMPRX[0] ", (uint32_t*)&PMUM->CMPRX[0] , PMUM->CMPRX[0] );
    BBU_REG_DUMP("CMPRX[1] ", (uint32_t*)&PMUM->CMPRX[1] , PMUM->CMPRX[1] );
    BBU_REG_DUMP("CMPRX[2] ", (uint32_t*)&PMUM->CMPRX[2] , PMUM->CMPRX[2] );
    BBU_REG_DUMP("CMPRX[3] ", (uint32_t*)&PMUM->CMPRX[3] , PMUM->CMPRX[3] );
    BBU_REG_DUMP("CMPRX[4] ", (uint32_t*)&PMUM->CMPRX[4] , PMUM->CMPRX[4] );
    BBU_REG_DUMP("APCR     ", (uint32_t*)&PMUM->APCR     , PMUM->APCR     );
    BBU_REG_DUMP("APSR     ", (uint32_t*)&PMUM->APSR     , PMUM->APSR     );
    BBU_REG_DUMP("APRR     ", (uint32_t*)&PMUM->APRR     , PMUM->APRR     );
    BBU_REG_DUMP("ACGR     ", (uint32_t*)&PMUM->ACGR     , PMUM->ACGR     );
    BBU_REG_DUMP("ARSR     ", (uint32_t*)&PMUM->ARSR     , PMUM->ARSR     );
    BBU_REG_DUMP("AWUCRS   ", (uint32_t*)&PMUM->AWUCRS   , PMUM->AWUCRS   );
    BBU_REG_DUMP("AWUCRM   ", (uint32_t*)&PMUM->AWUCRM   , PMUM->AWUCRM   );
    bbu_printf("*************Dragon : MAIN PMU Register Dump End ************\r\n");
    return;
}

void dump_pll_ctrlreg()
{
    bbu_printf("*************Dragon : APB PLL Control Register Dump Begin ************\r\n");
    BBU_REG_DUMP("APB_SPARE3_REG     ", (uint32_t*)&APBSPARE->PLL2_SW_CTRL2, APBSPARE->PLL2_SW_CTRL2);      
    BBU_REG_DUMP("PLL2CR   ", (uint32_t*)&PMUM->PLL2CR   , PMUM->PLL2CR   );    
    BBU_REG_DUMP("APB_SPARE2_REG     ", (uint32_t*)&APBSPARE->PLL2_SW_CTRL, APBSPARE->PLL2_SW_CTRL);
    BBU_REG_DUMP("FCCR     ", (uint32_t*)&PMUM->FCCR     , PMUM->FCCR     );
    BBU_REG_DUMP("CC_AP              ", (uint32_t*)&PMUA->CC_AP              , PMUA->CC_AP              );   
    BBU_REG_DUMP("CC2_AP             ", (uint32_t*)&PMUA->CC2_AP             , PMUA->CC2_AP             );     
    BBU_REG_DUMP("CC_CP              ", (uint32_t*)&PMUA->CC_CP              , PMUA->CC_CP              );  
    BBU_REG_DUMP("MCCR     ", (uint32_t*)&PMUM->MCCR     , PMUM->MCCR     );      
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
int bbu_ppset_test(int argc, uint8_t **argv){
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
#ifdef CONFIG_CA7
                dump_aop_list();
                dump_axiop_list();                
#endif
#ifdef CONFIG_CR5
                dump_cop_list();
#endif
            }
            else if(check_reg(argv[1])){
                dump_pmu_reg();
            }
            else{

                if((strcmp((const char*)argv[1], "core") == 0))
                {
#ifdef CONFIG_CA7
                    if(MV_OK == aop_test(argc, argv))              //if pp_test success, send the ack to screen test hanlder via AO_cli
                    {
                        return 1;
                        //screen_cmd_ack(1, scr_cmd_index);
                    }
#endif
#ifdef CONFIG_CR5
                    if(MV_OK == cop_test(argc, argv))
                //if pp_test success, send the ack to screen test hanlder via AO_cli
                    {
                        return 1;
                        //screen_cmd_ack(1, scr_cmd_index);
                    }
#endif
                }
                if((strcmp((const char*)argv[1], "axi") == 0))
                {
                    if(MV_OK == axiop_test(argc, argv))
                //if pp_test success, send the ack to screen test hanlder via AO_cli
                    {
                        return 1;
                        //screen_cmd_ack(1, scr_cmd_index);
                    }
                }
#ifdef CONFIG_CA7                
                if((strcmp((const char*)argv[1], "cs") == 0))
                {
                    if(MV_OK == csop_test(argc, argv))
                //if pp_test success, send the ack to screen test hanlder via AO_cli
                    {
                        return 1;
                        //screen_cmd_ack(1, scr_cmd_index);
                    }
                }
#endif                
            }
            break;
        }
    }
    return 0;
}
