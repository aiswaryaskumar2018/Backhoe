/*
 * SpeedoOdo.c
 *
 *  Created on: Sep 24, 2024
 *      Author: dharmendra.singh
 */

#include "Odo.h"


ODO_DATA_TYPE sODO = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

uint32_t IC_Value1 = 0u;
uint32_t IC_Value2 = 0u;
uint32_t Diffrence = 0u;
uint32_t Frequency = 0u;
uint8_t IsFirstCaptured = 0u;
uint32_t TotalOdoPulse = 0u;
uint8_t TotalOdoPulseFraction = 0u;
uint32_t OdoCount = 0u;
uint16_t OdoTripACount = 0u;
uint8_t FreqCapturedFlag = TRUE;
uint8_t OneKmStatus = FALSE;
static uint8_t bOdoEnableFlag = 0u;
static uint16_t u16SpeedoPulses = 0u;

#define LCD_DIGIT_FULL 999999U




void vSetAnalougeOdoEnable(uint8_t status);




void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{
		if(IsFirstCaptured == 0)
		{
			IC_Value1 = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_4); // captured the seconds ch- of timer 4
			IsFirstCaptured = 1;
		}
		else if(IsFirstCaptured == 1)
		{
			IC_Value2 = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_4); // captured the seconds ch- of timer4
			if(IC_Value2 > IC_Value1)
			{
				Diffrence = IC_Value2 - IC_Value1;
			}
			else if(IC_Value2 < IC_Value1)  // you know
			{
				Diffrence = ((0xFFFFFFFF - IC_Value1) + IC_Value2) + 1;
			}
			else
			{
				Error_Handler();
			}

			// Freq = TIM2 Clock / diff.
			// In my case TIM2 clock if same as PCLCK1 clock
			Frequency = HAL_RCC_GetPCLK1Freq() / Diffrence;
			IsFirstCaptured = 0u;
		}

		if(bOdoEnableFlag == TRUE)
		{
			if( ++sODO.u16OdoPulseCount >= u16SpeedoPulses)
			{
				sODO.u16OdoPulseCount = 0u;
				if( ++sODO.u8OdoFractionValue > ODO_FRACTION_VALUE )      //Incement in ODO fraction part
				{
					sODO.bOdoUpdate = TRUE;
					sODO.u8OdoFractionValue = 0u;
					//Save Odo in main when odo <= ODO_MAX_RANGE/10 otherwise Save Odo while Ign Off
					if(++sODO.u32OdoValue >= ODO_MAX_RANGE )
					{
						sODO.u32OdoValue = 0u;
					}
				}
			}

			if( ++sODO.u16TripAPulseCount >= u16SpeedoPulses)
			{
				sODO.u16TripAPulseCount = 0u;
				if( ++sODO.u8TripAFractionValue > ODO_FRACTION_VALUE )	//Incement in TRIP fraction part
				{
					sODO.u8TripAFractionValue = 0u;
					sODO.bTripAUpdate = TRUE;
					if( ++sODO.u32TripAValue > TRIPA_MAX_RANGE)         	//Check trip recycle to zero
					{
						sODO.u32TripAValue = 0u;
					}
				}
			}

			if( ++sODO.u16TripBPulseCount >= u16SpeedoPulses)
			{
				sODO.u16TripBPulseCount = 0u;
				if( ++sODO.u8TripBFractionValue > ODO_FRACTION_VALUE )	//Incement in TRIP fraction part
				{
					sODO.u8TripBFractionValue = 0u;
					sODO.bTripBUpdate = TRUE;
					if(++sODO.u32TripBValue > TRIPB_MAX_RANGE)  		//Check trip recycle to zero
					{
						sODO.u32TripBValue = 0u;
					}
				}
			}
		}//End if(bOdoEnableFlag)

		FreqCapturedFlag = TRUE;
	}
}


uint32_t GetFinalFreq(void)
{
	return Frequency;
}

void SetFinalFreq(uint32_t Val)
{
	Frequency = Val;
}

uint8_t GetFreqCapturedStatus(void)
{
	return FreqCapturedFlag;
}

void SetFreqCapturedStatus(uint8_t Status)
{
	 FreqCapturedFlag = Status;
}

void vSetAnalougeOdoEnable(uint8_t status)
{
	bOdoEnableFlag = status;
}

void vSetSpeedoPulses(uint32_t pulses)
{
	u16SpeedoPulses = pulses/10;
}



/*********************************************************************//**
 *
 * Set Odo data
 *
 *
 * @param      Odo Data
 *
 * @return     None
 *************************************************************************/
void vSetOdoValue(uint32_t Value)
{
	sODO.u32OdoValue = Value;
}

/*********************************************************************//**
 *
 * Get Odo data
 *
 *
 * @param      None
 *
 * @return     Odo Data
 *************************************************************************/
uint32_t u32GetOdoValue(void)
{
	return sODO.u32OdoValue;
}

/*********************************************************************//**
 *
 * Set Odo Fraction data
 *
 *
 * @param      Odo Fraction data
 *
 * @return     None
 *************************************************************************/
void vSetOdoFractionValue(uint8_t Value)
{
	sODO.u8OdoFractionValue = Value;

}

/*********************************************************************//**
 *
 * Get Odo Fraction data
 *
 *
 * @param      None
 *
 * @return     Odo Fraction data
 *************************************************************************/
uint8_t u8GetOdoFractionValue(void)
{
	return sODO.u8OdoFractionValue;

}

/*********************************************************************//**
 *
 * Set Odo Update Flag
 *
 *
 * @param      Odo Update Flag
 *
 * @return     None
 *************************************************************************/
void vSetOdoUpdateFlagStatus(uint8_t Status)
{
	sODO.bOdoUpdate = Status;
}


/*********************************************************************//**
 *
 * Get Odo Update Flag
 *
 *
 * @param      None
 *
 * @return     Odo Update Flag
 *************************************************************************/
uint8_t bGetOdoUpdateFlagStatus(void)
{
 return sODO.bOdoUpdate ;
}

/*********************************************************************//**
 *
 * Set Odo Fraction Update
 *
 *
 * @param      Set Odo Fraction Update
 *
 * @return     None
 *************************************************************************/
void vSetOdoFractionUpdateStatus(uint8_t Status)
{
	sODO.bFrOdoUpdate = Status;
}

/*********************************************************************//**
 *
 * Get Odo Fraction Update
 *
 *
 * @param      None
 *
 * @return     Odo Fraction Update
 *************************************************************************/
uint8_t bGetOdoFractionUpdateStatus(void)
{
	return sODO.bFrOdoUpdate;
}


/*********************************************************************//**
 *
 * Set Odo TripA Value
 *
 *
 * @param      Odo TripA Value
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripAValue(uint32_t Value)
{
	sODO.u32TripAValue = Value;
}

/*********************************************************************//**
 *
 * Get Odo TripA Value
 *
 *
 * @param      None
 *
 * @return     Odo TripA Value
 *************************************************************************/
uint32_t u32GetOdoTripAValue(void)
{
 	return sODO.u32TripAValue;
}

/*********************************************************************//**
 *
 * Set Odo TripA Fraction Value
 *
 *
 * @param      Odo TripA Fraction Value
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripAFractionValue(uint8_t Value)
{
	sODO.u8TripAFractionValue = Value;
}

/*********************************************************************//**
 *
 * Get Odo TripA Fraction Value
 *
 *
 * @param      None
 *
 * @return     Odo TripA Fraction Value
 *************************************************************************/
uint8_t u8GetOdoTripAFractionValue(void)
{
 	return sODO.u8TripAFractionValue;
}


/*********************************************************************//**
 *
 * Set Odo TripA Update
 *
 *
 * @param      TripA Update
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripAUpdateStatus(uint8_t Status)
{
	sODO.bTripAUpdate = Status;
}

/*********************************************************************//**
 *
 * Get Odo TripA Update
 *
 *
 * @param      None
 *
 * @return     Odo TripA Update
 *************************************************************************/
uint8_t bGetOdoTripAUpdateStatus(void)
{
	return sODO.bTripAUpdate;
}

/*********************************************************************//**
 *
 * Get Odo TripA Fraction Update
 *
 *
 * @param      None
 *
 * @return     Odo TripA Fraction Update
 *************************************************************************/
uint8_t bGetOdoTripAFractionUpdateStatus(void)
{
	return sODO.bFrTripAUpdate;
}

/*********************************************************************//**
 *
 * Set Odo TripA Fraction Update
 *
 *
 * @param      Odo TripA Fraction Update
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripAFractionUpdateStatus(uint8_t Status)
{
	sODO.bFrTripAUpdate = Status;
}

/*********************************************************************//**
 *
 * Set Odo TripA Pulse Counter
 *
 *
 * @param      TripA Pulse Counter
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripAPulseCounter(uint16_t Value)
{
	sODO.u16TripAPulseCount = Value;

}

/*********************************************************************//**
 *
 * Set Odo TripB Value
 *
 *
 * @param      Odo TripB Value
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripBValue(uint32_t Value)
{
	sODO.u32TripBValue = Value;
}

/*********************************************************************//**
 *
 * Get Odo TripB Value
 *
 *
 * @param      None
 *
 * @return     Odo TripB Value
 *************************************************************************/
uint32_t u32GetOdoTripBValue(void)
{
 return sODO.u32TripBValue;
}

/*********************************************************************//**
 *
 * Set TripB Fraction
 *
 *
 * @param      Odo TripB Fraction
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripBFractionValue(uint8_t Value)
{
	sODO.u8TripBFractionValue = Value;
}

/*********************************************************************//**
 *
 * Get Odo TripB Fraction Value
 *
 *
 * @param      None
 *
 * @return     Odo TripB Fraction Value
 *************************************************************************/
uint8_t u8GetOdoTripBFractionValue(void)
{
 	return sODO.u8TripBFractionValue;
}

/*********************************************************************//**
 *
 * Set Odo TripB Update
 *
 *
 * @param      Odo TripB Update
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripBUpdateStatus(uint8_t Status)
{
	sODO.bTripBUpdate = Status;
}

/*********************************************************************//**
 *
 * Get Odo TripB Update
 *
 *
 * @param      None
 *
 * @return     Odo TripB Update
 *************************************************************************/
uint8_t bGetOdoTripBUpdateStatus(void)
{
	return sODO.bTripBUpdate;
}

/*********************************************************************//**
 *
 * Get Odo TripB Fraction Update
 *
 *
 * @param      None
 *
 * @return     Odo TripB Fraction Update
 *************************************************************************/
uint8_t bGetOdoTripBFractionUpdateStatus(void)
{
	return sODO.bFrTripBUpdate;
}

/*********************************************************************//**
 *
 * Set Odo TripB Fraction Update
 *
 *
 * @param      Odo TripB Fraction Update
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripBFractionUpdateStatus(uint8_t Status)
{
	sODO.bFrTripBUpdate = Status;
}

/*********************************************************************//**
 *
 * Set Odo TripB Pulse Counter
 *
 *
 * @param      Odo TripB Pulse Counter
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripBPulseCounter(uint16_t Value)
{
	sODO.u16TripBPulseCount = Value;
}

uint32_t GetDisplayOdoFinalMergedValue(void)
{
	return ((u32GetOdoValue() * 10u) + u8GetOdoFractionValue());
}

uint32_t GetDisplayTripAFinalMergedValue(void)
{
	return ((u32GetOdoTripAValue() * 10u) + u8GetOdoTripAFractionValue());
}

uint32_t GetDisplayTripBFinalMergedValue(void)
{
	return ((u32GetOdoTripBValue() * 10u) + u8GetOdoTripBFractionValue());
}

