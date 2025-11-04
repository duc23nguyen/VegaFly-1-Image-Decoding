#include "i2c.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_i2c.h"

DMA_InitTypeDef I2CDMA_InitStructure;
__IO uint32_t I2CDirection = I2C_DIRECTION_TX;
__IO uint32_t NumbOfBytes1;
__IO uint32_t NumbOfBytes2;
__IO uint8_t Address;

/**
 * @brief Prepares the I2Cx slave for transmission.
 * @param I2Cx: I2C1 or I2C2.
 * @param Mode: DMA or Interrupt having the highest priority in the application.
 * @retval : None.
 */
void I2C_Slave_BufferReadWrite(I2C_TypeDef *I2Cx, I2C_ProgrammingModel Mode)
{
    /* Enable Event IT needed for ADDR and STOPF events ITs */
    I2Cx->CR2 |= I2C_IT_EVT;

    /* Enable Error IT */
    I2Cx->CR2 |= I2C_IT_ERR;

    if (Mode == DMA) /* I2Cx Slave Transmission using DMA */
    {
        /* Enable I2Cx DMA requests */
        I2Cx->CR2 |= CR2_DMAEN_Set;
    }

    else /* I2Cx Slave Transmission using Interrupt with highest priority in the application */
    {
        /* Enable Buffer IT (TXE and RXNE ITs) */
        I2Cx->CR2 |= I2C_IT_BUF;
    }
}

/**
 * @brief  Initializes peripherals: I2Cx, GPIO, DMA channels .
 * @param  None
 * @retval None
 */
void I2C_LowLevel_Init(I2C_TypeDef *I2Cx)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    /* GPIOB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    /* Enable the DMA1 clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    if (I2Cx == I2C1)
    {
        /* I2C1 clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        /* I2C1 SDA and SCL configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Enable I2C1 reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
        
        /* Release I2C1 from reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    }

    else /* I2Cx = I2C2 */
    {
        /* I2C2 clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

        /* I2C1 SDA and SCL configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Enable I2C2 reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);

        /* Release I2C2 from reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
    }

    /* I2C1 and I2C2 configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = OwnAddress1;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_InitStructure.I2C_OwnAddress1 = OwnAddress2;
    I2C_Init(I2C2, &I2C_InitStructure);

    if (I2Cx == I2C1)
    {
        /* I2C1 TX DMA Channel configuration */
        DMA_DeInit(I2C1_DMA_CHANNEL_TX);
        I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0; /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_BufferSize = 0xFFFF;          /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        I2CDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        I2CDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
        I2CDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        I2CDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        I2CDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        I2CDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(I2C1_DMA_CHANNEL_TX, &I2CDMA_InitStructure);

        /* I2C1 RX DMA Channel configuration */
        DMA_DeInit(I2C1_DMA_CHANNEL_RX);
        DMA_Init(I2C1_DMA_CHANNEL_RX, &I2CDMA_InitStructure);
    }

    else /* I2Cx = I2C2 */
    {
        /* I2C2 TX DMA Channel configuration */
        DMA_DeInit(I2C2_DMA_CHANNEL_TX);
        I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C2_DR_Address;
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0; /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_BufferSize = 0xFFFF;          /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        I2CDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        I2CDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
        I2CDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        I2CDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        I2CDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        I2CDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(I2C2_DMA_CHANNEL_TX, &I2CDMA_InitStructure);

        /* I2C2 RX DMA Channel configuration */
        DMA_DeInit(I2C2_DMA_CHANNEL_RX);
        DMA_Init(I2C2_DMA_CHANNEL_RX, &I2CDMA_InitStructure);
    }
}
