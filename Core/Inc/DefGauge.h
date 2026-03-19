/*
 * DefGauge.h
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_DEFGAUGE_H_
#define INC_DEFGAUGE_H_


#include "main.h"

typedef enum
{
	eDefLevel_0_Bars = 0u,
	eDefLevel_1_Bars,
	eDefLevel_2_Bars,
	eDefLevel_3_Bars,
	eDefLevel_4_Bars,
	eDefLevel_5_Bars,
	eDefLevel_6_Bars,
	eDefLevel_7_Bars,
	eDefLevel_8_Bars,
	eDefLevel_9_Bars,
	eDefLevel_10_Bars,
	eTotalDefLevelBars
};

#ifdef __cplusplus
extern "C"
{
#endif

void vDefLevelGauge					(void);
uint8_t GetDefLevelGaugeBars		(void);
void SetDefLevelGaugeBars			(uint8_t BarsValue);
void SetDefLevelGaugePercentage		(uint8_t PercentageValue);
uint8_t GetDefLevelGaugePercentage	(void);
void vDefLevelGauge					(void);

#ifdef __cplusplus
}
#endif


#endif /* INC_DEFGAUGE_H_ */
