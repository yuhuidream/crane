#include "bbu.h"
#include "utils.h"
#include "timer_dec.h"
#include <string.h>
#include "cp15.h"
#include "print.h"
#include "set_mmu.h"
#include "perf_counter.h"
#include "tim.h"
#include "bbu_mfile.h"

void POST_CMD2CLI (const char *cmdstr) 
{

  //const char *cmdstr = "ppset 100 1";
  ComCliEvt *sce = Q_NEW(ComCliEvt, COM_RX_CMD_READY_SIG);
  sce->len = sizeof(cmdstr);
  sce->source = 0;  /* differ with command join in auto screen test */
  strcpy((char*)sce->CliBuffer, cmdstr);
  QACTIVE_POST(AO_Cli, (QEvent *)sce, 0);

}

uint32_t conv_hex( char *s )
{
    uint32_t ui;
    char    *p = s, c;
    int     dcnt;
    
    for ( ui=0, dcnt=0; *p!=0  && dcnt < 10; p++, dcnt++ )
    {
        c = *p;
        if ( c >='0' && c <= '9' )
            ui = (ui<<4) +(c - '0');
        else 
        {
            if ( c >= 'a' && c <= 'f' )
                c = c -'a'+'A';
            if ( c >= 'A' && c <= 'F' )
                ui = (ui<<4) + ( c-'A'+10);
        }
    }
    
    return(ui);
}

uint32_t conv_dec( char *s )
{
    uint32_t    ui;
    char    *p = s, c;
    int     dcnt;
    
    for ( ui=0, dcnt=0; *p!=0  && dcnt < 8; p++, dcnt++ )
    {
        c = *p;
        if ( c >='0' && c <= '9' )
            ui = (ui*10) +(c - '0');
    }
    
    return(ui);
}


/* String utilities. */
uint32_t XpIsHex(char * pStr)
{
    if(pStr[0] == '0' && (pStr[1] == 'X' || pStr[1] == 'x'))
        return 1;
    return 0;
}

uint32_t XpStrToHex(char * pStr)
{
    int len = strlen(pStr), i;
    uint32_t val = 0;
    /* prefix by 0x/0X */
    for(i = 2; i < len; i++)
    {
        if(pStr[i] >= '0' && pStr[i] <= '9')
            val = (val << 4) + (pStr[i] - '0');
        else if(pStr[i] >= 'a' && pStr[i] <= 'f' )
            val = (val << 4) + (pStr[i] - 'a' + 10);
        else if(pStr[i] >= 'A' && pStr[i] <= 'F' )
            val = (val << 4) + (pStr[i] - 'A' + 10);
        else
        {
            DEBUGMSG(DBG_LEVEL,("Xp error: invalided hex value. \r\n"));
            break;          
        }          
    }
    return val;
}

uint32_t XpStrToDecimal(char * pStr)
{
    int len = strlen(pStr), i;
    uint32_t val = 0;

    for(i = 0; i < len; i++)
    {
        if(pStr[i] >= '0' && pStr[i] <= '9')
            val = (val * 10) + (pStr[i] - '0');
        else
        {
            DEBUGMSG(DBG_LEVEL,("Xp error: invalided decimal value. \r\n"));
            break;
        }
    }
    return val;
}

uint32_t XpStrToValue(char * pStr)
{
    if(XpIsHex(pStr))
    {
        return XpStrToHex(pStr);
    }
    else
    {
        return XpStrToDecimal(pStr);
    }
}

/**
 * print_log = 1: print the MCPS log, 0: don't print the MCPS log
 **/
int measure_core_frequency(int print_log){
   uint32_t cycle, freq;
   if(!is_cycle_counter_enabled()) /* if cycle counter not enabled */
      StartCycleCounter();  /* enable, clear divider */
   counter_disable(CYCLE_COUNTER_ENABLE_BASE);
   cycle = ReadCycleCounter();
   counter_enable(CYCLE_COUNTER_ENABLE_BASE);
   mdelay(1);
   counter_disable(CYCLE_COUNTER_ENABLE_BASE);
   cycle = ReadCycleCounter() - cycle;
   bbu_printf("cycle %d\n\r", cycle);
   if(is_cycle_counter_divider_set())
      cycle *= 64;
   freq = cycle/1000;
   return freq;
}

//int measure_ddr_frequency(void)
//{
//  int freq;
//  uint32_t count;
//  ddr_counter_disable_all();
//
//  ddr_counter_clear_overflow_all();
//
//  ddr_counter_set_clk_div(0);
//  ddr_counter_preload(DDR_COUNTER2, 0);
//  ddr_counter_evt_select(DDR_COUNTER2, CLOCK);
//  ddr_counter_enable_all();
//  mdelay(10);
//  count = ddr_counter_read(DDR_COUNTER2);
//  freq = count/(10*1000);
//  return freq;
//}

int bbu_mcps(void)
{
  int freq;
  freq = measure_core_frequency(0);
  bbu_printf("\n\r");
  bbu_printf("mcps--> CPU Frequecy of current OP is"
                 " : %d MHz\n\r",freq);
//  freq = measure_ddr_frequency();
//  bbu_printf("mcps--> DDR Frequecy of current OP is"
//                 " : %d MHz\n\r",freq);
}

#if 1
void qf_reschedule(void)
{
   int core_id = smp_processor_id();
   QF_RESTORE_CONTEXT(core_id);
}
#endif

uint32_t check_reg(uint8_t *reg_argu){
   if((strcmp((const char*)reg_argu, "/r") == 0) || 
      (strcmp((const char*)reg_argu, "--regdump") == 0))
   {
       return 1;
   }
   return 0;
};

#define SLTS 0x534C5453
#define ATES 0x41544553
uint8_t init_ate_slt_scripts(void)
{
	TIM tim;
    int imageNum, imageSize, i;
    pIMAGE_INFO_3_4_0 pImage;
    SetTIMPointers((uint8_t *)0xd1000000, &tim);
    imageNum = get_image_number(&tim);
    void *file_buffer;
    int slt = 0, ate = 0;
    for(i = 0; i < imageNum; i++){
       pImage = ReturnPImgPtr(&tim, i);
       if(pImage->ImageID == SLTS){   //SLT script load address
          imageSize = pImage->ImageSize;
          if(imageSize == 0){
             continue;
          }
          file_buffer = bbu_malloc(imageSize);
          if(file_buffer == 0)
          {
              bbu_printf("Error: can't init SLT script in DDR\n\r");
              return 0;
          }
          memcpy(file_buffer, (void *)pImage->LoadAddr, imageSize);
          mfile_init("slt_cmd.txt", (char *)file_buffer, imageSize);
          slt = 1;
       }
       if(pImage->ImageID == ATES){   //ATE script load address
          imageSize = pImage->ImageSize;
          if(imageSize == 0){
             continue;
          }
          file_buffer = bbu_malloc(imageSize);
          if(file_buffer == 0)
          {
              bbu_printf("Error: can't init ATE script in DDR\n\r");
              return 0;
          }
          memcpy(file_buffer, (void *)pImage->LoadAddr, imageSize);
          mfile_init("ate_cmd.txt", (char *)file_buffer, imageSize);
          ate = 1;
       }
    }
    return (slt | (ate << 1));
}

#ifdef CONFIG_CR5
void _sbrk(void)
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

void _write(void)
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

void _close(void)
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

void _lseek(void)
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

void _read(void)
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

void _kill(void)
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

void _getpid(void)
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

void _fstat(void)
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

void _isatty(void)
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

void __errno(void)
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

void _exit()
{
    bbu_printf("Should never enter this function %s\n\r", __FUNCTION__);
    while(1);
}

#endif
