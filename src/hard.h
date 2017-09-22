/*
 * hard.h
 *
 *  Created on: 28/11/2013
 *      Author: Mariano
 */

#ifndef HARD_H_
#define HARD_H_

#include "stm32f0xx.h"

//-- Defines For Configuration -------------------
//---- Configuration for Hardware Versions -------
#define VER_1_0

//---- Configuration for Firmware ----------------
#define FIRMWARE_VER_1_0
//#define WITH_GRANDMASTER	//tiene un quinto canal que funciona como grandmaster de los 4 primeros

//#define RGB_FOR_CHANNELS
#define RGB_FOR_PROGRAMS

//------ Configuration for Firmware-Channels -----
#ifdef RGB_FOR_CHANNELS
#define RGB_FOR_CHANNELS_NUM	2
//#define RGB_FOR_CHANNELS_NUM	4
#if (RGB_FOR_CHANNELS_NUM == 2)		//SYNCHRO
//#define RGB_FOR_CHANNELS_CH1_CH3_SYNC
//#define RGB_FOR_CHANNELS_CH2_CH4_SYNC
#endif
//-------- Configuration for Outputs-Channels -----
//#define RGB_OUTPUT_LM317
#define RGB_OUTPUT_MOSFET_KIRNO
//#define RGB_OUTPUT_CAT
#endif

//---- Configuration for Firmware-Programs --------
#ifdef RGB_FOR_PROGRAMS
#define WHITE_AS_IN_RGB		//el blanco lo forma con los 3 colores
//#define WHITE_AS_WHITE	//el blanco tiene leds blancos individuales

//-------- Configuration for Outputs-Firmware ------
#define RGB_OUTPUT_LM317
//#define RGB_OUTPUT_MOSFET_KIRNO	//muestra CAT en display
//#define RGB_OUTPUT_CAT			//muestra CAT en display
#endif

//-- End Of Defines For Configuration ---------------

//GPIOA pin0	adc LM335 input

//GPIOA pin1
#define LED ((GPIOA->ODR & 0x0002) != 0)
#define LED_ON	GPIOA->BSRR = 0x00000002
#define LED_OFF GPIOA->BSRR = 0x00020000

//GPIOA pin2
#define S1_PIN ((GPIOA->IDR & 0x0004) == 0)

//GPIOA pin3
#define S2_PIN ((GPIOA->IDR & 0x0008) == 0)

//GPIOA pin4
#define CTRL_FAN ((GPIOA->ODR & 0x0010) != 0)
#define CTRL_FAN_ON GPIOA->BSRR = 0x00000010
#define CTRL_FAN_OFF GPIOA->BSRR = 0x00100000

//GPIOA pin5
#define PWR_DS3 ((GPIOA->ODR & 0x0020) != 0)
#define PWR_DS3_ON GPIOA->BSRR = 0x00000020
#define PWR_DS3_OFF GPIOA->BSRR = 0x00200000

//GPIOA pin6
//GPIOA pin7
//GPIOB pin0
//GPIOB pin1	TIM3 CH1 CH2 CH3 CH4

//GPIOA pin8
//GPIOA pin9	TIM1 CH1 CH2

//GPIOA pin10	USART1 rx Input

//GPIOA pin11
#define PWR_DS2 ((GPIOA->ODR & 0x0800) != 0)
#define PWR_DS2_ON GPIOA->BSRR = 0x00000800
#define PWR_DS2_OFF GPIOA->BSRR = 0x08000000

//GPIOA pin12
#define PWR_DS1 ((GPIOA->ODR & 0x1000) != 0)
#define PWR_DS1_ON GPIOA->BSRR = 0x00001000
#define PWR_DS1_OFF GPIOA->BSRR = 0x10000000

//GPIOA pin13
//GPIOA pin14	NC

//GPIOA pin15
#define DMX_INPUT ((GPIOA->IDR & 0x8000) != 0)

//GPIOB pin3	SPI clk

//GPIOB pin4
#define PWR_DS4 ((GPIOB->ODR & 0x0010) != 0)
#define PWR_DS4_ON GPIOB->BSRR = 0x00000010
#define PWR_DS4_OFF GPIOB->BSRR = 0x00100000

//GPIOB pin5	SPI Mosi

//GPIOB pin6
#define OE ((GPIOB->ODR & 0x0040) != 0)
#define OE_OFF GPIOB->BSRR = 0x00000040
#define OE_ON GPIOB->BSRR = 0x00400000

//GPIOB pin7 NC

#define CH_IN_TEMP ADC_Channel_0
#define LAST_NUMBER		5
// #define LAST_NUMBER_SPEAK		5
//
// #define DOOR_ROOF	200
// #define DOOR_THRESH	180

//ESTADOS DEL PROGRAMA PRINCIPAL
#define MAIN_INIT					0

#define MAIN_DMX_CHECK_CHANNEL				10
#define MAIN_DMX_CHECK_CHANNEL_B			11
#define MAIN_DMX_CHECK_CHANNEL_SELECTED		12
#define MAIN_DMX_CHECK_CHANNEL_S1			13
#define MAIN_DMX_CHECK_CHANNEL_S2			14
#define MAIN_DMX_SAVE_CONF					15
#define MAIN_DMX_NORMAL						16

#define MAIN_MAN_PX_CHECK			20
#define MAIN_MAN_PX_CHECK_B			21
#define MAIN_MAN_PX_CHECK_DEEP		22
#define MAIN_MAN_PX_CHECK_S1		23
#define MAIN_MAN_PX_CHECK_S2		24
#define MAIN_MAN_PX_SAVE_CONF		25
#define MAIN_MAN_PX_NORMAL			26

#define MAIN_TEMP_OVERLOAD			30
#define MAIN_TEMP_OVERLOAD_B		31

//---- Temperaturas en el LM335
//37	2,572
//40	2,600
//45	2,650
//50	2,681
//55	2,725
//60	2,765
#define TEMP_IN_30		3226
#define TEMP_IN_35		3279
#define TEMP_IN_50		3434
#define TEMP_IN_65		3591
#define TEMP_DISCONECT		4000

//ESTADOS DEL DISPLAY EN RGB_FOR_CAT
#define SHOW_CHANNELS	0
#define SHOW_NUMBERS	1

//ESTADOS DEL DISPLAY
#define DISPLAY_INIT		0
#define DISPLAY_SENDING		1
#define DISPLAY_SHOWING		2
#define DISPLAY_WAITING		3

#define DISPLAY_ZERO	10
#define DISPLAY_POINT	11
#define DISPLAY_LINE	12
#define DISPLAY_REMOTE	13
#define DISPLAY_PROG	14
#define DISPLAY_E		15
#define DISPLAY_C		16
#define DISPLAY_H		17
#define DISPLAY_1P		18
#define DISPLAY_S		19
#define DISPLAY_A		20
#define DISPLAY_T		21
#define DISPLAY_R		22
#define DISPLAY_G		23
#define DISPLAY_N		24
#define DISPLAY_NONE	0

#define DISPLAY_P	DISPLAY_PROG

#define DISPLAY_DS4		0x08
#define DISPLAY_DS3		0x04
#define DISPLAY_DS2		0x02
#define DISPLAY_DS1		0x01

#define DISPLAY_TIMER_RELOAD	3		//333Hz / 4
#define SWITCHES_TIMER_RELOAD	10
#define BLINKING_UPDATE 	12			//son 18 ms de update * num elegido

#define SWITCHES_THRESHOLD_FULL	300		//3 segundos
#define SWITCHES_THRESHOLD_HALF	100		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//50 ms

#define TTIMER_FOR_CAT_DISPLAY			2000	//tiempo entre que dice canal y el numero
#define TIMER_STANDBY_TIMEOUT_REDUCED	2000	//reduced 2 segs
#define TIMER_STANDBY_TIMEOUT			6000	//6 segundos
#define DMX_DISPLAY_SHOW_TIMEOUT		30000	//30 segundos

#define S_FULL		10
#define S_HALF		3
#define S_MIN		1
#define S_NO		0

#define FUNCTION_DMX	1
#define FUNCTION_MAN	2
#define FUNCTION_CAT	FUNCTION_MAN

#endif /* HARD_H_ */
