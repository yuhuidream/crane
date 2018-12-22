#ifndef _GPIO_DEC_H_
#define _GPIO_DEC_H_

#define GPIO_IN    0
#define GPIO_OUT   1

#define GPIO_HIGH          1
#define GPIO_LOW           0
#define GPIO_RISE          1
#define GPIO_FALL          0
#define EDGE_DET_MASK      1
#define EDGE_DET_DISMASK   0

#define GPIO_SPURIOUS_INT	0xFFFF

void gpio_init(void);

#define BIT(x)	(1<<x)

void gpio_isr_connect(int gpio_idx, void (*ISR_Handle)() );

void gpio_set_direction(int gpio_idx, int direction);
void gpio_output_set(int gpio_idx, int value);
void gpio_int_enable(int gpio_idx, int rise_fall);
void gpio_int_disable(int gpio_idx, int rise_fall);

    
/* Config GPIO pin as (interput)input of rising-edge or falling-edge....... */
#define gpio_set_input(gpio_x_)   \
        gpio_set_direction(gpio_x_, GPIO_IN)
        
/* Config GPIO pin as input and enable rising-edge interrupt............... */
#define gpio_rise_enable(gpio_x_)   \
        gpio_int_enable(gpio_x_, GPIO_RISE)
        
/* Config GPIO pin as input and enable falling-edge interrupt.............. */
#define gpio_fall_enable(gpio_x_)   \
        gpio_int_enable(gpio_x_, GPIO_FALL)
        
/* Disable GPIO pin falling-edge interrupt................................. */
#define gpio_fall_disable(gpio_x_)   \
        gpio_int_disable(gpio_x_, GPIO_FALL)

/* Disable GPIO pin rising-edge interrupt.................................. */
#define gpio_rise_disable(gpio_x_)   \
        gpio_int_disable(gpio_x_, GPIO_RISE)    

/* Config GPIO pin as output of logic high or low.......................... */
#define gpio_set_output(gpio_x_)   \
        gpio_set_direction(gpio_x_, GPIO_OUT)
        
/* Config GPIO pin as output of logic high................................. */
#define gpio_output_high(gpio_x_)   \
        gpio_output_set(gpio_x_, GPIO_HIGH)
        
/* Config GPIO pin as output of logic low.................................. */
#define gpio_output_low(gpio_x_)   \
        gpio_output_set(gpio_x_, GPIO_LOW)

/* look up specified gpio to find if it get a interrupt.................... */
int gpio_lookup_int(int gpio_x);    

/* clear GPIO interrupt.................................................... */
void gpio_clear_int(int gpio_x);

int gpio_get_int_num(void);

uint32_t gpio_get_pin_level(int gpio_idx); 
uint32_t gpio_get_pin_level_register(uint32_t reg);  
int gpio_get_direction(int gpio_idx);                      

/*  GPIO Edge detects mark config */
#define gpio_ap_mask(gpio_x_)              \
                    gpio_ap_mask_cfg(gpio_x_, EDGE_DET_MASK)
#define gpio_ap_dismask(gpio_x_)           \
                    gpio_ap_mask_cfg(gpio_x_, EDGE_DET_DISMASK)
#define gpio_ext_proc_mask(gpio_x_)        \
                    gpio_ext_proc_mask_cfg(gpio_x_, EDGE_DET_MASK)
#define gpio_ext_proc_dismask(gpio_x_)     \
                    gpio_ext_proc_mask_cfg(gpio_x_, EDGE_DET_DISMASK)
/* Moltres */
void gpio_ap_mask_cfg(int gpio_x, int mask);
/* External Processor */
void gpio_ext_proc_mask_cfg(int gpio_x, int mask);

                                    
/* list of all the GPIO pins */
enum {
	GPIO_INVALID = -1,

	GPIO0 = 0,
	GPIO1,
	GPIO2,
	GPIO3,
	GPIO4,
	GPIO5,
	GPIO6,
	GPIO7,
	GPIO8,
	GPIO9,
	GPIO10,
	GPIO11,
	GPIO12,
	GPIO13,
	GPIO14,
	GPIO15,
	GPIO16,
	GPIO17,
	GPIO18,
	GPIO19,
	GPIO20,
	GPIO21,
	GPIO22,
	GPIO23,
	GPIO24,
	GPIO25,
	GPIO26,
	GPIO27,
	GPIO28,
	GPIO29,
	GPIO30,
	GPIO31,
	GPIO32,
	GPIO33,
	GPIO34,
	GPIO35,
	GPIO36,
	GPIO37,
	GPIO38,
	GPIO39,
	GPIO40,
	GPIO41,
	GPIO42,
	GPIO43,
	GPIO44,
	GPIO45,
	GPIO46,
	GPIO47,
	GPIO48,
	GPIO49,
	GPIO50,
	GPIO51,
	GPIO52,
	GPIO53,
	GPIO54,
	GPIO55,
	GPIO56,
	GPIO57,
	GPIO58,
	GPIO59,
	GPIO60,
	GPIO61,
	GPIO62,
	GPIO63,
	GPIO64,
	GPIO65,
	GPIO66,
	GPIO67,
	GPIO68,
	GPIO69,
	GPIO70,
	GPIO71,
	GPIO72,
	GPIO73,
	GPIO74,
	GPIO75,
	GPIO76,
	GPIO77,
	GPIO78,
	GPIO79,
	GPIO80,
	GPIO81,
	GPIO82,
	GPIO83,
	GPIO84,
	GPIO85,
	GPIO86,
	GPIO87,
	GPIO88,
	GPIO89,
	GPIO90,
	GPIO91,
	GPIO92,
	GPIO93,
	GPIO94,
	GPIO95,
	GPIO96,
	GPIO97,
	GPIO98,
	GPIO99,
	GPIO100,
	GPIO101,
	GPIO102,
	GPIO103,
	GPIO104,
	GPIO105,
	GPIO106,
	GPIO107,
	GPIO108,
	GPIO109,
	GPIO110,
	GPIO111,
	GPIO112,
	GPIO113,
	GPIO114,
	GPIO115,
	GPIO116,
	GPIO117,
	GPIO118,
	GPIO119,
	GPIO120,
	GPIO121,
	GPIO122,
	GPIO123,
	GPIO124,
	GPIO125,
	GPIO126,
	GPIO127,
	GPIO_NUMS,
};

#endif
