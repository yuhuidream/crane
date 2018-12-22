#include "predefines.h"
#include <string.h>
#include "bbu.h"
#include "pmic.h"
#include "timer_dec.h"
#include "CIU.h"
#include "common_tests.h"
#include "ppset.h"
#include "mcu_generate_pattern.h"

//#define SWEEP_DRAMDRV

uint8_t *argv104[] = {"ppset","0"};
uint8_t *argv312[] = {"ppset","4"};
uint8_t *argv400[] = {"ppset","5"};
uint8_t *argv533[] = {"ppset","8"};
#if 0
void bbu_ddr_sweep_help()
{
#if !defined(CONFIG_NANO)
   bbu_printf("\n\rddrsw   -  sweep ddr parameters.\n\r");
   bbu_printf(" Usage: ddrswp  [opt]> <arg1> <arg2> <arg3>\n\r");
   bbu_printf("  opt= calb,    do pad calibration with arg1<zdrv value>\n\r");
   bbu_printf("                <arg1> - zdrv: set to 5 or 1\n\r");
   bbu_printf("  opt= phasel,  sweep dll phase sel\n\r");
   bbu_printf("                <arg1> - mtsp buffer size on CS0, e.g 0x800000(8M buffer)\n\r");
   bbu_printf("                <arg2> - mtsp buffer size on CS1, e.g 0x800000(8M buffer)\n\r");
   bbu_printf("                <arg3> - phasel end value, should <=31\n\r");
   bbu_printf("  opt= setphs,  set phasel/phasel1 value\n\r");
   bbu_printf("                <arg1> - phasel value, need do <ddrsw phasel> first\n\r");
   bbu_printf("                <arg2> - phasel1 value, need do <ddrsw phasel> first\n\r");
   bbu_printf("  opt= mandrv,  mannul drv strength test\n\r");
#endif
   return;
}
#endif

int ddr_is_ddr3()
{
	uint32_t reg;

	reg = BU_REG_READ(0xc0100300);
	if((reg&0xF8) == (0x2<<3))
	{
	    bbu_printf("***DDR3***\n\r");
		return 1;
	}
	else
		return 0;
}

int ddr_is_lpddr2()
{
	uint32_t reg;

	reg = BU_REG_READ(0xc0100300);
	if((reg&0xF8) == (0x1<<3))
	{
	    bbu_printf("***LPDDR2***\n\r");
		return 1;
	}
	else
		return 0;
}

void set_dram_drv(uint32_t k)
{
    uint32_t value;

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

	if(ddr_is_lpddr2())
	  	BU_REG_WRITE(0xc0100024,0x03020003);

	if(ddr_is_ddr3())
	    BU_REG_WRITE(0xc0100020,0x03000200);
	mdelay(10);
}

char tbl_index_to_drv_str[DRV_STR_MAX] = {15,7,3,5};

#if SWEEP_ZDRV
struct mck4_phasel_info mck4_info[DRV_STR_MAX] = {
	[0 ... (DRV_STR_MAX-1)] = {
		.calb_ok  	= 0,
		.calb_zpr_znr 	= -1,
		.swp_zpr_znr_st 	= -1,
		.swp_zpr_znr_end	= -1,
	},
};

char phasel_points[DRV_STR_MAX][16] = {-1};
char phasel_st[DRV_STR_MAX][16] = {-1};
char phasel_end[DRV_STR_MAX][16] = {-1};

struct mck4_phasel_info info_temp;


int drv_str_to_tbl_index(int drv_str)
{
	int i = 0;
	for(i=0; i<DRV_STR_MAX; i++)
		if(tbl_index_to_drv_str[i] == drv_str)
			return i;

	return -1;
}
#endif

void dump_mck_regs(void)
{
	  bbu_printf("0xC0100404: 0x%08X\n\r",BU_REG_READ(0xc0100404));
	  bbu_printf("0xc0100408: 0x%08X\n\r",BU_REG_READ(0xc0100408));
	  bbu_printf("0xc010040C: 0x%08X\n\r",BU_REG_READ(0xc010040C));
	  bbu_printf("0xC0100410: 0x%08X\n\r",BU_REG_READ(0xc0100410));
	  bbu_printf("0xc010041C: 0x%08X\n\r",BU_REG_READ(0xc010041C));
	  bbu_printf("0xc0100500: 0x%08X\n\r",BU_REG_READ(0xc0100500));
}

int ddr_size_mb()
{
	uint32_t reg, ddr_size_in_MB = 0,cs_size;

	reg = BU_REG_READ(0xC0100200);
	if(BIT_0&reg)
	{
		cs_size = (1<<(((reg&0X1F0000)>>16)-4));
		ddr_size_in_MB += cs_size;
	}

	reg = BU_REG_READ(0xC0100204);
	if(BIT_0&reg)
	{
		cs_size = (1<<(((reg&0X1F0000)>>16)-4));
		ddr_size_in_MB += cs_size;
	}

	return ddr_size_in_MB;
}

#if 0
void goto_normal_bbu(void)
{

	bbu_printf("Jump to 0x8000000\n\r");
	dsb();
	dmb();

	__asm__ __volatile__(
	"mrc		p15, 0, r1, c1, c0, 0\n\t"
	"mov		r2, #0x3000\n\t"
	"orr		r2, r2, #0x0005\n\t"
	"bic		r1, r1, r2\n\t"
  	"mcr     p15, 0, r1, c1, c0, 0\n\t"
	"mov 	r1, r1\n\t"
 	"sub 	pc, pc, #4\n\t"
    "mov      r1, #0xFFFFFFFF\n\t"
   	"mcr      p15, 0, r1, c3, c0, 0\n\t"
   	"mcr      p15, 0, r2, c8, c7, 0\n\t"
   	"mcr      p15, 0, r1, c7, c10, 4\n\t"
   	"mov 	r2, r2\n\t"
 	"sub 	pc, pc, #4\n\t"
 	"mov    pc, #0x8000000\n\t":::"memory");
}
#endif
static void pad_calb_result(int result)
{
	if(1 == result)
		bbu_printf("CLB Pass\n\r");
	else
		bbu_printf("CLB Fail\n\r");
}

int 		argv_h[3];
int 		pad_calibration_ok = 0;
extern int 	exit_when_error;
unsigned int 	z_drv_value = 0x7;
extern 		uint32_t zpr_znr;

int ddr_swp_phasel(int cs0_buff_size, int cs1_buff_size, int swp_size)
{
	char i, j, k;
	uint32_t value;
	int index, ret, total_win_size,avg_win_size,size;

#ifdef SWEEP_DRAMDRV
	for(k = 1; k <= 3; k++)
	{
        set_dram_drv(k);
#endif

#ifdef 	SWEEP_ZDRV
	   		for(i = 7; i > 1; i -= 2)
	   		{
	   			z_drv_value = i;

	   		    if(i == 15)
	   				i = 9;

#ifdef DDR_SWEEP_AUTO
				total_win_size = 0;
				index = drv_str_to_tbl_index(i);
#endif
	   			for ( j = 1; j <31; j++ )
	   			{

					zpr_znr = j;
					bbu_printf("****zx_drv: %d, zpr_znr: %d ****\n\r",i,j);
#endif
	  			argv_h[0] = 0; 			//start
	  			argv_h[1] = swp_size; 	//end
	  			argv_h[2] = 1;  		//inc

	  			bbu_printf("\n\r===== bbu dll phase swp begin =====\n\r\n\r");

	  			ret = dll_phase_sweep_test(3, argv_h,cs0_buff_size,cs1_buff_size, NULL/*&info_temp*/);

#ifdef SWEEP_ZDRV
				if(ret != -1)
				{
					if(mck4_info[index].swp_zpr_znr_st == -1)
						mck4_info[index].swp_zpr_znr_st = j;

					size = (info_temp.win_end -info_temp.win_st);
					total_win_size += size;

					phasel_st[i][j]		= info_temp.win_st;
					phasel_end[i][j]	= info_temp.win_end;
					phasel_points[i][j] = info_temp.win_st + (size >> 2);
				}
				else
			    {
					if(mck4_info[drv_str_to_tbl_index(i)].swp_zpr_znr_st != -1)
						mck4_info[drv_str_to_tbl_index(i)].swp_zpr_znr_end = j-1;

					avg_win_size = total_win_size /(j-mck4_info[index].swp_zpr_znr_st);

					int calb = mck4_info[index].calb_zpr_znr;

					if((calb != -1)
						&& (avg_win_size -2) > ((phasel_end[i][calb] - phasel_st[i][calb])))
					{
						bbu_printf("*****calibrated value is bad*****\n\r");
						mck4_info[index].calb_ok = 0;
					}
				}
#endif
	  			bbu_printf("\n\r===== bbu dll phase sweep end=======\n\r\n\r\n\r");

#ifdef SWEEP_ZDRV
	  	  }
	  	}
#endif

#ifdef SWEEP_DRAMDRV
	  }
#endif
}

static void do_pad_calibration(int z_drv)
{
	uint32_t value;
	//int index = drv_str_to_tbl_index(z_drv);

    if(drive_strength_pad_calibration(z_drv) == 0) {
		pad_calb_result(1);
		pad_calibration_ok = 1;
	} else if(drive_strength_pad_calibration(z_drv) == 0) {
		pad_calb_result(1);
		pad_calibration_ok = 1;
	} else if(drive_strength_pad_calibration(z_drv) == 0) {
		pad_calb_result(1);
		pad_calibration_ok = 1;
	} else {
		pad_calb_result(0);
		pad_calibration_ok = 0;
	}

	if(pad_calibration_ok)
    {
    	z_drv_value = z_drv;
    	value = BU_REG_READ(0xc010040C);
    	zpr_znr =((value&0x1F0) >> 4);
#ifdef SWEEP_ZDRV
    	mck4_info[index].calb_zpr_znr = zpr_znr;
    	mck4_info[index].calb_ok = 1;
#endif

    	bbu_printf("zx_drv: %d,zpr/znr=%d/%d\n\r",z_drv_value,zpr_znr,zpr_znr);
    }
    else
    {
#ifdef SWEEP_ZDRV
      	//do nothing
      	mck4_info[index].calb_ok = 0;
#endif
      	if(ddr_is_ddr3())
      		zpr_znr = 0x1F;
      	if(ddr_is_lpddr2())
      		zpr_znr = 0x1F;

      	zpr_znr = 0x1F;
    }
}

/* ........................................................................ */
void bbu_ddr_sweep_test(int argc, uint8_t **argv){
	int i,j,index,zpr,znr,value;
    int z_drv;
    uint32_t k, phsel_test_bytes;
    uint32_t phsel_test_bytes1;

    switch(argc){
      //case 1:
       //bbu_printf("error cmd formart\n\r");
	   //break;
#if 0
      case 2:
       if(check_help(argv[1]))  {
            bbu_ddr_sweep_help();
            break;
       }
#if (defined(CONFIG_NANO)&&defined(CONFIG_NANO_DMA))
       if((const char*)strcmp((const char*)argv[1], "dma") == 0){
       		dma_nano_test();
	  	}
#endif
	   if((const char*)strcmp((const char*)argv[1], "reset") == 0){
       		print_line_line();
       		if(chip_is_wdt_reset())
       		{
       			bbu_printf("BBU_WATCHDOG_REST\n\r");
       		}
       		else
       		{
       			bbu_printf("BBU_POWERON_REST\n\r");

       		}
       		print_line_line();
	   }
      break;
#endif

	  case 3:
	   if(check_help(argv[1]))  {
            //bbu_ddr_sweep_help();
            break;
       }

       if(strcmp((const char*)argv[1], "calb") == 0) {

       	    z_drv =conv_dec((char *)argv[2]);
       	    z_drv_value = z_drv;

	  		value = BU_REG_READ(0xc0100404);
	  		value &=0xf00fffff;
	  		value |=((z_drv << 20)|(z_drv << 24));
	  		BU_REG_WRITE(0xc0100404,value);

	  		value = BU_REG_READ(0xc0100408);
	  		value &=0xf00fffff;
	  		value |=((z_drv << 20)|(z_drv << 24));
	  		BU_REG_WRITE(0xc0100408,value);

	  		dump_mck_regs();
	   	    if(z_drv == 5 ||z_drv == 3 || z_drv == 7 ||
	   	       z_drv == 15 ) {

	   	       do_pad_calibration(z_drv);

	  	    }
		    dump_mck_regs();
	  }	 else if(strcmp((const char*)argv[1], "setdly") == 0){
	 	    zpr =conv_dec((char *)argv[2]);
	 	    bbu_printf("\n\rset dlldly: %d\n\r",zpr);
	 	    ddr_set_dll_dly(zpr);
	  }

#ifdef DDR_SWEEP_AUTO
      else if((const char*)strcmp((const char*)argv[1], "400") == 0
       	  || (const char*)strcmp((const char*)argv[1], "533") == 0 ){

      		pp_test(2,argv104);

      		z_drv = ((BU_REG_READ(0xc0100404) >> 24)&0xF);

      		znr =conv_dec((char *)argv[2]);

#ifdef SWEEP_ZDRV
			for(z_drv = 7; z_drv > 1; z_drv -= 2) {
			    index = drv_str_to_tbl_index(z_drv);
#endif
			    do_pad_calibration(z_drv);

#ifdef SWEEP_ZDRV
			}
#endif
      		phsel_test_bytes  = 0x200000;
      		phsel_test_bytes1 = 0x200000;

      		if((const char*)strcmp((const char*)argv[1], "533") == 0) {
      			pp_test(2,argv533);
      		}
      		else
      		{
      			pp_test(2,argv400);
      		}

      		ddr_swp_phasel(phsel_test_bytes,phsel_test_bytes1,31);

      		//step 3: sweep dll dly value
      		pp_test(2,argv104);
      		z_drv = ((BU_REG_READ(0xc0100404) >> 24)&0xF);
			do_pad_calibration(z_drv);

      		if((BU_REG_READ(0xc0100500)&0x3) == 0x3)
      		{
      			k = 1;
      			pp_test(2,argv312);
      			dll_dly_sweep_test(phsel_test_bytes,phsel_test_bytes1);
      		}
      		else
      			k = 0;

      		pp_test(2,argv104);
      		z_drv = ((BU_REG_READ(0xc0100404) >> 24)&0xF);
			do_pad_calibration(z_drv);
      		//step 4: output log
      		dll_sweep_auto_log(k,znr);
	  	}
#endif

	  if((const char*)strcmp((const char*)argv[1], "rwd") == 0){
	  		znr =conv_dec((char *)argv[2]);
			rwd_sweep_test(znr);
	   }

	  break;

      case 4:
#ifndef DDR_SWEEP_AUTO
    	if(strcmp((const char*)argv[1], "setphs") == 0){

	 	 zpr =conv_dec((char *)argv[2]);
	 	 znr =conv_dec((char *)argv[3]);
	 	 bbu_printf("\n\rset phasel to: %d\n\r",zpr);
	 	 bbu_printf("set phasel1 to: %d\n\r",znr);
	 	 dll_set_phase_phsel1(zpr, znr);

		}
		else if(strcmp((const char*)argv[1], "wck") == 0){
    		bbu_printf("=============================== sweep wck begin ===============================\n\r");
	   		phsel_test_bytes =conv_hex((char *)argv[2]);
	   		phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   		wck_sweep_test(phsel_test_bytes,phsel_test_bytes1);
    		bbu_printf("=============================== sweep wck  end  ===============================\n\r");
		} else if(strcmp((const char*)argv[1], "dqs") == 0){
    		bbu_printf("=============================== sweep dqs begin ===============================\n\r");
	   		phsel_test_bytes =conv_hex((char *)argv[2]);
	   		phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   		dq_dqs_sweep_test(phsel_test_bytes,phsel_test_bytes1,BIT_1,0);
    		bbu_printf("=============================== sweep dqs  end  ===============================\n\r");
		} else if(strcmp((const char*)argv[1], "ck") == 0){
    		bbu_printf("=============================== sweep ck_dly begin ===============================\n\r");
	   		phsel_test_bytes =conv_hex((char *)argv[2]);
	   		phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   		subphy_sweep_test(phsel_test_bytes,phsel_test_bytes1,1,0);
    		bbu_printf("=============================== sweep ck_dly  end  ===============================\n\r");
		} else if(strcmp((const char*)argv[1], "adcm") == 0){
    		bbu_printf("=============================== sweep adcm begin ===============================\n\r");
	   		phsel_test_bytes =conv_hex((char *)argv[2]);
	   		phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   		subphy_sweep_test(phsel_test_bytes,phsel_test_bytes1,0,0);
    		bbu_printf("=============================== sweep adcm  end  ===============================\n\r");
		} else
#endif
		if(strcmp((const char*)argv[1], "dly") == 0){
    		//bbu_printf("========sweep dll dly begin =========\n\r");
	   		phsel_test_bytes =conv_hex((char *)argv[2]);
	   		phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   		dll_dly_sweep_test(phsel_test_bytes,phsel_test_bytes1);
    		//bbu_printf("========sweep dll_dly  end  =========\n\r");
		}
		else {
	 		bbu_printf("err cmd\n\r");
	 		break;
		}
	break;

	case 5:

		if(strcmp((const char*)argv[1], "mandrv") == 0){
	  	 z_drv =conv_dec((char *)argv[2]);
	 	 zpr =conv_dec((char *)argv[3]);
	 	 znr =conv_dec((char *)argv[4]);

	 	 zpr_znr = zpr;
	 	 z_drv_value = z_drv;

	 	 ddr_set_drv_strength(z_drv,zpr,znr);
		 dump_mck_regs();
         break;

    } else if((const char*)strcmp((const char*)argv[1], "phasel") == 0){

	   		phsel_test_bytes =conv_hex((char *)argv[2]);
	   		phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   		zpr =conv_dec((char *)argv[4]);
	   		ddr_swp_phasel(phsel_test_bytes,phsel_test_bytes1,zpr);
	}


	else if((const char*)strcmp((const char*)argv[1], "setphs") == 0) {
		 z_drv = conv_dec((char *)argv[2]); // alias of byte num
	 	 zpr =conv_dec((char *)argv[3]); 	// alias of phasel
	 	 znr =conv_dec((char *)argv[4]);	// alias of phasel1
	 	 bbu_printf("set phasel0 -> 0x%08X,phasel1 -> 0x%08X for byte: 0x%08X\n\r",
	 	 			zpr,znr,z_drv);

		 value = BU_REG_READ(0xc0100500+z_drv<<2);
		 value &= 0xE0E0FFFC;
		 value |= ((zpr << 16)|(znr << 24));
		 BU_REG_WRITE(0xc0100500+(z_drv<<2),value);

		 BU_REG_WRITE(0xc0100420,BU_REG_READ(0xc0100420)|(BIT_29));
		 mdelay(10);

		 BU_REG_WRITE(0xc0100420,BU_REG_READ(0xc0100420)|(BIT_30));
		 mdelay(10);
	}
#ifndef DDR_SWEEP_AUTO
	else if(strcmp((const char*)argv[1], "dqs") == 0){
    		bbu_printf("=============================== sweep dqs begin ===============================\n\r");
	   		phsel_test_bytes =conv_hex((char *)argv[2]);
	   		phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   		zpr = conv_dec((char *)argv[4]);
	   		dq_dqs_sweep_test(phsel_test_bytes,phsel_test_bytes1,zpr,1);
    		bbu_printf("=============================== sweep dqs  end  ===============================\n\r");
	} else if(strcmp((const char*)argv[1], "ck") == 0){
    		bbu_printf("=============================== sweep ck_dly begin ===============================\n\r");
	   		phsel_test_bytes =conv_hex((char *)argv[2]);
	   		phsel_test_bytes1 =conv_hex((char *)argv[3]);
	   		zpr =conv_hex((char *)argv[4]);
	   		subphy_sweep_test(phsel_test_bytes,phsel_test_bytes1,1,zpr);
    		bbu_printf("=============================== sweep ck_dly  end  ===============================\n\r");
	}
#endif

	break;

    default:
         //bbu_ddr_sweep_help();
         break;
   } 	// case
   return;
}

