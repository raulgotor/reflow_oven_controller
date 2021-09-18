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
#include "state_machine.h"
#include "state_machine_task.h"
#include "states/state_machine_states.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

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

QueueHandle_t m_state_machine_event_q = NULL;

/*
 *******************************************************************************
 * Static Data Declarations                                                    *
 *******************************************************************************
 */

static bool m_is_initialized = false;

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
                                     configMINIMAL_STACK_SIZE,
                                     NULL,
                                     1,
                                     NULL);

                if (pdPASS != result) {
                        success = false;
                }
        }

        if (success) {
                m_state_machine_event_q = xQueueCreate(10U, sizeof(int));

                if (NULL == m_state_machine_event_q) {
                        success = false;
                }
        }

        if (success) {
                state_machine_set_state(state_machine_state_idle);
        }
        if (success) {
                m_is_initialized = true;
        }

        return success;

}

bool state_machine_wait_for_event(uint32_t const time_ms,
                                  state_machine_event_t ** const pp_event)
{
        BaseType_t result = pdPASS;
        bool success = true;

        if ((NULL == pp_event) && (NULL == m_state_machine_event_q)) {
                success = false;
        }

        if (success) {
                result = xQueueReceive(m_state_machine_event_q,
                                       (void *)pp_event,
                                       time_ms);
        }

        if (success) {
                success = (pdPASS == result);
        }

        return success;
}

bool state_machine_send_event(state_machine_event_type_t const type,
                              state_machine_data_t const data,
                              uint32_t const timeout)
{
        size_t const event_size = sizeof(state_machine_event_t *);
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
                        p_event->data.action = data.action;
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
                result = xQueueSend(m_state_machine_event_q, p_event, timeout);
                success = (pdPASS == result);
        }

        return success;
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
