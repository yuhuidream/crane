#ifndef GLOBAL_VARIABLE
#define EXTERN extern
#else
#define EXTERN
#endif



#ifndef NULL
#define NULL ((void*)0)
#endif


#ifndef _MALLOC_H_

#define _MALLOC_H_

#define     HEAP_START_ADDR 0x0800000
#define     HEAP_SIZE       0x00010000
#define     BLOCK_NUM       100
#define byte unsigned char
//EXTERN unsigned char *    HEAP_START_ADDR;
//EXTERN unsigned int   HEAP_SIZE;


void malloc_init(void);
void m_free( byte *a );
unsigned char *m_malloc( int size);

unsigned char *MRVL_USB_memalloc( int size);

void MRVL_USB_memfree( unsigned char *a);

#endif


