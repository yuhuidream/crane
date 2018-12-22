#include "dma_dec.h"

Q_DEFINE_THIS_FILE

typedef struct DMA_TAG{
    QActive super;                                     
}Dma;

static QState Dma_initial(Dma *me, QEvent const *e);
static QState Dma_process(Dma *me, QEvent const *e);

static Dma l_dma;
QActive * const AO_Dma = (QActive *)&l_dma;

/* DMA Construction */
void Dma_ctor(void){
    Dma *me = &l_dma;
    QActive_ctor(&me->super, (QStateHandler)&Dma_initial);
}

/* HSM definition ----------------------------------------------------------*/
/*..........................................................................*/
static QState Dma_initial(Dma *me, QEvent const *e){

    (void)e;//avoid warning.

    QS_OBJ_DICTIONARY(&l_dma);

    QS_FUN_DICTIONARY(&Dma_initial);
    QS_FUN_DICTIONARY(&Dma_process);

    QS_SIG_DICTIONARY(DMA_TRAN_FROM_COM_SIG, &l_dma);
    QS_SIG_DICTIONARY(DMA_TRAN_SIG, &l_dma);

    //Connect interrupt to this AO, when AO transfers, the interrupt be rerouted
    QA_IRQ_CONNECT(IRQ_MMP_DMA1, AO_Dma);
    
    return Q_TRAN(&Dma_process);
}

static QState Dma_process(Dma *me, QEvent const *e){
    switch (e->sig){
        case DMA_TRAN_FROM_COM_SIG:{
            ComCmdEvt *evt = (ComCmdEvt *)e;
            bbu_dma_test(evt->argc, evt->argv, e);
                
            return Q_HANDLED();
        }
        case DMA_STRESS_SIG:{
            uint32_t channel = ((DmaStressEvt *)e)->channel;
            uint32_t num = ((DmaStressEvt *)e)->num;
            uint32_t size = ((DmaStressEvt *)e)->size;
            dma_stress_test(channel, num, size, 0);

            return Q_HANDLED();
        }
        case DMA_TRAN_SIG:{
	    #if 0
            int ret = 0;
            DmaEvt *evt = (DmaEvt *)e;
            ret = DmaTran(&(evt->cfg));
            bbu_printf("DMA Transfer Done. ret = %d\r\n", ret);
	    #endif
            return Q_HANDLED();
        }
    }

    return Q_SUPER(&QHsm_top);
}

void bbu_dma_help(){
    bbu_printf("\r\nUsage: dma <command> [<args>]\r\n");
    bbu_printf("dma mdma <mode> [<direction>] [<channel>]   \n\r");
    bbu_printf("  -- test memory to memory transfer\r\n");
    bbu_printf("          mode: 0 -- fetch mode | 1 -- no fetch mode.\n\r");
    bbu_printf("          direction: 0 -- SQU <=> SQU \n\r");
    bbu_printf("                     1 -- SQU => DDR\n\r");
    bbu_printf("                     2 -- DDR <=> DDR\n\r");
    bbu_printf("                     3 -- SQU <= DDR\n\r");
    bbu_printf("                     4 -- all directions\n\r");
    bbu_printf("          channel:  0~31, 32: all channels\n\r");
    bbu_printf(" dma squ  -- squ dma test\n\r");
    bbu_printf("dma stress channel_start channel_num size\r\n");
    bbu_printf("  <channel_start>      -- start channel.\n\r");
    bbu_printf("  <channel_num>        -- stress test channel number.\n\r");
    bbu_printf("  <size>               -- stress test size.\n\r");
    bbu_printf("Example: dma stress 0 31 0x10000\r\n");
    bbu_printf("dma stop               -- stop stress test\r\n");
    bbu_printf("dma stress iter--show stress iter\r\n");
    bbu_printf("dma stress set_iter <iter> -- set stress iter\r\n");
    bbu_printf("dma xfer source_addr target_addr size\r\n");

    return;
}


void bbu_dma_test(uint32_t argc, uint8_t **argv, QEvent const *e){
    switch(argc){
        case 1:
            bbu_dma_help();
            break;
        case 2:
            if(check_help(argv[1])){
                bbu_dma_help();
                break;
            }
            if(strcmp((char *)argv[1], "mdma")==0){
                int err;
                //all directions, channel 0, fetch mode
                err = dma_m2m_xfer_test(0, 4, 0);	//default mode is chain mode.
                ComCmdEvt *cmdEvt = (ComCmdEvt *)e;
                screen_cmd_ack((err==0)?2:1, cmdEvt->scr_cmd_index);
                break;
            }

            if(strcmp((char *)argv[1], "squ")==0){
                squ_dma_test(0);
                break;
            }

            if(strcmp((char *)argv[1], "stop")==0){
                dma_stress_stop();
                break;
            }else{
                bbu_printf("DMA: error input argument.\r\n");
                break;
            }
            break;
        case 3:
            if(strcmp((char *)argv[1], "mdma")==0){
                uint32_t mode = conv_dec((char *)(argv[2]));
                if(mode > 1){
                    bbu_printf("DMA: mdma case need mode 1/0.\r\n");
                }else{
                    int err;
                    err = dma_m2m_xfer_test(mode, 4, 0);
                    ComCmdEvt *cmdEvt = (ComCmdEvt *)e;
                    screen_cmd_ack((err==0)?2:1, cmdEvt->scr_cmd_index);
                }
            }else if(strcmp((char *)argv[1], "squ")==0){
                uint32_t mode = conv_dec((char *)(argv[2]));
                squ_dma_test(mode);
                break;
            }else{
            	if(strcmp((char *)argv[1], "stress")==0){
            		dma_stress_show_iter();
            	}else{
            		bbu_dma_help();
            	}
            }
            break;
        case 4:
            if(strcmp((char *)argv[1], "mdma")==0){
                uint32_t mode = conv_dec((char *)(argv[2]));
                uint32_t direction = conv_dec((char *)(argv[3]));
                if(mode>1){
                    bbu_printf("DMA: mdma case need mode 1/0.\r\n");
                }else{
                    int err;
                    err = dma_m2m_xfer_test(mode, direction, 0);
                    ComCmdEvt *cmdEvt = (ComCmdEvt *)e;
                    screen_cmd_ack((err==0)?2:1, cmdEvt->scr_cmd_index);
                }
            }else if(strcmp((char *)argv[1], "stress")==0){
            	if(strcmp((char *)argv[2], "set_iter")==0){
            		uint32_t iter = conv_dec((char *)(argv[3]));
            		dma_stress_set_iter(iter);
            		break;
            	}
            }else 
                bbu_dma_help();
            break;
        case 5:
            if(strcmp((char *)argv[1], "mdma")==0){
                uint32_t mode = conv_dec((char *)(argv[2]));
                uint32_t direction = conv_dec((char *)(argv[3]));
                uint32_t channel = conv_dec((char *)(argv[4]));
                if(mode>1){
                    bbu_printf("DMA: mdma case need mode 1/0.\r\n");
                }else{
                    int err;
                    err = dma_m2m_xfer_test(mode, direction, channel);
                    ComCmdEvt *cmdEvt = (ComCmdEvt *)e;
                    screen_cmd_ack((err==0)?2:1, cmdEvt->scr_cmd_index);
                }
            }else if(strcmp((char *)argv[1], "stress")==0){
                uint32_t channel = conv_dec((char *)(argv[2]));
                uint32_t num = conv_dec((char *)(argv[3]));
                uint32_t size = conv_hex((char *)(argv[4]));
                
                if((channel+num)<33){
                    bbu_printf("DMA: stress test start.\r\n");
                    dma_stress_test(channel, num, size, 1);
                }else{
                    bbu_printf("DMA: channel_start+channel_num should be less than 33.\r\n");
                }
                break;
            } else if(strcmp((char *)argv[1], "xfer")==0){
                uint32_t src = conv_hex((char *)(argv[2]));
                uint32_t dst = conv_hex((char *)(argv[3]));
                uint32_t size = conv_hex((char *)(argv[4]));
                uint32_t res = 0;
                res = dma_m2m_xfer(src, dst, size, DMA_MAX_BYTES_PER_DESC, DMA_BURST_SIZE_32B, DMA_XFER_CHANNEL);
                if(res){
                    bbu_printf("DMA: xfer success!\r\n");
                    bbu_printf("src_addr: 0x%08X | trg_addr: 0x%08X | total size: %d bytes\r\n", src, dst, size);
                }else{
                    bbu_printf("DMA: xfer fail!\r\n");
                }
            }else{
                bbu_dma_help();
            }
            break;
        default:
            bbu_dma_help();
            break;
    }

    return;
}

