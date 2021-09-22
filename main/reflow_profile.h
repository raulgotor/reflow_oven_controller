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

#define REFLOW_PROFILE_PREHEAT_TEMPERATURE_MIN          50
#define REFLOW_PROFILE_SOAK_TIME_MIN                    10
#define REFLOW_PROFILE_REFLOW_TEMPERATURE_MIN           70
#define REFLOW_PROFILE_REFLOW_TIME_MIN                  10


/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

typedef struct {
        char * name;
        uint16_t preheat_temperature;
        uint16_t soak_time;
        uint16_t reflow_temperature;
        uint16_t dwell_time;
        uint16_t cooling_time;
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

bool reflow_profile_save(reflow_profile_t const * const p_reflow_profile);

bool reflow_profile_load(reflow_profile_t * const p_reflow_profile);

bool reflow_profile_use(reflow_profile_t const * const p_reflow_profile);

bool reflow_profile_get_current(reflow_profile_t * const p_reflow_profile);

#endif //REFLOW_PROFILE_H