#include "predefines.h"
#include "gpio_dec.h"
#include "utils.h"
#include <string.h>
#include "gpio_dec.h"
#include "cache-test.h"
#include "bbu.h"
#include "CIU.h"
#include "mfp.h"
#include "mfp-emei.h"
#include "common_tests.h"
#include "pmic.h"

unsigned long test_pin_config[1];

#ifdef TEST_LPM_ARRAY
#define NR_MAX_PIN 160
static unsigned long run_pin_config[NR_MAX_PIN] = {0};
static unsigned long lpm_pin_config[NR_MAX_PIN] = {0};
/*for test only*/
static unsigned long emei_test_pin_config[] = {
	KP_MKIN0,
	KP_MKOUT0,
	KP_MKIN1,
	KP_MKOUT1,
	KP_MKIN2,
	KP_MKOUT2,
	KP_MKIN3 ,
	WIB_PDN	,
	WIB_WLAN,
	WIB_BT	,
	RF_DCDC_EN,
	WIB_RESET,
	TORCH_EN,
	CMMB_IRQ,
	PROX_IRQ,
	NFC_EN	,
	TP_RESET,
	TP_INT	,
	CMMB_EN	,
	CMMB_RESET,
	FLASH_EN,
	I2S_BITCLK,
	I2S_SYNC,	
	I2S_DATA_OUT,
	I2S_DATA_IN,
	PCM_CLK	,
	PCM_SYNC,
	PCM_TXD	,
	PCM_RXD ,
	CHARGER_IND1,
	CHARGER_IND2,
	CHARGER_EN,
	LCD_PWM	  ,
	CMMB_SPI_CLK,	
	CMMB_SPI_CS,	
	CMMB_SPI_DOUT,	
	CMMB_SPI_DIN ,
	WLAN_DAT3,
	WLAN_DAT2,
	WLAN_DAT1,
	WLAN_DAT0,
	WLAN_CMD,
	WLAN_CLK,
	DVC1,
	DVC2,
	GPS_UART_RXD,
	GPS_UART_TXD,
	AP_UART_RXD,
	AP_UART_TXD,
	BARA_INT2,
	BARA_INT1,
	CP_UART_RXD,
	CP_UART_TXD,
	CI2C_SCL,
	CI2C_SDA,
	CCIC_1,
	CCIC_2,
	CCIC_3,
	CCIC_4,
	CCIC_5,
	CCIC_6,
	CCIC_7,
	CCIC_8,
	CCIC_9,
	CCIC_10,
	CCIC_11,
	CCIC_12,
	CAM_STROBE,
	CAM_PD_MAIN,
	CAM_RST_MAIN,
	CAM_PD_SUB,
	CAM_RST_SUB,
	GPS_RESET_N,
	GPS_SLK_EN,
	GPS_SEN_EN,
	CI2C_SCL2,
	CI2C_SDA2,
	GPS_ECLK,
	CMMB_26MHZ,
	GYRO_INT,
	COMPASS_INT,
	G_INT,
	NFC_IRQ ,
	MOTION_INT,
	GPS_ON_OFF,
	GPS_PPS,
	PRESURE_DRDY ,
	CODEC_INT,
	GPS_TIMER_SYNC,
	TEST_POINT,	
	LCD_RESET_N,	
	LED_B_CTRL,	
	LED_R_CTRL,	
	LED_G_CTRL,	
	GPS_LDO_EN,
	VCM_PWDN,
	MMC_DATA0,
	MMC_DATA1,
	MMC_DATA2,
	MMC_DATA3,
	MMC_DATA4,
	MMC_DATA5,
	MMC_DATA6,
	MMC_DATA7,
	MMC_SCLK,
	MMC_CMD,
	VCTCXO_OUT,
    SD_DAT3,
    SD_DAT2,
    SD_DAT1,
    SD_DAT0,
    SD_CMD ,
    SD_CLK ,
    SD_CD  ,
    SD_WP  ,
};
#endif
void mfp_tests()
{
#ifdef MFP_FUNC_TEST
    unsigned long value_out, value_in;	
    uint32_t value_out32;
    value_in = 0;
   
    test_pin_config[1] = VCTCXO_OUT;
   
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("\n\rread VCTCXO_OUT default is %x\n\r",value_out);
    
    value_out32 = mfp_config_to_mfpr(VCTCXO_OUT);
    bbu_printf("VCTCXO_OUT convert value is %x\n\r",value_out32);

    mfpr_pull_config(MFP_PIN(VCTCXO_OUT),MFP_PULL_LOW);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT pull low is %x\n\r",value_out);   

    mfpr_pull_config(MFP_PIN(VCTCXO_OUT),MFP_PULL_HIGH);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT pull high is %x\n\r",value_out);  
 
    mfpr_pull_config(MFP_PIN(VCTCXO_OUT),MFP_PULL_BOTH);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT pull both is %x\n\r",value_out);   

    mfpr_pull_config(MFP_PIN(VCTCXO_OUT),MFP_PULL_FLOAT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT pull float is %x\n\r",value_out);   

    mfpr_pull_config(MFP_PIN(VCTCXO_OUT),MFP_PULL_NONE);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT pull none is %x\n\r",value_out);  

    mfpr_config_pin(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT default config is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_SLEEP_DEFAULT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT sleep default is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_DEFAULT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT sleep  is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_SLEEP_NOSLEEP);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT no sleep is %x\n\r",value_out);  

    mfpr_config_pin(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT default config is %x\n\r",value_out);  

    lpm_mfpr_edge_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_EDGE_NONE);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT edge none is %x\n\r",value_out);  

    lpm_mfpr_edge_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_EDGE_RISE);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT edge rise is %x\n\r",value_out);  

    lpm_mfpr_edge_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_EDGE_FALL);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT edge fall is %x\n\r",value_out);  

    lpm_mfpr_edge_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_EDGE_BOTH);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT edge both is %x\n\r",value_out);  

    mfpr_config_pin(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT default config is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_DEFAULT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm default is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_DRIVE_LOW);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm drive low is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_DRIVE_HIGH);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm drive high is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_PULL_LOW);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm pull low is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_PULL_HIGH);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm pull high is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_FLOAT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm float is %x\n\r",value_out); 

    mfpr_config_pin(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT default config is %x\n\r",value_out);  




    mfpr_pull_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_PULL_LOW);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT pull low is %x\n\r",value_out);   

    mfpr_pull_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_PULL_HIGH);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT pull high is %x\n\r",value_out);  
 
    mfpr_pull_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_PULL_BOTH);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT pull both is %x\n\r",value_out);   

    mfpr_pull_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_PULL_FLOAT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT pull float is %x\n\r",value_out);   

    mfpr_pull_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_PULL_NONE);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT pull none is %x\n\r",value_out);  

    mfpr_config_pin(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT default config is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_SLEEP_DEFAULT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT sleep default is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_LPM_DEFAULT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT sleep  is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_SLEEP_NOSLEEP);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT no sleep is %x\n\r",value_out);  

    mfpr_config_pin(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT default config is %x\n\r",value_out);  

    lpm_mfpr_edge_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_LPM_EDGE_NONE);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT edge none is %x\n\r",value_out);  

    lpm_mfpr_edge_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_LPM_EDGE_RISE);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT edge rise is %x\n\r",value_out);  

    lpm_mfpr_edge_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_LPM_EDGE_FALL);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT edge fall is %x\n\r",value_out);  

    lpm_mfpr_edge_config(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT|MFP_LPM_EDGE_BOTH);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT edge both is %x\n\r",value_out);  

    mfpr_config_pin(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT default config is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_DEFAULT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm default is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_DRIVE_LOW);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm drive low is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_DRIVE_HIGH);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm drive high is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_PULL_LOW);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm pull low is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_PULL_HIGH);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm pull high is %x\n\r",value_out);  

    mfpr_sleep_and_state_config(MFP_PIN(VCTCXO_OUT),MFP_LPM_FLOAT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT lpm float is %x\n\r",value_out); 



    mfpr_config_pin(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT default config is %x\n\r",value_out);  

    mfpr_function_config(MFP_PIN(VCTCXO_OUT),MFP_AF0);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT config function 0 is %x\n\r",value_out);  

    mfpr_function_config(MFP_PIN(VCTCXO_OUT),MFP_AF1);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT config function 1 is %x\n\r",value_out);  

    mfpr_function_config(MFP_PIN(VCTCXO_OUT),MFP_AF2);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT config function 2 is %x\n\r",value_out);  

    mfpr_function_config(MFP_PIN(VCTCXO_OUT),MFP_AF3);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT config function 3 is %x\n\r",value_out);  

    mfpr_function_config(MFP_PIN(VCTCXO_OUT),MFP_AF4);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT config function 4 is %x\n\r",value_out);  

    mfpr_function_config(MFP_PIN(VCTCXO_OUT),MFP_AF5);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT config function 5 is %x\n\r",value_out);  

    mfpr_function_config(MFP_PIN(VCTCXO_OUT),MFP_AF6);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT config function 6 is %x\n\r",value_out);  

    mfpr_function_config(MFP_PIN(VCTCXO_OUT),MFP_AF7);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT config function 7 is %x\n\r",value_out);  

    mfpr_config_pin(MFP_PIN(VCTCXO_OUT),VCTCXO_OUT);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT default is %x\n\r",value_out);  

    mfpr_drive_strength_config(MFP_PIN(VCTCXO_OUT),MFP_VERY_SLOW);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT MFP_VERY_SLOW is %x\n\r",value_out);  

    mfpr_drive_strength_config(MFP_PIN(VCTCXO_OUT),MFP_SLOW);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT MFP_VERY_SLOW 1 is %x\n\r",value_out);  

    mfpr_drive_strength_config(MFP_PIN(VCTCXO_OUT),MFP_MEDIUM);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT MFP_MEDIUM is %x\n\r",value_out);  

    mfpr_drive_strength_config(MFP_PIN(VCTCXO_OUT),MFP_FAST);
    value_out = mfp_read(MFP_PIN(VCTCXO_OUT));
    bbu_printf("read VCTCXO_OUT MFP_FAST is %x\n\r",value_out);  
    
    value_out32 = mfp_config_to_mfpr(MFP_AF0);
    bbu_printf("MFP_AF0 convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_AF1);
    bbu_printf("MFP_AF1 convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_AF2);
    bbu_printf("MFP_AF2 convert value is %x\n\r",value_out32);
   
    value_out32 = mfp_config_to_mfpr(MFP_AF3);
    bbu_printf("MFP_AF3 convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_AF4);
    bbu_printf("MFP_AF4 convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_AF5);
    bbu_printf("MFP_AF5 convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_AF6);
    bbu_printf("MFP_AF6 convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_AF7);
    bbu_printf("MFP_AF7 convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_VERY_SLOW);
    bbu_printf("MFP_VERY_SLOW convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_SLOW);
    bbu_printf("MFP_SLOW convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_MEDIUM);
    bbu_printf("MFP_MEDIUM convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_FAST);
    bbu_printf("MFP_FAST convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_LPM_EDGE_NONE);
    bbu_printf("MFP_LPM_EDGE_NONE convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_LPM_EDGE_RISE);
    bbu_printf("MFP_LPM_EDGE_RISE convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_LPM_EDGE_FALL);
    bbu_printf("MFP_LPM_EDGE_FALL convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_LPM_EDGE_BOTH);
    bbu_printf("MFP_LPM_EDGE_BOTH convert value is %x\n\r",value_out32);


    value_out32 = mfp_config_to_mfpr(MFP_LPM_DEFAULT);
    bbu_printf("MFP_LPM_DEFAULT convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_LPM_DRIVE_HIGH);
    bbu_printf("MFP_LPM_DRIVE_HIGH convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_LPM_DRIVE_LOW);
    bbu_printf("MFP_LPM_DRIVE_LOW convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_LPM_PULL_LOW);
    bbu_printf("MFP_LPM_PULL_LOW convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_LPM_PULL_HIGH);
    bbu_printf("MFP_LPM_PULL_HIGH convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_LPM_FLOAT);
    bbu_printf("MFP_LPM_FLOAT convert value is %x\n\r",value_out32);


    value_out32 = mfp_config_to_mfpr(MFP_PULL_NONE);
    bbu_printf("MFP_PULL_NONE convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_PULL_LOW);
    bbu_printf("MFP_PULL_LOW convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_PULL_HIGH);
    bbu_printf("MFP_PULL_HIGH convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_PULL_BOTH);
    bbu_printf("MFP_PULL_BOTH convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_SLEEP_DEFAULT);
    bbu_printf("MFP_SLEEP_DEFAULT convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_LPM_DEFAULT);
    bbu_printf("MFP_LPM_DEFAULT convert value is %x\n\r",value_out32);

    value_out32 = mfp_config_to_mfpr(MFP_SLEEP_NOSLEEP);
    bbu_printf("MFP_SLEEP_NOSLEEP convert value is %x\n\r",value_out32);

 #ifdef TEST_LPM_ARRAY    /*test only*/
    unsigned int i;
    for(i = 0; i < ARRAY_SIZE(emei_test_pin_config); i++)
    {
	 run_pin_config[i] = mfp_read(MFP_PIN(emei_test_pin_config[i]));
    }

    mfp_config_lpm_array(ARRAY_AND_SIZE(emei_test_pin_config));

    for(i = 0; i < ARRAY_SIZE(emei_test_pin_config); i++)
    {
	 lpm_pin_config[i] = mfp_read(MFP_PIN(emei_test_pin_config[i]));
    }
 
    for(i = 0; i < ARRAY_SIZE(emei_test_pin_config); i++)
    {
	
	 if(lpm_pin_config[i] != lpm_pin_config[i])
             break;
    }
    if(i == ARRAY_SIZE(emei_test_pin_config))
	bbu_printf("\n\rmfp_config_lpm_array function test ok\n\r");
    else 
  	bbu_printf("\n\rmfp_config_lpm_array function test error\n\r");

#endif 
#endif

}


