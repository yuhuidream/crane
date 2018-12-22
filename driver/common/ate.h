/*
 *  ate.h
 *
 *  Copyright (C) 2012 Marvell Corporation
 *
 */
 
#ifndef ATE_LOW_LEVEL_H
#define ATE_LOW_LEVEL_H


#ifdef CONFIG_ATE
void ate_case_select_gpio_init(void);
void ate_save_enter_lpm(void);
void ate_resume_exit_lpm(void);
#endif


#endif
