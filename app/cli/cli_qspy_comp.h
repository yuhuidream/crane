#ifndef _CLI_QSPY_COMP_H_
#define _CLI_QSPY_COMP_H_
#include <string.h>  /*newlibc should support it, otherwise need to code some basic operations*/
#include "qp_port.h"
#include "bsp.h"
#include "bbu.h"
#include "ascii.h"
#include "utils.h"
#include "uart_dec.h"
#include "gic_dec.h"

#ifdef Q_SPY_EXT
#define RBUF_SIZE 1024    /*keep the lenght as power of two, so that the wrap is easy*/

enum AppRecords {                 /* application-specific trace records */
    OUTPUT_DATA = QS_USER
};

typedef struct CliQspyCompTag {
    QHsm super;               /* derive from QHsm */
    uint8_t     Qspy_RxBuf[RBUF_SIZE]; 
    uint16_t    Qspy_rIndex; 
    uint16_t    Qspy_wIndex; 
    uint16_t    Qspy_cmdIndex;
    uint8_t     Qspy_CmdHistory[NHISTORY][MAX_LINE_LEN];
    uint8_t     Qspy_InsMode;   
    int8_t      Qspy_history_i; 
    int8_t      Qspy_history_j; 
    int8_t      Qspy_history_count;
} CliQspyComp;

void CliQspyComp_ctor(CliQspyComp *me);
#define CliQspyComp_init(me_)           QHsm_init((QHsm *)(me_), (QEvent *)0)
#define CliQspyComp_dispatch(me_, e_)   QHsm_dispatch((QHsm *)(me_), e_)

void spy_multi_filter_handler(uint8_t argc, uint8_t** argv);

#endif
#endif
