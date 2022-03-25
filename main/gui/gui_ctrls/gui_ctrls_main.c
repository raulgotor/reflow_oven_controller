/*!
 *******************************************************************************
 * @file gui_ctrls_main.c
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 17.10.21
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "reflow_profile.h"
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "lvgl.h"
#include "state_machine/states/state_machine_states.h"
#include "state_machine/state_machine.h"
#include "thermocouple.h"
#include "gui/gui_views/gui_views_main.h"
#include "gui/gui_ctrls/gui_ctrls_main.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define TAG                                 __FILENAME__

#define BUTTON_TEXT_START                   "Start"
#define BUTTON_TEXT_STOP                    "Stop"

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

/*
 *******************************************************************************
 * Public Data Declarations                                                    *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

void gui_ctrls_main_init(void)
{
        reflow_profile_t profile;
        int16_t meter_value_max;
        bool success = reflow_profile_get_current(&profile);

        if (!success) {
                assert(0);
        }

        meter_value_max = (int16_t)profile.reflow_temperature;

        lv_lmeter_set_range(p_lmeter, 0, meter_value_max);

        gui_ctrls_main_update_buttons(STATE_MACHINE_STATE_IDLE);
}

void gui_ctrls_main_refresh(void)
{
        char temperature_str[10];
        char const * profile_name;
        int16_t temperature;
        bool success;
        int16_t meter_value_max;
        reflow_profile_t reflow_profile;

        success = thermocouple_get_temperature(0, &temperature);

        if (success) {
                success = reflow_profile_get_current(&reflow_profile);
        }

        if (success) {
                meter_value_max = (int16_t)reflow_profile.reflow_temperature;
                profile_name = reflow_profile.name;

                // TODO: round
                snprintf(temperature_str, 9, "%dº", temperature / 100);
                lv_label_set_text(p_temp_label, temperature_str);
                lv_label_set_text(p_profile_label, profile_name);
                lv_lmeter_set_value(p_lmeter, temperature);
                lv_lmeter_set_range(p_lmeter, 0, meter_value_max);
        }
}


void gui_ctrls_main_button_event_cb(lv_obj_t * p_object, lv_event_t event)
{
        state_machine_data_t state_machine_event_data;
        state_machine_state_text_t state;
        bool success;

        if (LV_EVENT_CLICKED != event) {
                return;
        }

        if (p_start_button == p_object) {
                ESP_LOGI(TAG, "Start button pressed");
                success = state_machine_get_state(&state);

                if (!success) {
                        return;
                }

                switch (state) {

                case STATE_MACHINE_STATE_IDLE:
                        state_machine_event_data.user_action = STATE_MACHINE_ACTION_START;
                        break;

                        // Intentionally fall-through
                case STATE_MACHINE_STATE_HEATING:
                case STATE_MACHINE_STATE_SOAKING:
                case STATE_MACHINE_STATE_REFLOW:
                case STATE_MACHINE_STATE_DWELL:
                        state_machine_event_data.user_action = STATE_MACHINE_ACTION_ABORT;
                        break;

                default:
                        return;
                }

                state_machine_send_event(STATE_MACHINE_EVENT_TYPE_ACTION,
                                         state_machine_event_data,
                                         portMAX_DELAY);
        }
}

void gui_ctrls_main_update_buttons(state_machine_state_text_t const state)
{
        char const * const p_state_str = state_machine_get_state_string(state);

        if (NULL == p_state_str) {
                // Code style exception for readability
                return;
        }

        switch (state) {
        // Intentionally fall through
        case STATE_MACHINE_STATE_HEATING:
        case STATE_MACHINE_STATE_SOAKING:
        case STATE_MACHINE_STATE_REFLOW:
        case STATE_MACHINE_STATE_DWELL:
                lv_label_set_text(p_start_button_label, LV_SYMBOL_STOP BUTTON_TEXT_STOP);
                break;
        case STATE_MACHINE_STATE_IDLE:
                lv_label_set_text(p_start_button_label, LV_SYMBOL_PLAY BUTTON_TEXT_START);
                break;
        case STATE_MACHINE_STATE_COOLING:
                break;
        default:
                break;
        }

        lv_label_set_text(p_state_label, p_state_str);
}


/*
 *******************************************************************************
 * Static Data Declarations                                                    *
 *******************************************************************************
 */



/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */


