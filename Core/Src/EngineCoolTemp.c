/*
 * EngineCoolTemp.c
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */

#include "EngineCoolTemp.h"
#include "J1939.h"


#define MAX_EngineCoolTemp_GUAGE_LVL					250u
#define EngineCoolTemp_GAUGE_SIGNAL_AVG					4u
#define EngineCoolTemp_LEVEL_GAUGE_MAX_PERCENT			100u
#define MIN_VAL_TEMP                                    0X50u
#define MAX_VAL_TEMP                                    0X96u
// CoolTemp Gauge Bar ON/OFF Macro for CAN
#define EngineCoolTemp_LEVEL_CAN_ON_BAR1				0x50 //1 Bar
#define EngineCoolTemp_LEVEL_CAN_ON_BAR2				0x64 //2 Bar
#define EngineCoolTemp_LEVEL_CAN_ON_BAR3				0x6E //3 Bar
#define EngineCoolTemp_LEVEL_CAN_ON_BAR4				0x78 //4 Bar
#define EngineCoolTemp_LEVEL_CAN_ON_BAR5				0x82 //5 Bar
#define EngineCoolTemp_LEVEL_CAN_ON_BAR6				0x8C //6 Bar
#define EngineCoolTemp_LEVEL_CAN_ON_BAR7				0x91 //7 Bar
#define EngineCoolTemp_LEVEL_CAN_ON_BAR8				0x96 //7 Bar


#define HYS_OFF											0x01

#define EngineCoolTemp_LEVEL_CAN_OFF_BAR1			EngineCoolTemp_LEVEL_CAN_ON_BAR1 - HYS_OFF
#define EngineCoolTemp_LEVEL_CAN_OFF_BAR2			EngineCoolTemp_LEVEL_CAN_ON_BAR2 - HYS_OFF
#define EngineCoolTemp_LEVEL_CAN_OFF_BAR3			EngineCoolTemp_LEVEL_CAN_ON_BAR3 - HYS_OFF
#define EngineCoolTemp_LEVEL_CAN_OFF_BAR4			EngineCoolTemp_LEVEL_CAN_ON_BAR4 - HYS_OFF
#define EngineCoolTemp_LEVEL_CAN_OFF_BAR5			EngineCoolTemp_LEVEL_CAN_ON_BAR5 - HYS_OFF
#define EngineCoolTemp_LEVEL_CAN_OFF_BAR6			EngineCoolTemp_LEVEL_CAN_ON_BAR6 - HYS_OFF
#define EngineCoolTemp_LEVEL_CAN_OFF_BAR7			EngineCoolTemp_LEVEL_CAN_ON_BAR7 - HYS_OFF
#define EngineCoolTemp_LEVEL_CAN_OFF_BAR8			EngineCoolTemp_LEVEL_CAN_ON_BAR8 - HYS_OFF

/****************************************************************************************************************/
/* Enumeration                                                                                                  */
/***************************************************************************************************************/


/****************************************************************************************************************/
/* Type EngineCoolTempinitions                                                                                             */
/****************************************************************************************************************/


/****************************************************************************************************************/
/* Local variables                                                                                              */
/****************************************************************************************************************/
 /* Set flash when CANlost ,out of range ,error data and bad data comes */
uint8_t EngineCoolTempSensorErrorFlag = FALSE;
uint8_t BuzzerStatus = FALSE;
uint8_t EngineCoolTempWarningLampFlag = FALSE;

// local declarations --------------------------------------------------------
typedef struct
{
	uint16_t    u16EngineCoolTempSignalValue;								// APG ADC value
	uint16_t    u16EngineCoolTempSignalData[EngineCoolTemp_GAUGE_SIGNAL_AVG];		// Array for averaging signal samples
	uint16_t    u16EngineCoolTempPreviousSignal;
	uint8_t     u8EngineCoolTempbarNo;
	uint8_t		u8EngineCoolTempVal;
} EngineCoolTemp_PRESSURE_DATA_TYPE;

EngineCoolTemp_PRESSURE_DATA_TYPE	sEngineCoolTempGauge = {0x00, 0x00, 0x00, 0x00, 0x00};


/****************************************************************************************************************/
/* Local function declarations                                                                                  */
/****************************************************************************************************************/
/// @addtogroup MODULE_LIBRARY
/// @{
/// @EngineCoolTempgroup ModuleLib  Private functions used
////
///
/// @addtogroup ModuleLib
/// @{




/// @} // endgroup ModuleLib

/// @} // endgroup MODULE_LIBRARY

/****************************************************************************************************************/
/* Local function EngineCoolTempinitions                                                                                   */
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
/* Global function EngineCoolTempinitions                                                                                   */
/****************************************************************************************************************/

/*************************************************************************************************************//**
 *
 * Purpose  :  Check EngineCoolTemp Level Gauge CAN msg and CAN lost msg ,According to EngineCoolTemp Level calculate bars.
 *
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 ****************************************************************************************************************/
void vEngineCoolTempLevelGauge(void)
{
	uint16_t data = 0;
	uint8_t i = 0;
	static uint8_t   AvgCount = 0;
	static uint8_t FirstEntry = TRUE;


	// Take CAN data -------------------------*/
	data = u32GetPGNdata(eJEngineCoolLevel);			/* Row data --- */

	if(bGetPGNTimeOut(eJEngineCoolLevel) || (uint16_t)data >= BADDATA)
	{
		data = 0u;
		sEngineCoolTempGauge.u8EngineCoolTempbarNo = 0u;
		EngineCoolTempSensorErrorFlag = TRUE;
		EngineCoolTempWarningLampFlag = eStatusBlink;
		sEngineCoolTempGauge.u8EngineCoolTempVal = 0u;
	}
	else
	{
		EngineCoolTempSensorErrorFlag = FALSE;

		if(data < MIN_VAL_TEMP)
		{
			data = 0;
			EngineCoolTempWarningLampFlag = eStatusOff;
			sEngineCoolTempGauge.u8EngineCoolTempVal = 0;
		}
		else if(data > MAX_VAL_TEMP)
		{
			data = MAX_VAL_TEMP;
			sEngineCoolTempGauge.u8EngineCoolTempVal = data - 40u;
		}
		else
		{
			sEngineCoolTempGauge.u8EngineCoolTempVal = data - 40u;
		}

		if(sEngineCoolTempGauge.u8EngineCoolTempVal >= 105u)
		{
			EngineCoolTempWarningLampFlag = eStatusON;
		}
		else if(sEngineCoolTempGauge.u8EngineCoolTempVal <= 100u && EngineCoolTempWarningLampFlag == eStatusON)
		{
			EngineCoolTempWarningLampFlag = eStatusOff;
		}
		else
		{
			if(EngineCoolTempWarningLampFlag == eStatusBlink)
			{
				EngineCoolTempWarningLampFlag = eStatusOff;
			}
		}
	}

	if(FirstEntry == TRUE)
	{
		for(i = 0; i < EngineCoolTemp_GAUGE_SIGNAL_AVG; i++)
		{
			sEngineCoolTempGauge.u16EngineCoolTempSignalData[i]  = data;
		}
		FirstEntry = FALSE;
	}

	sEngineCoolTempGauge.u16EngineCoolTempSignalData[AvgCount] = data;
	/* Average of ADC samples	*/
	data = 0;
	for(i = 0; i < EngineCoolTemp_GAUGE_SIGNAL_AVG; i++)
	{
		data = data + sEngineCoolTempGauge.u16EngineCoolTempSignalData[i];
	}
	data = data / EngineCoolTemp_GAUGE_SIGNAL_AVG;

	if(++AvgCount >= EngineCoolTemp_GAUGE_SIGNAL_AVG)
	{
		AvgCount = 0;
	}

	sEngineCoolTempGauge.u16EngineCoolTempSignalValue = data;//EngineCoolTempLevelAntiSlosh(data);	/* for slow increase EngineCoolTemp BARs */


	if(sEngineCoolTempGauge.u8EngineCoolTempVal <= 0u)
	{
		sEngineCoolTempGauge.u8EngineCoolTempVal = 0;
	}



	if(sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal !=  (uint16_t)sEngineCoolTempGauge.u16EngineCoolTempSignalValue)
	{
		sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal = sEngineCoolTempGauge.u16EngineCoolTempSignalValue;
		/* Ascending inputs */
		if((sEngineCoolTempGauge.u8EngineCoolTempbarNo == eEngineCoolTempLevel_0_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal < EngineCoolTemp_LEVEL_CAN_ON_BAR1))
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_0_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo <= eEngineCoolTempLevel_1_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal >= EngineCoolTemp_LEVEL_CAN_ON_BAR1) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal < EngineCoolTemp_LEVEL_CAN_ON_BAR2) )
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_1_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo <= eEngineCoolTempLevel_2_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal >= EngineCoolTemp_LEVEL_CAN_ON_BAR2) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal < EngineCoolTemp_LEVEL_CAN_ON_BAR3) )
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_2_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo <= eEngineCoolTempLevel_3_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal >= EngineCoolTemp_LEVEL_CAN_ON_BAR3) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal < EngineCoolTemp_LEVEL_CAN_ON_BAR4) )
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_3_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo <= eEngineCoolTempLevel_4_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal >= EngineCoolTemp_LEVEL_CAN_ON_BAR4) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal < EngineCoolTemp_LEVEL_CAN_ON_BAR5) )
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_4_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo <= eEngineCoolTempLevel_5_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal >= EngineCoolTemp_LEVEL_CAN_ON_BAR5) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal < EngineCoolTemp_LEVEL_CAN_ON_BAR6) )
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_5_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo <= eEngineCoolTempLevel_6_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal >= EngineCoolTemp_LEVEL_CAN_ON_BAR6) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal < EngineCoolTemp_LEVEL_CAN_ON_BAR7) )
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_6_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo <= eEngineCoolTempLevel_6_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal >= EngineCoolTemp_LEVEL_CAN_ON_BAR6) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal < EngineCoolTemp_LEVEL_CAN_ON_BAR7) )
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_6_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo <= eEngineCoolTempLevel_7_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal >= EngineCoolTemp_LEVEL_CAN_ON_BAR7) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal < EngineCoolTemp_LEVEL_CAN_ON_BAR8) )
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_7_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo <= eEngineCoolTempLevel_8_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal >= EngineCoolTemp_LEVEL_CAN_ON_BAR8 ))
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_8_Bars;
		}

		/* Descending inputs */
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo > eEngineCoolTempLevel_0_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal <= EngineCoolTemp_LEVEL_CAN_OFF_BAR1))
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_0_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo > eEngineCoolTempLevel_1_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal <= EngineCoolTemp_LEVEL_CAN_OFF_BAR2))
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_1_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo > eEngineCoolTempLevel_2_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal <= EngineCoolTemp_LEVEL_CAN_OFF_BAR3))
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_2_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo > eEngineCoolTempLevel_3_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal <= EngineCoolTemp_LEVEL_CAN_OFF_BAR4))
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_3_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo > eEngineCoolTempLevel_4_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal <= EngineCoolTemp_LEVEL_CAN_OFF_BAR5))
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_4_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo > eEngineCoolTempLevel_5_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal <= EngineCoolTemp_LEVEL_CAN_OFF_BAR6))
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_5_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo > eEngineCoolTempLevel_6_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal <= EngineCoolTemp_LEVEL_CAN_OFF_BAR7))
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_6_Bars;
		}
		else if((sEngineCoolTempGauge.u8EngineCoolTempbarNo > eEngineCoolTempLevel_7_Bars) && (sEngineCoolTempGauge.u16EngineCoolTempPreviousSignal <= EngineCoolTemp_LEVEL_CAN_OFF_BAR8))
		{
			sEngineCoolTempGauge.u8EngineCoolTempbarNo = eEngineCoolTempLevel_7_Bars;
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

	SetEngineCoolTempLevelGaugeBars(sEngineCoolTempGauge.u8EngineCoolTempbarNo);

}




/**********************************************************************************//**
 *
 * Purpose  :  Set EngineCoolTemp Level Gauge Bars
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
void SetEngineCoolTempLevelGaugeBars(uint8_t BarsValue)
{
	sEngineCoolTempGauge.u8EngineCoolTempbarNo = BarsValue;
}

/**********************************************************************************//**
 *
 * Purpose  :  Get EngineCoolTemp Level Gauge Bars
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 **************************************************************************************/
uint8_t GetEngineCoolTempLevelGaugeBars(void)
{
	return sEngineCoolTempGauge.u8EngineCoolTempbarNo;
}

uint8_t GetEngineCoolTempWarningLampStatus(void)
{
	static uint8_t Status = FALSE;
	static uint32_t u32clkTime = 0;
	static uint16_t u16CanOutTime = 0;

	switch(EngineCoolTempWarningLampFlag)
	{
		case eStatusON:
			Status = TRUE;
			BuzzerStatus = TRUE;
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
			BuzzerStatus = FALSE;
		break;
		case eStatusOff:
			Status = FALSE;
			BuzzerStatus = FALSE;
		break;
		default:
			Status = FALSE;
			BuzzerStatus = FALSE;
		break;

	}

	return Status;

}


uint8_t GetEngineCoolTempBuzzerStatus(void)
{
	return BuzzerStatus;
}

uint8_t GetEngineCoolTempValue(void)
{
	return sEngineCoolTempGauge.u8EngineCoolTempVal;
}

