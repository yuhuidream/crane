#include "bbu.h"
#include "bbu_malloc.h"
#include "set_mmu.h"
#include <string.h>

#define MAX_MEM_TO_FLUSH 1<<24

void flushall(int yes)
{
   static char *buffer = NULL;
   static char val = 1;

   if (yes)
   {
      if (buffer == NULL)
      {
         buffer = (char *)bbu_vm_malloc(MAX_MEM_TO_FLUSH*sizeof(char), CACHEABLE);
         if (buffer == NULL)
         {
            bbu_printf("flushall.c: malloc of %d bytes failed.\n\r",MAX_MEM_TO_FLUSH);
            return;
         }
      }
      memset(buffer,val++,MAX_MEM_TO_FLUSH);
   }
   else
   {
      if (buffer)
      {
         bbu_vm_free(buffer);
         buffer = NULL;
      }
   }
}

