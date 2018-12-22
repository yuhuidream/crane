#ifndef H_PCIE_APP_H
#define H_PCIE_APP_H

typedef struct{
    __IO uint32_t INT_STATUS;               //0x000, Application Interrupt Status
    __IO uint32_t MISC_ERR_STATUS;          //0x004, Application Miscellaneous Error Status
    __IO uint32_t INT_ENABLE;               //0x008, Application Interrupt Enable
    __IO uint32_t MISC_ERR_ENABLE;          //0x00C, Application Miscellaneous Error Enable
    __IO uint32_t RESERVED0[5];             //0x010~0x020
    __IO uint32_t ENUM_ID;                  //0x024, Application Enumeration ID
    __IO uint32_t RESERVED1[6];             //0x028~0x03C
    __IO uint32_t MISC_CTRL;                //0x040, Application Misc Control
    __IO uint32_t TX_INT_CTRL;              //0x044, Application Transmit Interrupt Control
    __IO uint32_t TX_MSG_CTRL;              //0x048, Application Transmit Message Control
    __IO uint32_t RESERVED2[5];             //0x04C~0x05C
    __IO uint32_t TX_VDM_HDR_BYTES[4];      //0x060~0x06C, Application Transmit VDM Header Bytes 0 - 15
    __IO uint32_t RX_VDM_HDR_BYTES[3];      //0x070~0x078, Application Receive VDM Header Bytes 4-15
    __IO uint32_t RESERVED3;                //0x07C
    __IO uint32_t INBOUND_CPL_TIMEOUT_HDR;  //0x080, Application Inbound Completion Timeout Header
    __IO uint32_t OUTBOUND_CPL_TIMEOUT_HDR; //0x084, Application Outbound Completion Timeout Header
    __IO uint32_t RESERVED4[2];             //0x088~0x08C
    __IO uint32_t DBG_CTRL;                 //0x090, Application Debug Control
    __IO uint32_t PHY_DBG_DATA;             //0x094, Application PHY Debug Data
    __IO uint32_t RX_FLOW_CTRL_UPDATE_DATE; //0x098, Application Receive Flow Control Update Data
    __IO uint32_t RESERVED5;                //0x09C
    __IO uint32_t DIAGNOSTIC_STATUS[14];    //0x0A0~0x0D4, Application Diagnostic Status 0 ~ 13
    __IO uint32_t RESERVED6[6];
    __IO uint32_t SRAM_TIMING_CTRL[2];      //0x0F0~0x0F4, Application SRAM Timing Control 0~1
    __IO uint32_t RESERVED7;                //0x0F8
    __IO uint32_t MISC_RST_CTRL;            //0x0FC
    __IO uint32_t SYSP_MISC_CTRL;           //0x100, Sysp Application Misc Control
    __IO uint32_t SYSP_DEV_STATUS;          //0x104, Sysp Application Device Status
    __IO uint32_t ERR_MSG_TX_TRIGGER;       //0x108, Error Message Transmit Trigger
    __IO uint32_t HDR_TLP_ERROR[4];         //0x10C~0x118, Header content of TLP with Error 0 ~ 3
    __IO uint32_t ERR_MSG_MISC_INFO;        //0x11C, Error Message Misc Information
    __IO uint32_t APP_WARM_RST_TIMER;       //0x120
}PCIE_APP_TypeDef;

#define PCIE_APP_BASE 0xD420B000

#define PCIE_APP (( PCIE_APP_TypeDef *) PCIE_APP_BASE )

/* INT_STATUS */
#define RX_MSI          BIT_8
#define DMA_LOCAL_INT   BIT_2

/* INT_ENABLE */
/* Received Message Signalled Interrupt */
#define DMA_LOCAL_INT_EN    BIT_2
#define RX_MSI_EN           BIT_8
#define RX_PME_EN           BIT_19
#define RX_PME_TO_ACK_EN    BIT_20
#define RX_PM_TURN_OFF_EN   BIT_21

/*
MISC_RST_CTRL:
Bit 8:  power up rst
Bit 9:  core rst
Bit 10: pipe rst
Bit 11: stick rst
Bit 12: non-stick rst
Bit 13: phy rst
Bit 14: axi rst

Bit2: pcie_mac2phy reset enable
Bit3:  pcie_mac2phy reset
If (bit[2]) then pcie_mac2phy_reset = bit[3];
Else if (bit [1:0] == 2’b10) pcie_mac2phy_reset = syned power up reset;
Else pcie_mac2phy_reset = pcie_clk_res_ctrl[0];
*/

/* SYSP_DEV_STATUS */
#define DLL_LINK_UP   BIT_1
#define PHY_LINK_UP   BIT_0

/* Application Transmit Message Control (0x048) */
#define TX_MSG_BUSY   BIT_31
#define TX_MSG_OP_BASE 24
#define TX_MSG_OP_MASK       (0x3 << 24)
#define TX_MSG_PM_TURN_OFF   (0x1 << 24) 
#define TX_MSG_LOCK          (0x2 << 24) 
#define TX_MSG_VDM           (0x3 << 24) 
#endif