/*!
 *******************************************************************************
 * @file queue.c
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
#include <stdio.h>
#include <strings.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define debug printf

#define QUEUE_SPY_QUEUE_LENGTH          (100)
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

static bool m_is_queue_full = false;

static UBaseType_t m_queue_item_size = 0;

static uint8_t m_queue[QUEUE_SPY_QUEUE_LENGTH];

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength,
                                   const UBaseType_t uxItemSize,
                                   const uint8_t ucQueueType )
{
        // FIXME: size of queue_handle_t is not int
        QueueHandle_t handle = NULL;

        if (0 == uxItemSize) {
                handle = NULL;
        } else {
                m_queue_item_size = uxItemSize;
                handle = (QueueHandle_t)malloc(sizeof(int));
        }

        return handle;
}

void queue_spy_queue_data(const void * p_item_to_queue)
{
        debug("saving -> %p, contents -> %p\n", p_item_to_queue, *(void**)p_item_to_queue);
        memcpy((void *)m_queue, (void const *)p_item_to_queue, m_queue_item_size);
}

BaseType_t xQueueGenericSend( QueueHandle_t xQueue,
                              const void * const pvItemToQueue,
                              TickType_t xTicksToWait,
                              const BaseType_t xCopyPosition )
{
        bool success = pdTRUE;

        if ((NULL == xQueue) || (NULL == pvItemToQueue) || (m_is_queue_full)) {
                success = pdFALSE;
        }

        queue_spy_queue_data(pvItemToQueue);

        return success;
}

void vQueueDelete( QueueHandle_t xQueue )
{

}

BaseType_t xQueueReceive( QueueHandle_t xQueue,
                          void * const pvBuffer,
                          TickType_t xTicksToWait )
{
        BaseType_t success = pdTRUE;

        if (NULL == pvBuffer) {
                success = pdFALSE;
        } else {

                debug("copy from -> %p, contents -> %p\n", m_queue, *(void**)m_queue);
                debug("copy to -> %p, contents -> %p\n", pvBuffer, *(void**)pvBuffer);

                memcpy((void *) pvBuffer, (void const *) m_queue, m_queue_item_size);
        }

        return success;
}

void queue_spy_set_queue_full(bool is_full)
{
        m_is_queue_full = is_full;
}

void queue_spy_create(void)
{
        memset((void *)m_queue, 0, QUEUE_SPY_QUEUE_LENGTH);
        m_is_queue_full = false;
}

void queue_spy_destroy(void)
{
        memset((void *)m_queue, 0, QUEUE_SPY_QUEUE_LENGTH);
        m_is_queue_full = false;
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
