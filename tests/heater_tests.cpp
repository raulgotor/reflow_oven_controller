/*!
 *******************************************************************************
 * @file heater_tests.cpp
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

#define NDEBUG

/*
 *******************************************************************************
 * #include Statements                                                         *
 *******************************************************************************
 */
#define debug printf

#include "CppUTest/TestHarness.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio_spy.h"

#include "thermocouple.h"
#include "thermocouple_fake.h"
#include "heater.h"

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

static int16_t const m_valid_target_degrees = 200;

static int16_t const m_oor_high_target_degrees = 270 + 1;

static int16_t const m_oor_low_target_degrees = 0 - 1;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

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

TEST_GROUP(heater_no_init)
{
        void setup()
        {
                gpio_spy_init();
                (void)heater_deinit();
        }

        void teardown()
        {
                gpio_spy_deinit();
        }
};

TEST(heater_no_init, set_target_no_init_fails)
{
        heater_error_t result;

        result = heater_set_target(m_valid_target_degrees);

        ENUMS_EQUAL_INT(HEATER_ERROR_NOT_INITIALIZED, result);
}

TEST(heater_no_init, get_target_no_init_fails)
{
        heater_error_t result;
        int16_t buffer;

        result = heater_get_target(&buffer);

        ENUMS_EQUAL_INT(HEATER_ERROR_NOT_INITIALIZED, result);
}


TEST(heater_no_init, start_no_init_fails)
{
        heater_error_t result;

        result = heater_start();

        ENUMS_EQUAL_INT(HEATER_ERROR_NOT_INITIALIZED, result);
}

TEST(heater_no_init, stop_no_init_fails)
{
        heater_error_t result;

        result = heater_stop();

        ENUMS_EQUAL_INT(HEATER_ERROR_NOT_INITIALIZED, result);
}

TEST(heater_no_init, deinit_no_init_fails)
{
        heater_error_t result;

        result = heater_deinit();

        ENUMS_EQUAL_INT(HEATER_ERROR_NOT_INITIALIZED, result);
}

TEST(heater_no_init, init_invalid_temperature_reader_fp_fails)
{
        heater_error_t result;

        result = heater_init(NULL);

        ENUMS_EQUAL_INT(HEATER_ERROR_BAD_PARAMETER, result);
}

/*!
 * @test Init the heater without starting it
 *
 * @result - Function should succeed
 *         - Heater controller should be low
 *         - Heater should be marked as not running
 *         - Heater target should be 0
 */
TEST(heater_no_init, init_succeeds)
{
        heater_error_t result;
        uint32_t level;
        int16_t target;

        (void)gpio_spy_get_pin_level(
                        (gpio_num_t)HEATER_ACTIVE_HIGH_GPIO_PIN, &level);

        result = heater_init((heater_temp_getter_t)thermocouple_fake_get_temperature);

        (void)gpio_spy_get_pin_level(
                        (gpio_num_t)HEATER_ACTIVE_HIGH_GPIO_PIN, &level);

        (void)heater_get_target(&target);

        LONGS_EQUAL(0, level);
        CHECK(!heater_is_running());
        LONGS_EQUAL(0, target);
        ENUMS_EQUAL_INT(HEATER_ERROR_SUCCESS, result);

        (void)heater_deinit();

}

TEST_GROUP(heater_initialized_no_deinit)
{
        void setup() {
                (void)heater_init((heater_temp_getter_t)thermocouple_fake_get_temperature);
        }

        void teardown() {
                (void)heater_deinit();
        }
};

TEST(heater_initialized_no_deinit, deinit_succeeds)
{
        heater_error_t result;

        result = heater_deinit();

        ENUMS_EQUAL_INT(HEATER_ERROR_SUCCESS, result);
}

TEST_GROUP(heater_initialized)
{
        void setup() {
                gpio_spy_init();
                queue_spy_create();
                (void)heater_init((heater_temp_getter_t)thermocouple_fake_get_temperature);
        }

        void teardown() {
                ENUMS_EQUAL_INT(HEATER_ERROR_SUCCESS, heater_deinit());
                queue_spy_destroy();
                gpio_spy_deinit();
        }

        void check_heater_with(int16_t const target,
                               int16_t const temperature,
                               uint32_t const expected_level,
                               bool const expected_heater_running)
        {
                heater_error_t success;
                TaskFunction_t task_function;
                int16_t target_temperature = 0;
                uint32_t level = 0;

                task_spy_get_task_function(&task_function);

                thermocouple_fake_set_temperature(
                                THERMOCOUPLE_ID_0,
                                temperature);

                success = heater_set_target(target);

                if (HEATER_ERROR_SUCCESS == success) {
                        success = heater_start();
                }

                if (HEATER_ERROR_SUCCESS == success) {
                        // force a run through the task loop
                        task_function(NULL);

                        gpio_spy_get_pin_level(
                                        (gpio_num_t)HEATER_ACTIVE_HIGH_GPIO_PIN,
                                        &level);

                        success = heater_get_target(&target_temperature);
                }

                ENUMS_EQUAL_INT(HEATER_ERROR_SUCCESS, success);
                CHECK(expected_heater_running == heater_is_running());
                LONGS_EQUAL(m_valid_target_degrees, target_temperature);
                LONGS_EQUAL(expected_level, level);
        }
};

TEST(heater_initialized, get_target_null_param_fails)
{
        heater_error_t result;

        result = heater_get_target(NULL);

        ENUMS_EQUAL_INT(HEATER_ERROR_BAD_PARAMETER, result);
}

TEST(heater_initialized, set_target_valid_succeeds)
{
        heater_error_t result;
        int16_t retrieved_target_degrees;

        result = heater_set_target(m_valid_target_degrees);

        (void)heater_get_target(&retrieved_target_degrees);

        ENUMS_EQUAL_INT(HEATER_ERROR_SUCCESS, result);
        LONGS_EQUAL(m_valid_target_degrees, retrieved_target_degrees);
        CHECK(!heater_is_running());
}

TEST(heater_initialized, start_succeeds)
{
        heater_error_t result;

        result = heater_start();

        ENUMS_EQUAL_INT(HEATER_ERROR_SUCCESS, result);
}

TEST(heater_initialized, start_msg_valid_succeeds)
{
        heater_msg_t * p_msg = NULL;
        heater_error_t result;

        (void)heater_set_target(m_valid_target_degrees);
        result = heater_start();

        (void)xQueueReceive(0, &p_msg, 0);

        ENUMS_EQUAL_INT(HEATER_ERROR_SUCCESS, result);
        CHECK(NULL != p_msg);
        LONGS_EQUAL(m_valid_target_degrees, p_msg->target);
        LONGS_EQUAL(true, p_msg->heater_control_active);
}

/*!
 * @test Setting target to OOR temperature values
 *
 * @result `heater_set_target` errors out.
 */
TEST(heater_initialized, set_target_oor_fails)
{
        heater_error_t result_oor_low;
        heater_error_t result_oor_high;

        result_oor_low = heater_set_target(m_oor_low_target_degrees);

        ENUMS_EQUAL_INT(HEATER_ERROR_BAD_PARAMETER, result_oor_low);

        result_oor_high = heater_set_target(m_oor_high_target_degrees);

        ENUMS_EQUAL_INT(HEATER_ERROR_BAD_PARAMETER, result_oor_high);

}

/*!
 * @test Set a specific target, start the heater, and then re-start the heater
 *       but simulate a full queue so this last message fails.
 *
 * @result - `heater_start` should error out with `HEATER_ERROR_GENERAL_ERROR`,
 *         - heater pin should turn off,
 *         - and message memory should be freed.
 */
TEST(heater_initialized, start_msg_queue_error_free_memory_and_failss)
{
        heater_msg_t * p_msg;

        heater_error_t result;
        uint32_t pin_state;

        queue_spy_set_queue_full(false);
        (void)heater_set_target(m_valid_target_degrees);
        (void)heater_start();

        queue_spy_set_queue_full(true);
        result = heater_start();

        gpio_spy_get_pin_level(
                        (gpio_num_t )HEATER_ACTIVE_HIGH_GPIO_PIN, &pin_state);

        ENUMS_EQUAL_INT(HEATER_ERROR_GENERAL_ERROR, result);
        LONGS_EQUAL(0, pin_state);

        (void)xQueueReceive(0, &p_msg, 0);

        // TODO: tracking system for checking whether the variable was freed or not
        // i.e: spy on malloc / free + array or linked list
        // CHECK(NULL == p_msg);
}

/*!
 * @test Stopping heater
 *
 * @result - Successful result
 *         - Heater controller driven low
 */
TEST(heater_initialized, stop_succeeds)
{
        heater_msg_t * p_msg = NULL;
        heater_error_t success;
        uint32_t pin_level;

        success = heater_stop();

        (void)gpio_spy_get_pin_level(
                        (gpio_num_t)HEATER_ACTIVE_HIGH_GPIO_PIN, &pin_level);

        (void)xQueueReceive(0, &p_msg, 0);

        ENUMS_EQUAL_INT(HEATER_ERROR_SUCCESS, success);
        LONGS_EQUAL(0, pin_level);
        CHECK(NULL != p_msg);
        LONGS_EQUAL(false, p_msg->heater_control_active);
        LONGS_EQUAL(0, p_msg->target);
}

/*!
 * @test Stopping heater after starting heater
 *
 * @result - Successful result
 *         - Heater controller driven low
 */
TEST(heater_initialized, stop_after_start_succeeds)
{
        heater_msg_t * p_msg = NULL;
        heater_error_t success;
        uint32_t pin_level;

        (void)heater_start();
        success = heater_stop();

        (void)gpio_spy_get_pin_level(
                        (gpio_num_t)HEATER_ACTIVE_HIGH_GPIO_PIN, &pin_level);

        (void)xQueueReceive(0, &p_msg, 0);

        ENUMS_EQUAL_INT(HEATER_ERROR_SUCCESS, success);
        LONGS_EQUAL(0, pin_level);
        CHECK(NULL != p_msg);
        LONGS_EQUAL(false, p_msg->heater_control_active);
        LONGS_EQUAL(0, p_msg->target);
}

/*!
 * @test Set a specific target, start the heater, and then stop the heater
 *       but simulate a full queue so this last message fails.
 *
 * @result - `heater_stop` should error out with `HEATER_ERROR_GENERAL_ERROR`,
 *         - heater pin should turn off,
 *         - message memory should be freed.
 */
TEST(heater_initialized, stop_msg_queue_error_stop_heater_free_memory_and_fails)
{
        heater_msg_t * p_msg;
        heater_error_t result;
        uint32_t pin_state;

        queue_spy_set_queue_full(false);
        (void)heater_set_target(m_valid_target_degrees);
        (void)heater_start();
        queue_spy_set_queue_full(true);
        result = heater_stop();

        gpio_spy_get_pin_level(
                        (gpio_num_t )HEATER_ACTIVE_HIGH_GPIO_PIN, &pin_state);

        ENUMS_EQUAL_INT(HEATER_ERROR_GENERAL_ERROR, result);
        LONGS_EQUAL(0, pin_state);

        (void)xQueueReceive(0, &p_msg, 0);

        // TODO: tracking system for checking whether the variable was freed or not
        // i.e: spy on malloc / free + array or linked list
        //  CHECK(NULL == msg);
}

/*!
 * @test Configure a valid target and start the heater with a fake temperature
 *       lower than the target
 *
 * @result - Starting heater succeeds
 *         - Heater is running
 *         - Heater controller gpio is high
 *         - Target temperature is the right one
 */
TEST(heater_initialized, fake_temperature_lower_than_target_heats)
{
        check_heater_with(m_valid_target_degrees,
                          m_valid_target_degrees - 20,
                          1,
                          true);
}

/*!
 * @test Configure a valid target and start the heater with a fake temperature
 *       higher than the target
 *
 * @result - Starting heater succeeds
 *         - Heater is running
 *         - Heater controller gpio is low
 *         - Target temperature is the right one
 */
TEST(heater_initialized, fake_temperature_higher_than_target_stops_heating)
{
        check_heater_with(m_valid_target_degrees,
                          m_valid_target_degrees + 20,
                          0,
                          true);
}

/*!
 * @test Configure a valid target and start the heater with a fake temperature
 *       lower than the target, then next cycle, pass the target
 *
 * @result - Starting heater succeeds
 *         - Heater is running until target temperature is reached
 *         - Heater controller gpio is high until target temperature is reached
 *         - Target temperature is the right one
 */
TEST(heater_initialized, stops_heating_when_reaching_target_temperature)
{
        check_heater_with(m_valid_target_degrees,
                          m_valid_target_degrees - 20,
                          1,
                          true);

        check_heater_with(m_valid_target_degrees,
                          m_valid_target_degrees + 20,
                          0,
                          true);
}