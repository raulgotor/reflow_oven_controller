/*!
 *******************************************************************************
 * @file queue.h
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

#ifndef QUEUE_H
#define QUEUE_H

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

/**
 * Generic version of the function used to create a queue using dynamic memory
 * allocation.  This is called by other functions and macros that create other
 * RTOS objects that use the queue structure as their base.
 */
QueueHandle_t xQueueGenericCreate(const UBaseType_t uxQueueLength,
                                  const UBaseType_t uxItemSize,
                                  const uint8_t ucQueueType);


BaseType_t xQueueGenericSend(QueueHandle_t xQueue,
                             const void * const pvItemToQueue,
                             TickType_t xTicksToWait,
                             const BaseType_t xCopyPosition);

void vQueueDelete(QueueHandle_t xQueue);

BaseType_t xQueueReceive(QueueHandle_t xQueue,
                         void * const pvBuffer,
                         TickType_t xTicksToWait);


#define xQueueSend( xQueue, pvItemToQueue, xTicksToWait ) \
    xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), 0 )

void queue_spy_set_queue_full(bool is_full);

void queue_spy_create(void);

void queue_spy_destroy(void);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif //QUEUE_H