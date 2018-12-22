#include "predefines.h"
#include "GPIO.h"
#include "gpio_dec.h"
#include "interrupt.h"
#include "APBCLK.h"
#include "timer_dec.h"

static inline GPIO_TypeDef *get_gpio_base(int gpio_idx);

void *gpioISR_JumpTable[GPIO_NUMS];

void (*gpioISR_Handle)(void);

static void gpio_isr_handler(void *arg)
{
   uint32_t int_num;
   (void)arg;
   
   int_num = gpio_get_int_num();
   
   if(int_num != GPIO_SPURIOUS_INT)
   {
      //clear GPIO interrupt
      gpio_clear_int(int_num);
      
      /* get ISR entry from table and call it */
      gpioISR_Handle =  (void (*)())(gpioISR_JumpTable[int_num]);
      if ( gpioISR_Handle )
         (gpioISR_Handle)();
   }
   
}

void gpio_init(void)
{
   GPIO_TypeDef *mrvl_gpio;
   uint32_t gpio_base = GPIO0_BASE;
   
   APBClock->GPIO_CLK_RST = GPIO_CLK_RST_FNCLK | GPIO_CLK_RST_APBCLK;
   udelay(80);
   
   for(uint32_t i = 0; i < 3; i++)
   {
      mrvl_gpio = (GPIO_TypeDef *)(gpio_base + i*4);
      /* unmask GPIO edge detection for all 4 banks - APMASKx */
      mrvl_gpio->APMASK_REG = 0xFFFFFFFF;
      mrvl_gpio->PDR = 0;
      /* clear up edge detect status register for interrupt */
      mrvl_gpio->RER = 0;
      mrvl_gpio->FER = 0;
      mrvl_gpio->EDR = ~0;
   }
   
   gpio_base = GPIO3_BASE;
   for(uint32_t i = 0; i < 3; i++)
   {
      mrvl_gpio = (GPIO_TypeDef *)(gpio_base + i*4);
      /* unmask GPIO edge detection for all 4 banks - APMASKx */
      mrvl_gpio->APMASK_REG = 0xFFFFFFFF;
      mrvl_gpio->PDR = 0;
      /* clear up edge detect status register for interrupt */
      mrvl_gpio->RER = 0;
      mrvl_gpio->FER = 0;
      mrvl_gpio->EDR = ~0;
   }
   
   for (uint32_t i=0; i<GPIO_NUMS; i++ )
   { 
      gpioISR_JumpTable[i] = NULL;
   }
   
   ISR_Connect(IRQ_MMP_GPIO_AP, gpio_isr_handler, NULL);
   INT_Enable(IRQ_MMP_GPIO_AP, 0, 15);
}





void gpio_int_enable(int gpio_idx, int rise_fall)
{
   uint32_t pin_idx = gpio_idx % 32;
   GPIO_TypeDef *mrvl_gpio = get_gpio_base(gpio_idx);
   if(mrvl_gpio == (GPIO_TypeDef *)NULL)
      return;
   mrvl_gpio->CDR |= BIT(pin_idx);    //set as input
   if(rise_fall == GPIO_RISE)
      mrvl_gpio->SRER |= BIT(pin_idx);
   else if(rise_fall == GPIO_FALL)
      mrvl_gpio->SFER |= BIT(pin_idx);
}

void gpio_int_disable(int gpio_idx, int rise_fall){
   uint32_t pin_idx = gpio_idx % 32;
   GPIO_TypeDef *mrvl_gpio = get_gpio_base(gpio_idx);
   if(mrvl_gpio == (GPIO_TypeDef *)NULL)
      return;
   if(rise_fall == GPIO_RISE)
      mrvl_gpio->CRER |= BIT(pin_idx);
   else if(rise_fall == GPIO_FALL)
      mrvl_gpio->CFER |= BIT(pin_idx);
}

void gpio_clear_int(int gpio_idx){
   uint32_t pin_idx;
   GPIO_TypeDef *mrvl_gpio = get_gpio_base(gpio_idx);
   if(mrvl_gpio == (GPIO_TypeDef *)NULL)
      return;
   pin_idx = gpio_idx % 32;
   mrvl_gpio->EDR = BIT(pin_idx);       
}

int gpio_lookup_int(int gpio_idx){
   uint32_t pin_idx = gpio_idx % 32;
   GPIO_TypeDef *mrvl_gpio = get_gpio_base(gpio_idx);
   if(mrvl_gpio == (GPIO_TypeDef *)NULL)
      return 0;
   uint32_t status = mrvl_gpio->EDR;
   if(status & BIT(pin_idx))
      return 1;
   return 0;
}

uint32_t gpio_get_pin_level_register(uint32_t grp_idx)
{
   GPIO_TypeDef *mrvl_gpio;
   uint32_t gpio_base = GPIO0_BASE;
   if(grp_idx < 3){
      mrvl_gpio = (GPIO_TypeDef *)(gpio_base + grp_idx*4);
   }
   if(grp_idx>=3 && grp_idx<6)
   {
      gpio_base = GPIO3_BASE;
      grp_idx -= 3;
      mrvl_gpio = (GPIO_TypeDef *)(gpio_base + grp_idx*4);
   }
   return mrvl_gpio->PLR;
}

void gpio_isr_connect(int gpio_idx, void (*ISR_Handle)() )
{
   if(NULL == ISR_Handle || gpio_idx < 0 || gpio_idx >= GPIO_NUMS) 
      return;
   
   /* assign the user defined function*/
   gpioISR_JumpTable[gpio_idx] = (void *)ISR_Handle;
}

int gpio_get_int_num(void)
{
   uint32_t int_status, msk, i, j;
   uint32_t gpio_base = GPIO0_BASE;
   GPIO_TypeDef *mrvl_gpio;
   for(j = 0; j < 3; j++)
   {
      mrvl_gpio = (GPIO_TypeDef *)(gpio_base + j*4);
      int_status = mrvl_gpio->EDR;
      if ( int_status != 0 )
      {
         for ( i=0, msk = 0x00000001; i<=31; i++, msk<<=1 )
         {
            if ( int_status & msk )
               return (i+j*32);
          }
      }
   }
   if(j>=3 && j<6)
   {
      for(j = 3; j < 6; j++)
      {
         gpio_base = GPIO3_BASE;
         mrvl_gpio = (GPIO_TypeDef *)(gpio_base + (j-3)*4);
         int_status = mrvl_gpio->EDR;
         if ( int_status != 0 )
         {
            for ( i=0, msk = 0x00000001; i<=31; i++, msk<<=1 )
            {
               if ( int_status & msk )
                  return (i+j*32);
             }
         }
      }
   }
   else
   {
      bbu_printf("got spurious gpio interrupt\r\n");
   }
   return GPIO_SPURIOUS_INT;
}





void gpio_ap_mask_cfg(int gpio_idx, int mask){
   uint32_t pin_idx = gpio_idx % 32;
   GPIO_TypeDef *mrvl_gpio = get_gpio_base(gpio_idx);
   if(mrvl_gpio == (GPIO_TypeDef *)NULL)
      return;
   if(EDGE_DET_DISMASK == mask)
      mrvl_gpio->APMASK_REG |= BIT(pin_idx);
   else if(EDGE_DET_MASK == mask)
      mrvl_gpio->APMASK_REG &= ~BIT(pin_idx);
}

void gpio_ext_proc_mask_cfg(int gpio_idx, int mask){
   uint32_t pin_idx = gpio_idx % 32;
   GPIO_TypeDef *mrvl_gpio = get_gpio_base(gpio_idx);
   if(EDGE_DET_DISMASK == mask)
      mrvl_gpio->EXTPROCMASK_REG |= BIT(pin_idx);
   else if(EDGE_DET_MASK == mask)
      mrvl_gpio->EXTPROCMASK_REG &= ~BIT(pin_idx);
}

int gpio_get_direction(int gpio_idx)
{
   uint32_t pin_idx = gpio_idx % 32;
   uint32_t val;
   GPIO_TypeDef *mrvl_gpio = get_gpio_base(gpio_idx);
   if(mrvl_gpio == (GPIO_TypeDef *)NULL)
      return -1;
   val = mrvl_gpio->PDR;
   if(val & BIT(pin_idx))
      return 1;  /* output */
   else
      return 0;  /* input */
}

void gpio_set_direction(int gpio_idx, int direction)
{
   uint32_t pin_idx = gpio_idx % 32;
   GPIO_TypeDef *mrvl_gpio = get_gpio_base(gpio_idx);
   if(mrvl_gpio == (GPIO_TypeDef *)NULL)
      return;
   if(GPIO_IN == direction)
   {
      mrvl_gpio->CDR |= BIT(pin_idx);
   }
   else if(GPIO_OUT == direction)
   {
      mrvl_gpio->SDR |= BIT(pin_idx);
   }
}

void gpio_output_set(int gpio_idx, int value)
{
   uint32_t pin_idx = gpio_idx % 32;
   GPIO_TypeDef *mrvl_gpio = get_gpio_base(gpio_idx);
   if(mrvl_gpio == (GPIO_TypeDef *)NULL)
      return;
   if(GPIO_HIGH == value)
   {
      mrvl_gpio->PSR |= BIT(pin_idx);
   }
   else if(GPIO_LOW == value)
   {
      mrvl_gpio->PCR |= BIT(pin_idx);
   }
}

uint32_t gpio_get_pin_level(int gpio_idx)
{
   uint32_t pin_idx = gpio_idx % 32;
   GPIO_TypeDef *mrvl_gpio = get_gpio_base(gpio_idx);
   if(mrvl_gpio == (GPIO_TypeDef *)NULL){
      bbu_printf("Error! The GPIO %d is invalid\n\r", gpio_idx);
      return 0;
   }
   uint32_t reg = mrvl_gpio->PLR;
   return (reg >> pin_idx) & 1;
}

static inline GPIO_TypeDef *get_gpio_base(int gpio_idx)
{
   uint32_t gpio_base, grp_idx;
   if(gpio_idx < 0 || gpio_idx >= GPIO_NUMS)
      return (GPIO_TypeDef *)NULL; 
   gpio_base = GPIO0_BASE;
   grp_idx = gpio_idx / 32;
   if(grp_idx >= 3 && grp_idx <6 )
   {
      gpio_base = GPIO3_BASE;
      grp_idx -= 3;
   }
   return (GPIO_TypeDef *)(gpio_base + grp_idx*4);
}
