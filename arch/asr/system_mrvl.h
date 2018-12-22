/*******************************************************************************
 * @file     system_nevo.h
 * @brief    CMSIS-like HAL Nevo System Header File
 *           mainly configure the clock and memory map,etc.
 * system_nevo.c will call pin-mux, timer initialization functions from related unit driver
 * @note
 * Copyright (C) 2011 Marvell. All rights reserved.
 *
  ******************************************************************************/


#ifndef __SYSTEM_MRVL_H
#define __SYSTEM_MRVL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "predefines.h"
//--------------------- Clock Configuration ----------------------------------


//--------------------Memory Configuration and Memory map======================



//-------- <<< end of configuration section >>> ------------------------------

#define     NUM_CORES			4

#define 	CORE_DRAGONITE	 	0
#define 	CORE_CA9_SMP0		1
#define 	CORE_CA9_SMP1		2
#define 	CORE_PJ_MM		  3

// stack location define
/***************************************************************************
* WARNING: must comply with stack define in startup_nevo.s
****************************************************************************/
#define STACK_MMU_IN_SQU 0                      /* define only one */
#define STACK_MMU_IN_DRAM  1

#define ITCM_BASE       		0x0
#define ITCM_SIDEBAND_BASE 		0xB0010000
#define ITCM_SIZE       		0x8000
#define DTCM_BASE       		0x10000
#define DTCM_SIDEBAND_BASE 		0xB0020000
#define DTCM_SIZE       		0x8000


/****************************************************************/
/* 	SQU memory map with stack in SQU 							*/
/*																*/
/*	First SQU start from 0xd1000_0000							*/
/*				---------------------							*/
/*	0xD100_FFFC	| stack	downward	|							*/
/*				| ...				|							*/
/*				| ...				|							*/
/*	0xD100_C000	| RW region			|							*/
/*				---------------------							*/
/*				| ...				|							*/
/*				| ...				|							*/
/*	0xD100_4000	| MMU table			|							*/
/*				---------------------							*/
/*				| ...				|							*/
/*				| ...				|							*/
/*	0xD100_0010	| Reserved			|							*/
/*				---------------------							*/
/*				| Test Result:		|							*/
/*				| good: xx0D		|							*/
/*	0xD100_0000	| bad:	xxBD		|							*/
/*				---------------------							*/
/*	Rest of first SQU 													*/
/*				---------------------							*/
/*	0xD101_0000	| ...				|							*/
/*				| ...				|							*/
/*				| Not cacheable		|							*/
/*	0xD102_0000	| Data				|							*/
/*				---------------------							*/
/****************************************************************/
#if (STACK_MMU_IN_SQU)

#ifdef CPU_AP
/*	Need to COMPLY with test_start.s or test_start_sim.s		*/
	#define SQU_BASE_ADDRESS 			(0xD1000000)
	#define TEST_RESULT_1		(SQU_BASE_ADDRESS + 0x0000)
	#define TEST_RESULT_2		(TEST_RESULT_1 + 0x0004)
	#define TEST_RESULT_3		(TEST_RESULT_2 + 0x0004)
	#define TEST_RESULT_4		(TEST_RESULT_3 + 0x0004)
	#define SQU_RESERVED_START		(SQU_BASE_ADDRESS + 0x0100)
	#define SQU_RESERVED_SIZE		(0x1000 - 0x0100)     //0xf00
	#define SQU_RW_REGION_START		(SQU_RESERVED_START + SQU_RESERVED_SIZE) //0x1000
	#define SQU_RW_REGION_SIZE		(0x1000)	//0x2000
	#define SQU_STACK_START			(SQU_BASE_ADDRESS + 0x4000 - 4)		//stack size 0x2000
	#define SQU_MMU_TABLE_START		(SQU_STACK_START + 4) //0x4000
	#define SQU_MMU_TABLE_SIZE		(0x4000)
	#define SQU_DATA_NUM_0_NC_START	(SQU_BASE_ADDRESS + 0x10000)
	#define SQU_DATA_NUM_0_NC_SIZE	(0x10000)
#else
/*	Need to COMPLY with test_start.s or test_start_sim.s		*/
	#define SQU_BASE_ADDRESS 			(0xD1008000)
	#define TEST_RESULT_1		(SQU_BASE_ADDRESS + 0x0000)
	#define TEST_RESULT_2		(TEST_RESULT_1 + 0x0004)
	#define TEST_RESULT_3		(TEST_RESULT_2 + 0x0004)
	#define TEST_RESULT_4		(TEST_RESULT_3 + 0x0004)
	#define SQU_RESERVED_START		(SQU_BASE_ADDRESS  + 0x0100)
	#define SQU_RESERVED_SIZE		(0x1000 - 0x0100)
	#define SQU_RW_REGION_START		(SQU_RESERVED_START + SQU_RESERVED_SIZE)
	#define SQU_STACK_START			(SQU_BASE_ADDRESS + 0x4000 - 4)
	#define SQU_DATA_NUM_0_NC_START	(SQU_BASE_ADDRESS + 0x10000)
	#define SQU_DATA_NUM_0_NC_SIZE	(0x8000)
#endif


/* there are more SQU can be enabled */
	#define SQU_DATA_NUM_1_NC_START	(SQU_BASE_ADDRESS + 0x10000)
	#define SQU_DATA_NUM_2_NC_START	(SQU_DATA_NUM_1_NC_START + 0x10000)
	#define SQU_DATA_NUM_3_NC_START	(SQU_DATA_NUM_2_NC_START + 0x20000)
//	#define SQU_DATA_NUM_4_NC_START	(SQU_DATA_NUM_3_NC_START + 0x20000)
	#define SQU_SIZE_ALL_BANK		(0x200000) // according to figure 169: memory map



/****************************************************************/
/* 	DRAM memory map with stack in SQU  							*/
/*	Need to COMPLY with test_start.s or test_start_sim.s		*/
/*																*/
/*				---------------------							*/
/*	End of DRAM	| 					|							*/
/*				| ...				|							*/
/*				| Not Cacheable		|							*/
/*	0x0080_0000	| Data 				|							*/
/*				---------------------							*/
/*				| ...				|							*/
/*				| ...				|							*/
/*	0x0020_0000	| Data & cacheable	|							*/
/*				---------------------							*/
/*				| ...				|							*/
/*				| Cacheable			|							*/
/*	0x0001_0000	| Code & RW region	|							*/
/*				---------------------							*/
/*				| Cacheable			|							*/
/*				| & reserved		|							*/
/*	0x0000_0000	| Vector Table		|							*/
/*				---------------------							*/
/****************************************************************/

/*	Need to COMPLY with test_start.s or test_start_sim.s		*/
	#define DRAM_BASE 				(0x00000000)
	#define DRAM_SIZE 				(0x08000000) /* 128 M dram */
	#define DRAM_ALIAS_BASE 				(0xd0000000)
	#define DRAM_ALIAS_SIZE 				(0x01000000) /* 16 M dram */

	#define DRAM_VTABLE_START		(DRAM_BASE + 0x0000)
	#define DRAM_VTABLE_SIZE		(0x100)
	#define DRAM_RESERVED_START		(DRAM_VTABLE_START + DRAM_VTABLE_SIZE)
	#define DRAM_RESERVED_SIZE		(0x10000 - 0x0100)

#ifdef CPU_AP  // code start with 0x10000(64k), v-table at 0x0
	#define DRAM_CODE_RW_START		(DRAM_RESERVED_START + DRAM_RESERVED_SIZE)
	#define DRAM_CODE_RW_SIZE		(0x100000 -0x10000)
	#define DRAM_DATA_CB_START		(DRAM_CODE_RW_START + DRAM_CODE_RW_SIZE)
	#define DRAM_DATA_CB_SIZE		(0x00800000)
	#define DRAM_DATA_NC_START		(DRAM_DATA_CB_START + DRAM_DATA_CB_SIZE)
	#define DRAM_DATA_NC_SIZE		(DRAM_BASE + DRAM_SIZE - DRAM_DATA_NC_START)
#else			// CP, code start with 0x100000(1M), v-table at 0x0 (TCM)
	#define DRAM_CODE_RW_START		(0x100000 )
	#define DRAM_CODE_RW_SIZE		(0x100000 )
	#define DRAM_DATA_CB_START		(DRAM_CODE_RW_START + DRAM_CODE_RW_SIZE)
	#define DRAM_DATA_CB_SIZE		(0x00800000)
	#define DRAM_DATA_NC_START		(DRAM_DATA_CB_START + DRAM_DATA_CB_SIZE)
	#define DRAM_DATA_NC_SIZE		(DRAM_BASE + DRAM_SIZE - DRAM_DATA_NC_START)
#endif


#endif // (STACK_IN_SQU)

#if (STACK_MMU_IN_DRAM)
/****************************************************************/
/* 	DRAM memory map with stack in DRAM 							*/
/*	Need to COMPLY with test_start.s or test_start_sim.s		*/
/*																*/
/*				---------------------							*/
/*	End of DRAM	| 					|							*/
/*				| ...				|							*/
/*				| Cacheable	        |							*/
/*	0x0080_0000	| Data 				|							*/
/*				---------------------							*/
/*	0x007F_FFFC	| stack	downward	|							*/
/*				| ...				|							*/
/*				| ...				|							*/
/*	0x0020_0000	| Data & cacheable	|							*/
/*				---------------------							*/
/*				| ...				|							*/
/*				| Cacheable			|							*/
/*	0x0001_0000	| Code & RW region	|							*/
/*				---------------------							*/
/*				| ...				|							*/
/*				| ...				|							*/
/*	0x0000_8000	| MMU L2 table		|							*/
/*				---------------------							*/
/*				| ...				|							*/
/*				| ...				|							*/
/*	0x0000_4000	| MMU L1 table		|							*/
/*				---------------------							*/
/*				| Cacheable			|							*/
/*				| & reserved		|							*/
/*	0x0000_0000	| Vector Table		|							*/
/*				---------------------							*/
/****************************************************************/

/*	Need to COMPLY with test_start.s or test_start_sim.s		*/
	#define DRAM_BASE 				(0x00000000)
	#define DRAM_SIZE 				(0x08000000) /* 128 M dram */
	#define DRAM_ALIAS_BASE 				(0xd0000000)
	#define DRAM_ALIAS_SIZE 				(0x01000000) /* 16 M dram */

	#define DRAM_VTABLE_START		(DRAM_BASE + 0x0000)
	#define DRAM_VTABLE_SIZE		(0x100)
	#define DRAM_RESERVED_START		(DRAM_VTABLE_START + DRAM_VTABLE_SIZE)
	#define DRAM_RESERVED_SIZE		(0x4000 - 0x0100)
	#define TEST_RESULT_1			(DRAM_RESERVED_START + 0x0)
	#define TEST_RESULT_2			(DRAM_RESERVED_START + 0x4)
	#define TEST_RESULT_3			(DRAM_RESERVED_START + 0x8)
	#define TEST_RESULT_4			(DRAM_RESERVED_START + 0xc)
	#define DRAM_MMU_L1_TABLE_START	(DRAM_RESERVED_START + DRAM_RESERVED_SIZE)
	#define DRAM_MMU_L1_TABLE_SIZE	(0x4000)
	#define DRAM_MMU_L2_TABLE_START	(DRAM_MMU_L1_TABLE_START + DRAM_MMU_L1_TABLE_SIZE)
	#define DRAM_MMU_L2_TABLE_SIZE	(0x4000)
	#define DRAM_CODE_RW_START		(0x10000)
	#define DRAM_CODE_RW_SIZE		(0x100000)
	#define DRAM_DATA_CB_START		(0x200000)
	#define DRAM_DATA_CB_SIZE		(0x00600000 - 0x4000) /* 16K stack */
	#define DRAM_STACK_START		(DRAM_CODE_RW_START - 4)

/****************************************************************/
/* 	SQU memory map with stack in DRAM 							*/
/*																*/
/*	First SQU 													*/
/*				---------------------							*/
/*	0xD101_FFFF	| ...				|							*/
/*				| ...				|							*/
/*				| Not cacheable		|							*/
/*	0xD100_0000	| Data				|							*/
/*				---------------------							*/
/*	Second SQU 													*/
/*				---------------------							*/
/*	0xD111_FFFF	| ...				|							*/
/*				| ...				|							*/
/*				| not cacheable		|							*/
/*	0xD110_0000	| Data				|							*/
/*				---------------------							*/
/*	Third SQU 													*/
/*				---------------------							*/
/*	0xD121_FFFF	| ...				|							*/
/*				| ...				|							*/
/*				| not cacheable		|							*/
/*	0xD120_0000	| Data				|							*/
/*				---------------------							*/
/****************************************************************/

/*	Need to COMPLY with test_start.s or test_start_sim.s		*/
	#define SQU_BASE_ADDRESS 			(0xD1000000)
	#define SQU_DATA_NUM_0_NC_START	(SQU_BASE_ADDRESS + 0x10000)
	#define SQU_DATA_NUM_0_NC_SIZE		(0x10000)

	#define SQU_DATA_NUM_1_NC_START	(SQU_DATA_NUM_0_NC_START + 0x10000)
	#define SQU_DATA_NUM_2_NC_START	(SQU_DATA_NUM_1_NC_START + 0x20000)
	#define SQU_DATA_NUM_3_NC_START	(SQU_DATA_NUM_2_NC_START + 0x20000)
//	#define SQU_DATA_NUM_4_NC_START	(SQU_DATA_NUM_3_NC_START + 0x20000)
	#define SQU_SIZE_ALL_BANK		(0x200000) // according to figure 169: memory map

#endif // (STACK_IN_DRAM)

/*CPU type and stepping*/
extern uint32_t soc_type;
extern uint32_t soc_step;

static inline int cpu_is_a0(void)
{
        return (soc_step == 0xA0);
}

static inline int cpu_is_b0(void)
{
        return (soc_step == 0xB0);
}

static inline int cpu_is_pxa1920(void)  //Helan-LTE
{
        return (soc_type == 0x1188);
}

static inline int cpu_is_pxa1822(void)  //Nezha2
{
        return (soc_type == 0x1822);
}

static inline int cpu_is_pxa1822_z1(void)
{
        return ((soc_type == 0x1822) && (soc_step == 0xF0));
}

static inline int cpu_is_pxa1822_z2(void)
{
        return ((soc_type == 0x1822) && (soc_step == 0xF1));
}

static inline int cpu_is_pxa1826(void)  //Nezha3
{
        return (soc_type == 0x1826);
}

static inline int cpu_is_pxa1826_z1(void)
{
        return ((soc_type == 0x1826) && (soc_step == 0xF0));
}

static inline int cpu_is_pxa1826_z2(void)
{
        return ((soc_type == 0x1826) && (soc_step == 0xF2));
}

static inline int cpu_is_pxa1826_z1z2(void)
{
        return ((soc_type == 0x1826) && ((soc_step == 0xF2)||(soc_step == 0xF0)));
}

static inline int cpu_is_pxa1826_z3(void)
{
        return ((soc_type == 0x1826) && (soc_step == 0xF3));
}

static inline int cpu_is_pxa1826_zx(void)
{
        return ((soc_type == 0x1826) && ((soc_step == 0xF2)||(soc_step == 0xF0)||(soc_step == 0xF3)));
}

static inline int cpu_is_pxa1826_a0(void)
{
        return ((soc_type == 0x1826) && (soc_step == 0xA0));
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

extern int GetCPUID(void);
extern int smp_processor_id(void);


#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_MRVL_H */
