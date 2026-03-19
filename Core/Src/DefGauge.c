/*
 * DefGauge.c
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */

#include "DefGauge.h"
#include "J1939.h"




#define MAX_DEF_GUAGE_LVL					250u
#define Def_GAUGE_SIGNAL_AVG				4u
#define Def_LEVEL_GAUGE_MAX_PERCENT			100u

// Oil Gauge Bar ON/OFF Macro for CAN
#define Def_LEVEL_CAN_ON_BAR1				0x19 //1 Bar
#define Def_LEVEL_CAN_ON_BAR2				0x32 //2 Bar
#define Def_LEVEL_CAN_ON_BAR3				0x4B //3 Bar
#define Def_LEVEL_CAN_ON_BAR4				0x64 //4 Bar
#define Def_LEVEL_CAN_ON_BAR5				0x7D //5 Bar
#define Def_LEVEL_CAN_ON_BAR6				0x96 //6 Bar
#define Def_LEVEL_CAN_ON_BAR7				0xAF //7 Bar
#define Def_LEVEL_CAN_ON_BAR8				0xC8 //7 Bar
#define Def_LEVEL_CAN_ON_BAR9				0xE1 //7 Bar
#define Def_LEVEL_CAN_ON_BAR10				0xFA //7 Bar

#define HYS_OFF								0x01

#define Def_LEVEL_CAN_OFF_BAR1				Def_LEVEL_CAN_ON_BAR1 - HYS_OFF
#define Def_LEVEL_CAN_OFF_BAR2				Def_LEVEL_CAN_ON_BAR2 - HYS_OFF
#define Def_LEVEL_CAN_OFF_BAR3				Def_LEVEL_CAN_ON_BAR3 - HYS_OFF
#define Def_LEVEL_CAN_OFF_BAR4				Def_LEVEL_CAN_ON_BAR4 - HYS_OFF
#define Def_LEVEL_CAN_OFF_BAR5				Def_LEVEL_CAN_ON_BAR5 - HYS_OFF
#define Def_LEVEL_CAN_OFF_BAR6				Def_LEVEL_CAN_ON_BAR6 - HYS_OFF
#define Def_LEVEL_CAN_OFF_BAR7				Def_LEVEL_CAN_ON_BAR7 - HYS_OFF
#define Def_LEVEL_CAN_OFF_BAR8				Def_LEVEL_CAN_ON_BAR8 - HYS_OFF
#define Def_LEVEL_CAN_OFF_BAR9				Def_LEVEL_CAN_ON_BAR9 - HYS_OFF
#define Def_LEVEL_CAN_OFF_BAR10				Def_LEVEL_CAN_ON_BAR10 - HYS_OFF
/****************************************************************************************************************/
/* Enumeration                                                                                                  */
/***************************************************************************************************************/


/****************************************************************************************************************/
/* Type definitions                                                                                             */
/****************************************************************************************************************/


/****************************************************************************************************************/
/* Local variables                                                                                              */
/****************************************************************************************************************/
 /* Set flash when CANlost ,out of range ,error data and bad data comes */
uint8_t SensorErrorFlag = 0;



// local declarations --------------------------------------------------------
typedef struct
{
	uint16_t    u16DefSignalValue;								// APG ADC value
	uint16_t    u16DefSignalData[Def_GAUGE_SIGNAL_AVG];		// Array for averaging signal samples
	uint16_t    u16DefPreviousSignal;
	uint8_t     u8DefbarNo;
	uint8_t		u8DefPercentage;
} Def_PRESSURE_DATA_TYPE;

Def_PRESSURE_DATA_TYPE	sDefGauge = {0x00, 0x00, 0x00, 0x00, 0x00};


/****************************************************************************************************************/
/* Local function declarations                                                                                  */
/****************************************************************************************************************/
/// @addtogroup MODULE_LIBRARY
/// @{
/// @defgroup ModuleLib  Private functions used
////
///
/// @addtogroup ModuleLib
/// @{

static uint16_t DefLevelAntiSlosh(uint16_t newDefLevelPer);


/// @} // endgroup ModuleLib

/// @} // endgroup MODULE_LIBRARY

/****************************************************************************************************************/
/* Local function definitions                                                                                   */
/****************************************************************************************************************/

/*************************************************************************************************************//**
 *
 * Purpose  :
 *
 * @param[in]:  None
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/





/****************************************************************************************************************/
/* Global function definitions                                                                                   */
/****************************************************************************************************************/

/*************************************************************************************************************//**
 *
 * Purpose  :  Check Def Level Gauge CAN msg and CAN lost msg ,According to Def Level calculate bars.
 *
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 ****************************************************************************************************************/
void vDefLevelGauge(void)
{
	uint16_t data = 0;
	uint8_t i = 0;
	static uint8_t   AvgCount = 0;
	static uint8_t FirstEntry = TRUE;


	// Take CAN data -------------------------*/
	data = u32GetPGNdata(eJDefGauge);			/* Row data --- */

	if(bGetPGNTimeOut(eJDefGauge) || (uint16_t)data >= BADDATA)
	{
		data = 0u;
		sDefGauge.u8DefbarNo = 0u;
		SensorErrorFlag = TRUE;
	}
	else
	{
		SensorErrorFlag = FALSE;
	}

	if(FirstEntry == TRUE)
	{
		for(i = 0; i < 4; i++)
		{
			sDefGauge.u16DefSignalData[i]  = data;
		}
		FirstEntry = FALSE;
	}

	sDefGauge.u16DefSignalData[AvgCount] = data;
	/* Average of ADC samples	*/
	data = 0;
	for(i = 0; i < 4; i++)
	{
		data = data + sDefGauge.u16DefSignalData[i];
	}
	data = data / 4;

	if(++AvgCount >= 4)
	{
		AvgCount = 0;
	}

	sDefGauge.u16DefSignalValue = data;//DefLevelAntiSlosh(data);	/* for slow increase Def BARs */

	if(sDefGauge.u16DefPreviousSignal !=  (uint16_t)sDefGauge.u16DefSignalValue)
	{
		sDefGauge.u16DefPreviousSignal = sDefGauge.u16DefSignalValue;
		/* Ascending inputs */
		if((sDefGauge.u8DefbarNo == eDefLevel_0_Bars) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR1))
		{
			sDefGauge.u8DefbarNo = eDefLevel_0_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_1_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR1) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR2) )
		{
			sDefGauge.u8DefbarNo = eDefLevel_1_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_2_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR2) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR3) )
		{
			sDefGauge.u8DefbarNo = eDefLevel_2_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_3_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR3) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR4) )
		{
			sDefGauge.u8DefbarNo = eDefLevel_3_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_4_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR4) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR5) )
		{
			sDefGauge.u8DefbarNo = eDefLevel_4_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_5_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR5) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR6) )
		{
			sDefGauge.u8DefbarNo = eDefLevel_5_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_6_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR6) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR7) )
		{
			sDefGauge.u8DefbarNo = eDefLevel_6_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_6_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR6) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR7) )
		{
			sDefGauge.u8DefbarNo = eDefLevel_6_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_7_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR7) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR8) )
		{
			sDefGauge.u8DefbarNo = eDefLevel_7_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_8_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR8) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR9) )
		{
			sDefGauge.u8DefbarNo = eDefLevel_8_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_9_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR9) && (sDefGauge.u16DefPreviousSignal < Def_LEVEL_CAN_ON_BAR10) )
		{
			sDefGauge.u8DefbarNo = eDefLevel_9_Bars;
		}
		else if((sDefGauge.u8DefbarNo <= eDefLevel_10_Bars) && (sDefGauge.u16DefPreviousSignal >= Def_LEVEL_CAN_ON_BAR10 ))
		{
			sDefGauge.u8DefbarNo = eDefLevel_10_Bars;
		}

		/* Descending inputs */
		else if((sDefGauge.u8DefbarNo > eDefLevel_0_Bars) && (sDefGauge.u16DefPreviousSignal <= Def_LEVEL_CAN_OFF_BAR1))
		{
			sDefGauge.u8DefbarNo = eDefLevel_0_Bars;
		}
		else if((sDefGauge.u8DefbarNo > eDefLevel_1_Bars) && (sDefGauge.u16DefPreviousSignal <= Def_LEVEL_CAN_OFF_BAR2))
		{
			sDefGauge.u8DefbarNo = eDefLevel_1_Bars;
		}
		else if((sDefGauge.u8DefbarNo > eDefLevel_2_Bars) && (sDefGauge.u16DefPreviousSignal <= Def_LEVEL_CAN_OFF_BAR3))
		{
			sDefGauge.u8DefbarNo = eDefLevel_2_Bars;
		}
		else if((sDefGauge.u8DefbarNo > eDefLevel_3_Bars) && (sDefGauge.u16DefPreviousSignal <= Def_LEVEL_CAN_OFF_BAR4))
		{
			sDefGauge.u8DefbarNo = eDefLevel_3_Bars;
		}
		else if((sDefGauge.u8DefbarNo > eDefLevel_4_Bars) && (sDefGauge.u16DefPreviousSignal <= Def_LEVEL_CAN_OFF_BAR5))
		{
			sDefGauge.u8DefbarNo = eDefLevel_4_Bars;
		}
		else if((sDefGauge.u8DefbarNo > eDefLevel_5_Bars) && (sDefGauge.u16DefPreviousSignal <= Def_LEVEL_CAN_OFF_BAR6))
		{
			sDefGauge.u8DefbarNo = eDefLevel_5_Bars;
		}
		else if((sDefGauge.u8DefbarNo > eDefLevel_6_Bars) && (sDefGauge.u16DefPreviousSignal <= Def_LEVEL_CAN_OFF_BAR7))
		{
			sDefGauge.u8DefbarNo = eDefLevel_6_Bars;
		}
		else if((sDefGauge.u8DefbarNo > eDefLevel_7_Bars) && (sDefGauge.u16DefPreviousSignal <= Def_LEVEL_CAN_OFF_BAR8))
		{
			sDefGauge.u8DefbarNo = eDefLevel_7_Bars;
		}
		else if((sDefGauge.u8DefbarNo > eDefLevel_8_Bars) && (sDefGauge.u16DefPreviousSignal <= Def_LEVEL_CAN_OFF_BAR9))
		{
			sDefGauge.u8DefbarNo = eDefLevel_8_Bars;
		}
		else if((sDefGauge.u8DefbarNo > eDefLevel_9_Bars) && (sDefGauge.u16DefPreviousSignal <= Def_LEVEL_CAN_OFF_BAR10))
		{
			sDefGauge.u8DefbarNo = eDefLevel_9_Bars;
		}
		else
		{
			/*NA*/
		}
	}
	else
	{
		/*NA*/
	}

	SetDefLevelGaugeBars(sDefGauge.u8DefbarNo);
	/* Calculate microsteps for drive stepper of Def gauge ---------------*/

}


/*************************************************************************************************************//**
 *
 * Purpose  :  DefLevelAntiSlosh, This function used for Def percentage increase slow 25 ms,for Bar slow display
 *
 * @param[in]   :Raw value of Def input from CAN
 * @param[out]:	None
 * @return    : Slow Def percentage
 ****************************************************************************************************************/
static uint16_t DefLevelAntiSlosh(uint16_t newDefLevelPer)
{
	uint16_t tempDefLevel1;
	uint16_t DampingTime = 0U;
	uint8_t  DampingSize = 0U;

	static uint8_t firstEntry = 1; // Must keep true
	static uint16_t oldDefLevelPer = 0;
	static uint32_t calcUpdateRate =0;

	if( SensorErrorFlag == TRUE )
	{
		newDefLevelPer = 0U;
		oldDefLevelPer = 0U;
		/* this will be used to get the fule immediately to initial position */
		firstEntry = 1;
		calcUpdateRate = u32GetClockTicks();
	}

	if(oldDefLevelPer >= newDefLevelPer )
	{
		tempDefLevel1 = oldDefLevelPer - newDefLevelPer;
	}
	else
	{
		tempDefLevel1 = newDefLevelPer - oldDefLevelPer;
	}

	DampingSize = 1;
	DampingTime = 22; //in ms for time achive 2.5 seconds 0 to full bars

	if( ((u32GetClockTicks() - calcUpdateRate) > DampingTime) )
	{
		if( tempDefLevel1 >= DampingSize)
		{
			if( oldDefLevelPer >= newDefLevelPer )
			{
				oldDefLevelPer = oldDefLevelPer - DampingSize;
			}
			else
			{
				oldDefLevelPer = oldDefLevelPer + DampingSize;
			}
		}
		calcUpdateRate = u32GetClockTicks();
	}

	if (firstEntry == 1)
	{
		oldDefLevelPer = newDefLevelPer;
	}
	firstEntry = 0U;
	if(oldDefLevelPer > Def_LEVEL_GAUGE_MAX_PERCENT)
	{
		oldDefLevelPer = Def_LEVEL_GAUGE_MAX_PERCENT;
	}

	return oldDefLevelPer;

}


/**********************************************************************************//**
 *
 * Purpose  :  Set Def Level Gauge Bars
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
void SetDefLevelGaugeBars(uint8_t BarsValue)
{
	sDefGauge.u8DefbarNo = BarsValue;
}

/**********************************************************************************//**
 *
 * Purpose  :  Get Def Level Gauge Bars
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
uint8_t GetDefLevelGaugeBars(void)
{
	return sDefGauge.u8DefbarNo;
}

/**********************************************************************************//**
 *
 * Purpose  :  Set Def Level Gauge Percentage
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
void SetDefLevelGaugePercentage(uint8_t PercentageValue)
{
	sDefGauge.u8DefPercentage = PercentageValue;
}

/**********************************************************************************//**
 *
 * Purpose  :  Get Def Level Gauge Percentage
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
uint8_t GetDefLevelGaugePercentage(void)
{
	return sDefGauge.u8DefPercentage;
}
