/*!
 *******************************************************************************
 * @file user_interface.h
 *
 * @brief 
 *
 * @author Raúl Gotor (raulgotor@gmail.com)
 * @date 19.09.21
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */


#ifndef GUI_H
#define GUI_H

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


/*
 *******************************************************************************
 * Public Function Prototypes                                                  *
 *******************************************************************************
 */

bool gui_init(void);

bool gui_configure_main_scr(void);

void gui_configure_buttons_for_state(state_machine_state_text_t const state);

void gui_configure_for_profile(reflow_profile_t const profile);

void gui_update_profiles_dropdown(void);



#endif //GUI_H