#ifndef _H_SET_MMU_H_
#define _H_SET_MMU_H_
#include "mem_map.h"
#include "mpu.h"

#define CACHEABLE             (1)
#define NON_CACHEABLE         (0)
#define NON_CACHE_BUFFER      (0)

typedef enum{
   STRONGLY_ORDERED,
   SHAREABLE_DEVICE,
   OI_WT_NO_WA,  /*Outer&Inner write through, no write allocate*/
   OI_WB_NO_WA,  /*Outer&Inner write back, no write allocate*/
   OI_NON_CACHEABLE,  /*Outer&Inner Non cacheable*/
   Reserved1,
   IMPLEMENTATION_DEFINED1,
   OI_WB_WA,   /*Outer&Inner write back, write allocate*/
   NON_SHAREABLE_DEVICE,
   Reserved2,
   Reserved3
}Memory_Types;


#endif
