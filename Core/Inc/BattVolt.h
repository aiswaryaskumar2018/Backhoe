/*
 * BattVolt.h
 *
 *  Created on: Sep 20, 2024
 *      Author: aiswarya.kumar
 */

#ifndef INC_BATTVOLT_H_
#define INC_BATTVOLT_H_

#define GAUGE_AVG_SIZE 	16
#define GAUGE_AVG_SIZE				16
#define FILTER_VOLTAGE				10     //volt 1.0
#define VOLTAGE_HYS_UP				6      //volt 0.6
#define VOLTAGE_HYS_DOWN			4      //volt 0.4
#define VOLT_VARIATION_TIME 		3000


#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

uint16_t get_battery_volt(void);
void vBattVoltDataProcessing();

#ifdef __cplusplus
}
#endif





#endif /* INC_BATTVOLT_H_ */
