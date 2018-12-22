#include "bbu.h"
#include <string.h>

/*................... get AO's pointer according to its name................*/
struct AoInfoEntry{char *ao_name;   void *ao_ptr;};
void *ao_info_translate(void *ao_name, int n2p) /* n2p: 1 - name to pointer */
                                                /* n2p: 0 - pointer to name */
{
   struct AoInfoEntry ao_info_tbl[] = 
   {
      {"cli",         AO_Cli},
      {"common",      AO_COMMON},
      {"timer",       AO_Timer},
      {"i2c",         AO_I2C},
#ifndef   CONFIG_MMT
//      {"usb",         AO_USB},

#endif
      {"pmu",         AO_pmu},
      {"stress",      AO_STRESS},
      {"sched",       AO_SCHED},
#if !defined(CONFIG_DBG)
      {"console",      AO_Console},
#endif
   };
   int i;
   int len = sizeof(ao_info_tbl)/sizeof(ao_info_tbl[0]);
   if(n2p){
      if(ao_name == 0)
         bbu_printf("\n\r **** AO  **  CORE ****\n\r");
      for(i = 0; i < len; i++)
      {
         if(ao_name == 0){
            bbu_printf("  %7s      %d\n\r",ao_info_tbl[i].ao_name, 0);
         }
         else if(strcmp(ao_name, (const char*)ao_info_tbl[i].ao_name) == 0)
            return ao_info_tbl[i].ao_ptr;
      }
      return (void *)0;
   }else{
      for(i = 0; i < len; i++)
      {
         if((QActive *)ao_name == ao_info_tbl[i].ao_ptr)
            return ao_info_tbl[i].ao_name;
      }
      #ifdef CONFIG_CPU_AO
      if(((QActive *)ao_name)->prio >(QF_MAX_ACTIVE-CORE_NUM)){
         static char *name = "cpu0";
         name[3] = '0'+QF_MAX_ACTIVE-((QActive *)ao_name)->prio;
         return (void *)name;
      }
      #endif
      return (void *)"unknown";
   }
}
