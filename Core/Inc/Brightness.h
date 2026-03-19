/*
 * Brightness.h
 *
 *  Created on: Dec 11, 2024
 *      Author: aiswarya.kumar
 */

#ifndef INC_BRIGHTNESS_H_
#define INC_BRIGHTNESS_H_


#include "main.h"


typedef enum
{
	eBrightnessLevel_0_Bars = 0u,
	eBrightnessLevel_1_Bars,
	eBrightnessLevel_2_Bars,
	eBrightnessLevel_3_Bars,
	eBrightnessLevel_4_Bars,
	eBrightnessLevel_5_Bars,
	eBrightnessLevel_6_Bars,
	eBrightnessLevel_7_Bars,
	eBrightnessLevel_8_Bars,
	eBrightnessLevel_9_Bars,
	eBrightnessLevel_10_Bars,
	eTotalBrightnessLevelBars
};

#ifdef __cplusplus
extern "C"
{
#endif

void vDefLevelGauge					    (void);
uint8_t GetBrightnessLevelBars		    (void);
void TftBrightNessProcess				(void);

//void SetDefLevelGaugePercentage		(uint8_t PercentageValue);
//uint8_t GetDefLevelGaugePercentage	(void);
//void vDefLevelGauge					(void);

#ifdef __cplusplus
}
#endif


#endif /* INC_BRIGHTNESS_H_ */
