#ifndef H_PCIE_PHY_H
#define H_PCIE_PHY_H

typedef struct{
    __IO uint32_t   CHIP_ID;            //0x0
    __IO uint32_t   POWER_REG0;         //0x4
    __IO uint32_t   CAL_REG0;           //0x8, KVCO Calibration Control
    __IO uint32_t   CAL_REG1;           //0xC, Impedance Calibration Control
    __IO uint32_t   CAL_REG2;           //0x10, Impedance and TX SSC Control
    __IO uint32_t   RESERVED0;          //0x14
    __IO uint32_t   RX_REG0;            //0x18, Squelch and FFE Setting
    __IO uint32_t   DFE_REG0;           //0x1C, DFE Resolution Control
    __IO uint32_t   DFE_REG1;           //0x20, DFE Update Control
    __IO uint32_t   DFE_REG2;           //0x24, DFE F0-F2 Coefficient Control
    __IO uint32_t   DFE_REG3;           //0x28, DFE F3-F5 Coefficient Control
    __IO uint32_t   DFE_REG4;           //0x2C, Adapted DFE Coefficient 0
    __IO uint32_t   DFE_REG5;           //0x30, Adapted DFE Coefficient 1
    __IO uint32_t   G1_SETTING0;        //0x34, Genration 1 setting 0
    __IO uint32_t   G1_SETTING1;        //0x38, Genration 1 setting 1
    __IO uint32_t   G2_SETTING0;        //0x3C, Genration 2 setting 0
    __IO uint32_t   G2_SETTING1;        //0x40, Genration 2 setting 1
    __IO uint32_t   G3_SETTING0;        //0x44, Genration 3 setting 0
    __IO uint32_t   G3_SETTING1;        //0x48, Genration 3 setting 1
    __IO uint32_t   G4_SETTING0;        //0x4C, Genration 4 setting 0
    __IO uint32_t   G4_SETTING1;        //0x50, Genration 4 setting 1
    __IO uint32_t   PHYTST_REG0;        //0x54, PHY Test Control 0
    __IO uint32_t   PHYTST_DATA_REG0;   //0x58, PHY Test Data 0
    __IO uint32_t   PHYTST_DATA_REG1;   //0x5C, PHY Test Data 1
    __IO uint32_t   PHYTST_DATA_REG2;   //0x60, PHY Test Data 2
    __IO uint32_t   PHYTST_DATA_REG3;   //0x64, PHY Test Data 3
    __IO uint32_t   PHYTST_DATA_REG4;   //0x68, PHY Test Data 4
    __IO uint32_t   PHYTST_DATA_REG5;   //0x6C, PHY Test Data 5
    __IO uint32_t   PHYTST_PRBSCNT_REG0;    //0x70, PHY Test PRBS Counter 0
    __IO uint32_t   PHYTST_PRBSCNT_REG1;    //0x74, PHY Test PRBS Counter 1
    __IO uint32_t   PHYTST_PRBSCNT_REG2;    //0x78, PHY Test PRBS Counter 2
    __IO uint32_t   PHYTST_ERRCNT_REG0;     //0x7C, PHY Test PRBS Error Counter 0
    __IO uint32_t   PHYTST_ERRCNT_REG1;     //0x80, PHY Test PRBS Error Counter 1
    __IO uint32_t   PHYTST_OOB_REG0;        //0x84, PHY Test OOB 0
    __IO uint32_t   RESERVED1;              //0x88
    __IO uint32_t   LOOPBACK_REG0;          //0x8C, Digital Loopback Enable
    __IO uint32_t   INTERFACE_REG0;         //0x90, Sync Pattern
    __IO uint32_t   INTERFACE_REG1;         //0x94, Sync Mask and Maximal Generation Setting
    __IO uint32_t   ISOLATE_REG0;           //0x98, PHY Isolation Mode Control
    __IO uint32_t   EOM_REG1;               //0x9C, EOM Circuit Control
    __IO uint32_t   EOM_SET1_REG[18];       //0xA0~E4, EOM Set 1 Control 0~17
    __IO uint32_t   RESERVED2;              //0xE8
    __IO uint32_t   DFE_REG20;              //0xEC, DFE step size register 0
    __IO uint32_t   DFE_REG21;              //0xF0, DFE step size register 1
    __IO uint32_t   G1_SETTING2;            //0xF4, Genration 1 setting 2
    __IO uint32_t   G2_SETTING2;            //0xF8, Genration 2 setting 2
    __IO uint32_t   G3_SETTING2;            //0xFC, Genration 3 setting 2
    __IO uint32_t   G4_SETTING2;            //0x100, Genration 4 setting 2
    __IO uint32_t   RESERVED3[8];           //0x104~0x120
    __IO uint32_t   LANE_ALIGN_REG0;        //0x124, Lane Alignment Control
    __IO uint32_t   RESERVED4[5];           //0x128~0x138
    __IO uint32_t   MISC_REG0;              //0x13C, Misc Control 0
    __IO uint32_t   RESERVED5;              //0x140
    __IO uint32_t   INTERFACE_REG2;         //0x144, Interface and Digital Reference Clock Control
    __IO uint32_t   RESERVED6[15];          //0x148~0x180
    __IO uint32_t   RX_REG2;                //0x184, DTL Control
    __IO uint32_t   RESERVED7[5];           //0x188~0x198
    __IO uint32_t   DFE_REG6;               //0x19C, DFE Miscellaneous Control
    __IO uint32_t   RESERVED8[2];           //0x1A0~0x1A4
    __IO uint32_t   DFE_REG9;               //0x1A8, Error Counter Constraint Control
    __IO uint32_t   DFE_REG10;              //0x1AC, DFE Timer
    __IO uint32_t   RESERVED9[5];           //0x1B0~0x1C0
    __IO uint32_t   PLLCAL_REG1;            //0x1C4, PLL Calibration Control 1
    __IO uint32_t   SQ_REG0;                //0x1C8, squelch glitch filter control
    __IO uint32_t   MISC_REG1;              //0x1CC, Misc Control 1
    __IO uint32_t   MISC_REG2;              //0x1D0, Misc Control 2
    __IO uint32_t   RESERVED10[2];          //0x1D4~0x1D8
    __IO uint32_t   CLK150M_REG2;           //0x1DC, clk150M control register 2
    __IO uint32_t   RESERVED11[10];         //0x1E0~0x204
    __IO uint32_t   DFE_REG13;              //0x208, DFE f1p5 and f2p5 control and other
    __IO uint32_t   FFE_REG1;               //0x20C, FFE control register
    __IO uint32_t   RESERVED12[7];          //0x210~0x228
    __IO uint32_t   TRX_TRAIN_REG0;         //0x22C, TX training control register 0
    __IO uint32_t   TRX_TRAIN_REG1;         //0x230, TX training control register 1
    __IO uint32_t   RESERVED13[3];          //0x234~0x23C
    __IO uint32_t   RX_TRAIN_REG3;          //0x240, RX training control register 3
    __IO uint32_t   RESERVED14[9];          //0x244~0x264
    __IO uint32_t   TX_TRAIN_REG0;          //0x268, TX training control register 0
    __IO uint32_t   RESERVED15;             //0x26C
    __IO uint32_t   TRX_TRAIN_REG2;         //0x270, TRX training control register 2
    __IO uint32_t   RESERVED16;             //0x274
    __IO uint32_t   TRX_TRAIN_REG4;         //0x278, TRX training control register 4
    __IO uint32_t   RESERVED17[6];          //0x27C~0x290
    __IO uint32_t   TX_REG5;                //0x294, tx_amp_adj and tx_emph_en
    __IO uint32_t   RESERVED18;             //0x298
    __IO uint32_t   TX_REG7;                //0x29C, tx amplitude index limit
    __IO uint32_t   TX_REG8;                //0x2A0, tx emphasis index mininum value
    __IO uint32_t   TRX_TRAIN_REG5;         //0x2A4, TRX training control register 5
    __IO uint32_t   RESERVED19;             //0x2A8
    __IO uint32_t   INTERRUPT_REG1;         //0x2AC
    __IO uint32_t   RESERVED20;             //0x2B0
    __IO uint32_t   INTERRUPT_REG3;         //0x2B4
    __IO uint32_t   RESERVED21;             //0x2B8
    __IO uint32_t   INTERRUPT_REG5;         //0x2BC
    __IO uint32_t   RESERVED22[8];          //0x2C0~0x2DC
    __IO uint32_t   TO_ANA_REG0;            //0x2E0, value of signals to analog selection
    __IO uint32_t   RESERVED23[12];         //0x2E4~0x310
    __IO uint32_t   TX_TRAIN_REG3;          //0x314, TX train result
    __IO uint32_t   DFE_REG15;              //0x318, adapted edge result
    __IO uint32_t   TX_TRAIN_REG4;          //0x31C, TX Train Preset Register
    __IO uint32_t   RESERVED24[19];         //0x320~0x368
    __IO uint32_t   TRX_TRAIN_REG6;         //0x36C, TRX training control register 6
    __IO uint32_t   RESERVED25[16];         //0x370~0x3AC
    __IO uint32_t   DFE_REG22;              //0x3B0, DFE step size register 2
    __IO uint32_t   RESERVED26[11];         //0x3B4~0x3DC
    __IO uint32_t   CLK150M_REG3;           //0x3E0
    __IO uint32_t   RESERVED27[23];         //0x3E4~0x43C
    __IO uint32_t   G1_SETTING3;            //0x440
    __IO uint32_t   G1_SETTING4;            //0x444
    __IO uint32_t   G2_SETTING3;            //0x448
    __IO uint32_t   G2_SETTING4;            //0x44C
    __IO uint32_t   G3_SETTING3;            //0x450
    __IO uint32_t   G3_SETTING4;            //0x454
    __IO uint32_t   G4_SETTING3;            //0x458
    __IO uint32_t   G4_SETTING4;            //0x45C
    __IO uint32_t   RESERVED28[2];          //0x460~0x464
    __IO uint32_t   TX_TRAIN_REG15;         //0x468, PCIE tx preset index
    __IO uint32_t   TX_TRAIN_REG16;         //0x46C, TX power protection control register
    __IO uint32_t   RESERVED29[100];        //0x470~0x5FC
    __IO uint32_t   LANE_CFG0;              //0x600, Lane Configuration 0
    __IO uint32_t   LANE_CFG1;              //0x604, Lane Configuration 1
    __IO uint32_t   LANE_STATUS0;           //0x608, Lane Status 0
    __IO uint32_t   LANE_STATUS1;           //0x60C, Lane Status 1
    __IO uint32_t   LANE_STATUS2;           //0x610, Lane Status 2
    __IO uint32_t   LANE_BEACON;            //0x614, Lane Beacon Control
    __IO uint32_t   LANE_CFG2;              //0x618, Lane Configuration 2
    __IO uint32_t   RESERVED30;             //0x61C
    __IO uint32_t   LANE_CFG4;              //0x620, Lane Configuration 4
    __IO uint32_t   LANE_CFG5;              //0x624, Lane Configuration 5
    __IO uint32_t   LANE_STATUS3;           //0x628, Lane Status 3
    __IO uint32_t   LANE_TXCMN_CFG;         //0x62C, Lane TX Common Mode Configuration
    __IO uint32_t   RESERVED31[52];         //0x630~0x6FC
    __IO uint32_t   GLOB_PHY_REV;           //0x700, PIPE Revision
    __IO uint32_t   GLOB_CLK_CTRL;          //0x704, Reset and Clock Control
    __IO uint32_t   GLOB_TEST_CTRL;         //0x708, Test Mode Control
    __IO uint32_t   GLOB_CLK_SRC_LO;        //0x70C, Clock Source Low
    __IO uint32_t   GLOB_CLK_SRC_HI;        //0x710, Clock Source High
    __IO uint32_t   GLOB_TRIGGER;           //0x714, Trigger
    __IO uint32_t   GLOB_MISC_CTRL;         //0x718, Miscellaneous Control
    __IO uint32_t   GLOB_CLKREQ_CTRL;       //0x71C, CLKREQ# Timing Control
    __IO uint32_t   GLOB_DP_CFG;            //0x720, Datapath Configuration
    __IO uint32_t   RESERVED32[7];          //0x724~0x73C
    __IO uint32_t   GLOB_PM_CFG0;           //0x740, Power Management Timing Parameter 1
    __IO uint32_t   RESERVED33;             //0x744
    __IO uint32_t   GLOB_COUNTER_CTRL;      //0x748, Counter Lane and Type Register
    __IO uint32_t   GLOB_COUNTER_LO;        //0x74C, Counter Low Register
    __IO uint32_t   GLOB_COUNTER_HI;        //0x750, Counter High Register
    __IO uint32_t   RESERVED34;             //0x754
    __IO uint32_t   GLOB_PM_DP_CTRL;        //0x758, PM Datapath and Clock Control Override
    __IO uint32_t   GLOB_LOW_FREQ_CFG;      //0x75C, Low Frequency Configuration Register
    __IO uint32_t   RESERVED35[8];          //0x760~0x77C
    __IO uint32_t   GLOB_BIST_CTRL;         //0x780, BIST Control Input
    __IO uint32_t   GLOB_BIST_CFG;          //0x784, BIST Configuration Input
    __IO uint32_t   GLOB_BIST_DATA;         //0x788, BIST Data Input
    __IO uint32_t   GLOB_BIST_LANE_TYPE;    //0x78C, BIST LANE and TYPE
    __IO uint32_t   GLOB_BIST_START;        //0x790, BIST Loggin Window Start Cycle
    __IO uint32_t   GLOB_BIST_LENGTH;       //0x794, BIST Loggin Window Length
    __IO uint32_t   GLOB_BIST_MASK_LO;      //0x798, BIST Result Mask Low
    __IO uint32_t   GLOB_BIST_MASK_HI;      //0x79C, BIST Result Mask High
    __IO uint32_t   GLOB_BIST_RESULT_LO;    //0x7A0, BIST Result Low
    __IO uint32_t   GLOB_BIST_RESULT_HI;    //0x7A4, BIST Result High
    __IO uint32_t   GLOB_BIST_SEQR_CFG;     //0x7A8, BIST Sequencer Configuration
}PCIE_PHY_TypeDef;

#define PCIE_PHY_BASE 0xD420A000

#define PCIE_PHY (( PCIE_PHY_TypeDef *) PCIE_PHY_BASE )

#endif