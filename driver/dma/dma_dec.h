#ifndef __DMA_EXT_H_
#define __DMA_EXT_H_
#include "predefines.h"
#include "DMA.h"
#include "bbu_malloc.h"
#include "timer_dec.h"
#include "cache_ops.h"

#define DMA_DDADR_RESERVED_MASK	    0xFFFFFFF0
#define DMA_STATUS_INTERRUPTS_MASK  0x01000217
#define DMA_DEVICE_PART1_NUM		64

#define DMA_SQU_2_SQU_CHANNEL       0
#define DMA_SQU_2_DDR_CHANNEL       1
#define DMA_DDR_2_SQU_CHANNEL       2
#define DMA_DDR_2_DDR_CHANNEL       3
#define DMA_XFER_CHANNEL            4

#define DMA_GEU_W_CHANNEL           5
#define DMA_GEU_R_CHANNEL           6

#define DMA_SQU_ADDR_1              0xD1000000
#define DMA_SQU_ADDR_2              0xD1004000

#define DMA_MDMA_XFER_SIZE	        0xF000
#define DMA_MAX_BYTES_PER_DESC      0x1F00

#define DMA_MDMA_NF_XFER_SIZE       0x1F00

#define DMA_STRESS_XFER_SIZE        0x5000


typedef struct DMA_DESCRIPTOR_TAG{
    uint32_t    DDADR;  // descriptor address reg
    uint32_t    DSADR;  // source address register
    uint32_t    DTADR;  // target address register
    uint32_t    DCMD;   // command address register
}DMA_DESCRIPTOR, *P_DMA_DESCRIPTOR;

struct DMA_CMD_BITS 
{
    uint32_t Length         :13; //Bits [12:0]: Length of transfer in Bytes
    uint32_t Reserved4      :1; //Bit 13: reserved
    uint32_t Width          :2; //Bits [15:14]: Width of onchip peripheral - 0=reserved, 1=1B, 2=2B, 3=4B
    uint32_t MaxBurstSize   :2; //Bits [17:16]: Max Burst Size - 0=reserved, 1=8B, 2=16B, 3=32B
    uint32_t Reserved3      :3; //Bits [20:18]: reserved
    uint32_t EndIRQEn       :1; //Bit 21: end interrupt enable
    uint32_t StartIRQEn     :1; //Bit 22: start interrupt enable
    uint32_t AddrMode       :1; //Bit 23: Addressing mode for descriptor compare - 0=target has addr/1=target has data
    uint32_t Reserved2      :1; //Bit 24: reserved
    uint32_t CmpEn          :1; //Bit 25: Compare Enable - descriptor branching compare enable
    uint32_t Reserved1      :2; //Bits [27:26]: reserved
    uint32_t FlowTrg        :1; //Bit 28: Target flow control - 1=wait for request signal
    uint32_t FlowSrc        :1; //Bit 29: Source flow control - 1=wait for request signal
    uint32_t IncTrgAddr     :1; //Bit 30: Target increment - 0=no increment, 1=increment
    uint32_t IncSrcAddr     :1; //Bit 31: Source increment - 0=no increment, 1=increment
};

typedef union
{
    uint32_t value;
    struct DMA_CMD_BITS bits;   
} DMA_CMDx_T, *P_DMA_CMDx_T;

typedef enum{
    DMA_DREQ0 = 0,            // DREQ 0, Companion Chip request 0
    DMA_DREQ1 = 1,            // DREQ 1, Companion Chip request 1
    DMA_DREQ2 = 2,
    DMA_UART_0_RX = 3,
    DMA_UART_0_TX = 4,
    DMA_UART_1_RX = 5,
    DMA_UART_1_TX = 6,
    DMA_I2C0_TX = 7,
    DMA_I2C0_RX = 8,

    DMA_SSP_0_RX = 9,
    DMA_SSP_0_TX = 10,
    DMA_SSP_2_RX = 11,
    DMA_SSP_2_TX = 12,

    DMA_LCD0 = 13,
    DMA_LCD1 = 14,

    DMA_I2C1_TX = 15,
    DMA_I2C1_RX = 16,

    DMA_NAND_DATA = 44,
    DMA_NAND_CMD = 45,

    DMA_AEU_TX = 46,
    DMA_AEU_RX = 47,

    DMA_MEM2MEM_MOVE = 255  // RESERVED for Memory to Memory moves
}DMA_MAP_DEVICE;

typedef struct DMA_STRESS_ARG_TAG{
    uint32_t channel;
    uint32_t num;
    uint32_t size;
    uint32_t stop_flag;
    uint32_t mask;
    uint32_t mask_t;
    uint32_t src[32];
    uint32_t dst[32];
    uint32_t des[32];
}DMA_STRESS_ARG, *P_DMA_STRESS_ARG;

typedef enum{
    DMA_BURST_SIZE_RSVD = 0,
    DMA_BURST_SIZE_8B,
    DMA_BURST_SIZE_16B,
    DMA_BURST_SIZE_32B
}DMA_BURST_SIZE;

typedef enum{
    DMA_MODE_FETCH = 0,
    DMA_MODE_NONFETCH
}DMA_MODE;

void dma_config_desc(P_DMA_DESCRIPTOR p_des,
                     P_DMA_CMDx_T pCMD,
                     uint32_t src, 
                     uint32_t trg,
                     uint32_t bytes_len);
void dma_set_des(P_DMA_DESCRIPTOR p_des, uint32_t channel);
uint32_t dma_get_des_num(uint32_t bytes_step, uint32_t bytes_len);
void dma_map_device_to_channel(DMA_MAP_DEVICE device_id, uint32_t channel);
void dma_unmap_device_to_channel(DMA_MAP_DEVICE device_id, uint32_t channel);
void set_user_aligment(uint32_t channel);
void dma_xfer_start(uint32_t channel);
void dma_xfer_stop(uint32_t channel);
void dma_set_mode(uint32_t mode, uint32_t channel);
void dma_clr_irq(uint32_t channel);
void dma_stop_irq_en(uint32_t channel);
void dma_stop_irq_dis(uint32_t channel);
void dma_set_dcsr(uint32_t val, uint32_t mask, uint32_t channel);
void dma_connect_irq_handler();
void dma_disconnect_irq_handler();
uint32_t dma_is_channel_busy(uint32_t channel);
uint32_t dma_read_status(uint32_t channel);
uint32_t _dma_m2m_xfer(uint32_t des, uint32_t src, uint32_t dst, uint32_t size, uint32_t byte_per_desc, uint32_t burst_size, uint32_t channel);

uint32_t dma_m2m_xfer(uint32_t src, uint32_t dst, uint32_t size, uint32_t byte_per_desc, uint32_t burst_size, uint32_t channel);
uint32_t dma_m2m_xfer_nf(uint32_t src, uint32_t dst, uint32_t size, uint32_t burst_size, uint32_t channel);

//uint32_t dma_m2m_xfer_test(uint32_t mode);
uint32_t dma_stress_test(uint32_t channel_start, uint32_t channel_num, uint32_t size, uint32_t entry_first);
void dma_stress_stop();

void bbu_dma_test(uint32_t argc, uint8_t **argv, QEvent const *e);
void bbu_dma_help();
void dma_stress_show_iter();
void dma_stress_set_iter(uint32_t iter);
uint32_t dma_m2m_xfer_test(uint32_t mode, uint32_t direction, uint32_t channel);
typedef enum
{
	DMA_STOP = 0,
	DMA_END
}DMA_WAIT_DONE_FLAG;

void dma_config_descriptor(uint32_t* p_desc,
                       uint32_t* p_next_desc,
                       uint32_t p_src_addr,
                       uint32_t p_tag_addr,
                       uint32_t p_cmd,
                       uint32_t stop_bit);
void dma_load_descriptor(P_DMA_DESCRIPTOR p_desc, uint32_t channel);
void dma_enable(uint32_t channel);
/*
 * pass--1  fail--0
 */
int dma_wait_done(uint32_t channel, uint32_t timeout, uint32_t flags);

#endif
