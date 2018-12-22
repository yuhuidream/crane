#ifndef __RIPC_H__
#define __RIPC_H__

typedef struct
{

    __IO uint32_t RIPCx_STATUS_REG;
    __IO uint32_t RIPCx_INT1_REG;
    __IO uint32_t RIPCx_INT2_REG;
    __IO uint32_t RIPCx_INT3_REG;



}RIPC_TypeDef;


#define RIPC0_BASE 0xD403D000
#define RIPC1_BASE 0xD403D100
#define RIPC2_BASE 0xD403D200
#define RIPC3_BASE 0xD403D300

#define    RIPC0  (( RIPC_TypeDef *) RIPC0_BASE )
#define    RIPC1  (( RIPC_TypeDef *) RIPC1_BASE )
#define    RIPC2  (( RIPC_TypeDef *) RIPC2_BASE )
#define    RIPC3  (( RIPC_TypeDef *) RIPC3_BASE )

#define    RIPC_NUM       (4)


//Bitfield Constants Register RIPCx_STATUS_REG
#define RIPCX_STATUS_REG_STATUS_MASK                           0x00000001
#define RIPCX_STATUS_REG_STATUS_SHIFT                          0

//Bitfield Constants Register RIPCx_INT1_REG
#define RIPCX_INT1_REG_INT_REQ_MASK                          0x00000001
#define RIPCX_INT1_REG_INT_REQ_SHIFT                         0



//Bitfield Constants Register RIPCx_INT2_REG
#define RIPCX_INT2_REG_INT_REQ_MASK                          0x00000001
#define RIPCX_INT2_REG_INT_REQ_SHIFT                         0



//Bitfield Constants Register RIPCx_INT3_REG
#define RIPCX_INT3_REG_INT_REQ_MASK                          0x00000001
#define RIPCX_INT3_REG_INT_REQ_SHIFT                         0




#endif
