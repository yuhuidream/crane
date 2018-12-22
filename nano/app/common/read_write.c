
//#define PURE_RD_WR
#ifdef PURE_RD_WR
#include "predefines.h"
#include <string.h>
#include "bbu.h"
#include "pmic.h"
#include "timer_dec.h"
#include "CIU.h"
#include "common_tests.h"
#include "ppset.h"

static unsigned int data_pattern[4][2] = 
{
	{0xFFFFFFFF,0x00000000},
	{0x7FFF7FFF,0x80008000},
	{0x7F7F7F7F,0x80808080},
	{0xEFEFEFEF,0x10101010},
};

void bbu_mtest_func(int argc, uint8_t **argv)
{
	unsigned int start;
	unsigned int len,sublen, first_data, second_data;
	unsigned int rfirst_data, rsecond_data;
	int i,j;
    	switch(argc)
	{
	  case 4:
		start =conv_hex((char *)argv[1]);
		len =conv_hex((char *)argv[2]);
		len = 0xFFFFFFF0&len;
		sublen = len/4;
		
		if(strcmp((char *)argv[3],"w") == 0) {	
			bbu_printf("DDR write test at: [0x%X, 0x%X]\n\r",start,start+len);
			first_data = data_pattern[0][0];
			second_data = data_pattern[0][1];
			bbu_printf("write sub buff [0x%x,0x%x]\n\r",start,start+sublen);
			for(j = 0; j < 100; j++)
			{
				for(i=0; i<sublen;i+=8)
				{	
					*((unsigned int *)(start+i)) = first_data;
					*((unsigned int *)(start+i+4)) = second_data;
				}
				bbu_printf("write 0xffffffff/0x00000000 LOOP %d finished ...\n\r",j);
			}
			first_data = data_pattern[1][0];
			second_data = data_pattern[1][1];
			start = start+sublen;
			bbu_printf("write sub buff [0x%x,0x%x]\n\r",start,start+sublen);
			for(j = 0; j < 100; j++)
			{
				for(i=0; i<sublen;i+=8)
				{	
					*((unsigned int *)(start+i)) = first_data;
					*((unsigned int *)(start+i+4)) = second_data;
				}
				bbu_printf("write 0x7FFF7FFF,0x80008000 LOOP %d finished ...\n\r",j);
			}
			first_data = data_pattern[2][0];
			second_data = data_pattern[2][1];
			start = start+sublen;
			bbu_printf("write sub buff [0x%x,0x%x]\n\r",start,start+sublen);
			for(j = 0; j < 100; j++)
			{
				for(i=0; i<sublen;i+=8)
				{	
					*((unsigned int *)(start+i)) = first_data;
					*((unsigned int *)(start+i+4)) = second_data;
				}
				bbu_printf("write 0x7F7F7F7F/0x80808080 LOOP %d finished ...\n\r",j);
			}
			first_data = data_pattern[3][0];
			second_data = data_pattern[3][1];
			start = start +sublen;
			bbu_printf("write sub buff [0x%x,0x%x]\n\r",start,start+sublen);
			for(j = 0; j < 100; j++)
			{
				for(i=0; i<sublen;i+=8)
				{	
					*((unsigned int *)(start+i)) = first_data;
					*((unsigned int *)(start+i+4)) = second_data;
				}
				bbu_printf("write 0xEFEFEFEF/0x10101010 LOOP %d finished ...\n\r",j);
			}
		} else if(strcmp((char *)argv[3],"r") == 0) {
			bbu_printf("DDR read test at: [0x%X, 0x%X]\n\r",start,start+len);
			first_data = data_pattern[0][0];
			second_data = data_pattern[0][1];
			bbu_printf("read sub buff [0x%x,0x%x]\n\r",start,start+sublen);
			for(j = 0; j < 100; j++)
			{
				for(i=0; i<sublen;i+=8)
				{	
					rfirst_data = *((unsigned int *)(start+i));
					rsecond_data = *((unsigned int *)(start+i+4));

					if((j == 99) &&(rfirst_data != first_data || rsecond_data != second_data))
					{
						bbu_printf("write data != read data\n\r");
						bbu_printf("write: 0x%x: [0x%x]\n\r",start+i,first_data);
						bbu_printf("       0x%x: [0x%x]\n\r",start+i+4,second_data);
						bbu_printf(" read: 0x%x: [0x%x]\n\r",start+i,rfirst_data);
						bbu_printf("       0x%x: [0x%x]\n\r",start+i+4,rsecond_data);
						break;
					}					
				}
				bbu_printf("read 0xffffffff/0x00000000 LOOP %d finished ...\n\r",j);
			}

			first_data = data_pattern[1][0];
			second_data = data_pattern[1][1];
			start = start + sublen;
			bbu_printf("read sub buff [0x%x,0x%x]\n\r",start,start+sublen);
			for(j = 0; j < 100; j++)
			{
				for(i=0; i<sublen;i+=8)
				{	
					rfirst_data = *((unsigned int *)(start+i));
					rsecond_data = *((unsigned int *)(start+i+4));

					if((j == 99) &&(rfirst_data != first_data || rsecond_data != second_data))
					{
						bbu_printf("write data != read data\n\r");
						bbu_printf("write: 0x%x: [0x%x]\n\r",start+i,first_data);
						bbu_printf("       0x%x: [0x%x]\n\r",start+i+4,second_data);
						bbu_printf(" read: 0x%x: [0x%x]\n\r",start+i,rfirst_data);
						bbu_printf("       0x%x: [0x%x]\n\r",start+i+4,rsecond_data);
						break;
					}	
				}
				bbu_printf("read 0x7FFF7FFF,0x80008000 LOOP %d finished ...\n\r",j);
			}
			first_data = data_pattern[2][0];
			second_data = data_pattern[2][1];
			start = start + sublen;
			bbu_printf("read sub buff [0x%x,0x%x]\n\r",start,start+sublen);
			for(j = 0; j < 100; j++)
			{
				for(i=0; i<sublen;i+=8)
				{	
					rfirst_data = *((unsigned int *)(start+i));
					rsecond_data = *((unsigned int *)(start+i+4));

					if((j == 99) &&(rfirst_data != first_data || rsecond_data != second_data))
					{
						bbu_printf("write data != read data\n\r");
						bbu_printf("write: 0x%x: [0x%x]\n\r",start+i,first_data);
						bbu_printf("       0x%x: [0x%x]\n\r",start+i+4,second_data);
						bbu_printf(" read: 0x%x: [0x%x]\n\r",start+i,rfirst_data);
						bbu_printf("       0x%x: [0x%x]\n\r",start+i+4,rsecond_data);
						break;
					}	
				}
				bbu_printf("read 0x7F7F7F7F/0x80808080 LOOP %d finished ...\n\r",j);
			}
			first_data = data_pattern[3][0];
			second_data = data_pattern[3][1];
			start = start + sublen;
			bbu_printf("read sub buff [0x%x,0x%x]\n\r",start,start+sublen);
			for(j = 0; j < 100; j++)
			{
				for(i=0; i<sublen;i+=8)
				{	
					rfirst_data = *((unsigned int *)(start+i));
					rsecond_data = *((unsigned int *)(start+i+4));

					if((j == 99) &&(rfirst_data != first_data || rsecond_data != second_data))
					{
						bbu_printf("write data != read data\n\r");
						bbu_printf("write: 0x%x: [0x%x]\n\r",start+i,first_data);
						bbu_printf("       0x%x: [0x%x]\n\r",start+i+4,second_data);
						bbu_printf(" read: 0x%x: [0x%x]\n\r",start+i,rfirst_data);
						bbu_printf("       0x%x: [0x%x]\n\r",start+i+4,rsecond_data);
						break;
					}	
				}
				bbu_printf("read 0xEFEFEFEF/0x10101010 LOOP %d finished ...\n\r",j);
			}
		}
            	//bbu_ddr_sweep_help();
            break;
	  default:
		bbu_printf("error cmd\n");
       	}
}
#else
void bbu_mtest_func(int argc, char **argv)
{
	return;
}
#endif
