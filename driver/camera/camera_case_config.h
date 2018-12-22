
struct camera_config cur_camera_cfg; /*used by case*/

struct camera_test_struct camera_test_case[] = {
	{
		.test_type = RAW8_SPI_2LAN_NORMAL,
		{
            .sensor_width = 640,
            .sensor_height = 480,
			.sensor_formt = RAW8,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
			.camera_interface = 0x1,   //0x0:spi1lan  0x1:spi2lan  0x2:spi4lan   0x3:mipi1lan  0x4:mipi2lan  0x5:mipi4lan  0x6:dvp
			.isp_bypass = 0x0,  //0x0:no bypass   0x1:bypass
            .isp_aec = 0,    //0x00:not use isp aec   0x01:use isp aec
            .isp_i2c_control = 0,   //0x00:camera control i2c    0x01:isp control i2c
            .isp_raw_parten = 0x10, //00: GRBG    0x01: RGGB    0x10: BGGR    0x11: GBRG
            .isp_color_bar = 0,
            .isp_crop = {
                .start_x = 0,
                .start_y = 0,
                .end_x = 640,
                .end_y = 480,
            },
            .spi_config = {
                .spi_sdr = 0,  //0x0:sdr   0x1:ddr
                .spi_crc = 1,  //0x0:no crc   0x1:crc
                .spi_manual_enable = 0,  //0x0:not enable   0x1:enable
                .spi_manual_mode = 0, 
                .spi_manual_height_enable = 0,  //0x0:not enable   0x1:enable
                .spi_manual_width_enable = 0,  //0x0:not enable   0x1:enable
                .spi_manual_height = 0,
                .spi_manual_width = 0,
                .spi_ignore_line_id = 1,
            },
            .pipe0_config =  {
                .pipeline_num = 0,  //0x0:preview   0x1:video    0x2:capture    
                .pipeline_enable = 1,  //0x0:not enable   0x1:enable
                .shadow_mode = 0,  //0x0:direct mode   0x1:shadow mode
                .pipeline_crop = {
                    .start_x = 0,
                    .start_y = 0,
                    .end_x = 320,
                    .end_y = 240,
                },
                .subsample = 0,  //0x0:not enable   0x1:enable
                .scaler = NO_SCALER,   // 1/4:SCALER_QUARTER 0x0       1/2:SCALER_HALF    1:NO_SCALER   2:SCALER_2   4:SCALER_4
                .jpeg_mode = 0,  //0x0:not jpeg_mode   0x1:jpeg_mode
                .output_format = YUV420_SEMI,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
            },
            .pipe1_config =  {
                .pipeline_num = 1,  //0x0:preview   0x1:video    0x2:capture    
                .pipeline_enable = 0,  //0x0:not enable   0x1:enable
                .shadow_mode = 0,  //0x0:direct mode   0x1:shadow mode
                .pipeline_crop = {
                    .start_x = 0,
                    .start_y = 0,
                    .end_x = 640,
                    .end_y = 480,
                },
                .subsample = 0,  //0x0:not enable   0x1:enable
                .scaler = NO_SCALER,  // 1/4:SCALER_QUARTER 0x0       1/2:SCALER_HALF    1:NO_SCALER   2:SCALER_2   4:SCALER_4
                .jpeg_mode = 0,  //0x0:not jpeg_mode   0x1:jpeg_mode
                .output_format = YUV420_SEMI,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
            },

            .pipe2_config =  {
                .pipeline_num = 2,  //0x0:preview   0x1:video    0x2:capture    
                .pipeline_enable = 0,  //0x0:not enable   0x1:enable
                .shadow_mode = 0,  //0x0:direct mode   0x1:shadow mode
                .pipeline_crop = {
                    .start_x = 0,
                    .start_y = 0,
                    .end_x = 640,
                    .end_y = 480,
                },
                .subsample = 0,  //0x0:not enable   0x1:enable
                .scaler = NO_SCALER,  // 1/4:SCALER_QUARTER 0x0       1/2:SCALER_HALF    1:NO_SCALER   2:SCALER_2   4:SCALER_4
                .jpeg_mode = 0,  //0x0:not jpeg_mode   0x1:jpeg_mode
                .output_format = YUV420_SEMI,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
            },

		},
	},

};
struct camera_config camera_cfg_p0_online_shadow ={
		.sensor_width = 640,
		.sensor_height = 480,
		.sensor_formt = RAW8,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
		.camera_interface = 0x1,   //0x0:spi1lan  0x1:spi2lan  0x2:spi4lan   0x3:mipi1lan  0x4:mipi2lan  0x5:mipi4lan  0x6:dvp
		.isp_bypass = 0x0,  //0x0:no bypass   0x1:bypass
		.isp_aec = 0,    //0x00:not use isp aec   0x01:use isp aec
		.isp_i2c_control = 0,   //0x00:camera control i2c    0x01:isp control i2c
		.isp_raw_parten = 0x10, //00: GRBG    0x01: RGGB    0x10: BGGR    0x11: GBRG
		.isp_color_bar = 0,
		.isp_crop = {
			.start_x = 0,
			.start_y = 0,
			.end_x = 640,
			.end_y = 480,
		},
		.spi_config = {
			.spi_sdr = 0,  //0x0:sdr   0x1:ddr
			.spi_crc = 1,  //0x0:no crc   0x1:crc
			.spi_manual_enable = 0,  //0x0:not enable   0x1:enable
			.spi_manual_mode = 0, 
			.spi_manual_height_enable = 0,  //0x0:not enable   0x1:enable
			.spi_manual_width_enable = 0,  //0x0:not enable   0x1:enable
			.spi_manual_height = 0,
			.spi_manual_width = 0,
			.spi_ignore_line_id = 1,
		},
		.pipe0_config =  {
			.pipeline_num = 0,  //0x0:preview   0x1:video    0x2:capture    
			.pipeline_enable = 1,  //0x0:not enable   0x1:enable
			.shadow_mode = 1,  //0x0:direct mode   0x1:shadow mode
			.pipeline_crop = {
				.start_x = 0,
				.start_y = 0,
				.end_x = 320,
				.end_y = 240,
			},
			.subsample = 0,  //0x0:not enable   0x1:enable
			.scaler = NO_SCALER,   // 1/4:SCALER_QUARTER 0x0       1/2:SCALER_HALF    1:NO_SCALER   2:SCALER_2   4:SCALER_4
			.jpeg_mode = 0,  //0x0:not jpeg_mode   0x1:jpeg_mode
			.output_format = YUV420_SEMI,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
		},
		.pipe1_config =  {
			.pipeline_num = 1,  //0x0:preview   0x1:video    0x2:capture    
			.pipeline_enable = 0,  //0x0:not enable   0x1:enable
			.shadow_mode = 0,  //0x0:direct mode   0x1:shadow mode
			.pipeline_crop = {
				.start_x = 0,
				.start_y = 0,
				.end_x = 640,
				.end_y = 480,
			},
			.subsample = 0,  //0x0:not enable   0x1:enable
			.scaler = NO_SCALER,  // 1/4:SCALER_QUARTER 0x0       1/2:SCALER_HALF    1:NO_SCALER   2:SCALER_2   4:SCALER_4
			.jpeg_mode = 0,  //0x0:not jpeg_mode   0x1:jpeg_mode
			.output_format = YUV420_SEMI,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
		},

		.pipe2_config =  {
			.pipeline_num = 2,  //0x0:preview   0x1:video    0x2:capture    
			.pipeline_enable = 0,  //0x0:not enable   0x1:enable
			.shadow_mode = 0,  //0x0:direct mode   0x1:shadow mode
			.pipeline_crop = {
				.start_x = 0,
				.start_y = 0,
				.end_x = 640,
				.end_y = 480,
			},
			.subsample = 0,  //0x0:not enable   0x1:enable
			.scaler = NO_SCALER,  // 1/4:SCALER_QUARTER 0x0       1/2:SCALER_HALF    1:NO_SCALER   2:SCALER_2   4:SCALER_4
			.jpeg_mode = 0,  //0x0:not jpeg_mode   0x1:jpeg_mode
			.output_format = YUV420_SEMI,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
		}
	};

    struct camera_config camera_cfg_p1_online_shadow ={
            .sensor_width = 640,
            .sensor_height = 480,
            .sensor_formt = RAW8,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
            .camera_interface = 0x1,   //0x0:spi1lan  0x1:spi2lan  0x2:spi4lan   0x3:mipi1lan  0x4:mipi2lan  0x5:mipi4lan  0x6:dvp
            .isp_bypass = 0x0,  //0x0:no bypass   0x1:bypass
            .isp_aec = 0,    //0x00:not use isp aec   0x01:use isp aec
            .isp_i2c_control = 0,   //0x00:camera control i2c    0x01:isp control i2c
            .isp_raw_parten = 0x10, //00: GRBG    0x01: RGGB    0x10: BGGR    0x11: GBRG
            .isp_color_bar = 0,
            .isp_crop = {
                .start_x = 0,
                .start_y = 0,
                .end_x = 640,
                .end_y = 480,
            },
            .spi_config = {
                .spi_sdr = 0,  //0x0:sdr   0x1:ddr
                .spi_crc = 1,  //0x0:no crc   0x1:crc
                .spi_manual_enable = 0,  //0x0:not enable   0x1:enable
                .spi_manual_mode = 0, 
                .spi_manual_height_enable = 0,  //0x0:not enable   0x1:enable
                .spi_manual_width_enable = 0,  //0x0:not enable   0x1:enable
                .spi_manual_height = 0,
                .spi_manual_width = 0,
                .spi_ignore_line_id = 1,
            },
            .pipe0_config =  {
                .pipeline_num = 0,  //0x0:preview   0x1:video    0x2:capture    
                .pipeline_enable = 0,  //0x0:not enable   0x1:enable
                .shadow_mode = 0,  //0x0:direct mode   0x1:shadow mode
                .pipeline_crop = {
                    .start_x = 0,
                    .start_y = 0,
                    .end_x = 320,
                    .end_y = 240,
                },
                .subsample = 0,  //0x0:not enable   0x1:enable
                .scaler = NO_SCALER,   // 1/4:SCALER_QUARTER 0x0       1/2:SCALER_HALF    1:NO_SCALER   2:SCALER_2   4:SCALER_4
                .jpeg_mode = 0,  //0x0:not jpeg_mode   0x1:jpeg_mode
                .output_format = YUV420_SEMI,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
            },
            .pipe1_config =  {
                .pipeline_num = 1,  //0x0:preview   0x1:video    0x2:capture    
                .pipeline_enable = 1,  //0x0:not enable   0x1:enable
                .shadow_mode = 1,  //0x0:direct mode   0x1:shadow mode
                .pipeline_crop = {
                    .start_x = 0,
                    .start_y = 0,
                    .end_x = 320,
                    .end_y = 240,
                },
                .subsample = 0,  //0x0:not enable   0x1:enable
                .scaler = NO_SCALER,  // 1/4:SCALER_QUARTER 0x0       1/2:SCALER_HALF    1:NO_SCALER   2:SCALER_2   4:SCALER_4
                .jpeg_mode = 0,  //0x0:not jpeg_mode   0x1:jpeg_mode
                .output_format = YUV420_SEMI,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
            },
    
            .pipe2_config =  {
                .pipeline_num = 2,  //0x0:preview   0x1:video    0x2:capture    
                .pipeline_enable = 0,  //0x0:not enable   0x1:enable
                .shadow_mode = 0,  //0x0:direct mode   0x1:shadow mode
                .pipeline_crop = {
                    .start_x = 0,
                    .start_y = 0,
                    .end_x = 640,
                    .end_y = 480,
                },
                .subsample = 0,  //0x0:not enable   0x1:enable
                .scaler = NO_SCALER,  // 1/4:SCALER_QUARTER 0x0       1/2:SCALER_HALF    1:NO_SCALER   2:SCALER_2   4:SCALER_4
                .jpeg_mode = 0,  //0x0:not jpeg_mode   0x1:jpeg_mode
                .output_format = YUV420_SEMI,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
            }
        };
    struct camera_config camera_cfg_p2_online_shadow ={
               .sensor_width = 640,
               .sensor_height = 480,
               .sensor_formt = RAW8,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
               .camera_interface = 0x1,   //0x0:spi1lan  0x1:spi2lan  0x2:spi4lan   0x3:mipi1lan  0x4:mipi2lan  0x5:mipi4lan  0x6:dvp
               .isp_bypass = 0x0,  //0x0:no bypass   0x1:bypass
               .isp_aec = 0,    //0x00:not use isp aec   0x01:use isp aec
               .isp_i2c_control = 0,   //0x00:camera control i2c    0x01:isp control i2c
               .isp_raw_parten = 0x10, //00: GRBG    0x01: RGGB    0x10: BGGR    0x11: GBRG
               .isp_color_bar = 0,
               .isp_crop = {
                   .start_x = 0,
                   .start_y = 0,
                   .end_x = 640,
                   .end_y = 480,
               },
               .spi_config = {
                   .spi_sdr = 0,  //0x0:sdr   0x1:ddr
                   .spi_crc = 1,  //0x0:no crc   0x1:crc
                   .spi_manual_enable = 0,  //0x0:not enable   0x1:enable
                   .spi_manual_mode = 0, 
                   .spi_manual_height_enable = 0,  //0x0:not enable   0x1:enable
                   .spi_manual_width_enable = 0,  //0x0:not enable   0x1:enable
                   .spi_manual_height = 0,
                   .spi_manual_width = 0,
                   .spi_ignore_line_id = 1,
               },
               .pipe2_config =  {
                   .pipeline_num = 2,  //0x0:preview   0x1:video    0x2:capture    
                   .pipeline_enable = 1,  //0x0:not enable   0x1:enable
                   .shadow_mode = 0,  //0x0:direct mode   0x1:shadow mode
                   .pipeline_crop = {
                       .start_x = 0,
                       .start_y = 0,
                       .end_x = 640,
                       .end_y = 480,
                   },
                   .subsample = 0,  //0x0:not enable   0x1:enable
                   .scaler = NO_SCALER,  // 1/4:SCALER_QUARTER 0x0       1/2:SCALER_HALF    1:NO_SCALER   2:SCALER_2   4:SCALER_4
                   .jpeg_mode = 0,  //0x0:not jpeg_mode   0x1:jpeg_mode
                   .output_format = YUV420_SEMI,   //0x0:RAW8  0x1:RAW10  0x2:RAW12  0x3:YUV444   0x4:YUV422   0x5:YUV420   0x6:YUV420_SEMI  
               }
           };

