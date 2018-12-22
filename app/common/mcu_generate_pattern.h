
#ifndef __MCU_MEM_TEST_H__
#define __MCU_MEM_TEST_H__


#define     ALL					0
#define 	ANALOG				1
#define     CONSTANT			2
#define     COMPLEMENT			3
#define     DECREMENT			4
#define     INCREMENT			5
#define     INCREMENTB			6
#define     MILLER_EFFECT		7
#define     ZERO_ROR_INVERT		8
#define     ONE_ROL_INVERT		9
#define     ZERO_ROR			10
#define     ONE_ROL				11
#define     WALKING_ONES		12
#define     WALKING_ZEROS		13
#define     SSTAILGATE_0101XXXX 14
#define     SSTAILGATE_01010000	15
#define     SSTAILGATE_01011111	16
#define     SSTAILGATE_01011010	17
#define     MSTAILGATE			18
#define 	FREQ_SWEEP			19
#define     VERT_INCREMENT		20
#define     HORZ_INCREMENT		21

struct mck4_phasel_info {
	char calb_ok;
	char calb_zpr_znr;
	char swp_zpr_znr_st;
	char swp_zpr_znr_end;
	char win_st;
	char win_end;
};

enum ddr_sweep_index {
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

//#define SWEEP_ZDRV
#define DLL_PHASEL_PER_BYTE
int drv_str_to_tbl_index(int drv_str);
#endif
