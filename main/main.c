/*!
 *******************************************************************************
 * @file state_machine_states.c
 *
 * @brief
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 18.09.21
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */

/*
 *******************************************************************************
 * #include Statements                                                         *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <driver/spi_master.h>
#include "configuration.h"
#include "esp_system.h"
#include "esp_freertos_hooks.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl/lvgl.h"
#include "driver/gpio.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "disp_spi.h"
#include "ili9341.h"
#include "max6675_spi.h"
#include "tp_spi.h"
#include "xpt2046.h"
#include "freertos/timers.h"
#include "heater.h"
#include "reflow_profile.h"
#include "state_machine/states/state_machine_states.h"
#include "state_machine/state_machine.h"
#include "gui/gui.h"
#include "thermocouple.h"
#include "wdt.h"
#include "reflow_timer.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define WDT_TIMEOUT_S                       CONFIGURATION_WDT_TIMEOUT_S

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

static void IRAM_ATTR lv_tick_task(void);

static void nvs_init();

static bool hardware_init(void);

static bool display_init(void);

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

static lv_color_t buf1[DISP_BUF_SIZE];
static lv_color_t buf2[DISP_BUF_SIZE];
static lv_disp_buf_t display_buffer;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

void app_main()
{

        bool success = true;

        // @note: failing to initialize hardware will assert
        success = hardware_init();

        success = success && wdt_init(WDT_TIMEOUT_S);

        success = success && display_init();

        success = success && reflow_timer_init();

        success = success && reflow_profile_init();

        gui_init();

        success = success && state_machine_init();

        success = success && thermocouple_init();

        success = success && (HEATER_ERROR_SUCCESS == heater_init(thermocouple_get_temperature));

        if (!success) {
                assert(0);
        }

        while (1) {
                vTaskDelay(1);
                lv_task_handler();
        }
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

static void nvs_init()
{
        esp_err_t result = nvs_flash_init();

        if ((ESP_ERR_NVS_NO_FREE_PAGES == result) ||
            (ESP_ERR_NVS_NEW_VERSION_FOUND == result)) {
                // NVS partition was truncated and needs to be erased
                // Retry nvs_flash_init
                ESP_ERROR_CHECK(nvs_flash_erase());
                result = nvs_flash_init();
        }
        ESP_ERROR_CHECK(result);
}

static bool hardware_init(void)
{

        spi_bus_config_t const buscfg = {
                        .miso_io_num = TP_SPI_MISO,
                        .mosi_io_num = TP_SPI_MOSI,
                        .sclk_io_num = TP_SPI_CLK,
                        .quadwp_io_num = -1,
                        .quadhd_io_num = -1,
                        .max_transfer_sz = (4 * 8)
        };

        bool success;
        esp_err_t esp_result;

        esp_result = spi_bus_initialize(HSPI_HOST, &buscfg, 2);

        success = (ESP_OK == esp_result);

        if (success) {
                disp_spi_init();
                tp_spi_init();
                success = max6675_spi_init();
        }

        if (success) {
                gpio_set_direction(17, GPIO_MODE_OUTPUT);
                gpio_set_level(17, 1);

                gpio_set_direction(32, GPIO_MODE_OUTPUT);
                gpio_set_level(32, 0);

                ili9341_init();
                xpt2046_init();
                nvs_init();
        }

        return success;
}

static bool display_init(void)
{

        lv_disp_t const * p_display = NULL;
        lv_indev_t const * p_input_device= NULL;
        bool success = true;

        lv_disp_drv_t display_driver;
        lv_indev_drv_t input_dev_driver;

        lv_init();

        lv_disp_drv_init(&display_driver);
        display_driver.flush_cb = ili9341_flush;
        display_driver.buffer = &display_buffer;
        p_display = lv_disp_drv_register(&display_driver);

        success = (NULL != p_display);

        if (success) {
                lv_indev_drv_init(&input_dev_driver);
                input_dev_driver.read_cb = xpt2046_read;
                input_dev_driver.type = LV_INDEV_TYPE_POINTER;

                p_input_device = lv_indev_drv_register(&input_dev_driver);

                success = (NULL != p_input_device);
        }

        if (success) {
                lv_disp_buf_init(&display_buffer, buf1, buf2, DISP_BUF_SIZE);
                esp_register_freertos_tick_hook(lv_tick_task);
        }

        return success;
}

static void IRAM_ATTR lv_tick_task(void)
{
        lv_tick_inc(portTICK_RATE_MS);
}
/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */
