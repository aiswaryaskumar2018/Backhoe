/* CA78K0R C Source Converter V1.00.03.01 [10 May 2018] */
/*****************************************************************************
 DISCLAIMER
 This software is supplied by Renesas Electronics Corporation and is only
 intended for use with Renesas products. No other uses are authorized. This
 software is owned by Renesas Electronics Corporation and is protected under
 all applicable laws, including copyright laws.
 THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 Renesas reserves the right, without notice, to make changes to this software
 and to discontinue the availability of this software. By using this software,
 you agree to the additional terms and conditions found by accessing the
 following link:
 http://www.renesas.com/disclaimer
 Copyright (C) 2016-2018 Renesas Electronics Corporation. All rights reserved.
******************************************************************************/

#include "MuxDemux.h"
#include "TellTales.h"
#include "ledDriver5716.h"
#include "FuelGauze.h"
#include "EngineCoolTemp.h"
#include "J1939.h"
#include "AirPressureGauge.h"
#include "TransOilTemp.h"


// Pins declarations -----------------------------------------------------

// constant declarations -----------------------------------------------------
#define INTERVAL_TIME_250				250 //ms
#define INTERVAL_TIME_500				500 //ms
#define TIME_HYS						100 //ms
#define BLINK_RATE						250		//Time in ms				
#define INPUT_DEBOUNCE_TIME				200		//Time in ms
#define FUEL_WARNING_ON_TIME			1000	// in ms
#define FUEL_WARNING_OFF_TIME			3000	//in ms
#define TURN_LEFT_RIGHT_FLASH_TIME		500		//in ms
#define INDICATOR_FLASHING_TIME			500		//in ms
#define SEAT_BELT_BUZZER_TIME 			30000 	// in ms
#define HYSTERESIS						50		//RPM
#define ENGINE_RPM						200
#define UP_DOWN_SHIFT_LAMP_BUZZER_ON	200		//In ms
#define UP_DOWN_SHIFT_LAMP_BUZZER_OFF	10000	//In ms
#define TELL_TELL_REFRESH_TIME			10		//in ms
#define MIL_PGN5080_SENSE_TIME 			10000



//****************************************************************************************
#define SPARE5_ON			   { tlc6c5816_data[1] = tlc6c5816_data[1] | 0x01;}
#define SPARE5_OFF		       { tlc6c5816_data[1] = tlc6c5816_data[1] & 0xFE;}

#define SPARE6_ON			   { tlc6c5816_data[1] = tlc6c5816_data[1] | 0x02;}
#define SPARE6_OFF			   { tlc6c5816_data[1] = tlc6c5816_data[1] & 0xFD;}

#define SPARE7_ON			   { tlc6c5816_data[1] = tlc6c5816_data[1] | 0x04;}
#define SPARE7_OFF		       { tlc6c5816_data[1] = tlc6c5816_data[1] & 0xFB;}

#define SPARE8_ON			   { tlc6c5816_data[1] = tlc6c5816_data[1] | 0x08;}
#define SPARE8_OFF		       { tlc6c5816_data[1] = tlc6c5816_data[1] & 0xF7;}

#define SPARE9_ON			   { tlc6c5816_data[1] = tlc6c5816_data[1] | 0x10;}
#define SPARE9_OFF		       { tlc6c5816_data[1] = tlc6c5816_data[1] & 0xEF;}

#define SPARE10_ON			   { tlc6c5816_data[1] = tlc6c5816_data[1] | 0x20;}
#define SPARE10_OFF		       { tlc6c5816_data[1] = tlc6c5816_data[1] & 0xDF;}

#define SPARE11_ON			  { tlc6c5816_data[1] = tlc6c5816_data[1] | 0x40;}
#define SPARE11_OFF			  { tlc6c5816_data[1] = tlc6c5816_data[1] & 0xBF;}

#define SPARE12_ON			  { tlc6c5816_data[1] = tlc6c5816_data[1] | 0x80;}
#define PSPARE12_OFF		  { tlc6c5816_data[1] = tlc6c5816_data[1] & 0x7F;}


/*******************************************************************************************************/
#define BRAKE_AIR_LED_ON			   { tlc6c5816_data[2] = tlc6c5816_data[2] | 0x01;}
#define BRAKE_AIR_LED_OFF		       { tlc6c5816_data[2] = tlc6c5816_data[2] & 0xFE;}

#define TRANS_OIL_TEMP_ON			   { tlc6c5816_data[2] = tlc6c5816_data[2] | 0x02;}
#define TRANS_OIL_TEMP_OFF		       { tlc6c5816_data[2] = tlc6c5816_data[2] & 0xFD;}

#define PARKING_LIGHT_ON		{ tlc6c5816_data[2] = tlc6c5816_data[2] | 0x04;}
#define PARKING_LIGHT_OFF		{ tlc6c5816_data[2] = tlc6c5816_data[2] & 0xFB;}

#define GLOW_PLUG_ON			       { tlc6c5816_data[2] = tlc6c5816_data[2] | 0x08;}
#define GLOW_PLUG_OFF		       { tlc6c5816_data[2] = tlc6c5816_data[2] & 0xF7;}

#define SPARE1_ON			   { tlc6c5816_data[2] = tlc6c5816_data[2] | 0x10;}
#define SPARE1_OFF		       { tlc6c5816_data[2] = tlc6c5816_data[2] & 0xEF;}

#define SPARE2_ON			   { tlc6c5816_data[2] = tlc6c5816_data[2] | 0x20;}
#define SPARE2_OFF		       { tlc6c5816_data[2] = tlc6c5816_data[2] & 0xDF;}

#define SPARE3_ON		{ tlc6c5816_data[2] = tlc6c5816_data[2] | 0x40;}
#define SPARE3_OFF		{ tlc6c5816_data[2] = tlc6c5816_data[2] & 0xBF;}

#define SPARE4_ON			       { tlc6c5816_data[2] = tlc6c5816_data[2] | 0x80;}
#define SPARE4_OFF		       { tlc6c5816_data[2] = tlc6c5816_data[2] & 0x7F;}


//****************************************************************************************
#define FUEL_GAUGE_ON			   { tlc6c5816_data[4] = tlc6c5816_data[4] | 0x01;}
#define FUEL_GAUGE_OFF		       { tlc6c5816_data[4] = tlc6c5816_data[4] & 0xFE;}

#define MIL_LAMP_ON				{ tlc6c5816_data[4] = tlc6c5816_data[4] | 0x02;}
#define MIL_LAMP_OFF				{ tlc6c5816_data[4] = tlc6c5816_data[4] & 0xFD;}

#define SYSTEM_OK_LAMP_ON			{ tlc6c5816_data[4] = tlc6c5816_data[4] | 0x04;}
#define SYSTEM_OK_LAMP_OFF		    { tlc6c5816_data[4] = tlc6c5816_data[4] & 0xFB;}

#define HEST_LAMP_ON			   { tlc6c5816_data[4] = tlc6c5816_data[4] | 0x08;}
#define HEST_LAMP_OFF		       { tlc6c5816_data[4] = tlc6c5816_data[4] & 0xF7;}

#define ENGINE_OIL_PRESS_ON			   { tlc6c5816_data[4] = tlc6c5816_data[4] | 0x10;}
#define ENGINE_OIL_PRESS_OFF		       { tlc6c5816_data[4] = tlc6c5816_data[4] & 0xEF;}

#define PARKING_BRAKE_ON			   { tlc6c5816_data[4] = tlc6c5816_data[4] | 0x20;}
#define PARKING_BRAKE_OFF		       { tlc6c5816_data[4] = tlc6c5816_data[4] & 0xDF;}

#define WATER_IN_FUEL_ON		{ tlc6c5816_data[4] = tlc6c5816_data[4] | 0x40;}
#define WATER_IN_FUEL_OFF		{ tlc6c5816_data[4] = tlc6c5816_data[4] & 0xBF;}

#define POOR_DEF_QUALITY_ON			       { tlc6c5816_data[4] = tlc6c5816_data[4] | 0x80;}
#define POOR_DEF_QUALITY_OFF		       { tlc6c5816_data[4] = tlc6c5816_data[4] & 0x7F;}


/*******************************************************************************************************/
#define FUEL_GAUGE_LED1_ON			   { tlc6c5816_data[5] = tlc6c5816_data[5] | 0x01;}
#define FUEL_GAUGE_LED1_OFF		       { tlc6c5816_data[5] = tlc6c5816_data[5] & 0xFE;}

#define FUEL_GAUGE_LED2_ON			   { tlc6c5816_data[5] = tlc6c5816_data[5] | 0x02;}
#define FUEL_GAUGE_LED2_OFF		       { tlc6c5816_data[5] = tlc6c5816_data[5] & 0xFD;}

#define FUEL_GAUGE_LED3_ON		{ tlc6c5816_data[5] = tlc6c5816_data[5] | 0x04;}
#define FUEL_GAUGE_LED3_OFF		{ tlc6c5816_data[5] = tlc6c5816_data[5] & 0xFB;}

#define FUEL_GAUGE_LED4_ON			       { tlc6c5816_data[5] = tlc6c5816_data[5] | 0x08;}
#define FUEL_GAUGE_LED4_OFF		       { tlc6c5816_data[5] = tlc6c5816_data[5] & 0xF7;}

#define FUEL_GAUGE_LED5_ON			   { tlc6c5816_data[5] = tlc6c5816_data[5] | 0x10;}
#define FUEL_GAUGE_LED5_OFF		       { tlc6c5816_data[5] = tlc6c5816_data[5] & 0xEF;}

#define FUEL_GAUGE_LED6_ON			   { tlc6c5816_data[5] = tlc6c5816_data[5] | 0x20;}
#define FUEL_GAUGE_LED6_OFF		       { tlc6c5816_data[5] = tlc6c5816_data[5] & 0xDF;}

#define FUEL_GAUGE_LED7_ON		{ tlc6c5816_data[5] = tlc6c5816_data[5] | 0x40;}
#define FUEL_GAUGE_LED7_OFF		{ tlc6c5816_data[5] = tlc6c5816_data[5] & 0xBF;}

#define FUEL_GAUGE_LED8_ON			       { tlc6c5816_data[5] = tlc6c5816_data[5] | 0x80;}
#define FUEL_GAUGE_LED8_OFF		       { tlc6c5816_data[5] = tlc6c5816_data[5] & 0x7F;}

/*******************************************************************************************/
#define COOLENT_TEMP_LED1_ON			   { tlc6c5816_data[7] = tlc6c5816_data[7] | 0x01;}
#define COOLENT_TEMP_LED1_OFF		       { tlc6c5816_data[7] = tlc6c5816_data[7] & 0xFE;}

#define COOLENT_TEMP_LED2_ON		{ tlc6c5816_data[7] = tlc6c5816_data[7] | 0x02;}
#define COOLENT_TEMP_LED2_OFF		{ tlc6c5816_data[7] = tlc6c5816_data[7] & 0xFD;}

#define COOLENT_TEMP_LED3_ON			       { tlc6c5816_data[7] = tlc6c5816_data[7] | 0x04;}
#define COOLENT_TEMP_LED3_OFF		       { tlc6c5816_data[7] = tlc6c5816_data[7] & 0xFB;}

#define COOLENT_TEMP_LED4_ON			   { tlc6c5816_data[7] = tlc6c5816_data[7] | 0x08;}
#define COOLENT_TEMP_LED4_OFF		       { tlc6c5816_data[7] = tlc6c5816_data[7] & 0xF7;}

#define COOLENT_TEMP_LED5_ON			   { tlc6c5816_data[7] = tlc6c5816_data[7] | 0x10;}
#define COOLENT_TEMP_LED5_OFF		       { tlc6c5816_data[7] = tlc6c5816_data[7] & 0xEF;}

#define COOLENT_TEMP_LED6_ON		{ tlc6c5816_data[7] = tlc6c5816_data[7] | 0x20;}
#define COOLENT_TEMP_LED6_OFF		{ tlc6c5816_data[7] = tlc6c5816_data[7] & 0xDF;}

#define COOLENT_TEMP_LED7_ON			       { tlc6c5816_data[7] = tlc6c5816_data[7] | 0x40;}
#define COOLENT_TEMP_LED7_OFF		       { tlc6c5816_data[7] = tlc6c5816_data[7] & 0xBF;}

#define COOLENT_TEMP_LED8_ON			   { tlc6c5816_data[7] = tlc6c5816_data[7] | 0x80;}
#define COOLENT_TEMP_LED8_OFF		       { tlc6c5816_data[7] = tlc6c5816_data[7] & 0x7F;}

//******************************************************************************************
#define RIGHT_TURN_ON			   { tlc6c5816_data[8] = tlc6c5816_data[8] | 0x01;}
#define RIGHT_TURN_OFF		       { tlc6c5816_data[8] = tlc6c5816_data[8] & 0xFE;}

#define NETURAL_ON			   { tlc6c5816_data[8] = tlc6c5816_data[8] | 0x02;}
#define NETURAL_OFF		       { tlc6c5816_data[8] = tlc6c5816_data[8] & 0xFD;}

#define HIGH_BEAM_ON		{ tlc6c5816_data[8] = tlc6c5816_data[8] | 0x04;}
#define HIGH_BEAM_OFF		{ tlc6c5816_data[8] = tlc6c5816_data[8] & 0xFB;}

#define LEFT_TURN_ON			   		{ tlc6c5816_data[8] = tlc6c5816_data[8] | 0x08;}
#define LEFT_TURN_OFF		        { tlc6c5816_data[8] = tlc6c5816_data[8] & 0xF7;}

#define ENGINE_STOP_ON		{ tlc6c5816_data[8] = tlc6c5816_data[8] | 0x10;}
#define ENGINE_STOP_OFF		{ tlc6c5816_data[8] = tlc6c5816_data[8] & 0xEF;}

#define BATT_CHARG_ON			{ tlc6c5816_data[8] = tlc6c5816_data[8] | 0x20;}
#define BATT_CHARG_OFF		{ tlc6c5816_data[8] = tlc6c5816_data[8] & 0xDF;}

#define ENGINE_COLANR_LEVEL_ON			        { tlc6c5816_data[8] = tlc6c5816_data[8] | 0x40;}
#define ENGINE_COLANR_LEVEL_OFF		       		{ tlc6c5816_data[8] = tlc6c5816_data[8] & 0xBF;}

#define DPF_REGE_INHIBIT_ON		{ tlc6c5816_data[8] = tlc6c5816_data[8] | 0x80;}
#define DPF_REGE_INHIBIT_OFF		{ tlc6c5816_data[8] = tlc6c5816_data[8] & 0x7F;}


//---------------------------------------------------------------------------------------------------



//****************************************************************************************
#define NO_SERVICE_REMINDER				3

typedef enum
{
  eHORIZONTAL_AXIS_LOCK1_LOW_MUX_INPUT = 0,
  eDIFFERENTIAL_LOCK_REAR_AXLE_HIGH_MUX_INPUT,
  eDIFFERENTIAL_LOCK_REAR_AXLE_LOW_MUX_INPUT,
  eMAIN_BEAM_MUX_INPUT,
  eESC_MALFUNCTION_MUX_INPUT,
  eDIFF_LOCK_LOW_MUX_INPUT,
  eDIFF_LOCK_HIGH_MUX_INPUT,
  eENGINE_WARNING_ADD_MUX_INPUT,
  eHIGH_GEAR_MUX_INPUT,
  eEXAUST_BREAK_MUX_INPUT,
  eHIGH_BEAM_MUX_INPUT,
  eSEAT_BELT_MUX_INPUT,
  eWATER_IN_FUEL_MUX_INPUT,
  eWAIT_TO_START_MUX_INPUT,
  eLOW_BEAM_LIGHT_MUX_INPUT,
  eHORIZONTAL_AXIS_LOCK1_HIGH_MUX_INPUT,
  
  eYELLOW_WARNING_LIGHT_MUX_INPUT,
  eLINING_PAD_MUX_INPUT,
  eBREAK_LIQUID_LEVEL_MUX_INPUT,
  eHORIZONTAL_AXIS_LOCK2_LOW_MUX_INPUT,
  eHORIZONTAL_AXIS_LOCK2_HIGH_MUX_INPUT,
  eREAR_FOG_LIGHT_HIGH_MUX_INPUT,
  ePTO_MUX_INPUT,
  eREAR_FOG_LIGHT_LOW_MUX_INPUT,
  eLIM_NOX_MUX_INPUT,
  eTURN_LEFT_INPUT,
  eTURN_RIGHT_INPUT,
  ePARK_BRAKE_INPUT,
  eCAB_TILT_INPUT,
  eSTOP_ENGINE_MAL_INPUT,
  eFRONT_FOG_INPUT,
  eDOOR_LOW_INPUT,
  eRETARDER_HIGH_INPUT,
  eMAIN_BEAM_INPUT,
  eCLUTCH_OIL_LEVEL_INPUT,
  eENGINE_COOLANT_LEVEL_INPUT,
  eBATT_CHARGING1_INPUT,
  eBATT_CHARGING2_INPUT,
  eRED_STOP_LAMP,
  eENGINE_OIL_LEVEL,
  eABS_FAILURE,
  eREVERSE_LIGHT,
  eGEN_ILLUMINATION,
  eLOW_GEAR,
  eCRUISE_CONTROL_INPUT,
  eBRAKE_AIR_COMPRESSOR_LOW_INPUT,
  eBRAKE_AIR_COMPRESSOR_HIGH_INPUT,
  eADBLUE_DEF_INPUT,
  eNUM_OF_INPUTS
};
typedef enum
{                      
	
	eNUM_OF_MUX_INPUTS
} eMUX;


typedef enum
{                       // bit data
	eTurnLampOff = 0,      	// 000
	eTurnRight,             // 001
	eTurnLeft,            	// 010
	eTurnLeftDoubleFlag,    // 011
	eTurnRightDoubleFlag,   // 100
	eHazard,	    		// 101
	eReserve1,	    		// 110
	eReserve2,   	    	// 111
} eTurnLampdataType3bit;


typedef enum
{                       // bit data
	eLedOff = 0,        // 000
	eLedOn,             // 001
	eError2,            // 010
	eError3,            // 011
	eLampFastBlink,     // 100
	eError5,	    	// 101
	eError6,	    	// 110
	eDataNA   	    	// 111
} eOBDLampdataType3bit;
	
typedef enum
{                       // bit data
	eLampOff = 0,       // 00
	eLampOn,            // 01
	eError,             // 10
	eDataNotAvailable   // 11
} eOBDLampdataType;

	

typedef enum
{						
	eBothLampOff = 0,		
	eLeftTurnFlash = 1, 		
	eRightTurnFlash = 2,			
	eRev = 3,	
	eErrorBothFlash = 14,
} eTurnLampType;


//Type of Buzzer sound
typedef enum
{
	eLifeCriticalAlarm = 0,
	eEngineCriticalAlarm,
	eGeneralAlarm,
	eTicAlarm, 
	eTocAlarm,
	eBeepAlarm,
	eContinueAlarm, 
	eIrritateAlarm,
	eStopAlarm, 
 } eBuzzerStateType;
	

 uint8_t LowFeuelStatus = FALSE;
 uint8_t ParkBrakeLampBuzzStatus = FALSE;

static uint32_t AlarmTimeStamp = 0;//Common Time stamps variable for all chimes
static uint32_t u16AlarmTimeStamp = 0;
static uint8_t u8MilStatus,u8SysOkMIl2Status,u8MilSPN5080,u8PCDStatus,u8MilStatusDLCC1;
static uint8_t u8MilFlash,u8SysOkMIl2Flash,u8PCDFlash;
static uint8_t bTimeOutFlag65226_0x00 = 0;
static uint8_t bTimeOutFlagDm165226_0x00 = 0;
static uint8_t bTimeOutFlagDLCC1_64775_0x00 = 0;
static uint8_t bTimeOutFlag_PCDlamp_65226_0x00 = 0;
static uint8_t bTimeOutFlag_PCDflash_65226_0x00 = 0;
static uint8_t LedStatus = FALSE;
static uint8_t PCDStatus = FALSE;
static uint8_t TransOIlTempLedOnTFT = FALSE;
static uint8_t bTimeOutFlagDLCC1_MIL_64775 = 0;
uint8_t finalNeutralStatus = FALSE;
 BUZZER_TYPE sBuzz_Control = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x00};
//extern u8 u8GSdata[GS_DATA_SIZE];

// private prototypes --------------------------------------------------------
//Lamp Connected With IIC1

/// @addtogroup TELL_TALE_LIBRARY
/// @{ 
/// @defgroup TellTaleLib Private functions used in Tell tale file
//// 
///   
/// @addtogroup TellTaleLib
/// @{


void vLampDriven(void);
static void FuelWarningLedBuzzer(void);
static void SetCoolTempGaugeLedBars(void);
static void SetFuelGaugeLedBars(void);
static void GetPoorDEFQualityLampStatusCAN(void);
static void GetWaterInFuelLampStatusCAN(void);
static void GetGlowTimeIndicationLampStatusCAN(void);
static void GetHestLampStatusCAN(void);
static void GetDpfLampIndicationLampStatusCAN(void);
static void EngineStopLampStatusCAN(void);
void _vMilL2SysOkLamp(void);
void _vMilL(void);
void _vPCDLamp(void);
static void EngineOilPressureWarningLamp(void);
static void  EngineCoolTempWarningLamp(void);
static void _vReadPgn65226_0x00(void);
static void Left_TurnLamp();
static void Park_BrakeLamp();
static void Park_LightLamp();
static void Right_TurntLamp();
static void High_BeamtLamp();
static void Glow__PlugtLamp();
static void NeutralLamp();
static void BATT_CHARGingLamp();
static void _vTicTocAlarm(eBuzzerStateType BuzzerSound);
static void _vCheckBuzzerStatus();
static void _vLifeCriticalAlarm();
static void _vOnBuzzer();
static void _vTicAlarm(void);
static void _vTocAlarm(void);
static void CoolTempBuzzer(void);
static void TransmissionOilTempWarningLedBuzzer(void);
static void AirPressureWarningLedBuzzer(void);
static void TransmisionOilPressBarBuzzer();
uint8_t GetfinalNeutralStatus(void);
static void _vOnBuzzer_2_4kHz();
/// @} // endgroup TellTaleLib

/// @} // endgroup TELL_TALE_LIBRARY





void vControlTellTales(void)
{
	vLedDataSendToSpi();
	_vUpdateMuxPinStatus();
	_vInputsDebounce();
	SetCoolTempGaugeLedBars();
	SetFuelGaugeLedBars();
	GetWaterInFuelLampStatusCAN();
	GetGlowTimeIndicationLampStatusCAN();
	GetHestLampStatusCAN();
	GetDpfLampIndicationLampStatusCAN();
	EngineOilPressureWarningLamp();
	EngineCoolTempWarningLamp();
	_vReadPgn65226_0x00();
	_vMilL2SysOkLamp();
	_vMilL();
	_vPCDLamp();
	/* Hardwire MCU driven LAMP -----------------*/
	Left_TurnLamp();
	Park_BrakeLamp();
	Park_LightLamp();
	Right_TurntLamp();
	High_BeamtLamp();
	NeutralLamp();
	BATT_CHARGingLamp();
	/****************************************/
	_vCheckBuzzerStatus();
	FuelWarningLedBuzzer();
	TransmisionOilPressBarBuzzer();
	CoolTempBuzzer();
	TransmissionOilTempWarningLedBuzzer();

}


/*********************************************************************//**
*
* OFF All indicators 
*
* @param      None 
*
* @return     None
*************************************************************************/
void vAllLampsOff(void)
{
	uint8_t i;
	
	for(i = 0; i < TI_L4C_LED_DRIVER_DATA_SIZE; i++)
	{
		tlc6c5816_data[i] = 0x00;
	}

	vLedDataSendToSpi();
}


/*********************************************************************//**
*
* Off disabled Indicators
*
* @param      None 
*
* @return     None
*************************************************************************/

void vCheckLamps(void)
{
	tlc6c5816_data[0] = 0x00;
	tlc6c5816_data[1] = 0xFF;
	tlc6c5816_data[2] = 0xFE;
	tlc6c5816_data[3] = 0x00;
	tlc6c5816_data[4] = 0x7D;
	tlc6c5816_data[5] = 0xFF;
	tlc6c5816_data[6] = 0x00;
	tlc6c5816_data[7] = 0xFF;
	tlc6c5816_data[8] = 0xEF;

	vLedDataSendToSpi();

}



static void EngineCoolTempWarningLamp(void)
{
	if(GetEngineCoolTempWarningLampStatus())
	{
		ENGINE_COLANR_LEVEL_ON;
	}
	else
	{
		ENGINE_COLANR_LEVEL_OFF;
	}

}

static void EngineOilPressureWarningLamp(void)
{
	if(GetEngineOilPressWarningLampStatus() == TRUE)
	{
		ENGINE_OIL_PRESS_ON;
	}
	else
	{
		ENGINE_OIL_PRESS_OFF;
	}

}

static void CoolTempBuzzer(void)
{
	if(GetEngineCoolTempBuzzerStatus() == TRUE)
	{
		sBuzz_Control.bEngineCoolTempGauge = TRUE;
	}
	else
	{
		sBuzz_Control.bEngineCoolTempGauge = FALSE;
	}
}

static void FuelWarningLedBuzzer(void)
{
	static uint8_t Status = FALSE;
	static uint32_t u32clkTime = 0;
	static uint16_t u16CanOutTime = 0;

	switch(GetFuelWarningBuzzerStatus())
	{
		case eStatusON:
			Status = TRUE;
			sBuzz_Control.bFuelGauge = TRUE;
		break;
		case eStatusBlink:
			sBuzz_Control.bFuelGauge = FALSE;
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
			sBuzz_Control.bFuelGauge = FALSE;
		break;
		default:
			Status = FALSE;
			sBuzz_Control.bFuelGauge = FALSE;
		break;

	}

	if(Status)
	{
		FUEL_GAUGE_ON;
	}
	else
	{
		FUEL_GAUGE_OFF;
	}

}

static void AirPressureWarningLedBuzzer(void)
{
	static uint8_t Status = FALSE;
	static uint32_t u32clkTime = 0;
	static uint16_t u16CanOutTime = 0;

	if(GetEngineSelectionType() == eBackhoeLoaderEngineTata74Hp)
	{
		switch(GetAirPressureWarningBuzzerStatus())
		{
			case eStatusON:
				Status = TRUE;
				sBuzz_Control.bAIR_PRESSUR = TRUE;
			break;
			case eStatusBlink:
				sBuzz_Control.bAIR_PRESSUR = FALSE;
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
				sBuzz_Control.bAIR_PRESSUR = FALSE;
			break;
			default:
				Status = FALSE;
				sBuzz_Control.bAIR_PRESSUR = FALSE;
			break;

		}

		if(Status)
		{
			BRAKE_AIR_LED_ON;
		}
		else
		{
			BRAKE_AIR_LED_OFF;
		}
	}
	else
	{
		BRAKE_AIR_LED_OFF;
		sBuzz_Control.bAIR_PRESSUR = FALSE;
	}
}

static void TransmissionOilTempWarningLedBuzzer(void)
{
	static uint8_t Status = FALSE;
	static uint32_t u32clkTime = 0;
	static uint16_t u16CanOutTime = 0;

	switch(GetTransOilTempWarningBuzzerStatus())
		{
			case eStatusON:
				Status = TRUE;
				sBuzz_Control.bTransmissionOilTemp = TRUE;
			break;
			case eStatusBlink:
				sBuzz_Control.bTransmissionOilTemp = FALSE;
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
				sBuzz_Control.bTransmissionOilTemp = FALSE;
			break;
			default:
				Status = FALSE;
				sBuzz_Control.bTransmissionOilTemp = FALSE;
			break;

		}

		if(Status)
		{
			TRANS_OIL_TEMP_ON;
			TransOIlTempLedOnTFT = TRUE;
		}
		else
		{
			TRANS_OIL_TEMP_OFF;
			TransOIlTempLedOnTFT = FALSE;
		}
}


static void Left_TurnLamp()
{
	if(GetLeft_Turn_InputStatus() == TRUE)
	{
		LEFT_TURN_ON;
		sBuzz_Control.bTURN_LEFT = TRUE;
	}
	else
	{
		LEFT_TURN_OFF;
		sBuzz_Control.bTURN_LEFT = FALSE;
	}
}



static void Park_LightLamp()
{
	if(Park_Light_InputStatus() == TRUE)
	{
		PARKING_LIGHT_ON;
	}
	else
	{
		PARKING_LIGHT_OFF;
	}
}

static void Right_TurntLamp()
{
	if(Right_Turn_InputStatus() == TRUE)
	{
		RIGHT_TURN_ON;
		sBuzz_Control.bTURN_RIGHT = TRUE;
	}
	else
	{
		RIGHT_TURN_OFF;
		sBuzz_Control.bTURN_RIGHT = FALSE;
	}
}

static void TransmisionOilPressBarBuzzer()
{
	if(GetTrnsOIlPressureBuzzerStatus() == TRUE)
	{
		sBuzz_Control.bTransOilPrssBar = TRUE;
	}
	else
	{
		sBuzz_Control.bTransOilPrssBar = FALSE;
	}
}

static void High_BeamtLamp()
{
	if(GetHigh_Beam_InputStatus() == TRUE)
	{
		HIGH_BEAM_ON;
	}
	else
	{
		HIGH_BEAM_OFF;
	}
}



static void NeutralLamp()
{
	uint8_t dataSPN_3708 = 0u;
	uint8_t canNeutralStatus = FALSE;
	uint8_t hardwireNeutralStatus = FALSE;

	// Check CAN-based Neutral status
	if(!bGetPGNTimeOut(ePopupMessageSPN_4175))
	{
	    dataSPN_3708 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3708);  // CAN message
	    switch (dataSPN_3708)
	    {
	        case eLampOff:
	            canNeutralStatus = TRUE;
	            break;
	        case eLampOn:
	        case eError:             // 10
	        case eDataNotAvailable: // 11
	            canNeutralStatus = FALSE;
	            break;
	        default:
	            canNeutralStatus = FALSE;
	            break;
	    }
	}
	else
	{
		canNeutralStatus = FALSE;
	}

	// Check Hardwire Neutral status
	if(GetNeutral_InputStatus() == TRUE)
	{
	    hardwireNeutralStatus = TRUE;
	}
	else
	{
		hardwireNeutralStatus = FALSE;
	}

	// Final status: if either CAN or Hardwire says neutral is ON
	if(canNeutralStatus || hardwireNeutralStatus)
	{
	    finalNeutralStatus = TRUE;
	}
	else
	{
	    finalNeutralStatus = FALSE;
	}

	// Control lamp
	if(finalNeutralStatus)
	{
	    NETURAL_ON;
	}
	else
	{
	    NETURAL_OFF;
	}

}

uint8_t GetfinalNeutralStatus(void)
{
	return finalNeutralStatus;
}

static void Park_BrakeLamp()
{
	uint8_t dataSPN_3710 = 0u;
	uint8_t canPrakeBrakeStatus = FALSE;
	uint8_t hardwirePrakeBrakeStatus = FALSE;
	uint8_t finalPrakeBrakeStatus = FALSE;

	// Check CAN-based Neutral status
	if(!bGetPGNTimeOut(ePopupMessageSPN_4175))
	{
		dataSPN_3710 = (uint8_t)u32GetPGNdata(ePopupMessageSPN_3710);  // CAN message
	    switch (dataSPN_3710)
	    {
	        case eLampOff:
	            canPrakeBrakeStatus = TRUE;
	            break;
	        case eLampOn:
	        case eError:             // 10
	        case eDataNotAvailable: // 11
	            canPrakeBrakeStatus = FALSE;
	            break;
	        default:
	            canPrakeBrakeStatus = FALSE;
	            break;
	    }
	}
	else
	{
		canPrakeBrakeStatus = FALSE;
	}

	// Check Hardwire Neutral status
	if(Park_Brake_InputStatus() == TRUE)
	{
	    hardwirePrakeBrakeStatus = TRUE;
	}
	else
	{
		hardwirePrakeBrakeStatus = FALSE;
	}

	// Final status: if either CAN or Hardwire says neutral is ON
	if(canPrakeBrakeStatus || hardwirePrakeBrakeStatus)
	{
	    finalPrakeBrakeStatus = TRUE;
	}
	else
	{
	    finalPrakeBrakeStatus = FALSE;
	}

	// Control lamp
	if(finalPrakeBrakeStatus)
	{
		PARKING_BRAKE_ON;
		/* When the Neutral lamp is ON, the Parking brake buzzer should be OFF; otherwise, it should remain ON.*/
		if(finalNeutralStatus == FALSE)
		{
			sBuzz_Control.bPARK_BREAK = TRUE;
		}
		else
		{
			sBuzz_Control.bPARK_BREAK = FALSE;
		}
	}
	else
	{
		PARKING_BRAKE_OFF;
		sBuzz_Control.bPARK_BREAK = FALSE;
	}




}

static void BATT_CHARGingLamp()
{
	if(GetBattCharging_InputStatus() == TRUE)
	{
		BATT_CHARG_ON;
	}
	else
	{
		BATT_CHARG_OFF;
	}
}


static void SetFuelGaugeLedBars(void)
{
	eFuelGaugeBarType Bars;

	Bars = GetFuelGaugeBar();

	switch(Bars)
	{
		case efuelGaugeBar0:
			 FUEL_GAUGE_LED1_OFF;	FUEL_GAUGE_LED2_OFF;	FUEL_GAUGE_LED3_OFF;	FUEL_GAUGE_LED4_OFF;
			 FUEL_GAUGE_LED5_OFF;	FUEL_GAUGE_LED6_OFF;	FUEL_GAUGE_LED7_OFF;	FUEL_GAUGE_LED8_OFF;
		break;
		case efuelGaugeBar1:
			 FUEL_GAUGE_LED1_OFF;	FUEL_GAUGE_LED2_OFF;	FUEL_GAUGE_LED3_OFF;	FUEL_GAUGE_LED4_OFF;
			 FUEL_GAUGE_LED5_OFF;	FUEL_GAUGE_LED6_OFF;	FUEL_GAUGE_LED7_OFF;	FUEL_GAUGE_LED8_ON;
		break;
		case efuelGaugeBar2:
			 FUEL_GAUGE_LED1_OFF;	FUEL_GAUGE_LED2_OFF;	FUEL_GAUGE_LED3_OFF;	FUEL_GAUGE_LED4_OFF;
			 FUEL_GAUGE_LED5_OFF;	FUEL_GAUGE_LED6_OFF;	FUEL_GAUGE_LED7_ON;		FUEL_GAUGE_LED8_ON;
		break;
		case efuelGaugeBar3:
			 FUEL_GAUGE_LED1_OFF;	FUEL_GAUGE_LED2_OFF;	FUEL_GAUGE_LED3_OFF;	FUEL_GAUGE_LED4_OFF;
			 FUEL_GAUGE_LED5_OFF;	FUEL_GAUGE_LED6_ON;		FUEL_GAUGE_LED7_ON;		FUEL_GAUGE_LED8_ON;
		break;
		case efuelGaugeBar4:
			 FUEL_GAUGE_LED1_OFF;	FUEL_GAUGE_LED2_OFF;	FUEL_GAUGE_LED3_OFF;	FUEL_GAUGE_LED4_OFF;
			 FUEL_GAUGE_LED5_ON;	FUEL_GAUGE_LED6_ON;		FUEL_GAUGE_LED7_ON;		FUEL_GAUGE_LED8_ON;
		break;
		case efuelGaugeBar5:
			 FUEL_GAUGE_LED1_OFF;	FUEL_GAUGE_LED2_OFF;	FUEL_GAUGE_LED3_OFF;	FUEL_GAUGE_LED4_ON;
			 FUEL_GAUGE_LED5_ON;	FUEL_GAUGE_LED6_ON;		FUEL_GAUGE_LED7_ON;		FUEL_GAUGE_LED8_ON;
		break;
		case efuelGaugeBar6:
			 FUEL_GAUGE_LED1_OFF;	FUEL_GAUGE_LED2_OFF;	FUEL_GAUGE_LED3_ON;		FUEL_GAUGE_LED4_ON;
			 FUEL_GAUGE_LED5_ON;	FUEL_GAUGE_LED6_ON;		FUEL_GAUGE_LED7_ON;		FUEL_GAUGE_LED8_ON;
		break;
		case efuelGaugeBar7:
			 FUEL_GAUGE_LED1_OFF;	FUEL_GAUGE_LED2_ON;		FUEL_GAUGE_LED3_ON;		FUEL_GAUGE_LED4_ON;
			 FUEL_GAUGE_LED5_ON;	FUEL_GAUGE_LED6_ON;		FUEL_GAUGE_LED7_ON;		FUEL_GAUGE_LED8_ON;
		break;
		case efuelGaugeBar8:
			 FUEL_GAUGE_LED1_ON;	FUEL_GAUGE_LED2_ON;		FUEL_GAUGE_LED3_ON;		FUEL_GAUGE_LED4_ON;
			 FUEL_GAUGE_LED5_ON;	FUEL_GAUGE_LED6_ON;		FUEL_GAUGE_LED7_ON;		FUEL_GAUGE_LED8_ON;
		break;
		default:
			/*NA*/
		break;
	}

}




static void SetCoolTempGaugeLedBars(void)
{
	eCoolTempGaugeBarType Bars;
	Bars = GetEngineCoolTempLevelGaugeBars();

	switch(Bars)
	{
		case eCoolTempGaugeBar0:
			 COOLENT_TEMP_LED1_OFF;	COOLENT_TEMP_LED2_OFF;	COOLENT_TEMP_LED3_OFF;	COOLENT_TEMP_LED4_OFF;
			 COOLENT_TEMP_LED5_OFF;	COOLENT_TEMP_LED6_OFF;	COOLENT_TEMP_LED7_OFF;	COOLENT_TEMP_LED8_OFF;
		break;
		case eCoolTempGaugeBar1:
			 COOLENT_TEMP_LED1_ON;	COOLENT_TEMP_LED2_OFF;	COOLENT_TEMP_LED3_OFF;	COOLENT_TEMP_LED4_OFF;
			 COOLENT_TEMP_LED5_OFF;	COOLENT_TEMP_LED6_OFF;	COOLENT_TEMP_LED7_OFF;	COOLENT_TEMP_LED8_OFF;
		break;
		case eCoolTempGaugeBar2:
			 COOLENT_TEMP_LED1_ON;	COOLENT_TEMP_LED2_ON;	COOLENT_TEMP_LED3_OFF;	COOLENT_TEMP_LED4_OFF;
			 COOLENT_TEMP_LED5_OFF;	COOLENT_TEMP_LED6_OFF;	COOLENT_TEMP_LED7_OFF;	COOLENT_TEMP_LED8_OFF;
		break;
		case eCoolTempGaugeBar3:
			 COOLENT_TEMP_LED1_ON;	COOLENT_TEMP_LED2_ON;	COOLENT_TEMP_LED3_ON;	COOLENT_TEMP_LED4_OFF;
			 COOLENT_TEMP_LED5_OFF;	COOLENT_TEMP_LED6_OFF;	COOLENT_TEMP_LED7_OFF;	COOLENT_TEMP_LED8_OFF;
		break;
		case eCoolTempGaugeBar4:
			 COOLENT_TEMP_LED1_ON;	COOLENT_TEMP_LED2_ON;	COOLENT_TEMP_LED3_ON;	COOLENT_TEMP_LED4_ON;
			 COOLENT_TEMP_LED5_OFF;	COOLENT_TEMP_LED6_OFF;	COOLENT_TEMP_LED7_OFF;	COOLENT_TEMP_LED8_OFF;
		break;
		case eCoolTempGaugeBar5:
			 COOLENT_TEMP_LED1_ON;	COOLENT_TEMP_LED2_ON;	COOLENT_TEMP_LED3_ON;	COOLENT_TEMP_LED4_ON;
			 COOLENT_TEMP_LED5_ON;	COOLENT_TEMP_LED6_OFF;	COOLENT_TEMP_LED7_OFF;	COOLENT_TEMP_LED8_OFF;
		break;
		case eCoolTempGaugeBar6:
			 COOLENT_TEMP_LED1_ON;	COOLENT_TEMP_LED2_ON;	COOLENT_TEMP_LED3_ON;	COOLENT_TEMP_LED4_ON;
			 COOLENT_TEMP_LED5_ON;	COOLENT_TEMP_LED6_ON;	COOLENT_TEMP_LED7_OFF;	COOLENT_TEMP_LED8_OFF;
		break;
		case eCoolTempGaugeBar7:
			 COOLENT_TEMP_LED1_ON;	COOLENT_TEMP_LED2_ON;	COOLENT_TEMP_LED3_ON;	COOLENT_TEMP_LED4_ON;
			 COOLENT_TEMP_LED5_ON;	COOLENT_TEMP_LED6_ON;	COOLENT_TEMP_LED7_ON;	COOLENT_TEMP_LED8_OFF;
		break;
		case eCoolTempGaugeBar8:
			 COOLENT_TEMP_LED1_ON;	COOLENT_TEMP_LED2_ON;	COOLENT_TEMP_LED3_ON;	COOLENT_TEMP_LED4_ON;
			 COOLENT_TEMP_LED5_ON;	COOLENT_TEMP_LED6_ON;	COOLENT_TEMP_LED7_ON;	COOLENT_TEMP_LED8_ON;
		break;
		default:
			/*NA*/
		break;
	}
}


uint8_t GetSCRTemperingLampStatusCAN(void)
{
	uint8_t data = 0;
	static uint32_t clkTime = 0;
	static uint8_t status = 0;

	data = u32GetPGNdata(eSCR_TempRingLampTFT);
	//Check CAN message lost
	if(bGetPGNTimeOut(eSCR_TempRingLampTFT))
	{
		status = FALSE;
	}
	else
	{
		switch (data)
		{
			case eLedOff :
				status = FALSE;
			break;
			case eLedOn :
				status = TRUE;
			break;
			case eError2:
			case eError3:
				if(u32GetClockTicks() - clkTime  > 250)
				{
					clkTime = u32GetClockTicks();
					if(status == TRUE)
					{
						status = FALSE;
					}
					 else
				 	{
						status = TRUE;
				 	}
				}
				break;
			default :
				status = FALSE;
			break;
		}
	}

	return status;

}

static void GetPoorDEFQualityLampStatusCAN(void)
{
	uint8_t data = 0;
	static uint32_t clkTime = 0;
	static uint8_t status = 0;

	data = u32GetPGNdata(ePoorDefQualityLampDial);
	//Check CAN message lost
	if(bGetPGNTimeOut(ePoorDefQualityLampDial))
	{
		status = FALSE;
	}
	else
	{
		switch (data)
		{
			case eLedOff :
				status = FALSE;
			break;
			case eLedOn :
				status = TRUE;
			break;
			case eError2 :
			case eError3 :
				if(u32GetClockTicks() - clkTime  > 250)
				{
					clkTime = u32GetClockTicks();
					if(status == TRUE)
					{
						status = FALSE;
					}
					 else
				 	{
						status = TRUE;
				 	}
				}
				break;
			default :
				status = FALSE;
			break;
		}
	}

	if(status)
	{
		POOR_DEF_QUALITY_ON;
	}
	else
	{
		POOR_DEF_QUALITY_OFF;
	}

}

static void GetWaterInFuelLampStatusCAN(void)
{
	uint8_t data = 0;
	static uint8_t status = 0;

	data = u32GetPGNdata(eWaterInFuelLamp);
	//Check CAN message lost
	if(bGetPGNTimeOut(eWaterInFuelLamp))
	{
		status = FALSE;
	}
	else
	{
		switch (data)
		{
			case eLedOff :
				status = FALSE;
			break;
			case eLedOn :
				status = TRUE;
			break;
			default :
				status = FALSE;
			break;
		}
	}

	if(status)
	{
		WATER_IN_FUEL_ON;
	}
	else
	{
		WATER_IN_FUEL_OFF;
	}

}

static void GetGlowTimeIndicationLampStatusCAN(void)
{
	uint8_t data = 0;
	static uint8_t status = 0;
	uint8_t GlowPLugInputType = FALSE;


	data = u32GetPGNdata(eGlowtimeIndicationLamp_KoelEngine);
	//Check CAN message lost
	if(bGetPGNTimeOut(eGlowtimeIndicationLamp_KoelEngine))
	{
		status = FALSE;
	}
	else
	{
		switch (data)
		{
			case eLedOff :
				status = FALSE;
			break;
			case eLedOn :
				status = TRUE;
			break;
			default :
				status = FALSE;
			break;
		}
	}


	if(status)
	{
		GLOW_PLUG_ON;
	}
	else
	{
		GLOW_PLUG_OFF;
	}

}


uint8_t GetLowAdBlueLevelLampStatusCAN(void)
{
	uint8_t data = 0;
	static uint32_t clkTime = 0;
	static uint8_t status = 0;

	data = u32GetPGNdata(eLowAdBlueLevelLampTFT);
	//Check CAN message lost
	if(bGetPGNTimeOut(eLowAdBlueLevelLampTFT))
	{
		status = FALSE;
	}
	else
	{
		switch (data)
		{
			case eLedOff :
				status = FALSE;
			break;
			case eLedOn :
				status = TRUE;
			break;
			case eError2:
			case eError3:
				if(u32GetClockTicks() - clkTime  > 250)
				{
					clkTime = u32GetClockTicks();
					if(status == TRUE)
					{
						status = FALSE;
					}
					 else
				 	{
						status = TRUE;
				 	}
				}
				break;
			default :
				status = FALSE;
			break;
		}
	}

	return status;

}


uint8_t GetDPFRegenInhibitLampStatusCAN(void)
{
	uint8_t data = 0;
	static uint32_t clkTime = 0;
	static uint8_t status = 0;

	data = u32GetPGNdata(eDpfRegenInhibitLampSPN_3703);
	//Check CAN message lost
	if(bGetPGNTimeOut(eDpfRegenInhibitLampSPN_3703))
	{
		status = FALSE;
	}
	else
	{
		switch (data)
		{
			case eLedOff :
				status = FALSE;
			break;
			case eLedOn :
				status = TRUE;
			break;
			default :
				status = FALSE;
			break;
		}
	}

	return status;

}


static void GetHestLampStatusCAN(void)
{
	uint8_t data = 0;
	static uint32_t clkTime = 0;
	static uint8_t status = 0;

	data = u32GetPGNdata(eHestLampOnDial);
	//Check CAN message lost
	if(bGetPGNTimeOut(eHestLampOnDial))
	{
		status = FALSE;
	}
	else
	{
		switch (data)
		{
			case eLedOff :
				status = FALSE;
			break;
			case eLedOn :
				status = TRUE;
			break;
			case eLampFastBlink :
				if(u32GetClockTicks() - clkTime  > 250)
				{
					clkTime = u32GetClockTicks();
					if(status == TRUE)
					{
						status = FALSE;
					}
					 else
				 	{
						status = TRUE;
				 	}
				}
				break;
			default :
				status = FALSE;
			break;
		}
	}

	if(status)
	{
		HEST_LAMP_ON;
	}
	else
	{
		HEST_LAMP_OFF;
	}
}


static void GetDpfLampIndicationLampStatusCAN(void)
{
	uint8_t data = 0;
	static uint32_t clkTime = 0;
	static uint8_t status = 0;

	data = u32GetPGNdata(eDpfLampIndicationLampSPN_3697);
	//Check CAN message lost
	if(bGetPGNTimeOut(eDpfLampIndicationLampSPN_3697))
	{
		status = FALSE;
	}
	else
	{
		switch (data)
		{
			case eLedOff :
				status = FALSE;
			break;
			case eLedOn :
				status = TRUE;
			break;
			case eLampFastBlink :
				if(u32GetClockTicks() - clkTime  > 250)
				{
					clkTime = u32GetClockTicks();
					if(status == TRUE)
					{
						status = FALSE;
					}
					 else
				 	{
						status = TRUE;
				 	}
				}
				break;
			default :
				status = FALSE;
			break;
		}
	}

	if(status)
	{
		DPF_REGE_INHIBIT_ON;
	}
	else
	{
		DPF_REGE_INHIBIT_OFF;
	}

}

static void EngineStopLampStatusCAN(void)
{
	uint8_t dataAdBlueLamp = 0;
	uint8_t dataPoorDef = 0;
	uint8_t dataSCRTemp = 0;
	uint8_t dataEngineStop = 0;
	static uint32_t clkTime = 0;
	static uint8_t status = 0;

	dataAdBlueLamp = u32GetPGNdata(eLowAdBlueLevelLampTFT);
	dataPoorDef = u32GetPGNdata(ePoorDefQualityLampDial);
	dataSCRTemp = u32GetPGNdata(eSCR_TempRingLampTFT);
	dataEngineStop = u32GetPGNdata(eEngineStopLamp);

	//Check CAN message lost
	if(bGetPGNTimeOut(eEngineStopLamp))
	{
		ENGINE_STOP_OFF;
	}
	else
	{
		if(dataEngineStop == 5u)
		{
			if(dataAdBlueLamp == 3u || dataPoorDef == 3u || dataSCRTemp == 3u)
			{
				ENGINE_STOP_ON;
			}
			else
			{
				ENGINE_STOP_OFF;
			}
		}
		else
		{
			ENGINE_STOP_OFF;
		}
	}
}

static void _vReadPgn65226_0x00(void)
{
	bTimeOutFlag65226_0x00 =  bGetPGNTimeOut(eJMilSPN1213);
	bTimeOutFlagDm165226_0x00 =  bGetPGNTimeOut(eJMilDM1_1213);
	bTimeOutFlagDLCC1_MIL_64775 =  bGetPGNTimeOut(eMIL_DLCC1_64775);

	if(bTimeOutFlagDLCC1_MIL_64775 == TRUE)
	{
		u8MilStatusDLCC1 = 0;
	}

	if(bTimeOutFlag65226_0x00 == TRUE && bTimeOutFlagDm165226_0x00 == TRUE)
	{
		u8MilStatus = FALSE;
		u8MilFlash = FALSE;
	}


	//Check Engine , Mil and Stop engine have a same PGN time out
	if(bGetPGNDataReady(eJMilSPN1213))
	{
		u8MilStatus = ((eOBDLampdataType)u32GetPGNdata(eJMilSPN1213));
		u8MilFlash = ((eOBDLampdataType)u32GetPGNdata(eJMilSPN3038));
		u8SysOkMIl2Status = ((eOBDLampdataType)u32GetPGNdata(eJMil2_SysOkSPN987));
		u8SysOkMIl2Flash = ((eOBDLampdataType)u32GetPGNdata(eJMil2_SysOkSPN3041));
		u8PCDStatus = ((eOBDLampdataType)u32GetPGNdata(eJMil2_PCDlampSPN624));
		u8PCDFlash = ((eOBDLampdataType)u32GetPGNdata(eJMil2_PCDlampSPN3040));
		ClearPGNDataReady(eJMilSPN1213);
	}
	else if(bGetPGNDataReady(eJMilDM1_1213) )
	{
		u8MilStatus =  ((eOBDLampdataType)u32GetPGNdata(eJMilDM1_1213));
		u8MilFlash=	((eOBDLampdataType)u32GetPGNdata(eJMilDM1_3038));
		u8SysOkMIl2Status =  ((eOBDLampdataType)u32GetPGNdata(eJMil2_SysOkDM1_987));
		u8SysOkMIl2Flash = ((eOBDLampdataType)u32GetPGNdata(eJMil2_SysOkDM1_3041));
		u8PCDStatus = ((eOBDLampdataType)u32GetPGNdata(eJMil2_PCDlampDM1_624));
		u8PCDFlash = ((eOBDLampdataType)u32GetPGNdata(eJMil2_PCDlampDM1_3040));
		ClearPGNDataReady(eJMilDM1_1213);
	}

	else if(bGetPGNDataReady(eMIL_DLCC1_64775))
	{
		u8MilStatusDLCC1 = ((eOBDLampdataType)u32GetPGNdata(eMIL_DLCC1_64775));
		if(u8MilStatusDLCC1 != 1u)
		{
			u8MilStatusDLCC1 = 0u;
		}
		ClearPGNDataReady(eMIL_DLCC1_64775);
	}

}


void _vPCDLamp(void)
{
	uint8_t Data = 0;
	static uint32_t clkTime = 0;
	static uint16_t MilStartingTimeStamp = 0;
	static uint8_t StatusFlag = 0;

	if(bTimeOutFlag65226_0x00 == TRUE && bTimeOutFlagDm165226_0x00 == TRUE)
	{
		PCDStatus = FALSE;
	}
	else
	{
		if( u8PCDStatus == 1 && u8PCDFlash == 0 ) //Mode 4
		{
			if(u32GetClockTicks() - clkTime  > 250)
			{
				clkTime = u32GetClockTicks() ;
				StatusFlag = ~StatusFlag;
				if(StatusFlag)
				{
					PCDStatus = TRUE;
				}
				else
				{
					PCDStatus = FALSE;
				}
			}
		}
		else if( u8PCDStatus == 1 && u8PCDFlash == 1 )//Fast Flash Mode 4
		{
			if(u32GetClockTicks() - clkTime  > 125)
			{
				clkTime = u32GetClockTicks() ;
				StatusFlag = ~StatusFlag;
				if(StatusFlag)
				{
					PCDStatus = TRUE;
				}
				else
				{
					PCDStatus = FALSE;
				}
			}
		}
		else if( u8PCDStatus == 1 && u8PCDFlash == 2 ) //Mode 4
		{
			PCDStatus = FALSE;
		}
		else if( u8PCDStatus == 1 && u8PCDFlash == 3 ) //Mode 4
		{
			PCDStatus = TRUE;
		}
		else
		{
			PCDStatus = FALSE;
		}
	}
}

uint8_t GetPCDLampStatus()
{
	return PCDStatus;
}
/*********************************************************************//**
*
* On/OFF Mil indicator as per input
*
* @param      None
*
* @return     None
*************************************************************************/

void _vMilL(void)
{
	static uint32_t clkTime = 0;
	static uint8_t StatusFlag = 0;


	if(bTimeOutFlag65226_0x00 == TRUE && bTimeOutFlagDm165226_0x00 == TRUE && bTimeOutFlagDLCC1_MIL_64775 == TRUE)
	{
		LedStatus = FALSE;
	}
	else
	{
		if((u8MilStatus == 1u && u8MilFlash == 3u) || u8MilStatusDLCC1 == 1u) //Mode 4
		{
			LedStatus = TRUE;
		}
		else if( u8MilStatus == 1u && u8MilFlash == 0u) //Mode 4
		{
			if(u32GetClockTicks() - clkTime  > 250u)
			{
				clkTime = u32GetClockTicks() ;
				StatusFlag = ~StatusFlag;
				if(StatusFlag)
				{
					LedStatus = TRUE;
				}
				else
				{
					LedStatus = FALSE;
				}
			}
		}
		else if( u8MilStatus == 1u && u8MilFlash == 1u )//Fast Flash Mode 4
		{
			if(u32GetClockTicks() - clkTime  > 125u)
			{
				clkTime = u32GetClockTicks() ;
				StatusFlag = ~StatusFlag;
				if(StatusFlag)
				{
					LedStatus = TRUE;
				}
				else
				{
					LedStatus = FALSE;
				}
			}
		}
		else
		{
			LedStatus = FALSE;
			u8MilStatusDLCC1 = FALSE;
		}

	}

}

uint8_t GetMilLampStatus()
{
	return LedStatus;
}



/*********************************************************************//**
*
* On/OFF _vMilL2 SysOk indicator as per input
*
* @param      None
*
* @return     None
*************************************************************************/
void _vMilL2SysOkLamp(void)
{
	uint8_t Data = 0;
	static uint32_t clkTime = 0;
	static uint16_t MilStartingTimeStamp = 0;
	static uint8_t StatusFlag = 0;

	if(bTimeOutFlag65226_0x00 == TRUE && bTimeOutFlagDm165226_0x00 == TRUE)
	{
		SYSTEM_OK_LAMP_OFF;
	}
	else
	{
		if( u8SysOkMIl2Status == 1 && u8SysOkMIl2Flash == 0 ) //Mode 4
		{
			if(u32GetClockTicks() - clkTime  > 250)
			{
				clkTime = u32GetClockTicks() ;
				StatusFlag = ~StatusFlag;
				if(StatusFlag)
				{
					SYSTEM_OK_LAMP_ON;
				}
				else
				{
					SYSTEM_OK_LAMP_OFF;
				}
			}
		}
		else if( u8SysOkMIl2Status == 1 && u8SysOkMIl2Flash == 1 )//Fast Flash Mode 4
		{
			if(u32GetClockTicks() - clkTime  > 125)
			{
				clkTime = u32GetClockTicks() ;
				StatusFlag = ~StatusFlag;
				if(StatusFlag)
				{
					SYSTEM_OK_LAMP_ON;
				}
				else
				{
					SYSTEM_OK_LAMP_OFF;
				}
			}
		}
		else if( u8SysOkMIl2Status == 1u && u8SysOkMIl2Flash == 3u ) //Mode 4
		{
			SYSTEM_OK_LAMP_ON;
		}
		else
		{
			SYSTEM_OK_LAMP_OFF;
		}
	}
}


uint8_t GetLowFeuelStatus()
{
	return LowFeuelStatus;
}


uint8_t GetParkBrakeLampBuzzStatus()
{
	return ParkBrakeLampBuzzStatus;
}

/*********************************************************************//**
*
* Set buzzer On/OFF status bit wise in variable
*
* @param      None
*
* @return     None
*************************************************************************/
static void _vCheckBuzzerStatus()
{
	//Check Life Critical Alarms Low Air Pressure,Tilt Cab,Seat belt,GSO
	if(sBuzz_Control.bAIR_PRESSUR == TRUE || sBuzz_Control.bFuelGauge == TRUE
		|| sBuzz_Control.bEngineCoolTempGauge == TRUE || sBuzz_Control.bTransmissionOilTemp == TRUE
		||sBuzz_Control.bPARK_BREAK == TRUE  || sBuzz_Control.bTransOilPrssBar == TRUE)
	{
		sBuzz_Control.u8BuzzerSound |= LIFE_CRITICAL_ALARM;
	}
	else
	{
		sBuzz_Control.u8BuzzerSound &= ~LIFE_CRITICAL_ALARM;
	}

	//Tic Toc Alarm Left and Right Turn
	if(sBuzz_Control.bTURN_LEFT == TRUE || sBuzz_Control.bTURN_RIGHT == TRUE)
	{
		sBuzz_Control.u8BuzzerSound |= TIC_ALARM;
		sBuzz_Control.u8BuzzerSound &= ~TOC_ALARM;
	}
	else
	{
		sBuzz_Control.u8BuzzerSound &= ~TIC_ALARM;
		sBuzz_Control.u8BuzzerSound |= TOC_ALARM;
	}


}


/*********************************************************************//**
 *
 * Handles system alarms as per priority
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void vBuzzerControl()
{
	uint8_t Data = 0;
	uint8_t Data3 = 0;

	eBuzzerStateType BuzzerSoundType;

	if(sBuzz_Control.bBuzzerEnable)
	{
		// Set Buzzer priority order Here
		if(sBuzz_Control.u8BuzzerSound & (uint8_t)IRRITATE_ALARM)
		{
			BuzzerSoundType = eIrritateAlarm;
		}
		else if(sBuzz_Control.u8BuzzerSound & (uint8_t)LIFE_CRITICAL_ALARM)
			BuzzerSoundType = eLifeCriticalAlarm;
		else if(sBuzz_Control.u8BuzzerSound & (uint8_t)TIC_ALARM)
			BuzzerSoundType = eTicAlarm;
		else if(sBuzz_Control.u8BuzzerSound & (uint8_t)TOC_ALARM)
			BuzzerSoundType = eTocAlarm;
		else if(sBuzz_Control.u8BuzzerSound & (uint8_t)ENGINE_CRITICAL_ALARM)
			BuzzerSoundType = eEngineCriticalAlarm;
		else if(sBuzz_Control.u8BuzzerSound & (uint8_t)GENRAL_ALARM)
			BuzzerSoundType = eGeneralAlarm;
		else if(sBuzz_Control.u8BuzzerSound & (uint8_t)CONTINUE_ALARM)
			BuzzerSoundType = eContinueAlarm;
		else if(sBuzz_Control.u8BuzzerSound & (uint8_t)BEEP_ALARM)
			BuzzerSoundType = eBeepAlarm;
		else
			BuzzerSoundType = eStopAlarm;
	}
	else
	{
		BuzzerSoundType = eStopAlarm;
	}

	switch(BuzzerSoundType)
	{
		case eLifeCriticalAlarm:
			_vLifeCriticalAlarm();
		break;

		case eEngineCriticalAlarm:
			//_vEngineCriticalAlarm();
		break;
		case eGeneralAlarm:
			//_vGeneralAlarm();
		break;
		case eTicAlarm:
		case eTocAlarm:
			_vTicTocAlarm(BuzzerSoundType);
		break;
		case eContinueAlarm:
			_vOnBuzzer();
		break;
		case eStopAlarm:
			_vStopBuzzer();
		break;

		default:
			_vStopBuzzer();
		break;
	}

}

/*********************************************************************//**
 *
 * Clear All buzzer status flag
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void vClearAllLampsBuzzerFlag()
{
	sBuzz_Control.bPARK_BREAK = FALSE;
	sBuzz_Control.bTURN_RIGHT = FALSE;
	sBuzz_Control.bTURN_LEFT = FALSE;
	sBuzz_Control.bFuelGauge = FALSE;
	sBuzz_Control.bEngineCoolTempGauge = FALSE;
	sBuzz_Control.bTransmissionOilTemp = FALSE;
	sBuzz_Control.bAIR_PRESSUR = FALSE;
	sBuzz_Control.bBuzzerEnable = TRUE;

}

/*********************************************************************//**
 *
 * Tic Toc sound generate
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vTicTocAlarm(eBuzzerStateType BuzzerSound)
{
	static uint32_t Chimerate = 0;
	static eBuzzerStateType PreviousStatus = eTocAlarm;

	if( PreviousStatus != BuzzerSound)
	{
		PreviousStatus = BuzzerSound;
		if(BuzzerSound == eTicAlarm)
		{
			_vTicAlarm();
		}
		else if(BuzzerSound == eTocAlarm)
		{
			_vTocAlarm();
		}

		Chimerate = 20;
		AlarmTimeStamp = u32GetClockTicks();

	}
	else if( u32GetClockTicks() - AlarmTimeStamp >= Chimerate)
	{
		_vStopBuzzer();
	}

}
/*********************************************************************//**
 *
 * Buzzer chime continuous
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vOnBuzzer_2_4kHz()
{
	HAL_GPIO_WritePin(GPIOA,HIGH_OUTPUT__IN_Pin, GPIO_PIN_SET); // High out put PIn8
	 htim15.Init.Prescaler = 40;
	__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1,300u);
}

/*********************************************************************//**
 *
 * Life critical Alarm 500ms On and 500ms Off
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vLifeCriticalAlarm()
{
	static uint8_t isBuzzerOn = FALSE;
	static uint32_t u32AlarmTimeStamp = 0;

	if( u32GetClockTicks() - u32AlarmTimeStamp >= 250)
	{
		if(isBuzzerOn)
		{
			_vOnBuzzer_2_4kHz();
		}
		else
		{
			_vStopBuzzer();
		}
		isBuzzerOn = (uint8_t)!isBuzzerOn;
		u32AlarmTimeStamp = u32GetClockTicks();
	}

}

/*********************************************************************//**
 *
 * Generate Toc sound
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vTocAlarm()
{
	_vStopBuzzer();
}


/*********************************************************************//**
 *
 * Generate TIC sound
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vTicAlarm()
{
	 _vOnBuzzer_2_4kHz();
}
/*********************************************************************//**
 *
 * Buzzer chime continuous
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vOnBuzzer()
{
	_vOnBuzzer_2_4kHz();
}
/*********************************************************************//**
 *
 * Buzzer chime Stop
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void _vStopBuzzer()
{
	HAL_GPIO_WritePin(GPIOA,HIGH_OUTPUT__IN_Pin, GPIO_PIN_RESET); // High out put PIn8
	 htim15.Init.Prescaler = 0u;
	__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1,0u);

}

/*********************************************************************//**
 *
 * Buzzer enable
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void vBuzzerEnable()
{
	sBuzz_Control.bBuzzerEnable = TRUE;
}


/*********************************************************************//**
 *
 * Buzzer disable
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void vBuzzerDisable()
{
	sBuzz_Control.bBuzzerEnable = FALSE;
}


/*********************************************************************//**
 *
 * Set Buzzer sound type like life critical ,engine critical ,continuous etc..
 *
 * @param      Sound Type
 *
 * @return     None
 *************************************************************************/
void vSetBuzzerSoundType(uint8_t SoundType)
{
	sBuzz_Control.u8BuzzerSound = SoundType;

}

uint8_t GetTransOIlTempLedOnTFTStatus()
{
	return TransOIlTempLedOnTFT;
}







