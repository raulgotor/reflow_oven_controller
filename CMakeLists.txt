cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(ENV{PATH} "$ENV{XPATH}:$ENV{PATH}")

project(reflow_oven_controller)
