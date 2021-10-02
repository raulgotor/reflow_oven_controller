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

typedef struct {
        state_machine_state_text_t text;
        state_machine_state_t function;
} state_machine_state_map_t;

/*
 *******************************************************************************
 * Public Constants                                                            *
 *******************************************************************************
 */

state_machine_state_text_t state_machine_pointer_to_text(
        state_machine_state_t const state);

state_machine_state_t state_machine_text_to_pointer(state_machine_state_text_t const text);

/*
 *******************************************************************************
 * Public Function Prototypes                                                  *
 *******************************************************************************
 */

void state_machine_states_set_entry_point_state(void);

#endif //STATE_MACHINE_STATES_IDLE_H