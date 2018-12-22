#include "predefines.h"
#include "print.h"
#include "clock.h"
#include "bbu_malloc.h"
#include <string.h>
#include "smp.h"
#include "bbu.h"

static int DEF_MP_VAR(m)={DEF_CORE_ARRAY(167772161)};
static int DEF_MP_VAR(N)={DEF_CORE_ARRAY(1)};
static int DEF_MP_VAR(a);
static int *DEF_MP_VAR(p);
static int DEF_MP_VAR(i);
static int DEF_MP_VAR(e)={DEF_CORE_ARRAY(34893349)};
static int DEF_MP_VAR(s);
static int DEF_MP_VAR(c);
static int DEF_MP_VAR(U)={DEF_CORE_ARRAY(1)};

static int *DEF_MP_VAR(t);  //t[1<<25]={2},

/* ........................................................................ */
static void g(int d, int h)
{
   DEF_GET_COREID
   for(MP_VAR(i)=MP_VAR(s);MP_VAR(i)<1<<24;MP_VAR(i)*=2)
     d=d*1LL*d%MP_VAR(m);
   for(MP_VAR(p)=MP_VAR(t);MP_VAR(p)<MP_VAR(t)+MP_VAR(N);MP_VAR(p)+=MP_VAR(s))
      for(MP_VAR(i)=MP_VAR(s),MP_VAR(c)=1;MP_VAR(i);MP_VAR(i)--)
         MP_VAR(a)=MP_VAR(p)[MP_VAR(s)]*(h?MP_VAR(c):1LL)%MP_VAR(m),MP_VAR(p)[MP_VAR(s)]=(MP_VAR(m)+*MP_VAR(p)-MP_VAR(a))*(h?1LL:MP_VAR(c))%MP_VAR(m),MP_VAR(a)+=*MP_VAR(p), \
         *MP_VAR(p)++=MP_VAR(a)%MP_VAR(m),MP_VAR(c)=MP_VAR(c)*1LL*d%MP_VAR(m);
}

/* ........................................................................ */
void calc_print_biggest_prime(void)
{
   bbu_printf("\n\r*************** Start to calc the biggest "
              "Mersenne prime ***************\n\r");
   bbu_printf("CPU%d is runing CPBP Test\n\r", smp_processor_id());
   DEF_GET_COREID
   print_current_time();
   MP_VAR(t) = (int *)bbu_malloc(1<<25);
   memset((void *)MP_VAR(t), 0, sizeof(int)*(1<<25));
   MP_VAR(t)[0] = 2;
   MP_VAR(N)=1;
   MP_VAR(e)=34893349;
   MP_VAR(m)=167772161;
   MP_VAR(U)=1;
   while(MP_VAR(e)/=2)
   {
      MP_VAR(N)*=2;
      MP_VAR(U)=MP_VAR(U)*1LL*(MP_VAR(m)+1)/2%MP_VAR(m);
      for(MP_VAR(s)=MP_VAR(N);MP_VAR(s)/=2;)
         g(17,0);
      for(MP_VAR(p)=MP_VAR(t);MP_VAR(p)<MP_VAR(t)+MP_VAR(N);MP_VAR(p)++)
         *MP_VAR(p)=*MP_VAR(p)*1LL**MP_VAR(p)%MP_VAR(m)*MP_VAR(U)%MP_VAR(m);
      for(MP_VAR(s)=1;MP_VAR(s)<MP_VAR(N);MP_VAR(s)*=2)
         g(29606852,1);
      for(MP_VAR(a)=0,MP_VAR(p)=MP_VAR(t);MP_VAR(p)<MP_VAR(t)+MP_VAR(N);)
         MP_VAR(a)+=*MP_VAR(p)<<(MP_VAR(e)&1),*MP_VAR(p)++=MP_VAR(a)%10,MP_VAR(a)/=10;
      bbu_putchar('.');
   }
   while(!*--MP_VAR(p));
   bbu_printf("\n\r\n\r The calculating process is done\n\r\n\r");
   bbu_printf("\n\r************* Start to print the biggest "
             "Mersenne prime **************\n\r");
   print_current_time();
#if 0    /* remove the print process, it's meaningless */
   for(t[0]--;p>=t;)
      bbu_putchar(48+*p--);
   bbu_printf("\n\r\n\r The printing process is done\n\r\n\r");
   print_current_time();
#endif
   bbu_mfree((void *)MP_VAR(t));
}
