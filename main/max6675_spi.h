/*!
 *******************************************************************************
 * @file spi.h
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

#ifndef SPI_H
#define SPI_H

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

//! @brief Initialize MAX6675 devices in the esp-idf SPI framework
bool max6675_spi_init(void);

//! @brief Wrapper function for generic transfer function with instance 0
bool max6675_spi_id0_xchg(uint8_t const * const p_rx_buffer, size_t const size);

//! @brief Wrapper function for generic transfer function with instance 1
bool max6675_spi_id1_xchg(uint8_t const * const p_rx_buffer, size_t const size);

//! @brief Wrapper function for generic transfer function with instance 2
bool max6675_spi_id2_xchg(uint8_t const * const p_rx_buffer, size_t const size);

//! @brief Wrapper function for generic transfer function with instance 3
bool max6675_spi_id3_xchg(uint8_t const * const p_rx_buffer, size_t const size);
#endif //SPI_H