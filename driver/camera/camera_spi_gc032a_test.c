#include "i2c_dec.h"
#include "gpio_dec.h"
#include  "camera_spi_gc032a_test.h"

#define PIN_OUTPUT 0
void gpio_pin_direction(int gpio, int type)
{
	//TODO  sunao

}

void gpio_set_value(int gpio, int value)
{

}

int gc030a_sensor_read_id(){

	char id1, id2;
	enum I2C_RETURNS ret1, ret2;
       bbu_printf("gc030a_sensor_read_id: E!!!\n");

        gpio_init();

       gpio_output_low(GPIO22);  

	//i2c read id
	i2c_init(HS1_I2C) ;

	ret1 = i2c_read_addr8_byte(HS1_I2C,  0x42 , 0xf0 , &id1,  ICR_MODE_STD) ;
       bbu_printf("gc030a_sensor_read_id: ret1 = %d, id1 = 0x%x\n", ret1, id1);

	ret2 = i2c_read_addr8_byte(HS1_I2C,  0x42, 0xf1 , &id2,  ICR_MODE_STD) ;
       bbu_printf("gc030a_sensor_read_id: ret2 = %d, id2 = 0x%x\n", ret2, id2);

	if(ret1 != I2C_OK || ret2 != I2C_OK){
		bbu_printf("I2C error!\n");
		return -1;
	}
	if(id1 != 0x23 || id2 != 0x2a){
			bbu_printf("sensor read id err [%d, %d]!", id1, id2);
			return -1;
	}

	bbu_printf("read id success!\n");


	return 0;

}

int gc030a_sensor_init(){
	int i;
	enum I2C_RETURNS ret;
        char id1, id2;

   gc030a_sensor_read_id();

   bbu_printf("gc030a_sensor_init \n");

#if 0
        ret =  i2c_write_addr8_byte(HS1_I2C, 0x42, 0xf3, 0x83, ICR_MODE_STD);
        if(ret != I2C_OK){
            bbu_printf("I2C error! write 0xf3, 0x83\n");
            return -1;
        }
        bbu_printf("gc030a_sensor_init: i2c_write_addr8_byte  0xf3, 0x83\n");

        ret =  i2c_write_addr8_byte(HS1_I2C, 0x42, 0xf5, 0x0c, ICR_MODE_STD);
        if(ret != I2C_OK){
            bbu_printf("I2C error! write 0xf5, 0x0c\n");
            return -1;
        }
        bbu_printf("gc030a_sensor_init: i2c_write_addr8_byte  0xf5, 0x0c\n");


        ret = i2c_read_addr8_byte(HS1_I2C,  0x42 , 0xf3 , &id1,  ICR_MODE_STD) ;
        if(ret != I2C_OK){
            bbu_printf("I2C error!  read  0xf3\n");
            return -1;
        }
        bbu_printf("gc030a_sensor_init: ret = %d,  0xf3   id1 = 0x%x\n", ret, id1);

        ret = i2c_read_addr8_byte(HS1_I2C,  0x42 , 0xf5 , &id1,  ICR_MODE_STD) ;
        if(ret != I2C_OK){
            bbu_printf("I2C error!  read  0xf5\n");
            return -1;
        }
        bbu_printf("gc030a_sensor_init: ret = %d,   0xf5  id1 = 0x%x\n", ret, id1);

#endif

	for(i = 0; i< ARRAY_SIZE(gc032a_init_code); i++){
		ret =  i2c_write_addr8_byte(HS1_I2C, 0x42, gc032a_init_code[i].reg, gc032a_init_code[i].val, ICR_MODE_STD);
		if(ret != I2C_OK){
			bbu_printf("----------I2C error! [%d]\n", i);
			return -1;
		}
	}

	return 0;
}


int gc030a_stream_on()
{
	enum I2C_RETURNS ret;

	ret =  i2c_write_addr8_byte(HS1_I2C, 0x42, 0xfe, 0x03, ICR_MODE_STD);
	if(ret != I2C_OK){
		bbu_printf("gc030a_stream_on: I2C error! write 0xfe, 0x03  ret = %d\n", ret);
		return -1;
	}

	ret =  i2c_write_addr8_byte(HS1_I2C, 0x42, 0x51, 0x01, ICR_MODE_STD);
	if(ret != I2C_OK){
		bbu_printf("gc030a_stream_on: I2C error! write 0x51, 0x01  ret = %d\n", ret);
		return -1;
	}


}

int gc030a_stream_off()
{
	enum I2C_RETURNS ret;

	ret =  i2c_write_addr8_byte(HS1_I2C, 0x42, 0xfe, 0x03, ICR_MODE_STD);
	if(ret != I2C_OK){
		bbu_printf("gc030a_stream_off: I2C error! write 0xfe, 0x03  ret = %d\n", ret);
		return -1;
	}

	ret =  i2c_write_addr8_byte(HS1_I2C, 0x42, 0x51, 0x00, ICR_MODE_STD);
	if(ret != I2C_OK){
		bbu_printf("gc030a_stream_off: I2C error! write 0x51, 0x00  ret = %d\n", ret);
		return -1;
	}

}


