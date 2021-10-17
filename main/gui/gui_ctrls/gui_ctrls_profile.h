/*!
 *******************************************************************************
 * @file gui_ctrls_profile.h
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

#ifndef GUI_CTRLS_PROFILE_H
#define GUI_CTRLS_PROFILE_H

/*
 *******************************************************************************
 * Public Macros                                                               *
 *******************************************************************************
 */

#define BUTTON_TEXT_NEW                         "New"
#define BUTTON_TEXT_EDIT                        "Edit"
#define BUTTON_TEXT_DELETE                      "Delete"

#define LABEL_TEXT_PROFILE                      "Profile:"

#define MESSAGE_TEXT_DELETE_PROFILE             "Are you sure you want to delete this profile?"

#define BUTTON_TEXT_CANCEL                      "Cancel"

/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

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

void gui_tab_profile_new_btn_event(lv_obj_t * const p_object,
                                   lv_event_t const event);

void gui_tab_profile_delete_event(lv_obj_t * const p_object,
                                  lv_event_t const event);

void gui_select_profile_cb(lv_obj_t * p_object, lv_event_t const event);

void gui_ctrls_profile_init(void);


#endif //GUI_CTRLS_PROFILE_H