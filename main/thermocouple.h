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

typedef enum {
        THERMOCOUPLE_REFRESH_RATE_1_HZ = 0,
        THERMOCOUPLE_REFRESH_RATE_100_HZ,
        THERMOCOUPLE_REFRESH_RATE_1_KHZ,
        THERMOCOUPLE_REFRESH_RATE_COUNT
} thermocouple_refresh_rate_t;

typedef enum {
        THERMOCOUPLE_ID_0 = 0,
        THERMOCOUPLE_ID_1,
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

bool thermocouple_init(void);

bool thermocouple_set_referesh_rate(thermocouple_refresh_rate_t const refresh_rate);

bool thermocouple_get_temperature(thermocouple_id_t const id, int16_t * const temperature);

#endif //THERMOCOUPLE_H