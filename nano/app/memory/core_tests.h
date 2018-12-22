#ifndef BBU_CORE_TESTS_H
#define BBU_CORE_TESTS_H

extern  int bbu_putchar(char u8c);
#define BBU_putstr(str_)  bbu_printf("%s", str_)
#define BBU_puthexw(hex_) bbu_printf("0x%08X", hex_)
#define BBU_puthexh(hex_) bbu_printf("0x%04X", hex_)
#define BBU_puthexb(hex_) bbu_printf("0x%02X", hex_)
#define BBU_putchr(chr_)  bbu_putchar(chr_)
#define BBU_crlf()        bbu_printf("\n\r")
//definitions
#define L1_ICACHE_SIZE 0x8000  //32KB
#define L1_DCACHE_SIZE 0x8000  //32KB

#define L1_DCACHE_SETS_MASK     0x0FFFE000
#define L1_DCACHE_SETS_OFFSET   13
#define L1_DCACHE_WAYS_MASK     0x00001FF8
#define L1_DCACHE_WAYS_OFFSET   03
#define L1_DCACHE_WORDS_PER_LINE_MASK     0x07
#define L1_DCACHE_WORDS_PER_LINE_OFFSET   0x00

#define L1_DCACHE_CHECK_WHOLE_SIZE   0x00
#define L1_DCACHE_CHECK_WAYS_SIZE    0x01

typedef enum
{
	SCOUT_AREA_TYPE_SRAM_TEST_AREA = 0,
	SCOUT_AREA_TYPE_VIRTUAL_TEST_AREA = 1,
	SCOUT_AREA_TYPE_DDR_TEST_AREA = 2,	
}SCOUT_AREA_TYPE;
 
typedef enum
{
	ACCESS_TYPE_BYTE = 0,
	ACCESS_TYPE_HWORD = 1,
	ACCESS_TYPE_WORD = 2,
}ACCESS_TYPE;

typedef enum
{
	CACHE_TEST_L1_ONLY = 0,
	CACHE_TEST_L1L2 = 1,
	CACHE_TEST_SRAM = 2,
}CACHE_TEST;

typedef enum
{
	CACHE_DISABLE = 0,
	CACHE_ENABLE = 1,
}CACHE_OPERATION;

typedef enum
{
	SILENT_MODE  = 0,
	SIGN_MODE    = 1,
	MINIMAL_MODE = 2,
	FULL_MODE    =3
}PRINT_MODE;

typedef enum
{
	MTSP_OPERATION_OK     = 0 ,
	MTSP_WRONG_PARAMETER  = 1 ,
	MTSP_READ_FAILED      = 2 ,  // Reading pattern did not matched the writing
	MTSP_ERROR            = 3 ,  // general error
	MTSP_DIR_ERR          = 4 ,  // direction unknown
	MTSP_DRIVE_ERR        = 5 ,  // drive unknown
	MTSP_CPU_UNKNOWN      = 6 ,  
}MTSP_TEST_VAL;

typedef enum
{
	PATTERN_NUMBER_ONE   = 0,
	PATTERN_NUMBER_TWO   = 1,
	PATTERN_NUMBER_THREE = 2,
  PATTERN_NUMBER_FOUR  = 3,
	PATTERN_NUMBER_FIVE  = 4,
	PATTERN_NUMBER_SIX   = 5,
	PATTERN_NUMBER_SEVEN = 6,
	PATTERN_NUMBER_EIGHT = 7,
	PATTERN_NUMBER_NINE  = 8,
	PATTERN_NUMBER_TEN   = 9,
	PATTERN_NUMBER_ELEVEN      = 10,
	PATTERN_NUMBER_TWELVE      = 11,
	PATTERN_NUMBER_THIRTEEN    = 12,
	PATTERN_NUMBER_FOURTEEN    = 13,
	PATTERN_NUMBER_FIFTEEN     = 14,
	PATTERN_NUMBER_SIXTEEN     = 15,
	PATTERN_NUMBER_SEVENTEEN   = 16,
	PATTERN_NUMBER_EIGHTEEN    = 17,
	PATTERN_NUMBER_NINETEEN    = 18,
	PATTERN_NUMBER_TWENTY      = 19,
	PATTERN_NUMBER_TWENTYONE   = 20,
	PATTERN_NUMBER_TWENTYTWO   = 21,
	PATTERN_NUMBER_TWENTYTHREE = 22,
  PATTERN_NUMBER_TWENTYFOUR  = 23,
	PATTERN_NUMBER_TWENTYFIVE  = 24,
	PATTERN_NUMBER_TWENTYSIX   = 25,
	PATTERN_NUMBER_TWENTYSEVEN = 26,
	PATTERN_NUMBER_TWENTYEIGHT = 27,
	PATTERN_NUMBER_TWENTYNINE  = 28,
	PATTERN_NUMBER_THIRTY      = 29,
	PATTERN_NUMBER_THIRTYONE   = 30,
	PATTERN_NUMBER_THIRTYTWO   = 31,	
}PATTERN_NUMBER;

typedef enum
{
	SCREEN_APPS_ONLY = 0,
	SCREEN_COMM_ONLY = 1,
	SCREEN_APPS_COMM = 2,
}SCREEN_OPTIONS;

typedef struct _PatternDwords
{
  uint32_t     First_Pattern;
	uint32_t     Second_Pattern;
} PatternDwords;

typedef struct{
	uint32_t address;
	uint32_t pattern;
	uint32_t readDword;
	uint32_t L2cacheDataTest;
	uint32_t cacheSize;
	}dcacheparams;

//Declerations
typedef struct s_Err_Info_tag
{
	uint32_t        err_defected_bits;	   //'ORed' defected bits (each bit corresponds to relevant ddr data line)
	uint32_t   			errors_counter;        //Count the number of errors till test stop/completed.
}s_Err_Info;



typedef struct s_MTSP_Result_Info_tag
{
	uint32_t              lastOperation;	   //Last operation acually occured between last write before read.
	uint32_t   			failureAddress;    //DDRTest_Mode_Enum    TestMode;
	s_Err_Info          err_info_ap;
	s_Err_Info          err_info_cp;
  //s_Err_Info          err_info_gb;
	uint32_t              failureCore;
}s_MTSP_Result_Info;


//////////////////////////////////////////
//////    Functions Declerations     /////
//////////////////////////////////////////
void bbu_L1_icache_enable(uint8_t enable);
void bbu_L1_dcache_enable(uint8_t enable);
void bbu_L2_cache_enable(uint8_t enable);
uint32_t bbu_mtsp_test(uint32_t Address, uint32_t Size, uint32_t Pattern, uint32_t preReadAction);
uint32_t bbu_memory_test(uint32_t Address, uint32_t Size, uint32_t Pattern, uint8_t Access, uint8_t Mtsp);
uint32_t get_mtsp_test_results_ptr(void);
void   clear_mtsp_test_results(void);
void   set_print_level(PRINT_MODE printLev);
PRINT_MODE get_ap_print_level(void);
void set_increment(int increment);
uint32_t bbu_icache_test(uint32_t Address, uint32_t Size, uint32_t Parameter, PRINT_MODE silent);
uint32_t bbu_dcache_test(uint32_t Address, uint32_t Size, uint32_t Parameter, PRINT_MODE silent);
uint32_t bbu_L2cache_test(uint32_t Address, uint32_t Size, uint32_t Parameter);
int bbu_screening_test(SCREEN_OPTIONS option, PRINT_MODE silent);

int dcacheFooTest(dcacheparams *par, PRINT_MODE silent);
uint8_t dcacheThoroughTest(uint32_t startAddress);
uint32_t ReadCoreFreq(PRINT_MODE silent);
void DataCacheCleanAndInvalidateWithoutL2(void);
uint32_t GetDataCacheWayStatus(void);
void   SetDataCacheWayStatus(uint32_t total_ways, uint32_t ways_status);
void   LockDataCacheAllWays(uint32_t total_ways);
void   UnLockDataCacheWay(uint32_t total_ways,uint32_t way_num);
uint32_t disableInterrupts(void);
void restoreInterrupts(uint32_t Value);
int bbu_smtd(int argc, uint8_t **argv);
int bbu_mtsp(int argc, uint8_t **argv);
//Macro
//#define bbu_ror(Value,Number)  Value = Value >> Number
#define bbu_ror(Value, Number)  Value = (Value << (sizeof(uint32_t)*8-Number)) | (Value >> Number)

#endif
