#ifndef __CEC_H__
#define __CEC_H__


typedef struct
{
   __IO uint32_t RESERVED0[0x200/4];
   __IO uint32_t TOGGLE_FOR_WRITE;
   __IO uint32_t TOGGLE_FOR_READ;
   __IO uint32_t RDY;
   __IO uint32_t RX_RDY;
   __IO uint32_t TX_FIFO_RESET;
   __IO uint32_t RX_FIFO_RESET; 
   __IO uint32_t PMODE;
   __IO uint32_t TX_RDY;
   __IO uint32_t TX_TYPE;
   __IO uint8_t TX_RESP_TIME_0;
   __IO uint8_t TX_RESP_TIME_1;
   __IO uint8_t TX_RESP_TIME_2;
   __IO uint8_t TX_RESP_TIME_3;
   __IO uint8_t SIGNAL_FREE_TIME_0;
   __IO uint8_t SIGNAL_FREE_TIME_1;
   __IO uint8_t SIGNAL_FREE_TIME_2;
   __IO uint8_t SIGNAL_FREE_TIME_3;
   __IO uint8_t START_BIT_LO_THRESH_0;
   __IO uint8_t START_BIT_LO_THRESH_1;
   __IO uint8_t START_BIT_LO_THRESH_2;
   __IO uint8_t START_BIT_LO_THRESH_3;
   __IO uint8_t START_BIT_HI_THRESH_0;
   __IO uint8_t START_BIT_HI_THRESH_1;
   __IO uint8_t START_BIT_HI_THRESH_2;
   __IO uint8_t START_BIT_HI_THRESH_3;
   __IO uint8_t DATA_BIT_0_LO_THRESH_0;
   __IO uint8_t DATA_BIT_0_LO_THRESH_1;
   __IO uint8_t DATA_BIT_0_LO_THRESH_2;
   __IO uint8_t DATA_BIT_0_LO_THRESH_3;
   __IO uint8_t DATA_BIT_0_HI_THRESH_0;
   __IO uint8_t DATA_BIT_0_HI_THRESH_1;
   __IO uint8_t DATA_BIT_0_HI_THRESH_2;
   __IO uint8_t DATA_BIT_0_HI_THRESH_3;
   __IO uint8_t DATA_BIT_1_LO_THRESH_0;
   __IO uint8_t DATA_BIT_1_LO_THRESH_1;
   __IO uint8_t DATA_BIT_1_LO_THRESH_2;
   __IO uint8_t DATA_BIT_1_LO_THRESH_3;
   __IO uint8_t DATA_BIT_1_HI_THRESH_0;
   __IO uint8_t DATA_BIT_1_HI_THRESH_1;
   __IO uint8_t DATA_BIT_1_HI_THRESH_2;
   __IO uint8_t DATA_BIT_1_HI_THRESH_3;
   __IO uint8_t SSP_ACK_0;
   __IO uint8_t SSP_ACK_1;
   __IO uint8_t SSP_ACK_2;
   __IO uint8_t SSP_ACK_3;
   __IO uint8_t INTR_ENABLE0;
   __IO uint8_t INTR_ENABLE1;   //0X0249
   __IO uint16_t RESERVED1;
   __IO uint32_t DATA;  //0X024C
   __IO uint32_t EOM;
   __IO uint32_t RESERVED3;
   __IO uint8_t INTR_STATUS0;
   __IO uint8_t INTR_STATUS1;
   __IO uint16_t RESERVED4;
   __IO uint8_t NOMINAL_SAMPLE_TIME_0;
   __IO uint8_t NOMINAL_SAMPLE_TIME_1;
   __IO uint8_t NOMINAL_SAMPLE_TIME_2;
   __IO uint8_t NOMINAL_SAMPLE_TIME_3;
   __IO uint8_t HYSTERESIS_TIME_0;
   __IO uint8_t HYSTERESIS_TIME_1;
   __IO uint8_t HYSTERESIS_TIME_2;
   __IO uint8_t HYSTERESIS_TIME_3;
   __IO uint8_t FOLLOWER_ACK_TIME_0;
   __IO uint8_t FOLLOWER_ACK_TIME_1;
   __IO uint8_t FOLLOWER_ACK_TIME_2;
   __IO uint8_t FOLLOWER_ACK_TIME_3;
   __IO uint8_t RX_BUF_READ;
   __IO uint8_t RX_EOM_READ;
   __IO uint8_t LOGICAL_ADDR0;
   __IO uint8_t LOGICAL_ADDR1;
   __IO uint8_t LOGICAL_ADDR2;
   __IO uint8_t LOGICAL_ADDR3;
   __IO uint8_t LOGICAL_ADDR4;//0X026e
   __IO uint8_t RESERVED5;
   __IO uint8_t JITTER_CNT_0;//0X0270
   __IO uint8_t JITTER_CNT_1;
   __IO uint8_t JITTER_CNT_2;
   __IO uint8_t JITTER_CNT_3;
   __IO uint32_t LINE_STATUS;
   __IO uint8_t TX_PRESENT_STATE;
   __IO uint8_t RX_PRESENT_STATE;
   __IO uint8_t COLL_CTRL;
   //__IO uint8_t RESERVED6;
   __IO uint8_t GLITCH_FILTER_STAGES;//0X27B
   __IO uint8_t COLL_WINDOW_TIME_0;
   __IO uint8_t COLL_WINDOW_TIME_1;
   __IO uint8_t COLL_WINDOW_TIME_2;
   __IO uint8_t COLL_WINDOW_TIME_3;
   __IO uint8_t TX_FIFO_FULL_THRESH;
   __IO uint8_t TX_FIFO_WPTR;
   __IO uint8_t TX_FIFO_RPTR;
   __IO uint8_t TX_FIFO_DPTR;
   __IO uint8_t RX_FIFO_FULL_THRESH;
   __IO uint8_t RX_FIFO_WPTR;
   __IO uint8_t RX_FIFO_RPTR;
   __IO uint8_t RX_FIFO_DPTR;//0X287







}CEC_TypeDef;

#define CEC_BASE      0xD420BC00

#define CEC     (( CEC_TypeDef *) CEC_BASE )



//Bitfield Constants Register CEC_TOGGLE_FOR_WRITE
#define TOGGLE_FOR_WRITE_WR_PULSE_MASK                         0x00000001



//Bitfield Constants Register CEC_TOGGLE_FOR_READ
#define TOGGLE_FOR_READ_RD_PULSE_MASK                         0x00000001




//Bitfield Constants Register CEC_RDY
#define RDY_CEC_RDY_MASK                          0x00000001




//Bitfield Constants Register CEC_RX_RDY
#define RX_RDY_CEC_RX_RDY_MASK                       0x00000001



//Bitfield Constants Register CEC_TX_FIFO_RESET
#define TX_FIFO_RESET_CEC_TX_FIFO_RESET_MASK                0x00000001




//Bitfield Constants Register CEC_RX_FIFO_RESET
#define RX_FIFO_RESET_CEC_RX_FIFO_RESET_MASK                0x00000001





//Bitfield Constants Register CEC_PMODE
#define PMODE_PMODE_MASK                            0x00000001


//Bitfield Constants Register CEC_TX_RDY
#define TX_RDY_CEC_TX_RDY_MASK                       0x00000001


//Bitfield Constants Register CEC_TX_TYPE
#define TX_TYPE_TX_TYPE_MASK                          0x00000001


//Bitfield Constants Register CEC_TX_RESP_TIME_0
#define TX_RESP_TIME_0_TX_RESP_TIME_BYTE0_MASK               0x000000FF
#define TX_RESP_TIME_0_TX_RESP_TIME_BYTE0_SHIFT              0

//Bitfield Constants Register CEC_TX_RESP_TIME_1
#define TX_RESP_TIME_1_TX_RESP_TIME_BYTE1_MASK               0x000000FF
#define TX_RESP_TIME_1_TX_RESP_TIME_BYTE1_SHIFT              0


//Bitfield Constants Register CEC_TX_RESP_TIME_2
#define TX_RESP_TIME_2_TX_RESP_TIME_BYTE2_MASK               0x000000FF
#define TX_RESP_TIME_2_TX_RESP_TIME_BYTE2_SHIFT              0


//Bitfield Constants Register CEC_TX_RESP_TIME_3
#define TX_RESP_TIME_3_TX_RESP_TIME_BYTE3_MASK               0x000000FF
#define TX_RESP_TIME_3_TX_RESP_TIME_BYTE3_SHIFT              0


//Bitfield Constants Register CEC_SIGNAL_FREE_TIME_0
#define SIGNAL_FREE_TIME_0_SIG_FREE_TIME_BYTE0_MASK              0x000000FF
#define SIGNAL_FREE_TIME_0_SIG_FREE_TIME_BYTE0_SHIFT             0


//Bitfield Constants Register CEC_SIGNAL_FREE_TIME_1
#define SIGNAL_FREE_TIME_1_SIG_FREE_TIME_BYTE1_MASK              0x000000FF
#define SIGNAL_FREE_TIME_1_SIG_FREE_TIME_BYTE1_SHIFT             0



//Bitfield Constants Register CEC_SIGNAL_FREE_TIME_2
#define SIGNAL_FREE_TIME_2_SIG_FREE_TIME_BYTE2_MASK              0x000000FF
#define SIGNAL_FREE_TIME_2_SIG_FREE_TIME_BYTE2_SHIFT             0


//Bitfield Constants Register CEC_SIGNAL_FREE_TIME_3
#define SIGNAL_FREE_TIME_3_SIG_FREE_TIME_BYTE3_MASK              0x000000FF
#define SIGNAL_FREE_TIME_3_SIG_FREE_TIME_BYTE3_SHIFT             0


//Bitfield Constants Register CEC_START_BIT_LO_THRESH_0
#define START_BIT_LO_THRESH_0_SB_LO_TH_BYTE0_MASK                   0x000000FF
#define START_BIT_LO_THRESH_0_SB_LO_TH_BYTE0_SHIFT                  0


//Bitfield Constants Register CEC_START_BIT_LO_THRESH_1
#define START_BIT_LO_THRESH_1_SB_LO_TH_BYTE1_MASK                   0x000000FF
#define START_BIT_LO_THRESH_1_SB_LO_TH_BYTE1_SHIFT                  0



//Bitfield Constants Register CEC_START_BIT_LO_THRESH_2
#define START_BIT_LO_THRESH_2_SB_LO_TH_BYTE2_MASK                   0x000000FF
#define START_BIT_LO_THRESH_2_SB_LO_TH_BYTE2_SHIFT                  0


//Bitfield Constants Register CEC_START_BIT_LO_THRESH_3
#define START_BIT_LO_THRESH_3_SB_LO_TH_BYTE3_MASK                   0x000000FF
#define START_BIT_LO_THRESH_3_SB_LO_TH_BYTE3_SHIFT                  0


//Bitfield Constants Register CEC_START_BIT_HI_THRESH_0
#define START_BIT_HI_THRESH_0_SB_HI_TH_BYTE0_MASK                   0x000000FF
#define START_BIT_HI_THRESH_0_SB_HI_TH_BYTE0_SHIFT                  0


//Bitfield Constants Register CEC_START_BIT_HI_THRESH_1
#define START_BIT_HI_THRESH_1_SB_HI_TH_BYTE1_MASK                   0x000000FF
#define START_BIT_HI_THRESH_1_SB_HI_TH_BYTE1_SHIFT                  0



//Bitfield Constants Register CEC_START_BIT_HI_THRESH_2
#define START_BIT_HI_THRESH_2_SB_HI_TH_BYTE2_MASK                   0x000000FF
#define START_BIT_HI_THRESH_2_SB_HI_TH_BYTE2_SHIFT                  0


//Bitfield Constants Register CEC_START_BIT_HI_THRESH_3
#define START_BIT_HI_THRESH_3_SB_HI_TH_BYTE3_MASK                   0x000000FF
#define START_BIT_HI_THRESH_3_SB_HI_TH_BYTE3_SHIFT                  0



//Bitfield Constants Register CEC_DATA_BIT_0_LO_THRESH_0
#define DATA_BIT_0_LO_THRESH_0_DATA_LO_TH_BYTE0_MASK                 0x000000FF
#define DATA_BIT_0_LO_THRESH_0_DATA_LO_TH_BYTE0_SHIFT                0


//Bitfield Constants Register CEC_DATA_BIT_0_LO_THRESH_1
#define DATA_BIT_0_LO_THRESH_1_DATA_LO_TH_BYTE1_MASK                 0x000000FF
#define DATA_BIT_0_LO_THRESH_1_DATA_LO_TH_BYTE1_SHIFT                0



//Bitfield Constants Register CEC_DATA_BIT_0_LO_THRESH_2
#define DATA_BIT_0_LO_THRESH_2_DATA_LO_TH_BYTE2_MASK                 0x000000FF
#define DATA_BIT_0_LO_THRESH_2_DATA_LO_TH_BYTE2_SHIFT                0


//Bitfield Constants Register CEC_DATA_BIT_0_LO_THRESH_3
#define DATA_BIT_0_LO_THRESH_3_DATA_LO_TH_BYTE3_MASK                 0x000000FF
#define DATA_BIT_0_LO_THRESH_3_DATA_LO_TH_BYTE3_SHIFT                0


//Bitfield Constants Register CEC_DATA_BIT_0_HI_THRESH_0
#define DATA_BIT_0_HI_THRESH_0_DATA_HI_TH_BYTE0_MASK                 0x000000FF
#define DATA_BIT_0_HI_THRESH_0_DATA_HI_TH_BYTE0_SHIFT                0

//Bitfield Constants Register CEC_DATA_BIT_0_HI_THRESH_1
#define DATA_BIT_0_HI_THRESH_1_DATA_HI_TH_BYTE1_MASK                 0x000000FF
#define DATA_BIT_0_HI_THRESH_1_DATA_HI_TH_BYTE1_SHIFT                0

//Bitfield Constants Register CEC_DATA_BIT_0_HI_THRESH_2
#define DATA_BIT_0_HI_THRESH_2_DATA_HI_TH_BYTE2_MASK                 0x000000FF
#define DATA_BIT_0_HI_THRESH_2_DATA_HI_TH_BYTE2_SHIFT                0


//Bitfield Constants Register CEC_DATA_BIT_0_HI_THRESH_3
#define DATA_BIT_0_HI_THRESH_3_DATA_HI_TH_BYTE3_MASK                 0x000000FF
#define DATA_BIT_0_HI_THRESH_3_DATA_HI_TH_BYTE3_SHIFT                0

//Bitfield Constants Register CEC_DATA_BIT_1_LO_THRESH_0
#define DATA_BIT_1_LO_THRESH_0_DATA_LO_TH_BYTE0_MASK                 0x000000FF
#define DATA_BIT_1_LO_THRESH_0_DATA_LO_TH_BYTE0_SHIFT                0


//Bitfield Constants Register CEC_DATA_BIT_1_LO_THRESH_1
#define DATA_BIT_1_LO_THRESH_1_DATA_LO_TH_BYTE1_MASK                 0x000000FF
#define DATA_BIT_1_LO_THRESH_1_DATA_LO_TH_BYTE1_SHIFT                0

//Bitfield Constants Register CEC_DATA_BIT_1_LO_THRESH_2
#define DATA_BIT_1_LO_THRESH_2_DATA_LO_TH_BYTE2_MASK                 0x000000FF
#define DATA_BIT_1_LO_THRESH_2_DATA_LO_TH_BYTE2_SHIFT                0

//Bitfield Constants Register CEC_DATA_BIT_1_LO_THRESH_3
#define DATA_BIT_1_LO_THRESH_3_DATA_LO_TH_BYTE3_MASK                 0x000000FF
#define DATA_BIT_1_LO_THRESH_3_DATA_LO_TH_BYTE3_SHIFT                0


//Bitfield Constants Register CEC_DATA_BIT_1_HI_THRESH_0
#define DATA_BIT_1_HI_THRESH_0_DATA_HI_TH_BYTE0_MASK                 0x000000FF
#define DATA_BIT_1_HI_THRESH_0_DATA_HI_TH_BYTE0_SHIFT                0

//Bitfield Constants Register CEC_DATA_BIT_1_HI_THRESH_1
#define DATA_BIT_1_HI_THRESH_1_DATA_HI_TH_BYTE1_MASK                 0x000000FF
#define DATA_BIT_1_HI_THRESH_1_DATA_HI_TH_BYTE1_SHIFT                0


//Bitfield Constants Register CEC_DATA_BIT_1_HI_THRESH_2
#define DATA_BIT_1_HI_THRESH_2_DATA_HI_TH_BYTE2_MASK                 0x000000FF
#define DATA_BIT_1_HI_THRESH_2_DATA_HI_TH_BYTE2_SHIFT                0

//Bitfield Constants Register CEC_DATA_BIT_1_HI_THRESH_3
#define DATA_BIT_1_HI_THRESH_3_DATA_HI_TH_BYTE3_MASK                 0x000000FF
#define DATA_BIT_1_HI_THRESH_3_DATA_HI_TH_BYTE3_SHIFT                0


//Bitfield Constants Register CEC_SSP_ACK_0
#define SSP_ACK_0_SSP_ACK_BYTE0_MASK                    0x000000FF
#define SSP_ACK_0_SSP_ACK_BYTE0_SHIFT                   0

//Bitfield Constants Register CEC_SSP_ACK_1
#define SSP_ACK_1_SSP_ACK_BYTE1_MASK                    0x000000FF
#define SSP_ACK_1_SSP_ACK_BYTE1_SHIFT                   0

//Bitfield Constants Register CEC_SSP_ACK_2
#define SSP_ACK_2_SSP_ACK_BYTE2_MASK                    0x000000FF
#define SSP_ACK_2_SSP_ACK_BYTE2_SHIFT                   0


//Bitfield Constants Register CEC_SSP_ACK_3
#define SSP_ACK_3_SSP_ACK_BYTE3_MASK                    0x000000FF
#define SSP_ACK_3_SSP_ACK_BYTE3_SHIFT                   0

//Bitfield Constants Register CEC_INTR_ENABLE0
#define INTR_ENABLE0_INTR_EN[7]_MASK                       0x00000080
#define INTR_ENABLE0_INTR_EN[7]_SHIFT                      7
#define INTR_ENABLE0_INTR_EN[6]_MASK                       0x00000040
#define INTR_ENABLE0_INTR_EN[6]_SHIFT                      6
#define INTR_ENABLE0_INTR_EN[5]_MASK                       0x00000020
#define INTR_ENABLE0_INTR_EN[5]_SHIFT                      5
#define INTR_ENABLE0_INTR_EN[4]_MASK                       0x00000010
#define INTR_ENABLE0_INTR_EN[4]_SHIFT                      4
#define INTR_ENABLE0_INTR_EN[3]_MASK                       0x00000008
#define INTR_ENABLE0_INTR_EN[3]_SHIFT                      3
#define INTR_ENABLE0_INTR_EN[2]_MASK                       0x00000004
#define INTR_ENABLE0_INTR_EN[2]_SHIFT                      2
#define INTR_ENABLE0_INTR_EN[1]_MASK                       0x00000002
#define INTR_ENABLE0_INTR_EN[1]_SHIFT                      1
#define INTR_ENABLE0_INTR_EN[0]_MASK                       0x00000001
#define INTR_ENABLE0_INTR_EN[0]_SHIFT                      0


//Bitfield Constants Register CEC_INTR_ENABLE1
#define INTR_ENABLE1_INTR_EN[13]_MASK                      0x00000020
#define INTR_ENABLE1_INTR_EN[13]_SHIFT                     5
#define INTR_ENABLE1_INTR_EN[12]_MASK                      0x00000010
#define INTR_ENABLE1_INTR_EN[12]_SHIFT                     4
#define INTR_ENABLE1_INTR_EN[11]_MASK                      0x00000008
#define INTR_ENABLE1_INTR_EN[11]_SHIFT                     3
#define INTR_ENABLE1_INTR_EN[10]_MASK                      0x00000004
#define INTR_ENABLE1_INTR_EN[10]_SHIFT                     2
#define INTR_ENABLE1_INTR_EN[9]_MASK                       0x00000002
#define INTR_ENABLE1_INTR_EN[9]_SHIFT                      1
#define INTR_ENABLE1_INTR_EN[8]_MASK                       0x00000001
#define INTR_ENABLE1_INTR_EN[8]_SHIFT                      0



//Bitfield Constants Register CEC_DATA
#define DATA_TX_DATA_MASK                          0x000000FF
#define DATA_TX_DATA_SHIFT                         0

//Bitfield Constants Register CEC_EOM
#define EOM_EOM_MASK                              0x00000001
#define EOM_EOM_SHIFT                             0

//Bitfield Constants Register CEC_INTR_STATUS0
#define INTR_STATUS0_RCVD_DATA_BIT_ERR_1_MASK              0x00000080
#define INTR_STATUS0_RCVD_DATA_BIT_ERR_1_SHIFT             7
#define INTR_STATUS0_RCVD_DATA_BIT_ERR_0_MASK              0x00000040
#define INTR_STATUS0_RCVD_DATA_BIT_ERR_0_SHIFT             6
#define INTR_STATUS0_RX_COMPLETE_MASK                      0x00000020
#define INTR_STATUS0_RX_COMPLETE_SHIFT                     5
#define INTR_STATUS0_TX_COMPLETE_MASK                      0x00000010
#define INTR_STATUS0_TX_COMPLETE_SHIFT                     4
#define INTR_STATUS0_SIG_FREE_TIME_FAIL_MASK               0x00000008
#define INTR_STATUS0_SIG_FREE_TIME_FAIL_SHIFT              3
#define INTR_STATUS0_COLL_DETECT_MASK                      0x00000004
#define INTR_STATUS0_COLL_DETECT_SHIFT                     2
#define INTR_STATUS0_NOACK_BROADCAST_MASK                  0x00000002
#define INTR_STATUS0_NOACK_BROADCAST_SHIFT                 1
#define INTR_STATUS0_NOACK_UNICAST_MASK                    0x00000001
#define INTR_STATUS0_NOACK_UNICAST_SHIFT                   0



//Bitfield Constants Register CEC_INTR_STATUS1
#define INTR_STATUS1_WAKE_UP_INTR_MASK                     0x00000010
#define INTR_STATUS1_WAKE_UP_INTR_SHIFT                    4
#define INTR_STATUS1_RX_FIFO_EMPTY_MASK                    0x00000008
#define INTR_STATUS1_RX_FIFO_EMPTY_SHIFT                   3
#define INTR_STATUS1_RX_FIFO_FULL_MASK                     0x00000004
#define INTR_STATUS1_RX_FIFO_FULL_SHIFT                    2
#define INTR_STATUS1_TX_FIFO_FULL_MASK                     0x00000002
#define INTR_STATUS1_TX_FIFO_FULL_SHIFT                    1



//Bitfield Constants Register CEC_NOMINAL_SAMPLE_TIME_0
#define NOMINAL_SAMPLE_TIME_0_NOM_SST_BYTE0_MASK                    0x000000FF
#define NOMINAL_SAMPLE_TIME_0_NOM_SST_BYTE0_SHIFT                   0


//Bitfield Constants Register CEC_NOMINAL_SAMPLE_TIME_1
#define NOMINAL_SAMPLE_TIME_1_NOM_SST_BYTE1_MASK                    0x000000FF
#define NOMINAL_SAMPLE_TIME_1_NOM_SST_BYTE1_SHIFT                   0

//Bitfield Constants Register CEC_NOMINAL_SAMPLE_TIME_2
#define NOMINAL_SAMPLE_TIME_2_NOM_SST_BYTE2_MASK                    0x000000FF
#define NOMINAL_SAMPLE_TIME_2_NOM_SST_BYTE2_SHIFT                   0


//Bitfield Constants Register CEC_NOMINAL_SAMPLE_TIME_3
#define NOMINAL_SAMPLE_TIME_3_NOM_SST_BYTE3_MASK                    0x000000FF
#define NOMINAL_SAMPLE_TIME_3_NOM_SST_BYTE3_SHIFT                   0


//Bitfield Constants Register CEC_HYSTERESIS_TIME_0
#define HYSTERESIS_TIME_0_HYST_CNT_BYTE0_MASK                   0x000000FF
#define HYSTERESIS_TIME_0_HYST_CNT_BYTE0_SHIFT                  0


//Bitfield Constants Register CEC_HYSTERESIS_TIME_1
#define HYSTERESIS_TIME_1_HYST_CNT_BYTE1_MASK                   0x000000FF
#define HYSTERESIS_TIME_1_HYST_CNT_BYTE1_SHIFT                  0


//Bitfield Constants Register CEC_HYSTERESIS_TIME_2
#define HYSTERESIS_TIME_2_HYST_CNT_BYTE2_MASK                   0x000000FF
#define HYSTERESIS_TIME_2_HYST_CNT_BYTE2_SHIFT                  0

//Bitfield Constants Register CEC_HYSTERESIS_TIME_3
#define HYSTERESIS_TIME_3_HYST_CNT_BYTE3_MASK                   0x000000FF
#define HYSTERESIS_TIME_3_HYST_CNT_BYTE3_SHIFT                  0

//Bitfield Constants Register CEC_FOLLOWER_ACK_TIME_0
#define FOLLOWER_ACK_TIME_0_FOLL_ACK_BYTE0_MASK                   0x000000FF
#define FOLLOWER_ACK_TIME_0_FOLL_ACK_BYTE0_SHIFT                  0


//Bitfield Constants Register CEC_FOLLOWER_ACK_TIME_1
#define FOLLOWER_ACK_TIME_1_FOLL_ACK_BYTE1_MASK                   0x000000FF
#define FOLLOWER_ACK_TIME_1_FOLL_ACK_BYTE1_SHIFT                  0

//Bitfield Constants Register CEC_FOLLOWER_ACK_TIME_2
#define FOLLOWER_ACK_TIME_2_FOLL_ACK_BYTE2_MASK                   0x000000FF
#define FOLLOWER_ACK_TIME_2_FOLL_ACK_BYTE2_SHIFT                  0

//Bitfield Constants Register CEC_FOLLOWER_ACK_TIME_3
#define FOLLOWER_ACK_TIME_3_FOLL_ACK_BYTE3_MASK                   0x000000FF
#define FOLLOWER_ACK_TIME_3_FOLL_ACK_BYTE3_SHIFT                  0


//Bitfield Constants Register CEC_RX_BUF_READ
#define RX_BUF_READ_RX_READ_MASK                          0x000000FF
#define RX_BUF_READ_RX_READ_SHIFT                         0


//Bitfield Constants Register CEC_RX_EOM_READ
#define RX_EOM_READ_RX_EOM_MASK                           0x000000FF
#define RX_EOM_READ_RX_EOM_SHIFT                          0


//Bitfield Constants Register CEC_LOGICAL_ADDR0
#define LOGICAL_ADDR0_LOG_ADDR0_EN_MASK                     0x00000010
#define LOGICAL_ADDR0_LOG_ADDR0_EN_SHIFT                    4
#define LOGICAL_ADDR0_LOG_ADDR0_MASK                        0x0000000F
#define LOGICAL_ADDR0_LOG_ADDR0_SHIFT                       0


//Bitfield Constants Register CEC_LOGICAL_ADDR1
#define LOGICAL_ADDR1_LOG_ADDR1_EN_MASK                     0x00000010
#define LOGICAL_ADDR1_LOG_ADDR1_EN_SHIFT                    4
#define LOGICAL_ADDR1_LOG_ADDR1_MASK                        0x0000000F
#define LOGICAL_ADDR1_LOG_ADDR1_SHIFT                       0


//Bitfield Constants Register CEC_LOGICAL_ADDR2
#define LOGICAL_ADDR2_LOG_ADDR2_EN_MASK                     0x00000010
#define LOGICAL_ADDR2_LOG_ADDR2_EN_SHIFT                    4
#define LOGICAL_ADDR2_LOG_ADDR2_MASK                        0x0000000F
#define LOGICAL_ADDR2_LOG_ADDR2_SHIFT                       0

//Bitfield Constants Register CEC_LOGICAL_ADDR3
#define LOGICAL_ADDR3_LOG_ADDR3_EN_MASK                     0x00000010
#define LOGICAL_ADDR3_LOG_ADDR3_EN_SHIFT                    4
#define LOGICAL_ADDR3_LOG_ADDR3_MASK                        0x0000000F
#define LOGICAL_ADDR3_LOG_ADDR3_SHIFT                       0

//Bitfield Constants Register CEC_LOGICAL_ADDR4
#define LOGICAL_ADDR4_LOG_ADDR4_EN_MASK                     0x00000010
#define LOGICAL_ADDR4_LOG_ADDR4_EN_SHIFT                    4
#define LOGICAL_ADDR4_LOG_ADDR4_MASK                        0x0000000F
#define LOGICAL_ADDR4_LOG_ADDR4_SHIFT                       0


//Bitfield Constants Register CEC_JITTER_CNT_0
#define JITTER_CNT_0_JITTER_CNT_BYTE0_MASK                 0x000000FF
#define JITTER_CNT_0_JITTER_CNT_BYTE0_SHIFT                0

//Bitfield Constants Register CEC_JITTER_CNT_1
#define JITTER_CNT_1_JITTER_CNT_BYTE1_MASK                 0x000000FF
#define JITTER_CNT_1_JITTER_CNT_BYTE1_SHIFT                0


//Bitfield Constants Register CEC_JITTER_CNT_2
#define JITTER_CNT_2_JITTER_CNT_BYTE2_MASK                 0x000000FF
#define JITTER_CNT_2_JITTER_CNT_BYTE2_SHIFT                0


//Bitfield Constants Register CEC_JITTER_CNT_3
#define JITTER_CNT_3_JITTER_CNT_BYTE3_MASK                 0x000000FF
#define JITTER_CNT_3_JITTER_CNT_BYTE3_SHIFT                0


//Bitfield Constants Register CEC_LINE_STATUS
#define LINE_STATUS_CEC_LINE_STATUS_MASK                  0x00000001
#define LINE_STATUS_CEC_LINE_STATUS_SHIFT                 0



//Bitfield Constants Register CEC_TX_PRESENT_STATE
#define TX_PRESENT_STATE_TX_STATE_MASK                         0x0000003F
#define TX_PRESENT_STATE_TX_STATE_SHIFT                        0


//Bitfield Constants Register CEC_RX_PRESENT_STATE
#define RX_PRESENT_STATE_RX_STATE_MASK                         0x0000003F
#define RX_PRESENT_STATE_RX_STATE_SHIFT                        0


//Bitfield Constants Register CEC_COLL_CTRL
#define COLL_CTRL_TR_TIMER_EN_1_MASK                    0x00000002
#define COLL_CTRL_TR_TIMER_EN_1_SHIFT                   1
#define COLL_CTRL_TR_TIMER_EN_0_MASK                    0x00000001
#define COLL_CTRL_TR_TIMER_EN_0_SHIFT                   0


//Bitfield Constants Register CEC_GLITCH_FILTER_STAGES
#define GLITCH_FILTER_STAGES_GLITCH_WIDTH7_0_MASK                  0x000000FF
#define GLITCH_FILTER_STAGES_GLITCH_WIDTH7_0_SHIFT                 0


//Bitfield Constants Register CEC_COLL_WINDOW_TIME_0
#define COLL_WINDOW_TIME_0_COLL_WIN_TIME_BYTE0_MASK              0x000000FF
#define COLL_WINDOW_TIME_0_COLL_WIN_TIME_BYTE0_SHIFT             0


//Bitfield Constants Register CEC_COLL_WINDOW_TIME_1
#define COLL_WINDOW_TIME_1_COLL_WIN_TIME_BYTE1_MASK              0x000000FF
#define COLL_WINDOW_TIME_1_COLL_WIN_TIME_BYTE1_SHIFT             0



//Bitfield Constants Register CEC_COLL_WINDOW_TIME_2
#define COLL_WINDOW_TIME_2_COLL_WIN_TIME_BYTE2_MASK              0x000000FF
#define COLL_WINDOW_TIME_2_COLL_WIN_TIME_BYTE2_SHIFT             0


//Bitfield Constants Register CEC_COLL_WINDOW_TIME_3
#define COLL_WINDOW_TIME_3_COLL_WIN_TIME_BYTE3_MASK              0x000000FF
#define COLL_WINDOW_TIME_3_COLL_WIN_TIME_BYTE3_SHIFT             0


//Bitfield Constants Register CEC_TX_FIFO_FULL_THRESH
#define TX_FIFO_FULL_THRESH_TX_FIFO_FULL_THRESH_MASK              0x0000000F
#define TX_FIFO_FULL_THRESH_TX_FIFO_FULL_THRESH_SHIFT             0

//Bitfield Constants Register CEC_TX_FIFO_WPTR
#define TX_FIFO_WPTR_TX_FIFO_WPTR_MASK                     0x0000000F
#define TX_FIFO_WPTR_TX_FIFO_WPTR_SHIFT                    0


//Bitfield Constants Register CEC_TX_FIFO_RPTR
#define TX_FIFO_RPTR_TX_FIFO_RPTR_MASK                     0x0000000F
#define TX_FIFO_RPTR_TX_FIFO_RPTR_SHIFT                    0

//Bitfield Constants Register CEC_TX_FIFO_DPTR
#define TX_FIFO_DPTR_TX_FIFO_DPTR_MASK                     0x0000000F
#define TX_FIFO_DPTR_TX_FIFO_DPTR_SHIFT                    0

//Bitfield Constants Register CEC_RX_FIFO_FULL_THRESH
#define RX_FIFO_FULL_THRESH_RX_FIFO_FULL_THRESH_MASK              0x0000000F
#define RX_FIFO_FULL_THRESH_RX_FIFO_FULL_THRESH_SHIFT             0

//Bitfield Constants Register CEC_RX_FIFO_WPTR
#define RX_FIFO_WPTR_RX_FIFO_WPTR_MASK                     0x0000000F
#define RX_FIFO_WPTR_RX_FIFO_WPTR_SHIFT                    0

//Bitfield Constants Register CEC_RX_FIFO_RPTR
#define RX_FIFO_RPTR_RX_FIFO_RPTR_MASK                     0x0000000F
#define RX_FIFO_RPTR_RX_FIFO_RPTR_SHIFT                    0

//Bitfield Constants Register CEC_RX_FIFO_DPTR
#define RX_FIFO_DPTR_RX_FIFO_DPTR_MASK                     0x0000000F
#define RX_FIFO_DPTR_RX_FIFO_DPTR_SHIFT                    0




#endif
