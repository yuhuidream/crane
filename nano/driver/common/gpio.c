#include "predefines.h"
#include "GPIO.h"
#include "gpio_dec.h"
#include "interrupt.h"
#include "APBCLK.h"
#include "timer_dec.h"

static inline GPIO_TypeDef *get_gpio_base(int gpio_idx);

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
      bbu_printf("Err! The GPIO %d is invalid\n\r", gpio_idx);
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
