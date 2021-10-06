/*!
 *******************************************************************************
 * @file reflow_profile.h
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


#ifndef REFLOW_PROFILE_H
#define REFLOW_PROFILE_H

/*
 *******************************************************************************
 * Public Macros                                                               *
 *******************************************************************************
 */

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

#define REFLOW_PROFILE_RAMP_SPEED_MIN_CS                  (1)
#define REFLOW_PROFILE_RAMP_SPEED_MAX_CS                  (10)
#define REFLOW_PROFILE_NAME_LEN_MAX                       (15)

/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

typedef struct {
        char * name;
        uint16_t preheat_temperature;
        uint16_t soak_time_s;
        uint16_t reflow_temperature;
        uint16_t dwell_time_s;
        uint16_t cooling_temperature;
        uint16_t cooling_time_s;
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

bool reflow_profile_init(void);

bool reflow_profile_save(reflow_profile_t const * const p_reflow_profile);

bool reflow_profile_load(char const * const p_name, reflow_profile_t * const p_reflow_profile);

bool reflow_profile_use(reflow_profile_t const * const p_reflow_profile);

bool reflow_profile_get_current(reflow_profile_t * const p_reflow_profile);

#endif //REFLOW_PROFILE_H