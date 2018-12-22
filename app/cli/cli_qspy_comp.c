#ifdef Q_SPY_EXT
#include "cli_qspy_comp.h"
#ifdef CONFIG_ATE
#include "ate.h"
#endif

Q_DEFINE_THIS_FILE

extern CmdLineEntry gCmdTable[MAX_CMD_NUM];
                                          /* hierarchical state machine ... */
static QState Cli_qspy_comp_initial       (CliQspyComp *me, QEvent const *e);
static QState Cli_qspy_comp_receiving     (CliQspyComp *me, QEvent const *e);
static QState Cli_qspy_comp_processing    (CliQspyComp *me, QEvent const *e);
static QState Cli_qspy_comp_idle          (CliQspyComp *me, QEvent const *e);
static QState Cli_qspy_comp_help          (CliQspyComp *me, QEvent const *e);

/*..........................................................................*/
void CliQspyComp_ctor(CliQspyComp *me) {
   QHsm_ctor(&me->super, (QStateHandler)&Cli_qspy_comp_initial);
}

/* HSM definition ----------------------------------------------------------*/
QState Cli_qspy_comp_initial(CliQspyComp *me, QEvent const *e) {
    (void)e;               /* avoid compiler warning about unused parameter */

    me->Qspy_rIndex = 0;
    me->Qspy_wIndex = 0;
    me->Qspy_InsMode = 0;
    me->Qspy_history_i = 0; 
    me->Qspy_history_j = 0; 
    me->Qspy_history_count = 0;

#ifndef CONFIG_MMT 
    /* Connect interrupt to this AO, when AO transfers, the interrupt be
    * rerouted
    */
    QA_IRQ_CONNECT(IRQ_MMP_UART_CP,      AO_Cli);
    setup_qspy_ffuart_interrupt();
#endif
    return Q_TRAN(&Cli_qspy_comp_receiving);
}

/*..........................................................................*/
QState Cli_qspy_comp_receiving(CliQspyComp *me, QEvent const *e) {
    switch (e->sig) {
        case Q_INIT_SIG: {
            bbu_printf("\r"BBU_PROMPT);
            return Q_TRAN(&Cli_qspy_comp_processing);
        }

        case COM_RX_REQUEST_SIG: {  /*Rx interrupts event ComRxEvt*/
            ComCliEvt* reminderEvt = Q_NEW(ComCliEvt, COM_RX_NEXT_BYTE_SIG);

		    for(int i = 0; i<((ComRxEvt const *)e)->len; i++){
		        me->Qspy_RxBuf[me->Qspy_wIndex++] = ((ComRxEvt const *)e)->data[i];
		        me->Qspy_wIndex &= (RBUF_SIZE - 1);
		        Q_ASSERT(((me->Qspy_wIndex + 1)&(RBUF_SIZE - 1)) != me->Qspy_rIndex); //buffer is not full
		        //otherwise we may need to enlarge the RBUF_SIZE
		    }

            QACTIVE_POST(AO_Cli, (QEvent*)reminderEvt, me);
            return Q_HANDLED();
        }

        case COM_RX_NEXT_BYTE_SIG: {
            static ComCliEvt *Qspycce = 0;

            if(0 == Qspycce){
                Qspycce = Q_NEW(ComCliEvt,COM_RX_CMD_READY_SIG);
                Qspycce->len = 0;
                Qspycce->cursor = 0;
				memset(Qspycce->CliBuffer, 0, MAX_LINE_LEN);
            }

            while((me->Qspy_rIndex != me->Qspy_wIndex)){   
                switch (me->Qspy_RxBuf[me->Qspy_rIndex]){
					case ASCII_CTRL_Z:
                        Qspycce->len = 0;
                        Qspycce->cursor = 0;
                        bbu_printf("\r\n"BBU_PROMPT);
                        break;
                    case ASCII_LF: 
                    case ASCII_CR: {
                        if(Qspycce->len){
                            if(Qspycce->len > MAX_LINE_LEN){
                             bbu_printf("\nThe command line is too long to support...\n");
                            }else{
                                Qspycce->CliBuffer[Qspycce->len] = '\0'; //NULL to terminate
                                strcpy((char *)me->Qspy_CmdHistory[me->Qspy_history_i], 
                                       (const char *)Qspycce->CliBuffer);
                                me->Qspy_history_i = ++(me->Qspy_history_i) % NHISTORY;
                                if(me->Qspy_history_count < NHISTORY)
                                {
                                    ++me->Qspy_history_count;
                                }
                                me->Qspy_history_j = me->Qspy_history_i;
                                Qspycce->source = 0;  /* show command come from UART */

                                QACTIVE_POST(AO_Cli, (QEvent *)Qspycce, me);
                                Qspycce = Q_NEW(ComCliEvt,COM_RX_CMD_READY_SIG);
                            }
                            Qspycce->len = 0;
                            Qspycce->cursor = 0;
							memset(Qspycce->CliBuffer, 0, MAX_LINE_LEN);
                        }
                        bbu_printf("\r\n"BBU_PROMPT); /*we can show core name later*/
                       // QActive_postFIFO((QActive *)me, &reminderEvt); /*always send this in default branch?*/
                        break;
                    }
                    case BBU_KEY_INS: {
                        me->Qspy_InsMode = !(me->Qspy_InsMode);
                        break;
                    }
                    case BBU_KEY_UP:
                        //last command
                        for (int i = 0; i<(Qspycce->len-Qspycce->cursor); i++) //put cursor to the end
                        {
                            bbu_printf(" ");
                        }
                        for ( ; Qspycce->len > 0; Qspycce->len--)
                        {
                            bbu_printf("\b \b");
                        }
                        if(--me->Qspy_history_j<0)
                        {
                            me->Qspy_history_j = me->Qspy_history_count - 1; //circular 
                        }
                        strcpy((char *)Qspycce->CliBuffer, (const char*)me->Qspy_CmdHistory[me->Qspy_history_j]);
                        Qspycce->len = strlen((const char*)me->Qspy_CmdHistory[me->Qspy_history_j]);
                        Qspycce->cursor = Qspycce->len;
                        bbu_putstr((const char*)me->Qspy_CmdHistory[me->Qspy_history_j], Qspycce->len);
                        break;
                    case BBU_KEY_DOWN:
                        //next command
                        for (int i = 0; i<(Qspycce->len-Qspycce->cursor); i++) //put cursor to the end
                        {
                            bbu_printf(" ");
                        }
                        for ( ; Qspycce->len > 0; Qspycce->len--)
                        {
                            bbu_printf("\b \b");
                        }
                        me->Qspy_history_j = ++me->Qspy_history_j % me->Qspy_history_count;
                        strcpy((char *)Qspycce->CliBuffer, (const char*)me->Qspy_CmdHistory[me->Qspy_history_j]);
                        Qspycce->len = strlen((const char*)me->Qspy_CmdHistory[me->Qspy_history_j]);
                        Qspycce->cursor = Qspycce->len;

                        bbu_putstr((const char*)me->Qspy_CmdHistory[me->Qspy_history_j], Qspycce->len);
                        break;
                    case BBU_KEY_LEFT:
                        //move cursor, not index
                        if(Qspycce->cursor)
                        {
                            Qspycce->cursor--;
                            bbu_printf("\b");
                        }
                        break;
                    case BBU_KEY_RIGHT:
                        //move cursor, not index
                        if(Qspycce->CliBuffer[Qspycce->cursor])
                        {
                            bbu_putchar(Qspycce->CliBuffer[Qspycce->cursor]);
                            Qspycce->cursor++;
                        }
                        break;
                    case ASCII_BS:
                    case ASCII_DEL:
                        //delete a char backward
                        if(!Qspycce->cursor)
                            break;
                        if(me->Qspy_InsMode)
                        {
                            bbu_printf("\b");
                            bbu_putstr((const char*)(&(Qspycce->CliBuffer[Qspycce->cursor])), Qspycce->len - Qspycce->cursor);
                            bbu_printf(" ");
                            for(int i = Qspycce->cursor; i<Qspycce->len; i++)
                            {
                                Qspycce->CliBuffer[i-1] = Qspycce->CliBuffer[i];
                            }
                            Qspycce->CliBuffer[Qspycce->len-1] = '\0';
                            for (int i = 0; i<=(Qspycce->len-Qspycce->cursor); i++)
                            {
                                bbu_printf("\b");
                            }
                            Qspycce->len--;
                            Qspycce->cursor--;

                        }else{
                            bbu_printf("\b \b");
                            Qspycce->CliBuffer[Qspycce->cursor-1] = ' ';
                            if(Qspycce->cursor == Qspycce->len)Qspycce->len--;
                            Qspycce->cursor--;
                       }
                        break;
                    case ASCII_HT:{
                       char str1[10], str2[10];
                       int cmd_idx[20] = {0};
                       int val1 = 0, val2 = 0, match_num = 0;
                       for(int i = 0;i<Qspycce->cursor;i++){
                          if(Qspycce->CliBuffer[i] == ASCII_SP){
                             val2 = 1;
                             i = Qspycce->cursor;
                          }
                       }
                       if(!val2){
                          val2 = 0;
                          for(val1 = 0; val1<Qspycce->cursor;val1++){
                             str1[val1] = Qspycce->CliBuffer[val1];
                             str1[val1+1] = 0;
                          }
                          while(gCmdTable[val2].Cmd != 0){
                             strncpy(str2, gCmdTable[val2].Cmd,val1);
                             str2[val1] = 0;
                             if(strcmp(str2, str1)==0){
                                cmd_idx[match_num] = val2;
                                match_num ++;
                             }
                             val2 ++;
                          }
                          if(match_num == 1){
                             for(uint32_t i = val1; i < strlen((const char*)(gCmdTable[cmd_idx[0]].Cmd)); i++){
                                bbu_putchar(*((gCmdTable[cmd_idx[0]].Cmd)+i));
                                if(Qspycce->cursor < Qspycce->len)
                                   for(int i = Qspycce->len; i > Qspycce->cursor; i--)
                                      Qspycce->CliBuffer[i] = Qspycce->CliBuffer[i-1];
                                Qspycce->len ++;
                                Qspycce->CliBuffer[Qspycce->cursor] = *((gCmdTable[cmd_idx[0]].Cmd)+i); 
                                if(Qspycce->cursor == Qspycce->len)Qspycce->len++;
                                Qspycce->cursor++;
                             }
                             Qspycce->CliBuffer[Qspycce->cursor] = ASCII_SP;
                             Qspycce->cursor++;
                             Qspycce->len++;
                             for(int i = Qspycce->cursor; i < Qspycce->len; i++){
                                bbu_putchar(Qspycce->CliBuffer[i]);
                             }
                             for(int i = Qspycce->cursor; i < Qspycce->len; i++){
                                bbu_printf("\b");
                             }
                             bbu_putchar(' ');
                          }
                          else if(match_num >1){
                             val2 = Qspycce->len - Qspycce->cursor;
                             if(me->Qspy_RxBuf[me->Qspy_rIndex-1]==ASCII_HT){
                               me->Qspy_RxBuf[me->Qspy_rIndex]=ASCII_SP;
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
                                   if(Qspycce->cursor < Qspycce->len)
                                   for(int i = Qspycce->len; i > Qspycce->cursor; i--)
                                      Qspycce->CliBuffer[i] = Qspycce->CliBuffer[i-1];
                                   Qspycce->len++;
                                   Qspycce->CliBuffer[Qspycce->cursor] = *((gCmdTable[cmd_idx[idx]].Cmd)+val1);
                                   Qspycce->cursor++;
                                   val1 ++;
                                }
                             }
                             bbu_printf("\r"BBU_PROMPT);
                             for(int i =0; i< Qspycce->len;i++)
                                bbu_putchar(Qspycce->CliBuffer[i]);
                             for(int i =0;i<val2;i++)
                                bbu_printf("\b");
                             
                          }
                       }
                       break;
                    }
                    default: {
                        //fill the char into ComCliEvt cmd buffer
					   bbu_putchar(me->Qspy_RxBuf[me->Qspy_rIndex]);
                       if(me->Qspy_InsMode)
                        {
                            bbu_putstr((const char*)(&(Qspycce->CliBuffer[Qspycce->cursor])), 
                                        Qspycce->len - Qspycce->cursor);
                          
                            for(int i = Qspycce->len; i>Qspycce->cursor; i--)
                            {
                                Qspycce->CliBuffer[i] = Qspycce->CliBuffer[i-1];
                            }
                            Qspycce->CliBuffer[Qspycce->cursor] = me->Qspy_RxBuf[me->Qspy_rIndex];
                            for (int i = 0; i<(Qspycce->len-Qspycce->cursor); i++)
                            {
                                bbu_printf("\b");
                            }
                            Qspycce->len++;
                            Qspycce->cursor++;

                        }else{
                            Qspycce->CliBuffer[Qspycce->cursor] = me->Qspy_RxBuf[me->Qspy_rIndex];
                            if(Qspycce->cursor == Qspycce->len)Qspycce->len++;
                            Qspycce->cursor++;
                       }                        
                        break;
                    }

                }
                me->Qspy_rIndex++;
                me->Qspy_rIndex &= (RBUF_SIZE - 1);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Cli_qspy_comp_processing(CliQspyComp *me, QEvent const *e) {
    switch (e->sig) {
        case Q_INIT_SIG: {
            return Q_TRAN(&Cli_qspy_comp_idle);
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
				char *p_moduletest = "moduletest";
				char *p_qsfilter = "qsfilter";

                //{"foo", AO_Demo, SYSTEM_CMD_DEMO_SIG, "The demo test command."},
                pCmdEntry = &gCmdTable[0];

                if(!strcmp((const char*)(cmde->argv[0]),p_help)) {
                //HELP state to display it to user
                //The HELP state need to override the COM_RX_NEXT_BYTE_SIG event so that any user key will
                //lead to the continue display of the command table. 
                    QACTIVE_POST(AO_Cli,(QEvent *)cmde, me); //dummy post so that memory released,AO_cli does not deal with MAX_SIG
//                    bbu_printf("\r\n"BBU_PROMPT); /*we can show core name later*/
                    return Q_TRAN(&Cli_qspy_comp_help);
                }
               	else if(!strcmp((const char*)cmde->argv[0],p_malloc)){
               	    bbu_malloc_test(cmde->argc, cmde->argv);
               	}
				else if(!strcmp((const char*)cmde->argv[0],p_qsfilter)){
					spy_multi_filter_handler(cmde->argc, cmde->argv);
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
                            if(((const ComCliEvt *)e)->source){
                                ((ComCmdEvt *)cmde)->scr_cmd_index = ((ScreenCmdEvt *)e)->cmd_index;
                            }else{
								if(!strcmp((const char*)(cmde->argv[cmde->argc-2]),p_moduletest)){
                                    uint8_t module_id;
                                    module_id = conv_dec((char*)(cmde->argv[cmde->argc-1]));
                                    ((ComCmdEvt *)cmde)->scr_cmd_index = module_id;
 				    				cmde->argc = cmde->argc-2;
                                }else{
                                ((ComCmdEvt *)cmde)->scr_cmd_index = 0;
                              }
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

            bbu_printf(BBU_PROMPT); /*we can show core name later*/

            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Cli_qspy_comp_receiving);
}

/*..........................................................................*/
QState Cli_qspy_comp_help(CliQspyComp *me, QEvent const *e) {
   static int index = 1;
   static int length = 0;
   static char line[8];
   switch(e->sig){
      case Q_ENTRY_SIG:{
         /* send command to terminal to get cursor position */
         bbu_printf("%c[6n", 27);
         return Q_HANDLED();
      }
      case COM_RX_NEXT_BYTE_SIG:{
         if(index){       /* Calculate the row number of cursor */
            while(me->Qspy_rIndex != me->Qspy_wIndex){
               if(me->Qspy_RxBuf[me->Qspy_rIndex] != 27  &&
               	  me->Qspy_RxBuf[me->Qspy_rIndex] != 91  &&
               	  me->Qspy_RxBuf[me->Qspy_rIndex] != ';' &&
               	  me->Qspy_RxBuf[me->Qspy_rIndex] != 'R'){
               	  line[index-1] = me->Qspy_RxBuf[me->Qspy_rIndex];
               	  index ++;
               }
               if(me->Qspy_RxBuf[me->Qspy_rIndex] == ';'){
                  line[index-1] = 0;
                  length = conv_dec(line);
                  if(length < 20)  /* print (20-2) commands' helps at least */
                     	length = 20;
               }
               me->Qspy_rIndex++;
               me->Qspy_rIndex &= (RBUF_SIZE - 1);
               if(me->Qspy_RxBuf[me->Qspy_rIndex-1] == 'R'){
                  me->Qspy_rIndex = me->Qspy_wIndex - 1;
                  index = 0;
                  break;
               }
            }
         }
         if(me->Qspy_RxBuf[me->Qspy_rIndex] != ASCII_ESC){
            bbu_printf("%c[H%c[J", 27,27); /* clean screen and go to top */
            for(int i = 0; i<length-2; i++){
                if(i) bbu_printf("\r\n");
                bbu_printf("%-12.12s   %s",gCmdTable[me->Qspy_cmdIndex].Cmd,gCmdTable[me->Qspy_cmdIndex].Help);
                me->Qspy_cmdIndex++;
                if(!gCmdTable[me->Qspy_cmdIndex].Cmd){
                    me->Qspy_rIndex++;
                    me->Qspy_rIndex &= (RBUF_SIZE - 1);
                    bbu_printf("\n"BBU_PROMPT);
                    index = 1;
                    return Q_TRAN(&Cli_qspy_comp_idle);
               }
            }
            me->Qspy_rIndex++;
            me->Qspy_rIndex &= (RBUF_SIZE - 1);
            bbu_printf("\n\rPress any key to continue,press [Esc] to cancel\n\r");
	          return Q_HANDLED();
         }
         me->Qspy_rIndex++;
         me->Qspy_rIndex &= (RBUF_SIZE - 1);
         index = 1;
         bbu_printf(BBU_PROMPT);
         return Q_TRAN(&Cli_qspy_comp_idle);
      }
   }
   return Q_SUPER(&Cli_qspy_comp_processing);
}

/*..........................................................................*/
QState Cli_qspy_comp_idle(CliQspyComp *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
	    	me->Qspy_cmdIndex=0;
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Cli_qspy_comp_processing);
}
#endif

