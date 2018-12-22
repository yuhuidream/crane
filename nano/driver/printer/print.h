#ifndef _PRINT_H_
#define _PPRIT_H_


#define __DEBUG_PRINT
#define DBG_LEVEL       1

#ifdef __DEBUG_PRINT
        #define DEBUGMSG(cond,str) ((void)((cond)?(bbu_printf str),1:0))
#else
        #define DEBUGMSG(cond,str) ((void)0)
#endif // DEBUG

//usage:   DEBUGMSG(DBG_LEVEL,(" PPPPPPP\r \n"));     
int print (char **out, int *varg, int align);
int bbu_printf(const char *format, ...);
int bbu_sprintf (char *out, const char *format, ...);
int bbu_putchar(char u8c);
int bbu_putstr(const char *str, uint8_t size);
void printpass(void);
void printfail(void);

#define dbg_printf bbu_printf

#endif //_PPRIT_H_
