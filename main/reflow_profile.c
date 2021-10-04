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
#define REFLOW_PROFILE_DEFAULT_SLOT                     (0)
#define REFLOW_PROFILE_NVS_NAMESPACE                    "reflow_profile"
#define REFLOW_PROFILE_NVS_INITIALIZED                  "initialized"
#define REFLOW_PROFILE_NVS_CURRENT_PROFILE_SLOT         "current_profile_slot"
#define REFLOW_PROFILE_NVS_PROFILE                      "reflow_profile"



#define REFLOW_PROFILE_DEFAULT_NAME                     "Sn60Pb40"
#define REFLOW_PROFILE_DEFAULT_PREHEAT_TEMP_C           (170)
#define REFLOW_PROFILE_DEFAULT_SOAK_TIME_S              (5 * 1000)
#define REFLOW_PROFILE_DEFAULT_REFLOW_TEMP_C            (220)
#define REFLOW_PROFILE_DEFAULT_DWEEL_TIME_S             (5 * 1000)
#define REFLOW_PROFILE_DEFAULT_COOLING_TIME_S           (50 * 1000)
#define REFLOW_PROFILE_DEFAULT_RAMP_SPEED               (100)
#define REFLOW_PROFILE_DEFAULT_COOLING_TEMP_C           (30)

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

static bool is_profile_nvs_initialized(void);

static bool initialize_profile_nvs(void);

static void print_namespace_contents(void);

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
        REFLOW_PROFILE_DEFAULT_COOLING_TEMP_C,
        REFLOW_PROFILE_DEFAULT_RAMP_SPEED,
};

static nvs_handle_t m_nvs_h;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

bool reflow_profile_init(void)
{
        bool success = !m_is_initialized;

        if (success) {

                if (!is_profile_nvs_initialized()) {
                        success = initialize_profile_nvs();
                }
        }

        // TODO: surround with debug macro
        print_namespace_contents();

        if (success) {
                m_is_initialized = true;
                success = reflow_profile_load(m_reflow_profile_default.name,
                                              &m_reflow_profile);
        }

        return success;
}

bool reflow_profile_save(reflow_profile_t const * const p_reflow_profile)
{
        bool success = ((NULL != p_reflow_profile) && (m_is_initialized));
        bool needs_close = false;
        size_t required_size = sizeof(reflow_profile_t);
        esp_err_t result;

        if (success) {
                success = reflow_profile_validate(p_reflow_profile);
        }

        if (success) {
                result = nvs_open(REFLOW_PROFILE_NVS_NAMESPACE,
                                  NVS_READWRITE,
                                  &m_nvs_h);

                success = (ESP_OK == result);
        }

        if (success) {
                needs_close = true;
                result = nvs_set_blob(m_nvs_h,
                                      p_reflow_profile->name,
                                      &p_reflow_profile,
                                      required_size);

                success = (ESP_OK == result);
        }

        if (success) {
                result = nvs_commit(m_nvs_h);

                success = (ESP_OK == result);
        }

        if (needs_close) {
                nvs_close(m_nvs_h);
        }

        return success;
}

bool reflow_profile_load(char const * const p_name,
                         reflow_profile_t * const p_reflow_profile)
{
        size_t required_size = sizeof(reflow_profile_t);
        bool success = ((NULL != p_reflow_profile) && (m_is_initialized));
        bool needs_close = false;
        nvs_handle_t nvs_handle;
        esp_err_t result;
        reflow_profile_t reflow_profile_buffer;

        if (success) {
                success = reflow_profile_validate(p_reflow_profile);
        }

        if (success) {
                result = nvs_open(REFLOW_PROFILE_NVS_NAMESPACE,
                                  NVS_READONLY,
                                  &nvs_handle);

                success = (ESP_OK == result);
        }

        if (success) {
                needs_close = true;
                result = nvs_get_blob(nvs_handle,
                                      p_name,
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
        bool success = (NULL != p_reflow_profile && m_is_initialized);

        if (success) {
                success = reflow_profile_validate(p_reflow_profile);
        }

        if (success) {
                m_reflow_profile = *p_reflow_profile;
        }

        return success;
}

bool reflow_profile_get_current(reflow_profile_t * const p_reflow_profile) {

        bool success =  ((NULL != p_reflow_profile) && (m_is_initialized));

        if (success) {
                *p_reflow_profile = m_reflow_profile;
        }

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
        // TODO: implementation required
        return success;
}

static void reflow_profile_use_defaults(void)
{
        m_reflow_profile = m_reflow_profile_default;
}

static bool is_profile_nvs_initialized(void)
{
        uint8_t initialized = 0;
        bool needs_close = false;
        bool success;
        esp_err_t result = nvs_open(REFLOW_PROFILE_NVS_NAMESPACE,
                                    NVS_READWRITE,
                                    &m_nvs_h);

        success = (ESP_OK == result);

        if (success) {
                result = nvs_get_u8(m_nvs_h,
                            REFLOW_PROFILE_NVS_INITIALIZED,
                            &initialized);
                printf("Restart counter = %d\n", initialized);

                needs_close = true;
        }

        if ((ESP_OK == result) && (1 == initialized)) {
                success = true;
        } else if ((ESP_ERR_NOT_FOUND == result) || (0 == initialized)) {
                success = false;
        } else {
                assert(0);
        }

        if (needs_close) {
                nvs_close(m_nvs_h);
        }

        return success;
}

static bool initialize_profile_nvs(void)
{
        bool needs_close = false;
        size_t const profile_size = sizeof(m_reflow_profile_default);
        esp_err_t result = nvs_open(REFLOW_PROFILE_NVS_NAMESPACE,
                                    NVS_READWRITE, &m_nvs_h);
        bool success;


        success = (ESP_OK == result);

        if (success) {
                needs_close = true;
                result = nvs_set_u8(m_nvs_h,
                                    REFLOW_PROFILE_NVS_INITIALIZED,
                                    1);
                success = (ESP_OK == result);
        }

        if (success) {
                result = nvs_set_blob(m_nvs_h,
                                      m_reflow_profile_default.name,
                                      &m_reflow_profile_default,
                                      profile_size);

                success = (ESP_OK == result);
        }

        if (needs_close) {
                result = nvs_commit(m_nvs_h);

                if (ESP_OK != result){
                        assert(result && "Couldn't commit changes to nvs");
                }
                nvs_close(m_nvs_h);
        }

        return success;
}


static void print_namespace_contents(void)
{
        nvs_entry_info_t info;
        nvs_iterator_t iterator = nvs_entry_find("nvs",
                                                 REFLOW_PROFILE_NVS_NAMESPACE,
                                                 NVS_TYPE_ANY);
        while (NULL != iterator) {
                nvs_entry_info(iterator, &info);
                iterator = nvs_entry_next(iterator);
                printf("key '%s', type '%d' \n", info.key, info.type);
        }

}



/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */
