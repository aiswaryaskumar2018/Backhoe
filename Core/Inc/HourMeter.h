/*
 * HourMeter.h
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_HOURMETER_H_
#define INC_HOURMETER_H_

#include "main.h"

#define MAX_HOUR_TRIP_A 		99999u
#define MAX_HOUR_TRIP_B 		99999u

#ifdef __cplusplus
extern "C"
{
#endif

void vHourMeterCAN();
uint32_t GetHourDataValue(void);
uint8_t GetHourDataComingOrNotStatus();
uint8_t GetHourSymbolBlinkStatus();
uint32_t GetTripBHourValue(void);
uint32_t GetTripAHourValue(void);
void SetTripBHourValue(uint32_t Val);
void SetTripAHourValue(uint32_t Val);

void SetHourTripBOverFlowFlag(uint8_t Status);
void SetHourTripAOverFlowFlag(uint8_t Status);
uint8_t GetHourTripBOverFlowFlag(void);
uint8_t GetHourTripAOverFlowFlag(void);
#ifdef __cplusplus
}
#endif

#endif /* INC_HOURMETER_H_ */
