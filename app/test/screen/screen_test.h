
#ifndef SCREEN_TEST_H
#define SCREEN_TEST_H

#include "bbu_test.h"



uint8_t collect_screen_result(const ScreenCmdAck *e);
void screen_timeout_handle(void);
void screen_test_cmd_ack(uint8_t result, uint8_t cmd_index);
uint8_t screen_find_cmd_index(char * cmd_line);
uint32_t slt_load_cmd_from_local(void);
uint32_t slt_add_cmd_to_group(uint32_t seq, uint32_t index, char *cmd, uint32_t timeout);
void screen_init(void);
void slt_update_cmd_number(void);
void slt_free_group_list(void);

extern void lcd_disp_pass(void);
extern void lcd_disp_fail(void);

extern uint32_t lcd_buffer;

#endif
