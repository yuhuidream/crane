/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/******************************************************************************
 * Filename: cacheI.h
 *
 * DESCRIPTION:
 *  Cache Instruction bbu-test
 ******************************************************************************/

#ifndef H_CACHE_I_H
#define H_CACHE_I_H

/*********** NOTE about API **********************************
* Refer is also the file cacheI_data.c
* having config-data which change the test behaviour.
**************************************************************/

typedef enum e_cacheI_tstNo_tag
{
	CASHE_I__SHORT_32WORDS,
	CASHE_I__L1ONLY,
	CASHE_I__L1L2,
	//----------------RAM tests-----------------
	CASHE_I__SRAM_APP,
	CASHE_I__SRAM_COM,
	CASHE_I__SQU,      // relevant for FLAVOR_COM
	CASHE_I__ITCM,     // relevant for FLAVOR_COM
	CASHE_I__DTCM,     // relevant for FLAVOR_COM run Instruction from Data-TCM
	//------------------------------------------
	CASHE_I__NUM_TESTS
}e_cacheI_tstNo;


typedef struct s_cacheI_cntr_tag
{
	uint32_t  cntrR0;       //Counter in ASM-CMD register R0
	uint32_t  accessSlow;   //Memory access counter non-cached
	uint32_t  accessFast;   //Memory access counter in-cache
	uint32_t  spare;
}s_cacheI_cntr;

typedef /*const*/ struct s_cacheI_dataIn_tag
{
	uint32_t* startAddr;
	uint32_t  sizeBytes;
	uint32_t  codePattern;
	uint32_t  loops;      // 2 for Cache, 1 for ITCM or RAM

	s_cacheI_cntr  expectCntr;
}s_cacheI_dataIn;

typedef s_cacheI_cntr   s_cacheI_dataOut;

extern s_cacheI_dataIn  cacheI_dataIn[CASHE_I__NUM_TESTS];

//=====================================================
uint8_t casheI(uint32_t repeat, uint8_t silent);
int casheI_tstNo(uint32_t tstNo, uint8_t silent);

#endif//_CACHE_I_H_
