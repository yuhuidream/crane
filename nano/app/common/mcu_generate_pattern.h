
#ifndef __MCU_MEM_TEST_H__
#define __MCU_MEM_TEST_H__


#define     ALL                                        0
#define 	ANALOG                                  1
#define     CONSTANT                          2
#define     COMPLEMENT                            3
#define     DECREMENT                      4
#define     INCREMENT                       5
#define     INCREMENTB                    6
#define     MILLER_EFFECT                7
#define     ZERO_ROR_INVERT                  8
#define     ONE_ROL_INVERT            9
#define     ZERO_ROR                          10
#define     ONE_ROL                                      11
#define     WALKING_ONES                12
#define     WALKING_ZEROS              13
#define     SSTAILGATE_0101XXXX 14
#define     SSTAILGATE_01010000   15
#define     SSTAILGATE_01011111   16
#define     SSTAILGATE_01011010   17
#define     MSTAILGATE                      18
#define 	FREQ_SWEEP                         19
#define     VERT_INCREMENT           20
#define     HORZ_INCREMENT                   21

struct mck4_phasel_info {
	char calb_ok;
	char calb_zpr_znr;
	char swp_zpr_znr_st;
	char swp_zpr_znr_end;
	char win_st;
	char win_end;
};

enum ddr_swp_index {
	DRV_STR_15 = 0,
	DRV_STR_7,
	DRV_STR_3,
	DRV_STR_5,
	DRV_STR_MAX, //1 is omited.
};

enum platform{
	BBU = 0,
	ANDROID,
};

enum{
    	ODS_34OHM = 1,
    	ODS_40OHM,
    	ODS_48OHM,
    	NUM_ODS,
};

enum {
	PP_312_IDX = 0,
	PP_400_IDX,
	PP_533_IDX,
	PP_667_IDX,
	PP_800_IDX,
	NUM_PP_IDX,
};

enum {
	BYTE_LANE0 = 0,
	BYTE_LANE1,
	BYTE_LANE2,
	BYTE_LANE3,	
	NUM_BYTE_LANE,
};

struct dswp_result {
	char avail_ddrv;
	char is_default_ddrv;
	char is_perbyte;
	char is_bypass_mode;
	char phasel_pass_st[NUM_BYTE_LANE];
	char phasel_pass_end[NUM_BYTE_LANE];
	char phasel1_pass_st[NUM_BYTE_LANE];
	char phasel1_pass_end[NUM_BYTE_LANE];
	char common_phasel_pass_st;
	char common_phasel_pass_end;
	char dqs_pass_st;
	char dqs_pass_end;
	char dqs_val;
	char ca_pass_st;
	char ca_pass_end;
	char ca_set_val;
	char zpndrv;
	char zpr_pass_start;
	char zpr_pass_end;
	char dlldly_pass_st;
	int  dlldly_pass_end;
};

struct dswp_result_all { 
	struct dswp_result dswp_result[NUM_ODS];
};

#define SWP_DQS
#define SWP_ODS
#define DLL_PHASEL_PER_BYTE
#define DDR_SWP_AUTO
#ifdef PE_DDR_SWP
#undef SWP_ZDRV
#define SWP_ODS
#undef DLL_PHASEL_PER_BYTE
#endif
int drv_str_to_tbl_index(int drv_str);
#define min(a,b) (a<b? a:b)
#endif
