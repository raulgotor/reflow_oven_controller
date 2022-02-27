cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_STANDARD 99)

message("Building ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL production)

    #set(PRODUCTION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/main)

    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    set(ENV{PATH} "$ENV{XPATH}:$ENV{PATH}")
    add_definitions(-DTEST_COMPILATION=0)

    project(reflow_oven_controller LANGUAGES C CXX)

elseif(CMAKE_BUILD_TYPE STREQUAL test)

    project(tests_main)

    add_definitions(-DTEST_COMPILATION=1)
    set(PRODUCTION_DIR ${PROJECT_SOURCE_DIR}/main)
    set(TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)
    add_subdirectory(${TESTS_DIR})

else()

    message(FATAL_ERROR "I don't know the CMAKE_BUILD_TYPE you gave me!")

endif()