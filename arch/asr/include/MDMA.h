#ifndef __MDMA_H__
#define __MDMA_H__

typedef struct
{
  
    __IO uint32_t CHAN_0_BYTE_CNT;
    __IO uint32_t CHAN_1_BYTE_CNT;
    __IO uint32_t RESERVED0[2];
    __IO uint32_t CHAN_0_SRC_ADDR;
    __IO uint32_t CHAN_1_SRC_ADDR;
    __IO uint32_t RESERVED1[2];
    __IO uint32_t CHAN_0_DEST_ADDR;
    __IO uint32_t CHAN_1_DEST_ADDR;
    __IO uint32_t RESERVED2[2];
    __IO uint32_t CHAN_0_NEXT_DESC_PTR;
    __IO uint32_t CHAN_1_NEXT_DESC_PTR;
    __IO uint32_t RESERVED3[2];
    __IO uint32_t CHAN_0_CTRL;
    __IO uint32_t CHAN_1_CTRL; //0X044 OFFSET
    __IO uint32_t RESERVED4[(0x060-0x044)/4-1];
    __IO uint32_t CHAN_PRI;//0X060 OFFSET
    __IO uint32_t RESERVED5[3];
    __IO uint32_t CHAN_0_CURR_DESC_PTR;//0X070 OFFSET
    __IO uint32_t CHAN_1_CURR_DESC_PTR;
    __IO uint32_t RESERVED6[2];
    __IO uint32_t CHAN_0_INT_MASK;
    __IO uint32_t CHAN_1_INT_MASK;  //0X084
    __IO uint32_t RESERVED7[(0x0A0-0x084)/4-1];
    __IO uint32_t CHAN_0_INT_STATUS;//0X0A0 
    __IO uint32_t CHAN_1_INT_STATUS;
    __IO uint32_t CHAN_0_FILL_DATA;
    __IO uint32_t CHAN_1_FILL_DATA;




}MDMA_TypeDef;


#define    MDMA_BASE      0xD42A0A00


#define    MDMA  (( MDMA_TypeDef *) MDMA_BASE )



//Bitfield Constants Register MDMA_CHAN_0_CTRL
#define CHAN_0_CTRL_FILLMOD_MASK                          0x02000000
#define CHAN_0_CTRL_FILLMOD_SHIFT                         25
#define CHAN_0_CTRL_ABR_MASK                              0x00100000
#define CHAN_0_CTRL_ABR_SHIFT                             20
#define CHAN_0_CTRL_CDE_MASK                              0x00020000
#define CHAN_0_CTRL_CDE_SHIFT                             17
#define CHAN_0_CTRL_CHANACT_MASK                          0x00004000
#define CHAN_0_CTRL_CHANACT_SHIFT                         14
#define CHAN_0_CTRL_FETCHND_MASK                          0x00002000
#define CHAN_0_CTRL_FETCHND_SHIFT                         13
#define CHAN_0_CTRL_CHANEN_MASK                           0x00001000
#define CHAN_0_CTRL_CHANEN_SHIFT                          12
#define CHAN_0_CTRL_TRANSMOD_MASK                         0x00000800
#define CHAN_0_CTRL_TRANSMOD_SHIFT                        11
#define CHAN_0_CTRL_INTMODE_MASK                          0x00000400
#define CHAN_0_CTRL_INTMODE_SHIFT                         10
#define CHAN_0_CTRL_CHAINMOD_MASK                         0x00000200
#define CHAN_0_CTRL_CHAINMOD_SHIFT                        9
#define CHAN_0_CTRL_BURSTLIMIT_MASK                       0x000001C0
#define CHAN_0_CTRL_BURSTLIMIT_SHIFT                      6
#define CHAN_0_CTRL_DESTDIR_MASK                          0x00000030
#define CHAN_0_CTRL_DESTDIR_SHIFT                         4
#define CHAN_0_CTRL_SRCDIR_MASK                           0x0000000C
#define CHAN_0_CTRL_SRCDIR_SHIFT                          2




//Bitfield Constants Register MDMA_CHAN_1_CTRL
#define CHAN_1_CTRL_FILLMOD_MASK                          0x02000000
#define CHAN_1_CTRL_FILLMOD_SHIFT                         25
#define CHAN_1_CTRL_ABR_MASK                              0x00100000
#define CHAN_1_CTRL_ABR_SHIFT                             20
#define CHAN_1_CTRL_CDE_MASK                              0x00020000
#define CHAN_1_CTRL_CDE_SHIFT                             17
#define CHAN_1_CTRL_CHANACT_MASK                          0x00004000
#define CHAN_1_CTRL_CHANACT_SHIFT                         14
#define CHAN_1_CTRL_FETCHND_MASK                          0x00002000
#define CHAN_1_CTRL_FETCHND_SHIFT                         13
#define CHAN_1_CTRL_CHANEN_MASK                           0x00001000
#define CHAN_1_CTRL_CHANEN_SHIFT                          12
#define CHAN_1_CTRL_TRANSMOD_MASK                         0x00000800
#define CHAN_1_CTRL_TRANSMOD_SHIFT                        11
#define CHAN_1_CTRL_INTMODE_MASK                          0x00000400
#define CHAN_1_CTRL_INTMODE_SHIFT                         10
#define CHAN_1_CTRL_CHAINMOD_MASK                         0x00000200
#define CHAN_1_CTRL_CHAINMOD_SHIFT                        9
#define CHAN_1_CTRL_BURSTLIMIT_MASK                       0x000001C0
#define CHAN_1_CTRL_BURSTLIMIT_SHIFT                      6
#define CHAN_1_CTRL_DESTDIR_MASK                          0x00000030
#define CHAN_1_CTRL_DESTDIR_SHIFT                         4
#define CHAN_1_CTRL_SRCDIR_MASK                           0x0000000C
#define CHAN_1_CTRL_SRCDIR_SHIFT                          2



//Bitfield Constants Register MDMA_CHAN_0_INT_STATUS
#define CHAN_0_INT_STATUS_DECODE_ERR_MASK                       0x00000008
#define CHAN_0_INT_STATUS_DECODE_ERR_SHIFT                      3
#define CHAN_0_INT_STATUS_SECURE_ERR_MASK                       0x00000004
#define CHAN_0_INT_STATUS_SECURE_ERR_SHIFT                      2
#define CHAN_0_INT_STATUS_COMP_MASK                             0x00000001
#define CHAN_0_INT_STATUS_COMP_SHIFT                            0



//Bitfield Constants Register MDMA_CHAN_1_INT_STATUS
#define CHAN_1_INT_STATUS_DECODE_ERR_MASK                       0x00000008
#define CHAN_1_INT_STATUS_DECODE_ERR_SHIFT                      3
#define CHAN_1_INT_STATUS_SECURE_ERR_MASK                       0x00000004
#define CHAN_1_INT_STATUS_SECURE_ERR_SHIFT                      2
#define CHAN_1_INT_STATUS_COMP_MASK                             0x00000001
#define CHAN_1_INT_STATUS_COMP_SHIFT                            0










#endif
