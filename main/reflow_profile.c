/*!
 *******************************************************************************
 * @file reflow_profile.c
 *
 * @brief This module defines a reflow profile, and provides an API for all the
 *        operations needed regarding them, such as load, save, delete,
 *        validate, compare,...
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
#include "esp_log.h"
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

#define TAG                                             __FILENAME__

#define REFLOW_PROFILE_NVS_NAMESPACE                    "reflow_profile"
#define REFLOW_PROFILE_NVS_NAMESPACE_INIT               "init"
#define REFLOW_PROFILE_NVS_INITIALIZED                  "initialized"
#define REFLOW_PROFILE_NVS_DEFAULT_PROFILE_NAME         "default_profile"

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

//! @brief Validate given `reflow_profile_t` object
static bool is_valid_reflow_profile(reflow_profile_t const * const p_reflow_profile);

//! @brief Check whether the profile NVS section is initialized
static bool is_profile_nvs_initialized(void);

//! @brief Initialize NVS
static bool initialize_profile_nvs(void);

//! @brief Print contents of the provided namespace
static void print_namespace_contents(char const * const p_namespace);

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

//! @brief Control wheter the module is initialized or not
static bool m_is_initialized = false;

//! @brief Current reflow profile being used
static reflow_profile_t m_reflow_profile;

//! @brief Reflow profile used for default or new profiles
static reflow_profile_t const m_reflow_profile_factory = {
        REFLOW_PROFILE_DEFAULT_NAME,
        REFLOW_PROFILE_DEFAULT_PREHEAT_TEMP_C,
        REFLOW_PROFILE_DEFAULT_SOAK_TIME_S,
        REFLOW_PROFILE_DEFAULT_REFLOW_TEMP_C,
        REFLOW_PROFILE_DEFAULT_DWELL_TIME_S,
        REFLOW_PROFILE_DEFAULT_COOLING_TEMP_C,
        REFLOW_PROFILE_DEFAULT_COOLING_TIME_S,
        REFLOW_PROFILE_DEFAULT_RAMP_SPEED,
};

//! @brief Handle for the NVS objet being used
static nvs_handle_t m_nvs_h;

// TODO: remove after development
static bool add_fake_profiles(void);

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

/*!
 * @brief Initialize reflow profile module
 *
 * @param           -                       -
 *
 * @return          bool                    Result of the operation
 */
bool reflow_profile_init(void)
{
        bool success = !m_is_initialized;
        char default_name[REFLOW_PROFILE_NAME_LEN_MAX + 1];

        if (success) {
                // TODO: check for NVS initialized

                if (!is_profile_nvs_initialized()) {
                        success = initialize_profile_nvs();
                }
        }

        // TODO: surround with debug macro
        print_namespace_contents(REFLOW_PROFILE_NVS_NAMESPACE_INIT);
        print_namespace_contents(REFLOW_PROFILE_NVS_NAMESPACE);

        if (success) {
                m_is_initialized = true;

                success = reflow_profile_get_default(default_name);
        }

        if (success) {
                success = reflow_profile_load(default_name,
                                              &m_reflow_profile);
        }

        return success;
}

/*!
 * @brief Determine whether two profiles are identical
 *
 * @param[in]       p_reflow_profile_1      pointer to the first profile to
 *                                          compare
 * @param[in]       p_reflow_profile_2      pointer to the second profile to
 *                                          compare
 * @return          bool                    Result of the operation
 * @retval          True                    If profiles are equal
 * @retval          False                   If profiles differ or pointer invalid
 */
bool reflow_profile_are_equal(reflow_profile_t const * const p_reflow_profile_1,
                              reflow_profile_t const * const p_reflow_profile_2)
{
        bool success = (NULL != p_reflow_profile_1) &&
                       (NULL != p_reflow_profile_2);

        if ((success) &&
            (0 == strcmp(p_reflow_profile_1->name, p_reflow_profile_2->name)) &&
            (p_reflow_profile_1->preheat_temperature == p_reflow_profile_2->preheat_temperature) &&
            (p_reflow_profile_1->soak_time_s         == p_reflow_profile_2->soak_time_s) &&
            (p_reflow_profile_1->reflow_temperature  == p_reflow_profile_2->reflow_temperature) &&
            (p_reflow_profile_1->dwell_time_s        == p_reflow_profile_2->dwell_time_s) &&
            (p_reflow_profile_1->cooling_temperature == p_reflow_profile_2->cooling_temperature) &&
            (p_reflow_profile_1->cooling_time_s      == p_reflow_profile_2->cooling_time_s) &&
            (p_reflow_profile_1->ramp_speed          == p_reflow_profile_2->ramp_speed)) {
                success = true;
        }

        return success;
}

/*!
 * @brief Save profile to NVS
 *
 * The function will take a pointer to the profile and validate it. If is valid,
 * it will try to save it to the NVS profile section
 *
 * @warning This function is not checking for the amount of already saved
 *          profiles. This is caller responsibility.
 *
 * @param[in]       p_reflow_profile        pointer to the `reflow_profile_t`
 *                                          to save
 *
 * @return          bool                    Result of the operation
 * @retval          true                    If everything went well
 * @retval          false                   If pointer was invalid, module not
 *                                          initialized, profile was invalid or
 *                                          couldn't be saved due to I/O failure
 */
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
                ESP_LOGI(TAG, "Profile %s was saved", p_reflow_profile->name);
        }

        if (needs_close) {
                nvs_close(m_nvs_h);
        }

        return success;
}

/*!
 * @brief Load a reflow profile from NVS to a `reflow_profile_t` object
 *
 * @note This function loads a profile from the NVS to the given object, but
 *       it's not set as the current or default profile.
 *       @see `reflow_profile_use` and ``
 *
 * @param[in]       p_name              Name of the profile to load
 * @param[out]      p_reflow_profile    Pointer to object where to store the
 *                                      profile at
 *
 * @return          bool                Result of the operation
 * @retval          true                If everything went well
 * @retval          false               If a pointer was invalid, module not
 *                                      initialized, retrieved profile was
 *                                      invalid or couldn't be loaded due to an
 *                                      I/O failure
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
                ESP_LOGI(TAG, "Profile %s was loaded", reflow_profile_buffer.name);
        }

        if (needs_close) {
                nvs_close(nvs_handle);
        }

        return success;
}

/*!
 * @brief Delete a `reflow_profile_t` from the NVS
 *
 * The function will search on the NVS for a key with the given name and delete
 * it if found
 *
 * @param[in]       p_name                  pointer to a string containing the
 *                                          name of the `reflow_profile_t` to
 *                                          delete
 *
 * @return          bool                    Result of the operation
 * @retval          true                    If everything went well
 * @retval          false                   If pointer was invalid, module not
 *                                          initialized, profile was not found
 *                                          or couldn't be deleted due to I/O
 *                                          failure
 */
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

                if (success) {
                        ESP_LOGI(TAG, "Profile %s was deleted", p_name);
                }
        }

        if (needs_close) {
                nvs_close(nvs_handle);
        }

        return success;
}

/*!
 * @brief Load and use the profile with the given name
 *
 * This function will load a profile from a given `p_name` and will set it as
 * default in the NVS so it is used next time the device boots
 *
 * @param               p_name              pointer to string holding the
 *                                          p_name of the profile to load
 *
 * @return              bool                Result of the operation
 * @retval              True                If everything went well
 * @retval              False               If pointer is null, module is not
 *                                          initialized, or there was a I/O
 *                                          error
 */
bool reflow_profile_use(char const * p_name)
{
        bool success = ((NULL != p_name) && (m_is_initialized));
        bool needs_close = false;

        reflow_profile_t buffer_profile;
        esp_err_t result;

        if (success) {
                success = reflow_profile_load(p_name, &buffer_profile);
        }

        if (success) {
                m_reflow_profile = buffer_profile;

                result = nvs_open(REFLOW_PROFILE_NVS_NAMESPACE_INIT,
                                  NVS_READWRITE, &m_nvs_h);

                success = (ESP_OK == result);
        }

        if (success) {
                needs_close = true;
                result = nvs_set_str(m_nvs_h,
                                     REFLOW_PROFILE_NVS_DEFAULT_PROFILE_NAME,
                                     m_reflow_profile.name);

                success = (ESP_OK == result);
        }

        if (success) {
                ESP_LOGI(TAG, "Profile %s is being used and set to default", m_reflow_profile.name);

                result = nvs_commit(m_nvs_h);

                success = (ESP_OK == result);
        }

        if (needs_close) {
                nvs_close(m_nvs_h);
        }

        printf("p_name %s\n", m_reflow_profile.name);
        printf("preheat_temperature %d\n", m_reflow_profile.preheat_temperature);
        printf("soak_time_s %d\n", m_reflow_profile.soak_time_s);
        printf("reflow_temperature %d\n", m_reflow_profile.reflow_temperature);
        printf("dwell_time_s %d\n", m_reflow_profile.dwell_time_s);
        printf("cooling_temperature %d\n", m_reflow_profile.cooling_temperature);
        printf("cooling_time_s %d\n", m_reflow_profile.cooling_time_s);
        printf("ramp_speed %d\n", m_reflow_profile.ramp_speed);

        return success;
}

/*!
 * @brief Get from NVS the default profile to be used
 *
 * This function reads on the specific NVS section which profile is currently
 * being used at the time, or was being used last time that the device went off
 *
 * @param[in]           p_name              pointer to a string where to store
 *                                          the default profile name to
 *
 * @return              bool                Result of the operation
 * @retval              True                If everything went well
 * @retval              False               If pointer is null, module is not
 *                                          initialized, or there was a I/O
 *                                          error
 */
bool reflow_profile_get_default(char const * p_name)
{

        bool success = ((NULL != p_name) && (m_is_initialized));
        bool needs_close = false;
        esp_err_t result;
        size_t name_len = REFLOW_PROFILE_NAME_LEN_MAX;

        if (success) {
                result = nvs_open(REFLOW_PROFILE_NVS_NAMESPACE_INIT,
                                  NVS_READWRITE,
                                  &m_nvs_h);

                success = (ESP_OK == result);
        }

        if (success) {
                needs_close = true;

                result = nvs_get_str(m_nvs_h,
                                     REFLOW_PROFILE_NVS_DEFAULT_PROFILE_NAME,
                                     p_name,
                                     &name_len);

                success = (ESP_OK == result);
        }

        if (success) {

                ESP_LOGI(TAG, "Default profile is %s", p_name);
                result = nvs_commit(m_nvs_h);

                success = (ESP_OK == result);
        }

        if (needs_close) {
                nvs_close(m_nvs_h);
        }

        return success;
}

/*!
 * @brief Get from module variable a `reflow_profile_t` object being used right
 *        now
 *
 * @param[out]          p_reflow_profile    pointer to a `reflow_profile_t`
 *                                          where to store the current profile
 *                                          to
 *
 * @return              bool                Result of the operation
 * @retval              True                If everything went well
 * @retval              False               If pointer is null or module is not
 *                                          initialized
 */
bool reflow_profile_get_current(reflow_profile_t * const p_reflow_profile)
{

        bool success = ((NULL != p_reflow_profile) && (m_is_initialized));

        if (success) {
                *p_reflow_profile = m_reflow_profile;
        }

        return success;
}

/*!
 * @brief Get a string with all the profile names stored in the NVS
 *
 * @warning This function allocates memory for providing the string object.
 *          Memory is only freed by the function if it was allocated and then a
 *          failure occured afterwards. Otherwise, its caller responsibility to
 *          free it
 *
 * @param[out]          p_profiles          Double pointer to store the pointer
 *                                          of the allocated list to.
 * @param[out]          p_size              Pointer to return the size of the
 *                                          allocated list.
 *
 * @return              bool                Result of the operation
 * @retval              True                If everything went well
 * @retval              False               If a pointer is null, module is not
 *                                          initialized or allocation failed
 */
bool reflow_profile_get_profiles_list(char ** p_profiles, size_t * const p_size)
{
        bool success = ((NULL != p_profiles) && (NULL != p_size));
        char * buffer = NULL;
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
                strcpy(buffer, "");

                while (NULL != iterator) {
                        nvs_entry_info(iterator, &info);
                        iterator = nvs_entry_next(iterator);
                        strcat(buffer, info.key);
                        strcat(buffer, "\n");
                }
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
                ESP_LOGI(TAG, "Profiles list %s", buffer);
        }

        return success;
}

/*!
 * @brief Get default factory `reflow_profile_t` object
 *
 * @param[out]          p_reflow_profile    Pointer to the object where to store
 *                                          the profile at
 *
 * @return              bool                Result of the operation
 * @retval              True                If everything went well
 * @retval              False               If pointer is null
 */
bool reflow_profile_get_factory_profile(reflow_profile_t * const p_reflow_profile)
{
        bool success = (NULL != p_reflow_profile);

        if (success) {
                *p_reflow_profile = m_reflow_profile_factory;
        }

        return success;
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

/*!
 * @brief Validate given `reflow_profile_t` object
 *
 * The function will check that the profile fields are within the established
 * maximum and minimum values
 *
 * @param[in]           p_reflow_profile    Pointer to the object to validate
 *
 * @return              bool                Result of the operation
 * @retval              True                If profile is valid
 * @retval              False               If pointer is null, or profile is
 *                                          invalid
 */
static bool is_valid_reflow_profile(reflow_profile_t const * const p_reflow_profile)
{
        bool success = (NULL != p_reflow_profile);

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

/*!
 * @brief Check whether the profile NVS section is initialized
 *
 * The function will check that the profile fields are within the established
 * maximum and minimum values
 *
 * @param               -                   -
 *
 * @return              bool                Result of the operation
 * @retval              True                If nvs section is initialized
 * @retval              False               If I/O error happened or nvs section
 *                                          is not initialized
 */
static bool is_profile_nvs_initialized(void)
{
        uint8_t initialized = 0;
        bool needs_close = false;
        esp_err_t result = nvs_open(REFLOW_PROFILE_NVS_NAMESPACE_INIT,
                                            NVS_READWRITE,
                                            &m_nvs_h);

        bool success = (ESP_OK == result);

        if (success) {
                needs_close = true;

                result = nvs_get_u8(m_nvs_h,
                            REFLOW_PROFILE_NVS_INITIALIZED,
                            &initialized);

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

/*!
 * @brief Initialize NVS
 *
 * The function will save default values to both INIT and PROFILE NVS
 *
 * @param               -                   -
 *
 * @return              bool                Result of the operation
 * @retval              True                If nvs section is initialized
 * @retval              False               If I/O error occurred
 */
static bool initialize_profile_nvs(void)
{
        bool needs_close = false;
        size_t const profile_size = sizeof(m_reflow_profile_factory);
        esp_err_t result = nvs_open(REFLOW_PROFILE_NVS_NAMESPACE_INIT,
                                    NVS_READWRITE,
                                    &m_nvs_h);

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
                result = nvs_set_str(m_nvs_h,
                                     REFLOW_PROFILE_NVS_DEFAULT_PROFILE_NAME,
                                     REFLOW_PROFILE_DEFAULT_NAME);

                success = (ESP_OK == result);
        }

        if (success) {
                result = nvs_commit(m_nvs_h);
                success = (ESP_OK == result);
        }

        if (needs_close) {
                nvs_close(m_nvs_h);
                needs_close = false;
        }

        if (success) {
                result = nvs_open(REFLOW_PROFILE_NVS_NAMESPACE,
                                  NVS_READWRITE, &m_nvs_h);
                success = (ESP_OK == result);
        }

        if (success) {
                needs_close = true;
                result = nvs_set_blob(m_nvs_h,
                                      m_reflow_profile_factory.name,
                                      &m_reflow_profile_factory,
                                      profile_size);

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

//TODO: remove after development done
static bool add_fake_profiles(void)
{
        bool success = true;
        reflow_profile_t fake_profile = m_reflow_profile_factory;
        char fakename[11] = "fake_namex\0";
        uint8_t i = 0;

        for (i = 0; (10 > i) && (success); i++) {
                memcpy(fake_profile.name, fakename, 11);
                fake_profile.name[9] = '0' + i;
                reflow_profile_save(&fake_profile);
        }

        return success;
}
/*!
 * @brief Print contents of the provided namespace
 *
 * @param               p_namespace         Pointer to the namespace string to
 *                                          print the contents from
 *
 * @return              -                   -
 */
static void print_namespace_contents(char const * const p_namespace)
{
        nvs_entry_info_t info;
        nvs_iterator_t iterator = nvs_entry_find("nvs",
                                                 p_namespace,
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
