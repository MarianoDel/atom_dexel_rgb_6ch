/*
 * programs_functions.c
 *
 *  Created on: 17/05/2015
 *      Author: Mariano
 */

#include "programs_functions.h"
#include "hard.h"

//cual es el de mas ciclo de trabajo el 1 o el 9
#define ONE_FOR_NINE


/* Externals variables ---------------------------------------------------------*/
extern volatile unsigned short prog_timer;

/* Global variables ------------------------------------------------------------*/
unsigned char prog_state = 0;
unsigned char l_deep = 0;
unsigned short prog_fade = 0;
unsigned char cycles_strobe = 0;

//unsigned char last_ds1 = 0;
unsigned char last_ds2 = 0;
unsigned char last_ds3 = 0;

#ifdef RGB_FOR_CHANNELS
unsigned char last_r = 0;
unsigned char last_g = 0;
unsigned char last_b = 0;
unsigned char last_w = 0;
#endif

//posicion y porcentajes generales
//posicion							 0   1   2   3   4     5   6   7   8   9
//porcentajes						10  10  17.5 25  32.5  40 52.5 65  85  100
const unsigned char v_percent [] = {25, 25, 44, 64, 83, 102, 134, 166, 217, 255 };

//cambios de velocidad en ms
//posicion							0	 1    2    3    4     5     6     7     8     9
const unsigned short v_speed [] = { 50, 50, 100, 200, 400, 600, 1000, 1600, 2200, 3000 };

#ifdef ONE_FOR_NINE
//cambios de velocidad de fading en multiplos de ms
//posicion				  				  0   1   2   3  4  5  6  7  8  9
const unsigned char v_speed_fading[] = { 20, 20, 15, 10, 6, 5, 4, 3, 2, 1 };

//cambios de velocidad de strobe en multiplos de ms (buscar todos divisible por 2)
//posicion				  				    0    1    2    3    4    5    6    7    8    9
const unsigned short v_speed_strobe[] = { 700, 700, 600, 500, 400, 350, 300, 250, 200, 100 };
const unsigned char v_cycles_strobe[] = {  4,  4,   5,   6,   6,   7,   7,   8,   8,   9 };
#else
//cambios de velocidad de fading en multiplos de ms
//posicion				  				 0  1  2  3  4  5  6  7   8   9
const unsigned char v_speed_fading[] = { 1, 1, 2, 3, 4, 5, 6, 10, 15, 20 };

//cambios de velocidad de strobe en multiplos de ms (buscar todos divisible por 2)
//posicion				  				   0   1    2    3    4    5    6    7    8     9
const unsigned short v_speed_strobe[] = { 100, 100, 200, 250, 300, 350, 400, 500, 600, 700 };
const unsigned char v_cycles_strobe[] = {  9,  9,   8,   8,   7,   7,   6,   6,   5,    4 };
#endif
/* Functions -------------------------------------------------------------------*/

#ifdef RGB_FOR_CHANNELS
void Func_For_Cat(unsigned char r, unsigned char g)
{
	unsigned short acc;

	if (last_r != r)
	{
		acc = r * 255;
		acc = acc / 100;
#ifdef RGB_FOR_CHANNELS_CH1_CH3_SYNC
		RED_PWM(acc);
		BLUE_PWM(acc);
#else
		RED_PWM(acc);
#endif
		last_r = r;
	}

	if (last_g != g)
	{
		acc = g * 255;
		acc = acc / 100;
#ifdef RGB_FOR_CHANNELS_CH2_CH4_SYNC
		GREEN_PWM(acc);
		WHITE_PWM(acc);
#else
		GREEN_PWM(acc);
#endif


		last_g = g;
	}
}

void ResetLastValues(void)
{
	last_r = 0;
	last_g = 0;
	last_b = 0;
	last_w = 0;
}

/*
void Func_For_Cat(unsigned char r, unsigned char g, unsigned char b, unsigned char w)
{
	unsigned short acc;

	if (last_r != r)
	{
		acc = r * 255;
		acc = acc / 100;
		RED_PWM(acc);
		last_r = r;
	}

	if (last_g != g)
	{
		acc = g * 255;
		acc = acc / 100;
		GREEN_PWM(acc);
		last_g = g;
	}

	if (last_b != b)
	{
		acc = b * 255;
		acc = acc / 100;
		BLUE_PWM(acc);
		last_b = b;
	}

	if (last_w != w)
	{
		acc = w * 255;
		acc = acc / 100;
		WHITE_PWM(acc);
		last_w = w;
	}
}
*/
#endif

//funcion general la llamo para determinar las funciones individuales, la llamo con los 3 DS
void Func_PX_Ds(unsigned char ds1, unsigned char ds2, unsigned char ds3)
{
	//ds1 lo uso como chequeo
	if (ds1 != DISPLAY_PROG)
		return;

	if ((last_ds2 != ds2) || (last_ds3 != ds3))
	{
		//algo cambio, fuerzo el update
		last_ds2 = ds2;
		last_ds3 = ds3;

		l_deep = 0;		//fuerza update
	}

	switch (ds2)	//tiene el programa de 1 a 9
	{
		case 1:
			Func_P1(ds3);
			break;

		case 2:
			Func_P2(ds3);
			break;

		case 3:
			Func_P3(ds3);
			break;

		case 4:
			Func_P4(ds3);
			break;

		case 5:
			Func_P5(ds3);
			break;

		case 6:
			Func_P6(ds3);
			break;

		case 7:
			Func_P7(ds3);
			break;

		case 8:
			Func_P8(ds3);
			break;

		case 9:
			Func_P9(ds3);
			break;

		default:
			break;
	}

}

//funcion general la llamo para determinar las funciones individuales, la llamo con programa y profundidad
//es un wrapper de Func_PX_Ds()
//
void Func_PX(unsigned char prog, unsigned char deep)
{
	unsigned char ds1, ds2, ds3;

	ds1 = DISPLAY_PROG;
	ds2 = prog;
	ds3 = deep;

	Func_PX_Ds(ds1, ds2, ds3);
}

//solo ejecuta el programa especifico, los switches y otras cosas se resuelven fuera
void Func_P1(unsigned char deep)	//static RED
{
	if (deep > 9)	//chequeo errores
		return;

	if (l_deep != deep)
	{
		l_deep = deep;
		RED_PWM(v_percent[deep]);
		GREEN_PWM(0);
		BLUE_PWM(0);
		WHITE_PWM(0);
	}
}

void Func_P2(unsigned char deep)	//static GREEN
{
	if (deep > 9)	//chequeo errores
		return;

	if (l_deep != deep)
	{
		l_deep = deep;
		RED_PWM(0);
		GREEN_PWM(v_percent[deep]);
		BLUE_PWM(0);
		WHITE_PWM(0);
	}
}

void Func_P3(unsigned char deep)	//static BLUE
{
	if (deep > 9)	//chequeo errores
		return;

	if (l_deep != deep)
	{
		l_deep = deep;
		RED_PWM(0);
		GREEN_PWM(0);
		BLUE_PWM(v_percent[deep]);
		WHITE_PWM(0);
	}
}

void Func_P4(unsigned char deep)	//static PURPLE
{
	if (deep > 9)	//chequeo errores
		return;

	if (l_deep != deep)
	{
		l_deep = deep;
		RED_PWM(v_percent[deep]);
		GREEN_PWM(0);
		BLUE_PWM(v_percent[deep]);
		WHITE_PWM(0);
	}
}

void Func_P5(unsigned char deep)	//static YELLOW
{
	if (deep > 9)	//chequeo errores
		return;

	if (l_deep != deep)
	{
		l_deep = deep;
		RED_PWM(v_percent[deep]);
		GREEN_PWM(v_percent[deep]);
		BLUE_PWM(0);
		WHITE_PWM(0);
	}
}

void Func_P6(unsigned char deep)	//static CYAN
{
	if (deep > 9)	//chequeo errores
		return;

	if (l_deep != deep)
	{
		l_deep = deep;
		RED_PWM(0);
		GREEN_PWM(v_percent[deep]);
		BLUE_PWM(v_percent[deep]);
		WHITE_PWM(0);
	}
}

void Func_P7(unsigned char deep)	//hago un strobe del 50% de toda la paleta de colores
{

	if (deep > 9)	//chequeo errores
		return;

	if (l_deep != deep)		//fuerzo el update
	{
		l_deep = deep;
		prog_state = P7_COLOR1_ON;
		prog_timer = 0;
		cycles_strobe = v_cycles_strobe[deep];
	}

	switch (prog_state)
	{
		case P7_COLOR1_ON:		//RED
			if (!prog_timer)
			{
				RED_PWM(255);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR1_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				if ((cycles_strobe - 1) > 0)
				{
					cycles_strobe--;
					prog_state--;
				}
				else
				{
					cycles_strobe = v_cycles_strobe[deep];
					prog_state++;
				}
			}
			break;

		case P7_COLOR2_ON:		//GREEN
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(255);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR2_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				if ((cycles_strobe - 1) > 0)
				{
					cycles_strobe--;
					prog_state--;
				}
				else
				{
					cycles_strobe = v_cycles_strobe[deep];
					prog_state++;
				}
			}
			break;

		case P7_COLOR3_ON:		//BLUE
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(255);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR3_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				if ((cycles_strobe - 1) > 0)
				{
					cycles_strobe--;
					prog_state--;
				}
				else
				{
					cycles_strobe = v_cycles_strobe[deep];
					prog_state++;
				}

			}
			break;

		case P7_COLOR4_ON:		//PURPLE
			if (!prog_timer)
			{
				RED_PWM(255);
				GREEN_PWM(0);
				BLUE_PWM(255);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR4_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				if ((cycles_strobe - 1) > 0)
				{
					cycles_strobe--;
					prog_state--;
				}
				else
				{
					cycles_strobe = v_cycles_strobe[deep];
					prog_state++;
				}

			}
			break;

		case P7_COLOR5_ON:		//YELLOW
			if (!prog_timer)
			{
				RED_PWM(255);
				GREEN_PWM(255);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR5_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				if ((cycles_strobe - 1) > 0)
				{
					cycles_strobe--;
					prog_state--;
				}
				else
				{
					cycles_strobe = v_cycles_strobe[deep];
					prog_state++;
				}

			}
			break;

		case P7_COLOR6_ON:		//CYAN
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(255);
				BLUE_PWM(255);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR6_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				if ((cycles_strobe - 1) > 0)
				{
					cycles_strobe--;
					prog_state--;
				}
				else
				{
					cycles_strobe = v_cycles_strobe[deep];
					prog_state++;
				}

			}
			break;

		case P7_COLOR7_ON:		//WHITE
			if (!prog_timer)
			{
#ifdef WHITE_AS_IN_RGB
				RED_PWM(255);
				GREEN_PWM(255);
				BLUE_PWM(255);
				WHITE_PWM(0);
#endif
#ifdef WHITE_AS_WHITE
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM255);
#endif
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR7_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				if ((cycles_strobe - 1) > 0)
				{
					cycles_strobe--;
					prog_state--;
				}
				else
				{
					cycles_strobe = v_cycles_strobe[deep];
					prog_state = P7_COLOR1_ON;
				}
			}
			break;

		default:
			prog_state = P7_COLOR1_ON;
			break;
	}

}

/*
void Func_P7(unsigned char deep)	//hago un strobe del 50% de toda la paleta de colores
{

	if (deep > 9)	//chequeo errores
		return;

	if (l_deep != deep)		//fuerzo el update
	{
		l_deep = deep;
		prog_state = P7_COLOR1_ON;
		prog_timer = 0;
		cycles_strobe = v_cycles_strobe[deep];
	}

	switch (prog_state)
	{
		case P7_COLOR1_ON:		//RED
			if (!prog_timer)
			{
				RED_PWM(255);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR1_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR2_ON:		//GREEN
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(255);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR2_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR3_ON:		//BLUE
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(255);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR3_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR4_ON:		//PURPLE
			if (!prog_timer)
			{
				RED_PWM(255);
				GREEN_PWM(0);
				BLUE_PWM(255);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR4_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR5_ON:		//YELLOW
			if (!prog_timer)
			{
				RED_PWM(255);
				GREEN_PWM(255);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR5_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR6_ON:		//CYAN
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(255);
				BLUE_PWM(255);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR6_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR7_ON:		//WHITE
			if (!prog_timer)
			{
#ifdef WHITE_AS_IN_RGB
				RED_PWM(255);
				GREEN_PWM(255);
				BLUE_PWM(255);
				WHITE_PWM(0);
#endif
#ifdef WHITE_AS_WHITE
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM255);
#endif
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state++;
			}
			break;

		case P7_COLOR7_OFF:
			if (!prog_timer)
			{
				RED_PWM(0);
				GREEN_PWM(0);
				BLUE_PWM(0);
				WHITE_PWM(0);
				prog_timer = (v_speed_strobe[deep] >> 1);	//velocidad / 2
				prog_state = P7_COLOR1_ON;
			}
			break;

		default:
			prog_state = P7_COLOR1_ON;
			break;
	}

}
*/
void Func_P8(unsigned char deep)	//single color fading
{
	if (deep > 9)	//chequeo errores
		return;

	if (l_deep != deep)		//fuerzo el update
	{
		l_deep = deep;
		prog_state = P8_INCREASE_COLOR1;
		prog_timer = 0;
	}

	switch (prog_state)
	{
		case P8_INCREASE_COLOR1:	//RED
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					RED_PWM(prog_fade);
					GREEN_PWM(0);
					BLUE_PWM(0);
					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_DECREASE_COLOR1:
			if (!prog_timer)
			{
				if (prog_fade)
				{
					RED_PWM(prog_fade);
					GREEN_PWM(0);
					BLUE_PWM(0);
					WHITE_PWM(0);
					prog_fade--;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_INCREASE_COLOR2:	//GREEN
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					RED_PWM(0);
					GREEN_PWM(prog_fade);
					BLUE_PWM(0);
					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_DECREASE_COLOR2:
			if (!prog_timer)
			{
				if (prog_fade)
				{
					RED_PWM(0);
					GREEN_PWM(prog_fade);
					BLUE_PWM(0);
					WHITE_PWM(0);
					prog_fade--;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_INCREASE_COLOR3:	//BLUE
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					RED_PWM(0);
					GREEN_PWM(0);
					BLUE_PWM(prog_fade);
					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_DECREASE_COLOR3:
			if (!prog_timer)
			{
				if (prog_fade)
				{
					RED_PWM(0);
					GREEN_PWM(0);
					BLUE_PWM(prog_fade);
					WHITE_PWM(0);
					prog_fade--;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_INCREASE_COLOR4:	//PURPLE
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					RED_PWM(prog_fade);
					GREEN_PWM(0);
					BLUE_PWM(prog_fade);
					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_DECREASE_COLOR4:
			if (!prog_timer)
			{
				if (prog_fade)
				{
					RED_PWM(prog_fade);
					GREEN_PWM(0);
					BLUE_PWM(prog_fade);
					WHITE_PWM(0);
					prog_fade--;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_INCREASE_COLOR5:	//YELLOW
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					RED_PWM(prog_fade);
					GREEN_PWM(prog_fade);
					BLUE_PWM(0);
					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_DECREASE_COLOR5:
			if (!prog_timer)
			{
				if (prog_fade)
				{
					RED_PWM(prog_fade);
					GREEN_PWM(prog_fade);
					BLUE_PWM(0);
					WHITE_PWM(0);
					prog_fade--;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_INCREASE_COLOR6:	//CYAN
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					RED_PWM(0);
					GREEN_PWM(prog_fade);
					BLUE_PWM(prog_fade);
					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_DECREASE_COLOR6:
			if (!prog_timer)
			{
				if (prog_fade)
				{
					RED_PWM(0);
					GREEN_PWM(prog_fade);
					BLUE_PWM(prog_fade);
					WHITE_PWM(0);
					prog_fade--;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_INCREASE_COLOR7:	//WHITE
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
#ifdef WHITE_AS_IN_RGB
					RED_PWM(prog_fade);
					GREEN_PWM(prog_fade);
					BLUE_PWM(prog_fade);
					WHITE_PWM(0);
#endif
#ifdef WHITE_AS_WHITE
					RED_PWM(0);
					GREEN_PWM(0);
					BLUE_PWM(0);
					WHITE_PWM(prog_fade);
#endif
					prog_fade++;
				}
				else
				{
					prog_state++;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P8_DECREASE_COLOR7:
			if (!prog_timer)
			{
				if (prog_fade)
				{
#ifdef WHITE_AS_IN_RGB
					RED_PWM(prog_fade);
					GREEN_PWM(prog_fade);
					BLUE_PWM(prog_fade);
					WHITE_PWM(0);
#endif
#ifdef WHITE_AS_WHITE
					RED_PWM(0);
					GREEN_PWM(0);
					BLUE_PWM(0);
					WHITE_PWM(prog_fade);
#endif
					prog_fade--;
				}
				else
				{
					prog_state = P8_INCREASE_COLOR1;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		default:
			prog_state = P8_INCREASE_COLOR1;
			break;
	}
}

void Func_P9(unsigned char deep)	//multicolor fading
{
	if (deep > 9)	//chequeo errores
		return;

	if (l_deep != deep)		//fuerzo el update
	{
		l_deep = deep;
		prog_state = P9_FIRST_COLOR7;

		//seteo en 0
		prog_timer = 0;
		prog_fade = 0;
		RED_PWM(0);
		GREEN_PWM(0);
		BLUE_PWM(0);
		WHITE_PWM(0);
	}

	switch (prog_state)
	{
		case P9_FIRST_COLOR7:
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
#ifdef WHITE_AS_IN_RGB
					RED_PWM(prog_fade);
					GREEN_PWM(prog_fade);
					BLUE_PWM(prog_fade);
					WHITE_PWM(0);
#endif

#ifdef WHITE_AS_WHITE
					RED_PWM(0);
					GREEN_PWM(0);
					BLUE_PWM(0);
					WHITE_PWM(prog_fade);
#endif
					prog_fade++;
				}
				else
				{
					prog_state++;
					prog_fade = 0;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P9_INCREASE_COLOR1_DECREASE_COLOR7:
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
#ifdef WHITE_AS_IN_RGB
					//R se comparte
					if (prog_fade > (MAX_FADE - prog_fade))	//crece R
						RED_PWM(prog_fade);
					else
						RED_PWM(MAX_FADE - prog_fade);
					GREEN_PWM(MAX_FADE - prog_fade);	//decrece G
					BLUE_PWM(MAX_FADE - prog_fade);		//decrece B
					WHITE_PWM(0);
#endif

#ifdef WHITE_AS_WHITE
					RED_PWM(prog_fade);		//crece R
					GREEN_PWM(0);
					BLUE_PWM(0);
					WHITE_PWM(MAX_FADE - prog_fade);	//decrece W
#endif

					prog_fade++;
				}
				else
				{
					prog_state++;
					prog_fade = 0;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P9_INCREASE_COLOR2_DECREASE_COLOR1:
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					RED_PWM(MAX_FADE - prog_fade);	//decrece R
					GREEN_PWM(prog_fade);			//crece G
					BLUE_PWM(0);
					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
					prog_fade = 0;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P9_INCREASE_COLOR3_DECREASE_COLOR2:
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					RED_PWM(0);
					GREEN_PWM(MAX_FADE - prog_fade);	//decrece G
					BLUE_PWM(prog_fade);				//crece B
					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
					prog_fade = 0;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P9_INCREASE_COLOR4_DECREASE_COLOR3:
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					RED_PWM(prog_fade);						//crece RB
					GREEN_PWM(0);
					//B se comparte
					if (prog_fade > (MAX_FADE - prog_fade))	//crece RB
						BLUE_PWM(prog_fade);
					else
						BLUE_PWM(MAX_FADE - prog_fade);

					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
					prog_fade = 0;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P9_INCREASE_COLOR5_DECREASE_COLOR4:
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					//R se comparte
					if (prog_fade > (MAX_FADE - prog_fade))	//crece R
						RED_PWM(prog_fade);
					else
						RED_PWM(MAX_FADE - prog_fade);

					GREEN_PWM(prog_fade);							//crece G
					BLUE_PWM(MAX_FADE - prog_fade);			//decrece B
					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
					prog_fade = 0;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P9_INCREASE_COLOR6_DECREASE_COLOR5:
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
					RED_PWM(MAX_FADE - prog_fade);			//decrece R
					//G se comparte
					if (prog_fade > (MAX_FADE - prog_fade))	//crece G
						GREEN_PWM(prog_fade);
					else
						GREEN_PWM(MAX_FADE - prog_fade);

					BLUE_PWM(prog_fade);
					WHITE_PWM(0);
					prog_fade++;
				}
				else
				{
					prog_state++;
					prog_fade = 0;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		case P9_INCREASE_COLOR7_DECREASE_COLOR6:
			if (!prog_timer)
			{
				if (prog_fade < MAX_FADE)
				{
#ifdef WHITE_AS_IN_RGB
					RED_PWM(prog_fade);			//crece R

					//G se comparte
					if (prog_fade > (MAX_FADE - prog_fade))	//crece G
						GREEN_PWM(prog_fade);
					else
						GREEN_PWM(MAX_FADE - prog_fade);

					//B se comparte
					if (prog_fade > (MAX_FADE - prog_fade))	//crece B
						BLUE_PWM(prog_fade);
					else
						BLUE_PWM(MAX_FADE - prog_fade);

					WHITE_PWM(0);
#endif

#ifdef WHITE_AS_WHITE
					RED_PWM(0);
					GREEN_PWM(MAX_FADE - prog_fade);	//decrece G
					BLUE_PWM(MAX_FADE - prog_fade);		//decrece B
					WHITE_PWM(prog_fade);
#endif

					prog_fade++;
				}
				else
				{
					prog_state = P9_INCREASE_COLOR1_DECREASE_COLOR7;
					prog_fade = 0;
				}
				prog_timer = v_speed_fading[deep];
			}
			break;

		default:
			prog_state = P9_FIRST_COLOR7;
			//seteo en 0
			prog_timer = 0;
			prog_fade = 0;
			RED_PWM(0);
			GREEN_PWM(0);
			BLUE_PWM(0);
			WHITE_PWM(0);
			break;
	}
}
