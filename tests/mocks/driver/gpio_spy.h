/*!
 *******************************************************************************
 * @file gpio.h
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

#ifndef GPIO_SPY_H
#define GPIO_SPY_H

#ifdef __cplusplus
extern "C"
{
#endif // #ifdef __cplusplus

#include "driver/gpio.h"

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

esp_err_t gpio_spy_get_pin_level(gpio_num_t gpio_num, uint32_t *level);

esp_err_t gpio_set_level(gpio_num_t a, uint32_t b);

void gpio_spy_init(void);

void gpio_spy_deinit(void);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif //GPIO_SPY_H