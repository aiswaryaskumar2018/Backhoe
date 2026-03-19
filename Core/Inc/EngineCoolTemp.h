/*
 * EngineCoolTemp.h
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_ENGINECOOLTEMP_H_
#define INC_ENGINECOOLTEMP_H_



#include "main.h"


typedef enum
{
	eEngineCoolTempLevel_0_Bars = 0u,
	eEngineCoolTempLevel_1_Bars,
	eEngineCoolTempLevel_2_Bars,
	eEngineCoolTempLevel_3_Bars,
	eEngineCoolTempLevel_4_Bars,
	eEngineCoolTempLevel_5_Bars,
	eEngineCoolTempLevel_6_Bars,
	eEngineCoolTempLevel_7_Bars,
	eEngineCoolTempLevel_8_Bars,
	eTotalCoolTempLevelBars
};

#ifdef __cplusplus
extern "C"
{
#endif

void vEngineCoolTempLevelGauge				(void);
uint8_t GetEngineCoolTempLevelGaugeBars		(void);
void SetEngineCoolTempLevelGaugeBars		(uint8_t BarsValue);
uint8_t GetEngineCoolTempWarningLampStatus	(void);
uint8_t GetEngineCoolTempBuzzerStatus		(void);
uint8_t GetEngineCoolTempValue				(void);
#ifdef __cplusplus
}
#endif


#endif /* INC_ENGINECOOLTEMP_H_ */
