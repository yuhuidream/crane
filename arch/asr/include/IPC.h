#ifndef __IPC_H__
#define __IPC_H__

typedef struct
{

    __IO uint32_t ISRR;
    __IO uint32_t WDR;
    __IO uint32_t  ISRW;
    __IO uint32_t  ICR;    
    __IO uint32_t  IIR;
    __IO uint32_t  RDR;



}IPC_TypeDef;




#define    IPCA_BASE      0xD401D000
#define    IPCC_BASE      0xD401D800
#define    IPCMSA_BASE    0xD403C000
#define    IPCCP_BASE     0xF020B000


#define    IPCA  (( IPC_TypeDef *) IPCA_BASE )
#define    IPCC  (( IPC_TypeDef *) IPCC_BASE )
#define    IPCMSA  (( IPC_TypeDef *) IPCMSA_BASE )
#define    IPCCP  (( IPC_TypeDef *) IPCCP_BASE )




//Bitfield Constants Register IPC_ISRR
#define ISRR_APB_IPC_ISRR_MASK                     0x000007FF
#define ISRR_APB_IPC_ISRR_SHIFT                    0

//Bitfield Constants Register IPC_WDR
#define WDR_APB_IPC_WDR_MASK                      0xFFFFFFFF
#define WDR_APB_IPC_WDR_SHIFT                     0

//Bitfield Constants Register IPC_ISRW
#define ISRW_APB_IPC_ISRW_MASK                     0x000007FF
#define ISRW_APB_IPC_ISRW_SHIFT                    0

//Bitfield Constants Register IPC_ICR
#define ICR_APB_IPC_ICR_MASK                      0x000007FF
#define ICR_APB_IPC_ICR_SHIFT                     0

//Bitfield Constants Register IPC_IIR
#define IIR_APB_IPC_IIR_MASK                      0x000007FF
#define IIR_APB_IPC_IIR_SHIFT                     0

//Bitfield Constants Register IPC_RDR
#define RDR_APB_IPC_RDR_MASK                      0xFFFFFFFF
#define RDR_APB_IPC_RDR_SHIFT                     0



#endif
