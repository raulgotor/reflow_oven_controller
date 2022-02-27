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

#ifdef __cplusplus
extern "C"
{
#endif // #ifdef __cplusplus

/*
 *******************************************************************************
 * Public Macros                                                               *
 *******************************************************************************
 */

//TODO: define which pin
#define HEATER_ACTIVE_HIGH_GPIO_PIN              (10)

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

typedef struct {
        int16_t target;
        bool heater_running;
} heater_msg_t;

typedef bool (*heater_temp_getter_t)(size_t const, int16_t * const);

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

heater_error_t heater_init(heater_temp_getter_t const p_f_temp_getter);

heater_error_t heater_set_target(int16_t const degrees);

heater_error_t heater_get_target(int16_t * const p_degrees);

heater_error_t heater_start(void);

heater_error_t heater_stop(void);

heater_error_t heater_deinit(void);

bool heater_is_running(void);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif //HEATER_H