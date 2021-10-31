/*!
 *******************************************************************************
 * @file reflow_profile.h
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


#ifndef REFLOW_PROFILE_H
#define REFLOW_PROFILE_H

/*
 *******************************************************************************
 * Public Macros                                                               *
 *******************************************************************************
 */

//! @brief Limit values for temperature and time at the different phases
#define REFLOW_PROFILE_PREHEAT_TEMP_MAX_C                 (220)
#define REFLOW_PROFILE_PREHEAT_TEMP_MIN_C                 (50)
#define REFLOW_PROFILE_SOAK_TIME_MIN_S                    (5)
#define REFLOW_PROFILE_SOAK_TIME_MAX_S                    (300)
#define REFLOW_PROFILE_REFLOW_TEMP_MAX_C                  (280)
#define REFLOW_PROFILE_REFLOW_TEMP_MIN_C                  (70)
#define REFLOW_PROFILE_DWELL_TIME_MIN_S                   (5)
#define REFLOW_PROFILE_DWELL_TIME_MAX_S                   (100)
#define REFLOW_PROFILE_COOLING_TIME_MAX_S                 (600)
#define REFLOW_PROFILE_COOLING_TIME_MIN_S                 (100)
#define REFLOW_PROFILE_COOLING_TEMP_MAX_C                 (80)
#define REFLOW_PROFILE_COOLING_TEMP_MIN_C                 (25)

//! @brief Limit values for heating ramp
#define REFLOW_PROFILE_RAMP_SPEED_MIN_CS                  (1)
#define REFLOW_PROFILE_RAMP_SPEED_MAX_CS                  (10)

//! @brief Maximum profile name length
#define REFLOW_PROFILE_NAME_LEN_MAX                       (15)

//! @brief Maximum allowed number of profiles in NVS
#define REFLOW_PROFILES_MAX_PROFILES_CNT                  (10)

/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

//! @brief Reflow profile structure with its configuration fields
typedef struct {
        //! @brief Name of the profile
        char name[16];

        //! @brief Temperature to reach at preheat phase, in Celsius
        uint16_t preheat_temperature;

        //! @brief Time to settle at soak phase in seconds
        uint16_t soak_time_s;

        //! @brief Temperature to reach at reflow phase, in Celsius
        uint16_t reflow_temperature;

        //! @brief Time to settle at dwell phase in seconds
        uint16_t dwell_time_s;

        //! @brief Temperature to reach (downwards) at cooling phase, in Celsius
        uint16_t cooling_temperature;

        /*!
         * @brief Maximum time allowed durng cooling phase in seconds before
         *        erroring out
         */
        uint16_t cooling_time_s;

        /*!
         * @brief Heating speed for preheat and reflow phases in celsius per second
         */
        uint16_t ramp_speed;
} reflow_profile_t;

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

//! @brief Initialize reflow profile module
bool reflow_profile_init(void);

//! @brief Save profile to NVS
bool reflow_profile_save(reflow_profile_t const * const p_reflow_profile);

//! @brief Load a reflow profile from NVS to a `reflow_profile_t` object
bool reflow_profile_load(char const * const p_name,
                         reflow_profile_t * const p_reflow_profile);

//! @brief Delete a `reflow_profile_t` from the NVS
bool reflow_profile_delete(char const * const p_name);

//! @brief Load and use the profile with the given name
bool reflow_profile_use(char const * p_name);

//! @brief Get from module variable a `reflow_profile_t` object being used right
bool reflow_profile_get_current(reflow_profile_t * const p_reflow_profile);

//! @brief Get a string with all the profile names stored in the NVS
bool reflow_profile_get_profiles_list(char ** p_profiles,
                                      size_t * const p_size);

//! @brief Get default factory `reflow_profile_t` object
bool reflow_profile_get_factory_profile(reflow_profile_t * const p_reflow_profile);

//! @brief Get from NVS the default profile to be used
bool reflow_profile_get_default(char const * p_name);

//! @brief Determine whether two profiles are identical
bool reflow_profile_are_equal(reflow_profile_t const * const p_reflow_profile_1,
                              reflow_profile_t const * const p_reflow_profile_2);

#endif //REFLOW_PROFILE_H