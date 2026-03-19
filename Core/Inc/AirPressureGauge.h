/*
 * AirPressureGauge.h
 *
 *  Created on: Sep 25, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_AIRPRESSUREGAUGE_H_
#define INC_AIRPRESSUREGAUGE_H_

#include "main.h"


#ifdef __cplusplus
extern "C"
{
#endif

uint8_t GetAirPressureBarNo(void);
uint8_t getAirPressure(void);
void AirPressure_Process(void);
uint8_t GetAirPressureWarningBuzzerStatus();

#ifdef __cplusplus
}
#endif

#endif /* INC_AIRPRESSUREGAUGE_H_ */
