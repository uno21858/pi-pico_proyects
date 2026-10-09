cmake_minimum_required(VERSION 3.13)

# Dato del proyecto: este firmware es para la Pico 2 W.
# CACHE permite sobreescribirlo con -DPICO_BOARD=... sin tocar el archivo.
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Busca el SDK (usa PICO_SDK_PATH). Tiene que ir antes de project().
include(../pico_sdk_import.cmake)

project(led_serial C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# El programa: un target con sus fuentes
add_executable(led_serial main.cpp)

# pico_stdlib: GPIO, sleep, printf
# pico_cyw43_arch_none: driver del chip WiFi (ahí cuelga el LED), sin red
target_link_libraries(led_serial pico_stdlib pico_cyw43_arch_none)

# printf sale por el USB, no por los pines UART
pico_enable_stdio_usb(led_serial 1)
pico_enable_stdio_uart(led_serial 0)

# Genera el .uf2 además del .elf
pico_add_extra_outputs(led_serial)