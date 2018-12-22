#include "ssp_gd5f.h"

Q_DEFINE_THIS_MODULE("ssp_gd5f.c")

static void ssp_erase_page_gd5f(uint32_t offset){
	//write enable before erase operation.
	ssp_write_enable_gd5f();
	//erase command.
	uint8_t cmd[4];
	cmd[0] = SSP_ERASE_COMMAND_GD5F;
	cmd[1] = 0;
	cmd[2] = (offset>>8)&0xFF;
	cmd[3] = offset&0xC0;
	ssp_xfer(8*4, NULL, cmd, SSP_XFER_BE);
	//wait for erase operation finish.
	int stat_chk = ssp_wait_ready_gd5f();
	if(!stat_chk){
		bbu_printf("SSP: status error!\r\n");
	}
	//mdelay(1);//Need delay to finish erase operation.
	return;
}

static void ssp_read_page_gd5f(uint32_t offset, uint8_t *buf){
	//page read to cache.
	uint8_t cmd1[4];
	cmd1[0] = SSP_PAGE_READ_TCACHE_COMMAND_GD5F;
	cmd1[1] = 0;
	cmd1[2] = (offset>>8)&0xFF;
	cmd1[3] = offset&0xFF;
	ssp_xfer(8*4, NULL, cmd1, SSP_XFER_BE);

	int stat_chk = ssp_wait_ready_gd5f();
	if(!stat_chk){
		bbu_printf("SSP: wait status error!\r\n");
		goto error;
	}
	//read data from cache.
	uint8_t cmd2[4];
	cmd2[0] = SSP_PAGE_READ_FCACHE_COMMAND_GD5F;
	//We read data from the 0x00 address in cache.
	cmd2[1] = 0;
	cmd2[2] = 0;
	cmd2[3] = 0xFF;
	ssp_xfer(8*4, NULL, cmd2, SSP_XFER_BEGIN);
	//read data to memory.
	ssp_xfer(8*2048, buf, NULL, SSP_XFER_END);

	return;

	error:
		bbu_printf("SSP: DMA read data error\r\n");

		return;
}

static void ssp_write_page_gd5f(uint32_t offset, uint8_t *buf){
	//ssp_unlock_gd5f();
	//ssp_erase_page_gd5f(offset);

	//send the command to write data to cache.
	uint8_t cmd1[3];
	cmd1[0] = SSP_PAGE_WRITE_TCACHE_CONMMAND_GD5F;
	//start by 0x00 address in cache.
	cmd1[1] = 0;
	cmd1[2] = 0;
	ssp_xfer(8*3, NULL, cmd1, SSP_XFER_BEGIN);

	ssp_xfer(8*2048, NULL, buf, SSP_XFER_END);

	ssp_write_enable_gd5f();

	//write data to flash.
	uint8_t cmd2[4];
	cmd2[0] = SSP_PAGE_WRITE_TFLASH_CONMMAND_GD5F;
	cmd2[1] = 0;
	cmd2[2] = (offset>>8)&0xFF;
	cmd2[3] = offset&0xFF;
	ssp_xfer(8*4, NULL, cmd2, SSP_XFER_BE);

	int stat_chk = ssp_wait_ready_gd5f();
	if(!stat_chk){
		bbu_printf("SSP: status error!\r\n");
	}

	return;
}

static void ssp_test_help(){
	bbu_printf("\n\rssp - Command to test ssp.");
	bbu_printf("\r\nUsage: ssp <command> <mode> ...\r\n");
	bbu_printf("    <command>: \n\r"
		       "      stress  -- stress test\n\r");
	bbu_printf("      speed   -- speed test\n\r");
	bbu_printf("      erase   -- erase operation\n\r");
	bbu_printf("      read   -- read operation\n\r");
	bbu_printf("      write   -- write operation\n\r");
	bbu_printf("    <mode>: \n\r"
		       "      dma  -- dma mode\n\r");
	bbu_printf("      udma   -- normal mode\n\r");
	bbu_printf("===================================================================\n\r");
	bbu_printf("      ssp stress <mode> <iteration >\n\r");
	bbu_printf("      ssp speed <mode> <R/W> 1\n\r");
	bbu_printf("          <R/W>: \n\r"
		       "          0  -- write test\n\r");
	bbu_printf("          1  -- read test\n\r");
	bbu_printf("      ssp erase <offset> <block number>\n\r");
	bbu_printf("      ssp read/write <mode> <offset> <memory address> <data length>\n\r");

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void ssp_init_gd5f(uint32_t clk){
	ssp_dec_init(clk, SSP_DATA_LEN_8);
	ssp_unlock_gd5f();
	//ssp_erase_page_gd5f(0);//erase block 0.

	return;
}

void ssp_write_enable_gd5f(){
	uint8_t cmd;
	cmd = SSP_WRITE_ENABLE_COMMAND_GD5F;
	ssp_xfer(8, NULL, &cmd, SSP_XFER_BE);

	return;
}

uint8_t ssp_get_feature_gd5f(uint8_t addr){
	uint8_t cmd[3];
	uint8_t stat[3];

	cmd[0] = SSP_GET_FEATURE_COMMAND_GD5F;
	cmd[1] = addr;
	cmd[2] = 0;
	ssp_xfer(8*3, stat, cmd, SSP_XFER_BE);
	//bbu_printf("The %02X status is %02X\r\n", addr, *status);
	//udelay(50);//We need delay in the end of operation.

	return stat[2];
}

void ssp_set_feature_gd5f(uint8_t status, uint8_t addr){
	uint8_t cmd[3];
	uint8_t stat[3];

	cmd[0] = SSP_SET_FEATURE_COMMAND_GD5F;
	cmd[1] = addr;
	cmd[2] = 0;
	ssp_xfer(8*3, stat, cmd, SSP_XFER_BE);
	status = stat[2];
	(void)status;//avoid warning.
	bbu_printf("The %02X status is %02X\r\n", addr, status);

	return;
}

void ssp_reset_gd5f(){
	uint8_t cmd;
	cmd = SSP_RESET_COMMAND_GD5F;
	ssp_xfer(8, NULL, &cmd, SSP_XFER_BE);

	return;
}

void ssp_unlock_gd5f(){
	uint8_t cmd[3];
	cmd[0] = SSP_SET_FEATURE_COMMAND_GD5F;
	cmd[1] = 0xA0;
	cmd[2] = 0;
	ssp_xfer(8*3, NULL, cmd, SSP_XFER_BE);
	mdelay(1);

	return;
}

int ssp_wait_ready_gd5f(){
	int ret = 1;
	uint8_t stat = 0;
	uint8_t flag = 1;
	uint32_t retry = SSP_TIMEOUT_MAX;
	while(flag&&retry--){
		stat = ssp_get_feature_gd5f(0xC0);
		//check operation result.
		if(stat&(BIT_2|BIT_3)){
			flag = 0;
			ret = 0;
			bbu_printf("SSP: status error! status = %02X\r\n", stat);
		}

		if(!(stat&BIT_0))
			flag = 0;
		//udelay(1);
	}

	if(!retry)
		ret = 0;

	return ret;
}

void ssp_read_id_gd5f(){
	uint8_t cmd[4];
	uint8_t id[4];
	uint32_t val;

	cmd[0] = SSP_READID_COMMAND_GD5F;
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = 0;
	ssp_xfer(8*4, id, cmd, SSP_XFER_BE);
	ssp_xfer(8*4, (uint8_t*)(&val), cmd, SSP_XFER_BE);
	
	bbu_printf("The value is 0x%08X\r\n", val);
	bbu_printf("The manufacture id is %02X\r\n", id[2]);
	bbu_printf("The device id is %02X\r\n", id[3]);

	return;
}

void ssp_erase_gd5f(uint32_t offset, uint32_t len){
	uint32_t k = len;
	while(k--){
		ssp_erase_page_gd5f(offset);
		offset = offset+0x40;
	}

	return;
}

void ssp_write_tail_gd5f(uint32_t offset, 
						 uint32_t start_addr, 
						 uint32_t len, 
						 uint8_t *buf){
	uint8_t* data_temp = NULL;
	data_temp = bbu_nc_malloc(2048);
	if((start_addr+len) > SSP_BYTES_PER_PAGE_GD5F){
		bbu_printf("SSP: start address and data lengh overflow\r\n");
		goto error;
	}

	ssp_read_page_gd5f(offset, data_temp);
	uint32_t i = 0;
	for(i=0; i<len; i++){
		data_temp[start_addr+i] = buf[i];
	}
	ssp_write_page_gd5f(offset, data_temp);

	bbu_mfree(data_temp);
	return;

	error:
		bbu_mfree(data_temp);
		return;
}

void ssp_read_tail_gd5f(uint32_t offset, 
						uint32_t start_addr, 
						uint32_t len, 
						uint8_t *buf){
	uint8_t* data_temp = NULL;
	data_temp = bbu_nc_malloc(2048);
	if((start_addr+len) > SSP_BYTES_PER_PAGE_GD5F){
		bbu_printf("SSP: start address and data lengh overflow\r\n");
		goto error;
	}

	ssp_read_page_gd5f(offset, data_temp);
	uint32_t i = 0;
	for(i=0; i<len; i++){
		buf[i] = data_temp[start_addr+i];
	}

	bbu_mfree(data_temp);
	return;

	error:
		bbu_mfree(data_temp);
		return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void ssp_write_gd5f(uint32_t offset, 
					uint32_t start_addr, 
					uint32_t len, 
					uint8_t *buf){
	int pages = 0;
	int tail = 0;
	if(start_addr != 0){
		pages = (len+start_addr-SSP_BYTES_PER_PAGE_GD5F)/SSP_BYTES_PER_PAGE_GD5F;
		tail = (len+start_addr-SSP_BYTES_PER_PAGE_GD5F)%SSP_BYTES_PER_PAGE_GD5F;
	}else{
		pages = len/SSP_BYTES_PER_PAGE_GD5F;
		tail = len%SSP_BYTES_PER_PAGE_GD5F;
	}
	int i = 0;
	uint32_t length = len;
	if(start_addr != 0){
		ssp_write_tail_gd5f(offset, start_addr, SSP_BYTES_PER_PAGE_GD5F-start_addr, buf);
		buf = buf+SSP_BYTES_PER_PAGE_GD5F-start_addr;
		offset++;
		length = length+start_addr-SSP_BYTES_PER_PAGE_GD5F;
	}
	for(i=0; i<pages; i++){
		ssp_write_page_gd5f(offset, buf);
		buf = buf+SSP_BYTES_PER_PAGE_GD5F;//point the next page memory.
		offset++;
		length = length-SSP_BYTES_PER_PAGE_GD5F;
	}
	if(tail != 0)
		ssp_write_tail_gd5f(offset, 0, length, buf);

	return;
}

void ssp_read_gd5f(uint32_t offset, 
					uint32_t start_addr, 
					uint32_t len, 
					uint8_t *buf){
	int pages = 0;
	int tail = 0;
	if(start_addr != 0){
		pages = (len+start_addr-SSP_BYTES_PER_PAGE_GD5F)/SSP_BYTES_PER_PAGE_GD5F;
		tail = (len+start_addr-SSP_BYTES_PER_PAGE_GD5F)%SSP_BYTES_PER_PAGE_GD5F;
	}else{
		pages = len/SSP_BYTES_PER_PAGE_GD5F;
		tail = len%SSP_BYTES_PER_PAGE_GD5F;
	}

	int i = 0;
	uint32_t length = len;
	if(start_addr != 0){
		ssp_read_tail_gd5f(offset, start_addr, SSP_BYTES_PER_PAGE_GD5F-start_addr, buf);
		buf = buf+SSP_BYTES_PER_PAGE_GD5F-start_addr;
		offset++;
		length = length+start_addr-SSP_BYTES_PER_PAGE_GD5F;
	}
	for(i=0; i<pages; i++){
		ssp_read_page_gd5f(offset, buf);
		buf = buf+SSP_BYTES_PER_PAGE_GD5F;//point the next page memory.
		offset++;
		length = length-SSP_BYTES_PER_PAGE_GD5F;
	}
	if(tail != 0)
		ssp_read_tail_gd5f(offset, 0, length, buf);

	return;
}

void ssp_stress_test_gd5f(uint32_t count){
	uint8_t* data_tra = NULL;
	uint8_t* data_temp = NULL;
	data_tra = bbu_nc_malloc(SSP_STRESS_TEST_BYTES);
	data_temp = bbu_nc_malloc(SSP_STRESS_TEST_BYTES);

	int i = 0;
	int j = 0;
	for(i=0; i<SSP_STRESS_TEST_BYTES; i++){
		data_tra[i] = 0x12;
		data_temp[i] = 0x12;
	}
	uint32_t k = 0;
	for(k=0; k<count; k++){
		ssp_erase_gd5f(0, 1);
		ssp_write_gd5f(0, 0, SSP_STRESS_TEST_BYTES, data_tra);
		memset(data_tra, 0, SSP_STRESS_TEST_BYTES);
		ssp_read_gd5f(0, 0, SSP_STRESS_TEST_BYTES, data_tra);

		for(j=0; j<SSP_STRESS_TEST_BYTES; j++){
			//bbu_printf("data_temp = %02X\r\ndata_tra = %02X\r\n", data_temp[j], data_tra[j]);
			if(data_temp[j] != data_tra[j]){
				bbu_printf("SSP: stress test error\r\n");
				bbu_mfree(data_tra);
				bbu_mfree(data_temp);
				Q_ASSERT(0);
			}
		}
		bbu_printf(".");
	}
	
	bbu_printf("\r\nSSP: stress test success\r\n");
	bbu_mfree(data_tra);
	bbu_mfree(data_temp);

	return;
}

void ssp_speed_test(uint32_t mode, uint32_t test_count){
	uint32_t i = 0;
	uint32_t interval = 0;
	uint32_t t = 0;
	uint8_t* data_tra = NULL;
	data_tra = bbu_nc_malloc(SSP_STRESS_TEST_BYTES);
	switch(mode){
		case SSP_TEST_MODE_W:{
			interval = timer_get_count(0);
			for(i=0; i<test_count; i++){
				ssp_write_gd5f(0, 0, SSP_STRESS_TEST_BYTES, data_tra);
			}
			t = timer_get_count(0) - interval;
			float speed_w = SSP_BYTES_PER_BLOCK_GD5F*test_count*3250000/t;
			bbu_printf("SSP: write speed is %.2fKbyte/s\r\n", speed_w);
			break;
		}
		case SSP_TEST_MODE_R:{
			interval = timer_get_count(0);
			for(i=0; i<test_count; i++){
				ssp_read_gd5f(0, 0, SSP_STRESS_TEST_BYTES, data_tra);
			}
			t = timer_get_count(0) - interval;
			float speed_r = SSP_BYTES_PER_BLOCK_GD5F*test_count*3250000/t;
			bbu_printf("SSP: read speed is %.2fKbyte/s\r\n", speed_r);
			break;
		}
		default:
			break;
	}
	bbu_mfree(data_tra);

	return;
}

void ssp_test_gd5f(int argc, uint8_t **argv){
	switch(argc){
		case 1:
		case 2:{
			if(!strcmp((char*)argv[1], "id")){
				ssp_read_id_gd5f();
				break;
			}
			ssp_test_help();
			break;
		}
		case 3:{
			if(!strcmp((char*)argv[1], "stress")){
				uint32_t stress_count = conv_dec((char*)argv[2]);
				ssp_stress_test_gd5f(stress_count);
			}
			break;
		}
		case 4:{
			if(!strcmp((char*)argv[1], "stress")){
				if(!strcmp((char*)argv[2], "dma")){
		            uint32_t count = conv_dec((char*)argv[3]);
					ssp_stress_test_dma_gd5f(count);
				}
				else if(!strcmp((char*)argv[2], "udma")){
                	uint32_t count = conv_dec((char*)argv[3]);
					ssp_stress_test_gd5f(count);
				}else{
					ssp_test_help();
				}
			break;
			}
			else if(!strcmp((char*)argv[1], "erase")){
					uint32_t offset = conv_hex((char*)argv[2]);
					uint32_t len = conv_hex((char*)argv[3]);
					ssp_erase_gd5f(offset, len);
					bbu_printf("SSP: erase from block %d total %d\r\n", offset, len);

					break;
			}
		}
		case 5:{
			if(!strcmp((char*)argv[1], "speed")){
				if(!strcmp((char*)argv[2], "dma")){
					uint32_t mode = conv_dec((char*)argv[3]);
		            uint32_t count = conv_dec((char*)argv[4]);
					ssp_speed_test_dma(mode, count);
				}
				else if(!strcmp((char*)argv[2], "udma")){
					uint32_t mode = conv_dec((char*)argv[3]);
                	uint32_t count = conv_dec((char*)argv[4]);
					ssp_speed_test(mode, count);
				}else{
					ssp_test_help();
				}
			break;
			}
		}
		case 6:{
			if(!strcmp((char*)argv[1], "read")){
				if(!strcmp((char*)argv[2], "dma")){
					uint32_t offset = conv_hex((char*)argv[3]);
                	uint32_t addr = conv_hex((char*)argv[4]);
					uint32_t len = conv_hex((char*)argv[5]);
					ssp_read_dma_gd5f(offset, len, (uint32_t*)addr);
					bbu_printf("SSP: read address %08X to memory %08X, total %d bytes\r\n", offset, addr, len);
				}
				else if(!strcmp((char*)argv[2], "udma")){
					uint32_t offset = conv_hex((char*)argv[3]);
                	uint32_t addr = conv_hex((char*)argv[4]);
					uint32_t len = conv_hex((char*)argv[5]);
					ssp_read_gd5f(offset, 0, len, (uint8_t*)addr);
					bbu_printf("SSP: read address %08X to memory %08X, total %d bytes\r\n", offset, addr, len);
				}else{
					ssp_test_help();
				}
			break;
			}
			else if(!strcmp((char*)argv[1], "write")){
					if(!strcmp((char*)argv[2], "dma")){
						uint32_t offset = conv_hex((char*)argv[3]);
		            	uint32_t addr = conv_hex((char*)argv[4]);
						uint32_t len = conv_hex((char*)argv[5]);
						ssp_write_dma_gd5f(offset, len, (uint32_t*)addr);
						bbu_printf("SSP: write memory %08X to address %08X, total %d bytes\r\n", addr, offset, len);
					}
					else if(!strcmp((char*)argv[2], "udma")){
						uint32_t offset = conv_hex((char*)argv[3]);
		            	uint32_t addr = conv_hex((char*)argv[4]);
						uint32_t len = conv_hex((char*)argv[5]);
						ssp_write_gd5f(offset, 0, len, (uint8_t*)addr);
						bbu_printf("SSP: write memory %08X to address %08X, total %d bytes\r\n", addr, offset, len);
					}else{
						ssp_test_help();
					}
				break;
				}
		}
		default:
			ssp_test_help();
			break;
	}

	return;
}







