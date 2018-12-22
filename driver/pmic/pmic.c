#include "bbu.h"
#include "pmic.h"
#include "ustica.h"
#include "i2c_dec.h"

int procida_available = 0;
int ustica_available  = 0;
int pmic_type = DUMMY_PMIC;
uint32_t pmic_stepping = 0x0;
static int pmic_vendor = DUMMY_VENDOR;

static void (* __pmic_ldo_cfg)(int, LDO_CHANNEL, LDO_VOL, LDO_VBUCK_OPT);
static void (* __pmic_vbuck_cfg)(int, VBUCK_CHANNEL, VBUCK_VOL, LDO_VBUCK_OPT);
static int  (* __pmic_vbuck_get_vol)(VBUCK_CHANNEL, LDO_VBUCK_OPT);
static int  (* __pmic_ldo_get_vol)(LDO_CHANNEL, LDO_VBUCK_OPT);
static int  (* __pmic_is_vbuck_on)(VBUCK_CHANNEL);
static int  (* __pmic_is_ldo_on)(LDO_CHANNEL);

struct pmic_int_desc *pmic_irq_array = NULL;
extern struct pmic_callback procida_callback;
extern struct pmic_callback ustica_callback;
extern struct pmic_callback dummy_callback;

void pmic_init(void)
{
   	uint8_t version = 0;
   	int ret;
   	__pmic_ldo_cfg = 0;
   	__pmic_vbuck_cfg = 0;
   	__pmic_vbuck_get_vol = 0;
   	__pmic_ldo_get_vol = 0;
   	__pmic_is_vbuck_on = 0;
   	__pmic_is_ldo_on = 0;

   	/* Read Procida version and check if there is Procida on this board */
   	ret = i2c_read_addr8_byte(PM8XX_I2C, PM8XX_BASE_ADDRESS, 
                              0, &version, ICR_MODE_STD);
   	if(ret == I2C_TIMEOUT){
      		ustica_available = 0;
		procida_available = 0;
   	}
   	else{
		pmic_vendor = PMIC_MARVELL;

		if(version >= 0x69 && version <= 0x6f) {
		      	ustica_available = 1;
		      	pmic_type = USTICA;
		      	__pmic_ldo_cfg       = ustica_callback.__pmic_ldo_cfg;
		      	__pmic_vbuck_cfg     = ustica_callback.__pmic_vbuck_cfg;
		      	__pmic_vbuck_get_vol = ustica_callback.__pmic_vbuck_get_vol;
		      	__pmic_ldo_get_vol   = ustica_callback.__pmic_ldo_get_vol;
		      	__pmic_is_ldo_on     = ustica_callback.__pmic_is_ldo_on;
		      	__pmic_is_vbuck_on   = ustica_callback.__pmic_is_vbuck_on;
		      	bbu_printf("PMIC: Ustica, ID: 0x%X\n\r", version);
			if(cpu_is_ca7())
				ustica_dev_init();
		} else {
		      	procida_available = 1;
		      	pmic_type = PROCIDA;
		      	__pmic_ldo_cfg       = procida_callback.__pmic_ldo_cfg;
		      	__pmic_vbuck_cfg     = procida_callback.__pmic_vbuck_cfg;
		      	__pmic_vbuck_get_vol = procida_callback.__pmic_vbuck_get_vol;
		      	__pmic_ldo_get_vol   = procida_callback.__pmic_ldo_get_vol;
		      	__pmic_is_ldo_on     = procida_callback.__pmic_is_ldo_on;
		      	__pmic_is_vbuck_on   = procida_callback.__pmic_is_vbuck_on;
		      	bbu_printf("PMIC: Procida, ID: 0x%X\n\r", version);
			if(cpu_is_ca7())
				procida_dev_init();
		}
      		return;
   	}

   	pmic_type = DUMMY_PMIC;
   	__pmic_ldo_cfg       = dummy_callback.__pmic_ldo_cfg;
	__pmic_vbuck_cfg     = dummy_callback.__pmic_vbuck_cfg;
	__pmic_vbuck_get_vol = dummy_callback.__pmic_vbuck_get_vol;
	__pmic_ldo_get_vol   = dummy_callback.__pmic_ldo_get_vol;
	__pmic_is_ldo_on     = dummy_callback.__pmic_is_ldo_on;
	__pmic_is_vbuck_on   = dummy_callback.__pmic_is_vbuck_on;
	bbu_printf("PMIC: Dummy\n\r");
	return;
}

void pmic_ldo_cfg(int on_off, LDO_CHANNEL channel, LDO_VOL mv, LDO_VBUCK_OPT option)
{
   	if(__pmic_ldo_cfg) {
      		__pmic_ldo_cfg(on_off, channel, mv, option);
   	}else{
      		bbu_printf("PMIC Error! %s %d\n\r", __FUNCTION__, __LINE__);
   	}
}

void pmic_vbuck_cfg(int on_off, VBUCK_CHANNEL channel, VBUCK_VOL mv, LDO_VBUCK_OPT option)
{
   	if(__pmic_vbuck_cfg) {
      		__pmic_vbuck_cfg(on_off, channel, mv, option);
  	} else {
      		bbu_printf("PMIC Error! %s %d\n\r", __FUNCTION__, __LINE__);
   	}
}

int pmic_vbuck_get_vol(VBUCK_CHANNEL channel, LDO_VBUCK_OPT option)
{
   	if(__pmic_vbuck_get_vol){
      		return __pmic_vbuck_get_vol(channel, option);
   	} else {
      		bbu_printf("PMIC Error! %s %d\n\r", __FUNCTION__, __LINE__);
      		return -1;
   	}
}

int pmic_ldo_get_vol(LDO_CHANNEL channel, LDO_VBUCK_OPT option)
{
   	if(__pmic_ldo_get_vol){
      		return __pmic_ldo_get_vol(channel, option);
   	} else {
      		bbu_printf("PMIC Error! %s %d\n\r", __FUNCTION__, __LINE__);
      		return -1;
   	}
}

int pmic_is_vbuck_on(VBUCK_CHANNEL channel)
{
   	if(__pmic_is_vbuck_on){
      		return __pmic_is_vbuck_on(channel);
   	} else {
      		bbu_printf("PMIC Error! %s %d\n\r", __FUNCTION__, __LINE__);
      		return -1;
   	}
}

int pmic_is_ldo_on(LDO_CHANNEL channel)
{
   	if(__pmic_is_ldo_on){
      		return __pmic_is_ldo_on(channel);
   	} else {
      		bbu_printf("PMIC Error! %s %d\n\r", __FUNCTION__, __LINE__);
      		return -1;
   	}
}

/*
*pmic interrupt related func
*/		
void pmic_int_enable(e_pmic_int pmic_int_idx)
{
	uint8_t value;

	if(pmic_is_pm8xx()){
	   	if(pmic_int_idx >= MAX_PMIC_INT)
	   	{
	   		bbu_printf("pmic int_enable failed,error pmic int num\n\r");
	   		return;
	   	}
		if(pmic_irq_array == NULL)
	      	{
		   	bbu_printf("%s:irq array is NULL\n\r",__func__);
		   	return;
	      	}
	   	pm8xx_base_read(pmic_irq_array[pmic_int_idx].en_reg_offset,&value);
	   	value |=pmic_irq_array[pmic_int_idx].pmic_int_mask;
	   	pm8xx_base_write(pmic_irq_array[pmic_int_idx].en_reg_offset,value);
		pmic_irq_array[pmic_int_idx].irq_valid = 1;
   	}else{
        	bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   	}
}

void pmic_int_disable(e_pmic_int pmic_int_idx)
{
	uint8_t value;
	if(pmic_is_pm8xx()){
   	if(pmic_int_idx >= MAX_PMIC_INT)
   	{
   		bbu_printf("pmic int_disable failed,error pmic int num\n\r");
   		return;
   	}
	if(pmic_irq_array == NULL)
      	{
	   	bbu_printf("%s:irq array is NULL\n\r",__func__);
	   	return;
      	}   	
   	pm8xx_base_read(pmic_irq_array[pmic_int_idx].en_reg_offset,&value);
   	value &= ~(pmic_irq_array[pmic_int_idx].pmic_int_mask);
   	pm8xx_base_write(pmic_irq_array[pmic_int_idx].en_reg_offset,value);
	pmic_irq_array[pmic_int_idx].irq_valid = 0;
   }else{
      	bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   }
}

void pmic_isr_connect(e_pmic_int pmic_int_idx, void (*ISR_Handle)() )
{
   	if(pmic_is_pm8xx()){
      		if(NULL == ISR_Handle) 
         		return;
      		if(pmic_irq_array == NULL)
      		{
	   		bbu_printf("%s:no irq handler\n\r",__func__);
	   		return;
      		}
      		/* assign the user defined function*/
      		pmic_irq_array[pmic_int_idx].pmic_isr_handler = (void *)ISR_Handle;
   	} else {
      		bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   	}
}

void pmic_isr_disconnect(e_pmic_int pmic_int_idx, void (*ISR_Handle)() )
{
   	if(pmic_is_pm8xx()){
      		if(NULL == ISR_Handle) 
         		return;
      	if(pmic_irq_array == NULL)
      	{
	   	bbu_printf("%s:no irq handler\n\r",__func__);
	   	return;
      	}
      	/* assign the user defined function*/
      	if( pmic_irq_array[pmic_int_idx].pmic_isr_handler == ISR_Handle)
      		pmic_irq_array[pmic_int_idx].pmic_isr_handler=NULL;
   	} else {
      		bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   	}
}

void pmic_clear_int(e_pmic_int pmic_int_idx)
{
	uint8_t value;	
	if(pmic_is_pm8xx()){
	      	if(pmic_irq_array == NULL)
		{
		      bbu_printf("%s:no irq handler\n\r",__func__);
		      return;
		}

		pm8xx_base_read(pmic_irq_array[pmic_int_idx].stat_reg_offset,&value);
	    	//pm8xx_base_write(pmic_irq_array[pmic_int_idx].stat_reg_offset,value);
   	} else {
      		bbu_printf(DBG_MSG "%s is dummy as PMIC isn't available\n\r", __FUNCTION__);
   	}
}
		
void pmic_irq_init(void)
{  
    	if(pmic_is_ustica())
		ustica_irq_init();
	if(pmic_is_procida())
		procida_irq_init();
}

int pmic_is_ustica(void)
{
   	return (pmic_type == USTICA);
}

int pmic_is_dummy(void)
{
   	return (pmic_type == DUMMY_PMIC);
}

int pmic_is_procida(void)
{
   	return (pmic_type == PROCIDA);
}

int pmic_is_pm8xx(void)
{
   	return (pmic_vendor == PMIC_MARVELL);
}
