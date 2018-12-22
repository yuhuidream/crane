/*
 * $Copyright (c) 2008-2009, ARM Ltd. All rights reserved.
 * -------------------------------------------------------
 * File:     cli.c
 * $ *
 * Command Line Interface
 *
 */

#include <string.h>
#include "ascii.h"
#include "predefines.h"
#include "UART.h"
#include "print.h"
#include "bbu.h"

#undef CMDLINE_MAX_ARGS
#define NCMDS 7
#define CMDLINE_MAX_ARGS 10
#define CMDSIZE 128
#define NHISTORY 4


extern int bbu_setv_test(int argc, uint8_t **argv);
extern int bbu_smtd(int argc, uint8_t **argv);
extern int bbu_mtsp(int argc, uint8_t **argv);
extern int bbu_mtest_func(int argc, uint8_t **argv);
extern int bbu_help(int argc, uint8_t **argv);
extern int bbu_pid_test(int argc, uint8_t **argv);
extern int bbu_ppset_test(int argc, uint8_t **argv);
extern int bbu_opset_test(int argc, uint8_t **argv);
extern int bbu_ddr_sweep_test(int argc, uint8_t **argv);
extern int bbu_peek(int argc, uint8_t **argv);
extern int bbu_poke(int argc, uint8_t **argv);
extern int bbu_transfer2normal(int argc, uint8_t **argv);
extern int bbu_reset(int argc, uint8_t **argv);
extern int bbu_release_seagull(int argc, uint8_t **argv);
extern int ddr_init(int argc, uint8_t **argv);
extern int bbu_uart_ctrl(int argc, uint8_t **argv);
extern int bbu_delay_test(int argc, uint8_t **argv);
#ifdef NANO_SDH
extern int bbu_mmc_init(int argc, uint8_t **argv);
extern int bbu_mmc_read(int argc, uint8_t **argv);
extern int bbu_mmc_write(int argc, uint8_t **argv);
#ifdef NANO_SDH_ERASE
extern int bbu_mmc_erase(int argc, uint8_t **argv);
#endif
extern void bbu_mmc_switch(int argc, uint8_t **argv);
#endif
extern int bbu_qspi_test(int argc, uint8_t **argv);

struct cmd_struct
{
    char *string;
    int (*function)(int, uint8_t **);
    char *help;
};

/*
 * Description:
 * Commands control structure
 *
 * Remarks:
 * This structure associates command line commands with a
 * function and a brief description string.
 */
static struct cmd_struct cmds[] =
{
   {"help",   bbu_help, "BBU help message print command"},
#ifdef NANO_SDH
   {"immc",   bbu_mmc_init, "eMMC init cmd"},
   {"mwrite", bbu_mmc_write, "eMMC write cmd"},
   {"mread",  bbu_mmc_read, "eMMC read cmd"},
#ifdef NANO_SDH_ERASE
   {"merase",  bbu_mmc_erase, "eMMC erase cmd"},
#endif
   {"mswitch", bbu_mmc_switch, "eMMC partition switch command"},
#endif
   {"qspi",  bbu_qspi_test, "Qspi test"},
   {"mtsp", bbu_mtsp, "Memory Test Subroutine Package command"},
   {"peek", bbu_peek, "peek command"},
   {"poke", bbu_poke, "peek command"},
//   {"ppset",  bbu_ppset_test, "PP change command of AP subsystem"},
//   {"opset",  bbu_opset_test, "OP change command of AP subsystem"},
   //{"pid",    bbu_pid_test,  "Get Processor ID command"},
   {"setv",   bbu_setv_test, "Set VBUCK voltage command"},
   {"delay",  bbu_delay_test, "Test delay command"},
   {0, 0, 0, 0},
   //{"reset",  bbu_reset, "Reset command"},
   //{"release", bbu_release_seagull, "Release seagull"},
   //{"iddr", ddr_init, "Initialize DDR"},
    /* Add user defined commands here - NCMDS needs to be updated accordingly */
};



int bbu_help(int argc, uint8_t **argv){
   (void)argc;
   (void)argv;
   int i = 0;
   //uint8_t cmdNum = sizeof(cmds)/sizeof(struct cmd_struct);
   bbu_printf("\n\r");
   //bbu_printf("cmdNum %d\n\r", cmdNum);
   while(cmds[i].string != 0)
   {
      bbu_printf("%-10s    %s\n\r", cmds[i].string, cmds[i].help);
      i++;
   }
   return 0;
}


/*
 * Description:
 * Array containing an history of used commands (in cronological order)
 *
 */
//static char cmd_history[NHISTORY][CMDSIZE];


/*
 * Description:
 * Index for the command history array
 *
 * Remarks:
 * The index points to the next location after the last entered command
 */
//static uint32_t history_i = 0;


/*
 * Description:
 * String to hold the current command to be executed
 *
 */
static char cmd[CMDSIZE];


/*
 * Description:
 * Character index of the last command being entered
 *
 */
static uint32_t cmd_i = 0;


/*
 * Description:
 * Index for the command history array
 *
 * Remarks:
 * Alternative pointer to parse through the command history array
 * to retrieve a previously used command.
 *
 */
//static int history_j = 0;


/*
 * Description:
 * Number of elements currently in the command history array
 *
 * Remarks:
 * This counter never decrements. Once the array is full, it is
 * treated as a circular buffer
 *
 */
int history_count = 0;


volatile int execute = 0;


void uart_char_handler(char c){
        switch(c)
        {
            case ASCII_ESC: /* Escape */
//                c = serial_getc(CONSOLE);
//                if (c == ESC_SEQ)
//                {
//                    c = serial_getc(CONSOLE);
//                switch (c)
//                {
//                    case ASCII_UP: /* Up-arrow; use previous command */
//
//                        for ( ; cmd_i > 0 ; cmd_i--)
//                        {
//                            bbu_putchar( ASCII_BS);
//                            bbu_putchar( ASCII_SP);
//                            bbu_putchar( ASCII_BS);
//                        }
//
//                          if (--history_j < 0)
//                            history_j = history_count - 1;
//
//                        strcpy(cmd, cmd_history[history_j]);
//                        bbu_putstr( cmd);
//
//                        cmd_i = strlen(cmd);
//
//                        break;
//
//                    case ASCII_DOWN: /* Down-arrow; use next command */
//
//                        for ( ; cmd_i > 0 ; cmd_i--)
//                        {
//                            bbu_putchar( ASCII_BS);
//                            bbu_putchar( ASCII_SP);
//                            bbu_putchar( ASCII_BS);
//                        }
//
//                        history_j = ++history_j % history_count;
//
//                        strcpy(cmd, cmd_history[history_j]);
//                        bbu_putstr( cmd);
//
//                        cmd_i = strlen(cmd);
//
//                        break;
//
//                     default:
//                        break;
//            }
//                }
            break;

            case ASCII_BS:
            case ASCII_DEL: /* Delete */
                if (cmd_i > 0)
                {
                    bbu_putchar( ASCII_BS);
                    bbu_putchar( ASCII_SP);
                    bbu_putchar( ASCII_BS);
                    cmd_i--;
                }
                break;

            case ASCII_CR : /* Execute command - CR, CR+LF */
                //bbu_putchar( ASCII_LF);
                cmd[cmd_i] = '\0';
                cmd_i = 0;
                execute = MV_TRUE;
                bbu_printf("\n\r"BBU_PROMPT);
                break;

            case ASCII_LF : /* Execute command - LF */
                if (cmd_i!=0) {
                    //bbu_putchar( ASCII_LF);
                    cmd[cmd_i] = '\0';
                    cmd_i = 0;
                    execute = MV_TRUE;
                }
                bbu_printf("\n\r"BBU_PROMPT);
                break;

            default: /* Valid character */
                //work around the issue UART always get a Null character after CR
                #ifdef CONFIG_Z1
                if(c == 0)
                {
                    break;
                }
                #endif
                if (cmd_i < CMDSIZE - 1)
                {
                    bbu_putchar( c);
                    cmd[cmd_i++] = c;
                }
                break;
        }
}


 void cli_uart_handler(void)
{
    char c;
    uint32_t uart_iir = DBG_UART->IIR;

    //the Rx timeout interrupt indicates there are trailing bytes, need to follow the steps in manual
    //step1: read the LSR and check for errors (this is same for all cases and highest interrupt inside UART)
    uint32_t uart_lsr= DBG_UART->LSR;
    if (uart_lsr & (LSR_FIFOE|LSR_FE|LSR_PE|LSR_OE)) {
        bbu_printf("LSR error : %x\n",uart_lsr);
    }

    if ((uart_iir & IIR_IID10_MSK)==SHIFT1(0x2)) {//If Rx FIFO/timeout interrupt 
        if (uart_iir & IIR_TOD) { //timeout interrupt
            //step2: disable the Rx timeout interrupt
            DBG_UART->IER &= ~IER_RTOIE;
            //step3: receive data available
            for (; uart_lsr & LSR_DR;  ) {
                uint32_t uart_for=0,i=0;
                uart_for = DBG_UART->FOR;
                // read out the rest of characters in FIFO
                for (i=0;i<uart_for;i++) {
                    c = DBG_UART->RBR;
                    uart_char_handler(c);
                }
                //step4:read LSR, check for errors; and go to step3 if LSR[DR] is set else go to step 5.
                uart_lsr= DBG_UART->LSR;
                if (uart_lsr & (LSR_FIFOE|LSR_FE|LSR_PE|LSR_OE)) {
                    bbu_printf("LSR error again : %x\n",uart_lsr);
                }

            }

            //step5: no more data in FIFO,re-enable Receive Timeout
            DBG_UART->IER |= IER_RTOIE;
        }
        else { //If Rx FIFO threshold interrupt  
            for (; DBG_UART->LSR & LSR_DR;) {
                c = DBG_UART->RBR;
                uart_char_handler(c);
            }
        }
    }
    else if ((uart_iir & IIR_IID10_MSK)==SHIFT1(0x1)) {  //If Tx FIFO low interrupt

    }
    else {
        //bbu_printf("This interrupt is not used in current UART app!\n");
    }

}

  void cli_execute_command(char *cmd, int argc, uint8_t **args)
{
    int i = 0;
    int cmdNum = sizeof(cmds)/sizeof(struct cmd_struct);
    if (cmd == (char *)NULL)
        return;

    while (cmds[i].string != 0)
    {
        if (strncmp(cmd, cmds[i].string, sizeof(cmds[i].string)) == 0)
        {
            if(cmds[i].function)
               (cmds[i].function)(argc, args);
            else
               return;
            bbu_printf(BBU_PROMPT);
            return;
        }
        i++;
    }

    //if (i == cmdNum)
    bbu_printf("Unrecognized command '%s' %d %d\n\r"BBU_PROMPT, cmd, cmdNum, i);
}

void cli_do_cmd(char* cmd)
{
    char *pcChar;
    char command[CMDSIZE];
    uint8_t *argv[CMDLINE_MAX_ARGS];

    command[0] = '\0';

    if (cmd[0] == '\0')
        return;

    //strcpy(cmd_history[history_i], cmd);
    //history_i = ++history_i % NHISTORY;

    //if (history_count < NHISTORY)
    //    ++history_count;

    //history_j = history_i;

    /* Remove front spaces */
    cmd += strspn(cmd, " ");

    strcpy((char *)command, (const char*)cmd);
    pcChar = command;
    int argc = 0;
    int bFindArg = 1;
    
    while(*pcChar)
    {
        if(*pcChar == ' ' || *pcChar == '\t')
        {
            //if it's space, then replace it with zero and set the flag to search for the next arg 
            *pcChar = '\0';
            bFindArg = 1;
        }
        else{ //it is not space, so it must be a char in an argument
            if(bFindArg)
            {
                if(argc < CMDLINE_MAX_ARGS)
                {
                    argv[argc] = (uint8_t *)pcChar;
                    argc++;
                    bFindArg = 0;
                }
                else{
                    bbu_printf("Too many arguments entered...\r\n");
                    bbu_printf("\r\n"BBU_PROMPT); /*we can show core name later*/
                    return;
                }
            }
        }
        pcChar++;
    }
    cli_execute_command(command, argc, argv);
}

 void cli_handler(void)
{

    /* Loop, executing commands everytime the user presses <ENTER> */
    while (1)
    {

        while(!execute){

#ifdef CONFIG_MMT
            if(output_is_usb())
                usb_dci_vusb20_isr();
#endif			

#ifdef CONFIG_NANO_NO_INT
            cli_uart_handler();
#endif
        };
        if(execute)
            cli_do_cmd(cmd);

        execute = 0;
    }
}
