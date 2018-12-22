#include <string.h>  /*newlibc should support it, otherwise need to code some basic operations*/
#include "qp_port.h"
#include "bsp.h"
#include "bbu.h"
#include "ascii.h"
#include "utils.h"
#include "uart_dec.h"
#include "gic_dec.h"
#include "cli_comp.h"
#ifdef CONFIG_ATE
#include "ate.h"
#endif

Q_DEFINE_THIS_FILE

extern CmdLineEntry gCmdTable[MAX_CMD_NUM];
                                          /* hierarchical state machine ... */
static QState Cli_comp_initial       (CliComp *me, QEvent const *e);
static QState Cli_comp_receiving     (CliComp *me, QEvent const *e);
static QState Cli_comp_processing    (CliComp *me, QEvent const *e);
static QState Cli_comp_idle          (CliComp *me, QEvent const *e);
static QState Cli_comp_help          (CliComp *me, QEvent const *e);

/*..........................................................................*/
void CliComp_ctor(CliComp *me) {
    QHsm_ctor(&me->super, (QStateHandler)&Cli_comp_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState Cli_comp_initial(CliComp *me, QEvent const *e) {
    (void)e;               /* avoid compiler warning about unused parameter */

    me->rIndex = 0;
    me->wIndex = 0;
    me->InsMode = 0;
    me->history_i = 0; 
    me->history_j = 0; 
    me->history_count = 0;
    /* Connect interrupt to this AO, when AO transfers, the interrupt be
    * rerouted
    */
    QA_IRQ_CONNECT(IRQ_MMP_UART_CP,      AO_Cli);
    return Q_TRAN(&Cli_comp_receiving);
}

/*..........................................................................*/
QState Cli_comp_receiving(CliComp *me, QEvent const *e) {
    switch (e->sig) {
        case Q_INIT_SIG: {
            //bbu_printf("\r\n"BBU_PROMPT);
            return Q_TRAN(&Cli_comp_processing);
        }

        case COM_RX_REQUEST_SIG: {  /*Rx interrupts event ComRxEvt*/
            ComCliEvt* reminderEvt = Q_NEW(ComCliEvt, COM_RX_NEXT_BYTE_SIG);
		    for(int i = 0; i<((ComRxEvt const *)e)->len; i++)
		    {
		        me->RxBuf[me->wIndex++] = ((ComRxEvt const *)e)->data[i];
		        me->wIndex &= (RBUF_SIZE - 1);
		        Q_ASSERT(((me->wIndex + 1)&(RBUF_SIZE - 1)) != me->rIndex); //buffer is not full
		        //otherwise we may need to enlarge the RBUF_SIZE
		    }

            QACTIVE_POST(AO_Cli, (QEvent*)reminderEvt, me);
            return Q_HANDLED();
        }

        case COM_RX_NEXT_BYTE_SIG: {
            static ComCliEvt *cce = 0;
            if ( 0==cce )
            {
                cce = Q_NEW(ComCliEvt,COM_RX_CMD_READY_SIG);
                cce->len = 0;
                cce->cursor = 0;
                memset(cce->CliBuffer, 0, MAX_LINE_LEN);
            }
            while(me->rIndex != me->wIndex)
            {   
                switch (me->RxBuf[me->rIndex]) {
                    case ASCII_CTRL_Z:
                        cce->len = 0;
                        cce->cursor = 0;
                        bbu_printf("\n\r"BBU_PROMPT);
                        break;
                    case ASCII_LF: 
                    case ASCII_CR: {
                        if(cce->len){
                            if(cce->len > MAX_LINE_LEN)
                            {
                             bbu_printf("\nThe command line is too long to support...\n");
                            }
                            else
                            {
                                cce->CliBuffer[cce->len] = '\0'; //NULL to terminate
                                strcpy((char *)me->CmdHistory[me->history_i], (const char *)cce->CliBuffer);
                                me->history_i = ++(me->history_i) % NHISTORY;
                                if(me->history_count < NHISTORY)
                                {
                                    ++me->history_count;
                                }
                                me->history_j = me->history_i;
                                cce->source = 0;  /* show command come from UART */

                                QACTIVE_POST(AO_Cli, (QEvent *)cce, me);
                                cce = Q_NEW(ComCliEvt,COM_RX_CMD_READY_SIG);
                            }
                            cce->len = 0;
                            cce->cursor = 0;
                            memset(cce->CliBuffer, 0, MAX_LINE_LEN);
                        }
                        bbu_printf("\r\n"BBU_PROMPT); /*we can show core name later*/
                       // QActive_postFIFO((QActive *)me, &reminderEvt); /*always send this in default branch?*/
                        break;
                    }
                    case BBU_KEY_INS: {
                        me->InsMode = !(me->InsMode);
                        break;
                    }
                    case BBU_KEY_UP:
                        //last command
                        for (int i = 0; i<(cce->len-cce->cursor); i++) //put cursor to the end
                        {
                            bbu_printf(" ");
                        }
                        for ( ; cce->len > 0; cce->len--)
                        {
                            bbu_printf("\b \b");
                        }
                        if(--me->history_j<0)
                        {
                            me->history_j = me->history_count - 1; //circular 
                        }
                        strcpy((char *)cce->CliBuffer, (const char*)me->CmdHistory[me->history_j]);
                        cce->len = strlen((const char*)me->CmdHistory[me->history_j]);
                        cce->cursor = cce->len;
                        bbu_putstr((const char*)me->CmdHistory[me->history_j], cce->len);
                        break;
                    case BBU_KEY_DOWN:
                        //next command
                        for (int i = 0; i<(cce->len-cce->cursor); i++) //put cursor to the end
                        {
                            bbu_printf(" ");
                        }
                        for ( ; cce->len > 0; cce->len--)
                        {
                            bbu_printf("\b \b");
                        }
                        me->history_j = ++me->history_j % me->history_count;
                        strcpy((char *)cce->CliBuffer, (const char*)me->CmdHistory[me->history_j]);
                        cce->len = strlen((const char*)me->CmdHistory[me->history_j]);
                        cce->cursor = cce->len;

                        bbu_putstr((const char*)me->CmdHistory[me->history_j], cce->len);
                        break;
                    case BBU_KEY_LEFT:
                        //move cursor, not index
                        if(cce->cursor)
                        {
                            cce->cursor--;
                            bbu_printf("\b");
                        }
                        break;
                    case BBU_KEY_RIGHT:
                        //move cursor, not index
                        if(cce->CliBuffer[cce->cursor])
                        {
                            bbu_putchar(cce->CliBuffer[cce->cursor]);
                            cce->cursor++;
                        }
                        break;
                    case ASCII_BS:
                    case ASCII_DEL:
                        //delete a char backward
                        if(!cce->cursor)
                            break;
                        if(me->InsMode)
                        {
                            bbu_printf("\b");
                            bbu_putstr((const char*)(&(cce->CliBuffer[cce->cursor])), cce->len - cce->cursor);
                            bbu_printf(" ");
                            for(int i = cce->cursor; i<cce->len; i++)
                            {
                                cce->CliBuffer[i-1] = cce->CliBuffer[i];
                            }
                            cce->CliBuffer[cce->len-1] = '\0';
                            for (int i = 0; i<=(cce->len-cce->cursor); i++)
                            {
                                bbu_printf("\b");
                            }
                            cce->len--;
                            cce->cursor--;

                        }else{
                            bbu_printf("\b \b");
                            cce->CliBuffer[cce->cursor-1] = ' ';
                            if(cce->cursor == cce->len)cce->len--;
                            cce->cursor--;
                       }
                        break;
                    case ASCII_HT:{
                       char str1[32], str2[32];
                       int cmd_idx[64] = {0};
                       int val1 = 0, val2 = 0, match_num = 0;
                       for(int i = 0;i<cce->cursor;i++){
                          if(cce->CliBuffer[i] == ASCII_SP){
                             val2 = 1;
                             i = cce->cursor;
                          }
                       }
                       if(!val2){
                          val2 = 0;
                          for(val1 = 0; val1<cce->cursor;val1++){
                             str1[val1] = cce->CliBuffer[val1];
                             str1[val1+1] = 0;
                          }

                          Q_ASSERT(sizeof(str1) > cce->cursor);

                          while(gCmdTable[val2].Cmd != 0){
                             strncpy(str2, gCmdTable[val2].Cmd,val1);
                             str2[val1] = 0;
                             if(strcmp(str2, str1)==0){
                                cmd_idx[match_num] = val2;
                                match_num ++;
                             }
                             val2 ++;
                          }

                          Q_ASSERT(sizeof(cmd_idx)/sizeof(cmd_idx[0]) >= match_num);

                          if(match_num == 1){
                             for(uint32_t i = val1; i < strlen((const char*)(gCmdTable[cmd_idx[0]].Cmd)); i++){
                                bbu_putchar(*((gCmdTable[cmd_idx[0]].Cmd)+i));
                                if(cce->cursor < cce->len)
                                   for(int i = cce->len; i > cce->cursor; i--)
                                      cce->CliBuffer[i] = cce->CliBuffer[i-1];
                                cce->len ++;
                                  cce->CliBuffer[cce->cursor] = *((gCmdTable[cmd_idx[0]].Cmd)+i); 
                                  if(cce->cursor == cce->len)cce->len++;
                                  cce->cursor++;
                             }
                             cce->CliBuffer[cce->cursor] = ASCII_SP;
                             cce->cursor++;
                             cce->len++;
                             for(int i = cce->cursor; i < cce->len; i++){
                                bbu_putchar(cce->CliBuffer[i]);
                             }
                             for(int i = cce->cursor; i < cce->len; i++){
                                bbu_printf("\b");
                             }
                             bbu_putchar(' ');
                          }
                          else if(match_num >1){
                             val2 = cce->len - cce->cursor;
                             if(me->RxBuf[me->rIndex-1]==ASCII_HT){
                               me->RxBuf[me->rIndex]=ASCII_SP;
                               bbu_printf("\r\n");
	                             for(int i = 0;i<match_num;i++){
	                                bbu_printf("%-10.10s ",gCmdTable[(cmd_idx[i])].Cmd);
	                                if((i+1)%5 == 0 && (i+1) != match_num)
	                                   bbu_printf("\r\n");
	                             }
                                     bbu_printf("\n\r");
                             }
                             int equal = 1;
                             while(equal){
                                int idx = 0;
                                for(idx = 0; idx<match_num-1;idx++)
                                {
                                   if(*((gCmdTable[cmd_idx[idx]].Cmd)+val1) != *((gCmdTable[cmd_idx[idx+1]].Cmd)+val1)){
                                      equal = 0;
                                      idx = match_num;
                                   }
                                }
                                if(equal){
                                   if(cce->cursor < cce->len)
                                   for(int i = cce->len; i > cce->cursor; i--)
                                      cce->CliBuffer[i] = cce->CliBuffer[i-1];
                                   cce->len++;
                                   cce->CliBuffer[cce->cursor] = *((gCmdTable[cmd_idx[idx]].Cmd)+val1);
                                   cce->cursor++;
                                   val1 ++;
                                }
                             }
                             bbu_printf("\r"BBU_PROMPT);
                             for(int i =0; i< cce->len;i++)
                                bbu_putchar(cce->CliBuffer[i]);
                             for(int i =0;i<val2;i++)
                                bbu_printf("\b");
                             
                          }
                       }
                       break;
                    }
                    default: {
                        //fill the char into ComCliEvt cmd buffer
                        //QActive_postFIFO((QActive *)me, &reminderEvt);
                       bbu_putchar(me->RxBuf[me->rIndex]);
                       if(me->InsMode)
                        {
                            bbu_putstr((const char*)(&(cce->CliBuffer[cce->cursor])), cce->len - cce->cursor);
                          
                            for(int i = cce->len; i>cce->cursor; i--)
                            {
                                cce->CliBuffer[i] = cce->CliBuffer[i-1];
                            }
                            cce->CliBuffer[cce->cursor] = me->RxBuf[me->rIndex];
                            for (int i = 0; i<(cce->len-cce->cursor); i++)
                            {
                                bbu_printf("\b");
                            }
                            cce->len++;
                            cce->cursor++;

                        }else{
                            cce->CliBuffer[cce->cursor] = me->RxBuf[me->rIndex];
                            if(cce->cursor == cce->len)cce->len++;
                            cce->cursor++;
                       }                        
                        break;
                    }

                }
                me->rIndex++;
                me->rIndex &= (RBUF_SIZE - 1);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Cli_comp_processing(CliComp *me, QEvent const *e) {
    switch (e->sig) {
        case Q_INIT_SIG: {
            return Q_TRAN(&Cli_comp_idle);
        }
        case COM_RX_CMD_READY_SIG: {

/****************************************************************************
*
* Process a command line string into arguments and execute the command.
* The supplied command line string will be broken up into individual arguments.
* The first argument is treated as a command and is searched for in the (command,signal) table.
* If the command is found, then the command event is posted to relevant AO with the command line arguments 
*
* The command table is contained in an array named gCmdTable which
* must be provided by the application.
*
* Print warning message to console if the command is not found,
* Print warning message to console if there are more arguments than allowed.
* Otherwise it posts the event to relevant AO.
*
*****************************************************************************/
            ComCmdEvt *cmdeTemp = (ComCmdEvt *)0;
            ComCmdEvt *cmde = Q_NEW(ComCmdEvt,MAX_SIG); //the signal to be updated once it's decided, here is dummy
            uint8_t *pcChar;
            uint32_t bFindArg = 1;
            CmdLineEntry *pCmdEntry;
            
            strcpy((char *)cmde->cmdline, (const char*)(((ComCliEvt const *)e)->CliBuffer));
            pcChar = cmde->cmdline;
            cmde->argc = 0;
            cmde->cmd_number = 0;

            while(*pcChar)
            {
                if(*pcChar == ' ' || *pcChar == '\t')
                {
                    //if it's space, then replace it with zero and set the flag to search for the next arg 
                    *pcChar = '\0';
                    bFindArg = 1;
                }else if(*pcChar == '&' && *(pcChar+1) == '&'){
                    *pcChar++ = '\0';
                    *pcChar++ = '\0';
                    cmdeTemp = Q_NEW(ComCmdEvt,MAX_SIG);
                    cmdeTemp->argc = 0;
                    cmdeTemp->cmd_number = 0;
                    strcpy((char *)cmdeTemp->cmdline, pcChar);
                    //Handle the post this command
                    if(cmde->argc)
                    {
                         char *p_help = "help";
                         char *p_malloc = "malloc";
         
                         //{"foo", AO_Demo, SYSTEM_CMD_DEMO_SIG, "The demo test command."},
                         pCmdEntry = &gCmdTable[0];
         
                         if(!strcmp((const char*)(cmde->argv[0]),p_help)) {
                         //HELP state to display it to user
                         //The HELP state need to override the COM_RX_NEXT_BYTE_SIG event so that any user key will
                         //lead to the continue display of the command table. 
                             QACTIVE_POST(AO_Cli,(QEvent *)cmde, me); //dummy post so that memory released,AO_cli does not deal with MAX_SIG
         //                    bbu_printf(CRIT_MSG "\r\n"BBU_PROMPT); /*we can show core name later*/
                             return Q_TRAN(&Cli_comp_help);
                         }
                        else if(!strcmp((const char*)cmde->argv[0],p_malloc)){
                             bbu_malloc_test(cmde->argc, cmde->argv);
                         }
         #ifdef CONFIG_SMP
                         else if(!strcmp((const char*)cmde->argv[0],"sgi")){
                             sgi_broadcast_all_but_self(GIC_INT_SW_12);
                         }
         #endif
                         else{
         
                             while(pCmdEntry->Cmd)
                             {
                                 if(!strcmp((const char*)cmde->argv[0],pCmdEntry->Cmd))
                                 {
                                     if(((const ComCliEvt *)e)->source)
                                     {
                                         ((ComCmdEvt *)cmde)->scr_cmd_index = ((ScreenCmdEvt *)e)->cmd_index;
                                     } 
                                     else
                                     {
                                         ((ComCmdEvt *)cmde)->scr_cmd_index = 0;
                                     } 
                                     ((QEvent *)cmde)->sig = pCmdEntry->EvtSig;
                                     QACTIVE_POST(pCmdEntry->AO_Cmd, (QEvent *)cmde, me);
                                     cmde = cmdeTemp;
                                     break;
                                 }
                                 pCmdEntry++;
                                 cmde->cmd_number++;
                             }
                        }
                    }
                    if(cmde->cmd_number) /* command like 'help' its cmd_number should be 0 */
                        bbu_printf(CRIT_MSG "not supported command...\r\n\n");
                    QACTIVE_POST(AO_Cli,(QEvent *)cmde, me); //dummy post so that memory released,AO_cli does not deal with MAX_SIG

                    cmde = cmdeTemp;
                    pcChar = cmde->cmdline;
                    continue;
                }
                else{ //it is not space, so it must be a char in an argument
                    if(bFindArg)
                    {
                        if(cmde->argc < CMDLINE_MAX_ARGS)
                        {
                            cmde->argv[cmde->argc] = pcChar;
                            cmde->argc++;
                            bFindArg = 0;
                        }
                        else{
                            bbu_printf("Too many arguments entered...\r\n");
                            QACTIVE_POST(AO_Cli,(QEvent *)cmde,me); //dummy post so that memory released,AO_cli does not deal with MAX_SIG
                            bbu_printf("\r\n"BBU_PROMPT); /*we can show core name later*/
                            return Q_HANDLED();
                        }
                    }
                }
                pcChar++;
            }
            
            if(cmde->argc)
            {
                char *p_help = "help";
                char *p_malloc = "malloc";

                //{"foo", AO_Demo, SYSTEM_CMD_DEMO_SIG, "The demo test command."},
                pCmdEntry = &gCmdTable[0];

                if(!strcmp((const char*)(cmde->argv[0]),p_help)) {
                //HELP state to display it to user
                //The HELP state need to override the COM_RX_NEXT_BYTE_SIG event so that any user key will
                //lead to the continue display of the command table. 
                    QACTIVE_POST(AO_Cli,(QEvent *)cmde, me); //dummy post so that memory released,AO_cli does not deal with MAX_SIG
//                    bbu_printf("\r\n"BBU_PROMPT); /*we can show core name later*/
                    return Q_TRAN(&Cli_comp_help);
                }
               else if(!strcmp((const char*)cmde->argv[0],p_malloc)){
                    bbu_malloc_test(cmde->argc, cmde->argv);
                }
#ifdef CONFIG_SMP
                else if(!strcmp((const char*)cmde->argv[0],"sgi")){
                    sgi_broadcast_all_but_self(GIC_INT_SW_12);
                }
#endif
                else{

                    while(pCmdEntry->Cmd)
                    {
                        if(!strcmp((const char*)cmde->argv[0],pCmdEntry->Cmd))
                        {
                            if(((const ComCliEvt *)e)->source)
                            {
                                ((ComCmdEvt *)cmde)->scr_cmd_index = ((ScreenCmdEvt *)e)->cmd_index;
                            } 
                            else
                            {
                               ((ComCmdEvt *)cmde)->scr_cmd_index = 0;
                            } 
                           ((QEvent *)cmde)->sig = pCmdEntry->EvtSig;
                            QACTIVE_POST(pCmdEntry->AO_Cmd, (QEvent *)cmde, me);
                            return Q_HANDLED();
                        }
                        pCmdEntry++;
                        cmde->cmd_number++;
                    }
                }
            }
            if(cmde->cmd_number) /* command like 'help' its cmd_number should be 0 */
               bbu_printf("not supported command...\r\n\n");
            QACTIVE_POST(AO_Cli,(QEvent *)cmde, me); //dummy post so that memory released,AO_cli does not deal with MAX_SIG
            bbu_printf("\r"BBU_PROMPT); /*we can show core name later*/
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Cli_comp_receiving);
}

/*..........................................................................*/
QState Cli_comp_help(CliComp *me, QEvent const *e) {
   static int index = 1;
   static int length = 0;
   static char line[8];
   switch(e->sig){
      case Q_ENTRY_SIG:{
         /* send command to terminal to get cursor position */
   #ifndef CONFIG_MMT 
         bbu_printf("%c[6n", 27);
   #else
         bbu_printf("\n\rPress Enter key to continue,press [Esc] to cancel\n\r"); 
   #endif
         return Q_HANDLED();
      }
 
      case COM_RX_NEXT_BYTE_SIG:{
	#ifndef CONFIG_MMT 
         if(index){       /* Calculate the row number of cursor */
            while(me->rIndex != me->wIndex){
               if(me->RxBuf[me->rIndex] != 27  &&
               	  me->RxBuf[me->rIndex] != 91  &&
               	  me->RxBuf[me->rIndex] != ';' &&
               	  me->RxBuf[me->rIndex] != 'R'){
               	  line[index-1] = me->RxBuf[me->rIndex];
               	  index ++;
               }
               if(me->RxBuf[me->rIndex] == ';'){
                  line[index-1] = 0;
                  length = conv_dec(line);
                  if(length < 20)  /* print (20-2) commands' helps at least */
                     length = 20;
               }
               me->rIndex++;
               me->rIndex &= (RBUF_SIZE - 1);
               if(me->RxBuf[me->rIndex-1] == 'R'){
                  me->rIndex = me->wIndex - 1;
                  index = 0;
                  break;
               }
            }
         }
	#else
		 length = 20;
	#endif
         if(me->RxBuf[me->rIndex] != ASCII_ESC){
	   #ifndef CONFIG_MMT 
            bbu_printf("%c[H%c[J", 27,27); /* clean screen and go to top */
	   #endif
            for(int i = 0; i<length-2; i++)
            {
                if(i) bbu_printf("\r\n");
                bbu_printf("%-12.12s   %s",gCmdTable[me->cmdIndex].Cmd,gCmdTable[me->cmdIndex].Help);
                me->cmdIndex++;
                if(!gCmdTable[me->cmdIndex].Cmd){
                    me->rIndex++;
                    me->rIndex &= (RBUF_SIZE - 1);
                    bbu_printf("\n\r"BBU_PROMPT);
                    index = 1;
                    return Q_TRAN(&Cli_comp_idle);
               }
            }
            me->rIndex++;
            me->rIndex &= (RBUF_SIZE - 1);
	        #ifdef CONFIG_MMT 	
	         bbu_printf("\n\rPress Enter key to continue,press [Esc] to cancel\n\r");
		  #else
                bbu_printf("\n\rPress any key to continue,press [Esc] to cancel\n\r");
		  #endif
	          return Q_HANDLED();
         }
         me->rIndex++;
         me->rIndex &= (RBUF_SIZE - 1);
         index = 1;
         bbu_printf(BBU_PROMPT);
         return Q_TRAN(&Cli_comp_idle);
      }
   }
   return Q_SUPER(&Cli_comp_processing);
}

/*..........................................................................*/
QState Cli_comp_idle(CliComp *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
	    me->cmdIndex=0;
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Cli_comp_processing);
}

