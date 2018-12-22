#ifndef __usbprv_host_h__
#define __usbprv_host_h__ 1
/**HEADER*********************************************************
-----------------------------------------------------------------------------
 CONFIDENTIAL AND PROPRIETARY INFORMATION OF TRANSDIMENSION, INC. AND/OR ITS
 WHOLLY OWNED SUBSIDIARY SOFTCONNEX TECHNOLOGIES, INC. THIS NOTICE IS NOT TO
 BE DELETED, MODIFIED, MOVED OR CHANGED IN ANY WAY.

 (TransDimension, Inc. and SoftConnex Technologies, Inc. are referred to
 collectively below as TransDimension, Inc.)
 Copyright (c) 1999 - 2004 by TransDimension, Inc.

 This software is protected by copyright laws and international copyright
 treaties, as well as other intellectual property laws and treaties.  This
 software is a CONFIDENTIAL, unpublished work of authorship, and with portions
 constituting TRADE SECRETS of TransDimension, Inc..  Any unauthorized use,
 disclosure, and/or reproduction of this software, or any part of this software;
 or distribution of this software in any form or by any means; or storage of
 this software in any database or retrieval system, without the express written
 consent of, and license from, TransDimension, Inc. is strictly prohibited.
 This software is protected under the copyright and/or trade secret laws in
 other countries in addition to USA.  All Rights Reserved.  Failure to abide by
 the use, disclosure and/or reproduction restrictions may result in civil and
 or criminal penalties, and will be prosecuted to the maximum extent of the law.
 -------------------------------------------------------------------------------

*** Comments:
***   This file contains the internal USB Host specific type definitions
***
**************************************************************************
**END*********************************************************/
#include "usb_types.h"
#include "vusbhs.h"
#include "hostapi.h"

#define  HUB_LS_SETUP                        (333)

/*The following two lines set the size of EHCI frame list */
#define  MAX_EHCI_FRAME_LIST_SIZE            8
#define  MAX_EHCI_FRAME_LIST_BITS            EHCI_CMD_FRAME_SIZE_8

#define  MAX_QH_DESCRS                       (8)
#define  MAX_QTD_DESCRS                      (16)
#define  MAX_ITD_DESCRS                      (16)
#define  MAX_SITD_DESCRS                     (16)


#define  USB_MAX_INTERFACES                  (10)
#define  RX_BUFFER_SIZE                      (0xFF)

//#define _DATA_CACHE_

/***************************************
**
** Code macros
**
*/

/* Macro for alignment to specific byte boundary */
#define  USB_MEM32_ALIGN(n)                  ((n) + (-(n) & 31))
#define  USB_MEM512_ALIGN(n)                 ((n) + (-(n) & 511))
#define  USB_MEM4096_ALIGN(n)                ((n) + (-(n) & 4095))

#define  BitStuffTime(x)                     (7* 8 * x / 6)

/* Callback function storage structure */
typedef  struct host_service_struct
{
    uint_32              TYPE;
    void                 (_CODE_PTR_ SERVICE)(pointer, uint_32);
    struct host_service_struct _PTR_ NEXT;
} USB_SERVICE_STRUCT, _PTR_ USB_SERVICE_STRUCT_PTR;

typedef struct pipe_tr_struct
{
    TR_INFO_FIELDS                 /* 28 bytes in size */
    struct pipe_tr_struct   _PTR_ next;
    uint_32                       status;
    /* Second phase of setup packet:
    ** Send/Receive
    */
    boolean                       SEND_PHASE;
    /* number of ITds/SITDs used for this transfer */
    uint_32                       no_of_itds_sitds;

    /****************************************************
    Data cache requirement is that any buffer that stores
    the data should be cache aligned. This means that the
    setup packet buffer must be cache aligned too. Structure
    fields above this have occupied 28 + 16 = 44 bytes.
    We add a 20 bytes padding to makesure that it is aligned
    on a 32 byte boundry since 44 + 20 = 64; We also
    add a padding at the end to ensure that we don't corrupt
    someother memory when we flush the setup packet
    from cache.
    ****************************************************/
#ifdef _DATA_CACHE_
    uint_8                       RESERVED1[USB_CACHE_ALIGN(44) - 44];
#endif

    USB_SETUP                     setup_packet;

#ifdef _DATA_CACHE_
    uint_8               RESERVED2[USB_CACHE_ALIGN(8) - 8];
#endif


} PIPE_TR_STRUCT, _PTR_ PIPE_TR_STRUCT_PTR;

typedef struct  _PIPE_DESCRIPTOR_STRUCT
{
    PIPE_INFO_FIELDS     /* 34 bytes in size */
    uint_8               CURRENT_INTERVAL;
    uint_8               NEXTDATA01;       /* Data 0/1 synch bit for the PID */
    uint_8               HUB_DEVICE_ADDR;
    uint_8               HUB_PORT_NUM;
    int_16               PIPE_ID;          /* Pipe ID */
    uint_32              SOFAR;            /* number of bytes sent/recv'd so far */
    boolean              OPEN;
    uint_32              CURRENT_NAK_COUNT;
    PIPE_TR_STRUCT_PTR   tr_list_ptr;
    pointer              QH_FOR_THIS_PIPE; /* Queue head for this pipe */

    /* Special fields defined for periodic transfers */
    uint_32              START_FRAME;    /*frame number from which this pipe is started to schedule*/
    uint_32              START_UFRAME;   /*Micro frame number from which this pipe is started to schedule*/
    uint_32              NO_OF_START_SPLIT; /* store the number of start splits (used for ISO OUT )*/
    uint_8               START_SPLIT;    /* store the start split slots for full speed devices*/
    uint_8               COMPLETE_SPLIT; /* store the complete split slots for full speed devices*/
    uint_16              BWIDTH;         /* time take by this pipe (for periodic pipes only */
    uint_8               BWIDTH_SLOTS[8];/*micro frame slots budgeted for transaction */
    /* 84 bytes so far add padding to ensure cache alignment*/
#ifdef _DATA_CACHE_
    uint_8               reserved[USB_CACHE_ALIGN(74) - 74] ;
#endif

} PIPE_DESCRIPTOR_STRUCT, _PTR_ PIPE_DESCRIPTOR_STRUCT_PTR;

/* Queue head management data structure */
typedef struct active_qh_mgmt_struct
{
    EHCI_QH_STRUCT_PTR                  QH_PTR;
    EHCI_QTD_STRUCT_PTR                 FIRST_QTD_PTR;
    struct active_qh_mgmt_struct _PTR_  NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
} ACTIVE_QH_MGMT_STRUCT, _PTR_ ACTIVE_QH_MGMT_STRUCT_PTR;

/* ITD,SITD list management data structure (doubly link list )*/
typedef struct list_node_struct
{
    struct list_node_struct _PTR_  next;        /* next member in the list */
    struct list_node_struct _PTR_  prev;        /* previous member in the list */
    boolean   next_active; /* is next node a active node */
    pointer   member;      /* pointer to the currently active ITD or SITD*/
} LIST_NODE_STRUCT, _PTR_ LIST_NODE_STRUCT_PTR;


/* Class Callback function storage structure */
typedef struct class_service_struct
{
    uint_8               CLASS_TYPE;
    uint_8               SUB_CLASS;
    uint_8               PROTOCOL;
    /* index of the instance */
    uint_8               INDEX;
    /* identification string unique to this type */
    char_ptr             CLASS_ID_STRING;
    /* class handle will be NULL if it is not initialized */
    pointer              CLASS_HANDLE;
    pointer (_CODE_PTR_  INIT)(pointer, uint_32);
    void    (_CODE_PTR_  DEINIT)(pointer);
    void    (_CODE_PTR_  CALL_BACK_INIT)(pointer, char_ptr);
    void    (_CODE_PTR_  CALL_BACK_REMOVE)(pointer);
    struct class_service_struct _PTR_ NEXT_INSTANCE;
    struct class_service_struct _PTR_ NEXT;
} USB_HOST_CLASS_DRIVER_STRUCT, _PTR_ USB_HOST_CLASS_DRIVER_STRUCT_PTR;

typedef struct  usb_host_state_structure
{
    uint_32                             FRAME_LIST_SIZE;
    pointer                             DEV_PTR;
    pointer                             DEVICE_LIST_PTR;
    USB_SERVICE_STRUCT_PTR              SERVICE_HEAD_PTR;
    PIPE_DESCRIPTOR_STRUCT_PTR          PIPE_DESCRIPTOR_BASE_PTR;//all the host pipes
    uint_8_ptr                          PERIODIC_FRAME_LIST_BW_PTR;
    pointer                             CALLBACK_STRUCT_PTR;
    pointer                             PERIODIC_LIST_BASE_ADDR;
    pointer                             ALIGNED_PERIODIC_LIST_BASE_ADDR;
    EHCI_QH_STRUCT_PTR                  ASYNC_LIST_BASE_ADDR;
    EHCI_QH_STRUCT_PTR                  ALIGNED_ASYNCLIST_ADDR;
    EHCI_QH_STRUCT_PTR                  QH_BASE_PTR;
    SCRATCH_STRUCT_PTR                  QH_SCRATCH_STRUCT_BASE;
    EHCI_QH_STRUCT_PTR                  QH_ALIGNED_BASE_PTR;
    uint_32                             QH_ENTRIES;
    EHCI_QH_STRUCT_PTR                  QH_HEAD;
    EHCI_QH_STRUCT_PTR                  QH_TAIL;
    ACTIVE_QH_MGMT_STRUCT_PTR           ACTIVE_ASYNC_LIST_PTR;
    ACTIVE_QH_MGMT_STRUCT_PTR           ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;
    EHCI_QTD_STRUCT_PTR                 QTD_BASE_PTR;
    EHCI_QTD_STRUCT_PTR                 QTD_HEAD;
    EHCI_QTD_STRUCT_PTR                 QTD_TAIL;
    EHCI_QTD_STRUCT_PTR                 QTD_ALIGNED_BASE_PTR;
    uint_32                             QTD_ENTRIES;
    SCRATCH_STRUCT_PTR                  QTD_SCRATCH_STRUCT_BASE;
    boolean                             HIGH_SPEED_ISO_QUEUE_ACTIVE;
    LIST_NODE_STRUCT_PTR                ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR;
    LIST_NODE_STRUCT_PTR                ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR;
    EHCI_ITD_STRUCT_PTR                 ITD_BASE_PTR;
    EHCI_ITD_STRUCT_PTR                 ITD_ALIGNED_BASE_PTR;
    EHCI_ITD_STRUCT_PTR                 ITD_HEAD;
    EHCI_ITD_STRUCT_PTR                 ITD_TAIL;
    uint_32                             ITD_ENTRIES;
    SCRATCH_STRUCT_PTR                  ITD_SCRATCH_STRUCT_BASE;
    boolean                             FULL_SPEED_ISO_QUEUE_ACTIVE;
    LIST_NODE_STRUCT_PTR                ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR;
    LIST_NODE_STRUCT_PTR                ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR;
    EHCI_SITD_STRUCT_PTR                SITD_BASE_PTR;
    EHCI_SITD_STRUCT_PTR                SITD_ALIGNED_BASE_PTR;
    SCRATCH_STRUCT_PTR                  SITD_SCRATCH_STRUCT_BASE;
    EHCI_SITD_STRUCT_PTR                SITD_HEAD;
    EHCI_SITD_STRUCT_PTR                SITD_TAIL;
    uint_32                             SITD_ENTRIES;
    uint_32                             RESET_RECOVERY_TIMER;
    uint_32                             SPEED;
    uint_32                             PORT_NUM;
    uint_8                              DEV_NUM;
    uint_8                              UFRAME_COUNT;
    pointer                             DEVICE_INFO_TABLE;
    pointer                             CONTROLLER_MEMORY;
    boolean                             PERIODIC_LIST_INITIALIZED;
    boolean                             ITD_LIST_INITIALIZED;
    boolean                             SITD_LIST_INITIALIZED;
} USB_HOST_STATE_STRUCT, _PTR_ USB_HOST_STATE_STRUCT_PTR;

#define EHCI_GET_TYPE(data_struct_ptr) \
    (*((USB_Uncached uint_32 _PTR_)data_struct_ptr) & EHCI_ELEMENT_TYPE_MASK)

#define EHCI_ITD_QADD(head, tail, ITD)      \
    if ((head) == NULL) {         \
        (head) = (ITD);            \
    } else {                      \
        (tail)->SCRATCH_PTR->PRIVATE = (void *) (ITD);   \
    } /* Endif */                 \
    (tail) = (ITD);               \
    (ITD)->SCRATCH_PTR->PRIVATE = NULL

#define EHCI_ITD_QGET(head, tail, ITD)      \
    (ITD) = (head);               \
    if (head) {                   \
        (head) = (EHCI_ITD_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
        if ((head) == NULL) {      \
            (tail) = NULL;          \
        } /* Endif */              \
    } /* Endif */

#define EHCI_ACTIVE_QUEUE_ADD_NODE(tail,member_ptr)     \
    if (tail->prev != NULL) {                             \
        tail->prev->next_active = TRUE;                   \
    }                                                     \
    tail->member = (pointer) member_ptr;                                  \
    tail->next_active = FALSE;                            \
    tail = tail->next;

#define EHCI_QUEUE_FREE_NODE(head,tail,node_ptr) \
    if(node_ptr->prev != NULL) {                             \
        node_ptr->prev->next = node_ptr->next;                 \
        node_ptr->prev->next_active = node_ptr->next_active;   \
        node_ptr->next->prev = node_ptr->prev;                 \
    } else  {                                                \
        head = node_ptr->next;                                 \
        head->prev = NULL;                                     \
    }                                                        \
    node_ptr->next = tail->next;                             \
    node_ptr->prev = tail;                                   \
    node_ptr->next_active = FALSE;                           \
    node_ptr->member = NULL;                                 \
    if(tail->next != NULL)   {                               \
        tail->next->prev = node_ptr;                          \
    }                                                        \
    tail->next = node_ptr;                                   \
    tail->next_active = FALSE;




#define EHCI_SITD_QADD(head, tail, SITD)      \
    if ((head) == NULL) {         \
        (head) = (SITD);            \
    } else {                      \
        (tail)->SCRATCH_PTR->PRIVATE = (void *) (SITD);   \
    } /* Endif */                 \
    (tail) = (pointer) (SITD);               \
    (SITD)->SCRATCH_PTR->PRIVATE = NULL

#define EHCI_SITD_QGET(head, tail, SITD)      \
    (SITD) = (head);               \
    if (head) {                   \
        (head) = (EHCI_SITD_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
        if ((head) == NULL) {      \
            (tail) = NULL;          \
        } /* Endif */              \
    } /* Endif */

#define EHCI_QTD_QADD(head, tail, QTD)      \
    if ((head) == NULL) {         \
        (head) = (QTD);            \
    } else {                      \
        (tail)->SCRATCH_PTR->PRIVATE = (void *) (QTD);   \
    } /* Endif */                 \
    (tail) = (pointer) (QTD);               \
    (QTD)->SCRATCH_PTR->PRIVATE = NULL

#define EHCI_QTD_QGET(head, tail, QTD)      \
    (QTD) = (head);               \
    if (head) {                   \
        (head) = (EHCI_QTD_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
        if ((head) == NULL) {      \
            (tail) = NULL;          \
        } /* Endif */              \
    } /* Endif */

#define EHCI_QH_QADD(head, tail, QH)      \
    if ((head) == NULL) {         \
        (head) = (pointer) (QH);            \
    } else {                      \
        (tail)->SCRATCH_PTR->PRIVATE = (void *) (QH);   \
    } /* Endif */                 \
    (tail) = (QH);               \
    (QH)->SCRATCH_PTR->PRIVATE = NULL

#define EHCI_QH_QGET(head, tail, QH)      \
    (QH) = (head);               \
    if (head) {                   \
        (head) = (EHCI_QH_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
        if ((head) == NULL) {      \
            (tail) = NULL;          \
        } /* Endif */              \
    } /* Endif */

extern USB_HOST_STATE_STRUCT_PTR usb_host_state_struct_ptr;

/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __USB_OS_MQX__
extern void _usb_hci_vusb20_isr(_usb_host_handle);
#else
extern void HOST_INTERRUPT_ROUTINE_KEYWORD _usb_hci_vusb20_isr(void);
#endif
extern void _usb_host_delay(_usb_host_handle, uint_32);
extern uint_8 _bsp_get_usb_vector(uint_8);
extern pointer _bsp_get_usb_base(uint_8);
extern pointer _bsp_get_usb_capability_register_base(uint_8);
extern PIPE_TR_STRUCT_PTR _usb_host_get_tr_element(_usb_pipe_handle, uint_32);
extern uint_32 _usb_host_set_up_TR(_usb_pipe_handle, TR_INIT_PARAM_STRUCT_PTR, PIPE_TR_STRUCT_PTR _PTR_);
extern uint_8 _usb_hci_vusb20_init(_usb_host_handle);
extern uint_32 _usb_ehci_allocate_bandwidth(_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR);
extern void _usb_ehci_close_interrupt_pipe (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR);
extern void _usb_ehci_close_isochronous_pipe (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR);
extern void _usb_ehci_free_resources(_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR);
extern USB_STATUS _usb_hci_vusb20_send_setup(_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern void _usb_hci_vusb20_shutdown(_usb_host_handle);
extern void _usb_hci_vusb20_bus_control(_usb_host_handle, uint_8);
extern void _usb_hci_vusb20_cancel_transfer(_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern USB_STATUS _usb_hci_vusb20_send_data(_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern USB_STATUS _usb_hci_vusb20_recv_data(_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern USB_STATUS _usb_hci_ehci_queue_pkts(_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern uint_32 _usb_ehci_add_isochronous_xfer_to_periodic_schedule_list(_usb_host_handle,
        PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern uint_32 _usb_ehci_add_xfer_to_asynch_schedule_list(_usb_host_handle,
        PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern uint_32 _usb_ehci_add_interrupt_xfer_to_periodic_list(_usb_host_handle,
        PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern void _usb_ehci_reset_and_enable_port(_usb_host_handle, uint_8);
extern void _usb_ehci_process_hnp_hW_assisted_detach (_usb_host_handle);
//extern void _usb_ehci_process_hnp_host_initialize ();
extern void _usb_ehci_process_tr_complete(_usb_host_handle);
extern void _usb_ehci_process_system_errors(_usb_host_handle);
extern void _usb_ehci_bus_suspend(_usb_host_handle);
extern uint_32 _usb_ehci_get_frame_number(_usb_host_handle);
extern uint_32 _usb_ehci_get_micro_frame_number(_usb_host_handle);
extern void _usb_hci_vusb20_free_QTD(pointer, pointer);
extern void _usb_hci_vusb20_free_QH(pointer, pointer);
extern void _usb_hci_vusb20_free_SITD(pointer, pointer);
extern void _usb_hci_vusb20_free_ITD(pointer, pointer);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */


