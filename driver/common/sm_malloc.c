#include "bbu.h"
#include "shared_mem.h"

Q_DEFINE_THIS_FILE

QMPool  * const SmPool0_  = (QMPool *)SM_POOL0_CTRL;


void sm_malloc_init(void)
{
   QMPool_init(SmPool0_, (void *)SM_POOL0_BUF, SM_POOL0_SIZE,
               SM_POOL0_BLOCK_LEN);
}

void *sm_malloc(int size)
{
   void *block = (void *)0;

   if(size > SM_POOL0_BLOCK_LEN){
      bbu_printf(DBG_MSG "The size %d exceed the MAX %d\n\r", size, SM_POOL0_BLOCK_LEN);
      return block;
   }

   block = (void *)QMPool_get(SmPool0_, 1);
   return block;
}

void sm_free(void *block)
{
   QMPool_put(SmPool0_, block);
}

