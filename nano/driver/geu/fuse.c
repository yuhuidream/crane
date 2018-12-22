#include "predefines.h"
#include "SFO.h"
#include "APBCLK.h"
#include "GEU.h"
#include "PMUA.h"
#include "USBUTMI.h"
#include "timer_dec.h"
#include "geu_fuse.h"

extern void usb_clk_enable(void);

void decodeLotChandler(uint32_t uiLot, char* cLot);
void infEmeiReadPartInfo(void);
/**
* "private" function for reading and logging Emei/Kunlun ULT/DRO and Profile (if supported).
*
* \returns none
*/
uint32_t uiDRO_LVT = 0, uiDRO_SVT = 0, uiSVCRev = 0, uiProfile = 0;
void infEmeiReadPartInfo(void)
{
    // ULT: read, print and store off statistics
    uint32_t uiManFuses_31_00 = GEU->FUSE_MANU_PARA_0;    //Fuse Block 0 127:96
    uint32_t uiManFuses_63_32 = GEU->FUSE_MANU_PARA_1;    //Fuse Block 0 159:128
    uint32_t uiManFuses_95_64 = GEU->FUSE_MANU_PARA_2;    //Fuse Block 0 191:160
    uint32_t uiblock0_rsv1 = GEU->BLOCK0_RESERVED_1; //Fuse Block 0 255:224
    uint32_t uiManFuses_127_96 = GEU->BLOCK7_RESERVED_3; //Fuse Block 7 127:96, for Z12
    uint32_t uiLot = 0, uiWafer = 0, uiX = 0, uiY = 0, uiParity = 0, uiFuses = 0;
    char cLot[7] = {0};

    if(cpu_is_pxa1826_z1z2())
    {
        uiManFuses_31_00 = GEU->BLOCK7_RESERVED_0;    //Fuse Block 7 31:0
        uiManFuses_63_32 = GEU->BLOCK7_RESERVED_1;    //Fuse Block 7 63:32
        uiManFuses_95_64 = GEU->BLOCK7_RESERVED_2;    //Fuse Block 7 95:64
    }
    uiLot       = ((uiManFuses_63_32 & 0x3) << 29) | ((uiManFuses_31_00 >> 3) & 0x1fffffff);
    uiWafer     =  (uiManFuses_63_32 >>  2) & 0x1f;
    uiX         =  (uiManFuses_63_32 >>  7) & 0x7f;
    uiY         =  (uiManFuses_63_32 >> 15) & 0x7f;
    uiParity    =  (uiManFuses_63_32 >> 23) & 0x1;

    if(cpu_is_pxa1826_z1z2())
    {
        uiDRO_LVT  =  uiManFuses_95_64 & 0x3ff;
        uiDRO_SVT  =  (uiManFuses_95_64 >> 22) & 0x3ff;

        if (uiDRO_SVT == 0) //parts were improperly fused
        {
            uiDRO_LVT   =  (uiManFuses_95_64 >> 11) & 0x3ff;
            uiDRO_SVT  =  (uiManFuses_127_96 >> 1) & 0x3ff;
            if (uiDRO_SVT)
            {
                bbu_printf("This is a part fused incorrectly with dro values computed@1.1v, converted to value @normal 1.05v\r\n");
                uiDRO_LVT = uiDRO_LVT * 913 / 1000;
                uiDRO_SVT = uiDRO_SVT * 88 / 100;
            }
            else
            {
                bbu_printf("\r\nThis is a unfused chip...\r\n");
            }
        }
        else if ( (uiManFuses_95_64 >> 11) & 0x3ff)
        {
            bbu_printf("This is a refused part\r\n");
        }
        else
        {
            bbu_printf("This is a part fused correctly\r\n");
        }
    }
    else //Z3 A0
    {
        uiDRO_LVT   =  (uiManFuses_95_64 >> 4) & 0x3ff;
        uiDRO_SVT  =  ((uiManFuses_95_64 & 0xF) << 6) + ((uiManFuses_63_32 >> 26) & 0x3f);
        uiSVCRev = (uiblock0_rsv1 >> 13) & 0x00000007;
        uiFuses = (uiblock0_rsv1 >> 16) & 0x0000FFFF;
    }

    bbu_printf("  \n\r");
    bbu_printf("     ***************************\r\n");
    bbu_printf("     *  ULT: %08X%08X  *\r\n", uiManFuses_63_32, uiManFuses_31_00);
    decodeLotChandler(uiLot, cLot);
    bbu_printf("     ***************************\n\r");
    bbu_printf("     *  Lot   = %s\n\r",      cLot);
    bbu_printf("     *  wafer = %d\n\r",    uiWafer);
    bbu_printf("     *  x     = %d\n\r",        uiX);
    bbu_printf("     *  y     = %d\n\r",        uiY);
    bbu_printf("     *  parity= %d\n\r",   uiParity);
    uiProfile = getprofile(uiFuses);
    bbu_printf("     ***************************\n\r");
    bbu_printf("     *  SVT_DRO = %d\n\r",  uiDRO_SVT);
    bbu_printf("     *  SVC Revision = %d\n\r",  uiSVCRev);
    bbu_printf("     *  SVC Profile = %d\n\r",  uiProfile);
    bbu_printf("     ***************************\n\r");


    return;
}

/**
 * "private" function that decodes the Lot using the "Intel" method
 *  Projects that use this currently are Chandler products: MMP3, Nezha2, HelanLTE etc.
 *
 * \returns cLot - char* that contains the Lot string
 */
void decodeLotChandler(uint32_t uiLot, char* cLot)
{
    unsigned int uiTemp, i=0;

    uiTemp = (uiLot >> 26) & 0x1f;
    cLot[i++] = (char)(uiTemp+64);

    uiTemp = (uiLot >> 20) & 0x3f;
    if(uiTemp & 0x20)
    {
        uiTemp &= ~0x20;
        cLot[i++] = (char)(uiTemp+64);
    }
    else
    {
        cLot[i++] = (char)(uiTemp+(int)('0'));
    }

    uiTemp = (uiLot >> 14) & 0x3f;
    if(uiTemp & 0x20)
    {
        uiTemp &= ~0x20;
        cLot[i++] = (char)(uiTemp+64);
    }
    else
    {
        cLot[i++] = (char)(uiTemp+(int)('0'));
    }

    uiTemp = (uiLot >>  8) & 0x3f;
    if(uiTemp & 0x20)
    {
        uiTemp &= ~0x20;
        cLot[i++] = (char)(uiTemp+64);
    }
    else
    {
        cLot[i++] = (char)(uiTemp+(int)('0'));
    }

    uiTemp = uiLot & 0xff;
    if((uiTemp & 0xC0) == 0xC0)
    {//UMC or Samsung
        uiTemp &= ~0xC0;
        if(uiTemp & 0x20)
        {
            uiTemp &= ~0x20;
            cLot[i++] = (char)(uiTemp+64);
        }
        else
        {
            cLot[i++] = (char)(uiTemp+(int)('0'));
        }
        cLot[i++] = (char)((int)('0'));
    }
    else
    {
        cLot[i++] = (char)((uiTemp & 0xf) + (int)('0'));
        cLot[i++] = (char)(((uiTemp >> 4) & 0xf) +(int)('0'));
    }

    return;
}


