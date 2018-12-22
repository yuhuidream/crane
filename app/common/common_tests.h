#ifndef H_COMMON_TESTS_H
#define H_COMMON_TESTS_H

void bbu_ledt(int argc, uint8_t **argv);
void bbu_gpio(int argc, uint8_t **argv);
void bbu_eica_test(int argc, uint8_t **argv);
void bbu_dica_test(int argc, uint8_t **argv);
void bbu_ivfp(int argc, uint8_t **argv);
void bbu_clear_screen(void);
int bbu_print_switch(int argc, uint8_t **argv);
int bbu_stack(int argc, uint8_t **argv);
int bbu_setv_test(int argc, uint8_t **argv);
int bbu_pmic_revision(void);
void bbu_ddrw(int argc, uint8_t **argv);
void bbu_ddrp(int argc, uint8_t **argv);
void bbu_durm(int argc, uint8_t **argv);
void bbu_curm();
uint32_t get_chip_id(void);
int bbu_pid(uint32_t *type, uint32_t *step);
void bbu_bid(void);
void bbu_ddr_power_test(int argc, uint8_t **argv);
void drys_test(uint32_t loops);
void core_mark_main(int argc, char **argv);
extern void TMark_main(int argc, char **argv);
extern void linpack_main(void);
void mfp_tests();
void pm_test(void);
void bbu_xtc_test(int argc, uint8_t **argv);
int bbu_uart_switch(int argc, uint8_t **argv);
int bbu_ipc_test(int argc, uint8_t **argv);

/* board version flag */
enum BOARD_VERSION {
	NZA3_DKB_V10 = 0,
	NZA3_DKB_V20A,
	NZA3_DKB_V20B,
	NZA3_DKB_V21A,
};

#define FXL6408_ADDR			(0x86)
#define FXL6408_INPUT_STATUS		(0x0F)
#define FXL6408_DEFAULT_STATE		(0x09)

#endif
