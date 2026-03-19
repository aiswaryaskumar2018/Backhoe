/*
 * EngineOilPress.h
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_ENGINEOILPRESS_H_
#define INC_ENGINEOILPRESS_H_


#include "main.h"

typedef enum
{
	eEngineOilLevel_0_Bars = 0u,
	eEngineOilLevel_1_Bars,
	eEngineOilLevel_2_Bars,
	eEngineOilLevel_3_Bars,
	eEngineOilLevel_4_Bars,
	eEngineOilLevel_5_Bars,
	eEngineOilLevel_6_Bars,
	eEngineOilLevel_7_Bars,
	eEngineOilLevel_8_Bars,
	eEngineOilLevel_9_Bars,
	eEngineOilLevel_10_Bars,
	eTotalEngineOilLevelBars
};



#ifdef __cplusplus
extern "C"
{
#endif

void vEngineOilLevelGauge					(void);
uint8_t GetEngineOilLevelGaugeBars			(void);
void SetEngineOilLevelGaugeBars				(uint8_t BarsValue);
void SetEngineOilLevelGaugePercentage		(uint8_t PercentageValue);
uint8_t GetEngineOilLevelGaugePercentage	(void);
void vEngineOilLevelGauge					(void);
uint8_t GetEngineOilPressWarningLampStatus	(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_ENGINEOILPRESS_H_ */
