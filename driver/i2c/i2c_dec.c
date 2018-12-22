/*****************************************************************************
* Product: BBU
* 
* Purpose: I2C driver
* 
* Copyright (C) 2010-2011 Marvell MTBL-Hefei. All rights reserved.
*
* Author: Yu Zhang  email: zhangy@marvell.com
* 
*****************************************************************************/

#include "predefines.h"
#include "bbu.h"
#include "UART.h"
#include "APBCLK.h"
#include "timer_dec.h"
#include "i2c_dec.h"
#include <string.h>
#include "interrupt.h"
#include "APBCON.h"
#include "spinlock.h"
#include "qf.h"

#ifdef CONFIG_SMP
MRVL_I2C_UNIT_INFO i2c_units_info[MRVL_I2C_MAX];
#endif
int i2c_log_on = MV_TRUE;
static int i2c_fast_mode[MRVL_I2C_MAX];
/****************************************************************************
*****************************************************************************/
static void i2c_clock_enable(I2C_UNIT i2c_index){
   /*!!! Only find two register in APBClock for I2C */
   switch(i2c_index){
      case CI2C1:
         APBClock->TWSI_CLK_RST = 0x7;
         udelay(5);
         APBClock->TWSI_CLK_RST = 0x3;
         break;
      case PI2C:
         APBCON->ICER = 0x7;
         udelay(5);
         APBCON->ICER = 0x3;
         break;
      default:
         break;
   }
   udelay(80);

}

void i2c_init_all(void)
{
   if (!cpu_is_pxa1826_z1z2())
   {
      I2C_UNIT i2c_idx;
      for(i2c_idx = HS2_I2C; i2c_idx >= HS1_I2C; i2c_idx--)
      {
         i2c_init(i2c_idx);
      }
   } else {
      //#ifdef CONFIG_CA7
      i2c_init(HS2_I2C);   //In Nezha3, AP can only access CI2C.
      //#endif
#ifdef CONFIG_CR5
      i2c_init(HS1_I2C);
#endif
   }
}

#define I2C_ICR_REG_INIT_VALUE      (ICR_IUE | ICR_SCLE)
#define I2C_ISR_CLEAR_ALL_INTERRUPTS      0x000007f0

I2C_RETURNS i2c_reset(I2C_UNIT i2c_index, int fast_mode)
{
   MRVL_I2C_TypeDef *mrvl_i2c;
   mrvl_i2c = i2c_lookup_base_type(i2c_index);
   if(mrvl_i2c == NULL)
      return -I2C_NOT_EXIST;
   uint32_t value;
   value = mrvl_i2c->ICR;
   value &= ~ICR_MA;        // Clear the abort bit
   value &= ~ICR_START;     // Clear the start bit
   value &= ~ICR_STOP;      // Clear the stop bit
   value &= ~ICR_ACKNAK;    // Clear the ACK bit
   value &= ~ICR_TB;        // Clear the transfer_byte bit
   value &= ~ICR_MODE_MASK; // Clear mode bits
   if(fast_mode == I2C_HS_SD_MODE){
      value |= ICR_GPIOEN_MASK; // Set gpio mode enable
   }
   
   /* clear the ICR register */
   mrvl_i2c->ICR = 0x0;
   
   /* Set the RESET bit in the ICR rewgister */
   mrvl_i2c->ICR = ICR_UR;      /* reset the I2C interface unit */
   
   /* clear the ISR register after reset */
   mrvl_i2c->ISR = I2C_ISR_CLEAR_ALL_INTERRUPTS;
   
   /* Clear the RESET bit in the ICR rewgister */
   mrvl_i2c->ICR &= ~ICR_UR; 

   value |= ICR_INIT_VALUE;

   /* Restore the ICR register */
   mrvl_i2c->ICR = value;

   value = mrvl_i2c->ILCR;

   /* Fast speed mode register ILCR */
   value &= ~ILCR_FLV_MASK;
   value |= (0x25 << ILCR_FLV_SHIFT);  // 355kHz
//   value |= (0x21 << ILCR_FLV_SHIFT);  // 390kHz
//   value |= (0x20 << ILCR_FLV_SHIFT);  // 400kHz

   /* Slow speed mode register ILCR */
   value &= ~ILCR_SLV_MASK;
//   value |= 0x9A;  // 98.7kHz
//   value |= 0x98;  // 100kHz
   value |= 0xB3;  // 85.5kHz

   /* High speed mode register ILCR */
   value &= ~ILCR_HLVL_MASK;
   value |= (0x02 << ILCR_HLVL_SHIFT); // 1.77MHz Rp = 750
   value &= ~ILCR_HLVH_MASK;
   value |= (0x04 << ILCR_HLVH_SHIFT);

   mrvl_i2c->ILCR = value;

   value = mrvl_i2c->IWCR;

   /* Fast speed mode register IWCR */
   value &= ~IWCR_COUNT_MASK;
   value |= 0x0A;

   /* High speed mode register IWCR */
   value &= ~IWCR_HS_COUNT1_MASK;
   value |= (0x1 << IWCR_HS_COUNT1_SHIFT);
   value &= ~IWCR_HS_COUNT2_MASK;
   value |= (0x5 << IWCR_HS_COUNT2_SHIFT);

   mrvl_i2c->IWCR = value;
   
   if(fast_mode == I2C_FAST_MODE){
      i2c_fast_mode[i2c_index-1] = I2C_FAST_MODE;
#ifdef MODE_SWITCH
      mrvl_i2c->ICR |= ICR_MODE_FAST;
#endif
   }
   else if(fast_mode == I2C_SLOW_MODE){
      i2c_fast_mode[i2c_index-1] = I2C_SLOW_MODE;
   }
   else if(fast_mode == I2C_HS_SD_MODE){
#ifdef MODE_SWITCH
      mrvl_i2c->ICR |= ICR_MODE_HS_SD;
#endif
      i2c_fast_mode[i2c_index-1] = I2C_HS_SD_MODE;
   }
   /* Delay of 1 millisecond - enable the module to sync with the bus */
   mdelay(1);

   return I2C_OK;
}

/* initiate I2C, i2c_index represent different I2C interface unit............*/
I2C_RETURNS i2c_init(I2C_UNIT i2c_index)
{
   MRVL_I2C_TypeDef *mrvl_i2c;
   mrvl_i2c = i2c_lookup_base_type(i2c_index);
   if(mrvl_i2c == NULL)
      return -I2C_NOT_EXIST;
      
   i2c_clock_enable(i2c_index);

   i2c_reset(i2c_index, I2C_SLOW_MODE);
   
   /* Let I2C unit own slave address be 0x0 to avoid indeterminate state */
   mrvl_i2c->ISAR = 0x0;
   
   /* all interrupts are disabled, set standard mode */
   mrvl_i2c->ICR |= (ICR_ALDIE | ICR_GCD | ICR_IUE | ICR_SCLE);

   /* initialize I2C unit information */
   #ifdef CONFIG_SMP
   i2c_units_info[i2c_index-1].i2c_status = I2C_FREE;
   i2c_units_info[i2c_index-1].i2c_slave  = 0x0;
   i2c_units_info[i2c_index-1].i2c_lock = (spinlock_t){0};
   #endif
   return I2C_OK;   
}
static I2C_UNIT i2c_base2index(MRVL_I2C_TypeDef * mrvl_i2c);

void i2c_wait_for_not_busy(MRVL_I2C_TypeDef *mrvl_i2c){
   int time_out = 10;
   while(time_out){
      if(mrvl_i2c->ISR & (ISR_IBB | ISR_EBB | ISR_UB)){ /* I2C unit is busy */
         mdelay(1);
         time_out -- ;
      }
      else
         break;
   }
   if(mrvl_i2c->ISR & (ISR_IBB | ISR_EBB | ISR_UB)){
      i2c_reset(i2c_base2index(mrvl_i2c), I2C_SLOW_MODE);
   }
}

#ifdef CONFIG_SMP
/* ........................................................................ */
I2C_RETURNS
i2c_unit_alloc(I2C_UNIT i2c_index, uint8_t slave_addr){
   
   uint32_t cpsr_bak;
      
   cpsr_bak = disable_irq_fiq();
   spin_lock(&i2c_units_info[i2c_index-1].i2c_lock);

   if(i2c_units_info[i2c_index-1].i2c_status == I2C_FREE){
      i2c_units_info[i2c_index-1].i2c_status = I2C_BUSY;
      i2c_units_info[i2c_index-1].i2c_slave  = slave_addr;
   }
   else{
      spin_unlock(&i2c_units_info[i2c_index-1].i2c_lock);
      restore_irq_fiq(cpsr_bak); 
      bbu_printf("The I2C unit %d is used by other device (address: 0x%02X)"
                 " now, please try later", i2c_index,
                 i2c_units_info[i2c_index-1].i2c_slave);
      return (I2C_BUSY_ERR);
   }
   spin_unlock(&i2c_units_info[i2c_index-1].i2c_lock);
   restore_irq_fiq(cpsr_bak); 
   return I2C_OK; 
}

/* ........................................................................ */
I2C_RETURNS
i2c_unit_free(I2C_UNIT i2c_index, uint8_t slave_addr){
   uint32_t cpsr_bak;
      
   cpsr_bak = disable_irq_fiq();
   spin_lock(&i2c_units_info[i2c_index-1].i2c_lock);
   if(i2c_units_info[i2c_index-1].i2c_slave == slave_addr){
      i2c_units_info[i2c_index-1].i2c_status = I2C_FREE;
      i2c_units_info[i2c_index-1].i2c_slave  = 0x0;
   }
   else{
      spin_unlock(&i2c_units_info[i2c_index-1].i2c_lock);
      restore_irq_fiq(cpsr_bak);
      bbu_printf("The I2C unit can only be freed by its owner device\n\r");
      return (I2C_FREE_ERR);
   }
   spin_unlock(&i2c_units_info[i2c_index-1].i2c_lock);
   restore_irq_fiq(cpsr_bak);
   return I2C_OK; 
}
#endif

/* get the i2c base type according to its index............................ */
MRVL_I2C_TypeDef* i2c_lookup_base_type(I2C_UNIT i2c_index)
{
   MRVL_I2C_TypeDef * mrvl_i2c = NULL;
   switch(i2c_index){
      case CI2C1:
         mrvl_i2c = MRVL_I2C1;
         break;
      case PI2C:
         mrvl_i2c = MRVL_I2C2;
         break;
      default:
         bbu_printf("The i2c index does not match any existing i2c bus\n\r");
         break;
   }

   return mrvl_i2c;
}

/* get the i2c base type according to its index............................ */
I2C_UNIT i2c_base2index(MRVL_I2C_TypeDef * mrvl_i2c)
{
   I2C_UNIT index = I2C_INVALID;
   switch((uint32_t)mrvl_i2c){
      case (uint32_t)MRVL_I2C1:
         index = HS1_I2C;
         break;
      case (uint32_t)MRVL_I2C2:
         index = HS2_I2C;
         break;
      default:
         break;
   }
   return index;
}

/* ........................................................................ */
static int get_i2c_int_num(I2C_UNIT i2c_index){
   int int_num = -1;
   if (!cpu_is_pxa1826_z1z2())
   {
      switch(i2c_index){
         case CI2C1:
            int_num = IRQ_MMP_TWSI1;
            break;
         case PI2C:
            int_num = IRQ_MMP_TWSI_CP;
         default:
            break;
      }
   } else {
      switch(i2c_index){
         case CI2C1:
            int_num = IRQ_MMP_TWSI_CP;
            break;
         case PI2C:
            int_num = IRQ_MMP_TWSI1;
         default:
            break;
      }
   }

   return int_num;
}

/* enable all the I2Cs interrupt........................................... */
void i2c_isr_enable_all(void){
   for(int i=HS1_I2C; i<I2C_INVALID; i++)   //MMP3 have 6 I2Cs
       i2c_isr_enable(i);
   return;
}

void i2c_isr_enable(I2C_UNIT i2c_index){
   int int_num;
   int_num = get_i2c_int_num(i2c_index);
   ISR_Connect(int_num, mrvl_i2c_isr, (void *)(uint32_t)i2c_index);
   INT_Enable(int_num, 0, 15);
   return;
}

/* disable all the I2Cs interrupt.......................................... */
void i2c_isr_disable_all(void){
   for(int i=HS1_I2C; i<I2C_INVALID; i++)    //MMP3 have 6 I2Cs
      i2c_isr_disable(i);
}

void i2c_isr_disable(I2C_UNIT i2c_index){
   int int_num;
   int_num = get_i2c_int_num(i2c_index);
   INT_Disable(int_num);
   return;
}

/* interrupt handler of I2Cs............................................... */
void mrvl_i2c_isr(void *i2c){
   I2C_UNIT i2c_index;
   MRVL_I2C_TypeDef *mrvl_i2c;
   uint32_t isr;
   i2c_index = (I2C_UNIT)(uint32_t)i2c;
   mrvl_i2c = i2c_lookup_base_type(i2c_index);
   if(mrvl_i2c == NULL)
      return;
   isr = mrvl_i2c->ISR;
   mrvl_i2c->ISR = isr;                             /* clear the interrupt */
   I2cRetEvt * retEvt;
   if(isr & ISR_BED){
      retEvt = Q_NEW(I2cRetEvt, I2C_BED_SIG);
      retEvt->i2c_index = i2c_index;
      QACTIVE_POST(AO_I2C,  (QEvent *)retEvt, 0);
   }   
   else if(isr & ISR_ITE){
      retEvt = Q_NEW(I2cRetEvt, I2C_ITE_SIG);
      retEvt->i2c_index = i2c_index;
      QACTIVE_POST(AO_I2C,  (QEvent *)retEvt, 0);
   }
   else if(isr & ISR_IRF){
      retEvt = Q_NEW(I2cRetEvt, I2C_IRF_SIG);
      retEvt->i2c_index = i2c_index;
      QACTIVE_POST(AO_I2C,  (QEvent *)retEvt, 0);
   }
   return;
}

/* test if a slave address of a specified I2C is valid..................... */
I2C_RETURNS i2c_test_scan_addr(uint8_t slave_addr, MRVL_I2C_TypeDef *mrvl_i2c)
{
   i2c_wait_for_not_busy(mrvl_i2c);
   mrvl_i2c->IDBR = slave_addr;
#ifndef MODE_SWITCH
   mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | \
                   ICR_START | ICR_ITEIE | ICR_MODE_STD;
#else
   mrvl_i2c->ICR |= (ICR_START | ICR_TB | ICR_ITEIE);
#endif
   return I2C_OK;
}


/* get the slave address entered in Scratchpad Register.................... */
uint8_t i2c_read_uart_scr(void)
{
   return DBG_UART->SCR;   //UART3 is DEBUG UART
}

/* enter the slave address into Scratchpad Register........................ */
void i2c_write_uart_scr(uint8_t addr){
   DBG_UART->SCR = addr;   //UART3 is DEBUG UART
   return;
}


/* device slave address and its corresponding i2c index mapping table...... */
static MRVL_I2C_LIST mrvl_i2c_info[MRVL_I2C_MAX_SLAVES+1] = {
   {0x60,   PI2C},   //ustica
   {0x62,   PI2C},   //ustica
   {0x64,   PI2C},   //ustica
   {0xD0,   PI2C},   //charger
   {0xD4,    PI2C},   //charger
   {0x7A,   CI2C1},  //display
   {0x60,   CI2C1},  //audio
   {0xFF,   0xFF},
};

/* get the i2c address according to device slave address................... */
I2C_UNIT i2c_lookup_index(uint8_t slave_addr){
   I2C_UNIT i2c_index = I2C_INVALID;
   uint8_t i = 0;
   while(mrvl_i2c_info[i].i2c_unit != 0xFF){
      if(mrvl_i2c_info[i].i2c_slave == slave_addr){
         i2c_index = mrvl_i2c_info[i].i2c_unit;
         break;
      }
      else
         i++;
   }
   return i2c_index;
}

/* used in master mode to generate a stop without transmitting another byte */
I2C_RETURNS
i2c_send_master_abort(I2C_UNIT i2c_index)
{
   MRVL_I2C_TypeDef *mrvl_i2c;
   mrvl_i2c = i2c_lookup_base_type(i2c_index);
   mrvl_i2c->ICR |= ICR_MA;
   return I2C_OK;
}

/* i2c driver functtions using waiting mechanism........................... */

/* read n bytes directly from a slave device............................... */
I2C_RETURNS 
i2c_read_block(I2C_UNIT  i2c_index, 
               uint8_t  slave_addr,
               uint32_t  value_len, 
               uint8_t  *value,
               uint32_t i2c_mode)
{
   return i2c_read(i2c_index, slave_addr, NULL, 0, 
                   i2c_mode, value, value_len);
}               

/* read one byte from 8-bit address register............................... */
I2C_RETURNS 
i2c_read_addr8_byte(I2C_UNIT  i2c_index, 
                    uint8_t  slave_addr,
                    uint8_t  reg_slave_addr, 
                    uint8_t  *value,
                    uint32_t i2c_mode)
{
   return i2c_read(i2c_index, slave_addr, 
                    &reg_slave_addr, 1, i2c_mode, value, 1);
}

/* read n bytes from 8-bit address register................................ */
I2C_RETURNS 
i2c_read_addr8_block(I2C_UNIT  i2c_index, 
                     uint8_t  slave_addr,
                     uint8_t  reg_slave_addr,
                     uint32_t  value_len, 
                     uint8_t  *value,
                     uint32_t i2c_mode)
{
   return i2c_read(i2c_index, slave_addr, 
                   &reg_slave_addr, 1, i2c_mode, value, value_len);
}                    

/* read one byte from 16-bit address register.............................. */
I2C_RETURNS 
i2c_read_addr16_byte(I2C_UNIT  i2c_index, 
                     uint8_t  slave_addr,
                     uint16_t reg_slave_addr,
                     uint8_t  *value,
                     uint32_t i2c_mode)
{
   uint8_t reg_addr[2];
   reg_addr[0] = (uint8_t)((reg_slave_addr >> 8) & 0xFF);
   reg_addr[1] = (uint8_t)(reg_slave_addr & 0xFF);
   return i2c_read(i2c_index, slave_addr, reg_addr, 2, i2c_mode, value, 1);
} 

/* read n bytes from 16-bit address register................................ */
I2C_RETURNS 
i2c_read_addr16_block(I2C_UNIT  i2c_index, 
                     uint8_t  slave_addr,
                     uint16_t  reg_slave_addr,
                     uint32_t  value_len, 
                     uint8_t  *value,
                     uint32_t i2c_mode)
{
   uint8_t reg_addr[2];
   reg_addr[0] = (uint8_t)((reg_slave_addr >> 8) & 0xFF);
   reg_addr[1] = (uint8_t)(reg_slave_addr & 0xFF);
   return i2c_read(i2c_index, slave_addr, 
                   reg_addr, 2, i2c_mode, value, value_len);
}

/* write n bytes directly to a slave device................................ */
I2C_RETURNS 
i2c_write_block(I2C_UNIT  i2c_index, 
                uint8_t  slave_addr,
                uint32_t  value_len, 
                uint8_t  *value,
                uint32_t i2c_mode)
{
   return i2c_write(i2c_index, slave_addr, NULL, 0, 
                    i2c_mode, value, value_len);
}               

/* write one byte to 8-bit address register................................ */
I2C_RETURNS 
i2c_write_addr8_byte(I2C_UNIT  i2c_index, 
                     uint8_t  slave_addr,
                     uint8_t  reg_slave_addr, 
                     uint8_t  value,
                     uint32_t i2c_mode)
{
   return i2c_write(i2c_index, slave_addr, 
                    &reg_slave_addr, 1, i2c_mode, &value, 1);
}

/* write n bytes to 8-bit address register................................. */
I2C_RETURNS 
i2c_write_addr8_block(I2C_UNIT  i2c_index, 
                      uint8_t  slave_addr,
                      uint8_t  reg_slave_addr,
                      uint32_t  value_len, 
                      uint8_t  *value,
                      uint32_t i2c_mode)
{
   return i2c_write(i2c_index, slave_addr, 
                    &reg_slave_addr, 1, i2c_mode, value, value_len);
}

/* write one byte to 16-bit address register............................... */
I2C_RETURNS 
i2c_write_addr16_byte(I2C_UNIT  i2c_index, 
                      uint8_t  slave_addr,
                      uint16_t reg_slave_addr, 
                      uint8_t  value,
                      uint32_t i2c_mode)
{
   uint8_t reg_addr[2];
   reg_addr[0] = (uint8_t)((reg_slave_addr >> 8) & 0xFF);//high
   reg_addr[1] = (uint8_t)(reg_slave_addr & 0xFF);   //low
   /* write high 8-bit register address first */
   return i2c_write(i2c_index, slave_addr, 
                           reg_addr, 2, i2c_mode, &value, 1);
}

/* write n bytes tp 16-bit address register................................ */
I2C_RETURNS 
i2c_write_addr16_block(I2C_UNIT  i2c_index, 
                      uint8_t  slave_addr,
                      uint16_t reg_slave_addr, 
                      uint32_t  value_len, 
                      uint8_t  *value,
                      uint32_t i2c_mode)
{
   uint8_t reg_addr[2];
   reg_addr[0] = (uint8_t)((reg_slave_addr >> 8) & 0xFF);
   reg_addr[1] = (uint8_t)(reg_slave_addr & 0xFF);
   return i2c_write(i2c_index, slave_addr, 
                           reg_addr, 2, i2c_mode, value, value_len);
}

/* i2c write operation..................................................... */
I2C_RETURNS 
i2c_write(I2C_UNIT  i2c_index, 
          uint8_t  slave_addr, 
          uint8_t  *reg_slave_addr,
          uint8_t  reg_addr_len,
          uint32_t i2c_mode,
          uint8_t  *value, 
          uint32_t  value_len)
{
   uint8_t i;
   #ifdef CONFIG_SMP
   if(i2c_unit_alloc(i2c_index, slave_addr) == I2C_BUSY_ERR)
      return I2C_BUSY_ERR;
   #endif
   MRVL_I2C_TypeDef* mrvl_i2c  = MRVL_I2C2;
   
   mrvl_i2c = i2c_lookup_base_type(i2c_index);

   mrvl_i2c->ISAR = 0;
#ifdef I2C_LOG
         bbu_printf("\r\n[I2C I/O] write address: %02X", slave_addr);
#endif
   
   /* write the device slave address for write */
   if(!i2c_send_slave_addr(slave_addr, mrvl_i2c, i2c_mode))
      goto I2C_TIME_OUT;
#ifdef I2C_LOG
      bbu_printf("\r\n[I2C I/O] send slave_addr: %02X", slave_addr);
#endif
   
   /* write device internal register address */
   for(i = 0; i < reg_addr_len; i++){
      if(!i2c_put_data(*(reg_slave_addr + i), mrvl_i2c, i2c_mode))
         goto I2C_TIME_OUT;
#ifdef I2C_LOG
      bbu_printf("\r\n[I2C I/O] send reg_addr: %02X", *(reg_slave_addr + i));
#endif
   }
   
   /* write the data except and last byte */
   for(i = 0; i < value_len - 1; i++){
      if(!i2c_put_data(*(value + i), mrvl_i2c, i2c_mode))
         goto I2C_TIME_OUT;
#ifdef I2C_LOG
      bbu_printf("\r\n[I2C I/O] write value: %02X", *(value + i));
#endif
   }
   
   /* write the last byte and stop the transmission */
   if(!i2c_put_stop(*(value + value_len - 1), mrvl_i2c, i2c_mode))
      goto I2C_TIME_OUT;
#ifdef I2C_LOG
      bbu_printf("\r\n[I2C I/O] write value: %02X\r\n", *(value + value_len - 1));
#endif
   #ifdef CONFIG_SMP
   i2c_unit_free(i2c_index, slave_addr);
   #endif
   return I2C_OK;
I2C_TIME_OUT:
   #ifdef CONFIG_SMP
   i2c_unit_free(i2c_index, slave_addr);
   #endif
   return I2C_TIMEOUT;
}

/* i2c read operation...................................................... */
I2C_RETURNS 
i2c_read(I2C_UNIT  i2c_index, 
         uint8_t   slave_addr, 
         uint8_t   *reg_slave_addr,
         uint8_t   reg_addr_len,
         uint32_t  i2c_mode,
         uint8_t   *value, 
         uint32_t  value_len)
{
   uint8_t i;
   #ifdef CONFIG_SMP
   if(i2c_unit_alloc(i2c_index, slave_addr) == I2C_BUSY_ERR)
      return I2C_BUSY_ERR;
   #endif
   MRVL_I2C_TypeDef *mrvl_i2c = MRVL_I2C2;
   mrvl_i2c = i2c_lookup_base_type(i2c_index);
#ifdef I2C_LOG
         bbu_printf("\r\n[I2C I/O] read address: %02X", slave_addr);
#endif
   /* write the register address for different cases */
   switch(reg_addr_len){
      case 0:
         break;
      case 1:{
         if(!i2c_send_slave_addr(slave_addr, mrvl_i2c, i2c_mode))
            goto I2C_TIME_OUT;
#ifdef I2C_LOG
         bbu_printf("\r\n[I2C I/O] send slave_addr: %02X", slave_addr);
#endif
         if(!i2c_send_reg_addr(*(reg_slave_addr), mrvl_i2c, 
                                 i2c_mode, STOP_YES))
            goto I2C_TIME_OUT;
#ifdef I2C_LOG
         bbu_printf("\r\n[I2C I/O] send reg_addr: %02X", *(reg_slave_addr));
#endif
         break;
      }
      case 2:{
         if(!i2c_send_slave_addr(slave_addr, mrvl_i2c, i2c_mode))
            goto I2C_TIME_OUT;
#ifdef I2C_LOG
         bbu_printf("\r\n[I2C I/O] send slave_addr: %02X", slave_addr);
#endif
         if(!i2c_send_reg_addr(*(reg_slave_addr), mrvl_i2c, 
                                 i2c_mode, STOP_NO))
            goto I2C_TIME_OUT;
#ifdef I2C_LOG
         bbu_printf("\r\n[I2C I/O] send reg_addr: %02X", *(reg_slave_addr));
#endif
         if(!i2c_send_reg_addr(*(reg_slave_addr+1), 
                                 mrvl_i2c, i2c_mode, STOP_NO))
            goto I2C_TIME_OUT;
#ifdef I2C_LOG
         bbu_printf("\r\n[I2C I/O] send reg_addr: %02X", *(reg_slave_addr + 1));
#endif
         break;
      }
      default:
         bbu_printf("The register address length not supported!\n\r");
         break;   
   }
   
   /* send the slave address for read */
   if(!i2c_send_slave_addr(slave_addr + 1, mrvl_i2c, i2c_mode))
            goto I2C_TIME_OUT;
#ifdef I2C_LOG
         bbu_printf("\r\n[I2C I/O] send slave_addr for read: %02X", slave_addr + 1);
#endif
            
   /* read the data except and last byte */
   for(i = 0; i < value_len - 1; i++){
      if(!i2c_get_data(value + i, mrvl_i2c, i2c_mode))
         goto I2C_TIME_OUT;
#ifdef I2C_LOG
      bbu_printf("\r\n[I2C I/O] read value: %02X", *(value + i));
#endif
   }
   
   /* write the last byte and stop the transmission */
   if(!i2c_get_stop(value + value_len - 1, mrvl_i2c, i2c_mode))
      goto I2C_TIME_OUT;
#ifdef I2C_LOG
   bbu_printf("\r\n[I2C I/O] read value: %02X\r\n", *(value + value_len - 1));
#endif
   #ifdef CONFIG_SMP
   i2c_unit_free(i2c_index, slave_addr);
   #endif
   return I2C_OK;
I2C_TIME_OUT:
   #ifdef CONFIG_SMP
   i2c_unit_free(i2c_index, slave_addr);
   #endif
   return I2C_TIMEOUT;
}

/* write slave address and start a write operation.......................... */
inline 
I2C_RETURNS 
i2c_send_slave_addr(uint8_t slave_addr, 
                    MRVL_I2C_TypeDef *mrvl_i2c, 
                    uint32_t i2c_mode)
{
   uint32_t timeout;
   uint32_t isr;
   int ret = I2C_OK;
   i2c_wait_for_not_busy(mrvl_i2c);

   if(i2c_mode == ICR_MODE_HS_SD) {
      mrvl_i2c->IDBR = I2C_MASTER_CODE;
      mrvl_i2c->ICR  = ICR_IUE | ICR_SCLE | ICR_TB | ICR_START | ICR_GPIOEN | ICR_ITEIE | i2c_mode;
      timeout = 0x4000;
      while(1){
         if(!((mrvl_i2c->ICR) & ICR_TB))       /* was the byte send yet */
            break;
            timeout--;
         if(timeout <= 0){
            if(i2c_log_on)
               bbu_printf("I2C Timeout!\n\r");
            ret = I2C_TIMEOUT;
            break;
         }
      }
      isr = mrvl_i2c->ISR;
      mrvl_i2c->ISR = isr;
   }

   //Send slave address
   mrvl_i2c->IDBR = slave_addr;   //load slave address to data buffer
#ifndef MODE_SWITCH
   mrvl_i2c->ICR  = ICR_IUE | ICR_SCLE | ICR_TB | ICR_START | i2c_mode;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR  |= (ICR_TB | ICR_START);
#endif

   timeout = 0x4000;
   while(1){
      if(!((mrvl_i2c->ICR) & ICR_TB))       /* was the byte send yet */
         break;
         timeout--;
      if(timeout <= 0){
         if(i2c_log_on)
            bbu_printf("I2C Timeout!\n\r");
         ret = I2C_TIMEOUT;
         break;
      }
   }
      
   if ((mrvl_i2c->ISR) & ISR_BED)
   {
      if(i2c_log_on)
         bbu_printf("I2C NO ack!\n\r");
      ret = I2C_TIMEOUT;      //no ack
   }      

   /* clear the interrupt */
   isr = mrvl_i2c->ISR;
   mrvl_i2c->ISR = isr;
   
   return ret;
}

/* send slave register address.............................................. */
inline
I2C_RETURNS 
i2c_send_reg_addr(uint8_t reg_slave_addr,
                  MRVL_I2C_TypeDef *mrvl_i2c, 
                  uint32_t i2c_mode, 
                  uint8_t stop)
{
   uint32_t timeout;
   uint32_t isr;
   int ret = I2C_OK;
   
   //write  register address
   mrvl_i2c->IDBR = reg_slave_addr;
   if(stop){
#ifndef MODE_SWITCH
      mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | i2c_mode | ICR_STOP;
#else
      (void)i2c_mode;
      mrvl_i2c->ICR |= (ICR_TB | ICR_STOP);
#endif
   }else{
#ifndef MODE_SWITCH
      mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | i2c_mode;
#else
      mrvl_i2c->ICR |= ICR_TB;
#endif
   }
   timeout = 0x4000;
   while (1) {
      if (!((mrvl_i2c->ICR) & ICR_TB))   /*was the byte send yet?*/
         break;
      timeout--;
      if (timeout <= 0) {
         if(i2c_log_on)
            bbu_printf("I2C timeout!\n\r");
         ret = I2C_TIMEOUT;
         break;
      }
   }
   /* clear the interrupt */
   isr = mrvl_i2c->ISR;
   mrvl_i2c->ISR = isr;   
   return ret;
}

/* write data to the slave.................................................. */
inline 
I2C_RETURNS 
i2c_put_data(uint8_t data, 
             MRVL_I2C_TypeDef *mrvl_i2c, 
             uint32_t i2c_mode)
{
   uint32_t timeout;
   uint32_t isr;
   int ret = I2C_OK;

   /* write data to IDBR */
   mrvl_i2c->IDBR = data;
#ifndef MODE_SWITCH
   mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | i2c_mode;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR |= ICR_TB;
#endif
   
   timeout = 0x4000;
   while(1){
      if(!((mrvl_i2c->ICR) & ICR_TB))   /* was the byte send yet */
         break;
         timeout--;
      if(timeout <= 0){
         if(i2c_log_on)
            bbu_printf("I2C Timeout!\n\r");
         ret = I2C_TIMEOUT;
         break;
      }
   }

   /* clear the interrupt */
   isr = mrvl_i2c->ISR;
   mrvl_i2c->ISR = isr;

   return ret;
}

/* write last data and stop the write operation............................. */
inline 
I2C_RETURNS 
i2c_put_stop(uint8_t           data, 
             MRVL_I2C_TypeDef *mrvl_i2c, 
             uint32_t          i2c_mode)
{
   uint32_t timeout;
   uint32_t isr;
   int ret = I2C_OK;
   
   //write data to register
   mrvl_i2c->IDBR = data;   //load slave address to data buffer
#ifndef MODE_SWITCH
   mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | ICR_STOP | i2c_mode | \
                   ICR_MSDE;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR |= (ICR_TB | ICR_STOP | ICR_MSDE);
#endif
   timeout = 0x4000;
   while(1){
      /* was the byte send yet and master stop detected */
      if(!(((mrvl_i2c->ICR) & ICR_TB)) && ((mrvl_i2c->ISR)&ISR_MSD))
         break;
      timeout--;
      if(timeout <= 0){
         if(i2c_log_on)
            bbu_printf("I2C Timeout!\n\r");
         ret = I2C_TIMEOUT;
         break;
      }
   }

   /* clear the interrupt */
   isr = mrvl_i2c->ISR;
   mrvl_i2c->ISR = isr;
   mrvl_i2c->ICR &= (~(ICR_STOP));
   return ret;
}

/* read data from IDBR but donot stop the operation......................... */
inline 
I2C_RETURNS 
i2c_get_data(uint8_t          *data, 
             MRVL_I2C_TypeDef *mrvl_i2c,
             uint32_t         i2c_mode)
{
   uint32_t timeout;
   uint32_t isr;
   int ret = I2C_OK;

#ifndef MODE_SWITCH
   mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | i2c_mode;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR |= ICR_TB;
#endif
   timeout = 0x4000;
   while (1) {
      if(!((mrvl_i2c->ICR) & ICR_TB))       /* was the byte send yet */
         break;
      timeout--;
      if (timeout <= 0) {
         if(i2c_log_on)
            bbu_printf("I2C timeout!\n\r");
         ret = I2C_TIMEOUT;
         break;
      }
   }
   *data = mrvl_i2c->IDBR;             /* Read the IDBR data */
   
   /* Clear the ICR[STOP] and ICR[ACKNAK] bits */
   mrvl_i2c->ICR &= (~(ICR_STOP | ICR_ACKNAK));
   /* clear the interrupt */
   isr = mrvl_i2c->ISR;
   mrvl_i2c->ISR = isr;
   return ret;
}

/* read data from IDBR and stop the read operation.......................... */
inline 
I2C_RETURNS 
i2c_get_stop(uint8_t           *data, 
             MRVL_I2C_TypeDef  *mrvl_i2c,
             uint32_t           i2c_mode)
{
   uint32_t timeout;
   uint32_t isr;
   int ret = I2C_OK;

#ifndef MODE_SWITCH
   mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_ACKNAK | ICR_TB | \
                  ICR_STOP | i2c_mode | ICR_MSDE;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR |= (ICR_ACKNAK | ICR_STOP | ICR_TB | ICR_MSDE);
#endif

   timeout = 0x4000;
   while (1) {
      /* was the byte send yet and master stop detected */
      if(!(((mrvl_i2c->ICR) & ICR_TB)) && ((mrvl_i2c->ISR)&ISR_MSD))
         break;
      timeout--;
      if (timeout <= 0) {
         if(i2c_log_on)
            bbu_printf("I2C timeout!\n\r");
         ret = I2C_TIMEOUT;
         break;
      }
   }
   *data = mrvl_i2c->IDBR;             /* Read the IDBR data */

   /* clear the interrupt */
   isr = mrvl_i2c->ISR;
   mrvl_i2c->ISR = isr;

   /* Clear the ICR[STOP] and ICR[ACKNAK] bits */
   mrvl_i2c->ICR &= (~(ICR_STOP | ICR_ACKNAK));
   return ret;
}

/* different write operations...............................................*/
I2C_RETURNS 
pxa_i2c_write(I2C_UNIT  i2c_index, 
              uint8_t   i2c_mode, 
              uint8_t   slave_addr,
              uint8_t   *reg_addr,
              uint8_t   reg_addr_len, 
              uint8_t   *value,
              uint32_t  value_len)
{
   I2C_RETURNS ret    = I2C_TIMEOUT;
   I2cRwEvt *rwEvt     = Q_NEW(I2cRwEvt, I2C_WRITE_SIG);
   rwEvt->i2c_index    = i2c_index;
   rwEvt->i2c_mode     = i2c_mode;
   rwEvt->length       = value_len;
   rwEvt->reg_addr     = reg_addr;
   rwEvt->reg_addr_len = reg_addr_len;
   rwEvt->slave_addr   = slave_addr;
   rwEvt->value        = value;
#ifndef CONFIG_SMP
   rwEvt->AO_callI2C   = QC_getCurrentAO(0)->prio;
#else
   rwEvt->AO_callI2C   = QC_getCurrentAO(QC_getSmpCoreId())->prio;
#endif
   QACTIVE_POST(AO_I2C, (QEvent *)rwEvt, 0);
   ret = QA_SUSPEND_WAIT4RESUME();
   return ret;
}

/* ........................................................................ */
I2C_RETURNS 
pxa_i2c_read (I2C_UNIT  i2c_index, 
              uint8_t   i2c_mode, 
              uint8_t   slave_addr,
              uint8_t   *reg_addr,
              uint8_t   reg_addr_len, 
              uint8_t   *value,
              uint32_t  value_len)
{
   I2C_RETURNS ret    = I2C_TIMEOUT;
   I2cRwEvt *rwEvt     = Q_NEW(I2cRwEvt, I2C_READ_SIG);
   rwEvt->i2c_index    = i2c_index;
   rwEvt->i2c_mode     = i2c_mode;
   rwEvt->length       = value_len;
   rwEvt->reg_addr     = reg_addr;
   rwEvt->reg_addr_len = reg_addr_len;
   rwEvt->slave_addr   = slave_addr;
   rwEvt->value        = value;
#ifndef CONFIG_SMP
   rwEvt->AO_callI2C   = QC_getCurrentAO(0)->prio;
#else
   rwEvt->AO_callI2C   = QC_getCurrentAO(QC_getSmpCoreId())->prio;
#endif
   QACTIVE_POST(AO_I2C, (QEvent *)rwEvt, 0);
   ret = QA_SUSPEND_WAIT4RESUME();
   return ret;
}

/* ........................................................................ */
I2C_RETURNS 
pxa_i2c_write_byte(I2C_UNIT  i2c_index,
                   uint8_t   slave_addr, 
                   uint8_t   value ,
                   uint8_t  i2c_mode)   
{
   return pxa_i2c_write(i2c_index, i2c_mode, slave_addr, 0, 0, 
                        &value, 1);
}

/* ........................................................................ */
I2C_RETURNS 
pxa_i2c_read_byte (I2C_UNIT  i2c_index, 
                   uint8_t   slave_addr, 
                   uint8_t   *value, 
                   uint8_t  i2c_mode)   
{
   return pxa_i2c_read(i2c_index, i2c_mode, slave_addr, 0, 0, 
                        value, 1);
}

/* ........................................................................ */
I2C_RETURNS
pxa_i2c_write_addr8_byte(I2C_UNIT  i2c_index, 
                         uint8_t  slave_addr,
                         uint8_t  reg_addr, 
                         uint8_t  value ,
                         uint8_t  i2c_mode)   
{
   return pxa_i2c_write(i2c_index, i2c_mode, slave_addr, &reg_addr, 1, 
                         &value, 1);
}

/* ........................................................................ */
I2C_RETURNS 
pxa_i2c_write_addr8_block(I2C_UNIT  i2c_index, 
                          uint8_t   slave_addr,
                          uint8_t   reg_addr,
                          uint32_t  value_len, 
                          uint8_t   *value,
                          uint8_t   i2c_mode)
{
   return pxa_i2c_write(i2c_index, i2c_mode, slave_addr,
                       &reg_addr, 1, value, value_len);
}

/* ........................................................................ */
I2C_RETURNS
pxa_i2c_read_addr8_byte(I2C_UNIT  i2c_index, 
                        uint8_t   slave_addr,
                        uint8_t   reg_addr, 
                        uint8_t   *value, 
                        uint8_t   i2c_mode)      
{
   return pxa_i2c_read(i2c_index, i2c_mode, slave_addr, &reg_addr, 1, 
                       value, 1);
}

/* read n bytes from 8-bit address register................................ */
I2C_RETURNS 
pxa_i2c_read_addr8_block(I2C_UNIT  i2c_index, 
                     uint8_t   slave_addr,
                     uint8_t   reg_addr,
                     uint32_t  value_len, 
                     uint8_t   *value,
                     uint8_t   i2c_mode)
{
   return pxa_i2c_read(i2c_index, i2c_mode, slave_addr, 
                       &reg_addr, 1, value, value_len);
}

/* start i2c read operation................................................ */
void 
i2c_read_start(uint8_t slave_addr, 
               uint32_t i2c_mode, 
               MRVL_I2C_TypeDef *mrvl_i2c)
{
   i2c_wait_for_not_busy(mrvl_i2c);
   mrvl_i2c->IDBR = slave_addr;
#ifndef MODE_SWITCH
   mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | \
                   ICR_START | ICR_ITEIE | i2c_mode;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR |= (ICR_TB | ICR_ITEIE | ICR_START);
#endif
   
   return;
}

/* send register address to i2c controller................................. */
void 
i2c_send_reg(uint8_t  reg_slave_addr, 
             uint32_t i2c_mode, 
             MRVL_I2C_TypeDef *mrvl_i2c, uint8_t stop)
{
   mrvl_i2c->IDBR = reg_slave_addr;
   if(stop){
#ifndef MODE_SWITCH
      mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | \
                      ICR_ITEIE  | i2c_mode | ICR_STOP;
#else
      (void)i2c_mode;
      mrvl_i2c->ICR |= (ICR_TB | ICR_STOP | ICR_ITEIE);
#endif
   }else{
#ifndef MODE_SWITCH
      mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | ICR_ITEIE  | i2c_mode;
#else
      mrvl_i2c->ICR |= (ICR_TB | ICR_ITEIE);
#endif
   }
   return;
}

/* set the i2c controller register to read data............................ */
void
i2c_read_data(uint32_t i2c_mode, MRVL_I2C_TypeDef *mrvl_i2c)
{
#ifndef MODE_SWITCH
   mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | i2c_mode | \
         ICR_TB | ICR_IRFIE;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR |= (ICR_TB | ICR_IRFIE);
#endif
   return;
}

/* read the value in IDBR register......................................... */
void
i2c_read_idbr(uint8_t *data, MRVL_I2C_TypeDef *mrvl_i2c){
   *data = mrvl_i2c->IDBR;
   return;
}

/* read data from IDBR and stop the read operation......................... */
void
i2c_read_stop(uint32_t i2c_mode, MRVL_I2C_TypeDef *mrvl_i2c)
{
#ifndef MODE_SWITCH
   mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | i2c_mode | ICR_ACKNAK | ICR_TB | \
                  ICR_STOP | ICR_IRFIE | ICR_MSDE;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR |= (ICR_ACKNAK | ICR_TB | ICR_STOP | ICR_IRFIE | ICR_MSDE);
#endif

   return;
}
/****************************************************************************/

/* write slave address and start a write operation......................... */
void 
i2c_write_start(uint8_t  slave_addr, 
                uint32_t i2c_mode, 
                MRVL_I2C_TypeDef *mrvl_i2c)
{
   i2c_wait_for_not_busy(mrvl_i2c);
   //Send slave address
   mrvl_i2c->IDBR = slave_addr;   //load slave address to data buffer
#ifndef MODE_SWITCH
   mrvl_i2c->ICR  = ICR_IUE | ICR_SCLE | ICR_TB | \
                    ICR_START | i2c_mode | ICR_ITEIE;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR |= (ICR_TB | ICR_START | ICR_ITEIE);
#endif
   
   return;
}

/* write data to the slave................................................. */
void
i2c_write_data(uint8_t data, uint32_t i2c_mode, MRVL_I2C_TypeDef *mrvl_i2c)
{
   mrvl_i2c->IDBR = data;
#ifndef MODE_SWITCH
   mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | i2c_mode | ICR_TB | ICR_ITEIE;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR |= (ICR_TB | ICR_ITEIE);
#endif

   return;
}

/* write last data and stop the write operation............................ */
void
i2c_write_stop(uint8_t data, uint32_t i2c_mode, MRVL_I2C_TypeDef *mrvl_i2c)
{
   //write data to register
   mrvl_i2c->IDBR = data;
#ifndef MODE_SWITCH
   mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | \
                   ICR_STOP | i2c_mode | ICR_ITEIE | ICR_MSDE;
#else
   (void)i2c_mode;
   mrvl_i2c->ICR |= (ICR_TB | ICR_ITEIE | ICR_STOP | ICR_MSDE);
#endif
   
   return;
}

/* Open/Close the I2C log printer.......................................... */
void switch_i2c_log(void)
{
   if(i2c_log_on == MV_TRUE)
      i2c_log_on = MV_FALSE;
   else
      i2c_log_on = MV_TRUE;
}

/* ........................................................................ */
int is_i2c_log_on(void)
{
   return i2c_log_on;
}

/* ........................................................................ */
I2C_RETURNS i2c_mode_change(I2C_UNIT i2c_unit, int fast_mode)
{
   if(fast_mode)
      i2c_fast_mode[i2c_unit] = I2C_FAST_MODE;
   else
      i2c_fast_mode[i2c_unit] = I2C_SLOW_MODE;
   return i2c_reset(i2c_unit, fast_mode);
}

/* ........................................................................ */
int i2c_get_fast_mode(I2C_UNIT i2c_unit)
{
   return i2c_fast_mode[i2c_unit];
}
#ifdef CONFIG_CR5
/****************************************************************************
*****************************************************************************/

int i2c_test_scan(uint8_t i2c_index){
        MRVL_I2C_TypeDef *mrvl_i2c;	

        uint32_t slave_addr = 0x02;	
        bbu_printf("\n\r");
        switch(i2c_index){
            case CI2C1:
               mrvl_i2c = MRVL_I2C1;
               break;
            case PI2C:
               mrvl_i2c = MRVL_I2C2;
               break;
            default:
              bbu_printf("i2c_scan--> The index of I2C is wrong.\n\r");
              bbu_printf("i2c_scan--> 1~2.\n\r");
              return 0;
        }
        i2c_wait_for_not_busy(mrvl_i2c);
        bbu_printf("i2c_scan--> List of valid I2C %d bus device addresses:\n\r", i2c_index);
        while(slave_addr < 0x100)
        {  
	        slave_addr &= 0xFF;
	        if(i2c_index == 2 && slave_addr == 0x26)
		        slave_addr += 0x2;  
	        if(i2c_scan_test_addr(slave_addr, mrvl_i2c)){
		        bbu_printf("i2c_scan--> I2C device found at address 0x%02X\n\r", slave_addr);
		        mrvl_i2c->ICR = 0x10;		/* abort the transmission */
	        }
	        slave_addr += 0x2;		
        }
	
	return 1;
}

/* test if a slave address of a specified I2C is valid...................... */
int i2c_scan_test_addr(uint8_t slave_addr, MRVL_I2C_TypeDef *mrvl_i2c)
{
        volatile uint32_t timeout;
        uint32_t isr;
        volatile uint8_t stat = 1;

        mrvl_i2c->IDBR = slave_addr;

        mrvl_i2c->ICR = ICR_IUE | ICR_SCLE | ICR_TB | ICR_START | ICR_MODE_STD;

        timeout = 0x4000;
        while(1){
	        if(!((mrvl_i2c->ICR) & ICR_TB))	/* was the byte send yet */
	        break;
	        timeout--;
	        if(timeout <= 0){
		        stat = 0;
		        break;
	        }
        }

        if ((mrvl_i2c->ISR) & ISR_BED){
	        stat =0 ;
        }

        isr = mrvl_i2c->ISR;
        mrvl_i2c->ISR = isr;

        return stat;
}

int i2c_test_write(uint8_t i2c_index, uint8_t reg_slave_addr, uint8_t data){
        MRVL_I2C_TypeDef *mrvl_i2c = i2c_lookup_base_type(i2c_index);
        i2c_wait_for_not_busy(mrvl_i2c);
        uint8_t slave_addr;
        slave_addr = DBG_UART->SCR;
        if(i2c_write_addr8_byte(i2c_index,slave_addr,reg_slave_addr,data,ICR_MODE_STD))
	        bbu_printf("wi2c--> I2C %d slave register updated.\n\r",i2c_index);
        else
	        bbu_printf("wi2c--> I2C bus time out. No device at address 0x%02X\n\r on I2C %d",slave_addr, i2c_index);
        return 1;
}

int i2c_test_read(uint8_t i2c_index,uint8_t reg_slave_addr){
        MRVL_I2C_TypeDef *mrvl_i2c = i2c_lookup_base_type(i2c_index);
        i2c_wait_for_not_busy(mrvl_i2c);
        uint8_t data;
        uint8_t slave_addr;
        slave_addr = DBG_UART->SCR;

        if(i2c_read_addr8_byte(i2c_index,slave_addr,reg_slave_addr, &data,ICR_MODE_STD))
	        bbu_printf("ri2c--> I2C slave register data: 0x%02X\n\r",data);
        else
	        bbu_printf("ri2c--> I2C bus time out. No device at address 0x%02X on I2C %d\n\r",slave_addr, i2c_index);
        return 1;  
}

#endif

