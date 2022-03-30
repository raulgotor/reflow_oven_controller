/**
 * @file tp_spi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include <stddef.h>
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "../../main/configuration.h"
#include <string.h>
#include "tp_spi.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/
static spi_device_handle_t m_touch_panel_spi;
static spi_device_handle_t m_max6675_spi_handles[THERMOCOUPLE_ON_BOARD_COUNT];
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
                                .spics_io_num = 34, //TODO: right pinout
                                .queue_size = 3,
                        },
                        {
                                .mode = 0,
                                .clock_speed_hz = 2 * 1000 * 1000,
                                .spics_io_num = 7, //TODO: right pinout
                                .queue_size = 3,
                        }
                };

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void spi_init(void) {
        esp_err_t esp_result;
        bool success;
        size_t i;

        spi_bus_config_t buscfg = {
                        .miso_io_num = TP_SPI_MISO,
                        .mosi_io_num = TP_SPI_MOSI,
                        .sclk_io_num = TP_SPI_CLK,
                        .quadwp_io_num = -1,
                        .quadhd_io_num = -1,
                        .max_transfer_sz = (4 * 8)
        };

        spi_device_interface_config_t max6675_cfg={
                        .mode = 0,
                        .clock_speed_hz = 2*1000*1000,
                        .spics_io_num=4,
                        .queue_size=3,
        };

        spi_device_interface_config_t tp_cfg={
                        .mode=0,                                //SPI mode 0
                        .clock_speed_hz=2*1000*1000,           //Clock out at 80 MHz
                        //.spics_io_num=-1,              //CS pin
                        .spics_io_num=17,              //CS pin
                        .queue_size=1,
                        .pre_cb=NULL,
                        .post_cb=NULL,
        };

        //Initialize the SPI bus
        esp_result = spi_bus_initialize(HSPI_HOST, &buscfg, 2);

        success = (ESP_OK == esp_result);

        for (i = 0; (THERMOCOUPLE_ON_BOARD_COUNT > i) && (success); ++i) {
                esp_result = spi_bus_add_device(
                                HSPI_HOST,
                                &m_max6675_interface_configs[i],
                                &m_max6675_spi_handles[i]);

                success = (ESP_OK == esp_result);
        }

        //Attach the LCD to the SPI bus
        esp_result = spi_bus_add_device(HSPI_HOST, &tp_cfg, &m_touch_panel_spi);

        //TODO change signature to have a return here:
        assert(esp_result==ESP_OK);
}

void tp_spi_init(void)
{
#if 1

	esp_err_t ret;

	spi_bus_config_t buscfg={
		.miso_io_num=TP_SPI_MISO,
		.mosi_io_num=TP_SPI_MOSI,
		.sclk_io_num=TP_SPI_CLK,
		.quadwp_io_num=-1,
		.quadhd_io_num=-1
	};

	spi_device_interface_config_t devcfg={
		.clock_speed_hz=2*1000*1000,           //Clock out at 80 MHz
		.mode=0,                                //SPI mode 0
		.spics_io_num=-1,              //CS pin
		.queue_size=1,
		.pre_cb=NULL,
		.post_cb=NULL,
	};

	//Initialize the SPI bus
	ret=spi_bus_initialize(HSPI_HOST, &buscfg, 2);
	assert(ret==ESP_OK);

	//Attach the LCD to the SPI bus
	ret=spi_bus_add_device(HSPI_HOST, &devcfg, &m_touch_panel_spi);
	assert(ret==ESP_OK);
#endif // #if 0
}

uint8_t tp_spi_xchg(uint8_t data_send)
{
    uint8_t data_recv = 0;
    
    spi_transaction_t t = {
        .length = 8, // length is in bits
        .tx_buffer = &data_send,
        .rx_buffer = &data_recv
    };


    spi_device_queue_trans(m_touch_panel_spi, &t, portMAX_DELAY);

    spi_transaction_t * rt;
    spi_device_get_trans_result(m_touch_panel_spi, &rt, portMAX_DELAY);

    return data_recv;
}

bool max6675_generic_spi_xchg(uint8_t const * const rx_buffer,
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

bool max6675_id0_spi_xchg(uint8_t const * const rx_buffer, size_t const size)
{
        return max6675_generic_spi_xchg(
                        rx_buffer, size, m_max6675_spi_handles[0]);
}

bool max6675_id1_spi_xchg(uint8_t const * const rx_buffer, size_t const size)
{
        return max6675_generic_spi_xchg(
                        rx_buffer, size, m_max6675_spi_handles[1]);
}

bool max6675_id2_spi_xchg(uint8_t const * const rx_buffer, size_t const size)
{
        return max6675_generic_spi_xchg(
                        rx_buffer, size, m_max6675_spi_handles[2]);
}

bool max6675_id3_spi_xchg(uint8_t const * const rx_buffer, size_t const size)
{
        return max6675_generic_spi_xchg(
                        rx_buffer, size, m_max6675_spi_handles[3]);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
