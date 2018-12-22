/******************************************************************************
 *
 *  (C)Copyright 2005 - 2011 Marvell. All Rights Reserved.
 *
 *  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF MARVELL.
 *  The copyright notice above does not evidence any actual or intended
 *  publication of such source code.
 *  This Module contains Proprietary Information of Marvell and should be
 *  treated as Confidential.
 *  The information in this file is provided for the exclusive use of the
 *  licensees of Marvell.
 *  Such users have the right to use, modify, and incorporate this code into
 *  products for purposes authorized by the license agreement provided they
 *  include this notice and the associated copyright notice with any such
 *  product.
 *  The information in this file is provided "AS IS" without warranty.
 
 ******************************************************************************/

#include "tim.h"

uint32_t SetTIMPointers( uint8_t *pStartAddr, TIM *pTIM_h)
{
	pTIM_h->pConsTIM = (pCTIM) pStartAddr;	// Overlap Contant Part of TIM with actual TIM...

	// Assign a pointer to start of Images Area
	pTIM_h->pImg = (pIMAGE_INFO_3_4_0) (pStartAddr + sizeof (CTIM));

	// Assign a pointer to start of Key Area
	if (pTIM_h->pConsTIM->VersionBind.Version >= TIM_3_4_00)
		pTIM_h->pKey = (pKEY_MOD_3_4_0) ((uint32_t)pTIM_h->pImg + ((pTIM_h->pConsTIM->NumImages) * sizeof (IMAGE_INFO_3_4_0)));
	else
		pTIM_h->pKey = (pKEY_MOD_3_4_0) ((uint32_t)pTIM_h->pImg + ((pTIM_h->pConsTIM->NumImages) * sizeof (IMAGE_INFO_3_2_0)));

	// Assign a pointer to start of reserved area
	if (pTIM_h->pConsTIM->VersionBind.Version >= TIM_3_4_00)
		pTIM_h->pReserved = (uint32_t *) ((uint32_t)pTIM_h->pKey + ((pTIM_h->pConsTIM->NumKeys) * sizeof (KEY_MOD_3_4_0)));
	else if (pTIM_h->pConsTIM->VersionBind.Version >= TIM_3_3_00)
		pTIM_h->pReserved = (uint32_t *) ((uint32_t)pTIM_h->pKey + ((pTIM_h->pConsTIM->NumKeys) * sizeof (KEY_MOD_3_3_0)));
	else
		pTIM_h->pReserved = (uint32_t *) ((uint32_t)pTIM_h->pKey + ((pTIM_h->pConsTIM->NumKeys) * sizeof (KEY_MOD_3_2_0)));

	// Assign a pointer to start of DS
	pTIM_h->pTBTIM_DS = (pPLAT_DS) ((uint32_t)pTIM_h->pReserved + pTIM_h->pConsTIM->SizeOfReserved);
	return (MV_OK);
}

pIMAGE_INFO_3_4_0 ReturnPImgPtr(pTIM pTIM_h, uint32_t ImageNumber)
{
 void* pIMG;

 if (pTIM_h->pConsTIM->VersionBind.Version >= TIM_3_5_00)
	pIMG = (void*) &pTIM_h->pImg[ImageNumber];
 else if (pTIM_h->pConsTIM->VersionBind.Version >= TIM_3_4_00)
	pIMG = (void *) &( ((pIMAGE_INFO_3_4_0) pTIM_h->pImg)[ImageNumber]);
 else
	pIMG = (void *) &( ((pIMAGE_INFO_3_2_0) pTIM_h->pImg)[ImageNumber]);

 return pIMG;
}

uint32_t get_image_number(TIM *pTIM_h)
{
   return pTIM_h->pConsTIM->NumImages;
}

uint32_t get_image_size(uint32_t Image)
{
    TIM tim;
    pTIM pTIM_h = &tim;
    int imageNum;
    pIMAGE_INFO_3_4_0 pImage;

    SetTIMPointers((uint8_t *)TIM_ADDRESS, pTIM_h);

    /* check if there is valid tim in SRAM */
    if( pTIM_h->pConsTIM->VersionBind.Identifier != TIMIDENTIFIER )
    {
        return 0;
    }

    imageNum = pTIM_h->pConsTIM->NumImages;

    for(int i = 0; i < imageNum; i++){
        pImage = ReturnPImgPtr(pTIM_h, i);
        if(pImage->ImageID == Image) {
            return pImage->ImageSize;
        }
    }

    return 0;
}

int load_image_from_flash(uint32_t Image, uint32_t address)
{
	TIM tim;
	pTIM pTIM_h = &tim;
	int imageNum, imageSize, i;
    pIMAGE_INFO_3_4_0 pImage;
    uint32_t load_address;

    SetTIMPointers((uint8_t *)TIM_ADDRESS, pTIM_h);

    /* check if there is valid tim in SRAM */
    if( pTIM_h->pConsTIM->VersionBind.Identifier != TIMIDENTIFIER )
    {
        return 0;
    }

    imageNum = pTIM_h->pConsTIM->NumImages;

    for(i = 0; i < imageNum; i++){
    	pImage = ReturnPImgPtr(pTIM_h, i);
    	if(pImage->ImageID == Image)
    	{
    		//Load image from flash to DDR
    		if(address!=0 && address != 0xFFFFFFFF){
    			load_address = address;
    		}else{
    			load_address = pImage->LoadAddr;
    		}

    		if(load_address == 0xFFFFFFFF)
    			return 0;
            bbu_print_level = MASK_LEVEL;
            mmc_switch_partiton(pImage->PartitionNumber, 0);
            bbu_print_level = INFO_LEVEL;
    		mmc_read_orig(pImage->FlashEntryAddr<<9, (uint8_t *)load_address, pImage->ImageSize);
            return pImage->ImageSize;
    	}
    }

    return 0;
}