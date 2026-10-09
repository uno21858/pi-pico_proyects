cmake_minimum_required(VERSION 3.13)

# Dato del proyecto: este firmware es para la Pico 2 W.
# CACHE permite sobreescribirlo con -DPICO_BOARD=... sin tocar el archivo.
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Busca el SDK (usa PICO_SDK_PATH). Tiene que ir antes de project().
include(../pico_sdk_import.cmake)

project(duck_roll C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# El programa: un target con sus fuentes
add_executable(duck_roll
        main.c
        usb_descriptors.c
)

# tinyusb_device: trae tusb.h y el stack USB
# tinyusb_board:  trae board_api.h (board_init, LED, botón)
target_link_libraries(duck_roll pico_stdlib tinyusb_device tinyusb_board)

# para que encuentre tu propio tusb_config.h y usb_descriptors.h
target_include_directories(duck_roll PRIVATE ${CMAKE_CURRENT_LIST_DIR})

pico_add_extra_outputs(duck_roll)