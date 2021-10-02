/*!
 *******************************************************************************
 * @file reflow_profile.c
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

#include <stdint.h>
#include <stdbool.h>
#include <lvgl/src/lv_core/lv_style.h>

#include "nvs.h"
#include "nvs_flash.h"
#include "reflow_profile.h"


/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define REFLOW_PROFILE_DEFAULT_NAME                     "Sn60Pb40"
#define REFLOW_PROFILE_DEFAULT_PREHEAT_TEMP_C           170
#define REFLOW_PROFILE_DEFAULT_SOAK_TIME_S              5 * 1000
#define REFLOW_PROFILE_DEFAULT_REFLOW_TEMP_C            220
#define REFLOW_PROFILE_DEFAULT_DWEEL_TIME_S             5 * 1000
#define REFLOW_PROFILE_DEFAULT_COOLING_TIME_S           50 * 1000
#define REFLOW_PROFILE_DEFAULT_RAMP_SPEED               100
#define REFLOW_PROFILE_DEFAULT_COOLING_TEMP_C           30

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

static bool reflow_profile_validate(reflow_profile_t const * const p_reflow_profile);

static void reflow_profile_use_defaults(void);

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

static bool m_is_initialized = false;

static reflow_profile_t m_reflow_profile;

static reflow_profile_t const m_reflow_profile_default = {
        REFLOW_PROFILE_DEFAULT_NAME,
        REFLOW_PROFILE_DEFAULT_PREHEAT_TEMP_C,
        REFLOW_PROFILE_DEFAULT_SOAK_TIME_S,
        REFLOW_PROFILE_DEFAULT_REFLOW_TEMP_C,
        REFLOW_PROFILE_DEFAULT_DWEEL_TIME_S,
        REFLOW_PROFILE_DEFAULT_COOLING_TIME_S,
        REFLOW_PROFILE_DEFAULT_RAMP_SPEED
};

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

bool reflow_profile_init(void)
{
        bool success = !m_is_initialized;

        if (success) {

                //TODO: change this once we have access to flash

                m_reflow_profile = m_reflow_profile_default;

                // success = reflow_profile_load(&m_reflow_profile);
        }

        return success;

}

bool reflow_profile_save(reflow_profile_t const * const p_reflow_profile)
{
        bool success = (NULL != p_reflow_profile || !m_is_initialized);
        bool needs_close = false;
        size_t required_size = sizeof(reflow_profile_t);
        nvs_handle_t nvs_handle;
        esp_err_t result;
        if (success) {
                result = nvs_open("storage", NVS_READWRITE, &nvs_handle);

                success = (ESP_OK == result);
        }

        if (success) {
                needs_close = true;

                result = nvs_get_blob(nvs_handle, "profile", NULL, &required_size);

                // TODO:
                // if (result != ESP_OK && result != ESP_ERR_NVS_NOT_FOUND) { return result; }

                success = ((ESP_OK == result) &&
                           (sizeof(reflow_profile_t) == required_size));
        }

        if (success) {
                result = nvs_set_blob(nvs_handle, "profile", &p_reflow_profile, required_size);

                success = (ESP_OK == result);
        }

        if (success) {
                result = nvs_commit(nvs_handle);

                success = (ESP_OK == result);
        }

        if (needs_close) {
                nvs_close(nvs_handle);
        }

        return success;
}

bool reflow_profile_load(reflow_profile_t * const p_reflow_profile)
{
        size_t required_size = sizeof(reflow_profile_t);
        bool success = (NULL != p_reflow_profile || !m_is_initialized);
        bool needs_close = false;
        nvs_handle_t nvs_handle;
        esp_err_t result;
        reflow_profile_t reflow_profile_buffer;

        if (success) {
                result = nvs_open("storage", NVS_READWRITE, &nvs_handle);

                success = (ESP_OK == result);
        }

        if (success) {
                needs_close = true;
                result = nvs_get_blob(nvs_handle, "profile",
                                      &reflow_profile_buffer,
                                      &required_size);

                success = ((ESP_OK == result) &&
                           (sizeof(reflow_profile_t) == required_size));
        }

        if (success) {
                *p_reflow_profile = reflow_profile_buffer;
        }

        if (needs_close) {
                nvs_close(nvs_handle);
        }

        return success;
}

bool reflow_profile_use(reflow_profile_t const * const p_reflow_profile)
{
        bool success = (NULL != p_reflow_profile || !m_is_initialized);

        if (success) {
                // TODO: check for current state
        }

        if (success) {
                success = reflow_profile_validate(p_reflow_profile);
        }

        if (success) {
                m_reflow_profile = *p_reflow_profile;
        }

        return success;
}

bool reflow_profile_get_current(reflow_profile_t * const p_reflow_profile) {

        bool success =  (NULL != p_reflow_profile || !m_is_initialized);
        // TODO: change this once we have a proper profile management
        // if (success) {
        *p_reflow_profile = m_reflow_profile;
        // }

        return success;
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

static bool reflow_profile_validate(reflow_profile_t const * const p_reflow_profile)
{
        bool success = true;
        return success;
}

static void reflow_profile_use_defaults(void)
{
        m_reflow_profile = m_reflow_profile_default;
}


/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */
