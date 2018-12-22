/*******************************************************************************
 Copyright 2001, 2002 Georges Menie (<URL snipped>)
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/*******************************************************************************
 putchar is the only external dependency for this file, 
 if you have a working putchar, just remove the following
 define. If the function should be called something else,
 replace it by your own function call.
*/
//*******************************************************************************
//  Updated by Daniel D Miller.  Changes to the original Menie code are
//  Copyright 2009-2010 Daniel D Miller
//  All such changes are distributed under the same license as the original,
//  as described above.
//  11/06/09 11:42 - adding floating-point support
//  03/19/10 19:06 - pad fractional portion of floating-point number with 0s
//  03/30/10 14:57 - document the \% bug
//  07/20/10 12:37 - Fix a round-off bug in floating-point conversions
//                   ( 0.99 with %.1f did not round to 1.0 )
//*******************************************************************************
//  BUGS
//  If '%' is included in a format string, in the form \% with the intent
//  of including the percent sign in the output string, this function will
//  mis-handle the data entirely!!  
//  Essentially, it will just discard the character following the percent sign.  
//  This bug is not easy to fix in the existing code; 
//  for now, I'll just try to remember to use %% instead of \% ...
//*******************************************************************************

//#define  TEST_PRINTF    1

#ifdef CONFIG_CA7
#undef  FLOAT_SUPPORT
#else
#define  FLOAT_SUPPORT  1
#endif
#ifdef TEST_PRINTF
#include <stdio.h>
//#undef  USE_INTERNALS
#define  USE_INTERNALS
#else
//  USE_INTERNALS implements code for company-specific linkages
//  at the company that I currently work for.
//  Other users will *not* want this defined!!
#define  USE_INTERNALS
#endif

#ifdef   USE_INTERNALS
#include "predefines.h"
#include "UART.h"
#include "bbu.h"
#include "bsp.h"
#include <string.h>
#include "print.h"
#endif
#include <stdarg.h>
#include "uart_dec.h"

typedef int (*put_char_t)(int);


int uart_put_char(int u8c)
{
    while(!(DBG_UART->LSR & LSR_TEMT)); 
    DBG_UART->THR = u8c;
}

#ifdef CONFIG_MMT
extern int usb_debug_put_char(int u8c);

int usb_put_char(int u8c)
{
    uart_put_char(u8c);
    return usb_debug_put_char(u8c);
}
#endif

put_char_t __put_char = uart_put_char;

//#if defined(TEST_PRINTF) || defined(CONFIG_DBG)
int put_char(int u8c)
{
    __put_char(u8c);
     return (u8c);
}
//#endif

//****************************************************************************
static void printchar (char **str, int c)
{
	if (str) {
		**str = c;
		++(*str);
	}
//#if defined(TEST_PRINTF) || defined(CONFIG_DBG)
   else {
     // extern int putchar (int c);
		(void) put_char (c);
   }
//#endif
}

//****************************************************************************
static uint32_t my_strlen(char *str)
{
   if (str == 0)
      return 0;
   uint32_t slen = 0 ;
   while (*str != 0) {
      slen++ ;
      str++ ;
   }
   return slen;
}

//****************************************************************************
//  This version returns the length of the output string.
//  It is more useful when implementing a walking-string function.
//****************************************************************************
#ifdef FLOAT_SUPPORT
static double round_nums[8] = {
   0.5,
   0.05,
   0.005,
   0.0005,
   0.00005,
   0.000005,
   0.0000005,
   0.00000005
} ;

static unsigned dbl2stri(char *outbfr, double dbl, unsigned dec_digits)
{
   static char local_bfr[81] ;
   char *output = (outbfr == 0) ? local_bfr : outbfr ;

   //*******************************************
   //  extract negative info
   //*******************************************
   if (dbl < 0.0) {
      *output++ = '-' ;
      dbl *= -1.0 ;
   }
   //  handling rounding by adding .5LSB to the floating-point data
   if (dec_digits < 8) {
      dbl += round_nums[dec_digits] ;
   }
//   if(dec_digits > 9)
//      dec_digits = 9;

   //**************************************************************************
   //  construct fractional multiplier for specified number of digits.
   //**************************************************************************
   uint64_t mult = 1 ;
   uint32_t idx ;
   for (idx=0; idx < dec_digits; idx++)
      mult *= 10 ;

   // printf("mult=%u\n", mult) ;
   uint64_t wholeNum = (uint64_t) dbl ;
   uint64_t decimalNum = (uint64_t) ((dbl - wholeNum) * mult);

   //*******************************************
   //  convert integer portion
   //*******************************************
   char tbfr[40] ;
   idx = 0 ;
   while (wholeNum != 0) {
      tbfr[idx++] = '0' + (wholeNum % 10) ;
      wholeNum /= 10 ;
   }
   // printf("%.3f: whole=%s, dec=%d\n", dbl, tbfr, decimalNum) ;
   if (idx == 0) {
      *output++ = '0' ;
   } else {
      while (idx > 0) {
         *output++ = tbfr[idx-1] ;  //lint !e771
         idx-- ;
      }
   }
   if (dec_digits > 0) {
      *output++ = '.' ;

      //*******************************************
      //  convert fractional portion
      //*******************************************
      idx = 0 ;
      while (decimalNum != 0) {
         tbfr[idx++] = '0' + (decimalNum % 10) ;
         decimalNum /= 10 ;
      }
      //  pad the decimal portion with 0s as necessary;
      //  We wouldn't want to report 3.093 as 3.93, would we??
      while (idx < dec_digits) {
         tbfr[idx++] = '0' ;
      }
      // printf("decimal=%s\n", tbfr) ;
      if (idx == 0) {
         *output++ = '0' ;
      } else {
         while (idx > 0) {
            *output++ = tbfr[idx-1] ;
            idx-- ;
         }
      }
   }
   *output = 0 ;

   //  prepare output
   output = (outbfr == 0) ? local_bfr : outbfr ;
   return my_strlen(output) ;
}

static unsigned exp2stri(char *outbfr, double dbl, unsigned dec_digits)
{
   static char local_bfr[81] ;
   char *output = (outbfr == 0) ? local_bfr : outbfr ;
   int exp = 0; 
   int idx = 0;
   int pc = 0;

   //*******************************************
   //  extract negative info
   //*******************************************
   if (dbl < 0.0) {
      *output++ = '-' ;
      dbl *= -1.0 ;
   }
   if(((uint64_t)dbl & 0x7FF0000000000000)==0x7FF0000000000000)
   {
      	strcpy(output,"infinity");
      	output+=8;
   }
   else{
	   if(dbl != 0){
	      while(dbl > 10)
	      {
	         dbl /= 10;
	         exp++;
	      }
	      while(dbl < 1)
	      {
	         dbl *= 10;
	         exp--;
	      }
	   }
	   pc = dbl2stri(output, dbl, dec_digits);
	   output += pc;
	   *output++ = 'e';
	   if(exp < 0){
	      *output++ = '-' ;
	      exp *= -1;
	   }
	   char tbfr[4] ;
	   while (exp != 0) {
	      tbfr[idx++] = '0' + (exp % 10) ;
	      exp /= 10 ;
	   }
	   if (idx == 0) {
	      *output++ = '0' ;
	   } else {
	      while (idx > 0) {
	         *output++ = tbfr[idx-1] ;  //lint !e771
	         idx-- ;
	      }
	   }
   }
   *output = 0 ;
   //  prepare output
   output = (outbfr == 0) ? local_bfr : outbfr ;
   return my_strlen(output) ;
}
#endif

//****************************************************************************
#define  PAD_RIGHT   1
#define  PAD_ZERO    2

static int prints (char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';
	if (width > 0) {
      int len = 0;
      const char *ptr;
		for (ptr = string; *ptr; ++ptr)

			++len;
		if (len >= width)
			width = 0;
		else
			width -= len;
		if (pad & PAD_ZERO)
			padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for (; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for (; *string; ++string) {
		printchar (out, *string);
		++pc;
	}
	for (; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}
	return pc;
}

//****************************************************************************
/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12
static int printi (char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
   char *s;
   int t, neg = 0, pc = 0;
   unsigned u = (unsigned) i;
	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}
	if (sg && b == 10 && i < 0) {
		neg = 1;
      u = (unsigned) -i;
	}
	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';
	while (u) {
      t = u % b;  //lint !e573  Warning 573: Signed-unsigned mix with divide
		if (t >= 10)
			t += letbase - '0' - 10;
		*--s = t + '0';
      u /= b;  //lint !e573  Warning 573: Signed-unsigned mix with divide
	}
	if (neg) {
		if (width && (pad & PAD_ZERO)) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}
	return pc + prints (out, s, width, pad);
}

//****************************************************************************
/* the argument 'align' of "print" should equal to the number of 32-bit 
   length arguments before "const char *format..." of the mother functions
   which calls "print" 
   for example: 
    1, in "bbu_printf(const char *format, ...)" which calls "print", align = 0
    2, in "bbu_sprintf (char *out, const char *format, ...)",  align = 1
*/
int print (char **out, int *varg, int align)
{
   int post_decimal ;
   int width, pad ;
   unsigned dec_width = 0 ;
   int pc = 0;
   int varg_bk = (int)(varg - align);
   char *format = (char *) (*varg++);
	char scr[2];
	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0')
				break;
			if (*format == '%')
				goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
         post_decimal = 0 ;
         if (*format == '.'  ||
            (*format >= '0' &&  *format <= '9') || *format == '*') {

            while (1) {
               if (*format == '.') {
                  post_decimal = 1 ;
                  dec_width = 0 ;
                  format++ ;
               } else if ((*format >= '0' &&  *format <= '9')) {
                  if (post_decimal) {
                     dec_width *= 10;
                     dec_width += *format - '0';
                  } else {
                     width *= 10;
                     width += *format - '0';
                  }
                  format++ ;
               }
               else if(*format == '*'){
               		if(post_decimal){
               			dec_width = (int)(*varg++);
               		}
               		else
               			width = (int)(*varg++);
               		format++ ;
               }
               else {
                  break;
               }
            }
         }
         if (*format == 'l' || *format == 'L')
            ++format;
         switch (*format) {
         case 's':
            {
            char *s = *((char **) varg++);   //lint !e740
            // printf("[%s] w=%u\n", s, width) ;
            pc += prints (out, s ? s : "(null)", width, pad);
            }
            break;
         case 'd':
         case 'i':
            pc += printi (out, *varg++, 10, 1, width, pad, 'a');
            break;
         case 'x':
            pc += printi (out, *varg++, 16, 0, width, pad, 'a');
            break;
         case 'X':
            pc += printi (out, *varg++, 16, 0, width, pad, 'A');
            break;
         case 'u':
            pc += printi (out, *varg++, 10, 0, width, pad, 'a');
            break;
         case 'c':
            /* char are converted to int then pushed on the stack */
            scr[0] = *varg++;
            scr[1] = '\0';
            pc += prints (out, scr, width, pad);
            break;
#ifdef FLOAT_SUPPORT
         case 'f':
            {
            /*
             * according to "Procedure Call Standard for the ARM Architecture"
             *(1)For variadic functions, float arguments that match the ellipsis (…) are converted to type double.
             *(2)The relative address between the float with the varg_bk should be 8-bytes align
             */
            double *dblptr = (double *)((((int)(varg) +7 -varg_bk) & (~7)) + varg_bk);
//            double *dblptr = (double *) varg ;  //lint !e740 !e826  convert to double pointer
            double dbl = *dblptr++ ;   //  increment double pointer
            varg = (int *) dblptr ;    //lint !e740  copy updated pointer back to base pointer
            char bfr[81] ;
            // unsigned slen =
            dbl2stri(bfr, dbl, dec_width) ;
            // stuff_talkf("[%s], width=%u, dec_width=%u\n", bfr, width, dec_width) ;
            pc += prints (out, bfr, width, pad);
            }
            break;
         case 'e':
         case 'g':
            {
            /*
             * according to "Procedure Call Standard for the ARM Architecture"
             *(1)For variadic functions, float arguments that match the ellipsis (…) are converted to type double.
             *(2)The relative address between the float with the varg_bk should be 8-bytes align
             */
            double *dblptr = (double *)((((int)(varg) +7 -varg_bk) & (~7)) + varg_bk);
//            double *dblptr = (double *) varg ;  //lint !e740 !e826  convert to double pointer
            double dbl = *dblptr++ ;   //  increment double pointer
            varg = (int *) dblptr ;    //lint !e740  copy updated pointer back to base pointer
            char bfr[81] ;
            // unsigned slen =
            exp2stri(bfr, dbl, dec_width) ;
            // stuff_talkf("[%s], width=%u, dec_width=%u\n", bfr, width, dec_width) ;
            pc += prints (out, bfr, width, pad);
            }
            break;
#endif

         default:
            printchar (out, '%');
            printchar (out, *format);
            break;
         }
		}
		else {
		 out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out)
		**out = '\0';
	return pc;
}

//****************************************************************************
//  assuming sizeof(void *) == sizeof(int)
//  This function is not valid in embedded projects which don't
//  have a meaningful stdout device.
//****************************************************************************
#ifdef TEST_PRINTF
int termf (const char *format, ...)
{
   int *varg = (int *) (char *) (&format);
   return print (0, varg, 0);
}  //lint !e715
#endif



//********************************************************************
//  On Windows platform, try to redefine printf/fprintf
//  so we can output code to a debug window.
//  Also, shadow syslog() within OutputDebugStringA()
//  Note: printf() remapping was unreliable,
//  but syslog worked great.
//********************************************************************
#ifdef   USE_INTERNALS

static size_t log_prefix(const char *p, unsigned int *level)
{
	unsigned int lev;
	size_t len;

	if (p[0] != '<' || !p[1])
		return 0;
	if (p[2] == '>') {
		/* usual single digit level number or special char */
		switch (p[1]) {
		case '0' ... '7':
			lev = p[1] - '0';
			break;
		default:
			return 0;
		}
		len = 3;
	} else {
		return 0;
	}

	if (level)
		*level = lev;
	return len;
}

int bbu_printf(const char *format, ...)
{
 /* This function can only print MAX_LINE_LEN length line each time.
  * For large lines, need to divide it into serveral lines by the user. 
  * The alternative ways are tricky and not safe. 
  * Need to define DBG_PRINT(switch,str) to switch on/off the debug messages
  */
   char *out;
   int size = 0;
//#ifndef CONFIG_NANO_BL
   int *varg = (int *) (char *) (&format);

   /* check the print level and decide to print or not */
   int current_print_level = CRIT_LEVEL;
   size_t len;
   char *str = (char *) (*varg);
   len = log_prefix(str, &current_print_level);
   str += len;
   *varg = str;
   if(current_print_level >= 0){
        size = print (0, varg, 0);
   }
//#endif
   return size;

}

int bbu_putchar(char u8c)
{

   put_char(u8c);
   return 0;
}

int bbu_putstr(const char *str, uint8_t size)
{
      while(size--)
      {
         put_char(*str);
         ++str;
      }
   return 0;
}




#endif

#ifdef CONFIG_MMT

#define OUTPUT_INFO  0xd4282ce8
int bbu_output = 0; //0 UART, 1 USB.

//This function should be called before init UART and USB serial.
int bbu_output_init1(void)
{
    uint32_t output_info = BU_REG_READ(0xd4282ce8);
    uint32_t buadrate = 0;
    bbu_output = (output_info & 0xFF) >> 4;
    buadrate = (output_info & 0xFF);

    if(bbu_output == 0){
        __put_char = uart_put_char;
    }
    else{
        __put_char = usb_put_char;
        usb_close_service();
    }
    switch(buadrate)
    {
        case 1:
            uart_baudrate = BAUDRATE_9600;
            break;
        case 2:
            uart_baudrate = BAUDRATE_14400;
            break;
        case 3:
            uart_baudrate = BAUDRATE_38400;
            break;
        case 4:
            uart_baudrate = BAUDRATE_57600;
            break;
        case 5:
            uart_baudrate = BAUDRATE_115200;
            break;
        case 6:
            uart_baudrate = BAUDRATE_921600;
            break;
        case 7:
            uart_baudrate = BAUDRATE_1842000;
            break;
        case 8:
            uart_baudrate = BAUDRATE_3686400;
            break;
        default:
            uart_baudrate = BAUDRATE_115200;
            break;
    }
}


void bbu_output_init2(void)
{
    if(bbu_output == 1)
    {
        uint8_t  flag=0;
        usb_serial_init(1);

        while(!flag)
        {
            flag=readUsbInitFlag();
            udelay(1);
        }
        //bbu_printf("USB serial is chosen as output\n\r");
    }else{

        //bbu_printf("UART serial is chosen as output\n\r");
    }
}

int output_is_usb(void)
{
    return (bbu_output == 1);
}

int output_is_uart(void)
{
    return (bbu_output == 0);
}
#endif

//****************************************************************************




