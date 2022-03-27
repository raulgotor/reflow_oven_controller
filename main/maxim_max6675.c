/*!
 *******************************************************************************
 * @file maxim_max6675.c
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

/*
 *******************************************************************************
 * #include Statements                                                         *
 *******************************************************************************
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "maxim_max6675.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define MAX6675_INTERFACE_DATA_START_BIT    (3)
#define MAX6675_INTERFACE_OPEN_TC_BIT       (2)

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

//! @brief Read n-bytes from the transfer function
static max6675_error_t max6675_read_n_bytes(uint8_t * const p_rx_buffer,
                                            size_t const size);

//! @brief Get MAX6675 raw readout
static max6675_error_t max6675_get_raw_readout(uint16_t * const p_readout);

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

//! @brief Pointer to transfer function
static pf_read_func_t m_read_function = NULL;

//! @brief Whether the module is initialized or not
static bool m_is_initialized = false;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

/*!
 * @brief Initialize module
 *
 * @param[in]           pf_read_func        Function pointer to a SPI transfer
 *                                          function
 *
 * @return              max6675_error_t     Operation result
 * @retval              MAX6675_ERROR_SUCCESS
 *                                          Operation was successful
 * @retval              MAX6675_ERROR_BAD_PARAMETER
 *                                          Null function pointer
 * @retval              MAX6675_ERROR_ALREADY_INITIALIZED
 *                                          Module was already initialized
 */
max6675_error_t max6675_init(pf_read_func_t const pf_read_func)
{
        max6675_error_t result = MAX6675_ERROR_SUCCESS;

        if (NULL == pf_read_func) {
                result = MAX6675_ERROR_BAD_PARAMETER;
        } else if (m_is_initialized) {
                result = MAX6675_ERROR_ALREADY_INITIALIZED;
        } else {
                m_read_function = pf_read_func;
                m_is_initialized = true;
        }

        return result;
}

/*!
 * @brief Deinitialize module
 *
 * @param               -                   -
 *
 * @return              max6675_error_t     Operation result
 * @retval              MAX6675_ERROR_SUCCESS
 *                                          Operation was successful
 * @retval              MAX6675_ERROR_NOT_INITIALIZED
 *                                          Module is not yet initialized
 */
max6675_error_t max6675_deinit(void)
{
        max6675_error_t result = MAX6675_ERROR_SUCCESS;

        if (!m_is_initialized) {
                result = MAX6675_ERROR_NOT_INITIALIZED;
        } else {
                m_read_function = NULL;
                m_is_initialized = false;
        }

        return result;
}

/*!
 * @brief Get whether thermocouple is connected
 *
 * Query whether the thermocouple is connected to the MAX6675 sensor or it is in
 * open-circuit
 *
 * @param[out]          p_connected         Pointer where to store the thermo-
 *                                          couple status at.
 *                                          true: thermocouple is connected
 *                                          false: thermocouple is open-circuit
 *
 * @return              max6675_error_t     Operation result
 * @retval              MAX6675_ERROR_SUCCESS
 *                                          Operation was successful
 * @retval              MAX6675_ERROR_NOT_INITIALIZED
 *                                          Module is not yet initialized
 * @retval              MAX6675_ERROR_BAD_PARAMETER
 *                                          Parameter is null
 */
max6675_error_t max6675_is_sensor_connected(bool * const p_connected)
{
        max6675_error_t result = MAX6675_ERROR_SUCCESS;
        uint16_t sensor_output;

        if (!m_is_initialized) {
                result = MAX6675_ERROR_NOT_INITIALIZED;
        } else if (NULL == p_connected) {
                result = MAX6675_ERROR_BAD_PARAMETER;
        }

        if (MAX6675_ERROR_SUCCESS == result) {
                result = max6675_get_raw_readout(&sensor_output);
        }

        if (MAX6675_ERROR_SUCCESS == result) {
                *p_connected = !(sensor_output &
                                 (1 << MAX6675_INTERFACE_OPEN_TC_BIT));
        }

        return  result;
}


/*!
 * @brief Read sensor temperature
 *
 * Query the MAX6675 sensor to retrieve the last temperature measurement
 *
 * The ADC adds the cold-junction diode measurement with the amplified
 * thermocouple voltage and reads out the 12-bit result onto the SO pin.
 * A sequence of all zeros means the thermocouple reading is 0°C.
 * A sequence of all ones means the thermocouple reading is +1023.75°C
 *
 * @param[out]          p_temperature       Pointer where to return the value at
 *
 * @return              max6675_error_t     Operation result
 * @retval              MAX6675_ERROR_SUCCESS
 *                                          Operation was successful
 * @retval              MAX6675_ERROR_NOT_INITIALIZED
 *                                          Module is not yet initialized
 * @retval              MAX6675_ERROR_BAD_PARAMETER
 *                                          Parameter is null
 */
max6675_error_t max6675_read_temperature(uint16_t * const p_temperature)
{
        max6675_error_t result = MAX6675_ERROR_SUCCESS;
        uint16_t sensor_output;

        if (!m_is_initialized) {
                result = MAX6675_ERROR_NOT_INITIALIZED;
        } else if (NULL == p_temperature) {
                result = MAX6675_ERROR_BAD_PARAMETER;
        }

        if (MAX6675_ERROR_SUCCESS == result) {
                result = max6675_get_raw_readout(&sensor_output);
        }

        if (MAX6675_ERROR_SUCCESS == result) {
                sensor_output >>= MAX6675_INTERFACE_DATA_START_BIT;
                sensor_output *=25;

                *p_temperature = sensor_output;
        }

        return result;
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

/*!
 * @brief Read n-bytes from the transfer function
 *
 * @param[out]          p_rx_buffer         Pointer to a buffer where to read
 *                                          the data to
 * @param[in]           size                Number of bytes to read
 *
 * @return              max6675_error_t     Operation result
 * @retval              MAX6675_ERROR_SUCCESS
 *                                          Operation was successful
 * @retval              MAX6675_ERROR_NOT_INITIALIZED
 *                                          Module is not yet initialized
 * @retval              MAX6675_ERROR_BAD_PARAMETER
 *                                          Parameter is null
 * @retval              MAX6675_ERROR_GENERAL_ERROR
 *                                          Transfer function failed
*/
static max6675_error_t max6675_read_n_bytes(uint8_t * const p_rx_buffer,
                                            size_t const size)
{
        max6675_error_t result = MAX6675_ERROR_SUCCESS;
        bool read_success;

        if (!m_is_initialized) {
                result = MAX6675_ERROR_NOT_INITIALIZED;
        } else if (NULL == p_rx_buffer) {
                result = MAX6675_ERROR_BAD_PARAMETER;
        }

        if (MAX6675_ERROR_SUCCESS == result) {
                read_success = m_read_function(p_rx_buffer, size);

                if (!read_success) {
                        result = MAX6675_ERROR_GENERAL_ERROR;
                }
        }

        return result;
}

/*!
 * @brief Get sensor raw readout
 *
 * Get MAX6675 sensor raw readout containing the temperature data and sensor
 * status information
 *
 * A complete serial interface read requires 16 clock cycles. Read the 16 output
 * bits on the falling edge of the clock. The first bit, D15, is a dummy sign
 * bit and is always zero. Bits D14–D3 contain the converted temperature in the
 * order of MSB to LSB. Bit D2 is normally low and goes high when the
 * thermocouple input is open. D1 is low to provide a device ID for the MAX6675
 * and bit D0 is three-state.
 *
 * | 15   | 14  13  12  11  10  9  8  7  6  5  4  3 |    2    |    1    |  0   |
 * |------|-----------------------------------------|---------|---------|------|
 * | Dummy|                 12-bit                  | Thermoc.|  Device |State |
 * | sign | MSB      Temperature reading        LSB |   input |    ID   |      |
 * | bit  |                                         |         |         |      |
 *
 * @param[out]          p_readout           Pointer where to return the value at
 *
 * @return              max6675_error_t     Operation result
 * @retval              MAX6675_ERROR_SUCCESS
 *                                          Operation was successful
 * @retval              MAX6675_ERROR_NOT_INITIALIZED
 *                                          Module is not yet initialized
 * @retval              MAX6675_ERROR_BAD_PARAMETER
 *                                          Parameter is null
 */
static max6675_error_t max6675_get_raw_readout(uint16_t * const p_readout)
{
        size_t const rx_buffer_size = 2;
        max6675_error_t result = MAX6675_ERROR_SUCCESS;
        uint8_t rx_buffer[2];

        if (!m_is_initialized) {
                result = MAX6675_ERROR_NOT_INITIALIZED;
        } else if (NULL == p_readout) {
                result = MAX6675_ERROR_BAD_PARAMETER;
        }

        if (MAX6675_ERROR_SUCCESS == result) {
                result = max6675_read_n_bytes(rx_buffer, rx_buffer_size);
        }

        if (MAX6675_ERROR_SUCCESS == result) {
                *p_readout = (uint16_t)((rx_buffer[0] & 0x00FF) << 8);
                *p_readout |= (uint16_t)(rx_buffer[1] & 0x00FF);
        }

        return result;
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */
