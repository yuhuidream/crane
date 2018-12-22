#ifndef __AIB_H__
#define __AIB_H__

typedef struct
{
    __IO uint32_t AIB_NAND_DCS_REG;   /* 0x0000 offset AIB NAND I/O Domain Control Register*/
    __IO uint32_t AIB_ANT_IO_REG;   /*0x0004 offset AIB ANT I/O Domain Control Register*/
    __IO uint32_t GPIO1_IO_REG;  //*0x0008 offset AIB GPIO 1 I/O Domain Control Register
    __IO uint32_t GPIO2_IO_REG;  //*0x000c offset AIB GPIO 2(Camera Interface) I/O Domain Control Register
    __IO uint32_t GPIO3_IO_REG;  //0x0010 offset AIB GPIO 3 (LCD Interface) I/O Domain Control Register
    __IO uint32_t AIB_GSM_IO_REG;  //0x0014 offset AIB GSM I/O Domain Control Register

    __IO uint32_t AIB_MEM_IO_REG;   //0x0018 offset AIB MEM I/O Domain Control Register
    __IO uint32_t SDMMC_IO_REG;  //0x001c offset AIB SDMMC I/O Domain Control Register
    __IO uint32_t NAND_IO_REG;   //0x0020 offset AIB NAND I/O Domain Control Register

    __IO uint32_t PMIC1_IO_REG;  //0x0024 offset AIB PMIC 1 I/O Domain Control Register
 
    __IO uint32_t PMIC2_IO_REG;  //0x0028 offset AIB PMIC 2 I/O Domain Control Register
    __IO uint32_t USIM_IO_REG;   //0x002c offset AIB USIM I/O Domain Control Register
    __IO uint32_t AIB_GPIO3_DCS_REG;     //0x0030 offset AIB GPIO3 I/O Domain Control Register
    __IO uint32_t RECEIVED2[(0x400-0x030)/4-1];
    __IO uint32_t AIB_AP_CFG_15_0;//0X400 offset AIB AP config [15:0];
    __IO uint32_t AIB_AP_CFG_31_16;//0X404 offset AIB AP config [31:16];
    __IO uint32_t AIB_AP_CFG_47_32;//0X408 offset AIB AP config [47:32];
    __IO uint32_t AIB_AP_CFG_63_48;//0X40C offset AIB AP config [63:48];
    __IO uint32_t AIB_AP_CFG_79_64;//0X410 offset AIB AP config [79:64];
    __IO uint32_t AIB_CP_CFG_15_0;//0X414 offset AIB CP config [15:0];

}AIBAux_TypeDef;

#define AIBAux_BASE      0xD401E800

#define AIBAux     (( AIBAux_TypeDef *) AIBAux_BASE )
/*
 *
 *	THE BIT DEFINES
 *
 */

 /* 0x0000 offset AIB NAND I/O Domain Control Register*/
#define  AIB_NAND_DCS_REG_IO_DCS_MSK    SHIFT6(0x3)
#define  AIB_NAND_DCS_REG_IO_DCS_BASE    6

  /*0x0004 offset AIB ANT I/O Domain Control Register*/
#define   AIB_ANT_IO_REG_V25EN     BIT_3
#define   AIB_ANT_IO_REG_V18EN   BIT_2
#define   AIB_ANT_IO_REG_PDB   BIT_0



//0x0008 offset AIB GPIO 1 I/O Domain Control Register
#define  GPIO1_IO_REG_IO_DCS_MSK    SHIFT6(0x3)
#define  GPIO1_IO_REG_IO_DCS_BASE    6

//0x000c offset AIB GPIO 2(Camera Interface) I/O Domain Control Register
#define GPIO2_IO_REG_PDB    BIT_0
#define GPIO2_IO_REG_V18EN   BIT_2
#define GPIO2_IO_REG_V25EN     BIT_3
//0x0010 offset AIB GPIO 3 (LCD Interface) I/O Domain Control Register
#define GPIO3_IO_REG_PDB    BIT_0
#define GPIO3_IO_REG_V18EN   BIT_2
#define GPIO3_IO_REG_V25EN     BIT_3

//0x0014 offset AIB GSM I/O Domain Control Register
#define  AIB_GSM_IO_REG_IO_DCS_MSK    SHIFT6(0x3)
#define  AIB_GSM_IO_REG_IO_DCS_BASE    6

//0x001c offset AIB SDMMC I/O Domain Control Register
#define SDMMC_IO_REG_PDB    BIT_0
#define SDMMC_IO_REG_V18EN   BIT_2
#define SDMMC_IO_REG_V25EN     BIT_3

//0x0020 offset AIB NAND I/O Domain Control Register
#define NAND_IO_REG_PDB    BIT_0
#define BYP_SEL_IO_REG_PDB    BIT_1
#define NAND_IO_REG_V18EN   BIT_2
#define NAND_IO_REG_SEN_DIS   BIT_3


//0x0024 offset AIB PMIC 1 I/O Domain Control Register
#define  PMIC1_IO_REG_IO_DCS_MSK    SHIFT6(0x3)
#define  PMIC1_IO_REG_IO_DCS_BASE    6

//0x0028 offset AIB PMIC 2 I/O Domain Control Register
#define  PMIC2_IO_REG_IO_DCS_MSK    SHIFT6(0x3)
#define  PMIC2_IO_REG_IO_DCS_BASE    6

//0x002c offset AIB USIM I/O Domain Control Register
#define USIM_IO_REG_PDB    BIT_0
#define USIM_IO_REG_V18EN   BIT_2
#define USIM_IO_REG_V25EN     BIT_3
//0x0030 offset AIB Base Band I/O Domain Control Register
#define  AIB_GPIO3_DCS_REG_IO_DCS_MSK    SHIFT6(0x3)
#define  AIB_GPIO3_DCS_REG_IO_DCS_BASE    6

//0X400 offset AIB AP config [15:0];
#define AIB_AP_CFG_15_0_MSK  SHIFT0(0xffff)
#define AIB_AP_CFG_15_0_BASE    0

//0X404 offset AIB AP config [31:16];
#define AIB_AP_CFG_31_16_MSK  SHIFT0(0xffff)
#define AIB_AP_CFG_31_16_BASE    0

//0X408 offset AIB AP config [47:32];
#define AIB_AP_CFG_47_32_MSK  SHIFT0(0xffff)
#define AIB_AP_CFG_47_32_BASE    0

//0X40C offset AIB AP config [63:48];
#define AIB_AP_CFG_63_48_MSK  SHIFT0(0xffff)
#define AIB_AP_CFG_63_48_BASE    0



//0X410 offset AIB AP config [79:64];
#define AIB_AP_CFG_79_64_MSK  SHIFT0(0xffff)
#define AIB_AP_CFG_79_64_BASE    0

//0X414 offset AIB CP config [15:0];
#define AIB_CP_CFG_15_0_MSK  SHIFT0(0xffff)
#define AIB_CP_CFG_15_0_BASE    0





#endif
