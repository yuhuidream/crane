#ifndef GLOBAL_VARIABLE
#define EXTERN extern
#else
#define EXTERN
#endif
#include "usb_devapi.h"

#include "set_mmu.h"
#ifndef NULL
#define NULL ((void*)0)
#endif
#include "bbu_malloc.h"

#ifndef _MALLOC_H_

#define _MALLOC_H_

//#define USB_ALL_BUF_LEN             (0x00200000)

#define USB_ALL_BUF2_LEN            (0x1c00)
#define USB_BUFFERSTEP                (0x400)
#define USB_SER_BUFFER                 (64)
//USB_BUF_ST// 0x8a000000                 //0x0800000  need be changed
#define     HEAP_SIZE       0x00010000
#define     BLOCK_NUM       100
// USB_BUF2_ST//(USB_BUF_ST+USB_BUF_LEN-0x00200000)
#define byte unsigned char




void malloc_init(void);
void m_free( byte *a );
unsigned char *m_malloc( int size);

unsigned char *MRVL_USB_memalloc( int size);

void MRVL_USB_memfree( unsigned char *a);

#endif


