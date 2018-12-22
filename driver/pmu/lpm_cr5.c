/* ========================================================================== */
/*                                                                            */
/*   lpm_udr.c                                                                */
/*   initial version by xlianghu@marvell.com                                  */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#include "bbu.h"
#include "ppset.h"
#include "PMUA.h"
#include "PMUM.h"
#include "pmic.h"
#include "timer_dec.h"
#include "CIU.h"
#include "SDH.h"
#include "pmu_dec.h"
#include "ICU.h"
#include "msabin.h"
#include "common_tests.h"
#include "interrupt.h"
#include "cache_ops.h"
#include "APBCLK.h"
#include "IPC.h"
#include "ddr_power.h"
#include "ate.h"
#include "APBSPARE.h"
#include "LCD.h"
#include "PMU_DVC.h"

////*****************************NOTE*******************************************/
//ALL Pheriperials Power optimizations are put in AP side lpm code.
//CP side just control its own cores and modem related settings.
/********************************************************************************/

/*mode:
  1: put Seagull/MSA into D2;
  0/2: put Seagull/MSA into while loop;  0/2 is same for MSA now
  For Seagull: 0: loop in TCM, 1: wfi (in TCM actually)  2: loop in DDR
  Both start from DDR.
  */

uint32_t  cp_boot_test = 0;
uint32_t  msa_boot_test = 0;
uint32_t  digrf3_ext_loop_test = 0;
uint32_t  td_crc_test = 0;
uint32_t  gsm_eq_test = 0;

extern int ate_timer;

#ifdef CONFIG_DVC
//pp1: 104MHz; pp2: 208MHz; pp3: 416MHz; pp4: PLL2_OUT/2 MHz; pp5: 624MHz
uint32_t  msa_dvl[5] = {0, 0, 0, 1, 1}; //the DVL need to be tuned later
#endif   

/*MSA L2 SRAM latency, L1 RTC, mask for  pmu2gb_wakeup signal*/
#define AIB_spare0_reg       *(volatile uint32_t *)0xd401e7f8
#define AIB_spare1_reg       *(volatile uint32_t *)0xd401e7fc

static void msa_test(uint32_t dsp_mode, uint32_t index, uint32_t pp_index)
{
    uint32_t  dummy_read_apb = 0;

    msa_boot_test = 0;
    td_crc_test = 0;
    gsm_eq_test = 0;

    //For MSA part
    msa_mode = dsp_mode; //mode 1/3: go to D2, 0: while loop 2: while loop but unvote to wake up DDR and AXI when M2 and deeper
    msa_index = index; //msa test 0:boot 1: DigRF3, 2: TD CRC unit 3: Seagull APB/SQU/GSRAM R/W test for Vmax issue
    msa_boot_ind = 1; //test flag set to 1, MSA will set it to 0 after finish loopback for digiRF3G.
    crc_result = 0xdeadbeef; //to avoid reset value confusion
    gsm_eq_result = 0xdeadbeef; //to avoid reset value confusion
    ddr_axi_wake_result = 0xdeadbeef; //to avoid rest value confusion

    if (1 == (msa_mode & BIT_0))
    {
        bbu_printf(MP3_PWR_LOG"put MSA into D2 state now...\r\n");
    }
    else
    {
        // bbu_printf(MP3_PWR_LOG"put MSA into while loop state now...\r\n");
    }
    *(volatile uint32_t *)(0xd4200200) = (((uint32_t)(&msa_d2_ops[0])) & 0xFF000000) >> 24;
    *(volatile uint32_t *)(0xd4070000) = 0xd0000000 + (((uint32_t)(&msa_d2_ops[0])) & 0xFFFFFF);
    AIB_spare1_reg = 0x49; //reset default is 0x49

    *(volatile uint32_t *)(0xd405003c) &= 0xfffffffc;
    dummy_read_apb = *(volatile uint32_t *)(0xd405003c);

    if ((PMUM->APSR) & BIT_31) //core is in C2
    {
        msa_mode = 0; //mode 1/3: go to D2, 0: while loop 2: while loop but unvote to wake up DDR and AXI when M2 and deeper
        msa_index = 0; //msa test 0:boot 1: DigRF3, 2: TD CRC unit 3: Seagull APB/SQU/GSRAM R/W test for Vmax issue
        while (!((PMUM->APSR) & BIT_28)) //until MSA SS is up
        {
            PMUM->CPCR &= ~BIT_14; //unvote MSASLPEN can wake up MSA & MSA subsys, it will reset at wake up
            dummy_read_apb = PMUM->CPCR;
            udelay(256);
            //make sure MSA has been wake up but not reach the idle instruction yet
            //if(msa_boot_ind == 0)bbu_printf("the delay is too much!\r\n");
        }
        //make sure MSA has been go into L2 SRAM loop
        timeout_mdelay(256, &msa_boot_ind, 0);
        //bbu_printf("address is &msa_boot_ind = %d\r\n", &msa_boot_ind);
        udelay(256);
        PMUM->CPCR |= BIT_14; //vote MSASLPEN again after wake up MSA and reset it.
        dummy_read_apb = PMUM->CPCR;
    }

    //above make sure msa is active and loop in L2 SRAM before silent reset, in Helan LTE that will be improved
    //we should not reset MSA if it has pending AXI transaction to APPs(SQU or DDR access).
    //Otherwise, AXI bus will go to deadlock and cause both AP/CP/DSP hangs.
    //10000 times test shows it's safe if MSA is looping on single DDR instruction although MSA cache is not enabled.
    //However, we can't make sure it's always safe, so let MSA running in L2 SRAM loop if it's active
    PMUM->CPRR |= 0x2C;
    dummy_read_apb = PMUM->CPRR;
    udelay(512); //500us
    msa_mode = dsp_mode; //mode 1/3: go to D2, 0: while loop 2: while loop but unvote to wake up DDR and AXI when M2 and deeper
    msa_index = index; //msa test 0:boot 1: DigRF3, 2: TD CRC unit 3: Seagull APB/SQU/GSRAM R/W test for Vmax issue
    msa_boot_ind = 1; //test flag set to 1, MSA will set it to 0 after finish loopback for digiRF3G.
    msa_pp = pp_index; //3; //pp1: 104MHz; pp2: 208MHz; pp3: 416MHz; pp4: PLL2_OUT/2 MHz; pp5: 624MHz
#ifdef CONFIG_DVC
    PMU_DVC->DVC_DP = (PMU_DVC->DVC_DP & (~(0x7 << 4))) | 0x88 | ((msa_dvl[(pp_index-1)]) << 4);
    bbu_printf("CP: PMU_DVC->DVC_DP= 0x%x\r\n", PMU_DVC->DVC_DP);    
#endif        
    PMUM->CPRR &= ~(1 << 3);
    dummy_read_apb = PMUM->CPRR;
    PMUM->CPRR &= ~(1 << 2);
    dummy_read_apb = PMUM->CPRR;
    udelay(32); //20us
    PMUM->CPRR &= ~(1 << 5);
    dummy_read_apb = PMUM->CPRR;

    //--mdelay(128);
    msa_boot_test = timeout_mdelay(256, &msa_boot_ind, 0); //make sure MSA side test have time to finish.

    if (msa_boot_ind == 0)
    {
        msa_boot_test = 1;
    }
    else
    {
        bbu_printf("MSA boot fails, timeout=%dmS\r\n", msa_boot_test);
        msa_boot_test = 0;
    }

    isb();

    if (0 == crc_result)
    {
        td_crc_test = 1;
        bbu_printf("TD CRC0 test PASS \r\n");
    }
    else
    {
        td_crc_test = 0;
        //bbu_printf("TD CRC0 test FAIL \r\n");
    }

    if (0 == gsm_eq_result)
    {
        gsm_eq_test = 1;
        bbu_printf("GSM equalizer test PASS \r\n");
    }
    else
    {
        gsm_eq_test = 0;
        //bbu_printf("TD CRC0 test FAIL \r\n");
    }

    if (1 == (msa_mode & BIT_0))
    {
        while (!((PMUM->APSR) & BIT_31)); //until D2 is really entered.
        udelay(32); //Need some wait to enter D2 if Seagull vote that already
    }
}


void comm_test(uint32_t dsp_mode, uint32_t index, uint32_t pp_index)
{
    //Note: set the voltage and applicable PP before call comm_test to test CP&MSA boot Vmin.
    //Mode 0xF means keep the current state and don't test
    if (0xF != dsp_mode)
        msa_test(dsp_mode, index, pp_index);
    return;
}


extern uint32_t *svc_init_table;

/*Note: This should run after pp_init which enabled PMIC DVC function!*/

void lpm_init(void)
{

    uint32_t readback;

    *(volatile uint32_t *)(0xD405000c) = 0x001F0500; //POCR
    readback = *(volatile uint32_t *)(0xD4050000);
    *(volatile uint32_t *)(0xD4050038) = 0x00000005; //SCCR, force BB sleep
    readback = *(volatile uint32_t *)(0xD4050000);

    PMUM->CPCR = 0x42424000; //disable port 5 (USB) and port 1(WB) and allow CP/MSA into lpm

#ifndef CONFIG_DVC
    pmic_vbuck_cfg(VBUCK_ON, VBUCK1, svc_init_table[DVC10], DVC00);
    pmic_vbuck_cfg(VBUCK_ON, VBUCK1, svc_init_table[DVC10], DVC01);
#endif 

#ifndef CONFIG_DP_ALONE
    comm_test(1, 0, 3); //put MSA side into D2 at init.
#endif

#ifndef CONFIG_DVC
    //After MSA is in D2, restore the DVC levels. 
    pmic_vbuck_cfg(VBUCK_ON, VBUCK1, svc_init_table[DVC00], DVC00);
    pmic_vbuck_cfg(VBUCK_ON, VBUCK1, svc_init_table[DVC01], DVC01);
#endif
}

static void int_mask(void)
{

#if 0 //Cut all route to CP interrupts so that CP won't wake up
    bbu_printf("put Seagull into D2 state and never wake up...\r\n");
    for (int i = 0; i < 96; i++)
    {
        ICU->INT_CFGx[i] &= ~BIT_5;
    }
#endif

//Mask interrupts through ICU global mask bits
    ICU->SEAGULL_GBL_IRQ_MSK |= (BIT_0 | BIT_1);
}

// lpmset mode <sram_off> <wake_src> <vctcxo_off> <udr> <time_sec>
//           arg[1] = low power mode level,
//              4 hiarchies : core/MP/Subsys/Chip
//             mode: 0-C11, 1-C12, 2-C13 (not for SMP mode), 3-C2x, 4-M11, 5-M2x, 6-AP idle,
//                   7-AP sleep, 8-Chip idle, 9-Chip Sleep
//           arg[2] = Cache on/off when idle
//                BIT0: L1  BIT1: L2, BIT3: SCU.  L1 and SCU should be power off in SMP mode
//              -1: off
//              -0: on
//           arg[3] = set wake source
//                 please check above(0x0~0xa);  3-keypad, 4-UART, 5-RTC, 9-SD card
//           arg[4] = VCTCXO
//              -1 :off
//              -0 :or not
//           arg[5] = UDR
//              -1 : UDR
//              -0 : non-UDR
//           arg[6] = sleep time for scheduled type sleep(Sec), not use for async interrupt type

//Note:There are two methods to controll the DDR lpm modes. One is SW method which use
//three vote registers PMUA_MC_SLP_REQ_AP/CP/MSA  and  PMU_MC_SW_SLP_TYPE. It can put
//DDR into lpm mode at any time. SW method can shutdown DDR event at active mode.
//The other is HW mode by PMU (depend on wfi instr) which will put DDR into PMU_MC_HW_SLP_TYPE.
//The SW mechanism can be disabled by PMU_CP_IDLE_CFG[21] and PMU_CA7MP_IDLE_CFGx[5] (all?) :
//-Disable the MC entry to idle mode using the MC sleep request bits.
//-MC will always enter into idle mode based on the hardware SM request.
//-<p>0 = enable the idle entry using SW register bits.
//-<p>1 = disable the idle entry using the SW register bits.
//The SW method seems to be a debug method.  Don’t use it at normal LPM procedure.
//PMU should always use the HW mode to control DDR lpm modes.
//Otherwise, the last core to vote DDR lpm should put its vote operation and wfi instr in non-DDR memory.

//Note: JTAG will keep SOC awake in coresight design according to Xikai, so need use SWD to debug lpm
//Note: this code should be independent on coreID and can ran on both Cores.
//Note: Partial chip idle state has many cases, it's the combination of three cores's low power states,
//and we just choose those typical ones.
//Note: MSA is take as a pheriperial in TTC series architecture, esp in the PMU state machine.
//MSA should run sleep and wake logic in L2 logic since AXI and DDR do not depends on MSA idle instr.
//Or it should be enter into D2/C2 before APPS (i.e. at least one of Seagull/Dragon cores has not issued wfi).
//Note: The AXI bus and DDR Idle state can only be entered if the Seagull and Dragon subsystem are
//at Low Power state and if the MSA votes to allow it.
//Note: Before Emei A0/B0 and Nezha A0, MSA can't wakeup AP_SLEEP with DDR_VOTE clear.
//It need wake up AP_SLEEP by IPC interrupt.

/**************************IMPORTANT NOTE**************************/
/*
We have five levels lpm modes:
-Core internal (no relation to any level PMU state machine, C11 and C12)
-C13 and C2x
(C13 will trigger C level PMU but still in SMP cluster, and may have wake up unknown
by the PMU state machine, causing APMU_CORE_STATUS inaccurate and PMU state machine
out of order for further work, see JIRA EMEI-104.  SW need to clean and invalidate
L1 cache before C13 entry and remove itself from SMP domain.
Its penalty is close to C2 entry penalty.
In this aspect, C13 is not a very usable mode for SMP.
C2x will remove core from SMP domain by setting both SCU_CPSR power bit and FW bit in CP15 SMP ctrol reg.
-Mx level (both core in C13 or C2x, will trigger M level PMU state machine)
-AP level (will trigger AP level PMU SM)
-Chip level

We can view any power state as combination of Cx+Votes, and we only have
Cx state with different votes.

The last core to enter C2 will vote for other cores already in C2 to
get Mx and Dx and UDR.

Note: Mx stands for state with all cores in C2x.
C13 state has no use in current ARM architecture.
Other Mx can be get by combine Cx, dedicated routine is not necessary

In the future, we may fix the smp0 as the last one to enter Cx to simplify code.
And we may also consider maintain table based method to re-orgnize the code.
e.g. <mode, cpu, reg, reg_val>. May be not better than current method.
*/

extern int squ_dma_freed;

/*
*invoke power_save_peripherals only once in mp3 playback mode,
*this will save power,because lpmset is repeatedly invoked.
*/
static int power_save_peripherals_invoke_once = 0;

/*This function is shared by all the SMP cores*/
#ifndef CONFIG_CP_ALONE
void lpmset(uint32_t argc, uint8_t *argv[])
{
    uint32_t mode, sram_off, wake_src, vctcxo_off, udr;
    uint32_t cpu, wfi_mode, l1off, l2off, scu_off;
    uint32_t time_sec = 5;

    mode = XpStrToValue((char *)argv[1]);

    sram_off = 0;  //BIT0: L1  BIT1: L2

    PMUM->CWUCRM = 0x03000000;//mask all wake up sources except interrupts

    //===========Those two are both needed to mask wake up sources!!!==============
    //===========WB wake up source are active when WB module is not enable===========
    //===========Xikai said USB is also active at boot, but it seems not==============
    //===========if enable USB and BB, then their wake up source will not active========
    //*(volatile uint32_t *)(0xD4050000) = 0xFE0A6000; //CPCR

    //disable port 5 (USB) and port 1(WB) and allow MSA into lpm
    PMUM->CPCR = 0x42424000;
    PMUA->CP_IDLE_CFG = 0x00000230;
    PMUM->CPCR |= BIT_24;
    PMUM->CPCR &= ~BIT_24;

    /*
    core low power mode is controlled by PMU_CA7_CORE0/1_IDLE_CFG;
    Dragon MP-Subsystem low power mode is controlled by PMU_CA7MP_CORE_IDLE_CFGx;
    Each bit of the register takes effect only when both set.
    MP-SUB can not enter low power state until both core0 and core1 enter C13 or C2* mode.

    Before software executes WFI instruction, software should set ICU global mask bits and
    GIC global mask bits to guarantee the WFI sequence atomicity.
    These bits will automatically be cleared by hardware when falcon_cpu enters low power mode.

    GIC is used to wakeup cores.
    ICU is used to wakeup Dragon, so SW need to configure ICU routing the same as GIC.
    because GIC is off when Dragon is in idle or power-down.
    */
    switch (mode)
    {

    case  3  :   //Retention mode (C21) shutdown mode (C22)
        //cpu logic is powered off but cpu sram content is retained (C21).
        //both cpu logic and cpu sram are powered off
        if (argc > 2)
        {
            sram_off =  XpStrToValue((char *)argv[2]);
        }
        else
        {
            sram_off = 0;
        }
        l1off = (sram_off & BIT_0) ? BIT_2 : 0; //1:L1 SRAM power off, 0: L1 SRAM retain.
        l2off = (sram_off & BIT_1) ? BIT_3 : 0; //1:L2 SRAM power off, 0: L2 SRAM retain.

        PMUA->CP_IDLE_CFG  |= BIT_1 | l1off | l2off;
        //PMUM->CPCR |= BIT_28;
        PMUM->CPCR |= BIT_13;  //allows UDR, critical vote is up to AP now
        PMUM->CPCR |= BIT_19; //Allow VCTCXO shutdown, critical vote is up to AP now
        PMUM->CPCR |= BIT_26; //allows APB
        PMUM->CPCR |= BIT_27; //allows DDR
        PMUM->CPCR |= BIT_29; //allows system sleep
        PMUM->CPCR |= BIT_31;  //allows AXI_IDLE

#ifdef CONFIG_DVC
        //PMU_DVC->DVC_CP &= ~0x7;
        //PMU_DVC->DVC_CP |= (0x8 | cr5_value[c_cop].dvc);
        //We actually does not enable the C2 level request since DDR is active
        //and DDR is coupled with CP core
        //Let APSUB idle to take care the next level lpm related DVL
#endif        

    case  2  :   //External WFI Idle mode:(C13)
        //Clock is externally gated off with CORE_IDLE=1; CORE_PWRDWN=0
        //In C13, important that CPCR.STBEN bit should not be set in this mode
        PMUA->CP_IDLE_CFG  |= BIT_0;
        PMUM->CPCR |= BIT_28;
        int_mask();

    case  1  :   //Internal WFI Idle (C12)
    case  0  :  //don't test C11 (WFE) here since it's used for spinlock
#ifdef CONFIG_DEADIDLE_POWER_TEST
        //set CCGR to save power
        PMUM->CCGR = 0x2812;
#endif
        break;
    }
    bbu_printf("CP_IDLE_CFG: 0x%x CPCR: 0x%x\n\r", PMUA->CP_IDLE_CFG, PMUM->CPCR);
    /*NOTE:
      QP may have tick interrupt prevent Cx, so disable it.
      Interrupt(mask or unmasked) will force cpu exit WFI standby mode
      even if CPSR.I/F bit is set
    */
    unsigned int flags;
    flags = disable_irq_fiq();

#ifdef CONFIG_AUDIO
    if (wake_src == WAKE_SQU && squ_dma_freed)
    {
        // DO NOT enter lowpower when squ dma was freed.
        //also here to free the invoke once lock
        power_save_peripherals_invoke_once = 0;
    }
    else
#endif
    {
#ifdef  CONFIG_ATE
        stop_timer(ate_timer);
#endif
        if (mode > 2)
        {
            c2_save(sram_off);
        }
        else
        {
            l2_cache_sync();
            wfi();
        }

#ifdef  CONFIG_ATE
        restart_timer(ate_timer);
#endif
    }
    //pin-mux resume, different low power level, different configuration.

    //restore ACGR to default value
    PMUM->CCGR = 0xdffedffe;

    //Need to make sure wake up source interrupt is handled and cleaned.

    //After wake up, we do some resume ops
    PMUM->CPCR = 0x42424000; //reset APCR value to be active state.

    //This should be the last step after finish restore.
    //It can be woken up but will not interrupted before this step.
    //After this, core can enter ISR and we're ready for ISR.
    restore_irq_fiq(flags);
}
#else
//lpmset <mode> <sram> <wake_src> <vctcxo> <udr> <msec>
void lpmset(uint32_t argc, uint8_t *argv[])
{
    uint32_t mode, sram_off, wake_src, vctcxo_off, udr;
    uint32_t cpu, wfi_mode, l1off, l2off, scu_off;
    uint32_t time_sec = 5;

    mode = XpStrToValue((char *)argv[1]);

    sram_off = 0;  //BIT0: L1  BIT1: L2
#ifdef CONFIG_CP_ALONE
    PMUM->AWUCRM = 0x03000000;//mask all wake up sources except interrupts
    PMUM->APCR = 0x52424000; //disable port 5 (USB) and port 1(WB) and allow CP/MSA into lpm
#endif
    PMUM->CWUCRM = 0x03000000;//mask all wake up sources except interrupts

    //===========Those two are both needed to mask wake up sources!!!==============
    //===========WB wake up source are active when WB module is not enable===========
    //===========Xikai said USB is also active at boot, but it seems not==============
    //===========if enable USB and BB, then their wake up source will not active========
    //*(volatile uint32_t *)(0xD4050000) = 0xFE0A6000; //CPCR

    //disable port 5 (USB) and port 1(WB) and allow MSA into lpm
    PMUM->CPCR = 0x42424000;
    PMUA->CP_IDLE_CFG = 0;
    PMUM->CPCR |= BIT_24;
    PMUM->CPCR &= ~BIT_24;

    /*
    core low power mode is controlled by PMU_CA7_CORE0/1_IDLE_CFG;
    Dragon MP-Subsystem low power mode is controlled by PMU_CA7MP_CORE_IDLE_CFGx;
    Each bit of the register takes effect only when both set.
    MP-SUB can not enter low power state until both core0 and core1 enter C13 or C2* mode.

    Before software executes WFI instruction, software should set ICU global mask bits and
    GIC global mask bits to guarantee the WFI sequence atomicity.
    These bits will automatically be cleared by hardware when falcon_cpu enters low power mode.

    GIC is used to wakeup cores.
    ICU is used to wakeup Dragon, so SW need to configure ICU routing the same as GIC.
    because GIC is off when Dragon is in idle or power-down.
    */
    switch (mode)
    {
    case 9 : //Chip Sleep= CP_Sleep + APB down
    case 8 : //Chip Sleep= CP_Sleep + APB down
        //both AXI and DDR clocks are off
        PMUM->CPCR |= BIT_26;  //allow APB shutdown
        //Enable automatic voltage change  request to  DVL0 for UDR/non-UDR chip sleep with VCTCXO ON
        PMU_DVC->DVC_CHIP = 0x88;        
    case 7 : //CP_Sleep
        //Enable automatic voltage change request to DVL0  for AP SUB low power modes.
        //except AP idle with DDR ON mode may need higher voltage level, so left it to SW
        PMU_DVC->DVC_APSUB = 0x8880;
        PMUA->RES_FRM_SLP_CLR = 1; //enable the SYS_BOOT_CNTRL[14] bit
        while (CIU->SYS_BOOT_CNTRL & BIT_14);

        if (argc > 4)
        {
            vctcxo_off =  XpStrToValue((char *)argv[4]);
        }
        else
        {
            vctcxo_off = 0;
        }
        vctcxo_off = (vctcxo_off) ? BIT_19 : 0;
        if (argc > 5)
        {
            udr = XpStrToValue((char *)argv[5]);
        }
        else
        {
            udr = 0;
        }
        udr = (udr) ? BIT_13 : 0;

        PMUM->CPCR |= (BIT_29 | BIT_27 | vctcxo_off | udr); //disable USB
    case  6 : //CP subsys idle
        // CP IDLE, only AXI clock shutdown, DDR clock still on.
        PMUM->CPCR |= BIT_31 | BIT_24;  //Ax only need to vote AXI_IDLE
#ifdef CONFIG_DVC //HW-DVC
        PMU_DVC->DVC_APSUB |= 0x8;  
/*
Note: since AP wants to know the DDR OP and it's controlled by CP in Nezha2.
Let CP use the CIU->SW_SCRATCH register to  save the current DDR OP.
and AP read the register to get the OP
 */    
/*
Note: if it comes here and we want to use this state in real system (Nezha2 does not have 
home screen state), we need to wake up CP to do a DFC to low down the ddr clock to save power.
or AP can do it for CP since it's in C2 and  write the OP into CIU->SW_SCRATCH and let CP do 
c_cop = CIU->SW_SCRATCH after C2 exit? //TODO
 */
        //TODO
        c_cop = CIU->SW_SCRATCH;
        PMU_DVC->DVC_APSUB &= ~0x7;
        PMU_DVC->DVC_APSUB |= cr5_value[c_cop].dvc;  //Make sure DDR secure
#endif
        if (argc > 3)
        {
            wake_src =  XpStrToValue((char *)argv[3]);
        }
        else
        {
            wake_src = WAKE_SDH; //just use it as default, or should we enable ALL wake source?
        }

        //TODO: pin-mux, PMIC, buck slope may need save/restore for save power in lpm modes
        //TODO: IO_POWER OFF/ON for save power in lpm modes
        if(argc > 6)
            time_sec = XpStrToValue((char *)argv[6]);
        set_wake_src((e_wake_src)wake_src, time_sec); //for Full chip sleep wake up.

        /*Full chip sleep will be reached if
        Both CP and AP are in sleep mode.
        All votes for APB are ok.
        Full chip sleep will do:
        Shutdown PLL, and VCTCXO (optional), mp3 mode need VCTCXO on.
        Shutdown AP subsys power (if in UDR sleep mode).
        Notify PMIC to lower voltage (if VCTCXO is down).
         Full chip wakeup is by the 8 wakeup sources.

         Note: in current stage, we put CP&MSA into D2 always at boot up or in reset.
        */
        bbu_printf(MP3_PWR_LOG"lpmset: enter mode=%d with wake_src=%d, vctcxo_off=%d, udr=%d, time_sec=%d\r\n",
                   mode, wake_src, vctcxo_off >> 19, udr >> 13, time_sec);
        //bbu_printf("MFPR config value of GPIO91 D401E218 is 0x%x\r\n", BU_REG_READ(0xD401E218));



    case  5  :   //Retention Mode(M21) and Shutdown mode(M22)
#ifdef CONFIG_DVC //HW-DVC
        PMU_DVC->DVC_AP &=  ~0x7;
        PMU_DVC->DVC_AP |= (0x8|aclk_value[c_axiop].dvc);
        //Note: we can limit only AP can change AXI freq, 
        //if CP chang AXI, c_axiop may not correct
        //Note: CP C2 lpm DVL auto trigger is disabled so the DDR DVL is secure!
#endif        
    case 4 :  //External idle mode(M11),
        /*The last core should set MP_IDLE_CFG for ALL cores!*/

    case  3  :   //Retention mode (C21) shutdown mode (C22)
        //cpu logic is powered off but cpu sram content is retained (C21).
        //both cpu logic and cpu sram are powered off
        if (argc > 2)
        {
            sram_off =  XpStrToValue((char *)argv[2]);
        }
        else
        {
            sram_off = 0;
        }
        l1off = (sram_off & BIT_0) ? BIT_2 : 0; //1:L1 SRAM power off, 0: L1 SRAM retain.
        l2off = (sram_off & BIT_1) ? BIT_3 : 0; //1:L2 SRAM power off, 0: L2 SRAM retain.

        PMUA->CP_IDLE_CFG  |= BIT_1 | l1off | l2off;
        PMUM->CPCR |= BIT_31;  //allows AXI_IDLE

#ifdef CONFIG_DVC
        //PMU_DVC->DVC_CP &= ~0x7;
        //PMU_DVC->DVC_CP |= (0x8 | cr5_value[c_cop].dvc);
        //We actually does not enable the C2 level request since DDR is active
        //and DDR is coupled with CP core
        //Let APSUB idle to take care the next level lpm related DVL
#endif        

    case  2  :   //External WFI Idle mode:(C13)
        //Clock is externally gated off with CORE_IDLE=1; CORE_PWRDWN=0
        //In C13, important that CPCR.STBEN bit should not be set in this mode
        PMUA->CP_IDLE_CFG  |= BIT_0;
        PMUM->CPCR |= BIT_28;
        int_mask();

    case  1  :   //Internal WFI Idle (C12)
    case  0  :  //don't test C11 (WFE) here since it's used for spinlock
#ifdef CONFIG_DEADIDLE_POWER_TEST
        //set CCGR to save power
        PMUM->CCGR = 0x2812;
#endif
        break;
    }
    bbu_printf("CP_IDLE_CFG: 0x%x CPCR: 0x%x\n\r", PMUA->CP_IDLE_CFG, PMUM->CPCR);
    /*NOTE:
      QP may have tick interrupt prevent Cx, so disable it.
      Interrupt(mask or unmasked) will force cpu exit WFI standby mode
      even if CPSR.I/F bit is set
    */
    unsigned int flags;
    flags = disable_irq_fiq();

#ifdef CONFIG_AUDIO
    if (wake_src == WAKE_SQU && squ_dma_freed)
    {
        // DO NOT enter lowpower when squ dma was freed.
        //also here to free the invoke once lock
        power_save_peripherals_invoke_once = 0;
    }
    else
#endif
    {
#ifdef  CONFIG_ATE
        stop_timer(ate_timer);
#endif
        if (mode > 2)
        {
            c2_save(sram_off);
        }
        else
        {
            l2_cache_sync();
            wfi();
        }

#ifdef  CONFIG_ATE
        restart_timer(ate_timer);
#endif
    }
    //pin-mux resume, different low power level, different configuration.

    //restore ACGR to default value
    PMUM->CCGR = 0xdffedffe;

    //Need to make sure wake up source interrupt is handled and cleaned.

    //After wake up, we do some resume ops
    PMUM->CPCR = 0x42424000; //reset APCR value to be active state.

    //This should be the last step after finish restore.
    //It can be woken up but will not interrupted before this step.
    //After this, core can enter ISR and we're ready for ISR.
    restore_irq_fiq(flags);
}
#endif
