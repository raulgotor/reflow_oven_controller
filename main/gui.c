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

static void ui_button_event(lv_obj_t * p_object, lv_event_t event);

static void ui_roller_event(lv_obj_t * p_object, lv_event_t event);

static void update_rollers();

static void label_refresher_task(void * p);

static void gui_configure_styles(void);

static void gui_configure_tab_1(void);

static void gui_configure_tab_2(void);

static void gui_configure_tab_3(void);

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
static lv_obj_t * p_lmeter;

static lv_obj_t * p_start_button_label;

// Buttons
static lv_obj_t * m_p_start_button;
//static lv_obj_t * m_p_stop_button;

// Other objects
static lv_obj_t * mp_chrt_1;
static lv_obj_t * p_lmeter;
static lv_obj_t * p_preheat_temp_roller;
static lv_obj_t * p_preheat_time_roller;
static lv_obj_t * p_reflow_temp_roller;
static lv_obj_t * p_reflow_time_roller;
static lv_obj_t * p_ramp_speed_roller;

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
static lv_obj_t * m_p_tab_3;

// Dimensions
static uint16_t const m_menu_bar_height = 30;
static uint16_t const m_chart_tick_space_width = 20;
static uint16_t const m_margin_left = 10;
static uint16_t const m_margin_right = 10;
static uint16_t const m_margin_top = 20;
static uint16_t const m_margin_bottom = m_menu_bar_height + m_chart_tick_space_width + 10;
static lv_coord_t const m_btn_vertical_separation = 10;
static int8_t const m_roller_margin = 10;


/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

bool ui_init(void)
{
        state_machine_state_text_t state = STATE_MACHINE_STATE_COUNT;
        bool success = state_machine_get_state(&state);

        if (success) {
                gui_configure_styles();
                gui_configure_main_scr();
                gui_configure_tab_1();
                gui_configure_tab_2();
                gui_configure_tab_3();
                // TODO: change this for a INIT_STATE macro pointing to IDLE
                gui_configure_buttons_for_state(STATE_MACHINE_STATE_IDLE);
        }

        return success;
}



bool gui_configure_main_scr(void)
{


        //lv_point_t const points[] = {{0,   210}, {320, 210}};
        lv_obj_t * p_scr = lv_scr_act();
        lv_obj_t * p_separation_line;
        lv_theme_t * p_theme;

        lv_task_create(label_refresher_task, 10, LV_TASK_PRIO_MID, NULL);

        // Theme

        p_theme = lv_theme_night_init(210, NULL);
        lv_theme_set_current(p_theme);

        mp_tabview = lv_tabview_create(p_scr, NULL);

        mp_tab_1 = lv_tabview_add_tab(mp_tabview, "Main");
        mp_tab_2 = lv_tabview_add_tab(mp_tabview, "Graph");
        m_p_tab_3 = lv_tabview_add_tab(mp_tabview, "Profile");

        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_BTN_BG, &m_style_tv_btn_bg);
        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_INDIC, &lv_style_plain);
        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_BTN_REL, &m_style_tv_btn_rel);
        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_BTN_PR, &m_style_tv_btn_pr);
        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_BTN_TGL_REL, &m_style_tv_btn_rel);
        lv_tabview_set_style(mp_tabview, LV_TABVIEW_STYLE_BTN_TGL_PR, &m_style_tv_btn_pr);

        lv_tabview_set_btns_pos(mp_tabview, LV_TABVIEW_BTNS_POS_TOP);

        // Separation line

      //  separation_line = lv_line_create(scr, NULL);
       // lv_line_set_points(separation_line, points, 2);
        //lv_line_set_style(separation_line, LV_LINE_STYLE_MAIN, &m_line_style);

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

void gui_configure_buttons_for_state(state_machine_state_text_t state) {

        switch (state) {

        // Intentionally fall through
        case STATE_MACHINE_STATE_HEATING:
        case STATE_MACHINE_STATE_SOAKING:
        case STATE_MACHINE_STATE_REFLOW:
        case STATE_MACHINE_STATE_DWELL:
                lv_label_set_text(p_start_button_label, LV_SYMBOL_STOP "Stop");
                break;

        case STATE_MACHINE_STATE_IDLE:
                lv_label_set_text(p_start_button_label, LV_SYMBOL_PLAY "Start");
                break;

        default:
                break;
        }
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

static void label_refresher_task(void * p)
{
        char str[10];
        int16_t temperature;
        //if (reflowState == REFLOW_STATE_RUNNING) {
        //        serie2->points[reflowTime] = tempCounter;
        //}

        lv_chart_refresh(mp_chrt_1);

        thermocouple_get_temperature(0, &temperature);
        sprintf(str, "%dº", temperature);
        lv_label_set_text(m_p_temp_label, str);
        lv_obj_align(m_p_temp_label, p_lmeter, LV_ALIGN_CENTER, 0, 0);
        lv_lmeter_set_value(p_lmeter, temperature);


        if (lv_obj_get_screen(m_p_temp_label)) {

        }
}

static void update_rollers()
{
        //    lv_roller_set_selected(preheat_temp_roller, my_profile.preheat_temp - MIN_PREHEAT_TEMP, false);
        //   lv_roller_set_selected(preheat_time_roller, (my_profile.preheat_time - MIN_PREHEAT_TIME) / 10, false);
        //  lv_roller_set_selected(reflow_temp_roller, my_profile.reflow_temp - MIN_REFLOW_TEMP, false);
        // lv_roller_set_selected(reflow_time_roller, (my_profile.reflow_time - MIN_REFLOW_TIME) / 10, false);
        //lv_roller_set_selected(ramp_speed_roller, my_profile.ramp_speed, false);
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

        lv_obj_t * stop_button_label;

        // Start button

        m_p_start_button = lv_btn_create(mp_tab_1, NULL);
        lv_obj_set_pos(m_p_start_button, 10, 10);
        lv_obj_set_size(m_p_start_button, 100, 100);
        lv_obj_set_event_cb(m_p_start_button, ui_button_event);

        // Stop button

        /*
        m_p_stop_button = lv_btn_create(mp_tab_1, NULL);
        lv_obj_set_size(m_p_stop_button, 100, m_menu_bar_height);
        stop_button_label = lv_label_create(m_p_stop_button, NULL);
        lv_label_set_text(stop_button_label, LV_SYMBOL_STOP "Stop");
        lv_obj_set_event_cb(m_p_stop_button, ui_button_event);
        */

        // Labels

        p_profile_label = lv_label_create(mp_tab_1, NULL);
        lv_label_set_text(p_profile_label, "PROFILE: Sn60Pb40");
        lv_label_set_style(p_profile_label, LV_LABEL_STYLE_MAIN, &m_style);

        p_start_button_label = lv_label_create(m_p_start_button, NULL);
        lv_label_set_text(p_start_button_label, LV_SYMBOL_PLAY "Run");

        m_p_temp_label = lv_label_create(mp_tab_1, NULL);
        lv_label_set_align(m_p_temp_label, LV_LABEL_ALIGN_CENTER);
        lv_label_set_text(m_p_temp_label, "193º");
        lv_label_set_style(m_p_temp_label, LV_LABEL_STYLE_MAIN, &m_big_style);

        // Gauge

        p_lmeter = lv_lmeter_create(mp_tab_1, NULL);
        lv_obj_set_pos(p_lmeter, 135, 10);
        lv_obj_set_size(p_lmeter, 140, 140);
        lv_lmeter_set_range(p_lmeter, 0, 300);
        lv_lmeter_set_scale(p_lmeter, 270, 54);
        lv_lmeter_set_value(p_lmeter, 183);

        //lv_obj_align(m_p_stop_button, m_p_start_button, LV_ALIGN_OUT_BOTTOM_LEFT, 0, btnVerticalSeparation);
        //lv_obj_align(p_profile_label, m_p_stop_button, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 41);
        lv_obj_align(p_profile_label, m_p_start_button, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 81);

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

        p_degree_label_1 = lv_label_create(m_p_tab_3, NULL);
        lv_label_set_text(p_degree_label_1, "ºC");

        p_degree_label_2 = lv_label_create(m_p_tab_3, p_degree_label_1);

        p_seconds_label_1 = lv_label_create(m_p_tab_3, NULL);
        lv_label_set_text(p_seconds_label_1, "s.");

        p_seconds_label_2 = lv_label_create(m_p_tab_3, p_seconds_label_1);

        p_degrees_seconds_label = lv_label_create(m_p_tab_3, NULL);
        lv_label_set_text(p_degrees_seconds_label, "ºC/s.");

        // Labels configuration: parameters

        p_preheat_temp_label = lv_label_create(m_p_tab_3, NULL);
        lv_label_set_text(p_preheat_temp_label, "Preheat");

        p_preheat_time_label = lv_label_create(m_p_tab_3, NULL);
        lv_label_set_text(p_preheat_time_label, "Time");

        p_reflow_temp_label = lv_label_create(m_p_tab_3, NULL);
        lv_label_set_text(p_reflow_temp_label, "Reflow");

        p_reflow_time_label = lv_label_create(m_p_tab_3, p_preheat_time_label);

        p_speed_label = lv_label_create(m_p_tab_3, NULL);
        lv_label_set_text(p_speed_label, "Speed");

        // Rollers configuration

        p_preheat_temp_roller = lv_roller_create(m_p_tab_3, NULL);
        lv_roller_set_options(p_preheat_temp_roller,
                              "140\n141\n142\n143\n144\n145\n146\n147\n148\n149\n150\n151\n152\n153\n154\n155\n156\n157\n158\n159\n160\n161\n162\n163\n164\n165\n166\n167\n168\n169\n170\n171\n172\n173\n174\n175\n176\n177\n178\n179\n180\n181\n182\n183\n184\n185\n186\n187\n188\n189\n190\n191\n192\n193\n194\n195\n196\n197\n198\n199\n200",
                              LV_ROLLER_MODE_NORMAL);
        lv_obj_set_event_cb(p_preheat_temp_roller, ui_roller_event);

        p_preheat_time_roller = lv_roller_create(m_p_tab_3, NULL);
        lv_roller_set_options(p_preheat_time_roller,
                              "0\n10\n20\n30\n40\n50\n60\n70\n80\n90\n100\n110\n120",
                              LV_ROLLER_MODE_NORMAL);
        lv_obj_set_event_cb(p_preheat_time_roller, ui_roller_event);

        p_reflow_temp_roller = lv_roller_create(m_p_tab_3, NULL);
        lv_roller_set_options(p_reflow_temp_roller,
                              "200\n201\n202\n203\n204\n205\n206\n207\n208\n209\n210\n211\n212\n213\n214\n215\n216\n217\n218\n219\n220\n221\n222\n223\n224\n225\n226\n227\n228\n229\n230",
                              LV_ROLLER_MODE_NORMAL);
        lv_obj_set_event_cb(p_reflow_temp_roller, ui_roller_event);


        p_reflow_time_roller = lv_roller_create(m_p_tab_3, NULL);
        lv_roller_set_options(p_reflow_time_roller,
                              "30\n40\n50\n60\n70\n80\n90\n100\n110\n120",
                              LV_ROLLER_MODE_NORMAL);
        lv_obj_set_event_cb(p_reflow_time_roller, ui_roller_event);

        p_ramp_speed_roller = lv_roller_create(m_p_tab_3, NULL);
        lv_roller_set_options(p_ramp_speed_roller,
                              "1\n2\n3\n4\n5",
                              LV_ROLLER_MODE_NORMAL);

        lv_obj_align(p_preheat_temp_roller, m_p_tab_3, LV_ALIGN_IN_LEFT_MID, m_roller_margin, -30);
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

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

static void ui_roller_event(lv_obj_t * p_object, lv_event_t event)
{
        uint16_t const selection = lv_roller_get_selected(p_object);

        reflow_profile_t my_profile;


        if (p_object == p_preheat_temp_roller) {
                my_profile.preheat_temperature = selection + REFLOW_PROFILE_PREHEAT_TEMPERATURE_MIN;

        } else if (p_object == p_preheat_time_roller) {
                my_profile.soak_time_ms = selection * 10 + REFLOW_PROFILE_SOAK_TIME_MIN;
        } else if (p_object == p_reflow_temp_roller) {
                my_profile.reflow_temperature = selection + REFLOW_PROFILE_REFLOW_TEMPERATURE_MIN;
        } else if (p_object == p_reflow_time_roller) {
                my_profile.dwell_time_ms = selection * 10 + REFLOW_PROFILE_REFLOW_TIME_MIN;
        } else if (p_object == p_ramp_speed_roller) {
                my_profile.ramp_speed = selection;
        }

        //profileUpdater();

        //lv_chart_set_points(chrt1, serie2, prof1array);

        //save_profile();


        lv_chart_refresh(mp_chrt_1);
}

static void ui_button_event(lv_obj_t * p_object, lv_event_t event)
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

