#ifndef bbu_h
#define bbu_h
#include <stdint.h> 
#include "predefines.h"
#include "print.h"
#include "utils.h"
#include "system_mrvl.h"
#include "bbu_help.h"


#ifdef CONFIG_CA7
#define BBU_PROMPT         "CA7> "
#endif
#ifdef CONFIG_CR5
#define BBU_PROMPT         "CR5> "
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/*
 * Macro for screen test result return 
*/
#define SCREEN_TEST_PASS 1
#define SCREEN_TEST_FIAL 2
#define SCREEN_TEST_COMPLETED_NEED_CHECK 3
#define SCREEN_TEST_TIMEOUT 4
#define SCREEN_TEST_UNKNOWN_CMD 5

//#define BBU_TEST_PASS       (1<<0)    /* Test failed */
//#define BBU_TEST_FAIL       (1<<1)    /* Test pass */
//#define BBU_TEST_CHECK      (1<<2)    /* Test completed, and need people to check */
//#define BBU_TEST_TIMEOUT    (1<<3)    /* Do not receive ack from test owner in time */
//#define BBU_TEST_UNKNOWN    (1<<4)    /* Unknown test command */

#define BBU_TEST_PASS       1    /* Test failed */
#define BBU_TEST_FAIL       2    /* Test pass */
#define BBU_TEST_CHECK      3    /* Test completed, and need people to check */
#define BBU_TEST_TIMEOUT    4    /* Do not receive ack from test owner in time */
#define BBU_TEST_UNKNOWN    5    /* Unknown test command */


/*  board version detect functions */
int Board_Is_V11(void);
int Board_Is_V10(void);
int board_is_tablet_v10(void);

//This function will delay about (2*cycle) CPU cycles
extern void cpu_cycle_delay(uint32_t cycle);

extern int is_test_buffer_safe(uint32_t addr);


#define CR5_RELEASED     1
#define CR5_IN_RESET     0

#define CORE_SHARED_BUF_LEN  0x400
//#define CR5_CA7_SHARED_BUF   (0xD1010000-CORE_SHARED_BUF_LEN)
#define CR5_CA7_SHARED_BUF   (0xD1001500)
#define CA7_TASK_ID_BUF      CR5_CA7_SHARED_BUF
#define CA7_TASK_ARGC_BUF    (CA7_TASK_ID_BUF+0x4)
#define CA7_TASK_ARGV_BUF    (CA7_TASK_ARGC_BUF+0x4)
#define CA7_TASK_RTRN_BUF    (CA7_TASK_ARGC_BUF+0x4)
#define CA7_TASK_CMDLN_BUF   (CA7_TASK_RTRN_BUF+0x4*16)

#define CR5_TASK_ID_BUF      (CA7_TASK_CMDLN_BUF+256)
#define CR5_TASK_ARGC_BUF    (CR5_TASK_ID_BUF+0x4)
#define CR5_TASK_ARGV_BUF    (CR5_TASK_ARGC_BUF+0x4)
#define CR5_TASK_RTRN_BUF    (CR5_TASK_ARGC_BUF+0x4)
#define CR5_TASK_CMDLN_BUF   (CR5_TASK_RTRN_BUF+0x4*16)

#define CP_RELEASE_FLAG_BUF  (CR5_TASK_CMDLN_BUF+256)
#define CORE_SHARED_BUF_END  (CP_RELEASE_FLAG_BUF+4)
#define CP_RELEASE_FLAG   (*((uint32_t *)CP_RELEASE_FLAG_BUF))


enum PlatformType{
    REAL_SI = 0,
    HAPS,
    ZEBU_Z1,
    VDK,
};
int platform_is_zebu();
int platform_is_z1(void);
int platform_is_haps(void);
int platform_is_emulator(void);
int platform_is_realsi(void);
#endif

