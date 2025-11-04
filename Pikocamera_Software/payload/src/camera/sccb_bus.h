
#ifndef _SCCB_BUS_H_
#define _SCCB_BUS_H_

#include "stm32f10x.h"
#include "sys.h"

#define I2C_TIM 1

// SDA B1,  SCL B0
#define SCCB_SIC_H() PBout(0) = 1 // SCL H
#define SCCB_SIC_L() PBout(0) = 0 // SCL H
#define SCCB_SID_H() PBout(1) = 1 // SDA H
#define SCCB_SID_L() PBout(1) = 0 // SDA H

// CRH: Configure pins 8-15 of the port 
// CRL: Configure pins 0-7 of the port 
#define SCCB_DATA_IN {GPIOB->CRL&=0XFFFFFF0F; GPIOB->CRL|=0X00000080;}
#define SCCB_DATA_OUT {GPIOB->CRL&=0XFFFFF0F; GPIOB->CRL|=0X00000030;}
#define SCCB_SID_STATE PBin(1)

void sccb_bus_init(void);
void sccb_bus_start(void);
void sccb_bus_stop(void);
void sccb_bus_send_noack(void);
void sccb_bus_send_ack(void);
uint8_t sccb_bus_write_byte(uint8_t data);
uint8_t sccb_bus_read_byte(void);

#endif
