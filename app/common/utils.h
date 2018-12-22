
#ifndef _UTILS_H_
#define _UTILS_H_

#define MMU_OFF 0
#define INVALID_VIRTUAL_ADDRESS 0xFF
#define xlli_control_mmu 0x0001

#define _readb(p)   (*(volatile uint8_t *)(p))
#define _readw(p)   (*(volatile uint16_t *)(p))
#define _readl(p)   (*(volatile uint32_t *)(p))

#define _writeb(v,p)  (*(volatile uint8_t *)(p) = ((uint8_t)v))
#define _writew(v,p)  (*(volatile uint16_t *)(p) = ((uint16_t)v))
#define _writel(v,p)  (*(volatile uint32_t *)(p) = (v))

/* Cortex R5 doesn't have MMU, so PA is 'VA' */
#define get_physical(va)   va
int measure_core_frequency(int print_log);
void page_table_walk(uint32_t vaddr);

uint32_t check_reg(uint8_t *reg_argu);

/*for command parameter parsing usage*/
uint32_t conv_hex( char *s );
uint32_t conv_dec( char *s );
uint32_t XpStrToValue(char * pStr);


void POST_CMD2CLI (const char *cmdstr);

#endif
