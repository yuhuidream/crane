#ifndef __MRVL_I2C_H__
#define __MRVL_I2C_H__

#include "predefines.h"

typedef struct
{
    __IO uint32_t IBMR;
    __IO uint32_t RESERVED0;
    __IO uint32_t IDBR;
    __IO uint32_t RESERVED1;
    __IO uint32_t ICR;
    __IO uint32_t RESERVED2;
    __IO uint32_t ISR;
    __IO uint32_t RESERVED3;
    __IO uint32_t ISAR;
    __IO uint32_t RESERVED4;
    __IO uint32_t ILCR;
    __IO uint32_t RESERVED5;
    __IO uint32_t IWCR;
    __IO uint32_t RESERVED6[3];
    __IO uint32_t WFIFO;

    __IO uint32_t WFIFO_WPTR;

    __IO uint32_t WFIFO_RPTR;
    __IO uint32_t RESERVED7;
    __IO uint32_t RFIFO;

    __IO uint32_t RFIFO_WPTR;

    __IO uint32_t RFIFO_RPTR;
    __IO uint32_t RESERVED8;
    __IO uint32_t FIFO_TSHLD;

}MRVL_I2C_TypeDef;

#define    I2C1_BASE      0xD4011000
#define    MRVL_I2C1  (( MRVL_I2C_TypeDef *) I2C1_BASE )

#define    I2C2_BASE      0xD4037000
#define    MRVL_I2C2  (( MRVL_I2C_TypeDef *) I2C2_BASE )

/* I2C Control Register (ICR)................................................ */
#define ICR_SDA_GFE       BIT_30    /* SDA glitch fix enable for MMP3 B0      */
#define ICR_TXOV_IE       BIT_29    /* Transmit FIFO Overrun Interrupt Enable */
#define ICR_RXUN_IE       BIT_28    /* Receive FIFO Underrun Interrupt Enable */
#define ICR_DMA_EN        BIT_27    /* DMA Enable for Both Tx and Rx FIFOs    */
#define ICR_RXOV_IE       BIT_26    /* Receive FIFO Overrun Interrupt Enable  */
#define ICR_RXF_IE        BIT_25    /* Receive FIFO Full Interrupt Enable     */
#define ICR_TXSR_IE       BIT_24    /* Tx FIFO Service Request Int Enable     */
#define ICR_RXSR_IE       BIT_23    /* Receive FIFO Half Full Int Enable      */
#define ICR_TXDONE_IE     BIT_22    /* Transaction Done Interrupt Enable      */
#define ICR_TXBEGIN       BIT_21    /* Transaction Begins                     */
#define ICR_FIFOEN        BIT_20    /* FIFO Mode                              */
#define ICR_GPIOEN        BIT_19    /* GPIO Mode Enable for SCL During HS     */
#define ICR_MSDE          BIT_18    /* Master Stop Detected Enable            */
#define ICR_MSDIE         BIT_17    /* Master Stop Detected Interrupt Enable  */
#define ICR_MODE_MSK      SHIFT15(0x3)
#define ICR_MODE_BASE     15
#define ICR_UR            BIT_14    /* Unit Reset                             */
#define ICR_SADIE         BIT_13    /* Slave Address Detected Int Enable      */
#define ICR_ALDIE         BIT_12    /* Arbitration Loss Detected Int Enable   */
#define ICR_SSDIE         BIT_11    /* Slave Stop Detected Interrupt Enable   */
#define ICR_BEIE          BIT_10    /* Bus Error Interrupt Enable             */
#define ICR_IRFIE         BIT_9     /* DBR Receive Full Interrupt Enable      */
#define ICR_ITEIE         BIT_8     /* IDBR Transmit Empty Interrupt Enable   */
#define ICR_GCD           BIT_7     /* General Call Disable                   */
#define ICR_IUE           BIT_6     /* I2C Unit Enable                        */
#define ICR_SCLE          BIT_5     /* Master Clock Enable                    */
#define ICR_MA            BIT_4     /* Master Abort                           */
#define ICR_TB            BIT_3     /* Transfer Byte Bit                      */
#define ICR_ACKNAK        BIT_2     /* Send ACK(0) or NAK(1)                  */
#define ICR_STOP          BIT_1     /* Stop Bit                               */
#define ICR_START         BIT_0     /* Start Bit                              */

/* I2C Status Register (ISR)................................................. */
#define ISR_TXOV          BIT_19    /* Transmit FIFO Overrun                  */
#define ISR_RXUN          BIT_18    /* Receive FIFO Underrun                  */
#define ISR_RXOV          BIT_17    /* Receive FIFO Overrun                   */
#define ISR_RXF           BIT_16    /* Receive FIFO Full                      */
#define ISR_TXSR          BIT_15    /* Transmit FIFO Service Request          */
#define ISR_RXSR          BIT_14    /* Receive FIFO Service Request           */
#define ISR_TXDONE        BIT_13    /* Transaction Done                       */
#define ISR_MSD           BIT_12    /* Master Stop Detected                   */
#define ISR_EBB           BIT_11    /* Early Bus Busy                         */
#define ISR_BED           BIT_10    /* Bus Error Detected                     */
#define ISR_SAD           BIT_9     /* Slave Address Detected                 */
#define ISR_GCAD          BIT_8     /* General Call Address Detected          */
#define ISR_IRF           BIT_7     /* IDBR Receive Full                      */
#define ISR_ITE           BIT_6     /* IDBR Transmit Empty                    */
#define ISR_ALD           BIT_5     /* Arbitration Loss Detected              */
#define ISR_SSD           BIT_4     /* Slave Stop Detected                    */
#define ISR_IBB           BIT_3     /* I2C Bus Busy                           */
#define ISR_UB            BIT_2     /* Unit Busy                              */
#define ISR_ACKNAK        BIT_1     /* ACK/NACK Status                        */
#define ISR_RWM           BIT_0     /* Read/write Mode                        */



//Bitfield Constants Register IBMR
#define IBMR_SCL_MASK                              0x00000002
#define IBMR_SCL_SHIFT                             1
#define IBMR_SDA_MASK                              0x00000001
#define IBMR_SDA_SHIFT                             0


//Bitfield Constants Register IDBR
#define IDBR_DATA_BUFFER_MASK                      0x000000FF
#define IDBR_DATA_BUFFER_SHIFT                     0


//Bitfield Constants Register ICR
#define ICR_SDA_GFE_MASK                          0x40000000
#define ICR_SDA_GFE_SHIFT                         30
#define ICR_TXOV_IE_MASK                          0x20000000
#define ICR_TXOV_IE_SHIFT                         29
#define ICR_RXUN_IE_MASK                          0x10000000
#define ICR_RXUN_IE_SHIFT                         28
#define ICR_DMA_EN_MASK                           0x08000000
#define ICR_DMA_EN_SHIFT                          27
#define ICR_RXOV_IE_MASK                          0x04000000
#define ICR_RXOV_IE_SHIFT                         26
#define ICR_RXF_IE_MASK                           0x02000000
#define ICR_RXF_IE_SHIFT                          25
#define ICR_TXSR_IE_MASK                          0x01000000
#define ICR_TXSR_IE_SHIFT                         24
#define ICR_RXSR_IE_MASK                          0x00800000
#define ICR_RXSR_IE_SHIFT                         23
#define ICR_TXDONE_IE_MASK                        0x00400000
#define ICR_TXDONE_IE_SHIFT                       22
#define ICR_TXBEGIN_MASK                          0x00200000
#define ICR_TXBEGIN_SHIFT                         21
#define ICR_FIFOEN_MASK                           0x00100000
#define ICR_FIFOEN_SHIFT                          20
#define ICR_GPIOEN_MASK                           0x00080000
#define ICR_GPIOEN_SHIFT                          19
#define ICR_MSDE_MASK                             0x00040000
#define ICR_MSDE_SHIFT                            18
#define ICR_MSDIE_MASK                            0x00020000
#define ICR_MSDIE_SHIFT                           17
#define ICR_MODE_MASK                             0x00018000
#define ICR_MODE_SHIFT                            15
#define ICR_UR_MASK                               0x00004000
#define ICR_UR_SHIFT                              14
#define ICR_SADIE_MASK                            0x00002000
#define ICR_SADIE_SHIFT                           13
#define ICR_ALDIE_MASK                            0x00001000
#define ICR_ALDIE_SHIFT                           12
#define ICR_SSDIE_MASK                            0x00000800
#define ICR_SSDIE_SHIFT                           11
#define ICR_BEIE_MASK                             0x00000400
#define ICR_BEIE_SHIFT                            10
#define ICR_DRFIE_MASK                            0x00000200
#define ICR_DRFIE_SHIFT                           9
#define ICR_ITEIE_MASK                            0x00000100
#define ICR_ITEIE_SHIFT                           8
#define ICR_GCD_MASK                              0x00000080
#define ICR_GCD_SHIFT                             7
#define ICR_IUE_MASK                              0x00000040
#define ICR_IUE_SHIFT                             6
#define ICR_SCLE_MASK                             0x00000020
#define ICR_SCLE_SHIFT                            5
#define ICR_MA_MASK                               0x00000010
#define ICR_MA_SHIFT                              4
#define ICR_TB_MASK                               0x00000008
#define ICR_TB_SHIFT                              3
#define ICR_ACKNAK_MASK                           0x00000004
#define ICR_ACKNAK_SHIFT                          2
#define ICR_STOP_MASK                             0x00000002
#define ICR_STOP_SHIFT                            1
#define ICR_START_MASK                            0x00000001
#define ICR_START_SHIFT                           0

//Bitfield Constants Register ISR
#define ISR_TXOV_MASK                             0x00080000
#define ISR_TXOV_SHIFT                            19
#define ISR_RXUN_MASK                             0x00040000
#define ISR_RXUN_SHIFT                            18
#define ISR_RXOV_MASK                             0x00020000
#define ISR_RXOV_SHIFT                            17
#define ISR_RXF_MASK                              0x00010000
#define ISR_RXF_SHIFT                             16
#define ISR_TXSR_MASK                             0x00008000
#define ISR_TXSR_SHIFT                            15
#define ISR_RXSR_MASK                             0x00004000
#define ISR_RXSR_SHIFT                            14
#define ISR_TXDONE_MASK                           0x00002000
#define ISR_TXDONE_SHIFT                          13
#define ISR_MSD_MASK                              0x00001000
#define ISR_MSD_SHIFT                             12
#define ISR_EBB_MASK                              0x00000800
#define ISR_EBB_SHIFT                             11
#define ISR_BED_MASK                              0x00000400
#define ISR_BED_SHIFT                             10
#define ISR_SAD_MASK                              0x00000200
#define ISR_SAD_SHIFT                             9
#define ISR_GCAD_MASK                             0x00000100
#define ISR_GCAD_SHIFT                            8
#define ISR_IRF_MASK                              0x00000080
#define ISR_IRF_SHIFT                             7
#define ISR_ITE_MASK                              0x00000040
#define ISR_ITE_SHIFT                             6
#define ISR_ALD_MASK                              0x00000020
#define ISR_ALD_SHIFT                             5
#define ISR_SSD_MASK                              0x00000010
#define ISR_SSD_SHIFT                             4
#define ISR_IBB_MASK                              0x00000008
#define ISR_IBB_SHIFT                             3
#define ISR_UB_MASK                               0x00000004
#define ISR_UB_SHIFT                              2
#define ISR_ACKNAK_MASK                           0x00000002
#define ISR_ACKNAK_SHIFT                          1
#define ISR_RWM_MASK                              0x00000001
#define ISR_RWM_SHIFT                             0






//Bitfield Constants Register ISAR
#define ISAR_SLAVE_ADDRESS_MASK                    0x0000007F
#define ISAR_SLAVE_ADDRESS_SHIFT                   0


//Bitfield Constants Register ILCR
#define ILCR_HLVH_MASK                             0xF8000000
#define ILCR_HLVH_SHIFT                            27
#define ILCR_HLVL_MASK                             0x07FC0000
#define ILCR_HLVL_SHIFT                            18
#define ILCR_FLV_MASK                              0x0003FE00
#define ILCR_FLV_SHIFT                             9
#define ILCR_SLV_MASK                              0x000001FF
#define ILCR_SLV_SHIFT                             0


//Bitfield Constants Register IWCR
#define IWCR_HS_COUNT2_MASK                        0x00007C00
#define IWCR_HS_COUNT2_SHIFT                       10
#define IWCR_HS_COUNT1_MASK                        0x000003E0
#define IWCR_HS_COUNT1_SHIFT                       5
#define IWCR_COUNT_MASK                            0x0000001F
#define IWCR_COUNT_SHIFT                           0


//Bitfield Constants Register WFIFO
#define WFIFO_CONTROL_MASK                          0x00000F00
#define WFIFO_CONTROL_SHIFT                         8
#define WFIFO_DATA_MASK                             0x000000FF
#define WFIFO_DATA_SHIFT                            0


//Bitfield Constants Register WFIFO_WPTR
#define WFIFO_WPTR_DATA_MASK                             0x0000001F
#define WFIFO_WPTR_DATA_SHIFT                            0


//Bitfield Constants Register WFIFO_RPTR
#define WFIFO_RPTR_DATA_MASK                             0x0000001F
#define WFIFO_RPTR_DATA_SHIFT                            0



//Bitfield Constants Register RFIFO
#define RFIFO_DATA_MASK                             0x000000FF
#define RFIFO_DATA_SHIFT                            0

//Bitfield Constants Register RFIFO_WPTR
#define RFIFO_WPTR_DATA_MASK                             0x0000001F
#define RFIFO_WPTR_DATA_SHIFT                            0



//Bitfield Constants Register RFIFO_RPTR
#define RFIFO_RPTR_DATA_MASK                             0x0000001F
#define RFIFO_RPTR_DATA_SHIFT                            0

//Bitfield Constants Register FIFO_TSHLD
#define FIFO_TSHLD_RX_MASK                               0x001F0000
#define FIFO_TSHLD_RX_SHIFT                              16
#define FIFO_TSHLD_TX_MASK                               0x0000001F
#define FIFO_TSHLD_TX_SHIFT                              0



#endif
