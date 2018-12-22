#include "usb_types.h"
#include "cp15.h"
#include "USB.h" //by tongchao
#include "cache_ops.h"
//#define __USB_OTG__

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _bsp_get_usb_base
*  Returned Value : Address of the VUSB register base
*  Comments       :
*        Get the USB register base address
*END*-----------------------------------------------------------------*/
uint_8 _bsp_get_usb_vector
(
    uint_8 device_number
)
{
    /* Body */
#ifdef __USB_HOST__
    if (device_number == 0)
    {
        return BSP_VUSB20_HOST_VECTOR0;
    } /* Endif */
#endif

#ifndef __USB_DEVICE__
    if (device_number == 0)
    {
        return BSP_VUSB20_DEVICE_VECTOR0;
    } /* Endif */
#endif

#ifdef __USB_OTG__
    if (device_number == 0)
    {
        return BSP_VUSB20_OTG_VECTOR0;
    } /* Endif */

#endif

    return 0;  // fl: clean warnings
} /* EndBody */
pointer _bsp_get_usb_base
(
    uint_8 device_number
)
{
    /* Body */
    if (device_number == 0)
    {
        return (pointer)BSP_VUSB20_HOST_BASE_ADDRESS0;
    } /* Endif */

} /* EndBody */


pointer  bsp_get_nevo_usb_base          //by tongchao
(
    uint_8 device_number
)


{
    /* Body */
    if (device_number == 0)
    {

        return MAVL_USB_OTG;// by tongchao                 //need be changed
    } /* Endif */
    return 0;
} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _bsp_get_usb_vector
*  Returned Value : interrupt vector number
*  Comments       :
*        Get the vector number for the specified device number
*END*-----------------------------------------------------------------*/

uint_8 bsp_get_usb_vector
(
    uint_8 device_number
)
{
    /* Body */
#ifdef __USB_HOST__
    if (device_number == 0)
    {
        return BSP_VUSB20_HOST_VECTOR0;
    } /* Endif */
#endif

    //#ifdef __USB_DEVICE__
    if (device_number == 0)
    {
        return BSP_VUSB20_DEVICE_VECTOR0;
    } /* Endif */
    //#endif

#ifdef __USB_OTG__
    if (device_number == 0)
    {
        return BSP_VUSB20_OTG_VECTOR0;
    } /* Endif */
#endif

    return 0;  // fl: clean warnings
} /* EndBody */

#if 0
void  USB_dev_memfree(void *aligned_ptr)
{

    MRVL_USB_memfree(aligned_ptr);
    return ;
}
void USB_dev_memcopy(void *ptr, void *dst, int n)
{
    uint_8_ptr srcptr = (uint_8_ptr)ptr;
    uint_8_ptr dstptr = (uint_8_ptr)dst;
    int i;
    for (i = 0; i < n; i++)
        dstptr[i] = srcptr[i];
}
#define _DATA_CACHE_      1
#ifdef _DATA_CACHE_



#define _USB_dcache_flush_mlines_(address)  {cp15_reg_write(c7, c10, 1, address);l2_cache_clean_line(address)  ;}     //need be changed
#define _USB_dcache_invalidate_mlines_(address) {cp15_reg_write(c7, c6, 1, address);l2_cache_clean_invalidate_line(address) ;}
//need be changed


#else
#define _USB_dcache_flush_mlines_
#define _USB_dcache_invalidate_mlines_
#endif
#define     SIZEOF_CACHE_LINE       32
#define     CACHE_LINE_MASK         (SIZEOF_CACHE_LINE-1)
#define CACHED_TO_NON_CACHED_OFFSET 0x00100000

#define RETURN_IF_NON_CACHED(address)                   { \
        if ( address >= CACHED_TO_NON_CACHED_OFFSET ) return;  }

void USB_dcache_flush_mlines( void *pMem , uint_32 size)
{
#if 1
    uint_32
    address = (uint_32)pMem & ~CACHE_LINE_MASK,
    end_address ;



    for ( end_address = (uint_32)pMem + size ; address < end_address ; address += SIZEOF_CACHE_LINE )
        _USB_dcache_flush_mlines_( address ) ;
#else
    _USB_dcache_flush_mlines_
#endif
}
void USB_dcache_invalidate_mlines( void *pMem, uint_32 size)
{
#if 1
    uint_32
    address = (uint_32)pMem & ~CACHE_LINE_MASK,

    end_address = (uint_32)pMem + size,
    last_address_to_invalidate = end_address & ~CACHE_LINE_MASK ;



    if ( address != (uint_32)pMem )
    {
        ///   cpsr = disableInterrupts() ;
        _USB_dcache_flush_mlines_( address ) ;
        _USB_dcache_invalidate_mlines_( address ) ;
        ///  restoreInterrupts( cpsr ) ;
        address += SIZEOF_CACHE_LINE ;
    }

    for ( ; address < last_address_to_invalidate ; address += SIZEOF_CACHE_LINE )
        _USB_dcache_invalidate_mlines_( address ) ;

    if ( last_address_to_invalidate != end_address )
    {
        /// cpsr = disableInterrupts() ;
        _USB_dcache_flush_mlines_( address ) ;
        _USB_dcache_invalidate_mlines_( address ) ;
        ///    restoreInterrupts( cpsr ) ;
    }

#else
    _USB_dcache_invalidate_mlines_
#endif
}

#endif
void USB_dev_memzero(void *ptr, int n)
{
    uint_8_ptr u8ptr = (uint_8_ptr)ptr;
    int i;

    for (i = 0; i < n; i++)
        u8ptr[i] = 0;
}





