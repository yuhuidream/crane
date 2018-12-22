#include "predefines.h"
#include "bbu_malloc.h"
#include "bbu_mfile.h"
#include <string.h>
#include "bbu.h"

#define MFILE_TEST 1
//#define MFILE_DEBUG 1
extern int print (char **out, int *varg, int align);
extern int bbu_putchar(char u8c);
#define MAX_MEM_FILE         16
#define DEFAULT_MFILE_SIZE  (120*1024)

static MFILE *mem_files[MAX_MEM_FILE] = 
      {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static void mf_write_alloc(MFILE *mfp, const char * filename);
static int mode_translate(const char *mode);
static int search_mfile(const char* filename);

/*..........................................................................*/
void mfile_init(const char * filename, char *array, int size)
{
   int i;
   MFILE *mfp = (MFILE *)0;

   uint32_t idx = search_mfile(filename);
   if(idx < MAX_MEM_FILE)
   {
      bbu_printf("Can't init an existed memory file\n\r");
   }

   for( i=0; i<MAX_MEM_FILE; i++ )
   {
      if ( mem_files[i] == NULL )
      {
         break;
      }
   }
   if(i < MAX_MEM_FILE){
      mfp = (MFILE *)bbu_malloc(sizeof(MFILE));
      strcpy(mfp->name, filename);
      mfp->buffer_size = (uint32_t)size;
      if(array){
         mfp->file_begin  = (uint8_t *)array;
         mfp->file_size = (uint32_t)size;
      }
      else{   //init a empty file with a specific size
         mfp->file_begin  = (uint8_t *)bbu_malloc(size);
         mfp->file_size = 0;
      }
      mfp->overflow  = (uint32_t)0;
      mfp->rw_idx  = 0;
      mfp->sum = 0;
      mfp->file_begin[mfp->file_size] = (char)MEOF;
      mfp->mode = MFILE_CLOSED;
      mem_files[i] = mfp;
#ifdef MFILE_DEBUG
      bbu_printf("A new file created: %s: 0x%08X\n\r",
                 mfp->name, mfp->file_begin);
#endif
      return;
   }
#ifdef MFILE_DEBUG
   bbu_printf("ERROR: the memory file is exhaused\n\r");
#endif
   return;
}

/*..........................................................................*/
MFILE * mfopen(const char * filename, const char * mode)
{
   int idx;
   MFILE_MODE fmode;
   MFILE *mfp = (MFILE *)0;
   fmode = mode_translate(mode);
   idx = search_mfile(filename);
   
   /* The file found and opened */
   if(idx != MAX_MEM_FILE){
      mfp = mem_files[idx];
      mfp->mode = fmode;
      mfp->rw_idx  = 0;
      mfp->overflow = 0;
      mfp->sum = 0;
#ifdef MFILE_DEBUG
      bbu_printf("Opening file: %s succeeded\n\r", filename);
#endif
      switch(fmode){
         case MFILE_R_ONLY:
         case MFILE_R_PLUS:
         case MFILE_RB_ONLY:
         case MFILE_RB_PLUS:
            return mfp;
         case MFILE_W_ONLY:
         case MFILE_W_PLUS:
         case MFILE_WB_ONLY:
         case MFILE_WB_PLUS:
            mfp->file_size = 0;
            return mfp;
         case MFILE_A_ONLY:
         case MFILE_A_PLUS:
            mfp->rw_idx  = mfp->file_size;
            return mfp;
         default:
#ifdef MFILE_DEBUG
            bbu_printf("ERROR: open a mfile in an unknown mode: %s\n\r",mode);
#endif
            return (MFILE *)(0);
      }
   }
   else{  /* The file does not exsit */
      switch(fmode){
         case MFILE_R_ONLY:
         case MFILE_R_PLUS:
         case MFILE_RB_ONLY:
         case MFILE_RB_PLUS:
#ifdef MFILE_DEBUG
           bbu_printf("Opening file: %s failed\n\r", filename);
#endif
            return ((MFILE *)0);
         case MFILE_W_ONLY:
         case MFILE_W_PLUS:
         case MFILE_A_ONLY:
         case MFILE_A_PLUS:
         case MFILE_WB_ONLY:
         case MFILE_WB_PLUS:
            mfp = (MFILE *)bbu_malloc(sizeof(MFILE));
            mf_write_alloc(mfp, filename);
            mfp->mode = fmode; /* must after the file created */
#ifdef MFILE_DEBUG
            bbu_printf("Opening & creating file: %s succeeded\n\r", filename);
#endif
            return mfp;
         default:
#ifdef MFILE_DEBUG
            bbu_printf("ERROR: open a mfile in an unknown mode: %s\n\r",mode);
#endif
            return (MFILE *)(0);
      }
   }
   
   return (MFILE *)(0);
}

/*..........................................................................*/
int mfclose(MFILE *mfp)
{
#ifdef MFILE_DEBUG
   bbu_printf("Closing file: %s succeeded\n\r", mfp->name);
#endif
   mfp->mode = MFILE_CLOSED;
   return 0;
}

/*..........................................................................*/
int mfgetc(MFILE *mfp)
{
   if(!is_mfile_readable(mfp)){
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: %s file: %s in invalid mode: %d\n\r",
                 __FUNCTION__, mfp->name, mfp->mode);
#endif
      return MERROR;
   }
#ifdef MFILE_DEBUG
   bbu_printf("%s from file: %s\n\r", __FUNCTION__, mfp->name);
#endif
   if(mfp->rw_idx >= mfp->file_size)
      return MEOF;
   uint8_t result = mfp->file_begin[mfp->rw_idx];
   mfp->rw_idx++;
   return ((int)result);
}

/*..........................................................................*/
int mfputc(int c, MFILE *mfp)
{
   if(!is_mfile_writable(mfp)){
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: %s file: %s in invalid mode: %d\n\r",
                 __FUNCTION__, mfp->name, mfp->mode);
#endif
      return MERROR;
   }
#ifdef MFILE_DEBUG
   bbu_printf("%s to file: %s\n\r", __FUNCTION__, mfp->name);
#endif
   if(mfp->rw_idx+1 >= mfp->buffer_size)
   {
      uint8_t *buffer = bbu_malloc(mfp->buffer_size + DEFAULT_MFILE_SIZE);
      if(buffer){
         memcpy(buffer, mfp->file_begin, mfp->file_size+1);
         bbu_mfree((void *)mfp->file_begin);
         mfp->file_begin = buffer;
         mfp->buffer_size += DEFAULT_MFILE_SIZE;
      }else{
         mfp->overflow += 1;
         return (MOVERFLOW);
      }
   }
   
   mfp->file_begin[mfp->rw_idx] = (char)c;
   mfp->rw_idx ++;
   
   if ( mfp->rw_idx > mfp->file_size ){
      mfp->file_size = mfp->rw_idx;
      mfp->file_begin[mfp->file_size] = (char)MEOF;
   }
      
   return c;
}

/*..........................................................................*/
char *mfgets(char *s, int n, MFILE *mfp)
{
   if(!is_mfile_readable(mfp)){
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: %s file: %s in invalid mode: %d\n\r",
                 __FUNCTION__, mfp->name, mfp->mode);
#endif
      return (char *)NULL;
   }
#ifdef MFILE_DEBUG
   bbu_printf("%s from file: %s\n\r", __FUNCTION__, mfp->name);
#endif
   int i=0;
   if(mfp->file_begin[mfp->rw_idx] == (uint8_t)MEOF
   	  || mfp->rw_idx >= mfp->file_size)
      return (char *)NULL;
   do
   {
      s[i++] = mfp->file_begin[mfp->rw_idx++];
   }
   while( s[i-1] != '\n' && s[i-1] != '\r' && s[i-1] != '\0' && i<(n-1) &&
          s[i-1] != (uint8_t)MEOF);
   
   if ( s[i-1] ) s[i] = '\0';
   s[n-1] = '\0';

   return( s );
}

/*..........................................................................*/
int mfgetline(char *s, int *n, MFILE *mfp)
{
   QF_CRIT_STAT_TYPE state;
   QF_CRIT_ENTRY(state);
   MFILE_LOCK(mfp);
   if(!is_mfile_readable(mfp)){
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: %s file: %s in invalid mode: %d\n\r",
                 __FUNCTION__, mfp->name, mfp->mode);
#endif
      MFILE_UNLOCK(mfp);
      QF_CRIT_EXIT(state);
      return -1;
   }
#ifdef MFILE_DEBUG
   bbu_printf("%s from file: %s\n\r", __FUNCTION__, mfp->name);
#endif
   int i=0;
   if(mfp->file_begin[mfp->rw_idx] == (uint8_t)MEOF
   	  || mfp->rw_idx >= mfp->file_size){
      MFILE_UNLOCK(mfp);
      QF_CRIT_EXIT(state);
      return -1;
   }
   do
   {
      s[i++] = mfp->file_begin[mfp->rw_idx++];
   }
   while( s[i-1] != '\n' && s[i-1] != '\r' && s[i-1] != '\0' && i<(*n-1) &&
          s[i-1] != (uint8_t)MEOF);
   
   if ( s[i-1] ) s[i] = '\0';
   s[*n-1] = '\0';

   MFILE_UNLOCK(mfp);
   QF_CRIT_EXIT(state);
   return i;
}

/*..........................................................................*/
int mfputs(const char * s, MFILE *mfp)
{
   if(!is_mfile_writable(mfp)){
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: %s file: %s in invalid mode: %d\n\r",
                 __FUNCTION__, mfp->name, mfp->mode);
#endif
      return MERROR;
   }
#ifdef MFILE_DEBUG
   bbu_printf("%s to file: %s\n\r", __FUNCTION__, mfp->name);
#endif
   int i=0;
   int len = strlen(s);
   
   if(mfp->rw_idx+len >= mfp->buffer_size)
   {
      uint8_t *buffer = bbu_malloc(mfp->buffer_size+len+DEFAULT_MFILE_SIZE);
      if(buffer){
         memcpy(buffer, mfp->file_begin, mfp->file_size+1);
         bbu_mfree((void *)mfp->file_begin);
         mfp->file_begin = buffer;
         mfp->buffer_size += (len + DEFAULT_MFILE_SIZE);
      }else{
         mfp->overflow += len;
         return (MOVERFLOW);
         //return (MEOF);
      }
   }
   
   while(i < len)
   {
      mfp->file_begin[mfp->rw_idx++] = s[i++];
   }
   
   if ( mfp->rw_idx > mfp->file_size ){
      mfp->file_size = mfp->rw_idx;
      mfp->file_begin[mfp->file_size] = (char)MEOF;
   }
   
   return ( len );
}

/*..........................................................................*/
int mfprintf( MFILE *mfp, const char *format, ... )
{
   if(!is_mfile_writable(mfp)){
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: %s file: %s in invalid mode: %d\n\r",
                 __FUNCTION__, mfp->name, mfp->mode);
#endif
      return MERROR;
   }
#ifdef MFILE_DEBUG
   bbu_printf("%s to file: %s\n\r", __FUNCTION__, mfp->name);
#endif

   int *varg = (int *) (char *) (&format);
   char *out = bbu_malloc(DEFAULT_MFILE_SIZE);

   if(out == NULL)
      return MERROR;
   char *out_bk = out;

   int size = print (&out, varg, 1);        /* out will be changed after it */
   if(size > DEFAULT_MFILE_SIZE)
      return MERROR;

   if(mfp->rw_idx + size >= mfp->buffer_size){
      uint8_t *buffer = bbu_malloc(mfp->buffer_size+size+DEFAULT_MFILE_SIZE);
      if(buffer){
         memcpy(buffer, mfp->file_begin, mfp->file_size+1);
         bbu_mfree((void *)mfp->file_begin);
         mfp->file_begin = buffer;
         mfp->buffer_size += (size + DEFAULT_MFILE_SIZE);
      }else{
         mfp->overflow += size;
         return (MOVERFLOW);
      }
   }

   memcpy( mfp->file_begin+mfp->rw_idx, out_bk, size );
   bbu_mfree((void *)out_bk);
   mfp->rw_idx += size;

   if ( mfp->rw_idx > mfp->file_size ){
      mfp->file_size = mfp->rw_idx;
      mfp->file_begin[mfp->file_size] = (char)MEOF;
   }

   return size;
}

/*..........................................................................*/
int mfread(void * buffer, int size, int count, MFILE* mfp)
{
   if(!is_mfile_readable(mfp)){
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: %s file: %s in invalid mode: %d\n\r",
                 __FUNCTION__, mfp->name, mfp->mode);
#endif
      return MERROR;
   }
#ifdef MFILE_DEBUG
   bbu_printf("%s from file: %s\n\r", __FUNCTION__, mfp->name);
#endif
   int readsize = mfp->file_size - mfp->rw_idx;
   if ( (count * size) < readsize )
      readsize = count * size;

   memcpy( buffer, mfp->file_begin+mfp->rw_idx, readsize );
   mfp->rw_idx += readsize;
   
   return( readsize );
}

/*..........................................................................*/
int mfwrite(void *buffer, int size, int count, MFILE* mfp)
{
   if(!is_mfile_writable(mfp)){
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: %s file: %s in invalid mode: %d\n\r",
                 __FUNCTION__, mfp->name, mfp->mode);
#endif
      return MERROR;
   }
#ifdef MFILE_DEBUG
   bbu_printf("%s to file: %s\n\r", __FUNCTION__, mfp->name);
#endif
   int writesize = size*count;
                                                     /* align by word */
   uint32_t *ibuffer = (uint32_t *)((uint32_t)buffer & 0xFFFFFFFC); 
   int i;
   for( i=0; i<(writesize/4); i++)
      mfp->sum = (mfp->sum ^ (ibuffer[i]));
   
   if ( writesize + mfp->rw_idx > mfp->buffer_size )
   {
      uint8_t *tmp = 
         bbu_malloc(mfp->buffer_size+writesize+DEFAULT_MFILE_SIZE);
      if(tmp){
         memcpy(tmp, mfp->file_begin, mfp->file_size+1);
         bbu_mfree((void *)mfp->file_begin);
         mfp->file_begin = tmp;
         mfp->buffer_size += (writesize + DEFAULT_MFILE_SIZE);
      }else{
         mfp->overflow += writesize;
         return (MOVERFLOW);
      }
   }
   
   memcpy( mfp->file_begin+mfp->rw_idx, buffer, writesize );
   mfp->rw_idx += writesize;
   if ( mfp->rw_idx > mfp->file_size ){
      mfp->file_size = mfp->rw_idx;
      mfp->file_begin[mfp->file_size] = (char)MEOF;
   }
   
   return ( writesize );
}

/*..........................................................................*/
int mftell( MFILE *mfp )
{
   if(mfp->mode <= MFILE_CLOSED || mfp->mode >= MFILE_INVALID)
      return MERROR;
   return( mfp->rw_idx );
}

/*..........................................................................*/
int mfseek(MFILE *mfp, long offset, uint32_t fromwhere)
{
   int position = offset;
   if ( fromwhere == MFILE_SEEK_SET ) position = offset;
   else if ( fromwhere == MFILE_SEEK_CUR ) position = offset+mftell(mfp);
   else if ( fromwhere == MFILE_SEEK_END ) position = offset+mfp->file_size;
   else return MERROR;

   mfp->rw_idx = position;
   if ( mfp->rw_idx > mfp->file_size )
      mfp->rw_idx = mfp->file_size;
   if ( mfp->rw_idx > mfp->file_size )
      mfp->rw_idx = mfp->file_size;

   return( mfp->rw_idx );
}

/*..........................................................................*/
int mfeof(MFILE *mfp)
{
   return( mfp->rw_idx == mfp->file_size );
}

/*..........................................................................*/
int mferror(MFILE *mfp)
{
   (void)mfp;
   return 0;
}


/*..........................................................................*/
int mfflush(MFILE* mfp)
{
   (void)mfp;
   return 0;
}

/*..........................................................................*/
int mfremove (const char *filename)
{
   int idx;
   for( idx=0; idx < MAX_MEM_FILE; idx++ )
   {
      if ( mem_files[idx] == NULL ) continue;
      if ( !strcmp( mem_files[idx]->name, filename ) ) break;
   }
   if(idx < MAX_MEM_FILE){  /* the file exists */
      bbu_mfree(mem_files[idx]->file_begin);
      bbu_mfree(mem_files[idx]);
      mem_files[idx] = NULL;
      return 0;
   }
   else{
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: try to delete an inexistent file\n\r");
#endif
      return -1;
   }
}

/*..........................................................................*/
int mfrename (const char *oldname, const char *newname)
{
   int idx;
   for( idx=0; idx < MAX_MEM_FILE; idx++ )
   {
      if ( mem_files[idx] == NULL ) continue;
      if ( !strcmp( mem_files[idx]->name, oldname ) ) break;
   }
   if(idx < MAX_MEM_FILE){  /* the file exists */
      strcpy(mem_files[idx]->name, newname);
      return 0;
   }
   else{
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: try to rename an inexistent file\n\r");
#endif
      return -1;
   }
}


/*..........................................................................*/
void mf_write_alloc(MFILE *mfp, const char * filename)
{
   int i;
   strcpy(mfp->name, filename);
   mfp->buffer_size = DEFAULT_MFILE_SIZE;
   mfp->file_begin  = bbu_malloc(DEFAULT_MFILE_SIZE);  
   mfp->file_size = mfp->overflow  = 0;
   mfp->rw_idx  = 0;
   mfp->sum = 0;
   mfp->file_begin[mfp->file_size] = (char)MEOF;
   mfp->mode = MFILE_CLOSED;
   for( i=0; i<MAX_MEM_FILE; i++ )
   {
      if ( mem_files[i] == NULL )
      {
         mem_files[i] = mfp;
         break;
      }
   }
   
   if(i == MAX_MEM_FILE)
#ifdef MFILE_DEBUG
      bbu_printf("ERROR: the memory file is exhaused\n\r");
#endif
   bbu_printf("A new file created: %s: 0x%08X\n\r",mfp->name, mfp->file_begin);
}

/*..........................................................................*/
int is_mfile_readable(MFILE *mfp)
{
   if(mfp->mode == MFILE_W_ONLY  || mfp->mode == MFILE_A_ONLY || 
   	  mfp->mode == MFILE_WB_ONLY || mfp->mode == MFILE_CLOSED || 
   	  mfp->mode >= MFILE_INVALID){
      return 0;
   }
   return 1;
}

/*..........................................................................*/
int is_mfile_writable(MFILE *mfp)
{
   if(mfp->mode == MFILE_R_ONLY  || mfp->mode == MFILE_CLOSED ||
   	  mfp->mode == MFILE_RB_ONLY || mfp->mode >= MFILE_INVALID){
      return 0;
   }
   return 1;
}

/*..........................................................................*/
static int search_mfile(const char* filename)
{
   int idx;
   for( idx=0; idx < MAX_MEM_FILE; idx++ )
   {
      if ( mem_files[idx] == NULL ) continue;
      if ( !strcmp( mem_files[idx]->name, filename ) ) break;
   }
   if(idx < MAX_MEM_FILE)
      return idx;
   else
      return MAX_MEM_FILE;
}

/*..........................................................................*/
int mfile_dump_info(const char *filename)
{
   MFILE *mfp = (MFILE *)0;
   uint32_t idx = search_mfile(filename);
   if(idx < MAX_MEM_FILE)
      mfp = mem_files[idx];
   else{
      bbu_printf("ERROR: cann't dump info of an inexistent file\n\r");
      return 0;
   }
   bbu_printf("*** Dump infomation of %s ***\n\r", filename);
   bbu_printf("Memory file name: %s\n\r", filename);
   bbu_printf("Meomry file begin: 0x%08X\n\r", (uint32_t)mfp->file_begin);
   bbu_printf("Meomry file size:  0x%08X\n\r",  mfp->file_size);
   bbu_printf("Meomry file buffer size:  0x%08X\n\r",  mfp->buffer_size);
   bbu_printf("Meomry file overflow: %d\n\r",  mfp->overflow);
   bbu_printf("Memory file R/W index: %d\n\r",  mfp->rw_idx);
   bbu_printf("Memory file check sum: 0x%08X\n\r",  mfp->sum);
   return 1;
}


/*..........................................................................*/
int mfile_dump_content(const char *filename)
{
   MFILE *mfp = (MFILE *)0;
   uint32_t idx = search_mfile(filename);
   if(idx < MAX_MEM_FILE)
      mfp = mem_files[idx];
   else{
      bbu_printf("ERROR: cann't dump content of an inexistent file\n\r");
      return 0;
   }
   bbu_printf("*** Dump content of %s ***\n\r", filename);
   if(mfp->file_size == 0)
      bbu_printf("This file is empty\n\r");
   else
      for(idx = 0; idx < mfp->file_size; idx++)
         bbu_putchar(mfp->file_begin[idx]);
   return 1;
}

/*..........................................................................*/
int mfile_print_all_name(void)
{
   int idx = 0;
   int exist_file = 0;
   for(; idx < MAX_MEM_FILE; idx++){
      if(mem_files[idx] != NULL){
         exist_file++;
         if(exist_file == 1)
            bbu_printf("Current existent memory files as below:\n\r");
         bbu_printf("Memory file%d: %s\n\r", idx+1, mem_files[idx]->name);
      }
   }
   if(!exist_file)
      bbu_printf("There is no memory file\n\r");
   return exist_file;
}

/*..........................................................................*/
void mfile_remove_all(void)
{
   int idx = 0;
   for(; idx < MAX_MEM_FILE; idx++){
      if(mem_files[idx] != NULL){
         bbu_mfree(mem_files[idx]->file_begin);
         bbu_mfree(mem_files[idx]);
         mem_files[idx] = NULL;
      }
   }
}

/*..........................................................................*/
int mfile_dump_content_all(void)
{
   int idx = 0;
   int exist_file = 0;
   for(; idx < MAX_MEM_FILE; idx++){
      if(mem_files[idx] != NULL){
         exist_file++;
         if(exist_file == 1)
            bbu_printf("Current existent memory files  as below:\n\r");
         bbu_printf("Memory file%d: %s\n\r", idx+1, mem_files[idx]->name);
         mfile_dump_content(mem_files[idx]->name);
      }
   }
   if(!exist_file)
      bbu_printf("There is no memory file\n\r");
   return exist_file;
}
/*..........................................................................*/
int mode_translate(const char *mode)
{
   if(!strcmp( mode, "r"))
      return ((int)MFILE_R_ONLY);
   
   if(!strcmp( mode, "r+"))
      return ((int)MFILE_R_PLUS);
   
   if(!strcmp( mode, "w"))
      return ((int)MFILE_W_ONLY);
   
   if(!strcmp( mode, "w+"))
      return ((int)MFILE_W_PLUS);
   
   if(!strcmp( mode, "a"))
      return ((int)MFILE_A_ONLY);
   
   if(!strcmp( mode, "a+"))
      return ((int)MFILE_A_PLUS);
   
   if(!strcmp( mode, "rb"))
      return ((int)MFILE_RB_ONLY);
   
   if(!strcmp( mode, "rb+"))
      return ((int)MFILE_RB_PLUS);
   
   if(!strcmp( mode, "wb"))
      return ((int)MFILE_WB_ONLY);
   
   if(!strcmp( mode, "wb+"))
      return ((int)MFILE_WB_PLUS);
   
   return ((int)MFILE_INVALID);
}

#if MFILE_TEST
/*..........................................................................*/
void mfile_test(void)
{
   MFILE *mfp;
   char *line = "Hello BBU\n\r";
   char buffer[128];
   bbu_printf("\n\r");
   mfp = mfopen("mfile_test.old", "w+");
   
   bbu_printf("Debug mfprintf... \n\r");
   mfprintf(mfp, "This is a %s test\n\r", "mfile");
   mfile_dump_info("mfile_test.old");
   bbu_printf("\n\r");
   
   bbu_printf("Debug mfputs... \n\r");
   mfputs(line, mfp);
   mfile_dump_info("mfile_test.old");
   bbu_printf("\n\r");
   
   bbu_printf("Debug mfseek... \n\r");
   mfseek(mfp, 0, MFILE_SEEK_SET);
   mfile_dump_info("mfile_test.old");
   bbu_printf("\n\r");
   
   bbu_printf("Debug mfgets... \n\r");
   mfgets(buffer, sizeof(buffer), mfp);
   mfile_dump_info("mfile_test.old");
   bbu_printf(buffer);
   bbu_printf("\n\r");
   
   bbu_printf("Debug mfrename... \n\r");
   mfrename("mfile_test.old", "mfile_test.new");
   mfile_dump_info("mfile_test.new");
   mfile_dump_content("mfile_test.new");
   bbu_printf("\n\r");
   
   bbu_printf("Debug mfremove... \n\r");
   mfremove("mfile_test.new");
   mfremove("mfile_test.new");
   
   bbu_printf("\n\r");
   mfp = mfopen("mfile_test.1", "wb+");
   bbu_printf("\n\r");
   mfp = mfopen("mfile_test.2", "a+");
   bbu_printf("\n\r");
   mfile_print_all_name();
   bbu_printf("\n\r");
   mfile_dump_info("mfile_test.1");
   bbu_printf("\n\r");
   mfile_dump_info("mfile_test.2");
   bbu_printf("\n\r");
   mfile_remove_all();
}



static void bbu_mfile_test_help(void)
{
   bbu_printf("\n\r mfile - BBU memory based file system test command\n\r");
   bbu_printf(" Usage: mfile [<opt>] [<name>]\n\r");
   bbu_printf("  <opt> : all, info, dump\n\r");
   bbu_printf("   all   - display all the memory files' name\n\r");
   bbu_printf("   info  - display the infomation of a memory file\n\r");
   bbu_printf("   dump  - dump the content of a memory file\n\r");
   bbu_printf("   rm    - remove a memory file\n\r");
   bbu_printf("   test  - a simple memory file test\n\r");
   bbu_printf(" <name> : the name of a memory file\n\r");
}

int bbu_mfile_test(int argc, uint8_t **argv)
{
   if(argc < 2){
      bbu_mfile_test_help();
      return SCREEN_TEST_PASS;
   }

   if(!strcmp((const char*)argv[1], "all")){
      mfile_print_all_name();
      return SCREEN_TEST_PASS;
   }

   if(!strcmp((const char*)argv[1], "test")){
      mfile_test();
      return SCREEN_TEST_PASS;
   }

   if(!strcmp((const char*)argv[1], "info")){
      mfile_dump_info((char *)argv[2]);
      return SCREEN_TEST_PASS;
   }

   if(!strcmp((const char*)argv[1], "dump")){
      if(!strcmp((const char*)argv[2], "all")){
         mfile_dump_content_all();
      }else{
         mfile_dump_content((char *)argv[2]);
      }
      return SCREEN_TEST_PASS;
   }

   if(!strcmp((const char*)argv[1], "rm")){
      if(!strcmp((const char*)argv[2], "all")){
         mfile_remove_all();
      }else{
         mfremove((char *)argv[2]);
      }
      return SCREEN_TEST_PASS;
   }

}
#endif
