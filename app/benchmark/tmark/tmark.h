#ifndef _TMARK_H_
#define _TMARK_H_

typedef struct node* link;

struct node {
  link *next;
  int     a1;
  int     a2;
  int     a3;
  int     a4;
  int     a5;
  int     a6;
  int     a7;
};

typedef struct node_64* link_64;

struct node_64 {
  link_64 *next;
  int     a1;
  int     a2;
  int     a3;
  int     a4;
  int     a5;
  int     a6;
  int     a7;
  int     a8;
  int     a9;
  int     a10;
  int     a11;
  int     a12;
  int     a13;
  int     a14;
  int     a15;
};


/* low level test function prototypes */
/* Load */
void single_ldr_4(char *src, int iter);
void single_ldr_32_linked(char *src, int iter);
void single_ldr_64_linked(char *src, int iter);
void single_ldr_32(char *src, int iter);
void single_ldr_64(char *src, int iter);
void pipe_ldr_4(char *src, int iter);
void pipe_ldrd_8(char *src, int iter);
void pipe_ldr_32(char *src, int iter);
void pipe_ldr_32_loop12(char *src, int iter);
void pipe_ldr_64(char *src, int iter);
void calibrate_overhead();
void coalesce_load_4_ldm_2( char *src, int iter);
void coalesce_load_4_ldm_4( char *src, int iter);
void coalesce_load_4_ldm_8( char *src, int iter);
extern void single_ldr_32_DSB(char *src, int iter);
extern void pipe_pld_32(char *src, int iter);
extern void pipe_ldr_32_WOBU(char *src, int iter);
extern void ReadTest(char *src, int iter);

/* Store */
void single_store_4(char *dst, int iter);
void pipe_str_4(char *dst, int iter);
void pipe_strd_8(char *dst, int iter);
void pipe_strd_8_postincr( char *dst, int iter);
void coalesce_store_4_stm_2( char *dst, int iter);
void coalesce_store_4_stm_4( char *dst, int iter);
void coalesce_store_4_stm_8( char *dst, int iter);
extern void pipe_strh_4(char *dst, int iter);
extern void pipe_strb_4_1(char *dst, int iter);
extern void pipe_strb_4_2(char *dst, int iter);
extern void pipe_str_1_8_7(char *dst, int iter);
extern void pipe_str_1_8_7(char *dst, int iter);
extern void pipe_str_9_7(char *dst, int iter);
extern void coalesce_store_4_stm_7_skip_1(char *dst, int iter);
extern void single_store_4_min_1(char *dst, int iter);
extern void combine_store_4(char *dst, int iter);
extern void hop_store_4_1(char *dst, int iter);
extern void hop_store_4_2(char *dst, int iter);
extern void hop_store_4_3(char *dst, int iter);
extern void hop_store_4_4(char *dst, int iter);
/* Memcpy */
extern char *memcpy_xsc_v5(char *dst, const char *src, int size);
extern char *memcpy_glibc_opt(char *dst, const char *src, int size);
extern char *memcpy_feroceon(char *dst, const char *src, int size);
extern char *memcpy_fast(char *dst, const char *src, int size);
extern char *mrvl_memcpy_v2(char *dst, const char *src, int size);
extern char *mrvl_memcpy_v3(char *dst, const char *src, int size);
extern char *mrvl_memcpy_v3_2(char *dst, const char *src, int size);
extern char *mrvl_memcpy4(char *dst, const char *src, int size);

extern void flush_all_cache(void);
unsigned int mrvl_times(void);



#endif /* _TMARK_H_ */

/* EOF */

