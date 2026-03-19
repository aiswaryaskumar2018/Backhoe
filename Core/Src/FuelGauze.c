/*
 * FuelGauze.c
 *
 *  Created on: Sep 20, 2024
 *      Author: aiswarya.kumar
 */


#include "adc.h"
#include "CurveHeader.h"
#include "FuelGauze.h"


#define FUEL_SIGNAL_AVG_SIZE			16u
#define FUEL_ADC_AVG_SIZE				16u
#define HYS								10u

#define OPEN_CKT_VALUE 					7655u//500U
#define SHORT_CKT_VAL_RES 				5u//500U

#define FUEL_ANALOG_ON_BAR1				18U		//ohm*10
#define FUEL_ANALOG_ON_BAR2				57U 	//ohm*10
#define FUEL_ANALOG_ON_BAR3				80U		//ohm*10
#define FUEL_ANALOG_ON_BAR4				104U	//ohm*10
#define FUEL_ANALOG_ON_BAR5				124U 	//ohm*10
#define FUEL_ANALOG_ON_BAR6				145U 	//ohm*10
#define FUEL_ANALOG_ON_BAR7				165U 	//ohm*10
#define FUEL_ANALOG_ON_BAR8				180U 	//ohm*10


#define FUEL_ANALOG_OFF_BAR1				15
#define FUEL_ANALOG_OFF_BAR2				52
#define FUEL_ANALOG_OFF_BAR3				75U
#define FUEL_ANALOG_OFF_BAR4				96U
#define FUEL_ANALOG_OFF_BAR5				115U
#define FUEL_ANALOG_OFF_BAR6				137U
#define FUEL_ANALOG_OFF_BAR7				156U
#define FUEL_ANALOG_OFF_BAR8				170U



struct
{
    uint32_t     u32CurrentValue;      						/* Hold Currect value of Gauge */
    uint32_t     u32FuelSignalData[FUEL_SIGNAL_AVG_SIZE];    /* Array for averaging signal samples */
    uint16_t	u16OpenCircuit;								/* Hold open circuit Value  */
    uint8_t     u8AvgCount;                    			    /* used in averaging samples */
    uint8_t    bSwitchOutStatus;							/* set True  for switchout */
    uint8_t		uint8_tFUELbarNo;
    uint32_t	uint16_tFUELSignalValue;
    uint32_t	uint16_tFUELPreviousSignal;
    uint32_t     u32FuelSignalADCData[FUEL_ADC_AVG_SIZE];    /* Array for Averaging Signal Samples */
} sFuel = { 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},	0,	0,	FALSE,0u,0,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

uint8_t TxData_FuelGauge[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};    // for can trx
uint8_t WarningBuzzerStatus = eStatusBlink;






void fuel_gauge_Process(void)
{
	uint32_t data = 0u;
	uint8_t i = 0u;
    uint8_t j = 0u;
    uint32_t AdcValue = 0u;
    uint32_t TempAdcValue = 0u;
    static uint8_t AvgCount = 0;
    static uint8_t AvgCountAdc = 0;
	static uint8_t OpentCktSense = 0u;
	static uint8_t ShortCktSense = FALSE;
	static uint8_t   FirstEntry = TRUE;
	uint32_t FuelCANTxData = 0;
	static uint32_t PrevAdcValue = 0u;


	// Take ADC value ------------------------------------------------*/
	AdcValue = GetADCValue(eMux_FuelGauge_AnalogInput);


    if(AdcValue >= OPEN_CKT_VALUE )  // Resistance value >=250
    {
    	OpentCktSense = TRUE;
    	AdcValue = 0u;
    	WarningBuzzerStatus = eStatusBlink;
    	sFuel.uint16_tFUELSignalValue = 0;
    	sFuel.uint8_tFUELbarNo = 0u;
    }
    else if(AdcValue <= (OPEN_CKT_VALUE - 50u) && OpentCktSense == TRUE )  // Resistance value <=230
    {
    	OpentCktSense = FALSE;
    	WarningBuzzerStatus = eStatusOff;
    }

    if(FirstEntry == TRUE)
	{
		for(j = 0; j < FUEL_ADC_AVG_SIZE; j++)
		{
			sFuel.u32FuelSignalADCData[j]  = AdcValue;
		}
	}
	sFuel.u32FuelSignalADCData[AvgCountAdc] = AdcValue;
	// calculate moving average of 8 samples
	AdcValue = 0U;
	for(j = 0U; j < FUEL_ADC_AVG_SIZE; j++)
	{
		AdcValue = AdcValue + sFuel.u32FuelSignalADCData[j];
	}

	AdcValue = AdcValue / FUEL_ADC_AVG_SIZE;
	if(++AvgCountAdc >= FUEL_ADC_AVG_SIZE)
	{
		AvgCountAdc = 0U;
	}
	//SetAdcValueForDisplay(AdcValue);

	vGetCurveDataValue(eResFuelCurve,AdcValue,&data); /* pass ADC avg. value for find resistance value from curve*/

	/* Calculate resistance Input------------- */
	if(FirstEntry == TRUE)
	{
		for(i = 0; i < FUEL_SIGNAL_AVG_SIZE; i++)
		{
			sFuel.u32FuelSignalData[i]  = data;
		}
		FirstEntry = FALSE;
	}

	sFuel.u32FuelSignalData[AvgCount] = data;
	/* Average of ADC samples ------------*/
	data = 0u;

	for(i = 0u; i < FUEL_SIGNAL_AVG_SIZE; i++)
		data = data + sFuel.u32FuelSignalData[i];
	data = data / FUEL_SIGNAL_AVG_SIZE;

	if(++AvgCount >= FUEL_SIGNAL_AVG_SIZE)
	{
		for(i = 0; i < FUEL_SIGNAL_AVG_SIZE - 1u; i++)
		{
			sFuel.u32FuelSignalData[i]=  sFuel.u32FuelSignalData[i+1];
		}
		AvgCount = FUEL_SIGNAL_AVG_SIZE - 1u;
	}


    if(data >= SHORT_CKT_VAL_RES && data <= 32u && OpentCktSense == FALSE)	// 32 ohms ADC Count
	{
    	WarningBuzzerStatus = eStatusON;
    	ShortCktSense = FALSE;
	}
    else if(data >= 38u && OpentCktSense == FALSE)
    {
    	WarningBuzzerStatus = eStatusOff;
    }

	else if(data < SHORT_CKT_VAL_RES && OpentCktSense == FALSE) // 8 ohms
	{
		WarningBuzzerStatus = eStatusBlink;
		ShortCktSense = TRUE;
	}

	sFuel.uint16_tFUELSignalValue = data;

	vGetCurveDataValue(eFuelCAN_TX,sFuel.uint16_tFUELSignalValue,&FuelCANTxData);  /* Getting fuel gauge can tx using curve */
	if((OpentCktSense == TRUE) || (ShortCktSense == TRUE))
	{
		TxData_FuelGauge[1] = 0xFE;
	}
	else
	{
		TxData_FuelGauge[1] = FuelCANTxData;                // saving fuel gauge value to first position of the buffer
	}

	if(OpentCktSense ==  FALSE)
	{
		if(sFuel.uint16_tFUELPreviousSignal !=  (uint16_t)sFuel.uint16_tFUELSignalValue)
		{
			sFuel.uint16_tFUELPreviousSignal = sFuel.uint16_tFUELSignalValue;
			//Ascending inputs
			if(sFuel.uint8_tFUELbarNo ==0 && sFuel.uint16_tFUELPreviousSignal < FUEL_ANALOG_ON_BAR1)
				sFuel.uint8_tFUELbarNo =0;
			else if(sFuel.uint8_tFUELbarNo <=1 && sFuel.uint16_tFUELPreviousSignal >= FUEL_ANALOG_ON_BAR1 && sFuel.uint16_tFUELPreviousSignal < FUEL_ANALOG_ON_BAR2 )
				sFuel.uint8_tFUELbarNo =1;
			else if(sFuel.uint8_tFUELbarNo <=2 && sFuel.uint16_tFUELPreviousSignal >= FUEL_ANALOG_ON_BAR2 && sFuel.uint16_tFUELPreviousSignal < FUEL_ANALOG_ON_BAR3 )
				sFuel.uint8_tFUELbarNo =2;
			else if(sFuel.uint8_tFUELbarNo <=3 && sFuel.uint16_tFUELPreviousSignal >= FUEL_ANALOG_ON_BAR3 && sFuel.uint16_tFUELPreviousSignal < FUEL_ANALOG_ON_BAR4 )
				sFuel.uint8_tFUELbarNo =3;
			else if(sFuel.uint8_tFUELbarNo <=4 && sFuel.uint16_tFUELPreviousSignal >= FUEL_ANALOG_ON_BAR4 && sFuel.uint16_tFUELPreviousSignal < FUEL_ANALOG_ON_BAR5 )
				sFuel.uint8_tFUELbarNo =4;
			else if(sFuel.uint8_tFUELbarNo <=5 && sFuel.uint16_tFUELPreviousSignal >= FUEL_ANALOG_ON_BAR5 && sFuel.uint16_tFUELPreviousSignal < FUEL_ANALOG_ON_BAR6 )
				sFuel.uint8_tFUELbarNo =5;
			else if(sFuel.uint8_tFUELbarNo <=6 && sFuel.uint16_tFUELPreviousSignal >= FUEL_ANALOG_ON_BAR6 && sFuel.uint16_tFUELPreviousSignal < FUEL_ANALOG_ON_BAR7 )
				sFuel.uint8_tFUELbarNo =6;
			else if(sFuel.uint8_tFUELbarNo <=7 && sFuel.uint16_tFUELPreviousSignal >= FUEL_ANALOG_ON_BAR7 && sFuel.uint16_tFUELPreviousSignal < FUEL_ANALOG_ON_BAR8 )
				sFuel.uint8_tFUELbarNo =7;
			else if(sFuel.uint8_tFUELbarNo <=8 && sFuel.uint16_tFUELPreviousSignal >= FUEL_ANALOG_ON_BAR8 )
				sFuel.uint8_tFUELbarNo =8;


			//Descending inputs
			else if(sFuel.uint8_tFUELbarNo > 0 && sFuel.uint16_tFUELPreviousSignal <= FUEL_ANALOG_OFF_BAR1)
				sFuel.uint8_tFUELbarNo = 0;
			else if(sFuel.uint8_tFUELbarNo > 1 && sFuel.uint16_tFUELPreviousSignal <= FUEL_ANALOG_OFF_BAR2)
				sFuel.uint8_tFUELbarNo = 1;
			else if(sFuel.uint8_tFUELbarNo > 2 && sFuel.uint16_tFUELPreviousSignal <= FUEL_ANALOG_OFF_BAR3)
				sFuel.uint8_tFUELbarNo = 2;
			else if(sFuel.uint8_tFUELbarNo > 3 && sFuel.uint16_tFUELPreviousSignal <= FUEL_ANALOG_OFF_BAR4)
				sFuel.uint8_tFUELbarNo = 3;
			else if(sFuel.uint8_tFUELbarNo > 4 && sFuel.uint16_tFUELPreviousSignal <= FUEL_ANALOG_OFF_BAR5)
				sFuel.uint8_tFUELbarNo = 4;
			else if(sFuel.uint8_tFUELbarNo > 5 && sFuel.uint16_tFUELPreviousSignal <= FUEL_ANALOG_OFF_BAR6)
				sFuel.uint8_tFUELbarNo = 5;
			else if(sFuel.uint8_tFUELbarNo > 6 && sFuel.uint16_tFUELPreviousSignal <= FUEL_ANALOG_OFF_BAR7)
				sFuel.uint8_tFUELbarNo = 6;
			else if(sFuel.uint8_tFUELbarNo > 7 && sFuel.uint16_tFUELPreviousSignal <= FUEL_ANALOG_OFF_BAR8)
				sFuel.uint8_tFUELbarNo = 7;

		}
	}

}

uint8_t GetFuelCANTxData(void)
{
	return TxData_FuelGauge[1];
}

uint8_t GetFuelGaugeBar(void)
{
	return sFuel.uint8_tFUELbarNo;
}

uint8_t GetFuelWarningBuzzerStatus()
{
	return WarningBuzzerStatus;
}


