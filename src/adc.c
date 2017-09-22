/*
 * adc.c
 *
 *  Created on: 04/05/2015
 *      Author: Mariano
 */
#include "adc.h"
#include "stm32f0xx.h"
#include "hard.h"


//--- VARIABLES EXTERNAS ---//
extern volatile unsigned short adc_ch [];

#ifdef ADC_WITH_INT
extern volatile unsigned char seq_ready;
#endif

//--- VARIABLES GLOBALES ---//
//volatile unsigned char channel_conv = 0;

//Single conversion mode (CONT=0)
//In Single conversion mode, the ADC performs a single sequence of conversions,
//converting all the channels once.

//Continuous conversion mode (CONT=1)
//In continuous conversion mode, when a software or hardware trigger event occurs,
//the ADC performs a sequence of conversions, converting all the channels once and then
//automatically re-starts and continuously performs the same sequence of conversions

//Discontinuous mode (DISCEN)
//In this mode (DISCEN=1), a hardware or software trigger event is required to start
//each conversion defined in the sequence. Only with (CONT=0)

void AdcConfig (void)
{
#ifdef ADC_WITH_INT
	NVIC_InitTypeDef    NVIC_InitStructure;
#endif

	if (!RCC_ADC_CLK)
		RCC_ADC_CLK_ON;

	// preseteo los registros a default, la mayoria necesita tener ADC apagado
	ADC1->CR = 0x00000000;
	ADC1->IER = 0x00000000;
	ADC1->CFGR1 = 0x00000000;
	ADC1->CFGR2 = 0x00000000;
	ADC1->SMPR = 0x00000000;
	ADC1->TR = 0x0FFF0000;
	ADC1->CHSELR = 0x00000000;

	//set clock
	ADC1->CFGR2 = ADC_ClockMode_SynClkDiv4;

	//set resolution, trigger & Continuos or Discontinuous
	//ADC1->CFGR1 |= ADC_Resolution_10b | ADC_ExternalTrigConvEdge_Rising | ADC_ExternalTrigConv_T3_TRGO;
	//ADC1->CFGR1 |= ADC_Resolution_12b | ADC_ExternalTrigConvEdge_Rising | ADC_ExternalTrigConv_T1_TRGO;
	//ADC1->CFGR1 |= ADC_Resolution_12b | ADC_CFGR1_DISCEN;
	ADC1->CFGR1 |= ADC_Resolution_12b;

	//DMA Config
	//ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;

	//set sampling time
	//ADC1->SMPR |= ADC_SampleTime_41_5Cycles;		//17.39 son SP 420
	//ADC1->SMPR |= ADC_SampleTime_28_5Cycles;		//17.39 son SP 420
	//ADC1->SMPR |= ADC_SampleTime_7_5Cycles;		//17.36 de salida son SP 420 pero a veces pega
													//las dos int (usar DMA?) y pierde el valor intermedio
	//ADC1->SMPR |= ADC_SampleTime_1_5Cycles;			//20.7 de salida son SP 420 (regula mal)
	ADC1->SMPR |= ADC_SampleTime_239_5Cycles;

	//set channel selection
	//ADC1->CHSELR |= ADC_Channel_0 | ADC_Channel_1 | ADC_Channel_2 | ADC_Channel_3 | ADC_Channel_4;
	//ADC1->CHSELR |= ADC_Channel_0 | ADC_Channel_1 | ADC_Channel_2;
	//ADC1->CHSELR |= ADC_Channel_0 | ADC_Channel_1;
	//ADC1->CHSELR |= ADC_Channel_2;	//individuales andan todos

#ifdef ADC_WITH_INT
	//set interrupts
	ADC1->IER |= ADC_IT_EOC;

	/* Configure and enable ADC1 interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

	//calibrar ADC
	ADCGetCalibrationFactor();

	// Enable ADC1
	ADC1->CR |= ADC_CR_ADEN;
}

#ifdef ADC_WITH_INT
void ADC1_COMP_IRQHandler (void)
{
	/*
	if (ADC1->ISR & ADC_IT_EOC)
	{
		LED_ON;
		//clear pending
		ADC1->ISR |= ADC_IT_EOC | ADC_IT_EOSEQ;
		LED_OFF;
	}
	*/


	if (ADC1->ISR & ADC_IT_EOC)
	{
		//LED_ON;
		if (ADC1->ISR & ADC_IT_EOSEQ)	//seguro que es channel2
		{
			adc_ch2 = ADC1->DR;
			seq_ready = 1;
		}
		else
		{
//			LED_ON;
			adc_ch1 = ADC1->DR;
		}

		//clear pending
		ADC1->ISR |= ADC_IT_EOC | ADC_IT_EOSEQ;

		//LED_OFF;
	}
}
#endif

/*

//USA STD LIB DE ST
unsigned short ReadADC1 (unsigned int channel)
{
	uint32_t tmpreg = 0;
	//GPIOA_PIN4_ON;
	// Set channel and sample time
	//ADC_ChannelConfig(ADC1, channel, ADC_SampleTime_7_5Cycles);	//pifia la medicion 2800 o 3400 en ves de 4095
	//ADC_ChannelConfig(ADC1, channel, ADC_SampleTime_239_5Cycles);
	//ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_239_5Cycles);

	//ADC_ChannelConfig INTERNALS
	// Configure the ADC Channel
	ADC1->CHSELR = channel;

	// Clear the Sampling time Selection bits
	tmpreg &= ~ADC_SMPR1_SMPR;

	// Set the ADC Sampling Time register
	tmpreg |= (uint32_t)ADC_SampleTime_239_5Cycles;

	// Configure the ADC Sample time register
	ADC1->SMPR = tmpreg ;


	// Start the conversion
	ADC_StartOfConversion(ADC1);
	// Wait until conversion completion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	// Get the conversion value
	//GPIOA_PIN4_OFF;	//tarda 20us en convertir
	return ADC_GetConversionValue(ADC1);
}
*/

//Setea el sample time en el ADC
void SetADC1_SampleTime (void)
{
	uint32_t tmpreg = 0;

	/* Clear the Sampling time Selection bits */
	tmpreg &= ~ADC_SMPR1_SMPR;

	/* Set the ADC Sampling Time register */
	tmpreg |= (uint32_t)ADC_SampleTime_239_5Cycles;

	/* Configure the ADC Sample time register */
	ADC1->SMPR = tmpreg ;
}


//lee el ADC sin cambiar el sample time anterior
unsigned short ReadADC1 (unsigned int channel)
{
	// Configure the ADC Channel
	ADC1->CHSELR = channel;

	// Start the conversion
	ADC1->CR |= (uint32_t)ADC_CR_ADSTART;

	// Wait until conversion completion
	while((ADC1->ISR & ADC_ISR_EOC) == 0);

	// Get the conversion value
	return (uint16_t) ADC1->DR;
}

unsigned short ReadADC1Check (unsigned char channel)
{
	if (ADC1->CR & 0x01)			//reviso ADEN
		return 0xFFFF;

	//espero que este listo para convertir
	while ((ADC1->ISR & 0x01) == 0);	//espero ARDY = 1

	if ((ADC1->CFGR1 & 0x00010000) == 0)			//reviso DISCONTINUOS = 1
		return 0xFFFF;

	if (ADC1->CFGR1 & 0x00002000)					//reviso CONT = 0
		return 0xFFFF;

	if (ADC1->CFGR1 & 0x00000C00)					//reviso TRIGGER = 00
		return 0xFFFF;

	if (ADC1->CFGR1 & 0x00000020)					//reviso ALIGN = 0
		return 0xFFFF;

	if (ADC1->CFGR1 & 0x00000018)					//reviso RES = 00
		return 0xFFFF;

	//espero que no se este convirtiendo ADCSTART = 0
	while ((ADC1->CR & 0x02) != 0);	//espero ADCSTART = 0

	ADC1->CHSELR = 0x00000001;	//solo convierto CH0

	return 1;
}

unsigned int ADCGetCalibrationFactor (void)
{
  uint32_t tmpreg = 0, calibrationcounter = 0, calibrationstatus = 0;

  /* Set the ADC calibartion */
  ADC1->CR |= (uint32_t)ADC_CR_ADCAL;

  /* Wait until no ADC calibration is completed */
  do
  {
    calibrationstatus = ADC1->CR & ADC_CR_ADCAL;
    calibrationcounter++;
  } while((calibrationcounter != CALIBRATION_TIMEOUT) && (calibrationstatus != 0x00));

  if((uint32_t)(ADC1->CR & ADC_CR_ADCAL) == RESET)
  {
    /*Get the calibration factor from the ADC data register */
    tmpreg = ADC1->DR;
  }
  else
  {
    /* Error factor */
    tmpreg = 0x00000000;
  }
  return tmpreg;
}
