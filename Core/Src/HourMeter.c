/*
 * HourMeter.c
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */
#include "HourMeter.h"
#include "J1939.h"
#include "extEEPROM.h"

#define HOUR_CLOCK_TIMEOUT_RCV			35000u			// timeout for hourmeter can mesaage is 70 sec
#define MAX_HOUR_ODO_VALUE 				9999999u



static uint8_t gbResetFlag = TRUE;						// set true by default and if hourmeter data is not ready set clock off otherwise
static uint32_t FinalCurrentHourData = 0u;						// set false in timeout or data is ready
uint8_t EngineHourSymbolFlag = FALSE;
uint8_t HourDataBlankScreenStatus = FALSE;
static uint32_t HourTripAValue = 0;
static uint32_t HourTripBValue = 0;
static uint8_t HourTripAOverFlowFlag = FALSE;
static uint8_t HourTripBOverFlowFlag = FALSE;

void CalculateTripdata(void);

/***************************************************************************//**
 *
 *   Extract CAN data for Engine Hours Acordingly process hourmeter symbol & data to lcd
 *
 *@param    none
 *
 *@return   none
 *
 *****************************************************************************/
void vHourMeterCAN()
{
	uint32_t hourdata = 0u;
	static uint32_t blinktimestamp = 0u;
	uint8_t btimeout = 0,bDataReady = 0u;


	btimeout = bGetPGNTimeOut(eHourMeter_AshokEicherEng);
	hourdata = u32GetPGNdata(eHourMeter_AshokEicherEng);
	bDataReady = bGetSPNDataReady(eHourMeter_AshokEicherEng);

	FinalCurrentHourData = ((hourdata * 50) / 100);

	if(hourdata >= BADDATA_4BYTE)
	{
		FinalCurrentHourData = 0u;
	}
	if(FinalCurrentHourData > MAX_HOUR_ODO_VALUE)
	{
		FinalCurrentHourData = MAX_HOUR_ODO_VALUE;
	}


	if(gbResetFlag && !bDataReady)
	{
		HourDataBlankScreenStatus = TRUE;
		EngineHourSymbolFlag = 	eStatusON;
	}
	else if(btimeout)
	{
		gbResetFlag = FALSE;
		HourDataBlankScreenStatus = FALSE;
		EngineHourSymbolFlag = 	eStatusON;
	}
	else
	{
		HourDataBlankScreenStatus = FALSE;
		gbResetFlag = FALSE;
		EngineHourSymbolFlag = 	eStatusBlink;
	}

	CalculateTripHourdata();

}

uint32_t GetHourDataValue(void)
{
	return (FinalCurrentHourData);
}

uint8_t GetHourDataComingOrNotStatus()
{
	return HourDataBlankScreenStatus;
}


uint8_t GetHourSymbolBlinkStatus()
{
	static uint8_t Status = FALSE;
	static uint32_t u32clkTime = 0;
	static uint16_t u16CanOutTime = 0;


	switch(EngineHourSymbolFlag)
	{
		case eStatusON:
			Status = TRUE;
		break;
		case eStatusBlink:
			if (u32GetClockTicks() - u32clkTime  > u16CanOutTime)
			{
				u32clkTime = u32GetClockTicks();
				Status ^= TRUE;
				if (Status)
				{
					u16CanOutTime = 500u;
				}
				else
				{
					u16CanOutTime = 500u;
				}
			}
		break;
		default:
			Status = TRUE;
		break;

	}

	return Status;
}


void SetTripAHourValue(uint32_t Val)
{
	HourTripAValue = Val;
}

void SetTripBHourValue(uint32_t Val)
{
	HourTripBValue = Val;
}

uint32_t GetTripAHourValue(void)
{
	return HourTripAValue;
}

uint32_t GetTripBHourValue(void)
{
	return HourTripBValue;
}

void CalculateTripHourdata(void)
{
	uint32_t LastSavedHourValueTripA = 0u;
	uint32_t LastSavedHourValueTripB = 0u;
	static uint32_t prevFinalCurrentHourData = 0;

	LastSavedHourValueTripA = GetLastSavedHourValueForTripA();
	LastSavedHourValueTripB = GetLastSavedHourValueForTripB();

	if(prevFinalCurrentHourData != FinalCurrentHourData)
	{
		if(FinalCurrentHourData > LastSavedHourValueTripA)
		{
			HourTripAValue = FinalCurrentHourData - LastSavedHourValueTripA;
			if(HourTripAValue > MAX_HOUR_TRIP_A)
			{
				HourTripAOverFlowFlag = TRUE;
			}
		}
		else
		{
			HourTripAValue = 0;
		}

		if(FinalCurrentHourData > LastSavedHourValueTripB)
		{
			HourTripBValue = FinalCurrentHourData - LastSavedHourValueTripB;
			if(HourTripBValue > MAX_HOUR_TRIP_B)
			{
				HourTripBOverFlowFlag = TRUE;
			}
		}
		else
		{
			HourTripBValue = 0;
		}
		prevFinalCurrentHourData = FinalCurrentHourData;
	}
}

void SetHourTripBOverFlowFlag(uint8_t Status)
{
	HourTripBOverFlowFlag = Status;
}

void SetHourTripAOverFlowFlag(uint8_t Status)
{
	HourTripAOverFlowFlag = Status;
}

uint8_t GetHourTripBOverFlowFlag(void)
{
	return HourTripBOverFlowFlag;
}

uint8_t GetHourTripAOverFlowFlag(void)
{
	return HourTripAOverFlowFlag;
}













