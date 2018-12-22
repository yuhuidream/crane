#include "bbu.h"
#include "CIU.h"
#include "mrvl_amp.h"
#include "shared_mem.h"
#include "timer_dec.h"
#include "pmu_dec.h"
#include "tim.h"
#include "PMUM.h"

//#define CP_ASSERT_HANDLE
#if 0
void set_seagull_boot_vector_low(void)
{
    CIU->CR5_CONF &= ~CR5_CONF_VINITHI0;
}

void set_seagull_boot_vector_high(void)
{
    CIU->CR5_CONF |= CR5_CONF_VINITHI0;
}

void release_seagull_from_reset(void)
{
    uint32_t val;
//    val = BU_REG_READ(PMUM_APRR);
//    val &= ~PMUM_APRR_CPR;
//    BU_REG_WRITE(PMUM_APRR, val);
    *(unsigned long*)0xD4051020 = *(unsigned long*)0xD4051020 & ~0x1;
}

void boot_seagull_from_ddr(void)
{
#ifndef CONFIG_NATO
    /* Need to make sure the program for CR5 is placed at 0x0 in DDR
    */
    CIU->SW_BRANCH_ADDR = CR5_BRANCH_ADDR;
    PMUM->SGADDRMAP &= ~BIT_16;
    //PMUM->SGADDRMAP = (CR5_BRANCH_ADDR >> 16) | BIT_16;
    CIU->CR5_CONF &= ~BIT_9;   //disable ATCM at reset
    CIU->CR5_CONF &= ~BIT_10;  //disable BTCM at reset
    memcpy((void *)0, (void *)CR5_BRANCH_ADDR, 0x10000);
    set_seagull_boot_vector_low();
    release_seagull_from_reset();
    bbu_printf("Release seagull!!\n\r");
#else
    *(volatile unsigned long*) 0xD4282C04 &= ~0x1;
    BU_REG_WRITE(0xd4282888, 0x18060002);
    BU_REG_WRITE(0xd4282888, 0x08060002);
#endif
}

void boot_seagull_from_tcm(void)
{
    // halt the CR5 CPU
    CIU->CR5_CONF &= ~CR5_CONF_NCPUHALT0;
    CIU->CR5_CONF |= BIT_9;   //Enable ATCM at reset
    CIU->CR5_CONF |= BIT_10;  //Enable BTCM at reset
    CIU->CR5_CONF |= BIT_11;  //Set ATCM base address as 0

    set_seagull_boot_vector_low();

    release_seagull_from_reset();

    /* move program for CR5 to TCM through DMA
       Currently, the load address of CR5 image is 0x100000,
       But thr execute address of the first 32K is 0x0(ATCM)
    */
    if(DDR_START_ADDR == 0)
        if((*(volatile uint32_t *)0x1FF0000 & 0xFF) == 0xDB){
           bbu_printf("Debugging with XDB, don't need to do DMA to TCM\n\r" BBU_PROMPT);
           *(volatile uint32_t *)0x1FF0000 &= ~0xFF;
           CIU->CR5_CONF |= CR5_CONF_NCPUHALT0;
           return;
        }

    dma_m2m_xfer(CR5_BRANCH_ADDR, 0xB0010000, 0x10000, DMA_MAX_BYTES_PER_DESC, 2, 0);

    /* resume the CR5 CPU */
    CIU->CR5_CONF |= CR5_CONF_NCPUHALT0;
}

void boot_seagull_from_high_vector(uint32_t branchAddr)
{
    CIU->CR5_CONF |= BIT_9;   //Enable ATCM at reset
    CIU->CR5_CONF |= BIT_10;  //Enable BTCM at reset
    CIU->CR5_CONF |= BIT_11;  //Set ATCM base address as 0

    /* CR5 boot up from high vector address 0xFFFF0000
    */
    set_seagull_boot_vector_high();

    /* Set the branch address for CR5, when boot rom finds current core
     * is CR5, it will jump to the address in SW_BRANCH_ADDR when REMAP disabled
     *  and it will jump to the remapped address in PMUM->SGADDRMAP when high vector boot.
     */
    CIU->SW_BRANCH_ADDR = branchAddr;
    //PMUM->SGADDRMAP &= ~BIT_16;
    PMUM->SGADDRMAP = (branchAddr >> 16) | BIT_16;
    while ( PMUM->SGADDRMAP != ((branchAddr >> 16) | BIT_16) );
    release_seagull_from_reset();
}
#endif
#if defined(CONFIG_CA7) && defined(CP_ASSERT_HANDLE)
static void cp_assert_ap_handler(void *arg)
{
    bbu_printf("\n\n\rAP: CP assert, need to reset it\n\r");
    bbu_printf("AP: Reset CP...\n\n\r");
    comm_test(1, 0, 1);
}
#endif

void boot_up_seagull(Cr5BootType type, uint32_t address, int check)
{
    /* When BBU is burnt to flash, check tim to decide if need to release CR5
    */
    TIM tim;
    int imageNum, imageSize, i;
    pIMAGE_INFO_3_4_0 pImage;

    if(DDR_START_ADDR == 0)
        if((*(volatile uint32_t *)0x1FF0000 & 0xFFF00) == 0xC1500){
           bbu_printf("Debugging with XDB and CR5 is not released\n\r"BBU_PROMPT);
           *(volatile uint32_t *)0x1FF0000 &= ~0xFFF00;
           return;
        }

#if 0
    SetTIMPointers(0xd1000000, &tim);
    imageNum = get_image_number(&tim);
    if(imageNum < 4){
       //BBU which contains CR5 has 4 images at least
       bbu_printf("CR5 won't be release as only %d images are found\n\r"BBU_PROMPT, imageNum);
       return;
    }
    for(i = 0; i < imageNum; i++){
       pImage = ReturnPImgPtr(&tim, i);
       if(pImage->LoadAddr == 0x100000){
          break;
       }
    }
    if(i == imageNum){
        bbu_printf("CR5 won't be release as its image is not found\n\r"BBU_PROMPT);
        return;
    }else{
        imageSize = pImage->ImageSize;
        if(imageSize < 0x80000){
           bbu_printf("CR5 won't be release as its image is invalid\n\r"BBU_PROMPT);
           return;
        }
    }
#endif
    bbu_printf("Release Seagull, please wait...\n\r");
    uint32_t time_out = 0x4000;

    CR5_RLS_FLAG = CR5_IN_RESET;

    switch(type){
        case FROM_HIGH_VECTOR:
            boot_seagull_from_high_vector(address);
            break;
        case FROM_LOW_DDR:
            boot_seagull_from_ddr();
            break;
        case FROM_LOW_TCM:
            boot_seagull_from_tcm();
            break;
    }

    if(check){
        while(time_out--){
        	if(CR5_RLS_FLAG == CR5_OUT_RESET){
                bbu_printf("Seagull is running up\n\r");
#if defined(CONFIG_CA7) && defined(CP_ASSERT_HANDLE)
                start_specific_timer(0, cp_assert_ap_handler, TIMER_ONE_SHOT, 0, 0, 1, TIMER_CLK_13M);
#endif
        		return;   //CR5 is released successfully
        	}
        	else
        		udelay(10);
        }
        bbu_printf("ERROR: Fail to release Seagull in time\n\r");
    }
}

void seagull_say_hello(void)
{
	CR5_RLS_FLAG = CR5_OUT_RESET;
   sg_ind = 1; //indicate to AP silent reset Segaull is up
}

static void bbu_cpld_help(void)
{
    bbu_printf("cpld [<load_addr>] [<high>]\n\r");
    bbu_printf(" <load_addr> : DDR address the CP image is at, default is 0x100000\n\r");
    bbu_printf(" <high>      : 1, release CP from high vector; 0, low vector \n\r");
    bbu_printf("BBU> ");
}

void bbu_cpld(int argc, uint8_t **argv)
{
    uint32_t load_addr = 0x100000;
    int high = 0;
    if(argc > 1)
    {
        load_addr = conv_hex((char *)argv[1]);
    }else{
        bbu_cpld_help();
        return;
    }

    if(argc > 2)
    {
        if(check_help( (char *)argv[2] ) ) {
            bbu_cpld_help();
            return;
        }else
            high = conv_dec((char *)argv[2]);
    }

    if(high)
    {
        bbu_printf("Boot CP from high vector, load address is 0x%08X\n\r", load_addr);
        boot_up_seagull(FROM_HIGH_VECTOR, load_addr, 0);
    }else{
        bbu_printf("Boot CP from low vector, load address is 0x%08X\n\r", load_addr);
        boot_up_seagull(FROM_LOW_DDR, load_addr, 0);
    }
    bbu_printf("BBU> ");
}

#ifdef CONFIG_CR5
static void cp_assert_timer_handler(void *argv)
{
    bbu_printf("\n\n\rCP: Assert, arm timer to inform AP to reset itself\n\r");
    start_specific_timer(500, 0, TIMER_ONE_SHOT, 0, 0, 1, TIMER_CLK_13M);
}

void cp_assert_cp_handler(void)
{
#ifdef CP_ASSERT_HANDLE
    enable_irq_fiq();
    start_timer(100, cp_assert_timer_handler, TIMER_ONE_SHOT);
#endif
}
#endif
