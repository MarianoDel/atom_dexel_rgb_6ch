#ifndef SPI_H
#define SPI_H

#define SPI_RXNE	0x0001
#define SPI_TXE		0x0002
#define SPI_BSY		0x0080
#define SPI_OVR		0x0040

//Clock Peripherals
#define RCC_SPI1_CLK 			(RCC->APB2ENR & 0x00001000)
#define RCC_SPI1_CLK_ON 		RCC->APB2ENR |= 0x00001000
#define RCC_SPI1_CLK_OFF		RCC->APB2ENR &= ~0x00001000


/* Uncomment the line below if you will use the SPI peripheral as a Master */
#define SPI_MASTER
/* Uncomment the line below if you will use the SPI peripheral as a Slave */
//#define SPI_SLAVE

/* Uncomment the size of data to be transmetted (only one data size must be selected) */
#define SPI_DATASIZE_8
//#define SPI_DATASIZE_7
//#define SPI_DATASIZE_6
//#define SPI_DATASIZE_5


/* Communication boards SPIx Interface */
#define SPIx                             SPI1
#define SPIx_CLK                         RCC_APB2Periph_SPI1
#define SPIx_IRQn                        SPI1_IRQn
#define SPIx_IRQHandler                  SPI1_IRQHandler


  // #define SPIx_SCK_PIN                     GPIO_Pin_3
  // #define SPIx_SCK_GPIO_PORT               GPIOB
  // #define SPIx_SCK_GPIO_CLK                RCC_AHBPeriph_GPIOB
  // #define SPIx_SCK_SOURCE                  GPIO_PinSource3
  // #define SPIx_SCK_AF                      GPIO_AF_0
  //
  // #define SPIx_MISO_PIN                    GPIO_Pin_4
  // #define SPIx_MISO_GPIO_PORT              GPIOB
  // #define SPIx_MISO_GPIO_CLK               RCC_AHBPeriph_GPIOB
  // #define SPIx_MISO_SOURCE                 GPIO_PinSource4
  // #define SPIx_MISO_AF                     GPIO_AF_0
  //
  // #define SPIx_MOSI_PIN                    GPIO_Pin_5
  // #define SPIx_MOSI_GPIO_PORT              GPIOB
  // #define SPIx_MOSI_GPIO_CLK               RCC_AHBPeriph_GPIOB
  // #define SPIx_MOSI_SOURCE                 GPIO_PinSource5
  // #define SPIx_MOSI_AF                     GPIO_AF_0

#define TXBUFFERSIZE                     16
#define RXBUFFERSIZE                     TXBUFFERSIZE

/* Define data size and data masks */
#ifdef SPI_DATASIZE_8
 #define SPI_DATASIZE                     SPI_DataSize_8b
 #define SPI_DATAMASK                     (uint8_t)0xFF
#elif defined (SPI_DATASIZE_7)
 #define SPI_DATASIZE                     SPI_DataSize_7b
 #define SPI_DATAMASK                     (uint8_t)0x7F
#elif defined (SPI_DATASIZE_6)
 #define SPI_DATASIZE                     SPI_DataSize_6b
 #define SPI_DATAMASK                     (uint8_t)0x3F
#elif defined (SPI_DATASIZE_5)
 #define SPI_DATASIZE                     SPI_DataSize_5b
 #define SPI_DATAMASK                     (uint8_t)0x1F
#else
 #error " Select at least one data size in main.h file "
#endif


//-------- Functions -------------
void SPI_Config(void);
void Send_SPI (unsigned char * , unsigned char);
unsigned char Send_Receive_SPI (unsigned char a);
void Wait_SPI_Busy (void);
void Send_SPI_Multiple (unsigned char);
void Send_SPI_Single (unsigned char);
unsigned char Receive_SPI_Single (void);

#endif
