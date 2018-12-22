#ifndef H_MEM_TESTS_H
#define H_MEM_TESTS_H

/* peek/poke/dump related functions */
void mem_dump (uint32_t *adrs,int nwords,int isbytePrint);
void bbu_peek_8(uint32_t addr, int num_of_reads);
void bbu_poke_8(uint32_t addr, uint8_t data);
void bbu_peek_16(uint32_t addr, int num_of_reads);
void bbu_poke_16(uint32_t addr, uint16_t data);
void bbu_peek_32(uint32_t addr, int num_of_reads);
void bbu_poke_32(uint32_t addr, uint32_t data);
int bbu_mem_copy(uint32_t *source_address,
                 uint32_t *dest_address,
                 int       byte_count);
void bbu_mem_fill(uint32_t *start_address,
                  uint32_t data,
                  int      byte_count);
int bbu_mem_std_test(uint32_t *start_address, int byte_count);

void bbu_peek(int argc, uint8_t **argv);
void bbu_poke(int argc, uint8_t **argv);
void bbu_dump(int argc, uint8_t **argv);
void bbu_fmem(int argc, uint8_t **argv);
void bbu_memcopy(int argc, uint8_t **argv);
int bbu_memt(int argc, uint8_t **argv);
void bbu_vtpa(int argc, uint8_t **argv);
void ddr_test(int argc, uint8_t **argv);
void dump_mem_map_info(void);

#endif
