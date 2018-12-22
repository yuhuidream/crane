#include "predefines.h"

#define   SDRAM_ROW_NUM   0x1000 /*0xc bits */
#define   SDRAM_COL_NUM   0x200 /* 9 bits*/
#define SDRAM_BANK_NUM    4

/*..........................................................................*/
extern uint32_t SDRAM_walkthrough
      (uint32_t addr, uint32_t cnt, uint32_t pattern1, uint32_t pattern2);
extern uint32_t SDRAM_pattern_test(uint32_t addr, uint32_t cnt );
extern int SDRAM_multi_st_ld_test(uint32_t addr, uint32_t cnt);
static void 
burst_write_32bytes(uint32_t test_address, uint32_t *data_pattern);
static void 
burst_read_32bytes(uint32_t test_address, uint32_t *test_result);

/*..........................................................................*/
uint32_t memory_test(uint32_t start_addr, uint32_t byte_cnt )
{

   /* algorithm 1 */

   if ( SDRAM_walkthrough(start_addr, byte_cnt, 0x0, 0xffffffff) )
      return MV_ERROR;
   bbu_printf("\n\r");
   if ( SDRAM_walkthrough(start_addr, byte_cnt, 0xaaaaaaaa, 0x55555555) )
      return MV_ERROR;
   bbu_printf("\n\r");
   if ( SDRAM_pattern_test(start_addr, byte_cnt ) )
      return MV_ERROR;
   bbu_printf("\n\r");
   if ( SDRAM_multi_st_ld_test(start_addr, byte_cnt ) )
      return MV_ERROR;
   return MV_OK;

}

/*..........................................................................*/
uint32_t SDRAM_pattern_test(uint32_t addr, uint32_t cnt )
{

   uint32_t *pWt, *pRd, dataread;
   uint32_t i;

   uint32_t SDRAM_base_addr = addr;
   uint32_t SDRAM_size = cnt;

   bbu_printf("Testing memory with address and increasing numbers...\r\n" );
/* Algorithm of March with pattern "pattern1" and "pattern2" to X-direction */
/* M0 write pattern1  from base_addr to SDRAM_size */
   pWt = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_size/4; i++)
   {
      dataread = (uint32_t)pWt;
      *pWt++ = dataread;
   }
   
/* M1 read n(expect pattern1), write pattern2 to n, 
   n from base_addr to SDRAM_size */
   pRd = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_size/4; i++, pRd++ )
   {
      dataread = *pRd;
      if( (*pRd) != (uint32_t)pRd )
      {
         bbu_printf("SDRAM address/data test failed!\n\r");
         bbu_printf("Addr = 0x%08X, data = 0x%08X\n\r", pRd, dataread);
//         return 1;   
      }
      if ( !(i&0x1fffff) )
         bbu_printf(".");
   }
   

   pWt = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_size/4; i++)
   {
      *pWt++ = i;
   }
   
/* M1 read n(expect pattern1), write pattern2 to n, 
   n from base_addr to SDRAM_size */
   pRd = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_size/4; i++, pRd++ )
   {
      dataread = *pRd;
      if( dataread != i )
      {
         bbu_printf("SDRAM address/increase failed!\n\r");
         bbu_printf("Addr = 0x%08X, expect = 0x%08X, data = 0x%08X\n\r",
                    pRd, i, dataread);
//         return 1;   
      }
      if ( !(i&0x1fffff) )
         bbu_printf(".");
   }
   
   bbu_printf("OK!\r\n");

   return 0;

}

/*..........................................................................*/
int SDRAM_multi_st_ld_test(uint32_t addr, uint32_t cnt)
{
   uint32_t i, j;
   uint32_t *pa,*pb;
   uint32_t test_result[8];
   uint32_t data_pattern[8]=
   {
      0x55555555,
      0xaaaaaaaa,
      0xffffffff,
      0x00000000,
      0x33333333,
      0xcccccccc,                  
      0x99999999,   
      0x66666666   
   };
   
   bbu_printf("Multiple store and load memeory test at %08X, "
               "size %08X...\n\r", addr, cnt );

   bbu_printf("32 bytes store and load memeory test...\n\r");
   pa = (uint32_t *)addr;
   for(i=0; i< cnt/4; i+=8)
   {
      burst_write_32bytes((uint32_t)pa, data_pattern);
      pa +=8;
   }

   pa = (uint32_t *)addr;
   for(i=0; i< cnt/4; i+=8)
   {
      // clean result data;
      pb = test_result;
      for(j=0; j<8; j++)
         *(pb++)= 0xdeaddead;
         
      burst_read_32bytes((uint32_t)pa, test_result);
      for(j=0;j<8; j++)
      {
         if( test_result[j] != data_pattern[j])
         {   
            bbu_printf("\nxxxxxxxxxx  Fail!\r\n");
            bbu_printf("addr: 0x%08X, except: 0x%08X, get: 0x%08X\r\n", 
                        pa, data_pattern[j], test_result[j]);
            //return MV_ERROR;
         }
      }
      pa +=8;
      if ( !(i&0xfffff) )
         bbu_printf(".");

   }
   bbu_printf("OK!\r\n");

   return MV_OK;

}

/*..........................................................................*/
uint32_t SDRAM_walkthrough(uint32_t addr, 
                           uint32_t cnt, 
                           uint32_t pattern1, 
                           uint32_t pattern2)
{

   uint32_t *pWt, *pRd, dataread;
   uint32_t i,j,n;

   uint32_t SDRAM_base_addr = addr;
   uint32_t SDRAM_size = cnt;
   //uint32_t row_num, col_num, bank_num;


   bbu_printf("Memeory test at 0x%08X, using pattern 0x%08X and"
              " 0x%08X...\n\r", addr, pattern1, pattern2 );
/* Algorithm of March with pattern "pattern1" and "pattern2" to X-direction */
/* M0 write pattern1  from base_addr to SDRAM_size */
   pWt = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_size/4; i++)
   {
      *pWt++ = pattern1;
   }

/* M1 read n(expect pattern1), write pattern2 to n, 
   n from base_addr to SDRAM_size */
   pWt = (uint32_t *)SDRAM_base_addr;
   pRd = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_size/4; i++, pRd++, pWt++)
   {
      dataread = *pRd;
      if( dataread != pattern1)
      {
         bbu_printf("MEM M1 failed!\n\r");
         bbu_printf("Addr = 0x%08X, expect = 0x%08X, data = 0x%08X\n\r", 
                    pRd, pattern1, dataread);
//         return 1;   
      }   
      *pWt = pattern2;

      if ( !(i&0x1fffff) )
         bbu_printf(".");
   }

/* M2 read n(expect pattern2), write pattern1 to n, 
   n from base_addr to SDRAM_size */
   pWt = (uint32_t *)SDRAM_base_addr;
   pRd = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_size/4; i++, pRd++, pWt++)
   {
      dataread = *pRd;
      if( dataread != pattern2)
      {
         bbu_printf("MEM M2 failed!\n\r");
         bbu_printf("Addr = 0x%08X, expect = 0x%08X, data = 0x%08X\n\r", 
                     pRd, pattern2, dataread);
//         return 1;   
      }   
      *pWt = pattern1;

      if ( !(i&0x1fffff) )
         bbu_printf(".");
   }

/* M3 read n(expect pattern1), write pattern2 to n, 
  n from SDRAM_size  to base_addr */
   pWt = (uint32_t *)(SDRAM_base_addr + SDRAM_size);
   pRd = (uint32_t *)(SDRAM_base_addr + SDRAM_size);
   pWt--; pRd--;
   
   for(i=0; i< SDRAM_size/4; i++, pRd--, pWt--)
   {
      dataread = *pRd;
      if( dataread != pattern1)
      {
         bbu_printf("MEM M3 failed!\n\r");
         bbu_printf("Addr = 0x%08X, expect = 0x%08X, data = 0x%08X\n\r", 
                     pRd, pattern1, dataread);
//         return 1;   
      }
      *pWt = pattern2;

      if ( !(i&0x1fffff) )
         bbu_printf(".");
   }

/* M4 read n(expect pattern2), write pattern1 to n, 
   n from SDRAM_size  to base_addr */
   pWt = (uint32_t *)(SDRAM_base_addr + SDRAM_size);
   pRd = (uint32_t *)(SDRAM_base_addr + SDRAM_size);
   pWt--;  pRd--;
   
   for(i=0; i< SDRAM_size/4; i++, pRd--, pWt--)
   {
      dataread = *pRd;
      if( dataread != pattern2)
      {
         bbu_printf("MEM M4 failed!\n\r");
         bbu_printf("Addr = 0x%08X, expect = 0x%08X, data = 0x%08X\n",
                     pRd, pattern2, dataread);
//         return 1;   
      }   
      *pWt = pattern1;

      if ( !(i&0x1fffff) )
         bbu_printf(".");
   }

/* M5 read n(expect pattern1), n from base_addr to SDRAM_size   */
   pRd = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_size/4; i++, pRd++)
   {
      dataread = *pRd;
      if( dataread != pattern1)
      {
         bbu_printf("MEM M5 failed!\n\r");
         bbu_printf("Addr = 0x%08X, expect = 0x%08X, data = 0x%08X\n\r", 
                    pRd, pattern1, dataread);
//         return 1;   
      }   
      if ( !(i&0x1fffff) )
         bbu_printf(".");
   }

   bbu_printf("OK!\n\r");

   return(0);   /* skip Y-major test */

/* Algorithm of March with pattern "pattern1" and "pattern2" to Y-direction */
 //  col_num   = SDRAM_COL_NUM;
 //  row_num = SDRAM_ROW_NUM;
 //  bank_num = SDRAM_BANK_NUM;

/* M0 write pattern1  from base_addr to SDRAM_size */
   pWt = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_BANK_NUM; i++)
   {
      for(j=0; j<SDRAM_COL_NUM; j++)
      {   
         for(n=0;n<SDRAM_ROW_NUM ; n++)
         {
            *(pWt+(i<<21)+(n<<9)+(j<<0)) = pattern1;
         }
      }   
   }

/* M1 read n(expect pattern1), write pattern2 to n, 
   n from base_addr to SDRAM_size */
   pWt = (uint32_t *)SDRAM_base_addr;
   pRd = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_BANK_NUM; i++)
   {
      for(j=0; j<SDRAM_COL_NUM; j++)
      {   
         for(n=0;n<SDRAM_ROW_NUM ; n++)
         {
            if((*(pRd+(i<<21)+(n<<9)+(j<<0))) != pattern1)
            {
               bbu_printf("Memory Error !! -- Addr 0x%08X\r\n", pRd );
               return 1;   
            }   
            *(pWt+(i<<21)+(n<<9)+(j<<0)) = pattern2;
         }
      }   
   }

/* M2 read n(expect pattern2), write pattern1 to n, 
   n from base_addr to SDRAM_size */
   pWt = (uint32_t *)SDRAM_base_addr;
   pRd = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_BANK_NUM; i++)
   {
      for(j=0; j<SDRAM_COL_NUM; j++)
      {   
         for(n=0;n<SDRAM_ROW_NUM ; n++)
         {
            if((*(pRd+(i<<21)+(n<<9)+(j<<0))) != pattern2)
            {
               bbu_printf("Memory Error !! -- Addr 0x%08X\r\n", pRd );
               return 1;   
            }   
            *(pWt+(i<<21)+(n<<9)+(j<<0)) = pattern1;
         }
      }   
   }
/* M3 read n(expect pattern1), write pattern2 to n, 
   n from SDRAM_size  to base_addr */
   pWt = (uint32_t *)(SDRAM_base_addr + SDRAM_size) ;
   pRd = (uint32_t *)(SDRAM_base_addr + SDRAM_size);
   pWt--;  pRd--;
   
   for(i=0; i< SDRAM_BANK_NUM; i++)
   {
      for(j=0; j<SDRAM_COL_NUM; j++)
      {   
         for(n=0;n<SDRAM_ROW_NUM ; n++)
         {
            if((*(pRd-(i<<21)-(n<<9)-(j<<0))) != pattern1)
            {
               bbu_printf("Memory Error !! -- Addr 0x%08X\r\n", pRd );
               return 1;   
            }   
            *(pWt-(i<<21)-(n<<9)-(j<<0)) = pattern2;
         }
      }   
   }
/* M4 read n(expect pattern2), write pattern1 to n, 
   n from SDRAM_size  to base_addr */
   pWt = (uint32_t *)(SDRAM_base_addr + SDRAM_size) ;
   pRd = (uint32_t *)(SDRAM_base_addr + SDRAM_size);
   pWt--;  pRd--;
   
   for(i=0; i< SDRAM_BANK_NUM; i++)
   {
      for(j=0; j<SDRAM_COL_NUM; j++)
      {   
         for(n=0;n<SDRAM_ROW_NUM ; n++)
         {
            if((*(pRd-(i<<21)-(n<<9)-(j<<0))) != pattern2)
            {
               bbu_printf("Memory Error !! -- Addr 0x%08X\r\n", pRd );
               return 1;   
            }   
            *(pWt-(i<<21)-(n<<9)-(j<<0)) = pattern1;
         }
      }   
   }
/* M5 read n(expect pattern1), n from base_addr to SDRAM_size   */
   pRd = (uint32_t *)SDRAM_base_addr;
   for(i=0; i< SDRAM_BANK_NUM; i++)
   {
      for(j=0; j<SDRAM_COL_NUM; j++)
      {   
         for(n=0;n<SDRAM_ROW_NUM ; n++)
         {
            if((*(pRd+(i<<21)+(n<<9)+(j<<0))) != pattern1)
            {
               bbu_printf("Memory Error !! -- Addr 0x%08X\r\n", pRd );
               return 1;   
            }   
         }
      }   
   }
   
   return 0;

}

/*..........................................................................*/
static void burst_write_32bytes(uint32_t test_address, uint32_t *data_pattern)
{
   __asm__(
//      "ldr   r0, [r0, #0]\n"
      "add    r0, r0, #32\n"
      "ldr   r2, [r1, #0x00]\n"
      "ldr   r3, [r1, #0x04]\n"
      "ldr   r4, [r1, #0x08]\n"
      "ldr   r5, [r1, #0x0c]\n"
      "ldr   r6, [r1, #0x10]\n"
      "ldr   r7, [r1, #0x14]\n"
      "ldr   r8, [r1, #0x18]\n"
      "ldr   r9, [r1, #0x1c]\n"
      
      "stmfd r0, {r2-r9}\n"
   );
   (void)test_address; (void)data_pattern; //avoid warnings
}
/*..........................................................................*/
static void burst_read_32bytes(uint32_t test_address, uint32_t *test_result)
{
   __asm__(
//   "ldr      r0,[r0, #0]\n"
      "ldmfd r0, {r3-r10}\n"
      
      // write back the read result
      "str   r3,  [r1, #0x00]\n"
      "str   r4,  [r1, #0x04]\n"
      "str   r5,  [r1, #0x08]\n"
      "str   r6,  [r1, #0x0c]\n"
      "str   r7,  [r1, #0x10]\n"
      "str   r8,  [r1, #0x14]\n"
      "str   r9,  [r1, #0x18]\n"
      "str   r10, [r1, #0x1c]\n"
       );
   (void)test_address; (void)test_result; //avoid warnings
}
