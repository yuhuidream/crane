#include "bbu.h"
#include "pmic.h"
#include "ustica.c"

int ustica_available  = 0;
int pmic_type = DUMMY_PMIC;

static void (* __pmic_ldo_cfg)(int, LDO_CHANNEL, LDO_VOL, LDO_VBUCK_OPT);
static void (* __pmic_vbuck_cfg)(int, VBUCK_CHANNEL, VBUCK_VOL, LDO_VBUCK_OPT);
static int  (* __pmic_vbuck_get_vol)(VBUCK_CHANNEL, LDO_VBUCK_OPT);
static int  (* __pmic_ldo_get_vol)(LDO_CHANNEL, LDO_VBUCK_OPT);
static int  (* __pmic_is_vbuck_on)(VBUCK_CHANNEL);
static int  (* __pmic_is_ldo_on)(LDO_CHANNEL);

void pmic_init(void)
{

#ifdef PMIC_DIALOG
   pmic_type = DIALOG;
   bbu_printf("PMIC: Dialog\n\r");
#endif

#ifdef PMIC_PROCIDA
   pmic_type = PROCIDA;
   bbu_printf("PMIC: Procida\n\r");
#endif

#ifdef PMIC_DUMMY
   pmic_type = DUMMY_PMIC;
   bbu_printf("PMIC: Dummy\n\r");
#endif

#ifdef PMIC_USTICA
   uint8_t reg;
   ustica_available = 1;
   pmic_type = USTICA;
   ustica_base_read(0x00, &reg);
   bbu_printf("PMIC ID: 0x%X\n\r", reg);
#endif

   return;
}

int pmic_is_ustica(void)
{
   return (pmic_type == USTICA);
}

int pmic_is_dialog(void)
{
   return (pmic_type == DIALOG);
}

int pmic_is_dummy(void)
{
   return (pmic_type == DUMMY_PMIC);
}

