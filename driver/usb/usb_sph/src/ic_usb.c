
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2013  -  All rights reserved
//
//  This computer program contains confidential and proprietary information,
//  and  may NOT  be reproduced or transmitted, in whole or in part,  in any
//  form,  or by any means electronic, mechanical, photo-optical, or  other-
//  wise, and  may NOT  be translated  into  another  language  without  the
//  express written permission from Marvell Corporation.
//
// ============================================================================
// =      C O M P A N Y      P R O P R I E T A R Y      M A T E R I A L       =
// ============================================================================

/*
-------------------------------------------------------------------------------
*
*
*  description:   USB ic-usb  driver  file
*
*  author:chaotong@marvell.com
*
*
*
*
*-----------------------------------------------------------------------------*/


#include "ic_usb.h"
#include "usb_test_app.h"

void  usb_phy_poweroff_for_Cp(void)
{
    USBSPH_MISC_TypeDef     *misc_sph = USB_MISC_SPH;
    MAVL_USB_PHY_TypeDef     *usb_phy = MAVL_USB_PHY;

    if (usb_service_type == IC_USB)
    {
        misc_sph->USBSPH_MISC_CTRL &= ~(MISC_CTRL_icusb_in_use_cp | MISC_CTRL_icusb_suspendm_en);

    }
    else
    {

        usb_phy->USB_CTL |= USB_CTL_REG_IDDQ_TEST;
    }


}


void judge_otgPll(void)
{

    USBSPH_MISC_TypeDef     *misc_sph = USB_MISC_SPH;
    int i = 0;
    if (misc_sph->USBSPH_MISC_CTRL & BIT_31)
        return;
    misc_sph->USBSPH_MISC_CTRL |= (BIT_7 | BIT_8);
    do
    {
        if (misc_sph->USBSPH_MISC_CTRL & BIT_31)
        {
            bbu_printf("otg pll is up\r\n");
            break;
        }
        else
        {
            i++;
            mdelay(1);
        }


    }
    while (i <= 100);
    if (i >= 100)
        bbu_printf("otg pll up beyond 100ms\r\n");

}
#ifdef __USB_OS_MQX__
void _usb_icusb_hci_vusb20_isr
(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle
)
#else
void HOST_INTERRUPT_ROUTINE_KEYWORD _usb_icusb_hci_vusb20_isr
(
    void
)
#endif
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR           usb_host_ptr;
    VUSB20_REG_STRUCT_PTR               dev_ptr;
    uint_32                             status;
    // clean warning volatile int    i;

    // bbu_printf("_usb_hci_vusb20_isr\r\n");
    /* if(ic_choose_vol==FALSE)
       {
       bbu_printf("support 1.8v power\r\n");
          ic_choose_vol=TRUE;
       }*/
#ifdef __USB_OTG__
#ifdef   HNP_HARDWARE_ASSISTANCE
    OTG_STRUCT_PTR  otg_reg;
    boolean  auto_hnp;
#endif
#endif

#ifdef __USB_OS_MQX__
    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
#else
#ifndef __USB_OTG__
    IN_ISR = TRUE;
#endif
    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)usb_host_state_struct_ptr;
#endif

    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

    /* We use a while loop to process all the interrupts while we are in the
     ** loop so that we don't miss any interrupts
     */
    //while (TRUE)
    //  {

    /* Check if any of the enabled interrupts are asserted */
    status = (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS &
              dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR);

    //bbu_printf(" USB INTR STATUS : \r\n");
    //TODO: the status is always 0, USB_INTR=0x37  USBSTS=0x88
    if (!status)
    {
        /* No more interrupts pending to be serviced */
        // break;
    } /* Endif */

    //bbu_printf(" USB INTR STATUS : ");
    //BBU_puthexd(status);
    ///BBU_puthexd(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR);
    //BBU_puthexd(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS);
    //bbu_printf("\r\n");


    /* Clear all enabled interrupts */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS = status;

    if (status & EHCI_STS_SOF_COUNT)
    {
        /* Waiting for an interval of 10 ms for reset recovery */
        if (usb_host_ptr->RESET_RECOVERY_TIMER)
        {
            usb_host_ptr->RESET_RECOVERY_TIMER--;
            if (!usb_host_ptr->RESET_RECOVERY_TIMER)
            {
                bbu_printf("EHCI_STS_SOF_COUNT: _usb_host_process_reset_recovery_done\r\n");
                _usb_host_process_reset_recovery_done((pointer)usb_host_ptr);
            } /* Endif */
        } /* Endif */
    } /* Endif */

    if (status & EHCI_STS_ASYNCH_ADVANCE)
    {
        /* Process the asynch advance */
    } /* Endif */

    //    if (status & EHCI_STS_HOST_SYS_ERROR) {
    /* Host system error. Controller halted. Inform the upper layers */
    //    _usb_host_call_service((pointer)usb_host_ptr, USB_SERVICE_SYSTEM_ERROR, 0);
    // } /* Endif */

    if (status & EHCI_STS_FRAME_LIST_ROLLOVER)
    {
        /* Process frame list rollover */
    } /* Endif */

    if (status & EHCI_STS_RECLAIMATION)
    {
        /* Process reclaimation */
    } /* Endif */

    if (status & EHCI_STS_PORT_CHANGE)
    {
        /* if it is aprocess port change under HNP, just return
        because it is an spurious interrupt */
        bbu_printf("Port Change Interrupt\r\n");
#ifdef __USB_OTG__
#ifdef   HNP_HARDWARE_ASSISTANCE
        auto_hnp = FALSE;
        otg_reg = ((USB_OTG_STATE_STRUCT_PTR)usb_otg_state_struct_ptr)->OTG_REG_PTR;
        IS_AUTO_HNP_ON(otg_reg, auto_hnp);
        if (auto_hnp)
        {
            /* Body */
            ignore = TRUE;
            return;
        } /* Endbody */
#endif
#endif


        /* Process the port change detect */
        if (_usb_ehci_process_port_change((pointer)usb_host_ptr))
        {
            /* There was a detach on port 0 so we should return */
            return;
        } /* Endif */
        /* Should return if there was a detach on OTG port */
    } /* Endif */

    if (status & EHCI_STS_USB_INTERRUPT )
    {
        /* Process the USB transaction completion and transaction error
        ** interrupt
        */
        //  bbu_printf(" EHCI_STS_USB_INTERRUPT \r\n");
        _usb_ehci_process_tr_complete((pointer)usb_host_ptr);

#ifdef DEBUG_INFO
        //bbu_printf("TR completed\r\n");
#endif

    } /* Endif */

    if (status & EHCI_STS_USB_ERROR )
    {
        /* Process the USB transaction completion and transaction error
        ** interrupt
        */
        bbu_printf(" EHCI_STS_USB_ERROR!!!!! \r\n");
        _usb_ehci_error_process((pointer)usb_host_ptr);
        //  _usb_ehci_process_tr_complete((pointer)usb_host_ptr);

#ifdef DEBUG_INFO
        //  bbu_printf("TR completed\r\n");
#endif

    } /* Endif */

    //  } /* EndWhile */

#ifndef __USB_OS_MQX__
#ifndef __USB_OTG__
    IN_ISR = FALSE;
#endif
#endif

} /* Endbody */


extern uint_8 ic_choose_vol;
static void usb_clk_enable(void)//by tongchao

{
    PMUA_TypeDef *pmua = PMUA;

    pmua->USB_CLK_RES_CTRL = 0x8;
    pmua->USB_CLK_RES_CTRL = 0xb;

}

static void usb_phy_powerup( void )//by tongchao

{
    MAVL_USB_PHY_TypeDef     *usb_phy = MAVL_USB_PHY;

    uint_32 i = 0;
    /*while(1)
        {


        if(PMUA->SD_ROT_WAKE_CLR&BIT_15)
               break;


    }*/
    //  *(volatile uint_32 *)0xd4207104=0x10901003;
    bbu_printf("usb_phy_powerup\r\n");
    //usb_phy->USB_CTL |= USB_CTL_OTG_SEL;

    usb_phy->PLLCONTROL0 &= (~(PLLCONTROL0_SEL_LPFR_MSK | PLLCONTROL0_FBDIV_MSK |
                               PLLCONTROL0_REFDIV_MSK | PLLCONTROL0_ICP_BASE));
    usb_phy->PLLCONTROL0 |= ((0x01 << PLLCONTROL0_SEL_LPFR_BASE & PLLCONTROL0_SEL_LPFR_MSK) |
                             (0xf0 << PLLCONTROL0_FBDIV_BASE & PLLCONTROL0_FBDIV_MSK) |
                             (0xd << PLLCONTROL0_REFDIV_BASE & PLLCONTROL0_REFDIV_MSK) |
                             (0x3 << PLLCONTROL0_ICP_BASE & PLLCONTROL0_ICP_MSK));

    //bbu_printf("PLLCONTROL0 add is 0x%x\r\n",&usb_phy->PLLCONTROL0);

    //bbu_printf("PLLCONTROL0 is 0x%x\r\n",usb_phy->PLLCONTROL0);
    usb_phy->USB2_DIG_REG0 &= (~(USB2_DIG_REG0_SQ_FILT_MSK | USB2_DIG_REG0_SQ_BLK_MSK
                                 | USB2_DIG_REG0_SYNC_NUM_MSK | USB2_DIG_REG0_PLL_LOCK_BYPASS));
    usb_phy->USB2_DIG_REG0 |= (0x7 << USB2_DIG_REG0_SQ_FILT_BASE |
                               0x4 << USB2_DIG_REG0_SQ_BLK_BASE |
                               0x2 << USB2_DIG_REG0_SYNC_NUM_BASE |
                               USB2_DIG_REG0_PLL_LOCK_BYPASS);
    //bbu_printf("USB2_DIG_REG0 add is 0x%x\r\n",&usb_phy->USB2_DIG_REG0);

    //bbu_printf("USB2_DIG_REG0 is 0x%x\r\n",usb_phy->USB2_DIG_REG0);
    //usb_phy->PLLCONTROL1 |= (PLLCONTROL1_PU_PLL | PLLCONTROL1_PU_PLL_BY_REG);

    //bbu_printf("PLLCONTROL1 add is 0x%x\r\n",&usb_phy->PLLCONTROL1);

    //bbu_printf("PLLCONTROL1 is 0x%x\r\n",usb_phy->PLLCONTROL1);

    //#if 0//for nezha3
    //usb_phy->PLLCONTROL1 &= ~(PLLCONTROL1_PU_PLL );
    usb_phy->PLLCONTROL1 |= (PLLCONTROL1_PU_PLL );

    usb_phy->PLLCONTROL1 &= ~PLLCONTROL1_PU_PLL_BY_REG;
    usb_phy->USB_CTL |= USB_CTL_PU_PLL;


    //#endif
    usb_phy->USB_CTL |= USB_CTL_REG_OTG_SUSPENDM_EN;
    udelay(30);
    usb_phy->USB2_TX_REG0 |= (USB2_TX_REG0_PU_ANA | USB2_TX_REG0_PU_BY_REG);
    //bbu_printf("USB2_TX_REG0 add is 0x%x\r\n",&usb_phy->USB2_TX_REG0);

    //bbu_printf("USB2_TX_REG0 is 0x%x\r\n",usb_phy->USB2_TX_REG0);
    usb_phy->USB2_RX_REG0 &= ~USB2_RX_REG0_SQ_THRESH_MSK;
    usb_phy->USB2_RX_REG0 |= 0xa;


    //bbu_printf("USB2_RX_REG0 add is 0x%x\r\n",&usb_phy->USB2_RX_REG0);

    //bbu_printf("USB2_RX_REG0 is 0x%x\r\n",usb_phy->USB2_RX_REG0);

    usb_phy->USB2_OTG_REG &= ~USB2_OTG_OTG_CONTROL_BY_PIN;
    usb_phy->USB2_OTG_REG |= (USB2_OTG_PU_OTG);

    //bbu_printf("USB2_OTG_REG add is 0x%x\r\n",&usb_phy->USB2_OTG_REG);

    //bbu_printf("USB2_OTG_REG is 0x%x\r\n",usb_phy->USB2_OTG_REG);

    udelay( 200 );
    usb_phy->CALIBRATIONCONTROL |= CALIBRATIONCONTROL1_PLLCAL_START;
    udelay( 200 );

    while ((usb_phy->USB2_RX_REG1 & USB2_RX_REG1_REG_SQCAL_DONE) == 0)
    {

        mdelay(1);
        i++;
        if (i > 100)
        {

            bbu_printf("SQ beyond 100Ms\r\n");
        }

    }
    i = 0;
    while (((usb_phy->CALIBRATIONCONTROL & CALIBRATIONCONTROL1_PLLCAL_DONE) == 0) ||
            ((usb_phy->CALIBRATIONCONTROL & CALIBRATIONCONTROL1_IMPCAL_DONE) == 0))
    {

        mdelay(1);
        i++;
        if (i > 100)
        {

            bbu_printf("PLL or IMP beyond 100Ms\r\n");
        }

    }
    i = 0;
    while ((usb_phy->PLLCONTROL0 & PLLCONTROL0_PLL_READY) == 0)
    {

        mdelay(1);
        i++;
        if (i > 100)
        {

            bbu_printf("PLL LOCK  beyond 100Ms\r\n");
        }

    }
#if 0
    bbu_printf("PLLCONTROL0 add is 0x%x\r\n", &usb_phy->PLLCONTROL0);

    bbu_printf("PLLCONTROL0 is 0x%x\r\n", usb_phy->PLLCONTROL0);

    bbu_printf("USB2_DIG_REG0 add is 0x%x\r\n", &usb_phy->USB2_DIG_REG0);

    bbu_printf("USB2_DIG_REG0 is 0x%x\r\n", usb_phy->USB2_DIG_REG0);
    bbu_printf("PLLCONTROL1 add is 0x%x\r\n", &usb_phy->PLLCONTROL1);

    bbu_printf("PLLCONTROL1 is 0x%x\r\n", usb_phy->PLLCONTROL1);

    bbu_printf("USB2_TX_REG0 add is 0x%x\r\n", &usb_phy->USB2_TX_REG0);

    bbu_printf("USB2_TX_REG0 is 0x%x\r\n", usb_phy->USB2_TX_REG0);

    bbu_printf("USB2_RX_REG0 add is 0x%x\r\n", &usb_phy->USB2_RX_REG0);

    bbu_printf("USB2_RX_REG0 is 0x%x\r\n", usb_phy->USB2_RX_REG0);

    bbu_printf("USB2_OTG_REG add is 0x%x\r\n", &usb_phy->USB2_OTG_REG);

    bbu_printf("USB2_OTG_REG is 0x%x\r\n", usb_phy->USB2_OTG_REG);

    bbu_printf("CALIBRATIONCONTROL add is 0x%x\r\n", &usb_phy->CALIBRATIONCONTROL);

    bbu_printf("CALIBRATIONCONTROL is 0x%x\r\n", usb_phy->CALIBRATIONCONTROL);

    bbu_printf("USB2_RX_REG1 add is 0x%x\r\n", &usb_phy->USB2_RX_REG1);

    bbu_printf("USB2_RX_REG1 is 0x%x\r\n", usb_phy->USB2_RX_REG1);
#endif
}





//#define SET_USB_PW(val) msi_wl_WriteGPIOs(GPIO_USB_PW_PIN, val)
void  usb2_phy_init(void)
{


    //  if (init_bit == USB_STATE_UNKNOWN)
    {
        usb_clk_enable();

        udelay(200);

        usb_phy_powerup();
    }




}
void _closk_ic_usb_int(void)
{

    USB_MISC_SPH->USBSPH_MISC_CTRL &= (~MISC_CTRL_xcvr_udrdis);
    USB_MISC_SPH->USBSPH_MISC_CTRL |= MISC_CTRL_pll_lock;

    //bbu_printf("USBSPH_MISC_CTRL is 0x%x\r\n",& ( USB_MISC_SPH->USBSPH_MISC_CTRL));
    //bbu_printf("USBSPH_MISC_CTRL is 0x%x\r\n",( USB_MISC_SPH->USBSPH_MISC_CTRL));

}

void  _turn_power_18(void)
{
    AIBAux->USIM_IO_REG |= 0x1;
    AIBAux->USIM_IO_REG |= USIM_IO_REG_PDB;
    AIBAux->USIM_IO_REG |= USIM_IO_REG_V18EN;
    bbu_printf("USIM_IO_REG is 0x%x\r\n", & (AIBAux->USIM_IO_REG));
    bbu_printf("USIM_IO_REG is 0x%x\r\n", (AIBAux->USIM_IO_REG));


}


void  _turn_power_33(void)
{
    if ((ic_choose_vol == FALSE))
    {
        mdelay(25);
        if (ic_choose_vol == TRUE)
            bbu_printf("support 1.8v power\r\n");
        else
        {
            ic_choose_vol = TRUE;
            APBClock->ASFAR = 0xBABA;
            APBClock->ASSAR = 0xEB10;
            AIBAux->USIM_IO_REG &= ~(USIM_IO_REG_V18EN | USIM_IO_REG_V25EN);
            bbu_printf("support 3.3v power\r\n");

        }
    }

}


static void usb_sph_clk_enable(void)//for nezha3

{
    // PMUA_TypeDef *pmua = PMUA;

    // pmua->USB_CLK_RES_CTRL |= USB_CLK_RES_CTRL_USB_SPH_AXICLK_EN;

    // pmua->USB_CLK_RES_CTRL |= USB_CLK_RES_CTRL_USB_SPH_AXI_RST;
    *(volatile BU_U32 *)0xd025006c |= BIT_11;
    *(volatile BU_U32 *)0xd0250068 |= BIT_12;


}

extern void print_phy(void);
uint_8  _usb_icusb_hci_vusb20_init
(
    /* the USB Host State structure */
    _usb_host_handle     handle
)
{
    /* Body */
    USB_HOST_STATE_STRUCT_PTR                    usb_host_ptr;
    VUSB20_REG_STRUCT_PTR                        dev_ptr;
    VUSB20_REG_STRUCT_PTR                        cap_dev_ptr;
    uint_8                                       vector;
    uint_32                                      frame_list_size_bits, port_control;
    // uint_32                  i;
    USB_STATUS                                   status;

    // uint_32                                      /* clean warning delay, */temp;
#ifndef __USB_OTG__
    // uint_32                                      port_control;
#endif
    //  unsigned int ttc_chip_id;
    // int is_ttc_y0 = 0;

    udelay(100);
    //  temp = BU_REG_READ( CYG_DEVICE_USBPHY_CTRL );
    //  BU_REG_WRITE( CYG_DEVICE_USBPHY_CTRL, temp | (1<<REG_BIT_PU_PLL) | (1<<REG_BIT_PU)) ;
    usb_sph_clk_enable();

    usb2_phy_init();
    _closk_ic_usb_int();
    //judge_otgPll();

    if (cpu_is_pxa1826_z3())
    {
        pmic_ldo_cfg(LDO_ON, LDO4, 3300, ACTIVE);
        pmic_ldo_cfg(LDO_ON, LDO4, 3300, SLEEP);
    }
    else
    {
        pmic_ldo_cfg(LDO_ON, LDO3, 3300, ACTIVE);
        pmic_ldo_cfg(LDO_ON, LDO3, 3300, SLEEP);
    }
    *(volatile uint_32 *)0xd1050184 |= BIT_12;
    APBClock->AIB_CLK_RST = 0x3;
    APBClock->AIB_CLK_RST = 0x3;
    APBClock->ASFAR = 0xBABA;
    APBClock->ASSAR = 0xEB10;

    //   bbu_printf(" APBClock->ASFAR is 0x%x\r\n",& ( APBClock->ASFAR));
    // bbu_printf("APBClock->ASFAR is 0x%x\r\n",(APBClock->ASFAR));
    //  bbu_printf(" APBClock->ASSAR is 0x%x\r\n",& (APBClock->ASSAR));
    //  bbu_printf("APBClock->ASSAR is 0x%x\r\n",(APBClock->ASSAR));

    *(volatile uint_32 *)0xd401e82c |= (1 << 0);

    //_turn_power_33();
    APBClock->ASFAR = 0xBABA;
    APBClock->ASSAR = 0xEB10;
    AIBAux->USIM_IO_REG &= ~(USIM_IO_REG_V18EN | USIM_IO_REG_V25EN);
    // bbu_printf("USIM_IO_REG is 0x%x\r\n",& (AIBAux->USIM_IO_REG));

    /*   APBClock->ASFAR=0xBABA;
       APBClock->ASSAR=0xEB10;
        bbu_printf("USIM_IO_REG is 0x%x\r\n",& (AIBAux->USIM_IO_REG));
        APBClock->ASFAR=0xBABA;
        APBClock->ASSAR=0xEB10;
        bbu_printf("USIM_IO_REG is 0x%x\r\n",(AIBAux->USIM_IO_REG));*/
    APBClock->ASFAR = 0xBABA;
    APBClock->ASSAR = 0xEB10;
    
	//APBSPARE->APB_spare6_reg |= (IC_USB_N_PD_EN | IC_USB_P_PD_EN);


    usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

    cap_dev_ptr  = (VUSB20_REG_STRUCT_PTR)
                   _bsp_get_usb_capability_register_base(usb_host_ptr->DEV_NUM);

    /* Get the base address of the VUSB_HS registers */
    usb_host_ptr->DEV_PTR = (pointer) ((uint_32)cap_dev_ptr +
                                       (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.CAPLENGTH_HCIVER &
                                        EHCI_CAP_LEN_MASK));
    // bbu_printf("CAPLENGTH_HCIVER address is 0x%x\r\n",&(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.CAPLENGTH_HCIVER));
    //bbu_printf("0xc0001100 is 0x%x\r\n",*test&0xff);

    //bbu_printf("0xd4208100 is 0x%x\r\n",*test1&0xff);
    bbu_printf("usb_host_ptr->DEV_PTR=0x%x\r\n", usb_host_ptr->DEV_PTR);
    dev_ptr = usb_host_ptr->DEV_PTR;

    /*Initialize the memory required for driver to operate */
#if 1
    status = _usb_hci_init_driver_memory(handle);
    if (status != USB_OK) return status;
#endif


#ifndef NO_ISR_INSTALLATION
    /* Get the interrupt vector number for the VUSB_HS host */
    vector = _bsp_get_usb_vector(usb_host_ptr->DEV_NUM);

#ifndef __USB_OTG__

#ifdef __USB_OS_MQX__
    if (!(USB_install_isr(vector, _usb_hci_vusb20_isr, (pointer)usb_host_ptr)))
    {
        return USBERR_INSTALL_ISR;
    } /* Endbody */
#else
    //USB_install_isr(vector, _usb_hci_vusb20_isr, (pointer)usb_host_ptr);
#endif /* USB_OS_MQX */

#endif /* __USB_OTG__ */

#endif /* NO_ISR_INSTALLATION */

    //IRQ_Glb_Ena();
    //  INT_Enable( INT_USB, IRQ_ROUTE, USB_PRIORITY );
    //ISR_Connect(INT_USB, _usb_hci_vusb20_isr );

    /*******************************************************************
     Set the size of frame list in CMD register
    *******************************************************************/
    usb_host_ptr->FRAME_LIST_SIZE = MAX_EHCI_FRAME_LIST_SIZE;
    frame_list_size_bits = MAX_EHCI_FRAME_LIST_BITS;

    /*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
     HARDWARE  REGISTERS INITIALIZATION STUFF
    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
    USB_lock();

    /*LED CHECK AFTER DEVICE SHUTDOWN but BEFORE HOST CONTROLLER RESET*/
#ifdef TEST_WRITE
    /* LED check point */
    (*USB_REGISTER_TO_WRITE) |= 0xC000;
#endif
    if (
        (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS & EHCI_STS_HC_HALTED)
#ifdef __USB_OTG__
#ifdef HNP_HARDWARE_ASSISTANCE
        || (usb_otg_state_struct_ptr->STATE_STRUCT_PTR->B_HNP_ENABLE == TRUE)
#endif
#endif
    )
    {
        /* Reset the controller to get default values */
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD = EHCI_CMD_CTRL_RESET;

        while (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD &
                EHCI_CMD_CTRL_RESET)
        {
            /* Wait for the controller reset to complete */
        } /* EndWhile */
    } /* Endif */

    /* Stop the controller */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD &=
        ~EHCI_CMD_RUN_STOP;

    /*LED CHECK AFTER HOST CONTROLLER RESET BUT BEFORE MODE SWITCH*/
#ifdef TEST_WRITE
    /* LED check point */
    (*USB_REGISTER_TO_WRITE) |= 0xC000;
#endif


    /* Configure the VUSBHS has a host controller */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_MODE =
        VUSBHS_MODE_CTRL_MODE_HOST;

    /*LED CHECK AFTER MODE SWITCH*/
#ifdef TEST_WRITE
    (*USB_REGISTER_TO_WRITE) |= 0xC000;
#endif

    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CTRLDSSEGMENT = 0;

    //   bbu_printf("cap_dev_ptr is 0x%x\r\n",cap_dev_ptr);
    /* 4-Gigabyte segment where all of the  interface data structures are allocated. */
    /* If no 64-bit addressing capability then this is zero */
    if ((cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCC_PARAMS &
            EHCI_HCC_PARAMS_64_BIT_ADDR_CAP))
    {

        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CTRLDSSEGMENT = 0;
    }
    else
    {
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CTRLDSSEGMENT =
            EHCI_DATA_STRUCTURE_BASE_ADDRESS;
    } /* Endif */

#ifndef __USB_OTG__
    //FIXME:
    /* !!!!
        Set PORTSC resigister cause the board halt!
    */

    if (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS &
            VUSB20_HCS_PARAMS_PORT_POWER_CONTROL_FLAG)
    {
        port_control = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0];
        port_control &= ~EHCI_PORTSCX_W1C_BITS;
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0] =
            (port_control | EHCI_PORTSCX_PORT_POWER);
    }
    // dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0] |=
    //    EHCI_PORTSCX_PORT_POWER;
    uint_32          cmd_val;
    if (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCC_PARAMS & EHCI_HCC_PARAMS_ASYNC_PARK_CAP)
    {
        cmd_val = dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD;
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD = cmd_val | EHCI_CMD_ASYNCHRNOUS_PARK_ENABLE |
                EHCI_CMD_ASYNCHRONOUS_COUNT;

    } /* Endif */
#endif


    //TODO: check this
    //dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR = VUSB20_HOST_INTR_EN_BITS | 0x00000080;
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR = VUSB20_HOST_INTR_EN_BITS;//0xBF;



    if (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCC_PARAMS &
            EHCI_HCC_PARAMS_PGM_FRM_LIST_FLAG)
    {
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD =
            (EHCI_INTR_NO_THRESHOLD_IMMEDIATE | frame_list_size_bits | EHCI_CMD_RUN_STOP);

    }
    else
    {
        dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD =
            (EHCI_INTR_NO_THRESHOLD_IMMEDIATE | EHCI_CMD_RUN_STOP);
    } /* Endif */
//    APBSPARE->APB_spare6_reg |= (IC_USB_N_PD_EN | IC_USB_P_PD_EN);

    USB_unlock();

    /* route all ports to the EHCI controller */
    dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CONFIG_FLAG = 1;

    /*  bbu_printf("USB_INTR is 0x%x\r\n",& (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR));
      bbu_printf("USB_INTR is 0x%x\r\n",(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR));
      bbu_printf("USB_CMD is 0x%x\r\n",& (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD));
      bbu_printf("USB_CMD is 0x%x\r\n",(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD));
      bbu_printf("PORTSCX[0] is 0x%x\r\n",& (dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0]));
      bbu_printf("PORTSCX[0] is 0x%x\r\n",(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[0]));
            APBClock->ASFAR=0xBABA;
            APBClock->ASSAR=0xEB10;
      bbu_printf("USIM_IO_REG is 0x%x\r\n",& (AIBAux->USIM_IO_REG));
            APBClock->ASFAR=0xBABA;
            APBClock->ASSAR=0xEB10;
      bbu_printf("USIM_IO_REG is 0x%x\r\n",(AIBAux->USIM_IO_REG));*/
    // print_phy();
    return USB_OK;

} /* EndBody */
