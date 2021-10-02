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

//TODO: define time and convert to ticks
#define STATE_MACHINE_TIMEOUT_CALLBACK          1000

/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

typedef void (*state_machine_state_t)(void);

typedef enum {
        STATE_MACHINE_ACTION_START = 0,
        STATE_MACHINE_ACTION_PAUSE,
        STATE_MACHINE_ACTION_ABORT,
        STATE_MACHINE_ACTION_RESET,
        STATE_MACHINE_ACTION_COUNT
} state_machine_action_t;

typedef enum {
        STATE_MACHINE_EVENT_TYPE_MESSAGE = 0,
        STATE_MACHINE_EVENT_TYPE_ACTION,
        STATE_MACHINE_EVENT_TYPE_COUNT
} state_machine_event_type_t;

typedef enum {
        STATE_MACHINE_MSG_HEATER_PREHEAT_TARGET_REACHED = 0,
        STATE_MACHINE_MSG_HEATER_REFLOW_TARGET_REACHED,
        STATE_MACHINE_MSG_SOAK_TIME_REACHED,
        STATE_MACHINE_MSG_DWELL_TIME_REACHED,
        STATE_MACHINE_MSG_HEATER_COOLING_TARGET_REACHED,
        STATE_MACHINE_MSG_HEATER_COOLING_TIMEOUT,
        STATE_MACHINE_MSG_HEATER_ERROR,
        STATE_MACHINE_MSG_HEATER_TIMEOUT,
        STATE_MACHINE_MSG_HEATER_TOO_FAST,
        STATE_MACHINE_MSG_HEATER_TOO_SLOW,
        STATE_MACHINE_MSG_COUNT
} state_machine_msg_t;

typedef union {
        state_machine_action_t user_action;
        state_machine_msg_t message;
} state_machine_data_t;

typedef struct {
        state_machine_event_type_t type;
        state_machine_data_t data;
        uint32_t time_received;
} state_machine_event_t;

typedef enum {
        STATE_MACHINE_STATE_IDLE = 0,
        STATE_MACHINE_STATE_HEATING,
        STATE_MACHINE_STATE_SOAKING,
        STATE_MACHINE_STATE_REFLOW,
        STATE_MACHINE_STATE_DWELL,
        STATE_MACHINE_STATE_COOLING,
        STATE_MACHINE_STATE_ERROR,
        STATE_MACHINE_STATE_COUNT
} state_machine_state_text_t;

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

bool state_machine_get_state(state_machine_state_text_t * const p_state);

bool state_machine_wait_for_event(uint32_t const time_ms,
                                  state_machine_event_t * const p_event);

bool state_machine_send_event(state_machine_event_type_t const type,
                              state_machine_data_t const data,
                              uint32_t const timeout);

state_machine_msg_t state_machine_get_timeout_msg(
                state_machine_state_text_t const state);

#endif //STATE_MACHINE_H