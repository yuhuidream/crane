#ifndef _CLI_COMP_H_
#define _CLI_COMP_H_

#define RBUF_SIZE 1024    /*keep the lenght as power of two, so that the wrap is easy*/

typedef struct CliCompTag {
    QHsm super;               /* derive from QHsm */
    uint8_t RxBuf[RBUF_SIZE]; //the buffer for keep the input from cli
    uint16_t rIndex; //RxBuf read index
    uint16_t wIndex; //RxBuf write index
    uint16_t cmdIndex;
    uint8_t CmdHistory[NHISTORY][MAX_LINE_LEN];//array of command history in cronological order, circular buffer
    uint8_t InsMode;   //default is replace mode 0: Replace mode, 1: ins mode
    int8_t history_i; //index for the cmd history array,point to the next location after the last entered command
    int8_t history_j; //index for the cmd history array,pointer used to retrieve a previous command
    int8_t history_count; //number of commands in the history array.
} CliComp;

void CliComp_ctor(CliComp *me);
#define CliComp_init(me_)           QHsm_init((QHsm *)(me_), (QEvent *)0)
#define CliComp_dispatch(me_, e_)   QHsm_dispatch((QHsm *)(me_), e_)

#endif
