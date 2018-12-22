#ifndef __MRVL_SENSOR_H__
#define __MRVL_SENSOR_H__

#include "camera_reg.h"
#include "predefines.h"

#ifndef NULL
#define NULL ((void*)0)
#endif


#define AP_PMU_BASE 0xd4282800


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define SENSOR_WIDTH 640
#define SENSOR_HEIGHT 480

//0x0:pipe0  0x1:pipe1  0x2:pipe2
#define PIPE_0 0x0
#define PIPE_1 0x1
#define PIPE_2 0x2

//0x0:no scaler   0x1:down2  0x2:down4  0x3:down8   0x4:up2   0x5:up4
#define NO_SCALER 0x0
#define DOWN_SCALER_2 0x1
#define DOWN_SCALER_4 0x2
#define DOWN_SCALER_8 0x3
#define UP_SCALER_2 0x4
#define UP_SCALER_4 0x5

//0x0:raw8  0x1:raw10  0x2:raw12  0x3:yuv444   0x4:yuv422   0x5:yuv420   0x6:YUV420_SEMI    0x7:jpeg
#define RAW8 0x0
#define RAW10 0x1
#define RAW12 0x2
#define YUV444 0x3
#define YUV422 0x4
#define YUV420 0x5
#define YUV420_SEMI 0x6
#define JPEG 0x7
//#define YUV_SENSOR 0x8

//0x0:spi1lan  0x1:spi2lan	0x2:spi4lan   0x3:mipi1lan	0x4:mipi2lan  0x5:mipi4lan	0x6:dvp
#define SPI_1_LAN 0x0
#define SPI_2_LAN 0x1
#define SPI_4_LAN 0x2
#define MIPI_1_LAN 0x3
#define MIPI_2_LAN 0x4
#define MIPI_4_LAN 0x5

//online test case
#define RAW8_SPI_2LAN_NORMAL 0


#define IRQ_CAMERA_IPE 58

//CAMERA_IRQ_RAW_STATUS   Offset: 0x48
#define AEC_UPDATE_IRQ             (0x01 << 24)
#define SPI_LINE_ERR_IRQ           (0x01 << 23)
#define SPI_CRC_ERR_IRQ            (0x01 << 22)
#define PIP1_SCL_FIFO_OVERRUN_IRQ  (0x01 << 21)
#define PIP0_SCL_FIFO_OVERRUN_IRQ  (0x01 << 20)
#define PIP2_DMA_FIFO_OVERRUN_IRQ  (0x01 << 19)
#define PIP1_DMA_FIFO_OVERRUN_IRQ  (0x01 << 18)
#define PIP0_DMA_FIFO_OVERRUN_IRQ  (0x01 << 17)
#define ISIM_IRQ                   (0x01 << 16)
#define PIP1_SHADOW_DONE_IRQ       (0x01 << 15)
#define PIP0_SHADOW_DONE_IRQ       (0x01 << 14)
#define PIP2_DMA_SOF_IRQ           (0x01 << 13)
#define PIP2_DMA_EOF_IRQ           (0x01 << 12)
#define PIP1_DMA_SOF_IRQ           (0x01 << 11)
#define PIP1_DMA_EOF_IRQ           (0x01 << 10)
#define PIP0_DMA_SOF_IRQ           (0x01 << 9)	
#define PIP0_DMA_EOF_IRQ           (0x01 << 8)	
#define PIP1_SCL_SOF_IRQ           (0x01 << 7)	
#define PIP1_SCL_EOF_IRQ           (0x01 << 6)	
#define PIP0_SCL_SOF_IRQ           (0x01 << 5)	
#define PIP0_SCL_EOF_IRQ           (0x01 << 4)	
#define ISP_SOF_IRQ                (0x01 << 3)	
#define ISP_EOF_IRQ                (0x01 << 2)	
#define CSI2_SOF_IRQ               (0x01 << 1)	
#define CSI2_EOF_IRQ               (0x01 << 0)	



typedef signed   char   int_8;   /* 8-bit signed integer   */
typedef unsigned char  uint_8;  /* 8-bit signed integer   */
typedef          short int_16;  /* 16-bit signed integer  */
typedef unsigned short uint_16; /* 16-bit unsigned integer*/
typedef          long  int_32;  /* 32-bit signed integer  */
typedef unsigned long  uint_32; /* 32-bit unsigned integer*/

static uint_32 img_width = SENSOR_WIDTH;
static uint_32 img_height = SENSOR_HEIGHT;
static uint_32 h_init_ph	 = 0x8;
static uint_32 h_delta_ph = 0x10000;
static uint_32 v_init_ph	 = 0x8;
static uint_32 v_delta_ph = 0x10000;
static uint_32 h_out_size = SENSOR_WIDTH;
static uint_32 v_out_size = SENSOR_HEIGHT;
static uint_32 y_pitch = 0;

struct regval_tab{
	char reg;
	char val;
};

struct camera_config {
	uint_8 sensor_formt;   //0x0:raw8  0x1:raw10  0x2:raw12  0x3:yuv444   0x4:yuv422   0x5:yuv420   0x6:YUV420_SEMI    0x7:jpeg 
	uint_8 camera_interface;   //0x0:spi1lan  0x1:spi2lan  0x2:spi4lan   0x3:mipi1lan  0x4:mipi2lan  0x5:mipi4lan  0x6:dvp
	uint_8 spi_sdr;  //0x0:no sdr   0x1:sdr
	uint_8 spi_crc;  //0x0:no crc   0x1:crc
	uint_8 isp_bypass;  //0x0:no bypass   0x1:bypass
	uint_8 scaler;   //0x0:no scaler   0x1:down2  0x2:down4  0x3:down8   0x4:up2   0x5:up4
	uint_8 out_formt;   //0x0:raw8  0x1:raw10  0x2:raw12  0x3:yuv444   0x4:yuv422   0x5:yuv420   0x6:YUV420_SEMI    0x7:jpeg
	uint_8 jpeg_mode;   //0x0:no jpeg mode    0x1:jpeg mode
	uint_8 pipeline;   //0x0:pipe0  0x1:pipe1  0x2:pipe2 
	uint_8 set_aec;   //0x0:no set   0x1:set 
};

struct camera_test_struct {
	uint_8 test_type;
	struct camera_config camera_cfg;
};

static inline uint_32 camera_reg_read(uint_32 reg)
{
	return BU_REG_READ(REG_CAMERA_BASE + reg);
}

#if 1
static inline void camera_reg_write(uint_32 reg, uint_32 val)
{
	uint_32 read_value = 0;
	BU_REG_WRITE(REG_CAMERA_BASE + reg, val);
        mdelay(1);

	//test reg value == write value
	read_value = camera_reg_read(reg);
	if (val != read_value) {
		bbu_printf("camera_reg_write  error   reg = 0x%x   write_val = 0x%x   read_value = 0x%x\n\r", reg, val, read_value);
	}
}


#else
static inline void camera_reg_write(uint_32 reg, uint_32 val)
{
	BU_REG_WRITE(REG_CAMERA_BASE + reg, val);
}
#endif

static inline void pmu_reg_write(uint_32 reg, uint_32 val)
{
	BU_REG_WRITE(AP_PMU_BASE + reg, val);
}


static inline void camera_write_mask(uint_32 reg, uint_32 val, uint_32 mask)
{
	uint_32 v;

	v = camera_reg_read(reg);
	v = (v & ~mask) | (val & mask);
	camera_reg_write(reg, v);
}

static inline void camera_set_bit(uint_32 reg, uint_32 val)
{
	camera_write_mask(reg, val, val);
}

static inline void camera_clr_bit(uint_32 reg, uint_32 val)
{
	camera_write_mask(reg, 0, val);
}


static inline uint_8 isp_reg_read(uint_32 reg)
{
	return BU_REG_READ(ISP_REG_BASE + reg);
}

#if 1
static inline void isp_reg_write(uint_32 reg, uint_32 val)
{
	uint_32 read_value = 0;
       val &= 0xff;

	BU_REG_WRITE(ISP_REG_BASE + reg, val);
	//test reg value == write value
      mdelay(1);

	read_value = isp_reg_read(reg);
	if (val != read_value) {
		bbu_printf("isp_reg_write  error   reg = 0x%x   write_val = 0x%x   read_value = 0x%x\n\r", reg, val, read_value);
	}

}


#else
static inline void isp_reg_write(uint_32 reg, uint_32 val)
{
	BU_REG_WRITE(ISP_REG_BASE + reg, val);
}
#endif

int init_PMU();
int camear_sensor_read_id();

int camear_sensor_init();

int camera_online(uint_8 test_case);

int camera_reg_read_write();


#endif
