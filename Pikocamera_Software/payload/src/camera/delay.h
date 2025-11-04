#ifndef __DELAY_H
#define __DELAY_H 

#include "stm32f10x.h"

void delay_init(void);
void delay_ms(__IO uint16_t nms);
void delay_us(__IO uint32_t nus);

#endif





























