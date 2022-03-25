/*!
 *******************************************************************************
 * @file heater.h
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 13.02.22
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */

#ifndef HEATER_H
#define HEATER_H

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
        HEATER_ERROR_SUCCESS = 0,
        HEATER_ERROR_GENERAL_ERROR,
        HEATER_ERROR_NOT_INITIALIZED,
        HEATER_ERROR_BAD_PARAMETER,
        HEATER_ERROR_OOM,
        HEATER_ERROR_COUNT
} heater_error_t;
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

heater_error_t heater_init(void);

heater_error_t heater_set_target(int16_t const degrees);

heater_error_t heater_get_target(int16_t * const p_degrees);

heater_error_t heater_start(void);

heater_error_t heater_stop(void);

heater_error_t heater_deinit(void);


#endif //HEATER_H