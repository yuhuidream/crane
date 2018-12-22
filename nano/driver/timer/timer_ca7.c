#include "predefines.h"
#include "PMR.h"
#include "timer_dec.h"
#include "APBCLK.h"
#include "PMUA.h"
#include "PMUM.h"
#include "ppset.h"

TIME_BASE *timer[2] = {TIME1, TIME2};

void apb_timer_reset(void){
    int i;
    /* timer clock on and timer reset (0x5) */
    BU_REG_WRITE(APBClock_BASE + 0x34, TIMERS0_CLK_RST_RST | TIMERS0_CLK_RST_APBCLK);
    i = 0x200;
    while(i--);

    /*  select timer0 clock 3.25MHz, release reset */
    BU_REG_WRITE(APBClock_BASE + 0x34, (0x3 <<TIMERS0_CLK_RST_FNCLKSEL_BASE) | TIMERS0_CLK_RST_FNCLK | TIMERS0_CLK_RST_APBCLK);
    i = 0x300;
    while(i--);

    /* timer clock on and timer reset (0x5) */
    BU_REG_WRITE(APBClock_BASE + 0x44, TIMERS0_CLK_RST_RST | TIMERS0_CLK_RST_APBCLK);
    i = 0x200;
    while(i--);

    /*  select timer1 clock 3.25MHz, release reset */
    BU_REG_WRITE(APBClock_BASE + 0x44, (0x3 <<TIMERS0_CLK_RST_FNCLKSEL_BASE) | TIMERS0_CLK_RST_FNCLK | TIMERS0_CLK_RST_APBCLK);
    i = 0x300;
    while(i--);

    return;
}

static void _apb_systimer_init(TIME_BASE *timer_p){
    uint32_t i;

    timer_p->CER &= ~CER_T0EN_MASK;   //disable timer0 before configure it
    i = 0x200;
    while(i--);

    /* timer0 3.25M */
    timer_p->CCR &= ~CCR_CS_0_MASK;   //TIME1 timer0 select configurable clock.

    i = 0x200;
    while(i--);

    timer_p->CMR |= CMR_T0MODE_MASK;  //TIME1 timer0 as free running timer mode
    
    /* Set up the preload value register */
    timer_p->PLVRn[0] = 0;    
    /* Set up the preload control regiter, disable the preload function let it free running */
    timer_p->PLCRn[0] = 0;

    timer_p->ICRn[0] |= ICRN_TCLR0_MASK;      //clear timer0 irq

    /* disable all the match interrupts at init stage, open T10 in BSP.c for systick */
    timer_p->IERn[0] = 0x0;

    i = 0x200;
    while(i--);

    /* enable the timer to kick off counter */
    timer_p->CER = 0x1;   //enable timer0 after configure

    return;
}

void apb_systimer_init(void){
    int i = 0;
    for(i = 0; i<2; i++){
        _apb_systimer_init(timer[i]);
    }

    return;
}

/*Let the TIME1_timer0 at free running, and don't enable the preload when a
  match occurs between TMR_Tn_Tm and Timer Count Registers.
  At the matching, BSP.c will add the match value in Tn_Tm in the match interrupt
  */
void sys_timer_init(void) //This funciton is related to OS tick, Care to modify!
{
    apb_timer_reset();
    apb_systimer_init(); //this is the timers on APB bus
#ifdef CONFIG_GENERIC_TIME
    generic_timer_config(); //this is the global generic timers, for delay functions,etc.
#endif
}

static uint32_t abs_diff(uint32_t value1, uint32_t value2) {
   if (value1<value2){
      return (value2 - value1);
   }else{
      return (value1 - value2);
   }
}

uint32_t TIMER_GET_INTERVAL(uint32_t end, uint32_t begin){
//The end can be smaller than begin due to metastability issue.
//If the delta is within 0x10000000(about 1min, it cost time when cache is not enabled yet at boot), 
//we assume it's acceptable and cover the metastability.
//if the delta is bigger than 0x10000000, we think it's overflow.
//The max error is about 1mS (3250) according to test with 3.25MHz clock.
    if(abs_diff(end, begin) < 0x10000000){
        return ((end >= begin) ? (end - begin) : 0);
    }else{
        Q_ASSERT(end < begin); //it's a true oveflow
        return (TIMER0_MAX_CNT - begin + end);
    }
}

uint32_t timer_get_count(int id) //This funciton is related to OS tick, Care to modify!
{
        /* read twice, this is ok as long as timer runs at slow clock, i.e. 32KHz.
         * because CPU is fast enough to read twice and get same count value to avoid metastablility.
         * If its too fast, then two subsequent reads might give two different values.
         * So using the TCVWRn register to snapshot the count first then read.
         * However, it needs to wait 3 functinal(the timer functional clock selected)  clock cycles 
         * before you issue a read after writing 1 into the register.
         * Has asked silicon to add a status register to avoid such issue.
         */
        
         uint32_t read1 = 0;
         uint32_t delay = 0; //if cache is not enabled, this value should be measured.
    
    /*Note: Don't choose read twice method if it's not 32KHz clock because metastability may cause read2 smaller than read1
      (may be 2000clocks in the test). so that udelay and mdelay are not accurate*/
    #if (1) //when choose the read-twice method, the XDB can't interrupt the while loop
        uint32_t read2 = 0; 
    //if CPU is faster than ~15cycle(below code cost) * 3.25M = 64 M, then it should get two same reads.
    // but test shows even 312M core speed need two cycles, so always set delay to 32
    //if Timer is slow, e.g. configured as 32K TIMER, read twice can get same.
        	//delay = 1; 
    //if CPU is too slow, the min CPU freq 156 measured as 1~2M without cache, (3.25/2)*15~30, 
    //Note: timer can be 13M at MAX, so we may need adjust the numbers according to the timer functional clock selected.
    	delay = 32;  

        do{ 
            read1 = SYSTIME->CRn[id];
            read2 = SYSTIME->CRn[id];
    	}while ((read2 < read1)||((read2 -read1) > delay )); //the filter condition since read2 may be smaller than read1 due to metastability
            
    #else //use the snapshot way
         delay = 3; //Max CPU freq. 1.5G measured as 8~10M without cache, 10/3.25
    
         SYSTIME->CVWRn[id] = 1; //command to do snapshot
         //wait 3 funtional clock cycles, then read
         //while(delay--); //a sub, a cmp and a jump instructions cost 2 cycles for CA-9.
         /* need to multi a number large than 10 here,
            otherwise, in O2 version, the fps which caculated
            in VPU stress test will abnormal */
         cpu_cycle_delay(delay*15);
         read1 = SYSTIME->CVWRn[id];
    #endif
        return read1; 
}

#ifdef CONFIG_GENERIC_TIME
// generic timer use 26Mhz clk
static void timer_tick_delay(int tick){   
    unsigned long long begin, end, interval;

    begin = generic_timer_read();
    do{
        end = generic_timer_read();
        interval = end - begin;
    }while( interval < (unsigned long long)tick );
}

void udelay(int us){
    timer_tick_delay(us * 26);
}

void mdelay(int ms){
    udelay(ms * 1000);
}

uint32_t timeout_mdelay(uint32_t ms, uint32_t *ind, uint32_t expected_value){
    uint64_t begin, end, interval;

    begin =(uint64_t)generic_timer_read();
    do{
        end =(uint64_t)generic_timer_read();
        interval = end - begin;
    }while( (interval < (uint64_t)(26000*ms)) && (*ind != expected_value) );

    return (uint32_t)(interval/26000);
}

#else
static void timer_tick_delay(int tick){
    uint32_t begin, end, interval;
    
    begin = timer_get_count(0);

    do{
        end = timer_get_count(0);
        interval = TIMER_GET_INTERVAL(end, begin);
    }while( interval < (uint32_t)tick );
}

/*  Every 4 uS delay will be 13 ticks using a 3.25 MHz clock 
    1 tick = 0.3077 us;
    There is a limition in the below function, the returned cycle is
    only 32bits, so the input us should within a certain range.
*/
void udelay(int us){
#if 0
    // for 13Mhz
    timer_tick_delay(13*us);
#endif    
#if 0  
    // for 6.5Mhz
    int cnt2us, cnt, tick;
    cnt2us = ( us & (~0x1) ) >> 1;
    cnt = us & 0x1;
    tick = cnt2us * 13 + (cnt ? 6:0);
    timer_tick_delay(tick);
#endif    
#if 1  //FIXME:the tdelay does not work yet. need check
    // for 3.25Mhz
    uint32_t cnt4us, cnt, tick;
    cnt4us = ( us & (~0x3) ) >> 2;
    cnt = us & 0x3;
    tick = cnt4us * 13 + cnt * 3;
    timer_tick_delay(tick);
    //__tdelay(tick);
#endif
}

void mdelay(int ms){
    timer_tick_delay(ms * 3250);
    //udelay(ms * 1000);
}


uint32_t timeout_mdelay(uint32_t ms, uint32_t *ind, uint32_t expected_value){
    uint32_t begin, end, interval;
    
    begin = timer_get_count(0);

    do{
        end = timer_get_count(0);
        interval = TIMER_GET_INTERVAL(end, begin);
        //if(end<begin)bbu_printf("end=%d, begin=%d\r\n", end, begin);
    }while( (interval < (uint32_t)(3250*ms)) && (*ind != expected_value) );
    //if(end<begin)bbu_printf("end=%d, begin=%d\r\n", end, begin);
    return (interval/3250);
}

#endif

void delay(int s){
    mdelay(s * 1000);
}
/* End Delay Functions */
/////////////////////////////////////////////////////////////////////////
#ifdef CONFIG_GENERIC_TIME 
void generic_timer_config(void){
    PMUA->TRACE_CONFIG |= TRACE_CONFIG_DBGCLK_EN;
    // enable hw mode
    BU_REG_WRITE( (0xD4015000 + 0x64), BU_REG_READ((0xD4015000 + 0x64)) | 0x1 );
    // enable the generic counter
    // on emei, this will cause XDB can't connect to the board
    // emei doesn't have generic timer
    BU_REG_WRITE( (0xD4101000 + 0x00), BU_REG_READ((0xD4101000 + 0x00)) | 0x1 );
    
    return;
}

uint64_t generic_timer_read(void){
    uint32_t low, up;
    uint64_t time;
    low = BU_REG_READ((0xD4101000 + 0x08));
    up  = BU_REG_READ((0xD4101000 + 0x0C));
    time = ((uint64_t)up << 32) | low;
    
    return time;
}

void generic_timer_test(){
    uint64_t begin, end, interval;

    bbu_printf("generic test begin.\r\nWill last 10 seconds\r\n");
    begin = generic_timer_read();
    do{
        end = generic_timer_read();
        interval = end - begin;
    }while( interval < (uint64_t)260000000 );

    bbu_printf("generic test end.\r\n");

    return;
}
#endif

int count_2_msec(uint32_t count){
   return (count/TIMER0_TICK_PER_MSEC);
}

int interval_2_msec(uint32_t begin, uint32_t end){
   return (TIMER_GET_INTERVAL(end, begin)/TIMER0_TICK_PER_MSEC);
}

int count_2_sec(uint32_t count){
   return ((count/TIMER0_TICK_PER_MSEC)/1000);
}

int interval_2_sec(uint32_t begin, uint32_t end){
   return ((TIMER_GET_INTERVAL(end, begin)/TIMER0_TICK_PER_MSEC)/1000);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


