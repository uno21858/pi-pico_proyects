//
// Created by Erick on 10/8/26.
//


// Incluir las librerias
#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "class/hid/hid_device.h"


// Definir el HID (identificador del usb)
#define _PID_MAP(itf, n) ( (CFG_TUD_##itf) << (n) )
#define USB_PID (0x1532 | _PID_MAP(HID, 0) )
/**
*• CDC (Communications Device Class): Se usa para emular Puertos de comunicación/Serie (Virtual COM ports). Es lo que te permite enviar y recibir datos mediante Serial.print() a la terminal de tu computadora.
• MSC (Mass Storage Class): Esto es Almacenamiento Masivo (como una memoria USB o un lector de tarjetas SD). No es el mouse. Si activas MSC, tu computadora verá a tu placa como si fuera un disco duro o pendrive.
• HID (Human Interface Device): ¡Esta es la clase que buscas para el Mouse y el Teclado! Todo dispositivo que interactúe directamente con humanos (ratones, teclados, gamepads) entra en la categoría HID.
• MIDI (Musical Instrument Digital Interface): Permite que tu placa funcione como un instrumento musical digital o controlador de audio para conectarlo a software de producción musical.
• VENDOR: Significa "Clase de Proveedor" (Vendor-specific). Se usa cuando quieres crear un protocolo de comunicación 100% personalizado que no encaja en ninguna de las categorías anteriores.
*/

// Numero o pq el hex, significa el vendor.
/**
 * se puede buscar The USB ID Repository
*• 0x1532 -> Razer
• 0x05AC -> Apple
• 0x045E -> Microsoft
*/


// Modelo del producto
#define USB_VID 0x0226

// BCD significan Binary Coded Decimal (Decimal Codificado en Binario)
// En pocas palabras si es usb 1.0, 2.0, 3.0, 3.1 (the usb bible chapter 5)
#define USB_BCD 0x0300

/*
 *Configuracion de la USB. lo que inyecta a la compu o le da la identidad
 * o emula el teclado
*/

tusb_desc_device_t const desc_device = {
 .bLength = sizeof(tusb_desc_device_t),
 .bDescriptorType    = TUSB_DESC_DEVICE,          // Tipo de descriptor (Dispositivo = 1)
 .bcdUSB             = USB_BCD,                   // Versión USB (Ej: 0x0200 para USB 2.0)
 .bDeviceClass       = 0x00,                      // Clase (0x00 significa "mira los detalles en la interfaz")
 .bDeviceSubClass    = 0x00,                      // Subclase
 .bDeviceProtocol    = 0x00,                      // Protocolo
 .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,    // Tamaño máximo de paquete para canal de control

 .idVendor           = USB_VID,                   // Tu famoso 0xCafe
 .idProduct          = USB_PID,                   // Tu PID calculado (Ej: 0x4001)
 .bcdDevice          = 0x0100,                    // Versión de tu propio software (v1.0)

 .iManufacturer      = 0x01,                      // Índice del texto con el nombre del fabricante
 .iProduct           = 0x02,                      // Índice del texto con el nombre del producto
 .iSerialNumber      = 0x03,                      // Índice del número de serie
 .bNumConfigurations = 0x01                       // Cuántas configuraciones tiene el aparato (usualmente 1)
};


// Crea un espacio en memoria por el pointer.
// Cuando tinyUSB lo necesita lo llama y ya tiene todo el desto en memoria
uint8_t const * tud_descriptor_device_cb(void) {
 return (uint8_t const *) &desc_device;
}


// Lista de los descriptores que estan
uint8_t const desc_hid_report[] = {
 TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(REPORT_ID_KEYBOARD))
 // EJ. mouse TUD_HID_REPORT_DESC_MOUSE   ( HID_REPORT_ID(REPORT_ID_MOUSE            )),
};


