cmake_minimum_required(VERSION 3.13)

# Todos los proyectos de este árbol son para la Pico 2 W
set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(proyectos_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Un add_subdirectory por proyecto
add_subdirectory(led_serial)
add_subdirectory(duck_roll)