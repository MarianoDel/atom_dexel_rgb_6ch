#include "spi.h"
#include "stm32f0xx.h"






//-------------------------- EXTERNAL VARIABLES ------------------------
extern volatile unsigned char TxBuffer_SPI [];
extern volatile unsigned char RxBuffer_SPI [];
extern volatile unsigned char *pspi_tx;
extern volatile unsigned char *pspi_rx;
extern volatile unsigned char spi_bytes_left;
//extern SPI_InitTypeDef  SPI_InitStructure;

//-------------------------- GLOBAL VARIABLES --------------------------
//SPI_InitTypeDef  SPI_InitStructure;

//--------------------------- FUNCTIONS --------------------------------

/**
  * @brief  Configures the SPI Peripheral.
  * @param  None
  * @retval None
  */
void SPI_Config(void)
{
	//Habilitar Clk
	if (!RCC_SPI1_CLK)
		RCC_SPI1_CLK_ON;

	//Configuracion SPI
	SPIx->CR1 = 0;
	//SPI speed; clk / 256; master
	SPIx->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;
	//CPOL High; CPHA second clock
	SPIx->CR1 |= SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_SSM | SPI_CR1_SSI;

	//thresh 8 bits; data 8 bits;
	SPIx->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;

	SPIx->CR1 |= SPI_CR1_SPE;		//habilito periferico
}

unsigned char Send_Receive_SPI (unsigned char a)
{
	unsigned char rx;
	unsigned char j;

	//primero limpio buffer rx spi
	while ((SPIx->SR & SPI_SR_RXNE) == 1)
	{
		rx = SPIx->DR & 0x0F;
	}

	//espero que haya lugar en el buffer
	while ((SPIx->SR & SPI_TXE) == 0);

	*(__IO uint8_t *) ((uint32_t)SPIx + (uint32_t)0x0C) = a; //evito enviar 16bits problemas de compilador

    //espero tener el dato en RX
    for (j = 0; j < 150; j++)
    {
    	asm("nop");
    }

    return (SPIx->DR & 0x0F);
}

void Wait_SPI_Busy (void)
{
	//espero que se transfiera el dato
	while ((SPIx->SR & SPI_BSY) != 0);
}
void Send_SPI_Multiple (unsigned char a)
{
	//espero que haya lugar en el buffer
	while ((SPIx->SR & SPI_TXE) == 0);

	//*(__IO uint8_t *) SPIx->DR = a;
	*(__IO uint8_t *) ((uint32_t)SPIx + (uint32_t)0x0C) = a; //evito enviar 16bits problemas de compilador

}

void Send_SPI_Single (unsigned char a)
{
	//espero que se libere el buffer
	while ((SPIx->SR & SPI_TXE) == 0);

	//tengo espacio
	//SPIx->DR = a;
	//SPIx->DR = a;
	*(__IO uint8_t *) ((uint32_t)SPIx + (uint32_t)0x0C) = a; //evito enviar 16bits problemas de compilador

	//espero que se transfiera el dato
	while ((SPIx->SR & SPI_BSY) != 0);
}

unsigned char Receive_SPI_Single (void)
{
	unsigned char dummy;

	//espero que se libere el buffer
	while (((SPIx->SR & SPI_TXE) == 0) || ((SPIx->SR & SPI_BSY) != 0));

	//limpio buffer RxFIFO
	while ((SPIx->SR & SPI_RXNE) != 0)
		dummy = SPIx->DR;

	*(__IO uint8_t *) ((uint32_t)SPIx + (uint32_t)0x0C) = 0xff; //evito enviar 16bits problemas de compilador

	//espero que se transfiera el dato
	while ((SPIx->SR & SPI_BSY) != 0);

	dummy = (unsigned char) SPIx->DR;
	return dummy;
}
