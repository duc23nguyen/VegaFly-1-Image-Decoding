#ifndef _FLASH_H_
#define _FLASH_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x_gpio.h"
#include "inttypes.h"

#define FLASH_CMD_WRITE 0x02 // Write to flash
#define FLASH_CMD_WRSR 0x01  // Write status register
#define FLASH_CMD_WREN 0x06  // Write enable
#define FLASH_CMD_READ 0x03  // Read from flash
#define FLASH_CMD_RDSR 0x05  // Read status register
#define FLASH_CMD_RDID 0x9F  // Read ID
#define FLASH_CMD_SE 0xD8    // Sector erase
#define FLASH_CMD_BE 0xC7    // Bulk erase
#define FLASH_WIP_FLAG 0x01  // Write in progress

#define FLASH_DUMMY_BYTE 0xA5
#define FLASH_SPI_PAGESIZE 0x100

#define FLASH_ID_512 0xEF4020

#define FLASH_SPI SPI1
#define FLASH_SPI_CLK RCC_APB2Periph_SPI1
#define FLASH_SPI_CLK_INIT RCC_APB2PeriphClockCmd

#define FLASH_SPI_SCK_PIN GPIO_Pin_5
#define FLASH_SPI_SCK_GPIO_PORT GPIOA
#define FLASH_SPI_SCK_GPIO_CLK RCC_APB2Periph_GPIOA
#define FLASH_SPI_SCK_SOURCE GPIO_PinSource5
#define FLASH_SPI_SCK_AF GPIO_AF_SPI1

#define FLASH_SPI_MISO_PIN GPIO_Pin_6
#define FLASH_SPI_MISO_GPIO_PORT GPIOA
#define FLASH_SPI_MISO_GPIO_CLK RCC_APB2Periph_GPIOA
#define FLASH_SPI_MISO_SOURCE GPIO_PinSource6
#define FLASH_SPI_MISO_AF GPIO_AF_SPI1

#define FLASH_SPI_MOSI_PIN GPIO_Pin_7
#define FLASH_SPI_MOSI_GPIO_PORT GPIOA
#define FLASH_SPI_MOSI_GPIO_CLK RCC_APB2Periph_GPIOA
#define FLASH_SPI_MOSI_SOURCE GPIO_PinSource7
#define FLASH_SPI_MOSI_AF GPIO_AF_SPI1

#define FLASH_CS_PIN GPIO_Pin_3
#define FLASH_CS_GPIO_PORT GPIOA
#define FLASH_CS_GPIO_CLK RCC_APB2Periph_GPIOA

  void flash_init(void);
  void flash_deinit(void);
  void flash_erase_sector(uint32_t SectorAddr);
  void flash_erase_block(void);
  void flash_write_page(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
  void flash_write_buffer(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
  void flash_read_buffer(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
  uint32_t flash_read_id(void);
  void flash_start_read_sequence(uint32_t ReadAddr);

  uint8_t flash_read_byte(void);
  uint8_t flash_send_byte(uint8_t byte);
  uint16_t flash_send_half_word(uint16_t HalfWord);
  void flash_write_enable(void);
  void flash_wait_for_write_end(void);

  inline void flash_cs_low()
  {
    GPIO_ResetBits(FLASH_CS_GPIO_PORT, FLASH_CS_PIN);
  }

  inline void flash_cs_high()
  {
    GPIO_SetBits(FLASH_CS_GPIO_PORT, FLASH_CS_PIN);
  }

#ifdef __cplusplus
}
#endif

#endif
