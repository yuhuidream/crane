//===================================================================================================================
// FUNCTION NAME:   mcuGenerateintPattern() - support function
//-------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: Generates the selected pattern to the given address for the number of ints provided or checks
//  the data at uiSourceAdddress against the selected pattern.  
//
// PARAMETERS:  uiSourceAddress:    Start Address for transaction
//  uiPattern:  Pattern number to generate
//  uiTransSize:    Number of ints for the transaction
//  uiStartPattern: Start value for pattern
//  bCheck: If TRUE the function compares uiSourceAddress to the pattern
//  If FALSE the function writes the pattern to uiSourceAddress
// RETURN:  NONE
//===================================================================================================================
#include "predefines.h"
#include <string.h>
#include "bbu.h"
#include "pmic.h"
#include "timer_dec.h"
#include "CIU.h"
#include "common_tests.h"
#include "mcu_generate_pattern.h"

// int giPatternRuns[32] = {0};
//int //guiCSBaseAddress;
int giByteLaneFails[4] = {0};
//int giPatternNumOfFails[32] = {0};
//int giPatternFails[32] ={0};
int uiSourceAddress = 0;
int giBitFails[32] = {0};
int uiFails2 = 0;
/*need to tune*/
char * SV_ERROR_LOG = "FAIL - OMIT FOLLOWING LOG ...\n\r";
 
uint32_t ror32(uint32_t input) {
	uint32_t output,temp = input;
	__asm__ __volatile__(
	"mov %0,%1, ror  #1\n\t"
	:"=&r"(output)
	:"r"(temp)
	:"cc","memory"
	);
	return output;	
}

uint32_t rol32(uint32_t input) {
	uint32_t output,temp = input;
	__asm__ __volatile__(
	"mov %0, %1, ror #31\n\t"
	:"=&r"(output)
	:"r"(temp)
	:"cc","memory"
	);
	return output;	
}

static void pass_print_log(uint32_t pattern, uint32_t value, uint32_t addr, uint32_t expected)
{
    //bbu_printf("PASS - pattern %d [@0x%08X]=0x%08X Expect 0x%08X\n\r",pattern,addr,value,expected);
}

uint32_t mcuGenerateWordPattern(uint32_t uiSourceAddress, uint32_t uiPattern, uint32_t uiTransSize, uint32_t uiStartPattern, int bCheck)
{
    const uint32_t c_zero = 0x00000000;
    int test_signal = 0x00000001;
    uint32_t c_pattern;
    uint32_t c_pattern_or_signal;
    int uiLogFailures = 10;
    uint32_t i=0, j=0,k=0,uiTemp, uiFails=0, bit=0, byte=0, expected;
    
    //giPatternRuns[uiPattern]++;
    bbu_printf("SourceAddr 0x%08X Pattern %d TransSize 0x%X StartVal 0x%08X\n\r\n\r",uiSourceAddress, uiPattern, uiTransSize*4, uiStartPattern);
    switch(uiPattern)
    {
        case ANALOG:
            uiFails2 = 0;
            //bbu_printf("Generating ANALOG Pattern\n\r");
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != 0xffffffff)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures))
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,0xffffffff,(uiTemp^0xffffffff),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = 0xffffffff;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,0xffffffff);
                    i++;
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != ~(0xffffffff))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,~(0xffffffff),(uiTemp^(~0xffffffff)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = ~(0xffffffff);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,~(0xffffffff));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = 0xffffffff;
                    i++;
                    *(int *)(uiSourceAddress +(i*4)) = ~0xffffffff;
                }
            }
            break;
        case CONSTANT: 
            uiFails2 = 0;
            //bbu_printf("Generating CONSTANT Pattern %x\n\r", uiStartPattern);
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != uiStartPattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern),(uiTemp ^ uiStartPattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = uiStartPattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (uiStartPattern);
                    //////bbu_printf("i %x %x @ (%x){%x}\n\r",i,(int)(uiSourceAddress +(i*4)),*(int *)(uiSourceAddress +(i*4)), (uiStartPattern));
                }
            }
            break;
        case COMPLEMENT:
            uiFails2 = 0;
            //bbu_printf("Generating COMPLEMENT Pattern - Start Pattern %08X\n\r", uiStartPattern);
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != (uiStartPattern))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern),(uiTemp ^ (uiStartPattern)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = uiStartPattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (uiStartPattern);
                    //////bbu_printf("i %x (%x)\n\r",i,*(int *)(uiSourceAddress +(i*4)));
                }
                i++;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != (~uiStartPattern))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(~uiStartPattern),(uiTemp ^ (~uiStartPattern)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = (~uiStartPattern);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(~uiStartPattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (~uiStartPattern);
                    //////bbu_printf("i %x (%x)\n\r",i,*(int *)(uiSourceAddress +(i*4)));
                }
            }
            break;

        case DECREMENT:
            uiFails2 = 0;
            //bbu_printf("Generating DECREMENT Pattern - Start Pattern %08X\n\r", uiStartPattern);
            if(uiStartPattern == 0)
                uiStartPattern = 0xF0F0F0F0;  
            c_pattern = uiStartPattern;
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                }
                if (c_pattern == 0 )  
                    c_pattern = uiStartPattern;
                else
                    c_pattern--;
                //HexDisplay(c_pattern);
            }
            break;
        case INCREMENT:
            uiFails2 = 0;
            //bbu_printf("Generating INCREMENT Pattern - Start Pattern %08X\n\r", uiStartPattern);
            if(uiStartPattern == 0xFFFFFFFF)
                uiStartPattern = 0;  
            c_pattern = uiStartPattern;
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(uint32_t *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                }
                if (c_pattern == 0xFFFFFFFF )  
                    c_pattern = uiStartPattern;
                else
                    c_pattern++;
                //HexDisplay(c_pattern);
            }
            break;
        case INCREMENTB:
            uiFails2 = 0;
            //bbu_printf("Generating INCREMENTB Pattern - Start Pattern %08X\n\r", c_zero);
            c_pattern = uiStartPattern;
            //c_pattern = c_zero;
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                }
                c_pattern += 0x04040404;
                if ( ( c_pattern & 0xFF ) == 0 )  
                    c_pattern = uiStartPattern;
                    //HexDisplay(c_pattern);
            }
            break;

        case MILLER_EFFECT:
            uiFails2 = 0;
            //bbu_printf("Generating MILLER_EFFECT ");
            c_pattern = 0x55555555;
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != (c_pattern))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(c_pattern),(uiTemp ^ (c_pattern)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(c_pattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (c_pattern);
                    //////bbu_printf("i %x (%x)\n\r",i,*(int *)(uiSourceAddress +(i*4)));
                }
                i++;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != (~c_pattern))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(~c_pattern),(uiTemp ^ (~c_pattern)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = (~c_pattern);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(~c_pattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (~c_pattern);
                    //////bbu_printf("i %x (%x)\n\r",i,*(int *)(uiSourceAddress +(i*4)));
                }
            }
            break;

        case ZERO_ROR_INVERT:
            uiFails2 = 0;
            //bbu_printf("Generating ZERO_ROR_INVERT Pattern");
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != (uiStartPattern))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern),(uiTemp ^ (uiStartPattern)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = (uiStartPattern);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (uiStartPattern);
                }
                i++;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != (~uiStartPattern))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(~uiStartPattern),(uiTemp ^ (~uiStartPattern)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = (~uiStartPattern);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(~uiStartPattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (~uiStartPattern);
                }
                uiStartPattern= ror32(uiStartPattern);
            }
            break;
        case ONE_ROL_INVERT:
            uiFails2 = 0;
            //bbu_printf("Generating ONE_ROL_INVERT Pattern");
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != (uiStartPattern))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern),(uiTemp ^ (uiStartPattern)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = (uiStartPattern);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (uiStartPattern);
                }
                i++;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != (~uiStartPattern))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(~uiStartPattern),(uiTemp ^ (~uiStartPattern)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = (~uiStartPattern);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(~uiStartPattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (~uiStartPattern);
                }
                uiStartPattern= rol32(uiStartPattern);
            }
          
        case ZERO_ROR:
            uiFails2 = 0;
            //bbu_printf("Generating ZERO_ROR Pattern");
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != (uiStartPattern))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern),(uiTemp ^ (uiStartPattern)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = (uiStartPattern);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (uiStartPattern);
                }
                uiStartPattern= ror32(uiStartPattern);
            }
          
        case ONE_ROL:
            uiFails2 = 0;
            //bbu_printf("Generating ONE_ROL Pattern");
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != (uiStartPattern))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern),(uiTemp ^ (uiStartPattern)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = (uiStartPattern);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,(uiStartPattern));
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = (uiStartPattern);
                }
                uiStartPattern= rol32(uiStartPattern);
            }

        case WALKING_ONES:
            uiFails2 = 0;
            //bbu_printf("Generating WALKING_ONES Pattern");
            i=0;
            for(j=0;j<uiTransSize;j++ )
            {
                if(bCheck)
                {
                    uiTemp = *(uint32_t *)(uiSourceAddress +(j*4));
                    if(uiTemp != ((uint32_t)(1<<i)))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(j*4)),uiTemp,(1<<i),(uiTemp ^ (1<<i)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = (1<<i);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(j*4)),uiTemp,(1<<i));
                }
                else
                {
                    ////bbu_printf("i: %d - %08X\n\r",i,(int)(1<<i));
                    *(int *)(uiSourceAddress +(j*4)) = (int)(1<<i);
                    ////bbu_printf("j %d [0x%08X] (0x%08X)\n\r",j,(uiSourceAddress +(j*4)),*(int *)(uiSourceAddress +(j*4)));
                }
                i++;
                if(i>=32) i=0;
            }
            break;
        case WALKING_ZEROS: 
            uiFails2 = 0;
            //bbu_printf("Generating WALKING_ZEROS Pattern");
            i=0;
            for(j=0;j<uiTransSize;j++ )
            {
                if(bCheck)
                {
                    uiTemp = *(uint32_t *)(uiSourceAddress +(j*4));
                    if(uiTemp != (uint32_t)(~(1<<i)))
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(j*4)),uiTemp,~(1<<i),(uiTemp ^ ~(1<<i)),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = ~(1<<i);
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(j*4)),uiTemp,~(1<<i));
                }
                else
                {
                    ////bbu_printf("i: %d - %08X\n\r",i,(int)~(1<<i));
                    *(int *)(uiSourceAddress +(j*4)) = (int)~(1<<i);
                }
                i++;
                if(i>=32) i=0;
                }
            break;
        case SSTAILGATE_0101XXXX:  //SSTAILGATE 0101XXXX
        case SSTAILGATE_01010000:  //SSTAILGATE 0101XXXX
        case SSTAILGATE_01011111:  //SSTAILGATE 0101XXXX
        case MSTAILGATE:
            uiFails2 = 0;
            //bbu_printf("Generating SSTAILGATE_0101 Pattern");
            if(uiPattern == SSTAILGATE_01010000)
                uiStartPattern = 0x00000000;
            else if(uiPattern == SSTAILGATE_01011111)
                uiStartPattern = 0x00000001;
            else if(uiPattern == MSTAILGATE)   
                uiStartPattern = 0xFFFFFFFF;
            for(i=0;i<uiTransSize;i++)
            {
                c_pattern = ( uiStartPattern );
                c_pattern_or_signal = ( uiStartPattern | test_signal );
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_zero)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero,(uiTemp ^ c_zero),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_zero;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_zero;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern_or_signal)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern_or_signal,(uiTemp ^ c_pattern_or_signal),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern_or_signal;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern_or_signal);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern_or_signal;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_zero)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero,(uiTemp ^ c_zero),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_zero;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_zero;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern_or_signal)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern_or_signal,(uiTemp ^ c_pattern_or_signal),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern_or_signal;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern_or_signal);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern_or_signal;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                }
                uiStartPattern = rol32( uiStartPattern );
                test_signal = rol32( test_signal );
            }
            break;
        case SSTAILGATE_01011010:  //SSTAILGATE 01011010
            uiFails2 = 0;
            //bbu_printf("Generating SSTAILGATE_1100 Pattern");
            uiStartPattern = 0x00000001;
            for(i=0;i<uiTransSize;i++)
            {
                c_pattern = ( uiStartPattern );
                c_pattern_or_signal = ( uiStartPattern | test_signal );
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_zero)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero,(uiTemp ^ c_zero),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_zero;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_zero;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern_or_signal)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern_or_signal,(uiTemp ^ c_pattern_or_signal),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern_or_signal;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern_or_signal);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern_or_signal;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_zero)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero,(uiTemp ^ c_zero),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_zero;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_zero;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern_or_signal)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern_or_signal,(uiTemp ^ c_pattern_or_signal),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern_or_signal;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern_or_signal);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern_or_signal;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_zero)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero,(uiTemp ^ c_zero),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_zero;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_zero;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                }
                i++;
                if(i>=uiTransSize) break;
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_zero)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero,(uiTemp ^ c_zero),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_zero;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_zero;
                }
                uiStartPattern = rol32( uiStartPattern );
                test_signal = rol32( test_signal );
            }
            break;
        case FREQ_SWEEP:
            uiFails2 = 0;
            //bbu_printf("Generating FREQ_SWEEP Pattern");
            i = 0;
            c_pattern = ( c_zero );
            c_pattern_or_signal = ( ~c_zero );
            for(j=1;j<uiTransSize/2;j++)
            {
                k=0;
                while(k<j)
                {
                    if(bCheck)
                    {
                        uiTemp = *(int *)(uiSourceAddress +(i*4));
                        if(uiTemp != c_pattern)
                        {
                            //HexDisplay(0x20006FF6);
                            //*(int *)guiCSBaseAddress = 0x20006FF6;
                            uiFails++; 
                            uiFails2++;
                            if((uiFails2<= uiLogFailures) )
                                bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                            if(uiFails2 == uiLogFailures+1)
                                bbu_printf(SV_ERROR_LOG);
                            expected = c_pattern;
                            for(bit=0;bit<32;bit++)
                            {
                                if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                    giBitFails[bit%32]++;            
                            }
                            for(byte=0;byte<4;byte++)
                            {
                                if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                    giByteLaneFails[byte%4]++;
                            }
                        }
                        else
                            pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                    }
                    else
                    {
                        *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                        ////bbu_printf("i(%d)[0x%08X]= %08X\n\r",i,(uiSourceAddress + (i*4)),c_pattern);
                    }
                    k++;
                    i++;
                    if(i >= uiTransSize)
                        break;
                }
                if(i >= uiTransSize)
                    break;
                k=0;
                while(k<j)
                {
                    if(bCheck)
                    {
                        uiTemp = *(int *)(uiSourceAddress +(i*4));
                        if(uiTemp != c_pattern_or_signal)
                        {
                            //HexDisplay(0x20006FF6);
                            //*(int *)guiCSBaseAddress = 0x20006FF6;
                            uiFails++; 
                            uiFails2++;
                            if((uiFails2<= uiLogFailures) )
                                bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern_or_signal,(uiTemp ^ c_pattern_or_signal),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                            if(uiFails2 == uiLogFailures+1)
                                bbu_printf(SV_ERROR_LOG);
                            expected = c_pattern_or_signal;
                            for(bit=0;bit<32;bit++)
                            {
                                if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                    giBitFails[bit%32]++;            
                            }
                            for(byte=0;byte<4;byte++)
                            {
                                if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                    giByteLaneFails[byte%4]++;
                            }
                        }
                        else
                            pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern_or_signal);
                    }
                    else
                    {
                        *(int *)(uiSourceAddress +(i*4)) = c_pattern_or_signal;
                            ////bbu_printf("i(%d)[0x%08X]= %08X\n\r",i,(uiSourceAddress + (i*4)),c_pattern_or_signal);
                    }
                    k++;
                    i++;
                    if(i >= uiTransSize)
                        break;
                }
                if(i >= uiTransSize)
                    break;
            }
            break;
        case VERT_INCREMENT:
            uiFails2 = 0;
            //bbu_printf("Generating VERT_INCREMENT Pattern");
            c_pattern = ( uiStartPattern );
            for(i=0;i<uiTransSize;i++)
            {
                if(bCheck)
                {
                    uiTemp = *(int *)(uiSourceAddress +(i*4));
                    if(uiTemp != c_pattern)
                    {
                        //HexDisplay(0x20006FF6);
                        //*(int *)guiCSBaseAddress = 0x20006FF6;
                        uiFails++; 
                        uiFails2++;
                        if((uiFails2<= uiLogFailures) )
                            bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                        if(uiFails2 == uiLogFailures+1)
                            bbu_printf(SV_ERROR_LOG);
                        expected = c_pattern;
                        for(bit=0;bit<32;bit++)
                        {
                            if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                giBitFails[bit%32]++;            
                        }
                        for(byte=0;byte<4;byte++)
                        {
                            if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                giByteLaneFails[byte%4]++;
                        }
                    }
                    else
                        pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                }
                else
                {
                    *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                }
                c_pattern++;
            }
            break;
        case HORZ_INCREMENT:
            uiFails2 = 0;
            //bbu_printf("Generating HORZ_INCREMENT Pattern");
            c_pattern = 0x00000001;
            i = 0;
            for(k=0;k<(uiTransSize/16);k++)                                                                                           
            {
                for(j=0;j<16;j++)
                {
                    if((j%2)==1)
                    {               // ODD
                        if(bCheck)
                        {
                            uiTemp = *(int *)(uiSourceAddress +(i*4));
                            if(uiTemp != c_pattern)
                            {
                                //HexDisplay(0x20006FF6);
                                //*(int *)guiCSBaseAddress = 0x20006FF6;
                                uiFails++; 
                                uiFails2++;
                                if((uiFails2<= uiLogFailures) )
                                    bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                                if(uiFails2 == uiLogFailures+1)
                                    bbu_printf(SV_ERROR_LOG);
                                expected = c_pattern;
                                for(bit=0;bit<32;bit++)
                                {
                                    if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                        giBitFails[bit%32]++;            
                                }
                                for(byte=0;byte<4;byte++)
                                {
                                    if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                        giByteLaneFails[byte%4]++;
                                }
                            }
                            else
                                pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                        }
                        else
                        {
                            *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                            ////bbu_printf("i(%d) - j(%d) = %08X\n\r",i,j,c_pattern);
                        }
                        i++;
                    }
                    else
                    {                                      // EVEN
                        if(bCheck)
                        {
                            uiTemp = *(int *)(uiSourceAddress +(i*4));
                            if(uiTemp != c_zero)
                            {
                                //HexDisplay(0x20006FF6);
                                //*(int *)guiCSBaseAddress = 0x20006FF6;
                                uiFails++; 
                                uiFails2++;
                                if((uiFails2<= uiLogFailures) )
                                    bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero,(uiTemp ^ c_zero),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                                if(uiFails2 == uiLogFailures+1)
                                    bbu_printf(SV_ERROR_LOG);
                                expected = c_zero;
                                for(bit=0;bit<32;bit++)
                                {
                                    if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                        giBitFails[bit%32]++;            
                                }
                                for(byte=0;byte<4;byte++)
                                {
                                    if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                        giByteLaneFails[byte%4]++;
                                }
                            }
                            else
                                pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero);
                        }
                        else
                        {
                            *(int *)(uiSourceAddress +(i*4)) = c_zero;
                            ////bbu_printf("i(%d) - j(%d) = %08X\n\r",i,j,c_zero);
                        }
                        i++;
                    }
                    if(i >= uiTransSize)
                        break;
                    if(((j%4)/2)==1)
                    {        // {2,3,6,7,10,11,14,15}
                        if(bCheck)
                        {
                            uiTemp = *(int *)(uiSourceAddress +(i*4));
                            if(uiTemp != c_pattern)
                            {
                                //HexDisplay(0x20006FF6);
                                //*(int *)guiCSBaseAddress = 0x20006FF6;
                                uiFails++; 
                                uiFails2++;
                                if((uiFails2<= uiLogFailures) )
                                    bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                                if(uiFails2 == uiLogFailures+1)
                                    bbu_printf(SV_ERROR_LOG);
                                expected = c_pattern;
                                for(bit=0;bit<32;bit++)
                                {
                                    if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                        giBitFails[bit%32]++;            
                                }
                                for(byte=0;byte<4;byte++)
                                {
                                    if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                        giByteLaneFails[byte%4]++;
                                }
                            }
                            else
                                pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                        }
                        else
                        {
                            *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                            ////bbu_printf("i(%d) - j(%d) = %08X\n\r",i,j,c_pattern);
                        }
                        i++;
                    }
                    else
                    {                                      // {0,1,4,5,8,9,12,13}
                        if(bCheck)
                        {
                            uiTemp = *(int *)(uiSourceAddress +(i*4));
                            if(uiTemp != c_zero)
                            {
                                //HexDisplay(0x20006FF6);
                                //*(int *)guiCSBaseAddress = 0x20006FF6;
                                uiFails++; 
                                uiFails2++;
                                if((uiFails2<= uiLogFailures) )
                                    bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero,(uiTemp ^ c_zero),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                                if(uiFails2 == uiLogFailures+1)
                                    bbu_printf(SV_ERROR_LOG);
                                expected = c_zero;
                                for(bit=0;bit<32;bit++)
                                {
                                    if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                        giBitFails[bit%32]++;            
                                }
                                for(byte=0;byte<4;byte++)
                                {
                                    if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                        giByteLaneFails[byte%4]++;
                                }
                            }
                            else
                                pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero);
                        }
                        else
                        {
                            *(int *)(uiSourceAddress +(i*4)) = c_zero;
                            ////bbu_printf("i(%d) - j(%d) = %08X\n\r",i,j,c_zero);
                        }
                        i++;
                    }
                    if(i >= uiTransSize)
                        break;
                    if(((j%8)/4)==1)
                    {        // {4,5,6,7,12,13,14,15}
                        if(bCheck)
                        {
                            uiTemp = *(int *)(uiSourceAddress +(i*4));
                            if(uiTemp != c_pattern)
                            {
                                //HexDisplay(0x20006FF6);
                                //*(int *)guiCSBaseAddress = 0x20006FF6;
                                uiFails++; 
                                uiFails2++;
                                if((uiFails2<= uiLogFailures) )
                                    bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                                if(uiFails2 == uiLogFailures+1)
                                    bbu_printf(SV_ERROR_LOG);
                                expected = c_pattern;
                                for(bit=0;bit<32;bit++)
                                {
                                    if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                        giBitFails[bit%32]++;            
                                }
                                for(byte=0;byte<4;byte++)
                                {
                                    if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                        giByteLaneFails[byte%4]++;
                                }
                            }
                            else
                                pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                        }
                        else
                        {
                            *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                            ////bbu_printf("i(%d) - j(%d) = %08X\n\r",i,j,c_pattern);
                        }
                        i++;
                    }
                    else
                    {                                      // {0,1,2,3,8,9,10,11}
                        if(bCheck)
                        {
                            uiTemp = *(int *)(uiSourceAddress +(i*4));
                            if(uiTemp != c_zero)
                            {
                                //HexDisplay(0x20006FF6);
                                //*(int *)guiCSBaseAddress = 0x20006FF6;
                                uiFails++; 
                                uiFails2++;
                                if((uiFails2<= uiLogFailures) )
                                    bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero,(uiTemp ^ c_zero),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                                if(uiFails2 == uiLogFailures+1)
                                    bbu_printf(SV_ERROR_LOG);
                                expected = c_zero;
                                for(bit=0;bit<32;bit++)
                                {
                                    if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                        giBitFails[bit%32]++;            
                                }
                                for(byte=0;byte<4;byte++)
                                {
                                    if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                        giByteLaneFails[byte%4]++;
                                }
                            }
                            else
                                pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero);
                        }
                        else
                        {
                            *(int *)(uiSourceAddress +(i*4)) = c_zero;
                            ////bbu_printf("i(%d) - j(%d) = %08X\n\r",i,j,c_zero);
                        }
                        i++;
                    }
                    if(i >= uiTransSize)
                        break;
                    if((j/8)==1)
                    {                // {8 - 15}
                        if(bCheck)
                        {
                            uiTemp = *(int *)(uiSourceAddress +(i*4));
                            if(uiTemp != c_pattern)
                            {
                                //HexDisplay(0x20006FF6);
                                //*(int *)guiCSBaseAddress = 0x20006FF6;
                                uiFails++; 
                                uiFails2++;
                                if((uiFails2<= uiLogFailures) )
                                    bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern,(uiTemp ^ c_pattern),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                                if(uiFails2 == uiLogFailures+1)
                                    bbu_printf(SV_ERROR_LOG);
                                expected = c_pattern;
                                for(bit=0;bit<32;bit++)
                                {
                                    if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                        giBitFails[bit%32]++;            
                                }
                                for(byte=0;byte<4;byte++)
                                {
                                    if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                        giByteLaneFails[byte%4]++;
                                }
                            }
                            else
                                pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_pattern);
                        }
                        else
                        {
                            *(int *)(uiSourceAddress +(i*4)) = c_pattern;
                            ////bbu_printf("i(%d) - j(%d) = %08X\n\r",i,j,c_pattern);
                        }
                        i++;
                    }
                    else
                    {                                      // {0 - 7}
                        if(bCheck)
                        {
                            uiTemp = *(int *)(uiSourceAddress +(i*4));
                            if(uiTemp != c_zero)
                            {
                                //HexDisplay(0x20006FF6);
                                //*(int *)guiCSBaseAddress = 0x20006FF6;
                                uiFails++; 
                                uiFails2++;
                                if((uiFails2<= uiLogFailures) )
                                    bbu_printf("FAIL - pattern %d [@0x%08X]=0x%08X Expect 0x%08X - Diff 0x%08X - Data before 0x%08X - after 0x%08X\n\r",uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero,(uiTemp ^ c_zero),*(int *)(uiSourceAddress +((i-1)*4)),*(int *)(uiSourceAddress +((i+1)*4)));
                                if(uiFails2 == uiLogFailures+1)
                                    bbu_printf(SV_ERROR_LOG);
                                expected = c_zero;
                                for(bit=0;bit<32;bit++)
                                {
                                    if((uiTemp & (1<<bit)) != (expected & (1<<bit)))
                                        giBitFails[bit%32]++;            
                                }
                                for(byte=0;byte<4;byte++)
                                {
                                    if((uiTemp & (0xFF<<(byte*8))) != (expected & (0xFF<<(byte*8))))
                                        giByteLaneFails[byte%4]++;
                                }
                            }
                            else
                                pass_print_log(uiPattern,(uiSourceAddress +(i*4)),uiTemp,c_zero);
                        }
                        else
                        {
                            *(int *)(uiSourceAddress +(i*4)) = c_zero;
                            ////bbu_printf("i(%d) - j(%d) = %08X\n\r",i,j,c_zero);
                        }
                        i++;
                    }
                    if(i >= uiTransSize)
                        break;
                }
                if(i >= uiTransSize)
                    break;
                c_pattern = rol32(c_pattern);
            }
            break;  
 
        default:
            //bbu_printf("No Pattern option %x\n\r",uiPattern);
            break;
        }
        if(uiFails)
        {
            //giPatternNumOfFails[uiPattern]+=uiFails;
            //giPatternFails[uiPattern]++;
        }
    return uiFails;
}

static void bbu_mgwp_help(void)
{
    bbu_printf("mgwp: MCU generate word pattern test\n\r");
    bbu_printf(" usage:\n\r");
    bbu_printf("   mgwp <src_addr> <pattern> <size> <start_pattern> [<check>]\n\r");
    bbu_printf(" <pattern>:\n\r");
    bbu_printf("    1   -- ANALOG              \n\r"); 
    bbu_printf("    2   -- CONSTANT            \n\r"); 
    bbu_printf("    3   -- COMPLEMENT          \n\r"); 
    bbu_printf("    4   -- DECREMENT           \n\r"); 
    bbu_printf("    5   -- INCREMENT           \n\r"); 
    bbu_printf("    6   -- INCREMENTB          \n\r"); 
    bbu_printf("    7   -- MILLER_EFFECT       \n\r"); 
    bbu_printf("    8   -- ZERO_ROR_INVERT     \n\r"); 
    bbu_printf("    9   -- ONE_ROL_INVERT      \n\r"); 
    bbu_printf("    10  -- ZERO_ROR            \n\r"); 
    bbu_printf("    11  -- ONE_ROL             \n\r"); 
    bbu_printf("    12  -- WALKING_ONES        \n\r"); 
    bbu_printf("    13  -- WALKING_ZEROS       \n\r"); 
    bbu_printf("    14  -- SSTAILGATE_0101XXXX \n\r"); 
    bbu_printf("    15  -- SSTAILGATE_01010000 \n\r"); 
    bbu_printf("    16  -- SSTAILGATE_01011111\n\r"); 
    bbu_printf("    17  -- SSTAILGATE_01011010\n\r"); 
    bbu_printf("    18  -- MSTAILGATE         \n\r"); 
    bbu_printf("    19  -- FREQ_SWEEP         \n\r"); 
    bbu_printf("    20  -- VERT_INCREMENT     \n\r"); 
    bbu_printf("    21  -- HORZ_INCREMENT     \n\r"); 

}

int bbu_mcuGenerateWordPattern(int argc, uint8_t **argv)
{
    uint32_t src;
    uint32_t pattern;
    uint32_t size;
    uint32_t start_pattern;
    int bCheck = 0;
    int result;
    if(argc < 5)
    {
        bbu_mgwp_help();
        return SCREEN_TEST_PASS;
    }
    src = conv_hex((char *)argv[1]);
    pattern = conv_hex((char *)argv[2]);
    size = conv_hex((char *)argv[3]);
    start_pattern = conv_hex((char *)argv[4]);
    if(argc == 6)
        bCheck = conv_dec((char *)argv[5]);
    result = mcuGenerateWordPattern(src, pattern, size/4, start_pattern, bCheck);

    if(bCheck == 0){
        bbu_printf("Write Pattern PASS\n\r");
        return SCREEN_TEST_PASS;
    }

    if(result == 0)
    {
        bbu_printf("PASS: Read back patterns all match the ones written\n\r");
        return SCREEN_TEST_PASS;
    }else{
        bbu_printf("FAIL: Read back patterns don't match the ones written\n\r");
        return SCREEN_TEST_FIAL;
    }
}

