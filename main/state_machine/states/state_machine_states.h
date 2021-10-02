/*!
 *******************************************************************************
 * @file state_machine_states_idle.h
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 18.09.21
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */

#ifndef STATE_MACHINE_STATES_IDLE_H
#define STATE_MACHINE_STATES_IDLE_H

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

void state_machine_states_set_entry_point_state(void);

void state_machine_state_idle(void);

void state_machine_state_heating(void);

void state_machine_state_soak(void);

void state_machine_state_reflow(void);

void state_machine_state_dwell(void);

void state_machine_state_cooling(void);

void state_machine_state_error(void);

#endif //STATE_MACHINE_STATES_IDLE_H