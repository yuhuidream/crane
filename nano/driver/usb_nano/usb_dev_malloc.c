
//#include <stdio.h>
#include "usb_mv8618malloc.h"

#include "set_mmu.h"
#if 0
struct map
{
    int             m_size;
    unsigned char   *m_addr;
};

struct map free_map[BLOCK_NUM];
struct map used_map[BLOCK_NUM];
int used_cnt;
#endif
extern void *bbu_vm_malloc(uint32_t len, unsigned int mem_flags);
//static void *     HEAP_START_ADDR ;
uint_8          *NONE_Cache_ADDR   ;


extern void usb_disk_free(void);
/* initialize the block array. set the 1st block with  */
/* heap size and start address. its only non-zero size */
/* block in the beginning.                             */

uint_8    *usbNoneCacheAddr  =  (uint_8 *)(0xD1000000);
#define   none_cache_length 0x1000
void
USB_malloc_init()
{

    NONE_Cache_ADDR  = (uint_8 *) (usbNoneCacheAddr + none_cache_length);    //bbu_vm_malloc(USB_ALL_BUF2_LEN,NON_CACHEABLE);

    //     if(NONE_Cache_ADDR==NULL)
    //   {
    //  bbu_printf("malloc usb2_buffer error\r\n");
    // return;
    //   }
    //else
    //   bbu_printf("USB2 test buffer is 0x%x\r\n",NONE_Cache_ADDR);





}

extern int usb_service_type;
void USB_vm_free(void)
{





    usb_free_serialBuff();


}


