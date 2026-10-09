//
// Created by Erick on 10/8/26.
//

#include <cstdio>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

int main() {
    stdio_init_all();

    if (cyw43_arch_init() != 0) {
        // Si falla el driver del WiFi, avisa por serial en loop para que lo veas
        while (true) {
            printf("error: cyw43_arch_init fallo\n");
            sleep_ms(1000);
        }
    }

    bool led = false;
    unsigned long n = 0;
    while (true) {
        led = !led;
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led);
        printf("tick %lu\n", n++);
        sleep_ms(500);
    }
}