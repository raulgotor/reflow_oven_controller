set(SOURCES main.c state_machine/state_machine.c
                   state_machine/state_machine_task.c
                   state_machine/states/state_machine_states.c
                   reflow_profile.c)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS .
                    REQUIRES lvgl_ili9341 lvgl nvs_flash)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
