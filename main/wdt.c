/*!
 *******************************************************************************
 * @file wdt.c
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 26.03.22
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

#include <stdbool.h>
#include <stdint.h>
#include <FreeRTOS.h>
#include <esp_task_wdt.h>
#include "wdt.h"

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

/*!
 * @brief Initialize the WDT
 *
 * The WDT will cause the system to panic if not kicked in time
 *
 * @warning esp_task_wdt_init() must only be called after the scheduler started
 *
 * @param               timeout_s           Timeout in seconds for the the WDT
 *                                          to bark
 *
 * @return              bool                Operation result
 */
bool wdt_init(uint32_t const timeout_s)
{
        bool const panic = true;
        esp_err_t esp_result;

        esp_result = esp_task_wdt_init(timeout_s, panic);

        return (ESP_OK == esp_result);
}

bool wdt_kick(void)
{
        esp_err_t esp_result;

        esp_result = esp_task_wdt_reset();
        return (ESP_OK == esp_result);
}

bool wdt_add_task(TaskHandle_t const handle)
{
        esp_err_t esp_result;

        esp_result = esp_task_wdt_add(handle);

        return (ESP_OK == esp_result);
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
