#include "predefines.h"
#include "MFPR.h"
#include "mfp-emei.h"
#include "mfp.h"
#include "APBCLK.h"
#include "pmu_dec.h"
#include "common_tests.h"
#include "pmic.h"
#include "system_mrvl.h"
#include "bbu.h"
#include "timer_dec.h"

static unsigned long nezha3_v10_pin_config[] = {
	PCM_CLK						               ,                       
	PCM_SYNC					               ,                       
	PCM_TXD						               ,                       
	PCM_RXD						               ,                                          
	CP_WAKE_AP		                   ,                       
	CP_BOOT_ACK		                   ,                       
	CP_WDT_LCD_RST		               ,                       
	CP_ASSERT	    		               ,                                              
	CHARGER_INT                      ,                       
	WLAN_WAKE_HOST                   ,                                      
	CP_UART_RXD				               ,                       
	CP_UART_TXD				               ,                               
	CI2C_SCL					               ,                       
	CI2C_SDA					               ,                             
	BAT_DET                         ,                                 
	SD_DAT3                         ,                       
	SD_DAT2                         ,                       
	SD_DAT1                         ,                       
	SD_DAT0                         ,                       
	SD_CMD                          ,                       
	SD_CLK                          ,                       
	SD_CD                           ,                       
	SD_WP                           ,                                      
	CHG_STAT                        ,                       
	// CLK_REQ                         ,                       
	//VCXOREQ                         ,                                         
	DIGRF_EN                        ,                       
	HSL_DATA2		                   ,                       
	HSL_DATA1		                   ,                       
	HSL_DATA0		                   ,                       
	HSL_DATA4		                   ,                       
	HSL_DATA3		                   ,                       
	HSL_CLK			                   ,                                      
	MMC3_CMD                        ,                       
	WLAN_RSTN                       ,                       
	WLAN_PDN                        ,                       
	LTEWLAN_COEX_UART_IN            ,                       
	LTEWLAN_COEX_UART_OUT           ,   
	CHG_OTG_EN                      ,
	CHG_STAT                        ,                    
	MMC3_DAT3                       ,                       
	MMC3_DAT2                       ,                       
	MMC3_DAT1                       ,                       
	MMC3_DAT0                       ,                       
	MMC3_CLK                        ,                       
	SEC_DIGRF_EN                    ,                                        
	KEY_UAP                         ,                       
	KEY_TD                          ,                       
	SEC_SYS_CLK_EN                  ,                       
	RF_DCDC_EN                      ,                       
	SEC_RESET_B                     ,                       
	UART2_TXD                       ,                       
	UART2_RXD                       ,                       
	GPS_CLK_EN                      ,                       
	KEY_WIFI                        ,  
	DVC0           ,
	DVC1		       ,
	//CHARGER_DISABLE,
	SM_NCS0		     ,
	SM_NCS1		     ,
	ND_NCS1		     ,
	ND_NCS0		     ,
	ND_NRE		     ,
	ND_NWE		     ,
	ND_RDY0	       ,
	//ND_RDY1	       ,
	ND_ALE			   ,
	ND_CLE			   ,   
	ND_IO0				,
	ND_IO1				,
	ND_IO2				,
	ND_IO3				,
	ND_IO4				,
	ND_IO5				,
	ND_IO6				,
	ND_IO7				,
	ND_IO8				,
	ND_IO9				,
	ND_IO10				,
	ND_IO11				,
	ND_IO12				,
	ND_IO13				,
	ND_IO14				,
	ND_IO15				,                  
	APT_DCDC_EN_BB			,
	GPS_PPS			 	,
	GPS_TIMER_SYNC		 	,
	FSCHG_GPIO2			,
	FSCHG_GPIO1			,
	GPS_ON_OFF	        	,
	GPS_RESET_N	        	,
	BOARD_ID0			,
	BOARD_ID1			,
	BOARD_ID2			,
};

static unsigned long nezha3_v20av10_diff_pin_config[] = {
               TP_RESET				,
               LCD_TE				,
               LCD_PWM				,
               Z3A_DVC0				,
               Z3A_DVC1				,
               Z3A_GPS_RESET_N			,
               Z3A_GPS_ON_OFF			,
               BT_WAKE_HOST			,
               TP_INT				,

               CHG_PSEL				,
               Z3A_WLAN_WAKE_HOST		,
               LCD_RST				,
               USB_SEL				,
               CHG_INT				,
               Z3_CHG_STAT			,
               CODEC_INT			,

               LCD_RESETN			,
               LCD_D13				,
               LCD_WRN				,
               LCD_D11				,
               LCD_RDN				,
               LCD_CSN				,
               LCD_DATA_CMD			,
               LCD_D15				,
               LCD_D14				,
               LCD_D12				,
               LCD_D0				,
               LCD_D1				,
               LCD_D2				,
               LCD_D3				,
               LCD_D4				,
               LCD_D5				,
               LCD_D6				,
               LCD_D7				,
               LCD_D8				,
               LCD_D9				,
               LCD_D10				,
};

static unsigned long nezha3_v20bv20a_diff_pin_config[] = {
               EMMC_RESETN			,
               PCIE_CLKREQ_N			,
               PCIE_WAKE_N			,
               PERST_N				,
#if 0
               EMMC50_DAT0			,
               EMMC50_DAT1			,
               EMMC50_DAT2			,
               EMMC50_DAT3			,
               EMMC50_DAT4			,
               EMMC50_DAT5			,
               EMMC50_DAT6			,
               EMMC50_DAT7			,
               EMMC50_CMD			,
               EMMC50_CLK			,
               EMMC50_STROBE			,
#endif
};

static struct mfp_addr_map emei_addr_map[] = {
	MFP_ADDR_X(GPIO0, GPIO54, 0xdc),
	MFP_ADDR_X(GPIO60, GPIO66, 0x304),
	MFP_ADDR_X(GPIO67, GPIO98, 0x1b8),
	MFP_ADDR_X(GPIO100, GPIO109, 0x238),
	//MFP_ADDR_X(GPIO110, GPIO116, 0x298),
	MFP_ADDR(DF_IO0, 0x40),
	MFP_ADDR(DF_IO1, 0x3c),
	MFP_ADDR(DF_IO2, 0x38),
	MFP_ADDR(DF_IO3, 0x34),
	MFP_ADDR(DF_IO4, 0x30),
	MFP_ADDR(DF_IO5, 0x2c),
	MFP_ADDR(DF_IO6, 0x28),
	MFP_ADDR(DF_IO7, 0x24),
	MFP_ADDR(DF_IO8, 0x20),
	MFP_ADDR(DF_IO9, 0x1c),
	MFP_ADDR(DF_IO10, 0x18),
	MFP_ADDR(DF_IO11, 0x14),
	MFP_ADDR(DF_IO12, 0x10),
	MFP_ADDR(DF_IO13, 0xc),
	MFP_ADDR(DF_IO14, 0x8),
	MFP_ADDR(DF_IO15, 0x4),

	MFP_ADDR(DF_nCS0_SM_nCS2, 0x44),
	MFP_ADDR(DF_nCS1_SM_nCS3, 0x48),
	MFP_ADDR(SM_CS0, 0x4c),
	MFP_ADDR(SM_CS1, 0x50),
	MFP_ADDR(DF_WEn, 0x54),
	MFP_ADDR(DF_REn, 0x58),
	MFP_ADDR(DF_CLE_SM_OEn, 0x5c),
	MFP_ADDR(DF_ALE_SM_WEn, 0x60),
	MFP_ADDR(SM_SCLK, 0x64),
	MFP_ADDR(DF_RDY0, 0x68),
	MFP_ADDR(SM_BE0, 0x6c),
	MFP_ADDR(SM_BE1, 0x70),
	MFP_ADDR(SM_ADV, 0x74),
	MFP_ADDR(DF_RDY1, 0x78),
	MFP_ADDR(SM_ADVMUX, 0x7c),
	MFP_ADDR(SM_RDY, 0x80),
	MFP_ADDR(ANT_SW4, 0x26c),

	MFP_ADDR_X(MMC1_DAT7, MMC1_WP, 0x84),

	MFP_ADDR(GPIO124, 0xd0),
	MFP_ADDR(PWM4, 0xd8),
	MFP_ADDR(CLK_REQ, 0xcc),
	MFP_ADDR(VCXOREQ,0xd4),

 	MFP_ADDR(MMC1_DAT3,0x94),
    	MFP_ADDR(MMC1_DAT2,0x98),
    	MFP_ADDR(MMC1_DAT1,0x9c),
    	MFP_ADDR(MMC1_DAT0,0xa0),
    	MFP_ADDR(MMC1_CMD, 0xa4),
	MFP_ADDR(MMC1_CLK, 0xa8),
    	MFP_ADDR(MMC1_CD,  0xac),
    	MFP_ADDR(MMC1_WP,  0xb0),
    
    	MFP_ADDR(USIM_UCLK,  0x320),
    	MFP_ADDR(USIM_UIO,  0x324),
    	MFP_ADDR(USIM_nURST,  0x328),
	MFP_ADDR_X(TDS_DIO0, TDS_TXREV, 0x2B4),
	MFP_ADDR(RF_CONT_4, 0x274),
	MFP_ADDR(PA_MODE, 0x270),
	MFP_ADDR_END,
};

void emei_mfp_config(void)
{
	return;

	APBClock->AIB_CLK_RST = AIB_CLK_RST_FNCLK | AIB_CLK_RST_APBCLK; //enable MFP clock
	mfp_init_base(MFPR_BASE);
	mfp_init_addr(emei_addr_map);
	mfp_config(ARRAY_AND_SIZE(nezha3_v10_pin_config));
}

void emei_mfp_post_config(void)
{
	uint32_t value;
	return;

	if(board_is_nza3_dkb_v20a() || board_is_nza3_dkb_v20b()) {
		mfp_config(ARRAY_AND_SIZE(nezha3_v20av10_diff_pin_config));

		//GPIO37~GPIO47 used as normal pinmux function
		value = BU_REG_READ(0xd428284c);
		value |= BIT_31;
		BU_REG_WRITE(0xd428284c, value);
	}
	if(board_is_nza3_dkb_v20b()) {
		mfp_config(ARRAY_AND_SIZE(nezha3_v20bv20a_diff_pin_config));

		//GPIO37~GPIO47 used as emmc5.0 pads
		value = BU_REG_READ(0xd428284c);
		value &= ~BIT_31;
		BU_REG_WRITE(0xd428284c, value);
	}
}	

