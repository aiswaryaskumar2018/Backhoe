/*
 * Brightness.c
 *
 *  Created on: Dec 11, 2024
 *      Author: aiswarya.kumar
 */


#include "Brightness.h"
#include "main.h"

//uint8_t value=0;
static uint8_t count=0;

#define brightness_level_1   10
#define brightness_level_2   20
#define brightness_level_3   30
#define brightness_level_4   40
#define brightness_level_5   50
#define brightness_level_6   60
#define brightness_level_7   70
#define brightness_level_8   80
#define brightness_level_9   90
#define brightness_level_10  100

//uint8_t value = GetBrightnessBarStatus();

void TftBrightNessProcess(void)
{
	uint8_t value = eBrightnessLevel_1_Bars;

	value = GetBrightnessBarStatus();

	switch(value)
	{
		case eBrightnessLevel_1_Bars:
				PWMSetValueForTFTBrightness(brightness_level_1);
				count = eBrightnessLevel_1_Bars;
				break;

		case eBrightnessLevel_2_Bars:
				PWMSetValueForTFTBrightness(brightness_level_2);
				count = eBrightnessLevel_2_Bars;
				break;

		case eBrightnessLevel_3_Bars:
				PWMSetValueForTFTBrightness(brightness_level_3);
				count = eBrightnessLevel_3_Bars;
				break;

		case eBrightnessLevel_4_Bars:
				PWMSetValueForTFTBrightness(brightness_level_4);
				count = eBrightnessLevel_4_Bars;
				break;

		case eBrightnessLevel_5_Bars:
				PWMSetValueForTFTBrightness(brightness_level_5);
				count = eBrightnessLevel_5_Bars;
				break;

		case eBrightnessLevel_6_Bars:
				PWMSetValueForTFTBrightness(brightness_level_6);
				count = eBrightnessLevel_6_Bars;
				break;

		case eBrightnessLevel_7_Bars:
				PWMSetValueForTFTBrightness(brightness_level_7);
				count = eBrightnessLevel_7_Bars;
				break;

		case eBrightnessLevel_8_Bars:
				PWMSetValueForTFTBrightness(brightness_level_8);
				count = eBrightnessLevel_8_Bars;
				break;

		case eBrightnessLevel_9_Bars:
				PWMSetValueForTFTBrightness(brightness_level_9);
				count = eBrightnessLevel_9_Bars;
				break;

		case eBrightnessLevel_10_Bars:
				PWMSetValueForTFTBrightness(brightness_level_10);
				count = eBrightnessLevel_10_Bars;
				break;
		default:
				break;

    }
}

uint8_t GetBrightnessLevelBars(void)
{
	return (count * 10);
}
