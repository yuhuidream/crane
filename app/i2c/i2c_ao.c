/*****************************************************************************
* Product: BBU
* 
* Purpose: I2C AO
* 
* Copyright (C) 2010-2011 Marvell MTBL-Hefei. All rights reserved.
*
* Author: Yu Zhang  email: zhangy@marvell.com
* 
*****************************************************************************/
#include "i2c_comp.h"
#include "utils.h"
#include "bsp.h"

#include <stdlib.h>
#include <string.h>

Q_DEFINE_THIS_FILE

/* I2C Active Object .......................................................*/
typedef struct I2CTag {
   QActive super;             /* extend the QActive class */
   uint32_t i2c_index;
   QTimeEvt  time_out;
   I2C_COMP  *i2c_comp[MRVL_I2C_MAX];      /* active i2c components */
}I2C;

/* hierarchical state machine ............................................. */
static QState i2c_initial(I2C *me, QEvent const *e);
static QState i2c_processing(I2C *me, QEvent const *e);

/* i2c commands help info.................................................. */
static void i2c_scan_help(void);
static void wi2c_help(void);
static void ri2c_help(void);
static void ai2c_help(void);
static void i2c_dump_help(void);
static void ii2c_help(void);
static void i2cm_help(void);

static I2C l_i2c;             /* the sole instance of the I2C active object */

#define I2C_dispatch(i2c_index, e_) \
            QHsm_dispatch((QHsm *)(me->i2c_comp[i2c_index-1]),e_)
                          
/* global objects.......................................................... */
QActive * const AO_I2C = (QActive *)&l_i2c;   /*opaque pointer to I2C */

/*..........................................................................*/
void I2C_ctor(void) {
   uint8_t n;
   I2C *me = &l_i2c;
   QActive_ctor(&me->super, (QStateHandler)&i2c_initial);
   QTimeEvt_ctor(&me->time_out,     I2C_TIMEOUT_SIG);
   for (n = 0; n < MRVL_I2C_MAX; ++n) {
      /* instantiate i2c unit */
      me->i2c_comp[n] = (I2C_COMP *)i2c_comp_ctor(n);
   }
}

/*..........................................................................*/
static QState i2c_initial(I2C *me, QEvent const *e){
   uint8_t n;

   (void)e;      /* avoid the "unreferenced parameter" warning */

   for (n = 0; n < MRVL_I2C_MAX; ++n) {
      i2c_comp_init(me->i2c_comp[n]);  /*initial tran. for I2C_comp */
   }
   /* move I2C initialization to SystemInit() */
   //i2c_init_all();
   me->i2c_index = I2C_INVALID;
   return Q_TRAN(&i2c_processing);
}

/*..........................................................................*/   
static QState i2c_processing(I2C *me, QEvent const *e){
   uint32_t i2c_index;
   uint32_t n;
   ComCmdEvt *cmdEvt = (ComCmdEvt *)e;
   switch(e->sig){
      case I2C_INIT_SIG:{
         if(1 == cmdEvt->argc) {
              /* if no argument entered, initialize all I2Cs */
            bbu_printf("\n\r");
            for(n = MRVL_I2C_MAX; n >= 1; --n)
               I2C_dispatch(n, e);
         }
         else{
            if(check_help(cmdEvt->argv[1]))
               ii2c_help();
            else{
               i2c_index = atoi((const char *)cmdEvt->argv[1]);
               if(i2c_index <= MRVL_I2C_MAX && i2c_index != 0)
                  I2C_dispatch(i2c_index, e);
               else
                  bbu_printf("ii2c--> The i2c_index is out of range\n\r");
            }
         }
         screen_cmd_ack(SCREEN_TEST_PASS, cmdEvt->scr_cmd_index);
         bbu_printf("\r"BBU_PROMPT);
         return Q_HANDLED();
      }
      case I2C_READ_SIG:
      case I2C_WRITE_SIG:{
         I2C_dispatch(((I2cRwEvt *)e)->i2c_index, e);
         return Q_HANDLED();
      }
      case I2C_DUMP_SIG:{
         switch(cmdEvt->argc){
            case 1:{
               bbu_printf("i2c_dump-->  Usage: i2c_dump [/w] <start_reg> <dump_reg_num>\n\r");
               break;
            }
            case 2:{
               if(check_help(cmdEvt->argv[1])){
                  i2c_dump_help();
               }
               else
                  bbu_printf("i2c_dump--> Usage: i2c_dump [/w] <start_reg> <dump_reg_num>\n\r");
               break;
            }
            case 3:{
               if(!strcmp((const char*)cmdEvt->argv[1], "/w"))
                  bbu_printf("i2c_dump--> Usage: i2c_dump [/w] <start_reg> <dump_reg_num>\n\r");
               else{
                  if(me->i2c_index == I2C_INVALID)
                     bbu_printf("i2c_dump--> <i2c_index> is not correct\n\r");
                  else{
                     QTimeEvt_postIn(&me->time_out, (QActive *)me, BSP_TICKS_PER_SEC);
                     I2C_dispatch(me->i2c_index, e);
                  }
               }
               break;
            }
            case 4:{
               if(!strcmp((const char*)cmdEvt->argv[1], "/w")){
                  if(me->i2c_index == I2C_INVALID)
                     bbu_printf("i2c_dump--> <i2c_index> is not correct\n\r");
                  else{
                     QTimeEvt_postIn(&me->time_out, (QActive *)me, BSP_TICKS_PER_SEC);
                     I2C_dispatch(me->i2c_index, e);
                  }
               }
               else
                  bbu_printf("i2c_dump--> Usage: i2c_dump [/w] <start_reg> <dump_reg_num>\n\r");
               break;
            }
            default:
               bbu_printf("i2c_dump--> Usage: i2c_dump [/w] <start_reg> <dump_reg_num>\n\r");
               break;
         }
         bbu_printf("\r"BBU_PROMPT);
         return Q_HANDLED();
      }
      case I2C_SCAN_SIG:{
         if(1 == cmdEvt->argc) {            
            /* If no argument input ,scan the HS1_I2C as default. */
            I2C_dispatch(HS1_I2C, e);  
         }
         else if(check_help(cmdEvt->argv[1])){
            i2c_scan_help();
            bbu_printf(BBU_PROMPT);
         }
         else{
            me->i2c_index = conv_dec(( char *)cmdEvt->argv[1]);
            if(me->i2c_index <= MRVL_I2C_MAX && me->i2c_index != 0){
               QTimeEvt_postIn(&me->time_out, (QActive *)me,
                             BSP_TICKS_PER_SEC*2);
               I2C_dispatch(me->i2c_index,e);
            }
            else
               bbu_printf("i2c_scan--> The i2c_index is out of range\n\r"BBU_PROMPT);
         }
         return Q_HANDLED();
      }
      case I2C_ADDR_SIG:{
         uint32_t slave_addr;
         switch(cmdEvt->argc){
            case 1:{
               bbu_printf("ai2c--> Slave address is set to: "
                       "0x%02X\n\r", i2c_read_uart_scr());
               bbu_printf("             I2C unit which the device "
                          "connects to is set to %d\n\r", me->i2c_index);
               break;
            }
            case 2:{
               if(check_help(cmdEvt->argv[1]))
                  ai2c_help();
               else{
                  slave_addr = strtoul((const char *)cmdEvt->argv[1],0,16);
                  me->i2c_index = i2c_lookup_index(slave_addr);
                  if(me->i2c_index == I2C_INVALID)
                     bbu_printf("ai2c--> <i2c_index> cannot be found "
                                 "according to the slave address\n\r");
                  else{
                     i2c_write_uart_scr(slave_addr);
                     bbu_printf("ai2c--> I2C slave address and I2C unit index has been updated.\n\r");
                  }
               }
               break;
            }
            case 3:{
               slave_addr = strtoul((const char *)cmdEvt->argv[1],0,16);
               me->i2c_index = atoi((const char *)cmdEvt->argv[2]);
               i2c_write_uart_scr(slave_addr);
               bbu_printf("ai2c--> I2C master/slave address has been updated.\n\r");
               break;
            }
            default:
               break;
         }
         bbu_printf("\r"BBU_PROMPT);
         return Q_HANDLED();
      }
      case I2C_RD_TEST_SIG:{
         switch(cmdEvt->argc){
            case 1:{
               bbu_printf("ri2c--> Usage: ri2c [/w] <reg_addr>\n\r");
               break;
            }
            case 2:{
               if(check_help(cmdEvt->argv[1])){
                  ri2c_help();
               }
               else{
                  if(!strcmp((const char*)cmdEvt->argv[1], "/w"))
                     bbu_printf("ri2c--> Usage: ri2c [/w] <reg_addr>\n\r");
                  else{
                     if(me->i2c_index == I2C_INVALID){
                        bbu_printf("ri2c--> <i2c_index> is not correct\n\r");
                     }
                     else{
                        QTimeEvt_postIn(&me->time_out, (QActive *)me, BSP_TICKS_PER_SEC);
                        I2C_dispatch(me->i2c_index, e);
                     }
                  }
               }
               break;
            }
            case 3:{
               if(!strcmp((const char*)cmdEvt->argv[1], "/w")){
                  if(me->i2c_index == I2C_INVALID){
                     bbu_printf("ri2c--> <i2c_index> is not correct\n\r");
                  }
                  else{
                     QTimeEvt_postIn(&me->time_out, (QActive *)me, BSP_TICKS_PER_SEC);
                     I2C_dispatch(me->i2c_index, e);
                  }
               }
               else
                  bbu_printf("ri2c--> Usage: ri2c [/w] <reg_addr>\n\r");
               break;
            }
            default:
               bbu_printf("ri2c--> Usage: ri2c [/w] <reg_addr>\n\r");
               break;
         }
         bbu_printf("\r"BBU_PROMPT);
         return Q_HANDLED();
      }
      case I2C_WR_TEST_SIG:{
         switch(cmdEvt->argc){
            case 1:{
               bbu_printf("wi2c--> Usage: wi2c [/w] <reg_addr> <value>\n\r");
               break;
            }
            case 2:{
               if(check_help(cmdEvt->argv[1])){
                  wi2c_help();
               }
               else{
                  bbu_printf("wi2c--> Usage: wi2c [/w] <reg_addr> <value>\n\r");
               }
               break;
            }
            case 3:{
              if(!strcmp((const char*)cmdEvt->argv[1], "/h"))
                  bbu_printf("wi2c--> Usage: wi2c [/w] <reg_addr> <value>\n\r");
              else{
                  if(me->i2c_index == I2C_INVALID){
                     bbu_printf("wi2c--> <i2c_index> is not correct\n\r");
                  }
                  else{
                     QTimeEvt_postIn(&me->time_out, (QActive *)me, BSP_TICKS_PER_SEC);
                     I2C_dispatch(me->i2c_index, e);
                  }
               }
               break;
            }
            case 4:{
               if(!strcmp((const char*)cmdEvt->argv[1], "/w")){
                  if(me->i2c_index == I2C_INVALID){
                     bbu_printf("wi2c--> <i2c_index> is not correct\n\r");
                  }
                  else{
                     QTimeEvt_postIn(&me->time_out, (QActive *)me, BSP_TICKS_PER_SEC);
                     I2C_dispatch(me->i2c_index, e);
                  }
               }
               else
                  bbu_printf("wi2c--> Usage: wi2c [/w] <reg_addr> <value>\n\r");
               break;
            }
            default:
               bbu_printf("wi2c--> Usage: wi2c [/w] <reg_addr> <value>\n\r");
               break;
         }
         bbu_printf("\r"BBU_PROMPT);
         return Q_HANDLED();
      }
      case I2C_MODE_SIG:{
         int mode;
         switch(cmdEvt->argc){
            case 1:
               mode = i2c_get_fast_mode(CI2C1);
               bbu_printf("i2cm-->  CI2C 1 is in %s mode\n\r", mode ? "fast":"slow");
               mode = i2c_get_fast_mode(PI2C);
               bbu_printf("i2cm-->  Power I2C is in %s mode\n\r",mode?"fast":"slow");
               bbu_printf("i2cm-->  I2C log is %s\n\r", is_i2c_log_on()? "on":"off");
               break;
            case 2:
               if(!strcmp((const char*)cmdEvt->argv[1], "log")){
                  switch_i2c_log();
                  bbu_printf("i2cm-->  I2C log is turned %s\n\r", 
                           is_i2c_log_on() ? "on":"off");
               }else
                  i2cm_help();
               break;
            case 3:
               i2c_index = conv_dec((char *)cmdEvt->argv[1]);
               if(i2c_index >= I2C_INVALID){
                  bbu_printf("i2cm-->  The I2C index is out of range\n\r");
                  break;
               }
               if(!strcmp((const char*)cmdEvt->argv[2], "fast") || 
               	  !strcmp((const char*)cmdEvt->argv[2], "f")){
                  i2c_mode_change(i2c_index, I2C_FAST_MODE);
                  bbu_printf("i2cm--> I2C %d is set into fast mode\n\r", i2c_index);
               }else if(!strcmp((const char*)cmdEvt->argv[2], "slow") || 
               	        !strcmp((const char*)cmdEvt->argv[2], "s")){
                  i2c_mode_change(i2c_index, I2C_SLOW_MODE);
                  bbu_printf("i2cm--> I2C %d is set into slow mode\n\r", i2c_index);
               }else
                  bbu_printf("i2cm-->  The speed mode is not correct\n\r");
               break;
            default:
               i2cm_help();
               break;
         }
         bbu_printf("\r"BBU_PROMPT);
         return Q_HANDLED();
      }
      case I2C_SUC_SIG:{
         I2C_dispatch(((I2cRwEvt *)e)->i2c_index, e);
         return Q_HANDLED();
      }
      case I2C_IRF_SIG:
      case I2C_ITE_SIG:
      case I2C_BED_SIG:{
         I2C_dispatch(((I2cRetEvt *)e)->i2c_index, e);
         return Q_HANDLED();
      }
      case I2C_TIMEOUT_SIG:{
         bbu_printf("I2C time out %d\n\r", me->i2c_index);
         QTimeEvt_disarm(&me->time_out);
         if(me->i2c_index > 0 && me->i2c_index <= MRVL_I2C_MAX)
            I2C_dispatch(me->i2c_index, e);
         return Q_HANDLED();
      }
#ifdef CONFIG_SUSP_DBG
      case I2C_SUSP_SIG:{
          bbu_printf("AO I2C suspend, prio: %d\n\r", AO_I2C->prio);
          int ret = QA_SUSPEND_WAIT4RESUME();
          bbu_printf("AO I2C wake up, prio: %d, return: %d\n\r",
                     AO_I2C->prio, ret);
          return Q_HANDLED();
      }
#endif
   }
   return Q_SUPER(&QHsm_top);
}



/* i2c test command help................................................... */
static void ii2c_help(void)
{
   bbu_printf("\n\rii2c - Initialize I2C unit.\n\r");
   bbu_printf(" Usage: ii2c [<index>]\n\r");
   bbu_printf("  <index> -- stands for the i2c used, For 1 or 2 for Nezha2\n\r");
   bbu_printf("             1: CI2C, 2: PI2C\n\r");
   bbu_printf("             If no argument input, Initialize all the I2C units.\n\r");
   return;
}

static void i2c_scan_help(void)
{
   bbu_printf("\n\ri2c_scan - Scan platform for valid I2C addresses.\n\r");
   bbu_printf(" Usage: i2c_scan [<index>] [<start_address>]\n\r");
   bbu_printf("  <index> -- stands for the i2c used, For 1 or 2 for Nezha2\n\r");
   bbu_printf("             1: CI2C, 2: PI2C\n\r");
   bbu_printf("             If no argument input, scan the CI2C as default.\n\r");
   bbu_printf("  <start_address> -- The slave address that the scan starts from\n\r");
   return;
}

static void wi2c_help(void){
   bbu_printf("\n\rwi2c - Write date into internal register of a slave device which is \n\r");
   bbu_printf("       indicated by 'ai2c <address> [<i2c_index>]' previously\n\r");
   bbu_printf(" Usage: wi2c [/w] <reg_addr> <value>\n\r");
   bbu_printf("  <reg_addr> -- stands for the device register address\n\r");
   bbu_printf("  <value>    -- data to write into the register\n\r");
   bbu_printf("  [/w]       -- option for half-word(16-bit) register address,\n\r"
              "                default is 8-bit\n\r");
   return;
}

static void ri2c_help(void){
   bbu_printf("\n\rri2c - Read internal register value of a slave device which is \n\r");
   bbu_printf("       indicated by 'ai2c <address> [<i2c_index>]' previously\n\r");
   bbu_printf(" Usage: ri2c [/w] <reg_addr>\n\r");
   bbu_printf("  <reg_addr> -- stands for the device register address\n\r");
   bbu_printf("  [/w]       -- option for half-word(16-bit) register address,\n\r"
              "                default is 8-bit\n\r");
   return;
}

static void i2cm_help(void){
   bbu_printf("\n\ri2cm - Set/display I2C speed mode and turn on/off i2c driver log \n\r");
   bbu_printf(" Usage: i2cm <i2c_index> <speed_mode>\n\r");
   bbu_printf("  <index> -- stands for the i2c used, For 1 or 2 for Nezha2\n\r");
   bbu_printf("             1: CI2C, 2: PI2C\n\r");
   bbu_printf("  <speed_mode> -- fast(f) or slow(s)\n\r");
   bbu_printf(" Note: use 'i2cm log' to turn on/off i2c driver log \n\r");
   bbu_printf("       If no argument entered, display all I2Cs' current speed modes and if \n\r"
              "       the drive log is turned on or not.");
   return;
}

static void ai2c_help(void){
   bbu_printf("\n\rai2c - Display/Set the slave address and I2C unit for I2C commands.\n\r");
   bbu_printf("       This commands displays/sets the salve address and I2C unit index to \n\r");
   bbu_printf("       be used with the RI2C and WI2C commands. The address entered should \n\r");
   bbu_printf("       always be the value for the slave address. \n\r");
   bbu_printf(" Usage: ai2c <address> [<i2c_index>]\n\r");
   bbu_printf("  <address>   -- optional value that sets the I2C slave address to the entered\n\r");
   bbu_printf("                 value. If no address is entered AI2C will display the current\n\r");
   bbu_printf("                 slave address in use.\n\r");
   bbu_printf("  <i2c_index> -- The index of I2C unit which the slave device connects to\n\r");
   bbu_printf("                 If <i2c_index> not entered, it'll be looked up according to \n\r");
   bbu_printf("                 <address> in the table which is predefined\n\r");
   return;
}

static void i2c_dump_help(void){
   bbu_printf("\n\ri2c_dump - Dump registers in an I2C device\n\r");
   bbu_printf(" Usage: i2c_dump [/w] <start_reg> <dump_reg_num>\n\r");
   bbu_printf("  <start_reg>     -- The first register to dump\n\r");
   bbu_printf("  <dump_reg_num>  -- The numbers of continuous registers to dump\n\r");
   bbu_printf("  [/w]            -- option for half-word(16-bit) register address,\n\r"
              "                     default is 8-bit\n\r");
   return;
}

void disarm_i2c_time_evt(void){
   QTimeEvt_disarm(&(l_i2c.time_out));
}
