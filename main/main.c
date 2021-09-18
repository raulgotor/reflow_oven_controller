/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "lvgl/lvgl.h"

#include "reflowOven.h"

#include "esp_freertos_hooks.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "disp_spi.h"
#include "ili9341.h"

#define ENABLE_TOUCH_INPUT 1
#if ENABLE_TOUCH_INPUT

#include "tp_spi.h"
#include "xpt2046.h"

#include "state_machine/state_machine.h"

#endif

static void IRAM_ATTR lv_tick_task(void);

void nvs_init();

void nvs_init()
{

        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                // NVS partition was truncated and needs to be erased
                // Retry nvs_flash_init
                ESP_ERROR_CHECK(nvs_flash_erase());
                err = nvs_flash_init();
        }
        ESP_ERROR_CHECK(err);

}

esp_err_t save_profile(void)
{
        nvs_handle_t my_handle;
        esp_err_t err;

        err = nvs_open("storage", NVS_READWRITE, &my_handle);
        if (err != ESP_OK) { return err; }

        size_t required_size = sizeof(ReflowProfile);
        err = nvs_get_blob(my_handle, "profile", NULL, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) { return err; }

        err = nvs_set_blob(my_handle, "profile", &my_profile, required_size);
        if (err != ESP_OK) { return err; }
        err = nvs_commit(my_handle);
        if (err != ESP_OK) { return err; }
        nvs_close(my_handle);
        return ESP_OK;

}


void app_main()
{
        lv_init();
        nvs_init();


        disp_spi_init();
        ili9341_init();

#if ENABLE_TOUCH_INPUT
        tp_spi_init();
        xpt2046_init();
#endif

        static lv_color_t buf1[DISP_BUF_SIZE];
        static lv_color_t buf2[DISP_BUF_SIZE];
        static lv_disp_buf_t disp_buf;
        lv_disp_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

        lv_disp_drv_t disp_drv;
        lv_disp_drv_init(&disp_drv);
        disp_drv.flush_cb = ili9341_flush;
        disp_drv.buffer = &disp_buf;
        lv_disp_drv_register(&disp_drv);

#if ENABLE_TOUCH_INPUT
        lv_indev_drv_t indev_drv;
        lv_indev_drv_init(&indev_drv);
        indev_drv.read_cb = xpt2046_read;
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        lv_indev_drv_register(&indev_drv);
#endif

        esp_register_freertos_tick_hook(lv_tick_task);

        state_machine_init();

        while (1) {

                vTaskDelay(1);
                lv_task_handler();
        }
}

static void IRAM_ATTR lv_tick_task(void)
{
        lv_tick_inc(portTICK_RATE_MS);
}
