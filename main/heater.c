/*!
 *******************************************************************************
 * @file heater.c
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 13.02.22
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */

/*
 *******************************************************************************
 * #include Statements                                                         *
 *******************************************************************************
 */
#define NDEBUG

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <printf.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "gpio_types.h"
#include "driver/gpio_spy.h"

#include "thermocouple.h"
#include "heater.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#if (defined(TEST_COMPILATION) && (1 == TEST_COMPILATION))
#define FOREVER 0
#else
#define FOREVER 1
#endif

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

static void heater_task(void * pvParameters);

static void heater_power_on(void);

static void heater_power_off(void);

static heater_error_t heater_send_msg(heater_msg_t const message);

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

static bool m_is_initialized = false;

static bool m_heater_running = false;

static int16_t m_heater_target = 0;

static int16_t m_target_max_degrees = 270;

static int16_t m_target_min_degrees = 0;

static TaskHandle_t heater_task_h = NULL;

static xQueueHandle m_heater_queue_h = NULL;

static heater_temp_getter_t m_pf_temperature_getter = NULL;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

heater_error_t heater_init(heater_temp_getter_t const p_f_temp_getter)
{
        heater_error_t success = HEATER_ERROR_SUCCESS;
        int result;

        //TODO configure hardware!

        if (m_is_initialized) {
                success = HEATER_ERROR_GENERAL_ERROR;
        } else if (NULL == p_f_temp_getter) {
                success = HEATER_ERROR_BAD_PARAMETER;
        } else {
                m_heater_queue_h = xQueueCreate(3, sizeof(heater_msg_t *));

                if (NULL == m_heater_queue_h) {
                        success = HEATER_ERROR_GENERAL_ERROR;
                }
        }

        if (HEATER_ERROR_SUCCESS == success) {

                m_heater_target = 0;
                m_heater_running = false;
                m_pf_temperature_getter = p_f_temp_getter;

                result = xTaskCreate(
                                heater_task,
                                "heater_task",
                                configMINIMAL_STACK_SIZE,
                                NULL,
                                1,
                                &heater_task_h);

                if (pdPASS != result) {
                        success = HEATER_ERROR_GENERAL_ERROR;
                }
        }

        if (HEATER_ERROR_SUCCESS == success) {

                m_is_initialized = true;
        }

        return success;
}

heater_error_t heater_set_target(int16_t const degrees)
{
        heater_error_t success = HEATER_ERROR_SUCCESS;

        if (!m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else if ((m_target_max_degrees < degrees) ||
                   (m_target_min_degrees > degrees)) {
                success = HEATER_ERROR_BAD_PARAMETER;
        }
        
        if (HEATER_ERROR_SUCCESS == success) {
                m_heater_target = degrees;
        }
        
        return success;
}

heater_error_t heater_get_target(int16_t * const p_degrees)
{
        heater_error_t success = HEATER_ERROR_SUCCESS;
        
        if (!m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else if (NULL == p_degrees) {
                success = HEATER_ERROR_BAD_PARAMETER;
        } else {
                *p_degrees = m_heater_target;
        }
        
        return success;
}

heater_error_t heater_start(void)
{
        heater_error_t success;
        heater_msg_t message;

        if (!m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else {

                message.target = m_heater_target;
                message.heater_running = true;

                success = heater_send_msg(message);
        }

        return success;
}

heater_error_t heater_stop(void)
{
        heater_error_t success;
        heater_msg_t message;

        if (!m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else {
                message.target = m_heater_target;
                message.heater_running = false;

                success = heater_send_msg(message);
        }

        return success;
}

heater_error_t heater_deinit(void)
{
        heater_error_t success = HEATER_ERROR_SUCCESS;

        if (!m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else {
                m_heater_target = 0;
                printf("Disabling heater\n");
                m_heater_running = false;
                m_is_initialized = false;
                if (NULL != m_heater_queue_h) {
                        vQueueDelete(m_heater_queue_h);
                }
                m_heater_queue_h = NULL;
                if (NULL != heater_task_h) {
                        vTaskDelete(heater_task_h);
                }
                heater_task_h = NULL;
        }

        return success;
}

bool heater_is_running(void)
{
        printf("Getting heater heater %d\n", m_heater_running);

        return m_heater_running;
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

static void heater_power_on(void)
{
        (void)gpio_set_level(HEATER_ACTIVE_HIGH_GPIO_PIN, 1);
}

static void heater_power_off(void)
{
        (void)gpio_set_level(HEATER_ACTIVE_HIGH_GPIO_PIN, 0);
}

static heater_error_t heater_send_msg(heater_msg_t const message)
{
        heater_error_t success = HEATER_ERROR_SUCCESS;
        heater_msg_t * p_message = NULL;
        BaseType_t result;

        p_message = pvPortMalloc(sizeof(heater_msg_t));

        if (NULL == p_message) {
                success = HEATER_ERROR_OOM;
        }

        if (HEATER_ERROR_SUCCESS == success) {
                p_message->heater_running = message.heater_running;
                p_message->target = message.target;

                result = xQueueSend(m_heater_queue_h, &p_message, 0);

                if (pdPASS != result) {
                        success = HEATER_ERROR_GENERAL_ERROR;

                        // Emergency shut down
                        heater_power_off();

                        vPortFree(p_message);
                        p_message = NULL;
                        assert(0);
                }
        }

        return success;
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

static void heater_task(void * pvParameters)
{
        heater_msg_t * p_in_message = NULL;
        int16_t temperature = 0;
        BaseType_t result;
        bool success;

        do {

                result = xQueueReceive(m_heater_queue_h, &p_in_message, pdMS_TO_TICKS(100));

                if ((pdTRUE == result) && (NULL != p_in_message)) {
                        m_heater_running = p_in_message->heater_running;
                        m_heater_target = p_in_message->target;

                        if (!m_heater_running) {
                                heater_power_off();
                        }

                        vPortFree(p_in_message);
                        p_in_message = NULL;
                }

                if (m_heater_running) {

                        success = m_pf_temperature_getter(
                                        THERMOCOUPLE_ID_0,
                                        &temperature);

                        if (!success) {
                                heater_power_off();
                                assert(0);
                        }

                        if (m_heater_target < temperature) {
                                heater_power_off();
                        } else {
                                heater_power_on();
                        }
                }

        // Will run forever in production, but only once in unit testing
        } while (FOREVER);

}