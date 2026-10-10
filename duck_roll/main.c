//
// Created by Erick on 10/8/26.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bsp/board_api.h"
#include "tusb_config.h"
#include "pico/cyw43_arch.h"

#include "usb_descriptors.h"
#include "class/hid/hid_device.h"
#include "device/usbd.h"


// Mostrar el estado de la usb mediante el led (vía chip WiFi, ver cyw43_led_task)

/* Blink pattern
 * - 250 ms  : no montado
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */

enum {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void hid_task(void);
void led_blinking_task(void);

#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT 0
#endif

int main(void) {
    board_init();

    // El LED de la Pico 2 W vive en el chip WiFi, no en un GPIO normal;
    // esto solo levanta el driver para poder prenderlo/apagarlo (sin red).
    cyw43_arch_init();

    // inicia el dispositivo en el root stack configurado del puerto

    const tusb_rhport_init_t rh_init = {
        .role = TUSB_ROLE_DEVICE,
        .speed = TUD_OPT_HIGH_SPEED ? TUSB_SPEED_HIGH : TUSB_SPEED_FULL
    };

    TU_ASSERT(tud_rhport_init(BOARD_TUD_RHPORT, &rh_init));
    board_init_after_tusb();


    while (1) {
        tud_task(); // tinyusb device task
        led_blinking_task();

        hid_task();
    }

}


// Blink de cuando ya esta montado
void tud_mount_cb(void) {
    blink_interval_ms = BLINK_MOUNTED;
}

// Blink cuando no esta montado
void tud_umount_cb(void) {
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invocado cuando el bus del usb esta suspendido
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

// Invocado cuando el bus del usb se resumio
void tud_resume_cb(void) {
    blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}


// manda los reportes con los datos de las teclas que estás presionando
/**
static void send_hid_report(uint8_t report_id, uint32_t btn) {
    // ignora si el hid no esta listo

    if (!tud_hid_ready()) return;

    switch (report_id) {
        case REPORT_ID_KEYBOARD: {
            // se usa para evitar mandar multipes reportes de cero
            static bool has_keyboar_key = false;

            if (btn) {
                uint8_t keycode[6] = {0}; // 6 pues el teclado debe de mandar 6 posiciones pa saber si estas precionando 6 teclas a la vez
                keycode[0] = HID_KEY_A;

                // manda la se;al
                tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
                has_keyboar_key = true;
            } else {
                // Mandar llave vacia si la tecla anterior ya se preciono
                if (has_keyboar_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
                has_keyboar_key = false;
            }
            break;
        }
        default: break;
    }
}
*/

// Tabla de conversión ASCII -> (shift, keycode) que ya trae TinyUSB.
// conv_table[c][0] = 1 si el caracter 'c' necesita Shift.
// conv_table[c][1] = keycode HID correspondiente.
static uint8_t const conv_table[128][2] = { HID_ASCII_TO_KEYCODE };

const char duckyScript[] =
"GUI r \n"
"DELAY 5 \n"
"STRING https://youtu.be/dQw4w9WgXcQ?list=RDdQw4w9WgXcQ\n"
"ENTER \n"
"DELAY 50\n"
"STRING F \n"
;



// Funcion que ahora si manda lo de ruberduky. arriba ejemplo de
static void send_hid_report(uint8_t report_id, uint32_t btn) {


    (void) btn;
    if (!tud_hid_ready()) return;

    static const char* p = duckyScript; // programm counter
    static uint32_t espera_ms = 0;
    static bool precionado = false;
    static char cmd[12], arg[64];
    static size_t letra_idx = 0;


    // Filtro de retrasos
    if (espera_ms > 0) {
        if (board_millis() < espera_ms) return;
        espera_ms = 0;
    }

    // Ciclo hardware, una vuelta la presiona, la otra la suelta.
    if (precionado) {
        tud_hid_keyboard_report(report_id, 0 , NULL);
        precionado = false;
        return;
    }

    // Solo parseamos/avanzamos de línea cuando NO estamos a medio de un
    // STRING multi-letra (letra_idx == 0). Mientras falten letras, cmd/arg
    // (static) ya tienen lo que necesitamos: no hay que re-leer el script.
    if (letra_idx == 0) {
        if (*p == '\0') return; // fin del script

        /// DECODIFICADOR
        if (sscanf(p, "%s %[^\n]", cmd , arg) >= 1) {
            // Avanza El puntero 'p' hasta la siguiente linea del script
            while (*p != '\n' && *p != '\0') p++;
            if (*p == '\n') p++;
        }
    }

    // Unidad de ejecucion
    // evaluamos directamente la primera letra del comando ('D', 'E', 'G', 'S')

    uint8_t keycode[6] = {0};

    switch (cmd[0]) {
        case 'D' : // Delay
            espera_ms = board_millis() + atoi(arg);
            break;
        case 'E' : // Enter
            keycode[0] = HID_KEY_ENTER;
            tud_hid_keyboard_report(report_id, 0, keycode);
            precionado = true;
            break;
        case 'G' : { // Combinaciones con la tecla windows
            unsigned char c = (unsigned char) arg[0];
            uint8_t modifier = KEYBOARD_MODIFIER_LEFTGUI;
            if (c < 128 && conv_table[c][0]) modifier |= KEYBOARD_MODIFIER_LEFTSHIFT;
            keycode[0] = (c < 128) ? conv_table[c][1] : 0;

            tud_hid_keyboard_report(report_id, modifier, keycode);
            precionado = true;
            break;
        }

        case 'S' : //String
            if (arg[letra_idx] != '\0') {
                unsigned char c = (unsigned char) arg[letra_idx];
                uint8_t modifier = (c < 128 && conv_table[c][0]) ? KEYBOARD_MODIFIER_LEFTSHIFT : 0;
                keycode[0] = (c < 128) ? conv_table[c][1] : 0;

                tud_hid_keyboard_report(report_id, modifier, keycode);
                precionado = true;
                letra_idx++;

                // Si ya no quedan letras, deja letra_idx en 0 para que la
                // próxima vuelta parsee la siguiente línea del script.
                if (arg[letra_idx] == '\0') letra_idx = 0;
            } else {
                letra_idx = 0;
            }
            break;

    }




}

// Administrador de tiempos y eventos del teclado q mandara cada 10ms un reporte
void hid_task(void) {

    // Poll de cada 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms) return;

    start_ms += interval_ms;

    uint32_t const btn = board_button_read();

    // Remote wakeup
    if (tud_suspended() && btn) {
        // Prender el host si esta suspendido
        tud_remote_wakeup();
    } else {
        send_hid_report(REPORT_ID_KEYBOARD, btn);
    }
}


// Se invoca cuando el reporte se mando correctamente al host
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len) {
    (void) instance;
    (void) len;

    uint8_t next_report_id = report[0] + 1u;

    if (next_report_id < REPORT_ID_COUNT) {
        send_hid_report(next_report_id, board_button_read());
    }
}

// Se invoca cuando el host pide un reporte (GET_REPORT). No manejamos reportes
// de entrada bajo demanda (solo los enviamos por polling), así que no hay nada que devolver.
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
    uint8_t* buffer, uint16_t reqlen) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

//Asegurarme visualemnte q la computadora esta recibiendo los datos de manera correcta
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
    uint8_t const* buffer, uint16_t bufsize) {

    (void) instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        if (report_id == REPORT_ID_KEYBOARD) {
            // Bufsize debe de ser almenos 1
            if ( bufsize < 1 ) return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
                // Capslock On: desactiva el parpadeo, prende el led fijo
                blink_interval_ms = 0;
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
            } else {
                // Capslock Off: regresa al parpadeo normal
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
                blink_interval_ms = BLINK_MOUNTED;
            }
        }
    }
}



// Estado del led.
void led_blinking_task(void) {
    static uint32_t start_ms = 0;
    static bool led_state = false;

    // Parpadeo apagado
    if (!blink_interval_ms) return;

    // Parpadear cada intervalo ms
    if (board_millis() - start_ms < blink_interval_ms) return; // No tiempo justo
    start_ms += blink_interval_ms;

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
    led_state = !led_state;
}


