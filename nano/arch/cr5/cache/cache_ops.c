#include "predefines.h"
#include "cache_ops.h"
#include "cp15.h"

int is_i_cache_enabled(void)
{
   uint32_t val;
   cp15_reg_read(c0, c0, 1, val);
   if(val & (0x1 << 12))
      return 1;
   else
      return 0;
}

int is_d_cache_enabled(void)
{
   uint32_t val;
   cp15_reg_read(c0, c0, 1, val);
   if(val & (0x1 << 2))
      return 1;
   else
      return 0;
}
