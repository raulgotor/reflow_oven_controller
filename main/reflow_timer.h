/*!
 *******************************************************************************
 * @file reflow_timer.h
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 26.09.21
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */

#ifndef REFLOW_TIMER_H
#define REFLOW_TIMER_H

/*
 *******************************************************************************
 * Public Macros                                                               *
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Public Constants                                                            *
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Public Function Prototypes                                                  *
 *******************************************************************************
 */

bool reflow_timer_init(void);

bool reflow_timer_start_timer(uint32_t const period_s, state_machine_state_text_t const state);

bool reflow_timer_stop_timer(void);


#endif //REFLOW_TIMER_H