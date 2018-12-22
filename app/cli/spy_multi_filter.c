#include "cli_qspy_comp.h"

static void spy_multi_filter_help(void){
	bbu_printf("\n\rqsfilter - BBU qspy record filter command\n\r");
    bbu_printf("Usage: qsfilter [<record type>] [<record id>] [<on/off>]\n\r");
    bbu_printf(" <record type>: \n\r"
               "     -ao -- select to trace ao record or not \n\r");
    bbu_printf("     -glob -- select to trace global record or not \n\r");
	bbu_printf("Use qsfilter all on/off to turn on/off all records \n\r");
}

void spy_multi_filter_handler(uint8_t argc, uint8_t** argv){
	switch(argc){
		case 1:
		case 2:{
			spy_multi_filter_help();
			break;
		}
		case 3:{
			if(!strcmp((const char*)argv[1],"all")){
				if(!strcmp((const char*)argv[2],"on")){
				QS_FILTER_ON(QS_ALL_RECORDS);
				}
				else if(!strcmp((const char*)argv[2],"off")){
				   	QS_FILTER_OFF(QS_ALL_RECORDS);
					QS_FILTER_ON(QS_USER);//We should keep the user record open for console log.
				}
				else{
					spy_multi_filter_help();
				}
				break;
			}
		}
		case 4:{
			if(!strcmp((const char*)argv[1],"-ao")){
				uint32_t prio = conv_dec((char*)argv[2]);
				if(prio>=64){
					bbu_printf("The AO number overflow\r\n");
					break;
				}
				if(!strcmp((const char*)argv[3],"on")){
					QS_AO_RECORD_ON(prio);
				}
				else if(!strcmp((const char*)argv[3],"off")){
					QS_AO_RECORD_OFF(prio);
				}else{
					spy_multi_filter_help();
				}
			}
			else if(!strcmp((const char*)argv[1],"-glob")){
			    uint8_t qs_record_id;
			   	qs_record_id = conv_dec((char*)argv[2]);
				if(qs_record_id>=125){
					bbu_printf("The record number overflow\r\n");
					break;
				}
			   	if(!strcmp((const char*)argv[3],"on")){
					QS_FILTER_ON(qs_record_id);
				}else{
					QS_FILTER_OFF(qs_record_id);
			   	}
			}else{
				spy_multi_filter_help();
			}
			break;
		}
		default:
			spy_multi_filter_help();
			break;
	}

	return;
}
