#ifndef __APBSPARE_H__
#define __APBSPARE_H__

typedef struct
{
   __IO uint32_t       APB_SPARE0;
   __IO uint32_t       RESERVED0[2];
   __IO uint32_t       SENSOR_V18;
   __IO uint32_t       RECERVED1[(0x100-0x0C)/4-1];
   __IO uint32_t       PLL1_SW_CTRL;//0X100
   __IO uint32_t       PLL2_SW_CTRL;
   __IO uint32_t       PLL2_SW_CTRL2;
   __IO uint32_t       APB_spare4_reg;
   __IO uint32_t       ANA_GRP_SW;
   __IO uint32_t       APB_spare6_reg; //0x114
   __IO uint32_t       APB_spare7_reg; //0x118

}APBSPARE_TypeDef;

#define  APBSPARE_BASE  0xD4090000

#define  APBSPARE  (( APBSPARE_TypeDef *) APBSPARE_BASE )



//APB spare0 Register
#define  SPARE0_CP_PWRSW_DLY              BIT_1
#define  SPARE0_CP_AP_SEL                 BIT_0

//PLL1 SW control Register
//We're not likely to change PLL1 settings, so not list the bit fields here.


//PLL2/PLL3 SW control Register fields are exactly same

#define VDDM_MSK                            SHIFT30(0x3)
#define VDDM_BASE                           30
#define VDDL_MSK                            SHIFT26(0xf)
#define VDDL_BASE                           26
#define VREG_IVREG_MSK                      SHIFT24(0x3)
#define VREG_IVREG_BASE                     24
#define ICP_MSK                             SHIFT21(0x7)
#define ICP_BASE                            21
#define KVCO_MSK                            SHIFT17(0xf)
#define KVCO_BASE                           17
#define VCO_VNRG_MSK                        SHIFT14(0x7)
#define VCO_VNRG_BASE                       14
#define CTUNE_MSK                           SHIFT12(0x3)
#define CTUNE_BASE                          12
#define VCODIV_SEL_DIFF_MSK                 SHIFT8(0xf)
#define VCODIV_SEL_DIFF_BASE                8
#define VCODIV_SEL_SE_MSK                   SHIFT4(0xf)
#define VCODIV_SEL_SE_BASE                  4
#define DIFFCLK_EN                          BIT_3
#define BYPASS_EN                           BIT_2
#define GATE_CLK_CTRL                       BIT_1
#define LINEUP_EN                           BIT_0




#define IC_USB_P_SLP_DI                BIT_12
#define IC_USB_P_PU_EN                 BIT_11
#define IC_USB_P_PD_EN                 BIT_10
#define IC_USB_P_SR_MSK                 SHIFT8(0x3)
#define IC_USB_P_SR_BASE                8

#define IC_USB_N_SLP_DI                    BIT_4
#define IC_USB_N_PU_EN                     BIT_3
#define IC_USB_N_PD_EN                    BIT_2
#define IC_USB_N_SR_MSK                   SHIFT0(0x3)
#define IC_USB_N_SR_BASE                  0


#endif
