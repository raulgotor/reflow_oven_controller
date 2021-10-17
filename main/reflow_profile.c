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
#include "freertos/FreeRTOS.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "reflow_profile.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define REFLOW_PROFILE_NVS_NAMESPACE                    "reflow_profile"
#define REFLOW_PROFILE_NVS_INITIALIZED                  "initialized"

#define REFLOW_PROFILE_DEFAULT_NAME                     "Sn60Pb40"
#define REFLOW_PROFILE_DEFAULT_PREHEAT_TEMP_C           (170)
#define REFLOW_PROFILE_DEFAULT_SOAK_TIME_S              (5)
#define REFLOW_PROFILE_DEFAULT_REFLOW_TEMP_C            (220)
#define REFLOW_PROFILE_DEFAULT_DWELL_TIME_S             (5)
#define REFLOW_PROFILE_DEFAULT_COOLING_TIME_S           (100)
#define REFLOW_PROFILE_DEFAULT_COOLING_TEMP_C           (30)
#define REFLOW_PROFILE_DEFAULT_RAMP_SPEED               (10)

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

static bool is_valid_reflow_profile(reflow_profile_t const * const p_reflow_profile);

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
        REFLOW_PROFILE_DEFAULT_DWELL_TIME_S,
        REFLOW_PROFILE_DEFAULT_COOLING_TEMP_C,
        REFLOW_PROFILE_DEFAULT_COOLING_TIME_S,
        REFLOW_PROFILE_DEFAULT_RAMP_SPEED,
};

static nvs_handle_t m_nvs_h;

static bool add_fake_profiles(void);

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
                add_fake_profiles();
        }

        return success;
}

bool reflow_profile_save(reflow_profile_t const * const p_reflow_profile)
{
        bool success = ((NULL != p_reflow_profile) && (m_is_initialized));
        bool needs_close = false;
        size_t const required_size = sizeof(reflow_profile_t);
        esp_err_t result;

        if (success) {
                success = is_valid_reflow_profile(p_reflow_profile);
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
                                      p_reflow_profile,
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

/*!
 * @brief Load a reflow profile from NVS to a `reflow_profile_t` object
 *
 * @param[in]       p_name              Name of the profile to load
 * @param[out]      p_reflow_profile    Pointer to object where to store the
 *                                      profile at
 *
 * @return          bool                Result of the operation
 */
bool reflow_profile_load(char const * const p_name,
                         reflow_profile_t * const p_reflow_profile)
{
        size_t required_size = sizeof(reflow_profile_t);
        bool success = m_is_initialized;
        bool needs_close = false;
        nvs_handle_t nvs_handle;
        esp_err_t result;
        reflow_profile_t reflow_profile_buffer;

        if (success) {
                success = (NULL != p_reflow_profile) && (NULL != p_name);
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
                success = is_valid_reflow_profile(&reflow_profile_buffer);
        }

        if (success) {
                *p_reflow_profile = reflow_profile_buffer;
        }

        if (needs_close) {
                nvs_close(nvs_handle);
        }

        return success;
}

bool reflow_profile_delete(char const * const p_name)
{
        bool success = (m_is_initialized) && (NULL != p_name);
        bool needs_close = false;
        nvs_handle_t nvs_handle;
        esp_err_t result;

        if (success) {
                result = nvs_open(REFLOW_PROFILE_NVS_NAMESPACE,
                                  NVS_READWRITE,
                                  &nvs_handle);

                success = (ESP_OK == result);
        }

        if (success) {
                needs_close = true;
                result = nvs_erase_key(nvs_handle, p_name);

                success = (ESP_OK == result);
        }

        if (needs_close) {
                nvs_close(nvs_handle);
        }

        return success;
}


bool reflow_profile_use(reflow_profile_t const * const p_reflow_profile)
{
        bool success = ((NULL != p_reflow_profile) && (m_is_initialized));

        if (success) {
                success = is_valid_reflow_profile(p_reflow_profile);
        }

        if (success) {
                m_reflow_profile = *p_reflow_profile;
        }

        return success;
}

bool reflow_profile_get_current(reflow_profile_t * const p_reflow_profile)
{

        bool success = ((NULL != p_reflow_profile) && (m_is_initialized));

        if (success) {
                *p_reflow_profile = m_reflow_profile;
        }

        return success;
}
/*!
 * @brief
 *
 * @warning this function only frees memory if it was allocated and then a failure
 *          occurs afterwards. Otherwise, its caller responsibility to free it
 * @param p_profiles
 * @param p_size
 * @return
 */
bool reflow_profile_get_profiles_list(char ** p_profiles, size_t * const p_size)
{
        bool success = ((NULL != p_profiles) && (NULL != p_size));
        char * buffer = NULL;
        bool needs_to_free_on_fail = true;
        size_t counter = 0;
        nvs_iterator_t iterator = nvs_entry_find("nvs",
                                                 REFLOW_PROFILE_NVS_NAMESPACE,
                                                 NVS_TYPE_ANY);
        nvs_entry_info_t info;


        while ((NULL != iterator) && (success)) {
                nvs_entry_info(iterator, &info);
                iterator = nvs_entry_next(iterator);
                // adding space for the name but also for the \n character
                counter += strlen(info.key) + 1;

                printf("key '%s', type '%d' \n", info.key, info.type);
        }

        if (success) {

                iterator = nvs_entry_find("nvs",
                                          REFLOW_PROFILE_NVS_NAMESPACE,
                                          NVS_TYPE_ANY);

                // adding space for the \0 terminator
                buffer = pvPortMalloc(counter + 1);

                success = (NULL != buffer);
        }

        if (success) {
                needs_to_free_on_fail = true;
                strcpy(buffer, "");
        }

        while ((NULL != iterator) && (success)) {
                nvs_entry_info(iterator, &info);
                iterator = nvs_entry_next(iterator);
                strcat(buffer, info.key);
                strcat(buffer, "\n");
        }

        if (!success || (0 == strlen(buffer))) {
                *p_size = 0;
                success = false;
                vPortFree(buffer);
                buffer = NULL;
        }

        if (success) {
                *p_profiles = buffer;
                *p_size = counter;
        }


        printf("%s %d\n", buffer, counter);

        return success;
}

bool reflow_profile_get_default(reflow_profile_t * const p_reflow_profile)
{
        bool success = (NULL != p_reflow_profile);

        if (success) {
                *p_reflow_profile = m_reflow_profile_default;
        }

        return success;
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */
static bool is_valid_reflow_profile(reflow_profile_t const * const p_reflow_profile)
{
        bool success = false;

        if ((REFLOW_PROFILE_REFLOW_TEMP_MAX_C >=
                                    p_reflow_profile->reflow_temperature) &&
            (REFLOW_PROFILE_REFLOW_TEMP_MIN_C <=
                                    p_reflow_profile->reflow_temperature) &&
            (REFLOW_PROFILE_PREHEAT_TEMP_MAX_C >=
                                   p_reflow_profile->preheat_temperature) &&
            (REFLOW_PROFILE_REFLOW_TEMP_MIN_C <=
                                   p_reflow_profile->preheat_temperature) &&
            (REFLOW_PROFILE_COOLING_TEMP_MAX_C >=
                                   p_reflow_profile->cooling_temperature) &&
            (REFLOW_PROFILE_COOLING_TEMP_MIN_C <=
                                   p_reflow_profile->cooling_temperature) &&
            (REFLOW_PROFILE_DWELL_TIME_MAX_S >=
                                         p_reflow_profile->dwell_time_s) &&
            (REFLOW_PROFILE_DWELL_TIME_MIN_S <=
                                         p_reflow_profile->dwell_time_s) &&
            (REFLOW_PROFILE_SOAK_TIME_MAX_S >=
                                          p_reflow_profile->soak_time_s) &&
            (REFLOW_PROFILE_SOAK_TIME_MIN_S <=
                                          p_reflow_profile->soak_time_s) &&
            (REFLOW_PROFILE_COOLING_TIME_MAX_S >=
                                          p_reflow_profile->cooling_time_s) &&
            (REFLOW_PROFILE_COOLING_TIME_MIN_S <=
                                          p_reflow_profile->cooling_time_s) &&
            (REFLOW_PROFILE_RAMP_SPEED_MAX_CS >=
                                          p_reflow_profile->ramp_speed) &&
            (REFLOW_PROFILE_RAMP_SPEED_MIN_CS <=
                                              p_reflow_profile->ramp_speed) &&
            (REFLOW_PROFILE_NAME_LEN_MAX >= strlen(p_reflow_profile->name)))
        {
                success = true;
        }
        return success;
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

                if (ESP_OK != result) {
                        assert(result && "Couldn't commit changes to nvs");
                }
                nvs_close(m_nvs_h);
        }

        return success;
}

static bool add_fake_profiles(void)
{
        bool success = true;
        reflow_profile_t fake_profile = m_reflow_profile_default;
        char fakename[11] = "fake_namex\0";
        uint8_t i = 0;

        for (i = 0; (10 > i) && (success); i++) {
                memcpy(fake_profile.name, fakename, 11);
                fake_profile.name[9] = '0' + i;
                reflow_profile_save(&fake_profile);
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
