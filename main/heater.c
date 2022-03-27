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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "wdt.h"
#include "thermocouple.h"
#include "heater.h"
#include "panic.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define HEATER_ACTIVE_HIGH_GPIO_PIN              (32)

/*
 *******************************************************************************
 * Data types                                                                  *
 *******************************************************************************
 */

typedef struct {
        uint16_t target;
        bool heater_running;
} heater_msg_t;

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

static uint16_t m_target_temperature = 0;

static uint16_t m_target_max_degrees = 300;

static TaskHandle_t heater_task_h = NULL;

static xQueueHandle heater_queue_h = NULL;

static bool m_heater_running = false;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

heater_error_t heater_init(void)
{
        heater_error_t result = HEATER_ERROR_SUCCESS;
        BaseType_t task_result;
        bool success;

        if (m_is_initialized) {
                result = HEATER_ERROR_GENERAL_ERROR;
        } else {
                m_target_temperature = 0;
                m_is_initialized = true;

                heater_queue_h = xQueueCreate(3, sizeof(heater_msg_t));

                if (NULL == heater_queue_h) {
                        result = HEATER_ERROR_GENERAL_ERROR;
                }
        }

        if (HEATER_ERROR_SUCCESS == result) {

                task_result = xTaskCreate(
                                heater_task,
                                "heater_task",
                                configMINIMAL_STACK_SIZE * 4,
                                NULL,
                                1,
                                &heater_task_h);

                if (pdPASS != task_result) {
                        result = HEATER_ERROR_GENERAL_ERROR;
                }
        }

        if (HEATER_ERROR_SUCCESS == result) {
                success = wdt_add_task(heater_task_h);

                if (!success) {
                        result = HEATER_ERROR_GENERAL_ERROR;
                }
        }

        return result;
}

heater_error_t heater_set_target(uint16_t const degrees)
{
        heater_error_t success = HEATER_ERROR_SUCCESS;

        if (!m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else if (m_target_max_degrees < degrees) {
                success = HEATER_ERROR_BAD_PARAMETER;
        }
        
        if (HEATER_ERROR_SUCCESS == success) {
                m_target_temperature = degrees;
        }
        
        return success;
}

heater_error_t heater_get_target(uint16_t * const p_degrees)
{
        heater_error_t success = HEATER_ERROR_SUCCESS;
        
        if (!m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else if (NULL == p_degrees) {
                success = HEATER_ERROR_BAD_PARAMETER;
        } else {

                *p_degrees = m_target_temperature;
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

                message.target = m_target_temperature;
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
                message.target = m_target_temperature;
                message.heater_running = false;

                success = heater_send_msg(message);
        }

        return success;
}

void heater_emergency_stop(void)
{
        m_heater_running = false;
        heater_power_off();
}

heater_error_t heater_deinit(void)
{
        heater_error_t success = HEATER_ERROR_SUCCESS;

        if (!m_is_initialized) {
                success = HEATER_ERROR_GENERAL_ERROR;
        } else {
                m_target_temperature = 0;
                heater_power_off();
                vQueueDelete(heater_queue_h);
                heater_queue_h = NULL;
                vTaskDelete(heater_task_h);
                heater_task_h = NULL;
                m_is_initialized = false;
        }

        return success;
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

        p_message = pvPortMalloc(sizeof(heater_msg_t *));

        if (NULL == p_message) {
                success = HEATER_ERROR_OOM;
        }

        if (HEATER_ERROR_SUCCESS == success) {
                p_message->heater_running = message.heater_running;
                p_message->target = message.target;

                result = xQueueSend(heater_queue_h, &p_message, 0);

                if (pdPASS != result) {
                        success = HEATER_ERROR_GENERAL_ERROR;

                        // Emergency shut down
                        heater_power_off();

                        vPortFree(p_message);
                        p_message = NULL;
                }
        }

        return success;
}


/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

void heater_task(void * pvParameters)
{
        heater_msg_t * p_in_message = NULL;
        int16_t temperature;
        BaseType_t result;
        bool success = true;

        for (;;) {
                result = xQueueReceive(heater_queue_h, &p_in_message, pdMS_TO_TICKS(100));

                if ((pdTRUE == result) && (NULL != p_in_message)) {
                        m_heater_running = p_in_message->heater_running;
                        m_target_temperature = p_in_message->target;

                        if (!m_heater_running) {
                                heater_power_off();
                        }

                        vPortFree(p_in_message);
                        p_in_message = NULL;
                }

                if (m_heater_running) {
                        success = thermocouple_get_temperature(
                                        THERMOCOUPLE_ID_0,
                                        &temperature);

                        if (success) {
                                if (m_target_temperature <= temperature) {
                                        heater_power_off();
                                } else {
                                        heater_power_on();
                                }
                        }
                }

                if ((!success) || (!wdt_kick())) {
                        // Code style exception for readability
                        break;
                }
        }

        panic("General failure at heater_task ", __FILENAME__, __LINE__);
}