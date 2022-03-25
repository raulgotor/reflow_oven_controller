/*!
 *******************************************************************************
 * @file state_machine.c
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

#include <stdint.h>
#include <stdbool.h>
#include "esp_log.h"
#include "states/state_machine_states.h"
#include "state_machine.h"
#include "state_machine_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define TAG                     __FILENAME__

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

static size_t const m_state_map_size = sizeof(m_state_map) /
                                       sizeof(m_state_map[0]);

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

extern void state_machine_task(void * pvParameter);

/*
 *******************************************************************************
 * Static Data Declarations                                                    *
 *******************************************************************************
 */

static bool m_is_initialized = false;

static QueueHandle_t m_state_machine_event_q = NULL;

static TaskHandle_t m_state_machine_task_h = NULL;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

bool state_machine_init(void) {

        bool success = !m_is_initialized;
        BaseType_t result = pdPASS;

        if (success) {
                result = xTaskCreate(state_machine_task,
                                     "state_machine_task",
                                     configMINIMAL_STACK_SIZE * 3,
                                     NULL,
                                     10,
                                     &m_state_machine_task_h);

                if ((pdPASS != result) || (NULL == m_state_machine_task_h)) {
                        success = false;
                }
        }

        if (success) {
                m_state_machine_event_q = xQueueCreate(
                                          10U, sizeof(state_machine_event_t *));

                if (NULL == m_state_machine_event_q) {
                        success = false;
                }
        }

        if (success) {
                state_machine_states_set_entry_point_state();
        }

        if (success) {
                m_is_initialized = true;
                xTaskNotify(m_state_machine_task_h, 0, eNoAction);
        }

        return success;

}

bool state_machine_wait_for_event(uint32_t const time_ms,
                                  state_machine_event_t * const p_event)
{
        BaseType_t result = pdPASS;
        bool success = true;
        state_machine_event_t * p_event_buffer = NULL;

        if ((NULL == p_event) || (NULL == m_state_machine_event_q)) {
                success = false;
        }

        if (success) {
                result = xQueueReceive(m_state_machine_event_q,
                                       (void *)&p_event_buffer,
                                       time_ms);

                success = ((pdPASS == result) && (NULL != p_event_buffer));
        }

        if (success) {
                ESP_LOGI(TAG, "Got event %d", p_event_buffer->data.message);
                *p_event = *p_event_buffer;

                vPortFree(p_event_buffer);
                p_event_buffer = NULL;
        }

        return success;
}

bool state_machine_send_event(state_machine_event_type_t const type,
                              state_machine_data_t const data,
                              uint32_t const timeout)
{
        size_t const event_size = sizeof(state_machine_event_t);
        state_machine_event_t * p_event = NULL;
        BaseType_t result = pdPASS;
        bool success = true;

        if (STATE_MACHINE_EVENT_TYPE_COUNT < type) {
                //TODO: verify data?
                success = NULL;
        }

        p_event = pvPortMalloc(event_size);

        if (NULL == p_event) {
                success = false;
        } else {
                p_event->type = type;
                p_event->time_received = pdTICKS_TO_MS(xTaskGetTickCount());

                switch (p_event->type) {
                case STATE_MACHINE_EVENT_TYPE_ACTION:
                        p_event->data.user_action = data.user_action;
                        break;
                case STATE_MACHINE_EVENT_TYPE_MESSAGE:
                        p_event->data.message = data.message;
                        break;
                default:
                        assert(0);
                        success = false;
                        break;
                }
        }

        if (success) {
                result = xQueueSend(m_state_machine_event_q, &p_event, timeout);
                success = (pdPASS == result);
        }

        return success;
}

state_machine_msg_t state_machine_get_timeout_msg(
                state_machine_state_text_t const state)
{
        state_machine_msg_t message = STATE_MACHINE_MSG_COUNT;
        bool found = false;
        size_t i;

        for (i = 0; ((m_state_map_size > i) && (!found)); i++) {
                if (state == m_state_map[i].text) {
                        message = m_state_map[i].timeout_msg;
                        found = true;
                }
        }

        return message;
}

state_machine_state_text_t state_machine_pointer_to_text(
                state_machine_state_t const state)
{
        bool found = false;
        state_machine_state_text_t text = STATE_MACHINE_STATE_COUNT;
        size_t i;

        for (i = 0; ((STATE_MACHINE_STATE_COUNT > i) && (!found)); i++) {

                if (state == m_state_map[i].function) {
                        text = m_state_map[i].text;
                        found = true;
                }
        }

        return text;
}

char * state_machine_get_state_string(state_machine_state_text_t const state)
{
        size_t i;
        bool found = false;
        char * p_string = NULL;

        for (i = 0; m_state_map_size > i && (!found); i++) {
                if (state == m_state_map[i].text) {
                        p_string = m_state_map[i].string;
                        found = true;
                }
        }

        return p_string;
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
