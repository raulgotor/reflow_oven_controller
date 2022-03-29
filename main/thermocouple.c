/*!
 *******************************************************************************
 * @file thermocouple.c
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

#define TEMP_SIMULATION 0

#include <stdbool.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <tp_spi.h>
#include <esp_log.h>

#include "state_machine/states/state_machine_states.h"
#include "state_machine/state_machine.h"
#include "state_machine/state_machine.h"
#include "reflow_profile.h"
#include "driver/spi_master.h"
#include "maxim_max6675.h"
#include "panic.h"
#include "wdt.h"
#include "thermocouple.h"
#include "reflow_profile.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

//! @brief Number of thermocouples available
#define THERMOCOUPLE_COUNT                              1

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

//! @brief Update temperature value
static bool thermocouple_update_temperature(void);

//! @brief Thermocouple internal task
static void thermocouple_task(void * pvParameters);

/*
 *******************************************************************************
 * Public Data Declarations                                                    *
 *******************************************************************************
 */

//! @brief Thermocouple task handle
xTaskHandle m_thermocouple_task_h = NULL;

/*
 *******************************************************************************
 * Static Data Declarations                                                    *
 *******************************************************************************
 */

//TODO: unused
//! @brief Thermocouple refresh rate
static thermocouple_refresh_rate_t m_refresh_rate = THERMOCOUPLE_REFRESH_RATE_1_HZ;

//! @brief Whether the module is initialized or not
static bool m_is_initialized = false;

//! @brief Temperature tracking of the different thermocouples
static int16_t m_temperature[THERMOCOUPLE_COUNT];

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

/*!
 * @brief Initialize thermocouple module
 *
 * @params              -                   -
 *
 * @return              bool                Operation result
 * @retval              true                Everything went well
 * @retval              false               Module already initialized,
 *                                          error initializing the driver,
 *                                          not enought memory,
 *                                          couldn't add task to WDT or
 *                                          issues while reading temperature
 */
bool thermocouple_init(void)
{
        bool success = !m_is_initialized;
        BaseType_t result = pdPASS;
        max6675_error_t max6675_result;

        max6675_result = max6675_init(max6675_spi_xchg);

        success = (MAX6675_ERROR_SUCCESS == max6675_result);

        if (success) {
                result = xTaskCreate(thermocouple_task,
                                     "thermocouple_task",
                                     configMINIMAL_STACK_SIZE*4,
                                     NULL,
                                     1,
                                     &m_thermocouple_task_h);

                if (pdPASS != result) {
                        success = false;
                }
        }

        if (success) {
                success = wdt_add_task(m_thermocouple_task_h);
        }

        if (success) {
                success = thermocouple_update_temperature();
        }


        if (success) {
                m_is_initialized = true;
        }

        return success;
}

/*!
 * @brief Get thermocouple temperature
 *
 * Gets the temperature in degrees celsius from the internal module tracking
 * variable.
 *
 * @note The function returns the temperature atomically, so it is thread safe
 *
 * @param               id                  Thermocouple ID to get the temp.
 *                                          from
 * @param               temperature         Pointer where to store the
 *                                          retrieved temperature (degrees Celsius)
 *
 * @return              bool                Operation result
 * @retval              true                Everything went well
 * @retval              false               Invalid pointer or ID
 */
bool thermocouple_get_temperature(thermocouple_id_t const id,
                                  int16_t * const temperature)
{
        bool success = true;
        //TODO implement id check
        *temperature = m_temperature[id];

        return success;
}


/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

/*!
 * @brief Update temperature value
 *
 * This function will read and convert the thermocouple sensor and update the
 * internal module value accordingly. This function is meant to be called
 * periodically.
 *
 * @param               -                   -
 *
 * @return              bool                Operation result
 * @retval              true                Everything went well
 * @retval              false               Reading failed or thermocouple is on
 *                                          open-circuit
 */
static bool thermocouple_update_temperature(void)
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

        uint16_t temperature;
        bool sensor_is_connected;
        bool success;
        max6675_error_t result;
        uint16_t const centideg_to_deg_factor = 100;

        result = max6675_is_sensor_connected(&sensor_is_connected);

        success = ((MAX6675_ERROR_SUCCESS == result) && (sensor_is_connected));

        if (success) {
                result = max6675_read_temperature(&temperature);

                // Convert to degrees celsius with rounding
                temperature += (centideg_to_deg_factor / 2);
                temperature /= centideg_to_deg_factor;
                m_temperature[0] = (int16_t)temperature;

                success = (MAX6675_ERROR_SUCCESS == result);
        }

        return success;
#endif
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

/*!
 * @brief Thermocouple internal task
 *
 * The task will send events to the state machine when a specific temperature
 * target for the current state is reached. For that, the task constantly
 * updates the temperature value and state.
 *
 * Once the event is sent to the state machine, it will wait for it to
 * acknowledge before continue processing further. If the process takes long or
 * gets blocked, the WDT will bark and a panic condition will be raised.
 *
 * @param               pvParameters        Not used
 *
 * @return              -                   -
 */
void thermocouple_task(void * pvParameters)
{
        bool success;
        state_machine_state_text_t state;
        state_machine_data_t data;
        reflow_profile_t profile;

#if TEMP_SIMULATION
        m_temperature[0] = 27;
#endif
        for (;;) {

                // According to datasheet, conversion time is 170 ms nominal.
                // Setting a readout time smaller than conversion time will reset
                // the conversion and will return the previous value again
                vTaskDelay(1000);

                success = thermocouple_update_temperature();
                data.message = STATE_MACHINE_MSG_COUNT;

                if (success) {
                        success = reflow_profile_get_current(&profile);
                }

                if (success) {
                        (void)state_machine_get_state(&state);
                } else {
                        // Code style exception for readability
                        break;
                }

                switch (state) {
                case STATE_MACHINE_STATE_HEATING:
                        if (profile.preheat_temperature <= m_temperature[0]) {
                                data.message = STATE_MACHINE_MSG_HEATER_PREHEAT_TARGET_REACHED;
                        }
                        break;


                case STATE_MACHINE_STATE_REFLOW:
                        if (profile.reflow_temperature <= m_temperature[0]) {
                                data.message = STATE_MACHINE_MSG_HEATER_REFLOW_TARGET_REACHED;
                        }
                        break;

                case STATE_MACHINE_STATE_COOLING:
                        if (profile.cooling_temperature >= m_temperature[0]) {
                                data.message = STATE_MACHINE_MSG_HEATER_COOLING_TARGET_REACHED;
                        }

                // Intentionally fall through
                case STATE_MACHINE_STATE_SOAKING:
                case STATE_MACHINE_STATE_DWELL:
                case STATE_MACHINE_STATE_ERROR:
                default:
                        break;
                }

                if (STATE_MACHINE_MSG_COUNT != data.message) {

                        success = state_machine_send_event(
                                        STATE_MACHINE_EVENT_TYPE_MESSAGE,
                                        data,
                                        portMAX_DELAY);

                        if (!success) {
                                // Code style exception for readability
                                break;
                        }

                        /*
                         * Wait for the state machine to process the event and
                         * change state so the message is not sent multiple
                         * times
                         */
                        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                }

                if (!wdt_kick()) {
                        break;
                }
        }

        panic("General failure at thermocouple_task ", __FILENAME__, __LINE__);
}
