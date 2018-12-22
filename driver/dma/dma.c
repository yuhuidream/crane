#include "dma_dec.h"
#include "qspinlock.h"
#include "clock.h"

QSpinlock dma_stress_stop_lock = Q_SPIN_LOCK_INITIALIZER;

static DMA_STRESS_ARG arg_glob; //global variable for stress test.
static uint32_t dma_stress_iter;


//atomic operation to fill single descriptor.
static void _dma_config_desc(P_DMA_DESCRIPTOR p_des, 
							 P_DMA_DESCRIPTOR p_next_des, 
							 uint32_t stop, 
							 uint32_t branch_mode, 
							 uint32_t src, 
							 uint32_t trg, 
							 P_DMA_CMDx_T pCMD)
{
	uint32_t next_des_addr = (uint32_t)p_next_des;
	if(!stop){
		//clear the reserved bits.
		next_des_addr = next_des_addr & DMA_DDADR_RESERVED_MASK;
	}else{
		next_des_addr = 0;
	}

	next_des_addr |= ( (stop & 0x1) | ( (branch_mode&0x1) << 1) );

	//fill the descriptor entries.
	p_des->DDADR = (uint32_t)next_des_addr;
	p_des->DSADR = (uint32_t)src;
	p_des->DTADR = (uint32_t)trg;
	p_des->DCMD = (uint32_t)pCMD->value;

	return;
}

//no fetch mode.
void dma_set_reg_nf(uint32_t src, uint32_t trg, P_DMA_CMDx_T pCMD, uint32_t channel){
	DMA->DMA_DESC[channel].DSADR = src;
	DMA->DMA_DESC[channel].DTADR = trg;
	DMA->DMA_DESC[channel].DCMD = pCMD->value;

	return;
}

static void dma_irq_handler_default(){
	uint32_t t = 0;
	for(t=0; t<32; t++){
		//check irq channel
		if(DMA->DINT & (1<<t)){
			uint32_t dcsr_val = DMA->DCSR[t];
			DMA->DCSR[t] |= DMA_STATUS_INTERRUPTS_MASK;//clear all the interrupt.

			if(dcsr_val & CSR_BUSERRINTR){
				bbu_printf("DMA: bus error on channel %d\r\n", t);
			} 
					   
			if(dcsr_val & CSR_STARTINTR){
				bbu_printf("DMA: start int on channel %d\r\n", t);
			}
				
			if(dcsr_val & CSR_ENDINTR){
				bbu_printf("DMA: end int on channel %d\r\n", t);
				DMA->DCSR[t] &= ~CSR_EORIRQEN;
			}

			if(dcsr_val & CSR_STOPINTR){
				bbu_printf("DMA: stop int on channel %d\r\n", t);
				DMA->DCSR[t] &= ~CSR_STOPIRQEN;//disable the stop int.
			}
		}
	}

	return;	
}


#define DMA_TEST_STRESS_ITER_MAX		5000
static uint32_t dma_stress_iter_max_in_use = DMA_TEST_STRESS_ITER_MAX;
static void dma_irq_handler_stress(){
	uint32_t t = 0;
	for(t=0; t<32; t++){
		//check irq channel
		if(DMA->DINT & (1<<t)){
			uint32_t dcsr_val = DMA->DCSR[t];
			DMA->DCSR[t] |= DMA_STATUS_INTERRUPTS_MASK;//clear all the interrupt.

			if(dcsr_val & CSR_BUSERRINTR){
				bbu_printf("DMA: bus error on channel %d\r\n", t);
			}

			if(dcsr_val & CSR_STARTINTR){
				bbu_printf("DMA: start int on channel %d\r\n", t);
			}
				
			if(dcsr_val & CSR_ENDINTR){
				bbu_printf("DMA: end int on channel %d\r\n", t);
				DMA->DCSR[t] &= ~CSR_EORIRQEN;
			}

			if(dcsr_val & CSR_STOPINTR){
				arg_glob.mask_t |= (1<<t);
				DMA->DCSR[t] &= ~CSR_STOPIRQEN;//disable the stop int.
				//bbu_printf("DMA: stop int on channel %d\r\n", t);
				if(arg_glob.mask_t == arg_glob.mask){
					Q_SPIN_LOCK(&dma_stress_stop_lock);
					uint32_t stop = arg_glob.stop_flag;
					Q_SPIN_UNLOCK(&dma_stress_stop_lock);

					arg_glob.mask_t = 0;//clear the irq mask temp.
					
					if(!stop){
/*						DmaStressEvt *dse = Q_NEW(DmaStressEvt, DMA_STRESS_SIG);*/
/*						dse->channel = arg_glob.channel;*/
/*						dse->num = arg_glob.num;*/
/*						dse->size = arg_glob.size;*/
/*						QACTIVE_POST(AO_Dma, (QEvent *)dse, 0);*/
			if(dma_stress_iter>dma_stress_iter_max_in_use){
				dma_stress_iter = 0;
				bbu_printf("DMA stress test is running correctly\r\n");
			}else{
				dma_stress_iter++;
			}
						
						dma_stress_test(arg_glob.channel, arg_glob.num, arg_glob.size, 0);
					}else{
						Q_SPIN_LOCK(&dma_stress_stop_lock);
						arg_glob.stop_flag = 0; //reset dma_stress_stop value.
						Q_SPIN_UNLOCK(&dma_stress_stop_lock);

						arg_glob.mask = 0;

						//free the used memory.
						uint32_t i = 0;
						for(i=0; i<arg_glob.num; i++){
							bbu_mfree((void *)arg_glob.src[i]);
							bbu_mfree((void *)arg_glob.dst[i]);
							bbu_mfree((void *)arg_glob.des[i]);
						}
					}
				}
			}
		}
	}

	return;
}

//Check the DMA transfer success or fail.
//success 0   fail 1
static uint32_t dma_mem_cmp(uint32_t mem1, uint32_t mem2, uint32_t len){
	if(!memcmp((void *)mem1, (void *)mem2, len))
		return 0;

	return 1;	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Configure the descriptor entries.
 * p_des should be malloc ahead and the malloc size can be got by dma_get_des_num().
 * Descriptor, source and target address should align.
 */
void dma_config_desc(P_DMA_DESCRIPTOR p_des,
					 P_DMA_CMDx_T pCMD,
					 uint32_t src, 
					 uint32_t trg,
					 uint32_t bytes_len){
	uint32_t bytes_step = (uint32_t)pCMD->bits.Length;
	uint32_t des_num = bytes_len / bytes_step;
	uint32_t bytes_tail = bytes_len % bytes_step;
	if(bytes_tail != 0){
		des_num++;
	}else{
		bytes_tail = bytes_step;
	}

	uint32_t i = 0;
	uint32_t p_cur_des = (uint32_t)p_des;
	uint32_t p_next_des = 0;
	for(i=0; i<des_num-1; i++){
		p_cur_des = (uint32_t)p_des + i*sizeof(DMA_DESCRIPTOR);
		p_next_des = (uint32_t)p_des + (i+1)*sizeof(DMA_DESCRIPTOR);
		_dma_config_desc((P_DMA_DESCRIPTOR)p_cur_des, (P_DMA_DESCRIPTOR)p_next_des, 0, 0, src, trg, pCMD);

		// For some cases, such as ssp fifo. FIFO address should set src_inc but the address may not increase.
		if(pCMD->bits.IncSrcAddr) src = src + pCMD->bits.Length;
		if(pCMD->bits.IncTrgAddr) trg = trg + pCMD->bits.Length;
	}

	//handle the tail bytes.
	p_cur_des = (uint32_t)p_des + (des_num-1)*sizeof(DMA_DESCRIPTOR);
	p_next_des = (uint32_t)p_des + des_num*sizeof(DMA_DESCRIPTOR);	
	pCMD->bits.Length = bytes_tail;
	_dma_config_desc((P_DMA_DESCRIPTOR)p_cur_des, (P_DMA_DESCRIPTOR)p_next_des, 1, 0, src, trg, pCMD);

	if(des_num>1)
		pCMD->bits.Length = bytes_step;//reset the pCMD.

	return;
}

void dma_set_des(P_DMA_DESCRIPTOR p_des, uint32_t channel){
	DMA->DMA_DESC[channel].DDADR = (uint32_t)p_des;

	return;
}

void dma_xfer_start(uint32_t channel){
	if(DMA->DCSR[channel]&CSR_RUN){
		bbu_printf("DMA: channel %d is busy\r\n", channel);
		return;
	}
	DMA->DCSR[channel] |= CSR_RUN;

	return;
}

void dma_xfer_stop(uint32_t channel){
	if(DMA->DCSR[channel]&CSR_RUN){
		DMA->DCSR[channel] &= ~CSR_RUN;
	}else{
		bbu_printf("DMA: channel %d is not running\r\n", channel);
	}

	return;
}

void dma_set_mode(uint32_t mode, uint32_t channel){
	DMA->DCSR[channel] &= ~CSR_NODESCFETCH;
	DMA->DCSR[channel] |= mode<<30;

	return;
}


uint32_t dma_get_des_num(uint32_t bytes_step, uint32_t bytes_len){
	uint32_t ret = bytes_len / bytes_step;
	uint32_t bytes_tail = bytes_len % bytes_step;
	if(bytes_tail != 0)
		ret++;

	return ret;
}

void dma_map_device_to_channel(DMA_MAP_DEVICE device_id, uint32_t channel){
	if(device_id < DMA_DEVICE_PART1_NUM){
		DMA->DRCMR1[device_id] |= (RCMR_MAPVLD | channel);
	}else{
		DMA->DRCMR2[device_id-DMA_DEVICE_PART1_NUM] |= (RCMR_MAPVLD | channel);
	}

	return;
}

void dma_unmap_device_to_channel(DMA_MAP_DEVICE device_id, uint32_t channel){
	(void)channel; //avoid warning.
	if(device_id < DMA_DEVICE_PART1_NUM){
		DMA->DRCMR1[device_id] &= RCMR_CHLNUM_MSK;
	}else{
		DMA->DRCMR2[device_id-DMA_DEVICE_PART1_NUM] &= RCMR_CHLNUM_MSK;
	}

	return;
}

void set_user_aligment(uint32_t channel){
	DMA->DALGN |= 1<<channel;

	return;
}

uint32_t dma_is_channel_busy(uint32_t channel){
	uint32_t ret = 0;
	if(DMA->DCSR[channel] & CSR_STOPINTR){
		ret = 0;
	}else{
		ret = 1;
	}

	return ret;
}

void dma_clr_irq(uint32_t channel){
	uint32_t t = dma_is_channel_busy(channel);
	if(t){
		bbu_printf("DMA: channel %d is busy and can not clear the interrupt\r\n", channel);
		return;
	}

	DMA->DCSR[channel] |= DMA_STATUS_INTERRUPTS_MASK;
	
	return;
}

//Read DMA DCSR register.
uint32_t dma_read_status(uint32_t channel){
	return DMA->DCSR[channel];
}

void dma_stop_irq_en(uint32_t channel){
	DMA->DCSR[channel] |= CSR_STOPIRQEN;
	
	return;
}
void dma_stop_irq_dis(uint32_t channel){
	DMA->DCSR[channel] &= ~CSR_STOPIRQEN;
	
	return;
}

void dma_set_dcsr(uint32_t val, uint32_t mask, uint32_t channel){
	DMA->DCSR[channel] &= ~mask;
	DMA->DCSR[channel] |= val;

	return;
}

void dma_connect_irq_handler(){
	ISR_Connect(IRQ_MMP_DMA1, dma_irq_handler_default, NULL);
	INT_Enable(IRQ_MMP_DMA1, 0, 15);

	return;
}
void dma_disconnect_irq_handler(){
	ISR_Disconnect(IRQ_MMP_DMA1);
	INT_Disable(IRQ_MMP_DMA1);

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t _dma_m2m_xfer(uint32_t des, uint32_t src, uint32_t dst, uint32_t size, 
					   uint32_t byte_per_desc, uint32_t burst_size, uint32_t channel){
	uint32_t ret = 1;
	
	//set command register.
	DMA_CMDx_T cmd;
	cmd.value = 0;
	cmd.bits.IncSrcAddr = 1;
	cmd.bits.IncTrgAddr = 1;
	cmd.bits.MaxBurstSize = burst_size;
	cmd.bits.Length = byte_per_desc;

	P_DMA_DESCRIPTOR descriptor_addr = (P_DMA_DESCRIPTOR)des;

	set_user_aligment(channel);
	dma_config_desc(descriptor_addr, &cmd, src, dst, size);
	dma_set_des(descriptor_addr, channel);

	//dma_connect_irq_handler();
	//dma_stop_irq_en(channel);

	dma_xfer_start(channel);	//DMA transfer begin

	return ret;
}

//fetch mode
uint32_t dma_m2m_xfer(uint32_t src, uint32_t dst, uint32_t size, uint32_t byte_per_desc, uint32_t burst_size, uint32_t channel){
	uint32_t ret = 1;

	dcache_clean_range(src, size);
	dcache_clean_range(dst, size);

	//malloc descriptor address.
	uint32_t des_num = dma_get_des_num(byte_per_desc, size);	
	P_DMA_DESCRIPTOR descriptor_addr = NULL;
	descriptor_addr = (P_DMA_DESCRIPTOR)bbu_nc_malloc(des_num*sizeof(DMA_DESCRIPTOR));
	if(!descriptor_addr){
		bbu_printf("DMA: descriptor_addr malloc fail\r\n");
		return 0;
	}

	ret = _dma_m2m_xfer((uint32_t)descriptor_addr, src, dst, size, byte_per_desc, burst_size, channel);

	dcache_clean_range(src, size);
	dcache_clean_range(dst, size);

	bbu_mfree(descriptor_addr);

	int retry = 0;
	for(retry = 0; retry < 10000; retry++){
		if(!dma_is_channel_busy(channel))
			break;
		mdelay(1);
	}

	if(retry >= 10000){
		bbu_printf("DMA: transfer data timeout!\r\n");
		ret = 0;
	}
/*	if(dma_mem_cmp(src, dst, size)){*/
/*		bbu_printf("DMA: DMA memory test fail\r\n");*/
/*		ret = 0;*/
/*	}*/

	return ret;
}

//no-fetch mode.
uint32_t dma_m2m_xfer_nf(uint32_t src, uint32_t dst, uint32_t size, uint32_t burst_size, uint32_t channel){
	uint32_t ret = 1;
	dma_set_mode(DMA_MODE_NONFETCH, channel);

	//set command register.
	DMA_CMDx_T cmd;
	cmd.value = 0;
	cmd.bits.IncSrcAddr = 1;
	cmd.bits.IncTrgAddr = 1;
	cmd.bits.MaxBurstSize = burst_size;
	cmd.bits.Length = size;
	dma_set_reg_nf(src, dst, &cmd, channel);

	set_user_aligment(channel);

	dcache_clean_range(src, size);
	dcache_clean_range(dst, size);

	dma_xfer_start(channel);	//DMA transfer begin

	dcache_invalidate_range(src, size);
	dcache_invalidate_range(dst, size);

	int retry = 0;
	for(retry = 0; retry < 10000; retry++){
		if(!dma_is_channel_busy(channel))
			break;
		mdelay(1);
	}

	if(retry >= 10000){
		bbu_printf("DMA: transfer data timeout!\r\n");
		ret = 0;
	}
	if(dma_mem_cmp(src, dst, size)){
		bbu_printf("DMA: DMA memory test fail\r\n");
		ret = 0;
	}
	dma_set_mode(DMA_MODE_FETCH, channel);

	return ret;
}

static void dma_fill_src(uint32_t addr, uint32_t len)
{
	for(int i=0; i<len/16; i = i+16)
	{
		*(uint32_t *)(addr+i) = 0x0;
		*(uint32_t *)(addr+i+4) = 0xFFFFFFFF;
		*(uint32_t *)(addr+i+8) = 0x55555555;
		*(uint32_t *)(addr+i+12) = 0xAAAAAAAA;
	}
}

/**
 * There are 4 type test in dma memory test case.
 * SQU=>DDR SQU<=DDR SQU<=>SQU DDR<=>DDR
 */
//direction: 0-"SQU <=> SQU"， 1-"SQU => DDR", 2-"DDR <=> DDR", 3-"SQU <= DDR"
//4-all directions are tested
uint32_t dma_m2m_xfer_test(uint32_t mode, uint32_t direction, uint32_t channel){
	uint32_t src[4];
	uint32_t dst[4];
	uint32_t ret = 1;
	uint32_t i = 0, j=0;
	int start_d, end_d, start_c, end_c;
	const char p[4][20] = {"SQU <=> SQU", "SQU => PSRAM", "PSRAM <=> PSRAM", "SQU <= PSRAM"};
	uint32_t xfer_size = 0;


	if(mode == DMA_MODE_FETCH){
		if(direction == 2)
			xfer_size = DMA_MDMA_XFER_SIZE;
		else
			//16K if test SQU
			xfer_size = 0x4000;
	}else{
		xfer_size = DMA_MDMA_NF_XFER_SIZE;
	}

	if(direction != 4){
		start_d = direction;
		end_d = direction+1;
	}else{
		start_d = 0;
		end_d = 4;
	}

	if(channel != 32)
	{
		start_c = channel;
		end_c = channel +1;
	}else
	{
		start_c = 0;
		end_c = 32;
	}
#if 0
	//init the source and target address.
	src[0] = DMA_SQU_ADDR_1;
	src[1] = DMA_SQU_ADDR_1;
	src[2] = (uint32_t)bbu_nc_malloc(xfer_size);
	src[3] = (uint32_t)bbu_nc_malloc(xfer_size);

	dst[0] = DMA_SQU_ADDR_2;
	dst[1] = (uint32_t)bbu_nc_malloc(xfer_size);
	dst[2] = (uint32_t)bbu_nc_malloc(xfer_size);
	dst[3] = DMA_SQU_ADDR_2;
#endif
	if(direction != 0){
		bbu_printf("Only SQU <==> SQU supported for now\n\r");
		return 0;
	}

	src[0] = (uint32_t)bbu_nc_malloc(xfer_size);
	dst[0] = (uint32_t)bbu_nc_malloc(xfer_size);

	for(j = start_c; j < end_c; j++){
		for(i=start_d; i<end_d; i++){
			//8 bytes.
			dma_fill_src(src[i], xfer_size);
			memset((uint32_t*)dst[i], 0, xfer_size);
			bbu_printf(DBG_MSG"### DMA: %s TRANSFER at channel %d, ###\r\n", p[i], j);
			bbu_printf(DBG_MSG"SRC:0x%08x, DST:0x%08x, SIZE:0x%08x BURST:%s\r\n",
				 src[i], dst[i], xfer_size, "  8B");
			if(mode == DMA_MODE_FETCH){
				dma_m2m_xfer(src[i], dst[i], xfer_size, DMA_MAX_BYTES_PER_DESC, DMA_BURST_SIZE_8B, j);
			}else{
				dma_m2m_xfer_nf(src[i], dst[i], xfer_size, DMA_BURST_SIZE_8B, j);
			}
	
			if(dma_mem_cmp(src[i], dst[i], xfer_size)){
				bbu_printf("DMA: DMA memory test fail %s burst_size:8 bytes\r\n", p[i]);
				goto fail;
			}
			bbu_printf(DBG_MSG"PASS\n\r");
	
			//16 bytes.
			dma_fill_src(src[i], xfer_size);
			memset((uint32_t*)dst[i], 0, xfer_size);
			bbu_printf(DBG_MSG"SRC:0x%08x, DST:0x%08x, SIZE:0x%08x BURST:%s\r\n",
				 src[i], dst[i], xfer_size, " 16B");
			if(mode == DMA_MODE_FETCH){
				dma_m2m_xfer(src[i], dst[i], xfer_size, DMA_MAX_BYTES_PER_DESC, DMA_BURST_SIZE_16B, j);
			}else{
				dma_m2m_xfer_nf(src[i], dst[i], xfer_size, DMA_BURST_SIZE_16B, j);
			}
	
			if(dma_mem_cmp(src[i], dst[i], xfer_size)){
				bbu_printf("DMA: DMA memory test fail %s burst_size:16 bytes\r\n", p[i]);
				goto fail;
			}
			bbu_printf(DBG_MSG"PASS\n\r");
	
			//32 bytes.
			dma_fill_src(src[i], xfer_size);
			memset((uint32_t*)dst[i], 0, xfer_size);
			bbu_printf(DBG_MSG"SRC:0x%08x, DST:0x%08x, SIZE:0x%08x BURST:%s\r\n",
				src[i], dst[i], xfer_size, " 32B");
			if(mode == DMA_MODE_FETCH){
				dma_m2m_xfer(src[i], dst[i], xfer_size, DMA_MAX_BYTES_PER_DESC, DMA_BURST_SIZE_32B, j);
			}else{
				dma_m2m_xfer_nf(src[i], dst[i], xfer_size, DMA_BURST_SIZE_32B, j);
			}
	
			if(dma_mem_cmp(src[i], dst[i], xfer_size)){
				bbu_printf("DMA: DMA memory test fail %s burst_size:32 bytes\r\n", p[i]);
				goto fail;
			}
			bbu_printf(DBG_MSG"PASS\n\r");
	
			//64 bytes.
			dma_fill_src(src[i], xfer_size);
			memset((uint32_t*)dst[i], 0, xfer_size);
			bbu_printf(DBG_MSG"SRC:0x%08x, DST:0x%08x, SIZE:0x%08x BURST:%s\r\n",
				src[i], dst[i], xfer_size, " 64B");
			if(mode == DMA_MODE_FETCH){
				dma_m2m_xfer(src[i], dst[i], xfer_size, DMA_MAX_BYTES_PER_DESC, 4, j);
			}else{
				dma_m2m_xfer_nf(src[i], dst[i], xfer_size, 4, j);
			}
	
			if(dma_mem_cmp(src[i], dst[i], xfer_size)){
				bbu_printf("DMA: DMA memory test fail %s burst_size:64 bytes\r\n", p[i]);
				goto fail;
			}
			bbu_printf(DBG_MSG"PASS\n\r");
		}
	}

	#if 0
	bbu_mfree((void *)src[2]);
	bbu_mfree((void *)src[3]);
	bbu_mfree((void *)dst[1]);
	bbu_mfree((void *)dst[2]);
	#endif
	bbu_mfree((void *)src[0]);
	bbu_mfree((void *)dst[0]);
	return ret;

	fail:
		ret = 0;
		#if 0
		bbu_mfree((void *)src[2]);
		bbu_mfree((void *)src[3]);
		bbu_mfree((void *)dst[1]);
		bbu_mfree((void *)dst[2]);
		#endif
		bbu_mfree((void *)src[0]);
		bbu_mfree((void *)dst[0]);
		return ret;
}

/**
 * DMA stress test.
 * DMA channels run in parallel.
 * The channels restart in dma_irq_handler_stress().
 * Use dma_stress_stop() to stop the test.
 */
uint32_t dma_stress_test(uint32_t channel_start, uint32_t channel_num, uint32_t size, uint32_t entry_first){
	uint32_t ret = 1;
	uint32_t t = 0;

	if(entry_first){
		uint32_t k = 0;
		for(k=0; k<32; k++){
			DMA->DCSR[k] |= DMA_STATUS_INTERRUPTS_MASK;//clear all the interrupt.
		}
		//set the mask for stress irq handler.
		uint32_t c = 0;
		for(c=0; c<channel_num; c++)
			arg_glob.mask |= (1<<(c+channel_start));
		
		uint32_t stress_des_num = dma_get_des_num(DMA_MAX_BYTES_PER_DESC, size); 

		for(t=0; t<channel_num; t++){
			arg_glob.src[t] = (uint32_t)bbu_nc_malloc(size);
			arg_glob.dst[t] = (uint32_t)bbu_nc_malloc(size);
			arg_glob.des[t] = (uint32_t)bbu_nc_malloc(stress_des_num*sizeof(DMA_DESCRIPTOR));
			dma_fill_src(arg_glob.src[t], size);
			memset((uint32_t*)arg_glob.dst[t], 0x55, size);

			if((arg_glob.src[t]==0)||(arg_glob.dst[t]==0)){
				bbu_printf("DMA: bbu_nc_malloc fail.\r\n");
				ret = 0;
				goto fail;
			}
		}
		
		//fill the global variable.
		arg_glob.channel = channel_start;
		arg_glob.num = channel_num;
		arg_glob.size = size;
		ISR_Connect(IRQ_MMP_DMA1, dma_irq_handler_stress, NULL);
		INT_Enable(IRQ_MMP_DMA1, 0, 15);
	}

	for(t=0; t<channel_num; t++){
		ret = _dma_m2m_xfer(arg_glob.des[t], arg_glob.src[t], arg_glob.dst[t], DMA_STRESS_XFER_SIZE, 
							DMA_MAX_BYTES_PER_DESC, DMA_BURST_SIZE_32B, 
							channel_start+t);
		//We need to enable the stop irq after dmac run.
		dma_stop_irq_en(channel_start+t);
		if(ret==0)
			goto fail;
	}

//	bbu_printf("DMA: stress test ongoing.\r\n");

/*	for(i=0; i<channel_num; i++){*/
/*		if(arg_glob.src[i])*/
/*			bbu_mfree((void *)arg_glob.src[i]);*/
/*		if(arg_glob.dst[i])*/
/*			bbu_mfree((void *)arg_glob.dst[i]);*/
/*	}*/
/*	Q_SPIN_LOCK(&dma_stress_stop_lock);*/
/*	uint32_t stop = arg_glob.stop_flag;*/
/*	Q_SPIN_UNLOCK(&dma_stress_stop_lock);*/
/*	*/
/*	if(!stop){*/
/*		DmaStressEvt *dse = Q_NEW(DmaStressEvt, DMA_STRESS_SIG);*/
/*		dse->channel = channel_start;*/
/*		dse->num = channel_num;*/
/*		dse->size = size;*/
/*		QACTIVE_POST(AO_Dma, (QEvent *)dse, 0);*/
/*	}else{*/
/*		Q_SPIN_LOCK(&dma_stress_stop_lock);*/
/*		arg_glob.stop_flag = 0; //reset dma_stress_stop value.*/
/*		Q_SPIN_UNLOCK(&dma_stress_stop_lock);*/
/*	}*/

	return ret;

fail:
	bbu_printf("DMA: stress test fail in channel %d\r\n", channel_start+t);
	for(t=0; t<channel_num; t++){
		if(arg_glob.src[t])
			bbu_mfree((void *)arg_glob.src[t]);
		if(arg_glob.dst[t])
			bbu_mfree((void *)arg_glob.dst[t]);
	}
	return ret;
}

void dma_stress_stop(){
	Q_SPIN_LOCK(&dma_stress_stop_lock);
	arg_glob.stop_flag = 1;
	Q_SPIN_UNLOCK(&dma_stress_stop_lock);
	bbu_printf("DMA: stress test stop.\r\n");

	return;
}

void dma_stress_show_iter(){
	bbu_printf("DMA stress iter is %d\r\n", dma_stress_iter_max_in_use);
	
	return;
}

void dma_stress_set_iter(uint32_t iter){
	dma_stress_iter_max_in_use = iter;
	bbu_printf("DMA stress iter has been set to %d\r\n", iter);
	
	return;
}

void dma_config_descriptor(uint32_t* p_desc,
					   uint32_t* p_next_desc,
					   uint32_t p_src_addr,
					   uint32_t p_tag_addr,
					   uint32_t p_cmd,
					   uint32_t stop_bit){
	((P_DMA_DESCRIPTOR)p_desc)->DDADR = ((uint32_t)p_next_desc | (stop_bit & 0x1));
	((P_DMA_DESCRIPTOR)p_desc)->DSADR = p_src_addr;
	((P_DMA_DESCRIPTOR)p_desc)->DTADR = p_tag_addr;
	((P_DMA_DESCRIPTOR)p_desc)->DCMD = p_cmd;

	return;
}

static void dmac_descriptor_fetch(uint32_t channel){
	uint32_t temp_data;

	temp_data = DMA->DCSR[channel];
	temp_data &= ~(1<<30);
	DMA->DCSR[channel] = temp_data;

	return;
}

void dma_load_descriptor(P_DMA_DESCRIPTOR p_desc, uint32_t channel){
	DMA->DMA_DESC[channel].DDADR = (uint32_t)p_desc;
	dmac_descriptor_fetch(channel);

	return;
}

void dma_enable(uint32_t channel){
	dma_xfer_start(channel);
}

int dma_wait_done(uint32_t channel, uint32_t timeout, uint32_t flags){
	int flag = 0;
	uint64_t count = 0;
	uint64_t current_count;
	int ret = 0;
	int i = 0;
	
	count = bbu_get_usec();
	count += (uint64_t)timeout;

	switch(flags){
	case DMA_STOP:
		flag = CSR_STOPINTR;
		break;
	case DMA_END:
		flag = CSR_ENDINTR;
		break;
	default:
		bbu_printf("Unsupport DMA done flag!!!\r\n");
		return 0;
	}
	
	current_count = bbu_get_usec();
	while((!(DMA->DCSR[channel] & flag))&&(count > current_count))
	{
		current_count = bbu_get_usec();
		for (i = 0; i < 0x500; i++)
			__nop();
	}
	
	if (count <= current_count){
		ret = 0;
		bbu_printf("DMA wait done TIMEOUT!!!\r\n");
	} else {
		ret = 1;
	}
	
	return ret;
}















