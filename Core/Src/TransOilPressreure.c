/*
 * TransTRANS_OILPressreure.c
 *
 *  Created on: Sep 20, 2024
 *      Author: aiswarya.kumar
 */

#include "TransOilPressreure.h"
#include"adc.h"

#include "CurveHeader.h"


#define TRANS_OIL_GAUGE_SIGNAL_AVG				16U
#define HYS										10u

#define OPEN_CKT_VALUE 							440U

#define TRANS_OIL_ANALOG_ON_BAR1				231U	//ohm
#define TRANS_OIL_ANALOG_ON_BAR2				270U 	//ohm
#define TRANS_OIL_ANALOG_ON_BAR3				299U	//ohm
#define TRANS_OIL_ANALOG_ON_BAR4				355U	//ohm


#define TRANS_OIL_ANALOG_OFF_BAR1				140U	//ohm*10
#define TRANS_OIL_ANALOG_OFF_BAR2				(TRANS_OIL_ANALOG_ON_BAR2 - HYS)	//ohm*10
#define TRANS_OIL_ANALOG_OFF_BAR3				(TRANS_OIL_ANALOG_ON_BAR3 - HYS)//ohm*10
#define TRANS_OIL_ANALOG_OFF_BAR4				(TRANS_OIL_ANALOG_ON_BAR4 - HYS)	//ohm*10

uint8_t WarningLampStatus = FALSE;
uint8_t TransOilBuzzerStatus = FALSE;

typedef struct
{
	uint16_t     uint16_tOilSignalValue;							// APG ADC value
	uint16_t     uint16_tOilSignalData[TRANS_OIL_GAUGE_SIGNAL_AVG];		// Array for averaging signal samples
	uint16_t     uint16_tOilPreviousSignal;
	uint8_t      uint8_tOilbarNo;
	uint8_t 	bSymboleStatus;
	uint8_t	bOpenCktFlag;
	uint8_t	bShortCktFlag;

} OIL_PRESSURE_DATA_TYPE;

OIL_PRESSURE_DATA_TYPE	sTransOilPressure = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


uint32_t Trans_TRANS_OIL_pressure_process(void)
{
	uint16_t data = 0;
	uint16_t AdcValue = 0;
	uint8_t i = 0;
	static uint8_t   AvgCount = 0;
	static uint16_t Timestamp = 0;
	static uint8_t OpenCktSense = 0;
	static uint16_t OpenCktSenseDelay = 0;
	static uint8_t   FirstEntry = TRUE;
	//Take ADC value
	uint8_t Count = 0;

	//Take ADC value
	//for(Count = 0u; Count < 20u; Count++)
	//{
		//data = GetADCValue(eMux_TransoilPressure_AnalogInput);
	//}
	//SetAdcValueForDisplay(data);
	if(data >= OPEN_CKT_VALUE)  // Resistance value >=250
	{
		OpenCktSense = TRUE;
		data = 0u;
		WarningLampStatus = eStatusBlink;
		sTransOilPressure.uint8_tOilbarNo = 0;
	}
	else if(data <= (OPEN_CKT_VALUE - 10) && OpenCktSense == TRUE)  // Resistance value <=230
	{
		OpenCktSense = FALSE;
		WarningLampStatus = eStatusOff;
	}

	else if(data >= 26 && data < TRANS_OIL_ANALOG_ON_BAR1 && OpenCktSense == FALSE)
	{
		WarningLampStatus = eStatusON;
	}
	else if(data > 242u && OpenCktSense == FALSE)		// 115 ohms
	{
		WarningLampStatus = eStatusOff;
	}
	else if(data < 26 && OpenCktSense == FALSE) // 8 ohms
	{
		WarningLampStatus = eStatusBlink;
	}

	//****************************************
	if(FirstEntry == TRUE)
	{
		for(i = 0; i < TRANS_OIL_GAUGE_SIGNAL_AVG; i++)
			sTransOilPressure.uint16_tOilSignalData[i]  = data;
		FirstEntry = FALSE;
	}
	sTransOilPressure.uint16_tOilSignalData[AvgCount] = data;
	//Average of ADC samples
	sTransOilPressure.uint16_tOilSignalValue = 0;
	for(i = 0; i < TRANS_OIL_GAUGE_SIGNAL_AVG; i++)
		sTransOilPressure.uint16_tOilSignalValue = sTransOilPressure.uint16_tOilSignalValue + sTransOilPressure.uint16_tOilSignalData[i];
	sTransOilPressure.uint16_tOilSignalValue = sTransOilPressure.uint16_tOilSignalValue / TRANS_OIL_GAUGE_SIGNAL_AVG;

	if(++AvgCount >= TRANS_OIL_GAUGE_SIGNAL_AVG)
	{
		for(i = 0; i < TRANS_OIL_GAUGE_SIGNAL_AVG - 1; i++)
			sTransOilPressure.uint16_tOilSignalData[i]=  sTransOilPressure.uint16_tOilSignalData[i+1];
		AvgCount = TRANS_OIL_GAUGE_SIGNAL_AVG - 1;
	}

	if(OpenCktSense == FALSE)
	{
		if(sTransOilPressure.uint16_tOilPreviousSignal !=  (uint16_t)sTransOilPressure.uint16_tOilSignalValue)
		{
			sTransOilPressure.uint16_tOilPreviousSignal = sTransOilPressure.uint16_tOilSignalValue;
			//Ascending inputs
			if(sTransOilPressure.uint8_tOilbarNo == 0 && sTransOilPressure.uint16_tOilPreviousSignal < TRANS_OIL_ANALOG_ON_BAR1)
				sTransOilPressure.uint8_tOilbarNo = 0;
			else if(sTransOilPressure.uint8_tOilbarNo <= 1 && sTransOilPressure.uint16_tOilPreviousSignal >= TRANS_OIL_ANALOG_ON_BAR1 && sTransOilPressure.uint16_tOilPreviousSignal < TRANS_OIL_ANALOG_ON_BAR2 )
				sTransOilPressure.uint8_tOilbarNo = 1;
			else if(sTransOilPressure.uint8_tOilbarNo <= 2 && sTransOilPressure.uint16_tOilPreviousSignal >= TRANS_OIL_ANALOG_ON_BAR2 && sTransOilPressure.uint16_tOilPreviousSignal < TRANS_OIL_ANALOG_ON_BAR3 )
				sTransOilPressure.uint8_tOilbarNo = 2;
			else if(sTransOilPressure.uint8_tOilbarNo <= 3 && sTransOilPressure.uint16_tOilPreviousSignal >= TRANS_OIL_ANALOG_ON_BAR3 && sTransOilPressure.uint16_tOilPreviousSignal < TRANS_OIL_ANALOG_ON_BAR4 )
				sTransOilPressure.uint8_tOilbarNo = 3;
			else if(sTransOilPressure.uint8_tOilbarNo <= 4 && sTransOilPressure.uint16_tOilPreviousSignal >= TRANS_OIL_ANALOG_ON_BAR4 )
				sTransOilPressure.uint8_tOilbarNo = 4;

			//Descending inputs
			else if(sTransOilPressure.uint8_tOilbarNo > 0 && sTransOilPressure.uint16_tOilPreviousSignal <= TRANS_OIL_ANALOG_OFF_BAR1)
				sTransOilPressure.uint8_tOilbarNo = 0;
			else if(sTransOilPressure.uint8_tOilbarNo > 1 && sTransOilPressure.uint16_tOilPreviousSignal <= TRANS_OIL_ANALOG_OFF_BAR2)
				sTransOilPressure.uint8_tOilbarNo = 1;
			else if(sTransOilPressure.uint8_tOilbarNo > 2 && sTransOilPressure.uint16_tOilPreviousSignal <= TRANS_OIL_ANALOG_OFF_BAR3)
				sTransOilPressure.uint8_tOilbarNo = 2;
			else if(sTransOilPressure.uint8_tOilbarNo > 3 && sTransOilPressure.uint16_tOilPreviousSignal <= TRANS_OIL_ANALOG_OFF_BAR4)
				sTransOilPressure.uint8_tOilbarNo = 3;

		}
	}
}


uint8_t GetTransOilBar()
{
	return sTransOilPressure.uint8_tOilbarNo;
}

uint16_t get_Trans_oil_pressure(void)
{
	uint16_t data = 0;
	data = adc_monitor(1);		// for adc value debug only
	return data;
}

uint8_t GetTrnsOIlPressureLampWarningStatus()
{
	static uint8_t Status = FALSE;
	static uint32_t u32clkTime = 0;
	static uint16_t u16CanOutTime = 0;

	switch(WarningLampStatus)
	{
		case eStatusON:
			Status = TRUE;
			TransOilBuzzerStatus = FALSE;
		break;
		case eStatusBlink:
			if (u32GetClockTicks() - u32clkTime  > u16CanOutTime)
			{
				u32clkTime = u32GetClockTicks();
				Status ^= TRUE;
				if (Status)
				{
					u16CanOutTime = INDICATOR_ON_TIME;
				}
				else
				{
					u16CanOutTime = INDICATOR_OFF_TIME;
				}
			}
			TransOilBuzzerStatus = FALSE;
		break;
		case eStatusOff:
			Status = FALSE;
			TransOilBuzzerStatus = FALSE;
		break;
		default:
			Status = FALSE;
			TransOilBuzzerStatus = FALSE;
		break;

	}

	return Status;

}

uint8_t GetTrnsOIlPressureBuzzerStatus()
{
	return TransOilBuzzerStatus;
}







