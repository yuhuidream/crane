#include "bbu.h"
#include "NSSP.h"
#include "ssp_dec.h"
#include "predefines.h"
#include "gpio_dec.h"
#include "dma_dec.h"

#define FALSE 0
#define TRUE 1

#define reg_write(reg, val) *(volatile unsigned int *)reg = val; *(volatile unsigned int *)reg;
#define reg_bit_set(reg, val) *(volatile unsigned int *)reg |= val; *(volatile unsigned int *)reg;
#define reg_bit_clr(reg, val) *(volatile unsigned int *)reg &= ~val; *(volatile unsigned int *)reg;

#define SSP_READ_TIME_OUT_MILLI		0x200
#define SSP_READ_DMA_DESC			0x10	//total RX descriptors
#define SSP_READ_DMA_SIZE			0x1ff8	//bytes per descriptor
#define SSP_MAX_TX_SIZE_WORDS		64
#define SSP_MAX_TX_SIZE_BYTES		SSP_MAX_TX_SIZE_WORDS << 2
#define WRITE_SIZE (256)

#define SPI_CMD_RESET_ENABLE			0x66
#define SPI_CMD_RESET					0x99
#define SPI_CMD_JEDEC_ID				0x009F
#define SPI_CMD_RELEASE_POWER_DOWN		0x00AB
#define SPI_CMD_READ_STATUS			0x05
#define SPI_READ_CMD   0x03
#define SPI_CMD_WRITE_ENABLE   0x06
#define SPI_CMD_PROGRAM				0x02

#define TOP_INIT_DATA8       0x000620E0
#define TOP_INIT_DATA16      0x000621E0
#define TOP_INIT_DATA32      0x000623E0
#define FIFO_INIT            0x00000108
#define FIFO_INIT_READ       0xCE7
#define FIFO_INIT_WRITE      0x4E7
#define INT_EN_INIT    0x30

//2*4 byte
#define RX_THRESHOLD         1
//3*4 bytes
#define TX_THRESHOLD        

static int ssp_port = 2;
static int ssp_init_clk = 0;
static ssp_init_param spi_param = {SSP_SPI_MODE, SSP_DATA_32BIT, 48000, 8, 8, SPI_MODE_0, 0};

unsigned int Endian_Convert (unsigned int in)
{
	unsigned int out;
	out = in << 24;
	out |= (in & 0xFF00) << 8;
	out |= (in & 0xFF0000) >> 8;
	out |= (in & 0xFF000000) >> 24;
	return out;
}

void spi_host_init(void)
{
	uint32_t word, id;
	bbu_printf("Step 0\n\r");
	unsigned char mid, status2;
	unsigned short did;
	BU_REG_WRITE(0xD401E2C4, 0x1081);	//nHold
	BU_REG_WRITE(0xD401E2C8, 0x1081);	//nWP
	gpio_set_output(71);
	gpio_set_output(72);
	gpio_output_high(71);
	gpio_output_high(72);
	udelay(20);

	BU_REG_WRITE(0xD401E2CC, 0x1082);	//RX
	BU_REG_WRITE(0xD401E2D0, 0x1082);	//TX
	BU_REG_WRITE(0xD401E2D4, 0x1082);	//FRM (CS)
	BU_REG_WRITE(0xD401E2D8, 0x1082);	//CLK
	udelay(1);
	ssp_init_master(ssp_port, ssp_init_clk);

	SPINOR_ReadId(&mid, &did);

	bbu_printf("ID: 0x%x 0x%x\n\r", mid, did);
#if 0
	SPINOR_ReadId(&mid, &did);

	SPINOR_Reset();

	SPINOR_ReadId(&mid, &did);

	bbu_printf("ID: 0x%x 0x%x\n\r", mid, did);
#endif
}

void SPI_WaitSSPComplete(void)
{
	uint32_t val;
	do{
		val = SSP2->STATUS;
		//bbu_printf("SPI_WaitSSPComplete: 0x%x\n\r", val);
		if(((val & 0x1) ==0 ) && 
			( ((val&0xF80) == 0) &&  ((val & 0x40) == 0x40) )
			)
		{
			//bbu_printf("break\n\r");
			break;
		}
	}while(1);
}

void SPI_Write_Read(unsigned char *cmd, unsigned char *data, unsigned char len)
{
	unsigned char i;
	
	for (i = 0; i < len; i++)
	{
		BU_REG_WRITEB(0xD401C010, cmd[i]);
		SPI_WaitSSPComplete();

		data[i] = BU_REG_READB(0xD401C010);
	}
}

void SPI_DisableSSP(void)
{
	SSP2->TOP_CTRL  = TOP_INIT_DATA8;
	SSP2->FIFO_CTRL = FIFO_INIT;
}

void SPINOR_ReadStatus(uint32_t Wait)
{
	uint32_t i=0;
	uint32_t read, ready, dummy, status;

	status = 0;
	read = FALSE;	//this flag gets set when we read first entry from fifo
	//if the caller waits to 'Wait' for the BUSY to be cleared, start READY off as FALSE
	//if the caller doesn't wait to wait, set READY as true, so we don't wait on the bit
	ready = (Wait) ? FALSE : TRUE;

	do{
		SPI_DisableSSP();

		//need to use 16bits data
		SSP2->TOP_CTRL = TOP_INIT_DATA16;

		//fire it up
		SSP2->TOP_CTRL |= 0x1;

		Assert_CS();

		//load the command + 1 dummy byte
		SSP2->DATAR = SPI_CMD_READ_STATUS << 8;

		SPI_WaitSSPComplete();

		Deassert_CS();

		dummy = SSP2->DATAR;

		//set the READ flag, and read the status
		read = TRUE;
		bbu_printf("status: 0x%x\n\r", dummy);

		status = dummy & 0xFF;	//the status will be in the second byte

		//set the READY flag if the status wait bit is cleared
		if((status & 1) == 0)		// operation complete (eg. not busy)?
			ready = TRUE;

		SPI_DisableSSP();

		//we need to wait until we read at least 1 valid status entry
		//if we're waiting for the Write, wait till WIP bits goes to 0
	}while ((!read) || (!ready));


	//return last known status
	return;
}

void SPINOR_Rdp(void)
{
	uint8_t cmd[1], data[1];
	cmd[0] = SPI_CMD_RELEASE_POWER_DOWN;
	
	SPI_DisableSSP();

	//fire it up
	SSP2->TOP_CTRL |= 0x1;

	Assert_CS();

	SPI_Write_Read(cmd, data, sizeof(cmd));

	Deassert_CS();

	SPI_DisableSSP();

	udelay(10);
}

void SPINOR_Reset(void)
{
	uint8_t status, cmd[1], data[1];
#if 0
	cmd[0] = SPI_CMD_RELEASE_POWER_DOWN;
	
	SPI_DisableSSP();

	//fire it up
	SSP2->TOP_CTRL |= 0x1;

	Assert_CS();

	SPI_Write_Read(cmd, data, sizeof(cmd));

	Deassert_CS();

	SPI_DisableSSP();

	udelay(10);
#endif
	// reset memory
	SPINOR_ResetEnable();

	SPINOR_ResetMemory();

	SPINOR_ReadStatus(TRUE);

	return;
}

void SPINOR_ResetEnable(void)
{
	uint8_t status, cmd[1], data[1];

	cmd[0] = 0x66;
	
	SPI_DisableSSP();

	//fire it up
	SSP2->TOP_CTRL |= 0x1;

	Assert_CS();

	SPI_Write_Read(cmd, data, 1);

	Deassert_CS();

	SPI_DisableSSP();

	return;
}

void SPINOR_ResetMemory(void)
{
	uint8_t status, cmd[1], data[1];

	cmd[0] = 0x99;
	
	SPI_DisableSSP();

	//fire it up
	SSP2->TOP_CTRL |= 0x1;

	Assert_CS();

	SPI_Write_Read(cmd, data, sizeof(cmd));

	Deassert_CS();

	SPI_DisableSSP();

	return;
}

#if 0
void SPINOR_ReadId(unsigned char *mid, unsigned short *did)
{
	uint8_t cmd[4], data[4];

	cmd[0] = 0x9f;
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = 0;

	SPI_DisableSSP();

	//fire it up
	SSP2->TOP_CTRL |= 0x1;

	Assert_CS();

	SPI_Write_Read(cmd, data, sizeof(cmd));

	Deassert_CS();	

	SPI_DisableSSP();

	*mid = data[1];
	*did = data[3] | (data[2] << 8);

	return;
}

#else

void SPINOR_ReadId(unsigned char *mid, unsigned short *did)
{
	uint32_t cmd, data;

	SPI_DisableSSP();

	SSP2->TOP_CTRL = TOP_INIT_DATA32;
	//fire it up
	SSP2->TOP_CTRL |= 0x1;

	Assert_CS();

	SSP2->DATAR = 0x9F000000;

	SPI_WaitSSPComplete();

	data = SSP2->DATAR;

	SPI_DisableSSP();

	bbu_printf("Read ID finish:0x%x\n\r", data);
	*mid = (data & 0xFF00)>>8;
	*did = (data&0xFFFF0000)>>16;

	return;
}
#endif

void Assert_CS(void)
{

}

void Deassert_CS()
{

}

#if 1
// SPI READ: DMA - with linked descriptors
uint32_t SPI_Read(uint32_t FlashOffset, uint32_t Buffer, uint32_t Size)
{
	int i=1;
	uint32_t	*buff;
	uint32_t	read_size, total_size, start_time, Retval, end_time;
	uint32_t 	restore_dword = 0, *restore_addr = 0;
	uint32_t *p1, *p2, *p3, *p4, *p5;
	Retval = 0;
	DMA_CMDx_T RX_Cmd, TX_Cmd;

	DMA_DESCRIPTOR* RX_Desc = NULL;
	DMA_DESCRIPTOR* TX_Desc = NULL;
	uint32_t *rx1_fromcommand, *tx1_command, *tx2_runclocks;
	bbu_printf("STEP 0\n\r");
	//pre-DMA setup:
	// modify the size value to be modulo 8 byte, as DMA cannot handle smaller burst sizes
	// this may cause an issue where 4 bytes after the image gets corrupted.  in this case
	// we must save and restore those 4 bytes
	if(Size & 0x7) 	// check for modulo 8 bytes in length
	{	   			// - non 8 bytes.  adjustments required
		restore_addr = (unsigned int *)(Buffer + Size);//address *after* the loaded image
		restore_dword = *restore_addr;					//save the word after the loaded image
		Size = (Size & 0xFFFFFFF8) + 0x8;				//modulo 8 byte align the size
	}

	p1 = bbu_nc_malloc(SSP_READ_DMA_DESC*sizeof(DMA_DESCRIPTOR) +16);
	p2 = bbu_nc_malloc(SSP_READ_DMA_DESC*sizeof(DMA_DESCRIPTOR) +16);
	//p3 = bbu_nc_malloc(4+16);
	//p4 = bbu_nc_malloc(4+16);
	//p5 = bbu_nc_malloc(4+16);
	RX_Desc = (DMA_DESCRIPTOR*)(((uint32_t)p1 & 0xFFFFFFF0) + 0x10);
	TX_Desc = (DMA_DESCRIPTOR*)(((uint32_t)p2 & 0xFFFFFFF0) + 0x10);
	tx1_command = (uint32_t*)0xD1000000;
	tx2_runclocks = (uint32_t*)0xD1000010;
	rx1_fromcommand = (uint32_t*)0xD1000020;
	//for(int j=0; j< 32; j++)
	//	tx1_command[j] = 0xb2b4b6ba;
	*tx2_runclocks = 0xb2b4b6ba;

	// * initialize variables * //
	//save off the buffer pointer - for endian convert at end of routine
	buff = (uint32_t *)Buffer;
	//remember how much (in words) is Read - for endian convert at the end of routine
	total_size = Size >> 2;

	//tx0 - load the SPI command and address (0xCmd_AddrHI_AddrMid_AddrLow)
	tx1_command[0]  = (SPI_READ_CMD << 24) | (FlashOffset & 0x00FFFFFF);
	
	//fill out commands
	RX_Cmd.value = 0;
	RX_Cmd.bits.IncSrcAddr = 0;
	RX_Cmd.bits.IncTrgAddr = 1;
	RX_Cmd.bits.FlowSrc = 1;
	RX_Cmd.bits.FlowTrg = 0;
	RX_Cmd.bits.Width = 3;
	RX_Cmd.bits.MaxBurstSize = 1;

	TX_Cmd.value = 0;
	TX_Cmd.bits.IncSrcAddr = 0;
	TX_Cmd.bits.IncTrgAddr = 0;
	TX_Cmd.bits.FlowSrc = 0;
	TX_Cmd.bits.FlowTrg = 1;
	TX_Cmd.bits.Width = 3;
	TX_Cmd.bits.MaxBurstSize = 1;
	bbu_printf("STEP 1\n\r");
	//setup DMA
	//Map Device to Channels
	ssp_map_channel(2, SSP_DMA_TX_CHANNEL, SSP_DMA_RX_CHANNEL);

	//turn ON user alignment - in case buffer address is 64bit aligned
	set_user_aligment(SSP_DMA_RX_CHANNEL);
	set_user_aligment(SSP_DMA_TX_CHANNEL);

	//reset SSP CR's
	SSP2->TOP_CTRL = TOP_INIT_DATA32;  //trailing byte???
	SSP2->FIFO_CTRL = FIFO_INIT_READ;
	SSP2->INT_EN = INT_EN_INIT;
	SSP2->TO = 0x200;

	//fire SSP up
	SSP2->TOP_CTRL |= 0x1;

	Retval = 0;

	//for each loop iteration, one Read Command will be issued with a link descriptor chain
	//	the chain will read a total of (SSP_READ_DMA_DESC-1)*SSP_READ_DMA_SIZE bytes from SPI
	bbu_printf("STEP 2\n\r");
	while (Size > 0)
	{
		// * configure RX & TX descriptors * //
		//initial 1 word transfer:
		//		TX: send command+address
		//		RX: receive dummy word
		RX_Cmd.bits.Length = 4;
		TX_Cmd.bits.Length = 4;
		dma_config_descriptor(&RX_Desc[0], &RX_Desc[1], ssp_get_datar(2), rx1_fromcommand, RX_Cmd.value, 0);
		dma_config_descriptor(&TX_Desc[0], &TX_Desc[1], tx1_command,	ssp_get_datar(2), TX_Cmd.value, 0);

		i = 1;
		//chaining of full transfers (descriptors that are not "stop"ped)
		//fill out descriptors until either:
		//	- there is only enough data for 1 more descriptor (which needs to be the "stopped" descriptor)
		//	- the pool of descriptors is depleted (minus 1 because the last needs to be "stopped")
		while( (Size > SSP_READ_DMA_SIZE) && (i < (SSP_READ_DMA_DESC-1)) )
		{
			RX_Cmd.bits.Length = SSP_READ_DMA_SIZE;
			TX_Cmd.bits.Length = SSP_READ_DMA_SIZE;
			dma_config_descriptor(&RX_Desc[i], &RX_Desc[i+1],	ssp_get_datar(2),	Buffer, RX_Cmd.value, 0);
			dma_config_descriptor(&TX_Desc[i], &TX_Desc[i+1],	tx2_runclocks,	ssp_get_datar(2), TX_Cmd.value, 0);
			//NOTE: writing SSP_SR to SSP_DR to keep clocks running.  cannot be a value in DDR as it causes a fault on MCK5

			//Update counters
			Buffer 		+=	SSP_READ_DMA_SIZE;
			FlashOffset +=	SSP_READ_DMA_SIZE;
			Size 		-=	SSP_READ_DMA_SIZE;			
			i++;
		}
		bbu_printf("STEP 3\n\r");
		//last link: descriptor must be "stopped"
		read_size 	=  Size > SSP_READ_DMA_SIZE ? SSP_READ_DMA_SIZE : Size;
		RX_Cmd.bits.Length = read_size;
		TX_Cmd.bits.Length = read_size;
		dma_config_descriptor(&RX_Desc[i], NULL,	ssp_get_datar(2), Buffer, RX_Cmd.value, 1);
		dma_config_descriptor(&TX_Desc[i], NULL,	tx2_runclocks,	ssp_get_datar(2), TX_Cmd.value, 1);
		//update counters after "stop" descriptor
		Buffer 		+= read_size;
		FlashOffset += read_size; 
		Size 		-= read_size;

		//Load descriptors
		dma_load_descriptor (&RX_Desc[0], SSP_DMA_RX_CHANNEL);
		dma_load_descriptor (&TX_Desc[0], SSP_DMA_TX_CHANNEL);

		//Kick off DMA's
		dma_xfer_start(SSP_DMA_RX_CHANNEL);
		dma_xfer_start(SSP_DMA_TX_CHANNEL);

		//setup a timer to fail gracefully in case of error
		start_time = timer_get_count(0);

		//wait until the RX channel gets the stop interrupt and the TX fifo is drained
		while(!(DMA->DCSR[SSP_DMA_RX_CHANNEL] & CSR_STOPINTR))
		{
			end_time = timer_get_count(0);
			end_time = TIMER_GET_INTERVAL(end_time, start_time);
			end_time = end_time/(13*250);

			//if we've waited long enough, fail
			if( end_time > SSP_READ_TIME_OUT_MILLI)
			{
				bbu_printf("TIMEOUT\n\r");
				Retval = -1;
				break;
			}
		}
		if(Retval != 0)
			break;

		//update command to the new Flash Offset
		tx1_command  = (SPI_READ_CMD << 24) | (FlashOffset & 0x00FFFFFF);
	}
	bbu_printf("STEP 4\n\r");
	//if we errored out, kill the DMA transfers
	if(Retval != 0)
	{
		dma_xfer_stop( SSP_DMA_RX_CHANNEL );
		dma_xfer_stop( SSP_DMA_TX_CHANNEL );
	}

	//make sure SSP is disabled
	SSP2->TOP_CTRL = TOP_INIT_DATA8;
	SSP2->FIFO_CTRL = FIFO_INIT;
	SSP2->INT_EN = INT_EN_INIT;
	//clear out DMA settings
	ssp_unmap_channel(2);

	//if we errored out, return now
	if(Retval != 0)
		goto done;

	//postprocessing... endian convert
	for(i = 0; i < total_size; i++)
		buff[i] = Endian_Convert(buff[i]);

done:
	bbu_mfree(p1);
	bbu_mfree(p2);

	return Retval;
}
#endif

void SPI_WriteEnable()
{

	SPI_DisableSSP();

	//fire it up
	SSP2->TOP_CTRL |= 0x1;

	Assert_CS();

	SSP2->DATAR = SPI_CMD_WRITE_ENABLE;

	SPI_WaitSSPComplete();

	SPI_DisableSSP();

	return;
}

unsigned int SPINOR_Wipe()
{
	//make sure the device is ready for the command
	SPINOR_ReadStatus(TRUE);

	//enable program mode. this is required for any program, eg: erase, program page or otp or write status reg.
	SPI_WriteEnable();

	SPI_DisableSSP();

	//fire it up
	SSP2->TOP_CTRL |= 0x1;

	SSP2->DATAR = 0xC7;

	SPI_WaitSSPComplete();

	//don't return until the operation completes.
	SPINOR_ReadStatus(TRUE);	// wait for the last erase operation to complete.
	SPI_DisableSSP();
	return 0;	//can't tell whether it failed or not (without a ton of overhead), so just return a success
}

uint8_t SPINOR_ReadReg(void){
	uint32_t dummy;
	uint8_t status, command;

	command = SPI_CMD_READ_STATUS;

	SPI_DisableSSP();
	
	//need to use 32bits data
	SSP2->TOP_CTRL = TOP_INIT_DATA16;
	
	//fire it up
	SSP2->TOP_CTRL |= 0x1;

	Assert_CS();
	
	SSP2->DATAR = command << 8;
	SPI_WaitSSPComplete();

	dummy = SSP2->DATAR;
	status = dummy & 0xFF;	//the status will be in the second byte

	Deassert_CS();

	SPI_DisableSSP();

	//return last known status
	return status;
}

void SPINOR_WriteEnable(void)
{
	uint8_t status, cmd[1], data[1];

	cmd[0] = SPI_CMD_WRITE_ENABLE;

	SPI_DisableSSP();

	SSP2->TOP_CTRL |= 0x1;

	Assert_CS();
		
	SPI_Write_Read(cmd, data, sizeof(cmd));

	Deassert_CS();
		
	//make sure SSP is disabled
	SPI_DisableSSP();

	status = SPINOR_ReadReg();
	if (status & BIT_1 !=BIT_1)
	{
		bbu_printf("Write enable failed!!!\n\r");
	}
 
	return;
}

unsigned int SPINOR_Page_Program_DMA(unsigned int Address, unsigned int Buffer, unsigned int Size)
{
	unsigned int dummy, start_time, Retval = 0;
	DMA_CMDx_T TX_data;
	uint8_t cmd[4], data[4];

	cmd[0] = SPI_CMD_PROGRAM;
	cmd[1] = ((Address & 0x00FFFFFF) >> 16) & 0xff;
	cmd[2] = ((Address & 0x00FFFFFF) >> 8) & 0xff;
	cmd[3] = Address & 0xFF;
	uint8_t *temp_buffer, *p1;
	uint8_t *p2;
	p2 = (uint8_t *)Buffer;

	p1 = bbu_nc_malloc(Size + 4 +16);
	temp_buffer = ADDR_ALIGN((uint32_t)p1, 16);
	temp_buffer[0] = SPI_CMD_PROGRAM;
	temp_buffer[1] = ((Address & 0x00FFFFFF) >> 16) & 0xff;
	temp_buffer[2] = ((Address & 0x00FFFFFF) >> 8) & 0xff;
	temp_buffer[3] = Address & 0xFF;
	bbu_printf("temp_buffer: 0x%x\n\r", (uint32_t)temp_buffer);

	for(int i = 0; i < Size; i++)
	{
		temp_buffer[i+4] = p2[i];
	}


	TX_data.value = 0;
	TX_data.bits.IncSrcAddr = 1;
	TX_data.bits.FlowTrg = 1;
	TX_data.bits.Width = 1;
	TX_data.bits.MaxBurstSize = 1;
	TX_data.bits.Length = Size+4;

	//setup DMA
	//Map Device to Channels
	dma_map_device_to_channel(61, SSP_DMA_TX_CHANNEL);
	dma_set_mode(DMA_MODE_NONFETCH, SSP_DMA_TX_CHANNEL);

	//turn ON user alignment - in case buffer address is 64bit aligned
	set_user_aligment(SSP_DMA_TX_CHANNEL);

	DMA->DMA_DESC[SSP_DMA_TX_CHANNEL].DSADR = temp_buffer;
    DMA->DMA_DESC[SSP_DMA_TX_CHANNEL].DTADR = ssp_get_datar(2);
    DMA->DMA_DESC[SSP_DMA_TX_CHANNEL].DCMD = TX_data.value;
    bbu_printf("DMA: 0x%x 0x%x 0x%x\n\r", DMA->DMA_DESC[SSP_DMA_TX_CHANNEL].DSADR,DMA->DMA_DESC[SSP_DMA_TX_CHANNEL].DTADR, DMA->DMA_DESC[SSP_DMA_TX_CHANNEL].DCMD);
   //loadNonDescriptor((unsigned int)Buffer, (unsigned int) SSP_DR, &TX_data, SSP_TX_CHANNEL);

	SPI_DisableSSP();

	SSP2->INT_EN = INT_EN_INIT;
	SSP2->FIFO_CTRL = FIFO_INIT_READ;
	SSP2->TO = 0x200;

	//fire SSP up
	SSP2->TOP_CTRL |= 0x1;
	Assert_CS();

	//SPI_Write_Read(cmd, data, sizeof(cmd));

	//Kick off DMA's
	dma_xfer_start(SSP_DMA_TX_CHANNEL);

	//wait until the RX channel gets the stop interrupt
	while(!(DMA->DCSR[SSP_DMA_TX_CHANNEL] & CSR_STOPINTR));
	
	SPI_WaitSSPComplete();

	dma_xfer_stop(SSP_DMA_TX_CHANNEL);

	Deassert_CS();

	SPI_DisableSSP();

	//clear out DMA settings
	dma_unmap_device_to_channel(53, SSP_DMA_TX_CHANNEL);

	return Retval;

}

uint32_t SPINOR_Write(uint32_t Address, uint32_t Buffer, uint32_t Size)
{
	uint32_t Retval, i, total_size, write_size, status;
	uint32_t *buff = (uint32_t *) Buffer;

	if(Size & 0x3)    //make size 4bytes-align
    	Size = (Size+4)&(~3);

	total_size = Size >> 2;

	//postprocessing... endian convert
	if ((Address + Size) > 0x1000000)
		return -1;
	
	//for(i = 0; i < total_size; i++)
	//	buff[i] = Endian_Convert(buff[i]);

	do {
		//make sure the device is ready to be written to
		SPINOR_ReadStatus(TRUE);
		//get device ready to Program
		SPINOR_WriteEnable();
		
		SPINOR_ReadStatus(TRUE);

		write_size = Size > WRITE_SIZE ? WRITE_SIZE : Size;
        
		//write a byte
		if (write_size == WRITE_SIZE)
		{
			Retval = SPINOR_Page_Program_DMA(Address, Buffer, WRITE_SIZE);
			
			//update counters
			Address+=WRITE_SIZE;
			Buffer+=WRITE_SIZE;
			Size-=WRITE_SIZE;
		}
		else
		{
			Retval = SPINOR_Page_Program_DMA(Address, Buffer, WRITE_SIZE);
			Size=0;
		}

		SPINOR_ReadStatus(TRUE);

	} while( (Size > 0) && (Retval == 0) );

	return Retval;
}

void bbu_spi_rw_test(int size)
{
	uint32_t *tx_buff, *rx_buff, *p1, *p2;
	p1 = bbu_nc_malloc(size+16);
	p2 = bbu_nc_malloc(size+16);
	tx_buff = (uint32_t *)ADDR_ALIGN((uint32_t)p1, 16);
	rx_buff = (uint32_t *)ADDR_ALIGN((uint32_t)p2, 16);

	for(int i = 0; i < size/4; i++)
	{
		tx_buff[i] = 0xdeadbeef;
		rx_buff[i] = 0x12345678;
	}

	bbu_printf("TX Buffer: 0x%x, RX Buffer: 0x%x\n\r", (uint32_t)tx_buff, (uint32_t)rx_buff);

	SPINOR_Wipe();
	SPINOR_Write(0, tx_buff, size);
	bbu_printf("Write Done\n\r");

	SPI_Read(0, rx_buff, size);

	bbu_printf("Read Done\n\r");

	for(int i = 0; i < size/4; i++)
	{
		if(tx_buff[i] != rx_buff[i]){
			bbu_printf("Write/Read FAIL\n\r");
			bbu_mfree(p1);bbu_mfree(p2);
			return;
		}
	}
	bbu_mfree(p1);bbu_mfree(p2);
	bbu_printf("PASS\n\r");
}

void bbu_spi_nor_test(uint8_t argc, uint8_t **argv)
{
	int clk = 0;
	uint32_t size = 128;

	if(argc < 2){
		return ;
	}

	if(strcmp((char *)argv[1], "init")==0){
		if(argc >= 3 )
			clk = conv_dec(argv[2]);
		ssp_init_clk = clk;
		spi_host_init();
		bbu_printf("Init host for SPI NOR, clk: %d\n\r", ssp_init_clk);
		return;
	}

	if(strcmp((char *)argv[1], "wipe")==0){
		SPINOR_Wipe();
		bbu_printf("Wipe Done\n\r");
		return;
	}

	if(strcmp((char *)argv[1], "rdp")==0){
		SPINOR_Rdp();
		bbu_printf("Release device from deep power down\n\r");
		return;
	}

	if(strcmp((char *)argv[1], "rw")==0){
		if(argc >= 3 )
			size = conv_dec(argv[2]);
		bbu_spi_rw_test(size);
	}
}
