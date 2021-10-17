/*!
 *******************************************************************************
 * @file gui.c
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

#include "lvgl.h"
#include "elegance4.c"
#include "state_machine/states/state_machine_states.h"
#include "state_machine/state_machine.h"

#include "gui.h"
#include "gui/gui_ctrls/gui_ctrls_main.h"
#include "gui/gui_ctrls/gui_ctrls_profile.h"
#include "gui/gui_views/gui_views_main.h"
#include "gui/gui_views/gui_views_profile.h"

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

static void label_refresher_task(void * p_parameters);

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

static lv_obj_t * m_p_tabview;
static lv_obj_t * m_p_tab_1;
static lv_obj_t * m_p_tab_2;
static lv_obj_t * m_p_tab_3;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

void gui_init(void)
{
        lv_obj_t * p_scr = lv_scr_act();
        lv_theme_t * p_theme;

        lv_task_create(label_refresher_task, 10, LV_TASK_PRIO_MID, NULL);

        lv_style_copy(&m_style, &lv_style_plain);
        m_style.body.main_color = lv_color_hsv_to_rgb(210, 11, 30);
        m_style.body.grad_color = lv_color_hsv_to_rgb(210, 11, 30);
        m_style.text.color = LV_COLOR_WHITE;

        lv_obj_set_style(lv_scr_act(), &m_style);

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

        // Theme

        p_theme = lv_theme_night_init(210, NULL);
        lv_theme_set_current(p_theme);

        m_p_tabview = lv_tabview_create(p_scr, NULL);

        m_p_tab_1 = lv_tabview_add_tab(m_p_tabview, "Main");
        m_p_tab_2 = lv_tabview_add_tab(m_p_tabview, "Graph");
        m_p_tab_3 = lv_tabview_add_tab(m_p_tabview, "Profile");

        lv_tabview_set_style(m_p_tabview, LV_TABVIEW_STYLE_BTN_BG, &m_style_tv_btn_bg);
        lv_tabview_set_style(m_p_tabview, LV_TABVIEW_STYLE_INDIC, &lv_style_plain);
        lv_tabview_set_style(m_p_tabview, LV_TABVIEW_STYLE_BTN_REL, &m_style_tv_btn_rel);
        lv_tabview_set_style(m_p_tabview, LV_TABVIEW_STYLE_BTN_PR, &m_style_tv_btn_pr);
        lv_tabview_set_style(m_p_tabview, LV_TABVIEW_STYLE_BTN_TGL_REL, &m_style_tv_btn_rel);
        lv_tabview_set_style(m_p_tabview, LV_TABVIEW_STYLE_BTN_TGL_PR, &m_style_tv_btn_pr);

        lv_tabview_set_btns_pos(m_p_tabview, LV_TABVIEW_BTNS_POS_TOP);

        gui_views_main(m_p_tab_1);
        gui_views_profile(m_p_tab_3);

        gui_ctrls_main_init();
        gui_ctrls_profile_init();
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

static void label_refresher_task(void * p_parameters)
{
        //TODO: if in tab main, then
        gui_ctrls_main_refresh();
}