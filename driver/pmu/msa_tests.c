#include "bbu.h"
#include "PMUM.h"

#define MSA_CODE_DDR_ADDR  0x3600000
#define MSA_FLAG      0xD100FFFC
#define MSA_TEST      0xD100FFF8
#define MSA_RESULT    0xD100FFFC
#define MSA_TEST_ARG  0xD100FFF4
//0xD040E149 P0.h = 0xD040
//0x0000E109 P0.l = 0
//0x20000061 call(P0)
//0xA000E109
//0xE1092000  //jump 0
#define AIB_spare0_reg       *(volatile uint32_t *)0xd401e7f8
#define AIB_spare1_reg       *(volatile uint32_t *)0xd401e7fc

int release_msa_in_ddr(void)
{
	uint32_t  dummy_read_apb = 0;
	int time_out = 0x1000000;
	BU_REG_WRITE(MSA_FLAG, 0x111);
	BU_REG_WRITE(MSA_TEST, 0);

	*(volatile uint32_t *)(0xD4051050 ) = ((MSA_CODE_DDR_ADDR & 0xFE000000) >> 25) | 0x80;
	*(volatile uint32_t *)(0xd4070000) = 0xd2000000 + (MSA_CODE_DDR_ADDR & 0x1FFFFFF);

	AIB_spare1_reg = 0x49; //reset default is 0x49

    *(volatile uint32_t *)(0xd405003c) &= 0xfffffffc;
    dummy_read_apb = *(volatile uint32_t *)(0xd405003c);

    PMUM->CPRR |= 0x2C;
    dummy_read_apb = PMUM->CPRR;
    bbu_printf("Step 0\n\r");
    udelay(512); //500us
    bbu_printf("Step 1\n\r");

	// MCB DDR cacheable, for NeZha
	//*(volatile unsigned long *)0xB0000540 |= 1<<25;

	
	//BU_REG_WRITE(0x1FFFFF0, 0xD0E0E149); 
	//BU_REG_WRITE(0x1FFFFF4, 0x0000E109);
	//BU_REG_WRITE(0x1FFFFF8, 0x20000061);
	//BU_REG_WRITE(0xD4070000, 0xD0FFFFF0); //Boot Address
	
    PMUM->CPRR &= ~(1 << 3);
    dummy_read_apb = PMUM->CPRR;
    PMUM->CPRR &= ~(1 << 2);
    dummy_read_apb = PMUM->CPRR;
    bbu_printf("Step 2\n\r");
    udelay(32); //20us
    PMUM->CPRR &= ~(1 << 5);
    dummy_read_apb = PMUM->CPRR;
    bbu_printf("Step 3\n\r");

	while(1)
	{
		if(BU_REG_READ(MSA_FLAG) == 0xdeadbeef)
		{
			bbu_printf("MSA boot success\n\r");
			return 0;
		}
		time_out --;
		if(time_out == 0)
		{
			bbu_printf("MSA boot fail\n\r");
			return 1;
		}
	}
}


//MPHY test is not usable since NZ MPHY does not support such selftest
#if 0
int __msa_mphy_test(void)
{
	int time_out = 0xFFFFFFFF;
	int result = 0;
	BU_REG_WRITE(MSA_TEST_ARG, 1);
	BU_REG_WRITE(MSA_TEST, 1); //write 1 to tell msa to run mphy_test
	while(1){
		if(BU_REG_READ(MSA_TEST) == 0)
		{
			result = BU_REG_READ(MSA_RESULT);
			return result;
		}
		time_out --;
		if(time_out == 0)
		{
			serial_outstr("MSA MPHY test timeout\n\r");
			return 1;
		}
	}
	return 0;
}
#endif


int __msa_tests(int test_id)
{
	int time_out = 0xFFFFFFFF;
	int result = 0;
	BU_REG_WRITE(MSA_TEST, test_id); //write test_id to tell msa to run acording test
	if(test_id == 9)
	{
		return 0;
	}
	while(1){
		if(BU_REG_READ(MSA_TEST) == 0)
		{
			result = BU_REG_READ(MSA_RESULT);
			bbu_printf("MSA Test done\n\rError code: %d\n\r",result);
			return result;
		}
		time_out --;
		if(time_out == 0)
		{
			bbu_printf("MSA test timeout\n\r");
			return 1;
		}
	}
	return 0;
}

void msa_tests(int argc, uint8_t **argv)
{
	int test_id, test_arg1;
	if(argc < 2)
	{
		bbu_printf("Error\n\r");
		return;
	}

	if(argc > 2)
	{
		test_arg1 = conv_dec(argv[2]);
		BU_REG_WRITE(MSA_TEST_ARG,test_arg1);
	}

	test_id = conv_dec(argv[1]);
	switch(test_id)
	{
		case 0:
			bbu_printf(" MSA Boot test\n\r");
			if(release_msa_in_ddr())
				bbu_printf("Test FAIL\n\r");
			else
				bbu_printf("Test PASS\n\r");
			return;
			break;
		case 1:
			bbu_printf("MSA L1 Caches/RAMs test\n\r");
			break;
		case 2:
			bbu_printf("MSA L2 SRAM test\n\r");
			break;
		case 3:
			bbu_printf("MSA Exponent test\n\r");
			break;
		case 4:
			bbu_printf("Measure MSA frequency\n\r");
			break;
		case 5:
			bbu_printf("Read AP timer count\n\r");
			break;
		case 6:
			bbu_printf("MSA frequency change\n\r");
			break;
		case 7:
			bbu_printf("Dump memory configurations\n\r");
			break;
		case 8:
			bbu_printf("DDR read/write test\n\r");
			break;
		case 9:
			bbu_printf("Test MSA enter D2\n\r");
			break;
        }

        if(__msa_tests(test_id))
                bbu_printf("Test FAIL\n\r");
        else
                bbu_printf("Test PASS\n\r");
}
