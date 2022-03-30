/*!
 *******************************************************************************
 * @file thermocouple_fake.h
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 20.02.22
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Raúl
 * All rights reserved.
 *******************************************************************************
 */

#ifndef THERMOCOUPLE_FAKE_H
#define THERMOCOUPLE_FAKE_H

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

void thermocouple_fake_set_temperature(uint16_t const temperature);

bool thermocouple_fake_get_temperature(uint16_t * const temperature);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif //THERMOCOUPLE_FAKE_H