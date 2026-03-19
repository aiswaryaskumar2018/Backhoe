/*
 * adc.c
 *
 *  Created on: Sep 18, 2024
 *      Author: aiswarya.kumar
 */

#include "adc.h"
#include "CurveHeader.h"




uint16_t AdcValueTransOilTemp = 0;
uint16_t ADC_value;
uint16_t adc_buf[8];
uint32_t ADC_TimeStamp = 0;
uint32_t data = 0;
uint32_t AdcDGetAdcValue[eMuxAnalogInputMaximum] = {0,0,0,0,0,0};


void AdcDataProcessing(void)
{
	static uint32_t	TimeStamp  = 0u;
	static uint8_t Status = 0u;
	static uint8_t FirstEntry = 1u;

	if(FirstEntry)
	{
		TimeStamp = u32GetClockTicks();
		FirstEntry = 0;
	}

	switch (Status)
	{
		case eMux_AirPressure_AnalogInput:
			HAL_GPIO_WritePin(S0_GPIO_Port, S0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, Z0_AnalogMuxWritePIN_Pin, GPIO_PIN_SET);

			break;

		case eMux_FuelGauge_AnalogInput:
			HAL_GPIO_WritePin(S0_GPIO_Port, S0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, Z0_AnalogMuxWritePIN_Pin, GPIO_PIN_SET);
			break;

		default:
			/* NA */
		break;

	}

	if( u32GetClockTicks() - TimeStamp >= 100u)
	{
		AdcDGetAdcValue[Status] = GetAdcConversionRank1_Ch16();
		Status++;
		if(Status == eMuxAnalogInputMaximum)
		{
			AdcValueTransOilTemp = GetAdcConversionRank2_Ch15();	/* Read trans oil temp data on direct MCU PIN */
		}
		else if(Status > eMuxAnalogInputMaximum)
		{
			Status  = 0u;
		}
		TimeStamp = u32GetClockTicks();
	}

}

uint32_t GetADCValue(uint8_t Channel)
{
	return AdcDGetAdcValue[Channel];
}

uint16_t GetADCValueTransOilTemp()
{
	return AdcValueTransOilTemp;
}




