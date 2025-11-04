/*
    Declarations of global payload settings.
*/

#ifndef _PAYLOAD_CONFIG_H_
#define _PAYLOAD_CONFIG_H_

#include "ssdv.h"

#define PAYLOAD_I2C_SLAVE_ADDRESS 0x9
#define PAYLOAD_IMAGE_BUFFER_SIZE 8192
 
// Debug flags: 1->Enable
#define PAYLOAD_DEBUG_INIT 1
#define PAYLOAD_DEBUG_MODE 1
#define PAYLOAD_DEBUG_IMAGE 0
#define PAYLOAD_DEBUG_ARDUCAM 0
#define PAYLOAD_DEBUG_SSDV 0
#define PAYLOAD_DEBUG_FLASH 0

// Payload settings
extern uint32_t payload_uart_baud_rate;

// SSDV settings
extern char ssdv_type;
extern char ssdv_call_sign[];
extern uint8_t ssdv_image_id;
extern int8_t ssdv_quality;
// Camera settings
extern uint8_t arducam_image_size;
extern uint8_t arducam_image_format;

void NVIC_Configuration(void);

#endif
