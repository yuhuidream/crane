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
#ifdef CONFIG_SMP
#include "spinlock.h"
#endif
#include "qf.h"

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
   MRVL_I2C_TypeDef* mrvl_i2c  = MRVL_I2C2;
   
   mrvl_i2c = i2c_lookup_base_type(i2c_index);

   mrvl_i2c->ISAR = 0;
   
   /* write the device slave address for write */
   if(!i2c_send_slave_addr(slave_addr, mrvl_i2c, i2c_mode))
      goto I2C_TIME_OUT;
   
   /* write device internal register address */
   for(i = 0; i < reg_addr_len; i++){
      if(!i2c_put_data(*(reg_slave_addr + i), mrvl_i2c, i2c_mode))
         goto I2C_TIME_OUT;
   }
   
   /* write the data except and last byte */
   for(i = 0; i < value_len - 1; i++){
      if(!i2c_put_data(*(value + i), mrvl_i2c, i2c_mode))
         goto I2C_TIME_OUT;
   }
   
   /* write the last byte and stop the transmission */
   if(!i2c_put_stop(*(value + value_len - 1), mrvl_i2c, i2c_mode))
      goto I2C_TIME_OUT;
   return I2C_OK;
I2C_TIME_OUT:
   bbu_printf("I2C Timeout!\n\r");
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
   
   /* write the register address for different cases */
   switch(reg_addr_len){
      case 0:
         break;
      case 1:{
         if(!i2c_send_slave_addr(slave_addr, mrvl_i2c, i2c_mode))
            goto I2C_TIME_OUT;
         if(!i2c_send_reg_addr(*(reg_slave_addr), mrvl_i2c, 
                                 i2c_mode, STOP_YES))
            goto I2C_TIME_OUT;
         break;
      }
      case 2:{
         if(!i2c_send_slave_addr(slave_addr, mrvl_i2c, i2c_mode))
            goto I2C_TIME_OUT;
         if(!i2c_send_reg_addr(*(reg_slave_addr), mrvl_i2c, 
                                 i2c_mode, STOP_NO))
            goto I2C_TIME_OUT;
         if(!i2c_send_reg_addr(*(reg_slave_addr+1), 
                                 mrvl_i2c, i2c_mode, STOP_NO))
            goto I2C_TIME_OUT;
         break;
      }
      default:
         //bbu_printf("The register address length not supported!\n\r");
         break;   
   }
   
   /* send the slave address for read */
   if(!i2c_send_slave_addr(slave_addr + 1, mrvl_i2c, i2c_mode))
            goto I2C_TIME_OUT;
            
   /* read the data except and last byte */
   for(i = 0; i < value_len - 1; i++){
      if(!i2c_get_data(value + i, mrvl_i2c, i2c_mode))
         goto I2C_TIME_OUT;
   }
   
   /* write the last byte and stop the transmission */
   if(!i2c_get_stop(value + value_len - 1, mrvl_i2c, i2c_mode))
      goto I2C_TIME_OUT;
   return I2C_OK;
I2C_TIME_OUT:
   bbu_printf("I2C Timeout!\n\r");
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
         ret = I2C_TIMEOUT;
         break;
      }
   }
      
   if ((mrvl_i2c->ISR) & ISR_BED)
   {
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
