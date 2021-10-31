/*!
 *******************************************************************************
 * @file gui_views_profile.c
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

#include "lvgl.h"
#include "reflow_profile.h"
#include "gui/gui_ctrls/gui_ctrls_profile.h"
#include "gui/gui_views/gui_views_profile.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define BUTTON_TEXT_CANCEL                      "Cancel"

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

void gui_views_profile(lv_obj_t * const p_parent)
{
        uint8_t const button_width = 85;
        uint8_t const button_height = 50;

        // Configure buttons

        p_new_button = lv_btn_create(p_parent, NULL);
        lv_obj_set_size(p_new_button, button_width, button_height);

        p_edit_button = lv_btn_create(p_parent, NULL);
        lv_obj_set_size(p_edit_button, button_width, button_height);

        p_delete_button = lv_btn_create(p_parent, NULL);
        lv_obj_set_size(p_delete_button, button_width, button_height);

        // Add labels

        p_new_button_label = lv_label_create(p_new_button, NULL);
        lv_label_set_text(p_new_button_label, LV_SYMBOL_OK BUTTON_TEXT_NEW);
        p_edit_button_label = lv_label_create(p_edit_button, NULL);
        lv_label_set_text(p_edit_button_label, LV_SYMBOL_EDIT BUTTON_TEXT_EDIT);
        p_delete_button_label = lv_label_create(p_delete_button, NULL);
        lv_label_set_text(p_delete_button_label, LV_SYMBOL_TRASH BUTTON_TEXT_DELETE);

        // Configure dropdown

        p_dropdown_label = lv_label_create(p_parent, NULL);
        lv_label_set_text(p_dropdown_label, LABEL_TEXT_PROFILE);

        p_dropdown = lv_ddlist_create(p_parent, NULL);

        lv_obj_set_event_cb(p_dropdown, gui_ctrls_profile_select_dd_event);

        lv_ddlist_set_sb_mode(p_dropdown, LV_SB_MODE_ON);
        lv_ddlist_set_fix_height(p_dropdown, 150);
        lv_obj_align(p_dropdown, p_dropdown, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // Layout

        lv_obj_align(p_edit_button, p_parent, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
        lv_obj_align(p_new_button, p_edit_button, LV_ALIGN_OUT_LEFT_MID, -15, 0);
        lv_obj_align(p_delete_button, p_edit_button, LV_ALIGN_OUT_RIGHT_MID, 15, 0);

        // Set button event callbacks
        lv_obj_set_event_cb(p_new_button, gui_ctrls_profile_edit_button_event);
        lv_obj_set_event_cb(p_edit_button, gui_ctrls_profile_edit_button_event);
        lv_obj_set_event_cb(p_delete_button, gui_ctrls_profile_delete_button_event);
}


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
