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
//! @brief GPIO controlling the heater
#define HEATER_ACTIVE_HIGH_GPIO_PIN              (10)

/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

//! @brief Heater module return values
typedef enum {

        //! @brief Everything went well
        HEATER_ERROR_SUCCESS = 0,

        //! @brief General error
        HEATER_ERROR_GENERAL_ERROR,

        //! @brief Module is not initialized
        HEATER_ERROR_NOT_INITIALIZED,

        //! @brief Null or out of range parameter passed
        HEATER_ERROR_BAD_PARAMETER,

        //! @brief Out of memory error
        HEATER_ERROR_OOM,

        //! @brief Fence member
        HEATER_ERROR_COUNT
} heater_error_t;

//! @brief Heater message type
typedef struct {
        //! @brief Desired target temperature in degrees celsius
        int16_t target;

        //! @brief Desired heater controller state
        bool heater_control_active;
} heater_msg_t;

/*!
 * @brief Temperature getter function pointer
 *
 * Function pointer to a function capable of returning the actual temperature
 * of a specified probe
 *
 * @param[in]           id                  ID of the probe to query
 *
 * @param[out]          p_temp              Pointer where to save the temperature
 *
 * @return              Bool                Operation result
 */
typedef bool (*heater_temp_getter_t)(size_t const id, uint16_t * const p_temp);

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

//! @brief Initialize heater module
heater_error_t heater_init(heater_temp_getter_t const p_f_temp_getter);

//! @brief Set heater target temperature
heater_error_t heater_set_target(uint16_t const degrees);

//! @brief Get actual heater target temperature
heater_error_t heater_get_target(uint16_t * const p_degrees);

//! @brief Start the heater control
heater_error_t heater_start(void);

//! @brief Stop the heater control
heater_error_t heater_stop(void);

//! @brief Deinitialize the heater module
heater_error_t heater_deinit(void);

//! @brief Query whether the heater is running
bool heater_is_running(void);

void heater_emergency_stop(void);
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif //HEATER_H