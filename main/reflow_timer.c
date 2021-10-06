/*!
 *******************************************************************************
 * @file reflow_timer.c
 *
 * @brief 
 *
 * @author Raúl Gotor (raul.gotor@midge-medical.com)
 * @date 26.09.21
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

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "state_machine/states/state_machine_states.h"
#include "state_machine/state_machine.h"
#include "reflow_timer.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define TAG                                 __FILENAME__
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

static void reflow_timer_callback(TimerHandle_t handle);

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

 xTimerHandle m_reflow_timer_h = NULL;

static bool m_is_initialized = false;

static state_machine_state_text_t m_reflow_timer_state = STATE_MACHINE_STATE_COUNT;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

bool reflow_timer_init(void)
{
        bool success = (!m_is_initialized);

        if (success) {
                m_reflow_timer_h = xTimerCreate("reflow_timer",
                                                portMAX_DELAY,
                                                pdFALSE,
                                                (void *)0,
                                                reflow_timer_callback);

                success = (NULL != m_reflow_timer_h);
        }

        if (success) {
                m_is_initialized = true;
        }

        return success;
}

bool reflow_timer_start_timer(uint32_t const period_s,
                              state_machine_state_text_t const state)
{
        uint32_t const period_ticks = pdMS_TO_TICKS(period_s * 1000);
        BaseType_t result;
        bool success;

        result = xTimerChangePeriod(m_reflow_timer_h, pdMS_TO_TICKS(period_ticks), portMAX_DELAY);

        success  = (pdPASS == result);

        if (success) {
                result = xTimerStart(m_reflow_timer_h, portMAX_DELAY);
                success  = (pdPASS == result);
        }

        if (success) {
                m_reflow_timer_state = state;
                ESP_LOGI(TAG, "Timer started for %d ticks", pdMS_TO_TICKS(period_ticks));

        }

        return success;
}

bool reflow_timer_stop_timer(void)
{
        BaseType_t result;

        result = xTimerStop(m_reflow_timer_h, portMAX_DELAY);

        return (pdPASS == result);
}

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

static void reflow_timer_callback(TimerHandle_t const handle)
{
        state_machine_msg_t const message =
                        state_machine_get_timeout_msg(m_reflow_timer_state);

        bool success = (STATE_MACHINE_MSG_COUNT != message);
        state_machine_data_t data;

        ESP_LOGI(TAG, "Timer is done, state is %d and message is %d", m_reflow_timer_state, message);

        if (success) {
                data.message = message;
                state_machine_send_event(STATE_MACHINE_EVENT_TYPE_MESSAGE,
                                         data, STATE_MACHINE_TIMEOUT_CALLBACK);
        }
}
