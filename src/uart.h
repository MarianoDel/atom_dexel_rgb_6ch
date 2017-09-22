/**
  ******************************************************************************
  * @file    Template_2/main.h
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Header for gpio module
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F0X_UART_H
#define __STM32F0X_UART_H

//--- Exported types ---//
//--- Exported constants ---//
//--- Exported macro ---//

//--- Enable USART -----//
#define USE_USART1			//puede ser utilizado con simultaneamente con USART2
//#define USE_USART2

#define USART1_CLK (RCC->APB2ENR & 0x00004000)
#define USART1_CLK_ON RCC->APB2ENR |= 0x00004000
#define USART1_CLK_OFF RCC->APB2ENR &= ~0x00004000

#define USART2_CLK (RCC->APB1ENR & 0x00020000)
#define USART2_CLK_ON RCC->APB1ENR |= 0x00020000
#define USART2_CLK_OFF RCC->APB1ENR &= ~0x00020000

#define USART_9600		5000
#define USART_115200		416
#define USART_250000		192

#ifdef USE_USART1
#define USART1_RX_DISA	USART1->CR1 &= 0xfffffffb
#define USART1_RX_ENA	USART1->CR1 |= 0x04
#endif
#ifdef USE_USART2
#define USART2_RX_DISA	USART2->CR1 &= 0xfffffffb
#define USART2_RX_ENA	USART2->CR1 |= 0x04
#endif

//--- Exported functions ---//
unsigned char ReadUsart1Buffer (unsigned char *, unsigned short);
unsigned char ReadUsart2Buffer (unsigned char *, unsigned short);

void USART1Config(void);
void USART1_IRQHandler(void);
void Usart1Send (char *);
void Usart1SendUnsigned(unsigned char *, unsigned char);
void Usart1SendSingle(unsigned char );

void USART2_IRQHandler(void);
void USART2Config(void);
void Usart2Send (char *);
void Usart2SendUnsigned(unsigned char *, unsigned char);
void Usart2SendSingle(unsigned char );


#endif
//--- END OF FILE ---//
