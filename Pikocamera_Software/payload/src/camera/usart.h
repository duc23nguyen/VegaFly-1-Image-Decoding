#ifndef __USART_H
#define __USART_H

#include <stdint.h>

extern unsigned char USART1_RecieveData;
extern unsigned char NewCMD;

void uart_init(uint32_t bound);
void uart_send(uint32_t len, uint8_t *p);

#endif
