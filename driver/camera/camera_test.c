#include "camera_spi_gc032a_test.h"
#include "camera_test.h"
#include "camera_reg.h"


struct camera_test_struct camera_test_case[] = {
	{
		.test_type = RAW8_SPI_2LAN_NORMAL,
		{
			.sensor_formt = 0x0,   //0x0:raw8  0x1:raw10  0x2:raw12  0x3:yuv444   0x4:yuv422   0x5:yuv420   0x6:YUV420_SEMI    0x7:jpeg
			.camera_interface = 0x1,   //0x0:spi1lan  0x1:spi2lan  0x2:spi4lan   0x3:mipi1lan  0x4:mipi2lan  0x5:mipi4lan  0x6:dvp
			.spi_sdr = 0x1,  //0x0:no sdr   0x1:sdr
			.spi_crc = 0x0,  //0x0:no crc   0x1:crc
			.isp_bypass = 0x0,  //0x0:no bypass   0x1:bypass
			.scaler = 0x0,   //0x0:no scaler   0x1:down2  0x2:down4  0x3:down8   0x4:up2   0x5:up4
			.out_formt = 0x0,   //0x0:raw8  0x1:raw10  0x2:raw12  0x3:yuv444   0x4:yuv422   0x5:yuv420   0x6:YUV420_SEMI    0x7:jpeg
			.jpeg_mode = 0x0,   //0x0:no jpeg mode    0x1:jpeg mode
			.pipeline = 0x0,   //0x0:pipe0  0x1:pipe1  0x2:pipe2
			.set_aec = 0x0,   //0x0:no set   0x1:set
		}
	},

};



//0x0:down2  0x1:down4  0x2:down8   0x3:up2   0x4:up4
void get_phase_and_size_base_scaler(uint_8 scaler_value)
{

	switch (scaler_value) {
		case DOWN_SCALER_2:
			h_init_ph	= 0x10;
			h_delta_ph = 0x20000;
			v_init_ph	= 0x10;
			v_delta_ph = 0x20000;
			h_out_size = img_width/2;
			v_out_size = img_height/2;
		break;
		case DOWN_SCALER_4:
			h_init_ph	= 0x20;
			h_delta_ph = 0x40000;
			v_init_ph	= 0x20;
			v_delta_ph = 0x40000;
			h_out_size = img_width/4;
			v_out_size = img_height/4;
		break;
		case DOWN_SCALER_8:
			h_init_ph	= 0x40;
			h_delta_ph = 0x80000;
			v_init_ph	= 0x40;
			v_delta_ph = 0x80000;
			h_out_size = img_width/8;
			v_out_size = img_height/8;
		break;
		case UP_SCALER_2:
			h_init_ph	= 0x4;
			h_delta_ph = 0x8000;
			v_init_ph	= 0x4;
			v_delta_ph = 0x8000;
			h_out_size = img_width*2;
			v_out_size = img_height*2;
		break;
		case UP_SCALER_4:
			h_init_ph	= 0x2;
			h_delta_ph = 0x4000;
			v_init_ph	= 0x2;
			v_delta_ph = 0x4000;
			h_out_size = img_width*4;
			v_out_size = img_height*4;
		break;
		default:
			break;
	}
    bbu_printf("get_phase_and_size_base_scaler: scaler_value = %d    h_out_size = %d\n", scaler_value, h_out_size);

	if (0 == h_out_size % 8) {
		y_pitch = h_out_size;
    } else { 
        y_pitch = ((h_out_size/8)+1)*8;
    }
}


void config_csi2()
{
    camera_reg_write(REG_CSI2_CTRL_0, 0x1); // [0] csi2_en 
    camera_reg_write(REG_CSI2_DPHY1, 0x1); // DPHY 1 ana pu
    camera_reg_write(REG_CSI2_DPHY2, 0xa2840080); // DPHY 2
    camera_reg_write(REG_CSI2_DPHY3, 0x1500); // DPHY 3 hs_term_en/hs_settle
    camera_reg_write(REG_CSI2_DPHY5, 0x11); // DPHY 5 lane_en
}

void set_yuv_addr()
{
    bbu_printf("set_yuv_addr: E!! \n");

    camera_reg_write(REG_Y0_BASE, 0x0+0x80000000); // Y0 Base Addr
    camera_reg_write(REG_Y1_BASE, 0x10000+0x80000000); // Y1 Base Addr
    camera_reg_write(REG_Y2_BASE, 0x20000+0x80000000); // Y2 Base Addr
    camera_reg_write(REG_U0_BASE, 0x400000+0x80000000); // U0 Base Addr
    camera_reg_write(REG_U1_BASE, 0x410000+0x80000000); // U1 Base Addr
    camera_reg_write(REG_U2_BASE, 0x420000+0x80000000); // U2 Base Addr
    camera_reg_write(REG_V0_BASE, 0x800000+0x80000000); // V0 Base Addr
    camera_reg_write(REG_V1_BASE, 0x810000+0x80000000); // V1 Base Addr
    camera_reg_write(REG_V2_BASE, 0x820000+0x80000000); // V2 Base Addr
}

void set_isp_output_image_size()
{
    bbu_printf("set_isp_output_image_size: img_width = %d , img_height = %d\n", img_width, img_height);

    img_width &= 0x3fff;
    img_height &= 0x3fff;
    camera_reg_write(REG_ISP_IMG_SIZE, img_width+(img_height<<16));  // image size from isp
}


void use_which_pipe_scaler(uint_8 pipe, uint_8 scaler_value)
{
    uint_32 value = 0;

    if (PIPE_0 == pipe) {
        value = 0x00001;
        if (UP_SCALER_4 == scaler_value) {
            value = 0xff001;
        } else if (UP_SCALER_2 == scaler_value) {
            value = 0xff001;
        } else if (DOWN_SCALER_2 == scaler_value) {
            value = 0x55001;
        } else if (DOWN_SCALER_4 == scaler_value) {
            value = 0x00001;
        }
    } else if (PIPE_1 == pipe) {
        value = 0x00002;
        if (UP_SCALER_4 == scaler_value) {
            value = 0xff002;
        } else if (UP_SCALER_2 == scaler_value) {
            value = 0xff002;
        } else if (DOWN_SCALER_2 == scaler_value) {
            value = 0x55002;
        } else if (DOWN_SCALER_4 == scaler_value) {
            value = 0x00002;
        }
    } else if (PIPE_2 == pipe) {
        value = 0x4;
    }
    
    bbu_printf("use_which_pipe_scaler: pipe = %d , scaler_value = %d  value = 0x%x\n", pipe, scaler_value, value);
    camera_reg_write(REG_SUBSAMPLE_SCALER_CTRL, value); // Scaler/Subsample mode [15:12] sel scl_coef_table
}

void set_yuv_jpeg_mode(uint_8 yuv_type, uint_8 jpeg_mode)
{

    bbu_printf("set_yuv_jpeg_mode: yuv_type = %d , jpeg_mode = %d \n", yuv_type, jpeg_mode);

	if (YUV420 == yuv_type) {
		camera_reg_write(REG_IMG_PITCH_PIP0, y_pitch+(h_out_size<<16)); // Pipe 1 image pitch
		camera_reg_write(REG_PIP1_CCIC_IMG_PITCH, y_pitch+(h_out_size<<16)); // Pipe 1 image pitch
		camera_reg_write(REG_PIP2_CCIC_IMG_PITCH, y_pitch+(h_out_size<<16)); // Pipe 2 image pitch
    	if (1 == jpeg_mode) {
			camera_reg_write(REG_DATA_FORMAT_CTRL, 0x1d0d0d0); // [6:4] YUV420 mode output [7] JPEG_MDOE
			camera_reg_write(REG_IMG_SIZE_PIP0, (h_out_size-2)+(v_out_size<<16));  // image width by bytes for wdma
			camera_reg_write(REG_IMG_SIZE_PIP1, (h_out_size-2)+(v_out_size<<16));	// image width by bytes for wdma
			camera_reg_write(REG_IMG_SIZE_PIP2, (h_out_size-2)+(v_out_size<<16));	// image width by bytes for wdma
    	} else {
		   camera_reg_write(REG_DATA_FORMAT_CTRL, 0x1505050); // [6:4] YUV420 mode output
		   camera_reg_write(REG_IMG_SIZE_PIP0, h_out_size+(v_out_size<<16));  // image width by bytes for wdma
		   camera_reg_write(REG_IMG_SIZE_PIP1, h_out_size+(v_out_size<<16));  // image width by bytes for wdma
		   camera_reg_write(REG_IMG_SIZE_PIP2, h_out_size+(v_out_size<<16));  // image width by bytes for wdma
    	}
	} else if (YUV422 == yuv_type) {
		camera_reg_write(REG_IMG_PITCH_PIP0, h_out_size*2+(h_out_size<<16)); // Pipe 1 image pitch
		camera_reg_write(REG_PIP1_CCIC_IMG_PITCH, h_out_size*2+(h_out_size<<16)); // Pipe 1 image pitch
		camera_reg_write(REG_PIP2_CCIC_IMG_PITCH, h_out_size*2+(h_out_size<<16)); // Pipe 2 image pitch
   		if (1 == jpeg_mode) {
			camera_reg_write(REG_DATA_FORMAT_CTRL, 0xa9c0c0c0); // [6:4] YUV422 mode output [27:26] yuvendfmt [7] JPEG_MDOE
			camera_reg_write(REG_IMG_SIZE_PIP0, (h_out_size-2)*2+(v_out_size<<16));  // image width by bytes for wdma
			camera_reg_write(REG_IMG_SIZE_PIP1, (h_out_size-2)*2+(v_out_size<<16));  // image width by bytes for wdma
			camera_reg_write(REG_IMG_SIZE_PIP2, (h_out_size-2)*2+(v_out_size<<16));  // image width by bytes for wdma
		} else {
			camera_reg_write(REG_DATA_FORMAT_CTRL, 0xa9404040); // [6:4] YUV422 mode output [27:26] yuvendfmt
			camera_reg_write(REG_IMG_SIZE_PIP0, h_out_size*2+(v_out_size<<16));  // image width by bytes for wdma
			camera_reg_write(REG_IMG_SIZE_PIP1, h_out_size*2+(v_out_size<<16));  // image width by bytes for wdma
			camera_reg_write(REG_IMG_SIZE_PIP2, h_out_size*2+(v_out_size<<16));  // image width by bytes for wdma
		}
	} else if (YUV420_SEMI == yuv_type) {
		camera_reg_write(REG_IMG_PITCH_PIP0, y_pitch+(h_out_size<<15)); // Pipe 1 image pitch
		camera_reg_write(REG_PIP1_CCIC_IMG_PITCH, y_pitch+(h_out_size<<15)); // Pipe 1 image pitch
		camera_reg_write(REG_PIP2_CCIC_IMG_PITCH, y_pitch+(h_out_size<<15)); // Pipe 2 image pitch
   		if (1 == jpeg_mode) {

			camera_reg_write(REG_DATA_FORMAT_CTRL, 0x1858585); // [6:4] YUV420-semi mode output [7] jpeg mod
			camera_reg_write(REG_IMG_SIZE_PIP0, (h_out_size-2)+(v_out_size<<16));  // image width by bytes for wdma
			camera_reg_write(REG_IMG_SIZE_PIP1, (h_out_size-2)+(v_out_size<<16));	// image width by bytes for wdma
			camera_reg_write(REG_IMG_SIZE_PIP2, (h_out_size-2)+(v_out_size<<16));	// image width by bytes for wdma
		} else {
			camera_reg_write(REG_DATA_FORMAT_CTRL, 0x1050505); // [6:4] YUV420-semi mode output [7] jpeg mode
			camera_reg_write(REG_IMG_SIZE_PIP0, (h_out_size)+(v_out_size<<16));  // image width by bytes for wdma
			camera_reg_write(REG_IMG_SIZE_PIP1, (h_out_size)+(v_out_size<<16));  // image width by bytes for wdma
			camera_reg_write(REG_IMG_SIZE_PIP2, (h_out_size)+(v_out_size<<16));  // image width by bytes for wdma
		}

	}

	if (1 == jpeg_mode) {
	    camera_reg_write(REG_PIP0_CROP_ROI_X, img_width<<16+2); // Pipe 0 ROI X crop 2
	    camera_reg_write(REG_PIP0_SCALER_IN_SIZE, (img_width-2)+(img_height<<16)); // Pipe 0 SCALER IN SIZE
	    camera_reg_write(REG_PIP0_SCALER_OUT_SIZE, (h_out_size-2)+(v_out_size<<16)); // Pipe 0 SCALER OUT SIZ
	    camera_reg_write(REG_PIP1_SCALER_IN_SIZE, (img_width-2)+(img_height<<16)); // Pipe 1 SCALER IN SIZE
	    camera_reg_write(REG_PIP1_SCALER_OUT_SIZE, (h_out_size-2)+(v_out_size<<16)); // Pipe 1 SCALER OUT SIZE
	} else {
	    camera_reg_write(REG_PIP0_CROP_ROI_X, img_width<<16); // Pipe 0 ROI X
	    camera_reg_write(REG_PIP0_SCALER_IN_SIZE, img_width+(img_height<<16)); // Pipe 0 SCALER IN SIZE
	    camera_reg_write(REG_PIP0_SCALER_OUT_SIZE, h_out_size+(v_out_size<<16)); // Pipe 0 SCALER OUT SIZ 
	    camera_reg_write(REG_PIP1_SCALER_IN_SIZE, img_width+(img_height<<16)); // Pipe 1 SCALER IN SIZE
	    camera_reg_write(REG_PIP1_SCALER_OUT_SIZE, h_out_size+(v_out_size<<16)); // Pipe 1 SCALER OUT SIZE
	}
}


void config_pipe()
{
    bbu_printf("config_pipe:  E!! \n");

    camera_reg_write(REG_PIP0_CROP_ROI_Y, img_height<<16); // Pipe 0 ROI Y
    camera_reg_write(REG_PIP1_CROP_ROI_X, img_width<<16); // Pipe 1 ROI X
    camera_reg_write(REG_PIP1_CROP_ROI_Y, img_height<<16); // Pipe 1 ROI Y
    camera_reg_write(REG_PIP0_SCALER_H_PHASE, h_delta_ph+(h_init_ph<<24)); // Pipe 0 SCALER H phase
    camera_reg_write(REG_PIP0_SCALER_V_PHASE, v_delta_ph+(v_init_ph<<24)); // Pipe 0 SCALER V phase
    camera_reg_write(REG_PIP1_SCALER_H_PHASE, h_delta_ph+(h_init_ph<<24)); // Pipe 1 SCALER H phase
    camera_reg_write(REG_PIP1_SCALER_V_PHASE, v_delta_ph+(v_init_ph<<24)); // Pipe 1 SCALER V phase
}

void set_ccic_sensor_format(uint_8 sensor_format)
{
    bbu_printf("set_ccic_sensor_format:  sensor_format = %d \n", sensor_format);

	if (sensor_format > RAW12) {
		camera_reg_write(REG_ISP_DVP_IF_CTRL, (img_width<<16)+0x80008050); // [7:0] min hblank [26:16] dvp fifo trigger [31] bypass ISP
	} else {
		camera_reg_write(REG_ISP_DVP_IF_CTRL, (img_width<<16)+0x8050); // [7:0] min hblank [26:16] dvp fifo trigger 
	}
}

void set_isp_sensor_format(uint_8 output_formt)
{
	uint_32 data_tmp = 0;
    bbu_printf("set_isp_sensor_format:  output_formt = %d \n", output_formt);

	if (RAW8 == output_formt) {
		isp_reg_write(0x210, 0x00); // [4] is_yuv422  [3:0] byapss mode) =  0:YUV) =  5:SPI YUV422) =  6:RGB
		isp_reg_write(0x24c, 0x00); // out_win_x1 Hi
		isp_reg_write(0x250, 0x02); // out_win_x1 Low
		isp_reg_write(0x044, 0x00000000); // SPI trig num
		isp_reg_write(0x048, 0x00000096);
		data_tmp = img_width;
		isp_reg_write(0x084, (data_tmp>>8) & 0x7); // SPI_manual_width=1280 (in bytes)) =  = image_width*2  when YUV422 mode
		isp_reg_write(0x088, data_tmp & 0xff);
	} else {
		isp_reg_write(0x210, 0x15); // [4] is_yuv422  [3:0] byapss mode) =  0:YUV) =  5:SPI YUV422) =  6:RGB
		isp_reg_write(0x24c, 0x00); // out_win_x1 Hi
		isp_reg_write(0x250, 0x00); // out_win_x1 Low
		isp_reg_write(0x044, 0x00000001); // SPI trig num
		isp_reg_write(0x048, 0x0000002c);
		data_tmp = img_width *2;
		isp_reg_write(0x084, (data_tmp>>8) & 0x7); // SPI_manual_width=1280 (in bytes)) =  = image_width*2  when YUV422 mode
		isp_reg_write(0x088, data_tmp & 0xff);
	}
}

void set_isp_bypass(uint_8 isp_bypass)
{

    bbu_printf("set_isp_bypass:  isp_bypass = %d \n", isp_bypass);

	if (1 == isp_bypass) {
		isp_reg_write(0x210, 0x01); // [4] is_yuv422  [3:0] byapss mode) =  0:YUV) =  5:SPI YUV422) =  6:RGB
		isp_reg_write(0x250, 0x00); // out_win_x1 Low
		isp_reg_write(0x234, 0x01); // debug_mode3
	} else {
		isp_reg_write(0x210, 0x00); // [4] is_yuv422  [3:0] byapss mode) =  0:YUV) =  5:SPI YUV422) =  6:RGB
		isp_reg_write(0x250, 0x02); // out_win_x1 Low
		isp_reg_write(0x234, 0x03); // debug_mode3
	}
}

void set_spi_sdr(struct camera_config camera_cfg)
{
	uint_32 sdr_ddr = 0;
	uint_32 neg_sample = 0;
	uint_32 crc = 0;
	uint_32 lane_num = 0;
	uint_32 lane_num_sen = 0;
	uint_32 ddr_switch = 0;

	if (1 == camera_cfg.spi_crc) {
		crc = 1;
	}

	if (SPI_1_LAN == camera_cfg.camera_interface) {
		lane_num		= 0;
		lane_num_sen	= 0;
		ddr_switch	 = 0;
	} else if (SPI_2_LAN == camera_cfg.camera_interface) {
		lane_num		= 1;
		lane_num_sen	= 1;
		ddr_switch	 = 0;
	} else if (SPI_4_LAN == camera_cfg.camera_interface) {
		lane_num		= 3;
		lane_num_sen	= 2;
		ddr_switch	 = 1;
	}

	if (1 == camera_cfg.spi_sdr) {
		if (camera_cfg.camera_interface > SPI_4_LAN) {
			return;
		}
		set_isp_sensor_format(camera_cfg.out_formt);
		isp_reg_write(0x218, (crc<<2)+0x3); // [2] SPI crc en [1] SPI en [0] input sel
		isp_reg_write(0x078, 0x00000000); //[6:4] spi manual mode  [3] manual mode en [2] manual height en [1] manual width [0]ignore line id
		isp_reg_write(0x07c, (lane_num<<4)+(neg_sample<<3)+(sdr_ddr<<2)+lane_num);  //[5:4] SPI data switch [3]neg sample [2]ddr mode [1:0]lane num
		isp_reg_write(0x08c, (img_height>>8) & 0x7); // SPI_manual_height=80
		isp_reg_write(0x090, img_height & 0xff);
		isp_reg_write(0x234, 0x03); // debug_mode3
	} else {
		isp_reg_write(0x24c, 0x00); // out_win_x1 Hi
		set_isp_bypass(camera_cfg.isp_bypass);
	}

}

void config_aec(uint_8 set_aec)
{
	if (1 == set_aec) {
		isp_reg_write(0xaa0, 0x00000080); // AEC_CTL_mode1[7:0] = 0x00;
		/*
		0x0AA0	AEC_CTL_mode1
		[7] is_16bit_addr
		[6] is_16bit_data
		[5] use vsync_posedge
		[4] exp_swap_byte
		[2] use_AEC_update
		[1:0] exp_bits_sel
		*/
		isp_reg_write(0x0b4, 0x0000001d); // AEC_CTL_mode2[7:0] = 0x1f;	// [1] send_page_en is on in 8bit addr mode
		/*
		0x00B4 AEC_CTL_mode2
		[6] send_Again2_en
		[5] send_Again1_en
		[4] send_code_en
		[3] send_exp2_en
		[2] send_exp1_en
		[1] send_page_en
		[0] send_en
		*/
		isp_reg_write(0xaec, 0x000000af); // I2C_master_mode[7:0] = 0x0f;

		isp_reg_write(0x2d8, 0x00000001); // [0] AEC_en=1'b1;
		/*
		0x02D8 AEC_enable
		[7:1] NA
		[0] AEC enable
		*/
		isp_reg_write(0x42c, 0x00000011); // [6:4] AEC_every_N = 3'd1;
		/*
		0x042C	AEC_mode2
		[7] fix target
		[6:4] AEC take action every N frame
		[3:0 Reserved
		*/
	}
}

void config_isp(struct camera_config camera_cfg)
{
	uint_32 data_tmp = img_height+6;;

	isp_reg_write(0x34, (data_tmp>>8) & 0x7); // CISCTL_win_height
	isp_reg_write(0x38, data_tmp & 0xff);  // CISCTL_win_height
	isp_reg_write(0x3c, (img_width>>8) & 0x7); // CISCTL_win_width
	isp_reg_write(0x40, img_width & 0xff);  // CISCTL_win_width

	set_spi_sdr(camera_cfg);
	isp_reg_write(0x5c, 0x10); //[5:4] CFA) =  00:GRBG) =  01:RGGB) =  10:BGGR) =  11:GBRG
	isp_reg_write(0x254, (img_height>>8) & 0x7); // out window height
	isp_reg_write(0x258, img_height & 0xff);  // out window height
	isp_reg_write(0x25c, (img_width>>8) & 0x7);	// out window width
	isp_reg_write(0x260, img_width & 0xff);	 // out window width

	config_aec(camera_cfg.set_aec);
}

void camera_run(struct camera_config camera_cfg)
{

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

	get_phase_and_size_base_scaler(camera_cfg.scaler);
	
	config_csi2();

	set_yuv_addr();
	
	set_isp_output_image_size();

	set_yuv_jpeg_mode(camera_cfg.out_formt, camera_cfg.jpeg_mode);

	config_pipe();

	use_which_pipe_scaler(camera_cfg.pipeline, camera_cfg.scaler);

	set_ccic_sensor_format(camera_cfg.sensor_formt);

	config_isp(camera_cfg);

}


struct camera_config get_camera_config(uint_8 test_case) 
{
	int i = 0;
	for (i = 0; i< ARRAY_SIZE(camera_test_case); i++) {
		if (test_case == camera_test_case[i].test_type) {
			return camera_test_case[i].camera_cfg;
		}
	}

}


void camera_isr_handler(void *arg)
{
	uint_32 irqs;

	bbu_printf("%s IN!!!\n", __func__);

	irqs = camera_reg_read(REG_CAMERA_IRQ_RAW_STATUS);
	camera_reg_write(REG_CAMERA_IRQ_STATUS, irqs);   //clear irq
	bbu_printf("REG_online_IRQ_STATUS = 0x%08x\n", irqs);

	if (irqs & PIP1_SCL_SOF_IRQ) {
		bbu_printf("PIP1_SCL_SOF_IRQ\n");
	}

	if (irqs & PIP1_SCL_EOF_IRQ) {
		bbu_printf("PIP1_SCL_EOF_IRQ\n");
	}

	if (irqs & PIP0_SCL_SOF_IRQ) {
		bbu_printf("PIP0_SCL_SOF_IRQ\n");
	}

	if (irqs & PIP0_SCL_EOF_IRQ) {
		bbu_printf("PIP0_SCL_EOF_IRQ\n");
	}

	if (irqs & ISP_SOF_IRQ) {
		bbu_printf("ISP_SOF_IRQ\n");
	}

	if (irqs & ISP_EOF_IRQ) {
		bbu_printf("ISP_EOF_IRQ\n");
	}
}

void camera_irq_init(void)
{
	bbu_printf("camera_irq_init: E!!\n");

	/* Interupt Mask */
	//camera_set_bit(REG_CAMERA_IRQ_RAW_MASK, PIP1_SCL_SOF_IRQ | PIP1_SCL_EOF_IRQ | PIP0_SCL_SOF_IRQ | PIP0_SCL_EOF_IRQ | ISP_SOF_IRQ | ISP_EOF_IRQ);
    camera_set_bit(REG_CAMERA_IRQ_RAW_MASK, 0xffffffff);

	ISR_Connect(IRQ_CAMERA_IPE, camera_isr_handler, NULL);
	INT_Enable(IRQ_CAMERA_IPE, 0, 15);
}


int camera_stream_on()
{
	return gc030a_stream_on();
}

int camera_stream_off()
{
	return gc030a_stream_off();
}


void run_camera_1(void) {
    unsigned img_width =640;
    unsigned img_height =480;
    unsigned h_init_ph   = 0x8;
    unsigned h_delta_ph = 0x10000;
    unsigned v_init_ph   = 0x8;
    unsigned v_delta_ph = 0x10000;
    unsigned h_out_size = img_width;
    unsigned v_out_size = img_height;
    unsigned y_pitch;
    unsigned data_tmp;
    unsigned       sdr_ddr    = 0;
    unsigned       neg_sample = 1;
    unsigned       crc = 0;
    unsigned  lane_num      = 1;
    unsigned  lane_num_sen  = 1;
    unsigned       ddr_switch    = 0;
    unsigned       yuv_raw       = 0;
    bbu_printf("-------run_camera_1-------!\n\r");

    camera_reg_write(0x100,   0x1); // [0] csi2_en 
    camera_reg_write(0x124,   0x1); // DPHY 1 ana pu
    camera_reg_write(0x128,   0xa2840080); // DPHY 2
    camera_reg_write(0x12c,   0x1500); // DPHY 3 hs_term_en/hs_settle
    camera_reg_write(0x134,   0x11); // DPHY 5 lane_en
    camera_reg_write(0x0,   0x220000+RAM_BASE_REG); // Y0 Base Addr
    camera_reg_write(0x4,   0x230000+RAM_BASE_REG); // Y1 Base Addr
    camera_reg_write(0x8,   0x240000+RAM_BASE_REG); // Y2 Base Addr
    camera_reg_write(0xc,   0x250000+RAM_BASE_REG); // U0 Base Addr
    camera_reg_write(0x10,   0x260000+RAM_BASE_REG); // U1 Base Addr
    camera_reg_write(0x14,   0x270000+RAM_BASE_REG); // U2 Base Addr
    camera_reg_write(0x18,   0x280000+RAM_BASE_REG); // V0 Base Addr
    camera_reg_write(0x1c,   0x290000+RAM_BASE_REG); // V1 Base Addr
    camera_reg_write(0x20,   0x2a0000+RAM_BASE_REG); // V2 Base Addr
    camera_reg_write(0x28c,   img_width+(img_height<<16));  // image size from isp
    // YUV422 mode 2B/pix
    if(h_out_size%8==0) y_pitch = h_out_size;
    else y_pitch = ((h_out_size/8)+1)*8;

    camera_reg_write(0x24,   y_pitch+(h_out_size<<16)); // Pipe 1 image pitch
    camera_reg_write(0x240, y_pitch+(h_out_size<<16)); // Pipe 1 image pitch
    camera_reg_write(0x244, y_pitch+(h_out_size<<16)); // Pipe 2 image pitch
    camera_reg_write(0x27c,  0x1505050); // [6:4] YUV420 mode output
    camera_reg_write(0x34,   h_out_size+(v_out_size<<16));  // image width by bytes for wdma
    camera_reg_write(0x284,   h_out_size+(v_out_size<<16));  // image width by bytes for wdma
    camera_reg_write(0x288,   h_out_size+(v_out_size<<16));  // image width by bytes for wdma
    camera_reg_write(0x248,   img_width<<16); // Pipe 0 ROI X
    camera_reg_write(0x258,   img_width+(img_height<<16)); // Pipe 0 SCALER IN SIZE
    camera_reg_write(0x25c,   h_out_size+(v_out_size<<16)); // Pipe 0 SCALER OUT SIZ 
    camera_reg_write(0x268,   img_width+(img_height<<16)); // Pipe 1 SCALER IN SIZE
    camera_reg_write(0x26c,   h_out_size+(v_out_size<<16)); // Pipe 1 SCALER OUT SIZE
    camera_reg_write(0x24c,   img_height<<16); // Pipe 0 ROI Y
    camera_reg_write(0x250,   img_width<<16); // Pipe 1 ROI X
    camera_reg_write(0x254,   img_height<<16); // Pipe 1 ROI Y
    camera_reg_write(0x260,   h_delta_ph+(h_init_ph<<24)); // Pipe 0 SCALER H phase
    camera_reg_write(0x264,   v_delta_ph+(v_init_ph<<24)); // Pipe 0 SCALER V phase
    camera_reg_write(0x270,   h_delta_ph+(h_init_ph<<24)); // Pipe 1 SCALER H phase
    camera_reg_write(0x274,   v_delta_ph+(v_init_ph<<24)); // Pipe 1 SCALER V phase
    camera_reg_write(0x278,  0x00002); // Scaler/Subsample mode [15:12] sel scl_coef_table default value
    camera_reg_write(0x280,   (img_width<<16)+0x8050); // [7:0] min hblank [26:16] dvp fifo trigger 

    // george:config ISP
    data_tmp = img_height+6;
    isp_reg_write(0x34,   (data_tmp>>8) & 0x7); // CISCTL_win_height
    isp_reg_write(0x38,   data_tmp & 0xff);  // CISCTL_win_height
    isp_reg_write(0x3c,   (img_width>>8) & 0x7); // CISCTL_win_width
    isp_reg_write(0x40,   img_width & 0xff);  // CISCTL_win_width
    isp_reg_write(0x210,   0x00); // [4] is_yuv422  [3:0] byapss mode,   0:YUV,   5:SPI YUV422,   6:RGB
    isp_reg_write(0x24c,   0x00); // out_win_x1 Hi
    isp_reg_write(0x250,   0x02); // out_win_x1 Low
    isp_reg_write(0x044,  0x00000000); // SPI trig num
    isp_reg_write(0x048,  0x00000096);
    data_tmp = img_width;
    isp_reg_write(0x084,  (data_tmp>>8) & 0x7); // SPI_manual_width=1280 (in bytes),   = image_width*2  when YUV422 mode
    isp_reg_write(0x088,  data_tmp & 0xff);
    isp_reg_write(0x218,  (crc<<2)+0x3); // [2] SPI crc en [1] SPI en [0] input sel
    isp_reg_write(0x078,  0x00000000); //[6:4] spi manual mode  [3] manual mode en [2] manual height en [1] manual width [0]ignore line id
    isp_reg_write(0x07c,  (lane_num<<4)+(neg_sample<<3)+(sdr_ddr<<2)+lane_num);  //[5:4] SPI data switch [3]neg sample [2]ddr mode [1:0]lane num
    isp_reg_write(0x08c,  (img_height>>8) & 0x7); // SPI_manual_height=80
    isp_reg_write(0x090,  img_height & 0xff);
    isp_reg_write(0x234,   0x03); // debug_mode3
    isp_reg_write(0x5c,   0x10); //[5:4] CFA,   00:GRBG,   01:RGGB,   10:BGGR,   11:GBRG
    isp_reg_write(0x254,   (img_height>>8) & 0x7); // out window height
    isp_reg_write(0x258,   img_height & 0xff);  // out window height
    isp_reg_write(0x25c,   (img_width>>8) & 0x7);  // out window width
    isp_reg_write(0x260,   img_width & 0xff);   // out window width
}

void bare_run_camera_mipi(void)
{
	unsigned img_width = 640;
	unsigned img_height = 480;
	unsigned h_init_ph = 0x8;
	unsigned h_delta_ph = 0x10000;
	unsigned v_init_ph = 0x8;
	unsigned v_delta_ph = 0x10000;
	unsigned h_out_size = img_width;
	unsigned v_out_size = img_height;
	unsigned y_pitch;
	unsigned data_tmp;
	unsigned sdr_ddr = 0;
	unsigned neg_sample = 1;
	unsigned crc = 0;
	unsigned lane_num = 1;
	unsigned lane_num_sen = 1;
	unsigned ddr_switch = 0;
	unsigned yuv_raw = 0;
	bbu_printf("-------run_camera_1-------!\n\r");

	camera_reg_write(0x100, 0x1);				  // [0] csi2_en
	camera_reg_write(0x124, 0x1);				  // DPHY 1 ana pu
	camera_reg_write(0x128, 0xa2840080);			  // DPHY 2
	camera_reg_write(0x12c, 0x1500);			  // DPHY 3 hs_term_en/hs_settle
	camera_reg_write(0x134, 0x11);				  // DPHY 5 lane_en
	camera_reg_write(0x0, 0x220000 + 0x80000000);		  // Y0 Base Addr
	camera_reg_write(0x4, 0x230000 + 0x80000000);		  // Y1 Base Addr
	camera_reg_write(0x8, 0x240000 + 0x80000000);		  // Y2 Base Addr
	camera_reg_write(0xc, 0x250000 + 0x80000000);		  // U0 Base Addr
	camera_reg_write(0x10, 0x260000 + 0x80000000);		  // U1 Base Addr
	camera_reg_write(0x14, 0x270000 + 0x80000000);		  // U2 Base Addr
	camera_reg_write(0x18, 0x280000 + 0x80000000);		  // V0 Base Addr
	camera_reg_write(0x1c, 0x290000 + 0x80000000);		  // V1 Base Addr
	camera_reg_write(0x20, 0x2a0000 + 0x80000000);		  // V2 Base Addr
	camera_reg_write(0x28c, img_width + (img_height << 16));  // image size from isp
	// YUV422 mode 2B/pix
	if (h_out_size % 8 == 0)
		y_pitch = h_out_size;
	else
		y_pitch = ((h_out_size / 8) + 1) * 8;

	camera_reg_write(0x24, y_pitch + (h_out_size << 16));      // Pipe 1 image pitch
	camera_reg_write(0x240, y_pitch + (h_out_size << 16));     // Pipe 1 image pitch
	camera_reg_write(0x244, y_pitch + (h_out_size << 16));     // Pipe 2 image pitch
	camera_reg_write(0x27c, 0x1505050);			   // [6:4] YUV420 mode output
	camera_reg_write(0x34, h_out_size + (v_out_size << 16));   // image width by bytes for wdma
	camera_reg_write(0x284, h_out_size + (v_out_size << 16));  // image width by bytes for wdma
	camera_reg_write(0x288, h_out_size + (v_out_size << 16));  // image width by bytes for wdma
	camera_reg_write(0x248, img_width << 16);		   // Pipe 0 ROI X
	camera_reg_write(0x258, img_width + (img_height << 16));   // Pipe 0 SCALER IN SIZE
	camera_reg_write(0x25c, h_out_size + (v_out_size << 16));  // Pipe 0 SCALER OUT SIZ
	camera_reg_write(0x268, img_width + (img_height << 16));   // Pipe 1 SCALER IN SIZE
	camera_reg_write(0x26c, h_out_size + (v_out_size << 16));  // Pipe 1 SCALER OUT SIZE
	camera_reg_write(0x24c, img_height << 16);		   // Pipe 0 ROI Y
	camera_reg_write(0x250, img_width << 16);		   // Pipe 1 ROI X
	camera_reg_write(0x254, img_height << 16);		   // Pipe 1 ROI Y
	camera_reg_write(0x260, h_delta_ph + (h_init_ph << 24));   // Pipe 0 SCALER H phase
	camera_reg_write(0x264, v_delta_ph + (v_init_ph << 24));   // Pipe 0 SCALER V phase
	camera_reg_write(0x270, h_delta_ph + (h_init_ph << 24));   // Pipe 1 SCALER H phase
	camera_reg_write(0x274, v_delta_ph + (v_init_ph << 24));   // Pipe 1 SCALER V phase
	camera_reg_write(0x278, 0x00002);			   // Scaler/Subsample mode [15:12] sel scl_coef_table default value
	camera_reg_write(0x280, (img_width << 16) + 0x8050);       // [7:0] min hblank [26:16] dvp fifo trigger

	// george:config ISP
	data_tmp = img_height + 6;
	isp_reg_write(0x34, (data_tmp >> 8) & 0x7);   // CISCTL_win_height
	isp_reg_write(0x38, data_tmp & 0xff);	 // CISCTL_win_height
	isp_reg_write(0x3c, (img_width >> 8) & 0x7);  // CISCTL_win_width
	isp_reg_write(0x40, img_width & 0xff);	// CISCTL_win_width
	isp_reg_write(0x5c, 0x10);		      //[5:4] CFA,   00:GRBG,   01:RGGB,   10:BGGR,   11:GBRG
	isp_reg_write(0x210, 0x00);		      // [4] is_yuv422  [3:0] byapss mode,   0:YUV,   5:SPI YUV422,   6:RGB
	isp_reg_write(0x234, 0x03);		      // debug_mode3
	data_tmp = img_width;
	isp_reg_write(0x250, 0x02);		       // out_win_x1 Low
	isp_reg_write(0x25c, (img_width >> 8) & 0x7);  // out window width
	isp_reg_write(0x260, img_width & 0xff);	// out window width
}


#if 1

int camera_online(uint_8 test_case)
{
	struct camera_config camera_cfg;

    init_PMU();



	if (camear_sensor_init() < 0) {
		bbu_printf("camera_online: camear_sensor_init fail, exit test!!\n");
		return -1;
	}

    camera_irq_init();
    camera_stream_off();
    run_camera_1();
    camera_stream_on();

	return 0;
}

int zebu_camera_online(uint_8 test_case)
{
	struct camera_config camera_cfg;

	init_PMU();
	bbu_printf("zebu camera_online start\r\n");

	camera_irq_init();
	bare_run_camera_mipi();

	return 0;
}

#else
int camera_online(uint_8 test_case)
{
	struct camera_config camera_cfg;

    pmu_reg_write(0x4c, 0x7003f); // refer to ap-pmu register map
    pmu_reg_write(0x50, 0xc1c1033f);// refer to ap-pmu register map



	if (camear_sensor_init() < 0) {
		bbu_printf("camera_online: camear_sensor_init fail, exit test!!\n");
		return -1;
	}

#if 0
    set_yuv_addr();


#else
    camera_irq_init();

	camera_stream_off();
	camera_cfg = get_camera_config(test_case);
	camera_run(camera_cfg);
	camera_stream_on();
#endif
	return 0;
}
#endif

int camera_online_irq(uint_8 test_case)
{
	struct camera_config camera_cfg;
      init_PMU();

	if (camear_sensor_init() < 0) {
		bbu_printf("camera_online: camear_sensor_init fail, exit test!!\n");
		return -1;
	}
        camera_irq_init();

	camera_stream_off();
	camera_cfg = get_camera_config(test_case);
	camera_run(camera_cfg);
	camera_stream_on();
	return 0;
}

int init_PMU()
{
    pmu_reg_write(0x4c, 0x7003f); // refer to ap-pmu register map
    pmu_reg_write(0x50, 0xc1c1033f);// refer to ap-pmu register map

}
int camear_sensor_init(){
	return gc030a_sensor_init();
}

int camear_sensor_read_id(){
	return gc030a_sensor_read_id();
}

int camera_reg_read_write()
{
    uint_32 i = 0;
    uint_32 value = 0;

    init_PMU();

    while (value < 0x2a4) {
        value += 4;
        camera_reg_write(value, 0xffffffff);
    }
    bbu_printf("camera_reg_read_write  ----------double check------- value  = 0x%x\n\r", value );

    value = 0;
    while (value < 0x2a4) {
        value += 4;
        camera_reg_write(value, 0x00000000);

    }
    bbu_printf("camera_reg_read_write  ----------camera reg check over-------\n\r");


    value = 0;
    while (value < 0x02D8) {
        value += 4;
        isp_reg_write(value, 0xffffffff);
    }
    bbu_printf("camera_reg_read_write  ----0~0x02D8----isp_reg_write  0xffffffff------- value = 0x%x\n\r", value);


    value = 0;
    while (value < 0x02D8) {
        value += 4;
        isp_reg_write(value, 0x00000000);

    }
    bbu_printf("camera_reg_read_write  -----0~0x02D8---isp_reg_write  0x00000000------- value = 0x%x\n\r", value);

    value = 0x03B0;
    while (value < 0x0BA0) {
        value += 4;
        isp_reg_write(value, 0xffffffff);
    }
    bbu_printf("camera_reg_read_write  ---0x03B0~ 0x0BA0-------isp_reg_write ------- value = 0x%x\n\r", value);

    value = 0x03B0;
    while (value < 0x0BA0) {
        value += 4;
        isp_reg_write(value, 0x00000000);
    }
    bbu_printf("camera_reg_read_write  ----0x03B0~ 0x0BA0-----isp_reg_write ------- value = 0x%x\n\r", value);


}
