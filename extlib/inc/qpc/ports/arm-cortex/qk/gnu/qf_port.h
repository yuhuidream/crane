/*****************************************************************************
* Product:  QF/C, ARM, QK port, GNU compiler
* Last Updated for Version: 4.4.02
* Date of the Last Update:  May 14, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#ifndef qf_port_h
#define qf_port_h

#include <stdint.h> 

                 /* The maximum number of active objects in the application */
#define QF_MAX_ACTIVE               63
                    /* The maximum number of event pools in the application */
#define QF_MAX_EPOOL                6

extern void restore_irq_fiq(uint32_t);
extern  uint32_t disable_irq_fiq(void);
extern void enable_irq_fiq(void);

                                             /* QF interrupt disable/enable */
#define QF_INT_DISABLE()            disable_irq_fiq() //__asm volatile ("cpsid if")
#define QF_INT_ENABLE()             enable_irq_fiq() //__asm volatile ("cpsie if")

                                          /* QF critical section entry/exit */
/* QF_INT_KEY_TYPE defined: "save and restore" policy  */
#define QF_CRIT_STAT_TYPE     uint32_t
    
#define QF_CRIT_ENTRY(critStat_)      ((critStat_) = disable_irq_fiq())
#define QF_CRIT_EXIT(critStat_)    restore_irq_fiq(critStat_) 

#define QF_LOG2(n_) ((uint8_t)(32U - __builtin_clz(n_)))

#include "qep_port.h"                                           /* QEP port */
//#include "qk_port.h"                                             /* QK port */
/*for Cortex-A series, just include qk.h is enough, qk_port.h is removed*/
#include "qk.h"                 /* QK platform-independent public interface */
#include "qf.h"                 /* QF platform-independent public interface */

#endif                                                         /* qf_port_h */
