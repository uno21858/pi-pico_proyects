//
// Created by Erick on 10/8/26.
//

/**
 * Usb descriptors sirve para darle la identidad a lo que vendria siendo
 *  un usb. como lo es el identificador, el vendedor, velociddad. modelo. etc etc.
**/


// Incluir las librerias
#include "bsp/board_api.h"
#include "usb_descriptors.h"
#include "class/hid/hid_device.h"
#include "device/usbd.h"


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
//
// OJO: tiene que coincidir con la velocidad real que configuramos en
// tusb_config.h (OPT_MODE_FULL_SPEED). Poner 0x0300 (USB 3.0) aquí hace que
// el host espere un descriptor BOS (obligatorio desde USB 2.1) que este
// firmware no implementa -> Windows lo rechaza con "device not recognized".
#define USB_BCD 0x0200

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


// Invoca cuando recive el GET HID descriptor.
// Osea es el apuntador del descriptor que es del array
uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance) {
 (void) instance;
 return desc_hid_report;
}


// Configuracion del descriptor


// Define la cantidad de interfaces USB. y le asigna un numero identificador.
enum {
 ITF_NUM_HID,
 ITF_NUM_TOTAL
};


// Size de los bytes q mandara del bloque de configuracion
/**
 * TUD_CONFIG_DEC_LEN = 9 bytes (siempre)
 * TUD_HID_DESC_LEN = 9 bytes (siempre)
 * siempre se mandan 18 bytes en total
 */
#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

// define la direccion del endpoint. osea por donde lo mandara.
// 0x80 para out. 0x81 para in.
#define EPNUM_HID 0x81



/**
* 1. La primera macro (TUD_CONFIG_DESCRIPTOR) es la etiqueta de la caja externa.
2. La segunda macro (TUD_HID_DESCRIPTOR) es el producto que metes dentro de la caja.
 */
uint8_t const desc_configuration[] = {
 /**
  *Define las reglas globales de energía y comportamiento de tu placa al conectarse
  **/
 // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

 /**
  *Define los detalles específicos de hardware para la interfaz del teclado.
  **/
 // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
  TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 5)
};


//Selecciona el tipo de configuracion de usb
uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
 (void) index;
 return desc_configuration;
}


// String Descriptors


//Index del descriptor
enum {
 STRID_LANGID = 0,  //Identificador del idioma
 STRID_MANUFACTURER,  // Nombre de la empresa o creador
 STRID_PRODUCT,  // Nombre comercial del producto (modelo)
 STRID_SERIAL // Numero de serie unico
};


// Arreglos del puntero para el string descriptor
char const *string_desc_arr[] = {
 (const char[]) {0x09, 0x04},
 "TinyUSB",  // Empresa
 "TinyUSB Device", // Producto o modelo
 NULL // Seriales usaran un ID unico si es posible
};

// Escribir 32+1 es una forma de documentar el límite de hardware en el mismo tamaño del búfer.

static uint16_t _desc_str[32+1];


// JUNTAR TODO. cuando es invocado regresa el puntero

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
 (void) langid;
 size_t chr_count;


 switch (index) {
  case STRID_LANGID:
   // Destino, origen, size
   memcpy(&_desc_str[1], string_desc_arr[0], 2);
   chr_count = 1; // Cuenta Cuantas palabras de 16 bits ya se procesaaron
   break;

  case STRID_SERIAL:
   chr_count = board_usb_get_serial(_desc_str + 1, 32);
   break;

  default:

   if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) ) return NULL;

   // Extrae el texto que la computadora le pregunta.
   const char *str = string_desc_arr[index];

   // Proteccion de desbordamiento
   chr_count = strlen(str);
   size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; // 66/2 = 33 - 1 = 32;

   if (chr_count > max_count) chr_count = max_count;

   // Convertir ASCII a utf-16
   /**
    *copia las letras del nombre de tu producto y las traduce al formato que exige el cable USB.
    **/
   for (size_t i = 0; i < chr_count; i++) {
    _desc_str[1 + i] = str[i];
   }
   break;
 }

 // MEte todo al array. ya traducido o como la compu necesita para el identificador.
 _desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

 return _desc_str;

}
