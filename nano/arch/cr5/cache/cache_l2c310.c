#include "predefines.h"
#include "cache_ops.h"
#include "cp15.h"
#include "L2C.h"

#define cpu_relax()

#define CONFIG_CACHE_L2X0_PREFETCH
#define CONFIG_CACHE_L2X0_PREFETCH_OFFSET 7

uint32_t l2c310_size = 512*1024;  //512KB
static void __l2_cache_clean_invalidate_all(void);

uint32_t l2c310_way_mask = 0xFF;

static void l2_cache_unlock(void){
   PL310->DLKDN0 = 0;
   PL310->DLKDN1 = 0;
   PL310->DLKDN2 = 0;
   PL310->DLKDN3 = 0;
   PL310->DLKDN4 = 0;
   PL310->DLKDN5 = 0;
   PL310->DLKDN6 = 0;
   PL310->DLKDN7 = 0;
   PL310->ILKDN0 = 0;
   PL310->ILKDN1 = 0;
   PL310->ILKDN2 = 0;
   PL310->ILKDN3 = 0;
   PL310->ILKDN4 = 0;
   PL310->ILKDN5 = 0;
   PL310->ILKDN6 = 0;
   PL310->ILKDN7 = 0;
}

static void set_way_mask(void){
    uint32_t aux;
    int ways;
    aux = PL310->AUXCTRL;
    if(aux & BIT_16)
        ways = 16;
    else
        ways = 8;
    l2c310_way_mask = (1 << ways) - 1;
}

void nezha2_l2_cache_init(void){
   uint32_t aux;
   if(PL310->CTRL == 0){
      int prefetch_ctrl = 0;

      PL310->TRAMCTRL = 0x10;
      PL310->DRAMCTRL = 0x10;

      /* L2X0 Power Control  */
      PL310->PWRCTRL = 0x3;
       
      aux = PL310->AUXCTRL;
      aux &= 0xFE7FFFFF;
      aux |= 0x30800000;
      
      /*
       * Set bit 22 in the auxiliary control register. If this bit
       * is cleared, PL310 treats Normal Shared Non-cacheable
       * accesses as Cacheable no-allocate.
       */
      aux |= BIT_22;
       
      set_way_mask();
       
   #ifdef CONFIG_CACHE_L2X0_PREFETCH
      /* Configure double line fill and prefetch */
      prefetch_ctrl |= (1u << 30); /* DLF (double linefill) enabled*/
      prefetch_ctrl |= (1u << 29); /* instruction prefetch enabled */
      prefetch_ctrl |= (1u << 28); /* data prefetch enabled */

      if (CONFIG_CACHE_L2X0_PREFETCH_OFFSET <= 7)
         prefetch_ctrl |= CONFIG_CACHE_L2X0_PREFETCH_OFFSET;
      else if (CONFIG_CACHE_L2X0_PREFETCH_OFFSET <= 15)
         prefetch_ctrl |= 15;
      else if (CONFIG_CACHE_L2X0_PREFETCH_OFFSET <= 23)
         prefetch_ctrl |= 23;
      else
         prefetch_ctrl |= 31;
      PL310->PRFCTRL = prefetch_ctrl;
   #endif
      
      if(!(PL310->CTRL & 1)){
         l2_cache_unlock();
         PL310->AUXCTRL = aux;
         l2_cache_invalidate_all();
         PL310->CTRL = 1;
      }
   }
}

/* ........................................................................ */
void l2_cache_enable(void){
   uint32_t val;
   val = PL310->CTRL;
   if(!(val & L2CENABLE)){
      l2_cache_invalidate_all();
      val |= L2CENABLE;
      PL310->CTRL |= L2CENABLE;
   }
   else
      bbu_printf("L2 cache already enabled\n\r");
   dsb();
   return;
}

/* ........................................................................ */
void l2_cache_disable(void){
   __l2_cache_clean_invalidate_all();
   PL310->CTRL = 0;
   return;
}

/* ........................................................................ */
static void __l2_cache_clean_invalidate_all(void){
   uint32_t val = l2c310_way_mask;
   PL310->CLNINVWAY = val;
   do{
      val = (PL310->CLNINVWAY) & l2c310_way_mask;
   }while(val != 0);
   cpu_relax();
   l2_cache_sync();
}

/* ........................................................................ */
void l2_cache_clean_invalidate_all(void){
   __l2_cache_clean_invalidate_all();
}

/* ........................................................................ */
void l2_cache_clean_all(void){
   uint32_t val = l2c310_way_mask;
   PL310->CLNWAY = val;
   do{
       val = (PL310->CLNWAY) & l2c310_way_mask;
   }while(val != 0);
   cpu_relax();
   l2_cache_sync();
}

/* ........................................................................ */
void l2_cache_invalidate_all(void){
   uint32_t val = l2c310_way_mask;
   PL310->INVWAY = val;
   do{
       val = (PL310->INVWAY) & l2c310_way_mask;
   }while(val != 0);
   cpu_relax();
   l2_cache_sync();
}

/* ........................................................................ */
void l2_cache_clean_line(uint32_t pa){
   while(PL310->CLNPA & 1);
   cpu_relax();
   PL310->CLNPA = pa;
   return;
}

/* ........................................................................ */
void l2_cache_invalidate_line(uint32_t pa){
   while(PL310->INVPA & 1);
   cpu_relax();
   PL310->INVPA = pa;
   return;
}

/* ........................................................................ */
void l2_cache_clean_invalidate_line(uint32_t pa){
   while(PL310->CLNINVPA & 1);
   cpu_relax();
   PL310->CLNINVPA = pa;
   return;
}

/* ........................................................................ */
void l2_cache_clean_range(uint32_t start_pa, uint32_t byteNum){
   uint32_t end_pa;
   if(byteNum > l2c310_size){
      l2_cache_clean_all();
      return;
   }
   end_pa = start_pa + byteNum;
   start_pa &= ~(L2C_CACHE_LINE_SIZE - 1);
   while(start_pa < end_pa){
      uint32_t blk_end_pa = start_pa + min((end_pa-start_pa), 4096UL);
      while(start_pa < blk_end_pa){
         l2_cache_clean_line(start_pa);
         start_pa += L2C_CACHE_LINE_SIZE;
      }
   }
   while(PL310->CLNPA & 1);//cache wait
   cpu_relax();
   l2_cache_sync();
}

/* ........................................................................ */
void l2_cache_invalidate_range(uint32_t start_pa, uint32_t byteNum){
   uint32_t end_pa;
   end_pa = start_pa + byteNum;
   if(start_pa & (L2C_CACHE_LINE_SIZE-1)){
      start_pa &= ~(L2C_CACHE_LINE_SIZE - 1);
      l2_cache_clean_invalidate_line(start_pa);
      start_pa += L2C_CACHE_LINE_SIZE;
   }
   if(end_pa & (L2C_CACHE_LINE_SIZE-1)){
      end_pa &= ~(L2C_CACHE_LINE_SIZE - 1);
      l2_cache_clean_invalidate_line(end_pa);
   }
   while(start_pa < end_pa){
      uint32_t blk_end_pa = start_pa + min((end_pa-start_pa), 4096UL);
      while(start_pa < blk_end_pa){
         l2_cache_invalidate_line(start_pa);
         start_pa += L2C_CACHE_LINE_SIZE;
      }
   }
   while(PL310->INVPA & 1);//cache wait
   cpu_relax();
   l2_cache_sync();
}

/* ........................................................................ */
void l2_cache_clean_invalidate_range(uint32_t start_pa, uint32_t byteNum){
   uint32_t end_pa;
   if(byteNum > l2c310_size){
      l2_cache_clean_invalidate_all();
      return;
   }
   end_pa = start_pa + byteNum;
   start_pa &= ~(L2C_CACHE_LINE_SIZE - 1);
   while(start_pa < end_pa){
      uint32_t blk_end_pa = start_pa + min((end_pa-start_pa), 4096UL);
      while(start_pa < blk_end_pa){
         l2_cache_clean_invalidate_line(start_pa);
         start_pa += L2C_CACHE_LINE_SIZE;
      }
   }
   while(PL310->CLNINVPA & 1);//cache wait
   cpu_relax();
   l2_cache_sync();
}

/* ........................................................................ */
void l2_cache_sync(void){
   uint32_t val = 0;
   PL310->CSYNC = val;
   do{
    val = (PL310->CSYNC) & 0x1;
   }while(val != 0);
   cpu_relax();
   return;
}

/* ........................................................................ */
void l2_cache_standby_enable(void){
   uint32_t val;
   dsb();
   val = PL310->PWRCTRL;
   val |= L2C_STANDBY_EN;
   PL310->PWRCTRL = val;
   dsb();
   return;
}

/* ........................................................................ */
void l2_cache_standby_disable(void){
   uint32_t val;
   dsb();
   val = PL310->PWRCTRL;
   val &= ~L2C_STANDBY_EN;
   PL310->PWRCTRL = val;
   dsb();
   return;
}

/* ........................................................................ */
void l2_cache_prefetch_enable(void){
   uint32_t val;
   dsb();
   val = PL310->PRFCTRL;
   val |= L2C_I_D_PRF_EN;
   PL310->PRFCTRL = val;
   dsb();
   return;
}

/* ........................................................................ */
void l2_cache_prefetch_disable(void){
   uint32_t val;
   dsb();
   val = PL310->PRFCTRL;
   val &= ~L2C_I_D_PRF_EN;
   PL310->PRFCTRL = val;
   dsb();
   return;
}

/* ........................................................................ */
uint32_t GetL2EnableDisable(void)
{
    volatile uint32_t pEn;
    pEn = PL310->CTRL;
    return (pEn & 0x1u);
}
//---------------------------
   /*L2 Tag and Data SRAMs latency values must be configured into the Tag and Data RAM 
    Latency Control Registers based on Table 8, Latency Values before enabling L2$ for 
    the first time. SW must not rely on the default value of these registers. These values 
    may also require asjustment when core frequency is changing, similar to Vcc_main.    
   */ 
   //SetL2TagDataLatencies(0);
   //We don't have solution on PP change sync yet. simply set 2 cycles and don't change it

/* Set SRAM Tag and Data L2$ latencies. See Nevo APPS Main IAS chapter 3.8  for more information
 * val - '0' - <= 416MHz
 *       '1' - 416~624MHz
 * TODO: The value and segments are for Nevo, may need tuning for Emei.
 */ 
void SetL2TagDataLatencies(uint32_t value)
{
    uint32_t data_latency, tag_latency;

    if (0==value)
    {
        data_latency = 0; //data latencies for low-Mips
        tag_latency = 0;
    }
    else 
    {
        data_latency = 0x00000010; //data latencies for hi-Mips and overdrive
        tag_latency = 0x00000010;
    }

    __asm__("dsb"); //ensure ordering with previous memory accesses

    if(is_l2_cache_enabled()) {
        //TODO:Need to sync with the other core to let it don't use cache during the change.
        l2_cache_disable();
        PL310->TRAMCTRL = tag_latency; //tag latencies for li-Mips/hi-Mips/Overdrive
        PL310->DRAMCTRL = data_latency;
        l2_cache_enable();
    }
    else
    {
        PL310->TRAMCTRL = tag_latency; //tag latencies for li-Mips/hi-Mips/Overdrive
        PL310->DRAMCTRL = data_latency;
    }
 
    __asm__("dsb");

   return;
}

/* ........................................................................ */
static char *associativity[] ={"8-way", "16-way"};
static char *wsize[]={"reserved(16KB)",
    "16KB",
    "32KB",
    "64KB",
    "128KB",
    "256KB",
    "512KB",
    "reserved(512KB)"
};

/* ........................................................................ */
void sl2c_dump_info(void){
    uint32_t aux;
    bbu_printf("L2 Cache Information:\n\r");
    bbu_printf("Base Addr = 0x%08X\n\r", L2C_BASE);

    aux = PL310->CID;
    bbu_printf("Implementer : 0x%X\n\r", (aux >> 24) & 0xff);
    bbu_printf("Cache ID    : 0x%X\n\r", (aux >> 10) & 0x3f);
    bbu_printf("Part Number : 0x%X\n\r", (aux >> 6) & 0xf);
    bbu_printf("RTL release : 0x%X\n\r", (aux >> 0) & 0x3f);
    aux = PL310->CTYPE;
    bbu_printf("System L2 Cache Associativity : %s\n\r", 
                associativity[(aux >> 3) & 0xf]);
    bbu_printf("System L2 Cache Way size      : %s\n\r", 
                wsize[(aux >> 8) & 0x7]);
}


/*********Cortex A9 L2 cache performance counter operation functions**********/
int l2c_evt_counter_cfg(uint8_t counter, uint32_t cfg_val){
   uint32_t val;
   dsb();
   val = PL310->EVCTCTRL;
   val &= ~L2C_EVT_COUNTER_EN;
   PL310->EVCTCTRL = val;      //disable counter first
   if(counter == 0){
      val |= L2C_EVT_COUNTER0_RST;   //counter 0 reset
      PL310->EVCTCTRL = val;
      PL310->EVCT0CFG = cfg_val;
   }
   else{
      val |= L2C_EVT_COUNTER1_RST;      //counter 1 reset
      PL310->EVCTCTRL = val;
      PL310->EVCT1CFG = cfg_val;
   }
   dsb();
   return MV_OK;
}

/* ........................................................................ */
int l2c_evt_counter_preload(uint8_t counter, uint32_t pre_num){
   uint32_t cfg_bk;
   dsb();
   if(counter == 0){
      cfg_bk = PL310->EVCT0CFG;
      PL310->EVCT0CFG &= ~L2C_EVT_CFG_SRC_MASK;   //disable counter 0
      PL310->EVCT0 = pre_num;      //set preload number to counter 0
      PL310->EVCT0CFG = cfg_bk;      //restore the configuration
   }
   else{
      cfg_bk = PL310->EVCT1CFG;
      PL310->EVCT1CFG &= ~L2C_EVT_CFG_SRC_MASK;   //disable counter 1
      PL310->EVCT1 = pre_num;      //set preload number to counter 1
      PL310->EVCT1CFG = cfg_bk;      //restore the configuration
   }
   dsb();
   return MV_OK;
}

/* ........................................................................ */
int l2c_evt_counter_read(uint8_t counter, uint32_t *count_num){
   dsb();//ensure ordering with previous memory accesses
   if(counter == 0){
      *count_num =PL310->EVCT0;
   }
   else{
      *count_num =PL310->EVCT1;
   }
   dsb();
   return MV_OK;
}

/* ........................................................................ */
int l2c_evt_counters_enable_all(void){
   dsb();   //ensure ordering with previous memory accesses
   if(!is_l2_cache_evt_monitor_enabled()){
      PL310->CTRL &= ~L2CENABLE;            //disable L2 cache
      PL310->AUXCTRL |= L2C_EVT_M_BUS_EN;   //set Event Monitor Bus Enable
      PL310->CTRL |= L2CENABLE;             //enable L2 cache
   }
   PL310->EVCTCTRL |= L2C_EVT_COUNTER_EN;    //set Event Counter Enable
   dsb();
   return MV_OK;
}

/* ........................................................................ */
int l2c_evt_counters_disable_all(void){
   dsb();       //ensure ordering with previous memory accesses
   PL310->EVCTCTRL &= ~L2C_EVT_COUNTER_EN;   //clear Event Counter disable
   dsb();
   return MV_OK;
}

/* ........................................................................ */
int l2c_evt_counter_reset(uint8_t counter){
   dsb();        //ensure ordering with previous memory accesses
   if(counter == 0){
      PL310->EVCTCTRL |= L2C_EVT_COUNTER0_RST;      //counter 0 reset
   }
   else{
      PL310->EVCTCTRL |= L2C_EVT_COUNTER1_RST;      //counter 1 reset
   }
   dsb();
   return MV_OK;
}

/* ........................................................................ */
int l2c_evt_counters_reset_all(void){
   dsb();        //ensure ordering with previous memory accesses
   PL310->EVCTCTRL |= (L2C_EVT_COUNTER0_RST |L2C_EVT_COUNTER1_RST);
   dsb();
   return MV_OK;
}

/* ........................................................................ */
int l2c_evt_counter_release(uint8_t counter){
   dsb();        //ensure ordering with previous memory accesses
   if(counter){    //clear [5-2] -disable counting
      PL310->EVCT1CFG &= ~L2C_EVT_CFG_SRC_MASK;
   }
   else{          //clear [5-2] -disable counting
      PL310->EVCT0CFG &= ~L2C_EVT_CFG_SRC_MASK;
   }
   dsb();
   return MV_OK;
   
}


/* ........................................................................ */
int is_l2_cache_enabled(void){
    return (PL310->CTRL) & L2CENABLE;
}


/* ........................................................................ */
int is_l2_cache_prefetch_enabled(void){
   return (PL310->PRFCTRL & L2C_I_D_PRF_EN);
}

/* ........................................................................ */
int is_l2_cache_evt_monitor_enabled(void){
   return (PL310->AUXCTRL & L2C_EVT_M_BUS_EN);
}
