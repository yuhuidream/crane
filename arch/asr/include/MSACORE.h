#ifndef __MSACORE_H__
#define __MSACORE_H__

typedef struct
{
    __IO uint32_t      RESERVED0;
    __IO uint32_t      DMEM_CONTROL;
    __IO uint32_t      DATA_FAULT_STATUS;
    __IO uint32_t      DATA_FAULT_ADDR;
    __IO uint32_t      RESERVED1[(0x0100-0x000C)/4-1];//
    __IO uint32_t      DCPLB_ADDR[(0x3C)/4+1];//0X0100-0X013C
    __IO uint32_t      RESERVED2[(0x0200-0x013C)/4-1];
    __IO uint32_t      DCPLB_DATA[(0x3C)/4+1];//0X0200-0x023C
    __IO uint32_t      RESERVED3[(0x0300-0x023C)/4-1];
union{
    __IO uint32_t      DTEST_COMMAND_NONECACHE;
    __IO uint32_t      DTEST_COMMAND_CACHE;  //0x0300
};

    __IO uint32_t      RESERVED4[0X100/4-1];
union{
    __IO uint32_t      DTEST DATA0_NONECACHE;
    __IO uint32_t      DTEST DATA0_CACHE;//0x0400
};
union{
    __IO uint32_t      DTEST DATA1_NONECACHE;
    __IO uint32_t      DTEST DATA1_CACHE;//0x0404
};

    __IO uint32_t      RESERVED5[(0X500-0x0404)/4-1];
    __IO uint32_t      DMEM_TESTMODE;
    __IO uint32_t      RESERVED6[(0x1004-0x0500)/4-1];
    __IO uint32_t      IMEM_CONTROL;
    __IO uint32_t      RESERVED7[(0x1100-0x1004)/4-1];
    __IO uint32_t      ICPLB_ADDR[(0x3C)/4+1];//0X1100-0X113C
    __IO uint32_t      RESERVED8[(0x0200-0x013C)/4-1];
    __IO uint32_t      ICPLB_DATA[(0x3C)/4+1];//Offset: 0x1200-0x123C
    __IO uint32_t      RESERVED9[(0x0300-0x023C)/4-1];
union{
    __IO uint32_t      ITEST_COMMAND_NONECACHE;
    __IO uint32_t      ITEST_COMMAND_CACHE;  //0x1300
};

    __IO uint32_t      RESERVED10[0X100/4-1];
union{
    __IO uint32_t      ITEST DATA0_NONECACHE;
    __IO uint32_t      ITEST DATA0_CACHE;//0x1400
};
union{
    __IO uint32_t      ITEST DATA1_NONECACHE;
    __IO uint32_t      ITEST DATA1_CACHE;//0x1404
};

    __IO uint32_t      RESERVED5[(0X500-0x0404)/4-1];
    __IO uint32_t      IMEM_TESTMODE;



} MSACORE_TypeDef;




#define    MSACORE_BASE         0xFFE00000


#define     MSACORE  ((  MSACORE_TypeDef *) MSACORE_BASE )














#endif
