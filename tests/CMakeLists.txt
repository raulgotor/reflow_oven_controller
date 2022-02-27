set(CMAKE_CXX_FLAGS "")
message("Current dir:       " ${CMAKE_CXX_FLAGS})

message(${CMAKE_CURRENT_SOURCE_DIR})

set(SRC_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})
set(MOCKS_DIR ${TESTS_DIR}/mocks)
set(IDF_COMPONENTS_PATH $ENV{IDF_PATH}/components)

message("Current dir:         " ${CMAKE_CURRENT_SOURCE_DIR})
message("Production dirs:     " ${PRODUCTION_DIR})
message("Tests source dirs:   " ${SRC_DIRECTORIES})
message("Mocks dirs:          " ${MOCKS_DIR})
message("IDF components path: " ${IDF_COMPONENTS_PATH})

if(DEFINED ENV{CPPUTEST_HOME})
    message(STATUS "Using CppUTest home: $ENV{CPPUTEST_HOME}")
    set(CPPUTEST_INCLUDE_DIRS $ENV{CPPUTEST_HOME}/include)
    set(CPPUTEST_LIBRARIES $ENV{CPPUTEST_HOME}/lib)
    set(CPPUTEST_LDFLAGS CppUTest CppUTestExt)
else()
    find_package(PkgConfig REQUIRED)
    #find_package(CPPUTEST REQUIRED cpputest>=3.8)
    pkg_search_module(CPPUTEST REQUIRED cpputest>=3.8)
    message(STATUS "Found CppUTest version ${CPPUTEST_VERSION}")
endif()

add_subdirectory(${MOCKS_DIR})


file(GLOB SOURCES
        "${SRC_DIRECTORIES}/*.h"
        "${SRC_DIRECTORIES}/*.cpp"
        "${SRC_DIRECTORIES}/*.c"
        "${PRODUCTION_DIR}/heater.c"

        )

include_directories(
        ${CPPUTEST_INCLUDE_DIRS}
        ${TESTS_DIR}/mocks/hal

        "${IDF_COMPONENTS_PATH}/driver/include"
        "${IDF_COMPONENTS_PATH}/esp_common/include"
        "${IDF_COMPONENTS_PATH}/hal/include/hal"
        "${IDF_COMPONENTS_PATH}/hal/include"
        "${IDF_COMPONENTS_PATH}/esp_rom/include"
        "${IDF_COMPONENTS_PATH}/esp_hw_support/include"
        "${IDF_COMPONENTS_PATH}/soc/esp32/include"
        "${IDF_COMPONENTS_PATH}/soc/include"
        "${PRODUCTION_DIR}"
        "${PROJECT_SOURCE_DIR}/cmake-build-production-xtensa/config"
)

link_directories(${CPPUTEST_LIBRARIES})

add_executable(reflow_oven_controller_tests
        heater_tests.cpp
        ${SOURCES}
        )

target_link_libraries(reflow_oven_controller_tests
        #main
        mocks
        ${CPPUTEST_LDFLAGS}
       )
