/*
 * EngineOilPress.c
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */


#include "EngineOilPress.h"
#include "J1939.h"
#include "main.h"


#define MAX_EngineOil_GUAGE_LVL					250u
#define EngineOil_GAUGE_SIGNAL_AVG				4u
#define EngineOil_LEVEL_GAUGE_MAX_PERCENT		100u
#define ENG_OIL_WARNING_ON_VAL					0x25/* in KPA 150 kpa*/
#define ENG_OIL_WARNING_OFF_VAL					0x3E/* in KPA 250 kpa*/

// Oil Gauge Bar ON/OFF Macro for CAN
#define EngineOil_LEVEL_CAN_ON_BAR1				0x19 //1 Bar
#define EngineOil_LEVEL_CAN_ON_BAR2				0x32 //2 Bar
#define EngineOil_LEVEL_CAN_ON_BAR3				0x4B //3 Bar
#define EngineOil_LEVEL_CAN_ON_BAR4				0x64 //4 Bar
#define EngineOil_LEVEL_CAN_ON_BAR5				0x7D //5 Bar
#define EngineOil_LEVEL_CAN_ON_BAR6				0x96 //6 Bar
#define EngineOil_LEVEL_CAN_ON_BAR7				0xAF //7 Bar
#define EngineOil_LEVEL_CAN_ON_BAR8				0xC8 //7 Bar
#define EngineOil_LEVEL_CAN_ON_BAR9				0xE1 //7 Bar
#define EngineOil_LEVEL_CAN_ON_BAR10			0xFA //7 Bar

#define HYS_OFF									0x01

#define EngineOil_LEVEL_CAN_OFF_BAR1			EngineOil_LEVEL_CAN_ON_BAR1 - HYS_OFF
#define EngineOil_LEVEL_CAN_OFF_BAR2			EngineOil_LEVEL_CAN_ON_BAR2 - HYS_OFF
#define EngineOil_LEVEL_CAN_OFF_BAR3			EngineOil_LEVEL_CAN_ON_BAR3 - HYS_OFF
#define EngineOil_LEVEL_CAN_OFF_BAR4			EngineOil_LEVEL_CAN_ON_BAR4 - HYS_OFF
#define EngineOil_LEVEL_CAN_OFF_BAR5			EngineOil_LEVEL_CAN_ON_BAR5 - HYS_OFF
#define EngineOil_LEVEL_CAN_OFF_BAR6			EngineOil_LEVEL_CAN_ON_BAR6 - HYS_OFF
#define EngineOil_LEVEL_CAN_OFF_BAR7			EngineOil_LEVEL_CAN_ON_BAR7 - HYS_OFF
#define EngineOil_LEVEL_CAN_OFF_BAR8			EngineOil_LEVEL_CAN_ON_BAR8 - HYS_OFF
#define EngineOil_LEVEL_CAN_OFF_BAR9			EngineOil_LEVEL_CAN_ON_BAR9 - HYS_OFF
#define EngineOil_LEVEL_CAN_OFF_BAR10			EngineOil_LEVEL_CAN_ON_BAR10 - HYS_OFF
/****************************************************************************************************************/
/* Enumeration                                                                                                  */
/***************************************************************************************************************/


/****************************************************************************************************************/
/* Type EngineOilinitions                                                                                             */
/****************************************************************************************************************/


/****************************************************************************************************************/
/* Local variables                                                                                              */
/****************************************************************************************************************/
 /* Set flash when CANlost ,out of range ,error data and bad data comes */
uint8_t EngineOILSensorErrorFlag = FALSE;
uint8_t EngineOILWarningLampFlag = FALSE;


// local declarations --------------------------------------------------------
typedef struct
{
	uint16_t    u16EngineOilSignalValue;								// APG ADC value
	uint16_t    u16EngineOilSignalData[EngineOil_GAUGE_SIGNAL_AVG];		// Array for averaging signal samples
	uint16_t    u16EngineOilPreviousSignal;
	uint8_t     u8EngineOilbarNo;
	uint8_t		u8EngineOilPercentage;
} EngineOil_PRESSURE_DATA_TYPE;

EngineOil_PRESSURE_DATA_TYPE	sEngineOilGauge = {0x00, 0x00, 0x00, 0x00, 0x00};


/****************************************************************************************************************/
/* Local function declarations                                                                                  */
/****************************************************************************************************************/
/// @addtogroup MODULE_LIBRARY
/// @{
/// @EngineOilgroup ModuleLib  Private functions used
////
///
/// @addtogroup ModuleLib
/// @{




/// @} // endgroup ModuleLib

/// @} // endgroup MODULE_LIBRARY

/****************************************************************************************************************/
/* Local function EngineOilinitions                                                                                   */
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
/* Global function EngineOilinitions                                                                                   */
/****************************************************************************************************************/

/*************************************************************************************************************//**
 *
 * Purpose  :  Check EngineOil Level Gauge CAN msg and CAN lost msg ,According to EngineOil Level calculate bars.
 *
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 ****************************************************************************************************************/
void vEngineOilLevelGauge(void)
{
	uint16_t data = 0;
	uint8_t i = 0;
	static uint8_t   AvgCount = 0;
	static uint8_t FirstEntry = TRUE;


	data = u32GetPGNdata(eEngineOilPressureGauge);  // Read raw data

	// 1. Timeout or Bad Data Check
	if (bGetPGNTimeOut(eEngineOilPressureGauge) || (uint16_t)data >= BADDATA)
	{
	    data = 0u;
	    sEngineOilGauge.u8EngineOilbarNo = 0u;
	    EngineOILSensorErrorFlag = TRUE;
	    EngineOILWarningLampFlag = eStatusBlink;
	}
	else
	{
	    // 2. Valid data received → clear sensor error & update LED properly
	    EngineOILSensorErrorFlag = FALSE;

	    if (data <= ENG_OIL_WARNING_ON_VAL)	/* 150 kpa */
	    {
	        EngineOILWarningLampFlag = eStatusON;
	    }
	    else if (data >= ENG_OIL_WARNING_OFF_VAL) /* 250 kpa */
	    {
	        EngineOILWarningLampFlag = eStatusOff;
	    }
	    else
	    {
	        // Mid-range data → retain previous state, BUT ensure it's not blinking
	        if (EngineOILWarningLampFlag == eStatusBlink)
	        {
	            EngineOILWarningLampFlag = eStatusOff;  // or maintain previous ON/OFF state if stored
	        }
	    }
	}

	if(FirstEntry == TRUE)
	{
		for(i = 0; i < EngineOil_GAUGE_SIGNAL_AVG; i++)
		{
			sEngineOilGauge.u16EngineOilSignalData[i]  = data;
		}
		FirstEntry = FALSE;
	}

	sEngineOilGauge.u16EngineOilSignalData[AvgCount] = data;
	/* Average of ADC samples	*/
	data = 0;
	for(i = 0; i < EngineOil_GAUGE_SIGNAL_AVG; i++)
	{
		data = data + sEngineOilGauge.u16EngineOilSignalData[i];
	}
	data = data / EngineOil_GAUGE_SIGNAL_AVG;

	if(++AvgCount >= EngineOil_GAUGE_SIGNAL_AVG)
	{
		AvgCount = 0;
	}

	sEngineOilGauge.u16EngineOilSignalValue = data;//EngineOilLevelAntiSlosh(data);	/* for slow increase EngineOil BARs */

	if(sEngineOilGauge.u16EngineOilPreviousSignal !=  (uint16_t)sEngineOilGauge.u16EngineOilSignalValue)
	{
		sEngineOilGauge.u16EngineOilPreviousSignal = sEngineOilGauge.u16EngineOilSignalValue;
		/* Ascending inputs */
		if((sEngineOilGauge.u8EngineOilbarNo == eEngineOilLevel_0_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR1))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_0_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_1_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR1) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR2) )
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_1_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_2_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR2) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR3) )
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_2_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_3_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR3) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR4) )
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_3_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_4_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR4) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR5) )
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_4_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_5_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR5) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR6) )
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_5_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_6_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR6) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR7) )
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_6_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_6_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR6) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR7) )
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_6_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_7_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR7) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR8) )
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_7_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_8_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR8) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR9) )
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_8_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_9_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR9) && (sEngineOilGauge.u16EngineOilPreviousSignal < EngineOil_LEVEL_CAN_ON_BAR10) )
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_9_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo <= eEngineOilLevel_10_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal >= EngineOil_LEVEL_CAN_ON_BAR10 ))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_10_Bars;
		}

		/* Descending inputs */
		else if((sEngineOilGauge.u8EngineOilbarNo > eEngineOilLevel_0_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal <= EngineOil_LEVEL_CAN_OFF_BAR1))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_0_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo > eEngineOilLevel_1_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal <= EngineOil_LEVEL_CAN_OFF_BAR2))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_1_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo > eEngineOilLevel_2_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal <= EngineOil_LEVEL_CAN_OFF_BAR3))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_2_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo > eEngineOilLevel_3_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal <= EngineOil_LEVEL_CAN_OFF_BAR4))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_3_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo > eEngineOilLevel_4_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal <= EngineOil_LEVEL_CAN_OFF_BAR5))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_4_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo > eEngineOilLevel_5_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal <= EngineOil_LEVEL_CAN_OFF_BAR6))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_5_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo > eEngineOilLevel_6_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal <= EngineOil_LEVEL_CAN_OFF_BAR7))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_6_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo > eEngineOilLevel_7_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal <= EngineOil_LEVEL_CAN_OFF_BAR8))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_7_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo > eEngineOilLevel_8_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal <= EngineOil_LEVEL_CAN_OFF_BAR9))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_8_Bars;
		}
		else if((sEngineOilGauge.u8EngineOilbarNo > eEngineOilLevel_9_Bars) && (sEngineOilGauge.u16EngineOilPreviousSignal <= EngineOil_LEVEL_CAN_OFF_BAR10))
		{
			sEngineOilGauge.u8EngineOilbarNo = eEngineOilLevel_9_Bars;
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

	SetEngineOilLevelGaugeBars(sEngineOilGauge.u8EngineOilbarNo);

	/* Calculate microsteps for drive stepper of EngineOil gauge ---------------*/

}





/**********************************************************************************//**
 *
 * Purpose  :  Set EngineOil Level Gauge Bars
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
void SetEngineOilLevelGaugeBars(uint8_t BarsValue)
{
	sEngineOilGauge.u8EngineOilbarNo = BarsValue;
}

/**********************************************************************************//**
 *
 * Purpose  :  Get EngineOil Level Gauge Bars
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
uint8_t GetEngineOilLevelGaugeBars(void)
{
	return sEngineOilGauge.u8EngineOilbarNo;
}

/**********************************************************************************//**
 *
 * Purpose  :  Set EngineOil Level Gauge Percentage
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
void SetEngineOilLevelGaugePercentage(uint8_t PercentageValue)
{
	sEngineOilGauge.u8EngineOilPercentage = PercentageValue;
}

/**********************************************************************************//**
 *
 * Purpose  :  Get EngineOil Level Gauge Percentage
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
uint8_t GetEngineOilLevelGaugePercentage(void)
{
	return sEngineOilGauge.u8EngineOilPercentage;
}

uint8_t GetEngineOilPressWarningLampStatus(void)
{
	static uint8_t Status = FALSE;
	static uint32_t u32clkTime = 0;
	static uint16_t u16CanOutTime = 0;

	switch(EngineOILWarningLampFlag)
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
					u16CanOutTime = INDICATOR_ON_TIME;
				}
				else
				{
					u16CanOutTime = INDICATOR_OFF_TIME;
				}
			}
		break;
		case eStatusOff:
			Status = FALSE;
		break;
		default:
			Status = FALSE;
		break;

	}

	return Status;

}


