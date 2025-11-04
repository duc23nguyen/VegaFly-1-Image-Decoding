#include "flash.h"
#include "stm32f10x_spi.h"

void flash_init(void)
{

  RCC_APB2PeriphClockCmd(FLASH_CS_GPIO_CLK | FLASH_SPI_MOSI_GPIO_CLK | FLASH_SPI_MISO_GPIO_CLK | FLASH_SPI_SCK_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(FLASH_SPI_CLK, ENABLE);

  // Congifure SCK
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(FLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  // Congifure MOSI
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
  GPIO_Init(FLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  // Congifure MISO
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(FLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  // Configure CS
  GPIO_InitStructure.GPIO_Pin = FLASH_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(FLASH_CS_GPIO_PORT, &GPIO_InitStructure);
  flash_cs_high(); // Deselect CS

  // Configure SPI
  SPI_InitTypeDef SPI_InitStructure;
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(FLASH_SPI, &SPI_InitStructure);
  SPI_Cmd(FLASH_SPI, ENABLE);
}

void flash_deinit(void)
{
}

void flash_erase_sector(uint32_t SectorAddr)
{
  flash_write_enable();
  flash_cs_low();
  flash_send_byte(FLASH_CMD_SE);
  flash_send_byte((SectorAddr & 0xFF0000) >> 16);
  flash_send_byte((SectorAddr & 0xFF00) >> 8);
  flash_send_byte(SectorAddr & 0xFF);
  flash_cs_high();
  flash_wait_for_write_end();
}

void flash_erase_block(void)
{
  flash_write_enable();
  flash_cs_low();
  flash_send_byte(FLASH_CMD_BE);
  flash_cs_high();
  flash_wait_for_write_end();
}

void flash_write_page(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  flash_write_enable();
  flash_cs_low();
  flash_send_byte(FLASH_CMD_WRITE);
  flash_send_byte((WriteAddr & 0xFF0000) >> 16);
  flash_send_byte((WriteAddr & 0xFF00) >> 8);
  flash_send_byte(WriteAddr & 0xFF);

  while (NumByteToWrite--)
  {
    flash_send_byte(*pBuffer);
    pBuffer++;
  }

  flash_cs_high();
  flash_wait_for_write_end();
}

void flash_write_buffer(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % FLASH_SPI_PAGESIZE;
  count = FLASH_SPI_PAGESIZE - Addr;
  NumOfPage = NumByteToWrite / FLASH_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

  if (Addr == 0)
  {
    if (NumOfPage == 0)
    {
      flash_write_page(pBuffer, WriteAddr, NumByteToWrite);
    }
    else
    {
      while (NumOfPage--)
      {
        flash_write_page(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
        WriteAddr += FLASH_SPI_PAGESIZE;
        pBuffer += FLASH_SPI_PAGESIZE;
      }

      flash_write_page(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else
  {
    if (NumOfPage == 0)
    {
      if (NumOfSingle > count)
      {
        temp = NumOfSingle - count;

        flash_write_page(pBuffer, WriteAddr, count);
        WriteAddr += count;
        pBuffer += count;

        flash_write_page(pBuffer, WriteAddr, temp);
      }
      else
      {
        flash_write_page(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else
    {
      NumByteToWrite -= count;
      NumOfPage = NumByteToWrite / FLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

      flash_write_page(pBuffer, WriteAddr, count);
      WriteAddr += count;
      pBuffer += count;

      while (NumOfPage--)
      {
        flash_write_page(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
        WriteAddr += FLASH_SPI_PAGESIZE;
        pBuffer += FLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        flash_write_page(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

void flash_read_buffer(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  flash_cs_low();
  flash_send_byte(FLASH_CMD_READ);
  flash_send_byte((ReadAddr & 0xFF0000) >> 16);
  flash_send_byte((ReadAddr & 0xFF00) >> 8);
  flash_send_byte(ReadAddr & 0xFF);

  while (NumByteToRead--)
  {
    *pBuffer = flash_send_byte(FLASH_DUMMY_BYTE);
    pBuffer++;
  }

  flash_cs_high();
}

uint32_t flash_read_id(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
  flash_cs_low();
  flash_send_byte(0x9F);
  Temp0 = flash_send_byte(FLASH_DUMMY_BYTE);
  Temp1 = flash_send_byte(FLASH_DUMMY_BYTE);
  Temp2 = flash_send_byte(FLASH_DUMMY_BYTE);
  flash_cs_high();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
  return Temp;
}

void flash_start_read_sequence(uint32_t ReadAddr)
{
  flash_cs_low();
  flash_send_byte(FLASH_CMD_READ);
  flash_send_byte((ReadAddr & 0xFF0000) >> 16);
  flash_send_byte((ReadAddr & 0xFF00) >> 8);
  flash_send_byte(ReadAddr & 0xFF);
}

uint8_t flash_read_byte(void)
{
  return (flash_send_byte(FLASH_DUMMY_BYTE));
}

uint8_t flash_send_byte(uint8_t byte)
{
  while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET)
    ;
  SPI_I2S_SendData(FLASH_SPI, byte);
  while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET)
    ;
  return SPI_I2S_ReceiveData(FLASH_SPI);
}

uint16_t FLASH_SendHalfWord(uint16_t HalfWord)
{
  while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET)
    ;
  SPI_I2S_SendData(FLASH_SPI, HalfWord);
  while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET)
    ;
  return SPI_I2S_ReceiveData(FLASH_SPI);
}

void flash_write_enable(void)
{
  flash_cs_low();
  flash_send_byte(FLASH_CMD_WREN);
  flash_cs_high();
}

void flash_wait_for_write_end(void)
{
  uint8_t flashstatus = 0;
  flash_cs_low();
  flash_send_byte(FLASH_CMD_RDSR);

  do
  {
    flashstatus = flash_send_byte(FLASH_DUMMY_BYTE);

  } while ((flashstatus & FLASH_WIP_FLAG) == SET);

  flash_cs_high();
}
