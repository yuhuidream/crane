#include "predefines.h"
#include "pmic.h"

static void dummy_ldo_cfg(int on_off, LDO_CHANNEL channel, LDO_VOL mv, LDO_VBUCK_OPT option)
{
   (void)on_off;
   (void)channel;
   (void)mv;
   (void)option;
}

static void dummy_vbuck_cfg(int on_off, VBUCK_CHANNEL channel, VBUCK_VOL mv, LDO_VBUCK_OPT option)
{
   (void)on_off;
   (void)channel;
   (void)mv;
   (void)option;
}

static int dummy_vbuck_get_vol(VBUCK_CHANNEL channel, LDO_VBUCK_OPT option)
{
   (void)channel;
   (void)option;
   return -1;
}

static int dummy_ldo_get_vol(LDO_CHANNEL channel, LDO_VBUCK_OPT option)
{
   (void)channel;
   (void)option;
   return -1;
}

static int dummy_is_vbuck_on(VBUCK_CHANNEL channel)
{
   (void)channel;
   return -1;
}

static int dummy_is_ldo_on(LDO_CHANNEL channel)
{
   (void)channel;
   return -1;
}

struct pmic_callback dummy_callback = {
	 .__pmic_ldo_cfg = dummy_ldo_cfg,
	 .__pmic_vbuck_cfg = dummy_vbuck_cfg,
	 .__pmic_vbuck_get_vol = dummy_vbuck_get_vol,
	 .__pmic_ldo_get_vol =  dummy_ldo_get_vol,
	 .__pmic_is_vbuck_on = dummy_is_vbuck_on,
	 .__pmic_is_ldo_on  = dummy_is_ldo_on,
};
