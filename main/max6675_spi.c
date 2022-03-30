/*!
 *******************************************************************************
 * @file spi.c
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 30.03.22
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "../../main/configuration.h"
#include <string.h>
#include "max6675_spi.h"

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

//! @brief Generic transfer function for MAX6675
static bool max6675_generic_spi_xchg(uint8_t const * const rx_buffer,
                                     size_t const size,
                                     spi_device_handle_t const handle);

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

//! @brief Collection of the different MAX6675 SPI handles
static spi_device_handle_t m_max6675_spi_handles[CONFIGURATION_THERMOCOUPLE_COUNT];

//! @brief Different MAX6675 SPI specific configurations
static spi_device_interface_config_t m_max6675_interface_configs[] =
                {
                                {
                                                .mode = 0,
                                                .clock_speed_hz = 2 * 1000 * 1000,
                                                .spics_io_num = 4,
                                                .queue_size = 3,
                                },
                                {
                                                .mode = 0,
                                                .clock_speed_hz = 2 * 1000 * 1000,
                                                .spics_io_num = 4, //TODO: right pinout
                                                .queue_size = 3,
                                },
                                {
                                                .mode = 0,
                                                .clock_speed_hz = 2 * 1000 * 1000,
                                                .spics_io_num = 4, //TODO: right pinout
                                                .queue_size = 3,
                                },
                                {
                                                .mode = 0,
                                                .clock_speed_hz = 2 * 1000 * 1000,
                                                .spics_io_num = 4, //TODO: right pinout
                                                .queue_size = 3,
                                }
                };

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

/*!
 * @brief Initialize MAX6675 devices in the esp-idf SPI framework
 *
 * This function will add the amount of configured MAX6675 devices to the
 * SPI bus of the ESP32 and fill in the required handles
 *
 * @param               -                   -
 *
 * @return              -                   -
 */
bool max6675_spi_init(void) {
        esp_err_t esp_result;
        bool success = true;
        size_t i;

        for (i = 0; (CONFIGURATION_THERMOCOUPLE_COUNT > i) && (success); ++i) {
                esp_result = spi_bus_add_device(
                                HSPI_HOST,
                                &m_max6675_interface_configs[i],
                                &m_max6675_spi_handles[i]);

                success = (ESP_OK == esp_result);
        }

        return success;
}

/*!
 * @brief Wrapper function for generic transfer function with instance 0
 *
 * @param[out]          p_rx_buffer         Pointer to a buffer where to read
 *                                          the data to
 * @param[in]           size                Number of bytes to read
 *
 * @return              bool                Operation result*
 */
bool max6675_spi_id0_xchg(uint8_t const * const p_rx_buffer, size_t const size)
{
        return max6675_generic_spi_xchg(
                        p_rx_buffer, size, m_max6675_spi_handles[0]);
}

/*!
 * @brief Wrapper function for generic transfer function with instance 1
 *
 * @param[out]          p_rx_buffer         Pointer to a buffer where to read
 *                                          the data to
 * @param[in]           size                Number of bytes to read
 *
 * @return              bool                Operation result*
 */
bool max6675_spi_id1_xchg(uint8_t const * const p_rx_buffer, size_t const size)
{
        return max6675_generic_spi_xchg(
                        p_rx_buffer, size, m_max6675_spi_handles[1]);
}

/*!
 * @brief Wrapper function for generic transfer function with instance 2
 *
 * @param[out]          p_rx_buffer         Pointer to a buffer where to read
 *                                          the data to
 * @param[in]           size                Number of bytes to read
 *
 * @return              bool                Operation result*
 */
bool max6675_spi_id2_xchg(uint8_t const * const p_rx_buffer, size_t const size)
{
        return max6675_generic_spi_xchg(
                        p_rx_buffer, size, m_max6675_spi_handles[2]);
}

/*!
 * @brief Wrapper function for generic transfer function with instance 3
 *
 * @param[out]          p_rx_buffer         Pointer to a buffer where to read
 *                                          the data to
 * @param[in]           size                Number of bytes to read
 *
 * @return              bool                Operation result*
 */
bool max6675_spi_id3_xchg(uint8_t const * const rx_buffer, size_t const size)
{
        return max6675_generic_spi_xchg(
                        rx_buffer, size, m_max6675_spi_handles[3]);
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

/*!
 * @brief Generic transfer function for MAX6675
 *
 * @param[out]          p_rx_buffer         Pointer to a buffer where to read
 *                                          the data to
 * @param[in]           size                Number of bytes to read
 * @param[in]           handle              Handle to an esp-idf spi device
 *
 * @return              bool                Operation result*
 */
static bool max6675_generic_spi_xchg(uint8_t const * const rx_buffer,
                                     size_t const size,
                                     spi_device_handle_t const handle)
{
        spi_transaction_t transaction = {
                        .tx_buffer = NULL,
                        .rx_buffer = rx_buffer,
                        .length = 8 * size,
                        .rxlength = 8 * size,
        };

        bool success = (NULL != rx_buffer) && (0 != size);
        esp_err_t esp_result;

        if (success) {
                esp_result = spi_device_acquire_bus(handle, portMAX_DELAY);

                success = (ESP_OK == esp_result);
        }

        if (success) {
                esp_result = spi_device_transmit(handle, &transaction);
                spi_device_release_bus(handle);

                success = (ESP_OK == esp_result);
        }

        return success;
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */
