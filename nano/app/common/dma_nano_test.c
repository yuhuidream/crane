#include "predefines.h"
#include "DMA.h"
#include "timer_dec.h"
#include "cache_ops.h"

#define DMA_CHAN_CNT                32  // 32 channels work concurrently
#define DMA_TRAN_SIZE               0x1F00
#define DMA_DES_MAX_COUNT           20
#define DMA_POLL_MAX                10000  
#define write_des(reg,value)	    *(volatile uint32_t *)(reg) = value

//Global variables 
//DADR DSDR DTDR CMD
static uint32_t desc_global_buffer[DMA_DES_MAX_COUNT*4];  //hold the descriptor address.

int dma_m2m_nano_xfer(uint32_t src, uint32_t dst, uint32_t size, uint32_t byte_per_desc, uint32_t burst_size){
    int ret = 0;
    int descriptor_num = 0;
    int trail_num = 0;
    uint32_t descriptor_addr = 0;
    
    descriptor_num = size / byte_per_desc;
    trail_num = size % byte_per_desc;
    if(trail_num != 0)
        descriptor_num++;
    //use channel 1 in nano
    DMA->DCSR[1] = CSR_EORJMPEN;
    DMA->DALGN |= (1 << 1);

    descriptor_addr = (uint32_t)desc_global_buffer;
    descriptor_addr = (((uint32_t)descriptor_addr & 0xFFFFFFF0) + 0x10);

    int i = 0;
    for(i=0; i<(int)descriptor_num; i++){
        uint32_t dadr = descriptor_addr + i*0x10;
        uint32_t sadr = descriptor_addr + 0x4 + i*0x10;
        uint32_t tadr = descriptor_addr + 0x8 + i*0x10;
        uint32_t cadr = descriptor_addr + 0xc + i*0x10;
        
        uint32_t dadr_value = 0;
        uint32_t cmd_reg_value = 0;
        uint32_t sadr_value = src + i*byte_per_desc;
        uint32_t tadr_value = dst + i*byte_per_desc;

        if(i != (int)(descriptor_num-1)){
          cmd_reg_value |= CMD_INCSRCADDR |
                           CMD_INCTRGADDR |
                           (burst_size << CMD_SIZE_BASE) |
                           (0 << CMD_WIDTH_BASE) |
                           (byte_per_desc << CMD_LEN_BASE);
          dadr_value = descriptor_addr + (i+1)*0x10;
        }else{
          cmd_reg_value |= CMD_INCSRCADDR |
                           CMD_INCTRGADDR |
                           (burst_size << CMD_SIZE_BASE) |
                           (0 << CMD_WIDTH_BASE) |
                           (trail_num << CMD_LEN_BASE);
          dadr_value = descriptor_addr + (i+1)*0x10;
          dadr_value |= DADR_STOP;
        } 
        write_des(dadr, dadr_value);
        write_des(sadr, sadr_value);
        write_des(tadr, tadr_value);
        write_des(cadr, cmd_reg_value);
    }

    DMA->DMA_DESC[1].DDADR = descriptor_addr;

    DMA->DCSR[1] |= CSR_RUN;    //DMA transfer begin

    int retry = 0;
    for(retry = 0; retry < DMA_POLL_MAX; retry++){
        if((DMA->DCSR[1] & CSR_STOPINTR))
            break;
        mdelay(1);
    }

    if(retry >= DMA_POLL_MAX){
        bbu_printf("DMA: transfer data timeout!\r\n");
        ret = -1;
    }

    return ret;
}
