#ifndef MRVL_COUNTER_H
#define MRVL_COUNTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********** register 11, performance counter control(extension) **********/
/* Custom counters (Counter0, Counter1, Counter2, Counter3) */
/* Programming Details: */
#define DISABLE_COUNTER                         0x0

#ifdef FALCON_COUNTER /* Old counter */
/* Counter0: */
#define C0_CYCLE_COUNT                        0x00000001
#else /* New counter */

#define PMU_SOFTWARE_INCREMENT                  0x0
#define PMU_INSTRUCTION_FETCH_L1_REFILL         0x1
#define PMU_INSTRUCTION_FETCH_L1_TLB_REFILL     0x2
/*#define PMU_DATA_L1_REFILL                      0x3 */
#define PMU_DATA_L1_ACCESS                      0x4
#define PMU_DATA_TLB_REFILL                     0x5
#define PMU_DATA_READ                           0x6
#define PMU_DATA_WRITE                          0x7
#define PMU_INSTRUCTION_EXECUTED                0x8
#define PMU_IMMEDIATE_BRANCH                    0x0D
#define PMU_BRANCH_MISPREDICT                   0x10
#define PMU_CYCLE                               0x11
#define PMU_BRANCH                              0x12

#define PMU_DCACHE_READ_HIT                     0x40
#define PMU_DCACHE_READ_MISS                    0x41
#define PMU_DCACHE_WRITE_HIT                    0x42
#define PMU_DCACHE_WRITE_MISS                   0x43
#define PMU_MMU_BUSREQUEST                      0x44
#define PMU_ICACHE_BUS_REQUEST                  0x45
#define PMU_WB_LATENCY                          0x46
#define PMU_LDMSTM_HOLD                         0x47
#define PMU_NODUAL_CFLAG                        0x48
#define PMU_NODUAL_REG_PLUS                     0x49
#define PMU_LDST_ROB0_ONHOLD                    0x4A
#define PMU_LDST_ROB1_ONHOLD                    0x4B
#define PMU_DATA_WRITE_ACCESS                   0x4C
#define PMU_DATA_READ_ACCESS                    0x4D
#define PMU_A2_STALL                            0x4E
#define PMU_L2CACHE_WRITE_HIT                   0x4F
#define PMU_L2CACHE_WRITE_MISS                  0x50
#define PMU_L2CACHE_READ_COUNT                  0x51
/* 0x52-0x5F reserved */
#define PMU_ICACHE_READ_MISS                    0x60
#define PMU_ITLB_MISS                           0x61
#define PMU_SINGLE_ISSUE                        0x62
#define PMU_BRANCH_RETIRED                      0x63
#define PMU_ROB_FULL                            0x64
#define PMU_MMU_READ_BEAT                       0x65
#define PMU_WB_WRITE_BEAT                       0x66
#define PMU_DUAL_ISSUE                          0x67
#define PMU_NODUAL_RAW                          0x68
#define PMU_HOLD_IS                             0x69
#define PMU_L2C_LATENCY                         0x6A
/* not used */
#define PMU_BRANCH_RETIRED_2_INC                0x6B 
/* 0x6C-0x6F reserverd */
#define PMU_DCACHE_ACCESS                       0x70
#define PMU_DTLB_MISS                           0x71
#define PMU_BRANCH_PREDICTION_MISS              0x72
/* 0x73 reserved */
#define PMU_A1_STALL                            0x74
#define PMU_DCACHE_READ_LATENCY                 0x75
#define PMU_DCACHE_WRITE_LATENCY                0x76
#define PMU_NODUAL_REG                          0x77
#define PMU_BIU_SIMUL_ACCESS                    0x78
#define PMU_L2CACHE_READ_HIT                    0x79
#define PMU_L2CACHE_READ_MISS                   0x7A
#define PMU_L2CWB_FULL                          0x7B
/* 0x7C-0x7F reserved */
#define PMU_TLB_MISS                            0x80
#define PMU_BRANCHES_TAKEN                      0x81
#define PMU_WB_FULL                             0x82
#define PMU_DCACHE_READ_BEAT                    0x83
#define PMU_DCACHE_WRITE_BEAT                   0x84
#define PMU_NODUAL_HW                           0x85
#define PMU_NODUAL_MULTIPLY                     0x86
#define PMU_BIU_ACCESS                          0x87
#define PMU_MAIN_ITLB_MISS                      0x88
#define PMU_MAIN_DTLB_MISS                      0x89
#define PMU_ICACHE_READ_BEAT                    0x8A
#define PMU_VALID_B_BL_INSTR_RETIRED		0x8B
/*  0x8C-0x8F reserved */
#define PMU_EXTIN_0				0x90
#define PMU_EXTIN_1				0x91
#define PMU_EXTIN_0_OR_1			0x92
/* 0xA0-0xBF reserved */

/* 0xC0- MMX events */
/* 0xD0- VFP events */
/* 0xE0- External SOC(MP) events */
extern char *mode_str[] = {
	"SOFTWARE_INCREMENT",
	"INSTRUCTION_FETCH_L1_REFILL",
	"INSTRUCTION_FETCH_L1_TLB_REFILL",
	"",
	"DATA_L1_ACCESS",
	"DATA_TLB_REFILL",
	"DATA_READ",
	"DATA_WRITE",
	"INSTRUCTION_EXECUTED",
	"",
	"",
	"",
	"",
	"IMMEDIATE_BRANCH",
	"",
	"",
	"BRANCH_MISPREDICT",
	"CYCLE",
	"BRANCH",       /* 0x12 */
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",     /* 0x1f */
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",     /* 0x2f */
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",     /* 0x3f */
	"DCACHE_READ_HIT",
	"DCACHE_READ_MISS",
	"DCACHE_WRITE_HIT",
	"DCACHE_WRITE_MISS",
	"MMU_BUSREQUEST",
	"ICACHE_BUS_REQUEST",
	"WB_LATENCY",
	"LDMSTM_HOLD",
	"NODUAL_CFLAG",
	"NODUAL_REG_PLUS",
	"LDST_ROB0_ONHOLD",
	"LDST_ROB1_ONHOLD",
	"DATA_WRITE_ACCESS",
	"DATA_READ_ACCESS",
	"A2_STALL",
	"L2CACHE_WRITE_HIT",    /* 0x4F */
	"L2CACHE_WRITE_MISS",
	"L2CACHE_READ_COUNT", /* 0x51 */
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"ICACHE_READ_MISS",     /* 0x60 */
	"ITLB_MISS",
	"SINGLE_ISSUE",
	"BRANCH_RETIRED",
	"ROB_FULL",
	"MMU_READ_BEAT",
	"WB_WRITE_BEAT",
	"DUAL_ISSUE",   /* 0x67 */
	"NODUAL_RAW",
	"HOLD_IS",
	"L2C_LATENCY", /* 0x6A */
	"",
	"",
	"",
	"",
	"",
	"DCACHE_ACCESS",        /* 0x70 */
	"DTLB_MISS",
	"BRANCH_PREDICTION_MISS",
	"",
	"A1_STALL",
	"DCACHE_READ_LATENCY",  /* 0x75 */
	"DCACHE_WRITE_LATENCY",
	"NODUAL_REG",
	"BIU_SIMUL_ACCESS",
	"L2CACHE_READ_HIT",             /* 0x79 */
	"L2CACHE_READ_MISS",
	"L2CWB_FULL",
	"",
	"",
	"",
	"",
	"TLB_MISS",                      /* 0x80 */
	"BRANCHES_TAKEN",
	"WB_FULL",
	"DCACHE_READ_BEAT",
	"DCACHE_WRITE_BEAT",
	"NODUAL_HW",
	"NODUAL_MULTIPLY",
	"BIU_ACCESS",
	"PMU_MAIN_ITLB_MISS",           /*0x88*/
	"PMU_MAIN_DTLB_MISS",           /*0x89*/
	"ICACHE_READ_BEAT",
	"VALID_B_BL_INSTR_RETIRED",     /* 0x8B */
	"",
	"",
	"",
	"",
	"EXTIN_0",               /* 0x90 */
	"EXTIN_1",
	"EXTIN_0_OR_1"          /* 0x92 */
};
#endif 

/*
 * default setting
 */
//#define  MRVL_VERBOSE
#define  MRVL_PMU
//#define  MRVL_OVERFLOW_CHECK

/*
 * initialize the counters events
 */
#ifdef MRVL_PMU
static unsigned int event_type[4]={PMU_DCACHE_WRITE_HIT, PMU_DCACHE_WRITE_MISS, PMU_L2CACHE_WRITE_HIT, PMU_L2CACHE_WRITE_MISS};

#endif //MRVL_PMU

#ifdef LOOP_EVENTS
extern int common_event[]={
	PMU_DCACHE_READ_HIT,
	PMU_DCACHE_READ_MISS,
	PMU_DCACHE_WRITE_HIT,
	PMU_DCACHE_WRITE_MISS,
	PMU_L2CACHE_WRITE_HIT,
	PMU_L2CACHE_WRITE_MISS,
	PMU_L2CACHE_READ_HIT,
	PMU_L2CACHE_READ_MISS,
	PMU_DATA_L1_ACCESS,
	PMU_WB_LATENCY,
	PMU_LDST_ROB0_ONHOLD,
	PMU_LDST_ROB1_ONHOLD,
	PMU_DATA_WRITE_ACCESS,
	PMU_DATA_WRITE_ACCESS,
	PMU_ROB_FULL,
	PMU_HOLD_IS,
	PMU_A1_STALL,
	PMU_WB_FULL,
	PMU_BIU_ACCESS,
	PMU_TLB_MISS
};
#endif //LOOP_EVENTS

#define MYINLINE inline static
typedef unsigned int     _mrvl_uint32;


MYINLINE void reset_all_counters() {
	reset_CCNT();
	reset_PMU();
}

MYINLINE void get_all_counters(_mrvl_uint32 *ccnt, _mrvl_uint32 *c0, _mrvl_uint32 *c1, _mrvl_uint32 *c2, _mrvl_uint32 *c3, _mrvl_uint32 uti) {
	/*
	 *initialize the counters
	 *David Yang, 08.11.02
	 */
	disable_counters();
	if( uti == 0 ){
		reset_all_counters();
		enable_counters();
	}
	if(ccnt)
		*(ccnt+uti) = read_CCNT();
	if(c0)
		*(c0+uti) = read_PMCNT(0);
	if(c1)
                *(c1+uti) = read_PMCNT(1);
	if(c2)
	        *(c2+uti) = read_PMCNT(2);
	if(c3)
	        *(c3+uti) = read_PMCNT(3);
	return ;
}


MYINLINE void get_ccnt(_mrvl_uint32 *ccnt, _mrvl_uint32 uti) {
	/*
	 *initialize the counters
	 *David Yang, 08.11.02
	 */
	disable_counters();
	if( uti == 0 ){
		reset_all_counters();
		enable_counters();
	}
	if(ccnt)
		*(ccnt+uti) = read_CCNT();
	return ;
}

#ifdef __cplusplus
}
#endif

#endif /* MRVL_COUNTER_H */
