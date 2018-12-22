#ifndef _I2C_DEC_H_
#define _I2C_DEC_H_

#include "I2C.h"
#include "qp_port.h"
#include "spinlock.h"

union i2c_data {
   uint8_t      byte;
   uint16_t     word;
   uint8_t      block[32];
};

typedef enum I2C_TYPE{
   I2C_NOT_USED = 0,  //used to stand for Power I2C
   HS1_I2C,
   HS2_I2C,
   I2C_INVALID
}I2C_UNIT;

#define CI2C1   HS1_I2C
#define PI2C    HS2_I2C

//#define I2C_LOG   1

typedef enum REG_SEND_STOP{
   STOP_NO = 0,
   STOP_YES
}REG_SEND_STOP;

typedef enum I2C_RETURNS
{
   I2C_TIMEOUT = 0,
   I2C_OK,
   I2C_ERROR,
   I2C_BUSY_ERR,
   I2C_FREE_ERR,
   I2C_NOT_EXIST,
}I2C_RETURNS;

typedef struct{
	uint8_t i2c_slave;
	uint8_t i2c_unit;
}MRVL_I2C_LIST;

typedef struct{
	spinlock_t i2c_lock;
	uint8_t i2c_status;
	uint8_t i2c_slave;
}MRVL_I2C_UNIT_INFO;

typedef enum{
	I2C_FREE = 0,
	I2C_BUSY,
}I2C_STATUS;

//#define MODE_SWITCH
#define    MRVL_I2C_MAX           (I2C_INVALID-1)          //Orchid has six I2Cs
#define    MRVL_I2C_MAX_SLAVES     16           //Orchid has 16 I2C slaves

#define I2C_SLOW_MODE       (0)
#define I2C_FAST_MODE       (1)
#define I2C_HS_SD_MODE      (2)
#define I2C_HS_FT_MODE      (3)
#define ICR_MODE_STD       SHIFT15(0x0)          /* Standard mode           */
#define ICR_MODE_FAST      SHIFT15(0x1)          /* Fast/Fast-Plus mode     */
             /* HS mode; Standard mode when not doing a high speed transfer */
#define ICR_MODE_HS_SD     SHIFT15(0x2)
             /* HS mode; Fast mode when not doing a high speed transfer     */
#define ICR_MODE_HS_FT     SHIFT15(0x3)
//#define ICR_MODE_MASK      SHIFT15(0x3)

#define ICR_INIT_VALUE     0x014E0

#define I2C_MASTER_CODE    0x08
/* i2c read/write interface functions...................................... */ 
/* i2c driver functtions using waiting mechanism........................... */
/* read n bytes directly from a slave device............................... */
I2C_RETURNS 
i2c_read_block(I2C_UNIT  i2c_index, 
               uint8_t   slave_addr,
               uint32_t  value_len, 
               uint8_t*  value,
               uint32_t  i2c_mode);
               
/* read one byte from 8-bit address register............................... */
I2C_RETURNS 
i2c_read_addr8_byte(I2C_UNIT  i2c_index, 
                    uint8_t   slave_addr,
                    uint8_t   reg_slave_addr, 
                    uint8_t*  value,
                    uint32_t  i2c_mode);
                    
/* read n bytes from 8-bit address register................................ */
I2C_RETURNS 
i2c_read_addr8_block(I2C_UNIT  i2c_index, 
                     uint8_t   slave_addr,
                     uint8_t   reg_slave_addr,
                     uint32_t  value_len, 
                     uint8_t*  value,
                     uint32_t  i2c_mode);   
                     
/* read one byte from 16-bit address register.............................. */
I2C_RETURNS 
i2c_read_addr16_byte(I2C_UNIT  i2c_index, 
                     uint8_t   slave_addr,
                     uint16_t  reg_slave_addr,
                     uint8_t*  value,
                     uint32_t  i2c_mode);

/* read n bytes from 16-bit address register................................ */
I2C_RETURNS 
i2c_read_addr16_block(I2C_UNIT  i2c_index, 
                      uint8_t   slave_addr,
                      uint16_t  reg_slave_addr,
                      uint32_t  value_len, 
                      uint8_t*  value,
                      uint32_t  i2c_mode);

/* write n bytes directly to a slave device................................ */
I2C_RETURNS 
i2c_write_block(I2C_UNIT  i2c_index, 
                uint8_t   slave_addr,
                uint32_t  value_len, 
                uint8_t*  value,
                uint32_t  i2c_mode);
                     
/* write one byte to 8-bit address register................................ */
I2C_RETURNS 
i2c_write_addr8_byte(I2C_UNIT  i2c_index, 
                     uint8_t   slave_addr,
                     uint8_t   reg_slave_addr, 
                     uint8_t   value,
                     uint32_t  i2c_mode);
                     
/* write n bytes to 8-bit address register................................. */
I2C_RETURNS 
i2c_write_addr8_block(I2C_UNIT  i2c_index, 
                      uint8_t   slave_addr,
                      uint8_t   reg_slave_addr,
                      uint32_t  value_len, 
                      uint8_t*  value,
                      uint32_t  i2c_mode);
                      
/* write one byte to 16-bit address register............................... */
I2C_RETURNS 
i2c_write_addr16_byte(I2C_UNIT  i2c_index, 
                      uint8_t   slave_addr,
                      uint16_t  reg_slave_addr, 
                      uint8_t   value,
                      uint32_t  i2c_mode); 

/* write n bytes tp 16-bit address register................................ */
I2C_RETURNS 
i2c_write_addr16_block(I2C_UNIT   i2c_index, 
                       uint8_t    slave_addr,
                       uint16_t   reg_slave_addr, 
                       uint32_t   value_len, 
                       uint8_t*   value,
                       uint32_t   i2c_mode);

#define release_i2c(i2c_unit, i2c_slave)  \
        set_i2c_unit(i2c_unit, i2c_slave, I2C_FREE)
#define control_i2c(i2c_unit, i2c_slave)  \
        set_i2c_unit(i2c_unit, i2c_slave, I2C_BUSY)

/* ........................................................................ */
void i2c_init_all(void);
I2C_RETURNS i2c_init(I2C_UNIT i2c_index); 
I2C_RETURNS i2c_reset(I2C_UNIT i2c_index, int fast_mode);
void i2c_isr_enable_all(void);
void i2c_isr_disable_all(void);
void i2c_isr_enable(I2C_UNIT i2c_index);
void i2c_isr_disable(I2C_UNIT i2c_index);
void mrvl_i2c_isr(void *i2c_index);

MRVL_I2C_TypeDef* i2c_lookup_base_type(I2C_UNIT i2c_index);
/* used in master mode to generate a stop without transmitting another byte */
I2C_RETURNS
i2c_send_master_abort(I2C_UNIT i2c_index);

/*..........................................................................*/              
I2C_RETURNS i2c_test_scan_addr(uint8_t slave_addr, MRVL_I2C_TypeDef *mrvl_i2c);
//I2C_RETURNS i2c_test_read(I2C_UNIT i2c_index,uint8_t reg_slave_addr);
//I2C_RETURNS i2c_test_write(I2C_UNIT i2c_index, uint8_t reg_slave_addr, uint8_t data);

void    i2c_write_uart_scr(uint8_t addr);
uint8_t i2c_read_uart_scr(void);
I2C_UNIT i2c_lookup_index(uint8_t slave_addr);
I2C_RETURNS i2c_mode_change(I2C_UNIT i2c_unit, int fast_mode);
int is_i2c_log_on(void);
void switch_i2c_log(void);
int i2c_get_fast_mode(I2C_UNIT i2c_unit);


/* cell i2c operations........................... */
I2C_RETURNS 
i2c_send_slave_addr(uint8_t slave_addr, 
                    MRVL_I2C_TypeDef *mrvl_i2c, 
                    uint32_t i2c_mode);
I2C_RETURNS 
i2c_send_reg_addr(uint8_t reg_slave_addr,
                  MRVL_I2C_TypeDef *mrvl_i2c, 
                  uint32_t i2c_mode, 
                  uint8_t stop);
I2C_RETURNS 
i2c_put_data(uint8_t data, 
             MRVL_I2C_TypeDef *mrvl_i2c, 
             uint32_t i2c_mode);
I2C_RETURNS 
i2c_put_stop(uint8_t data, 
             MRVL_I2C_TypeDef *mrvl_i2c, 
             uint32_t i2c_mode);
I2C_RETURNS 
i2c_get_data(uint8_t *data, 
              MRVL_I2C_TypeDef *mrvl_i2c,
              uint32_t i2c_mode);
I2C_RETURNS 
i2c_get_stop(uint8_t *data, 
              MRVL_I2C_TypeDef *mrvl_i2c,
              uint32_t i2c_mode);                                                                              
/* i2c write operation..................................................... */
I2C_RETURNS 
i2c_write(I2C_UNIT  i2c_index, 
          uint8_t   slave_addr, 
          uint8_t   *reg_slave_addr,
          uint8_t   reg_addr_len,
          uint32_t  i2c_mode,
          uint8_t   *value, 
          uint32_t  value_len);
/* i2c read operation...................................................... */
I2C_RETURNS 
i2c_read(I2C_UNIT  i2c_index, 
         uint8_t   slave_addr, 
         uint8_t   *reg_slave_addr,
         uint8_t   reg_addr_len,
         uint32_t  i2c_mode,
         uint8_t   *value, 
         uint32_t  value_len);
             

                      

/* i2c interface functions................................................. */
I2C_RETURNS 
pxa_i2c_write_byte(I2C_UNIT  i2c_index,
                   uint8_t   slave_addr, 
                   uint8_t   value ,
                   uint8_t  i2c_mode);
I2C_RETURNS 
pxa_i2c_read_byte (I2C_UNIT  i2c_index, 
                   uint8_t   slave_addr, 
                   uint8_t   *value, 
                   uint8_t  i2c_mode);
I2C_RETURNS
pxa_i2c_write_addr8_byte(I2C_UNIT  i2c_index, 
                         uint8_t  slave_addr,
                         uint8_t  reg_addr, 
                         uint8_t  value ,
                         uint8_t  i2c_mode);
I2C_RETURNS
pxa_i2c_read_addr8_byte(I2C_UNIT  i2c_index, 
                        uint8_t   slave_addr,
                        uint8_t   reg_addr, 
                        uint8_t   *value, 
                        uint8_t   i2c_mode);
                        
/* ........................................................................ */

               
void 
i2c_read_start(uint8_t  slave_addr, 
               uint32_t i2c_mode, MRVL_I2C_TypeDef *mrvl_i2c);
void 
i2c_send_reg(uint8_t  reg_slave_addr, 
             uint32_t i2c_mode, 
             MRVL_I2C_TypeDef *mrvl_i2c, 
             uint8_t stop);
            
void 
i2c_read_data(uint32_t i2c_mode, MRVL_I2C_TypeDef *mrvl_i2c);

void 
i2c_read_idbr(uint8_t *data, MRVL_I2C_TypeDef *mrvl_i2c);

void 
i2c_read_stop(uint32_t i2c_mode, MRVL_I2C_TypeDef *mrvl_i2c);

void 
i2c_write_start(uint8_t slave_addr, 
                uint32_t i2c_mode, MRVL_I2C_TypeDef *mrvl_i2c);

void 
i2c_write_data(uint8_t data, uint32_t i2c_mode, MRVL_I2C_TypeDef *mrvl_i2c);

void 
i2c_write_stop(uint8_t data, uint32_t i2c_mode, MRVL_I2C_TypeDef *mrvl_i2c);

/* ........................................................................ */
#ifdef CONFIG_CR5
int i2c_test_scan(uint8_t i2c_index);
int i2c_scan_test_addr(uint8_t slave_addr, MRVL_I2C_TypeDef *mrvl_i2c);
int i2c_test_read(uint8_t i2c_index,uint8_t reg_slave_addr);
int i2c_test_write(uint8_t i2c_index, uint8_t reg_slave_addr, uint8_t data);
#endif                                                                                                                                                                                                                                                
             
#endif

