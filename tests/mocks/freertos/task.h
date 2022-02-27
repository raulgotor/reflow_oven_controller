/*!
 *******************************************************************************
 * @file task.h
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail..com)
 * @date 17.02.22
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */

#ifndef TASK_H
#define TASK_H

#ifdef __cplusplus
extern "C"
{
#endif // #ifdef __cplusplus

/*
 *******************************************************************************
 * Public Macros                                                               *
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Public Constants                                                            *
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Public Function Prototypes                                                  *
 *******************************************************************************
 */

BaseType_t xTaskCreate(
                TaskFunction_t pvTaskCode,
                const char * const pcName,
                const uint32_t usStackDepth,
                void * const pvParameters,
                UBaseType_t uxPriority,
                TaskHandle_t * const pvCreatedTask);

void vTaskDelete( TaskHandle_t xTaskToDelete );

void task_spy_get_task_function(TaskFunction_t * p_task_function);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif //TASK_H