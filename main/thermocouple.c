/*!
 *******************************************************************************
 * @file thermocouple.c
 *
 * @brief 
 *
 * @author Raúl Gotor (raul.gotor@midge-medical.com)
 * @date 18.09.21
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Midge Medical GmbH
 * All rights reserved.
 *******************************************************************************
 */

/*
 *******************************************************************************
 * #include Statements                                                         *
 *******************************************************************************
 */

#define TEMP_SIMULATION 1

#include <stdbool.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#if TEMP_SIMULATION
#include "state_machine/state_machine.h"
#include "reflow_profile.h"
#endif

#include "thermocouple.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define THERMOCOUPLE_COUNT                              2

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

static void thermocouple_update_temperature(void);

static void thremocouple_task(void * pvParameters);

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

static thermocouple_refresh_rate_t m_refresh_rate = THERMOCOUPLE_REFRESH_RATE_1_HZ;

static xTaskHandle m_thermocouple_task_h = NULL;

static bool m_is_initialized = false;

static int16_t m_temperature[THERMOCOUPLE_COUNT];

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

bool thermocouple_init(void)
{
        bool success = !m_is_initialized;
        BaseType_t result = pdPASS;

        if (success) {
                result = xTaskCreate(thremocouple_task,
                                     "thermocouple_task",
                                     configMINIMAL_STACK_SIZE,
                                     NULL,
                                     1,
                                     m_thermocouple_task_h);

                if (pdPASS != result) {
                        success = false;
                }
        }

        if (success) {

        }

        if (success) {
                thermocouple_update_temperature();
        }

        return success;
}

bool thermocouple_set_referesh_rate(thermocouple_refresh_rate_t const refresh_rate)
{
        bool success = (THERMOCOUPLE_REFRESH_RATE_COUNT > refresh_rate);

        if (success) {
                m_refresh_rate = refresh_rate;
        }

        return success;
}

bool thermocouple_get_temperature(thermocouple_id_t const id,
                                  int16_t * const temperature)
{
        bool success = true;

        *temperature = m_temperature[id];

        return success;
}


/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

static void thermocouple_update_temperature(void)
{
#if TEMP_SIMULATION
        state_machine_state_text_t state;
        reflow_profile_t profile;
        reflow_profile_get_current(&profile);
        state_machine_get_state(&state);
        state_machine_data_t data;

        switch (state) {
        case STATE_MACHINE_STATE_HEATING:
                if (profile.preheat_temperature > m_temperature[0]) {
                        m_temperature[0]++;
                } else {
                        data.message = STATE_MACHINE_MSG_HEATER_PREHEAT_TARGET_REACHED;
                        state_machine_send_event(STATE_MACHINE_EVENT_TYPE_MESSAGE, data,
                                                 portMAX_DELAY);
                }
                break;
        case STATE_MACHINE_STATE_SOAKING:
                if (profile.preheat_temperature > m_temperature[0]) {
                        m_temperature[0]++;
                }
                break;

        case STATE_MACHINE_STATE_REFLOW:
                if (profile.reflow_temperature > m_temperature[0]) {
                        m_temperature[0]++;
                } else {
                        data.message = STATE_MACHINE_MSG_HEATER_REFLOW_TARGET_REACHED;
                        state_machine_send_event(STATE_MACHINE_EVENT_TYPE_MESSAGE, data,
                                                 portMAX_DELAY);
                }
                break;

        case STATE_MACHINE_STATE_DWELL:
                if (profile.reflow_temperature > m_temperature[0]) {
                        m_temperature[0]++;
                }

                break;
        case STATE_MACHINE_STATE_COOLING:
                if (profile.cooling_temperature < m_temperature[0]) {
                        m_temperature[0]--;
                } else {
                        data.message = STATE_MACHINE_MSG_HEATER_COOLING_TARGET_REACHED;
                        state_machine_send_event(STATE_MACHINE_EVENT_TYPE_MESSAGE, data,
                                                 portMAX_DELAY);
                }
                break;
        case STATE_MACHINE_STATE_ERROR:
                if (10 < m_temperature[0]) {
                        m_temperature[0]--;
                }
                break;
        default:
                break;
        }
#else
        m_temperature[0] = hal_thermocouple_get_temperature(0);
        m_temperature[1] = hal_thermocouple_get_temperature(1);
#endif
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

void thremocouple_task(void * pvParameters)
{
#if TEMP_SIMULATION
        m_temperature[0] = 27;
#endif
        for (;;) {

#if TEMP_SIMULATION
                vTaskDelay(100);
#else
                vTaskDelay(m_refresh_rate);
#endif
                thermocouple_update_temperature();
        }
}
