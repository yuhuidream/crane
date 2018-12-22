#ifndef __APBAUX_H__
#define __APBAUX_H__

typedef struct
{

   __IO uint32_t         COEL_APB_MSA_BOOT;
   __IO uint32_t         RESERVED0;
   __IO uint32_t         APB_RF_CONT_MUX;
   __IO uint32_t         COEL_APB_CLK_GATE;
   __IO uint32_t         COEL_APB_GSM_WB_CHOOSE;
   __IO uint32_t         COEL_APB_RTU_CLKEN;
   __IO uint32_t         COEL_APB_SPARE1;
   __IO uint32_t         COEL_APB_SPARE2;
   __IO uint32_t         COEL_APB_CONTROL;//0X0020
   __IO uint32_t         RESERVED1[(0x20000-0x00020)/4-1];
   __IO uint32_t         GSRAM_CP_AP_SEL;//0X20000
   __IO uint32_t         RESERVED2[2];
   __IO uint32_t         SENSOR_V18_OUT_REG;

}APBAUX_TypeDef;

#define  APBAUX_BASE  0xD4070000

#define  APBAUX  (( APBAUX_TypeDef *) APBAUX_BASE )

//Continuity Multiplexer Register
#define  APB_RF_CONT_MUX_RF_CONT_MUX_9                                   BIT_9
#define  APB_RF_CONT_MUX_RF_CONT_MUX_8                                   BIT_8
#define  APB_RF_CONT_MUX_RF_CONT_MUX_7                                   BIT_7
#define  APB_RF_CONT_MUX_RF_CONT_MUX_6                                   BIT_6
#define  APB_RF_CONT_MUX_RF_CONT_MUX_5                                   BIT_5
#define  APB_RF_CONT_MUX_RF_CONT_MUX_4                                   BIT_4
#define  APB_RF_CONT_MUX_RF_CONT_MUX_3                                   BIT_3
#define  APB_RF_CONT_MUX_RF_CONT_MUX_2                                   BIT_2
#define  APB_RF_CONT_MUX_RF_CONT_MUX_1                                   BIT_1
#define  APB_RF_CONT_MUX_RF_CONT_MUX_0                                   BIT_0


//APB spare register 1
#define  COEL_APB_SPARE1_cg_sw_en
#define  COEL_APB_SPARE1_shutdown_clk_around_rst_param
#define  COEL_APB_SPARE1_shutdown_clk_around_rst_param
#define  COEL_APB_SPARE1_shutdown_clk_around_rst_bypass_ctrl
#define  COEL_APB_SPARE1_apb_gb_l2_ctrl_sel
#define  COEL_APB_SPARE1_apb_gb_l2_ratio
#define  COEL_APB_SPARE1_apb_gb_l2_latency
#define  COEL_APB_SPARE1_apb_gb_l2_latency


//APB spare register 2
#define  COEL_APB_SPARE2_gb_gater_delay_MSK                    SHIFT3(0x3f)       
#define  COEL_APB_SPARE2_gb_gater_delay_BASE                          3
#define  COEL_APB_SPARE2_gb_gater_sw_ctrl_MSK                    SHIFT1(0x3)
#define  COEL_APB_SPARE2_gb_gater_sw_ctrl_BASE                    1
#define  COEL_APB_SPARE2_gbm_gater_sw_ctrl                    BIT_0                   


//APB Control Register
#define  COEL_APB_CONTROL_APB_EXT_CYCLE_NUM_MSK                      SHIFT3(0x1f)
#define  COEL_APB_CONTROL_APB_EXT_CYCLE_NUM_BASE                          3
#define  COEL_APB_CONTROL_COEL_APB_EXT_IF2                         BIT_2
#define  COEL_APB_CONTROL_COEL_APB_EXT_IF1_MSK                     SHIFT0(0x3)
#define  COEL_APB_CONTROL_COEL_APB_EXT_IF1_BASE                       0

#endif
