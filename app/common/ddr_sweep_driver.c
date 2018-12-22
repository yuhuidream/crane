//===================================================================================================================
// SV drive strength tune code
// FUNCTION NAME:	mcuDriveStrengthShmoo()
//-------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:	Test is used to calibrate the lowest DUT and DRAM drive strengths in order to reduce noise. 
// Run at maximum DDR frequency to ensure correct values
// Writes drive strength (DUT):
//a)	Coarse strength: Sweep zpdrv/zndrv from weak to strong: 0x1 (60),0x5 (48), 0x3 (40),0x7 (35) 
//b)	Fine strength: do pad calibration (at low DDR freq) and pick zpr/znr  with this equation:
//		zpr/znr= (((calibrated_zpr+calibrated_znr)/2)+offset) Where offset can be swept 0,1,2,3. Zpr can be copied to znr. 
//c)	Set dll phase sel 0x0 to check boundary on min side and run test
//d)	Use a simple test writing/reading 1Mb size so we get a burst of 0xf s and 0x0 s and 0xa s and 0x5s (even/odd mode)
//		Write and read from both CS0 and CS1. (if you run a bigger more stressful test, no issue, just takes time)
//e)	Set dll phase sel 0x6 (or use 0xA for lower stress test like CPU accesses) to check boundary on max side and run test
//f)	Use a simple test writing/reading 1Mb size so we get a burst of 0xf s and 0x0 s and 0xa s and 0x5s (even/odd mode)
//		Write and read from both CS0 and CS1. (if you run a bigger more stressful test, no issue, just takes time)
//g)	Check passing drive window and pick sweet spot (pick the lowest drive that can get good passing so it reduces noise )
//		Choose the first drive strength that passes for at least 2 offsets and choose the highest passing offset
//
// Reads drive strength (DRAM): 
//a)	Configure the chosen drive strength for writes from above and write the 0xF's/0x0's and 0xA's/0x5's pattern to CS0/1 range
//b)	Sweep dram drive strengths from weak to strong:  0x4 (60), 0x3 (48), 0x2 (40 default), 0x1 (34.3) 
//c)	Set dll phase sel 0x0 to check boundary on min side and run test
//d)	Use a simple test to read 1Mb size so we get a burst of 0xf s and 0x0 s and 0xa s and 0x5s (even/odd mode)
//		Read from both CS0 and CS1. (if you run a bigger more stressful test, no issue, just takes time)
//e)	Set dll phase sel 0x6 (or use 0xA for lower stress test like CPU accesses) to check boundary on max side and run test
//f)	Use a simple test to read 1Mb size so we get a burst of 0xf s and 0x0 s and 0xa s and 0x5s (even/odd mode)
//		Read from both CS0 and CS1. (if you run a bigger more stressful test, no issue, just takes time)
//g)	Check passing drive window and pick sweet spot (pick the lowest drive that can get good passing so it reduces noise )
//		Choose the first drive strength that passes 
//
// PARAMETERS:		argc  	  - Number of parameters passed to main
//					argv	  - Command Line parameters
// RETURN:			N/A
//===================================================================================================================
#include "predefines.h"
#include <string.h>
#include "bbu.h"
#include "pmic.h"
#include "timer_dec.h"
#include "CIU.h"
#include "common_tests.h"
#include "mcu_generate_pattern.h"
#include "clock.h"

//#define SV_MEM_TEST
#define NO_LOG
//#define USE_OLD_WINDOW_ALGO
//default use bounch sweep method
#define DLL_SWEEP_BOUNCH
#ifdef  DLL_SWEEP_BOUNCH
#define SIMPLE_TEST_DATA_SIZE  (0x400)
#define USE_OLD_WINDOW_ALGO
#endif

#define	EMEI_MCU_BASE_ADDRESS	0xC0100000
uint32_t zpr_znr=0x7, uiOffset=0x3;
//uint32_t uiByte=0x0, uiZDrv=0x1, uiDramDrv=0x4;
//uint32_t uiPhaseSel=0, uiCSSource=0x0, uiTemp1=0, uiTemp2=0, uiFails=0;
//uint32_t guiDDRHeapBaseAddress = 0x1ff00000;
//uint32_t guiSize = 0x200000/16;

void  dll_set_phase_phsel1(uint32_t phasel, uint32_t phasel1, int byte_map);
extern unsigned int z_drv_value;

static void ddr_set_phasel_phasel1(uint32_t phasel, uint32_t phasel1,int byte_map)
{
    uint32_t i;	 
	uint32_t value,byte_addr;
	bbu_printf("map: 0x%x\n\r",byte_map);
	
    for(i = 0; i < 4; i++)
    {
    	//bbu_printf("i: 0x%x\n\r",i);
	    //bbu_printf("%d,%d,0x%x\n\r",phasel,phasel1,byte_map);
	    if((BIT_0 << i)&byte_map) {
	        byte_addr = (i*4);
	       	byte_addr += 0xc0100500;
			//bbu_printf("byte_addr: 0x%x\n\r",byte_addr);
	    	value = BU_REG_READ(byte_addr);
	    	value &= 0xc0c0fffc;
	    	value |= ((phasel<< 16)|(phasel1<< 24));
	    	//bbu_printf("value: 0x%x\n\r",value);
	    	BU_REG_WRITE(byte_addr,value);
	    } 
    }
    
	//mdelay(10);
	//V3.7, pls confirm this	
	//bbu_printf("updat   e dll\n\r");
	BU_REG_WRITE(0xc0100420,BU_REG_READ(0xc0100420)|(BIT_29));
	//bbu_printf("REST DLL\n\r");
	mdelay(10); //delay is enough???
		
	BU_REG_WRITE(0xc0100420,BU_REG_READ(0xc0100420)|(BIT_30));
	//bbu_printf("update DLL\n\r");
	mdelay(10);
	bbu_printf("phsl: %d, phsl1: %d\n\r",phasel,phasel1);
	//dump_mck_regs();
}

void ddr_set_drv_strength(uint32_t zx_drv, uint32_t zpr, uint32_t znr)
{	
	uint32_t value;
	uint32_t zpr_4lsb,zpr_bit4,znr_4lsb,znr_bit4;
	zpr_4lsb = (zpr & 0xF); 
	zpr_bit4 = (zpr & BIT_4);
	znr_4lsb = (znr & 0xF); 
	znr_bit4 = (znr & BIT_4);
		
	value = BU_REG_READ(0xc0100404);
	value &=0x300ff00f;
	value |=((zx_drv << 20)|(zx_drv << 24));
 	value |= (zpr_4lsb <<4);
 	value |= (znr_4lsb << 8);
 	value |= ((znr_bit4 << (31-4)) | (zpr_bit4 << (30-4)));
 	BU_REG_WRITE(0xc0100404,value);
 	value = BU_REG_READ(0xc0100404);
 	
	value = BU_REG_READ(0xc0100408);
	value &=0x300ff00f;
	value |=((zx_drv << 20)|(zx_drv << 24));
 	value |= (zpr_4lsb <<4);
 	value |= (znr_4lsb << 8);
 	value |= ((znr_bit4 << (31-4)) | (zpr_bit4 << (30-4)));
 	BU_REG_WRITE(0xc0100408,value);
 	value = BU_REG_READ(0xc0100408);
	 	
	 	
	value = BU_REG_READ(0xc010040C);
 	value &= 0xfffffc00;
 	value |= (zpr_4lsb << 0);
 	value |= (znr_4lsb << 4);
 	value |= ((znr_bit4 << (8-4)) | (zpr_bit4 << (9-4)));
 	BU_REG_WRITE(0xc010040C,value);
 	value = BU_REG_READ(0xc010040C);
	  	 
	bbu_printf("zx_drv=0x%08X, zpr/znr=0x%08X\n\r",zx_drv,zpr);
	mdelay(10);	 
}

/*
*NOTE: zpr/znr will be modified when PP change,
*pad calibration can be done at system init stage, and set the zpr/znr value into PP table,
*or just change 0x404/0408/0x40C after PP change.
*zdrv_value defaut value is 0x5 now
* if zdrv_value is 5, then uiOffset is set to 0
* if zdrv_value is 1, then uiOffset is set to 5
*/
void print_line_line(void)
{
    bbu_printf("***************\n\r");
}

int drive_strength_pad_calibration(int zdrv_val)
{
	uint32_t uiPPadCal =0x7, uiNPadCal=0x7;
	uint32_t phy_ctrl_9;
	//int iWrFailCount[4][4], iRdFailCount[4], iPassCount=0;
	//int i=0;
	//Do pad calibration at default freq (or low DDR 104MHz freq in order to get accurate result)
	//1) enable pad calibration by writing to (offset 0x410) phy control reg 10 bit 16 pad_cal_auto=1
	*(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x410) |= (1<<16);	
	//2) do not set pad_cal_auto_sel yet because we dont want to use the calibrated values directly
	//check after a 100 cycles wait time to see if offset 0x0420 bit 2 is set (phy_cal_done) and phy_cal_err (bit 3) is not set 
	//(else report error)
	udelay(10);
	print_line_line();
	
	zpr_znr = ((BU_REG_READ(0xC010040C)&0x1F0)>>4);
	bbu_printf("default zpr/znr: 0x%x/0x%x\n\r",zpr_znr,zpr_znr);
	
	phy_ctrl_9 = *(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x420);
	
	//3) calibration procedure error checks
	if(phy_ctrl_9 & (1<<2))
	{
		if((phy_ctrl_9 & (1<<3)))
		{
			bbu_printf("MAXED OUT [PHY_CTRL14=0x%08X]\n\r"
					,phy_ctrl_9);
			
			zpr_znr = 0x1F;
            ddr_set_drv_strength(zdrv_val, zpr_znr,zpr_znr);
	        goto out_success;
		}

		//3) if it is not set, then read phy_cal_zpr, phy_cal_znr (offset 0x0420) which is the calibrated value
		//4) check to make sure it is not 0 (else report error)
		if(((phy_ctrl_9 & (0x1F<<20))) && ((phy_ctrl_9 & (0x1F<<15)))){
			uiPPadCal = (((phy_ctrl_9 & (0x1F<<20))>>20));
			uiNPadCal = (((phy_ctrl_9 & (0x1F<<15))>>15));
			bbu_printf("PASS-PHY_CAL_ZPR=0x%01X, PHY_CAL_ZNR=0x%01X\n\r",uiPPadCal,uiNPadCal);
		} else {
			bbu_printf("FAIL-ZNR/ZPR value of 0x0 [PHY_CTRL14=0x%08X]\n\r",
					phy_ctrl_9);
			//V3.7, ZPR/ZNR =0X7/0X7??
			goto out_fail;
		}
	}
	
	else{
		bbu_printf("FAIL-PHY CAL DONE NOT DETECTED [PHY_CTRL14=0x%08X]\n\r"
				,phy_ctrl_9);
									
		//V3.7, how about this ???? ZPR/ZNR =0X7/0X7??
		goto out_fail;
	}
	
	//3) disable pad calibration pad_cal_auto to save power
	*(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x410) &= ~(1<<16);	

	 if(zdrv_val == 1) {
		bbu_printf("zx_drv = 1,set Offset=5\n\r");
		uiOffset = 5;
	 }	
	
	//4) copy znr to zpr and copy the values to all pads only if the value is >=2
	if (uiPPadCal >= 2)
	{
		uiNPadCal = uiPPadCal;
		if (uiPPadCal+uiOffset <= 31)
        {
			bbu_printf("uiPPadCal: 0x%08X, uiOffset: 0x%08X\n\r",uiPPadCal,uiOffset);
			bbu_printf("zpr/znr = 0x%08X\n\r",uiPPadCal +  uiOffset);
			zpr_znr = uiNPadCal +  uiOffset;
            ddr_set_drv_strength(zdrv_val,zpr_znr,zpr_znr);
        } else {              
			bbu_printf("(zpr/znr = 0x%08X\n\r",0xf);
			zpr_znr = 0x1f;
            ddr_set_drv_strength(zdrv_val,zpr_znr,zpr_znr);
        }
        
   } else {
			//V3.7, how about this ???? ZPR/ZNR =0X7/0X7??
			bbu_printf("uiPPadCal<2, deflt zpr/znr\n\r");
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

static void dll_sweep_prepare(void)
{
   	//mcuWritePhy_control_13Dll_auto_manual_up(0);
   	//V3.7,pls confirm on this, should clear or set BIT_3 ??????
   	BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_3))); // BIT_3 is 0 in blf, This bit is reserved according to ZhengXue
    BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_2)));     
    BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_0)));      
    BU_REG_WRITE(0xc0100420,(BU_REG_READ(0xc0100420)&(~BIT_30))); 
    mdelay(10);
}

/*
*V3.7
*Find pass window and set phasel/phasel1, phasel and phasel1 are set to same values now
*if there is no pass window, set to 2/2 by default
*/

unsigned char phasel_test_result[32][4] = {{0}};
char h[32] = {0};
char l[32] = {0};
char r[32] = {0};
char dll_test_result[512/8] = {0};
#ifdef DLL_SWEEP_BOUNCH
static char phasel_win_st = 0;
static char phasel_win_end = 0;
static char dlldly_win_st = 0;
static char dlldly_win_end = 0;
#ifdef DLL_PHASEL_PER_BYTE
static char phasel_win_st0 = 0;
static char phasel_win_end0 = 0;	
static char phasel_win_st1 = 0;
static char phasel_win_end1 = 0;	
static char phasel_win_st2 = 0;
static char phasel_win_end2 = 0;	
static char phasel_win_st3 = 0;
static char phasel_win_end3 = 0;	
static char max_phasel_st = 0;
static char min_phasel_end = 100;	
#endif
int find_bounch_phase_pass_window(int n,int is_dlldly,struct mck4_phasel_info * info, int byte_map)
{
    int i,j,k = 0,max = 0;
    int l = -1,r = -2,ll = -1,rr = -2;
    int first_pass = -1;
    
    for(i = 0; i < n; i++)
    {
		if((((dll_test_result[i>>3]&(1<<(i%8)))!=(1<<(i%8))))) 
		{
			l = i;
			r = i; 	
			j = i+1;
			
			if(max < (r-l+1)) {
				max = r-l+1;
				ll = l;
				rr = r;
			}
			
			if(k == 0) {
				first_pass = l;
				k = 1;
			}
				
			while(j < n)
			{
				if((((dll_test_result[j/8]&(1<<(j%8))) != (1<<(j%8)))
					&&((dll_test_result[(j-1)/8]&(1<<((j-1)%8))) != (1<<((j-1)%8)))))
				{
					r = j;
					
					if(max < (r-l+1)) {
						max = r-l+1;
						ll = l;
						rr = r;
					}
					
					j++;
				
				} else break;
			}
		} //if
    } //for
    
 	if(max == 1)
	{
		i = l;
		j = r;
	}
	
 	if(max > 1)
	{
		i = ll;
		j = rr;
	}	
	
	bbu_printf("zx_drv:%d, zpr/znr:%d/%d\n\r",z_drv_value,zpr_znr);
	//bbu_printf("i = %d, j= %d\n\r",i,j);
	if(i >= n)
		i = j = 0;
		
	if(!is_dlldly)
	{
			phasel_win_st = i;
			phasel_win_end = j;
			
#ifdef DLL_PHASEL_PER_BYTE
			if(BIT_0&byte_map)
			{
				phasel_win_st0 = i;
				phasel_win_end0 = j;
			}
			if(BIT_1&byte_map)
			{
				phasel_win_st1 = i;
				phasel_win_end1 = j;
			}
			if(BIT_2&byte_map)
			{
				phasel_win_st2 = i;
				phasel_win_end2 = j;
			}
			if(BIT_3&byte_map)
			{
				phasel_win_st3 = i;
				phasel_win_end3 = j;
			}
			
			if(max_phasel_st < i)
				max_phasel_st = i;
				
			if(min_phasel_end > j)
				min_phasel_end = j;				
#endif
    		bbu_printf("Phasel pass win: [%d, %d]\n\r",i,j);
    		
    }
    else
    {
			dlldly_win_st = i;
			dlldly_win_end = j;
#ifdef SWEEP_ZDRV
			if(info != NULL) 
			{			
				info->win_st = i;
    			info->win_end = j;
    		}
#endif
    		bbu_printf("Dll dly pass win: [%d, %d]\n\r",i,j);
	} 	

    if(i <= j && j!=0)
    {
    	return (i + ((j-i) >>2));	
    }
    else
    { 
    	bbu_printf("No pass win\n\r");
    	return -1;	
    }
}
#endif

/**
*get the pass window of phase sel value, this is used in the drive strength sweep function
*argv[0]:start dll phase sel value, 0~0x1F, argv[1]:end dll phase sel value
*argv[2]: phase sel inc value
*/
extern uint32_t mtsp_total_error;
extern int exit_when_error;

int dll_phase_sweep_test(uint32_t argc, int argv[],uint32_t phsel_test_bytes,uint32_t phsel_test_bytes1, struct mck4_phasel_info * info)
{
	int byte, i, j, x, y,yy, byte_map;
	uint32_t value, ret_val, uiDlyStart=0x00, uiDlyEnd=0x1F, uiDlyInc=1;
	uint32_t uiDLLPhSel, uiDlyStart1=0x00, uiDlyEnd1=0x1F, uiDLLPhSel1;

	(void)argc;
	bbu_printf("zx_drv: 0x%08X\n\r",z_drv_value);
	bbu_printf("zpr/znr: 0x%08X\n\r",zpr_znr,zpr_znr);

	dll_sweep_prepare();
     
	uiDlyStart = argv[0];
	uiDlyEnd = argv[1];
	bbu_printf("Test buf: [0x%08X - 0x%08X]\n\r",uiDlyStart,uiDlyEnd);
	
	uiDlyInc = argv[2];
	
	if((uiDlyStart > uiDlyEnd) || (uiDlyStart > 0x3F))
		uiDlyStart=0x00;
		
	if(uiDlyEnd > 0x3F)
		uiDlyEnd=0x3F;
		
	if((uiDlyStart1 > uiDlyEnd1) || (uiDlyStart1 > 0x3F))
		uiDlyStart1=0x00;
		
	if(uiDlyEnd1 > 0x3F)
		uiDlyEnd1=0x3F;
		
	//set the calibrated out zpr/znr, zpr_znr is calibarated out at first stage
	//bbu_printf("1\n\r");
	ddr_set_drv_strength(z_drv_value,zpr_znr, zpr_znr);
	//bbu_printf("2\n\r");
 	byte_map = 0xF;
#ifdef DLL_PHASEL_PER_BYTE
 	for(byte = 0; byte < 4; byte++) {
 		byte_map = (BIT_0 << byte);
#endif
	//Shmoo DLL_PHSEL from 0x0 to point of failure
	for(uiDLLPhSel=uiDlyStart; uiDLLPhSel<=uiDlyEnd; uiDLLPhSel+=uiDlyInc)
	{
			//set phasel/phasel1	 
			//bbu_printf("3\n\r");
			ddr_set_phasel_phasel1(uiDLLPhSel, uiDLLPhSel,byte_map);
			//bbu_printf("4\n\r");
			mtsp_total_error = 0; //clear error num before invoke mtsp test
			ret_val = bbu_mtsp_test((0x1000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
			//bbu_printf("5\n\r");
			//bbu_printf("bbu mtsp return value is %d\n\r",ret_val);
		    y = uiDLLPhSel/8;
		    yy =uiDLLPhSel%8;
		    
		if(ret_val) {
			dll_test_result[y] |= (1 << yy); //fail
			//bbu_printf("phasel sweep FAILED\n\r");
		} else {
			dll_test_result[y] &= ~(1 << yy); //pass
			//bbu_printf("phasel sweep PASS\n\r");
		}	
			
			//bbu_printf("------------------------------>mtsp_total_error: %d\n\r",mtsp_total_error);
			//bbu_printf("------------------------------>exit_when_error:  %d\n\r",exit_when_error);
	} ///end of for(uiDLLPhSel...
	
#ifndef DDR_SWEEP_AUTO
	bbu_printf("==============phasel/phasel1 sweep results==============\n\r");
	bbu_printf("S: pass, F: fail, then choose a pass value from pass window\n\r");
	bbu_printf("\n\r====================    phasel1   ======================\n\r");
	bbu_printf("           ");
	mdelay(10);
	
	for(i = 0;i <=uiDlyEnd; i++)
		bbu_printf("%2d ", i);
			
	bbu_printf("\n\r");
	
	for(i = 0; i <= uiDlyEnd; i++)
	{
		bbu_printf("phasel - %2d:",i);
			
		for(j = 0; j <= uiDlyEnd; j++)
		{
			x = i;
			y = j/8;
			yy = j%8;
			
			if(i == j) {
				if(((phasel_test_result[x][y])&(1<<yy)) == 0)
					bbu_printf("S  ");	
				else
					bbu_printf("F  ");
			}else {
					bbu_printf("   ");
			}
		}
			
		bbu_printf("\n\r");
	}
#endif	//DDR_SWEEP_AUTO

	print_line_line();
	// value: 1/3 pass window
	
#ifndef DLL_PHASEL_PER_BYTE
	value = find_bounch_phase_pass_window(uiDlyEnd+1,0,info, 0);
	//just set phasel/phasel to the same value
	// user can also use cmd to set phasel/phasel1
	if(value != -1)
	{
		bbu_printf("\n\rset phs/phs1 = %d/%d\n\r",value,value);
    	dll_set_phase_phsel1(value, value, 0xF);
    }
   	else
   	{
   		bbu_printf("\n\rpass win too small\n\r");
    	dll_set_phase_phsel1(2, 2, 0xF);
   	}
#else
	value = find_bounch_phase_pass_window(uiDlyEnd+1,0,info, byte_map);
	if(i == 3)
	{
		bbu_printf("4 bytes overlap pass win: [%d,%d]\n\r",max_phasel_st, min_phasel_end);
		value = max_phasel_st + ((min_phasel_end - max_phasel_st)>>2);
	}
	
	if(value != -1)
	{
		bbu_printf("\n\rset phsl/phsl1 = %d/%d\n\r",value,value);
    	dll_set_phase_phsel1(value, value, byte_map);
    }
   	else
   	{
   		bbu_printf("\n\rpass window too small\n\r");
    	dll_set_phase_phsel1(2, 2, byte_map);
   	}
#endif   	   	

#ifdef DLL_PHASEL_PER_BYTE
	}
#endif

	//set 1/3 pass window if find pass window, else use default value in blf
	//V3.7, set bit 3, ALSO dll_set_phase_phsel1 will be invoked in setphs cmd
	//BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)|(BIT_3)));
	return value;
}

#ifndef DDR_SWEEP_AUTO
void set_dq_dqs_dly_value(uint32_t dq_dly, uint32_t dqs_dly)
{
	BU_REG_WRITE(0xc0100580,((dqs_dly<<8)|dq_dly));	
    BU_REG_WRITE(0xc0100584,((dqs_dly<<8)|dq_dly));
    BU_REG_WRITE(0xc0100588,((dqs_dly<<8)|dq_dly));
    BU_REG_WRITE(0xc010058c,((dqs_dly<<8)|dq_dly));
	BU_REG_WRITE(0xc01005a0,((dqs_dly<<8)|dq_dly));
    BU_REG_WRITE(0xc01005a4,((dqs_dly<<8)|dq_dly));
    BU_REG_WRITE(0xc01005a8,((dqs_dly<<8)|dq_dly));
    BU_REG_WRITE(0xc01005ac,((dqs_dly<<8)|dq_dly));
    mdelay(4); 
}

void set_ck_adcm_dly_value(uint32_t ck_dly, uint32_t adcm_dly)
{
  	BU_REG_WRITE(0xc0100610,ck_dly); 	      
  	BU_REG_WRITE(0xc0100614,ck_dly);
  	
  	BU_REG_WRITE(0xc0100620,adcm_dly);  
  	BU_REG_WRITE(0xc0100624,adcm_dly);
  	BU_REG_WRITE(0xc0100628,adcm_dly);  
  	mdelay(4);
}

char wck_pass_window[16][16][16][2] = {{{{0}}}};
#endif

int argv_dll[3] = {0, 31, 1};

#ifndef DDR_SWEEP_AUTO
int wck_sweep_test(uint32_t phsel_test_bytes,uint32_t phsel_test_bytes1)
{
	int ret_val;
	int i,j,k,l;
	int ck, adcm,dq,dqs;
			
   	BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_3))); // BIT_3 is 0 in blf, This bit is reserved according to ZhengXue
    BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_2)));     
    mdelay(10);
    
	//Shmoo DLL_PHSEL from 0x0 to point of failure
	for(ck = 0; ck < 16; ck++)
	for(adcm = 0; adcm < 16; adcm++)
	for(dq = 0; dq < 16; dq++)
	for(dqs = 0; dqs < 16; dqs++)
	{
		set_ck_adcm_dly_value(ck, adcm);
		set_dq_dqs_dly_value(dq, dqs);
		mtsp_total_error = 0; //clear error num before invoke mtsp test
		ret_val = bbu_mtsp_test((0x4000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
		bbu_printf("bbu mtsp return value is %d\n\r",ret_val);
		if(ret_val)
		{
			 bbu_printf("[ck,adcm,dq,dqs]=[%d,%d,%d,%d],FAIL\n\r",ck,adcm,dq,dqs);
			 wck_pass_window[ck][adcm][dq][dqs>>3] &= ~(1<<(dqs%8)); 
		} 
		else {
				mtsp_total_error = 0; //clear error num before invoke mtsp test
				ret_val = bbu_mtsp_test((0x4000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
				bbu_printf("bbu mtsp return value is %d\n\r",ret_val);
				
				if(ret_val) 
				{
			 		bbu_printf("[ck,adcm,dq,dqs]=[%d,%d,%d,%d],FAIL\n\r",ck,adcm,dq,dqs);
			 		wck_pass_window[ck][adcm][dq][dqs>>3] &= ~(1<<(dqs%8)); 
				} else {
					bbu_printf("                                                                                                                          [ck,adcm,dq,dqs]=[%d,%d,%d,%d],Pass\n\r",ck,adcm,dq,dqs);
					wck_pass_window[ck][adcm][dq][dqs>>3] |= (1<<(dqs%8));
				}
		}
	} 
	
	
	bbu_printf("******************************\n\r");
    dump_mck_regs();
	bbu_printf("******************************\n\r");	
	
	//Shmoo DLL_PHSEL from 0x0 to point of failure
	for(ck = 0; ck < 16; ck++)
	for(adcm = 0; adcm < 16; adcm++)
	for(dq = 0; dq < 16; dq++)
	for(dqs = 0; dqs < 16; dqs++)
	{
		if((wck_pass_window[ck][adcm][dq][dqs>>3]&((1<<(dqs%8)))) == 0)
			continue;

		z_drv_value = 0x7;
		//zpr_znr = 0xF;
		argv_dll[0] = 0;
		argv_dll[1] = 31;
		argv_dll[2] = 1;
		
		set_ck_adcm_dly_value(ck, adcm);
		set_dq_dqs_dly_value(dq, dqs);	
			
		bbu_printf("\n\r******************phasel sweep for [ck,adcm,dq,dqs]=[%d,%d,%d,%d] begins------------------------------------------------------------->\n\r",ck,adcm,dq,dqs);
		dll_phase_sweep_test(3, argv_dll,phsel_test_bytes,phsel_test_bytes1,NULL);
		bbu_printf("******************phasel sweep for [ck,adcm,dq,dqs]=[%d,%d,%d,%d] ends ------------------------------------------------------>\n\r\n\r\n\r",ck,adcm,dq,dqs);		
	} 

	return 0;
}
void set_dqs_dly_value(uint32_t dqs0[2],uint32_t dqs1[2],uint32_t dqs2[2],uint32_t dqs3[2])
{

	BU_REG_WRITE(0xc0100580,(dqs0[1]<<8)|(dqs0[0]));	
    BU_REG_WRITE(0xc0100584,(dqs1[1]<<8)|(dqs1[0]));	
    BU_REG_WRITE(0xc0100588,(dqs2[1]<<8)|(dqs2[0]));	
    BU_REG_WRITE(0xc010058c,(dqs3[1]<<8)|(dqs3[0]));	
	BU_REG_WRITE(0xc01005a0,(dqs0[1]<<8)|(dqs0[0]));	
    BU_REG_WRITE(0xc01005a4,(dqs1[1]<<8)|(dqs1[0]));	
    BU_REG_WRITE(0xc01005a8,(dqs2[1]<<8)|(dqs2[0]));	
    BU_REG_WRITE(0xc01005ac,(dqs3[1]<<8)|(dqs3[0]));	
    mdelay(4); 
    bbu_printf("******0xc0100580: 0x%08X******\n\r",BU_REG_READ(0xc0100580));
}

int dq_dqs_sweep_test(uint32_t phsel_test_bytes,uint32_t phsel_test_bytes1, int dqs_map, int is_simu)
{
	int ret_val;
	int i,j,k,l;
	int dqs0, dqs1,dqs2,dqs3;
	uint32_t dq0[2],dq1[2],dq2[2],dq3[2];
	uint32_t old_dq,old_dqs;
			
   	BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_3))); // BIT_3 is 0 in blf, This bit is reserved according to ZhengXue
    BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_2)));     
    mdelay(10);
    
    old_dq = dq0[0] = BU_REG_READ(0xc0100580)&(0x7F);
    dq1[0] = dq0[0];
    dq2[0] = dq0[0]; 
    dq3[0] = dq0[0];
    
    old_dqs = dq0[1] = (BU_REG_READ(0xc0100580)&0x7F00)>>8;
    dq1[1] = dq0[1];
    dq2[1] = dq0[1];
    dq3[1] = dq0[1];        
 
	//Shmoo DLL_PHSEL from 0x0 to point of failure
	for(dqs0 = 0; dqs0 < 16; dqs0++)
	for(dqs1 = 0; dqs1 < 16; dqs1++)
	for(dqs2 = 0; dqs2 < 16; dqs2++)
	for(dqs3 = 0; dqs3 < 16; dqs3++)
	{
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
		}
		
		if(dqs_map&BIT_1)
		{
    		dq0[1] = dqs0;
   	 		dq1[1] = dqs1;
    		dq2[1] = dqs2;
    		dq3[1] = dqs3;    
		}
		
		set_dqs_dly_value(dq0,dq1,dq2,dq3);
		
		mtsp_total_error = 0; //clear error num before invoke mtsp test
		ret_val = bbu_mtsp_test((0x4000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
		bbu_printf("bbu mtsp return value is %d\n\r",ret_val);
		if(ret_val)
		{
			 if(dqs_map&BIT_1)
			 	bbu_printf("[dqs0,dqs1,dqs2,dqs3]=[%d,%d,%d,%d],FAIL\n\r",dqs0,dqs1,dqs2,dqs3);
			 if(dqs_map&BIT_0)
			 	bbu_printf("[dq0 ,dq1 ,dq2 ,dq3 ]=[%d,%d,%d,%d],FAIL\n\r",dqs0,dqs1,dqs2,dqs3);	
			 	
			 wck_pass_window[dqs0][dqs1][dqs2][dqs3>>3] &= ~(1<<(dqs3%8)); 
		} 
		else {
				mtsp_total_error = 0; //clear error num before invoke mtsp test
				ret_val = bbu_mtsp_test((0x4000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
				bbu_printf("bbu mtsp return value is %d\n\r",ret_val);
				
				if(ret_val) 
				{
					if(dqs_map&BIT_1)
			 		bbu_printf("[dqs0,dqs1,dqs2,dqs3]=[%d,%d,%d,%d],FAIL\n\r",dqs0,dqs1,dqs2,dqs3);
			 		if(dqs_map&BIT_0)
			 		bbu_printf("[dqq0,dqq1,dqq2,dqq3]=[%d,%d,%d,%d],FAIL\n\r",dqs0,dqs1,dqs2,dqs3);
			 		wck_pass_window[dqs0][dqs1][dqs2][dqs3>>3] &= ~(1<<(dqs3%8)); 
				} else {
					if(dqs_map&BIT_1)
					bbu_printf("                                                                                                                          [dqs0,dqs1,dqs2,dqs3]=[%d,%d,%d,%d],Pass\n\r",dqs0,dqs1,dqs2,dqs3);
					if(dqs_map&BIT_0)
					bbu_printf("                                                                                                                          [dqq0,dqq1,dqq2,dqq3]=[%d,%d,%d,%d],Pass\n\r",dqs0,dqs1,dqs2,dqs3);
					wck_pass_window[dqs0][dqs1][dqs2][dqs3>>3] |= (1<<(dqs3%8));
				}
		}
	} 
	
	bbu_printf("******************************\n\r");
    dump_mck_regs();
	bbu_printf("******************************\n\r");	
	
	//Shmoo DLL_PHSEL from 0x0 to point of failure
	for(dqs0 = 0; dqs0 < 16; dqs0++)
	for(dqs1 = 0; dqs1 < 16; dqs1++)
	for(dqs2 = 0; dqs2 < 16; dqs2++)
	for(dqs3 = 0; dqs3 < 16; dqs3++)
	{
		if((wck_pass_window[dqs0][dqs1][dqs2][dqs3>>3]&((1<<(dqs3%8)))) == 0)
			continue;

		z_drv_value = 0x7;
		//zpr_znr = 0xF;
		argv_dll[0] = 0;
		argv_dll[1] = 31;
		argv_dll[2] = 1;
		
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
		}else
		{
			dq0[0] = old_dq;
   	 		dq1[0] = old_dq;
    		dq2[0] = old_dq;
    		dq3[0] = old_dq;   
		}
		
		if(dqs_map&BIT_1)
		{
    		dq0[1] = dqs0;
   	 		dq1[1] = dqs1;
    		dq2[1] = dqs2;
    		dq3[1] = dqs3;    
		}else
		{
	    	dq0[1] = old_dqs;
   	 		dq1[1] = old_dqs;
    		dq2[1] = old_dqs;
    		dq3[1] = old_dqs;  	
		}
		
		set_dqs_dly_value(dq0,dq1,dq2,dq3);
		
		bbu_printf("\n\r******************phasel sweep for [dqs0,dqs1,dqs2,dqs3]=[%d,%d,%d,%d] begins ------------------->\n\r",dq0[1],dq1[1],dq2[1],dq3[1]);		
		bbu_printf("\n\r******************phasel sweep for [dqq0,dqq1,dqq2,dqq3]=[%d,%d,%d,%d] begins ------------------->\n\r",dq0[0],dq1[0],dq2[0],dq3[0]);
		dll_phase_sweep_test(3, argv_dll,phsel_test_bytes,phsel_test_bytes1,NULL);
		
		bbu_printf("******************phasel sweep for [dqs0,dqs1,dqs2,dqs3]=[%d,%d,%d,%d] ends -------------------------------->\n\r\n\r\n\r",dq0[1],dq1[1],dq2[1],dq3[1]);		
		bbu_printf("******************phasel sweep for [dqq0,dqq1,dqq2,dqq3]=[%d,%d,%d,%d] ends -------------------------------->\n\r\n\r\n\r",dq0[0],dq1[0],dq2[0],dq3[0]);			
	} 

	return 0;
}
void set_subphy_dly(uint32_t subphy0,uint32_t subphy1,uint32_t subphy2,int is_ck)
{
    bbu_printf("subphy wck is not supported on mck5\n");
}

int subphy_sweep_test(uint32_t phsel_test_bytes,uint32_t phsel_test_bytes1, int is_ck, int sweep_pattern)
{
	int ret_val;
	int i,j,k,l;
	int subphy0, subphy1, subphy2;
	uint32_t reg;

   	BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_3))); // BIT_3 is 0 in blf, This bit is reserved according to ZhengXue
    BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)&(~BIT_2)));     
    mdelay(10);
    
	//Shmoo DLL_PHSEL from 0x0 to point of failure
	for(subphy0 = 0; subphy0 < 16; subphy0++)
	for(subphy1 = 0; subphy1 < 16; subphy1++)
	for(subphy2 = 0; subphy2 < 16; subphy2++)
	{
	    if(sweep_pattern == 3)
	    {
	        if((subphy0 == subphy1) && (subphy0 == subphy2))
	            set_ck_adcm_dly_value(subphy0, subphy0);
	        else
	            continue;
	    } else if(sweep_pattern == 2){
	        reg = BU_REG_READ(0xc010038C)&0x1F; 
	        if((subphy0 == subphy1) && (subphy0 == subphy2))
	            set_ck_adcm_dly_value(subphy0, reg);
	        else
	            continue;
	    } else if(sweep_pattern == 1){
	        reg = BU_REG_READ(0xc0100388)&0x1F; 
	        if((subphy0 == subphy1) && (subphy0 == subphy2))
	            set_ck_adcm_dly_value(reg, subphy0);
	        else
	            continue;
	    } else {
		    set_subphy_dly(subphy0,subphy1,subphy2,is_ck);
		}
		
		mtsp_total_error = 0; //clear error num before invoke mtsp test
		ret_val = bbu_mtsp_test((0x4000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
		bbu_printf("bbu mtsp return value is %d\n\r",ret_val);
		if(ret_val)
		{
			 bbu_printf("[subphy0,subphy1,subphy2]=[%d,%d,%d],FAIL\n\r",subphy0,subphy1,subphy2);
			 wck_pass_window[0][subphy0][subphy1][subphy2>>3] &= ~(1<<(subphy2%8)); 
		} 
		else {
				mtsp_total_error = 0; //clear error num before invoke mtsp test
				ret_val = bbu_mtsp_test((0x4000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
				bbu_printf("bbu mtsp return value is %d\n\r",ret_val);
				
				if(ret_val) 
				{
			 		bbu_printf("[subphy0,subphy1,subphy2]=[%d,%d,%d],FAIL\n\r",subphy0,subphy1,subphy2);
			 		wck_pass_window[0][subphy0][subphy1][subphy2] &= ~(1<<(subphy2%8)); 
				} else {
					bbu_printf("                                                                                                                          [subphy0,subphy1,subphy2]=[%d,%d,%d],Pass\n\r",subphy0,subphy1,subphy2);
					wck_pass_window[0][subphy0][subphy1][subphy2>>3] |= (1<<(subphy2%8));
				}
		}
	} 
	
	bbu_printf("******************************\n\r");
    dump_mck_regs();
	bbu_printf("******************************\n\r");	
	
	//Shmoo DLL_PHSEL from 0x0 to point of failure
	for(subphy0 = 0; subphy0 < 16; subphy0++)
	for(subphy1 = 0; subphy1 < 16; subphy1++)
	for(subphy2 = 0; subphy2 < 16; subphy2++)
	{
		if(sweep_pattern == 3)
	    {
	        if((subphy0 == subphy1) && (subphy0 == subphy2)
	            && ((wck_pass_window[0][subphy0][subphy0][subphy0>>3]&((1<<(subphy0%8)))) != 0))
	            set_ck_adcm_dly_value(subphy0, subphy0);
	        else
	            continue;
	    } else if(sweep_pattern == 2){
	        reg = BU_REG_READ(0xc010038C)&0x1F; 
	        if((subphy0 == subphy1) && (subphy0 == subphy2)
	           && ((wck_pass_window[0][subphy0][subphy0][subphy0>>3]&((1<<(subphy0%8)))) != 0))
	            set_ck_adcm_dly_value(subphy0, reg);
	        else
	            continue;
	    } else if(sweep_pattern == 1){
	        reg = BU_REG_READ(0xc0100388)&0x1F; 
	        if((subphy0 == subphy1) && (subphy0 == subphy2)
	          && ((wck_pass_window[0][subphy0][subphy0][subphy0>>3]&((1<<(subphy0%8)))) != 0))
	            set_ck_adcm_dly_value(reg, subphy0);
	        else
	            continue;
	    } else {
	        if((wck_pass_window[0][subphy0][subphy0][subphy0>>3]&((1<<(subphy0%8)))) != 0)
		        set_subphy_dly(subphy0,subphy1,subphy2,is_ck);
		    else 
		        continue;
		}
		
		z_drv_value = 0x7;
		//zpr_znr = 0xF;
		argv_dll[0] = 0;
		argv_dll[1] = 31;
		argv_dll[2] = 1;
		
		set_subphy_dly(subphy0,subphy1,subphy2,is_ck);
		bbu_printf("\n\r******************phasel sweep for [subphy0,subphy1,subphy2=[%d,%d,%d] begins **********************------------------->\n\r",subphy0,subphy1,subphy2);
		dll_phase_sweep_test(3, argv_dll,phsel_test_bytes,phsel_test_bytes1,NULL);
		bbu_printf("******************phasel sweep for [subphy0,subphy1,subphy2]=[%d,%d,%d] ends ********************----------------------->\n\r\n\r\n\r",subphy0,subphy1,subphy2);		
	} 

	return 0;
}
#endif

void ddr_set_dll_dly(uint32_t dll_val)
{
	uint32_t value;	
	//sync clk
	//BU_REG_WRITE(0xc0100420,BU_REG_READ(0xc0100420)|(BIT_31));
	//mdelay(1);

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
	mdelay(4);
	bbu_printf("***dll delay: %d ***\n\r",dll_val);	
}

int dll_dly_sweep_test(uint32_t phsel_test_bytes,uint32_t phsel_test_bytes1)
{
	int ret_val,argv[3];
 	int i, j, x, y,yy;
	uint32_t value, uiDlyStart=0x00, uiDlyEnd=0x1F, uiDlyInc=1;
	uint32_t uiDLLPhSel;
		
 	argv[0] = 0;
 	argv[1] = 0x1ff;
 	argv[2] = 1;
	uiDlyStart = argv[0];
	uiDlyEnd = argv[1];
	bbu_printf("Test Buff: [0x%08X - 0x%08X]\n\r",uiDlyStart,uiDlyEnd);
	uiDlyInc = argv[2];

	if((uiDlyStart > uiDlyEnd) || (uiDlyStart > 0x1FF))
		uiDlyStart=0x00;
		
	if(uiDlyEnd > 0x1FF)
		uiDlyEnd=0x1FF;
			
	//set the calibrated out zpr/znr, zpr_znr is calibarated out at first stage
	ddr_set_drv_strength(z_drv_value, zpr_znr, zpr_znr);
	
	//Shmoo DLL_PHSEL from 0x0 to point of failure
	for(uiDLLPhSel=uiDlyStart; uiDLLPhSel<=uiDlyEnd; uiDLLPhSel+=uiDlyInc)
	{

		//set phasel/phasel1	 
		ddr_set_dll_dly(uiDLLPhSel);
		mtsp_total_error = 0; //clear error num before invoke mtsp test
		ret_val = bbu_mtsp_test((0x1000000 - (phsel_test_bytes)),phsel_test_bytes+phsel_test_bytes1,0xffffffff,0);
		//bbu_printf("bbu mtsp return value is %d\n\r",ret_val);
		
		y = uiDLLPhSel/8;
		yy =uiDLLPhSel%8;
		
		if(ret_val) {
			dll_test_result[y] |= (1 << yy); //fail
			//bbu_printf("Dll phase sweep FAILED\n\r");
		} else {
			dll_test_result[y] &= ~(1 << yy); //pass
			//bbu_printf("Dll phase sweep PASS\n\r");
		}		
	} ///end of for(uiDLLPhSel...
	
#ifndef DDR_SWEEP_AUTO
	bbu_printf("==============dll delay sweep results==============\n\r");
	bbu_printf("S: pass, F: fail, then choose a pass value from pass window\n\r");
	bbu_printf("\n\r****************************************************************\n\r");
	bbu_printf("           ");
	mdelay(10);
	
	for(i = 0;i <=uiDlyEnd; i++)
		bbu_printf("%2d ", i);
	bbu_printf("\n\r");
	
	for(j = 0; j <= uiDlyEnd; j++)
	{
		y = j/8;
		yy = j%8;
			
		if(((dll_test_result[y])&(1<<yy)) == 0)
			bbu_printf("S ");	
		else
			bbu_printf("F ");
	}	
	
#endif
	
	print_line_line();
	// value: 1/4 pass window
	value = find_bounch_phase_pass_window(uiDlyEnd+1,1,NULL,0);
	//just set phasel/phasel to the same value
	// user can also use cmd to set phasel/phasel1
	print_line_line();
	if(value != -1)
	{
		bbu_printf("\n\r---->set dll dly: %d\n\r",value);
    	ddr_set_dll_dly(value);
	}
	else
	{
		bbu_printf("\n\r---->set dll dly: %d\n\r",22);
    	ddr_set_dll_dly(22);
	}
	print_line_line();
	//set 1/4 pass window if find pass window, else use default value in blf
	//V3.7, set bit 3, ALSO dll_set_phase_phsel1 will be invoked in setphs cmd
	//BU_REG_WRITE(0xc010041C,(BU_REG_READ(0xc010041C)|(BIT_3)));
	return value;
}

/*
*this function is invoked when dll sweep is over
*/
void  dll_set_phase_phsel1(uint32_t phasel, uint32_t phasel1,int byte_map)
{
	uint32_t value;		
	ddr_set_drv_strength(z_drv_value, zpr_znr, zpr_znr);
	ddr_set_phasel_phasel1(phasel, phasel1,byte_map);
	return;	
}

extern uint8_t *argv104[];
extern uint8_t *argv400[];
extern uint8_t *argv533[];
extern int ddr_size_mb();
#if 0
void rwd_sweep_test(int is_533)
{
	int i,ret_val, buffer1,buffer2;
	uint32_t reg, fifo_delay_old_val,fifo_delay_old_val2, fifo_dly_index;
	
	pp_test(2,argv104);
	fifo_delay_old_val = BU_REG_READ(0xC0100400);
	
	mdelay(1);
	pp_test(2,argv400);
	fifo_delay_old_val2 = BU_REG_READ(0xC0100400);
	
	if(fifo_delay_old_val == fifo_delay_old_val2)
		fifo_dly_index = 0x27;
	else
		fifo_dly_index = 0x28;
	
	if(is_533 == 533)
		pp_test(2,argv533);	
	else
		pp_test(2,argv400);
	
	//sweep reg 0x220
	fifo_delay_old_val = BU_REG_READ(0xC0100400);
	
	for(i = 15; i >= 0; i --)
	{
	
		reg = BU_REG_READ(0xC0100400);
		reg &= 0xffffff00;
		reg |= (i|i<<4);
		BU_REG_WRITE(0xC01000C4,reg);
		BU_REG_WRITE(0xC01000C8,0x00000400);
		BU_REG_WRITE(0xC01000C0,fifo_dly_index);
		
		pp_test(2,argv104);
		mdelay(1);
		
		if(is_533 == 533)
			pp_test(2,argv533);	
		else
			pp_test(2,argv400);
		bbu_printf("\t\t\t\t\t******end of fifo_dq_dly %d *****\n\r\n\r\n\r",i);

		buffer1 = 0x200000;
		buffer2 = 0x200000;
		
		dll_phase_sweep_test(3, argv_dll,buffer1,buffer2,NULL);			
	}
		
	BU_REG_WRITE(0xC01000C4,fifo_delay_old_val);
	BU_REG_WRITE(0xC01000C8,0xc0100400);
	BU_REG_WRITE(0xC01000C0,0x00000028);
	
	pp_test(2,argv104);
	mdelay(1);
		
	if(is_533 == 533)
		pp_test(2,argv533);	
	else
		pp_test(2,argv400);
	
	//sweep reg 0x08c
	fifo_delay_old_val = BU_REG_READ(0xC01003AC);
	for(i = 7; i >= 0; i --)
	{
		reg = BU_REG_READ(0xC01003AC);
		reg &= 0xfff1ffff;
		reg |= (i<<17);
		BU_REG_WRITE(0xC01000C4,reg);
		BU_REG_WRITE(0xC01000C8,0x000003AC);
		BU_REG_WRITE(0xC01000C0,0x00000024);
		
		pp_test(2,argv104);
		mdelay(1);
		
		if(is_533 == 533)
			pp_test(2,argv533);	
		else
			pp_test(2,argv400);

		bbu_printf("\t\t\t\t\t******end of rwd dly %d ******\n\r\n\r\n\r",i);
		if(ddr_size_mb() <= 512)
		{
			buffer1 = 0x200000;
			buffer2 = 0;
		}
		else
		{
			buffer1 = 0x200000;
			buffer2 = 0x200000;
		}
	
		dll_phase_sweep_test(3, argv_dll,buffer1,buffer2,NULL);
	}
	
	BU_REG_WRITE(0xC01000C4,fifo_delay_old_val);
	BU_REG_WRITE(0xC01000C8,0x000003AC);
	BU_REG_WRITE(0xC01000C0,0x00000024);
	
	pp_test(2,argv104);
	mdelay(1);
		
	if(is_533 == 533)
		pp_test(2,argv533);	
	else
		pp_test(2,argv400);
}
#else
void rwd_sweep_test(int is_533)
{
    (void)is_533;
}
#endif
extern struct mck4_phasel_info mck4_info[DRV_STR_MAX];
extern char tbl_index_to_drv_str[DRV_STR_MAX];
extern char phasel_points[DRV_STR_MAX][16];
extern char phasel_st[DRV_STR_MAX][16] ;
extern char phasel_end[DRV_STR_MAX][16];
static uint32_t phy_ctrl_7;
static uint32_t phy_ctrl_8;
static uint32_t phy_ctrl_9;
static uint32_t phy_ctrl_phasel;
static uint32_t phy_ctrl_dlldly;

void dll_sweep_auto_log(int is_bypass_mode, int platform)
{
	int i,phasel_val,dll_val, index, max_zpr_znr_win = 0;
	uint32_t zpr_znr_4lsb,zpr_znr_bit4;
	
#ifdef SWEEP_ZDRV
	for(i = 0; i < DRV_STR_MAX; i++)
	{
		if(mck4_info[i].calb_ok == 0)
			continue;
			
		if(mck4_info[i].calb_zpr_znr > mck4_info[i].swp_zpr_znr_st
			 && mck4_info[i].calb_zpr_znr < mck4_info[i].swp_zpr_znr_end)
		{
			z_drv_value = tbl_index_to_drv_str[i];
			zpr_znr = mck4_info[i].calb_zpr_znr;
			phasel_val = phasel_st[i][zpr_znr] + ((phasel_end[i][zpr_znr] - phasel_st[i][zpr_znr])>>2);
			break;
		}
		else {
			if((mck4_info[i].swp_zpr_znr_end -mck4_info[i].swp_zpr_znr_st) > max_zpr_znr_win)
			{
				max_zpr_znr_win = (mck4_info[i].swp_zpr_znr_end - mck4_info[i].swp_zpr_znr_st);
				z_drv_value = tbl_index_to_drv_str[i];
				zpr_znr = mck4_info[i].swp_zpr_znr_st + (max_zpr_znr_win << 1)/3;
				phasel_val = phasel_points[i][zpr_znr];
			}
		}
	}      
	
	phasel_win_st 	= phasel_st[z_drv_value][zpr_znr] ;
	phasel_win_end	= phasel_end[z_drv_value][zpr_znr];
	bbu_printf("\n\r\n\rPhasel pass window: [%d,%d]\n\r",phasel_win_st,phasel_win_end);
	
#else   //SWEEP_ZDRV
#ifdef DLL_PHASEL_PER_BYTE
	bbu_printf("\n\r\n\r*****byte 0 phasel pass window: [%d,%d]\n\r",phasel_win_st0,phasel_win_end0);
	bbu_printf("*****byte 1 phasel pass window: [%d,%d]\n\r",phasel_win_st1,phasel_win_end1);
	bbu_printf("*****byte 2 phasel pass window: [%d,%d]\n\r",phasel_win_st2,phasel_win_end2);
	bbu_printf("*****byte 3 phasel pass window: [%d,%d]\n\r",phasel_win_st3,phasel_win_end3);
	
#ifdef CONFIG_MMT
    bbu_putchar(0x80); 
#endif	
	if(max_phasel_st >= min_phasel_end)
	{
		//bbu_printf("*****no common phasel pass window*****\n\r");
		bbu_printf("*****use default phasel0/phasel1*****\n\r");
		max_phasel_st = min_phasel_end = 4;
		bbu_printf("*****common phasel pass window: [%d,%d]\n\r",0,16);
	}
	else
		bbu_printf("*****common phasel pass window: [%d,%d]\n\r",max_phasel_st,min_phasel_end);

	phasel_win_st   = max_phasel_st;
	phasel_win_end 	=min_phasel_end;
#else
	bbu_printf("\n\r\n\rPhasel pass window: [%d,%d]\n\r",phasel_win_st,phasel_win_end);
#endif
#endif

	if(is_bypass_mode)
	{
		if(dlldly_win_st < dlldly_win_end)
			bbu_printf("*****dlldly pass window: [%d,%d]\n\r",dlldly_win_st,dlldly_win_end);
		else
		{
			bbu_printf("*****dlldly pass window: [%d,%d]\n\r",0,128);
			bbu_printf("***************dlldly is default value for MMT **************\n\r");
		}
	} 
	else //master mode
	{
		bbu_printf("*****dlldly pass window: [%d,%d]\n\r",0,128);
		bbu_printf("***************dlldly is fake value for MMT **************\n\r");
	}
	
	bbu_printf("zpr_znr:%d#\n\r",zpr_znr);
	zpr_znr_4lsb = zpr_znr&0xF;
	zpr_znr_bit4 = zpr_znr&BIT_4;
	
	phy_ctrl_7 = ((*(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x404) 
				 & 0x300FF00F) |(z_drv_value<<24)|(z_drv_value<<20)
        |(zpr_znr_4lsb<<4)|(zpr_znr_4lsb<<8)|(zpr_znr_bit4<<(30-4))|(zpr_znr_bit4<<(31-4)));
            			 
    phy_ctrl_8 = ((*(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x408) 
            			& 0x300FF00F) |(z_drv_value<<24)|(z_drv_value<<20)
        |(zpr_znr_4lsb<<4)|(zpr_znr_4lsb<<8)|(zpr_znr_bit4<<(30-4))|(zpr_znr_bit4<<(31-4)));
            			 
    phy_ctrl_9 = ((*(uint32_t *)(EMEI_MCU_BASE_ADDRESS+0x40C) 
            			& 0xFFFFFC00) |(zpr_znr_4lsb <<4)|zpr_znr_4lsb
            			|(zpr_znr_bit4<<(8-4))|(zpr_znr_bit4<<(9-4))); 
           			
    bbu_printf("BLF_UPDATE_BEGIN\n\r");  	
    phasel_val = (phasel_win_st + ((phasel_win_end - phasel_win_st)>>2));
	dll_val = (dlldly_win_st + ((dlldly_win_end - dlldly_win_st)>>2));		
	
	for(i = 4; i>=2; i--)
	{
		if(i == 4)
		{
	    	if(is_bypass_mode)
    			ddr_set_dll_dly(dll_val);
   	 		else
    			ddr_set_phasel_phasel1(phasel_val, phasel_val,0xF);	
		}
		
	bbu_printf("*****choose 1/%d pass window ****\n\r",i);
	phasel_val = (phasel_win_st + (phasel_win_end - phasel_win_st)/i);
	dll_val = (dlldly_win_st + (dlldly_win_end - dlldly_win_st)/i);
	
	phy_ctrl_phasel = (phasel_val << 16 |phasel_val << 24);
	phy_ctrl_dlldly = (dll_val << 4 |0x3);
	
	bbu_printf("WRITE%d_404 = <0xC0100404,0x%08X>\n\r",4-i,phy_ctrl_7);
	bbu_printf("WRITE%d_408 = <0xC0100408,0x%08X>\n\r",4-i,phy_ctrl_8);
	bbu_printf("WRITE%d_40C = <0xC010040C,0x%08X>\n\r",4-i,phy_ctrl_9);
	bbu_printf("WRITE%d_500 = <0xc0100500,0x%08X>\n\r",4-i,phy_ctrl_phasel);
	
	if(platform == BBU)
		bbu_printf("WRITE%d_304 = <0xc0100500,0x%08X>\n\r",4-i,phy_ctrl_phasel);
	
	if(is_bypass_mode)
		bbu_printf("WRITE%d_304 = <0xc0100500,0x%08X>\n\r",4-i,phy_ctrl_dlldly);
	else
		bbu_printf("WRITE%d_304 = <0xc0100500,0x%08X>\n\r",4-i,phy_ctrl_phasel);
		
	if(is_bypass_mode)
		bbu_printf("WRITE%d_304 = <0xc0100500,0x%08X>\n\r",4-i,phy_ctrl_dlldly);
	else
		bbu_printf("WRITE%d_304 = <0xc0100500,0x%08X>\n\r",4-i,phy_ctrl_phasel);	

	if(platform == ANDROID)
		bbu_printf("WRITE%d_304 = <0xc0100500,0x%08X>\n\r",4-i,phy_ctrl_phasel);	
	}
	
	bbu_printf("BLF_UPDATE_END\n\r");  

#ifdef CONFIG_MMT
    	bbu_putchar(0x81); 
#endif	
	ddr_set_drv_strength(z_drv_value,zpr_znr,zpr_znr);
}

