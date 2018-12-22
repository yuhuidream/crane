#ifndef h_qa_stk_h
#define h_qa_stk_h

/** \brief The length(byte number) of stack signature, if one of these bytes
* is overwritten, we consider that stack may be overflow
*/
#define STACK_SIGNATURE_LEN  64

/** \brief Byte and word version of stack signature
*/
#define STACK_SIGNATURE_BYTE 0xBD
#define STACK_SIGNATURE_WORD 0xBDBDBDBD

/** \brief The minimum stack size which is supported
*/
#define MIN_STACK_SIZE       0x400

#ifdef QF_STACK_PER_AO                /* enable the QF_STACK_PER_AO feature */

/** \brief Initialize the head and the end of the AO-stack-buffer
*
* \sa QA_stackInit()
*/
void QF_stackInit(uint32_t ao_stack_start, uint32_t ao_stack_end);

/** \brief Initialize the stack of a specific Active object
*
* \a act, AO pointer; stkSize, stack size, unit is KB
* \sa QF_stackInit(), QA_stkCheckOverflow()
*/
void QA_stackInit(QActive *act, uint32_t stkSize);

/** \brief Check if an AO stack is overflow
*
* \note Simply compare the 16-Byte of the stack top with the signature
* \a act, AO pointer
* \sa QA_stkUsedSize()
*/
int QA_stkCheckOverflow(QActive *act);

/** \brief Check how many bytes have been used of a AO stack
*
* \a act, AO pointer
* \sa QA_stkCheckOverflow()
*/
int QA_stkUsedSize(QActive *act);

/** \brief Macro used to switch CPU stack to AO stack
*
* \a p, AO priority
* \sa QF_SAVE_CPU_SP(), QF_SWITCH_2_CPU_STK()
*/
#define QF_SWITCH_2_ACT_STK(p_) \
   do{      \
      __asm__ __volatile__(      \
         "ldr  sp, [%0, #0]\n"      \
         :     \
         :"r" (&QF_active_[p_]->stk)      \
         :       \
      );    \
   }while(0)

/** \brief Macro used to save the CPU stack pointer
*
* \a, c, core id(not used in single core mode)
* \sa QF_SWITCH_2_ACT_STK(), QF_SWITCH_2_CPU_STK()
*/
#ifdef CONFIG_SMP
#define QF_SAVE_CPU_SP(c_)   \
   do{      \
      __asm__ __volatile__(   \
        "str  sp, [%0, #0]\n"    \
        :         \
        :"r" (&(QCORE_STACK_POINTER(c_)))    \
        :     \
      );          \
   }while(0)
#else
#define QF_SAVE_CPU_SP(c_)   \
   do{      \
      (void)c_;    \
      __asm__ __volatile__(      \
         "str  sp,  [%0, #0]\n"        \
         :           \
         :"r" (&(QCORE_STACK_POINTER()))     \
         :   \
      );       \
   }while(0)
#endif

/** \brief Macro used to switch AO stack to CPU stack
*
* \a, c, core id(not used in single core mode)
* \sa QF_SWITCH_2_ACT_STK(), QF_SAVE_CPU_SP()
*/
#ifdef CONFIG_SMP
#define QF_SWITCH_2_CPU_STK(c_)   \
   do{      \
      __asm__ __volatile__(         \
         "ldr  sp,  [%0, #0]\n"        \
         :        \
         :"r" (&(QCORE_STACK_POINTER(c_)))   \
         :    \
      );       \
   }while(0)
#else
#define QF_SWITCH_2_CPU_STK(c_)   \
   do{      \
      (void)c_;    \
      __asm__ __volatile__(      \
         "ldr  sp, [%0, #0]\n"      \
         :        \
         :"r"(&(QCORE_STACK_POINTER()))      \
         :    \
      );       \
   }while(0)
#endif

#else                                /* disable the QF_STACK_PER_AO feature */

#define QF_SWITCH_2_ACT_STK(p_)  do{(void)p_;}while(0)
#define QF_SAVE_CPU_SP(c_)       do{(void)c_;}while(0)
#define QF_SWITCH_2_CPU_STK(c_)  do{(void)c_;}while(0)
#define QF_stackInit(start_, end_)
#define QA_stackInit(act_, size_)   do{(void)act_;(void)size_;}while(0)
#define QA_stkCheckOverflow(act_)   do{(void)act_;}while(0)
#define QA_stkUsedSize(act_)        do{(void)act_; }while(0)
#endif                                    /* end of 'ifdef QF_STACK_PER_AO' */
#endif                                        /* end of 'ifndef h_qa_stk_h' */
