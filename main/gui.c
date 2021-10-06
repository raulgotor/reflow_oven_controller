/*!
 *******************************************************************************
 * @file user_interface.c
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

/*
 *******************************************************************************
 * #include Statements                                                         *
 *******************************************************************************
 */

#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "state_machine/states/state_machine_states.h"
#include "state_machine/state_machine.h"
#include "elegance4.c"
#include "esp_log.h"


#include "reflow_profile.h"
#include "thermocouple.h"
#include "gui.h"


/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */

#define TAG                                     __FILENAME__

#define BUTTON_TEXT_NEW                         "New"
#define BUTTON_TEXT_EDIT                        "Edit"
#define BUTTON_TEXT_DELETE                      "Delete"
#define BUTTON_TEXT_CANCEL                      "Cancel"
#define BUTTON_TEXT_START                       "Start"
#define BUTTON_TEXT_STOP                        "Stop"

#define LABEL_TEXT_PROFILE                      "Profile:"

#define MESSAGE_TEXT_DELETE_PROFILE             "Are you sure you want to delete this profile?"


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

static void gui_button_event(lv_obj_t * p_object, lv_event_t event);

static void ui_roller_event(lv_obj_t * p_object, lv_event_t event);

static void gui_tab_profile_delete_event(lv_obj_t * const p_object, lv_event_t const event);

static void gui_tab_profile_delete_msg_box_event(lv_obj_t * const p_object, lv_event_t const event);

static void update_rollers();

static void label_refresher_task(void * p);

static void gui_configure_styles(void);

static void gui_configure_tab_1(void);

static void gui_configure_tab_2(void);

static void gui_configure_tab_3(void);

static void gui_configure_tab_4(void);


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

// Labels
static lv_obj_t * m_p_temp_label;
static lv_obj_t * p_profile_label;
static lv_obj_t * p_state_label;
static lv_obj_t * p_start_button_label;
static lv_obj_t * p_dropdown_label;
static lv_obj_t * p_new_button_label;
static lv_obj_t * p_edit_button_label;
static lv_obj_t * p_delete_button_label;

// Buttons
static lv_obj_t * m_p_start_button;
static lv_obj_t * p_new_button;
static lv_obj_t * p_edit_button;
static lv_obj_t * p_delete_button;
//static lv_obj_t * m_p_stop_button;

// Other objects
static lv_obj_t * mp_chrt_1;
static lv_obj_t * p_lmeter;
static lv_obj_t * p_preheat_temp_roller;
static lv_obj_t * p_preheat_time_roller;
static lv_obj_t * p_reflow_temp_roller;
static lv_obj_t * p_reflow_time_roller;
static lv_obj_t * p_ramp_speed_roller;
static lv_obj_t * p_dropdown;
static lv_obj_t * p_msg_box;

// Styles
static lv_style_t m_style;
static lv_style_t m_line_style;
static lv_style_t m_big_style;
static lv_style_t m_chart_style;
static lv_style_t m_style_tv_btn_bg;
static lv_style_t m_style_tv_btn_rel;
static lv_style_t m_style_tv_btn_pr;

// Views
static lv_obj_t * mp_tabview;
static lv_obj_t * mp_tab_1;
static lv_obj_t * mp_tab_2;
static lv_obj_t * mp_tab_3;
static lv_obj_t * mp_tab_4;

// Dimensions
static uint16_t const m_chart_tick_space_width = 20;
static int8_t const m_roller_margin = 10;

static const char * m_msg_box_delete_options[] = {"Delete", "Cancel", ""};

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

bool gui_init(void)
{
        state_machine_state_text_t state = STATE_MACHINE_STATE_COUNT;
        bool success = state_machine_get_state(&state);
        reflow_profile_t profile;

        if (success) {
                gui_configure_styles();
                gui_configure_main_scr();
                gui_configure_tab_1();
                gui_configure_tab_2();
                //gui_configure_tab_3();
                gui_configure_tab_4();
                // TODO: change this for a INIT_STATE macro pointing to IDLE
                gui_configure_buttons_for_state(STATE_MACHINE_STATE_IDLE);
        }

        if (success) {
                //TODO: remove once we have a proper profile loading on boot
                reflow_profile_get_current(&profile);
                gui_configure_for_profile(profile);


                gui_update_profiles_dropdown();

        }

        return success;
}


bool gui_configure_main_scr(void)
{
        lv_obj_t * p_scr = lv_scr_act();
        lv_theme_t * p_theme;

        lv_task_create(label_refresher_task, 10, LV_TASK_PRIO_MID, NULL);

        // Theme

        p_theme = lv_theme_night_init(210, NULL);
        lv_theme_set_current(p_theme);

        mp_tabview = lv_tabview_create(p_scr, NULL);

        mp_tab_1 = lv_tabview_add_tab(mp_tabview, "Main");
        mp_tab_2 = lv_tabview_add_tab(mp_tabview, "Graph");
        //mp_tab_3 = lv_tabview_add_tab(mp_tabview, "Profile");
        mp_tab_4 = lv_tabview_add_tab(mp_tabview, "Profile");

        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_BTN_BG, &m_style_tv_btn_bg);
        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_INDIC, &lv_style_plain);
        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_BTN_REL, &m_style_tv_btn_rel);
        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_BTN_PR, &m_style_tv_btn_pr);
        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_BTN_TGL_REL, &m_style_tv_btn_rel);
        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_BTN_TGL_PR, &m_style_tv_btn_pr);

        lv_tabview_set_btns_pos(mp_tabview, LV_TABVIEW_BTNS_POS_TOP);

        return false;
}

bool gui_configure_splash_src()
{
        /*   lv_obj_t * scr = lv_scr_act();

           static lv_style_t style_screen;
           lv_style_copy(&style_screen, &lv_style_plain);
           style_screen.body.main_color = lv_color_hsv_to_rgb(0, 0, 100);
           style_screen.body.grad_color = lv_color_hsv_to_rgb(0, 0, 100);
           lv_obj_set_style(scr, &style_screen);

           lv_disp_load_scr(scr);


           lv_obj_t * logo = lv_img_create(scr, NULL);
           //lv_obj_align(&logo, scr, LV_ALIGN, 0, 0);

           lv_obj_set_x(logo, lv_obj_get_width(scr) / 2 - lv_obj_get_width(logo) / 2);
           lv_anim_t animation;

           lv_anim_set_exec_cb(&animation, logo, lv_obj_set_y);
           lv_anim_set_time(&animation, 2000, 0);

           animation.start = -lv_obj_get_height(logo);
           animation.end = lv_obj_get_height(scr) / 2 - lv_obj_get_height(logo) / 2;
           animation.playback = 0;

           lv_anim_set_path_cb(&animation, lv_anim_path_ease_in_out);
           //lv_anim_set_ready_cb(&animation, intro_ready);
           lv_anim_create(&animation);

           while (!intro_finished) {
                   lv_task_handler();

           };
           const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;

           vTaskDelay(xDelay);
   */
        return false;
}

void gui_configure_buttons_for_state(state_machine_state_text_t const state)
{
        char const * const p_state_str = state_machine_get_state_string(state);

        if (NULL == p_state_str) {
                // Code style exception for readability
                return;
        }

        switch (state) {
        // Intentionally fall through
        case STATE_MACHINE_STATE_HEATING:
        case STATE_MACHINE_STATE_SOAKING:
        case STATE_MACHINE_STATE_REFLOW:
        case STATE_MACHINE_STATE_DWELL:
                lv_label_set_text(p_start_button_label,
                                  LV_SYMBOL_STOP BUTTON_TEXT_STOP);
                break;
        case STATE_MACHINE_STATE_IDLE:
                lv_label_set_text(p_start_button_label,
                                  LV_SYMBOL_PLAY BUTTON_TEXT_START);
                break;
        case STATE_MACHINE_STATE_COOLING:
                break;
        default:
                break;
        }

        lv_label_set_text(p_state_label, p_state_str);
}

void gui_configure_for_profile(reflow_profile_t const profile)
{

        int16_t const meter_value_max = (int16_t)profile.reflow_temperature;
        int16_t const meter_angle = 270;
        int16_t const meter_line_count = 54;

        lv_lmeter_set_range(p_lmeter, 0, meter_value_max);
        lv_lmeter_set_scale(p_lmeter, meter_angle, meter_line_count);
}

void gui_update_profiles_dropdown(void)
{
        char * profiles_list = NULL;
        uint32_t counter = 0;
        size_t profiles_size;
        size_t i;
        bool needs_free = false;
        bool success = reflow_profile_get_profiles_list(&profiles_list,
                                                        &profiles_size);
        if (success) {
                needs_free = true;
                lv_ddlist_set_options(p_dropdown, profiles_list);

                for (i = 0; profiles_size > i; i++) {
                        if ('\n' == profiles_list[i]) {
                                counter++;
                        }
                }
        }

        if (needs_free) {
                vPortFree(profiles_list);
                profiles_list = NULL;
        }

        if (success) {
                // Deactivate delete button if there is only one profile left
                if (1 >= counter) {
                        lv_btn_set_state(p_delete_button, LV_BTN_STATE_INA);
                } else {
                        lv_btn_set_state(p_delete_button, LV_BTN_STATE_REL);
                }
        }
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

static void label_refresher_task(void * p_parameters)
{
        char temperature_str[10];
        int16_t temperature;
        bool success;

        lv_chart_refresh(mp_chrt_1);

        success = thermocouple_get_temperature(0, &temperature);

        if (success) {
                sprintf(temperature_str, "%dº", temperature);
                lv_label_set_text(m_p_temp_label, temperature_str);
                lv_lmeter_set_value(p_lmeter, temperature);
        }

        //TODO: this belongs to somewhere else
        lv_obj_align(m_p_temp_label, p_lmeter, LV_ALIGN_CENTER, 0, 0);
}

static void gui_configure_styles(void)
{
        lv_style_copy(&m_style, &lv_style_plain);
        m_style.body.main_color = lv_color_hsv_to_rgb(210, 11, 30);
        m_style.body.grad_color = lv_color_hsv_to_rgb(210, 11, 30);
        m_style.text.color = LV_COLOR_WHITE;

        lv_obj_set_style(lv_scr_act(), &m_style);

        lv_style_copy(&m_chart_style, &m_style);
        m_chart_style.text.color = LV_COLOR_GRAY;
        m_chart_style.text.font = &lv_font_unscii_8;

        lv_style_copy(&m_line_style, &lv_style_plain);
        m_line_style.line.color = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF);
        m_line_style.line.width = 1;
        m_line_style.line.rounded = 1;

        lv_style_copy(&m_big_style, &lv_style_plain);
        m_big_style.body.main_color = lv_color_hsv_to_rgb(210, 11, 30);
        m_big_style.body.grad_color = lv_color_hsv_to_rgb(210, 11, 30);
        m_big_style.text.color = LV_COLOR_WHITE;
        m_big_style.text.font = &elegance4;

        lv_style_copy(&m_style_tv_btn_bg, &lv_style_plain);
        m_style_tv_btn_bg.body.main_color = lv_color_hex(0x43484c);
        m_style_tv_btn_bg.body.grad_color = lv_color_hex(0x43484c);
        m_style_tv_btn_bg.body.padding.top = 0;
        m_style_tv_btn_bg.body.padding.bottom = 0;

        lv_style_copy(&m_style_tv_btn_rel, &lv_style_btn_rel);
        m_style_tv_btn_rel.body.opa = LV_OPA_TRANSP;
        m_style_tv_btn_rel.body.border.width = 0;

        lv_style_copy(&m_style_tv_btn_pr, &lv_style_btn_pr);
        m_style_tv_btn_pr.body.radius = 0;
        m_style_tv_btn_pr.body.opa = LV_OPA_50;
        m_style_tv_btn_pr.body.main_color = LV_COLOR_WHITE;
        m_style_tv_btn_pr.body.grad_color = LV_COLOR_WHITE;
        m_style_tv_btn_pr.body.border.width = 0;
        m_style_tv_btn_pr.text.color = LV_COLOR_GRAY;
}

static void gui_configure_tab_1(void)
{
        // Start button

        m_p_start_button = lv_btn_create(mp_tab_1, NULL);
        lv_obj_set_pos(m_p_start_button, 10, 10);
        lv_obj_set_size(m_p_start_button, 100, 100);
        lv_obj_set_event_cb(m_p_start_button, gui_button_event);

        // Labels

        p_profile_label = lv_label_create(mp_tab_1, NULL);
        lv_label_set_text(p_profile_label, "PROFILE: Sn60Pb40");
        lv_label_set_style(p_profile_label, LV_LABEL_STYLE_MAIN, &m_style);

        p_state_label = lv_label_create(mp_tab_1, NULL);
        lv_label_set_style(p_state_label, LV_LABEL_STYLE_MAIN, &m_style);

        p_start_button_label = lv_label_create(m_p_start_button, NULL);
        lv_label_set_text(p_start_button_label, LV_SYMBOL_PLAY "Run");

        m_p_temp_label = lv_label_create(mp_tab_1, NULL);
        lv_label_set_align(m_p_temp_label, LV_LABEL_ALIGN_CENTER);
        lv_label_set_style(m_p_temp_label, LV_LABEL_STYLE_MAIN, &m_big_style);

        // Gauge

        p_lmeter = lv_lmeter_create(mp_tab_1, NULL);
        lv_obj_set_pos(p_lmeter, 135, 10);
        lv_obj_set_size(p_lmeter, 140, 140);

        lv_obj_align(p_profile_label, m_p_start_button, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
        lv_obj_align(p_state_label, p_profile_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

        lv_obj_align(m_p_temp_label, p_lmeter, LV_ALIGN_CENTER, 0, 0);
        //   lv_obj_align(profileLabel, chrt1, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
        //   lv_obj_align(profileLabel, chrt1, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
}

static void gui_configure_tab_2(void)
{

        lv_chart_series_t * serie;
        lv_chart_series_t * serie2;

        mp_chrt_1 = lv_chart_create(mp_tab_2, NULL);

        lv_chart_set_margin(mp_chrt_1, m_chart_tick_space_width);
        //  lv_obj_set_pos(chrt1, m_margin_left, m_margin_top);
        //   lv_obj_set_size(chrt1,lv_obj_get_width(scr) - m_margin_left - m_margin_right,lv_obj_get_height(scr) - m_margin_top - m_margin_bottom);

        serie = lv_chart_add_series(mp_chrt_1, LV_COLOR_RED);
        serie2 = lv_chart_add_series(mp_chrt_1, LV_COLOR_GREEN);


        const char * list_of_values = "4\n8\n12min";
        lv_chart_set_x_tick_texts(mp_chrt_1, list_of_values, 4, LV_CHART_AXIS_DRAW_LAST_TICK);


        lv_chart_set_point_count(mp_chrt_1, 500);
        //     lv_chart_set_points(chrt1, serie, prof1array);

        lv_chart_set_range(mp_chrt_1, 0, 300);
        lv_chart_set_x_tick_length(mp_chrt_1, 5, 3);
        lv_obj_set_size(mp_chrt_1, 280, 140);
        lv_chart_set_type(mp_chrt_1, LV_CHART_TYPE_LINE);
        //    lv_chart_set_style(chrt1, LV_CHART_)

        lv_chart_set_style(mp_chrt_1, LV_CHART_STYLE_MAIN, &m_chart_style);

}

static void gui_configure_tab_3(void)
{

        lv_obj_t * p_preheat_temp_label;
        lv_obj_t * p_preheat_time_label;
        lv_obj_t * p_reflow_temp_label;
        lv_obj_t * p_reflow_time_label;
        lv_obj_t * p_speed_label;
        lv_obj_t * p_degree_label_1;
        lv_obj_t * p_degree_label_2;
        lv_obj_t * p_seconds_label_1;
        lv_obj_t * p_seconds_label_2;
        lv_obj_t * p_degrees_seconds_label;

        // Labels configuration: units

        p_degree_label_1 = lv_label_create(mp_tab_3, NULL);
        lv_label_set_text(p_degree_label_1, "ºC");

        p_degree_label_2 = lv_label_create(mp_tab_3, p_degree_label_1);

        p_seconds_label_1 = lv_label_create(mp_tab_3, NULL);
        lv_label_set_text(p_seconds_label_1, "s.");

        p_seconds_label_2 = lv_label_create(mp_tab_3, p_seconds_label_1);

        p_degrees_seconds_label = lv_label_create(mp_tab_3, NULL);
        lv_label_set_text(p_degrees_seconds_label, "ºC/s.");

        // Labels configuration: parameters

        p_preheat_temp_label = lv_label_create(mp_tab_3, NULL);
        lv_label_set_text(p_preheat_temp_label, "Preheat");

        p_preheat_time_label = lv_label_create(mp_tab_3, NULL);
        lv_label_set_text(p_preheat_time_label, "Time");

        p_reflow_temp_label = lv_label_create(mp_tab_3, NULL);
        lv_label_set_text(p_reflow_temp_label, "Reflow");

        p_reflow_time_label = lv_label_create(mp_tab_3, p_preheat_time_label);

        p_speed_label = lv_label_create(mp_tab_3, NULL);
        lv_label_set_text(p_speed_label, "Speed");

        // Rollers configuration

        p_preheat_temp_roller = lv_roller_create(mp_tab_3, NULL);
        lv_roller_set_options(p_preheat_temp_roller,
                              "140\n141\n142\n143\n144\n145\n146\n147\n148\n149\n150\n151\n152\n153\n154\n155\n156\n157\n158\n159\n160\n161\n162\n163\n164\n165\n166\n167\n168\n169\n170\n171\n172\n173\n174\n175\n176\n177\n178\n179\n180\n181\n182\n183\n184\n185\n186\n187\n188\n189\n190\n191\n192\n193\n194\n195\n196\n197\n198\n199\n200",
                              LV_ROLLER_MODE_NORMAL);
        lv_obj_set_event_cb(p_preheat_temp_roller, ui_roller_event);

        p_preheat_time_roller = lv_roller_create(mp_tab_3, NULL);
        lv_roller_set_options(p_preheat_time_roller,
                              "0\n10\n20\n30\n40\n50\n60\n70\n80\n90\n100\n110\n120",
                              LV_ROLLER_MODE_NORMAL);
        lv_obj_set_event_cb(p_preheat_time_roller, ui_roller_event);

        p_reflow_temp_roller = lv_roller_create(mp_tab_3, NULL);
        lv_roller_set_options(p_reflow_temp_roller,
                              "200\n201\n202\n203\n204\n205\n206\n207\n208\n209\n210\n211\n212\n213\n214\n215\n216\n217\n218\n219\n220\n221\n222\n223\n224\n225\n226\n227\n228\n229\n230",
                              LV_ROLLER_MODE_NORMAL);
        lv_obj_set_event_cb(p_reflow_temp_roller, ui_roller_event);


        p_reflow_time_roller = lv_roller_create(mp_tab_3, NULL);
        lv_roller_set_options(p_reflow_time_roller,
                              "30\n40\n50\n60\n70\n80\n90\n100\n110\n120",
                              LV_ROLLER_MODE_NORMAL);
        lv_obj_set_event_cb(p_reflow_time_roller, ui_roller_event);

        p_ramp_speed_roller = lv_roller_create(mp_tab_3, NULL);
        lv_roller_set_options(p_ramp_speed_roller,
                              "1\n2\n3\n4\n5",
                              LV_ROLLER_MODE_NORMAL);

        lv_obj_align(p_preheat_temp_roller, mp_tab_3, LV_ALIGN_IN_LEFT_MID, m_roller_margin, -30);
        lv_obj_align(p_preheat_time_roller, p_preheat_temp_roller, LV_ALIGN_OUT_RIGHT_MID, m_roller_margin, 0);
        lv_obj_align(p_reflow_temp_roller, p_preheat_time_roller, LV_ALIGN_OUT_RIGHT_MID, m_roller_margin, 0);
        lv_obj_align(p_reflow_time_roller, p_reflow_temp_roller, LV_ALIGN_OUT_RIGHT_MID, m_roller_margin, 0);
        lv_obj_align(p_ramp_speed_roller, p_reflow_time_roller, LV_ALIGN_OUT_RIGHT_MID, m_roller_margin, 0);

        lv_obj_align(p_preheat_temp_label, p_preheat_temp_roller, LV_ALIGN_OUT_TOP_MID, 0, 0);
        lv_obj_align(p_degree_label_1, p_preheat_temp_roller, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

        lv_obj_align(p_preheat_time_label, p_preheat_time_roller, LV_ALIGN_OUT_TOP_MID, 0, 0);
        lv_obj_align(p_seconds_label_1, p_preheat_time_roller, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

        lv_obj_align(p_reflow_time_label, p_reflow_time_roller, LV_ALIGN_OUT_TOP_MID, 0, 0);
        lv_obj_align(p_seconds_label_2, p_reflow_time_roller, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

        lv_obj_align(p_reflow_temp_label, p_reflow_temp_roller, LV_ALIGN_OUT_TOP_MID, 0, 0);
        lv_obj_align(p_degree_label_2, p_reflow_temp_roller, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

        lv_obj_align(p_speed_label, p_ramp_speed_roller, LV_ALIGN_OUT_TOP_MID, 0, 0);
        lv_obj_align(p_degrees_seconds_label, p_ramp_speed_roller, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);


        lv_obj_set_event_cb(p_ramp_speed_roller, ui_roller_event);

}

static void gui_configure_tab_4(void)
{
        uint8_t const button_width = 85;
        uint8_t const button_height = 50;

        // Configure buttons

        p_new_button = lv_btn_create(mp_tab_4, NULL);
        lv_obj_set_size(p_new_button, button_width, button_height);

        p_edit_button = lv_btn_create(mp_tab_4, NULL);
        lv_obj_set_size(p_edit_button, button_width, button_height);

        p_delete_button = lv_btn_create(mp_tab_4, NULL);
        lv_obj_set_size(p_delete_button, button_width, button_height);

        // Add labels

        p_new_button_label = lv_label_create(p_new_button, NULL);
        lv_label_set_text(p_new_button_label, LV_SYMBOL_OK BUTTON_TEXT_NEW);
        p_edit_button_label = lv_label_create(p_edit_button, NULL);
        lv_label_set_text(p_edit_button_label, LV_SYMBOL_EDIT BUTTON_TEXT_EDIT);
        p_delete_button_label = lv_label_create(p_delete_button, NULL);
        lv_label_set_text(p_delete_button_label, LV_SYMBOL_TRASH BUTTON_TEXT_DELETE);

        // Configure dropdown

        p_dropdown_label = lv_label_create(mp_tab_4, NULL);
        lv_label_set_text(p_dropdown_label, LABEL_TEXT_PROFILE);

        p_dropdown = lv_ddlist_create(mp_tab_4, NULL);

        lv_ddlist_set_sb_mode(p_dropdown, LV_SB_MODE_ON);
        lv_ddlist_set_fix_height(p_dropdown, 150);
        lv_obj_align(p_dropdown, p_dropdown, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        // Layout

        lv_obj_align(p_edit_button, mp_tab_4, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
        lv_obj_align(p_new_button, p_edit_button, LV_ALIGN_OUT_LEFT_MID, -15, 0);
        lv_obj_align(p_delete_button, p_edit_button, LV_ALIGN_OUT_RIGHT_MID, 15, 0);

        // Set button event callbacks
        //lv_obj_set_event_cb(p_new_button, gui_tab_profile_delete_event);
        //lv_obj_set_event_cb(p_edit_button, gui_tab_profile_delete_event);
        lv_obj_set_event_cb(p_delete_button, gui_tab_profile_delete_event);
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

static void gui_tab_profile_delete_event(lv_obj_t * const p_object,
                                         lv_event_t const event)
{
        lv_coord_t const msg_box_width = 200;

        if (LV_EVENT_CLICKED == event) {

                p_msg_box = lv_mbox_create(mp_tab_4, NULL);
                lv_mbox_set_text(p_msg_box, MESSAGE_TEXT_DELETE_PROFILE);
                lv_mbox_add_btns(p_msg_box, m_msg_box_delete_options);
                lv_obj_set_width(p_msg_box, msg_box_width);
                lv_obj_set_event_cb(p_msg_box, gui_tab_profile_delete_msg_box_event);
                lv_obj_align(p_msg_box, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
        }


}

static void gui_tab_profile_delete_msg_box_event(lv_obj_t * const p_object,
                                                 lv_event_t const event)
{
        char buffer[16];
        size_t buffer_size = sizeof(buffer);

        if (event == LV_EVENT_VALUE_CHANGED) {

                char const * text = lv_mbox_get_active_btn_text(p_object);

                if (0 == strcmp(text, BUTTON_TEXT_DELETE)) {
                        lv_ddlist_get_selected_str(p_dropdown, buffer, buffer_size);
                        (void)reflow_profile_delete(buffer);
                        gui_update_profiles_dropdown();
                        lv_mbox_start_auto_close(p_msg_box,0);
                } else if (0 == strcmp(text, BUTTON_TEXT_CANCEL)) {
                        lv_mbox_start_auto_close(p_msg_box,0);
                }
        }
}

static void gui_button_event(lv_obj_t * p_object, lv_event_t event)
{
        state_machine_data_t state_machine_event_data;
        state_machine_state_text_t state;
        bool success;

        if (LV_EVENT_CLICKED != event) {
                return;
        }

        if (m_p_start_button == p_object) {
                ESP_LOGI(TAG, "Start button pressed");
                success = state_machine_get_state(&state);

                if (!success) {
                        return;
                }

                switch (state) {

                case STATE_MACHINE_STATE_IDLE:
                        state_machine_event_data.user_action = STATE_MACHINE_ACTION_START;
                        break;

                // Intentionally fall-through
                case STATE_MACHINE_STATE_HEATING:
                case STATE_MACHINE_STATE_SOAKING:
                case STATE_MACHINE_STATE_REFLOW:
                case STATE_MACHINE_STATE_DWELL:
                        state_machine_event_data.user_action = STATE_MACHINE_ACTION_ABORT;
                        break;

                default:
                        return;
                }

                state_machine_send_event(STATE_MACHINE_EVENT_TYPE_ACTION,
                                         state_machine_event_data,
                                         portMAX_DELAY);
        }
}

