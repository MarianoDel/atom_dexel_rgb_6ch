/**
  ******************************************************************************
  * @file    Template_2/stm32f0_gpio.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   GPIO functions.
  ******************************************************************************
  * @attention
  *
  * Use this functions to configure global inputs/outputs.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "stm32f0xx.h"
#include "hard.h"



//--- Private typedef ---//
//--- Private define ---//
//--- Private macro ---//
//--- Private variables ---//
//--- Private function prototypes ---//
//--- Private functions ---//

//-------------------------------------------//
// @brief  GPIO configure.
// @param  None
// @retval None
//------------------------------------------//
void GPIO_Config (void)
{
	unsigned long temp;

	//--- MODER ---//
	//00: Input mode (reset state)
	//01: General purpose output mode
	//10: Alternate function mode
	//11: Analog mode

	//--- OTYPER ---//
	//These bits are written by software to configure the I/O output type.
	//0: Output push-pull (reset state)
	//1: Output open-drain

	//--- ORSPEEDR ---//
	//These bits are written by software to configure the I/O output speed.
	//x0: Low speed.
	//01: Medium speed.
	//11: High speed.
	//Note: Refer to the device datasheet for the frequency.

	//--- PUPDR ---//
	//These bits are written by software to configure the I/O pull-up or pull-down
	//00: No pull-up, pull-down
	//01: Pull-up
	//10: Pull-down
	//11: Reserved


#ifdef GPIOA_ENABLE

	//--- GPIO A ---//
	if (!GPIOA_CLK)
		GPIOA_CLK_ON;

#ifdef VER_1_0
	temp = GPIOA->MODER;	//2 bits por pin
	temp &= 0xFC0C0000;		//PA0 (analog input); PA1 output; PA2 PA3 input; PA4 PA5 output; PA6 PA7(alternative)
	temp |= 0x0160A507;		//PA8 input; PA10 alternative;  PA11 PA12 output
	//temp |= 0x0140A504;
	GPIOA->MODER = temp;

	temp = GPIOA->OTYPER;	//1 bit por pin
	temp &= 0xFFFFE7E3;
	temp |= 0x00000000;		//PA0 a PA6push pull
	GPIOA->OTYPER = temp;

	temp = GPIOA->OSPEEDR;	//2 bits por pin
	temp &= 0xFC3FC000;
	temp |= 0x00000000;		//low speed
	GPIOA->OSPEEDR = temp;

	temp = GPIOA->PUPDR;	//2 bits por pin
	temp &= 0xFFFCFF0F;
	//temp |= 0x00020050;	//PA8 con pulldown
	temp |= 0x00010050;		//PA8 con pullup
	GPIOA->PUPDR = temp;
#endif

#if ((defined VER_1_1) || (defined VER_1_2) || (defined VER_1_3))
	temp = GPIOA->MODER;	//2 bits por pin
	temp &= 0xFC0C0000;		//PA0 (analog input); PA1 output; PA2 PA3 input; PA4 PA5 output; PA6 PA7(alternative)
	temp |= 0x0160A507;		//PA8 input; PA10 alternative;  PA11 PA12 output
	//temp |= 0x0140A504;
	GPIOA->MODER = temp;

	temp = GPIOA->OTYPER;	//1 bit por pin
	temp &= 0xFFFFE7C3;
	temp |= 0x00000000;		//PA0 a PA6push pull;	PA5 PA11 PA12 push pull
	GPIOA->OTYPER = temp;

	temp = GPIOA->OSPEEDR;	//2 bits por pin
	temp &= 0xFC3FC000;
	temp |= 0x00000000;		//low speed
	GPIOA->OSPEEDR = temp;

	temp = GPIOA->PUPDR;	//2 bits por pin
	temp &= 0xFFFCFF0F;
	//temp |= 0x00020050;	//PA8 con pulldown
	temp |= 0x00010050;		//PA8 con pullup
	GPIOA->PUPDR = temp;
#endif

	//Alternate Fuction
	//GPIOA->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1


#endif

#ifdef GPIOB_ENABLE

	//--- GPIO B ---//
	if (!GPIOB_CLK)
		GPIOB_CLK_ON;

	temp = GPIOB->MODER;	//2 bits por pin
	temp &= 0xFFFFC330;		//PB0 PB1 PB3 PB5 (alternative) PB6 out
	temp |= 0x0000188A;
	GPIOB->MODER = temp;

	temp = GPIOB->OTYPER;	//1 bit por pin
	temp &= 0xFFFFFFBC;
	temp |= 0x00000000;		//PB1 push pull
	GPIOB->OTYPER = temp;

	temp = GPIOB->OSPEEDR;	//2 bits por pin
	temp &= 0xFFFFCFF0;
	temp |= 0x00000000;		//low speed
	GPIOB->OSPEEDR = temp;

	temp = GPIOB->PUPDR;	//2 bits por pin
	temp &= 0xFFFFCFF0;
	temp |= 0x00000000;
	GPIOB->PUPDR = temp;

	//Alternate Fuction
	//GPIOB->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1

#endif

#ifdef GPIOF_ENABLE

	//--- GPIO F ---//
	if (!GPIOF_CLK)
		GPIOF_CLK_ON;

	temp = GPIOF->MODER;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->MODER = temp;

	temp = GPIOF->OTYPER;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->OTYPER = temp;

	temp = GPIOF->OSPEEDR;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->OSPEEDR = temp;

	temp = GPIOF->PUPDR;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->PUPDR = temp;

#endif

#ifdef WITH_EXTI
		//Interrupt en PA8
		if (!SYSCFG_CLK)
			SYSCFG_CLK_ON;

		SYSCFG->EXTICR[0] = 0x00000000; //Select Port A
		SYSCFG->EXTICR[1] = 0x00000000; //Select Port A
		EXTI->IMR |= 0x0100; 			//Corresponding mask bit for interrupts PA8
		EXTI->EMR |= 0x0000; 			//Corresponding mask bit for events
		EXTI->RTSR |= 0x0100; 			//Pin Interrupt line on rising edge PA8
		EXTI->FTSR |= 0x0100; 			//Pin Interrupt line on falling edge PA8

		NVIC_EnableIRQ(EXTI4_15_IRQn);
		NVIC_SetPriority(EXTI4_15_IRQn, 2);
#endif
}

inline void EXTIOff (void)
{
	EXTI->IMR &= ~0x00000100;
}

inline void EXTIOn (void)
{
	EXTI->IMR |= 0x00000100;
}

//--- end of file ---//
