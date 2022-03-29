/*!
 *******************************************************************************
 * @file task.c
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 17.02.22
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

static TaskHandle_t m_singe_handle ;

static TaskFunction_t m_task_function = NULL;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

 BaseType_t xTaskCreate(
                TaskFunction_t pvTaskCode,
                const char * const pcName,
                const uint32_t usStackDepth,
                void * const pvParameters,
                UBaseType_t uxPriority,
                TaskHandle_t * const pvCreatedTask)
{
         bool success = pdPASS;

         if (NULL == pvCreatedTask || NULL == pvTaskCode) {
                 success = pdFALSE;
         }

         if (pdPASS == success) {
                 // FIXME: size of handle is not int
                 *pvCreatedTask = malloc(sizeof(int));

                 success = (NULL != *pvCreatedTask);
         }

        if (pdPASS == success) {
                m_task_function = pvTaskCode;
                pvTaskCode(NULL);
        }

         return success;
}

void vTaskDelete( TaskHandle_t xTaskToDelete )
{
        free(xTaskToDelete);
}

void task_spy_get_task_function(TaskFunction_t * p_task_function)
{
         *p_task_function = m_task_function;
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
