/*
 * TransOilPressreure.h
 *
 *  Created on: Sep 20, 2024
 *      Author: aiswarya.kumar
 */

#ifndef INC_TRANSOILPRESSREURE_H_
#define INC_TRANSOILPRESSREURE_H_
#include "stm32u5xx_hal.h"

#define GAUGE_AVG_SIZE 	16
#define GAUGE_AVG_SIZE				16
#define FILTER_VOLTAGE				10     //volt 1.0
#define VOLTAGE_HYS_UP				6      //volt 0.6
#define VOLTAGE_HYS_DOWN			4      //volt 0.4
#define VOLT_VARIATION_TIME 		3000

#ifdef __cplusplus
extern "C"
{
#endif


uint16_t get_Trans_oil_pressure(void);
uint8_t GetTransOilBar();
uint32_t Trans_TRANS_OIL_pressure_process();
uint8_t GetTrnsOIlPressureLampWarningStatus();
uint8_t GetTrnsOIlPressureBuzzerStatus();

#ifdef __cplusplus
}
#endif



#endif /* INC_TRANSOILPRESSREURE_H_ */
