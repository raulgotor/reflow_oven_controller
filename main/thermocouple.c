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

#include <stdbool.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
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
        //m_temperature[0] = hal_thermocouple_get_temperature(0);
        //m_temperature[1] = hal_thermocouple_get_temperature(1);

        m_temperature[0]++;
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

void thremocouple_task(void * pvParameters)
{
        for (;;) {

                vTaskDelay(m_refresh_rate);

                thermocouple_update_temperature();
        }
}
