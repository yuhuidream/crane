#ifndef h_sm_malloc_h
#define h_sm_malloc_h

void sm_malloc_init(void);
void *sm_malloc(int size);
void sm_free(void *block);

#endif
