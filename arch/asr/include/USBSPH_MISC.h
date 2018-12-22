#ifndef __USBSPH_H__
#define __USBSPH_H__

typedef struct
{
   __IO uint32_t RESERVED0;
   __IO uint32_t USBSPH_MISC;//0X04
   __IO uint32_t RESERVED1[(0x28-0x04)/4-1];
   __IO uint32_t USBSPH_INT;//0X28
   __IO uint32_t RESERVED2[(0x34-0x28)/4-1];
   __IO uint32_t USBSPH_MISC_CTRL;//0X34

}USBSPH_MISC_TypeDef;




//#define    USBSPH_MISC_BASE      0xC0000000
#define    USBSPH_MISC_BASE      0xD1051000      //FOR NEZHA3

#define    USB_MISC_SPH  (( USBSPH_MISC_TypeDef *) USBSPH_MISC_BASE )


//Bitfield Constants Register USBSPH_MISC
#define MISC_BG_VSEL_MASK                          SHIFT21(0x3)
#define MISC_BG_VSEL_SHIFT                         21
#define MISC_PU_MISC                               BIT_20


//Bitfield Constants Register USBSPH_INT
#define INT_RESUME_INT_MASK                       0x00000008
#define INT_RESUME_INT_SHIFT                      3
#define INT_LINE_INT_EN_MASK                      0x00000002
#define INT_LINE_INT_EN_SHIFT                     1
#define INT_INTERRUPT_MASK                        0x00000001
#define INT_INTERRUPT_SHIFT                       0



//Bitfield Constants Register USBSPH_CTRL
#define MISC_CTRL_OTG_CLK480_READY						BIT_31
#define MISC_CTRL_pll_lock_bypass                       BIT_15
#define MISC_CTRL_pll_lock                              BIT_14
#define MISC_CTRL_xcvr_udrdis                           BIT_13
#define MISC_CTRL_xcvr_swdis                            BIT_12
#define MISC_CTRL_icusb_suspendm_en						BIT_8
#define MISC_CTRL_icusb_in_use_cp						BIT_7
#define MISC_CTRL_CLKSEL                                BIT_6
#define MISC_CTRL_PDWN_EN                               BIT_5
#define MISC_CTRL_RTC_MASK                              SHIFT3(0x3)
#define MISC_CTRL_RTC_SHIFT                             3
#define MISC_CTRL_WTC_MASK                              SHIFT1(0x3)
#define MISC_CTRL_WTC_SHIFT                             1
#define MISC_CTRL_PDWN_MASK                             0x00000001
#define MISC_CTRL_PDWN_SHIFT                            0







#endif
