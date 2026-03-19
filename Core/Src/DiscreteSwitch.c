/*
 * DiscreteSwitch.c
 *
 *  Created on: Oct 10, 2024
 *      Author: dharmendra.singh
 */


#include "DiscreteSwitch.h"

// MACRO Declarations...........................................
// INPUT BUTTON TIME

#define DEBOUNCE_TIME				50

#define TIME_100MS					100         // 100 milisecs
#define TIME_2S						2000		// 2000 milisecs
#define TIME_3S						3000        // 3000 milisecs
#define TIME_5S				 		5000        // 5000 milisecs
#define TIME_10S				 	10000       // 5000 milisecs


#define	SHORT_PRESS_MIN_TIME		(uint8_t)TIME_100MS/2u
#define	SHORT_PRESS_MAX_TIME		(uint16_t)TIME_2S/2u

#define	LONG_PRESS_MIN_TIME			(uint16_t)TIME_3S/2u
#define	LONG_PRESS_MAX_TIME			(uint16_t)TIME_5S/2u

#define FAULT_TIME					(uint16_t)TIME_10S/2u


static uint8_t Get_SET_SwitchStatus(void);	// MODE KEY
static uint8_t Get_MODE_SwitchStatus(void);	// SET KEY

static uint32_t u32ModeTimeStamp = 0u;
static uint32_t u32SetTimeStamp = 0u;


/*********************************************************************//**
*
* MODE SWITCH STATUS
*
* @param 	 None
*
* @return	 None
*************************************************************************/
SW_STATE eGetMODESwitchStatus()
{
	static SW_STATE eLastState = eReleased;
	static SW_STATE eCombination = eReleased;
	uint32_t u32ElapsedTime;

	SW_STATE ePressType = eReleased;

	if(eLastState == eReleased)
	{
		u32ModeTimeStamp = u32GetClockTicks();
	}

	u32ElapsedTime = u32GetClockTicks() - u32ModeTimeStamp;

	if((Get_MODE_SwitchStatus() == ePressed) && (u32ElapsedTime < SHORT_PRESS_MAX_TIME ) && ( Get_SET_SwitchStatus() == eReleased))
	{
		eLastState = ePressed;
	}

	if((Get_MODE_SwitchStatus() == ePressed) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME ) && (Get_SET_SwitchStatus() == eReleased))
	{
		eLastState = ePressed;
		eCombination = ePressed;
		return eLongPressed;
	}

	if((Get_MODE_SwitchStatus() == ePressed && Get_SET_SwitchStatus() == ePressed ) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME))
	{
		eLastState = ePressed;
		eCombination = ePressed;
		return eModeSetLongPressed;
	}

	if( eLastState == ePressed && Get_MODE_SwitchStatus() == eReleased)
	{
		// Looking for invalid press.
		if(u32ElapsedTime <= (uint32_t)DEBOUNCE_TIME )
			ePressType = eInvalidPress;

		else if(u32ElapsedTime >  SHORT_PRESS_MIN_TIME && u32ElapsedTime <  SHORT_PRESS_MAX_TIME)
			ePressType = eShortPressed;

		else if(u32ElapsedTime >  FAULT_TIME)
			ePressType = eStuckFault;

		eCombination = eReleased;
		eLastState = eReleased;
	}

	// Press time exceeds maximum time.
	else if(Get_MODE_SwitchStatus() == ePressed && u32ElapsedTime >= LONG_PRESS_MAX_TIME)
	{
		ePressType = eStuckFault;
		eLastState = ePressed;
	}

	return ePressType;
}

/*********************************************************************//**
*
* SET SWITCH STATUS
*
* @param	  None
*
* @return	  None
*************************************************************************/
SW_STATE eGetSETSwitchStatus()
{
	static SW_STATE eLastState = eReleased;
	static SW_STATE eCombination = eReleased;
	uint32_t u32ElapsedTime;

	SW_STATE ePressType = eReleased;

	if(eLastState == eReleased)
	{
		u32SetTimeStamp = u32GetClockTicks();
	}

	u32ElapsedTime = u32GetClockTicks() - u32SetTimeStamp;

	if((Get_SET_SwitchStatus() == ePressed)
	&& (u32ElapsedTime < SHORT_PRESS_MAX_TIME ) && ( Get_MODE_SwitchStatus() == eReleased))
	{
		eLastState = ePressed;
	}

	if((Get_SET_SwitchStatus() == ePressed) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME ) && (Get_MODE_SwitchStatus() == eReleased))
	{
		eLastState = ePressed;
		eCombination = ePressed;
		return eLongPressed;
	}

	if((Get_SET_SwitchStatus() == ePressed && Get_MODE_SwitchStatus() == ePressed ) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME))
	{
		eLastState = ePressed;
		eCombination = ePressed;
		return eModeSetLongPressed;
	}

	if( eLastState == ePressed && Get_SET_SwitchStatus() == eReleased)
	{
		// Looking for invalid press.
		if(u32ElapsedTime <= (uint32_t)DEBOUNCE_TIME )
			ePressType = eInvalidPress;

		else if(u32ElapsedTime >  SHORT_PRESS_MIN_TIME && u32ElapsedTime <  SHORT_PRESS_MAX_TIME)
			ePressType = eShortPressed;

		else if(u32ElapsedTime >  FAULT_TIME)
			ePressType = eStuckFault;

		eCombination = eReleased;
		eLastState = eReleased;
	}

	// Press time exceeds maximum time.
	else if(Get_SET_SwitchStatus() == ePressed && u32ElapsedTime >= LONG_PRESS_MAX_TIME)
	{
		ePressType = eStuckFault;
		eLastState = ePressed;
	}

	return ePressType;

}



static uint8_t Get_MODE_SwitchStatus(void)
{
	uint8_t Status = FALSE;

	if(HAL_GPIO_ReadPin(GPIOA, MOD_BUTTON_SW2_Pin) == LOW)
	{
		Status = ePressed;
	}
	else
	{
		Status = eReleased;
	}

	return Status;
}

static uint8_t Get_SET_SwitchStatus(void)
{
	uint8_t Status = FALSE;

	if(HAL_GPIO_ReadPin(GPIOA, SET_BUTTON_SW1_Pin) == LOW)
	{
		Status = ePressed;
	}
	else
	{
		Status = eReleased;
	}

	return Status;
}

/*********************************************************************//**
*
* Resets all buttons press time
*
* @param 	 None
*
* @return	 None
*************************************************************************/
void vResetButtonTimeStamps()
{
	u32ModeTimeStamp = u32GetClockTicks();
	u32SetTimeStamp = u32GetClockTicks();
}


