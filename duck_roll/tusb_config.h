//
// Created by Erick on 10/9/26.
//

#ifndef PROYECTOS_PICO_TUSB_CONFIG_H
#define PROYECTOS_PICO_TUSB_CONFIG_H

// Configuración del Sistema Operativo
// CFG_TUSB_MCU y CFG_TUSB_DEBUG NO se definen aquí: el pico-sdk ya los
// inyecta por línea de comandos (CFG_TUSB_MCU=OPT_MCU_RP2040, el mismo
// driver USB se usa para RP2040 y RP2350). Redefinirlos aquí con
// OPT_MCU_RP2350 (que no existe en este TinyUSB) rompía la detección
// de MCU y dejaba TUP_DCD_ENDPOINT_MAX sin definir.
#define CFG_TUSB_OS             OPT_OS_PICO     // SDK nativo de la Pico

// Modo del puerto USB físico: dispositivo a Full Speed.
// Sin esto, CFG_TUD_ENABLED queda en 0 y todo el stack de device
// (usbd.c, hid_device.c) se compila vacío -> undefined reference.
#define CFG_TUSB_RHPORT0_MODE   (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)

// Alineación de memoria para transferencias DMA de alta velocidad

#define CFG_TUSB_MEM_ALIGN      __attribute__((aligned(4)))

// Habilitar ÚNICAMENTE el motor de dispositivos Humanos (Teclado/Mouse)
#define CFG_TUD_HID             1

// Tamaño máximo del buffer de transferencia por hardware (Fijo en 64 bytes para Full Speed)
#define CFG_TUD_HID_EP_BUFSIZE  64

// Desactivar por completo el resto de las fábricas lógicas (Cero basura)
#define CFG_TUD_CDC             0
#define CFG_TUD_MSC             0
#define CFG_TUD_MIDI            0
#define CFG_TUD_VENDOR          0

#endif // PROYECTOS_PICO_TUSB_CONFIG_H
