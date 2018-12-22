#include "usb_devapi.h"
#include "usb_prv_dev.h"
#include "usb_device.h"
#include  "string.h"

#include "predefines.h"    //by tongchao
#include "cp15.h"        //by tongchao
#include "cache_ops.h"//by tongchao
#include "mmu.h"//by tongchao
#include "set_mmu.h"
#include "usb_mv8618malloc.h"
extern Pagetable nevobbuPT ;



#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif

//#define  _DEVICE_DEBUG_

#ifndef  CONFIG_SCREEN_CHIP
//#define  _DEVICE_DEBUG_
#endif
extern void           *NONE_Cache_ADDR   ;
extern uint_8 usb_dci_vusb20_init(uint_8, usb_device_handle);
extern void usb_device_free_XD(pointer);
extern void usb_dci_vusb20_free_dTD(pointer);
extern uint_8 usb_dci_vusb20_add_dTD(usb_device_handle, XD_STRUCT_PTR);
extern uint_8 usb_dci_vusb20_send_data(usb_device_handle, XD_STRUCT_PTR);
extern uint_8 usb_dci_vusb20_recv_data(usb_device_handle, XD_STRUCT_PTR);
extern uint_8 usb_dci_vusb20_cancel_transfer(usb_device_handle, uint_8, uint_8);
extern uint_8 usb_dci_vusb20_get_transfer_status(usb_device_handle, uint_8, uint_8);
extern XD_STRUCT_PTR usb_dci_vusb20_get_transfer_details(usb_device_handle, uint_8, uint_8);
extern void usb_dci_vusb20_set_address(usb_device_handle, uint_8);
extern void usb_dci_vusb20_get_setup_data(usb_device_handle,
        uint_8, uchar_ptr);
extern void usb_dci_vusb20_assert_resume(usb_device_handle);
extern uint_8 usb_dci_vusb20_init_endpoint(usb_device_handle, XD_STRUCT_PTR);
extern void usb_dci_vusb20_stall_endpoint(usb_device_handle, uint_8, uint_8);
extern void usb_dci_vusb20_unstall_endpoint(usb_device_handle, uint_8, uint_8);
extern uint_8 usb_dci_vusb20_deinit_endpoint(usb_device_handle, uint_8, uint_8);
extern void usb_dci_vusb20_set_endpoint_status(usb_device_handle, uint_8,
        uint_8);
extern void usb_dci_vusb20_set_test_mode(usb_device_handle, uint_16);
extern uint_8  usb_dci_vusb20_get_endpoint_status(usb_device_handle, uint_8);
extern void usb_dci_vusb20_chip_initialize(usb_device_handle);

extern void USB_dev_memzero(void *ptr, int n);
extern void usb_isr_disable(void);
extern void  USB_dev_memfree(void *aligned_ptr);
extern void service_ep0
(
    /* [IN] Handle of the USB device */
    usb_device_handle   handle,

    /* [IN] Is it a setup packet? */
    boolean              setup,

    /* [IN] Direction of the transfer.  Is it transmit? */
    uint_8               direction,

    /* [IN] Pointer to the data buffer */
    uint_8_ptr           buffer,

    /* [IN] Length of the transfer */
    uint_32              length,

    /* [IN] Error, if any */
    uint_8               error

);

extern void service_ep1
(
    usb_device_handle   handle,     /* [IN] Handle of the USB device */
    boolean              setup,         /* [IN] Is it a setup packet? */
    uint_8               direction,     /* [IN] Direction of the transfer.  Is it transmit? */
    uint_8_ptr           buffer,        /* [IN] Pointer to the data buffer */
    uint_32              length,        /* [IN] Length of the transfer */
    uint_8               error          /* [IN] Error, if any */
);

void reset_ep0
(
    /* [IN] Handle of the USB device */
    usb_device_handle   handle,

    /* [IN] Unused */
    boolean              setup,

    /* [IN] Unused */
    uint_8               direction,

    /* [IN] Unused */
    uint_8_ptr           buffer,

    /* [IN] Unused */
    uint_32              length,

    /* [IN] Error, if any */
    uint_8               error

);

extern void service_speed
(
    /* [IN] Handle of the USB device */
    usb_device_handle   handle,

    /* [IN] Unused */
    boolean              setup,

    /* [IN] Unused */
    uint_8               direction,

    /* [IN] Unused */
    uint_8_ptr           buffer,

    /* [IN] Unused */
    uint_32              length,

    /* [IN] Error, if any */
    uint_8               error

);

// callbacks for iso mode test -- speaker.c
extern void iso_service_ep0
(
    usb_device_handle   handle,
    boolean              setup,
    uint_8               direction,
    uint_8_ptr           buffer,
    uint_32              length,
    uint_8               error
);

extern void iso_service_ep1
(
    usb_device_handle   handle,     /* [IN] Handle of the USB device */
    boolean              setup,         /* [IN] Is it a setup packet? */
    uint_8               direction,     /* [IN] Direction of the transfer.  Is it transmit? */
    uint_8_ptr           buffer,        /* [IN] Pointer to the data buffer */
    uint_32              length,        /* [IN] Length of the transfer */
    uint_8               error          /* [IN] Error, if any */
);

void iso_reset_ep0
(
    /* [IN] Handle of the USB device */
    usb_device_handle   handle,

    /* [IN] Unused */
    boolean              setup,

    /* [IN] Unused */
    uint_8               direction,

    /* [IN] Unused */
    uint_8_ptr           buffer,

    /* [IN] Unused */
    uint_32              length,

    /* [IN] Error, if any */
    uint_8               error

);

extern unsigned int usb_dev_type;

#ifdef __VUSBHS__
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_free_XD
*  Returned Value : void
*  Comments       :
*        Enqueues a XD onto the free XD ring.
*
*END*-----------------------------------------------------------------*/

void usb_device_free_XD
(
    /* [IN] the dTD to enqueue */
    pointer  xd_ptr
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)
            (((XD_STRUCT_PTR)xd_ptr)->SCRATCH_PTR->PRIVATE);

    /*
    ** This function can be called from any context, and it needs mutual
    ** exclusion with itself.
    */

#ifdef _DEVICE_DEBUG_
    //bbu_printf("_usb_device_free_XD.\r\n");
#endif

    USB_lock();

    /*
    ** Add the XD to the free XD queue (linked via PRIVATE) and
    ** increment the tail to the next descriptor
    */
    if (usb_dev_ptr->XD_ENTRIES < 32)
    {
        USB_XD_QADD(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL,
                    (XD_STRUCT_PTR)xd_ptr);
        usb_dev_ptr->XD_ENTRIES++;
        //bbu_printf("usb_dev_ptr->XD_ENTRIES is %d\r\n",usb_dev_ptr->XD_ENTRIES);
    }
    USB_unlock();
#ifdef _DEVICE_DEBUG_
    // bbu_printf("_usb_device_free_XD, SUCCESSFUL.\r\n");
#endif

} /* Endbody */

#endif


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_device_init
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device specific data structures and calls
*  the low-level device controller chip initialization routine.
*
*END*-----------------------------------------------------------------*/

//Region usb_XD_SCRATCH_STRUCT_BASE_Region = {0x8AE00000, 1024, 2, RWNA,0, 0x8AE00000,&nevobbuPT};//by tongchao fix cache bug

static USB_DEV_STATE_STRUCT  nanousb_dev_state_struct;
static XD_STRUCT   nanoxd_struct[MAX_XDS_FOR_TR_CALLS];
static XD_STRUCT   nanoTEMP_XD_PTR;
uint_8 usb_device_init
(
    /* [IN] the USB device controller to initialize */
    uint_8                    devnum,

    /* [OUT] the USB_USB_dev_initialize state structure */
    usb_device_handle _PTR_  handle,

    /* [IN] number of endpoints to initialize */
    uint_8                    endpoints
)
{
    /* Body */
#ifdef __VUSBHS__
    USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
    XD_STRUCT_PTR                    xd_ptr;
    uint_8                          temp, i, error;
    SCRATCH_STRUCT_PTR               temp_scratch_ptr;
    //  uint_32                          total_memory;
    // uint_8_ptr                       stack_mem_ptr;
    //   mmuMapRegion(&usb_XD_SCRATCH_STRUCT_BASE_Region);//by tongchao fix cache bug




    // mmu_set_region_ncnb(NONE_Cache_ADDR,2);


    //dcache_clean_all();
    // tlb_invalidate_all();
    //l1_dcache_clean_all();//by tongchao fix cache bug
    // l2_cache_clean_all();

#ifdef _usb_OS_MQX__
    USB_CALLBACK_FUNCTIONS_STRUCT_PTR call_back_table_ptr;
#endif

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_init\r\n");
#endif

#ifndef REDUCED_ERROR_CHECKING

    if (devnum > MAX_USB_DEVICES)   ///////step
    {
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_init, error invalid device numbe\r\n");
#endif
        return USBERR_INVALID_DEVICE_NUM;
    } /* Endif */
#endif
    /**************************************************************
       All memory allocations should be consolidated in one.
       **************************************************************/

#ifndef SMALL_CODE_SIZE                    ///////step
    /* Allocate memory for the state structure */
    usb_dev_ptr = &nanousb_dev_state_struct;//(USB_DEV_STATE_STRUCT_PTR)bbu_malloc(sizeof(USB_DEV_STATE_STRUCT));

    // USB_memalloc(sizeof(USB_DEV_STATE_STRUCT));

    if (usb_dev_ptr == NULL)
    {
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_init, error NULL device handle\r\n");
#endif
        return USBERR_ALLOC_STATE;
    } /* Endif */

    /* Zero out the internal USB state structure */
    //  USB_dev_memzero(usb_dev_ptr,sizeof(USB_DEV_STATE_STRUCT));
    memset((pointer)usb_dev_ptr, 0, sizeof(USB_DEV_STATE_STRUCT));
#endif

    /* Multiple devices will have different base addresses and
      ** interrupt vectors (For future)
      */

    usb_dev_ptr->NEVO_DEV_PTR = (MAVL_USB_TypeDef_PTR )bsp_get_nevo_usb_base(devnum);    ///////step   //by tongchao
    // bbu_printf("usb_device_init, USB_DEVICE_PTR add is 0x%x\r\n",(uint_32)usb_dev_ptr->NEVO_DEV_PTR);
    usb_dev_ptr->DEV_VEC = bsp_get_usb_vector(devnum);
    usb_dev_ptr->USB_STATE = USB_STATE_UNKNOWN;

    usb_dev_ptr->MAX_ENDPOINTS = endpoints;

#ifndef SMALL_CODE_SIZE                                     ///////step
    temp = (usb_dev_ptr->MAX_ENDPOINTS * 2);
#ifdef _DEVICE_DEBUG_
    bbu_printf("the number of EP is %d\r\n", temp);
#endif
#endif

    //#ifndef SMALL_CODE_SIZE                                     ///////step
    //     temp = (usb_dev_ptr->MAX_ENDPOINTS * 2);
    //  #endif


#ifndef SMALL_CODE_SIZE                    ///////step
    /* Allocate MAX_XDS_FOR_TR_CALLS */
    xd_ptr = nanoxd_struct;//(XD_STRUCT_PTR)bbu_malloc(sizeof(XD_STRUCT) * MAX_XDS_FOR_TR_CALLS);
    //USB_memalloc();
    if (xd_ptr == NULL)
    {
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_init, malloc error\r\n");
#endif
        return USBERR_ALLOC_TR;
    }

    USB_dev_memzero(xd_ptr, sizeof(XD_STRUCT) * MAX_XDS_FOR_TR_CALLS);

    /* Allocate memory for internal scratch structure */
    usb_dev_ptr->XD_SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR)(NONE_Cache_ADDR + 64); //by tongchao fixcache bug //  (SCRATCH_STRUCT_PTR) USB_memalloc(sizeof(SCRATCH_STRUCT)*MAX_XDS_FOR_TR_CALLS);

    if (usb_dev_ptr->XD_SCRATCH_STRUCT_BASE == NULL)
    {
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_init, malloc error\r\n");
#endif
        return USBERR_ALLOC;
    } /* Endif */


#else
    /* Allocate MAX_XDS_FOR_TR_CALLS */
    xd_ptr = (XD_STRUCT_PTR) (stack_mem_ptr + sizeof(USB_DEV_STATE_STRUCT));
    usb_dev_ptr->XD_SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR) \
                                          (stack_mem_ptr + sizeof(USB_DEV_STATE_STRUCT) + \
                                           (sizeof(XD_STRUCT) * MAX_XDS_FOR_TR_CALLS)
                                          );

#endif

    usb_dev_ptr->XD_BASE = xd_ptr;                     ///////step


    temp_scratch_ptr = usb_dev_ptr->XD_SCRATCH_STRUCT_BASE;

#ifndef SMALL_CODE_SIZE                      ///////step
    usb_dev_ptr->XD_HEAD = NULL;
    usb_dev_ptr->XD_TAIL = NULL;
    usb_dev_ptr->XD_ENTRIES = 0;
#endif

    /* Enqueue all the XDs */                         ///////step
    for (i = 0; i < MAX_XDS_FOR_TR_CALLS; i++)
    {
        xd_ptr->SCRATCH_PTR = temp_scratch_ptr;
        xd_ptr->SCRATCH_PTR->FREE = usb_device_free_XD;
        xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
        usb_device_free_XD((pointer)xd_ptr);
        xd_ptr++;
        temp_scratch_ptr++;
    } /* Endfor */

#ifndef SMALL_CODE_SIZE                   ///////step
    usb_dev_ptr->TEMP_XD_PTR = &nanoTEMP_XD_PTR; // (XD_STRUCT_PTR)bbu_malloc(sizeof(XD_STRUCT));


    USB_dev_memzero(usb_dev_ptr->TEMP_XD_PTR, sizeof(XD_STRUCT));

#else
    usb_dev_ptr->TEMP_XD_PTR =  (XD_STRUCT_PTR) (stack_mem_ptr + sizeof(USB_DEV_STATE_STRUCT) + \
                                (sizeof(XD_STRUCT) * MAX_XDS_FOR_TR_CALLS) + \
                                sizeof(SCRATCH_STRUCT) * MAX_XDS_FOR_TR_CALLS
                                                );
#endif
    /* Initialize the USB controller chip */

#ifdef __USB_OS_MQX__
    error = ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)\
             usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_INIT(devnum, usb_dev_ptr);
#else
    error = usb_dci_vusb20_init(devnum, usb_dev_ptr);               ///////step
#endif
    /// bbu_printf("usb_dci_vusb20_init, return\r\n");
    if (error)                                                                            ///////step
    {
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_init, init failed\r\n");
#endif
        return USBERR_INIT_FAILED;
    } /* Endif */

    *handle = usb_dev_ptr;
#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_init, SUCCESSFUL\r\n");
#endif
    //bbu_printf("usb_device_init, SUCCESSFUL\r\n");
    return USB_OK;                                                                 ///////stepover
#endif
}



#ifndef SMALL_CODE_SIZE

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_device_register_service
* Returned Value : USB_OK or error code
* Comments       :
*     Registers a callback routine for a specified event or endpoint.
*
*END*--------------------------------------------------------------------*/
static SERVICE_STRUCT   nanoservice[10];

uint_8 usb_device_register_service
(
    /* [IN] Handle to the USB device */
    usb_device_handle         handle,

    /* [IN] type of event or endpoint number to service */
    uint_8                     type,

    /* [IN] Pointer to the service's callback function */
    void(_CODE_PTR_ service)(pointer, boolean, uint_8, uint_8_ptr, uint_32, uint_8)
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr;
    SERVICE_STRUCT_PTR         service_ptr = NULL;
    SERVICE_STRUCT_PTR _PTR_   search_ptr;
    static uint_8 i = 0;
#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_register_service\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    /* Needs mutual exclusion */
    USB_lock();

    /* Search for an existing entry for type */

    for (search_ptr = &usb_dev_ptr->SERVICE_HEAD_PTR;
            *search_ptr;
            search_ptr = &(*search_ptr)->NEXT)
    {
        if ((*search_ptr)->TYPE == type)
        {
            /* Found an existing entry */
            USB_unlock();
#ifdef _DEVICE_DEBUG_
            bbu_printf("usb_device_register_service, service closed\r\n");
#endif
            return USBERR_OPEN_SERVICE;
        } /* Endif */
    } /* Endfor */

    /* No existing entry found - create a new one */
    if (i < 10)
    {
        service_ptr = &( nanoservice[i++]); //(SERVICE_STRUCT_PTR)bbu_malloc(sizeof(SERVICE_STRUCT));

    }
    if (!service_ptr)
    {
        USB_unlock();
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_register_service, error allocating service\r\n");
#endif
        return USBERR_ALLOC;
    } /* Endif */
    //  memset(service_ptr,0,sizeof(SERVICE_STRUCT));

    service_ptr->TYPE = type;
    service_ptr->SERVICE = service;
    service_ptr->NEXT = NULL;
    *search_ptr = service_ptr;

    USB_unlock();

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_register_service, SUCCESSFUL\r\n");
#endif

    return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_device_unregister_service
* Returned Value : USB_OK or error code
* Comments       :
*     Unregisters a callback routine for a specified event or endpoint.
*
*END*--------------------------------------------------------------------*/
uint_8 usb_device_unregister_service
(
    /* [IN] Handle to the USB device */
    usb_device_handle         handle,

    /* [IN] type of event or endpoint number to service */
    uint_8                     type
)
{
    /* Body */
    USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr;
    SERVICE_STRUCT_PTR         service_ptr;
    SERVICE_STRUCT_PTR _PTR_   search_ptr;

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_unregister_service");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    /* Needs mutual exclusion */
    USB_lock();

    /* Search for an existing entry for type */
    for (search_ptr = &usb_dev_ptr->SERVICE_HEAD_PTR;
            *search_ptr;
            search_ptr = &(*search_ptr)->NEXT)
    {
        if ((*search_ptr)->TYPE == type)
        {
            /* Found an existing entry - delete it */
            break;
        } /* Endif */
    } /* Endfor */

    /* No existing entry found */
    if (!*search_ptr)
    {
        USB_unlock();
#ifdef _DEVICE_DEBUG_
        bbu_printf("usb_device_unregister_service, no service found");
#endif
        return USBERR_CLOSED_SERVICE;
    } /* Endif */

    service_ptr = *search_ptr;
    *search_ptr = service_ptr->NEXT;



    bbu_mfree((pointer)service_ptr);
    USB_unlock();

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_unregister_service, SUCCESSFUL");
#endif
    return USB_OK;

} /* EndBody */

#endif


extern uint_8  usb_close_register_service( /* [IN] Handle to the USB device */
    void);
uint_8     init_bit = USB_STATE_UNKNOWN;

//extern void usb_isr_disable(void)
extern uint_8 osType;
uint_8  usb_close_service( /* [IN] Handle to the USB device */
    void )
{
    uint_8  usb_result = 3;
    // osType==0x00;
    //if(!init_bit)
    //{
    uint_32  temp = BU_REG_READ(0xD4208140);
    //  bbu_printf("close usb \r\n");
    //   usb_isr_disable();
    //  usb_result=usb_close_register_service();
    init_bit = USB_STATE_UNKNOWN;
    BU_REG_WRITE(0xD4208140, temp & (~EHCI_CMD_RUN_STOP));
    //  }
    //  else
    usb_result = 1;

    if (!usb_result)
        return 1;
    else
        return usb_result;
}






/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_device_call_service
* Returned Value : USB_OK or error code
* Comments       :
*     Calls the appropriate service for the specified type, if one is
*     registered. Used internally only.
*
*END*--------------------------------------------------------------------*/
uint_8 usb_device_call_service
(
    /* [IN] Handle to the USB device */
    usb_device_handle   handle,

    /* [OUT] Type of service or endpoint */
    uint_8               type,

    /* [OUT] Is it a Setup transfer? */
    boolean              setup,

    /* [OUT] Direction of transmission; is it a Transmit? */
    boolean              direction,

    /* [OUT] Pointer to the data */
    uint_8_ptr           buffer_ptr,

    /* [OUT] Number of bytes in transmission */
    uint_32              length,

    /* [OUT] Any errors */
    uint_8               errors
)
{
    /* Body */

#ifndef SMALL_CODE_SIZE
    USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
    SERVICE_STRUCT _PTR_          service_ptr;

#ifdef _DEVICE_DEBUG_
    //bbu_printf("usb_device_call_service\r\n");
#endif
    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    /* Needs mutual exclusion */
    USB_lock();

    /* Search for an existing entry for type */
    for (service_ptr = usb_dev_ptr->SERVICE_HEAD_PTR;
            service_ptr;
            service_ptr = service_ptr->NEXT)
    {
        if (service_ptr->TYPE == type)
        {
            service_ptr->SERVICE(handle, setup, direction, buffer_ptr, length, errors);
            USB_unlock();
#ifdef _DEVICE_DEBUG_
            //    bbu_printf("usb_device_call_service, SUCCESSFUL service called\r\n");
#endif
            return USB_OK;
        } /* Endif */

    } /* Endfor */

    USB_unlock();

#ifdef _DEVICE_DEBUG_
    //  bbu_printf("usb_device_call_service, SUCCESSFUL service closed\r\n");
#endif
    return USBERR_CLOSED_SERVICE;
#endif

    return USB_OK;
} /* EndBody */




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : usb_device_init_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*     Initializes the endpoint and the data structures associated with the
*  endpoint
*
*END*-----------------------------------------------------------------*/
uint_8 usb_device_init_endpoint
(
    /* [IN] the USB_USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] MAX Packet size for this endpoint */
    uint_16                    max_pkt_size,

    /* [IN] Direction */
    uint_8                     direction,

    /* [IN] Type of Endpoint */
    uint_8                     type,

    /* [IN] After all data is transfered, should we terminate the transfer
    ** with a zero length packet if the last packet size == MAX_PACKET_SIZE?
    */
    uint_8                     flag
)
{
    /* Body */



#ifdef __VUSBHS__
    uint_8         error = 0;
    USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

#ifdef _DEVICE_DEBUG_
    // bbu_printf("usb_device_init_endpoint\r\n");
#endif

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    /* Initialize the transfer descriptor */
    usb_dev_ptr->TEMP_XD_PTR->EP_NUM = ep_num;
    usb_dev_ptr->TEMP_XD_PTR->BDIRECTION = direction;
    usb_dev_ptr->TEMP_XD_PTR->WMAXPACKETSIZE = max_pkt_size;
    usb_dev_ptr->TEMP_XD_PTR->EP_TYPE = type;
    usb_dev_ptr->TEMP_XD_PTR->DONT_ZERO_TERMINATE = flag & USB_DEVICE_DONT_ZERO_TERMINATE;
    usb_dev_ptr->TEMP_XD_PTR->MAX_PKTS_PER_UFRAME =
        ((flag & USB_MAX_PKTS_PER_UFRAME) >> 1);

#ifdef __USB_OS_MQX__
    ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)\
     usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_INIT_ENDPOINT(handle, usb_dev_ptr->TEMP_XD_PTR);
#else
    error = usb_dci_vusb20_init_endpoint(handle, usb_dev_ptr->TEMP_XD_PTR);
#endif

#ifdef _DEVICE_DEBUG_
    // bbu_printf("usb_device_init_endpoint, SUCCESSFUL\r\n");
#endif
    return error;
#endif
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_get_transfer_status
*  Returned Value : Status of the transfer
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 usb_device_get_transfer_status
(
    /* [IN] the USB_USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
)
{
    /* Body */
    uint_8   status;
    // USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

#ifdef _DEVICE_DEBUG_
    //bbu_printf("usb_device_get_transfer_status\r\n");
#endif

    // usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    USB_lock();

#ifdef __VUSBHS__
#ifdef __USB_OS_MQX__
    status = ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)\
              usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_GET_TRANSFER_STATUS(handle, ep_num, direction);
#else
    status = usb_dci_vusb20_get_transfer_status(handle, ep_num, direction);
#endif
#elif defined __VUSB32__
    if (direction == USB_SEND)
    {
        status = usb_dev_ptr->XDSEND[ep_num].STATUS;
    }
    else
    {
        status = usb_dev_ptr->XDRECV[ep_num].STATUS;
    } /* Endif */
#endif

    USB_unlock();

#ifdef _DEVICE_DEBUG_
    // bbu_printf("usb_device_get_transfer_status, SUCCESSFUL\r\n");
#endif

    /* Return the status of the last queued transfer */
    return (status);

} /* EndBody */


#ifdef __VUSBHS__

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_get_transfer_details
*  Returned Value : Status of the transfer
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
void usb_device_get_transfer_details
(
    /* [IN] the USB_USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction,

    /* [OUT] transfer detail data structure maintained by driver*/
    uint_32_ptr _PTR_           xd_ptr_ptr
)
{
    /* Body */
    /*uint_8   status;*/
    //USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
    XD_STRUCT_PTR                 temp;

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_get_transfer_status");
#endif

    //usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    //   USB_lock();

    temp = usb_dci_vusb20_get_transfer_details(handle, ep_num, direction);
    (*xd_ptr_ptr) = (uint_32_ptr) temp;

    //   USB_unlock();

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_get_transfer_status, SUCCESSFUL");
#endif

    return;

} /* EndBody */

#endif



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_read_setup_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Reads the setup data from the hardware
*
*END*-----------------------------------------------------------------*/
void usb_device_read_setup_data
(
    /* [IN] the USB_USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] buffer for receiving Setup packet */
    uchar_ptr                  buff_ptr
)
{
    /* Body */
    //  USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

#ifdef _DEVICE_DEBUG_
    // bbu_printf("usb_device_read_setup_data.\r\n");
#endif

#ifdef __VUSBHS__
    // usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

#ifdef __USB_OS_MQX__
    ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)\
     usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_GET_SETUP_DATA(handle, ep_num, buff_ptr);
#else
    usb_dci_vusb20_get_setup_data(handle, ep_num, buff_ptr);
#endif
#elif defined __VUSB32__
    XD_STRUCT_PTR              pxd;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    pxd = &usb_dev_ptr->XDRECV[ep_num];

    if (buff_ptr)
    {
        /* Copy the setup packet from the BD to the user's buffer */
        USB_dev_memcopy((uchar_ptr)pxd->STARTADDRESS +
                        (pxd->SOFAR ? (pxd->SOFAR - 8) : 0), buff_ptr, 8);
    } /* Endif */
#endif

#ifdef _DEVICE_DEBUG_
    //bbu_printf("usb_device_read_setup_data, SUCCESSFUL.\r\n");
#endif

} /* EndBody */

#ifdef __VUSBHS__
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_set_address
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sets the device address as assigned by the host during enumeration
*
*END*-----------------------------------------------------------------*/
void usb_device_set_address
(
    /* [IN] the USB_USB_dev_initialize state structure */
    usb_device_handle         handle,

    /* [IN] the USB address to be set in the hardware */
    uint_8                     address
)
{
    /* Body */
    // USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

    //   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_set_address\r\n");
#endif

#ifdef _usb_OS_MQX__
    ((USB_CALLBACK_FUNCTIONS_STRUCT_PTR)\
     usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SET_ADDRESS(handle, address);
#else
    usb_dci_vusb20_set_address(handle, address);
#endif

#ifdef _DEVICE_DEBUG_
    bbu_printf("usb_device_set_address, SUCCESSFUL\r\n");
#endif

} /* EndBody */
#endif









