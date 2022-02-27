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

/*!
 * @brief Executes task loop only once if being on a testing compilation, or
 *        infinitely if is the normal production compilation
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

//! @brief Task for the heater module
static void heater_task(void * pvParameters);

//! @brief Power on the heater at hardware level
static void heater_power_on(void);

//! @brief Power off the heater at hardware level
static void heater_power_off(void);

//! @brief Send a message to the heater task
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

//! @brief Whether module is initialized or not
static bool m_is_initialized = false;

//! @brief Whether heater control is activeor not
static bool m_heater_running = false;

//! @brief Current heater target in degrees celsius
static int16_t m_heater_target = 0;

//! @brief Maximum allowed heater target in degrees celsius
static int16_t m_target_max_degrees = 270;

//! @brief Minimum allowed heater target in degrees celsius
static int16_t m_target_min_degrees = 0;

//! @brief Heater task handle
static TaskHandle_t heater_task_h = NULL;

//! @brief Heater queue handle
static xQueueHandle m_heater_queue_h = NULL;

//! @brief Temperature getter function pointer
static heater_temp_getter_t m_pf_temperature_getter = NULL;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

/*!
 * @brief Initialize heater module
 *
 * @param[in]           p_f_temp_getter     Pointer to a heater_temperature_getter_t
 *                                          function to retrieve the current
 *                                          temperature
 *
 * @return              heater_error_t      Operation result
 * @retval              HEATER_ERROR_SUCCESS
 *                                          Everything went well
 * @retval              HEATER_ERROR_GENERAL_ERROR
 *                                          Problem creating queue or task
 * @retval              HEATER_ERROR_BAD_PARAMETER
 *                                          Null function pointer passed
 */
heater_error_t heater_init(heater_temp_getter_t const p_f_temp_getter)
{
        heater_error_t success = HEATER_ERROR_SUCCESS;
        gpio_config_t io_config = {};
        esp_err_t esp_success;
        int result;

        //TODO configure hardware!

        if (m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else if (NULL == p_f_temp_getter) {
                success = HEATER_ERROR_BAD_PARAMETER;
        } else {
                m_heater_queue_h = xQueueCreate(3, sizeof(heater_msg_t *));

                if (NULL == m_heater_queue_h) {
                        success = HEATER_ERROR_GENERAL_ERROR;
                }
        }

        if (HEATER_ERROR_SUCCESS == success) {
                io_config.mode = GPIO_MODE_OUTPUT;
                io_config.pull_down_en = GPIO_PULLDOWN_ENABLE;
                io_config.pin_bit_mask = HEATER_ACTIVE_HIGH_GPIO_PIN;

                esp_success = gpio_config(&io_config);

                if (ESP_OK != esp_success) {
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

/*!
 * @brief Set heater target temperature
 *
 * @param[in]           degrees             Target temperature to set the heater
 *                                          to. Degrees celsius
 *
 * @return              heater_error_t      Result of the operation
 * @retval              HEATER_ERROR_SUCCESS
 *                                          Everything when well
 * @retval              HEATER_ERROR_NOT_INITIALIZED
 *                                          Module was not initialized
 * @retval              HEATER_ERROR_BAD_PARAMETER
 *                                          Target was outside the allowed limits
 */
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

/*!
 * @brief Get actual heater target temperature
 *
 * @param[out]          p_degrees           Pointer to retrieve the target
 *                                          temperature in degrees celsius
 *
 * @return              heater_error_t      Result of the operation
 * @retval              HEATER_ERROR_SUCCESS
 *                                          Everything when well
 * @retval              HEATER_ERROR_NOT_INITIALIZED
 *                                          Module was not initialized
 * @retval              HEATER_ERROR_BAD_PARAMETER
 *                                          Pointer was null
 */
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

/*!
 * @brief Start the heater control
 *
 * This function starts the heater control using the previously specified target
 *
 * @param               -                   -
 *
 * @return              heater_error_t      Result of the operation
 * @retval              HEATER_ERROR_SUCCESS
 *                                          Everything went well
 * @retval              HEATER_ERROR_NOT_INITIALIZED
 *                                          Module was not initialized
 * @retval              *                   Any other error
 */
heater_error_t heater_start(void)
{
        heater_error_t success;
        heater_msg_t message;

        if (!m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else {

                message.target = m_heater_target;
                message.heater_control_active = true;

                success = heater_send_msg(message);
        }

        return success;
}

/*!
 * @brief Stop the heater control
 *
 * This function stops the heater control and disables its power
 *
 * @param               -                   -
 *
 * @return              heater_error_t      Result of the operation
 * @retval              HEATER_ERROR_SUCCESS
 *                                          Everything went well
 * @retval              HEATER_ERROR_NOT_INITIALIZED
 *                                          Module was not initialized
 * @retval              *                   Any other error
 */
 heater_error_t heater_stop(void)
{
        heater_error_t success;
        heater_msg_t message;

        if (!m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else {
                message.target = m_heater_target;
                message.heater_control_active = false;

                success = heater_send_msg(message);
        }

        return success;
}

/*!
 * @brief Deinitialize the heater module
 *
 * This function will set the target and other variables to the default value,
 * and delete both the RTOS queue and the task. This function is mostly intended
 * to be used for unit testing.
 *
 * @param               -                   -
 *
 * @return              heater_error_t      Result of the operation
 * @retval              HEATER_ERROR_SUCCESS
 *                                          Everything went well
 * @retval              HEATER_ERROR_NOT_INITIALIZED
 *                                          Module was not initialized, so it
 *                                          couldn't be deinitialized.
 */
heater_error_t heater_deinit(void)
{
        heater_error_t success = HEATER_ERROR_SUCCESS;

        if (!m_is_initialized) {
                success = HEATER_ERROR_NOT_INITIALIZED;
        } else {
                m_heater_target = 0;
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

/*!
 * @brief Query whether the heater is running
 *
 * @note This doesn't means whether the heater is physically on or off, but
 *       whether the heater task is trying to adjust the temperature to the
 *       target value
 *
 * @param               -                   -
 *
 * @return              bool                Result of the query
 */
bool heater_is_running(void)
{
        return m_heater_running;
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

/*!
 * @brief Power on the heater at hardware level
 *
 * @param               -                   -
 *
 * @result              -                   -
 */
static void heater_power_on(void)
{
        (void)gpio_set_level(HEATER_ACTIVE_HIGH_GPIO_PIN, 1);
}

/*!
 * @brief Power off the heater at hardware level
 *
 * @param               -                   -
 *
 * @result              -                   -
 */
static void heater_power_off(void)
{
        (void)gpio_set_level(HEATER_ACTIVE_HIGH_GPIO_PIN, 0);
}

/*!
 * @brief Send a message to the heater task
 *
 * @param[out]          message             Message to be sent
 *
 * @return              heater_error_t      Result of the operation
 * @retval              HEATER_ERROR_SUCCESS
 *                                          Everything went well
 * @retval              HEATER_ERROR_OOM    There is not enough memory to allo-
 *                                          cate the message
 * @retval              HEATER_ERROR_GENERAL_ERROR
 *                                          Message couldn't be sent, most
 *                                          likely a full queue
 */
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
                p_message->heater_control_active = message.heater_control_active;
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

/*!
 * @brief Task for the heater module
 *
 * @param               pvParameters        Not used
 *
 * @result              -                   -
 */
static void heater_task(void * pvParameters)
{
        heater_msg_t * p_in_message = NULL;
        int16_t temperature = 0;
        BaseType_t result;
        bool success;

        (void)pvParameters;

        do {
                result = xQueueReceive(m_heater_queue_h,
                                       &p_in_message,
                                       pdMS_TO_TICKS(100));

                if ((pdTRUE == result) && (NULL != p_in_message)) {
                        m_heater_running = p_in_message->heater_control_active;
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