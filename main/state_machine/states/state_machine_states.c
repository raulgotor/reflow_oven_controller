/*!
 *******************************************************************************
 * @file state_machine_states.c
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

/*
 *******************************************************************************
 * #include Statements                                                         *
 *******************************************************************************
 */

#include <stdbool.h>
#include <stddef.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "reflow_profile.h"
#include "state_machine/states/state_machine_states.h"
#include "state_machine/state_machine.h"
#include "state_machine/state_machine_task.h"
#include "reflow_timer.h"
#include "gui.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define TAG                                     __FILENAME__

/*
 *******************************************************************************
 * Data types                                                                  *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Constants                                                                   *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private Function Prototypes                                                 *
 *******************************************************************************
 */

static bool clean_up_device(void);

/*
 *******************************************************************************
 * Public Data Declarations                                                    *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Static Data Declarations                                                    *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

void state_machine_states_set_entry_point_state(void)
{
        state_machine_set_state(state_machine_state_idle);
}

void state_machine_state_idle(void)
{
        state_machine_event_t event = {.type = STATE_MACHINE_EVENT_TYPE_COUNT};
        state_machine_state_text_t state;
        bool success = state_machine_get_state(&state);
        ESP_LOGI(TAG, "State Idle");

        if (success) {
                gui_configure_buttons_for_state(state);
                success = state_machine_wait_for_event(portMAX_DELAY, &event);

        }

        if (success) {
                // TODO: validate event?
        }

        if (!success) {
                assert(0);
        } else {
                switch (event.type) {
                case STATE_MACHINE_EVENT_TYPE_ACTION:
                        if (STATE_MACHINE_ACTION_START == event.data.user_action) {
                                state_machine_set_state(state_machine_state_heating);
                        } else {
                                assert(0 && "This event type was not expected here");
                        }
                        break;
                default:
                        assert(0 && "This event type was not expected here");
                }
        }
}

static void state_machine_transition_abort(void)
{

        ESP_LOGI(TAG, "Transition Abort");
        reflow_timer_stop_timer();
        state_machine_set_state(state_machine_state_cooling);
}

void state_machine_state_heating(void)
{
        ESP_LOGI(TAG, "State Heating");

        state_machine_event_t event;
        state_machine_state_text_t state;
        bool success = state_machine_get_state(&state);
        reflow_profile_t profile;

        if (success) {
                gui_configure_buttons_for_state(state);
                success = reflow_profile_get_current(&profile);
        }

        if (success) {
                // TODO: success = heater_set_target(profile.reflow_temperature);
        }

        if (success) {
                success = state_machine_wait_for_event(portMAX_DELAY, &event);
        }

        if (!success) {
                state_machine_set_state(state_machine_state_error);
        } else {
                switch (event.type) {
                case STATE_MACHINE_EVENT_TYPE_ACTION:
                        if (STATE_MACHINE_ACTION_ABORT == event.data.user_action) {
                                state_machine_transition_abort();
                                state_machine_set_state(state_machine_state_cooling);
                        }
                        break;
                case STATE_MACHINE_EVENT_TYPE_MESSAGE:
                        if (STATE_MACHINE_MSG_HEATER_PREHEAT_TARGET_REACHED == event.data.message) {
                                state_machine_set_state(state_machine_state_soak);
                        } else if (STATE_MACHINE_MSG_HEATER_ERROR ==
                                   event.data.message) {
                                state_machine_set_state(state_machine_state_error);
                        }
                        break;
                default:
                        assert(0);
                }
        }
}

void state_machine_state_soak(void)
{
        ESP_LOGI(TAG, "State Soaking");

        state_machine_event_t event;
        state_machine_state_text_t state;
        bool success = state_machine_get_state(&state);
        reflow_profile_t profile;

        if (success) {
                gui_configure_buttons_for_state(state);
                success = reflow_profile_get_current(&profile);
        }

        if (success) {
                success = reflow_timer_start_timer(profile.soak_time_s, state);
        }

        if (success) {
                success = state_machine_wait_for_event(portMAX_DELAY, &event);
        }

        if (!success) {
                state_machine_set_state(state_machine_state_error);
        }

        switch (event.type) {
        case STATE_MACHINE_EVENT_TYPE_ACTION:
                if (STATE_MACHINE_ACTION_ABORT == event.data.user_action) {
                        state_machine_transition_abort();
                        state_machine_set_state(state_machine_state_cooling);
                }
                break;
        case STATE_MACHINE_EVENT_TYPE_MESSAGE:
                if (STATE_MACHINE_MSG_SOAK_TIME_REACHED == event.data.message) {
                        state_machine_set_state(state_machine_state_reflow);
                } else if (STATE_MACHINE_MSG_HEATER_ERROR ==
                           event.data.message) { //cannot keep temperature, too high, too low for long time, etc
                        state_machine_set_state(state_machine_state_error);
                }
                break;
        default:
                assert(0);
        }
}

void state_machine_state_reflow(void)
{
        ESP_LOGI(TAG, "State Reflow");

        state_machine_event_t event;
        state_machine_state_text_t state;
        bool success = state_machine_get_state(&state);
        reflow_profile_t profile;

        if (success) {
                gui_configure_buttons_for_state(state);
                success = reflow_profile_get_current(&profile);
        }

        if (success) {
                //TODO: implement
                // success = heater_set_target(profile.reflow_temperature);
        }

        if (success) {
                //TODO: move to right place
                uint32_t const m_reflow_timeout = portMAX_DELAY;
                success = state_machine_wait_for_event(m_reflow_timeout, &event);
        }

        if (!success) {
                state_machine_set_state(state_machine_state_error);
        }

        switch (event.type) {
        case STATE_MACHINE_EVENT_TYPE_ACTION:
                if (STATE_MACHINE_ACTION_ABORT == event.data.user_action) {
                        state_machine_transition_abort();
                        state_machine_set_state(state_machine_state_cooling);
                }
                break;
        case STATE_MACHINE_EVENT_TYPE_MESSAGE:
                if (STATE_MACHINE_MSG_HEATER_REFLOW_TARGET_REACHED == event.data.message) {
                        state_machine_set_state(state_machine_state_dwell);
                } else if (STATE_MACHINE_MSG_HEATER_ERROR ==
                           event.data.message) {
                        state_machine_set_state(state_machine_state_error);
                }
                break;
        default:
                assert(0);
        }
}

void state_machine_state_dwell(void)
{
        ESP_LOGI(TAG, "State Dwell");

        state_machine_event_t event;
        state_machine_state_text_t state;
        bool success = state_machine_get_state(&state);
        reflow_profile_t profile;

        if (success) {
                gui_configure_buttons_for_state(state);
                success = reflow_profile_get_current(&profile);
        }

        if (success) {
                success = reflow_timer_start_timer(profile.dwell_time_s, state);
        }

        if (success) {
                //TODO: move to right place
                uint32_t const m_reflow_timeout = portMAX_DELAY;
                success = state_machine_wait_for_event(m_reflow_timeout, &event);
        }

        if (!success) {
                state_machine_set_state(state_machine_state_error);
        }

        switch (event.type) {
        case STATE_MACHINE_EVENT_TYPE_ACTION:
                if (STATE_MACHINE_ACTION_ABORT == event.data.user_action) {
                        state_machine_transition_abort();
                        state_machine_set_state(state_machine_state_cooling);
                }
                break;
        case STATE_MACHINE_EVENT_TYPE_MESSAGE:
                if (STATE_MACHINE_MSG_DWELL_TIME_REACHED == event.data.message) {
                        state_machine_transition_abort();
                        state_machine_set_state(state_machine_state_cooling);
                } else if (STATE_MACHINE_MSG_HEATER_ERROR ==
                           event.data.message) {
                        state_machine_set_state(state_machine_state_error);
                }
                break;
        default:
                assert(0);
        }
}

void state_machine_state_cooling(void)
{
        ESP_LOGI(TAG, "State Cooling");

        state_machine_event_t event;
        state_machine_state_text_t state;
        bool success = state_machine_get_state(&state);

        if (success) {
                gui_configure_buttons_for_state(state);
        }

        if (success) {
                //TODO: implement
                // success = heater_set_target(m_cooling_temperature);
                // with a ramp?
        }

        if (success) {
                //TODO: move to right place
                uint32_t const m_cooling_timeout = portMAX_DELAY;
                success = state_machine_wait_for_event(m_cooling_timeout, &event);
        }

        if (!success) {
                state_machine_set_state(state_machine_state_error);
        }

        switch (event.type) {
        case STATE_MACHINE_EVENT_TYPE_ACTION:
                break;
        case STATE_MACHINE_EVENT_TYPE_MESSAGE:
                if (STATE_MACHINE_MSG_HEATER_COOLING_TARGET_REACHED == event.data.message) {
                        state_machine_set_state(state_machine_state_idle);
                } else if (STATE_MACHINE_MSG_HEATER_ERROR ==
                           event.data.message) {
                        state_machine_set_state(state_machine_state_error);
                }
                break;
        default:
                assert(0);
        }
}

void state_machine_state_error(void)
{
        ESP_LOGI(TAG, "State Error");

        state_machine_event_t event;
        state_machine_state_text_t state;
        bool success = true;

        gui_configure_buttons_for_state(STATE_MACHINE_STATE_ERROR);

        (void)clean_up_device();
        // TODO: display_ui_advice();

        success = state_machine_wait_for_event(portMAX_DELAY, &event);

        if (!success) {
                state_machine_set_state(state_machine_state_error);
        }

        switch (event.type) {
        case STATE_MACHINE_EVENT_TYPE_ACTION:
                if (STATE_MACHINE_ACTION_RESET == event.data.user_action) {
                        state_machine_set_state(state_machine_state_idle);
                }
                break;
        default:
                assert(0);
        }

}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

static bool clean_up_device(void)
{
        // TODO: bool success = heater_stop_heater();
        bool success = true;

        return success;
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */
