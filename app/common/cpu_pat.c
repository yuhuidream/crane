#include "predefines.h"
#include "cpu_pat.h"
#include "utils.h"
#include "timer_dec.h"

#define CPUT_DEFAULT_ADDRESS  0x2000000

unsigned long cpu_test_start_address = CPUT_DEFAULT_ADDRESS;

unsigned long scan(unsigned long data){
    unsigned long i;
    
    for(i=0;i<MEM_MAX_SIZE;i+=4){
        MEM(i)                              = data;
    }
    
    for(i=0;i<MEM_MAX_SIZE;i+=4){
        if(MEM(i)!=data){
            return 1;
        }
    }
    
    return 0;
}

unsigned long cpu_test_101(){
    unsigned long err                       = 0;
    err                                    += scan(0x00000000);
    err                                    += scan(0xffffffff);
    
    return err;
}

unsigned long cpu_test_102(){
    unsigned long err                       = 0;
    err                                    += scan(0xffffffff);
    err                                    += scan(0x00000000);
    
    return err;
}

unsigned long cpu_test_103(){
    unsigned long err                       = 0;
    err                                    += scan(0x55555555);
    err                                    += scan(0xaaaaaaaa);
    
    return err;
}

unsigned long cpu_test_104(){
    unsigned long err                       = 0;
    err                                    += scan(0xaaaaaaaa);
    err                                    += scan(0xffffffff);
    
    return err;
}

unsigned long cpu_test_133(){
    unsigned long rand;
    unsigned long i;
    
    rand                                    = SEED;
    for(i=0;i<MEM_MAX_SIZE;i+=4){
        up_rand(rand);
        MEM(i)                              = rand;
    }
    
    rand                                    = SEED;
    for(i=0;i<MEM_MAX_SIZE;i+=4){
        up_rand(rand);
        if(MEM(i)!=rand){
            return 1;
        }
    }
    
    rand                                    = SEED;
    for(i=0;i<MEM_MAX_SIZE;i+=4){
        up_rand(rand);
        MEM(i)                              = rand;
    }
    
    rand                                    = SEED;
    for(i=0;i<MEM_MAX_SIZE;i+=4){
        up_rand(rand);
        if(MEM(i)!=rand){
            return 1;
        }
    }
    
    return 0;    
}
    
    
unsigned long cpu_test_147(){
    unsigned long i;
    unsigned long j;
    unsigned long pat[16]                   ={
          0xfffe0001,
          0xfffd0002,
          0xfffb0004,
          0xfff70008,
          0xffef0010,
          0xffdf0020,
          0xffbf0040,
          0xff7f0080,
          0xfeff0100,
          0xfdff0200,
          0xfbff0400,
          0xf7ff0800,
          0xefff1000,
          0xdfff2000,
          0xbfff4000,
          0x7fff8000
    };

    for(i=0;i<MEM_MAX_SIZE;i+=4*16){
        for(j=0;j<16;j++){
            MEM(i+(j<<2))                   = pat[j];
        }
    }
    
    for(i=0;i<MEM_MAX_SIZE;i+=4*16){
        for(j=0;j<16;j++){
            if(MEM(i+(j<<2))!= pat[j]){
                return 1;
            }
        }
    }
    
    for(j=0;j<16;j++){
        pat[j]                              =~pat[j];
    }
    
    for(i=0;i<MEM_MAX_SIZE;i+=4*16){
        for(j=0;j<16;j++){
            MEM(i+(j<<2))                   = pat[j];
        }
    }
    
    for(i=0;i<MEM_MAX_SIZE;i+=4*16){
        for(j=0;j<16;j++){
            if(MEM(i+(j<<2))!= pat[j]){
                return 1;
            }
        }
    }

    return 0;
}

void show_test_result(unsigned long rst,unsigned long idx, unsigned long start)
{
   unsigned long end = bbu_get_msec();
   
   bbu_printf("cpu test %d use time(ms): %d\n\r", idx, end - start);

   if ( rst != 0)
   {                               
      bbu_printf("fail\n\r");
   }
   else
   {
      bbu_printf("ok\n\r");
   }
}

unsigned long show_message(void)
{
   bbu_printf("Testing..........\n\r");
   return bbu_get_msec();
}

unsigned long cpu_test(int argc, unsigned char** argv){
    unsigned long err                       = 0;
    unsigned long rst, start;
    
    cpu_test_start_address = CPUT_DEFAULT_ADDRESS;
    if(((argc > 1)&&check_help(argv[1])) || argc > 2)
    {
       bbu_printf("\n\rUsage: \n\r %s [<start_address>]\n\r", argv[0]);
       bbu_printf(" <start_address> default value is 0x%08X\n\r",cpu_test_start_address);
       return err;
    }
    else if(argc == 2)
       cpu_test_start_address = conv_hex((char *)argv[1]);
       
    bbu_printf("\n\rCPU test start at 0x%08X\n\r", cpu_test_start_address);
    start = show_message();
    rst                                     = cpu_test_101();
    //show_message(PAT_END   | 101);
    show_test_result(rst, 101, start);

    start = show_message();
    rst                                     = cpu_test_102();
    //show_message(PAT_END   | 102);
    show_test_result(rst, 102, start);

    start = show_message();
    rst                                     = cpu_test_103();
    //show_message(PAT_END   | 103);
    show_test_result(rst, 103, start);
     
    
    start = show_message();
    rst                                     = cpu_test_104();
    //show_message(PAT_END   | 104);
    show_test_result(rst, 104, start);
    
    
    start = show_message();
    rst                                     = cpu_test_133();
    //show_message(PAT_END   | 133);
    show_test_result(rst, 133, start);
    
    
    start = show_message();
    rst                                     = cpu_test_147();
    //show_message(PAT_END   | 147);      
    show_test_result(rst, 147, start);
    
    return err;
}
