/*!
 *******************************************************************************
 * @file gui_ctrls_profile.c
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

#include <stdio.h>
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "gui/gui_views/gui_views_profile.h"
#include "reflow_profile.h"
#include "gui/gui.h"
#include "gui/gui_ctrls/gui_ctrls_profile.h"

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

typedef struct
{
        lv_obj_t * container;
        lv_obj_t * slider;
        lv_obj_t * value_label;
        lv_obj_t * name_label;
        char const * name;
        lv_coord_t slider_min;
        lv_coord_t slider_max;
        size_t offset;
        void (* f_pointer)(lv_obj_t * const p_object, lv_event_t const event);

} container_t;

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

static void save_new_profile_cb(lv_obj_t * p_object, lv_event_t const event);

static void save_existing_profile_cb(lv_obj_t * p_object, lv_event_t const event);

static uint8_t get_slider_index_from_pointer(lv_obj_t const * const p_object);

static void delete_profile_msb_box_button_event_cb(lv_obj_t * const p_object,
                                                   lv_event_t const event);

static bool helper_get_list_len(char const * const p_profiles_list, uint32_t * const p_len);

static bool helper_get_position_at_list(char const * const p_profiles_list,
                                        char const * const p_name,
                                        uint32_t * const p_selected);

static bool helper_get_first_name(char const * const p_profiles_list,
                                  char * const p_first_name);

static void update_profiles_dropdown(void);

static void keyboard_cb(lv_obj_t * p_object, lv_event_t const event);

static void textfield_cb(lv_obj_t * const p_object, lv_event_t const event);

static void profile_slider_changed_cb(lv_obj_t * const p_object,
                                      lv_event_t const event);

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

static const char * m_delete_msg_box_options[] = {"Delete", "Cancel", ""};

static lv_obj_t * m_p_msg_box;

static reflow_profile_t m_buffer_profile;

static container_t m_slider_container_list[] = {
                {NULL, NULL, NULL, NULL, "Preheat temp", REFLOW_PROFILE_PREHEAT_TEMP_MIN_C, REFLOW_PROFILE_PREHEAT_TEMP_MAX_C, offsetof(reflow_profile_t,
                                                                                                                                        preheat_temperature), profile_slider_changed_cb},
                {NULL, NULL, NULL, NULL, "Soak time",    REFLOW_PROFILE_SOAK_TIME_MIN_S,    REFLOW_PROFILE_SOAK_TIME_MAX_S,    offsetof(reflow_profile_t,
                                                                                                                                        soak_time_s),         profile_slider_changed_cb},
                {NULL, NULL, NULL, NULL, "Reflow temp",  REFLOW_PROFILE_REFLOW_TEMP_MIN_C,  REFLOW_PROFILE_REFLOW_TEMP_MAX_C,  offsetof(reflow_profile_t,
                                                                                                                                        reflow_temperature),  profile_slider_changed_cb},
                {NULL, NULL, NULL, NULL, "Dwell time",   REFLOW_PROFILE_DWELL_TIME_MIN_S,   REFLOW_PROFILE_DWELL_TIME_MAX_S,   offsetof(reflow_profile_t,
                                                                                                                                        dwell_time_s),        profile_slider_changed_cb},
                //{NULL, NULL, NULL, NULL, "Cooling temp", REFLOW_PROFILE_COOLING_TEMP_MIN_C, REFLOW_PROFILE_COOLING_TEMP_MAX_C, offsetof(reflow_profile_t, cooling_temperature),profile_slider_changed_cb},
                //{NULL, NULL, NULL, NULL, "Cooling time", REFLOW_PROFILE_COOLING_TIME_MIN_S, REFLOW_PROFILE_COOLING_TIME_MIN_S, offsetof(reflow_profile_t , cooling_time_s),profile_slider_changed_cb},
};

static size_t const edit_prof_cont_list_len = sizeof(m_slider_container_list) /
                                              sizeof(m_slider_container_list[0]);

static char m_profile_name_buffer[16];

static lv_obj_t * m_window_close_button;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

void gui_ctrls_profile_init(void)
{
        update_profiles_dropdown();
}

void gui_ctrls_profile_edit_button_event(lv_obj_t * const p_object,
                                         lv_event_t const event)
{
        bool success = true;
        char * window_title = "";

        lv_obj_t * window;
        lv_obj_t * name_field;
        lv_obj_t * one_line_label;
        lv_obj_t * previous_object;
        uint8_t i;
        uint16_t slider_value;

        char buf[6];

        if ((LV_EVENT_CLICKED != event) ||
            (LV_BTN_STATE_REL != lv_btn_get_state(p_object))) {
                return;
        }

        if (p_new_button == p_object) {
                window_title = "New Profile";
                success = reflow_profile_get_factory_profile(&m_buffer_profile);
        } else if (p_edit_button == p_object) {
                window_title = "Edit Profile";
                success = reflow_profile_get_current(&m_buffer_profile);
        }

        if (!success) {
                return;
        }

        // Window
        window = lv_win_create(lv_scr_act(), NULL);
        lv_win_set_title(window, window_title);
        m_window_close_button = lv_win_add_btn(window, LV_SYMBOL_CLOSE);

        if (p_new_button == p_object) {
                lv_obj_set_event_cb(m_window_close_button, save_new_profile_cb);
        } else if (p_edit_button == p_object) {
                lv_obj_set_event_cb(m_window_close_button, save_existing_profile_cb);
        }

        name_field = lv_ta_create(window, NULL);
        lv_obj_set_pos(window, 1, 1);

        // Text field
        lv_obj_set_width(name_field, 200);
        lv_ta_set_one_line(name_field, true);
        lv_ta_set_cursor_type(name_field, LV_CURSOR_LINE);
        lv_obj_align(name_field, window, LV_ALIGN_IN_TOP_LEFT, 10, 80);
        lv_ta_set_cursor_type(name_field, LV_CURSOR_BLOCK);
        lv_ta_set_text(name_field, m_buffer_profile.name);
        lv_ta_set_max_length(name_field, 15);
        lv_obj_set_event_cb(name_field, textfield_cb);

        // Text field label
        one_line_label = lv_label_create(window, NULL);
        lv_label_set_text(one_line_label, "Profile name:");
        lv_obj_align(one_line_label, name_field, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

        for (i = 0; edit_prof_cont_list_len > i; i++) {

                previous_object = name_field;

                if (i != 0) {
                        previous_object = m_slider_container_list[i - 1].container;
                }

                slider_value = *((uint16_t *)((uint8_t *)(&m_buffer_profile) + m_slider_container_list[i].offset));

                // Label text
                m_slider_container_list[i].name_label = lv_label_create(window, NULL);
                lv_label_set_text(m_slider_container_list[i].name_label, m_slider_container_list[i].name);
                lv_obj_align(m_slider_container_list[i].name_label, previous_object, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

                // Container
                m_slider_container_list[i].container = lv_cont_create(window, NULL);
                lv_obj_set_auto_realign(m_slider_container_list[i].container, true);
                lv_obj_align(m_slider_container_list[i].container, m_slider_container_list[i].name_label,
                             LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
                lv_cont_set_fit(m_slider_container_list[i].container, LV_FIT_NONE);
                lv_obj_set_width(m_slider_container_list[i].container, 280);
                lv_cont_set_layout(m_slider_container_list[i].container, LV_LAYOUT_ROW_M);

                // Slider
                m_slider_container_list[i].slider = lv_slider_create(m_slider_container_list[i].container, NULL);
                lv_slider_set_range(m_slider_container_list[i].slider, m_slider_container_list[i].slider_min,
                                    m_slider_container_list[i].slider_max);
                lv_slider_set_value(m_slider_container_list[i].slider, (int16_t)slider_value, LV_ANIM_OFF);
                lv_obj_set_event_cb(m_slider_container_list[i].slider, m_slider_container_list[i].f_pointer);
                lv_obj_set_width(m_slider_container_list[i].slider, 200);

                // Label value
                m_slider_container_list[i].value_label = lv_label_create(m_slider_container_list[i].container, NULL);
                //lv_obj_align(m_slider_container_list[i].label, m_slider_container_list[i].slider, LV_ALIGN_OUT_LEFT_MID,0,0);
                snprintf(buf, 6, "%u", slider_value);
                lv_label_set_text(m_slider_container_list[i].value_label, buf);

        }
}

void gui_ctrls_profile_delete_button_event(lv_obj_t * const p_object,
                                           lv_event_t const event)
{
        lv_coord_t const msg_box_width = 200;
        lv_obj_t * p_parent;

        if ((LV_EVENT_CLICKED == event) &&
            (LV_BTN_STATE_REL == lv_btn_get_state(p_object))) {

                p_parent = lv_obj_get_parent(p_object);
                m_p_msg_box = lv_mbox_create(p_parent, NULL);
                lv_mbox_set_text(m_p_msg_box, MESSAGE_TEXT_DELETE_PROFILE);
                lv_mbox_add_btns(m_p_msg_box, m_delete_msg_box_options);
                lv_obj_set_width(m_p_msg_box, msg_box_width);
                lv_obj_set_event_cb(m_p_msg_box, delete_profile_msb_box_button_event_cb);
                lv_obj_align(m_p_msg_box, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
        }
}

void gui_ctrls_profile_select_dd_event(lv_obj_t * p_object,
                                       lv_event_t const event)
{
        char buffer[16];
        reflow_profile_t profile;
        bool success = true;
        if (LV_EVENT_RELEASED == event) {
                lv_ddlist_get_selected_str(p_object, buffer, sizeof(buffer));
                success = reflow_profile_load(buffer, &profile);

                if (success) {
                        printf("Reflow profile in use is %s\n", profile.name);
                        success = reflow_profile_use(profile.name);
                }
        }

}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

static void textfield_cb(lv_obj_t * const p_object, lv_event_t const event)
{
        lv_obj_t * keyboard;
        char const * text_area_text = NULL;

        if (LV_EVENT_FOCUSED == event) {
                text_area_text = lv_ta_get_text(p_object);
                memcpy(m_profile_name_buffer, text_area_text, 16);

                //lv_win_scroll_ver(parent, 200);
                keyboard = lv_kb_create(lv_scr_act(), NULL);
                lv_obj_set_pos(keyboard, 5, 90);
                lv_obj_set_size(keyboard, LV_HOR_RES - 10, 140);

                lv_kb_set_ta(keyboard, p_object);
                lv_kb_set_cursor_manage(keyboard, true);
                lv_obj_set_event_cb(keyboard, keyboard_cb);
                lv_obj_set_hidden(m_window_close_button, true);
        }
}

static void keyboard_cb(lv_obj_t * p_object, lv_event_t const event)
{
        lv_obj_t * keyboard = p_object;
        lv_obj_t * text_field;
        char const * profile_name = NULL;

        lv_kb_def_event_cb(keyboard, event);

        if(event == LV_EVENT_APPLY)
        {
                text_field = lv_kb_get_ta(keyboard);
                profile_name = lv_ta_get_text(text_field);
                memcpy(m_buffer_profile.name, profile_name, 16);
                lv_obj_del(keyboard);
                keyboard = NULL;
                lv_obj_set_hidden(m_window_close_button, false);
        }

        if(event == LV_EVENT_CANCEL)
        {
                memcpy(m_buffer_profile.name, m_profile_name_buffer, 16);
                text_field = lv_kb_get_ta(keyboard);
                lv_ta_set_text(text_field, m_profile_name_buffer);

                lv_obj_del(keyboard);
                keyboard = NULL;
                lv_obj_set_hidden(m_window_close_button, false);
        }
}

static void save_new_profile_cb(lv_obj_t * p_object, lv_event_t const event)
{
        bool success = true;

        // This type of object seems to not generate LV_EVENT_CLICKED on click
        if (LV_EVENT_RELEASED == event) {

                success = reflow_profile_save(&m_buffer_profile);

                if (success) {
                        update_profiles_dropdown();
                }

                if (!success) {
                        //TODO: there was a problem while saving new profile
                }

                lv_win_close_event_cb(p_object, event);

        }
}

static void save_existing_profile_cb(lv_obj_t * p_object, lv_event_t const event)
{
        bool success = true;
        reflow_profile_t current_profile;

        // This type of object seems to not generate LV_EVENT_CLICKED on click
        if (LV_EVENT_RELEASED == event) {

                success = reflow_profile_save(&m_buffer_profile);

                if (success) {
                        success = reflow_profile_get_current(&current_profile);
                }

                // Delete old profile version if name has changed.
                if ((success) &&
                    (0 != strcmp(current_profile.name, m_buffer_profile.name))) {
                        success = reflow_profile_delete(current_profile.name);
                }

                if (success) {
                        success = reflow_profile_use(m_buffer_profile.name);
                }

                if (success) {
                        update_profiles_dropdown();
                }

                if (!success) {
                        //TODO: there was a problem while saving edited profile
                }

                lv_win_close_event_cb(p_object, event);
        }
}

static uint8_t get_slider_index_from_pointer(lv_obj_t const * const p_object)
{
        uint8_t index = 0xFF;
        bool found = false;
        uint8_t i;

        if (NULL != p_object) {
                for (i = 0; (edit_prof_cont_list_len > i) && (!found); ++i) {
                        if (p_object == m_slider_container_list[i].slider) {
                                found = true;
                                index = i;
                        }
                }
        }

        return index;
}

static void profile_slider_changed_cb(lv_obj_t * const p_object,
                                      lv_event_t const event)
{
        uint8_t slider_index;
        int16_t slider_value;
        size_t offset;
        char buf[4];

        if (LV_EVENT_VALUE_CHANGED == event) {
                slider_value = lv_slider_get_value(p_object);
                slider_index = get_slider_index_from_pointer(p_object);
                offset = m_slider_container_list[slider_index].offset;
                *(((uint8_t *)&m_buffer_profile) + offset) = slider_value;

                snprintf(buf, 4, "%u", slider_value);

                if (0xFF != slider_index) {

                        lv_label_set_text(m_slider_container_list[slider_index].value_label, buf);
                }
        }

}

static void delete_profile_msb_box_button_event_cb(lv_obj_t * const p_object,
                                                   lv_event_t const event)
{
        char buffer[16];
        bool success;
        char * p_profiles_list = NULL;
        char p_first_name[REFLOW_PROFILE_NAME_LEN_MAX];
        size_t profiles_list_size;
        size_t buffer_size = sizeof(buffer);

        if (event == LV_EVENT_VALUE_CHANGED) {

                char const * text = lv_mbox_get_active_btn_text(p_object);

                if (0 == strcmp(text, BUTTON_TEXT_DELETE)) {
                        lv_ddlist_get_selected_str(p_dropdown, buffer, buffer_size);

                        success = reflow_profile_delete(buffer);

                        if (success) {
                                success = reflow_profile_get_profiles_list(
                                         &p_profiles_list, &profiles_list_size);
                        }

                        if (success) {
                                success = helper_get_first_name(
                                                 p_profiles_list, p_first_name);
                        }

                        if (success) {
                                reflow_profile_use(p_first_name);
                        }

                        update_profiles_dropdown();
                        lv_mbox_start_auto_close(m_p_msg_box, 0);

                } else if (0 == strcmp(text, BUTTON_TEXT_CANCEL)) {
                        lv_mbox_start_auto_close(m_p_msg_box, 0);
                }
        }
}

static bool helper_get_list_len(char const * const p_profiles_list,
                                uint32_t * const p_len)
{
        bool success = (NULL != p_profiles_list) && (NULL != p_len);
        bool needs_free = false;
        uint32_t list_len = 0;
        char * p_buffer_list = NULL;
        char * ptr = NULL;
        size_t const profiles_list_size = sizeof(char) *
                (strlen(p_profiles_list) + 1);

        if (success) {
                p_buffer_list = pvPortMalloc(profiles_list_size);

                success = (NULL != p_buffer_list);
        }

        if (success) {
                needs_free = true;

                strncpy(p_buffer_list, p_profiles_list, profiles_list_size);
                ptr = strtok(p_buffer_list, "\n");

                while (ptr != NULL) {
                        list_len++;
                        ptr = strtok(NULL, "\n");
                }

                *p_len = list_len;
        }

        if (needs_free) {
                vPortFree(p_buffer_list);
                p_buffer_list = NULL;
        }

        return success;
}


static bool helper_get_first_name(char const * const p_profiles_list,
                                  char * const p_first_name)
{
        bool success = (NULL != p_profiles_list);
        bool needs_free = false;
        char * p_buffer_list = NULL;
        char * ptr = NULL;
        size_t const profiles_list_size = sizeof(char) *
                                          (strlen(p_profiles_list) + 1);

        if (success) {
                p_buffer_list = pvPortMalloc(profiles_list_size);

                success = (NULL != p_buffer_list);
        }

        if (success) {
                needs_free = true;

                strncpy(p_buffer_list, p_profiles_list, profiles_list_size);
                ptr = strtok(p_buffer_list, "\n");

                strncpy(p_first_name, ptr, REFLOW_PROFILE_NAME_LEN_MAX);
        }

        if (needs_free) {
                vPortFree(p_buffer_list);
                p_buffer_list = NULL;
        }

        return success;
}

static bool helper_get_position_at_list(char const * const p_profiles_list,
                                        char const * const p_name,
                                        uint32_t * const p_selected)
{
        bool success = ((NULL != p_profiles_list) && (NULL != p_name) &&
                        (NULL != p_selected));
        bool found = false;
        bool needs_free = false;
        char * p_buffer_list = NULL;
        char * ptr = NULL;
        uint32_t i = 0;
        size_t const profiles_list_size = sizeof(char) *
                                          (strlen(p_profiles_list) + 1);

        if (success) {
                p_buffer_list = pvPortMalloc(profiles_list_size);

                success = (NULL != p_buffer_list);
        }

        if (success) {
                needs_free = true;
                strncpy(p_buffer_list, p_profiles_list, profiles_list_size);

                ptr = strtok(p_buffer_list, "\n");

                while ((NULL != ptr) && (!found)) {

                        if (0 == strcmp(p_name,ptr)) {
                                found = true;
                                *p_selected = i;
                        }

                        i++;
                        ptr = strtok(NULL, "\n");
                }
                success = found;
        }

        if (needs_free) {
                vPortFree(p_buffer_list);
                p_buffer_list = NULL;
        }

        return success;
}

static void update_profiles_dropdown(void)
{
        char * p_profiles_list = NULL;
        bool needs_free = false;
        uint32_t selected_item;
        uint32_t counter;
        size_t profiles_size;
        reflow_profile_t current_reflow_profile;
        bool success = reflow_profile_get_profiles_list(&p_profiles_list,
                                                        &profiles_size);
        if (success) {
                needs_free = true;
                lv_ddlist_set_options(p_dropdown, p_profiles_list);

                success = reflow_profile_get_current(&current_reflow_profile);
        }

        if (success) {
                success = helper_get_position_at_list(p_profiles_list,
                                                      current_reflow_profile.name,
                                                      &selected_item);
        }

        if (success) {
                lv_ddlist_set_selected(p_dropdown, selected_item);
                success = helper_get_list_len(p_profiles_list, &counter);
        }

        if (success) {
                // Deactivate delete button if there is only one profile left
                if (1 >= counter) {
                        lv_btn_set_state(p_delete_button, LV_BTN_STATE_INA);
                } else {
                        lv_btn_set_state(p_delete_button, LV_BTN_STATE_REL);
                }

                if (REFLOW_PROFILES_MAX_PROFILES_CNT <= counter) {
                        lv_btn_set_state(p_new_button, LV_BTN_STATE_INA);
                } else {
                        lv_btn_set_state(p_new_button, LV_BTN_STATE_REL);
                }
        }

        if (needs_free) {
                vPortFree(p_profiles_list);
                p_profiles_list = NULL;
        }
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */
