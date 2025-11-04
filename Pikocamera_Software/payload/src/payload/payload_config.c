/* Payload settings */

#include "payload_config.h"
#include "ArduCAM.h"
#include "sys.h"
#include "misc.h"

/* UART */
uint32_t payload_uart_baud_rate = 921600;

/* SSDV */
char ssdv_type = SSDV_TYPE_NORMAL;
char ssdv_call_sign[5] = "9NPQ2";
uint8_t ssdv_image_id = 0;
int8_t ssdv_quality = 4;

/* Camera */
uint8_t arducam_image_size = OV2640_320x240;
uint8_t arducam_image_format = JPEG;

/* I2C interrupts */
void NVIC_Configuration(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_SetPriority(I2C2_EV_IRQn, 0x00);
	NVIC_SetPriority(I2C2_ER_IRQn, 0x01);
	NVIC_EnableIRQ(I2C2_EV_IRQn);
	NVIC_EnableIRQ(I2C2_ER_IRQn);
}
