/*****************************************************************************
* Product: BBU
* 
* Purpose: I2C Component
* 
* Copyright (C) 2010-2011 Marvell MTBL-Hefei. All rights reserved.
*
* Author: Yu Zhang  email: zhangy@marvell.com
* 
*****************************************************************************/
#include "predefines.h"
#include "i2c_comp.h"
#include "bbu_malloc.h"
#include <string.h>
#include <stdlib.h>

Q_DEFINE_THIS_FILE


static QState i2c_comp_initial         (I2C_COMP *me, QEvent const *e);
static QState i2c_comp_busy            (I2C_COMP *me, QEvent const *e);
static QState i2c_comp_idle            (I2C_COMP *me, QEvent const *e);
static QState i2c_comp_read            (I2C_COMP *me, QEvent const *e);
static QState i2c_comp_write           (I2C_COMP *me, QEvent const *e);
static QState i2c_comp_test_scan       (I2C_COMP *me, QEvent const *e);

static I2C_COMP l_I2C_comp[MRVL_I2C_MAX];

#define I2C_comp_ID(me_)    ((me_) - l_I2C_comp)


QHsm *i2c_comp_ctor(uint8_t id){
   I2C_COMP *me;
   
   Q_REQUIRE(id < MRVL_I2C_MAX);

   me = &l_I2C_comp[id];
   QEQueue_init(&me->deferQueue, me->deferQSto, Q_DIM(me->deferQSto));
   QHsm_ctor(&me->super, (QStateHandler)&i2c_comp_initial);
   return (QHsm *)me;
}
   
QState i2c_comp_initial (I2C_COMP *me, QEvent const *e){
   (void)e;
   me->reg_addr_idx = 0;
   me->value_index   =0;
   me->i2c_index = I2C_comp_ID(me) + 1;   /* get the i2c index */
   me->mrvl_i2c = i2c_lookup_base_type(me->i2c_index);
   me->scr_cmd_index = 0;
   me->test_type = I2C_NO_TEST;
   return Q_TRAN(&i2c_comp_idle);
}

/*..........................................................................*/
QState i2c_comp_idle (I2C_COMP *me, QEvent const *e){
   uint32_t address;
   switch(e->sig){
      case Q_ENTRY_SIG:{
         me->value_index = 0;
         me->reg_addr_idx = 0;
         if(me->test_type){
            disarm_i2c_time_evt();
            screen_cmd_ack(SCREEN_TEST_PASS, me->scr_cmd_index);
            me->test_type = I2C_NO_TEST;
         }
         //QActive_recall(AO_I2C, &me->deferQueue);
         return Q_HANDLED();
      }
      case I2C_READ_SIG:{
         I2cRwEvt *readEvt = (I2cRwEvt *)e;
         me->slave_addr   = readEvt->slave_addr;
         me->i2c_mode     = readEvt->i2c_mode;
         me->reg_addr     = readEvt->reg_addr;
         me->reg_addr_len = readEvt->reg_addr_len;
         me->value        = readEvt->value;
         me->length       = readEvt->length;
         me->AO_callI2C   = readEvt->AO_callI2C;
         return Q_TRAN(&i2c_comp_read);
      }
      case I2C_WRITE_SIG:{
         I2cRwEvt * writeEvt = (I2cRwEvt *)e;
         me->slave_addr   = writeEvt->slave_addr;
         me->i2c_mode     = writeEvt->i2c_mode;
         me->reg_addr   = writeEvt->reg_addr;
         me->reg_addr_len = writeEvt->reg_addr_len;
         me->value        =  writeEvt->value;
         me->length       = writeEvt->length;
         me->AO_callI2C   = writeEvt->AO_callI2C;
         return Q_TRAN(&i2c_comp_write);
      }
      case I2C_RD_TEST_SIG:{
#ifdef CONFIG_CR5
         disarm_i2c_time_evt();
         if(((ComCmdEvt *)e)->argc == 2){  //no [/w]
            me->i2c_index = I2C_comp_ID(me)+1;
            me->slave_addr = strtoul((const char *)((ComCmdEvt *)e)->argv[1],0,16);
            i2c_test_read(me->i2c_index,me->slave_addr);
         }else{
            bbu_printf("CR5 does not support half-word register address\r\n");
         }
         bbu_printf(BBU_PROMPT);
         return Q_HANDLED();
#endif
         uint8_t *test_buffer = (uint8_t *)bbu_malloc(10);
         me->reg_addr   = test_buffer;
         me->value      = test_buffer + 4;   /* register address <= 4Bytes */
         me->length     = 1;
         me->i2c_mode   = ICR_MODE_STD;
         me->slave_addr = i2c_read_uart_scr();
         if(((ComCmdEvt *)e)->argc == 2){    //no [/w]
            address = strtoul((const char *)((ComCmdEvt *)e)->argv[1], 0, 16);
            me->reg_addr_len = 1;
            me->reg_addr[0] = (address&0xFF);
         }
         else{
            address = strtoul((const char *)((ComCmdEvt *)e)->argv[2], 0, 16);
            me->reg_addr_len = 2;
            me->reg_addr[0] = (address>>8)&0xFF;
            me->reg_addr[1] = (address&0xFF);
         }
         me->test_type = I2C_READ_TEST;
         return Q_TRAN(&i2c_comp_read);
      }
      case I2C_WR_TEST_SIG:{
#ifdef CONFIG_CR5
         disarm_i2c_time_evt();
         if(((ComCmdEvt *)e)->argc == 3){  //no [/w]
            me->i2c_index = I2C_comp_ID(me)+1;
            me->slave_addr = strtoul((const char *)((ComCmdEvt *)e)->argv[1],0,16);
            me->value = strtoul(((ComCmdEvt *)e)->argv[2],0,16);
            i2c_test_write(me->i2c_index,me->slave_addr, me->value);
         }else{
            bbu_printf("CR5 does not support half-word register address\r\n");
	      }
         bbu_printf(BBU_PROMPT);
         return Q_HANDLED();
#endif
         uint8_t *test_buffer = (uint8_t *)bbu_malloc(10);
         me->reg_addr   = test_buffer;
         me->value      = test_buffer + 4;   /* register address <= 4Bytes */
         me->length     = 1;
         me->i2c_mode   = ICR_MODE_STD;
         me->slave_addr = i2c_read_uart_scr();
         if(((ComCmdEvt *)e)->argc == 3){  //no [/w]
            address = strtoul((const char *)((ComCmdEvt *)e)->argv[1], 0, 16);
            me->value[0] = strtoul((const char *)((ComCmdEvt *)e)->argv[2], 0, 16);
            me->reg_addr_len = 1;
            me->reg_addr[0] = (address&0xFF);
         }
         else{
            address = strtoul((const char *)((ComCmdEvt *)e)->argv[2], 0, 16);
            me->value[0] = strtoul((const char *)((ComCmdEvt *)e)->argv[3], 0, 16);
            me->reg_addr_len = 2;
            me->reg_addr[0] = (address>>8)&0xFF;
            me->reg_addr[1] = (address&0xFF);
         }
         me->test_type = I2C_WRITE_TEST;
         return Q_TRAN(&i2c_comp_write);
      }
      case I2C_DUMP_SIG:{
         me->i2c_mode = ICR_MODE_STD;
         me->slave_addr = i2c_read_uart_scr();
         if(((ComCmdEvt *)e)->argc == 3){  //no [/w]
            address = strtoul((const char *)((ComCmdEvt *)e)->argv[1], 0, 16);
            me->length= atoi((const char *)((ComCmdEvt *)e)->argv[2]);
            uint8_t *test_buffer = (uint8_t *)bbu_malloc(me->length + 4);
            me->reg_addr   = test_buffer;
            me->value      = test_buffer + 4;   /* register address <= 4Bytes */
            me->reg_addr_len = 1;
            me->reg_addr[0] = (address&0xFF);
         }
         else{
            address = strtoul((const char *)((ComCmdEvt *)e)->argv[2], 0, 16);
            me->length= atoi((const char *)((ComCmdEvt *)e)->argv[3]);
            uint8_t *test_buffer = (uint8_t *)bbu_malloc(me->length + 4);
            me->reg_addr  = test_buffer;
            me->value      = test_buffer + 4;   /* register address <= 4Bytes */
            me->reg_addr_len = 2;
            me->reg_addr[0] = (address>>8)&0xFF;
            me->reg_addr[1] = (address&0xFF);
         }
         me->test_type = I2C_DUMP_TEST;
         return Q_TRAN(&i2c_comp_read);
      }
      case I2C_SCAN_SIG:{
#ifdef CONFIG_CR5
         disarm_i2c_time_evt();
         i2c_test_scan(I2C_comp_ID(me)+1);
         bbu_printf(BBU_PROMPT);
         return Q_HANDLED();
#endif
         if(((ComCmdEvt *)e)->argc == 3){
            me->slave_addr = (uint8_t)conv_hex((char *)((ComCmdEvt *)e)->argv[2]);
            if(!me->slave_addr)
               me->slave_addr = 0x02;
         }else{
            me->slave_addr = 0x02;
         }
         me->i2c_mode   = ICR_MODE_STD;
         me->scr_cmd_index = ((ComCmdEvt *)e)->scr_cmd_index;
         me->test_type = I2C_SCAN_TEST;
         return Q_TRAN(&i2c_comp_test_scan);   
      }
      case I2C_INIT_SIG:{
         i2c_init(I2C_comp_ID(me)+1);         /* initiate the i2c component */
         bbu_printf("ii2c--> The i2c %d is initialized\n\r", I2C_comp_ID(me)+1);
         return Q_HANDLED();
      }
      
   }
   return Q_SUPER(&QHsm_top);
}

/*..........................................................................*/
QState i2c_comp_read(I2C_COMP * me, QEvent const * e){
   switch(e->sig){
      case Q_ENTRY_SIG:{
                  /* start the transaction and send the write slave address */
         i2c_read_start(me->slave_addr, me->i2c_mode, me->mrvl_i2c);
         return Q_HANDLED();
      }
      case I2C_ITE_SIG:{
         if(me->reg_addr_idx < me->reg_addr_len){
            i2c_send_reg(me->reg_addr[me->reg_addr_idx], 
                         me->i2c_mode, me->mrvl_i2c, STOP_NO);
            me->reg_addr_idx ++;
         }
         else if(me->reg_addr_idx == me->reg_addr_len){
                      /* complete sending reg_addr, send slave read address */
            i2c_read_start(me->slave_addr + 1, me->i2c_mode, me->mrvl_i2c);
            me->reg_addr_idx ++;
         }
         else if(me->value_index == me->length -1){
                     /* request to read last data and stop the transaction  */
            i2c_read_stop(me->i2c_mode, me->mrvl_i2c);
         }
         else{                                    /* request to read a data */
            i2c_read_data(me->i2c_mode, me->mrvl_i2c);
         }
         return Q_HANDLED();
      }
      case I2C_IRF_SIG:{
         if(me->value_index == me->length -1){        /* last data is ready */
            i2c_read_idbr(&me->value[me->value_index], me->mrvl_i2c);
            if(I2C_READ_TEST == me->test_type){         /* read test branch */
               bbu_printf("ri2c--> I2C slave register data: "
                          "0x%02X\n\r"BBU_PROMPT,*(me->value));
               bbu_mfree((void *)me->reg_addr);
            }
            else if(I2C_DUMP_TEST == me->test_type)     /* dump test branch */
            {
               bbu_printf("\r\n");
               if(me->reg_addr_len == 1)
                  bbu_printf("i2c_dump--> The start dump register is "
                             "0x%02X\n\r", me->reg_addr[0]);
               else
                  bbu_printf("i2c_dump--> The start dump register is "
                           "0x%02X%02X\n\r", me->reg_addr[0],me->reg_addr[1]);
               for(uint32_t i =0;i < me->length;i++){
                  bbu_printf("i2c_dump--> The value of next %2d register"
                          " is 0x%02X\n\r", 
                          i, me->value[i]);
               }
               bbu_mfree((void *)me->reg_addr);
               bbu_printf(BBU_PROMPT);
            }
            else{                                /* normal read transaction */
               //QACTIVE_POST(me->AO_callI2C,Q_NEW(QEvent, I2C_SUC_SIG),me);
               QA_KICK_SEND_RESULT(me->AO_callI2C, I2C_OK);
            }
            return Q_TRAN(&i2c_comp_idle);
         }
         else if(me->value_index == me->length -2){/*second last data ready */
            i2c_read_idbr(&me->value[me->value_index], me->mrvl_i2c);
            me->value_index ++;
                      /* request to read last data and stop the transaction */
            i2c_read_stop(me->i2c_mode, me->mrvl_i2c);
         }
         else{
            i2c_read_idbr(&me->value[me->value_index], me->mrvl_i2c);
            me->value_index ++;
                                                  /* request to read a data */
            i2c_read_data(me->i2c_mode, me->mrvl_i2c);
         }
         
         return Q_HANDLED();         
      }
      case I2C_BED_SIG:{
         if(I2C_READ_TEST == me->test_type){
            bbu_mfree((void *)me->reg_addr);
            bbu_printf("ri2c--> I2C bus time out. No device at address"
                  " 0x%02X on I2C %d\n\r"BBU_PROMPT,me->slave_addr, me->i2c_index);
         }
         else if(I2C_DUMP_TEST == me->test_type){
            bbu_mfree((void *)me->reg_addr);
            bbu_printf("i2c_dump--> Can not read the registers, "
                    "please check and try again!\n\r"BBU_PROMPT);
         }
         else{
            bbu_printf("I2C Bus error!\r\n");
            QA_KICK_SEND_RESULT(me->AO_callI2C, I2C_TIMEOUT);
            //QACTIVE_POST(me->AO_callI2C,Q_NEW(QEvent, I2C_ERROR_SIG),me);
         }
         return Q_TRAN(&i2c_comp_idle);         
      }
      case I2C_TIMEOUT_SIG:{
         if(me->test_type == I2C_READ_TEST || me->test_type == I2C_DUMP_TEST){
            bbu_printf("i2c_scan-->  I2C %d TIME OUT when read the slave :"
                      " 0x%02X\n\r", me->i2c_index, me->slave_addr);
            me->test_type = I2C_NO_TEST;
            screen_cmd_ack(SCREEN_TEST_FIAL, me->scr_cmd_index);
         }
         return Q_HANDLED();
      }
   }
   return Q_SUPER(&i2c_comp_busy);
}


/*..........................................................................*/
QState i2c_comp_write(I2C_COMP * me, QEvent const * e){
   switch(e->sig){
      case Q_ENTRY_SIG:{
                  /* start the transaction and send the write slave address */
         i2c_write_start(me->slave_addr, me->i2c_mode, me->mrvl_i2c);
         return Q_HANDLED();
      }
      case I2C_ITE_SIG:{
         if(me->reg_addr_idx < me->reg_addr_len ){
            i2c_send_reg(me->reg_addr[me->reg_addr_idx], 
                         me->i2c_mode, me->mrvl_i2c, STOP_NO);
            me->reg_addr_idx ++ ;
         }
         else if(me->reg_addr_idx == me->reg_addr_len){
            if(me->value_index < me->length - 1){
               i2c_write_data(me->value[me->value_index], 
                              me->i2c_mode, me->mrvl_i2c);
               me->value_index++;
            }
            else if(me->value_index == me->length - 1){
               i2c_write_stop(me->value[me->value_index], 
                              me->i2c_mode, me->mrvl_i2c);
               me->value_index ++;
            }
            else{
               if(I2C_WRITE_TEST == me->test_type){
                  bbu_mfree((void *)me->reg_addr);
                  bbu_printf("wi2c--> I2C %d slave register updated."
                             "\n\r"BBU_PROMPT, me->i2c_index);
               }
               else{
                  QA_KICK_SEND_RESULT(me->AO_callI2C, I2C_OK);
                //QACTIVE_POST(me->AO_callI2C,Q_NEW(QEvent, I2C_SUC_SIG),me);
               }
               return Q_TRAN(&i2c_comp_idle);
            }
         }
         return Q_HANDLED();
      }
      case I2C_BED_SIG:{
         if(I2C_WRITE_TEST == me->test_type){
            bbu_mfree((void *)me->reg_addr);
            bbu_printf("wi2c--> I2C bus time out. No device at address "
                       "0x%02X on I2C %d\n\r"BBU_PROMPT,
                       me->slave_addr, me->i2c_index);
         }
         else{
            bbu_printf("I2C Bus error!\r\n");
            QA_KICK_SEND_RESULT(me->AO_callI2C, I2C_TIMEOUT);
            //QACTIVE_POST(me->AO_callI2C,Q_NEW(QEvent, I2C_ERROR_SIG),me);
         }
         return Q_TRAN(&i2c_comp_idle);         
      }
      case I2C_TIMEOUT_SIG:{
         if(me->test_type == I2C_WRITE_TEST){
            bbu_printf("i2c_scan-->  I2C %d TIME OUT when write to slave :"
                      " 0x%02X\n\r", me->i2c_index, me->slave_addr);
            screen_cmd_ack(SCREEN_TEST_FIAL, me->scr_cmd_index);
            me->test_type = I2C_NO_TEST;
         }
         return Q_HANDLED();
      }
   }
   return Q_SUPER(&i2c_comp_busy);
}

static QState i2c_comp_test_scan      (I2C_COMP *me, QEvent const *e){
   switch(e->sig){
      case Q_ENTRY_SIG:{
         bbu_printf("\n\ri2c_scan--> List of valid I2C %d bus "
                    "device addresses:\n\r", me->i2c_index);
         i2c_test_scan_addr(me->slave_addr, me->mrvl_i2c);
         return Q_HANDLED();
      }
      case I2C_ITE_SIG:{
         bbu_printf("i2c_scan--> I2C device on I2C %d found at address"
                    " 0x%02X\n\r", me->i2c_index, me->slave_addr);
         i2c_send_master_abort(I2C_comp_ID(me)+1);
         me->slave_addr += 0x02;
         if(me->slave_addr < 0xFE){
            i2c_test_scan_addr(me->slave_addr, me->mrvl_i2c);
            return Q_HANDLED();
         }
         else{
            bbu_printf(BBU_PROMPT);
            return Q_TRAN(&i2c_comp_idle);
         }
      }
      case I2C_BED_SIG:{
         me->slave_addr += 0x02;
         if(me->slave_addr < 0xFE){
            i2c_test_scan_addr(me->slave_addr, me->mrvl_i2c);
            return Q_HANDLED();
         }
         else{
            bbu_printf(BBU_PROMPT);
            return Q_TRAN(&i2c_comp_idle);   
         }
      }
      case I2C_TIMEOUT_SIG:{
         bbu_printf("i2c_scan-->  I2C %d TIME OUT when scan the slave address:"
                    " 0x%02X\n\r", me->i2c_index, me->slave_addr);
         me->test_type = I2C_NO_TEST;
         screen_cmd_ack(SCREEN_TEST_FIAL, me->scr_cmd_index);
         return Q_HANDLED();
      }
   }
   return Q_SUPER(&i2c_comp_busy);
}


/*..........................................................................*/
QState i2c_comp_busy(I2C_COMP *me, QEvent const *e){ 
   switch(e->sig){
      case Q_ENTRY_SIG:{
         //bbu_printf("i2c in busy\n\r");
         return Q_HANDLED();
      }
      case I2C_INIT_SIG:{
         i2c_init(I2C_comp_ID(me)+1);         /* initiate the i2c component */
         bbu_printf("ii2c--> The i2c unit %d is reset\n\r", I2C_comp_ID(me)+1);
         return Q_TRAN(&i2c_comp_idle);
      }
      case I2C_SUC_SIG:{
         return Q_TRAN(&i2c_comp_idle);
      }
      case I2C_READ_SIG:
      case I2C_WRITE_SIG:{
         QActive_defer(AO_I2C, &me->deferQueue, e);
         return Q_HANDLED();
      }
      case I2C_RD_TEST_SIG:{
         disarm_i2c_time_evt();
         bbu_printf("ri2c--> This I2C unit is not idle, use 'ii2c %d' "
                     "to reset it first\n\r"BBU_PROMPT, I2C_comp_ID(me)+1);
         return Q_HANDLED();
      }
      case I2C_WR_TEST_SIG:{
         disarm_i2c_time_evt();
         bbu_printf("wi2c--> This I2C unit is not idle, use 'ii2c %d' "
                     "to reset it first\n\r"BBU_PROMPT, I2C_comp_ID(me)+1);
         return Q_HANDLED();
      }
      case I2C_DUMP_SIG:{
         disarm_i2c_time_evt();
         bbu_printf("i2c_dump--> This I2C unit is not idle, use 'ii2c %d' "
                     "to reset it first\n\r"BBU_PROMPT, I2C_comp_ID(me)+1);
         return Q_HANDLED();
      }
      case I2C_SCAN_SIG:{
         disarm_i2c_time_evt();
         bbu_printf("i2c_scan--> This I2C unit is not idle, use 'ii2c %d' "
                     "to reset it first\n\r"BBU_PROMPT, I2C_comp_ID(me)+1);
         return Q_HANDLED();
      }  
   }
   return Q_SUPER(&QHsm_top);
}
