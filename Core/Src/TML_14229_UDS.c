/** ###################################################################
**     Filename  : TML_14229_UDS.c
**     Project   : Y1ClusterDiagnostics
**     Processor : MC9S12XHZ256VAG
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 5/4/2016, 4:58 PM
**     Contents  :
**         User source code
**
** ###################################################################*/
/*** ---------------------------------------------------------------------------
**	COPYRIGHT: Indication Instruments Limited
**				Plot 19, Sector 6
**				Industrial Layout
**				Faridabad - 121006
**
**				Copyright 2016
**
**
** ---------------------------------------------------------------------------
** ==========  ===============	==============================================
** 
** Author:	Khusboo Lalani 
*****************************************************************************/


/* MODULE TML_14229_UDS */


#include "TML_14229_UDSH.h"
#include "J1939.h" 
#include "Speedo.h"
#include "RpmMeter.h"
#include "extEEPROM.h"
#include "IICRoutine.h"
#include "TellTales.h"




// Constant declarations -----------------------------------------------------

#define getRandomNumber (uint8_t)(rand() % 255)

#define CONTROL_WITH_ECU					0
#define CONTROL_WITH_TE 					3


const uint8_t dtcSupportedGroup[eNumofGroups][3] = {
													{0x80,0x0,0xff},//body group
													{0xc0,0x0,0xff},//network group
													{0xff,0xff,0xff}//all groups
												};

//IDs are defined and are arranged in the order of enum SUPPORTED_DTC and defined in the other files
//Use the enum SUPPORTED_DTC to point to the relevant id in this array
const uint8_t dtcSupportedId[eNumofSupportedDTC+3][3]={
													{0xa1,0x82,0x11},//FrontAirPresShortGnd
													{0xa1,0x83,0x11},//RearAirPresShortGnd
													{0xa1,0x30,0x11},//FuelLevelShortGnd
													{0x21,0x84,0x11},//OilPressureShortGnd		
													{0xc0,0x01,0x88},//J1939BusOff													
													{0,	0,	0}//eNumofSupportedDTC th: invalid id
													};

//DTC
 uint8_t firstTestFailedDTC = eNumofSupportedDTC;
 uint8_t firstConfirmedDTC = eNumofSupportedDTC;
 uint8_t mostRecentTestFailed = eNumofSupportedDTC;
 uint8_t mostRecentConfirmedDTC = eNumofSupportedDTC;
 uint8_t errorFlags[NON_ADC_DTC] = {0,0,0,0,0};



#define SUCCESSFUL							0x01
#define UNSUCCESSFUL						0x02
#define REASON1_FOR_FAILING					0x04
#define REASON2_FOR_FAILING					0x08

#define START_ROUTINE						0x01U
#define STOP_ROUTINE						0x02U
#define REQUEST_ROUTINE_RESULT				0x03U
#define ERASE_MEMORY						0xFF00U

#define CRC_VERIFICATION					0xFF03U
#define SERVICE_RESET						0x3F00U

#define ROUTINE_CONTROL_TYPE				0x01
#define ROUTINE_IDENTIFIER_BYTE1			0x3F
#define ROUTINE_IDENTIFIER_BYTE2			0x00

#define FIXED_BAUD_RATE						0x01
#define SPECIFIC_BAUD_RATE 					0x02
#define CAN_125K_BAUD_RATE					0x10
#define CAN_250K_BAUD_RATE					0x11
#define CAN_500K_BAUD_RATE					0x12
#define CAN_1M_BAUD_RATE					0x13

#define LID_GEN_BYTE2 						0x0A
#define LID_GEN_BYTE1 						0x01
#define LID_VEH_OPT							0x02
#define LID_FUEL_FILTER_CAL 				0x20
#define LID_BIAS_SPD_MUL_FACTOR				0x24
#define LID_AFE_CALC_UPDATE					0x46
#define LID_VEH_SPD_PPR						0x2C
#define LID_ENG_SPD							0x2D
#define LID_FUEL_TANK_CAP					0x34
#define LID_COOLANT_TEMP_THRES				0x35
#define LID_IFE_CALC_UPDATE					0x25
#define LID_FE_THRESH						0x39
#define LID_DTE_CALC_UPDATE					0x47
#define LID_LOW_AIR_PRES					0x41
#define	LID_ENGINE_RPM						0x43
#define LID_FUEL_CONSUMPTION_CONV_FACTOR	0x44
#define LID_VEH_SPD_CONV_FACTOR				0x45
#define LID_AWAKE_MODE						0x05
#define LID_TELL_TALE						0x03
#define DISP_LCD_SCREEN						0x06
#define LID_ODOMETER						0x40
#define LID_HOURMETER						0x43
#define LID_BUZZER_CONTROL					0x49
#define LID_FIRST_SERVICE_DUE				0x51
#define LID_SERVICE_INTERVAL				0x52
#define LID_SERVICE_THRESHOLD				0x53
#define LID_SVN_NO							0x41
#define LID_PCB_NO							0x42

#define SERVICE_MAX_RANGE					0xFFFFFF
#define RPM_EOL_MAX_RANGE					160	//160*25 = 4k RPM 
#define AFE_MIN_DISTANCE_TRAVEL				255 	 
#define AFE_MUL_FACTOR_MIN					50		 
#define AFE_MUL_FACTOR_MAX					150
#define SPEEDO_PULSES_MIN					4000
#define SPEEDO_PULSES_MAX					40000
#define SPEEDO_OVER_SPEED					140
#define SPEEDO_BIAS_FACTOR_MIN				128
#define SPEEDO_BIAS_FACTOR_MAX				154
#define FE_DATA_RANGE 						160
#define DTE_MUL_FACTOR_MIN 					50
#define DTE_MUL_FACTOR_MAX 					150
#define TEMP_START_RED_ZONE_MIN 			90
#define TEMP_START_RED_ZONE_MAX				120

#define LID_ANALOG_GAUGES					0x01

//---------------------------------------------//
//---------------------------------------------//

#define LID_ECU_ID_BYTE1					0xF1
#define LID_ECU_SERIAL_NUM					0x8C //For BS6 0x83 & For RDE 0x8C
//#define LID_ECU_SUPPLIER_PART_NUM 			0x91 //0x91
#define LID_ECU_HW_NUM						0x92 //0x92
#define LID_ECU_Hardware_NUM				0x91

#define LID_CONTAINER_PART_NUM				0x87
#define LID_ECU_SW_NUM						0x94
#define LID_VEHICLE_NUM						0x90 
#define LID_VARIANT_DATASET_NUM 			0x97
#define LID_VEH_CONFIG_NUM					0xA0
#define LID_PARAMETER_PART_NUMBER			0xA1
#define LID_REPROGRAMMING_COUNTER			0xA2
#define LID_UNIQUE_ID_FOR_FLASHING			0xA3
#define LID_ECU_ID_NUMBER					0xA4
#define LID_VCID_NUMBER						0xA6

#define LID_TML_ECU_Software_NUM			0x88
#define LID_BOOT_SW_ID_DATA_Identifier 		0x80


#define LID_ECUHW_VER_NUM 					0x93
#define LID_ECUSW_VER_NUM 					0x95
#define LID_DATE_OF_LAST_PROGRAM 			0x99
#define LID_PROGRAM_SHOP_CODE				0x98
#define LID_CUSTOMER_PART_NUM				0x9E
#define LID_OIL_PRESSURE_TALE_TELL			0xE2


//EOL Read and Write parameters length
//only R
#define EXTRA_READ_BYTES			 		3
#define EXTRA_READ_MUL_BYTES		 		2

//RW
#define EXTRA_WRITE_BYTES			 		3
#define IO_CONTROL_GAUGES_BYTE		 		(9U + 1U) // 1 extra byte for control option 

#define SPEEDO_PULSES_MIN_VAL				4000U
#define SPEEDO_PULSES_MAX_VAL				300000U
//*********************************CONST ECU PARAMETERS*************************************************//
const uint8_t u8IILPcbNo[IIL_PCB_NUMBER_BYTES] = {'3','4','7','6','8','R','0','1'}; // released software PCB no. (used for internal purpose only)
const uint16_t  u16ECUHardwareVersionNum = 0x0300; //ECU Harware no
const uint16_t  u16ECUSoftwareVersionNum = 0x0400; //ECU Soft no
uint8_t  u8VehicleOptionContent[VEHICLE_TYPE_BYTES] = {0x82,0xFF}; //0102 -
/* NEUTRAL GEAR - 1 bit
PARKING BRAKE  - 1 bit
NG CRANE ENGINE VARIANT  - 3 bit
BAUD RATE  - 2bit
RESERVED - 1 bit
RESERVED- 1 bit
RESERVED- 1 bit
RESERVED- 1 bit
RESERVED- 1 bit
RESERVED- 1 bit
RESERVED- 1 bit
RESERVED- 1 bit
RESERVED- 1 bit
RESERVED - 1 bit*/
uint8_t	u8VsPulsPerRev[SPEED_SENSOR_BYTES] = {0x02, 0xC6, 0xDC}; // Default Pulses 181980
const uint8_t u8TmlContainerPartNumber[TML_CONTAINER_PART_NUMBER_BYTES] = {'0','0','0','0','0','0','5','2','4','1','4','3'}; // 524143



//****************************************//Change Variables When Update Software Version//*****************************//
 //Change when update software version no.
 //Change when update software version no.
//****************************************//Change Variables When Update Software Version//*****************************//
// released software SVN no. (used for internal purpose only)
const uint8_t u8IILSvnNo[IIL_SVN_NUMBER_BYTES] = {'0','0','1','1'};

void AssignSwHrNo(void)
{

}


typedef struct
{
	uint8_t serverLockStatus : 2 ; //SERVER_LOCK_TYPE
	uint8_t securityDelayOn:1 ;
	uint8_t securityAccessAttempts :3;
	uint8_t securitySequence; // this is mapped to a byte in the message //SECURITY_ACCESS_TYPE
uint32_t securityAccessDelay; //this delay is to handle more than 3 failed attemps

}SECURITY_STRUCT;


//global variables 
uint8_t criticalFunctionStatus = FALSE;
uint8_t conditionNotCorrect = TRUE;
uint8_t odoProgrammingCtr = 0;

//local variables 
static uint8_t bCheckSumVarificationDone = 0;
static uint16_t RoutineControlStatusPending = 0;
static uint8_t dataTxBuffer[MAX_LENGTH + 20];//it should actually not contain data more than MAX_DATA_LENGTH
static sJ1939_RX_MSG_TYPE diagnosticsResponse;
static uint16_t resetTimeTracker;
static uint16_t setTimeForReset;
static uint8_t activeDiagnosticsSession = eDefaultSession;
static uint8_t bECUResetServiceActive = 0;
static uint8_t encryptedReceivedData[17];


static const uint8_t TMLAESCipherKey[17] = "IILFBDACECLUSTER";  //ACE Cluster Key  IILFBDACECLUSTER
static SECURITY_STRUCT securityAccessInfo = {eServerLocked,eServerLocked,FALSE,0,eNone,0};



/// @addtogroup TML_UDS_LIBRARY
/// @{
/// @defgroup TmlUdsLib  functions used 
//// 
///   
/// @addtogroup TmlUdsLib
/// @{

//local functions
static void getEncryptedData(void);
//data to be transmitted is converted into frames
static void formatMulBytesIntoFrames(void);
//data is extracted from received frames
static void formatFrameIntoMulBytes(void);

/// @} // endgroup TmlUdsLib

/// @} // endgroup TML_UDS_LIBRARY



// all communication is true by default
COMM_STRUCT_TYPE commInfo = {TRUE,TRUE,TRUE,TRUE};


//global functions
void resetDiagnosticSession(void);
void setStatusDefaultSession(void);
uint8_t isItTimeToReset(void);

/*********************************************************************//**
 *
 * Start diagnostic session 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void startDiagnosticsSession(void) 
{
	uint8_t i;
	uint8_t receivedMessageError = FALSE;

	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	if(singleFrameRx.numBytes != 2 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	else if(conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
		
	switch(singleFrameRx.dataBytes[2])
	{
		case eProgrammingSession: // this case should be on top because its negative response code has higher priority
			if(receivedMessageError == FALSE) //fall out of eDefaultSession &  eExtendedSession if there is error
			{
				activeDiagnosticsSession = eProgrammingSession;
			}
		break;

		case eDefaultSession: 
			if(receivedMessageError == FALSE) //fall out of eDefaultSession &  eExtendedSession if there is error 
			{
				activeDiagnosticsSession = eDefaultSession;
				//these should ideally happen after this response is sent
				//reset timers
				diagnosticsTimer = u32GetClockTicks(); //UDSData.timeStamp; 
				//reset diagnostics message receive buffer
				for(i=0; i<RX_QUE_SIZE ; i++)
					diagnosticsBuf[i].bDataRdy = FALSE;
				//lock if unlocked
				securityAccessInfo.serverLockStatus = eServerLocked;
          
			}
			break;

		case eExtendedSession: 
			if(receivedMessageError == FALSE) //fall out of eDefaultSession &  eExtendedSession if there is error 
			{
				activeDiagnosticsSession = eExtendedSession;
			}
			break;
		case eFotaSession: 
			if(receivedMessageError == FALSE) //fall out of eDefaultSession &  eExtendedSession if there is error 
			{
				activeDiagnosticsSession = eFotaSession;
					
			}
			break;
		default:
				diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
				receivedMessageError = TRUE;
			
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = StartDiagnosticSession;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 6;
		diagnosticsResponse.u8data[1] = 0x50;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
		diagnosticsResponse.u8data[3] = P1ServerMaxMSB;//P1servermax MSB
		diagnosticsResponse.u8data[4] = P1ServerMaxLSB;//P1servermax LSB ; 100ms
		diagnosticsResponse.u8data[5] = P2ServerMaxMSB;//P2servermax MSB
		diagnosticsResponse.u8data[6] = P2ServerMaxLSB;//P2servermax LSB ; 5000ms

	}
	diagnosticsResponse.bDataRdy = TRUE ;
	
}

/*********************************************************************//**
 *
 * ECU reset service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void ECUResetService(void)
{
	uint8_t receivedMessageError = FALSE;
	uint16_t oldRPM = 0;
	uint16_t speed = 0;
	
	//don't entertain if no session is active
	/*if( activeDiagnosticsSession != eDefaultSession  && activeDiagnosticsSession != eExtendedSession )
		return;*/
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE ) 
		return;
		
	 oldRPM = 0;//u16GetEngineSpeed();
	 speed = 0;//GetVehicleSpeed();
		
	// check this error conditions with the customer
   if(singleFrameRx.numBytes != 2)
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	//check engine speed and frequency speed as per diagnostics specs remove comments	 
	else if(conditionNotCorrect == TRUE || oldRPM != 0 || speed != 0 )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	
	switch(singleFrameRx.dataBytes[2])
	{
		case eSoftReset: 
			if( receivedMessageError == FALSE )
			{
				diagnosticsResponse.u8data[0] = 3;
				diagnosticsResponse.u8data[1] = 0x51;
				diagnosticsResponse.u8data[2] = eSoftReset;
				//reset ECU reset time tracker so that in setTimeForReset, ECU will reset
				resetTimeTracker = u32GetClockTicks();
				//as per 14229-1
				diagnosticsResponse.u8data[3] = 1;//extra second for watchdog to reset
				bECUResetServiceActive = TRUE;
				//activeDiagnosticsSession = eDefaultSession; //this happens in cpu_reset
				//watchdog reset will happen in main routine
			}
			break;

		case eHardReset :
		case eKeyOffOnReset:
			if( receivedMessageError == FALSE )
			{
				diagnosticsResponse.u8data[0] = 2;
				diagnosticsResponse.u8data[1] = 0x51;
				diagnosticsResponse.u8data[2] = eKeyOffOnReset;
				//reset ECU reset time tracker so that in setTimeForReset, ECU will reset
				resetTimeTracker = u32GetClockTicks();
				//as per 14229-1
				diagnosticsResponse.u8data[3] = 1;//extra second for watchdog to reset
				bECUResetServiceActive = TRUE;
				//activeDiagnosticsSession = eDefaultSession; //this happens in cpu_reset
				//watchdog reset will happen in main routine
			}
			break;
		case eEnableRapidPowerShutdown:
		case eDisableRapidPowerShutdown:
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
			break;

		default:
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ECUReset;
	}
	diagnosticsResponse.bDataRdy = TRUE;
	
}

/*********************************************************************//**
 *
 * security Access service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void securityAccessService(void)
{
	uint8_t i;
	static uint8_t seed[17];
	uint8_t receivedMessageError = FALSE;
	
	//don't entertain if extended session is not active & //10s delay as per TML_UDS
	if( (activeDiagnosticsSession != eExtendedSession) 
		&& (activeDiagnosticsSession != eProgrammingSession) 
		&&  (activeDiagnosticsSession != eFotaSession) )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}
	else if(securityAccessInfo.securityDelayOn == TRUE 
		&& (u32GetClockTicks() - securityAccessInfo.securityAccessDelay) < 5000 )
	{
		diagnosticsResponse.u8data[3] = RequiredTimeDelayNotExpired;
		receivedMessageError = TRUE;
	}
	else if(securityAccessInfo.securityAccessAttempts >= 3 ) //TML_UDS
	{
		securityAccessInfo.securityAccessDelay = u32GetClockTicks();
		securityAccessInfo.securityAccessAttempts = 0;
		securityAccessInfo.securityDelayOn = TRUE;
		diagnosticsResponse.u8data[3] = ExceededNumberofAttempts;
		receivedMessageError = TRUE;
	}
	else if(securityAccessInfo.serverLockStatus == eServerLocked)
	{
		if( singleFrameRx.dataBytes[2] == eRequestSeed ) //if the number is odd, then its requesting seed
		{
			securityAccessInfo.securityDelayOn = FALSE;
			//securityAccessInfo.securityAccessAttempts = 0;

			securityAccessInfo.securitySequence = eRequestSeed;
			multipleFramesTx.numBytes= 18;
			//multipleFramesTx.timeStamp = UDSData.timeStamp;//assign the original ID
			//multipleFramesTx.numFrames = 3;
			//multipleFramesTx.waitForResponse = TRUE;

			dataTxBuffer[0] = 0x67;
			dataTxBuffer[1] = eRequestSeed;
			// Seed the random time generator
			if(u32GetClockTicks() == 0)
				srand(352487);
			else
    			srand(u32GetClockTicks());
			
			for(i = 2; i < 18; i++)
			{
				dataTxBuffer[i] = getRandomNumber;
				seed[i-2] = dataTxBuffer[i];
			}
		
			seed[16] = 0;
			for(i= 18; i<42 ; i++)
			{
				dataTxBuffer[i]  = 0;
			}
			
			singleFrameRx.dataBytes[2] = eNone; // so that it doesn't come in this loop next time when key is sent
			formatMulBytesIntoFrames();
			
		}
		else if( multipleFramesRx.mulByteDiagRespData[3] == eSendKey
			&& securityAccessInfo.securitySequence != eNone)//this condition may not be required
		{
			//extract the decrypted data
			getEncryptedData();
			
			data_decrypt(encryptedReceivedData,TMLAESCipherKey);
			if((memcmp(encryptedReceivedData,&seed,16)== ERR_OK) )
			{
				securityAccessInfo.serverLockStatus = eServerUnlocked;
				diagnosticsResponse.u8data[0] = 2;
				diagnosticsResponse.u8data[1] = 0x67;
				diagnosticsResponse.u8data[2] = eSendKey;
				diagnosticsResponse.bDataRdy = TRUE;
			}
			else
			{
				diagnosticsResponse.u8data[3] = InvalidKey;
				receivedMessageError = TRUE;
			}
				
		}
	}
	else if (securityAccessInfo.serverLockStatus == eServerUnlocked )
	{
		if(singleFrameRx.dataBytes[2] == eRequestSeed ) //if the number is odd, then its requesting seed
		{
	
			securityAccessInfo.securitySequence = eRequestSeed;
			multipleFramesTx.numBytes= 18;
			//multipleFramesTx.numFrames = 3;
			dataTxBuffer[0] = 0x67;
			dataTxBuffer[1] = eRequestSeed;

			for(i = 2; i < 18; i++)
			{
				dataTxBuffer[i] = 0;
				seed[i-2] = 0;
			}
			formatMulBytesIntoFrames();
			securityAccessInfo.securityAccessAttempts = 0;
		}
 	}

	if (receivedMessageError == FALSE)
	{
		//does this block need to handle sub function sendkey ?
		if(  singleFrameRx.dataBytes[2] > eSendKey  )
		{
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
		}
		
			
		else if((singleFrameRx.dataBytes[1]== SecurityAccess && singleFrameRx.numBytes != 2) 
			||
			(multipleFramesRx.mulByteDiagRespData[2]== SecurityAccess && multipleFramesRx.numBytes != 18) )
	 	{
			diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
			receivedMessageError = TRUE;
		}
		//send key comes before request seed
		else if ( securityAccessInfo.securitySequence == eNone && ( multipleFramesRx.mulByteDiagRespData[3] == eSendKey ||
			singleFrameRx.dataBytes[2] == eSendKey) )
		{
			diagnosticsResponse.u8data[3] = RequestSequenceError;
			receivedMessageError = TRUE;
		}
		else if(conditionNotCorrect == TRUE)
		{
			diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
			receivedMessageError = TRUE;
		}
	}
	if(receivedMessageError == TRUE )
	{
		for(i=4;i<8;i++)
			diagnosticsResponse.u8data[i] = 0;	
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = SecurityAccess;
		securityAccessInfo.securitySequence = eNone;
		//count attempts only if the failed-attempt -delay-timer has not started
		if(diagnosticsResponse.u8data[3] != RequiredTimeDelayNotExpired)
			securityAccessInfo.securityAccessAttempts++;
		diagnosticsResponse.bDataRdy = TRUE;
		//in case this was set before the low priority errors were detected
		multipleFramesTx.dataReady = FALSE;
	}
	

}

/*********************************************************************//**
 *
 * Communication Control Service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void communicationControlService(void)
{
	uint8_t receivedMessageError = FALSE;

	if( activeDiagnosticsSession != eDefaultSession )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	else if(singleFrameRx.numBytes != 3 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
    else
    {
	  	switch(singleFrameRx.dataBytes[2]) //control type
    	{
    		case eEnableRxTx: 
    			if( receivedMessageError == FALSE )
    			{
    				if(((singleFrameRx.dataBytes[3] & 0xf0) >> 4) == 0)//communication type upper nibble
    				{
    					if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalComm )//comunication type lower nibble
    					{
    						commInfo.normalCommTxEnabled = TRUE;
    						commInfo.normalCommRxEnabled = TRUE;
    					}
    					
    					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNMComm)
    					{
    						commInfo.networkCommTxEnabled = TRUE;
    						commInfo.networkCommRxEnabled = TRUE;
    					}
    					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalCommNMComm )
    					{
    						commInfo.networkCommTxEnabled = TRUE;
    						commInfo.networkCommRxEnabled = TRUE;
    						commInfo.normalCommTxEnabled = TRUE;
    						commInfo.normalCommRxEnabled = TRUE;
    					}
    					else //subnets not supported in this version
    					{
    						diagnosticsResponse.u8data[3] = RequestOutOfRange;
    						receivedMessageError = TRUE;
    					}
    				}
    				//else handle subnets
    			}
    			break;
    		case eEnableRxDisableTx: 
    			if( receivedMessageError == FALSE )
    			{
    				if(((singleFrameRx.dataBytes[3] & 0xf0) >> 4) == 0)//communication type upper nibble
    				{
    					if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalComm )//comunication type lower nibble
    					{
    						commInfo.normalCommTxEnabled = FALSE;
    						commInfo.noResponse = FALSE;//respond to this message
    						commInfo.normalCommRxEnabled = TRUE;
    					}
    					
    					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNMComm)
    					{
    						commInfo.networkCommTxEnabled = FALSE;
    						commInfo.noResponse = FALSE;//respond to this message
    						commInfo.networkCommRxEnabled = TRUE;
    					}
    					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalCommNMComm )
    					{
    						commInfo.networkCommTxEnabled = FALSE;
    						commInfo.networkCommRxEnabled = TRUE;
    						commInfo.normalCommTxEnabled = FALSE;
    						commInfo.normalCommRxEnabled = TRUE;
    						commInfo.noResponse = FALSE;//respond to this message
    					}
    					else //subnets not supported in this version
    					{
    						diagnosticsResponse.u8data[3] = RequestOutOfRange;
    						receivedMessageError = TRUE;
    					}
    				}
    				//else handle subnets
    			}
    			break;
    		case eDisableRxEnableTx: 
    			if( receivedMessageError == FALSE )
    			{
    				if(((singleFrameRx.dataBytes[3] & 0xf0) >> 4) == 0)//communication type upper nibble
    				{
    					if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalComm )//comunication type lower nibble
    					{
    						commInfo.normalCommTxEnabled = TRUE;
    						commInfo.normalCommRxEnabled = FALSE;
    					}
    					
    					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNMComm)
    					{
    						commInfo.networkCommTxEnabled = TRUE;
    						commInfo.networkCommRxEnabled = FALSE;
    					}
    					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalCommNMComm )
    					{
    						commInfo.networkCommTxEnabled = TRUE;
    						commInfo.networkCommRxEnabled = FALSE;
    						commInfo.normalCommTxEnabled = TRUE;
    						commInfo.normalCommRxEnabled = FALSE;
    					}
    					else //subnets not supported in this version
    					{
    						diagnosticsResponse.u8data[3] = RequestOutOfRange;
    						receivedMessageError = TRUE;
    					}
    				}
    				//else handle subnets
    			}
    			break;
    		case eDisableRxandTx: 
    			if( receivedMessageError == FALSE )
    			{
    				if(((singleFrameRx.dataBytes[3] & 0xf0) >> 4) == 0)//communication type upper nibble
    				{
    					if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalComm )//comunication type lower nibble
    					{
    						commInfo.normalCommTxEnabled = FALSE;
    						commInfo.noResponse = FALSE;//respond to this message
    						commInfo.normalCommRxEnabled = FALSE;
    					}
    					
    					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNMComm)
    					{
    						commInfo.networkCommTxEnabled = FALSE;
    						commInfo.noResponse = FALSE;//respond to this message
    						commInfo.networkCommRxEnabled = FALSE;
    					}
    					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalCommNMComm )
    					{
    						commInfo.networkCommTxEnabled = FALSE;
    						commInfo.networkCommRxEnabled = FALSE;
    						commInfo.noResponse = FALSE;//respond to this message
    						commInfo.normalCommTxEnabled = FALSE;
    						commInfo.normalCommRxEnabled = FALSE;
    					}
    					else //subnets not supported in this version
    					{
    						diagnosticsResponse.u8data[3] = RequestOutOfRange;
    						receivedMessageError = TRUE;
    					}
    				}
    				//else handle subnets
    			}
    			break;
    		default:
    			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
    			receivedMessageError = TRUE;
    
    	}
    	}
	if(receivedMessageError== FALSE && conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}	
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = CommunicationControl;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 2;
		diagnosticsResponse.u8data[1] = 0x68;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
	}
	diagnosticsResponse.bDataRdy = TRUE;
	
}

/*********************************************************************//**
 *
 * Tester Present Service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void TesterPresentService(void)
{
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	uint8_t receivedMessageError = FALSE;

	if(singleFrameRx.numBytes != 2 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	
	if(singleFrameRx.dataBytes[2] == 0 || singleFrameRx.dataBytes[2] == 0x80 )
	{
		if(receivedMessageError == FALSE )
		{
			diagnosticsResponse.u8data[0] = 2;
			diagnosticsResponse.u8data[1] = 0x7e;
			diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2] ;
			//reset the timer since the tester is present so that the current diagnostic session does not expire
			diagnosticsTimer = u32GetClockTicks();
		}
	}
	else
	{	diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
		receivedMessageError = TRUE;
	}	

	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = TesterPresent;
	}
	diagnosticsResponse.bDataRdy = TRUE;
	
}

void ControlDTCSettingServiceSingle(void)
{
	uint8_t receivedMessageError = FALSE;
	uint8_t i;
	
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;
	
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	if( (activeDiagnosticsSession != eProgrammingSession )
		&& (activeDiagnosticsSession != eFotaSession ) )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}
	else if(singleFrameRx.numBytes != 5 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}


	if(receivedMessageError== FALSE && conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ControlDTCSetting;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 2;
		diagnosticsResponse.u8data[1] = 0xC5;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];		
		//vSetInternalDtcSaveFlag(TRUE);
	}

	diagnosticsResponse.bDataRdy = TRUE;
			
}

void ControlDTCSettingServiceMultiple(void)
{
	uint8_t receivedMessageError = FALSE;
	uint8_t i=0,j=0,numOfDTCs;
	
	/*if(securityAccessInfo.serverLockStatus == eServerLocked)
		return;*/
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;

		
	//Service Id bytes  
	numOfDTCs = ( multipleFramesRx.numBytes - 2 )/3;

	if( (activeDiagnosticsSession != eProgrammingSession )
		&& (activeDiagnosticsSession != eFotaSession ) )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}
	else if( numOfDTCs > eNumofSupportedDTC )
	{
		diagnosticsResponse.u8data[3] = RequestOutOfRange;
		receivedMessageError = TRUE;
	}
	//error = numOfDTCs;//this error will decrement on every DTC match; it should be zero in the end so that negative response is not sent

	
	if(receivedMessageError== FALSE && conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ControlDTCSetting;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 2;
		diagnosticsResponse.u8data[1] = 0xC5;
		diagnosticsResponse.u8data[2] = multipleFramesRx.mulByteDiagRespData[3];	
		//vSetInternalDtcSaveFlag(TRUE);
	}
	diagnosticsResponse.bDataRdy = TRUE ;
			
}

void ClearDiagnosticsInformationService(void)
{

	uint8_t receivedMessageError = FALSE;
	uint8_t i;

	/*if(securityAccessInfo.serverLockStatus == eServerLocked)
		return;*/
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
			return;
	
	if( (activeDiagnosticsSession != eProgrammingSession )
		&& (activeDiagnosticsSession != eFotaSession ) )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}			
	else if(singleFrameRx.numBytes != 4 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	else
	{
		//if DTC was specified

			
	}
	if(receivedMessageError == FALSE && conditionNotCorrect == TRUE )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}

	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ClearDiagnosticInformation;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 1;
		diagnosticsResponse.u8data[1] = 0x54;		
		vSetInternalDtcSaveFlag(TRUE);
		//clear them all regardless of which DTCs have been cleared
		firstTestFailedDTC = eNumofSupportedDTC;
 		firstConfirmedDTC = eNumofSupportedDTC;
  		mostRecentTestFailed = eNumofSupportedDTC;
  		mostRecentConfirmedDTC = eNumofSupportedDTC;
	}
	
	diagnosticsResponse.bDataRdy = TRUE ;
}

void ReadDiagnosticTroubleInformationService(void)
{
	uint8_t receivedMessageError = FALSE;
	uint8_t mulByteResponse = FALSE;
	uint8_t i, dtcloggedFlag = FALSE;
	uint8_t count=0,multiByteIndex = 3;

	
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	if (commInfo.normalCommRxEnabled == FALSE )
		return;

	if( (activeDiagnosticsSession != eProgrammingSession )
		&& (activeDiagnosticsSession != eFotaSession ) )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}	
	else if( (singleFrameRx.dataBytes[2] <=  eDTCbyStatusMask  && singleFrameRx.numBytes != 3 )
		|| (singleFrameRx.dataBytes[2] > eDTCbyStatusMask && singleFrameRx.numBytes != 2) 
		)
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}


	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ReadDTCInformationService;
		diagnosticsResponse.bDataRdy = TRUE;
	}
	else if (mulByteResponse == TRUE )
	{
		dataTxBuffer[0] = 0x59;
		dataTxBuffer[1] = singleFrameRx.dataBytes[2];
		//dataTxBuffer[2] = DTC_STATUS_MASK_SUPPORTED;
		formatMulBytesIntoFrames();
	}
	else
	{
		diagnosticsResponse.u8data[1] = 0x59;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
		diagnosticsResponse.bDataRdy = TRUE;
	}

}


/*********************************************************************//**
 *
 * Routine control service used for service reminder reset
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void RoutineService()
{
	uint8_t receivedMessageError = FALSE;
	uint8_t status = 0;
	//eSERVICE_REMINDER_TYPE ServiceStatus;
	uint16_t RoutineIdentifier = 0;
	uint16_t speed = 0;
	static uint32_t ClkTimer = 0;
	uint8_t temp = 0;
	uint8_t temp1 = 0;
	static uint8_t PendingResponseCount = 0;

	if(commInfo.normalCommTxEnabled == FALSE )
	{
		commInfo.noResponse = TRUE;
	}
	
	if (commInfo.normalCommRxEnabled == FALSE )
	{
		return;
	}
	
	if( (RoutineControlStatusPending == CHECK_PROG_DEP ) || (RoutineControlStatusPending == ERASE_MEMORY) )
	{
		temp = bCheckDepenDencies();
		temp1 = bFlashEarseStatus();
		if( (temp == TRUE ) && (RoutineControlStatusPending == CHECK_PROG_DEP) )
		{
			diagnosticsResponse.u8data[0] = 4;
			diagnosticsResponse.u8data[1] = 0x71;
			diagnosticsResponse.u8data[2] = START_ROUTINE;
			diagnosticsResponse.u8data[3] = 0xFF;
			diagnosticsResponse.u8data[4] = 0x01;
			diagnosticsResponse.bDataRdy = TRUE;
			RoutineControlStatusPending = 0;
		}
		else if((temp1 == TRUE) && (RoutineControlStatusPending == ERASE_MEMORY) )
		{
			diagnosticsResponse.u8data[0] = 4;
			diagnosticsResponse.u8data[1] = 0x71;
			diagnosticsResponse.u8data[2] = START_ROUTINE;
			diagnosticsResponse.u8data[3] = 0xFF;
			diagnosticsResponse.u8data[4] = 0x00;
			diagnosticsResponse.bDataRdy = TRUE;
			RoutineControlStatusPending =0;
			//ClearFlashEraseFlag();
		}
		else
		{
		  if( (u32GetClockTicks()- ClkTimer) > 2000)
			{
				ClkTimer = u32GetClockTicks();
				diagnosticsResponse.u8data[0] = 3;
				diagnosticsResponse.u8data[1] = 0x7f;
				diagnosticsResponse.u8data[2] = RoutineControlService;
				diagnosticsResponse.u8data[3] = RequestCorrectlyReceivedResponsePending;
				diagnosticsResponse.bDataRdy = TRUE;
				if(++PendingResponseCount > 2)
				{
					PendingResponseCount = 0;
					RoutineControlStatusPending =0;
				}
			}
		}

		return;

	}

	speed = u16GetEngineSpeed();
		
	if(securityAccessInfo.serverLockStatus == eServerLocked)
	{
		diagnosticsResponse.u8data[3] = SecurityAccessDenied;
		receivedMessageError = TRUE;
	}
	else if(singleFrameRx.numBytes != 4 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	else
	{
	;
	}

	RoutineIdentifier = singleFrameRx.dataBytes[3];
	RoutineIdentifier = (RoutineIdentifier << 8) | singleFrameRx.dataBytes[4];
		
	if(singleFrameRx.dataBytes[2] == START_ROUTINE )
	{
		
		if(receivedMessageError == FALSE)
		{
	 	 	switch(RoutineIdentifier)
	 	 	{
		 	 		case CHECK_PROG_DEP:
					temp = GetControllerMode();
					if(temp == eBootloaderMode)
					{
						; //do nothing in application code 
					}
					else				
					{
						RoutineControlStatusPending = CHECK_PROG_DEP;					
						
					}
						
						break;
		 	 		case ERASE_MEMORY:
											//TBD check Speedo& RPM erase memory logic to be added 
		 	 			temp = 1u;//GetControllerMode();
						if(temp == eBootloaderMode)
						{
							RoutineControlStatusPending = ERASE_MEMORY;							
						}
						else				
						{
						//In aplication code erase memory not support 
						 receivedMessageError = TRUE;
						 diagnosticsResponse.u8data[3] = RequestOutOfRange; //request out of range							
						}
						break;
						case SERVICE_RESET:
											//TBD check Speedo& RPM erase memory logic to be added 
							temp = 1u;//GetControllerMode();
						if(temp == eBootloaderMode)
						{
							status = UNSUCCESSFUL;
						}
						else				
						{

						}
						break;
					default:
						receivedMessageError = TRUE;
						diagnosticsResponse.u8data[3] = RequestOutOfRange; //request out of range
					break;
	 	 	}
		}
	}
	else if( (receivedMessageError == FALSE ) && (singleFrameRx.dataBytes[2] == REQUEST_ROUTINE_RESULT ) )
	{
		if(RoutineIdentifier == CRC_VERIFICATION)
		{
			temp = 1u;//GetControllerMode();
			if(temp == eBootloaderMode)
			{
				status =  bCheckSumStatus();	
				bCheckSumVarificationDone = TRUE;
			}
			else
			{
			    receivedMessageError = TRUE;
			    diagnosticsResponse.u8data[3] = SubfunctionNotSupported;			
			}		 	
		}
		else
		{
			receivedMessageError = TRUE;
			diagnosticsResponse.u8data[3] = RequestOutOfRange;
		}
			
	}
	else if ( diagnosticsResponse.u8data[3] != SecurityAccessDenied )
	{
		receivedMessageError = TRUE;
		diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
	}
	else
	{
		;
	}
		
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = RoutineControlService;
	}
	else if( (speed !=0 ) && ( RoutineIdentifier != SERVICE_RESET) )
	{
		receivedMessageError = TRUE;
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
	}
		
	else
	{
		if( (singleFrameRx.dataBytes[2] == REQUEST_ROUTINE_RESULT ) || ( RoutineIdentifier == SERVICE_RESET) ) //request routine responce in 5 byte
		{
			diagnosticsResponse.u8data[0] = 5;
			diagnosticsResponse.u8data[1] = 0x71;
			diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
			diagnosticsResponse.u8data[3] = singleFrameRx.dataBytes[3];
			diagnosticsResponse.u8data[4] = singleFrameRx.dataBytes[4];
			diagnosticsResponse.u8data[5] = status;
		}
		else if( (RoutineControlStatusPending == CHECK_PROG_DEP ) || ( RoutineControlStatusPending == ERASE_MEMORY ) )
		{
			diagnosticsResponse.u8data[0] = 3;
			diagnosticsResponse.u8data[1] = 0x7f;
			diagnosticsResponse.u8data[2] = RoutineControlService;
			diagnosticsResponse.u8data[3] = RequestCorrectlyReceivedResponsePending;
		}
		else
		{
			diagnosticsResponse.u8data[0] = 4;
			diagnosticsResponse.u8data[1] = 0x71;
			diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
			diagnosticsResponse.u8data[3] = singleFrameRx.dataBytes[3];
			diagnosticsResponse.u8data[4] = singleFrameRx.dataBytes[4];	
			
		}
		
		
	}
	diagnosticsResponse.bDataRdy = TRUE;
	
}
/*********************************************************************//**
 *
 * Link Control Service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void LinkControlService(void)
{
	uint8_t receivedMessageError = FALSE;

	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;
	
   if(singleFrameRx.numBytes != 3 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}

	
	if(receivedMessageError == FALSE)	
	{
		if(singleFrameRx.dataBytes[2] == FIXED_BAUD_RATE )
		{
			switch (singleFrameRx.dataBytes[3])
			{
				
				case CAN_250K_BAUD_RATE:
					vSetCanBaudRate(e250);
				break;
				case CAN_500K_BAUD_RATE:					
					vSetCanBaudRate(e500);
				break;
				case CAN_1M_BAUD_RATE:
					vSetCanBaudRate(e1000);
				break;

				default: 
					diagnosticsResponse.u8data[3] = RequestOutOfRange;
					receivedMessageError = TRUE;
				break;
					
			}
		}
		else
		{
		 	 	
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
		}
		
	}//End if(receivedMessageError == FALSE)	
	if(receivedMessageError == FALSE && conditionNotCorrect == TRUE )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = LinkControl;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 2;
		diagnosticsResponse.u8data[1] = 0xC7;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
	}
	diagnosticsResponse.bDataRdy = TRUE;


}



/*********************************************************************//**
 *
 * Input Output Control dentifier Service multiple
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void InputOutputControlByIdentifierServiceMul(void)
{
	uint8_t receivedMessageError = FALSE;
	uint8_t mulByteResponse = FALSE;
	uint16_t oldRPM = 0;
	uint16_t SpeedoSpeed = 0;

	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;
	diagnosticsResponse.u8data[3] = ServiceNotSupported;
		receivedMessageError = TRUE;

	
	if(receivedMessageError == FALSE)	
	{
		if(multipleFramesRx.mulByteDiagRespData[3] == LID_GEN_BYTE2) 
		{
			switch (multipleFramesRx.mulByteDiagRespData[4])
			{
				

				default: 
					diagnosticsResponse.u8data[3] = RequestOutOfRange;
					receivedMessageError = TRUE;
				break;	
			}
			
		}
		else
		{
			diagnosticsResponse.u8data[3] = RequestOutOfRange;
			receivedMessageError = TRUE;
		}
		
	}
	
	if(receivedMessageError == FALSE && conditionNotCorrect == TRUE  )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = InputandOutputControlbyId;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = 0x6F;
		diagnosticsResponse.u8data[2] = multipleFramesRx.mulByteDiagRespData[3];
		diagnosticsResponse.u8data[3] = multipleFramesRx.mulByteDiagRespData[4];
	}
	
	diagnosticsResponse.bDataRdy = TRUE;


}

/*********************************************************************//**
 *
 * Input Output Control  dentifier Service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void InputOutputControlByIdentifierService(void)
{
	uint8_t receivedMessageError = FALSE;
	uint16_t oldRPM = 0;
	uint16_t SpeedoSpeed = 0;
	uint8_t i;

	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;
	diagnosticsResponse.u8data[3] = ServiceNotSupported;
		receivedMessageError = TRUE;


	if(receivedMessageError == FALSE && conditionNotCorrect == TRUE )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = InputandOutputControlbyId;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = 0x6F;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
		diagnosticsResponse.u8data[3] = singleFrameRx.dataBytes[3];
	}
	diagnosticsResponse.bDataRdy = TRUE;
	
}


/*********************************************************************//**
 *
 * Control DTC Setting
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/
/*
void ControlDTCSettingServiceSingle(void)
{
	
}
*/

/*********************************************************************//**
 *
 * Control DTC Setting Service Multiple
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

/*
void ControlDTCSettingServiceMultiple(void)
{
	
			
}
*/
/*********************************************************************//**
 *
 * Read Data dentifier Service Single
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void ReadDataByIdentifierServiceSingle(void)
{
	uint8_t receivedMessageError = FALSE;
	uint8_t mulByteResponse = FALSE;
	uint8_t i;
	uint32_t TempByte =0;
	uint32_t Temp1 =0;
	uint32_t Temp2 =0;
	uint16_t temp = 0;
	uint32_t VehiclePPR = 0;
	uint16_t VehicleOptionContentVal = 0;

	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	if(singleFrameRx.numBytes != 3 && singleFrameRx.numBytes != 5 && singleFrameRx.numBytes != 7  )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	else if(conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(singleFrameRx.dataBytes[2] == LID_ECU_ID_BYTE1)
	{
		switch(singleFrameRx.dataBytes[3] ) //second byte of the LID
		{
			case LID_ECUHW_VER_NUM:
				if( receivedMessageError == FALSE )
				{
					diagnosticsResponse.u8data[0] = ECU_HW_PART_BYTES + EXTRA_READ_BYTES;;
					diagnosticsResponse.u8data[4] = (uint16_t)u16ECUHardwareVersionNum>> 8;
					diagnosticsResponse.u8data[5] = (uint8_t)u16ECUHardwareVersionNum;
				}
				break;
			case LID_ECUSW_VER_NUM:
				if( receivedMessageError == FALSE )
				{
					diagnosticsResponse.u8data[0] = ECU_SW_PART_BYTES + EXTRA_READ_BYTES;;
					diagnosticsResponse.u8data[4] = (uint16_t)u16ECUSoftwareVersionNum>> 8;
					diagnosticsResponse.u8data[5] = (uint8_t)u16ECUSoftwareVersionNum;
				}
				break;

			case LID_SVN_NO:
				if( receivedMessageError == FALSE )
				{
					diagnosticsResponse.u8data[0] = IIL_SVN_NUMBER_BYTES + EXTRA_READ_BYTES;
					for(i = 0; i < IIL_SVN_NUMBER_BYTES;i++)
					{
						diagnosticsResponse.u8data[4+i] = u8IILSvnNo[i];
					}
				}
				break;

			case LID_ECU_Hardware_NUM: // f1 91 EOL tool read
				if( receivedMessageError == FALSE )
				{
					multipleFramesTx.numBytes = TML_CONTAINER_PART_NUMBER_BYTES + EXTRA_READ_BYTES;
					for(i = 0;i < TML_CONTAINER_PART_NUMBER_BYTES;i++)
						dataTxBuffer[3+i] = u8TmlContainerPartNumber[i];
					mulByteResponse = TRUE;
				}
				break;

				default:
					diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
					receivedMessageError = TRUE;
			}			

	}
	else if(singleFrameRx.dataBytes[2]== LID_GEN_BYTE1 )
	{
		switch(singleFrameRx.dataBytes[3] ) //second byte of the LID
		{
			case LID_VEH_OPT: 
				if( receivedMessageError == FALSE )
				{
					VehicleOptionContentVal = GetEolVeicleOptionContentValueInFlash();
					// Reverse fill the u8VsPulsPerRev array
					u8VehicleOptionContent[0] = (VehicleOptionContentVal >> 8) & 0xFF;   // Extract the middle byte
					u8VehicleOptionContent[1] = VehicleOptionContentVal & 0xFF;          // Extract the least significant byte
					diagnosticsResponse.u8data[0] = VEHICLE_TYPE_BYTES + EXTRA_READ_BYTES;
					for(i = 0;i<VEHICLE_TYPE_BYTES;i++)
					{
						diagnosticsResponse.u8data[4+i] = u8VehicleOptionContent[i];
					}
				}

			break;
			case LID_VEH_SPD_PPR:
				{
					VehiclePPR = GetEolSpeedoPulsesInFlash();
					 if(VehiclePPR < SPEEDO_PULSES_MIN_VAL)
					 {
						 VehiclePPR = SPEEDO_PULSES_MIN_VAL;
					 }
					 else if(VehiclePPR > SPEEDO_PULSES_MAX_VAL)
					 {
						 VehiclePPR = SPEEDO_PULSES_MAX_VAL;
					 }
					// Reverse fill the u8VsPulsPerRev array
					u8VsPulsPerRev[0] = (VehiclePPR >> 16) & 0xFF;  // Extract the most significant byte
					u8VsPulsPerRev[1] = (VehiclePPR >> 8) & 0xFF;   // Extract the middle byte
					u8VsPulsPerRev[2] = VehiclePPR & 0xFF;          // Extract the least significant byte
					diagnosticsResponse.u8data[0] = SPEED_SENSOR_BYTES + EXTRA_READ_BYTES;
					for(i = 0;i < SPEED_SENSOR_BYTES;i++)
					{
						diagnosticsResponse.u8data[4+i] = u8VsPulsPerRev[i];
					}
				}
				break;
			default:
				diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
				receivedMessageError = TRUE;
				
		}
	}
	else
	{
		diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ReadDataIdentifier;
		diagnosticsResponse.bDataRdy = TRUE ;
	}
	else if (mulByteResponse == TRUE )
	{
		dataTxBuffer[0] = 0x62;
		dataTxBuffer[1] = singleFrameRx.dataBytes[2];
		dataTxBuffer[2] = singleFrameRx.dataBytes[3];
		formatMulBytesIntoFrames();
	}
	else
	{
		diagnosticsResponse.u8data[1] = 0x62;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
		diagnosticsResponse.u8data[3] = singleFrameRx.dataBytes[3];
		diagnosticsResponse.bDataRdy = TRUE ;
	}
	
			
}


/*********************************************************************//**
 *
 * Read Data dentifier Service Multiple
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void ReadDataByIdentifierServiceMultiple(void)
{
	uint8_t receivedMessageError = FALSE;
	uint8_t mulByteResponse = FALSE;
	uint8_t i,multiByteIndex = 0,numOfIds,j;


	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	numOfIds = ( multipleFramesRx.numBytes - 1 )/2;
	if( numOfIds > NUM_READ_ID_SUPPORTED )
	{
		diagnosticsResponse.u8data[3] = RequestOutOfRange;
		receivedMessageError = TRUE;
	}

	else if(conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	
	multipleFramesTx.numBytes = 1;
	dataTxBuffer[multiByteIndex++] = 0x62;
	for(i=0; i < numOfIds && multiByteIndex < MAX_DATA_LENGTH ; i++)
	{
		if((multipleFramesRx.mulByteDiagRespData[2*i+3])== LID_GEN_BYTE1)
		{
			
		}
		else if((multipleFramesRx.mulByteDiagRespData[2*i+3])== LID_ECU_ID_BYTE1)
		{
			switch(multipleFramesRx.mulByteDiagRespData[2*i+4]) //second u8 of the LID
			{
				case LID_ECU_Hardware_NUM:
				if( receivedMessageError == FALSE )
				{
					multipleFramesTx.numBytes += TML_CONTAINER_PART_NUMBER_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<TML_CONTAINER_PART_NUMBER_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8TmlContainerPartNumber[j];

				}
				break;
				default:
					diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
					receivedMessageError = TRUE;
			}

		}
		
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ReadDataIdentifier;
		diagnosticsResponse.bDataRdy = TRUE ;
	}
	else
	{
		formatMulBytesIntoFrames();
	}
	
			
}


/*********************************************************************//**
 *
 * Write Data Identifier Service Single
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void WriteDataByIdentifierServiceSingle(void)
{
	uint8_t receivedMessageError = FALSE;
	uint32_t VehiclePPR = 0;
	uint16_t VehicleOptionConteValue = 0;
	uint8_t i = 0;

	
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;

	
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	if( activeDiagnosticsSession == eProgrammingSession )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}
	else if ( securityAccessInfo.serverLockStatus == eServerLocked) // check this error conditions with the customer
	{
		diagnosticsResponse.u8data[3] = SecurityAccessDenied;
		receivedMessageError = TRUE;
	}
	else if(singleFrameRx.dataReady == TRUE && singleFrameRx.numBytes > 7 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}  
	

	if(singleFrameRx.dataBytes[2]== LID_GEN_BYTE1 )
	{
		switch(singleFrameRx.dataBytes[3] ) //second byte of the LID
		{
			case LID_VEH_OPT:
			if( receivedMessageError == FALSE )
			{
				if( singleFrameRx.numBytes != VEHICLE_TYPE_BYTES + EXTRA_WRITE_BYTES)
				{
					diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
					receivedMessageError = TRUE;
				}
				else
				{
					for(i = 0;i < VEHICLE_TYPE_BYTES;i++)
					{
						u8VehicleOptionContent[i] = singleFrameRx.dataBytes[4 + i];
					}

					VehicleOptionConteValue = (u8VehicleOptionContent[0] << 8) | (u8VehicleOptionContent[1]);

					HAL_IWDG_Refresh(&hiwdg);
					/* write 0xaa data for memory data curruption checking purpose*/
					WriteByteEEPROM(EOL_VEH_OP_ADD + 0,0xAA);
					WriteByteEEPROM(EOL_VEH_OP_ADD + 3,0xAA);

					WriteByteEEPROM(EOL_VEH_OP_ADD + 1,VehicleOptionConteValue>>8);
					WriteByteEEPROM(EOL_VEH_OP_ADD + 2,VehicleOptionConteValue);
					/* write 0x55 data for memory data curruption checking purpose */
					WriteByteEEPROM(EOL_VEH_OP_ADD + 0,0x55);
					WriteByteEEPROM(EOL_VEH_OP_ADD + 3,0x55);
					HAL_IWDG_Refresh(&hiwdg);

					WriteByteEEPROM(EOL_VEH_OP_ADD + 4,VehicleOptionConteValue>>8);
					WriteByteEEPROM(EOL_VEH_OP_ADD + 5,VehicleOptionConteValue);

				}
			}
			break;

			case LID_VEH_SPD_PPR:
			if( receivedMessageError == FALSE )
			{
				if( singleFrameRx.numBytes != SPEED_SENSOR_BYTES + EXTRA_WRITE_BYTES)
				{
					diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
					receivedMessageError = TRUE;
				}
				else
				{
					for(i = 0;i < SPEED_SENSOR_BYTES;i++)
					{
						u8VsPulsPerRev[i] = singleFrameRx.dataBytes[4+i];
					}

					VehiclePPR = (u8VsPulsPerRev[0] << 16) | (u8VsPulsPerRev[1] << 8) | u8VsPulsPerRev[2];

					if(VehiclePPR < SPEEDO_PULSES_MIN_VAL)
					{
						VehiclePPR = SPEEDO_PULSES_MIN_VAL;
					}
					else if(VehiclePPR > SPEEDO_PULSES_MAX_VAL)
					{
						VehiclePPR = SPEEDO_PULSES_MAX_VAL;
					}

					HAL_IWDG_Refresh(&hiwdg);
					/* write 0xaa data for memory data curruption checking purpose*/
					WriteByteEEPROM(EOL_SPD_PULSE_ADD + 0,0xAA);
					WriteByteEEPROM(EOL_SPD_PULSE_ADD + 4,0xAA);

					WriteByteEEPROM(EOL_SPD_PULSE_ADD + 1,VehiclePPR>>16);
					WriteByteEEPROM(EOL_SPD_PULSE_ADD + 2,VehiclePPR>>8);
					WriteByteEEPROM(EOL_SPD_PULSE_ADD + 3,VehiclePPR);
					/* write 0x55 data for memory data curruption checking purpose */
					WriteByteEEPROM(EOL_SPD_PULSE_ADD + 0,0x55);
					WriteByteEEPROM(EOL_SPD_PULSE_ADD + 4,0x55);
					HAL_IWDG_Refresh(&hiwdg);

					WriteByteEEPROM(EOL_SPD_PULSE_ADD + 5,VehiclePPR>>16);
					WriteByteEEPROM(EOL_SPD_PULSE_ADD + 6,VehiclePPR>>8);
					WriteByteEEPROM(EOL_SPD_PULSE_ADD + 7,VehiclePPR);

				}
			}
			break;
			

			default:
				diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;;
				receivedMessageError = TRUE;

		}
	}
	
	else
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	
	//add the error : general programming failure when eeprom saving returns error
	if(receivedMessageError== FALSE && conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = WriteDatabyIdentifier;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = 0x6E;
		if( multipleFramesRx.dataReady == TRUE )
		{
			diagnosticsResponse.u8data[2] = multipleFramesRx.mulByteDiagRespData[3] ;
			diagnosticsResponse.u8data[3] = multipleFramesRx.mulByteDiagRespData[4] ;

		}
		else
		{
			diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
			diagnosticsResponse.u8data[3] = singleFrameRx.dataBytes[3];
		}
	}
	diagnosticsResponse.bDataRdy = TRUE ;

}

/*********************************************************************//**
 *
 * Encryptoin Data
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

static void getEncryptedData(void)
{
	uint8_t i;
	//remove the first 4 bytes of first frame
	for(i=4;i<20;i++)
		encryptedReceivedData[i-4] = multipleFramesRx.mulByteDiagRespData[i];
	encryptedReceivedData[16]=0;
	
}


/*********************************************************************//**
 *
 * Format Mul Bytes Frames
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

static void formatMulBytesIntoFrames(void)
{
	uint8_t i,j;

	multipleFramesTx.frameInProgress = eIdle;
	multipleFramesTx.status = eSegmentedTxNotStarted;
	multipleFramesTx.minSepTime = MIN_SEP_TIME; //default sepration time is 127 ms
	multipleFramesTx.ID = diagnosticsCANId;
	
	multipleFramesTx.mulByteDiagRespData[0] = 0x10;//msb number bytes are ignored in this byte
	multipleFramesTx.mulByteDiagRespData[1] = (uint8_t)multipleFramesTx.numBytes;
	
	//calculate number of frames
	i = 6;//6 bytes in first frame
	multipleFramesTx.numFrames = 1;//first frame
	while ( multipleFramesTx.numBytes > i )
	{
		multipleFramesTx.numFrames++;
		i=i+7; //seven bytes in each frame
	}
	j = multipleFramesTx.numFrames; //tracks number of frames to be formatted;
	for(i=2;i<8;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-2];
	if(j > 0)
		j--;
	multipleFramesTx.mulByteDiagRespData[8] = 0x21; //first consecutive frame
	for(i=9;i<16 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-3];
	
	if(j > 0)
		j--;
	
	multipleFramesTx.mulByteDiagRespData[16] = 0x22; //second consecutive frame
	for(i=17;i<24 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-4];

	if(j > 0)
		j--;
	
	multipleFramesTx.mulByteDiagRespData[24] = 0x23; //third consecutive frame //not used as yet
	for(i=25;i<32 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-5];

	if(j > 0)
		j--;

	multipleFramesTx.mulByteDiagRespData[32] = 0x24; //fourth consecutive frame //not used as yet
	for(i=33;i<40 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-6];

	if(j > 0)
		j--;

	multipleFramesTx.mulByteDiagRespData[40] = 0x25; //fifth consecutive frame //not used as yet
	for(i=41;i<48 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-7];

	if(j > 0)
		j--;

	multipleFramesTx.mulByteDiagRespData[48] = 0x26; //sixth consecutive frame //not used as yet
	for(i=49;i<56 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-8];
	
	if(testerAddress == REQ_PHYSICAL_SA1 )
		multipleFramesTx.ID |= REQ_PHYSICAL_SA1_L;
	else
		multipleFramesTx.ID |= REQ_PHYSICAL_SA2_L;
	
	multipleFramesTx.ID |= PREFERRED_SA;
	multipleFramesTx.dataReady = TRUE;

}


/*********************************************************************//**
 *
 * send Diagnostics Response
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void sendDiagnosticsResponse (void)
{
	uint8_t i;
	static uint8_t dataByteIndex;
	uint32_t ID;

	//if (commInfo.noResponse == TRUE || !HSCAN_GetStateTX())
	if (commInfo.noResponse == TRUE )
		return;
	
	if( diagnosticsResponse.bDataRdy == TRUE )
	{
		diagnosticsResponse.sMsgID.Identifier = diagnosticsCANId; 
		if(testerAddress == REQ_PHYSICAL_SA1 )
			diagnosticsResponse.sMsgID.Identifier |= REQ_PHYSICAL_SA1_L;
		else
			diagnosticsResponse.sMsgID.Identifier |= REQ_PHYSICAL_SA2_L;

		diagnosticsResponse.sMsgID.Identifier |= PREFERRED_SA;
		
		//vTxMsgBufProcessing(diagnosticsResponse.sMsgID.Identifier,8,diagnosticsResponse.u8data,UDS_TX);
		TxHeader.DataLength = 8u;
		TxHeader.Identifier = diagnosticsResponse.sMsgID.Identifier;
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&diagnosticsResponse.u8data);
		diagnosticsResponse.bDataRdy = FALSE;
		//if reset diagnosticsTimer here then delay counter will be reset in case of  wrong message in tester present also
		//diagnosticsTimer = clock1msTicker;
		for(i = 0;i < 8;i++)
		{
			diagnosticsResponse.u8data[i] = 0;
			singleFrameRx.dataBytes[i]=0;
		}
	}

	if (multipleFramesTx.dataReady == TRUE )
	{
		
		// sending first frame
		if(	multipleFramesTx.frameInProgress == eIdle && multipleFramesTx.status == eSegmentedTxNotStarted)
		{
			dataByteIndex = 0;
			//vTxMsgBufProcessing(multipleFramesTx.ID,8,&multipleFramesTx.mulByteDiagRespData[dataByteIndex],UDS_TX);
			TxHeader.DataLength = 8u;
			TxHeader.Identifier = multipleFramesTx.ID;
			HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&multipleFramesTx.mulByteDiagRespData[dataByteIndex]);
			diagnosticsTimer = u32GetClockTicks();
			
			multipleFramesTx.serialNum++;
			dataByteIndex = 8 * multipleFramesTx.serialNum;
			multipleFramesTx.timeStamp = u32GetClockTicks();
			multipleFramesTx.status = eWait;
			multipleFramesTx.frameInProgress = eFirst;
		}
		// sending consecutive frames
		// min separation time over & has the flow control frame from receiver cleared to send
		else if ( ( u32GetClockTicks() - multipleFramesTx.timeStamp ) > multipleFramesTx.minSepTime 
			&& (multipleFramesTx.status == eClearToSend))
		{
			// if blocksize is zero, send all the frames
			if(!multipleFramesTx.blockSize)
			{
				if ( multipleFramesTx.serialNum < multipleFramesTx.numFrames )
				{
					//vTxMsgBufProcessing(multipleFramesTx.ID,8,&multipleFramesTx.mulByteDiagRespData[dataByteIndex],UDS_TX);
					TxHeader.DataLength = 8u;
					TxHeader.Identifier = multipleFramesTx.ID;
					HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&multipleFramesTx.mulByteDiagRespData[dataByteIndex]);
					diagnosticsTimer = u32GetClockTicks();
					multipleFramesTx.serialNum++;
					dataByteIndex = 8 * multipleFramesTx.serialNum;
					multipleFramesTx.timeStamp = u32GetClockTicks();
					multipleFramesTx.frameInProgress = eConsecutive;
					//this has to be there for system with longer main loop:otherwise  multiframe messages are not received
					if(multipleFramesTx.serialNum >= multipleFramesTx.numFrames )
					{
						multipleFramesTx.frameInProgress = eTxCompleted;
						multipleFramesTx.status = eStopped;
						multipleFramesTx.dataReady = FALSE;
					} 
				}//this may not be required since it has been done in the if loop above but let it stay
				else //if all the frames have been sent, stop sending
				{
					multipleFramesTx.frameInProgress = eTxCompleted;
					multipleFramesTx.status = eStopped;
					multipleFramesTx.dataReady = FALSE;
				}
			}//if blocksize has been specified by the receiver, send only that number of frames
			else if (	 multipleFramesTx.serialNum < multipleFramesTx.blockSize 
				&& multipleFramesTx.blockSize <= multipleFramesTx.numFrames
				)
			{
				dataByteIndex = 8 * multipleFramesTx.serialNum;				
				//vTxMsgBufProcessing(multipleFramesTx.ID,8,&multipleFramesTx.mulByteDiagRespData[dataByteIndex],UDS_TX);
				TxHeader.DataLength = 8u;
				TxHeader.Identifier = multipleFramesTx.ID;
				HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&multipleFramesTx.mulByteDiagRespData[dataByteIndex]);
				diagnosticsTimer = u32GetClockTicks();
				multipleFramesTx.serialNum++;
				dataByteIndex = 8 * multipleFramesTx.serialNum;
				multipleFramesTx.timeStamp = u32GetClockTicks();
				multipleFramesTx.frameInProgress = eConsecutive;
				
				if(multipleFramesTx.blockSize == multipleFramesTx.numFrames)//if all the frames have been sent, stop sending
				{
					multipleFramesTx.frameInProgress = eTxCompleted;
					multipleFramesTx.status = eStopped;
					multipleFramesTx.dataReady = FALSE;
				}
			}
			//wait for next flow control
			else 
			{
				//multipleFramesTx.waitForResponse = TRUE;
				multipleFramesTx.status = eWait;
				multipleFramesTx.timeStamp = u32GetClockTicks();
			}
		}
		
		//if time is out since last consecutive frame was received,  stop sending
		else if (( u32GetClockTicks() - multipleFramesTx.timeStamp ) > NEXT_CAN_FRAME_TIMEOUT 
			|| multipleFramesTx.status == eOverflowAndTerminate )
		{
			multipleFramesTx.frameInProgress = eTerminate;
			multipleFramesTx.status = eStopped;
			multipleFramesTx.dataReady = FALSE;
		}
	

	}// if all the frames were sent or if data overflow has occured, reset the structure to stop sending 
	else if ( (( dataByteIndex >= MAX_DATA_LENGTH ) || multipleFramesTx.frameInProgress >= eTxCompleted ) 
		|| (multipleFramesTx.status == eWait && (u32GetClockTicks() - multipleFramesTx.timeStamp) >  NEXT_CAN_FRAME_TIMEOUT )
	)
	{
		multipleFramesTx.frameInProgress = eIdle;
		for(i=0;i< MAX_LENGTH; i++)
		{
			multipleFramesTx.mulByteDiagRespData[i]=0;
			dataTxBuffer[i]=0;
		}
		
		//multipleFramesTx.waitForResponse = FALSE;
		// if the request message was sent but no message was received for more than NEXT_CAN_FRAME_TIMEOUT
		multipleFramesTx.status = eSegmentedTxNotStarted;
		multipleFramesTx.serialNum = 0;
		//if( securitySequence == eRequestSeed )
			//securitySequence = eNone;
	}
	if(multipleFramesRx.frameInProgress == eFirst && multipleFramesRx.status == eWait)
	{
		if(testerAddress == REQ_PHYSICAL_SA1 )
			ID = multipleFramesRx.ID | REQ_PHYSICAL_SA1_L;
		else
			ID = multipleFramesRx.ID | REQ_PHYSICAL_SA2_L;

		ID |= PREFERRED_SA;
		frameConrolDataToSend[0] = 0x30;	
		//vTxMsgBufProcessing(ID,8,&frameConrolDataToSend,UDS_TX);
		TxHeader.DataLength = 8u;
		TxHeader.Identifier = ID;
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&frameConrolDataToSend);
		multipleFramesRx.status = eClearToSend;
		diagnosticsTimer = u32GetClockTicks();
	}
	else if(multipleFramesRx.frameInProgress == eTerminate && multipleFramesRx.status == eOverflowAndTerminate)
	{
		diagnosticsResponse.sMsgID.Identifier = multipleFramesRx.ID; 
		if(testerAddress == REQ_PHYSICAL_SA1 )
			diagnosticsResponse.sMsgID.Identifier |= REQ_PHYSICAL_SA1_L;
		else
			diagnosticsResponse.sMsgID.Identifier |= REQ_PHYSICAL_SA2_L;

		diagnosticsResponse.sMsgID.Identifier |= PREFERRED_SA;
		
		frameConrolDataToSend[0] = 0x32;//overflow sent in Frame control frame
		//vTxMsgBufProcessing(diagnosticsResponse.sMsgID.Identifier,8,&frameConrolDataToSend,UDS_TX);
		TxHeader.DataLength = 8u;
		TxHeader.Identifier = diagnosticsResponse.sMsgID.Identifier;
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&frameConrolDataToSend);
		multipleFramesRx.status = eClearToSend;
		diagnosticsTimer = u32GetClockTicks();
	}

	if (multipleFramesRx.frameInProgress >= eTxCompleted )
	{
		//data has been processed, so clear the buffer
		for(i=0;i< MAX_LENGTH; i++)
			multipleFramesRx.mulByteDiagRespData[i]=0;
		multipleFramesRx.frameInProgress =  eIdle;
		multipleFramesRx.status = eReserved;
		multipleFramesRx.serialNum = 0;
		multipleFramesRx.dataReady = FALSE;
		for(i=0;i<8;i++)
			singleFrameRx.dataBytes[i]=0;
	}
	
}


/*********************************************************************//**
 * init Diagnostics Session
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

//Called in Events:: CPU_Reset
void initDiagnosticsSession(void)
{
	setStatusDefaultSession(); //set to diagnostics to default session if watchdog reset has occured
}


/*********************************************************************//**
 * Reset Diagnostic Session call this after booting is over
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void resetDiagnosticSession(void)
{
	activeDiagnosticsSession = eDefaultSession;

}

/*********************************************************************//**
 *
 * Set Status Default Session
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void setStatusDefaultSession(void)
{
	activeDiagnosticsSession = eDefaultSession;

}

/*********************************************************************//**
 *check session Time out
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void sessionTimeout(void)
{
	//after 3 failed attempts,  after waiting for 10s, reset all the parameters to allow fresh access
	if( securityAccessInfo.securityDelayOn && (u32GetClockTicks() - securityAccessInfo.securityAccessDelay > SECURITY_ACCESS_DELAY)) 
	{
		securitySessionTimeOutInit();		
	}
	//if there is no message exchange
	if(u32GetClockTicks() - diagnosticsTimer > NO_MESSAGE_TIMEOUT )
	{
		activeDiagnosticsSession = eDefaultSession;
		securitySessionTimeOutInit();
	}
	
}

/*********************************************************************//**
 *security Session Time Out Init
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void securitySessionTimeOutInit(void)
{
	uint8_t i;
	
	securityAccessInfo.securityDelayOn = FALSE;
	multipleFramesTx.dataReady = FALSE;
	multipleFramesTx.frameInProgress = eIdle;
	securityAccessInfo.serverLockStatus = eServerLocked;
	securityAccessInfo.securitySequence = eNone; //RequestSeed msg must come again,
	securityAccessInfo.securityAccessAttempts = 0;
	for(i=0;i< MAX_LENGTH; i++)
	{
		multipleFramesTx.mulByteDiagRespData[i]=0;
		dataTxBuffer[i]=0;
		multipleFramesRx.mulByteDiagRespData[i]=0;
	}
	multipleFramesTx.frameInProgress = multipleFramesRx.frameInProgress =  eIdle;
	multipleFramesTx.status  = eSegmentedTxNotStarted;
	multipleFramesTx.serialNum = multipleFramesRx.serialNum = 0;
	multipleFramesTx.dataReady = multipleFramesRx.dataReady = FALSE;
	multipleFramesRx.status = eReserved;

	
}

/*********************************************************************//**
 *
 * ECU rest with time
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

uint8_t isItTimeToReset(void)
{
	uint16_t time;
	
    time =  setTimeForReset * 1000;
    if(setTimeForReset > 0 && (u32GetClockTicks() - resetTimeTracker > (time/2)))
    {
		return TRUE;
    }
	else
	{
		return FALSE;
	}
		
}

/*********************************************************************//**
 *
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void diagnosticDataInit(void)
{
	securitySessionTimeOutInit();
	conditionNotCorrect = FALSE;
	testerAddress = REQ_PHYSICAL_SA1;
	//setTimeForReset = 0; // this will be initialized on wtd reset anyway
	
}

uint16_t u16GetRoutineControlServiceStatus(void)
{
	return RoutineControlStatusPending;
}

uint8_t bIsECUResetByService(void)
{
	return (bECUResetServiceActive);
}

/*********************************************************************//**
 *
 * Set Condition not Correct
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void setConditionNotCorrect(uint8_t status)
{
	conditionNotCorrect = status;
}

/*********************************************************************//**
 *
 * Get Condition  Not Correct
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

uint8_t getConditionNotCorrect(void)
{
	return conditionNotCorrect;
}

/*********************************************************************//**
 *
 * Set CriticalFunctionStatus
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void setCriticalFunctionStatus(uint8_t status)
{
	criticalFunctionStatus = status;
}

/*********************************************************************//**
 * Get CriticalFunctionStatus
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

uint8_t getCriticalFunctionStatus(void)
{
	return criticalFunctionStatus;
}


uint16_t u16getSoftwareVersion(void)
{
	return (u16ECUSoftwareVersionNum);
}

uint16_t u16GetFotaSoftwareID(void)
{
	//return (FotaSwId);
}


/* END TML_14229_UDS */
