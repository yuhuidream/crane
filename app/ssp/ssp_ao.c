#include "ssp_dec.h"
#include "bbu_help.h"
#include "ssp_dec.h"
#include "ssp_gd5f.h"

Q_DEFINE_THIS_FILE

typedef struct SSPAoTag{
    QActive super;                              
}SSP;    

static QState ssp_initial(SSP *me, QEvent const *e);
static QState ssp_init(SSP *me, QEvent const *e);
static QState ssp_process(SSP *me, QEvent const *e);

static SSP l_ssp;
QActive * const AO_SSP = (QActive *)&l_ssp;

void SSP_ctor(void){
    SSP *me = &l_ssp;
    QActive_ctor(&me->super, (QStateHandler)&ssp_initial);
}

QState ssp_initial(SSP *me, QEvent const *e){
    (void)me;
    (void)e;    

    return Q_TRAN(&ssp_init);
}

QState ssp_init(SSP *me, QEvent const *e){
    switch(e->sig){
        case SSP_INIT_SIG:{
            ComCmdEvt *evt = (ComCmdEvt *)e;
            if(evt->argc == 2){
                uint32_t clk = conv_dec((char*)(evt->argv[1]));
                switch(clk){
                    case 0:{
                        bbu_printf("SSP: ssp has init with 6.5M clock\r\n");
                        break;
                    }
                    case 1:{
                        bbu_printf("SSP: ssp has init with 13M clock\r\n");
                        break;
                    }
                    case 2:{
                        bbu_printf("SSP: ssp has init with 26M clock\r\n");
                        break;
                    }
                    case 3:{
                        bbu_printf("SSP: ssp has init with 52M clock\r\n");
                        break;
                    }
                    default:{
                        bbu_printf("SSP: clock only support 0~3\r\n");
                        return Q_SUPER(&QHsm_top);
                    }
                }
                ssp_init_gd5f(clk);
                return Q_TRAN(&ssp_process);
            }else{
                bbu_printf("SSP: please select clock\r\n");
                return Q_SUPER(&QHsm_top);
            }
        }
        case SSP_TEST_SIG:{
            ComCmdEvt *evt = (ComCmdEvt *)e;
            bbu_ssp_test_new(evt->argc, evt->argv);
            return Q_HANDLED();
        }
        case SSP_LPBK_SIG:{
            ComCmdEvt *evt = (ComCmdEvt *)e;
            bbu_ssp_test(evt->argc, evt->argv);
            return Q_HANDLED();
        }
        case SSP_SPI_NOR_SIG:{
            ComCmdEvt *evt = (ComCmdEvt *)e;
            bbu_spi_nor_test(evt->argc, evt->argv);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

QState ssp_process(SSP *me, QEvent const *e){
    (void)me;
    switch(e->sig){
        case SSP_TEST_SIG:{
            ComCmdEvt *evt = (ComCmdEvt *)e;
            ssp_test_gd5f(evt->argc, evt->argv);
            return Q_HANDLED();
        }
        case SSP_LPBK_SIG:{
            ComCmdEvt *evt = (ComCmdEvt *)e;
            bbu_ssp_test(evt->argc, evt->argv);
            return Q_HANDLED();
        }
    }
    
    return Q_SUPER(&QHsm_top);
}
