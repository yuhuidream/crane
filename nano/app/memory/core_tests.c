#include "predefines.h"
#include <string.h>
#include "core_tests.h"
#include "clock.h"
#include "utils.h"
#include "CIU.h"
#include "cp15.h"
#include "set_mmu.h"
#include "bbu.h"
#include "timer_dec.h"

#define MTSP_QT
#define BIT_BYTE_CHECK 0

int exit_when_error = 1;
                       //otherwise, mtsp will keep running and print out all the errors
uint32_t mtsp_total_error = 0;      // BBU Global errors counter

uint32_t BBU_SMTD_STAT = 0;
uint32_t BBU_SMTD_PAT  = 0x55555555;

static int bit_byte_check = 0;
static uint32_t giByteLaneFails[4];
static uint32_t giBitFails[32];

#ifdef MTSP_QT
#define print_write_pass()
#define print_read_pass()  bbu_putchar('P')
#define print_read_fail()  bbu_putchar('F')
#else
#define print_write_pass()   bbu_printf("\b\b\b\b\b\bPass  ")
#define print_read_pass()    bbu_printf("\b\b\b\b\b\bPass  ")
#define print_read_fail()    bbu_printf("\b\b\b\b\b\bFail  ")
#endif

static volatile int gIncrement = 0;
//static uint32_t mtspAction;
//static s_MTSP_Result_Info   testresults;


const PatternDwords PatternList[] =
{
   {0x00000000,0xFFFFFFFF},//
   {0x55AA55AA,0xAA55AA55},//
   {0x7FFF7FFF,0x80008000},//
   {0xFFFFFFFF,0xFFFFFFFF},//+decrement
   {0x55555555,0xAAAAAAAA},//
   {0x7F7F7F7F,0x80808080},//+rotate
   {0x7FFF7FFF,0x80008000},//+rotate
   {0x7FFFFFFF,0x80000000},//+rotate
   {0x01010101,0xFEFEFEFE},//+rotate left
   {0x00010001,0xFFFEFFFE},//+rotate left
   {0x00000001,0xFFFFFFFE},//+rotate left
   {0x7F7F7F7F,0x7F7F7F7F},//+rotate + without second pattern
   {0x7FFF7FFF,0x7FFF7FFF},//+rotate + without second pattern
   {0x7FFFFFFF,0x7FFFFFFF},//+rotate + without second pattern
   {0x01010101,0x01010101},//+rotate left
   {0x00010001,0x00010001},//+rotate left
   {0x00000001,0x00000001},//+rotate left
   {0x01010101,0xFFFFFFFF},//+rotate left
   {0x00010001,0xFFFFFFFF},//+rotate left
   {0x00000001,0xFFFFFFFF},//+rotate left
   {0xFEFEFEFE,0x00000000},//+rotate left
   {0xFFFEFFFE,0x00000000},//+rotate left
   {0xFFFFFFFE,0x00000000},//+rotate left
   {0x00000000,0x00000001},//+32 bytes+rotate left
   {0x00000001,0x00000001},//+32 bytes+rotate left
   {0x00000000,0x00000001},//+32 bytes+rotate left
   {0x00000000,0x00000001},//+32 bytes+rotate left
   {0x00000000,0xFFFFFFFF},//FREQ ???
   {0x00000000,0x00000000},//Increment
   {0x00000001,0x00000000},//special test
   {0x00000001,0xFFFFFFFF},//special test
   {0x00000001,0x00000000},//special test   
};

#ifndef MTSP_QT
volatile char AP_MTSP_Tst_Msg[32][85] =
{
   {"\n\r  0x00000001 - Writing Analog Data Pattern - Start \0"},                               //1
   {"\n\r  0x00000002 - Writing Constant Data Pattern - Start \0"},                             //2
   {"\n\r  0x00000004 - Writing Complement Data Pattern - Start \0"},                           //3
   {"\n\r  0x00000008 - Writing Decrement Data Pattern - Start \0"},                            //4
   {"\n\r  0x00000010 - Writing Miller Effect Data Pattern - Start \0"},                        //5
   {"\n\r  0x00000020 - Writing Zero Rotate Right & Invert Byte Data Pattern - Start \0"},      //6
   {"\n\r  0x00000040 - Writing Zero Rotate Right & Invert Word Data Pattern - Start \0"},      //7
   {"\n\r  0x00000080 - Writing Zero Rotate Right & Invert DoubleWord Data Pattern - Start \0"},//8
   {"\n\r  0x00000100 - Writing One Rotate Left & Invert Byte Data Pattern - Start \0"},        //9
   {"\n\r  0x00000200 - Writing One Rotate Left & Invert Word Data Pattern - Start \0"},        //10
   {"\n\r  0x00000400 - Writing One Rotate Left & Invert DoubleWord Data Pattern - Start \0"},  //11
   {"\n\r  0x00000800 - Writing Zero Rotate Right Byte Data Pattern - Start \0"},               //12
   {"\n\r  0x00001000 - Writing Zero Rotate Right Word Data Pattern - Start \0"},               //13
   {"\n\r  0x00002000 - Writing Zero Rotate Right DoubleWord Data Pattern - Start \0"},         //14
   {"\n\r  0x00004000 - Writing One Rotate Left Byte Data Pattern - Start \0"},                 //15
   {"\n\r  0x00008000 - Writing One Rotate Left Word Data Pattern - Start \0"},                 //16
   {"\n\r  0x00010000 - Writing One Rotate Left DoubleWord Data Pattern - Start \0"},           //17
   {"\n\r  0x00020000 - Writing Walking 1 Byte Data Pattern - Start \0"},                       //18
   {"\n\r  0x00040000 - Writing Walking 1 Word Data Pattern - Start \0"},                       //19
   {"\n\r  0x00080000 - Writing Walking 1 DoubleWord Data Pattern - Start \0"},                 //20
   {"\n\r  0x00100000 - Writing Walking Zero Byte Data Pattern - Start \0"},                    //21
   {"\n\r  0x00200000 - Writing Walking Zero Word Data Pattern - Start \0"},                    //22
   {"\n\r  0x00400000 - Writing Walking Zero DoubleWord Data Pattern - Start \0"},              //23
   {"\n\r  0x00800000 - Writing Single Signal Tailgating 0101_0000 Data Pattern - Start \0"},   //24
   {"\n\r  0x01000000 - Writing Single Signal Tailgating 0101_1111 Data Pattern - Start \0"},   //25
   {"\n\r  0x02000000 - Writing Single Signal Tailgating 0101_1010 Data Pattern - Start \0"},   //26
   {"\n\r  0x04000000 - Writing Multiple Signal Tailgating Data Pattern - Start \0"},           //27
   {"\n\r  0x08000000 - Writing Frequency Sweep Data Pattern - Start \0"},                      //28
   {"\n\r  0x10000000 - Writing Vertical Increment Data Pattern - Start \0"},                   //29
   {"\n\r  0x20000000 - Writing Horzontal Increment Data Pattern - Start \0"},                  //30
   {"\n\r  0x40000000 - Writing fixed 2nd pattern 0xFFFFFFFF - Start \0"},                      //31
   {"\n\r  0x80000000 - Writing fixed 2nd pattern 0x00000000 - Start \0"},                      //32       
};

const char AP_MTSP_Tst_Msg_Reading[7] = {"Reading"};
const char AP_MTSP_Tst_Msg_Writing[7] = {"Writing"};
#endif

#define   AP_MTSP_TST_MSG_WRITING_OFFSET 17

/*********************   Implementation of functions      *******************/


/*****************************************************************************
* bbu_Print_Error
*
* Input: Address - where data failed
*      Access - BYTE,WORD or DWORD
*      Data - the current read
*      Expected - expected data
*
* This function prints an error message to the user
*****************************************************************************/
void bbu_Print_Error(uint32_t Address, uint8_t Access, 
                uint32_t Data, uint32_t Expected)
{
   int bit, byte;
   uint32_t mask = 0xFFFFFFFF;
#if BIT_BYTE_CHECK
   if(bit_byte_check){
      for(bit=0;bit<32;bit++)
      {
         if((Data & (1<<bit)) != (Expected & (1<<bit)))
            giBitFails[bit%32]++;
      }
      for(byte=0;byte<4;byte++)
      {
         if((Data & (0xFF<<(byte*8))) != (Expected & (0xFF<<(byte*8))))
            giByteLaneFails[byte%4]++;
      }
   }
#endif
   switch(Access)
   {
     case ACCESS_TYPE_BYTE:
        mask = 0xFF;
        break;
     case ACCESS_TYPE_HWORD:
        mask = 0xFFFF;
        break;
    case ACCESS_TYPE_WORD:
       mask = 0xFFFFFFFF;
       break;
     default:
        break;
   }
   bbu_printf("\r\n  ERROR:   Expecting 0x%08X at address 0x%08X . Actual = 0x%08X", 
             Expected&mask, Address, Data&mask);
#ifndef MTSP_QT
   if(exit_when_error)
      bbu_printf("\n\r");
#endif

}

/*****************************************************************************
* bbu_memory_test_type1
*
* Input:  Address - start address for memory to be tested
*       Size - size of memory to be tested
*       First_Pattern - pattern to test the memory
*       Second_Pattern - pattern to test the memory
*       Pattern - kind of pattern to test the memory
*
* Output: 0 - if test passed
*       >1 - if test failed - see error_enum
*
* This function tests the wanted memory according to parameters
*****************************************************************************/
uint32_t bbu_memory_test_type1(uint32_t Address,      uint32_t Size, 
                               uint32_t First_Pattern, 
                               uint32_t Second_Pattern,uint32_t Pattern)
{
   uint32_t i=0,j=0, error=0;
   uint32_t *Tmp_Address32 = (uint32_t*)Address;
   uint32_t First_Pattern_restore = First_Pattern;
   uint32_t Second_Pattern_restore = Second_Pattern;
   (void)Pattern;
   uint32_t actual;

   while(j < Size)
   {
      //Write the data
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);
      Tmp_Address32[i++] = (uint32_t)(Second_Pattern);
      j += 8;
   }
   print_write_pass();

#ifndef MTSP_QT
   //Inform user that test has continue - read sequence
   for(i=0;i<7;i++)
   {
      AP_MTSP_Tst_Msg[Pattern][AP_MTSP_TST_MSG_WRITING_OFFSET+i] = \
                        AP_MTSP_Tst_Msg_Reading[i];
   }

   BBU_putstr((char*)AP_MTSP_Tst_Msg[Pattern]);
#endif

   //Perform activities between write to read
   //testresults.lastOperation = preReadOperation(mtspAction);

   j=0;
   i=0;

   //Restore patterns
   First_Pattern = First_Pattern_restore;
   Second_Pattern = Second_Pattern_restore;

   while(j < Size)
   {
       //Read the data and compare it to the pattern
      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                          actual,First_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(Second_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+4,ACCESS_TYPE_WORD,
                           actual,Second_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      j += 8;
   }
   if(error == 0)
      print_read_pass();

   return MTSP_OPERATION_OK;
}

/*****************************************************************************
* bbu_memory_test_type2
*
* Input:  Address - start address for memory to be tested
*       Size - size of memory to be tested
*       First_Pattern - pattern to test the memory
*       Pattern - kind of pattern to test the memory
*       Increment - if its increment or decrement
*
* Output: 0 - if test passed
*       >1 - if test failed - see error_enum
*
* This function tests the wanted memory according to parameters
*****************************************************************************/
uint32_t bbu_memory_test_type2(uint32_t Address, uint32_t Size, 
                               uint32_t First_Pattern,
                               uint32_t Pattern,uint8_t Increment)
{
   uint32_t i=0,j=0, error=0;
   uint32_t *Tmp_Address32 = (uint32_t*)Address;
   uint32_t First_Pattern_restore = First_Pattern;
   int inc;
   (void)Pattern;
   uint32_t actual;

   if(Increment)
   {
      inc = 1;
   }
   else
   {
      inc = -1;
   }

   while(j < Size)
   {
      //Write the data
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);

      j += 4;

      First_Pattern = First_Pattern + inc;
   }
   print_write_pass();

#ifndef MTSP_QT
   //Inform user that test has continue - read sequence
   for(i=0;i<7;i++)
   {
      AP_MTSP_Tst_Msg[Pattern][AP_MTSP_TST_MSG_WRITING_OFFSET+i] = \
                        AP_MTSP_Tst_Msg_Reading[i];
   }

   BBU_putstr((char*)AP_MTSP_Tst_Msg[Pattern]);
#endif

   //Perform activities between write to read
   //testresults.lastOperation = preReadOperation(mtspAction);

   j=0;
   i=0;

   //Restore patterns
   First_Pattern = First_Pattern_restore;

   while(j < Size)
   {
      //Read the data and compare it to the pattern
      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                          actual,First_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      j += 4;

      First_Pattern = First_Pattern + inc;
   }

   if(error == 0)
      print_read_pass();

   return MTSP_OPERATION_OK;
}

/*****************************************************************************
* bbu_memory_test_type3
*
* Input:  Address - start address for memory to be tested
*       Size - size of memory to be tested
*       First_Pattern - pattern to test the memory
*       Second_Pattern - pattern to test the memory
*       Pattern - kind of pattern to test the memory
*       Left - it the rotation is left or right
*
* Output: 0 - if test passed
*       >1 - if test failed - see error_enum
*
* This function tests the wanted memory according to parameters
*****************************************************************************/
uint32_t bbu_memory_test_type3(uint32_t Address, uint32_t Size, 
                               uint32_t First_Pattern, 
                               uint32_t Second_Pattern,
                               uint32_t Pattern,uint8_t Left)
{
   uint32_t i=0,j=0, error=0;
   uint32_t *Tmp_Address32 = (uint32_t*)Address;
   uint32_t First_Pattern_restore = First_Pattern;
   uint32_t Second_Pattern_restore = Second_Pattern;
   uint32_t rotate_number;
   (void)Pattern;
   uint32_t actual;

   if(Left)
   {
      rotate_number=31;
   }
   else
   {
      rotate_number=1;
   }

   while(j < Size)
   {
      //Write the data
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);

      Tmp_Address32[i++] = (uint32_t)(Second_Pattern);

      j += 8;

      bbu_ror(First_Pattern,rotate_number);
      bbu_ror(Second_Pattern,rotate_number);
   }
   print_write_pass();

#ifndef MTSP_QT
   //Inform user that test has continue - read sequence
   for(i=0;i<7;i++)
   {
      AP_MTSP_Tst_Msg[Pattern][AP_MTSP_TST_MSG_WRITING_OFFSET+i] = \
                        AP_MTSP_Tst_Msg_Reading[i];
   }

   BBU_putstr((char*)AP_MTSP_Tst_Msg[Pattern]);
#endif

   //Perform activities between write to read
   //testresults.lastOperation = preReadOperation(mtspAction);

   j=0;
   i=0;

   //Restore patterns
   First_Pattern = First_Pattern_restore;
   Second_Pattern = Second_Pattern_restore;

   while(j < Size)
   {
      //Read the data and compare it to the pattern
      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                           actual,First_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(Second_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+4,ACCESS_TYPE_WORD,
                           actual,Second_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      j += 8;

      bbu_ror(First_Pattern,rotate_number);
      bbu_ror(Second_Pattern,rotate_number);

   }

   if(error == 0)
      print_read_pass();
   return MTSP_OPERATION_OK;
}

/*****************************************************************************
* bbu_memory_test_type4
*
* Input:  Address - start address for memory to be tested
*       Size - size of memory to be tested
*       First_Pattern - pattern to test the memory
*       Pattern - kind of pattern to test the memory
*       Left - it the rotation is left or right
*
* Output: 0 - if test passed
*       >1 - if test failed - see error_enum
*
* This function tests the wanted memory according to parameters
*****************************************************************************/
uint32_t bbu_memory_test_type4(uint32_t Address, uint32_t Size, 
                               uint32_t First_Pattern,
                               uint32_t Pattern,uint8_t Left)
{
   uint32_t i=0,j=0, error=0;
   uint32_t *Tmp_Address32 = (uint32_t*)Address;
   uint32_t First_Pattern_restore = First_Pattern;
   uint32_t rotate_number;
   (void)Pattern;
   uint32_t actual;

   if(Left)
   {
      rotate_number=31;
   }
   else
   {
      rotate_number=1;
   }

   while(j < Size)
   {
      //Write the data
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);

      j += 4;

      bbu_ror(First_Pattern,rotate_number);
   }
   print_write_pass();

#ifndef MTSP_QT
   //Inform user that test has continue - read sequence
   for(i=0;i<7;i++)
   {
      AP_MTSP_Tst_Msg[Pattern][AP_MTSP_TST_MSG_WRITING_OFFSET+i] = \
                        AP_MTSP_Tst_Msg_Reading[i];
   }

   BBU_putstr((char*)AP_MTSP_Tst_Msg[Pattern]);
#endif

   //Perform activities between write to read
   //testresults.lastOperation = preReadOperation(mtspAction);

   j=0;
   i=0;

   //Restore patterns
   First_Pattern = First_Pattern_restore;

   while(j < Size)
   {
      //Read the data and compare it to the pattern
      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                           actual,First_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      j += 4;

      bbu_ror(First_Pattern,rotate_number);

   }

   if(error == 0)
      print_read_pass();
   return MTSP_OPERATION_OK;
}

/*****************************************************************************
* bbu_memory_test_type5
*
* Input:  Address - start address for memory to be tested
*       Size - size of memory to be tested
*       First_Pattern - pattern to test the memory
*       Second_Pattern - pattern to test the memory
*       Pattern - kind of pattern to test the memory
*
* Output: 0 - if test passed
*       >1 - if test failed - see error_enum
*
* This function tests the wanted memory according to parameters
*****************************************************************************/
uint32_t bbu_memory_test_type5(uint32_t Address, uint32_t Size, 
                               uint32_t First_Pattern, 
                               uint32_t Second_Pattern,uint32_t Pattern)
{
   uint32_t i=0,j=0, error=0;
   uint32_t *Tmp_Address32 = (uint32_t*)Address;
   uint32_t First_Pattern_restore = First_Pattern;
   uint32_t Second_Pattern_restore = Second_Pattern;
   (void)Pattern;
   uint32_t actual;

   while(j < Size)
   {
      //Write the data
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);

      Tmp_Address32[i++] = (uint32_t)(Second_Pattern);

      j += 8;

      bbu_ror(First_Pattern,31);
   }
   print_write_pass();

#ifndef MTSP_QT
   //Inform user that test has continue - read sequence
   for(i=0;i<7;i++)
   {
      AP_MTSP_Tst_Msg[Pattern][AP_MTSP_TST_MSG_WRITING_OFFSET+i] = \
                        AP_MTSP_Tst_Msg_Reading[i];
   }

   BBU_putstr((char*)AP_MTSP_Tst_Msg[Pattern]);
#endif

   //Perform activities between write to read
   //testresults.lastOperation = preReadOperation(mtspAction);

   j=0;
   i=0;

   //Restore patterns
   First_Pattern = First_Pattern_restore;
   Second_Pattern = Second_Pattern_restore;

   while(j < Size)
   {
      //Read the data and compare it to the pattern
      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                           actual,First_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(Second_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+4,ACCESS_TYPE_WORD,
                           actual,Second_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      j += 8;

      bbu_ror(First_Pattern,31);

   }

   if(error == 0)
      print_read_pass();
   return MTSP_OPERATION_OK;
}

/*****************************************************************************
* bbu_memory_test_type6
*
* Input:  Address - start address for memory to be tested
*       Size - size of memory to be tested
*       First_Pattern - pattern to test the memory
*       Second_Pattern - pattern to test the memory
*       Pattern - kind of pattern to test the memory
*
* Output: 0 - if test passed
*       >1 - if test failed - see error_enum
*
* This function tests the wanted memory according to parameters
*****************************************************************************/
uint32_t bbu_memory_test_type6(uint32_t Address, uint32_t Size, 
                               uint32_t First_Pattern, 
                               uint32_t Second_Pattern,uint32_t Pattern)
{
   uint32_t i=0,j=0, error=0;
   uint32_t Temp;
   uint32_t *Tmp_Address32 = (uint32_t*)Address;
   uint32_t First_Pattern_restore = First_Pattern;
   uint32_t Second_Pattern_restore = Second_Pattern;
   (void)Pattern;
   uint32_t actual;

   while(j < Size)
   {
      //Write the data
      Tmp_Address32[i++] = 0x0;
      Temp = (uint32_t)(First_Pattern|Second_Pattern);
      Tmp_Address32[i++] = Temp;
      Tmp_Address32[i++] = 0x0;
      Tmp_Address32[i++] = Temp;
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);
      j += 32;
      bbu_ror(First_Pattern,31);
      bbu_ror(Second_Pattern,31);
   }
   print_write_pass();

#ifndef MTSP_QT
   //Inform user that test has continue - read sequence
   for(i=0;i<7;i++)
   {
      AP_MTSP_Tst_Msg[Pattern][AP_MTSP_TST_MSG_WRITING_OFFSET+i] = \
                        AP_MTSP_Tst_Msg_Reading[i];
   }

   BBU_putstr((char*)AP_MTSP_Tst_Msg[Pattern]);
#endif

   //Perform activities between write to read
   //testresults.lastOperation = preReadOperation(mtspAction);

   j=0;
   i=0;

   //Restore patterns
   First_Pattern = First_Pattern_restore;
   Second_Pattern = Second_Pattern_restore;

   while(j < Size)
   {
      //Read the data and compare it to the pattern
      actual = Tmp_Address32[i++];
      if(actual != 0x0)
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                         actual,0x0);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      Temp = (uint32_t)(First_Pattern|Second_Pattern);

      actual = Tmp_Address32[i++];
      if(actual != Temp)
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+4,ACCESS_TYPE_WORD,
                         actual,Temp);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != 0x0)
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+8,ACCESS_TYPE_WORD,
                         actual,0x0);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != Temp)
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+12,ACCESS_TYPE_WORD,
                         actual,Temp);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+16,ACCESS_TYPE_WORD,
                          actual,First_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+20,ACCESS_TYPE_WORD,
                         actual,First_Pattern);
         mtsp_total_error++;
         error++;      
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+24,ACCESS_TYPE_WORD,
                          actual,First_Pattern);         
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+28,ACCESS_TYPE_WORD,
                         actual,First_Pattern);         
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      j += 32;

      bbu_ror(First_Pattern,31);
      bbu_ror(Second_Pattern,31);
   }

   if(error == 0)
      print_read_pass();
   return MTSP_OPERATION_OK;
}

/*****************************************************************************
* bbu_memory_test_type7
*
* Input:  Address - start address for memory to be tested
*       Size - size of memory to be tested
*       First_Pattern - pattern to test the memory
*       Second_Pattern - pattern to test the memory
*       Pattern - kind of pattern to test the memory
*
* Output: 0 - if test passed
*       >1 - if test failed - see error_enum
*
* This function tests the wanted memory according to parameters
*****************************************************************************/
uint32_t bbu_memory_test_type7(uint32_t Address, uint32_t Size, 
                               uint32_t First_Pattern, 
                               uint32_t Second_Pattern,uint32_t Pattern)
{
   uint32_t i=0,j=0, error=0;
   uint32_t Temp;
   uint32_t *Tmp_Address32 = (uint32_t*)Address;
   uint32_t First_Pattern_restore = First_Pattern;
   uint32_t Second_Pattern_restore = Second_Pattern;
   (void)Pattern;
   uint32_t actual;

   while(j < Size)
   {
      //Write the data
      Tmp_Address32[i++] = 0x0;
      Temp = (uint32_t)(First_Pattern|Second_Pattern);
      Tmp_Address32[i++] = Temp;
      Tmp_Address32[i++] = 0x0;
      Tmp_Address32[i++] = Temp;
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);
      Tmp_Address32[i++] = 0x0;
      Tmp_Address32[i++] = (uint32_t)(First_Pattern);
      Tmp_Address32[i++] = 0x0;
      j += 32;
      bbu_ror(First_Pattern,31);
      bbu_ror(Second_Pattern,31);
   }
   print_write_pass();

#ifndef MTSP_QT
   //Inform user that test has continue - read sequence
   for(i=0;i<7;i++)
   {
      AP_MTSP_Tst_Msg[Pattern][AP_MTSP_TST_MSG_WRITING_OFFSET+i] = \
                        AP_MTSP_Tst_Msg_Reading[i];
   }

   BBU_putstr((char*)AP_MTSP_Tst_Msg[Pattern]);
#endif

   //Perform activities between write to read
   //testresults.lastOperation = preReadOperation(mtspAction);

   j=0;
   i=0;

   //Restore patterns
   First_Pattern = First_Pattern_restore;
   Second_Pattern = Second_Pattern_restore;

   while(j < Size)
   {
      //Read the data and compare it to the pattern
      actual = Tmp_Address32[i++];
      if(actual != 0x0)
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                         actual,0x0);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      Temp = (uint32_t)(First_Pattern|Second_Pattern);

      actual = Tmp_Address32[i++];
      if(actual != Temp)
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+4,ACCESS_TYPE_WORD,
                         actual,Temp);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != 0x0)
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+8,ACCESS_TYPE_WORD,
                          actual,0x0);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != Temp)
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+12,ACCESS_TYPE_WORD,
                         actual,Temp);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+16,ACCESS_TYPE_WORD,
                          actual,First_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != 0x0)
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+20,ACCESS_TYPE_WORD,
                         actual,0x0);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != (uint32_t)(First_Pattern))
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+24,ACCESS_TYPE_WORD,
                          actual,First_Pattern);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      actual = Tmp_Address32[i++];
      if(actual != 0x0)
      {
         if(!error)
            print_read_fail();
         bbu_Print_Error((uint32_t)Tmp_Address32+j+28,ACCESS_TYPE_WORD,
                         actual,0x0);
         mtsp_total_error++;
         error++;
         if(exit_when_error)
            return MTSP_READ_FAILED;
      }

      j += 32;

      bbu_ror(First_Pattern,31);
      bbu_ror(Second_Pattern,31);
   }

   if(error == 0)
      print_read_pass();
   return MTSP_OPERATION_OK;
}

/*****************************************************************************
* bbu_memory_test_type8
*
* Input:  Address - start address for memory to be tested
*       Size - size of memory to be tested
*       First_Pattern - pattern to test the memory
*       Second_Pattern - pattern to test the memory
*       Pattern - kind of pattern to test the memory
*
* Output: 0 - if test passed
*       >1 - if test failed - see error_enum
*
* This function tests the wanted memory according to parameters
*****************************************************************************/
uint32_t bbu_memory_test_type8(uint32_t Address, uint32_t Size, 
                               uint32_t First_Pattern, 
                               uint32_t Second_Pattern,uint32_t Pattern)
{
   uint32_t i=0,j=0, error=0;
   int loop=1,freq;
   int endTest=0;
   uint32_t *Tmp_Address32 = (uint32_t*)Address;
   uint32_t First_Pattern_restore = First_Pattern;
   uint32_t Second_Pattern_restore = Second_Pattern;
   (void)Pattern;
   uint32_t actual;

   //Infinite loop - if J=>Size break the loop
   for(;;)
   {
      //Write the data
      freq = loop;
      while(freq)
      {
         Tmp_Address32[i++] = (uint32_t)(First_Pattern);
         j += 4;
         if(j>=Size)
         {
            endTest=1;
            freq = 1;
            break;
         }
         freq--;
      }

      if(endTest)
         break;
      freq = loop;

      while(freq)
      {
         Tmp_Address32[i++] = (uint32_t)(Second_Pattern);
         j += 4;

         if(j>=Size)
         {
            endTest=1;
            freq = 1;
            break;
         }
         freq--;
     }

     if(endTest)
        break;

     loop++;
     if(loop == 0x100)
        loop = 1;
   }
   print_write_pass();

#ifndef MTSP_QT
   //Inform user that test has continue - read sequence
   for(i=0;i<7;i++)
   {
      AP_MTSP_Tst_Msg[Pattern][AP_MTSP_TST_MSG_WRITING_OFFSET+i] = \
                        AP_MTSP_Tst_Msg_Reading[i];
   }

   BBU_putstr((char*)AP_MTSP_Tst_Msg[Pattern]);
#endif

   //Perform activities between write to read
   //testresults.lastOperation = preReadOperation(mtspAction);

   j=0;
   i=0;
   loop = 1;

   //Restore patterns
   First_Pattern = First_Pattern_restore;
   Second_Pattern = Second_Pattern_restore;

   //Infinite loop - if J=>Size break the loop
   for(;;)
   {
      //Write the data
      freq = loop;

      while(freq)
      {
         actual = Tmp_Address32[i++];
         if(actual != (uint32_t)(First_Pattern))
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                           actual,First_Pattern);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
         j += 4;

         if(j>=Size)
         {
            endTest=1;
            freq = 1;
            break;
         }
         freq--;
      }

      if(endTest)
         break;

      freq = loop;

      while(freq)
      {
         actual = Tmp_Address32[i++];
         if(actual != (uint32_t)(Second_Pattern))
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                            actual,Second_Pattern);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
         j += 4;

         if(j>=Size)
         {
            endTest=1;
            freq = 1;
            break;
         }
         freq--;
      }

      if(endTest)
         break;

      loop++;
      if(loop == 0x100)
         loop = 1;
   }

   if(error == 0)
      print_read_pass();
   return MTSP_OPERATION_OK;
}

/*****************************************************************************
* bbu_memory_test_type9
*
* Input:  Address - start address for memory to be tested
*       Size - size of memory to be tested
*       First_Pattern - pattern to test the memory
*       Second_Pattern - pattern to test the memory
*       Pattern - kind of pattern to test the memory
*
* Output: 0 - if test passed
*       >1 - if test failed - see error_enum
*
* This function tests the wanted memory according to parameters
*****************************************************************************/
uint32_t bbu_memory_test_type9(uint32_t Address, uint32_t Size, 
                               uint32_t First_Pattern, 
                               uint32_t Second_Pattern,uint32_t Pattern)
{
   uint32_t i=0,j=0, error=0;
   uint32_t *Tmp_Address32 = (uint32_t*)Address;
   uint32_t First_Pattern_restore = First_Pattern;
   uint32_t Second_Pattern_restore = Second_Pattern;
   uint32_t Address_restore = Address;
   (void)Pattern;
   uint32_t actual;

   while(j < Size)
   {
      //Write the data
      if(Address&0x20) //Bit 5 set?
      {
         Tmp_Address32[i] = (uint32_t)(First_Pattern);
         Tmp_Address32[i+1] = (uint32_t)(Second_Pattern);
      }
      else
      {
         Tmp_Address32[i] = 0x0;
         Tmp_Address32[i+1] = 0x0;
      }
      if(Address&0x40) //Bit 6 set?
      {
         Tmp_Address32[i+2] = (uint32_t)(First_Pattern);
         Tmp_Address32[i+3] = (uint32_t)(Second_Pattern);
      }
      else
      {
         Tmp_Address32[i+2] = 0x0;
         Tmp_Address32[i+3] = 0x0;
      }
      if(Address&0x80) //Bit 7 set?
      {
         Tmp_Address32[i+4] = (uint32_t)(First_Pattern);
         Tmp_Address32[i+5] = (uint32_t)(Second_Pattern);
      }
      else
      {
         Tmp_Address32[i+4] = 0x0;
         Tmp_Address32[i+5] = 0x0;
      }
      if(Address&0x100) //Bit 8 set?
      {
         Tmp_Address32[i+6] = (uint32_t)(First_Pattern);
         Tmp_Address32[i+7] = (uint32_t)(Second_Pattern);
      }
      else
      {
         Tmp_Address32[i+6] = 0x0;
         Tmp_Address32[i+7] = 0x0;
      }

      i += 8;
      j += 32;
      Address += 32;

      if(!(Address&0x1E0))
      {
         if(((uint32_t)(First_Pattern)) == 0x80000000)
         {
            Second_Pattern = 0x1;
            First_Pattern = 0x0;
         }
         else if(((uint32_t)(Second_Pattern)) == 0x80000000)
         {
            Second_Pattern = 0x0;
            First_Pattern = 0x1;
         }
         else
         {
            Second_Pattern = Second_Pattern<<1;
            First_Pattern = First_Pattern<<1;
         }
      }
   }
   print_write_pass();

#ifndef MTSP_QT
   //Inform user that test has continue - read sequence
   for(i=0;i<7;i++)
   {
      AP_MTSP_Tst_Msg[Pattern][AP_MTSP_TST_MSG_WRITING_OFFSET+i] = \
                        AP_MTSP_Tst_Msg_Reading[i];
   }

   BBU_putstr((char*)AP_MTSP_Tst_Msg[Pattern]);
#endif

   //Perform activities between write to read
   //testresults.lastOperation = preReadOperation(mtspAction);

   j=0;
   i=0;

   //Restore patterns
   First_Pattern = First_Pattern_restore;
   Second_Pattern = Second_Pattern_restore;
   Address = Address_restore;
   while(j < Size)
   {
      //Write the data
      if(Address&0x20) //Bit 5 set?
      {
         actual = Tmp_Address32[i];
         if(actual != (uint32_t)(First_Pattern))
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                            actual,First_Pattern);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
         actual = Tmp_Address32[i+1];
         if(actual != (uint32_t)(Second_Pattern))
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+4,ACCESS_TYPE_WORD,
                            actual,Second_Pattern);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
      }
      else
      {
         actual = Tmp_Address32[i];
         if(actual != 0x0)
         {
            if(!error)
               print_read_fail();   
            bbu_Print_Error((uint32_t)Tmp_Address32+j,ACCESS_TYPE_WORD,
                            actual,0x0);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
         actual = Tmp_Address32[i+1];
         if(actual != 0x0)
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+4,ACCESS_TYPE_WORD,
                             actual,0x0);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
      }
      if(Address&0x40) //Bit 6 set?
      {
         actual = Tmp_Address32[i+2];
         if(actual != (uint32_t)(First_Pattern))
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+8,ACCESS_TYPE_WORD,
                            actual,First_Pattern);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
         actual = Tmp_Address32[i+3];
         if(actual != (uint32_t)(Second_Pattern))
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+12,ACCESS_TYPE_WORD,
                           actual,Second_Pattern);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
      }
      else
      {
         actual = Tmp_Address32[i+2];
         if(actual != 0x0)
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+8,ACCESS_TYPE_WORD,
                            actual,0x0);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
         actual = Tmp_Address32[i+3];
         if(actual != 0x0)
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+12,ACCESS_TYPE_WORD,
                            actual,0x0);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
      }
      if(Address&0x80) //Bit 7 set?
      {
         actual = Tmp_Address32[i+4];
         if(actual != (uint32_t)(First_Pattern))
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+16,ACCESS_TYPE_WORD,
                            actual,First_Pattern);
            mtsp_total_error++;
            error++;        
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
         actual = Tmp_Address32[i+5];
         if(actual != (uint32_t)(Second_Pattern))
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+20,ACCESS_TYPE_WORD,
                            actual,Second_Pattern);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
      }
      else
      {
         actual = Tmp_Address32[i+4];
         if(actual != 0x0)
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+16,ACCESS_TYPE_WORD,
                            actual,0x0);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
         actual = Tmp_Address32[i+5];
         if(actual != 0x0)
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+20,ACCESS_TYPE_WORD,
                            actual,0x0);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
      }
      if(Address&0x100) //Bit 8 set?
      {
         actual = Tmp_Address32[i+6];
         if(actual != (uint32_t)(First_Pattern))
         {
            if(!error)
               print_read_fail();   
            bbu_Print_Error((uint32_t)Tmp_Address32+j+24,ACCESS_TYPE_WORD,
                            actual,First_Pattern);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
         actual = Tmp_Address32[i+7];
         if(actual != (uint32_t)(Second_Pattern))
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+28,ACCESS_TYPE_WORD,
                            actual,Second_Pattern);
            mtsp_total_error++;
            error++;      
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
      }
      else
      {
         actual = Tmp_Address32[i+6];
         if(actual != 0x0)
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+24,ACCESS_TYPE_WORD,
                             actual,0x0);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
         actual = Tmp_Address32[i+7];
         if(actual != 0x0)
         {
            if(!error)
               print_read_fail();
            bbu_Print_Error((uint32_t)Tmp_Address32+j+28,ACCESS_TYPE_WORD,
                        actual,0x0);
            mtsp_total_error++;
            error++;
            if(exit_when_error)
               return MTSP_READ_FAILED;
         }
      }

      i += 8;
      j += 32;
      Address += 32;

      if(!(Address&0x1E0))
      {
         if(((uint32_t)(First_Pattern)) == 0x80000000)
         {
            Second_Pattern = 0x1;
            First_Pattern = 0x0;
         }
         else if(((uint32_t)(Second_Pattern)) == 0x80000000)
         {
            Second_Pattern = 0x0;
            First_Pattern = 0x1;
         }
         else
         {
            Second_Pattern = Second_Pattern<<1;
            First_Pattern = First_Pattern<<1;
         }
      }
   }
   if(error == 0)
      print_read_pass();
   return MTSP_OPERATION_OK;
}

/*****************************************************************************
* bbu_memory_test
*
* Input:  Address - start address for memory to be tested
*       Size - size of memory to be tested
*       Pattern - kind of pattern to test the memory
*       Access - BYTE,WORD or DWORD
*       Mtsp - to define if the test is MTSP type
*       Cache - option for Cache issues - TBD
*
* Output: 0 - if test passed
*       >1 - if test failed - see error_enum
*
* This function tests the wanted memory according to parameters
*****************************************************************************/
uint32_t bbu_memory_test(uint32_t Address, uint32_t Size, uint32_t Pattern, 
                         uint8_t Access, uint8_t Mtsp)
{
   uint32_t i=0,j=0;
   uint8_t  mtsp_rc = MTSP_OPERATION_OK;
   uint32_t First_Pattern=0,Second_Pattern=0;
   uint8_t  *Tmp_Address8  = (uint8_t*)Address;
   uint16_t *Tmp_Address16 = (uint16_t*)Address;
   uint32_t *Tmp_Address32 = (uint32_t*)Address;
   //Make sure address is DWORD aligned
   //TBD

   //Get the desired pattern but only for DWORD patterns
   if(Mtsp)
   {
      if(BBU_SMTD_STAT&0x1)
      {
         First_Pattern  = BBU_SMTD_PAT;
         Second_Pattern = ~BBU_SMTD_PAT;
      }
      else
      {
         First_Pattern = PatternList[Pattern].First_Pattern;
         Second_Pattern = PatternList[Pattern].Second_Pattern;
      }

#ifndef MTSP_QT
      BBU_putstr((char*)AP_MTSP_Tst_Msg[Pattern]);
#endif
      //Call test functions
      switch(Pattern)
      {
         case PATTERN_NUMBER_ONE:
         case PATTERN_NUMBER_TWO:
         case PATTERN_NUMBER_THREE:
         case PATTERN_NUMBER_FIVE:
            mtsp_rc = bbu_memory_test_type1(Address,Size,First_Pattern,
                                            Second_Pattern,Pattern);
            break;
         case PATTERN_NUMBER_FOUR:
            mtsp_rc = bbu_memory_test_type2(Address,Size,First_Pattern,
                                             Pattern,0);
            break;
         case PATTERN_NUMBER_SIX:
         case PATTERN_NUMBER_SEVEN:
         case PATTERN_NUMBER_EIGHT:
            mtsp_rc = bbu_memory_test_type3(Address,Size,First_Pattern,
                                            Second_Pattern,Pattern,0);
            break;
         case PATTERN_NUMBER_NINE:
         case PATTERN_NUMBER_TEN:
         case PATTERN_NUMBER_ELEVEN:
            mtsp_rc = bbu_memory_test_type3(Address,Size,First_Pattern,
                                            Second_Pattern,Pattern,1);
            break;
         case PATTERN_NUMBER_TWELVE:
         case PATTERN_NUMBER_THIRTEEN:
         case PATTERN_NUMBER_FOURTEEN:
            mtsp_rc = bbu_memory_test_type4(Address,Size,First_Pattern, 
                                            Pattern,0);
            break;
         case PATTERN_NUMBER_FIFTEEN:
         case PATTERN_NUMBER_SIXTEEN:
         case PATTERN_NUMBER_SEVENTEEN:
            mtsp_rc = bbu_memory_test_type4(Address,Size,First_Pattern,
                                            Pattern,1);
            break;
         case PATTERN_NUMBER_EIGHTEEN:
         case PATTERN_NUMBER_NINETEEN:
         case PATTERN_NUMBER_TWENTY:
         case PATTERN_NUMBER_TWENTYONE:
         case PATTERN_NUMBER_TWENTYTWO:
         case PATTERN_NUMBER_TWENTYTHREE:
            mtsp_rc = bbu_memory_test_type5(Address,Size,First_Pattern,
                                            Second_Pattern,Pattern);
            break;
         case PATTERN_NUMBER_TWENTYFOUR:
         case PATTERN_NUMBER_TWENTYFIVE:
         case PATTERN_NUMBER_TWENTYSEVEN:
            mtsp_rc = bbu_memory_test_type6(Address,Size,First_Pattern,
                                            Second_Pattern,Pattern);
            break;
         case PATTERN_NUMBER_TWENTYSIX:
            mtsp_rc = bbu_memory_test_type7(Address,Size,First_Pattern,
                                            Second_Pattern,Pattern);
            break;
         case PATTERN_NUMBER_TWENTYEIGHT:
            mtsp_rc = bbu_memory_test_type8(Address,Size,First_Pattern,
                                            Second_Pattern,Pattern);
            break;
         case PATTERN_NUMBER_TWENTYNINE:
            mtsp_rc = bbu_memory_test_type2(Address,Size,First_Pattern,
                                            Pattern,1);
            break;
         case PATTERN_NUMBER_THIRTY:
            mtsp_rc = bbu_memory_test_type9(Address,Size,First_Pattern,
                                            Second_Pattern,Pattern);
            break;
         case PATTERN_NUMBER_THIRTYONE:
            mtsp_rc = bbu_memory_test_type1(Address,Size,First_Pattern,
                                            0xFFFFFFFF,Pattern);
            break;
         case PATTERN_NUMBER_THIRTYTWO:
            mtsp_rc = bbu_memory_test_type1(Address,Size,First_Pattern,
                                            0x00000000,Pattern);         
            break;
         default:
            break;
      }

#ifndef MTSP_QT
      bbu_printf("\n\r");
      //Change back the string to write
      for(i=0;i<7;i++)
      {
         AP_MTSP_Tst_Msg[Pattern][AP_MTSP_TST_MSG_WRITING_OFFSET+i] \
                   = AP_MTSP_Tst_Msg_Writing[i];
      }
#endif
   }
   else
   {
#ifndef MTSP_QT
      BBU_putstr("Regular memory test begins.\n\r");
#endif
      while(j < Size)
      {
         switch(Access)
         {
            case ACCESS_TYPE_BYTE:
               Tmp_Address8[i] = (uint8_t)(Pattern&0xFF);
            
               if(Tmp_Address8[i++] != (uint8_t)(Pattern&0xFF))
               {
                  bbu_Print_Error((uint32_t)Tmp_Address8+j,Access,
                                   Tmp_Address8[i-1],Pattern);
                  mtsp_total_error++;
                  if(exit_when_error)
                     return MTSP_READ_FAILED;
               }
               j += 1;
               break;
            case ACCESS_TYPE_HWORD:
               Tmp_Address16[i] = (uint16_t)(Pattern&0xFFFF);
            
               if(Tmp_Address16[i++] != (uint16_t)(Pattern&0xFFFF))
               {
                  bbu_Print_Error((uint32_t)Tmp_Address16+j,Access,
                                   Tmp_Address16[i-1],Pattern);
                  mtsp_total_error++;
                  if(exit_when_error)
                     return MTSP_READ_FAILED;
               }
               j += 2;
               break;
            case ACCESS_TYPE_WORD:
               Tmp_Address32[i] = (uint32_t)(Pattern);
               if(Tmp_Address32[i++] != (uint32_t)(Pattern))
               {
                  bbu_Print_Error((uint32_t)Tmp_Address32+j,Access,
                                  Tmp_Address32[i-1],Pattern);
                  mtsp_total_error++;
                  if(exit_when_error)
                     return MTSP_READ_FAILED;
               }
               j += 4;
               break;
            default:
               return MTSP_WRONG_PARAMETER;
         }
         Pattern += gIncrement;
      }
      mtsp_rc = MTSP_OPERATION_OK;

   }

   return mtsp_rc;
}

/* ........................................................................ */
uint32_t bbu_mtsp_test(uint32_t Address,  uint32_t Size, 
                       uint32_t Pattern , uint32_t preReadAction)
{
   uint32_t CurrrentPattern=0;
   uint32_t index=0x1;
   size_t i;
   uint8_t  mtsp_rc = MTSP_OPERATION_OK;
   uint32_t tmsec = 0, count = 0;

   mtsp_total_error = 0;
#if BIT_BYTE_CHECK
   if(bit_byte_check){
      for(i = 0; i < sizeof(giByteLaneFails)/sizeof(uint32_t); i++)
         giByteLaneFails[i] = 0;
      for(i = 0; i < sizeof(giBitFails)/sizeof(uint32_t); i++)
         giBitFails[i] = 0;
   }
#endif

   bbu_printf("MTSP test starts from: 0x%08X to : 0x%08X\n\n\r", Address, Address+Size);

#if 0
   tmsec = timer_get_count(0);
#endif

   //Update pre-Read action
   //mtspAction = preReadAction;
   (void)preReadAction;

   for(i=0;i<32;i++)
   {
      CurrrentPattern = Pattern&(index<<i);
      if(CurrrentPattern)
      {
         mtsp_rc = bbu_memory_test(Address,Size,i,ACCESS_TYPE_WORD,1);
         if(mtsp_rc)
         {
            break;
         }
      }

   }
#if 0
   count = timer_get_count(0);
   tmsec = TIMER_GET_INTERVAL(count, tmsec)/TIMER0_TICK_PER_MSEC;
//   tmsec = (timer_get_count(0) - tmsec)/TIMER0_TICK_PER_MSEC;
   bbu_printf("\n\r  Elapsed time: %d msec\n\r", tmsec);
#endif

   if(mtsp_total_error){
      bbu_printf("\n\rBBU mtsp memory test finished and %d errors occur\n\r",
              mtsp_total_error);
   }else{
      bbu_printf("\n\rBBU mtsp memory test finished successfully.\n\n\r");
   }

   if(mtsp_total_error){
#if BIT_BYTE_CHECK
      if(bit_byte_check){
         bbu_printf("******* MTSP BYTE & BIT error statistics ******\n\r");
         for(i = 0; i < sizeof(giByteLaneFails)/sizeof(uint32_t); i++){
            bbu_printf("BYTE %d error: %d\n\r", i, giByteLaneFails[i]);
         }
         bbu_printf("----------\n\r");
         for(i = 0; i < sizeof(giBitFails)/sizeof(uint32_t); i++){
            bbu_printf("BIT %d error: %d\n\r", i, giBitFails[i]);
         }
      }
#endif
      return MTSP_READ_FAILED;
   }

   return mtsp_rc;
}


/* ........................................................................ */
int bbu_mtsp(int argc, uint8_t **argv)
{
   //Local variables definition
   uint32_t  mem_test_size, patterns_to_activate;
   uint32_t  startAddr, test_times;
   test_times = 1;
   switch(argc){
      case 1:
         bbu_printf("mtsp-->Enter the start address to be tested\n\r");
         break;
      case 2:
         if(check_help(argv[1]))
            bbu_mstp_help();
         else
            bbu_printf("mtsp-->Enter number of bytes to be tested\n\r");
         break;
      case 3:
         bbu_printf("mtsp-->Set the bits of the test to be run \n\r");
         break;
      case 5:
         test_times = conv_dec((char *)argv[4]);
      case 4:
         mem_test_size = conv_hex((char *)argv[2]);
         startAddr = conv_hex((char *)argv[1]);
         patterns_to_activate = conv_hex((char *)argv[3]);
         for(uint32_t i = 0; i < test_times; i++){
            bbu_printf("\n\r************ MTSP TEST LOOP: %d"
                      " ************\n\n\r", i+1);
            if (bbu_mtsp_test(startAddr,mem_test_size,patterns_to_activate,0))
            {
               return SCREEN_TEST_FIAL; //Remember to add the string
            }
         }
         break;
      default:
         bbu_mstp_help();
         break;
   }
   
   return SCREEN_TEST_PASS;
} 

#if 0
/* ........................................................................ */
void bbu_smtd_help(void)
{
   bbu_printf("\n\r smtd - Set Memory Test Defaults.\n\r");
   bbu_printf("  The SMTD command is used to set the memory test "
              "defaults for several memory\n\r");
   bbu_printf("  test subroutines - most notably the "
              "MTSP set of memory tests.\n\r");
   bbu_printf(" Usage:smtd [<pattern>]\n\r");
   bbu_printf("  pattern  = The first pattern for mtsp test\n\r");
   bbu_printf(" Note: 1. Use 'smtd ena' to enable/disable "
              "default pattern list\n\r");
   bbu_printf("       2. Use 'smtd mode' to switch the mtsp test"
              " mode: \n\r          continue or terminate the test when"
              " error occurs\n\r");
   bbu_printf("       3. Use 'smtd check' to enable/disable byte&bit check mechanism\n\r");
   bbu_printf("  If no argument entered, will display current defaults\n\r");
}
/* ........................................................................ */
int bbu_smtd(int argc, uint8_t **argv)
{
   switch(argc){
      case 1:
         if(BBU_SMTD_STAT == 0)
            bbu_printf("\n\rsmtd-->  The default pattern list is enabled\n\r");
         else
            bbu_printf("\n\rsmtd-->  The default pattern list is disabled\n\r");
         if(exit_when_error == 0)
            bbu_printf("    -->  In the mode that continue test when error happens\n\r");
         else
            bbu_printf("    -->  In the mode that exit test when error happens\n\r");
         if(bit_byte_check == 0)
            bbu_printf("    -->  Byte and bit check mechanism is disabled\n\r");
         else
            bbu_printf("    -->  Byte and bit check mechanism is enabled\n\r");
         break;
      case 2:
         if(check_help(argv[1])){
            bbu_smtd_help();
         }else if(!strcmp((const char*)argv[1], "ena")){
            if(BBU_SMTD_STAT == 0){
               BBU_SMTD_STAT = 1;
               bbu_printf("smtd-->  The default pattern list is "
                          "disabled\n\r");
            }
            else{
               BBU_SMTD_STAT = 0;
               bbu_printf("smtd-->  The default pattern list is enabled\n\r");
            }
         }else if(!strcmp((const char*)argv[1], "mode")){
            if(exit_when_error){
               exit_when_error = 0;
               bbu_printf("smtd-->  Change MTSP to the mode that tests "
                           "will continue even error occurs\n\r");
            }
            else{
               exit_when_error = 1;
               bbu_printf("smtd-->  Change MTSP to the mode that tests "
                           "will terminate when error occurs\n\r");
            }
         }
         else if(!strcmp((const char*)argv[1], "check")){
            if(bit_byte_check){
               bit_byte_check = 0;
               bbu_printf("smtd-->  MTSP byte&bit check is disabled\n\r");
            }
            else{
               bit_byte_check = 1;
               bbu_printf("smtd-->  MTSP byte&bit check is enabled\n\r");
            }
         }
         else{
            BBU_SMTD_PAT = conv_hex((char *)argv[1]);
         }
         break;
      default:
         bbu_smtd_help();
         break;
            
   }
   return 1;
}
#endif
