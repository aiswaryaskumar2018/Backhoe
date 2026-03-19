/*
 * Screen.c
 *
 *  Created on: Oct 11, 2024
 *      Author: dharmendra.singh
 */

#include "Screen.h"
#include "DiscreteSwitch.h"
#include "extEEPROM.h"



void vSlectionHourOrOdo_Screen(void);
void vSlectionTrip_A_B_Screen(void);
void vSlection_PCodes_Screen(void);
void vSlection_Menu_Screen(void);
void vSlection_Brightness_Screen(void);




/*---------Globale varible --------------------------------*/
static uint8_t u8ModeStatus = eReleased;
static uint8_t u8SetStatus = eReleased;
static uint8_t SetTripScreen = eScreenTripA;
static uint8_t TripAResetStatus = FALSE;
static uint8_t TripBResetStatus = FALSE;
static uint8_t screenSelectionFlag = eHomeScreen;
static uint8_t menuScreenItem = eBrightnessSelectState;
static uint8_t HourOdoSelectionFlag = eOdoMeterSelect;
static uint8_t Brightness_Value = 0;
static uint8_t BrightNessScreenExitFlag = FALSE;
static uint8_t PcodesSetButtonCountForNavigate = 0;




void ScreensProcessed(void)
{
	u8ModeStatus    = eGetMODESwitchStatus();        	// Get Mode switch Status
	u8SetStatus     = eGetSETSwitchStatus();         	// Get Set switch Status

	vSlectionTrip_A_B_Screen();
	TripA_B_Reset();
	vSlection_Menu_Screen();
	vSlection_Brightness_Screen();
	vSlection_PCodes_Screen();
	vSlectionHourOrOdo_Screen();
}


void vSlectionTrip_A_B_Screen(void)
{
	if(u8ModeStatus == eShortPressed && SetTripScreen == eScreenTripA && screenSelectionFlag == eHomeScreen )
	{
		SetTripScreen = eScreenTripB;
		vResetButtonTimeStamps();
	}
	else if(u8ModeStatus == eShortPressed && SetTripScreen == eScreenTripB && screenSelectionFlag == eHomeScreen )
	{
		SetTripScreen = eScreenTripA;
		vResetButtonTimeStamps();
	}
	else
	{
		/*NA*/
	}
}

void vSlectionHourOrOdo_Screen(void)
{
	if(u8SetStatus == eShortPressed && HourOdoSelectionFlag == eOdoMeterSelect && screenSelectionFlag == eHomeScreen )
	{
		HourOdoSelectionFlag = eHourMeterSelect;
		vResetButtonTimeStamps();
	}
	else if(u8SetStatus == eShortPressed && HourOdoSelectionFlag == eHourMeterSelect && screenSelectionFlag == eHomeScreen )
	{
		HourOdoSelectionFlag = eOdoMeterSelect;
		vResetButtonTimeStamps();
	}
	else
	{
		/*NA*/
	}
}

uint8_t GetvShowHourOdoSelectOnTFT()
{

//	if(GetNgCrameHydraEngineSelectionType() == eNgCraneEngineTata74Hp || GetNgCrameHydraEngineSelectionType() == eNgCraneEngineTata101Hp
//		|| GetNgCrameHydraEngineSelectionType() == eNgCraneEngineAL130Hp)
//	{
//		return HourOdoSelectionFlag;
//	}
//	else
//	{
		return eHourMeterSelect;
//	}
}

uint8_t GetvShowTrip_A_B_DataOnTFT(void)
{
	return SetTripScreen;
}

void TripA_B_Reset(void)
{
	if(GetvShowTrip_A_B_DataOnTFT() == eScreenTripA)
	{
		if(u8ModeStatus == eLongPressed)
		{
			TripAResetStatus = TRUE;
			TripBResetStatus = FALSE;
		}
		else
		{
			TripAResetStatus = FALSE;
		}
	}
	else
	{
		if(u8ModeStatus == eLongPressed)
		{
			TripBResetStatus = TRUE;
			TripAResetStatus = FALSE;
		}
		else
		{
			TripBResetStatus = FALSE;
		}
	}
}

void vSlection_Menu_Screen(void)
{
	if(u8SetStatus == eLongPressed && screenSelectionFlag == eHomeScreen )
	{
		 screenSelectionFlag = eMenuScreen;
    }
	else if( screenSelectionFlag == eMenuScreen && u8ModeStatus == eShortPressed && menuScreenItem == eBrightnessSelectState)
	{
		screenSelectionFlag = eBrightnessScreen;
		vResetButtonTimeStamps();
	}
	else if( screenSelectionFlag == eMenuScreen && u8ModeStatus == eShortPressed && menuScreenItem == ePCodeSelectState)
	{
		screenSelectionFlag = ePCodeScreen;
		vResetButtonTimeStamps();
	}
	else if( screenSelectionFlag == eMenuScreen && u8ModeStatus == eShortPressed && menuScreenItem == eExitState)
	{
		screenSelectionFlag = eHomeScreen;
		vResetButtonTimeStamps();
	}

	if( screenSelectionFlag == eMenuScreen && u8SetStatus == eShortPressed && menuScreenItem == eBrightnessSelectState)
	{
		menuScreenItem = ePCodeSelectState;
		vResetButtonTimeStamps();
	}
	else if(screenSelectionFlag == eMenuScreen && u8SetStatus == eShortPressed && menuScreenItem == ePCodeSelectState)
	{
		menuScreenItem = eExitState;
		vResetButtonTimeStamps();
	}
	else if(screenSelectionFlag == eMenuScreen && u8SetStatus == eShortPressed && menuScreenItem == eExitState)
	{
		menuScreenItem = eBrightnessSelectState;
		vResetButtonTimeStamps();
	}
}

void vSlection_Brightness_Screen(void)
{
	static uint8_t brightness_counter = 1;

	brightness_counter = GetBrightnessBarStatus();

	if( u8SetStatus == eShortPressed && screenSelectionFlag == eBrightnessScreen && GetBrightnessScreenEnterStatus() == TRUE)
	{
		if(++brightness_counter > 10u)
		{
			brightness_counter = 1u;
		}
    }

	Brightness_Value = brightness_counter;

	if( screenSelectionFlag == eBrightnessScreen && u8ModeStatus == eShortPressed && GetBrightnessScreenEnterStatus() == TRUE)
	{
		screenSelectionFlag = eHomeScreen;
		SetBrightnessScreenEnterStatus(FALSE);
		BrightNessScreenExitFlag = TRUE;
		vResetButtonTimeStamps();
	}
}

void vSlection_PCodes_Screen(void)
{
	if( screenSelectionFlag == ePCodeScreen && u8ModeStatus == eShortPressed && GetPcodeScreenEnterStatus() == TRUE)
	{
		screenSelectionFlag = eHomeScreen;
		SetPcodeScreenEnterStatus(FALSE);
		vResetButtonTimeStamps();
	}

	if( screenSelectionFlag == ePCodeScreen && u8SetStatus == eShortPressed && GetPcodeScreenEnterStatus() == TRUE)
	{
		vResetButtonTimeStamps();
		PcodesSetButtonCountForNavigate++;
	}
}

/*****************************************************************************************/
uint8_t GetScreenStatus()
{
	return screenSelectionFlag;
}

uint8_t GetArrowStatus()
{
	return menuScreenItem;
}

uint8_t GetBrightnessBarStatus()
{
	return Brightness_Value;
}

void SetBrightnessBarStatus(uint8_t Value)
{
	Brightness_Value = Value;
}

uint8_t GetTripAResetStatus(void)
{
	return TripAResetStatus;
}

uint8_t GetTripBResetStatus(void)
{
	return TripBResetStatus;
}

uint8_t GetBrightNessScreenExitStatus(void)
{
	return BrightNessScreenExitFlag;
}

void SetBrightNessScreenExitStatus(uint8_t Status)
{
	BrightNessScreenExitFlag = Status;
}

uint8_t GetCountSetButtonForPcode(void)
{
	return PcodesSetButtonCountForNavigate;
}

void SetCountSetButtonForPcode(uint8_t Count)
{
	PcodesSetButtonCountForNavigate = Count;
}





