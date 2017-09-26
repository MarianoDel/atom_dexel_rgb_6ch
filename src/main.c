/**
  ******************************************************************************
  * @file    Template_2/main.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Use this template for new projects with stm32f0xx family.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "uart.h"
#include "spi.h"

#include "core_cm0.h"
#include "adc.h"
#include "flash_program.h"
#include "programs_functions.h"

#include "tim.h"

//#include <stdio.h>
//#include <string.h>




//--- VARIABLES EXTERNAS ---//
volatile unsigned char timer_1seg = 0;

volatile unsigned short timer_led_comm = 0;
volatile unsigned short timer_for_cat_switch = 0;
volatile unsigned short timer_for_cat_display = 0;
volatile unsigned char buffrx_ready = 0;
volatile unsigned char *pbuffrx;
volatile unsigned short wait_ms_var = 0;

volatile unsigned char TxBuffer_SPI [TXBUFFERSIZE];
volatile unsigned char RxBuffer_SPI [RXBUFFERSIZE];
volatile unsigned char *pspi_tx;
volatile unsigned char *pspi_rx;
volatile unsigned char spi_bytes_left = 0;

// ------- Externals del DMX -------
volatile unsigned char Packet_Detected_Flag;
volatile unsigned char dmx_receive_flag = 0;
volatile unsigned char DMX_channel_received = 0;
volatile unsigned char DMX_channel_selected = 1;
volatile unsigned char DMX_channel_quantity = 4;		//TODO: mejorar esto, relacionar con canales a recibir

volatile unsigned char data1[512];
//static unsigned char data_back[10];
volatile unsigned char data[14];			//TODO: mejorar esto es el doble de los canales a recibir + grandmaster

volatile unsigned short prog_timer = 0;

//--- VARIABLES GLOBALES ---//
parameters_typedef param_struct;

// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned short timer_dmx_display_show;
volatile unsigned char display_timer;
volatile unsigned char switches_timer;
volatile unsigned char filter_timer;
static __IO uint32_t TimingDelay;

//volatile unsigned char door_filter;
volatile unsigned char take_sample;
//volatile unsigned char move_relay;

volatile unsigned char secs = 0;
volatile unsigned short minutes = 0;

// ------- del display -------
unsigned char numbers[LAST_NUMBER];
unsigned char * p_numbers;

unsigned char last_digit;
unsigned char display_state;
unsigned char ds4_number;
unsigned char ds3_number;
unsigned char ds2_number;
unsigned char ds1_number;

unsigned char display_blinking_timer = 0;
unsigned char display_blinking = 0;
unsigned char display_was_on = 0;


// ------- de los switches -------
unsigned short s1;
unsigned short s2;

// ------- del DMX -------
volatile unsigned char signal_state = 0;
volatile unsigned char dmx_timeout_timer = 0;
//unsigned short tim_counter_65ms = 0;

// ------- de los filtros DMX -------
#define LARGO_F		32
#define DIVISOR_F	5
unsigned char vd0 [LARGO_F + 1];
unsigned char vd1 [LARGO_F + 1];
unsigned char vd2 [LARGO_F + 1];
unsigned char vd3 [LARGO_F + 1];
unsigned char vd4 [LARGO_F + 1];
unsigned char vd5 [LARGO_F + 1];


#define IDLE	0
#define LOOK_FOR_BREAK	1
#define LOOK_FOR_MARK	2
#define LOOK_FOR_START	3

//--- FUNCIONES DEL MODULO ---//
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);


unsigned short Get_Temp (void);
void Update_PWM (unsigned short);

// ------- del display -------
void UpdateDisplay (void);
void VectorToDisplay (unsigned char);
void ShowNumbers (unsigned short);
void SendSegment (unsigned char, unsigned char);
void ShowNumbersAgain (void);
unsigned char TranslateNumber (unsigned char);
unsigned short FromDsToChannel (void);
#define FromChannelToDs(X)	ShowNumbers((X))


// ------- de los switches -------
void UpdateSwitches (void);
unsigned char CheckS1 (void);
unsigned char CheckS2 (void);
#define TIMER_FOR_CAT_SW	200

// ------- del DMX -------
extern void EXTI4_15_IRQHandler(void);
#define DMX_TIMEOUT	20
void DMX_Ena(void);
void DMX_Disa(void);
unsigned char MAFilter (unsigned char, unsigned char *);

//--- FILTROS DE SENSORES ---//
#define LARGO_FILTRO 16
#define DIVISOR      4   //2 elevado al divisor = largo filtro
//#define LARGO_FILTRO 32
//#define DIVISOR      5   //2 elevado al divisor = largo filtro
unsigned short vtemp [LARGO_FILTRO + 1];
//unsigned short vpote [LARGO_FILTRO + 1];

//--- FIN DEFINICIONES DE FILTRO ---//


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
	unsigned char i;
	unsigned char main_state = 0;
	unsigned char last_function;
	unsigned char last_program, last_program_deep;
	unsigned short last_channel;
	unsigned short current_temp = 0;

//	unsigned char sw_state = 0;

#ifdef WITH_GRANDMASTER
	unsigned short acc = 0;
	unsigned char dummy = 0;
#endif
#ifdef RGB_FOR_CHANNELS
	unsigned char show_channels_state = 0;
	unsigned char fixed_data[2];		//la eleccion del usuario en los canales de 0 a 100
	unsigned char need_to_save = 0;
#endif
	parameters_typedef * p_mem_init;
	//!< At this stage the microcontroller clock setting is already configured,
    //   this is done through SystemInit() function which is called from startup
    //   file (startup_stm32f0xx.s) before to branch to application main.
    //   To reconfigure the default setting of SystemInit() function, refer to
    //   system_stm32f0xx.c file

	//GPIO Configuration.
	GPIO_Config();

	//ACTIVAR SYSTICK TIMER
	 if (SysTick_Config(48000))
	 {
		 while (1)	/* Capture error */
		 {
			if (LED)
				LED_OFF;
			else
				LED_ON;

			for (i = 0; i < 255; i++)
			{
				asm (	"nop \n\t"
						"nop \n\t"
						"nop \n\t" );
			}
		 }
	 }

	 //TIM Configuration.
 	TIM_1_Init();
 	TIM_3_Init();
 	TIM_14_Init();

	//--- PRUEBA PWM TIM1
	// while (1)
	// {
	// 	for (i = 0; i < 255; i++)
	// 	{
	// 	  	UV_PWM (i);
	// 	  	AMBER_PWM (i);
	// 		// Update_TIM1_CH1 (i);
	// 		// Update_TIM2_CH2 (i);
	//
	// 		Wait_ms(100);
	// 	}
	// }
	//--- FIN PRUEBA PWM TIM1

	 //--- PRUEBA LED Y OE ---
	//  while (1)
	//  {
	// 	 if (LED)
	// 	 {
	// 		 LED_OFF;
	// 		 OE_OFF;
	// 	 }
	// 	 else
	// 	 {
	// 		 LED_ON;
	// 		 OE_ON;
	// 	 }
	// 	 Wait_ms(1000);
	//  }
	 //--- FIN PRUEBA LED Y OE ---

	 /* SPI configuration ------------------------------------------------------*/
	 SPI_Config();

	 //DE PRODUCCION Y PARA PRUEBAS EN DMX
	 Packet_Detected_Flag = 0;
	 DMX_channel_selected = 1;
	 DMX_channel_quantity = 6;
	 USART1Config();

	 //arranco con todo apagado
	 DMX_Disa();

 	//--- PRUEBA DISPLAY ---
	// //  PWR_DS1_OFF;
	// //  PWR_DS2_OFF;
	// //  PWR_DS3_ON;
	//
	// //  PWR_DS1_OFF;
	// //  PWR_DS2_ON;
	// //  PWR_DS3_OFF;
	//
	//  PWR_DS1_ON;
	//  PWR_DS2_OFF;
	//  PWR_DS3_OFF;
	//
	//  main_state = TranslateNumber(DISPLAY_LINE);
	//  SendSegment(DISPLAY_DS1, main_state);
	//  while(1);
	 //--- FIN PRUEBA DISPLAY ---

 	//--- PRUEBA NUMEROS EN DISPLAY ---
	// i = 0;
	// ds4_number = DISPLAY_P;
	// while (1)
	// {
	// 	if (!timer_standby)
	// 	{
	// 		if (i <= 255)
	// 		{
	// 			ShowNumbers(i);
	// 			i++;
	// 		}
	// 		else
	// 			i = 0;
	//
	// 		timer_standby = 300;
	// 	}
	// 	UpdateDisplay ();
	// }
	//--- FIN PRUEBA NUMEROS EN DISPLAY ---


	 //--- PRUEBA TIM14 DMX
	//  ShowNumbers(0);
	//  while (1)
	//  {
	// 	 TIM14->CNT = 0;					//tick cada 1us
	// 	 TIM14->CR1 |= 0x0001;
	 //
	// 	 while ((TIM14->CNT) < 2000)	//2ms
	// 	 {
	// 	 }
	// 	 TIM14->CR1 &= ~0x0001;
	// 	 if (LED)
	// 		 LED_OFF;
	// 	 else
	// 		 LED_ON;
	 //
	// 	 UpdateDisplay ();
	//  }
	 //--- FIN PRUEBA TIM14 DMX

	 //--- PRUEBA USART
	 /*
	 EXTIOff();
	 USART_Config();
	 ShowNumbers(0);
	 while (1)
	 {
		 DMX_channel_received = 0;
		 data1[0] = 0;
		 data1[1] = 0;
		 data1[2] = 0;
		 USARTSend('M');
		 USARTSend('E');
		 USARTSend('D');
		 Wait_ms(1);
		 if ((data1[0] == 'M') && (data1[1] == 'E') && (data1[2] == 'D'))
			 LED_ON;
		 else
			 LED_OFF;
		 Wait_ms(200);
		 UpdateDisplay ();
	 }
	*/
	 //--- FIN PRUEBA USART

	 //--- PRUEBA EXTI PA8 con DMX
	 /*
	 ShowNumbers(0);
	 while (1)
	 {
		 //cuando tiene DMX mueve el LED
		 EXTIOn();
		 Wait_ms(200);
		 EXTIOff();
		 Wait_ms(200);
	 }
	 */
	 //--- FIN PRUEBA EXTI PA8 con DMX

	 //--- PRUEBA ADC
	 /*
		if (ADC_Conf() == 0)
		{
			while (1)
			{
				if (LED)
					LED_OFF;
				else
					LED_ON;

				Wait_ms(150);
			}
		}
		while (1)
		{
			i = ReadADC1(ADC_CH0);
			if (i > 2048)
				LED_ON;
			else
				LED_OFF;
			Wait_ms(50);
			i = i >> 4;
			 ShowNumbers (i);
		}
		*/
	//--- FIN ADC

		 //--- PRUEBA FAN
		//  while (1)
		//  {
		// 	 LED_ON;
		// 	 CTRL_FAN_ON;
		// 	 Wait_ms(300);
		// 	 LED_OFF;
		// 	 CTRL_FAN_OFF;
		// 	 Wait_ms(300);
		//  }
		//--- FIN PRUEBA FAN

	//--- PRUEBA CH0 CH1 CH2 CH3 DMX	inicializo mas arriba USART y variables
	// 	DMX_Ena();
	//  while (1)
	//  {
	// 		 if (Packet_Detected_Flag)
	// 		 {
	 //
	// 			 //llego un paquete DMX
	// 			 Packet_Detected_Flag = 0;
	// 			 //en data tengo la info
	// 			 ShowNumbers (data[0]);
	// 			 //LED_OFF;
	// 			 Update_TIM3_CH1 (data[0]);
	// 			 Update_TIM3_CH2 (data[0]);
	// 			 Update_TIM3_CH3 (data[0]);
	// 			 Update_TIM3_CH4 (data[0]);
	 //
	// 		 }
	 //
	// 	 UpdateDisplay ();
	// 	 UpdateSwitches ();
	 //
	//  }
	//--- FIN PRUEBA CH0 CH1 CH2 CH3 DMX

	//--- PRUEBA blinking de display y pulsadores
	//	//activar unsigned char sw_state en main
	// ds1_number = 1;
	// ds2_number = 2;
	// ds3_number = 3;
	// 	 while (1)
	// 	 {
	// 		 if (CheckS1() > S_NO)
	// 		 {
	// 			 //muevo el display en blinking
	// 			 display_blinking <<= 1;
	// 			 if (!(display_blinking & 0x07))
	// 				 display_blinking = DISPLAY_DS1;
	// 		 }
	// 		 else if (CheckS2() > S_NO)
	// 		 {
	// 			 //prendo o apago el blinking
	// 			 if (sw_state)
	// 			 {
	// 				 display_blinking = 0;
	// 				 sw_state = 0;
	// 			 }
	// 			 else
	// 			 {
	// 				 display_blinking = DISPLAY_DS1;
	// 				 sw_state = 1;
	// 			 }
	// 		 }
	// 		 UpdateDisplay ();
	// 		 UpdateSwitches ();
	// 	 }
	//--- FIN PRUEBA blinking de display y pulsadores

	//--- PRUEBA CHANNELS PWM
	// while (1)
	// {
	// 	for (i = 0; i < 255; i++)
	// 	{
	// 		RED_PWM (i);
	// 	  	GREEN_PWM (i);
	// 	  	BLUE_PWM (i);
	// 	  	WHITE_PWM (i);
	// 	  	UV_PWM (i);
	// 	  	AMBER_PWM (i);
	//
	// 		Wait_ms(100);
	// 	}
	// }
	//--- FIN PRUEBA CHANNELS PWM


	 //--- COMIENZO PROGRAMA DE PRODUCCION
	 RED_PWM (0);
	 GREEN_PWM (0);
	 BLUE_PWM (0);
	 WHITE_PWM (0);
	 UV_PWM (0);
	 AMBER_PWM (0);

	 //inicio cuestiones particulares
	 //iniciar variables de usao del programa segun funcion de memoria

	//ADC configuration.

	AdcConfig();

	LED_ON;
	Wait_ms(300);
	LED_OFF;

//muestro versiones de hardware, software y firmware
//-- HARDWARE --
#ifdef VER_1_3
	timer_standby = 1000;
	ds1_number = DISPLAY_H;				//Hardware
	ds2_number = DISPLAY_1P;			//1.
	ds3_number = 3;						//3
	while (timer_standby)
		UpdateDisplay();
#endif

#ifdef VER_1_2
	timer_standby = 1000;
	ds1_number = DISPLAY_H;				//Hardware
	ds2_number = DISPLAY_1P;			//1.
	ds3_number = 2;						//2
	while (timer_standby)
		UpdateDisplay();
#endif

#ifdef VER_1_1
	timer_standby = 1000;
	ds1_number = DISPLAY_H;				//Hardware
	ds2_number = DISPLAY_1P;			//1.
	ds3_number = 1;						//1
	while (timer_standby)
		UpdateDisplay();
#endif

#ifdef VER_1_0
	timer_standby = 1000;
	ds1_number = DISPLAY_H;				//Hardware
	ds2_number = DISPLAY_1P;			//1.
	ds3_number = DISPLAY_ZERO;			//0
	while (timer_standby)
		UpdateDisplay();
#endif

//-- SOFTWARE --
#ifdef FIRMWARE_VER_1_0
	timer_standby = 1000;
	ds1_number = DISPLAY_S;				//Software
	ds2_number = DISPLAY_1P;			//1.
	ds3_number = DISPLAY_ZERO;			//0
	while (timer_standby)
		UpdateDisplay();
#endif

//-- FIRMWARE --
#ifdef RGB_FOR_CHANNELS
	timer_standby = 1000;
	ds1_number = DISPLAY_C;				//Channels
	ds2_number = DISPLAY_H;				//
	ds3_number = DISPLAY_N;				//
	while (timer_standby)
		UpdateDisplay();
#endif

#ifdef RGB_FOR_PROGRAMS
	timer_standby = 1000;
	ds1_number = DISPLAY_P;				//PRG
	ds2_number = DISPLAY_R;				//
	ds3_number = DISPLAY_G;				//
	while (timer_standby)
		UpdateDisplay();
#endif

//-- OUTPUTS --
#ifdef RGB_OUTPUT_LM317
	timer_standby = 1000;
	ds1_number = 3;						//LM317
	ds2_number = 1;						//
	ds3_number = 7;						//
	while (timer_standby)
		UpdateDisplay();
#endif

#if ((defined RGB_OUTPUT_MOSFET_KIRNO) || (defined RGB_OUTPUT_CAT))
	timer_standby = 1000;
	ds1_number = DISPLAY_C;				//CAT o MOSFET KIRNO
	ds2_number = DISPLAY_A;				//
	ds3_number = DISPLAY_T;				//
	while (timer_standby)
		UpdateDisplay();
#endif

	//--- Main loop ---//
	while(1)
	{
		//PROGRAMA DE PRODUCCION

		//prueba DMX_ena() DMX_dis()
		/*
		if (!timer_standby)
		{
			timer_standby = 3000;
			if (swi)
			{
				swi = 0;
				DMX_Ena();
			}
			else
			{
				DMX_Disa();
				swi = 1;
			}
		}
		*/

		switch (main_state)
		{
			case MAIN_INIT:
				//cargo las variables desde la memoria
				p_mem_init = (unsigned int *) PAGE31;
#ifdef RGB_FOR_PROGRAMS
				param_struct.last_function_in_flash = p_mem_init->last_function_in_flash;
				param_struct.last_program_in_flash = p_mem_init->last_program_in_flash;
				param_struct.last_program_deep_in_flash = p_mem_init->last_program_deep_in_flash;
				param_struct.last_channel_in_flash = p_mem_init->last_channel_in_flash;
#endif

#ifdef RGB_FOR_CHANNELS
				param_struct.last_function_in_flash = p_mem_init->last_function_in_flash;
				param_struct.last_program_in_flash = p_mem_init->last_program_in_flash;
				param_struct.last_program_deep_in_flash = p_mem_init->last_program_deep_in_flash;
				param_struct.last_channel_in_flash = p_mem_init->last_channel_in_flash;

				param_struct.pwm_channel_1 = p_mem_init->pwm_channel_1;
				if (param_struct.pwm_channel_1 > 100)
					param_struct.pwm_channel_1 = 100;

				param_struct.pwm_channel_2 = p_mem_init->pwm_channel_2;
				if (param_struct.pwm_channel_2 > 100)
					param_struct.pwm_channel_2 = 100;

				param_struct.pwm_channel_3 = p_mem_init->pwm_channel_3;
				if (param_struct.pwm_channel_3 > 100)
					param_struct.pwm_channel_3 = 100;

				param_struct.pwm_channel_4 = p_mem_init->pwm_channel_4;
				if (param_struct.pwm_channel_4 > 100)
					param_struct.pwm_channel_4 = 100;

#endif

				//reviso donde estaba la ultima vez
				if (param_struct.last_function_in_flash == 0xFF)	//memoria borrada
				{
					last_function = FUNCTION_DMX;
					last_program = 1;
					last_program_deep = 1;
					last_channel = 1;

#ifdef RGB_FOR_CHANNELS
					fixed_data[0] = 0;
					fixed_data[1] = 0;
#endif

				}
				else
				{
#ifdef RGB_FOR_PROGRAMS
					last_function = param_struct.last_function_in_flash;
					last_program = param_struct.last_program_in_flash;
					last_channel = param_struct.last_channel_in_flash;
					last_program_deep = param_struct.last_program_deep_in_flash;
#endif

#ifdef RGB_FOR_CHANNELS
					last_function = param_struct.last_function_in_flash;
					last_program = param_struct.last_program_in_flash;
					last_channel = param_struct.last_channel_in_flash;
					last_program_deep = param_struct.last_program_deep_in_flash;

					fixed_data[0] = param_struct.pwm_channel_1;
					fixed_data[1] = param_struct.pwm_channel_2;
#endif
				}

				if (last_function == FUNCTION_DMX)
				{
					FromChannelToDs(last_channel);		//muestro el ultimo canal DMX seleccionado
					DMX_channel_selected = last_channel;
					ds4_number = DISPLAY_C;
					DMX_Ena();
					main_state = MAIN_DMX_NORMAL;
					timer_dmx_display_show = DMX_DISPLAY_SHOW_TIMEOUT;
				}
				else
				{
					DMX_Disa();
#ifdef RGB_FOR_PROGRAMS
					ds1_number = DISPLAY_PROG;
					ds2_number = last_program;
					ds3_number = last_program_deep;
					main_state = MAIN_MAN_PX_NORMAL;
#endif
#ifdef RGB_FOR_CHANNELS
					ds1_number = DISPLAY_C;
					ds2_number = DISPLAY_H;
					ds3_number = last_program;
					main_state = MAIN_MAN_PX_NORMAL;
#endif
				}
				break;

			case MAIN_DMX_CHECK_CHANNEL:
				//apago todos los canales antes de empezar
				RED_PWM (0);
				GREEN_PWM (0);
				BLUE_PWM (0);
				WHITE_PWM (0);
				UV_PWM (0);
		  	 	AMBER_PWM (0);

				FromChannelToDs(last_channel);		//muestro el ultimo canal DMX seleccionado
				ds4_number = DISPLAY_C;
				
				timer_standby = TIMER_STANDBY_TIMEOUT;
				main_state++;
				break;

			case MAIN_DMX_CHECK_CHANNEL_B:
				//espero que se libere el switch teniendo en cuenta que puedo venir de MANUAL
				if (CheckS1() == S_NO)
				{
					timer_standby = TIMER_STANDBY_TIMEOUT;
					main_state = MAIN_DMX_CHECK_CHANNEL_SELECTED;
				}
				break;

			case MAIN_DMX_CHECK_CHANNEL_SELECTED:
				if (CheckS1() > S_NO)
					main_state = MAIN_DMX_CHECK_CHANNEL_S1;

				if (CheckS2() > S_NO)
					main_state = MAIN_DMX_CHECK_CHANNEL_S2;

				if (!timer_standby)
					main_state = MAIN_DMX_SAVE_CONF;

				break;

			case MAIN_DMX_CHECK_CHANNEL_S1:
				//espero que se libere el switch o poner S_HALF y luego forzar 0
				if (CheckS1() == S_NO)
				{
					//corro una posicion o empiezo blinking
					//segun donde este puede ser que tenga que ir a programas si
					//se daja aretado mas tiempo
					if (display_blinking & 0x07)
					{
						//muevo el display en blinking
						if (display_blinking & DISPLAY_DS3)
							display_blinking = DISPLAY_DS1;
						else
							display_blinking <<= 1;
					}
					else	//no estaba en blinking, lo actico para DS1
					{
						display_blinking = DISPLAY_DS1;
					}
					main_state = MAIN_DMX_CHECK_CHANNEL_SELECTED;
				}

				//me fijo si tengo que cambiar a MANUAL
				if (CheckS1() >= S_HALF)
				{
					main_state = MAIN_MAN_PX_CHECK;
				}

				timer_standby = TIMER_STANDBY_TIMEOUT;
				break;

			case MAIN_DMX_CHECK_CHANNEL_S2:
				//espero que se libere el switch o poner S_HALF y luego forzar 0
				if (CheckS2() == S_NO)
				{
					//sino hay blinking no doy bola
					//segun donde esta blinking sumo 0-5 o 0-9
					if (display_blinking & 0x07)
					{
						switch (display_blinking)	//el canal dmx lo formo despues de guardar
						{
							case DISPLAY_DS1:	//desde 0 a 5
								if (ds1_number == DISPLAY_ZERO)
									ds1_number = 1;
								else if (ds1_number < 5)
									ds1_number++;
								else
									ds1_number = DISPLAY_ZERO;
								break;

							case DISPLAY_DS2:
								if (ds2_number == DISPLAY_ZERO)
									ds2_number = 1;
								else if (ds2_number < 9)
									ds2_number++;
								else
									ds2_number = DISPLAY_ZERO;
								break;

							case DISPLAY_DS3:
								if (ds3_number == DISPLAY_ZERO)
									ds3_number = 1;
								else if (ds3_number < 9)
									ds3_number++;
								else
									ds3_number = DISPLAY_ZERO;
								break;
						}
						last_channel = FromDsToChannel();
					}

					main_state = MAIN_DMX_CHECK_CHANNEL_SELECTED;
				}
				timer_standby = TIMER_STANDBY_TIMEOUT;
				break;

			case MAIN_DMX_SAVE_CONF:
				display_blinking = 0;
				//se cambio el canal, hago el update
				if ((last_channel == 0) || (last_channel > 511))		//no puede ser canal 0 ni mas de 511
					last_channel = 1;

				DMX_channel_selected = last_channel;
				DMX_Disa();
#ifdef RGB_FOR_PROGRAMS
				//hago update de memoria y grabo
				param_struct.last_channel_in_flash = last_channel;
				param_struct.last_function_in_flash = FUNCTION_DMX;
				param_struct.last_program_in_flash = last_program;
				param_struct.last_program_deep_in_flash = last_program_deep;
				WriteConfigurations ();
#endif

#ifdef RGB_FOR_CHANNELS
				//hago update de memoria y grabo
				param_struct.last_channel_in_flash = last_channel;
				param_struct.last_function_in_flash = FUNCTION_DMX;
				param_struct.last_program_in_flash = last_program;
				param_struct.last_program_deep_in_flash = last_program_deep;

				param_struct.pwm_channel_1 = fixed_data[0];
				param_struct.pwm_channel_2 = fixed_data[1];
				WriteConfigurations ();
#endif
				main_state++;
				timer_dmx_display_show = DMX_DISPLAY_SHOW_TIMEOUT;
				DMX_Ena();
				break;

			case MAIN_DMX_NORMAL:
				 if (Packet_Detected_Flag)
				 {
					 //llego un paquete DMX
					 Packet_Detected_Flag = 0;
					 //en data tengo la info
					 //FromChannelToDs(data[0]);	//no muestro el valor actual, solo el canal 1


					 /*
					 //filtro en tramas DMX
					 RED_PWM (MAFilter(data[0], vd0));	//RED
					 GREEN_PWM (MAFilter(data[1], vd1));	//GREEN
					 BLUE_PWM (MAFilter(data[2], vd2));	//BLUE
					 WHITE_PWM (MAFilter(data[3], vd3));	//WHITE
					 */

					 /*
					 RED_PWM (data[0]);	//RED	la salida ahora la hace el filtro
					 GREEN_PWM (data[1]);	//GREEN
					 BLUE_PWM (data[2]);	//BLUE
					 WHITE_PWM (data[3]);	//WHITE
					 */
				 }

#ifdef WITH_GRANDMASTER
				 if (!filter_timer)
				 {
					 filter_timer = 5;

					 acc = data[0] * data[6];
					 data[7] = acc >> 8;
					 acc = data[1] * data[6];
					 data[8] = acc >> 8;
					 acc = data[2] * data[6];
					 data[9] = acc >> 8;
					 acc = data[3] * data[6];
					 data[10] = acc >> 8;
					 acc = data[4] * data[6];
					 data[11] = acc >> 8;
					 acc = data[5] * data[6];
					 data[12] = acc >> 8;

					 RED_PWM (MAFilter(data[7], vd0));	//RED
					 GREEN_PWM (MAFilter(data[8], vd1));	//GREEN
					 BLUE_PWM (MAFilter(data[9], vd2));	//BLUE
					 WHITE_PWM (MAFilter(data[10], vd3));	//WHITE
					 UV_PWM (MAFilter(data[11], vd4));	//UV
					 AMBER_PWM (MAFilter(data[12], vd5));	//AMBER

				 }
#else
				 if (!filter_timer)
				 {
					 //filter_timer = 100;		//para prueba con placa mosfet comparad con controldor chino
					 filter_timer = 5;
					 //TODO: integrar a canales
					 RED_PWM (MAFilter(data[0], vd0));	//RED
					 GREEN_PWM (MAFilter(data[1], vd1));	//GREEN
					 BLUE_PWM (MAFilter(data[2], vd2));	//BLUE
					 WHITE_PWM (MAFilter(data[3], vd3));	//WHITE
					 UV_PWM (MAFilter(data[4], vd4));	//UV
					 AMBER_PWM (MAFilter(data[5], vd5));	//AMBER
				 }
#endif

				 if ((CheckS1() > S_NO) || (CheckS2() > S_NO))
				 {
					 //si se toco un boton una vez solo prendo display
					 if (!timer_dmx_display_show)
					 {
						 timer_dmx_display_show = DMX_DISPLAY_SHOW_TIMEOUT;
					 }
					 else if (timer_dmx_display_show < (DMX_DISPLAY_SHOW_TIMEOUT - 1000))	//1 seg despues
					 {
						 main_state = MAIN_DMX_CHECK_CHANNEL_SELECTED;
						 timer_standby = TIMER_STANDBY_TIMEOUT;
					 }
					 FromChannelToDs(last_channel);
					 ds4_number = DISPLAY_C;
				 }

				 if (!timer_dmx_display_show)
				 {
					 //debo apagar display
					 ds1_number = DISPLAY_NONE;
					 ds2_number = DISPLAY_NONE;
					 ds3_number = DISPLAY_NONE;
					 ds4_number = DISPLAY_NONE;
				 }
				 break;

			case MAIN_MAN_PX_CHECK:
#ifdef RGB_FOR_PROGRAMS
				//vengo de la otra funcion, reviso cual fue el ultimo programa
				DMX_Disa();
				ds1_number = DISPLAY_PROG;
				ds2_number = last_program;		//program no puede ser menor a 1
				ds3_number = last_program_deep;	//program_deep no puede ser menor a 1
				ds4_number = DISPLAY_NONE;

				display_blinking = DISPLAY_DS2;

				main_state++;
#endif
#ifdef RGB_FOR_CHANNELS
				//vengo de la otra funcion, reviso cual fue el ultimo canal usado
				DMX_Disa();
				LED_OFF;
				ds1_number = DISPLAY_C;
				ds2_number = DISPLAY_H;		//program no puede ser menor a 1
				ds3_number = last_program;	//program_deep no puede ser menor a 1
				ds4_number = DISPLAY_NONE;

				display_blinking = 0;
				need_to_save = 1;			//tengo que grabar, vengo de DMX
				main_state++;
				//limpio variables last y PWM
				Func_For_Cat(0, 0);
#endif
				break;

			case MAIN_MAN_PX_CHECK_B:
				//verifico que se libere el boton, porque puede ser que venga de la otra funcion
				if (CheckS1() == S_NO)
				{
					main_state = MAIN_MAN_PX_CHECK_DEEP;
					timer_standby = TIMER_STANDBY_TIMEOUT;
				}
				break;

			case MAIN_MAN_PX_CHECK_DEEP:
#ifdef RGB_FOR_PROGRAMS
				if (CheckS1() > S_NO)
					main_state = MAIN_MAN_PX_CHECK_S1;

				if (CheckS2() > S_NO)
					main_state = MAIN_MAN_PX_CHECK_S2;

				if (!timer_standby)
					main_state = MAIN_MAN_PX_SAVE_CONF;

				//si estan cambiando el programa igual muestro como va quedando
				Func_PX_Ds(ds1_number, ds2_number, ds3_number);
#endif

#ifdef RGB_FOR_CHANNELS
				if (CheckS1() > S_NO)
					main_state = MAIN_MAN_PX_CHECK_S1;

				if (CheckS2() > S_NO)
				{
					main_state = MAIN_MAN_PX_CHECK_S2;
					need_to_save = 1;
				}

				//salgo de dos maneras
				//grabando o no grabando
				if (!timer_standby)
				{
					if (need_to_save)
					{
						main_state = MAIN_MAN_PX_SAVE_CONF;
						need_to_save = 0;
					}
					else
						main_state = MAIN_MAN_PX_NORMAL;
				}


				//si estan cambiando el programa igual muestro como va quedando
				Func_For_Cat(fixed_data[0], fixed_data[1]);
#endif
				break;

			case MAIN_MAN_PX_CHECK_S1:
#ifdef RGB_FOR_PROGRAMS
				//espero que se libere el switch  o poner S_HALF y luego forzar 0
				if (CheckS1() == S_NO)
				{
					//corro una posicion o empiezo blinking
					//segun donde este puede ser que tenga que ir a programas
					if (display_blinking & 0x07)
					{
						//muevo el display en blinking
						if (display_blinking & DISPLAY_DS3)
						{
							display_blinking = DISPLAY_DS2;
						}
						else
							display_blinking <<= 1;
					}
					else	//no estaba en blinking, lo activo para DS1
					{
						display_blinking = DISPLAY_DS2;
					}
					main_state = MAIN_MAN_PX_CHECK_DEEP;
				}

				//me fijo si tengo que cambiar a DMX
				if (CheckS1() >= S_HALF)
				{
					//salto a dmx
					DMX_Ena();
					main_state = MAIN_DMX_CHECK_CHANNEL;
					display_blinking = DISPLAY_DS1;
				}

				timer_standby = TIMER_STANDBY_TIMEOUT;
#endif
#ifdef RGB_FOR_CHANNELS
				//espero que se libere el switch  o poner S_HALF y luego forzar 0
				if (CheckS1() == S_NO)
				{
					//TODO: integrar a canales
					if (last_program < RGB_FOR_CHANNELS_NUM)
						last_program++;
					else
						last_program = 1;

					ds1_number = DISPLAY_C;
					ds2_number = DISPLAY_H;
					ds3_number = last_program;
					main_state = MAIN_MAN_PX_CHECK_DEEP;
				}

				//me fijo si tengo que cambiar a DMX
				if (CheckS1() >= S_HALF)
				{
					//salto a dmx
					DMX_Ena();
					main_state = MAIN_DMX_CHECK_CHANNEL;
					display_blinking = DISPLAY_DS1;
				}

				timer_standby = TIMER_STANDBY_TIMEOUT_REDUCED;
				//timer_standby = TIMER_STANDBY_TIMEOUT;
#endif
				break;

			case MAIN_MAN_PX_CHECK_S2:
#ifdef RGB_FOR_PROGRAMS
				//espero que se libere el switch  o poner S_HALF y luego forzar 0
				if (CheckS2() == S_NO)
				{
					//sino hay blinking no doy bola
					//muevo DS3 desde 1 a 9 cambiando profundidad
					if (display_blinking & DISPLAY_DS3)
					{
						if (ds3_number < 9)
							ds3_number++;
						else
							ds3_number = 1;		//no puede ser 0

						last_program_deep = ds3_number;
					}

					//muevo DS2 desde 1 a 9 cambiando programas
					if (display_blinking & DISPLAY_DS2)
					{
						if (ds2_number < 9)
							ds2_number++;
						else
							ds2_number = 1;

						last_program = ds2_number;
					}
					main_state = MAIN_MAN_PX_CHECK_DEEP;
				}
				timer_standby = TIMER_STANDBY_TIMEOUT;
#endif
#ifdef RGB_FOR_CHANNELS
				switch (last_program)
				{
					case 1:
						//necesito un short para la cuenta
						if (!timer_for_cat_switch)
						{
							if (fixed_data[0] < 100)
								fixed_data[0]++;
							else
								fixed_data[0] = 0;

							FromChannelToDs(fixed_data[0]);
							timer_for_cat_switch = TIMER_FOR_CAT_SW;
						}
						break;

					case 2:
						//necesito un short para la cuenta
						if (!timer_for_cat_switch)
						{
							if (fixed_data[1] < 100)
								fixed_data[1]++;
							else
								fixed_data[1] = 0;

							FromChannelToDs(fixed_data[1]);
							timer_for_cat_switch = TIMER_FOR_CAT_SW;
						}
						break;

					default:
						last_program = 1;
						break;
				}
				main_state = MAIN_MAN_PX_CHECK_DEEP;

				timer_standby = TIMER_STANDBY_TIMEOUT;
#endif
				break;

			case MAIN_MAN_PX_SAVE_CONF:
				DMX_Disa();
				display_blinking = 0;
#ifdef RGB_FOR_PROGRAMS
				//hago update de memoria y grabo
				param_struct.last_channel_in_flash = last_channel;
				param_struct.last_function_in_flash = FUNCTION_MAN;
				param_struct.last_program_in_flash = last_program;
				param_struct.last_program_deep_in_flash = last_program_deep;
				WriteConfigurations ();
#endif

#ifdef RGB_FOR_CHANNELS
				param_struct.last_channel_in_flash = last_channel;
				param_struct.last_function_in_flash = FUNCTION_CAT;
				param_struct.last_program_in_flash = last_program;
				param_struct.last_program_deep_in_flash = last_program_deep;

				param_struct.pwm_channel_1 = fixed_data[0];
				param_struct.pwm_channel_2 = fixed_data[1];
				WriteConfigurations ();
#endif

				main_state++;
				break;

			case MAIN_MAN_PX_NORMAL:
#ifdef RGB_FOR_PROGRAMS
				Func_PX(last_program, last_program_deep);

				if ((CheckS1() > S_NO) || (CheckS2() > S_NO))
				{
					main_state = MAIN_MAN_PX_CHECK_DEEP;
					timer_standby = TIMER_STANDBY_TIMEOUT;
				}
#endif
#ifdef RGB_FOR_CHANNELS
				Func_For_Cat(fixed_data[0], fixed_data[1]);

				if ((CheckS1() > S_NO) || (CheckS2() > S_NO))
				{
					main_state = MAIN_MAN_PX_CHECK_DEEP;
					timer_standby = TIMER_STANDBY_TIMEOUT;
				}

				if (!timer_for_cat_display)
				{
					switch (show_channels_state)
					{
						case SHOW_CHANNELS:
							ds1_number = DISPLAY_C;
							ds2_number = DISPLAY_H;		//program no puede ser menor a 1
							ds3_number = last_program;	//program_deep no puede ser menor a 1
							show_channels_state = SHOW_NUMBERS;
							break;

						case SHOW_NUMBERS:
							switch (last_program)
							{
								case 1:
									FromChannelToDs(fixed_data[0]);
									break;

								case 2:
									FromChannelToDs(fixed_data[1]);
									break;

								default:
									last_program = 1;
									break;
							}
							show_channels_state = SHOW_CHANNELS;
							break;

						default:
							show_channels_state = SHOW_NUMBERS;
							break;
					}
					timer_for_cat_display = TTIMER_FOR_CAT_DISPLAY;
				}
#endif

				break;

			case MAIN_TEMP_OVERLOAD:
				//corto completo y dejo prendiendo y apagando el led cada 100ms
				//excepto FAN que lo pongo a maximo
				CTRL_FAN_ON;
				//DMX_Disa();	//por ahora no corto el DMX
				RED_PWM (0);
				GREEN_PWM (0);
				BLUE_PWM (0);
				WHITE_PWM (0);

				//con ds1_number, ds2_number y ds3_number tengo la info de display
				ds1_number = DISPLAY_NONE;
				ds2_number = DISPLAY_NONE;
				ds3_number = DISPLAY_E;

				main_state++;
				break;

			case MAIN_TEMP_OVERLOAD_B:
				if (!timer_standby)
				{
					timer_standby = 100;
					if (LED)
						LED_OFF;
					else
						LED_ON;
				}

				if (current_temp > TEMP_DISCONECT)
				{
					ds2_number = DISPLAY_E;
				}

				if (current_temp < TEMP_IN_50)
				{
					if (last_function == FUNCTION_DMX)
					{
						FromChannelToDs(last_channel);		//muestro el ultimo canal DMX seleccionado
						DMX_channel_selected = last_channel;
						main_state = MAIN_DMX_NORMAL;
						timer_dmx_display_show = DMX_DISPLAY_SHOW_TIMEOUT;
					}
					else
					{
#ifdef RGB_FOR_CHANNELS
						ds1_number = DISPLAY_C;
						ds2_number = DISPLAY_H;
						ds3_number = last_program;
						ResetLastValues();
#else
						ds1_number = DISPLAY_PROG;
						ds2_number = last_program;
						ds3_number = last_program_deep;
#endif
						main_state = MAIN_MAN_PX_NORMAL;
					}
				}
				break;

			default:
				main_state = MAIN_INIT;
				break;

		}

		UpdateDisplay ();
		UpdateSwitches ();

		//sensado de temperatura
		if (!take_sample)
		{
			take_sample = 10;	//tomo muestra cada 10ms
			current_temp = Get_Temp();

			if ((main_state != MAIN_TEMP_OVERLOAD) && (main_state != MAIN_TEMP_OVERLOAD_B))
			{
				if (current_temp > TEMP_IN_65)
				{
					//corto los leds	ver si habia DMX cortar y poner nuevamente
					main_state = MAIN_TEMP_OVERLOAD;
				}
				else if (current_temp > TEMP_IN_35)
					CTRL_FAN_ON;
				else if (current_temp < TEMP_IN_30)
					CTRL_FAN_OFF;
			}
		}
	}	//termina while(1)

	return 0;
}


//--- End of Main ---//
void Update_PWM (unsigned short pwm)
{
	Update_TIM3_CH1 (pwm);
	Update_TIM3_CH2 (4095 - pwm);
}


/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

unsigned short Get_Temp (void)
{
	unsigned short total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	vtemp[LARGO_FILTRO] = ReadADC1 (CH_IN_TEMP);
    for (j = 0; j < (LARGO_FILTRO); j++)
    {
    	total_ma += vtemp[j + 1];
    	vtemp[j] = vtemp[j + 1];
    }

    return total_ma >> DIVISOR;
}

unsigned char MAFilter (unsigned char new_sample, unsigned char * vsample)
{
	unsigned short total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	*(vsample + LARGO_F) = new_sample;

    for (j = 0; j < (LARGO_F); j++)
    {
    	total_ma += *(vsample + j + 1);
    	*(vsample + j) = *(vsample + j + 1);
    }

    return total_ma >> DIVISOR_F;
}

unsigned short MAFilter16 (unsigned char new_sample, unsigned char * vsample)
{
	unsigned short total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	*(vsample + LARGO_F) = new_sample;

    for (j = 0; j < (LARGO_F); j++)
    {
    	total_ma += *(vsample + j + 1);
    	*(vsample + j) = *(vsample + j + 1);
    }

    return total_ma >> DIVISOR_F;
}

void SendSegment (unsigned char display, unsigned char segment)
{
	unsigned char displacement = 0;

	OE_OFF;

#ifdef VER_1_0
	//PRUEBO desplazando 1 a la izq
	PWR_DS1_OFF;
	PWR_DS2_OFF;
	PWR_DS3_OFF;
	PWR_DS4_OFF;

//	dbkp = display;

    if (segment & 0x80)
    	displacement = 1;
   //  else
   //  	display &= 0xFE;

    Send_SPI_Single (displacement);
    segment <<= 1;
    Send_SPI_Single (segment);

	if (display == DISPLAY_DS1)
		PWR_DS1_ON;
	else if (display == DISPLAY_DS2)
		PWR_DS2_ON;
	else if (display == DISPLAY_DS3)
		PWR_DS3_ON;
	else if (display == DISPLAY_DS4)
		PWR_DS4_ON;

#endif

	OE_ON;
}

void ShowNumbersAgain (void)
{
	ShowNumbers (last_digit);
}


//carga los numeros a mostrar en secuencia en un vector
//del 1 al 9; 10 es cero; 11 es punto; 0, 12, 13, 14, 15 apagar
void VectorToDisplay (unsigned char new_number)
{
	unsigned char i;
	//me fijo si hay espacio
	if (p_numbers < &numbers[LAST_NUMBER])
	{
		//busco la primer posicion vacia y pongo el nuevo numero
		for (i = 0; i < LAST_NUMBER; i++)
		{
			if (numbers[i] == 0)
			{
				numbers[i] = new_number;
				i = LAST_NUMBER;
			}
		}
	}
}

unsigned short FromDsToChannel (void)	//en DS 10 es cero; OJO CON 11 es punto; 0, 12, 13, 14, 15 apagar
{
	unsigned short ch;

	if (ds1_number == DISPLAY_ZERO)
		ch = 0;
	else
		ch = ds1_number * 100;

	if (ds2_number != DISPLAY_ZERO)
		ch += ds2_number * 10;

	if (ds3_number != DISPLAY_ZERO)
		ch += ds3_number;

	return ch;
}

//DS1 centena
//DS2 decena
//DS3 unidades
void ShowNumbers (unsigned short number)
{
	unsigned char a, b;

	a = number / 100;
	ds1_number = a;
	if (ds1_number == 0)
		ds1_number = 10;

	b = (number - a * 100) / 10;
	ds2_number = b;
	if (ds2_number == 0)
		ds2_number = 10;

	ds3_number = number - a * 100 - b * 10;
	if (ds3_number == 0)
		ds3_number = 10;
}

#ifdef VER_1_0
//		dp g f e d c b a
//bits   7 6 5 4 3 2 1 0
//sin negar
unsigned char TranslateNumber (unsigned char number)	//del 1 al 9; 10 es cero; 11 es punto; 0, 12, 13, 14, 15 apagar
{
	//											     none,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
	const unsigned char vnumbers [25] = { 0x00, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x3F,
	//                                   point, line,  rem, prog,    E,    C,    H,   1P,    S,    A,    T,
													  0x80, 0x40, 0x63, 0x73, 0x79, 0x58, 0x74, 0x86, 0x6D, 0x5F, 0x78,
	//                                       R,    G,    N,
													  0x50, 0x6F, 0x54 };
	if (number < sizeof (vnumbers))
		number = vnumbers[number];
	else
		number = 0;

	return number;
}
#endif


void UpdateDisplay (void)
{
	unsigned char a;

	if (!display_timer)
	{
		switch (display_state)	//revisa a que display le toca
		{
			case 0:
				//primero reviso si tengo blink de digito
				if (display_blinking & DISPLAY_DS3)
				{
					//rutina blinking
					if (!display_blinking_timer)
					{
						if (display_was_on)
						{
							a = TranslateNumber(DISPLAY_NONE);
							SendSegment(DISPLAY_DS3, a);
							display_was_on = 0;
						}
						else
						{
							a = TranslateNumber(ds3_number);
							SendSegment(DISPLAY_DS3, a);
							display_was_on =1;
						}
						display_blinking_timer = BLINKING_UPDATE;
					}
					else
					{
						if (display_was_on)		//si mostraba el numero sigo asi
						{
							a = TranslateNumber(ds3_number);
							SendSegment(DISPLAY_DS3, a);
						}
						else
						{
							a = TranslateNumber(DISPLAY_NONE);
							SendSegment(DISPLAY_DS3, a);
						}
						display_blinking_timer--;
					}
				}
				else	//si no muestro como siempre
				{
					a = TranslateNumber(ds3_number);
					SendSegment(DISPLAY_DS3, a);
				}
				display_state++;
				break;

			case 1:
				//primero reviso si tengo blink de digito
				if (display_blinking & DISPLAY_DS2)
				{
					//rutina blinking
					if (!display_blinking_timer)
					{
						if (display_was_on)
						{
							a = TranslateNumber(DISPLAY_NONE);
							SendSegment(DISPLAY_DS2, a);
							display_was_on = 0;
						}
						else
						{
							a = TranslateNumber(ds2_number);
							SendSegment(DISPLAY_DS2, a);
							display_was_on =1;
						}
						display_blinking_timer = BLINKING_UPDATE;
					}
					else
					{
						if (display_was_on)		//si mostraba el numero sigo asi
						{
							a = TranslateNumber(ds2_number);
							SendSegment(DISPLAY_DS2, a);
						}
						else
						{
							a = TranslateNumber(DISPLAY_NONE);
							SendSegment(DISPLAY_DS2, a);
						}
						display_blinking_timer--;
					}
				}
				else	//si no muestro como siempre
				{
					a = TranslateNumber(ds2_number);
					SendSegment(DISPLAY_DS2, a);
				}
				display_state++;
				break;

			case 2:
				//primero reviso si tengo blink de digito
				if (display_blinking & DISPLAY_DS1)
				{
					//rutina blinking
					if (!display_blinking_timer)
					{
						if (display_was_on)
						{
							a = TranslateNumber(DISPLAY_NONE);
							SendSegment(DISPLAY_DS1, a);
							display_was_on = 0;
						}
						else
						{
							a = TranslateNumber(ds1_number);
							SendSegment(DISPLAY_DS1, a);
							display_was_on =1;
						}
						display_blinking_timer = BLINKING_UPDATE;
					}
					else
					{
						if (display_was_on)		//si mostraba el numero sigo asi
						{
							a = TranslateNumber(ds1_number);
							SendSegment(DISPLAY_DS1, a);
						}
						else
						{
							a = TranslateNumber(DISPLAY_NONE);
							SendSegment(DISPLAY_DS1, a);
						}
						display_blinking_timer--;
					}
				}
				else	//si no muestro como siempre
				{
					a = TranslateNumber(ds1_number);
					SendSegment(DISPLAY_DS1, a);
				}
				display_state++;
				break;

			case 3:
				//primero reviso si tengo blink de digito
				if (display_blinking & DISPLAY_DS4)
				{
					//rutina blinking
					if (!display_blinking_timer)
					{
						if (display_was_on)
						{
							a = TranslateNumber(DISPLAY_NONE);
							SendSegment(DISPLAY_DS4, a);
							display_was_on = 0;
						}
						else
						{
							a = TranslateNumber(ds4_number);
							SendSegment(DISPLAY_DS4, a);
							display_was_on =1;
						}
						display_blinking_timer = BLINKING_UPDATE;
					}
					else
					{
						if (display_was_on)		//si mostraba el numero sigo asi
						{
							a = TranslateNumber(ds4_number);
							SendSegment(DISPLAY_DS4, a);
						}
						else
						{
							a = TranslateNumber(DISPLAY_NONE);
							SendSegment(DISPLAY_DS4, a);
						}
						display_blinking_timer--;
					}
				}
				else	//si no muestro como siempre
				{
					a = TranslateNumber(ds4_number);
					SendSegment(DISPLAY_DS4, a);
				}
				display_state = 0;
				break;

			default:
				display_state = 0;
				break;
		}
		display_timer = DISPLAY_TIMER_RELOAD;
	}
}

void UpdateSwitches (void)
{
	//revisa los switches cada 10ms
	if (!switches_timer)
	{
		if (S1_PIN)
			s1++;
		else if (s1 > 50)
			s1 -= 50;
		else if (s1 > 10)
			s1 -= 5;
		else if (s1)
			s1--;

		if (S2_PIN)
			s2++;
		else if (s2 > 50)
			s2 -= 50;
		else if (s2 > 10)
			s2 -= 5;
		else if (s2)
			s2--;

		switches_timer = SWITCHES_TIMER_RELOAD;
	}
}

unsigned char CheckS1 (void)	//cada check tiene 10ms
{
	if (s1 > SWITCHES_THRESHOLD_FULL)
		return S_FULL;

	if (s1 > SWITCHES_THRESHOLD_HALF)
		return S_HALF;

	if (s1 > SWITCHES_THRESHOLD_MIN)
		return S_MIN;

	return S_NO;
}

unsigned char CheckS2 (void)
{
	if (s2 > SWITCHES_THRESHOLD_FULL)
		return S_FULL;

	if (s2 > SWITCHES_THRESHOLD_HALF)
		return S_HALF;

	if (s2 > SWITCHES_THRESHOLD_MIN)
		return S_MIN;

	return S_NO;
}


void DMX_Ena(void)
{
	//habilito la interrupcion
	EXTIOn ();
	USART1_RX_ENA;
}

void DMX_Disa(void)
{
	//deshabilito la interrupcion
	EXTIOff ();
	USART1_RX_DISA;
}

void EXTI4_15_IRQHandler(void)		//nueva detecta el primer 0 en usart Consola PHILIPS
{
	unsigned short aux;

	//if(EXTI->PR & 0x0100)	//Line8
	if(EXTI->PR & 0x8000)	//Line15
	{
		//si no esta con el USART detecta el flanco	PONER TIMEOUT ACA?????
		if ((dmx_receive_flag == 0) || (dmx_timeout_timer == 0))
		//if (dmx_receive_flag == 0)
		{
			switch (signal_state)
			{
				case IDLE:
					if (!(DMX_INPUT))
					{
						//Activo timer en Falling.
						TIM14->CNT = 0;
						TIM14->CR1 |= 0x0001;
						signal_state++;
					}
					break;

				case LOOK_FOR_BREAK:
					if (DMX_INPUT)
					{
						//Desactivo timer en Rising.
						aux = TIM14->CNT;

						//reviso BREAK
						//if (((tim_counter_65ms) || (aux > 88)) && (tim_counter_65ms <= 20))
						//if ((aux > 87) && (aux < 210))	//Consola STARLET 6
						//if ((aux > 87) && (aux < 2000))		//Consola marca CODE tiene break 1.88ms
						if ((aux > 87) && (aux < 4600))		//Consola marca CODE modelo A24 tiene break 4.48ms fecha 11-04-17
						{
							//LED_ON;		//TODO: apaga para pruebas
							signal_state++;
							DMX_channel_received = 0;
							dmx_timeout_timer = DMX_TIMEOUT;		//activo el timeout para esperar un MARK valido
						}
						else	//falso disparo
							signal_state = IDLE;
					}
					else	//falso disparo
						signal_state = IDLE;

					TIM14->CR1 &= 0xFFFE;
					break;

				case LOOK_FOR_MARK:
					if ((!(DMX_INPUT)) && (dmx_timeout_timer))	//termino Mark after break
					{
						//ya tenia el serie habilitado
						dmx_receive_flag = 1;
						LED_ON;
					}
					else	//falso disparo
					{
						//termine por timeout
						dmx_receive_flag = 0;
					}
					signal_state = IDLE;
					//LED_OFF;						//TODO: apaga para pruebas
					break;

				default:
					signal_state = IDLE;
					break;
			}
		}
		//EXTI->PR |= 0x0100;	//Line8
		EXTI->PR |= 0x8000;	//Line15
	}
}

void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{
		TimingDelay--;
	}

	if (wait_ms_var)
		wait_ms_var--;

	if (display_timer)
		display_timer--;

	if (timer_standby)
		timer_standby--;

	if (switches_timer)
		switches_timer--;

	if (dmx_timeout_timer)
		dmx_timeout_timer--;

	if (timer_dmx_display_show)
		timer_dmx_display_show--;

	if (prog_timer)
		prog_timer--;

	if (take_sample)
		take_sample--;

	if (filter_timer)
		filter_timer--;

	if (timer_for_cat_switch)
		timer_for_cat_switch--;

	if (timer_for_cat_display)
		timer_for_cat_display--;

	/*
	//cuenta 1 segundo
	if (button_timer_internal)
		button_timer_internal--;
	else
	{
		if (button_timer)
		{
			button_timer--;
			button_timer_internal = 1000;
		}
	}
	*/
}
