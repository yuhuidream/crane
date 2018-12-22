#include "usb_test_app.h"





typedef struct USBAppTag
{
    QActive super;
    QTimeEvt USBTimeOutEvt;
    QEvent const *requestQSto[4];
    QEQueue requestQueue;
} USBApp;


static QState USB_App_initial(USBApp *me, QEvent const *e);
static QState USB_ack(USBApp *me, QEvent const *e);
static QState USB_App_process(USBApp *me, QEvent const *e);
//static QState USB_DATE_handle(USBApp *me, QEvent const *e);
//static QState USB_COMPELETE_handle(USBApp *me,QEvent const *e);
static QState  USB_Process_Complete(USBApp *me, QEvent const *e);
//static QState USB_SD_ACK(USBApp *me, QEvent const *e);
static USBApp I_USBApp;
QActive *const AO_USB = (QActive *) &I_USBApp;
static  int usb_cmd_index = 0;
static   int usb_result = 2;
int usb_service_type = usb2_unknow;
int  storage_type = sd_storage;
//static void usbm_help(void);
//static void usbm_mmcHelp(void);

/*USBApp Construction*/
void USB_ctor(void)
{

    USBApp *me = &I_USBApp;
    QActive_ctor(&me->super, (QStateHandler)&USB_App_initial);
    QEQueue_init(&me->requestQueue, me->requestQSto, Q_DIM(me->requestQSto));
    QTimeEvt_ctor(&me->USBTimeOutEvt, USB_TIMEOUT_SIG);
}
#if 0

void get_SD_Capacity(void)
{

    SDTransReqEvt   *cmdevt = Q_NEW(SDTransReqEvt, SD_TRANS_REQ_SIG);

    //cmdevt->ops_type=1;//readCapacity;;
    QACTIVE_POST(AO_SDH, (QEvent *)cmdevt, 0);
}

void sd_infopost(U32 blocknum, LP_U8 buffer, U32 size, U8 ops_type)
{

    SDTransReqEvt   *cmdevt = Q_NEW(SDTransReqEvt, SD_TRANS_REQ_SIG);

    cmdevt->AO_caller = AO_USB;
    cmdevt->ops_type = ops_type;

    cmdevt->start_addr = blocknum;
    cmdevt->size = size;
    cmdevt->buffer = buffer;
    cmdevt->trans_mode = SD_TRANS_TYPE_ADMA;
    QACTIVE_POST(AO_SDH, (QEvent *)cmdevt, 0);



}

void mmc_infopost(U32 blocknum, LP_U8 buffer, U32 size, U8 ops_type)
{

    SDTransReqEvt   *cmdevt = Q_NEW(SDTransReqEvt, MMC_TRANS_REQ_SIG);

    cmdevt->AO_caller = AO_USB;
    cmdevt->ops_type = ops_type;

    cmdevt->start_addr = blocknum;
    cmdevt->size = size;
    cmdevt->buffer = buffer;
    cmdevt->trans_mode = MMC_TRANS_TYPE_ADMA;
    QACTIVE_POST(AO_SDH, (QEvent *)cmdevt, 0);



}
//#else
#define sd_infopost(blocknum, buffer, size, ops_type)   \
    sd_send_request(AO_USB, blocknum, buffer, size, ops_type, 0)
#define mmc_infopost(blocknum, buffer, size, ops_type)  \
    mmc_send_request(AO_USB, blocknum, buffer, size, ops_type, 0)
//#endif

void  USB_post(void)
{
    //ScreenCmdAck *sca = Q_NEW(ScreenCmdAck,COM_RX_SCREEN_ACK_SIG);
    //cmd_index=5;
    //if(!usb_result)
    QTimeEvt_disarm(&(I_USBApp.USBTimeOutEvt));
    usb_result = 1;
    if (usb_cmd_index != 0)
        usb_close_service();
    screen_cmd_ack( usb_result, usb_cmd_index);

    //#ifdef CONFIG_SCREEN_CHIP
    //  usb_close_service();
    // #endif
    /* sca->cmd_index = usb_cmd_index;  //just send command location back so that  AO_Cli can distinguish each command, must     do it
     sca->result = usb_result;   // depend on your command execute return value . 0x01 -- success , 0x02 -- failure, 0x03 â€“ complete but need people to check result *

     bbu_printf("result result result is %d\r\n",usb_result);
     bbu_printf("cmd_index cmd_index cmd_index is %d\r\n",usb_cmd_index);
      QACTIVE_POST(AO_Cli, (QEvent *)sca, 0);  // post this event to AO_Cli ,showing completion
      */
}



void  USBCDC_post(void)
{

    bbu_printf("close cdc timer\r\n");
    QTimeEvt_disarm(&(I_USBApp.USBTimeOutEvt));

}
#endif

static QState USB_App_initial(USBApp *me, QEvent const *e)
{
    (void)e;

    QS_OBJ_DICTIONARY(&I_USBApp);

    QS_FUN_DICTIONARY(&USB_ack);
    QS_FUN_DICTIONARY(&USB_App_initial);
    QS_FUN_DICTIONARY(&USB_App_process);
    //QS_FUN_DICTIONARY(&USB_DATE_handle);
    //QS_FUN_DICTIONARY(&USB_SD_ACK);

    //QS_SIG_DICTIONARY(USB_IUSB_STORAGE, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_SPEAKER, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_MMC_STORAGE, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_SERIAL, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_CDC2, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_RNDIS, &I_USBApp);
    QS_SIG_DICTIONARY(USB_HOST_TEST, &I_USBApp);



    //QS_SIG_DICTIONARY(USB_CHARGER, &I_USBApp);
    QS_SIG_DICTIONARY(USB_CLOSE, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_RESET, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_PORT_CHANGER, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_STS_ERR, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_STS_SUSPEND, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_STS_COMPLETE, &I_USBApp);
    //QS_SIG_DICTIONARY(USB_CHARGER_INIT, &I_USBApp);

    //return Q_TRAN(&sd_ack);
    //get_SD_Capacity();


    return Q_TRAN(&USB_ack);


}

static QState USB_ack(USBApp *me, QEvent const *e)
{

    //SDTransAckEvt *TransAckEvt=(SDTransAckEvt *)e;


    USBReqEvt  *regEvt = Q_NEW(USBReqEvt, USB_REQ_SIG);

    switch (e->sig)
    {
    case Q_ENTRY_SIG:
    {
        /*   */



        QACTIVE_POST((QActive *)me, (QEvent *)regEvt, 0);
        return Q_HANDLED();

    }
    case USB_REQ_SIG:
    {
#ifdef CONFIG_USB_UART_DEFAULT
        QACTIVE_POST(AO_USB, Q_NEW(QEvent , USB_SERIAL), 0);
#endif

        return Q_TRAN(&USB_App_process);
    }

        /*   block_number=TransAckEvt->block_number;
           usb_DiskBufInit(block_number,1);
                    return Q_TRAN(&USB_App_process);
              case SD_TRANS_FAIL_SIG:
           block_number=20480;
           usb_DiskBufInit(block_number,0);
                    return Q_TRAN(&USB_App_process);     */

    }

    return Q_SUPER(&QHsm_top);

}
#if 0
void getSDblockNumber()
{
    U32  block_number = 0;
#ifndef CONFIG_SCREEN_CHIP
    block_number = sd_get_card_total_blocks_num();
#endif
    if (!block_number)
    {
        block_number = 20480;
        usb_DiskBufInit(block_number, 0);
    }
    else
    {
        usb_DiskBufInit(block_number, 1);
        if (!block_number)
            bbu_printf("there is no sd card\r\n");
        else
            bbu_printf("SD card is used as USB disk CardReader_Counts is %d.\r\n", block_number);
    }


}


void u_ether_start_xmit(uint8_t *data, uint32_t length)
{

    //bbu_printf("usb_service_type is %d xmit length is %d\r\n",length,usb_service_type);

#ifdef CONFIG_TCPIP
    if (usb_service_type >= usb3_unknow)
    {
        //bbu_printf("xmit length is %d\r\n");
        mv_eth_start_xmit(data, length);


    }
    else if (usb_service_type == usb_service_rndis)
    {
        rndisSend2_host(data, length);
    }
#endif
}
void  u_ether_data_receive(uint8_t status)

{

    //bbu_printf("usb_service_type is %d xmit length is %d\r\n",length,usb_service_type);

#ifdef CONFIG_TCPIP
    if (usb_service_type >= usb3_unknow)
    {
        //bbu_printf("receive length is %d\r\n");
        //mv_eth_start_xmit(data,length);
        mv_eth_receive(status);

    }
    else if (usb_service_type == usb_service_rndis)
    {
        rndisReceiveData(status);
    }
#endif
}

static  void getMMCblockNumber()
{
    U32  block_number = 0;
    //#ifndef CONFIG_SCREEN_CHIP
    block_number = mmc_get_user_total_blocks_num();
    //#endif
    if (!block_number)
    {
        block_number = 20480;
        usb_DiskBufInit(block_number, 0);
    }
    else
    {
        usb_DiskBufInit(block_number, 1);
        if (!block_number)
            bbu_printf("there is no sd card\r\n");
        else
            bbu_printf("MMC card is used as USB disk CardReader_Counts is %d.\r\n", block_number);

    }


}
#endif
static QState USB_App_process(USBApp *me, QEvent const *e)
{

    ComCmdEvt *evt = (ComCmdEvt *)e;
    U32 a1, a2;


    switch (e->sig)
    {
    case Q_ENTRY_SIG:
    {
        return Q_HANDLED();
    }
#if 0
    case USB_IUSB_STORAGE:
        bbu_printf("usbm test begin\r\n");
        usb_cmd_index = evt->scr_cmd_index;

        if (init_bit != 0xff)
        {
            USB_post();
        }
        else
        {
            storage_type = sd_storage;

            usb_service_type = usb_service_bulk;
            getSDblockNumber();
            if (evt->argc == 1)
            {

                usb_result = usb_mass_storage_init(0, 0);
                if (!usb_result)
                {
                    QTimeEvt_disarm(&(I_USBApp.USBTimeOutEvt));
                    QTimeEvt_postIn(&(I_USBApp.USBTimeOutEvt), AO_USB, BSP_TICKS_PER_SEC * 2);
                }
                else
                {
                    usb_result = 2;
                    screen_cmd_ack( usb_result, usb_cmd_index);
                }


                // USB_post();
            }
            else if (evt->argc >= 3)
            {
                a1 = conv_hex((char *)(evt->argv[1]));
                a2 = conv_hex((char *)(evt->argv[2]));
                usb_mass_storage_init(a1, a2);
            }
            else if (check_help(evt->argv[1]))
            {
                usbm_help();
            }
            else if ( (strcmp((const char *)evt->argv[1], "HSIC") == 0) || (strcmp((const char *)evt->argv[1], "hsic") == 0))
            {
                bbu_printf("usb hsic device init\r\n");
                usb_mass_storage_init(1, 0);
            }
        }
        return Q_TRAN(&USB_DATE_handle);
    case USB_MMC_STORAGE:
        bbu_printf("usbm_mmc test begin\r\n");
        storage_type = mmc_storage;
        usb_cmd_index = evt->scr_cmd_index;
        usb_service_type = usb_service_bulk;
        if (evt->argc == 1)
        {
            getMMCblockNumber();
            usb_mass_storage_init(0, 0);
            //     USB_post();
        }
        else if (check_help(evt->argv[1]))
        {
            usbm_mmcHelp();
        }
        return Q_TRAN(&USB_DATE_handle);
    case  USB_SPEAKER:
        bbu_printf("usb audio test begin\r\n");
        usb_service_type = usb_service_audio;
        usb_speak_init(1);
        return Q_TRAN(&USB_DATE_handle);
    case  USB_SERIAL:
        bbu_printf("usb serial test begin\r\n");
        usb_service_type = usb_service_serial;
        usb_serial_init(1);
        return Q_TRAN(&USB_DATE_handle);
    case  USB_ECM:
        bbu_printf("usb ecm test begin\r\n");
        usb_service_type = usb_service_ecm;
#ifdef CONFIG_TCPIP
        usb_ecm_init(1);
#endif
        return Q_TRAN(&USB_DATE_handle);
    case  USB_CDC2:
        bbu_printf("usb composite driver test begin\r\n");
        QTimeEvt_disarm(&(I_USBApp.USBTimeOutEvt));
        QTimeEvt_postIn(&(I_USBApp.USBTimeOutEvt), AO_USB, BSP_TICKS_PER_SEC * 4);
        usb_service_type = usb_service_cdc2;
        getSDblockNumber();
#ifdef CONFIG_TCPIP
        usb_cdc2_init(1);
#endif
        return Q_TRAN(&USB_DATE_handle);
    case USB_RNDIS:
        bbu_printf("usb rndis test begin\r\n");
        usb_service_type = usb_service_rndis;
#ifdef CONFIG_TCPIP
        usb_rndis_init(1);
#endif
        return Q_TRAN(&USB_DATE_handle);
#endif
    case USB_HOST_TEST:
        usb_service_type = usb_service_host;
        if (evt->argc == 1)
        {
            usb_service_type = OTG_HOST;
            bbu_printf("otg host test begin\r\n");
            usb_host_main( 0);
        }
        else if ( (strcmp((const char *)evt->argv[1], "HSIC") == 0) || (strcmp((const char *)evt->argv[1], "hsic") == 0))
        {
            usb_service_type = HSIC_HOST;
            bbu_printf("usb hsic host init\r\n");
            usb_host_main(1);
        }
        else if ( (strcmp((const char *)evt->argv[1], "ICUSB") == 0) || (strcmp((const char *)evt->argv[1], "icusb") == 0))
        {
            usb_service_type = IC_USB;
            bbu_printf("usb icusb host init\r\n");
            usb_host_main(2);
        }


        return Q_HANDLED();
    case USB_CLOSE:
#ifndef USB_UART
        bbu_printf("USB_close\r\n");
#endif
#if 0
        if (usb_service_type != usb_service_host)
        {
            usb_cmd_index = evt->scr_cmd_index;
            usb_result = usb_close_service();
            screen_cmd_ack(usb_result, usb_cmd_index);
        }
        else
#endif
        {
            usb_host_close();
        }
        return Q_HANDLED();
#if 0
    case USB_DR:
#ifdef __USB_OTG__
        usb_otg_init();
#endif
        usb_dump();
        return Q_HANDLED();
    case USB_DQH:
        if ((evt->argc) == 2)
        {
            a1 = conv_hex((char *)(evt->argv[1]));
            dqh_dtdDump(a1);
        }
        else
            dqh_dtdDump(0);
        return Q_HANDLED();
    case USB_TIMEOUT_SIG:
        QTimeEvt_disarm(&(I_USBApp.USBTimeOutEvt));
        if (usb_service_type == usb_service_cdc2)
        {
#ifdef CONFIG_TCPIP
            usb_cdc_change_state()  ;
#endif
            return Q_HANDLED();


        }
        if (usb_result != 1)
        {
            usb_result = 3;
            bbu_printf("\r\nNote:please check usb cable if  it was connected\r\n");
            screen_cmd_ack( usb_result, usb_cmd_index);
        }
        else
        {
            //bbu_printf("success\r\n");
        }
        return Q_HANDLED();
    case USB_CHARGER:
        usb_charger_detector_init();

        return Q_HANDLED();
    case USB_CHARGER_INIT:
        usb_docharger_detector_init();
        return Q_HANDLED();
#endif
    }

    return Q_SUPER(&QHsm_top);


}


#if 0
static USBHandleEvt *usbHandlEvt;

static QState USB_DATE_handle(USBApp *me, QEvent const *e)
{
    USBHandleEvt *HandleEvt = (USBHandleEvt *)e;
    //  bbu_printf("USB_DATE_handle is coming \r\n");
    BU_U32 complete_status;
    switch (e->sig)
    {

    case Q_ENTRY_SIG:
    {
        //bbu_printf("enter USB_DATE_handle\r\n");
        return Q_HANDLED();
    }
    case USB_CHARGER:
        usb_charger_detector_init();

        return Q_HANDLED();
    case USB_ETHER_SEND_SIG:
    {
        USBEtherSendEvt *send_evt = (USBEtherSendEvt *)e;
        uint8_t *send_data = send_evt->data;
        uint32_t length = send_evt->length;
        u_ether_start_xmit(send_data, length);
        return Q_HANDLED();
    }
    case USB_ETHER_RECE_SIG:
    {
        USBEtherReceiveEvt *rece_evt = (USBEtherReceiveEvt *)e;
        uint8_t status = rece_evt->status;
        u_ether_data_receive(status);
        return Q_HANDLED();



    }
    case USB_CLOSE:
        bbu_printf("USB_close\r\n");
        ComCmdEvt *evt1 = (ComCmdEvt *)e;
        usb_cmd_index = evt1->scr_cmd_index;
        usb_result = usb_close_service();
        screen_cmd_ack(usb_result, usb_cmd_index);
        return Q_TRAN(&USB_App_process);
    case USB_RESET:
        //  bbu_printf("USB_RESET is %d\r\n",USB_RESET);
        usb_dci_vusb20_process_reset((usb_device_handle)(HandleEvt->handle));
        return Q_HANDLED();
    case USB_PORT_CHANGER:
        //  bbu_printf("USB_PORT_CHANGER is %d\r\n",USB_PORT_CHANGER);
        usb_dci_vusb20_process_port_change((usb_device_handle)(HandleEvt->handle));
        return Q_HANDLED();
    case USB_STS_ERR:
        usb_dci_vusb20_process_error((usb_device_handle)(HandleEvt->handle));
        return Q_HANDLED();
    case USB_STS_SUSPEND:
        //bbu_printf("USB_STS_SUSPEND is %d\r\n",USB_STS_SUSPEND);
        usb_dci_vusb20_process_suspend((usb_device_handle)(HandleEvt->handle));
        //post_info2Ao((usb_device_handle)(HandleEvt->handle),USB_CLOSE);
        return Q_HANDLED();
    case USB_STS_COMPLETE:

        do
        {
            usb_dci_vusb20_process_tr_complete((usb_device_handle)(HandleEvt->handle));


            complete_status = get_complete_status();
        }
        while (complete_status);

        //bbu_printf("USB_STS_COMPLETE is comingr\r\n");


        post_info2Ao((usb_device_handle)(HandleEvt->handle), USB_PROCESS_COMPLETE);
        return Q_TRAN(&USB_Process_Complete);
    //return Q_HANDLED();
    case USB_ENABLE_ISR:
        //bbu_printf("usb enable isr\r\n");
        usb_isr_enable();
        return Q_HANDLED();
    case USB_SERIAL_READ:
        memcpy(usbHandlEvt, HandleEvt, sizeof(USBHandleEvt));
        // usb_serial_read((uint_8 *)(usbHandlEvt->handle),usbHandlEvt->length);

        return Q_HANDLED();
    }
    return Q_SUPER(&USB_App_process);
}





static   USBEndpointEvt *usbEndpointEvt;
static QState  USB_Process_Complete(USBApp *me, QEvent const *e)
{
    USBEndpointEvt *HandleEvt = (USBEndpointEvt *)e;


    switch (e->sig)
    {

    case Q_ENTRY_SIG:
    {

        QActive_recall((QActive *)me, &me->requestQueue);
        //bbu_printf("QActive_recall from usb request queue\r\n");
        //bbu_printf("enter USB_Process_Complete\r\n");
        return Q_HANDLED();
    }

    case USB_2_SD_TRAN:
        //bbu_printf("USB_2_SD_TRAN is coming\r\n");
        //usbEndpointEvt=HandleEvt;
        memcpy(usbEndpointEvt, HandleEvt, sizeof(USBEndpointEvt));
        if (storage_type == sd_storage)
            sd_infopost(usbEndpointEvt->start_addr, usbEndpointEvt->buffer_ptr, usbEndpointEvt->size / 512, usbEndpointEvt->tran_type);
        else if (storage_type == mmc_storage)
            mmc_infopost(usbEndpointEvt->start_addr, usbEndpointEvt->buffer_ptr, usbEndpointEvt->size / 512, usbEndpointEvt->tran_type);
        return Q_TRAN(&USB_SD_ACK);
#if 0
    case USB_READ_HANDLE:
        //bbu_printf("usb_read_handle\r\n");
        usb_bulk_endpoint4SDRead(usbEndpointEvt->handle, usbEndpointEvt->ep_num, usbEndpointEvt->buffer_ptr, usbEndpointEvt->size,
                                 usbEndpointEvt->usbcbw_process, usbEndpointEvt->csw_prt);

        return Q_HANDLED();
    case  USB_WRITE_HANDLE:
        usb_bulk_endpoint4SDWrite(usbEndpointEvt->handle, usbEndpointEvt->ep_num, usbEndpointEvt->buffer_ptr, usbEndpointEvt->size,
                                  usbEndpointEvt->usbcbw_process, usbEndpointEvt->csw_prt);
        return Q_HANDLED();
#endif
    case USB_STS_COMPLETE:
        bbu_printf("USB_STS_COMPLETE is %d\r\n", USB_STS_COMPLETE);
        return Q_HANDLED();
    case USB_PROCESS_COMPLETE:
        // complete_status=get_complete_status();
        // if(complete_status)
        // {
        // post_info2Ao((usb_device_handle)(HandleEvt->handle),USB_STS_COMPLETE);

        // }
        // else
        //bbu_printf("USB_PROCESS_COMPLETE\r\n");
        post_info2Ao((usb_device_handle)(HandleEvt->handle), USB_ENABLE_ISR);
        return Q_TRAN(&USB_DATE_handle);
    }

    return Q_SUPER(&USB_DATE_handle);
}


static QState USB_SD_ACK(USBApp *me, QEvent const *e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
    {
        //bbu_printf("Enter QState: USB_SD_ACK\r\n");
        return Q_HANDLED();
    }

    case USB_2_SD_TRAN:
    case USB_PROCESS_COMPLETE:
    {
        if (QEQueue_getNFree(&me->requestQueue) > 0)
        {
            QActive_defer(AO_USB, &me->requestQueue, e);
            //bbu_printf("QActive_defer to USB request queue\r\n");
        }
        else
            bbu_printf("Request ignored.\r\n");



        return Q_HANDLED();
    }

    case SD_TRANS_SUC_SIG:
        //bbu_printf("sd transfer suc\r\n");
        if (usbEndpointEvt->tran_type == SD_OPS_CARD_READ)
        {

            usb_bulk_endpoint4SDRead(usbEndpointEvt->handle, usbEndpointEvt->ep_num, usbEndpointEvt->buffer_ptr, usbEndpointEvt->size,
                                     usbEndpointEvt->usbcbw_process, usbEndpointEvt->csw_prt);

        }
        else if (usbEndpointEvt->tran_type == SD_OPS_CARD_WRITE)
            usb_bulk_endpoint4SDWrite(usbEndpointEvt->handle, usbEndpointEvt->ep_num, usbEndpointEvt->buffer_ptr, usbEndpointEvt->size,
                                      usbEndpointEvt->usbcbw_process, usbEndpointEvt->csw_prt);
        return Q_TRAN(&USB_Process_Complete);
    case SD_TRANS_FAIL_SIG:
        bbu_printf("SD Card failed\r\n");
        return Q_TRAN(&USB_Process_Complete);
    case MMC_TRANS_SUC_SIG:
        bbu_printf("mmc transfer suc\r\n");
        if (usbEndpointEvt->tran_type == SD_OPS_CARD_READ)
        {

            usb_bulk_endpoint4SDRead(usbEndpointEvt->handle, usbEndpointEvt->ep_num, usbEndpointEvt->buffer_ptr, usbEndpointEvt->size,
                                     usbEndpointEvt->usbcbw_process, usbEndpointEvt->csw_prt);

        }
        else if (usbEndpointEvt->tran_type == SD_OPS_CARD_WRITE)
            usb_bulk_endpoint4SDWrite(usbEndpointEvt->handle, usbEndpointEvt->ep_num, usbEndpointEvt->buffer_ptr, usbEndpointEvt->size,
                                      usbEndpointEvt->usbcbw_process, usbEndpointEvt->csw_prt);
        return Q_TRAN(&USB_Process_Complete);
    case MMC_TRANS_FAIL_SIG:
        bbu_printf("SD Card failed\r\n");
        return Q_TRAN(&USB_Process_Complete);
    }
    return Q_SUPER(&USB_DATE_handle);
}


uint16_t getTxHeadbufLength(void)
{
    if ((usb_service_type == usb_service_rndis) || (usb_service_type == usb3_service_rndis))
        return rndisHeadbugLength();
    else if (usb_service_type == usb_service_ecm)
        return 0;
    /*for NCM, MBIM and others*/
    else
        return 0;

}

void usb_tx_process(uint8_t *data, uint32_t length)
{
    USBEtherSendEvt *evt = (USBEtherSendEvt *) Q_NEW(USBEtherSendEvt, USB_ETHER_SEND_SIG);
    {
        evt->data = data;
        evt->length = length;
    }
    if (usb_service_type >= usb3_unknow)
        QACTIVE_POST((QActive *)AO_Usb3, (QEvent *)evt, NULL);
    else
        QACTIVE_POST((QActive *)AO_USB, (QEvent *)evt, NULL);




}

void usb_rx_process(uint8_t status)
{
    USBEtherReceiveEvt *evt = (USBEtherReceiveEvt *)Q_NEW(USBEtherReceiveEvt, USB_ETHER_RECE_SIG);
    evt->status = status;
    if (usb_service_type >= usb3_unknow)
        QACTIVE_POST((QActive *)AO_Usb3, (QEvent *)evt, NULL);
    else
        QACTIVE_POST((QActive *)AO_USB, (QEvent *)evt, NULL);


}

static void usbm_help(void)
{
    bbu_printf("\n\r USB storage test command:\r\n");
    bbu_printf(" Usage:usbm<init_ep_number><mode>\r\n");
    bbu_printf("<init_ep_number> -- init the number of endpoint ,value from 0 ,default is 0\r\n ");
    bbu_printf("<mode> -- choose usb speed mode,default is 0\r\n");
    bbu_printf("                  0:high speed mode\r\n");
    bbu_printf("                  1:full speed mode\r\n");
    bbu_printf(" Example:usbm 0 1\r\n");
}



static void usbm_mmcHelp(void)
{
    bbu_printf("\n\r USB mmc storage test command:\r\n");
    bbu_printf(" Usage:usbm_mmc<init_ep_number><mode>\r\n");
    bbu_printf("<init_ep_number> -- init the number of endpoint ,value from 0 ,default is 0\r\n ");
    bbu_printf("<mode> -- choose usb speed mode,default is 0\r\n");
    bbu_printf("                  0:high speed mode\r\n");
    bbu_printf("                  1:full speed mode\r\n");
    bbu_printf(" Example:usbm_mmc 0 1\r\n");
}

#endif

