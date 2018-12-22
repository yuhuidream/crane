#include "predefines.h"    //by tongchao
#include "timer_dec.h"
#include "cp15.h"        //by tongchao
#include "cache_ops.h"//by tongchao
#include "mmu.h"//by tongchao
#include "set_mmu.h"
#include  "usb_types.h"
//#include "ACCU.h"//by tongchao

#include "usb_types.h"
#include "usb_devapi.h"
#include "usb_prv_dev.h"
//#include "usb_dev_board.h"
#include "usb_device.h"
#include "usb_mv8618malloc.h"

#include "PMUA.h"   //by tongchao
#include "USBUTMI.h"  //by tongchao
#include  "CIU.h"
//#include "bbu_usb_speaker.h"
//#define  _DEVICE_DEBUG_
#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif
#include "bbu.h"
#define _DATA_CACHE_
//#define CDC_TYPE
//#define   _USB_CHARGER
//#ifndef  CONFIG_SCREEN_CHIP
//#if ((!defined(CONFIG_SCREEN_BOARD)) &&(! defined(CONFIG_SCREEN_CHIP)))
//#define  _DEVICE_DEBUG_
//#endif
//#define ISO_TYPE
extern int _1EP;
//#define USB_DEBUG
#if defined(USB_DEBUG)
#define  _DEVICE_DEBUG_
#endif
/* in the  OTG mode this need to be a global */
static USB_DEV_STATE_STRUCT_PTR usb_global_usb_device_state_struct_ptr;

uint_8    usb_success_bit = USB_STATE_UNKNOWN;
extern volatile  uchar FORCE_FULL_SPEED;
extern uint_8 ep_number;
extern void USB_dev_memzero(void *ptr, int n);
extern void  USB_dev_memfree(void *aligned_ptr);
extern void usb_charger_detect(void);
#define USB_PRIORITY    15
#define TRIP_WIRE
extern void  bbu_mfree(void *aligned_ptr);
extern void usb_isr_disable(void);
extern void           *NONE_Cache_ADDR   ;
inline void _disable_interrupts()
{
};

inline void _enable_interrupts()
{
};
//#define USB_ISR_MODE

//static uint_8 disable_count = 0;
void usb_dci_vusb20_chip_initialize
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle
);

void usb_dci_vusb20_free_dTD(pointer);

#if 0
void print_phy(void)
{

    MAVL_USB_PHY_TypeDef     *usb_phy = MAVL_USB_PHY;
    uint_32 utmi_old = *(volatile uint_32 *)0xd4207104;
    //*(volatile uint_32 *)0xd4207104=0x10901003;


    bbu_printf("USB_CLK_RES_CTRL add is 0x%x\r\n", &(PMUA->USB_CLK_RES_CTRL));
    bbu_printf("USB_CLK_RES_CTRL  is 0x%x\r\n", (PMUA->USB_CLK_RES_CTRL));
    bbu_printf("usb_phy->PLL_CTRL_0 add is 0x%x\r\n", &(usb_phy->PLL_CTRL_0));
    bbu_printf("usb_phy->PLL_CTRL_0  is 0x%x\r\n", (usb_phy->PLL_CTRL_0));
    bbu_printf("usb_phy->PLL_CTRL_1 add is 0x%x\r\n", &(usb_phy->PLL_CTRL_1));
    bbu_printf("usb_phy->PLL_CTRL_1  is 0x%x\r\n", (usb_phy->PLL_CTRL_1));
    bbu_printf("usb_phy->TX_CH_CTRL_0 add is 0x%x\r\n", &(usb_phy->TX_CH_CTRL_0));
    bbu_printf("usb_phy->TX_CH_CTRL_0  is 0x%x\r\n", (usb_phy->TX_CH_CTRL_0));
    bbu_printf("usb_phy->TX_CH_CTRL_1 add is 0x%x\r\n", &(usb_phy->TX_CH_CTRL_1));
    bbu_printf("usb_phy->TX_CH_CTRL_1  is 0x%x\r\n", (usb_phy->TX_CH_CTRL_1));
    bbu_printf("usb_phy->TX_CH_CTRL_2 add is 0x%x\r\n", &(usb_phy->TX_CH_CTRL_2));
    bbu_printf("usb_phy->TX_CH_CTRL_2  is 0x%x\r\n", (usb_phy->TX_CH_CTRL_2));
    bbu_printf("usb_phy->RX_CH_CTRL_0 add is 0x%x\r\n", &(usb_phy->RX_CH_CTRL_0));
    bbu_printf("usb_phy->RX_CH_CTRL_0  is 0x%x\r\n", (usb_phy->RX_CH_CTRL_0));
    bbu_printf("usb_phy->RX_CH_CTRL_1 add is 0x%x\r\n", &(usb_phy->RX_CH_CTRL_1));
    bbu_printf("usb_phy->RX_CH_CTRL_1  is 0x%x\r\n", (usb_phy->RX_CH_CTRL_1));
    bbu_printf("usb_phy->RX_CH_CTRL_2 add is 0x%x\r\n", &(usb_phy->RX_CH_CTRL_2));
    bbu_printf("usb_phy->RX_CH_CTRL_2  is 0x%x\r\n", (usb_phy->RX_CH_CTRL_2));
    bbu_printf("usb_phy->ANALOG_CTRL_0 add is 0x%x\r\n", &(usb_phy->ANALOG_CTRL_0));
    bbu_printf("usb_phy->ANALOG_CTRL_0  is 0x%x\r\n", (usb_phy->ANALOG_CTRL_0));
    bbu_printf("usb_phy->ANALOG_CTRL_1 add is 0x%x\r\n", &(usb_phy->ANALOG_CTRL_1));
    bbu_printf("usb_phy->ANALOG_CTRL_1  is 0x%x\r\n", (usb_phy->ANALOG_CTRL_1));
    bbu_printf("usb_phy->DIGITAL_CTRL_0 add is 0x%x\r\n", &(usb_phy->DIGITAL_CTRL_0));
    bbu_printf("usb_phy->DIGITAL_CTRL_0  is 0x%x\r\n", (usb_phy->DIGITAL_CTRL_0));
    bbu_printf("usb_phy->DIGITAL_CTRL_1 add is 0x%x\r\n", &(usb_phy->DIGITAL_CTRL_1));
    bbu_printf("usb_phy->DIGITAL_CTRL_1  is 0x%x\r\n", (usb_phy->DIGITAL_CTRL_1));
    bbu_printf("usb_phy->CHARGERDTCT_CTRL add is 0x%x\r\n", &(usb_phy->CHARGERDTCT_CTRL));
    bbu_printf("usb_phy->CHARGERDTCT_CTRL  is 0x%x\r\n", (usb_phy->CHARGERDTCT_CTRL));
    bbu_printf("usb_phy->OTG_CTRL_0 add is 0x%x\r\n", &(usb_phy->OTG_CTRL_0));
    bbu_printf("usb_phy->OTG_CTRL_0  is 0x%x\r\n", (usb_phy->OTG_CTRL_0));
    bbu_printf("0xd4207104  is 0x%x\r\n", (utmi_old));


}
#endif
void usb_clk_enable(void)//by tongchao

{
    PMUA_TypeDef *pmua = PMUA;

    pmua->USB_CLK_RES_CTRL |= USB_CLK_RES_CTRL_USB_AXICLK_EN;

    pmua->USB_CLK_RES_CTRL |= USB_CLK_RES_CTRL_USB_AXI_RST;

}

void usb_clk_disable(void)
{
    PMUA->USB_CLK_RES_CTRL &= ~USB_CLK_RES_CTRL_USB_AXICLK_EN;
    PMUA->USB_CLK_RES_CTRL &= ~USB_CLK_RES_CTRL_USB_AXI_RST;

}


#define PLL_READY_TIMEOUT       (20 * 100)          // 20 To get resulotion of 1 miliSec -- 100 is the number of loops of 1 miliSec
#define RevisionID_A0   0xA0
#define RevisionID_A1   0xA1
#define ChipID_A0       0xC828
/*void usb_phy_powerup ( void )//by tongchao
{
  // NEVO_APP_BPB_TypeDef    *app_bpb=NEVO_APP_BPB;
   MAVL_USB_PHY_TypeDef     *usb_phy=MAVL_USB_PHY;

    CIU_TypeDef              *ciu=CIU;

     uint_32                temp;
      temp=ciu->CHIP_ID;
      if((((temp&CHIP_ID_REV_ID_MSK)>>CHIP_ID_REV_ID_BASE)==RevisionID_A0)||(((temp & CHIP_ID_REV_ID_MSK) >> CHIP_ID_REV_ID_BASE) == RevisionID_A1))
      {
             usb_phy->PLL_CTRL_0=0x5af0;
      }
     else
      {
    usb_phy->PLL_CTRL_0 &=~(USB_PHY_PLL_CTRL_0_FBDIV_MSK|USB_PHY_PLL_CTRL_0_REFDIV_MSK);
   usb_phy->PLL_CTRL_0 |=(0xf0<<USB_PHY_PLL_CTRL_0_FBDIV_BASE|0xd<<USB_PHY_PLL_CTRL_0_REFDIV_BASE);
      }
   usb_phy->PLL_CTRL_1 &=~(USB_PHY_PLL_CTRL_1_PLLCAL12_MSK|USB_PHY_PLL_CTRL_1_KVCO_MSK| USB_PHY_PLL_CTRL_1_ICP_MSK);
   usb_phy->PLL_CTRL_1 |=(0x3<<USB_PHY_PLL_CTRL_1_PLLCAL12_BASE|0x3<<USB_PHY_PLL_CTRL_1_KVCO_BASE|
                       0x3<<USB_PHY_PLL_CTRL_1_ICP_BASE|USB_PHY_PLL_CTRL_1_PLL_LOCK_BYPASS|USB_PHY_PLL_CTRL_1_PU_PLL);

   usb_phy->TX_CH_CTRL_0&=~USB_PHY_TX_CH_CTRL_0_IMPCAL_VTH_MSK;
   usb_phy->TX_CH_CTRL_0|=0x2<<USB_PHY_TX_CH_CTRL_0_IMPCAL_VTH_BASE;

   usb_phy->TX_CH_CTRL_1&=~(USB_PHY_TX_CH_CTRL_1_CK60_PHSEL_MSK|USB_PHY_TX_CH_CTRL_1_AMP_MSK|USB_PHY_TX_CH_CTRL_1_TXVDD12_MSK);
    usb_phy->TX_CH_CTRL_1|=(0x4<<USB_PHY_TX_CH_CTRL_1_CK60_PHSEL_BASE|0x4<<USB_PHY_TX_CH_CTRL_1_AMP_BASE|
    0x3<<USB_PHY_TX_CH_CTRL_1_TXVDD12_BASE);

     usb_phy->TX_CH_CTRL_2&=~USB_PHY_TX_CH_CTRL_2_DRV_SLEWRATE_MSK;
      usb_phy->TX_CH_CTRL_2|=0x3<<USB_PHY_TX_CH_CTRL_2_DRV_SLEWRATE_BASE;



   usb_phy->RX_CH_CTRL_0 &=~(USB_PHY_RX_CH_CTRL_0_SQ_LENGTH_MSK|USB_PHY_RX_CH_CTRL_0_SQ_THRESH_MSK);
   usb_phy->RX_CH_CTRL_0 |=(0x2<<USB_PHY_RX_CH_CTRL_0_SQ_LENGTH_BASE|0xa<<USB_PHY_RX_CH_CTRL_0_SQ_THRESH_BASE);

   usb_phy->ANALOG_CTRL_1|=(USB_PHY_ANALOG_CTRL_1_PU_ANA);//|USB_PHY_ANALOG_CTRL_1_R_ROTATE_SEL);

  usb_phy->OTG_CTRL_0|=USB_PHY_OTG_CTRL_0_PU_OTG;

 //usb_phy->DIGITAL_CTRL_4 |=0x3<<USB_PHY_DIGITAL_CTRL_4_PLLVDD12_BASE;
  //udelay(PLL_READY_TIMEOUT);

   //Is this exist
   usb_phy->INTERNALCID_0=0x9011;
   usb_phy->INTERNALCID_1=0;
    *(volatile uint_32 *)0xd4207104|=(BIT_20|BIT_23);
    *(volatile uint_32 *)0xd4207104|=BIT_0|BIT_1;
    udelay(200);
     usb_phy->PLL_CTRL_1|=USB_PHY_PLL_CTRL_1_VCOCAL_START;
    udelay(400);
     usb_phy->TX_CH_CTRL_0|=USB_PHY_TX_CH_CTRL_0_RCAL_START;
     udelay(40);
     usb_phy->TX_CH_CTRL_0&=~USB_PHY_TX_CH_CTRL_0_RCAL_START;
   udelay(400);
      // Limiting USB burst length to 8. AXI2MC module used in Wukong-Y0 does not support 16.
    *(unsigned long*)0xd4208090 = 0x2;
    // We don't have a reliable way to detect if Vbus is high. So set Run bit on controller.
       *(volatile uint_32 *)0xd4208140|=0x1;
}*/



void usb_phy_powerup ( void )//by tongchao

{
    MAVL_USB_PHY_TypeDef     *usb_phy = MAVL_USB_PHY;

    uint_32 i = 0;
    /*while(1)
        {


        if(PMUA->SD_ROT_WAKE_CLR&BIT_15)
               break;


    }*/
    //  *(volatile uint_32 *)0xd4207104=0x10901003;
    usb_phy->USB_CTL |= USB_CTL_OTG_SEL;

    usb_phy->PLLCONTROL0 &= (~(PLLCONTROL0_SEL_LPFR_MSK | PLLCONTROL0_FBDIV_MSK |
                               PLLCONTROL0_REFDIV_MSK | PLLCONTROL0_ICP_BASE));
    usb_phy->PLLCONTROL0 |= ((0x01 << PLLCONTROL0_SEL_LPFR_BASE & PLLCONTROL0_SEL_LPFR_MSK) |
                             (0xf0 << PLLCONTROL0_FBDIV_BASE & PLLCONTROL0_FBDIV_MSK) |
                             (0xd << PLLCONTROL0_REFDIV_BASE & PLLCONTROL0_REFDIV_MSK) |
                             (0x3 << PLLCONTROL0_ICP_BASE & PLLCONTROL0_ICP_MSK));

    //bbu_printf("PLLCONTROL0 add is 0x%x\r\n",&usb_phy->PLLCONTROL0);

    //bbu_printf("PLLCONTROL0 is 0x%x\r\n",usb_phy->PLLCONTROL0);
    usb_phy->USB2_DIG_REG0 &= (~(USB2_DIG_REG0_SQ_FILT_MSK | USB2_DIG_REG0_SQ_BLK_MSK
                                 | USB2_DIG_REG0_SYNC_NUM_MSK) | USB2_DIG_REG0_PLL_LOCK_BYPASS);
    usb_phy->USB2_DIG_REG0 |= (0x7 << USB2_DIG_REG0_SQ_FILT_BASE |
                               0x4 << USB2_DIG_REG0_SQ_BLK_BASE |
                               0x2 << USB2_DIG_REG0_SYNC_NUM_BASE |
                               USB2_DIG_REG0_PLL_LOCK_BYPASS);
    //bbu_printf("USB2_DIG_REG0 add is 0x%x\r\n",&usb_phy->USB2_DIG_REG0);

    //bbu_printf("USB2_DIG_REG0 is 0x%x\r\n",usb_phy->USB2_DIG_REG0);
    usb_phy->PLLCONTROL1 |= (PLLCONTROL1_PU_PLL | PLLCONTROL1_PU_PLL_BY_REG);

    //bbu_printf("PLLCONTROL1 add is 0x%x\r\n",&usb_phy->PLLCONTROL1);

    //bbu_printf("PLLCONTROL1 is 0x%x\r\n",usb_phy->PLLCONTROL1);

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

void  usb2_phy_init(void)
{

    usb_clk_enable();

    udelay(200);

    usb_phy_powerup();





}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_init
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device controller.
*
*END*-----------------------------------------------------------------*/

//Region usb_SCRATCH_STRUCT_BASE_Region        = {0x8A0019B0, 1024, 0x10, RWNA, 0, 0x8A0019B0,&nevobbuPT};         //by tongchao fix cache bug
//Region usbRegion        = {0x8AF00000, 1024, 0x2, RWNA, 0, 0x8AF00000,&nevobbuPT};

extern uint_8    *usbNoneCacheAddr  ;

uint_8 usb_dci_vusb20_init
(
    /* [IN] the USB device controller to initialize */
    uint_8                     devnum,

    /* [OUT] the USB_dev_initialize state structure */
    usb_device_handle         handle
)

{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

    uint_32                                      total_memory = 0;
    uchar_ptr                                    driver_memory;

    unsigned int temp;

    MAVL_USB_TypeDef_PTR               new_dev_ptr;
    devnum = devnum;
#ifdef LA_DEBUG
    /* enable all GPIO to look at signal for USB controller */
    BU_REG_WRITE( 0x90006050, 0x7f883000);
    BU_REG_WRITE( 0x8000a878, 0x000cfffc);
    BU_REG_WRITE( 0x8000d008, 0x00000000);
    BU_REG_WRITE( 0x8000d024, 0x0000ffff);
    BU_REG_WRITE( 0x8000d05c, 0x00000018);
#endif



    //Enable usb clock
    usb_clk_enable();



    /*******modified by cosmo*********/
    //  BBU_usWait(1000);
    udelay(200);


    //add the PHY init code accordingly to hm2_diag software packet


    /*define usb phy registers       */
    usb_phy_powerup();

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;

#ifndef SMALL_CODE_SIZE

    usb_dev_ptr->MAX_ENDPOINTS = new_dev_ptr->U2XDCCPARAMS & VUSB20_MAX_ENDPTS_SUPPORTED;
    temp = (usb_dev_ptr->MAX_ENDPOINTS * 2);

    /****************************************************************
      Consolidated memory allocation
    ****************************************************************/
    total_memory = ((temp * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT)) ) +
                   ((MAX_EP_TR_DESCRS * sizeof(VUSB20_EP_TR_STRUCT)) + 32) +
                   (sizeof(SCRATCH_STRUCT) * MAX_EP_TR_DESCRS);

    driver_memory = (uchar_ptr)(usbNoneCacheAddr);//(uchar_ptr) USB_Uncached_memalloc(total_memory);(uchar_ptr) USB_Uncached_memalloc(total_memory);//(uchar_ptr) USB_Uncached_memalloc(total_memory);


    if (driver_memory == NULL)
    {
#ifdef _DEVICE_DEBUG_
        bbu_printf("_usb_dci_vusb20_init, malloc failed\r\n");
#endif
        return USBERR_ALLOC;
    }
#else

    //  dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    /****************************************************************
      Consolidated memory allocation
    ****************************************************************/
#ifndef NO_DYNAMIC_MEMORY_ALLOCATION
    total_memory = ((usb_dev_ptr->MAX_ENDPOINTS * 2 * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT)) + 2048) +
                   ((MAX_EP_TR_DESCRS * sizeof(VUSB20_EP_TR_STRUCT)) + 32) +
                   (sizeof(SCRATCH_STRUCT) * MAX_EP_TR_DESCRS);
    driver_memory = (uchar_ptr) USB_Uncached_memalloc(total_memory);
    if (driver_memory == NULL)
    {
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_dci_vusb20_init, malloc failed.\r\n");
#endif
        return USBERR_ALLOC;
    }

#else
    total_memory = ((usb_dev_ptr->MAX_ENDPOINTS * 2 * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT)) + 2048) +
                   ((MAX_EP_TR_DESCRS * sizeof(VUSB20_EP_TR_STRUCT)) + 32) +
                   (sizeof(SCRATCH_STRUCT) * MAX_EP_TR_DESCRS);
    driver_memory = (uchar_ptr) USB_DRIVER_MEMORY_START;
#endif


#endif
    /****************************************************************
      Zero out the memory allocated
    ****************************************************************/
    USB_dev_memzero((void *) driver_memory, total_memory );

#ifdef _DATA_CACHE_
    /****************************************************************
       Flush the zeroed memory if cache is enabled
    ****************************************************************/
    //  USB_dcache_flush_mlines((pointer)driver_memory,total_memory);

#endif


#ifndef NO_DYNAMIC_MEMORY_ALLOCATION
    /****************************************************************
      Keep a pointer to driver memory alloctaion
    ****************************************************************/
    usb_dev_ptr->DRIVER_MEMORY = driver_memory;

    /****************************************************************
       Assign QH base
    ****************************************************************/
    usb_dev_ptr->EP_QUEUE_HEAD_BASE = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)\
                                      driver_memory;
    // bbu_printf("EP_QUEUE_HEAD_BASE add is 0x%x\r\n",usb_dev_ptr->EP_QUEUE_HEAD_BASE);

    driver_memory += temp * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT);

    // bbu_printf("2048 add is 0x%x\r\n",driver_memory);
    // memset(driver_memory,'a',2048);
    //driver_memory += ((temp * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT)) + 2048);
    //  driver_memory +=  2048;


    /* Align the endpoint queue head to 2K boundary */
    usb_dev_ptr->EP_QUEUE_HEAD_PTR = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
                                     USB_MEM2048_ALIGN((uint_32)usb_dev_ptr->EP_QUEUE_HEAD_BASE);

    //bbu_printf("EP_QUEUE_HEAD_PTR add is 0x%x\r\n",usb_dev_ptr->EP_QUEUE_HEAD_PTR);

    /****************************************************************
       Assign DTD base
    ****************************************************************/

    usb_dev_ptr->DTD_BASE_PTR = (VUSB20_EP_TR_STRUCT_PTR) \
                                driver_memory;
    driver_memory += ((MAX_EP_TR_DESCRS * sizeof(VUSB20_EP_TR_STRUCT)) + 32);

    /* Align the dTD base to 32 byte boundary */
    usb_dev_ptr->DTD_ALIGNED_BASE_PTR = (VUSB20_EP_TR_STRUCT_PTR)
                                        USB_MEM32_ALIGN((uint_32)usb_dev_ptr->DTD_BASE_PTR);

    /****************************************************************
       Assign SCRATCH Structure base
    ****************************************************************/
    /* Allocate memory for internal scratch structure */
    usb_dev_ptr->SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR)\
                                       driver_memory;
#else
    usb_dev_ptr->EP_QUEUE_HEAD_BASE = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)\
                                      USB_DRIVER_QH_BASE;
    usb_dev_ptr->DTD_BASE_PTR = (VUSB20_EP_TR_STRUCT_PTR) \
                                USB_DRIVER_DTD_BASE;
    usb_dev_ptr->SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR)\
                                       USB_DRIVER_SCRATCH_STRUCT_BASE;
    usb_dev_ptr->EP_QUEUE_HEAD_PTR = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_BASE;

    usb_dev_ptr->DTD_ALIGNED_BASE_PTR = ( VUSB20_EP_TR_STRUCT_PTR)usb_dev_ptr->DTD_BASE_PTR;

#endif
    /*********************************************************************
    If there is no need to install ISR, don't install it. Some USB products
    would just like to hardcode the function pointers for ISR calls.
    *********************************************************************/
#ifndef NO_ISR_INSTALLATION
#ifndef __USB_OTG__
    /* Install the interrupt service routine */
#ifndef __USB_OS_MQX__
    //   USB_install_isr(usb_dev_ptr->DEV_VEC, usb_dci_vusb20_isr, NULL);
#else
    //   if (!(USB_install_isr(_bsp_getusb_vector(usb_dev_ptr->DEV_NUM),
    //     usb_dci_vusb20_isr, (pointer)usb_dev_ptr)))
    //  {
    //    #ifdef _DEVICE_DEBUG_
    //      bbu_printf("usb_dci_vusb20_init, error installing ISR.\r\n");
    //   #endif
    //    return USBERR_INSTALL_ISR;
    //  } /* Endbody */
#endif
#endif   /* _usb_OTG__*/
#endif

    /* Enable global interrupt */


    usb_dev_ptr->USB_STATE = USB_STATE_UNKNOWN;

    /* Initialize the VUSB_HS controller */

    usb_dci_vusb20_chip_initialize((usb_device_handle)usb_dev_ptr);

    usb_global_usb_device_state_struct_ptr = usb_dev_ptr;

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_init, SUCCESSFUL.\r\n");
#endif

    return USB_OK;
} /* EndBody */




//----------------------------------------------------------------------------------------
//  usb_dci_vusb20_set_xcvr_interface()
//
//
//----------------------------------------------------------------------------------------


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_chip_initialize
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device controller.
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_chip_initialize
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

    MAVL_USB_TypeDef_PTR                          new_dev_ptr;//by tongchao
    //NEVO_APP_BPB_TypeDef                          *app_bpb=NEVO_APP_BPB         ;//by tongchao
    VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
    VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr;
    uint_32                                      i, port_control;
    SCRATCH_STRUCT_PTR                           temp_scratch_ptr;
    uint_32 temp;

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_chip_initialize.\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;

    /* clear all interrupts before start up USB device
    */

    temp = new_dev_ptr->U2XUSBSTS;
    new_dev_ptr->U2XUSBSTS = temp;

    /* Stop the controller */

    new_dev_ptr->U2XUSBCMD &= ~EHCI_CMD_RUN_STOP;

    /* Reset the controller to get default values */

    new_dev_ptr->U2XUSBCMD = EHCI_CMD_CTRL_RESET;

    while (new_dev_ptr->U2XUSBCMD & EHCI_CMD_CTRL_RESET)
    {
    }

#ifdef TRIP_WIRE
    /* Program the controller to be the USB device controller */

    new_dev_ptr->U2XUSBMODE = (VUSBHS_MODE_CTRL_MODE_DEV | VUSBHS_MODE_SETUP_LOCK_DISABLE);
#else
    /* Program the controller to be the USB device controller */
    dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_MODE =  VUSBHS_MODE_CTRL_MODE_DEV;
#endif

    /* Initialize the internal dTD head and tail to NULL */
    usb_dev_ptr->DTD_HEAD = NULL;
    usb_dev_ptr->DTD_TAIL = NULL;

    /* Make sure the 16 MSBs of this register are 0s */

    new_dev_ptr->U2XEPSETUPSTAT = 0;
    ep_queue_head_ptr = usb_dev_ptr->EP_QUEUE_HEAD_PTR;

    /* Initialize all device queue heads */
    for (i = 0; i < (usb_dev_ptr->MAX_ENDPOINTS * 2); i++)
    {
        /* Interrupt on Setup packet */
        (ep_queue_head_ptr + i)->MAX_PKT_LENGTH =
            (((uint_32)USB_MAX_CTRL_PAYLOAD <<  VUSB_EP_QUEUE_HEAD_MAX_PKT_LEN_POS) | VUSB_EP_QUEUE_HEAD_IOS);
        (ep_queue_head_ptr + i)->NEXT_DTD_PTR =  VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;
    } /* Endfor */

    /* Configure the Endpoint List Address
    */

    new_dev_ptr->U2XEPLISTADDR = (uint_32)ep_queue_head_ptr;


    if (new_dev_ptr->U2XHCSPARAMS & VUSB20_HCS_PARAMS_PORT_POWER_CONTROL_FLAG)

    {
        port_control = new_dev_ptr->U2XPORTSC[0];
        port_control &= (~EHCI_PORTSCX_W1C_BITS | ~EHCI_PORTSCX_PORT_POWER);
        new_dev_ptr->U2XPORTSC[0] = port_control;
    }


    dTD_ptr = usb_dev_ptr->DTD_ALIGNED_BASE_PTR;

    temp_scratch_ptr = usb_dev_ptr->SCRATCH_STRUCT_BASE;

    /* Enqueue all the dTDs */
    for (i = 0; i < MAX_EP_TR_DESCRS; i++)
    {
        dTD_ptr->SCRATCH_PTR = temp_scratch_ptr;
        dTD_ptr->SCRATCH_PTR->FREE = usb_dci_vusb20_free_dTD;
        /* Set the dTD to be invalid */
        dTD_ptr->NEXT_TR_ELEM_PTR = VUSBHS_TD_NEXT_TERMINATE;
        /* Set the Reserved fields to 0 */
        dTD_ptr->SIZE_IOC_STS &= ~VUSBHS_TD_RESERVED_FIELDS;
        dTD_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
        usb_dci_vusb20_free_dTD((pointer)dTD_ptr);
        dTD_ptr++;
        temp_scratch_ptr++;
    } /* Endfor */



    new_dev_ptr->U2XEPCTRL[0] = (EHCI_EPCTRL_TX_DATA_TOGGLE_RST | EHCI_EPCTRL_RX_DATA_TOGGLE_RST);
    new_dev_ptr->U2XEPCTRL[0] =  ~(EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);


    //    *(volatile unsigned long *)0xd4207004 |= 0x100000;    //Is this General Register 4 (GEN_REG4) ? (1435 in nevo soc main 0.33)

    //app_bpb->GEN_REG4|=APP_BPB_GEN_REG4_MIPI_REF_CTRL_PU_REF_ ;
    /* Enable interrupts don't enable if one is disabled by a compiler switch*/


    new_dev_ptr->U2XUSBINTR =
        (
            EHCI_INTR_INT_EN |
#ifndef ERROR_INTERRUPT_DISABLE
            EHCI_INTR_ERR_INT_EN |
#endif
#ifndef PORT_CHANGE_INTERRUPT_DISABLE
            EHCI_INTR_PORT_CHANGE_DETECT_EN |
#endif
#ifndef SOF_INTERRUPT_DISABLE
            //   EHCI_INTR_SOF_UFRAME_EN |
#endif
#ifndef SUSPEND_INTERRUPT_DISABLE
            EHCI_INTR_DEVICE_SUSPEND |
#endif
            EHCI_INTR_RESET_EN
        );

#ifndef SMALL_CODE_SIZE
    usb_dev_ptr->USB_STATE = USB_STATE_UNKNOWN;
#endif

    // DCB RETURN: TPV A0 app specific, set UTMI+
    // interface and 8 bit width
    //-------------------------------------------
    // usb_dci_vusb20_set_xcvr_interface( handle, EHCI_PORTSCX_UTMI_XCVR_SELECT, 0 );
    // usb_dci_vusb20_set_xcvr_width( handle, EHCI_PORTSCX_PTW_8BIT_SELECT, 0 );

    if (FORCE_FULL_SPEED == TRUE)
    {
        //  bbu_printf("Full Speed Mode\r\n");

        new_dev_ptr->U2XPORTSC[0] |= (1 << 24);
    }
    else
    {
        //  bbu_printf("High Speed Mode.\r\n");
    }


    /* Set the Run bit in the command register */

    new_dev_ptr->U2XUSBCMD = EHCI_CMD_RUN_STOP;

    // bbu_printf(" new_dev_ptr->U2XCAPLENGTH is %d",new_dev_ptr->U2XCAPLENGTH);

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_chip_initialize, SUCCESSFUL.\r\n");
#endif

} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_free_dTD
*  Returned Value : void
*  Comments       :
*        Enqueues a dTD onto the free DTD ring.
*
*END*-----------------------------------------------------------------*/

void usb_dci_vusb20_free_dTD
(
    /* [IN] the dTD to enqueue */
    pointer  dTD_ptr
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)
            (((VUSB20_EP_TR_STRUCT_PTR)dTD_ptr)->SCRATCH_PTR->PRIVATE);

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_free_dTD");
#endif

    /*
    ** This function can be called from any context, and it needs mutual
    ** exclusion with itself.
    */
    USB_lock();

    /*
    ** Add the dTD to the free dTD queue (linked via PRIVATE) and
    ** increment the tail to the next descriptor
    */


    EHCI_DTD_QADD(usb_dev_ptr->DTD_HEAD, usb_dev_ptr->DTD_TAIL,
                  (VUSB20_EP_TR_STRUCT_PTR)dTD_ptr);

    usb_dev_ptr->DTD_ENTRIES++;

    USB_unlock();

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_free_dTD, SUCCESSFUL");
#endif


} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_add_dTD
*  Returned Value : USB_OK or error code
*  Comments       :
*        Adds a device transfer desriptor(s) to the queue.
*
*END*-----------------------------------------------------------------*/
uint_8 usb_dci_vusb20_add_dTD
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] The transfer descriptor address */
    XD_STRUCT_PTR              xd_ptr
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    // VUSB20_REG_STRUCT_PTR                        dev_ptr;

    MAVL_USB_TypeDef_PTR                          new_dev_ptr;//by tongchao
    VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr, temp_dTD_ptr, first_dTD_ptr ;//= NULL;
    VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
    uint_32                                      curr_pkt_len, remaining_len;
    uint_32                                      curr_offset, temp, bit_pos, tempDqh; /*temp_ep_stat*/
#ifdef TRIP_WIRE
    boolean                                      read_safe;
    boolean                                      temp_ep_stat;
#endif
    /*********************************************************************
    For a optimal implementation, we need to detect the fact that
    we are adding DTD to an empty list. If list is empty, we can
    actually skip several programming steps esp. those for ensuring
    that there is no race condition.The following boolean will be useful
    in skipping some code here.
    *********************************************************************/
    boolean                                      list_empty = FALSE;
    //   int        i;
    int   type_ep = xd_ptr->EP_TYPE;
#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_add_dTD\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    // dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR; //by tongchao
    remaining_len = xd_ptr->WTOTALLENGTH;

    curr_offset = 0;
    temp = (2 * xd_ptr->EP_NUM + xd_ptr->BDIRECTION);
    bit_pos = (1 << (16 * xd_ptr->BDIRECTION + xd_ptr->EP_NUM));

    ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)new_dev_ptr->U2XEPLISTADDR + temp;
    /*********************************************************************
    This loops iterates through the length of the transfer and divides
    the data in to DTDs each handling the a max of 0x4000 bytes of data.
    The first DTD in the list is stored in a pointer called first_dTD_ptr.
    This pointer is later linked in to QH for processing by the hardware.
    *********************************************************************/

    do
    {
        /* Check if we need to split the transfer into multiple dTDs */
        if (remaining_len > VUSB_EP_MAX_LENGTH_TRANSFER)
        {
            curr_pkt_len = VUSB_EP_MAX_LENGTH_TRANSFER;
        }
        else
        {
            curr_pkt_len = remaining_len;
        } /* Endif */

        remaining_len -= curr_pkt_len;

        /* Get a dTD from the queue */
        EHCI_DTD_QGET(usb_dev_ptr->DTD_HEAD, usb_dev_ptr->DTD_TAIL, dTD_ptr);

        if (!dTD_ptr)
        {
#ifdef _DEVICE_DEBUG_
            bbu_printf("usb_dci_vusb20_add_dTD, SUCCESSFUL\r\n");
#endif
            return USBERR_TR_FAILED;
        } /* Endif */

        usb_dev_ptr->DTD_ENTRIES--;

        if (curr_offset == 0)
        {
            first_dTD_ptr = dTD_ptr;
        } /* Endif */

#ifdef _DATA_CACHE_
        /**************************************************************
        USB Memzero does not bypass the cache and hence we must use
        DTD pointer to update the memory and bypass the cache. If
        your DTD are allocated from an uncached regigio, you can
        eliminitate this approach and switch back to USB_memzero().
        **************************************************************/

        dTD_ptr->NEXT_TR_ELEM_PTR   = 0;
        dTD_ptr->SIZE_IOC_STS       = 0;
        dTD_ptr->BUFF_PTR0          = 0;
        dTD_ptr->BUFF_PTR1          = 0;
        dTD_ptr->BUFF_PTR2          = 0;
        dTD_ptr->BUFF_PTR3          = 0;
        dTD_ptr->BUFF_PTR4          = 0;

#else
        /* Zero the dTD. Leave the last 4 bytes as that is the scratch pointer */
        USB_dev_memzero((void *) dTD_ptr, (sizeof(VUSB20_EP_TR_STRUCT) - 4));
        //memset((pointer)dTD_ptr,0,(sizeof(VUSB20_EP_TR_STRUCT) - 4));
#endif

        /* Initialize the dTD */
        dTD_ptr->SCRATCH_PTR->PRIVATE = handle;

        /* Set the Terminate bit */
        dTD_ptr->NEXT_TR_ELEM_PTR = VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;


        /*************************************************************
        FIX ME: For hig-speed and high-bandwidth ISO IN endpoints,
        we must initialize the multiplied field so that Host can issues
        multiple IN transactions on the endpoint. See the DTD data
        structure for MultiIO field.

        S Garg 11/06/2003
        *************************************************************/

        /* Fill in the transfer size */
        if (!remaining_len)
        {
            dTD_ptr->SIZE_IOC_STS = ((curr_pkt_len <<
                                      VUSBHS_TD_LENGTH_BIT_POS) | (VUSBHS_TD_IOC) |
                                     (VUSBHS_TD_STATUS_ACTIVE));
        }
        else
        {
            dTD_ptr->SIZE_IOC_STS = ((curr_pkt_len << VUSBHS_TD_LENGTH_BIT_POS)
                                     | VUSBHS_TD_STATUS_ACTIVE);
        } /* Endif */

        /* Set the reserved field to 0 */
        dTD_ptr->SIZE_IOC_STS &= ~VUSBHS_TD_RESERVED_FIELDS;


        /* 4K apart buffer page pointers */
        dTD_ptr->BUFF_PTR0 = (uint_32)(xd_ptr->WSTARTADDRESS + curr_offset);
        dTD_ptr->BUFF_PTR1 = (dTD_ptr->BUFF_PTR0 + 4096);
        dTD_ptr->BUFF_PTR2 = (dTD_ptr->BUFF_PTR1 + 4096);
        dTD_ptr->BUFF_PTR3 = (dTD_ptr->BUFF_PTR2 + 4096);
        dTD_ptr->BUFF_PTR4 = (dTD_ptr->BUFF_PTR3 + 4096);

        curr_offset += curr_pkt_len;

        /* Maintain the first and last device transfer descriptor per
        ** endpoint and direction
        */
        if (!usb_dev_ptr->EP_DTD_HEADS[temp])
        {
            usb_dev_ptr->EP_DTD_HEADS[temp] = dTD_ptr;
            /***********************************************
            If list does not have a head, it means that list
            is empty. An empty condition is detected.
            ***********************************************/
            list_empty = TRUE;
        } /* Endif */

        /* Check if the transfer is to be queued at the end or beginning */
        temp_dTD_ptr = usb_dev_ptr->EP_DTD_TAILS[temp];

        /* Remember which XD to use for this dTD */
        dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD = (pointer)xd_ptr;

        /* New tail */
        usb_dev_ptr->EP_DTD_TAILS[temp] = dTD_ptr;
        if (temp_dTD_ptr)
        {
            /* Should not do |=. The Terminate bit should be zero */
            temp_dTD_ptr->NEXT_TR_ELEM_PTR = (uint_32)dTD_ptr;
        } /* Endif */

    }
    while (remaining_len);   /* EndWhile */


    /**************************************************************
    In the loop above DTD has already been added to the list
    However endpoint has not been primed yet. If list is not empty we need safter ways to add DTD to the
    existing list. Else we just skip to adding DTD to QH safely.
    **************************************************************/

    if (!list_empty)
    {
#ifdef TRIP_WIRE
        /*********************************************************
        Hardware v3.2+ require the use of semaphore to ensure that
        QH is safely updated.
        *********************************************************/

        /*********************************************************
        Check the prime bit. If set goto done
        *********************************************************/
        // if (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME & bit_pos)
        if (new_dev_ptr->U2XEPPRIME & bit_pos)
        {
            goto done;
        }

        read_safe = FALSE;
        while (!read_safe)
        {

            /*********************************************************
            start with setting the semaphores
            *********************************************************/
            /*dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD |=
                                           EHCI_CMD_ATDTW_TRIPWIRE_SET;*/

            new_dev_ptr->U2XUSBCMD |= USB_CMD_ATDTW;
            /*********************************************************
            Read the endpoint status
            *********************************************************/

            // temp_ep_stat = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS & bit_pos;


            temp_ep_stat = new_dev_ptr->U2XEPSTAT & bit_pos;
            /*********************************************************
            Reread the ATDTW semaphore bit to check if it is cleared.
            When hardware see a hazard, it will clear the bit or
            else we remain set to 1 and we can proceed with priming
            of endpoint if not already primed.
            *********************************************************/
            /*if(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &
                                           EHCI_CMD_ATDTW_TRIPWIRE_SET);*/
            if (new_dev_ptr->U2XUSBCMD & USB_CMD_ATDTW)
            {
                read_safe = TRUE;
            }
            //  read_safe = TRUE;

        }/*end while loop */

        /*********************************************************
        Clear the semaphore
        *********************************************************/
        /*dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &=
                                       EHCI_CMD_ATDTW_TRIPWIRE_CLEAR;*/
        new_dev_ptr->U2XUSBCMD &= ~USB_CMD_ATDTW;
        /*********************************************************
        If endpoint is not active, we activate it now.
        *********************************************************/
        if (!temp_ep_stat)
        {
            /* No other transfers on the queue */
            ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)first_dTD_ptr;
            ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;
            ep_queue_head_ptr->RESERVED2[3] = 0;
            tempDqh = ep_queue_head_ptr->RESERVED2[3];

            dsb();               //fix armv7 system memory sequence
            /* Prime the Endpoint */
            //dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = bit_pos;
            new_dev_ptr->U2XEPPRIME = bit_pos;
        }

#else   /*workaround old method */

        /* Start CR 1015 */
        /* Prime the Endpoint */


        new_dev_ptr->U2XEPPRIME = bit_pos;


        if (!(new_dev_ptr->U2XEPSTAT & bit_pos))

        {

            /* old workaround will be compiled */

            while (new_dev_ptr->U2XEPPRIME & bit_pos)
            {
                /* Wait for the ENDPTPRIME to go to zero */
            } /* EndWhile */


            if (new_dev_ptr->U2XEPSTAT & bit_pos)
            {
                /* The endpoint was not not primed so no other transfers on
                ** the queue
                */
                goto done;
            } /* Endif */

        }
        else
        {
            goto done;
        } /* Endif */

        /* No other transfers on the queue */
        ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)first_dTD_ptr;
        ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;

        /* Prime the Endpoint */
        new_dev_ptr->U2XEPPRIME = bit_pos;

#endif


    }
    else
    {
        /* No other transfers on the queue */
        ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)first_dTD_ptr;
        ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;

        /* Prime the Endpoint */
        ep_queue_head_ptr->RESERVED2[3] = 0;
        tempDqh = ep_queue_head_ptr->RESERVED2[3];
        dsb();  //fix armv7 system memory sequence
        new_dev_ptr->U2XEPPRIME = bit_pos;
        //i =   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME;


    }

    //FIXME:
done:

    /* wait a little */
    {
        int i;
        for (i = 0; i < 1000; i++);
    }

    //bbu_printf("add ATD IS ok \r\n");
    return USB_OK;
    /* End CR 1015 */
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_send_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sends data by adding and executing the dTD. Non-blocking.
*
*END*-----------------------------------------------------------------*/
uint_8 usb_dci_vusb20_send_data
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] The transfer descriptor address */
    XD_STRUCT_PTR              xd_ptr
)
{
    /* Body */



    /* Add and execute the device transfer descriptor */
    //bbu_printf("enter send data going\r\n");
    return (usb_dci_vusb20_add_dTD(handle, xd_ptr));
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_recv_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Receives data by adding and executing the dTD. Non-blocking.
*
*END*-----------------------------------------------------------------*/
uint_8 usb_dci_vusb20_recv_data
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] The transfer descriptor address */
    XD_STRUCT_PTR              xd_ptr
)
{
    /* Body */


    /* Add and execute the device transfer descriptor */
    //bbu_printf("enter receive data going\r\n");
    return (usb_dci_vusb20_add_dTD(handle, xd_ptr));
} /* EndBody */



uint_32 get_complete_status(void)
{
    return MAVL_USB_OTG->U2XEPCOMPLETE;

}







/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_process_tr_complete
*  Returned Value : None
*  Comments       :
*        Services transaction complete interrupt
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_process_tr_complete
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

    MAVL_USB_TypeDef_PTR                          new_dev_ptr; //by tongchao
    VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr;
    VUSB20_EP_TR_STRUCT_PTR                      temp_dTD_ptr;
    VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
    uint_32                                      temp, i, ep_num, direction, bit_pos;
    uint_32                                      remaining_length = 0;
    uint_32                                      actual_transfer_length = 0;
    uint_32                                      errors = 0;//, print;
    XD_STRUCT_PTR                                xd_ptr;
    XD_STRUCT_PTR                                temp_xd_ptr = NULL;
    uchar_ptr                                    buff_start_address = NULL;
    boolean                                      endpoint_detected = FALSE;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;  //by tongchao
    //bbu_printf("usb_dci_vusb20_process_tr_complete\r\n");
#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_process_tr_complete\r\n");
#endif

    //bbu_printf("usb_cancel_transfer  direction  is %d ,ep_num is %d  .\r\n",direction,ep_num);
    /* We use separate loops for ENDPTSETUPSTAT and ENDPTCOMPLETE because the
    ** setup packets are to be read ASAP
    */

    /* Process all Setup packet received interrupts */

    bit_pos = new_dev_ptr->U2XEPSETUPSTAT;
    //new_dev_ptr->U2XEPSETUPSTAT=bit_pos;
    if (bit_pos)
    {
        for (i = 0; i < USB_MAX_CONTROL_ENDPOINTS; i++)
        {
            if (bit_pos & (1 << i))
            {
                usb_device_call_service(handle, i, TRUE, 0, 0, 8, 0);               //call ep0 service
            } /* Endif */
        } /* Endfor */
    } /* Endif */

    /* Don't clear the endpoint setup status register here. It is cleared as a
    ** setup packet is read out of the buffer
    */

    /* Process non-setup transaction complete interrupts */


    bit_pos = new_dev_ptr->U2XEPCOMPLETE;
    //bbu_printf("enpoint number : %d\r\n",bit_pos);
#ifdef _DEVICE_DEBUG_
    if ((bit_pos != 0) && (bit_pos != 65536) && (bit_pos != 65537))
        bbu_printf("enpoint number : %d\r\n", bit_pos);
#endif
    //  bbu_printf("\r\n");

    /* Clear the bits in the register */
    new_dev_ptr->U2XEPCOMPLETE = bit_pos;

    if (bit_pos)
    {
        /* Get the endpoint number and the direction of transfer */

        for (i = 0; i < USB_MAX_ENDPOINTS * 2; i++)
        {

            endpoint_detected = FALSE;
            if ((i < USB_MAX_ENDPOINTS) && (bit_pos & (1 << i)))
            {


                ep_num = i;
                direction = 0;
                endpoint_detected = TRUE;
            }
            else if ((i >= USB_MAX_ENDPOINTS) && (bit_pos & (1 << (i + 16 - USB_MAX_ENDPOINTS))))
            {

                ep_num = (i - USB_MAX_ENDPOINTS);
                direction = 1;
                endpoint_detected = TRUE;
            }

            if (endpoint_detected)
            {


#ifdef _DEVICE_DEBUG_
                if ((bit_pos != 0) && (bit_pos != 65536) && (bit_pos != 65537))
                    bbu_printf("enpoint ep_num : %d\r\n", ep_num);
#endif


                temp = (2 * ep_num + direction);


                // dcache_clean_all();
                //  tlb_invalidate_all();
                /* Get the first dTD */
                dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];

                ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)new_dev_ptr->U2XEPLISTADDR + temp;

                /* Process all the dTDs for respective transfers */

                while (dTD_ptr)
                {

                    if (dTD_ptr->SIZE_IOC_STS & VUSBHS_TD_STATUS_ACTIVE)
                    {
                        /* No more dTDs to process. Next one is owned by VUSB */
                        // usb_device_cancel_transfer(handle, ep_num, direction);
#if 0
                        xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
                        if (xd_ptr)
                        {
                            buff_start_address = xd_ptr->WSTARTADDRESS;
                            actual_transfer_length = xd_ptr->WTOTALLENGTH;
                            bbu_printf("actual_transfer_length is %d", actual_transfer_length);
                            temp_xd_ptr = xd_ptr;
                        } /* Endif */

                        bbu_printf("ep_num is %d,direction is %d\r\n", ep_num, direction);
#endif
                        break;
                    } /* Endif */

                    /* Get the correct internal transfer descriptor */
                    xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
                    if (xd_ptr)
                    {
                        buff_start_address = xd_ptr->WSTARTADDRESS;
                        actual_transfer_length = xd_ptr->WTOTALLENGTH;

                        temp_xd_ptr = xd_ptr;
                    } /* Endif */

                    /* Get the address of the next dTD */
                    temp_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)
                                   (dTD_ptr->NEXT_TR_ELEM_PTR & VUSBHS_TD_ADDR_MASK);

                    /* Read the errors */
                    errors = (dTD_ptr->SIZE_IOC_STS & VUSBHS_TD_ERROR_MASK);

                    if (!errors)
                    {
                        /* No errors */
                        /* Get the length of transfer from the current dTD */
                        remaining_length += ((dTD_ptr->SIZE_IOC_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);
                        actual_transfer_length -= remaining_length;
                    }
                    else
                    {
                        if (errors & VUSBHS_TD_STATUS_HALTED)
                        {
                            /* Clear the errors and Halt condition */
                            ep_queue_head_ptr->SIZE_IOC_INT_STS &= ~errors;
                        } /* Endif */
                    } /* Endif */

                    /* Retire the processed dTD */
                    //  usb_dci_vusb20_cancel_transfer();



                    //if(xd_ptr->EP_TYPE==USB_ISOCHRONOUS_ENDPOINT)

                    // bbu_printf("usb_cancel_transfer  direction  is %d ,ep_num is %d  .\r\n",direction,ep_num);
#if 0
                    if (ep_num != 0)
                    {
                        for (uint_32 pt = 0; pt < actual_transfer_length; pt++)
                        {
                            bbu_printf("buff_start_address is0x%x,actual_transfer_length=%d\r\n", buff_start_address, actual_transfer_length);
                            bbu_printf("buff_start_address[pt] is%d\r\n", buff_start_address[pt]);
                            bbu_printf("\r\n ");
                        }
                    }
#endif
                    //mdelay(10);

#ifdef _DEVICE_DEBUG_
                    bbu_printf("usb_cancel_transfer  direction  is %d ,ep_num is %d  .\r\n", direction, ep_num);
#endif
                    if (!ep_num)
                    {
                        usb_device_cancel_transfer(handle, ep_num, direction);
                        //usb_device_cancel_transfer(handle, ep_num, !direction);   //fix certification bug
                    }
                    else
                        usb_device_cancel_transfer(handle, ep_num, direction);
                    if (temp_dTD_ptr)
                    {
                        if ((uint_32)temp_dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD != (uint_32)temp_xd_ptr)
                        {

                            //bbu_printf("enter temp_dTD_ptr\r\n ");
                            /* Transfer complete. Call the register service function for the
                            ** endpoint
                            */

                            ///     bbu_printf("buff_start_address:  ");
                            ///     BBU_puthexd(buff_start_address);
                            ///     bbu_printf("\r\n actual_transfer_length");
                            ///     BBU_puthexd(actual_transfer_length);
                            ///         bbu_printf("\r\n ");

                            usb_device_call_service(handle, ep_num, FALSE, direction,
                                                    buff_start_address, actual_transfer_length, errors);
                            remaining_length = 0;
                        } /* Endif */
                    }
                    else
                    {
                        /* Transfer complete. Call the register service function for the
                        ** endpoint
                        */
                        ///                  for ( print = 0; print < actual_transfer_length; print++){
                        ///     BBU_puthexd(buff_start_address[print]);
                        ///     bbu_printf("\r\n ");
                        ///                        }
                        ///                 bbu_printf("buff_start_address:  ");
                        /// BBU_puthexd(buff_start_address);
                        /// bbu_printf("\r\n actual_transfer_length");
                        /// BBU_puthexd(actual_transfer_length);
                        ///        bbu_printf("\r\n ");
                        usb_device_call_service(handle, ep_num, FALSE, direction,
                                                buff_start_address, actual_transfer_length, errors);
                    } /* Endif */
                    dTD_ptr = temp_dTD_ptr;
                    errors = 0;
                } /* Endwhile */
            } /* Endif */
        } /* Endfor */
    } /* Endif */
    else
    {
        //mdelay(3);
        //bbu_printf("usb_device_cancel_transfer bit==0\r\n");
        usb_device_cancel_transfer(handle, 0, 1);  //fix certification bug
    }
#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_process_tr_complete, SUCCESSFUL\r\n");
#endif

} /* EndBody */
extern  void USB_vm_free();
#if 0
uint_8  usb_close_register_service( /* [IN] Handle to the USB device */
    void)
{
    USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr;
    SERVICE_STRUCT_PTR         service_ptr;
    SERVICE_STRUCT_PTR _PTR_   search_ptr;
    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)usb_global_usb_device_state_struct_ptr;
    USB_lock();
    // usb_clk_disable();

    /* for(search_ptr = &usb_dev_ptr->SERVICE_HEAD_PTR;
        *search_ptr;
        search_ptr = &(*search_ptr)->NEXT)
      {
               if((*search_ptr))
                  {
                     service_ptr=*search_ptr;
         *search_ptr=service_ptr->NEXT;
          bbu_printf("service_ptr  is 0x%x\r\n",service_ptr);
          bbu_mfree((pointer)service_ptr);


                  }
      }*/

    do
    {
        search_ptr = &usb_dev_ptr->SERVICE_HEAD_PTR;
        if (*search_ptr)
        {
            service_ptr = *search_ptr;
            usb_dev_ptr->SERVICE_HEAD_PTR = (*search_ptr)->NEXT;
            //bbu_printf("service_ptr  is 0x%x\r\n",service_ptr);
            bbu_mfree((pointer)service_ptr);
        }

    }
    while (usb_dev_ptr->SERVICE_HEAD_PTR);


    //  bbu_printf("usb_dev_ptr->XD_BASE  is 0x%x\r\n",usb_dev_ptr->XD_BASE);
    bbu_mfree((pointer)(usb_dev_ptr->XD_BASE));
    // bbu_printf("usb_dev_ptr->TEMP_XD_PTR  is 0x%x\r\n",usb_dev_ptr->TEMP_XD_PTR);
    bbu_mfree((pointer)(usb_dev_ptr->TEMP_XD_PTR));
    USB_vm_free();

    //bbu_printf("usb_dev_ptr  is 0x%x\r\n",usb_dev_ptr);
    bbu_mfree((pointer)usb_dev_ptr);
    USB_unlock();

    return USB_OK;
}

#endif





/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_isr
*  Returned Value : None
*  Comments       :
*        Services all the VUSB_HS interrupt sources
*
*END*-----------------------------------------------------------------*/

extern int post_bit;
#ifndef _usb_OS_MQX__
DEVICE_INTERRUPT_ROUTINE_KEYWORD void usb_dci_vusb20_isr (   void )
#else
void usb_dci_vusb20_isr (  usb_device_handle handle  )
#endif

//void USB_ISR(void)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    //  VUSB20_REG_STRUCT_PTR                        dev_ptr;

    MAVL_USB_TypeDef_PTR                    new_dev_ptr;//by tongchao
    uint_32                                      status;


#ifdef _usb_OS_MQX__
    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
#else




    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)usb_global_usb_device_state_struct_ptr;

#endif
    //bbu_printf("usb_dci_vusb20_isr\r\n");
#ifdef _DEVICE_DEBUG_
    // bbu_printf("usb_dci_vusb20_isr\r\n");
#endif
    //DEBUGMSG(TTC_DBG_LEVEL,("isr"));

    //   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;

    status = new_dev_ptr->U2XUSBSTS;




    /* Clear all the interrupts occured */
    new_dev_ptr->U2XUSBSTS = status;

    if (status & EHCI_STS_RESET)
    {

        udelay(1550);
        // bbu_printf("usb_dci_vusb20_process_reset.\r\n");
#ifdef _DEVICE_DEBUG_
        bbu_printf("RESET RESET new_dev_ptr->U2XUSBSTS: 0x%X\n\r", new_dev_ptr->U2XUSBSTS);
#endif
#ifndef USB_ISR_MODE
        post_info2Ao((usb_device_handle)usb_dev_ptr, USB_RESET);
#else
        usb_dci_vusb20_process_reset((pointer)usb_dev_ptr);
#endif
    } /* Endif */

#ifndef PORT_CHANGE_INTERRUPT_DISABLE
    if (status & EHCI_STS_PORT_CHANGE)
    {
        //bbu_printf("usb_dci_vusb20_process_port_change.\r\n");
#ifndef USB_ISR_MODE
        post_info2Ao((usb_device_handle)usb_dev_ptr, USB_PORT_CHANGER);
#else
        usb_dci_vusb20_process_port_change((pointer)usb_dev_ptr);
#endif
#ifdef _DEVICE_DEBUG_
        // bbu_printf("PORT PORT new_dev_ptr->U2XUSBSTS: 0x%X\n\r",new_dev_ptr->U2XUSBSTS);
#endif
    } /* Endif */
#endif

#ifndef ERROR_INTERRUPT_DISABLE
    if (status & EHCI_STS_ERR)
    {
        //bbu_printf("usb_dci_vusb20_process_error.\r\n");
#ifndef USB_ISR_MODE
        post_info2Ao((usb_device_handle)usb_dev_ptr, USB_STS_ERR);
#else
        usb_dci_vusb20_process_error((pointer)usb_dev_ptr);
#endif
    } /* Endif */
#endif



#ifndef USB_ISR_MODE

    if (status & EHCI_STS_INT)
    {
        //     bbu_printf("usb_dci_vusb20_process_tr_complete\r\n");

#ifdef  _USB_CHARGER
#if ((!defined(CONFIG_SCREEN_BOARD) )&&(! defined(CONFIG_SCREEN_CHIP)))
        if (post_bit == 0)
            usb_charger_detector_init();
#endif
#endif

        // do{

        // usb_dci_vusb20_process_tr_complete((pointer)usb_dev_ptr);
        //complete_num++;
        post_info2Ao((usb_device_handle)usb_global_usb_device_state_struct_ptr, USB_STS_COMPLETE);
        usb_isr_disable();
        //}while(new_dev_ptr->U2XEPCOMPLETE);
    } /* Endif */

#else
    if (status & EHCI_STS_INT)
    {
        // bbu_printf("usb_dci_vusb20_process_tr_complete\r\n");

#ifdef  _USB_CHARGER
#if ((!defined(CONFIG_SCREEN_BOARD) )&&(! defined(CONFIG_SCREEN_CHIP)))
        if (post_bit == 0)
            usb_charger_detector_init();
#endif
#endif

        do
        {
            usb_dci_vusb20_process_tr_complete((pointer)usb_dev_ptr);
#ifdef _DEVICE_DEBUG_

            //bbu_printf("STS_INT STS_INT new_dev_ptr->U2XUSBSTS: 0x%X\n\r",new_dev_ptr->U2XUSBSTS);

#endif
        }

        while (new_dev_ptr->U2XEPCOMPLETE);
    } /* Endif */
#endif
#ifndef SUSPEND_INTERRUPT_DISABLE
    if (status & EHCI_STS_SUSPEND)
    {
#ifndef USB_ISR_MODE
        post_info2Ao((usb_device_handle)usb_dev_ptr, USB_STS_SUSPEND);
#else
        usb_dci_vusb20_process_suspend((pointer)usb_dev_ptr);
#endif
        return;
    } /* Endif */
#endif

    ///  } /* Endfor */

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_isr, SUCCESSFUL\r\n");
#endif
    usb_success_bit = usb_dev_ptr->USB_STATE;
    return;

} /* EndBody */






/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_process_reset
*  Returned Value : None
*  Comments       :
*        Services reset interrupt
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_process_reset
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    // VUSB20_REG_STRUCT_PTR             dev_ptr;

    MAVL_USB_TypeDef_PTR             new_dev_ptr;//by tongchao
    uint_32                                      temp;
    //   uint_32 tmp;

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_process_reset.\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;


    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR; //by tongchao
    usb_dev_ptr->BUS_RESETTING = TRUE;

    usb_dev_ptr->USB_STATE =  USB_STATE_DEFAULT;
    usb_dev_ptr->USB_CURR_CONFIG = 0;
#if 0
    /* Inform the application so that it can cancel all previously queued transfers */
    usb_device_cancel_transfer(handle, 0, USB_RECV);
    usb_device_cancel_transfer(handle, 0, USB_SEND);
    usb_device_cancel_transfer(handle, ep_number, USB_RECV);
    //#ifndef  ISO_TYPE
    usb_device_cancel_transfer(handle, ep_number, USB_SEND);
#ifdef CDC_TYPE
    usb_device_cancel_transfer(handle, ep_number + 1, USB_SEND);
#endif
#endif
    usb_device_call_service(usb_dev_ptr, USB_SERVICE_RESET_ENPOINT, 0, 0, 0, 0, 0);

    /* The address bits are past bit 25-31. Set the address */
    new_dev_ptr->U2XDEVICEADDR &= ~0xFE000000;                                      //   USB_DEVICEADDR_USBADR_MSK
    /* Clear all the setup token semaphores
    */


    temp = new_dev_ptr->U2XEPSETUPSTAT;
    new_dev_ptr->U2XEPSETUPSTAT = temp;
    /* Clear all the endpoint complete status bits
     */


    temp = new_dev_ptr->U2XEPCOMPLETE;
    new_dev_ptr->U2XEPCOMPLETE = temp;

    while (new_dev_ptr->U2XEPPRIME & 0xFFFFFFFF)
    {
        ;/* Wait until all ENDPTPRIME bits cleared */
    } /* Endif */

    /* Write 1s to the Flush register */

    if (new_dev_ptr->U2XPORTSC[0]&EHCI_PORTSCX_PORT_RESET)
    {

        new_dev_ptr->U2XEPFLUSH = 0xFFFFFFFF;
        while (new_dev_ptr->U2XEPFLUSH & 0xFFFFFFFF);

    }

    usb_device_call_service(usb_dev_ptr, USB_SERVICE_BUS_RESET, 0, 0, 0, 0, 0);



#ifdef _DEVICE_DEBUG_
    //   bbu_printf("usb_dci_vusb20_process_reset, SUCCESSFUL.\r\n");
#endif

} /* EndBody */




#ifndef SUSPEND_INTERRUPT_DISABLE
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_process_suspend
*  Returned Value : None
*  Comments       :
*        Services suspend interrupt
*
*END*-----------------------------------------------------------------*/
extern int post_bit;
void usb_dci_vusb20_process_suspend
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_process_suspend.\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND = usb_dev_ptr->USB_STATE;

#ifdef   _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_process_suspend   USB_STATE is%d   .\r\n", usb_dev_ptr->USB_STATE);
#endif
    usb_dev_ptr->USB_STATE = USB_STATE_SUSPEND;

    /* Inform the upper layers */
    usb_device_call_service(usb_dev_ptr, USB_SERVICE_SLEEP, 0, 0, 0, 0, 0);

#ifdef  _USB_CHARGER
#if ((!defined(CONFIG_SCREEN_BOARD) )&&(! defined(CONFIG_SCREEN_CHIP)))
    if ((USB_STATE_CONFIG == usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND) || (USB_STATE_ADDRESS == usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND))
    {
        if (post_bit == 1)
#ifdef CONFIG_CHARGER
        {
            USB_CHARGER_POST(4);
#endif
            post_bit = 0;
            usb_success_bit = USB_STATE_UNKNOWN;
        }
    }
#endif
#endif
#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_process_suspend, SUCCESSFUL.\r\n");
#endif

} /* EndBody */
#endif

#ifndef SOF_INTERRUPT_DISABLE
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_process_SOF
*  Returned Value : None
*  Comments       :
*        Services SOF interrupt
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_process_SOF
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

    MAVL_USB_TypeDef_PTR                    new_dev_ptr;//by tongchao
#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_process_SOF.\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;
    /* Inform the upper layer */
    usb_device_call_service(usb_dev_ptr, USB_SERVICE_SOF, 0, 0, 0, new_dev_ptr->U2XFRINDEX, 0);

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_process_SOF, SUCCESSFUL.\r\n");
#endif

} /* EndBody */
#endif


#ifndef PORT_CHANGE_INTERRUPT_DISABLE
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_process_port_change
*  Returned Value : None
*  Comments       :
*        Services port change detect interrupt
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_process_port_change
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    //   VUSB20_REG_STRUCT_PTR                        dev_ptr;

    MAVL_USB_TypeDef_PTR                        new_dev_ptr;// by tongchao
#ifdef _DEVICE_DEBUG_
    // bbu_printf("usb_dci_vusb20_process_port_change.\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    //  dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR; // by tongchao
    if (usb_dev_ptr->BUS_RESETTING)
    {
        /* Bus reset operation complete */
        usb_dev_ptr->BUS_RESETTING = FALSE;
    } /* Endif */



    if (!(new_dev_ptr->U2XPORTSC[0]&EHCI_PORTSCX_PORT_RESET))
    {
        if (new_dev_ptr->U2XPORTSC[0]&EHCI_PORTSCX_PORT_HIGH_SPEED)
        {
            usb_dev_ptr->SPEED = USB_SPEED_HIGH;
        }
        else
        {
            usb_dev_ptr->SPEED = USB_SPEED_FULL;
        } /* Endif */

    }

#ifndef SMALL_CODE_SIZE

    if (new_dev_ptr->U2XPORTSC[0]&EHCI_PORTSCX_PORT_SUSPEND)
    {

        usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND = usb_dev_ptr->USB_STATE;
        usb_dev_ptr->USB_STATE = USB_STATE_SUSPEND;

        /* Inform the upper layers */

    } /* Endif */
#endif

    if (!(new_dev_ptr->U2XPORTSC[0]&EHCI_PORTSCX_PORT_SUSPEND) &&
            (usb_dev_ptr->USB_STATE == USB_STATE_SUSPEND))
    {
        usb_dev_ptr->USB_STATE = usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND;
        /* Inform the upper layers */

#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_dci_vusb20_process_port_change, SUCCESSFUL, resumed.\r\n");
#endif
        return;
    } /* Endif */


#ifdef _DEVICE_DEBUG_
    //bbu_printf("usb_dci_vusb20_process_port_change, SUCCESSFUL.\r\n");
#endif


} /* EndBody */
#endif


#ifndef ERROR_INTERRUPT_DISABLE
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_process_error
*  Returned Value : None
*  Comments       :
*        Services error interrupt
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_process_error
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_process_error.\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    /* Increment the error count */
    usb_dev_ptr->ERRORS++;

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_process_error, SUCCESSFUL.\r\n");
#endif

} /* EndBody */
#endif


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_set_speed_full
*  Returned Value : None
*  Comments       :
*        Force the controller port in full speed mode.
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_set_speed_full
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* The port number on the device */
    uint_8                     port_number
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    MAVL_USB_TypeDef_PTR                      new_dev_ptr;//by tongchao
    uint_32                                      port_control;


#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_set_speed_full");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;


    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR; //by tongchao

    port_control = new_dev_ptr->U2XPORTSC[port_number];
    port_control |= EHCI_PORTSCX_FORCE_FULL_SPEED_CONNECT;
    new_dev_ptr->U2XPORTSC[port_number] = port_control;


#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_set_speed_full, SUCCESSFUL");
#endif

} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_suspend_phy
*  Returned Value : None
*  Comments       :
*        Suspends the PHY in low power mode
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_suspend_phy
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* The port number on the device */
    uint_8                     port_number
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    // VUSB20_REG_STRUCT_PTR                        dev_ptr;
    MAVL_USB_TypeDef_PTR                          new_dev_ptr;//by tongchao
    uint_32                                      port_control;


#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_set_speed_full");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;

    port_control = new_dev_ptr->U2XPORTSC[port_number];
    port_control |= EHCI_PORTSCX_FORCE_FULL_SPEED_CONNECT;
    new_dev_ptr->U2XPORTSC[port_number] = port_control;
#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_set_speed_full, SUCCESSFUL");
#endif

} /* EndBody */




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_set_address
*  Returned Value : None
*  Comments       :
*        Sets the newly assigned device address
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_set_address
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* Address of the device assigned by the host */
    uint_8                     address
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    // VUSB20_REG_STRUCT_PTR             dev_ptr;
    MAVL_USB_TypeDef_PTR          new_dev_ptr;//by tongchao
#ifdef _DEVICE_DEBUG_
    //bbu_printf("usb_dci_vusb20_set_address\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    // dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;
#ifdef SET_ADDRESS_HARDWARE_ASSISTANCE
    /***********************************************************
    Hardware Rev 4.0 onwards have special assistance built in
    for handling the set_address command. As per the USB specs
    a device should be able to receive the response on a new
    address, within 2 msecs after status phase of set_address is
    completed. Since 2 mili second may be a very small time window
    (on high interrupt latency systems) before software could
    come to the code below and write the device register,
    this routine will be called in advance when status phase of
    set_address is still not finished. The following line in the
    code will set the bit 24 to '1' and hardware will take
    the address and queue it in an internal buffer. From which
    it will use it to decode the next USB token. Please look
    at hardware rev details for the implementation of this
    assistance.

    Also note that writing bit 24 to 0x01 will not break
    any old hardware revs because it was an unused bit.
    ***********************************************************/
    /* The address bits are past bit 25-31. Set the address
    also set the bit 24 to 0x01 to start hardware assitance*/
    new_dev_ptr->U2XDEVICEADDR = (uint_32)address << VUSBHS_ADDRESS_BIT_SHIFT;
#else

    new_dev_ptr->U2XDEVICEADDR = (uint_32)address << VUSBHS_ADDRESS_BIT_SHIFT;
#endif

    usb_dev_ptr->USB_STATE = USB_STATE_ADDRESS;

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_set_address, SUCCESSFUL\r\n");
#endif

} /* EndBody */




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_get_setup_data
*  Returned Value : None
*  Comments       :
*        Reads the Setup data from the 8-byte setup buffer
*
*END*-----------------------------------------------------------------*/
void usb_dci_vusb20_get_setup_data
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [OUT] address of the buffer to read the setup data into */
    uchar_ptr                  buffer_ptr
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

    MAVL_USB_TypeDef_PTR                                new_dev_ptr;//by tongchao
    VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
#ifdef TRIP_WIRE
    boolean                                      read_safe;
#endif


    int i;

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_get_setup_data.\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;
    /* Get the endpoint queue head
    */


    ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)new_dev_ptr->U2XEPLISTADDR + 2 * ep_num + USB_RECV;


    /********************************************************************
    CR 1219. Hardware versions 2.3+ have a implementation of tripwire
    semaphore mechanism that requires that we read the contents of
    QH safely by using the semaphore. Read the USBHS document to under
    stand how the code uses the semaphore mechanism. The following are
    the steps in brief

    1. USBCMD Write ??to Setup Tripwire in register.
    2. Duplicate contents of dQH.StatusBuffer into local software byte
       array.
    3  Read Setup TripWire in register. (if set - continue; if
       cleared goto 1.)
    4. Write '0' to clear Setup Tripwire in register.
    5. Process setup packet using local software byte array copy and
       execute status/handshake phases.


    ********************************************************************/
#ifdef TRIP_WIRE  /*if semaphore mechanism is used the following code is compiled in*/
    read_safe = FALSE;
    while (!read_safe)
    {
        /*********************************************************
        start with setting the semaphores
        *********************************************************/

        new_dev_ptr->U2XUSBCMD |= EHCI_CMD_SETUP_TRIPWIRE_SET;

        /*********************************************************
        Duplicate the contents of SETUP buffer to our buffer
        *********************************************************/
#ifdef _DATA_CACHE_
        for (i = 0; i < 8; i++)
        {
            *(buffer_ptr + i) = ep_queue_head_ptr->SETUP_BUFFER[i];
        }
#else
        /* Copy the setup packet to private buffer */
        USB_dev_memcopy((uchar_ptr)ep_queue_head_ptr->SETUP_BUFFER, buffer_ptr, 8);
#endif
        /*********************************************************
        If setup tripwire semaphore is cleared by hardware it means
        that we have a danger and we need to restart.
        else we can exit out of loop safely.
        *********************************************************/

        if (new_dev_ptr->U2XUSBCMD & EHCI_CMD_SETUP_TRIPWIRE_SET)
            read_safe = TRUE;

    }

    /*********************************************************
    Clear the semaphore bit now
    *********************************************************/

    new_dev_ptr->U2XUSBCMD &= EHCI_CMD_SETUP_TRIPWIRE_CLEAR;
#else   /*when semaphore is not used */
#ifdef _DATA_CACHE_
    for (i = 0; i < 8; i++)
    {
        *(buffer_ptr + i) = ep_queue_head_ptr->SETUP_BUFFER[i];
    }
#else
    /* Copy the setup packet to private buffer */
    USB_dev_memcopy((uchar_ptr)ep_queue_head_ptr->SETUP_BUFFER, buffer_ptr, 8);
#endif
#endif

    /* Clear the bit in the ENDPTSETUPSTAT */

    new_dev_ptr->U2XEPSETUPSTAT = (1 << ep_num);
#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_get_setup_data, SUCCESSFUL.\r\n");
#endif

} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_init_endpoint
*  Returned Value : None
*  Comments       :
*        Initializes the specified endpoint and the endpoint queue head
*
*END*-----------------------------------------------------------------*/
uint_8 usb_dci_vusb20_init_endpoint
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the transaction descriptor address */
    XD_STRUCT_PTR              xd_ptr
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    //VUSB20_REG_STRUCT_PTR                        dev_ptr;

    MAVL_USB_TypeDef_PTR                           new_dev_ptr;//by tongchao
    VUSB20_EP_QUEUE_HEAD_STRUCT _PTR_            ep_queue_head_ptr;
    uint_32                                      bit_pos;
    uint_8 flag;

#ifdef _DEVICE_DEBUG_
    // bbu_printf("usb_dci_vusb20_init_endpoint.\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    //dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;

    flag = usb_dev_ptr->TEMP_XD_PTR->DONT_ZERO_TERMINATE;

    /* Get the endpoint queue head address
    */


    ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)new_dev_ptr->U2XEPLISTADDR +
                        2 * xd_ptr->EP_NUM + xd_ptr->BDIRECTION;


    bit_pos = (1 << (16 * xd_ptr->BDIRECTION + xd_ptr->EP_NUM));

    /* Check if the Endpoint is Primed
    */

    if ((!(new_dev_ptr->U2XEPPRIME & bit_pos)) && (!(new_dev_ptr->U2XEPSTAT & bit_pos)))

    {
        /* Set the max packet length, interrupt on Setup and Mult fields */
        if (xd_ptr->EP_TYPE == USB_ISOCHRONOUS_ENDPOINT)
        {
            /* Mult bit should be set for isochronous endpoints */
            ep_queue_head_ptr->MAX_PKT_LENGTH = ((xd_ptr->WMAXPACKETSIZE << 16) |
                                                 ((xd_ptr->MAX_PKTS_PER_UFRAME ?  xd_ptr->MAX_PKTS_PER_UFRAME : 1) <<
                                                  VUSB_EP_QUEUE_HEAD_MULT_POS));
            //bbu_printf("ep_queue_head_ptr->MAX_PKT_LENGTH is 0x%x.\r\n",ep_queue_head_ptr->MAX_PKT_LENGTH);
        }
        else
        {
            if (xd_ptr->EP_TYPE != USB_CONTROL_ENDPOINT)
            {
                ep_queue_head_ptr->MAX_PKT_LENGTH = ((xd_ptr->WMAXPACKETSIZE << 16) |
                                                     (xd_ptr->DONT_ZERO_TERMINATE ?
                                                      VUSB_EP_QUEUE_HEAD_ZERO_LEN_TER_SEL : 0));
            }
            else
            {
                ep_queue_head_ptr->MAX_PKT_LENGTH = ((xd_ptr->WMAXPACKETSIZE << 16) |
                                                     VUSB_EP_QUEUE_HEAD_IOS);
            } /* Endif */
        } /* Endif */

        if (flag & USB_DEVICE_DONT_RESET_TOGGLE)
        {
            /* Enable the endpoint for Rx and Tx and set the endpoint type */
            /* No Data Toggle Reset */
            new_dev_ptr->U2XEPCTRL[xd_ptr->EP_NUM] |=
                ((xd_ptr->BDIRECTION ? (EHCI_EPCTRL_TX_ENABLE ) :   (EHCI_EPCTRL_RX_ENABLE )) |
                 (xd_ptr->EP_TYPE << (xd_ptr->BDIRECTION ?  EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT)));

        }


        else
        {

            /* Enable the endpoint for Rx and Tx and set the endpoint type */
            /* Reset Data Toggle */

            new_dev_ptr->U2XEPCTRL[xd_ptr->EP_NUM] |= ((xd_ptr->BDIRECTION ? (EHCI_EPCTRL_TX_ENABLE |  EHCI_EPCTRL_TX_DATA_TOGGLE_RST) :   (EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST)) |
                    (xd_ptr->EP_TYPE << (xd_ptr->BDIRECTION ?  EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT)));

        }

        /* Note: Disabled endpoint must be changed from default control type
        */

        if (( new_dev_ptr->U2XEPCTRL[xd_ptr->EP_NUM] &  EHCI_EPCTRL_RX_ENABLE) == 0)
            new_dev_ptr->U2XEPCTRL[xd_ptr->EP_NUM] |=   (USB_BULK_ENDPOINT << EHCI_EPCTRL_RX_EP_TYPE_SHIFT);

        if (( new_dev_ptr->U2XEPCTRL[xd_ptr->EP_NUM]&   EHCI_EPCTRL_TX_ENABLE) == 0)
            new_dev_ptr->U2XEPCTRL[xd_ptr->EP_NUM] |=  (USB_BULK_ENDPOINT << EHCI_EPCTRL_TX_EP_TYPE_SHIFT);


    }
    else
    {
#ifdef _DEVICE_DEBUG_
        //  bbu_printf("usb_dci_vusb20_init_endpoint, error ep init.\r\n");
#endif
        return USBERR_EP_INIT_FAILED;
    } /* Endif */

#ifdef _DEVICE_DEBUG_
    // bbu_printf("usb_dci_vusb20_init_endpoint, SUCCESSFUL.\r\n");

#endif

    return USB_OK;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_get_transfer_status
*  Returned Value : USB_OK or error code
*  Comments       :
*        Gets the status of a transfer
*
*END*-----------------------------------------------------------------*/
uint_8 usb_dci_vusb20_get_transfer_status
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr;
    XD_STRUCT_PTR                                xd_ptr;
    uint_8                                       status;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_get_transfer_status\r\n");
#endif

    /* Unlink the dTD */
    dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[2 * ep_num + direction];

    if (dTD_ptr)
    {
        /* Get the transfer descriptor for the dTD */
        xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
        status = xd_ptr->BSTATUS;
    }
    else
    {
        status = USB_STATUS_IDLE;
    } /* Endif */

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_dci_vusb20_get_transfer_status, SUCCESSFUL\r\n");
#endif

    return (status);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_dci_vusb20_get_transfer_details
*  Returned Value : pointer to structure that has details for transfer
*        Gets the status of a transfer
*
*END*-----------------------------------------------------------------*/
XD_STRUCT_PTR  usb_dci_vusb20_get_transfer_details
(
    /* [IN] the USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

    MAVL_USB_TypeDef_PTR                          new_dev_ptr;//by tongchao
    VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr, temp_dTD_ptr;
    XD_STRUCT_PTR                                xd_ptr;
    uint_32                                      temp, remaining_bytes;
    VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;


    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    new_dev_ptr = (MAVL_USB_TypeDef_PTR)usb_dev_ptr->NEVO_DEV_PTR;
    temp = (2 * ep_num + direction);

    /* get a pointer to QH for this endpoint
    */


    ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)new_dev_ptr->U2XEPLISTADDR + temp;
#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_get_transfer_status");
#endif

    /* Unlink the dTD */
    dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[2 * ep_num + direction];

    if (dTD_ptr)
    {

        /* Get the transfer descriptor for the dTD */
        xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
        if (!xd_ptr) return NULL;

        /* Initialize the transfer length field */
        xd_ptr->WSOFAR = 0;
        remaining_bytes = 0;

        /*if length of this transfer is greater than 20K
        we have multiple DTDs to count */
        if (xd_ptr->WTOTALLENGTH > VUSB_EP_MAX_LENGTH_TRANSFER)
        {
            /* it is a valid DTD. We should parse all DTDs for this XD
            and find the total bytes used so far */
            temp_dTD_ptr = dTD_ptr;

            /*loop through the list of DTDS until an active DTD is found
            or list has finished */
            while (!(dTD_ptr->NEXT_TR_ELEM_PTR & VUSBHS_TD_NEXT_TERMINATE))
            {

                /**********************************************************
                If this DTD has been overlayed, we take the actual length
                from QH.
                **********************************************************/

                if ((uint_32)(ep_queue_head_ptr->CURR_DTD_PTR & VUSBHS_TD_ADDR_MASK) ==
                        (uint_32)temp_dTD_ptr)
                {
                    remaining_bytes +=
                        ((ep_queue_head_ptr->SIZE_IOC_INT_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);
                }
                else
                {
                    /* take the length from DTD itself */
                    remaining_bytes +=
                        ((temp_dTD_ptr->SIZE_IOC_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);
                }

                dTD_ptr = temp_dTD_ptr;

                /* Get the address of the next dTD */
                temp_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)
                               (temp_dTD_ptr->NEXT_TR_ELEM_PTR & VUSBHS_TD_ADDR_MASK);
            }
            xd_ptr->WSOFAR = xd_ptr->WTOTALLENGTH - remaining_bytes;

        }
        else
        {
            /*look at actual length from QH*/
            xd_ptr->WSOFAR = xd_ptr->WTOTALLENGTH -
                             ((ep_queue_head_ptr->SIZE_IOC_INT_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);

        }

    }
    else
    {
        xd_ptr = NULL;
    } /* Endif */

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_dci_vusb20_get_transfer_status, SUCCESSFUL");
#endif

    return (xd_ptr);

} /* EndBody */


#if 0
void usb_dump(void)
{
    bbu_printf("MAVL_USB_OTG->U2XID is 0x%x\r\n", MAVL_USB_OTG->U2XID);
    bbu_printf("MAVL_USB_OTG->U2XHWGENERAL is 0x%x\r\n", MAVL_USB_OTG->U2XHWGENERAL);
    bbu_printf("MAVL_USB_OTG->U2XHWHOST is 0x%x\r\n", MAVL_USB_OTG->U2XHWHOST);
    bbu_printf("MAVL_USB_OTG->U2XHWDEVICE is 0x%x\r\n", MAVL_USB_OTG->U2XHWDEVICE);
    bbu_printf("MAVL_USB_OTG->U2XHWTXBUF is 0x%x\r\n", MAVL_USB_OTG->U2XHWTXBUF);
    bbu_printf("MAVL_USB_OTG->U2XHWRXBUF is 0x%x\r\n", MAVL_USB_OTG->U2XHWRXBUF);
    bbu_printf("MAVL_USB_OTG->U2XGPTIMER0LD is 0x%x\r\n", MAVL_USB_OTG->U2XGPTIMER0LD);
    bbu_printf("MAVL_USB_OTG->U2XGPTIMER0CTRL is 0x%x\r\n", MAVL_USB_OTG->U2XGPTIMER0CTRL);
    bbu_printf("MAVL_USB_OTG-> isU2XGPTIMER1LD 0x%x\r\n", MAVL_USB_OTG->U2XGPTIMER1LD);
    bbu_printf("MAVL_USB_OTG-> U2XGPTIMER1CTRL is 0x%x\r\n", MAVL_USB_OTG->U2XGPTIMER1CTRL);
    bbu_printf("MAVL_USB_OTG->U2XSBUSCFG is 0x%x\r\n", MAVL_USB_OTG->U2XSBUSCFG);
    bbu_printf("MAVL_USB_OTG->U2XCAPLENGTH is 0x%x\r\n", MAVL_USB_OTG->U2XCAPLENGTH);
    bbu_printf("MAVL_USB_OTG->U2XHCIVERSION is 0x%x\r\n", MAVL_USB_OTG->U2XHCIVERSION);
    bbu_printf("MAVL_USB_OTG->U2XHCSPARAMS is 0x%x\r\n", MAVL_USB_OTG->U2XHCSPARAMS);
    bbu_printf("MAVL_USB_OTG->U2XHCCPARAMS is 0x%x\r\n", MAVL_USB_OTG->U2XHCCPARAMS);
    bbu_printf("MAVL_USB_OTG->U2XDCIVERSION is 0x%x\r\n", MAVL_USB_OTG->U2XDCIVERSION);
    bbu_printf("MAVL_USB_OTG->U2XDCCPARAMS is 0x%x\r\n", MAVL_USB_OTG->U2XDCCPARAMS);
    bbu_printf("MAVL_USB_OTG-> U2XUSBCMDis 0x%x\r\n", MAVL_USB_OTG->U2XUSBCMD);
    bbu_printf("MAVL_USB_OTG->U2XUSBSTS is 0x%x\r\n", MAVL_USB_OTG->U2XUSBSTS);
    bbu_printf("MAVL_USB_OTG->U2XUSBINTR is 0x%x\r\n", MAVL_USB_OTG->U2XUSBINTR);
    bbu_printf("MAVL_USB_OTG->U2XFRINDEX is 0x%x\r\n", MAVL_USB_OTG->U2XFRINDEX);
    bbu_printf("MAVL_USB_OTG->U2XDEVICEADDR is 0x%x\r\n", MAVL_USB_OTG->U2XDEVICEADDR);
    bbu_printf("MAVL_USB_OTG->U2XEPLISTADDR is 0x%x\r\n", MAVL_USB_OTG->U2XEPLISTADDR);
    bbu_printf("MAVL_USB_OTG->U2XTTCTRL is 0x%x\r\n", MAVL_USB_OTG->U2XTTCTRL);
    bbu_printf("MAVL_USB_OTG->U2XBURSTSIZE is 0x%x\r\n", MAVL_USB_OTG->U2XBURSTSIZE);
    bbu_printf("MAVL_USB_OTG->U2XTXFILLTUNING is 0x%x\r\n", MAVL_USB_OTG->U2XTXFILLTUNING);
    bbu_printf("MAVL_USB_OTG->U2XICUSB is 0x%x\r\n", MAVL_USB_OTG->U2XICUSB);
    bbu_printf("MAVL_USB_OTG->U2XVIEWPORT is 0x%x\r\n", MAVL_USB_OTG->U2XVIEWPORT);
    bbu_printf("MAVL_USB_OTG->U2XEPNAK is 0x%x\r\n", MAVL_USB_OTG->U2XEPNAK);
    bbu_printf("MAVL_USB_OTG->U2XEPNAKEN is 0x%x\r\n", MAVL_USB_OTG->U2XEPNAKEN);
    bbu_printf("MAVL_USB_OTG->U2XCONFIGFLAG is 0x%x\r\n", MAVL_USB_OTG->U2XCONFIGFLAG);
    bbu_printf("MAVL_USB_OTG->U2XPORTSC[0] is 0x%x\r\n", MAVL_USB_OTG->U2XPORTSC[0]);
    bbu_printf("MAVL_USB_OTG->U2XOTGSC is 0x%x\r\n", MAVL_USB_OTG->U2XOTGSC);
    bbu_printf("MAVL_USB_OTG->U2XUSBMODE is 0x%x\r\n", MAVL_USB_OTG->U2XUSBMODE);
    bbu_printf("MAVL_USB_OTG->U2XEPSETUPSTAT is 0x%x\r\n", MAVL_USB_OTG->U2XEPSETUPSTAT);
    bbu_printf("MAVL_USB_OTG->U2XEPPRIME is 0x%x\r\n", MAVL_USB_OTG->U2XEPPRIME);
    bbu_printf("MAVL_USB_OTG->U2XEPFLUSH is 0x%x\r\n", MAVL_USB_OTG->U2XEPFLUSH);
    bbu_printf("MAVL_USB_OTG->U2XEPSTAT is 0x%x\r\n", MAVL_USB_OTG->U2XEPSTAT);
    bbu_printf("MAVL_USB_OTG->U2XEPCOMPLETE is 0x%x\r\n", MAVL_USB_OTG->U2XEPCOMPLETE);
    bbu_printf("MAVL_USB_OTG->U2XEPCTRL[0]  is 0x%x\r\n", MAVL_USB_OTG->U2XEPCTRL[0] );


}

void dqh_dtdDump(uint_32 epnum)
{
    uint_32 temp = epnum * 2;


    VUSB20_EP_QUEUE_HEAD_STRUCT_PTR ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)MAVL_USB_OTG->U2XEPLISTADDR + temp;
    VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr = usb_global_usb_device_state_struct_ptr->EP_DTD_HEADS[temp];
    bbu_printf("ep %d  send\r\n", epnum);
    bbu_printf("ep_queue_head_ptr-BUFF_PTR0 is 0x%x\r\n", ep_queue_head_ptr->BUFF_PTR0);
    bbu_printf("ep_queue_head_ptr-BUFF_PTR1 is 0x%x \r\n", ep_queue_head_ptr->BUFF_PTR1);
    bbu_printf("ep_queue_head_ptr-BUFF_PTR2 is 0x%x\r\n", ep_queue_head_ptr->BUFF_PTR2);
    bbu_printf("ep_queue_head_ptr-BUFF_PTR3 is 0x%x\r\n", ep_queue_head_ptr->BUFF_PTR3);
    bbu_printf("ep_queue_head_ptr-BUFF_PTR4 is 0x%x \r\n", ep_queue_head_ptr->BUFF_PTR4);
    bbu_printf("ep_queue_head_ptr-MAX_PKT_LENGTH is 0x%x\r\n", ep_queue_head_ptr->MAX_PKT_LENGTH);
    bbu_printf("ep_queue_head_ptr- CURR_DTD_PTR is 0x%x\r\n", ep_queue_head_ptr->CURR_DTD_PTR);
    bbu_printf("ep_queue_head_ptr-NEXT_DTD_PTR is 0x%x\r\n", ep_queue_head_ptr->NEXT_DTD_PTR);
    bbu_printf("ep_queue_head_ptr-SIZE_IOC_INT_STS is 0x%x\r\n", ep_queue_head_ptr->SIZE_IOC_INT_STS);


    bbu_printf("dTD_ptr-NEXT_TR_ELEM_PTR is 0x%x\r\n", dTD_ptr->NEXT_TR_ELEM_PTR);
    bbu_printf("dTD_ptr-BUFF_PTR0 is 0x%x\r\n", dTD_ptr->BUFF_PTR0);
    bbu_printf("dTD_ptr-BUFF_PTR1 is 0x%x\r\n", dTD_ptr->BUFF_PTR1);
    bbu_printf("dTD_ptr-BUFF_PTR2 is 0x%x\r\n", dTD_ptr->BUFF_PTR2);
    bbu_printf("dTD_ptr-BUFF_PTR3 is 0x%x\r\n", dTD_ptr->BUFF_PTR3);
    bbu_printf("dTD_ptr-BUFF_PTR3 is 0x%x\r\n", dTD_ptr->SIZE_IOC_STS);

    temp++;
    ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)MAVL_USB_OTG->U2XEPLISTADDR + temp;

    bbu_printf("ep %d receive\r\n", epnum);
    bbu_printf("ep_queue_head_ptr-BUFF_PTR0 is 0x%x\r\n", ep_queue_head_ptr->BUFF_PTR0);
    bbu_printf("ep_queue_head_ptr-BUFF_PTR1 is 0x%x\r\n", ep_queue_head_ptr->BUFF_PTR1);
    bbu_printf("ep_queue_head_ptr-BUFF_PTR2 is 0x%x\r\n", ep_queue_head_ptr->BUFF_PTR2);
    bbu_printf("ep_queue_head_ptr-BUFF_PTR3 is 0x%x \r\n", ep_queue_head_ptr->BUFF_PTR3);
    bbu_printf("ep_queue_head_ptr-BUFF_PTR4 is 0x%x \r\n", ep_queue_head_ptr->BUFF_PTR4);
    bbu_printf("ep_queue_head_ptr-MAX_PKT_LENGTH is 0x%x\r\n", ep_queue_head_ptr->MAX_PKT_LENGTH);
    bbu_printf("ep_queue_head_ptr- CURR_DTD_PTR is 0x%x\r\n", ep_queue_head_ptr->CURR_DTD_PTR);
    bbu_printf("ep_queue_head_ptr-NEXT_DTD_PTR is 0x%x\r\n", ep_queue_head_ptr->NEXT_DTD_PTR);
    bbu_printf("ep_queue_head_ptr-SIZE_IOC_INT_STS is 0x%x\r\n", ep_queue_head_ptr->SIZE_IOC_INT_STS);

    dTD_ptr = usb_global_usb_device_state_struct_ptr->EP_DTD_HEADS[temp] ;

    bbu_printf("dTD_ptr-NEXT_TR_ELEM_PTR is 0x%x\r\n", dTD_ptr->NEXT_TR_ELEM_PTR);
    bbu_printf("dTD_ptr-BUFF_PTR0 is 0x%x\r\n", dTD_ptr->BUFF_PTR0);
    bbu_printf("dTD_ptr-BUFF_PTR1 is 0x%x\r\n", dTD_ptr->BUFF_PTR1);
    bbu_printf("dTD_ptr-BUFF_PTR2 is 0x%x\r\n", dTD_ptr->BUFF_PTR2);
    bbu_printf("dTD_ptr-BUFF_PTR3 is 0x%x\r\n", dTD_ptr->BUFF_PTR3);
    bbu_printf("dTD_ptr-BUFF_PTR3 is 0x%x\r\n", dTD_ptr->SIZE_IOC_STS);




}
#endif
USB_DEV_STATE_STRUCT_PTR getUSBhandle(void)
{

    return usb_global_usb_device_state_struct_ptr;

}



