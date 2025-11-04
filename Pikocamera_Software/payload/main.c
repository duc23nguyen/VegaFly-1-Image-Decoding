/**
 * @file main.c
 * @brief Implementation of main() for camera payload.
 * @note Configure the parameters on payload_config.c.
 * @author Rishav
 * @date 2022/08/12
**/

#include "stm32f10x_i2c.h"
#include "payload_config.h"
#include "payload.h"
#include "stdlib.h"
#include "flash.h"
#include "i2c.h"

extern uint8_t payload_flag;

void setup()
{
    payload_system_init();
#if PAYLOAD_DEBUG_INIT == 1
    uart_init(payload_uart_baud_rate);
#endif

    delay_ms(10);
}

void loop()
{
    while (1)
    {
        // If Rx = PAYLOAD_I2C_CAPTURE_IMAGE
        // Write SSDV packets in flash memory
        if (payload_flag == 1)
        {
            payload_take_image();         // Fill "payload_image_buffer"
            payload_ssdv_encode(1);       // "payload_image_buffer" to SSDV
            payload_erase_image_buffer(); // Reset "payload_image_buffer"
            payload_flag = 2;
        }
    }
}

int main(void)
{
    setup();
    loop();
}