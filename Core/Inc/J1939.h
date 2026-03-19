/****************************************************************************************************************/
/* COPYRIGHT                                                                                                    */
/* Inidication Instruments Ltd.(c) 2020                                                                         */
/****************************************************************************************************************/
/* File name   : {J1939.h}                                                                                    */
/* Module      : {D:\GIT_ServerRepo\PVPL_OBDII\Code\PVPL_OBD2_RL78_48pin\ApplicationFiles\SAE_J1939\J1939.h}                                                                                    */
/*--------------------------------------------------------------------------------------------------------------*/
/* Description :  For J1939 CAN msg use                                                                                              */
/****************************************************************************************************************/


#ifndef _J1939_H_
#define _J1939_H_


/****************************************************************************************************************/
/* File Includes                                                                                                */
/****************************************************************************************************************/
#include "main.h"
#include "J1939.h"

/****************************************************************************************************************/
/* Constant Defines                                                                                             */
/****************************************************************************************************************/
#define NO_OF_DTC_CODE        		70U //maximum is 64 DTCs
#define	MSGBUF_BASE_ADD	((uint32_t)(0x0F0600))

#define BADDATA						0xFEU 				
#define ERRORDATA					0XFFU

#define BADDATA_2BYTE				0xFE00U 
#define BADDATA_4BYTE				0xFE000000U

#define TEMP_UND_RANGE            	0x64U    //J1939 value at temp 60 deg 
#define TEMP_OVR_RANGE            	0xA1U    //J1939 value at temp >120 deg 

#define OAT_UND_RANGE            	0x2220U    //J1939 value at 0 deg temp
#define OAT_OVR_RANGE            	99U    	  // 99 deg Temp

#define RX_QUE_SIZE      			30U		//Buffer for receive CAN msg
#define ERR_OK           			0U		//Not used            

// (Not used ) Required ISO PGN List 
#define PGN60928                60928U           // TxRx 0xEE00 ISO Adress claim
#define PGN59392                59392U           // TxRx 0xE800 ISO Acknowledgement

// (Not used ) Proprietary PGN List
#define PGN_USER_INTERFACE      65401U           // Rx Proprietary PGN used for calibration
#define PGN_USER_INTERFACE_ext  0x18FF7900U      // 29 bit Tx Proprietary used for calibration

#define J1939_NULL_ADDRESS    	254U

#define J1939_GLOBAL_ADDRESS    255U
#define J1939_MAX_PF_PDU1FORMAT 239U             // From J1939-21 section 5.2.5 "PDU Specific (PS)
                                                 // PDU1 range is 0-239 and is for PDU destination format
                                                 // PDU2 range is 240-255 and is for PDU group extension values
//PGN transmmit for request
#define PGN65257				65257U			// AFE
#define PGN65257_ext            0x18FEE900U		// 29 bit ID priority 7
#define PGN65219				65219U			// TRANSMISSION RANGE LOW
#define PGN65219_ext            0x1CFEC300U		// 29 bit ID priority 7
#define PGN65088                65088U			// Trailer
#define PGN65088_ext            0x18FE4000U     // 29 bit ID priority 6


//Request PGN
#define PGN59904                59904      		// Request PGN
#define PGN65303                65303           // Tx Proprietary PGN used to answer the PGN request
#define PGN65303_ext            0x18FF1700      // 29 bit Tx Proprietary used to answer the PGN request


#define MAX_LENGTH					255
#define MAX_DATA_LENGTH	 			MAX_LENGTH + 20

#define REQ_FUNCTIONAL_SA       		0x33            // TATA Diagnostics Specs
#define REQ_PHYSICAL_SA1        		0xF1
#define REQ_PHYSICAL_SA2        		0xF9
#define REQ_PHYSICAL_SA1_L        		0xF100
#define REQ_PHYSICAL_SA2_L        		0xF900
#define RESPONSE_PF1        			0xDA
#define RESPONSE_PF2        			0xDB

#define PREFERRED_SA          	 0x17// 0xc8            // J1939 TABLE B2..B9: Instrument cluster
/****************************************************************************************************************/
/* Enumeration types                                                                                            */
/****************************************************************************************************************/
typedef enum
{
	eJSoftVersionDataTx = 0U,
	eNumofTxDataTypes
} eTxDataType;

typedef enum
{
	eTx65303 = 0U,	
	eNoOfPgnTx	
} tPGN_TxIndexerType;


typedef enum
{
    eJTachoMeter = 0U,
    eJEngineCoolLevel,
	eJDefGauge,
	eEngineOilPressureGauge,
	eSpeedoMeterCAN,
	eTransmissionOilTemperatureVal,
	eSCR_TempRingLampTFT,
	eHourMeter_AshokEicherEng,
	eHourMeter_TATA_Eng,
	eShootLoad,
	eBattVolt,
	ePoorDefQualityLampDial,
	eLowAdBlueLevelLampTFT,
	eHestLampOnDial,
	eEngineStopLamp,
	eWaterInFuelLamp,
	eDpfLampIndicationLampSPN_3697,
	eGlowtimeIndicationLamp_KoelEngine,
	eGlowtimeIndicationLamp_MahindraEngine,
	eDpfRegenInhibitLampSPN_3703,
    eJMilSPN1213,
    eJMilSPN3038,
    eJMilDM1_1213,
    eJMilDM1_3038,
    eJMil2_SysOkSPN987,
    eJMil2_SysOkSPN3041,
    eJMil2_SysOkDM1_987,
    eJMil2_SysOkDM1_3041,
    eJDM1DTC_spn1,
    eJDM1DTC_spn2,
    eJDM1DTC_fmi,
	eDieselParticulateFilter,
	eSootMass,
	eMIL_DLCC1_64775,
	ePopupMessageSPN_4175,
	ePopupMessageSPN_3700,
	ePopupMessageSPN_3704,
	ePopupMessageSPN_3705,
	ePopupMessageSPN_3707,
	ePopupMessageSPN_3708,
	ePopupMessageSPN_3710,
	ePopupMessageSPN_3711,
	ePopupMessageSPN_3712,
	ePopupMessageSPN_3716,
	eJMil2_PCDlampSPN624,
	eJMil2_PCDlampSPN3040,
	eJMil2_PCDlampDM1_624,
	eJMil2_PCDlampDM1_3040,
    eNumofRxDataTypes  
} eRxDataType;

typedef enum
{
	eRx61444 = 0U,
	eRx65262,	// Coolent temp
	eRx65110,	// Def,Engine Stop Lamp
	eRx65263,  	// Engine Oil press
	eRx65265,	// Speedo CAN
	eRx65272,	// Transmission Oil Temperature Value
	eRx65383,	// Engine Oil Pressure Lamp , SCR tempering,Poor DEF Quality
	eRx65253,	// eHourMeter_AshokEicherEng
	eRx65255,	// eHourMeter_TATA_Eng
	eRx64891,   // eShootLoad
	eRx65271,	// Batt volt
	eRx65279,	// Low Ad-Blue Level lamp
	eRx64892,	// DPF Regeneration lamp
	eRx64966,	// eGlowtimeIndicationLamp_KoelEngine
	eRx65252,	// eGlowtimeIndicationLamp_MahindraEngine
	eRx65226,
	eRx60160,
	eRx64946,
	eRx64796,
	eRx64775,
	eNoOfPgnRx
} tPGN_RxIndexerType;                                                

/****************************************************************************************************************/
/* Type definitions                                                                                             */
/****************************************************************************************************************/

// Identifier format
// bits | 28 27 26 | 25 | 24 | 23 22 21 20 19 18 17 16 | 15 14 13 12 11 10 09 08 | 07 06 05 04 03 02 01 00 |
//      | priority | R  | DP | PDU format (PF)         | PDU specific (PS)       | Source address          |
//                 |<-------------------------- PGN Id ------------------------->|

//NEC MCU have a architecture little endian so change stucture according to it.
typedef union
{
    uint32_t  Identifier;
    struct
    {
		uint8_t SourceAddress;            	// 0 - 7
		uint8_t PduSpecific;              	// 8 - 15
		uint8_t PduFormat;                	// 16 - 23
        struct
        {
            uint8_t DataPage       :1;    // 24
            uint8_t Reserved       :1;    // 25
            uint8_t Priority       :3;    // 26 - 28
            uint8_t UnusedMSB      :3;    // 29 - 31
        } sBitField;
        
    } sIdField;
} sJ1939_Id_Struct_Type;

// J1939 message buffer structure
typedef struct
{
    sJ1939_Id_Struct_Type   sMsgID;           // Message ID
    uint32_t                u32TimeStamp;     // Time of message reception
    uint32_t                u32ValidDataTS;   // Time stamp of last valid data
                                              // The message processor will copy the timestamp into this variable
                                              // if at least one of the datums was valid
    uint8_t                 bDataRdy;         // True = fresh data for processing
    uint8_t                 ucNumofRxBytes;   // 8 or less received
    uint8_t                 u8data[8];
	uint8_t				 	bDataOutDate;
} sJ1939_RX_MSG_TYPE;

typedef struct 
{
	uint32_t SPNValue;
	uint8_t FMICode;
}DTCdata;


extern sJ1939_RX_MSG_TYPE   sJ1939RxQueue[RX_QUE_SIZE];
extern sJ1939_RX_MSG_TYPE   J1939RxData[eNoOfPgnRx];


/****************************************************************************************************************/
/* Global function declarations                                                                                 */
/****************************************************************************************************************/

/// @addtogroup J1939_LIBRARY
/// @{
 
/// @defgroup J1939HeaderLib Global functions used in j1939 file
//// 
///   
/// @addtogroup J1939HeaderLib
/// @{

void vResetDTCParameters(void);
uint8_t u8GetDTCStatus(void);
uint8_t u8GetDTCCount(void);
uint8_t u8GetDTCSA(void);
uint8_t u8GetDtcFramNum();
uint16_t u16GetDtcTotalBytes();
void vJ1939Init(void);
void vJ1939_ServiceManager(void);
uint8_t bProcessISORequest(uint32_t PGN);
void vPutPGNdata(eTxDataType dtype, uint32_t rawData, int16_t overrange);
uint8_t u8GetBitData(eRxDataType dType);
uint32_t u32GetPGNdata(eRxDataType dType);
void vJ1939OnFullRx(void);
uint8_t bGetPGNTimeOut(eRxDataType dType);
uint8_t bGetPGNDataReady(eRxDataType dType);
void ClearPGNDataReady(eRxDataType dType);
void vJ1939_TxMsgEnableDisable(tPGN_TxIndexerType eTxPgn, uint8_t EnableFlag);
void vJ1939TxInit(void);
void vClearDTC(void);
uint8_t bGetSPNDataReady(eRxDataType dType);
void vClearSPNDataReady(eRxDataType dtype);
void vJ1939_TxSendOnDemandEnable(tPGN_TxIndexerType eTxPgn );
void vJ1939_TxSendOnDemandDisable(tPGN_TxIndexerType eTxPgn );


/// @} // endgroup J1939HeaderLib

/// @} // endgroup J1939_LIBRARY


#endif

