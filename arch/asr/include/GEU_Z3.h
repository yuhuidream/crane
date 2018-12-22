#ifndef __GEU_Z3_H__
#define __GEU_Z3_H__

typedef struct
{
    __IO uint32_t      STATUS;
    __IO uint32_t      CONFIG;
    __IO uint32_t      INIT_KEY_VALUE[8];
    __IO uint32_t      INPUT_DATA_ENC_DEC[4];

    union {
        __IO uint32_t      GUE_1ST_OFF_CODE_OCB_MODE[4];
        __IO uint32_t      CBC_INIT_VAL_REG[4];
        __IO uint32_t      FUSE_PROG_VAL1[4];
    };
    union {
        __IO uint32_t      GUE_2ND_OFF_CODE_OCB_MODE[4];
        __IO uint32_t      FUSE_PROG_VAL2[4];
    };
    __IO uint32_t      OUT_DATA_AFTER_ENC_DEC[4];
    __IO uint32_t      SRAM_SPACE_ROUND_KEY_TABLE[64];
    __IO uint32_t      ROM_SPACE_SBOX_TABLE[64];
    __IO uint32_t      ROM_SPACE_INV_SBOX_TABLE[64];
    __IO uint32_t      OTP_DATA1;
    __IO uint32_t      OTP_DATA2;
    __IO uint32_t      OTP_DATA3;
    __IO uint32_t      OTP_DATA4;//0x374
    __IO uint32_t      RESERVED00[(0x3a8 - 0x374) / 4 - 1];
    __IO uint32_t      REGULATOR_CNT_REG;//0x3a8
    __IO uint32_t      FUSE_TEST_FIELD;
    __IO uint32_t      FUSE_SCLK_DIV_CNTR;//0x3b0
    __IO uint32_t      RESERVED0[(0x404 - 0x3b0) / 4 - 1];
    __IO uint32_t      FUSE_VAL_APCFG1;//0x404;
    __IO uint32_t      FUSE_VAL_APCFG2;
    __IO uint32_t      FUSE_VAL_APCFG3;
    __IO uint32_t      FUSE_MANU_PARA_0;
    __IO uint32_t      FUSE_MANU_PARA_1;
    __IO uint32_t      FUSE_MANU_PARA_2;
    __IO uint32_t      AP_CP_MP_ECC;
    __IO uint32_t      BLOCK0_RESERVED_1;
    __IO uint32_t      FUSE_VAL_ROOT_KEY1;
    __IO uint32_t      FUSE_VAL_ROOT_KEY2;
    __IO uint32_t      FUSE_VAL_ROOT_KEY3;
    __IO uint32_t      FUSE_VAL_ROOT_KEY4;
    __IO uint32_t      FUSE_VAL_ROOT_KEY5;
    __IO uint32_t      FUSE_VAL_ROOT_KEY6;
    __IO uint32_t      FUSE_VAL_ROOT_KEY7;
    __IO uint32_t      FUSE_VAL_ROOT_KEY8;
    __IO uint32_t      FUSE_VAL_OEM_HASH_KEY[8];
    __IO uint32_t      FUSE_VAL_OEM_JTAG_HASH[8];
    __IO uint32_t      FUSE_STATUS;
    __IO uint32_t      HW_RANDOM_NUM_GEN;//0X488

    __IO uint32_t      FUSE_VAL_OEM_UID_H;


    __IO uint32_t      BLOCK7_RESERVED_0;
    __IO uint32_t      BLOCK7_RESERVED_1;
    __IO uint32_t      BLOCK7_RESERVED_2;
    __IO uint32_t      BLOCK7_RESERVED_3;
    __IO uint32_t      BLOCK7_RESERVED_4;
    __IO uint32_t      BLOCK7_RESERVED_5;//0X4A4
    __IO uint32_t      FUSE_VAL_OEM_UID_L;//according helan_lte  0x4a8 offset
    __IO uint32_t      RESERVED1[(0x4C0 - 0x4A8) / 4 - 1];
    __IO uint32_t      ECC_CAL;//0X4C0
    __IO uint32_t      ECC_STATUS;
    __IO uint32_t      FUSE_VAL_APCP_ECC;
    __IO uint32_t      RESERVED2[(0x4d4 - 0x4c8) / 4 - 1];
    __IO uint32_t      FUSE_BACK_UP_REG[7];




} GEU_Z3_TypeDef;

#define  GEU_Z3_BASE   0xD4201000

#define  GEU_Z3  (( GEU_Z3_TypeDef *) GEU_Z3_BASE )


//GEU Status Register
#define  STATUS_dout2_done                                   BIT_14
#define  STATUS_dout1_done                                   BIT_13
#define  STATUS_din1_valid                                   BIT_11
#define  STATUS_din0_valid                                   BIT_10
#define  STATUS_DATAISR                                   BIT_9
#define  STATUS_KEYISR                                   BIT_8
#define  STATUS_DATAPROC                                   BIT_5
#define  STATUS_KEYPROC                                   BIT_4
#define  STATUS_DOUTREADY                                   BIT_3
#define  STATUS_KEYDONE                                   BIT_2
#define  STATUS_DINREADY                                   BIT_1
#define  STATUS_KEYREADY                                   BIT_0







/*  GEU_CONFIG                          0x0004  Configuration Register */
/* Enable DMA Mode of AES Cipher */
#define CONFIG_EN_DMA_MODE_AES_CIPHER           BIT_31
#define CONFIG_STICKY_CONTROL_BIT               BIT_30
#define CONFIG_CBC_ECB_MODE                     BIT_29          /* CBC/ECB Mode */
#define CONFIG_FUSE_POWER_BYPASS            BIT_28
#define CONFIG_FUSE_CLOCK_DIVIDER_MSK           SHIFT25(0x7)
#define CONFIG_FUSE_CLOCK_DIVIDER_BASE          25
#define CONFIG_WRITE_INI_VAL_IN_CBC_MODE        BIT_24
#define CONFIG_FUSE_CLOCK_DISABLE               BIT_23
#define CONFIG_FUSE_SOFTWARE_RESET              BIT_22
#define CONFIG_FAB_PATH_SEL                     BIT_21
#define CONFIG_FUSE_BLOCK_NUMBER_MSK            SHIFT18(0x7)
#define CONFIG_FUSE_BLOCK_NUMBER_BASE           18
#define CONFIG_HIGH_VOLT_ENABLE                 BIT_17
#define CONFIG_BURN_FUSE_ENABLE                 BIT_16
#define CONFIG_FUSE_LOCK                        BIT_15          /* Fuse Lock */
#define CONFIG_ENABLE_SOFT_FUSE_PROG            BIT_14
#define CONFIG_AES_KEY_SIZE_SEL                 BIT_13
#define CONFIG_JTAG_CTL_DIS                         BIT_12
#define CONFIG_DATARSR                          BIT_11
#define CONFIG_DATAIMR                          BIT_10
#define CONFIG_KEYRSR                           BIT_9
#define CONFIG_KEYIMR                           BIT_8
#define CONFIG_JTAG_ACCESS_CTRL                 BIT_7
#define CONFIG_DMA_REQ_SEL_SIG                  BIT_6
#define CONFIG_RNDKEYHA                         BIT_5
#define CONFIG_SBOXHA                           BIT_4
#define CONFIG_ENCDEC                           BIT_3
#define CONFIG_OCBBYP                           BIT_2           /* OCB Bypass */
#define CONFIG_KEYSZ_MSK                        SHIFT0(0x3)     /* Key Size */
#define CONFIG_KEYSZ_BASE                       0



/*  GEU_FUSE_STATUS                     0x0484  Fuse Operation Status Register */
/*      Bit(s) GEU_FUSE_STATUS_RSRV_31_30 reserved */
#define FUSE_STATUS_SPARE_FUSE          BIT_29          /* Spare fuse bit */
/*      Bit(s) GEU_FUSE_STATUS_RSRV_28_26 reserved */
#define FUSE_STATUS_LIFE_CYCLE_MSK      SHIFT10(0xffff) /* Life Cycle Fuse Bits */
#define FUSE_STATUS_LIFE_CYCLE_BASE     10
#define FUSE_STATUS_FUSE_READY          BIT_9           /* Fuse Ready */
#define FUSE_STATUS_FUSE_BURN_DONE      BIT_8           /* Fuse Burn Done */
#define FUSE_STATUS_LOCK_BIT_MSK        SHIFT0(0xff)    /* Fuse Lock Bit */
#define FUSE_STATUS_LOCK_BIT_BASE       0


/*  GEU_ECC_STATUS                      0x04C4  ECC Status Register */
#define ECC_STATUS_ERR_TYPE_C_MSK       SHIFT30(0x3)    /* Error Type A */
#define ECC_STATUS_ERR_TYPE_C_BASE      30
#define ECC_STATUS_ERR_TYPE_B_MSK       SHIFT28(0x3)    /* Error Type B */
#define ECC_STATUS_ERR_TYPE_B_BASE      28
#define ECC_STATUS_ERR_TYPE_A_MSK       SHIFT26(0x3)    /* Error Type C */
#define ECC_STATUS_ERR_TYPE_A_BASE      26
#define ECC_STATUS_ERR_TYPE_9_MSK       SHIFT24(0x3)    /* Error Type 9 */
#define ECC_STATUS_ERR_TYPE_9_BASE      24
#define ECC_STATUS_ERR_TYPE_8_MSK       SHIFT22(0x3)    /* Error Type 8 */
#define ECC_STATUS_ERR_TYPE_8_BASE      22
#define ECC_STATUS_ERR_TYPE_7_MSK       SHIFT20(0x3)    /* Error Type 7 */
#define ECC_STATUS_ERR_TYPE_7_BASE      20
#define ECC_STATUS_ERR_TYPE_6_MSK       SHIFT18(0x3)    /* Error Type 6 */
#define ECC_STATUS_ERR_TYPE_6_BASE      18
/*      Bit(s) ECC_STATUS_RSRV_23_22 reserved */
#define ECC_STATUS_ERR_TYPE_5_MSK       SHIFT16(0x3)    /* Error Type 5 */
#define ECC_STATUS_ERR_TYPE_5_BASE      16
/*      Bit(s) ECC_STATUS_RSRV_19_18 reserved */
#define ECC_STATUS_ERR_TYPE_4_MSK       SHIFT14(0x3)    /* Error Type 4 */
#define ECC_STATUS_ERR_TYPE_4_BASE      14
/*      Bit(s) ECC_STATUS_RSRV_15_14 reserved */
#define ECC_STATUS_ERR_TYPE_3_MSK       SHIFT12(0x3)    /* Error Type 3 */
#define ECC_STATUS_ERR_TYPE_3_BASE      12
/*      Bit(s) ECC_STATUS_RSRV_11_10 reserved */
#define ECC_STATUS_ERR_TYPE_2_MSK       SHIFT8(0x3)     /* Error Type 2 */
#define ECC_STATUS_ERR_TYPE_2_BASE      8
/*      Bit(s) ECC_STATUS_RSRV_7_6 reserved */
#define ECC_STATUS_ERR_TYPE_1_MSK       SHIFT4(0x3)     /* Error Type 1 */
#define ECC_STATUS_ERR_TYPE_1_BASE      4
/*      Bit(s) ECC_STATUS_RSRV_3_2 reserved */
#define ECC_STATUS_ERR_TYPE_0_MSK       SHIFT0(0x3)     /* Error Type 0 */
#define ECC_STATUS_ERR_TYPE_0_BASE      0








#endif
