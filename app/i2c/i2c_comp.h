#ifndef _I2C_COMP_H_
#define _I2C_COMP_H_

#include "predefines.h"
#include "qp_port.h"
#include "i2c_dec.h"
#include "bbu.h"

typedef struct I2C_COMPTag{
   QHsm      super;
   QEQueue   deferQueue;     /* native QF queue for deferred request events */
   QEvent    const *deferQSto[3];      /* storage for deferred queue buffer */

   uint8_t   i2c_index;                           /* index of specified I2C */
   uint8_t   slave_addr;                  /* slave address of target device */
   uint8_t   value_index;
   uint8_t   test_type;
   
   uint8_t   reg_addr_idx;
   uint8_t   reg_addr_len;
   uint8_t   AO_callI2C;               /* priority of the AO which call I2C */
   uint8_t   i2c_mode;                    /* I2C bus mode. default:standard */

   uint32_t  length;                                      /* length of data */
   uint8_t   *value;             /* pointer to data for transmit or receive */
   uint8_t   *reg_addr;
   int       scr_cmd_index;
   MRVL_I2C_TypeDef * mrvl_i2c;
}I2C_COMP;

typedef enum{
   I2C_NO_TEST = 0,
   I2C_DUMP_TEST,
   I2C_READ_TEST,
   I2C_WRITE_TEST,
   I2C_SCAN_TEST,
}I2C_TEST_TYPE;

#define i2c_comp_init(me_)           QHsm_init((QHsm *)(me_), (QEvent *)0)
#define i2c_comp_dispatch(me_, e_)   QHsm_dispatch((QHsm *)(me_), e_)

QHsm *i2c_comp_ctor(uint8_t id);

extern void disarm_i2c_time_evt(void);

#endif
