// Rishav (2022/08/12)

// #include "payload_config.h"
#include "sccb_bus.h"
#include "payload.h"
#include "ArduCAM.h"
#include "usart.h"
#include "flash.h"
#include "ssdv.h"
#include "spi.h"
#include "i2c.h"

// Externs settings
extern char ssdv_type;
extern char ssdv_call_sign[];
extern uint8_t ssdv_image_id;
extern int8_t ssdv_quality;
extern ssdv_t ssdv;
extern uint8_t arducam_image_size;
extern uint8_t arducam_image_format;

size_t ssdv_packets_in_image = 0;
uint8_t payload_image_buffer[PAYLOAD_IMAGE_BUFFER_SIZE] = {0};

// Destination is lager array
void append_array(uint8_t *s, uint8_t *d, uint16_t start, uint16_t n)
{
  for (uint16_t i = 0; i < n; i++)
  {
    d[i + start] = s[i];
  }
}

void payload_system_init()
{
  NVIC_Configuration();
  I2C_LowLevel_Init(I2C2);
  I2C_Slave_BufferReadWrite(I2C2, Interrupt);
  delay_init();
}

void payload_arducam_init()
{
  sccb_bus_init();
  SPI1_Init();
  arducam_init(OV2640);
  payload_erase_image_buffer();

  while (1)
  {
    write_reg(ARDUCHIP_TEST1, 0x55);
    uint8_t temp = read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55)
    {
#if PAYLOAD_DEBUG_MODE == 1
      uint8_t msg[] = "Trying to reconnect SPI.\r\n";
      uart_send(sizeof(msg), msg);
#endif
      delay_ms(1000);
      continue;
    }
    else
    {
#if PAYLOAD_DEBUG_MODE == 1
      uint8_t msg[] = "SPI connected!\r\n";
      uart_send(sizeof(msg), msg);
#endif
      break;
    }
  }

  uint8_t vid, pid;
  sensor_addr = 0x60;
  wrSensorReg8_8(0xff, 0x01);
  rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);

  if ((vid == 0x26) && (pid == 0x42))
  {
    sensor_model = OV2640;
#if PAYLOAD_DEBUG_MODE == 1
    uint8_t msg[] = "OV2640 found!\r\n";
    uart_send(sizeof(msg), &msg[0]);
#endif
  }
  else
  {
#if PAYLOAD_DEBUG_MODE == 1
    uint8_t msg[] = "OV2640 not found.\r\n";
    uart_send(sizeof(msg), &msg[0]);
#endif
  }

  arducam_set_format(arducam_image_format);
  arducam_init(sensor_model);
  arducam_OV2640_set_JPEG_size(arducam_image_size);
  delay_ms(5000);
}

void payload_take_image()
{
  payload_arducam_init();
  uint16_t imp_counter = 0;
  SingleCapTransfer();
  if (send_OK)
  {
    send_OK = false;
    while (!send_OK)
    {
      uint8_t a = SendbyUSART1();
      delay_ms(15);

#if PAYLOAD_DEBUG_ARDUCAM == 1
      uart_send(sendlen, arducam);
#endif

      append_array(arducam, payload_image_buffer, imp_counter, sendlen);
      imp_counter += sendlen;
      // char str[10];
      // itoa(sendlen, str, 10);
      // uart_send(10, str);

      if (a == 1)
      {
        noRev = length - haveRev;
        sendlen = (noRev >= BUFFER_MAX_SIZE) ? BUFFER_MAX_SIZE : noRev;
        DMA1_RX(picbuf, sendlen);
      }
    }
  }

#if PAYLOAD_DEBUG_IMAGE == 1
  uart_send(sizeof(payload_image_buffer), payload_image_buffer);
#endif
}

void payload_ssdv_encode(uint8_t flash_log_flag)
{
  int c;
  uint8_t *b;
  uint16_t bi = 0;
  uint32_t image_len = sizeof(payload_image_buffer);
  uint8_t ssdv_pkt[SSDV_PKT_SIZE];

  ssdv_t ssdv;
  ssdv_enc_init(&ssdv, ssdv_type, ssdv_call_sign, ssdv_image_id, ssdv_quality);
  ssdv_enc_set_buffer(&ssdv, ssdv_pkt);

  ssdv_packets_in_image = 0;

  flash_init();
  flash_erase_sector(0);
  flash_erase_sector(1);
  flash_erase_sector(2);

  uint32_t flash_id = flash_read_id();
  if (flash_id != FLASH_ID_512)
  {
    while (1)
    {
    }
  }

  while (1)
  {
    while ((c = ssdv_enc_get_packet(&ssdv)) == SSDV_FEED_ME)
    {
      b = &payload_image_buffer[bi];
      uint8_t r = bi < image_len - 128 ? 128 : image_len - bi;
      bi += r;
      if (r <= 0)
      {
        break;
      }
      ssdv_enc_feed(&ssdv, b, r);
    }

    if (c == SSDV_EOI)
    {
      break;
    }
    else if (c != SSDV_OK)
    {
      // return (-1);
      break;
    }

#if PAYLOAD_DEBUG_SSDV == 1
    uart_send(sizeof(ssdv_pkt), ssdv_pkt);
#endif

    if (flash_log_flag)
    {
      flash_write_buffer(ssdv_pkt, ssdv_packets_in_image * SSDV_PKT_SIZE, sizeof(ssdv_pkt));
      ssdv_packets_in_image++;
    }
  }

#if PAYLOAD_DEBUG_FLASH == 1
  for (uint32_t i = 0; i < ssdv_packets_in_image - 1; i++)
  {
    uint8_t ssdv[SSDV_PKT_SIZE] = {0};
    flash_read_buffer(ssdv, i * SSDV_PKT_SIZE, sizeof(ssdv));
    uart_send(sizeof(ssdv), ssdv);
  }
#endif
  payload_erase_image_buffer();
}

void payload_erase_image_buffer()
{
  for (size_t i = 0; i < sizeof(payload_image_buffer); i++)
  {
    payload_image_buffer[i] = 0;
  }
}
