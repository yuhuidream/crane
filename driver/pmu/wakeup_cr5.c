#include "bbu.h"
#include "ppset.h"
#include "pmic.h"
#include "PMUA.h"
#include "PMUM.h"
#include "interrupt.h"
#include "pmu_dec.h"
#include "timer_dec.h"
//#include "keypad.h"
#include "gic_dec.h"
#include "mfp-emei.h"
#include "GPIOE.h"
//#include "sdh_sd.h"
#include "APBCLK.h"
#include "IPC.h"


static void sd_wk_isr(void *dummy) {
    (void)dummy;
    //PMUA->SD_ROT_WAKE_CLR |= (BIT_4|BIT_7|BIT_1|BIT_0);
    *(volatile uint8_t *)(0xD428002B) &= ~0x7;
    *(volatile uint16_t *)(0xD4280034) &= ~0x1C0;
    *(volatile uint32_t *)0xd428287c |= 0x9F;
    bbu_printf("sd card wk isr\r\n");

}

static void sd_wk_set(uint32_t time_sec) {

    (void)time_sec;

    PMUM->AWUCRM |= 0x3C00040; //set SDH2/3 wakeup source@PMUM_AWUCRM
    PMUM->APCR &= (~BIT_16);
    PMUM->CPCR &= (~BIT_16);

    //pin-mux is done in mfp driver
    //power ldo4
    pmic_ldo_cfg(1, LDO4, 2800, ACTIVE);
    mdelay(1);

    //on Emei DKB: SDH0/MMC1 is for SD card; SDH1/MMC2 is for Wifi; SDH2/MMC3 is for eMMC
    PMUA->SDH0_CLK_RES_CTRL = 0x18;
    PMUA->SDH0_CLK_RES_CTRL = 0x1B;

    ISR_Connect(IRQ_MMP_MMC, sd_wk_isr, 0);
    INT_Enable(IRQ_MMP_MMC, 0, 15);

    /*for SD card CD pin wake up only, need to restore REG_2B after wake up*/
    *(volatile uint8_t *)(0xD428002B) |= 0x7;
    *(volatile uint16_t *)(0xD4280034) |= 0x1C0;
    *(volatile uint32_t *)0xd428287c &= ~0x9F;
    //bbu_printf("PMUA_SD_ROT_WAKE_CLR = 0x%x\r\n", PMUA->SD_ROT_WAKE_CLR);

    /* Only for those enable the 1st level interrupt in lpm_set, need to set the GIC since it's after
     * gic_migrate_irqs(cpu) in bsp.c.  ICU is routed to one or two cores according to set here.
     */
    icu_target_clear(IRQ_MMP_MMC);
    icu_target_set(IRQ_MMP_MMC, 0, 1);

    bbu_printf("SD card is set as wakeup source\r\n");

}

static void pmic_wk_isr(void *dummy) {

    uint8_t reg;

    (void)dummy;

   //bbu_printf("pmic CHG_INT wk isr,cpu=%d\r\n", cpu);
    //The pmic interrupt is cleared in the common PMIC irq routine.


   /*** we use the following code for vbus test  ,if vbus is connectted usb can work  ,else close usb                       ***/

    //ustica_base_read(0x1, &reg);

   #ifndef   CONFIG_NANO_USB_SER

    if (reg&BIT_2) {

    POST_CMD2CLI("usbm");
    }
    else
    {

    QACTIVE_POST(AO_USB,Q_NEW(QEvent ,USB_CLOSE),0);
    }
   #endif
    //bbu_printf("pmic ONKEY_INT wk isr,cpu=%d\r\n", cpu);
}

void pmic_chg_set(void) {

    uint8_t reg;

    //ustica_base_read(0xE4, &reg);
    //ustica_base_write(0xE4, (reg&0xF3)|0x8); //set the debouce timer as 8ms

#if 0 //ustica does not support this
    pmic_isr_connect(PMIC_CHG_INT,pmic_wk_isr);
    pmic_int_enable(PMIC_CHG_INT);//enable the CHG_INT@PMIC side
#endif
}

static void pmic_wk_set(uint32_t time_sec) {
    (void)time_sec;

    pmic_chg_set();

    //pmic_isr_connect(PMIC_ONKEY_INT,pmic_wk_isr);
    //pmic_int_enable(PMIC_ONKEY_INT);//enable the ONKEY INT@PMIC side

    PMUM->AWUCRM |= 0x3000080; //set PMIC wakeup source@PMUM_AWUCRM
    PMUM->APCR &= (~BIT_15);
    PMUM->CPCR &= (~BIT_15);

    icu_target_clear(IRQ_MMP_PMIC_INT);
    icu_target_set(IRQ_MMP_PMIC_INT, 0, 1);
    bbu_printf("PMIC is set up as wake up source\n\r");

}

#if 0
static void kp_wk_set(uint32_t time_sec)
{
    (void)time_sec;

     PMUM->AWUCRM |= 0x03380008; //set keypad wakeup source@PMUM_AWUCRM
     PMUM->APCR &= (~BIT_20);
     PMUM->CPCR &= (~BIT_20);
     keypad_init();
     keypad_isr_enable();
    /* Only for those enabled the 1st level interrupt in lpm_set, need to set the GIC since it's after
     * gic_migrate_irqs(cpu) in bsp.c.  ICU is routed to one or two cores according to set here.
     */
     icu_target_clear(IRQ_MMP_KEYPAD);
     icu_target_set(IRQ_MMP_KEYPAD, 0, 1);
#ifdef CONFIG_SMP //temp solution to wake up all cores at the same time for test
     icu_target_set_all(IRQ_MMP_KEYPAD);
#endif
     keypad_enter_lpm();
     bbu_printf("Keypad is set up as wake up source\n\r");

}
#endif

/*If we want to support serveral pad's edge detect with wake up interrupt handler, need to use a list*/
static void pad_edge_wk_isr(void* dummy)
{
    (void)dummy;

    uint32_t valid_pad_cnt = 0;

    //bbu_printf("pad wake up \r\n");

    if(GPIOE->RER1 == BIT_1)
    {
        //once disable the edge detect funtion, the RERx bit will be cleared
        //lpm_mfpr_edge_config(MFP_PIN(ATE_WAKEUP), MFP_LPM_EDGE_NONE); //in ATE test, GPIO33 is used as wake up source
        valid_pad_cnt++;
        bbu_printf("ATE test: GPIO33 edge detect wake up!\r\n");
    }

    if(GPIOE->RER2 == BIT_27)
    {
//        lpm_mfpr_edge_config(MFP_PIN(MMC1_CD), MFP_LPM_EDGE_NONE); //on DKB 1.1, GPIO91 is used as wake up source of SD card
        valid_pad_cnt++;
        bbu_printf("GPIO91 edge wake up: SD card insert/remove detected!\r\n");
        //Need to check the GPIO91 level to decide it's insert/remove so for SD function, have to configure it as GPIO.
        /*call it here will cause SD funtion in dead loop, need tuning.
          We still keep the unit test mode(BBU need it), we should support system test mode i.e. once insert/remove event process correctly
        */
        //mrvl_sdh_sd_insert_remove_isr(NULL);

    }

    if(!valid_pad_cnt)
    {
//        INT_Disable(IRQ_MMP_PAD_EDGE); //disable the interrupt to prevent endless interrupting
        //Better TODO: find the source and disable the edge detect enable status
        bbu_printf("warning: unexpected pad edge detected!\r\n");
    }

}

static void pad_edge_wk_set(uint32_t time_sec)
{
    (void)time_sec;

     PMUM->AWUCRM |= 0x03000004; //set pad edge detect wakeup source@PMUM_AWUCRM
     PMUM->APCR &= (~BIT_21);
     PMUM->CPCR &= (~BIT_21);

     //enabled edge detect
     //lpm_mfpr_edge_config(MFP_PIN(MMC1_CD), MFP_LPM_EDGE_BOTH); //on DKB 1.1, GPIO91 is used as wake up source of SD card
#ifdef CONFIG_ATE
     //lpm_mfpr_edge_config(MFP_PIN(ATE_WAKEUP), MFP_LPM_EDGE_BOTH); //in ATE test, GPIO33 is used as wake up source
#endif

     //enable interrupt
//    ISR_Connect(IRQ_MMP_PAD_EDGE, pad_edge_wk_isr, 0);
//    INT_Enable(IRQ_MMP_PAD_EDGE, 0, 15);

    /* Only for those enabled the 1st level interrupt in lpm_set, need to set the GIC since this is called after
     * gic_migrate_irqs(cpu) in bsp.c.  ICU is routed to one or two cores according to set here.
     */
//     gic_target_clear(IRQ_MMP_PAD_EDGE);
//     gic_target_set(IRQ_MMP_PAD_EDGE, cpu, 1);

//     icu_target_clear(IRQ_MMP_PAD_EDGE);
//     icu_target_set(IRQ_MMP_PAD_EDGE, cpu, 1);
#ifdef CONFIG_SMP //temp solution to wake up all cores at the same time for PE test
//     icu_target_set_all(IRQ_MMP_PAD_EDGE);
#endif
#ifdef CONFIG_ATE
     bbu_printf("MFPR config value of GPIO91 0xD401E160 is 0x%x\r\n", BU_REG_READ(0xD401E160));
#endif
     bbu_printf("Pad edge detect wake up is set.\n\r");
}

static void timer_irq(void)
{
   bbu_printf("Waked up by 32K timer\n\r");
}

static void rtc_wk_set(uint32_t time_msec)
{

     PMUM->AWUCRM |= 0x02023F10; //set RTC wakeup source@PMUM_AWUCRM
     PMUM->APCR &= (~BIT_18);
     PMUM->CPCR &= (~BIT_18);

     bbu_printf("%d msec %s alarm is set as wakeup source\r\n", time_msec,(time_msec<1000)?"Timer":"RTC");

     int int_id;
     if(time_msec < 1000){
        if(time_msec < 50)
            time_msec = 50;
        int_id = start_timer_wakeup(time_msec, timer_irq);
     }else{
        int_id = IRQ_MMP_RTC_ALARM;
        rtc_alarm(3);
     }

     icu_target_clear(int_id);
     icu_target_set(int_id, 0, 1);
#ifdef CONFIG_SMP //temp solution to wake up all cores at the same time for PE
     icu_target_set_all(int_id);
#endif

}


static void squ_wk_set(uint32_t time_sec)
{
    (void)time_sec;

     //FCCR[28] and PMUA_QU_CLK_GATE_CTRL[30] is already set in ssp_clock_init function
     //Take care not to overrides the settings since the audio part calls the lpmset function
     PMUM->AWUCRM |= 0x3C00040; //set SQU/SDH wakeup source@PMUM_AWUCRM
     PMUM->APCR &= (~BIT_16);
     PMUM->CPCR &= (~BIT_16);

//     icu_target_clear(IRQ_MMP_HIFI_DMA);
//     icu_target_set(IRQ_MMP_HIFI_DMA, cpu, 1);
     bbu_printf(MP3_PWR_LOG"SQU audio low power playback is set up as wake up source\n\r");
}


uint32_t  ddr_axi_wake_test = 0;

static void ipc_wk_isr(void *dummy)
{
    (void)dummy;

    IPCC->ICR = BIT_0; //clear the interrupt after wake up SG
    APBClock->IPC_CLK_RST = 0x4; //disable IPC clock

/*The unvote can only wake up DDR and AXI, not AP core, so we use IPC to wake up AP to check*/
   if (0xbeef == ddr_axi_wake_result)
   {
       ddr_axi_wake_test = 1;
       bbu_printf("DDR_IDLE and AXI_IDLE unvote wake up test PASS \r\n");
   }
   else
   {
       ddr_axi_wake_test = 0;
   }

}


static void msa_unvote_wk_set(uint32_t time_sec)
{

       ddr_idle_time = time_sec;

       APBClock->IPC_CLK_RST = 0x5;
       udelay(10);
       APBClock->IPC_CLK_RST = 0x3;

       comm_test(2, 0); //let MSA go to polling

     //enable interrupt
       ISR_Connect(IRQ_MMP_CEVA2CP_IPC0, ipc_wk_isr, 0);
       INT_Enable(IRQ_MMP_CEVA2CP_IPC0, 0, 15);

    /* Only for those enabled the 1st level interrupt in lpm_set, need to set the GIC since this is called after
     * gic_migrate_irqs(cpu) in bsp.c.  ICU is routed to one or two cores according to set here.
     */
       icu_target_clear(IRQ_MMP_CEVA2CP_IPC0);
       icu_target_set(IRQ_MMP_CEVA2CP_IPC0, 0, 1);
       bbu_printf("MSA unvote wake up DDR/AXI test while using IPC wake up AP to check result\r\n");
  }

static void usb3_wk_set(uint32_t time_sec)
{

     PMUM->AWUCRM |= 0x20; //set USB3 wakeup source@PMUM_AWUCRM
     PMUM->APCR &= (~BIT_17);
     PMUM->CPCR &= (~BIT_17);
     PMUA->SD_ROT_WAKE_CLR |= BIT_29; // clear the previous int
     PMUA->SD_ROT_WAKE_CLR |= BIT_28; // enable usb3 wakeup
     icu_target_clear(IRQ_MMP_USB3);
     icu_target_set(IRQ_MMP_USB3, 0, 1);
#ifdef CONFIG_SMP //temp solution to wake up all cores at the same time for PE
     icu_target_set_all(IRQ_MMP_USB3);
#endif
     bbu_printf("USB3 is set as wakeup source\r\n");
}

/*
 * Enable AP wakeup sources and ports. To enalbe wakeup
 * ports, it needs both AP side to configure MPMU_APCR
 * and CP side to configure MPMU_CPCR to really enable
 * it. To enable wakeup sources, either AP side to set
 * MPMU_AWUCRM or CP side to set MPMU_CWRCRM can really
 * enable it.
 */

void set_wake_src(e_wake_src wake_src, uint32_t time_sec) {
    /*We have make sure only the last core at power on can enter this function*/

    /*We make sure that all wake up ports are disabled at the start, then we configure the desired one*/
    PMUM->APCR |= (BIT_23 | BIT_22 | BIT_21 | BIT_20 | BIT_18 | BIT_17 | BIT_16 | BIT_15);

    switch (wake_src) {

    case    WAKE_KP :
        //kp_wk_set(time_sec);
        break;

    case    WAKE_EDGE :
        /*For UART, SD WR, wifi,and other peripherials need pad edge detect wake up,
          on Emei, don't need to configure the Pin as GPIO function.
          */
        pad_edge_wk_set(time_sec);
        break;

    case    WAKE_WTD:
        break;

    case    WAKE_RTC :
        rtc_wk_set(time_sec);
        break;

    case    WAKE_SDH :
        sd_wk_set(time_sec);
        break;

    case WAKE_PMIC :
        pmic_wk_set(time_sec); //use time_sec to identify the PMIC sub-sources
        break;

    case WAKE_SQU :
        squ_wk_set(time_sec);
        break;

    case WAKE_DDR_IDLE_LOST:
        msa_unvote_wk_set(time_sec);
        break;
    case WAKE_USB_VBUS:  
    case WAKE_USB_ID:
    case WAKE_USB_LINE:
    case WAKE_HSIC_RESUME:
	  //usb_wk_set(time_sec,wake_src);
	  break;
    case WAKE_USB3:
        usb3_wk_set(time_sec);
        break;
    case WAKE_ALL :
        //kp_wk_set(time_sec);
        //rtc_wk_set(time_sec);
        sd_wk_set(time_sec);
        pmic_wk_set(time_sec);
        //squ_wk_set(time_sec);
        break;

    default:
        bbu_printf("unsupported wake up source yet\r\n");
        break;

    }
}
