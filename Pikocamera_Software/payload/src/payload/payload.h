// Rishav (2022/08/12)

#ifndef _PAYLOAD_H_
#define _PAYOAD_H_

#include "payload_config.h"
#include "usart.h"
#include "delay.h"
#include "stdlib.h"

extern size_t ssdv_packets_in_image;
extern uint8_t payload_image_buffer[PAYLOAD_IMAGE_BUFFER_SIZE];

void payload_system_init();
void payload_arducam_init();
void payload_take_image();
void payload_erase_image_buffer();
void payload_ssdv_encode(uint8_t flash_log_flag);
void payload_read_from_flash(uint32_t indx);

void append_array(uint8_t *s, uint8_t *d, uint16_t start, uint16_t n);

#endif // payload.h