
#if (defined(CONFIG_NANO)&&defined(CONFIG_NANO_DMA)) 
#include "predefines.h"
#include "interrupt.h"
#include "DMA.h"

#define DMA_CHAN_CNT    32  // 32 channels work concurrently
#define DMA_TRAN_SIZE   0x1F00
#define DMA_DDR_SRC_START   0x10000000  // 256M
#define DMA_DDR_DST_START   0x11000000  
//#define DMA_DDR_SIZE    (DMA_DDR_START * DMA_TRAN_SIZE * 2)   
unsigned int dma_nano_desc_addr[16*DMA_CHAN_CNT+0x10];

struct dma_nano_desc {
	volatile unsigned int ddadr;	/* Points to the next descriptor + flags */
	volatile unsigned int dsadr;	/* DSADR value for the current transfer */
	volatile unsigned int dtadr;	/* DTADR value for the current transfer */
	volatile unsigned int dcmd;	    /* DCMD value for the current transfer */
};

static uint32_t irq_count = 1;
static void dma_nano_irq_handler(void)
{
    int i;
    unsigned int dint = DMA->DINT;
    unsigned int dcsr;
      
    for(i = 0; i < DMA_CHAN_CNT; i++)
    {
        if(dint & (1 << i)) 
        {
            dcsr = DMA->DCSR[i];
            DMA->DCSR[i] |= CSR_STARTINTR | CSR_ENDINTR | CSR_BUSERRINTR; // clear dma int
            if(dcsr & CSR_BUSERRINTR)
            {
                // bus error
                bbu_printf("be %d\r\n", i);
                //continue;
            } 
               
            if(dcsr & CSR_STARTINTR)
            {
                // start interrupt
                bbu_printf("si %d\r\n", i);
                //continue;
            }    
               
            if(dcsr & CSR_ENDINTR)
            {
                //bbu_printf("DMA: end irq on channel %d\r\n", i);
            }    
            
            // restart
            //bbu_printf("i = %d finish\r\n", i);
            DMA->DMA_DESC[i].DDADR = (((unsigned int)dma_nano_desc_addr + 16) & (~15)) + i*16;
            DMA->DCSR[i] |= CSR_RUN;
        }
    }
    
    //irq_count++;
    //if(irq_count%10 == 0)
    	//bbu_printf("DMA transfer %d times\n\r",irq_count);
}


void dma_nano_test(void)
{
    int i;
    struct dma_nano_desc *desc;
    // enable irq
    
    ISR_Connect(IRQ_MMP_DMA1, dma_nano_irq_handler, NULL);
    INT_Enable(IRQ_MMP_DMA1, 0, 15);
    //single core
    //gic_target_set(IRQ_MMP_DMA1, 1, 1);
    //icu_target_set(IRQ_MMP_DMA1, 1, 1);
    
    
    // TODO
    desc = (struct dma_nano_desc *)(((unsigned int)dma_nano_desc_addr + 16) & (~15));
    
    // configure descriptor
    for(i = 0; i < DMA_CHAN_CNT; i++) {
        desc->ddadr = (unsigned int)desc | DADR_STOP; 
        desc->dsadr = DMA_DDR_SRC_START + i * DMA_TRAN_SIZE; 
        desc->dtadr = DMA_DDR_DST_START + i * DMA_TRAN_SIZE;
        desc->dcmd  = CMD_INCSRCADDR |
                      CMD_INCTRGADDR |
                      CMD_ENDIRQEN   |
                      (3 << CMD_SIZE_BASE) |
                      (0 << CMD_WIDTH_BASE)    |
                      (DMA_TRAN_SIZE << CMD_LEN_BASE); 
        
        DMA->DALGN |= (1 << i);
        DMA->DMA_DESC[i].DDADR = (unsigned int)desc;
                      
        desc++;
        bbu_printf("desc addr = 0x%08x\r\n", desc);
    }
    
    // clean cache
    dcache_clean_range(((unsigned int)dma_nano_desc_addr + 16) & (~15), 16*DMA_CHAN_CNT);
    
    // start transfer
    for(i = 0; i < DMA_CHAN_CNT; i++) {
        DMA->DCSR[i] |= CSR_RUN;
    }
//DMA->DCSR[8] |= CSR_RUN; 
//DMA->DCSR[9] |= CSR_RUN; 
//DMA->DCSR[10] |= CSR_RUN; 
//DMA->DCSR[11] |= CSR_RUN; 
//DMA->DCSR[12] |= CSR_RUN; 
//DMA->DCSR[13] |= CSR_RUN; 
//DMA->DCSR[14] |= CSR_RUN; 
//DMA->DCSR[15] |= CSR_RUN; 
//DMA->DCSR[24] |= CSR_RUN; 
//DMA->DCSR[25] |= CSR_RUN; 
//DMA->DCSR[26] |= CSR_RUN; 
//DMA->DCSR[27] |= CSR_RUN; 
//DMA->DCSR[28] |= CSR_RUN; 
//DMA->DCSR[29] |= CSR_RUN; 
//DMA->DCSR[30] |= CSR_RUN; 
//DMA->DCSR[31] |= CSR_RUN; 
//DMA->DCSR[4] |= CSR_RUN; 
//DMA->DCSR[5] |= CSR_RUN; 
//DMA->DCSR[6] |= CSR_RUN; 
//DMA->DCSR[7] |= CSR_RUN; 
//DMA->DCSR[20] |= CSR_RUN; 
//DMA->DCSR[21] |= CSR_RUN; 
//DMA->DCSR[22] |= CSR_RUN; 
//DMA->DCSR[23] |= CSR_RUN; 
//DMA->DCSR[0] |= CSR_RUN; 
//DMA->DCSR[1] |= CSR_RUN; 
//DMA->DCSR[2] |= CSR_RUN; 
//DMA->DCSR[3] |= CSR_RUN; 
//DMA->DCSR[16] |= CSR_RUN; 
//DMA->DCSR[17] |= CSR_RUN; 
//DMA->DCSR[18] |= CSR_RUN; 
//DMA->DCSR[19] |= CSR_RUN; 
}
#endif

