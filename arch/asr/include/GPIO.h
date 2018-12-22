#ifndef __GPIO_H__
#define __GPIO_H__

typedef struct
{

   __IO uint32_t PLR;//0X00  offset
   __IO uint32_t RESERVED0[(0x0c)/4-1];             
   __IO uint32_t PDR;//0X0C  offset
   __IO uint32_t RESERVED1[(0x18-0x0c)/4-1];
   __IO uint32_t PSR;//0X18  offset
   __IO uint32_t RESERVED2[(0x24-0x18)/4-1];
   __IO uint32_t PCR;//0X24  offset
   __IO uint32_t RESERVED3[(0x30-0x24)/4-1];
   __IO uint32_t RER;//0X30  offset
   __IO uint32_t RESERVED4[(0x3C-0x30)/4-1];
   __IO uint32_t FER;//0X3C  offset
   __IO uint32_t RESERVED5[(0x48-0x3C)/4-1];
   __IO uint32_t EDR;//0X48  offset
   __IO uint32_t RESERVED6[(0x54-0x48)/4-1];
   __IO uint32_t SDR;//0X54  offset
   __IO uint32_t RESERVED7[(0x60-0x54)/4-1];
   __IO uint32_t CDR;//0X60  offset
   __IO uint32_t RESERVED8[(0x6C-0x60)/4-1];
   __IO uint32_t SRER;//0X6C  offset
   __IO uint32_t RESERVED9[(0x78-0x6C)/4-1];
   __IO uint32_t CRER;//0X78  offset
   __IO uint32_t RESERVED10[(0x84-0x78)/4-1];
   __IO uint32_t SFER;//0X84  offset
   __IO uint32_t RESERVED11[(0x90-0x84)/4-1];
   __IO uint32_t CFER;//0X90  offset
   __IO uint32_t RESERVED12[(0x9C-0x90)/4-1];
   __IO uint32_t APMASK_REG;//0X9C  offset
   __IO uint32_t RESERVED13[(0xA8-0x9C)/4-1];
   __IO uint32_t EXTPROCMASK_REG;//0XA8  offset
 


}GPIO_TypeDef;




#define    GPIO0_BASE      0xD4019000
#define    GPIO1_BASE      0xD4019004
#define    GPIO2_BASE      0xD4019008
#define    GPIO3_BASE      0xD4019100
#define    GPIO4_BASE      0xD4019104
#define    GPIO5_BASE      0xD4019108

#define    MRVL_GPIO0  (( GPIO_TypeDef *) GPIO0_BASE )
#define    MRVL_GPIO1  (( GPIO_TypeDef *) GPIO1_BASE )
#define    MRVL_GPIO2  (( GPIO_TypeDef *) GPIO2_BASE )
#define    MRVL_GPIO3  (( GPIO_TypeDef *) GPIO3_BASE )
#define    MRVL_GPIO4  (( GPIO_TypeDef *) GPIO4_BASE )
#define    MRVL_GPIO5  (( GPIO_TypeDef *) GPIO5_BASE )


#endif
