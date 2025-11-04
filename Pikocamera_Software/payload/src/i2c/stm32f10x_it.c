#include "stm32f10x_it.h"
#include "i2c.h"
#include "payload.h"
#include "watchdog.h"
#include "flash.h"
#include "test_card_ssdv.h"
#include "ssdv.h"

// I2C rx bytes and functions
#define PAYLOAD_I2C_RESET 0x7e
#define PAYLOAD_I2C_REFRESH 0x15
#define PAYLOAD_I2C_CAPTURE_IMAGE 0x25
#define PAYLOAD_I2C_UPDATE_SSDV_PKT 0x35
#define PAYLOAD_I2C_UPDATE_MINI_PKT 0x45
#define PAYLOAD_I2C_FETCH_SSDV_FLASH 0x55
#define PAYLOAD_I2C_IMAGE_CAPTURED 0x65
#define PAYLOAD_I2C_GET_SSDV_CNT 0x75
#define PAYLOAD_I2C_STATIC_IMAGE 0x85
#define PAYLOAD_I2C_NO_CAMERA_IMAGE 0x95


uint8_t send_ssdv_cnt_flag = 0;
uint8_t send_static_image_flag = 0;

/**
 * @brief Determines overall payload operation.
 * @note
 *    0 : Disable I2C transmission and image capture.
 *    1 : Image capture enable, I2C tx disable.
 *    2 : I2C tx enable, Image capture disable.
 */
uint8_t payload_flag = 0;

// Buffers
uint8_t rx_byte;                  // Rx byte from master
uint8_t payload_ssdv_buffer[SSDV_PKT_SIZE]; // Single SSDV packet tx buffer

// Indices and count
__IO uint8_t mini_ssdv_idx = 0;  // Index of mini packet (0-32)
__IO uint16_t mini_ssdv_cnt = 0; // Count of mini packet (0-8)
__IO uint16_t ssdv_pkt_cnt = 0;  // Count of SSDV packet (0-n)

void I2C2_EV_IRQHandler(void)
{
    __IO uint32_t SR1Register = 0;
    __IO uint32_t SR2Register = 0;

    // Read the I2C1 SR1 and SR2 status registers
    SR1Register = I2C2->SR1;
    SR2Register = I2C2->SR2;

    // If I2C2 is slave (MSL flag = 0)
    if ((SR2Register & 0x0001) != 0x0001)
    {
        // If ADDR = 1: EV1
        if ((SR1Register & 0x0002) == 0x0002)
        {
            SR1Register = 0;
            SR2Register = 0;
            mini_ssdv_idx = 0;
        }

        // If TXE = 1: EV3
        if (payload_flag == 2)
        {
            if ((SR1Register & 0x0080) == 0x0080)
            {
                if (send_ssdv_cnt_flag)
                {
                    I2C2->DR = ssdv_packets_in_image;
                    send_ssdv_cnt_flag = 0;
                }
                else
                {
                    if (send_static_image_flag)
                    {
                        I2C2->DR = test_card_ssdv_buffer[ssdv_pkt_cnt * SSDV_PKT_SIZE + mini_ssdv_cnt * 32 + mini_ssdv_idx];     
                    }
                    else
                    {
                        I2C2->DR = payload_ssdv_buffer[mini_ssdv_cnt * 32 + mini_ssdv_idx];
                    }
                    mini_ssdv_idx++;
                    SR1Register = 0;
                    SR2Register = 0;
                }
            }
        }

        // If RXNE = 1: EV2
        if ((SR1Register & 0x0040) == 0x0040)
        {
            // Read data from data register
            rx_byte = I2C2->DR;

            // Take image
            if (rx_byte == PAYLOAD_I2C_CAPTURE_IMAGE)
            {
                uint8_t msg[] = "Capture image!\r\n";
                uart_send(sizeof(msg), msg);
                send_static_image_flag = 0;
                payload_flag = 1;
            }

            if (rx_byte == PAYLOAD_I2C_NO_CAMERA_IMAGE)
            {
                send_static_image_flag = 0;
                payload_flag = 2;
            }

            // Prepare for static image SSDV transmission
            if (rx_byte == PAYLOAD_I2C_STATIC_IMAGE)
            {
                send_static_image_flag = 1;
                payload_flag = 2;
                ssdv_packets_in_image = STATIC_IMAGE_SSDV_PKT_CNT;
            }

            // Reset all indices
            if (rx_byte == PAYLOAD_I2C_RESET)
            {
                uint8_t msg[] = "Reset success!\r\n";
                uart_send(sizeof(msg), msg);
                watchdog_init();

                while (1)
                {
                }
            }

            if (rx_byte == PAYLOAD_I2C_GET_SSDV_CNT)
            {
                send_ssdv_cnt_flag = 1;
            }

            if (payload_flag == 2)
            {
                // Send SSDV in mini packets
                if (rx_byte == PAYLOAD_I2C_UPDATE_MINI_PKT)
                {
                    mini_ssdv_idx = 0;
                    mini_ssdv_cnt++;
                }

                // Fetch "ssdv_pkt_cnt"ith packet to "payload_ssdv_buffer"
                if (rx_byte == PAYLOAD_I2C_FETCH_SSDV_FLASH)
                {
                    if (!send_static_image_flag)
                    {
                        flash_init();
                        flash_read_buffer(payload_ssdv_buffer, ssdv_pkt_cnt * SSDV_PKT_SIZE, SSDV_PKT_SIZE);
                    }
                }

                // Shift to next SSDV packets
                if (rx_byte == PAYLOAD_I2C_UPDATE_SSDV_PKT)
                {
                    mini_ssdv_idx = 0;
                    mini_ssdv_cnt = 0;
                    ssdv_pkt_cnt++;

                    if (ssdv_pkt_cnt != 0 && ssdv_pkt_cnt == ssdv_packets_in_image)
                    {
                        watchdog_init();
                        while (1)
                        {
                        }
                    }
                }

                // Transmission packet start
                if (rx_byte == PAYLOAD_I2C_REFRESH)
                {
                    mini_ssdv_idx = 0;
                    mini_ssdv_cnt = 0;
                    ssdv_pkt_cnt = 0;
                }
            }
            SR1Register = 0;
            SR2Register = 0;
        }

        // If STOPF =1: EV4 (Slave has detected a STOP condition on the bus)
        if ((SR1Register & 0x0010) == 0x0010)
        {
            I2C2->CR1 |= CR1_PE_Set;
            SR1Register = 0;
            SR2Register = 0;
        }
    } // End slave mode
}

void I2C2_ER_IRQHandler(void)
{
    __IO uint32_t SR1Register = 0;

    // Read status register
    SR1Register = I2C2->SR1;

    // If AF = 1
    if ((SR1Register & 0x0400) == 0x0400)
    {
        I2C2->SR1 &= 0xFBFF;
        SR1Register = 0;
    }

    // If ARLO = 1
    if ((SR1Register & 0x0200) == 0x0200)
    {
        I2C2->SR1 &= 0xFBFF;
        SR1Register = 0;
    }

    // If BERR = 1
    if ((SR1Register & 0x0100) == 0x0100)
    {
        I2C2->SR1 &= 0xFEFF;
        SR1Register = 0;
    }

    // If OVR = 1
    if ((SR1Register & 0x0800) == 0x0800)
    {
        I2C2->SR1 &= 0xF7FF;
        SR1Register = 0;
    }
}

void HardFault_Handler(void)
{
    NVIC_SystemReset();
    while (1)
    {
    }
}
