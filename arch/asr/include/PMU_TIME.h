#ifndef __PMU_TIME_REG_H__
#define __PMU_TIME_REG_H__

typedef struct
{
    __IO uint32_t CCR;
    __IO uint32_t Tn_Mm[3][3];//0x04, index is 0x04+((3n+m)<<2)
    __IO uint32_t CRn[3];//0x28
    __IO uint32_t SRn[3];//0X34
    __IO uint32_t IERn[3];//0X40
    __IO uint32_t PLVRn[3];
    __IO uint32_t PLCRn[3];
    __IO uint32_t WMER;
    __IO uint32_t WMR;
    __IO uint32_t WVR;
    __IO uint32_t WSR;
    __IO uint32_t ICRn[3];
    __IO uint32_t WICR;
    __IO uint32_t CER;
    __IO uint32_t CMR;
    __IO uint32_t ILRn[3]; //use 3 here to occupy the address
    __IO uint32_t WCR;
    __IO uint32_t WFAR;
    __IO uint32_t WSAR;
    __IO uint32_t CVWRn[3];

}PMU_TIME_TypeDef;

#define     TIME_PMU_BASE   0xD4080000
#define     TIME_PMU        (( PMU_TIME_TypeDef *) TIME_PMU_BASE )

//Bitfield Constants Register TMR_CCR
#define CCR_CS_2_MASK                             0x00000060
#define CCR_CS_2_SHIFT                            5
#define CCR_CS_1_MASK                             0x0000000C
#define CCR_CS_1_SHIFT                            2
#define CCR_CS_0_MASK                             0x00000003
#define CCR_CS_0_SHIFT                            0



//Bitfield Constants Register TMR_SRn
#define SRN_M2_MASK                               0x00000004
#define SRN_M2_SHIFT                              2
#define SRN_M1_MASK                               0x00000002
#define SRN_M1_SHIFT                              1
#define SRN_M0_MASK                               0x00000001
#define SRN_M0_SHIFT                              0

//Bitfield Constants Register TMR_IERn
#define IERN_IE2_MASK                              0x00000004
#define IERN_IE2_SHIFT                             2
#define IERN_IE1_MASK                              0x00000002
#define IERN_IE1_SHIFT                             1
#define IERN_IE0_MASK                              0x00000001
#define IERN_IE0_SHIFT                             0


//Bitfield Constants Register TMR_PLCRn
#define PLCRN_MCS_MASK                              0x00000003
#define PLCRN_MCS_SHIFT                             0


//Bitfield Constants Register TMR_WMER
#define WMER_WRIE_MASK                             0x00000002
#define WMER_WRIE_SHIFT                            1
#define WMER_WE_MASK                               0x00000001
#define WMER_WE_SHIFT                              0


//Bitfield Constants Register TMR_WMR
#define WMR_WTM_MASK                              0x0000FFFF
#define WMR_WTM_SHIFT                             0



//Bitfield Constants Register TMR_WVR
#define WVR_WTV_MASK                              0x0000FFFF
#define WVR_WTV_SHIFT                             0


//Bitfield Constants Register TMR_WSR
#define WSR_WTS_MASK                              0x00000001
#define WSR_WTS_SHIFT                             0




//Bitfield Constants Register TMR_ICRn
#define ICRN_TCLR2_MASK                            0x00000004
#define ICRN_TCLR2_SHIFT                           2
#define ICRN_TCLR1_MASK                            0x00000002
#define ICRN_TCLR1_SHIFT                           1
#define ICRN_TCLR0_MASK                            0x00000001
#define ICRN_TCLR0_SHIFT                           0




//Bitfield Constants Register WICR
#define WICR_WICLR_MASK                            0x00000001
#define WICR_WICLR_SHIFT                           0


//Bitfield Constants Register TMR_CER
#define CER_T2EN_MASK                             0x00000004
#define CER_T2EN_SHIFT                            2
#define CER_T1EN_MASK                             0x00000002
#define CER_T1EN_SHIFT                            1
#define CER_T0EN_MASK                             0x00000001
#define CER_T0EN_SHIFT                            0





//Bitfield Constants Register TMR_CMR
#define CMR_T2MODE_MASK                           0x00000004
#define CMR_T2MODE_SHIFT                          2
#define CMR_T1MODE_MASK                           0x00000002
#define CMR_T1MODE_SHIFT                          1
#define CMR_T0MODE_MASK                           0x00000001
#define CMR_T0MODE_SHIFT                          0



//Bitfield Constants Register TMR_ILRn
#define ILRN_TNIL2_MASK                            0x00000004
#define ILRN_TNIL2_SHIFT                           2
#define ILRN_TNIL1_MASK                            0x00000002
#define ILRN_TNIL1_SHIFT                           1
#define ILRN_TNIL0_MASK                            0x00000001
#define ILRN_TNIL0_SHIFT                           0



//Bitfield Constants Register TMR_WCR
#define WCR_WCR_MASK                              0x00000001
#define WCR_WCR_SHIFT                             0



//Bitfield Constants Register TMR_WFAR
#define WFAR_KEY_MASK                              0x0000FFFF
#define WFAR_KEY_SHIFT                             0


//Bitfield Constants Register TMR_WSAR
#define WSAR_KEY_MASK                              0x0000FFFF
#define WSAR_KEY_SHIFT                             0


//Bitfield Constants Register TMR_CVWRn
#define CVWRN_TCVWR_MASK                            0xFFFFFFFF
#define CVWRN_TCVWR_SHIFT                           0




//Bitfield Constants Register GEN_TMR_CFG
#define GEN_TMR_CFG_AUTO_CORRECTION_DIS_MASK              0x00000080
#define GEN_TMR_CFG_AUTO_CORRECTION_DIS_SHIFT             7
#define GEN_TMR_CFG_GT_CLK_SEL_MASK                       0x00000030
#define GEN_TMR_CFG_GT_CLK_SEL_SHIFT                      4
#define GEN_TMR_CFG_GT_LD_ON_ENA_MASK                     0x00000002
#define GEN_TMR_CFG_GT_LD_ON_ENA_SHIFT                    1
#define GEN_TMR_CFG_GT_CNT_ENA_MASK                       0x00000001
#define GEN_TMR_CFG_GT_CNT_ENA_SHIFT                      0


//Bitfield Constants Register GEN_TMR_RR
#define GEN_TMR_RR_RESET_MASK                            0x00000001
#define GEN_TMR_RR_RESET_SHIFT                           0

//Bitfield Constants Register GEN_TMR_LD2
#define GEN_TMR_LD2_LD_VAL2_MASK                          0x007FFFFF
#define GEN_TMR_LD2_LD_VAL2_SHIFT                         0

//Bitfield Constants Register GEN_TMR_CNTR2
#define GEN_TMR_CNTR2_Counter_55_32_MASK                  0x007FFFFF
#define GEN_TMR_CNTR2_Counter_55_32_SHIFT                 0

#endif
