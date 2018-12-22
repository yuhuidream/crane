#include "predefines.h"
#include <string.h>
#include "bbu.h"
#include "pmic.h"
#include "timer_dec.h"
#include "CIU.h"
#include "common_tests.h"
#include "mcu_generate_pattern.h"
#include "clock.h"

#define NO_LOG
#define DLL_SWP_BOUNCH
#ifdef  DLL_SWP_BOUNCH
#define SIMPLE_TEST_DATA_SIZE  (0x10000)
#define USE_OLD_WINDOW_ALGO
#endif

#define	EMEI_MCU_BASE_ADDRESS	0xC0100000
#define ZPNR_OFFSET 0x3

uint32_t zpr_znr=0x18, uiOffset=0x3;
static uint32_t shadow_zpnr = 0x12;

int phs0_phs1_equal = 0;
int cur_byte_lane = 0;
int cur_ods = 2;
int is_dll_bypass = 0;
int use_mannual_drv = 0;
int is_perbyte_swp = 0;
int cur_pp_idx = 0;
int cur_ods_index = 2;
int default_ddrv = 2;
extern int ods_swp_st;
extern int ods_swp_end;

void  set_phs_val(uint32_t phasel, uint32_t phasel1, int byte_map);
static void ddr_reset(void);
extern unsigned int z_d_value;
extern int highest_pp;
int zpr_need_update = 0;
char update_str_pattern[] = "------------>WRITE";

static struct dswp_result_all swp_result_all[NUM_PP_IDX] = {
	[PP_800_IDX] = {
		.dswp_result[2] = {
			.is_bypass_mode = 0,
			.phasel_pass_st[0] = 0,
			.phasel_pass_st[1] = 0,
			.phasel_pass_st[2] = 0,
			.phasel_pass_st[3] = 0,
			.phasel_pass_end[0] = 40,
			.phasel_pass_end[1] = 40,
			.phasel_pass_end[2] = 40,
			.phasel_pass_end[3] = 40,
			.phasel1_pass_st[0] = 8,
			.phasel1_pass_st[1] = 8,
			.phasel1_pass_st[2] = 8,
			.phasel1_pass_st[3] = 8,
			.phasel1_pass_end[0] = 48,
			.phasel1_pass_end[1] = 48,
			.phasel1_pass_end[2] = 48,
			.phasel1_pass_end[3] = 48,
			.dqs_pass_st = 0,
			.dqs_pass_end = 40,
			.dlldly_pass_st = 16,
			.dlldly_pass_end = 208,	
		},
	},
}; 

uint32_t org_clk_dqs_dq_val = 0x0;
uint32_t org_reg_404 = 0x0;

static uint32_t org_reg_700;
static uint32_t org_reg_704;
static uint32_t org_reg_708;
static uint32_t org_reg_70c;
void ddr_swp_init()
{
	org_clk_dqs_dq_val = BU_REG_READ(0xc0100580);
	org_reg_404 = BU_REG_READ(0xc0100404);
	org_reg_700 = BU_REG_READ(0xc0100700);
	org_reg_704 = BU_REG_READ(0xc0100704);
	org_reg_708 = BU_REG_READ(0xc0100708);
	org_reg_70c = BU_REG_READ(0xc010070c);
}

static int dqs_update = 0;

uint32_t restore_wck_dqs_dq_val(void)
{
	if(dqs_update == 1)
	{
		bbu_printf("dqs need update\n\r");
		return 0;
	}

	BU_REG_WRITE(0xc0100580, org_clk_dqs_dq_val);
	BU_REG_WRITE(0xc0100584, org_clk_dqs_dq_val);
	BU_REG_WRITE(0xc0100588, org_clk_dqs_dq_val);
	BU_REG_WRITE(0xc010058C, org_clk_dqs_dq_val);

	BU_REG_WRITE(0xc01005A0, org_clk_dqs_dq_val);
	BU_REG_WRITE(0xc01005A4, org_clk_dqs_dq_val);
	BU_REG_WRITE(0xc01005A8, org_clk_dqs_dq_val);
	BU_REG_WRITE(0xc01005AC, org_clk_dqs_dq_val);	
}

void restore_ca_dly_value(void)
{
	BU_REG_WRITE(0xc0100700, org_reg_700);
	BU_REG_WRITE(0xc0100704, org_reg_704);
	BU_REG_WRITE(0xc0100708, org_reg_708);
	BU_REG_WRITE(0xc010070c, org_reg_70c);
}

int phasel_index = 0;
void dump_mck_regs(void)
{
	  bbu_printf("0xc0100580: 0x%08X\n\r",BU_REG_READ(0xc0100580));
}

static void dll_swp_prepare(void)
{
   	BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_3)));
    	BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_2)));
    	BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_0)));
    	BU_REG_WRITE(0xc0100420,(BU_REG_READ(0xc0100420)&(~BIT_30)));
    	mdelay(4);
}

static void ddr_set_phasel_phasel1(uint32_t phasel, uint32_t phasel1,int byte_map)
{
    	uint32_t i, org_phasel0, org_phasel1;
	uint32_t value,byte_addr;

	org_phasel0 = (BU_REG_READ(0xc0100500)&0x3F0000) >> 16;
    	org_phasel1 = (BU_REG_READ(0xc0100500)&0x3F000000) >> 24;
	if(phasel == 0xffffffff)
		phasel = org_phasel0;
	if(phasel1 == 0xffffffff)
		phasel1 = org_phasel1;

    	for(i = 0; i < 4; i++)
    	{
	    if((BIT_0 << i)&byte_map) {
	        byte_addr = (i*4);
	       	byte_addr += 0xc0100500;
	    	value = BU_REG_READ(byte_addr);
	    	value &= 0xc0c0fffc;
	    	value |= ((phasel<< 16)|(phasel1<< 24));
	    	BU_REG_WRITE(byte_addr,value);
	    } 
    	}
    

	BU_REG_WRITE(0xc0100420,BU_REG_READ(0xc0100420)|(BIT_29));
	mdelay(1); 
	BU_REG_WRITE(0xc0100420,BU_REG_READ(0xc0100420)|(BIT_30));
	mdelay(2);
	bbu_printf("phsl: %d, phsl1: %d\n\r",phasel,phasel1);
}

uint32_t cur_reg_404 = 0x0;

void ddr_set_drv_val(uint32_t z_d_val, uint32_t zpr, uint32_t znr, int half_test, int zpnr_only)
{	
	uint32_t value;
	uint32_t zpr_4lsb,zpr_bit4,znr_4lsb,znr_bit4;

	if(zpr > 31)
		zpr = 31;
	if(znr > 31)
		znr = 31;

	zpr_4lsb = (zpr & 0xF); 
	zpr_bit4 = (zpr & BIT_4);
	znr_4lsb = (znr & 0xF); 
	znr_bit4 = (znr & BIT_4);	
	value = BU_REG_READ(0xc0100404);
	value &= 0x3ffff00f;
	if(!zpnr_only) {
		value &=0x300ff00f;
		value |=((z_d_val << 20)|(z_d_val << 24));
	}
 	value |= (zpr_4lsb <<4);
 	value |= (znr_4lsb << 8);
 	value |= ((znr_bit4 << (31-4)) | (zpr_bit4 << (30-4)));
 	BU_REG_WRITE(0xc0100404,value);
	cur_reg_404 = value;

 	value = BU_REG_READ(0xc0100404);
 	if(half_test) {
 		dump_mck_regs();
 		return;
 	}
	value = BU_REG_READ(0xc0100408);
	value &=0x3ffff00f;
	if(!zpnr_only) {
		value &=0x300ff00f;
		value |=((z_d_val << 20)|(z_d_val << 24));
	}
 	value |= (zpr_4lsb <<4);
 	value |= (znr_4lsb << 8);
 	value |= ((znr_bit4 << (31-4)) | (zpr_bit4 << (30-4)));
 	BU_REG_WRITE(0xc0100408,value);
 	value = BU_REG_READ(0xc0100408);

	value = BU_REG_READ(0xc010040C);
 	value &= 0xFFFFFC00;
	if(!zpnr_only) {
		value &= 0xfffC0000;
		value |= (z_d_val << 10);
		value |= (z_d_val << 14);
	}
 	value |= (zpr_4lsb << 0);
 	value |= (znr_4lsb << 4);
 	value |= ((znr_bit4 << (8-4)) | (zpr_bit4 << (9-4)));	
 	BU_REG_WRITE(0xc010040C,value);
 	value = BU_REG_READ(0xc010040C);

	dump_mck_regs();
	mdelay(1);	 
}

void set_dqs_val(uint32_t dq_dly, uint32_t dqs_dly)
{
	uint32_t value;

	value = BU_REG_READ(0xc0100580);
	value &= 0xffff0000;
	value |= ((dqs_dly<<8)|dq_dly);
	BU_REG_WRITE(0xc0100580,value);	
    	BU_REG_WRITE(0xc0100584,value);
    	BU_REG_WRITE(0xc0100588,value);
    	BU_REG_WRITE(0xc010058c,value);

	value = BU_REG_READ(0xc01005a0);
	value &= 0xffff0000;
	value |= ((dqs_dly<<8)|dq_dly);
	BU_REG_WRITE(0xc01005a0,value);
    	BU_REG_WRITE(0xc01005a4,value);
    	BU_REG_WRITE(0xc01005a8,value);
    	BU_REG_WRITE(0xc01005ac,value);
    	mdelay(1); 
 	bbu_printf("***0xc0100580: 0x%08X***\n\r",BU_REG_READ(0xc0100580));
}

void set_dqsc_val(uint32_t dq_dly, uint32_t dqs_dly)
{
	uint32_t value;

	value = BU_REG_READ(0xc0100580);
	value &= 0x0000ffff;
	value |= ((dqs_dly<<24)|dq_dly << 16);

	BU_REG_WRITE(0xc0100580,value);	
    	BU_REG_WRITE(0xc0100584,value);
    	BU_REG_WRITE(0xc0100588,value);
    	BU_REG_WRITE(0xc010058c,value);

	value = BU_REG_READ(0xc01005a0);
	value &= 0x0000ffff;
	value |= ((dqs_dly<<24)|dq_dly << 16);
	BU_REG_WRITE(0xc01005a0,value);	
    	BU_REG_WRITE(0xc01005a4,value);
    	BU_REG_WRITE(0xc01005a8,value);
    	BU_REG_WRITE(0xc01005ac,value);
    	mdelay(1);
 	bbu_printf("***0xc0100580: 0x%08X***\n\r",BU_REG_READ(0xc0100580));
}

void set_dqs_val_2(uint32_t dqs0[2],uint32_t dqs1[2],uint32_t dqs2[2],uint32_t dqs3[2])
{

	uint32_t value;

	value = BU_REG_READ(0xc0100580);
	value &= 0xffff0000;
	BU_REG_WRITE(0xc0100580,(value | (dqs0[1]<<8) | (dqs0[0])));	
    	BU_REG_WRITE(0xc0100584,(value | (dqs1[1]<<8) | (dqs1[0])));
    	BU_REG_WRITE(0xc0100588,(value | (dqs2[1]<<8) | (dqs2[0])));
    	BU_REG_WRITE(0xc010058c,(value | (dqs3[1]<<8) | (dqs3[0])));

	value = BU_REG_READ(0xc01005a0);
	value &= 0xffff0000;
	BU_REG_WRITE(0xc01005a0,(value | (dqs0[1]<<8) | (dqs0[0])));	
    	BU_REG_WRITE(0xc01005a4,(value | (dqs1[1]<<8) | (dqs1[0])));	
    	BU_REG_WRITE(0xc01005a8,(value | (dqs2[1]<<8) | (dqs2[0])));	
    	BU_REG_WRITE(0xc01005ac,(value | (dqs3[1]<<8) | (dqs3[0])));

    	mdelay(1); 
    	bbu_printf("***0xc0100580: 0x%08X***\n\r",BU_REG_READ(0xc0100580));
}

void set_dqsc_val_2(uint32_t dqs0[2],uint32_t dqs1[2],uint32_t dqs2[2],uint32_t dqs3[2])
{
	uint32_t value;

	value = BU_REG_READ(0xc0100580);
	value &= 0x0000ffff;
	BU_REG_WRITE(0xc0100580,(value | (dqs0[1] << 24) | (dqs0[0] << 16)));	
    	BU_REG_WRITE(0xc0100584,(value | (dqs1[1] << 24) | (dqs1[0] << 16)));
    	BU_REG_WRITE(0xc0100588,(value | (dqs2[1] << 24) | (dqs2[0] << 16)));
    	BU_REG_WRITE(0xc010058c,(value | (dqs3[1] << 24) | (dqs3[0] << 16)));

	value = BU_REG_READ(0xc01005a0);
	value &= 0x0000ffff;
	BU_REG_WRITE(0xc01005a0,(value | (dqs0[1] << 24) | (dqs0[0] << 16)));	
    	BU_REG_WRITE(0xc01005a4,(value | (dqs1[1] << 24) | (dqs1[0] << 16)));	
    	BU_REG_WRITE(0xc01005a8,(value | (dqs2[1] << 24) | (dqs2[0] << 16)));	
    	BU_REG_WRITE(0xc01005ac,(value | (dqs3[1] << 24) | (dqs3[0] << 16)));

    	mdelay(1); 
    	bbu_printf("***0xc0100580: 0x%08X***\n\r",BU_REG_READ(0xc0100580));
}

void set_dqs_val_3(uint32_t val)
{
	int val1, val2, val3, val4, val5;

	val1 = swp_result_all[cur_pp_idx].dswp_result[cur_ods].dqs_pass_st;
	val2 = swp_result_all[cur_pp_idx].dswp_result[cur_ods].dqs_pass_end;	
	val3 = val & 0xff;
	val4 = (val >> 8) & 0xff;	
	val5 = swp_result_all[cur_pp_idx].dswp_result[cur_ods].dqs_val;
	dqs_update = 0;

	if(val1 > 9) {
		set_dqs_val(val3, val5 + val3 - 10);
		set_dqsc_val(val3, val5 + val3 - 10);
		dqs_update = 1;
		return;			
	} 
	if(val2 < 11)
	{
		set_dqs_val(10, val5);
		set_dqsc_val(10, val5);
		dqs_update = 1;
		return;
	}
	set_dqs_val(val3, val4);
	set_dqsc_val(val3, val4);
}

void set_ca_nonca_val(uint32_t ca_val, uint32_t nonca_val)
{
	uint32_t value1, value2, value3;

	value1 = (ca_val | (ca_val << 4 )|(ca_val << 8 )|(ca_val << 12 )|  
		(ca_val << 16)|(ca_val << 20)|(ca_val << 24)|(ca_val << 28)); 

	value2 = (ca_val | (ca_val << 4 )|(ca_val << 8 )|(ca_val << 12 ));

	value3 =  (nonca_val | (nonca_val << 4 )|(nonca_val << 8 )|(nonca_val << 12 )| 
		(nonca_val << 16)|(nonca_val << 20)|(nonca_val << 24)|(nonca_val << 28));

  	BU_REG_WRITE(0xc0100700,value1);
  	BU_REG_WRITE(0xc0100704,value1);
  	BU_REG_WRITE(0xc0100708,value2);  
  	BU_REG_WRITE(0xc010070c,value3); 

  	BU_REG_WRITE(0xc0100700,value1);  
  	BU_REG_WRITE(0xc0100704,value1);
  	BU_REG_WRITE(0xc0100708,value2);  
  	BU_REG_WRITE(0xc010070c,value3); 
 
	ddr_reset();
  	mdelay(1);
}

void ddr_set_dly(uint32_t dll_val)
{
	uint32_t value;	

	value = BU_REG_READ(0xc0100500);
	value &= 0xffffc00f;
	value |= (dll_val << 4|0x3);
	BU_REG_WRITE(0xc0100500,value);
	BU_REG_READ(0xc0100500);
	value = BU_REG_READ(0xc0100504);
	value &= 0xffffc00f;
	value |= (dll_val << 4|0x3);
	BU_REG_WRITE(0xc0100504,value);
	BU_REG_READ(0xc0100504);
	value = BU_REG_READ(0xc0100508);
	value &= 0xffffc00f;
	value |= (dll_val << 4|0x3);
	BU_REG_WRITE(0xc0100508,value);
	BU_REG_READ(0xc0100508);
	value = BU_REG_READ(0xc010050c);
	value &= 0xffffc00f;
	value |= (dll_val << 4|0x3);
	BU_REG_WRITE(0xc010050c,value);
	BU_REG_READ(0xc010050c);
		
	BU_REG_WRITE(0xc0100420,(BU_REG_READ(0xc0100420)|BIT_30));
	mdelay(1);
	bbu_printf("*dly: %d*\n\r",dll_val);	
}

void  set_phs_val(uint32_t phasel, uint32_t phasel1,int byte_map)
{
	ddr_set_drv_val(z_d_value, zpr_znr, zpr_znr, 0, 1);
	ddr_set_phasel_phasel1(phasel, phasel1,byte_map);
	return;	
}

void set_ods(uint32_t k)
{
    	uint32_t value;
    	
    	if(k > 4)
    		k = default_ddrv;
    		
	value = BU_REG_READ(0xC0100310);
	value &=0xfff0ffff;
	value |=(k <<16);
	BU_REG_WRITE(0xC0100310,value);
	value = BU_REG_READ(0xC0100310);
	
	value = BU_REG_READ(0xC0100314);
	value &=0xfff0ffff;
	value |=(k <<16);
	BU_REG_WRITE(0xC0100314,value);
	value = BU_REG_READ(0xC0100314);
	  		
	if(ddr_is_lpddr2() || ddr_is_lpddr3())
	{
	  	BU_REG_WRITE(0xc0100024,0x11020003);
	}
	  	
	cur_ods = k;

	mdelay(10);
}

int ddr_is_lpddr2()
{
	uint32_t reg;
	
	reg = BU_REG_READ(0xc0100300);
	if((reg&0xF0) == (0x9<<4))
	{
		return 1;
	}
	else
		return 0;
}

int ddr_is_lpddr3()
{
	uint32_t reg;
	
	reg = BU_REG_READ(0xc0100300);
	if((reg&0xF0) == (0xA<<4))
	{
		return 1;
	}
	else
		return 0;
}
static void ddr_reset(void)
{
	BU_REG_WRITE(0xC0100020,0x13000001);

	mdelay(10);
	if(ddr_is_lpddr2() || ddr_is_lpddr3()) {
		BU_REG_WRITE(0xC0100024,0x1302003F);
		mdelay(10);

		BU_REG_WRITE(0xC0100024,0x13020001);
		BU_REG_WRITE(0xC0100024,0x13020002);
		BU_REG_WRITE(0xC0100024,0x13020003);	
	}
	mdelay(10);
}

static uint32_t ddr_addr_st = 0x0;
int ddr_size_mb()
{
	uint32_t reg, ddr_size_in_MB = 0,cs_size;
	
	reg = BU_REG_READ(0xC0100200);
	if(BIT_0&reg)
	{
		cs_size = (1<<(((reg&0X1F0000)>>16)-4));
		ddr_size_in_MB += cs_size;
		ddr_addr_st = reg & 0xff800000;
	}
	
	reg = BU_REG_READ(0xC0100204);
	if(BIT_0&reg)
	{
		cs_size = (1<<(((reg&0X1F0000)>>16)-4));
		ddr_size_in_MB += cs_size;
	}
	
	return ddr_size_in_MB;
}

void detect_default_ods(void)
{
	uint32_t value;
	int i;

	value = BU_REG_READ(0xC0100310);
	value &= 0xf0000;
	value = value >> 16;

	default_ddrv = value;
	cur_ods = value;
	bbu_printf("dft ddrv: %d\n\r",value);
}

void print_line_line(void)
{
    	bbu_printf("-------------------------------------------------------------------\n\r");
}

int drive_strength_pad_calibration(int zdrv_val)
{
	uint32_t uiPPadCal =0x7, uiNPadCal=0x7;
	uint32_t phy_ctrl_9;

	if(use_mannual_drv) {
		return 0;
	}
		
	ddr_set_drv_val(z_d_value, zpr_znr, zpr_znr, 0, 1);
	
	*(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x410) |= (1<<16);	
	udelay(10);
	print_line_line();
	bbu_printf("dft zpnr: 0x%x/0x%x\n\r",zpr_znr,zpr_znr);
	
	phy_ctrl_9 = *(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x420);
	
	if(phy_ctrl_9 & (1<<2))
	{
		if((phy_ctrl_9 & (1<<3)))
		{
			bbu_printf("MAXED OUT [PHY_CTRL14=0x%08X]\n\r"
					,phy_ctrl_9);
			
			zpr_znr = 0x1F;
            		ddr_set_drv_val(zdrv_val, zpr_znr,zpr_znr, 0, 1);
	        	goto out_success;
		}

		if(((phy_ctrl_9 & (0x1F<<20))) && ((phy_ctrl_9 & (0x1F<<15)))){
			uiPPadCal = (((phy_ctrl_9 & (0x1F<<20))>>20));
			uiNPadCal = (((phy_ctrl_9 & (0x1F<<15))>>15));
			bbu_printf("PASS-PHY_CAL_ZPR=0x%01X, PHY_CAL_ZNR=0x%01X\n\r",uiPPadCal,uiNPadCal);
		} else {
			bbu_printf("FAIL-ZN/PR value of 0x0 [PHY_CTRL14=0x%08X]\n\r",
					phy_ctrl_9);
			goto out_fail;
		}
	}
	
	else{
		bbu_printf("FAIL-PHY CAL DONE NOT DETECTED [PHY_CTRL14=0x%08X]\n\r"
				,phy_ctrl_9);
		goto out_fail;
	}

	*(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x410) &= ~(1<<16);	

	 if(zdrv_val == 1) {
		bbu_printf("zdrv = 1,Offset=5\n\r");
		uiOffset = 5;
	 }	
	
	//4) copy znr to zpr and copy the values to all pads only if the value is >=2
	if (uiPPadCal >= 2)
	{
		uiNPadCal = uiPPadCal;
		if (uiPPadCal+uiOffset <= 31)
        	{
			bbu_printf("uiPPadCal: 0x%08X, uiOffset: 0x%08X\n\r",uiPPadCal,uiOffset);
			bbu_printf("zpnr = 0x%08X\n\r",uiPPadCal +  uiOffset);
			zpr_znr = uiNPadCal +  uiOffset;
            		ddr_set_drv_val(zdrv_val,zpr_znr,zpr_znr, 0, 1);
        	} else {              
			bbu_printf("(zpnr = 0x%08X\n\r",0xf);
			zpr_znr = 0x1f;
            		ddr_set_drv_val(zdrv_val,zpr_znr,zpr_znr, 0, 1);
        	}
        
   } else {
			bbu_printf("uiPPadCal<2, deflt zpnr\n\r");
			goto out_fail;
   }

out_success:
   //3) disable pad calibration pad_cal_auto to save power
   *(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x410) &= ~(1<<16);	
   return 0;
   
out_fail:
   //3) disable pad calibration pad_cal_auto to save power
   *(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x410) &= ~(1<<16);	  
   return 1;
}

extern uint32_t mtsp_total_error;
extern int exit_when_error;
int phasel_shmoo_use_mtsp = 1;

int phs_swp_test(uint32_t argc, int argv[],uint32_t phsel_test_bytes,uint32_t phsel_test_bytes1, struct mck4_phasel_info * info)
{
	int value,byte, i, j, x, y,yy, byte_map,phs_pass_st = -1, phs_pass_end = -1;
	uint32_t ret_val, uiDlyStart=0x00, uiDlyEnd=0x1F, uiDlyInc=1;
	uint32_t uiDLLPhSel, org_phasel0, org_phasel1;
    	int phasel0_done = 0, phasel1_done = 0;

	(void)argc;
	bbu_printf("zdrv: 0x%08X\n\r",z_d_value);
	bbu_printf("zpnr: 0x%08X\n\r",zpr_znr);

	ret_val = BU_REG_READ(0xc0100310);
	bbu_printf("0xC0100310: 0x%08X\n\r", ret_val);

	dll_swp_prepare();
     
	uiDlyStart = argv[0];
	uiDlyEnd = argv[1];
	bbu_printf("buf: [0x%08X - 0x%08X]\n\r",uiDlyStart,uiDlyEnd);
	
	uiDlyInc = argv[2];
	uiDlyStart &= 0x3F;
	uiDlyEnd &= 0x3F;


    	org_phasel0 = (BU_REG_READ(0xc0100500)&0x3F0000) >> 16;
    	org_phasel1 = (BU_REG_READ(0xc0100500)&0x3F000000) >> 24;
    		
    	bbu_printf("original phsl0: 0x%X, org_phs1: 0x%X\n\r",org_phasel0,org_phasel1);		

	ddr_set_drv_val(z_d_value,zpr_znr, zpr_znr, 0, 1);
 	byte_map = 0xF;

redo_1:  	
    	if((phasel0_done == 1) && (phasel1_done == 1))
        	goto out_ret;

        if(phasel0_done == 0)
		phasel_index = 0;
	else if(phasel1_done == 0)
		phasel_index = 1;

#ifdef DLL_PHASEL_PER_BYTE
	is_perbyte_swp = 1;
 	for(byte = 0; byte < 4; byte++) {
 		byte_map = (BIT_0 << byte);
		cur_byte_lane = byte;	
#endif

	for(uiDLLPhSel=uiDlyStart; uiDLLPhSel<=uiDlyEnd; uiDLLPhSel+=uiDlyInc)
	{
		if(!phasel0_done && !phasel1_done)
		{
			bbu_printf("shmoo phs0\n\r");
			if(phs0_phs1_equal) {
				phasel0_done = 1;
				org_phasel1 = uiDLLPhSel;
			}
			
			set_phs_val(uiDLLPhSel, org_phasel1,byte_map);
		}
			
		if(phasel0_done && (!phasel1_done))
		{
			if(value == -1)
				value = org_phasel0;
			    
			if(phs0_phs1_equal)
				value = uiDLLPhSel;
			bbu_printf("shmoo phs1,phs0: 0x%x\n\r", value);
			set_phs_val(value, uiDLLPhSel,byte_map);
		}
			
		if(phasel_shmoo_use_mtsp)
		{
			mtsp_total_error = 0; 
			ret_val = bbu_mtsp_test((ddr_addr_st + 0x3000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);

		} 
		y = uiDLLPhSel/8;
		yy =uiDLLPhSel%8;
		    
		if(ret_val) {
			bbu_printf("phsl sweep FAILED\n\r");
			if((phs_pass_st != -1) && (phs_pass_end == -1)) {
				phs_pass_end = uiDLLPhSel -1; 
			   	bbu_printf("*** phs_pass_end:%d\n\r",  phs_pass_end);
			} 
		} else {
			bbu_printf("phsl sweep PASS\n\r");
			if(phs_pass_st == -1) {
				phs_pass_st = uiDLLPhSel;
				bbu_printf("***phs_pass_st: %d\n\r",phs_pass_st);
                    	}
		}					
	} ///end of for(uiDLLPhSel...
	
	print_line_line();
	bbu_printf("phs_pass_st=%d, phs_pass_end=%d\n\r",phs_pass_st, phs_pass_end);

   	if(phs_pass_end == -1 && phs_pass_st != -1)
   		phs_pass_end = 63;

	if((phs_pass_st < phs_pass_end)
	    && (phs_pass_st!= -1) && (phs_pass_end!= -1))
    	{
            	value = (phs_pass_st + ((phs_pass_end - phs_pass_st)/4));
		if(phasel0_done == 0) {
			swp_result_all[cur_pp_idx].dswp_result[cur_ods].phasel_pass_st[byte] = phs_pass_st;
			swp_result_all[cur_pp_idx].dswp_result[cur_ods].phasel_pass_end[byte]  = phs_pass_end;
		} else {
			swp_result_all[cur_pp_idx].dswp_result[cur_ods].phasel1_pass_st[byte] = phs_pass_st;
			swp_result_all[cur_pp_idx].dswp_result[cur_ods].phasel1_pass_end[byte]  = phs_pass_end;
		}

    	} else { 
    	    	bbu_printf("No dqs pass window\n\r");
    	    	value = -1;
    	}

	phs_pass_st = phs_pass_end = -1;

	swp_result_all[cur_pp_idx].dswp_result[cur_ods].avail_ddrv = cur_ods;
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].is_bypass_mode  = 0;

	#ifndef DLL_PHASEL_PER_BYTE
	if(value != -1)
	{
		bbu_printf("\n\rset phs/phs1 = %d/%d\n\r",value,value);
    		set_phs_val(value, value, 0xF);
    	}
   	else
   	{
   		bbu_printf("\n\rpass win too small\n\r");
    		set_phs_val(13, 18, 0xF);
   	}
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].is_perbyte  = 0;
	#else
	if(value != -1)
	{
		bbu_printf("\n\rset phsl/phsl1 = %d/%d\n\r",value,value);
		if(phasel0_done == 0)
    			set_phs_val(value, org_phasel1, byte_map);
		else if(phasel1_done == 0)
    			set_phs_val(0xffffffff, value, byte_map);
    	}
   	else
   	{
   		bbu_printf("\n\rpass window too small\n\r");
    		set_phs_val(13, 18, byte_map);
   	}
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].is_perbyte  = 1;
	#endif   	   	


	#ifdef DLL_PHASEL_PER_BYTE
	}
	#endif


    	if(phasel0_done == 1)
        	phasel1_done = 1;
        
    	if(phasel0_done == 0)
        	phasel0_done = 1;
        	
        if(phs0_phs1_equal)
        	goto out_ret;
    
    	goto redo_1;
	is_perbyte_swp = 0; 
out_ret:
	return value;
}

int dll_dly_swp_test(uint32_t phsel_test_bytes,uint32_t phsel_test_bytes1)
{
	int ret_val,argv[3];
 	int i, j, x, y,yy,dly_pass_st = -1, dly_pass_end = -1;
	uint32_t value, uiDlyStart=0x00, uiDlyEnd=0x1F, uiDlyInc=1;
	uint32_t uiDLLPhSel;
		
 	argv[0] = 0;
 	argv[1] = 0x1ff;
 	argv[2] = 4;
	uiDlyStart = argv[0];
	uiDlyEnd = argv[1];
	bbu_printf("Buf: [0x%08X - 0x%08X]\n\r",uiDlyStart,uiDlyEnd);
	uiDlyInc = argv[2];

	uiDlyStart &= 0x1FF;
	uiDlyEnd &= 0x1FF;
		
	is_dll_bypass = 1; 
	ddr_set_drv_val(z_d_value, zpr_znr, zpr_znr, 0, 1);
	
	for(uiDLLPhSel=uiDlyStart; uiDLLPhSel<=uiDlyEnd; uiDLLPhSel+=uiDlyInc)
	{ 
		ddr_set_dly(uiDLLPhSel);
		mtsp_total_error = 0; //clear error num before invoke mtsp test
		ret_val = bbu_mtsp_test((ddr_addr_st + 0x3000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
		
		y = uiDLLPhSel/8;
		yy =uiDLLPhSel%8;
		
		if(ret_val) {
			if(dly_pass_st != -1 && dly_pass_end == -1) {
			       	dly_pass_end = uiDLLPhSel -1; 
			        bbu_printf("*** dly_pass_end:%d\n\r",dly_pass_end);
			}  
		} else {
			if(dly_pass_st == -1) {
				dly_pass_st = uiDLLPhSel;
				bbu_printf("***dly_pass_st: %d\n\r",dly_pass_st);
                    	}
		}		
	} ///end of for(uiDLLPhSel...
	print_line_line();

   	if(dly_pass_end == -1 && dly_pass_st != -1)
   		dly_pass_end = 512;

	if((dly_pass_st < dly_pass_end)
	    && (dly_pass_st!= -1) && (dly_pass_end!= -1))
    	{
            	value = (dly_pass_st + ((dly_pass_end - dly_pass_st)/4));
    	} else { 
    	    	bbu_printf("No window\n\r");
    	    	value = -1;
    	}
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].avail_ddrv = cur_ods;
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].is_bypass_mode  = 1;
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].is_perbyte  = 0;
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].dlldly_pass_st = dly_pass_st;
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].dlldly_pass_end  = dly_pass_end;

	print_line_line();
	if(value != -1)
	{
		bbu_printf("\n\r---->set dll dly: %d\n\r",value);
    		ddr_set_dly(value);
	}
	else
	{
		bbu_printf("\n\r---->set dll dly: %d\n\r",0x40);
    		ddr_set_dly(0x40);
	}
	print_line_line();

	is_dll_bypass = 1; 
	return value;
}


int argv_dll[3] = {0, 31, 1};
char dqs_dly_val[NUM_ODS];
int dqs_swp_test(uint32_t phsel_test_bytes,uint32_t phsel_test_bytes1, int dqs_map, int is_simu)
{
	int ret_val;
	int i, dq_offset, dqs_offset;
	int dqs0, dqs1,dqs2,dqs3;
	uint32_t dq0[2],dq1[2],dq2[2],dq3[2], value;
	uint32_t dqc0[2],dqc1[2],dqc2[2],dqc3[2];
	int old_dq_dly,old_dqs_dly;
	int old_dqc_dly, old_dqsc_dly;
	int dqs_pass_start = -1;
    	int dqs_pass_end = -1;
	int dq_dly, dqs_dly, dqc_dly, dqsc_dly;
    
	value = BU_REG_READ(0xc0100580);
	bbu_printf("0xc0100580:0x%x\n\r", value);
    	old_dq_dly = dq0[0] = (value & (0x7F));
    	dq1[0] = dq0[0];
    	dq2[0] = dq0[0];
    	dq3[0] = dq0[0];
    
    	old_dqs_dly = dq0[1] = ((value & 0x7F00) >>8);
    	dq1[1] = dq0[1];
    	dq2[1] = dq0[1];
    	dq3[1] = dq0[1];        

    	old_dqc_dly = dqc0[0] = ((value & (0x7F0000)) >>16);
    	dqc1[0] = dqc0[0];
    	dqc2[0] = dqc0[0]; 
    	dqc3[0] = dqc0[0];
    
    	old_dqsc_dly = dqc0[1] = ((value &0x7F000000) >>24);
    	dqc1[1] = dqc0[1];
    	dqc2[1] = dqc0[1];
    	dqc3[1] = dqc0[1];
	
	dq_offset = old_dq_dly - old_dqc_dly;
	dqs_offset = old_dqs_dly - old_dqsc_dly;
	bbu_printf("dq_offset: 0x%08x\n\r", dq_offset);
	bbu_printf("dqs_offset: 0x%08x\n\r", dqs_offset);

 	ddr_set_drv_val(z_d_value,zpr_znr, zpr_znr, 0, 1);

	for(dqs3 = 0; dqs3 < 64; dqs3++)
	{
		dqs0 = dqs1 = dqs2 = dqs3;		

		if(is_simu&&(dqs0 == dqs1)&&(dqs1==dqs2)&&(dqs2==dqs3))
			;
		else if(!is_simu)
			;
		else			
			continue;
		
		if(dqs_map&BIT_0)
		{
    			dq0[0] = dqs0;
   	 		dq1[0] = dqs1;
    			dq2[0] = dqs2;
    			dq3[0] = dqs3;   
 
    			dqc0[0] = dqs0;
   	 		dqc1[0] = dqs1;
    			dqc2[0] = dqs2;
    			dqc3[0] = dqs3;  
			if(dq_offset > 0)
			{
				dq0[0] = dq0[0] + dq_offset;
   	 			dq1[0] = dq1[0] + dq_offset;
    				dq2[0] = dq2[0] + dq_offset;
    				dq3[0] = dq3[0] + dq_offset;  
			}  else {
				dq_offset = 0 - dq_offset;
	    			dqc0[0] = dqc0[0] + dq_offset;
	   	 		dqc1[0] = dqc1[0] + dq_offset;
	    			dqc2[0] = dqc2[0] + dq_offset;
	    			dqc3[0] = dqc3[0] + dq_offset;
			}
		}
		
		if(dqs_map&BIT_1)
		{
    			dq0[1] = dqs0;
   	 		dq1[1] = dqs1;
    			dq2[1] = dqs2;
    			dq3[1] = dqs3;    

    			dqc0[1] = dqs0;
   	 		dqc1[1] = dqs1;
    			dqc2[1] = dqs2;
    			dqc3[1] = dqs3;   

			if(dqs_offset > 0)
			{
				dq0[1] = dq0[1] + dqs_offset;
   	 			dq1[1] = dq0[1] + dqs_offset;
    				dq2[1] = dq0[1] + dqs_offset;
    				dq3[1] = dq0[1] + dqs_offset;  
			}  else {
				dqs_offset = 0 - dqs_offset;
	    			dqc0[1] = dqc0[1] + dqs_offset;
	   	 		dqc1[1] = dqc1[1] + dqs_offset;
	    			dqc2[1] = dqc2[1] + dqs_offset;
	    			dqc3[1] = dqc3[1] + dqs_offset;
			}
		}

		set_dqsc_val_2(dqc0,dqc1,dqc2,dqc3);
		set_dqs_val_2(dq0,dq1,dq2,dq3);

		mtsp_total_error = 0; //clear error num before invoke mtsp test
		ret_val = bbu_mtsp_test((ddr_addr_st + 0x3000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
		if(ret_val)
		{
			 if(dqs_map&BIT_1)
			 	bbu_printf("[qs]=[%d],FAIL\n\r",dqs0,dqs1,dqs2,dqs3);
			 if(dqs_map&BIT_0)
			 	bbu_printf("[dq]=[%d],FAIL\n\r",dqs0,dqs1,dqs2,dqs3);	
			 if(dqs_pass_start != -1 && dqs_pass_end == -1)
			 {
			        dqs_pass_end = dqs0 -1; 
			        bbu_printf("***s_pass_end:%d\n\r", dqs_pass_end);
			 }   
		} 
		else {
				mtsp_total_error = 0; //clear error num before invoke mtsp test
				ret_val = bbu_mtsp_test((ddr_addr_st + 0x3000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
				
				if(ret_val) 
				{
					if(dqs_map&BIT_1)
			 		bbu_printf("[qs]=[%d],FAIL\n\r",dqs0,dqs1,dqs2,dqs3);
			 		if(dqs_map&BIT_0)
			 		bbu_printf("[dq]=[%d],FAIL\n\r",dqs0,dqs1,dqs2,dqs3);
			       		if((dqs_pass_start != -1) && (dqs_pass_end == -1))
			        	{
			            		dqs_pass_end = dqs0 -1; 
			            		bbu_printf("***s_pass_end:%d\n\r", dqs_pass_end);
			        	} 
				} else {
					if(dqs_map&BIT_1)
					bbu_printf("\t\t\t\t\t\t\t\t\t\t[qs]=[%d],Pass\n\r",dqs0,dqs1,dqs2,dqs3);
					if(dqs_map&BIT_0)
					bbu_printf("\t\t\t\t\t\t\t\t\t\t[dq]=[%d],Pass\n\r",dqs0,dqs1,dqs2,dqs3);
					
					if(dqs_pass_start == -1)
					{
					    dqs_pass_start = dqs0;
					    bbu_printf("***s pass start: %d\n\r",dqs_pass_start);
                    			}
				}
			    
		}
	} 
	
    	dump_mck_regs();
	cur_ods_index = cur_ods;

	if((dqs_pass_start!=-1) && (dqs_pass_end ==-1))
		dqs_pass_end = 63;

    	if((dqs_pass_start <= dqs_pass_end) 
	    && (dqs_pass_start!=-1) && (dqs_pass_end!=-1))
    	{
            	dqs_dly_val[cur_ods_index] = (dqs_pass_start + ((dqs_pass_end-dqs_pass_start)/3));
		if(dqs_dly_val[cur_ods_index] < (old_dqs_dly + 2)
		   && dqs_dly_val[cur_ods_index] > (old_dqs_dly - 2))
			dqs_dly_val[cur_ods_index] = old_dqs_dly;
		bbu_printf("dq/dqs pass win:[%d, %d]\n\r", dqs_pass_start, dqs_pass_end);
    	}
    	else
    	{
    	    	bbu_printf("No dqs pass window\n\r");
    	    	dqs_dly_val[cur_ods_index] = old_dqs_dly;
    	}

	//bbu_printf("old_dqs_dly: %d\n\r", old_dqs_dly);
    	//bbu_printf("set dqs to 0x%X\n\r", dqs_dly_val[cur_ods_index]);
	bbu_printf("cur_pp_idx: %d, cur_ods: %d\n\r", cur_pp_idx, cur_ods);
  	swp_result_all[cur_pp_idx].dswp_result[cur_ods].dqs_pass_st =   dqs_pass_start;
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].dqs_pass_end  = dqs_pass_end;
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].dqs_val  =  dqs_dly_val[cur_ods_index];	
	
	restore_wck_dqs_dq_val();    
        return dqs_dly_val[cur_ods_index];	
}

int cak_swp_test(uint32_t phsel_test_bytes,uint32_t phsel_test_bytes1, int ca_map, int swp_pattern)
{
	int ret_val;
	int i;
	uint32_t old_ck,old_ca, ca_val, ck_dly;
	int ca_pass_start = -1;
    	int ca_pass_end = -1;
	uint32_t value;

    	value = BU_REG_READ(0xc0100708);
	bbu_printf("0xc0100708:0x%x",value);
    	value = BU_REG_READ(0xc010070c);
	bbu_printf("0xc010070c:0x%x",value);

    	old_ck = ((value >> 16) & 0xF);
	old_ca = (value & 0xF);       

 	ddr_set_drv_val(z_d_value,zpr_znr, zpr_znr, 0, 1);

	for(i = 0; i < 16; i++)
	{
			
		if(ca_map&BIT_0)
		{
			ca_val = i;
		} else 
			ca_val = old_ca;
		
		if(ca_map&BIT_1)
		{
    			ck_dly = i;   
		} else 
			ck_dly = old_ck;
		
		set_ca_nonca_val(ca_val, ck_dly);
		
		mtsp_total_error = 0; //clear error num before invoke mtsp test
		ret_val = bbu_mtsp_test((ddr_addr_st + 0x3000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
		if(ret_val)
		{
			 if(ca_map&BIT_1)
			 	bbu_printf("[ck]=[%d],FAIL\n\r",ck_dly,ck_dly,ck_dly,ck_dly);
			 if(ca_map&BIT_0)
			 	bbu_printf("[ca]=[%d],FAIL\n\r",ca_val,ca_val,ca_val,ca_val);
	
			 if(ca_pass_start != -1 && ca_pass_end == -1)
			 {
			        ca_pass_end = i -1; 
			        bbu_printf("***s_pass_end:%d\n\r", ca_pass_end);
			 }   
		} 

		else {
			mtsp_total_error = 0; //clear error num before invoke mtsp test
			ret_val = bbu_mtsp_test((ddr_addr_st + 0x3000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
				
			if(ret_val) 
			{
				if(ca_map&BIT_1)
			 		bbu_printf("[ck]=[%d],FAIL\n\r",ck_dly);
			 	if(ca_map&BIT_0)
			 		bbu_printf("[ca]=[%d],FAIL\n\r",ca_val);
			       	if((ca_pass_start != -1) && (ca_pass_end == -1))
			        {
			            	ca_pass_end = i -1; 
			            	bbu_printf("***s_pass_end:%d\n\r", ca_pass_end);
			        } 
			} else {
				if(ca_map&BIT_1)
					bbu_printf("\t\t\t\t\t\t\t\t\t\t[ck]=[%d],Pass\n\r",ck_dly);
				if(ca_map&BIT_0)
					bbu_printf("\t\t\t\t\t\t\t\t\t\t[ca]=[%d],Pass\n\r",ca_val);
					
				if(ca_pass_start == -1)
				{
					ca_pass_start = i;
					bbu_printf("***s pass start: %d\n\r",ca_pass_start);
                    		}
			}
			    
		}
	} 
	
    	dump_mck_regs();
	if(ca_pass_start == -1)
	{
		ca_pass_start = 0;
		ca_pass_end = 0;
	}

	if(ca_pass_end == -1)
		ca_pass_end = 15;

	bbu_printf("ca dly pass win: [%d, %d]\n\r",ca_pass_start, ca_pass_end);

	cur_ods_index = cur_ods;
	//set_ca_nonca_val(old_ca,old_ck);
  	swp_result_all[cur_pp_idx].dswp_result[cur_ods].ca_pass_st =   ca_pass_start;
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].ca_pass_end  = ca_pass_end;
	      
        return 0;
}
//#endif
void check_shadow_zpnr(void)
{
	if(shadow_zpnr & BIT_30)
		zpr_znr = (shadow_zpnr & (~BIT_30));
}

void swp_zpnr_pass_window(int phsel_test_bytes, int phsel_test_bytes1)
{
	int ret_val;
	int i,j;
	int zpr_pass_start = -1;
    	int zpr_pass_end = -1;

	for(i = 0; i < 32; i++)
	{
		ddr_set_drv_val(z_d_value, i, i, 1, 1);
		
		mtsp_total_error = 0; //clear error num before invoke mtsp test
		ret_val = bbu_mtsp_test((ddr_addr_st + 0x3000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
		if(ret_val)
		{
			 bbu_printf("zpr %d failed\n\r", i);
			 if(zpr_pass_start != -1 && zpr_pass_end == -1)
			 {
			        zpr_pass_end = i -1; 
			        bbu_printf("***s_pass_end:%d\n\r", zpr_pass_end);
			 }   
		} else {
			 bbu_printf("\t\t\t\t\t\t\t\t\t\tzpr %d pass\n\r", i);		
			 if(zpr_pass_start == -1)
			 {
				zpr_pass_start = i;
				bbu_printf("***s pass start: %d\n\r",zpr_pass_start);
                    	 }
		}
	} 
    
   	if(zpr_pass_end == -1 && zpr_pass_start != -1)
   		zpr_pass_end = 31;

   	if(zpr_pass_end == -1 && zpr_pass_start == -1)
   		zpr_pass_start = zpr_pass_end = 0;
  		
   	bbu_printf("zpr pass win: [%d: %d] at ddrv %d\n\r",zpr_pass_start, zpr_pass_end, cur_ods);
   	
  	swp_result_all[cur_pp_idx].dswp_result[cur_ods].zpr_pass_start =   zpr_pass_start;
	swp_result_all[cur_pp_idx].dswp_result[cur_ods].zpr_pass_end   = zpr_pass_end;	

	if(zpr_pass_start == 0 && zpr_pass_end == 0) {
		shadow_zpnr &= ~BIT_30;
		return;
	}

	if((zpr_znr <= zpr_pass_start + (ZPNR_OFFSET- 1)) || (zpr_znr >= zpr_pass_end-(ZPNR_OFFSET - 1))) {
		shadow_zpnr = (zpr_pass_start + zpr_pass_end)/2;
		shadow_zpnr |= BIT_30;
		zpr_need_update = 1;
	} else {
		shadow_zpnr &= ~BIT_30;
		zpr_need_update = 0;
	}
}

extern uint8_t *argv104[];
extern uint8_t *argv400[];
extern uint8_t *argv533[];
extern uint8_t *argv667[];
extern uint8_t *argv800[];
//#endif

#ifdef DDR_SWP_AUTO
void print_phs_pass_win(int cur_pp, int ddrv, int print_setting, int win_p)
{
	int i, j, val1 = -1, val2 = 1000;
	int pass_st, pass_end, pass_st1, pass_end1, dlldly_pass_st, dlldly_pass_end;
	int stress_phs0_st = 0, stress_phs0_end = 40;
	int stress_phs1_st = 8, stress_phs1_end = 40, stress_phs0_st2;
	uint32_t phasel0, phasel1, val, dlldly;

	#ifdef DLL_PHASEL_PER_BYTE
	for(i = 0; i < 4; i++)
	{
		bbu_printf("     **lane%d phasel pass win: [%d, %d]\n\r"
			, i 
			, swp_result_all[cur_pp].dswp_result[ddrv].phasel1_pass_st[i]
			, swp_result_all[cur_pp].dswp_result[ddrv].phasel1_pass_end[i]);
		if(val1 < swp_result_all[cur_pp].dswp_result[ddrv].phasel1_pass_st[i])
			val1 = swp_result_all[cur_pp].dswp_result[ddrv].phasel1_pass_st[i];
		if(val2 > swp_result_all[cur_pp].dswp_result[ddrv].phasel1_pass_end[i])
			val2 = swp_result_all[cur_pp].dswp_result[ddrv].phasel1_pass_end[i];
	}
	if(val1 >= val2)
	{
		pass_st1 = 8;
		pass_end1 = 48;		
	}
	else 
	{
		pass_st1 = val1;
		pass_end1 = val2;
		
		//add offset for stress window
		stress_phs1_st = pass_st1 + (pass_end1 - pass_st1 + 4) / 9;
		stress_phs1_end = pass_end1;	
	}
	bbu_printf("mtspwin: (%d,%d)\n\r",pass_st1, pass_end1);
	bbu_printf("*****common phasel1 pass window: [%d,%d]\n\r\n\r\n\r",stress_phs1_st, stress_phs1_end);
	
	val1 = -1;
	val2 = 1000;
	for(i = 0; i < 4; i++)
	{
		bbu_printf("     **lane%d phasel pass win: [%d, %d]\n\r"
			, i
			, swp_result_all[cur_pp].dswp_result[ddrv].phasel_pass_st[i]
			, swp_result_all[cur_pp].dswp_result[ddrv].phasel_pass_end[i]);
	
		if(val1 < swp_result_all[cur_pp].dswp_result[ddrv].phasel_pass_st[i])
			val1 = swp_result_all[cur_pp].dswp_result[ddrv].phasel_pass_st[i];
		if(val2 > swp_result_all[cur_pp].dswp_result[ddrv].phasel_pass_end[i])
			val2 = swp_result_all[cur_pp].dswp_result[ddrv].phasel_pass_end[i];
	}
	if(val1 >= val2)
	{
		pass_st = 0;
		pass_end = 40;
	} 
	else
	{
		pass_st = val1;
		pass_end = val2;

		//add offset for stress window
		stress_phs0_st = pass_st + (pass_end - pass_st + 4) / 9;
		stress_phs0_end = pass_end;

		if(pass_st == 0) {
			stress_phs0_st2 = stress_phs1_st - stress_phs1_end + stress_phs0_end;
			if(stress_phs0_st2 < 0)
				stress_phs0_st2 = 0;
			if(stress_phs0_st > stress_phs0_st2)
				stress_phs0_st = stress_phs0_st2;
		}
	}
	bbu_printf("mtspwin: (%d,%d)\n\r",pass_st, pass_end);
	bbu_printf("*****common phasel0 pass window: [%d,%d]\n\r",
					stress_phs0_st, stress_phs0_end);
	pass_st = stress_phs0_st;
	pass_end = stress_phs0_end;
	pass_st1 = stress_phs1_st;
	pass_end1 = stress_phs1_end;
	#else 
	if(swp_result_all[cur_pp].dswp_result[ddrv].common_phasel_pass_st
		>= swp_result_all[cur_pp].dswp_result[ddrv].common_phasel_pass_end)
	{
		pass_st1 = 8;
		pass_end = 40;
		pass_end1 = 48;
		pas_st = 0;
	} 
	else
	{
		pass_st1 = swp_result_all[cur_pp].dswp_result[ddrv].common_phasel_pass_st;
		pass_end = swp_result_all[cur_pp].dswp_result[ddrv].common_phasel_pass_end;
		pass_end1 = pass_end + 8;
		pas_st = pass_st1 - 8;
	}
	bbu_printf("*****common phasel0 pass window: [%d,%d]\n\r\n\r", pass_st, pass_end);
	bbu_printf("*****common phasel1 pass window: [%d,%d]\n\r\n\r", pass_st1, pass_end1);
	#endif
	if(!print_setting)
		return;
	
	if(swp_result_all[PP_312_IDX].dswp_result[ddrv].dlldly_pass_end <=
		swp_result_all[PP_312_IDX].dswp_result[ddrv].dlldly_pass_st)
	{
		dlldly_pass_st = 16;
		dlldly_pass_st = 208;
	} 
	else 
	{
		dlldly_pass_st = swp_result_all[PP_312_IDX].dswp_result[ddrv].dlldly_pass_st;
		dlldly_pass_st = swp_result_all[PP_312_IDX].dswp_result[ddrv].dlldly_pass_end;
	}

	for(i = win_p; i >= win_p; i--)
	{
		phasel0 = (pass_end - pass_st)/i + pass_st;
		phasel1 = (pass_end1 - pass_st1)/i + pass_st1;
	
		dlldly = (dlldly_pass_end - dlldly_pass_st)/i + dlldly_pass_st;

		val = BU_REG_READ(0xC0100500);
		val &= ~(0x3|(0x3f<<16)|(0x3f<<24));
		val |=((phasel0 << 16) | (phasel1 << 24)) ;

		val &= ~(0x3ff<<4);
		val |= ((dlldly << 4));

		bbu_printf("%s%d_500 = <0xC0100500,0x%08X>\n\r",update_str_pattern,4-i,val);
		bbu_printf("%s%d_504 = <0xC0100504,0x%08X>\n\r",update_str_pattern,4-i,val);
		bbu_printf("%s%d_508 = <0xC0100508,0x%08X>\n\r",update_str_pattern,4-i,val);
		bbu_printf("%s%d_50C = <0xC010050C,0x%08X>\n\r",update_str_pattern,4-i,val);		
	}

	print_line_line();	
}

void print_dll_dly_window(void)
{
	int i;

	bbu_printf("**dclk: 312 Mhz\n\r");
	for(i = ods_swp_st; i <= ods_swp_end; i++)
	{
		bbu_printf("ods: %d\n\r", i);
		bbu_printf("*****dly pass window: [%d,%d]\n\r"
			, swp_result_all[PP_312_IDX].dswp_result[i].dlldly_pass_st
			, swp_result_all[PP_312_IDX].dswp_result[i].dlldly_pass_end);
	}
}

void mmt_print_dly_setting(uint32_t ddrv, int win_p)
{
	uint32_t dlldly, val, pass_st, pass_end;
	int i;
	
	if(swp_result_all[PP_312_IDX].dswp_result[ddrv].dlldly_pass_end <=
		swp_result_all[PP_312_IDX].dswp_result[ddrv].dlldly_pass_st)
	{
		pass_st = 16;
		pass_end = 208;
	} 
	else 
	{
		pass_st = swp_result_all[PP_312_IDX].dswp_result[ddrv].dlldly_pass_st;
		pass_end = swp_result_all[PP_312_IDX].dswp_result[ddrv].dlldly_pass_end;
	}
	bbu_printf("*****dlldly pass window: [%d,%d]\n\r"
		, pass_st, pass_end);
}

void mmt_update_blf(int is_bypass_mode, int ddrv, int pp)
{
	int i, j, pass_st, pass_end;
	uint32_t val,val2;
	int dqs_dly;

    	bbu_printf("zpr_znr: %d#\n\r",zpr_znr);
    	
 	for(i = 4; i > 1; i--)
 	{
		print_line_line();
 		bbu_printf("**choose 1/%d pass window ***\n\r",i);

 		#ifdef SWP_ODS
 		bbu_printf("ods:%d#\n\r",ddrv);
 		#endif
 		
 		if(swp_result_all[pp].dswp_result[ddrv].dqs_pass_st >=
 			swp_result_all[pp].dswp_result[ddrv].dqs_pass_end)
 		{
 			pass_st = 0;
 			pass_end = 40; 
 		}
 		else 
 		{
 		 	pass_st = swp_result_all[pp].dswp_result[ddrv].dqs_pass_st;
 			pass_end = swp_result_all[pp].dswp_result[ddrv].dqs_pass_end; 
 		}

		if(pass_end >= 11)
			dqs_dly = (pass_end - pass_st)/3 -10 + pass_st;
		else
			dqs_dly = 0;

		if(dqs_dly < 0)
			dqs_dly = 0;

		#ifdef SWP_DQS
		if(dqs_update)
		{
			bbu_printf("dq/s:%d#\n\r",dqs_dly);
			
		}		
		#endif
		
 		if(BIT_0&BU_REG_READ(0xC0100200))
		{
			#ifdef SWP_ODS
			val = BU_REG_READ(0xC0100310);
			val &= 0xfff0ffff;
			val |=(ddrv << 16);
			bbu_printf("%s%d_310 = <0xC0100310,0x%08X>\n\r",update_str_pattern,4-i,val);
			#endif	
		}
 		if(BIT_0&BU_REG_READ(0xC0100204))
		{
			#ifdef SWP_ODS
			val = BU_REG_READ(0xC0100314);
			val &= 0xfff0ffff;
			val |=(ddrv << 16);
			bbu_printf("%s%d_314 = <0xC0100314,0x%08X>\n\r",update_str_pattern,4-i,val);
			#endif	
		}
	
		if (zpr_need_update) {
			bbu_printf("%s%d_404 = <0xC0100404,0x%08X>\n\r",update_str_pattern,4-i,BU_REG_READ(0xC0100404));
			bbu_printf("%s%d_408 = <0xC0100408,0x%08X>\n\r",update_str_pattern,4-i,BU_REG_READ(0xC0100408));
			bbu_printf("%s%d_40C = <0xC010040C,0x%08X>\n\r",update_str_pattern,4-i,BU_REG_READ(0xC010040C));
		}

 		if(BIT_0&BU_REG_READ(0xC0100200))
		{
			#ifdef SWP_DQS
			if(dqs_update)
			{
				val = BU_REG_READ(0xC0100580);
				val &= 0x00ff00ff;
				val |=(dqs_dly << 8 | dqs_dly << 24);
				bbu_printf("%s%d_580 = <0xC0100580,0x%08X>\n\r",update_str_pattern,4-i,val);
				bbu_printf("%s%d_584 = <0xC0100584,0x%08X>\n\r",update_str_pattern,4-i,val);
				bbu_printf("%s%d_588 = <0xC0100588,0x%08X>\n\r",update_str_pattern,4-i,val);
				bbu_printf("%s%d_58C = <0xC010058C,0x%08X>\n\r",update_str_pattern,4-i,val);
			}
			#endif
		}
 		if(BIT_0&BU_REG_READ(0xC0100204))
		{
			#ifdef SWP_DQS
			if(dqs_update)
			{
				val = BU_REG_READ(0xC01005A0);
				val &= 0x00ff00ff;
				val |=(dqs_dly << 8 | dqs_dly << 24);
				bbu_printf("%s%d_5A0 = <0xC01005A0,0x%08X>\n\r",update_str_pattern,4-i,val);
				bbu_printf("%s%d_5A4 = <0xC01005A4,0x%08X>\n\r",update_str_pattern,4-i,val);
				bbu_printf("%s%d_5A8 = <0xC01005A8,0x%08X>\n\r",update_str_pattern,4-i,val);
				bbu_printf("%s%d_5AC = <0xC01005AC,0x%08X>\n\r",update_str_pattern,4-i,val);
			}
			#endif
		}
		if(is_bypass_mode == 1) 
		{
			mmt_print_dly_setting(ddrv, i);
		}
		print_phs_pass_win(pp, ddrv, 1, i);
		
	}
}

void dll_swp_auto_log(int is_bypass_mode, int platform)
{	
	int val1 = -1, val2 = 1000, i, j;
	uint32_t dqs_dly,val;

	bbu_printf("*************************************\n\r");
	dump_mck_regs();
	
    	bbu_putchar(0x80); 
    	bbu_printf("output log ...\n\r");
    	
    	bbu_printf("BLF_UPDATE_BEGIN\n\r"); 
	mmt_update_blf(is_bypass_mode, default_ddrv, highest_pp);
	bbu_printf("BLF_UPDATE_END\n\r");  
	bbu_putchar(0x93);
	
	bbu_printf("BLF_UPDATE_BEGIN\n\r"); 
	print_line_line();
#ifdef SWP_ODS
	for(j = ods_swp_st; j <= ods_swp_end; j++)
	{
		if(j == default_ddrv)
			continue;
			
		mmt_update_blf(is_bypass_mode, j, highest_pp);
		bbu_putchar(0x93);
		print_line_line();
	}
	bbu_printf("BLF_UPDATE_END\n\r");  	
#endif

	bbu_putchar(0x81);
	bbu_printf("*************************************\n\r\n\r");		
}

void ddr_swp_test(int test_case)
{
	if(test_case == 0)
		dll_swp_auto_log(0, 0);	
	if(test_case == 1)
		dll_swp_auto_log(1, 0);
}
#endif

void shmoo_phy_sync(int inv_ld, int phy_wrst_sel, int wckg_dly)
{
	uint32_t val;
	int ret;

	val = BU_REG_READ(0xc0100414);
	val &= ~BIT_9;
	val |= (inv_ld << 9);
	BU_REG_WRITE(0xc0100414, val);

	val = BU_REG_READ(0xc0100438);
	val &= ~0x3;
	val |= (phy_wrst_sel << 0);
	BU_REG_WRITE(0xc0100438, val);

	val = BU_REG_READ(0xc010043c);
	val &= ~0xf0;
	val |= (wckg_dly << 4);
	BU_REG_WRITE(0xc010043c, val);
	
	BU_REG_WRITE(0xc0100420, 0x80000000);
	mdelay(1);

	ret = bbu_mtsp_test(0x0,0x400000,0xffffffff,0);
	if(ret) {
		bbu_printf("------------------->[invld, sel, dly] = [%d, %d, %d] FAIL\n\r", inv_ld, phy_wrst_sel, wckg_dly);
	} else {
		bbu_printf("[invld, sel, dly] = [%d, %d, %d] PASS\n\r", inv_ld, phy_wrst_sel, wckg_dly);
	}
}


