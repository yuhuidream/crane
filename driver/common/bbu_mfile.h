#ifndef MEM_FILE_H_
#define MEM_FILE_H_

#include "predefines.h"
#include "spinlock.h"

typedef struct
{
   char      name[128];
   uint32_t  rw_idx;
   uint32_t  file_size;
   uint32_t  buffer_size;
   uint32_t  overflow;
   uint32_t  sum;
   uint32_t  mode;
#ifdef CONFIG_SMP
   spinlock_t  lock;
#endif
   uint8_t   *file_begin;
}MFILE;

#ifdef CONFIG_SMP
#define MFILE_LOCK(mfp)    spin_lock(&(mfp->lock))
#define MFILE_UNLOCK(mfp)  spin_unlock(&(mfp->lock))
#else
#define MFILE_LOCK(mfp)
#define MFILE_UNLOCK(mfp)
#endif

typedef enum{
   MFILE_CLOSED = 0,
   MFILE_R_ONLY,        /* r  */
   MFILE_R_PLUS,        /* r+ */
   MFILE_W_ONLY,        /* w  */
   MFILE_W_PLUS,        /* w+ */
   MFILE_A_ONLY,        /* a  */
   MFILE_A_PLUS,        /* a+ */
   MFILE_WB_ONLY,       /* wb */
   MFILE_WB_PLUS,       /* wb+ */
   MFILE_RB_ONLY,       /* rb */
   MFILE_RB_PLUS,       /* rb+ */
   MFILE_INVALID,
}MFILE_MODE;

#define MEOF       (-1)
#define MERROR     (-2)
#define MOVERFLOW  (0)


#define MFILE_SEEK_SET    0
#define MFILE_SEEK_CUR    1
#define MFILE_SEEK_END    2
void mfile_init(const char * filename, char *array, int size);
MFILE * mfopen(const char * filename, const char * mode);
int mfclose(MFILE *mfp);
int mfgetc(MFILE *mfp);
int mfputc(int c, MFILE *mfp);
char *mfgets(char *s, int n, MFILE *mfp);
int mfputs(const char * s, MFILE *mfp);
int mfprintf( MFILE *mfp, const char *format, ... );
int mfread(void * buffer, int size, int count, MFILE* mfp);
int mfwrite(void *buffer, int size, int count, MFILE* mfp);
int mftell( MFILE *mfp );
int mfseek(MFILE *mfp, long offset, uint32_t fromwhere);
int mfeof(MFILE *mfp);
int mferror(MFILE *mfp);
int mfflush(MFILE* mfp);
int mfrename (const char *oldname, const char *newname);
int mfremove (const char *filename);
int mfile_dump_content(const char *filename);
int mfile_dump_info(const char *filename);
int mfgetline(char *s, int *n, MFILE *mfp);

int is_mfile_readable(MFILE *mfp);
int is_mfile_writable(MFILE *mfp);
#endif  
