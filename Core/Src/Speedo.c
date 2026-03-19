/*
 * Speedo.c
 *
 *  Created on: Oct 11, 2024
 *      Author: dharmendra.singh
 */

#include "Speedo.h"

#define SPEEDO_FREQ_AVG_SIZE		26


// local declarations --------------------------------------------------------
typedef struct
{
	uint32_t     u32Frequency;
	uint32_t     u32Signaldata[SPEEDO_FREQ_AVG_SIZE];    //For frequency or CAN data averaging
	uint16_t     u16Value; 								//Speed value
	uint16_t     u16SpeedoMaxfreq;						//Speedo maximum range frequency
	uint16_t     u16CutOffFreq;							// speed cut of frequency
	uint16_t 	u16k1SpeedoFactor;						//EOL cofigurable used for ODO calculation
	uint16_t 	u16k2SpeedoFactor;						//EOL cofigurable used for speed calulation
	uint8_t      u8OverSpeed;							// over speed for speedo
	uint8_t      u8SpeedOffset;							// over speed for speedo

} SPEEDO_DATA_TYPE;


SPEEDO_DATA_TYPE sSpeedo = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};



uint32_t GetFrequencyValue(void)
{
	static uint32_t FreqTimeOutStamp = 0;
	static uint32_t TachoMeterUpdateFlag = FALSE;
	uint32_t data = 0;
	uint8_t i = 0;
	uint32_t temp = 0;
	uint32_t temp1 = 0;
	static uint8_t AvgSampleCount = 0;
	static uint8_t NoOfAvgSample = 0;
	static uint8_t speedoupdateFlag = 0;
	uint8_t StatusFlag = 0;


	if(GetFreqCapturedStatus() == TRUE)
	{
		SetFreqCapturedStatus(FALSE);
		if((u32GetClockTicks() - FreqTimeOutStamp) <= FREQ_OUT_TIME )
		{
		 	TachoMeterUpdateFlag = TRUE;
		}
		FreqTimeOutStamp = u32GetClockTicks();
	}
	else if((u32GetClockTicks() - FreqTimeOutStamp) >= FREQ_OUT_TIME + 10)
	{
		//Fill zero in avg array
		SetFinalFreq(0);
		TachoMeterUpdateFlag = FALSE;
		sSpeedo.u32Frequency = 0;
	}
	data = GetFinalFreq() + 2u;

	if(data > 100)
		NoOfAvgSample = 25;
	else if(data > 50)
		NoOfAvgSample = 10;
	else if(data > 30)
		NoOfAvgSample = 5;
	else if(data > 10)
		NoOfAvgSample = 3;
	else if(data > 5)
		NoOfAvgSample = 2;
	else
		NoOfAvgSample = 1;

	// Averging of signal
	sSpeedo.u32Signaldata[AvgSampleCount] = data;

	if(++AvgSampleCount >= NoOfAvgSample)
	{
		temp = 0;
		for(i = 0; i < NoOfAvgSample; i++)
			temp = temp + sSpeedo.u32Signaldata[i];
		temp = temp / NoOfAvgSample;

		for(i = 0; i < NoOfAvgSample - 1; i++)
			sSpeedo.u32Signaldata[i]=  sSpeedo.u32Signaldata[i+1];
		AvgSampleCount = NoOfAvgSample - 1;
		sSpeedo.u32Frequency = temp;
	}

	//if frequency is coming more Max freq then stop odo meter counting
	if(sSpeedo.u32Frequency >= 3000u)
	{
		vSetAnalougeOdoEnable(FALSE);
		sSpeedo.u32Frequency = 0u;
	}
	else if(sSpeedo.u32Frequency < 2950u)
	{
		vSetAnalougeOdoEnable(TRUE);
	}
	if(TachoMeterUpdateFlag)
	{
		return sSpeedo.u32Frequency;
	}
	else
	{
		return 0;
	}
}


uint32_t GetVehicleSpeed(void)
{
	uint32_t VehicleSpeed = 0;

	VehicleSpeed = ((GetFrequencyValue() * MAX_SPEED_FRQ)/MAX_FREQ_SPEEDO);
	if(VehicleSpeed > MAX_SPEED_FRQ)
	{
		VehicleSpeed = MAX_SPEED_FRQ;
	}

	return VehicleSpeed;
}
