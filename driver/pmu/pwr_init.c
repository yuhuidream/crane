#include "predefines.h"
#include "gpio_dec.h"
#include "utils.h"
#include <string.h>
#include "gpio_dec.h"
#include "ddr_dec.h"
#include "gic_dec.h"
#include "bbu.h"
#include "CIU.h"
#include "mfp.h"
#include "mfp-emei.h"
#include "common_tests.h"
#include "pmic.h"
#include "PMUA.h"
#include "APBCLK.h"
#include "timer_dec.h"
#include "CIU.h"
#include "MCU.h"

#ifdef SV_MEM_TEST
extern void sv_mem_test_datapattern_init(void);
#endif 
#ifdef SIMPLE_DATA_PATTEN
int simple_data_test(int bCheck);
#endif

void fab_gating_init(void)
{	
    uint32_t reg;
    
    reg = CIU->MC_CONF;
    reg |= 0x0C06FF00;
    CIU->MC_CONF = reg;
}
	
