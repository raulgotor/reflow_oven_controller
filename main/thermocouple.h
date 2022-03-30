/*!
 *******************************************************************************
 * @file thermocouple.h
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 18.09.21
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */

#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H

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

/*!
 *  @brief Different refresh rates for thermocouple readouts at different states
 *
 *  @note While the enum name is in herz, the hold value is in milliseconds to
 *        wait so it can be conveniently passed to `vTaskDelay()`
 */

typedef enum {
        THERMOCOUPLE_REFRESH_RATE_1_HZ = 1000,
        THERMOCOUPLE_REFRESH_RATE_4_HZ = 250,
        THERMOCOUPLE_REFRESH_RATE_COUNT
} thermocouple_refresh_rate_t;

//! @brief ID of the different thermocouples
typedef enum {
        THERMOCOUPLE_ID_0 = 0,
        THERMOCOUPLE_ID_1,
        THERMOCOUPLE_ID_2,
        THERMOCOUPLE_ID_3,
        THERMOCOUPLE_ID_COUNT
} thermocouple_id_t;

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

//! @brief Initialize thermocouple module
bool thermocouple_init(void);

//! @brief Get thermocouple temperature
bool thermocouple_get_temperature(thermocouple_id_t const id,
                                  uint16_t * const p_temperature);

bool thermocouple_get_avg_temperature(uint16_t * const p_avg_temperature);

#endif //THERMOCOUPLE_H