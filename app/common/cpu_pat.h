#ifndef __CPU_PAT_H__
#define __CPU_PAT_H__

unsigned long cpu_test(int argc, unsigned char** argv);

unsigned long scan(unsigned long data);
unsigned long cpu_test_101(void );
unsigned long cpu_test_102(void );
unsigned long cpu_test_103(void );
unsigned long cpu_test_104(void );
unsigned long cpu_test_133(void );
unsigned long cpu_test_147(void );

#define ARM_MSG *(unsigned long *)0xd11005a0

#ifdef RTL_SIM
    #define BK_NUM                2
    #define ROW_NUM               2
    #define COL_NUM               512
    #define MEM_MAX_SIZE          128
    #define STOP_IF_ERROR
#else
    #define BK_NUM                4
    #define ROW_NUM               4096
    #define COL_NUM               512
    #define MEM_MAX_SIZE         (BK_NUM * ROW_NUM * COL_NUM * 4)
#endif

#define SIM_END                   0x76540000
#define OVERFLOW                  0x76540001
#define SIM_FAIL                  0x76540002
#define SIM_SUCCESS               0x76540003
#define ENTER_C                   0x76540004
#define PAT_START                 0x76541000
#define PAT_END                   0x76542000
#define TEST_FAIL                 0x76543000
#define TEST_PASS                 0x76544000
#define FAILED_NUMBER             0x76545000

extern unsigned long cpu_test_start_address;

#define REG_WRITE(addr,data) ((*(volatile unsigned long *)(addr)) = data)
#define MEM(addr)            *(volatile unsigned long *)(addr+cpu_test_start_address)
#define up_rand(rand)        rand      = rand << 1 | (((rand >> 3) ^ (rand >> 13)) & 0x1)

#define SEED                 0x01234567

#endif
