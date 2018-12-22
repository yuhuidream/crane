#ifndef USB_WAKEUP
#define USB_WAKEUP

#include "predefines.h"
#include "PMUA.h"
#include "timer_dec.h" 
#include "USBUTMI.h"
#include "USB.h"
#include "vusbhs.h"
#include "bbu.h"
#include "ppset.h"
#include "pmic.h"
#include "PMUM.h"
#include "interrupt.h"
#include "APBCLK.h"
#include "pmu_dec.h"
#define      WAKE_USB_BIT     (WAKE_USB_VBUS_BIT|WAKE_USB_ID_BIT|WAKE_USB_LINE_BIT)
//#define      WAKE_HSIC_BIT    (WAKE_HSIC_RESUME)
#define      USB_LINESTATE_BIT0            0
#define      USB_LINESTATE_BIT1            1
#define      CHARGER_INIT                        0
extern  void usb_dci_vusb20_isr (   void );
extern  void usb_isr_enable(void);
extern void usb_charger_dci_vusb20_isr(void);

//extern void usbvbus_wk_set(uint32_t time_sec) ;

//extern void usbID_wk_isr(void);

//extern void usbID_wk_set(uint32_t time_sec);

//extern void usb_otg_host_isr_enable(void);

//extern void usblinestate_wk_set(uint32_t time_sec);

//extern void usblinestatte_wk_isr(void);

//extern void usb_reg_dump(void);

extern  void usb_wk_set(uint_32 time_sec, e_wake_src type);

extern void usb_wk_isr(uint_32 type);

extern uint_32 get_usb_wk_flag(void);

extern void set_charger_type(uint_32 type);
extern void  usb_phy_poweroff_for_Ap(void);

#endif

