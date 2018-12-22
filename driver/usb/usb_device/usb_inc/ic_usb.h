
#ifndef  IC_USB_H
#define   IC_USB_H
#include "hostapi.h"
#include "usbprv_host.h"



extern boolean  IN_ISR ;
#ifdef __USB_OTG__
#include "otgapi.h"
#include "usbprv_otg.h"
extern USB_OTG_STATE_STRUCT_PTR usb_otg_state_struct_ptr;
#endif
#include "USBSPH_MISC.h"
#include "APBSPARE.h"
#include "AIB.h"
#include "PMUA.h"
#include "pmic.h"
#include "APBCLK.h"
#include "USBUTMI.h"
extern USB_HOST_STATE_STRUCT_PTR usb_host_state_struct_ptr;
extern void  usb2_phy_init(void);

extern void _usb_host_process_reset_recovery_done

(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle
);







extern boolean _usb_ehci_process_port_change

(
    /* [IN] the USB Host state structure */
    _usb_host_handle        handle
);

extern    void   _usb_ehci_error_process( _usb_host_handle handle);


extern  USB_STATUS  _usb_hci_init_driver_memory

(
    /* the USB Host State structure */
    _usb_host_handle     handle
);

extern uint_8  _usb_icusb_hci_vusb20_init
(
    /* the USB Host State structure */
    _usb_host_handle     handle
);


extern void HOST_INTERRUPT_ROUTINE_KEYWORD _usb_icusb_hci_vusb20_isr
(
    void
);



extern int usb_service_type;
#endif
