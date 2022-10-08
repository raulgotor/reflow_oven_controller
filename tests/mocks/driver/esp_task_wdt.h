/*!
 *******************************************************************************
 * @file esp_task_wdt.h
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 28.03.22
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */

#ifndef ESP_TASK_WDT_H
#define ESP_TASK_WDT_H

/*
 *******************************************************************************
 * Public Macros                                                               *
 *******************************************************************************
 */

#define ESP_OK 0

/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

typedef int esp_err_t;

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

esp_err_t esp_task_wdt_init(uint32_t time, bool panic);

esp_err_t esp_task_wdt_reset(void);

esp_err_t esp_task_wdt_add(TaskHandle_t handle);

#endif //ESP_TASK_WDT_H