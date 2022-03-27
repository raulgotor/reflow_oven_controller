/*!
 *******************************************************************************
 * @file maxim_max6675.h
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 27.02.22
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */

#ifndef MAXIM_MAX6675_H
#define MAXIM_MAX6675_H

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

//! @brief max6675 module error types
typedef enum
{
        MAX6675_ERROR_SUCCESS = 0,
        MAX6675_ERROR_GENERAL_ERROR,
        MAX6675_ERROR_BAD_PARAMETER,
        MAX6675_ERROR_ALREADY_INITIALIZED,
        MAX6675_ERROR_NOT_INITIALIZED,
        MAX6675_ERROR_COUNT,

} max6675_error_t;

/*!
 * @brief SPI Transfer function
 *
 * Function should write the specified number of bytes in the provided buffer
 *
 * @param[out]          p_rx_buffer         Pointer to a buffer where to read
 *                                          the data to
 * @param[in]           size                Number of bytes to read
 *
 * @return              bool                Operation result
 */
typedef bool (*pf_read_func_t)(uint8_t * const p_rx_buffer, size_t const size);

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

//! @brief Initialize module
max6675_error_t max6675_init(pf_read_func_t const pf_read_func);

//! @brief Deinitialize module
max6675_error_t max6675_deinit(void);

//! @brief Get whether thermocouple is connected
max6675_error_t max6675_is_sensor_connected(bool * const p_connected);

//! @brief Read sensor temperature
max6675_error_t max6675_read_temperature(uint16_t * const p_temperature);



#endif //MAXIM_MAX6675_H