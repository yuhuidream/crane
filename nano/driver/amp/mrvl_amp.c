#include "bbu.h"
#include "CIU.h"
#include "PMUM.h"

#define DMA_MAX_BYTES_PER_DESC 0x1F00

int release_seagull(void)
{
    int time_out = 0x10000;
    CP_RELEASE_FLAG = CR5_IN_RESET;
#ifdef CONFIG_NATO
//    memcpy((void *)0x0, (void *)0x100000, 0x10000);
//    memcpy((void *)0x820000, (void *)0x108000, 0x10000);
    *(volatile unsigned long*) 0xD4282C04 &= ~0x1; 
    BU_REG_WRITE(0xd4282888, 0x18060002);
    BU_REG_WRITE(0xd4282888, 0x08060002);
#else
    //CIU->CR5_CONF &= ~CR5_CONF_NCPUHALT0;  //halt CR5
    CIU->CR5_CONF |= BIT_9;   //Enable ATCM at reset
    CIU->CR5_CONF |= BIT_10;  //Enable BTCM at reset
    CIU->CR5_CONF |= BIT_11;  //Set ATCM base address as 0
    CIU->CR5_CONF |= CR5_CONF_VINITHI0; //Set CR5 boot from high vector
    //CIU->SW_BRANCH_ADDR = 0x200000;
    //PMUM->SGADDRMAP &= ~BIT_16;
    //PMUM->SGADDRMAP = (0x200000 >> 16) | BIT_16;    
    PMUM->APRR &= ~BIT_0;    //release CR5

    //if((*(volatile uint32_t *)0x1FF0000 & 0xFF) != 0xDB){
    //    dma_m2m_nano_xfer(0x100000, 0xB0010000, 0x10000, DMA_MAX_BYTES_PER_DESC, 2);
    //    dma_m2m_nano_xfer(0x108000, 0xB0020000, 0x10000, DMA_MAX_BYTES_PER_DESC, 2);
    //}
    /* resume the CR5 CPU */
    //CIU->CR5_CONF |= CR5_CONF_NCPUHALT0;
#endif

    while(time_out--){
        if(CP_RELEASE_FLAG == CR5_RELEASED ){
            return 1;
        }
    }
    return 0;
}

#if 0
int bbu_release_seagull(int argc, uint8_t **argv)
{
    (void)argc;
    (void)argv;
    int result;
    bbu_printf("Start to release Seagull\n\r");
    result = release_seagull();
    if(result == 1)
        bbu_printf("Seagull boot up successfully\n\r");
    else
        bbu_printf("TIME OUT: Seagull boot up fail\n\r");
}

int ddr_init(int argc, uint8_t **argv)
{
    (void)argc;
    (void)argv;
    bbu_printf("Config script: ttd2-ddr3-20MHzDCLK_new.xdb.\n\r");
    bbu_printf("**** Begin SoC DDR controller / PHY initialization. \n\r");
    bbu_printf("**** DDR controller Configuration for platform:     \n\r");
    bbu_printf("**** DDR controller Configuration for device:     \n\r");
    bbu_printf("****    Timings based on tCK speed: 400 MHz. Check SoC PLL setup to verify DCLK frequency. \n\r");
    //AP_PMU set for AHB reset release
    BU_REG_WRITE(0xD42828e8, BU_REG_READ(0xD42828e8) | 0x1);
    
    BU_REG_WRITE(0xC0100000+0x010, 0x000D0001);     //____MEMORY_ADDRESS_MAP_CS0
    BU_REG_WRITE(0xC0100000+0x020, 0x00046430);     //____SDRAM_CONFIG_TYPE1-CS0
    BU_REG_WRITE(0xC0100000+0x030, 0x00000000);     //____SDRAM_CONFIG_TYPE2-CS0
    
    BU_REG_WRITE(0xC0100000+0x0080, 0x8CD4009C);    //____SDRAM_TIMING1
    BU_REG_WRITE(0xC0100000+0x0084, 0x63660064);    //SDRAM_TIMING2
    BU_REG_WRITE(0xC0100000+0x0088, 0xC2004453);    //SDRAM_TIMING3
    BU_REG_WRITE(0xC0100000+0x0088, 0xC2004453);  //SDRAM_TIMING3
    BU_REG_WRITE(0xC0100000+0x008C, 0x24F84187);  //SDRAM_TIMING4
    BU_REG_WRITE(0xC0100000+0x0090, 0x000E2100);  //SDRAM_TIMING5
    BU_REG_WRITE(0xC0100000+0x0094, 0x44040200);  //SDRAM_TIMING6
    BU_REG_WRITE(0xC0100000+0x0098, 0x00005501);  //SDRAM_TIMING7
    bbu_printf("1\n\r");
    BU_REG_WRITE(0xC0100000+0x0054, 0x00000000);    //SDRAM_CTRL2
    BU_REG_WRITE(0xC0100000+0x0058, 0x00C08409);    //SDRAM_CTRL4
    
    //BU_REG_WRITE(0xC0100000+, );        //
    //ODT only enabled for Writes to SDRAM for CS0
    BU_REG_WRITE(0xC0100000+0x005C, 0x00000001);        //SDRAM_CTRL6_SDRAM_ODT_CTRL
    BU_REG_WRITE(0xC0100000+0x0060, 0x00000002);        //SDRAM_CTRL7_SDRAM_ODT_CTRL2
    BU_REG_WRITE(0xC0100000+0x0064, 0xffff0000);        //SDRAM_CTRL13
    bbu_printf("2\n\r");

    //Missing from spread sheet
    BU_REG_WRITE(0xC0100000+0x0100, 0x000FF1FC);        //Exclusive Monitor Control
    BU_REG_WRITE(0xC0100000+0x0180, 0x00000000);        //CM_WRITE_PROTECTION

    //_PHY Deskew PLL config and PHY initialization____________________________
    BU_REG_WRITE(0xC0100000+0x0240, 0x00300008);        //PHY_CTRL11

    BU_REG_WRITE(0xC0100000+0x024C, 0x80000000);        //PHY_CTRL14
    
    // disabled auto cal
    BU_REG_WRITE(0xC0100000+0x023C, 0x0000311C);        //PHY_CTRL10

    // schmoo "44"
    BU_REG_WRITE(0xC0100000+0x0220, 0x00004411);        //PHY_CTRL3

    BU_REG_WRITE(0xC0100000+0x0230, 0x17700779);        //PHY_CTRL7
    
    BU_REG_WRITE(0xC0100000+0x0234, 0x0FF00770);        //PHY_CTRL8
    
    BU_REG_WRITE(0xC0100000+0x0238, 0x3F000077);        //PHY_CTRL9

    //Using Master DLL value below for 400MHz
    BU_REG_WRITE(0xC0100000+0x0248, 0xF0800008);        //PHY_CTRL13

    //#_DLL reset / update________________________________________________
    //#  DLL Reset causes master delay to calculate 90degree point.  Need this after any DCLK freq change
    BU_REG_WRITE(0xC0100300, 0x00000000);               //PHY DATA BYTE SEL
    BU_REG_WRITE(0xC0100000+0x0304, 0x00000104);        //PHY DATA BYTE SEL
    BU_REG_WRITE(0xC0100300, 0x00000001);               //PHY DATA BYTE SEL
    BU_REG_WRITE(0xC0100000+0x0304, 0x00000104);        //PHY DATA BYTE SEL
    BU_REG_WRITE(0xC0100300, 0x00000002);               //PHY DATA BYTE SEL
    BU_REG_WRITE(0xC0100000+0x0304, 0x00000104);        //PHY DATA BYTE SEL
    BU_REG_WRITE(0xC0100300, 0x00000003);               //PHY DATA BYTE SEL
    BU_REG_WRITE(0xC0100000+0x0304, 0x00000104);        //PHY DATA BYTE SEL
    bbu_printf("4\n\r");
    //Read Leveling CS0
    //to disable dqs gating write 0x100 etc to 0x380 
    //read level control, change tap delay & cycles. 
    bbu_printf("5\n\r");
    
    //__Initialize DDR______________________________________________________
    // init command to both chip selects
    BU_REG_WRITE(0xC0100000+0x0160, 0x01000001);    //USER_INITIATED_COMMAND0
    bbu_printf("6\n\r");
    while((BU_REG_READ(0xC0100008) & 0x1) == 0);
    bbu_printf("7\n\r");
    BU_REG_WRITE(0xC0100000+0x0058, BU_REG_READ(0xC0100000+0x0058) | 0x80000000);
    BU_REG_WRITE(0xC0100000+0x160, 0x01000200);     //MEMORY_ADDRESS_MAP_CS0
    BU_REG_WRITE(0xC0100000+0x0050, BU_REG_READ(0xC0100000+0x0050) & 0xFFFFFFBF);
    BU_REG_WRITE(0xC0100000+0x0058, 0x80C08409);    //MEMORY_ADDRESS_MAP_CS0
    BU_REG_WRITE(0xC0100000+0x160, 0x01000100);     //MEMORY_ADDRESS_MAP_CS0
    BU_REG_WRITE(0xC0100000+0x160, 0x01000400);     //MEMORY_ADDRESS_MAP_CS0
    BU_REG_WRITE(0xC0100000+0x220, 0x00004420);     //MEMORY_ADDRESS_MAP_CS0

    //DDR Control Latency
    BU_REG_WRITE(0xC0100000+0x220, 0x00004422);
    //DQ_MUX_EN
    BU_REG_WRITE(0xD4280000+0x2c40, BU_REG_READ(0xD4280000+0x2c40) | 0x80000000);
    //__All done - move along____________________________________________
    bbu_printf("**** End of SoC DDR controller / PHY initialization. \n\r");
    //mdelay(1000);
}
#endif
