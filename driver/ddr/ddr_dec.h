#ifndef H_DDR_DEC_H
#define H_DDR_DEC_H

typedef struct DdrCycleInfoTag{
   uint32_t total;       /* Clock cycles (divided by pc_clk_div) */
   uint32_t busy;        /* busy cycles  */
   uint32_t bus_idle;    /*  no bus utilization cycles when not idle */
   uint32_t rw_cmd;      /* Read + Write command count */
   uint8_t  total_div;   /* total div =  pc_clk_div*total_div2 */
   uint8_t  total_div2;  /* soft div */
   uint8_t  busy_div;
   uint8_t  bus_idle_div;
   uint8_t  rw_cmd_div;
   uint8_t  burst_len;
}DdrCycleInfo;

extern DdrCycleInfo  ddr_cycles_info;

typedef enum{
   DDR_PRIO_LOW = 0,
   DDR_PRIO_LOW1,
   DDR_PRIO_HIGH,
   DDR_PRIO_CRITICAL
}DDR_PRIO;

typedef enum{
   DDR_PORT0 = 0,
   DDR_PORT1,
   DDR_PORT2,
   DDR_PORT3,
   DDR_PORT4,
   DDR_PORT5,
   DDR_PORT6,
   DDR_PORT7,
   DDR_PORT_INVALID
}DDR_PORT;

typedef enum{
   DDR_COUNTER0 = 0,
   DDR_COUNTER1,
   DDR_COUNTER2,
   DDR_COUNTER3,
   DDR_COUNTER4,
   DDR_COUNTER5,
   DDR_COUNTER6,
   DDR_COUNTER7,
   DDR_COUNTER_INVALID
}DDR_COUNTER;

typedef enum{
   CLOCK = 0,            //0x0
   PORT0_RD_REQ,         //0x1
   PORT1_RD_REQ,         //0x2
   PORT2_RD_REQ,         //0x3
   PORT3_RD_REQ,         //0x4
   PORT4_RD_REQ,         //0x5
   PORT5_RD_REQ,         //0x6
   PORT6_RD_REQ,         //0x7
   PORT7_RD_REQ,         //0x8
   WR_REQ,               //0x9
   CRITICAL_AXI_RD_REQ,  //0xA
   HIGH_AXI_RD_REQ,      //0xB
   PORT0_RD_DFC,         //0xC  DFC: data flow-control
   PORT1_RD_DFC,         //0xD  DFC: data flow-control
   PORT2_RD_DFC,         //0xE  DFC: data flow-control
   PORT3_RD_DFC,         //0xF  DFC: data flow-control
   PORT4_RD_DFC,         //0x10  DFC: data flow-control
   PORT5_RD_DFC,         //0x11  DFC: data flow-control
   PORT6_RD_DFC,         //0x12  DFC: data flow-control
   PORT7_RD_DFC,         //0x13  DFC: data flow-control
   CH0_BUSY_CYCLE = 0x18,        //0x18
   CH1_BUSY_CYCLE,               //0x19
   CH0_RW_CMD_CNT,               //0x1A
   CH1_RW_CMD_CNT,               //0x1B
   CH0_RW_SWITCH_CNT,            //0x1C
   CH1_RW_SWITCH_CNT,            //0x1D
   CH0_ACT_CMD_CNT,              //0x1E
   CH1_ACT_CMD_CNT,              //0x1F
   CH0_TOTAL_PRECHAR_CMD_CNT,    //0x20
   CH1_TOTAL_PRECHAR_CMD_CNT,    //0x21
   CH0_PRECHAR_CMD_CNT4DREQ,     //0x22
   CH1_PRECHAR_CMD_CNT4DREQ,     //0x23
   CH0_PRECHAR_ALL,              //0x24
   CH1_PRECHAR_ALL,              //0x25
   CH0_AUTO_REFRESH_BQ_NO_IDLE,  //0x26
   CH1_AUTO_REFRESH_BQ_NO_IDLE,  //0x27
   CH0_NO_BUS_NO_IDLE,           //0x28
   CH1_NO_BUS_NO_IDLE,           //0x29
   CH0_AUTO_REFRESH,             //0x2A
   CH1_AUTO_REFRESH,             //0x2B
   CH0_AUTO_REFRESH_BQ_NO_IDLE2, //0x2C
   CH1_AUTO_REFRESH_BQ_NO_IDLE2, //0x2D
   ONE_CH_BUSY_ONE_IDLE,         //0x2E
   HAZARD = 0x30,                //0x30
   WCB_DRAIN,                    //0x31
   WCB_FULL,                     //0x32
   DDR_EVENT_INVALID
}DDR_EVENT;

int ddr_set_port_prio(DDR_PORT port_num, DDR_PRIO prio);
int ddr_get_port_prio(DDR_PORT port_num);
int ddr_set_port_prio_all(uint8_t prio[8]);
void ddr_disable_priority(void);
int ddr_set_port_starv_timer(DDR_PORT port_num, uint8_t timer_count);
void ddr_set_port_starv_timer_all(uint8_t timer_count[8]);
void ddr_set_bq_starv_timer(uint16_t timer_count);
int ddr_set_port_weight(DDR_PORT port_num, uint8_t weight);
int ddr_get_port_weight(DDR_PORT port_num);
int ddr_set_port_weight_all(uint8_t weight[8]);
void ddr_disable_weight_round_robin(void);
void ddr_enable_flat_round_robin(void);
void ddr_counter_clear_overflow_all(void);

int ddr_counter_enable(DDR_COUNTER ddr_counter);
int ddr_counter_disable(DDR_COUNTER ddr_counter);
void ddr_counter_enable_all(void);
void ddr_counter_disable_all(void);
int ddr_counter_evt_select(DDR_COUNTER ddr_counter, DDR_EVENT event);
int ddr_counter_preload(DDR_COUNTER ddr_counter, int value);
int ddr_counter_read(DDR_COUNTER ddr_counter);
int ddr_counter_get_overflow(DDR_COUNTER ddr_counter);
int ddr_counter_clear_overflow(DDR_COUNTER ddr_counter);
void ddr_counter_set_start(int start);
void ddr_counter_set_stop(int stop);
void ddr_counter_set_clk_div(int clk_div);
int ddr_counter_get_clk_div(void);
uint8_t ddr_get_burst_length(void);


void ddr_utilization_measure_start(int clock_div);
void ddr_utilization_measure_update(void);
void ddr_utilization_measure_stop(void);

#endif
