/*!
 *******************************************************************************
 * @file state_machine.cpp
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
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "states/state_machine_states.h"
#include "state_machine.h"

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

/*
 *******************************************************************************
 * Static Data Declarations                                                    *
 *******************************************************************************
 */

static state_machine_state_t m_pf_state = NULL;

static state_machine_state_text_t m_state_machine_state = STATE_MACHINE_STATE_COUNT;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

bool state_machine_get_state(state_machine_state_text_t * const p_state)
{

        bool success = (NULL != p_state);

        if (success) {
                *p_state = m_state_machine_state;
        }

        return success;
}

bool state_machine_set_state(state_machine_state_t const state)
{
        bool success = (NULL != state);
        // Avoid state_text might be used uninitialized warning
        state_machine_state_text_t state_text = STATE_MACHINE_STATE_COUNT;

        if (success) {
                state_text = state_machine_pointer_to_text(state);
                success = (STATE_MACHINE_STATE_COUNT != state_text);
        }

        if (success) {
                m_state_machine_state = state_text;
                m_pf_state = state;
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

void state_machine_task(void * pvParameter)
{
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

        for (;;) {
                m_pf_state();
                vTaskDelay(1);
        }
}
