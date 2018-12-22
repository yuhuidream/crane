#ifndef   _usb_test_app
#define   _usb_test_app

#include "predefines.h"
#include "qp_port.h"
#include "bsp.h"
#include "bbu.h"
#include "string.h"
#include "usb_define.h"
#include "usb_types.h"
//#include "sd_ops.h"
#include "bbu_help.h"
#ifdef CONFIG_TCPIP
#include "usb_rndis.h"
#endif
//#include "usb3_ecm.h"
#define usb2_unknow       0x00
#define usb_service_bulk  0x01
#define usb_service_audio 0x02
#define usb_service_serial  0x03
#define usb_service_ecm  0x04
#define usb_service_cdc2 0x05
#define usb_service_rndis 0x06
#define usb_service_host   0x10

#define usb3_unknow       0x08
#define usb3_service_bulk  0x09
#define usb3_service_audio 0x0a
#define usb3_service_serial  0x0b
#define usb3_service_ecm  0x0c
#define usb3_service_cdc2 0x0d
#define usb3_service_rndis 0x0e
#define usb3_service_host   0x0f


enum host_type{
  OTG_HOST=0x10,
  HSIC_HOST,
  IC_USB


};
#define sd_storage        0x00
#define mmc_storage  0x01

extern uint_8     init_bit;
extern uint_8 usb_mass_storage_init(U32 epnum, U32 mode);//, U32 media);
extern void usb_charger_detector_init(void);
extern void usb_docharger_detector_init(void);

extern int usb_charger_bit;
extern uint_8 usb_speak_init (U32 epnum);
extern uint_8 usb_serial_init(U32 epnum);
extern uint_8 usb_ecm_init (U32 epnum);
extern  void usb_DiskBufInit(U32 blockNumber, char type);
extern uint_8 usb_cdc2_init (U32 epnum);
extern uint_8  usb_close_service( void );
extern uint_8 usb_rndis_init (U32 epnum);
extern void usb_dci_vusb20_process_reset(usb_device_handle         handle);

extern  void usb_dci_vusb20_process_port_change(usb_device_handle         handle);
extern  void usb_dci_vusb20_process_error(usb_device_handle         handle);
extern  void usb_dci_vusb20_process_suspend(usb_device_handle         handle);
extern  void usb_dci_vusb20_process_tr_complete(usb_device_handle         handle);
extern  uint32_t sd_get_card_total_blocks_num(void);
extern  void usb_isr_enable(void);
extern BU_U32 get_complete_status(void);
extern void post_info2Ao(usb_device_handle handle, uint_32 isr_mode);
extern void usb_dump(void);
extern void dqh_dtdDump(uint_32 epnum);
extern void usb_host_main( uint_8 host_device_num);
extern void    usb_host_close(void);
extern void usb_cdc2_buff_free(void);
extern void usb_free_serialBuff(void);
extern void usb_cdc_change_state(void);
extern void usb_otg_init();
extern void usb_bulk_endpoint4SDRead
( void               *handle,
  uint8_t               ep_num,
  uint8_t         *buffer_ptr,
  uint32_t              size,
  uint8_t               usbcbw_process,
  void              *csw_prt

);
extern void usb_bulk_endpoint4SDWrite
( void               *handle,
  uint8_t               ep_num,
  uint8_t         *buffer_ptr,
  uint32_t              size,
  uint8_t               usbcbw_process,
  void              *csw_prt

);

#ifdef CONFIG_TCPIP
extern uint16_t getTxHeadbufLength(void);
extern uint_8 mv_eth_start_xmit(uint_8 *data, uint_32 length);
extern void mv_eth_receive(uint8_t status);
extern void usb_rx_process(uint8_t status);


#endif
extern void usb_tx_process(uint8_t *data, uint32_t length);

extern int usb_service_type;
#endif
