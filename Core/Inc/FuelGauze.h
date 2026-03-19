/*
 * FuelGauze.h
 *
 *  Created on: Sep 20, 2024
 *      Author: aiswarya.kumar
 */

#ifndef INC_FUELGAUZE_H_
#define INC_FUELGAUZE_H_

#define GAUGE_AVG_SIZE 	16
#define GAUGE_AVG_SIZE				16
#define FILTER_VOLTAGE				10     //volt 1.0
#define VOLTAGE_HYS_UP				6      //volt 0.6
#define VOLTAGE_HYS_DOWN			4      //volt 0.4
#define VOLT_VARIATION_TIME 		3000


#include "stm32u5xx_hal.h"



#ifdef __cplusplus
extern "C"
{
#endif

uint8_t GetFuelCANTxData(void);
uint8_t GetFuelGaugeBar(void);
void fuel_gauge_Process(void);
uint8_t GetFuelWarningBuzzerStatus();

#ifdef __cplusplus
}
#endif




#endif /* INC_FUELGAUZE_H_ */
