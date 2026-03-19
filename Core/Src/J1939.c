
/****************************************************************************************************************/
/* COPYRIGHT                                                                                                    */
/* Inidication Instruments Ltd.(c) 2020                                                                         */
/****************************************************************************************************************/
/* File name   : {J1939.c}                                                                                    */
/* Module      : {D:\GIT_ServerRepo\PVPL_OBDII\Code\PVPL_OBD2_RL78_48pin\ApplicationFiles\SAE_J1939\J1939.c}                                                                                    */
/*--------------------------------------------------------------------------------------------------------------*/
/* Description :  In this file , All J1939 CAN message processed                                                                                              */
/****************************************************************************************************************/

/****************************************************************************************************************/
/* Includes                                                                                                     */
/****************************************************************************************************************/
#include "J1939.h" 
#include "Main.h"
#include "P15765H.h"
#include "TML_14229_UDSH.h"


/****************************************************************************************************************/
/****************************************************************************************************************/
/* Defines                                                                                                      */
/****************************************************************************************************************/
#define PGN65281				65281U
#define PGN65282				65282U



#define PGN60416				60416U //TP_CM 
#define PGN60160				60160U //TP_DT
#define PGN60160_0x03			60160U //TP_DT
#define PGN60160_0x10			60160U //TP_DT
#define PGN60160_0x0B			60160U //TP_DT
#define PGN60160_0x33			60160U //TP_DT

#define PGN65226_0				0xCAU				
#define PGN65226_1				0xFEU
#define PGN65226_2				0U



//Transmmiting PGN
#define PGN65276				65276U
#define PGN65276_ext			0x18FEFC17U

#define PGN65426				65426U
#define PGN65426_ext			0x0CFF9247U

#define PGN61444  				61444U
#define PGN65262 				65262U

#define PGN64796				64796U

#define PGN65110				65110U
#define PGN65263  				65263U
#define PGN65265				65265U
#define PGN65272				65272U
#define PGN65383				65383U
#define PGN65253				65253U
#define PGN65255				65255U
#define PGN64891  				64891U
#define PGN65271				65271U

#define	PGN65279				65279U
#define	PGN64892				64892U
#define	PGN64966				64966U
#define	PGN65252				65252U

#define	PGN65226				65226U
#define PGN64775				64775U


#define PGN64946				64946U

//Source Address macro
#define EMS_SA1_EXT						0x00U
#define BCM_SA_EXT						0x21U
#define ABS_SA_EXT						0x0BU
#define TCU_SA_EXT						0x03U
#define RETARDER_ECU_SA_EXT				0x10U
#define TPMS_SA_EXT						0x33U
#define ATC_ECU_SA_EXT					0xA0U



#define RTS_TIMEOUT						750U
#define BAM_TIMEOUT						375U

//ABS malfunction trailer
//Exhaust break PGN TBD	
#define BITDATA                     	0xFFU    // indicates bit data
#define SERVICE_INTERVAL            	1U      // service time interval in msec
#define SEND_ON_DEMAND              	0U
#define MAX_OVERRANGE 					20U    // maximum percent overrange


/****************************************************************************************************************/
/* Enumeration                                                                                                  */
/***************************************************************************************************************/
typedef enum
{
	ePGN_J1939 = 0U,
	ePGN_NMEA,
	ePGN_FAST_PACKET
} PGN_TYPE;

typedef enum
{
	eEmsSA = 0U,
	eRetarderEcuSA,
	eAbsSA,
	eTcuSA,
	eTpmsSA,
	NumberOfSA
} eSourceAddType;

typedef enum
{
	eNoNewMultiPacket,
	eTPCMRTS,
	eTPCMBAM,
	eTPDT
}MULTI_PACKET_STATE;


/****************************************************************************************************************/
/* Local variables                                                                                              */
/****************************************************************************************************************/
//variables used for multipacket data
// multipacket timestamp holds time of last frame reception
static uint32_t u32MultiPacketTimer[NumberOfSA];
//indicates if multipacket frames are being received
static uint8_t u8MultiPacketDM1[NumberOfSA] = {eNoNewMultiPacket,eNoNewMultiPacket,eNoNewMultiPacket,eNoNewMultiPacket,eNoNewMultiPacket};
//holds the address of multipacket being processed
static uint8_t u8MultiFrameSA[NumberOfSA] = {J1939_NULL_ADDRESS,J1939_NULL_ADDRESS,J1939_NULL_ADDRESS,J1939_NULL_ADDRESS,J1939_NULL_ADDRESS};
// number of frames in the multipacket being processed
static uint8_t u8DM1NumFrames = 0U;
//TRUE if fresh DM1 DTCs have been received
static uint8_t u8DTCready = FALSE;
//source address of the ECU that sent DTCs
static uint8_t u8DTCSA = J1939_NULL_ADDRESS;
//number of DTCs in DM1
static uint8_t u8DTCCount = 0U;
static uint16_t u16lTotalBytes = 0U;


//sJ1939_RX_MSG_TYPE diagnosticsBuf[RX_QUE_SIZE];


//uint8_t testerAddress;
//uint32_t diagnosticsTimer;
/****************************************************************************************************************/
/* Type definitions                                                                                             */
/****************************************************************************************************************/
typedef struct
{
	uint8_t	bEnabled;          // not related to address claim
	uint8_t    bSendOnDemand;     // used used for on demand and retry output
	uint8_t    u8data[8];         // Data buffer
	uint32_t   u32TimeStamp;      // Last output tim	
	
} J1939_TX_DATA_TYPE;

typedef struct
{
	uint8_t    u8data[8];
	uint32_t   u32TimeStamp;     	// Time of message reception
} J1939_RX_DATA_TYPE;

typedef struct
{
	uint32_t	PGN_ext;        // J1939 29 bit ID
	uint32_t	timeInterval;   // If != 0 then periodic output interval in msec.
	                    // If == 0, then output is on demand rather than periodic.
} J1939_TX_TYPE;

typedef struct
{
	uint8_t	PGNindex;           // index into sPGNconstantsTx
	uint8_t	byteIndex;          // index into the PGN message data
	uint8_t	byteSize;           // number of bytes of data
	uint8_t	bitMask;            // useful only for bit data
	int16_t	dataOffset;         // j1939 data offset
	uint32_t	bitRes;             // j1939 bit resolution
	uint32_t	overrangeData;      // data to output if calculation results in a value beyond acceptable range
	uint32_t	underrangeData;      // data to output if calculation results in a value beyond acceptable range
	uint32_t	u32TimeStamp;
} PGNdataType;


//Can transmit not implement 
typedef struct
{
	uint16_t    PGN;
	uint32_t    ID;         // 29 bit ID
	PGN_TYPE    PGNtype;
	uint16_t    rate;       // in msec
} tPGNconstTxType;                                      


const tPGNconstTxType sPGNconstantsTx[eNoOfPgnTx] = 
{                                                     
	//  PGN         	ID              	PGNtype         rate     	  
	{   PGN65303,   	PGN65303_ext,   	ePGN_J1939,      SEND_ON_DEMAND   },  
};

// The bit resolution in this case is the reciprocal of the
const PGNdataType sPGNdataTx[eNumofTxDataTypes] =                                     /* J1939 bit resolution multiplied by 100 for integer math */
{                                                                   
	//  PGNIndex    	byteIndex   byteSize    bitMask     dataOffset  bitRes  overrangeData  		
	{   eTx65303,   	0U,          4U,          0x00U,       0U,         	1U, 		0xFFU	},		// Software Versio		   
};


typedef struct
{
	uint16_t		PGN;
	uint8_t u8SourceAdd;
	void	(*RxFunct)(sJ1939_RX_MSG_TYPE *);    // pointer to PGN processing method
	
} tPGNsonstRxType;

const PGNdataType sPGNdataRx[eNumofRxDataTypes] =
{
	//  PGNindex    byteIndex  byteSize   		bitMask     dataOffset  	bitRes      overrangeData underrange   		In ms            
	{   eRx61444,   	3U,          2U,          0x00U,       0,          1U,   	    BADDATA_2BYTE,		0U,			2500U	},   	/* RPM	Gauge */
	{   eRx65262,   	0U,          1U,          0x00U,       0,          1U,   		BADDATA,		0U,				2500U	},   	/* Cool Temp Gauge	*/
	{   eRx65110,   	0U,          1U,          0x00U,       0,          1U,   		BADDATA,		0U,				2500U	},   	/* Def Gauge	*/
	{   eRx65263,   	3U,          1U,          0x00U,       0,          1U,   		BADDATA,		0U,				2500U	},   	/* Engine Oil press GAuge	*/
	{   eRx65265,   	1U,          2U,          0x00U,       0,          1U,   		BADDATA_2BYTE,		0U,			2500U	},   	/* Speedo CAN	*/
	{   eRx65272,   	4U,          2U,          0x00U,       0,          1U,   		BADDATA_2BYTE,		0U,			2500U	},   	/* Transmission Oil Temperature Value	*/
	{   eRx65383,   	1U,          1U,          0xF8U,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* eSCR_TempRingLampTFT	*/
	{   eRx65253,   	0U,          4U,          0x00U,       0,          1U,   		BADDATA_4BYTE,		0U,			35000U	},   	/* Hour Ashok & Eicher Engine	*/
	{   eRx65255,   	0U,          4U,          0x00U,       0,          1U,   		BADDATA_4BYTE,		0U,			35000U	},  	 /* Hour meter TATA Engine	*/
	{   eRx64891,   	0U,          1U,          0x00U,       0,          1U,   		BADDATA,		0U,				5000U	},   	/* Shoot Load */
	{   eRx65271,   	6U,          2U,          0x00U,       0,          1U,   		BADDATA_2BYTE,		0U,			2500U	},   	/* BATT volt */
	{   eRx65383,   	0U,          1U,          0xC7U,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* ePoorDefQualityLampDial	*/
	{   eRx65279,   	1U,          1U,          0xF8U,       0,          BITDATA,   	BADDATA,		0U,				25000U	},   	/* eLowAdBlueLevelLampTFT	*/
	{   eRx64892,   	6U,          1U,          0xE3U,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* eHestLampOnDial	SPN 3698*/
	{   eRx65110,   	5U,          1U,          0x1FU,       0,          BITDATA,   	BADDATA,		0U,				2500U	},   	/* eEngineStopLamp	*/
	{   eRx65279,   	0U,          1U,          0xFCU,       0,          BITDATA,   	BADDATA,		0U,				25000U	},   	/* eWaterInFuelLamp	*/
	{   eRx64892,   	0U,          1U,          0xF8U,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* eDpfLampIndicationLamp on dial SPN 3697 3 bits*/
	{   eRx64966,   	0U,          1U,          0xFCU,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* eGlowtimeIndicationLamp_KoelEngine	*/
	{   eRx65252,   	3U,          1U,          0xFCU,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* eGlowtimeIndicationLamp_MahindraEngine	*/
	{   eRx64892,   	2U,          1U,          0xF3U,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* eDpfRegenInhibitLamp,	SPN 3703 */
	{   eRx65226,   	0U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 11000000    MIL          	        			SPN 1213   1.7
	{   eRx65226,   	1U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //	 bits 11000000    MIL FLASH  	      	    		SPN 3038   2.7
	{   eRx60160,   	1U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 11000000    MIL          	        			SPN 1213   2.7
	{   eRx60160,   	2U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 11000000    MIL FLASH  	      	    		SPN 3038   3.7
	{   eRx65226,   	0U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00000011    Check ENG or SysOk         	        			SPN 987   1.7
	{   eRx65226,   	1U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //	 bits 00000011    Check ENG or SysOk FLASH  	      	    		SPN 3041   2.7
	{   eRx60160,   	1U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 00000011    Check ENG or SysOk          	        			SPN 987   2.7
	{   eRx60160,   	2U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 00000011    Check ENG  or SysOk FLASH
	{   eRx65226,   	2U,          1U,          0x00U,       0,          1U ,  	   BADDATA_4BYTE,	0U,				5000U	},   //  bytes 2-4        dtc SPN1
	{   eRx65226,   	3U,          1U,          0x00U,       0,          1U ,  	   BADDATA_4BYTE,	0U,				5000U	},   //  bytes 2-4        dtc SPN2
	{   eRx65226,   	4U,          1U,          0x00U,       0,          1U ,  	   BADDATA_4BYTE,	0U,				5000U	},   //  bytes 2-4        dtc fmi
	{   eRx64946,   	4U,          2U,          0x00U,       0,          1U ,  	   BADDATA_2BYTE,	0U,				2500U	},   // iesel Particulate Filter Differential Pressure  x18FDB200 , 5-6
	{   eRx64796,   	0U,          1U,          0x00U,       0,          1U ,  	   BADDATA,	        0U,				5000U	},   // Sootload in grams SPN 4781 , D1
	{ 	eRx64775,		0u,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 11000000    MIL  5080 PGN 64775
	{   eRx64892,   	6U,          1U,          0x1FU,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* Dpf SPN 64892, SPN 4175 3bits 7.6 */
	{   eRx64892,   	1U,          1U,          0xF3U,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* Dpf SPN 64892, SPN 3700 2 bits 2.3 */
	{   eRx64892,   	2U,          1U,          0xCFU,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* Dpf SPN 64892, SPN 3704 2 bits 3.5 */
	{   eRx64892,   	2U,          1U,          0x3FU,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* Dpf SPN 64892, SPN 3705 2 bits 3.7 */
	{   eRx64892,   	3U,          1U,          0xF3U,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* Dpf SPN 64892, SPN 3707 2 bits 4.3 */
	{   eRx64892,   	3U,          1U,          0xCFU,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* Dpf SPN 64892, SPN 3708 2 bits 4.5 */
	{   eRx64892,   	4U,          1U,          0xFCU,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* Dpf SPN 64892, SPN 3710 2 bits 5.1 */
	{   eRx64892,   	4U,          1U,          0xF3U,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* Dpf SPN 64892, SPN 3711 2 bits 5.3 */
	{   eRx64892,   	4U,          1U,          0xCFU,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* Dpf SPN 64892, SPN 3712 2 bits 5.5 */
	{   eRx64892,   	5U,          1U,          0xCFU,       0,          BITDATA,   	BADDATA,		0U,				5000U	},   	/* Dpf SPN 64892, SPN 3716 2 bits 6.5 */
	{   eRx65226,   	0U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00001100    PCD Lamp status        	 SPN 624    1.3
	{   eRx65226,   	1U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //	 bits 00001100    PCD Lamp flash  	      	 SPN 3040   2.3
	{   eRx60160,   	1U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00000011    PCD Lamp status            SPN 624    2.3
	{   eRx60160,   	2U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00000011    PCD Lamp flash             SPN 3040   3.1

};

sJ1939_RX_MSG_TYPE   sJ1939RxQueue[RX_QUE_SIZE];
sJ1939_RX_MSG_TYPE   J1939RxData[eNoOfPgnRx]; 
J1939_TX_TYPE        J1939_Tx[eNoOfPgnTx];
DTCdata DTC_J1939[NO_OF_DTC_CODE];
uint8_t u8DtcFramNum = 0U;
// Create an array corresponding to sPGNconstantsTx above for the PGN data
J1939_TX_DATA_TYPE  J1939TxData[eNoOfPgnTx]; 
static uint8_t bSpnDataReadyStatus[eNumofRxDataTypes];
static uint32_t u32DtcFrameData =0;

// Define all allowed CAN IDs at the top
const uint32_t allowedCANIDs[] =
{
    0x0CF00400, 0x18FEEF00, 0x18FEEE00, 0x18FEF700, 0x18FEE500,
    0x18FE5600, 0x18FD7B00, 0x18FEFF00, 0x18FECA00, 0x18ECFF00,
    0x18EBFF00, 0x18FDC600, 0x18FD7C00, 0x18FF6700, 0x1BDA17F9,
    0x18FD0700, 0x18FEF100, 0x18FEE000, 0x18FD1C00
};

#define NUM_ALLOWED_IDS (sizeof(allowedCANIDs) / sizeof(allowedCANIDs[0]))

/****************************************************************************************************************/
/* Local function declarations                                                                                  */
/****************************************************************************************************************/

/// @addtogroup J1939_LIBRARY
/// @{
 
/// @defgroup J1939Lib Private functions used in J1939 File
//// 
///   
/// @addtogroup J1939Lib
/// @{

static void _vPGN65262_CoolentTempGauge(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN61444_TachoMeter(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65271_BATT_Volt(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64891_ShootLoad(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65255_HourMeterGaugeTataEngine(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65253_HourMeterGaugeAskokEicher(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65272_TransmissionOilTemperatureVal(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65265_SpeedoMeterGauge(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65263_EngineOilPress(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65383_SCR_TempRingLampTFT(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64966_GlowtimeIndicationLamp_Mahindra(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64946_DieselParticulateFilter(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65252_GlowtimeIndicationLamp(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65279_LowAdBlueLamp(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64775_MIL_DLCC1_Lamp(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64892_DPF_Lamp(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65110_DefGauge(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64796_SootMass(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN60160_DM1_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65226_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vResetDM1Reception(uint8_t SourceAdd);
static void _J1939_RxManager(void);
static void _J1939_TxManager(void);
uint8_t isAllowedCANID(uint32_t id);

/// @} // endgroup J1939Lib

/// @} // endgroup J1939_LIBRARY


// prototype dependent definitions ------------------------------------------ 
// This needs to be defined after prototype definitions because they are used in this structure
const tPGNsonstRxType sPGNconstantsRx[eNoOfPgnRx] =
{
	//  PGN        SorceAdd         RxFunct             				  			
	{   PGN61444,  0x00U,   	_vPGN61444_TachoMeter },
	{   PGN65262,  0x00U,   	_vPGN65262_CoolentTempGauge },
	{	PGN65110,  0x00U, 		_vPGN65110_DefGauge},
	{	PGN65263,  0x00U, 		_vPGN65263_EngineOilPress},
	{	PGN65265,  0x00U, 		_vPGN65265_SpeedoMeterGauge},
	{	PGN65272,  0x32U, 		_vPGN65272_TransmissionOilTemperatureVal},
	{	PGN65383,  0x00U, 		_vPGN65383_SCR_TempRingLampTFT},
	{	PGN65253,  0x00U, 		_vPGN65253_HourMeterGaugeAskokEicher},
	{	PGN65255,  0x00U, 		_vPGN65255_HourMeterGaugeTataEngine},
	{	PGN64891,  0x00U, 		_vPGN64891_ShootLoad},
	{	PGN65271,  0x00U, 		_vPGN65271_BATT_Volt},
	{	PGN65279,	0x00U,		_vPGN65279_LowAdBlueLamp},
	{	PGN64892,	0x00U,		_vPGN64892_DPF_Lamp},
	{	PGN65252,	0x00U,		_vPGN65252_GlowtimeIndicationLamp},
	{	PGN64966,	0x00U,		_vPGN64966_GlowtimeIndicationLamp_Mahindra},
	{   PGN65226,	0x00U,      _vPGN65226_Data },
	{   PGN60160,   0x00U,      _vPGN60160_DM1_Data },
	{   PGN64946,	0x00U,      _vPGN64946_DieselParticulateFilter },
	{   PGN64796,	0x00U,      _vPGN64796_SootMass },
	{   PGN64775,	0x00U,      _vPGN64775_MIL_DLCC1_Lamp },
};



/****************************************************************************************************************/
/* Global function definitions                                                                                   */
/****************************************************************************************************************/
/*************************************************************************************************************//**
 *
 * Purpose  : J1939 services main loop
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void vJ1939_ServiceManager(void)
{
	static uint32_t serviceTime = 0U;

	if ((u32GetClockTicks() - serviceTime) >= SERVICE_INTERVAL)
	{
		// Service the J1939 received messages
		_J1939_RxManager();

		// Service the J1939 transmit messages		
		_J1939_TxManager();

		// Reset the timer
		serviceTime = u32GetClockTicks();
	}
}

/*************************************************************************************************************//**
 *
 * Purpose  : Initialize receive and transmit message structures. Set all PGN message data to invalid.
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void vJ1939Init(void)
{
	int16_t i, j;

	// Initialize the receive queue
	for (i = 0U; i < RX_QUE_SIZE; ++i)
	{
		sJ1939RxQueue[i].bDataRdy = FALSE;
	}
	// Initialize the Tx data structure
	for (i = 0; i < eNoOfPgnTx; ++i)
	{
		J1939TxData[i].bEnabled = FALSE;
		J1939TxData[i].bSendOnDemand = FALSE;
		J1939TxData[i].u32TimeStamp = u32GetClockTicks();
		// Set all data invalid
		for (j = 0U; j < 8U; ++j)
		J1939TxData[i].u8data[j] = 0xFFU;
		
	}
		
	// Initialize the Rx data structure
	for (i = 0U; i < eNoOfPgnRx; ++i)
	{
		J1939RxData[i].u32TimeStamp = 0;
		J1939RxData[i].bDataOutDate = TRUE;
		J1939RxData[i].bDataRdy = FALSE;
		// Set all data invalid
		for (j = 0U; j < 8U; ++j)
		{
			J1939RxData[i].u8data[j] = 0xFFU;
		}
		
	}
	for (j = 2U; j < 8U; ++j)	// Reinit of last six bytes of 65526 SA 0x00 so that DTC 65535 is never seen in DTC menu
	{
		J1939RxData[eJDM1DTC_spn1].u8data[j] = 0x00U;
	}
}

/*************************************************************************************************************//**
 *
 * Purpose  : Tx message buffer initialization
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void vJ1939TxInit(void)
{
	int16_t i, j;
	// Initialize the Tx data structure
	for (i = 0U; i < eNoOfPgnTx; ++i)
	{
		J1939TxData[i].bEnabled = TRUE;
		J1939TxData[i].bSendOnDemand = FALSE;
		J1939TxData[i].u32TimeStamp = u32GetClockTicks();
		// Set all data invalid
		for (j = 0U; j < 8U; ++j)
		{
			J1939TxData[i].u8data[j] = 0xFFU;
		}
	}
	
}

/*************************************************************************************************************//**
 *
 * Purpose  : Processs all interested received J1939 messages
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
static void _J1939_RxManager(void)
{
	uint32_t PGN,ID;	
	sJ1939_RX_MSG_TYPE *msg,*UDSbufPtr,UDSMsg;	   
   	uint16_t  RxQuePush;
	uint16_t i, timeNow;
	uint8_t RxQuePop;
	uint8_t SOURCEADD;
	uint8_t PriorityDataPageReserved5Bits;
	uint8_t  framNum, frameIndex = 0U;	
	uint8_t SourceAddType = 0U;

	u8DtcFramNum = 0U;
	
	// Check the entire input queue for fresh messaages
	for (RxQuePop = 0U; RxQuePop < RX_QUE_SIZE; ++RxQuePop)
	{
		if (sJ1939RxQueue[RxQuePop].bDataRdy)
		{
			// Process fresh message
			msg = &sJ1939RxQueue[RxQuePop];
			UDSMsg = sJ1939RxQueue[RxQuePop];

			// Extract the PGN
			ID = msg->sMsgID.Identifier;
			//PriorityDataPageReserved5Bits = ((msg->sMsgID.Identifier) & 0x1F000000U)>>24;
			PGN = (msg->sMsgID.Identifier >> 8) & 0x0003FFFFU;
			SOURCEADD = (msg->sMsgID.Identifier) & 0x000000FFU;
			
			//both diagnostics and pass through messages
		   if((UDSMsg.sMsgID.sIdField.SourceAddress == REQ_PHYSICAL_SA1 ||
			   UDSMsg.sMsgID.sIdField.SourceAddress == REQ_PHYSICAL_SA2) &&
			   (UDSMsg.sMsgID.sIdField.PduSpecific == PREFERRED_SA || //0x1BDA17F1/F9
			   UDSMsg.sMsgID.sIdField.PduSpecific == REQ_FUNCTIONAL_SA) //0x1BDB33F1/F9
			)
		   {
			   // Find an available queue slot
			   for (RxQuePush = 0U;
			   (RxQuePush < RX_QUE_SIZE) && (diagnosticsBuf[RxQuePush].bDataRdy == TRUE);
			   ++RxQuePush)
				{
				 //*** DO NOTHING ***
				}
			   if( RxQuePush < RX_QUE_SIZE )
			   {
				   //UDSbufPtr = &diagnosticsBuf[RxQuePush];
				   diagnosticsBuf[RxQuePush].sMsgID.Identifier = ((sJ1939RxQueue[RxQuePop].sMsgID.Identifier)  & 0xffff0000U);
					for(i= 0U;i < 8U; i++)
					{
						diagnosticsBuf[RxQuePush].u8data[i] = sJ1939RxQueue[RxQuePop].u8data[i];
					}
					diagnosticsBuf[RxQuePush].u32TimeStamp = sJ1939RxQueue[RxQuePop].u32TimeStamp;
					diagnosticsBuf[RxQuePush].ucNumofRxBytes = sJ1939RxQueue[RxQuePop].ucNumofRxBytes;
				    diagnosticsBuf[RxQuePush].bDataRdy = TRUE;

				   diagnosticsTimer = u32GetClockTicks();
				   testerAddress = UDSMsg.sMsgID.sIdField.SourceAddress;
			   }
		   }
		    if(commInfo.networkCommRxEnabled == TRUE)
			{
				if(SOURCEADD == EMS_SA1_EXT)
				{
					SourceAddType = eEmsSA;
				}

				switch (PGN & 0xFF00U)
				{
					case PGN60416  : // EBFF
						//accept only if its TP.CM_BAM
						if(	(PGN & 0x00FF) == J1939_GLOBAL_ADDRESS
							&& sJ1939RxQueue[RxQuePop].u8data[0] == 32U )
						{
							//is the PGN DM1 from relevant ECUs
							if( sJ1939RxQueue[RxQuePop].u8data[5] == PGN65226_0
								&& sJ1939RxQueue[RxQuePop].u8data[6] == PGN65226_1
								&& sJ1939RxQueue[RxQuePop].u8data[7] == PGN65226_2
								&&  SOURCEADD == EMS_SA1_EXT )
							{
								//to track time to stop waiting for TP_DT
								u32MultiPacketTimer[SourceAddType] = u32GetClockTicks();
								u8MultiPacketDM1[SourceAddType] = eTPCMBAM;
								u8MultiFrameSA[SourceAddType] = SOURCEADD;

								//DTCs will be read from EMS and TCU only if the previous DTCs
								//from either ECUs have been processed
								if( u8DTCready == FALSE
									&& (u8MultiFrameSA[SourceAddType] == EMS_SA1_EXT /*|| u8MultiFrameSA[SourceAddType] == TCU_SA_EXT*/)
									)
								{
									u8DTCSA = u8MultiFrameSA[SourceAddType];
									u8DM1NumFrames = sJ1939RxQueue[RxQuePop].u8data[3];
									u16lTotalBytes = sJ1939RxQueue[RxQuePop].u8data[2];
									u16lTotalBytes = (u16lTotalBytes<<8) | sJ1939RxQueue[RxQuePop].u8data[1];
								}
							}
						}
						break;
					case PGN60160 :		// EBFF
						//accept only if its TP.DT from the relevant SA
						if( (PGN & 0x00FF) == J1939_GLOBAL_ADDRESS
							&& u8MultiFrameSA[SourceAddType] != J1939_NULL_ADDRESS
							&& u8MultiFrameSA[SourceAddType] == SOURCEADD )
						{
							// accept only if this frame is continued from same dataset
							if((u32GetClockTicks() - u32MultiPacketTimer[SourceAddType]) < BAM_TIMEOUT)
							{
								//if its a first frame
								if( (sJ1939RxQueue[RxQuePop].u8data[0] == 0x01 ) && (u8MultiPacketDM1[SourceAddType] == eTPCMBAM ) )
								{
									u32MultiPacketTimer[SourceAddType] = u32GetClockTicks();
									//DTCs are read only from EMS
									if( u8DTCSA == u8MultiFrameSA[SourceAddType] )
									{
										u8DtcFramNum = 1;
										u8MultiPacketDM1[SourceAddType] = eTPDT;
										DTC_J1939[0].SPNValue = (uint32_t)sJ1939RxQueue[RxQuePop].u8data[3];
										DTC_J1939[0].SPNValue |= ((uint32_t)sJ1939RxQueue[RxQuePop].u8data[4] << 8);
										DTC_J1939[0].SPNValue |= ( (uint32_t)(sJ1939RxQueue[RxQuePop].u8data[5] & 0xE0) << 11);
										DTC_J1939[0].FMICode = sJ1939RxQueue[RxQuePop].u8data[5] & 0x1F;
										u32DtcFrameData = sJ1939RxQueue[RxQuePop].u8data[7]; // DTC_J1939[1].SPNValue
									}
									else
									{
										_vResetDM1Reception(SourceAddType);
									}

								}
								else if(u8MultiPacketDM1[SourceAddType] == eTPDT )
								{
									u32MultiPacketTimer[SourceAddType] = u32GetClockTicks();
									//so that these frame is rejected for DM1 controlled lamps
									SOURCEADD = J1939_NULL_ADDRESS;

									framNum = sJ1939RxQueue[RxQuePop].u8data[0];
									u8DtcFramNum = framNum;
									//if this is the last frame
									if(framNum == u8DM1NumFrames )
									{
										u8DTCCount = (u16lTotalBytes - 2U)/4U;
										_vResetDM1Reception(SourceAddType);
										//this flag should be set to false when DTCs have been processed
										u8DTCready = TRUE;
									}
									frameIndex = 0U;

									// max num byte in TP.CM is 255 : 255/7 ~ 36 frames maximum
									if(framNum > 36U )
									{
										framNum = 36U;
									}

									if(framNum > 5U )
									{
										// calculating the index to extract DTC; the pattern repeats after every 4 frames
										frameIndex = (framNum - 2U) / 4U ;
										framNum = framNum - (frameIndex * 4U);
										frameIndex = frameIndex * 7U;
									}

									switch(framNum)
									{
										case 0x2U:
										{
											//should not be more than number of DTCs supported
											if((frameIndex + 1U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 1U].SPNValue = u32DtcFrameData;
												DTC_J1939[frameIndex + 1U].SPNValue |= (uint32_t)sJ1939RxQueue[RxQuePop].u8data[1] << 8;
												DTC_J1939[frameIndex + 1U].SPNValue |= ((uint32_t)(sJ1939RxQueue[RxQuePop].u8data[2] & 0xE0) << 11);
												DTC_J1939[frameIndex + 1U].FMICode = sJ1939RxQueue[RxQuePop].u8data[2] & 0x1F;
											}
											if( (frameIndex + 2U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 2U].SPNValue = (uint32_t)sJ1939RxQueue[RxQuePop].u8data[4];
												DTC_J1939[frameIndex + 2U].SPNValue |= (uint32_t)sJ1939RxQueue[RxQuePop].u8data[5] << 8;
												DTC_J1939[frameIndex + 2U].SPNValue |= ((uint32_t)(sJ1939RxQueue[RxQuePop].u8data[6] & 0xE0) << 11);
												DTC_J1939[frameIndex + 2U].FMICode = sJ1939RxQueue[RxQuePop].u8data[6] & 0x1F;
											}
										}
										break;

										case 0x3U:
										{
											//should not be more than number of DTCs supported
											if( (frameIndex + 3U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 3U].SPNValue = (uint32_t)sJ1939RxQueue[RxQuePop].u8data[1];
												DTC_J1939[frameIndex + 3U].SPNValue |= (uint32_t)sJ1939RxQueue[RxQuePop].u8data[2] << 8;
												DTC_J1939[frameIndex + 3U].SPNValue |= ((uint32_t)(sJ1939RxQueue[RxQuePop].u8data[3] & 0xE0) << 11);
												DTC_J1939[frameIndex + 3U].FMICode = sJ1939RxQueue[RxQuePop].u8data[3] & 0x1F;
											}

											if( (frameIndex + 4U) < NO_OF_DTC_CODE)
											{
												//Change sequence for remove error
												DTC_J1939[frameIndex + 4U].SPNValue = (uint32_t)sJ1939RxQueue[RxQuePop].u8data[5];
												DTC_J1939[frameIndex + 4U].SPNValue |= (uint32_t)sJ1939RxQueue[RxQuePop].u8data[6] << 8;
												DTC_J1939[frameIndex + 4U].SPNValue |= ((uint32_t)(sJ1939RxQueue[RxQuePop].u8data[7] & 0xE0) << 11);
												DTC_J1939[frameIndex + 4U].FMICode = sJ1939RxQueue[RxQuePop].u8data[7] & 0x1F;

											}
										}
										break;

										case 0x04U:
										{
											//should not be more than number of DTCs supported
											if((frameIndex + 5U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 5U].SPNValue = (uint32_t)sJ1939RxQueue[RxQuePop].u8data[2];
												DTC_J1939[frameIndex + 5U].SPNValue |= (uint32_t)sJ1939RxQueue[RxQuePop].u8data[3] << 8;
												DTC_J1939[frameIndex + 5U].SPNValue |= ((uint32_t)(sJ1939RxQueue[RxQuePop].u8data[4] & 0xE0) << 11);
												DTC_J1939[frameIndex + 5U].FMICode = sJ1939RxQueue[RxQuePop].u8data[4] & 0x1F;
											}
											if( (frameIndex + 6U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 6U].SPNValue = (uint32_t)sJ1939RxQueue[RxQuePop].u8data[6];   //
												DTC_J1939[frameIndex + 6U].SPNValue |= ((uint32_t)sJ1939RxQueue[RxQuePop].u8data[7] << 8);
											}
										}
										break;

										case 0x05U:
										{
											//should not be more than number of DTCs supported
											if((frameIndex + 6U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 6U].SPNValue |= ((uint32_t)(sJ1939RxQueue[RxQuePop].u8data[1] & 0xE0) << 11);
												DTC_J1939[frameIndex + 6U].FMICode = sJ1939RxQueue[RxQuePop].u8data[1] & 0x1F;
											}
											if( (frameIndex + 7) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 7U].SPNValue = (uint32_t)sJ1939RxQueue[RxQuePop].u8data[3];
												DTC_J1939[frameIndex + 7U].SPNValue |= (uint32_t)sJ1939RxQueue[RxQuePop].u8data[4] << 8;
												DTC_J1939[frameIndex + 7U].SPNValue |= ((uint32_t)(sJ1939RxQueue[RxQuePop].u8data[5] & 0xE0) << 11);
												DTC_J1939[frameIndex + 7U].FMICode = sJ1939RxQueue[RxQuePop].u8data[5] & 0x1F;
											}
											if( (frameIndex + 8U) < NO_OF_DTC_CODE)
											{
												u32DtcFrameData = sJ1939RxQueue[RxQuePop].u8data[7];//
											}
										}
										break;
									}
								}
								// if the first frame was missed, prepare for next multipacket
								else
								{
									_vResetDM1Reception(SourceAddType);
									//so that these frame is rejected for DM1 controlled lamps
									SOURCEADD = J1939_NULL_ADDRESS;
								}
							}
							//if BAM timeout, prepare for next multipacket
							else
							{
								if(u8MultiPacketDM1[SourceAddType] == eTPDT)
								{
									u8DTCready = TRUE;
								}
								_vResetDM1Reception(SourceAddType);
								//so that these frame is rejected for DM1 controlled lamps
								SOURCEADD = J1939_NULL_ADDRESS;
							}
						}
						else
						{
							//so that these frames are rejected for DM1 controlled lamps
							SOURCEADD = J1939_NULL_ADDRESS;
						}
					break;
				}

				//if(msg->sMsgID.sIdField.PduSpecific == J1939_GLOBAL_ADDRESS)

				if((PGN == 0xECFF) || (PGN == 0xEBFF))
				{
					PGN = PGN & 0xFF00U;
				}

				// Check all other PGN inputs
				for(i = 0U; i < eNoOfPgnRx; ++i)
				{
					if( (PGN == sPGNconstantsRx[i].PGN)  && (SOURCEADD == sPGNconstantsRx[i].u8SourceAdd) )
					{
						sPGNconstantsRx[i].RxFunct(msg);
					}
				}
			}//end if(commInfo.networkCommRxEnabled == TRUE)
			// Clear the queue slot
			sJ1939RxQueue[RxQuePop].bDataRdy = FALSE;
		}// end if (sJ1939RxQueue[RxQuePop].bDataRdy)

	}//end for (RxQuePop = 0; RxQuePop < RX_QUE_SIZE; ++RxQuePop)
}

/*************************************************************************************************************//**
 *
 * Purpose  : Processs all transmit J1939 messages
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
static void _J1939_TxManager(void)
{
	static uint8_t n = 0U;
	uint8_t TxReturnCode;
	uint8_t StatusFlag = FALSE;

	
	++n;			
	if(n >= eNoOfPgnTx)
	{
		n =0U;
	}	
	
	if (J1939TxData[n].bEnabled)
	{
		// Determine whether sending on demand or periodic
		if (((sPGNconstantsTx[n].rate != SEND_ON_DEMAND)
		&& ((u32GetClockTicks() - J1939TxData[n].u32TimeStamp) >= sPGNconstantsTx[n].rate))
		|| J1939TxData[n].bSendOnDemand)
		{
			switch (sPGNconstantsTx[n].PGNtype)
			{
				case ePGN_J1939 :
					// Engine instance is part of the PGN
					 //vTxMsgBufProcessing(sPGNconstantsTx[n].ID,MSG_LEN,J1939TxData[n].u8data,J1939_TX);
				break;				
			}			
			// Reset the timer and on demand flag
			J1939TxData[n].u32TimeStamp = u32GetClockTicks();
			J1939TxData[n].bSendOnDemand = FALSE;			
		}
	}
	
}

/*************************************************************************************************************//**
 *
 * Purpose  :  Enable Disable Transmit PGN 
 *											
 *
 * @param[in]:  Transmit PGN ,ENUM    
 * @param[out]:	None
 * @return     	TRUE/FALSE
 ****************************************************************************************************************/
 void vJ1939_TxMsgEnableDisable(tPGN_TxIndexerType eTxPgn , uint8_t EnableFlag)
{
	J1939TxData[eTxPgn].bEnabled = EnableFlag;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Enable Disable Transmit PGN 
 *											
 *
 * @param[in]:  Transmit PGN     
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void vJ1939_TxSendOnDemandEnable(tPGN_TxIndexerType eTxPgn )
{
	J1939TxData[eTxPgn].bSendOnDemand = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Enable Disable Transmit PGN 
 *											
 *
 * @param[in]:  Tx PGN enum    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void vJ1939_TxSendOnDemandDisable(tPGN_TxIndexerType eTxPgn )
{
	J1939TxData[eTxPgn].bSendOnDemand = FALSE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : (Not use in code) Determines whether or not the PGN is supported for ISO requests.
 * If it is supported, it is then transmitted.							
 *
 * @param[in]:  PGN    
 * @param[out]:	None
 * @return     	TRUE if PGN is supported, FALSE otherwise
 ****************************************************************************************************************/
uint8_t bProcessISORequest(uint32_t PGN)
{
	int16_t n;
	uint8_t foundPGN;

	// Find a matching PGN
	foundPGN = FALSE;
	n = -1;
	do 
	{
		++n;
		foundPGN = (uint8_t)(sPGNconstantsTx[n].PGN == PGN);
	} while ((n < eNoOfPgnTx) && (!foundPGN));

	if (foundPGN)
	{
		// Set flag to force immediate output
		if(n < eNoOfPgnTx)
		{
			J1939TxData[n].bSendOnDemand = TRUE;
		}
	}

	return foundPGN;
}

/*************************************************************************************************************//**
 *
 * Purpose  : (Not using transmite )Convert input data into PGN specification and deposit into the
 * 				PGN data for the output message.
 *											
 *
 * @param[in]:   data type,
 *             data,
 *             +- percent overrange      
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void vPutPGNdata(eTxDataType dtype, uint32_t rawData, int16_t overrange)
{
	uint32_t data;
	uint8_t *dataPtr;
	
	struct 
	{ 
		uint8_t byteSize;
		uint8_t bitMask; 
	} b;

	// Set a pointer to the PGN message data
	dataPtr = (uint8_t *)&J1939TxData[sPGNdataTx[dtype].PGNindex].u8data[sPGNdataTx[dtype].byteIndex];

	if (dtype < eNumofTxDataTypes)
	{
		// Valid data type - determine whether it is byte or bit data
		if (sPGNdataTx[dtype].bitRes == BITDATA)
		{
			// Bit data --------------
			// Get the bit mask and clear the bits
			b.bitMask = sPGNdataTx[dtype].bitMask;
			*dataPtr &= b.bitMask;

			// Shift the raw data bits up to the proper position
			while ((b.bitMask & 1) == 1)
			{
				rawData <<= 1;
				b.bitMask >>= 1;
			}

			// Set the bits
			*dataPtr |= rawData;
		}
		else
		{	
			// unused and needs to be modified for future use
			// Byte data ------------- 
			if (overrange >= MAX_OVERRANGE)
			{
				data = sPGNdataTx[dtype].overrangeData;
			}
			else
			{
				// Convert rawData to PGN format for integer math
				 if(dtype == eJSoftVersionDataTx)	
				 {
					data = rawData;
				 }
				//data = ( ( (rawData*(u32)sPGNdataTx[dtype].bitRes)) - sPGNdataTx[dtype].dataOffset);
				b.byteSize = sPGNdataTx[dtype].byteSize;							
				do 
				{
				 	*dataPtr++ = (uint8_t)data;
					data >>= 8;
				} while (--b.byteSize != 0U);
						 				 
			}
		}
	}
	
}
/*************************************************************************************************************//**
 *
 * Purpose  : Convert input data into PGN specification and deposit into the
 * 				PGN data for the output message
 *											
 *
 * @param[in]:  data type,
 *             data,
 *             +- percent overrange      
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
uint32_t u32GetPGNdata(eRxDataType dtype)
{
	uint32_t data = 0U;	
	uint8_t i;
	union 
	{
		uint8_t byteSize;
		uint8_t bitMask; 
	} b;

	// Get the data bit mask
	b.bitMask = sPGNdataRx[dtype].bitMask;

	if (dtype < eNumofRxDataTypes)
	{
		// Valid data type - determine whether it is byte or bit data
		if (sPGNdataRx[dtype].bitRes == BITDATA)
		{
			// Set a pointer to the PGN message data
			data = J1939RxData[sPGNdataRx[dtype].PGNindex].u8data[sPGNdataRx[dtype].byteIndex];
			// Keep only the relevant bits of the data
			data &= ~(uint8_t)b.bitMask;
			// Shift them down to the least significant bit location
			while ((b.bitMask & 1U) == 1U)
			{
				b.bitMask >>= 1;
				data >>= 1;
			}
		}
		else
		{
			// Byte data -------------
			// Get the data byte size
			b.byteSize = sPGNdataRx[dtype].byteSize;
			if(b.byteSize==1U)
			{
				data = (uint8_t)J1939RxData[sPGNdataRx[dtype].PGNindex].u8data[sPGNdataRx[dtype].byteIndex];

				if(data < sPGNdataRx[dtype].overrangeData ) 
				{				
				   if(data <= sPGNdataRx[dtype].underrangeData)
						data = sPGNdataRx[dtype].underrangeData;
					data = (data * (uint32_t)sPGNdataRx[dtype].bitRes) +  sPGNdataRx[dtype].dataOffset;
					
				}
			}
			else
			{
				for( i = 0U; i< b.byteSize; i++)
				{
					data |= (uint32_t)J1939RxData[sPGNdataRx[dtype].PGNindex].u8data[sPGNdataRx[dtype].byteIndex + i] << (i*8);
				}
			}
			
			if(data < sPGNdataRx[dtype].overrangeData ) 
			{
			   if(data <= sPGNdataRx[dtype].underrangeData)
			   	{
			   		data = sPGNdataRx[dtype].underrangeData;
			   	}
				data =  (data * sPGNdataRx[dtype].bitRes)  +  sPGNdataRx[dtype].dataOffset;
				
			}
					  
		}
		
	}//end if (dtype < eNumofRxDataTypes)
	return data;
}

/*************************************************************************************************************//**
 *
 * Purpose  : (Not using this function) Gets the latest bit data from PGN received message buffer. Bits are
 * 				shifted down to the least significant bits before returning.						
 *
 * @param[in]:  data type to fetch     
 * @param[out]:	None
 * @return     	bit data
 ****************************************************************************************************************/
uint8_t u8GetBitData(eRxDataType dType)
{
	// Get the data bit mask
	uint8_t bitMask = sPGNdataRx[dType].bitMask;

	// Get the data
	uint8_t data = J1939RxData[sPGNdataRx[dType].PGNindex].u8data[sPGNdataRx[dType].byteIndex];

	// Keep only the relevant bits of the data
	data &= ~(uint8_t)bitMask;

	// Shift them down to the least significant bit location
	while ((bitMask & 1U) == 1U)
	{
		bitMask >>= 1;
		data >>= 1;
	}

	return data;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65281 data from rx receiving buffer to its actual buffer
 *											
 *
 * @param[in]:   Rx Pgn type    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65262_CoolentTempGauge(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;	  

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65262].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65262].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65262].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65262].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJEngineCoolLevel] = TRUE;
}
/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65110 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65110_DefGauge(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65110].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65110].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65110].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65110].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eJDefGauge] = TRUE;
   	bSpnDataReadyStatus[eEngineStopLamp] = TRUE;

}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 64892 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN64892_DPF_Lamp(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i = 0U; i < 8U; ++i)
	{
		J1939RxData[eRx64892].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx64892].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64892].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx64892].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eDpfLampIndicationLampSPN_3697] = TRUE;
   	bSpnDataReadyStatus[eDpfRegenInhibitLampSPN_3703] = TRUE;
   	bSpnDataReadyStatus[eHestLampOnDial] = TRUE;
	bSpnDataReadyStatus[ePopupMessageSPN_4175] = TRUE;
	bSpnDataReadyStatus[ePopupMessageSPN_3700] = TRUE;
	bSpnDataReadyStatus[ePopupMessageSPN_3704] = TRUE;
	bSpnDataReadyStatus[ePopupMessageSPN_3705] = TRUE;
	bSpnDataReadyStatus[ePopupMessageSPN_3707] = TRUE;
	bSpnDataReadyStatus[ePopupMessageSPN_3708] = TRUE;
	bSpnDataReadyStatus[ePopupMessageSPN_3710] = TRUE;
	bSpnDataReadyStatus[ePopupMessageSPN_3711] = TRUE;
	bSpnDataReadyStatus[ePopupMessageSPN_3712] = TRUE;
	bSpnDataReadyStatus[ePopupMessageSPN_3716] = TRUE;
	bSpnDataReadyStatus[eHestLampOnDial] = TRUE;

}


/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 64775 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN64775_MIL_DLCC1_Lamp(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx64775].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx64775].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64775].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx64775].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eMIL_DLCC1_64775] = TRUE;


}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65281 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65263_EngineOilPress(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65263].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65263].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65263].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65263].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eEngineOilPressureGauge] = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65265 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65265_SpeedoMeterGauge(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65265].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65265].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65265].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65265].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eSpeedoMeterCAN] = TRUE;
}

static void _vPGN64796_SootMass(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx64796].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx64796].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64796].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx64796].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eSootMass] = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65272 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65272_TransmissionOilTemperatureVal(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65272].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65272].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65272].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65272].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eTransmissionOilTemperatureVal] = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65272 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65272_0x32_TransmissionOilTemperatureVal(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65272].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65272].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65272].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65272].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eTransmissionOilTemperatureVal] = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65383 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65383_SCR_TempRingLampTFT(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i = 0U; i < 8U; ++i)
	{
		J1939RxData[eRx65383].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65383].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65383].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65383].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eSCR_TempRingLampTFT] = TRUE;
	bSpnDataReadyStatus[ePoorDefQualityLampDial] = TRUE;
}


/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 64946 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN64946_DieselParticulateFilter(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i = 0U; i < 8U; ++i)
	{
		J1939RxData[eRx64946].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx64946].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64946].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx64946].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eDieselParticulateFilter] = TRUE;

}


/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 64966 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN64966_GlowtimeIndicationLamp_Mahindra(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i = 0U; i < 8U; ++i)
	{
		J1939RxData[eRx64966].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx64966].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64966].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx64966].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eGlowtimeIndicationLamp_KoelEngine] = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65252 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65252_GlowtimeIndicationLamp(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i = 0U; i < 8U; ++i)
	{
		J1939RxData[eRx65252].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65252].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65252].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65252].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eGlowtimeIndicationLamp_MahindraEngine] = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65279 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65279_LowAdBlueLamp(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i = 0U; i < 8U; ++i)
	{
		J1939RxData[eRx65279].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65279].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65279].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65279].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eLowAdBlueLevelLampTFT] = TRUE;
	bSpnDataReadyStatus[eWaterInFuelLamp] = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65253 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65253_HourMeterGaugeAskokEicher(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65253].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65253].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65253].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65253].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eHourMeter_AshokEicherEng] = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65255 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65255_HourMeterGaugeTataEngine(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65262].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65255].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65255].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65255].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eHourMeter_TATA_Eng] = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 64891 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN64891_ShootLoad(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx64891].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx64891].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64891].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx64891].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eShootLoad] = TRUE;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65271 data from rx receiving buffer to its actual buffer
 *
 *
 * @param[in]:   Rx Pgn type
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/

static void _vPGN65271_BATT_Volt(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65271].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65271].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65271].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65271].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag
   	bSpnDataReadyStatus[eBattVolt] = TRUE;
}


/*************************************************************************************************************//**
 *
 * Purpose  : Copy PGN 65282 data from rx receiving buffer to its actual buffer
 *											
 *
 * @param[in]:  Rx Pgn type    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
static void _vPGN61444_TachoMeter(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;	  

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx61444].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx61444].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx61444].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx61444].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJTachoMeter] = TRUE;
}

static void _vPGN60160_DM1_Data(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	if(msg->u8data[0] == 1U)						// if frame number is 1.
	{
		// Save the latest received data
		for (i = 0U; i < 8U; ++i)
		{
			J1939RxData[eRx60160].u8data[i] = msg->u8data[i];
		}

		J1939RxData[eRx60160].u32TimeStamp = msg->u32TimeStamp;
		J1939RxData[eRx60160].sMsgID.Identifier = msg->sMsgID.Identifier;
		J1939RxData[eRx60160].bDataRdy = msg->bDataRdy;

		// Set SPN Data ready flag ----
		bSpnDataReadyStatus[eJMilDM1_1213] = TRUE;
		bSpnDataReadyStatus[eJMilDM1_3038] = TRUE;
		bSpnDataReadyStatus[eJMil2_SysOkDM1_987] = TRUE;
		bSpnDataReadyStatus[eJMil2_SysOkDM1_3041] = TRUE;
		bSpnDataReadyStatus[eJMil2_PCDlampDM1_624] = TRUE;
	    bSpnDataReadyStatus[eJMil2_PCDlampDM1_3040] = TRUE;

	}

}

/*********************************************************************//**
 *
 * Copy PGN 65226 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/

static void _vPGN65226_Data(sJ1939_RX_MSG_TYPE *msg)
{
	int16_t i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65226].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65226].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65226].sMsgID.Identifier = msg->sMsgID.Identifier;
	J1939RxData[eRx65226].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag

	bSpnDataReadyStatus[eJMilSPN1213] = TRUE;
	bSpnDataReadyStatus[eJMilSPN3038] = TRUE;
	bSpnDataReadyStatus[eJMil2_SysOkSPN987] = TRUE;
	bSpnDataReadyStatus[eJMil2_SysOkSPN3041] = TRUE;
	bSpnDataReadyStatus[eJDM1DTC_spn1] = TRUE;
	bSpnDataReadyStatus[eJDM1DTC_spn2] = TRUE;
	bSpnDataReadyStatus[eJDM1DTC_fmi] = TRUE;
	bSpnDataReadyStatus[eJMil2_PCDlampSPN624] = TRUE;
	bSpnDataReadyStatus[eJMil2_PCDlampSPN3040] = TRUE;

}



/*************************************************************************************************************//**
 *
 * Purpose  : This function use for Reset DTC Parameters
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void vResetDTCParameters(void)
{
	u8DTCSA = J1939_NULL_ADDRESS;
	u8DTCready = FALSE;
	u8DTCCount = 0U;
}

/*************************************************************************************************************//**
 *
 * Purpose  : This function use for Get DTC ready status 
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	DTC ready status 
 ****************************************************************************************************************/
uint8_t u8GetDTCStatus(void)
{
	return u8DTCready;
}

/*************************************************************************************************************//**
 *
 * Purpose  : This function use for Get Dtc count
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	Get Dtc count
 ****************************************************************************************************************/
uint8_t u8GetDTCCount(void)
{
	//return u8DTCCount;
}

/*************************************************************************************************************//**
 *
 * Purpose  : This function use for Get Dtc Source add
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	Dtc Source add
 ****************************************************************************************************************/
uint8_t u8GetDTCSA(void)
{
	return u8DTCSA;
}

/*************************************************************************************************************//**
 *
 * Purpose  : This function use for Get Dtc Fram Number
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	Get Dtc Fram Num
 ****************************************************************************************************************/
uint8_t u8GetDtcFramNum()
{
	return u8DtcFramNum;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Get Dtc Total Bytes
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	Total DTC bytes
 ****************************************************************************************************************/
uint16_t u16GetDtcTotalBytes()
{
	return u16lTotalBytes;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Reset DM1 Reception msg
 *											
 *
 * @param[in]:  Source adress    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
static void _vResetDM1Reception(uint8_t SourceAdd)
{
	//reset the DTC parameters at the end of multipacket reception
	u8MultiPacketDM1[SourceAdd] = eNoNewMultiPacket;
	u8MultiFrameSA[SourceAdd] = J1939_NULL_ADDRESS;
	
}

/*************************************************************************************************************//**
 *
 * Purpose  : Get data ready Of each PGN.
 *											
 *
 * @param[in]:  data type,    
 * @param[out]:	None
 * @return     	True if msg Received ,False if not
 ****************************************************************************************************************/
uint8_t bGetPGNDataReady(eRxDataType dtype)
{
	return J1939RxData[sPGNdataRx[dtype].PGNindex].bDataRdy;
}

/*************************************************************************************************************//**
 *
 * Purpose  : Clear data ready Of each PGN.
 *											
 *
 * @param[in]:  data type,    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void ClearPGNDataReady(eRxDataType dtype)
{
	J1939RxData[sPGNdataRx[dtype].PGNindex].bDataRdy = FALSE;
}

/*************************************************************************************************************//**
 *
 * Purpose  :  Get SPN data ready status
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	True if msg Received ,False if not
 ****************************************************************************************************************/
uint8_t bGetSPNDataReady(eRxDataType dtype)
{
	return bSpnDataReadyStatus[dtype];
}

/*************************************************************************************************************//**
 *
 * Purpose  : Clear data ready status of SPN
 *											
 *
 * @param[in]:  data type,    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void vClearSPNDataReady(eRxDataType dtype)
{
	bSpnDataReadyStatus[dtype] = FALSE;
	
}

/*************************************************************************************************************//**
 *
 * Purpose  : Check the timeout Of each PGN.
 *											
 *
 * @param[in]:  data type,    
 * @param[out]:	None
 * @return    : 	 True if timeout ,False if not
 ****************************************************************************************************************/
uint8_t bGetPGNTimeOut(eRxDataType dtype)
{
	if (dtype < eNumofRxDataTypes)
	{
		if((u32GetClockTicks() - J1939RxData[sPGNdataRx[dtype].PGNindex].u32TimeStamp) >= sPGNdataRx[dtype].u32TimeStamp)
		{
			J1939RxData[sPGNdataRx[dtype].PGNindex].bDataOutDate =TRUE;
		}
		else
		{
			J1939RxData[sPGNdataRx[dtype].PGNindex].bDataOutDate =FALSE;
		}
	}	
	return J1939RxData[sPGNdataRx[dtype].PGNindex].bDataOutDate;
}

/*************************************************************************************************************//**
 *
 * Purpose  : This method is called from the CAN receive interrupt.
 * It will find an available queue slot and save the message for later
 * processing. If there is no room in the queue, the message will be
 * dropped.							
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void vJ1939OnFullRx(void)
{
	uint32_t	MsgBuf_address;
	uint8_t	rx_msg_DLC;	
	uint32_t rx_msg_ID;
	uint8_t  rx_buffer_number;
	uint8_t	rx_data_cnt;
	uint32_t received_id = 0;
//	sJ1939_Id_Struct_Type ID;
	int16_t RxQuePush;

	//  Find an available queue slot
	for (RxQuePush = 0U;
	(RxQuePush < RX_QUE_SIZE) && (sJ1939RxQueue[RxQuePush].bDataRdy == TRUE);
	++RxQuePush)
	{
		// empty statement for this for loop
	}

	if( RxQuePush < RX_QUE_SIZE )
	{
		//  Ignore message if no room in the queue
		if (sJ1939RxQueue[RxQuePush].bDataRdy == FALSE)
		{
			received_id = GetRx29BitsId();

			if (isAllowedCANID(received_id))
			{
			    rx_data_cnt = 0U;
			    sJ1939RxQueue[RxQuePush].u32TimeStamp = u32GetClockTicks();
			    sJ1939RxQueue[RxQuePush].sMsgID.Identifier = received_id;
			    sJ1939RxQueue[RxQuePush].ucNumofRxBytes = GetRxDLC();

			    for (rx_data_cnt = 0; ((rx_data_cnt < rx_msg_DLC) && (rx_data_cnt < 8)); rx_data_cnt++)
			    {
			        sJ1939RxQueue[RxQuePush].u8data[rx_data_cnt] = RxData[rx_data_cnt];
			    }

			    sJ1939RxQueue[RxQuePush].bDataRdy = TRUE;
			}

		}
	}
	else
	{
	   //if message receiving buffer not available empty room then clear CAN interrupt 
		//C0INTS = 0x0002;
		//rx_buffer_number = RxData[RxQuePush];
		// Set CAN message buffer[n] register address
		//MsgBuf_address = (MSGBUF_BASE_ADD + (0x10 * rx_buffer_number));
		//*((uint32_t *)(MsgBuf_address + 0x0e)) = 0x0004;		// Clear DN bit
	}

}

// Function to check if received ID is in allowed list
uint8_t isAllowedCANID(uint32_t id)
{
    for (uint8_t i = 0; i < NUM_ALLOWED_IDS; i++)
    {
        if (allowedCANIDs[i] == id)
        {
            return TRUE; // ID found
        }
    }
    return FALSE; // Not found
}

/*************************************************************************************************************//**
 *
 * Purpose  : DTC clear
 *											
 *
 * @param[in]:  None    
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void vClearDTC(void)
{
	uint8_t i = 0;

	vResetDTCParameters();

}

