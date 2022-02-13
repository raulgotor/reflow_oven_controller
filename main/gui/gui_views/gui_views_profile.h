/*!
 *******************************************************************************
 * @file gui_views_profile.h
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

#ifndef GUI_VIEWS_PROFILE_H
#define GUI_VIEWS_PROFILE_H

/*
 *******************************************************************************
 * Public Macros                                                               *
 *******************************************************************************
 */


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

lv_obj_t * p_dropdown_label;
lv_obj_t * p_new_button_label;
lv_obj_t * p_edit_button_label;
lv_obj_t * p_delete_button_label;

lv_obj_t * p_new_button;
lv_obj_t * p_edit_button;
lv_obj_t * p_delete_button;

lv_obj_t * p_dropdown;

/*
 *******************************************************************************
 * Public Function Prototypes                                                  *
 *******************************************************************************
 */

void gui_views_profile(lv_obj_t * const p_parent);

#endif //GUI_VIEWS_PROFILE_H