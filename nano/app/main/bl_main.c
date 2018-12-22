#include "bbu.h"
#include "bsp.h"
#include "pmic.h"
#include "tim.h"
#include "ppset.h"
#include "CIU.h"

#ifdef NANO_SDH
#include "mmc_ops.h"
#endif

#define OSLO 0x4F534C4F
#define SLTS 0x534C5453
#define ATES 0x41544553

static void load_images(void)
{
    TIM tim;
    int imageNum, imageSize, i;
    pIMAGE_INFO_3_4_0 pImage;
    SetTIMPointers((uint8_t *)0xd1000000, &tim);
    imageNum = get_image_number(&tim);

    for(i = 0; i < imageNum; i++){
        pImage = ReturnPImgPtr(&tim, i);
        if( (pImage->ImageID == OSLO) || 
            (pImage->ImageID == SLTS) ||
            (pImage->ImageID == ATES)
        ){
            //mmc_read((pImage->FlashEntryAddr)*512, pImage->LoadAddr, pImage->ImageSize);
        }
    }

}

int main(int argc, char *argv[]){
    SystemInit();

    //pp_init(); 
    //adfc(PP1_3);

#ifdef NANO_SDH
    if(mmc_card_init())
        bbu_printf("Error: mmc init faild!\r\n");

    if(platform_is_realsi())
        load_images();
#endif
    bbu_printf("NANO transfer control to BBU\n\r");

#ifdef CONFIG_CA7
#ifdef CONFIG_CP_ALONE
    //AP work is done here
    if(platform_is_emulator())
        CIU->SW_BRANCH_ADDR = 0x4; //CP runs at 0x0 in DDR
    bbu_printf("CP alone, AP enter dead loop\n\r");
    while(1);
#endif
    //mmc_read(0xF00000, 0x8000000, 20*1024*1024);
    if(platform_is_emulator())
        transfer_ctrl(0x1000000);
    else
        transfer_ctrl(0x8000000);
#endif

#ifdef CONFIG_CR5
    transfer_ctrl(0x100000);
#endif

}
