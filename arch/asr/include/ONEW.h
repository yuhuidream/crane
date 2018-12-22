#ifndef __ONEW_H__
#define __ONEW_H__

typedef struct
{
   __IO uint32_t W1CMDR;
   __IO uint32_t W1TRR;
   __IO uint32_t W1INTR;
   __IO uint32_t W1IER;
   __IO uint32_t W1CDR;  //0X010
   __IO uint32_t RESERVED0[(0x30)/4-1];
   __IO uint32_t SDI_BUF0;//0X040
   __IO uint32_t SDI_BUF1;
   __IO uint32_t SDI_BUF2;
   __IO uint32_t SDI_BUF3;//0X04C
   __IO uint32_t RESERVED1[(0x060-0x04C)/4-1];
   __IO uint32_t SDI_BIT_CNT;//0X060
   __IO uint32_t SDI_CTRL;


}ONEW_TypeDef;




#define    ONEW_BASE      0xD4011800


#define    ONEW  (( ONEW_TypeDef *) ONEW_BASE )


//Bitfield Constants Register ONEW_W1CMDR
#define W1CMDR_DQI_MASK                              0x00000008
#define W1CMDR_DQI_SHIFT                             3
#define W1CMDR_DQO_MASK                              0x00000004
#define W1CMDR_DQO_SHIFT                             2
#define W1CMDR_SRA_MASK                              0x00000002
#define W1CMDR_SRA_SHIFT                             1
#define W1CMDR_ONE_WR_MASK                           0x00000001
#define W1CMDR_ONE_WR_SHIFT                          0






//Bitfield Constants Register ONEW_W1INTR
#define W1INTR_RSRF_MASK                             0x00000020
#define W1INTR_RSRF_SHIFT                            5
#define W1INTR_RBF_MASK                              0x00000010
#define W1INTR_RBF_SHIFT                             4
#define W1INTR_TEMT_MASK                             0x00000008
#define W1INTR_TEMT_SHIFT                            3
#define W1INTR_TBE_MASK                              0x00000004
#define W1INTR_TBE_SHIFT                             2
#define W1INTR_PDR_MASK                              0x00000002
#define W1INTR_PDR_SHIFT                             1
#define W1INTR_PD_MASK                               0x00000001
#define W1INTR_PD_SHIFT                              0


//Bitfield Constants Register ONEW_W1IER
#define W1IER_DQOE_MASK                             0x00000080
#define W1IER_DQOE_SHIFT                            7
#define W1IER_ERSF_MASK                             0x00000020
#define W1IER_ERSF_SHIFT                            5
#define W1IER_ERBF_MASK                             0x00000010
#define W1IER_ERBF_SHIFT                            4
#define W1IER_ETMT_MASK                             0x00000008
#define W1IER_ETMT_SHIFT                            3
#define W1IER_ETBE_MASK                             0x00000004
#define W1IER_ETBE_SHIFT                            2
#define W1IER_EPD_MASK                              0x00000001
#define W1IER_EPD_SHIFT                             0

//Bitfield Constants Register ONEW_W1CDR
#define W1CDR_DIV_MASK                              0x0000001C
#define W1CDR_DIV_SHIFT                             2
#define W1CDR_PRE_MASK                              0x00000003
#define W1CDR_PRE_SHIFT                             0

//Bitfield Constants Register SDI_CTRL
#define ONEWIRE_SDI_CTRL_SDI_POLARITY_MASK                     0x00000008
#define ONEWIRE_SDI_CTRL_SDI_POLARITY_SHIFT                    3
#define ONEWIRE_SDI_CTRL_SDI_INT_STS_MASK                      0x00000004
#define ONEWIRE_SDI_CTRL_SDI_INT_STS_SHIFT                     2
#define ONEWIRE_SDI_CTRL_SDI_INT_EN_MASK                       0x00000002
#define ONEWIRE_SDI_CTRL_SDI_INT_EN_SHIFT                      1
#define ONEWIRE_SDI_CTRL_SDI_EN_MASK                           0x00000001
#define ONEWIRE_SDI_CTRL_SDI_EN_SHIFT                          0





#endif
