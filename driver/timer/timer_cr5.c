#include "predefines.h"
#include "PMR.h"
#include "timer_dec.h"
#include "APBCLK.h"
#include "PMUA.h"
#include "PMUM.h"
#include "ppset.h"
#include "qassert.h"

Q_DEFINE_THIS_MODULE("timer_cr5.c")

/**
 * In nezha3 core cr5, we use timercp unit0 as system timer.
 * We choose the TIMER_GROUP_WAKEUP timer as wake up source in a timer table.
 */
//global timer array.
TIME_BASE *timer[TIMER_RES_CNT] = {TIME1, TIME2};
uint32_t timer_mmp_irq[TIMER_UNIT_NUM] = {IRQ_MMP_AP1_TIMER1, IRQ_MMP_AP1_TIMER2, IRQ_MMP_AP1_TIMER2,
							 IRQ_MMP_AP2_TIMER1, IRQ_MMP_AP2_TIMER2, IRQ_MMP_AP2_TIMER2};
static IrqArg irq_arg_g[TIMER_UNIT_NUM]; 
static int get_timer_irq(uint32_t index, uint32_t unit){
	return (int)timer_mmp_irq[index*UNIT_PER_TIMER+unit];
}

static int timer_irq_inited[TIMER_UNIT_NUM] = {0};
//#define SYSTEM_TIME_IDX				 1
//#define SYSTEM_TIME_UNIT				0
//static uint32_t is_system_timer(uint32_t index, uint32_t unit){
//	return ((index==SYSTEM_TIME_IDX)&&(unit==SYSTEM_TIME_UNIT));
//}
static uint32_t wakeup_timer_remain = 2;

static TimerAttr timer_table[] = {
	{0, 0, TIMER_GROUP_SYSTEM, TIMER_FREE, 0},
	{0, 1, TIMER_GROUP_AP, TIMER_FREE, 0},
	{0, 2, TIMER_GROUP_AP, TIMER_FREE, 0},
	{1, 0, TIMER_GROUP_AP, TIMER_FREE, 0},
	{1, 1, TIMER_GROUP_WAKEUP_CP, TIMER_FREE, 0},
	{1, 2, TIMER_GROUP_WAKEUP_CP, TIMER_FREE, 0}
};

static int timer_table_idx(uint32_t index, uint32_t unit){
	int i;
	uint32_t unit_num = sizeof(timer_table)/sizeof(timer_table[0]);

	for(i = 0; i < unit_num; i ++){
		if( (timer_table[i].idx == index) && 
			( timer_table[i].unit == unit ))
			return i;
	}

	return -1;
}

static void timer_clock_select(uint32_t index, uint32_t unit, uint32_t clk)
{
    uint32_t cs_shift, val;

    if(unit == 2){
        cs_shift = 5;
    }else{
        cs_shift = (unit << 1);
    }

    val = timer[index]->CCR;
    val &= ~(0x3 << cs_shift);
    if(clk == TIMER_CLK_32K) //use system 32K
        val |= (1 << cs_shift);
    timer[index]->CCR = val;

    //Select APB clock if it's not 32K
    val = (clk << 4) | 0x3;
    switch(index){
        case 0:
            APBClock->TIMERS_CLK_RST = 0x7; //reset APB and functional domain
            APBClock->TIMERS_CLK_RST = val;
            break;
        case 1:
            APBClock->TIMERS1_CLK_RST = 0x7;
            APBClock->TIMERS1_CLK_RST = val;
            break;
    }

    return;
}

void apb_timer_reset(void){

    //Timer 1
    APBClock->TIMERS_CLK_RST = 0x7;
    cpu_cycle_delay(100);
    APBClock->TIMERS_CLK_RST = (TIMER_CLK_3P25M <<4) | 0x3;


    //Timer 2
	APBClock->TIMERS1_CLK_RST = 0x7;
    cpu_cycle_delay(100);
    APBClock->TIMERS1_CLK_RST = (TIMER_CLK_3P25M <<4) | 0x3;
	return;
}

void apb_systimer_init(void){
	uint32_t i;

	SYSTIME->CER &= ~CER_T0EN_MASK;   //disable timer0 before configure it
	i = 0x200;
	while(i--);

	/* timer0 3.25M */
	SYSTIME->CCR &= ~CCR_CS_0_MASK;   //TIME1 timer0 select configurable clock.

	i = 0x200;
	while(i--);

	SYSTIME->CMR |= CMR_T0MODE_MASK;  //TIME1 timer0 as free running timer mode
	
	/* Set up the preload value register */
	SYSTIME->PLVRn[0] = 0;	
	/* Set up the preload control regiter, disable the preload function let it free running */
	SYSTIME->PLCRn[0] = 0;

	SYSTIME->ICRn[0] |= ICRN_TCLR0_MASK;	  //clear timer0 irq

	/* disable all the match interrupts at init stage, open T10 in BSP.c for systick */
	SYSTIME->IERn[0] = 0x0;

	i = 0x200;
	while(i--);

	/* enable the timer to kick off counter */
	SYSTIME->CER = 0x1;   //enable timer0 after configure

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
	generic_timer_config(); //this is the global generic timers, for delay  system timestamp,etc.
	rtc_clk_init();//this is for RTC and alarm.
}

static uint32_t abs_diff(uint32_t value1, uint32_t value2) {
   if (value1<value2){
	  return (value2 - value1);
   }else{
	  return (value1 - value2);
   }
}

static uint32_t clk_to_khz(TIMER_CLK clk)
{
    switch(clk){
        case TIMER_CLK_13M:
            return 13000;
        case TIMER_CLK_6P5M:
            return 6500;
        case TIMER_CLK_1M:
            return 1000;
        case TIMER_CLK_32K:
            return 32;
        case TIMER_CLK_3P25M:
        default:
            return 3250; //3.25 is default
    }
}

static uint32_t ms_to_count(int ms, TIMER_CLK clk)
{
	return (ms*clk_to_khz(clk));
}

static uint32_t count_to_ms(uint32_t count, TIMER_CLK clk)
{
    return (count/clk_to_khz(clk));
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
		bbu_printf("End: 0x%08X Begin: 0x%08X\r\n", end, begin);
		Q_ASSERT(end < begin); //it's a true oveflow
		return (TIMER0_MAX_CNT - begin + end);
	}
}

//This funciton is related to OS tick, Care to modify!
uint32_t timer_get_count(int id)
{ 
    return SYSTIME->CRn[id]; 
}

#ifdef CONFIG_GENERIC_TIME
// generic timer use 26Mhz clk
static void timer_tick_delay(int tick){   
	unsigned long long begin, end, interval;

	begin = generic_timer_read();
	do{
		end = generic_timer_read();
		interval = end - begin;
		if (interval <= 0)
		break;
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
		if (interval <= 0)
		break;
	}while( interval < (uint32_t)tick );
}

/*  Every 4 uS delay will be 13 ticks using a 3.25 MHz clock 
	1 tick = 0.3077 us;
	There is a limition in the below function, the returned cycle is
	only 32bits, so the input us should within a certain range.
*/
void udelay(int us){
#ifdef CONFIG_CP_TIME_TICK
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
#ifndef CONFIG_CP_TIME_TICK  //FIXME:the tdelay does not work yet. need check
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
	timer_tick_delay(ms * TIMER0_TICK_PER_MSEC);
}


uint32_t timeout_mdelay(uint32_t ms, uint32_t *ind, uint32_t expected_value){
	uint32_t begin, end, interval;
	uint32_t count;
    count = ms_to_count(ms, TIMER_CLK_3P25M);

	begin = timer_get_count(0);

	do{
		end = timer_get_count(0);
		interval = TIMER_GET_INTERVAL(end, begin);
	}while( (interval < count) && (*ind != expected_value) );
	return count_to_ms(interval, TIMER_CLK_3P25M);
}

#endif

void delay(int s){
	mdelay(s * 1000);
}
/* End Delay Functions */
/////////////////////////////////////////////////////////////////////////
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

#ifdef CONFIG_GENERIC_TIME 
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

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

static void timer_preload_comparator(TIME_BASE* timer, uint32_t unit, uint32_t comparator){
	uint32_t val;
	val = timer->PLCRn[unit];
	val &= ~PLCRN_MCS_MASK;
	val |= comparator;
	timer->PLCRn[unit] = val;

	return;
}

static void timer_count_enable(TIME_BASE* timer, uint32_t unit){
	uint32_t val;
	val = timer->CER;
	val |= (1 << unit);
	timer->CER = val;

	return;
}

static void timer_count_disable(uint32_t index, uint32_t unit){
	uint32_t val;
	int d = timer_table_idx(index, unit);
	if(d==-1){
		bbu_printf("Error timer index or unit\r\n");
		return;
	}
	if(timer_table[d].group==TIMER_GROUP_WAKEUP)
		wakeup_timer_remain++;
	if(timer_table[d].group==TIMER_GROUP_WAKEUP_CP)
		wakeup_timer_remain++;
	val = timer[index]->CER;
	val &= ~(1 << unit);
	timer[index]->CER = val;
	timer_table[d].stat = TIMER_FREE;

	return;
}

static void timer_count_mode_set(TIME_BASE* timer, uint32_t unit, uint32_t mode){
	uint32_t val;
	val = timer->CMR;
	val &= ~(1 << unit);
	val |= (mode << unit);
	timer->CMR = val;

	return;
}

static void ap_timer_irq(void *irq_arg){
	int index, unit, run_mode;
	irq_func irq_handler;
	//TimerIrqEvt *timer_irq_func_evt = Q_NEW(TimerIrqEvt, TIMER_HANDLE_SIG); 

	index = ((IrqArg*)irq_arg)->timer_index;
	unit = ((IrqArg*)irq_arg)->timer_unit;
	run_mode = ((IrqArg*)irq_arg)->mode;
	irq_handler = ((IrqArg*)irq_arg)->func;

	timer[index]->ICRn[unit] |= 1; // clear int
	if(run_mode == TIMER_ONE_SHOT)
		timer_count_disable(index, unit);

	if(irq_handler){
		irq_handler(((IrqArg*)irq_arg)->func_arg_p);
	}

	return;
}

static int _start_timer(int ms, irq_func irq_handler, uint32_t flag, void* irq_arg_, 
						int index, int unit, TIMER_CLK clk){
	int ret, d;
    uint32_t count, irq;
	IrqArg irq_arg;
	
	if(ms <= 0)
	   return -1;

    d = timer_table_idx(index, unit);
    if( d == -1){
        bbu_printf("Wrong timer, index %d, unit\r\n", index, unit);
        return -1;
    }

    count = ms_to_count(ms, clk);

    if((timer[index]->CER & (1<<unit))){
        bbu_printf("Timer %d unit %d is busy\r\n", index, unit);
        return -1;
    }
		
    if(timer_table[d].stat == TIMER_BUSY){
        bbu_printf("Timer %d unit %d is SW busy\r\n", index, unit);
        return -1;
    }

    if(irq_handler != NULL){
        ret = index << 2 | unit;
        irq_arg.timer_index = index;
        irq_arg.timer_unit = unit;
        irq_arg.func = irq_handler;
        irq_arg.mode = flag;
        irq_arg.func_arg_p = (void *)irq_arg_;

        irq_arg_g[3*index+unit] = irq_arg;

        irq = get_timer_irq(index, unit);
        if(!timer_table[d].irq_connected){
            ISR_Connect(irq, ap_timer_irq, (void *)&irq_arg_g[3*index+unit]);
            timer_table[d].irq_connected = 1;
        }
        INT_Enable(irq, 0, 15);
    }

    if(!!ms){
        timer_count_disable(index, unit);
        // We use TnM0 as match unit.
        timer[index]->IERn[unit] &= ~(1 << 0);
        // 3.25MHz
        timer[index]->Tn_Mm[unit][0] = count;

        timer[index]->PLVRn[unit] = 0;

        timer_clock_select(index, unit, clk);

        //Enable preload with match comparator 0.
        timer_preload_comparator(timer[index], unit, 1);

        timer_count_mode_set(timer[index], unit, flag);

        timer[index]->IERn[unit] |= (1 << 0);
        timer_count_enable(timer[index], unit);
    }
		
    timer_table[d].stat = TIMER_BUSY;

    return ret;
}

static void _stop_timer(uint32_t index, uint32_t unit){
    uint32_t irq;

	timer_count_disable(index, unit);
	//disable irq.
	timer[index]->IERn[unit] &= ~(1 << 0);
	timer[index]->ICRn[unit] |= 1; // clear irq.
	irq = get_timer_irq(index, unit);
    INT_Disable(irq);

	return;
}

static int is_timer_active(uint32_t index, uint32_t unit){
	int ret = TIMER_STOP;
	if(timer[index]->CER & (1<<unit))
		ret = TIMER_RUNNING;
	
	return ret;
}

int start_specific_timer(int ms, irq_func irq_handler, uint32_t flag, void* irq_arg_, 
						int timer_index, int timer_unit, TIMER_CLK clk)
{
	wakeup_timer_remain--;
	return _start_timer(ms, irq_handler, flag, irq_arg_, timer_index, timer_unit, clk);
}

int start_timer(int ms, irq_func irq_handler, uint32_t flag){
	uint32_t i, index, unit = 0;
    uint32_t unit_num = sizeof(timer_table)/sizeof(timer_table[0]);
	for(i = 0; i < unit_num; i++){
		if(timer_table[i].group!=TIMER_GROUP_NORMAL)
			continue;
		index = timer_table[i].idx;
		unit = timer_table[i].unit;
		if((!(timer[index]->CER&(1<<unit)))&&(timer_table[i].stat==TIMER_FREE))
			break;
	}

	if(i >= unit_num ){
		for(i = 0; i < unit_num; i++){
			if((timer_table[i].group!=TIMER_GROUP_WAKEUP)&&(timer_table[i].group==TIMER_GROUP_WAKEUP_CP))
				continue;
			index = timer_table[i].idx;
			unit = timer_table[i].unit;
			if((!(timer[index]->CER&(1<<unit)))&&(timer_table[i].stat!=TIMER_FREE)){
				if(wakeup_timer_remain>1){
					wakeup_timer_remain--;
					break;
				}else{
					bbu_printf("Wakeup timer can not be used as timer tick\r\n");
					bbu_printf("All timers are busy\r\n");
					return -1;
				}
			}
		}
	}

	if(i >= unit_num){
		bbu_printf("All timers are busy\r\n");
		return -1;
	}

	return _start_timer(ms, irq_handler, flag, NULL, index, unit, TIMER_CLK_13M);
}

void stop_timer(int index){
	int timer_index, unit;
	unit = index & 0x3;
	timer_index = index>>2;
	if(index < 0 || index > 7){
		bbu_printf("%s %d error index = %d\r\n", __FUNCTION__, __LINE__, index);
		return;
	}
	_stop_timer(timer_index, unit);

	return;
}

void free_timer(int index){
	stop_timer(index);

	return;
}

void restart_timer(int index){
	int timer_index, unit, irq;
	unit = index & 0x3;
	timer_index = index>>2;
	if(index < 0 || index > 7){
		bbu_printf("%s %d error index = %d\r\n", __FUNCTION__, __LINE__, index);
		return;
	}
	//enable irq.
	timer[timer_index]->IERn[unit] |= (1 << 0);
	
	irq = get_timer_irq(timer_index, unit);

    //ISR_Connect(irq, ap_timer_irq, (void *)&irq_arg_g[3*timer_index+unit]);
    INT_Enable(irq, 0, 15);
	timer_count_enable(timer[timer_index], unit);

	return;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//timer_info[index][unit][index/unit/interval]
static int timer_info[TIMER_RES_CNT][3][3];
static void timer_test_output(void* info){
	uint32_t index_ = ((uint32_t*)info)[0];
	uint32_t unit_ = ((uint32_t*)info)[1];
	uint32_t interval_ = ((uint32_t*)info)[2];
	bbu_printf("The timer %d unit %d interval is %d\r\n", index_, unit_, interval_);

	return;
}
//timer test start function.
//run mode: 1-oneshot, 0-periodic. 
void timer_test_start(uint32_t index, uint32_t unit, uint32_t run_mode, uint32_t interval){
	timer_info[index][unit][0] = index;
	timer_info[index][unit][1] = unit;
	timer_info[index][unit][2] = interval;
	_start_timer(interval, timer_test_output, run_mode, timer_info[index][unit], index, unit, TIMER_CLK_13M);
	//start_timer_clk(interval, timer_test_output, run_mode, index, unit, TIMER_CLK_32K);

	return;
}

void timer_test_stop(uint32_t index, uint32_t unit){
	int status;
	status = is_timer_active(index, unit);
	if(status == TIMER_STOP){
		bbu_printf("timer %d unit %d is not running\r\n", index, unit);
		return;
	}
	_stop_timer(index, unit);
	bbu_printf("timer %d unit %d is stopped\r\n", index, unit);

	return;
}

void time_start_all(uint32_t run_mode, uint32_t interval){
	uint32_t i, index, unit = 0;
	for(i=0; i<sizeof(timer_table)/sizeof(timer_table[0]); i++){
		if(timer_table[i].group==TIMER_GROUP_SYSTEM)
			continue;
		if(timer_table[i].group==TIMER_GROUP_AP)
			continue;
		if(timer_table[i].stat!=TIMER_FREE)
			continue;
		index = timer_table[i].idx;
		unit = timer_table[i].unit;
		timer_test_start(index, unit, run_mode, interval);
	}

	return;
}

void timer_stop_all(){
	uint32_t i, index, unit = 0;
	for(i=0; i<sizeof(timer_table)/sizeof(timer_table[0]); i++){
		if(timer_table[i].group==TIMER_GROUP_SYSTEM)
			continue;
		if(timer_table[i].group==TIMER_GROUP_AP)
			continue;
		if(timer_table[i].stat!=TIMER_BUSY)
			continue;
		index = timer_table[i].idx;
		unit = timer_table[i].unit;
		timer_test_stop(index, unit);
	}

	return;
}

void timer_test(int argc, uint8_t **argv){
	switch(argc){
		case 1:
		case 2:{
			bbu_timer_help();
			break;
		}
		case 3:{
			if(!strcmp((char*)argv[2],"all")){
				timer_stop_all();
			}else{
				bbu_timer_help();
			}
			break;
		}
		case 4:{
			if(!strcmp((char*)argv[1], "stop")){
					uint32_t index = conv_dec((char*)argv[2]);
					uint32_t unit = conv_dec((char*)argv[3]);
					timer_test_stop(index, unit);
			}else{
				bbu_timer_help();
			}
			break;
		}
		case 5:{
			if(!strcmp((char*)argv[1],"start")){
				if(!strcmp((char*)(argv[2]),"all")){
					time_start_all(conv_dec((char*)argv[3]), conv_dec((char*)argv[4]));
				}else{
					bbu_timer_help();
				}
			break;
			}
		}
		case 6:{
			if(!strcmp((char*)argv[1],"start")){
				uint32_t index = conv_dec((char*)argv[2]);
				uint32_t unit = conv_dec((char*)argv[3]);
				uint32_t run_mode = conv_dec((char*)argv[4]);
				uint32_t interval = conv_dec((char*)argv[5]);
				timer_test_start(index, unit, run_mode, interval);
			}else{
				bbu_timer_help();
			}
			break;
		}
	}
	
	return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
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
static int _start_timer_clk(int ms, irq_func irq_handler, uint32_t mode,
					 int timer_index, int timer_unit, TIMER_CLK clk){
	return _start_timer(ms, irq_handler, mode, NULL,timer_index, timer_unit, clk);
}

int start_timer_wakeup(int ms, irq_func irq_handler){
	uint32_t index, unit, i = 0;
	for(i=0; i<sizeof(timer_table)/sizeof(timer_table[0]); i++){
		if(timer_table[i].group!=TIMER_GROUP_WAKEUP_CP)
			continue;
		index = timer_table[i].idx;
		unit = timer_table[i].unit;
		if((!(timer[index]->CER&(1<<unit)))&&(timer_table[i].stat==TIMER_FREE))
			break;
	}
	if(i>=sizeof(timer_table)/sizeof(timer_table[0])){
		bbu_printf("No timer free for wake up\r\n");
		Q_ASSERT(0);
	}
	_start_timer_clk(ms, irq_handler, 1, index, unit, TIMER_CLK_32K);
	bbu_printf("timer %d unit %d is set as wake up source\r\n", index, unit);
	int ret = get_timer_irq(index, unit);
	
	return ret;
}



