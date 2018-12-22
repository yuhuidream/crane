#ifndef __GEU_FUSE_H
#define __GEU_FUSE_H

//input : DMA ------0 : non-DMA; 1 : DMA - non_descriptor; 2 : DMA -descriptor
//          mode------1 : ECB mode; 2: CBC mode
//use  geudiag for test
int diag_geu(int DMA, char mode);

/*
 *	Usage: aes_test
 *  1. Load pattern1.PT.txt at 0x100000 after clean pPT memory.
 *  2. Continue run the code to finish. The size is displayed by UART console.
 *  3. Dump memory at 0x1040000 to a file named memory.CT.txt for
 *     the size displayed on console.
 *  4. Diff pattern1.CT.txt and memory.CT.txt. If no difference, pass.
 *     Otherwise, failed.
 *    use geuaes for test
 */
void aes_test(void);


//iSampleSize<0x200 ;need buffer for test; if test ==1,  we will test many times ,else test =2 or 0 ,we test one time
// use geurng fot test
BU_U8	geu_TestRNG(BU_U8 seed, BU_U32 iSampleSize, BU_U32 buffer, BU_U8 test); 

BU_U32 fuse_read(void);

BU_U32  fuse_write(BU_U32 cmd, BU_U32 Block_Num, BU_U32 value);

uint32_t  fuse_read_DRO(void);

void TEST_override(void);

#endif
