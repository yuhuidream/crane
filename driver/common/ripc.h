#ifndef h_ripc_h
#define h_ripc_h

#include "RIPC_REG.h"


typedef enum{
   FREE = 0,
   BUSY4AP,
   BUSY4CP,
   BUSY4MSA,
   BUSY4OTHER,
}RIPC_STATUS;

int request_ripc(int idx, int interrupt);
int release_ripc(int idx);

void ripc_spin_lock(int idx);
void ripc_spin_unlock(int idx);

void ripc_init(void);

/* Nezha2 has 4 RIPCs, currently, we use the RIPC0 for VM manager
*/
#define request_vm_manager()   ripc_spin_lock(0)
#define release_vm_manager()   ripc_spin_unlock(0)

#endif
