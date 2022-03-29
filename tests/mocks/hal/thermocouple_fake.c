/*!
 *******************************************************************************
 * @file thermocouple_fake.c
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 20.02.22
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

#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

#include "thermocouple.h"

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

static int16_t m_fake_temperatures[THERMOCOUPLE_ID_COUNT] = {0};

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

void thermocouple_fake_set_temperature(thermocouple_id_t const id,
                                       uint16_t const temperature)
{
        m_fake_temperatures[id] = temperature;
}

bool thermocouple_fake_get_temperature(
                thermocouple_id_t const id,
                uint16_t * const temperature)
{
        bool success = (NULL != temperature);

        if (success) {
                *temperature = m_fake_temperatures[id];
        }

        return temperature;
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
