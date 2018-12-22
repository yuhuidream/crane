#ifndef h_shared_mem_h
#define h_shared_mem_h
#include "mem_map.h"

#define CA7_QUEUE_CTRL_LEN  (0x100)
#define CR5_QUEUE_CTRL_LEN  (0x100)
#define DSP_QUEUE_CTRL_LEN  (0x100)
#define SHARE_POOL_CTRL_LEN (0x100)
#define CA7_QUEUE_LEN       (0x80)
#define CR5_QUEUE_LEN       (0x80)
#define DSP_QUEUE_LEN       (0x80)
#define MAX_EVENT_LEN       (1024)
#define MAX_EVENT_NUM       (128)

#define SM_POOL0_CTRL_LEN         (0x100)
#define SM_POOL0_BLOCK_LEN        (64)
#define SM_POOL0_BLOCK_NUM        (128)

#define CORE_SHARE_MEM_HEAD    CORE_SHARE_ST
#define CORE_SHARE_MEM_LEN     CORE_SHARE_LEN

#define CA7_QUEUE_CTRL      (CORE_SHARE_MEM_HEAD)
#define CR5_QUEUE_CTRL      (CA7_QUEUE_CTRL+CA7_QUEUE_CTRL_LEN)
#define DSP_QUEUE_CTRL      (CR5_QUEUE_CTRL+CR5_QUEUE_CTRL_LEN)
#define SHARE_POOL_CTRL     (DSP_QUEUE_CTRL+DSP_QUEUE_CTRL_LEN)
#define CA7_QUEUE_BUF       (SHARE_POOL_CTRL+SHARE_POOL_CTRL_LEN)
#define CR5_QUEUE_BUF       (CA7_QUEUE_BUF+(CA7_QUEUE_LEN*4))
#define DSP_QUEUE_BUF       (CR5_QUEUE_BUF+(CR5_QUEUE_LEN*4))
#define SHARE_POOL_BUF      (DSP_QUEUE_BUF+(DSP_QUEUE_LEN*4))
#define SHARE_POOL_SIZE     (MAX_EVENT_LEN*MAX_EVENT_NUM)
#define SHARE_POOL_END      (SHARE_POOL_BUF+SHARE_POOL_SIZE)
#define SM_POOL0_CTRL       (SHARE_POOL_END+4)
#define SM_POOL0_BUF        (SM_POOL0_CTRL+SM_POOL0_CTRL_LEN)
#define SM_POOL0_SIZE       (SM_POOL0_BLOCK_LEN*SM_POOL0_BLOCK_NUM)
#define VM_NCNB_START       (SM_POOL0_BUF+SM_POOL0_SIZE)
#define VM_NCNB_LEN         (VM_NCNB_START+4)
#define VM_CB_START         (VM_NCNB_LEN+4)
#define VM_CB_LEN           (VM_CB_START+4)
#define CR5_RLS_FLAG_BUF    (VM_CB_LEN+4)
#define UART_OWNER_BUF       (CR5_RLS_FLAG_BUF+4)

#define shared_vm_ncnb_start  *((volatile unsigned int *)VM_NCNB_START)
#define shared_vm_ncnb_len    *((volatile unsigned int *)VM_NCNB_LEN)
#define shared_vm_cb_start    *((volatile unsigned int *)VM_CB_START)
#define shared_vm_cb_len      *((volatile unsigned int *)VM_CB_LEN)

#define CR5_RLS_FLAG        *((volatile unsigned int *)CR5_RLS_FLAG_BUF)
#define CR5_IN_RESET        (0x0)
#define CR5_OUT_RESET       (0xdeadbeef)

//1 AP, 2, CP, 3, DSP
#define UART_OWNER          *((volatile unsigned int *)UART_OWNER_BUF)


//stores two 4bytes pointer
#define VM_MAMAGER_BUFF         ((UART_OWNER_BUF+8)&0xFFFFFFFC)
#define VM_MAMAGER_BUFF_LEN     0x8
#define VM_MAMAGER_BUFF_END     (VM_MAMAGER_BUFF+VM_MAMAGER_BUFF_LEN)
#define NCNB_BUFF_PTR           *((volatile unsigned int *)VM_MAMAGER_BUFF)
#define CB_BUFF_PTR           *((volatile unsigned int *)VM_MAMAGER_BUFF+4)

#endif
