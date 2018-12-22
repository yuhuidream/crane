#include "predefines.h"
#include <string.h>
#include "bbu.h"
#include "pmic.h"
#include "timer_dec.h"
#include "CIU.h"
#include "common_tests.h"
#include "ppset.h"
#include "mcu_generate_pattern.h"

extern int phasel_shmoo_use_mtsp;
extern int cur_byte_lane;
extern int cur_ods;
extern int is_dll_bypass;
extern int use_mannual_drv;
extern int cur_pp_idx;
extern int phs0_phs1_equal;

uint8_t *argv104[] = {"104"};
uint8_t *argv312[] = {"312"};
uint8_t *argv400[] = {"400"};
uint8_t *argv533[] = {"533"};

//ap pp change
uint8_t *ap_argv832[] = {"opset","core", "6"};

//cp pp change
uint8_t *ddr_argv104[] = {"opset","ddr", "0"};
uint8_t *ddr_argv312[] = {"opset","ddr", "3"};
uint8_t *ddr_argv400[] = {"opset","ddr", "4"};
uint8_t *ddr_argv533[] = {"opset","ddr", "5"};

uint8_t **ppset400to800[] = {
	[PP_400_IDX] = argv400,
	[PP_533_IDX] = argv533,
};
extern uint32_t org_clk_dqs_dq_val;
extern uint32_t org_reg_404;

int request_ddr_fc(int argc, uint8_t **argv)
{
	return bbu_request_assistant(argc, argv);
}

int dswp_pp_change(uint32_t argc, uint8_t *argv[])
{
	request_ddr_fc(3, ap_argv832);

	if((strcmp((const char*)argv[0], "104") == 0))
	{
		bbu_printf("change to 104\n\r");
		request_ddr_fc(3, ddr_argv104);
	}
	if((strcmp((const char*)argv[0], "312") == 0))
	{
		bbu_printf("change to 312\n\r");
		request_ddr_fc(3, ddr_argv312);
		cur_pp_idx = PP_312_IDX;
	}
	if((strcmp((const char*)argv[0], "400") == 0))
	{
		bbu_printf("change to 400\n\r");
		request_ddr_fc(3, ddr_argv400);
		cur_pp_idx = PP_400_IDX;
	}
	if((strcmp((const char*)argv[0], "533") == 0))
	{
		bbu_printf("change to 533\n\r");
		request_ddr_fc(3, ddr_argv533);
		cur_pp_idx = PP_533_IDX;
	}
}

static void pad_cal_result(int result)
{
	if(1 == result)
		bbu_printf("CLB Pass\n\r");
	else
		bbu_printf("CLB Fail\n\r");
}

uint32_t org_dqs_val = 0;
extern uint32_t restore_wck_dqs_dq_val(void);
void swp_dqs_window(void)
{
	uint32_t value;
	int init_dly = 10, dq_offset, dqs_offset;
	int dq_dly, dqc_dly, dqs_dly, dqsc_dly;

	dq_dly = BU_REG_READ(0xc0100580)&(0x7f);
	dqc_dly = (BU_REG_READ(0xc0100580) >> 16)&(0x7f);
	dq_offset = dq_dly - dqc_dly;
	if(dq_offset > 0) {
		dq_dly = init_dly + dq_offset;
		dqc_dly = init_dly;
	} 
	else {
		dq_offset = 0 - dq_offset;
		dqc_dly = init_dly + dq_offset;
		dq_dly = init_dly;
	}

	dqs_dly =  (BU_REG_READ(0xc0100580) >> 8)&(0x7f);
	dqsc_dly = (BU_REG_READ(0xc0100580) >> 24)&(0x7f);
	dqs_offset = dqs_dly - dqsc_dly;
	if(dqs_offset > 0) {
		dqs_offset = 0 - dqs_offset;
		dqs_dly = init_dly + dqs_offset;
		dqsc_dly = init_dly;
	} 
	else {
		dqsc_dly = init_dly + dqs_offset;
		dqs_dly = init_dly;
	}

	set_ca_nonca_val(0x0, 0x0);
	cak_swp_test(0x80000,0x80000,1,0);
	set_ca_nonca_val(0x0, 0x0);	
	cak_swp_test(0x80000,0x80000,2,0);
	set_ca_nonca_val(0x0, 0x8);
	cak_swp_test(0x80000,0x80000,1,0);
	restore_ca_dly_value();
#if 0
	bbu_printf("dq_dly: %d, dqs_dly: %d\n\r", dq_dly, dqs_dly);
	bbu_printf("dqc_dly: %d, dqsc_dly: %d\n\r", dq_dly, dqs_dly);
#endif
	set_dqs_val(dq_dly, dqs_dly);
	set_dqsc_val(dqc_dly, dqsc_dly);
	dqs_swp_test(0x80000,0x80000,0x1,1);  
	set_dqs_val(dq_dly, dqs_dly);
	set_dqsc_val(dqc_dly, dqsc_dly);
	dqs_swp_test(0x80000,0x80000,0x2,1);  
	set_dqs_val_3(org_dqs_val);
	restore_wck_dqs_dq_val();
}

int 		argv_h[3];
int 		pad_calibration_ok = 0;
extern int 	exit_when_error;
unsigned int 	z_d_value = 0x7;
extern 		uint32_t zpr_znr;
int ods_swp_st = 1, ods_swp_end = 3;
int ddr_swp_phasel(int cs0_buff_size, int cs1_buff_size, int swp_size, int is_phasel)
{
	char i, j, k;
	uint32_t value;
	int index, ret, total_win_size,avg_win_size,size;

	for(k = ods_swp_st; k <= ods_swp_end; k++) 
	{ 	
        	set_ods(k);
 	  	argv_h[0] = 0; 			
	  	argv_h[1] = swp_size; 	
	  	argv_h[2] = 2;  	
	  			
	  	bbu_printf("\n\r** bbu shmoo begin **\n\r\n\r");
		if(is_phasel) {
                	phasel_shmoo_use_mtsp = 1;
                	swp_zpnr_pass_window(0x10000, 0x10000);
			check_shadow_zpnr();
    			ddr_set_drv_val(z_d_value, zpr_znr, zpr_znr, 0, 1);
                	swp_dqs_window();
	  		ret = phs_swp_test(3, argv_h,cs0_buff_size,cs1_buff_size, NULL/*&info_temp*/);
		} else {
	   		dll_dly_swp_test(cs0_buff_size,cs1_buff_size); 
		}
		bbu_printf("\n\r===== bbu shmoo end =======\n\r\n\r\n\r");
	}
}
void set_ods_range()
{
	ods_swp_st = 1;
	ods_swp_end = 3;
}

static void do_pad_calibration(int z_drv)
{
	uint32_t value;
	
	if(use_mannual_drv)
		return;

        if(drive_strength_pad_calibration(z_drv) == 0) {
		pad_cal_result(1);
		pad_calibration_ok = 1;
	}
	else { 
		pad_cal_result(0);
		pad_calibration_ok = 0;
	}
			
	zpr_znr = ((org_reg_404 & 0xF0) >> 4);
	zpr_znr |= ((org_reg_404 & BIT_30) >> 26);

	check_shadow_zpnr();
    	ddr_set_drv_val(z_d_value,zpr_znr, zpr_znr, 0, 1);
}

int highest_pp;
uint32_t org_dq = 0;
extern uint32_t cur_reg_404;
/* ........................................................................ */
void bbu_ddr_sweep_test(int argc, uint8_t **argv)
{
	int i,j,index,zpr,znr,value;
    	int z_drv;
    	uint32_t k, phsel_test_bytes;
    	uint32_t phsel_test_bytes1;
	
	//get the orignal drv strength value
	value = BU_REG_READ(0xc0100404);
    	z_d_value = ((value >> 20) & 0xF);
    	
	value = BU_REG_READ(0xc0100404);
	zpr_znr = ((value & 0xF0) >> 4);
	zpr_znr |= ((value & BIT_30) >> 26);
	set_ods_range();
	bbu_printf("dft zdrv: 0x%x, zpnr: 0x%x\n\r",z_d_value,zpr_znr);	

    	switch(argc){
      	case 1:
      	case 2:
	case 3: 

       	if(strcmp((const char*)argv[1], "calb") == 0) {
       
       	    	z_drv =conv_dec((char *)argv[2]);
       	    	z_d_value = z_drv;

		ddr_set_drv_val(z_drv, zpr_znr, zpr_znr, 0, 0);
	  	dump_mck_regs();
	   	do_pad_calibration(z_drv);
		dump_mck_regs();
	}
       	else if(strcmp((const char*)argv[1], "sync") == 0) {
       	    	value = conv_dec((char *)argv[2]);
		for (i = 0; i < value; i++) {
			z_drv = (i & BIT_5) >> 5;
			zpr = (i & (0x3 << 3)) >> 3;
			znr = (i & (0x7 << 0)) >> 0;
			shmoo_phy_sync(z_drv, zpr, znr);
		}
	} 
	else if(strcmp((const char*)argv[1], "setdly") == 0){
	 	zpr =conv_dec((char *)argv[2]);
	 	bbu_printf("\n\rset dly: %d\n\r",zpr);
	 	ddr_set_dly(zpr);	
	}

	else if(strcmp((const char*)argv[1], "setpequ") == 0){
	 	zpr =conv_dec((char *)argv[2]);
	 	bbu_printf("\n\rset pequal: %d\n\r",zpr);
	 	phs0_phs1_equal = zpr;	
	}
	else if(strcmp((const char*)argv[1], "test") == 0){
	 	zpr =conv_dec((char *)argv[2]);
	 	ddr_swp_test(zpr);	
	}
      	else if((const char*)strcmp((const char*)argv[1], "400") == 0
       	     || (const char*)strcmp((const char*)argv[1], "533") == 0) {

		detect_default_ods();
		org_dqs_val = BU_REG_READ(0xc0100580);
		phs0_phs1_equal = 0; 
		
      		dswp_pp_change(1,argv104);
      		znr =conv_dec((char *)argv[2]);
		do_pad_calibration(z_d_value);
		
		phsel_test_bytes  = 0x10000;
      		phsel_test_bytes1 = 0x10000;		
		
		if((const char*)strcmp((const char*)argv[1], "400") == 0) {
			highest_pp = PP_400_IDX;
      			goto dswp_400;
      		} else if((const char*)strcmp((const char*)argv[1], "533") == 0) {
			highest_pp = PP_533_IDX;
      			goto dswp_533;
      		}
dswp_533:
      		dswp_pp_change(1,argv533);
      		ddr_swp_phasel(phsel_test_bytes,phsel_test_bytes1,63,1);
		if((znr & BIT_0) == 0)	
			goto swp_one_pp;
dswp_400:
      		dswp_pp_change(1,argv400);
      		ddr_swp_phasel(phsel_test_bytes,phsel_test_bytes1,63,1);
		if((znr & BIT_0) == 0)	
			goto swp_one_pp;

swp_one_pp:
		bbu_printf("\n\rhighest_pp: %d\n\r\n\r",highest_pp);
      		dswp_pp_change(1,argv104);
		do_pad_calibration(z_d_value);
				     			
      		k = 1; 
      		dswp_pp_change(1,argv312);
      		ddr_swp_phasel(phsel_test_bytes,phsel_test_bytes1,512,0);
      			
      		dswp_pp_change(1,argv104);
      		set_ods(0xffffffff); 
		do_pad_calibration(z_d_value);
		if(znr == 0 || znr == 1)
      			dll_swp_auto_log(k,znr);
	  }
	  break;

      case 4:
    	if(strcmp((const char*)argv[1], "setphs") == 0) {
	 	zpr =conv_dec((char *)argv[2]);
	 	znr =conv_dec((char *)argv[3]);
	 	bbu_printf("\n\rset phs0 to: %d\n\r",zpr);
	 	bbu_printf("set phs1 to: %d\n\r",znr);
	 	set_phs_val(zpr, znr, 0xF);	
	}
	else if(strcmp((const char*)argv[1], "setdqs") == 0) {
	 	zpr =conv_dec((char *)argv[2]);
	 	znr =conv_dec((char *)argv[3]);
	 	bbu_printf("\n\rset dqs to: %d\n\r",zpr);
	 	bbu_printf("set dq to: %d\n\r",znr);
	 	set_dqs_val(znr, zpr);	
		org_clk_dqs_dq_val = (((zpr&0x7f) << 24)|((znr&0x7f) << 16)|((zpr&0x7f) << 8)|(znr&0x7f));
	}
	else if(strcmp((const char*)argv[1], "setdqsc") == 0) {
	 	zpr =conv_dec((char *)argv[2]);
	 	znr =conv_dec((char *)argv[3]);
	 	bbu_printf("\n\rset dqsc to: %d\n\r",zpr);
	 	bbu_printf("set dqc to: %d\n\r",znr);
	 	set_dqsc_val(znr, zpr);
		org_clk_dqs_dq_val = (((zpr&0x7f) << 24)|((znr&0x7f) << 16)|((zpr&0x7f) << 8)|(znr&0x7f));
	} 
	else if(strcmp((const char*)argv[1], "setck") == 0) {
	 	zpr =conv_dec((char *)argv[2]);
	 	znr =conv_dec((char *)argv[3]);
	 	bbu_printf("\n\rset ca to: %d\n\r",zpr);
	 	bbu_printf("set ck to: %d\n\r",znr);
	 	set_ca_nonca_val(zpr, znr);	
	} 
	else if(strcmp((const char*)argv[1], "dly") == 0){
	   	phsel_test_bytes =conv_hex((char *)argv[2]);
	   	phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   	dll_dly_swp_test(phsel_test_bytes,phsel_test_bytes1);
	} 
	else {
	 	bbu_printf("err cmd\n\r");
	 	break;
	}
	break;
	
	case 5: 
	if(strcmp((const char*)argv[1], "mandrv") == 0
		|| strcmp((const char*)argv[1], "dismandrv") == 0 ){
	  	z_drv =conv_dec((char *)argv[2]);
	 	zpr =conv_dec((char *)argv[3]);
	 	znr =conv_dec((char *)argv[4]);

	 	zpr_znr = zpr;
	 	z_d_value = z_drv;
	 	ddr_set_drv_val(z_drv,zpr,znr, 0, 0);

		use_mannual_drv = 1;
		if(strcmp((const char*)argv[1], "dismandrv") == 0)
			use_mannual_drv = 0;
		else {
			org_reg_404 = cur_reg_404;
		}

		dump_mck_regs(); 
         	break;
    	}
	else if((const char*)strcmp((const char*)argv[1], "phasel") == 0){
	   	phsel_test_bytes =conv_hex((char *)argv[2]);
	   	phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   	zpr =conv_dec((char *)argv[4]);
	   	phasel_shmoo_use_mtsp = 1;
		argv_h[0] = 0; 			
	  	argv_h[1] = zpr; 		
	  	argv_h[2] = 2;  		
	        phs_swp_test(3, argv_h,phsel_test_bytes,phsel_test_bytes1, NULL /* &info_temp */);
	} 
	else if(strcmp((const char*)argv[1], "dqs") == 0) {
	   	phsel_test_bytes =conv_hex((char *)argv[2]);
	   	phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   	zpr = conv_dec((char *)argv[4]);
	   	dqs_swp_test(phsel_test_bytes,phsel_test_bytes1,zpr,1);   		
	} 
	else if(strcmp((const char*)argv[1], "ck") == 0) {
	   	phsel_test_bytes =conv_hex((char *)argv[2]);
	   	phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   	zpr =conv_hex((char *)argv[4]);
	   	cak_swp_test(phsel_test_bytes,phsel_test_bytes1,zpr,0);
	}
	break;
	
    default:
         break;         
   }
   return;
}

