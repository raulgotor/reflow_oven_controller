/*!
 *******************************************************************************
 * @file gui_views_main.c
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 17.10.21
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

#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "lvgl.h"

#include "state_machine/states/state_machine_states.h"
#include "state_machine/state_machine.h"
#include "esp_log.h"
#include "reflow_profile.h"
#include "thermocouple.h"
#include "gui/gui.h"
#include "gui/gui_ctrls/gui_ctrls_main.h"
#include "gui/gui.h"
#include "gui/gui_views/gui_views_main.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

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

/*
 *******************************************************************************
 * Public Data Declarations                                                    *
 *******************************************************************************
 */

void gui_views_main(lv_obj_t * const p_parent)
{

        int16_t const meter_angle = 270;
        int16_t const meter_line_count = 54;

        if (NULL == p_parent) {
                assert(0);
        }

        // Start button

        p_start_button = lv_btn_create(p_parent, NULL);
        lv_obj_set_pos(p_start_button, 10, 10);
        lv_obj_set_size(p_start_button, 100, 100);
        lv_obj_set_event_cb(p_start_button, gui_ctrls_main_button_event_cb);

        // Labels

        p_profile_label = lv_label_create(p_parent, NULL);
        lv_label_set_text(p_profile_label, "PROFILE: Sn60Pb40");
        lv_label_set_style(p_profile_label, LV_LABEL_STYLE_MAIN, &m_style);

        p_state_label = lv_label_create(p_parent, NULL);
        lv_label_set_style(p_state_label, LV_LABEL_STYLE_MAIN, &m_style);

        p_start_button_label = lv_label_create(p_start_button, NULL);
        lv_label_set_text(p_start_button_label, LV_SYMBOL_PLAY "Run");

        p_temp_label = lv_label_create(p_parent, NULL);
        lv_label_set_align(p_temp_label, LV_LABEL_ALIGN_CENTER);
        lv_label_set_style(p_temp_label, LV_LABEL_STYLE_MAIN, &m_big_style);
        lv_obj_align(p_temp_label, p_lmeter, LV_ALIGN_CENTER, 0, 0);

        // Gauge

        p_lmeter = lv_lmeter_create(p_parent, NULL);
        lv_obj_set_pos(p_lmeter, 135, 10);
        lv_obj_set_size(p_lmeter, 140, 140);

        lv_obj_align(p_profile_label, p_start_button, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
        lv_obj_align(p_state_label, p_profile_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

        lv_obj_align(p_temp_label, p_lmeter, LV_ALIGN_CENTER, 0, 0);

        lv_lmeter_set_scale(p_lmeter, meter_angle, meter_line_count);
}

/*
 *******************************************************************************
 * Static Data Declarations                                                    *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

