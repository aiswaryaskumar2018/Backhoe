/*
 * SpeedoOdo.h
 *
 *  Created on: Sep 24, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_ODO_H_
#define INC_ODO_H_


#include "main.h"


#define PULSES_PER_KM				181980u
#define ODO_MAX_RANGE      			99999 	// decimal place not included
#define TRIPA_MAX_RANGE    			9999	// decimal place not included
#define TRIPB_MAX_RANGE    			9999	// decimal place not included
#define ODO_FRACTION_VALUE 			9


typedef struct
{
	uint32_t     u32OdoValue;
	uint32_t     u32TripAValue;
	uint32_t     u32TripBValue;
	uint16_t     u16OdoPulseCount;
	uint16_t     u16TripAPulseCount;
	uint16_t     u16TripBPulseCount;
	uint8_t      u8OdoFractionValue;
	uint8_t      u8TripAFractionValue;
	uint8_t      u8TripBFractionValue;
	uint8_t    bOdoUpdate : 1;          			// Set when ODO update
	uint8_t    bTripAUpdate : 1;          			// Set when TripA update
	uint8_t    bTripBUpdate : 1;          			// Set when TripB update
	uint8_t    bFrOdoUpdate : 1;          			// Set when Fraction Odo update
	uint8_t    bFrTripAUpdate : 1;          		// Set when Fraction TripA update
	uint8_t    bFrTripBUpdate : 1;          		// Set when Fraction TripB update

} ODO_DATA_TYPE;

extern ODO_DATA_TYPE sODO;


#ifdef __cplusplus
extern "C"
{
#endif

uint32_t GetOdoCountValue(void);
uint16_t GetTripAOdoMeterCountValue(void);
uint8_t GetOneKmStatus(void);
void SetOneKmStatus(uint8_t Status);
void SetTripAOdoMeterCountValue(uint16_t VAl);
void SetOdoCountValue(uint32_t Val);
void SetFreqCapturedStatus(uint8_t Status);
uint8_t GetFreqCapturedStatus(void);
uint32_t GetFinalFreq(void);
void SetFinalFreq(uint32_t Val);

void vSetSpeedoPulses(uint32_t pulses);



/// @addtogroup ODO_METER_DATA_LIBRARY
/// @{
/// @defgroup odometerdataLib  functions used
////
///
/// @addtogroup odometerdataLib
/// @{

void vSetOdoValue(uint32_t);
uint32_t u32GetOdoValue(void);
void vSetOdoFractionValue(uint8_t);
uint8_t u8GetOdoFractionValue(void);
void vSetOdoUpdateFlagStatus(uint8_t);
uint8_t bGetOdoUpdateFlagStatus(void);
void vSetOdoFractionUpdateStatus(uint8_t);
uint8_t bGetOdoFractionUpdateStatus(void);

void vSetOdoTripAValue(uint32_t);
uint32_t u32GetOdoTripAValue(void);
void vSetOdoTripAFractionValue(uint8_t);
uint8_t u8GetOdoTripAFractionValue(void);
void vSetOdoTripAPulseCounter(uint16_t);
void vSetOdoTripAUpdateStatus(uint8_t);
uint8_t bGetOdoTripAUpdateStatus(void);
uint8_t bGetOdoTripAFractionUpdateStatus(void);
void vSetOdoTripAFractionUpdateStatus(uint8_t);

void vSetOdoTripBValue(uint32_t);
uint32_t u32GetOdoTripBValue(void);
void vSetOdoTripBFractionValue(uint8_t );
uint8_t u8GetOdoTripBFractionValue(void);
void vSetOdoTripBPulseCounter(uint16_t);
void vSetOdoTripBUpdateStatus(uint8_t);
uint8_t bGetOdoTripBUpdateStatus(void);
uint8_t bGetOdoTripBFractionUpdateStatus(void);
void vSetOdoTripBFractionUpdateStatus(uint8_t);

uint32_t GetDisplayTripBFinalMergedValue(void);
uint32_t GetDisplayTripAFinalMergedValue(void);
uint32_t GetDisplayOdoFinalMergedValue(void);

#ifdef __cplusplus
}
#endif
/// @} // endgroup odometerdataLib

/// @} // endgroup ODO_METER_DATA_LIBRARY

#endif /* INC_ODO_H_ */
