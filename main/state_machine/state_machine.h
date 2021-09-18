/*!
 *******************************************************************************
 * @file state_machine.h
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

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

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

typedef void (*state_machine_state_t)(void);

typedef enum {
        STATE_MACHINE_ACTION_START = 0,
        STATE_MACHINE_ACTION_ABORT,
        STATE_MACHINE_ACTION_COUNT
} state_machine_action_t;

typedef enum {
        STATE_MACHINE_EVENT_TYPE_MESSAGE = 0,
        STATE_MACHINE_EVENT_TYPE_ACTION,
        STATE_MACHINE_EVENT_TYPE_COUNT
} state_machine_event_type_t;

typedef char * state_machine_message_t;

typedef union {
        state_machine_action_t action;
        state_machine_message_t message;
} state_machine_data_t;

typedef struct {
        state_machine_event_type_t type;
        state_machine_data_t data;
        uint32_t time_received;
} state_machine_event_t;

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

bool state_machine_init(void);

bool state_machine_wait_for_event(uint32_t const time_ms,
                                  state_machine_event_t ** const pp_event);

bool state_machine_send_event(state_machine_event_type_t const type,
                              state_machine_data_t const data,
                              uint32_t const timeout);

#endif //STATE_MACHINE_H