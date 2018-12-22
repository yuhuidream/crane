#ifndef __PMUD_H__
#define __PMUD_H__

typedef struct
{
   __IO uint32_t       RESERVED0[(0x2000)/4];
   __IO uint32_t       DPCR;//0X2000
   __IO uint32_t       RESERVED1;
   __IO uint32_t       DPSR ;
   __IO uint32_t       DCGR ;
   __IO uint32_t       RESERVED2[(0x2018-0x200C)/4-1] ;
   __IO uint32_t       DPRR     ;
   __IO uint32_t       DRSR     ;//0X201C
   __IO uint32_t       RESERVED3[(0x2100-0x201c)/4-1];
   __IO uint32_t       DMPR[5]         ;
 

}PMUD_Typedef;

#define    PMUD_BASE      0xFFA80000


#define     PMUD  ((  PMUD_TypeDef *) PMUD_BASE )


#endif












