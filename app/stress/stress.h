#ifndef _STRESS_H_
#define _STRESS_H_

typedef struct StressCmdEntryTag{
    const char *Cmd;  //pointer to the name string of the command
    const char *StopCmd;
    int8_t   type;
    int8_t   wait;
    int8_t   duration;
    int8_t   alive;
}StressCmdEntry;

typedef enum{
   DDR_STRESS = 1,
   CPU_STRESS,
   PWR_STRESS,
   PE_STRESS,
   PE2_STRESS,
#ifdef CONFIG_GCU
   GCU_STRESS,
#endif
#ifdef CONFIG_VPU
   VPU_STRESS,
#endif
   MINI_DDR_STRESS,
   SOC_STRESS,
   CW_STRESS,  //co-work stress
   INVALID_STRESS
}StressType;

typedef struct StressTestEntry{
   const char      *cmd;
   const char      *name;
   StressCmdEntry  *cmdArray;
   int              cmdNum;
   StressType       type;
}StressTestEntry;

#define DISARMED     0
#define ONE_SHOT     1
#define PERIODIC     2
#define SPECIAL      3
#define TRIGGER      4
#define COSTRESS     5

#define TESTALONE    0
#define TESTMASTER   1
#define TESTSLAVE    2

#define CO_TEST_GOON 0
#define CO_TEST_FAIL 1
#define CO_TEST_PASS 2

int continue2stress(void);
int isAutoRunStress(void);
void stress_trigger_next(void);
extern void stress_cmd_ack(int result, char *info);
void stress_test_fail(char *info);
void stress_test_ack(char *info);
#endif
