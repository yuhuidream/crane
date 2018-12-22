
#ifndef _USBHSIC_H
#define _USBHSIC_H


#include "predefines.h"    //by tongchao
#include "timer_dec.h"
#include "cp15.h"        //by tongchao
#include "cache_ops.h"//by tongchao
#include "set_mmu.h"
#include  "usb_types.h"
//#include "ACCU.h"//by tongchao

#include "usb_types.h"
#include "usb_devapi.h"
#include "usb_prv_dev.h"
//#include "usb_dev_board.h"
#include "usb_device.h"
#include "usb_mv8618malloc.h"

#include "PMUA.h"   //by tongchao
#include "USBUTMI.h"  //by tongchao
#include "USB_HSIC_PHY.h"
#include  "CIU.h"
//#include "bbu_usb_speaker.h"

#include "bbu.h"

#include "hostapi.h"
#include "usbprv_host.h"
//#include "usb_board.h"

//#define INT_USB 44

#ifdef __USB_OS_MQX__
#include "mqx_arc.h"
#endif


extern boolean  IN_ISR;
#include "USBSPH_MISC.h"
#include "APBSPARE.h"
#include "AIB.h"
//extern void * USB_memalloc(uint_32 n);
#include "PMUA.h"
//#define TEST_WRITE

#include "pmic.h"
#include "APBCLK.h"
//#include "i2c_dec.h"
//#include "gpio_dec.h"
#include "timer_dec.h"
#include "USB_HSIC_PHY.h"
#include "USB.h"




extern void usb_sph_clk_enable(void);//by tongchao
extern void usb_sph_clk_disable(void);
extern void host_last_step(void);

extern  void usb_sph_phy_host_private(void);
extern void usb_sph_phy_read(void);
extern void usb_sph_phy_dev_private(void);
extern void usb_sph_phy_powerup ( uint_8 type );//by tongchao
extern void usb_sph_phy_init(uint_8 type);
extern void usb_hsic_dci_vusb20_chip_initialize
(
   /* [IN] the USB_dev_initialize state structure */
   usb_device_handle         handle
);

extern DEVICE_INTERRUPT_ROUTINE_KEYWORD void usb_hsic_dci_vusb20_isr (   void );
extern void usb_hsicdci_vusb20_process_port_change
(
   /* [IN] the USB_dev_initialize state structure */
   usb_device_handle         handle
);


extern void HOST_INTERRUPT_ROUTINE_KEYWORD _usb_hsic_hci_vusb20_isr
(
   void
);


extern uint_8  _usb_hsichci_vusb20_init
(
   /* the USB Host State structure */
   _usb_host_handle     handle
);


extern USB_HOST_STATE_STRUCT_PTR usb_host_state_struct_ptr;


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


extern  USB_STATUS  _usb_hci_init_driver_memory
(
   /* the USB Host State structure */
   _usb_host_handle     handle
);


#endif

