/*
 * SootLoad.c
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */

#include "J1939.h"
#include "SootLoad.h"


#define SootLoad_SIGNAL_AVG              	4
#define SootLoad_HYS						10		//SootLoad


uint16_t SootLoadvalue = 0;
uint8_t BadTimeOutdataFlag = FALSE;


// local declarations --------------------------------------------------------
typedef struct
{
	uint16_t     u16EngineSootLoad;
	uint16_t     u16SootLoadSignalData[SootLoad_SIGNAL_AVG];
	uint16_t	u16EngineOverSpeed;
} SootLoad_DATA_TYPE;

SootLoad_DATA_TYPE sSootLoad = {0x00,0x00,0x00};

/*********************************************************************//**
 *
 * Check CAN msg and CAN lost msg, Do average of CAN signal and calculate
*  Stepper target position for SootLoad gauge
 *
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void vSootLoadDataProcessing() {
	uint32_t data = 0u;
	uint32_t temp = 0u;
	uint8_t i = 0u;
	static uint8_t AvgSampleCount = 0u;			// Counter for averaging samples

	uint8_t engineSelection = 0;


	if(GetEngineSelectionType() == eBackhoeLoaderKoelEngine74Hp)
	{
		data = u32GetPGNdata(eSootMass);
		//Check CAN message lost
		if (bGetPGNTimeOut(eSootMass) || data >= BADDATA)
		{
			data = 0;
			BadTimeOutdataFlag = TRUE;
		}
		else
		{
			BadTimeOutdataFlag = FALSE;
			//Go back if new sample is not coming

		}

		//Averging of signal
		sSootLoad.u16SootLoadSignalData[AvgSampleCount] = (uint16_t) data;

		if (++AvgSampleCount >= SootLoad_SIGNAL_AVG)
		{
			AvgSampleCount = 0;
		}
		data = 0;
		for (i = 0; i < SootLoad_SIGNAL_AVG; i++)
			data = data + sSootLoad.u16SootLoadSignalData[i];
		data = data / SootLoad_SIGNAL_AVG;

		data = data * 4;                              // SootMass physical value

		sSootLoad.u16EngineSootLoad = (uint16_t) data;

		SootLoadvalue = sSootLoad.u16EngineSootLoad;
		if (SootLoadvalue >= 1000)
		{
			SootLoadvalue = 1000;
		}
	}
	else
	{
		data = u32GetPGNdata(eShootLoad);
		//Check CAN message lost
		if (bGetPGNTimeOut(eShootLoad) || data >= BADDATA)
		{
			data = 0;
			BadTimeOutdataFlag = TRUE;
		}
		else
		{
			BadTimeOutdataFlag = FALSE;
			//Go back if new sample is not coming

		}

		//Averging of signal
		sSootLoad.u16SootLoadSignalData[AvgSampleCount] = (uint16_t) data;

		if (++AvgSampleCount >= SootLoad_SIGNAL_AVG)
		{
			AvgSampleCount = 0;
		}
		data = 0;
		for (i = 0; i < SootLoad_SIGNAL_AVG; i++)
			data = data + sSootLoad.u16SootLoadSignalData[i];
		data = data / SootLoad_SIGNAL_AVG;

		//data = data * 4;                              // SootMass physical value

		sSootLoad.u16EngineSootLoad = (uint16_t) data;

		SootLoadvalue = sSootLoad.u16EngineSootLoad;
		if (SootLoadvalue >= 250)
		{
			SootLoadvalue = 250;
		}
	}
}

/*********************************************************************//**
 *
 * Current engine speed of vehicle
 *
 * @param	None
 *
 * @return	Current Engine speed
 *************************************************************************/
uint16_t u16GetSootLoadValue(void)
{
	return SootLoadvalue;
}

/*********************************************************************//**
 *
 * SootLoad BadData Timeout Status
 *
 * @param	None
 *
 * @return	Uint8
 *************************************************************************/
uint8_t u8GetSootLoadBadDataTimeoutStatus(void)
{
	return BadTimeOutdataFlag;
}



