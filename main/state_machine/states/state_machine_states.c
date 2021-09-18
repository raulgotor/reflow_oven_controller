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
#include "freertos/FreeRTOS.h"
#include "state_machine/state_machine.h"
#include "state_machine/state_machine_task.h"
#include "state_machine/states/state_machine_states.h"

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

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

void state_machine_state_idle(void)
{
        state_machine_event_t * p_event = NULL;
        bool success = true;

        success = state_machine_wait_for_event(portMAX_DELAY, &p_event);

        if ((success) && (NULL != p_event) &&
            (STATE_MACHINE_EVENT_TYPE_ACTION == p_event->type)) {

                switch (p_event->data.action) {
                case STATE_MACHINE_ACTION_START:
                        state_machine_set_state(state_machine_state_heating);

                        vPortFree(p_event);
                        p_event = NULL;

                        break;
                default:
                        break;
                }
        }


}

void state_machine_state_heating(void)
{

}

void state_machine_state_soak(void)
{

}

void state_machine_state_reflow(void)
{

}

void state_machine_state_cooling(void)
{

}

void state_machine_state_error(void)
{

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
