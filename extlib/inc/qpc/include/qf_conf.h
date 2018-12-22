#ifndef qf_conf_h
#define qf_conf_h

#ifndef CONFIG_MSA
/** \brief the macro to enable the feature ctrl+c to break RTC step
*/
#define QF_CONTEXT_CTRL

/** \brief the macro to enable AO-suspend-wakeup, 
 *   a.k.a. RTC auto-break mechanism
*/
#define QACTIVE_SUSPEND

/** \brief the macro to enable suspened AOs to wake up disorderly
*/
#define QACTIVE_WAKE_DISORDER

/** \brief the macro to enable the component-level suspend-wakeup
*/
#define COMPOSITE_LEVEL_SUSPEND

/** \brief the macro to control the fearture of on-demand-tick
*/
#define QF_ONDEMAND_TICK

/** \brief automatic switch of QTimeEvt trigger(timer irq)
*/
#define QF_TICK_AUTO_SWITCH

/** \brief the macro to control the CPU cycle monitoring for each AO
*/
#define QACTIVE_CPU_CYCLE

/** \brief the macro to control the CPU hot plug/unplug mechanism
*/
#define QF_CPU_HTPLG

/** \brief the macro to control the AO-tied-2-Core mechanism
*/
#define QACTIVE_CPU_TIED

/** \brief the macro to control the irq-tied-2-AO mechanism
*/
#define QACTIVE_IRQ_TIED

#define COMPONENT_EVT_PS

#define LRAB
#endif
#endif
