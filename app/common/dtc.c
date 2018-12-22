#include "bbu.h"
#define REG32(x)   (*(volatile uint32_t *)(x))

void dtc_ch0_isr(void *arg)
{
    bbu_printf("DTC CH0 ISR\n\r");
    bbu_printf("Value in 0xD4284070: 0x%08X\n\r", BU_REG_READ(0xD4285070));
}

void dtc_ch1_isr(void *arg)
{
    bbu_printf("DTC CH1 ISR\n\r");
    bbu_printf("Value in 0xD4284070: 0x%08X\n\r", BU_REG_READ(0xD4285070));
}

void dtc_error_isr(void *arg)
{
    bbu_printf("DTC ERROR ISR\n\r");
    bbu_printf("Value in 0xD4284070: 0x%08X\n\r", BU_REG_READ(0xD4285070));
}


int dtc_snow_3g_test1(void)
{
    uint32_t des_addr, IK_addr, data_addr;
    uint32_t *addr;
    uint32_t val;
    int time_out = 1000;
    int ret = SCREEN_TEST_FIAL;

    des_addr = (uint32_t)bbu_vm_malloc(0x10000, 0);
    data_addr = des_addr + 0x1000;
    IK_addr   = data_addr + 0x1000;

    bbu_printf("des_addr: 0x%08x\n\r", des_addr);
    bbu_printf("data_addr: 0x%08x\n\r", data_addr);
    bbu_printf("IK_addr: 0x%08x\n\r", IK_addr);

    addr = (uint32_t *)des_addr;
    *addr++ = 0x00000000;
    *addr++ = data_addr;
    *addr++ = 0x00000000;
    *addr++ = 0x004200A8;
    *addr++ = 0x08000000;
    *addr++ = 0x00000000;
    *addr++ = 0x0000003A;
    *addr++ = IK_addr;
    addr = (uint32_t *)des_addr;
    for(int i = 0; i < 8; i++)
    {
        bbu_printf("0x%08X: 0x%08X\n\r", (uint32_t)addr, *addr);
        addr++;
    }

    addr = (uint32_t *)IK_addr;
    *addr++ = 0x2c930e24;
    *addr++ = 0xb1efb54f;
    *addr++ = 0x9b13c6d0;
    *addr++ = 0x8c76d414;

    addr = (uint32_t *)IK_addr;
    for(int i = 0; i < 4; i++)
    {
        bbu_printf("0x%08X: 0x%08X\n\r", (uint32_t)addr, *addr);
        addr++;
    }

    addr = (uint32_t *)data_addr;
    *addr++ = 0x2402481a;
    *addr++ = 0xe7ca2af2;
    *addr++ = 0xd339f606;
    *addr++ = 0x4433b13c;
    *addr++ = 0x810c8596;
    *addr++ = 0x000000e0;
    addr = (uint32_t *)data_addr;
    for(int i = 0; i < 6; i++)
    {
        bbu_printf("0x%08X: 0x%08X\n\r", (uint32_t)addr, *addr);
        addr++;
    }
    REG32(0xD02500f0) = 0xfffeffff;

    BU_REG_WRITE(0xD4285004, 0x00010fc4);  //DOCR_CTRL0
    BU_REG_WRITE(0xD4285010, 0x40000200);  //DOCR_SW_BYPASS
    BU_REG_WRITE(0xD4285078, des_addr);    //DOCR_CBA0
    BU_REG_WRITE(0xD4285040, 0x1);         //CHAIN_LST_INC
    bbu_printf("DTC done\n\r");

    bbu_vm_free(des_addr);

    while(time_out --){
        val = BU_REG_READ(0xD4285070);
        if(val == 0x23A80DB8)
        {
            ret = SCREEN_TEST_PASS;
            break;
        }
    }

    BU_REG_WRITE(0xD4285004, 0x00008fc4);  //DOCR_CTRL0, clear descripter counter
//    BU_REG_WRITE(0xD4285040, 0x1);         //CHAIN_LST_INC

    if(time_out == 0)
    {
        bbu_printf("DTC Snow-3G Test FAIL\n\r");
    }else{
        bbu_printf("DTC Snow-3G Test PASS\n\r");
    }


    return ret;
}


int dtc_snow_3g_test2(void)
{
    int i, ret;
    uint32_t val;
    uint32_t addr = bbu_vm_malloc(0x100000, 0);
    uint32_t addr_bk = addr;

    uint32_t ck_addr  = addr + 0x4ac0;   //0x07f14ac0;
    REG32(ck_addr)   = 0x2c930e24;
    REG32(ck_addr+4) = 0xb1efb54f;
    REG32(ck_addr+8) = 0x9b13c6d0;
    REG32(ck_addr+12)= 0x8c76d414;

    addr += 0x10000;

    uint32_t data_addr = addr + 0x8024;//0x07f38024;
    for(i=0;i<100;i++){
        REG32(data_addr+4*i) = 0x2402481a;
    }

    addr += 0x10000;

    uint32_t docr_cba0  = addr + 0x4cc0; //0x07f24cc0;

    uint32_t des_addr0  = docr_cba0+32*0;//78 
    uint32_t des_addr1  = docr_cba0+32*1;//79
    uint32_t des_addr2  = docr_cba0+32*2;//34
    uint32_t des_addr3  = docr_cba0+32*3;//36
    uint32_t des_addr4  = docr_cba0+32*4;//80
    uint32_t des_addr5  = docr_cba0+32*5;//38
    uint32_t des_addr6  = docr_cba0+32*6;//81
    uint32_t des_addr7  = docr_cba0+32*7;//40
    uint32_t des_addr8  = docr_cba0+32*8;//82
    uint32_t des_addr9  = docr_cba0+32*9;//83

    addr += 0x10000;
    uint32_t nxt_addr0  = addr + 0x4cc0;      //0x07f44cc0;//35
    uint32_t nxt_addr1  = addr + 0x4d00;      //0x07f44d00;//37
    uint32_t nxt_addr2  = addr + 0x4d40;      //0x07f44d40;//39
    uint32_t nxt_addr3  = addr + 0x4d80;      //0x07f44d80;//41

    addr += 0x10000;

    uint32_t dst_addr0  = addr + 0x09aa;      //0x075e09aa;//78 
    uint32_t dst_addr1  = addr + 0x0a2a;      //0x075e0a2a;//79
    uint32_t dst_addr2  = addr + 0x0008;      //0x07460008;//34
    uint32_t dst_addr3  = addr + 0x4008;      //0x07464008;//36
    uint32_t dst_addr4  = addr + 0x0ae2;      //0x075e0ae2;//80
    uint32_t dst_addr5  = addr + 0x8008;      //0x07468008;//38
    uint32_t dst_addr6  = addr + 0x0b62;      //0x075e0b62;//81
    uint32_t dst_addr7  = addr + 0xc008;      //0x0746c008;//40
    uint32_t dst_addr8  = addr + 0x0c22;      //0x075e0c22;//82
    uint32_t dst_addr9  = addr + 0x0d2a;      //0x075e0d2a;//83

   REG32(des_addr0+28) = ck_addr;
   REG32(des_addr0+24) = 0x4e;
   REG32(des_addr0+20) = 0x0;
   REG32(des_addr0+16) = 0x0;
   REG32(des_addr0+12) = 0x804101b0;
   REG32(des_addr0+8 ) = dst_addr0;
   REG32(des_addr0+4 ) = data_addr;
   REG32(des_addr0)    = 0x00000000;//78

   REG32(des_addr1+28) = ck_addr;
   REG32(des_addr1+24) = 0x4f;
   REG32(des_addr1+20) = 0x0;
   REG32(des_addr1+16) = 0x0;
   REG32(des_addr1+12) = 0x80410348;
   REG32(des_addr1+8 ) = dst_addr1;
   REG32(des_addr1+4 ) = data_addr;
   REG32(des_addr1)    = 0x00000000;//79

   REG32(des_addr2+28) = ck_addr;
   REG32(des_addr2+24) = 0x14;
   REG32(des_addr2+20) = 0x0;
   REG32(des_addr2+16) = 0x0;
   REG32(des_addr2+12) = 0xd8410228;
   REG32(des_addr2+8 ) = dst_addr2;
   REG32(des_addr2+4 ) = data_addr;
   REG32(des_addr2)    = nxt_addr0;//34
   REG32(nxt_addr0+28) = ck_addr;
   REG32(nxt_addr0+24) = 0x14;
   REG32(nxt_addr0+20) = 0x0;
   REG32(nxt_addr0+16) = 0x0;
   REG32(nxt_addr0+12) = 0x88410048;
   REG32(nxt_addr0+8 ) = 0x00000000;
   REG32(nxt_addr0+4 ) = data_addr;
   REG32(nxt_addr0)    = 0x00000000;//35

   REG32(des_addr3+28) = ck_addr;
   REG32(des_addr3+24) = 0x15;
   REG32(des_addr3+20) = 0x0;
   REG32(des_addr3+16) = 0x0;
   REG32(des_addr3+12) = 0xd8410268;
   REG32(des_addr3+8 ) = dst_addr3;
   REG32(des_addr3+4 ) = data_addr;
   REG32(des_addr3)    = nxt_addr1;//36 
   REG32(nxt_addr1+28) = ck_addr;
   REG32(nxt_addr1+24) = 0x15;
   REG32(nxt_addr1+20) = 0x0;
   REG32(nxt_addr1+16) = 0x0;
   REG32(nxt_addr1+12) = 0x88410040;
   REG32(nxt_addr1+8 ) = 0x00000000;
   REG32(nxt_addr1+4 ) = data_addr;
   REG32(nxt_addr1)    = 0x00000000;//37

   REG32(des_addr4+28) = ck_addr;
   REG32(des_addr4+24) = 0x50;
   REG32(des_addr4+20) = 0x0;
   REG32(des_addr4+16) = 0x0;
   REG32(des_addr4+12) = 0x804101a0;
   REG32(des_addr4+8 ) = dst_addr4;
   REG32(des_addr4+4 ) = data_addr;
   REG32(des_addr4)    = 0x00000000;//80

   REG32(des_addr5+28) = ck_addr;
   REG32(des_addr5+24) = 0x16;
   REG32(des_addr5+20) = 0x0;
   REG32(des_addr5+16) = 0x0;
   REG32(des_addr5+12) = 0xd8410268;
   REG32(des_addr5+8 ) = dst_addr5;
   REG32(des_addr5+4 ) = data_addr;
   REG32(des_addr5)    = nxt_addr2;//38
   REG32(nxt_addr2+28) = ck_addr;
   REG32(nxt_addr2+24) = 0x16;
   REG32(nxt_addr2+20) = 0x0;
   REG32(nxt_addr2+16) = 0x0;
   REG32(nxt_addr2+12) = 0x88410148;
   REG32(nxt_addr2+8 ) = 0x00000000;
   REG32(nxt_addr2+4 ) = data_addr;
   REG32(nxt_addr2)    = 0x00000000;//39

   REG32(des_addr6+28) = ck_addr;
   REG32(des_addr6+24) = 0x51;
   REG32(des_addr6+20) = 0x0;
   REG32(des_addr6+16) = 0x0;
   REG32(des_addr6+12) = 0x804103c0;
   REG32(des_addr6+8 ) = dst_addr6;
   REG32(des_addr6+4 ) = data_addr;
   REG32(des_addr6)    = 0x00000000;//81

   REG32(des_addr7+28) = ck_addr;
   REG32(des_addr7+24) = 0x17;
   REG32(des_addr7+20) = 0x0;
   REG32(des_addr7+16) = 0x0;
   REG32(des_addr7+12) = 0xd8410528;
   REG32(des_addr7+8 ) = dst_addr7;
   REG32(des_addr7+4 ) = data_addr;
   REG32(des_addr7)    = nxt_addr3;//40
   REG32(nxt_addr3+28) = ck_addr;
   REG32(nxt_addr3+24) = 0x17;
   REG32(nxt_addr3+20) = 0x0;
   REG32(nxt_addr3+16) = 0x0;
   REG32(nxt_addr3+12) = 0x88410008;
   REG32(nxt_addr3+8 ) = 0x00000000;
   REG32(nxt_addr3+4 ) = data_addr;
   REG32(nxt_addr3)    = 0x00000000;//41

   REG32(des_addr8+28) = ck_addr;
   REG32(des_addr8+24) = 0x52;
   REG32(des_addr8+20) = 0x0;
   REG32(des_addr8+16) = 0x0;
   REG32(des_addr8+12) = 0x804105f0;
   REG32(des_addr8+8 ) = dst_addr8;
   REG32(des_addr8+4 ) = data_addr;
   REG32(des_addr8)    = 0x00000000;//82

   REG32(des_addr9+28) = ck_addr;
   REG32(des_addr9+24) = 0x53;
   REG32(des_addr9+20) = 0x0;
   REG32(des_addr9+16) = 0x0;
   REG32(des_addr9+12) = 0x804106d8;
   REG32(des_addr9+8 ) = dst_addr9;
   REG32(des_addr9+4 ) = data_addr;
   REG32(des_addr9)    = 0x00000000;//83
   REG32(0xD02500f0) = 0xfffeffff;

//control register
    REG32(0xD4285010) = 0x40000200;  //DOCR_SW_BYPASS
    REG32(0xD4285078) = docr_cba0;  //DOCR_CBA0
    REG32(0xD4285004) = 0x00028fc4;  //DOCR_CTRL0

    for(i=0;i<10;i++){
       REG32(0xD4285040) = 0x1;  //CHAIN_LST_INC 
    }

    //wait for dtc interrupt
    bbu_vm_free(addr_bk);

    mdelay(100);
    val = REG32(0xD4285040);
    BU_REG_WRITE(0xD4285004, 0x00008fc4);
    //bbu_printf("val 0x%08x\n\r", val);
    if((((val >> 1)&0xFF) == 0xA) && (((val >> 9)&0xFF) == 0xA))
    {
        bbu_printf("DTC test finishes\n\r");
    }else{
        bbu_printf("DTC test doesn't finish\n\r");
        return SCREEN_TEST_FIAL;
    }

    val = REG32(0xD4285060);
    if((val & 0x1F) == 0)
    {
        bbu_printf("No Error, test PASS\n\r");
        ret = SCREEN_TEST_PASS;
    }else{
        bbu_printf("Error, test FAIL\n\r");
        ret = SCREEN_TEST_FIAL;
    }

    return ret;

}


int dtc_test(int argc, uint8_t **argv)
{
    uint32_t val;
    int ret = SCREEN_TEST_FIAL;
    ISR_Connect(IRQ_MMP_DTC_CH0, dtc_ch0_isr, NULL);
    INT_Enable(IRQ_MMP_DTC_CH0, IRQ_ROUTE, 15);
    ISR_Connect(IRQ_MMP_DTC_CH1, dtc_ch1_isr, NULL);
    INT_Enable(IRQ_MMP_DTC_CH1, IRQ_ROUTE, 15);
    ISR_Connect(IRQ_MMP_DTC_ERROR, dtc_error_isr, NULL);
    INT_Enable(IRQ_MMP_DTC_ERROR, IRQ_ROUTE, 15);
    
    int test = 0;
    if(argc == 2)
    {
        test = conv_dec((char *)argv[1]);
    }

    if(test > 2)
    {
        bbu_printf("Test index is out of range\n\r");
        return SCREEN_TEST_FIAL;
    }

    val = BU_REG_READ(0xD0250068);
    val |= (1<<5);
    BU_REG_WRITE(0xD0250068, val);

    val = BU_REG_READ(0xD025006C);
    val |= (1<<5);
    BU_REG_WRITE(0xD025006C, val);

    BU_REG_WRITE(0xD027000C, 0x3);

    BU_REG_WRITE(0xD02500C8, 0xffffffff);

    switch(test){
        case 0:
            bbu_printf("\n\rDTC Snow-3G integrity test\n\r");
            ret = dtc_snow_3g_test1();
            break;
        case 1:
            bbu_printf("\n\rDTC Snow-3G encode-decode test\n\r");
            ret = dtc_snow_3g_test2();
            break;
        default:
            bbu_printf("Un-supported test\n\r");
            break;
    }

    return ret;
        
}