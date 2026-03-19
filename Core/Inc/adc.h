/*
 * adc.h
 *
 *  Created on: Sep 18, 2024
 *      Author: aiswarya.kumar
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"



typedef enum
{
	//eMux_adc_Input2 = 0u,
	//eMux_adc_Input1  = 0u,
	eMux_AirPressure_AnalogInput = 0u,
	eMux_FuelGauge_AnalogInput,
	//eMux_TransoilTemp_AnalogInput,
	//eMux_TransoilPressure_AnalogInput,
	//eMux_AdcRefVoltage,
	eMuxAnalogInputMaximum
};



uint32_t GetADCValue(uint8_t Channel);
void AdcDataProcessing(void);
uint16_t GetADCValueTransOilTemp();

#endif /* INC_ADC_H_ */

