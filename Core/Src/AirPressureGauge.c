/* CA78K0R C Source Converter V1.00.03.01 [10 May 2018] */
/*****************************************************************************
 DISCLAIMER
 This software is supplied by Renesas Electronics Corporation and is only
 intended for use with Renesas products. No other uses are authorized. This
 software is owned by Renesas Electronics Corporation and is protected under
 all applicable laws, including copyright laws.
 THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 Renesas reserves the right, without notice, to make changes to this software
 and to discontinue the availability of this software. By using this software,
 you agree to the additional terms and conditions found by accessing the
 following link:
 http://www.renesas.com/disclaimer
 Copyright (C) 2016-2018 Renesas Electronics Corporation. All rights reserved.
******************************************************************************/

#include "AirPressureGauge.h" 
#include "adc.h"
#include "CurveHeader.h"
#include "main.h"



#define AP_GAUGE_SIGNAL_AVG_SIZE			16u
#define AP_GAUGE_ADC_AVG_SIZE				16u
#define HYS									5u

#define OPEN_CKT_VALUE 						7655u//500U

#define AP_GAUGE_ANALOG_ON_BAR1				30U		//ohm*10
#define AP_GAUGE_ANALOG_ON_BAR2				51U 	//ohm*10
#define AP_GAUGE_ANALOG_ON_BAR3				68U		//ohm*10
#define AP_GAUGE_ANALOG_ON_BAR4				86U		//ohm*10
#define AP_GAUGE_ANALOG_ON_BAR5				104U 	//ohm*10
#define AP_GAUGE_ANALOG_ON_BAR6				122U 	//ohm*10
#define AP_GAUGE_ANALOG_ON_BAR7				139U 	//ohm*10
#define AP_GAUGE_ANALOG_ON_BAR8				157U 	//ohm*10
#define AP_GAUGE_ANALOG_ON_BAR9				174U 	//ohm*10
#define AP_GAUGE_ANALOG_ON_BAR10			192U 	//ohm*10


#define AP_GAUGE_ANALOG_OFF_BAR1			AP_GAUGE_ANALOG_ON_BAR1 - HYS
#define AP_GAUGE_ANALOG_OFF_BAR2			AP_GAUGE_ANALOG_ON_BAR2 - HYS
#define AP_GAUGE_ANALOG_OFF_BAR3			AP_GAUGE_ANALOG_ON_BAR3 - HYS
#define AP_GAUGE_ANALOG_OFF_BAR4			AP_GAUGE_ANALOG_ON_BAR4 - HYS
#define AP_GAUGE_ANALOG_OFF_BAR5			AP_GAUGE_ANALOG_ON_BAR5 - HYS
#define AP_GAUGE_ANALOG_OFF_BAR6			AP_GAUGE_ANALOG_ON_BAR6 - HYS
#define AP_GAUGE_ANALOG_OFF_BAR7			AP_GAUGE_ANALOG_ON_BAR7 - HYS
#define AP_GAUGE_ANALOG_OFF_BAR8			AP_GAUGE_ANALOG_ON_BAR8 - HYS
#define AP_GAUGE_ANALOG_OFF_BAR9			AP_GAUGE_ANALOG_ON_BAR9 - HYS
#define AP_GAUGE_ANALOG_OFF_BAR10			AP_GAUGE_ANALOG_ON_BAR10 - HYS



struct
{
    uint32_t     u32CurrentValue;      						/* Hold Currect value of Gauge */
    uint32_t     u32AP_GAUGESignalData[AP_GAUGE_SIGNAL_AVG_SIZE];    /* Array for averaging signal samples */
    uint16_t	u16OpenCircuit;								/* Hold open circuit Value  */
    uint8_t     u8AvgCount;                    			    /* used in averaging samples */
    uint8_t    bSwitchOutStatus;							/* set True  for switchout */
    uint8_t		uint8_tAP_GAUGEbarNo;
    uint16_t		uint16_tAP_GAUGESignalValue;
    uint16_t		uint16_tAP_GAUGEPreviousSignal;
    uint32_t     u32AP_GAUGESignalADCData[AP_GAUGE_ADC_AVG_SIZE];    /* Array for Averaging Signal Samples */
} sAP_GAUGE = { 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},	0,	0,	FALSE,0u,0,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};


uint8_t AirPressureWarningBuzzerStatus = eStatusBlink;






void AirPressure_Process(void)
{
	uint32_t data = 0u;
	uint8_t i = 0u;
    uint8_t j = 0u;
    uint32_t AdcValue = 0u;
    static uint8_t AvgCount = 0;
    static uint8_t AvgCountAdc = 0;
	static uint8_t OpentCktSense = 0u;
	static uint8_t   FirstEntry = TRUE;



	// Take ADC value ------------------------------------------------*/
	AdcValue = GetADCValue(eMux_AirPressure_AnalogInput);


    if(AdcValue >= OPEN_CKT_VALUE)  // Resistance value >=250
    {
    	OpentCktSense = TRUE;
    	data = 0u;
    	AirPressureWarningBuzzerStatus = eStatusBlink;
    	sAP_GAUGE.uint8_tAP_GAUGEbarNo = 0u;
    	AdcValue = 0;
    }
    else if(AdcValue <= (OPEN_CKT_VALUE - 50u) && OpentCktSense == TRUE)  // Resistance value <=230
    {
    	OpentCktSense = FALSE;
    	AirPressureWarningBuzzerStatus = eStatusOff;
    }
	/*--------------------------------------------------------------*/
    if(FirstEntry == TRUE)
	{
		for(j = 0; j < AP_GAUGE_ADC_AVG_SIZE; j++)
		{
			sAP_GAUGE.u32AP_GAUGESignalADCData[j]  = AdcValue;
		}
	}
	sAP_GAUGE.u32AP_GAUGESignalADCData[AvgCountAdc] = AdcValue;
	// calculate moving average of 8 samples
	AdcValue = 0U;
	for(j = 0U; j < AP_GAUGE_ADC_AVG_SIZE; j++)
	{
		AdcValue = AdcValue + sAP_GAUGE.u32AP_GAUGESignalADCData[j];
	}

	AdcValue = AdcValue / AP_GAUGE_ADC_AVG_SIZE;
	if(++AvgCountAdc >= AP_GAUGE_ADC_AVG_SIZE)
	{
		AvgCountAdc = 0U;
	}
	//SetAdcValueForDisplay(AdcValue);

	vGetCurveDataValue(eAirPressureSensor,AdcValue,&data); /* pass ADC avg. value for find resistance value from curve*/

	/* Calculate resistance Input------------- */
	if(FirstEntry == TRUE)
	{
		for(i = 0; i < AP_GAUGE_SIGNAL_AVG_SIZE; i++)
		{
			sAP_GAUGE.u32AP_GAUGESignalData[i]  = data;
		}
		FirstEntry = FALSE;
	}

	sAP_GAUGE.u32AP_GAUGESignalData[AvgCount] = data;
	/* Average of ADC samples ------------*/
	data = 0u;

	for(i = 0u; i < AP_GAUGE_SIGNAL_AVG_SIZE; i++)
		data = data + sAP_GAUGE.u32AP_GAUGESignalData[i];
	data = data / AP_GAUGE_SIGNAL_AVG_SIZE;

	if(++AvgCount >= AP_GAUGE_SIGNAL_AVG_SIZE)
	{
		for(i = 0; i < AP_GAUGE_SIGNAL_AVG_SIZE - 1u; i++)
		{
			sAP_GAUGE.u32AP_GAUGESignalData[i]=  sAP_GAUGE.u32AP_GAUGESignalData[i+1];
		}
		AvgCount = AP_GAUGE_SIGNAL_AVG_SIZE - 1u;
	}
	/*---------------------------------------------------------------------------*/
	sAP_GAUGE.uint16_tAP_GAUGESignalValue = data;




    if(data >= 10u && data <= 123u && OpentCktSense == FALSE)	// 123 ohms ADC Count
	{
    	AirPressureWarningBuzzerStatus = eStatusON;
	}
    else if(data >= 130u && OpentCktSense == FALSE)
    {
    	AirPressureWarningBuzzerStatus = eStatusOff;
    }

	else if(data <= 8u && OpentCktSense == FALSE) // 8 ohms
	{
		AirPressureWarningBuzzerStatus = eStatusBlink;
	}
	else
	{
		/*NA*/
	}


	if(OpentCktSense ==  FALSE)
	{
		if(sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal !=  (uint16_t)sAP_GAUGE.uint16_tAP_GAUGESignalValue)
		{
			sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal = sAP_GAUGE.uint16_tAP_GAUGESignalValue;
			//Ascending inputs
			if(sAP_GAUGE.uint8_tAP_GAUGEbarNo == 0 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal < AP_GAUGE_ANALOG_ON_BAR1)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 0;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo <= 1 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal >= AP_GAUGE_ANALOG_ON_BAR1 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal < AP_GAUGE_ANALOG_ON_BAR2 )
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 1;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo <= 2 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal >= AP_GAUGE_ANALOG_ON_BAR2 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal < AP_GAUGE_ANALOG_ON_BAR3 )
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 2;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo <= 3 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal >= AP_GAUGE_ANALOG_ON_BAR3 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal < AP_GAUGE_ANALOG_ON_BAR4 )
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 3;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo <= 4 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal >= AP_GAUGE_ANALOG_ON_BAR4 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal < AP_GAUGE_ANALOG_ON_BAR5 )
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 4;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo <= 5 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal >= AP_GAUGE_ANALOG_ON_BAR5 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal < AP_GAUGE_ANALOG_ON_BAR6 )
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 5;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo <= 6 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal >= AP_GAUGE_ANALOG_ON_BAR6 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal < AP_GAUGE_ANALOG_ON_BAR7 )
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 6;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo <= 7 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal >= AP_GAUGE_ANALOG_ON_BAR7 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal < AP_GAUGE_ANALOG_ON_BAR8 )
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 7;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo <= 8 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal >= AP_GAUGE_ANALOG_ON_BAR8 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal < AP_GAUGE_ANALOG_ON_BAR9 )
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 8;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo <= 9 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal >= AP_GAUGE_ANALOG_ON_BAR9 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal < AP_GAUGE_ANALOG_ON_BAR10 )
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 9;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo <= 10 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal >= AP_GAUGE_ANALOG_ON_BAR10 )
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 10;


			//Descending inputs
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo > 0 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal <= AP_GAUGE_ANALOG_OFF_BAR1)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 0;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo > 1 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal <= AP_GAUGE_ANALOG_OFF_BAR2)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 1;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo > 2 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal <= AP_GAUGE_ANALOG_OFF_BAR3)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 2;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo > 3 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal <= AP_GAUGE_ANALOG_OFF_BAR4)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 3;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo > 4 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal <= AP_GAUGE_ANALOG_OFF_BAR5)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 4;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo > 5 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal <= AP_GAUGE_ANALOG_OFF_BAR6)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 5;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo > 6 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal <= AP_GAUGE_ANALOG_OFF_BAR7)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 6;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo > 7 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal <= AP_GAUGE_ANALOG_OFF_BAR8)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 7;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo > 8 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal <= AP_GAUGE_ANALOG_OFF_BAR9)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 8;
			else if(sAP_GAUGE.uint8_tAP_GAUGEbarNo > 9 && sAP_GAUGE.uint16_tAP_GAUGEPreviousSignal <= AP_GAUGE_ANALOG_OFF_BAR10)
				sAP_GAUGE.uint8_tAP_GAUGEbarNo = 9;

		}
	}

}


uint8_t getAirPressure(void)
{
	return sAP_GAUGE.uint8_tAP_GAUGEbarNo;
}

uint8_t GetAirPressureWarningBuzzerStatus()
{
	return AirPressureWarningBuzzerStatus;
}
