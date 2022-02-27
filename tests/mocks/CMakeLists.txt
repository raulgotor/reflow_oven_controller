set(SRC_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SOURCES
        ${SRC_DIRECTORIES}/driver/*.c
        ${SRC_DIRECTORIES}/freertos/*.c
        ${SRC_DIRECTORIES}/hal/*.c
        )
include_directories(
        ${SRC_DIRECTORIES}

        ${PRODUCTION_DIR}

        "${IDF_COMPONENTS_PATH}/driver/include"
        "${IDF_COMPONENTS_PATH}/esp_common/include"
        "${IDF_COMPONENTS_PATH}/esp_rom/include"
        "${IDF_COMPONENTS_PATH}/esp_hw_support/include"
        "${IDF_COMPONENTS_PATH}/hal/include/hal"
        "${IDF_COMPONENTS_PATH}/hal/include"
        "${IDF_COMPONENTS_PATH}/soc/esp32/include"
        "${IDF_COMPONENTS_PATH}/soc/include"
        "${PROJECT_SOURCE_DIR}/cmake-build-production-xtensa/config"

)

add_library(mocks ${SOURCES})
target_include_directories(mocks PUBLIC ${CMAKE_CURRENT_LIST_DIR})
