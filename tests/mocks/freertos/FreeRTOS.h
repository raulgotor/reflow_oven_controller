/*!
 *******************************************************************************
 * @file FreeRTOS.h
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

#ifndef FREERTOS_H
#define FREERTOS_H

/*
 *******************************************************************************
 * Public Macros                                                               *
 *******************************************************************************
 */
#include <stdlib.h>

/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

/**
 * task. h
 *
 * Type by which tasks are referenced.  For example, a call to xTaskCreate
 * returns (via a pointer parameter) an TaskHandle_t variable that can then
 * be used as a parameter to vTaskDelete to delete the task.
 *
 * @cond
 * \defgroup TaskHandle_t TaskHandle_t
 * @endcond
 * \ingroup Tasks
 */
struct tskTaskControlBlock;     /* The old naming convention is used to prevent breaking kernel aware debuggers. */
#ifdef ESP_PLATFORM // IDF-3769
typedef void* TaskHandle_t;
#else
typedef struct tskTaskControlBlock* TaskHandle_t;
#endif // ESP_PLATFORM

/**
 * Type by which queues are referenced.  For example, a call to xQueueCreate()
 * returns an QueueHandle_t variable that can then be used as a parameter to
 * xQueueSend(), xQueueReceive(), etc.
 */
struct QueueDefinition; /* Using old naming convention so as not to break kernel aware debuggers. */
typedef struct QueueDefinition   * QueueHandle_t;

#define queueQUEUE_TYPE_BASE                  ( ( uint8_t ) 0U )

#define xQueueHandle                  QueueHandle_t
#define xQueueCreate( uxQueueLength, uxItemSize )    xQueueGenericCreate( ( uxQueueLength ), ( uxItemSize ), ( queueQUEUE_TYPE_BASE ) )

#define configMINIMAL_STACK_SIZE 1
#define pdPASS 1
#define pdFALSE 0

#define pdTRUE true
#define pdFALSE false

typedef unsigned int UBaseType_t;
typedef unsigned int BaseType_t;
typedef void (* TaskFunction_t)( void * );


typedef uint32_t TickType_t;

#define vPortFree free

#define pvPortMalloc malloc


#define configTICK_RATE_HZ 1000
#define pdMS_TO_TICKS( xTimeInMs )    ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000U ) )


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

#endif //FREERTOS_H